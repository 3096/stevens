#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_namco.h"
#include "ps3_tekrev.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   TEKKEN_REVOLUTION

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

// AC1
// 0x10 byte header
// followed by list of <offset, filesize> pairs
// 0x10-aligned offsets and filesizes
// may contain <0, 0> pairs
bool isAC1(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // if filesize is small
 if(filesize < 0x10) return false;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = BE_read_uint32(ifile);
 uint32 h02 = BE_read_uint32(ifile);
 uint32 h03 = BE_read_uint32(ifile);
 uint32 h04 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // conditions
 if(h01 == 0x00) return false;
 if(h03 == 0x00) return false;
 if(h03 % 0x10) return false;

 // move to offset
 ifile.seekg(h03);
 if(ifile.fail()) return false;

 // read data in pairs of twos
 uint32 n_items = 0;
 deque<pair<uint32,uint32>> itemlist;
 for(;;)
    {
     // read pair
     uint32 p01 = BE_read_uint32(ifile);
     uint32 p02 = BE_read_uint32(ifile);
     if(ifile.fail()) break;

     // both are zero
     if(p01 == 0x00 && p02 == 0x00)
        continue;

     // must be divisible by 0x10
     if(p01 % 0x10) break;
     if(p02 % 0x10) break;

     // offset should be sum of previous item
     if(itemlist.size()) {
        pair<uint32,uint32> item = itemlist.back();
        uint32 offset = item.first + item.second;
        if(p01 != offset) return false;
       }

     // insert item
     itemlist.push_back(pair<uint32,uint32>(p01, p02));
    }

 // no items
 if(itemlist.size() == 0) return false;

 // validate last item
 uint32 last_index = itemlist.size() - 1;
 uint32 size = itemlist[last_index].first + itemlist[last_index].second;
 if(size != filesize) return false;

 // success
 return true;
}

// AC2
// 0x04 byte header
// followed by list of <offset, filesize> pairs
// 0x10-aligned offsets
// filesizes are not aligned
bool isAC2(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // if filesize is small
 if(filesize < 0x10) return false;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 n_items = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // validate items
 if(n_items == 0x00 || n_items > 0xFFF) return false;
 if(filesize < (0x04 + (0x08*n_items))) return false;

 // read data in pairs of twos
 deque<pair<uint32,uint32>> itemlist;
 for(uint32 i = 0; i < n_items; i++)
    {
     // read pair
     uint32 p01 = BE_read_uint32(ifile);
     uint32 p02 = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // can't be zero
     if(p01 == 0x00) return false;
     if(p02 == 0x00) return false;

     // p01 must be divisible by 0x10
     if(p01 % 0x10) return false;

     // offset should be sum of previous item
     if(itemlist.size()) {
        pair<uint32,uint32> item = itemlist.back();
        uint32 offset = item.first + ((item.second + 0x0F) & (~0x0F));
        if(p01 != offset) return false;
       }

     // insert item
     itemlist.push_back(pair<uint32,uint32>(p01, p02));
    }

 // validate last item
 auto last_index = itemlist.size() - 1;
 auto item = itemlist[last_index];
 auto size = item.first + ((item.second + 0x0F) & (~0x0F));
 if(size != filesize) return false;

 return true;
}

// AC3
// 0x04 byte header
// followed by list of <offset, filesize> pairs
// offsets and filesizes are not aligned
bool isAC3(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // if filesize is small
 if(filesize < 0x10) return false;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 n_items = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // validate items
 if(n_items == 0x00 || n_items > 0xFFF) return false;
 if(filesize < (0x04 + (0x08*n_items))) return false;

 // read data in pairs of twos
 deque<pair<uint32,uint32>> itemlist;
 for(uint32 i = 0; i < n_items; i++)
    {
     // read pair
     uint32 p01 = BE_read_uint32(ifile);
     uint32 p02 = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // can't be zero
     if(p01 == 0x00) return false;
     if(p02 == 0x00) return false;

     // offset should be sum of previous item
     if(itemlist.size()) {
        pair<uint32,uint32> item = itemlist.back();
        uint32 offset = item.first + item.second;
        if(p01 != offset) return false;
       }

     // insert item
     itemlist.push_back(pair<uint32,uint32>(p01, p02));
    }

 // validate last item
 auto last_index = itemlist.size() - 1;
 auto item = itemlist[last_index];
 auto size = item.first + item.second;
 if(size != filesize) return false;

 return true;
}

// AC4
// little-endian AC
bool isAC4(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // if filesize is small
 if(filesize < 0x10) return false;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 n_items = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // validate items
 if(n_items == 0x00 || n_items > 0xFFF) return false;
 if(filesize < (0x04 + (0x08*n_items))) return false;

 // read data in pairs of twos
 deque<pair<uint32,uint32>> itemlist;
 for(uint32 i = 0; i < n_items; i++)
    {
     // read pair
     uint32 p01 = LE_read_uint32(ifile);
     uint32 p02 = LE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // can't be zero
     if(p01 == 0x00) return false;
     if(p02 == 0x00) return false;

     // p01 must be divisible by 0x10
     if(p01 % 0x10) return false;

     // offset should be sum of previous item
     if(itemlist.size()) {
        pair<uint32,uint32> item = itemlist.back();
        uint32 offset = item.first + ((item.second + 0x0F) & (~0x0F));
        if(p01 != offset) {
           cout << "Failed at 0x" << hex << (uint32)ifile.tellg() << dec << endl;
           return false;
          }
       }

     // insert item
     itemlist.push_back(pair<uint32,uint32>(p01, p02));
    }

 // validate last item
 auto last_index = itemlist.size() - 1;
 auto item = itemlist[last_index];
 auto size = item.first + ((item.second + 0x0F) & (~0x0F));
 if(size != filesize) return false;

 return true;
}

// AC5
// 0x10 byte header
// followed by list of 0x10 byte entries
// offsets link to lanaguage strings (very useless)
bool isAC5(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // if filesize is small
 if(filesize < 0x10) return false;

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
 if(h01 == 0x00) return false;
 if(h02 == 0x00) return false;
 if(h03 == 0x00) return false;
 if(h04 == 0x00) return false;

 // h01 must be divisible by 0x10
 if(h01 % 0x10) return false;

 // number of items
 uint32 items = (h01 - 0x10)/0x10;
 if(!items) return false;

 // for each item
 deque<uint32> itemlist;
 for(uint32 i = 0; i < items; i++)
    {
     // read item
     uint32 p01 = BE_read_uint32(ifile);
     uint32 p02 = BE_read_uint32(ifile);
     uint32 p03 = BE_read_uint32(ifile);
     uint32 p04 = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // p01 must be a valid offset
     if(!(p01 < filesize)) return false;

     // validate item
     if(itemlist.size()) {
        if(!(itemlist.back() < p01)) return false;
       }
     // first item
     else {
        if(p01 != h01) return false;
       }

     // insert item
     itemlist.push_back(p01);
    }

 return true;
}

bool processTXT(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create association information file
 STDSTRING afname = pathname;
 afname += TEXT("association.txt");
 ofstream afile(afname.c_str());
 if(!afile) return error("Failed to create association file.");

 // build a list of DAT files
 deque<STDSTRING> filelist;
 for(;;)
    {
     // get line
     char buffer[2048];
     ifile.getline(&buffer[0], 2048);
     if(ifile.fail()) break;

     // convert value
     unsigned int value = strtoul(buffer, nullptr, 0);
     STDSTRINGSTREAM ss;
     ss << hex << value << dec;

     // align string
     STDSTRING temp = ss.str();
     if(temp.length() % 2) {
        STDSTRING adjs(TEXT("0"));
        adjs += temp;
        temp = adjs;
       }
     // use double zeroes
     if(temp.length() == 6) {
        STDSTRING adjs(TEXT("00"));
        adjs += temp;
        temp = adjs;
       }

     // construct full path
     STDSTRING datname = pathname;
     datname += temp;
     datname += TEXT(".dat");

     // make sure file exists
     if(GetFileAttributes(datname.c_str()) == INVALID_FILE_ATTRIBUTES) {
        STDSTRINGSTREAM ss;
        ss << TEXT("Filename not found: ") << datname;
        return error(ss);
       }

     // insert filename
     filelist.push_back(datname);

     // association file data
     STDSTRING shrtdatname = temp;
     shrtdatname += TEXT(".dat");     
     afile << (filelist.size() - 1) << ": " << Unicode16ToUnicode08(shrtdatname.c_str()).get() << endl;
    }

 // copy files
 for(uint32 i = 0; i < filelist.size(); i++) {
     wcout << TEXT("Copying ") << filelist[i] << endl;
     STDSTRINGSTREAM ss;
     ss << pathname;
     ss << setfill(TEXT('0')) << setw(4) << i;
     ss << TEXT(".dat");
     CopyFile(filelist[i].c_str(), ss.str().c_str(), FALSE);
    }

 // delete old DAT files
 for(uint32 i = 0; i < filelist.size(); i++)
     DeleteFile(filelist[i].c_str());

 return true;
}

