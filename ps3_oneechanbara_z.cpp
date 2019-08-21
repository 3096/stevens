#include "xentax.h"
#include "x_file.h"
#include "x_stream.h"
#include "x_amc.h"
#include "ps3_oneechanbara_z.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   ONEECHANBARA_Z

//
// ADJACENCY LIST
//

class indexing_adjacency_list {
 private :
  deque<deque<size_t>> data;
  set<size_t> rootlist;
 public :
  void insert_root(size_t i);
  void remove_root(size_t i);
 public :
  explicit indexing_adjacency_list(size_t n);
 ~indexing_adjacency_list();
};

indexing_adjacency_list::indexing_adjacency_list(size_t n)
{
 data.resize(n);
}

indexing_adjacency_list::~indexing_adjacency_list()
{
}

void indexing_adjacency_list::insert_root(size_t i)
{
 if(!(i < data.size())) return;
 rootlist.insert(i);
}

void indexing_adjacency_list::remove_root(size_t i)
{
 if(!(i < data.size())) return;
 rootlist.erase(i);
}


//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processCAT_LE(LPCTSTR filename)
{
 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read header
 uint32 h01 = LE_read_uint32(ifile); // 0x00000000
 uint32 h02 = LE_read_uint32(ifile); // number of files
 uint32 h03 = LE_read_uint32(ifile); // 0x00000000

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < h02; i++) {
     uint32 p01 = LE_read_uint32(ifile); // start offset
     if(ifile.fail()) return error("Read failure 1.");
     offsets.push_back(p01);
    }

 // read sizes
 deque<uint32> sizelist;
 for(uint32 i = 0; i < h02; i++) {
     uint32 p01 = LE_read_uint32(ifile); // filesize
     if(ifile.fail()) return error("Read failure 2.");
     sizelist.push_back(p01);
    }

 // create save path
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // save files
 for(uint32 i = 0; i < h02; i++)
    {
     // move to offset
     if(sizelist[i] == 0) continue;
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[sizelist[i]]);
     ifile.read(data.get(), sizelist[i]);
     if(ifile.fail()) return error("Read failure.");

     // create output filename
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TEXT('0')) << setw(4) << i << TEXT(".obz");

     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save data
     ofile.write(data.get(), sizelist[i]);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
}

bool processOBZ_LE(LPCTSTR filename)
{
 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read header
 uint32 h01 = LE_read_uint32(ifile); // 0x00000000
 uint32 h02 = LE_read_uint32(ifile); // number of files
 uint32 h03 = LE_read_uint32(ifile); // ???
 uint32 h04 = LE_read_uint32(ifile); // ???
 uint32 h05 = LE_read_uint32(ifile); // ???

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < h02; i++) {
     uint32 p01 = LE_read_uint32(ifile); // start offset
     if(ifile.fail()) return error("Read failure.");
     offsets.push_back(p01);
    }

 // read sizes
 deque<uint32> sizelist;
 for(uint32 i = 0; i < h02; i++) {
     uint32 p01 = LE_read_uint32(ifile); // filesize
     if(ifile.fail()) return error("Read failure.");
     sizelist.push_back(p01);
    }

 // compute expected 1st offset
 uint32 expected_offset = 0x14 + h02 * 0x08;
 expected_offset = ((expected_offset + 0xFu) & (~0xFu));
 if(expected_offset != offsets[0]) return error("Unexpected offset.");

 // save files
 for(uint32 i = 0; i < h02; i++)
    {
     // move to offset
     if(sizelist[i] == 0) continue;
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[sizelist[i]]);
     ifile.read(data.get(), sizelist[i]);
     if(ifile.fail()) return error("Read failure.");

     // create output filename
     STDSTRINGSTREAM ss;
     ss << pathname << shrtname << L"_";
     ss << setfill(L'0') << setw(2) << i << L".part";

     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");
     
     // save data
     ofile.write(data.get(), sizelist[i]);
     if(ofile.fail()) return error("Write failure.");
     
     // close output file and create binary stream from output
     ofile.close();
     binary_stream bs(data, sizelist[i]);
     
     // read the first four bytes
     uint32 magic = bs.LE_read_uint32();
     if(bs.fail()) continue;

     // rename file
     if(magic == 0x50485952) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"phyr").get(); // model file
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x4D335242) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"m3rb").get(); // ??? model file ???
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x020200FF) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"fxtx").get(); // effects texture
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x41637469) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"action").get(); // actions
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x54455854) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"text").get(); // text data
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x424D4249) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"bmbi").get(); // ???
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"unknown").get(); // ???
        MoveFile(ss.str().c_str(), newname.c_str());
       }
    }

 return true;
}

