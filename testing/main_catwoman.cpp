#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
using namespace std;

#include "x_stream.h"
#include "x_amc.h"
#include "x_dsp.h"
#include "x_bmp.h"
#include "x_dds.h"
#include "x_mod.h"

bool TestXNALara1(void)
{
 // create output file
 ofstream ofile("generic_item.mesh", ios::binary);
 if(!ofile) return error("error");

 // save number of bones
 uint32 n_bones = 1;
 ofile.write((char*)&n_bones, sizeof(n_bones));

 // save bone name
 const uint08 b_namelen = 4;
 const char* b_name = "root";
 ofile.write((char*)&b_namelen, sizeof(b_namelen));
 ofile.write((char*)b_name, b_namelen);
 uint16 parent = 0xFFFF;
 real32 bx = 0.0f;
 real32 by = 0.0f;
 real32 bz = 0.0f;
 ofile.write((char*)&parent, sizeof(parent));
 ofile.write((char*)&bx, sizeof(bx));
 ofile.write((char*)&by, sizeof(by));
 ofile.write((char*)&bz, sizeof(bz));

 // save number of mehses
 uint32 n_meshes = 1;
 ofile.write((char*)&n_meshes, sizeof(n_meshes));

 // save mesh #1 name
 const uint08 m_namelen = 10;
 const char* m_name = "5_test_0.1";
 ofile.write((char*)&m_namelen, sizeof(m_namelen));
 ofile.write((char*)m_name, m_namelen);

 // save mesh #1 uv layers
 uint32 n_layers = 1;
 ofile.write((char*)&n_layers, sizeof(n_layers));

 // save mesh #1 textures
 uint32 n_textures = 1;
 ofile.write((char*)&n_textures, sizeof(n_textures));

 // save mesh #1 texture #1
 const uint08 mt_namelen = 26;
 const char* mt_name = "data\\aaaa\\test_diffuse.png";
 uint32 mt_layerindex = 0;
 ofile.write((char*)&mt_namelen, sizeof(mt_namelen));
 ofile.write((char*)mt_name, mt_namelen);
 ofile.write((char*)&mt_layerindex, sizeof(mt_layerindex));

 //
 // mesh #1 vertices
 //

 // save number of vertices
 uint32 n_vertices = 4;
 ofile.write((char*)&n_vertices, sizeof(n_vertices));

 // vertex #1
 real32 vx = -10.0f;
 real32 vy =  2.0f;
 real32 vz = -10.0f;
 real32 nx = 0.0f;
 real32 ny = 1.0f;
 real32 nz = 0.0f;
 uint08 col_r = 0;
 uint08 col_g = 0;
 uint08 col_b = 0;
 uint08 col_a = 255;
 real32 tex_u = 0.0f;
 real32 tex_v = 0.0f;
 real32 tan_x = 1.0f;
 real32 tan_y = 0.0f;
 real32 tan_z = 0.0f;
 real32 tan_w = 0.0f;
 uint16 bi1 = 0;
 uint16 bi2 = 0;
 uint16 bi3 = 0;
 uint16 bi4 = 0;
 real32 bw1 = 1.0f;
 real32 bw2 = 0.0f;
 real32 bw3 = 0.0f;
 real32 bw4 = 0.0f;
 ofile.write((char*)&vx, sizeof(vx));
 ofile.write((char*)&vy, sizeof(vy));
 ofile.write((char*)&vz, sizeof(vz));
 ofile.write((char*)&nx, sizeof(nx));
 ofile.write((char*)&ny, sizeof(ny));
 ofile.write((char*)&nz, sizeof(nz));
 ofile.write((char*)&col_r, sizeof(col_r));
 ofile.write((char*)&col_g, sizeof(col_g));
 ofile.write((char*)&col_b, sizeof(col_b));
 ofile.write((char*)&col_a, sizeof(col_a));
 ofile.write((char*)&tex_u, sizeof(tex_u));
 ofile.write((char*)&tex_v, sizeof(tex_v));
 ofile.write((char*)&tan_x, sizeof(tan_x));
 ofile.write((char*)&tan_y, sizeof(tan_y));
 ofile.write((char*)&tan_z, sizeof(tan_z));
 ofile.write((char*)&tan_w, sizeof(tan_w));
 ofile.write((char*)&bi1, sizeof(bi1));
 ofile.write((char*)&bi2, sizeof(bi2));
 ofile.write((char*)&bi3, sizeof(bi3));
 ofile.write((char*)&bi4, sizeof(bi4));
 ofile.write((char*)&bw1, sizeof(bw1));
 ofile.write((char*)&bw2, sizeof(bw2));
 ofile.write((char*)&bw3, sizeof(bw3));
 ofile.write((char*)&bw4, sizeof(bw4));

 // vertex #2
 vx = -10.0f;
 vy =  2.0f;
 vz =  10.0f;
 nx = 0.0f;
 ny = 1.0f;
 nz = 0.0f;
 col_r = 0;
 col_g = 0;
 col_b = 0;
 col_a = 255;
 tex_u = 0.0f;
 tex_v = 1.0f;
 tan_x = 1.0f;
 tan_y = 0.0f;
 tan_z = 0.0f;
 tan_w = 0.0f;
 bi1 = 0;
 bi2 = 0;
 bi3 = 0;
 bi4 = 0;
 bw1 = 1.0f;
 bw2 = 0.0f;
 bw3 = 0.0f;
 bw4 = 0.0f;
 ofile.write((char*)&vx, sizeof(vx));
 ofile.write((char*)&vy, sizeof(vy));
 ofile.write((char*)&vz, sizeof(vz));
 ofile.write((char*)&nx, sizeof(nx));
 ofile.write((char*)&ny, sizeof(ny));
 ofile.write((char*)&nz, sizeof(nz));
 ofile.write((char*)&col_r, sizeof(col_r));
 ofile.write((char*)&col_g, sizeof(col_g));
 ofile.write((char*)&col_b, sizeof(col_b));
 ofile.write((char*)&col_a, sizeof(col_a));
 ofile.write((char*)&tex_u, sizeof(tex_u));
 ofile.write((char*)&tex_v, sizeof(tex_v));
 ofile.write((char*)&tan_x, sizeof(tan_x));
 ofile.write((char*)&tan_y, sizeof(tan_y));
 ofile.write((char*)&tan_z, sizeof(tan_z));
 ofile.write((char*)&tan_w, sizeof(tan_w));
 ofile.write((char*)&bi1, sizeof(bi1));
 ofile.write((char*)&bi2, sizeof(bi2));
 ofile.write((char*)&bi3, sizeof(bi3));
 ofile.write((char*)&bi4, sizeof(bi4));
 ofile.write((char*)&bw1, sizeof(bw1));
 ofile.write((char*)&bw2, sizeof(bw2));
 ofile.write((char*)&bw3, sizeof(bw3));
 ofile.write((char*)&bw4, sizeof(bw4));

 // vertex #3
 vx =  10.0f;
 vy =  2.0f;
 vz = -10.0f;
 nx = 0.0f;
 ny = 1.0f;
 nz = 0.0f;
 col_r = 0;
 col_g = 0;
 col_b = 0;
 col_a = 255;
 tex_u = 1.0f;
 tex_v = 0.0f;
 tan_x = 1.0f;
 tan_y = 0.0f;
 tan_z = 0.0f;
 tan_w = 0.0f;
 bi1 = 0;
 bi2 = 0;
 bi3 = 0;
 bi4 = 0;
 bw1 = 1.0f;
 bw2 = 0.0f;
 bw3 = 0.0f;
 bw4 = 0.0f;
 ofile.write((char*)&vx, sizeof(vx));
 ofile.write((char*)&vy, sizeof(vy));
 ofile.write((char*)&vz, sizeof(vz));
 ofile.write((char*)&nx, sizeof(nx));
 ofile.write((char*)&ny, sizeof(ny));
 ofile.write((char*)&nz, sizeof(nz));
 ofile.write((char*)&col_r, sizeof(col_r));
 ofile.write((char*)&col_g, sizeof(col_g));
 ofile.write((char*)&col_b, sizeof(col_b));
 ofile.write((char*)&col_a, sizeof(col_a));
 ofile.write((char*)&tex_u, sizeof(tex_u));
 ofile.write((char*)&tex_v, sizeof(tex_v));
 ofile.write((char*)&tan_x, sizeof(tan_x));
 ofile.write((char*)&tan_y, sizeof(tan_y));
 ofile.write((char*)&tan_z, sizeof(tan_z));
 ofile.write((char*)&tan_w, sizeof(tan_w));
 ofile.write((char*)&bi1, sizeof(bi1));
 ofile.write((char*)&bi2, sizeof(bi2));
 ofile.write((char*)&bi3, sizeof(bi3));
 ofile.write((char*)&bi4, sizeof(bi4));
 ofile.write((char*)&bw1, sizeof(bw1));
 ofile.write((char*)&bw2, sizeof(bw2));
 ofile.write((char*)&bw3, sizeof(bw3));
 ofile.write((char*)&bw4, sizeof(bw4));

 // vertex #4
 vx =  10.0f;
 vy =  2.0f;
 vz =  10.0f;
 nx = 0.0f;
 ny = 1.0f;
 nz = 0.0f;
 col_r = 0;
 col_g = 0;
 col_b = 0;
 col_a = 255;
 tex_u = 1.0f;
 tex_v = 1.0f;
 tan_x = 1.0f;
 tan_y = 0.0f;
 tan_z = 0.0f;
 tan_w = 0.0f;
 bi1 = 0;
 bi2 = 0;
 bi3 = 0;
 bi4 = 0;
 bw1 = 1.0f;
 bw2 = 0.0f;
 bw3 = 0.0f;
 bw4 = 0.0f;
 ofile.write((char*)&vx, sizeof(vx));
 ofile.write((char*)&vy, sizeof(vy));
 ofile.write((char*)&vz, sizeof(vz));
 ofile.write((char*)&nx, sizeof(nx));
 ofile.write((char*)&ny, sizeof(ny));
 ofile.write((char*)&nz, sizeof(nz));
 ofile.write((char*)&col_r, sizeof(col_r));
 ofile.write((char*)&col_g, sizeof(col_g));
 ofile.write((char*)&col_b, sizeof(col_b));
 ofile.write((char*)&col_a, sizeof(col_a));
 ofile.write((char*)&tex_u, sizeof(tex_u));
 ofile.write((char*)&tex_v, sizeof(tex_v));
 ofile.write((char*)&tan_x, sizeof(tan_x));
 ofile.write((char*)&tan_y, sizeof(tan_y));
 ofile.write((char*)&tan_z, sizeof(tan_z));
 ofile.write((char*)&tan_w, sizeof(tan_w));
 ofile.write((char*)&bi1, sizeof(bi1));
 ofile.write((char*)&bi2, sizeof(bi2));
 ofile.write((char*)&bi3, sizeof(bi3));
 ofile.write((char*)&bi4, sizeof(bi4));
 ofile.write((char*)&bw1, sizeof(bw1));
 ofile.write((char*)&bw2, sizeof(bw2));
 ofile.write((char*)&bw3, sizeof(bw3));
 ofile.write((char*)&bw4, sizeof(bw4));

 //
 // mesh #1 triangles
 //

 // save number of vertices
 uint32 n_triangles = 2;
 ofile.write((char*)&n_triangles, sizeof(n_triangles));

 // triangle #1
 uint32 a = 0;
 uint32 b = 2;
 uint32 c = 1;
 ofile.write((char*)&a, sizeof(a));
 ofile.write((char*)&b, sizeof(b));
 ofile.write((char*)&c, sizeof(c));

 // triangle #2
 a = 1;
 b = 2;
 c = 3;
 ofile.write((char*)&a, sizeof(a));
 ofile.write((char*)&b, sizeof(b));
 ofile.write((char*)&c, sizeof(c));

 return true;
}