bool processDAT(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // if filesize is small
 if(filesize < 0x10) {
    ifile.close();
    MoveFile(filename, ChangeFileExtension(filename, TEXT("USELESS")).get());
    return true;
   }

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read first four bytes and close file
 uint32 type = BE_read_uint32(ifile);
 uint32 next = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // close file
 ifile.close(); 

 // rename NDP3
 if(type == 0x4E445033)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NDP3")).get());

 // rename NTP3
 else if(type == 0x4E545033)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NTP3")).get());

 // rename NTXB
 else if(type == 0x4E545842)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NTXB")).get());

 // rename BONE
 else if(type == 0x424F4E45)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("BONE")).get());

 // rename MOTA
 else if(type == 0x4D4F5441)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("MOTA")).get());

 // rename MBB_
 else if(type == 0x4D424200)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("MBB_")).get());

 // rename PNG
 else if(type == 0x89504E47)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("PNG")).get());

 // rename NUS3 (audio)
 else if(type == 0x4E555333)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NUS")).get());

 // rename PAOF
 else if(type == 0x70616F66)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("PAOF")).get());

 // rename PAMF (video)
 else if(type == 0x50414D46)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("PAM")).get());

 // rename PCTB
 else if(type == 0x50435442)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("PCTB")).get());

 // rename 40A0
 else if(type == 0x40A00000)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("40A0")).get());

 // rename BOM (XML)
 else if(type == 0xEFBBBF3C)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("BOM")).get());

 // rename BX01
 else if(type == 0x42583031)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("BX01")).get());

 // rename XML
 else if(type == 0x3C3F786D)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("XML")).get());

 // rename XML (without header)
 else if(type == 0x3C726F6F)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("XML")).get());

 // rename BXML
 else if(type == 0x42584D4C)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("BXML")).get());

 // rename COM_
 else if(type == 0x00000000 && next == 0x434F4D00)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("COM_")).get());

 // rename LMB_
 else if(type == 0x4C4D4200)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("LMB_")).get());

 // rename MVLT
 else if(type == 0x4D564C54)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("MVLT")).get());

 // rename NFH_
 else if(type == 0x4E464800)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NFH_")).get());

 // rename NSP3
 else if(type == 0x4E535033)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NSP3")).get());

 // rename OELA
 else if(type == 0x4F454C41)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("OELA")).get());

 // rename XTAL
 else if(type == 0x7874616C)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("XTAL")).get());

 // test for AC1
 else if(isAC1(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("AC1")).get());

 // test for AC2
 else if(isAC2(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("AC2")).get());

 // test for AC3
 else if(isAC3(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("AC3")).get());

 // test for AC4
 else if(isAC4(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("AC4")).get());

 // test for AC5
 else if(isAC5(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("AC5")).get());

 // useless
 else if(filesize < 0x40)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("USELESS")).get());

 // unknown
 else
    MoveFile(filename, ChangeFileExtension(filename, TEXT("UNKNOWN")).get());

 return true;
}

bool processAC1(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create save path
 STDSTRING savepath = pathname;
 savepath += TEXT("AC1_");
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // read number of files
 uint32 h01 = BE_read_uint32(ifile);
 uint32 h02 = BE_read_uint32(ifile);
 uint32 h03 = BE_read_uint32(ifile);
 uint32 h04 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset pairs
 typedef pair<uint32,uint32> pair_t;
 deque<pair_t> itemlist;
 for(;;) {
     uint32 p01 = BE_read_uint32(ifile);
     uint32 p02 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // skip
     if(p01 == 0 && p02 == 0) continue;

     // must be divisible by 0x10
     if(p01 % 0x10) break;
     if(p02 % 0x10) break;

     // offset should be sum of previous item
     if(itemlist.size()) {
        pair<uint32,uint32> item = itemlist.back();
        uint32 offset = item.first + item.second;
        if(p01 != offset) break;
       }

     // insert item
     itemlist.push_back(pair_t(p01, p02));
    }

 // for each item
 for(uint32 i = 0; i < itemlist.size(); i++)
    {
     // move to offset
     ifile.seekg(itemlist[i].first);
     if(ifile.fail()) return error("Seek failure.");

     // nothing to save
     if(!itemlist[i].second) continue;

     // read data
     boost::shared_array<char> data(new char[itemlist[i].second]);
     ifile.read(data.get(), itemlist[i].second);
     if(ifile.fail()) return error("Read failure.");

     // generate filename
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TEXT('0')) << setw(3) << i << TEXT(".dat");

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     ofile.write(data.get(), itemlist[i].second);
     if(ofile.fail()) return error("Write failure.");
     ofile.close();
    }

 // close and delete archive
 ifile.close();
 DeleteFile(filename);
 return true;
}

bool processAC2(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create save path
 STDSTRING savepath = pathname;
 savepath += TEXT("AC2_");
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // read number of files
 uint32 h01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset/size pairs
 typedef pair<uint32,uint32> pair_t;
 deque<pair_t> itemlist;
 for(uint32 i = 0; i < h01; i++) {
     uint32 p01 = BE_read_uint32(ifile);
     uint32 p02 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     itemlist.push_back(pair_t(p01, p02));
    }

 // for each item
 for(uint32 i = 0; i < itemlist.size(); i++)
    {
     // move to offset
     ifile.seekg(itemlist[i].first);
     if(ifile.fail()) return error("Seek failure.");

     // nothing to save
     if(!itemlist[i].second) continue;

     // read data
     boost::shared_array<char> data(new char[itemlist[i].second]);
     ifile.read(data.get(), itemlist[i].second);
     if(ifile.fail()) return error("Read failure.");

     // generate filename
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TEXT('0')) << setw(3) << i << TEXT(".dat");

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     ofile.write(data.get(), itemlist[i].second);
     if(ofile.fail()) return error("Write failure.");
     ofile.close();
    }

 // close and delete archive
 ifile.close();
 DeleteFile(filename);
 return true;
}

bool processAC3(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create save path
 STDSTRING savepath = pathname;
 savepath += TEXT("AC3_");
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // read number of files
 uint32 h01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset/size pairs
 typedef pair<uint32,uint32> pair_t;
 deque<pair_t> itemlist;
 for(uint32 i = 0; i < h01; i++) {
     uint32 p01 = BE_read_uint32(ifile);
     uint32 p02 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     itemlist.push_back(pair_t(p01, p02));
    }

 // for each item
 for(uint32 i = 0; i < itemlist.size(); i++)
    {
     // move to offset
     ifile.seekg(itemlist[i].first);
     if(ifile.fail()) return error("Seek failure.");

     // nothing to save
     if(!itemlist[i].second) continue;

     // read data
     boost::shared_array<char> data(new char[itemlist[i].second]);
     ifile.read(data.get(), itemlist[i].second);
     if(ifile.fail()) return error("Read failure.");

     // generate filename
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TEXT('0')) << setw(3) << i << TEXT(".dat");

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     ofile.write(data.get(), itemlist[i].second);
     if(ofile.fail()) return error("Write failure.");
     ofile.close();
    }

 // close and delete archive
 ifile.close();
 DeleteFile(filename);
 return true;
}

bool processAC4(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create save path
 STDSTRING savepath = pathname;
 savepath += TEXT("AC4_");
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // read number of files
 uint32 h01 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset/size pairs
 typedef pair<uint32,uint32> pair_t;
 deque<pair_t> itemlist;
 for(uint32 i = 0; i < h01; i++) {
     uint32 p01 = LE_read_uint32(ifile);
     uint32 p02 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     itemlist.push_back(pair_t(p01, p02));
    }

 // for each item
 for(uint32 i = 0; i < itemlist.size(); i++)
    {
     // move to offset
     ifile.seekg(itemlist[i].first);
     if(ifile.fail()) return error("Seek failure.");

     // nothing to save
     if(!itemlist[i].second) continue;

     // read data
     boost::shared_array<char> data(new char[itemlist[i].second]);
     ifile.read(data.get(), itemlist[i].second);
     if(ifile.fail()) return error("Read failure.");

     // generate filename
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TEXT('0')) << setw(3) << i << TEXT(".dat");

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     ofile.write(data.get(), itemlist[i].second);
     if(ofile.fail()) return error("Write failure.");
     ofile.close();
    }

 // close and delete archive
 ifile.close();
 DeleteFile(filename);
 return true;
}

