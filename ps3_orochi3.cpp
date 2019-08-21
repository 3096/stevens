#include "xentax.h"
#include "x_zlib.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_alg.h"
#include "x_dds.h"
#include "x_smc.h"
#include "ps3_orochi3.h"

#define X_SYSTEM XBOX360
#define X_GAME   Orochi3

namespace X_SYSTEM { namespace X_GAME {

#define GUSELESS  0x00
#define G1TG0050  0x01
#define G1TG0060  0x02
#define G1CT0001  0x03
#define G1FX0000  0x04
#define G1M_0034  0x05
#define G1EM0000  0x06
#define SLO_0000  0x07
#define WBD_0000  0x08
#define WBH_0000  0x09
#define KSHL0117  0x0A
#define KPS_0013  0x0B
#define PNG_0000  0x0C
#define G1A_0042  0x0D
#define G2A_0020  0x0E
#define KTFK0260  0x0F
#define RIFFWAVE  0x10
#define LX000013  0x11
#define LX000014  0x12
#define UNK00001  0x13
#define PAK00000  0x14
#define OSTG0000  0x15
#define O2OB0000  0x16
#define COLK0000  0x17
#define PK100000  0x18
#define PK200000  0x19
#define PK300000  0x1A
#define GV1S0000  0x1B

struct G1MF0021_ITEM {
 uint32 elem;
 boost::shared_array<uint32> data;
};

class extractor {
 private :
  std::string pathname;
  std::deque<std::string> binlist;
 private :
  bool debug;
  std::ofstream dfile;
 private :
  char getFileType(std::ifstream& ifile)const;
  bool pk1Test(std::ifstream& ifile)const;
  bool pk2Test(std::ifstream& ifile)const;
  bool pk3Test(std::ifstream& ifile)const;
 private :
  bool processIDX(const std::string& filename);
  bool processZRC(const std::string& filename);
  bool processBIN(const std::string& filename, bool separate = false);
  bool processPK1(const std::string& filename);
  bool processPK2(const std::string& filename);
  bool processPK3(const std::string& filename);
  bool processDAT(const std::string& filename);
  bool processG1T(const std::string& filename);
  bool processG1M(const std::string& filename);
 public :
  bool extract(void);
 public :
  extractor(const char* pn) : pathname(pn) {}
 ~extractor() {}
};

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
  std::deque<G1MF0021_ITEM> g1mfdata;
  SIMPLEMODELCONTAINER2 smc;
 private :
  bool processG1MF0021(boost::shared_array<char> data, uint32 size);
  bool processG1MS0032(boost::shared_array<char> data, uint32 size);
  bool processG1MM0020(boost::shared_array<char> data, uint32 size);
  bool processG1MG0044(boost::shared_array<char> data, uint32 size);
 private :
  bool processSECT0101(boost::shared_array<char> data, uint32 size);
  bool processSECT0102(boost::shared_array<char> data, uint32 size);
  bool processSECT0103(boost::shared_array<char> data, uint32 size);
  bool processSECT0104(boost::shared_array<char> data, uint32 size);
  bool processSECT0105(boost::shared_array<char> data, uint32 size);
  bool processSECT0106(boost::shared_array<char> data, uint32 size);
  bool processSECT0107(boost::shared_array<char> data, uint32 size);
  bool processSECT0108(boost::shared_array<char> data, uint32 size);
  bool processSECT0109(boost::shared_array<char> data, uint32 size);
 public :
  bool extract(void);
 public :
  modelizer(const char* fname);
 ~modelizer();
};

};};

