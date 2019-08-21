#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_zlib.h"
#include "[PC] RE7.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   RE7

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool ProcessPAK(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 magic = LE_read_uint32(ifile);
 uint32 unk01 = LE_read_uint32(ifile);
 uint32 files = LE_read_uint32(ifile);
 uint32 unk02 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // validate header
 if(magic != 0x414B504B) return error("Not a KPKA file.", __LINE__);
 if(unk01 != 0x04) return error("Not a KPKA file.", __LINE__);
 if(files == 0x00) return true; // nothing to do;

 struct RE7PAKENTRY {
  uint64 hash1;
  uint64 offset;
  uint64 deflate_size;
  uint64 inflate_size;
  uint64 iscompressed;
  uint64 hash2;
 };
 std::vector<RE7PAKENTRY> entrylist(files);

 // read entries
 for(uint32 i = 0; i < files; i++)
    {
     // read entry
     boost::shared_array<char> entry(new char[0x30]);
     ifile.read((char*)entry.get(), 0x30);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // create binary stream
     binary_stream bs(entry, 0x30);
     bs.set_endian_mode(ENDIAN_LITTLE);
     entrylist[i].hash1 = bs.read_uint64();
     entrylist[i].offset = bs.read_uint64();
     entrylist[i].deflate_size = bs.read_uint64();
     entrylist[i].inflate_size = bs.read_uint64();
     entrylist[i].iscompressed = bs.read_uint64();
     entrylist[i].hash2 = bs.read_uint64();
     if(bs.fail()) return error("Stream read failure.", __LINE__);
    }

 // create save path
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += L"\\";
 CreateDirectory(savepath.c_str(), NULL);

 // read data
 for(uint32 i = 0; i < files; i++)
    {
     // display information
     cout << " Saving file " << i << " of " << files << "." << endl;
     // cout << "  offset = 0x" << hex << entrylist[i].offset << dec << "." << endl;
     // cout << "  deflate_size = 0x" << hex << entrylist[i].deflate_size << dec << "." << endl;
     // cout << "  inflate_size = 0x" << hex << entrylist[i].inflate_size << dec << "." << endl;
     // cout << "  iscompressed = 0x" << hex << entrylist[i].iscompressed << dec << "." << endl;

     // move to data
     ifile.seekg(entrylist[i].offset);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // read data
     unique_ptr<char[]> data(new char[static_cast<uint32>(entrylist[i].deflate_size)]);
     ifile.read(data.get(), entrylist[i].deflate_size);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // create filename
     STDSTRINGSTREAM ss;
     ss << setfill(TEXT('0'));
     ss << savepath.c_str() << setw(6) << i << TEXT(".re7");

     // create files
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.", __LINE__);

     // no compression
     if(entrylist[i].iscompressed == 0)
       {
        ofile.write(data.get(), entrylist[i].inflate_size);
        if(ofile.fail()) return error("Write failure.", __LINE__);
       }
     // zlib
     else if(entrylist[i].iscompressed == 1)
       {
        // initialize ZLIB entries
        ZLIBINFO2 zi;
        zi.deflatedBytes = static_cast<uint32>(entrylist[i].deflate_size); // deflated size
        zi.inflatedBytes = static_cast<uint32>(entrylist[i].inflate_size); // inflated size
        zi.offset = entrylist[i].offset; // offset to data
        // set ZLIB entries
        std::deque<ZLIBINFO2> zlist;
        zlist.push_back(zi);
        // inflate
        if(!InflateZLIB(ifile, zlist, -15, ofile)) return error("ZLIB inflation failed.", __LINE__);
       }
     // unknown
     else
        return error("Unknown PAK compression type.", __LINE__);
    }

 return true;
}

bool ProcessRE7(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 magic1 = LE_read_uint32(ifile);
 uint32 magic2 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // close file
 ifile.close();

 // special cases
 if(magic1 == 0x00530040 && magic2 == 0x0075006F) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("bnk")).get());
    return true;
   }
 else if(magic2 == 0x6E616863) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("chan")).get());
    return true;
   }
 else if(magic2 == 0x74736C63) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("clst")).get());
    return true;
   }
 else if(magic2 == 0x47464347) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("gcfg")).get());
    return true;
   }
 else if(magic2 == 0x47534D47) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("gmsg")).get());
    return true;
   }
 else if(magic2 == 0x52495547) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("guir")).get());
    return true;
   }
 else if(magic2 == 0x544E4649) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("ifnt")).get());
    return true;
   }
 else if(magic2 == 0x70797466) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("ftyp")).get());
    return true;
   }
 else if(magic2 == 0x70616D6A) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("jmap")).get());
    return true;
   }
 else if(magic2 == 0x6B6E626D) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("mbnk")).get());
    return true;
   }
 else if(magic2 == 0x6D61636D) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("mcam")).get());
    return true;
   }
 else if(magic2 == 0x6D73666D) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("mfsm")).get());
    return true;
   }
 else if(magic2 == 0x74736C6D) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("mlst")).get());
    return true;
   }
 else if(magic2 == 0x20746F6D) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("mot")).get());
    return true;
   }
 else if(magic2 == 0x72617675) {
    MoveFile(filename, ChangeFileExtension(filename, TEXT("uvar")).get());
    return true;
   }

 // rename file
 switch(magic1) {
   case(0x504D4941) : MoveFile(filename, ChangeFileExtension(filename, TEXT("aimp")).get()); break; // navigation geometry?
   case(0x4B504B41) : MoveFile(filename, ChangeFileExtension(filename, TEXT("akpk")).get()); break;
   case(0x44484B42) : MoveFile(filename, ChangeFileExtension(filename, TEXT("bkhd")).get()); break; // sound
   case(0x46454443) : MoveFile(filename, ChangeFileExtension(filename, TEXT("cdef")).get()); break;
   case(0x4C494643) : MoveFile(filename, ChangeFileExtension(filename, TEXT("cfill")).get()); break;
   case(0x50494C43) : MoveFile(filename, ChangeFileExtension(filename, TEXT("clip")).get()); break;
   case(0x004F4C43) : MoveFile(filename, ChangeFileExtension(filename, TEXT("clo")).get()); break; // cloth (havok)
   case(0x54414D43) : MoveFile(filename, ChangeFileExtension(filename, TEXT("cmat")).get()); break;
   case(0x20464544) : MoveFile(filename, ChangeFileExtension(filename, TEXT("def")).get()); break;
   case(0x72786665) : MoveFile(filename, ChangeFileExtension(filename, TEXT("efxr")).get()); break;
   case(0x4F464246) : MoveFile(filename, ChangeFileExtension(filename, TEXT("fbfo")).get()); break;
   case(0x544C4946) : MoveFile(filename, ChangeFileExtension(filename, TEXT("filt")).get()); break;
   case(0x54435846) : MoveFile(filename, ChangeFileExtension(filename, TEXT("fxct")).get()); break;
   case(0x4D534648) : MoveFile(filename, ChangeFileExtension(filename, TEXT("hfsm")).get()); break;
   case(0x00534549) : MoveFile(filename, ChangeFileExtension(filename, TEXT("ies")).get()); break;
   case(0x00444F4C) : MoveFile(filename, ChangeFileExtension(filename, TEXT("lod")).get()); break;
   case(0x4C4F434D) : MoveFile(filename, ChangeFileExtension(filename, TEXT("mcol")).get()); break;
   case(0x0046444D) : MoveFile(filename, ChangeFileExtension(filename, TEXT("mdf")).get()); break; // materials
   case(0x4853454D) : MoveFile(filename, ChangeFileExtension(filename, TEXT("mesh")).get()); break; // model
   case(0x4252504E) : MoveFile(filename, ChangeFileExtension(filename, TEXT("nprb")).get()); break;
   case(0x00424650) : MoveFile(filename, ChangeFileExtension(filename, TEXT("pfb")).get()); break;
   case(0x00444252) : MoveFile(filename, ChangeFileExtension(filename, TEXT("rbd")).get()); break;
   case(0x004C4452) : MoveFile(filename, ChangeFileExtension(filename, TEXT("rdl")).get()); break;
   case(0x4C4F4352) : MoveFile(filename, ChangeFileExtension(filename, TEXT("rcol")).get()); break;
   case(0x564D4552) : MoveFile(filename, ChangeFileExtension(filename, TEXT("remv")).get()); break;
   case(0x005A5352) : MoveFile(filename, ChangeFileExtension(filename, TEXT("rsz")).get()); break;
   case(0x58455452) : MoveFile(filename, ChangeFileExtension(filename, TEXT("rtex")).get()); break;
   case(0x004E4353) : MoveFile(filename, ChangeFileExtension(filename, TEXT("scn")).get()); break; // scenes
   case(0x00464453) : MoveFile(filename, ChangeFileExtension(filename, TEXT("sdf")).get()); break;
   case(0x00535353) : MoveFile(filename, ChangeFileExtension(filename, TEXT("sss")).get()); break;
   case(0x5556532E) : MoveFile(filename, ChangeFileExtension(filename, TEXT("svu")).get()); break; // references texture file
   case(0x52524554) : MoveFile(filename, ChangeFileExtension(filename, TEXT("terr")).get()); break; // terrain???
   case(0x00584554) : MoveFile(filename, ChangeFileExtension(filename, TEXT("tex")).get()); break; // texture
   case(0x00444957) : MoveFile(filename, ChangeFileExtension(filename, TEXT("wid")).get()); break;
   //default : return error("Unknown file.", __LINE__);
  }
 return true;
}

}}

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool ProcessTEX(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 magic = LE_read_uint32(ifile);
 uint32 unk01 = LE_read_uint32(ifile);
 uint16 dx = LE_read_uint16(ifile);
 uint16 dy = LE_read_uint16(ifile);
 uint08 flag1 = LE_read_uint08(ifile);
 uint08 flag2 = LE_read_uint08(ifile);
 uint08 flag3 = LE_read_uint08(ifile); // number of entries
 uint08 flag4 = LE_read_uint08(ifile);
 uint32 unk02 = LE_read_uint32(ifile);
 uint32 unk03 = LE_read_uint32(ifile); // 0xFFFFFFFF
 uint32 unk04 = LE_read_uint32(ifile); // 0x0
 uint32 unk05 = LE_read_uint32(ifile); // 0x0 or 0x1
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // validate header
 if(magic != 0x00584554) return error("Invalid magic number.", __LINE__);
 if(unk01 != 0x08) return error("Should be 0x08.", __LINE__);
 if(flag3 == 0) return true; // nothing to do
 if(unk03 != 0xFFFFFFFF) return error("Should be 0xFFFFFFFF.", __LINE__);

 // cout << "unk02 = 0x" << hex << unk02 << dec << " flags = " << (uint16)flag1 << " " << (uint16)flag2 << " " << (uint16)flag4 << endl;

 // read entries
 struct RE7TEXTUREITEM {
  uint64 offset;
  uint32 unk01;
  uint32 size;
 };
 uint32 n_items = flag3;
 vector<RE7TEXTUREITEM> itemlist(n_items);
 for(uint32 i = 0; i < n_items; i++) {
     itemlist[i].offset = LE_read_uint64(ifile);
     itemlist[i].unk01 = LE_read_uint32(ifile);
     itemlist[i].size = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
    }

 // move to offset
 ifile.seekg(itemlist[0].offset);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read data
 boost::shared_array<char> data(new char[itemlist[0].size]);
 ifile.read(data.get(), itemlist[0].size);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // create output filename
 STDSTRING ofname = pathname;
 ofname += shrtname;
 ofname += TEXT(".dds");

 // 01 00 07 01 - 01 00 00 00 = BC7
 // 63 00 00 00 = 01100011 = BC7
 // 62 00 00 00 = 01100010 = BC7
 // 47 00 00 00 = 01000111 = DXT1
 // 48 00 00 00 = 01001000 = DXT1

 // RGBA FLOAT (WORKS)
 if(unk02 == 0x02) {
    uint32 convsize = 4*dx*dy;
    boost::shared_array<char> conv(new char[convsize]);
    R32G32B32A32FloatToR8G8B8A8Byte(conv.get(), data.get(), dx, dy);
    DDS_HEADER ddsh;
    CreateR8G8B8A8DDSHeader(dx, dy, 0, FALSE, &ddsh);
    if(!SaveDDSFile(ofname.c_str(), ddsh, conv, convsize)) return error("Failed to save DDS file.", __LINE__);
   }
 // RGBA HALF-FLOAT (WORKS)
 else if(unk02 == 0x0A) {
    uint32 convsize = 4*dx*dy;
    boost::shared_array<char> conv(new char[convsize]);
    R16G16B16A16FloatToR8G8B8A8Byte(conv.get(), data.get(), dx, dy);
    DDS_HEADER ddsh;
    CreateR8G8B8A8DDSHeader(dx, dy, 0, FALSE, &ddsh);
    if(!SaveDDSFile(ofname.c_str(), ddsh, conv, convsize)) return error("Failed to save DDS file.", __LINE__);
   }
 // RGBA 4x4 (WORKS)
 else if(unk02 == 0x1C || unk02 == 0x1D) {
    DDS_HEADER ddsh;
    CreateR8G8B8A8DDSHeader(dx, dy, 0, FALSE, &ddsh);
    if(!SaveDDSFile(ofname.c_str(), ddsh, data, itemlist[0].size)) return error("Failed to save DDS file.", __LINE__);
   }
 // DXT1 (WORKS)
 else if(unk02 == 0x47 || unk02 == 0x48) {
    DDS_HEADER ddsh;
    if(!CreateDXT1Header(dx, dy, 0, FALSE, &ddsh)) return error("Failed to create DDS file header.", __LINE__);
    if(!SaveDDSFile(ofname.c_str(), ddsh, data, itemlist[0].size)) return error("Failed to save DDS file.", __LINE__);
   }
 // BC4/ATI1
 else if(unk02 == 0x50) {
    DDS_HEADER ddsh;
    if(!CreateATI1Header(dx, dy, 0, FALSE, &ddsh)) return error("Failed to create DDS file header.", __LINE__);
    // create output filename
    STDSTRING ofname = pathname;
    ofname += shrtname;
    ofname += TEXT("_ATI1.dds");
    if(!SaveDDSFile(ofname.c_str(), ddsh, data, itemlist[0].size)) return error("Failed to save DDS file.", __LINE__);
   }
 // BC5/ATI2 (WORKS)
 else if(unk02 == 0x53) {
    DDS_HEADER ddsh;
    // create output filename
    STDSTRING ofname = pathname;
    ofname += shrtname;
    ofname += TEXT("_ATI2.dds");
    if(!CreateATI2Header(dx, dy, 0, FALSE, &ddsh)) return error("Failed to create DDS file header.", __LINE__);
    if(!SaveDDSFile(ofname.c_str(), ddsh, data, itemlist[0].size)) return error("Failed to save DDS file.", __LINE__);
   }
 // BC6H (WORKS)
 else if(unk02 == 0x5F) {
    // DDS_HEADER ddsh;
    // if(!CreateDXT5Header(dx, dy, 0, FALSE, &ddsh)) return error("Failed to create DDS file header.", __LINE__);
    // if(!SaveDDSFile(ofname.c_str(), ddsh, data, itemlist[0].size)) return error("Failed to save DDS file.", __LINE__);
   }
 // BC6S (WORKS)
 else if(unk02 == 0x60) {
    // DDS_HEADER ddsh;
    // if(!CreateDXT5Header(dx, dy, 0, FALSE, &ddsh)) return error("Failed to create DDS file header.", __LINE__);
    // if(!SaveDDSFile(ofname.c_str(), ddsh, data, itemlist[0].size)) return error("Failed to save DDS file.", __LINE__);
   }
 // BC7 (WORKS)
 // NVIDIA PHOTOSHOP ONLY WORKS WITH A8R8G8B8 DDS mask!
 else if(unk02 == 0x62 || unk02 == 0x63) {
    uint32 convsize = 4*dx*dy;
    boost::shared_array<char> conv(new char[convsize]);
    BC7ToB8G8R8A8Byte(conv.get(), data.get(), dx, dy);
    DDS_HEADER ddsh;
    CreateA8R8G8B8DDSHeader(dx, dy, 0, FALSE, &ddsh);
    if(!SaveDDSFile(ofname.c_str(), ddsh, conv, convsize)) return error("Failed to save DDS file.", __LINE__);
   }
 else
    return error("Unknown texture format.", __LINE__);

 return true;
}

