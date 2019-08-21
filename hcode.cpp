#include "xentax.h"
#include "x_file.h"
#include "x_amc.h"
#include "x_dds.h"
#include "x_stream.h"
#include "hcode.h"

#include "x_alg.h"

bool TestAdaWong(void)
{
 using namespace std;
 ifstream ifile("adawong skel.ssg", ios::binary);
 if(!ifile) return error("Failed to open file.");

 ofstream ofile("adawong skel.obj");
 if(!ofile) return error("Failed to create file.");
 ofile << "o adawong skel" << endl;
 ofile << endl;

 uint32 n_joints = 0xB4;
 boost::shared_array<uint16> parent(new uint16[n_joints]);
 boost::shared_array<boost::shared_array<real32>> rlist(new boost::shared_array<real32>[n_joints]);
 boost::shared_array<boost::shared_array<real32>> alist(new boost::shared_array<real32>[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     rlist[i].reset(new real32[12]);
     alist[i].reset(new real32[12]);
    }

 ifile.seekg(0x3F0);
 for(uint32 i = 0; i < n_joints; i++)
    {
     LE_read_array(ifile, &rlist[i][0], 12);
     cout << "[" << rlist[i][0x0] << ", " << rlist[i][0x1] << ", " << rlist[i][0x2] << ", " << rlist[i][0x3] << "]" << endl;
     cout << "[" << rlist[i][0x4] << ", " << rlist[i][0x5] << ", " << rlist[i][0x6] << ", " << rlist[i][0x7] << "]" << endl;
     cout << "[" << rlist[i][0x8] << ", " << rlist[i][0x9] << ", " << rlist[i][0xA] << ", " << rlist[i][0xB] << "]" << endl;
     cout << endl;
    }
 cout << endl;

 // read parent information
 ifile.seekg(0x25B0);
 LE_read_array(ifile, parent.get(), n_joints);
 for(uint32 i = 0; i < n_joints; i++) cout << "parent[" << i << "] = " << parent[i] << endl;
 cout << endl;

 uint32 a = 0x3;
 uint32 b = 0x7;
 uint32 c = 0xB;
 for(uint32 i = 0; i < n_joints; i++)
    {
     // compute absolute position
     uint32 j =  parent[i];
     if(j == 0xFFFF) {
        alist[i][a] = rlist[i][a];
        alist[i][b] = rlist[i][b];
        alist[i][c] = rlist[i][c];
       }
     else {
        alist[i][a] = alist[j][a] + rlist[i][a];
        alist[i][b] = alist[j][b] + rlist[i][b];
        alist[i][c] = alist[j][c] + rlist[i][c];
       }
    }

 // read string offsets
 ifile.seekg(0x2720);
 for(uint32 i = 0; i < n_joints; i++)
    {
    }
 cout << endl;

 // read joint names
 ifile.seekg(0x29F0);
 for(uint32 i = 0; i < n_joints; i++) {
     char name[1024];
     read_string(ifile, name, 1024);
     cout << "joint[" << i << "] = " << name << endl;
    }
 cout << endl;

 // read 
 ifile.seekg(0x3580);
 for(uint32 i = 0; i < n_joints; i++)
    {
     real32 a = LE_read_sint08(ifile);
     real32 b = LE_read_sint08(ifile);
     real32 c = LE_read_sint08(ifile);
     real32 d = LE_read_sint08(ifile);
     cout << a << ", " << b << ", " << c << ", " << d << endl;
     //
     //rlist[i][0] = a;
     //rlist[i][1] = b;
     //rlist[i][2] = c;
     //
     //// compute absolute position
     //uint32 j =  parent[i];
     //if(j == 0xFFFF) {
     //   alist[i][0] = rlist[i][0];
     //   alist[i][1] = rlist[i][1];
     //   alist[i][2] = rlist[i][2];
     //  }
     //else {
     //   alist[i][0] = alist[j][0] + rlist[i][0];
     //   alist[i][1] = alist[j][1] + rlist[i][1];
     //   alist[i][2] = alist[j][2] + rlist[i][2];
     //  }
     //
     ofile << "v " << a << " " << b << " " << c << endl;
     if(i > 0) ofile << "f " << i << " " << i + 1 << endl;
     //ofile << "v " << alist[i][a] << " " << alist[i][b] << " " << alist[i][c] << endl;
    }
 cout << endl;

 return true;
}

bool TestBraveTexture(const char* src, const char* dst, uint64 offset)
{
 // get program directory
 using namespace std;
 string pathname = GetModulePathname();
 string filename = pathname;
 filename += src;

 // open texture file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("error");

 // seek offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("error");

 // read unknowns
 uint32 unk01 = LE_read_uint32(ifile); // 0x07
 uint32 unk02 = LE_read_uint32(ifile); // 0xA0
 uint32 unk03 = LE_read_uint32(ifile); // 0x00

 // read dimensions and mipmaps
 uint32 dx = LE_read_uint32(ifile);
 uint32 dy = LE_read_uint32(ifile);
 uint32 mipmaps = LE_read_uint32(ifile);
 
 // read unknowns
 uint32 unk04 = LE_read_uint32(ifile); // 0x000
 uint32 unk05 = LE_read_uint32(ifile); // 0x400

 // read format
 // 0x31545844 = DXT1
 // 0x35545844 = DXT5
 uint32 format = LE_read_uint32(ifile);

 // read image data size
 ifile.seekg(offset + 0x38);
 uint32 datasize = LE_read_uint32(ifile);
 datasize -= 0xA0; // subtract header crap

 // read image data
 ifile.seekg(offset + 0x40);
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 
 // create image header
 DDS_HEADER ddsh;
 if(format == 0x31545844) CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh);
 else if(format == 0x35545844) CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh);
 
 // destination file
 string destfile = pathname;
 destfile += dst;

 // save image data
 ofstream ofile(destfile.c_str(), ios::binary);
 if(ofile.fail()) return error("error");
 ofile.write("DDS ", 4);
 ofile.write((char*)&ddsh, sizeof(ddsh));
 ofile.write(data.get(), datasize);

 return true;
}

