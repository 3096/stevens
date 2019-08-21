#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_capcom.h"
#include "ps3_ddda.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   DRAGONS_DOGMA_DARK_ARISEN

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

struct MODHEADER {
 uint32 magic;       // 0x00
 uint16 signature;   // 0x04
 uint16 n_joints;    // 0x06
 uint16 n_meshes;    // 0x08
 uint16 n_materials; // 0x0A
 uint32 n_vertices;  // 0x0C
 uint32 n_indices;   // 0x10
 uint32 n_triangles; // 0x14
 uint32 vbufsize;    // 0x18
 uint32 unk01;       // 0x1C 0x00000000
 uint32 unk02;       // 0x20 number of 0x20-byte entries
 uint32 jntsoffset;  // 0x24 offset (to joint data)
 uint32 unk04;       // 0x28 offset (to 0x20-byte entries)
 uint32 unk05;       // 0x2C offset (to material names n_materials * 0x80 bytes)
 uint32 meshoffset;  // 0x30 offset (to mesh information)
 uint32 vbufoffset;  // 0x34 offset (to vbuffer data)
 uint32 ibufoffset;  // 0x38 offset (to ibuffer data)
 uint32 eofmoffset;  // 0x3C offset (to EOF marker)
 real32 sphere_cx;   // 0x40
 real32 sphere_cy;   // 0x44
 real32 sphere_cz;   // 0x48
 real32 sphere_cr;   // 0x4C
 real32 bbox_min_x;  // 0x50
 real32 bbox_min_y;  // 0x54
 real32 bbox_min_z;  // 0x58
 real32 bbox_min_w;  // 0x5C
 real32 bbox_max_x;  // 0x60
 real32 bbox_max_y;  // 0x64
 real32 bbox_max_z;  // 0x68
 real32 bbox_max_w;  // 0x6C
 uint32 unk07;       // 0x70
 uint32 unk08;       // 0x74
 uint32 unk09;       // 0x78
 uint32 unk10;       // 0x7C
 uint32 unk11;       // 0x80
};

struct MODJOINTINFO {
 uint08 j01; // jntmap index
 uint08 j02; // parent index
 uint08 j03; // index of mirrored joint (if present)
 uint08 j04; // 0x00
 real32 j05; // unknown floating-point values
 real32 j06; // unknown floating-point values
 real32 j07; // unknown floating-point values
 real32 j08; // unknown floating-point values
 real32 j09; // unknown floating-point values
};

struct MODJOINTDATA {
 boost::shared_array<MODJOINTINFO> jntlist;
 boost::shared_array<boost::shared_array<real32>> mxlist1;
 boost::shared_array<boost::shared_array<real32>> mxlist2;
};

struct MODELDATA {
 public :
  bool debug;
  STDSTRING filename;
  STDSTRING pathname;
  STDSTRING shrtname;
  std::ifstream ifile;
  std::ofstream dfile;
 public :
  MODHEADER header;
  MODJOINTDATA jntdata;
 public :
  ADVANCEDMODELCONTAINER amc;
};

bool processMeshInfo(MODELDATA& md);

