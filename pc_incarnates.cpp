#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "pc_incarnates.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   RISE_OF_INCARNATES

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool validateEXT(boost::shared_array<char> data, uint32 size)
{
 binary_stream bs(data, size);
 uint32 magic = bs.BE_read_uint32();
 switch(magic) {
   case(0x00736D63) : return true; // CMS0
   case(0x006A6F63) : return true; // COJ0
   case(0x40555446) : return true; // CPK0
   case(0x00767263) : return true; // CRV0
   case(0x00647363) : return true; // CSD0
   case(0x00687363) : return true; // CSH0
   case(0x6D687363) : return true; // CSHM
   case(0x00746164) : return true; // DAT0
   case(0x00666665) : return true; // EFF0
   case(0x00706668) : return true; // HFP0
   case(0x00647568) : return true; // HUD0
   case(0x0052334D) : return true; // M3R0
   case(0x006C626D) : return true; // MBL0
   case(0x0063646D) : return true; // MDC0
   case(0x006C646D) : return true; // MDL0
   case(0x006B696D) : return true; // MIK0
   case(0x006B706D) : return true; // MPK0
   case(0x7071736D) : return true; // MSQP
   case(0x7173746D) : return true; // MTSQ
   case(0x0077766D) : return true; // MVW0
   case(0x00646D70) : return true; // PMD0
   case(0x00627370) : return true; // PSB0
   case(0x00435352) : return true; // RSC0
   case(0x00535352) : return true; // RSS0
   case(0x00647472) : return true; // RTD0
   case(0x006B7073) : return true; // SPK0
   case(0x006C6274) : return true; // TBL0
   case(0x00786574) : return true; // TEX0
   case(0x006B6974) : return true; // TIK0
   case(0x00627874) : return true; // TXB0
   case(0x00627376) : return true; // VSB0
   default : return false;
  }
 return false;
}

STDSTRING determineEXT(boost::shared_array<char> data, uint32 size)
{
 binary_stream bs(data, size);
 uint32 magic = bs.BE_read_uint32();
 switch(magic) {
   case(0x00736D63) : return STDSTRING(TEXT("CMS0"));
   case(0x006A6F63) : return STDSTRING(TEXT("COJ0"));
   case(0x40555446) : return STDSTRING(TEXT("CPK0"));
   case(0x00767263) : return STDSTRING(TEXT("CRV0"));
   case(0x00647363) : return STDSTRING(TEXT("CSD0"));
   case(0x00687363) : return STDSTRING(TEXT("CSH0"));
   case(0x6D687363) : return STDSTRING(TEXT("CSHM"));
   case(0x00746164) : return STDSTRING(TEXT("DAT0"));
   case(0x00666665) : return STDSTRING(TEXT("EFF0"));
   case(0x00706668) : return STDSTRING(TEXT("HFP0"));
   case(0x00647568) : return STDSTRING(TEXT("HUD0"));
   case(0x0052334D) : return STDSTRING(TEXT("M3R0"));
   case(0x006C626D) : return STDSTRING(TEXT("MBL0"));
   case(0x0063646D) : return STDSTRING(TEXT("MDC0"));
   case(0x006C646D) : return STDSTRING(TEXT("MDL0"));
   case(0x006B696D) : return STDSTRING(TEXT("MIK0"));
   case(0x006B706D) : return STDSTRING(TEXT("MPK0"));
   case(0x7071736D) : return STDSTRING(TEXT("MSQP"));
   case(0x7173746D) : return STDSTRING(TEXT("MTSQ"));
   case(0x0077766D) : return STDSTRING(TEXT("MVW0"));
   case(0x00646D70) : return STDSTRING(TEXT("PMD0"));
   case(0x00627370) : return STDSTRING(TEXT("PSB0"));
   case(0x00435352) : return STDSTRING(TEXT("RSC0"));
   case(0x00535352) : return STDSTRING(TEXT("RSS0"));
   case(0x00647472) : return STDSTRING(TEXT("RTD0"));
   case(0x006B7073) : return STDSTRING(TEXT("SPK0"));
   case(0x006C6274) : return STDSTRING(TEXT("TBL0"));
   case(0x00786574) : return STDSTRING(TEXT("TEX0"));
   case(0x006B6974) : return STDSTRING(TEXT("TIK0"));
   case(0x00627874) : return STDSTRING(TEXT("TXB0"));
   case(0x00627376) : return STDSTRING(TEXT("VSB0"));
   return STDSTRING(TEXT("UNK0"));
  }
 return STDSTRING(TEXT("UNK0"));
}

bool processDAT(const STDSTRING& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open GDAT file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename.c_str()).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename.c_str()).get();

 // read GDAT
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x54414447) return error("Not a GDAT file.");

 // read number of entries
 uint32 n_entries = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(n_entries == 0) return true; // nothing to do

 // read entries
 struct GDATENTRY {
  uint32 p01; // offset
  uint32 p02; // size
 };
 deque<GDATENTRY> entrylist;
 for(uint32 i = 0; i < n_entries; i++) {
     GDATENTRY item;
     item.p01 = LE_read_uint32(ifile);
     item.p02 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     entrylist.push_back(item);
    }

 // compute first expected offset
 uint32 first = 0x08 + (n_entries * 0x08);
 if(entrylist[0].p01 < first) return error("Invalid first offset.");

 // create directory to store extracted files
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // let's process the entries
 for(uint32 i = 0; i < n_entries; i++)
    {
     // nothing to do
     if(entrylist[i].p02 < 0x04)
        continue;

     // seek offset
     ifile.seekg(entrylist[i].p01);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[entrylist[i].p02]);
     ifile.read(data.get(), entrylist[i].p02);
     if(ifile.fail()) return error("Read failure.");

     // get file extension
     STDSTRING extension = determineEXT(data, entrylist[i].p02);
     if(!extension.length()) return error("Invalid filename extension.");

     // construct filename
     STDSTRINGSTREAM ofname;
     ofname << savepath;
     ofname << setfill(TEXT('0'));
     ofname << setw(4) << hex << i << dec;
     ofname << TEXT(".") << extension;

     // create output file
     ofstream ofile(ofname.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create file.");

     // save data
     ofile.write(data.get(), entrylist[i].p02);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
}

