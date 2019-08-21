#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "pc_alan_wake.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   ALAN_WAKE

//
// PROMOTE TO XENTAX.H
//
bool GetFileSizeFromFileStream(std::ifstream& ifile, size_t& filesize)
{
 // not open
 filesize = 0;
 if(!ifile.is_open()) return false;

 // save position
 size_t position = (size_t)ifile.tellg();
 if(ifile.fail()) return false;

 // save filesize
 ifile.seekg(0, std::ios::end);
 if(ifile.fail()) return false;
 size_t value = ifile.tellg();
 ifile.seekg(0, std::ios::beg);
 if(ifile.fail()) return false;

 // restore position
 ifile.seekg(position);
 if(ifile.fail()) return false;

 // set filesize
 filesize = value;
 return true;
}

// archives
// ep999-000.bin
// 0x95 - table of 0x1C-byte entries
// 0x5C55 - 0x16EC6C item table (0x169018 bytes)
// 0x16EC6C - EOF filename table (0xE42C9 bytes)

// 00 00 03 47 = number of 0x1C byte entries starting at 0x95?
// 00 00 90 67 = number of 0x28 byte entries
// 00 0E 42 C9 = size of string table

// look at ep999-004.bin there is only one file

bool ProcessBinMsh(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = LE_read_uint32(ifile); // 0x13
 uint32 h02 = LE_read_uint32(ifile); // size of vertex buffer
 uint32 h03 = LE_read_uint32(ifile); // number of indices
 uint32 h04 = LE_read_uint32(ifile); // 0x02 (bytes per index?)
 uint32 h05 = LE_read_uint32(ifile); // 0x00
 if(ifile.fail()) return error("Read failure.");

 // read vertex buffer
 boost::shared_array<char> vbdata(new char[h02]);
 ifile.read(vbdata.get(), h02);
 if(ifile.fail()) return error("Read failure.");

 // read index buffer
 uint32 ibsize = h03*h04;
 boost::shared_array<char> ibdata(new char[ibsize]);
 ifile.read(ibdata.get(), ibsize);
 if(ifile.fail()) return error("Read failure.");

 //
 // SKELETON DATA
 //

 struct AWJOINTINFO {
  uint32 namelen;
  char name[1024];
  real32 m[16];
 };
 deque<AWJOINTINFO> jointlist;

 // read number of joints
 uint32 n_joints = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read joints
 for(uint32 i = 0; i < n_joints; i++)
    {
     AWJOINTINFO info;

     // read joint name length
     info.namelen = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read name
     if(!read_string(ifile, &info.name[0], info.namelen))
        return error("Read failure.");

     // read matrix
     if(!LE_read_array(ifile, &info.m[0], 16))
        return error("Read failure.");

     // transpose matrix
     cs::math::matrix4x4<real32> src(&info.m[0]);
     cs::math::matrix4x4<real32> dst;
     cs::math::transpose(src, dst);
     for(uint32 j = 0; j < 16; j++) info.m[j] = dst[j];

     jointlist.push_back(info);
    }

 // skip 0x28 bytes (bounding box?)
 ifile.seekg(0x28, ios::cur);
 if(ifile.fail()) return error("Seek failure.");

 // read unknown
 uint32 unk01 = LE_read_uint32(ifile); // 0x01 or 0x02
 if(ifile.fail()) return error("Read failure.");

 // read number of RFX
 uint32 n_rfx = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read RFX entries
 for(uint32 i = 0; i < n_rfx; i++)
    {
     // read name length
     uint32 rfxnamelen = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read name
     char rfxname[255];
     if(!read_string(ifile, &rfxname[0], rfxnamelen))
        return error("Read failure.");

     // read unknowns (0x10 bytes)
     LE_read_uint32(ifile);
     LE_read_uint32(ifile);
     LE_read_uint32(ifile);
     LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read number of variables
     uint32 n_vars = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read variables
     for(uint32 j = 0; j < n_vars; j++)
        {
         // read name length
         uint32 vnamelen = LE_read_uint32(ifile);
         if(ifile.fail()) return error("Read failure.");
    
         // read name
         char vname[255];
         if(!read_string(ifile, &vname[0], vnamelen))
            return error("Read failure.");

         // process variables
         if(strcmp(vname, "g_sColorMap") == 0)
           {
            // read unknown
            LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read filename length
            uint32 fnamelen = LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read name
            char fname[255];
            if(!read_string(ifile, &fname[0], fnamelen))
               return error("Read failure.");
           }
         else if(strcmp(vname, "g_sNormalMap") == 0)
           {
            // read unknown
            LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read filename length
            uint32 fnamelen = LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read name
            char fname[255];
            if(!read_string(ifile, &fname[0], fnamelen))
               return error("Read failure.");
           }
         else if(strcmp(vname, "g_sDetailMap") == 0)
           {
            // read unknown
            LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read filename length
            uint32 fnamelen = LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read name
            char fname[255];
            if(!read_string(ifile, &fname[0], fnamelen))
               return error("Read failure.");
           }
         else if(strcmp(vname, "g_sDetailNormalMap") == 0)
           {
            // read unknown
            LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read filename length
            uint32 fnamelen = LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read name
            char fname[255];
            if(!read_string(ifile, &fname[0], fnamelen))
               return error("Read failure.");
           }
         else if(strcmp(vname, "g_sSpecularMap") == 0)
           {
            // read unknown
            LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read filename length
            uint32 fnamelen = LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read name
            char fname[255];
            if(!read_string(ifile, &fname[0], fnamelen))
               return error("Read failure.");
           }
         else if(strcmp(vname, "g_sAlphaTestSampler") == 0)
           {
            // read unknown
            LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read filename length
            uint32 fnamelen = LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read name
            char fname[255];
            if(!read_string(ifile, &fname[0], fnamelen))
               return error("Read failure.");
           }
         else if(strcmp(vname, "g_sReflectionMask") == 0)
           {
            // read unknown
            LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read filename length
            uint32 fnamelen = LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read name
            char fname[255];
            if(!read_string(ifile, &fname[0], fnamelen))
               return error("Read failure.");
           }
         else if(strcmp(vname, "g_sReflectionMap") == 0)
           {
            // read unknown
            LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read filename length
            uint32 fnamelen = LE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // read name
            char fname[255];
            if(!read_string(ifile, &fname[0], fnamelen))
               return error("Read failure.");
           }
         else if(strcmp(vname, "g_vColorMultiplier") == 0)
           {
            ifile.seekg(0x14, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(strcmp(vname, "g_vSubsurfaceScattering") == 0)
           {
            ifile.seekg(0x10, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(strcmp(vname, "g_fDetailRepeat") == 0)
           {
            ifile.seekg(0x08, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(strcmp(vname, "g_vSpecularMultiplier") == 0)
           {
            ifile.seekg(0x10, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(strcmp(vname, "g_fGlossiness") == 0)
           {
            ifile.seekg(0x08, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(strcmp(vname, "") == 0)
           {
           }
         else if(strcmp(vname, "") == 0)
           {
           }
         else {
            stringstream ss;
            ss << "Unknown variable name " << vname << ".";
            return error(ss);
           }
        }
    }

 //
 // MESH INFORMATION
 //
 struct MESHINFO {
  uint32 p01; // material ID
  uint32 p02; // number of vertices
  uint32 p03; // number of triangles
  uint32 p04; // vertex buffer start offset
  uint32 p05; // index  buffer start index
  uint32 p06; // vertex buffer format?
  uint16 p07; // vertex buffer format?
  deque<uint32> p08; // joint map
 };
 deque<MESHINFO> meshlist;

 // number of meshes
 uint32 n_mesh = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read mesh information
 for(uint32 i = 0; i < n_mesh; i++)
    {
     // part #1
     MESHINFO info;
     info.p01 = LE_read_uint32(ifile);
     info.p02 = LE_read_uint32(ifile);
     info.p03 = LE_read_uint32(ifile);
     info.p04 = LE_read_uint32(ifile);
     info.p05 = LE_read_uint32(ifile);
     info.p06 = LE_read_uint32(ifile);
     info.p07 = LE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.");

     // no joint map
     if(info.p07 == 0x03)
       {
        ifile.seekg(0x0C, ios::cur);
        if(ifile.fail()) return error("Seek failure.");
       }
     // joint map
     else if(info.p07 == 0x05)
       {
        // skip data
        ifile.seekg(0x0E, ios::cur);
        if(ifile.fail()) return error("Seek failure.");

        // read joint map
        uint32 n_items = LE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");

        // read joint map
        for(uint32 j = 0; j < n_items; j++) {
            uint32 item = LE_read_uint08(ifile);
            if(ifile.fail()) return error("Read failure.");
            info.p08.push_back(item);
           }
       }
     // joint map
     else if(info.p07 == 0x06)
       {
        // skip data
        ifile.seekg(0x11, ios::cur);
        if(ifile.fail()) return error("Seek failure.");

        // read joint map
        uint32 n_items = LE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");

        // read joint map
        for(uint32 j = 0; j < n_items; j++) {
            uint32 item = LE_read_uint08(ifile);
            if(ifile.fail()) return error("Read failure.");
            info.p08.push_back(item);
           }
       }

     // insert mesh information
     meshlist.push_back(info);
    }

 // model container
 ADVANCEDMODELCONTAINER amc;

 //
 // VERTEX BUFFER CONVERSION
 //

 // process mesh information
 binary_stream bs(vbdata, h02);
 for(uint32 i = 0; i < meshlist.size(); i++)
    {
     // move to vertex buffer
     bs.seek(meshlist[i].p04);
     if(bs.fail()) return error("Stream seek failure.");

     // prepare vertex buffer
     AMC_VTXBUFFER vb;
     vb.name = "default";
     vb.elem = meshlist[i].p02;
     vb.data.reset(new AMC_VERTEX[vb.elem]);
     vb.flags = AMC_VERTEX_POSITION;
     vb.uvchan = 0;
     vb.uvtype[0] = AMC_INVALID_MAP;
     vb.uvtype[1] = AMC_INVALID_MAP;
     vb.uvtype[2] = AMC_INVALID_MAP;
     vb.uvtype[3] = AMC_INVALID_MAP;
     vb.uvtype[4] = AMC_INVALID_MAP;
     vb.uvtype[5] = AMC_INVALID_MAP;
     vb.uvtype[6] = AMC_INVALID_MAP;
     vb.uvtype[7] = AMC_INVALID_MAP;
     vb.colors = 0;

     // adjust flags
     if(meshlist[i].p07 == 0x03) {
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.flags |= AMC_VERTEX_UV;
       }
     else if(meshlist[i].p07 == 0x05) {
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.flags |= AMC_VERTEX_UV | AMC_VERTEX_WEIGHTS;
       }
     else if(meshlist[i].p07 == 0x06) {
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.flags |= AMC_VERTEX_UV | AMC_VERTEX_WEIGHTS;
       }

     // copy vertex data
     if(meshlist[i].p07 == 0x03)
       {
        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = bs.LE_read_real32();
            vb.data[j].vy = bs.LE_read_real32();
            vb.data[j].vz = bs.LE_read_real32();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08(); // normal?
            bs.LE_read_uint08(); // normal?
            bs.LE_read_uint08(); // normal?
            bs.LE_read_uint08(); // normal?
            vb.data[j].uv[0][0] = bs.LE_read_uint16()/4096.0f;
            vb.data[j].uv[0][1] = bs.LE_read_uint16()/4096.0f;
           }
       }
     else if(meshlist[i].p07 == 0x05)
       {
        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = bs.LE_read_real32();
            vb.data[j].vy = bs.LE_read_real32();
            vb.data[j].vz = bs.LE_read_real32();
            bs.LE_read_uint08(); // vector #1
            bs.LE_read_uint08(); // vector #1
            bs.LE_read_uint08(); // vector #1
            bs.LE_read_uint08(); // vector #1
            bs.LE_read_uint08(); // vector #2
            bs.LE_read_uint08(); // vector #2
            bs.LE_read_uint08(); // vector #2
            bs.LE_read_uint08(); // vector #2
            vb.data[j].uv[0][0] = bs.LE_read_uint16()/4096.0f;
            vb.data[j].uv[0][1] = bs.LE_read_uint16()/4096.0f;
            vb.data[j].wi[0] = bs.LE_read_uint08();
            vb.data[j].wi[1] = bs.LE_read_uint08();
            vb.data[j].wi[2] = bs.LE_read_uint08();
            vb.data[j].wi[3] = bs.LE_read_uint08();
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = bs.LE_read_uint08()/255.0f;
            vb.data[j].wv[1] = bs.LE_read_uint08()/255.0f;
            vb.data[j].wv[2] = bs.LE_read_uint08()/255.0f;
            vb.data[j].wv[3] = bs.LE_read_uint08()/255.0f;
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }
     else if(meshlist[i].p07 == 0x06)
       {
        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = bs.LE_read_real32();
            vb.data[j].vy = bs.LE_read_real32();
            vb.data[j].vz = bs.LE_read_real32();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            bs.LE_read_uint08();
            vb.data[j].uv[0][0] = bs.LE_read_uint16()/4096.0f;
            vb.data[j].uv[0][1] = bs.LE_read_uint16()/4096.0f;
            vb.data[j].wi[0] = bs.LE_read_uint08();
            vb.data[j].wi[1] = bs.LE_read_uint08();
            vb.data[j].wi[2] = bs.LE_read_uint08();
            vb.data[j].wi[3] = bs.LE_read_uint08();
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = bs.LE_read_uint08()/255.0f;
            vb.data[j].wv[1] = bs.LE_read_uint08()/255.0f;
            vb.data[j].wv[2] = bs.LE_read_uint08()/255.0f;
            vb.data[j].wv[3] = bs.LE_read_uint08()/255.0f;
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }

     // insert vertex buffer
     amc.vblist.push_back(vb);
    }

 // process index buffer
 AMC_IDXBUFFER ib;
 ib.type = AMC_TRIANGLES;
 ib.format = AMC_UINT16;
 ib.name = "default";
 ib.wind = AMC_CCW;
 ib.elem = h03; // h03 is number of indices
 uint32 ib_datasize = ib.elem * 0x02;
 ib.data = ibdata;

 // insert index buffer
 amc.iblist.push_back(ib);

 // process surfaces
 for(uint32 i = 0; i < meshlist.size(); i++)
    {
     // create surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << i;

     // create reference
     AMC_REFERENCE ref;
     ref.vb_index = i;
     ref.vb_start = 0; 
     ref.vb_width = meshlist[i].p02;
     ref.ib_index = 0;
     ref.ib_start = meshlist[i].p05;
     ref.ib_width = meshlist[i].p03 * 3;
     ref.jm_index = AMC_INVALID_JMAP_INDEX;

     // create joint map
     if(meshlist[i].p08.size()) {
        AMC_JOINTMAP jmap;
        jmap.jntmap = meshlist[i].p08;
        amc.jmaplist.push_back(jmap);
        ref.jm_index = amc.jmaplist.size() - 1;
       }

     // create surface
     AMC_SURFACE surface;
     surface.name = ss.str();
     surface.surfmat = AMC_INVALID_SURFMAT;
     surface.refs.push_back(ref);

     // insert surface
     amc.surfaces.push_back(surface);
    }

 //
 // AMC SKELETON
 //

 if(jointlist.size())
   {
    // initialize skeleton data
    AMC_SKELETON2 skel;
    skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
    skel.name = "skeleton";

    // create and insert root joint
    AMC_JOINT joint;
    joint.name = "root";
    joint.id = 255;
    joint.parent = AMC_INVALID_JOINT;
    joint.m_rel[0x0] = 1.0f;
    joint.m_rel[0x1] = 0.0f;
    joint.m_rel[0x2] = 0.0f;
    joint.m_rel[0x3] = 0.0f;
    joint.m_rel[0x4] = 0.0f;
    joint.m_rel[0x5] = 1.0f;
    joint.m_rel[0x6] = 0.0f;
    joint.m_rel[0x7] = 0.0f;
    joint.m_rel[0x8] = 0.0f;
    joint.m_rel[0x9] = 0.0f;
    joint.m_rel[0xA] = 1.0f;
    joint.m_rel[0xB] = 0.0f;
    joint.m_rel[0xC] = 0.0f;
    joint.m_rel[0xD] = 0.0f;
    joint.m_rel[0xE] = 0.0f;
    joint.m_rel[0xF] = 1.0f;
    joint.p_rel[0] = 0.0f;
    joint.p_rel[1] = 0.0f;
    joint.p_rel[2] = 0.0f;
    joint.p_rel[3] = 1.0f;
    skel.joints.push_back(joint);

    for(uint32 i = 0; i < jointlist.size(); i++)
       {
        // create joint name
        stringstream ss;
        //ss << "jnt_" << setfill('0') << setw(3) << i;
        ss << jointlist[i].name;
    
        // create and insert joint
        AMC_JOINT joint;
        joint.name = ss.str();
        joint.id = i;
        joint.parent = 255;
        joint.m_rel[0x0] = jointlist[i].m[0x0];
        joint.m_rel[0x1] = jointlist[i].m[0x1];
        joint.m_rel[0x2] = jointlist[i].m[0x2];
        joint.m_rel[0x3] = jointlist[i].m[0x3];
        joint.m_rel[0x4] = jointlist[i].m[0x4];
        joint.m_rel[0x5] = jointlist[i].m[0x5];
        joint.m_rel[0x6] = jointlist[i].m[0x6];
        joint.m_rel[0x7] = jointlist[i].m[0x7];
        joint.m_rel[0x8] = jointlist[i].m[0x8];
        joint.m_rel[0x9] = jointlist[i].m[0x9];
        joint.m_rel[0xA] = jointlist[i].m[0xA];
        joint.m_rel[0xB] = jointlist[i].m[0xB];
        joint.m_rel[0xC] = jointlist[i].m[0xC];
        joint.m_rel[0xD] = jointlist[i].m[0xD];
        joint.m_rel[0xE] = jointlist[i].m[0xE];
        joint.m_rel[0xF] = jointlist[i].m[0xF];
        joint.p_rel[0] = jointlist[i].m[0x3];
        joint.p_rel[1] = jointlist[i].m[0x7];
        joint.p_rel[2] = jointlist[i].m[0xB];
        joint.p_rel[3] = 1.0f;
        skel.joints.push_back(joint);
       }
    
    // insert skeleton
    amc.skllist2.push_back(skel);
   }

 // transform and save AMC file
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

//
// ARCHIVE
//

namespace X_SYSTEM { namespace X_GAME {

bool processBIN(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // load file
 size_t filesize;
 if(!GetFileSizeFromFileStream(ifile, filesize)) return error("Failed to obtain filesize.", __LINE__);
 boost::shared_array<char> filedata(new char[filesize]);
 ifile.read(filedata.get(), filesize);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // binary stream
 binary_stream bs(filedata, filesize);
 bs.set_endian_mode(ENDIAN_LITTLE);

 // read header
 uint32 h01 = bs.read_uint32();
 uint16 h02 = bs.read_uint16();
 uint08 h03 = bs.read_uint08();


 

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
 p1 += TEXT("[PC] Alan Wake");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where BIN and RDMP files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~ GB free space.\n");
 p3 += TEXT("3.  GB for game +  GB extraction.\n");

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
 bool doBIN = true;

 // questions
 // if(doLDA) doLDA = YesNoBox("Process LINKDATA.A file?\nSay 'No' if you have already done this before.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // PHASE #1
 if(doBIN)
   {
    // build filename list
    cout << "Searching for .BIN files... please wait." << endl;
    deque<STDSTRING> filelist;
    if(!BuildFilenameList(filelist, TEXT(".BIN"), pathname.c_str())) return true;
    
    cout << "Processing .BIN files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processBIN(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

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

}}

bool TestAlanWake(void)
{
 PC::ALAN_WAKE::extract();
 return true;
}