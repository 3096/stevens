#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_smc.h"
#include "x_lwo.h"
#include "ps3_neptunia_mk2.h"

#define X_SYSTEM PS3
#define X_GAME   NEPTUNIA_MK2

namespace X_SYSTEM { namespace X_GAME {

struct point2D {
 real32 x;
 real32 y;
};

struct point3D {
 real32 x, y, z;
};

struct triangle3D {
 uint32 a;
 uint32 b;
 uint32 c;
};

struct TEXTUREINFO {
 unsigned int data1;
 unsigned int data2;
 unsigned int data3;
 unsigned int data4;
 unsigned int data5;
 unsigned int data6;
 unsigned int data7;
 unsigned int data8;
};

class extractor {
 private :
  std::string pathname;
 private :
  std::ifstream ifile;
  std::ofstream dfile;
  bool debug;
 private :
  std::deque<std::string> string_table;
  std::deque<TEXTUREINFO> texture_info;
 private :
  std::deque<point3D> points;
  std::deque<point3D> normal;
  std::deque<point2D> uvdata;
 private :
  bool processTID(const std::string& filename);
  bool processISM(const std::string& filename);
 private :
  bool clearISM(void);
  bool processXXX(uint32 offset, uint32 parent, uint32 caller);
  bool processX03(uint32 offset, uint32 parent, uint32 caller); // R node (scene) (0x04, 0x05 children)
  bool processX04(uint32 offset, uint32 parent, uint32 caller); // C node (scene) (0x4C, 0x4D, 0x5B children)
  bool processX05(uint32 offset, uint32 parent, uint32 caller); // C node (scene) (0x5B, 0x5C children)
  bool processX0A(uint32 offset, uint32 parent, uint32 caller); // C node (geometry) (0x46, 0x59, 0x6E children)
  bool processX0B(uint32 offset, uint32 parent, uint32 caller); // R node (geometry) (0x0A children)
  bool processX0D(uint32 offset, uint32 parent, uint32 caller); // C node (attributes) (0x6C children)
  bool processX0F(uint32 offset, uint32 parent, uint32 caller); // TODO: motion
  bool processX21(uint32 offset, uint32 parent, uint32 caller); // R node (string table) (no children)
  bool processX2D(uint32 offset, uint32 parent, uint32 caller); // C node (texture node) (no children)
  bool processX2E(uint32 offset, uint32 parent, uint32 caller); // R node (texture list) (0x2D children)
  bool processX30(uint32 offset, uint32 parent, uint32 caller); // C node (skinning) (0x44 children)
  bool processX31(uint32 offset, uint32 parent, uint32 caller); // C node (skinning) (0x30 children)
  bool processX32(uint32 offset, uint32 parent, uint32 caller); // R node (skinning) (0x31 children)
  bool processX34(uint32 offset, uint32 parent, uint32 caller); // TODO: motion
  bool processX44(uint32 offset, uint32 parent, uint32 caller); // C node (skinning) (no children)
  bool processX45(uint32 offset, uint32 parent, uint32 caller); // C node (geometry) (no children)
  bool processX46(uint32 offset, uint32 parent, uint32 caller); // C node (geometry) (0x45, 0x6E children)
  bool processX4B(uint32 offset, uint32 parent, uint32 caller); // C node (materials) (no children)
  bool processX4C(uint32 offset, uint32 parent, uint32 caller); // C node (scene) (0x4B children)
  bool processX4D(uint32 offset, uint32 parent, uint32 caller); // C node (scene) (0x4B children)
  bool processX50(uint32 offset, uint32 parent, uint32 caller);
  bool processX59(uint32 offset, uint32 parent, uint32 caller); // C node (geometry) (no children)
  bool processX5B(uint32 offset, uint32 parent, uint32 caller); // C node (transforms) (no children)
  bool processX5C(uint32 offset, uint32 parent, uint32 caller); // C node (scene) (no children)
  bool processX61(uint32 offset, uint32 parent, uint32 caller); // R node (attributes) (0x0D children)
  bool processX62(uint32 offset, uint32 parent, uint32 caller); // R node (0x63 children)
  bool processX63(uint32 offset, uint32 parent, uint32 caller); // C node (0x65 children)
  bool processX65(uint32 offset, uint32 parent, uint32 caller); // C node (no children)
  bool processX6A(uint32 offset, uint32 parent, uint32 caller); // C node (0x6B children)
  bool processX6B(uint32 offset, uint32 parent, uint32 caller); // C node (no children, 0x6B children)
  bool processX6C(uint32 offset, uint32 parent, uint32 caller); // C node (0x6A children)
  bool processX6E(uint32 offset, uint32 parent, uint32 caller); // C node (geometry) (no children)
 public :
  bool extract(void);
 public :
  extractor(const char* pn) : pathname(pn), debug(false) {}
 ~extractor() {}
};

};};

namespace X_SYSTEM { namespace X_GAME {

bool extractor::extract(void)
{
 using namespace std;
 bool doTID = false;
 bool doISM = true;
 debug = true;

 if(doTID) {
    cout << "STAGE 1" << endl;
    cout << "Processing TID files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, "tid", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTID(filelist[i])) return false;
       }
    cout << endl;
   }