namespace X_SYSTEM { namespace X_GAME {

bool extractor::extract(void)
{
 using namespace std;
 debug = true;

 bool unpackA = false;
 bool unpackB = false;
 bool doZRC = false;
 bool doBIN = false;
 bool doG1T = false;
 bool doG1M = false;

 cout << "STAGE 1" << endl;
 if(unpackA) {
    cout << "Unpacking LINKDATA files..." << endl;
    processIDX("LINKDATA");
    cout << endl;
   }

 cout << "STAGE 2" << endl;
 if(unpackB) {
    cout << "Unpacking LINKDATA2 files..." << endl;
    processIDX("LINKDATA2");
    cout << endl;
   }

 cout << "STAGE 3" << endl;
 if(doZRC) {
    cout << "Unpacking .ZRC files..." << endl;
    stringstream ss;
    ss << pathname << "LINKDATA" << "\\";
    deque<string> filelist;
    BuildFilenameList(filelist, ".zrc", ss.str().c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processZRC(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 4" << endl;
 if(doBIN) {
    cout << "Processing .BIN files..." << endl;
    stringstream ss;
    ss << pathname << "LINKDATA" << "\\";
    deque<string> filelist;
    BuildFilenameList(filelist, ".bin", ss.str().c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processBIN(filelist[i], true)) return false;
       }
    cout << endl;
   }

 uint32 counter = 1;
 for(;;)
    {
     stringstream ss;
     ss << pathname << "LINKDATA" << "\\";
     deque<string> pk1list;
     deque<string> pk2list;
     deque<string> pk3list;
     BuildFilenameList(pk1list, ".pk1", ss.str().c_str());
     BuildFilenameList(pk2list, ".pk2", ss.str().c_str());
     BuildFilenameList(pk3list, ".pk3", ss.str().c_str());

     // stop when no archives are left
     if(pk1list.size() == 0 && pk2list.size() == 0 && pk3list.size() == 0)
        break;

     cout << "STAGE 5: " << counter << endl;
     cout << "Processing .pk1 files..." << endl;
     for(size_t i = 0; i < pk1list.size(); i++) {
         cout << "Processing file " << (i + 1) << " of " << pk1list.size() << ": " << pk1list[i] << "." << endl;
         if(!processPK1(pk1list[i])) return false;
         DeleteFileA(pk1list[i].c_str());
        }
     pk1list.clear();
     cout << endl;

     cout << "STAGE 6: " << counter << endl;
     cout << "Processing .pk2 files..." << endl;
     for(size_t i = 0; i < pk2list.size(); i++) {
         cout << "Processing file " << (i + 1) << " of " << pk2list.size() << ": " << pk2list[i] << "." << endl;
         if(!processPK2(pk2list[i])) return false;
         DeleteFileA(pk2list[i].c_str());
        }
     pk2list.clear();
     cout << endl;

     cout << "STAGE 7: " << counter << endl;
     cout << "Processing .pk3 files..." << endl;
     for(size_t i = 0; i < pk3list.size(); i++) {
         cout << "Processing file " << (i + 1) << " of " << pk3list.size() << ": " << pk3list[i] << "." << endl;
         if(!processPK3(pk3list[i])) return false;
         DeleteFileA(pk3list[i].c_str());
        }
     pk3list.clear();
     cout << endl;

     cout << "STAGE 8: " << counter << endl;
     cout << "Processing .BIN files..." << endl;
     deque<string> binlist;
     BuildFilenameList(binlist, ".bin", ss.str().c_str());
     for(size_t i = 0; i < binlist.size(); i++) {
         cout << "Processing file " << (i + 1) << " of " << binlist.size() << ": " << binlist[i] << "." << endl;
         if(!processBIN(binlist[i])) return false;
        }
     binlist.clear();
     cout << endl;
    }

 cout << "EXTRACTION DONE!!!" << endl;

/*
 cout << "STAGE 8" << endl;
 if(doG1T) {
    cout << "Unpacking .g1t files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".g1t", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processG1T(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 9" << endl;
 if(doG1M) {
    cout << "Unpacking .g1m files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".g1m", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processG1M(filelist[i])) return false;
       }
    cout << endl;
   }
*/
 return true;
}

char extractor::getFileType(std::ifstream& ifile)const
{
 // get filesize
 using namespace std;
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x10) return GUSELESS;

 // read 1st uint32
 uint32 head01 = BE_read_uint32(ifile);
 if(ifile.fail()) return GUSELESS;

 // read 2nd uint32
 uint32 head02 = BE_read_uint32(ifile);
 if(ifile.fail()) return GUSELESS;

 // tests
 if(head01 == 0x47314D5F && head02 == 0x30303334) return G1M_0034; // G1M_0034
 else if(head01 == 0x47315447 && head02 == 0x30303530) return G1TG0050; // G1TG0050
 else if(head01 == 0x47315447 && head02 == 0x30303630) return G1TG0060; // G1TG0060
 else if(head01 == 0x47314354) return G1CT0001; // G1CT0001
 else if(head01 == 0x4731454D) return G1EM0000; // G1EM0000
 else if(head01 == 0x47314658) return G1FX0000; // G1FX0000
 else if(head01 == 0x4731415F) return G1A_0042; // G1A_0042
 else if(head01 == 0x4732415F) return G2A_0020; // G2A_0020
 else if(head01 == 0x534C4F5F) return SLO_0000; // SLO_0000
 else if(head01 == 0x5742445F) return WBD_0000; // WBD_0000
 else if(head01 == 0x5742485F) return WBH_0000; // WBH_0000
 else if(head01 == 0x4B53484C) return KSHL0117; // KSHL0117
 else if(head01 == 0x4B50535F) return KPS_0013; // KPS_0013
 else if(head01 == 0x89504E47) return PNG_0000; // PNG_0000
 else if(head01 == 0x52494646) return RIFFWAVE; // RIFFWAVE
 else if(head01 == 0x4B54464B) return KTFK0260; // KTFK0260
 else if(head01 == 0x47315653) return GV1S0000;
 else if(head01 == 0x4C580013) return LX000013;
 else if(head01 == 0x4C580014) return LX000014;
 else if(head01 == 0x4F535447) return OSTG0000;
 else if(head01 == 0x4F324F42) return O2OB0000;
 else if(head01 == 0x434F4C4B) return COLK0000;
 else if(head01 == 0x11111111) return GUSELESS; // useless crap
 else if(head01 == 0x22222222) return GUSELESS; // useless crap
 else if(head01 == 0x33333333) return GUSELESS; // useless crap
 else if(head01 == 0x44444444) return GUSELESS; // useless crap
 else if((head01 & 0xFFFF0000) == 0x00050000) return GUSELESS; // useless crap
 else if((head01 & 0xFFFF0000) == 0x00800000) return GUSELESS; // useless crap
 else if(pk1Test(ifile)) return PK100000; // PK1 TEST
 else if(pk2Test(ifile)) return PK200000; // PK2 TEST
 else if(pk3Test(ifile)) return PK300000; // PK3 TEST

 return GUSELESS;
}

bool extractor::pk1Test(std::ifstream& ifile)const
{
 // PK1 FILES
 // 00 00 00 10 offset #1
 // 00 00 19 3C length #1
 // 00 00 19 4C offset #2
 // 00 03 40 0C length #2
 // filesize = 0x35958

 // move to beginning
 using namespace std;
 ifile.seekg(0x00);
 if(ifile.fail()) return false;

 // compute filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filesize must be greater than 0x10
 if(filesize < 0x10) return false;

 // read first offset
 uint32 curr_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // validate first offset
 if(!(curr_offset < 0xFFFF)) return false; // must be less than 0xFF
 if(!(curr_offset < filesize)) return false; // must be less than filesize

 // read first length
 uint32 curr_length = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(curr_length == 0) return false; // must not be zero

 // compute next offset
 uint32 curr_finish = curr_offset + curr_length;
 if((curr_offset == 0x08) && (curr_finish == filesize)) return true; // last file
 if(!(curr_finish < filesize)) return false; // must be less than filesize

 // repeat
 for(;;)
    {
     // save previous data
     uint32 prev_offset = curr_offset;
     uint32 prev_length = curr_length;
     uint32 prev_finish = curr_finish;

     // read offset
     curr_offset = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // offset check #1
     if(curr_offset == 0) break; // finished

     // offset check #2
     if(curr_offset < filesize) ; // OK
     else if(filesize < curr_offset) return false; // must be less than filesize
     else return false; // must be less than filesize

     // offset check #3      
     if(prev_finish < curr_offset) { // files cannot have too much padding
        uint32 padding = curr_offset - prev_finish;
        if(!(padding < 0x10)) return false;
       }
     else if(curr_offset < prev_finish) return false; // files cannot overlap

     // read length
     curr_length = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // length check #1
     if(curr_length == 0) return false; // can't be zero

     // length check #2
     curr_finish = curr_offset + curr_length;
     if(curr_finish < filesize) { // OK, might be done
        uint32 padding = filesize - curr_finish;
        if(padding < 0x10) return true;
       }
     else if(filesize < curr_finish) return false; // must be less than or equal to filesize
     else return true; // DONE
    }

 return false;
}

bool extractor::pk2Test(std::ifstream& ifile)const
{
 // PK2 FILES
 // 00 00 00 05 number of files
 // 00 04 01 80 length of data (0x10 aligned)
 // 00 0A AA E0 length of data (0x10 aligned)
 // 00 00 0C F0 length of data (0x10 aligned)
 // 00 9A AB 70 length of data (0x10 aligned)
 // 00 0F 72 40 length of data (0x10 aligned)
 // 00 00 00 00 padding
 // 00 00 00 00 padding
 // data

 // move to beginning
 using namespace std;
 ifile.seekg(0x00);
 if(ifile.fail()) return false;

 // compute filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filesize must be greater than 0x10
 if(filesize < 0x10) return false;

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // validate
 if(n_files == 0) return false;
 if(!(n_files < 0xFFFF)) return false;
 if(filesize < (n_files + 1)*sizeof(uint32)) return false;

 // read filesizes
 boost::shared_array<uint32> sizes(new uint32[n_files]);
 BE_read_array(ifile, sizes.get(), n_files);
 if(ifile.fail()) return false;

 // header size
 uint32 bytes_read = 0x04;
 bytes_read += n_files*sizeof(uint32);
 bytes_read = ((bytes_read + 0x0F) & ~(0x0F));

 // sum of filesizes
 const uint32 total_size = filesize - bytes_read;
 bytes_read = 0;

 // check first filesize
 if(sizes[0] == 0) return false; // first one can't be 0
 if(total_size < sizes[0]) return false; // first one must be less than or equal to total size
 if(sizes[0] % 16) return false; // first one must be divisible by 16
 bytes_read += sizes[0];

 // check other filesizes
 for(uint32 i = 1; i < n_files; i++) {
     if(sizes[i] && (sizes[i] % 16)) return false; // must be divisible by 16
     if(total_size < sizes[i]) return false; // must be less than or equal to the total size
     bytes_read += sizes[i];
    }

 return (bytes_read == total_size);
}

bool extractor::pk3Test(std::ifstream& ifile)const
{
 // PK3 FILES
 // 00 00 00 02 00 00 00 14 00 00 00 DA 00 00 00 F0 00 00 00 06
 // 0x00000002 number of files
 // 0x00000014 offset #1
 // 0x000000DA length #1 possibly 0, possibly padding
 // 0x000000F0 offset #2 
 // 0x00000006 length #2 possibly 0, possibly padding

 //#define DO_FAIL { std::cout << "Failure in " << __FILE__ << " at line " << __LINE__ << "." << std::endl; return false; }
 #define DO_FAIL return false;

 // move to beginning
 using namespace std;
 ifile.seekg(0x00);
 if(ifile.fail()) DO_FAIL

 // compute filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filesize must be greater than 0x10
 if(filesize < 0x10) DO_FAIL

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) DO_FAIL

 // validate number of files
 if(n_files == 0) DO_FAIL
 if(!(n_files < 0xFFFF)) DO_FAIL

 // read first offset
 uint32 curr_offset = BE_read_uint32(ifile);
 if(ifile.fail()) DO_FAIL

 // validate first offset
 if(!(curr_offset < 0xFFFF)) DO_FAIL // must be less than 0xFF
 if(!(curr_offset < filesize)) DO_FAIL // must be less than filesize

 // read first length
 uint32 curr_length = BE_read_uint32(ifile);
 if(ifile.fail()) DO_FAIL
 if(curr_length == 0) DO_FAIL // must not be zero

 // compute next offset
 uint32 curr_finish = curr_offset + curr_length;
 if((n_files == 1) && (curr_offset == 0x0C)) {
    if((curr_finish < filesize) && (filesize - curr_finish < 0x10)) return true; // a little padding
    else if(curr_finish > filesize) DO_FAIL // first file too big
    if(curr_finish == filesize) return true; // perfect
   }
 if(!(curr_finish < filesize)) DO_FAIL // must be less than filesize

 // for other files
 for(uint32 i = 1; i < n_files; i++)
    {
     // save previous data
     uint32 prev_offset = curr_offset;
     uint32 prev_length = curr_length;
     uint32 prev_finish = curr_finish;

     // read offset
     curr_offset = BE_read_uint32(ifile);
     if(ifile.fail()) DO_FAIL

     // offset check #1
     if(curr_offset == 0) break; // finished

     // offset check #2
     if(curr_offset < filesize) ; // OK
     else if(filesize < curr_offset) DO_FAIL // must be less than filesize
     else DO_FAIL // must be less than filesize

     // offset check #3      
     if(prev_finish < curr_offset) { // files cannot have too much padding
        uint32 padding = curr_offset - prev_finish;
        if(!(padding < 0x10)) DO_FAIL
       }
     else if(curr_offset < prev_finish) DO_FAIL // files cannot overlap

     // read length
     curr_length = BE_read_uint32(ifile);
     if(ifile.fail()) DO_FAIL

     // length check #1
     if(curr_length == 0) DO_FAIL // could be zero, but when it is, archive is useless

     // length check #2
     curr_finish = curr_offset + curr_length;
     if(curr_finish < filesize) { // OK, might be done
        uint32 padding = filesize - curr_finish;
        if(padding < 0x10) return true;
       }
     else if(filesize < curr_finish) DO_FAIL // must be less than or equal to filesize
     else return true; // DONE
    }

 return false;
}

bool extractor::processIDX(const std::string& filename)
{
 // generate filenames
 using namespace std;
 string ifname1 = pathname;
 string ifname2 = pathname;
 ifname1 += filename;
 ifname2 += filename;
 ifname1 += ".IDX";
 ifname2 += ".BIN";

 // create destination directory
 stringstream ss;
 ss << pathname << "LINKDATA" << "\\";
 string destpath = ss.str();
 CreateDirectoryA(destpath.c_str(), NULL);

 // open idx file
 ifstream idxfile;
 idxfile.open(ifname1.c_str(), ios::binary);
 if(!idxfile) return error("Could not open file.");

 // open bin file
 ifstream binfile;
 binfile.open(ifname2.c_str(), ios::binary);
 if(!binfile) return error("Could not open file.");

 // structure to read file offsets
 struct IDXITEM {
  uint64 p1; // offset
  uint64 p2; // length (uncompressed?)
  uint64 p3; // length (compressed)
  uint64 p4; // filetype
  string filename;
 };

 // read file offsets
 uint32 index = 0;
 for(;;)
    {
     IDXITEM item;
     item.p1 = BE_read_uint64(idxfile); if(idxfile.fail()) break;
     item.p2 = BE_read_uint64(idxfile); if(idxfile.fail()) break;
     item.p3 = BE_read_uint64(idxfile); if(idxfile.fail()) break;
     item.p4 = BE_read_uint64(idxfile); if(idxfile.fail()) break;

     // validate item
     if((item.p4 == 0) && (item.p2 != item.p3)) {
        stringstream ss;
        ss << "IDX file lengths do not match at offset 0x" << hex << idxfile.tellg() << dec << ".";
        return error(ss.str().c_str());
       }

     // build filename
     stringstream ss;
     ss << destpath << setfill('0') << setw(4) << index << ".";
     if(item.p4 == 0) ss << "bin";
     else if(item.p4 == 1) ss << "zrc";
     else return error("Unknown IDX file type.");
     item.filename = ss.str();
     cout << " " << item.filename << endl;

     // something to save
     if(item.p3 != 0)
       {
        // save item
        ofstream ofile(item.filename.c_str(), ios::binary);
        if(!ofile) return error("Failed to create file.");

        // 16 MB buffer
        boost::shared_array<char> data(new char[0x1000000]);
        binfile.seekg(item.p1);
   
        // transfer data
        uint64 bytes_remaining = item.p3;
        while(bytes_remaining) {
              if(bytes_remaining < 0x1000000) {
                 binfile.read(data.get(), bytes_remaining);
                 if(binfile.fail()) return error("Read failure.");
                 ofile.write(data.get(), bytes_remaining);
                 if(ofile.fail()) return error("Write failure.");
                 bytes_remaining = 0;
                }
              else {
                 binfile.read(data.get(), 0x1000000);
                 if(binfile.fail()) return error("Read failure.");
                 ofile.write(data.get(), 0x1000000);
                 if(ofile.fail()) return error("Write failure.");
                 bytes_remaining -= 0x1000000;
                }
             }
       }

     // insert item
     index++;
    }

 cout << endl;
 return true;
}

bool extractor::processZRC(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filesize can be 1
 if(filesize < 0x08) {
    ifile.close();
    DeleteFileA(filename.c_str());
    return true;
   }

 // read magic (0x10000 or 0x18000)
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Invalid ZRC file.");

 // read number of files (it is possible to have 0 files)
 uint32 files = BE_read_uint32(ifile);
 if(files == 0) {
    ifile.close();
    DeleteFileA(filename.c_str());
    return true;
   }

 // read unknown
 BE_read_uint32(ifile);

 // read file offsets
 boost::shared_array<uint32> sizes(new uint32[files]);
 BE_read_array(ifile, sizes.get(), files);

 // create directory to store files
 string shrtname;
 shrtname += GetPathnameFromFilename(filename);
 shrtname += GetShortFilenameWithoutExtension(filename);

 // create output file
 stringstream ss;
 ss << shrtname << ".bin";
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // for each file
 for(uint32 i = 0; i < files; i++)
    {
     // record position
     uint32 position = (uint32)ifile.tellg();
     if(ifile.fail()) return error("Tell failure.");

     // seek to nearest 0x80-byte alignment
     position = ((position + 0x7F) & ~(0x7F));
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read size of data
     uint32 n = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // compressed
     if((sizes[i] > 4) && (n == sizes[i] - 4)) {
        bool status = DecompressZLIB(ifile, n, ofile, 0);
        if(!status) return false;
       }
     // uncompressed
     else {
        ifile.seekg(position);
        boost::shared_array<char> temp(new char[sizes[i]]);
        ifile.read(temp.get(), sizes[i]);
        if(ifile.fail()) return error("Read failure.");
        ofile.write(temp.get(), sizes[i]);
        if(ofile.fail()) return error("Write failure.");
       }
    }

 // cleanup ZRC file
 ifile.close();
 DeleteFileA(filename.c_str());

 return true;
}

bool extractor::processBIN(const std::string& filename, bool separate)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // short filesizes
 if(filesize < 0x04) {
    ifile.close();
    DeleteFileA(filename.c_str());
    return true;
   }

 // get filetype
 char ftype = ((filesize == 0) ? GUSELESS : getFileType(ifile));
 if(ifile.fail()) return error("Seek failure.");

 // close file
 ifile.close();

 // create save paths
 string mainpath = GetPathnameFromFilename(filename);
 string B_path = mainpath;
 string I_path = mainpath;
 string M_path = mainpath;
 string S_path = mainpath;
 string T_path = mainpath;
 string U_path = mainpath;
 string W_path = mainpath;
 string A_path = mainpath;
 string G_path = mainpath;

 // create alternate save paths
 if(separate) {
    // binaries
    B_path = mainpath;
    B_path += "\\bin\\";
    CreateDirectoryA(B_path.c_str(), NULL);
    // models
    I_path = mainpath;
    I_path += "\\images\\";
    CreateDirectoryA(I_path.c_str(), NULL);
    // models
    M_path = mainpath;
    M_path += "\\models\\";
    CreateDirectoryA(M_path.c_str(), NULL);
    // sounds
    S_path = mainpath;
    S_path += "\\sounds\\";
    CreateDirectoryA(S_path.c_str(), NULL);
    // textures
    T_path = mainpath;
    T_path += "\\textures\\";
    CreateDirectoryA(T_path.c_str(), NULL);
    // textures
    U_path = mainpath;
    U_path += "\\unknown\\";
    CreateDirectoryA(U_path.c_str(), NULL);
    // pak #1
    W_path = mainpath;
    W_path += "\\wbd\\";
    CreateDirectoryA(W_path.c_str(), NULL);
    // pak #2
    A_path = mainpath;
    A_path += "\\anim\\";
    CreateDirectoryA(A_path.c_str(), NULL);
    // pak #3
    G_path = mainpath;
    G_path += "\\gem\\";
    CreateDirectoryA(G_path.c_str(), NULL);
   }

