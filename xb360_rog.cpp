#include "xentax.h"
#include "x_math.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "xb360_rog.h"
using namespace std;

#define X_SYSTEM XB360
#define X_GAME   ROG

namespace X_SYSTEM { namespace X_GAME {

bool extract(void)
{
 // model container data
 ADVANCEDMODELCONTAINER amc;

 // image #1
 AMC_IMAGEFILE image1;
 image1.filename = "Jack_a.dds";
 amc.iflist.push_back(image1);

 // image #2
 AMC_IMAGEFILE image2;
 image2.filename = "Jack_g.dds";
 amc.iflist.push_back(image2);

 // image #3
 AMC_IMAGEFILE image3;
 image3.filename = "Jack_n.dds";
 amc.iflist.push_back(image3);

 // image #4
 AMC_IMAGEFILE image4;
 image4.filename = "Jack_s.dds";
 amc.iflist.push_back(image4);

 // filename to analyze
 string shrtname("PLAYERJack.bin");
 string fullname = GetModulePathname();
 fullname += shrtname;

 // open file
 ifstream ifile(fullname.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // move to vertex buffer
 uint32 vb_offset = 0x19DA76;
 ifile.seekg(vb_offset);
 if(ifile.fail()) return error("Seek failure.");

 //
 // READ VERTEX BUFFER #1
 //

 // vertex buffer properties
 uint32 vb_size = LE_read_uint32(ifile);
 uint32 vb_unk1 = LE_read_uint16(ifile); // 0x43
 uint32 vb_unk2 = LE_read_uint16(ifile); // 0x04
 uint32 vb_stride = 0x30;
 uint32 vb_vertices = vb_size/vb_stride;

 // read vertex buffer
 boost::shared_array<char> vb_data(new char[vb_size]);
 ifile.read(vb_data.get(), vb_size);
 if(ifile.fail()) return error("Read failure.");

 // allocate vertex buffer
 AMC_VTXBUFFER vb;
 vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV | AMC_VERTEX_WEIGHTS;
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
 vb.elem = vb_vertices;
 vb.data.reset(new AMC_VERTEX[vb.elem]);

 // process vertex buffer
 binary_stream bs(vb_data, vb_size);
 for(uint32 j = 0; j < vb_vertices; j++)
    {
     // read vertex
     vb.data[j].vx = bs.BE_read_real32();
     vb.data[j].vy = bs.BE_read_real32();
     vb.data[j].vz = bs.BE_read_real32();
     vb.data[j].nx = bs.BE_read_real32();
     vb.data[j].ny = bs.BE_read_real32();
     vb.data[j].nz = bs.BE_read_real32();
     bs.BE_read_real32();
     bs.BE_read_real32();
     bs.BE_read_real32();
     bs.BE_read_real32();
     vb.data[j].tu = bs.BE_read_real32();
     vb.data[j].tv = bs.BE_read_real32();
     vb.data[j].uv[0][0] = vb.data[j].tu;
     vb.data[j].uv[0][1] = vb.data[j].tv;

     // initialize weights
     vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

     // set weights
     vb.data[j].wv[0] = 1.0f;
     vb.data[j].wi[0] = 0xFF;
    }

 amc.vblist.push_back(vb);

 //
 // READ INDEX BUFFER #1
 //

 // index buffer properties
 uint32 ib_size = LE_read_uint32(ifile);
 uint32 ib_unk1 = LE_read_uint16(ifile); // 0x44
 uint32 ib_unk2 = LE_read_uint16(ifile); // 0x02
 uint32 ib_elem = ib_size / 2;

 // allocate index buffer
 AMC_IDXBUFFER ib;
 ib.format = AMC_UINT16;
 ib.type = AMC_TRIANGLES;
 ib.name = "default";
 ib.elem = ib_elem;
 ib.data.reset(new char[ib_size]);

 // read index buffer
 ifile.read(ib.data.get(), ib_size);
 if(ifile.fail()) return error("Read failure.");

 // reverse byte order of data
 uint16* ptr = reinterpret_cast<uint16*>(ib.data.get());
 reverse_byte_order(ptr, ib_elem);

 // insert vertex buffer
 amc.iblist.push_back(ib);

 //
 // READ VERTEX BUFFER #2
 //

 // vertex buffer properties
 uint32 vb2_size = LE_read_uint32(ifile);
 uint32 vb2_unk1 = LE_read_uint16(ifile); // 0x43
 uint32 vb2_unk2 = LE_read_uint16(ifile); // 0x04
 uint32 vb2_stride = 0x40;
 uint32 vb2_vertices = vb2_size/vb2_stride;

 // read vertex buffer
 boost::shared_array<char> vb2_data(new char[vb2_size]);
 ifile.read(vb2_data.get(), vb2_size);
 if(ifile.fail()) return error("Read failure.");

 // allocate vertex buffer #2
 AMC_VTXBUFFER vb2;
 vb2.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV | AMC_VERTEX_WEIGHTS;
 vb2.name = "default";
 vb2.uvchan = 1;
 vb2.uvtype[0] = AMC_DIFFUSE_MAP;
 vb2.uvtype[1] = AMC_INVALID_MAP;
 vb2.uvtype[2] = AMC_INVALID_MAP;
 vb2.uvtype[3] = AMC_INVALID_MAP;
 vb2.uvtype[4] = AMC_INVALID_MAP;
 vb2.uvtype[5] = AMC_INVALID_MAP;
 vb2.uvtype[6] = AMC_INVALID_MAP;
 vb2.uvtype[7] = AMC_INVALID_MAP;
 vb2.colors = 0;
 vb2.elem = vb2_vertices;
 vb2.data.reset(new AMC_VERTEX[vb2.elem]);

 // process vertex buffer
 binary_stream cs(vb2_data, vb2_size);
 for(uint32 j = 0; j < vb2_vertices; j++)
    {
     // read vertex
     vb2.data[j].vx = cs.BE_read_real32(); // 12
     vb2.data[j].vy = cs.BE_read_real32();
     vb2.data[j].vz = cs.BE_read_real32();
     vb2.data[j].nx = cs.BE_read_real32(); // 24
     vb2.data[j].ny = cs.BE_read_real32();
     vb2.data[j].nz = cs.BE_read_real32();
     cs.BE_read_real32(); // 36
     cs.BE_read_real32();
     cs.BE_read_real32();
     cs.BE_read_real32(); // 48
     vb2.data[j].tu = cs.BE_read_real32();
     vb2.data[j].tv = cs.BE_read_real32();
     vb2.data[j].uv[0][0] = vb2.data[j].tu;
     vb2.data[j].uv[0][1] = vb2.data[j].tv;
     real32 w1 = cs.BE_read_real32(); // 60
     real32 w2 = cs.BE_read_real32();
     real32 w3 = cs.BE_read_real32();
     real32 w4 = cs.BE_read_real32();
     uint16 i4 = cs.BE_read_uint08(); // 68
     uint16 i3 = cs.BE_read_uint08();
     uint16 i2 = cs.BE_read_uint08();
     uint16 i1 = cs.BE_read_uint08();

     vb2.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

     if(i1 != 0) {
        vb2.data[j].wv[0] = w1;
        vb2.data[j].wi[0] = i1;
       }
     if(i2 != 0) {
        vb2.data[j].wv[1] = w2;
        vb2.data[j].wi[1] = i2;
       }
     if(i3 != 0) {
        vb2.data[j].wv[2] = w3;
        vb2.data[j].wi[2] = i3;
       }
     if(i4 != 0) {
        vb2.data[j].wv[3] = w4;
        vb2.data[j].wi[3] = i4;
       }
    }

 amc.vblist.push_back(vb2);

 //
 // READ INDEX BUFFER #2
 //

 // index buffer properties
 uint32 ib2_size = LE_read_uint32(ifile);
 uint32 ib2_unk1 = LE_read_uint16(ifile); // 0x44
 uint32 ib2_unk2 = LE_read_uint16(ifile); // 0x02
 uint32 ib2_elem = ib2_size / 2;

 // allocate index buffer
 AMC_IDXBUFFER ib2;
 ib2.format = AMC_UINT16;
 ib2.type = AMC_TRIANGLES;
 ib2.name = "default";
 ib2.elem = ib2_elem;
 ib2.data.reset(new char[ib2_size]);

 // read index buffer
 ifile.read(ib2.data.get(), ib2_size);
 if(ifile.fail()) return error("Read failure.");

 // reverse byte order of data
 ptr = reinterpret_cast<uint16*>(ib2.data.get());
 reverse_byte_order(ptr, ib2_elem);

 // insert vertex buffer
 amc.iblist.push_back(ib2);

 // SURFACE #1
 // there are 0x1D0D indices
 // start: 0x0000 there are 0x043E indices in 1st one (at 0x19ABA2) size 0x87C
 // start: 0x043E there are 0x01F8 indices in 2nd one size 0x3F0
 // start: 0x0636 there are 0x0252 indices in 3rd one size 0x4A4
 // start: 0x0888 there are 0x1485 indices in 3rd one size 0x290A

 // process surfaces
 AMC_SURFACE surf1;
 surf1.name = "surface01";
 AMC_REFERENCE r1;
 r1.vb_index = 0;
 r1.vb_start = 0;
 r1.vb_width = 0x62;
 r1.ib_index = 0;
 r1.ib_start = 0;
 r1.ib_width = 0x1F8; // 0x87C first one 0x43E indices
 surf1.refs.push_back(r1);
 surf1.surfmat = AMC_INVALID_SURFMAT;
 amc.surfaces.push_back(surf1);

 AMC_SURFACE surf2;
 surf2.name = "surface02";
 AMC_REFERENCE r2;
 r2.vb_index = 0;
 r2.vb_start = 0x62;
 r2.vb_width = 0x8B;
 r2.ib_index = 0;
 r2.ib_start = 0x1F8;
 r2.ib_width = 0x246;
 surf2.refs.push_back(r2);
 surf2.surfmat = AMC_INVALID_SURFMAT;
 amc.surfaces.push_back(surf2);

 AMC_SURFACE surf3;
 surf3.name = "surface03";
 AMC_REFERENCE r3;
 r3.vb_index = 0;
 r3.vb_start = 0xED;
 r3.vb_width = 0x62;
 r3.ib_index = 0;
 r3.ib_start = 0x43E;
 r3.ib_width = 0x1F8;
 surf3.refs.push_back(r3);
 surf3.surfmat = AMC_INVALID_SURFMAT;
 amc.surfaces.push_back(surf3);

 AMC_SURFACE surf4;
 surf4.name = "surface04";
 AMC_REFERENCE r4;
 r4.vb_index = 0;
 r4.vb_start = 0x14F;
 r4.vb_width = 0x8F;
 r4.ib_index = 0;
 r4.ib_start = 0x636;
 r4.ib_width = 0x252;
 surf4.refs.push_back(r4);
 surf4.surfmat = AMC_INVALID_SURFMAT;
 amc.surfaces.push_back(surf4);

 AMC_SURFACE surf5;
 surf5.name = "surface05";
 AMC_REFERENCE r5;
 r5.vb_index = 0;
 r5.vb_start = 0x1DE;
 r5.vb_width = 0x66D;
 r5.ib_index = 0;
 r5.ib_start = 0x888;
 r5.ib_width = 0x1485;
 surf5.refs.push_back(r5);
 surf5.surfmat = AMC_INVALID_SURFMAT;
 amc.surfaces.push_back(surf5);

 // need to adjust weights!
 uint16 jmaplist1[0x1E] = { 0x00, 0x01, 0x02, 0x03, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1D, 0x1E, 0x1F, 0x20, 0x33 };
 uint16 jmaplist2[0x1E] = { 0x00, 0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x1C, 0x1D, 0x1E, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x34, 0x35, 0x38, 0x39, 0x3A, 0x3D };
 uint16 jmaplist3[0x1A] = { 0x02, 0x03, 0x04, 0x05, 0x06, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x24, 0x27, 0x28, 0x29, 0x2A, 0x2C, 0x2F, 0x30, 0x31, 0x32, 0x35, 0x36, 0x37, 0x3A, 0x3B, 0x3C };

 AMC_SURFACE surf6;
 surf6.name = "surface06";
 AMC_REFERENCE r6;
 r6.vb_index = 1;
 r6.vb_start = 0x0;
 r6.vb_width = 0x1735;
 r6.ib_index = 1;
 r6.ib_start = 0x0;
 r6.ib_width = 0x14C4;
 surf6.refs.push_back(r6);
 surf6.surfmat = AMC_INVALID_SURFMAT;
 amc.surfaces.push_back(surf6);

 // set to keep track of which indices have been modified
 set<uint16> iset;

 // get vertex from index buffer and adjust weight
 //for(uint32 i = r6.ib_start; i < r6.ib_width; i++) {
 //    const uint16* ib = reinterpret_cast<const uint16*>(amc.iblist[1].data.get());
 //    if(iset.find(ib[i]) != iset.end()) continue;
 //    iset.insert(ib[i]);
 //    AMC_VERTEX& vertex = amc.vblist[1].data[ib[i]];
 //    if(vertex.wi[0] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[0] = jmaplist1[vertex.wi[0]];
 //    if(vertex.wi[1] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[1] = jmaplist1[vertex.wi[1]];
 //    if(vertex.wi[2] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[2] = jmaplist1[vertex.wi[2]];
 //    if(vertex.wi[3] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[3] = jmaplist1[vertex.wi[3]];
 //   }

 AMC_SURFACE surf7;
 surf7.name = "surface07";
 AMC_REFERENCE r7;
 r7.vb_index = 1;
 r7.vb_start = 0x0;
 r7.vb_width = 0x1735;
 r7.ib_index = 1;
 r7.ib_start = 0x14C4;
 r7.ib_width = 0x22B6;
 surf7.refs.push_back(r7);
 surf7.surfmat = AMC_INVALID_SURFMAT;
 amc.surfaces.push_back(surf7);

 // get vertex from index buffer and adjust weight
 //for(uint32 i = r7.ib_start; i < r7.ib_width; i++) {
 //    const uint16* ib = reinterpret_cast<const uint16*>(amc.iblist[1].data.get());
 //    if(iset.find(ib[i]) != iset.end()) continue;
 //    iset.insert(ib[i]);
 //    AMC_VERTEX& vertex = amc.vblist[1].data[ib[i]];
 //    if(vertex.wi[0] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[0] = jmaplist2[vertex.wi[0]];
 //    if(vertex.wi[1] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[1] = jmaplist2[vertex.wi[1]];
 //    if(vertex.wi[2] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[2] = jmaplist2[vertex.wi[2]];
 //    if(vertex.wi[3] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[3] = jmaplist2[vertex.wi[3]];
 //   }

 AMC_SURFACE surf8;
 surf8.name = "surface08";
 AMC_REFERENCE r8;
 r8.vb_index = 1;
 r8.vb_start = 0x0;
 r8.vb_width = 0x1735;
 r8.ib_index = 1;
 r8.ib_start = 0x377A;
 r8.ib_width = 0x39FC;
 surf8.refs.push_back(r8);
 surf8.surfmat = AMC_INVALID_SURFMAT;
 amc.surfaces.push_back(surf8);

 // get vertex from index buffer and adjust weight
 //for(uint32 i = r8.ib_start; i < r8.ib_width; i++) {
 //    const uint16* ib = reinterpret_cast<const uint16*>(amc.iblist[1].data.get());
 //    if(iset.find(ib[i]) != iset.end()) continue;
 //    iset.insert(ib[i]);
 //    AMC_VERTEX& vertex = amc.vblist[1].data[ib[i]];
 //    if(vertex.wi[0] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[0] = jmaplist3[vertex.wi[0]];
 //    if(vertex.wi[1] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[1] = jmaplist3[vertex.wi[1]];
 //    if(vertex.wi[2] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[2] = jmaplist3[vertex.wi[2]];
 //    if(vertex.wi[3] != AMC_INVALID_VERTEX_WMAP_INDEX) vertex.wi[3] = jmaplist3[vertex.wi[3]];
 //   }

 // initialize skeleton
 AMC_SKELETON2 skel2;
 skel2.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel2.tiplen = 0.25f;
 skel2.name = "skeleton";

 // skeleton information
 uint32 n_joints = 0x46;
 uint32 parent_offset = 0x19BF34;
 uint32 matrix_offset = 0x19BFCE;

 // move to parent list
 ifile.seekg(parent_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read parent data 
 boost::shared_array<uint16> parents(new uint16[n_joints]);
 if(!BE_read_array(ifile, parents.get(), n_joints)) return error("Read failure.");

 // move to matrix data
 ifile.seekg(matrix_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read matrix data
 for(uint32 i = 0; i < n_joints; i++)
    {
     // read data
     real32 m[16];
     if(!BE_read_array(ifile, &m[0], 16)) return error("Read failure.");
     matrix4x4_transpose(&m[0]);

     // create joint name
     stringstream ss;
     ss << "joint_" << setfill('0') << setw(3) << i;

     // create and insert joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = (parents[i] == 0xFFFF ? AMC_INVALID_JOINT : parents[i]);
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
     joint.p_rel[0] = m[0x3];
     joint.p_rel[1] = m[0x7];
     joint.p_rel[2] = m[0xB];
     joint.p_rel[3] = 1.0f;
     skel2.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel2);
 SaveLWO(GetModulePathname().c_str(), "PLAYERJack", amc);

 // save AMC file
 SaveAMC(GetModulePathname().c_str(), "PLAYERJack", amc);
 return true;
}

}};