bool processAC5(LPCTSTR filename)
{
 // just delete useless file
 DeleteFile(filename);
 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processNTP3(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 auto h01 = BE_read_uint32(ifile); // NTP3
 auto h02 = BE_read_uint16(ifile); // 0x0100
 auto h03 = BE_read_uint16(ifile); // number of files
 auto h04 = BE_read_uint32(ifile); // 0x00
 auto h05 = BE_read_uint32(ifile); // 0x00

 // validate header
 if(h01 != 0x4E545033) return error("Invalid NTP3 file.");
 if(h03 == 0x00) return true; // nothing to do

 // create output directory
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // for each image
 uint32 position = 0x10;
 for(uint32 i = 0; i < h03; i++)
    {
     // seek position
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read image header
     auto total_size = BE_read_uint32(ifile);
     auto unk01 = BE_read_uint32(ifile);
     auto image_size = BE_read_uint32(ifile);
     auto ihead_size = BE_read_uint16(ifile);
     auto unk03 = BE_read_uint16(ifile);

     auto mipmaps = BE_read_uint16(ifile); // mipmaps + 1
     auto image_type = BE_read_uint16(ifile);
     auto dx = BE_read_uint16(ifile);
     auto dy = BE_read_uint16(ifile);
     auto unk04 = BE_read_uint32(ifile);
     auto unk05 = BE_read_uint32(ifile);

     if(ihead_size >= 0x50) {
        auto unk06 = BE_read_uint32(ifile); // 0x00
        auto unk07 = BE_read_uint32(ifile); // 0x00
        auto unk08 = BE_read_uint32(ifile); // 0x00
        auto unk09 = BE_read_uint32(ifile); // 0x00
       }
     if(ihead_size >= 0x60) {
        auto unk10 = BE_read_uint32(ifile); // usually < 0x40
        auto unk11 = BE_read_uint32(ifile); // usually < 0x40
        auto unk12 = BE_read_uint32(ifile); // usually < 0x40
        auto unk13 = BE_read_uint32(ifile); // usually < 0x40
       }
     if(ihead_size >= 0x70) {
        BE_read_uint32(ifile);
        BE_read_uint32(ifile);
        BE_read_uint32(ifile);
        BE_read_uint32(ifile);
       }
     if(ihead_size >= 0x80) {
        BE_read_uint32(ifile);
        BE_read_uint32(ifile);
        BE_read_uint32(ifile);
        BE_read_uint32(ifile);
       }

     auto ext01 = BE_read_uint32(ifile); // eXt
     auto ext02 = BE_read_uint32(ifile); // usually < 0x20
     auto ext03 = BE_read_uint32(ifile); // usually < 0x20
     auto ext04 = BE_read_uint32(ifile); // usually < 0x20

     auto gidx01 = BE_read_uint32(ifile); // GIDX
     auto gidx02 = BE_read_uint32(ifile); // usually < 0x10
     auto gidx03 = BE_read_uint32(ifile); // texture identifier
     auto gidx04 = BE_read_uint32(ifile); // 0x00

     // read image data
     if(!image_size) return error("Invalid image.");
     boost::shared_array<char> data(new char[image_size]);
     ifile.read(data.get(), image_size);

     // create output file
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TEXT('0')) << setw(8) << hex << gidx03 << dec << TEXT(".dds");
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // DXT1 image
     if(image_type == 0x00)
       {
        // create DDS header
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, mipmaps - 1, FALSE, &ddsh);
        
        // reverse byte order of data
        // uint16* temp = reinterpret_cast<uint16*>(data.get());
        // reverse_byte_order(temp, image_size/2);
        
        // save DDS header
        ofile.write("DDS ", 4);
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write(data.get(), image_size);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(image_type == 0x01)
       {
        // create DDS header
        DDS_HEADER ddsh;
        CreateDXT3Header(dx, dy, mipmaps - 1, FALSE, &ddsh);
     
        // save DDS header
        ofile.write("DDS ", 4);
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write(data.get(), image_size);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(image_type == 0x02)
       {
        // create DDS header
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, mipmaps - 1, FALSE, &ddsh);
     
        // save DDS header
        ofile.write("DDS ", 4);
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write(data.get(), image_size);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(image_type == 0x0E)
       {
        // PHOTOSHOP ONLY SUPPORTS ONE TYPE OF 32-BIT UNCOMPRESSED!
        // YOU MUST USE ARGB!
        uint32 m1 = 0xFF000000; // A
        uint32 m2 = 0x00FF0000; // R 
        uint32 m3 = 0x0000FF00; // G
        uint32 m4 = 0x000000FF; // B

        // swap channels so shit loads in photoshop
        uint32 n_pixels = dx * dy;
        for(uint32 j = 0; j < n_pixels; j++) {
            uint32 index1 = 4*j;
            uint32 index2 = index1 + 1;
            uint32 index3 = index2 + 1;
            uint32 index4 = index3 + 1;
            swap(data[index1], data[index4]); // swap A and B
            swap(data[index2], data[index3]); // swap R and G
           }

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, mipmaps - 1, 32, m2, m3, m4, m1, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)data.get(), image_size);
       }
     else if(image_type == 0x11)
       {
        cout << "SKIPPING IMAGE TYPE 0x11" << endl;
       }
     else {
        stringstream ss;
        ss << "Unknown image format 0x" << hex << image_type << dec << endl;
        return error(ss);
       }

     // increment position
     position += total_size;
    }

 return true;
}

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