// PS3 includes
#include "ps3_bioshock.h"
#include "ps3_mgrr.h"
#include "ps3_macross30.h"
#include "ps3_one_piece2.h"
#include "ps3_re6.h"
#include "ps3_riseguard.h"

// XBOX360 includes
#include "xb360_injustice.h"
#include "xb360_rog.h"
#include "xb360_ttt2.h"

// PC includes
#include "pc_tr2013.h"

#include<cs/math.h>
#include "x_mod.h"

// xbox graphics include
#include "d3d9.h"
#include "d3d9types.h"
#include "xgraphics.h"

bool TestTalesGracesVB0(ifstream& ifile, ofstream& ofile, uint32 n_points)
{
 for(uint32 i = 0; i < n_points; i++)
    {
     real32 vx = BE_read_real32(ifile);
     real32 vy = BE_read_real32(ifile);
     real32 vz = BE_read_real32(ifile);
     ofile << "v " << vx << " " << vy << " " << vz << endl;

     real32 nx = BE_read_real32(ifile);
     real32 ny = BE_read_real32(ifile);
     real32 nz = BE_read_real32(ifile);
     ofile << "vn " << nx << " " << ny << " " << nz << endl;
    }

 return true;
}

bool TestTalesGracesVB1(ifstream& ifile, ofstream& ofile, uint32 n_points)
{
 for(uint32 i = 0; i < n_points; i++)
    {
     real32 vx = BE_read_real32(ifile);
     real32 vy = BE_read_real32(ifile);
     real32 vz = BE_read_real32(ifile);
     ofile << "v " << vx << " " << vy << " " << vz << endl;

     real32 nx = BE_read_real32(ifile);
     real32 ny = BE_read_real32(ifile);
     real32 nz = BE_read_real32(ifile);
     ofile << "vn " << nx << " " << ny << " " << nz << endl;

     uint16 u1 = BE_read_uint08(ifile); // bi1, bw1 = 1.0
     uint16 u2 = BE_read_uint08(ifile); // 0
     uint16 u3 = BE_read_uint08(ifile); // 0
     uint16 u4 = BE_read_uint08(ifile); // 0
    }

 return true;
}

bool TestTalesGracesVB2(ifstream& ifile, ofstream& ofile, uint32 n_points)
{
 for(uint32 i = 0; i < n_points; i++)
    {
     real32 vx = BE_read_real32(ifile);
     real32 vy = BE_read_real32(ifile);
     real32 vz = BE_read_real32(ifile);
     ofile << "v " << vx << " " << vy << " " << vz << endl;

     real32 nx = BE_read_real32(ifile);
     real32 ny = BE_read_real32(ifile);
     real32 nz = BE_read_real32(ifile);
     ofile << "vn " << nx << " " << ny << " " << nz << endl;

     uint16 u1 = BE_read_uint08(ifile); // bi1, bw1 = 1.0
     uint16 u2 = BE_read_uint08(ifile); // 0
     uint16 u3 = BE_read_uint08(ifile); // 0
     uint16 u4 = BE_read_uint08(ifile); // 0

     real32 w2 = BE_read_real32(ifile);
     real32 w1 = 1.0f - w2;
    }

 return true;
}

bool TestTalesGracesVB3(ifstream& ifile, ofstream& ofile, uint32 n_points)
{
 for(uint32 i = 0; i < n_points; i++)
    {
     real32 vx = BE_read_real32(ifile);
     real32 vy = BE_read_real32(ifile);
     real32 vz = BE_read_real32(ifile);
     ofile << "v " << vx << " " << vy << " " << vz << endl;

     real32 nx = BE_read_real32(ifile);
     real32 ny = BE_read_real32(ifile);
     real32 nz = BE_read_real32(ifile);
     ofile << "vn " << nx << " " << ny << " " << nz << endl;

     uint16 u1 = BE_read_uint08(ifile);
     uint16 u2 = BE_read_uint08(ifile);
     uint16 u3 = BE_read_uint08(ifile);
     uint16 u4 = BE_read_uint08(ifile);

     real32 w2 = BE_read_real32(ifile);
     real32 w3 = BE_read_real32(ifile);
     real32 w1 = 1.0f - w2 - w3;
    }

 return true;
}

bool TestTalesGracesVB4(ifstream& ifile, ofstream& ofile, uint32 n_points)
{
 for(uint32 i = 0; i < n_points; i++)
    {
     real32 vx = BE_read_real32(ifile);
     real32 vy = BE_read_real32(ifile);
     real32 vz = BE_read_real32(ifile);
     ofile << "v " << vx << " " << vy << " " << vz << endl;

     real32 nx = BE_read_real32(ifile);
     real32 ny = BE_read_real32(ifile);
     real32 nz = BE_read_real32(ifile);
     ofile << "vn " << nx << " " << ny << " " << nz << endl;

     uint16 u1 = BE_read_uint08(ifile);
     uint16 u2 = BE_read_uint08(ifile);
     uint16 u3 = BE_read_uint08(ifile);
     uint16 u4 = BE_read_uint08(ifile);

     real32 w2 = BE_read_real32(ifile);
     real32 w3 = BE_read_real32(ifile);
     real32 w4 = BE_read_real32(ifile);
     real32 w1 = 1.0f - w2 - w3 - w4;
    }

 return true;
}