 // depending on file type
 switch(ftype) {
   case(GUSELESS) : {
        string ofname = U_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".unk";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(G1TG0050) : {
        string ofname = T_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".g1t";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(G1TG0060) : {
        string ofname = T_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".g1t";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(G1CT0001) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".g1c";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(G1M_0034) : {
        string ofname = M_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".g1m";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(G1EM0000) : {
        string ofname = M_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".gem";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(G1FX0000) : {
        string ofname = M_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".gfx";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(SLO_0000) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".slo";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(WBD_0000) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".wbd";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(WBH_0000) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".wbh";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(KSHL0117) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".ksh";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(KPS_0013) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".kps";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(PNG_0000) : {
        string ofname = I_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".png";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(RIFFWAVE) : {
        string ofname = S_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".wav";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(G1A_0042) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".g1a";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(G2A_0020) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".g2a";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(GV1S0000) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".gvs";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(KTFK0260) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".ktf";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(LX000013) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".lx";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(LX000014) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".lx";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(OSTG0000) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".ost";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(O2OB0000) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".o2o";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(COLK0000) : {
        string ofname = B_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".clk";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(PK100000) : {
        string ofname = W_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".pk1";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(PK200000) : {
        string ofname = A_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".pk2";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
   case(PK300000) : {
        string ofname = G_path;
        ofname += GetShortFilenameWithoutExtension(filename);
        ofname += ".pk3";
        MoveFileA(filename.c_str(), ofname.c_str());
        break;
       }
  }

 return true;
}

bool extractor::processPK1(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // extract pathnames
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);
 stringstream ss;
 ss << pathname << shrtname << "\\";
 string destpath = ss.str();
 CreateDirectoryA(destpath.c_str(), NULL);

 // read first offset and length
 uint32 offset = BE_read_uint32(ifile);
 uint32 length = BE_read_uint32(ifile);

 // compute maximum number of files
 uint32 max_files = offset/0x08;
 if(!(max_files < 0xFFFF)) return error("Invalid PK1 file.");

 // add first item
 struct PK1ITEM { uint32 offset; uint32 length; };
 deque<PK1ITEM> items;
 PK1ITEM temp = { offset, length };
 items.push_back(temp);

 // add other items
 for(uint32 i = 1; i < max_files; i++) {
     uint32 offset = BE_read_uint32(ifile);
     uint32 length = BE_read_uint32(ifile);
     PK1ITEM temp = { offset, length };
     items.push_back(temp);
    }

 // save items
 for(uint32 i = 0; i < items.size(); i++)
    {
     // skip
     if(items[i].offset == 0) continue;
     if(items[i].length == 0) continue;

     // move to offset
     ifile.seekg(items[i].offset);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[items[i].length]);
     ifile.read(data.get(), items[i].length);
     if(ifile.fail()) return error("Read failure.");

     // save data
     stringstream ss;
     ss << destpath << setfill('0') << setw(3) << i << ".bin";
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create BIN file.");
     ofile.write(data.get(), items[i].length);
     if(ofile.fail()) return error("Write failure.");

     // TODO:
     // ADD BIN FILE TO BINLIST!
    }

 return true;
}

bool extractor::processPK2(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // extract pathnames
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);
 stringstream ss;
 ss << pathname << shrtname << "\\";
 string destpath = ss.str();
 CreateDirectoryA(destpath.c_str(), NULL);

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(!(n_files > 0x0000)) return error("Invalid PK2 file.");
 if(!(n_files < 0xFFFF)) return error("Invalid PK2 file.");

 // read sizes
 deque<uint32> sizes;
 for(uint32 i = 0; i < n_files; i++) {
     uint32 temp = BE_read_uint32(ifile);
     sizes.push_back(temp);
    }

 // move to next 0x10 offset
 uint32 position = (uint32)ifile.tellg();
 position = ((position + 0xF) & ~0xF);
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");

 // for each size
 for(uint32 i = 0; i < n_files; i++)
    {
     // validate data size
     uint32 datasize = sizes[i];
     if(datasize == 0) continue;

     // create filename
     stringstream ss;
     ss << destpath << setfill('0') << setw(3) << i << ".bin";

     // read data
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);
     if(ifile.fail()) return error("Read failure.");

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create BIN file.");
     ofile.write(data.get(), datasize);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
}

bool extractor::processPK3(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // extract pathnames
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);
 stringstream ss;
 ss << pathname << shrtname << "\\";
 string destpath = ss.str();
 CreateDirectoryA(destpath.c_str(), NULL);

 // compute maximum number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(!(n_files > 0x0000)) return error("Invalid PK3 file.");
 if(!(n_files < 0xFFFF)) return error("Invalid PK3 file.");

 struct PK3ITEM { uint32 offset; uint32 length; };
 deque<PK3ITEM> items;

 // add other items
 for(uint32 i = 0; i < n_files; i++) {
     uint32 offset = BE_read_uint32(ifile);
     uint32 length = BE_read_uint32(ifile);
     PK3ITEM temp = { offset, length };
     items.push_back(temp);
    }

 // save items
 for(uint32 i = 0; i < items.size(); i++)
    {
     // skip
     if(items[i].offset == 0) continue;
     if(items[i].length == 0) continue;

     // move to offset
     ifile.seekg(items[i].offset);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[items[i].length]);
     ifile.read(data.get(), items[i].length);
     if(ifile.fail()) return error("Read failure.");

     // save data
     stringstream ss;
     ss << destpath << setfill('0') << setw(3) << i << ".bin";
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create BIN file.");
     ofile.write(data.get(), items[i].length);
     if(ofile.fail()) return error("Write failure.");

     // TODO:
     // ADD BIN FILE TO BINLIST!
    }

 return true;
}

bool extractor::processG1T(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("processG1T: Could not open file.");

 // extract pathnames
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);
 stringstream ss;
 ss << pathname << shrtname << "\\";
 string destpath = ss.str();
 CreateDirectoryA(destpath.c_str(), NULL);

 // read header
 uint32 magic = BE_read_uint32(ifile);
 uint32 version = BE_read_uint32(ifile);
 uint32 section_size = BE_read_uint32(ifile);
 uint32 table_offset = BE_read_uint32(ifile);
 uint32 n_textures = BE_read_uint32(ifile);

 // validate header
 if(magic != 0x47315447) return error("Expecting G1TG section.");
 if(!(version == 0x30303530 || version == 0x30303630)) return error("Invalid G1TG version.");
 if(n_textures == 0) return error("Invalid number of textures.");

 // move to table
 ifile.seekg(table_offset);
 if(ifile.fail()) return error("Stream seek failure.");

 // read offset table
 deque<size_t> offset_list;
 for(size_t i = 0; i < n_textures; i++) {
     offset_list.push_back(BE_read_uint32(ifile));
     if(ifile.fail()) return error("Stream read failure.");
    }

 // process textures
 for(size_t i = 0; i < offset_list.size(); i++)
    {
     ifile.seekg(table_offset + offset_list[i]);
     if(ifile.fail()) return error("Stream seek failure.");

     // read texture information
     // 0x08 bytes
     uint08 b1 = BE_read_uint08(ifile); // unknown
     uint08 b2 = BE_read_uint08(ifile); // texture type
     uint08 b3 = BE_read_uint08(ifile); // dx/dy
     uint08 b4 = BE_read_uint08(ifile); // unknown
     uint16 s1 = BE_read_uint16(ifile); // unknown
     uint16 s2 = BE_read_uint16(ifile); // unknown

     // header is extended depending on s2
     // 0x0C bytes
     if(s2 == 0x1201) {
        uint32 v1 = BE_read_uint32(ifile); // 12
        uint32 v2 = BE_read_uint32(ifile); // 0
        uint32 v3 = BE_read_uint32(ifile); // 0 or 1
       }

     // number of bytes in header
     uint32 headerBytes = 0x08;
     if(s2 == 0x1201) headerBytes += 0x0C;

     // compute dimensions
     uint32 temp1 = ((b3 & 0xF0) >> 4);
     uint32 temp2 = ((b3 & 0x0F));
     uint32 dx = 1 << temp1;
     uint32 dy = 1 << temp2;

     // create texture file
     stringstream filename;
     filename << destpath << setfill('0') << setw(3) << i << ".dds";
     ofstream ofile(filename.str().c_str(), ios::binary);

     // save texture
     if(b2 == 0)
       {
        // read texture
        DWORD filesize = UncompressedDDSFileSize(dx, dy, 0, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, 0, 0xFF000000, 0xFF0000, 0xFF00, 0xFF, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // UNCOMPRESSED DDS (PS3 works good)
     else if(b2 == 1)
       {
        // read texture
        DWORD filesize = UncompressedDDSFileSize(dx, dy, 0, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, 0, 0xFF000000, 0xFF0000, 0xFF00, 0xFF, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // DXT1 (PS3 works good)
     else if(b2 == 6)
       {
        // read texture
        DWORD filesize = DXT1Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, 0, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // DXT5 (PS3 works good)
     else if(b2 == 8)
       {
        // read texture
        DWORD filesize = DXT5Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, 0, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // UNCOMPRESSED SWIZZLED AND MIPMAPPED DDS (PS3 works good)
     else if(b2 == 9)
       {
        // read texture (but don't save mipmaps)
        DWORD mipmaps = ComputeMipMapNumber(dx, dy);
        DWORD filesize = UncompressedDDSFileSize(dx, dy, 0, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        boost::shared_array<char> data(new char[filesize]);
        boost::shared_array<char> copy(new char[filesize]);
        ifile.read((char*)data.get(), filesize);

        // deswizzle
        for(uint32 r = 0; r < dy; r++) {
            for(uint32 c = 0; c < dx; c++) {
                uint32 morton = array_to_morton(r, c);
                uint32 copy_position = 4*r*dx + 4*c;
                uint32 data_position = 4*morton;
                copy[copy_position++] = data[data_position + 0]; // b
                copy[copy_position++] = data[data_position + 1]; // g
                copy[copy_position++] = data[data_position + 2]; // r
                copy[copy_position++] = data[data_position + 3]; // a
               }
           }

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, 0, 0xFF000000, 0xFF0000, 0xFF00, 0xFF, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)copy.get(), filesize);
       }
     else {
        stringstream msg;
        msg << "Unsupported texture type #" << (uint32)b2 << " at offset " << ifile.tellg() << ".";
        return error(msg.str().c_str());
       }
    }

 return true;
}

bool extractor::processG1M(const std::string& filename)
{
 modelizer obj(filename.c_str());
 return obj.extract();
}

/*
     else if(items[i].type == 0x00010004)
       {
            else if(vertsize == 0x28) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
               vb.flags |= VERTEX_WEIGHTS;
               for(size_t k = 0; k < vertices; k++)
                  {
                   real32 vx = ds.BE_read_real16(); // position 12
                   real32 vy = ds.BE_read_real16(); // position 
                   real32 vz = ds.BE_read_real16(); // position
                   real32 w1 = ds.BE_read_real16(); // weight    8
                   real32 w2 = ds.BE_read_real16(); // weight 
                   real32 w3 = ds.BE_read_real16(); // weight
                   real32 w4 = ds.BE_read_real16(); // weight
                   real32 u1 = ds.BE_read_real16(); // 0x00
                   uint32 b1 = ds.BE_read_uint08(); // bone
                   uint32 b2 = ds.BE_read_uint08(); // bone
                   uint32 b3 = ds.BE_read_uint08(); // bone
                   uint32 b4 = ds.BE_read_uint08(); // bone
                   real32 nx = ds.BE_read_real16(); // normal
                   real32 ny = ds.BE_read_real16(); // normal
                   real32 nz = ds.BE_read_real16(); // normal
                   real32 u2 = ds.BE_read_real16(); // ?
                   uint32 u3 = ds.BE_read_uint32(); // 0xFFFFFFFF
                   real32 tu = ds.BE_read_real16(); // texture coordinate
                   real32 tv = ds.BE_read_real16(); // texture coordinate
                   real32 u4 = ds.BE_read_real16(); // ??
                   real32 u5 = ds.BE_read_real16(); // ??
                   tv = 1.0f - tv;
                   if(debug) {
                      dfile << "v(" << k << ") = (" << vx << ", " << vy << ", " << vz << "), "
                            << "w(" << k << ") = (" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << "), "
                            << "b(" << k << ") = (" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << "), "
                            << "n(" << k << ") = (" << nx << ", " << ny << ", " << nz << "), "
                            << "u(" << k << ") = (" << u1 << ", " << u2 << ", " << hex << u3 << dec << ", " << u4 << ", " << u5 << "), "
                            << "t(" << k << ") = (" << tu << ", " << tv << ")" << endl;
                     }
                   vb.data[k].vx = vx;
                   vb.data[k].vy = vy;
                   vb.data[k].vz = vz;
                  }
              }
            else if(vertsize == 0x34) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
               vb.flags |= VERTEX_WEIGHTS;
               for(size_t k = 0; k < vertices; k++)
                  {
                   real32 vx = ds.BE_read_real32(); // position
                   real32 vy = ds.BE_read_real32(); // position
                   real32 vz = ds.BE_read_real32(); // position
                   real32 w1 = ds.BE_read_real32(); // weight
                   real32 w2 = ds.BE_read_real32(); // weight
                   real32 w3 = ds.BE_read_real32(); // weight
                   uint32 b1 = ds.BE_read_uint08(); // bone
                   uint32 b2 = ds.BE_read_uint08(); // bone
                   uint32 b3 = ds.BE_read_uint08(); // bone
                   uint32 b4 = ds.BE_read_uint08(); // bone (padding)
                   real32 nx = ds.BE_read_real16(); // normal
                   real32 ny = ds.BE_read_real16(); // normal
                   real32 nz = ds.BE_read_real16(); // normal
                   real32 u1 = ds.BE_read_real16(); // 0x0
                   real32 tu = ds.BE_read_real32(); // texture coordinate
                   real32 tv = ds.BE_read_real32(); // texture coordinate
                   tv = 1.0f - tv;
                   real32 u2 = ds.BE_read_real32(); // ???
                   real32 u3 = ds.BE_read_real32(); // ???
                   if(debug) {
                      dfile << "v(" << k << ") = (" << vx << ", " << vy << ", " << vz << "), "
                            << "n(" << k << ") = (" << nx << ", " << ny << ", " << nz << "), "
                            << "w(" << k << ") = (" << w1 << ", " << w2 << ", " << w3 << "), "
                            << "b(" << k << ") = (" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << "), "
                            << "u(" << k << ") = (" << u1 << ", " << u2 << ", " << u3 << "), "
                            << "t(" << k << ") = (" << tu << ", " << tv << ")" << endl;
                     }
                   vb.data[k].vx = vx;
                   vb.data[k].vy = vy;
                   vb.data[k].vz = vz;
                  }
              }
            else if(vertsize == 0x38) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
               vb.flags |= VERTEX_WEIGHTS;
               for(size_t k = 0; k < vertices; k++)
                  {
                   real32 vx = ds.BE_read_real32(); // position
                   real32 vy = ds.BE_read_real32(); // position
                   real32 vz = ds.BE_read_real32(); // position
                   real32 w1 = ds.BE_read_real32(); // weight
                   real32 w2 = ds.BE_read_real32(); // weight
                   real32 w3 = ds.BE_read_real32(); // weight
                   real32 w4 = ds.BE_read_real32(); // weight
                   uint32 b1 = ds.BE_read_uint08(); // bone
                   uint32 b2 = ds.BE_read_uint08(); // bone
                   uint32 b3 = ds.BE_read_uint08(); // bone
                   uint32 b4 = ds.BE_read_uint08(); // bone
                   real32 nx = ds.BE_read_real16(); // normal
                   real32 ny = ds.BE_read_real16(); // normal
                   real32 nz = ds.BE_read_real16(); // normal
                   real32 u1 = ds.BE_read_real16(); // 0x0
                   real32 tu = ds.BE_read_real32(); // texture coordinate
                   real32 tv = ds.BE_read_real32(); // texture coordinate
                   tv = 1.0f - tv;
                   real32 u2 = ds.BE_read_real32(); // ???
                   real32 u3 = ds.BE_read_real32(); // ???
                   if(debug) {
                      dfile << "v(" << k << ") = (" << vx << ", " << vy << ", " << vz << "), "
                            << "n(" << k << ") = (" << nx << ", " << ny << ", " << nz << "), "
                            << "w(" << k << ") = (" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << "), "
                            << "b(" << k << ") = (" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << "), "
                            << "t(" << k << ") = (" << tu << ", " << tv << ")" << endl;
                     }
                   vb.data[k].vx = vx;
                   vb.data[k].vy = vy;
                   vb.data[k].vz = vz;
                  }
              }
            else if(vertsize == 0x60)
              {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_UV;
               for(size_t k = 0; k < vertices; k++)
                  {
                   real32 p01 = ds.BE_read_real32(); // position
                   real32 p02 = ds.BE_read_real32(); // position
                   real32 p03 = ds.BE_read_real32(); // position
                   real32 p04 = ds.BE_read_real32(); // 0
                   real32 p05 = ds.BE_read_real32(); // x
                   real32 p06 = ds.BE_read_real32(); // y
                   real32 p07 = ds.BE_read_real32(); // z
                   real32 p08 = ds.BE_read_real32(); // 
                   real32 p09 = ds.BE_read_real16(); // 
                   real32 p10 = ds.BE_read_real16(); // 
                   real32 p11 = ds.BE_read_real16(); // 
                   real32 p12 = ds.BE_read_real16(); // 
                   real32 p13 = ds.BE_read_real16(); // 
                   real32 p14 = ds.BE_read_real16(); // 
                   real32 p15 = ds.BE_read_real16(); // 
                   real32 p16 = ds.BE_read_real16(); // 
                   real32 p17 = ds.BE_read_real32(); // 
                   real32 p18 = ds.BE_read_real32(); // 
                   uint32 p19 = ds.BE_read_uint08(); // 
                   uint32 p20 = ds.BE_read_uint08(); // 
                   uint32 p21 = ds.BE_read_uint08(); // 
                   uint32 p22 = ds.BE_read_uint08(); // 
                   uint32 p23 = ds.BE_read_uint08(); // 
                   uint32 p24 = ds.BE_read_uint08(); // 
                   uint32 p25 = ds.BE_read_uint08(); // 
                   uint32 p26 = ds.BE_read_uint08(); // 
                   uint32 p27 = ds.BE_read_uint08(); // 
                   uint32 p28 = ds.BE_read_uint08(); // 
                   uint32 p29 = ds.BE_read_uint08(); // 
                   uint32 p30 = ds.BE_read_uint08(); // 
                   uint32 p31 = ds.BE_read_uint08(); // 
                   uint32 p32 = ds.BE_read_uint08(); // 
                   uint32 p33 = ds.BE_read_uint08(); // 
                   uint32 p34 = ds.BE_read_uint08(); // 
                   real32 p35 = ds.BE_read_real16(); // 
                   real32 p36 = ds.BE_read_real16(); // 
                   real32 p37 = ds.BE_read_real16(); // 
                   real32 p38 = ds.BE_read_real16(); // 
                   real32 p39 = ds.BE_read_real16(); // 
                   real32 p40 = ds.BE_read_real16(); // 
                   real32 p41 = ds.BE_read_real16(); // small number
                   real32 p42 = ds.BE_read_real16(); // +1 or -1
                   real32 p43 = ds.BE_read_real32(); // u
                   real32 p44 = ds.BE_read_real32(); // v

                   if(debug) dfile << p01 << endl; // position
                   if(debug) dfile << p02 << endl; // position
                   if(debug) dfile << p03 << endl; // position
                   if(debug) dfile << p04 << endl; // 0?
                   if(debug) dfile << p05 << endl; // 
                   if(debug) dfile << p06 << endl; // 
                   if(debug) dfile << p07 << endl; // 
                   if(debug) dfile << p08 << endl; // 
                   if(debug) dfile << p09 << endl; // 
                   if(debug) dfile << p10 << endl; // 
                   if(debug) dfile << p11 << endl; // 
                   if(debug) dfile << p12 << endl; // 
                   if(debug) dfile << p13 << endl; // 
                   if(debug) dfile << p14 << endl; // 
                   if(debug) dfile << p15 << endl; // 
                   if(debug) dfile << p16 << endl; // 
                   if(debug) dfile << p17 << endl; // 
                   if(debug) dfile << p18 << endl; // 
                   if(debug) dfile << p19 << endl; // 
                   if(debug) dfile << p20 << endl; // 
                   if(debug) dfile << p21 << endl; // 
                   if(debug) dfile << p22 << endl; // 
                   if(debug) dfile << p23 << endl; // 
                   if(debug) dfile << p24 << endl; // 
                   if(debug) dfile << p25 << endl; // 
                   if(debug) dfile << p26 << endl; // 
                   if(debug) dfile << p27 << endl; // 
                   if(debug) dfile << p28 << endl; // 
                   if(debug) dfile << p29 << endl; // 
                   if(debug) dfile << p30 << endl; // 
                   if(debug) dfile << p31 << endl; // 
                   if(debug) dfile << p32 << endl; // 
                   if(debug) dfile << p33 << endl; // 
                   if(debug) dfile << p34 << endl; // 
                   if(debug) dfile << p35 << endl; // 
                   if(debug) dfile << p36 << endl; // 
                   if(debug) dfile << p37 << endl; // 
                   if(debug) dfile << p38 << endl; // 
                   if(debug) dfile << p39 << endl; // 
                   if(debug) dfile << p40 << endl; // 
                   if(debug) dfile << p41 << endl; // 
                   if(debug) dfile << p42 << endl; // 
                   if(debug) dfile << p43 << endl; // 
                   if(debug) dfile << p44 << endl; // 
                   if(debug) dfile << endl;

                   vb.data[k].vx = p01;
                   vb.data[k].vy = p02;
                   vb.data[k].vz = p03;
                  }
              }
            else {
               stringstream ss;
               ss << "Unknown vertex size 0x" << hex << vertsize << dec << ".";
               return error(ss.str().c_str());
              }
           }

        if(debug) dfile << endl;
       }
    }

      // TODO: DELETE
     // vertex section
     else if(items[i].type == 0x00010004)
       {
        for(size_t j = 0; j < n_meshes; j++)
           {
            // set vertex buffer properties
            SMC_VTX_BUFFER vb;
            vb.flags = 0;
            vb.name = name.str();
            vb.elem = vertices;
            vb.data.reset(new SMC_VERTEX[vertices]);

            // set vertex buffer flags
            if(vertsize == 0x10) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vertsize == 0x14) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vertsize == 0x18) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
              }
            else if(vertsize == 0x1C) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vertsize == 0x20) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vertsize == 0x24) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vertsize == 0x28) {
              }
            else if(vertsize == 0x38) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else
               return error("Unknown vertex format.");

            // read vertices
            for(size_t k = 0; k < vertices; k++)
               {
                SMC_VERTEX vertex;
                if(vertsize == 0x10) {
                   vertex.vx = ss.BE_read_real16();
                   vertex.vy = ss.BE_read_real16();
                   vertex.vz = ss.BE_read_real16();
                   ss.BE_read_real16();
                   vertex.nx = ss.BE_read_real16();
                   vertex.ny = ss.BE_read_real16();
                   vertex.nz = ss.BE_read_real16();
                   ss.BE_read_real16();
                   ofile << "v " << vertex.vx << " " << vertex.vy << " " << vertex.vz << endl;
                  }
                else if(vertsize == 0x14) {
                   vertex.vx = ss.BE_read_real16();
                   vertex.vy = ss.BE_read_real16();
                   vertex.vz = ss.BE_read_real16();
                   ss.BE_read_real16();
                   vertex.nx = ss.BE_read_real16();
                   vertex.ny = ss.BE_read_real16();
                   vertex.nz = ss.BE_read_real16();
                   ss.BE_read_real16();
                   vertex.tu = ss.BE_read_real16();
                   vertex.tv = ss.BE_read_real16();
                   ofile << "v " << vertex.vx << " " << vertex.vy << " " << vertex.vz << endl;
                   ofile << "vt " << vertex.tu << " " << vertex.tv << endl;
                  }
                else if(vertsize == 0x18) {
                   vertex.vx = ss.BE_read_real32();
                   vertex.vy = ss.BE_read_real32();
                   vertex.vz = ss.BE_read_real32();
                   vertex.nx = ss.BE_read_real16();
                   vertex.ny = ss.BE_read_real16();
                   vertex.nz = ss.BE_read_real16();
                   ss.BE_read_real16();
                   ss.BE_read_real32();
                   ofile << "v " << vertex.vx << " " << vertex.vy << " " << vertex.vz << endl;
                  }
                else if(vertsize == 0x1C) {
                   vertex.vx = ss.BE_read_real32();
                   vertex.vy = ss.BE_read_real32();
                   vertex.vz = ss.BE_read_real32();
                   vertex.nx = ss.BE_read_real16();
                   vertex.ny = ss.BE_read_real16();
                   vertex.nz = ss.BE_read_real16();
                   ss.BE_read_real16();
                   ss.BE_read_real32();
                   vertex.tu = ss.BE_read_real16();
                   vertex.tv = ss.BE_read_real16();
                   ofile << "v " << vertex.vx << " " << vertex.vy << " " << vertex.vz << endl;
                   ofile << "vt " << vertex.tu << " " << vertex.tv << endl;
                  }
                else if(vertsize == 0x20) {
                   vertex.vx = ss.BE_read_real32();
                   vertex.vy = ss.BE_read_real32();
                   vertex.vz = ss.BE_read_real32();
                   vertex.nx = ss.BE_read_real16();
                   vertex.ny = ss.BE_read_real16();
                   vertex.nz = ss.BE_read_real16();
                   ss.BE_read_real16();
                   ss.BE_read_real32();
                   ss.BE_read_real16();
                   ss.BE_read_real16();
                   vertex.tu = ss.BE_read_real16();
                   vertex.tv = ss.BE_read_real16();
                   ofile << "v " << vertex.vx << " " << vertex.vy << " " << vertex.vz << endl;
                   ofile << "vt " << vertex.tu << " " << vertex.tv << endl;
                  }
                else if(vertsize == 0x24) {
                  }
                else if(vertsize == 0x28) {
                   ss.BE_read_real32(); // TODO
                   ss.BE_read_real32();
                   ss.BE_read_real32();
                   ss.BE_read_real32();
                   ss.BE_read_real32();
                   ss.BE_read_real32();
                   ss.BE_read_real32();
                   ss.BE_read_real32();
                   ss.BE_read_real32();
                   ss.BE_read_real32();
                  }
                else if(vertsize == 0x38) {
                   vertex.vx = ss.BE_read_real32(); // x
                   vertex.vy = ss.BE_read_real32(); // y
                   vertex.vz = ss.BE_read_real32(); // z
                   ss.BE_read_real32(); // 1.0
                   ss.BE_read_real32(); // w1
                   ss.BE_read_real32(); // w2
                   ss.BE_read_real32(); // w3
                   (uint32)ss.BE_read_uint08();
                   (uint32)ss.BE_read_uint08();
                   (uint32)ss.BE_read_uint08();
                   (uint32)ss.BE_read_uint08();
                   ss.BE_read_real16();
                   ss.BE_read_real16();
                   ss.BE_read_real16();
                   ss.BE_read_real16();
                   vertex.tu = ss.BE_read_real32(); // u
                   vertex.tv = ss.BE_read_real32(); // v
                   ss.BE_read_real32();
                   ss.BE_read_real32();
                   ofile << "v " << vertex.vx << " " << vertex.vy << " " << vertex.vz << endl;
                   ofile << "vt " << vertex.tu << " " << vertex.tv << endl;
                  }

                // save vertex
                vb.data[k] = vertex;
               }
           }
       }
    }

 return true;
}
*/

};};

namespace X_SYSTEM { namespace X_GAME {

modelizer::modelizer(const char* fname)
{
 filename = fname;
 pathname = GetPathnameFromFilename(filename);
 shrtname = GetShortFilenameWithoutExtension(filename);
}

modelizer::~modelizer()
{
}

bool modelizer::extract(void)
{
 // open file
 using namespace std;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("processG1M: Could not open file.");

 // create debug file
 std::string dfname = pathname;
 dfname += shrtname;
 dfname += ".txt";
 if(debug) dfile.open(dfname.c_str());

 // read header
 uint32 head01 = BE_read_uint32(ifile); // magic
 uint32 head02 = BE_read_uint32(ifile); // version
 uint32 head03 = BE_read_uint32(ifile); // total section size
 uint32 head04 = BE_read_uint32(ifile); // start offset
 uint32 head05 = BE_read_uint32(ifile); // 0x00
 uint32 head06 = BE_read_uint32(ifile); // number of chunks to read
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << "magic = " << head01 << endl;
    dfile << "version = " << head02 << endl;
    dfile << "section size = " << head03 << endl;
    dfile << "start offset = 0x" << hex << head04 << dec << endl;
    dfile << "0x00 = " << head05 << endl;
    dfile << "number of chunks = " << head06 << endl;
    dfile << endl;
   }

 // validate header
 if(head01 != 0x47314D5F) return error("Expecting G1M_ section.");
 if(head02 != 0x30303334) return error("Invalid G1M_ version.");
 if(head03 == 0) return error("Invalid G1M_.");
 if(head04 == 0) return error("Invalid G1M_.");

 // move to start
 ifile.seekg(head04);
 if(ifile.fail()) return error("processG1M: Seek failure.");

 // read chunks
 for(uint32 i = 0; i < head06; i++)
    {
     // read first chunk
     uint32 cposition = (uint32)ifile.tellg(); // save position
     uint32 chunkname = BE_read_uint32(ifile); // chunk name
     uint32 chunkvers = BE_read_uint32(ifile); // chunk version
     uint32 chunksize = BE_read_uint32(ifile); // chunk size

     // process chunk
     switch(chunkname) {
       case(0x47314D46) : // G1MF
           {
            // create data
            ifile.seekg(cposition);
            uint32 size = chunksize;
            boost::shared_array<char> data(new char[size]);
            
            // read data
            BE_read_array(ifile, data.get(), size);
            if(ifile.fail()) return error("Read failure.");
            
            // process data
            if(!processG1MF0021(data, size)) return false;
            break;
           }
       case(0x47314D53) : // G1MS
           {
            // create data
            ifile.seekg(cposition);
            uint32 size = chunksize;
            boost::shared_array<char> data(new char[size]);
            
            // read data
            BE_read_array(ifile, data.get(), size);
            if(ifile.fail()) return error("Read failure.");
            
            // process data
            if(!processG1MS0032(data, size)) return false;
            break;
           }
       case(0x47314D4D) : // G1MM
           {
            // create data
            ifile.seekg(cposition);
            uint32 size = chunksize;
            boost::shared_array<char> data(new char[size]);
            
            // read data
            BE_read_array(ifile, data.get(), size);
            if(ifile.fail()) return error("Read failure.");
            
            // process data
            if(!processG1MM0020(data, size)) return false;
            break;
           }
       case(0x47314D47) : // G1MG
           {
            // create data
            ifile.seekg(cposition);
            uint32 size = chunksize;
            boost::shared_array<char> data(new char[size]);
            
            // read data
            BE_read_array(ifile, data.get(), size);
            if(ifile.fail()) return error("Read failure.");
            
            // process data
            if(!processG1MG0044(data, size)) return false;
            break;
           }
       case(0x434F4C4C) : { // COLL
            ifile.seekg(chunksize - 0x0C, ios::cur);
            break;
           }
       case(0x4E554E4F) : { // NUNO
            ifile.seekg(chunksize - 0x0C, ios::cur);
            break;
           }
       case(0x45585452) : { // EXTR
            ifile.seekg(chunksize - 0x0C, ios::cur);
            break;
           }
       default : {
            stringstream ss;
            ss << "processG1M: Unknown chunk 0x" << std::hex << chunkname << std::dec << ".";
            return error(ss.str().c_str());
           }
      }
    }

 // create output file
 string objname = pathname;
 objname += shrtname;
 objname += ".lwo";
 return SaveLWO(pathname.c_str(), shrtname.c_str(), smc);
}

bool modelizer::processG1MF0021(boost::shared_array<char> data, uint32 size)
{
 // build binary stream
 using namespace std;
 binary_stream bs(data, size);

 // read chunk properties
 uint32 chunkname = bs.BE_read_uint32();
 uint32 chunkvers = bs.BE_read_uint32();
 uint32 chunksize = bs.BE_read_uint32(); 
 if(debug) dfile << "G1MF0021 CHUNK" << endl;
 if(debug) dfile << " chunkname = 0x" << hex << chunkname << dec << endl;
 if(debug) dfile << " chunkvers = 0x" << hex << chunkvers << dec << endl;
 if(debug) dfile << " chunksize = 0x" << hex << chunksize << dec << endl;

 // save item
 G1MF0021_ITEM item;
 item.elem = 32;
 item.data.reset(new uint32[item.elem]);
 bs.BE_read_array(item.data.get(), item.elem);
 g1mfdata.push_back(item);

 // display data
 if(debug) {
    dfile << " G1MF0021_ITEM" << endl;
    for(uint32 i = 0; i < item.elem; i++) dfile << "  0x" << hex << setfill('0') << setw(8) << item.data[i] << dec << endl;
    dfile << endl;
   }

 return true;
}

bool modelizer::processG1MS0032(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool modelizer::processG1MM0020(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool modelizer::processG1MG0044(boost::shared_array<char> data, uint32 size)
{
 // build binary stream
 using namespace std;
 binary_stream bs(data, size);

 // read chunk properties
 uint32 chunkname = bs.BE_read_uint32();
 uint32 chunkvers = bs.BE_read_uint32();
 uint32 chunksize = bs.BE_read_uint32(); 
 if(debug) dfile << "G1MG0044 CHUNK" << endl;
 if(debug) dfile << " chunkname = 0x" << hex << chunkname << dec << endl;
 if(debug) dfile << " chunkvers = 0x" << hex << chunkvers << dec << endl;
 if(debug) dfile << " chunksize = 0x" << hex << chunksize << dec << endl;

 // read header
 uint32 head01 = bs.BE_read_uint32(); // platform
 uint32 head02 = bs.BE_read_uint32(); // unknown1
 real32 head03 = bs.BE_read_real32(); // min_x
 real32 head04 = bs.BE_read_real32(); // min_y
 real32 head05 = bs.BE_read_real32(); // min_z
 real32 head06 = bs.BE_read_real32(); // max_x
 real32 head07 = bs.BE_read_real32(); // max_y
 real32 head08 = bs.BE_read_real32(); // max_z
 uint32 head09 = bs.BE_read_uint32(); // sections
 if(debug) {
    dfile << " HEADER" << endl;
    dfile << "  platform = " << head01 << endl;
    dfile << "  unknown1 = " << head02 << endl;
    dfile << "  min_x = " << head03 << endl;
    dfile << "  min_y = " << head04 << endl;
    dfile << "  min_z = " << head05 << endl;
    dfile << "  max_x = " << head06 << endl;
    dfile << "  max_y = " << head07 << endl;
    dfile << "  max_z = " << head08 << endl;
    dfile << "  sections = " << head09 << endl;
   }

 // validate header
 if(head01 != 0x50533300) return error("Only PS3 version of game is supported.");
 if(head09 == 0) return error("Invalid number of sections.");

 // process sections
 uint32 n_sections = head09;
 for(size_t i = 0; i < n_sections; i++)
    {
     // section information
     if(debug) dfile << " SECTION #" << i << endl;
     uint32 section_type = bs.BE_read_uint32();
     uint32 section_size = bs.BE_read_uint32();
     if(debug) dfile << "  section_type = 0x" << hex << section_type << dec << endl;
     if(debug) dfile << "  section_size = 0x" << hex << section_size << dec << endl;

     section_size -= 0x8;

     // process section
     if(section_type == 0x00010001) {
        boost::shared_array<char> section_data(new char[section_size]);
        bs.BE_read_array(section_data.get(), section_size);
        if(!processSECT0101(section_data, section_size)) return false;
       }
     else if(section_type == 0x00010002) {
        boost::shared_array<char> section_data(new char[section_size]);
        bs.BE_read_array(section_data.get(), section_size);
        if(!processSECT0102(section_data, section_size)) return false;
       }
     else if(section_type == 0x00010003) {
        boost::shared_array<char> section_data(new char[section_size]);
        bs.BE_read_array(section_data.get(), section_size);
        if(!processSECT0103(section_data, section_size)) return false;
       }
     else if(section_type == 0x00010004) {
        boost::shared_array<char> section_data(new char[section_size]);
        bs.BE_read_array(section_data.get(), section_size);
        if(!processSECT0104(section_data, section_size)) return false;
       }
     else if(section_type == 0x00010005) {
        boost::shared_array<char> section_data(new char[section_size]);
        bs.BE_read_array(section_data.get(), section_size);
        if(!processSECT0105(section_data, section_size)) return false;
       }
     else if(section_type == 0x00010006) {
        boost::shared_array<char> section_data(new char[section_size]);
        bs.BE_read_array(section_data.get(), section_size);
        if(!processSECT0106(section_data, section_size)) return false;
       }
     else if(section_type == 0x00010007) {
        boost::shared_array<char> section_data(new char[section_size]);
        bs.BE_read_array(section_data.get(), section_size);
        if(!processSECT0107(section_data, section_size)) return false;
       }
     else if(section_type == 0x00010008) {
        boost::shared_array<char> section_data(new char[section_size]);
        bs.BE_read_array(section_data.get(), section_size);
        if(!processSECT0108(section_data, section_size)) return false;
       }
     else if(section_type == 0x00010009) {
        boost::shared_array<char> section_data(new char[section_size]);
        bs.BE_read_array(section_data.get(), section_size);
        if(!processSECT0109(section_data, section_size)) return false;
       }
     else
        return error("Unknown G1MG0044 section.");
    }

 if(debug) dfile << endl;
 return true;
}

bool modelizer::processSECT0101(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool modelizer::processSECT0102(boost::shared_array<char> data, uint32 size)
{
 if(!data.get()) return error("Invalid 0x010002 data.");
 if(!size) return error("Invalid 0x010002 data.");

 // build binary stream
 using namespace std;
 binary_stream bs(data, size);

 // read section information
 uint32 section_elem = bs.BE_read_uint32();
 if(debug) dfile << "  section_elem = 0x" << hex << section_elem << dec << endl;
 if(debug) dfile << "  TEXTURE ASSOCIATIONS" << endl;
 if(!section_elem) return true;

 // for each surface
 for(size_t i = 0; i < section_elem; i++)
    {
     // read item
     uint32 p01 = bs.BE_read_uint32(); // 0x00000000
     uint32 p02 = bs.BE_read_uint32(); // 0x00000001
     uint32 p03 = bs.BE_read_uint32(); // 0x00000001
     uint32 p04 = bs.BE_read_uint32(); // 0x00000000
     uint16 p05 = bs.BE_read_uint16(); // 0x000B     (filename reference)
     uint32 p06 = bs.BE_read_uint32(); // 0x00000001
     uint16 p07 = bs.BE_read_uint16(); // 0x0004
     uint16 p08 = bs.BE_read_uint16(); // 0x0004
     uint16 p09 = bs.BE_read_uint16(); // 0x0004

     // save item
     uint32 rb_index = i;
     uint32 color_map_index = p05;
     smc.colr_maps.insert(map<uint32, uint32>::value_type(rb_index, color_map_index));

     // insert texture
     stringstream ss;
     ss << setfill('0') << setw(3) << color_map_index << ".tga";
     smc.txlist.insert(map<uint32, string>::value_type(color_map_index, ss.str().c_str()));

     // print item
     if(debug) {
        dfile << "   " <<
                 "0x" << hex << setfill('0') << setw(8) << p01 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p02 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p03 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p04 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p05 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p06 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p07 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p08 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p09 << dec << endl;
       }
    }

 return true;
}

bool modelizer::processSECT0103(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool modelizer::processSECT0104(boost::shared_array<char> data, uint32 size)
{
 // validate
 if(g1mfdata.size() != 1) return error("Missing G1MF section.");

 // build binary stream
 using namespace std;
 binary_stream bs(data, size);

 // read section information
 uint32 section_elem = bs.BE_read_uint32();
 if(debug) dfile << "  section_elem = 0x" << hex << section_elem << dec << endl;
 if(!section_elem) return true;

 // static models
 if(g1mfdata[0].data[0] == 1)
   {
    // for each mesh
    for(size_t i = 0; i < section_elem; i++)
       {
        // read mesh vertex info
        uint32 unknown1 = bs.BE_read_uint32();
        uint32 vertsize = bs.BE_read_uint32();
        uint32 vertices = bs.BE_read_uint32();
        uint32 unknown2 = bs.BE_read_uint32();
        if(debug) dfile << "  VERTEX SET #" << i << endl;
        if(debug) dfile << "   unknown1 = 0x" << hex << unknown1 << dec << endl;
        if(debug) dfile << "   vertsize = 0x" << hex << vertsize << dec << endl;
        if(debug) dfile << "   vertices = 0x" << hex << vertices << dec << endl;
        if(debug) dfile << "   unknown2 = 0x" << hex << unknown2 << dec << endl;

        // prepare vertex buffer
        SMC_VTX_BUFFER vb;
        vb.elem = vertices;
        vb.data.reset(new SMC_VERTEX[vb.elem]);

        // create mesh name
        stringstream meshname;
        meshname << " mesh_" << setfill('0') << setw(3) << i;
        vb.name = meshname.str();

        if(vertsize == 0x14)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real16(); //  2
               real32 vy = bs.BE_read_real16(); //  4
               real32 vz = bs.BE_read_real16(); //  6
               real32 vw = bs.BE_read_real16(); //  8
               real32 nx = bs.BE_read_real16(); // 10
               real32 ny = bs.BE_read_real16(); // 12
               real32 nz = bs.BE_read_real16(); // 14
               real32 nw = bs.BE_read_real16(); // 16
               real32 tu = bs.BE_read_real16(); // 18
               real32 tv = bs.BE_read_real16(); // 20
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << ", " << vw << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << ", " << nw <<  "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x14" << endl;
          }
        else if(vertsize == 0x18)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real16(); //  2
               real32 vy = bs.BE_read_real16(); //  4
               real32 vz = bs.BE_read_real16(); //  6
               real32 vw = bs.BE_read_real16(); //  8
               uint32 a1 = bs.BE_read_uint08(); //  9
               uint32 a2 = bs.BE_read_uint08(); // 10
               uint32 a3 = bs.BE_read_uint08(); // 11
               uint32 a4 = bs.BE_read_uint08(); // 12
               real32 nx = bs.BE_read_real16(); // 14
               real32 ny = bs.BE_read_real16(); // 16
               real32 nz = bs.BE_read_real16(); // 18
               real32 nw = bs.BE_read_real16(); // 20
               real32 tu = bs.BE_read_real16(); // 22
               real32 tv = bs.BE_read_real16(); // 24
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << ", " << vw << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << ", " << nw <<  "), "
                        << "a(" << j << ") = (" << a1 << ", " << a2 << ", " << a3 << ", " << a4 <<  "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x18" << endl;
          }
        else if(vertsize == 0x1C)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real16(); //  2
               real32 vy = bs.BE_read_real16(); //  4
               real32 vz = bs.BE_read_real16(); //  6
               real32 vw = bs.BE_read_real16(); //  8
               real32 nx = bs.BE_read_real16(); // 10
               real32 ny = bs.BE_read_real16(); // 12
               real32 nz = bs.BE_read_real16(); // 14
               real32 nw = bs.BE_read_real16(); // 16
               uint32 a1 = bs.BE_read_uint08(); // 17
               uint32 a2 = bs.BE_read_uint08(); // 18
               uint32 a3 = bs.BE_read_uint08(); // 19
               uint32 a4 = bs.BE_read_uint08(); // 20
               uint32 b1 = bs.BE_read_uint08(); // 21
               uint32 b2 = bs.BE_read_uint08(); // 22
               uint32 b3 = bs.BE_read_uint08(); // 23
               uint32 b4 = bs.BE_read_uint08(); // 24
               real32 tu = bs.BE_read_real16(); // 26
               real32 tv = bs.BE_read_real16(); // 28
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << ", " << vw << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << ", " << nw <<  "), "
                        << "a(" << j << ") = (" << a1 << ", " << a2 << ", " << a3 << ", " << a4 <<  "), "
                        << "b(" << j << ") = (" << b1 << ", " << b2 << ", " << b3 << ", " << b4 <<  "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x1C" << endl;
          }
        else if(vertsize == 0x20)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); // position
               real32 vy = bs.BE_read_real32(); // position 
               real32 vz = bs.BE_read_real32(); // position
               real32 u1 = bs.BE_read_real32(); // float
               real32 nx = bs.BE_read_real16(); // normal
               real32 ny = bs.BE_read_real16(); // normal
               real32 nz = bs.BE_read_real16(); // normal
               real32 u2 = bs.BE_read_real16(); // half-float
               real32 tu = bs.BE_read_real32(); // texture coordinate
               real32 tv = bs.BE_read_real32(); // texture coordinate
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x20" << endl;
          }
        else if(vertsize == 0x24)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); // position
               real32 vy = bs.BE_read_real32(); // position 
               real32 vz = bs.BE_read_real32(); // position
               real32 nx = bs.BE_read_real32(); // normal
               real32 ny = bs.BE_read_real32(); // normal
               real32 nz = bs.BE_read_real32(); // normal
               real32 u1 = bs.BE_read_real32(); // ??
               real32 tu = bs.BE_read_real32(); // texture coordinate
               real32 tv = bs.BE_read_real32(); // texture coordinate
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x24" << endl;
          }
        else if(vertsize == 0x28)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); // position
               real32 vy = bs.BE_read_real32(); // position 
               real32 vz = bs.BE_read_real32(); // position
               real32 u1 = bs.BE_read_real32(); // 1.0
               real32 nx = bs.BE_read_real16(); // normal
               real32 ny = bs.BE_read_real16(); // normal
               real32 nz = bs.BE_read_real16(); // normal
               real32 u2 = bs.BE_read_real16(); // 0.0
               real32 tu = bs.BE_read_real32(); // texture coordinate
               real32 tv = bs.BE_read_real32(); // texture coordinate
               real32 u3 = bs.BE_read_real32(); // ???
               real32 u4 = bs.BE_read_real32(); // ???
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << "), "
                        << "u(" << j << ") = (" << u1 << ", " << u2 << ", " << u3 << ", " << u4 << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x28" << endl;
          }
        // PS3 (PERFECT)
        else if(vertsize == 0x2C)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); // position
               real32 vy = bs.BE_read_real32(); // position 
               real32 vz = bs.BE_read_real32(); // position
               real32 u1 = bs.BE_read_real32(); // 0x01
               real32 u2 = bs.BE_read_real32(); // 0x00
               real32 nx = bs.BE_read_real32(); // normal
               real32 ny = bs.BE_read_real32(); // normal
               real32 nz = bs.BE_read_real32(); // normal
               real32 u3 = bs.BE_read_real32(); // 0xFFFFFFFF
               real32 tu = bs.BE_read_real32(); // texture coordinate
               real32 tv = bs.BE_read_real32(); // texture coordinate
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].nx = nx;
               vb.data[j].ny = ny;
               vb.data[j].nz = nz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x2C" << endl;
          }
        // PS3 (PERFECT)
        else if(vertsize == 0x30)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); // position
               real32 vy = bs.BE_read_real32(); // position 
               real32 vz = bs.BE_read_real32(); // position
               real32 u1 = bs.BE_read_real32(); // 0x01
               real32 u2 = bs.BE_read_real32(); // 0x00
               real32 u3 = bs.BE_read_real32(); // 0x00
               real32 nx = bs.BE_read_real32(); // normal
               real32 ny = bs.BE_read_real32(); // normal
               real32 nz = bs.BE_read_real32(); // normal
               real32 u4 = bs.BE_read_real32(); // 0xFFFFFFFF
               real32 tu = bs.BE_read_real32(); // texture coordinate
               real32 tv = bs.BE_read_real32(); // texture coordinate
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].nx = nx;
               vb.data[j].ny = ny;
               vb.data[j].nz = nz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
          }
        else if(vertsize == 0x34)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); //  4 position
               real32 vy = bs.BE_read_real32(); //  8 position
               real32 vz = bs.BE_read_real32(); // 12 position
               real32 u1 = bs.BE_read_real32(); // 16 float
               real32 u2 = bs.BE_read_real32(); // 20 float
               real32 u3 = bs.BE_read_real32(); // 24 float
               uint32 p1 = bs.BE_read_uint08(); // 25 byte
               uint32 p2 = bs.BE_read_uint08(); // 26 byte
               uint32 p3 = bs.BE_read_uint08(); // 27 byte
               uint32 p4 = bs.BE_read_uint08(); // 28 byte
               real32 nx = bs.BE_read_real16(); // 30 normal
               real32 ny = bs.BE_read_real16(); // 32 normal
               real32 nz = bs.BE_read_real16(); // 34 normal
               real32 u4 = bs.BE_read_real16(); // 36 0x0
               real32 tu = bs.BE_read_real32(); // 40 texture
               real32 tv = bs.BE_read_real32(); // 44 texture
               real32 u5 = bs.BE_read_real32(); // 48 ??
               real32 u6 = bs.BE_read_real32(); // 52 ??
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << "), "
                        << "p(" << j << ") = (" << p1 << ", " << p2 << ", " << p3 << ", " << p4 << "), "
                        << "u(" << j << ") = (" << u1 << ", " << u2 << ", " << u3 << ", " << u4 << ", " << u5 << ", " << u6 << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x34" << endl;
          }
        else if(vertsize == 0x38)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); // position
               real32 vy = bs.BE_read_real32(); // position 
               real32 vz = bs.BE_read_real32(); // position
               real32 u1 = bs.BE_read_real32(); // 
               real32 u2 = bs.BE_read_real32(); // 
               real32 u3 = bs.BE_read_real32(); // 
               real32 u4 = bs.BE_read_real32(); // 0?
               real32 u5 = bs.BE_read_real32(); // int?
               real32 u6 = bs.BE_read_real32(); // 
               real32 u7 = bs.BE_read_real32(); // 
               real32 tu = bs.BE_read_real32(); // texture coordinate
               real32 tv = bs.BE_read_real32(); // texture coordinate
               real32 u8 = bs.BE_read_real32(); // ???
               real32 u9 = bs.BE_read_real32(); // ???
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << "), "
                        //<< "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << "), "
                        << "u(" << j << ") = (" << u1 << ", " << u2 << ", " << u3 << ", " << u4 << ", " << u5 << ", " << u6 << ", " << u7 << ", " << u8 << ", " << u9 << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x38" << endl;
          }
        else if(vertsize == 0x3C)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); //  4
               real32 vy = bs.BE_read_real32(); //  8
               real32 vz = bs.BE_read_real32(); // 12
               real32 vw = bs.BE_read_real32(); // 16
               uint32 a1 = bs.BE_read_uint16(); // 18 not float or half-float
               uint32 a2 = bs.BE_read_uint16(); // 22 not float or half-float
               real32 nx = bs.BE_read_real32(); // 24
               real32 ny = bs.BE_read_real32(); // 28
               real32 nz = bs.BE_read_real32(); // 32
               real32 nw = bs.BE_read_real32(); // 36
               real32 b1 = bs.BE_read_real32(); // 40
               real32 b2 = bs.BE_read_real32(); // 44
               real32 b3 = bs.BE_read_real32(); // 48
               real32 b4 = bs.BE_read_real32(); // 52
               real32 b5 = bs.BE_read_real32(); // 56 which ones are UV???
               real32 b6 = bs.BE_read_real32(); // 60
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << ", " << vw << "), "
                        << "n(" << j << ") = (" << nx << ", " << ny << ", " << nz << ", " << nw << "), "
                        << "a(" << j << ") = (" << a1 << ", " << a2 << "), "
                        << "b(" << j << ") = (" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << ", " << b5 << ", " << b6 << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               //vb.data[j].tu = tu;
               //vb.data[j].tv = tv;
              }
           cout << " 0x3C" << endl;
          }
        else if(vertsize == 0x44)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real16(); //  2
               real32 vy = bs.BE_read_real16(); //  4
               real32 vz = bs.BE_read_real16(); //  6
               real32 a1 = bs.BE_read_real16(); //  8
               real32 a2 = bs.BE_read_real16(); // 10 
               real32 a3 = bs.BE_read_real16(); // 12
               real32 a4 = bs.BE_read_real16(); // 14
               real32 a5 = bs.BE_read_real16(); // 16
               real32 a6 = bs.BE_read_real16(); // 18
               real32 a7 = bs.BE_read_real16(); // 20
               real32 a8 = bs.BE_read_real16(); // 22
               real32 a9 = bs.BE_read_real16(); // 24
               real32 b1 = bs.BE_read_real16(); // 26
               real32 b2 = bs.BE_read_real16(); // 28
               real32 b3 = bs.BE_read_real16(); // 30
               real32 b4 = bs.BE_read_real16(); // 32
               uint32 c1 = bs.BE_read_uint08(); // 33
               uint32 c2 = bs.BE_read_uint08(); // 34
               uint32 c3 = bs.BE_read_uint08(); // 35
               uint32 c4 = bs.BE_read_uint08(); // 36
               uint32 c5 = bs.BE_read_uint08(); // 37
               uint32 c6 = bs.BE_read_uint08(); // 38
               uint32 c7 = bs.BE_read_uint08(); // 39
               uint32 c8 = bs.BE_read_uint08(); // 40
               uint32 c9 = bs.BE_read_uint08(); // 41
               uint32 ca = bs.BE_read_uint08(); // 42
               uint32 cb = bs.BE_read_uint08(); // 43
               uint32 cc = bs.BE_read_uint08(); // 44
               uint32 cd = bs.BE_read_uint08(); // 45
               uint32 ce = bs.BE_read_uint08(); // 46
               uint32 cf = bs.BE_read_uint08(); // 47
               uint32 cg = bs.BE_read_uint08(); // 48
               real32 d1 = bs.BE_read_real16(); // 50
               real32 d2 = bs.BE_read_real16(); // 52
               real32 d3 = bs.BE_read_real16(); // 54
               real32 d4 = bs.BE_read_real16(); // 56
               real32 e1 = bs.BE_read_real16(); // 58
               real32 e2 = bs.BE_read_real16(); // 60
               real32 e3 = bs.BE_read_real16(); // 62
               real32 e4 = bs.BE_read_real16(); // 64
               real32 tu = bs.BE_read_real16(); // 66
               real32 tv = bs.BE_read_real16(); // 68
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << "), "
                        << "t(" << j << ") = (" << tu << ", " << tv << "), "
                        << "a(" << j << ") = (" << a1 << ", " << a2 << ", " << a3 << ", " << a4 << ", " << a5 << ", " << a6 << ", " << a7 << ", " << a8 << ", " << a9  << "), "
                        << "b(" << j << ") = (" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << "), "
                        << "c(" << j << ") = (" << c1 << ", " << c2 << ", " << c3 << ", " << c4 << ", " << c5 << ", " << c6 << ", " << c7 << ", " << c8 << ", " << c9 << ", " << ca << ", " << cc << ", " << cc << ", " << cd << ", " << ce << ", " << cf << ", " << cg << "), "
                        << "d(" << j << ") = (" << d1 << ", " << d2 << ", " << d3 << ", " << d4 << "), "
                        << "e(" << j << ") = (" << e1 << ", " << e2 << ", " << e3 << ", " << e4 << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x44" << endl;
          }
        else if(vertsize == 0x58)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); //  4 x
               real32 vy = bs.BE_read_real32(); //  8 y
               real32 vz = bs.BE_read_real32(); // 12 z
               real32 u1 = bs.BE_read_real32(); // 16 (float)
               real32 u2 = bs.BE_read_real32(); // 20 (float)
               real32 u3 = bs.BE_read_real32(); // 24 (float)
               real32 u4 = bs.BE_read_real32(); // 28 (float)
               real32 u5 = bs.BE_read_real32(); // 32 ???
               real32 u6 = bs.BE_read_real32(); // 36 (not float)
               real32 u7 = bs.BE_read_real32(); // 40 ???
               real32 u8 = bs.BE_read_real32(); // 44 (float)
               real32 u9 = bs.BE_read_real32(); // 48 (float)
               real32 ua = bs.BE_read_real32(); // 52 (float)
               real32 ub = bs.BE_read_real32(); // 56 (float)
               real32 p1 = bs.BE_read_uint08(); // 57 (byte)
               real32 p2 = bs.BE_read_uint08(); // 58 (byte)
               real32 p3 = bs.BE_read_uint08(); // 59 (byte)
               real32 p4 = bs.BE_read_uint08(); // 60 (byte)
               real32 p5 = bs.BE_read_uint08(); // 61 (byte)
               real32 p6 = bs.BE_read_uint08(); // 62 (byte)
               real32 p7 = bs.BE_read_uint08(); // 63 (byte)
               real32 p8 = bs.BE_read_uint08(); // 64 (byte)
               real32 p9 = bs.BE_read_uint08(); // 65 (byte)
               real32 pa = bs.BE_read_uint08(); // 66 (byte)
               real32 pb = bs.BE_read_uint08(); // 67 (byte)
               real32 pc = bs.BE_read_uint08(); // 68 (byte)
               real32 pd = bs.BE_read_uint08(); // 69 (byte)
               real32 pe = bs.BE_read_uint08(); // 70 (byte)
               real32 pf = bs.BE_read_uint08(); // 71 (byte)
               real32 pg = bs.BE_read_uint08(); // 72 (byte)
               real32 uc = bs.BE_read_real32(); // 84 (not float)
               real32 ud = bs.BE_read_real32(); // 88 (not float)
               real32 tu = bs.BE_read_real32(); // 92 u
               real32 tv = bs.BE_read_real32(); // 96 v
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   " << endl;
                  dfile << "   " << vx << endl;
                  dfile << "   " << vy << endl;
                  dfile << "   " << vz << endl;
                  dfile << "   " << u1 << endl;
                  dfile << "   " << u2 << endl;
                  dfile << "   " << u3 << endl;
                  dfile << "   " << u4 << endl;
                  dfile << "   " << u5 << endl;
                  dfile << "   " << u6 << endl;
                  dfile << "   " << u7 << endl;
                  dfile << "   " << u8 << endl;
                  dfile << "   " << u9 << endl;
                  dfile << "   " << ua << endl;
                  dfile << "   " << ub << endl;
                  dfile << "   " << p1 << endl;
                  dfile << "   " << p2 << endl;
                  dfile << "   " << p3 << endl;
                  dfile << "   " << p4 << endl;
                  dfile << "   " << p5 << endl;
                  dfile << "   " << p6 << endl;
                  dfile << "   " << p7 << endl;
                  dfile << "   " << p8 << endl;
                  dfile << "   " << p9 << endl;
                  dfile << "   " << pa << endl;
                  dfile << "   " << pb << endl;
                  dfile << "   " << pc << endl;
                  dfile << "   " << pd << endl;
                  dfile << "   " << pe << endl;
                  dfile << "   " << pf << endl;
                  dfile << "   " << pg << endl;
                  dfile << "   " << uc << endl;
                  dfile << "   " << ud << endl;
                  dfile << "   " << tu << endl;
                  dfile << "   " << tv << endl;
                  dfile << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x58" << endl;
          }
        else if(vertsize == 0x60)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); //  4 x
               real32 vy = bs.BE_read_real32(); //  8 y
               real32 vz = bs.BE_read_real32(); // 12 z
               real32 u1 = bs.BE_read_real32(); // 16 (float)
               real32 u2 = bs.BE_read_real32(); // 20 (float)
               real32 u3 = bs.BE_read_real32(); // 24 (float)
               real32 u4 = bs.BE_read_real32(); // 28 (float)
               real32 u5 = bs.BE_read_real32(); // 32 ???
               real32 u6 = bs.BE_read_real32(); // 36 (not float)
               real32 u7 = bs.BE_read_real32(); // 40 ???
               real32 u8 = bs.BE_read_real32(); // 44 (float)
               real32 u9 = bs.BE_read_real32(); // 48 (float)
               real32 ua = bs.BE_read_real32(); // 52 (float)
               real32 ub = bs.BE_read_real32(); // 56 (float)
               real32 p1 = bs.BE_read_uint08(); // 57 (byte)
               real32 p2 = bs.BE_read_uint08(); // 58 (byte)
               real32 p3 = bs.BE_read_uint08(); // 59 (byte)
               real32 p4 = bs.BE_read_uint08(); // 60 (byte)
               real32 p5 = bs.BE_read_uint08(); // 61 (byte)
               real32 p6 = bs.BE_read_uint08(); // 62 (byte)
               real32 p7 = bs.BE_read_uint08(); // 63 (byte)
               real32 p8 = bs.BE_read_uint08(); // 64 (byte)
               real32 p9 = bs.BE_read_uint08(); // 65 (byte)
               real32 pa = bs.BE_read_uint08(); // 66 (byte)
               real32 pb = bs.BE_read_uint08(); // 67 (byte)
               real32 pc = bs.BE_read_uint08(); // 68 (byte)
               real32 uc = bs.BE_read_real32(); // 72 (not float)
               real32 ud = bs.BE_read_real32(); // 76 ??
               real32 ue = bs.BE_read_real32(); // 80 ??
               real32 uf = bs.BE_read_real32(); // 84 (not float)
               real32 ug = bs.BE_read_real32(); // 88 (not float)
               real32 tu = bs.BE_read_real32(); // 92 u
               real32 tv = bs.BE_read_real32(); // 96 v
               tv = 1.0f - tv;
               if(debug) {
                  dfile << "   " << endl;
                  dfile << "   " << vx << endl;
                  dfile << "   " << vy << endl;
                  dfile << "   " << vz << endl;
                  dfile << "   " << u1 << endl;
                  dfile << "   " << u2 << endl;
                  dfile << "   " << u3 << endl;
                  dfile << "   " << u4 << endl;
                  dfile << "   " << u5 << endl;
                  dfile << "   " << u6 << endl;
                  dfile << "   " << u7 << endl;
                  dfile << "   " << u8 << endl;
                  dfile << "   " << u9 << endl;
                  dfile << "   " << ua << endl;
                  dfile << "   " << ub << endl;
                  dfile << "   " << p1 << endl;
                  dfile << "   " << p2 << endl;
                  dfile << "   " << p3 << endl;
                  dfile << "   " << p4 << endl;
                  dfile << "   " << p5 << endl;
                  dfile << "   " << p6 << endl;
                  dfile << "   " << p7 << endl;
                  dfile << "   " << p8 << endl;
                  dfile << "   " << p9 << endl;
                  dfile << "   " << pa << endl;
                  dfile << "   " << pb << endl;
                  dfile << "   " << pc << endl;
                  dfile << "   " << uc << endl;
                  dfile << "   " << ud << endl;
                  dfile << "   " << ue << endl;
                  dfile << "   " << uf << endl;
                  dfile << "   " << ug << endl;
                  dfile << "   " << tu << endl;
                  dfile << "   " << tv << endl;
                  dfile << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               vb.data[j].tu = tu;
               vb.data[j].tv = tv;
              }
           cout << " 0x60" << endl;
          }
        else if(vertsize == 0x68)
          {
           vb.flags |= VERTEX_POSITION;
           vb.flags |= VERTEX_NORMAL;
           vb.flags |= VERTEX_UV;
           for(size_t j = 0; j < vertices; j++)
              {
               real32 vx = bs.BE_read_real32(); //  4 x
               real32 vy = bs.BE_read_real32(); //  8 y
               real32 vz = bs.BE_read_real32(); // 12 z
               real32 vw = bs.BE_read_real32(); // 16
               real32 a1 = bs.BE_read_real32(); // 20 
               real32 a2 = bs.BE_read_real32(); // 24 
               real32 a3 = bs.BE_read_real32(); // 28 
               real32 a4 = bs.BE_read_real32(); // 32 
               uint32 b1 = bs.BE_read_uint32(); // 36 0xFFFFFFFF
               real32 c1 = bs.BE_read_real32(); // 40
               real32 c2 = bs.BE_read_real32(); // 44
               real32 c3 = bs.BE_read_real32(); // 48
               real32 c4 = bs.BE_read_real32(); // 52
               real32 d1 = bs.BE_read_real32(); // 56
               real32 d2 = bs.BE_read_real32(); // 60
               real32 d3 = bs.BE_read_real32(); // 64
               real32 d4 = bs.BE_read_real32(); // 68

               real32 e1 = bs.BE_read_uint08(); // 69 (byte)
               real32 e2 = bs.BE_read_uint08(); // 70 (byte)
               real32 e3 = bs.BE_read_uint08(); // 71 (byte)
               real32 e4 = bs.BE_read_uint08(); // 72 (byte)
               real32 f1 = bs.BE_read_uint08(); // 73 (byte)
               real32 f2 = bs.BE_read_uint08(); // 74 (byte)
               real32 f3 = bs.BE_read_uint08(); // 75 (byte)
               real32 f4 = bs.BE_read_uint08(); // 76 (byte)
               real32 g1 = bs.BE_read_uint08(); // 77 (byte)
               real32 g2 = bs.BE_read_uint08(); // 78 (byte)
               real32 g3 = bs.BE_read_uint08(); // 79 (byte)
               real32 g4 = bs.BE_read_uint08(); // 80 (byte)
               real32 h1 = bs.BE_read_uint08(); // 81 (byte)
               real32 h2 = bs.BE_read_uint08(); // 82 (byte)
               real32 h3 = bs.BE_read_uint08(); // 83 (byte)
               real32 h4 = bs.BE_read_uint08(); // 84 (byte)

               real32 i1 = bs.BE_read_real32(); //  88
               real32 i2 = bs.BE_read_real32(); //  92
               real32 i3 = bs.BE_read_real32(); //  96
               real32 i4 = bs.BE_read_real32(); // 100
               real32 i5 = bs.BE_read_real32(); // 104

               if(debug) {
                  dfile << "   ";
                  dfile << "v(" << j << ") = (" << vx << ", " << vy << ", " << vz << ", " << vw << "), "
                        //<< "t(" << j << ") = (" << tu << ", " << tv << "), "
                        << "a(" << j << ") = (" << a1 << ", " << a2 << ", " << a3 << ", " << a4 << "), "
                        << "b(" << j << ") = (" << hex << b1 << dec << "), "
                        << "c(" << j << ") = (" << c1 << ", " << c2 << ", " << c3 << ", " << c4 << "), "
                        << "d(" << j << ") = (" << d1 << ", " << d2 << ", " << d3 << ", " << d4 << "), "
                        << "e(" << j << ") = (" << e1 << ", " << e2 << ", " << e3 << ", " << e4 << "), "
                        << "f(" << j << ") = (" << f1 << ", " << f2 << ", " << f3 << ", " << f4 << "), "
                        << "g(" << j << ") = (" << g1 << ", " << g2 << ", " << g3 << ", " << g4 << "), "
                        << "h(" << j << ") = (" << h1 << ", " << h2 << ", " << h3 << ", " << h4 << "), "
                        << "i(" << j << ") = (" << i1 << ", " << i2 << ", " << i3 << ", " << i4 << ", " << i5 << ")" << endl;
                 }
               vb.data[j].vx = vx;
               vb.data[j].vy = vy;
               vb.data[j].vz = vz;
               //vb.data[j].tu = tu;
               //vb.data[j].tv = tv;
              }
           cout << " 0x68" << endl;
          }
        else {
           stringstream ss;
           ss << "Unknown vertex size 0x" << hex << vertsize << dec << ".";
           return error(ss.str().c_str());
          }

        // insert vertex buffer
        smc.vblist.push_back(vb);
       }
   }

 return true;
}

