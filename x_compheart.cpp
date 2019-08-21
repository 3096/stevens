#include "xentax.h"
#include "x_file.h"
#include "x_kary.h"
#include "x_amc.h"
#include "x_dds.h"
#include "x_con.h"
#include "x_compheart.h"
using namespace std;

/*

bool CompileHeartProcessTIDFiles(const std::string& pathname)
{
 // process TID files
 using namespace std;
 cout << "Processing .TID files..." << endl;
 deque<string> filelist;
 BuildFilenameList(filelist, ".TID", (pathname.length() ? pathname : GetModulePathname()).c_str());
 for(size_t i = 0; i < filelist.size(); i++) {
     cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!CompileHeartConvertTID(filelist[i])) return false;
    }

 // finished
 cout << endl;
 return true;
}

bool CompileHeartProcessISM2Files(const std::string& pathname)
{
 // process ISM2 files
 using namespace std;
 cout << "Processing .ISM2 files..." << endl;
 deque<string> filelist;
 BuildFilenameList(filelist, ".ISM2", (pathname.length() ? pathname : GetModulePathname()).c_str());
 for(size_t i = 0; i < filelist.size(); i++) {
     cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!CompileHeartConvertISM2(filelist[i])) return false;
    }

 // finished
 cout << endl;
 return true;
}

bool CompileHeartConvertTID(const std::string& filename)
{
 // open file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open TID file.");

 // filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // create output filename
 string ofname = pathname;
 ofname += shrtname;
 ofname += ".dds";

 // uncompressed RGBA mask types
 const uint32 MASK_B = 0xFF000000;
 const uint32 MASK_G = 0x00FF0000;
 const uint32 MASK_R = 0x0000FF00;
 const uint32 MASK_A = 0x000000FF;

 // read first four bytes
 uint32 m1 = static_cast<uint32>(BE_read_uint08(ifile));
 uint32 m2 = static_cast<uint32>(BE_read_uint08(ifile));
 uint32 m3 = static_cast<uint32>(BE_read_uint08(ifile));
 uint32 m4 = static_cast<uint32>(BE_read_uint08(ifile));
 if(ifile.fail()) return error("Read failure.");

 // validate first four bytes
 if(m1 != 0x54) return error("Invalid TID file.");
 if(m2 != 0x49) return error("Invalid TID file.");
 if(m3 != 0x44) return error("Invalid TID file.");

 // uncompressed
 // type 0x13, 0x83, 0x93, and 0x9B (upside down)
 if((m4 == 0x13) || (m4 == 0x83) || (m4 == 0x93) || (m4 == 0x9B))
   {
    // read header
    uint32 head01 = BE_read_uint32(ifile); // 0x00400080 (maybe two 16-bit values instead)
    uint32 head02 = BE_read_uint32(ifile); // offset to texture data
    uint32 head03 = BE_read_uint32(ifile); // number of files (0x01)
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
 // type 0x87 and 0x97 (uncompressed scrambled)
 else if((m4 == 0x87) || (m4 == 0x97))
   {
    // read header
    uint32 head01 = BE_read_uint32(ifile); // 0x00400080 (maybe two 16-bit values instead)
    uint32 head02 = BE_read_uint32(ifile); // offset to texture data
    uint32 head03 = BE_read_uint32(ifile); // number of files (0x01)
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
            copy[copy_position + 0] = data[data_position + 0]; // b
            copy[copy_position + 1] = data[data_position + 1]; // g
            copy[copy_position + 2] = data[data_position + 2]; // r
            copy[copy_position + 3] = data[data_position + 3]; // a
           }
       }

    // save DDS data
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create DDS file.");
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    ofile.write(copy.get(), head15);
   }
 // type 0x8D (DXT1/DXT5 scrambled)
 // TODO: unscramble it
 else if(m4 == 0x8D)
   {
    // read header
    uint32 head01 = BE_read_uint32(ifile);
    uint32 head02 = BE_read_uint32(ifile); // offset to texture data
    uint32 head03 = BE_read_uint32(ifile); // number of files (0x01)
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
    uint32 head12 = BE_read_uint16(ifile); // ??? number of mipmaps
    uint32 head13 = BE_read_uint16(ifile); // ??? number of mipmaps
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
    else if(head18 == 0x33545844) CreateDXT3Header(head09, head10, 1, FALSE, &ddsh);
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
 // type 0x98 (NEW!!! SAME AS 0x99 BUT LITTLE ENDIAN!!!)
 else if(m4 == 0x98)
   {
    // read header
    uint32 head01 = LE_read_uint32(ifile);
    uint32 head02 = LE_read_uint32(ifile); // offset to texture data
    uint32 head03 = LE_read_uint32(ifile); // number of files (0x01)
    uint32 head04 = LE_read_uint32(ifile); // 0x01
    uint32 head05 = LE_read_uint32(ifile); // 0x20
    uint32 head06 = LE_read_uint32(ifile); // 0x00
    uint32 head07 = LE_read_uint32(ifile); // 0x00

    // read filename
    char name[32];
    LE_read_array(ifile, &name[0], 32);

    uint32 head08 = LE_read_uint32(ifile); // 0x60
    uint32 head09 = LE_read_uint32(ifile); // dx
    uint32 head10 = LE_read_uint32(ifile); // dy
    uint32 head11 = LE_read_uint32(ifile); // 0x00
    uint32 head12 = LE_read_uint16(ifile); // ??? number of mipmaps
    uint32 head13 = LE_read_uint16(ifile); // ??? number of mipmaps
    uint32 head14 = LE_read_uint32(ifile); // 0x00
    uint32 head15 = LE_read_uint32(ifile); // size of image data
    uint32 head16 = LE_read_uint32(ifile); // 0x80
    uint32 head17 = LE_read_uint32(ifile); // 0x04
    uint32 head18 = LE_read_uint32(ifile); // compression
    uint32 head19 = LE_read_uint32(ifile); // ????
    uint32 head20 = LE_read_uint32(ifile); // 0x00

    // create DDS header
    DDS_HEADER ddsh;
    if(head18 == 0x31545844) CreateDXT1Header(head09, head10, 1, FALSE, &ddsh);
    else if(head18 == 0x33545844) CreateDXT3Header(head09, head10, 1, FALSE, &ddsh);
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
 // type 0x81, 0x89, 0x91 (toon_shadow), 0x99 (mipmapped)
 else if((m4 == 0x81) || (m4 == 0x89) || (m4 == 0x91) || (m4 == 0x99))
   {
    // read header
    uint32 head01 = BE_read_uint32(ifile);
    uint32 head02 = BE_read_uint32(ifile); // offset to texture data
    uint32 head03 = BE_read_uint32(ifile); // number of files (0x01)
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
    uint32 head12 = BE_read_uint16(ifile); // ??? number of mipmaps
    uint32 head13 = BE_read_uint16(ifile); // ??? number of mipmaps
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
    else if(head18 == 0x33545844) CreateDXT3Header(head09, head10, 1, FALSE, &ddsh);
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
 // unknown type
 else
    return error("Unknown TID file type.");

 // success
 return true;
}

class ISM2Extractor {
 private :
  bool debug;
  string filename;
  string pathname;
  string shrtname;
  ifstream ifile;
  ofstream dfile;
 private :
  boost::shared_array<string> stringtable;
  string meshname;
  deque<string> surfacenames;
  uint32 curr_vtxbuffer;
  uint32 curr_idxbuffer;
  point3D_array<real32> pntbuffer;
  point3D_array<real32> norbuffer;
  point4D_array<real32> colbuffer;
  deque<point2D_array<real32>> texbuffer;
  point4D_array<uint16> blibuffer;
  point4D_array<real32> blwbuffer;
  struct index_buffer {
   boost::shared_array<char> data;
   uint08 bits;
   uint32 size;
  };
  deque<index_buffer> idxlist;
 // skeleton data
 private :
  map<uint32, uint32> bone_ofs_to_idx; // ofs -> idx
  map<uint32, uint32> bone_idx_to_ofs; // idx -> ofs
  map<uint32, uint32> bone_parent_map; // ofs -> ofs
  deque<boost::shared_array<real32>> bone_matrix_lst;
 private :
  ADVANCEDMODELCONTAINER amc;
 private :
  bool processXXX(uint32 offset, uint32 parent, uint32 caller);
  bool processX03(uint32 offset, uint32 parent, uint32 caller);
  bool processX04(uint32 offset, uint32 parent, uint32 caller);
  bool processX05(uint32 offset, uint32 parent, uint32 caller);
  bool processX0A(uint32 offset, uint32 parent, uint32 caller);
  bool processX0B(uint32 offset, uint32 parent, uint32 caller);
  bool processX0D(uint32 offset, uint32 parent, uint32 caller);
  bool processX0F(uint32 offset, uint32 parent, uint32 caller);
  bool processX21(uint32 offset, uint32 parent, uint32 caller);
  bool processX2D(uint32 offset, uint32 parent, uint32 caller);
  bool processX2E(uint32 offset, uint32 parent, uint32 caller);
  bool processX30(uint32 offset, uint32 parent, uint32 caller);
  bool processX31(uint32 offset, uint32 parent, uint32 caller);
  bool processX32(uint32 offset, uint32 parent, uint32 caller);
  bool processX34(uint32 offset, uint32 parent, uint32 caller);
  bool processX44(uint32 offset, uint32 parent, uint32 caller);
  bool processX45(uint32 offset, uint32 parent, uint32 caller);
  bool processX46(uint32 offset, uint32 parent, uint32 caller);
  bool processX4B(uint32 offset, uint32 parent, uint32 caller);
  bool processX4C(uint32 offset, uint32 parent, uint32 caller);
  bool processX4D(uint32 offset, uint32 parent, uint32 caller);
  bool processX50(uint32 offset, uint32 parent, uint32 caller);
  bool processX59(uint32 offset, uint32 parent, uint32 caller);
  bool processX5B(uint32 offset, uint32 parent, uint32 caller);
  bool processX5C(uint32 offset, uint32 parent, uint32 caller);
  bool processX61(uint32 offset, uint32 parent, uint32 caller);
  bool processX62(uint32 offset, uint32 parent, uint32 caller);
  bool processX63(uint32 offset, uint32 parent, uint32 caller);
  bool processX65(uint32 offset, uint32 parent, uint32 caller);
  bool processX6A(uint32 offset, uint32 parent, uint32 caller);
  bool processX6B(uint32 offset, uint32 parent, uint32 caller);
  bool processX6C(uint32 offset, uint32 parent, uint32 caller);
  bool processX6E(uint32 offset, uint32 parent, uint32 caller);
 public :
  bool extract(const string& fname);
  void clean(void);
 public :
  ISM2Extractor();
 ~ISM2Extractor();
 private :
  ISM2Extractor(const ISM2Extractor&);
  void operator =(const ISM2Extractor&);
};

ISM2Extractor::ISM2Extractor() : debug(false)
{
}

ISM2Extractor::~ISM2Extractor()
{
}

bool ISM2Extractor::processXXX(uint32 offset, uint32 parent, uint32 caller)
{
 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read signature
 uint32 signature = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // process node
 switch(signature) {
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
        ss << "Unknown section 0x" << hex << signature << dec << "." << endl;
        return error(ss.str().c_str());
       }
  }

 // success
 return true;
}

bool ISM2Extractor::processX03(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x03 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x03: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[5];
 if(!BE_read_array(ifile, &header[0], 5)) return error("0x03: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0]: 0x" << hex << header[0] << dec << " (signature)" << endl;
    dfile << " header[1]: 0x" << hex << header[1] << dec << " (size of header)" << endl;
    dfile << " header[2]: 0x" << hex << header[2] << dec << " (number of children)" << endl;
    dfile << " header[3]: 0x" << hex << header[3] << dec << " (string table index) " << stringtable[header[0x3]] << endl;
    dfile << " header[4]: 0x" << hex << header[4] << dec << " (string table index) " << stringtable[header[0x4]] << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x03) return error("0x03: Invalid node.");
 if(header[1] != 0x14) return error("0x03: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x03: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX04(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x04 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x04: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[16];
 if(!BE_read_array(ifile, &header[0], 16)) return error("0x04: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (string table index) " << stringtable[header[0x3]] << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (string table index) " << stringtable[header[0x4]] << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (unknown)" << endl;
    dfile << " header[0x6]: 0x" << hex << header[0x6] << dec << " (unknown)" << endl;
    dfile << " header[0x7]: 0x" << hex << header[0x7] << dec << " (parent section 0x05 offset)" << endl;
    dfile << " header[0x8]: 0x" << hex << header[0x8] << dec << " (unknown)" << endl;
    dfile << " header[0x9]: 0x" << hex << header[0x9] << dec << " (unknown)" << endl;
    dfile << " header[0xA]: 0x" << hex << header[0xA] << dec << " (unknown)" << endl;
    dfile << " header[0xB]: 0x" << hex << header[0xB] << dec << " (unknown)" << endl;
    dfile << " header[0xC]: 0x" << hex << header[0xC] << dec << " (unknown)" << endl;
    dfile << " header[0xD]: 0x" << hex << header[0xD] << dec << " (index into section 0x03 offset array)" << endl;
    dfile << " header[0xE]: 0x" << hex << header[0xE] << dec << " (unknown)" << endl;
    dfile << " header[0xF]: 0x" << hex << header[0xF] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x04) return error("0x04: Invalid node.");
 if(header[1] != 0x40) return error("0x04: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x04: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX05(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "--------------------------" << endl;
    dfile << "- 0x05 JOINT INFORMATION -" << endl;
    dfile << "--------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x05: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[16];
 if(!BE_read_array(ifile, &header[0], 16)) return error("0x05: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (string table index) " << stringtable[header[0x3]] << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (string table index) " << stringtable[header[0x4]] << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (string table index) " << stringtable[header[0x5]] << endl;
    dfile << " header[0x6]: 0x" << hex << header[0x6] << dec << " (unknown)" << endl;
    dfile << " header[0x7]: 0x" << hex << header[0x7] << dec << " (parent section 0x05 offset)" << endl;
    dfile << " header[0x8]: 0x" << hex << header[0x8] << dec << " (number of children)" << endl;
    dfile << " header[0x9]: 0x" << hex << header[0x9] << dec << " (unknown)" << endl;
    dfile << " header[0xA]: 0x" << hex << header[0xA] << dec << " (unknown)" << endl;
    dfile << " header[0xB]: 0x" << hex << header[0xB] << dec << " (bone index)" << endl;
    dfile << " header[0xC]: 0x" << hex << header[0xC] << dec << " (unknown)" << endl;
    dfile << " header[0xD]: 0x" << hex << header[0xD] << dec << " (index into section 0x03 offset array)" << endl;
    dfile << " header[0xE]: 0x" << hex << header[0xE] << dec << " (unknown)" << endl;
    dfile << " header[0xF]: 0x" << hex << header[0xF] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x05) return error("0x05: Invalid node.");
 if(header[1] != 0x40) return error("0x05: Invalid node data.");

 // map bone parenting information
 typedef map<uint32, uint32>::value_type value_type;
 bone_ofs_to_idx.insert(value_type(offset, header[0xB]));
 bone_idx_to_ofs.insert(value_type(header[0xB], offset));
 bone_parent_map.insert(value_type(offset, header[0x7]));

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x05: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 return true;
}

bool ISM2Extractor::processX0A(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "------------------" << endl;
    dfile << "- 0x0A MESH NODE -" << endl;
    dfile << "------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x0A: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[8];
 if(!BE_read_array(ifile, &header[0], 8)) return error("0x0A: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (string table index) " << stringtable[header[0x3]] << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (string table index) " << stringtable[header[0x4]] << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (unknown)" << endl;
    dfile << " header[0x6]: 0x" << hex << header[0x6] << dec << " (unknown)" << endl;
    dfile << " header[0x7]: 0x" << hex << header[0x7] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x0A) return error("0x0A: Invalid node.");
 if(header[1] != 0x20) return error("0x0A: Invalid node data.");

 // PREPROCESSING 0x0A CHILDREN
 // Children of 0x0A are expected to fill out one or more non-interleaved vertex
 // buffers and one or more index buffers. We must clear out any previous vertex
 // data.

 // clear vertex buffers
 pntbuffer.clear();
 norbuffer.clear();
 colbuffer.clear();
 texbuffer.clear();
 blibuffer.clear();
 blwbuffer.clear();

 // clear index buffers
 idxlist.clear();

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x0A: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // POSTPROCESSING 0x0A CHILDREN
 // Once we are done processing the 0x0A subtree, we should have the following:
 // POSITION buffer
 // NORMAL buffer
 // COLOR buffer(s)
 // UV buffer(s)
 // BLENDWEIGHTS buffer
 // INDEX buffers

 // must have position buffer
 if(!pntbuffer.size()) return error("0x0A: POSITION buffer not present.");

  // prepare vertex buffer
  AMC_VTXBUFFER vb;
  vb.name = "default";
  vb.elem = pntbuffer.size();
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

 // adjust vertex flags
 if(norbuffer.size() == pntbuffer.size()) vb.flags |= AMC_VERTEX_NORMAL;
 if(colbuffer.size() == pntbuffer.size()) vb.colors = 1; // one channel for now
 if((blwbuffer.size() == pntbuffer.size()) && (blibuffer.size() == pntbuffer.size()))
    vb.flags |= AMC_VERTEX_WEIGHTS;

 // copy positions
 for(uint32 i = 0; i < vb.elem; i++) {
     vb.data[i].vx = pntbuffer[i][0];
     vb.data[i].vy = pntbuffer[i][1];
     vb.data[i].vz = pntbuffer[i][2];
    }

 // copy normals
 if(vb.flags & AMC_VERTEX_NORMAL) {
    for(uint32 i = 0; i < vb.elem; i++) {
        vb.data[i].nx = norbuffer[i][0];
        vb.data[i].ny = norbuffer[i][1];
        vb.data[i].nz = norbuffer[i][2];
       }
   }

 // copy colors
 if(vb.colors) {
    for(uint32 i = 0; i < vb.elem; i++) {
        vb.data[i].color[0][0] = colbuffer[i][0];
        vb.data[i].color[0][1] = colbuffer[i][1];
        vb.data[i].color[0][2] = colbuffer[i][2];
        vb.data[i].color[0][3] = colbuffer[i][3];
       }
   }

 // copy texcoords
 if(texbuffer.size())
   {
    // enable texture mapping
    vb.flags |= AMC_VERTEX_UV;
    vb.uvchan = texbuffer.size();

    // copy data
    for(uint32 channel = 0; channel < vb.uvchan; channel++) {
        vb.uvtype[channel] = AMC_DIFFUSE_MAP;
        for(uint32 i = 0; i < vb.elem; i++) {
            vb.data[i].uv[channel][0] = texbuffer[channel][i][0];
            vb.data[i].uv[channel][1] = texbuffer[channel][i][1];
           }
       }
   }

 // copy blend weights and blend indices
 if(vb.flags & AMC_VERTEX_WEIGHTS) {
    for(uint32 i = 0; i < vb.elem; i++) {
        // copy blend weights
        vb.data[i].wv[0] = blwbuffer[i][0];
        vb.data[i].wv[1] = blwbuffer[i][1];
        vb.data[i].wv[2] = blwbuffer[i][2];
        vb.data[i].wv[3] = blwbuffer[i][3];
        vb.data[i].wv[4] = 0.0f;
        vb.data[i].wv[5] = 0.0f;
        vb.data[i].wv[6] = 0.0f;
        vb.data[i].wv[7] = 0.0f;
        // copy blend indices
        vb.data[i].wi[0] = (vb.data[i].wv[0] > 0 ? blibuffer[i][0] : AMC_INVALID_VERTEX_WMAP_INDEX);
        vb.data[i].wi[1] = (vb.data[i].wv[1] > 0 ? blibuffer[i][1] : AMC_INVALID_VERTEX_WMAP_INDEX);
        vb.data[i].wi[2] = (vb.data[i].wv[2] > 0 ? blibuffer[i][2] : AMC_INVALID_VERTEX_WMAP_INDEX);
        vb.data[i].wi[3] = (vb.data[i].wv[3] > 0 ? blibuffer[i][3] : AMC_INVALID_VERTEX_WMAP_INDEX);
        vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
   }

 // insert vertex buffer
 uint32 vb_index = curr_vtxbuffer++;
 amc.vblist.push_back(vb);

 // TODO: THIS CHECK MIGHT CHANGE IF THERE ARE MULTIPLE SURFACE NODES
 // should have same number of surfaces as index buffers
 if(surfacenames.size() < idxlist.size()) {
    stringstream ss;
    ss << "The number of surfaces (" << surfacenames.size() << ") is less than the number of index buffers (" << idxlist.size() << ").";
    return error(ss);
   }
 else if(surfacenames.size() > idxlist.size())
    message("Warning: The number of surfaces exceeds the number of index buffers.");

 // must have at least one index buffer
 if(!idxlist.size()) return error("0x0A: There are no index buffers.");

 // process index buffers
 for(uint32 i = 0; i < idxlist.size(); i++)
    {
     // create index buffer
     AMC_IDXBUFFER ib;
     ib.format = ((idxlist[i].bits == 16) ? AMC_UINT16 : AMC_UINT32);
     ib.type = AMC_TRIANGLES;
     ib.wind = AMC_CW;
     ib.name = "default";
     ib.elem = idxlist[i].size;
     ib.data = idxlist[i].data;

     // save index buffer
     uint32 ib_index = curr_idxbuffer++;
     amc.iblist.push_back(ib);
 
     // create surface
     AMC_SURFACE surface;
     surface.name = surfacenames[i];
     surface.surfmat = AMC_INVALID_SURFMAT;
     AMC_REFERENCE ref;
     ref.vb_index = vb_index;
     ref.vb_start = 0;
     ref.vb_width = amc.vblist[vb_index].elem; // use full vertex buffer
     ref.ib_index = ib_index;
     ref.ib_start = 0;
     ref.ib_width = idxlist[i].size;
     ref.jm_index = AMC_INVALID_JMAP_INDEX;
     surface.refs.push_back(ref);

     // insert surface
     amc.surfaces.push_back(surface);
    }

 // success
 return true;
}

bool ISM2Extractor::processX0B(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "------------------------" << endl;
    dfile << "- 0x0B MESH ARRAY NODE -" << endl;
    dfile << "------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x0B: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[3];
 if(!BE_read_array(ifile, &header[0], 3)) return error("0x0B: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x0B) return error("0x0B: Invalid node.");
 if(header[1] != 0x0C) return error("0x0B: Invalid node data.");

 // before we process any meshes, reset buffer indices
 curr_vtxbuffer = 0;
 curr_idxbuffer = 0;

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x0B: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // CONCLUSION
 // After we are finished processing all 0x0B subtree, there will be a finished
 // model, and we can output the SMC file.

 // save model files
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), transformed);
 return true;
}

bool ISM2Extractor::processX0D(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x0D UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x0D: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[7];
 if(!BE_read_array(ifile, &header[0], 7)) return error("0x0D: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (string table index) " << stringtable[header[0x3]] << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (string table index) " << stringtable[header[0x4]] << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (string table index) " << stringtable[header[0x5]] << endl;
    dfile << " header[0x6]: 0x" << hex << header[0x6] << dec << " (string table index) " << stringtable[header[0x6]] << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x0D) return error("0x0D: Invalid node.");
 if(header[1] != 0x1C) return error("0x0D: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x0D: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX0F(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x0F UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x0F: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // nothing to read here other than the signature
 return true;
}

bool ISM2Extractor::processX21(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "---------------------" << endl;
    dfile << "- 0x21 STRING TABLE -" << endl;
    dfile << "---------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x21: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[3];
 if(!BE_read_array(ifile, &header[0], 3)) return error("0x21: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0]: 0x" << hex << header[0] << dec << " (signature)" << endl;
    dfile << " header[1]: 0x" << hex << header[1] << dec << " (size of header)" << endl;
    dfile << " header[2]: 0x" << hex << header[2] << dec << " (number of strings)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x21) return error("0x21: Invalid node.");
 if(header[1] != 0x0C) return error("0x21: Invalid node data.");
 if(header[2] == 0x00) return error("0x21: Invalid number of strings.");

 // read string offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < header[2]; i++) {
     uint32 item = BE_read_uint32(ifile);
     offsets.push_back(item);
    }

 // read string table
 stringtable.reset(new string[header[2]]);
 for(uint32 i = 0; i < header[2]; i++)
    {
     // move to table data
     ifile.seekg(offsets[i], ios::beg);
     if(ifile.fail()) return error("0x21: Seek failure.");

     // read string
     char item[1024];
     read_string(ifile, &item[0], 1024);

     // add string
     stringtable[i] = item;
     if(debug) dfile << " stringtable[0x" << hex << i << dec << "] = " << stringtable[i] << endl;
    }
 if(debug) dfile << endl;

 // success
 return true;
}

bool ISM2Extractor::processX2D(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------------------" << endl;
    dfile << "- 0x2D TEXTURE INFORMATION -" << endl;
    dfile << "----------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x2D: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 signature = BE_read_uint32(ifile);
 if(ifile.fail()) return error("0x2D: Read failure.");

 // read texture string table references
 uint32 data[7];
 if(!BE_read_array(ifile, &data[0], 7)) return error("0x2D: Read failure.");

 // display node header
 if(debug) {
    dfile << " signature: 0x" << hex << signature << dec << " (signature)" << endl;
    dfile << " data[0x0]: 0x" << hex << data[0x0] << dec << " (string table index) " << stringtable[data[0x0]] << endl;
    dfile << " data[0x1]: 0x" << hex << data[0x1] << dec << " (string table index) " << stringtable[data[0x1]] << endl;
    dfile << " data[0x2]: 0x" << hex << data[0x2] << dec << " (string table index) " << stringtable[data[0x2]] << endl;
    dfile << " data[0x3]: 0x" << hex << data[0x3] << dec << " (string table index) " << stringtable[data[0x3]] << endl;
    dfile << " data[0x4]: 0x" << hex << data[0x4] << dec << " (string table index) " << stringtable[data[0x4]] << endl;
    dfile << " data[0x5]: 0x" << hex << data[0x5] << dec << " (string table index) " << stringtable[data[0x5]] << endl;
    dfile << " data[0x6]: 0x" << hex << data[0x6] << dec << " (string table index) " << stringtable[data[0x6]] << endl;
    dfile << endl;
   }

 // success
 return true;
}

bool ISM2Extractor::processX2E(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x2E UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x2E: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[3];
 if(!BE_read_array(ifile, &header[0], 3)) return error("0x2E: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x2E) return error("0x2E: Invalid node.");
 if(header[1] != 0x0C) return error("0x2E: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x2E: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX30(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x30 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x30: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 p01 = BE_read_uint32(ifile);
 uint32 p02 = BE_read_uint32(ifile);
 uint32 p03 = BE_read_uint32(ifile);
 uint32 p04 = BE_read_uint32(ifile);
 uint32 p05 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("0x30: Read failure.");

 // read matrix
 real32 p06[16];
 if(!BE_read_array(ifile, &p06[0], 16)) return error("0x30: Read failure.");

 // display node header
 if(debug) {
    dfile << " p01: 0x" << hex << p01 << dec << " (signature)" << endl;
    dfile << " p02: 0x" << hex << p02 << dec << " (size of header)" << endl;
    dfile << " p03: 0x" << hex << p03 << dec << " (number of children)" << endl;
    dfile << " p04: 0x" << hex << p04 << dec << " (string table index) " << stringtable[p04] << endl;
    dfile << " p05: 0x" << hex << p05 << dec << " (string table index) " << stringtable[p05] << endl;
    dfile << endl;
   }

 // validate node header
 if(p01 != 0x30) return error("0x30: Invalid node.");
 if(p02 != 0x54) return error("0x30: Invalid node data.");

 // process children
 if(p03) {
    boost::shared_array<uint32> offsets(new uint32[p03]);
    if(!BE_read_array(ifile, offsets.get(), p03)) return error("0x30: Read failure.");
    for(uint32 i = 0; i < p03; i++) if(!processXXX(offsets[i], offset, p01)) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX31(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x31 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x31: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[5];
 if(!BE_read_array(ifile, &header[0], 5)) return error("0x31: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (string table index) " << stringtable[header[0x3]] << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (string table index) " << stringtable[header[0x4]] << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x31) return error("0x31: Invalid node.");
 if(header[1] != 0x14) return error("0x31: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x31: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX32(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "---------------------------" << endl;
    dfile << "- 0x32 SKINNING ROOT NODE -" << endl;
    dfile << "---------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x32: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[5];
 if(!BE_read_array(ifile, &header[0], 5)) return error("0x32: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (???)" << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (???)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x32) return error("0x32: Invalid node.");
 if(header[1] != 0x14) return error("0x32: Invalid node data.");

 // clear previous matrix
 bone_matrix_lst.clear();

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x32: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // COMPLETION
 // Upon completion of processing the 0x32 subtree, we should have a valid array of
 // transformation matrices for all joints.

 // create skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = "skeleton";

 // for each bone matrix
 for(uint32 i = 0; i < bone_matrix_lst.size(); i++)
    {
     // joint index to joint offset
     auto iter1 = bone_idx_to_ofs.find(i);
     if(iter1 == bone_idx_to_ofs.end()) return error("Joint index not found.");

     // get parent joint offset
     auto iter2 = bone_parent_map.find(iter1->second);
     if(iter2 == bone_parent_map.end()) return error("Joint offset not found.");

     // get parent joint index
     uint32 parent = AMC_INVALID_JOINT;
     if(iter2->second == 0x0000) parent = AMC_INVALID_JOINT;
     else {
        auto iter3 = bone_ofs_to_idx.find(iter2->second);
        if(iter3 == bone_ofs_to_idx.end()) {
           stringstream ss;
           ss << "Joint parent offset 0x" << hex << iter2->second << dec << " not found.";
           cout << ss.str() << endl;
           parent = AMC_INVALID_JOINT;
          }
        else parent = iter3->second;
       }

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;
     
     // create joint and set name and ID
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = parent;
     joint.m_rel[0x0] = bone_matrix_lst[i][0x0];
     joint.m_rel[0x1] = bone_matrix_lst[i][0x1];
     joint.m_rel[0x2] = bone_matrix_lst[i][0x2];
     joint.m_rel[0x3] = 0.0f;
     joint.m_rel[0x4] = bone_matrix_lst[i][0x4];
     joint.m_rel[0x5] = bone_matrix_lst[i][0x5];
     joint.m_rel[0x6] = bone_matrix_lst[i][0x6];
     joint.m_rel[0x7] = 0.0f;
     joint.m_rel[0x8] = bone_matrix_lst[i][0x8];
     joint.m_rel[0x9] = bone_matrix_lst[i][0x9];
     joint.m_rel[0xA] = bone_matrix_lst[i][0xA];
     joint.m_rel[0xB] = 0.0f;
     joint.m_rel[0xC] = bone_matrix_lst[i][0xC];
     joint.m_rel[0xD] = bone_matrix_lst[i][0xD];
     joint.m_rel[0xE] = bone_matrix_lst[i][0xE];
     joint.m_rel[0xF] = 0.0f;
     joint.p_rel[0] = -bone_matrix_lst[i][0x3];
     joint.p_rel[1] = -bone_matrix_lst[i][0x7];
     joint.p_rel[2] = -bone_matrix_lst[i][0xB];
     joint.p_rel[3] = 1.0f;
     skel.joints.push_back(joint);
    }

 // save skeleton
 ADVANCEDMODELCONTAINER amc;
 amc.skllist2.push_back(skel);
 string name = shrtname;
 name += "_skel";
 SaveAMC(pathname.c_str(), name.c_str(), amc);

 // success
 return true;
}

bool ISM2Extractor::processX34(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x34 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x34: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // nothing to read here other than the signature
 return true;
}

bool ISM2Extractor::processX44(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "-------------------------" << endl;
    dfile << "- 0x44 BONE INFORMATION -" << endl;
    dfile << "-------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x44: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[8];
 if(!BE_read_array(ifile, &header[0], 8)) return error("0x44: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of items)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (unknown)" << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (type of data 5 (shorts?) or 18 (floats?), 12 (matrix?))" << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (group size (typically 1, 5, 13, or 16))" << endl;
    dfile << " header[0x6]: 0x" << hex << header[0x6] << dec << " (unknown)" << endl;
    dfile << " header[0x7]: 0x" << hex << header[0x7] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x44) return error("0x44: Invalid node.");
 if(header[1] != 0x20) return error("0x44: Invalid node data.");

 // depending on the flags
 uint32 n_items = header[2];
 uint32 type_flag = header[4];
 uint32 size_flag = header[5];

 // read bone string table data
 if(type_flag == 5 && size_flag == 1)
   {
    for(size_t i = 0; i < n_items; i++) {
        uint16 index = BE_read_uint16(ifile);
        if(debug) dfile << " " << index << ", " << stringtable[index] << endl;
       }
    if(debug) dfile << endl;
   }
 // read bone matrices
 else if(type_flag == 12 && size_flag == 16)
   {
    for(size_t i = 0; i < n_items/16; i++) {
        boost::shared_array<real32> matrix(new real32[16]);
        BE_read_array(ifile, matrix.get(), 16);
        bone_matrix_lst.push_back(matrix); // add transformation matrix
        if(debug) {
           dfile << " ";
           for(size_t j = 0; j < 16; j++) dfile << matrix[j] << "  ";
           dfile << endl;
          }
       }
    if(debug) dfile << endl;
   }
 // unknown data
 else
    return error("0x44: Unknown skinning data.");

 // success
 return true;
}

bool ISM2Extractor::processX45(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "---------------------" << endl;
    dfile << "- 0x45 INDEX BUFFER -" << endl;
    dfile << "---------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x45: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[5];
 if(!BE_read_array(ifile, &header[0], 5)) return error("0x45: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of indices)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (data format 5:16-bit and 7:32-bit)" << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x45) return error("0x45: Invalid node.");
 if(header[1] != 0x14) return error("0x45: Invalid node data.");

 // indices must be divisible by 3
 uint32 n_indices = header[2];
 if(n_indices % 3) return error("0x45: The number of face indices must be divisible by three.");

 // save index buffer
 if(header[3] == 5) {
    index_buffer ib;
    ib.bits = 16;
    ib.size = n_indices;
    ib.data.reset(new char[ib.size * sizeof(uint16)]);
    if(!BE_read_array(ifile, reinterpret_cast<uint16*>(ib.data.get()), ib.size)) return error("0x45: Read failure.");
    idxlist.push_back(ib);
   }
 else if(header[3] == 7) {
    index_buffer ib;
    ib.bits = 32;
    ib.size = n_indices;
    ib.data.reset(new char[ib.size * sizeof(uint32)]);
    if(!BE_read_array(ifile, reinterpret_cast<uint32*>(ib.data.get()), ib.size)) return error("0x45: Read failure.");
    idxlist.push_back(ib);
   }
 else
    return error("0x45: Unknown index buffer format.");

 // success
 return true;
}

bool ISM2Extractor::processX46(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------------------" << endl;
    dfile << "- 0x46 SURFACE INFORMATION -" << endl;
    dfile << "----------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x46: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[7];
 if(!BE_read_array(ifile, &header[0], 7)) return error("0x46: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (unknown)" << endl; // skinSG, blinn1SG, etc.
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (unknown)" << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (unknown)" << endl;
    dfile << " header[0x6]: 0x" << hex << header[0x6] << dec << " (number of triangles)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x46) return error("0x46: Invalid node.");
 if(header[1] != 0x1C) return error("0x46: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x46: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX4B(uint32 offset, uint32 parent, uint32 caller)
{
 // 0x4B
 // Contains surface names. Parent is always 0x4C node.
 if(debug) {
    dfile << "---------------------" << endl;
    dfile << "- 0x4B SURFACE NODE -" << endl;
    dfile << "---------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x4B: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[6];
 if(!BE_read_array(ifile, &header[0], 6)) return error("0x4B: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (shader) " << stringtable[header[0x3]] << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (surface name) " << stringtable[header[0x4]] << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x4B) return error("0x4B: Invalid node.");
 if(header[1] != 0x18) return error("0x4B: Invalid node data.");

 // save surface name
 string name = stringtable[header[0x4]];
 if(name.length()) surfacenames.push_back(stringtable[header[0x4]]);
 else return error("0x4B: Invalid surface name.");

 // read data
 if(header[2])
   {
    // read offsets
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, &offsets[0], header[2])) return error("0x4B: Read failure.");

    // process offset data
    for(size_t i = 0; i < header[2]; i++)
       {
        // move to offset
        ifile.seekg(offsets[i]);
        if(ifile.fail()) return error("0x4B: Seek failure.");
   
        // read data
        static const uint32 elem = 7;
        uint32 data[elem];
        if(!BE_read_array(ifile, &data[0], elem)) return error("0x4B: Read failure.");
   
        // debug information
        if(debug) {
           dfile << " DATA" << endl;
           dfile << "  unknown = " << data[0] << endl; // 0x0
           dfile << "  unknown = " << data[1] << endl; // 0xC
           dfile << "  unknown = " << data[2] << endl; // 0x1
           dfile << "  string table index = " << data[3] << ", " << stringtable[data[3]] << endl; // TEX0
           dfile << "  string table index = " << data[4] << ", " << stringtable[data[4]] << endl; // TEXCOORD
           dfile << "  unknown = " << data[5] << endl; // 0x0
           dfile << "  unknown = " << data[6] << endl; // 0x0
           dfile << endl;
          }
       }
   }

 // success
 return true;
}

bool ISM2Extractor::processX4C(uint32 offset, uint32 parent, uint32 caller)
{
 // 0x4C
 // Contains a list of offsets to 0x4B nodes, which contain surface name information.
 // Once finished processing all children, we will have a list of all surface names.
 if(debug) {
    dfile << "---------------------------" << endl;
    dfile << "- 0x4C SURFACE ARRAY NODE -" << endl;
    dfile << "---------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x4C: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[6];
 if(!BE_read_array(ifile, &header[0], 6)) return error("0x4C: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (mesh name) " << stringtable[header[0x3]] << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (unknown)" << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (string table index) " << stringtable[header[0x5]] << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x4C) return error("0x4C: Invalid node.");
 if(header[1] != 0x18) return error("0x4C: Invalid node data.");

 // clear any previous surface names and set new mesh name
 surfacenames.clear();
 meshname = stringtable[header[0x3]];

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x4C: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX4D(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "---------------------------" << endl;
    dfile << "- 0x4D SURFACE ARRAY NODE -" << endl;
    dfile << "---------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x4D: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[5];
 if(!BE_read_array(ifile, &header[0], 5)) return error("0x4D: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (mesh name)" << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x4D) return error("0x4D: Invalid node.");
 if(header[1] != 0x14) return error("0x4D: Invalid node data.");

 // clear any previous surface names and set new mesh name
 // surfacenames.clear();
 // meshname = stringtable[header[0x3]];

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x4D: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX50(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x50 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x50: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[8];
 if(!BE_read_array(ifile, &header[0], 8)) return error("0x50: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of children)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (string table index) " << stringtable[header[0x3]] << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (unknown)" << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (unknown)" << endl;
    dfile << " header[0x6]: 0x" << hex << header[0x6] << dec << " (unknown)" << endl;
    dfile << " header[0x7]: 0x" << hex << header[0x7] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x50) return error("0x50: Invalid node.");
 if(header[1] != 0x20) return error("0x50: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x50: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX59(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------------" << endl;
    dfile << "- 0x59 VERTEX BUFFER -" << endl;
    dfile << "----------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x59: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[7];
 if(!BE_read_array(ifile, &header[0], 7)) return error("0x59: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (type)" << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (number of vertices)" << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (vertex bytes)" << endl;
    dfile << " header[0x6]: 0x" << hex << header[0x6] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x59) return error("0x59: Invalid node.");
 if(header[1] != 0x1C) return error("0x59: Invalid node data.");

 // variables
 uint32 elem = header[2]; // vertex elements
 uint32 type = header[3]; // vertex type
 uint32 n_vertices = header[4];
 uint32 vertex_bytes = header[5];

 // read data
 if(header[2])
   {
    // read offsets
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, &offsets[0], header[2])) return error("0x59: Read failure.");

    // read vertex offsets
    boost::shared_array<boost::shared_array<uint32>> vtxinfo;
    vtxinfo.reset(new boost::shared_array<uint32>[header[2]]);
    for(uint32 i = 0; i < header[2]; i++) {
        ifile.seekg(offsets[i]);
        if(ifile.fail()) return error("0x59: Seek failure.");
        vtxinfo[i].reset(new uint32[6]);
        if(!BE_read_array(ifile, vtxinfo[i].get(), 6)) return error("0x59: Read failure.");
        if(debug) {
           dfile << " VERTEX INFORMATION 0x" << hex << i << dec << endl;
           dfile << " data[0] = 0x" << hex << vtxinfo[i][0] << dec << " (element type)" << endl;
           dfile << " data[1] = 0x" << hex << vtxinfo[i][1] << dec << " (element dimensions)" << endl;
           dfile << " data[2] = 0x" << hex << vtxinfo[i][2] << dec << " (format)" << endl;
           dfile << " data[3] = 0x" << hex << vtxinfo[i][3] << dec << " (vertex stride)" << endl;
           dfile << " data[4] = 0x" << hex << vtxinfo[i][4] << dec << " (element offset)" << endl;
           dfile << " data[5] = 0x" << hex << vtxinfo[i][5] << dec << " (vertex buffer offset)" << endl;
           dfile << endl;
          }
       }

    // VERTEX TYPE: 1
    // POSITION, NORMAL, BINORMAL, TANGENT, UNKNOWN
    if(type == 1)
      {
       // for each vertex element
       for(uint32 i = 0; i < elem; i++)
          {
           // vertex buffer offset + offset to first element
           uint32 position = vtxinfo[i][5];
           position += vtxinfo[i][4];

           // POSITION SEMANTIC
           if(vtxinfo[i][0] == 0x0)
             {
              // must be a vector3D or vector4D
              if((vtxinfo[i][1] != 0x03) && (vtxinfo[i][1] != 0x04))
                 return error("0x59: Invalid POSITION dimensions.");

              // create point array
              point3D_array<real32> plist;
              plist.resize(n_vertices);

              // read point array
              for(uint32 j = 0; j < n_vertices; j++)
                 {
                  // seek element position
                  ifile.seekg(position);
                  if(ifile.fail()) return error("0x59: Seek failure.");

                  // read position
                  if(vtxinfo[i][2] == 0x0C) {
                     plist[j][0] = BE_read_real32(ifile);
                     plist[j][1] = BE_read_real32(ifile);
                     plist[j][2] = BE_read_real32(ifile);
                    }
                  else if(vtxinfo[i][2] == 0x12) {
                     plist[j][0] = BE_read_real16(ifile);
                     plist[j][1] = BE_read_real16(ifile);
                     plist[j][2] = BE_read_real16(ifile);
                    }
                  else
                     return error("0x59: Invalid POSITION type.");

                  // move to next element
                  position += vtxinfo[i][3];
                 }

              // set position data
              pntbuffer = plist;
             }
           // NORMAL SEMANTIC
           else if(vtxinfo[i][0] == 0x2)
             {
              // must be a vector3D or vector4D
              if((vtxinfo[i][1] != 0x03) && (vtxinfo[i][1] != 0x04))
                 return error("0x59: Invalid NORMAL dimensions.");

              // create point array
              point3D_array<real32> plist;
              plist.resize(n_vertices);

              // read point array
              for(uint32 j = 0; j < n_vertices; j++)
                 {
                  // seek element position
                  ifile.seekg(position);
                  if(ifile.fail()) return error("0x59: Seek failure.");
   
                  // read position
                  if(vtxinfo[i][2] == 0x0C) {
                     plist[j][0] = BE_read_real32(ifile);
                     plist[j][1] = BE_read_real32(ifile);
                     plist[j][2] = BE_read_real32(ifile);
                    }
                  else if(vtxinfo[i][2] == 0x12) {
                     plist[j][0] = BE_read_real16(ifile);
                     plist[j][1] = BE_read_real16(ifile);
                     plist[j][2] = BE_read_real16(ifile);
                    }
                  else
                     return error("0x59: Invalid POSITION type.");

                  // move to next element
                  position += vtxinfo[i][3];
                 }

              // set normal data
              norbuffer = plist;
             }
           // UNKNOWN SEMANTIC
           else if(vtxinfo[i][0] == 0x3) // try color?
             {
              // must be a vector3D or vector4D
              if(vtxinfo[i][1] != 0x04)
                 return error("0x59: Invalid COLOR dimensions.");

              // create point array
              point4D_array<real32> plist;
              plist.resize(n_vertices);

              // read point array
              for(uint32 j = 0; j < n_vertices; j++)
                 {
                  // seek element position
                  ifile.seekg(position);
                  if(ifile.fail()) return error("0x59: Seek failure.");
   
                  // read position
                  if(vtxinfo[i][2] == 0x03) {
                     plist[j][0] = BE_read_uint08(ifile)/255.0f;
                     plist[j][1] = BE_read_uint08(ifile)/255.0f;
                     plist[j][2] = BE_read_uint08(ifile)/255.0f;
                     plist[j][3] = BE_read_uint08(ifile)/255.0f;
                    }
                  else if(vtxinfo[i][2] == 0x0C) {
                     plist[j][0] = BE_read_real32(ifile);
                     plist[j][1] = BE_read_real32(ifile);
                     plist[j][2] = BE_read_real32(ifile);
                     plist[j][3] = BE_read_real32(ifile);
                    }
                  else if(vtxinfo[i][2] == 0x12) {
                     plist[j][0] = BE_read_real16(ifile);
                     plist[j][1] = BE_read_real16(ifile);
                     plist[j][2] = BE_read_real16(ifile);
                     plist[j][3] = BE_read_real16(ifile);
                    }
                  else
                     return error("0x59: Invalid COLOR type.");

                  // move to next element
                  position += vtxinfo[i][3];
                 }

              // set color data
              colbuffer = plist;
             }
           // UNKNOWN SEMANTIC
           else if(vtxinfo[i][0] == 0xe)
             {
             }
           // UNKNOWN SEMANTIC
           else return error("0x59: Unknown POSITION SEMANTIC.");
          }
      }
    // VERTEX TYPE: 2
    // TEXCOORD
    else if(type == 2)
      {
       // for each vertex element
       for(uint32 i = 0; i < elem; i++)
          {
           // vertex buffer offset + offset to first element
           uint32 position = vtxinfo[i][5];
           position += vtxinfo[i][4];

           // TEXCOORD SEMANTIC
           if(vtxinfo[i][0] == 0x08)
             {
              // must be a vector2D
              if(vtxinfo[i][1] != 0x02)
                 return error("0x59: Invalid TEXCOORD dimensions.");

              // create point array
              point2D_array<real32> plist;
              plist.resize(n_vertices);

              // read point array
              for(uint32 j = 0; j < n_vertices; j++)
                 {
                  // seek element position
                  ifile.seekg(position);
                  if(ifile.fail()) return error("0x59: Seek failure.");

                  // read texture coordinates
                  if(vtxinfo[i][2] == 0x0C) {
                     plist[j][0] = BE_read_real32(ifile);
                     plist[j][1] = BE_read_real32(ifile);
                    }
                  else if(vtxinfo[i][2] == 0x12) {
                     plist[j][0] = BE_read_real16(ifile);
                     plist[j][1] = BE_read_real16(ifile);
                    }
                  else
                     return error("0x59: Invalid TEXCOORD type.");

                  // move to next element
                  position += vtxinfo[i][3];
                 }

              // set texture coordinate data
              texbuffer.push_back(plist);
             }
           else
             return error("0x59: Unknown TEXCOORD SEMANTIC");
          }
      }
    // VERTEX TYPE: 3
    // BLENDWEIGHTS AND BLENDINDICES
    else if(type == 3)
      {
       // must have indices and weights
       if(elem != 0x02)
          return error("0x59: Must have blend weights and blend indices.");

       // dimensions must match
       if(vtxinfo[0][1] != vtxinfo[1][1])
          return error("0x59: Blend weight and blend index dimensions do not match.");

       // for each vertex element
       for(uint32 i = 0; i < elem; i++)
          {
           // vertex buffer offset + offset to first element
           uint32 position = vtxinfo[i][5];
           position += vtxinfo[i][4];

           // BLENDWEIGHT SEMANTIC
           if(vtxinfo[i][0] == 0x01)
             {
              // must be a vector4D or less
              if(vtxinfo[i][1] > 0x04)
                 return error("0x59: Invalid BLENDWEIGHT dimensions.");

              // create point array
              point4D_array<real32> plist(n_vertices, 0.0f);

              // read point array
              for(uint32 j = 0; j < n_vertices; j++)
                 {
                  // seek element position
                  ifile.seekg(position);
                  if(ifile.fail()) return error("0x59: Seek failure.");

                  // read blendweights
                  if(vtxinfo[i][2] == 0x0C) {
                     for(uint32 k = 0; k < vtxinfo[i][1]; k++) 
                         plist[j][k] = BE_read_real32(ifile);
                    }
                  else if(vtxinfo[i][2] == 0x12) {
                     for(uint32 k = 0; k < vtxinfo[i][1]; k++) 
                         plist[j][k] = BE_read_real16(ifile);
                    }
                  else
                     return error("0x59: Invalid BLENDWEIGHT type.");

                  // move to next element
                  position += vtxinfo[i][3];
                 }

              // set blendweight data
              blwbuffer = plist;
             }
           // BLENDINDEX SEMANTIC
           else if(vtxinfo[i][0] == 0x07)
             {
              // must be a vector4D or less
              if(vtxinfo[i][1] > 0x04)
                 return error("0x59: Invalid BLENDINDEX dimensions.");

              // create point array
              point4D_array<uint16> plist(n_vertices, AMC_INVALID_VERTEX_WMAP_INDEX);

              // read point array
              for(uint32 j = 0; j < n_vertices; j++)
                 {
                  // seek element position
                  ifile.seekg(position);
                  if(ifile.fail()) return error("0x59: Seek failure.");

                  // read blendindices
                  if(vtxinfo[i][2] == 0x05) {
                     for(uint32 k = 0; k < vtxinfo[i][1]; k++) 
                         plist[j][k] = BE_read_uint16(ifile);
                    }
                  else
                     return error("0x59: Invalid BLENDINDEX type.");

                  // move to next element
                  position += vtxinfo[i][3];
                 }

              // set blendweight data
              blibuffer = plist;
             }
           else
             return error("0x59: Unknown BLEND SEMANTIC");
          }
      }
   }

 // success
 return true;
}

bool ISM2Extractor::processX5B(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "------------------------" << endl;
    dfile << "- 0x5B BONE TRANSFORMS -" << endl;
    dfile << "------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x5B: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[3];
 if(!BE_read_array(ifile, &header[0], 3)) return error("0x5B: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x5B) return error("0x5B: Invalid node.");
 if(header[1] != 0x0C) return error("0x5B: Invalid node data.");

 // read data
 if(header[2])
   {
    // read offsets
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, &offsets[0], header[2])) return error("0x5B: Read failure.");

    // TODO
    // PROCESS TRANSFORM DATA
   }

 // success
 return true;
}

bool ISM2Extractor::processX5C(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "------------------------" << endl;
    dfile << "- 0x5C BONE REFERENCES -" << endl;
    dfile << "------------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x5C: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[3];
 if(!BE_read_array(ifile, &header[0], 3)) return error("0x5C: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x5C) return error("0x5C: Invalid node.");
 if(header[1] != 0x0C) return error("0x5C: Invalid node data.");

 // read data
 if(header[2])
   {
    // read offsets
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, &offsets[0], header[2])) return error("0x5C: Read failure.");

    // TODO
    // PROCESS BONE REFERENCE DATA
    // THESE OFFSETS POINT TO NODES OF TYPE 0x05 THAT HAVE ALREADY BEEN PROCESSED
    // DO NOT PROCESS THESE OFFSETS AGAIN!!!
   }

 // success
 return true;
}

bool ISM2Extractor::processX61(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x61 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x61: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[3];
 if(!BE_read_array(ifile, &header[0], 3)) return error("0x61: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x61) return error("0x61: Invalid node.");
 if(header[1] != 0x0C) return error("0x61: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x61: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX62(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x62 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x62: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[3];
 if(!BE_read_array(ifile, &header[0], 3)) return error("0x62: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x62) return error("0x62: Invalid node.");
 if(header[1] != 0x0C) return error("0x62: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x62: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX63(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x63 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x63: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[11];
 if(!BE_read_array(ifile, &header[0], 11)) return error("0x63: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (unknown)" << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (unknown)" << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x5] << dec << " (unknown)" << endl;
    dfile << " header[0x6]: 0x" << hex << header[0x6] << dec << " (unknown)" << endl;
    dfile << " header[0x7]: 0x" << hex << header[0x7] << dec << " (unknown)" << endl;
    dfile << " header[0x8]: 0x" << hex << header[0x8] << dec << " (unknown)" << endl;
    dfile << " header[0x9]: 0x" << hex << header[0x9] << dec << " (unknown)" << endl;
    dfile << " header[0xA]: 0x" << hex << header[0xA] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x63) return error("0x63: Invalid node.");
 if(header[1] != 0x2C) return error("0x63: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x63: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX65(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x65 UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x65: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[4];
 if(!BE_read_array(ifile, &header[0], 4)) return error("0x65: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (unknown)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x65) return error("0x65: Invalid node.");
 if(header[1] != 0x10) return error("0x65: Invalid node data.");

 // read data
 if(header[2])
   {
    // read offsets
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, &offsets[0], header[2])) return error("0x65: Read failure.");

    // TODO
    // PROCESS DATA
    // NOT SURE WHAT THIS DATA IS!!!
   }

 // success
 return true;
}

bool ISM2Extractor::processX6A(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x6A UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x6A: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[5];
 if(!BE_read_array(ifile, &header[0], 5)) return error("0x6A: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (string table index) " << stringtable[header[0x3]] << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (string table index) " << stringtable[header[0x4]] << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x6A) return error("0x6A: Invalid node.");
 if(header[1] != 0x14) return error("0x6A: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x6A: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX6B(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x6B UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x6B: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[6];
 if(!BE_read_array(ifile, &header[0], 6)) return error("0x6B: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (type)" << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (elem)" << endl;
    dfile << " header[0x5]: 0x" << hex << header[0x2] << dec << " (0x6A offset)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x6B) return error("0x6B: Invalid node.");
 if(header[1] != 0x18) return error("0x6B: Invalid node data.");

 // process children
 uint32 n_offsets = header[2];
 uint32 type = header[3];
 uint32 elem = header[4];
 if(n_offsets)
   {
    // read offsets
    boost::shared_array<uint32> offsets(new uint32[n_offsets]);
    if(!BE_read_array(ifile, offsets.get(), n_offsets)) return error("0x6B: Read failure.");

    // process children
    if((type == 0x02) || (type == 0x06))
      {
       for(uint32 i = 0; i < n_offsets; i++)
           if(!processXXX(offsets[i], offset, header[0]))
              return false;
      }
    // process data (string table indices)
    else if(type == 0x15)
      {
       boost::shared_array<uint32> data(new uint32[elem]);
       BE_read_array(ifile, data.get(), elem);
       if(debug) {
          dfile << " DATA TYPE 0x" << hex << type << dec << endl;
          for(uint32 i = 0; i < elem; i++) dfile << "  surface[" << i << "] = " << data[i] << ", " << stringtable[data[i]] << endl;
          dfile << endl;
         }
      }
    // process data (string table indices)
    else if(type == 0x18)
      {
       boost::shared_array<uint32> data(new uint32[elem]);
       BE_read_array(ifile, data.get(), elem);
       if(debug) {
          dfile << " DATA TYPE 0x" << hex << type << dec << endl;
          for(uint32 i = 0; i < elem; i++) dfile << "  image_format[" << i << "] = " << data[i] << ", " << stringtable[data[i]] << endl;
          dfile << endl;
         }
      }
    // process data (numbers)
    else
      {
       boost::shared_array<uint32> data(new uint32[elem]);
       BE_read_array(ifile, data.get(), elem);
       if(debug) {
          dfile << " DATA TYPE 0x" << hex << type << dec << endl;
          for(uint32 i = 0; i < elem; i++) dfile << "  data[" << i << "] = " << data[i] << endl;
          dfile << endl;
         }
      }
   }

 // success
 return true;
}

bool ISM2Extractor::processX6C(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "- 0x6C UNKNOWN -" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x6C: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[3];
 if(!BE_read_array(ifile, &header[0], 3)) return error("0x6C: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x6C) return error("0x6C: Invalid node.");
 if(header[1] != 0x0C) return error("0x6C: Invalid node data.");

 // process children
 if(header[2]) {
    boost::shared_array<uint32> offsets(new uint32[header[2]]);
    if(!BE_read_array(ifile, offsets.get(), header[2])) return error("0x6C: Read failure.");
    for(uint32 i = 0; i < header[2]; i++) if(!processXXX(offsets[i], offset, header[0])) return false;
   }

 // success
 return true;
}

bool ISM2Extractor::processX6E(uint32 offset, uint32 parent, uint32 caller)
{
 if(debug) {
    dfile << "--------------------" << endl;
    dfile << "- 0x6E BOUDING BOX -" << endl;
    dfile << "--------------------" << endl;
    dfile << endl;
   }

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("0x6E: Seek failure.");

 // display section and offset
 if(debug) {
    dfile << " node offset: 0x" << setfill('0') << setw(8) << hex << offset << dec << endl;
    dfile << " node parent: 0x" << setfill('0') << setw(8) << hex << parent << dec << endl;
    dfile << " node caller: 0x" << setfill('0') << setw(2) << hex << caller << dec << endl;
    dfile << endl;
   }

 // read node header
 uint32 header[4];
 if(!BE_read_array(ifile, &header[0], 4)) return error("0x6E: Read failure.");

 // read bounding box
 real32 min_bb[4];
 real32 max_bb[4];
 if(!BE_read_array(ifile, &min_bb[0], 4)) return error("0x6E: Read failure.");
 if(!BE_read_array(ifile, &max_bb[0], 4)) return error("0x6E: Read failure.");

 // display node header
 if(debug) {
    dfile << " header[0x0]: 0x" << hex << header[0x0] << dec << " (signature)" << endl;
    dfile << " header[0x1]: 0x" << hex << header[0x1] << dec << " (size of header)" << endl;
    dfile << " header[0x2]: 0x" << hex << header[0x2] << dec << " (number of offsets)" << endl;
    dfile << " header[0x3]: 0x" << hex << header[0x3] << dec << " (unknown)" << endl;
    dfile << " header[0x4]: 0x" << hex << header[0x4] << dec << " (unknown)" << endl;
    dfile << " min_bb[0x0]: " << min_bb[0x0] << endl;
    dfile << " min_bb[0x1]: " << min_bb[0x1] << endl;
    dfile << " min_bb[0x2]: " << min_bb[0x2] << endl;
    dfile << " min_bb[0x3]: " << min_bb[0x3] << endl;
    dfile << " max_bb[0x0]: " << max_bb[0x0] << endl;
    dfile << " max_bb[0x1]: " << max_bb[0x1] << endl;
    dfile << " max_bb[0x2]: " << max_bb[0x2] << endl;
    dfile << " max_bb[0x3]: " << max_bb[0x3] << endl;
    dfile << endl;
   }

 // validate node header
 if(header[0] != 0x6E) return error("0x6E: Invalid node.");
 if(header[1] != 0x30) return error("0x6E: Invalid node data.");

 // success
 return true;
}

bool ISM2Extractor::extract(const string& fname)
{
 // filename properties
 filename = fname;
 pathname = GetPathnameFromFilename(filename);
 shrtname = GetShortFilenameWithoutExtension(filename);

 // open file
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open ISM2 file.");

 // filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // debug file
 if(debug) {
    string dfname = pathname;
    dfname += shrtname;
    dfname += ".txt";
    dfile.open(dfname.c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // read header
 uint32 head01 = BE_read_uint32(ifile); // magic
 uint32 head02 = BE_read_uint32(ifile); // ???
 uint32 head03 = BE_read_uint32(ifile); // ???
 uint32 head04 = BE_read_uint32(ifile); // 0x0
 uint32 head05 = BE_read_uint32(ifile); // filesize
 uint32 head06 = BE_read_uint32(ifile); // number of root nodes
 uint32 head07 = BE_read_uint32(ifile); // 0x0
 uint32 head08 = BE_read_uint32(ifile); // 0x0

 if(debug) {
    dfile << "----------" << endl;
    dfile << "- HEADER -" << endl;
    dfile << "----------" << endl;
    dfile << endl;
    dfile << " filesize = 0x" << hex << head05 << dec << endl;
    dfile << " number of root nodes = 0x" << hex << head06 << dec << endl;
    dfile << endl;
   }

 // read root offsets
 // must do this separately because once depth-first search kicks the file pointer
 // moves around like crazy
 boost::shared_array<uint32> offsets(new uint32[head06]);
 for(uint32 i = 0; i < head06; i++) {
     uint32 p01 = BE_read_uint32(ifile); // signature
     uint32 p02 = BE_read_uint32(ifile); // offset
     offsets[i] = p02;
    }

 // process root sections (caller = parent signature)
 for(uint32 i = 0; i < head06; i++)
     if(!processXXX(offsets[i], 0xFFFFFFFF, 0xFFFFFFFF)) return false;

 // success
 return true;
}

void ISM2Extractor::clean(void)
{
}

bool CompileHeartConvertISM2(const std::string& filename)
{
 ISM2Extractor obj;
 return obj.extract(filename);
}

*/