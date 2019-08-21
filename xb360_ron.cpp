#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_amc.h"
#include "x_dds.h"
#include "xb360_ron.h"

using namespace std;

#define X_SYSTEM XBOX360
#define X_GAME   RON

//
// ARCHIVES
//
namespace X_SYSTEM { namespace X_GAME {

bool processFLP(const string& filename)
{
 // important strings
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // create save path
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read header
 uint32 h01 = LE_read_uint32(ifile); // magic
 uint32 h02 = LE_read_uint32(ifile); // size of data section
 uint32 h03 = LE_read_uint32(ifile); // 0x20
 uint32 h04 = LE_read_uint32(ifile); // 0x10000000
 uint32 h05 = LE_read_uint32(ifile); // 0x00
 uint32 h06 = LE_read_uint32(ifile); // size of data section
 uint32 h07 = LE_read_uint32(ifile); // 0x00
 uint32 h08 = LE_read_uint32(ifile); // 0x00
 uint32 h09 = BE_read_uint32(ifile); // number of files (big endian!)
 uint32 h10 = LE_read_uint32(ifile); // 0 or 1
 uint32 h11 = LE_read_uint32(ifile); // 0
 uint32 h12 = LE_read_uint32(ifile); // 0

 // file information
 struct fileinfo {
  string filename;
  size_t length;
  size_t offset;
 };

 // process file information
 deque<fileinfo> filelist;
 for(size_t i = 0; i < h09; i++)
    {
     // read filename
     char buffer[36];
     ifile.read(&buffer[0], 36);
     uint32 p1 = BE_read_uint32(ifile);
     uint32 p2 = BE_read_uint32(ifile);
     uint32 p3 = BE_read_uint32(ifile);
     
     // save file information
     fileinfo info;
     info.filename = buffer;
     info.length = p1;
     info.offset = p2;
     filelist.push_back(info);
    }

 // process files
 for(size_t i = 0; i < filelist.size(); i++)
    {
     // create file
     string path(savepath);
     path += filelist[i].filename;
     ofstream ofile(path.c_str(), ios::binary);
     if(!ofile) continue;

     // move to offset
     ifile.seekg(filelist[i].offset);
     if(ifile.fail()) return error("Seek failure.");

     // copy contents
     boost::shared_array<char> data(new char[filelist[i].length]);
     ifile.read(data.get(), filelist[i].length);
     ofile.write(data.get(), filelist[i].length);
    }

 return true;
}

}};

//
// TEXTURES
//
namespace X_SYSTEM { namespace X_GAME {

bool processHTX(const string& filename)
{
 // important strings
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read HTEX header
 uint32 h01 = LE_read_uint32(ifile); // magic
 uint32 h02 = LE_read_uint32(ifile); // filesize - EOFC data
 uint32 h03 = LE_read_uint32(ifile); // 0x20
 uint32 h04 = LE_read_uint32(ifile); // 0x1000000D
 uint32 h05 = LE_read_uint32(ifile); // 0x00
 uint32 h06 = LE_read_uint32(ifile); // 0x00
 uint32 h07 = LE_read_uint32(ifile); // 0x00
 uint32 h08 = LE_read_uint32(ifile); // 0x00

 // validate HTEX header
 if(h01 != 0x58455448)
    return error("Invalid HTX file.");

 // read textures
 for(;;)
    {
     // read HTSF magic
     unsigned int magic = LE_read_uint32(ifile);
     if(magic == 0x43464F45) break; // EOFC
     if(magic != 0x46535448) return error("Expecting HTEX:HTSF section.");

     // read HTSF header
     uint32 HTSF_02 = LE_read_uint32(ifile); // filesize - 0x20
     uint32 HTSF_03 = LE_read_uint32(ifile); // 0x20
     uint32 HTSF_04 = LE_read_uint32(ifile); // 0x1000000D
     uint32 HTSF_05 = LE_read_uint32(ifile); // 1
     uint32 HTSF_06 = LE_read_uint32(ifile); // filesize - 0x20
     uint32 HTSF_07 = LE_read_uint32(ifile); // 0x00
     uint32 HTSF_08 = LE_read_uint32(ifile); // 0x00
     uint32 HTSF_09 = LE_read_uint32(ifile); // 0x02
     uint32 HTSF_10 = LE_read_uint32(ifile); // 0x00
     uint32 HTSF_11 = LE_read_uint32(ifile); // 0x00
     uint32 HTSF_12 = LE_read_uint32(ifile); // 0x00
     uint32 HTSF_13 = LE_read_uint32(ifile); // 0x00
     uint32 HTSF_14 = LE_read_uint32(ifile); // 0x00
     uint32 HTSF_15 = LE_read_uint32(ifile); // 0x00
     uint32 HTSF_16 = LE_read_uint32(ifile); // 0x00

     // read DDS
     uint32 datasize = HTSF_02 - 0x60;
     boost::shared_array<char> data(new char[datasize]);
     ifile.read((char*)data.get(), datasize);

     // read filename
     char buffer[32];
     ifile.read(&buffer[0], 32);

     // read padding
     char padding[32];
     ifile.read(&padding[0], 32);

     // create DDS file
     string ofilename(pathname);
     ofilename += buffer;

     // write DDS
     ofstream ofile(ofilename.c_str(), ios::binary);
     ofile.write((char*)data.get(), datasize);
    }

 return true;
}

}};