bool modelizer::processSECT0105(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool modelizer::processSECT0106(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool modelizer::processSECT0107(boost::shared_array<char> data, uint32 size)
{
 // build binary stream
 using namespace std;
 binary_stream bs(data, size);

 // read section information
 uint32 section_elem = bs.BE_read_uint32();
 if(debug) dfile << "  section_elem = 0x" << hex << section_elem << dec << endl;
 if(!section_elem) return true;

 // for each mesh
 for(size_t i = 0; i < section_elem; i++)
    {
     // read index buffer header
     uint32 numindex = bs.BE_read_uint32();
     uint32 datatype = bs.BE_read_uint32();
     uint32 unknown1 = bs.BE_read_uint32();
     if(debug) dfile << "  INDEX SET #" << i << endl;
     if(debug) dfile << "   numindex = 0x" << hex << numindex << dec << endl;
     if(debug) dfile << "   datatype = 0x" << hex << datatype << dec << endl;
     if(debug) dfile << "   unknown1 = 0x" << hex << unknown1 << dec << endl;

     // creaet mesh name
     stringstream meshname;
     meshname << " mesh_" << setfill('0') << setw(3) << i;

     // determine index buffer format
     uint32 indexsize = 0;
     if(datatype == 0x10) indexsize = sizeof(uint16);
     else if(datatype == 0x20) indexsize = sizeof(uint32);
     else return error("Unknown index buffer data type.");

     // read index buffer
     uint32 total_bytes = numindex*indexsize;
     boost::shared_array<char> ibdata(new char[total_bytes]);
     if(datatype == 0x10) bs.BE_read_array(reinterpret_cast<uint16*>(ibdata.get()), numindex);
     else if(datatype == 0x20) bs.BE_read_array(reinterpret_cast<uint32*>(ibdata.get()), numindex);
     if(debug) {
        if(datatype == 0x10) {
           uint16* temp = reinterpret_cast<uint16*>(ibdata.get());
           uint16 min_index = 0;
           uint16 max_index = 0;
           minimum(temp, numindex, min_index);
           maximum(temp, numindex, max_index);
           dfile << "   min index = 0x" << hex << min_index << dec << endl;
           dfile << "   max index = 0x" << hex << max_index << dec << endl;
           for(uint32 j = 0; j < numindex; j++) dfile << "   i(" << j << ") = " << temp[j] << endl;
          }
        else if(datatype == 0x20) {
           uint32* temp = reinterpret_cast<uint32*>(ibdata.get());
           uint32 min_index = 0;
           uint32 max_index = 0;
           minimum(temp, numindex, min_index);
           maximum(temp, numindex, max_index);
           dfile << "   min index = 0x" << hex << min_index << dec << endl;
           dfile << "   max index = 0x" << hex << max_index << dec << endl;
           for(uint32 j = 0; j < numindex; j++) dfile << "   i(" << j << ") = " << temp[j] << endl;
          }
       }

     // save index buffer
     SMC_IDX_BUFFER ib;
     ib.format = (datatype == 0x10 ? FACE_FORMAT_UINT_16 : FACE_FORMAT_UINT_32);
     ib.type = FACE_TYPE_TRISTRIP;
     ib.name = meshname.str();
     ib.elem = numindex;
     ib.data = ibdata;
     ib.tris = 0;
     ib.material = INVALID_MATERIAL;
     smc.iblist.push_back(ib);
    }

 return true;
}

bool modelizer::processSECT0108(boost::shared_array<char> data, uint32 size)
{
 // build binary stream
 using namespace std;
 binary_stream bs(data, size);

 // read section information
 uint32 section_elem = bs.BE_read_uint32();
 if(debug) dfile << "  section_elem = 0x" << hex << section_elem << dec << endl;
 if(debug) dfile << "  BUFFER ASSOCIATIONS" << endl;
 if(!section_elem) return true;

 // for each mesh
 smc.rblist.resize(section_elem);
 for(size_t i = 0; i < section_elem; i++)
    {
     // read item
     uint32 p01 = bs.BE_read_uint32(); // C00000D0
     uint32 p02 = bs.BE_read_uint32(); // 00000000 vbuffer index
     uint32 p03 = bs.BE_read_uint32(); // 00000000
     uint32 p04 = bs.BE_read_uint32(); // 00000000
     uint32 p05 = bs.BE_read_uint32(); // 00000000
     uint32 p06 = bs.BE_read_uint32(); // 00000000
     uint32 p07 = bs.BE_read_uint32(); // 00000000 zero-based unique id
     uint32 p08 = bs.BE_read_uint32(); // 00000000 ibuffer index
     uint32 p09 = bs.BE_read_uint32(); // 00000001
     uint32 p10 = bs.BE_read_uint32(); // 00000004
     uint32 p11 = bs.BE_read_uint32(); // 00000000 vbuffer start offset
     uint32 p12 = bs.BE_read_uint32(); // 00000042 number of vertices
     uint32 p13 = bs.BE_read_uint32(); // 00000000 ibuffer start offset
     uint32 p14 = bs.BE_read_uint32(); // 000000BE number of indices

     // save item
     SMC_REF_BUFFER item;
     item.vb_index = p02;
     item.ib_index = p08;
     item.vb_start = p11;
     item.vb_width = p12;
     item.ib_start = p13;
     item.ib_width = p14;
     smc.rblist[p07] = item;

     // print item
     if(debug) {
        dfile << "   " <<
                 "0x" << hex << setfill('0') << setw(8) << p01 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p02 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p03 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p04 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p05 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p06 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p07 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p08 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p09 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p10 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p11 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p12 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p13 << dec << " " <<
                 "0x" << hex << setfill('0') << setw(8) << p14 << dec << endl;
       }
    }

 return true;
}

bool modelizer::processSECT0109(boost::shared_array<char> data, uint32 size)
{
 return true;
}

};};

namespace X_SYSTEM { namespace X_GAME {

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