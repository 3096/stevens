#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_dw_ps3.h"
#include "ps3_kdsp.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   KAGERO_DSP

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool UnpackK300(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = BE_read_uint32(ifile); // K300
 uint32 h02 = BE_read_uint32(ifile); // 0x00000000
 uint32 h03 = BE_read_uint32(ifile); // 0x00000000
 uint32 h04 = BE_read_uint32(ifile); // number of files
 uint64 h05 = BE_read_uint64(ifile); // total filesize
 uint32 h06 = BE_read_uint32(ifile); // 0x00000000
 uint32 h07 = BE_read_uint32(ifile); // 0x800 alignment???

 // read offsets
 struct KDSPFILEINFO {
  uint64 offset; // offset to data
  uint64 size_2; // size decompressed
  uint64 size_1; // size compressed
  uint32 unk_01; // 0x00000000
  uint32 unk_02; // 0x00000001
 };
 deque<KDSPFILEINFO> fileinfo;
 for(uint32 i = 0; i < h04; i++) {
     KDSPFILEINFO info;
     info.offset = BE_read_uint64(ifile);
     info.size_2 = BE_read_uint64(ifile);
     info.size_1 = BE_read_uint64(ifile);
     info.unk_01 = BE_read_uint32(ifile);
     info.unk_02 = BE_read_uint32(ifile);
     fileinfo.push_back(info);
     if(ifile.fail()) return error("Read failure.", __LINE__);
    }

 // create savepath
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // process files
 for(uint32 i = 0; i < h04; i++)
    {
     // display message
     cout << "Extracting file " << (i + 1) << " of " << h04 << "." << endl;

     // move to offset
     ifile.seekg(fileinfo[i].offset);
     if(ifile.fail()) return error("Seek failure.", __LINE__);

     // create output file
     STDSTRINGSTREAM ss;
     ss << savepath;
     ss << setfill(TEXT('0')) << setw(4) << i << TEXT(".idxout");
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.", __LINE__);

     // keep track of bytes read
     uint64 bytes_read = 0;
     while(bytes_read < fileinfo[i].size_1)
          {
           // read code
           uint32 datasize = BE_read_uint32(ifile);
           if(ifile.fail()) return error("Read failure.", __LINE__);
           bytes_read += 0x04;

           // uncompressed
           if(datasize < 0x8000)
             {
              // read data
              boost::shared_array<char> data(new char[datasize]);
              ifile.read(data.get(), datasize);
              if(ifile.fail()) return error("Read failure.", __LINE__);

              // save data
              ofile.write(data.get(), datasize);
              if(ofile.fail()) return error("Write failure.", __LINE__);
             }
           // compressed
           else if(datasize > 0x8000)
             {
              // decompress zlib data
              datasize = datasize - 0x8000;
              if(datasize) {
                 bool status = DecompressZLIB(ifile, datasize, ofile, 0);
                 if(!status) return error("ZLIB decompression failure.", __LINE__);
                }
             }
           else {
              stringstream es;
              es << "Invalid code at offset 0x" << hex << ((uint64)ifile.tellg() - 4) << dec << ".";
              return error(es, __LINE__);
             }

           // move to next 0x10-byte alignment
           uint64 curr = (uint64)ifile.tellg();
           if(ifile.fail()) return error("Tell failure.");
           uint32 next = ((curr + 0x0F) & (~0x0F));
           ifile.seekg((next - curr), ios::cur);
           if(ifile.fail()) return error("Seek failure.");

           // increment bytes read
           bytes_read += datasize;
           bytes_read += next - curr;
          }
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
 p1 += TEXT("[PS3] Kagero: Dark Side Princess");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where archive00.lnk file is.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~4 GB free space.\n");
 p3 += TEXT("3. 2 GB for game + 2 GB extraction.\n");

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
 bool doLNK = true;
 bool doARC = true;
 bool doG1T = true;
 bool doG1M = true;

 // questions
 if(doLNK) doLNK = YesNoBox("Process ARCHIVE00.LNK file?");
 if(doARC) doARC = YesNoBox("Process extracted (IDXZRC and IDXOUT) files?");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack archives
 if(doLNK) {
    STDSTRING file = pathname;
    file += TEXT("ARCHIVE00.LNK");
    UnpackK300(file.c_str());
   }

 if(doARC) {
    PS3::DYNASTY_WARRIORS::UnpackIDXOUT(pathname.c_str());
    PS3::DYNASTY_WARRIORS::IDXArchiveUnpackLoop(pathname.c_str());
   }

 // texture extraction
 if(doG1T)
    PS3::DYNASTY_WARRIORS::UnpackTextures(pathname.c_str());
 
 // model extraction
 if(doG1M)
    PS3::DYNASTY_WARRIORS::UnpackModels(pathname.c_str());

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