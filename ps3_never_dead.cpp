#include "xentax.h"
#include "psp_heroes_phantasia.h"

namespace PSP { namespace HeroesOfPhantasia {

 class extractor {
  private :
   std::string pathname;
  private :
   bool processPSARC(const char* fullname);
   bool processTEXTURES(const char* fullname);
   bool CreatePaths(const char* root, const char* path);
  public :
   bool extract(void);
  public :
   extractor(const char* pn);
  ~extractor();
 };

};};

namespace PSP { namespace HeroesOfPhantasia {

extractor::extractor(const char* pn) : pathname(pn)
{
}

extractor::~extractor()
{
}

bool extractor::CreatePaths(const char* root, const char* path)
{
 // check string properties
 if(!path) return false;
 uint32 elem = strlen(path);
 if(!elem) return false;

 // parse directories
 std::deque<std::string> list;
 std::string temp;     
 for(size_t i = 0; i < elem; i++) {
     if(path[i] == '\\' || path[i] == '/') {
        if(temp.length()) {
           list.push_back(temp);
           temp.clear();
          }
       }
     else
        temp += path[i];
    }

 // create directories
 std::string curr_path = root;
 for(size_t i = 0; i < list.size(); i++) {
     curr_path += list[i];
     curr_path += "\\";
     CreateDirectoryA(curr_path.c_str(), NULL);
    }
      
 return true;
}

bool extractor::extract(void)
{
 using namespace std;

 cout << "STAGE 1" << endl;
 cout << "Processing PSARC files..." << endl;
 deque<string> filelist;
 BuildFilenameList(filelist, "psarc", pathname.c_str());
 for(size_t i = 0; i < filelist.size(); i++) {
     cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!processPSARC(filelist[i].c_str())) return false;
    }
 cout << endl;

 cout << "STAGE 2" << endl;
 cout << "Processing PS3_TEXTURES files..." << endl;
 filelist.clear();
 BuildFilenameList(filelist, "PS3_TEXTURES", pathname.c_str());
 for(size_t i = 0; i < filelist.size(); i++) {
     cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!processTEXTURES(filelist[i].c_str())) return false;
    }
 cout << endl;

 return true;
}

bool extractor::processPSARC(const char* fullname)
{
 string filepath = GetPathnameFromFilename(fullname);
 string filename = GetShortFilenameWithoutExtension(fullname);

 // open file
 ifstream ifile(fullname, ios::binary);
 if(!ifile) return error("Could not open PSARC file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(magic != 0x50534152) return error("Invalid PSARC file.");

 // read unknowns
 uint16 unk01 = BE_read_uint16(ifile);
 uint16 unk02 = BE_read_uint16(ifile);

 // read zlib
 uint32 zlib = BE_read_uint32(ifile);
 if(zlib != 0x7A6C6962) return error("Expecting zlib.");

 // read offset
 uint32 offset = BE_read_uint32(ifile);
 if(offset == 0) return error("Expecting zlib offset.");

 // read size of filename
 ifile.seekg(0x38);
 uint32 elem = BE_read_uint08(ifile);
 if(elem == 0) return error("Invalid filename length.");

 // read name
 char name[256];
 ifile.read(&name[0], elem);
 name[elem] = '\0';

 // build output file
 stringstream ss;
 ss << filepath << filename << ".arc";

 // create ouput file
 ofstream ofile(ss.str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // decompress data
 ifile.seekg(offset + elem);
 if(!DecompressZLIB(ifile, ofile)) return false;

 // display warning if leftover data
 if((filesize - ifile.tellg()) > 1024)
    cout << "Warning... more data!" << endl;

 return true;
}

bool extractor::processTEXTURES(const char* fullname)
{
 string filepath = GetPathnameFromFilename(fullname);
 string filename = GetShortFilenameWithoutExtension(fullname);

 // open file
 ifstream ifile(fullname, ios::binary);
 if(!ifile) return error("Could not open PS3_TEXTURES file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize == 0) return true;

 // read magic
 uint64 magic = BE_read_uint64(ifile);
 if(magic != 0x4173757261202020) return error("Invalid PSARC file.");

 // read textures
 for(;;)
    {
     // save position
     uint32 position = (uint32)ifile.tellg();
     if(ifile.fail() || ifile.eof()) return true;

     // read FCSR
     uint32 param01 = BE_read_uint32(ifile);
     if(ifile.fail() || ifile.eof()) return true;
     if(param01 == 0x00000000) return true;
     if(param01 != 0x46435352) return error("Expecting FCSR.");

     uint32 param02 = BE_read_uint32(ifile);
     uint32 param03 = BE_read_uint32(ifile);
     uint32 param04 = BE_read_uint32(ifile);
     uint32 param05 = BE_read_uint32(ifile);
     uint32 param06 = BE_read_uint32(ifile);
     uint32 param07 = BE_read_uint32(ifile);

     char relname[1024];
     read_string(ifile, &relname[0], 1024);
     cout << "name = " << relname << endl;

     // take difference between param02 and param07
     if(param02 < param07) return error("Expecting distance to be positive.");
     uint32 distance = param02 - param07;
     ifile.seekg(position + distance);

     CreatePaths(filepath.c_str(), relname);

     stringstream ss;
     ss << filepath << relname;
     ofstream ofile(ss.str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     boost::shared_array<char> data(new char[param07]);
     ifile.read(data.get(), param07);
     ofile.write(data.get(), param07);
    }

 return true;
}

};};

namespace PSP { namespace HeroesOfPhantasia {

bool extract(void)
{
 char pathname[MAX_PATH];
 GetModulePathname(pathname, MAX_PATH);
 return extract(pathname);
}

bool extract(const char* pathname)
{
 return extractor(pathname).extract();
}

};};