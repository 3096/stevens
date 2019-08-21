#include "..\xentax.h"
#include "..\x_file.h"
#include "..\x_findfile.h"
#include "..\x_stream.h"
#include "..\x_dds.h"
#include "..\x_amc.h"
#include "pc_MKX.h"

// BC7 includes
#include "../libraries/bc7/avpcl.h"

// xbox graphics include
#include "../xbox360/d3d9.h"
#include "../xbox360/d3d9types.h"
#include "../xbox360/xgraphics.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "xgraphics.lib")

bool MKX_LoadHashtable(LPCTSTR filename, std::map<uint64, std::string>& hashtable)
{
 // open XXX file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open XXX file.", __LINE__);

 // move to name table information
 ifile.seekg(0x35); // always 0x35
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read name table data
 uint32 n_entries = LE_read_uint32(ifile); // entries
 uint64 offset = LE_read_uint64(ifile); // offset
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // move to name table
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // erase hash table
 hashtable.erase(hashtable.begin(), hashtable.end());
 hashtable.clear();

 // read hash table
 typedef std::map<uint64, std::string>::value_type value_type;
 for(uint32 i = 0; i < n_entries; i++) {
     uint32 elem = LE_read_uint32(ifile);
     char name[1024];
     if(!read_string(ifile, &name[0], elem)) return error("Read failure.", __LINE__);
     hashtable.insert(value_type(i, std::string(name)));
    }

 return true;
}

bool MKX_ProcessStaticMesh(LPCTSTR filename, const uint64* info)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 //
 // FIXED PROPERTIES
 // 

 uint64 offset1 = info[0x0]; // position
 uint64 offset2 = info[0x1]; // normal
 uint64 offset3 = info[0x2]; // uv
 uint64 offset4 = info[0x3]; // index buffer

 ofstream dfile("c:\\users\\steven\\desktop\\debug.obj");
 dfile << "o debug" << endl;

 //
 //
 //
 
 // move to offset
 ifile.seekg(offset1);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read n
 uint32 p01 = LE_read_uint32(ifile); // bytpes per vertex
 uint32 p02 = LE_read_uint32(ifile); // number of vertices
 uint32 p03 = LE_read_uint32(ifile); // number of vertices
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(p02 != p03) return error("Number of vertices must match.");

 // for each n
 for(uint32 i = 0; i < p02; i++)
    {
     real32 x = LE_read_real32(ifile);
     real32 y = LE_read_real32(ifile);
     real32 z = LE_read_real32(ifile);
     dfile << "v " << x << " " << y << " " << z << endl;
    }

 //
 //
 //
 
 // move to offset
 ifile.seekg(offset3);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read n
 uint32 uv_p01 = LE_read_uint32(ifile); // number of channels
 uint32 uv_p02 = LE_read_uint32(ifile); // bytpes per vertex
 uint32 uv_p03 = LE_read_uint32(ifile); // number of vertices
 uint32 uv_p04 = LE_read_uint32(ifile); // number of vertices
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(uv_p03 != uv_p04) return error("Number of vertices must match.");

 // read uv
 for(uint32 i = 0; i < uv_p03; i++)
    {
     real32 u1 = LE_read_real16(ifile);
     real32 v1 = LE_read_real16(ifile);
     real32 u2 = LE_read_real16(ifile);
     real32 v2 = LE_read_real16(ifile);
     dfile << "vt " << u1 << " " << v1 << endl;
    }

 //
 //
 //
 
 // move to offset
 ifile.seekg(offset4);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read number of indices
 uint32 ib_elem = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // read index buffer
 boost::shared_array<uint16> ib_data(new uint16[ib_elem]);
 LE_read_array(ifile, ib_data.get(), ib_elem);

 // number of triangles
 uint32 n_tri = ib_elem/3;
 uint32 index = 0;
 while(index < ib_elem) {
  uint16 a = ib_data[index++] + 1;
  uint16 b = ib_data[index++] + 1;
  uint16 c = ib_data[index++] + 1;
  dfile << "f ";
  dfile << a << "/" << a << " ";
  dfile << b << "/" << b << " ";
  dfile << c << "/" << c << " ";
  dfile << endl;
 }

 return true;
}

