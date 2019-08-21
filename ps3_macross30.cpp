#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_zlib.h"
#include "x_segs.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_gui.h"
#include "x_amc.h"
#include "ps3_macross30.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   MACROSS30

static bool debug = true;
// static set<uint32> set30;
// static set<uint32> set31;

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

struct DATTABLE1 {
 uint32 p01; // 0 (file) or 1 (folder)
 uint32 p02; // offset into string table
 uint32 p03;
 uint32 p04;
 uint32 p05;
 uint32 p06;
};

struct DATTABLE2 {
 uint32 p01; // offset into string table (folder)
 uint32 p02; // 0x00000000
 uint32 p03; // offset to FSTS data
 uint32 p04; // size of FSTS data
 uint32 p05; // number of FSTS items (also appears in FSTS header)
};

struct FSTSITEM {
 uint32 p01; // offset into string table
 uint32 p02; // offset to data
 uint32 p03; // size of uncompressed data
 uint32 p04; // size of compressed data
};

bool processDAT(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open DAT file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // debug file
 ofstream dfile;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT(".txt");
    dfile.open(ss.str().c_str());
    if(!dfile.is_open()) return error("Failed to create debug file.");
   }

 // read header
 uint32 h01 = LE_read_uint32(ifile); // PIDX0
 uint32 h02 = LE_read_uint32(ifile); // offset to table to 0x20-byte entries
 uint32 h03 = LE_read_uint32(ifile); // number of 0x20-byte entries
 uint32 h04 = LE_read_uint32(ifile); // offset to table of 0x18-byte entries
 uint32 h05 = LE_read_uint32(ifile); // number of 0x18-byte entries
 uint32 h06 = LE_read_uint32(ifile); // unknown 0x00, 0x01, or 0x02
 uint32 h07 = LE_read_uint32(ifile); // offset to table of 0x18-byte entries
 uint32 h08 = LE_read_uint32(ifile); // number of bytes in table
 uint32 h09 = LE_read_uint32(ifile); // offset to string table
 uint32 h10 = LE_read_uint32(ifile); // length of string table
 uint32 h11 = LE_read_uint32(ifile); // 0x00
 uint32 h12 = LE_read_uint32(ifile); // 0x00

 // validate header
 if(h01 != 0x58444950) return error("Not a DAT file.");

 // move to string table
 ifile.seekg(h09);
 if(ifile.fail()) return error("Seek failure.");

 // read string table into stream
 boost::shared_array<char> strtabledata;
 if(h10) {
    strtabledata.reset(new char[h10]);
    ifile.read(strtabledata.get(), h10);
    if(ifile.fail()) return error("Read failure.");
   }
 binary_stream ststream(strtabledata, h10);

 // parse string table
 deque<string> strtable;
 if(h10) {
    binary_stream bs(strtabledata, h10);
    for(;;) {
        string temp = bs.read_string();
        if(bs.fail()) return error("Stream read failure.");
        strtable.push_back(temp);
        if(bs.at_end()) break;
       }
   }
 if(debug) {
    dfile << "-----------" << endl;
    dfile << "STRINGTABLE" << endl;
    dfile << "-----------" << endl;
    dfile << endl;
    dfile << "0x" << hex << strtable.size() << dec << " entries" << endl;
    dfile << endl;
    for(uint32 i = 0; i < strtable.size(); i++) {
        dfile << "0x" << setfill('0') << setw(3) << hex << i << dec;
        dfile << ": " << strtable[i] << endl;
       }
    dfile << endl;
   }

 // process 1st table
 deque<DATTABLE1> table1;
 deque<string> pathlist1;
 if(h04 && h05)
   {
    // move to table
    ifile.seekg(h04);
    if(ifile.fail()) return error("Seek failure.");

    // read parameters
    for(uint32 i = 0; i < h05; i++) {
        DATTABLE1 item;
        item.p01 = LE_read_uint32(ifile);
        item.p02 = LE_read_uint32(ifile);
        item.p03 = LE_read_uint32(ifile);
        item.p04 = LE_read_uint32(ifile);
        item.p05 = LE_read_uint32(ifile);
        item.p06 = LE_read_uint32(ifile);
        table1.push_back(item);
       }
    if(dfile) dfile << endl;

    // set size of pathlist data
    pathlist1.resize(h05);

    // build full paths
    for(uint32 i = 0; i < h05; i++)
       {
        // item must be a folder
        DATTABLE1 item = table1[i];
        if(item.p01 != 1) continue;

        // read folder name
        ststream.seek(item.p02);
        string path = ststream.read_string();

        // for each file
        for(uint32 j = 0; j < item.p03; j++)
           {
            // get subitem
            uint32 subindex = item.p04 + j;
            if(!(subindex < h05)) return error("Subindex out of range.");
            DATTABLE1 subitem = table1[subindex];

            // read name
            ststream.seek(subitem.p02);
            string subname = ststream.read_string();

            // modify to full pathname
            string combined = (pathlist1[i].length() ? pathlist1[i] : path);
            combined += "\\";
            combined += subname;
            pathlist1[subindex] = combined;
           }
       }

    // BEGIN DEBUG
    if(debug) {
       dfile << "----------------" << endl;
       dfile << "TABLE ENTRIES #1" << endl;
       dfile << "----------------" << endl;
       dfile << endl;
       for(uint32 i = 0; i < h05; i++) {
           DATTABLE1 item = table1[i];
           dfile << setfill('0') << hex;
           dfile << "0x" << setw(4) << i << ": ";
           dfile << "0x" << setw(8) << item.p01 << " ";
           dfile << "0x" << setw(8) << item.p02 << " ";
           dfile << "0x" << setw(8) << item.p03 << " ";
           dfile << "0x" << setw(8) << item.p04 << " ";
           dfile << "0x" << setw(8) << item.p05 << " ";
           dfile << "0x" << setw(8) << item.p06 << dec << " -- ";
           ststream.seek(item.p02);
           dfile << ststream.read_string() << " -- ";
           dfile << pathlist1[i] << endl;
          }
       dfile << endl;
      }
    // END DEBUG
   }

 // process 2nd section
 deque<DATTABLE2> table2;
 if(h07 && h08)
   {
    // move to data
    ifile.seekg(h07);
    if(ifile.fail()) return error("Seek failure.", __LINE__);

    // read data
    boost::shared_array<char> data(new char[h08]);
    ifile.read(data.get(), h08);
    if(ifile.fail()) return error("Read failure.", __LINE__);
    binary_stream bs(data, h08);

    // read number of items
    uint32 n_items = bs.LE_read_uint32();
    if(bs.fail()) return error("Stream read failure.", __LINE__);

    // read list of offsets
    boost::shared_array<uint32> offsets(new uint32[n_items]);
    bs.LE_read_array(offsets.get(), n_items);
    if(bs.fail()) return error("Stream read failure.", __LINE__);

    // read items
    for(uint32 i = 0; i < n_items; i++) {
        bs.seek(offsets[i]);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);
        DATTABLE2 item;
        item.p01 = bs.LE_read_uint32();
        item.p02 = bs.LE_read_uint32();
        item.p03 = bs.LE_read_uint32();
        item.p04 = bs.LE_read_uint32();
        item.p05 = bs.LE_read_uint32();
        table2.push_back(item);
        if(debug) {
           dfile << setfill('0') << hex;
           dfile << "0x" << setw(4) << i << ": ";
           dfile << "0x" << setw(8) << item.p01 << " ";
           dfile << "0x" << setw(8) << item.p02 << " ";
           dfile << "0x" << setw(8) << item.p03 << " ";
           dfile << "0x" << setw(8) << item.p04 << " ";
           dfile << "0x" << setw(8) << item.p05 << dec << " -- ";
           ststream.seek(item.p01);
           dfile << ststream.read_string() << endl;
          }
       }

    // BEGIN DEBUG
    if(debug) {
       dfile << "----------------" << endl;
       dfile << "TABLE ENTRIES #2" << endl;
       dfile << "----------------" << endl;
       dfile << endl;
       for(uint32 i = 0; i < n_items; i++) {
           DATTABLE2 item = table2[i];
           dfile << setfill('0') << hex;
           dfile << "0x" << setw(4) << i << ": ";
           dfile << "0x" << setw(8) << item.p01 << " ";
           dfile << "0x" << setw(8) << item.p02 << " ";
           dfile << "0x" << setw(8) << item.p03 << " ";
           dfile << "0x" << setw(8) << item.p04 << " ";
           dfile << "0x" << setw(8) << item.p05 << dec << " -- ";
           ststream.seek(item.p01);
           dfile << ststream.read_string() << endl;
          }
       dfile << endl;
      }
    // END DEBUG
   }

 //
 // PHASE #1
 // PROCESS TABLE #1
 //

 // string table stream
 for(size_t i = 0; i < table1.size(); i++)
    {
     // file
     DATTABLE1 item = table1[i];
     if(item.p01 == 0)
       {
        // p02 = filename
        ststream.seek(item.p02);
        string filename = ststream.read_string();

        // p03 = 0
        if(item.p03 != 0) return error("This is not a file!");

        // p04 = offset
        uint32 offset = item.p04;
        if(offset == 0) return error("File offset cannot be zero.");

        // p05 = uncompressed size
        uint32 size_uncompressed = item.p05;
        if(size_uncompressed == 0) continue; // nothing to do

        // p06 = compressed size
        uint32 size_compressed = item.p06;
        bool compressed = (size_compressed > 0);

        // create filename
        STDSTRING ofname = pathname;
        if(pathlist1[i].length()) ofname += Unicode08ToUnicode16(pathlist1[i].c_str()).get();
        else ofname += Unicode08ToUnicode16(filename.c_str()).get();

        // create savepath
        STDSTRING savepath = GetPathnameFromFilename(ofname.c_str()).get();
        if(!savepath.length()) return error("Invalid save path.");

        // create directories recursively
        int result = SHCreateDirectoryEx(NULL, savepath.c_str(), NULL);
        if(result == ERROR_SUCCESS) ;
        else if(result == ERROR_FILE_EXISTS) ;
        else if(result == ERROR_ALREADY_EXISTS) ;
        else if(result == ERROR_BAD_PATHNAME) return error("SHCreateDirectoryExA:ERROR_BAD_PATHNAME");
        else if(result == ERROR_FILENAME_EXCED_RANGE) return error("SHCreateDirectoryExA:ERROR_FILENAME_EXCED_RANGE");
        else if(result == ERROR_PATH_NOT_FOUND) return error("SHCreateDirectoryExA:ERROR_PATH_NOT_FOUND");
        else if(result == ERROR_CANCELLED) return error("SHCreateDirectoryExA:ERROR_CANCELLED");
        else return error("Unknown SHCreateDirectoryExA error.");

        // save compressed
        if(compressed)
          {
           // create output file
           wcout << "Saving " << ofname.c_str() << endl;
           ofstream ofile(ofname.c_str(), ios::binary);
           if(!ofile) return error("Failed to create output file.");
           
           // read first two bytes
           ifile.seekg(offset);
           if(ifile.fail()) return error("Seek failure.");
           uint08 b1 = LE_read_uint08(ifile);
           uint08 b2 = LE_read_uint08(ifile);
           
           // read first four bytes
           ifile.seekg(offset);
           if(ifile.fail()) return error("Seek failure.");
           uint32 test = LE_read_uint32(ifile);
           
           // decompress
           if(isZLIB(b1, b2)) {
              ZLIBINFO2 info;
              info.deflatedBytes = size_compressed;
              info.inflatedBytes = size_uncompressed;
              info.offset = offset;
              deque<ZLIBINFO2> chunkdata;
              chunkdata.push_back(info);
              if(!InflateZLIB(ifile, chunkdata, 0, ofile))
                 return false;
             }
           // SEGS
           else if(test == 0x73676573) {
              if(!extractSEGS(ifile, (uint64)offset, ofile))
                 return false;
             }
           else {
              cout << "file #0x" << hex << i << dec << endl;
              cout << "offset #0x" << hex << offset << dec << endl;
              return error("Unknown format.");
             }
          }
        // save uncompressed
        else
          {
           // seek data
           wcout << "Saving " << ofname.c_str() << endl;
           ifile.seekg(offset);
           if(ifile.fail()) return error("Seek failure.");
           
           // read data
           boost::shared_array<char> data;
           if(size_uncompressed) {
              data.reset(new char[size_uncompressed]);
              ifile.read(data.get(), size_uncompressed);
              if(ifile.fail()) return error("Read failure.");
             }
           
           // save data
           ofstream ofile(ofname.c_str(), ios::binary);
           if(!ofile) return error("Failed to create output file.");
           if(size_uncompressed) {
              ofile.write(data.get(), size_uncompressed);
              if(ofile.fail()) return error("Write failure.");
             }
          }
       }
    }

 //
 // PHASE #1
 // PROCESS TABLE #1
 //

 // create savepath
 STDSTRING savepath = pathname;
 savepath += TEXT("FSTS");
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // loop files
 for(size_t i = 0; i < table2.size(); i++)
    {
     // p01 = filename
     DATTABLE2 item = table2[i];
     ststream.seek(item.p01);
     string filename = ststream.read_string();

     // p02 = 0
     if(item.p02 != 0) return error("Expecting 0x00000000.");

     // p03 = offset
     uint32 offset = item.p03;
     if(offset == 0) return error("File offset cannot be zero.");

     // p04 = size
     uint32 size = item.p04;
     if(offset == 0) return error("File size cannot be zero.");

     // create output filename
     STDSTRING ofname = savepath;
     ofname += Unicode08ToUnicode16(filename.c_str()).get();
     ofname += TEXT(".fsts");

     // seek data
     wcout << "Saving " << ofname.c_str() << endl;
     ifile.seekg(offset);
     if(ifile.fail()) return error("Seek failure.");
     
     // read data
     boost::shared_array<char> data;
     if(size) {
        data.reset(new char[size]);
        ifile.read(data.get(), size);
        if(ifile.fail()) return error("Read failure.");
       }
     
     // save data
     ofstream ofile(ofname.c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");
     if(size) {
        ofile.write(data.get(), size);
        if(ofile.fail()) return error("Write failure.");
       }
    }

 return true;
}