 if(doISM) {
    cout << "STAGE 2" << endl;
    cout << "Processing ISM files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, "ism2", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processISM(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

bool extractor::processTID(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open TID file.");

 // filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // create output filename
 string ofname = GetPathnameFromFilename(filename);
 ofname += GetShortFilenameWithoutExtension(filename);
 ofname += ".dds";

 // texture types
 const unsigned int T_TYPE0 = 0x54494413; // uncompressed
 const unsigned int T_TYPE1 = 0x54494481; // DXT1/DXT5
 const unsigned int T_TYPE2 = 0x54494483; // uncompressed
 const unsigned int T_TYPE3 = 0x54494487; // uncompressed (scrambled)
 const unsigned int T_TYPE4 = 0x54494489; // DXT1/DXT5 (upside down)
 const unsigned int T_TYPE5 = 0x54494491; // DXT1/DXT5 (toon_shadow)
 const unsigned int T_TYPE6 = 0x54494493; // uncompressed
 const unsigned int T_TYPE7 = 0x54494497; // uncompressed (scrambled)
 const unsigned int T_TYPE8 = 0x54494499; // DXT1/DXT5 (mipmapped)
 const unsigned int T_TYPE9 = 0x5449449B; // uncompressed (upside down)

 // uncompressed RGBA mask types
 const unsigned int MASK_B = 0xFF000000;
 const unsigned int MASK_G = 0x00FF0000;
 const unsigned int MASK_R = 0x0000FF00;
 const unsigned int MASK_A = 0x000000FF;

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read error.");

 // test magic
 switch(magic) {
   case(T_TYPE0) : break;
   case(T_TYPE1) : break;
   case(T_TYPE2) : break;
   case(T_TYPE3) : break;
   case(T_TYPE4) : break;
   case(T_TYPE5) : break;
   case(T_TYPE6) : break;
   case(T_TYPE7) : break;
   case(T_TYPE8) : break;
   case(T_TYPE9) : break;
   default : return error("Unknown TID file type.");
  }

 // read filesize
 uint32 head01 = BE_read_uint32(ifile);
 if(magic == T_TYPE0 || magic == T_TYPE2 || magic == T_TYPE3 || magic == T_TYPE6 || magic == T_TYPE7) {
    if(head01 != filesize) return error("Read versus actual filesizes do not match.");
   }

 // read offset to texture data
 uint32 head02 = BE_read_uint32(ifile);
 if(head02 == 0) return error("Invalid head02.");

 // read number of files (always 1)
 uint32 head03 = BE_read_uint32(ifile);
 if(head03 != 1) return error("Invalid head03.");

 // TYPE 0
 static uint32 index = 0;
 if(magic == T_TYPE0)
   {
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head04 = BE_read_uint32(ifile); // 64
    uint32 head05 = BE_read_uint32(ifile); // dx
    uint32 head06 = BE_read_uint32(ifile); // dy
    uint32 head07 = BE_read_uint32(ifile); // bpp (32)
    uint32 head08 = BE_read_uint16(ifile); // 1
    uint32 head09 = BE_read_uint16(ifile); // 1
    uint32 head10 = BE_read_uint32(ifile); // 0
    uint32 head11 = BE_read_uint32(ifile); // size of image data
    uint32 head12 = BE_read_uint32(ifile); // 128
    if(head04 != 0x40) return error("TID TYPE 0 unexpected value. Expecting 0x40.");
    if(head07 != 0x20) return error("TID TYPE 0 unexpected value. Expecting 0x20.");
    if(head08 != 0x01) return error("TID TYPE 0 unexpected value. Expecting 0x01.");
    if(head09 != 0x01) return error("TID TYPE 0 unexpected value. Expecting 0x01.");
    if(head10 != 0x00) return error("TID TYPE 0 unexpected value. Expecting 0x00.");
    if(head12 != 0x80) return error("TID TYPE 0 unexpected value. Expecting 0x80.");

    // create DDS header
    DDS_HEADER ddsh;
    CreateUncompressedDDSHeader(head05, head06, 0, MASK_R, MASK_G, MASK_B, MASK_A, FALSE, &ddsh);

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head11]);
    ifile.read(data.get(), head11);

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(data.get(), head11);
   }
 else if(magic == T_TYPE1)
   {
    uint32 head04 = BE_read_uint32(ifile); // ??
    uint32 head05 = BE_read_uint32(ifile); // ??
    uint32 head06 = BE_read_uint32(ifile); // ?? 
    uint32 head07 = BE_read_uint32(ifile); // ??

    // read filename
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head08 = BE_read_uint32(ifile); // 0x60
    uint32 head09 = BE_read_uint32(ifile); // dx
    uint32 head10 = BE_read_uint32(ifile); // dy
    uint32 head11 = BE_read_uint32(ifile); // 0x00
    uint32 head12 = BE_read_uint16(ifile); // 0x01
    uint32 head13 = BE_read_uint16(ifile); // 0x01
    uint32 head14 = BE_read_uint32(ifile); // 0x00
    uint32 head15 = BE_read_uint32(ifile); // size of image data
    uint32 head16 = BE_read_uint32(ifile); // 0x80
    uint32 head17 = BE_read_uint32(ifile); // 0x04
    uint32 head18 = BE_read_uint32(ifile); // compression
    uint32 head19 = BE_read_uint32(ifile); // 0x1000
    uint32 head20 = BE_read_uint32(ifile); // 0x00
    if(head08 != 0x60) return error("TID TYPE 1 unexpected value. Expecting 0x60.");
    if(head11 != 0x00) return error("TID TYPE 1 unexpected value. Expecting 0x00.");
    if(head12 != 0x01) return error("TID TYPE 1 unexpected value. Expecting 0x01.");
    if(head13 != 0x01) return error("TID TYPE 1 unexpected value. Expecting 0x01.");
    if(head14 != 0x00) return error("TID TYPE 1 unexpected value. Expecting 0x00.");
    if(head16 != 0x80) return error("TID TYPE 1 unexpected value. Expecting 0x80.");
    if(head17 != 0x04) return error("TID TYPE 1 unexpected value. Expecting 0x04.");
    if(head20 != 0x00) return error("TID TYPE 1 unexpected value. Expecting 0x00.");

    // create DDS header
    DDS_HEADER ddsh;
    if(head18 == 0x31545844) CreateDXT1Header(head09, head10, 0, FALSE, &ddsh);
    else if(head18 == 0x35545844) CreateDXT5Header(head09, head10, 0, FALSE, &ddsh);
    else return error("Unknown compression type.");

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head15]);
    ifile.read(data.get(), head15);

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(data.get(), head15);
   }
 else if(magic == T_TYPE2)
   {
    uint32 head04 = BE_read_uint32(ifile); // 0x01
    uint32 head05 = BE_read_uint32(ifile); // 0x20
    uint32 head06 = BE_read_uint32(ifile); // 0x00
    uint32 head07 = BE_read_uint32(ifile); // 0x00

    // read filename
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head08 = BE_read_uint32(ifile); // 0x60
    uint32 head09 = BE_read_uint32(ifile); // dx
    uint32 head10 = BE_read_uint32(ifile); // dy
    uint32 head11 = BE_read_uint32(ifile); // 0x20
    uint32 head12 = BE_read_uint16(ifile); // 0x01
    uint32 head13 = BE_read_uint16(ifile); // 0x01
    uint32 head14 = BE_read_uint32(ifile); // 0x00
    uint32 head15 = BE_read_uint32(ifile); // size of image data
    uint32 head16 = BE_read_uint32(ifile); // 0x80

    // create DDS header
    DDS_HEADER ddsh;
    CreateUncompressedDDSHeader(head09, head10, 0, MASK_R, MASK_G, MASK_B, MASK_A, FALSE, &ddsh);

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head15]);
    ifile.read(data.get(), head15);

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(data.get(), head15);
   }
 else if(magic == T_TYPE3)
   {
    uint32 head04 = BE_read_uint32(ifile); // 0x01
    uint32 head05 = BE_read_uint32(ifile); // 0x20
    uint32 head06 = BE_read_uint32(ifile); // 0x00
    uint32 head07 = BE_read_uint32(ifile); // 0x00

    // read filename
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head08 = BE_read_uint32(ifile); // 0x60
    uint32 head09 = BE_read_uint32(ifile); // dx
    uint32 head10 = BE_read_uint32(ifile); // dy
    uint32 head11 = BE_read_uint32(ifile); // 0x20
    uint32 head12 = BE_read_uint16(ifile); // 0x01
    uint32 head13 = BE_read_uint16(ifile); // 0x01
    uint32 head14 = BE_read_uint32(ifile); // 0x00
    uint32 head15 = BE_read_uint32(ifile); // size of image data
    uint32 head16 = BE_read_uint32(ifile); // 0x80

    // create DDS header
    DDS_HEADER ddsh;
    CreateUncompressedDDSHeader(head09, head10, 0, MASK_R, MASK_G, MASK_B, MASK_A, FALSE, &ddsh);

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head15]);
    boost::shared_array<char> copy(new char[head15]);
    ifile.read(data.get(), head15);

    // deswizzle
    for(uint32 r = 0; r < head10; r++) {
        for(uint32 c = 0; c < head09; c++) {
            uint32 morton = array_to_morton(r, c);
            uint32 copy_position = 4*r*head09 + 4*c;
            uint32 data_position = 4*morton;
            copy[copy_position++] = data[data_position + 0]; // b
            copy[copy_position++] = data[data_position + 1]; // g
            copy[copy_position++] = data[data_position + 2]; // r
            copy[copy_position++] = data[data_position + 3]; // a
           }
       }

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(copy.get(), head15);
   }
 else if(magic == T_TYPE4)
   {
    uint32 head04 = BE_read_uint32(ifile); // 0x01
    uint32 head05 = BE_read_uint32(ifile); // 0x20
    uint32 head06 = BE_read_uint32(ifile); // 0x00
    uint32 head07 = BE_read_uint32(ifile); // 0x00

    // read filename
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head08 = BE_read_uint32(ifile); // 0x60
    uint32 head09 = BE_read_uint32(ifile); // dx
    uint32 head10 = BE_read_uint32(ifile); // dy
    uint32 head11 = BE_read_uint32(ifile); // 0x00
    uint32 head12 = BE_read_uint16(ifile); // 0x01
    uint32 head13 = BE_read_uint16(ifile); // 0x01
    uint32 head14 = BE_read_uint32(ifile); // 0x00
    uint32 head15 = BE_read_uint32(ifile); // size of image data
    uint32 head16 = BE_read_uint32(ifile); // 0x80
    uint32 head17 = BE_read_uint32(ifile); // 0x04
    uint32 head18 = BE_read_uint32(ifile); // compression
    uint32 head19 = BE_read_uint32(ifile); // 0x1000
    uint32 head20 = BE_read_uint32(ifile); // 0x00

    // create DDS header
    DDS_HEADER ddsh;
    if(head18 == 0x31545844) CreateDXT1Header(head09, head10, 0, FALSE, &ddsh);
    else if(head18 == 0x35545844) CreateDXT5Header(head09, head10, 0, FALSE, &ddsh);
    else return error("Unknown compression type.");

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head15]);
    ifile.read(data.get(), head15);

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(data.get(), head15);
   }
 else if(magic == T_TYPE5)
   {
    uint32 head04 = BE_read_uint32(ifile); // 0x01
    uint32 head05 = BE_read_uint32(ifile); // 0x20
    uint32 head06 = BE_read_uint32(ifile); // 0x00
    uint32 head07 = BE_read_uint32(ifile); // 0x00

    // read filename
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head08 = BE_read_uint32(ifile); // 0x60
    uint32 head09 = BE_read_uint32(ifile); // dx
    uint32 head10 = BE_read_uint32(ifile); // dy
    uint32 head11 = BE_read_uint32(ifile); // 0x00
    uint32 head12 = BE_read_uint16(ifile); // 0x01
    uint32 head13 = BE_read_uint16(ifile); // 0x02 (different)
    uint32 head14 = BE_read_uint32(ifile); // 0x00
    uint32 head15 = BE_read_uint32(ifile); // size of image data
    uint32 head16 = BE_read_uint32(ifile); // 0x80
    uint32 head17 = BE_read_uint32(ifile); // 0x04
    uint32 head18 = BE_read_uint32(ifile); // compression
    uint32 head19 = BE_read_uint32(ifile); // ????
    uint32 head20 = BE_read_uint32(ifile); // 0x00

    // create DDS header
    DDS_HEADER ddsh;
    if(head18 == 0x31545844) CreateDXT1Header(head09, head10, 0, FALSE, &ddsh);
    else if(head18 == 0x35545844) CreateDXT5Header(head09, head10, 0, FALSE, &ddsh);
    else return error("Unknown compression type.");

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head15]);
    ifile.read(data.get(), head15);

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(data.get(), head15);
   }
 else if(magic == T_TYPE6)
   {
    uint32 head04 = BE_read_uint32(ifile); // 0x01
    uint32 head05 = BE_read_uint32(ifile); // 0x20
    uint32 head06 = BE_read_uint32(ifile); // 0x00
    uint32 head07 = BE_read_uint32(ifile); // 0x00

    // read filename
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head08 = BE_read_uint32(ifile); // 0x60
    uint32 head09 = BE_read_uint32(ifile); // dx
    uint32 head10 = BE_read_uint32(ifile); // dy
    uint32 head11 = BE_read_uint32(ifile); // 0x20
    uint32 head12 = BE_read_uint16(ifile); // 0x01
    uint32 head13 = BE_read_uint16(ifile); // 0x01
    uint32 head14 = BE_read_uint32(ifile); // 0x00
    uint32 head15 = BE_read_uint32(ifile); // size of image data
    uint32 head16 = BE_read_uint32(ifile); // 0x80
    // some extra data at 0x78 (0x0101)

    // create DDS header
    DDS_HEADER ddsh;
    CreateUncompressedDDSHeader(head09, head10, 0, MASK_R, MASK_G, MASK_B, MASK_A, FALSE, &ddsh);

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head15]);
    ifile.read(data.get(), head15);

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(data.get(), head15);
   }
 else if(magic == T_TYPE7)
   {
    uint32 head04 = BE_read_uint32(ifile); // 0x01
    uint32 head05 = BE_read_uint32(ifile); // 0x20
    uint32 head06 = BE_read_uint32(ifile); // 0x00
    uint32 head07 = BE_read_uint32(ifile); // 0x00

    // read filename
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head08 = BE_read_uint32(ifile); // 0x60
    uint32 head09 = BE_read_uint32(ifile); // dx
    uint32 head10 = BE_read_uint32(ifile); // dy
    uint32 head11 = BE_read_uint32(ifile); // 0x20
    uint32 head12 = BE_read_uint16(ifile); // 0x01
    uint32 head13 = BE_read_uint16(ifile); // 0x01
    uint32 head14 = BE_read_uint32(ifile); // 0x00
    uint32 head15 = BE_read_uint32(ifile); // size of image data
    uint32 head16 = BE_read_uint32(ifile); // 0x80

    // create DDS header
    DDS_HEADER ddsh;
    CreateUncompressedDDSHeader(head09, head10, 0, MASK_R, MASK_G, MASK_B, MASK_A, FALSE, &ddsh);

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head15]);
    boost::shared_array<char> copy(new char[head15]);
    ifile.read(data.get(), head15);

    // deswizzle
    for(uint32 r = 0; r < head10; r++) {
        for(uint32 c = 0; c < head09; c++) {
            uint32 morton = array_to_morton(r, c);
            uint32 copy_position = 4*r*head09 + 4*c;
            uint32 data_position = 4*morton;
            copy[copy_position++] = data[data_position + 0]; // b
            copy[copy_position++] = data[data_position + 1]; // g
            copy[copy_position++] = data[data_position + 2]; // r
            copy[copy_position++] = data[data_position + 3]; // a
           }
       }

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(copy.get(), head15);
   }
 else if(magic == T_TYPE8)
   {
    uint32 head04 = BE_read_uint32(ifile); // 0x01
    uint32 head05 = BE_read_uint32(ifile); // 0x20
    uint32 head06 = BE_read_uint32(ifile); // 0x00
    uint32 head07 = BE_read_uint32(ifile); // 0x00

    // read filename
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head08 = BE_read_uint32(ifile); // 0x60
    uint32 head09 = BE_read_uint32(ifile); // dx
    uint32 head10 = BE_read_uint32(ifile); // dy
    uint32 head11 = BE_read_uint32(ifile); // 0x00
    uint32 head12 = BE_read_uint16(ifile); // 0x01
    uint32 head13 = BE_read_uint16(ifile); // 0x05 (different)
    uint32 head14 = BE_read_uint32(ifile); // 0x00
    uint32 head15 = BE_read_uint32(ifile); // size of image data
    uint32 head16 = BE_read_uint32(ifile); // 0x80
    uint32 head17 = BE_read_uint32(ifile); // 0x04
    uint32 head18 = BE_read_uint32(ifile); // compression
    uint32 head19 = BE_read_uint32(ifile); // ????
    uint32 head20 = BE_read_uint32(ifile); // 0x00

    // create DDS header
    DDS_HEADER ddsh;
    if(head18 == 0x31545844) CreateDXT1Header(head09, head10, 1, FALSE, &ddsh);
    else if(head18 == 0x35545844) CreateDXT5Header(head09, head10, 1, FALSE, &ddsh);
    else return error("Unknown compression type.");

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head15]);
    ifile.read(data.get(), head15);

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(data.get(), head15);
   }
 else if(magic == T_TYPE9)
   {
    uint32 head04 = BE_read_uint32(ifile); // 0x01
    uint32 head05 = BE_read_uint32(ifile); // 0x20
    uint32 head06 = BE_read_uint32(ifile); // 0x00
    uint32 head07 = BE_read_uint32(ifile); // 0x00

    // read filename
    char name[32];
    BE_read_array(ifile, &name[0], 32);

    uint32 head08 = BE_read_uint32(ifile); // 0x60
    uint32 head09 = BE_read_uint32(ifile); // dx
    uint32 head10 = BE_read_uint32(ifile); // dy
    uint32 head11 = BE_read_uint32(ifile); // 0x20
    uint32 head12 = BE_read_uint16(ifile); // 0x01
    uint32 head13 = BE_read_uint16(ifile); // 0x01
    uint32 head14 = BE_read_uint32(ifile); // 0x00
    uint32 head15 = BE_read_uint32(ifile); // size of image data
    uint32 head16 = BE_read_uint32(ifile); // 0x80

    // create DDS header
    DDS_HEADER ddsh;
    CreateUncompressedDDSHeader(head09, head10, 0, MASK_R, MASK_G, MASK_B, MASK_A, FALSE, &ddsh);

    // read DDS data
    ifile.seekg(head02);
    boost::shared_array<char> data(new char[head15]);
    ifile.read(data.get(), head15);

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(data.get(), head15);
   }

 return true;
}