bool MKX_ProcessSkeletalMesh(LPCTSTR filename, const uint64* info)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 //
 // FIXED PROPERTIES
 // 

 uint32 n_verts = info[0x0];
 uint64 name_offset1 = info[0x1]; // not important
 uint64 name_offset2 = info[0x2]; // not important
 uint64 mesh_info_offset1 = info[0x3]; // not important
 uint64 mesh_info_offset2 = info[0x4]; // not important
 uint64 mesh_info_offset3 = info[0x5]; // very important
 uint64 ib_offset  = info[0x6]; // index buffer
 uint64 jm_offset  = info[0x7]; // joint map
 uint64 vb1_offset = info[0x8]; // points
 uint64 vb2_offset = info[0x9]; // UV
 uint64 vb3_offset = info[0xA]; // blending
 uint64 vb4_offset = info[0xB]; // normal/tangent

 if(name_offset1 != 0xFFFFFFFF)
   {
    // mesh names #1
    ifile.seekg(name_offset1);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
    uint64 name_tablesize1 = LE_read_uint64(ifile);
    uint32 n_names1 = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.", __LINE__);
    
    // read mesh names
    boost::shared_array<string> names1(new string[n_names1]);
    for(uint32 i = 0; i < n_names1; i++) {
        uint32 size = LE_read_uint32(ifile);
        char buffer[1024];
        if(!read_string(ifile, &buffer[0], size)) return error("Read failure.", __LINE__);
        names1[i] = buffer;
       }
   }

 if(name_offset2 != 0xFFFFFFFF)
   {
    // mesh names #2
    ifile.seekg(name_offset2);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
    uint64 name_tablesize2 = LE_read_uint64(ifile);
    uint32 n_names2 = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.", __LINE__);
   
    // read mesh names
    boost::shared_array<string> names2(new string[n_names2]);
    for(uint32 i = 0; i < n_names2; i++) {
        uint32 size = LE_read_uint32(ifile);
        char buffer[1024];
        if(!read_string(ifile, &buffer[0], size)) return error("Read failure.", __LINE__);
        names2[i] = buffer;
       }
   }

 struct MESHINFO1 {
  uint64 p01;
  uint64 p02;
  uint64 p03;
  uint32 p04;
  char p05[1024];
  uint64 p06;
  uint64 p07;
  uint64 p08;
  uint32 p09;
  uint64 p10;
 };
 struct MESHINFO2 {
  uint64 p01;
  uint64 p02;
  uint64 p03;
  uint32 p04;
  char p05[1024];
  uint64 p06;
  uint64 p07;
  uint32 p08;
  uint32 p09;
  uint32 p10;
  uint64 p11;
  uint64 p12;
  uint32 p13;
  uint32 p14;
  uint32 p15;
  uint64 p16;
  uint64 p17;
  uint32 p18;
  uint32 p19;
  uint32 p20;
  uint64 p21;
  uint64 p22;
  uint64 p23;
  uint64 p24;
  uint32 p25;
  uint32 p26;
  uint32 p27;
  uint64 p28;
 };
 deque<MESHINFO1> meshinfo1;
 deque<MESHINFO2> meshinfo2;

 if(mesh_info_offset1 != 0xFFFFFFFF)
   {
    // mesh info #1
    ifile.seekg(mesh_info_offset1);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
   
    uint64 mi1_type = LE_read_uint64(ifile); // 0x12, 0x15
    uint64 mi1_size = LE_read_uint64(ifile);
    uint32 mi1_elem = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.", __LINE__);
   
    for(uint32 i = 0; i < mi1_elem; i++) {
        MESHINFO1 item;
        item.p01 = LE_read_uint64(ifile);
        item.p02 = LE_read_uint64(ifile);
        item.p03 = LE_read_uint64(ifile);
        item.p04 = LE_read_uint32(ifile);
        if(item.p04) read_string(ifile, &item.p05[0], item.p04);
        else item.p05[0] = '\0';
        item.p06 = LE_read_uint64(ifile);
        item.p07 = LE_read_uint64(ifile);
        item.p08 = LE_read_uint64(ifile);
        item.p09 = LE_read_uint32(ifile);
        item.p10 = LE_read_uint64(ifile);
        meshinfo1.push_back(item);
       }
   }

 if(mesh_info_offset2 != 0xFFFFFFFF)
   {
    // mesh info #2
    ifile.seekg(mesh_info_offset2);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
    
    uint64 mi2_type = LE_read_uint64(ifile); // 0x12, 0x15
    uint64 mi2_size = LE_read_uint64(ifile);
    uint32 mi2_elem = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.", __LINE__);
    
    for(uint32 i = 0; i < mi2_elem; i++) {
        MESHINFO2 item;
        item.p01 = LE_read_uint64(ifile);
        item.p02 = LE_read_uint64(ifile);
        item.p03 = LE_read_uint64(ifile);
        item.p04 = LE_read_uint32(ifile);
        if(item.p04) read_string(ifile, &item.p05[0], item.p04);
        else item.p05[0] = '\0';
        item.p06 = LE_read_uint64(ifile);
        item.p07 = LE_read_uint64(ifile);
        item.p08 = LE_read_uint32(ifile);
        item.p09 = LE_read_uint32(ifile);
        item.p10 = LE_read_uint32(ifile);
        item.p11 = LE_read_uint64(ifile);
        item.p12 = LE_read_uint64(ifile);
        item.p13 = LE_read_uint32(ifile);
        item.p14 = LE_read_uint32(ifile);
        item.p15 = LE_read_uint32(ifile);
        item.p16 = LE_read_uint64(ifile);
        item.p17 = LE_read_uint64(ifile);
        item.p18 = LE_read_uint32(ifile);
        item.p19 = LE_read_uint32(ifile);
        item.p20 = LE_read_uint32(ifile);
        item.p21 = LE_read_uint64(ifile);
        item.p22 = LE_read_uint64(ifile);
        item.p23 = LE_read_uint64(ifile);
        item.p24 = LE_read_uint64(ifile);
        item.p25 = LE_read_uint32(ifile);
        item.p26 = LE_read_uint32(ifile);
        item.p27 = LE_read_uint32(ifile);
        item.p28 = LE_read_uint64(ifile);
        meshinfo2.push_back(item);
       }
   }

 //
 // INDEX BUFFER
 // Index buffer appears immediately after surface information.
 //

 // move to index buffer
 ifile.seekg(ib_offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read number of indices
 uint32 n_indices = LE_read_uint32(ifile);
 boost::shared_array<uint16> ibdata(new uint16[n_indices]);
 if(!LE_read_array(ifile, ibdata.get(), n_indices)) return error("Read failure.", __LINE__);

 //
 // JOINTMAP
 //

 // data
 struct MKXJOINTMAP {
  uint32 size;
  boost::shared_array<uint16> data;
 };

 // move to joint map
 ifile.seekg(jm_offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read info
 uint32 jm_p01 = LE_read_uint32(ifile); // number of maps
 uint32 jm_p02 = LE_read_uint32(ifile); // 0x00
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // read maps
 deque<MKXJOINTMAP> jmlist;
 for(uint32 i = 0; i < jm_p01; i++)
    {
     // skip 0x10 bytes
     ifile.seekg(0x10, ios::cur);
     if(ifile.fail()) return error("Seek failure.", __LINE__);

     // read item
     MKXJOINTMAP item;
     item.size = LE_read_uint32(ifile);
     item.data.reset(new uint16[item.size]);
     LE_read_array(ifile, item.data.get(), item.size);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // skip 0x10 bytes
     ifile.seekg(0x10, ios::cur);
     if(ifile.fail()) return error("Seek failure.", __LINE__);

     // insert map
     jmlist.push_back(item);
    }

 //
 // VERTEX BUFFER #1
 // POINTS
 //

 struct MKXPOINT {
  real32 x, y, z;
 };

 // move to vertex buffer
 ifile.seekg(vb1_offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read size of vertex
 uint32 vb1_h01 = LE_read_uint32(ifile); // bytes per vertex
 uint32 vb1_h02 = LE_read_uint32(ifile);
 uint32 vb1_h03 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(vb1_h02 != n_verts) return error("Unexpected number of vertices.", __LINE__);
 if(vb1_h03 != n_verts) return error("Unexpected number of vertices.", __LINE__); 

 // read verts
 boost::shared_array<MKXPOINT> pointlist(new MKXPOINT[n_verts]);
 for(uint32 i = 0; i < n_verts; i++) {
     real32 x = LE_read_real32(ifile);
     real32 y = LE_read_real32(ifile);
     real32 z = LE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
     pointlist[i].x = x;
     pointlist[i].y = y;
     pointlist[i].z = z;
    }

 //
 // VERTEX BUFFER #3
 // BLENDING
 //

 struct MKXBLENDWEIGHT {
  uint08 bytes[8];
 };

 // move to vertex buffer
 ifile.seekg(vb3_offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read info
 uint16 vb3_h01 = LE_read_uint32(ifile); // bytes per vertex
 uint16 vb3_h02 = LE_read_uint32(ifile); // number of vertices
 uint16 vb3_h03 = LE_read_uint32(ifile); // number of vertices
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(vb3_h02 != n_verts) return error("Unexpected number of vertices.", __LINE__);
 if(vb3_h03 != n_verts) return error("Unexpected number of vertices.", __LINE__);

 // read data
 boost::shared_array<MKXBLENDWEIGHT> bwlist(new MKXBLENDWEIGHT[n_verts]);
 for(uint32 i = 0; i < n_verts; i++) {
     bwlist[i].bytes[0] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[1] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[2] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[3] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[4] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[5] = LE_read_uint08(ifile); // blendweight
     bwlist[i].bytes[6] = LE_read_uint08(ifile); // blendweight
     bwlist[i].bytes[7] = LE_read_uint08(ifile); // blendweight
     if(ifile.fail()) return error("Read failure.", __LINE__);
    }

 //
 // VERTEX BUFFER #4
 // UV
 //

 struct MKXUVCHANNEL {
  boost::shared_array<boost::shared_array<real32>> data;
 };

 // move to vertex buffer
 ifile.seekg(vb4_offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read size of vertex
 uint16 vb4_h01 = LE_read_uint32(ifile); // number of channels
 uint32 vb4_h02 = LE_read_uint32(ifile); // bytes per vertex
 uint32 vb4_h03 = LE_read_uint32(ifile); // number of vertices
 uint32 vb4_h04 = LE_read_uint32(ifile); // number of vertices
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // read verts
 deque<MKXUVCHANNEL> uvlist;
 if(vb4_h01 == 1)
   {
    MKXUVCHANNEL channel;
    channel.data.reset(new boost::shared_array<real32>[n_verts]);
    for(uint32 i = 0; i < vb4_h03; i++) {
        channel.data[i].reset(new real32[2]);
        real32 u = LE_read_real16(ifile);
        real32 v = LE_read_real16(ifile);
        if(ifile.fail()) return error("Read failure.", __LINE__);
        channel.data[i][0] = u;
        channel.data[i][1] = v;
       }
    uvlist.push_back(channel);
   }
 else if(vb4_h01 == 2)
   {
    // create channels
    MKXUVCHANNEL channel1;
    MKXUVCHANNEL channel2;
    channel1.data.reset(new boost::shared_array<real32>[n_verts]);
    channel2.data.reset(new boost::shared_array<real32>[n_verts]);

    // read channels
    vb4_h03 /= 2;
    vb4_h04 /= 2;
    for(uint32 i = 0; i < vb4_h03; i++) {
        real32 u1 = LE_read_real16(ifile);
        real32 v1 = LE_read_real16(ifile);
        real32 u2 = LE_read_real16(ifile);
        real32 v2 = LE_read_real16(ifile);
        if(ifile.fail()) return error("Read failure.", __LINE__);
        channel1.data[i].reset(new real32[2]);
        channel1.data[i][0] = u1;
        channel1.data[i][1] = v1;
        channel2.data[i].reset(new real32[2]);
        channel2.data[i][0] = u2;
        channel2.data[i][1] = v2;
       }
    uvlist.push_back(channel1);
    uvlist.push_back(channel2);
   }
 else
    return error("Invalid number of UV channels.");

 //
 // SURFACE INFORMATION
 //

 struct MKXMESH {
  uint16 id;
  uint16 jm;
  uint32 start;
  uint32 n_tri;
  uint08 bflag;
 };

 // move to surface information
 ifile.seekg(mesh_info_offset3);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read number of surface
 uint32 n_surfaces = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // read surfaces
 boost::shared_array<MKXMESH> meshlist(new MKXMESH[n_surfaces]);
 for(uint32 i = 0; i < n_surfaces; i++) {
     meshlist[i].id    = LE_read_uint16(ifile); // id
     meshlist[i].jm    = LE_read_uint16(ifile); // joint map index
     meshlist[i].start = LE_read_uint32(ifile); // index buffer start
     meshlist[i].n_tri = LE_read_uint32(ifile); // number of triangles
     meshlist[i].bflag = LE_read_uint08(ifile); // some kind of flag
     if(ifile.fail()) return error("Read failure.", __LINE__);
    }

 // model container
 ADVANCEDMODELCONTAINER amc;

 // prepare vertex buffer
 AMC_VTXBUFFER vb;
 vb.name = "default";
 vb.elem = n_verts;
 vb.data.reset(new AMC_VERTEX[vb.elem]);
 vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_WEIGHTS;
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

 // set UV channels
 if(vb4_h01) {
    vb.flags |= AMC_VERTEX_UV;
    vb.uvchan = vb4_h01;
    if(vb4_h01 > 0) vb.uvtype[0] = AMC_DIFFUSE_MAP;
    if(vb4_h01 > 1) vb.uvtype[1] = AMC_DIFFUSE_MAP;
   }

 // copy vertex data
 for(uint32 i = 0; i < vb.elem; i++)
    {
     // position
     vb.data[i].vx = pointlist[i].x;
     vb.data[i].vy = pointlist[i].y;
     vb.data[i].vz = pointlist[i].z;

     // uv
     for(uint32 j = 0; j < vb.uvchan; j++)  {
         vb.data[i].uv[j][0] = uvlist[j].data[i][0];
         vb.data[i].uv[j][1] = uvlist[j].data[i][1];
        }

     // blend weights
     uint16 w1 = bwlist[i].bytes[5];
     uint16 w2 = bwlist[i].bytes[6];
     uint16 w3 = bwlist[i].bytes[7];
     uint16 w4 = 0;

     // sum blend weights
     uint16 sum_weights = 0;
     sum_weights += bwlist[i].bytes[5];
     sum_weights += bwlist[i].bytes[6];
     sum_weights += bwlist[i].bytes[7];

     // need to determine how many weights there are
     uint16 n_weights = 1;
     if(w2 == 0x00) n_weights = 1;
     else if(w3 == 0x00) n_weights = 2;
     else n_weights = 3;

     // if weights don't add up to 0xFF, we have an extra weight
     if((n_weights == 3) && (sum_weights != 0xFF)) {
        w4 = 0xFF - sum_weights;
        n_weights++;
       }

     // weights
     vb.data[i].wv[0] = w1/255.0f;
     vb.data[i].wv[1] = w2/255.0f;
     vb.data[i].wv[2] = w3/255.0f;
     vb.data[i].wv[3] = w4/255.0f;
     vb.data[i].wv[4] = 0.0f;
     vb.data[i].wv[5] = 0.0f;
     vb.data[i].wv[6] = 0.0f;
     vb.data[i].wv[7] = 0.0f;
     vb.data[i].wv[8] = 0.0f;

     // blend indices
     uint16 i1 = bwlist[i].bytes[0];
     uint16 i2 = bwlist[i].bytes[1];
     uint16 i3 = bwlist[i].bytes[2];
     uint16 i4 = bwlist[i].bytes[3];
     uint16 i5 = bwlist[i].bytes[4];

     // aaaaaaaa bbbbbbbb cccccccc dddddddd eeeeeeee
     // since i2 is divisible by 4, use bbaaaaaaaa for i1
     // bbaaaaaaaa bbbbbb00 cccccccc dddddddd eeeeeeee
     // since i3 is divisible by 16, use ccccbbbbbb00 for i2
     // bbaaaaaaaa ccccbbbbbb00 cccc0000 dddddddd eeeeeeee
     // since i4 is divisible by 64, use ddddddcccc0000
     // bbaaaaaaaa ccccbbbbbb00 ddddddcccc0000 eeeeeeeedd000000 

     // compute indices
     const uint16 d2 = 0x03;
     const uint16 d3 = 0x0F;
     const uint16 d4 = 0x3F;
     uint16 p1 = (i1 | ((i2 & d2) << 8));
     uint16 p2 = (i2 | ((i3 & d3) << 8)) >> 2;
     uint16 p3 = (i3 | ((i4 & d4) << 8)) >> 4;
     uint16 p4 = (i4 | (i5 << 8)) >> 6;

     if(n_weights == 1) {
        vb.data[i].wi[0] = p1;
        vb.data[i].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
     else if(n_weights == 2) {
        vb.data[i].wi[0] = p1;
        vb.data[i].wi[1] = p2;
        vb.data[i].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
     else if(n_weights == 3)  {
        vb.data[i].wi[0] = p1;
        vb.data[i].wi[1] = p2;
        vb.data[i].wi[2] = p3;
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
     else if(n_weights == 4) {
        vb.data[i].wi[0] = p1;
        vb.data[i].wi[1] = p2;
        vb.data[i].wi[2] = p3;
        vb.data[i].wi[3] = p4;
        vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
    }

 // insert vertex buffer
 amc.vblist.push_back(vb);

 // prepare index buffer
 AMC_IDXBUFFER ib;
 ib.type = AMC_TRIANGLES;
 ib.format = AMC_UINT16;
 ib.name = "default";
 ib.wind = AMC_CCW;
 ib.elem = n_indices;
 uint32 ib_datasize = n_indices * 0x02;
 ib.data.reset(new char[ib_datasize]);
 memmove(ib.data.get(), ibdata.get(), ib_datasize);

 // insert index buffer
 amc.iblist.push_back(ib);

 // process joint maps
 for(size_t i = 0; i < jmlist.size(); i++)
    {
     // copy joint map data
     deque<uint32> jntmap;
     for(size_t j = 0; j < jmlist[i].size; j++) jntmap.push_back(jmlist[i].data[j]);

     // insert joint map
     AMC_JOINTMAP jmap;
     jmap.jntmap = jntmap;
     amc.jmaplist.push_back(jmap);
    }

 // process surfaces
 for(uint32 i = 0; i < n_surfaces; i++)
    {
     // create surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << meshlist[i].id;

     // create surface
     AMC_SURFACE surface;
     surface.name = ss.str();
     surface.surfmat = AMC_INVALID_SURFMAT;
     AMC_REFERENCE ref;
     ref.vb_index = 0;
     ref.vb_start = 0;
     ref.vb_width = n_verts;
     ref.ib_index = 0;
     ref.ib_start = meshlist[i].start;
     ref.ib_width = meshlist[i].n_tri*3;
     ref.jm_index = meshlist[i].jm; // AMC_INVALID_JMAP_INDEX
     surface.refs.push_back(ref);
     // save surface
     amc.surfaces.push_back(surface);
    }

 // transform and save AMC file
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

bool MKX_ProcessSkeleton(LPCTSTR filename, const uint64* info)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // model containter
 ADVANCEDMODELCONTAINER amc;

 //
 // FIXED PROPERTIES
 // 

 uint32 n_joints = info[0];
 uint32 jntdata_offset = info[1];
 uint32 jparent_offset = info[2];

 // 
 // PARENT DATA
 //

 // move to parent data
 ifile.seekg(jparent_offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);
 
 // read number of joints
 uint32 jp_joints = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(jp_joints != n_joints) return error("Expecting number of joints.", __LINE__);

 // read parent data
 boost::shared_array<uint16> parents(new uint16[n_joints]);
 if(!LE_read_array(ifile, parents.get(), n_joints)) return error("Read failure.", __LINE__);

 //
 // JOINT DATA
 //

 // move to joint data
 ifile.seekg(jntdata_offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read number of joints
 uint32 jd_joints = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(jd_joints != n_joints) return error("Expecting number of joints.", __LINE__);

 // initialize skeleton data
 AMC_SKELETON2 skel2;
 skel2.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel2.name = "skeleton";

 // read joint data
 for(uint32 i = 0; i < n_joints; i++)
    {
     real32 p01 = LE_read_real32(ifile); // 0x04 unit quaternion
     real32 p02 = LE_read_real32(ifile); // 0x08 unit quaternion
     real32 p03 = LE_read_real32(ifile); // 0x0C unit quaternion
     real32 p04 = LE_read_real32(ifile); // 0x10 unit quaternion
     real32 p05 = LE_read_real32(ifile); // 0x14 position
     real32 p06 = LE_read_real32(ifile); // 0x18 position
     real32 p07 = LE_read_real32(ifile); // 0x1C position

     cs::math::quaternion<real32> quat(p04, p01, p02, p03); // w, x, y, z
     cs::math::normalize(quat);
     cs::math::matrix4x4<real32> mat;
     cs::math::quaternion_to_matrix4x4(&quat[0], mat.get());

     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;
     
     // create and insert joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = (parents[i] == i ? AMC_INVALID_JOINT : parents[i]);
     joint.m_rel[0x0] = mat[0x0];
     joint.m_rel[0x1] = mat[0x1];
     joint.m_rel[0x2] = mat[0x2];
     joint.m_rel[0x3] = mat[0x3];
     joint.m_rel[0x4] = mat[0x4];
     joint.m_rel[0x5] = mat[0x5];
     joint.m_rel[0x6] = mat[0x6];
     joint.m_rel[0x7] = mat[0x7];
     joint.m_rel[0x8] = mat[0x8];
     joint.m_rel[0x9] = mat[0x9];
     joint.m_rel[0xA] = mat[0xA];
     joint.m_rel[0xB] = mat[0xB];
     joint.m_rel[0xC] = mat[0xC];
     joint.m_rel[0xD] = mat[0xD];
     joint.m_rel[0xE] = mat[0xE];
     joint.m_rel[0xF] = mat[0xF];
     joint.p_rel[0] = p05;
     joint.p_rel[1] = p06;
     joint.p_rel[2] = p07;
     joint.p_rel[3] = 1.0f;
     skel2.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel2);

 // save skeleton
 STDSTRINGSTREAM ss;
 ss << shrtname << TEXT("_skeleton");
 return SaveAMC(pathname.c_str(), ss.str().c_str(), amc);
}

bool MKX_ProcessTexture2D(const MKTEXTURE2D& info)
{
 // open file
 using namespace std;
 ifstream ifile(info.filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(info.filename.c_str()).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(info.filename.c_str()).get();

 // determine size of data
 uint32 datasize = 0;
 switch(info.format) {
   case(MK_RGBA) : datasize = UncompressedDDSFileSize(info.dx, info.dy, 0, 32); break;
   case(MK_ARGB) : datasize = UncompressedDDSFileSize(info.dx, info.dy, 0, 32); break;
   case(MK_DXT1) : datasize = DXT1Filesize(info.dx, info.dy, 0); break;
   case(MK_DXT3) : datasize = DXT3Filesize(info.dx, info.dy, 0); break;
   case(MK_DXT5) : datasize = DXT5Filesize(info.dx, info.dy, 0); break;
   case(MK_ATI2) : datasize = ATI2Filesize(info.dx, info.dy, 0); break;
   case(MK_BC7)  : datasize = GetBC7DataSize(info.dx, info.dy, 0); break;
   default : return error("Format not supported.", __LINE__);
  }

 // seek data
 ifile.seekg(info.offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read data
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // reverse byte order
 if(info.rbo == MK_REVERSE_BYTE_ORDER_2)
    reverse_byte_order(reinterpret_cast<uint16*>(data.get()), datasize/2);
 else if(info.rbo == MK_REVERSE_BYTE_ORDER_4)
    reverse_byte_order(reinterpret_cast<uint32*>(data.get()), datasize/4);
 else if(info.rbo == MK_REVERSE_BYTE_ORDER_8)
    reverse_byte_order(reinterpret_cast<uint64*>(data.get()), datasize/8);

 // swizzle?

 // create DDS header
 DDS_HEADER ddsh;
 switch(info.format) {
   case(MK_RGBA) : CreateR8G8B8A8DDSHeader(info.dx, info.dy, 0, FALSE, &ddsh); break;
   case(MK_ARGB) : CreateA8R8G8B8DDSHeader(info.dx, info.dy, 0, FALSE, &ddsh); break;
   case(MK_DXT1) : CreateDXT1Header(info.dx, info.dy, 0, FALSE, &ddsh); break;
   case(MK_DXT3) : CreateDXT3Header(info.dx, info.dy, 0, FALSE, &ddsh); break;
   case(MK_DXT5) : CreateDXT5Header(info.dx, info.dy, 0, FALSE, &ddsh); break;
   case(MK_ATI2) : CreateATI2Header(info.dx, info.dy, 0, FALSE, &ddsh); break;
   case(MK_BC7)  : CreateR8G8B8A8DDSHeader(info.dx, info.dy, 0, FALSE, &ddsh); break;
   default : return error("Format not supported.", __LINE__);
  }

 // save DDS file
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname << TEXT(".dds");
 if(info.format == MK_BC7)
   {
    // block properties
    const unsigned int blocksize = 16;
    const unsigned int block_dx = 4;
    const unsigned int block_dy = 4;
   
    // compute expected size
    unsigned int block_rows = std::max((DWORD)1, (DWORD)((info.dy + 3)/4));
    unsigned int block_cols = std::max((DWORD)1, (DWORD)((info.dx + 3)/4));
    unsigned int n_blocks = block_rows*block_cols;
    unsigned int expected_size = blocksize*n_blocks;
    if(expected_size < datasize) return false;
   
    // create RGBA data
    uint32 outpitch = 4*info.dx;
    uint32 outsize = outpitch*info.dy;
    boost::shared_array<char> outdata(new char[outsize]);
   
    // premultiplied
    uint32 left_offset[4] = { 0, outpitch, 2*outpitch, 3*outpitch };
   
    // for each block
    const char* block = data.get();
    Tile t(4, 4);
    for(uint32 i = 0; i < n_blocks; i++)
       {
        // decompress
        AVPCL::decompress(&block[0], t);
   
        // now map data to RGBA image
        uint32 base_x = (i % block_cols) * block_dx;
        uint32 base_y = (i / block_cols) * block_dy;
        uint32 base_index = 4*(info.dx*base_y + base_x);

        // convert char* to unsigned char* so we can assign 0 to 255 to items
        unsigned char* ptr = reinterpret_cast<unsigned char*>(outdata.get());

        // row1
        uint32 offset = base_index;
        ptr[offset++] = (uint08)t.data[0][0].Z();
        ptr[offset++] = (uint08)t.data[0][0].Y();
        ptr[offset++] = (uint08)t.data[0][0].X();
        ptr[offset++] = (uint08)t.data[0][0].W();
        ptr[offset++] = (uint08)t.data[0][1].Z();
        ptr[offset++] = (uint08)t.data[0][1].Y();
        ptr[offset++] = (uint08)t.data[0][1].X();
        ptr[offset++] = (uint08)t.data[0][1].W();
        ptr[offset++] = (uint08)t.data[0][2].Z();
        ptr[offset++] = (uint08)t.data[0][2].Y();
        ptr[offset++] = (uint08)t.data[0][2].X();
        ptr[offset++] = (uint08)t.data[0][2].W();
        ptr[offset++] = (uint08)t.data[0][3].Z();
        ptr[offset++] = (uint08)t.data[0][3].Y();
        ptr[offset++] = (uint08)t.data[0][3].X();
        ptr[offset]   = (uint08)t.data[0][3].W();
   
        // row2
        offset = base_index + left_offset[1];
        ptr[offset++] = (uint08)t.data[1][0].Z();
        ptr[offset++] = (uint08)t.data[1][0].Y();
        ptr[offset++] = (uint08)t.data[1][0].X();
        ptr[offset++] = (uint08)t.data[1][0].W();
        ptr[offset++] = (uint08)t.data[1][1].Z();
        ptr[offset++] = (uint08)t.data[1][1].Y();
        ptr[offset++] = (uint08)t.data[1][1].X();
        ptr[offset++] = (uint08)t.data[1][1].W();
        ptr[offset++] = (uint08)t.data[1][2].Z();
        ptr[offset++] = (uint08)t.data[1][2].Y();
        ptr[offset++] = (uint08)t.data[1][2].X();
        ptr[offset++] = (uint08)t.data[1][2].W();
        ptr[offset++] = (uint08)t.data[1][3].Z();
        ptr[offset++] = (uint08)t.data[1][3].Y();
        ptr[offset++] = (uint08)t.data[1][3].X();
        ptr[offset]   = (uint08)t.data[1][3].W();
   
        // row3
        offset = base_index + left_offset[2];
        ptr[offset++] = (uint08)t.data[2][0].Z();
        ptr[offset++] = (uint08)t.data[2][0].Y();
        ptr[offset++] = (uint08)t.data[2][0].X();
        ptr[offset++] = (uint08)t.data[2][0].W();
        ptr[offset++] = (uint08)t.data[2][1].Z();
        ptr[offset++] = (uint08)t.data[2][1].Y();
        ptr[offset++] = (uint08)t.data[2][1].X();
        ptr[offset++] = (uint08)t.data[2][1].W();
        ptr[offset++] = (uint08)t.data[2][2].Z();
        ptr[offset++] = (uint08)t.data[2][2].Y();
        ptr[offset++] = (uint08)t.data[2][2].X();
        ptr[offset++] = (uint08)t.data[2][2].W();
        ptr[offset++] = (uint08)t.data[2][3].Z();
        ptr[offset++] = (uint08)t.data[2][3].Y();
        ptr[offset++] = (uint08)t.data[2][3].X();
        ptr[offset]   = (uint08)t.data[2][3].W();
   
        // row4
        offset = base_index + left_offset[3];
        ptr[offset++] = (uint08)t.data[3][0].Z();
        ptr[offset++] = (uint08)t.data[3][0].Y();
        ptr[offset++] = (uint08)t.data[3][0].X();
        ptr[offset++] = (uint08)t.data[3][0].W();
        ptr[offset++] = (uint08)t.data[3][1].Z();
        ptr[offset++] = (uint08)t.data[3][1].Y();
        ptr[offset++] = (uint08)t.data[3][1].X();
        ptr[offset++] = (uint08)t.data[3][1].W();
        ptr[offset++] = (uint08)t.data[3][2].Z();
        ptr[offset++] = (uint08)t.data[3][2].Y();
        ptr[offset++] = (uint08)t.data[3][2].X();
        ptr[offset++] = (uint08)t.data[3][2].W();
        ptr[offset++] = (uint08)t.data[3][3].Z();
        ptr[offset++] = (uint08)t.data[3][3].Y();
        ptr[offset++] = (uint08)t.data[3][3].X();
        ptr[offset]   = (uint08)t.data[3][3].W();
   
        // move to next block
        block += blocksize;
       }

    SaveDDSFile(ss.str().c_str(), ddsh, outdata, outsize);
   }
 else
    SaveDDSFile(ss.str().c_str(), ddsh, data, datasize);

 return true;
}

sint64 ProcessEnumProperty(uint32 size, const char* data)
{
 if(size == 1) return *reinterpret_cast<const sint08*>(data);
 if(size == 2) return *reinterpret_cast<const sint16*>(data);
 if(size == 4) return *reinterpret_cast<const sint32*>(data);
 if(size == 8) return *reinterpret_cast<const sint64*>(data);
 return 0;
}

bool ProcessBoolProperty(uint32 size, const char* data)
{
 if(size == 1) return ((*reinterpret_cast<const sint08*>(data)) != 0);
 if(size == 2) return ((*reinterpret_cast<const sint16*>(data)) != 0);
 if(size == 4) return ((*reinterpret_cast<const sint32*>(data)) != 0);
 return false;
}

sint64 ProcessIntProperty(uint32 size, const char* data)
{
 if(size == 1) return *reinterpret_cast<const sint08*>(data);
 if(size == 2) return *reinterpret_cast<const sint16*>(data);
 if(size == 4) return *reinterpret_cast<const sint32*>(data);
 if(size == 8) return *reinterpret_cast<const sint64*>(data);
 return 0;
}

bool MKX_ProcessTexture2D(LPCTSTR filename, const std::map<uint64, std::string>& hashtable)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // necessary data
 uint32 SizeX = 0xFFFFFFFFul;
 uint32 SizeY = 0xFFFFFFFFul;
 uint32 Format = 0xFFFFFFFFul;
 uint32 MipTailBaseIdx = 0xFFFFFFFFul;
 uint32 bIsStreamingTexture = false;

 // read data
 bool debug = false;
 for(;;)
    {
     if(debug) cout << "offset = 0x" << hex << (sint64)ifile.tellg() << dec << endl;

     // read property name
     uint64 name = LE_read_uint64(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // lookup name in hashtable
     auto iter = hashtable.find(name);
     if(iter == hashtable.end()) return error("Failed to find property name.");
     std::string namestr = iter->second;
     if(debug) std::cout << "0x" << hex << name << dec << ", " << iter->second << std::endl;

     // "None"
     if(namestr == "None")
        break;

     // read other property parameters
     uint64 type = LE_read_uint64(ifile);
     uint32 size = LE_read_uint32(ifile);
     uint32 indx = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // lookup type in hashtable
     iter = hashtable.find(type);
     if(iter == hashtable.end()) return error("Failed to find property type.");
     std::string typestr = iter->second;
     if(debug) std::cout << "0x" << hex << type << dec << ", " << iter->second << std::endl;

     // WTF LOL???
     if(size == 0) {
        if(typestr == "BoolProperty") size = 4;
       }

     // read property data
     boost::shared_array<char> data;
     if(size) {
        data.reset(new char[size]);
        ifile.read(data.get(), size);
        if(ifile.fail()) return error("Read failure.", __LINE__);
       }

     // "SizeX"
     if(namestr == "SizeX") {
        if(typestr == "IntProperty") SizeX = ProcessIntProperty(size, data.get());
        else return error("SizeX must use IntProperty.");
       }
     // "SizeY"
     else if(namestr == "SizeY") {
        if(typestr == "IntProperty") SizeY = ProcessIntProperty(size, data.get());
        else return error("SizeY must use IntProperty.");
       }
     // "Format"
     else if(namestr == "Format") {
        if(typestr == "EnumProperty") Format = ProcessEnumProperty(size, data.get());
        else return error("Format must use EnumProperty.");
       }
     // bIsStreamingTexture
     else if(namestr == "bIsStreamingTexture") {
        if(typestr == "BoolProperty") bIsStreamingTexture = ProcessBoolProperty(size, data.get());
        else return error("bIsStreamingTexture must use BoolProperty.");
      }
    }

 // must have valid values
 if(SizeX == 0xFFFFFFFFul) return error("Invalid SizeX.");
 if(SizeY == 0xFFFFFFFFul) return error("Invalid SizeY.");
 if(Format == 0xFFFFFFFFul) return error("Invalid Format.");

 // skip 0x38 bytes
 ifile.seekg(0x38, ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // skip 0x60 bytes for streaming texture
 if(bIsStreamingTexture) {
    ifile.seekg(0x60, ios::cur);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
    cout << "Warning: Skipping streaming texture. TFC streaming is not supported" << endl;
    return true;
   }

 // datasizes
 uint32 size1 = 0;
 uint32 size2 = 0;

 // read 3 uint32
 uint32 p1 = LE_read_uint32(ifile); // 0x00
 uint32 p2 = LE_read_uint32(ifile); // 0x09, 0x0A, ...
 uint32 p3 = LE_read_uint32(ifile); // 0x00 or 0x21?
 if(p1 != 0x00) return error("Expecting 0x00.", __LINE__);

 if(p3 == 0x00) {
    cout << "p3 = 0x00" << endl;
   }
 else if(p3 == 0x21) {
    cout << "p3 = 0x21" << endl;
    ifile.seekg(0x14, ios::cur);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
    LE_read_uint32(ifile); // dx 
    LE_read_uint32(ifile); // dy
    LE_read_uint32(ifile); // 0x00
   }
 else {
    cout << "p3 = ???" << endl;
    return error("Shit", __LINE__);
   }

 // read data sizes
 size1 = LE_read_uint32(ifile);
 size2 = LE_read_uint32(ifile);
 if(size1 != size2) return error("Data sizes should be the same.", __LINE__);

 // skip 0x0C bytes
 ifile.seekg(0x0C, ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // format
 if(Format == 0x16)
   {
    // get data size
    uint32 datasize = GetBC7DataSize(SizeX, SizeY, 0);
    if(!datasize) return error("Invalid datasize.", __LINE__);

    // something wrong with the datasize!
    if(datasize != size1) {
       cout << "Warning! Texture datasize does not match with computed size." << endl;
       if(datasize == (4*size1)) {
          datasize /= 4;
          SizeX /= 2;
          SizeY /= 2;
         }
       else
          return error("Image is scaled by more than 1/4.", __LINE__);
      }

    // read data
    boost::shared_array<char> data(new char[datasize]);
    ifile.read(data.get(), datasize);
    if(ifile.fail()) return error("Read failure.", __LINE__);

    // block properties
    const unsigned int blocksize = 16;
    const unsigned int block_dx = 4;
    const unsigned int block_dy = 4;
   
    // compute expected size
    unsigned int block_rows = std::max((DWORD)1, (DWORD)((SizeY + 3)/4));
    unsigned int block_cols = std::max((DWORD)1, (DWORD)((SizeX + 3)/4));
    unsigned int n_blocks = block_rows*block_cols;
    unsigned int expected_size = blocksize*n_blocks;
    if(expected_size < datasize) return false;
   
    // create RGBA data
    uint32 outpitch = 4*SizeX;
    uint32 outsize = outpitch*SizeY;
    boost::shared_array<char> outdata(new char[outsize]);
   
    // premultiplied
    uint32 left_offset[4] = { 0, outpitch, 2*outpitch, 3*outpitch };
   
    // for each block
    const char* block = data.get();
    Tile t(4, 4);
    for(uint32 i = 0; i < n_blocks; i++)
       {
        // decompress
        AVPCL::decompress(&block[0], t);
   
        // now map data to RGBA image
        uint32 base_x = (i % block_cols) * block_dx;
        uint32 base_y = (i / block_cols) * block_dy;
        uint32 base_index = 4*(SizeX*base_y + base_x);

        // convert char* to unsigned char* so we can assign 0 to 255 to items
        unsigned char* ptr = reinterpret_cast<unsigned char*>(outdata.get());

        // row1
        uint32 offset = base_index;
        ptr[offset++] = (uint08)t.data[0][0].Z();
        ptr[offset++] = (uint08)t.data[0][0].Y();
        ptr[offset++] = (uint08)t.data[0][0].X();
        ptr[offset++] = (uint08)t.data[0][0].W();
        ptr[offset++] = (uint08)t.data[0][1].Z();
        ptr[offset++] = (uint08)t.data[0][1].Y();
        ptr[offset++] = (uint08)t.data[0][1].X();
        ptr[offset++] = (uint08)t.data[0][1].W();
        ptr[offset++] = (uint08)t.data[0][2].Z();
        ptr[offset++] = (uint08)t.data[0][2].Y();
        ptr[offset++] = (uint08)t.data[0][2].X();
        ptr[offset++] = (uint08)t.data[0][2].W();
        ptr[offset++] = (uint08)t.data[0][3].Z();
        ptr[offset++] = (uint08)t.data[0][3].Y();
        ptr[offset++] = (uint08)t.data[0][3].X();
        ptr[offset]   = (uint08)t.data[0][3].W();
   
        // row2
        offset = base_index + left_offset[1];
        ptr[offset++] = (uint08)t.data[1][0].Z();
        ptr[offset++] = (uint08)t.data[1][0].Y();
        ptr[offset++] = (uint08)t.data[1][0].X();
        ptr[offset++] = (uint08)t.data[1][0].W();
        ptr[offset++] = (uint08)t.data[1][1].Z();
        ptr[offset++] = (uint08)t.data[1][1].Y();
        ptr[offset++] = (uint08)t.data[1][1].X();
        ptr[offset++] = (uint08)t.data[1][1].W();
        ptr[offset++] = (uint08)t.data[1][2].Z();
        ptr[offset++] = (uint08)t.data[1][2].Y();
        ptr[offset++] = (uint08)t.data[1][2].X();
        ptr[offset++] = (uint08)t.data[1][2].W();
        ptr[offset++] = (uint08)t.data[1][3].Z();
        ptr[offset++] = (uint08)t.data[1][3].Y();
        ptr[offset++] = (uint08)t.data[1][3].X();
        ptr[offset]   = (uint08)t.data[1][3].W();
   
        // row3
        offset = base_index + left_offset[2];
        ptr[offset++] = (uint08)t.data[2][0].Z();
        ptr[offset++] = (uint08)t.data[2][0].Y();
        ptr[offset++] = (uint08)t.data[2][0].X();
        ptr[offset++] = (uint08)t.data[2][0].W();
        ptr[offset++] = (uint08)t.data[2][1].Z();
        ptr[offset++] = (uint08)t.data[2][1].Y();
        ptr[offset++] = (uint08)t.data[2][1].X();
        ptr[offset++] = (uint08)t.data[2][1].W();
        ptr[offset++] = (uint08)t.data[2][2].Z();
        ptr[offset++] = (uint08)t.data[2][2].Y();
        ptr[offset++] = (uint08)t.data[2][2].X();
        ptr[offset++] = (uint08)t.data[2][2].W();
        ptr[offset++] = (uint08)t.data[2][3].Z();
        ptr[offset++] = (uint08)t.data[2][3].Y();
        ptr[offset++] = (uint08)t.data[2][3].X();
        ptr[offset]   = (uint08)t.data[2][3].W();
   
        // row4
        offset = base_index + left_offset[3];
        ptr[offset++] = (uint08)t.data[3][0].Z();
        ptr[offset++] = (uint08)t.data[3][0].Y();
        ptr[offset++] = (uint08)t.data[3][0].X();
        ptr[offset++] = (uint08)t.data[3][0].W();
        ptr[offset++] = (uint08)t.data[3][1].Z();
        ptr[offset++] = (uint08)t.data[3][1].Y();
        ptr[offset++] = (uint08)t.data[3][1].X();
        ptr[offset++] = (uint08)t.data[3][1].W();
        ptr[offset++] = (uint08)t.data[3][2].Z();
        ptr[offset++] = (uint08)t.data[3][2].Y();
        ptr[offset++] = (uint08)t.data[3][2].X();
        ptr[offset++] = (uint08)t.data[3][2].W();
        ptr[offset++] = (uint08)t.data[3][3].Z();
        ptr[offset++] = (uint08)t.data[3][3].Y();
        ptr[offset++] = (uint08)t.data[3][3].X();
        ptr[offset]   = (uint08)t.data[3][3].W();
   
        // move to next block
        block += blocksize;
       }

    // create DDS header
    DDS_HEADER ddsh;
    CreateA8R8G8B8DDSHeader(SizeX, SizeY, 0, FALSE, &ddsh);

    // save DDS file
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT(".dds");
    SaveDDSFile(ss.str().c_str(), ddsh, outdata, outsize);
   }
 else
    return error("Unsupported texture format.");

 return true;
}

bool MKX_ProcessTexture2DList(LPCTSTR pathname, const std::map<uint64, std::string>& hashtable)
{
 // build a list of Texture2D files
 cout << "Searching for Texture2D files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".Texture2D"), pathname);
 if(!filelist.size()) return true;

 // process Texture2D files
 cout << "Processing Texture2D files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     MKX_ProcessTexture2D(filelist[i].c_str(), hashtable);
    }
 cout << endl;
 return true;
}

bool MKX_ProcessSkeleton(LPCTSTR filename, const std::map<uint64, std::string>& hashtable)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read data
 bool debug = false;
 uint64 NoneValue = 0xFFFFFFFFul;
 for(;;)
    {
     if(debug) cout << "offset = 0x" << hex << (sint64)ifile.tellg() << dec << endl;

     // read property name
     uint64 name = LE_read_uint64(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // lookup name in hashtable
     auto iter = hashtable.find(name);
     if(iter == hashtable.end()) return error("Failed to find property name.");
     std::string namestr = iter->second;
     if(debug) std::cout << "0x" << hex << name << dec << ", " << iter->second << std::endl;

     // "None"
     if(namestr == "None") {
        NoneValue = name;
        break;
       }

     // read other property parameters
     uint64 type = LE_read_uint64(ifile);
     uint32 size = LE_read_uint32(ifile);
     uint32 indx = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // lookup type in hashtable
     iter = hashtable.find(type);
     if(iter == hashtable.end()) return error("Failed to find property type.");
     std::string typestr = iter->second;
     if(debug) std::cout << "0x" << hex << type << dec << ", " << iter->second << std::endl;

     // WTF LOL???
     if(size == 0) {
        if(typestr == "BoolProperty") size = 4;
       }

     // read property data
     boost::shared_array<char> data;
     if(size) {
        data.reset(new char[size]);
        ifile.read(data.get(), size);
        if(ifile.fail()) return error("Read failure.", __LINE__);
       }
    }

 // save offset
 uint64 offset = (uint64)ifile.tellg();
 uint64 repeat = LE_read_uint64(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // "None" appears twice for skeleton files, but if it doesn't, be prepared
 if(repeat != NoneValue) {
    ifile.seekg(offset);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
   }

 // after all the data is read, we are at the right place
 // to read the bone data

 //
 // JOINT DATA
 //

 // read number of joints
 uint32 n_joints1 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(!n_joints1) return error("Invalid number of joints.");

 if(debug) cout << "N joints = 0x" << hex << n_joints1 << dec << endl;

 // read joint data
 uint32 jsize = 0x1C * n_joints1;
 boost::shared_array<char> jdata(new char[jsize]);
 ifile.read(jdata.get(), jsize);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // 
 // PARENT DATA
 //

 // read number of joints
 uint32 n_joints2 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(n_joints2 != n_joints1) return error("Expecting number of joints.", __LINE__);

 // read parent data
 boost::shared_array<uint16> parents(new uint16[n_joints2]);
 if(!LE_read_array(ifile, parents.get(), n_joints2)) return error("Read failure.", __LINE__);

 //
 // PROCESS JOINT DATA
 //

 // binary stream
 ADVANCEDMODELCONTAINER amc;
 binary_stream bs(jdata, jsize);

 // initialize skeleton data
 AMC_SKELETON2 skel2;
 skel2.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel2.name = "skeleton";

 // read joint data
 for(uint32 i = 0; i < n_joints1; i++)
    {
     real32 p01 = bs.LE_read_real32(); // 0x04 unit quaternion
     real32 p02 = bs.LE_read_real32(); // 0x08 unit quaternion
     real32 p03 = bs.LE_read_real32(); // 0x0C unit quaternion
     real32 p04 = bs.LE_read_real32(); // 0x10 unit quaternion
     real32 p05 = bs.LE_read_real32(); // 0x14 position
     real32 p06 = bs.LE_read_real32(); // 0x18 position
     real32 p07 = bs.LE_read_real32(); // 0x1C position

     cs::math::quaternion<real32> quat(p04, p01, p02, p03); // w, x, y, z
     cs::math::normalize(quat);
     cs::math::matrix4x4<real32> mat;
     cs::math::quaternion_to_matrix4x4(&quat[0], mat.get());

     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;
     
     // create and insert joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = (parents[i] == i ? AMC_INVALID_JOINT : parents[i]);
     joint.m_rel[0x0] = mat[0x0];
     joint.m_rel[0x1] = mat[0x1];
     joint.m_rel[0x2] = mat[0x2];
     joint.m_rel[0x3] = mat[0x3];
     joint.m_rel[0x4] = mat[0x4];
     joint.m_rel[0x5] = mat[0x5];
     joint.m_rel[0x6] = mat[0x6];
     joint.m_rel[0x7] = mat[0x7];
     joint.m_rel[0x8] = mat[0x8];
     joint.m_rel[0x9] = mat[0x9];
     joint.m_rel[0xA] = mat[0xA];
     joint.m_rel[0xB] = mat[0xB];
     joint.m_rel[0xC] = mat[0xC];
     joint.m_rel[0xD] = mat[0xD];
     joint.m_rel[0xE] = mat[0xE];
     joint.m_rel[0xF] = mat[0xF];
     joint.p_rel[0] = p05;
     joint.p_rel[1] = p06;
     joint.p_rel[2] = p07;
     joint.p_rel[3] = 1.0f;
     skel2.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel2);

 // save skeleton
 STDSTRINGSTREAM ss;
 ss << shrtname << TEXT("_skeleton");
 return SaveAMC(pathname.c_str(), ss.str().c_str(), amc);
}

bool MKX_ProcessSkeletonList(LPCTSTR pathname, const std::map<uint64, std::string>& hashtable)
{
 // build a list of Skeleton files
 cout << "Searching for Skeleton files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".Skeleton"), pathname);
 if(!filelist.size()) return true;

 // process Skeleton files
 cout << "Processing Skeleton files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!MKX_ProcessSkeleton(filelist[i].c_str(), hashtable)) return false;
    }
 cout << endl;
 return true;
}

bool DecodeBC7(const char* data, size_t size, unsigned int dx, unsigned int dy)
{
 // simple test cases
 if(!data) return false;
 if(!size) return false;
 if(!dx || !dy) return false;

 // block properties
 const unsigned int blocksize = 16;
 const unsigned int block_dx = 4;
 const unsigned int block_dy = 4;

 // compute expected size
 unsigned int block_rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 unsigned int block_cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 unsigned int n_blocks = block_rows*block_cols;
 unsigned int expected_size = blocksize*n_blocks;
 if(expected_size < size) return false;

 // for each block
 const unsigned char* block = reinterpret_cast<const unsigned char*>(data);
 for(unsigned int i = 0; i < n_blocks; i++)
    {
     // BC7 can specify one of 8 modes, with the mode specified in the least
     // significant bit of the 16-byte (128-bit) block. The mode is encoded
     // by zero or more bits with a value of 0 followed by a 1. Mode 0 is a
     // reserved value and should not be used.
     unsigned char mode_byte = block[15];
     unsigned char mode = 8;
     if(mode_byte & 0x01) mode = 0;      // 00000001
     else if(mode_byte & 0x02) mode = 1; // 00000010
     else if(mode_byte & 0x04) mode = 2; // 00000100
     else if(mode_byte & 0x08) mode = 3; // 00001000
     else if(mode_byte & 0x10) mode = 4; // 00010000
     else if(mode_byte & 0x20) mode = 5; // 00100000
     else if(mode_byte & 0x40) mode = 6; // 01000000
     else if(mode_byte & 0x80) mode = 7; // 10000000

     // mode 0
     if(mode == 0)
       {
        // 1 subset bit
        // 4 partition bits
        // 72 endpoint bits
        // 6 P bits
        // 45 index bits
        // subsets: 3
        // there is always two endpoints per subset
        // S1: 4 bits R endpoint 0 - 4 bits R endpoint 1
        // S2: 4 bits R endpoint 0 - 4 bits R endpoint 1
        // S3: 4 bits R endpoint 0 - 4 bits R endpoint 1
        // S1: 4 bits G endpoint 0 - 4 bits G endpoint 1
        // S2: 4 bits G endpoint 0 - 4 bits G endpoint 1
        // S3: 4 bits G endpoint 0 - 4 bits G endpoint 1
        // S1: 4 bits B endpoint 0 - 4 bits B endpoint 1
        // S2: 4 bits B endpoint 0 - 4 bits B endpoint 1
        // S3: 4 bits B endpoint 0 - 4 bits B endpoint 1

        const unsigned int n_subsets = 3;
        unsigned char partition_bits = (mode_byte & 0x1E) >> 1;

       }
     // mode 1
     else if(mode == 1)
       {
       }
     // mode 2
     else if(mode == 2)
       {
       }
     // mode 3
     else if(mode == 3)
       {
       }
     // mode 4
     else if(mode == 4)
       {
       }
     // mode 5
     else if(mode == 5)
       {
       }
     // mode 6
     else if(mode == 6)
       {
       }
     // mode 7
     else if(mode == 7)
       {
       }
     // mode 8
     else
       {
        // TODO:
        // Just set everything to zero.
       }

     // move to next block
     block += blocksize;
    }

 return true;
}

bool TestDecodeBC7(const char* data, size_t size, unsigned int dx, unsigned int dy)
{
 // simple test cases
 if(!data) return false;
 if(!size) return false;
 if(!dx || !dy) return false;

 // block properties
 const unsigned int blocksize = 16;
 const unsigned int block_dx = 4;
 const unsigned int block_dy = 4;

 // compute expected size
 unsigned int block_rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 unsigned int block_cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 unsigned int n_blocks = block_rows*block_cols;
 unsigned int expected_size = blocksize*n_blocks;
 if(expected_size < size) return false;

 // create RGBA data
 uint32 outpitch = 4*dx;
 uint32 outsize = outpitch*dy;
 boost::shared_array<unsigned char> outdata(new unsigned char[outsize]);

 // premultiplied
 uint32 left_offset[4] = { 0, outpitch, 2*outpitch, 3*outpitch };

 // for each block
 const char* block = data;
 Tile t(4, 4);
 for(uint32 i = 0; i < n_blocks; i++)
    {
     // decompress
     AVPCL::decompress(&block[0], t);

     // now map data to RGBA image
     uint32 base_x = (i % block_cols) * block_dx;
     uint32 base_y = (i % block_rows) * block_dy;
     uint32 base_index = 4*(dx*base_y + base_x);

     // row1
     uint32 offset = base_index;
     outdata[offset++] = (uint32)t.data[0][0].X();
     outdata[offset++] = (uint32)t.data[0][0].Y();
     outdata[offset++] = (uint32)t.data[0][0].Z();
     outdata[offset++] = (uint32)t.data[0][0].W();
     outdata[offset++] = (uint32)t.data[0][1].X();
     outdata[offset++] = (uint32)t.data[0][1].Y();
     outdata[offset++] = (uint32)t.data[0][1].Z();
     outdata[offset++] = (uint32)t.data[0][1].W();
     outdata[offset++] = (uint32)t.data[0][2].X();
     outdata[offset++] = (uint32)t.data[0][2].Y();
     outdata[offset++] = (uint32)t.data[0][2].Z();
     outdata[offset++] = (uint32)t.data[0][2].W();
     outdata[offset++] = (uint32)t.data[0][3].X();
     outdata[offset++] = (uint32)t.data[0][3].Y();
     outdata[offset++] = (uint32)t.data[0][3].Z();
     outdata[offset]   = (uint32)t.data[0][3].W();

     // row2
     offset = base_index + left_offset[1];
     outdata[offset++] = (uint32)t.data[1][0].X();
     outdata[offset++] = (uint32)t.data[1][0].Y();
     outdata[offset++] = (uint32)t.data[1][0].Z();
     outdata[offset++] = (uint32)t.data[1][0].W();
     outdata[offset++] = (uint32)t.data[1][1].X();
     outdata[offset++] = (uint32)t.data[1][1].Y();
     outdata[offset++] = (uint32)t.data[1][1].Z();
     outdata[offset++] = (uint32)t.data[1][1].W();
     outdata[offset++] = (uint32)t.data[1][2].X();
     outdata[offset++] = (uint32)t.data[1][2].Y();
     outdata[offset++] = (uint32)t.data[1][2].Z();
     outdata[offset++] = (uint32)t.data[1][2].W();
     outdata[offset++] = (uint32)t.data[1][3].X();
     outdata[offset++] = (uint32)t.data[1][3].Y();
     outdata[offset++] = (uint32)t.data[1][3].Z();
     outdata[offset]   = (uint32)t.data[1][3].W();

     // row3
     offset = base_index + left_offset[2];
     outdata[offset++] = (uint32)t.data[2][0].X();
     outdata[offset++] = (uint32)t.data[2][0].Y();
     outdata[offset++] = (uint32)t.data[2][0].Z();
     outdata[offset++] = (uint32)t.data[2][0].W();
     outdata[offset++] = (uint32)t.data[2][1].X();
     outdata[offset++] = (uint32)t.data[2][1].Y();
     outdata[offset++] = (uint32)t.data[2][1].Z();
     outdata[offset++] = (uint32)t.data[2][1].W();
     outdata[offset++] = (uint32)t.data[2][2].X();
     outdata[offset++] = (uint32)t.data[2][2].Y();
     outdata[offset++] = (uint32)t.data[2][2].Z();
     outdata[offset++] = (uint32)t.data[2][2].W();
     outdata[offset++] = (uint32)t.data[2][3].X();
     outdata[offset++] = (uint32)t.data[2][3].Y();
     outdata[offset++] = (uint32)t.data[2][3].Z();
     outdata[offset]   = (uint32)t.data[2][3].W();

     // row4
     offset = base_index + left_offset[3];
     outdata[offset++] = (uint32)t.data[3][0].X();
     outdata[offset++] = (uint32)t.data[3][0].Y();
     outdata[offset++] = (uint32)t.data[3][0].Z();
     outdata[offset++] = (uint32)t.data[3][0].W();
     outdata[offset++] = (uint32)t.data[3][1].X();
     outdata[offset++] = (uint32)t.data[3][1].Y();
     outdata[offset++] = (uint32)t.data[3][1].Z();
     outdata[offset++] = (uint32)t.data[3][1].W();
     outdata[offset++] = (uint32)t.data[3][2].X();
     outdata[offset++] = (uint32)t.data[3][2].Y();
     outdata[offset++] = (uint32)t.data[3][2].Z();
     outdata[offset++] = (uint32)t.data[3][2].W();
     outdata[offset++] = (uint32)t.data[3][3].X();
     outdata[offset++] = (uint32)t.data[3][3].Y();
     outdata[offset++] = (uint32)t.data[3][3].Z();
     outdata[offset]   = (uint32)t.data[3][3].W();

     // move to next block
     block += blocksize;
    }

 return true;
}

bool MKX_ProcessSkeletalMeshList(LPCTSTR pathname, const std::map<uint64, std::string>& hashtable)
{
 // build a list of SkeletalMesh files
 cout << "Searching for SkeletalMesh files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".SkeletalMesh"), pathname);
 if(!filelist.size()) return true;

 // process SkeletalMesh files
 cout << "Processing SkeletalMesh files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!MKX_ProcessSkeletalMesh(filelist[i].c_str(), hashtable)) return false;
    }
 cout << endl;
 return true;
}

bool MKX_ProcessSkeletalMesh(LPCTSTR filename, const std::map<uint64, std::string>& hashtable)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read data
 bool debug = true;
 uint64 NoneValue = 0xFFFFFFFFul;
 for(;;)
    {
     if(debug) cout << "offset = 0x" << hex << (sint64)ifile.tellg() << dec << endl;

     // read property name
     uint64 name = LE_read_uint64(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // lookup name in hashtable
     auto iter = hashtable.find(name);
     if(iter == hashtable.end()) return error("Failed to find property name.");
     std::string namestr = iter->second;
     if(debug) std::cout << "0x" << hex << name << dec << ", " << iter->second << std::endl;

     // "None"
     if(namestr == "None") {
        NoneValue = name;
        break;
       }

     // read other property parameters
     uint64 type = LE_read_uint64(ifile);
     uint32 size = LE_read_uint32(ifile);
     uint32 indx = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // lookup type in hashtable
     iter = hashtable.find(type);
     if(iter == hashtable.end()) return error("Failed to find property type.");
     std::string typestr = iter->second;
     if(debug) std::cout << "0x" << hex << type << dec << ", " << iter->second << std::endl;

     // WTF LOL???
     if(size == 0) {
        if(typestr == "BoolProperty") size = 4;
       }

     // StumpTint
     if(namestr == "StumpTint" && typestr == "StructProperty") {
        LE_read_uint32(ifile);
        LE_read_uint32(ifile);
       }

     // read property data
     boost::shared_array<char> data;
     if(size) {
        data.reset(new char[size]);
        ifile.read(data.get(), size);
        if(ifile.fail()) return error("Read failure.", __LINE__);
       }

     // Skeleton has extra data
     if(namestr == "Skeleton" && typestr == "ObjectProperty") {
        uint32 objsize = LE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.", __LINE__);
        // ifile.seekg(objsize + 1, ios::cur);
        // if(ifile.fail()) return error("Seek failure.", __LINE__);
        ifile.seekg(0x38, ios::cur);
        if(ifile.fail()) return error("Seek failure.", __LINE__);
       }
    }

 // when done we are at the mesh table!

 //
 // SURFACE INFORMATION
 //

 struct MKXMESH {
  uint16 id;
  uint16 jm;
  uint32 start;
  uint32 n_tri;
  uint08 bflag;
 };

 // should be 0x01
 uint64 unk01 = LE_read_uint64(ifile);
 if(ifile.fail()) return error("Seek failure.", __LINE__);
 if(unk01 != 0x01) return error("Expecting 0x01.", __LINE__);

 // read number of surface
 uint32 n_surfaces = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // read surfaces
 boost::shared_array<MKXMESH> meshlist(new MKXMESH[n_surfaces]);
 for(uint32 i = 0; i < n_surfaces; i++) {
     meshlist[i].id    = LE_read_uint16(ifile); // id
     meshlist[i].jm    = LE_read_uint16(ifile); // joint map index
     meshlist[i].start = LE_read_uint32(ifile); // index buffer start
     meshlist[i].n_tri = LE_read_uint32(ifile); // number of triangles
     meshlist[i].bflag = LE_read_uint08(ifile); // some kind of flag
     if(ifile.fail()) return error("Read failure.", __LINE__);
    }

 //
 // INDEX BUFFER
 // Index buffer appears immediately after surface information.
 //

 // read number of indices
 uint32 n_indices = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // read indices
 boost::shared_array<uint16> ibdata(new uint16[n_indices]);
 if(!LE_read_array(ifile, ibdata.get(), n_indices)) return error("Read failure.", __LINE__);

 //
 // JOINT MAP PREFACE JUNK
 //

 // read unknown (uint64 = 0x00)
 uint64 unk02 = LE_read_uint64(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(unk02 != 0x00) return error("Expecting 0x00.", __LINE__);

 // read unknown data (uint32 + uint16[])
 uint32 unk03 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 ifile.seekg(unk03 * sizeof(uint16), ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read unknown (uint32 = 0x00)
 uint32 unk04 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(unk04 != 0x00) return error("Expecting 0x00.", __LINE__);

 //
 // JOINT MAP
 //

 // data
 struct MKXJOINTMAP {
  uint32 size;
  boost::shared_array<uint16> data;
 };

 // read info
 uint32 jm_p01 = LE_read_uint32(ifile); // number of maps
 uint32 jm_p02 = LE_read_uint32(ifile); // 0x00
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // read maps
 deque<MKXJOINTMAP> jmlist;
 for(uint32 i = 0; i < jm_p01; i++)
    {
     // skip 0x10 bytes
     ifile.seekg(0x10, ios::cur);
     if(ifile.fail()) return error("Seek failure.", __LINE__);

     // read item
     MKXJOINTMAP item;
     item.size = LE_read_uint32(ifile);
     item.data.reset(new uint16[item.size]);
     LE_read_array(ifile, item.data.get(), item.size);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // skip 0x10 bytes
     ifile.seekg(0x10, ios::cur);
     if(ifile.fail()) return error("Seek failure.", __LINE__);

     // insert map
     jmlist.push_back(item);
    }

 // debug
 if(debug) cout << "at offset 0x" << hex << (uint64)ifile.tellg() << dec << endl;

 //
 // VERTEX BUFFER PREFACE CRAP
 //

 // read number of vertices
 uint32 n_verts1 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // read unknown (uint32 = 0x00)
 uint32 unk05 = LE_read_uint32(ifile);
 if(unk05 != 0x00) return error("Expecting 0x00.", __LINE__);

 // read unknown (uint32 + uint08[])
 uint32 unk06 = LE_read_uint32(ifile);
 if(unk06 == 0x00) return error("Not expecting 0x00.", __LINE__);
 ifile.seekg(unk06 * sizeof(uint08), ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read unknown (uint32 = 0x00)
 uint32 unk07 = LE_read_uint32(ifile);
 if(unk07 != 0x00) return error("Expecting 0x00.", __LINE__);

 // read number of vertices again
 uint32 n_verts2 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(n_verts1 != n_verts2) return error("Expecting number of vertices to match.");

 // read number of bytes to skip (an array of indices, one index per vertex)
 uint32 n_verts2_size = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // skip 0x0C bytes
 ifile.seekg(0x0C, ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // skip n_vertex2_size
 ifile.seekg(n_verts2_size, ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // skip 0x18 bytes
 ifile.seekg(0x18, ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // debug
 if(debug) cout << "at offset 0x" << hex << (uint64)ifile.tellg() << dec << endl;

 //
 // VERTEX BUFFER #1
 // POINTS
 //

 struct MKXPOINT {
  real32 x, y, z;
 };

 // read size of vertex
 uint32 vb1_h01 = LE_read_uint32(ifile); // bytes per vertex
 uint32 vb1_h02 = LE_read_uint32(ifile);
 uint32 vb1_h03 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(vb1_h02 != n_verts1) return error("Unexpected number of vertices.", __LINE__);
 if(vb1_h03 != n_verts1) return error("Unexpected number of vertices.", __LINE__); 

 // read verts
 boost::shared_array<MKXPOINT> pointlist(new MKXPOINT[n_verts1]);
 for(uint32 i = 0; i < n_verts1; i++) {
     real32 x = LE_read_real32(ifile);
     real32 y = LE_read_real32(ifile);
     real32 z = LE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
     pointlist[i].x = x;
     pointlist[i].y = y;
     pointlist[i].z = z;
    }

 // debug
 if(debug) cout << "at offset 0x" << hex << (uint64)ifile.tellg() << dec << endl;

 //
 // VERTEX BUFFER #2
 // NORMALS
 //

 // read size of vertex
 uint32 vb2_h01 = LE_read_uint32(ifile); // bytes per vertex
 uint32 vb2_h02 = LE_read_uint32(ifile); // number of vertices
 uint32 vb2_h03 = LE_read_uint32(ifile); // number of vertices
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(vb2_h02 != n_verts1) return error("Unexpected number of vertices.", __LINE__);
 if(vb2_h03 != n_verts1) return error("Unexpected number of vertices.", __LINE__); 

 // skip
 ifile.seekg((vb2_h01*vb2_h02), ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // debug
 if(debug) cout << "at offset 0x" << hex << (uint64)ifile.tellg() << dec << endl;

 //
 // VERTEX BUFFER #3
 // BLENDING
 //

 struct MKXBLENDWEIGHT {
  uint08 bytes[8];
 };

 // read info
 uint16 vb3_h01 = LE_read_uint32(ifile); // bytes per vertex
 uint16 vb3_h02 = LE_read_uint32(ifile); // number of vertices
 uint16 vb3_h03 = LE_read_uint32(ifile); // number of vertices
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(vb3_h02 != n_verts1) return error("Unexpected number of vertices.", __LINE__);
 if(vb3_h03 != n_verts1) return error("Unexpected number of vertices.", __LINE__);

 // read data
 boost::shared_array<MKXBLENDWEIGHT> bwlist(new MKXBLENDWEIGHT[n_verts1]);
 for(uint32 i = 0; i < n_verts1; i++) {
     bwlist[i].bytes[0] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[1] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[2] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[3] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[4] = LE_read_uint08(ifile); // blendindex
     bwlist[i].bytes[5] = LE_read_uint08(ifile); // blendweight
     bwlist[i].bytes[6] = LE_read_uint08(ifile); // blendweight
     bwlist[i].bytes[7] = LE_read_uint08(ifile); // blendweight
     if(ifile.fail()) return error("Read failure.", __LINE__);
    }

 // debug
 if(debug) cout << "at offset 0x" << hex << (uint64)ifile.tellg() << dec << endl;

 //
 // VERTEX BUFFER #4
 // UV
 //

 struct MKXUVCHANNEL {
  boost::shared_array<boost::shared_array<real32>> data;
 };

 // read size of vertex
 uint16 vb4_h01 = LE_read_uint32(ifile); // number of channels
 uint32 vb4_h02 = LE_read_uint32(ifile); // bytes per vertex
 uint32 vb4_h03 = LE_read_uint32(ifile); // number of vertices
 uint32 vb4_h04 = LE_read_uint32(ifile); // number of vertices
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // read verts
 deque<MKXUVCHANNEL> uvlist;
 if(vb4_h01 == 1)
   {
    MKXUVCHANNEL channel;
    channel.data.reset(new boost::shared_array<real32>[n_verts1]);
    for(uint32 i = 0; i < vb4_h03; i++) {
        channel.data[i].reset(new real32[2]);
        real32 u = LE_read_real16(ifile);
        real32 v = LE_read_real16(ifile);
        if(ifile.fail()) return error("Read failure.", __LINE__);
        channel.data[i][0] = u;
        channel.data[i][1] = v;
       }
    uvlist.push_back(channel);
   }
 else if(vb4_h01 == 2)
   {
    // create channels
    MKXUVCHANNEL channel1;
    MKXUVCHANNEL channel2;
    channel1.data.reset(new boost::shared_array<real32>[n_verts1]);
    channel2.data.reset(new boost::shared_array<real32>[n_verts1]);

    // read channels
    vb4_h03 /= 2;
    vb4_h04 /= 2;
    for(uint32 i = 0; i < vb4_h03; i++) {
        real32 u1 = LE_read_real16(ifile);
        real32 v1 = LE_read_real16(ifile);
        real32 u2 = LE_read_real16(ifile);
        real32 v2 = LE_read_real16(ifile);
        if(ifile.fail()) return error("Read failure.", __LINE__);
        channel1.data[i].reset(new real32[2]);
        channel1.data[i][0] = u1;
        channel1.data[i][1] = v1;
        channel2.data[i].reset(new real32[2]);
        channel2.data[i][0] = u2;
        channel2.data[i][1] = v2;
       }
    uvlist.push_back(channel1);
    uvlist.push_back(channel2);
   }
 else
    return error("Invalid number of UV channels.");

 // debug
 if(debug) cout << "at offset 0x" << hex << (uint64)ifile.tellg() << dec << endl;

 // model container
 ADVANCEDMODELCONTAINER amc;

 // prepare vertex buffer
 AMC_VTXBUFFER vb;
 vb.name = "default";
 vb.elem = n_verts1;
 vb.data.reset(new AMC_VERTEX[vb.elem]);
 vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_WEIGHTS;
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

 // set UV channels
 if(vb4_h01) {
    vb.flags |= AMC_VERTEX_UV;
    vb.uvchan = vb4_h01;
    if(vb4_h01 > 0) vb.uvtype[0] = AMC_DIFFUSE_MAP;
    if(vb4_h01 > 1) vb.uvtype[1] = AMC_DIFFUSE_MAP;
   }

 // copy vertex data
 for(uint32 i = 0; i < vb.elem; i++)
    {
     // position
     vb.data[i].vx = pointlist[i].x;
     vb.data[i].vy = pointlist[i].y;
     vb.data[i].vz = pointlist[i].z;

     // uv
     for(uint32 j = 0; j < vb.uvchan; j++)  {
         vb.data[i].uv[j][0] = uvlist[j].data[i][0];
         vb.data[i].uv[j][1] = uvlist[j].data[i][1];
        }

     // blend weights
     uint16 w1 = bwlist[i].bytes[5];
     uint16 w2 = bwlist[i].bytes[6];
     uint16 w3 = bwlist[i].bytes[7];
     uint16 w4 = 0;

     // sum blend weights
     uint16 sum_weights = 0;
     sum_weights += bwlist[i].bytes[5];
     sum_weights += bwlist[i].bytes[6];
     sum_weights += bwlist[i].bytes[7];

     // need to determine how many weights there are
     uint16 n_weights = 1;
     if(w2 == 0x00) n_weights = 1;
     else if(w3 == 0x00) n_weights = 2;
     else n_weights = 3;

     // if weights don't add up to 0xFF, we have an extra weight
     if((n_weights == 3) && (sum_weights != 0xFF)) {
        w4 = 0xFF - sum_weights;
        n_weights++;
       }

     // weights
     vb.data[i].wv[0] = w1/255.0f;
     vb.data[i].wv[1] = w2/255.0f;
     vb.data[i].wv[2] = w3/255.0f;
     vb.data[i].wv[3] = w4/255.0f;
     vb.data[i].wv[4] = 0.0f;
     vb.data[i].wv[5] = 0.0f;
     vb.data[i].wv[6] = 0.0f;
     vb.data[i].wv[7] = 0.0f;
     vb.data[i].wv[8] = 0.0f;

     // blend indices
     uint16 i1 = bwlist[i].bytes[0];
     uint16 i2 = bwlist[i].bytes[1];
     uint16 i3 = bwlist[i].bytes[2];
     uint16 i4 = bwlist[i].bytes[3];
     uint16 i5 = bwlist[i].bytes[4];

     // aaaaaaaa bbbbbbbb cccccccc dddddddd eeeeeeee
     // since i2 is divisible by 4, use bbaaaaaaaa for i1
     // bbaaaaaaaa bbbbbb00 cccccccc dddddddd eeeeeeee
     // since i3 is divisible by 16, use ccccbbbbbb00 for i2
     // bbaaaaaaaa ccccbbbbbb00 cccc0000 dddddddd eeeeeeee
     // since i4 is divisible by 64, use ddddddcccc0000
     // bbaaaaaaaa ccccbbbbbb00 ddddddcccc0000 eeeeeeeedd000000 

     // compute indices
     const uint16 d2 = 0x03;
     const uint16 d3 = 0x0F;
     const uint16 d4 = 0x3F;
     uint16 p1 = (i1 | ((i2 & d2) << 8));
     uint16 p2 = (i2 | ((i3 & d3) << 8)) >> 2;
     uint16 p3 = (i3 | ((i4 & d4) << 8)) >> 4;
     uint16 p4 = (i4 | (i5 << 8)) >> 6;

     if(n_weights == 1) {
        vb.data[i].wi[0] = p1;
        vb.data[i].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
     else if(n_weights == 2) {
        vb.data[i].wi[0] = p1;
        vb.data[i].wi[1] = p2;
        vb.data[i].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
     else if(n_weights == 3)  {
        vb.data[i].wi[0] = p1;
        vb.data[i].wi[1] = p2;
        vb.data[i].wi[2] = p3;
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
     else if(n_weights == 4) {
        vb.data[i].wi[0] = p1;
        vb.data[i].wi[1] = p2;
        vb.data[i].wi[2] = p3;
        vb.data[i].wi[3] = p4;
        vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
    }

 // insert vertex buffer
 amc.vblist.push_back(vb);

 // prepare index buffer
 AMC_IDXBUFFER ib;
 ib.type = AMC_TRIANGLES;
 ib.format = AMC_UINT16;
 ib.name = "default";
 ib.wind = AMC_CCW;
 ib.elem = n_indices;
 uint32 ib_datasize = n_indices * 0x02;
 ib.data.reset(new char[ib_datasize]);
 memmove(ib.data.get(), ibdata.get(), ib_datasize);

 // insert index buffer
 amc.iblist.push_back(ib);

 // process joint maps
 for(size_t i = 0; i < jmlist.size(); i++)
    {
     // copy joint map data
     deque<uint32> jntmap;
     for(size_t j = 0; j < jmlist[i].size; j++) jntmap.push_back(jmlist[i].data[j]);

     // insert joint map
     AMC_JOINTMAP jmap;
     jmap.jntmap = jntmap;
     amc.jmaplist.push_back(jmap);
    }

 // process surfaces
 for(uint32 i = 0; i < n_surfaces; i++)
    {
     // create surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << meshlist[i].id;

     // create surface
     AMC_SURFACE surface;
     surface.name = ss.str();
     surface.surfmat = AMC_INVALID_SURFMAT;
     AMC_REFERENCE ref;
     ref.vb_index = 0;
     ref.vb_start = 0;
     ref.vb_width = n_verts1;
     ref.ib_index = 0;
     ref.ib_start = meshlist[i].start;
     ref.ib_width = meshlist[i].n_tri*3;
     ref.jm_index = meshlist[i].jm; // AMC_INVALID_JMAP_INDEX
     surface.refs.push_back(ref);
     // save surface
     amc.surfaces.push_back(surface);
    }

 // transform and save AMC file
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

bool MKX_ProcessStaticMeshList(LPCTSTR pathname, const std::map<uint64, std::string>& hashtable)
{
 // build a list of StaticMesh files
 cout << "Searching for StaticMesh files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".StaticMesh"), pathname);
 if(!filelist.size()) return true;

 // process StaticMesh files
 cout << "Processing StaticMesh files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!MKX_ProcessStaticMesh(filelist[i].c_str(), hashtable)) return false;
    }
 cout << endl;
 return true;
}

bool MKX_ProcessStaticMesh(LPCTSTR filename, const std::map<uint64, std::string>& hashtable)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read data
 bool debug = true;
 for(;;)
    {
     if(debug) cout << "offset = 0x" << hex << (sint64)ifile.tellg() << dec << endl;

     // read property name
     uint64 name = LE_read_uint64(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // lookup name in hashtable
     auto iter = hashtable.find(name);
     if(iter == hashtable.end()) return error("Failed to find property name.");
     std::string namestr = iter->second;
     if(debug) std::cout << "0x" << hex << name << dec << ", " << iter->second << std::endl;

     // "None"
     if(namestr == "None")
        break;

     // read other property parameters
     uint64 type = LE_read_uint64(ifile);
     uint32 size = LE_read_uint32(ifile);
     uint32 indx = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // lookup type in hashtable
     iter = hashtable.find(type);
     if(iter == hashtable.end()) return error("Failed to find property type.");
     std::string typestr = iter->second;
     if(debug) std::cout << "0x" << hex << type << dec << ", " << iter->second << std::endl;

     // WTF LOL???
     if(size == 0) {
        if(typestr == "BoolProperty") size = 4;
       }

     // Bounds && LightingGuid
     if(typestr == "StructProperty") {
        // read 8 bytes before actual property data
        ifile.seekg(0x08, ios::cur);
        if(ifile.fail()) return error("Seek failure.", __LINE__);
       }

     // read property data
     boost::shared_array<char> data;
     if(size) {
        data.reset(new char[size]);
        ifile.read(data.get(), size);
        if(ifile.fail()) return error("Read failure.", __LINE__);
       }
    }

 // read unknown table #1
 uint32 n_unk01 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 for(uint32 i = 0; i < n_unk01; i++) {
     ifile.seekg(0x24, ios::cur);
     if(ifile.fail()) return error("Seek failure.", __LINE__);
    }

 // read unknown table #2
 uint32 n_unk02 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 for(uint32 i = 0; i < n_unk02; i++) {
     ifile.seekg(0x10, ios::cur);
     if(ifile.fail()) return error("Seek failure.", __LINE__);
    }

 // skip 0x20 bytes of data
 ifile.seekg(0x20, ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read number of surfaces
 uint32 n_mesh = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(!n_mesh) return error("Invalid number of meshes.", __LINE__);

 struct MKX_MESH {
  uint32 data[8];
 };
 deque<MKX_MESH> meshlist;
 for(uint32 i = 0; i < n_mesh; i++) {
     MKX_MESH mesh;
     mesh.data[0] = LE_read_uint32(ifile);
     mesh.data[1] = LE_read_uint32(ifile);
     mesh.data[2] = LE_read_uint32(ifile);
     mesh.data[3] = LE_read_uint32(ifile);
     mesh.data[4] = LE_read_uint32(ifile);
     mesh.data[5] = LE_read_uint32(ifile);
     mesh.data[6] = LE_read_uint32(ifile);
     mesh.data[7] = LE_read_uint32(ifile);
     meshlist.push_back(mesh);
    }

 // 
 // VERTEX BUFFER #1
 //

 struct MKX_VERTEX {
  real32 x;
  real32 y;
  real32 z;
 };
 deque<MKX_VERTEX> pointlist;

 uint32 bpv1 = LE_read_uint32(ifile);
 uint32 n_vtx1 = LE_read_uint32(ifile);
 uint32 n_vtx2 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(n_vtx1 != n_vtx2) return error("Number of vertices should be the same.", __LINE__);

 // for each n
 for(uint32 i = 0; i < n_vtx1; i++) {
     MKX_VERTEX v;
     v.x = LE_read_real32(ifile);
     v.y = LE_read_real32(ifile);
     v.z = LE_read_real32(ifile);
     pointlist.push_back(v);
    }

 // skip 0x0C bytes of data
 ifile.seekg(0x0C, ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // 
 // VERTEX BUFFER #2
 //

 uint32 vb2_p01 = LE_read_uint32(ifile);
 uint32 vb2_p02 = LE_read_uint32(ifile);
 uint32 vb2_p03 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(vb2_p02 != vb2_p03) return error("Number of vertices should be the same.", __LINE__);

 // skip buffer
 ifile.seekg((vb2_p01*vb2_p02), ios::cur);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // 
 // VERTEX BUFFER #3
 //

 struct MKXUVCHANNEL {
  boost::shared_array<boost::shared_array<real32>> data;
 };
 deque<MKXUVCHANNEL> uvlist1;

 // read n
 uint32 uv_p01 = LE_read_uint32(ifile); // number of channels
 uint32 uv_p02 = LE_read_uint32(ifile); // bytpes per vertex
 uint32 uv_p03 = LE_read_uint32(ifile); // number of vertices
 uint32 uv_p04 = LE_read_uint32(ifile); // number of vertices
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(uv_p03 != uv_p04) return error("Number of vertices must match.", __LINE__);
 if(uv_p03 != n_vtx1) return error("Number of vertices must match.", __LINE__);

 // read verts
 deque<MKXUVCHANNEL> uvlist;
 if(uv_p01 == 1)
   {
    MKXUVCHANNEL channel;
    channel.data.reset(new boost::shared_array<real32>[n_vtx1]);
    for(uint32 i = 0; i < n_vtx1; i++) {
        channel.data[i].reset(new real32[2]);
        real32 u = LE_read_real16(ifile);
        real32 v = LE_read_real16(ifile);
        if(ifile.fail()) return error("Read failure.", __LINE__);
        channel.data[i][0] = u;
        channel.data[i][1] = v;
       }
    uvlist.push_back(channel);
   }
 else if(uv_p01 == 2)
   {
    // create channels
    MKXUVCHANNEL channel1;
    MKXUVCHANNEL channel2;
    channel1.data.reset(new boost::shared_array<real32>[n_vtx1]);
    channel2.data.reset(new boost::shared_array<real32>[n_vtx1]);

    // read channels
    for(uint32 i = 0; i < n_vtx1; i++) {
        real32 u1 = LE_read_real16(ifile);
        real32 v1 = LE_read_real16(ifile);
        real32 u2 = LE_read_real16(ifile);
        real32 v2 = LE_read_real16(ifile);
        if(ifile.fail()) return error("Read failure.", __LINE__);
        channel1.data[i].reset(new real32[2]);
        channel1.data[i][0] = u1;
        channel1.data[i][1] = v1;
        channel2.data[i].reset(new real32[2]);
        channel2.data[i][0] = u2;
        channel2.data[i][1] = v2;
       }
    uvlist.push_back(channel1);
    uvlist.push_back(channel2);
   }
 else if(uv_p01 == 3)
   {
    // create channels
    MKXUVCHANNEL channel1;
    MKXUVCHANNEL channel2;
    MKXUVCHANNEL channel3;
    channel1.data.reset(new boost::shared_array<real32>[n_vtx1]);
    channel2.data.reset(new boost::shared_array<real32>[n_vtx1]);
    channel3.data.reset(new boost::shared_array<real32>[n_vtx1]);

    // read channels
    for(uint32 i = 0; i < n_vtx1; i++) {
        real32 u1 = LE_read_real16(ifile);
        real32 v1 = LE_read_real16(ifile);
        real32 u2 = LE_read_real16(ifile);
        real32 v2 = LE_read_real16(ifile);
        real32 u3 = LE_read_real16(ifile);
        real32 v3 = LE_read_real16(ifile);
        if(ifile.fail()) return error("Read failure.", __LINE__);
        channel1.data[i].reset(new real32[2]);
        channel1.data[i][0] = u1;
        channel1.data[i][1] = v1;
        channel2.data[i].reset(new real32[2]);
        channel2.data[i][0] = u2;
        channel2.data[i][1] = v2;
        channel3.data[i].reset(new real32[2]);
        channel3.data[i][0] = u3;
        channel3.data[i][1] = v3;
       }
    uvlist.push_back(channel1);
    uvlist.push_back(channel2);
    uvlist.push_back(channel3);
   }
 else
    return error("Invalid number of UV channels.");

 // 
 // VERTEX BUFFER #4
 //

 uint32 vb4_p01 = LE_read_uint32(ifile); // size per vertex
 uint32 vb4_p02 = LE_read_uint32(ifile); // number of vertices
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // if there is a vertex buffer
 uint32 vb4_p03 = 0x00; 
 if(vb4_p01)
   {
    // number of vertices
    vb4_p03 = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.", __LINE__);
    // skip buffer
    ifile.seekg((vb4_p01*vb4_p02), ios::cur);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
   }

 // 
 // INDEX BUFFER
 //

 if(debug) {
    cout << "index buffer offset = 0x" << hex << (sint64)ifile.tellg() << dec << endl;
   }

 // read number of indices
 uint32 ib_p01 = LE_read_uint32(ifile); // number of vertices
 uint32 ib_p02 = LE_read_uint32(ifile); // number of indices
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(n_vtx1 != ib_p01) return error("Number of vertices must match.", __LINE__);

 // read index buffer
 boost::shared_array<uint16> ibdata(new uint16[ib_p02]);
 LE_read_array(ifile, ibdata.get(), ib_p02);

 // model container
 ADVANCEDMODELCONTAINER amc;

 // variables
 uint32 n_verts = n_vtx1;
 uint32 n_indices = ib_p02;
 uint32 uv_channels = uvlist.size();

 // prepare vertex buffer
 AMC_VTXBUFFER vb;
 vb.name = "default";
 vb.elem = n_verts;
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

 // set UV channels
 if(uv_channels) {
    vb.flags |= AMC_VERTEX_UV;
    vb.uvchan = uv_channels;
    if(uv_channels > 0) vb.uvtype[0] = AMC_DIFFUSE_MAP;
    if(uv_channels > 1) vb.uvtype[1] = AMC_DIFFUSE_MAP;
    if(uv_channels > 2) vb.uvtype[2] = AMC_DIFFUSE_MAP;
   }

 // copy vertex data
 for(uint32 i = 0; i < vb.elem; i++)
    {
     // position
     vb.data[i].vx = pointlist[i].x;
     vb.data[i].vy = pointlist[i].y;
     vb.data[i].vz = pointlist[i].z;

     // uv
     for(uint32 j = 0; j < vb.uvchan; j++)  {
         vb.data[i].uv[j][0] = uvlist[j].data[i][0];
         vb.data[i].uv[j][1] = uvlist[j].data[i][1];
        }
    }

 // insert vertex buffer
 amc.vblist.push_back(vb);

 // prepare index buffer
 AMC_IDXBUFFER ib;
 ib.type = AMC_TRIANGLES;
 ib.format = AMC_UINT16;
 ib.name = "default";
 ib.wind = AMC_CCW;
 ib.elem = n_indices;
 uint32 ib_datasize = n_indices * 0x02;
 ib.data.reset(new char[ib_datasize]);
 memmove(ib.data.get(), ibdata.get(), ib_datasize);

 // insert index buffer
 amc.iblist.push_back(ib);

 // process surfaces
 for(uint32 i = 0; i < meshlist.size(); i++)
    {
     // create surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << i; // meshlist[i].data[0];

     // create surface
     AMC_SURFACE surface;
     surface.name = ss.str();
     surface.surfmat = AMC_INVALID_SURFMAT;
     AMC_REFERENCE ref;
     ref.vb_index = 0;
     ref.vb_start = 0;
     ref.vb_width = n_verts;
     ref.ib_index = 0;
     ref.ib_start = meshlist[i].data[3]; // start index
     ref.ib_width = meshlist[i].data[4] * 3; // number of triangles * 3
     ref.jm_index =AMC_INVALID_JMAP_INDEX;
     surface.refs.push_back(ref);
     // save surface
     amc.surfaces.push_back(surface);
    }

 // transform and save AMC file
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}