bool processNDP3(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug file
 bool debug = true;
 ofstream dfile;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT(".debug");
    dfile.open(ss.str().c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // read header
 uint32 h01 = BE_read_uint32(ifile); // NDP3
 uint32 h02 = BE_read_uint32(ifile); // filesize
 uint16 h03 = BE_read_uint16(ifile); // 0x0201
 uint16 h04 = BE_read_uint16(ifile); // number of 0x30 byte entries
 uint16 h05 = BE_read_uint16(ifile); // unknown (sum these to get number of bones in base body)
 uint16 h06 = BE_read_uint16(ifile); // unknown (sum these to get number of bones in base body)
 uint32 h07 = BE_read_uint32(ifile); // size of mesh information
 uint32 h08 = BE_read_uint32(ifile); // size of idx buffer
 uint32 h09 = BE_read_uint32(ifile); // size of vtx buffer #1
 uint32 h10 = BE_read_uint32(ifile); // size of vtx buffer #2
 uint32 h11 = BE_read_uint32(ifile); // unknown
 uint32 h12 = BE_read_uint32(ifile); // unknown
 uint32 h13 = BE_read_uint32(ifile); // unknown
 uint32 h14 = BE_read_uint32(ifile); // unknown

 // debug output
 if(debug) {
    dfile << "------------" << endl;
    dfile << "-- HEADER --" << endl;
    dfile << "------------" << endl;
    dfile << endl;
    dfile << "0x" << hex << h01 << dec << " NDP3" << endl;
    dfile << "0x" << hex << h02 << dec << " filesize" << endl;
    dfile << "0x" << hex << h03 << dec << " 0x0201" << endl;
    dfile << "0x" << hex << h04 << dec << " number of meshes" << endl;
    dfile << "0x" << hex << h05 << dec << " unknown (sum these to get number of bones in base body)" << endl;
    dfile << "0x" << hex << h06 << dec << " unknown (sum these to get number of bones in base body)" << endl;
    dfile << "0x" << hex << h07 << dec << " size of mesh information" << endl;
    dfile << "0x" << hex << h08 << dec << " size of idx buffer" << endl;
    dfile << "0x" << hex << h09 << dec << " size of vtx buffer #1" << endl;
    dfile << "0x" << hex << h10 << dec << " size of vtx buffer #2" << endl;
    dfile << "0x" << hex << h11 << dec << " unknown" << endl;
    dfile << "0x" << hex << h12 << dec << " unknown" << endl;
    dfile << "0x" << hex << h13 << dec << " unknown" << endl;
    dfile << "0x" << hex << h14 << dec << " unknown" << endl;
    dfile << endl;
   }

 // read mesh information
 struct MESHINFOENTRY1 {
  real32 p01[8]; //
  uint32 p02;    // offset into mesh names
  uint32 p03;    // 0x2C
  uint16 p04;    //
  uint16 p05;    // number of MESHINFOENTRY2
  uint32 p06;    // offset to MESHINFOENTRY2 array
 };
 deque<MESHINFOENTRY1> entrylist1;
 for(uint32 i = 0; i < h04; i++) {
     MESHINFOENTRY1 item;
     BE_read_array(ifile, &item.p01[0], 8);
     item.p02 = BE_read_uint32(ifile);
     item.p03 = BE_read_uint32(ifile);
     item.p04 = BE_read_uint16(ifile);
     item.p05 = BE_read_uint16(ifile);
     item.p06 = BE_read_uint32(ifile);
     entrylist1.push_back(item);
    }

 // debug output
 if(debug) {
    dfile << "----------------------" << endl;
    dfile << "-- MESH INFORMATION --" << endl;
    dfile << "----------------------" << endl;
    dfile << endl;
    for(uint32 i = 0; i < h04; i++) {
        dfile << "MESH 0x" << i << endl;
        dfile << entrylist1[i].p01[0] << " unknown vector" << endl;
        dfile << entrylist1[i].p01[1] << " " << endl;
        dfile << entrylist1[i].p01[2] << " " << endl;
        dfile << entrylist1[i].p01[3] << " " << endl;
        dfile << entrylist1[i].p01[4] << " unknown vector " << endl;
        dfile << entrylist1[i].p01[5] << "  " << endl;
        dfile << entrylist1[i].p01[6] << "  " << endl;
        dfile << entrylist1[i].p01[7] << "  " << endl;
        dfile << "0x" << hex << entrylist1[i].p02 << dec << "  offset into mesh names" << endl;
        dfile << "0x" << hex << entrylist1[i].p03 << dec << "  0x2C" << endl;
        dfile << "0x" << hex << entrylist1[i].p04 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist1[i].p05 << dec << "  number of parts" << endl;
        dfile << "0x" << hex << entrylist1[i].p06 << dec << "  offset to parts" << endl;
        dfile << endl;
       }
   }

 // read part information
 struct MESHINFOENTRY2 {
  uint32 mesh_id;
  uint32 part_id;
  uint32 p01; // offset into ib
  uint32 p02; // offset into vb #1
  uint32 p03; // offset into vb #2
  uint16 p04; // number of vertices
  uint16 p05; // vertex format
  uint32 p06; // offset into material property data
  uint32 p07; // zero? 
  uint32 p08; // zero?
  uint16 p09; // zero?
  uint32 p10; // number of indices
  uint16 p11; // index buffer format
  uint32 p12; // zero?
  uint32 p13; // zero?
  uint32 p14; // zero?
 };
 deque<MESHINFOENTRY2> entrylist2;
 for(uint32 i = 0; i < entrylist1.size(); i++)
    {
     // seek offset
     uint32 offset = entrylist1[i].p06;
     ifile.seekg(offset);

     // for each entry #2
     uint16 n_item = entrylist1[i].p05;
     for(uint32 j = 0; j < n_item; j++)
        {
         MESHINFOENTRY2 item;
         item.mesh_id = i;
         item.part_id = j;
         item.p01 = BE_read_uint32(ifile);
         item.p02 = BE_read_uint32(ifile);
         item.p03 = BE_read_uint32(ifile);
         item.p04 = BE_read_uint16(ifile);
         item.p05 = BE_read_uint16(ifile);
         item.p06 = BE_read_uint32(ifile);
         item.p07 = BE_read_uint32(ifile);
         item.p08 = BE_read_uint32(ifile);
         item.p09 = BE_read_uint16(ifile);
         item.p10 = BE_read_uint32(ifile);
         item.p11 = BE_read_uint16(ifile);
         item.p12 = BE_read_uint32(ifile);
         item.p13 = BE_read_uint32(ifile);
         item.p14 = BE_read_uint32(ifile);
         entrylist2.push_back(item);
        }
    }

 // debug output
 if(debug) {
    dfile << "----------------------" << endl;
    dfile << "-- PART INFORMATION --" << endl;
    dfile << "----------------------" << endl;
    dfile << endl;
    for(uint32 i = 0; i < entrylist2.size(); i++) {
        dfile << "0x" << hex << entrylist2[i].mesh_id << dec << "  mesh index" << endl; 
        dfile << "0x" << hex << entrylist2[i].part_id << dec << "  part index" << endl;
        dfile << "0x" << hex << entrylist2[i].p01 << dec << "  offset into index buffer" << endl;
        dfile << "0x" << hex << entrylist2[i].p02 << dec << "  offset into vertex buffer #1" << endl;
        dfile << "0x" << hex << entrylist2[i].p03 << dec << "  offset into vertex buffer #2" << endl;
        dfile << "0x" << hex << entrylist2[i].p04 << dec << "  number of vertices" << endl;
        dfile << "0x" << hex << entrylist2[i].p05 << dec << "  vertex format" << endl;
        dfile << "0x" << hex << entrylist2[i].p06 << dec << "  offset into material property data" << endl;
        dfile << "0x" << hex << entrylist2[i].p07 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p08 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p09 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p10 << dec << "  number of indices" << endl;
        dfile << "0x" << hex << entrylist2[i].p11 << dec << "  index buffer format" << endl;
        dfile << "0x" << hex << entrylist2[i].p12 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p13 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p14 << dec << "  unknown" << endl;
        dfile << endl;
       }
   }

 // read material information
 struct MESHINFOENTRY3 {
  boost::shared_array<char> data;
  uint32 size;
  uint32 texture_id;
 };
 deque<MESHINFOENTRY3> entrylist3;
 for(uint32 i = 0; i < entrylist2.size(); i++)
    {
     // seek offset
     uint32 offset = entrylist2[i].p06;
     ifile.seekg(offset);

     // compute item size
     MESHINFOENTRY3 item;
     if((i + 1) == entrylist2.size()) item.size = (h07 + 0x30) - offset;
     else item.size = entrylist2[i + 1].p06 - offset;

     // read item
     item.data.reset(new char[item.size]);
     ifile.read(item.data.get(), item.size);
     if(ifile.fail()) return error("Read failure.");

     // convert item to binary stream
     binary_stream bs(item.data, item.size);
     uint32 id = bs.BE_read_uint32();
     if(bs.fail()) return error("Stream read failure.");

     // process binary stream
     switch(id) {
       case(0x10000001): 
       case(0x20600099): 
       case(0x206201d9): 
       case(0x20a00099): 
       case(0x20a02199): 
       case(0x20a10099): // 0x120 bytes
       case(0x20a201d9): 
       case(0x20c00001): 
       case(0x20e00019): 
       case(0x21000001): 
       case(0x21001001): 
       case(0x21008001): 
       case(0x21009001): 
       case(0x21040001): // new update 1.02
       case(0x21600059): 
       case(0x21800011): 
       case(0x21c00001): 
       case(0x22000011): // new update 1.02
       case(0x22a00011): 
       case(0x22c00001): 
       case(0x22c00019):
       case(0x22c40019): // new update 1.02 
       case(0x22e00019): 
       case(0x23000001): 
       case(0x23010001): 
       case(0x24400001): 
       case(0x24400011): 
       case(0x24400019): 
       case(0x24400091): 
       case(0x24400099):
       case(0x24401099): 
       case(0x24402099): // new update 1.02
       case(0x24402119): // new update 1.03
       case(0x24402199): 
       case(0x24408099): 
       case(0x24409099): 
       case(0x24409199): 
       case(0x24500099): 
       case(0x24502199): 
       case(0x24600019): 
       case(0x24600099):
       case(0x246000b9): 
       case(0x24601099): 
       case(0x24602199): 
       case(0x24608099):
       case(0x246080b9): 
       case(0x24800091): 
       case(0x24e01099): 
       case(0x25400081): 
       case(0x25800099): 
       case(0x2740819b): 
       case(0x27801099): 
       case(0x27a01099): 
       case(0x27a01199): 
       case(0x28000001):
       case(0x28000101):  
       case(0x28000109):
       case(0x28000401): // new update 1.02
       case(0x28001101): 
       case(0x28008101): 
       case(0x28100019): 
       case(0x28200101): 
       case(0x28200121): 
       case(0x28200401): // new update 1.02
       case(0x28211139): 
       case(0x28300039): 
       case(0x28301919): 
       case(0x2a008109): 
       case(0x2c010001): 
       case(0x2c011001): 
       case(0x2c100011): 
       case(0x2c100019): 
       case(0x2c101019):
       case(0x2c411001): {
            bs.seek(0x20);
            uint32 texture_id = bs.BE_read_uint32();
            if(bs.fail()) return error("Stream read failure.");
            item.texture_id = texture_id;
            break;
           }
       default: {
            stringstream ss;
            ss << "Invalid material identifier 0x" << hex << id << dec << ".";
            //return error(ss);
            // TODO: DELETE ME (INSTEAD OF THROWING AN ERROR ON AN UNSEEN MATERIAL, JUST DO THIS FOR NOW)
            bs.seek(0x20);
            uint32 texture_id = bs.BE_read_uint32();
            if(bs.fail()) return error("Stream read failure.");
            item.texture_id = texture_id;
            error(ss);
            // TODO: DELETE ME
           }
      }

     // debug information
     if(debug) {
        switch(id) {
          // material requires colorization
          case(0x24400099): {
               dfile << "------------------" << endl;
               dfile << "-- COLORIZATION --" << endl;
               dfile << "------------------" << endl;
               dfile << endl;
               bs.seek(0x020); uint32 t1 = bs.BE_read_uint32(); // diffuse
               bs.seek(0x038); uint32 t2 = bs.BE_read_uint32(); // normal
               bs.seek(0x050); uint32 t3 = bs.BE_read_uint32(); // specular
               bs.seek(0x068); uint32 t4 = bs.BE_read_uint32(); // masking map
               bs.seek(0x090); real32 c1[4]; bs.BE_read_array(&c1[0], 4);
               bs.seek(0x0B0); real32 c2[4]; bs.BE_read_array(&c2[0], 4);
               bs.seek(0x0D0); real32 c3[4]; bs.BE_read_array(&c3[0], 4);
               bs.seek(0x0F0); real32 c4[4]; bs.BE_read_array(&c4[0], 4);
               bs.seek(0x110); real32 c5[4]; bs.BE_read_array(&c5[0], 4);
               dfile << "mesh_" << setfill('0') << setw(3) << entrylist2[i].mesh_id << "_";
               dfile << "part_" << setfill('0') << setw(3) << entrylist2[i].part_id << endl;
               dfile << " diffuse: 0x" << hex << t1 << dec << endl;
               dfile << " normal: 0x" << hex << t2 << dec << endl;
               dfile << " specular: 0x" << hex << t3 << dec << endl;
               dfile << " colorization mask: 0x" << hex << t4 << dec << endl;
               dfile << " color[0]: " << (uint32)(c1[0]*255) << ", " << (uint32)(c1[1]*255) << ", " << (uint32)(c1[2]*255) << ", " << c1[3] << endl;
               dfile << " color[1]: " << (uint32)(c2[0]*255) << ", " << (uint32)(c2[1]*255) << ", " << (uint32)(c2[2]*255) << ", " << c2[3] << " (use this one)" << endl;
               dfile << " color[2]: " << (uint32)(c3[0]*255) << ", " << (uint32)(c3[1]*255) << ", " << (uint32)(c3[2]*255) << ", " << c3[3] << " (this one might be important too)" << endl;
               dfile << " color[3]: " << (uint32)(c4[0]*255) << ", " << (uint32)(c4[1]*255) << ", " << (uint32)(c4[2]*255) << ", " << c4[3] << endl;
               dfile << " color[4]: " << (uint32)(c5[0]*255) << ", " << (uint32)(c5[1]*255) << ", " << (uint32)(c5[2]*255) << ", " << c5[3] << endl;
               dfile << endl;
               break;
              }
          case(0x24600099) :
          case(0x24602199) : {
               dfile << "------------------" << endl;
               dfile << "-- COLORIZATION --" << endl;
               dfile << "------------------" << endl;
               dfile << endl;
               bs.seek(0x020); uint32 t1 = bs.BE_read_uint32(); // diffuse
               bs.seek(0x038); uint32 t2 = bs.BE_read_uint32(); // normal
               bs.seek(0x050); uint32 t3 = bs.BE_read_uint32(); // specular
               bs.seek(0x068); uint32 t4 = bs.BE_read_uint32(); // masking map
               bs.seek(0x090); real32 c1[4]; bs.BE_read_array(&c1[0], 4);
               bs.seek(0x0B0); real32 c2[4]; bs.BE_read_array(&c2[0], 4);
               bs.seek(0x0D0); real32 c3[4]; bs.BE_read_array(&c3[0], 4);
               bs.seek(0x0F0); real32 c4[4]; bs.BE_read_array(&c4[0], 4);
               bs.seek(0x110); real32 c5[4]; bs.BE_read_array(&c5[0], 4);
               bs.seek(0x130); real32 c6[4]; bs.BE_read_array(&c6[0], 4);
               dfile << "mesh_" << setfill('0') << setw(3) << entrylist2[i].mesh_id << "_";
               dfile << "part_" << setfill('0') << setw(3) << entrylist2[i].part_id << endl;
               dfile << " diffuse: 0x" << hex << t1 << dec << endl;
               dfile << " normal: 0x" << hex << t2 << dec << endl;
               dfile << " specular: 0x" << hex << t3 << dec << endl;
               dfile << " colorization mask: 0x" << hex << t4 << dec << endl;
               dfile << " color[0]: " << (uint32)(c1[0]*255) << ", " << (uint32)(c1[1]*255) << ", " << (uint32)(c1[2]*255) << ", " << c1[3] << " (???)" << endl;
               dfile << " color[1]: " << (uint32)(c2[0]*255) << ", " << (uint32)(c2[1]*255) << ", " << (uint32)(c2[2]*255) << ", " << c2[3] << " (???)" << endl;
               dfile << " color[2]: " << (uint32)(c3[0]*255) << ", " << (uint32)(c3[1]*255) << ", " << (uint32)(c3[2]*255) << ", " << c3[3] << " (???)" << endl;
               dfile << " color[3]: " << (uint32)(c4[0]*255) << ", " << (uint32)(c4[1]*255) << ", " << (uint32)(c4[2]*255) << ", " << c4[3] << " (???)" << endl;
               dfile << " color[4]: " << (uint32)(c5[0]*255) << ", " << (uint32)(c5[1]*255) << ", " << (uint32)(c5[2]*255) << ", " << c5[3] << " (???)" << endl;
               dfile << " color[5]: " << (uint32)(c6[0]*255) << ", " << (uint32)(c6[1]*255) << ", " << (uint32)(c6[2]*255) << ", " << c6[3] << " (???)" << endl;
               dfile << endl;
               break;
              }
         }
       }

     // insert item
     entrylist3.push_back(item);
    }

 // move to idx buffer
 uint32 ib_offset = 0x30 + h07;
 ifile.seekg(ib_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read idx buffer
 uint32 ib_size = h08;
 if(!ib_size) return error("Invalid index buffer.");
 boost::shared_array<char> ib_data(new char[ib_size]);
 ifile.read(ib_data.get(), ib_size);
 if(ifile.fail()) return error("Read failure.");

 // move to vtx buffer #1
 uint32 vb1_offset = ib_offset + ib_size;
 ifile.seekg(vb1_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vtx buffer #1
 uint32 vb1_size = h09;
 boost::shared_array<char> vb1_data;
 if(vb1_size) {
    vb1_data.reset(new char[vb1_size]);
    ifile.read(vb1_data.get(), vb1_size);
    if(ifile.fail()) return error("Read failure.");
   }

 // move to vtx buffer #2
 uint32 vb2_offset = vb1_offset + vb1_size;
 ifile.seekg(vb2_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vtx buffer #2
 uint32 vb2_size = h10;
 boost::shared_array<char> vb2_data;
 if(vb2_size) {
    vb2_data.reset(new char[vb2_size]);
    ifile.read(vb2_data.get(), vb2_size);
    if(ifile.fail()) return error("Read failure.");
   } 

 // model container
 ADVANCEDMODELCONTAINER amc;

 // binary streams
 binary_stream ibs(ib_data, ib_size);
 binary_stream vs1(vb1_data, vb1_size);
 binary_stream vs2(vb2_data, vb2_size);

 // for each mesh entry
 for(uint32 i = 0; i < entrylist2.size(); i++)
    {
     // important information
     uint32 ib_offset = entrylist2[i].p01; // offset into ib
     uint32 v1_offset = entrylist2[i].p02; // offset into vb #1
     uint32 v2_offset = entrylist2[i].p03; // offset into vb #2
     uint16 n_vertices = entrylist2[i].p04; // number of vertices
     uint16 vb_format = entrylist2[i].p05; // vertex format
     uint32 mat_index = entrylist2[i].p06; // offset into material property data
     uint32 n_indices = entrylist2[i].p10; // number of indices
     uint16 ib_format = entrylist2[i].p11; // index buffer format

     // initialize vertex buffer
     AMC_VTXBUFFER vb;
     vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
     vb.name = "default";
     vb.uvchan = 1;
     vb.uvtype[0] = AMC_DIFFUSE_MAP;
     vb.uvtype[1] = AMC_INVALID_MAP;
     vb.uvtype[2] = AMC_INVALID_MAP;
     vb.uvtype[3] = AMC_INVALID_MAP;
     vb.uvtype[4] = AMC_INVALID_MAP;
     vb.uvtype[5] = AMC_INVALID_MAP;
     vb.uvtype[6] = AMC_INVALID_MAP;
     vb.uvtype[7] = AMC_INVALID_MAP;
     vb.colors = 0;
     vb.elem = n_vertices;
     vb.data.reset(new AMC_VERTEX[vb.elem]);

     // read vertex data
     if(vb_format == 0x610)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV
           }
       }
     else if(vb_format == 0x612)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08(); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }
       }
     else if(vb_format == 0x710)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }
       }
     else if(vb_format == 0x712)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08(); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }
       }
     else if(vb_format == 0x1110)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32(); // ???
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;

            // invalidate indices with zero weights
            if(vb.data[j].wv[1] == 0.0f) vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[2] == 0.0f) vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[3] == 0.0f) vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
           }
       }
     else if(vb_format == 0x1112)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.colors = 1;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].color[0][0] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][1] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][2] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][3] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32(); // ???
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;

            // invalidate indices with zero weights
            if(vb.data[j].wv[1] == 0.0f) vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[2] == 0.0f) vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[3] == 0.0f) vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
           }
       }
     // new to 1.02 update
     // data062\AC1_0002\017.NDP3
     else if(vb_format == 0x1114)
       {
        // UV map data is 0x0C bytes per vertex
        // vertex data is 0x40 bytes per vertex

        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.colors = 1;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            // color
            vb.data[j].color[0][0] = vs1.BE_read_real16(); // color?
            vb.data[j].color[0][1] = vs1.BE_read_real16(); // color?
            vb.data[j].color[0][2] = vs1.BE_read_real16(); // color?
            vb.data[j].color[0][3] = vs1.BE_read_real16(); // color?
            // UV
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32(); // ???
            vb.data[j].nx = vs2.BE_read_real32(); // normal
            vb.data[j].ny = vs2.BE_read_real32(); // normal
            vb.data[j].nz = vs2.BE_read_real32(); // normal
            vs2.BE_read_real32(); // ???
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;

            // invalidate indices with zero weights
            if(vb.data[j].wv[0] == 0.0f) vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[1] == 0.0f) vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[2] == 0.0f) vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[3] == 0.0f) vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
           }
       }
     else if(vb_format == 0x1310)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32();
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;

            // invalidate indices with zero weights
            if(vb.data[j].wv[1] == 0.0f) vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[2] == 0.0f) vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[3] == 0.0f) vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
           }
       }
     else if(vb_format == 0x1312)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.colors = 1;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].color[0][0] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][1] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][2] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][3] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            // these numbers can be large
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vb.data[j].vw = vs2.BE_read_real32(); // 0x42C00000
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;

            // invalidate indices with zero weights
            if(vb.data[j].wv[1] == 0.0f) vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[2] == 0.0f) vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[3] == 0.0f) vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
           }
       }
     else if(vb_format == 0x1322)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.colors = 1;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.");

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].color[0][0] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][1] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][2] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][3] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
            vb.data[j].uv[1][0] = vs1.BE_read_real16();
            vb.data[j].uv[1][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.");

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32(); // 0x42C00000
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;

            // invalidate indices with zero weights
            if(vb.data[j].wv[1] == 0.0f) vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[2] == 0.0f) vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
            if(vb.data[j].wv[3] == 0.0f) vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
           }
       }
     else {
        stringstream ss;
        ss << "Unknown vertex format 0x" << hex << vb_format << dec << endl;
        return error(ss);
       }

     // insert vertex buffer
     amc.vblist.push_back(vb);

     // initialize index buffer
     AMC_IDXBUFFER ib;
     ib.format = AMC_UINT16;
     ib.type = AMC_TRISTRIPCUT;
     ib.wind = AMC_CW;
     ib.reserved = 0;
     ib.name = "default";
     ib.elem = n_indices;

     // seek index buffer data
     ibs.seek(ib_offset);
     if(ibs.fail()) return error("Stream seek failure.");

     // WHY THE FUCK DID I EVER ADD THIS?
     // // adjust number of indices
     // if(n_indices == 0) {
     //    n_indices = ib_size/2;
     //    message("Warning: Number of indices is equal to 0.");
     //   }

     // read index buffer data
     ib.data.reset(new char[n_indices*sizeof(uint16)]);
     ibs.BE_read_array(reinterpret_cast<uint16*>(ib.data.get()), n_indices);
     if(ibs.fail()) return error("Stream read failure.");

     // convert triangle strip cut to triangle list
     AMCTriangleList trilist;
     if(!ConvertStripCutToTriangleList(reinterpret_cast<uint16*>(ib.data.get()), n_indices, trilist, (uint16)0xFFFF))
        return error("Failed to convert triangle strip to list.");
  
     // adjust index buffer
     ib.type = AMC_TRIANGLES;
     ib.elem = trilist.n_indices;
     ib.data = trilist.data;

     // insert index buffer
     amc.iblist.push_back(ib);

     // create surface name
     stringstream mns;
     mns << "mesh_" << setfill('0') << setw(3) << entrylist2[i].mesh_id << "_";
     mns << "part_" << setfill('0') << setw(3) << entrylist2[i].part_id;

     // initialize surface
     AMC_SURFACE sur;
     sur.name = mns.str();
     AMC_REFERENCE ref;
     ref.vb_index = i;
     ref.vb_start = 0;
     ref.vb_width = n_vertices;
     ref.ib_index = i;
     ref.ib_start = 0;
     ref.ib_width = trilist.n_indices; // n_indices;
     ref.jm_index = AMC_INVALID_JMAP_INDEX;
     sur.refs.push_back(ref);
     sur.surfmat = i; // AMC_INVALID_SURFMAT;

     // insert surface
     amc.surfaces.push_back(sur);

     // initialize and insert filename
     stringstream sstex;
     sstex << hex << setfill('0') << setw(8) << entrylist3[i].texture_id << dec << ".dds";
     AMC_IMAGEFILE image;
     image.filename = sstex.str();
     amc.iflist.push_back(image);

     // initialize and insert material
     AMC_SURFMAT mat;
     mat.name = mns.str();
     mat.twoside = 0;
     mat.unused1 = 0;
     mat.unused2 = 0;
     mat.unused3 = 0;
     mat.basemap = i;
     mat.specmap = AMC_INVALID_TEXTURE;
     mat.tranmap = AMC_INVALID_TEXTURE;
     mat.bumpmap = AMC_INVALID_TEXTURE;
     mat.normmap = AMC_INVALID_TEXTURE;
     mat.lgthmap = AMC_INVALID_TEXTURE;
     mat.envimap = AMC_INVALID_TEXTURE;
     mat.glssmap = AMC_INVALID_TEXTURE;
     mat.resmap1 = AMC_INVALID_TEXTURE;
     mat.resmap2 = AMC_INVALID_TEXTURE;
     mat.resmap3 = AMC_INVALID_TEXTURE;
     mat.resmap4 = AMC_INVALID_TEXTURE;
     mat.resmap5 = AMC_INVALID_TEXTURE;
     mat.resmap6 = AMC_INVALID_TEXTURE;
     mat.resmap7 = AMC_INVALID_TEXTURE;
     mat.resmap8 = AMC_INVALID_TEXTURE;
     mat.basemapchan = 0;
     mat.specmapchan = 0xFF;
     mat.tranmapchan = 0xFF;
     mat.bumpmapchan = 0xFF;
     mat.normmapchan = 0xFF;
     mat.lghtmapchan = 0xFF;
     mat.envimapchan = 0xFF;
     mat.glssmapchan = 0xFF;
     mat.resmapchan1 = 0xFF;
     mat.resmapchan2 = 0xFF;
     mat.resmapchan3 = 0xFF;
     mat.resmapchan4 = 0xFF;
     mat.resmapchan5 = 0xFF;
     mat.resmapchan6 = 0xFF;
     mat.resmapchan7 = 0xFF;
     mat.resmapchan8 = 0xFF;
     amc.surfmats.push_back(mat);
    }

 SaveAMC(pathname.c_str(), shrtname.c_str(), amc);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