bool ProcessFBF(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 magic = LE_read_uint32(ifile);
 if(magic != 0x4F464246) return error("Not an FBF file.", __LINE__);
 ifile.seekg(8);

 uint32 dx = 32;
 uint32 dy = 32;
 uint32 datasize = 4*dx*dy;

 // read data
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // create output filename
 STDSTRING ofname = pathname;
 ofname += shrtname;
 ofname += TEXT(".dds");

 // convert
 DDS_HEADER ddsh;
 CreateR8G8B8A8DDSHeader(dx, dy, 0, FALSE, &ddsh);
 if(!SaveDDSFile(ofname.c_str(), ddsh, data, datasize)) return error("Failed to save DDS file.", __LINE__);

 return true;
}

}}

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

struct RE7HEADER {
 uint32 h01; // magic
 uint32 h02; // model flags?
 uint32 h03; // ???
 uint16 h04; // ???
 uint16 h05; // number of strings in stringtable
 uint64 h06; // offset to 1st section
 uint64 h07; // offset to 2nd section
 uint64 h08; // ??? 0x00
 uint64 h09; // offset to skeleton
 uint64 h10; // ??? 0x00
 uint64 h11; // offset to skeleton-related data
 uint64 h12; // offset to vertex buffer
 uint64 h13; // ??? 0x00
 uint64 h14; // ??? bone map (proceeds skeleton data)
 uint64 h15; // ??? bone map (proceeds skeleton data)
 uint64 h16; // ??? 0x00
 uint64 h17; // offset to string table offsets
 uint64 h18; // ??? 0x00
};

struct RE7SKELETONPARENTINFO {
 uint16 data[8];
};

struct RE7SKELETON {
 struct RE7SKELETONMATRIX { real32 data[16]; };
 uint32 h01;                             // header: number of joints
 uint32 h02;                             // header: ???
 uint32 h03;                             // header: ???
 uint32 h04;                             // header: ???
 uint64 h05;                             // header: offset to parenting information
 uint64 h06;                             // header: offset to 1st matrix set
 uint64 h07;                             // header: offset to 2nd matrix set
 uint64 h08;                             // header: offset to 3rd matrix set
 std::vector<RE7SKELETONPARENTINFO> p01; // parent information
 std::vector<RE7SKELETONMATRIX> p02;     // matrix set #1
 std::vector<RE7SKELETONMATRIX> p03;     // matrix set #2
 std::vector<RE7SKELETONMATRIX> p04;     // matrix set #3
};

struct RE7BUFFERS {
 uint32 h01; // 7C 8D 7D 00 -- size of vertex buffer in bytes
 uint32 h02; // 38 57 41 00 -- offset to 2nd vertex buffer
 uint32 h03; // 00 00 00 00
 uint32 h04; // 00 00 00 00
 uint32 h05; // E2 9E 15 00 -- size of index buffer data in bytes
 uint32 h06; // 1C 36 0B 00 -- offset to 2nd index buffer
 uint32 h07; // 00 00 00 00
 uint32 h08; // 00 00 00 00
 uint32 h09; // 00 00 00 00
 uint32 h10; // 00 00 00 00
 uint64 h11; // 80 AC 7E 00 00 00 00 00 -- offset to index buffer
 boost::shared_array<char> vbdata;
 boost::shared_array<char> ibdata;
};

struct RE7MESH {
 uint32 p01; // index
 uint32 p02; // number of indices
 uint32 p03; // base index
 uint32 p04; // base vertex
};

struct RE7MESHGROUP {
 uint08 h01; // index
 uint08 h02; // number of submeshes
 uint08 h03; // 0
 uint08 h04; // 0
 uint32 h05; // ???
 uint32 h06; // number of vertices
 uint32 h07; // number of indices
 std::vector<RE7MESH> meshlist; // mesh list
};

struct RE7SURFACE {
 uint08 h01; // 05 number of meshes
 uint08 h02; // 00
 uint08 h03; // 00
 uint08 h04; // FC number of joints
 uint32 h05; // ???
 uint64 h06; // pointer to group offsets
 std::vector<uint16> jntmap;
 std::vector<uint64> offsets;
 std::vector<RE7MESHGROUP> groups;
};

struct RE7MESHINFO1 {
 // HEADER
 public :
  uint08 h01; // 03 number on entries
  uint08 h02; // 09
  uint08 h03; // 02
  uint08 h04; // 01
  uint32 h05; // 10 00 00 00
  uint32 h06; // 00 00 00 00
  uint32 h07; // 00 00 00 00
  real32 h08[12]; // 6F 1C 29 3C - C8 69 A3 3D - 92 61 A4 3C
                  // F0 8F 83 3E - 5E DD ED BD - C1 D3 2F BE
                  // BC 59 23 BE - 00 00 80 3F - 34 12 21 3E
                  // 2C 98 78 3E - 61 75 02 3E - 00 00 80 3F
  uint64 h09; // C0 00 00 00 00 00 00 00 offset
 // DATA
 public :
  std::vector<uint64> offsets; // offsets to surfaces
  std::vector<RE7SURFACE> surfaces; // surface list
};

struct RE7MESHINFO2 {
 uint08 h01;     // 03
 uint08 h02;     // 09
 uint08 h03;     // 02
 uint08 h04;     // 01
 uint32 h05;     // 0B 00 00 00
 uint32 h06;     // 00 00 00 00
 uint32 h07;     // 00 00 00 00
 uint64 h08;     // 38 0B 00 00 00 00 00 00 offset
 uint32 h09[12]; // 00 00 00 00
 std::vector<uint64> offsets;     // offsets to surfaces
 std::vector<RE7SURFACE> surfaces; // surface list
};

class RE7ModelProcessor {
 private :
  ADVANCEDMODELCONTAINER amc1;
  ADVANCEDMODELCONTAINER amc2;
 private :
  STDSTRING filename;
  STDSTRING pathname;
  STDSTRING shrtname;
 private :
  bool debug;
  std::ifstream ifile;
  std::ofstream dfile;
  uint32 filesize;
 private :
  RE7HEADER header;
  RE7SKELETON skeleton;
  RE7BUFFERS buffers;
  RE7MESHINFO1 meshinfo1;
  RE7MESHINFO2 meshinfo2;
  bool is_ib32;
 private :
  bool read_header(void);
  bool read_stringtable(void);
  bool read_skeleton(void);
  bool read_buffers(void);
  bool read_meshinfo1(void);
  bool read_meshinfo2(void);
  bool construct_skeleton(void);
  bool index_buffer_heuristic(void);
  bool construct(void);
 public :
  bool run(void);
 public :
  RE7ModelProcessor(LPCTSTR fn, endian_mode em);
 ~RE7ModelProcessor() {}
};

RE7ModelProcessor::RE7ModelProcessor(LPCTSTR fn, endian_mode em) : debug(true)
{
 filename = fn;
 filesize = 0;
 is_ib32 = false;
}

