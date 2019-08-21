#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_xbox360.h"
#include "xb360_wwe2k14.h"
using namespace std;

#define X_SYSTEM XB360
#define X_GAME   WWE2K14

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processPAC(const string& filename)
{
 // decompress
 bool status = XB360Decompress(nullptr, filename.c_str());
 if(!status) return false;

 return true;
};

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
 bool doPAC = true;

 // process archive
 cout << "STAGE 1" << endl;
 if(doPAC) {
    cout << "Processing .PAC files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".PAC", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processPAC(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

}};