bool TestBraveElinor(void)
{
 // get program directory
 using namespace std;
 string pathname = GetModulePathname();
 string fname1 = pathname;
 fname1 += "outfit01.pc_dx9";
 string fname2 = pathname;
 fname2 += "assets.pc_dx9";

 // open costume file
 ifstream ifile(fname1.c_str(), ios::binary);
 if(!ifile) return error("Failed to open outfit01.pc_dx9.");

 // open assests file
 ifstream afile(fname2.c_str(), ios::binary);
 if(!afile) return error("Failed to open assets.pc_dx9.");

 // read hair and sword textures
 TestBraveTexture("assets.pc_dx9", "meridahair_d.dds", (uint64)0x412100);
 TestBraveTexture("assets.pc_dx9", "meridahair_n.dds", (uint64)0x467700);
 TestBraveTexture("assets.pc_dx9", "meridahair_s.dds", (uint64)0x472260);
 TestBraveTexture("assets.pc_dx9", "meridasword_d.dds", (uint64)0x47CDC0);
 TestBraveTexture("assets.pc_dx9", "meridasword_n.dds", (uint64)0x4A7920);
 TestBraveTexture("assets.pc_dx9", "meridasword_s.dds", (uint64)0x4D2480);

 // read body textures
 TestBraveTexture("outfit01.pc_dx9", "merida_d.dds", (uint64)0x12A0);
 TestBraveTexture("outfit01.pc_dx9", "merida_n.dds", (uint64)0xABE00);
 TestBraveTexture("outfit01.pc_dx9", "merida_s.dds", (uint64)0x156960);
 TestBraveTexture("outfit01.pc_dx9", "powerupstatic.dds", (uint64)0x241700);
 TestBraveTexture("outfit01.pc_dx9", "rarecollectspark.dds", (uint64)0x2457C0);

 // read index buffer #1
 AMC_IDXBUFFER ib1;
 ib1.format = AMC_UINT16;
 ib1.type = AMC_TRISTRIP;
 ib1.name = "surface1";
 ib1.elem = 0x391;
 ib1.data.reset(new char[ib1.elem * 2]);
 ifile.seekg(0x25C114);
 LE_read_array(ifile, (uint16*)ib1.data.get(), ib1.elem);

 // read index buffer #2
 AMC_IDXBUFFER ib2;
 ib2.format = AMC_UINT16;
 ib2.type = AMC_TRISTRIP;
 ib2.name = "surface2";
 ib2.elem = 0x195F;
 ib2.data.reset(new char[ib2.elem * 2]);
 ifile.seekg(0x25C93C);
 LE_read_array(ifile, (uint16*)ib2.data.get(), ib2.elem);

 // read index buffer #3
 AMC_IDXBUFFER ib3;
 ib3.format = AMC_UINT16;
 ib3.type = AMC_TRISTRIP;
 ib3.name = "surface3";
 ib3.elem = 0x309;
 ib3.data.reset(new char[ib3.elem * 2]);
 ifile.seekg(0x29C458);
 LE_read_array(ifile, (uint16*)ib3.data.get(), ib3.elem);

 // read index buffer #4 (assets)
 AMC_IDXBUFFER ib4;
 ib4.format = AMC_UINT16;
 ib4.type = AMC_TRISTRIP;
 ib4.name = "surface4";
 ib4.elem = 0x51;
 ib4.data.reset(new char[ib4.elem * 2]);
 afile.seekg(0x9B9734);
 LE_read_array(afile, (uint16*)ib4.data.get(), ib4.elem);

 // read index buffer #5 (assets)
 AMC_IDXBUFFER ib5;
 ib5.format = AMC_UINT16;
 ib5.type = AMC_TRISTRIP;
 ib5.name = "surface5";
 ib5.elem = 0x51;
 ib5.data.reset(new char[ib5.elem * 2]);
 afile.seekg(0x9BAF04);
 LE_read_array(afile, (uint16*)ib5.data.get(), ib5.elem);

 // read index buffer #6 (assets)
 AMC_IDXBUFFER ib6;
 ib6.format = AMC_UINT16;
 ib6.type = AMC_TRISTRIP;
 ib6.name = "surface6";
 ib6.elem = 0x13;
 ib6.data.reset(new char[ib6.elem * 2]);
 afile.seekg(0x9BC6E4);
 LE_read_array(afile, (uint16*)ib6.data.get(), ib6.elem);

 // read index buffer #7 (assets)
 AMC_IDXBUFFER ib7;
 ib7.format = AMC_UINT16;
 ib7.type = AMC_TRISTRIP;
 ib7.name = "surface7";
 ib7.elem = 0x13;
 ib7.data.reset(new char[ib7.elem * 2]);
 afile.seekg(0x9BCD84);
 LE_read_array(afile, (uint16*)ib7.data.get(), ib7.elem);

 // read index buffer #8 (assets)
 AMC_IDXBUFFER ib8;
 ib8.format = AMC_UINT16;
 ib8.type = AMC_TRISTRIP;
 ib8.name = "surface8";
 ib8.elem = 0xCF2;
 ib8.data.reset(new char[ib8.elem * 2]);
 afile.seekg(0x9BD488);
 LE_read_array(afile, (uint16*)ib8.data.get(), ib8.elem);

 // read index buffer #9 (assets)
 AMC_IDXBUFFER ib9;
 ib9.format = AMC_UINT16;
 ib9.type = AMC_TRISTRIP;
 ib9.name = "surface9";
 ib9.elem = 0x33;
 ib9.data.reset(new char[ib9.elem * 2]);
 afile.seekg(0x9D665C);
 LE_read_array(afile, (uint16*)ib9.data.get(), ib9.elem);

 // read index buffer #10 (assets)
 AMC_IDXBUFFER ib10;
 ib10.format = AMC_UINT16;
 ib10.type = AMC_TRISTRIP;
 ib10.name = "surface10";
 ib10.elem = 0x3F;
 ib10.data.reset(new char[ib10.elem * 2]);
 afile.seekg(0x9D757C);
 LE_read_array(afile, (uint16*)ib10.data.get(), ib10.elem);

 // read vertex buffer #1
 AMC_VTXBUFFER vb1;
 vb1.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb1.name = "vb1";
 vb1.elem = 0x1F0;
 vb1.data.reset(new AMC_VERTEX[vb1.elem]);

 ifile.seekg(0x25FBFC);
 for(uint32 i = 0; i < vb1.elem; i++)
    {
     vb1.data[i].vx = LE_read_real32(ifile);
     vb1.data[i].vy = LE_read_real32(ifile);
     vb1.data[i].vz = LE_read_real32(ifile);
     vb1.data[i].wv[0] = LE_read_real32(ifile); // weights
     vb1.data[i].wv[1] = LE_read_real32(ifile); // weights
     vb1.data[i].wv[2] = LE_read_real32(ifile); // weights
     vb1.data[i].wv[3] = 1.0f - vb1.data[i].wv[0] - vb1.data[i].wv[1] - vb1.data[i].wv[2];
     vb1.data[i].wv[4] = 0.0f;
     vb1.data[i].wv[5] = 0.0f;
     vb1.data[i].wv[6] = 0.0f;
     vb1.data[i].wv[7] = 0.0f;
     vb1.data[i].wi[0] = LE_read_uint08(ifile); // blend indices
     vb1.data[i].wi[1] = LE_read_uint08(ifile); // blend indices
     vb1.data[i].wi[2] = LE_read_uint08(ifile); // blend indices
     vb1.data[i].wi[3] = LE_read_uint08(ifile); // blend indices
     vb1.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb1.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb1.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb1.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb1.data[i].nx = LE_read_real32(ifile); // normal
     vb1.data[i].ny = LE_read_real32(ifile); // normal
     vb1.data[i].nz = LE_read_real32(ifile); // normal
     LE_read_uint08(ifile); // 0xFF
     LE_read_uint08(ifile); // 0xFF
     LE_read_uint08(ifile); // 0xFF
     LE_read_uint08(ifile); // 0xFF
     vb1.data[i].tu = LE_read_real32(ifile); // UV
     vb1.data[i].tv = LE_read_real32(ifile); // UV
     LE_read_real32(ifile); // ???
     LE_read_real32(ifile); // ???
     LE_read_real32(ifile); // ???
    }

 // read vertex buffer #2
 AMC_VTXBUFFER vb2;
 vb2.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb2.name = "vb2";
 vb2.elem = 0x948;
 vb2.data.reset(new AMC_VERTEX[vb2.elem]);

 ifile.seekg(0x267800);
 for(uint32 i = 0; i < vb2.elem; i++)
    {
     vb2.data[i].vx = LE_read_real32(ifile);
     vb2.data[i].vy = LE_read_real32(ifile);
     vb2.data[i].vz = LE_read_real32(ifile);
     vb2.data[i].wv[0] = LE_read_real32(ifile); // weights
     vb2.data[i].wv[1] = LE_read_real32(ifile); // weights
     vb2.data[i].wv[2] = LE_read_real32(ifile); // weights
     vb2.data[i].wv[3] = 1.0f - vb2.data[i].wv[0] - vb2.data[i].wv[1] - vb2.data[i].wv[2];
     vb2.data[i].wv[4] = 0.0f;
     vb2.data[i].wv[5] = 0.0f;
     vb2.data[i].wv[6] = 0.0f;
     vb2.data[i].wv[7] = 0.0f;
     vb2.data[i].wi[0] = LE_read_uint08(ifile); // blend indices
     vb2.data[i].wi[1] = LE_read_uint08(ifile); // blend indices
     vb2.data[i].wi[2] = LE_read_uint08(ifile); // blend indices
     vb2.data[i].wi[3] = LE_read_uint08(ifile); // blend indices
     vb2.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb2.data[i].nx = LE_read_real32(ifile); // normal
     vb2.data[i].ny = LE_read_real32(ifile); // normal
     vb2.data[i].nz = LE_read_real32(ifile); // normal
     LE_read_uint08(ifile); // 0xFF
     LE_read_uint08(ifile); // 0xFF
     LE_read_uint08(ifile); // 0xFF
     LE_read_uint08(ifile); // 0xFF
     vb2.data[i].tu = LE_read_real32(ifile); // UV
     vb2.data[i].tv = LE_read_real32(ifile); // UV
     LE_read_real32(ifile); // ???
     LE_read_real32(ifile); // ???
     LE_read_real32(ifile); // ???
    }

 // read vertex buffer #3
 AMC_VTXBUFFER vb3;
 vb3.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb3.name = "vb3";
 vb3.elem = 0x10C;
 vb3.data.reset(new AMC_VERTEX[vb3.elem]);

 ifile.seekg(0x29CA6C);
 for(uint32 i = 0; i < vb3.elem; i++)
    {
     vb3.data[i].vx = LE_read_real32(ifile);
     vb3.data[i].vy = LE_read_real32(ifile);
     vb3.data[i].vz = LE_read_real32(ifile);
     vb3.data[i].wv[0] = LE_read_real32(ifile); // weights
     vb3.data[i].wv[1] = LE_read_real32(ifile); // weights
     vb3.data[i].wv[2] = LE_read_real32(ifile); // weights
     vb3.data[i].wv[3] = 1.0f - vb3.data[i].wv[0] - vb3.data[i].wv[1] - vb3.data[i].wv[2];
     vb3.data[i].wv[4] = 0.0f;
     vb3.data[i].wv[5] = 0.0f;
     vb3.data[i].wv[6] = 0.0f;
     vb3.data[i].wv[7] = 0.0f;
     vb3.data[i].wi[0] = LE_read_uint08(ifile); // blend indices
     vb3.data[i].wi[1] = LE_read_uint08(ifile); // blend indices
     vb3.data[i].wi[2] = LE_read_uint08(ifile); // blend indices
     vb3.data[i].wi[3] = LE_read_uint08(ifile); // blend indices
     vb3.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb3.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb3.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb3.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb3.data[i].nx = LE_read_real32(ifile); // normal
     vb3.data[i].ny = LE_read_real32(ifile); // normal
     vb3.data[i].nz = LE_read_real32(ifile); // normal
     LE_read_uint08(ifile); // 0xFF
     LE_read_uint08(ifile); // 0xFF
     LE_read_uint08(ifile); // 0xFF
     LE_read_uint08(ifile); // 0xFF
     vb3.data[i].tu = LE_read_real32(ifile); // UV
     vb3.data[i].tv = LE_read_real32(ifile); // UV
     LE_read_real32(ifile); // ???
     LE_read_real32(ifile); // ???
     LE_read_real32(ifile); // ???
    }

 // read vertex buffer #4
 AMC_VTXBUFFER vb4;
 vb4.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb4.name = "vb4";
 vb4.elem = 0x51;
 vb4.data.reset(new AMC_VERTEX[vb4.elem]);

 afile.seekg(0x9B97D8);
 for(uint32 i = 0; i < vb4.elem; i++)
    {
     vb4.data[i].vx = LE_read_real32(afile);
     vb4.data[i].vy = LE_read_real32(afile);
     vb4.data[i].vz = LE_read_real32(afile);
     vb4.data[i].wv[0] = LE_read_real32(afile); // weights
     vb4.data[i].wv[1] = LE_read_real32(afile); // weights
     vb4.data[i].wv[2] = LE_read_real32(afile); // weights
     vb4.data[i].wv[3] = 1.0f - vb4.data[i].wv[0] - vb4.data[i].wv[1] - vb4.data[i].wv[2];
     vb4.data[i].wv[4] = 0.0f;
     vb4.data[i].wv[5] = 0.0f;
     vb4.data[i].wv[6] = 0.0f;
     vb4.data[i].wv[7] = 0.0f;
     vb4.data[i].wi[0] = LE_read_uint08(afile); // blend indices
     vb4.data[i].wi[1] = LE_read_uint08(afile); // blend indices
     vb4.data[i].wi[2] = LE_read_uint08(afile); // blend indices
     vb4.data[i].wi[3] = LE_read_uint08(afile); // blend indices
     vb4.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb4.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb4.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb4.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb4.data[i].nx = LE_read_real32(afile); // ???
     vb4.data[i].ny = LE_read_real32(afile); // ???
     vb4.data[i].nz = LE_read_real32(afile); // ???
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     vb4.data[i].tu = LE_read_real32(afile); // UV
     vb4.data[i].tv = LE_read_real32(afile); // UV
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
    }

 // read vertex buffer #4
 AMC_VTXBUFFER vb5;
 vb5.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb5.name = "vb5";
 vb5.elem = 0x51;
 vb5.data.reset(new AMC_VERTEX[vb5.elem]);

 afile.seekg(0x9BAFA8);
 for(uint32 i = 0; i < vb5.elem; i++)
    {
     vb5.data[i].vx = LE_read_real32(afile);
     vb5.data[i].vy = LE_read_real32(afile);
     vb5.data[i].vz = LE_read_real32(afile);
     vb5.data[i].wv[0] = LE_read_real32(afile); // weights
     vb5.data[i].wv[1] = LE_read_real32(afile); // weights
     vb5.data[i].wv[2] = LE_read_real32(afile); // weights
     vb5.data[i].wv[3] = 1.0f - vb5.data[i].wv[0] - vb5.data[i].wv[1] - vb5.data[i].wv[2];
     vb5.data[i].wv[4] = 0.0f;
     vb5.data[i].wv[5] = 0.0f;
     vb5.data[i].wv[6] = 0.0f;
     vb5.data[i].wv[7] = 0.0f;
     vb5.data[i].wi[0] = LE_read_uint08(afile); // blend indices
     vb5.data[i].wi[1] = LE_read_uint08(afile); // blend indices
     vb5.data[i].wi[2] = LE_read_uint08(afile); // blend indices
     vb5.data[i].wi[3] = LE_read_uint08(afile); // blend indices
     vb5.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb5.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb5.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb5.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb5.data[i].nx = LE_read_real32(afile); // ???
     vb5.data[i].ny = LE_read_real32(afile); // ???
     vb5.data[i].nz = LE_read_real32(afile); // ???
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     vb5.data[i].tu = LE_read_real32(afile); // UV
     vb5.data[i].tv = LE_read_real32(afile); // UV
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
    }

 // read vertex buffer #6
 AMC_VTXBUFFER vb6;
 vb6.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb6.name = "vb6";
 vb6.elem = 0x0E;
 vb6.data.reset(new AMC_VERTEX[vb6.elem]);

 afile.seekg(0x9BC70C);
 for(uint32 i = 0; i < vb6.elem; i++)
    {
     vb6.data[i].vx = LE_read_real32(afile);
     vb6.data[i].vy = LE_read_real32(afile);
     vb6.data[i].vz = LE_read_real32(afile);
     vb6.data[i].wv[0] = LE_read_real32(afile); // weights
     vb6.data[i].wv[1] = LE_read_real32(afile); // weights
     vb6.data[i].wv[2] = LE_read_real32(afile); // weights
     vb6.data[i].wv[3] = 1.0f - vb6.data[i].wv[0] - vb6.data[i].wv[1] - vb6.data[i].wv[2];
     vb6.data[i].wv[4] = 0.0f;
     vb6.data[i].wv[5] = 0.0f;
     vb6.data[i].wv[6] = 0.0f;
     vb6.data[i].wv[7] = 0.0f;
     vb6.data[i].wi[0] = LE_read_uint08(afile); // blend indices
     vb6.data[i].wi[1] = LE_read_uint08(afile); // blend indices
     vb6.data[i].wi[2] = LE_read_uint08(afile); // blend indices
     vb6.data[i].wi[3] = LE_read_uint08(afile); // blend indices
     vb6.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb6.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb6.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb6.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb6.data[i].nx = LE_read_real32(afile); // ???
     vb6.data[i].ny = LE_read_real32(afile); // ???
     vb6.data[i].nz = LE_read_real32(afile); // ???
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     vb6.data[i].tu = LE_read_real32(afile); // UV
     vb6.data[i].tv = LE_read_real32(afile); // UV
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
    }

 // read vertex buffer #7
 AMC_VTXBUFFER vb7;
 vb7.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb7.name = "vb7";
 vb7.elem = 0x0E;
 vb7.data.reset(new AMC_VERTEX[vb7.elem]);

 afile.seekg(0x9BCDAC);
 for(uint32 i = 0; i < vb7.elem; i++)
    {
     vb7.data[i].vx = LE_read_real32(afile);
     vb7.data[i].vy = LE_read_real32(afile);
     vb7.data[i].vz = LE_read_real32(afile);
     vb7.data[i].wv[0] = LE_read_real32(afile); // weights
     vb7.data[i].wv[1] = LE_read_real32(afile); // weights
     vb7.data[i].wv[2] = LE_read_real32(afile); // weights
     vb7.data[i].wv[3] = 1.0f - vb7.data[i].wv[0] - vb7.data[i].wv[1] - vb7.data[i].wv[2];
     vb7.data[i].wv[4] = 0.0f;
     vb7.data[i].wv[5] = 0.0f;
     vb7.data[i].wv[6] = 0.0f;
     vb7.data[i].wv[7] = 0.0f;
     vb7.data[i].wi[0] = LE_read_uint08(afile); // blend indices
     vb7.data[i].wi[1] = LE_read_uint08(afile); // blend indices
     vb7.data[i].wi[2] = LE_read_uint08(afile); // blend indices
     vb7.data[i].wi[3] = LE_read_uint08(afile); // blend indices
     vb7.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb7.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb7.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb7.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb7.data[i].nx = LE_read_real32(afile); // ???
     vb7.data[i].ny = LE_read_real32(afile); // ???
     vb7.data[i].nz = LE_read_real32(afile); // ???
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     vb7.data[i].tu = LE_read_real32(afile); // UV
     vb7.data[i].tv = LE_read_real32(afile); // UV
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
    }

 // read vertex buffer #8
 AMC_VTXBUFFER vb8;
 vb8.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb8.name = "vb8";
 vb8.elem = 0x5D4;
 vb8.data.reset(new AMC_VERTEX[vb8.elem]);

 afile.seekg(0x9BEE6C);
 for(uint32 i = 0; i < vb8.elem; i++)
    {
     vb8.data[i].vx = LE_read_real32(afile);
     vb8.data[i].vy = LE_read_real32(afile);
     vb8.data[i].vz = LE_read_real32(afile);
     vb8.data[i].wv[0] = LE_read_real32(afile); // weights
     vb8.data[i].wv[1] = LE_read_real32(afile); // weights
     vb8.data[i].wv[2] = LE_read_real32(afile); // weights
     vb8.data[i].wv[3] = 1.0f - vb8.data[i].wv[0] - vb8.data[i].wv[1] - vb8.data[i].wv[2];
     vb8.data[i].wv[4] = 0.0f;
     vb8.data[i].wv[5] = 0.0f;
     vb8.data[i].wv[6] = 0.0f;
     vb8.data[i].wv[7] = 0.0f;
     vb8.data[i].wi[0] = LE_read_uint08(afile); // blend indices
     vb8.data[i].wi[1] = LE_read_uint08(afile); // blend indices
     vb8.data[i].wi[2] = LE_read_uint08(afile); // blend indices
     vb8.data[i].wi[3] = LE_read_uint08(afile); // blend indices
     vb8.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb8.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb8.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb8.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb8.data[i].nx = LE_read_real32(afile); // ???
     vb8.data[i].ny = LE_read_real32(afile); // ???
     vb8.data[i].nz = LE_read_real32(afile); // ???
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     vb8.data[i].tu = LE_read_real32(afile); // UV
     vb8.data[i].tv = LE_read_real32(afile); // UV
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
    }

 // read vertex buffer #9
 AMC_VTXBUFFER vb9;
 vb9.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb9.name = "vb9";
 vb9.elem = 0x2F;
 vb9.data.reset(new AMC_VERTEX[vb9.elem]);

 afile.seekg(0x9D66C4);
 for(uint32 i = 0; i < vb9.elem; i++)
    {
     vb9.data[i].vx = LE_read_real32(afile);
     vb9.data[i].vy = LE_read_real32(afile);
     vb9.data[i].vz = LE_read_real32(afile);
     vb9.data[i].wv[0] = LE_read_real32(afile); // weights
     vb9.data[i].wv[1] = LE_read_real32(afile); // weights
     vb9.data[i].wv[2] = LE_read_real32(afile); // weights
     vb9.data[i].wv[3] = 1.0f - vb9.data[i].wv[0] - vb9.data[i].wv[1] - vb9.data[i].wv[2];
     vb9.data[i].wv[4] = 0.0f;
     vb9.data[i].wv[5] = 0.0f;
     vb9.data[i].wv[6] = 0.0f;
     vb9.data[i].wv[7] = 0.0f;
     vb9.data[i].wi[0] = LE_read_uint08(afile); // blend indices
     vb9.data[i].wi[1] = LE_read_uint08(afile); // blend indices
     vb9.data[i].wi[2] = LE_read_uint08(afile); // blend indices
     vb9.data[i].wi[3] = LE_read_uint08(afile); // blend indices
     vb9.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb9.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb9.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb9.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb9.data[i].nx = LE_read_real32(afile); // ???
     vb9.data[i].ny = LE_read_real32(afile); // ???
     vb9.data[i].nz = LE_read_real32(afile); // ???
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     vb9.data[i].tu = LE_read_real32(afile); // UV
     vb9.data[i].tv = LE_read_real32(afile); // UV
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
    }

 // read vertex buffer #10
 AMC_VTXBUFFER vb10;
 vb10.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
 vb10.name = "vb10";
 vb10.elem = 0x35;
 vb10.data.reset(new AMC_VERTEX[vb10.elem]);

 afile.seekg(0x9D75FC);
 for(uint32 i = 0; i < vb10.elem; i++)
    {
     vb10.data[i].vx = LE_read_real32(afile);
     vb10.data[i].vy = LE_read_real32(afile);
     vb10.data[i].vz = LE_read_real32(afile);
     vb10.data[i].wv[0] = LE_read_real32(afile); // weights
     vb10.data[i].wv[1] = LE_read_real32(afile); // weights
     vb10.data[i].wv[2] = LE_read_real32(afile); // weights
     vb10.data[i].wv[3] = 1.0f - vb10.data[i].wv[0] - vb10.data[i].wv[1] - vb10.data[i].wv[2];
     vb10.data[i].wv[4] = 0.0f;
     vb10.data[i].wv[5] = 0.0f;
     vb10.data[i].wv[6] = 0.0f;
     vb10.data[i].wv[7] = 0.0f;
     vb10.data[i].wi[0] = LE_read_uint08(afile); // blend indices
     vb10.data[i].wi[1] = LE_read_uint08(afile); // blend indices
     vb10.data[i].wi[2] = LE_read_uint08(afile); // blend indices
     vb10.data[i].wi[3] = LE_read_uint08(afile); // blend indices
     vb10.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb10.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb10.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb10.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb10.data[i].nx = LE_read_real32(afile); // ???
     vb10.data[i].ny = LE_read_real32(afile); // ???
     vb10.data[i].nz = LE_read_real32(afile); // ???
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     LE_read_uint08(afile); // 0xFF
     vb10.data[i].tu = LE_read_real32(afile); // UV
     vb10.data[i].tv = LE_read_real32(afile); // UV
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
     LE_read_real32(afile); // ???
    }

 // struct AMC_SURFMAT {
 //  std::string name; // name
 //  uint08 twoside;   // double-sided
 //  uint16 basemap;   // reference to texture index
 //  uint16 specmap;   // reference to texture index
 //  uint16 tranmap;   // reference to texture index
 //  uint16 bumpmap;   // reference to texture index
 //  uint16 normmap;   // reference to texture index
 //  uint16 resmap1;   // reserved1
 //  uint16 resmap2;   // reserved2
 //  uint16 resmap3;   // reserved3
 //  uint16 resmap4;   // reserved4
 // };

 // images
 std::deque<AMC_IMAGEFILE> images;

 AMC_IMAGEFILE image1;
 image1.filename = "merida_d.dds";
 images.push_back(image1);

 AMC_IMAGEFILE image2;
 image2.filename = "meridasword_d.dds";
 images.push_back(image2);

 AMC_IMAGEFILE image3;
 image3.filename = "meridahair_d.dds";
 images.push_back(image3);

 // materials
 deque<AMC_SURFMAT> materials;

 AMC_SURFMAT m1;
 m1.name = "mbody";
 m1.twoside = false;
 m1.basemap = 0;
 m1.specmap = AMC_INVALID_TEXTURE;
 m1.tranmap = AMC_INVALID_TEXTURE;
 m1.bumpmap = AMC_INVALID_TEXTURE;
 m1.normmap = AMC_INVALID_TEXTURE;
 m1.resmap1 = AMC_INVALID_TEXTURE;
 m1.resmap2 = AMC_INVALID_TEXTURE;
 m1.resmap3 = AMC_INVALID_TEXTURE;
 m1.resmap4 = AMC_INVALID_TEXTURE;
 materials.push_back(m1);

 AMC_SURFMAT m2;
 m2.name = "msword";
 m2.twoside = false;
 m2.basemap = 1;
 m2.specmap = AMC_INVALID_TEXTURE;
 m2.tranmap = AMC_INVALID_TEXTURE;
 m2.bumpmap = AMC_INVALID_TEXTURE;
 m2.normmap = AMC_INVALID_TEXTURE;
 m2.resmap1 = AMC_INVALID_TEXTURE;
 m2.resmap2 = AMC_INVALID_TEXTURE;
 m2.resmap3 = AMC_INVALID_TEXTURE;
 m2.resmap4 = AMC_INVALID_TEXTURE;
 materials.push_back(m2);

 AMC_SURFMAT m3;
 m3.name = "mhair";
 m3.twoside = false;
 m3.basemap = 2;
 m3.specmap = AMC_INVALID_TEXTURE;
 m3.tranmap = AMC_INVALID_TEXTURE;
 m3.bumpmap = AMC_INVALID_TEXTURE;
 m3.normmap = AMC_INVALID_TEXTURE;
 m3.resmap1 = AMC_INVALID_TEXTURE;
 m3.resmap2 = AMC_INVALID_TEXTURE;
 m3.resmap3 = AMC_INVALID_TEXTURE;
 m3.resmap4 = AMC_INVALID_TEXTURE;
 materials.push_back(m3);

 // surfaces
 AMC_SURFACE s1;
 s1.name = "surface1";
 s1.surfmat = 1; // sword
 AMC_REFERENCE r1;
 r1.vb_index = 0;
 r1.vb_start = 0;
 r1.vb_width = vb1.elem;
 r1.ib_index = 0;
 r1.ib_start = 0;
 r1.ib_width = ib1.elem;
 s1.refs.push_back(r1);

 AMC_SURFACE s2;
 s2.name = "surface2";
 s2.surfmat = 0; // body
 AMC_REFERENCE r2;
 r2.vb_index = 1;
 r2.vb_start = 0;
 r2.vb_width = vb2.elem;
 r2.ib_index = 1;
 r2.ib_start = 0;
 r2.ib_width = ib2.elem;
 s2.refs.push_back(r2);

 AMC_SURFACE s3;
 s3.name = "surface3";
 s3.surfmat = 0; // body
 AMC_REFERENCE r3;
 r3.vb_index = 2;
 r3.vb_start = 0;
 r3.vb_width = vb3.elem;
 r3.ib_index = 2;
 r3.ib_start = 0;
 r3.ib_width = ib3.elem;
 s3.refs.push_back(r3);

 AMC_SURFACE s4;
 s4.name = "surface4";
 s4.surfmat = 2; // hair
 AMC_REFERENCE r4;
 r4.vb_index = 3;
 r4.vb_start = 0;
 r4.vb_width = vb4.elem;
 r4.ib_index = 3;
 r4.ib_start = 0;
 r4.ib_width = ib4.elem;
 s4.refs.push_back(r4);

 AMC_SURFACE s5;
 s5.name = "surface5";
 s5.surfmat = 2; // hair
 AMC_REFERENCE r5;
 r5.vb_index = 4;
 r5.vb_start = 0;
 r5.vb_width = vb5.elem;
 r5.ib_index = 4;
 r5.ib_start = 0;
 r5.ib_width = ib5.elem;
 s5.refs.push_back(r5);

 AMC_SURFACE s6;
 s6.name = "surface6";
 s6.surfmat = 2; // hair
 AMC_REFERENCE r6;
 r6.vb_index = 5;
 r6.vb_start = 0;
 r6.vb_width = vb6.elem;
 r6.ib_index = 5;
 r6.ib_start = 0;
 r6.ib_width = ib6.elem;
 s6.refs.push_back(r6);

 AMC_SURFACE s7;
 s7.name = "surface7";
 s7.surfmat = 2; // hair
 AMC_REFERENCE r7;
 r7.vb_index = 6;
 r7.vb_start = 0;
 r7.vb_width = vb7.elem;
 r7.ib_index = 6;
 r7.ib_start = 0;
 r7.ib_width = ib7.elem;
 s7.refs.push_back(r7);

 AMC_SURFACE s8;
 s8.name = "surface8";
 s8.surfmat = 2; // hair
 AMC_REFERENCE r8;
 r8.vb_index = 7;
 r8.vb_start = 0;
 r8.vb_width = vb8.elem;
 r8.ib_index = 7;
 r8.ib_start = 0;
 r8.ib_width = ib8.elem;
 s8.refs.push_back(r8);

 AMC_SURFACE s9;
 s9.name = "surface9";
 s9.surfmat = 2; // hair
 AMC_REFERENCE r9;
 r9.vb_index = 8;
 r9.vb_start = 0;
 r9.vb_width = vb9.elem;
 r9.ib_index = 8;
 r9.ib_start = 0;
 r9.ib_width = ib9.elem;
 s9.refs.push_back(r9);

 AMC_SURFACE s10;
 s10.name = "surface10";
 s10.surfmat = 2; // hair
 AMC_REFERENCE r10;
 r10.vb_index = 9;
 r10.vb_start = 0;
 r10.vb_width = vb10.elem;
 r10.ib_index = 9;
 r10.ib_start = 0;
 r10.ib_width = ib10.elem;
 s10.refs.push_back(r10);

 ADVANCEDMODELCONTAINER amc;
 amc.iflist = images;
 amc.surfmats = materials;
 amc.vblist.push_back(vb1);
 amc.vblist.push_back(vb2);
 amc.vblist.push_back(vb3);
 amc.vblist.push_back(vb4);
 amc.vblist.push_back(vb5);
 amc.vblist.push_back(vb6);
 amc.vblist.push_back(vb7);
 amc.vblist.push_back(vb8);
 amc.vblist.push_back(vb9);
 amc.vblist.push_back(vb10);
 amc.iblist.push_back(ib1);
 amc.iblist.push_back(ib2);
 amc.iblist.push_back(ib3);
 amc.iblist.push_back(ib4);
 amc.iblist.push_back(ib5);
 amc.iblist.push_back(ib6);
 amc.iblist.push_back(ib7);
 amc.iblist.push_back(ib8);
 amc.iblist.push_back(ib9);
 amc.iblist.push_back(ib10);
 amc.surfaces.push_back(s1);
 amc.surfaces.push_back(s2);
 amc.surfaces.push_back(s3);
 amc.surfaces.push_back(s4);
 amc.surfaces.push_back(s5);
 amc.surfaces.push_back(s6);
 amc.surfaces.push_back(s7);
 amc.surfaces.push_back(s8);
 amc.surfaces.push_back(s9);
 amc.surfaces.push_back(s10);

 // create skeleton
 AMC_SKELETON skeleton;
 skeleton.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX;
 skeleton.name = "hs";

 // read matrices
 afile.seekg(0x9B8980);
 boost::shared_array<boost::shared_array<real32>> mlist(new boost::shared_array<real32>[0x2A]);
 for(uint32 i = 0; i < 0x2A; i++) {
     mlist[i].reset(new real32[16]);
     LE_read_array(afile, mlist[i].get(), 16);
    }

/*
 // read matrices
 afile.seekg(0x9B7C60);
 for(uint32 i = 0; i < 0x2A; i++)
    {
     real32 m[16];
     LE_read_array(afile, &m[0], 16);

     uint16 p01 = LE_read_uint16(afile);
     uint16 p02 = LE_read_uint16(afile);
     uint16 p03 = LE_read_uint16(afile);
     uint16 p04 = LE_read_uint16(afile);
     uint16 p05 = LE_read_uint16(afile);
     uint16 p06 = LE_read_uint16(afile);
     uint16 p07 = LE_read_uint16(afile);
     uint16 p08 = LE_read_uint16(afile);

     // construct joint name
     stringstream jn;
     jn << "hsj" << setfill('0') << setw(3) << i;

     // set parent
     uint32 parent = AMC_INVALID_JOINT;
     if(p01 == 0xFFFF) parent = AMC_INVALID_JOINT;
     else parent = p01;

     AMC_JOINT joint;
     joint.name = jn.str();
     joint.parent = parent;
     joint.m_abs[ 0] = 1.0f; joint.m_abs[ 1] = 0.0f; joint.m_abs[ 2] = 0.0f; joint.m_abs[ 3] = 0.0f;
     joint.m_abs[ 4] = 0.0f; joint.m_abs[ 5] = 1.0f; joint.m_abs[ 6] = 0.0f; joint.m_abs[ 7] = 0.0f;
     joint.m_abs[ 8] = 0.0f; joint.m_abs[ 9] = 0.0f; joint.m_abs[10] = 1.0f; joint.m_abs[11] = 0.0f;
     joint.m_abs[12] = 0.0f; joint.m_abs[13] = 0.0f; joint.m_abs[14] = 0.0f; joint.m_abs[16] = 1.0f;
     joint.p_rel[0] = mlist[i][12];
     joint.p_rel[1] = mlist[i][13];
     joint.p_rel[2] = mlist[i][14];
     InsertJoint(skeleton, joint);

     cout << "[";
     cout << m[0] << ", ";
     cout << m[1] << ", ";
     cout << m[2] << ", ";
     cout << m[3] << ", ";
     cout << m[4] << ", ";
     cout << m[5] << ", ";
     cout << m[6] << ", ";
     cout << m[7] << ", ";
     cout << m[8] << ", ";
     cout << m[9] << ", ";
     cout << m[10] << ", ";
     cout << m[11] << ", ";
     cout << m[12] << ", ";
     cout << m[13] << ", ";
     cout << m[14] << ", ";
     cout << m[15] << "]" << endl;

     cout << "0x" << hex << p01 << dec << ",";
     cout << "0x" << hex << p02 << dec << ",";
     cout << "0x" << hex << p03 << dec << ",";
     cout << "0x" << hex << p04 << dec << ",";
     cout << "0x" << hex << p05 << dec << ",";
     cout << "0x" << hex << p06 << dec << ",";
     cout << "0x" << hex << p07 << dec << ",";
     cout << "0x" << hex << p08 << dec << endl;
     cout << endl;
    }
 amc.skllist.push_back(skeleton);
*/

 // save LWO
 SaveLWO(pathname.c_str(), "merida", amc);
 return true;
}