bool RE7ModelProcessor::read_header(void)
{
 // read header
 header.h01 = LE_read_uint32(ifile);
 header.h02 = LE_read_uint32(ifile);
 header.h03 = LE_read_uint32(ifile);
 header.h04 = LE_read_uint16(ifile);
 header.h05 = LE_read_uint16(ifile);
 header.h06 = LE_read_uint64(ifile);
 header.h07 = LE_read_uint64(ifile);
 header.h08 = LE_read_uint64(ifile);
 header.h09 = LE_read_uint64(ifile);
 header.h10 = LE_read_uint64(ifile);
 header.h11 = LE_read_uint64(ifile);
 header.h12 = LE_read_uint64(ifile);
 header.h13 = LE_read_uint64(ifile);
 header.h14 = LE_read_uint64(ifile);
 header.h15 = LE_read_uint64(ifile);
 header.h16 = LE_read_uint64(ifile);
 header.h17 = LE_read_uint64(ifile);
 header.h18 = LE_read_uint64(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // print header
 if(debug) {
    dfile << "--------" << endl;
    dfile << " HEADER " << endl;
    dfile << "--------" << endl;
    dfile << endl;
    dfile << setfill('0');
    dfile << "magic                                 = 0x" << hex << setw( 8) << header.h01 << dec << endl;
    dfile << "model flags?                          = 0x" << hex << setw( 8) << header.h02 << dec << endl;
    dfile << "???                                   = 0x" << hex << setw( 8) << header.h03 << dec << endl;
    dfile << "???                                   = 0x" << hex << setw( 4) << header.h04 << dec << endl;
    dfile << "number of strings in stringtable      = 0x" << hex << setw( 4) << header.h05 << dec << endl;
    dfile << "offset to 1st section                 = 0x" << hex << setw(16) << header.h06 << dec << endl;
    dfile << "offset to 2nd section                 = 0x" << hex << setw(16) << header.h07 << dec << endl;
    dfile << "??? 0x00                              = 0x" << hex << setw(16) << header.h08 << dec << endl;
    dfile << "offset to skeleton                    = 0x" << hex << setw(16) << header.h09 << dec << endl;
    dfile << "??? 0x00                              = 0x" << hex << setw(16) << header.h10 << dec << endl;
    dfile << "offset to skeleton-related data       = 0x" << hex << setw(16) << header.h11 << dec << endl;
    dfile << "offset to vertex buffer               = 0x" << hex << setw(16) << header.h12 << dec << endl;
    dfile << "??? 0x00                              = 0x" << hex << setw(16) << header.h13 << dec << endl;
    dfile << "??? bone map                          = 0x" << hex << setw(16) << header.h14 << dec << endl;
    dfile << "??? bone map                          = 0x" << hex << setw(16) << header.h15 << dec << endl;
    dfile << "??? 0x00                              = 0x" << hex << setw(16) << header.h16 << dec << endl;
    dfile << "offset to string table offsets        = 0x" << hex << setw(16) << header.h17 << dec << endl;
    dfile << "??? 0x00                              = 0x" << hex << setw(16) << header.h18 << dec << endl;
    dfile << endl;
   }

 return true;
}

bool RE7ModelProcessor::read_stringtable(void)
{
 // number of strings in stringtable
 if(!header.h05) return true;

 // offset to string table offsets
 if(!header.h17) return true;

 // read string offsets
 ifile.seekg(header.h17);
 if(ifile.fail()) return error("Seek failure.", __LINE__);
 boost::shared_array<uint64> offsets(new uint64[header.h05]);
 if(!LE_read_array(ifile, offsets.get(), header.h05)) return error("Read failure.", __LINE__);

 // read strings
 uint32 n_strings = header.h05;
 std::vector<std::string> stringtable(n_strings);
 for(uint32 i = 0; i < n_strings; i++) {
     // move to string
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.", __LINE__);
     // read string
     char buffer[1024];
     if(!read_string(ifile, buffer, 1024)) return error("Read failure.", __LINE__);
     stringtable[i] = buffer;
    }

 // print skeleton data
 if(debug) {
    dfile << "-------------" << endl;
    dfile << " STRINGTABLE " << endl;
    dfile << "-------------" << endl;
    dfile << endl;
    for(uint32 i = 0; i < n_strings; i++) {
        dfile << "string[0x" << hex << i << dec << "] = ";
        dfile << stringtable[i].c_str() << endl;
       }
    dfile << endl;
   }
 return true;
}

bool RE7ModelProcessor::read_skeleton(void)
{
 // nothing to do
 if(!header.h09) return true;

 // move to skeleton header
 ifile.seekg(header.h09);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read skeleton header
 skeleton.h01 = LE_read_uint32(ifile); // n_joints
 skeleton.h02 = LE_read_uint32(ifile); // unk01   
 skeleton.h03 = LE_read_uint32(ifile); // unk02   
 skeleton.h04 = LE_read_uint32(ifile); // unk03   
 skeleton.h05 = LE_read_uint64(ifile); // p_offset
 skeleton.h06 = LE_read_uint64(ifile); // moffset1
 skeleton.h07 = LE_read_uint64(ifile); // moffset2
 skeleton.h08 = LE_read_uint64(ifile); // moffset3

 // if there is joint data
 if(skeleton.h01)
   {
    // move to parent data
    ifile.seekg(skeleton.h05);
    if(ifile.fail()) return error("Seek failure.", __LINE__);

    // read parent data
    skeleton.p01.resize(skeleton.h01);
    for(uint32 i = 0; i < skeleton.h01; i++) LE_read_array(ifile, &(skeleton.p01[i].data[0]), 8);
    if(ifile.fail()) return error("Read failure.", __LINE__);

    // move to matrix set #1
    ifile.seekg(skeleton.h06);
    if(ifile.fail()) return error("Seek failure.", __LINE__);

    // read matrix set #1
    skeleton.p02.resize(skeleton.h01);
    for(uint32 i = 0; i < skeleton.h01; i++) LE_read_array(ifile, &(skeleton.p02[i].data[0]), 16);
    if(ifile.fail()) return error("Read failure.", __LINE__);

    // move to matrix set #2
    ifile.seekg(skeleton.h07);
    if(ifile.fail()) return error("Seek failure.", __LINE__);

    // read matrix set #2
    skeleton.p03.resize(skeleton.h01);
    for(uint32 i = 0; i < skeleton.h01; i++) LE_read_array(ifile, &(skeleton.p03[i].data[0]), 16);
    if(ifile.fail()) return error("Read failure.");

    // move to matrix set #3
    ifile.seekg(skeleton.h08);
    if(ifile.fail()) return error("Seek failure.", __LINE__);

    // read matrix set #3
    skeleton.p04.resize(skeleton.h01);
    for(uint32 i = 0; i < skeleton.h01; i++) LE_read_array(ifile, &(skeleton.p04[i].data[0]), 16);
    if(ifile.fail()) return error("Read failure.", __LINE__);
   }

 // print skeleton data
 if(debug) {
    dfile << "----------" << endl;
    dfile << " SKELETON " << endl;
    dfile << "----------" << endl;
    dfile << endl;
    dfile << setfill('0');
    dfile << "Header:" << endl;
    dfile << "n_joints                = 0x" << hex << setw( 8) << skeleton.h01 << dec << endl;
    dfile << "???                     = 0x" << hex << setw( 8) << skeleton.h02 << dec << endl;
    dfile << "???                     = 0x" << hex << setw( 8) << skeleton.h03 << dec << endl;
    dfile << "???                     = 0x" << hex << setw( 8) << skeleton.h04 << dec << endl;
    dfile << "offset (parenting info) = 0x" << hex << setw(16) << skeleton.h05 << dec << endl;
    dfile << "offset (matrix set #1)  = 0x" << hex << setw(16) << skeleton.h06 << dec << endl;
    dfile << "offset (matrix set #2)  = 0x" << hex << setw(16) << skeleton.h07 << dec << endl;
    dfile << "offset (matrix set #3)  = 0x" << hex << setw(16) << skeleton.h08 << dec << endl;
    dfile << endl;
    dfile << "Parenting Information:" << endl;
    for(uint32 i = 0; i < skeleton.h01; i++) {
        dfile << "0x" << hex << setw(4) << skeleton.p01[i].data[0] << dec << " ";
        dfile << "0x" << hex << setw(4) << skeleton.p01[i].data[1] << dec << " ";
        dfile << "0x" << hex << setw(4) << skeleton.p01[i].data[2] << dec << " ";
        dfile << "0x" << hex << setw(4) << skeleton.p01[i].data[3] << dec << " ";
        dfile << "0x" << hex << setw(4) << skeleton.p01[i].data[4] << dec << " ";
        dfile << "0x" << hex << setw(4) << skeleton.p01[i].data[5] << dec << " ";
        dfile << "0x" << hex << setw(4) << skeleton.p01[i].data[6] << dec << " ";
        dfile << "0x" << hex << setw(4) << skeleton.p01[i].data[7] << dec << endl;
       }
    dfile << endl;
    dfile << "Matrix Set #1:" << endl;
    for(uint32 i = 0; i < skeleton.h01; i++) {
        cs::math::matrix4x4<real32> m(skeleton.p02[i].data);
        m.transpose();
        for(uint32 j = 0; j < 16; j++) if(fabs(m[j]) < 1.0e-7f) m[j] = 0.0f;
        dfile << "matrix[0x" << hex << i << dec << "] = " << endl;
        dfile << setfill(' ') << setprecision(4);
        dfile << setw(10) << m(0,0) << ", " << setw(10) << m(0,1) << ", " << setw(10) << m(0,2) << ", " << setw(10) << m(0,3) << ", " << endl;
        dfile << setw(10) << m(1,0) << ", " << setw(10) << m(1,1) << ", " << setw(10) << m(1,2) << ", " << setw(10) << m(1,3) << ", " << endl;
        dfile << setw(10) << m(2,0) << ", " << setw(10) << m(2,1) << ", " << setw(10) << m(2,2) << ", " << setw(10) << m(2,3) << ", " << endl;
        dfile << setw(10) << m(3,0) << ", " << setw(10) << m(3,1) << ", " << setw(10) << m(3,2) << ", " << setw(10) << m(3,3) << endl;
       }
    dfile << "Matrix Set #2:" << endl;
    for(uint32 i = 0; i < skeleton.h01; i++) {
        cs::math::matrix4x4<real32> m(skeleton.p03[i].data);
        m.transpose();
        for(uint32 j = 0; j < 16; j++) if(fabs(m[j]) < 1.0e-7f) m[j] = 0.0f;
        dfile << "matrix[0x" << hex << i << dec << "] = " << endl;
        dfile << setfill(' ') << setprecision(4);
        dfile << setw(10) << m(0,0) << ", " << setw(10) << m(0,1) << ", " << setw(10) << m(0,2) << ", " << setw(10) << m(0,3) << ", " << endl;
        dfile << setw(10) << m(1,0) << ", " << setw(10) << m(1,1) << ", " << setw(10) << m(1,2) << ", " << setw(10) << m(1,3) << ", " << endl;
        dfile << setw(10) << m(2,0) << ", " << setw(10) << m(2,1) << ", " << setw(10) << m(2,2) << ", " << setw(10) << m(2,3) << ", " << endl;
        dfile << setw(10) << m(3,0) << ", " << setw(10) << m(3,1) << ", " << setw(10) << m(3,2) << ", " << setw(10) << m(3,3) << endl;
       }
    dfile << "Matrix Set #3:" << endl;
    for(uint32 i = 0; i < skeleton.h01; i++) {
        cs::math::matrix4x4<real32> m(skeleton.p04[i].data);
        m.transpose();
        for(uint32 j = 0; j < 16; j++) if(fabs(m[j]) < 1.0e-7f) m[j] = 0.0f;
        dfile << "matrix[0x" << hex << i << dec << "] = " << endl;
        dfile << setfill(' ') << setprecision(4);
        dfile << setw(10) << m(0,0) << ", " << setw(10) << m(0,1) << ", " << setw(10) << m(0,2) << ", " << setw(10) << m(0,3) << ", " << endl;
        dfile << setw(10) << m(1,0) << ", " << setw(10) << m(1,1) << ", " << setw(10) << m(1,2) << ", " << setw(10) << m(1,3) << ", " << endl;
        dfile << setw(10) << m(2,0) << ", " << setw(10) << m(2,1) << ", " << setw(10) << m(2,2) << ", " << setw(10) << m(2,3) << ", " << endl;
        dfile << setw(10) << m(3,0) << ", " << setw(10) << m(3,1) << ", " << setw(10) << m(3,2) << ", " << setw(10) << m(3,3) << endl;
       }
    dfile << setfill(' ');
    dfile << endl;
   }

 return true;
}

bool RE7ModelProcessor::read_buffers(void)
{
 // nothing to do
 if(!header.h12) return true;

 // move to bufferlist header
 ifile.seekg(header.h12);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read bufferlist header
 buffers.h01 = LE_read_uint32(ifile); // size of vertex buffer in bytes
 buffers.h02 = LE_read_uint32(ifile); // offset to 2nd vertex buffer
 buffers.h03 = LE_read_uint32(ifile); //
 buffers.h04 = LE_read_uint32(ifile); //
 buffers.h05 = LE_read_uint32(ifile); // size of index buffer data in bytes
 buffers.h06 = LE_read_uint32(ifile); // offset to 2nd index buffer
 buffers.h07 = LE_read_uint32(ifile); //
 buffers.h08 = LE_read_uint32(ifile); //
 buffers.h09 = LE_read_uint32(ifile); //
 buffers.h10 = LE_read_uint32(ifile); //
 buffers.h11 = LE_read_uint64(ifile); // offset to index buffer
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // should we warn about non-16-byte alignment
 uint64 position = (uint64)ifile.tellg();
 if(ifile.fail()) return error("Tell failure.", __LINE__);
 if(position % 0x10) message("Vertex buffer offset is not 16-byte aligned.");

 // read vertex buffer
 if(buffers.h01) {
    buffers.vbdata.reset(new char[buffers.h01]);
    ifile.read(buffers.vbdata.get(), buffers.h01);
    if(ifile.fail()) return error("Read failure.", __LINE__);
   }

 // read index buffer
 if(buffers.h11) {
    // move
    ifile.seekg(buffers.h11);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
    // read
    buffers.ibdata.reset(new char[buffers.h05]);
    ifile.read(buffers.ibdata.get(), buffers.h05);
    if(ifile.fail()) return error("Read failure.", __LINE__);
   }

 if(debug) {
    dfile << "--------------------" << endl;
    dfile << " BUFFER INFORMATION " << endl;
    dfile << "--------------------" << endl;
    dfile << endl;
    dfile << setfill('0');
    dfile << "0x" << setw( 8) << hex << buffers.h01 << dec << " = size of vertex buffer in bytes" << endl;
    dfile << "0x" << setw( 8) << hex << buffers.h02 << dec << " = offset to 2nd vertex buffer" << endl;
    dfile << "0x" << setw( 8) << hex << buffers.h03 << dec << " = 0" << endl;
    dfile << "0x" << setw( 8) << hex << buffers.h04 << dec << " = 0" << endl;
    dfile << "0x" << setw( 8) << hex << buffers.h05 << dec << " = size of index buffer data in bytes" << endl;
    dfile << "0x" << setw( 8) << hex << buffers.h06 << dec << " = offset to 2nd index buffer" << endl;
    dfile << "0x" << setw( 8) << hex << buffers.h07 << dec << " = 0" << endl;
    dfile << "0x" << setw( 8) << hex << buffers.h08 << dec << " = 0" << endl;
    dfile << "0x" << setw( 8) << hex << buffers.h09 << dec << " = 0" << endl;
    dfile << "0x" << setw( 8) << hex << buffers.h10 << dec << " = 0" << endl;
    dfile << "0x" << setw(16) << hex << buffers.h11 << dec << " = offset to index buffer" << endl;
    dfile << endl;
   }

 return true;
}

bool RE7ModelProcessor::read_meshinfo1(void)
{
 // nothing to do
 if(!header.h06) return true;

 // move to meshinfo header
 ifile.seekg(header.h06);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read header
 meshinfo1.h01 = LE_read_uint08(ifile); // 03 number on entries
 meshinfo1.h02 = LE_read_uint08(ifile); // 09
 meshinfo1.h03 = LE_read_uint08(ifile); // 02
 meshinfo1.h04 = LE_read_uint08(ifile); // 01
 meshinfo1.h05 = LE_read_uint32(ifile); // 10 00 00 00
 meshinfo1.h06 = LE_read_uint32(ifile); // 00 00 00 00
 meshinfo1.h07 = LE_read_uint32(ifile); // 00 00 00 00
 LE_read_array(ifile, &(meshinfo1.h08[0]), 12);
 meshinfo1.h09 = LE_read_uint64(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // move to offsets
 ifile.seekg(meshinfo1.h09);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read offsets
 meshinfo1.offsets.resize(meshinfo1.h01);
 if(!LE_read_array(ifile, &(meshinfo1.offsets[0]), meshinfo1.h01)) return error("Read failure.", __LINE__);

 // read surface information
 meshinfo1.surfaces.resize(meshinfo1.h01);
 for(uint32 i = 0; i < meshinfo1.h01; i++)
    {
     // move to offset
     ifile.seekg(meshinfo1.offsets[i]);
     if(ifile.fail()) return error("Seek failure.", __LINE__);

     // read surface information
     meshinfo1.surfaces[i].h01 = LE_read_uint08(ifile); // 05 number of meshes
     meshinfo1.surfaces[i].h02 = LE_read_uint08(ifile); // 00
     meshinfo1.surfaces[i].h03 = LE_read_uint08(ifile); // 00
     meshinfo1.surfaces[i].h04 = LE_read_uint08(ifile); // FC number of joints
     meshinfo1.surfaces[i].h05 = LE_read_uint32(ifile); // ???
     meshinfo1.surfaces[i].h06 = LE_read_uint64(ifile); // pointer to mesh item offsets

     // read joint map
     if(meshinfo1.surfaces[i].h04) {
        meshinfo1.surfaces[i].jntmap.resize(meshinfo1.surfaces[i].h04);
        if(!LE_read_array(ifile, &(meshinfo1.surfaces[i].jntmap[0]), meshinfo1.surfaces[i].h04)) return error("Read failure.", __LINE__);
       }

     // move to offset
     ifile.seekg(meshinfo1.surfaces[i].h06);
     if(ifile.fail()) return error("Seek failure.", __LINE__);

     // read offset list
     meshinfo1.surfaces[i].offsets.resize(meshinfo1.surfaces[i].h01);
     if(!LE_read_array(ifile, &(meshinfo1.surfaces[i].offsets[0]), meshinfo1.surfaces[i].h01)) return error("Read failure.", __LINE__);

     // read mesh information
     meshinfo1.surfaces[i].groups.resize(meshinfo1.surfaces[i].h01);
     for(uint32 j = 0; j < meshinfo1.surfaces[i].h01; j++)
        {
         // move to group information
         ifile.seekg(meshinfo1.surfaces[i].offsets[j]);
         if(ifile.fail()) return error("Seek failure.", __LINE__);

         // read group information
         meshinfo1.surfaces[i].groups[j].h01 = LE_read_uint08(ifile); // index
         meshinfo1.surfaces[i].groups[j].h02 = LE_read_uint08(ifile); // number of submeshes
         meshinfo1.surfaces[i].groups[j].h03 = LE_read_uint08(ifile); // 0
         meshinfo1.surfaces[i].groups[j].h04 = LE_read_uint08(ifile); // 0
         meshinfo1.surfaces[i].groups[j].h05 = LE_read_uint32(ifile); // ???
         meshinfo1.surfaces[i].groups[j].h06 = LE_read_uint32(ifile); // ???
         meshinfo1.surfaces[i].groups[j].h07 = LE_read_uint32(ifile); // number of indices
         if(ifile.fail()) return error("Read failure.", __LINE__);

         // read meshes
         meshinfo1.surfaces[i].groups[j].meshlist.resize(meshinfo1.surfaces[i].groups[j].h02); // mesh list
         for(uint32 k = 0; k < meshinfo1.surfaces[i].groups[j].h02; k++)
            {
             meshinfo1.surfaces[i].groups[j].meshlist[k].p01 = LE_read_uint32(ifile); // index
             meshinfo1.surfaces[i].groups[j].meshlist[k].p02 = LE_read_uint32(ifile); // number of indices
             meshinfo1.surfaces[i].groups[j].meshlist[k].p03 = LE_read_uint32(ifile); // base index
             meshinfo1.surfaces[i].groups[j].meshlist[k].p04 = LE_read_uint32(ifile); // base vertex
             if(ifile.fail()) return error("Read failure.", __LINE__);
            }
        }
    }

 // debug information
 if(debug) {
    dfile << "---------------------" << endl;
    dfile << " MESH INFORMATION #1 " << endl;
    dfile << "---------------------" << endl;
    dfile << endl;
    dfile << setfill('0');
    dfile << "0x" << setw( 2) << hex << (uint16)meshinfo1.h01 << dec << " = number of offsets" << endl;
    dfile << "0x" << setw( 2) << hex << (uint16)meshinfo1.h02 << dec << " = ?" << endl;
    dfile << "0x" << setw( 2) << hex << (uint16)meshinfo1.h03 << dec << " = ?" << endl;
    dfile << "0x" << setw( 2) << hex << (uint16)meshinfo1.h04 << dec << " = ?" << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo1.h05 << dec << " = ?" << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo1.h06 << dec << " = ?" << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo1.h07 << dec << " = ?" << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x0]) << dec << " = bounding sphere " << meshinfo1.h08[0x0] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x1]) << dec << " = bounding sphere " << meshinfo1.h08[0x1] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x2]) << dec << " = bounding sphere " << meshinfo1.h08[0x2] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x3]) << dec << " = bounding sphere " << meshinfo1.h08[0x3] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x4]) << dec << " = min bounding box " << meshinfo1.h08[0x4] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x5]) << dec << " = min bounding box " << meshinfo1.h08[0x5] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x6]) << dec << " = min bounding box " << meshinfo1.h08[0x6] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x7]) << dec << " = min bounding box " << meshinfo1.h08[0x7] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x8]) << dec << " = max bounding box " << meshinfo1.h08[0x8] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0x9]) << dec << " = max bounding box " << meshinfo1.h08[0x9] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0xA]) << dec << " = max bounding box " << meshinfo1.h08[0xA] << endl;
    dfile << "0x" << setw( 8) << hex << interpret_as_uint32(meshinfo1.h08[0xB]) << dec << " = max bounding box " << meshinfo1.h08[0xB] << endl;
    dfile << "0x" << setw(16) << hex << meshinfo1.h09 << dec << " = offset to offsets" << endl;
    dfile << endl;
    dfile << "SURFACE OFFSETS" << endl;
    for(uint32 i = 0; i < meshinfo1.h01; i++) dfile << "0x" << setw(16) << hex << meshinfo1.offsets[i] << dec << " = offsets[" << i << "]" << endl;
    dfile << endl;
    for(uint32 i = 0; i < meshinfo1.h01; i++) {
        dfile << "SURFACE[" << i << "]" << endl;
        dfile << " 0x" << setw( 2) << hex << (uint16)meshinfo1.surfaces[i].h01 << dec << " = number of meshes" << endl;
        dfile << " 0x" << setw( 2) << hex << (uint16)meshinfo1.surfaces[i].h02 << dec << " = ???" << endl;
        dfile << " 0x" << setw( 2) << hex << (uint16)meshinfo1.surfaces[i].h03 << dec << " = ???" << endl;
        dfile << " 0x" << setw( 2) << hex << (uint16)meshinfo1.surfaces[i].h04 << dec << " = number of joints" << endl;
        dfile << " 0x" << setw( 8) << hex << meshinfo1.surfaces[i].h05 << dec << " = ???" << endl;
        dfile << " 0x" << setw(16) << hex << meshinfo1.surfaces[i].h06 << dec << " = pointer to mesh offsets" << endl;
        dfile << " JOINT MAP" << endl;
        // read joint map
         for(uint32 j = 0; j < meshinfo1.surfaces[i].h04; j++)
             dfile << "  0x" << setw(4) << hex << j << dec << ": 0x" << hex << meshinfo1.surfaces[i].jntmap[j] << dec << endl;
      }
    dfile << endl;
    for(uint32 i = 0; i < meshinfo1.h01; i++) {
        dfile << "SURFACE[" << i << "]" << endl;
        for(uint32 j = 0; j < meshinfo1.surfaces[i].h01; j++)
            dfile << " 0x" << setw(16) << hex << meshinfo1.surfaces[i].offsets[j] << dec << " = MESHGROUP[" << j << "]" << endl;
       }
    dfile << endl;
    for(uint32 i = 0; i < meshinfo1.h01; i++) {
        dfile << "SURFACE[" << i << "]" << endl;
        for(uint32 j = 0; j < meshinfo1.surfaces[i].h01; j++) {
            dfile << " MESHGROUP[" << j << "]" << endl;
            dfile << "  0x" << setw( 2) << hex << (uint16)meshinfo1.surfaces[i].groups[j].h01 << dec << " = index" << endl;
            dfile << "  0x" << setw( 2) << hex << (uint16)meshinfo1.surfaces[i].groups[j].h02 << dec << " = number of submeshes" << endl;
            dfile << "  0x" << setw( 2) << hex << (uint16)meshinfo1.surfaces[i].groups[j].h03 << dec << " = 0" << endl;
            dfile << "  0x" << setw( 2) << hex << (uint16)meshinfo1.surfaces[i].groups[j].h04 << dec << " = 0" << endl;
            dfile << "  0x" << setw( 8) << hex << meshinfo1.surfaces[i].groups[j].h05 << dec << " = ???" << endl;
            dfile << "  0x" << setw( 8) << hex << meshinfo1.surfaces[i].groups[j].h06 << dec << " = number of vertices" << endl;
            dfile << "  0x" << setw( 8) << hex << meshinfo1.surfaces[i].groups[j].h07 << dec << " = number of indices" << endl;
            for(uint32 k = 0; k < meshinfo1.surfaces[i].groups[j].h02; k++) {
                dfile << "  MESH[" << k << "] = ";
                dfile << "0x" << setw( 8) << hex << meshinfo1.surfaces[i].groups[j].meshlist[k].p01 << dec << " ";
                dfile << "0x" << setw( 8) << hex << meshinfo1.surfaces[i].groups[j].meshlist[k].p02 << dec << " ";
                dfile << "0x" << setw( 8) << hex << meshinfo1.surfaces[i].groups[j].meshlist[k].p03 << dec << " ";
                dfile << "0x" << setw( 8) << hex << meshinfo1.surfaces[i].groups[j].meshlist[k].p04 << dec << " ";
                dfile << "index, number of indices, base index, base vertex" << endl;
               }
           }
        dfile << endl;
       }
    dfile << endl;
   }

 return true;
}