bool processCAT(LPCTSTR filename)
{
 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read header
 uint32 h01 = BE_read_uint32(ifile); // 0x00000000
 uint32 h02 = BE_read_uint32(ifile); // number of files
 uint32 h03 = BE_read_uint32(ifile); // 0x00000000

 cout << "number of files = " << h02 << endl;

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < h02; i++) {
     uint32 p01 = BE_read_uint32(ifile); // start offset
     if(ifile.fail()) return error("Read failure 1.");
     offsets.push_back(p01);
    }

 // read sizes
 deque<uint32> sizelist;
 for(uint32 i = 0; i < h02; i++) {
     uint32 p01 = BE_read_uint32(ifile); // filesize
     if(ifile.fail()) return error("Read failure 2.");
     sizelist.push_back(p01);
    }

 // create save path
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // save files
 for(uint32 i = 0; i < h02; i++)
    {
     // move to offset
     if(sizelist[i] == 0) continue;
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[sizelist[i]]);
     ifile.read(data.get(), sizelist[i]);
     if(ifile.fail()) return error("Read failure.");

     // create output filename
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TEXT('0')) << setw(4) << i << TEXT(".obz");

     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save data
     ofile.write(data.get(), sizelist[i]);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
}

bool processOBZ(LPCTSTR filename)
{
 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read header
 uint32 h01 = BE_read_uint32(ifile); // 0x00000000
 uint32 h02 = BE_read_uint32(ifile); // number of files
 uint32 h03 = BE_read_uint32(ifile); // ???
 uint32 h04 = BE_read_uint32(ifile); // ???
 uint32 h05 = BE_read_uint32(ifile); // ???

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < h02; i++) {
     uint32 p01 = BE_read_uint32(ifile); // start offset
     if(ifile.fail()) return error("Read failure.");
     offsets.push_back(p01);
    }

 // read sizes
 deque<uint32> sizelist;
 for(uint32 i = 0; i < h02; i++) {
     uint32 p01 = BE_read_uint32(ifile); // filesize
     if(ifile.fail()) return error("Read failure.");
     sizelist.push_back(p01);
    }

 // compute expected 1st offset
 uint32 expected_offset = 0x14 + h02 * 0x08;
 expected_offset = ((expected_offset + 0xFu) & (~0xFu));
 if(expected_offset != offsets[0]) return error("Unexpected offset.");

 // save files
 for(uint32 i = 0; i < h02; i++)
    {
     // move to offset
     if(sizelist[i] == 0) continue;
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[sizelist[i]]);
     ifile.read(data.get(), sizelist[i]);
     if(ifile.fail()) return error("Read failure.");

     // create output filename
     STDSTRINGSTREAM ss;
     ss << pathname << shrtname << L"_";
     ss << setfill(L'0') << setw(2) << i << L".part";

     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");
     
     // save data
     ofile.write(data.get(), sizelist[i]);
     if(ofile.fail()) return error("Write failure.");
     
     // close output file and create binary stream from output
     ofile.close();
     binary_stream bs(data, sizelist[i]);
     
     // read the first four bytes
     uint32 magic = bs.BE_read_uint32();
     if(bs.fail()) continue;

     // rename file
     if(magic == 0x50485952) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"phyr").get(); // model file
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x4D335242) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"m3rb").get(); // ??? model file ???
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x020200FF) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"fxtx").get(); // effects texture
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x41637469) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"action").get(); // actions
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x54455854) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"text").get(); // text data
        MoveFile(ss.str().c_str(), newname.c_str());
       }
     else if(magic == 0x424D4249) {
        STDSTRING newname = ChangeFileExtension(ss.str().c_str(), L"bmbi").get(); // ???
        MoveFile(ss.str().c_str(), newname.c_str());
       }
    }

 return true;
}

};};

