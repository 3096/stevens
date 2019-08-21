#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "wii_super_smash.h"
using namespace std;

#include "x_namco.h"

#define X_SYSTEM WIIU
#define X_GAME   SUPER_SMASH_BROS

//-----------------------------------------------------------------------------
//                                 SKELETONS
//-----------------------------------------------------------------------------

namespace X_SYSTEM { namespace X_GAME {

bool ExtractVBN_BE(LPCTSTR filename, bool do_delete)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug file
 bool debug = true;
 ofstream dfile;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT("_SKELETON.debug");
    dfile.open(ss.str().c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x56424E20) return error("Not a VBN file.");

 // read header
 uint16 h01 = BE_read_uint16(ifile); // 0x0001 (no matrices) or 0x0002 (matrices)
 uint16 h02 = BE_read_uint16(ifile); // 0x0000 or 0x0001
 uint32 n_T_bones = BE_read_uint32(ifile); // total number of bones
 uint32 n_A_bones = BE_read_uint32(ifile); // number of bones A
 BE_read_uint32(ifile); // ????
 uint32 n_B_bones = BE_read_uint32(ifile); // number of bones B
 BE_read_uint32(ifile); // ????

 // validate header
 if(!n_T_bones) return true;
 uint32 n_joints = n_T_bones;

 // for each bone
 boost::shared_array<uint32> parentlist(new uint32[n_joints]);
 for(uint32 i = 0; i < n_joints; i++)
    {
     // string: 0x40 bytes (name)
     char name[0x40];
     ifile.read(&name[0], 0x40);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // uint32: 0x04 bytes (part)
     uint32 part = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // uint32: 0x04 bytes (parent)
     auto p1 = BE_read_uint16(ifile);
     auto p2 = BE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
     parentlist[i] = (p1 == 0xFFFF ? AMC_INVALID_JOINT : p1);

     // uint32: 0x04 bytes (unknown)
     uint32 unknown = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
    }

 // read joint data
 if(debug) dfile << "JOINT DATA" << endl;
 typedef cs::math::vector3D<real32> vector_t;
 boost::shared_array<vector_t> coords1(new vector_t[n_joints]);
 boost::shared_array<vector_t> coords2(new vector_t[n_joints]);
 boost::shared_array<vector_t> coords3(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++)
    {
     // 1st three floats
     coords1[i][0] = BE_read_real32(ifile);
     coords1[i][1] = BE_read_real32(ifile);
     coords1[i][2] = BE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // 2nd three floats
     coords2[i][0] = BE_read_real32(ifile);
     coords2[i][1] = BE_read_real32(ifile);
     coords2[i][2] = BE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // 3rd three floats
     coords3[i][0] = BE_read_real32(ifile);
     coords3[i][1] = BE_read_real32(ifile);
     coords3[i][2] = BE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     if(debug) dfile << "1st set " << i << ": " << coords1[i][0] << ", " << coords1[i][1] << ", " << coords1[i][2] << endl;
     if(debug) dfile << "2nd set " << i << ": " << coords2[i][0] << ", " << coords2[i][1] << ", " << coords2[i][2] << endl;
     if(debug) dfile << "3nd set " << i << ": " << coords3[i][0] << ", " << coords3[i][1] << ", " << coords3[i][2] << endl;
    }
 if(debug) dfile << endl;

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = Unicode16ToUnicode08(shrtname.c_str()).get();
 skel.name += "_SKELETON";
 skel.tiplen = 1.0f;