bool RE7ModelProcessor::read_meshinfo2(void)
{
 // nothing to do
 if(!header.h07) return true;

 // move to meshinfo header
 ifile.seekg(header.h07);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read header
 meshinfo2.h01 = LE_read_uint08(ifile); // 03 number on entries
 meshinfo2.h02 = LE_read_uint08(ifile); // 09
 meshinfo2.h03 = LE_read_uint08(ifile); // 02
 meshinfo2.h04 = LE_read_uint08(ifile); // 01
 meshinfo2.h05 = LE_read_uint32(ifile); // 10 00 00 00
 meshinfo2.h06 = LE_read_uint32(ifile); // 00 00 00 00
 meshinfo2.h07 = LE_read_uint32(ifile); // 00 00 00 00
 meshinfo2.h08 = LE_read_uint64(ifile); // 38 0B 00 00 00 00 00 00
 LE_read_array(ifile, &(meshinfo2.h09[0]), 12);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // move to offsets
 ifile.seekg(meshinfo2.h08);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // read offsets
 meshinfo2.offsets.resize(meshinfo2.h01);
 if(!LE_read_array(ifile, &(meshinfo2.offsets[0]), meshinfo2.h01)) return error("Read failure.", __LINE__);

 // read surface information
 meshinfo2.surfaces.resize(meshinfo2.h01);
 for(uint32 i = 0; i < meshinfo2.h01; i++)
    {
     // move to offset
     ifile.seekg(meshinfo2.offsets[i]);
     if(ifile.fail()) return error("Seek failure.", __LINE__);

     // read surface information
     meshinfo2.surfaces[i].h01 = LE_read_uint08(ifile); // 05 number of meshes
     meshinfo2.surfaces[i].h02 = LE_read_uint08(ifile); // 00
     meshinfo2.surfaces[i].h03 = LE_read_uint08(ifile); // 00
     meshinfo2.surfaces[i].h04 = LE_read_uint08(ifile); // FC number of joints
     meshinfo2.surfaces[i].h05 = LE_read_uint32(ifile); // ???
     meshinfo2.surfaces[i].h06 = LE_read_uint64(ifile); // pointer to mesh item offsets

     // read joint map
     if(meshinfo2.surfaces[i].h04) {
        meshinfo2.surfaces[i].jntmap.resize(meshinfo2.surfaces[i].h04);
        if(!LE_read_array(ifile, &(meshinfo2.surfaces[i].jntmap[0]), meshinfo2.surfaces[i].h04)) return error("Read failure.", __LINE__);
       }

     // move to offset
     ifile.seekg(meshinfo2.surfaces[i].h06);
     if(ifile.fail()) return error("Seek failure.", __LINE__);

     // read offset list
     meshinfo2.surfaces[i].offsets.resize(meshinfo2.surfaces[i].h01);
     if(!LE_read_array(ifile, &(meshinfo2.surfaces[i].offsets[0]), meshinfo2.surfaces[i].h01)) return error("Read failure.", __LINE__);

     // read mesh information
     meshinfo2.surfaces[i].groups.resize(meshinfo2.surfaces[i].h01);
     for(uint32 j = 0; j < meshinfo2.surfaces[i].h01; j++)
        {
         // move to group information
         ifile.seekg(meshinfo2.surfaces[i].offsets[j]);
         if(ifile.fail()) return error("Seek failure.", __LINE__);

         // read group information
         meshinfo2.surfaces[i].groups[j].h01 = LE_read_uint08(ifile); // index
         meshinfo2.surfaces[i].groups[j].h02 = LE_read_uint08(ifile); // number of submeshes
         meshinfo2.surfaces[i].groups[j].h03 = LE_read_uint08(ifile); // 0
         meshinfo2.surfaces[i].groups[j].h04 = LE_read_uint08(ifile); // 0
         meshinfo2.surfaces[i].groups[j].h05 = LE_read_uint32(ifile); // ???
         meshinfo2.surfaces[i].groups[j].h06 = LE_read_uint32(ifile); // ???
         meshinfo2.surfaces[i].groups[j].h07 = LE_read_uint32(ifile); // number of indices
         if(ifile.fail()) return error("Read failure.", __LINE__);

         // read meshes
         meshinfo2.surfaces[i].groups[j].meshlist.resize(meshinfo2.surfaces[i].groups[j].h02); // mesh list
         for(uint32 k = 0; k < meshinfo2.surfaces[i].groups[j].h02; k++)
            {
             meshinfo2.surfaces[i].groups[j].meshlist[k].p01 = LE_read_uint32(ifile); // index
             meshinfo2.surfaces[i].groups[j].meshlist[k].p02 = LE_read_uint32(ifile); // number of indices
             meshinfo2.surfaces[i].groups[j].meshlist[k].p03 = LE_read_uint32(ifile); // base index
             meshinfo2.surfaces[i].groups[j].meshlist[k].p04 = LE_read_uint32(ifile); // base vertex
             if(ifile.fail()) return error("Read failure.", __LINE__);
            }
        }
    }

 // debug information
 if(debug) {
    dfile << "---------------------" << endl;
    dfile << " MESH INFORMATION #2 " << endl;
    dfile << "---------------------" << endl;
    dfile << endl;
    dfile << setfill('0');
    dfile << "0x" << setw( 2) << hex << (uint16)meshinfo2.h01 << dec << " = number of offsets" << endl;
    dfile << "0x" << setw( 2) << hex << (uint16)meshinfo2.h02 << dec << " = ?" << endl;
    dfile << "0x" << setw( 2) << hex << (uint16)meshinfo2.h03 << dec << " = ?" << endl;
    dfile << "0x" << setw( 2) << hex << (uint16)meshinfo2.h04 << dec << " = ?" << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h05 << dec << " = ?" << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h06 << dec << " = ?" << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h07 << dec << " = ?" << endl;
    dfile << "0x" << setw(16) << hex << meshinfo2.h08 << dec << " = offset to offsets" << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x0] << dec << " = bounding sphere? " << meshinfo2.h09[0x0] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x1] << dec << " = bounding sphere? " << meshinfo2.h09[0x1] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x2] << dec << " = bounding sphere? " << meshinfo2.h09[0x2] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x3] << dec << " = bounding sphere? " << meshinfo2.h09[0x3] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x4] << dec << " = min bounding box? " << meshinfo2.h09[0x4] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x5] << dec << " = min bounding box? " << meshinfo2.h09[0x5] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x6] << dec << " = min bounding box? " << meshinfo2.h09[0x6] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x7] << dec << " = min bounding box? " << meshinfo2.h09[0x7] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x8] << dec << " = max bounding box? " << meshinfo2.h09[0x8] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0x9] << dec << " = max bounding box? " << meshinfo2.h09[0x9] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0xA] << dec << " = max bounding box? " << meshinfo2.h09[0xA] << endl;
    dfile << "0x" << setw( 8) << hex << meshinfo2.h09[0xB] << dec << " = max bounding box? " << meshinfo2.h09[0xB] << endl;
    dfile << endl;
    dfile << "SURFACE OFFSETS" << endl;
    for(uint32 i = 0; i < meshinfo2.h01; i++) dfile << "0x" << setw(16) << hex << meshinfo2.offsets[i] << dec << " = offsets[" << i << "]" << endl;
    dfile << endl;
    for(uint32 i = 0; i < meshinfo2.h01; i++) {
        dfile << "SURFACE[" << i << "]" << endl;
        dfile << " 0x" << setw( 2) << hex << (uint16)meshinfo2.surfaces[i].h01 << dec << " = number of meshes" << endl;
        dfile << " 0x" << setw( 2) << hex << (uint16)meshinfo2.surfaces[i].h02 << dec << " = ???" << endl;
        dfile << " 0x" << setw( 2) << hex << (uint16)meshinfo2.surfaces[i].h03 << dec << " = ???" << endl;
        dfile << " 0x" << setw( 2) << hex << (uint16)meshinfo2.surfaces[i].h04 << dec << " = number of joints" << endl;
        dfile << " 0x" << setw( 8) << hex << meshinfo2.surfaces[i].h05 << dec << " = ???" << endl;
        dfile << " 0x" << setw(16) << hex << meshinfo2.surfaces[i].h06 << dec << " = pointer to mesh offsets" << endl;
       }
    dfile << endl;
    for(uint32 i = 0; i < meshinfo2.h01; i++) {
        dfile << "SURFACE[" << i << "]" << endl;
        for(uint32 j = 0; j < meshinfo2.surfaces[i].h01; j++)
            dfile << " 0x" << setw(16) << hex << meshinfo2.surfaces[i].offsets[j] << dec << " = MESHGROUP[" << j << "]" << endl;
       }
    dfile << endl;
    for(uint32 i = 0; i < meshinfo2.h01; i++) {
        dfile << "SURFACE[" << i << "]" << endl;
        for(uint32 j = 0; j < meshinfo2.surfaces[i].h01; j++) {
            dfile << " MESHGROUP[" << j << "]" << endl;
            dfile << "  0x" << setw( 2) << hex << (uint16)meshinfo2.surfaces[i].groups[j].h01 << dec << " = index" << endl;
            dfile << "  0x" << setw( 2) << hex << (uint16)meshinfo2.surfaces[i].groups[j].h02 << dec << " = number of submeshes" << endl;
            dfile << "  0x" << setw( 2) << hex << (uint16)meshinfo2.surfaces[i].groups[j].h03 << dec << " = 0" << endl;
            dfile << "  0x" << setw( 2) << hex << (uint16)meshinfo2.surfaces[i].groups[j].h04 << dec << " = 0" << endl;
            dfile << "  0x" << setw( 8) << hex << meshinfo2.surfaces[i].groups[j].h05 << dec << " = ???" << endl;
            dfile << "  0x" << setw( 8) << hex << meshinfo2.surfaces[i].groups[j].h06 << dec << " = number of vertices" << endl;
            dfile << "  0x" << setw( 8) << hex << meshinfo2.surfaces[i].groups[j].h07 << dec << " = number of indices" << endl;
            for(uint32 k = 0; k < meshinfo2.surfaces[i].groups[j].h02; k++) {
                dfile << "  MESH[" << k << "] = ";
                dfile << "0x" << setw( 8) << hex << meshinfo2.surfaces[i].groups[j].meshlist[k].p01 << dec << " ";
                dfile << "0x" << setw( 8) << hex << meshinfo2.surfaces[i].groups[j].meshlist[k].p02 << dec << " ";
                dfile << "0x" << setw( 8) << hex << meshinfo2.surfaces[i].groups[j].meshlist[k].p03 << dec << " ";
                dfile << "0x" << setw( 8) << hex << meshinfo2.surfaces[i].groups[j].meshlist[k].p04 << dec << " ";
                dfile << "index, number of indices, base index, base vertex" << endl;
               }
           }
        dfile << endl;
       }
    dfile << endl;
   }

 return true;
}