bool extractor::processISM(const std::string& filename)
{
 // clear previous
 clearISM();

 // open file
 using namespace std;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open ISM2 file.");

 // filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // paths
 string savepath = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // debug file
 string dfname = savepath;
 dfname += shrtname;
 dfname += ".txt";
 if(debug) dfile.open(dfname.c_str());

 // read header
 uint32 head01 = BE_read_uint32(ifile); // magic
 uint32 head02 = BE_read_uint32(ifile); // ???
 uint32 head03 = BE_read_uint32(ifile); // ???
 uint32 head04 = BE_read_uint32(ifile); // 0x0
 uint32 head05 = BE_read_uint32(ifile); // filesize
 uint32 head06 = BE_read_uint32(ifile); // number of root sections
 uint32 head07 = BE_read_uint32(ifile); // 0x0
 uint32 head08 = BE_read_uint32(ifile); // 0x0

 // read root section information
 struct ROOTSECTION { uint32 v1, v2; };
 deque<ROOTSECTION> rsdata;
 for(uint32 i = 0; i < head06; i++) {
     ROOTSECTION rs;
     rs.v1 = BE_read_uint32(ifile);
     rs.v2 = BE_read_uint32(ifile);
     if(debug) dfile << "section[" << i << "] = (" << rs.v1 << ", " << rs.v2 << ")" << endl;
     rsdata.push_back(rs);
    }
 if(debug) dfile << endl;

 // process root section offsets
 for(size_t i = 0; i < rsdata.size(); i++)
    {
     // move to section
     ifile.seekg(rsdata[i].v2);
     if(ifile.fail()) return error("Seek failure.");

     // read signature
     uint32 signature = BE_read_uint32(ifile);
     if(!processXXX(rsdata[i].v2, 0, 0)) return false;
    }

 // create simple model container
 SIMPLEMODELCONTAINER1 smc;
 smc.vbuffer.flags = 0;
 smc.vbuffer.elem = 0;
 smc.vbuffer.name = "";

 // assign points
 if(points.size()) {
    smc.vbuffer.flags |= VERTEX_POSITION;
    smc.vbuffer.elem = points.size();
    smc.vbuffer.name = "model"; // reassign
    smc.vbuffer.data.reset(new SMC_VERTEX[smc.vbuffer.elem]);
    for(uint32 i = 0; i < smc.vbuffer.elem; i++) {
        smc.vbuffer.data[i].vx = points[i].x;
        smc.vbuffer.data[i].vy = points[i].y;
        smc.vbuffer.data[i].vz = points[i].z;
       }
   }

 // save LWO
 return SaveLWO(savepath.c_str(), shrtname.c_str(), smc);
}