bool TestTalesGraces(void)
{
 // open file
 string filename = "G:\\xentax\\Tales of Graces F\\temp\\ene010\\CRIC00A.MDL";
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename information
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // seek UV map information
 ifile.seekg(0x34);
 if(ifile.fail()) return error("Seek failure.");

 // read UV map information
 uint32 uv_offset = BE_read_uint32(ifile);
 uint32 uv_size1 = BE_read_uint32(ifile);
 uint32 uv_size2 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // seek texture information offset
 ifile.seekg(0x70);
 if(ifile.fail()) return error("Seek failure.");

 // read texture information offset
 uint32 tex_info_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // seek texture data offset
 ifile.seekg(0x7C);
 if(ifile.fail()) return error("Seek failure.");

 // read texture data offset
 uint32 tex_data_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // seek texture information
 ifile.seekg(tex_info_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 uint32 th_01 = BE_read_uint32(ifile); // 0x20000
 uint32 th_02 = BE_read_uint32(ifile); // length from end header to data
 uint32 th_03 = BE_read_uint32(ifile); // 0x0000000A
 uint32 th_04 = BE_read_uint32(ifile); // number of textures
 uint32 th_05 = BE_read_uint32(ifile); // 0x00000
 if(ifile.fail()) return error("Read failure.");

 struct TEXINFO {
  uint32 zero;
  uint32 dx;
  uint32 dy;
  uint32 mipmaps;
  uint32 flags;
  uint32 unknown; // possibly type?
  uint32 offset;
 };
 deque<TEXINFO> texinfo;

 // read texture information
 for(uint32 i = 0; i < th_04; i++)
    {
     TEXINFO item;
     item.zero = BE_read_uint32(ifile);
     item.dx = BE_read_uint32(ifile);
     item.dy = BE_read_uint32(ifile);
     item.mipmaps = BE_read_uint32(ifile);
     item.flags = BE_read_uint32(ifile);
     item.unknown = BE_read_uint32(ifile);
     item.offset = BE_read_uint32(ifile);
     texinfo.push_back(item);
    }

 // read 0x00000000
 uint32 zero = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read filenames
 deque<string> texname;
 for(uint32 i = 0; i < th_04; i++)
    {
     // read name
     char name[1024];
     if(!read_string(ifile, &name[0], 1024))
        return error("Read failure.");

     // save name
     string temp = name;
     texname.push_back(temp);
     cout << temp << endl;
    }

 // read textures
 for(uint32 i = 0; i < th_04; i++)
    {
     // seek image data
     TEXINFO item = texinfo[i];
     uint32 position = tex_data_offset + item.offset;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // compute texture data size
     uint32 size = 0;
     if(item.flags == 0x8602AAE4)
        size = DXT1Filesize(item.dx, item.dy, item.mipmaps - 1);
     else if(item.flags == 0x8804AAE4)
        size = DXT5Filesize(item.dx, item.dy, item.mipmaps - 1);

     // create filename
     string ofname = pathname;
     ofname += texname[i];
     ofname += ".dds";

     // create file
     ofstream ofile(ofname.c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save DDS header
     DDS_HEADER ddsh;
     if(item.flags == 0x8602AAE4)
        CreateDXT1Header(item.dx, item.dy, item.mipmaps - 1, FALSE, &ddsh);
     else if(item.flags == 0x8804AAE4)
        CreateDXT5Header(item.dx, item.dy, item.mipmaps - 1, FALSE, &ddsh);
     ofile.write("DDS ", 4);
     ofile.write((char*)&ddsh, sizeof(ddsh));
     if(ofile.fail()) return error("Write failure.");

     // read DDS data
     boost::shared_array<char> data(new char[size]);
     ifile.read(data.get(), size);
     if(ifile.fail()) return error("Read failure.");

     // save DDS data
     ofile.write(data.get(), size);
     if(ofile.fail()) return error("Write failure.");
    }

 // //
 // // UV MAP PROCESSING
 // //
 // cout << "READING UV DATA" << endl;
 // 
 // // seek UV map information
 // ifile.seekg(0x34);
 // if(ifile.fail()) return error("Seek failure.");
 // 
 // // read UV map information
 // uint32 uv_offset = BE_read_uint32(ifile);
 // uint32 uv_size1 = BE_read_uint32(ifile);
 // uint32 uv_size2 = BE_read_uint32(ifile);
 // if(ifile.fail()) return error("Read failure.");
 // 
 // // compute number of vertices
 // uint32 bytes_per_uv = 0xC;
 // uint32 n_uvs = uv_size2/bytes_per_uv;
 // 
 // // seek UV data
 // ifile.seekg(uv_offset);
 // if(ifile.fail()) return error("Seek failure.");
 // 
 // // read UV data
 // deque<pair<real32,real32>> uvdata;
 // for(uint32 i = 0; i < n_uvs; i++) {
 //     uint32 p01 = BE_read_uint32(ifile);
 //     real32 p02 = BE_read_real32(ifile);
 //     real32 p03 = BE_read_real32(ifile);
 //     pair<real32,real32> item(p02, p03);
 //     uvdata.push_back(item);
 //    }

 //
 // MESH INFORMATION PROCESSING
 //
 cout << "READING MESH INFORMATION" << endl;

 // seek geometry offset
 ifile.seekg(0x28);
 if(ifile.fail()) return error("Seek failure.");

 // read geometry information
 uint32 geo_offset = BE_read_uint32(ifile);
 uint32 geo_size1 = BE_read_uint32(ifile);
 uint32 geo_size2 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to geometry data
 ifile.seekg(geo_offset);
 if(ifile.fail()) return error("Seek failure.");

 // ASBE00A: 00010000 0003EB60 00000010 0000000B 0000006B (0x5F48 - 0x60F0 jntmap)
 // JURI00A: 00010000 000442C0 00000010 00000010 00000059
 // SOFI00A: 00010000 0003BEA0 00000010 0000000B 0000004D

 // read geometry header
 struct GEOHEADER {
  uint32 p01; // 0x010000
  uint32 p02; // total size of data
  uint32 p03; // number of 0x3C byte entries
  uint32 p04; // number of meshes
  uint32 p05; // number of 0x04 byte entries (number of joint map entries)
  uint32 p06; // 0x00
  uint32 p07; // 0x00
  uint32 p08; // 0x00
  uint32 p09; // 0x00
 };
 GEOHEADER gh;
 gh.p01 = BE_read_uint32(ifile);
 gh.p02 = BE_read_uint32(ifile);
 gh.p03 = BE_read_uint32(ifile);
 gh.p04 = BE_read_uint32(ifile);
 gh.p05 = BE_read_uint32(ifile);
 gh.p06 = BE_read_uint32(ifile);
 gh.p07 = BE_read_uint32(ifile);
 gh.p08 = BE_read_uint32(ifile);
 gh.p09 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 uint32 n_meshes = gh.p04;

 // read geometry information
 struct GEOINFO {
  real32 p01;
  real32 p02;
  real32 p03;
  real32 p04;
  uint32 p05; // number of vertices
  uint32 p06; // number of vertices
  uint32 p07; // number of vertices
  uint32 p08; // number of vertices
 };
 deque<GEOINFO> gilist;
 for(uint32 i = 0; i < n_meshes; i++)
    {
     GEOINFO item;
     item.p01 = BE_read_real32(ifile);
     item.p02 = BE_read_real32(ifile);
     item.p03 = BE_read_real32(ifile);
     item.p04 = BE_read_real32(ifile);
     item.p05 = BE_read_uint32(ifile);
     item.p06 = BE_read_uint32(ifile);
     item.p07 = BE_read_uint32(ifile);
     item.p08 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     gilist.push_back(item);
     cout << item.p01 << " ";
     cout << item.p02 << " ";
     cout << item.p03 << " ";
     cout << item.p04 << " ";
     cout << "0x" << hex << item.p05 << dec << " ";
     cout << "0x" << hex << item.p06 << dec << " ";
     cout << "0x" << hex << item.p07 << dec << " ";
     cout << "0x" << hex << item.p08 << dec << endl;
    }

 // skip next section
 ifile.seekg((n_meshes * 0x10), ios::cur);
 if(ifile.fail()) return error("Seek failure.");

 // read more geometry information
 struct GEOINFO2 {
  uint32 p01;
  uint32 p02;
  uint32 p03;
  uint32 p04;
  uint32 p05;
  uint32 p06; // offset into UV data
  uint32 p07; // offset from here into idxbuffer
  uint32 p07_offset;
  uint32 p08; // offset from here into vtxbuffer
  uint32 p08_offset;
  uint32 p09; // size of uv (bytes per vertex)
  uint32 p10;
  uint32 p11;
  uint32 p12;
  uint32 p13;
  uint32 p14;
  uint32 p14_offset;
  uint32 p15;
  uint32 p15_offset;
 };
 deque<GEOINFO2> gilist2;
 for(uint32 i = 0; i < n_meshes; i++)
    {
     uint32 position = (uint32)ifile.tellg();
     GEOINFO2 item;
     item.p01 = BE_read_uint32(ifile);
     item.p02 = BE_read_uint32(ifile);
     item.p03 = BE_read_uint32(ifile);
     item.p04 = BE_read_uint32(ifile);
     item.p05 = BE_read_uint32(ifile);
     item.p06 = BE_read_uint32(ifile);
     item.p07 = BE_read_uint32(ifile);
     item.p08 = BE_read_uint32(ifile);
     item.p09 = BE_read_uint32(ifile);
     item.p10 = BE_read_uint32(ifile);
     item.p11 = BE_read_uint32(ifile);
     item.p12 = BE_read_uint32(ifile);
     item.p13 = BE_read_uint32(ifile);
     item.p14 = BE_read_uint32(ifile);
     item.p15 = BE_read_uint32(ifile);
     item.p07_offset = position + 0x18 + item.p07;
     item.p08_offset = position + 0x1C + item.p08;
     item.p14_offset = position + 0x34 + item.p14;
     item.p15_offset = position + 0x38 + item.p15;
     if(ifile.fail()) return error("Read failure.");
     gilist2.push_back(item);
     cout << "0x" << hex << item.p01 << dec << " ";
     cout << "0x" << hex << item.p02 << dec << " ";
     cout << "0x" << hex << item.p03 << dec << " ";
     cout << "0x" << hex << item.p04 << dec << " ";
     cout << "0x" << hex << item.p05 << dec << " ";
     cout << "0x" << hex << item.p06 << dec << " ";
     cout << "0x" << hex << item.p07 << dec << " ";
     cout << "(0x" << hex << item.p07_offset << dec << ") ";
     cout << "0x" << hex << item.p08 << dec << " ";
     cout << "(0x" << hex << item.p08_offset << dec << ") ";
     cout << "0x" << hex << item.p09 << dec << " ";
     cout << "0x" << hex << item.p10 << dec << " ";
     cout << "0x" << hex << item.p11 << dec << " ";
     cout << "0x" << hex << item.p12 << dec << " ";
     cout << "0x" << hex << item.p13 << dec << " ";
     cout << "0x" << hex << item.p14 << dec << " ";
     cout << "0x" << hex << item.p14_offset << dec << " ";
     cout << "0x" << hex << item.p15 << dec << " ";
     cout << "0x" << hex << item.p15_offset << dec << endl;
    }

 // read joint map
 deque<uint32> jntmap;
 for(uint32 i = 0; i < gh.p05; i++) {
     uint32 item = BE_read_uint32(ifile);     
     jntmap.push_back(item);
     cout << "0x" << hex << jntmap[i] << dec << endl;
    }
 cout << endl;

 //
 // PRECREATE VERTICES
 //
 struct TOGVERTEX {
  real32 vx, vy, vz;
  real32 nx, ny, nz;
  real32 t1[2];
  real32 t2[2];
  real32 bw[4];
  uint32 bi[4];
 };

 typedef boost::shared_array<TOGVERTEX> BSA_TOGVERTEX;
 boost::shared_array<BSA_TOGVERTEX> vtxdata;
 vtxdata.reset(new BSA_TOGVERTEX[n_meshes]);
 for(uint32 i = 0; i < n_meshes; i++) vtxdata[i].reset(new TOGVERTEX[gilist2[i].p11]);

 //
 // VERTEX DATA
 //

 // for each mesh
 deque<pair<real32,real32>> uvdata;
 for(uint32 i = 0; i < n_meshes; i++)
    {
     if(gilist2[i].p10 == 0x071)
       {
        // move to vertex data
        ifile.seekg(gilist2[i].p08);
        if(ifile.fail()) return error("Seek failure.");

        // read vertex data
        for(uint32 j = 0; j < gilist2[i].p11; j++) {
            vtxdata[i][j].vx = BE_read_real32(ifile);
            vtxdata[i][j].vy = BE_read_real32(ifile);
            vtxdata[i][j].vz = BE_read_real32(ifile);
           }

        // move to secondary vertex data
        ifile.seekg(uv_offset + gilist2[i].p06);
        if(ifile.fail()) return error("Seek failure.");

        if(gilist2[i].p09 == 0x24)
          {
           for(uint32 j = 0; j < gilist2[i].p11; j++) {
               real32 p01 = BE_read_real32(ifile); // vx
               real32 p02 = BE_read_real32(ifile); // vy
               real32 p03 = BE_read_real32(ifile); // vz
               real32 p04 = BE_read_real32(ifile); // nx
               real32 p05 = BE_read_real32(ifile); // ny
               real32 p06 = BE_read_real32(ifile); // nz
               uint32 p07 = BE_read_uint32(ifile); // 0xFFFFFFFF
               real32 p08 = BE_read_real32(ifile); // channel 2
               real32 p09 = BE_read_real32(ifile); // channel 2
               vtxdata[i][j].nx = p01;
               vtxdata[i][j].ny = p02;
               vtxdata[i][j].nz = p03;
               vtxdata[i][j].t1[0] = p08;
               vtxdata[i][j].t1[1] = p09;
              }
          }
        else {
           stringstream ss;
           ss << "Mesh type 0x071 has unknown bytes per vertex 0x";
           ss << hex << gilist2[i].p09 << dec;
           ss << ".";
           return error(ss.str());
          }
       }
     else if(gilist2[i].p10 == 0x0F1)
       {
        // seek index buffer data
        ifile.seekg(gilist2[i].p07_offset);
        if(ifile.fail()) return error("Seek failure.");

        // read number of index buffers
        uint32 n_buffers = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");

        // read index buffer properties
        typedef pair<uint16,uint16> pair_t;
        boost::shared_array<pair_t> bprops(new pair_t[n_buffers]);
        for(uint32 j = 0; j < n_buffers; j++) {
            uint16 a = BE_read_uint16(ifile); // number of vertices
            uint16 b = BE_read_uint16(ifile); // number of indices
            if(ifile.fail()) return error("Read failure.");
            bprops[j] = pair_t(a, b);
           }

        // read index buffers
        for(uint32 j = 0; j < n_buffers; j++) {
            uint32 ib_elem = bprops[j].second;
            uint32 ib_size = ib_elem * 2;
            boost::shared_array<char> ib_data(new char[ib_size]);
            BE_read_array(ifile, reinterpret_cast<uint16*>(ib_data.get()), ib_elem);
           }

        // move to secondary vertex data
        ifile.seekg(uv_offset + gilist2[i].p06);
        if(ifile.fail()) return error("Seek failure.");

        if(gilist2[i].p09 == 0x0C)
          {
           for(uint32 j = 0; j < gilist2[i].p11; j++) {
               uint32 p01 = BE_read_uint32(ifile);
               real32 p02 = BE_read_real32(ifile);
               real32 p03 = BE_read_real32(ifile);
               vtxdata[i][j].t1[0] = p02;
               vtxdata[i][j].t1[1] = p03;
              }
          }
        else if(gilist2[i].p09 == 0x14)
          {
           for(uint32 j = 0; j < gilist2[i].p11; j++) {
               uint32 p01 = BE_read_uint32(ifile); // 0xFFFFFFFF
               real32 p02 = BE_read_real32(ifile); // channel 1
               real32 p03 = BE_read_real32(ifile); // channel 1
               real32 p04 = BE_read_real32(ifile); // channel 2
               real32 p05 = BE_read_real32(ifile); // channel 2
               vtxdata[i][j].t1[0] = p02;
               vtxdata[i][j].t1[1] = p03;
              }
          }
        else {
           stringstream ss;
           ss << "Mesh type 0x0F1 has unknown bytes per vertex 0x";
           ss << hex << gilist2[i].p09 << dec;
           ss << ".";
           return error(ss.str());
          }
       }
     else if(gilist2[i].p10 == 0x2F1)
       {
        // ????? TODO

        // move to secondary vertex data
        ifile.seekg(uv_offset + gilist2[i].p06);
        if(ifile.fail()) return error("Seek failure.");

        if(gilist2[i].p09 == 0x0C)
          {
           for(uint32 j = 0; j < gilist2[i].p11; j++) {
               uint32 p01 = BE_read_uint32(ifile);
               real32 p02 = BE_read_real32(ifile);
               real32 p03 = BE_read_real32(ifile);
               vtxdata[i][j].t1[0] = p02;
               vtxdata[i][j].t1[1] = p03;
              }
          }
        else if(gilist2[i].p09 == 0x14)
          {
           for(uint32 j = 0; j < gilist2[i].p11; j++) {
               uint32 p01 = BE_read_uint32(ifile); // 0xFFFFFFFF
               real32 p02 = BE_read_real32(ifile); // channel 1
               real32 p03 = BE_read_real32(ifile); // channel 1
               real32 p04 = BE_read_real32(ifile); // channel 2
               real32 p05 = BE_read_real32(ifile); // channel 2
               vtxdata[i][j].t1[0] = p02;
               vtxdata[i][j].t1[1] = p03;
              }
          }
        else {
           stringstream ss;
           ss << "Mesh type 0x2F1 has unknown bytes per vertex 0x";
           ss << hex << gilist2[i].p09 << dec;
           ss << ".";
           return error(ss.str());
          }
       }
     else {
        stringstream ss;
        ss << "Unknown mesh type 0x";
        ss << hex << gilist2[i].p10 << dec;
        ss << ".";
        return error(ss.str());
       }
    }

 //
 // MESH PROCESSING
 //

 // model container
 ADVANCEDMODELCONTAINER amc;
 uint32 curr_uv_index = 0;

 // for each mesh
 for(uint32 i = 0; i < n_meshes; i++)
    {
     // seek index buffer data
     uint32 position = gilist2[i].p07_offset;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read number of index buffers
     uint32 n_buffers = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read index buffer properties
     typedef pair<uint16,uint16> pair_t;
     boost::shared_array<pair_t> bprops(new pair_t[n_buffers]);
     for(uint32 j = 0; j < n_buffers; j++) {
         uint16 a = BE_read_uint16(ifile); // number of vertices
         uint16 b = BE_read_uint16(ifile); // number of indices
         if(ifile.fail()) return error("Read failure.");
         bprops[j] = pair_t(a, b);
        }

     // read index buffers
     for(uint32 j = 0; j < n_buffers; j++)
        {
         // read index buffer
         uint32 ib_elem = bprops[j].second;
         uint32 ib_size = ib_elem * 2;
         boost::shared_array<char> ib_data(new char[ib_size]);
         BE_read_array(ifile, reinterpret_cast<uint16*>(ib_data.get()), ib_elem);

         // create index buffer
         AMC_IDXBUFFER ib;
         ib.format = AMC_UINT16;
         ib.type = AMC_TRISTRIPCUT;
         ib.wind = AMC_CW;
         ib.reserved = 0;
         ib.name = "default";
         ib.elem = ib_elem;
         ib.data = ib_data;

         // insert index buffer
         amc.iblist.push_back(ib);
        }

     // seek vertex buffer data
     position = gilist2[i].p08_offset;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // construct vertex buffer flags
     uint32 vb_type = gilist2[i].p10;
     uint32 vb_flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
     if(vb_type == 0xF1) vb_flags |= AMC_VERTEX_WEIGHTS;

     // read vertex buffers
     for(uint32 j = 0; j < n_buffers; j++)
        {
         // create vertex buffer
         AMC_VTXBUFFER vb;
         vb.flags = vb_flags;
         vb.name = "default";
         vb.uvchan = 1;
         vb.uvtype[0] = AMC_DIFFUSE_MAP; // important!
         vb.uvtype[1] = AMC_INVALID_MAP; // important!
         vb.uvtype[2] = AMC_INVALID_MAP; // important!
         vb.uvtype[3] = AMC_INVALID_MAP; // important!
         vb.uvtype[4] = AMC_INVALID_MAP; // important!
         vb.uvtype[5] = AMC_INVALID_MAP; // important!
         vb.uvtype[6] = AMC_INVALID_MAP; // important!
         vb.uvtype[7] = AMC_INVALID_MAP; // important!
         vb.colors = 0;

         // WEIGHTED MESH
         if(vb_type == 0xF1)
           {
            uint32 n_vert_A;
            uint32 n_vert_B;
            uint32 n_vert_C;
            uint32 n_vert_D;

            // first vertex buffer (pull from mesh information)
            if(j == 0) {
               n_vert_A = gilist[i].p05;
               n_vert_B = gilist[i].p06;
               n_vert_C = gilist[i].p07;
               n_vert_D = gilist[i].p08;
              }
            // other vertex buffers (read inline)
            else {
               n_vert_A = BE_read_uint32(ifile);
               n_vert_B = BE_read_uint32(ifile);
               n_vert_C = BE_read_uint32(ifile);
               n_vert_D = BE_read_uint32(ifile);
              }

            // allocate vertex buffer
            vb.elem = n_vert_A + n_vert_B + n_vert_C + n_vert_D;
            vb.data.reset(new AMC_VERTEX[vb.elem]);

            // for each vertex type A
            for(uint32 k = 0; k < n_vert_A; k++)
               {
                vb.data[k].vx = BE_read_real32(ifile);
                vb.data[k].vy = BE_read_real32(ifile);
                vb.data[k].vz = BE_read_real32(ifile);
                vb.data[k].nx = BE_read_real32(ifile);
                vb.data[k].ny = BE_read_real32(ifile);
                vb.data[k].nz = BE_read_real32(ifile);
                vb.data[k].uv[0][0] = uvdata[curr_uv_index].first;
                vb.data[k].uv[0][1] = uvdata[curr_uv_index].second;
                curr_uv_index++;
                
                uint16 i1 = BE_read_uint08(ifile);
                uint16 i2 = BE_read_uint08(ifile);
                uint16 i3 = BE_read_uint08(ifile);
                uint16 i4 = BE_read_uint08(ifile);

                vb.data[k].wi[0] = i1;
                vb.data[k].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

                vb.data[k].wv[0] = 1.0f;
                vb.data[k].wv[1] = 0.0f;
                vb.data[k].wv[2] = 0.0f;
                vb.data[k].wv[3] = 0.0f;
                vb.data[k].wv[4] = 0.0f;
                vb.data[k].wv[5] = 0.0f;
                vb.data[k].wv[6] = 0.0f;
                vb.data[k].wv[7] = 0.0f;
               }

            // for each vertex type B
            uint32 offset = n_vert_A;
            for(uint32 k = 0; k < n_vert_B; k++)
               {
                vb.data[offset + k].vx = BE_read_real32(ifile);
                vb.data[offset + k].vy = BE_read_real32(ifile);
                vb.data[offset + k].vz = BE_read_real32(ifile);
                vb.data[offset + k].nx = BE_read_real32(ifile);
                vb.data[offset + k].ny = BE_read_real32(ifile);
                vb.data[offset + k].nz = BE_read_real32(ifile);
                vb.data[offset + k].uv[0][0] = uvdata[curr_uv_index].first;
                vb.data[offset + k].uv[0][1] = uvdata[curr_uv_index].second;
                curr_uv_index++;

                uint16 i1 = BE_read_uint08(ifile);
                uint16 i2 = BE_read_uint08(ifile);
                uint16 i3 = BE_read_uint08(ifile);
                uint16 i4 = BE_read_uint08(ifile);
                
                real32 w1 = BE_read_real32(ifile);
                real32 w2 = 1.0f - w1;

                vb.data[offset + k].wi[0] = i1;
                vb.data[offset + k].wi[1] = i2;
                vb.data[offset + k].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

                vb.data[offset + k].wv[0] = w1;
                vb.data[offset + k].wv[1] = w2;
                vb.data[offset + k].wv[2] = 0.0f;
                vb.data[offset + k].wv[3] = 0.0f;
                vb.data[offset + k].wv[4] = 0.0f;
                vb.data[offset + k].wv[5] = 0.0f;
                vb.data[offset + k].wv[6] = 0.0f;
                vb.data[offset + k].wv[7] = 0.0f;
               }

            // for each vertex type C
            offset += n_vert_B;
            for(uint32 k = 0; k < n_vert_C; k++)
               {
                vb.data[offset + k].vx = BE_read_real32(ifile);
                vb.data[offset + k].vy = BE_read_real32(ifile);
                vb.data[offset + k].vz = BE_read_real32(ifile);
                vb.data[offset + k].nx = BE_read_real32(ifile);
                vb.data[offset + k].ny = BE_read_real32(ifile);
                vb.data[offset + k].nz = BE_read_real32(ifile);
                vb.data[offset + k].uv[0][0] = uvdata[curr_uv_index].first;
                vb.data[offset + k].uv[0][1] = uvdata[curr_uv_index].second;
                curr_uv_index++;

                uint16 i1 = BE_read_uint08(ifile);
                uint16 i2 = BE_read_uint08(ifile);
                uint16 i3 = BE_read_uint08(ifile);
                uint16 i4 = BE_read_uint08(ifile);
                
                real32 w1 = BE_read_real32(ifile);
                real32 w2 = BE_read_real32(ifile);
                real32 w3 = 1.0f - w1 - w2;

                vb.data[offset + k].wi[0] = i1;
                vb.data[offset + k].wi[1] = i2;
                vb.data[offset + k].wi[2] = i3;
                vb.data[offset + k].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

                vb.data[offset + k].wv[0] = w1;
                vb.data[offset + k].wv[1] = w2;
                vb.data[offset + k].wv[2] = w3;
                vb.data[offset + k].wv[3] = 0.0f;
                vb.data[offset + k].wv[4] = 0.0f;
                vb.data[offset + k].wv[5] = 0.0f;
                vb.data[offset + k].wv[6] = 0.0f;
                vb.data[offset + k].wv[7] = 0.0f;
               }

            // for each vertex type D
            offset += n_vert_C;
            for(uint32 k = 0; k < n_vert_D; k++)
               {
                vb.data[offset + k].vx = BE_read_real32(ifile);
                vb.data[offset + k].vy = BE_read_real32(ifile);
                vb.data[offset + k].vz = BE_read_real32(ifile);
                vb.data[offset + k].nx = BE_read_real32(ifile);
                vb.data[offset + k].ny = BE_read_real32(ifile);
                vb.data[offset + k].nz = BE_read_real32(ifile);
                vb.data[offset + k].uv[0][0] = uvdata[curr_uv_index].first;
                vb.data[offset + k].uv[0][1] = uvdata[curr_uv_index].second;
                curr_uv_index++;

                uint16 i1 = BE_read_uint08(ifile);
                uint16 i2 = BE_read_uint08(ifile);
                uint16 i3 = BE_read_uint08(ifile);
                uint16 i4 = BE_read_uint08(ifile);
                
                real32 w1 = BE_read_real32(ifile);
                real32 w2 = BE_read_real32(ifile);
                real32 w3 = BE_read_real32(ifile);
                real32 w4 = 1.0f - w1 - w2 - w3;

                vb.data[offset + k].wi[0] = i1;
                vb.data[offset + k].wi[1] = i2;
                vb.data[offset + k].wi[2] = i3;
                vb.data[offset + k].wi[3] = i4;
                vb.data[offset + k].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[offset + k].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

                vb.data[offset + k].wv[0] = w1;
                vb.data[offset + k].wv[1] = w2;
                vb.data[offset + k].wv[2] = w3;
                vb.data[offset + k].wv[3] = w4;
                vb.data[offset + k].wv[4] = 0.0f;
                vb.data[offset + k].wv[5] = 0.0f;
                vb.data[offset + k].wv[6] = 0.0f;
                vb.data[offset + k].wv[7] = 0.0f;
               }
           }
         // NON-WEIGHTED MESH
         else if(vb_type == 0x2F1 || vb_type == 0x2F2)
           {
            // number of vertices
            uint32 n_vert_A;

            // first vertex buffer (pull from mesh information)
            if(j == 0) {
               n_vert_A = gilist[i].p05;
               if(gilist[i].p06 > 0) return error("Unexpected number of vertices.");
               if(gilist[i].p07 > 0) return error("Unexpected number of vertices.");
               if(gilist[i].p08 > 0) return error("Unexpected number of vertices.");
              }
            // other vertex buffers (read inline)
            else {
               n_vert_A = BE_read_uint32(ifile);
               if(ifile.fail()) return error("Read failure.");
              }

            // allocate vertex buffer
            vb.elem = n_vert_A;
            vb.data.reset(new AMC_VERTEX[vb.elem]);

            // for each vertex
            for(uint32 k = 0; k < n_vert_A; k++)
               {
                vb.data[k].vx = BE_read_real32(ifile);
                vb.data[k].vy = BE_read_real32(ifile);
                vb.data[k].vz = BE_read_real32(ifile);
               }
            // for each vertex
            for(uint32 k = 0; k < n_vert_A; k++)
               {
                vb.data[k].nx = BE_read_real32(ifile);
                vb.data[k].ny = BE_read_real32(ifile);
                vb.data[k].nz = BE_read_real32(ifile);
                vb.data[k].uv[0][0] = uvdata[curr_uv_index].first;
                vb.data[k].uv[0][1] = uvdata[curr_uv_index].second;
                curr_uv_index++;
               }
           }
         else
            return error("Invalid vertex buffer type.");

         // insert vertex buffer
         amc.vblist.push_back(vb);
        }
    }

 // DEBUG
 for(uint32 i = 0; i < amc.vblist.size(); i++)
    {
     uint16* ptr = reinterpret_cast<uint16*>(amc.iblist[i].data.get());
     uint16 min_v = 0xFFFF;
     uint16 max_v = 0x0000;
     for(uint32 j = 0; j < amc.iblist[i].elem; j++) {
         if(ptr[j] == 0xFFFF) continue; // skip strip cut index
         if(ptr[j] < min_v) min_v = ptr[j];
         else if(ptr[j] > max_v) max_v = ptr[j];
        }
     cout << "I = " << i << endl;
     cout << "NUMBER OF VERTICES: " << amc.vblist[i].elem << endl;
     cout << "NUMBER OF INDICES: " << amc.iblist[i].elem << endl;
     cout << "MINIMUM INDEX: " << min_v << endl;
     cout << "MAXIMUM INDEX: " << max_v << endl;
     cout << endl;
    }

 // validate buffer references
 if(amc.vblist.size() != amc.iblist.size())
    return error("The number of data buffers must equal.");

 // create buffer references
 for(uint32 i = 0; i < amc.vblist.size(); i++)
    {
     // create surface name
     stringstream ss;
     ss << "mesh_" << setfill('0') << setw(3) << i;

     // create surface
     AMC_SURFACE sur;
     sur.name = ss.str();
     AMC_REFERENCE ref;
     ref.vb_index = i;
     ref.vb_start = 0;
     ref.vb_width = amc.vblist[i].elem;
     ref.ib_index = i;
     ref.ib_start = 0;
     ref.ib_width = amc.iblist[i].elem;
     ref.jm_index = 0; // always one joint map
     sur.refs.push_back(ref);
     sur.surfmat = AMC_INVALID_SURFMAT;

     // insert surface
     amc.surfaces.push_back(sur);
    }

 //
 // SKELETON PROCESSING
 //

 // seek bone information
 ifile.seekg(0x0C);
 if(ifile.fail()) return error("Seek failure.");

 // read offset to joint data
 uint32 joint_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to joint data
 ifile.seekg(joint_offset);
 if(ifile.fail()) return error("Seek failure.");
 
 // read joint data header
 uint32 jnt_h01 = BE_read_uint32(ifile); // 0x0100
 uint32 jnt_h02 = BE_read_uint32(ifile); // size of section
 uint32 jnt_h03 = BE_read_uint32(ifile); // 0x10
 uint32 jnt_h04 = BE_read_uint32(ifile); // number of joint
 uint32 jnt_h05 = BE_read_uint32(ifile); // 0x10
 uint32 jnt_h06 = BE_read_uint32(ifile); // 0x00020005

 uint32 jnt_h07_offset = (uint32)ifile.tellg();
 uint32 jnt_h07 = BE_read_uint32(ifile); // offset from this item to matrices
 jnt_h07_offset += jnt_h07;

 // read joint identifiers
 boost::shared_array<uint32> idlist(new uint32[jnt_h04]);
 if(!BE_read_array(ifile, idlist.get(), jnt_h04)) return error("Read failure.");

 // read joint information
 struct TGJNTINFO {
  uint32 p01;
  uint32 p02; // parent identifier
  uint32 p03; // parent index
  uint32 p04; // index?
  uint32 p05; // index?
  uint32 p06; // 0x00000000
  uint32 p07; // offset from here to string table
  uint32 p07_offset;
  uint32 p08; // offset from here to string table
  uint32 p08_offset;
 };
 deque<TGJNTINFO> jntinfo;
 for(uint32 i = 0; i < jnt_h04; i++) {
     TGJNTINFO item;
     item.p01 = BE_read_uint32(ifile);
     item.p02 = BE_read_uint32(ifile);
     item.p03 = BE_read_uint32(ifile);
     item.p04 = BE_read_uint32(ifile);
     item.p05 = BE_read_uint32(ifile);
     item.p06 = BE_read_uint32(ifile);
     uint32 offset = (uint32)ifile.tellg();
     item.p07 = BE_read_uint32(ifile);
     item.p07_offset = item.p07 + offset;
     item.p08 = BE_read_uint32(ifile);
     item.p08_offset = item.p07 + offset + 4;
     jntinfo.push_back(item);
    }

 // read joint names
 deque<string> jntnames;
 for(uint32 i = 0; i < jnt_h04; i++) {
     ifile.seekg(jntinfo[i].p07_offset);
     char name[64];
     read_string(ifile, &name[0], 64);
     jntnames.push_back(name);
     cout << name << endl;
    }
 cout << endl;

 // seek joint matrix data
 ifile.seekg(jnt_h07_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read joint matrices
 deque<cs::math::matrix4x4<real32>> jntmats;
 for(uint32 i = 0; i < jnt_h04; i++) {
     cs::math::matrix4x4<real32> mat;
     if(!BE_read_array(ifile, &mat[0], 16)) return error("Read failure.");
     //mat.transpose();
     for(uint32 j = 0; j < 16; j++) cout << mat[j] << " ";
     cout << endl;
     jntmats.push_back(mat);
    }

 // read joint matrices (second set)
 deque<cs::math::matrix4x4<real32>> jntmats2;
 for(uint32 i = 0; i < jnt_h04; i++) {
     cs::math::matrix4x4<real32> mat;
     if(!BE_read_array(ifile, &mat[0], 16)) return error("Read failure.");
     //mat.transpose();
     for(uint32 j = 0; j < 16; j++) cout << mat[j] << " ";
     cout << endl;
     jntmats2.push_back(mat);
    }

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = "skeleton";

 // for each joint
 for(uint32 i = 0; i < jnt_h04; i++)
    {
     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;

     // create and insert joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = (jntinfo[i].p03 == 0xFFFFFFFF ? AMC_INVALID_JOINT : jntinfo[i].p03);
     joint.m_rel[0x0] = jntmats2[i][0x0];
     joint.m_rel[0x1] = jntmats2[i][0x1];
     joint.m_rel[0x2] = jntmats2[i][0x2];
     joint.m_rel[0x3] = 0.0f;
     joint.m_rel[0x4] = jntmats2[i][0x4];
     joint.m_rel[0x5] = jntmats2[i][0x5];
     joint.m_rel[0x6] = jntmats2[i][0x6];
     joint.m_rel[0x7] = 0.0f;
     joint.m_rel[0x8] = jntmats2[i][0x8];
     joint.m_rel[0x9] = jntmats2[i][0x9];
     joint.m_rel[0xA] = jntmats2[i][0xA];
     joint.m_rel[0xB] = 0.0f;
     joint.m_rel[0xC] = 0.0f;
     joint.m_rel[0xD] = 0.0f;
     joint.m_rel[0xE] = 0.0f;
     joint.m_rel[0xF] = 1.0f;
     joint.p_rel[0] = jntmats2[i][0x3];
     joint.p_rel[1] = jntmats2[i][0x7];
     joint.p_rel[2] = jntmats2[i][0xB];
     joint.p_rel[3] = 1.0f;
     skel.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel);

 //
 // JOINT MAP PROCESSING
 //

 // for each item in the joint map
 for(uint32 i = 0; i < jntmap.size(); i++)
    {
     // look up identifer
     uint32 index = 0xFFFFFFFF;
     for(uint32 j = 0; j < jnt_h04; j++) {
         if(jntmap[i] == idlist[j]) {
            index = j;
            break;
           }
        }

     // index not found
     if(index == 0xFFFFFFFF) {
        stringstream ss;
        ss << "Could not find joint identifier 0x";
        ss << hex << jntmap[i] << dec;
        ss << ".";
        error(ss.str());
        index = 0; // just remap to zero this happens alot!
       }

     // reset joint map index
     jntmap[i] = index;
    }

 // set joint map
 AMC_JOINTMAP jm;
 jm.jntmap = jntmap;
 amc.jmaplist.push_back(jm);

 // save model
 SaveAMC(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

bool TestInjustice(void)
{
 // open file
 using namespace std;
 string filename = "c:\\users\\semory\\desktop\\model3.bin";
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 ADVANCEDMODELCONTAINER amc;

 //
 // FIXED PROPERTIES
 //

 uint32 n_joints = 0x7D;

 //
 // VERTEX BUFFER PROCESSING
 //

 uint32 vb_offset = 0x23E82;
 uint32 vb_size = 0x75924;
 uint32 vb_elem = 0x75924 / 0x24;

 // move to vertex buffer
 ifile.seekg(vb_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vertex buffer
 boost::shared_array<char> vb_data(new char[vb_size]);
 ifile.read(vb_data.get(), vb_size);
 if(ifile.fail()) return error("Read failure.");

 // initialize vertex buffer
 AMC_VTXBUFFER vb;
 vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV; // | AMC_VERTEX_NORMAL;
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

 // process vertices
 binary_stream vbs(vb_data, vb_size);
 for(uint32 i = 0; i < vb_elem; i++)
    {
     vb.data[i].vx = vbs.BE_read_real32();
     vb.data[i].vy = vbs.BE_read_real32();
     vb.data[i].vz = vbs.BE_read_real32();

     ((real32)vbs.BE_read_uint08() - 127.0f)/128.0f;
     ((real32)vbs.BE_read_uint08() - 127.0f)/128.0f;
     ((real32)vbs.BE_read_uint08() - 127.0f)/128.0f;
     ((real32)vbs.BE_read_uint08() - 127.0f)/128.0f;
     vb.data[i].nw = ((real32)vbs.BE_read_uint08() - 127.0f)/128.0f;
     vb.data[i].nx = ((real32)vbs.BE_read_uint08() - 127.0f)/128.0f;
     vb.data[i].ny = ((real32)vbs.BE_read_uint08() - 127.0f)/128.0f;
     vb.data[i].nz = ((real32)vbs.BE_read_uint08() - 127.0f)/128.0f;
     //cout << vb.data[i].nx << " " << vb.data[i].ny << " " << vb.data[i].nz << " " << vb.data[i].nw << endl;
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

 uint32 ib_offset = 0x2815;
 uint32 ib_size = 0x19752;
 uint32 ib_elem = 0x19752 / 0x02;

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
 // JOINT MAP DATA
 //

 uint32 jmap_offset = 0x1C06D;
 uint32 jmap_elem = 0x6A;

 // move to joint map
 ifile.seekg(jmap_offset);
 if(ifile.fail()) return error("Seek failure.");

 // create joint map
 for(uint32 i = 0; i < jmap_elem; i++)
    {
     uint16 items = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Seek failure.");

     AMC_JOINTMAP jm;
     for(uint32 j = 0; j < items; j++) {
         uint16 item = BE_read_uint16(ifile);
         jm.jntmap.push_back(item);
        }
     amc.jmaplist.push_back(jm);

     // skip bytes
     ifile.seekg(0x20, ios::cur);
     if(ifile.fail()) return error("Seek failure.");
    }


 //
 // SURFACE PROCESSING
 //

 uint32 surf_offset = 0x23ED;
 uint32 surf_elem = 0x6A;
 uint32 surf_size = 0x6A * 0xA;

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
     ref.jm_index = AMC_INVALID_JMAP_INDEX; // i;
     sur.refs.push_back(ref);
     sur.surfmat = AMC_INVALID_SURFMAT;

     // insert surface
     amc.surfaces.push_back(sur);
    }

 //
 // SKELETON PROCESSING
 //

 // move to parent data
 ifile.seekg(0x16BC);
 if(ifile.fail()) return error("Seek failure.");

 // read number of joints
 uint32 n_joints1 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read parent data
 boost::shared_array<uint16> parents(new uint16[n_joints1]);
 if(!BE_read_array(ifile, parents.get(), n_joints1)) return error("Read failure.");

 // move to joint data
 ifile.seekg(0x90C);
 if(ifile.fail()) return error("Seek failure.");

 // read number of joints
 uint32 n_joints2 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 AMC_SKELETON2 skel2;
 skel2.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel2.name = "skeleton";

 // read joint data
 for(uint32 i = 0; i < n_joints2; i++)
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
 // amc.skllist2.push_back(skel2);

 // transform and save
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), amc);//transformed);
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
 // for(uint32 r = 0; r < dy; r++) {
 //     for(uint32 c = 0; c < dx; c++) {
 //         uint32 morton = array_to_morton(r, c);
 //         uint32 copy_position = 4*r*dx + 4*c;
 //         uint32 data_position = 4*morton;
 //         copy[copy_position++] = data[data_position + 0]; // b
 //         copy[copy_position++] = data[data_position + 1]; // g
 //         copy[copy_position++] = data[data_position + 2]; // r
 //         copy[copy_position++] = data[data_position + 3]; // a
 //        }
 //    }

 XGUntileSurface(copy.get(), 4*dx, NULL, data.get(), dx, dy, NULL, 4);

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
  TestInjustice();
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\eyes00.bin", 0x100, 0x100, 0x7, 0x122);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\eyes01.bin", 0x100, 0x100, 0x7, 0x122);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\eyes02.bin", 0x100, 0x100, 0x5, 0x157);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture00.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture01.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture02.bin", 0x, 0x, 0x, 0x);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture03.bin", 0x200, 0x400, 0x7, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture04.bin", 0x200, 0x400, 0x7, 0x157);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture05.bin", 0x200, 0x400, 0x7, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture06.bin", 0x400, 0x400, 0x7, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture07.bin", 0x400, 0x800, 0x7, 0x157);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture08.bin", 0x400, 0x400, 0x7, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture09.bin", 0x020, 0x020, 0x5, 0x13E);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture10.bin", 0x020, 0x020, 0x5, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture11.bin", 0x100, 0x100, 0x7, 0x189);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture12.bin", 0x100, 0x100, 0x5, 0x138);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture13.bin", 0x080, 0x100, 0x5, 0x13B);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture14.bin", 0x200, 0x200, 0x7, 0x189);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture15.bin", 0x080, 0x080, 0x5, 0x138);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture16.bin", 0x100, 0x100, 0x5, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture17.bin", 0x200, 0x200, 0x5, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture18.bin", 0x200, 0x200, 0x5, 0x157);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture19.bin", 0x200, 0x200, 0x5, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture20.bin", 0x100, 0x100, 0x5, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture21.bin", 0x100, 0x100, 0x5, 0x157);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture22.bin", 0x080, 0x080, 0x5, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture23.bin", 0x200, 0x200, 0x5, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture24.bin", 0x200, 0x200, 0x5, 0x157);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture25.bin", 0x200, 0x200, 0x5, 0x106);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture26.bin", 0x400, 0x080, 0x7, 0x11C);
 //TestInjusticeTexture("C:\\Users\\semory\\Desktop\\test\\texture27.bin", 0x400, 0x400, 0x7, 0x11C);
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\eyes00.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\eyes01.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\eyes02.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture00.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture01.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture02.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture03.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture04.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture05.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture06.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture07.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture08.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture09.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture10.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture11.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture12.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture13.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture14.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture15.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture16.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture17.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture18.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture19.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture20.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture21.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture22.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture23.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture24.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture25.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture26.dds");
 //DeswizzleDXT1("C:\\Users\\semory\\Desktop\\test\\texture27.dds");