bool RE7ModelProcessor::construct_skeleton(void)
{
 // nothing to do
 if(!header.h09 || !skeleton.h01) return true;

 // skeleton object
 AMC_SKELETON2 skel;

 // create skeleton name
 std::stringstream ss;
 ss << "skeleton";

 // set format
 skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = ss.str();
 skel.tiplen = 0.01f;

 // set joints
 for(uint32 i = 0; i < skeleton.h01; i++)
    {
     // copy the two transformation matrices
     using namespace cs::math;
     matrix4x4<cs::binary32> m1(&(skeleton.p03[i].data[0]));
     matrix4x4<cs::binary32> m2(&(skeleton.p04[i].data[0]));
     m1.transpose();
     m2.transpose();

     // joints are actually remapped between bones-in-file and bones-in-complete-model
     uint32 jnt_index = skeleton.p01[i].data[0];
     uint32 par_index = skeleton.p01[i].data[1];
   
     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << jnt_index;
   
     // create joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = jnt_index; // this is actual joint ID
     joint.parent = ((par_index == 0xFFFF) ? AMC_INVALID_JOINT : par_index);
     joint.m_rel[0x0] = m1[0x0];
     joint.m_rel[0x1] = m1[0x1];
     joint.m_rel[0x2] = m1[0x2];
     joint.m_rel[0x3] = m1[0x3];
     joint.m_rel[0x4] = m1[0x4];
     joint.m_rel[0x5] = m1[0x5];
     joint.m_rel[0x6] = m1[0x6];
     joint.m_rel[0x7] = m1[0x7];
     joint.m_rel[0x8] = m1[0x8];
     joint.m_rel[0x9] = m1[0x9];
     joint.m_rel[0xA] = m1[0xA];
     joint.m_rel[0xB] = m1[0xB];
     joint.m_rel[0xC] = m1[0xC];
     joint.m_rel[0xD] = m1[0xD];
     joint.m_rel[0xE] = m1[0xE];
     joint.m_rel[0xF] = m1[0xF];
     joint.p_rel[0] = m1[0x3];
     joint.p_rel[1] = m1[0x7];
     joint.p_rel[2] = m1[0xB];
     joint.p_rel[3] = 1.0f;
     skel.joints.push_back(joint);
    }

 // insert skeletons
 amc1.skllist2.push_back(skel);
 if(header.h07) amc2.skllist2.push_back(skel);
 return true;
}

