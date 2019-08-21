#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_zlib.h"
#include "x_dds.h"
#include "x_amc.h"
#include "ps3_bioshock.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   BIOSHOCK

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool process0x010000(const string& filename)
{
 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // open file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return error("Empty file.");

 // loop compressed chunks
 uint32 index = 0;
 for(;;)
    {
     // read signature number
     uint32 h01 = BE_read_uint32(ifile);
     if(ifile.eof()) break;
     if(ifile.fail()) return error("Read failure."); 
     if(h01 == 0x00000000) break; // done
     if(h01 != 0x9E2A83C1) {
        stringstream ss;
        ss << "Invalid signature at offset 0x" << hex << ((uint32)ifile.tellg() - 4) << dec << ".";
        return error(ss);
       }

     // read unknown
     uint32 h02 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure."); 

     // read compressed size
     uint32 h03 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure."); 

     // read uncompressed size
     uint32 h04 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure."); 

     // read sizes
     deque<pair<uint32, uint32>> sizes;
     uint32 total_bytes = 0;
     while(total_bytes < h03) {
           // read compressed/uncompressed size
           uint32 p01 = BE_read_uint32(ifile);
           uint32 p02 = BE_read_uint32(ifile);
           if(ifile.fail()) return error("Read failure."); 
           total_bytes += p01;
           sizes.push_back(pair<uint32, uint32>(p01, p02));
          }

     // create folder to save data
     stringstream ss1;
     ss1 << pathname;
     ss1 << shrtname << "\\";
     CreateDirectoryA(ss1.str().c_str(), NULL);

     /// create filename
     stringstream ss2;
     ss2 << pathname;
     ss2 << shrtname << "\\";
     ss2 << setfill('0') << setw(3) << index;
     ss2 << ".out";
     
     // create file
     ofstream ofile(ss2.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // read zlib data
     for(uint32 i = 0; i < sizes.size(); i++)
        {
         // decompress and save
         if(!DecompressZLIB(ifile, sizes[i].first, ofile, 0))
            return error("Failed to decompress data.");
        }

     // test for EOF
     if(ifile.eof()) break;
     index++;
    }

 return true;
}

bool processTFC(const string& filename)
{
 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // open file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open TFC file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return error("Empty TFC file.");

 // create file
 stringstream ss;
 ss << pathname << shrtname << ".TEX";
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // loop compressed chunks
 uint32 index = 0;
 for(;;)
    {
     // read signature number
     uint32 h01 = BE_read_uint32(ifile);
     if(ifile.eof()) break;
     if(ifile.fail()) return error("Read failure."); 
     if(h01 != 0x9E2A83C1) {
        stringstream ss;
        ss << "Invalid TFC signature at offset 0x" << hex << ((uint32)ifile.tellg() - 4) << dec << ".";
        return error(ss);
       }

     // read unknown
     uint32 h02 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure 2."); 

     // read compressed size
     uint32 h03 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure 3."); 

     // read uncompressed size
     uint32 h04 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure 4."); 

     // read sizes
     deque<pair<uint32, uint32>> sizes;
     uint32 total_bytes = 0;
     while(total_bytes < h03) {
           // read compressed/uncompressed size
           uint32 p01 = BE_read_uint32(ifile);
           uint32 p02 = BE_read_uint32(ifile);
           if(ifile.fail()) return error("Read failure 5."); 
           total_bytes += p01;
           sizes.push_back(pair<uint32, uint32>(p01, p02));
          }

     // create folder to save data
     // stringstream ss1;
     // ss1 << pathname;
     // ss1 << shrtname << "\\";
     // CreateDirectoryA(ss1.str().c_str(), NULL);

     // create filename
     // stringstream ss2;
     // ss2 << pathname;
     // ss2 << shrtname << "\\";
     // ss2 << setfill('0') << setw(3) << index;
     // ss2 << ".out";
     // 
     // // create file
     // ofstream ofile(ss2.str().c_str(), ios::binary);
     // if(!ofile) return error("Failed to create output file.");

     // read zlib data
     for(uint32 i = 0; i < sizes.size(); i++)
        {
         // decompress and save
         if(!DecompressZLIB(ifile, sizes[i].first, ofile, 0))
            return error("Failed to decompress data.");
        }

     // test for EOF
     if(ifile.eof()) break;
     index++;
    }

 return true;
}

bool processXXX(const string& filename)
{
 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // open file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open XXX file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return error("Empty XXX file.");

 // read signature
 uint32 h01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h01 != 0x9E2A83C1) return error("Unexpected XXX signature.");

 // read unknown (0x004B02D7?)
 uint32 h02 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 if(h02 == 0x4B02D7)
   {
    // offset to first compressed data
    uint32 h03 = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");
   
    // read unknown
    uint32 h04 = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");
   
    // read string length
    uint32 h05 = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");
   
    // read string
    char h06[1024];
    if(h05 && (h05 < 1024)) {
       if(!read_string(ifile, &h06[0], h05, '\0'))
          return error("Read failure.");
       cout << " " << h06 << endl;
      }

    //          unc size offset   datasize
    // 0000022F 00021AAB 000002BF 00007470 
    // 00021CDA 0003FF07 0000772F 0001690F 
    // 00061BE1 0003DA20 0001E03E 00030DD3 
    // 0009F601 0003C220 0004EE11 00033623 
    // 000DB821 0003FFF2 00082434 0000FB5D 
    // 0011B813 00034462 00091F91 0000C53C 
    // 0014FC75 0003E9D8 0009E4CD 0002848B 
    // 0018E64D 0003FDCB 000C6958 00023240 
    // 001CE418 00013D7C 000E9B98 0000127A

    // skip past 0x50 bytes
    ifile.seekg(0x50, ios::cur);
    if(ifile.fail()) return error("Seek failure.");

    // read number of entries
    uint32 n_entries = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    struct XXXENTRY {
     uint32 p01;
     uint32 p02;
     uint32 p03;
     uint32 p04;
    };
    deque<XXXENTRY> elist;
    for(uint32 i = 0; i < n_entries; i++) {
        XXXENTRY item;
        item.p01 = BE_read_uint32(ifile); // base offset
        item.p02 = BE_read_uint32(ifile); // uncompressed size
        item.p03 = BE_read_uint32(ifile); // offset
        item.p04 = BE_read_uint32(ifile); // compressed size
        elist.push_back(item);
       }

     // create folder to save data
     stringstream ss;
     ss << pathname;
     ss << shrtname << "\\";
     CreateDirectoryA(ss.str().c_str(), NULL);

    // process entries
    for(uint32 i = 0; i < n_entries; i++)
       {
        // move to entry
        ifile.seekg(elist[i].p03);
        if(ifile.fail()) return error("Seek failure.");

        // read signature number
        uint32 h01 = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure."); 
        if(h01 != 0x9E2A83C1) {
           stringstream ss;
           ss << "Invalid signature at offset 0x" << hex << ((uint32)ifile.tellg() - 4) << dec << ".";
           return error(ss);
          }

        // read type (should be 0x00010000)
        uint32 h02 = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure."); 

        // read compressed size
        uint32 h03 = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure."); 

        // read uncompressed size
        uint32 h04 = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure."); 

        // read sizes
        deque<pair<uint32, uint32>> sizes;
        uint32 total_bytes = 0;
        while(total_bytes < h03) {
              // read compressed/uncompressed size
              uint32 p01 = BE_read_uint32(ifile);
              uint32 p02 = BE_read_uint32(ifile);
              if(ifile.fail()) return error("Read failure."); 
              total_bytes += p01;
              sizes.push_back(pair<uint32, uint32>(p01, p02));
             }

        // create filename
        stringstream ss;
        ss << pathname;
        ss << shrtname << "\\";
        ss << setfill('0') << setw(3) << i;
        ss << ".out";
     
        // create file
        ofstream ofile(ss.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create output file.");

        // decompress and save
        for(uint32 j = 0; j < sizes.size(); j++) {
            if(!DecompressZLIB(ifile, sizes[j].first, ofile, 0))
               return error("Failed to decompress data.");
           }
       }
   }
 else if(h02 == 0x10000)
   {
    ifile.close();
    return process0x010000(filename);
   }
 else
    return error("Unknown XXX file type.");

 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

};};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool extract(void)
{
 std::string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pname)
{
 // set pathname
 using namespace std;
 std::string pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname();

 // variables
 bool doTFC = false;
 bool doXXX = true;

 // process archive
 cout << "STAGE 1" << endl;
 if(doTFC) {
    cout << "Processing .TFC files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".TFC", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTFC(filelist[i])) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 2" << endl;
 if(doXXX) {
    cout << "Processing .XXX files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".XXX", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXXX(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

}};

