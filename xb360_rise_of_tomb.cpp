#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "xb360_rise_of_tomb.h"
using namespace std;

#define X_SYSTEM XBOX360
#define X_GAME   RISE_OF_TOMB_RAIDER

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

struct TGRHEADER {
 uint32 p01; // TAFS (0x54414653)
 uint32 p02; // 0x04
 uint32 p03; // number of tiger files
 uint32 p04; // number of files
 uint32 p05; // 0x01
 uint64 p06; // xenon-w (0x78656E6F6E2D7700)
 uint32 p07[6]; // all 0x00
};

struct TGRFILEINFO {
 uint32 p01; // hash ID
 uint32 p02; // usually 0xFFFFFFFF
 uint32 p03; // filesize
 uint64 p04; // offset to data
 uint16 p05; // 0x01
 uint16 p06; // tiger file index (0x01 for .01_tiger)
};

struct DRMFILEINFO1 {
 uint32 p01; // 
 uint32 p02; // 
 uint32 p03; // 
 uint32 p04; // 
 uint32 p05; // 0xFFFFFFFF
};

struct DRMFILEINFO2 {
 uint32 p01; // CDRM file hash
 uint64 p02; // CDRM file offset into TIGER
 uint16 p03; // 0x01
 uint16 p04; // tiger file index (0x01 for .01_tiger)
 uint32 p05; // size of CDRM data
 uint32 p06; // flags???
};

bool processDRM(boost::shared_array<char> data, uint32 datasize, LPCTSTR filename)
{
 // create binary stream
 if(!datasize) return error("Invalid DRM data size.");
 binary_stream bs(data, datasize);

 // determine file type
 uint32 type = 0;
 uint32 test = bs.BE_peek_uint32();
 if(test == 0x16) type = 0x01;
 else if(test == 0xAC44) type = 0x02;
 else if(test == 0xBB80) type = 0x02;
 else if(test == 0x0000) type = 0x03; // 0x008da24b.drm
 else type = 0x04;

 // DRM file type #1
 if(type == 0x01)
   {
    // read header
    uint32 h01 = bs.BE_read_uint32(); // 0x16
    uint32 h02 = bs.BE_read_uint32(); // size of string table
    uint32 h03 = bs.BE_read_uint32(); // 0x00
    uint32 h04 = bs.BE_read_uint32(); // 0x00
    uint32 h05 = bs.BE_read_uint32(); // 0x00
    uint32 h06 = bs.BE_read_uint32(); // 0x00
    uint32 h07 = bs.BE_read_uint32(); // number of items
    uint32 h08 = bs.BE_read_uint32(); // 0xFFFFFFFF
    if(h01 != 0x16) return error("Invalid DRM header.");
    if(h07 == 0x00) return error("DRM file contains to file information.");
   
    // read DRMFILEINFO1 array
    vector<DRMFILEINFO1> drminfo1(h07);
    for(uint32 i = 0; i < h07; i++) {
        drminfo1[i].p01 = bs.BE_read_uint32();
        drminfo1[i].p02 = bs.BE_read_uint32();
        drminfo1[i].p03 = bs.BE_read_uint32();
        drminfo1[i].p04 = bs.BE_read_uint32();
        drminfo1[i].p05 = bs.BE_read_uint32();
       }
   
    // read string table
    boost::shared_array<char> strtable;
    if(h02) {
       strtable.reset(new char[h02]);
       bs.read(strtable.get(), h02);
      }
   
    // read DRMFILEINFO2 array
    vector<DRMFILEINFO2> drminfo2(h07);
    for(uint32 i = 0; i < h07; i++) {
        drminfo2[i].p01 = bs.BE_read_uint32();
        drminfo2[i].p02 = bs.BE_read_uint64();
        drminfo2[i].p03 = bs.BE_read_uint16();
        drminfo2[i].p04 = bs.BE_read_uint16();
        drminfo2[i].p05 = bs.BE_read_uint32();
        drminfo2[i].p06 = bs.BE_read_uint32();
       }
   }
 // DRM file type #2
 else if(type == 0x02)
   {
    // compressed
   }
 // DRM file type #3
 else if(type == 0x03)
   {
   }
 // DRM file type #4
 else {
   // simply just a single file
   // drm\0x10466ce1.drm
   // drm\0x13a6c130.drm
   // drm\0x19e5e611.drm
   // drm\0x2020e3c6.drm
   // drm\0x295161ce.drm
   // drm\0x4320f1ce.drm
   // drm\0x4f0eeab2.drm
   // drm\0x597536ae.drm
   // drm\0xb165f69c.drm
   // drm\0xb2c36492.drm
   // drm\0xc8b69ee7.drm
   // drm\0xcd6afd2f.drm
   // drm\0xce080f38.drm
   // drm\0xf2f5db61.drm
  }

 return true;
}