bool RE7ModelProcessor::index_buffer_heuristic(void)
{
 // by default we are false
 is_ib32 = false;

 for(uint32 i = 0; i < meshinfo1.surfaces.size(); i++)
    {
     // for each surface group
     for(uint32 j = 0; j < meshinfo1.surfaces[i].groups.size(); j++)
        {
         // number of points and vertices (overall in group)
         uint32 n_vtx = meshinfo1.surfaces[i].groups[j].h06;
         uint32 n_idx = meshinfo1.surfaces[i].groups[j].h07;

         // initialize surfaces
         for(uint32 k = 0; k < meshinfo1.surfaces[i].groups[j].meshlist.size(); k++)
            {
             // compute number of vertices in the vertex buffer since it is not given
             // this is important because if this goes over 0xFFFF we need 32-bit IB.
             uint32 n_verts = 0;
             if(k == (meshinfo1.surfaces[i].groups[j].meshlist.size() - 1)) {
                uint32 total = n_vtx + meshinfo1.surfaces[i].groups[j].meshlist[0].p04;
                n_verts = total - meshinfo1.surfaces[i].groups[j].meshlist[k].p04;
               }
             else
                n_verts = meshinfo1.surfaces[i].groups[j].meshlist[k + 1].p04 - meshinfo1.surfaces[i].groups[j].meshlist[k].p04;

             // 32-bit IB
             if(n_verts > 0xFFFF) {
                is_ib32 = true;
                return true;
               }
            }
        }
    }

 return true;
}