 // for each joint
 for(uint32 i = 0; i < n_joints; i++)
    {
     // convert euler to quaternion
     // note: angles are in reverse order in the file!
     cs::math::quaternion<real32> q;
     cs::math::rotation_quaternion_ZYX(&q[0], coords2[i][2], coords2[i][1], coords2[i][0]);
     cs::math::normalize(q);

     // convert quaternion to matrix
     cs::math::matrix4x4<real32> m;
     cs::math::quaternion_to_matrix4x4(&q[0], &m[0]);

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;

     // create joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = parentlist[i];
     joint.m_rel[0x0] = m[0x0];
     joint.m_rel[0x1] = m[0x1];
     joint.m_rel[0x2] = m[0x2];
     joint.m_rel[0x3] = m[0x3];
     joint.m_rel[0x4] = m[0x4];
     joint.m_rel[0x5] = m[0x5];
     joint.m_rel[0x6] = m[0x6];
     joint.m_rel[0x7] = m[0x7];
     joint.m_rel[0x8] = m[0x8];
     joint.m_rel[0x9] = m[0x9];
     joint.m_rel[0xA] = m[0xA];
     joint.m_rel[0xB] = m[0xB];
     joint.m_rel[0xC] = m[0xC];
     joint.m_rel[0xD] = m[0xD];
     joint.m_rel[0xE] = m[0xE];
     joint.m_rel[0xF] = m[0xF];
     joint.p_rel[0] = coords1[i][0];
     joint.p_rel[1] = coords1[i][1];
     joint.p_rel[2] = coords1[i][2];
     joint.p_rel[3] = 1.0f;

     // insert joint
     skel.joints.push_back(joint);
    }

 // create model container
 ADVANCEDMODELCONTAINER amc;
 amc.skllist2.push_back(skel);

 STDSTRINGSTREAM ss;
 ss << shrtname << TEXT("_SKELETON");
 SaveAMC(pathname.c_str(), ss.str().c_str(), amc);
 return true;
}

bool ExtractVBN_LE(LPCTSTR filename, bool do_delete)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug file
 bool debug = true;
 ofstream dfile;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT("_SKELETON.debug");
    dfile.open(ss.str().c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // read magic
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x56424E20) return error("Not a VBN file.");

 // read header
 uint16 h01 = LE_read_uint16(ifile); // 0x0001 (no matrices) or 0x0002 (matrices)
 uint16 h02 = LE_read_uint16(ifile); // 0x0000 or 0x0001
 uint32 n_T_bones = LE_read_uint32(ifile); // total number of bones
 uint32 n_A_bones = LE_read_uint32(ifile); // number of bones A
 LE_read_uint32(ifile); // ????
 uint32 n_B_bones = LE_read_uint32(ifile); // number of bones B
 LE_read_uint32(ifile); // ????

 // validate header
 if(!n_T_bones) return true;
 uint32 n_joints = n_T_bones;

 // for each bone
 boost::shared_array<uint32> parentlist(new uint32[n_joints]);
 for(uint32 i = 0; i < n_joints; i++)
    {
     // string: 0x40 bytes (name)
     char name[0x40];
     ifile.read(&name[0], 0x40);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // uint32: 0x04 bytes (part)
     uint32 part = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // uint32: 0x04 bytes (parent)
     auto p1 = LE_read_uint16(ifile);
     auto p2 = LE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
     parentlist[i] = (p1 == 0xFFFF ? AMC_INVALID_JOINT : p1);

     // uint32: 0x04 bytes (unknown)
     uint32 unknown = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
    }

 // read joint data
 if(debug) dfile << "JOINT DATA" << endl;
 typedef cs::math::vector3D<real32> vector_t;
 boost::shared_array<vector_t> coords1(new vector_t[n_joints]);
 boost::shared_array<vector_t> coords2(new vector_t[n_joints]);
 boost::shared_array<vector_t> coords3(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++)
    {
     // 1st three floats
     coords1[i][0] = LE_read_real32(ifile);
     coords1[i][1] = LE_read_real32(ifile);
     coords1[i][2] = LE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // 2nd three floats
     coords2[i][0] = LE_read_real32(ifile);
     coords2[i][1] = LE_read_real32(ifile);
     coords2[i][2] = LE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // 3rd three floats
     coords3[i][0] = LE_read_real32(ifile);
     coords3[i][1] = LE_read_real32(ifile);
     coords3[i][2] = LE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     if(debug) dfile << "1st set " << i << ": " << coords1[i][0] << ", " << coords1[i][1] << ", " << coords1[i][2] << endl;
     if(debug) dfile << "2nd set " << i << ": " << coords2[i][0] << ", " << coords2[i][1] << ", " << coords2[i][2] << endl;
     if(debug) dfile << "3nd set " << i << ": " << coords3[i][0] << ", " << coords3[i][1] << ", " << coords3[i][2] << endl;
    }
 if(debug) dfile << endl;

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = Unicode16ToUnicode08(shrtname.c_str()).get();
 skel.name += "_SKELETON";
 skel.tiplen = 1.0f;

