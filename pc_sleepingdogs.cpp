#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "pc_sleepingdogs.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   SLEEPING_DOGS

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processTEX(const string& filename)
{
 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // open perm file
 ifstream perm_file(filename.c_str(), ios::binary);
 if(!perm_file) return error("Failed to open perm file.");

 // open temp file
 stringstream ss;
 ss << pathname << shrtname << ".temp_bin";
 ifstream temp_file(ss.str().c_str(), ios::binary);
 if(!temp_file) return message("Failed to open temp file. It might be one of the files in UNKNOWN folder.");
 
 // get filesize
 perm_file.seekg(0, ios::end);
 uint32 perm_filesize = (uint32)perm_file.tellg();
 perm_file.seekg(0, ios::beg);
 if(perm_file.fail()) return error("Seek failure.");
 if(perm_filesize < 0x10) return true; // nothing to save

 // create save path
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // loop forever
 uint32 index = 0;
 for(;;)
    {
     // h01 = code
     // h02 + h04 = h03
     uint32 position = (uint32)perm_file.tellg();
 
     // read magic
     uint32 h01 = LE_read_uint32(perm_file);
     if(perm_file.fail()) break;

     // read sizes
     uint32 h02 = LE_read_uint32(perm_file); // sizes
     uint32 h03 = LE_read_uint32(perm_file); // sizes
     if(perm_file.fail()) return error("Read failure.");

     // read unknown
     uint32 h04 = LE_read_uint32(perm_file); // zero
     if(perm_file.fail()) return error("Read failure.");

     // read info data 
     // always read h02, it is the size of the data minus the 0x10-byte header
     boost::shared_array<char> info(new char[h02]);
     perm_file.read(info.get(), h02);
     if(perm_file.fail()) return error("Read failure.");

     // test for texture control header
     if(h01 == 0xCDBFA090)
       {
        // move to texture name
        binary_stream bs(info, h02);
        bs.seek(0x1C);
        if(bs.fail()) return error("Stream seek failure.");

        // read texture name
        char texture_name[0x24];
        bs.read(&texture_name[0], 0x24);
        if(bs.fail()) return error("Stream read failure.");

        // move to and read texture type
        bs.seek(0x44);
        uint32 texture_type = bs.LE_read_uint32();
        if(bs.fail()) return error("Stream read failure.");

        // move to texture dimensions
        bs.seek(0x4C);
        if(bs.fail()) return error("Stream seek failure.");

        // read texture dimensions
        uint16 dx = bs.LE_read_uint16();
        uint16 dy = bs.LE_read_uint16();
        uint16 mipmaps = bs.LE_read_uint16();
        if(bs.fail()) return error("Stream read failure.");

        // move to offset and size information
        bs.seek(0x70);
        if(bs.fail()) return error("Stream seek failure.");

        // read offset and size information
        uint32 dataoffs = bs.LE_read_uint32();
        uint32 datasize = bs.LE_read_uint32();
        if(bs.fail()) return error("Stream read failure.");

        // read texture data
        temp_file.seekg(dataoffs);
        if(temp_file.fail()) return error("Seek failure.");

        boost::shared_array<char> texture_data(new char[datasize]);
        temp_file.read(texture_data.get(), datasize);
        if(temp_file.fail()) return error("Read failure.");

        // create save filename
        stringstream ss;
        ss << savepath << texture_name << ".dds";

        // create output file
        ofstream ofile(ss.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create output file.");

        // save DDS header
        if(texture_type == 0x00) {
           DDS_HEADER ddsh;
           CreateA8R8G8B8DDSHeader(dx, dy, (mipmaps > 0 ? mipmaps - 1 : 0), FALSE, &ddsh);
           ofile.write("DDS ", 4);
           ofile.write((char*)&ddsh, sizeof(ddsh));
          }
        else if(texture_type == 0x01) {
           DDS_HEADER ddsh;
           CreateDXT1Header(dx, dy, (mipmaps > 0 ? mipmaps - 1 : 0), FALSE, &ddsh);
           ofile.write("DDS ", 4);
           ofile.write((char*)&ddsh, sizeof(ddsh));
          }
        else if(texture_type == 0x02) {
           DDS_HEADER ddsh;
           CreateDXT3Header(dx, dy, (mipmaps > 0 ? mipmaps - 1 : 0), FALSE, &ddsh);
           ofile.write("DDS ", 4);
           ofile.write((char*)&ddsh, sizeof(ddsh));
          }
        else if(texture_type == 0x03) {
           DDS_HEADER ddsh;
           CreateDXT5Header(dx, dy, (mipmaps > 0 ? mipmaps - 1 : 0), FALSE, &ddsh);
           ofile.write("DDS ", 4);
           ofile.write((char*)&ddsh, sizeof(ddsh));
          }
        // this is correct, however, only IRFANVIEW can view grayscale DDS files
        // TODO: change to BMP or TGA instead
        else if(texture_type == 0x06) {
           DDS_HEADER ddsh;
           CreateGrayscaleDDSHeader(dx, dy, (mipmaps > 0 ? mipmaps - 1 : 0), FALSE, &ddsh);
           ofile.write("DDS ", 4);
           ofile.write((char*)&ddsh, sizeof(ddsh));
          }
        else if(texture_type == 0x31) { // file #5798
           // DDS_HEADER ddsh;
           // CreateGrayscaleDDSHeader(dx, dy, (mipmaps > 0 ? mipmaps - 1 : 0), FALSE, &ddsh);
           // ofile.write("DDS ", 4);
           // ofile.write((char*)&ddsh, sizeof(ddsh));
           cout << " type 0x31: not supported yet - " << ss.str() << endl;
          }
        else if(texture_type == 0x100) { // file #2360
           // DDS_HEADER ddsh;
           // CreateGrayscaleDDSHeader(dx, dy, (mipmaps > 0 ? mipmaps - 1 : 0), FALSE, &ddsh);
           // ofile.write("DDS ", 4);
           // ofile.write((char*)&ddsh, sizeof(ddsh));
           cout << " type 100: not supported yet - " << ss.str() << endl;
          }
        else if(texture_type == 0x101) { // file #3242
           // DDS_HEADER ddsh;
           // CreateGrayscaleDDSHeader(dx, dy, (mipmaps > 0 ? mipmaps - 1 : 0), FALSE, &ddsh);
           // ofile.write("DDS ", 4);
           // ofile.write((char*)&ddsh, sizeof(ddsh));
           cout << " type 101: not supported yet - " << ss.str() << endl;
          }        
        else {
           stringstream ss;
           ss << "Unknown texture type 0x" << hex << texture_type << dec << ".";
           return error(ss);
          }

        // save data to output file
        ofile.write(texture_data.get(), datasize);
        if(ofile.fail()) return error("Write failure.");
       }
    }

 return true;
}

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

struct SDVTXBUFFER {
 uint32 p01; // identifier
 uint32 p02; // 0x92CDEC8F
 uint32 p03; // 0x00000000 (for vertex buffer)
 uint32 p04; // size of vertex buffer data
 uint32 p05; // offset from this value to vertex buffer data
 uint32 p06; // vertex stride
 uint32 p07; // number of vertices
 uint32 p08; // offset to 0x10 bytes
 uint32 p09; // offset to 0x10 bytes
 boost::shared_array<char> p10; // vertex buffer data
};

struct SDIDXBUFFER {
 uint32 p01; // identifier
 uint32 p02; // 0x92CDEC8F
 uint32 p03; // 0x00000001 (for index buffer)
 uint32 p04; // size of index buffer data
 uint32 p05; // offset from this value to index buffer data
 uint32 p06; // index stride
 uint32 p07; // number of index
 uint32 p08; // offset to 0x10 bytes
 uint32 p09; // offset to 0x10 bytes
 boost::shared_array<char> p10; // index buffer data
};

class SleepingDogsExtractor {
 private :
  bool debug;
  string filename;
  string pathname;
  string shrtname;
  string savepath;
 private :
  ifstream perm_file;
  ifstream temp_file;
  uint32 perm_size;
  uint32 temp_size;
 private :
  deque<SDVTXBUFFER> vblist;
  deque<SDIDXBUFFER> iblist;
 private :
  bool processSkeleton(binary_stream& bs, uint32 skip);
 public :
  bool extract(const string& fn);
 public :
  SleepingDogsExtractor();
 ~SleepingDogsExtractor();
 private :
  SleepingDogsExtractor(const SleepingDogsExtractor&);
  void operator =(const SleepingDogsExtractor&);
};

SleepingDogsExtractor::SleepingDogsExtractor() : debug(false)
{
 perm_size = 0;
 temp_size = 0;
}

SleepingDogsExtractor::~SleepingDogsExtractor()
{
}

bool SleepingDogsExtractor::processSkeleton(binary_stream& bs, uint32 skip)
{
 // move to and read ID #1
 bs.seek(skip + 0x0C);
 if(bs.fail()) return error("Stream seek failure.");
 uint32 id1 = bs.LE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");

 // move to and read ID #2
 bs.seek(skip + 0x18);
 if(bs.fail()) return error("Stream seek failure.");
 uint32 id2 = bs.LE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");

 // read name
 char name[0x24];
 bs.read(&name[0], 0x24);
 if(bs.fail()) return error("Stream read failure.");
 cout << " " << name << endl;

 // read ID #3
 uint32 id3 = bs.LE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");

 // read offset to an array of IDs
 uint32 offset = bs.LE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");

 // move to next section
 bs.seek(skip + 0x1E0);
 if(bs.fail()) return error("Stream seek failure.");

 // read number of joints
 uint32 n_joints = bs.LE_read_uint32();
 if(n_joints == 0x00) return error("Invalid number of joints.");
 cout << " Number of joints = 0x" << hex << n_joints << dec << endl;
 
 return true;
}

bool SleepingDogsExtractor::extract(const string& fn)
{
 // filename properties
 filename = fn;
 pathname = GetPathnameFromFilename(filename);
 shrtname = GetShortFilenameWithoutExtension(filename);

 // open perm file
 perm_file.open(filename.c_str(), ios::binary);
 if(!perm_file) return error("Failed to open perm file.");

 // open temp file
 // NOTE: it is OK if temp file is missing
 stringstream ss;
 ss << pathname << shrtname << ".temp_bin";
 temp_file.open(ss.str().c_str(), ios::binary);

 // get filesize
 perm_file.seekg(0, ios::end);
 perm_size = (uint32)perm_file.tellg();
 perm_file.seekg(0, ios::beg);
 if(perm_file.fail()) return error("Seek failure.");
 if(perm_size < 0x10) return true; // nothing to save

 // get filesize
 if(temp_file.is_open()) {
    temp_file.seekg(0, ios::end);
    temp_size = (uint32)temp_file.tellg();
    temp_file.seekg(0, ios::beg);
    if(temp_file.fail()) return error("Seek failure.");
   }

 // create save path
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // loop forever
 uint32 index = 0;
 for(;;)
    {
     // debug: print position
     uint32 position = (uint32)perm_file.tellg();
     //cout << "offset = 0x" << hex << position << dec << endl;

     // h01 = chunk identifier
     // h02 + h04 = h03
     // read magic
     uint32 h01 = LE_read_uint32(perm_file);
     if(perm_file.fail()) break;

     // read sizes
     uint32 h02 = LE_read_uint32(perm_file); // sizes
     uint32 h03 = LE_read_uint32(perm_file); // sizes
     if(perm_file.fail()) return error("Read failure.");

     // read offset into data stream
     // sometimes the chunk identifier lands on a 0x08-byte aligned boundary
     // rather than a 0x10-byte aligned boundary and in these cases h04 will
     // be 0x08 that tells us to skip the first 0x08 bytes in the stream
     uint32 h04 = LE_read_uint32(perm_file);
     if(perm_file.fail()) return error("Read failure.");

     // read info data 
     // always read h02, it is the size of the data minus the 0x10-byte header
     boost::shared_array<char> info(new char[h02]);
     perm_file.read(info.get(), h02);
     if(perm_file.fail()) return error("Read failure.");

     // create binary stream
     if(h02 == 0) return true; // nothing to do
     binary_stream bs(info, h02);

     // vertex or index buffer
     if(h01 == 0x7A971479)
       {
        // move to buffer name
        bs.seek(h04 + 0x1C);
        if(bs.fail()) return error("Stream seek failure.");

        // read buffer name
        char name[0x24];
        bs.read(&name[0], 0x24);
        if(bs.fail()) return error("Stream read failure.");

        // read identifier
        bs.seek(h04 + 0x0C);
        if(bs.fail()) return error("Stream seek failure.");
        uint32 p01 = bs.LE_read_uint32();
        if(bs.fail()) return error("Stream read failure.");

        // read identifier
        bs.seek(h04 + 0x18);
        if(bs.fail()) return error("Stream seek failure.");
        uint32 p02 = bs.LE_read_uint32();
        if(bs.fail()) return error("Stream read failure.");

        // read buffer type
        bs.seek(h04 + 0x40);
        if(bs.fail()) return error("Stream seek failure.");
        uint32 p03 = bs.LE_read_uint32();
        if(bs.fail()) return error("Stream read failure.");

        // read buffer parameters
        uint32 p04 = bs.LE_read_uint32();
        uint32 p05 = bs.LE_read_uint32();
        uint32 p06 = bs.LE_read_uint32();
        uint32 p07 = bs.LE_read_uint32();
        uint32 p08 = bs.LE_read_uint32();
        if(bs.fail()) return error("Stream read failure.");

        // read buffer parameters
        bs.seek(h04 + 0x60);
        if(bs.fail()) return error("Stream seek failure.");
        uint32 p09 = bs.LE_read_uint32();
        if(bs.fail()) return error("Stream read failure.");

        // nothing to save
        if(p04 == 0) continue;

        // move to buffer data
        bs.seek(h04 + 0x48 + p05);
        if(bs.fail()) return error("Stream seek failure.");

        // read buffer data
        boost::shared_array<char> p10(new char[p04]);
        bs.read(p10.get(), p04);
        if(bs.fail()) return error("Stream read failure.");

        // vertex buffer
        if(p03 == 0x00000000) {
           SDVTXBUFFER vb;
           vb.p01 = p01;
           vb.p02 = p02;
           vb.p03 = p03;
           vb.p04 = p04;
           vb.p05 = p05;
           vb.p06 = p06;
           vb.p07 = p07;
           vb.p08 = p08;
           vb.p09 = p09;
           vb.p10 = p10;
           vblist.push_back(vb);
          }
        // index buffer
        else if(p03 == 0x00000001) {
           SDIDXBUFFER ib;
           ib.p01 = p01;
           ib.p02 = p02;
           ib.p03 = p03;
           ib.p04 = p04;
           ib.p05 = p05;
           ib.p06 = p06;
           ib.p07 = p07;
           ib.p08 = p08;
           ib.p09 = p09;
           ib.p10 = p10;
           iblist.push_back(ib);
          }
        // attribute buffer
        else if(p03 == 0x00000002) {
          }
        // skinning?
        else if(p03 == 0x10000) {
           // this is actually a vertex buffer 
           // p03 is probably two 16-bit values instead
          }
        else {
           return error("Unknown buffer type.");
          }
       }
     // bone palette
     else if(h01 == 0x982456DB)
       {
        // move to 0x40
        bs.seek(0x40);
        if(bs.fail()) return error("Stream seek failure.");

        // read parameters
        uint32 p01 = bs.LE_read_uint32(); // 0x01 ???
        uint32 p02 = bs.LE_read_uint32(); // number of joints
        uint32 p03 = bs.LE_read_uint32(); // offset to names
        uint32 p04 = bs.LE_read_uint32(); // offset to data part 1
        uint32 p05 = bs.LE_read_uint32(); // offset to data part 2

        // move to names
        bs.seek(0x48 + p03);
        if(bs.fail()) return error("Stream seek failure.");

        // read names
        for(uint32 i = 0; i < p02; i++) {
            char name[0x40];
            bs.read(&name[0], 0x40);
            if(bs.fail()) return error("Stream read failure.");
           }

        // process data part 1
        bs.seek(0x48 + p04);
        if(bs.fail()) return error("Stream seek failure.");
        for(uint32 i = 0; i < p02; i++) {
            sint08 c1 = bs.LE_read_sint08();
            sint08 c2 = bs.LE_read_sint08();
            sint08 c3 = bs.LE_read_sint08();
            sint08 c4 = bs.LE_read_sint08();
            real32 f1 = (c1 - 127.0f)/128.0f;
            real32 f2 = (c2 - 127.0f)/128.0f;
            real32 f3 = (c3 - 127.0f)/128.0f;
            real32 f4 = (c4 - 127.0f)/128.0f;
           }

        // process data part 2
        bs.seek(0x48 + p05);
        if(bs.fail()) return error("Stream seek failure.");
        for(uint32 i = 0; i < p02; i++) {
            sint08 c1 = bs.LE_read_sint08();
            sint08 c2 = bs.LE_read_sint08();
            sint08 c3 = bs.LE_read_sint08();
            sint08 c4 = bs.LE_read_sint08();
            real32 f1 = (c1 - 127.0f)/128.0f;
            real32 f2 = (c2 - 127.0f)/128.0f;
            real32 f3 = (c3 - 127.0f)/128.0f;
            real32 f4 = (c4 - 127.0f)/128.0f;
           }

       }
     // SKELETON DATA
     else if(h01 == 0x1418DD74) {
        if(!processSkeleton(bs, h04)) return false; 
       }
     // ???
     // else if(h01 == 0x???) {
     //   }
    }

 return true;
}

bool processMOD(const string& filename)
{
 SleepingDogsExtractor sde;
 return sde.extract(filename);
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
 using namespace std;
 std::string pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname();

 bool doREN = false;
 bool doTEX = false;
 bool doMOD = true;

 // rename bin archives
 if(doREN)
   {
    cout << "Renaming files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".BIN", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++)
       {
        // console information
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
    
        // do not process BIN files less than 10 characters long
        size_t len = filelist[i].length();
        if(len < 10) continue;
       
        // test for .perm.bin
        if(filelist[i][len - 9] == '.' &&
           filelist[i][len - 8] == 'p' &&
           filelist[i][len - 7] == 'e' &&
           filelist[i][len - 6] == 'r' &&
           filelist[i][len - 5] == 'm' &&
           filelist[i][len - 4] == '.' &&
           filelist[i][len - 3] == 'b' &&
           filelist[i][len - 2] == 'i' &&
           filelist[i][len - 1] == 'n')
          {
           string replname = filelist[i];
           replname[len - 4] = '_';
           MoveFileA(filelist[i].c_str(), replname.c_str());
          }
        // test for .temp.bin
        else if(filelist[i][len - 9] == '.' &&
           filelist[i][len - 8] == 't' &&
           filelist[i][len - 7] == 'e' &&
           filelist[i][len - 6] == 'm' &&
           filelist[i][len - 5] == 'p' &&
           filelist[i][len - 4] == '.' &&
           filelist[i][len - 3] == 'b' &&
           filelist[i][len - 2] == 'i' &&
           filelist[i][len - 1] == 'n')
          {
           string replname = filelist[i];
           replname[len - 4] = '_';
           MoveFileA(filelist[i].c_str(), replname.c_str());
          }
    
       }
    cout << endl;
   }

 // process archive
 if(doTEX) {
    cout << "Processing .PERM_BIN files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".PERM_BIN", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTEX(filelist[i])) return false;
       }
    cout << endl;
   }

 // process archive
 if(doMOD) {
    cout << "Processing .PERM_BIN files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".PERM_BIN", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMOD(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

}};