bool RE7ModelProcessor::construct(void)
{
 // for each surface
 uint32 mesh_index = 0;
 binary_stream bs(buffers.vbdata, buffers.h01);
 binary_stream cs(buffers.ibdata, buffers.h05);
 for(uint32 i = 0; i < meshinfo1.surfaces.size(); i++)
    {
     // construct joint map
     AMC_JOINTMAP jmap;
     for(uint32 j = 0; j < meshinfo1.surfaces[i].h04; j++) jmap.jntmap.push_back(meshinfo1.surfaces[i].jntmap[j]);
     amc1.jmaplist.push_back(jmap);

     // determine vertex buffer format
     uint32 format = 0;

     // XX 01 01
     bool has_jntmap = (meshinfo1.surfaces[i].jntmap.size() != 0);
     if(meshinfo1.h03 == 1 && meshinfo1.h04 == 1) {
        if(has_jntmap) format = 2; // skeletal (0x28)
        else format = 3; // static (0x18)
       }
     // XX 02 01
     else if(meshinfo1.h03 == 2 && meshinfo1.h04 == 1) {
        if(has_jntmap) format = 1; // skeletal (0x2C)
        else format = 4; // static (0x1C)
       }
     // XX 03 01
     else if(meshinfo1.h03 == 3 && meshinfo1.h04 == 1) {
        if(has_jntmap) format = 1; // skeletal (0x2C)
        else format = 4; // static (0x1C)
       }
     // XX 04 01
     else if(meshinfo1.h03 == 4 && meshinfo1.h04 == 1) {
        if(has_jntmap) format = 1; // skeletal (0x2C)
        else format = 4; // static (0x1C)
       }

     // for each surface group
     for(uint32 j = 0; j < meshinfo1.surfaces[i].groups.size(); j++)
        {
         // number of points and vertices (overall in group)
         uint32 n_vtx = meshinfo1.surfaces[i].groups[j].h06;
         uint32 n_idx = meshinfo1.surfaces[i].groups[j].h07;

         uint32 start_idx = 0xFFFFFFFFul;
         uint32 start_vtx = 0xFFFFFFFFul;
         for(uint32 k = 0; k < meshinfo1.surfaces[i].groups[j].meshlist.size(); k++) {
             uint32 idxbase = meshinfo1.surfaces[i].groups[j].meshlist[k].p03;
             uint32 vtxbase = meshinfo1.surfaces[i].groups[j].meshlist[k].p04;
             if(idxbase < start_idx) start_idx = idxbase;
             if(vtxbase < start_vtx) start_vtx = vtxbase;
            }

         // initialize surfaces
         for(uint32 k = 0; k < meshinfo1.surfaces[i].groups[j].meshlist.size(); k++)
            {
             uint32 index = meshinfo1.surfaces[i].groups[j].meshlist[k].p01;
             uint32 n_indices = meshinfo1.surfaces[i].groups[j].meshlist[k].p02;
             uint32 base_idx = meshinfo1.surfaces[i].groups[j].meshlist[k].p03;
             uint32 base_vtx = meshinfo1.surfaces[i].groups[j].meshlist[k].p04;

             // compute number of vertices in the vertex buffer since it is not given
             // this is important because if this goes over 0xFFFF we need 32-bit IB.
             uint32 n_verts = 0;
             if(k == (meshinfo1.surfaces[i].groups[j].meshlist.size() - 1)) {
                uint32 total = n_vtx + meshinfo1.surfaces[i].groups[j].meshlist[0].p04;
                n_verts = total - meshinfo1.surfaces[i].groups[j].meshlist[k].p04;
               }
             else
                n_verts = meshinfo1.surfaces[i].groups[j].meshlist[k + 1].p04 - meshinfo1.surfaces[i].groups[j].meshlist[k].p04;

             // debug: save offsets
             size_t vb_offset = bs.tell();
             size_t ib_offset = cs.tell();

             // initialize vertex buffer
             AMC_VTXBUFFER vb;
             vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
             vb.name = "default";
             vb.uvchan = 0;
             vb.uvtype[0] = AMC_INVALID_MAP;
             for(uint32 i = 1; i < 8; i++) vb.uvtype[i] = AMC_INVALID_MAP;
             vb.colors = 0;
             vb.elem = n_verts;
             vb.data.reset(new AMC_VERTEX[vb.elem]);

             // XX 02 01 (0x2C VB)
             if(format == 1)
               {
                // set VB flags
                vb.flags |= AMC_VERTEX_WEIGHTS;

                // set UV flags
                vb.uvchan = 2;
                vb.uvtype[0] = AMC_DIFFUSE_MAP;
                vb.uvtype[1] = AMC_DIFFUSE_MAP;

                // read vertices
                for(uint32 k = 0; k < vb.elem; k++)
                   {
                    // position
                    vb.data[k].vx = bs.read_real32();
                    vb.data[k].vy = bs.read_real32();
                    vb.data[k].vz = bs.read_real32();
                    vb.data[k].vw = 1.0f;
                    // normal (0x80/128 maps to +1) (0x7F/127 maps to -1)
                    vb.data[k].nx = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].ny = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nz = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nw = ((real32)bs.read_sint08())/128.0f;
                    // tangent
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    // uv channel 1
                    vb.data[k].uv[0][0] = bs.read_real16();
                    vb.data[k].uv[0][1] = bs.read_real16();
                    // uv channel 2
                    vb.data[k].uv[1][0] = bs.read_real16();
                    vb.data[k].uv[1][1] = bs.read_real16();
                    // blend indices
                    vb.data[k].wi[0] = bs.read_uint08();
                    vb.data[k].wi[1] = bs.read_uint08();
                    vb.data[k].wi[2] = bs.read_uint08();
                    vb.data[k].wi[3] = bs.read_uint08();
                    vb.data[k].wi[4] = bs.read_uint08();
                    vb.data[k].wi[5] = bs.read_uint08();
                    vb.data[k].wi[6] = bs.read_uint08();
                    vb.data[k].wi[7] = bs.read_uint08();
                    // blend weights
                    vb.data[k].wv[0] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[1] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[2] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[3] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[4] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[5] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[6] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[7] = ((real32)bs.read_uint08())/255.0f;
                   }
               }
             // XX 01 01 (0x28 VB)
             else if(format == 2)
               {
                // set VB flags
                vb.flags |= AMC_VERTEX_WEIGHTS;

                // set UV flags
                vb.uvchan = 1;
                vb.uvtype[0] = AMC_DIFFUSE_MAP;

                // read vertices
                for(uint32 k = 0; k < vb.elem; k++)
                   {
                    // position
                    vb.data[k].vx = bs.read_real32();
                    vb.data[k].vy = bs.read_real32();
                    vb.data[k].vz = bs.read_real32();
                    vb.data[k].vw = 1.0f;
                    // normal (0x80/128 maps to +1) (0x7F/127 maps to -1)
                    vb.data[k].nx = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].ny = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nz = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nw = ((real32)bs.read_sint08())/128.0f;
                    // tangent
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    // uv channel 1
                    vb.data[k].uv[0][0] = bs.read_real16();
                    vb.data[k].uv[0][1] = bs.read_real16();
                    // blend indices
                    vb.data[k].wi[0] = bs.read_uint08();
                    vb.data[k].wi[1] = bs.read_uint08();
                    vb.data[k].wi[2] = bs.read_uint08();
                    vb.data[k].wi[3] = bs.read_uint08();
                    vb.data[k].wi[4] = bs.read_uint08();
                    vb.data[k].wi[5] = bs.read_uint08();
                    vb.data[k].wi[6] = bs.read_uint08();
                    vb.data[k].wi[7] = bs.read_uint08();
                    // blend weights
                    vb.data[k].wv[0] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[1] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[2] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[3] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[4] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[5] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[6] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[7] = ((real32)bs.read_uint08())/255.0f;
                    if(bs.fail()) return error("Stream read failure.", __LINE__);
                   }
               }
             // XX 01 01 (0x18 VB)
             else if(format == 3)
               {
                // set UV flags
                vb.uvchan = 1;
                vb.uvtype[0] = AMC_DIFFUSE_MAP;

                // read vertices
                for(uint32 k = 0; k < vb.elem; k++)
                   {
                    // position
                    vb.data[k].vx = bs.read_real32();
                    vb.data[k].vy = bs.read_real32();
                    vb.data[k].vz = bs.read_real32();
                    vb.data[k].vw = 1.0f;
                    // normal (0x80/128 maps to +1) (0x7F/127 maps to -1)
                    vb.data[k].nx = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].ny = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nz = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nw = ((real32)bs.read_sint08())/128.0f;
                    // tangent
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    // uv channel 1
                    vb.data[k].uv[0][0] = bs.read_real16();
                    vb.data[k].uv[0][1] = bs.read_real16();
                   }
               }
             // XX 04 01 (0x1C VB)
             // sm0288_corridorbrakewall01a
             // sm0615_woodset01b
             else if(format == 4)
               {
                // set UV flags
                vb.uvchan = 2;
                vb.uvtype[0] = AMC_DIFFUSE_MAP;
                vb.uvtype[1] = AMC_DIFFUSE_MAP;

                // read vertices
                for(uint32 k = 0; k < vb.elem; k++)
                   {
                    // position
                    vb.data[k].vx = bs.read_real32();
                    vb.data[k].vy = bs.read_real32();
                    vb.data[k].vz = bs.read_real32();
                    vb.data[k].vw = 1.0f;
                    // normal (0x80/128 maps to +1) (0x7F/127 maps to -1)
                    vb.data[k].nx = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].ny = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nz = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nw = ((real32)bs.read_sint08())/128.0f;
                    // tangent
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    // uv channel 1
                    vb.data[k].uv[0][0] = bs.read_real16();
                    vb.data[k].uv[0][1] = bs.read_real16();
                    // uv channel 2??? this really a UV channel?
                    vb.data[k].uv[1][0] = bs.read_real16();
                    vb.data[k].uv[1][1] = bs.read_real16();
                   }
               }
             else
                return error("Unknown vertex buffer format.", __LINE__);

             // initialize index buffer
             AMC_IDXBUFFER ib;
             ib.format = (is_ib32 ? AMC_UINT32 : AMC_UINT16);
             ib.type = AMC_TRIANGLES;
             ib.wind = AMC_CW;
             ib.name = "default";
             ib.elem = n_indices;
             if(is_ib32) ib.data.reset(new char[ib.elem*sizeof(uint32)]);
             else ib.data.reset(new char[ib.elem*sizeof(uint16)]);

             // must have triangles
             if(ib.elem % 3) return error("Indices must be divisible by three.", __LINE__);

             // read index buffer
             if(is_ib32) cs.LE_read_array(reinterpret_cast<uint32*>(ib.data.get()), ib.elem);
             else cs.LE_read_array(reinterpret_cast<uint16*>(ib.data.get()), ib.elem);
             if(cs.fail()) return error("Stream read failure.", __LINE__);

             // initialize surface
             AMC_SURFACE s;
             stringstream sn;
             sn << "s_" << setfill('0') << setw(2) << i << "_";
             sn << "g_" << setfill('0') << setw(2) << j << "_";
             sn << "m_" << setfill('0') << setw(2) << k << "_";
             sn << "id_" << setfill('0') << setw(2) << index;

             s.name = sn.str();
             AMC_REFERENCE ref;
             ref.vb_index = mesh_index;
             ref.vb_start = 0;
             ref.vb_width = vb.elem;
             ref.ib_index = mesh_index;
             ref.ib_start = 0;
             ref.ib_width = ib.elem;
             ref.jm_index = (jmap.jntmap.size() ? i : AMC_INVALID_JMAP_INDEX);
             s.refs.push_back(ref);
             s.surfmat = AMC_INVALID_SURFMAT;

             //cout << "i j k = " << i << " " << j << " " << k << endl;
             //cout << "vb_offset = 0x" << hex << vb_offset << dec << endl;
             //cout << "ib_offset = 0x" << hex << ib_offset << dec << endl;
             //cout << "ref.vb_start = 0x" << hex << ref.vb_start << dec << endl;
             //cout << "ref.vb_width = 0x" << hex << ref.vb_width << dec << endl;
             //cout << "ref.ib_start (offset) = 0x" << hex << 2*ref.ib_start << dec << endl;
             //cout << "ref.ib_width = 0x" << hex << ref.ib_width << dec << endl;
             //cout << endl;

             // insert buffers
             amc1.vblist.push_back(vb);
             amc1.iblist.push_back(ib);
             amc1.surfaces.push_back(s);

             // increment mesh index
             mesh_index++;
            }
        }
    }

 //
 // 2ND MESH
 //

 // struct RE7MESHINFO1
 // uint08 h01; // 03 number on entries
 // uint08 h02; // 09
 // uint08 h03; // 02
 // uint08 h04; // 01
 // uint32 h05; // 10 00 00 00
 // uint32 h06; // 00 00 00 00
 // uint32 h07; // 00 00 00 00
 // real32 h08[12];
 // uint64 h09; // C0 00 00 00 00 00 00 00 offset
 // std::vector<uint64> offsets; // offsets to surfaces
 // std::vector<RE7SURFACE> surfaces; // surface list

 // struct RE7MESHINFO2
 // uint08 h01;     // 03 - same as RE7MESHINFO1
 // uint08 h02;     // 09 - same as RE7MESHINFO1
 // uint08 h03;     // 02 - same as RE7MESHINFO1
 // uint08 h04;     // 01 - same as RE7MESHINFO1
 // uint32 h05;     // 0B 00 00 00 - same as RE7MESHINFO1
 // uint32 h06;     // 00 00 00 00 - same as RE7MESHINFO1
 // uint32 h07;     // 00 00 00 00 - same as RE7MESHINFO1
 // uint64 h08;     // 38 0B 00 00 00 00 00 00 offset
 // uint32 h09[12]; // 00 00 00 00
 // std::vector<uint64> offsets;     // offsets to surfaces
 // std::vector<RE7SURFACE> surfaces; // surface list

 // not necessary
 if(!header.h07) return true;

 // for each surface
 mesh_index = 0;
 for(uint32 i = 0; i < meshinfo2.surfaces.size(); i++)
    {
     // construct joint map
     AMC_JOINTMAP jmap;
     for(uint32 j = 0; j < meshinfo2.surfaces[i].h04; j++) jmap.jntmap.push_back(meshinfo2.surfaces[i].jntmap[j]);
     amc2.jmaplist.push_back(jmap);

     // determine vertex buffer format
     uint32 format = 0;

     // XX 01 01
     bool has_jntmap = (meshinfo2.surfaces[i].jntmap.size() != 0);
     if(meshinfo2.h03 == 1 && meshinfo2.h04 == 1) {
        if(has_jntmap) format = 2; // skeletal (0x28)
        else format = 3; // static (0x18)
       }
     // XX 02 01
     else if(meshinfo2.h03 == 2 && meshinfo2.h04 == 1) {
        if(has_jntmap) format = 1; // skeletal (0x2C)
        else format = 4; // static (0x1C)
       }
     // XX 03 01
     else if(meshinfo2.h03 == 3 && meshinfo2.h04 == 1) {
        if(has_jntmap) format = 1; // skeletal (0x2C)
        else format = 4; // static (0x1C)
       }
     // XX 04 01
     else if(meshinfo2.h03 == 4 && meshinfo2.h04 == 1) {
        if(has_jntmap) format = 1; // skeletal (0x2C)
        else format = 4; // static (0x1C)
       }

     // for each surface group
     for(uint32 j = 0; j < meshinfo2.surfaces[i].groups.size(); j++)
        {
         // number of points and vertices (overall in group)
         uint32 n_vtx = meshinfo2.surfaces[i].groups[j].h06;
         uint32 n_idx = meshinfo2.surfaces[i].groups[j].h07;

         // initialize surfaces
         for(uint32 k = 0; k < meshinfo2.surfaces[i].groups[j].meshlist.size(); k++)
            {
             uint32 index = meshinfo2.surfaces[i].groups[j].meshlist[k].p01;
             uint32 n_indices = meshinfo2.surfaces[i].groups[j].meshlist[k].p02;
             uint32 base_idx = meshinfo2.surfaces[i].groups[j].meshlist[k].p03;
             uint32 base_vtx = meshinfo2.surfaces[i].groups[j].meshlist[k].p04;

             // compute number of vertices in the vertex buffer since it is not given
             uint32 n_verts = 0;
             if(k == (meshinfo2.surfaces[i].groups[j].meshlist.size() - 1)) {
                uint32 total = n_vtx + meshinfo2.surfaces[i].groups[j].meshlist[0].p04;
                n_verts = total - meshinfo2.surfaces[i].groups[j].meshlist[k].p04;
               }
             else
                n_verts = meshinfo2.surfaces[i].groups[j].meshlist[k + 1].p04 - meshinfo2.surfaces[i].groups[j].meshlist[k].p04;

             // debug: save offsets
             size_t vb_offset = bs.tell();
             size_t ib_offset = cs.tell();

             // initialize vertex buffer
             AMC_VTXBUFFER vb;
             vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
             vb.name = "default";
             vb.uvchan = 0;
             vb.uvtype[0] = AMC_INVALID_MAP;
             for(uint32 i = 1; i < 8; i++) vb.uvtype[i] = AMC_INVALID_MAP;
             vb.colors = 0;
             vb.elem = n_verts;
             vb.data.reset(new AMC_VERTEX[vb.elem]);

             // XX 02 01 (0x2C VB)
             if(format == 1)
               {
                // set VB flags
                vb.flags |= AMC_VERTEX_WEIGHTS;

                // set UV flags
                vb.uvchan = 2;
                vb.uvtype[0] = AMC_DIFFUSE_MAP;
                vb.uvtype[1] = AMC_DIFFUSE_MAP;

                // read vertices
                for(uint32 k = 0; k < vb.elem; k++)
                   {
                    // position
                    vb.data[k].vx = bs.read_real32();
                    vb.data[k].vy = bs.read_real32();
                    vb.data[k].vz = bs.read_real32();
                    vb.data[k].vw = 1.0f;
                    // normal (0x80/128 maps to +1) (0x7F/127 maps to -1)
                    vb.data[k].nx = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].ny = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nz = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nw = ((real32)bs.read_sint08())/128.0f;
                    // tangent
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    // uv channel 1
                    vb.data[k].uv[0][0] = bs.read_real16();
                    vb.data[k].uv[0][1] = bs.read_real16();
                    // uv channel 2
                    vb.data[k].uv[1][0] = bs.read_real16();
                    vb.data[k].uv[1][1] = bs.read_real16();
                    // blend indices
                    vb.data[k].wi[0] = bs.read_uint08();
                    vb.data[k].wi[1] = bs.read_uint08();
                    vb.data[k].wi[2] = bs.read_uint08();
                    vb.data[k].wi[3] = bs.read_uint08();
                    vb.data[k].wi[4] = bs.read_uint08();
                    vb.data[k].wi[5] = bs.read_uint08();
                    vb.data[k].wi[6] = bs.read_uint08();
                    vb.data[k].wi[7] = bs.read_uint08();
                    // blend weights
                    vb.data[k].wv[0] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[1] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[2] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[3] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[4] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[5] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[6] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[7] = ((real32)bs.read_uint08())/255.0f;
                   }
               }
             // XX 01 01 (0x28 VB)
             else if(format == 2)
               {
                // set VB flags
                vb.flags |= AMC_VERTEX_WEIGHTS;

                // set UV flags
                vb.uvchan = 1;
                vb.uvtype[0] = AMC_DIFFUSE_MAP;

                // read vertices
                for(uint32 k = 0; k < vb.elem; k++)
                   {
                    // position
                    vb.data[k].vx = bs.read_real32();
                    vb.data[k].vy = bs.read_real32();
                    vb.data[k].vz = bs.read_real32();
                    vb.data[k].vw = 1.0f;
                    // normal (0x80/128 maps to +1) (0x7F/127 maps to -1)
                    vb.data[k].nx = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].ny = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nz = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nw = ((real32)bs.read_sint08())/128.0f;
                    // tangent
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    // uv channel 1
                    vb.data[k].uv[0][0] = bs.read_real16();
                    vb.data[k].uv[0][1] = bs.read_real16();
                    // blend indices
                    vb.data[k].wi[0] = bs.read_uint08();
                    vb.data[k].wi[1] = bs.read_uint08();
                    vb.data[k].wi[2] = bs.read_uint08();
                    vb.data[k].wi[3] = bs.read_uint08();
                    vb.data[k].wi[4] = bs.read_uint08();
                    vb.data[k].wi[5] = bs.read_uint08();
                    vb.data[k].wi[6] = bs.read_uint08();
                    vb.data[k].wi[7] = bs.read_uint08();
                    // blend weights
                    vb.data[k].wv[0] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[1] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[2] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[3] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[4] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[5] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[6] = ((real32)bs.read_uint08())/255.0f;
                    vb.data[k].wv[7] = ((real32)bs.read_uint08())/255.0f;
                    if(bs.fail()) return error("Stream read failure.", __LINE__);
                   }
               }
             // XX 01 01 (0x18 VB)
             else if(format == 3)
               {
                // set UV flags
                vb.uvchan = 1;
                vb.uvtype[0] = AMC_DIFFUSE_MAP;

                // read vertices
                for(uint32 k = 0; k < vb.elem; k++)
                   {
                    // position
                    vb.data[k].vx = bs.read_real32();
                    vb.data[k].vy = bs.read_real32();
                    vb.data[k].vz = bs.read_real32();
                    vb.data[k].vw = 1.0f;
                    // normal (0x80/128 maps to +1) (0x7F/127 maps to -1)
                    vb.data[k].nx = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].ny = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nz = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nw = ((real32)bs.read_sint08())/128.0f;
                    // tangent
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    // uv channel 1
                    vb.data[k].uv[0][0] = bs.read_real16();
                    vb.data[k].uv[0][1] = bs.read_real16();
                   }
               }
             // XX 04 01 (0x1C VB)
             // sm0288_corridorbrakewall01a
             // sm0615_woodset01b
             else if(format == 4)
               {
                // set UV flags
                vb.uvchan = 2;
                vb.uvtype[0] = AMC_DIFFUSE_MAP;
                vb.uvtype[1] = AMC_DIFFUSE_MAP;

                // read vertices
                for(uint32 k = 0; k < vb.elem; k++)
                   {
                    // position
                    vb.data[k].vx = bs.read_real32();
                    vb.data[k].vy = bs.read_real32();
                    vb.data[k].vz = bs.read_real32();
                    vb.data[k].vw = 1.0f;
                    // normal (0x80/128 maps to +1) (0x7F/127 maps to -1)
                    vb.data[k].nx = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].ny = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nz = ((real32)bs.read_sint08())/128.0f;
                    vb.data[k].nw = ((real32)bs.read_sint08())/128.0f;
                    // tangent
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    bs.read_uint08();
                    // uv channel 1
                    vb.data[k].uv[0][0] = bs.read_real16();
                    vb.data[k].uv[0][1] = bs.read_real16();
                    // uv channel 2??? this really a UV channel?
                    vb.data[k].uv[1][0] = bs.read_real16();
                    vb.data[k].uv[1][1] = bs.read_real16();
                   }
               }
             else
                return error("Unknown vertex buffer format.", __LINE__);

             // initialize index buffer
             AMC_IDXBUFFER ib;
             ib.format = (is_ib32 ? AMC_UINT32 : AMC_UINT16);
             ib.type = AMC_TRIANGLES;
             ib.wind = AMC_CW;
             ib.name = "default";
             ib.elem = n_indices;
             if(is_ib32) ib.data.reset(new char[ib.elem*sizeof(uint32)]);
             else ib.data.reset(new char[ib.elem*sizeof(uint16)]);

             // must have triangles
             if(ib.elem % 3) return error("Indices must be divisible by three.", __LINE__);

             // read index buffer
             if(is_ib32) cs.LE_read_array(reinterpret_cast<uint32*>(ib.data.get()), ib.elem);
             else cs.LE_read_array(reinterpret_cast<uint16*>(ib.data.get()), ib.elem);
             if(cs.fail()) return error("Stream read failure.", __LINE__);

             // initialize surface
             AMC_SURFACE s;
             stringstream sn;
             sn << "s_" << setfill('0') << setw(2) << i << "_";
             sn << "g_" << setfill('0') << setw(2) << j << "_";
             sn << "m_" << setfill('0') << setw(2) << k << "_";
             sn << "id_" << setfill('0') << setw(2) << index;

             s.name = sn.str();
             AMC_REFERENCE ref;
             ref.vb_index = mesh_index;
             ref.vb_start = 0;
             ref.vb_width = vb.elem;
             ref.ib_index = mesh_index;
             ref.ib_start = 0;
             ref.ib_width = ib.elem;
             ref.jm_index = (jmap.jntmap.size() ? i : AMC_INVALID_JMAP_INDEX);
             s.refs.push_back(ref);
             s.surfmat = AMC_INVALID_SURFMAT;

             //cout << "i j k = " << i << " " << j << " " << k << endl;
             //cout << "vb_offset = 0x" << hex << vb_offset << dec << endl;
             //cout << "ib_offset = 0x" << hex << ib_offset << dec << endl;
             //cout << "ref.vb_start = 0x" << hex << ref.vb_start << dec << endl;
             //cout << "ref.vb_width = 0x" << hex << ref.vb_width << dec << endl;
             //cout << "ref.ib_start (offset) = 0x" << hex << 2*ref.ib_start << dec << endl;
             //cout << "ref.ib_width = 0x" << hex << ref.ib_width << dec << endl;
             //cout << endl;

             // insert buffers
             amc2.vblist.push_back(vb);
             amc2.iblist.push_back(ib);
             amc2.surfaces.push_back(s);

             // increment mesh index
             mesh_index++;
            }
        }
    }

 return true;
}