bool processFST(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open DAT file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = LE_read_uint32(ifile); // magic
 uint32 h02 = LE_read_uint32(ifile); // number of items
 uint32 h03 = LE_read_uint32(ifile); // offset to items
 uint32 h04 = LE_read_uint32(ifile); // offset to string table
 uint32 h05 = LE_read_uint32(ifile); // size of string table
 uint32 h06 = LE_read_uint32(ifile); // 0x00000000
 uint32 h07 = LE_read_uint32(ifile); // 0x00000000
 uint32 h08 = LE_read_uint32(ifile); // 0x00000000

 // validate header
 if(h01 != 0x53545346) return error("Not an FSTS file.");
 if(h02 == 0x00) return true; // nothing to do

 // move to items
 ifile.seekg(h03);
 if(ifile.fail()) return error("Seek failure.");

 // read items
 std::deque<FSTSITEM> itemlist;
 for(uint32 i = 0; i < h02; i++) {
     FSTSITEM item;
     item.p01 = LE_read_uint32(ifile);
     item.p02 = LE_read_uint32(ifile);
     item.p03 = LE_read_uint32(ifile);
     item.p04 = LE_read_uint32(ifile);
     itemlist.push_back(item);
    }

 // move to string table
 ifile.seekg(h04);
 if(ifile.fail()) return error("Seek failure.");

 // read string table into binary stream
 boost::shared_array<char> stringtable;
 if(h05) {
    stringtable.reset(new char[h05]);
    ifile.read(stringtable.get(), h05);
    if(ifile.fail()) return error("Read failure.");
   }

 // process items
 binary_stream bs(stringtable, h05); 
 for(size_t i = 0; i < itemlist.size(); i++)
    {
     // p01 = filename
     bs.seek(itemlist[i].p01);
     string filename = bs.read_string();

     // replace '/' with '\' as mixing gives SHCreateDirectoryEx trouble
     for(uint32 j = 0; j < filename.length(); j++)
         if(filename[j] == '/') filename[j] = '\\';

     // p02 = offset
     uint32 offset = itemlist[i].p02;
     if(offset == 0) return error("File offset cannot be zero.");

     // p03 = uncompressed size
     uint32 size_uncompressed = itemlist[i].p03;
     if(size_uncompressed == 0) continue; // nothing to do

     // p04 = compressed size
     uint32 size_compressed = itemlist[i].p04;
     bool compressed = (size_compressed > 0);

     // create filename
     STDSTRING ofname = pathname;
     ofname += Unicode08ToUnicode16(filename.c_str()).get();

     // create savepath
     STDSTRING savepath = GetPathnameFromFilename(ofname.c_str()).get();
     if(!savepath.length()) return error("Invalid save path.");

     // create directories recursively
     int result = SHCreateDirectoryEx(NULL, savepath.c_str(), NULL);
     if(result == ERROR_SUCCESS) ;
     else if(result == ERROR_FILE_EXISTS) ;
     else if(result == ERROR_ALREADY_EXISTS) ;
     else if(result == ERROR_BAD_PATHNAME) return error("SHCreateDirectoryExA:ERROR_BAD_PATHNAME");
     else if(result == ERROR_FILENAME_EXCED_RANGE) return error("SHCreateDirectoryExA:ERROR_FILENAME_EXCED_RANGE");
     else if(result == ERROR_PATH_NOT_FOUND) return error("SHCreateDirectoryExA:ERROR_PATH_NOT_FOUND");
     else if(result == ERROR_CANCELLED) return error("SHCreateDirectoryExA:ERROR_CANCELLED");
     else return error("Unknown SHCreateDirectoryExA error.");

     // save compressed
     if(compressed)
       {
        // create output file
        wcout << "Saving " << ofname.c_str() << endl;
        ofstream ofile(ofname.c_str(), ios::binary);
        if(!ofile) return error("Failed to create output file.");
        
        // read first two bytes
        ifile.seekg(offset);
        if(ifile.fail()) return error("Seek failure.");
        uint08 b1 = LE_read_uint08(ifile);
        uint08 b2 = LE_read_uint08(ifile);
        
        // read first four bytes
        ifile.seekg(offset);
        if(ifile.fail()) return error("Seek failure.");
        uint32 test = LE_read_uint32(ifile);
        
        // decompress
        if(isZLIB(b1, b2)) {
           ZLIBINFO2 info;
           info.deflatedBytes = size_compressed;
           info.inflatedBytes = size_uncompressed;
           info.offset = offset;
           deque<ZLIBINFO2> chunkdata;
           chunkdata.push_back(info);
           if(!InflateZLIB(ifile, chunkdata, 0, ofile))
              return false;
          }
        // SEGS
        else if(test == 0x73676573) {
           if(!extractSEGS(ifile, offset, ofile))
              return false;
          }
        else {
           cout << "file #0x" << hex << i << dec << endl;
           cout << "offset #0x" << hex << offset << dec << endl;
           return error("Unknown format.");
          }
       }
     // save uncompressed
     else
       {
        // seek data
        wcout << "Saving " << ofname.c_str() << endl;
        ifile.seekg(offset);
        if(ifile.fail()) return error("Seek failure.");
        
        // read data
        boost::shared_array<char> data;
        if(size_uncompressed) {
           data.reset(new char[size_uncompressed]);
           ifile.read(data.get(), size_uncompressed);
           if(ifile.fail()) return error("Read failure.");
          }
        
        // save data
        ofstream ofile(ofname.c_str(), ios::binary);
        if(!ofile) return error("Failed to create output file.");
        if(size_uncompressed) {
           ofile.write(data.get(), size_uncompressed);
           if(ofile.fail()) return error("Write failure.");
          }
       }
    }

 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processGFP(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open DAT file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create savepath
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // loop forever counting .GFP chunks
 uint32 index = 0;
 for(;;)
    {
     // save position
     uint32 position = (uint32)ifile.tellg();
     if(ifile.eof() || ifile.fail()) break;

     // read header
     uint32 h01 = LE_read_uint32(ifile); // magic
     uint32 h02 = LE_read_uint32(ifile); // version (2.00)
     if(ifile.eof() || ifile.fail()) break;

     // validate header
     if(h01 != 0x5046472E) return error("Not a GFP file.");
     if(h02 != 0x30302E32) return error("Invalid GFP version.");

     // read first entry
     uint32 p01 = LE_read_uint32(ifile); // DDS size (all images)
     uint32 p02 = LE_read_uint32(ifile); // DDS size (1st image)
     uint32 p03 = LE_read_uint32(ifile); // 0x20
     uint32 p04 = LE_read_uint32(ifile); // offset from .GFP to first DDS
     uint16 p05 = LE_read_uint16(ifile); // 0x0002 (sum these to get number of total DDS)
     uint16 p06 = LE_read_uint16(ifile); // 0x0002 (sum these to get number of total DDS)
     uint16 p07 = LE_read_uint16(ifile); // 0x0000 (sum these to get number of total DDS)
     uint16 p08 = LE_read_uint16(ifile); // 0x0001 (sum these to get number of total DDS)

     // compute number of files
     uint32 n_files = p04 / 0x18;
     if(n_files != (p05 + p06 + p07 + p08)) return error("Incorrect number of files.");

     // append 1st file information
     typedef std::pair<uint32,uint32> pair_t;
     std::deque<pair_t> filelist;
     filelist.push_back(pair_t(position + p04, p02));

     // append other file information
     for(uint32 i = 0; i < n_files - 1; i++) {
         uint32 p01 = LE_read_uint32(ifile); // offset from .GFP to DDS
         uint32 p02 = LE_read_uint32(ifile); // DDS size
         uint16 p03 = LE_read_uint16(ifile); // unknown
         uint16 p04 = LE_read_uint16(ifile); // unknown
         uint16 p05 = LE_read_uint16(ifile); // unknown
         uint16 p06 = LE_read_uint16(ifile); // unknown
         uint16 p07 = LE_read_uint16(ifile); // unknown
         uint16 p08 = LE_read_uint16(ifile); // unknown
         uint16 p09 = LE_read_uint16(ifile); // unknown
         uint16 p10 = LE_read_uint16(ifile); // unknown
         filelist.push_back(pair_t(position + p01, p02));
        }

     // read DDS files
     for(uint32 i = 0; i < n_files; i++)
        {
         // move to offset
         uint32 offset = filelist[i].first;
         ifile.seekg(offset);
         if(ifile.fail()) return error("Seek failed.");

         // read data
         uint32 size = filelist[i].second;
         boost::shared_array<char> data(new char[size]);
         ifile.read(data.get(), size);
         if(ifile.fail()) return error("Read failed.");

         // create filename
         STDSTRINGSTREAM ss;
         ss << savepath;
         ss << setfill(TEXT('0')) << setw(2) << index;
         ss << TEXT("_");
         ss << setfill(TEXT('0')) << setw(2) << i;
         ss << TEXT(".dds");

         // create output file
         ofstream ofile(ss.str().c_str(), ios::binary);
         if(!ofile) return error("Failed to create output file.");

         // save data
         ofile.write(data.get(), size);
         if(ofile.fail()) return error("Write failure.");
        }

     // move to next .GFP section
     ifile.seekg((position + p01), ios::beg);
     if(ifile.eof() || ifile.fail()) break;

     // increment count
     index++;
    }

 // success
 return true;
}

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

class MDLProcess {
 private :
  STDSTRING filename;
  STDSTRING pathname;
  STDSTRING shrtname;
 private :
  std::ifstream ifile;
  std::ofstream ofile;
  std::ofstream dfile;
 private :
  binary_stream bs;
  ADVANCEDMODELCONTAINER amc;
 private :
  bool processHLDM(void);
  bool process_LDM(void);
  bool processHRTM(void);
  bool process_XET(void);
  bool processHARF(void);
  bool processMARF(void);
  bool processLKSE(void);
  bool process2MDB(void);
  bool process2HMG(void);
  bool process_2MG(uint32 index);
  bool processTPMG(void);
  bool processHMTA(void);
  bool processDMBS(void);
  bool processADOL(void);
  bool processDONE(void);
 public :
  bool extract(LPCTSTR filename)
  {
   // open file
   ifile.open(filename, ios::binary);
   if(!ifile) return error("Failed to open file.");

   // determine filesize
   uint32 filesize = 0x00;
   ifile.seekg(0, ios::end);
   filesize = (uint32)ifile.tellg();
   ifile.seekg(0, ios::beg);
   if(filesize < 0x10) return true; // nothing to do
  
   // filename properties
   this->filename = filename;
   pathname = GetPathnameFromFilename(filename).get();
   shrtname = GetShortFilenameWithoutExtension(filename).get();

   // open debug file
   if(debug) {
      STDSTRING dfname = pathname;
      dfname += shrtname;
      dfname += TEXT(".txt");
      dfile.open(dfname.c_str());
      if(!dfile) return error("Failed to create debug file.");
     }

   // read entire file into memory
   boost::shared_array<char> data(new char[filesize]);
   ifile.read(data.get(), filesize);
   if(ifile.fail()) return error("Read failure.");

   // process chunks in following order
   bs = binary_stream(data, filesize);
   if(!processHLDM()) return false; // required
   if(!process_LDM()) return false; // required
   if(!processHRTM()) return false; // required
   if(!process_XET()) return false; // required
   if(!processHARF()) return false; // required
   if(!processLKSE()) return false; // required
   if(!process2MDB()) return false; // required
   if(!process2HMG()) return false; // required
   if(!processHMTA()) return false; // required
   if(!processDMBS()) return false; // optional
   if(!processADOL()) return false; // required
   if(!processDONE()) return false; // required

   // save model
   SaveAMC(pathname.c_str(), shrtname.c_str(), amc);
   SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);

   // success
   return true;
  }
 public :
  MDLProcess() {}
 ~MDLProcess() {}
};