bool extractor::clearISM(void)
{
 // clear model
 points.clear();
 normal.clear();
 uvdata.clear();

 // clear ISM data
 texture_info.clear();
 string_table.clear();

 // clear file handles
 ifile.close();
 dfile.close();

 return true;
}

bool extractor::processXXX(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read signature
 uint32 type = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // process signature
 switch(type) {
   case(0x03) : return processX03(offset, parent, caller);
   case(0x04) : return processX04(offset, parent, caller);
   case(0x05) : return processX05(offset, parent, caller);
   case(0x0A) : return processX0A(offset, parent, caller);
   case(0x0B) : return processX0B(offset, parent, caller);
   case(0x0D) : return processX0D(offset, parent, caller);
   case(0x0F) : return processX0F(offset, parent, caller);
   case(0x21) : return processX21(offset, parent, caller);
   case(0x2D) : return processX2D(offset, parent, caller);
   case(0x2E) : return processX2E(offset, parent, caller);
   case(0x30) : return processX30(offset, parent, caller);
   case(0x31) : return processX31(offset, parent, caller);
   case(0x32) : return processX32(offset, parent, caller);
   case(0x34) : return processX34(offset, parent, caller);
   case(0x44) : return processX44(offset, parent, caller);
   case(0x45) : return processX45(offset, parent, caller);
   case(0x46) : return processX46(offset, parent, caller);
   case(0x4B) : return processX4B(offset, parent, caller);
   case(0x4C) : return processX4C(offset, parent, caller);
   case(0x4D) : return processX4D(offset, parent, caller);
   case(0x50) : return processX50(offset, parent, caller);
   case(0x59) : return processX59(offset, parent, caller);
   case(0x5B) : return processX5B(offset, parent, caller);
   case(0x5C) : return processX5C(offset, parent, caller);
   case(0x61) : return processX61(offset, parent, caller);
   case(0x62) : return processX62(offset, parent, caller);
   case(0x63) : return processX63(offset, parent, caller);
   case(0x65) : return processX65(offset, parent, caller);
   case(0x6A) : return processX6A(offset, parent, caller);
   case(0x6B) : return processX6B(offset, parent, caller);
   case(0x6C) : return processX6C(offset, parent, caller);
   case(0x6E) : return processX6E(offset, parent, caller);
   default : {
        stringstream ss;
        ss << "Unknown section 0x" << hex << type << dec << "." << endl;
        return error(ss.str().c_str());
       }
  }

 return true;
}