bool RE7ModelProcessor::run(void)
{
 // open file
 using namespace std;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // determine filesize
 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // filename properties
 pathname = GetPathnameFromFilename(filename.c_str()).get();
 shrtname = GetShortFilenameWithoutExtension(filename.c_str()).get();

 // create debug file
 if(debug) {
    STDSTRING dpath = pathname + shrtname + TEXT(".debug");
    dfile.open(dpath.c_str());
    if(!dfile) return error("Failed to create debug file", __LINE__);
   }

 // phases
 if(!read_header()) return false;
 if(!read_stringtable()) return false;
 if(!read_skeleton()) return false;
 if(!read_buffers()) return false;
 if(!read_meshinfo1()) return false;
 if(!read_meshinfo2()) return false;
 if(!construct_skeleton()) return false;
 if(!index_buffer_heuristic()) return false;
 if(!construct()) return false;

 // save 1st AMC
 // auto transformed = TransformAMC(amc);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc1);
 SaveAMC(pathname.c_str(), shrtname.c_str(), amc1);

 // save 2nd AMC
 if(header.h07) {
    STDSTRINGSTREAM ss;
    ss << shrtname << TEXT("_2nd");
    SaveOBJ(pathname.c_str(), ss.str().c_str(), amc2);
    SaveAMC(pathname.c_str(), ss.str().c_str(), amc2);
   }

 // success
 return true;
}

bool ProcessMES(LPCTSTR filename, endian_mode em)
{
 RE7ModelProcessor mp(filename, em);
 return mp.run();
}

}}

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Resident Evil 7");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Copy PAK files to new folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where PAK files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need 18 GB free space (beta).\n");
 p3 += TEXT("3. 4 GB for game + 14 GB extraction (beta).\n");
 p3 += TEXT("4. You need 116 GB free space (retail).\n");
 p3 += TEXT("5. 20 GB for game + 96 GB extraction (retail).\n");

 // insert title
 AddGameTitle(p1.c_str(), p2.c_str(), p3.c_str(), extract);
 return true;
}

bool extract(void)
{
 LPCTSTR pathname = GetModulePathname().get();
 return extract(pathname);
}

bool extract(LPCTSTR pname)
{
 // set pathname
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // booleans
 bool doPAK = true;
 bool doTX8 = true;
 bool doFBF = false;
 bool doM32 = true;

 // questions
 if(doPAK) doPAK = YesNoBox("Process archive (PAK) files?");
 if(doTX8) doTX8 = YesNoBox("Process texture (TEX, TEX.8, TEX.8.x64) files?");
 if(doM32) doM32 = YesNoBox("Process model (MESH, MES, MESH.32) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // archives
 if(doPAK) {
    cout << "Processing .PAK files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".PAK"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessPAK(filelist[i].c_str())) return false;
       }
    cout << endl;
   }
 if(doPAK) {
    cout << "Processing .RE7 files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".RE7"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessRE7(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // textures
 if(doTX8) {
    cout << "Processing .TEX files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".TEX"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessTEX(filelist[i].c_str())) return false;
       }
    cout << endl;
   }
 if(doTX8) {
    cout << "Processing TEX.8 files..." << endl;
    deque<STDSTRING> templist;
    BuildFilenameList(templist, TEXT(".8"), pathname.c_str());
    deque<STDSTRING> filelist;
    for(size_t i = 0; i < templist.size(); i++) {
        if(templist[i].find(TEXT(".tex.8")) != templist[i].npos) filelist.push_back(templist[i]);
       }
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessTEX(filelist[i].c_str())) return false;
       }
    cout << endl;
   }
 if(doTX8) {
    cout << "Processing TEX.8.x64 files..." << endl;
    deque<STDSTRING> templist;
    BuildFilenameList(templist, TEXT(".x64"), pathname.c_str());
    deque<STDSTRING> filelist;
    for(size_t i = 0; i < templist.size(); i++) {
        if(templist[i].find(TEXT(".tex.8.x64")) != templist[i].npos) filelist.push_back(templist[i]);
       }
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessTEX(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 if(doFBF) {
    cout << "Processing OFT.1 files..." << endl;
    deque<STDSTRING> templist;
    BuildFilenameList(templist, TEXT(".1"), pathname.c_str());
    deque<STDSTRING> filelist;
    for(size_t i = 0; i < templist.size(); i++) {
        if(templist[i].find(TEXT(".oft.1")) != templist[i].npos) filelist.push_back(templist[i]);
       }
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessFBF(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process models
 if(doM32) {
    cout << "Processing MESH files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".MESH"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessMES(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
   }
 if(doM32) {
    cout << "Processing MES files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".MES"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessMES(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
   }
 if(doM32) {
    cout << "Processing MESH.32 files..." << endl;
    deque<STDSTRING> templist;
    BuildFilenameList(templist, TEXT(".32"), pathname.c_str());
    deque<STDSTRING> filelist;
    for(size_t i = 0; i < templist.size(); i++)
        if(templist[i].find(TEXT(".mesh.32")) != templist[i].npos) filelist.push_back(templist[i]);
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessMES(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
   }

 // record end time
 uint64 t1 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t1);

 // determine time
 uint64 hz;
 QueryPerformanceFrequency((LARGE_INTEGER*)&hz);
 uint64 dt = (t1 - t0);
 real64 dt_sec = (real64)(t1 - t0)/(real64)hz;
 real64 dt_min = dt_sec/60.0;
 real64 dt_hrs = dt_min/60.0;
 cout << "Elapsed time = " << dt_min << " minutes." << endl;

 return true;
}

}};