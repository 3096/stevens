#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_dw_xb360.h"
#include "x_dw_ps3.h"
#include "x_dw_arc.h"
#include "ps3_sengoku4.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   SENGOKU_MUSOU_4

//
// ARCHIVE FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool processBIN(LPCTSTR filename)
{
 // uint32 magic
 // uint32 number of files
 // uint32 offset scale
 // uint32 unknown
 // for(uint32 i = 0; i < number of files; i++)
 //     uint32 0x00000000;
 //     uint32 offset;
 //     uint32 datasize;
 //     uint32 datasize_decompressed; // or zero

 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read magic
 uint32 h01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h01 != 0x00102A49) return error("Invalid magic number.");

 // read number of files
 uint32 h02 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h02 == 0) return true;

 // read scale
 uint32 h03 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h03 != 0x800) return error("Incorrect scale value.");

 // read item type
 uint32 h04 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read file table
 struct LINKDATA_ITEM {
  uint32 p01;
  uint32 p02;
  uint32 p03;
  uint32 p04;
 };
 deque<LINKDATA_ITEM> filetable;
 for(size_t i = 0; i < h02; i++) {
     LINKDATA_ITEM item;
     if(h04 == 0x00) {
        item.p01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
        item.p02 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
        item.p03 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
        item.p04 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
       }
     else if(h04 == 0x01) {
        item.p01 = 0x00000000ul;
        item.p02 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
        item.p03 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
        item.p04 = 0x00000000ul;
       }
     filetable.push_back(item);
    }

 // create output directory
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     size_t position = filetable[i].p02*h03;
     size_t filesize = filetable[i].p03;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // ignore small files
     if(filesize < 0x80) continue;

     // create BIN filename
     cout << "Reading file " << (i + 1) << " of " << filetable.size() << "." << endl;
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(4) << i;
     if(filetable[i].p04) ss << TEXT(".idxzrc");
     else ss << TEXT(".idxout");

     // create BIN file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create bin file.");

     // small files
     if(filesize < 0x2000000) // 32 MB
       {
        // read data
        boost::shared_array<char> data(new char[filesize]);
        ifile.read(data.get(), filesize);
        if(ifile.fail()) return error("Read failure.");
        
        // save data
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Write failure.");
       }
     // large files
     else
       {
        // create buffer
        cout << " NOTE: This is a large file and might take a while to save." << endl;
        uint32 buffersize = 0x1000000; // 16 MB
        boost::shared_array<char> data(new char[buffersize]);

        // read and copy data
        uint32 bytes_left = filesize;
        while(bytes_left)
             {
              // specify how much to read
              uint32 datasize = buffersize;
              if(bytes_left < buffersize) datasize = bytes_left;

              // read data
              ifile.read(data.get(), datasize);
              if(ifile.fail()) return error("Read failure.");
              
              // save data
              ofile.write(data.get(), datasize);
              if(ofile.fail()) return error("Failed to write to bin file.");

              // update
              bytes_left -= datasize;
             }
       }
    }

 // success
 cout << endl;
 return true;
}

bool processIDXOUT(LPCTSTR filename)
{
 return XB360::DYNASTY_WARRIORS::DWRenameIDXOUT(filename);
}

bool processIDXZRC(LPCTSTR filename)
{
 return XB360::DYNASTY_WARRIORS::DWUnpackIDXZRC_TypeB(filename);
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
 p1 += TEXT("[PS3] Sengoku Musou 4");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where LINKDATA files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~20 GB free space.\n");
 p3 += TEXT("3. 7 GB for game + 13 GB extraction.\n");
 p3 += TEXT("4. For DLC, EDATs must be decyrpted to DATs.");

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
 bool doPHASE1 = true;
 bool doPHASE2 = true;
 bool doPHASE3 = true;
 bool doPHASE4 = true;
 bool doPHASE5 = true;

 // questions
 if(doPHASE1) doPHASE1 = YesNoBox("Process BIN files?");
 if(doPHASE2) doPHASE2 = YesNoBox("Process DAT (DLC) files?");
 if(doPHASE3) doPHASE3 = YesNoBox("Process IDXOUT and IDXZRC files?");
 if(doPHASE4) doPHASE4 = YesNoBox("Process texture (G1T) files?");
 if(doPHASE5) doPHASE5 = YesNoBox("Process model (G1M) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // PHASE #1
 if(doPHASE1) {
    cout << "Processing .BIN files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BIN"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processBIN(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // PHASE #2
 if(doPHASE2) {
    PS3::DYNASTY_WARRIORS::UnpackDLC_TypeB(pathname.c_str());
   }

 // PHASE #3
 if(doPHASE3)
   {
    deque<STDSTRING> filelist;
    uint32 loop_counter = 1;
    for(;;)
       {
        // keep track of how many files were processed
        cout << "ARCHIVE UNPACK LOOP #" << loop_counter << ": " << endl;
        uint32 processed = 0;

        // process IDXZRC files
        cout << "Processing .IDXZRC files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".idxzrc"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processIDXZRC(filelist[i].c_str())) return false;
           }
        processed += filelist.size();
        cout << endl;

        // process IDXOUT files
        cout << "Processing .IDXOUT files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".idxout"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processIDXOUT(filelist[i].c_str())) return false;
           }
        cout << endl;

        // process ARC1 files
        cout << "Processing .ARC1 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc1"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            using namespace XB360::DYNASTY_WARRIORS;
            if(!DWUnpackArc1(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
        
        // process ARC2 files
        cout << "Processing .ARC2 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc2"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            using namespace XB360::DYNASTY_WARRIORS;
            if(!DWUnpackArc2(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
        
        // process ARC3 files
        cout << "Processing .ARC3 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc3"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            using namespace XB360::DYNASTY_WARRIORS;
            if(!DWUnpackArc3(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
        
        // process ARC4 files
        cout << "Processing .ARC4 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc4"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            using namespace XB360::DYNASTY_WARRIORS;
            if(!DWUnpackArc4(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
        
        // process ARC5 files
        cout << "Processing .ARC5 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc5"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            using namespace XB360::DYNASTY_WARRIORS;
            if(!DWUnpackArc5(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
        
        // process ARC6 files
        cout << "Processing .ARC6 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc6"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            using namespace XB360::DYNASTY_WARRIORS;
            if(!DWUnpackArc6(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
        
        // finished or not finished
        if(processed == 0) break;
        cout << "Processed a total of " << processed << " files in unpack loop #" << loop_counter << "." << endl;
        loop_counter++;
       }
   }

 // texture extraction
 if(doPHASE4)
    XB360::DYNASTY_WARRIORS::UnpackTextures(pathname.c_str(), false);
 
 // model extraction
 if(doPHASE5)
    XB360::DYNASTY_WARRIORS::UnpackModels(pathname.c_str());

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