#include "xentax.h"
#include "x_findfile.h"
#include "x_file.h"
#include "x_xbox360.h"
#include "x_smc.h"
#include "x_lwo.h"
#include "x_max.h"
#include "xb360_onechanbara2.h"

namespace XB360 { namespace Onechanbara2 {

struct SURFACEINFO {
 real32 bbox_xmin;
 real32 bbox_ymin;
 real32 bbox_zmin;
 real32 bbox_xmax;
 real32 bbox_ymax;
 real32 bbox_zmax;
 uint32 index;
 uint16 elem;
 uint16 type;
 std::string name;
};

struct TEXTASSINFO {
 uint32 material_index;
 uint32 trilist;
};

struct TRILISTINFO {
 uint32 triangles;
 uint32 offset;
};

class extractor {
 private :
  std::string pathname;
 private :
  bool decompress(const char* filename, const char* extension);
  bool processPK1(const char* filename);
  bool processPK2(const char* filename);
  bool processPK3(const char* filename);
  bool processTEX(const char* filename);
  bool processMDL(const char* filename);
  bool processTMO(const char* filename);
 public :
  bool extract(void);
 public :
  extractor(const char* pn);
 ~extractor();
};

std::string GetTEXFromMDL(const std::string& filename);
bool GetFilenamesFromTEX(const std::string& texfile, std::deque<std::string>& filenames);

};};

namespace XB360 { namespace Onechanbara2 {

extractor::extractor(const char* pn) : pathname(pn)
{
}

extractor::~extractor()
{
}

bool extractor::extract(void)
{
 using namespace std;

 // CAT files are archives that do not contain filenames
 bool decompressCAT = true;
 bool doPK1 = true;

 // CAN files are archives that also contain filenames
 bool decompressCAN = true;
 bool doPK2 = true;

 // CMP files are variable files in the demo, gamegui, and sys directories
 bool decompressCMP = true;
 bool doPK3 = true;

 // XPR files are texture files
 bool decompressXPR = true;
 bool doTEX = true;

 // TMD files are model files
 bool decompressTMD = true;
 bool doMDL = true;
 bool doTMO = false;

 // decompress CAT files
 if(decompressCAT) {
    std::cout << "STAGE 1" << std::endl;
    std::cout << "Decompressing CAT files to PK1 files using XBox360 unbundler." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "cat", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!decompress(filelist[i].c_str(), "pk1")) return false;
       }
    std::cout << std::endl;
   }

 // decompress CAN files
 if(decompressCAN) {
    std::cout << "STAGE 2" << std::endl;
    std::cout << "Decompressing CAN files to PK2 files using XBox360 unbundler." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "can", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!decompress(filelist[i].c_str(), "pk2")) return false;
       }
    std::cout << std::endl;
   }

 // decompress CMP files
 if(decompressCMP) {
    std::cout << "STAGE 3" << std::endl;
    std::cout << "Decompressing CMP files to PK3 files using XBox360 unbundler." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "cmp", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!decompress(filelist[i].c_str(), "pk3")) return false;
       }
    std::cout << std::endl;
   }

 // decompress XPR files
 if(decompressXPR) {
    std::cout << "STAGE 4" << std::endl;
    std::cout << "Decompressing XPR files to TEX files using XBox360 unbundler." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "xpr", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!decompress(filelist[i].c_str(), "tex")) return false;
       }
    std::cout << std::endl;
   }

 // decompress TMD files
 if(decompressTMD) {
    std::cout << "STAGE 5" << std::endl;
    std::cout << "Decompressing TMD files to MDL files using XBox360 unbundler." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "tmd", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!decompress(filelist[i].c_str(), "mdl")) return false;
       }
    std::cout << std::endl;
   }

 // process PK1 files
 if(doPK1) {
    std::cout << "STAGE 6" << std::endl;
    std::cout << "Processing PK1 files." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "pk1", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processPK1(filelist[i].c_str())) return false;
       }
    std::cout << std::endl;
   }

 // process PK2 files
 if(doPK2) {
    std::cout << "STAGE 7" << std::endl;
    std::cout << "Processing PK2 files." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "pk2", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processPK2(filelist[i].c_str())) return false;
       }
    std::cout << std::endl;
   }

 // process PK3 files
 if(doPK3) {
    std::cout << "STAGE 8" << std::endl;
    std::cout << "Processing PK3 files." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "pk3", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processPK3(filelist[i].c_str())) return false;
       }
    std::cout << std::endl;
   }

 // process TEX files
 if(doTEX) {
    std::cout << "STAGE 9" << std::endl;
    std::cout << "Processing TEX files." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "tex", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processTEX(filelist[i].c_str())) return false;
       }
    std::cout << std::endl;
   }

 // process MDL files
 if(doMDL) {
    std::cout << "STAGE 10" << std::endl;
    std::cout << "Processing MDL files." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "mdl", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processMDL(filelist[i].c_str())) return false;
       }
    std::cout << std::endl;
   }

 // process TMO files
 if(doTMO) {
    std::cout << "STAGE 11" << std::endl;
    std::cout << "Processing TMO files." << std::endl;
    std::deque<string> filelist;
    BuildFilenameList(filelist, "tmo", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processTMO(filelist[i].c_str())) return false;
       }
    std::cout << std::endl;
   }

 return true;
}

bool extractor::decompress(const char* filename, const char* extension)
{
 using namespace std;

 // make sure file is compressed
 std::ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // file is not compressed
 if(!(magic >= 0xFF51200 && magic <= 0xFF512FF))
   {
    // build filename
    string ofname = GetPathnameFromFilename(filename);
    ofname += GetShortFilenameWithoutExtension(filename);
    ofname += ".";
    ofname += extension;

    // create output file
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");

    // compute filesize
    ifile.seekg(0, ios::end);
    size_t filesize = (size_t)ifile.tellg();
    ifile.seekg(0, ios::beg);

    // read data
    boost::shared_array<char> data(new char[filesize]);
    ifile.read(data.get(), filesize);
    if(ifile.fail()) return error("Read failure.");
    ifile.close();

    // copy data
    ofile.write(data.get(), filesize);

    // delete original
    //DeleteFileA(filename);
    return true;
   }
 else
    ifile.close();

 // command line
 string param1 = "\"";
 param1 += pathname;
 param1 += "xbdecompress.exe";
 param1 += "\"";
 param1 += " ";
 param1 += "\"";
 param1 += filename;
 param1 += "\"";
 param1 += " ";
 param1 += "\"";
 param1 += GetPathnameFromFilename(filename);
 param1 += GetShortFilenameWithoutExtension(filename);
 param1 += ".";
 param1 += extension;
 param1 += "\"";

 // process startup information
 STARTUPINFOA sinfo;
 ZeroMemory(&sinfo, sizeof(sinfo));
 sinfo.cb = sizeof(sinfo);

 // process information
 PROCESS_INFORMATION pinfo;
 ZeroMemory(&pinfo, sizeof(pinfo));

 // run xbdecompress
 char buffer[1024];
 memmove(buffer, param1.c_str(), param1.length() + 1);
 if(!CreateProcessA(NULL, buffer, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &sinfo, &pinfo)) return error("Failed to run xbdecompress on XPR file.");
 WaitForSingleObject(pinfo.hProcess, INFINITE);
 CloseHandle(pinfo.hProcess);
 CloseHandle(pinfo.hThread);

 // delete original
 //DeleteFileA(filename);
 return true;
}