//
// MODELS AND TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

class PhryEngineReader {
 private :
  STDSTRING filename;
  STDSTRING pathname;
  STDSTRING shrtname;
  STDSTRING dbgfname;
  ifstream ifile;
  ofstream dfile;
 private :
  struct ENTRYTYPE1 {
   uint32 data[9];
  };
  struct ENTRYTYPE2 {
   uint32 data[6];
  };
  deque<uint32>     entries0;
  deque<ENTRYTYPE1> entries1;
  deque<ENTRYTYPE2> entries2;
  boost::shared_array<char> stringtable;
 private :
  boost::shared_array<char> readStringFromTable(uint64 offset);
 public :
  bool extract(LPCTSTR fn);
 public :
  PhryEngineReader() {}
 ~PhryEngineReader() {}
};

bool PhryEngineReader::extract(LPCTSTR fn)
{
 // filename properties
 filename = fn;
 pathname = GetPathnameFromFilename(filename.c_str()).get();
 shrtname = GetShortFilenameWithoutExtension(filename.c_str()).get();

 // open file
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read 0x58-byte header
 uint32 h01 = BE_read_uint32(ifile); // PHYR
 uint32 h02 = BE_read_uint32(ifile); // size of header
 if(h01 != 0x50485952) return error("Not a PHYR file.");
 if(h02 != 0x58) return error("Invalid PHYR header size. Expecting 0x58 bytes.");

 // create debug filename
 dbgfname = pathname;
 dbgfname += shrtname;
 dbgfname += TEXT(".txt");

 // create debug file
 ofstream dfile(dbgfname.c_str());
 if(!dfile) return error("Failed to create debug file.");

 // move to 0x58
 ifile.seekg(0x58);
 if(ifile.fail()) return error("Seek failure.");

 // read entry information
 uint32 e01 = BE_read_uint32(ifile); // 0x01020304
 uint32 e02 = BE_read_uint32(ifile); // offset from EOH to start of data
 uint32 e03 = BE_read_uint32(ifile); // number of string table references to read
 uint32 e04 = BE_read_uint32(ifile); // number of entries #1
 uint32 e05 = BE_read_uint32(ifile); // number of entries #2
 uint32 e06 = BE_read_uint32(ifile); // size of string table
 uint32 e07 = BE_read_uint32(ifile); // 0x00000000
 uint32 e08 = BE_read_uint32(ifile); // 0x00000000

 // validate entry information
 if(e01 != 0x01020304) return error("Invalid PHYR entry information. Expecting 0x01020304.");
 if(e07 != 0x00000000) return error("Invalid PHYR entry information. Expecting e07 = 0x00000000.");
 if(e08 != 0x00000000) return error("Invalid PHYR entry information. Expecting e08 = 0x00000000.");

 // read list of string table references (TYPES)
 for(uint32 i = 0; i < e03; i++) {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     entries0.push_back(item);
    }

 // read list of entries #1
 for(uint32 i = 0; i < e04; i++) {
     ENTRYTYPE1 item;
     for(uint32 j = 0; j < 9; j++) {
         item.data[j] = BE_read_uint32(ifile);
         if(ifile.fail()) return error("Read failure.");
        }
     if(item.data[0] == 0) item.data[0] = 0xFFFFFFFF;
     else item.data[0] = item.data[0] - 1;
     entries1.push_back(item);
    }

 // read list of entries #2
 for(uint32 i = 0; i < e05; i++) {
     ENTRYTYPE2 item;
     for(uint32 j = 0; j < 6; j++) {
         item.data[j] = BE_read_uint32(ifile);
         if(ifile.fail()) return error("Read failure.");
        }
     entries2.push_back(item);
    }

 // compute string table size
 uint32 offset1 = (uint32)ifile.tellg();
 uint32 offset2 = 0x58u + e02;
 if(!(offset1 < offset2)) return error("Invalid string table size.");

 // read string table
 uint32 tablesize = offset2 - offset1;
 stringtable.reset(new char[tablesize]);
 ifile.read(stringtable.get(), tablesize);
 if(ifile.fail()) return error("Read failure.");

 // convert string table to binary stream
 binary_stream sts(stringtable, tablesize);

 // BEGIN DEBUG
 dfile << "ENTRY TABLE #0: 0x" << hex << entries0.size() << dec << " ENTRIES" << endl;
 for(uint32 i = 0; i < e03; i++) {
     dfile << setfill('0') << setw(8) << hex << entries0[i] << dec << " ";
     uint32 stroffset = entries0[i];
     sts.seek(stroffset);
     string nodename = sts.read_string();
     dfile << nodename << endl;
    }
 dfile << endl;
 dfile << "ENTRY TABLE #1: 0x" << hex << entries1.size() << dec << " ENTRIES" << endl;
 for(uint32 i = 0; i < e04; i++) {
     dfile << setfill('0') << setw(8) << hex << i << dec << ": ";
     for(uint32 j = 0; j < 9; j++) dfile << setfill('0') << setw(8) << hex << entries1[i].data[j] << dec << " ";
     uint32 stroffset = entries1[i].data[2];
     sts.seek(stroffset);
     string nodename = sts.read_string();
     dfile << nodename << endl;
    }
 dfile << endl;
 dfile << "ENTRY TABLE #2: 0x" << hex << entries2.size() << dec << " ENTRIES" << endl;
 for(uint32 i = 0; i < e05; i++) {
     for(uint32 j = 0; j < 6; j++) dfile << setfill('0') << setw(8) << hex << entries2[i].data[j] << dec << " ";
     uint32 stroffset = entries2[i].data[0];
     sts.seek(stroffset);
     string nodename = sts.read_string();
     dfile << nodename << endl;
    }
 dfile << endl;
 // END DEBUG

 // move to data
 uint64 position = 0x58 + e02;
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");

 return true;
}

boost::shared_array<char> PhryEngineReader::readStringFromTable(uint64 offset)
{
 uint64 position = (uint64)ifile.tellg();
 boost::shared_array<char> data(new char[1024]);
 if(!read_string(ifile, data.get(), 1024)) return boost::shared_array<char>();
 ifile.seekg(position);
 if(ifile.fail()) return boost::shared_array<char>();
 return data;
}

bool processPHY(LPCTSTR filename)
{
 PhryEngineReader per;
 return per.extract(filename);
}

};};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

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

 // filetypes to process
 bool doCAT = true;
 bool doOBZ = true;
 bool doPHY = false;

 // process CAT
 if(doCAT && YesNoBox("Process CAT files?\nClick 'No' if you have already done so."))
   {
    cout << "Processing .CAT files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, L".cat", pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCAT_LE(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process OBZ
 if(doOBZ && YesNoBox("Process OBZ files?\nClick 'No' if you have already done so."))
   {
    cout << "Processing .OBZ files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, L".obz", pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processOBZ_LE(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process PHY
 if(doPHY && YesNoBox("Process PHYR files?\nClick 'No' if you have already done so."))
   {
    cout << "Processing .PHYR files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, L".phyr", pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processPHY(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 return true;
}

};};