#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_amc.h"
#include "ps3_tekken_hybrid.h"

#define X_SYSTEM PS3
#define X_GAME   TekkenHybrid

namespace X_SYSTEM { namespace X_GAME {

class extractor {
 private :
  std::string pathname;
 private :
 public :
  bool extract(void);
 public :
  extractor(const char* pn) : pathname(pn) {}
 ~extractor() {}
};

class modelizer {
 private :
  bool debug;
  std::string filename;
  std::string pathname;
  std::string shrtname;
  std::ifstream ifile;
  std::ofstream ofile;
  std::ofstream dfile;
 private :
  ADVANCEDMODELCONTAINER amc;
 private :
 public :
  bool extract(void);
 public :
  modelizer(const std::string& fname);
 ~modelizer();
};

};};

namespace X_SYSTEM { namespace X_GAME {

bool extract(void)
{
 std::string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pathname)
{
 return extractor(pathname).extract();
}

};};

namespace X_SYSTEM { namespace X_GAME {

bool extractor::extract(void)
{
 using namespace std;
 deque<string> filelist;
 BuildFilenameList(filelist, ".bin", pathname.c_str());
 for(size_t i = 0; i < filelist.size(); i++) {
     cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     modelizer m(filelist[i]);
     if(!m.extract()) return false;
    }
 cout << endl;
 return true;
}

};};