bool processTEX(const STDSTRING& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename.c_str()).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename.c_str()).get();

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x00786574) return error("Not a TEX0 file.");

 // move to BILZ
 ifile.seekg(0x80);
 if(ifile.fail()) return error("Seek failure.");

 // read BILZ header
 uint32 h01 = BE_read_uint32(ifile); // 0x42494C5A
 uint32 h02 = BE_read_uint32(ifile); // inflated size
 uint32 h03 = BE_read_uint32(ifile); // deflated size
 uint32 h04 = BE_read_uint32(ifile); // 0x07

 // construct filename
 STDSTRINGSTREAM ofname;
 ofname << pathname;
 ofname << shrtname;
 ofname << TEXT(".tex1");

 // non-compressed
 if(h02 == h03)
   {
    // read data
    boost::shared_array<char> data(new char[h02]);
    ifile.read(data.get(), h02);
    if(ifile.fail()) return error("Read failure.");

    // create output file
    ofstream ofile(ofname.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");

    // save data
    ofile.write(data.get(), h02);
    if(ofile.fail()) return error("Write failure.");
   }
 // compressed
 else if(h03 < h02)
   {
    // create output file
    ofstream ofile(ofname.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");

    // initialize inflate data
    ZLIBINFO2 info;
    info.inflatedBytes = h02;
    info.deflatedBytes = h03;
    info.offset = 0x90;

    // inflate data
    deque<ZLIBINFO2> items;
    items.push_back(info);
    if(!InflateZLIB(ifile, items, 0, ofile)) return false;
   }
 // WTF?
 else
    return error("Cannot determine if file is compressed or not.");

 return true;
}

bool processMDL(const STDSTRING& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename.c_str()).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename.c_str()).get();

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x006C646D) return error("Not a MDL0 file.");

 // move to BILZ
 ifile.seekg(0x80);
 if(ifile.fail()) return error("Seek failure.");

 // read BILZ header
 uint32 h01 = BE_read_uint32(ifile); // 0x42494C5A
 uint32 h02 = BE_read_uint32(ifile); // inflated size
 uint32 h03 = BE_read_uint32(ifile); // deflated size
 uint32 h04 = BE_read_uint32(ifile); // 0x07

 // construct filename
 STDSTRINGSTREAM ofname;
 ofname << pathname;
 ofname << shrtname;
 ofname << TEXT(".mdl1");

 // non-compressed
 if(h02 == h03)
   {
    // read data
    boost::shared_array<char> data(new char[h02]);
    ifile.read(data.get(), h02);
    if(ifile.fail()) return error("Read failure.");

    // create output file
    ofstream ofile(ofname.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");

    // save data
    ofile.write(data.get(), h02);
    if(ofile.fail()) return error("Write failure.");
   }
 // compressed
 else if(h03 < h02)
   {
    // create output file
    ofstream ofile(ofname.str().c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");

    // initialize inflate data
    ZLIBINFO2 info;
    info.inflatedBytes = h02;
    info.deflatedBytes = h03;
    info.offset = 0x90;

    // inflate data
    deque<ZLIBINFO2> items;
    items.push_back(info);
    if(!InflateZLIB(ifile, items, 0, ofile)) return false;
   }
 // WTF?
 else
    return error("Cannot determine if file is compressed or not.");

 return true;
}

}};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processTX1(const STDSTRING& filename)
{
 // FLAG EXAMPLE
 // 12 41 30 11
 // 12 = simple texture
 // 41 = mipmap flag
 // 30 = grayscale texture
 // 11 = ???

 // MIPMAP EXAMPLE
 // 0x400 by 0x400
 // format is DXT1
 // mipmap flag is 0x43
 // 0x400 by 0x400 = 0x80000 (1)
 // 0x200 by 0x200 = 0x20000 (2) (0xA0000)
 // 0x100 by 0x100 = 0x08000 (3) (0xA8000)

 // MIPMAP EXAMPLE
 // 00ab.tex
 // DXT1
 // filesize = 0x55568
 // 00 04
 // 00 02
 // 12 4B 00 BD
 // sizes are
 // 400 x 200 - 1 - 0x40000 total
 // 200 x 100 - 2 - 0x50000 total
 // 100 x 080 - 3 - 0x54000 total
 // 080 x 040 - 4 - 0x55000 total
 // 040 x 020 - 5 - 0x55400 total
 // 020 x 010 - 6 - 0x55500 total
 // 010 x 008 - 7 - 0x55540 total
 // 008 x 004 - 8 - 0x55550 total
 // 004 x 004 - 9 - 0x55558 total
 // 002 x 002 - A - 0x55560 total
 // 001 x 001 - B - 0x55568 total

 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename.c_str()).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename.c_str()).get();

 // determine filesize
 uint64 filesize = GetFileSize(ifile);
 if(filesize == numeric_limits<uint64>::max()) return error("Failed to retrive filesize."); 

 // read dimensions
 uint16 dx = LE_read_uint16(ifile);
 uint16 dy = LE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read flags
 uint32 flag1 = LE_read_uint32(ifile);
 uint32 flag2 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // set image type
 uint32 type = (flag1 & 0xFF);
 if(type == 0x12) ; // single image
 else if(type == 0x64) ; // cubemap
 else return error("Unknown image type.");

 // set image format
 uint32 format = (flag1 & 0xFF0000ul) >> 16;
 if(format == 0x00) ; // DXT1
 else if(format == 0x10) ; // DXT3 (hair)
 else if(format == 0x20) ; // DXT5
 else if(format == 0x30) ; // grayscale (text, no mipmaps, 8-bits per pixel)
 else return error("Unknown image format.");

 // set number of mipmaps
 uint32 mflag = (flag1 & 0xFF00ul) >> 8;
 if(mflag < 0x41) return error("Invalid number of mipmaps.");
 if(mflag > 0x50) return error("Invalid number of mipmaps.");
 uint32 mipmaps = mflag - 0x40;

 // read image data
 uint32 datasize = static_cast<uint32>(filesize) - 0xC;
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 if(ifile.fail()) return error("Read failure.");

 // create output filename
 STDSTRING ofname = pathname;
 ofname += shrtname;
 ofname += TEXT(".dds");

 // create DDS header
 DDS_HEADER ddsh;
 BOOL cubemap = ((type == 0x64) ? TRUE : FALSE);

 // DXT1
 if(format == 0x00) {
    if(!CreateDXT1Header(dx, dy, mipmaps - 1, cubemap, &ddsh)) return error("Failed to create DDS file header.");
    if(!SaveDDSFile(ofname.c_str(), ddsh, data, datasize)) return error("Failed to save DDS file.");
   }
 // DXT3 (hair)
 else if(format == 0x10) {
    if(!CreateDXT3Header(dx, dy, mipmaps - 1, cubemap, &ddsh)) return error("Failed to create DDS file header.");
    if(!SaveDDSFile(ofname.c_str(), ddsh, data, datasize)) return error("Failed to save DDS file.");
   }
 // DXT5
 else if(format == 0x20) {
    if(!CreateDXT5Header(dx, dy, mipmaps - 1, cubemap, &ddsh)) return error("Failed to create DDS file header.");
    if(!SaveDDSFile(ofname.c_str(), ddsh, data, datasize)) return error("Failed to save DDS file.");
   }
 // R8 (grayscale text, no mipmaps, 8-bits per pixel)
 else if(format == 0x30) {
    cout << "GRAYSCALE!" << endl;
    if(!CreateGrayscaleDDSHeader(dx, dy, mipmaps - 1, cubemap, &ddsh)) return error("Failed to create DDS file header.");
    if(!SaveDDSFile(ofname.c_str(), ddsh, data, datasize)) return error("Failed to save DDS file.");
   }

 return true;
}

}};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