bool MDLProcess::processHLDM(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // HLDM
 uint32 h02 = bs.BE_read_uint32(); // unknown
 uint32 h03 = bs.BE_read_uint32(); // unknown
 uint32 h04 = bs.BE_read_uint32(); // unknown

 // validate header
 if(h01 != 0x484C444D) return error("Invalid HLDM.");
 if(h02 != 0x08) return error("HLDM h02 expecting 0x08.");
 if(h03 != 0x01) return error("HLDM h03 expecting 0x01.");
 if(h04 != 0x08) return error("HLDM h04 expecting 0x08.");

 // success
 return true;
}

bool MDLProcess::process_LDM(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // _LDM
 uint32 h02 = bs.BE_read_uint32(); // 0x00000001

 // validate header
 if(h01 != 0x5F4C444D) return error("Invalid _LDM.");
 if(h02 != 0x01) return error("_LDM h02 expecting 0x01.");

 return true;
}

bool MDLProcess::processHRTM(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // HRTM
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // 0x01
 uint32 h04 = bs.BE_read_uint32(); // 0x04

 // validate header
 if(h01 != 0x4852544D) return error("Invalid HRTM.");
 if(h02 == 0x00) return error("Invalid HRTM chunk.");
 if(h03 != 0x01) return error("HRTM h03 expecting 0x01.");
 if(h04 != 0x04) return error("HRTM h04 expecting 0x04.");

 // next
 // uint32 n - number of subchunks
 // data - n LRTM subchunks
 // LRTM appears to be shading and lighting information

 // skip past data for now
 bs.move(h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::process_XET(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // _XET
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // number of textures
 uint32 h04 = bs.BE_read_uint32(); // size of texture item (0x104)

 // validate header
 if(h01 != 0x20584554) return error("Invalid _XET.");
 if(h04 == 0x00) return error("_XET h04 > 0."); 

 // save position
 auto position = bs.tell();
 if(bs.fail()) return error("Stream tell failure.");

 // allocate text buffer
 boost::shared_array<char> buffer(new char[h04]);

 // for each texture
 for(uint32 i = 0; i < h03; i++) {
     // read string
     bs.read(buffer.get(), h04);
     if(bs.fail()) return error("Stream read failure.");
     // save string
     AMC_IMAGEFILE item;
     item.filename = buffer.get();
     amc.iflist.push_back(item);
    }

 // BEGIN DEBUG
 if(debug) {
    dfile << "----------" << endl;
    dfile << " TEXTURES " << endl;
    dfile << "----------" << endl;
    dfile << endl;
    for(uint32 i = 0; i < amc.iflist.size(); i++) {
        dfile << "[0x" << setfill('0') << setw(2) << i << "]: ";
        dfile << amc.iflist[i].filename.c_str() << endl;
       }
    dfile << endl;
   }
 // END DEBUG

 // skip past data
 bs.seek(position + h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::processHARF(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // HARF
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // unknown 0x01
 uint32 h04 = bs.BE_read_uint32(); // unknown 0x04
 if(bs.fail()) return error("Stream read failure.");

 // validate header
 if(h01 != 0x48415246) return error("Invalid HARF.");
 if(h03 != 0x01) return error("HARF h03 expecting 0x01.");
 if(h04 != 0x04) return error("HARF h04 expecting 0x04.");

 // save position
 auto position = bs.tell();
 if(bs.fail()) return error("Stream tell failure.");

 // read number of MARFS
 uint32 n_MARF = bs.BE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");

 // read MARF chunks
 for(uint32 i = 0; i < n_MARF; i++)
     if(!processMARF()) return false;
 
 // skip past data
 bs.seek(position + h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::processMARF(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // MARF
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // unknown 0x01
 uint32 h04 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 if(bs.fail()) return error("Stream read failure.");

 // validate header
 if(h01 != 0x4D415246) return error("Invalid MARF.");
 if(h02 != 0xF8) return error("MARF expecting h02 == 0xF8.");
 if(h03 != 0x01) return error("MARF h03 expecting 0x01.");
 if(h02 != h04) return error("MARF expecting h02 == h04.");

 // save position
 auto position = bs.tell();
 if(bs.fail()) return error("Stream tell failure.");

 // struct MARFDATA {
 //  uint32 p01;     // unknown
 //  char p02[0x24]; // name
 //  uint32 p03;     // unknown
 //  uint32 p04;     // unknown
 //  uint32 p05;     // always 0xFFFFFFFF
 // };

 // skip past data
 bs.seek(position + h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::processLKSE(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // LKSE
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // unknown 0x01
 uint32 h04 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header

 // validate header
 if(h01 != 0x4C4B5345) return error("Invalid LKSE.");
 if(h03 != 0x01) return error("LKSE h03 expecting 0x01.");
 if(h02 != h04) return error("LKSE h02 is not same as h04.");

 // save position
 auto position = bs.tell();
 if(bs.fail()) return error("Stream tell failure.");

 // read ES00 header (0x40 bytes)
 uint32 p01 = bs.BE_read_uint32(); // ES00
 uint32 p02 = bs.BE_read_uint32(); // offset from ES00 to past index data
 uint32 p03 = bs.BE_read_uint32(); // offset from ES00 to end
 uint16 p04 = bs.BE_read_uint16(); // number of matrices, indices and 4-byte garbage
 uint16 p05 = bs.BE_read_uint16(); // unknown
 uint16 p06 = bs.BE_read_uint16(); // number of 0x10-byte pairs
 uint16 p07 = bs.BE_read_uint16(); // unknown
 uint32 p08 = bs.BE_read_uint32(); // unknown
 uint32 p09 = bs.BE_read_uint32(); // unknown
 uint32 p10 = bs.BE_read_uint32(); // unknown
 uint32 p11 = bs.BE_read_uint32(); // unknown
 uint32 p12 = bs.BE_read_uint32(); // unknown
 uint32 p13 = bs.BE_read_uint32(); // unknown
 uint32 p14 = bs.BE_read_uint32(); // 0x00000000

 // read ES000 0x10-byte indices
 boost::shared_array<boost::shared_array<uint16>> part1;
 if(p06) {
    part1.reset(new boost::shared_array<uint16>[p06]);
    for(uint32 i = 0; i < p06; i++) {
        part1[i].reset(new uint16[8]);
        bs.BE_read_array(part1[i].get(), 8);
        if(bs.fail()) return error("Stream read failure.");
       }
   }

 // read ES000 0x30-byte floats
 boost::shared_array<boost::shared_array<real32>> part2;
 if(p04) {
    part2.reset(new boost::shared_array<real32>[p04]);
    for(uint32 i = 0; i < p04; i++) {
        part2[i].reset(new real32[12]);
        bs.BE_read_array(part2[i].get(), 12);
        if(bs.fail()) return error("Stream read failure.");
       }
   }

 // read ES000 0x02-byte indices
 boost::shared_array<uint16> part3;
 if(p04) {
    part3.reset(new uint16[p04]);
    for(uint32 i = 0; i < p04; i++) {
        part3[i] = bs.BE_read_uint16();
        if(bs.fail()) return error("Stream read failure.");
       }
   }

 // move to part 4
 bs.seek(position + p02);
 if(bs.fail()) return error("Stream seek failure.");

 // read ES000 0x04-byte data (floats?)
 boost::shared_array<uint32> part4;
 if(p04) {
    part4.reset(new uint32[p04]);
    for(uint32 i = 0; i < p04; i++) {
        part4[i] = bs.BE_read_uint32();
        if(bs.fail()) return error("Stream read failure.");
       }
   }

 // read ES000 part 5
 boost::shared_array<uint32> part5A;
 boost::shared_array<uint08> part5B;
 if(p05) {
    part5A.reset(new uint32[p05 * 2]);
    part5B.reset(new uint08[p05]);
    for(uint32 i = 0; i < p05; i++) {
        part5A[2*i + 0] = bs.BE_read_uint32();
        part5A[2*i + 1] = bs.BE_read_uint32();
       }
    for(uint32 i = 0; i < p05; i++) {
        part5B[i] = bs.BE_read_uint08();
       }
   }

 // BEGIN DEBUG
 if(debug) {
    dfile << "-----------" << endl;
    dfile << " LKSE:ES00 " << endl;
    dfile << "-----------" << endl;
    dfile << endl;

    dfile << "p01 = 0x" << setfill('0') << hex << setw(8) << p01 << dec << endl;
    dfile << "p02 = 0x" << setfill('0') << hex << setw(8) << p02 << dec << endl;
    dfile << "p03 = 0x" << setfill('0') << hex << setw(8) << p03 << dec << endl;
    dfile << "p04 = 0x" << setfill('0') << hex << setw(4) << p04 << dec << endl;
    dfile << "p05 = 0x" << setfill('0') << hex << setw(4) << p05 << dec << endl;
    dfile << "p06 = 0x" << setfill('0') << hex << setw(4) << p06 << dec << endl;
    dfile << "p07 = 0x" << setfill('0') << hex << setw(4) << p07 << dec << endl;
    dfile << "p08 = 0x" << setfill('0') << hex << setw(8) << p08 << dec << endl;
    dfile << "p09 = 0x" << setfill('0') << hex << setw(8) << p09 << dec << endl;
    dfile << "p10 = 0x" << setfill('0') << hex << setw(8) << p10 << dec << endl;
    dfile << "p11 = 0x" << setfill('0') << hex << setw(8) << p11 << dec << endl;
    dfile << "p12 = 0x" << setfill('0') << hex << setw(8) << p12 << dec << endl;
    dfile << "p13 = 0x" << setfill('0') << hex << setw(8) << p13 << dec << endl;
    dfile << "p14 = 0x" << setfill('0') << hex << setw(8) << p14 << dec << endl;
    dfile << endl;

    for(uint32 i = 0; i < p06; i++) {
        dfile << "[0x" << setfill('0') << setw(2) << hex << i << dec << "]: ";
        for(uint32 j = 0; j < 8; j++)
            dfile << setfill('0') << "0x" << hex << setw(4) << part1[i][j] << dec << " ";
        dfile << endl;
       }
    dfile << endl;

    for(uint32 i = 0; i < p04; i++) {
        dfile << "[0x" << setfill('0') << setw(2) << hex << i << dec << "]: ";
        for(uint32 j = 0; j < 12; j++) {
            uint32 temp = interpret_as_uint32(part2[i][j]);
            dfile << setfill('0') << "0x" << hex << setw(8) << temp << dec << " ";
           }
        dfile << endl;
       }
    dfile << endl;

    for(uint32 i = 0; i < p04; i++) {
        dfile << "[0x" << setfill('0') << setw(2) << hex << i << dec << "]: ";
        dfile << setfill('0') << "0x" << hex << setw(4) << part3[i] << dec << " ";
        dfile << endl;
       }
    dfile << endl;

    for(uint32 i = 0; i < p04; i++) {
        dfile << "[0x" << setfill('0') << setw(2) << hex << i << dec << "]: ";
        dfile << setfill('0') << "0x" << hex << setw(8) << part4[i] << dec << " ";
        dfile << endl;
       }
    dfile << endl;

    // part 5
    if(p05) {
       for(uint32 i = 0; i < p05; i++) {
           dfile << "[0x" << setfill('0') << setw(2) << hex << i << dec << "]: ";
           dfile << "0x" << hex << setw(8) << part5A[2*i] << ", 0x" << part5A[2*i + 1] << dec << " - ";
           dfile << "0x" << hex << setw(2) << (uint16)part5B[i] << dec << " ";
           dfile << endl;
          }
       dfile << endl;
      }
   }
 // END DEBUG

 // skip past data
 bs.seek(position + h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::process2MDB(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // 2MDB
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // unknown 0x00
 uint32 h04 = bs.BE_read_uint32(); // unknown 0x00

 // validate header
 if(h01 != 0x324D4442) return error("Invalid 2MDB.");
 if(h02 != 0x30) return error("2MDB h02 expecting 0x30.");
 if(h03 != 0x00) return error("2MDB h03 expecting 0x00.");
 if(h04 != 0x00) return error("2MDB h04 expecting 0x00.");

 // save position
 auto position = bs.tell();
 if(bs.fail()) return error("Stream tell failure.");

 // read floats
 real32 p1[3] = {
  bs.BE_read_real32(),
  bs.BE_read_real32(),
  bs.BE_read_real32()
 };
 real32 p2[3] = {
  bs.BE_read_real32(),
  bs.BE_read_real32(),
  bs.BE_read_real32()
 };
 real32 p3[3] = {
  bs.BE_read_real32(),
  bs.BE_read_real32(),
  bs.BE_read_real32()
 };
 real32 p4[3] = {
  bs.BE_read_real32(),
  bs.BE_read_real32(),
  bs.BE_read_real32()
 };

 // BEGIN DEBUG
 if(debug) {
    dfile << "------------" << endl;
    dfile << " 2MDB CHUNK " << endl;
    dfile << "------------" << endl;
    dfile << endl;
    dfile << "p1 = <" << p1[0] << ", " << p1[1] << ", " << p1[2] << ">" << endl;
    dfile << "p2 = <" << p2[0] << ", " << p2[1] << ", " << p2[2] << ">" << endl;
    dfile << "p3 = <" << p3[0] << ", " << p3[1] << ", " << p3[2] << ">" << endl;
    dfile << "p4 = <" << p4[0] << ", " << p4[1] << ", " << p4[2] << ">" << endl;
    dfile << endl;
   }
 // END DEBUG

 // skip past data
 bs.seek(position + h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::process2HMG(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // 2HMG
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // number of _2MG chunks
 uint32 h04 = bs.BE_read_uint32(); // unknown 0x00

 // BEGIN DEBUG
 if(debug) {
    dfile << "------------" << endl;
    dfile << " 2HMG CHUNK " << endl;
    dfile << "------------" << endl;
    dfile << endl;
    dfile << "h01 = 0x" << setfill('0') << setw(8) << hex << h01 << dec << " 2HMG" << endl;
    dfile << "h02 = 0x" << setfill('0') << setw(8) << hex << h02 << dec << " size of chunk minus 0x10-byte header" << endl;
    dfile << "h03 = 0x" << setfill('0') << setw(8) << hex << h03 << dec << " number of _2MG chunks" << endl;
    dfile << "h04 = 0x" << setfill('0') << setw(8) << hex << h04 << dec << " 0x00000000" << endl;
    dfile << endl;
   }
 // END DEBUG

 // validate header
 if(h01 != 0x32484D47) return error("Invalid 2HMG.");
 if(h04 != 0x00) return error("2HMG h04 expecting 0x00.");

 // save position
 auto position = bs.tell();
 if(bs.fail()) return error("Stream tell failure.");

 // process each mesh
 for(uint32 i = 0; i < h03; i++)
     if(!process_2MG(i)) return false;

 // skip past data
 bs.seek(position + h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::process_2MG(uint32 index)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // _2MG
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // number of TPMG chunks
 uint32 h04 = bs.BE_read_uint32(); // unknown 0x00

 // validate header
 if(h01 != 0x20324D47) return error("Invalid _2MG.");
 if(h04 != 0x00) return error("_2MG h04 expecting 0x00.");

 // BEGIN DEBUG
 if(debug) {
    dfile << "------------" << endl;
    dfile << " _2MG CHUNK " << endl;
    dfile << "------------" << endl;
    dfile << endl;
    dfile << "_2MG index = 0x" << setfill('0') << setw(8) << hex << index << dec << endl;
    dfile << "h01 = 0x" << setfill('0') << setw(8) << hex << h01 << dec << " _2MG" << endl;
    dfile << "h02 = 0x" << setfill('0') << setw(8) << hex << h02 << dec << " size of chunk minus 0x10-byte header" << endl;
    dfile << "h03 = 0x" << setfill('0') << setw(4) << hex << h03 << dec << " number of TPMG chunks" << endl;
    dfile << "h04 = 0x" << setfill('0') << setw(8) << hex << h04 << dec << " 0x00000000" << endl;
    dfile << endl;
   }
 // END DEBUG

 // save position
 auto position = bs.tell();
 if(bs.fail()) return error("Stream tell failure.");

 // read string
 char p01[0x30];
 bs.read(&p01[0], 0x28);
 if(bs.fail()) return error("Stream read failure.");

 // read unknown
 uint32 p02 = bs.BE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");
 if(p02 != h03) return error("_2MG p02 is not same as h03.");

 // read floats
 real32 p03[3] = {
  bs.BE_read_real32(),
  bs.BE_read_real32(),
  bs.BE_read_real32()
 };
 real32 p04[3] = {
  bs.BE_read_real32(),
  bs.BE_read_real32(),
  bs.BE_read_real32()
 };
 real32 p05[3] = {
  bs.BE_read_real32(),
  bs.BE_read_real32(),
  bs.BE_read_real32()
 };

 // BEGIN DEBUG
 if(debug) {
    dfile << "p01 = " << p01 << endl;
    dfile << "p02 = 0x" << setfill('0') << setw(8) << hex << p02 << dec << endl;
    dfile << "p03 = <" << p03[0] << ", " << p03[1] << ", " << p03[2] << ">" << endl;
    dfile << "p04 = <" << p04[0] << ", " << p04[1] << ", " << p04[2] << ">" << endl;
    dfile << "p05 = <" << p05[0] << ", " << p05[1] << ", " << p05[2] << ">" << endl;
    dfile << endl;
   }
 // END DEBUG

 // process each mesh
 for(uint32 i = 0; i < h03; i++)
     if(!processTPMG()) return false;

 // skip past data
 bs.seek(position + h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::processTPMG(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // TPMG
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // 0x00
 uint32 h04 = bs.BE_read_uint32(); // 0x00

 // validate header
 if(h01 != 0x54504D47) return error("Invalid TPMG.");
 if(h03 != 0x00) return error("TPMG h04 expecting 0x00.");
 if(h04 != 0x00) return error("TPMG h04 expecting 0x00.");

 // save position
 auto position = bs.tell();
 if(bs.fail()) return error("Stream tell failure.");

 // read buffer information
 struct TPMGINFO {
  uint16 p01; // unknown
  uint16 p02; // vertex buffer format
  uint16 p03; // unknown 0x0010
  uint32 p04; // number of vertices
  uint16 p05; // number of indices
  uint32 p06; // unknown 0xFFFFFFFF
  uint16 p07; // unknown 0x0000
  uint16 p08; // offset to index buffer
  uint16 p09; // index buffer size in bytes #1
  uint16 p10; // index buffer size in bytes #2
  uint16 p11; // unknown 0x0000
  uint16 p12; // offset to vertex buffer
  uint16 p13; // unknown 0x0000
  uint16 p14; // unknown 0x0000
  uint16 p15; // bytes in 1st vertex buffer (sum to get total bytes)
  uint16 p16; // bytes in 2nd vertex buffer (sum to get total bytes)
  uint16 p17; // unknown 0x0000
  uint16 p18; // unknown 0x0000
  uint16 p19; // unknown 0x0000
  uint16 p20; // unknown 0x0000
  uint16 p21; // unknown SEE: data\ms\bjh\gb_bjh.mdl
  uint16 p22; // unknown sometimes not 0x00000
  uint16 p23; // unknown sometimes not 0x00000
  uint16 p24; // unknown sometimes not 0x00000
  uint16 p25; // size in bytes of additional data section
  uint16 p26; // unknown 0x0000
  uint16 p27; // unknown 0x0000
  uint16 p28; // unknown 0x0000
  uint32 p29; // offset to additional data section
  uint32 p30; // unknown
  uint32 p31; // unknown
  uint32 p32[0xE]; // unknown all 0x00000000
 };
 TPMGINFO info;
 info.p01 = bs.BE_read_uint16();
 info.p02 = bs.BE_read_uint16();
 info.p03 = bs.BE_read_uint16();
 info.p04 = bs.BE_read_uint32();
 info.p05 = bs.BE_read_uint16();
 info.p06 = bs.BE_read_uint32();
 info.p07 = bs.BE_read_uint16();
 info.p08 = bs.BE_read_uint16();
 info.p09 = bs.BE_read_uint16();
 info.p10 = bs.BE_read_uint16();
 info.p11 = bs.BE_read_uint16();
 info.p12 = bs.BE_read_uint16();
 info.p13 = bs.BE_read_uint16();
 info.p14 = bs.BE_read_uint16();
 info.p15 = bs.BE_read_uint16();
 info.p16 = bs.BE_read_uint16();
 info.p17 = bs.BE_read_uint16();
 info.p18 = bs.BE_read_uint16();
 info.p19 = bs.BE_read_uint16();
 info.p20 = bs.BE_read_uint16();
 info.p21 = bs.BE_read_uint16();
 info.p22 = bs.BE_read_uint16();
 info.p23 = bs.BE_read_uint16();
 info.p24 = bs.BE_read_uint16();
 info.p25 = bs.BE_read_uint16();
 info.p26 = bs.BE_read_uint16();
 info.p27 = bs.BE_read_uint16();
 info.p28 = bs.BE_read_uint16();
 info.p29 = bs.BE_read_uint32();
 info.p30 = bs.BE_read_uint32();
 info.p31 = bs.BE_read_uint32();
 bs.BE_read_array(&info.p32[0], 0x0E);
 if(bs.fail()) return error("Stream read failure.");

 // compute vertex stride
 // NOTE: vertex buffer may have zero padding, so don't check if length
 // is divisible by number of vertices.
 uint32 vertex_length = info.p15 + info.p16;
 if(!vertex_length) return error("Vertex stride cannot be zero.");

 // validate info
 if(info.p03 < 0x0010 || info.p03 > 0x0011) return error("info.p03 < 0x0010 || info.p03 > 0x0011");
 if(info.p04 >= 0xFFFF) return error("p04 >= 0xFFFF");
 if(info.p06 != 0xFFFFFFFF) return error("p06 != 0xFFFFFFFF");
 if(info.p07 != 0x0000) return error("p07 != 0x0000");
 if(info.p11 != 0x0000) return error("p11 != 0x0000");
 if(info.p13 != 0x0000) return error("p13 != 0x0000");
 if(info.p14 != 0x0000) return error("p14 != 0x0000");
 if(info.p17 != 0x0000) return error("p17 != 0x0000");
 if(info.p18 != 0x0000) return error("p18 != 0x0000");
 if(info.p19 != 0x0000) return error("p19 != 0x0000");
 if(info.p20 != 0x0000) return error("p20 != 0x0000");
 if(info.p26 != 0x0000) return error("p26 != 0x0000");
 if(info.p27 != 0x0000) return error("p27 != 0x0000");
 if(info.p28 != 0x0000) return error("p28 != 0x0000");
 for(uint32 i = 0; i < 0x0E; i++) if(info.p32[i] != 0x00000) return error("p32[i] != 0x0000");

 // TEST
 if(info.p25 && info.p29)
    cout << " MODEL HAS WEIGHTS 0x" << hex << info.p25 << dec << " BYTES" << endl;

 // seek index buffer data
 bs.seek(position + info.p08);
 if(bs.fail()) return error("Stream seek failure.");

 // compute number of indices
 // uint32 n_indices = (info.p09 + info.p10)/2;
 uint32 n_indices = info.p05;
 if(!n_indices) return error("Index buffer has no indices.");

 // read index buffer
 uint32 ib_size = info.p09 + info.p10;
 boost::shared_array<char> ibuffer(new char[ib_size]);
 bs.BE_read_array(reinterpret_cast<uint16*>(ibuffer.get()), info.p05);
 if(bs.fail()) return error("Stream read failure.");

 // set vertex stride and number of vertices
 // NOTE: although there is a number in the data that is close to the number of
 // vertices, it sometimes not the same, which forces us to recompute the number
 // from the stride and the buffer length. it's a pain in the ass, but that is
 // currently the only way to do it.
 uint32 vertex_stride = 0;
 uint32 n_vertices = 0;
 uint32 extra_vertex_flags = 0;
 if(info.p02 == 0x0303) vertex_stride = 0x18;      // done
 else if(info.p02 == 0x0606) vertex_stride = 0x1C; // done
 else if(info.p02 == 0x0707) vertex_stride = 0x10; // done
 else if(info.p02 == 0x0C08) vertex_stride = 0x10; // done
 else if(info.p02 == 0x0D09) vertex_stride = 0x14; // done
 else if(info.p02 == 0x0F0B) vertex_stride = 0x14; // done
 else if(info.p02 == 0x100C) vertex_stride = 0x14; // done
 else if(info.p02 == 0x110D) vertex_stride = 0x18; // done
 else if(info.p02 == 0x120E) vertex_stride = 0x1C; // done (big maps)
 else if(info.p02 == 0x130F) vertex_stride = 0x20; // done
 else if(info.p02 == 0x1511) vertex_stride = 0x28; // done (big maps)
 else if(info.p02 == 0x1A16) vertex_stride = 0x1C; // ???? (big maps, uncertain)
 else if(info.p02 == 0x1C18) vertex_stride = 0x24; // done (but strange normals)
 else if(info.p02 == 0x1D19) vertex_stride = 0x20; // done
 else if(info.p02 == 0x1E1A) vertex_stride = 0x2C; // done (big maps)
 else if(info.p02 == 0x1F1B) vertex_stride = 0x28; // done
 else if(info.p02 == 0x201C) vertex_stride = 0x24; // done
 else if(info.p02 == 0x2521) vertex_stride = 0x1C; // done (big maps, uncertain about two UV channels)
 else if(info.p02 == 0x2D29) vertex_stride = 0x28; // done (big maps, uncertain)
 else if(info.p02 == 0x2F2B) vertex_stride = 0x20; // done
 else if(info.p02 == 0x3430) vertex_stride = 0x18; // done (big maps)
 else if(info.p02 == 0x3C38) vertex_stride = 0x2C; // done
 else if(info.p02 == 0x403C) vertex_stride = 0x28; // done
 else if(info.p02 == 0x4440) vertex_stride = 0x38; // ???? (big maps, uncertain)
 else if(info.p02 == 0x4541) vertex_stride = 0x38; // ???? (big maps, uncertain)
 else if(info.p02 == 0x524E) vertex_stride = 0x28; // ???? (big maps, uncertain)
 else return error("Unknown vertex format.");
 n_vertices = vertex_length/vertex_stride;

 // initialize vertex buffer
 AMC_VTXBUFFER vb;
 vb.elem = n_vertices;
 vb.data.reset(new AMC_VERTEX[vb.elem]);
 vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
 vb.name = "default";
 vb.uvchan = 1;
 vb.uvtype[0] = AMC_DIFFUSE_MAP;
 vb.uvtype[1] = AMC_INVALID_MAP; // important to set!
 vb.uvtype[2] = AMC_INVALID_MAP; // important to set!
 vb.uvtype[3] = AMC_INVALID_MAP; // important to set!
 vb.uvtype[4] = AMC_INVALID_MAP; // important to set!
 vb.uvtype[5] = AMC_INVALID_MAP; // important to set!
 vb.uvtype[6] = AMC_INVALID_MAP; // important to set!
 vb.uvtype[7] = AMC_INVALID_MAP; // important to set!
 vb.colors = 0;

 // special cases
 switch(info.p02) {
   // no vertex normals
   case(0x0707) :
   case(0x0F0B) : {
        vb.flags &= ~AMC_VERTEX_NORMAL;
        break;
       }
   // no UV for test models
   case(0x0C08) : {
        vb.flags &= ~AMC_VERTEX_UV;
        vb.uvchan = 0;
        break;
       }
   // no UV but colors for viewer models
   case(0x100C) : {
        vb.flags &= ~AMC_VERTEX_UV;
        vb.uvchan = 0;
        vb.colors = 1;
        break;
       }
   // UV + colors
   case(0x130F) : {
        vb.colors = 1;
        break;
       }
   // two-channel UV map
   case(0x1A16) :
   case(0x1C18) :
   case(0x1D19) :
   case(0x1F1B) :
   case(0x201C) :
   case(0x2F2B) :
   case(0x3430) : {
        vb.uvchan = 2;
        vb.uvtype[1] = AMC_LIGHT_MAP; // actually ENVIRONMENT MAP
        break;
       }
   // two-channel UV map + one-channel color
   case(0x2521) : {
        vb.colors = 1;
        vb.uvchan = 2;
        vb.uvtype[1] = AMC_LIGHT_MAP;
        break;
       }
   // three-channel UV map
   case(0x1511) :
   case(0x1E1A) :
   case(0x2D29) : {
        vb.uvchan = 3;
        vb.uvtype[1] = AMC_LIGHT_MAP;
        vb.uvtype[2] = AMC_LIGHT_MAP;
        break;
       }
   // four-channel UV map
   case(0x403C) :
   case(0x524E) :
   case(0x3C38) : {
        vb.uvchan = 4;
        vb.uvtype[1] = AMC_LIGHT_MAP; // actually ENVIRONMENT MAP
        vb.uvtype[2] = AMC_LIGHT_MAP; // actually ENVIRONMENT MAP
        vb.uvtype[3] = AMC_LIGHT_MAP; // actually ENVIRONMENT MAP
        break;
       }
   // five-channel UV map
   case(0x4440) : {
        vb.uvchan = 5;
        vb.uvtype[1] = AMC_LIGHT_MAP; // actually ENVIRONMENT MAP
        vb.uvtype[2] = AMC_LIGHT_MAP; // actually ENVIRONMENT MAP
        vb.uvtype[3] = AMC_LIGHT_MAP; // actually ENVIRONMENT MAP
        vb.uvtype[4] = AMC_LIGHT_MAP; // actually ENVIRONMENT MAP
        break;
       }
   // six-channel UV map
   case(0x4541) : {
        vb.uvchan = 6;
        vb.uvtype[1] = AMC_LIGHT_MAP;
        vb.uvtype[2] = AMC_LIGHT_MAP;
        vb.uvtype[3] = AMC_LIGHT_MAP;
        vb.uvtype[4] = AMC_LIGHT_MAP;
        vb.uvtype[5] = AMC_LIGHT_MAP;
        break;
       }
  }

 // seek vertex buffer data
 bs.seek(position + info.p12);
 if(bs.fail()) return error("Stream seek failure.");

 // process vertices
 for(uint32 i = 0; i < n_vertices; i++)
    {
     if(info.p02 == 0x0303)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x0606)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x0707)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x0C08)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
       }
     else if(info.p02 == 0x0D09)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x0F0B)
       {
        // similar to 0x707 but with added color
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        bs.BE_read_uint08(); // unknown
        bs.BE_read_uint08(); // unknown
        bs.BE_read_uint08(); // unknown
        bs.BE_read_uint08(); // unknown
       }
     else if(info.p02 == 0x100C)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        vb.data[i].color[0][0] = bs.BE_read_uint08()/255.0f; // unknown
        vb.data[i].color[0][1] = bs.BE_read_uint08()/255.0f; // unknown
        vb.data[i].color[0][2] = bs.BE_read_uint08()/255.0f; // unknown
        vb.data[i].color[0][3] = bs.BE_read_uint08()/255.0f; // unknown
       }
     else if(info.p02 == 0x110D)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
       }
     else if(info.p02 == 0x120E)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].color[0][0] = bs.BE_read_uint08()/255.0f; // color???
        vb.data[i].color[0][1] = bs.BE_read_uint08()/255.0f; // color???
        vb.data[i].color[0][2] = bs.BE_read_uint08()/255.0f; // color???
        vb.data[i].color[0][3] = bs.BE_read_uint08()/255.0f; // color???
       }
     else if(info.p02 == 0x130F)
       {
        // similar to 0x120E but with extra tangent/binormal
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].color[0][0] = bs.BE_read_uint08()/255.0f; // color???
        vb.data[i].color[0][1] = bs.BE_read_uint08()/255.0f; // color???
        vb.data[i].color[0][2] = bs.BE_read_uint08()/255.0f; // color???
        vb.data[i].color[0][3] = bs.BE_read_uint08()/255.0f; // color???
       }
     else if(info.p02 == 0x1511)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        bs.BE_read_uint08(); // unknown
        bs.BE_read_uint08(); // unknown
        bs.BE_read_uint08(); // unknown
        bs.BE_read_uint08(); // unknown
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
        vb.data[i].uv[2][0] = bs.BE_read_real16();
        vb.data[i].uv[2][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x1A16)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x1C18)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x1D19)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x1E1A)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
        vb.data[i].uv[2][0] = bs.BE_read_real16();
        vb.data[i].uv[2][1] = bs.BE_read_real16();
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
       }
     else if(info.p02 == 0x1F1B)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
       }
     else if(info.p02 == 0x201C)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        bs.BE_read_sint08(); // unknown
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x2521)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        vb.data[i].color[0][0] = bs.BE_read_uint08()/255.0f; // unknown color???
        vb.data[i].color[0][1] = bs.BE_read_uint08()/255.0f; // unknown color???
        vb.data[i].color[0][2] = bs.BE_read_uint08()/255.0f; // unknown color???
        vb.data[i].color[0][3] = bs.BE_read_uint08()/255.0f; // unknown color???
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x2D29)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
        vb.data[i].uv[2][0] = bs.BE_read_real16();
        vb.data[i].uv[2][1] = bs.BE_read_real16();
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
       }
     else if(info.p02 == 0x2F2B)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x3430)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x3C38)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
        vb.data[i].uv[2][0] = bs.BE_read_real16(); // same as channel 0
        vb.data[i].uv[2][1] = bs.BE_read_real16(); // same as channel 0
        vb.data[i].uv[3][0] = bs.BE_read_real16();
        vb.data[i].uv[3][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x403C)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
        vb.data[i].uv[2][0] = bs.BE_read_real16();
        vb.data[i].uv[2][1] = bs.BE_read_real16();
        vb.data[i].uv[3][0] = bs.BE_read_real16();
        vb.data[i].uv[3][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x4440)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
        vb.data[i].uv[2][0] = bs.BE_read_real16();
        vb.data[i].uv[2][1] = bs.BE_read_real16();
        vb.data[i].uv[3][0] = bs.BE_read_real16();
        vb.data[i].uv[3][1] = bs.BE_read_real16();
        vb.data[i].uv[4][0] = bs.BE_read_real16();
        vb.data[i].uv[4][1] = bs.BE_read_real16();
        bs.BE_read_sint08(); // unknown 0xFFFFFFFF
        bs.BE_read_sint08(); // unknown 0xFFFFFFFF
        bs.BE_read_sint08(); // unknown 0xFFFFFFFF
        bs.BE_read_sint08(); // unknown 0xFFFFFFFF
       }
     else if(info.p02 == 0x4541)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        bs.BE_read_uint32(); // tangent/binormal
        bs.BE_read_uint32(); // tangent/binormal
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
        vb.data[i].uv[2][0] = bs.BE_read_real16();
        vb.data[i].uv[2][1] = bs.BE_read_real16();
        vb.data[i].uv[3][0] = bs.BE_read_real16();
        vb.data[i].uv[3][1] = bs.BE_read_real16();
        vb.data[i].uv[4][0] = bs.BE_read_real16();
        vb.data[i].uv[4][1] = bs.BE_read_real16();
        vb.data[i].uv[5][0] = bs.BE_read_real16();
        vb.data[i].uv[5][1] = bs.BE_read_real16();
       }
     else if(info.p02 == 0x524E)
       {
        vb.data[i].vx = bs.BE_read_real32();
        vb.data[i].vy = bs.BE_read_real32();
        vb.data[i].vz = bs.BE_read_real32();
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        bs.BE_read_sint08(); // unknown color???
        cs::math::vector3D<cs::binary32> normal = ConvertNormal_S10S11S11(bs.BE_read_uint32());
        vb.data[i].nx = normal[2];
        vb.data[i].ny = normal[1];
        vb.data[i].nz = normal[0];
        vb.data[i].uv[0][0] = bs.BE_read_real16();
        vb.data[i].uv[0][1] = bs.BE_read_real16();
        vb.data[i].uv[1][0] = bs.BE_read_real16();
        vb.data[i].uv[1][1] = bs.BE_read_real16();
        vb.data[i].uv[2][0] = bs.BE_read_real16();
        vb.data[i].uv[2][1] = bs.BE_read_real16();
        vb.data[i].uv[3][0] = bs.BE_read_real16();
        vb.data[i].uv[3][1] = bs.BE_read_real16();
       }
    }

 // save index buffer
 AMC_IDXBUFFER ib;
 ib.format = AMC_UINT16;
 ib.type = AMC_TRIANGLES;
 ib.wind = AMC_CW;
 ib.name = "default";
 ib.elem = n_indices;
 ib.data = ibuffer;

 // insert vertex and index buffers
 amc.vblist.push_back(vb);
 amc.iblist.push_back(ib);

 // create surface
 AMC_SURFACE surface;
 stringstream ss;
 ss << "mesh_" << setfill('0') << setw(3) << amc.surfaces.size() << "_" << hex << setw(4) << info.p02 << dec;
 surface.name = ss.str();
 AMC_REFERENCE ref;
 ref.vb_index = amc.vblist.size() - 1;
 ref.vb_start = 0;
 ref.vb_width = n_vertices;
 ref.ib_index = amc.iblist.size() - 1;
 ref.ib_start = 0;
 ref.ib_width = n_indices;
 ref.jm_index = AMC_INVALID_JMAP_INDEX;
 surface.refs.push_back(ref);
 surface.surfmat = AMC_INVALID_SURFMAT;
 amc.surfaces.push_back(surface);

 // BEGIN DEBUG
 if(debug) {
    dfile << "------------" << endl;
    dfile << " TPMG CHUNK " << endl;
    dfile << "------------" << endl;
    dfile << endl;
    dfile << "p01 = 0x" << setfill('0') << setw(4) << hex << info.p01 << dec << endl;
    dfile << "p02 = 0x" << setfill('0') << setw(4) << hex << info.p02 << dec << endl;
    dfile << "p03 = 0x" << setfill('0') << setw(4) << hex << info.p03 << dec << endl;
    dfile << "p04 = 0x" << setfill('0') << setw(8) << hex << info.p04 << dec << endl;
    dfile << "p05 = 0x" << setfill('0') << setw(4) << hex << info.p05 << dec << endl;
    dfile << "p06 = 0x" << setfill('0') << setw(8) << hex << info.p06 << dec << endl;
    dfile << "p07 = 0x" << setfill('0') << setw(4) << hex << info.p07 << dec << endl;
    dfile << "p08 = 0x" << setfill('0') << setw(4) << hex << info.p08 << dec << endl;
    dfile << "p09 = 0x" << setfill('0') << setw(4) << hex << info.p09 << dec << endl;
    dfile << "p10 = 0x" << setfill('0') << setw(4) << hex << info.p10 << dec << endl;
    dfile << "p11 = 0x" << setfill('0') << setw(4) << hex << info.p11 << dec << endl;
    dfile << "p12 = 0x" << setfill('0') << setw(4) << hex << info.p12 << dec << " offset to vertex buffer" << endl;
    dfile << "p13 = 0x" << setfill('0') << setw(4) << hex << info.p13 << dec << endl;
    dfile << "p14 = 0x" << setfill('0') << setw(4) << hex << info.p14 << dec << endl;
    dfile << "p15 = 0x" << setfill('0') << setw(4) << hex << info.p15 << dec << endl;
    dfile << "p16 = 0x" << setfill('0') << setw(4) << hex << info.p16 << dec << endl;
    dfile << "p17 = 0x" << setfill('0') << setw(4) << hex << info.p17 << dec << endl;
    dfile << "p18 = 0x" << setfill('0') << setw(4) << hex << info.p18 << dec << endl;
    dfile << "p19 = 0x" << setfill('0') << setw(4) << hex << info.p19 << dec << endl;
    dfile << "p20 = 0x" << setfill('0') << setw(4) << hex << info.p20 << dec << endl;
    dfile << "p21 = 0x" << setfill('0') << setw(4) << hex << info.p21 << dec << endl;
    dfile << "p22 = 0x" << setfill('0') << setw(4) << hex << info.p22 << dec << endl;
    dfile << "p23 = 0x" << setfill('0') << setw(4) << hex << info.p23 << dec << endl;
    dfile << "p24 = 0x" << setfill('0') << setw(4) << hex << info.p24 << dec << endl;
    dfile << "p25 = 0x" << setfill('0') << setw(4) << hex << info.p25 << dec << endl;
    dfile << "p26 = 0x" << setfill('0') << setw(4) << hex << info.p26 << dec << endl;
    dfile << "p27 = 0x" << setfill('0') << setw(4) << hex << info.p27 << dec << endl;
    dfile << "p28 = 0x" << setfill('0') << setw(4) << hex << info.p28 << dec << endl;
    dfile << "p29 = 0x" << setfill('0') << setw(8) << hex << info.p29 << dec << endl;
    dfile << "p30 = 0x" << setfill('0') << setw(8) << hex << info.p30 << dec << endl;
    dfile << "p31 = 0x" << setfill('0') << setw(8) << hex << info.p31 << dec << endl;
    dfile << "vertex stride = 0x" << setfill('0') << setw(8) << hex << vertex_stride << dec << endl;
    dfile << "number of vertices = 0x" << setfill('0') << setw(8) << hex << n_vertices << dec << endl;
    dfile << endl;
   }
 // END DEBUG

 // skip past data
 bs.seek(position + h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::processHMTA(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // HMTA
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // number of CMTA chunks (number also appears in 2HMG chunk)
 uint32 h04 = bs.BE_read_uint32(); // unknown 0x00

 // validate header
 if(h01 != 0x484D5441) return error("Invalid HMTA.");
 if(h04 != 0x00) return error("HMTA h04 expecting 0x00.");

 // next
 // data

 // skip past data for now
 bs.move(h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::processDMBS(void)
{
 // peek header and skip if not DMBS
 uint32 p01 = bs.BE_peek_uint32();
 if(p01 != 0x444E4253) return true;

 // read header
 uint32 h01 = bs.BE_read_uint32(); // DMBS
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // unknown 0x00
 uint32 h04 = bs.BE_read_uint32(); // unknown 0x00

 // validate header
 if(h01 != 0x444E4253) return error("Invalid DMBS.");
 if(h03 != 0x00) return error("DMBS h03 expecting 0x00.");
 if(h04 != 0x00) return error("DMBS h04 expecting 0x00.");

 // skip past data for now
 bs.move(h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::processADOL(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // ADOL
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // unknown 0x00
 uint32 h04 = bs.BE_read_uint32(); // unknown 0x00

 // validate header
 if(h01 != 0x41444F4C) return error("Invalid ADOL.");
 if(h02 != 0x00) return error("ADOL h02 expecting 0x00.");
 if(h03 != 0x00) return error("ADOL h03 expecting 0x00.");
 if(h04 != 0x00) return error("ADOL h04 expecting 0x00.");

 // skip past data for now
 bs.move(h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool MDLProcess::processDONE(void)
{
 // read header
 uint32 h01 = bs.BE_read_uint32(); // DONE
 uint32 h02 = bs.BE_read_uint32(); // size of chunk minus 0x10-byte header
 uint32 h03 = bs.BE_read_uint32(); // unknown 0x00
 uint32 h04 = bs.BE_read_uint32(); // unknown 0x00

 // validate header
 if(h01 != 0x20444E45) return error("Invalid DONE.");
 if(h02 != 0x00) return error("DONE h02 expecting 0x00.");
 if(h03 != 0x00) return error("DONE h03 expecting 0x00.");
 if(h04 != 0x00) return error("DONE h04 expecting 0x00.");

 // skip past data for now
 bs.move(h02);
 if(bs.fail()) return error("Stream seek failure.");

 return true;
}

bool processMDL(LPCTSTR filename)
{
 MDLProcess obj;
 return obj.extract(filename);
}

};};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PS3] Macross 30 (Beta)");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Do not run still in beta.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. Do not run still in beta.");

 // insert title
 AddGameTitle(p1.c_str(), p2.c_str(), p3.c_str(), extract);
 return true;
}

bool extract(void)
{
 STDSTRING pathname = GetModulePathname().get();
 return extract(pathname.c_str());
}

bool extract(LPCTSTR pname)
{
 // set pathname
 using namespace std;
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // variables
 bool doDAT = true;
 bool doFST = true;
 bool doGFP = true;
 bool doMDL = true;

 if(doDAT) doDAT = YesNoBox("Process DAT (archive) files?\nClick 'No' if you have already done so.");
 if(doFST) doFST = YesNoBox("Process FST (archive) files?\nClick 'No' if you have already done so.");
 if(doGFP) doGFP = YesNoBox("Process GFP (texture) files?\nClick 'No' if you have already done so.");
 if(doMDL) doMDL = YesNoBox("Process MDL (model) files?\nClick 'No' if you have already done so.");

 // process archive
 cout << "STAGE 1" << endl;
 if(doDAT) {
    cout << "Processing .DAT files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".DAT"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processDAT(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 2" << endl;
 if(doFST) {
    cout << "Processing .FST files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".FSTS"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processFST(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 3" << endl;
 if(doGFP) {
    cout << "Processing .GFP files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".GFP"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processGFP(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 4" << endl;
 if(doMDL) {
    cout << "Processing .MDL files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".MDL"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMDL(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 return true;
}

}};