bool extractor::processX03(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x03 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 5;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " string table index = " << header[4] << ", " << string_table[header[4]] << endl;
   }

 // validate
 if(header[0] != 0x03) return error("Expecting SECTION 0x03.");
 if(header[1] != 20) return error("Invalid SECTION 0x03 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x03)) return false;

 return true;
}

bool extractor::processX04(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x04 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 16;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " string table index = " << header[4] << ", " << string_table[header[4]] << endl;
    dfile << " unknown = " << header[ 5] << endl; // 0
    dfile << " unknown = " << header[ 6] << endl; // 0
    dfile << " unknown = " << header[ 7] << endl; // 0
    dfile << " unknown = " << header[ 8] << endl; // 0
    dfile << " unknown = " << header[ 9] << endl; // 1
    dfile << " unknown = " << header[10] << endl; // 1
    dfile << " unknown = " << header[11] << endl; // 0xFFFFFFFF
    dfile << " unknown = " << header[12] << endl; // 0
    dfile << " unknown = " << header[13] << endl; // 0 or 142
    dfile << " unknown = " << header[14] << endl; // 0
    dfile << " unknown = " << header[15] << endl; // 0
   }

 // validate
 if(header[0] != 0x04) return error("Expecting SECTION 0x04.");
 if(header[1] != 64) return error("Invalid SECTION 0x04 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x04)) return false;

 return true;
}

bool extractor::processX05(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x05 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 16;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " string table index = " << header[4] << ", " << string_table[header[4]] << endl;
    dfile << " string table index = " << header[5] << ", " << string_table[header[5]] << endl;
    dfile << " unknown = " << header[6] << endl; // 0
    dfile << " parent section 0x05 offset = 0x" << hex << header[7] << dec << endl;
    dfile << " number of children = " << header[8] << endl;
    dfile << " unknown = " << header[ 9] << endl; // 1
    dfile << " unknown = " << header[10] << endl; // 0
    dfile << " bone index = " << header[11] << endl;
    dfile << " unknown = " << header[12] << endl; // 0
    dfile << " index into section 0x03 offset array = " << header[13] << endl;
    dfile << " unknown = " << header[14] << endl; // 0
    dfile << " unknown = " << header[15] << endl; // 0
   }

 // validate
 if(header[0] != 0x05) return error("Expecting SECTION 0x05.");
 if(header[1] != 64) return error("Invalid SECTION 0x05 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x05)) return false;

 return true;
}

bool extractor::processX0A(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x0A at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 8;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " string table index = " << header[4] << ", " << string_table[header[4]] << endl;
    dfile << " unknown = " << header[5] << endl;
    dfile << " unknown = " << header[6] << endl;
    dfile << " unknown = " << header[7] << endl;
   }

 // validate
 if(header[0] != 0x0A) return error("Expecting SECTION 0x0A.");
 if(header[1] != 32) return error("Invalid SECTION 0x0A size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x0A)) return false;

 return true;
}

bool extractor::processX0B(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x0B at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 3;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
   }

 // validate
 if(header[0] != 0x0B) return error("Expecting SECTION 0x0B.");
 if(header[1] != 12) return error("Invalid SECTION 0x0B size of header.");

 if(header[2] == 0) return error("Section 0x0B must have at least one child.");
 if(header[2] > 1) return error("Section 0x0B has multiple children.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x0B)) return false;

 return true;
}

bool extractor::processX0D(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x0D at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 7;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " string table index = " << header[4] << ", " << string_table[header[4]] << endl;
    dfile << " string table index = " << header[5] << ", " << string_table[header[5]] << endl;
    dfile << " string table index = " << header[6] << ", " << string_table[header[6]] << endl;
   }

 // validate
 if(header[0] != 0x0D) return error("Expecting SECTION 0x0D.");
 if(header[1] != 28) return error("Invalid SECTION 0x0D size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x0D)) return false;

 return true;
}

bool extractor::processX0F(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x0F at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 return true;
}

bool extractor::processX21(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x21 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 3;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
   }

 // validate
 if(header[0] != 33) return error("Expecting SECTION 0x21.");
 if(header[1] != 12) return error("Invalid SECTION 0x21 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
    {
     // move to table data
     ifile.seekg(offsets[i], ios::beg);
     if(ifile.fail()) return error("Seek failure.");

     // read string
     char item[1024];
     read_string(ifile, &item[0], 1024);

     // add string
     string_table.push_back(item);
     if(debug) dfile << " string_table[" << i << "] = " << string_table[i] << endl;
    }

 return true;
}

bool extractor::processX2D(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x2D at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 8;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " string table index = " << header[1] << ", " << string_table[header[1]] << endl;
    dfile << " string table index = " << header[2] << ", " << string_table[header[2]] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " string table index = " << header[4] << ", " << string_table[header[4]] << endl;
    dfile << " string table index = " << header[5] << ", " << string_table[header[5]] << endl;
    dfile << " string table index = " << header[6] << ", " << string_table[header[6]] << endl;
    dfile << " string table index = " << header[7] << ", " << string_table[header[7]] << endl;
   }

 // save data
 TEXTUREINFO info;
 info.data1 = header[0];
 info.data2 = header[1];
 info.data3 = header[2];
 info.data4 = header[3];
 info.data5 = header[4];
 info.data6 = header[5];
 info.data7 = header[6];
 info.data8 = header[7];
 texture_info.push_back(info);

 return true;
}

bool extractor::processX2E(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x2E at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 3;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
   }

 // validate
 if(header[0] != 0x2E) return error("Expecting SECTION 0x2E.");
 if(header[1] != 12) return error("Invalid SECTION 0x2E size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x2E)) return false;

 return true;
}

bool extractor::processX30(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x30 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 21;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " unknown = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " unknown = " << header[4] << ", " << string_table[header[4]] << endl;
    dfile << " matrix[0x0] = " << interpret_as_real32(header[5]) << endl;
    dfile << " matrix[0x1] = " << interpret_as_real32(header[6]) << endl;
    dfile << " matrix[0x2] = " << interpret_as_real32(header[7]) << endl;
    dfile << " matrix[0x3] = " << interpret_as_real32(header[8]) << endl;
    dfile << " matrix[0x4] = " << interpret_as_real32(header[9]) << endl;
    dfile << " matrix[0x5] = " << interpret_as_real32(header[10]) << endl;
    dfile << " matrix[0x6] = " << interpret_as_real32(header[11]) << endl;
    dfile << " matrix[0x7] = " << interpret_as_real32(header[12]) << endl;
    dfile << " matrix[0x8] = " << interpret_as_real32(header[13]) << endl;
    dfile << " matrix[0x9] = " << interpret_as_real32(header[14]) << endl;
    dfile << " matrix[0xA] = " << interpret_as_real32(header[15]) << endl;
    dfile << " matrix[0xB] = " << interpret_as_real32(header[16]) << endl;
    dfile << " matrix[0xC] = " << interpret_as_real32(header[17]) << endl;
    dfile << " matrix[0xD] = " << interpret_as_real32(header[18]) << endl;
    dfile << " matrix[0xE] = " << interpret_as_real32(header[19]) << endl;
    dfile << " matrix[0xF] = " << interpret_as_real32(header[20]) << endl;
   }

 // validate
 if(header[0] != 0x30) return error("Expecting SECTION 0x30.");
 if(header[1] != 84) return error("Invalid SECTION 0x30 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x30)) return false;

 return true;
}