bool ExtractMOD(LPCTSTR filename, bool delete_file = false)
{
 // set debug mode and file properties
 MODELDATA md;
 md.debug = false;
 md.filename = filename;
 md.pathname = GetPathnameFromFilename(filename).get();
 md.shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open file
 md.ifile.open(filename, ios::binary);
 if(!md.ifile) return error("Failed to open file.");

 // create debug file
 if(md.debug) {
    STDSTRING fn = md.pathname;
    fn += md.shrtname;
    fn += TEXT(".txt");
    md.dfile.open(fn.c_str());
    if(!md.dfile) return error("Failed to create debug file.");
   }

 // read magic
 md.header.magic = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");
 if(md.header.magic != 0x444F4D) return error("Invalid magic number.");

 // read signature (0x2CFF)
 md.header.signature = BE_read_uint16(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // validate signature
 if(md.header.signature != 0x00D4) // DRAGON'S DOGMA: DARK ARISEN
    return error("Game is not Dragon's Dogma: Dark Arisen.");

 // read number of joints
 md.header.n_joints = BE_read_uint16(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read number of meshes
 md.header.n_meshes = BE_read_uint16(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read number of materials
 md.header.n_materials = BE_read_uint16(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read number of vertices
 md.header.n_vertices = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read number of indices
 md.header.n_indices = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read number of triangles excluding non-planar edge geometry
 md.header.n_triangles = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read total vertex buffer sizes in bytes
 md.header.vbufsize = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read unknown (always 0x00000000)
 md.header.unk01 = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");
 if(md.header.unk01 != 0) return error("Expecting 0x00000000.");

 // read unknown (RE6: number of 0x20-byte entries)
 md.header.unk02 = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read offset to joint data
 md.header.jntsoffset = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read unknown (RE6: offset to joint data that contains 0x18 byte items + 0x40 byte matrices)
 md.header.unk04 = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read unknown (RE6: offset to 0x20-byte entries)
 md.header.unk05 = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read offset (to mesh information)
 md.header.meshoffset = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read offset (to vertex buffer data)
 md.header.vbufoffset = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read offset (to index buffer data)
 md.header.ibufoffset = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read offset (to end-of-file-marker)
 md.header.eofmoffset = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read bounding sphere
 md.header.sphere_cx = BE_read_real32(md.ifile);
 md.header.sphere_cy = BE_read_real32(md.ifile);
 md.header.sphere_cz = BE_read_real32(md.ifile);
 md.header.sphere_cr = BE_read_real32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read bounding box
 md.header.bbox_min_x = BE_read_real32(md.ifile);
 md.header.bbox_min_y = BE_read_real32(md.ifile);
 md.header.bbox_min_z = BE_read_real32(md.ifile);
 md.header.bbox_min_w = BE_read_real32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read bounding box
 md.header.bbox_max_x = BE_read_real32(md.ifile);
 md.header.bbox_max_y = BE_read_real32(md.ifile);
 md.header.bbox_max_z = BE_read_real32(md.ifile);
 md.header.bbox_max_w = BE_read_real32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // read unknowns
 md.header.unk07 = BE_read_uint32(md.ifile);
 md.header.unk08 = BE_read_uint32(md.ifile);
 md.header.unk09 = BE_read_uint32(md.ifile);
 md.header.unk10 = BE_read_uint32(md.ifile);
 md.header.unk11 = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // move to joint data
 md.ifile.seekg(md.header.jntsoffset);
 if(md.ifile.fail()) return error("Seek failure.");

 // allocate joint data
 MODJOINTDATA& jd = md.jntdata;
 jd.jntlist.reset(new MODJOINTINFO[md.header.n_joints]);
 jd.mxlist1.reset(new boost::shared_array<real32>[md.header.n_joints]);
 jd.mxlist2.reset(new boost::shared_array<real32>[md.header.n_joints]);

 // read joints info
 for(uint32 i = 0; i < md.header.n_joints; i++) {
     jd.jntlist[i].j01 = BE_read_uint08(md.ifile);
     jd.jntlist[i].j02 = BE_read_uint08(md.ifile);
     jd.jntlist[i].j03 = BE_read_uint08(md.ifile);
     jd.jntlist[i].j04 = BE_read_uint08(md.ifile);
     jd.jntlist[i].j05 = BE_read_real32(md.ifile);
     jd.jntlist[i].j06 = BE_read_real32(md.ifile);
     jd.jntlist[i].j07 = BE_read_real32(md.ifile);
     jd.jntlist[i].j08 = BE_read_real32(md.ifile);
     jd.jntlist[i].j09 = BE_read_real32(md.ifile);
     if(md.ifile.fail()) return error("Read failure.");
    }

 // read joints matrix set 1
 for(uint32 i = 0; i < md.header.n_joints; i++) {
     jd.mxlist1[i].reset(new real32[16]);
     if(!BE_read_array(md.ifile, jd.mxlist1[i].get(), 16)) return error("Read failure.");
    }

 // read joints matrix set 2
 for(uint32 i = 0; i < md.header.n_joints; i++) {
     jd.mxlist2[i].reset(new real32[16]);
     if(!BE_read_array(md.ifile, jd.mxlist2[i].get(), 16)) return error("Read failure.");
    }

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS | AMC_JOINT_FORMAT_TIP_LENGTH;
 skel.tiplen = 0.1f;
 skel.name = "skeleton";

 // process 0x40 matrices
 for(uint32 i = 0; i < md.header.n_joints; i++)
    {
     // copy the two transformation matrices
     cs::math::matrix4x4<cs::binary32> m1(md.jntdata.mxlist1[i].get());
     cs::math::matrix4x4<cs::binary32> m2(md.jntdata.mxlist2[i].get());
     m1.transpose();
     m2.transpose();

     // extract matrix parmaters
     cs::binary32 scale_x = m1(0,0)/m2(0,0);
     cs::binary32 scale_y = m1(1,1)/m2(1,1);
     cs::binary32 scale_z = m1(2,2)/m2(2,2);
     cs::binary32 trans_x = m2(0,3);
     cs::binary32 trans_y = m2(1,3);
     cs::binary32 trans_z = m2(2,3);

     // m2 * T = m1
     // three equations and three unknowns can be solved for by cramer's rule
     // [r00 r01 r02 0] [ 1  0  0 tx] = [m00 m01 m02 m03]
     // [r10 r11 r12 0] [ 0  1  0 ty] = [m10 m11 m12 m13]
     // [r20 r21 r22 0] [ 0  0  1 tz] = [m20 m21 m22 m23]
     // [  0   0   0 1] [ 0  0  0  1] = [m30 m31 m32 m33]

     // find joint position in absolute coordinates using cramer's rule
     cs::binary32 A[9] = {
      m2(0,0), m2(0,1), m2(0,2),
      m2(1,0), m2(1,1), m2(1,2),
      m2(2,0), m2(2,1), m2(2,2),
     };
     cs::binary32 b[3] = {
      -m2(0,3),
      -m2(1,3),
      -m2(2,3),
     };
     cs::binary32 x[3] = { 0.0f, 0.0f, 0.0f };
     cs::math::cramer_3x3(A, b, x);

     // create joint name
     stringstream ss;
     ss << "joint_" << setfill('0') << setw(3) << i;

     // create and insert joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = (jd.jntlist[i].j02 == 0xFF ? AMC_INVALID_JOINT : jd.jntlist[i].j02);
     joint.m_rel[0x0] = m1[0x0];
     joint.m_rel[0x1] = m1[0x1];
     joint.m_rel[0x2] = m1[0x2];
     joint.m_rel[0x3] = m1[0x3];
     joint.m_rel[0x4] = m1[0x4];
     joint.m_rel[0x5] = m1[0x5];
     joint.m_rel[0x6] = m1[0x6];
     joint.m_rel[0x7] = m1[0x7];
     joint.m_rel[0x8] = m1[0x8];
     joint.m_rel[0x9] = m1[0x9];
     joint.m_rel[0xA] = m1[0xA];
     joint.m_rel[0xB] = m1[0xB];
     joint.m_rel[0xC] = m1[0xC];
     joint.m_rel[0xD] = m1[0xD];
     joint.m_rel[0xE] = m1[0xE];
     joint.m_rel[0xF] = m1[0xF];
     joint.p_rel[0] = x[0]; // m2(0,3)*scale_x; // scale_x*m1(0,3); // mat1[i][0x3];
     joint.p_rel[1] = x[1]; // m2(1,3)*scale_y; // scale_y*m1(1,3); // mat1[i][0x7];
     joint.p_rel[2] = x[2]; // m2(2,3)*scale_z; // scale_z*m1(2,3); // mat1[i][0xB];
     joint.p_rel[3] = 1.0f;

     // insert joint
     skel.joints.push_back(joint);
    }

 // insert skeleton
 md.amc.skllist2.push_back(skel);

 //
 if(!processMeshInfo(md)) return false;

 // transform and save AMC file
 ADVANCEDMODELCONTAINER transformed = TransformAMC(md.amc);
 SaveAMC(md.pathname.c_str(), md.shrtname.c_str(), transformed);
 SaveOBJ(md.pathname.c_str(), md.shrtname.c_str(), transformed);
 return true;
}

bool processMeshInfo(MODELDATA& md)
{
 // debug
 if(md.debug) {
    md.dfile << "---------" << endl;
    md.dfile << "MESH DATA" << endl;
    md.dfile << "---------" << endl;
    md.dfile << endl;
   }

 // contains no meshes
 if(!md.header.n_meshes || !md.header.meshoffset)
    return true;

 // move to offset
 md.ifile.seekg(md.header.meshoffset);
 if(md.ifile.fail()) return error("Seek failure.");

 // read mesh information (0x38/56 bytes per entry)
 uint32 size = md.header.n_meshes * 0x38;
 boost::shared_array<char> data(new char[size]);
 md.ifile.read(data.get(), size);
 if(md.ifile.fail()) return error("Read failure.");

 // save mesh information (DEBUG MODE ONLY)
 if(md.debug) {
    STDSTRING fname = ChangeFileExtension(md.filename.c_str(), TEXT("mbuffer")).get();
    ofstream temp(fname.c_str(), ios::binary);
    if(!temp) return error("Failed to create MBUFFER file.");
    temp.write(data.get(), size);
    if(temp.fail()) return error("Write failure.");
   }

 // NOTE:
 // After the mesh information data, there are a bunch of 0x90-byte entries.
 // These entries were not present in previous Resident Evil games. Each one of
 // these entries begins with a 4-byte ID that is always less than the number of
 // joints in the model. Not sure what this is, but let's load it anyways.

 // compute size of section from offsets
 uint32 section_size = md.header.vbufoffset - md.header.meshoffset;
 section_size -= size;

 // validate section size
 // must be zero multiple of 0x90
 if(md.header.unk02 != 0x01) message( "Expecting 0x01, but got 0x02.");
 if(section_size && (section_size % 0x90))
    message(" Unknown mesh information data size is not divisible by 0x90.");

 // success
 if(md.debug) md.dfile << endl;
 return true;
}

bool ExtractMODFromPath(LPCTSTR pathname, bool delete_file = false)
{
 // build filename list
 cout << "Searching for .MOD files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".MOD"), pathname)) return true;

 // process MOD files
 bool break_on_errors = true;
 cout << "Processing .MOD files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractMOD(filelist[i].c_str(), delete_file)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

}}

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PS3] Dragon's Dogma: Dark Arisen");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select game root directory.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~??? GB free space.\n");
 p3 += TEXT("3. ??? GB for game + ??? GB extraction.\n");
 p3 += TEXT("4. For DLC, ???.");

 // insert title
 AddGameTitle(p1.c_str(), p2.c_str(), p3.c_str(), extract);
 return true;
}

bool extract(void)
{
 LPCTSTR pathname = GetModulePathname().get();
 return extract(pathname);
}

bool extract(LPCTSTR pname)
{
 // set pathname
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // booleans
 bool doARC = false;
 bool doTEX = false;
 bool doMOD = true;

 // questions
 // if(doARC) doARC = YesNoBox("Process archive (ARC) files?\nSay 'No' if you have already done this before.");
 // if(doTEX) doTEX = YesNoBox("Process texture (TEX) files?\nSay 'No' if you have already done this before.");
 // if(doMOD) doMOD = YesNoBox("Process model (MOD) files?\nSay 'No' if you have already done this before.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process ARC files
 if(doARC) CAPCOM::PS3::ExtractARCFromPath(pathname.c_str());

 // process TEX files
 if(doTEX) CAPCOM::PS3::ExtractTEXFromPath(pathname.c_str());

 // process MOD files
 if(doMOD) ExtractMODFromPath(pathname.c_str());

 // record end time
 uint64 t1 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t1);

 // determine time
 uint64 hz;
 QueryPerformanceFrequency((LARGE_INTEGER*)&hz);
 uint64 dt = (t1 - t0);
 real64 dt_sec = (real64)(t1 - t0)/(real64)hz;
 real64 dt_min = dt_sec/60.0;
 real64 dt_hrs = dt_min/60.0;
 cout << "Elapsed time = " << dt_min << " minutes." << endl;

 return true;
}

}};