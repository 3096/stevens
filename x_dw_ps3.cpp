#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_dw_xb360.h"
#include "x_dw_ps3.h"

#define X_SYSTEM PS3
#define X_GAME   DYNASTY_WARRIORS

//
// UTILITY FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool DWGetFileExtension(LPCTSTR filename, LPTSTR extension)
{
 return XB360::DYNASTY_WARRIORS::DWGetFileExtension(filename, extension);
}

bool DWRenameIDXOUT(LPCTSTR filename)
{
 return XB360::DYNASTY_WARRIORS::DWRenameIDXOUT(filename);
}

bool DWUnpackIDXZRC_TypeA(LPCTSTR filename)
{
 return XB360::DYNASTY_WARRIORS::DWUnpackIDXZRC_TypeA(filename);
}

bool DWUnpackIDXZRC_TypeB(LPCTSTR filename)
{
 return XB360::DYNASTY_WARRIORS::DWUnpackIDXZRC_TypeB(filename);
}

}};

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool UnpackANS(LPCTSTR filename)
{
 // exact same algorithm
 return XB360::DYNASTY_WARRIORS::UnpackANS(filename);
}

bool UnpackABC(LPCTSTR filename)
{
 return XB360::DYNASTY_WARRIORS::UnpackABC(filename);
}

bool UnpackABC_Type2(LPCTSTR filename)
{
 return XB360::DYNASTY_WARRIORS::UnpackABC_Type2(filename);
}

bool UnpackIDX(LPCTSTR filename)
{
 // exact same algorithm
 return XB360::DYNASTY_WARRIORS::UnpackIDX(filename);
}

// archive file unpacking functions
bool DWUnpackArc1(LPCTSTR filename) { return  XB360::DYNASTY_WARRIORS::DWUnpackArc1(filename); }
bool DWUnpackArc2(LPCTSTR filename) { return  XB360::DYNASTY_WARRIORS::DWUnpackArc2(filename); }
bool DWUnpackArc3(LPCTSTR filename) { return  XB360::DYNASTY_WARRIORS::DWUnpackArc3(filename); }
bool DWUnpackArc4(LPCTSTR filename) { return  XB360::DYNASTY_WARRIORS::DWUnpackArc4(filename); }
bool DWUnpackArc5(LPCTSTR filename) { return  XB360::DYNASTY_WARRIORS::DWUnpackArc5(filename); }
bool DWUnpackArc6(LPCTSTR filename) { return  XB360::DYNASTY_WARRIORS::DWUnpackArc6(filename); }
bool DWUnpackMDLK(LPCTSTR filename) { return  XB360::DYNASTY_WARRIORS::DWUnpackMDLK(filename); }

bool UnpackDAT_TypeA(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = BE_read_uint32(ifile); // unknown
 uint32 h02 = BE_read_uint32(ifile); // unknown (not always = number of files)
 uint32 h03 = BE_read_uint32(ifile); // 0x00
 uint32 h04 = BE_read_uint32(ifile); // 0x00
 if(ifile.fail()) return error("Read failure.");

 // read offsets
 typedef pair<uint32,uint32> pair_t;
 deque<pair_t> offsets;
 for(;;) {
     uint32 value = BE_read_uint32(ifile);
     if(ifile.fail()) return false;
     if(!value) break;
     offsets.push_back(pair_t(value, 0xFFFFFFFF));
    }

 // skip past 00's
 uint32 position = (uint32)ifile.tellg();
 for(;;) {
     uint32 value = BE_read_uint32(ifile);
     if(ifile.fail()) return false;
     if(value) break;
     position += 4;
    }

 // move to sizes
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");

 // read sizes
 for(uint32 i = 0; i < offsets.size(); i++) {
     uint32 value = BE_read_uint32(ifile);
     if(ifile.fail()) return false;
     offsets[i].second = value;
    }

 // create save directory
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname << TEXT("\\");
 STDSTRING savepath = ss.str();
 CreateDirectory(savepath.c_str(), NULL);

 // for each offset
 for(uint32 i = 0; i < offsets.size(); i++)
    {
     // skip based on offset
     uint32 offset = offsets[i].first;
     if(!offset) continue; // invalid offset

     // skip based on datasize
     uint32 datasize = offsets[i].second;
     if(datasize < 0x80) continue; // nothing important

     // move to offset
     ifile.seekg(offset);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);
     if(ifile.fail()) return error("Read failure.");

     // create BIN file
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(4) << i << "." << "idxout";
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create bin file.");

     // save data
     ofile.write(data.get(), datasize);
     if(ofile.fail()) return error("Write failure.");
    }

 // success
 return true;
}