bool TestNamiSkeleton(void)
{
 using namespace std;
 ifstream ifile("000.g1m", ios::binary);
 if(!ifile) return error("Failed to open file.");

 ifile.seekg(0xA8);
 if(ifile.fail()) return error("Seek failure.");

 // read chunk type
 uint64 chunktype = BE_read_uint64(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(chunktype != 0x47314D5330303332LU) return error("Invalid G1MS0032.");

 // read chunksize, exit if nothing to read
 uint32 chunksize = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(chunksize == 0x0C) return true;

 // read chunk data
 uint32 bsSize = chunksize - 0x0C;
 boost::shared_array<char> bsData(new char[bsSize]);
 ifile.read(bsData.get(), bsSize);
 if(ifile.fail()) return error("Read failure.");

 // read header
 binary_stream bs(bsData, bsSize);
 uint32 head01 = bs.BE_read_uint32(); // offset to index data
 uint32 head02 = bs.BE_read_uint32(); // unknown
 uint16 head03 = bs.BE_read_uint16(); // number of joints in XXX.g1m file
 uint16 head04 = bs.BE_read_uint16(); // number of joints in 000.g1m file
 uint16 head05 = bs.BE_read_uint16(); // 0x0001
 uint16 head06 = bs.BE_read_uint16(); // 0x0000

 // joint data structure
 struct G1MSITEM {
  uint16 parent;
  real32 sx, sy, sz;
  real32 qx, qy, qz, qw;
  real32 px, py, pz, pw;
 };

 // read indices
 cout << "INDICES" << endl;
 deque<uint16> idlist;
 map<uint16, G1MSITEM> jointmap;
 for(uint16 i = 0; i < head04; i++) {
     uint16 id = bs.BE_read_uint16();
     G1MSITEM item;
     item.parent = 0xFFFF;
     item.sx = item.sy = item.sz = 0.0f;
     item.qx = item.qy = item.qz = item.qw = 0.0f;
     item.px = item.py = item.pz = item.pw = 0.0f;
     jointmap.insert(map<uint16, G1MSITEM>::value_type(id, item));
     idlist.push_back(id);
    }
 cout << endl;

 // adjust offset
 head01 = head01 - 0x0C;
 bs.seek(head01);
 if(bs.fail()) return error("Seek failure.");

 // create output file
 ofstream ofile("001.obj");
 if(!ofile) return error("Failed to create output file.");

 // read joint information
 cout << "JOINTS" << endl;
 for(uint16 i = 0; i < head03; i++)
    {
     // scale
     real32 sx = bs.BE_read_real32();
     real32 sy = bs.BE_read_real32();
     real32 sz = bs.BE_read_real32();
     cout << " scale: <" << sx << ", " << sy << ", " << sz << ">" << endl;

     // parent
     uint32 parent = bs.BE_read_uint32();
     cout << " parent: <" << parent << ">" << endl;

     // local rotation
     real32 qx = bs.BE_read_real32();
     real32 qy = bs.BE_read_real32();
     real32 qz = bs.BE_read_real32();
     real32 qw = bs.BE_read_real32();
     cout << " rotation = <" << qx << ", " << qy << ", " << qz << ", " << qw << ">" << endl;

     // local position
     real32 px = bs.BE_read_real32();
     real32 py = bs.BE_read_real32();
     real32 pz = bs.BE_read_real32();
     real32 pw = bs.BE_read_real32();
     cout << " position = <" << px << ", " << py << ", " << pz << ", " << pw << ">" << endl;

     // get G1MSITEM from joint id
     typedef map<uint16, G1MSITEM>::iterator iterator;
     iterator curr_iter = jointmap.find(idlist[i]);
     if(curr_iter == jointmap.end()) return error("Could not find joint in jointmap.");

     if(parent == 0xFFFFFFFF) {
        curr_iter->second.parent = 0xFFFF;
        curr_iter->second.px = px;
        curr_iter->second.py = py;
        curr_iter->second.pz = pz;
        curr_iter->second.pw = pw;
       }
     else {
        iterator parent_iter = jointmap.find(parent);
        if(parent_iter == jointmap.end()) return error("Could not find parent joint in jointmap.");
        curr_iter->second.parent = parent;
        curr_iter->second.px = px;
        curr_iter->second.py = py;
        curr_iter->second.pz = pz;
        curr_iter->second.pw = pw;
       }

     ofile << "v " << curr_iter->second.px << " " << curr_iter->second.py << " " << curr_iter->second.pz << endl;

     cout << endl;
    }
 cout << endl;

 return true;
}