bool extractor::processPK1(const char* filename)
{
 using namespace std;

 // open file
 std::ifstream ifile(filename, ios::binary);
 if(!ifile) return error("error");

 // read header
 std::deque<std::pair<size_t, size_t>> header;
 for(;;) {
     uint32 p01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     uint32 p02 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     if(p01 == 0xFFFFFFFF && p02 == 0xFFFFFFFF) break;
     if(header.size() > 1000) return error("Unexpected number of PAK sections.");
     header.push_back(std::pair<size_t, size_t>(p01, p02));
    }

 // create directory to store files
 string pakpath = GetPathnameFromFilename(filename);
 pakpath += GetShortFilenameWithoutExtension(filename);
 pakpath += "\\";
 CreateDirectoryA(pakpath.c_str(), NULL);

 // process files
 std::deque<string> subpacks;
 for(size_t i = 0; i < header.size(); i++)
    {
     // move to data
     ifile.seekg(header[i].first);
     if(ifile.fail()) return error("Seek failure.");
     if(header[i].second == 0) continue;

     // read data
     boost::shared_array<char> data(new char[header[i].second]);
     ifile.read(data.get(), header[i].second);
     if(ifile.fail()) return error("Read failure.");

     // create filename
     stringstream ss;
     ss << pakpath << setfill('0') << setw(3) << i << ".";
     uint32 value = *reinterpret_cast<uint32*>(data.get());
     switch(value) {
       case(0x746D6F30) : ss << "tmo"; break;
       case(0x746D6430) : ss << "mdl"; break;
       case(0x32525058) : ss << "tex"; break;
       case(0x54495400) : ss << "tit"; break;
       case(0x544C4900) : ss << "tli"; break;
       case(0x50524D70) : ss << "prm"; break;
       case(0x52435354) : ss << "rcs"; break;
       case(0x44544F4C) : ss << "dto"; break;
       case(0x4C43504F) : ss << "lcp"; break;
       case(0x4244534C) : ss << "bds"; break;
       case(0x4553544D) : ss << "mts"; break;
       case(0x64697242) : ss << "dir"; break;
       case(0x6C6C6F52) : ss << "llo"; break;
       case(0x706F7250) : ss << "por"; break;
       case(0x7366664F) : ss << "sff"; break;
       case(0x74697753) : ss << "tiw"; break;
       case(0x00000000) : ss << "dat"; break;
       case(0x01000000) : ss << "dat"; break;
       case(0x01000100) : ss << "dat"; break;
       case(0x01000200) : ss << "dat"; break;
       case(0x01000300) : ss << "dat"; break;
       case(0x01000400) : ss << "dat"; break;
       case(0x01000500) : ss << "dat"; break;
       case(0x02000000) : ss << "pla"; break;
       case(0x10000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x20000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x30000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x40000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x50000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x60000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x70000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x80000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x90000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0xA0000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0xB0000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0xC0000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       default : ss << "dat";
      }

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     ofile.write(data.get(), header[i].second);
    }

 // delete original
 ifile.close();
 //DeleteFileA(filename);

 // process subpacks
 for(size_t i = 0; i < subpacks.size(); i++)
     if(!processPK1(subpacks[i].c_str())) return false;

 return true;
}

bool extractor::processPK2(const char* filename)
{
 using namespace std;

 // open file
 std::ifstream ifile(filename, ios::binary);
 if(!ifile) return error("error");

 // filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read header
 struct triple { size_t offset, size; string name; };
 std::deque<triple> header;
 size_t position = 0;
 for(;;)
    {
     // seek position
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read item
     uint32 p01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     uint32 p02 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     if(p01 == 0xFFFFFFFF && p02 == 0xFFFFFFFF) break;

     // name follows
     bool is_name = false;
     uint32 namelen = 0;
     if(p01 < p02) {
        if((p02 - p01) == 0x10) { namelen = 0x10; is_name = true; }
        else if((p02 - p01) == 0x20) { namelen = 0x20; is_name = true; }
        else if((p02 - p01) == 0x30) { namelen = 0x30; is_name = true; }
        else if(p02 == 0xFFFFFFFF) { namelen = filesize - p01; is_name = true; }
       }

     // extract name
     if(is_name) {
        ifile.seekg(p01);
        char buffer[1024];
        read_string(ifile, buffer, 1024);
        header.back().name = buffer;
       }
     else {
        triple item;
        item.offset = p01;
        item.size = p02;
        header.push_back(item);
       }

     // move position
     if(is_name) position += 4;
     else position += 8;
    }

 // create directory to store files
 string sendpath = GetPathnameFromFilename(filename);
 sendpath += GetShortFilenameWithoutExtension(filename);
 sendpath += "\\";
 CreateDirectoryA(sendpath.c_str(), NULL);

 // process files
 std::deque<string> subpacks;
 for(size_t i = 0; i < header.size(); i++)
    {
     // move to data
     ifile.seekg(header[i].offset);
     if(ifile.fail()) return error("Seek failure.");
     if(header[i].size == 0) continue;

     // read data
     boost::shared_array<char> data(new char[header[i].size]);
     ifile.read(data.get(), header[i].size);
     if(ifile.fail()) return error("Read failure.");

     // create filename
     stringstream ss;
     ss << sendpath;
     if(header[i].name.length()) ss << GetShortFilenameWithoutExtension(header[i].name) << ".";
     else ss << setfill('0') << setw(3) << i << ".";

     // save data
     uint32 value = *reinterpret_cast<uint32*>(data.get());
     switch(value) {
       case(0x746D6F30) : ss << "tmo"; break;
       case(0x746D6430) : ss << "mdl"; break;
       case(0x32525058) : ss << "tex"; break;
       case(0x54495400) : ss << "tit"; break;
       case(0x544C4900) : ss << "tli"; break;
       case(0x50524D70) : ss << "prm"; break;
       case(0x52435354) : ss << "rcs"; break;
       case(0x44544F4C) : ss << "dto"; break;
       case(0x4C43504F) : ss << "lcp"; break;
       case(0x4244534C) : ss << "bds"; break;
       case(0x4553544D) : ss << "mts"; break;
       case(0x64697242) : ss << "dir"; break;
       case(0x6C6C6F52) : ss << "llo"; break;
       case(0x706F7250) : ss << "por"; break;
       case(0x7366664F) : ss << "sff"; break;
       case(0x74697753) : ss << "tiw"; break;
       case(0x00000000) : ss << "dat"; break;
       case(0x01000000) : ss << "dat"; break;
       case(0x01000100) : ss << "dat"; break;
       case(0x01000200) : ss << "dat"; break;
       case(0x01000300) : ss << "dat"; break;
       case(0x01000400) : ss << "dat"; break;
       case(0x01000500) : ss << "dat"; break;
       case(0x02000000) : ss << "pla"; break;
       case(0x10000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x20000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x30000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x40000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x50000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x60000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x70000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x80000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0x90000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0xA0000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0xB0000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       case(0xC0000000) : ss << "pk1"; subpacks.push_back(ss.str()); break; // more pk1 files!
       default : ss << "dat";
      }

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     ofile.write(data.get(), header[i].size);
    }

 // delete original
 ifile.close();
 //DeleteFileA(filename);

 // process subpacks
 for(size_t i = 0; i < subpacks.size(); i++)
     if(!processPK1(subpacks[i].c_str())) return false;

 return true;
}

bool extractor::processPK3(const char* filename)
{
 using namespace std;

 // open file
 std::ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // determine file extension
 string extension = "";
 if(magic == 0x30646D74) extension = "mdl";
 else if(magic == 0x58505232) extension = "tex";
 else if(magic == 0x54455854) extension = "sub";
 else return error("Unknown PK3 file format.");

 // build filename
 string ofname = GetPathnameFromFilename(filename);
 ofname += GetShortFilenameWithoutExtension(filename);
 ofname += ".";
 ofname += extension;

 // create output file
 ofstream ofile(ofname.c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read data
 boost::shared_array<char> data(new char[filesize]);
 ifile.read(data.get(), filesize);
 if(ifile.fail()) return error("Read failure.");
 ifile.close();

 // copy data
 ofile.write(data.get(), filesize);

 // delete original
 //DeleteFileA(filename);
 return true;
}

bool extractor::processTEX(const char* filename)
{
 using namespace std;

 // file properties
 string filepath = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilename(filename);

 // create storage path
 string savepath = GetPathnameFromFilename(filename);
 savepath += GetShortFilenameWithoutExtension(filename);
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // create storage path subdirectory
 // some XPR files have subdirectories, and unless you have a subdirectory name that way, unbundler won't save anything
 // fortunately, the subdirectory is always called tex
 string savepathsubdir = savepath;
 savepathsubdir += "tex\\";
 CreateDirectoryA(savepathsubdir.c_str(), NULL);

 // unbundle and delete RDF files
 bool retval = XB360Unbundle(pathname.c_str(), filename, savepath.c_str());
 string rdffile = filepath;
 rdffile += shrtname;
 rdffile += ".rdf";
 DeleteFileA(rdffile.c_str());
 return retval;
}

bool extractor::processMDL(const char* filename)
{
 // validate
 using namespace std;
 if(!filename || !strlen(filename)) return error("Invalid filename.");

 // variables
 string fn_path = GetPathnameFromFilename(filename);
 string fn_name = GetShortFilenameWithoutExtension(filename);

 // data
 uint32 fileinfo[30];
 std::deque<SURFACEINFO> surface_data;
 std::deque<TEXTASSINFO> textass_data;
 std::deque<TRILISTINFO> trilist_data;

 // initialize model
 SIMPLEMODELCONTAINER1 smc;
 smc.vbuffer.flags = 0;
 smc.vbuffer.name = "mesh";
 smc.vbuffer.elem = 0;

 // enable debug mode
 bool debug = false;
 ofstream dfile;
 if(debug) {
    stringstream ss;
    ss << fn_path << fn_name << ".txt";
    dfile.open(ss.str().c_str());
   }

 // open file
 std::ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open MDL file.");

 //
 // PROCESS #01
 // READ AND PROCESS FILE HEADER
 //

 // read header
 auto head01 = BE_read_uint32(ifile);
 auto head02 = BE_read_uint32(ifile);
 auto head03 = BE_read_uint32(ifile);
 auto head04 = BE_read_uint32(ifile);
 auto head05 = BE_read_real32(ifile);
 auto head06 = BE_read_real32(ifile);
 auto head07 = BE_read_real32(ifile);
 auto head08 = BE_read_real32(ifile);
 auto head09 = BE_read_real32(ifile);
 auto head10 = BE_read_real32(ifile);
 if(debug) dfile << "HEADER" << std::endl;
 if(debug) dfile << head01 << std::endl;
 if(debug) dfile << head02 << std::endl;
 if(debug) dfile << head03 << std::endl;
 if(debug) dfile << head04 << std::endl;
 if(debug) dfile << head05 << std::endl;
 if(debug) dfile << head06 << std::endl;
 if(debug) dfile << head07 << std::endl;
 if(debug) dfile << head08 << std::endl;
 if(debug) dfile << head09 << std::endl;
 if(debug) dfile << head10 << std::endl;
 if(debug) dfile << std::endl;

 // validate header
 if(head01 != 0x30646D74) return error("Invalid MDL file.");
 if(head04 != 0x28) return error("Invalid MDL file.");

 // determine vertex size
 uint32 vertexsize = 0;
 switch(head02) {
   case(0x1D01AF) : vertexsize = 0x0C; break; // OK (HUD)
   case(0x1D01BF) : vertexsize = 0x0C; break; // OK (PLAYER WEAPONS)
   case(0x1D02BF) : vertexsize = 0x0C; break; // OK
   case(0x1D80A7) : vertexsize = 0x18; break; // OK
   case(0x1D80AF) : vertexsize = 0x1C; break; // OK
   case(0x1D81A7) : vertexsize = 0x18; break; // OK
   case(0x1D81AF) : vertexsize = 0x1C; break; // OK
   case(0x1D82A7) : vertexsize = 0x18; break; // OK (HUGE MAPS)
   case(0x1D82AF) : vertexsize = 0x1C; break; // OK (HUGE MAPS)
   case(0x1DB8A7) : vertexsize = 0x20; break; // OK (LEVEL MODELS)
   case(0x1DB8AF) : vertexsize = 0x24; break; // OK (LEVEL MODELS)
   case(0x1DB9A7) : vertexsize = 0x20; break; // OK (PLAYER AND ENEMIES)
   default : {
        stringstream msg;
        msg << "Unknown vertex type " << head02 << ".";
        return error(msg.str().c_str());
       }
 }

 // determine index size and type
 unsigned char indextype = FACE_FORMAT_UINT_16;
 unsigned char indexsize = 2;
 if(head02 == 0x1D02BF || head02 == 0x1D82A7 || head02 == 0x1D82AF) {
    indextype = FACE_FORMAT_UINT_32;
    indexsize = 4;
   }

 //
 // PROCESS #02
 // READ AND SAVE FILE INFORMATION
 //
 if(debug) dfile << "FILE INFORMATION" << std::endl;
 for(size_t i = 0; i < 30; i++) fileinfo[i] = BE_read_uint32(ifile);
 if(debug) for(size_t i = 0; i < 30; i++) if(debug) dfile << fileinfo[i] << std::endl;
 if(debug) dfile << std::endl;

 //
 // PROCESS #03
 // READ SURFACE INFORMATION PART 1
 //

 // move to surface information
 if(debug) dfile << "SURFACE INFORMATION PART 1" << std::endl;
 ifile.seekg(fileinfo[0]);
 if(ifile.fail()) return error("Seek failure.");

 // process surfaces
 uint32 n_surfaces = fileinfo[1];
 if(n_surfaces == 0 || n_surfaces > 1000) return error("Unexpected number of surfaces.");
 if(debug) dfile << "Number of surfaces = " << n_surfaces << std::endl;

 // process surfaces
 for(uint32 i = 0; i < n_surfaces; i++)
    {
     if(debug) dfile << "SURFACE " << i << std::endl;
     auto param01 = BE_read_real32(ifile);
     auto param02 = BE_read_real32(ifile);
     auto param03 = BE_read_real32(ifile);
     auto param04 = BE_read_real32(ifile);
     auto param05 = BE_read_real32(ifile);
     auto param06 = BE_read_real32(ifile);
     if(debug) dfile << "box_min_x = " << param01 << std::endl;
     if(debug) dfile << "box_min_y = " << param02 << std::endl;
     if(debug) dfile << "box_min_z = " << param03 << std::endl;
     if(debug) dfile << "box_max_x = " << param04 << std::endl;
     if(debug) dfile << "box_max_y = " << param05 << std::endl;
     if(debug) dfile << "box_max_z = " << param06 << std::endl;

     // read index
     auto param07 = BE_read_uint32(ifile);
     if(debug) dfile << "index = " << param07 << std::endl;

     // read other parameters
     auto param08 = BE_read_uint16(ifile);
     auto param09 = BE_read_uint16(ifile);
     if(debug) dfile << "param08 = " << param08 << std::endl;
     if(debug) dfile << "param09 = " << param09 << std::endl;

     // read name
     char param10[5];
     BE_read_array(ifile, &param10[0], 4);
     param10[4] = '\0';
     std::swap(param10[0], param10[3]);
     std::swap(param10[1], param10[2]);
     if(debug) dfile << "name = " << param10 << std::endl;

     // read other parameters
     auto param11 = BE_read_uint32(ifile);
     auto param12 = BE_read_uint32(ifile);
     auto param13 = BE_read_uint32(ifile);
     auto param14 = BE_read_uint32(ifile);
     auto param15 = BE_read_uint32(ifile);
     if(debug) dfile << "param11 = " << param11 << std::endl;
     if(debug) dfile << "param12 = " << param12 << std::endl;
     if(debug) dfile << "param13 = " << param13 << std::endl;
     if(debug) dfile << "param14 = " << param14 << std::endl;
     if(debug) dfile << "param15 = " << param15 << std::endl;
     if(debug) dfile << std::endl;

     // set surface information
     SURFACEINFO si;
     si.bbox_xmin = param01;
     si.bbox_ymin = param02;
     si.bbox_zmin = param03;
     si.bbox_xmax = param04;
     si.bbox_ymax = param05;
     si.bbox_zmax = param06;
     si.index = param07;
     si.elem = param08;
     si.type = param09;
     si.name = param10;
     surface_data.push_back(si);
    }

 //
 // PROCESS #04
 // READ SURFACE INFORMATION PART 2
 //

 // move to surface information
 if(debug) dfile << "SURFACE INFORMATION PART 2" << std::endl;
 ifile.seekg(fileinfo[2]);
 if(ifile.fail()) return error("Seek failure.");

 // read surface information
 uint32 n_values = fileinfo[3];
 for(size_t i = 0; i < n_surfaces; i++)
    {
     // read group information
     boost::shared_array<uint16> data(new uint16[surface_data[i].elem]);
     BE_read_array(ifile, data.get(), surface_data[i].elem);
     if(ifile.fail()) return error("Read failure.");

     // 20XX reference to part 4
     // 40XX reference to part 3
     // 30XX reference to group
     uint16 curr_0x2000 = 0xFFFF; // texture_data reference
     uint16 curr_0x3000 = 0xFFFF; // trilist_data reference
     uint16 curr_0x4000 = 0xFFFF; // unknown reference

     // for each item read
     for(size_t j = 0; j < surface_data[i].elem; j++)
        {
         if(data[j] == 0x1000)
           {
            curr_0x2000 = 0xFFFF;
            curr_0x3000 = 0xFFFF;
            curr_0x4000 = 0xFFFF;
            if(debug) dfile << "0x1000" << endl;
           }
         else if((data[j] & 0xFF00) == 0x2000)
           {
            curr_0x2000 = (uint16)(data[j] & 0x00FF);
            if(debug) dfile << "0x" << std::hex << data[j] << " ";
           }
         else if((data[j] & 0xFF00) == 0x3000)
           {
            curr_0x3000 = (uint16)(data[j] & 0x00FF);
            if(debug) dfile << "0x" << std::hex << data[j] << " ";
            TEXTASSINFO info = { curr_0x2000, curr_0x3000 };
            textass_data.push_back(info);
           }
         else if((data[j] & 0xFF00) == 0x4000)
           {
            curr_0x4000 = (uint16)(data[j] & 0x00FF);
            if(debug) dfile << "0x" << std::hex << data[j] << " ";
           }
        }
     if(debug) dfile << std::dec << std::endl;
    }
 if(debug) dfile << "Number of texture-group associations = " << textass_data.size() << std::endl;
 if(debug) for(size_t i = 0; i < textass_data.size(); i++)  dfile << textass_data[i].material_index << "," << textass_data[i].trilist << std::endl;
 if(debug) dfile << std::endl;

 //
 // PROCESS #05
 // READ SURFACE INFORMATION PART 3
 //

 if(fileinfo[6] != 0)
   {
    // move to surface information
    if(debug) dfile << "SURFACE INFORMATION PART 3" << std::endl;
    ifile.seekg(fileinfo[6]);
    if(ifile.fail()) return error("Seek failure.");
   
    // read data
    uint32 n_items = fileinfo[7];
    if(debug) dfile << "Number of items = " << n_items << std::endl;
    for(uint32 i = 0; i < n_items; i++) {
        for(size_t j = 0; j < 132; j++) if(debug) dfile << setw(4) << (uint16)BE_read_uint08(ifile) << " ";
        if(debug) dfile << std::endl;
       }
    if(debug) dfile << std::endl;
   }

 //
 // PROCESS #06
 // READ MATERIALS
 //

 if(fileinfo[8] != 0)
   {
    // move to surface information
    if(debug) dfile << "MATERIALS" << std::endl;
    ifile.seekg(fileinfo[8]);
    if(ifile.fail()) return error("Seek failure.");
    
    // read data
    uint32 n_items = fileinfo[9];
    if(debug) dfile << "Number of items = " << n_items << std::endl;

    for(uint32 i = 0; i < n_items; i++)
       {
        // read material
        auto p01 = BE_read_uint32 (ifile); // 0x05
        auto p02 = BE_read_uint32 (ifile); // 0x00
        auto p03 = BE_read_uint32 (ifile); // base texture index
        auto p04 = BE_read_real32(ifile); // crap
        auto p05 = BE_read_real32(ifile); // crap
        auto p06 = BE_read_real32(ifile); // crap
        auto p07 = BE_read_real32(ifile); // crap
        auto p08 = BE_read_uint32 (ifile); // 0xFFFFFFFF
        auto p09 = BE_read_uint32 (ifile); // 0xFFFFFFFF
        auto p10 = BE_read_real32(ifile); // crap
        auto p11 = BE_read_real32(ifile); // crap
        auto p12 = BE_read_real32(ifile); // crap
        auto p13 = BE_read_real32(ifile); // crap
        auto p14 = BE_read_uint32 (ifile); // 0xFFFFFFFF
        auto p15 = BE_read_uint32 (ifile); // 0xFFFFFFFF
        auto p16 = BE_read_real32(ifile); // crap
        auto p17 = BE_read_real32(ifile); // crap
        auto p18 = BE_read_real32(ifile); // crap
        auto p19 = BE_read_real32(ifile); // std::endl of 3 4xfloats
        auto p20 = BE_read_uint32 (ifile); // 0x03
        auto p21 = BE_read_uint32 (ifile); // normal map index
        auto p22 = BE_read_real32(ifile); // crap
        auto p23 = BE_read_real32(ifile); // crap
        auto p24 = BE_read_real32(ifile); // crap
        auto p25 = BE_read_real32(ifile); // crap
        auto p26 = BE_read_uint32 (ifile); // 0x04
        auto p27 = BE_read_uint32 (ifile); // specular map index
        auto p28 = BE_read_real32(ifile); // crap
        auto p29 = BE_read_real32(ifile); // crap
        auto p30 = BE_read_real32(ifile); // crap
        auto p31 = BE_read_real32(ifile); // crap
        auto p32 = BE_read_uint32 (ifile); // 0x01
        auto p33 = BE_read_uint32 (ifile); // 0x00
        auto p34 = BE_read_uint32 (ifile); // 0x01
        auto p35 = BE_read_uint32 (ifile); // 0x01
        auto p36 = BE_read_uint32 (ifile); // 0x01
        auto p37 = BE_read_real32(ifile); // crap
        auto p38 = BE_read_real32(ifile); // crap
        auto p39 = BE_read_real32(ifile); // crap
        auto p40 = BE_read_real32(ifile); // crap
        auto p41 = BE_read_real32(ifile); // crap
        auto p42 = BE_read_real32(ifile); // crap
        auto p43 = BE_read_real32(ifile); // crap
        auto p44 = BE_read_real32(ifile); // crap
        auto p45 = BE_read_real32(ifile); // crap
        auto p46 = BE_read_real32(ifile); // crap
        auto p47 = BE_read_real32(ifile); // crap
        auto p48 = BE_read_real32(ifile); // crap
        auto p49 = BE_read_real32(ifile); // crap
        auto p50 = BE_read_real32(ifile); // crap
        if(debug) dfile << p01 << " "; // << std::endl;
        if(debug) dfile << p02 << " "; // << std::endl;
        if(debug) dfile << p03 << " "; // << std::endl;
        if(debug) dfile << p08 << " "; // << std::endl;
        if(debug) dfile << p09 << " "; // << std::endl;
        if(debug) dfile << p14 << " "; // << std::endl;
        if(debug) dfile << p15 << " "; // << std::endl;
        if(debug) dfile << p20 << " "; // << std::endl;
        if(debug) dfile << p21 << " "; // << std::endl;
        if(debug) dfile << p26 << " "; // << std::endl;
        if(debug) dfile << p27 << " "; // << std::endl;
        if(debug) dfile << p32 << " "; // << std::endl;
        if(debug) dfile << p33 << " "; // << std::endl;
        if(debug) dfile << p34 << " "; // << std::endl;
        if(debug) dfile << p35 << " "; // << std::endl;
        if(debug) dfile << p36 << " "; // << std::endl;
        if(debug) dfile << std::endl;

        // build material name
        stringstream name;
        name << "material_" << setfill('0') << setw(3) << i;

        // create material
        SMC_MATERIAL mat;
        mat.id = name.str();
        mat.twoside = false;
        mat.basemap = p03;
        mat.normmap = p21;
        mat.specmap = p27;
        mat.tranmap = p03;
        mat.bumpmap = INVALID_TEXTURE_INDEX;
        mat.resmap1 = INVALID_TEXTURE_INDEX;
        mat.resmap2 = INVALID_TEXTURE_INDEX;
        mat.resmap3 = INVALID_TEXTURE_INDEX;
        mat.resmap4 = INVALID_TEXTURE_INDEX;

        // save material
        smc.materials.push_back(mat);
       }
    if(debug) dfile << std::endl;
   }

 //
 // PROCESS #07
 // READ TRIANGLE GROUP DATA
 //

 if(fileinfo[10])
   {
    // move to triangle information
    if(debug) dfile << "TRIANGLE GROUPS (TRIANGLES, POSITION)" << std::endl;
    ifile.seekg(fileinfo[10]);
    if(ifile.fail()) return error("Seek failure.");

    // number of groups
    uint32 n_groups = fileinfo[11];
    if(n_groups == 0) return error("Invalid number of triangle groups.");

    // read triangle information
    if(indextype == FACE_FORMAT_UINT_16) {
       for(uint32 i = 0; i < n_groups; i++) {
           uint32 p1 = BE_read_uint16(ifile); // number of triangles
           BE_read_uint16(ifile);             // always 0
           uint32 p2 = BE_read_uint32(ifile); // triangle offset
           TRILISTINFO info = { p1, p2 };
           trilist_data.push_back(info);
           if(debug) dfile << "g[" << i << "] = (" << p1 << ", " << p2 << ")" << std::endl;
          }
      }
    else if(indextype == FACE_FORMAT_UINT_32) {
       for(uint32 i = 0; i < n_groups; i++) {
           uint32 p1 = BE_read_uint32(ifile); // number of triangles
           uint32 p2 = BE_read_uint32(ifile); // triangle offset
           TRILISTINFO info = { p1, p2 };
           trilist_data.push_back(info);
           if(debug) dfile << "g[" << i << "] = (" << p1 << ", " << p2 << ")" << std::endl;
          }
      }
    if(debug) dfile << std::endl;
   }

 //
 // PROCESS #08
 // READ INDEX BUFFER
 //

 // move to index buffer
 if(debug) dfile << "INDEX BUFFER DATA" << std::endl;
 ifile.seekg(fileinfo[12]);
 if(ifile.fail()) return error("Seek failure.");

 // number of triangles
 uint32 n_triangles = fileinfo[13];
 if(n_triangles == 0) return error("Invalid number of triangles.");

 // process triangle groups
 for(size_t i = 0; i < trilist_data.size(); i++)
    {
     // get texture-material association data
     TEXTASSINFO ai = textass_data[i];
     if(!(ai.material_index < smc.materials.size())) return error("Texture association data is invalid.");
     uint32 ib_material = ai.material_index;

     // surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << i;

     // move to data offset
     uint32 ib_offset = 3*indexsize*trilist_data[i].offset;
     ifile.seekg(fileinfo[12] + ib_offset);
     if(ifile.fail()) return error("Seek failure.");

     // read index buffer
     uint32 ib_tris = trilist_data[i].triangles;
     uint32 ib_elem = ib_tris*3;
     uint32 ib_char = ib_elem*indexsize;
     boost::shared_array<char> ib_data(new char[ib_char]);
     if(indextype == FACE_FORMAT_UINT_16) BE_read_array(ifile, reinterpret_cast<uint16*>(ib_data.get()), ib_elem);
     else BE_read_array(ifile, reinterpret_cast<uint32*>(ib_data.get()), ib_elem);
     if(ifile.fail()) return error("Read failure.");

     // create index buffer
     SMC_IDX_BUFFER ib;
     ib.format = indextype;
     ib.type = FACE_TYPE_TRIANGLES;
     ib.name = ss.str();
     ib.elem = ib_elem;
     ib.data = ib_data;
     ib.tris = ib_tris;
     ib.material = ib_material;

     // save index buffer
     smc.ibuffer.push_back(ib);
    }
 if(debug) dfile << std::endl;

 //
 // PROCESS #09
 // READ VERTEX BUFFER
 //

 // move to vertex buffer
 if(debug) dfile << "VERTEX BUFFER DATA" << std::endl;
 ifile.seekg(fileinfo[15]);
 if(ifile.fail()) return error("Seek failure.");

 // number of vertices
 uint32 n_vertices = fileinfo[14];
 if(n_vertices == 0) return error("Invalid number of vertices.");

 // create vertex buffer
 smc.vbuffer.flags = VERTEX_POSITION | VERTEX_UV;
 smc.vbuffer.name = "vbuffer";
 smc.vbuffer.elem = n_vertices;
 smc.vbuffer.data.reset(new SMC_VERTEX[n_vertices]);
 if(head02 == 0x1DB9A7) {
    smc.vbuffer.flags |= VERTEX_WEIGHTS;
    smc.vbuffer.flags |= VERTEX_WMAPIDX;
   }

 // read vertices
 for(size_t i = 0; i < n_vertices; i++)
    {
     // initialize weights
     for(size_t j = 0; j < 8; j++) {
         smc.vbuffer.data[i].wv[j] = 0;
         smc.vbuffer.data[i].mi[j] = INVALID_VERTEX_WMAP_INDEX;
         smc.vbuffer.data[i].si[j] = INVALID_VERTEX_SKEL_INDEX;
        }

     // 0x1D02AF
     // 0x0C (12 bytes)
     // OK
     if(head02 == 0x1D01AF)
       {
        float v01 = BE_read_real32(ifile); //  4
        float v02 = BE_read_real32(ifile); //  8
        float v03 = BE_read_real32(ifile); // 12
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu = 0.0f;
        smc.vbuffer.data[i].tv = 0.0f;
       }
     // 0x1D01BF
     // 0x0C (12 bytes)
     // OK
     if(head02 == 0x1D01BF)
       {
        float v01 = BE_read_real32(ifile); //  4
        float v02 = BE_read_real32(ifile); //  8
        float v03 = BE_read_real32(ifile); // 12
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu = 0.0f;
        smc.vbuffer.data[i].tv = 0.0f;
       }
     // 0x1D02BF
     // 0x0C (12 bytes)
     // OK
     else if(head02 == 0x1D02BF)
       {
        float v01 = BE_read_real32(ifile); //  4
        float v02 = BE_read_real32(ifile); //  8
        float v03 = BE_read_real32(ifile); // 12
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu = 0.0f;
        smc.vbuffer.data[i].tv = 0.0f;
       }
     // 0x1D80A7
     // 0x18 (24 bytes)
     // OK
     else if(head02 == 0x1D80A7)
       {
        auto v01 = BE_read_real32(ifile); //  4
        auto v02 = BE_read_real32(ifile); //  8
        auto v03 = BE_read_real32(ifile); // 12
        auto v04 = BE_read_sint16(ifile);  // 14
        auto v05 = BE_read_sint16(ifile);  // 16
        auto v06 = BE_read_sint16(ifile);  // 18
        auto v07 = BE_read_sint16(ifile);  // 20
        auto v08 = BE_read_uint32(ifile);  // 24
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << v04 << std::endl;
        if(debug) dfile << v05 << std::endl;
        if(debug) dfile << v06 << std::endl;
        if(debug) dfile << v07 << std::endl;
        if(debug) dfile << v08 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu =  v06/1023.0f;
        smc.vbuffer.data[i].tv = -v07/1023.0f;
       }
     // 0x1D80A7
     // 0x18 (24 bytes)
     // OK
     else if(head02 == 0x1D81A7)
       {
        auto v01 = BE_read_real32(ifile); //  4
        auto v02 = BE_read_real32(ifile); //  8
        auto v03 = BE_read_real32(ifile); // 12
        auto v04 = BE_read_sint16(ifile);  // 14
        auto v05 = BE_read_sint16(ifile);  // 16
        auto v06 = BE_read_sint16(ifile);  // 18
        auto v07 = BE_read_sint16(ifile);  // 20
        auto v08 = BE_read_uint32(ifile);  // 24
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << v04 << std::endl;
        if(debug) dfile << v05 << std::endl;
        if(debug) dfile << v06 << std::endl;
        if(debug) dfile << v07 << std::endl;
        if(debug) dfile << v08 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz = -v03;
        smc.vbuffer.data[i].tu =  v06/1023.0f;
        smc.vbuffer.data[i].tv = -v07/1023.0f;
       }
     // 0x1D80AF
     // 0x1C (28 bytes)
     // OK
     else if(head02 == 0x1D80AF)
       {
        auto v01 = BE_read_real32(ifile); //  4
        auto v02 = BE_read_real32(ifile); //  8
        auto v03 = BE_read_real32(ifile); // 12
        auto v04 = BE_read_sint16(ifile);  // 14
        auto v05 = BE_read_sint16(ifile);  // 16
        auto v06 = BE_read_sint16(ifile);  // 18
        auto v07 = BE_read_sint16(ifile);  // 20
        auto v08 = BE_read_uint32(ifile);  // 24
        auto v09 = BE_read_uint32(ifile);  // 28
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << v04 << std::endl;
        if(debug) dfile << v05 << std::endl;
        if(debug) dfile << v06 << std::endl;
        if(debug) dfile << v07 << std::endl;
        if(debug) dfile << v08 << std::endl;
        if(debug) dfile << v09 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu =  v06/1023.0f;
        smc.vbuffer.data[i].tv = -v07/1023.0f;
       }
     // 0x1D81AF
     // 0x1C (28 bytes)
     // OK
     else if(head02 == 0x1D81AF)
       {
        auto v01 = BE_read_real32(ifile); //  4
        auto v02 = BE_read_real32(ifile); //  8
        auto v03 = BE_read_real32(ifile); // 12
        auto v04 = BE_read_sint16(ifile);  // 14
        auto v05 = BE_read_sint16(ifile);  // 16
        auto v06 = BE_read_sint16(ifile);  // 18
        auto v07 = BE_read_sint16(ifile);  // 20
        auto v08 = BE_read_uint32(ifile);  // 24
        auto v09 = BE_read_uint32(ifile);  // 28
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << v04 << std::endl;
        if(debug) dfile << v05 << std::endl;
        if(debug) dfile << v06 << std::endl;
        if(debug) dfile << v07 << std::endl;
        if(debug) dfile << v08 << std::endl;
        if(debug) dfile << v09 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu =  v06/1023.0f;
        smc.vbuffer.data[i].tv = -v07/1023.0f;
       }
     // 0x1D82A7
     // 0x18 (24 bytes)
     // OK
     else if(head02 == 0x1D82A7)
       {
        auto v01 = BE_read_real32(ifile); //  4
        auto v02 = BE_read_real32(ifile); //  8
        auto v03 = BE_read_real32(ifile); // 12
        auto v04 = BE_read_sint16(ifile);  // 14
        auto v05 = BE_read_sint16(ifile);  // 16
        auto v06 = BE_read_sint16(ifile);  // 18
        auto v07 = BE_read_sint16(ifile);  // 20
        auto v08 = BE_read_uint32(ifile);  // 24
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << v04 << std::endl;
        if(debug) dfile << v05 << std::endl;
        if(debug) dfile << v06 << std::endl;
        if(debug) dfile << v07 << std::endl;
        if(debug) dfile << v08 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu =  v06/1023.0f;
        smc.vbuffer.data[i].tv = -v07/1023.0f;
       }
     // 0x1D82AF
     // 0x1C (28 bytes)
     // OK
     else if(head02 == 0x1D82AF)
       {
        auto v01 = BE_read_real32(ifile); //  4
        auto v02 = BE_read_real32(ifile); //  8
        auto v03 = BE_read_real32(ifile); // 12
        auto v04 = BE_read_sint16(ifile);  // 14
        auto v05 = BE_read_sint16(ifile);  // 16
        auto v06 = BE_read_sint16(ifile);  // 18
        auto v07 = BE_read_sint16(ifile);  // 20
        auto v08 = BE_read_uint32(ifile);  // 24
        auto v09 = BE_read_uint32(ifile);  // 28
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << v04 << std::endl;
        if(debug) dfile << v05 << std::endl;
        if(debug) dfile << v06 << std::endl;
        if(debug) dfile << v07 << std::endl;
        if(debug) dfile << v08 << std::endl;
        if(debug) dfile << v09 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu =  v06/1023.0f;
        smc.vbuffer.data[i].tv = -v07/1023.0f;
       }
     // 0x1DB8A7
     // 0x20 (32 bytes)
     // OK
     else if(head02 == 0x1DB8A7)
       {
        auto v01 = BE_read_real32(ifile); //  4
        auto v02 = BE_read_real32(ifile); //  8
        auto v03 = BE_read_real32(ifile); // 12
        auto v04 = BE_read_sint16(ifile);  // 14
        auto v05 = BE_read_sint16(ifile);  // 16
        auto v06 = BE_read_sint16(ifile);  // 18
        auto v07 = BE_read_sint16(ifile);  // 20
        auto v08 = BE_read_uint32(ifile);  // 24
        auto v09 = BE_read_uint16(ifile);  // 26
        auto v10 = BE_read_uint16(ifile);  // 28
        auto v11 = BE_read_uint16(ifile);  // 30
        auto v12 = BE_read_uint16(ifile);  // 32
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << v04 << std::endl;
        if(debug) dfile << v05 << std::endl;
        if(debug) dfile << v06 << std::endl;
        if(debug) dfile << v07 << std::endl;
        if(debug) dfile << v08 << std::endl;
        if(debug) dfile << v09 << std::endl;
        if(debug) dfile << v10 << std::endl;
        if(debug) dfile << v11 << std::endl;
        if(debug) dfile << v12 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu =  v06/1023.0f;
        smc.vbuffer.data[i].tv = -v07/1023.0f;
       }
     // 0x1DB8AF
     // 0x24 (36 bytes)
     // OK
     else if(head02 == 0x1DB8AF)
       {
        auto v01 = BE_read_real32(ifile); //  4
        auto v02 = BE_read_real32(ifile); //  8
        auto v03 = BE_read_real32(ifile); // 12
        auto v04 = BE_read_sint16(ifile);  // 14
        auto v05 = BE_read_sint16(ifile);  // 16
        auto v06 = BE_read_sint16(ifile);  // 18
        auto v07 = BE_read_sint16(ifile);  // 20
        auto v08 = BE_read_uint16(ifile);  // 22
        auto v09 = BE_read_uint16(ifile);  // 24
        auto v10 = BE_read_uint32(ifile);  // 28
        auto v11 = BE_read_uint32(ifile);  // 32
        auto v12 = BE_read_uint32(ifile);  // 36
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << v04 << std::endl;
        if(debug) dfile << v05 << std::endl;
        if(debug) dfile << v06 << std::endl;
        if(debug) dfile << v07 << std::endl;
        if(debug) dfile << v08 << std::endl;
        if(debug) dfile << v09 << std::endl;
        if(debug) dfile << v10 << std::endl;
        if(debug) dfile << v11 << std::endl;
        if(debug) dfile << v12 << std::endl;
        if(debug) dfile << std::endl;
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;
        smc.vbuffer.data[i].tu =  v06/1023.0f;
        smc.vbuffer.data[i].tv = -v07/1023.0f;
       }
     // 0x1DB9A7
     // 0x20 (32 bytes)
     // OK
     else if(head02 == 0x1DB9A7)
       {
        auto v01 = BE_read_real32(ifile); //  4
        auto v02 = BE_read_real32(ifile); //  8
        auto v03 = BE_read_real32(ifile); // 12
        auto v04 = BE_read_uint08(ifile);  // 14
        auto v05 = BE_read_uint08(ifile);  // 15
        auto v06 = BE_read_uint08(ifile);  // 16
        auto v07 = BE_read_uint08(ifile);  // 17
        auto v08 = BE_read_sint16(ifile);  // 18
        auto v09 = BE_read_sint16(ifile);  // 20
        auto v10 = BE_read_uint32(ifile);  // 24
        auto v11 = BE_read_uint08(ifile);  // 25
        auto v12 = BE_read_uint08(ifile);  // 26
        auto v13 = BE_read_uint08(ifile);  // 27
        auto v14 = BE_read_uint08(ifile);  // 28
        auto v15 = BE_read_uint08(ifile);  // 29
        auto v16 = BE_read_uint08(ifile);  // 30
        auto v17 = BE_read_uint08(ifile);  // 31
        auto v18 = BE_read_uint08(ifile);  // 32
        if(debug) dfile << "vertex " << i << std::endl;
        if(debug) dfile << v01 << std::endl;
        if(debug) dfile << v02 << std::endl;
        if(debug) dfile << v03 << std::endl;
        if(debug) dfile << (uint16)v04 << std::endl;
        if(debug) dfile << (uint16)v05 << std::endl;
        if(debug) dfile << (uint16)v06 << std::endl;
        if(debug) dfile << (uint16)v07 << std::endl;
        if(debug) dfile << v08 << std::endl;
        if(debug) dfile << v09 << std::endl;
        if(debug) dfile << v10 << std::endl;
        if(debug) dfile << (uint16)v11 << std::endl;
        if(debug) dfile << (uint16)v12 << std::endl;
        if(debug) dfile << (uint16)v13 << std::endl;
        if(debug) dfile << (uint16)v14 << std::endl;
        if(debug) dfile << (uint16)v15 << std::endl;
        if(debug) dfile << (uint16)v16 << std::endl;
        if(debug) dfile << (uint16)v17 << std::endl;
        if(debug) dfile << (uint16)v18 << std::endl;
        if(debug) dfile << std::endl;

        // set position
        smc.vbuffer.data[i].vx =  v01;
        smc.vbuffer.data[i].vy =  v02;
        smc.vbuffer.data[i].vz =  v03;

        // set UV
        smc.vbuffer.data[i].tu =  v08/1023.0f;
        smc.vbuffer.data[i].tv = -v09/1023.0f;

        // set weights
        if(!(v11 == 0 && v15 == 0)) {
           smc.vbuffer.data[i].wv[0] = ((((uint16)v11) << 8) | v11);
           smc.vbuffer.data[i].mi[0] = v15;
          }
        if(!(v12 == 0 && v16 == 0)) {
           smc.vbuffer.data[i].wv[1] = ((((uint16)v12) << 8) | v12);
           smc.vbuffer.data[i].mi[1] = v16;
          }
        if(!(v13 == 0 && v17 == 0)) {
           smc.vbuffer.data[i].wv[2] = ((((uint16)v13) << 8) | v13);
           smc.vbuffer.data[i].mi[2] = v17;
          }
        if(!(v14 == 0 && v18 == 0)) {
           smc.vbuffer.data[i].wv[3] = ((((uint16)v14) << 8) | v14);
           smc.vbuffer.data[i].mi[3] = v18;
          }
       }
    }
 if(debug) dfile << std::endl;

 //
 // PROCESS #10
 // READ SEPARATE UV DATA
 //

 // move to offset
 if(debug) dfile << "SEPARATE UV DATA" << std::endl;
 ifile.seekg(fileinfo[17]);
 if(ifile.fail()) return error("Seek failure.");
 if(head02 == 0x1D01AF || head02 == 0x1D01BF || head02 == 0x1D02BF) {
    for(size_t i = 0; i < n_vertices; i++) {
        auto v1 = BE_read_sint16(ifile);
        auto v2 = BE_read_sint16(ifile);
        if(debug) dfile << "v1 = " << v1 << std::endl;
        if(debug) dfile << "v2 = " << v2 << std::endl;
        smc.vbuffer.data[i].tu =  v1/1023.0f;
        smc.vbuffer.data[i].tv = -v2/1023.0f;
       }
   }
 if(debug) dfile << std::endl;

 //
 // PROCESS #11
 // READ BONE MATRICES
 //

 // process bones
 if(fileinfo[23])
   {
    // move to bone matrices
    if(debug) dfile << "BONE MATRICES" << std::endl;
    ifile.seekg(fileinfo[23]);
    if(ifile.fail()) return error("Seek failure.");

    // data to save
    std::deque<boost::shared_array<float>> mlist;
    SKELETON skeleton;
    skeleton.set_identifier("skeleton");

    // read matrices
    uint32 n_bones = fileinfo[24];
    if(debug) dfile << "number of bones = " << n_bones << std::endl;
    for(size_t i = 0; i < n_bones; i++) {
        boost::shared_array<float> m(new float[16]);
        m[ 0] = BE_read_real32(ifile);
        m[ 1] = BE_read_real32(ifile);
        m[ 2] = BE_read_real32(ifile);
        m[ 3] = BE_read_real32(ifile);
        m[ 4] = BE_read_real32(ifile);
        m[ 5] = BE_read_real32(ifile);
        m[ 6] = BE_read_real32(ifile);
        m[ 7] = BE_read_real32(ifile);
        m[ 8] = BE_read_real32(ifile);
        m[ 9] = BE_read_real32(ifile);
        m[10] = BE_read_real32(ifile);
        m[11] = BE_read_real32(ifile);
        m[12] = BE_read_real32(ifile);
        m[13] = BE_read_real32(ifile);
        m[14] = BE_read_real32(ifile);
        m[15] = BE_read_real32(ifile);
        mlist.push_back(m);
        if(debug) dfile << m[ 0] << " " << m[ 1] << " " << m[ 2] << " " << m[ 3] << std::endl;
        if(debug) dfile << m[ 4] << " " << m[ 5] << " " << m[ 6] << " " << m[ 7] << std::endl;
        if(debug) dfile << m[ 8] << " " << m[ 9] << " " << m[10] << " " << m[11] << std::endl;
        if(debug) dfile << m[12] << " " << m[13] << " " << m[14] << " " << m[15] << std::endl;
        if(debug) dfile << std::endl;
       }

    // move to bone information
    if(debug) dfile << "BONES" << std::endl;
    ifile.seekg(fileinfo[25]);
    if(ifile.fail()) return error("Seek failure.");
    if(n_bones != fileinfo[26]) return error("Number of joints do not match.");

    // read bone information
    for(size_t i = 0; i < n_bones; i++)
       {
        // joint
        JOINT joint;
        uint32 index = (uint32)i;
   
        // joint name
        char name[5];
        ifile.read(&name[0], 4);
        std::reverse(&name[0], &name[4]);
        name[4] = '\0';

        // make joint name unique
        std::stringstream jnss;
        jnss << name << "_" << std::setfill('0') << std::setw(3) << i;
        joint.name = jnss.str();
   
        // joint matrix
        joint.matrix = mlist[i];

        // joint relative positions
        float x = BE_read_real32(ifile);
        float y = BE_read_real32(ifile);
        float z = BE_read_real32(ifile);
        joint.rel_x = x;
        joint.rel_y = y;
        joint.rel_z = z;

        // set parent
        uint32 parent = BE_read_uint32(ifile);
        uint32 xxxxxx = BE_read_uint32(ifile);
        joint.parent = parent;
   
        // insert joint
        skeleton.insert(index, joint);

        if(debug) dfile << "index = " << index << std::endl;   
        if(debug) dfile << "name = " << name << std::endl;   
        if(debug) dfile << "x = " << x << std::endl;
        if(debug) dfile << "y = " << y << std::endl;
        if(debug) dfile << "z = " << z << std::endl;
        if(debug) dfile << "parent = " << parent << std::endl;
        if(debug) dfile << "xxxxxx = " << xxxxxx << std::endl;
        if(debug) dfile << std::endl;
       }

    // print joint tree in debug mode
    if(debug) {
       stringstream ss;
       skeleton.PrintJointTree(ss);
       dfile << ss.str() << std::endl;
      }

    // create skeleton
    smc.skeletons.push_back(skeleton);
   }

 //
 // PROCESS #12
 // READ TEXTURES
 //

 if(fileinfo[27])
   {
    // move to texture information
    if(debug) dfile << "TEXTURES" << std::endl;
    ifile.seekg(fileinfo[27]);
    if(ifile.fail()) return error("Seek failure.");
   
    // read texture information
    uint32 n_textures = fileinfo[28];
    for(uint32 i = 0; i < n_textures; i++)
       {
        uint32 param1 = BE_read_uint32(ifile); // index
        uint32 param2 = BE_read_uint32(ifile); // 0
        uint16 param3 = BE_read_uint16(ifile); // 0x0001
        uint16 param4 = BE_read_uint16(ifile); // 0x0200 | 0x0400
        uint16 param5 = BE_read_uint16(ifile); // 0x0200 | 0x0400
        uint16 param6 = BE_read_uint16(ifile); // 0x2000 | 0xFF00
        if(debug) dfile << "param1 = " << param1 << std::endl;
        if(debug) dfile << "param2 = " << param2 << std::endl;
        if(debug) dfile << "param3 = " << param3 << std::endl;
        if(debug) dfile << "param4 = " << param4 << std::endl;
        if(debug) dfile << "param5 = " << param5 << std::endl;
        if(debug) dfile << "param6 = " << param6 << std::endl;
        if(debug) dfile << std::endl;
   
        // texture id
        stringstream id;
        id << "texture_" << setfill('0') << setw(3) << param1;
   
        // texture filename
        stringstream fn;
        fn << setfill('0') << setw(3) << param1 << ".dds.tga";
   
        // insert texture
        SMC_TEXTURE item = { id.str(), fn.str() };
        smc.textures.push_back(item);
       }
    if(debug) dfile << std::endl;
   }

 // replace texture names
 string texfile = GetTEXFromMDL(filename);
 if(texfile.length()) {
    std::deque<string> filelist;
    GetFilenamesFromTEX(texfile, filelist);
    if(filelist.size() == smc.textures.size()) {
       for(size_t i = 0; i < smc.textures.size(); i++) {
           string shortname = GetShortFilename(filelist[i]);
           shortname += ".tga";
           smc.textures[i].filename = shortname;
          }
      }
    else {
       stringstream ss;
       ss << "The number of textures do not match. ";
       ss << "Expecting " << smc.textures.size() << " but obtained " << filelist.size() << "."; 
       error(ss.str().c_str());
      }
   }

 // save geometry
 return SaveLWO(fn_path.c_str(), fn_name.c_str(), smc);
}

bool extractor::processTMO(const char* filename)
{
 std::ifstream ifile(filename, std::ios::binary);
 if(!ifile) return error("");

 uint32 head01 = BE_read_uint32(ifile);
 uint32 head02 = BE_read_uint32(ifile);
 uint32 head03 = BE_read_uint32(ifile);
 uint32 head04 = BE_read_uint32(ifile);
 uint32 head05 = BE_read_uint32(ifile);
 uint32 head06 = BE_read_uint32(ifile); // number of bones
 uint32 head07 = BE_read_uint32(ifile);
 uint32 head08 = BE_read_uint32(ifile); // number of bones
 uint32 head09 = BE_read_uint32(ifile);
 uint32 head10 = BE_read_uint32(ifile);
 uint32 head11 = BE_read_uint32(ifile);
 uint32 head12 = BE_read_uint32(ifile);
 uint32 head13 = BE_read_uint32(ifile);
 uint32 head14 = BE_read_uint32(ifile);
 uint32 head15 = BE_read_uint32(ifile);
 uint32 head16 = BE_read_uint32(ifile);
 std::cout << head01 << std::endl;
 std::cout << head02 << std::endl;
 std::cout << head03 << std::endl;
 std::cout << head04 << std::endl;
 std::cout << head05 << std::endl;
 std::cout << head06 << std::endl;
 std::cout << head07 << std::endl;
 std::cout << head08 << std::endl;
 std::cout << head09 << std::endl;
 std::cout << head10 << std::endl;
 std::cout << head11 << std::endl;
 std::cout << head12 << std::endl;
 std::cout << head13 << std::endl;
 std::cout << head14 << std::endl;
 std::cout << head15 << std::endl;
 std::cout << head16 << std::endl;

 // read bone information
 uint32 n_bones = head06;
 for(size_t i = 0; i < n_bones; i++)
    {
     // joint name
     char name[5];
     ifile.read(&name[0], 4);
     std::reverse(&name[0], &name[4]);
     name[4] = '\0';

     // joint relative positions
     float x = BE_read_real32(ifile);
     float y = BE_read_real32(ifile);
     float z = BE_read_real32(ifile);

     // set parent
     uint32 parent = BE_read_uint32(ifile);
     uint32 xxxxxx = BE_read_uint32(ifile);

     std::cout << "index = " << i << std::endl;   
     std::cout << "name = " << name << std::endl;   
     std::cout << "x = " << x << std::endl;
     std::cout << "y = " << y << std::endl;
     std::cout << "z = " << z << std::endl;
     std::cout << "parent = " << parent << std::endl;
     std::cout << "xxxxxx = " << xxxxxx << std::endl;
     std::cout << std::endl;
    }

 ifile.seekg(head05);
 uint16 sect01 = BE_read_uint16(ifile);
 uint16 sect02 = BE_read_uint16(ifile);
 uint32 sect03 = BE_read_uint32(ifile);
 uint32 sect04 = BE_read_uint32(ifile);
 uint32 sect05 = BE_read_uint32(ifile);
 std::cout << "sect01 = " << sect01 << std::endl;
 std::cout << "sect02 = " << sect02 << std::endl;
 std::cout << "sect03 = " << sect03 << std::endl;
 std::cout << "sect04 = " << sect04 << std::endl;
 std::cout << "sect05 = " << sect05 << std::endl;

 for(size_t i = 0; i < sect02; i++)
    {
     std::cout << "i = " << i << std::endl;
     for(size_t j = 0; j < 27; j++) {
         uint32 temp = BE_read_uint32(ifile);
         std::cout << temp << std::endl;
        }
     std::cout << std::endl;
    }

 ifile.seekg(head14);
 uint32 param1 = BE_read_uint32(ifile);
 uint32 param2 = BE_read_uint32(ifile);
 std::cout << "param1 = " << param1 << std::endl;
 std::cout << "param2 = " << param2 << std::endl;

 for(size_t i = 0; i < sect01; i++)
    {
     std::cout << "i = " << i << std::endl;
     auto p1 = BE_read_uint16(ifile);
     auto p2 = BE_read_uint16(ifile);
     auto p3 = BE_read_uint16(ifile);
     auto p4 = BE_read_uint16(ifile);
     std::cout << "p1 = " << p1 << std::endl;
     std::cout << "p2 = " << p2 << std::endl;
     std::cout << "p3 = " << p3 << std::endl;
     std::cout << "p4 = " << p4 << std::endl;
     std::cout << std::endl;
    }

 float fp01 = BE_read_real32(ifile);
 float fp02 = BE_read_real32(ifile);
 float fp03 = BE_read_real32(ifile);
 float fp04 = BE_read_real32(ifile);
 float fp05 = BE_read_real32(ifile);
 float fp06 = BE_read_real32(ifile);
 float fp07 = BE_read_real32(ifile);
 float fp08 = BE_read_real32(ifile);
 float fp09 = BE_read_real32(ifile);
 float fp10 = BE_read_real32(ifile);
 float fp11 = BE_read_real32(ifile);
 float fp12 = BE_read_real32(ifile);
 std::cout << "fp01 = " << fp01 << std::endl;
 std::cout << "fp02 = " << fp02 << std::endl;
 std::cout << "fp03 = " << fp03 << std::endl;
 std::cout << "fp04 = " << fp04 << std::endl;
 std::cout << "fp05 = " << fp05 << std::endl;
 std::cout << "fp06 = " << fp06 << std::endl;
 std::cout << "fp07 = " << fp07 << std::endl;
 std::cout << "fp08 = " << fp08 << std::endl;
 std::cout << "fp09 = " << fp09 << std::endl;
 std::cout << "fp10 = " << fp10 << std::endl;
 std::cout << "fp11 = " << fp11 << std::endl;
 std::cout << "fp12 = " << fp12 << std::endl;
/*
 for(;;)
    {
     for(size_t i = 0; i < sect01; i++)
        {
         std::cout << "i = " << i << std::endl;
         auto p1 = BE_read_uint16(ifile);
         auto p2 = BE_read_uint16(ifile);
         auto p3 = BE_read_uint16(ifile);
         auto p4 = BE_read_uint16(ifile);
         std::cout << "p1 = " << p1 << std::endl;
         std::cout << "p2 = " << p2 << std::endl;
         std::cout << "p3 = " << p3 << std::endl;
         std::cout << "p4 = " << p4 << std::endl;
         std::cout << std::endl;
        }
    }
*/
 return true;
}

std::string GetTEXFromMDL(const std::string& filename)
{
 using namespace std;
 if(!filename.length()) return string();

 string path = GetPathnameFromFilename(filename);
 string name = GetShortFilenameWithoutExtension(filename);
 string name_with_ext = GetShortFilename(filename);

 // METHOD #1
 // SAME PATH - SAME NAME
 string test1 = path;
 test1 += name;
 test1 += ".tex";
 if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;

 // METHOD #2
 // SAME PATH - DIFFERENT NAME
 if(name == "000" || name == "001" || name == "002" || name == "003" || name == "004") {
    string test0 = path; test0 += "000.tex";
    string test1 = path; test1 += "001.tex";
    string test2 = path; test2 += "002.tex";
    string test3 = path; test3 += "003.tex";
    string test4 = path; test4 += "004.tex";
    if(GetFileAttributesA(test0.c_str()) != INVALID_FILE_ATTRIBUTES) return test0;
    if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
    if(GetFileAttributesA(test2.c_str()) != INVALID_FILE_ATTRIBUTES) return test2;
    if(GetFileAttributesA(test3.c_str()) != INVALID_FILE_ATTRIBUTES) return test3;
    if(GetFileAttributesA(test4.c_str()) != INVALID_FILE_ATTRIBUTES) return test4;
   } 

 // METHOD #3
 // REPLACE _mdl.mdl WITH _tex.tex
 size_t position = name_with_ext.find("_mdl.mdl");
 if(position != string::npos) {
    string test1 = path;
    string test2 = name_with_ext.replace(position, 8, "_tex.tex");
    test1 += test2;
    if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
   }

 // METHOD #4
 // REPLACE _mdl.mdl WITH _tex_color1.tex
 if(position != string::npos) {
    string test1 = path;
    string test2 = name_with_ext.substr(0, position);
    test1 += test2;
    test1 += "_tex_color1.tex";
    if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
   }

 // METHOD #5
 // REPLACE _mdl.mdl WITH _tex_democolor.tex
 if(position != string::npos) {
    string test1 = path;
    string test2 = name_with_ext.substr(0, position);
    test1 += test2;
    test1 += "_tex_democolor.tex";
    if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
   }

 // METHOD #6a
 // REPLACE EVERYTHING AFTER _tmd WITH _color1_xpr\\000\\xxx.tex
 position = path.find("_tmd\\000\\");
 if(position != string::npos) {
    string test1 = path.substr(0, position);
    test1 += "_color1_xpr\\000\\";
    if(name == "000") test1 += "000.tex";
    else if(name == "001") test1 += "001.tex";
    else if(name == "002") test1 += "002.tex";
    if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
   }

 // METHOD #6b
 // REPLACE EVERYTHING AFTER _tmd WITH _color1_xpr\\001\\xxx.tex
 position = path.find("_tmd\\001\\");
 if(position != string::npos) {
    string test1 = path.substr(0, position);
    test1 += "_color1_xpr\\001\\";
    if(name == "000") test1 += "000.tex";
    else if(name == "001") test1 += "001.tex";
    else if(name == "002") test1 += "002.tex";
    if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
   }

 // METHOD #6c
 // REPLACE EVERYTHING AFTER _tmd WITH _color1_xpr\\000\\xxx.tex
 position = path.find("_tmd\\");
 if(position != string::npos) {
    string test1 = path.substr(0, position);
    test1 += "_color1_xpr\\000\\";
    if(name == "000") test1 += "000.tex";
    else if(name == "001") test1 += "001.tex";
    else if(name == "002") test1 += "002.tex";
    if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
   }

 // METHOD #6d
 // REPLACE EVERYTHING AFTER _tmd WITH _color1_xpr\\000\\000.tex
 position = path.find("_tmd\\");
 if(position != string::npos) {
    string test1 = path.substr(0, position);
    test1 += "_color1_xpr\\000\\000.tex";
    if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
   }

 // METHOD #7
 // REPLACE SFX\\SFX\\XXX.MDL WITH SFX\\XXX.TEX
 position = path.find("sfx\\sfx\\");
 if(position != string::npos) {
    string test1 = path.substr(0, position);
    test1 += "sfx\\";
    test1 += name;
    test1 += ".tex";
    if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
   }

 // METHOD #8
 // REPLACE field\\fieldbg WITH field\\field_xbg
 position = path.find("field\\fieldbg");
 if(position != string::npos)
   {
    // build path part 1
    string test1 = path.substr(0, position);
    test1 += "field\\field_xbg";

    // build path part 2
    string test2 = path.substr(position + 13);
    size_t strip_position = test2.find("bf_fieldbg");
    if(strip_position != string::npos) {
       string test3 = test2.substr(0, strip_position);
       test1 += test3;
       if(name == "000") test1 += "000.tex";
       else if(name == "001") test1 += "001.tex";
       else if(name == "002") test1 += "002.tex";
       else if(name == "003") test1 += "003.tex";
       if(GetFileAttributesA(test1.c_str()) != INVALID_FILE_ATTRIBUTES) return test1;
      }
   }

 // METHOD #8
 // REPLACE field\\fieldbg WITH field\\field_xbg
 position = path.find("field\\fieldbg");
 if(position != string::npos)
   {
    // build path part 1
    string test1 = path.substr(0, position);
    test1 += "field\\field_xbg";

    // build path part 2
    string test2 = path.substr(position + 13);
    test1 += test2;

    // strip std::endling
    size_t strip_position = test1.find("_tmd\\");
    if(strip_position != string::npos) {
       string test3 = test1.substr(0, strip_position);
       test3 += "_xpr\\000.tex";
       if(GetFileAttributesA(test3.c_str()) != INVALID_FILE_ATTRIBUTES) return test3;
      }
   }

 return string();
}

bool GetFilenamesFromTEX(const std::string& texfile, std::deque<std::string>& filenames)
{
 // open texture file
 using namespace std;
 std::ifstream ifile(texfile, ios::binary);
 if(!ifile) return error("Failed to open TEX file; model will have to be manually textured.");

 // read number of textures
 ifile.seekg(0xC);
 if(ifile.fail()) return error("Seek failure; model will have to be manually textured.");

 // read number of textures
 uint32 n_textures = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure; model will have to be manually textured.");

 if(n_textures < 1 || n_textures > 1000)
    return error("Unexpected number of textures; model will have to be manually textured.");

 // move to filename offset
 ifile.seekg(16*n_textures + 20);
 if(ifile.fail()) return error("Seek failure; model will have to be manually textured.");

 // read filenames
 for(size_t i = 0; i < n_textures; i++) {
     char buffer[1024];
     read_string(ifile, buffer, 1024);
     if(ifile.fail()) return error("Read filename failure; model will have to be manually textured.");
     filenames.push_back(string(buffer));
    }

 if(filenames.size() == 0) return error("Filename search yielded no textures;  model will have to be manually textured.");
 return true;
}

};};

namespace XB360 { namespace Onechanbara2 {

bool extract(void)
{
 std::string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pathname)
{
 return extractor(pathname).extract();
}

};};