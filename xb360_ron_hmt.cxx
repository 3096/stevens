#ifndef __XB360_RON_HMT_CXX_
#define __XB360_RON_HMT_CXX_

#define X_SYSTEM XBOX360
#define X_GAME   RON

namespace X_SYSTEM { namespace X_GAME {

class HMTProcessor {
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
  bool extract(void);
 public :
  HMTProcessor(const std::string& fname);
 ~HMTProcessor();
};

HMTProcessor::HMTProcessor(const std::string& fname)
{
 filename = fname;
 pathname = GetPathnameFromFilename(fname);
 shrtname = GetShortFilenameWithoutExtension(fname);
}

HMTProcessor::~HMTProcessor()
{
}

bool HMTProcessor::extract(void)
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

}};

#endif