namespace X_SYSTEM { namespace X_GAME {

modelizer::modelizer(const std::string& fname) : debug(true)
{
 filename = fname;
 pathname = GetPathnameFromFilename(fname);
 shrtname = GetShortFilenameWithoutExtension(fname);
}

modelizer::~modelizer()
{
}

bool modelizer::extract(void)
{
 // open file
 using namespace std;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // create debug file
 std::string dfname = pathname;
 dfname += shrtname;
 dfname += ".txt";
 if(debug) dfile.open(dfname.c_str());

 // read number of models
 uint32 n_models = BE_read_uint32(ifile);
 if(!n_models) return error("Invalid number of models.");

 // create model pathname
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // create OBJ file
 string mfname = savepath;
 mfname += "model.obj";
 ofstream objfile(mfname.c_str());
 if(!objfile) return error("Failed to create OBJ file.");
 objfile << "o model.obj" << endl;
 objfile << "mtllib model.mtl" << endl;

 // create MTL file
 string sfname = savepath;
 sfname += "model.mtl";
 ofstream mtlfile(sfname.c_str());

 // for each model
 uint32 current_index = 0;
 for(uint32 model = 0; model < n_models; model++)
    {
     // read parameters
     uint32 p01 = BE_read_uint32(ifile); // ??? 0xA6
     uint32 p02 = BE_read_uint32(ifile); // 0x4E (texture id)
     uint32 p03 = BE_read_uint32(ifile); // ??? 0x300, 0x080
     uint08 p04 = BE_read_uint08(ifile); // 0xFF
     uint08 p05 = BE_read_uint08(ifile); // 0x00 or 0x01
     uint08 p06 = BE_read_uint08(ifile); // 0x00 or 0x01

     // define surface
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(4) << model;
     objfile << "g " << ss.str() << endl;
     objfile << "usemtl " << "material_" << setfill('0') << setw(3) << p02 << endl;

     // read number of vertices
     uint32 n_vertices = BE_read_uint32(ifile);
     if(!n_vertices) return error("Invalid number of vertices.");
    
     // read vertices
     uint32 vsize = n_vertices*0x24;
     boost::shared_array<char> vdata(new char[vsize]);
     ifile.read(vdata.get(), vsize);
     if(ifile.fail()) return error("Read error.");
    
     // dump vertex data
     if(debug) {
        dfile << "-----------" << endl;
        dfile << "VERTEX DATA" << endl;
        dfile << "-----------" << endl;
        dfile << endl;
        binary_stream bs(vdata, vsize);
        for(uint32 i = 0; i < n_vertices; i++) {
            for(uint32 j = 0; j < 0x24; j++) dfile << hex << setfill('0') << setw(2) << (uint32)bs.BE_read_uint08() << dec;
            dfile << endl;
           }
        dfile << endl;
       }
    
     // read indices
     uint32 n_indices = BE_read_uint32(ifile);
     if(!n_indices) return error("Invalid number of indices.");
     uint32 isize = n_indices*0x02;
     boost::shared_array<char> idata(new char[isize]);
     ifile.read(idata.get(), isize);
     if(ifile.fail()) return error("Read error.");
    
     // process vertices
     binary_stream bs(vdata, vsize);
     for(uint32 i = 0; i < n_vertices; i++)
        {
         real32 vx = bs.BE_read_real32();
         real32 vy = bs.BE_read_real32();
         real32 vz = bs.BE_read_real32();
         real32 nx = bs.BE_read_real32();
         real32 ny = bs.BE_read_real32();
         real32 nz = bs.BE_read_real32();
         real32 tu = bs.BE_read_real32();
         real32 tv = bs.BE_read_real32();
         uint32 unknown = bs.BE_read_uint32();
         if(debug)
            dfile << "<" << vx << ", " << vy << ", " << vz << ">, <"
                         << nx << ", " << ny << ", " << nz << ">, <"
                         << tu << ", " << tv << ">" << endl;
    
         objfile << "v  " << vx << " " << vy << " " << vz << "\n";
         objfile << "vn " << nx << " " << ny << " " << nz << "\n";
         objfile << "vt " << tu << " " << tv << endl;
        }
     objfile << endl;

     // process first triangle
     binary_stream ibs(idata, isize);
     uint32 a = ibs.BE_read_uint16() + current_index + 1;
     uint32 b = ibs.BE_read_uint16() + current_index + 1;
     uint32 c = ibs.BE_read_uint16() + current_index + 1;
     objfile << "f " << a << "/" << a << "/" << a << " "
                     << b << "/" << b << "/" << b << " "
                     << c << "/" << c << "/" << c << endl;
    
     // process remaining triangles
     for(uint32 i = 3; i < n_indices; i++) {
         a = b;
         b = c;
         c = ibs.BE_read_uint16() + current_index + 1;
         if(i % 2) {
            objfile << "f " << a << "/" << a << "/" << a << " "
                            << c << "/" << c << "/" << c << " "
                            << b << "/" << b << "/" << b << endl;
           }
         else {
            objfile << "f " << a << "/" << a << "/" << a << " "
                            << b << "/" << b << "/" << b << " "
                            << c << "/" << c << "/" << c << endl;
           }
        }
     objfile.write("\n", 1);

     // update current index
     current_index += n_vertices;
     if(debug) dfile << endl;
    }

 if(debug) dfile << "------------" << endl;
 if(debug) dfile << "TEXTURE DATA" << endl;
 if(debug) dfile << "------------" << endl;
 if(debug) dfile << endl;

 //cout << "position = 0x" << hex << setfill('0') << setw(4) << ifile.tellg() << dec << endl;

 uint32 thead00 = BE_read_uint32(ifile); // separator
 if(thead00 == 0x10000) {
    uint32 temp1 = BE_read_uint32(ifile); // 0x2FFFF
    if(temp1 != 0x2FFFF) return error("Expecting 0x2FFFF.");
    BE_read_uint16(ifile); // 0x00
   }
 cout << "position = 0x" << hex << setfill('0') << setw(4) << ifile.tellg() << dec << endl;
 uint32 thead01 = BE_read_uint32(ifile); // number of textures
 if(thead01 == 0) return error("Invalid number of textures.");

 // for each texture
 for(uint32 i = 0; i < thead01; i++)
    {
     // read texture properties
     uint32 p01 = BE_read_uint32(ifile); // 0x40 (dx)
     uint32 p02 = BE_read_uint32(ifile); // 0x40 (dy)
     uint32 p03 = BE_read_uint32(ifile); // 0x4000 (size of image)
     uint32 p04 = BE_read_uint32(ifile); // 0x3400
     uint32 p05 = BE_read_uint32(ifile); // 0x0000
     uint32 p06 = BE_read_uint32(ifile); // 0x01
     uint32 p07 = BE_read_uint32(ifile); // 0x01
     uint32 p08 = BE_read_uint32(ifile); // 0x00
     uint32 p09 = BE_read_uint32(ifile); // 0x01
     uint32 p10 = BE_read_uint32(ifile); // 0x01
     uint32 p11 = BE_read_uint32(ifile); // 0x00FC0001
     uint32 p12 = BE_read_uint32(ifile); // 0x00FF1200

     // read texture
     uint32 datasize = p03;
     if(!datasize) return error("Invalid texture data size.");
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);

     // swap channels
     for(uint32 j = 0; j < p03; j += 4) {
         char b0 = data[j + 0];
         char b1 = data[j + 1];
         char b2 = data[j + 2];
         char b3 = data[j + 3];
         data[j + 0] = b3;
         data[j + 1] = b2;
         data[j + 2] = b1;
         data[j + 3] = b0;
        }

     // construct filename
     stringstream ss;
     ss << savepath << "texture_" << setfill('0') << setw(3) << i << ".tga";
     string tfname = ss.str();

     // save texture
     ofstream ofile(tfname, ios::binary);
     if(!ofile) return error("Failed to create TGA file.");

     // TGA image type
     LE_write_uint08(ofile, (uint08)0);
     LE_write_uint08(ofile, (uint08)0);
     LE_write_uint08(ofile, (uint08)2);

     // TGA color map specification
     LE_write_uint16(ofile, (uint16)0);
     LE_write_uint16(ofile, (uint16)0);
     LE_write_uint08(ofile, (uint08)0);

     // TGA image specification
     LE_write_uint16(ofile, (uint16)0);
     LE_write_uint16(ofile, (uint16)0);
     LE_write_uint16(ofile, (uint16)p01);
     LE_write_uint16(ofile, (uint16)p02);
     LE_write_uint08(ofile, (uint08)32);
     LE_write_uint08(ofile, (uint08)8);

     // TGA image data
     ofile.write(data.get(), datasize);

     // TGA footer
     LE_write_uint32(ofile, (uint32)0);
     LE_write_uint32(ofile, (uint32)0);
     ofile.write("TRUEVISION-XFILE", 16);
     LE_write_uint08(ofile, (uint08)'.');
     LE_write_uint08(ofile, (uint08)0);      

     // save material
     mtlfile << "newmtl " << "material_" << setfill('0') << setw(3) << i << endl;
     mtlfile << "map_Kd " << tfname.c_str() << endl;
     mtlfile << endl;
    }

 return true;
}

}};