bool processDRM(LPCTSTR filename)
{
 // open file
 if(!filename) return error("Invalid filename.");
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read data
 uint32 datasize = filesize;
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 if(ifile.fail()) return error("Read failure."); 

 return processDRM(data, datasize, filename);
}

bool processTGR(LPCTSTR filename)
{
 // open file
 if(!filename) return error("Invalid filename.");
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 TGRHEADER header;
 header.p01 = BE_read_uint32(ifile); // TAFS (0x54414653)
 header.p02 = BE_read_uint32(ifile); // 0x04
 header.p03 = BE_read_uint32(ifile); // number of tiger files
 header.p04 = BE_read_uint32(ifile); // number of files
 header.p05 = BE_read_uint32(ifile); // 0x01
 header.p06 = BE_read_uint64(ifile); // xenon-w (0x78656E6F6E2D7700)
 BE_read_array(ifile, &header.p07[0], 6);
 if(ifile.fail()) return error("Read failure");

 // validate header
 if(header.p01 != 0x54414653) return error("Invalid TIGER file.");
 if(header.p03 == 0x00) return error("Invalid number of TIGER files.");
 if(header.p04 == 0x00) return error("There are no files in this TIGER file.");
 if(header.p06 != 0x78656E6F6E2D7700ull) return error("Only XBOX360 version of the game is supported.");

 // read items
 std::vector<TGRFILEINFO> filelist(header.p04);
 for(uint32 i = 0; i < header.p04; i++) {
     filelist[i].p01 = BE_read_uint32(ifile); // hash ID
     filelist[i].p02 = BE_read_uint32(ifile); // usually 0xFFFFFFFF
     filelist[i].p03 = BE_read_uint32(ifile); // filesize
     filelist[i].p04 = BE_read_uint64(ifile); // offset to data
     filelist[i].p05 = BE_read_uint16(ifile); // 0x01
     filelist[i].p06 = BE_read_uint16(ifile); // tiger file index (0x01 for .01_tiger)
     if(ifile.fail()) return error("Read failure");
    }

 // close ifstream
 ifile.close();
 return true;

 // ifstream for all tiger files
 vector<ifstream> ifilelist(header.p03);
 for(uint32 i = 0; i < header.p03; i++) {
     STDSTRINGSTREAM ss;
     ss << pathname << shrtname << TEXT(".");
     ss << setw(3) << setfill(TEXT('0')) << i;
     ss << TEXT("_tiger");
     ifilelist[i].open(ss.str().c_str(), ios::binary);
     if(!ifilelist[i]) return error("Failed to open .XXX_TIGER file");
    }

 // DRM path
 STDSTRINGSTREAM drmpath;
 drmpath << pathname.c_str() << TEXT("drm") << TEXT("\\");
 CreateDirectory(drmpath.str().c_str(), NULL);

 // save DRM files
 for(uint32 i = 0; i < header.p04; i++)
    {
     uint16 tiger_index = filelist[i].p06;
     ifstream& ifile = ifilelist[tiger_index];

     // seek data
     uint64 offset = filelist[i].p04;
     ifile.seekg(offset);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     uint32 datasize = filelist[i].p03;
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);
     if(ifile.fail()) return error("Read failure.");

     // output filename
     STDSTRINGSTREAM ofnss;
     ofnss << drmpath.str().c_str() << TEXT("0x");
     ofnss << setw(8) << setfill(TEXT('0')) << hex << filelist[i].p01 << dec;
     ofnss << TEXT(".drm");

     // save data
     ofstream ofile(ofnss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");
     ofile.write(data.get(), datasize);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
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
 p1 += TEXT("[XBOX360] Rise of the Tomb Raider");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where TIGER files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~? GB free space.\n");
 p3 += TEXT("3. 6 GB for game + ? GB extraction.\n");

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
 bool doTGR = false;
 bool doDRM = true;

 // questions
 if(doTGR) doTGR = YesNoBox("Process TIGER files?\nSay 'No' if you have already done this before.");
 if(doDRM) doDRM = YesNoBox("Process DRM files?\nSay 'No' if you have already done this before.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process TIGER files
 cout << "STAGE 1" << endl;
 if(doTGR) {
    cout << "Processing .TIGER files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".000_TIGER"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTGR(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process DRM files
 cout << "STAGE 2" << endl;
 if(doDRM) {
    cout << "Processing .DRM files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".DRM"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processDRM(filelist[i].c_str())) return false;
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