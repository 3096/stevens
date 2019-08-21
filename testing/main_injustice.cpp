#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
using namespace std;

#include "x_stream.h"
#include "x_amc.h"
#include "x_bmp.h"
#include "x_dds.h"

// PS3 includes

// XBOX360 includes
#include "xb360_ron.h"
#include "xb360_injustice.h"

// PC includes
//#include "pc_re7.h"

// xbox graphics include
#include<d3d9.h>
#include<d3d9types.h>
#include<xgraphics.h>

bool TestInjustice(void)
{
 // open file
 using namespace std;
 bool do_skeleton = true;
 string filename = "c:\\users\\semory\\desktop\\test\\CHAR_HarleyQuinn_C\\model12.bin";
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 ADVANCEDMODELCONTAINER amc;

 //
 // FIXED PROPERTIES
 //

 // part #0
 uint32 n_joints = 0x3;

 // part #1
 uint32 jntdata_offset = 0xFC;
 uint32 jntdata_elem = n_joints;
 uint32 jntdata_size = jntdata_elem * 0x1C;

 // part #2
 // to find, it is immediately after part #1
 uint32 jparent_offset = jntdata_offset + jntdata_size + 0x04;
 uint32 jparent_elem = n_joints;
 uint32 jparent_size = jparent_elem * 0x02;

 // part #3
 // to find, find index buffer
 // before index buffer you will find a bunch of repetitive data
 // at end of this data is the number of joints again
 // skip over this data section and you will see the 0xA pattern
 // or just find the damn index buffer, set HxD to display 10 characters per row, and you will see 00, 01, 02, 03, ..., going down
 uint32 surf_offset = 0x283;
 uint32 surf_elem = 0x1;
 uint32 surf_size = surf_elem * 0x0A;

 // PART #4
 // NO NEED TO SET ANYTHING

 // immediately after surface data
 uint32 ib_offset = surf_offset + surf_size + 0x04;
 ifile.seekg(ib_offset - 0x04);
 uint32 ib_elem = BE_read_uint32(ifile);
 uint32 ib_size = ib_elem * 0x02;
 cout << "ib_offset at 0x" << hex << ib_offset << dec << endl;

 // PART #5: JOINT MAP DATA
 // NO NEED TO SET ANYTHING

 // skip 0x00000000 after index buffer
 uint32 jmap_temp = ib_offset + ib_size;
 jmap_temp += 0x04; 

 // read a number
 ifile.seekg(jmap_temp);
 uint32 number = BE_read_uint32(ifile);
 jmap_temp += 0x04;

 // skip 2 * number bytes
 jmap_temp += 2 * number;

 // skip 0x00000000
 jmap_temp += 0x04;

 // read number of joint maps
 ifile.seekg(jmap_temp);
 uint32 jmap_elem = BE_read_uint32(ifile);
 jmap_temp += 0x04;

 // skip 0x00000000
 jmap_temp += 0x04;
 uint32 jmap_offset = jmap_temp;

 cout << "jap_offset at 0x" << hex << jmap_offset << dec << endl;

 // move to joint map
 ifile.seekg(jmap_offset);
 if(ifile.fail()) return error("Seek failure.");

 // create joint map
 for(uint32 i = 0; i < jmap_elem; i++)
    {
     // skip 0x10 bytes of zeroes
     ifile.seekg(0x10, ios::cur);
     if(ifile.fail()) return error("Seek failure.");

     // read number of items
     uint16 items = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Seek failure.");

     // read items
     AMC_JOINTMAP jm;
     for(uint32 j = 0; j < items; j++) {
         uint16 item = BE_read_uint16(ifile);
         jm.jntmap.push_back(item);
        }
     amc.jmaplist.push_back(jm);

     // skip 0x10 bytes
     ifile.seekg(0x10, ios::cur);
     if(ifile.fail()) return error("Seek failure.");
    }

 cout << "end of jmap_offset at 0x" << hex << (uint32)ifile.tellg() << dec << endl;

 //
 // PART #6
 // VERTEX BUFFER
 // 

 // read number of vertices
 uint32 vb_temp1 = BE_read_uint32(ifile);

 // read zero
 BE_read_uint32(ifile);

 // read number of joints again
 uint32 vb_temp2 = BE_read_uint32(ifile);

 // skip number of joints bytes
 ifile.seekg(vb_temp2, ios::cur);
 if(ifile.fail()) return error("Seek failure.");

 // read zero
 BE_read_uint32(ifile);

 // read number of vertices again
 uint32 vb_temp3 = BE_read_uint32(ifile);

 // read number of bytes of vertex index data
 uint32 vb_temp4 = BE_read_uint32(ifile);

 // read unknown number
 uint32 vb_temp5 = BE_read_uint32(ifile);

 // skip vertex index data
 ifile.seekg(vb_temp4, ios::cur);
 if(ifile.fail()) return error("Seek failure.");

 cout << "end of vertex index data at 0x" << hex << (uint32)ifile.tellg() << dec << endl;

 // read zero
 BE_read_uint32(ifile);

 // read number of vertices
 uint32 vb_elem = BE_read_uint32(ifile);
 uint32 vb_offset = (uint32)ifile.tellg();
 uint32 vb_size = vb_elem * 0x24;

 //
 // VERTEX BUFFER PROCESSING
 //

 // move to vertex buffer
 ifile.seekg(vb_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vertex buffer
 boost::shared_array<char> vb_data(new char[vb_size]);
 ifile.read(vb_data.get(), vb_size);
 if(ifile.fail()) return error("Read failure.");

 // initialize vertex buffer
 AMC_VTXBUFFER vb;
 vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;// | AMC_VERTEX_NORMAL;
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
 vb.elem = vb_elem;
 vb.data.reset(new AMC_VERTEX[vb.elem]);

 // process vertices (36 bytes per vertex)
 binary_stream vbs(vb_data, vb_size);
 for(uint32 i = 0; i < vb_elem; i++)
    {
     vb.data[i].vx = vbs.BE_read_real32();
     vb.data[i].vy = vbs.BE_read_real32();
     vb.data[i].vz = vbs.BE_read_real32();
     vb.data[i].nx = (vbs.BE_read_uint08()/255.0f)*2.0f - 1.0f;
     vb.data[i].ny = (vbs.BE_read_uint08()/255.0f)*2.0f - 1.0f;
     vb.data[i].nz = (vbs.BE_read_uint08()/255.0f)*2.0f - 1.0f;
     vb.data[i].nw = (vbs.BE_read_uint08()/255.0f)*2.0f - 1.0f;
     vb.data[i].tx = (vbs.BE_read_uint08()/255.0f)*2.0f - 1.0f;
     vb.data[i].ty = (vbs.BE_read_uint08()/255.0f)*2.0f - 1.0f;
     vb.data[i].tz = (vbs.BE_read_uint08()/255.0f)*2.0f - 1.0f;
     vb.data[i].tw = (vbs.BE_read_uint08()/255.0f)*2.0f - 1.0f;
     vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[i].wi[3] = vbs.BE_read_uint08();
     vb.data[i].wi[2] = vbs.BE_read_uint08();
     vb.data[i].wi[1] = vbs.BE_read_uint08();
     vb.data[i].wi[0] = vbs.BE_read_uint08();
     vb.data[i].wv[7] = 0.0f;
     vb.data[i].wv[6] = 0.0f;
     vb.data[i].wv[5] = 0.0f;
     vb.data[i].wv[4] = 0.0f;
     vb.data[i].wv[3] = vbs.BE_read_uint08()/255.0f;
     vb.data[i].wv[2] = vbs.BE_read_uint08()/255.0f;
     vb.data[i].wv[1] = vbs.BE_read_uint08()/255.0f;
     vb.data[i].wv[0] = vbs.BE_read_uint08()/255.0f;
     vb.data[i].uv[0][0] = vbs.BE_read_real16(); // 1st UV channel
     vb.data[i].uv[0][1] = vbs.BE_read_real16(); // 1st UV channel
     vb.data[i].uv[1][0] = vbs.BE_read_real16(); // 2nd UV channel
     vb.data[i].uv[1][1] = vbs.BE_read_real16(); // 2nd UV channel

     // first weight that is zero, the rest are also zero
     if(vb.data[i].wv[0] == 0) {
        vb.data[i].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
     else if(vb.data[i].wv[1] == 0) {
        vb.data[i].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
     else if(vb.data[i].wv[2] == 0) {
        vb.data[i].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
     else if(vb.data[i].wv[3] == 0) {
        vb.data[i].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
    }

 // insert vertex buffer
 amc.vblist.push_back(vb);

 //
 // INDEX BUFFER PROCESSING
 //

 // move to vertex buffer
 ifile.seekg(ib_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vertex buffer
 boost::shared_array<char> ib_data(new char[ib_size]);
 BE_read_array(ifile, (uint16*)ib_data.get(), ib_elem);
 if(ifile.fail()) return error("Read failure.");

 // allocate index buffer
 AMC_IDXBUFFER ib;
 ib.format = AMC_UINT16;
 ib.type = AMC_TRIANGLES;
 ib.wind = AMC_CCW;
 ib.reserved = 0;
 ib.name = "default";
 ib.elem = ib_elem;
 ib.data = ib_data;
     
 // insert index buffer
 amc.iblist.push_back(ib);

 //
 // SURFACE PROCESSING
 //

 // move to vertex buffer
 ifile.seekg(surf_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vertex buffer
 boost::shared_array<char> surf_data(new char[surf_size]);
 ifile.read(surf_data.get(), surf_size);
 if(ifile.fail()) return error("Read failure.");

 binary_stream sbs(surf_data, surf_size);
 for(uint32 i = 0; i < surf_elem; i++)
    {
     // read surface data
     uint16 p01 = sbs.BE_read_uint16(); // unknown
     uint16 p02 = sbs.BE_read_uint16(); // index
     uint32 p03 = sbs.BE_read_uint32(); // start index
     uint16 p04 = sbs.BE_read_uint16(); // number of triangles

     cout << "Surface: 0x" << hex << i << dec << endl;
     cout << "Index: 0x" << hex << p02 << dec << endl;
     cout << "Start: 0x" << hex << p03 << dec << endl;
     cout << "Number of triangles: 0x" << hex << p04 << dec << endl;
     cout << endl;

     // create mesh name
     stringstream ss;
     ss << "mesh_" << setfill('0') << setw(3) << i;

     // create surface
     AMC_SURFACE sur;
     sur.name = ss.str();
     AMC_REFERENCE ref;
     ref.vb_index = 0;
     ref.vb_start = 0;
     ref.vb_width = vb_elem;
     ref.ib_index = 0;
     ref.ib_start = p03;
     ref.ib_width = p04 * 3;
     ref.jm_index = (do_skeleton ? i : AMC_INVALID_JMAP_INDEX);
     sur.refs.push_back(ref);
     sur.surfmat = AMC_INVALID_SURFMAT;

     // insert surface
     amc.surfaces.push_back(sur);
    }

 //
 // SKELETON PROCESSING
 //

 // move to parent data
 ifile.seekg(jparent_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read parent data
 boost::shared_array<uint16> parents(new uint16[n_joints]);
 if(!BE_read_array(ifile, parents.get(), n_joints)) return error("Read failure.");

 // move to joint data
 ifile.seekg(jntdata_offset);
 if(ifile.fail()) return error("Seek failure.");

 AMC_SKELETON2 skel2;
 skel2.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel2.name = "skeleton";

 // read joint data
 for(uint32 i = 0; i < n_joints; i++)
    {
     real32 p01 = BE_read_real32(ifile); // 0x04 unit quaternion
     real32 p02 = BE_read_real32(ifile); // 0x08 unit quaternion
     real32 p03 = BE_read_real32(ifile); // 0x0C unit quaternion
     real32 p04 = BE_read_real32(ifile); // 0x10 unit quaternion
     real32 p05 = BE_read_real32(ifile); // 0x14 position
     real32 p06 = BE_read_real32(ifile); // 0x18 position
     real32 p07 = BE_read_real32(ifile); // 0x1C position

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

 // transform and save
 if(do_skeleton) {
    ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
    SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
   }
 else {
    stringstream ss;
    ss << shrtname << "_gooduv";
    SaveAMC(pathname.c_str(), ss.str().c_str(), amc);
   }
 return true;
}

bool DeswizzleDXT1(const char* filename)
{
 // open DDS file
 cout << "Deswizzling " << filename << "." << endl;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 uint32 h01 = LE_read_uint32(ifile);
 uint32 h02 = LE_read_uint32(ifile); // dwSize;
 uint32 h03 = LE_read_uint32(ifile); // dwFlags;
 uint32 h04 = LE_read_uint32(ifile); // dwHeight;
 uint32 h05 = LE_read_uint32(ifile); // dwWidth;
 uint32 h06 = LE_read_uint32(ifile); // dwPitchOrLinearSize;
 uint32 h07 = LE_read_uint32(ifile); // dwDepth;
 uint32 h08 = LE_read_uint32(ifile); // dwMipMapCount;
 uint32 h09[11];                     // reserved
 LE_read_array(ifile, &h09[0], 11);
 uint32 h10 = LE_read_uint32(ifile); // dwSize;
 uint32 h11 = LE_read_uint32(ifile); // dwFlags;
 uint32 h12 = LE_read_uint32(ifile); // dwFourCC;
 uint32 h13 = LE_read_uint32(ifile); // dwRGBBitCount;
 uint32 h14 = LE_read_uint32(ifile); // dwRBitMask;
 uint32 h15 = LE_read_uint32(ifile); // dwGBitMask;
 uint32 h16 = LE_read_uint32(ifile); // dwBBitMask;
 uint32 h17 = LE_read_uint32(ifile); // dwABitMask;
 uint32 h18 = LE_read_uint32(ifile); // dwCaps;
 uint32 h19 = LE_read_uint32(ifile); // dwCaps2;
 uint32 h20 = LE_read_uint32(ifile); // dwCaps3;
 uint32 h21 = LE_read_uint32(ifile); // dwCaps4;
 uint32 h22 = LE_read_uint32(ifile); // dwReserved2;

 // read image data
 uint32 size = 0;
 if(h12 == 0x31545844) size = DXT1Filesize(h05, h04, h08);
 else if(h12 == 0x35545844) size = DXT5Filesize(h05, h04, h08);
 boost::shared_array<char> data(new char[size]);
 boost::shared_array<char> copy(new char[size]);
 ifile.read(data.get(), size);
 if(ifile.fail()) return error("Read failure.");

 // endian swap
 uint32 pitch = 0;
 if(h12 == 0x31545844) pitch = (h05/4) * 8;
 else if(h12 == 0x35545844) pitch = (h05/4) * 16;
 XGEndianSwapSurface(copy.get(), pitch, data.get(), pitch, h05, h04, D3DFMT_DXT1);
 std::swap(data, copy);

 // DXT1 pitch...
 // 16 dx
 // 4 x 4 blocks
 // 16 dx/4 = 4 blocks
 // number of bytes in 4 blocks = 4 * 8

 // compute pitch
 DWORD gpuformat = 0;
 if(h12 == 0x31545844) gpuformat = XGGetGpuFormat(D3DFMT_DXT1);
 else if(h12 == 0x35545844) gpuformat = XGGetGpuFormat(D3DFMT_DXT5);
 XGUntileTextureLevel(h05, h04, 0, gpuformat, XGTILE_NONPACKED, copy.get(), pitch, NULL, data.get(), NULL);

 // create output file
 stringstream ss;
 ss << GetPathnameFromFilename(filename);
 ss << GetShortFilenameWithoutExtension(filename);
 ss << "_deswizzled.dds";
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // save data
 LE_write_uint32(ofile, h01);
 LE_write_uint32(ofile, h02); // dwSize;
 LE_write_uint32(ofile, h03); // dwFlags;
 LE_write_uint32(ofile, h04); // dwHeight;
 LE_write_uint32(ofile, h05); // dwWidth;
 LE_write_uint32(ofile, h06); // dwPitchOrLinearSize;
 LE_write_uint32(ofile, h07); // dwDepth;
 LE_write_uint32(ofile, h08); // dwMipMapCount;
 LE_write_array(ofile, &h09[0], 11);
 LE_write_uint32(ofile, h10); // dwSize;
 LE_write_uint32(ofile, h11); // dwFlags;
 LE_write_uint32(ofile, h12); // dwFourCC;
 LE_write_uint32(ofile, h13); // dwRGBBitCount;
 LE_write_uint32(ofile, h14); // dwRBitMask;
 LE_write_uint32(ofile, h15); // dwGBitMask;
 LE_write_uint32(ofile, h16); // dwBBitMask;
 LE_write_uint32(ofile, h17); // dwABitMask;
 LE_write_uint32(ofile, h18); // dwCaps;
 LE_write_uint32(ofile, h19); // dwCaps2;
 LE_write_uint32(ofile, h20); // dwCaps3;
 LE_write_uint32(ofile, h21); // dwCaps4;
 LE_write_uint32(ofile, h22); // dwReserved2;
 ofile.write(copy.get(), size);

 return true;
}

bool DeswizzleTGA32(const char* filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read image ID
 uint08 imageID = LE_read_uint08(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read color map type
 uint08 colorMapType = LE_read_uint08(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(colorMapType != 0) return error("32-bit TGA files cannot have a color map.");

 // read image type
 uint08 imageType = LE_read_uint08(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(imageType != 0x02) return error("32-bit TGA files must be uncompressed true-color images.");

 // read useless values
 uint16 useless01 = LE_read_uint16(ifile);
 uint16 useless02 = LE_read_uint16(ifile);
 uint08 useless03 = LE_read_uint08(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(useless01 != 0x00) return error("Invalid first entry index.");
 if(useless02 != 0x00) return error("Invalid color map length.");
 if(useless03 != 0x00) return error("Invalid color map entry size.");
 
 // read origins
 uint16 xOrigin = LE_read_uint16(ifile);
 uint16 yOrigin = LE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read dimensions
 uint16 dx = LE_read_uint16(ifile);
 uint16 dy = LE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(!dx) return error("Invalid TGA image dimensions.");
 if(!dy) return error("Invalid TGA image dimensions.");

 // read pixel depth
 uint08 pixelDepth = LE_read_uint08(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(pixelDepth != 0x20) return error("Expecting 32-bit TGA.");

 // read image descriptor
 uint08 imageDescriptor = LE_read_uint08(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read image data
 uint32 size = 4 * dx * dy;
 boost::shared_array<char> data(new char[size]);
 boost::shared_array<char> copy(new char[size]);
 ifile.read(data.get(), size);
 if(ifile.fail()) return error("Read failure.");

 // deswizzle
 for(uint32 r = 0; r < dy; r++) {
     for(uint32 c = 0; c < dx; c++) {
         uint32 morton = array_to_morton(r, c);
         uint32 copy_position = 4*r*dx + 4*c;
         uint32 data_position = 4*morton;
         copy[copy_position++] = data[data_position + 0]; // b
         copy[copy_position++] = data[data_position + 1]; // g
         copy[copy_position++] = data[data_position + 2]; // r
         copy[copy_position++] = data[data_position + 3]; // a
        }
    }

 // create output file
 stringstream ss;
 ss << GetPathnameFromFilename(filename);
 ss << GetShortFilenameWithoutExtension(filename);
 ss << "_deswizzled.tga";
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // save image ID
 LE_write_uint08(ofile, imageID);
 if(ofile.fail()) return error("Write failure.");

 // save color map type
 LE_write_uint08(ofile, colorMapType);
 if(ofile.fail()) return error("Write failure.");

 // save image type
 LE_write_uint08(ofile, imageType);
 if(ofile.fail()) return error("Write failure.");

 // save useless values
 LE_write_uint16(ofile, useless01);
 LE_write_uint16(ofile, useless02);
 LE_write_uint08(ofile, useless03);
 if(ofile.fail()) return error("Write failure.");
 
 // save origins
 LE_write_uint16(ofile, xOrigin);
 LE_write_uint16(ofile, yOrigin);
 if(ofile.fail()) return error("Write failure.");

 // save dimensions
 LE_write_uint16(ofile, dx);
 LE_write_uint16(ofile, dy);
 if(ofile.fail()) return error("Write failure.");

 // save pixel depth
 LE_write_uint08(ofile, pixelDepth);
 if(ofile.fail()) return error("Write failure.");

 // save image descriptor
 LE_write_uint08(ofile, imageDescriptor);
 if(ofile.fail()) return error("Write failure.");

 // save image data
 ofile.write(copy.get(), size);
 if(ofile.fail()) return error("Write failure.");

 return true;
}

bool TestInjusticeTexture(const char* filename, uint32 dx, uint32 dy, uint32 type, uint32 offset)
{
 // important stuff
 // using namespace std;
 // char* filename = "c:\\users\\semory\\desktop\\test\\texture10.bin";
 // uint32 dx = 0x200;
 // uint32 dy = 0x200;
 // uint32 type = 0x5;
 // uint32 size = 0x0;
 // uint32 offset = 0x192;
 cout << "Processing " << filename << "." << endl;

 // determine size of data to read
 uint32 size = 0;
 if(type == 0x05) size = DXT1Filesize(dx, dy, 0);
 else if(type == 0x07) size = DXT5Filesize(dx, dy, 0);
 else return error("Unknown image type.");

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // seek data
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read data
 boost::shared_array<char> data(new char[size]);
 if(type == 0x05) ifile.read(data.get(), size);
 else if(type == 0x07) ifile.read(data.get(), size);
 if(ifile.fail()) return error("Read failure.");

 // create output file
 stringstream ss;
 ss << GetPathnameFromFilename(filename);
 ss << GetShortFilenameWithoutExtension(filename);
 ss << ".dds";
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("");

 // save data
 DDS_HEADER ddsh;
 if(type == 0x05) CreateDXT1Header(dx, dy, 0, FALSE, &ddsh);
 else if(type == 0x07) CreateDXT5Header(dx, dy, 0, FALSE, &ddsh);
 ofile.write("DDS ", 4);
 ofile.write((char*)&ddsh, sizeof(ddsh));
 ofile.write(data.get(), size);
 if(ofile.fail()) return error("Write failure.");

 return true;
}

int main()
{
 XBOX360::RON::extract();

 //PC::RE7::extract("C:\\Users\\semory\\sticklove\\source\\xentax\\RE7\\");

 //TestInjustice();
                                                                   // 0x020 0x040 0x080 0x100 0x200 0x400 0x800
                                                                   // 0x0EA 0x0ED 0x106 0x157 0x192
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture30.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture31.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture32.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture33.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture34.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture35.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture36.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture37.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture38.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture39.bin", 0x, 0x, 0x, 0x);
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture30.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture31.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture32.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture33.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture34.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture35.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture36.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture37.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture38.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\CHAR_HarleyQuinn_C\\texture39.dds");

 return 0;
}