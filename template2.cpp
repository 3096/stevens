#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "template.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   GAMETITLE

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

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

 // // variables
 // bool doANS = false;

 // // process archive
 // cout << "STAGE 1" << endl;
 // if(doANS) {
 //    cout << "Processing .ANS files..." << endl;
 //    deque<string> filelist;
 //    BuildFilenameList(filelist, ".ANS", pathname.c_str());
 //    for(size_t i = 0; i < filelist.size(); i++) {
 //        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
 //        if(!processARC(filelist[i])) return false;
 //       }
 //    cout << endl;
 //   }

 return true;
}

}};