bool processMD1(const STDSTRING& filename)
{
 // 0x04E6.MDL1
 // header -> 0x2C bytes
 // string table -> 0x9B0 bytes
 // index table -> 0x160 bytes
 // skeleton data part 1 -> 0xC9 * 0x40 bytes = 0x3240 bytes
 // skeleton data part 2 -> 0xC9 * 0x40 bytes = 0x3240 bytes
 // skeleton data part 3 -> 0xC9 * 0x40 bytes = 0x3240 bytes

 // skeleton data parts two and three have the following 0x40-byte per item format
 // there is a vector and 
 // <0000803f-00000000-00000000>-<0d0a0909>
 // <00000000-0000803f-00000000>-<e7a4bae8>
 // <00000000-00000000-0000803f>-<2073632e>
 // <00000000-00000000-00000000>-<00000000>

 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename.c_str()).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename.c_str()).get();

 // debug file
 bool debug = true;
 ofstream dfile;
 if(debug) {
    STDSTRING dname = pathname;
    dname += shrtname;
    dname += TEXT(".txt");
    dfile.open(dname.c_str());
    if(!dfile.is_open()) return error("Failed to create debug file.");
   }

 // determine filesize
 uint64 filesize = GetFileSize(ifile);
 if(filesize == numeric_limits<uint64>::max()) return error("Failed to retrive filesize."); 

 // read header 
 // size is 0x2C bytes
 uint32 h01 = LE_read_uint32(ifile); // size of string table (starting at 0x2C)
 uint32 h02 = LE_read_uint32(ifile); // size of some kind of index table that references string table
 uint32 h03 = LE_read_uint32(ifile); // number of items in string table
 uint32 h04 = LE_read_uint32(ifile); // ????
 uint32 h05 = LE_read_uint32(ifile); // ???? usually 0, an array of floats
 uint32 h06 = LE_read_uint32(ifile); // ????
 uint32 h07 = LE_read_uint32(ifile); // number of 0x70-byte items
 uint32 h08 = LE_read_uint32(ifile); // number of 0x34-byte items (surface information) notice first two uint32 sum to next uint32 on next line
 uint32 h09 = LE_read_uint32(ifile); // size of vertex buffer data
 uint32 h10 = LE_read_uint32(ifile); // size of index buffer data
 uint32 h11 = LE_read_uint32(ifile); // size of material data
 if(ifile.fail()) return error("Read failure.");

 // validate header
 if(!h08) return true; // nothing to do; no surfaces
 if(!h09) return true; // nothing to do; no vertex buffer
 if(!h10) return true; // nothing to do; no index buffer

 // move to string table
 uint32 curr_offset = 0x2C;
 ifile.seekg(curr_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read string table
 deque<std::string> strtable;
 for(uint32 i = 0; i < h03; i++) {
     char data[256];
     if(!read_string(ifile, &data[0], 256)) return error("Read failure.");
     strtable.push_back(data);
    }

 // debug string table
 if(debug) {
    dfile << "--------------" << endl;
    dfile << " STRING TABLE " << endl;
    dfile << "--------------" << endl;
    dfile << endl;
    dfile << "Number of strings = 0x" << hex << strtable.size() << dec << endl;
    dfile << endl;
    for(uint32 i = 0; i < h03; i++)
        dfile << "strtable[0x" << hex << i << dec << "] = " << strtable[i] << endl;
    dfile << endl;
   }

 // move to index list
 curr_offset += h01;
 ifile.seekg(curr_offset);
 if(ifile.fail()) return error("Seek failure.");

 // compute number of indices and read indices
 // the number of indices can be zero
 if(h02 % 2) return error("Invalid index table size.");
 uint32 indextablesize = h02/2;
 boost::shared_array<uint16> indextable;
 if(indextablesize) {
    indextable.reset(new uint16[indextablesize]);
    if(!LE_read_array(ifile, indextable.get(), indextablesize))
       return error("Read failure.");
   }

 // debug index table
 if(debug) {
    dfile << "-------------" << endl;
    dfile << " INDEX TABLE " << endl;
    dfile << "-------------" << endl;
    dfile << endl;
    dfile << "Number of indices = 0x" << hex << indextablesize << dec << endl;
    dfile << endl;
    for(uint32 i = 0; i < indextablesize; i++) {
        dfile << "index[0x" << hex << i << dec;
        dfile << "] = 0x" << hex << indextable[i] << dec;
        dfile << " : " << strtable[indextable[i]] << endl;
       }
    dfile << endl;
   }

 // move to skeleton data
 curr_offset += h02;
 ifile.seekg(curr_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read skeleton data (if it exists and occurs in three sets of 0x40-bytes per item)
 boost::shared_array<char> bonedata1;
 boost::shared_array<char> bonedata2;
 boost::shared_array<char> bonedata3;
 uint32 bonedatasize = 0x40 * h03;
 if(bonedatasize) {
    bonedata1.reset(new char[bonedatasize]);
    bonedata2.reset(new char[bonedatasize]);
    bonedata3.reset(new char[bonedatasize]);
    ifile.read(bonedata1.get(), bonedatasize);
    ifile.read(bonedata2.get(), bonedatasize);
    ifile.read(bonedata3.get(), bonedatasize);
    if(ifile.fail()) return error("Read failure.");
   }

 // debug skeleton data
 if(debug && h03) {
    dfile << "------------------" << endl;
    dfile << " SKELETON DATA #1 " << endl;
    dfile << "------------------" << endl;
    dfile << endl;
    // hex format
    dfile << setfill('0');
    binary_stream bs(bonedata1, bonedatasize);
    for(uint32 i = 0; i < h03; i++) {
        dfile << "0x" << setw(2) << hex << i << dec << ": ";
        for(uint32 j = 0x00; j < 0x40; j++) {
            uint16 byte = bs.LE_read_uint08();
            dfile << setw(2) << hex << byte << dec;
           }
        dfile << endl;
       }
    dfile << endl;
    // float format
    bs.reset();
    for(uint32 i = 0; i < h03; i++) {
        dfile << "0x" << setw(2) << hex << i << dec << ": ";
        real32 p01 = bs.LE_read_real32();
        real32 p02 = bs.LE_read_real32();
        real32 p03 = bs.LE_read_real32();
        real32 p04 = bs.LE_read_real32();
        dfile << "<" << p01 << "," << p02 << "," << p03 << "," << p04 << "> ";
        real32 p05 = bs.LE_read_real32();
        real32 p06 = bs.LE_read_real32();
        real32 p07 = bs.LE_read_real32();
        dfile << "<" << p05 << "," << p06 << "," << p07 << "> ";
        real32 p08 = bs.LE_read_real32();
        real32 p09 = bs.LE_read_real32();
        real32 p10 = bs.LE_read_real32();
        dfile << "<" << p08 << "," << p09 << "," << p10 << "> ";
        uint32 p11 = bs.LE_read_uint32();
        dfile << "<" << setw(2) << "0x" << hex << p11 << dec << "> "; // offset to name
        uint16 p12[8];
        for(uint32 j = 0; j < 8; j++) p12[j] = bs.LE_read_uint08();
        uint32 p13 = bs.LE_read_uint32();
        dfile << "<" << setw(2) << "0x" << hex << p13 << dec << "> ";
        uint16 p14[8];
        for(uint32 j = 0; j < 8; j++) p14[j] = bs.LE_read_uint08();
        dfile << endl;
       }
    dfile << endl;
   }
 if(debug) {
    dfile << "------------------" << endl;
    dfile << " SKELETON DATA #2 " << endl;
    dfile << "------------------" << endl;
    dfile << endl;
    dfile << setfill('0');
    binary_stream bs(bonedata2, bonedatasize);
    for(uint32 i = 0; i < h03; i++) {
        for(uint32 j = 0x00; j < 0x40; j++) {
            uint16 byte = bs.LE_read_uint08();
            dfile << setw(2) << hex << byte << dec;
           }
        dfile << endl;
       }
    dfile << endl;
   }
 if(debug) {
    dfile << "------------------" << endl;
    dfile << " SKELETON DATA #3 " << endl;
    dfile << "------------------" << endl;
    dfile << endl;
    dfile << setfill('0');
    binary_stream bs(bonedata3, bonedatasize);
    for(uint32 i = 0; i < h03; i++) {
        for(uint32 j = 0x00; j < 0x40; j++) {
            uint16 byte = bs.LE_read_uint08();
            dfile << setw(2) << hex << byte << dec;
           }
        dfile << endl;
       }
    dfile << endl;
   }

 // read unknown table
 // table consists of h07 * 0x70-byte entries
 boost::shared_array<char> unktable1;
 uint32 unktable1_size = h07 * 0x70;
 if(unktable1_size)
   {
    // read unknown table
    unktable1.reset(new char[unktable1_size]);
    ifile.read(unktable1.get(), unktable1_size);
    if(ifile.fail()) return error("Read failure.");

    // debug unknown table
    if(debug) {
       dfile << "------------------" << endl;
       dfile << " UNKNOWN TABLE #1 " << endl;
       dfile << "------------------" << endl;
       dfile << endl;
       binary_stream bs(unktable1, unktable1_size);
       for(uint32 i = 0; i < h07; i++) {
           dfile << "item[0x" << hex << setw(2) << i << dec << "] = (";
           for(uint32 j = 0x00; j < 0x70; j++) {
               uint16 byte = bs.LE_read_uint08();
               dfile << setw(2) << hex << byte << dec << " ";
              }
           dfile << endl;
          }
       dfile << endl;
      }
   }

 // read unknown array of floats
 // if h05 is non-zero, there are h05 floats at this location
 boost::shared_array<char> unktable2;
 if(h05) {
    uint32 tablesize = h05 * 0x04;
    unktable2.reset(new char[tablesize]);
    ifile.read(unktable2.get(), tablesize);
    if(ifile.fail()) return error("Read failure.");
   }

 // debug unknown table
 if(debug) {
    dfile << "------------------" << endl;
    dfile << " UNKNOWN TABLE #2 " << endl;
    dfile << "------------------" << endl;
    dfile << endl;
    // TODO
   }

 // read surface information
 // each entry is 0x34 bytes
 boost::shared_array<char> surface_data;
 uint32 n_surfaces = h08;
 uint32 surface_data_size = n_surfaces * 0x34;
 surface_data.reset(new char[surface_data_size]);
 ifile.read(surface_data.get(), surface_data_size);
 if(ifile.fail()) return error("Read failure.");

 // debug surface table
 if(debug) {
    dfile << "---------------" << endl;
    dfile << " SURFACE TABLE " << endl;
    dfile << "---------------" << endl;
    dfile << endl;
    dfile << setfill('0');
    binary_stream bs(surface_data, surface_data_size);
    for(uint32 i = 0; i < n_surfaces; i++) {
        dfile << "s[0x" << hex << setw(2) << i << dec << "] = (";
        for(uint32 j = 0x00; j < 0x34; j++) {
            uint16 byte = bs.LE_read_uint08();
            dfile << setw(2) << hex << byte << dec << " ";
           }
        dfile << endl;
       }
    dfile << endl;
   }

 // save vertex buffer base offset
 uint32 vb_base_offset = (uint32)ifile.tellg();
 if(ifile.fail()) return error("Tell failure.");

 // read vertex buffer data
 boost::shared_array<char> vbdata;
 uint32 vbsize = h09;
 vbdata.reset(new char[vbsize]);
 ifile.read(vbdata.get(), vbsize);
 if(ifile.fail()) return error("Read failure.");

 // read index buffer data
 boost::shared_array<char> ibdata;
 uint32 ibsize = h10;
 ibdata.reset(new char[ibsize]);
 ifile.read(ibdata.get(), ibsize);
 if(ifile.fail()) return error("Read failure.");

 // read material data
 // 0x08B4 each item is 0x48 (72) bytes
 boost::shared_array<char> mbdata;
 uint32 mbsize = h11;
 mbdata.reset(new char[mbsize]);
 ifile.read(mbdata.get(), mbsize);
 if(ifile.fail()) return error("Read failure.");

 //
 // PROCESS SKELETON DATA 
 //

 // if skeleton exists
 ADVANCEDMODELCONTAINER amc;
 if(bonedatasize)
   {
    // bone data
    uint32 n_joints = h03;
    binary_stream bs1(bonedata1, bonedatasize); // parent information
    binary_stream bs2(bonedata2, bonedatasize); // matrices
    binary_stream bs3(bonedata3, bonedatasize); // 

    // initialize skeleton
    AMC_SKELETON2 skel;
    skel.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
    skel.tiplen = 0.01f;
    skel.name = "skeleton";

    // for each joint
    for(uint32 i = 0; i < n_joints; i++)
       {
        // move to position
        uint32 base = i * 0x40;
        bs1.seek(base + 16);
        if(bs1.fail()) return error("Stream seek failure.");

        // read position
        real32 x = bs1.LE_read_real32();
        real32 y = bs1.LE_read_real32();
        real32 z = bs1.LE_read_real32();

        // move stream to parent id
        bs1.seek(base + 44);
        if(bs1.fail()) return error("Stream seek failure.");

        // read parent id
        uint16 parent = bs1.LE_read_uint08();
        if(bs1.fail()) return error("Stream read failure.");

        // move to matrix
        bs2.seek(base);
        real32 m11 = bs2.LE_read_real32();
        real32 m12 = bs2.LE_read_real32();
        real32 m13 = bs2.LE_read_real32();
        bs2.LE_read_uint32(); // crap
        real32 m21 = bs2.LE_read_real32();
        real32 m22 = bs2.LE_read_real32();
        real32 m23 = bs2.LE_read_real32();
        bs2.LE_read_uint32(); // crap
        real32 m31 = bs2.LE_read_real32();
        real32 m32 = bs2.LE_read_real32();
        real32 m33 = bs2.LE_read_real32();
        bs2.LE_read_uint32(); // crap
        real32 m41 = bs2.LE_read_real32();
        real32 m42 = bs2.LE_read_real32();
        real32 m43 = bs2.LE_read_real32();
        bs2.LE_read_uint32(); // crap

        cs::math::matrix4x4<real32> m;
        m[0x0] = m11; m[0x1] = m12; m[0x2] = m13; m[0x3] = 0.0f;
        m[0x4] = m21; m[0x5] = m22; m[0x6] = m23; m[0x7] = 0.0f;
        m[0x8] = m31; m[0x9] = m32; m[0xA] = m33; m[0xB] = 0.0f;
        m[0xC] = m41; m[0xD] = m42; m[0xE] = m43; m[0xF] = 1.0f;
        m.transpose();

        // create joint name
        stringstream ss;
        ss << "jnt_" << setfill('0') << setw(3) << i;
   
        // create and insert joint
        AMC_JOINT joint;
        joint.name = ss.str();
        joint.id = i;
        joint.parent = (parent == 0xFF ? AMC_INVALID_JOINT : parent);
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
        joint.p_rel[0] = x;
        joint.p_rel[1] = y;
        joint.p_rel[2] = z;
        joint.p_rel[3] = 1.0f;
        skel.joints.push_back(joint);
       }

    // insert skeleton
    amc.skllist2.push_back(skel);
   }

 //
 // PROCESS SURFACE DATA
 //

 // surface table
 struct ROISURFACE {
  uint32 p01; // p01 + p02 = next p01
  uint32 p02; // p01 + p02 = next p01
  uint32 p03; // starting offset into vertex buffer
  uint32 p04; // size of vertex data in bytes
  uint32 p05; // type of vertex data ??? 0x40, 0x44, etc.
  uint32 p06; // starting offset into index buffer
  uint32 p07; // size of index data in bytes
  uint32 p08; // type of index data ??? always 0x01 ???
  uint32 p09; // ???
  uint32 p10; // base vertex group
  uint32 p11; // number of vertex groups (weight groups)
  uint32 p12; // ???
  uint32 p13; // ???
 };
 deque<ROISURFACE> surflist;

 // process surface table
 binary_stream ss(surface_data, surface_data_size);
 for(uint32 i = 0; i < n_surfaces; i++) {
     ROISURFACE item;
     item.p01 = ss.LE_read_uint32();
     item.p02 = ss.LE_read_uint32();
     item.p03 = ss.LE_read_uint32();
     item.p04 = ss.LE_read_uint32();
     item.p05 = ss.LE_read_uint32();
     item.p06 = ss.LE_read_uint32();
     item.p07 = ss.LE_read_uint32();
     item.p08 = ss.LE_read_uint32();
     item.p09 = ss.LE_read_uint32();
     item.p10 = ss.LE_read_uint32();
     item.p11 = ss.LE_read_uint32();
     item.p12 = ss.LE_read_uint32();
     item.p13 = ss.LE_read_uint32();
     surflist.push_back(item);
    }

 // debug vertex buffer offsets from surface table
 if(debug) {
    dfile << "-----------------------" << endl;
    dfile << " VERTEX BUFFER OFFSETS " << endl;
    dfile << "-----------------------" << endl;
    dfile << endl;
    for(uint32 i = 0; i < n_surfaces; i++) {
        uint32 offset = vb_base_offset + surflist[i].p03;
        uint32 size = surflist[i].p04;
        uint32 stride = surflist[i].p05;
        dfile << "vb[0x" << hex << setw(2) << i << dec << "] = (";
        dfile << "0x" << hex << offset << dec << ", ";
        dfile << "0x" << hex << size << dec << ", ";
        dfile << "0x" << hex << stride << dec;
        dfile << ") (offset, size, stride)" << endl;
       }
    dfile << endl;
   }

 // vertex and index buffer binary streams
 binary_stream vbs(vbdata, vbsize);
 binary_stream ibs(ibdata, ibsize);

 // convert ROISURFACE -> AMC data structures
 for(uint32 i = 0; i < n_surfaces; i++)
    {
     // initialize vertex buffer
     AMC_VTXBUFFER vb;
     vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL;
     vb.name = "default";
     vb.uvchan = 0;
     for(uint32 j = 0; j < 8; j++) vb.uvtype[j] = AMC_INVALID_MAP;
     vb.colors = 0;

     // compute number of vertices
     if(surflist[i].p04 % surflist[i].p05) return error("Vertex buffers must have vertex-byte alignment.");
     vb.elem = surflist[i].p04/surflist[i].p05;
     if(!vb.elem) return error("Vertex buffer contains no data.");

     // create vertex buffer
     vb.data.reset(new AMC_VERTEX[vb.elem]);

     // move to vertex buffer data
     vbs.seek(surflist[i].p03);
     if(vbs.fail()) return error("Stream seek failure.");

     // number of bytes per vertex
     uint32 vertex_bytes = surflist[i].p05;
     if(vertex_bytes == 0x00) return error("Invalid number of vertex bytes.");

     // transfer vertex buffer data
     if(vertex_bytes == 0x38)
       {
        vb.flags |= AMC_VERTEX_UV;
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;

        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = vbs.LE_read_real32(); // 0x00
            vb.data[j].vy = vbs.LE_read_real32(); // 0x04
            vb.data[j].vz = vbs.LE_read_real32(); // 0x08
            vb.data[j].nx = vbs.LE_read_real32(); // 0x0C
            vb.data[j].ny = vbs.LE_read_real32(); // 0x10
            vb.data[j].nz = vbs.LE_read_real32(); // 0x14
            real32 u2x = vbs.LE_read_real32(); // 0x18
            real32 u2y = vbs.LE_read_real32(); // 0x1C
            real32 u2z = vbs.LE_read_real32(); // 0x20
            real32 u3x = vbs.LE_read_real32(); // 0x24
            real32 u3y = vbs.LE_read_real32(); // 0x28
            real32 u3z = vbs.LE_read_real32(); // 0x2C
            vb.data[j].uv[0][0] = vbs.LE_read_real32(); // 0x30
            vb.data[j].uv[0][1] = vbs.LE_read_real32(); // 0x34
           }
       }
     else if(vertex_bytes == 0x3C)
       {
        vb.flags |= AMC_VERTEX_UV;
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.flags |= AMC_VERTEX_COLORS;
        vb.colors = 1;

        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = vbs.LE_read_real32(); // 0x00
            vb.data[j].vy = vbs.LE_read_real32(); // 0x04
            vb.data[j].vz = vbs.LE_read_real32(); // 0x08
            vb.data[j].nx = vbs.LE_read_real32(); // 0x0C
            vb.data[j].ny = vbs.LE_read_real32(); // 0x10
            vb.data[j].nz = vbs.LE_read_real32(); // 0x14
            vbs.LE_read_real32(); // 0x18 v1
            vbs.LE_read_real32(); // 0x1C v1
            vbs.LE_read_real32(); // 0x20 v1
            vbs.LE_read_real32(); // 0x24 v2
            vbs.LE_read_real32(); // 0x28 v2
            vbs.LE_read_real32(); // 0x2C v2
            vb.data[j].color[0][0] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x30
            vb.data[j].color[0][1] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x31
            vb.data[j].color[0][2] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x32
            vb.data[j].color[0][3] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x33
            vb.data[j].uv[0][0] = vbs.LE_read_real32(); // 0x34
            vb.data[j].uv[0][1] = vbs.LE_read_real32(); // 0x38
           }
       }
     else if((vertex_bytes == 0x40) && (h03 != 0x00))
       {
        cout << "  TYPE 1 VERTEX SIZE 0x40" << endl;
        vb.flags |= AMC_VERTEX_UV;
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;

        //                   vb0      vb size  vb vsize ib0      ib size  
        // 40010000 02000000 2ca23300 c0450000 40000000 90de0300 b8050000 01000000 b2000000 00000000 00000000 19000000 00000000 

        uint32 base_index = indextable[surflist[i].p10];
        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = vbs.LE_read_real32();
            vb.data[j].vy = vbs.LE_read_real32();
            vb.data[j].vz = vbs.LE_read_real32();
            vb.data[j].nx = vbs.LE_read_real32();
            vb.data[j].ny = vbs.LE_read_real32();
            vb.data[j].nz = vbs.LE_read_real32();
            vbs.LE_read_real32();
            vbs.LE_read_real32();
            vbs.LE_read_real32();
            vbs.LE_read_real32();
            vbs.LE_read_real32();
            vbs.LE_read_real32();
            vb.data[j].uv[0][0] = vbs.LE_read_real32();
            vb.data[j].uv[0][1] = vbs.LE_read_real32();
            vb.data[j].wi[0] = indextable[surflist[i].p10 + vbs.LE_read_uint08()];
            vb.data[j].wi[1] = indextable[surflist[i].p10 + vbs.LE_read_uint08()];
            vb.data[j].wi[2] = indextable[surflist[i].p10 + vbs.LE_read_uint08()];
            vb.data[j].wi[3] = indextable[surflist[i].p10 + vbs.LE_read_uint08()];
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].wv[1] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].wv[2] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].wv[3] = ((real32)vbs.LE_read_uint08())/255.0f;
           }
       }
     else if((vertex_bytes == 0x40) && (h03 == 0x00))
       {
        cout << "  TYPE 2 VERTEX SIZE 0x40" << endl;
        vb.flags |= AMC_VERTEX_UV;
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.flags |= AMC_VERTEX_COLORS;
        vb.colors = 2;

        //                   vb0      vb size  vb vsize ib0      ib size  
        // 40010000 02000000 2ca23300 c0450000 40000000 90de0300 b8050000 01000000 b2000000 00000000 00000000 19000000 00000000 

        uint32 base_index = indextable[surflist[i].p10];
        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = vbs.LE_read_real32(); // 0x00
            vb.data[j].vy = vbs.LE_read_real32(); // 0x04
            vb.data[j].vz = vbs.LE_read_real32(); // 0x08
            vb.data[j].nx = vbs.LE_read_real32(); // 0x0C
            vb.data[j].ny = vbs.LE_read_real32(); // 0x10
            vb.data[j].nz = vbs.LE_read_real32(); // 0x14
            vbs.LE_read_real32(); // 0x18
            vbs.LE_read_real32(); // 0x1C
            vbs.LE_read_real32(); // 0x20
            vbs.LE_read_real32(); // 0x24
            vbs.LE_read_real32(); // 0x28
            vbs.LE_read_real32(); // 0x2C
            vb.data[j].color[0][0] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x30
            vb.data[j].color[0][1] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x31
            vb.data[j].color[0][2] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x32
            vb.data[j].color[0][3] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x33
            vb.data[j].color[1][0] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x34
            vb.data[j].color[1][1] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x35
            vb.data[j].color[1][2] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x36
            vb.data[j].color[1][3] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x37
            vb.data[j].uv[0][0] = vbs.LE_read_real32(); // 0x38
            vb.data[j].uv[0][1] = vbs.LE_read_real32(); // 0x3C
           }
       }
     else if(vertex_bytes == 0x44)
       {
        vb.flags |= AMC_VERTEX_UV;
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.flags |= AMC_VERTEX_COLORS;
        vb.colors = 1;

        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = vbs.LE_read_real32();
            vb.data[j].vy = vbs.LE_read_real32();
            vb.data[j].vz = vbs.LE_read_real32();
            vb.data[j].nx = vbs.LE_read_real32();
            vb.data[j].ny = vbs.LE_read_real32();
            vb.data[j].nz = vbs.LE_read_real32();
            real32 u2x = vbs.LE_read_real32();
            real32 u2y = vbs.LE_read_real32();
            real32 u2z = vbs.LE_read_real32();
            real32 u3x = vbs.LE_read_real32();
            real32 u3y = vbs.LE_read_real32();
            real32 u3z = vbs.LE_read_real32();
            vb.data[j].color[0][0] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[0][1] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[0][2] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[0][3] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].uv[0][0] = vbs.LE_read_real32();
            vb.data[j].uv[0][1] = vbs.LE_read_real32();
            vb.data[j].wi[0] = indextable[surflist[i].p10 + vbs.LE_read_uint08()];
            vb.data[j].wi[1] = indextable[surflist[i].p10 + vbs.LE_read_uint08()];
            vb.data[j].wi[2] = indextable[surflist[i].p10 + vbs.LE_read_uint08()];
            vb.data[j].wi[3] = indextable[surflist[i].p10 + vbs.LE_read_uint08()];
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].wv[1] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].wv[2] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].wv[3] = ((real32)vbs.LE_read_uint08())/255.0f;
           }
       }
     else if(vertex_bytes == 0x48)
       {
        vb.flags |= AMC_VERTEX_UV;
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.flags |= AMC_VERTEX_COLORS;
        vb.colors = 2;

        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = vbs.LE_read_real32();
            vb.data[j].vy = vbs.LE_read_real32();
            vb.data[j].vz = vbs.LE_read_real32();
            vb.data[j].nx = vbs.LE_read_real32();
            vb.data[j].ny = vbs.LE_read_real32();
            vb.data[j].nz = vbs.LE_read_real32();
            real32 u2x = vbs.LE_read_real32();
            real32 u2y = vbs.LE_read_real32();
            real32 u2z = vbs.LE_read_real32();
            real32 u3x = vbs.LE_read_real32();
            real32 u3y = vbs.LE_read_real32();
            real32 u3z = vbs.LE_read_real32();
            vb.data[j].color[0][0] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[0][1] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[0][2] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[0][3] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[1][0] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[1][1] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[1][2] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].color[1][3] = ((real32)vbs.LE_read_uint08())/255.0f;
            vb.data[j].uv[0][0] = vbs.LE_read_real32();
            vb.data[j].uv[0][1] = vbs.LE_read_real32();
            vb.data[j].wi[0] = indextable[surflist[i].p10 + vbs.LE_read_uint08()]; // blendindex
            vb.data[j].wi[1] = indextable[surflist[i].p10 + vbs.LE_read_uint08()]; // blendindex
            vb.data[j].wi[2] = indextable[surflist[i].p10 + vbs.LE_read_uint08()]; // blendindex
            vb.data[j].wi[3] = indextable[surflist[i].p10 + vbs.LE_read_uint08()]; // blendindex
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = ((real32)vbs.LE_read_uint08())/255.0f; // blendweight
            vb.data[j].wv[1] = ((real32)vbs.LE_read_uint08())/255.0f; // blendweight
            vb.data[j].wv[2] = ((real32)vbs.LE_read_uint08())/255.0f; // blendweight
            vb.data[j].wv[3] = ((real32)vbs.LE_read_uint08())/255.0f; // blendweight
           }
       }
     else if(vertex_bytes == 0x58)
       {
        vb.flags |= AMC_VERTEX_UV;
        vb.uvchan = 2;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;

        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = vbs.LE_read_real32(); // 0x00
            vb.data[j].vy = vbs.LE_read_real32(); // 0x04
            vb.data[j].vz = vbs.LE_read_real32(); // 0x08
            vb.data[j].nx = vbs.LE_read_real32(); // 0x0C
            vb.data[j].ny = vbs.LE_read_real32(); // 0x10
            vb.data[j].nz = vbs.LE_read_real32(); // 0x14
            vbs.LE_read_real32(); // 0x18 v1
            vbs.LE_read_real32(); // 0x1C v1
            vbs.LE_read_real32(); // 0x20 v1
            vbs.LE_read_real32(); // 0x24 v2
            vbs.LE_read_real32(); // 0x28 v2
            vbs.LE_read_real32(); // 0x2C v2
            vbs.LE_read_real32(); // 0x30 v3
            vbs.LE_read_real32(); // 0x34 v3
            vbs.LE_read_real32(); // 0x38 v3
            vbs.LE_read_real32(); // 0x3C v4
            vbs.LE_read_real32(); // 0x40 v4
            vbs.LE_read_real32(); // 0x44 v4
            vb.data[j].uv[0][0] = vbs.LE_read_real32(); // 0x48
            vb.data[j].uv[0][1] = vbs.LE_read_real32(); // 0x4C
            vb.data[j].uv[1][0] = vbs.LE_read_real32(); // 0x50
            vb.data[j].uv[1][1] = vbs.LE_read_real32(); // 0x54
           }
       }
     else if(vertex_bytes == 0x5C)
       {
        vb.flags |= AMC_VERTEX_UV;
        vb.uvchan = 2;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;
        vb.flags |= AMC_VERTEX_COLORS;
        vb.colors = 1;

        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = vbs.LE_read_real32(); // 0x00
            vb.data[j].vy = vbs.LE_read_real32(); // 0x04
            vb.data[j].vz = vbs.LE_read_real32(); // 0x08
            vb.data[j].nx = vbs.LE_read_real32(); // 0x0C
            vb.data[j].ny = vbs.LE_read_real32(); // 0x10
            vb.data[j].nz = vbs.LE_read_real32(); // 0x14
            vbs.LE_read_real32(); // 0x18 v1
            vbs.LE_read_real32(); // 0x1C v1
            vbs.LE_read_real32(); // 0x20 v1
            vbs.LE_read_real32(); // 0x24 v2
            vbs.LE_read_real32(); // 0x28 v2
            vbs.LE_read_real32(); // 0x2C v2
            vbs.LE_read_real32(); // 0x30 v3
            vbs.LE_read_real32(); // 0x34 v3
            vbs.LE_read_real32(); // 0x38 v3
            vbs.LE_read_real32(); // 0x3C v4
            vbs.LE_read_real32(); // 0x40 v4
            vbs.LE_read_real32(); // 0x44 v4
            vb.data[j].color[0][0] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x48
            vb.data[j].color[0][1] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x49
            vb.data[j].color[0][2] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x4A
            vb.data[j].color[0][3] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x4B
            vb.data[j].uv[0][0] = vbs.LE_read_real32(); // 0x4C
            vb.data[j].uv[0][1] = vbs.LE_read_real32(); // 0x50
            vb.data[j].uv[1][0] = vbs.LE_read_real32(); // 0x54
            vb.data[j].uv[1][1] = vbs.LE_read_real32(); // 0x58
           }
       }
     else if(vertex_bytes == 0x64)
       {
        vb.flags |= AMC_VERTEX_UV;
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.uvchan = 2;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;
        vb.flags |= AMC_VERTEX_COLORS;
        vb.colors = 1;

        for(uint32 j = 0; j < vb.elem; j++)
           {
            vb.data[j].vx = vbs.LE_read_real32(); // 0x00
            vb.data[j].vy = vbs.LE_read_real32(); // 0x04
            vb.data[j].vz = vbs.LE_read_real32(); // 0x08
            vb.data[j].nx = vbs.LE_read_real32(); // 0x0C
            vb.data[j].ny = vbs.LE_read_real32(); // 0x10
            vb.data[j].nz = vbs.LE_read_real32(); // 0x14
            vbs.LE_read_real32(); // 0x18 v1
            vbs.LE_read_real32(); // 0x1C v1
            vbs.LE_read_real32(); // 0x20 v1
            vbs.LE_read_real32(); // 0x24 v2
            vbs.LE_read_real32(); // 0x28 v2
            vbs.LE_read_real32(); // 0x2C v2
            vbs.LE_read_real32(); // 0x30 v3
            vbs.LE_read_real32(); // 0x34 v3
            vbs.LE_read_real32(); // 0x38 v3
            vbs.LE_read_real32(); // 0x3C v4
            vbs.LE_read_real32(); // 0x40 v4
            vbs.LE_read_real32(); // 0x44 v4
            vb.data[j].color[0][0] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x48
            vb.data[j].color[0][1] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x49
            vb.data[j].color[0][2] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x4A
            vb.data[j].color[0][3] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x4B
            vb.data[j].uv[0][0] = vbs.LE_read_real32(); // 0x4C
            vb.data[j].uv[0][1] = vbs.LE_read_real32(); // 0x50
            vb.data[j].uv[1][0] = vbs.LE_read_real32(); // 0x54
            vb.data[j].uv[1][1] = vbs.LE_read_real32(); // 0x58
            vb.data[j].wi[0] = indextable[surflist[i].p10 + vbs.LE_read_uint08()]; // 0x5C
            vb.data[j].wi[1] = indextable[surflist[i].p10 + vbs.LE_read_uint08()]; // 0x5D
            vb.data[j].wi[2] = indextable[surflist[i].p10 + vbs.LE_read_uint08()]; // 0x5E
            vb.data[j].wi[3] = indextable[surflist[i].p10 + vbs.LE_read_uint08()]; // 0x5F
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x60
            vb.data[j].wv[1] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x61
            vb.data[j].wv[2] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x62
            vb.data[j].wv[3] = ((real32)vbs.LE_read_uint08())/255.0f; // 0x63
           }
       }
     else
        return error("Unknown number of vertex bytes.");

     // insert vertex buffer
     amc.vblist.push_back(vb);

     // initialize index buffer
     AMC_IDXBUFFER ib;
     ib.format = AMC_UINT16;
     ib.type = AMC_TRIANGLES;
     ib.wind = AMC_CW;
     ib.name = "default";

     // compute number of indices
     if(surflist[i].p07 % 2) return error("Index buffers must have 16-bit alignment.");
     ib.elem = surflist[i].p07/0x02;
     if(!ib.elem) return error("Index buffer contains no data.");

     // transfer index buffer data
     ib.data.reset(new char[surflist[i].p07]);
     ibs.seek(surflist[i].p06);
     if(ibs.fail()) return error("Stream seek failure.");
     ibs.LE_read_array(reinterpret_cast<uint16*>(ib.data.get()), ib.elem); 

     // insert index buffer
     amc.iblist.push_back(ib);

     // initialize surface
     AMC_SURFACE s;
     stringstream sn;
     sn << "surface_" << setfill('0') << setw(3) << i;
     s.name = sn.str();
     AMC_REFERENCE ref;
     ref.vb_index = i;       // vbuffer index
     ref.vb_start = 0;       // vbuffer start index (use whole vertex buffer)
     ref.vb_width = vb.elem; // number of vertices
     ref.ib_index = i;       // ibuffer index
     ref.ib_start = 0;       // ibuffer start index
     ref.ib_width = ib.elem; // number of indices
     ref.jm_index = AMC_INVALID_JMAP_INDEX;
     s.refs.push_back(ref);
     s.surfmat = AMC_INVALID_SURFMAT;

     // insert surface
     amc.surfaces.push_back(s);
    }

 // transform and convert (is transform necessary?)
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 bool result1 = SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 bool result2 = SaveOBJ(pathname.c_str(), shrtname.c_str(), transformed);
 return (result1 && result2);
}

}};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Rise of Incarnates (Beta Version)");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where GxArchivedFileXXX.dat files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~? GB free space.\n");
 p3 += TEXT("3. ? GB for game + ? GB extraction.\n");

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
 bool doDAT = true;
 bool doTEX = true;
 bool doMDL = true;
 bool doTX1 = true;
 bool doMD1 = true;

 // questions
 if(doDAT) doDAT = YesNoBox("Process DAT (archive) files?");
 if(doTEX) doTEX = YesNoBox("Decompress TEX0 (archive) files?");
 if(doMDL) doMDL = YesNoBox("Decompress MDL0 (archive) files?");
 if(doTX1) doTX1 = YesNoBox("Process TEX1 (texture) files?");
 if(doMD1) doMD1 = YesNoBox("Process MDL1 (model) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process archives
 if(doDAT) {
    cout << "STAGE 1" << endl;
    cout << "Processing .DAT files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".DAT"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processDAT(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archives
 if(doTEX) {
    cout << "STAGE 2" << endl;
    cout << "Processing .TEX0 files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".TEX0"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTEX(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archives
 if(doMDL) {
    cout << "STAGE 3" << endl;
    cout << "Processing .MDL0 files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".MDL0"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMDL(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process textures
 if(doTX1) {
    cout << "STAGE 4" << endl;
    cout << "Processing .TEX1 files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".TEX1"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTX1(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process models
 if(doMD1) {
    cout << "STAGE 5" << endl;
    cout << "Processing .MDL1 files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".MDL1"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMD1(filelist[i].c_str())) return false;
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