#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_dw_ps3.h"
#include "ps3_trinity.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   TRINITYSOULS

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

static bool DELETE_SMALL_FILES = true;
static UINT SMALL_FILESIZE = 0x100;

//
// UnpackIDX_V1
// Some older games use a different IDX/BIN archive format. IDX file is simply a
// list of 0x0C-byte items. For example, see game "Trinity: Souls of Zill O'll."
//
bool DWUnpackIDX_V1(LPCTSTR filename)
{
 // open IDX file
 ifstream ifile(filename, ios::binary);
 if(!ifile) {
    stringstream ss;
    ss << "Failed to open IDX file." << endl;
    ss << filename;
    return error(ss);
   }

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // compute filesize for IDX file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // construct BIN filename
 STDSTRING bfname = pathname;
 bfname += shrtname;
 bfname += TEXT(".BIN");

 // open BIN file
 ifstream bfile(bfname.c_str(), ios::binary);
 if(!bfile) return error("Failed to open BIN file.");

 // create destination directory
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname << TEXT("\\");
 STDSTRING savepath = ss.str();
 CreateDirectory(savepath.c_str(), NULL);

 // compute maximum number of indices
 uint32 max_index = filesize / 0x0C;
 if(!max_index) return error("Invalid LINKDATA.IDX file.");

 // read file information
 struct IDXITEM {
  uint32 offset;
  uint32 findex;
  uint32 p1; // length (uncompressed)
  uint32 p2; // length (compressed)
  uint32 p3; // filetype (idxout our idxzrc)
 };
 deque<IDXITEM> itemlist;
 uint32 offset = 0;
 for(uint32 i = 0; i < max_index; i++)
    {
     // read item
     IDXITEM item;
     item.p1 = BE_read_uint32(ifile);
     item.p2 = BE_read_uint32(ifile);
     item.p3 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // validate item
     if(!item.p1 && !item.p2) continue; // skip
     if(!(item.p3 == 0x00 || item.p3 == 0x01)) {
        stringstream ss;
        ss << "Unknown IDX compression format at offset 0x";
        ss << hex << ((uint32)ifile.tellg() - 0x04) << dec << ".";
        return error(ss, __LINE__);
       }

     // save item
     item.offset = offset;
     item.findex = i;
     itemlist.push_back(item);

     // increment offset and align to next 0x800-byte boundary
     offset += item.p2;
     offset = ((offset + 0x7FF) & (~0x7FF));
    }

 // process items
 for(uint32 i = 0; i < itemlist.size(); i++)
    {
     // construct output filename
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(4) << itemlist[i].findex << TEXT(".");
     if(itemlist[i].p3 == 0) ss << TEXT("idxout");
     else if(itemlist[i].p3 == 1) ss << TEXT("idxzrc");
     else ss << TEXT("unknown");

     // display information and ignore small files
     wcout << " FILE " << i << " of " << itemlist.size() << "." << endl;
     if(!(itemlist[i].p1 > SMALL_FILESIZE)) continue;

     // move to offset
     bfile.seekg(itemlist[i].offset);
     if(bfile.fail()) return error("Seek failure.");

     // read first four bytes and determine whether or not to save file
     // uint32 magic = BE_read_uint32(bfile);
     // if(bfile.fail()) continue;
     // if(!DWKeepFile(magic)) continue;

     // create 16 MB buffer and move to bin data
     boost::shared_array<char> data(new char[0x1000000]);
     bfile.seekg(itemlist[i].offset);
     if(bfile.fail()) return error("Seek failure.");

     // keep track of remaining bytes
     uint64 bytes_remaining = itemlist[i].p2;
     if(!(bytes_remaining < 0x2000000)) // 32 MB
        message("NOTE: This is a large file and might take a while to save.");

     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // transfer data
     while(bytes_remaining) {
           if(bytes_remaining < 0x1000000) {
              // read BIN data
              bfile.read(data.get(), bytes_remaining);
              if(bfile.fail()) return error("Read failure.");
              // save BIN data
              ofile.write(data.get(), bytes_remaining);
              if(ofile.fail()) return error("Write failure.");
              // nothing more to save
              bytes_remaining = 0;
             }
           else {
              // read BIN data
              bfile.read(data.get(), 0x1000000);
              if(bfile.fail()) return error("Read failure.");
              // save BIN data
              ofile.write(data.get(), 0x1000000);
              if(ofile.fail()) return error("Write failure.");
              // more to save
              bytes_remaining -= 0x1000000;
             }
          }
    }

 // finished
 cout << endl;
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
 p1 += TEXT("[PS3] Trinity: Souls of Zill O'll");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where LINKDATA.IDX is.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~12 GB free space.\n");
 p3 += TEXT("3. 4 GB for game + 8 GB extraction.\n");

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
 bool doIDX = true;
 bool doOUT = true;
 bool doG1T = true;
 bool doG1M = true;

 // questions
 if(doIDX) doIDX = YesNoBox("Process LINKDATA.IDX file?");
 if(doOUT) doOUT = YesNoBox("Process IDXOUT and IDXZRC files?");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack archives
 if(doIDX) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA.IDX");
    DWUnpackIDX_V1(file.c_str());
   }

 if(doOUT) {
    cout << "Processing .IDXZRC files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".idxzrc"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!PS3::DYNASTY_WARRIORS::DWUnpackIDXZRC_TypeA(filelist[i].c_str())) return false;
       }
    cout << endl;

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