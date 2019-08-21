#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "ps3_dothack.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   DOTHACK

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processNUC(LPCTSTR filename)
{
 // open file
 if(!filename) return error("Invalid filename argument.");
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open NUCC file.");

 // move to 0x44
 ifile.seekg(0x44);
 if(ifile.fail()) return error("Seek failure.");

 // read strings
 int n_str = 0;
 int n_zer = 0;
 for(;;) {
     char data[1024];
     data[0] = '\0';
     bool test = read_string(ifile, data, 1024);
     if(!test || !strlen(data)) {
       }
     n_str++;
    }

 cout << "tablesize = 0x" << hex << n_str << dec << endl;

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
 p1 += TEXT("[PS3] .hack//Versus");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Use QuickBMS to extract CPK files.\n");
 p2 += TEXT("3. Run ripper.\n");
 p2 += TEXT("4. Select game and click Browse.\n");
 p2 += TEXT("5. Select folder where game files are.\n");
 p2 += TEXT("6. Click OK.\n");
 p2 += TEXT("7. Answer questions.\n");
 p2 += TEXT("8. Watch console for a few hours.\n");
 p2 += TEXT("9. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("10. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~ GB free space.\n");
 p3 += TEXT("3.  GB for game +  GB extraction.\n");
 p3 += TEXT("4. CPK extraction is complex: read notes folder.");

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
 bool doNUC = true;
 // bool doLDB = true;
 // bool doLDC = true;
 // bool doLDD = true;
 // bool doDLC = true;
 // bool doARC = true;
 // bool doG1T = true;
 // bool doG1M = true;

 // questions
 if(doNUC) doNUC = YesNoBox("Process NUCC files?\nSay 'No' if you have already done this before.");
 // if(doDLC) doDLC = YesNoBox("Process PS3 DLC (BIN) files?");
 // if(doARC) doARC = YesNoBox("Process extracted (IDXZRC and IDXOUT) files?");
 // if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 // if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // stage #1
 cout << "STAGE 1" << endl;
 if(doNUC) {
    // process TMD files
    cout << "Processing .ARC files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".ARC"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processNUC(filelist[i].c_str())) return false;
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