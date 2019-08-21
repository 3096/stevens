#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_zlib.h"
#include "x_dds.h"
#include "x_amc.h"
#include "pc_tr2013.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   TR2013

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processPCD(const string& filename)
{
 // open CDRM file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open PCD9 file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // read magic
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x39444350) return error("Not a PCD9 file.");

 // read type
 uint32 type = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read datasize
 uint32 datasize = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown (number of mipmaps perhaps)
 uint32 unk01 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read width and height
 uint16 dx = LE_read_uint16(ifile);
 uint16 dy = LE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown
 uint32 unk02 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown
 uint32 unk03 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // A8R8G8B8
 DDS_HEADER ddsh;
 ZeroMemory(&ddsh, sizeof(ddsh));
 if(type == 0x00000015) {
    if(!CreateA8R8G8B8DDSHeader(dx, dy, unk01, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // DXT1
 else if(type == 0x31545844) {
    if(!::CreateDXT1Header(dx, dy, unk01, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // DXT5
 else if(type == 0x35545844) {
    if(!::CreateDXT5Header(dx, dy, unk01, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 else
    return error("Unknown texture format.");

 // read data
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 if(ifile.fail()) return error("Read failure.");

 // close input file so we can rename!
 ifile.close();

 // create output file
 ofstream ofile(filename.c_str(), ios::binary);
 if(!ofile) return error("Failed to create DDS file.");

 // write DDS header
 ofile.write("DDS ", 4);
 ofile.write((char*)&ddsh, sizeof(ddsh));

 // write DDS data
 ofile.write(data.get(), datasize);
 ofile.close();

 return true;
}

};};

//
// SOUNDS
//

namespace X_SYSTEM { namespace X_GAME {

bool processFSB(const string& filename)
{
 // open CDRM file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open FSB file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // skip the first 0x10 bytes
 ifile.seekg(0x10);
 if(ifile.fail()) return error("Seek failure.");

 // read FSB4
 uint32 magic = LE_read_uint32(ifile);
 if(magic != 0x34425346) return error("DRM is not an FSB file.");

 // reseek
 ifile.seekg(0x10);
 if(ifile.fail()) return error("Seek failure.");

 // read data
 uint32 datasize = filesize - 0x10;
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 if(ifile.fail()) return error("Read failure.");

 // close input file so we can delete it!
 ifile.close();
 DeleteFileA(filename.c_str());

 // rename file
 ofstream ofile(filename.c_str(), ios::binary);
 if(!ofile) return error("Failed to create FSB file.");
 ofile.write(data.get(), datasize);
 if(ofile.fail()) return error("Write failure.");

 return true;
}

}};

//
// MODDING
//

namespace X_SYSTEM { namespace X_GAME {

bool DDSToPCD(const char* dname, const char* pname)
{
 return true;
}

bool PCDToDRM(const char* dname, const char* pname)
{
 return true;
}

}};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

// read header
struct TRHEADER {
 uint32 unk01;
 uint32 meshsize;
 uint32 n_indices;
 real32 vector1[4];
 real32 vector2[4];
 real32 vector3[6];
 uint08 unk02[0x2C];
 uint32 partinfo_offset; // offset to part information
 uint32 iteminfo_offset; // offset to mesh information
 uint32 unk_offset01; // these two are usually the same
 uint32 unk_offset02; // these two are usually the same
 uint32 ib_offset;
 uint16 n_parts;
 uint16 n_items;
 uint32 n_joints;
 uint32 EOH;
};

struct TRMESHINFO {
 uint32 p01; // number of parts
 uint16 p02; // unknown
 uint16 p03; // number of bone map index entries
 uint32 p04; // offset from mesh to bone map data
 uint32 p05; // offset from mesh to vertex buffer
 uint32 p06; // 0x00
 uint32 p07; // 0x00
 uint32 p08; // 0x00
 uint32 p09; // offset from mesh to vertex information
 uint32 p10; // number of vertices
 uint32 p11; // 0x00
 uint32 p12; // start index from index buffer data
 uint32 p13; // number of triangles
};

struct TRVTXFORMATITEM {
 uint32 p01; // unknown
 uint16 p02; // offset
 uint16 p03; // type
};

struct TRVTXFORMAT {
 uint32 p01; // unknown
 uint32 p02; // unknown
 uint16 p03; // number of mesh info items
 uint16 p04; // bytes per vertex
 uint16 p05; // 0x0000
 uint16 p06; // 0x0000
 TRVTXFORMATITEM p07[10];
};

bool processMESH(const string& filename)
{
 // open CDRM file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open MESH file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // read header
 uint32 h01 = LE_read_uint32(ifile); // number of 0xEBBEEBBE
 uint32 h02 = LE_read_uint32(ifile); // 0x00
 uint32 h03 = LE_read_uint32(ifile); // 0x00
 uint32 h04 = LE_read_uint32(ifile); // 0x00
 uint32 h05 = LE_read_uint32(ifile); // number of 0x??000014 entries

 // read header entries part 1
 deque<pair<uint32, uint32>> h06;
 for(uint32 i = 0; i < h01; i++) {
     uint32 p01 = LE_read_uint32(ifile);
     uint32 p02 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     h06.push_back(pair<uint32, uint32>(p01, p02));
    }

 // read header entries part 2
 deque<uint32> h07;
 for(uint32 i = 0; i < h05; i++) {
     uint32 item = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
      h07.push_back(item);
    }

 // move to mesh data
 ifile.seekg(h06[0].second, ios::cur);
 if(ifile.fail()) return error("Seek failure.");
 uint32 mesh_offset = static_cast<uint32>(ifile.tellg());

 // read magic
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x6873654D) return error("Expecting Mesh.");

 TRHEADER header;
 header.unk01 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 header.meshsize = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 header.n_indices = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 LE_read_array(ifile, &header.vector1[0], 4);
 if(ifile.fail()) return error("Read failure.");

 LE_read_array(ifile, &header.vector2[0], 4);
 if(ifile.fail()) return error("Read failure.");

 LE_read_array(ifile, &header.vector3[0], 6);
 if(ifile.fail()) return error("Read failure.");

 LE_read_array(ifile, &header.unk02[0], 0x2C);
 if(ifile.fail()) return error("Read failure.");

 // read offset to part information
 header.partinfo_offset = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to item information
 // there is one of these item per vertex buffer
 header.iteminfo_offset = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // usually points to 0xA0
 header.unk_offset01 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // usually points to 0xA0
 header.unk_offset02 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to index buffer
 // there is only one index buffer per file
 header.ib_offset = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 header.n_parts = LE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 header.n_items = LE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of joints
 header.n_joints = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 header.EOH = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.EOH != 0xFFFFFFFF && header.EOH != 0xA0) return message(" Invalid end-of-header.");

 // model container
 ADVANCEDMODELCONTAINER amc;

 // seek mesh item information data
 uint32 position = mesh_offset + header.iteminfo_offset;
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");

 // read mesh item information
 deque<TRMESHINFO> meshinfo;
 for(uint32 i = 0; i < header.n_items; i++)
    {
     TRMESHINFO info;
     info.p01 = LE_read_uint32(ifile);
     info.p02 = LE_read_uint16(ifile);
     info.p03 = LE_read_uint16(ifile);
     info.p04 = LE_read_uint32(ifile);
     info.p05 = LE_read_uint32(ifile);
     info.p06 = LE_read_uint32(ifile);
     info.p07 = LE_read_uint32(ifile);
     info.p08 = LE_read_uint32(ifile);
     info.p09 = LE_read_uint32(ifile);
     info.p10 = LE_read_uint32(ifile);
     info.p11 = LE_read_uint32(ifile);
     info.p12 = LE_read_uint32(ifile);
     info.p13 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     meshinfo.push_back(info);
    }

 // read vertex buffers
 for(uint32 i = 0; i < meshinfo.size(); i++)
    {
     // seek vertex format information
     position = mesh_offset + meshinfo[i].p09;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read 60-byte vertex format information
     TRVTXFORMAT format;
     format.p01 = LE_read_uint32(ifile);
     format.p02 = LE_read_uint32(ifile);
     format.p03 = LE_read_uint16(ifile); // number of items
     format.p04 = LE_read_uint16(ifile); // bytes per vertex
     format.p05 = LE_read_uint16(ifile);
     format.p06 = LE_read_uint16(ifile);
     for(uint32 j = 0; j < 10; j++) {
         format.p07[j].p01 = LE_read_uint32(ifile);
         format.p07[j].p02 = LE_read_uint16(ifile);
         format.p07[j].p03 = LE_read_uint16(ifile);
        }

     // vertex format information
     uint16 position_offset = 0xFFFF;
     uint16 position_type = 0xFFFF;
     uint16 normal_offset = 0xFFFF;
     uint16 normal_type = 0xFFFF;
     uint16 texcoord_offset = 0xFFFF;
     uint16 texcoord_type = 0xFFFF;
     uint16 blendweights_offset = 0xFFFF;
     uint16 blendweights_type = 0xFFFF;
     uint16 blendindices_offset = 0xFFFF;
     uint16 blendindices_type = 0xFFFF;

     // set vertex format information
     for(uint32 j = 0; j < format.p03; j++)
        {
         // position
         if(format.p07[j].p01 == 0xD2F7D823) {
            position_offset = format.p07[j].p02;
            position_type = format.p07[j].p03;
           }
         // normal?
         else if(format.p07[j].p01 == 0x3E7F6149) {
            normal_offset = format.p07[j].p02;
            normal_type = format.p07[j].p03;
           }
         // texture coordinates
         else if(format.p07[j].p01 == 0x8317902A) {
            texcoord_offset = format.p07[j].p02;
            texcoord_type = format.p07[j].p03;
           }
         // blend weights
         else if(format.p07[j].p01 == 0x48E691C0) {
            blendweights_offset = format.p07[j].p02;
            blendweights_type = format.p07[j].p03;
           }
         // blend indices
         else if(format.p07[j].p01 == 0x5156D8D3) {
            blendindices_offset = format.p07[j].p02;
            blendindices_type = format.p07[j].p03;
           }
        }

     // allocate data to hold one vertex
     boost::shared_array<char> vertex(new char[format.p04]);
     binary_stream bs(vertex, format.p04);

     // seek vertex buffer
     position = mesh_offset + meshinfo[i].p05;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // allocate vertex buffer
     AMC_VTXBUFFER vb;
     vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV;
     if(blendweights_type != 0xFFFF) vb.flags |= AMC_VERTEX_WEIGHTS;
     vb.name = "default";
     vb.uvchan = 1;
     vb.uvtype[0] = AMC_DIFFUSE_MAP;
     vb.uvtype[1] = AMC_INVALID_MAP;
     vb.uvtype[2] = AMC_INVALID_MAP;
     vb.uvtype[3] = AMC_INVALID_MAP;
     vb.uvtype[4] = AMC_INVALID_MAP;
     vb.uvtype[5] = AMC_INVALID_MAP;
     vb.uvtype[6] = AMC_INVALID_MAP;
     vb.uvtype[7] = AMC_INVALID_MAP;
     vb.colors = 0;
     vb.elem = meshinfo[i].p10;
     vb.data.reset(new AMC_VERTEX[vb.elem]);

     // process vertices
     for(uint32 j = 0; j < vb.elem; j++)
        {
         // read vertex (also changes binary stream)
         LE_read_array(ifile, vertex.get(), format.p04);
         if(ifile.fail()) return error("Read failure.");

         // seek position
         if(position_type == 0xFFFF) return error("Position type not found in vertex data.");
         bs.seek(position_offset);
         if(bs.fail()) return error("Stream seek failure.");

         // read position
         if(position_type == 0x02) {
            vb.data[j].vx = bs.LE_read_real32();
            vb.data[j].vy = bs.LE_read_real32();
            vb.data[j].vz = bs.LE_read_real32();
           }
         else
            return error("Unknown position type.");

         // read texture coordinates
         if(texcoord_type != 0xFFFF)
           {
            bs.seek(texcoord_offset);
            if(bs.fail()) return error("Stream seek failure.");

            if(texcoord_type == 0x01) {
               vb.data[j].uv[0][0] = bs.LE_read_real32();
               vb.data[j].uv[0][1] = bs.LE_read_real32();
              }
            else if(texcoord_type == 0x13) {
               vb.data[j].uv[0][0] = bs.LE_read_sint16()/2048.0f;
               vb.data[j].uv[0][1] = bs.LE_read_sint16()/2048.0f;
              }
            else {
               stringstream ss;
               ss << "Unknown texture coordinates type for vertex buffer at 0x" << hex << position << dec << ".";
               return error(ss);
              }
           }

         // read blend weights
         if(blendweights_type != 0xFFFF)
           {
            if(blendweights_type == 0x06) {
               bs.seek(blendweights_offset);
               if(bs.fail()) return error("Stream seek failure.");
               vb.data[j].wv[0] = bs.LE_read_uint08()/255.0f;
               vb.data[j].wv[1] = bs.LE_read_uint08()/255.0f;
               vb.data[j].wv[2] = bs.LE_read_uint08()/255.0f;
               vb.data[j].wv[3] = bs.LE_read_uint08()/255.0f;
              }
            else {
               stringstream ss;
               ss << "Unknown blend weights type for vertex buffer at 0x" << hex << position << dec << ".";
               return error(ss);
              }
           }

         // read blend indices
         if(blendindices_type != 0xFFFF)
           {
            if(blendindices_type == 0x07) {
               bs.seek(blendindices_offset);
               if(bs.fail()) return error("Stream seek failure.");
               vb.data[j].wi[0] = (uint16)bs.LE_read_uint08();
               vb.data[j].wi[1] = (uint16)bs.LE_read_uint08();
               vb.data[j].wi[2] = (uint16)bs.LE_read_uint08();
               vb.data[j].wi[3] = (uint16)bs.LE_read_uint08();
               vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
              }
            else
               return error("Unknown blend weights type.");
           }
        }
     
     amc.vblist.push_back(vb);
    }

 // move to index buffer
 position = mesh_offset + header.ib_offset;
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");

 // read index buffer
 uint32 ib_elem = header.n_indices;
 uint32 ib_size = header.n_indices * 2;
 boost::shared_array<char> ib_data(new char[ib_size]);
 LE_read_array(ifile, reinterpret_cast<uint16*>(ib_data.get()), ib_elem);

 // allocate index buffer
 AMC_IDXBUFFER ib;
 ib.format = AMC_UINT16;
 ib.type = AMC_TRIANGLES;
 ib.wind = AMC_CW;
 ib.reserved = 0;
 ib.name = "default";
 ib.elem = ib_elem;
 ib.data = ib_data;
     
 // insert index buffer
 amc.iblist.push_back(ib);

 // move to buffer reference data
 position = mesh_offset + header.partinfo_offset;
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");

 struct TRBUFFERREF {
  real32 p01[4]; // unknown vector
  uint32 p02;    // ib_start
  uint32 p03;    // number of triangles
  uint32 p04;    // vb_start or n_vertices (depending)
  uint32 p05;    // ????
  uint32 p06;    // ????
  uint32 p07;    // ????
  uint32 p08;    // ????
  uint32 p09;    // 0x00000000
  uint32 p10;    // 0x00000000
  uint32 p11;    // 0x00000000
  uint32 p12;    // ????
  uint32 p13;    // 0xFFFFFFFF
  uint32 p14;    // 0xFFFFFFFF
  uint32 p15;    // 0xFFFFFFFF
  uint32 p16;    // 0xFFFFFFFF
  uint32 p17;    // 0xFFFFFFFF
 };

 // read buffer references
 deque<TRBUFFERREF> brefs;
 for(uint32 i = 0; i < header.n_parts; i++) {
     TRBUFFERREF item;
     item.p01[0] = LE_read_real32(ifile);
     item.p01[1] = LE_read_real32(ifile);
     item.p01[2] = LE_read_real32(ifile);
     item.p01[3] = LE_read_real32(ifile);
     item.p02 = LE_read_uint32(ifile); // ib_start
     item.p03 = LE_read_uint32(ifile); // number of triangles
     item.p04 = LE_read_uint32(ifile); // number of vertices
     item.p05 = LE_read_uint32(ifile);
     item.p06 = LE_read_uint32(ifile);
     item.p07 = LE_read_uint32(ifile);
     item.p08 = LE_read_uint32(ifile);
     item.p09 = LE_read_uint32(ifile);
     item.p10 = LE_read_uint32(ifile);
     item.p11 = LE_read_uint32(ifile);
     item.p12 = LE_read_uint32(ifile);
     item.p13 = LE_read_uint32(ifile);
     item.p14 = LE_read_uint32(ifile);
     item.p15 = LE_read_uint32(ifile);
     item.p16 = LE_read_uint32(ifile);
     item.p17 = LE_read_uint32(ifile);
     brefs.push_back(item);
    }

 // for each mesh item
 uint32 part_index = 0;
 for(uint32 i = 0; i < header.n_items; i++)
    {
     uint32 part_a = part_index;
     uint32 part_b = part_index + meshinfo[i].p01;

     // move to bone map data
     position = mesh_offset + meshinfo[i].p04;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read bone map indices
     deque<uint32> bmlist;
     for(uint32 j = 0; j < meshinfo[i].p03; j++) {
         uint32 item = LE_read_uint32(ifile);
         bmlist.push_back(item);
        }
     AMC_JOINTMAP jm;
     jm.jntmap = bmlist;
     amc.jmaplist.push_back(jm);

     // for each mesh part
     for(uint32 j = part_a; j < part_b; j++)
        {
         // create mesh name
         stringstream ss;
         ss << "mesh_" << setfill('0') << setw(3) << i << "_";
         ss << "part_" << setfill('0') << setw(3) << j;

         // create surface
         AMC_SURFACE sur;
         sur.name = ss.str();
         AMC_REFERENCE ref;
         ref.vb_index = i;
         ref.vb_start = 0;                // use full vertex buffer
         ref.vb_width = meshinfo[i].p10;  // use full vertex buffer
         ref.ib_index = 0;                // always only one index buffer
         ref.ib_start = brefs[j].p02;     // index into index buffer
         ref.ib_width = brefs[j].p03 * 3; // number of triangles * 3
         ref.jm_index = (bmlist.size() ? i : AMC_INVALID_JMAP_INDEX);
         sur.refs.push_back(ref);
         sur.surfmat = AMC_INVALID_SURFMAT;

         // insert surface
         amc.surfaces.push_back(sur);
        }

     // increment part index
     part_index += meshinfo[i].p01;
    }

 // find skeleton data
 if(h01 > 3)
   {
    // initialize skeleton
    AMC_SKELETON2 skel2;
    skel2.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
    skel2.name = "skeleton";

    // seek skeleton data
    uint32 start = 0x14 + h06.size() * 8 + h07.size() * 4;
    uint32 position = start + h06[h01 - 2].first - 4;
    ifile.seekg(position);
    if(ifile.fail()) return error("Seek failure.");

    // read number of joints
    uint32 n_joints = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // if there are joints
    if(n_joints != 0xBEEBBEEB && n_joints > 0 && n_joints < 1024)
      {
       // read 0xEBBEEBBE
       uint32 useless1 = LE_read_uint32(ifile);
       if(ifile.fail()) return error("Read failure.");
       if(useless1 != 0xBEEBBEEB) {
          stringstream ss;
          ss << "Expecting 0xEBBEEBBE during skeleton parsing at position 0x";
          ss << hex << position << dec << ".";
          return error(ss);
         }

       // move to joint list bytes
       ifile.seekg(start + h06[h01 - 2].second);
       if(ifile.fail()) return error("Seek failure.");

       // for each joint
       for(uint32 i = 0; i < n_joints; i++)
          {
           // seek joint position data
           ifile.seekg(0x20, ios::cur);
           if(ifile.fail()) return error("Seek failure.");

           // read joint position
           real32 jx = LE_read_real32(ifile);
           real32 jy = LE_read_real32(ifile);
           real32 jz = LE_read_real32(ifile);
           if(ifile.fail()) return error("Read failure.");

           // seek joint parent data
           ifile.seekg(0x08, ios::cur);
           if(ifile.fail()) return error("Seek failure.");

           // validate joint data (joint list offset must be correct!)
           uint32 temp = LE_read_uint32(ifile);
           if(temp != 0xFFFF0000) return error("Expecting 0xFFFF0000 while parsing joint data.");

           // read parent data
           uint32 parent = LE_read_uint32(ifile);
           if(ifile.fail()) return error("Read failure.");

           // seek end of joint data
           ifile.seekg(0x04, ios::cur);
           if(ifile.fail()) return error("Seek failure.");

           // create joint name
           stringstream ss;
           ss << "jnt_" << setfill('0') << setw(3) << i;
           
           // create and insert joint
           AMC_JOINT joint;
           joint.name = ss.str();
           joint.id = i;
           joint.parent = (parent == 0xFFFFFFFF ? AMC_INVALID_JOINT : parent);
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
           joint.p_rel[0] = jx;
           joint.p_rel[1] = jy;
           joint.p_rel[2] = jz;
           joint.p_rel[3] = 1.0f;
           skel2.joints.push_back(joint);
          }

       // insert skeleton
       amc.skllist2.push_back(skel2);
      }
   }

 // save SMC file
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 return true;
}

bool processSCENE(const string& filename)
{
 return true;
}

};};

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

struct TABLESUBENTRY1 {
 uint32 p01;
 uint32 p02;
 uint32 p03;
 uint32 p04;
 uint32 p05;
};
struct TABLESUBENTRY2 {
 uint32 p01;
 uint32 p02;
 uint32 p03;
 uint32 p04;
};

struct TABLEENTRY {
 uint32 p01;
 uint32 p02;
 uint32 p03;
 uint32 p04;
 uint32 p05;
 uint32 p06;
 uint32 p07;
 uint32 p08;
 deque<TABLESUBENTRY1> sub1;
 boost::shared_array<char> namelist;
 deque<TABLESUBENTRY2> sub2;
};

bool processTIGER(const std::string& pathname)
{
 // tiger filenames
 string filename[4];
 filename[0] = pathname; filename[0] += "bigfile.000.tiger";
 filename[1] = pathname; filename[1] += "bigfile.001.tiger";
 filename[2] = pathname; filename[2] += "bigfile.002.tiger";
 filename[3] = pathname; filename[3] += "bigfile.003.tiger";

 // open tiger files
 cout << "Opening files... please wait." << endl;
 ifstream filelist[4];
 filelist[0].open(filename[0], ios::binary);
 filelist[1].open(filename[1], ios::binary);
 filelist[2].open(filename[2], ios::binary);
 filelist[3].open(filename[3], ios::binary);
 if(!filelist[0].is_open()) return error("Failed to open TIGER file.");
 if(!filelist[0].is_open()) return error("Failed to open TIGER file.");
 if(!filelist[0].is_open()) return error("Failed to open TIGER file.");
 if(!filelist[0].is_open()) return error("Failed to open TIGER file.");
 cout << "Opening files... done." << endl;
 cout << endl;

 // create target directories
 cout << "Creating target directories... please wait." << endl;
 for(uint32 i = 0; i < 4; i++) {
     stringstream ss;
     ss << GetPathnameFromFilename(filename[i]);
     ss << GetShortFilenameWithoutExtension(filename[i]);
     ss << "\\";
     CreateDirectoryA(ss.str().c_str(), NULL);
    }
 cout << "Creating target directories... done." << endl;
 cout << endl;

 // create link directory
 cout << "Creating link directory... please wait." << endl;
 string linkpath = pathname;
 linkpath += "symbolic_links";
 linkpath += "\\";
 CreateDirectoryA(linkpath.c_str(), NULL);
 cout << "Creating link directory... done." << endl;
 cout << endl;

 // important data
 deque<TABLEENTRY> entrylist;
 map<uint32, string> filemap;
 set<uint32> fileset;

 // move to first link entry
 uint32 currpos = 0x1A800;
 filelist[0].seekg(0x1A800);
 if(filelist[0].fail()) return error("Seek failure.");

 // parse link table in first tiger file
 cout << "Processing link directories... please wait." << endl;
 uint32 n_files = 0;
 for(;;)
    {
     // read entry header
     TABLEENTRY entry;
     entry.p01 = LE_read_uint32(filelist[0]);
     entry.p02 = LE_read_uint32(filelist[0]); // string length part 1
     entry.p03 = LE_read_uint32(filelist[0]); // string length part 2
     entry.p04 = LE_read_uint32(filelist[0]);
     entry.p05 = LE_read_uint32(filelist[0]);
     entry.p06 = LE_read_uint32(filelist[0]);
     entry.p07 = LE_read_uint32(filelist[0]); // number of sections this file is composed of
     entry.p08 = LE_read_uint32(filelist[0]);
     if(entry.p01 != 0x16) break;
     n_files += entry.p07;

     // read entry part 1
     for(uint32 i = 0; i < entry.p07; i++) {
         TABLESUBENTRY1 se;
         se.p01 = LE_read_uint32(filelist[0]); // size of data
         se.p02 = LE_read_uint32(filelist[0]); // file type
         se.p03 = LE_read_uint32(filelist[0]); // XXXY X = size of header Y = file subtype
         se.p04 = LE_read_uint32(filelist[0]); // XXXXX (see TABLESUBENTRY2)
         se.p05 = LE_read_uint32(filelist[0]); // 0xFFFFFFFF
         entry.sub1.push_back(se);
        }

     // read entry part 2
     uint32 len = entry.p02 + entry.p03;
     if(len) {
        boost::shared_array<char> name(new char[len]);
        LE_read_array(filelist[0], name.get(), len);
        entry.namelist = name;
       }

     // read entry part 3
     for(uint32 i = 0; i < entry.p07; i++) {
         TABLESUBENTRY2 se;
         se.p01 = LE_read_uint32(filelist[0]); // for meshes this is 0x0C0XXXXX
         se.p02 = LE_read_uint32(filelist[0]); // offset + bigfile index
         se.p03 = LE_read_uint32(filelist[0]); // size of CDRM section
         se.p04 = LE_read_uint32(filelist[0]);
         entry.sub2.push_back(se);
         fileset.insert(se.p02);
        }

     // move to next 0x800-aligned position
     uint32 position = (uint32)filelist[0].tellg();
     position = ((position + 0x7FF) & (~0x7FF));
     filelist[0].seekg(position);
     if(filelist[0].fail()) return error("Seek failure.");

     // insert entry
     entrylist.push_back(entry);
     cout << "Processed link directory #0x" << hex << entrylist.size() << dec << "." << endl;
    }
 cout << "Processing link directories... done." << endl;
 cout << endl;

 cout << "Successfully processed all files." << endl;
 cout << "Number of link directories = 0x" << hex << entrylist.size() << dec << endl;
 cout << "Number of symbolic links = 0x" << hex << n_files << dec << endl;
 cout << "Number of target files = 0x" << hex << (uint32)fileset.size() << dec << endl;
 cout << endl;

 // BEGIN DEBUG
 // return true;
 // END DEBUG

 // decompress and build symbolic links
 cout << "Decompressing files... please wait." << endl;
 for(uint32 i = 0xF9F; i < entrylist.size(); i++)
    {
     // count the number of links we need to create
     uint32 n_links = 0;
     for(uint32 j = 0; j < entrylist[i].sub2.size(); j++)
        {
         if(entrylist[i].sub1[j].p02 == 0x00) ;
         else if(entrylist[i].sub1[j].p02 == 0x02) ;
         else if(entrylist[i].sub1[j].p02 == 0x05) n_links++;
         else if(entrylist[i].sub1[j].p02 == 0x06) n_links++;
         else if(entrylist[i].sub1[j].p02 == 0x07) ;
         else if(entrylist[i].sub1[j].p02 == 0x08) ;
         else if(entrylist[i].sub1[j].p02 == 0x09) ;
         else if(entrylist[i].sub1[j].p02 == 0x0A) ;
         else if(entrylist[i].sub1[j].p02 == 0x0B) ;
         else if(entrylist[i].sub1[j].p02 == 0x0C) {
            if((entrylist[i].sub1[j].p03 & 0xFF) == 0x30) ;
            else n_links++;
           }
         else if(entrylist[i].sub1[j].p02 == 0x0D) ;
         else ;
        }

     // if nothing needs to be linked, move on
     if(n_links == 0) continue;

     // create link subdirectory (only if there are files)
     if(entrylist[i].sub1.size()) {
        stringstream ss;
        ss << GetPathnameFromFilename(filename[0]);
        ss << "symbolic_links";
        ss << "\\";
        ss << setfill('0') << setw(4) << i;
        ss << "\\";
        CreateDirectoryA(ss.str().c_str(), NULL);
       }

     // decompress files
     cout << "Decompressing files for link directory #0x" << hex << i << dec << "." << endl;
     for(uint32 j = 0; j < entrylist[i].sub2.size(); j++)
        {
         // file offset and size of data
         uint32 offset = (entrylist[i].sub2[j].p02 & 0xFFFFFF00);
         uint32 size = entrylist[i].sub2[j].p03;
 
         // big file index
         uint32 bigfile = (entrylist[i].sub2[j].p02 & 0xFF);
         if(bigfile > 3) return error("Invalid bigfile index.");

         // select target extension
         string targext = ".";
         if(entrylist[i].sub1[j].p02 == 0x00) targext += "type_0";
         else if(entrylist[i].sub1[j].p02 == 0x02) targext += "type_2";
         else if(entrylist[i].sub1[j].p02 == 0x05) targext += "dds";
         else if(entrylist[i].sub1[j].p02 == 0x06) targext += "fsb";
         else if(entrylist[i].sub1[j].p02 == 0x07) targext += "type_7";
         else if(entrylist[i].sub1[j].p02 == 0x08) targext += "type_8";
         else if(entrylist[i].sub1[j].p02 == 0x09) targext += "shdr";
         else if(entrylist[i].sub1[j].p02 == 0x0A) targext += "mtrl";
         else if(entrylist[i].sub1[j].p02 == 0x0B) targext += "type_B";
         else if(entrylist[i].sub1[j].p02 == 0x0C) {
            if((entrylist[i].sub1[j].p03 & 0xFF) == 0x30) targext += "scene";
            else targext += "mesh";
           }
         else if(entrylist[i].sub1[j].p02 == 0x0D) targext += "type_D";
         else targext += "drm";

         // select symbolic link extension
         bool savelink = true;
         string linkext = ".";
         if(entrylist[i].sub1[j].p02 == 0x00) {
            savelink = false;
            linkext += "type_0";
           }
         else if(entrylist[i].sub1[j].p02 == 0x02) {
            savelink = false;
            linkext += "type_2";
           }
         else if(entrylist[i].sub1[j].p02 == 0x05) linkext += "dds";
         else if(entrylist[i].sub1[j].p02 == 0x06) linkext += "fsb";
         else if(entrylist[i].sub1[j].p02 == 0x07) {
            savelink = false;
            linkext += "type_7";
           }
         else if(entrylist[i].sub1[j].p02 == 0x08) {
            savelink = false;
            linkext += "type_8";
           }
         else if(entrylist[i].sub1[j].p02 == 0x09) {
            savelink = false;
            linkext += "shdr";
           }
         else if(entrylist[i].sub1[j].p02 == 0x0A) linkext += "mtrl";
         else if(entrylist[i].sub1[j].p02 == 0x0B) {
            savelink = false;
            linkext += "type_B";
           }
         else if(entrylist[i].sub1[j].p02 == 0x0C) {
            if((entrylist[i].sub1[j].p03 & 0xFF) == 0x30) linkext += "scene";
            else linkext += "mesh";
           }
         else if(entrylist[i].sub1[j].p02 == 0x0D) {
            savelink = false;
            linkext += "type_D";
           }
         else {
            savelink = false;
            linkext += "symlink";
           }

         // if file is a mesh
         bool is_MESH = (entrylist[i].sub1[j].p02 == 0x0C && (entrylist[i].sub1[j].p03 & 0xFF) != 0x30);
         stringstream meshname_targ;
         stringstream meshname_link;
         if(is_MESH)
           {
            // target filename
            meshname_targ << pathname;
            meshname_targ << GetShortFilenameWithoutExtension(filename[bigfile]);
            meshname_targ << "\\";
            meshname_targ << "0x" << hex << offset << dec;
            meshname_targ << ".smc";

            // linked filename
            meshname_link << GetPathnameFromFilename(filename[bigfile]);
            meshname_link << "symbolic_links";
            meshname_link << "\\";
            meshname_link << setfill('0') << setw(4) << i;
            meshname_link << "\\";
            meshname_link << setfill('0') << setw(4) << j << ".smc";
           }

         // generate a source filename
         stringstream targ;
         targ << pathname;
         targ << GetShortFilenameWithoutExtension(filename[bigfile]);
         targ << "\\";
         targ << "0x" << hex << offset << dec;
         targ << targext;

         // generate a symbolic link filename
         stringstream link;
         link << GetPathnameFromFilename(filename[bigfile]);
         link << "symbolic_links";
         link << "\\";
         link << setfill('0') << setw(4) << i;
         link << "\\";
         link << setfill('0') << setw(4) << j << linkext;

         // create target file
         auto iter = filemap.find(entrylist[i].sub2[j].p02);
         if(iter == filemap.end() && savelink)
           {
            // move to offset
            filelist[bigfile].seekg(offset);
            if(filelist[bigfile].fail()) return error("Seek failure.");
            
            // read CDRM
            uint32 magic = LE_read_uint32(filelist[bigfile]);
            if(filelist[bigfile].fail()) return error("Read failure.");
            if(magic != 0x4D524443) return error("Not a CDRM section.");
            
            // read header
            uint32 h01 = LE_read_uint32(filelist[bigfile]); // 0x00
            uint32 h02 = LE_read_uint32(filelist[bigfile]); // number of sections
            uint32 h03 = LE_read_uint32(filelist[bigfile]); // 0x00
            
            // read items
            deque<pair<uint32, uint32>> items;
            for(uint32 k = 0; k < h02; k++) {
                uint32 p01 = LE_read_uint32(filelist[bigfile]);
                uint32 p02 = LE_read_uint32(filelist[bigfile]);
                items.push_back(pair<uint32,uint32>(p01, p02));
               }
            
            // padding when items are odd
            if(h02 % 2) {
               LE_read_uint32(filelist[bigfile]);
               LE_read_uint32(filelist[bigfile]);
              }
            
            // create file
            ofstream ofile(targ.str().c_str(), ios::binary);
            if(!ofile) return error("Failed to create output file.");
            
            // read and save data
            for(uint32 k = 0; k < h02; k++)
               {
                // make sure 0x10-byte alignment
                uint32 position = (uint32)filelist[bigfile].tellg();
                position = ((position + 0x0F) & (~0x0F));
            
                // seek data
                filelist[bigfile].seekg(position);
                if(filelist[bigfile].fail()) return error("Seek failure.");
            
                // item information
                uint32 type = items[k].first;
                uint32 size = items[k].second;
            
                // read data
                boost::shared_array<char> data(new char[size]);
                filelist[bigfile].read(data.get(), size);
                if(filelist[bigfile].fail()) return error("Read failure.");
            
                // save data
                if(isZLIB(data[0], data[1])) {
                   if(!InflateZLIB(data, size, ofile))
                      return false;
                  }
                else {
                   ofile.write(data.get(), size);
                   if(ofile.fail()) return error("Write failure.");
                  }
               }
            
            // close output file
            ofile.close();
            
            // convert images to DDS
            if(entrylist[i].sub1[j].p02 == 0x05) {
               if(!processPCD(targ.str())) return false;
              }
            // strip FSB crap
            if(entrylist[i].sub1[j].p02 == 0x06) {
               if(!processFSB(targ.str())) return false;
              }
            // convert meshes to SMC
            if(is_MESH) {
               if(!processMESH(targ.str())) return false;
              }

            // insert source file entry
            typedef map<uint32, string>::value_type value_type;
            filemap.insert(value_type(entrylist[i].sub2[j].p02, targ.str()));
           }

         // create a symbolic link to source file
         if(savelink) {
            BOOLEAN success = CreateSymbolicLinkA(link.str().c_str(), targ.str().c_str(), 0x00);
            //BOOLEAN success = CreateHardLinkA(link.str().c_str(), targ.str().c_str(), NULL);
            if(success == 0) {
               DWORD status = GetLastError();
               HANDLE handle = CreateFileA(targ.str().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
               BY_HANDLE_FILE_INFORMATION bhfi;
               GetFileInformationByHandle(handle, &bhfi);
               cout << "There are " << bhfi.nNumberOfLinks << " symbolic links to this file." << endl;
               CloseHandle(handle);
               stringstream ss;
               ss << "Warning: failed to create symbolic link to ";
               ss << targ.str();
               ss << ". GetLastErrorCode() returned 0x" << hex << status << dec << ".";
               error(ss);
              }
            // create symbolic link for MESH file
            if(is_MESH)
              {
               BOOLEAN success = CreateHardLinkA(meshname_link.str().c_str(), meshname_targ.str().c_str(), NULL);
               if(success == 0) {
                  DWORD status = GetLastError();
                  HANDLE handle = CreateFileA(meshname_link.str().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
                  BY_HANDLE_FILE_INFORMATION bhfi;
                  GetFileInformationByHandle(handle, &bhfi);
                  cout << "There are " << bhfi.nNumberOfLinks << " symbolic links to this file." << endl;
                  CloseHandle(handle);
                  stringstream ss;
                  ss << "Warning: failed to create symbolic link ";
                  ss << meshname_link.str();
                  ss << " to ";
                  ss << meshname_targ.str();
                  ss << ". GetLastErrorCode() returned 0x" << hex << status << dec << ".";
                  error(ss);
                 }
              }
           }
        }
    }
 cout << "Decompressing files... done." << endl;
 cout << endl;

 return true;
}

};};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool extract(void)
{
 std::string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pname)
{
 // set pathname
 using namespace std;
 std::string pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname();

 // execution control
 bool doTIG = false;
 bool doMSH = true;
 bool doSCN = false;

 // process archive
 if(doTIG) {
    if(!processTIGER(pathname)) return false;
    cout << endl;
   }

 // construct link directory
 string linkpath = pathname;
 //linkpath += "symbolic_links";
 //linkpath += "\\";

 if(doMSH) {
    cout << "Processing .MESH files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".mesh", pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMESH(filelist[i])) return false;
       }
    cout << endl;
   }

 if(doSCN) {
    cout << "Processing .SCENE files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".scene", pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processSCENE(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

}};

