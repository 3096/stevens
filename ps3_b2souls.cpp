#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "ps3_b2souls.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   BEYOND_TWO_SOULS

//
// BIN PROCESSING
//

namespace X_SYSTEM { namespace X_GAME {

bool processBIN(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read header
 uint32 h01 = BE_read_uint32(ifile); // 0x44435F49 DC_I
 uint32 h02 = BE_read_uint32(ifile); // 0x4E464F20 NFO_
 uint32 h03 = BE_read_uint32(ifile); // 0x02
 uint32 h04 = BE_read_uint32(ifile); // offset to DC_DATA
 uint32 h05 = BE_read_uint32(ifile); // number of entries
 if(ifile.fail()) return error("Read failure.");

 // read entries
 struct BTSENTRYINFO {
  uint32 p01;
  uint32 p02;
 };
 deque<BTSENTRYINFO> entrylist;
 for(uint32 i = 0; i < h05; i++) {
     BTSENTRYINFO item;
     item.p01 = BE_read_uint32(ifile);
     item.p02 = BE_read_uint32(ifile);
     entrylist.push_back(item);
    }
 
 // move to DC_DATA
 ifile.seekg(h04);
 if(ifile.fail()) return error("Seek failure.");

 uint64 dcd01 = BE_read_uint64(ifile);
 if(dcd01 != 0x44435F44415441ul) return error("Expecting DC_DATA.");


 // 00 00 08 59 - 00 00 8C 9C - FileText
 // 00 00 08 55 - 00 00 39 FB - ShadCust
 // 00 00 08 52 - 00 00 55 B5 - MESHDATA
 // 00 00 07 D9 - 00 00 56 57 - ENTITY + MESH
 // 00 00 07 D2 - 00 00 B8 F3 - NODE
 // 00 00 07 D2 - 00 00 B8 F2 - NODE
 // 00 00 08 10 - 00 00 4D D6 - ENTITY + TRANSFOR
 // 00 00 07 D2 - 00 00 B8 F4 - NODE
 // 00 00 07 D7 - 00 00 06 04 - ENTITY
 // 00 00 07 D2 - 00 00 B8 F5 - NODE
 // 00 00 07 D7 - 00 00 06 05 - ENTITY
 // 00 00 07 D2 - 00 00 B8 F6 - NODE
 // 00 00 07 D7 - 00 00 06 06 - ENTITY
 // 00 00 07 D2 - 00 00 B8 F7 - NODE
 // 00 00 07 D7 - 00 00 06 07 - ENTITY

 // 0x52B8 = offset to texture data
 
 // DC_DATA FORMAT
 // BigFile_PS3_d01\2038\000.out
 // 00 00 00 9B
 // 01
 // data (0x9B bytes)

 // 01
 // 00 00 00 03
 // 00 00 00 01
 // 00 02 00 00 (these are sizes 005.out)
 // 00 00 80 00 (these are sizes 006.out)
 // 00 00 2A B8 (these are sizes 001.out)

 // 01
 // 00 01 87 69
 // 00 02 00 00
 // 00 00 40 74
 // 00 00 68 C2
 // 00 00 80 00
 // 00 01 C7 DD
 // 00 00 24 2D - size of segs section
 // 00 00 2A B8 - size of decompressed data

 // 00 00 00 00
 // 00 00 00 03 - COM_CONT, LOADCONT, FILETEXT

 // DC_DATA FORMAT

 // 44435F4441544120 -- DC_DATA
 // 00 00 00 07         -- ???
 // 00 02 49 65         -- size of all data after this point (BigFile_PS3_dat\1469 and BigFile_PS3_d01\0000)
 // 00 00 00 9B         -- section size
 // 01                  -- section count
 // 00 00 00 03         -- subsection count

 // 01 00 00 00 03 00 00 00 01 00 02
 // 00 00 00 00 80 00 00 00 2A B8 01

 // 02 00 00 00 00 40 74 00 00 68 C2
 // 01 C7 DD 00 00 24 2D 00 00 2A B8

 // 00 00 24 2D -- size of segs
 // 00 00 2A B8 -- size of segs data decompressed

 // 00 00 00 10 - number of entries
 // 00 00 00 00 - ???
 // 00 00 08 59 00 01 AA 52 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 7D B8 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 AA 51 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 AA 4A 04 00 00 00 00 - entry
 // 00 00 08 59 00 01 AA 50 01 00 00 00 00 - entry
 // 00 00 08 59 00 01 AA 4F 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 AA 4E 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 5B 01 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 5B 00 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 5A FF 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 CD AB 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 7D B7 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 CD C5 04 00 00 00 00 - entry
 // 00 00 08 59 00 01 7D B6 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 AA 4D 00 00 00 00 00 - entry
 // 00 00 08 59 00 01 AA 4C 00 00 00 00 01 - entry
 // 00 00 02 09 - size of data[0]
 // A1 B5 23 D6 - ???? of data[0]
 // data[0]
 // 0x00 padding (0x3C bytes)
 // 00 00 02 84 - size of data[1]
 // 66 5E 66 FE - ???? of data[1]
 // data[1]
 // 0x00 padding


 // 00 00 00 03
 // COM_CONT
 // 00 00 00 03
 // 00 00 00 00
 // LOADCONT
 // 00 00 00 02
 // FILETEXT

 // 00 00 00 03
 // COM_CONT
 // 00 00 00 03
 // 00 00 00 00
 // LOADCONT
 // 00 00 00 02
 // SHADCUST

 return true;
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
 p1 += TEXT("[PS3] Beyond Two Souls");

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
 p3 += TEXT("2. You need ~12 GB free space.\n");
 p3 += TEXT("3. 4 GB for game + 8 GB extraction.\n");
 p3 += TEXT("4. For DLC, EDATs must be decrypted to DATs.");

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
 bool doBIN = true;

 // questions
 if(doBIN) doBIN = YesNoBox("Process BIN files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process BIN
 cout << "STAGE 1" << endl;
 if(doBIN) {
    cout << "Processing .BIN files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BIN"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processBIN(filelist[i].c_str())) return false;
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