bool extractor::processX31(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x31 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 5;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " string table index = " << header[4] << ", " << string_table[header[4]] << endl;
   }

 // validate
 if(header[0] != 0x31) return error("Expecting SECTION 0x31.");
 if(header[1] != 20) return error("Invalid SECTION 0x31 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x31)) return false;

 return true;
}

bool extractor::processX32(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x32 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 5;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " unknown = " << header[3] << endl; // 0
    dfile << " unknown = " << header[4] << endl; // 0
   }

 // validate
 if(header[0] != 0x32) return error("Expecting SECTION 0x32.");
 if(header[1] != 20) return error("Invalid SECTION 0x32 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x32)) return false;

 return true;
}

bool extractor::processX34(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x34 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 if(debug) dfile << endl;
 return true;
}

bool extractor::processX44(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x44 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 8;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of items = " << header[2] << endl;
    dfile << " unknown = " << header[3] << endl; // 0x0
    dfile << " unknown = " << header[4] << endl; // type of data 5 (shorts?) or 18 (floats?), 12 (matrix?)
    dfile << " unknown = " << header[5] << endl; // group size (typically 1, 5, 13, or 16)
    dfile << " unknown = " << header[6] << endl; // 0x0
    dfile << " unknown = " << header[7] << endl; // 0x0
   }

 // validate
 if(header[0] != 0x44) return error("Expecting SECTION 0x44.");
 if(header[1] != 32) return error("Invalid SECTION 0x44 size of header.");

 // TODO: read data
 // could be matrix, single values

 // read bone string table data
 if(header[4] == 5 && header[5] == 1)
   {
    for(size_t i = 0; i < header[2]; i++) {
        uint16 index = BE_read_uint16(ifile);
        if(debug) dfile << " " << index << ", " << string_table[index] << endl;
       }
   }
 // read bone matrices
 else if(header[4] == 12 && header[5] == 16)
   {
    for(size_t i = 0; i < header[2]/16; i++) {
        boost::shared_array<float> matrix(new float[16]);
        BE_read_array(ifile, matrix.get(), 16);
        if(debug) dfile << " ";
        if(debug) for(size_t j = 0; j < 16; j++) dfile << matrix[j] << "  ";
        if(debug) dfile << endl;
       }
   }
 // ???
 else
    return error("SOME OTHER TYPE OF SKINNING OPTIONS.");

 return true;
}

bool extractor::processX45(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x45 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 5;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of indices = " << header[2] << endl;
    dfile << " unknown = " << header[3] << endl; // 5, 7
    dfile << " unknown = " << header[4] << endl; // 0
   }

 // validate
 if(header[0] != 0x45) return error("Expecting SECTION 0x45.");
 if(header[1] != 20) return error("Invalid SECTION 0x45 size of header.");

 // indices must be divisible by 3
 if(header[2] % 3) return error("The number of face indices must be divisible by three.");
 size_t n_triangles = header[2]/3; 

 // read triangles
 if(debug) dfile << "DATA" << endl;
 if(header[3] == 5) {
    for(size_t i = 0; i < n_triangles; i++) {
        uint16 a = BE_read_uint16(ifile);
        uint16 b = BE_read_uint16(ifile);
        uint16 c = BE_read_uint16(ifile);
        if(dfile) dfile << "face[" << i << "] = (" << a << ", " << b << ", " << c << ")" << endl;
       }
   }
 else if(header[3] == 7) {
    for(size_t i = 0; i < n_triangles; i++) {
        uint16 a = BE_read_uint32(ifile);
        uint16 b = BE_read_uint32(ifile);
        uint16 c = BE_read_uint32(ifile);
        if(dfile) dfile << "face[" << i << "] = (" << a << ", " << b << ", " << c << ")" << endl;
       }
   }
 else
    return error("Unknown index buffer format.");

 return true;
}

bool extractor::processX46(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x46 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 7;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " unknown = " << header[3] << endl; // 44, 54, 74 string table?
    dfile << " unknown = " << header[4] << endl; // 0?
    dfile << " unknown = " << header[5] << endl; // 4?
    dfile << " number of triangles = " << header[6] << endl;
   }

 // validate
 if(header[0] != 0x46) return error("Expecting SECTION 0x46.");
 if(header[1] != 28) return error("Invalid SECTION 0x46 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x46)) return false;

 return true;
}

bool extractor::processX4B(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x4B at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 6;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " string table index = " << header[4] << ", " << string_table[header[4]] << endl;
    dfile << " unknown = " << header[5] << endl;
   }

 // validate
 if(header[0] != 0x4B) return error("Expecting SECTION 0x4B.");
 if(header[1] != 24) return error("Invalid SECTION 0x4B size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
    {
     // move to offset
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     static const uint32 elem = 7;
     uint32 data[elem];
     BE_read_array(ifile, &data[0], elem);

     // debug information
     if(debug) {
        dfile << "DATA" << endl;
        dfile << " unknown = " << data[0] << endl; // 0x0
        dfile << " unknown = " << data[1] << endl; // 0xC
        dfile << " unknown = " << data[2] << endl; // 0x1
        dfile << " string table index = " << data[3] << ", " << string_table[data[3]] << endl; // TEX0
        dfile << " string table index = " << data[4] << ", " << string_table[data[4]] << endl; // TEXCOORD
        dfile << " unknown = " << data[5] << endl; // 0x0
        dfile << " unknown = " << data[6] << endl; // 0x0
       }
    }

 return true;
}

bool extractor::processX4C(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x4C at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 6;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " unknown = " << header[4] << endl;
    dfile << " string table index = " << header[5] << ", " << string_table[header[5]] << endl;
   }

 // validate
 if(header[0] != 0x4C) return error("Expecting SECTION 0x4C.");
 if(header[1] != 24) return error("Invalid SECTION 0x4C size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x4C)) return false;

 return true;
}

bool extractor::processX4D(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x4D at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 5;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " unknown = " << header[3] << endl;
    dfile << " unknown = " << header[4] << endl;
   }

 // validate
 if(header[0] != 0x4D) return error("Expecting SECTION 0x4D.");
 if(header[1] != 20) return error("Invalid SECTION 0x4D size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x4D)) return false;

 return true;
}

bool extractor::processX50(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x50 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 cout << "PROCESS #0x50" << endl;

 // read header
 static const uint32 header_elem = 8;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " unknown = " << header[4] << endl;
    dfile << " unknown = " << header[5] << endl;
    dfile << " unknown = " << header[6] << endl;
    dfile << " unknown = " << header[7] << endl;
   }

 // validate
 if(header[0] != 0x50) return error("Expecting SECTION 0x50.");
 if(header[1] != 32) return error("Invalid SECTION 0x50 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x50)) return false;

 return true;
}