//
// MODELS
//
namespace X_SYSTEM { namespace X_GAME {

class HMDExtractor {
 private :
  struct HMDLHEADER {
   uint32 param01; // magic
   uint32 param02; // bytes of HMDL data
   uint32 param03; // start of GSGM data
   uint32 param04; // 0x1000000D
   uint32 param05; // 0x00
   uint32 param06; // bytes of GSGM data
   uint32 param07; // 0x00
   uint32 param08; // 0x00
  };
  struct GSGMHEADER {
   uint32 param01; // magic
   uint32 param02; // 0x102 (always)
   uint16 param03; // 0x200 (always)
   uint16 param04; // 0x800 (always)
   uint32 param05; // size of data in BE
   uint16 param06; // unknown (changes)
  };
  struct SECTION_ITEM_00 {
   uint32 p01;
   uint32 p02;
   uint32 p03;
   uint32 p04;
   uint32 p05;
   uint32 p06;
   uint32 p07;
   uint32 p08;
   real32 p09[0x14];
   uint32 p10[0x04];
  };
 private :
  string filename;
  string pathname;
  string shrtname;
 private :
  ifstream ifile;
  ofstream dfile;
  bool debug;
 private :
  ADVANCEDMODELCONTAINER amc;
  HMDLHEADER hmdl;
  GSGMHEADER gsgm;
  uint32 offset[18];
  uint32 inform[18];
  deque<boost::shared_array<char>> section00;
  deque<boost::shared_array<char>> section01;
  deque<boost::shared_array<char>> section02;
  deque<boost::shared_array<char>> section03;
  deque<boost::shared_array<char>> section04;
  deque<boost::shared_array<char>> section05;
  deque<boost::shared_array<char>> section06;
  boost::shared_array<char> section07;
  deque<boost::shared_array<char>> section08;
  deque<boost::shared_array<char>> section09;
  deque<boost::shared_array<char>> section10;
  boost::shared_array<char> section11;
  boost::shared_array<char> section12;
  boost::shared_array<char> section13;
 private :
  bool processHeaders(void); // read headers, offset table
  bool processSection00(void); // read skeleton data
  bool processSection01(void); // read skeleton data
  bool processSection02(void); // read mesh information
  bool processSection03(void); // read surface information
  bool processSection04(void); // read unknown
  bool processSection05(void); // read bind pose matrices
  bool processSection06(void); // read vertex information
  bool processSection07(void); // read vertex buffer
  bool processSection08(void); // read texture association
  bool processSection09(void); // read unknown names
  bool processSection10(void); // read bone names
  bool processSection11(void); // read index buffer
  bool processSection12(void); // read unknown
  bool processSection13(void); // read joint maps
  bool processModel(void);
  bool processBones(void);
 public :
  bool extract(void);
 public :
  HMDExtractor(const string& fn);
 ~HMDExtractor() {}
};

HMDExtractor::HMDExtractor(const string& fn) : debug(true)
{
 filename = fn;
 pathname = GetPathnameFromFilename(filename);
 shrtname = GetShortFilenameWithoutExtension(filename);
}

bool HMDExtractor::processHeaders(void)
{
 // open file
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // create debug file
 if(debug) {
    string dfname = pathname;
    dfname += shrtname;
    dfname += ".txt";
    dfile.open(dfname.c_str());
    if(!dfile.is_open()) return error("Failed to create debug file.");
   }

 // read HMDL header
 hmdl.param01 = LE_read_uint32(ifile);
 hmdl.param02 = LE_read_uint32(ifile);
 hmdl.param03 = LE_read_uint32(ifile);
 hmdl.param04 = LE_read_uint32(ifile);
 hmdl.param05 = LE_read_uint32(ifile);
 hmdl.param06 = LE_read_uint32(ifile);
 hmdl.param07 = LE_read_uint32(ifile);
 hmdl.param08 = LE_read_uint32(ifile);
 if(hmdl.param01 != 0x4C444D48) return error("Invalid HMDL section.");

 // save GSGM position
 uint32 start = (uint32)ifile.tellg();
 if(ifile.fail()) return error("Tell failure.");

 // read GSGM header
 gsgm.param01 = LE_read_uint32(ifile);
 gsgm.param02 = LE_read_uint32(ifile);
 gsgm.param03 = LE_read_uint16(ifile);
 gsgm.param04 = LE_read_uint16(ifile);
 gsgm.param05 = BE_read_uint32(ifile);
 gsgm.param06 = LE_read_uint16(ifile);
 if(gsgm.param01 != 0x4D475347) return error("Invalid GSGM section.");

 if(debug) {
    dfile << "GSGM HEADER" << endl;
    dfile << "head01 = " << gsgm.param01 << endl;
    dfile << "head02 = " << gsgm.param02 << endl;
    dfile << "head03 = " << gsgm.param03 << endl;
    dfile << "head04 = " << gsgm.param04 << endl;
    dfile << "head05 = " << gsgm.param05 << endl;
    dfile << "head06 = " << gsgm.param06 << endl;
    dfile << endl;
   }

 // read name
 char name[30];
 ifile.read(&name[0], 30);
 if(debug) {
    dfile << "MODEL NAME" << endl;
    dfile << " name = " << name << endl;
    dfile << endl;
   }

 // read (offset, information) pairs
 uint32 n_offset_pairs = 18;
 for(uint32 i = 0; i < 14; i++) {
     offset[i] = BE_read_uint32(ifile) + start;
     inform[i] = BE_read_uint32(ifile);
    }

 // floats that come after offset section
 auto temp01 = BE_read_real32(ifile);
 auto temp02 = BE_read_real32(ifile);
 auto temp03 = BE_read_real32(ifile);
 auto temp04 = BE_read_real32(ifile);
 auto temp05 = BE_read_real32(ifile);
 auto temp06 = BE_read_real32(ifile);
 auto temp07 = BE_read_real32(ifile);
 auto temp08 = BE_read_real32(ifile);
 auto temp09 = BE_read_real32(ifile);
 auto temp10 = BE_read_real32(ifile);
 auto temp11 = BE_read_real32(ifile);
 auto temp12 = BE_read_real32(ifile);
 if(debug) {
    dfile << "BOUNDING VOLUME INFORMATION" << endl;
    dfile << " item = " << temp01 << endl;
    dfile << " item = " << temp02 << endl;
    dfile << " item = " << temp03 << endl;
    dfile << " item = " << temp04 << endl;
    dfile << " item = " << temp05 << endl;
    dfile << " item = " << temp06 << endl;
    dfile << " item = " << temp07 << endl;
    dfile << " item = " << temp08 << endl;
    dfile << " item = " << temp09 << endl;
    dfile << " item = " << temp10 << endl;
    dfile << " item = " << temp11 << endl;
    dfile << " item = " << temp12 << endl;
    dfile << endl;
   }

 // more offset information pairs
 for(uint32 i = 14; i < n_offset_pairs; i++) {
     offset[i] = BE_read_uint32(ifile) + start;
     inform[i] = BE_read_uint32(ifile);
    }

 // print offset table
 if(debug) {
    dfile << "OFFSET TABLE" << endl;
    for(uint32 i = 0; i < 18; i++) {
        dfile << " item[" << setfill('0') << setw(2) << i << "] = <0x" << hex << offset[i] << ", 0x" << inform[i] << dec << ">" << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection00(void)
{
 // nothing to do
 uint32 section_index = 0x00;
 uint32 item_size = 0x80;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #0" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section00.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section00[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection01(void)
{
 // nothing to do
 uint32 section_index = 0x01;
 uint32 item_size = 0x40;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #1" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section01.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section01[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection02(void)
{
 // nothing to do
 uint32 section_index = 0x02;
 uint32 item_size = 0x40;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #2" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section02.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section02[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection03(void)
{
 // nothing to do
 uint32 section_index = 0x03;
 uint32 item_size = 0x80;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #3" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section03.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section03[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection04(void)
{
 // nothing to do
 uint32 section_index = 0x04;
 uint32 item_size = 0x10;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #4" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section04.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section04[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection05(void)
{
 // nothing to do
 uint32 section_index = 0x05;
 uint32 item_size = 0x40;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #5" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section05.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section05[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection06(void)
{
 // 0x00: index
 // 0x04: vertices
 // 0x08: unknown
 // 0x0C: unknown
 // 0x0E: unknown
 // 0x10: offset
 // 0x14: length
 // 0x18: vertex size
 // 0x1C: unknown

 // nothing to do
 uint32 section_index = 0x06;
 uint32 item_size = 0x20;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #6" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section06.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section06[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection07(void)
{
 // nothing to do
 uint32 section_index = 0x07;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #7" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 section07.reset(new char[inform[section_index]]);
 ifile.read(section07.get(), inform[section_index]);
 if(ifile.fail()) return error("Read failure.");

 // // print debug output
 // if(debug) {
 //    for(uint32 i = 0; i < inform[section_index]; i++) {
 //        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section07[i].get());
 //        dfile << hex;
 //        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
 //        dfile << dec << endl;
 //       }
 //    dfile << endl;
 //   }

 return true;
}

bool HMDExtractor::processSection08(void)
{
 // TEXTURE ASSOCIATION
 // 0x00: identifier
 // 0x02: string

 // nothing to do
 uint32 section_index = 0x08;
 uint32 item_size = 0x20;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #8" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section08.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section08[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection09(void)
{
 // UNKNOWN NAMES
 // 0x00: identifier
 // 0x02: string

 // nothing to do
 uint32 section_index = 0x09;
 uint32 item_size = 0x20;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "----------" << endl;
    dfile << "SECTION #9" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section09.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section09[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection10(void)
{
 // BONE NAMES
 // 0x00: identifier
 // 0x02: string

 // nothing to do
 uint32 section_index = 0x0A;
 uint32 item_size = 0x20;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "-----------" << endl;
    dfile << "SECTION #10" << endl;
    dfile << "-----------" << endl;
    dfile << endl;
   }

 // for each joint
 for(uint32 i = 0; i < inform[section_index]; i++) {
     boost::shared_array<char> item(new char[item_size]);
     ifile.read(item.get(), item_size);
     if(ifile.fail()) return error("Read failure.");
     section10.push_back(item);
    }

 // print debug output
 if(debug) {
    for(uint32 i = 0; i < inform[section_index]; i++) {
        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(section10[i].get());
        dfile << hex;
        for(uint32 j = 0; j < item_size; j++) dfile << setfill('0') << setw(2) << (uint16)ptr[j];
        dfile << dec << endl;
       }
    dfile << endl;
   }

 return true;
}

bool HMDExtractor::processSection11(void)
{
 // nothing to do
 uint32 section_index = 0x0B;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "-----------" << endl;
    dfile << "SECTION #11" << endl;
    dfile << "-----------" << endl;
    dfile << endl;
   }

 uint32 n_indices = inform[section_index];
 section11.reset(new char[2*n_indices]);
 BE_read_array(ifile, reinterpret_cast<uint16*>(section11.get()), n_indices);
 if(ifile.fail()) return error("Read failure.");

 return true;
}

bool HMDExtractor::processSection12(void)
{
 // nothing to do
 uint32 section_index = 0x0C;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "-----------" << endl;
    dfile << "SECTION #12" << endl;
    dfile << "-----------" << endl;
    dfile << endl;
   }

 section12.reset(new char[inform[section_index]]);
 ifile.read(section12.get(), inform[section_index]);
 if(ifile.fail()) return error("Read failure.");

 return true;
}

bool HMDExtractor::processSection13(void)
{
 // nothing to do
 uint32 section_index = 0x0D;
 if(!inform[section_index]) return true;

 // move to offset
 ifile.seekg(offset[section_index]);
 if(ifile.fail()) return error("Seek failure.");

 // print debug information
 if(debug) {
    dfile << "-----------" << endl;
    dfile << "SECTION #13" << endl;
    dfile << "-----------" << endl;
    dfile << endl;
   }

 section13.reset(new char[inform[section_index]]);
 ifile.read(section13.get(), inform[section_index]);
 if(ifile.fail()) return error("Read failure.");

 return true;
}

bool HMDExtractor::processModel(void)
{
 // no vertex buffer
 if(!(inform[0x06] && inform[0x07]))
    return true;

 // process joint map
 bool has_jmap = (offset[13] && inform[13]);
 map<uint32, uint32> jmapfinder;
 if(has_jmap)
   {
    binary_stream data(section13, inform[13]);
    uint32 counter = 0;
    while(data.can_move(1))
         {
          // save position
          uint32 offset = data.tell();
          if(offset == binary_stream::npos) return error("Tell failure.");

          // read number of items
          uint32 items = data.BE_read_uint08();
          if(data.fail()) return error("Stream read failure.");

          if(debug) {
             dfile << "JOINT MAP #" << counter << endl;
             dfile << " NUMBER OF ITEMS: " << items << endl;
             if(items) dfile << " ";
            }

          // read items
          deque<uint32> itemlist;
          for(uint32 i = 0; i < items; i++) {
              uint32 temp = data.BE_read_uint08();
              if(data.fail()) return error("Stream read failure.");
              itemlist.push_back(temp);
              if(debug) dfile << temp << " ";
             }
          if(debug) dfile << endl;

          // insert joint map
          AMC_JOINTMAP jm;
          jm.jntmap = itemlist;
          amc.jmaplist.push_back(jm);

          // increment counter
          jmapfinder.insert(map<uint32,uint32>::value_type(offset, counter));
          counter++;
         }
   }


 // process vertex buffers
 uint32 n_items = inform[0x06];
 for(uint32 i = 0; i < n_items; i++)
    {
     // extract vertex information
     binary_stream vtxinfo(section06[i], 0x20);
     uint32 vxinfo01 = vtxinfo.BE_read_uint32(); // index
     uint32 vxinfo02 = vtxinfo.BE_read_uint32(); // number of vertices
     uint32 vxinfo03 = vtxinfo.BE_read_uint32(); // vertex type
     uint32 vxinfo04 = vtxinfo.BE_read_uint16();
     uint32 vxinfo05 = vtxinfo.BE_read_uint16();
     uint32 vxinfo06 = vtxinfo.BE_read_uint32(); // offset
     uint32 vxinfo07 = vtxinfo.BE_read_uint32(); // length
     uint32 vxinfo08 = vtxinfo.BE_read_uint32(); // vertex size
     uint32 vxinfo09 = vtxinfo.BE_read_uint32();
     if(vxinfo08 == 0x14) cout << " HAS 0x14 VERTEX SIZE" << endl;

     // initialize vertex buffer
     AMC_VTXBUFFER vb;
     vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV;
     if(vxinfo03 == 0x04) vb.flags |= AMC_VERTEX_WEIGHTS;
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
     vb.elem = vxinfo02;
     vb.data.reset(new AMC_VERTEX[vb.elem]);

     if(vxinfo08 == 0x20 && vxinfo03 == 0x04) vb.flags |= AMC_VERTEX_NORMAL;

     // move to vertex buffer offset
     binary_stream vtxdata(section07, inform[0x07]);
     vtxdata.seek(vxinfo06);
     if(vtxdata.fail()) return error("Stream seek failure.");

     // print vertex buffer
     if(debug) {
        dfile << "VERTEX BUFFER #" << i << endl;
        binary_stream cs(section07, inform[0x07]);
        cs.seek(vxinfo06);
        for(uint32 j = 0; j < vb.elem; j++) {
            for(uint32 k = 0; k < vxinfo08; k++) dfile << hex << setfill('0') << setw(2) << (uint32)cs.BE_read_uint08() << dec;
            dfile << endl;
           }
        dfile << endl;
       }

     // extract vertex buffer
     for(uint32 j = 0; j < vb.elem; j++)
        {
         // vertex size = 0x14
         if(vxinfo08 == 0x14)
           {
            vb.data[j].vx = vtxdata.BE_read_real32(); // 0x00
            vb.data[j].vy = vtxdata.BE_read_real32(); // 0x04
            vb.data[j].vz = vtxdata.BE_read_real32(); // 0x08
            vtxdata.BE_read_uint08(); // 0x0C
            vtxdata.BE_read_uint08(); // 0x0D
            vtxdata.BE_read_uint08(); // 0x0E
            vtxdata.BE_read_uint08(); // 0x0F
            vb.data[j].uv[0][0] = vtxdata.BE_read_real16(); // 0x10
            vb.data[j].uv[0][1] = vtxdata.BE_read_real16(); // 0x12
           }
         // vertex size = 0x1C
         else if(vxinfo08 == 0x1C)
           {
            if(vxinfo03 == 0x04)
              {
               vb.data[j].vx = vtxdata.BE_read_real32(); // 0x00
               vb.data[j].vy = vtxdata.BE_read_real32(); // 0x04
               vb.data[j].vz = vtxdata.BE_read_real32(); // 0x08
               vb.data[j].wv[0] = vtxdata.BE_read_uint08()/255.0f; // 0x0C
               vb.data[j].wv[1] = vtxdata.BE_read_uint08()/255.0f; // 0x0D
               vb.data[j].wv[2] = vtxdata.BE_read_uint08()/255.0f; // 0x0E
               vb.data[j].wv[3] = vtxdata.BE_read_uint08()/255.0f; // 0x0F
               vb.data[j].wv[4] = 0.0f;
               vb.data[j].wv[5] = 0.0f;
               vb.data[j].wv[6] = 0.0f;
               vb.data[j].wv[7] = 0.0f;
               vb.data[j].wi[0] = vtxdata.BE_read_uint08(); // 0x10
               vb.data[j].wi[1] = vtxdata.BE_read_uint08(); // 0x11
               vb.data[j].wi[2] = vtxdata.BE_read_uint08(); // 0x12
               vb.data[j].wi[3] = vtxdata.BE_read_uint08(); // 0x13
               vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vtxdata.BE_read_uint08(); // 0x14
               vtxdata.BE_read_uint08(); // 0x15
               vtxdata.BE_read_uint08(); // 0x16
               vtxdata.BE_read_uint08(); // 0x17
               vb.data[j].uv[0][0] = vtxdata.BE_read_real16(); // 0x18
               vb.data[j].uv[0][1] = vtxdata.BE_read_real16(); // 0x1A
              }
            else if(vxinfo03 == 0xEFFFFFF4)
              {
               vb.data[j].vx = vtxdata.BE_read_real32(); // 0x00
               vb.data[j].vy = vtxdata.BE_read_real32(); // 0x04
               vb.data[j].vz = vtxdata.BE_read_real32(); // 0x08
               vtxdata.BE_read_uint08(); // 0x0C
               vtxdata.BE_read_uint08(); // 0x0D
               vtxdata.BE_read_uint08(); // 0x0E
               vtxdata.BE_read_uint08(); // 0x0F
               vtxdata.BE_read_uint08(); // 0x10
               vtxdata.BE_read_uint08(); // 0x11
               vtxdata.BE_read_uint08(); // 0x12
               vtxdata.BE_read_uint08(); // 0x13
               vb.data[j].uv[0][0] = vtxdata.BE_read_real16(); // 0x14
               vb.data[j].uv[0][1] = vtxdata.BE_read_real16(); // 0x16
               vtxdata.BE_read_uint16(); // 0x18
               vtxdata.BE_read_uint16(); // 0x1A
              }
            else
               return error("Unknown 0x1C vertex format.");
           }
         else if(vxinfo08 == 0x20)
           {
            if(vxinfo03 == 0x04)
              {
               vb.data[j].vx = vtxdata.BE_read_real32(); //  4
               vb.data[j].vy = vtxdata.BE_read_real32(); //  8
               vb.data[j].vz = vtxdata.BE_read_real32(); // 12
               vb.data[j].wv[0] = vtxdata.BE_read_uint08()/255.0f; // 13
               vb.data[j].wv[1] = vtxdata.BE_read_uint08()/255.0f; // 14
               vb.data[j].wv[2] = vtxdata.BE_read_uint08()/255.0f; // 15
               vb.data[j].wv[3] = vtxdata.BE_read_uint08()/255.0f; // 16
               vb.data[j].wv[4] = 0.0f;
               vb.data[j].wv[5] = 0.0f;
               vb.data[j].wv[6] = 0.0f;
               vb.data[j].wv[7] = 0.0f;
               vb.data[j].wi[0] = vtxdata.BE_read_uint08(); // 17
               vb.data[j].wi[1] = vtxdata.BE_read_uint08(); // 18
               vb.data[j].wi[2] = vtxdata.BE_read_uint08(); // 19
               vb.data[j].wi[3] = vtxdata.BE_read_uint08(); // 20
               vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].nx = (static_cast<real32>(vtxdata.BE_read_uint08()) - 127.0f)/127.0f; // 21
               vb.data[j].ny = (static_cast<real32>(vtxdata.BE_read_uint08()) - 127.0f)/127.0f; // 22
               vb.data[j].nz = (static_cast<real32>(vtxdata.BE_read_uint08()) - 127.0f)/127.0f; // 23
               vb.data[j].nw = (static_cast<real32>(vtxdata.BE_read_uint08()) - 127.0f)/127.0f; // 24
               vtxdata.BE_read_sint08(); // 25
               vtxdata.BE_read_sint08(); // 26
               vtxdata.BE_read_sint08(); // 27
               vtxdata.BE_read_sint08(); // 28
               vb.data[j].uv[0][0] = vtxdata.BE_read_real16(); // 30
               vb.data[j].uv[0][1] = vtxdata.BE_read_real16(); // 32
              }
            else if(vxinfo03 == 0xEFFFFFF4)
              {
               vb.data[j].vx = vtxdata.BE_read_real32(); //  4
               vb.data[j].vy = vtxdata.BE_read_real32(); //  8
               vb.data[j].vz = vtxdata.BE_read_real32(); // 12
               vtxdata.BE_read_uint32(); // 16
               vtxdata.BE_read_uint32(); // 20
               vtxdata.BE_read_uint32(); // 24
               vb.data[j].uv[0][0] = vtxdata.BE_read_real16(); // 26
               vb.data[j].uv[0][1] = vtxdata.BE_read_real16(); // 28
               vtxdata.BE_read_uint32(); // 32
              }
            else if(vxinfo03 == 0xEFFFFF44)
              {
               vb.data[j].vx = vtxdata.BE_read_real32(); //  4
               vb.data[j].vy = vtxdata.BE_read_real32(); //  8
               vb.data[j].vz = vtxdata.BE_read_real32(); // 12
               vtxdata.BE_read_uint32(); // 16
               vtxdata.BE_read_uint32(); // 20
               vtxdata.BE_read_real16(); // 22
               vtxdata.BE_read_real16(); // 24
               vb.data[j].uv[0][0] = vtxdata.BE_read_real16(); // 26
               vb.data[j].uv[0][1] = vtxdata.BE_read_real16(); // 28
               vtxdata.BE_read_uint32(); // 32
              }
            else if(vxinfo03 == 0xEFFF4FF4)
              {
               vb.data[j].vx = vtxdata.BE_read_real32(); //  4
               vb.data[j].vy = vtxdata.BE_read_real32(); //  8
               vb.data[j].vz = vtxdata.BE_read_real32(); // 12
               vtxdata.BE_read_uint32(); // 16
               vtxdata.BE_read_uint32(); // 20
               vtxdata.BE_read_real16(); // 22
               vtxdata.BE_read_real16(); // 24
               vb.data[j].uv[0][0] = vtxdata.BE_read_real16(); // 26
               vb.data[j].uv[0][1] = vtxdata.BE_read_real16(); // 28
               vtxdata.BE_read_uint32(); // 32
              }
            else
               return error("Unknown 0x20 vertex format.");
           }
         else if(vxinfo08 == 0x24)
           {
            if(vxinfo03 == 0xEFFFFF44)
              {
               vb.data[j].vx = vtxdata.BE_read_real32(); //  4
               vb.data[j].vy = vtxdata.BE_read_real32(); //  8
               vb.data[j].vz = vtxdata.BE_read_real32(); // 12
               vtxdata.BE_read_uint32(); // 16
               vtxdata.BE_read_uint32(); // 20
               vtxdata.BE_read_uint32(); // 24
               vtxdata.BE_read_real16(); // 26
               vtxdata.BE_read_real16(); // 28
               vb.data[j].uv[0][0] = vtxdata.BE_read_real16(); // 30
               vb.data[j].uv[0][1] = vtxdata.BE_read_real16(); // 32
               vtxdata.BE_read_uint32(); // 36
              }
            else
               return error("Unknown 0x24 vertex format.");
           }
         else if(vxinfo08 == 0x28)
           {
            if(vxinfo03 == 0x04)
              {
               vb.data[j].vx = vtxdata.BE_read_real32(); //  4
               vb.data[j].vy = vtxdata.BE_read_real32(); //  8
               vb.data[j].vz = vtxdata.BE_read_real32(); // 12
               vb.data[j].wv[0] = vtxdata.BE_read_uint08()/255.0f; // 13
               vb.data[j].wv[1] = vtxdata.BE_read_uint08()/255.0f; // 14
               vb.data[j].wv[2] = vtxdata.BE_read_uint08()/255.0f; // 15
               vb.data[j].wv[3] = vtxdata.BE_read_uint08()/255.0f; // 16
               vb.data[j].wv[4] = 0.0f;
               vb.data[j].wv[5] = 0.0f;
               vb.data[j].wv[6] = 0.0f;
               vb.data[j].wv[7] = 0.0f;
               vb.data[j].wi[0] = vtxdata.BE_read_uint08(); // 17
               vb.data[j].wi[1] = vtxdata.BE_read_uint08(); // 18
               vb.data[j].wi[2] = vtxdata.BE_read_uint08(); // 19
               vb.data[j].wi[3] = vtxdata.BE_read_uint08(); // 20
               vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vtxdata.BE_read_uint32(); // 24
               vtxdata.BE_read_uint32(); // 28
               vtxdata.BE_read_uint32(); // 32
               vtxdata.BE_read_uint32(); // 36
               vb.data[j].uv[0][0] = vtxdata.BE_read_real16(); // 38
               vb.data[j].uv[0][1] = vtxdata.BE_read_real16(); // 40
              }
            else
               return error("Unknown 0x28 vertex format.");
           }
         else {
            stringstream ss;
            ss << "Unknown vertex size 0x" << hex << vxinfo08 << dec << "." << endl;
            return error(ss.str().c_str());
           }
        }

     // insert vertex buffer
     amc.vblist.push_back(vb);
    }

 // no index buffer
 if(!(inform[0x0B]))
    return true;

 // create index buffer
 AMC_IDXBUFFER ib;
 ib.format = AMC_UINT16;
 ib.type = AMC_TRIANGLES;
 ib.wind = AMC_CW;
 ib.reserved = 0;
 ib.name = "default";
 ib.elem = inform[11];
 ib.data = section11;

 // insert index buffer
 amc.iblist.push_back(ib);

 // no surface information
 if(!inform[0x02])
    return true;

 // for each surface
 for(uint32 i = 0; i < inform[2]; i++)
    {
     // extract vertex information
     binary_stream item(section02[i], 0x40);
     uint32 p01 = item.BE_read_uint32(); // index
     uint32 p02 = item.BE_read_uint32(); // mesh number
     uint32 p03 = item.BE_read_uint32(); // vertex buffer index
     uint32 p04 = item.BE_read_uint32(); // unknown
     uint32 p05 = item.BE_read_uint32(); // index buffer elem (LOD 0)
     uint32 p06 = item.BE_read_uint32(); // index buffer offset (LOD 0)
     uint32 p07 = item.BE_read_uint32(); // index buffer elem (LOD 1)
     uint32 p08 = item.BE_read_uint32(); // index buffer offset (LOD 1)
     uint32 p09 = item.BE_read_uint32(); // index buffer elem (LOD 2)
     uint32 p10 = item.BE_read_uint32(); // index buffer offset (LOD 2)
     uint32 p11 = item.BE_read_uint32(); // number of joints
     uint32 p12 = item.BE_read_uint32(); // offset to joint map

     // LOD 0 must be use triangles
     if(p05 % 3) return error("Mesh does not use triangles.");

     // lookup joint map
     auto jmapiter = jmapfinder.find(p12);

     // create mesh name
     stringstream ss;
     ss << "mesh_" << setfill('0') << setw(3) << p01 << "_";
     ss << "part_" << setfill('0') << setw(3) << p02;

     // create surface
     AMC_SURFACE sur;
     sur.name = ss.str();
     AMC_REFERENCE ref;
     ref.vb_index = p03;
     ref.vb_start = 0;
     ref.vb_width = amc.vblist[p03].elem;
     ref.ib_index = 0;
     ref.ib_start = p06;
     ref.ib_width = p05;
     ref.jm_index = (jmapiter != jmapfinder.end() ? jmapiter->second : AMC_INVALID_JMAP_INDEX);
     sur.refs.push_back(ref);
     sur.surfmat = AMC_INVALID_SURFMAT;

     // insert surface
     amc.surfaces.push_back(sur);
    }

 return true;
}

bool HMDExtractor::processBones(void)
{
 // no skeleton
 if(!(inform[0x00] && inform[0x00])) return true;
 if(!(inform[0x05] && inform[0x05])) return true;

 struct pointobj {
  real32 x;
  real32 y;
  real32 z;
 };
 deque<pointobj> pointlist;

 // process parent map
 uint32 n_elem = inform[0x00];
 map<uint32, uint32> parentmap;
 for(uint32 i = 0; i < n_elem; i++)
    {
     binary_stream bs(section00[i], 0x80);
     uint32 p01 = bs.BE_read_uint32(); // id
     uint32 p02 = bs.BE_read_uint32(); // child #1
     uint32 p03 = bs.BE_read_uint32(); // child #2
     uint32 p04 = bs.BE_read_uint32(); // child #3
     uint32 p05 = bs.BE_read_uint32(); // id (repeated)
     uint32 p06 = bs.BE_read_uint32(); // unknown (0, 1, or 2)
     uint32 p07 = bs.BE_read_uint32(); // offset to bone name in section10
     uint32 p08 = bs.BE_read_uint32(); // 0, 2 (0 for regular joint, 2 for IK?)
 
     // read a bunch of floats
     real32 p09[0x14];
     bs.BE_read_array(&p09[0], 0x14);
     if(bs.fail()) return error("Stream read failure.");

     // insert point
     pointobj pnt;
     pnt.x = p09[0xC];
     pnt.y = p09[0xD];
     pnt.z = p09[0xE];
     pointlist.push_back(pnt);

     // read a bunch of zeros
     real32 p10[0x4];
     bs.BE_read_array(&p10[0], 0x4);
     if(bs.fail()) return error("Stream read failure.");
 
     // given a joint id, we need to know the parent
     if(p08 == 0x00) {
        if(p02 != 0xFFFFFFFF) parentmap.insert(map<uint32,uint32>::value_type(p02, p01));
        if(p03 != 0xFFFFFFFF) parentmap.insert(map<uint32,uint32>::value_type(p03, p01));
        if(p04 != 0xFFFFFFFF) parentmap.insert(map<uint32,uint32>::value_type(p04, p01));
       }
    }

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = "skeleton";
 skel.tiplen = 1.0f;

 // process joints
 for(uint32 i = 0; i < n_elem; i++)
    {
     // joint position
     real32 x = pointlist[i].x;
     real32 y = pointlist[i].y;
     real32 z = pointlist[i].z;

     uint32 id = i;
     uint32 parent = AMC_INVALID_JOINT;
     auto iter = parentmap.find(id);
     if(iter != parentmap.end()) parent = iter->second;

     dfile << "parent of 0x" << hex << id << dec << " is 0x" << hex << parent << dec << "." << endl;

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << id;

     // create joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = id;
     joint.parent = parent;
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
     joint.p_rel[0] = x;
     joint.p_rel[1] = y;
     joint.p_rel[2] = z;
     joint.p_rel[3] = 1.0f;

     // insert joint
     skel.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel);
 return true;

/*
 // no skeleton
 if(!(inform[0x00] && inform[0x00])) return true;
 if(!(inform[0x05] && inform[0x05])) return true;

 // process parent map
 uint32 n_elem = inform[0x00];
 map<uint32, uint32> parentmap;
 for(uint32 i = 0; i < n_elem; i++)
    {
     binary_stream bs(section00[i], 0x80);
     uint32 p01 = bs.BE_read_uint32(); // id
     uint32 p02 = bs.BE_read_uint32(); // child #1
     uint32 p03 = bs.BE_read_uint32(); // child #2
     uint32 p04 = bs.BE_read_uint32(); // child #3
     uint32 p05 = bs.BE_read_uint32(); // id (repeated)
     uint32 p06 = bs.BE_read_uint32(); // unknown (0, 1, or 2)
     uint32 p07 = bs.BE_read_uint32(); // offset to bone name in section10
     uint32 p08 = bs.BE_read_uint32(); // 0, 2 (0 for regular joint, 2 for IK?)
 
     // read a bunch of floats
     real32 p09[0x14];
     bs.BE_read_array(&p09[0], 0x14);
     if(bs.fail()) return error("Stream read failure.");
     
     // read a bunch of zeros
     real32 p10[0x4];
     bs.BE_read_array(&p10[0], 0x4);
     if(bs.fail()) return error("Stream read failure.");
 
     // given a joint id, we need to know the parent
     if(p08 == 0x00) {
        if(p02 != 0xFFFFFFFF) parentmap.insert(map<uint32,uint32>::value_type(p02, p01));
        if(p03 != 0xFFFFFFFF) parentmap.insert(map<uint32,uint32>::value_type(p03, p01));
        if(p04 != 0xFFFFFFFF) parentmap.insert(map<uint32,uint32>::value_type(p04, p01));
       }

     // print debug information
     if(debug) {
        dfile << " JOINT 0x" << hex << i << dec << endl;
        dfile << " p01 = 0x" << hex << p01 << dec << endl;
        dfile << " p02 = 0x" << hex << p02 << dec << endl;
        dfile << " p03 = 0x" << hex << p03 << dec << endl;
        dfile << " p04 = 0x" << hex << p04 << dec << endl;
        dfile << " p05 = 0x" << hex << p05 << dec << endl;
        dfile << " p06 = 0x" << hex << p06 << dec << endl;
        dfile << " p07 = 0x" << hex << p07 << dec << endl;
        dfile << " p08 = 0x" << hex << p08 << dec << endl;
        dfile << " data = ";
        for(uint32 j = 0; j < 0x14; j++) dfile << p09[j] << " ";
        dfile << endl;
        dfile << " zero[0] = 0x" << hex << p10[0] << dec << endl;
        dfile << " zero[1] = 0x" << hex << p10[1] << dec << endl;
        dfile << " zero[2] = 0x" << hex << p10[2] << dec << endl;
        dfile << " zero[3] = 0x" << hex << p10[3] << dec << endl;
        dfile << endl;
       }
    }

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = "skeleton";
 skel.tiplen = 1.0f;

 // process joints
 uint32 n_jnts = inform[0x05];
 for(uint32 i = 0; i < n_jnts; i++)
    {
     // extract joint information
     binary_stream jntdata(section05[i], 0x40);
     real32 m[16];
     jntdata.BE_read_array(&m[0], 16);

     // joint position
     real32 x = -m[12];
     real32 y = -m[13];
     real32 z = -m[14];

     uint32 id = i;
     uint32 parent = AMC_INVALID_JOINT;
     auto iter = parentmap.find(id);
     if(iter != parentmap.end()) parent = iter->second;

     dfile << "parent of 0x" << hex << id << dec << " is 0x" << hex << parent << dec << "." << endl;

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << id;

     // create joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = id;
     joint.parent = parent;
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
     joint.p_rel[0] = x;
     joint.p_rel[1] = y;
     joint.p_rel[2] = z;
     joint.p_rel[3] = 1.0f;

     // insert joint
     skel.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel);
 return true;
*/
}

bool HMDExtractor::extract(void)
{
 if(!processHeaders()) return false;
 if(!processSection00()) return false;
 if(!processSection01()) return false;
 if(!processSection02()) return false;
 if(!processSection03()) return false;
 if(!processSection04()) return false;
 if(!processSection05()) return false;
 if(!processSection06()) return false;
 if(!processSection07()) return false;
 if(!processSection08()) return false;
 if(!processSection09()) return false;
 if(!processSection10()) return false;
 if(!processSection11()) return false;
 if(!processSection12()) return false;
 if(!processSection13()) return false;
 if(!processModel()) return false;
 if(!processBones()) return false;
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 return true;
}

bool processHMD(const string& filename)
{
 HMDExtractor temp(filename);
 return temp.extract();
}

};};

namespace X_SYSTEM { namespace X_GAME {

bool extract(void)
{
 string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pname)
{
 // set pathname
 using namespace std;
 std::string pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname();

 bool doFLP = false;
 bool doHTX = false;
 bool doHMD = true;
 bool doHMT = true;

 cout << "STAGE 1" << endl;
 if(doFLP) {
    cout << "Processing FLP files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".flp", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processFLP(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 2" << endl;
 if(doHTX) {
    cout << "Processing HTX files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".htx", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processHTX(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 3" << endl;
 if(doHMD) {
    cout << "Processing HMD files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".hmd", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processHMD(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

};};

namespace X_SYSTEM { namespace X_GAME {

 //bool extract(void)
 //{
 // cout << "STAGE 4" << endl;
 // if(doHMT) {
 //    cout << "Processing HMT files..." << endl;
 //    deque<string> filelist;
 //    BuildFilenameList(filelist, ".hmt", pathname.c_str());
 //    for(size_t i = 0; i < filelist.size(); i++) {
 //        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
 //        if(!processHMT(filelist[i])) return false;
 //       }
 //    cout << endl;
 //   }
 //
 // return true;
 //}

//bool extractor::processHMT(const string& filename)
//{
// HMTProcessor temp(filename);
// return temp.extract();
//}

};};