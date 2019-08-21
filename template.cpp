#include "xentax.h"
#include "x_file.h"
#include "x_amc.h"
#include "template.h"

#define X_SYSTEM PS3
#define X_GAME   ONE_PIECE

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

class unpacker {
 private :
  std::string filename;
  std::string pathname;
  std::string shrtname;
 private :
 public :
  bool unpack(void);
 public :
  unpacker(const char* fname);
 ~unpacker() {}
};

unpacker::unpacker(const char* fname) : filename(fname)
{
 pathname = GetPathnameFromFilename(fname);
 shrtname = GetShortFilenameWithoutExtension(fname);
}

bool unpacker::unpack(void)
{
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

class modelizer {
 private :
  bool debug;
  std::string filename;
  std::string pathname;
  std::string shrtname;
  std::ifstream ifile;
  std::ofstream ofile;
  std::ofstream dfile;
 private :
  ADVANCEDMODELCONTAINER amc;
 private :
 public :
  bool set_debug(bool state);
  bool extract(void);
 public :
  modelizer(const std::string& fname);
 ~modelizer() {}
};

modelizer::modelizer(const std::string& fname) : filename(fname)
{
 debug = false;
 pathname = GetPathnameFromFilename(fname);
 shrtname = GetShortFilenameWithoutExtension(fname);
}

bool modelizer::set_debug(bool state)
{
 bool prev = debug;
 debug = state;
 return prev;
}

bool modelizer::extract(void)
{
 // open file
 using namespace std;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // create debug file
 std::string dfname = pathname;
 dfname += shrtname;
 dfname += ".txt";
 if(debug) dfile.open(dfname.c_str());

 return true;
}

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
 string pathname(pname);
 if(!pathname.length()) return error("Invalid pathname.");

 // filetypes to process
 bool doBIN = true;

 // process filetype
 if(doBIN && YesNoBox("Process BIN files?\nClick 'No' if you have already done so."))
   {
    // cout << "Processing .BIN files..." << endl;
    // deque<string> filelist;
    // BuildFilenameList(filelist, ".bin", pathname.c_str());
    // sort(filelist.begin(), filelist.end());
    // for(size_t i = 0; i < filelist.size(); i++) {
    //     cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
    //     if(!processBIN(filelist[i])) return false;
    //    }
    // cout << endl;
   }

 return true;
}

};};