 // for each joint
 for(uint32 i = 0; i < n_joints; i++)
    {
     // convert euler to quaternion
     // note: angles are in reverse order in the file!
     cs::math::quaternion<real32> q;
     cs::math::rotation_quaternion_ZYX(&q[0], coords2[i][2], coords2[i][1], coords2[i][0]);
     cs::math::normalize(q);

     // convert quaternion to matrix
     cs::math::matrix4x4<real32> m;
     cs::math::quaternion_to_matrix4x4(&q[0], &m[0]);

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;

     // create joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = parentlist[i];
     joint.m_rel[0x0] = m[0x0];
     joint.m_rel[0x1] = m[0x1];
     joint.m_rel[0x2] = m[0x2];
     joint.m_rel[0x3] = m[0x3];
     joint.m_rel[0x4] = m[0x4];
     joint.m_rel[0x5] = m[0x5];
     joint.m_rel[0x6] = m[0x6];
     joint.m_rel[0x7] = m[0x7];
     joint.m_rel[0x8] = m[0x8];
     joint.m_rel[0x9] = m[0x9];
     joint.m_rel[0xA] = m[0xA];
     joint.m_rel[0xB] = m[0xB];
     joint.m_rel[0xC] = m[0xC];
     joint.m_rel[0xD] = m[0xD];
     joint.m_rel[0xE] = m[0xE];
     joint.m_rel[0xF] = m[0xF];
     joint.p_rel[0] = coords1[i][0];
     joint.p_rel[1] = coords1[i][1];
     joint.p_rel[2] = coords1[i][2];
     joint.p_rel[3] = 1.0f;

     // insert joint
     skel.joints.push_back(joint);
    }

 // create model container
 ADVANCEDMODELCONTAINER amc;
 amc.skllist2.push_back(skel);

 STDSTRINGSTREAM ss;
 ss << shrtname << TEXT("_SKELETON");
 SaveAMC(pathname.c_str(), ss.str().c_str(), amc);
 return true;
}

bool ExtractVBN(LPCTSTR filename, bool do_delete)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read magic
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // close file
 ifile.close();

 // load correct format
 if(magic == 0x56424E20) return ExtractVBN_LE(filename, do_delete);
 if(magic == 0x204E4256) return ExtractVBN_BE(filename, do_delete);
 return error("Not a VBN file.");
}

bool PathExtractVBN(LPCTSTR pathname, bool do_delete, uint32 start)
{
 // set pathname
 STDSTRING pname(pathname);
 if(!pname.length()) pname = GetModulePathname().get();

 // build filename list
 cout << "Searching for .VBN files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".VBN"), pname.c_str())) return true;
 
 // process NTP3 files
 bool break_on_errors = true;
 cout << "Processing .VBN files..." << endl;
 for(size_t i = start; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractVBN(filelist[i].c_str(), do_delete)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

bool PathExtractVBN(bool do_delete, uint32 start)
{
 return PathExtractVBN(NULL, do_delete, start);
}

}}

//-----------------------------------------------------------------------------
//                                 FUNCTIONS
//-----------------------------------------------------------------------------

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[WII-U] Super Smash Bros");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Use other tools (not provided) to decrypt.\n");
 p2 += TEXT("3. Use other tools (not provided) to unpack.\n");
 p2 += TEXT("4. Use other tools (not provided) for textures.\n");
 p2 += TEXT("5. Run ripper.\n");
 p2 += TEXT("6. Select game and click Browse.\n");
 p2 += TEXT("7. Select folder where content directory is.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~36 GB free space.\n");
 p3 += TEXT("3. 24 GB for game + 12 GB extraction.\n");

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
 bool doNUD = true;
 bool doVBN = true;

 // questions
 if(doNUD) doNUD = YesNoBox("Process model (NUD) files?");
 if(doVBN) doVBN = YesNoBox("Process skeleton (VBN) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // model extraction
 if(doNUD)
    NAMCO::PathExtractNDP3(pathname.c_str(), nullptr, false, 0, TEXT("NUD"));

 // skeleton extraction
 if(doVBN)
    PathExtractVBN(pathname.c_str(), false, 0);

 // skeleton extraction

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