//
// PS3 DLC FORMAT TYPE B
// SENGOKU MUSOU 4 (2014)
//
bool UnpackDAT_TypeB(LPCTSTR filename)
{
 // HEADER EXAMPLE
 //  01059401 -- magic
 //  00000003 -- number of entries
 //  00000100 -- offset scale
 //  00000000 -- ???
 // ENTRY TABLE EXAMPLE
 //  00000000 00000001 00000008 000000C9 0005621D FFFFFFFF
 //  00000000 00000564 00000009 000000C9 001D937C FFFFFFFF
 //  00000000 000022F8 00000020 00000066 0000000C FFFFFFFF
 // ENTRY
 //  00000000
 //  000022F8 -- offset divided by scale
 //  00000020 -- ???
 //  00000066 -- ??? file type
 //  0000000C -- datasize
 //  FFFFFFFF -- terminator

 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = BE_read_uint32(ifile);
 uint32 h02 = BE_read_uint32(ifile);
 uint32 h03 = BE_read_uint32(ifile);
 uint32 h04 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // validate header
 if(h01 != 0x01059401) return error("Invalid magic number.");
 if(h02 == 0x0000) return true; // nothing to do
 if(h03 != 0x0100) return error("Invalid offset scale.");
 if(h04 != 0x0000) return error("Expecting 0x00.");

 // read file table
 struct DLCENTRY {
  uint32 p01; // ???
  uint32 p02; // offset divided by scalar
  uint32 p03; // ???
  uint32 p04; // datatype
  uint32 p05; // datasize
  uint32 p06; // terminator
 };
 deque<DLCENTRY> filetable;
 for(size_t i = 0; i < h02; i++) {
     DLCENTRY item;
     item.p01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     item.p02 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     item.p03 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     item.p04 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     item.p05 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     item.p06 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     filetable.push_back(item);
    }

 // create output directory
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     size_t position = filetable[i].p02*h03;
     size_t filesize = filetable[i].p05;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // ignore small files
     if(filesize < 0x80) continue;

     // create BIN filename
     cout << "Reading file " << (i + 1) << " of " << filetable.size() << "." << endl;
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(4) << i;
     if(filetable[i].p04 == 0xC9) ss << TEXT(".idxzrc");
     else ss << TEXT(".idxout");

     // create BIN file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create bin file.");

     // read data
     boost::shared_array<char> data(new char[filesize]);
     ifile.read(data.get(), filesize);
     if(ifile.fail()) return error("Read failure.");
     
     // save data
     ofile.write(data.get(), filesize);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
}

// OLD VERSION... DO NOT USE...
bool UnpackDAT(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filename properties
 auto pathname = GetPathnameFromFilename(filename);
 auto shrtname = GetShortFilenameWithoutExtension(filename);

 // read size of offset list (not important)
 uint32 list_bytes = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to 0x10
 ifile.seekg(0x10);
 if(ifile.fail()) return error("Seek.");

 // read offset table ignoring zeros
 typedef pair<uint32,uint32> pair_t;
 deque<pair_t> offsets;
 for(uint32 i = 0; i < n_files; i++) {
     uint32 value = BE_read_uint32(ifile);
     if(ifile.fail()) return false;
     if(value != 0x00) offsets.push_back(pair_t(i, value));
    }
 
 // create save directory
 STDSTRINGSTREAM ss;
 ss << pathname.get() << shrtname.get() << TEXT("\\");
 STDSTRING savepath = ss.str();
 CreateDirectory(savepath.c_str(), NULL);

 // for each offset
 for(uint32 i = 0; i < offsets.size(); i++)
    {
     // determine data size
     uint32 datasize = 0;
     if(i < (offsets.size() - 1)) datasize = offsets[i + 1].second - offsets[i].second;
     else datasize = filesize - offsets[i].second;

     // nothing to save
     if(!datasize) continue;
     if(datasize < 0x80) continue;

     // move to offset
     ifile.seekg(offsets[i].second);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);
     if(ifile.fail()) return error("Read failure.");

     // create BIN file
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(4) << offsets[i].first << "." << "idxout";
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create bin file.");

     // save data
     ofile.write(data.get(), datasize);
     if(ofile.fail()) return error("Write failure.");
    }

 // success
 return true;
}

bool UnpackDLC_TypeA(LPCTSTR pathname)
{
 using namespace std;
 cout << "Processing DLC .DAT files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".DAT"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!UnpackDAT_TypeA(filelist[i].c_str())) return false;
    }
 cout << endl;
 return true;
}

bool UnpackDLC_TypeB(LPCTSTR pathname)
{
 using namespace std;
 cout << "Processing DLC .DAT files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".DAT"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     UnpackDAT_TypeB(filelist[i].c_str());
    }
 cout << endl;
 return true;
}

//
// UnpackDLC_TypeC
// EXAMPLE: [PS3] Dragon Quest Heroes DLC (2015)
//
bool UnpackDLC_TypeC(LPCTSTR pathname)
{
 using namespace std;
 cout << "Processing DLC .BIN.GZ files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT("GZ"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     auto fname = ChangeFileExtension(filelist[i].c_str(), TEXT("idxzrc"));
     if(!MoveFileEx(filelist[i].c_str(), fname.get(), 0)) message("Error: File renaming failed.");
    }
 cout << endl;
 return true;
}

//
// UnpackDLC_TypeC
// EXAMPLE: [PS3] Warriors Orochi 3
//
bool UnpackDLC_TypeD(LPCTSTR pathname)
{
 using namespace std;
 cout << "Processing DAT files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT("DAT"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     DWRenameIDXOUT(filelist[i].c_str());
    }
 cout << endl;
 return true;
}

bool UnpackDLC(LPCTSTR pathname)
{
 using namespace std;
 cout << "Processing DLC .DAT files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".DAT"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!UnpackDAT(filelist[i].c_str())) return false;
    }
 cout << endl;
 return true;
}

bool UnpackIDXOUT(LPCTSTR filename)
{
 // exact same algorithm
 return XB360::DYNASTY_WARRIORS::UnpackIDXOUT(filename);
}

bool UnpackIDXZRC(LPCTSTR pathname)
{
 // exact same algorithm
 return XB360::DYNASTY_WARRIORS::UnpackIDXZRC(pathname);
}

bool IDXArchiveUnpackLoop(LPCTSTR pathname)
{
 // exact same algorithm
 return XB360::DYNASTY_WARRIORS::IDXArchiveUnpackLoop(pathname);
}

bool UnpackTextures(LPCTSTR pathname)
{
 // exact same algorithm
 return XB360::DYNASTY_WARRIORS::UnpackTextures(pathname, false);
}

bool UnpackModels(LPCTSTR pathname)
{
 // exact same algorithm
 return XB360::DYNASTY_WARRIORS::UnpackModels(pathname);
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

};};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

}};