bool processBONE(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug file
 bool debug = true;
 ofstream dfile;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT(".debug");
    dfile.open(ss.str().c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x424F4E45) return error("Not a BONE file.");

 // read number of joints
 ifile.seekg(0x10);
 uint32 n_joints = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(n_joints == 0) return true;

 // read offsets
 ifile.seekg(0x18);
 uint32 offset1 = LE_read_uint32(ifile); // parent list
 uint32 offset2 = LE_read_uint32(ifile); // positions
 uint32 offset3 = LE_read_uint32(ifile); // angles
 uint32 offset4 = LE_read_uint32(ifile); // ???
 uint32 offset5 = LE_read_uint32(ifile); // ???
 uint32 offset6 = LE_read_uint32(ifile); // names
 uint32 offset7 = LE_read_uint32(ifile); // ???
 if(ifile.fail()) return error("Read failure.");

 // read parent list
 ifile.seekg(offset1);
 boost::shared_array<uint32> plist(new uint32[n_joints]);
 if(!LE_read_array(ifile, plist.get(), n_joints)) return error("Read failure.");

 // read positions 1
 if(debug) dfile << "float set #1" << endl;
 ifile.seekg(offset2);
 typedef cs::math::vector3D<real32> vector_t;
 boost::shared_array<vector_t> coords1(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     coords1[i][0] = LE_read_real32(ifile);
     coords1[i][1] = LE_read_real32(ifile);
     coords1[i][2] = LE_read_real32(ifile);
     if(debug) dfile << "pos " << i << ": " << coords1[i][0] << ", " << coords1[i][1] << ", " << coords1[i][2] << endl;
    }
 if(debug) dfile << endl;

 // read positions 2
 if(debug) dfile << "float set #2" << endl;
 ifile.seekg(offset3);
 boost::shared_array<vector_t> coords2(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     coords2[i][0] = LE_read_real32(ifile);
     coords2[i][1] = LE_read_real32(ifile);
     coords2[i][2] = LE_read_real32(ifile);
     if(debug) dfile << coords2[i][0] << ", " << coords2[i][1] << ", " << coords2[i][2] << endl;
    }
 if(debug) dfile << endl;

 // read names
 if(debug) dfile << "names" << endl;
 ifile.seekg(offset6);
 boost::shared_array<string> namelist(new string[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     char buffer[0x20];
     ifile.read(&buffer[0], 0x20);
     namelist[i] = buffer;
     if(debug) dfile << "0x" << hex << i << dec << ": " << buffer << endl;
    }
 if(debug) dfile << endl;

 // read positions 3
 if(debug) dfile << "float set #3" << endl;
 ifile.seekg(offset7);
 boost::shared_array<vector_t> coords3(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     coords3[i][0] = LE_read_real32(ifile);
     coords3[i][1] = LE_read_real32(ifile);
     coords3[i][2] = LE_read_real32(ifile);
     if(debug) dfile << "0x" << hex << i << dec << ": " << coords3[i][0] << ", " << coords3[i][1] << ", " << coords3[i][2] << endl;
    }
 if(debug) dfile << endl;

 // create model container
 ADVANCEDMODELCONTAINER amc;

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = Unicode16ToUnicode08(shrtname.c_str()).get();
 skel.name += "_SKELETON";
 skel.tiplen = 1.0f;

 // process joints
 for(uint32 i = 0; i < n_joints; i++)
    {
     // convert euler to quaternion
     // note: angles are in reverse order in the file!
     cs::math::quaternion<real32> q;
     cs::math::rotation_quaternion_ZYX(&q[0], coords2[i][2], coords2[i][1], coords2[i][0]);
     cs::math::normalize(q);

     // convert quaternion to matrix
     cs::math::matrix4x4<real32> m;
     cs::math::quaternion_to_matrix4x4(&q[0], &m[0]);

     // convert euler angle
     if(debug) {
        dfile << "matrix " << i << ":" << endl;
        dfile << m[0x0] << " " << m[0x1] << " " << m[0x2] << " " << m[0x3] << endl;
        dfile << m[0x4] << " " << m[0x5] << " " << m[0x6] << " " << m[0x7] << endl;
        dfile << m[0x8] << " " << m[0x9] << " " << m[0xA] << " " << m[0xB] << endl;
        dfile << m[0xC] << " " << m[0xD] << " " << m[0xE] << " " << m[0xF] << endl;
        dfile << endl;
       }

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;

     // create joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = plist[i];
     joint.m_rel[0x0] = m[0x0];
     joint.m_rel[0x1] = m[0x1];
     joint.m_rel[0x2] = m[0x2];
     joint.m_rel[0x3] = m[0x3];
     joint.m_rel[0x4] = m[0x4];
     joint.m_rel[0x5] = m[0x5];
     joint.m_rel[0x6] = m[0x6];
     joint.m_rel[0x7] = m[0x7];
     joint.m_rel[0x8] = m[0x8];
     joint.m_rel[0x9] = m[0x9];
     joint.m_rel[0xA] = m[0xA];
     joint.m_rel[0xB] = m[0xB];
     joint.m_rel[0xC] = m[0xC];
     joint.m_rel[0xD] = m[0xD];
     joint.m_rel[0xE] = m[0xE];
     joint.m_rel[0xF] = m[0xF];
     joint.p_rel[0] = coords1[i][0];
     joint.p_rel[1] = coords1[i][1];
     joint.p_rel[2] = coords1[i][2];
     joint.p_rel[3] = 1.0f;

     // insert joint
     skel.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel);

 // save model
 SaveAMC(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

bool processLINK(LPCTSTR filename)
{
 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug text file
 STDSTRING ofname = pathname;
 ofname += TEXT("BONEINFO.TXT");
 ofstream ofile(ofname.c_str());
 if(!ofile) return error("Failed to create file.");

 // find NDP3 files
 deque<STDSTRING> ndp3list;
 BuildFilenameList(ndp3list, TEXT(".NDP3"), pathname.c_str());

 // print out number of joints
 ofile << "MODEL LIST" << endl;
 ofile << endl;
 for(size_t i = 0; i < ndp3list.size(); i++)
    {
     wcout << "Processing file " << (i + 1) << " of " << ndp3list.size() << ": " << ndp3list[i] << "." << endl;

     // open file
     ifstream ifile(ndp3list[i].c_str(), ios::binary);
     if(!ifile) return error("Failed to open file.");
     
     // filename properties
     STDSTRING pathname = GetPathnameFromFilename(ndp3list[i].c_str()).get();
     STDSTRING shrtname = GetShortFilenameWithoutExtension(ndp3list[i].c_str()).get();

     // read header
     uint32 h01 = BE_read_uint32(ifile); // NDP3
     uint32 h02 = BE_read_uint32(ifile); // filesize
     uint16 h03 = BE_read_uint16(ifile); // 0x0201
     uint16 h04 = BE_read_uint16(ifile); // number of 0x30 byte entries
     uint16 h05 = BE_read_uint16(ifile); // unknown
     uint16 h06 = BE_read_uint16(ifile); // number of joints

     // associate filename with number of joints
     uint32 sum = h06 + 1;
     ofile << Unicode16ToUnicode08(shrtname.c_str()).get();
     ofile << ".NPD3 has " << sum << " joints. path is ";
     ofile << Unicode16ToUnicode08(pathname.c_str()).get() << "." << endl;
    }
 ofile << endl;

 // find BONE files
 deque<STDSTRING> bonelist;
 BuildFilenameList(bonelist, TEXT(".BONE"), pathname.c_str());

 // create joint map
 map<uint32, deque<STDSTRING>> bonemap;
 for(size_t i = 0; i < bonelist.size(); i++)
    {
     // display information
     cout << "Processing file " << (i + 1) << " of ";
     cout << bonelist.size() << ": " << Unicode16ToUnicode08(bonelist[i].c_str()).get() << "." << endl;

     // open file
     ifstream ifile(bonelist[i].c_str(), ios::binary);
     if(!ifile) return error("Failed to open file.");
     
     // filename properties
     STDSTRING pathname = GetPathnameFromFilename(bonelist[i].c_str()).get();
     STDSTRING shrtname = GetShortFilenameWithoutExtension(bonelist[i].c_str()).get();

     // read magic
     uint32 magic = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     if(magic != 0x424F4E45) return error("Not a BONE file.");
     
     // read number of joints
     ifile.seekg(0x10);
     uint32 n_joints = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // associate filename with number of joints
     auto iter = bonemap.find(n_joints);
     if(iter == bonemap.end()) {
        deque<STDSTRING> temp;
        temp.push_back(shrtname);
        map<uint32, deque<STDSTRING>>::value_type item(n_joints, temp);
        bonemap.insert(item);
       }
     else {
        iter->second.push_back(shrtname);
       }
    }

 // print list of BONE joints
 ofile << "BONE LIST" << endl;
 ofile << endl;
 for(auto iter = bonemap.begin(); iter != bonemap.end(); iter++) {
     uint32 n = iter->first;
     const deque<STDSTRING>& namelist = iter->second;
     ofile << "NUMBER OF JOINTS = " << n << endl;
     for(uint32 i = 0; i < namelist.size(); i++) {
         ofile << " " << Unicode16ToUnicode08(namelist[i].c_str()).get() << ".BONE. path is ";
         ofile << Unicode16ToUnicode08(pathname.c_str()).get() << "." << endl;
        }
    }
 ofile << endl;

 return true;
}

};};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

/*
bool extract(const char* pname)
{
 // set pathname
 using namespace std;
 std::string pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname();

 // variables
 bool doDAT = true;
 bool doNDP3 = true;
 bool doNTP3 = true;
 bool doBONE = true;
 bool doLINK = true;

 if(doDAT && YesNoBox("Process DAT files?\nClick 'No' if you have already done so."))
   {
    cout << "STAGE 0" << endl;
    cout << "Processing .TXT files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".TXT", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTXT(filelist[i])) return false;
       }
    cout << endl;

    // loop until nothing to do
    for(;;)
       {
        deque<string> filelist;
        uint32 DAT_files = 0;
        uint32 AC1_files = 0;
        uint32 AC2_files = 0;
        uint32 AC3_files = 0;
        uint32 AC4_files = 0;
        uint32 AC5_files = 0;
   
        cout << "STAGE 1" << endl;
        cout << "Processing .DAT files..." << endl;
        BuildFilenameList(filelist, ".DAT", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processDAT(filelist[i])) return false;
           }
        cout << endl;
        DAT_files += filelist.size();
        filelist.clear();
   
        cout << "STAGE 2" << endl;
        cout << "Processing .AC1 files..." << endl;
        BuildFilenameList(filelist, ".AC1", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC1(filelist[i])) return false;
           }
        cout << endl;
        AC1_files += filelist.size();
        filelist.clear();
   
        cout << "STAGE 3" << endl;
        cout << "Processing .AC2 files..." << endl;
        BuildFilenameList(filelist, ".AC2", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC2(filelist[i])) return false;
           }
        cout << endl;
        AC2_files += filelist.size();
        filelist.clear();
        
        cout << "STAGE 4" << endl;
        cout << "Processing .AC3 files..." << endl;
        BuildFilenameList(filelist, ".AC3", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC3(filelist[i])) return false;
           }
        cout << endl;
        AC3_files += filelist.size();
        filelist.clear();
        
        cout << "STAGE 5" << endl;
        cout << "Processing .AC4 files..." << endl;
        BuildFilenameList(filelist, ".AC4", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC4(filelist[i])) return false;
           }
        cout << endl;
        AC4_files += filelist.size();
        filelist.clear();
        
        cout << "STAGE 6" << endl;
        cout << "Processing .AC5 files..." << endl;
        BuildFilenameList(filelist, ".AC5", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC5(filelist[i])) return false;
           }
        cout << endl;
        AC5_files += filelist.size();
   
        // check for nothing to do
        uint32 sum = DAT_files;
        sum += AC1_files;
        sum += AC2_files;
        sum += AC3_files;
        sum += AC4_files;
        sum += AC5_files;
        if(!sum) break;
       }
   }

 //
 // DO NOT LOOP THESE
 //

 cout << "STAGE 7" << endl;
 if(doNDP3 && YesNoBox("Process NDP3 (model) files?\nClick 'No' if you have already done so.")) {
    cout << "Processing .NDP3 files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".NDP3", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processNDP3(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 8" << endl;
 if(doNTP3 && YesNoBox("Process NTP3 (texture) files?\nClick 'No' if you have already done so.")) {
    cout << "Processing .NTP3 files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".NTP3", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processNTP3(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 9" << endl;
 if(doBONE && YesNoBox("Process BONE (skeleton) files?\nClick 'No' if you have already done so."))
   {
    cout << "Processing .BONE files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".BONE", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processBONE(filelist[i])) return false;
       }
    cout << endl;

    cout << "STAGE 10" << endl;
    if(doLINK) {
       cout << "Processing .TXT files again (to associate meshes with skeletons)..." << endl;
       deque<string> filelist;
       BuildFilenameList(filelist, ".TXT", pathname.c_str());
       for(size_t i = 0; i < filelist.size(); i++) {
           cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
           if(!processLINK(filelist[i])) return false;
          }
       cout << endl;
      }
   }

 return true;
}
*/

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PS3] Tekken Revolution");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Extract each PSARC file into its own folder.\n");
 p2 += TEXT("3. Run ripper.\n");
 p2 += TEXT("4. Select game and click Browse.\n");
 p2 += TEXT("5. Select folder where all PSARC folders are.\n");
 p2 += TEXT("6. Click OK and answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need X GB free space.\n");
 p3 += TEXT("3. 8 GB for game + X GB extraction.");

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
 bool doDAT = true;
 bool doNTP = true;
 bool doNDP = true;
 bool doBNE = true;
 bool doLNK = true;

 // questions
 if(doDAT) doDAT = YesNoBox("Process DAT (archive) files?");
 if(doNTP) doNTP = YesNoBox("Process NTP3 (texture) files?");
 if(doNDP) doNDP = YesNoBox("Process NDP3 (model) files?");
 if(doBNE) doBNE = YesNoBox("Process BONE (skeleton) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process DAT files
 if(doDAT)
   {
    cout << "STAGE 0" << endl;
    cout << "Processing .TXT files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".TXT"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTXT(filelist[i].c_str())) return false;
       }
    cout << endl;

    // loop until nothing to do
    for(;;)
       {
        deque<STDSTRING> filelist;
        uint32 DAT_files = 0;
        uint32 AC1_files = 0;
        uint32 AC2_files = 0;
        uint32 AC3_files = 0;
        uint32 AC4_files = 0;
        uint32 AC5_files = 0;
   
        cout << "STAGE 1" << endl;
        cout << "Processing .DAT files..." << endl;
        BuildFilenameList(filelist, TEXT(".DAT"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processDAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        DAT_files += filelist.size();
        filelist.clear();
   
        cout << "STAGE 2" << endl;
        cout << "Processing .AC1 files..." << endl;
        BuildFilenameList(filelist, TEXT(".AC1"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC1(filelist[i].c_str())) return false;
           }
        cout << endl;
        AC1_files += filelist.size();
        filelist.clear();
   
        cout << "STAGE 3" << endl;
        cout << "Processing .AC2 files..." << endl;
        BuildFilenameList(filelist, TEXT(".AC2"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC2(filelist[i].c_str())) return false;
           }
        cout << endl;
        AC2_files += filelist.size();
        filelist.clear();
        
        cout << "STAGE 4" << endl;
        cout << "Processing .AC3 files..." << endl;
        BuildFilenameList(filelist, TEXT(".AC3"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC3(filelist[i].c_str())) return false;
           }
        cout << endl;
        AC3_files += filelist.size();
        filelist.clear();
        
        cout << "STAGE 5" << endl;
        cout << "Processing .AC4 files..." << endl;
        BuildFilenameList(filelist, TEXT(".AC4"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC4(filelist[i].c_str())) return false;
           }
        cout << endl;
        AC4_files += filelist.size();
        filelist.clear();
        
        cout << "STAGE 6" << endl;
        cout << "Processing .AC5 files..." << endl;
        BuildFilenameList(filelist, TEXT(".AC5"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC5(filelist[i].c_str())) return false;
           }
        cout << endl;
        AC5_files += filelist.size();
   
        // check for nothing to do
        uint32 sum = DAT_files;
        sum += AC1_files;
        sum += AC2_files;
        sum += AC3_files;
        sum += AC4_files;
        sum += AC5_files;
        if(!sum) break;
       }
   }

 // process NTP3 files
 if(doNTP) {
    cout << "STAGE 7" << endl;
    if(!NAMCO::PathExtractNTP3(pathname.c_str(), false, 0))
       return false;
   }

 // process NDP3 files
 if(doNDP) {
    cout << "STAGE 8" << endl;
    if(!NAMCO::PathExtractNDP3(pathname.c_str(), false, 0))
       return false;
   }

 // process BONE files
 if(doBNE)
   {
    cout << "STAGE 9" << endl;
    cout << "Processing .BONE files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BONE"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processBONE(filelist[i].c_str())) return false;
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