//ifstream ifile("G:\\xentax\\Injustice Gods Among Us\\Asset\\Startup.pak", ios::binary);
//typedef pair<uint32, uint32> pair_t;
//deque<pair_t> filelist;
//filelist.push_back(pair_t(0x000201f2, 0x019d3cca));
//filelist.push_back(pair_t(0x000201f2, 0x019f3ebc));
//filelist.push_back(pair_t(0x00010227, 0x01a140ae));
// for(uint32 i = 0; i < filelist.size(); i++) {
//     ifile.seekg(filelist[i].second);
//     boost::shared_array<char> data(new char[filelist[i].first]);
//     ifile.read(data.get(), filelist[i].first);
//     stringstream ss;
//     ss << "C:\\Users\\semory\\Desktop\\test\\";
//     ss << "eyes" << setfill('0') << setw(2) << (i) << ".bin";
//     ofstream ofile(ss.str().c_str(), ios::binary);
//     ofile.write(data.get(), filelist[i].first);
//    }

 //TestDesperado();
 //TestXNALara1();
 //TestDW7Model();
 //TestTalesGraces();
 //ExtractPSARC("bigdaddy_char_setup.psarc");

 //
 // PC
 //

 //PC::TR2013::extract();
 //PC::TR2013::extract("C:\\users\\semory\\desktop\\test\\");
 //PC::TR2013::extract("G:\\xentax\\TR2013\\");

 // TestMOD();

 //
 // PS3
 //

 // PS3::BIOSHOCK::extract();
 // PS3::BIOSHOCK::extract("G:\\xentax\\PS3_BIOSHOCK\\");

 //PS3::MACROSS30::extract();
 //PS3::MACROSS30::extract("C:\\Users\\semory\\sticklove\\source\\xentax\\Games\\Macross\\");

 //PS3::ROG::extract("C:\\Users\\semory\\sticklove\\source\\xentax\\Release\\[PS3] Rise of the Guardians (2012)\\");
 //PS3::RESIDENT_EVIL_6::extract("G:\\xentax\\RE6_extract\\data\\event\\face\\em4510\\");

 // Metal Gear Rising Retribution
 //PS3::MGRR::extract();
 //PS3::MGRR::extract("G:\\xentax\\MGRR_XBOX360\\");
 //PS3::MGRR::extract("G:\\xentax\\MGRR_PS3\\");

 // One Piece Kaizoku Musou 2
 // PS3::ONE_PIECE_X2::extract();
 // PS3::ONE_PIECE_X2::extract("G:\\xentax\\[PS3] One Piece Kaizoku Musou 2 (2013)\\");

 //
 // XBOX360
 //

 //XB360::ROG::extract();

 // Tekken Tag Tournament 2
 //XB360::TTT2::extract("C:\\Users\\semory\\sticklove\\source\\xentax\\Release\\TTT2\\");

 // Injustice
 // XBOX360::INJUSTICE::extract();
 //XBOX360::INJUSTICE::extract("C:\\Users\\semory\\Desktop\\lol\\");
 //XBOX360::INJUSTICE::extract("C:\\Users\\semory\\Desktop\\Injustice Gods Among Us\\_TEST_\\");

 // using namespace std;
 // ifstream ifile("C:\\Users\\semory\\Desktop\\lol\\CHAR_WonderWoman.xxx", ios::binary);
 // ifile.seekg(0x4D1);
 // 
 // uint32 n_strings1 = BE_read_uint32(ifile); // 0x37
 // for(uint32 i = 0; i < n_strings1; i++)
 //    {
 //     uint32 len = BE_read_uint32(ifile);
 //     char txt[1024];
 //     read_string(ifile, &txt[0], len);
 //     //cout << i << ":" << txt << endl;
 //     uint32 p01 = BE_read_uint32(ifile);
 //     uint32 p02 = BE_read_uint32(ifile);
 //     uint32 p03 = BE_read_uint32(ifile);
 //     uint32 p04 = BE_read_uint32(ifile);
 //     cout << setfill('0') << setw(8) << hex << p01 << dec << " ";
 //     cout << setfill('0') << setw(8) << hex << p02 << dec << " ";
 //     cout << setfill('0') << setw(8) << hex << p03 << dec << " ";
 //     cout << setfill('0') << setw(8) << hex << p04 << dec << endl;
 //    }
 // cout << endl;
 // 
 // uint32 n_strings2 = BE_read_uint32(ifile); // 0x06
 // for(uint32 i = 0; i < n_strings2; i++)
 //    {
 //     uint32 len = BE_read_uint32(ifile);
 //     char txt[1024];
 //     read_string(ifile, &txt[0], len);
 //     //cout << i << ":" << txt << endl;
 //     uint32 p01 = BE_read_uint32(ifile);
 //     uint32 p02 = BE_read_uint32(ifile);
 //     uint32 p03 = BE_read_uint32(ifile);
 //     uint32 p04 = BE_read_uint32(ifile);
 //     cout << setfill('0') << setw(8) << hex << p01 << dec << " ";
 //     cout << setfill('0') << setw(8) << hex << p02 << dec << " ";
 //     cout << setfill('0') << setw(8) << hex << p03 << dec << " ";
 //     cout << setfill('0') << setw(8) << hex << p04 << dec << endl;
 //    }
 // cout << endl;

 return 0;
}