bool extractor::processX59(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x59 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 7;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " type = " << header[3] << endl;
    dfile << " number of vertices = " << header[4] << endl;
    dfile << " vertex bytes = " << header[5] << endl;
    dfile << " unknown = " << header[6] << endl; // 0x0
   }

 // validate
 if(header[0] != 0x59) return error("Expecting SECTION 0x59.");
 if(header[1] != 28) return error("Invalid SECTION 0x59 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 struct VERTEXINFO {
  uint32 type;
  uint32 elem;
  uint32 vertex_type;
  uint32 vertex_size;
  uint32 item_offset;
  uint32 file_offset;
 };

 // data
 deque<VERTEXINFO> vinfo;
 bool has_points = false;
 bool has_normal = false;
 uint32 start = 0;

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
    {
     // move to offset
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read vertex information
     static const uint32 elem = 6;
     uint32 data[elem];
     BE_read_array(ifile, &data[0], elem);

     // save vertex information
     VERTEXINFO vi;
     vi.type = data[0];
     vi.elem = data[1];
     vi.vertex_type = data[2];
     vi.vertex_size = data[3];
     vi.item_offset = data[4];
     vi.file_offset = data[5];
     vinfo.push_back(vi);

     // test
     if(vi.type == 0) has_points = true;
     if(vi.type == 2) has_normal = true;

     // debug information
     if(debug) {
        dfile << "DATA" << endl;
        dfile << " type = " << data[0] << endl;
        dfile << " elem = " << data[1] << endl;
        dfile << " vertex_type = " << data[2] << endl; // 12 = float, 18 = half float
        dfile << " vertex_size = " << data[3] << endl;
        dfile << " item offset = " << data[4] << endl;
        dfile << " file offset = " << hex << data[5] << dec << endl;
       }

     // save start of vertex data
     if(i == 0) start = data[5];
     else if(start != data[5]) return error("Vertex data offsets do not match.");
    }

 // move to vertex data
 ifile.seekg(start);
 if(ifile.fail()) return error("Seek failure.");

 // VERTEX TYPE: 1
 // POSITION
 if(header[3] == 1)
   {
    // create position data
    uint32 n_vertices = header[4];
    uint32 vertexsize = header[5];
    if(has_points) points.resize(n_vertices);
    if(has_normal) normal.resize(n_vertices);

    // compute total number of bytes to read
    uint32 total_bytes = n_vertices*vertexsize;
    if(!total_bytes) return error("No vertex data to read.");

    // read vertex data
    boost::shared_array<char> data(new char[total_bytes]);
    ifile.read(data.get(), total_bytes);
    if(ifile.fail()) return error("Read failure.");

    // read points
    binary_stream bs(data, total_bytes);
    for(size_t i = 0; i < n_vertices; i++)
       {
        // for each vertex component
        for(size_t j = 0; j < vinfo.size(); j++)
           {
            // move to vertex component
            bs.seek(i*vinfo[j].vertex_size + vinfo[j].item_offset);

            // position
            if(vinfo[j].type == 0) {
               real32 x = bs.BE_read_real32();
               real32 y = bs.BE_read_real32();
               real32 z = bs.BE_read_real32();
               points[i].x = x;
               points[i].y = y;
               points[i].z = z;
               if(debug) dfile << " position[" << i << "] = (" << x << ", " << y << ", " << z << ") ";
              }
            else if(vinfo[j].type == 2) {
               real32 x = bs.BE_read_real16();
               real32 y = bs.BE_read_real16();
               real32 z = bs.BE_read_real16();
               normal[i].x = x;
               normal[i].y = y;
               normal[i].z = z;
               if(debug) dfile << " normal[" << i << "] = (" << x << ", " << y << ", " << z << ") ";
              }
            else if(vinfo[j].type == 3) {
               real32 x = bs.BE_read_real16();
               real32 y = bs.BE_read_real16();
               real32 z = bs.BE_read_real16();
               real32 w = bs.BE_read_real16();
               if(debug) dfile << " type03[" << i << "] = (" << x << ", " << y << ", " << z << ", " << w << ") ";
              }
            else if(vinfo[j].type == 14) {
               real32 x = bs.BE_read_real16();
               real32 y = bs.BE_read_real16();
               real32 z = bs.BE_read_real16();
               if(debug) dfile << " type14[" << i << "] = (" << x << ", " << y << ", " << z << ") ";
              }
            else if(vinfo[j].type == 15) {
               real32 x = bs.BE_read_real16();
               real32 y = bs.BE_read_real16();
               real32 z = bs.BE_read_real16();
               if(debug) dfile << " type15[" << i << "] = (" << x << ", " << y << ", " << z << ") ";
              }
           }
        if(debug) dfile << endl;
       }
   }
 // VERTEX TYPE: 2
 // UV
 else if(header[3] == 2)
   {
    // read UV map data
    if(debug) dfile << "UV DATA" << endl;
    for(size_t i = 0; i < header[4]; i++)
       {
        // read point
        real32 x = BE_read_real16(ifile);
        real32 y = BE_read_real16(ifile);
        y = 1.0f - y;
        if(debug) dfile << " uv[" << i << "] = (" << x << ", " << y << ")" << endl;
       }
   }
 // VERTEX TYPE: 3
 // WEIGHTS
 else if(header[3] == 3)
   {
   }
 else
   {
    return error("Unknown vertex type.");
   }

 return true;
}

bool extractor::processX5B(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x5B at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 3;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
   }

 // validate
 if(header[0] != 0x5B) return error("Expecting SECTION 0x5B.");
 if(header[1] != 12) return error("Invalid SECTION 0x5B size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
    {
     // move to offset
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     static const uint32 elem = 2;
     uint32 data[elem];
     BE_read_array(ifile, &data[0], elem);

     // debug information
     if(debug) {
        dfile << "DATA" << endl;
        dfile << " data type = " << data[0] << endl;
        dfile << " string table index = " << data[1] << ", " << string_table[data[1]] << endl;
       }

     // Translation
     // 12 bytes
     if(data[0] == 20)
       {
        real32 x = BE_read_real32(ifile);
        real32 y = BE_read_real32(ifile);
        real32 z = BE_read_real32(ifile);
        if(debug) {
           dfile << " x = " << x << endl;
           dfile << " y = " << y << endl;
           dfile << " z = " << z << endl;
          }
       }
     // Scale
     // 20 or 24 bytes?
     else if(data[0] == 21)
       {
        real32 x = BE_read_real32(ifile);
        real32 y = BE_read_real32(ifile);
        real32 z = BE_read_real32(ifile);
        if(debug) {
           dfile << " x = " << x << endl;
           dfile << " y = " << y << endl;
           dfile << " z = " << z << endl;
          }
       }
     // rotateX, rotateY, rotateZ
     // 24 bytes
     else if(data[0] >= 93 && data[0] <= 95)
       {
        real32 a = BE_read_real32(ifile);
        real32 b = BE_read_real32(ifile);
        real32 c = BE_read_real32(ifile);
        real32 d = BE_read_real32(ifile);
        if(debug) {
           dfile << " a = " << a << endl;
           dfile << " b = " << b << endl;
           dfile << " c = " << c << endl;
           dfile << " d = " << d << endl;
          }
       }
     // jointOrientX
     // 24 bytes
     else if(data[0] == 103)
       {
        real32 a = BE_read_real32(ifile);
        real32 b = BE_read_real32(ifile);
        real32 c = BE_read_real32(ifile);
        real32 d = BE_read_real32(ifile);
        if(debug) {
           dfile << " a = " << a << endl;
           dfile << " b = " << b << endl;
           dfile << " c = " << c << endl;
           dfile << " d = " << d << endl;
          }
       }
     // CollisionFlag
     // 20 bytes
     else if(data[0] == 112)
       {
       }
     // CollisionRadius
     // 20 bytes
     else if(data[0] == 113)
       {
       }
     // PhysicsFlag
     // 20 bytes
     else if(data[0] == 114)
       {
       }
     // PhysicsRadius
     // 20 bytes
     else if(data[0] == 115)
       {
       }
     // PhysicsCost
     // 20 bytes
     else if(data[0] == 116)
       {
       }
     // PhysicsMass
     // 20 bytes
     else if(data[0] == 117)
       {
       }
     // PhysicsExpand
     // 20 bytes
     else if(data[0] == 118)
       {
       }
     // PhysicsShapeMemory
     // 20 bytes
     else if(data[0] == 119)
       {
       }
     // ???
     else if(data[0] >= 122 && data[0] <= 126)
       {
       }
     // ???
     else
       {
       }
    }

 return true;
}

bool extractor::processX5C(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x5C at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 3;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
   }

 // validate
 if(header[0] != 0x5C) return error("Expecting SECTION 0x5C.");
 if(header[1] != 12) return error("Invalid SECTION 0x5C size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // TODO: offsets refer to bone offsets in SECTION 0x05
 // it is safe to link child bones here

 return true;
}

bool extractor::processX61(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x61 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 3;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
   }

 // validate
 if(header[0] != 97) return error("Expecting SECTION 0x61.");
 if(header[1] != 12) return error("Invalid SECTION 0x61 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x61)) return false;

 return true;
}

bool extractor::processX62(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x62 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 3;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
   }

 // validate
 if(header[0] != 0x62) return error("Expecting SECTION 0x62.");
 if(header[1] != 12) return error("Invalid SECTION 0x62 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x62)) return false;

 return true;
}

bool extractor::processX63(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x63 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 11;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " unknown = " << header[3] << endl;
    dfile << " unknown = " << header[4] << endl;
    dfile << " unknown = " << header[5] << endl;
    dfile << " unknown = " << header[6] << endl;
    dfile << " unknown = " << header[7] << endl;
    dfile << " unknown = " << header[8] << endl;
    dfile << " unknown = " << header[9] << endl;
    dfile << " unknown = " << header[10] << endl;
   }

 // validate
 if(header[0] != 0x63) return error("Expecting SECTION 0x63.");
 if(header[1] != 44) return error("Invalid SECTION 0x63 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x63)) return false;

 return true;
}

bool extractor::processX65(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x65 at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 4;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " unknown = " << header[3] << endl;
   }

 // validate
 if(header[0] != 0x65) return error("Expecting SECTION 0x65.");
 if(header[1] != 16) return error("Invalid SECTION 0x65 size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
    {
     // move to offset
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     static const uint32 elem = 4;
     uint32 data[elem];
     BE_read_array(ifile, &data[0], elem);

     // debug information
     if(debug) {
        dfile << "DATA" << endl;
        dfile << " index = " << data[0] << endl; // 1, 2, 3, ...
        dfile << " unknown = " << data[1] << endl; // 1, 2
        dfile << " number of items = " << data[2] << endl; // 1 (20 bytes) or 4 (32 bytes)
        dfile << " item type = " << data[3] << endl; // 0 or some integer
       }

     // read data
     for(size_t i = 0; i < data[2]; i++) {
         uint32 item = BE_read_uint32(ifile);
         if(debug) {
            if(data[3] == 0) dfile << " item = " << interpret_as_real32(item) << endl;
            else dfile << " item = " << item << endl;
           }
        }
    }

 return true;
}

bool extractor::processX6A(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x6A at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 5;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " string table index = " << header[3] << ", " << string_table[header[3]] << endl;
    dfile << " string table index = " << header[4] << ", " << string_table[header[4]] << endl;
   }

 // validate
 if(header[0] != 0x6A) return error("Expecting SECTION 0x6A.");
 if(header[1] != 20) return error("Invalid SECTION 0x6A size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x6A)) return false;

 return true;
}

bool extractor::processX6B(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x6B at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 6;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
    dfile << " type = " << header[3] << endl;
    dfile << " elem = " << header[4] << endl;
    dfile << " 0x6A offset = 0x" << hex << header[5] << dec << endl;
   }

 // validate
 if(header[0] != 0x6B) return error("Expecting SECTION 0x6B.");
 if(header[1] != 24) return error("Invalid SECTION 0x6B size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 if(header[3] == 0x02 || header[3] == 0x06) {
    for(size_t i = 0; i < offsets.size(); i++)
        if(!processXXX(offsets[i], offset, 0x6B)) return false;
   }
 // process data
 else if(header[3] == 0x15)
   {
    if(offsets.size() != 1) return error("Invalid section 0x6B data.");
    ifile.seekg(offsets[0]);
    if(ifile.fail()) return error("Seek failure.");
    uint32 index = BE_read_uint32(ifile);
    string value = string_table[index];
    if(debug) {
       dfile << "DATA" << endl;
       dfile << " surface = " << value << endl;
      }
   }
 // process data
 else if(header[3] == 0x18)
   {
    if(offsets.size() != 1) return error("Invalid section 0x6B data.");
    ifile.seekg(offsets[0]);
    if(ifile.fail()) return error("Seek failure.");
    uint32 index = BE_read_uint32(ifile);
    string value = string_table[index];
    if(debug) {
       dfile << "DATA" << endl;
       dfile << " image format = " << value << endl;
      }
   }
 // process data
 else
   {
    uint32 elem = header[4];
    boost::shared_array<uint32> data(new uint32[elem]);
    BE_read_array(ifile, data.get(), elem);
    if(debug) {
       dfile << "DATA" << endl;
       for(uint32 i = 0; i < elem; i++) dfile << " data[" << i << "] = " << data[i] << endl;
      }
   }

 return true;
}

bool extractor::processX6C(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x6C at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 3;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " number of children = " << header[2] << endl;
   }

 // validate
 if(header[0] != 0x6C) return error("Expecting SECTION 0x6C.");
 if(header[1] != 12) return error("Invalid SECTION 0x6C size of header.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 temp = BE_read_uint32(ifile);
     offsets.push_back(temp);
     if(debug) dfile << " offsets[" << i << "] = 0x" << hex << offsets[i] << dec << endl;
    }

 // process data at offsets
 for(size_t i = 0; i < offsets.size(); i++)
     if(!processXXX(offsets[i], offset, 0x6C)) return false;

 return true;
}

bool extractor::processX6E(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 using namespace std;
 if(debug) dfile << endl;
 if(debug) dfile << "LOADING SECTION 0x6E at 0x" << hex << offset << " from 0x" << caller << dec << endl;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read header
 static const uint32 header_elem = 12;
 uint32 header[header_elem];
 BE_read_array(ifile, &header[0], header_elem);

 // debug information
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << " headtype = " << header[0] << endl;
    dfile << " headsize = " << header[1] << endl;
    dfile << " unknown = " << header[2] << endl; // 0
    dfile << " unknown = " << header[3] << endl; // 0
    dfile << " min_x = " << interpret_as_real32(header[ 4]) << endl;
    dfile << " min_y = " << interpret_as_real32(header[ 5]) << endl;
    dfile << " min_z = " << interpret_as_real32(header[ 6]) << endl;
    dfile << " max_w = " << interpret_as_real32(header[ 7]) << endl;
    dfile << " max_x = " << interpret_as_real32(header[ 8]) << endl;
    dfile << " max_y = " << interpret_as_real32(header[ 9]) << endl;
    dfile << " max_z = " << interpret_as_real32(header[10]) << endl;
    dfile << " max_w = " << interpret_as_real32(header[11]) << endl;
   }

 // validate
 if(header[0] != 0x6E) return error("Expecting SECTION 0x6E.");
 if(header[1] != 48) return error("Invalid SECTION 0x6E size of header.");

 return true;
}

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