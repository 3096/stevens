#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "pc_saints_row3.h"

#define X_SYSTEM PC
#define X_GAME   SAINTS_ROW_III

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
 public :
  bool extract(void);
 public :
  modelizer(const std::string& fname);
 ~modelizer();
};

modelizer::modelizer(const std::string& fname) : debug(false)
{
 filename = fname;
 pathname = GetPathnameFromFilename(fname);
 shrtname = GetShortFilenameWithoutExtension(fname);
}

modelizer::~modelizer()
{
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

namespace X_SYSTEM { namespace X_GAME {

class extractor {
 private :
  std::string pathname;
  bool debug;
 private :
  bool processSTR2(const std::string& filename);
  bool processCPEG(const std::string& filename);
  bool processCVBM(const std::string& filename);
  bool processCMSH(const std::string& filename);
  bool processLMSH(const std::string& filename);
  bool processRIGS(const std::string& filename);
 public :
  bool extract(void);
 public :
  extractor(const char* pn) : pathname(pn), debug(false) {}
 ~extractor() {}
};

bool extractor::extract(void)
{
 using namespace std;
 bool doSTR2 = true;
 bool doCPEG = true;
 bool doCVBM = true;
 bool doCMSH = true;
 bool doLMSH = false;
 bool doRIGS = true;

 cout << "STAGE 1" << endl;
 if(doSTR2) {
    cout << "Processing .STR2_PC files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".STR2_PC", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processSTR2(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 2" << endl;
 if(doCPEG) {
    cout << "Processing .CPEG_PC files... (NOTE: This may take a while.)" << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".CPEG_PC", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCPEG(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 3" << endl;
 if(doCVBM) {
    cout << "Processing .CVBM_PC files... (NOTE: This may take a while.)" << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".CVBM_PC", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCVBM(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 4" << endl;
 if(doCMSH) {
    cout << "Processing .CCMESH_PC files... (NOTE: This may take a while.)" << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".CCMESH_PC", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCMSH(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 5" << endl;
 if(doLMSH) {
    cout << "Processing .CLMESH_PC files... (NOTE: This may take a while.)" << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".CLMESH_PC", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processLMSH(filelist[i])) return false;
       }
    cout << endl;
   }

 cout << "STAGE 6" << endl;
 if(doRIGS) {
    cout << "Processing .RIG_PC files... (NOTE: This may take a while.)" << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".RIG_PC", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processRIGS(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

bool extractor::processSTR2(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open STR2_PC file.");

 // read magic
 uint32 magic = LE_read_uint32(ifile);
 if(magic != 0x51890ACE) return error("Invalid STR2_PC file.");

 // read version
 uint32 version = LE_read_uint32(ifile);
 if(version != 0x06) return error("Invalid STR2_PC file.");

 // move to fileheader data
 ifile.seekg(0x014C);
 uint32 h01 = LE_read_uint32(ifile);
 uint32 h02 = LE_read_uint32(ifile);
 uint32 nFileEntries = LE_read_uint32(ifile); // number of entries
 uint32 h04 = LE_read_uint32(ifile); // str2_pc filesize
 uint32 h05 = LE_read_uint32(ifile); // size of file information data
 uint32 h06 = LE_read_uint32(ifile); // size of filename list data
 uint32 h07 = LE_read_uint32(ifile); // uncompressed data size
 uint32 h08 = LE_read_uint32(ifile); // compressed data size
 if(h01 != 0x4803) return error("Expecting h01 = 0x4803.");
 if(h02 != 0x0000) return error("Expecting h02 = 0x0000.");

 // file information data
 struct STR2FILEENTRY {
  uint32 p01, p02, p03;
  uint32 p04, p05, p06;
  std::string filename;
 };
 deque<STR2FILEENTRY> fileinfo;
 deque<ZLIBINFO> zlibinfo;

 // read file information
 ifile.seekg(0x0800);
 for(uint32 i = 0; i < nFileEntries; i++)
    {
     // read item
     uint32 p01 = LE_read_uint32(ifile); // filename offset
     uint32 p02 = LE_read_uint32(ifile); // 0x00000000
     uint32 p03 = LE_read_uint32(ifile); // filedata offset
     uint32 p04 = LE_read_uint32(ifile); // filedata size
     uint32 p05 = LE_read_uint32(ifile); // compressed size
     uint32 p06 = LE_read_uint32(ifile); // 0x00000000

     // save item
     STR2FILEENTRY info;
     info.p01 = p01;
     info.p02 = p02;
     info.p03 = p03;
     info.p04 = p04; // uncompressed size
     info.p05 = p05; // compressed size
     info.p06 = p06;
     fileinfo.push_back(info);

     // decompressed files can have zero size
     if(info.p04 == 0x00000000 && info.p05 == 0xFFFFFFFF) {
        info.p04 = 0x00000000;
        info.p05 = 0x00000000;
       }

     // save item
     ZLIBINFO zinfo;
     zinfo.inflatedBytes = info.p04;
     zinfo.deflatedBytes = info.p05;
     zlibinfo.push_back(zinfo);
    }

 // move to string table
 uint32 position = (uint32)ifile.tellg();
 position = (position + 0x7FF) & (~0x7FF);
 ifile.seekg(position);

 // read string table
 for(uint32 i = 0; i < nFileEntries; i++)
    {
     // read string
     ifile.seekg(position + fileinfo[i].p01);
     char buffer[1024];
     if(!read_string(ifile, buffer, 1024)) return error("Failed to read string.");

     // save string
     // cout << " " << buffer << endl;
     fileinfo[i].filename = buffer;
     zlibinfo[i].filename = buffer;
    }

 // move to compressed data
 position = (uint32)ifile.tellg();
 position = (position + 0x7FF) & (~0x7FF);
 ifile.seekg(position);

 // create directory to store extracted files
 string str2path = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);
 str2path += shrtname;
 str2path += "\\";
 CreateDirectoryA(str2path.c_str(), NULL);

 // set filenames
 for(uint32 i = 0; i < nFileEntries; i++) {
     string tempname = str2path;
     tempname += fileinfo[i].filename;
     zlibinfo[i].filename = tempname;
    }

 // save compressed data
 DecompressZLIB(ifile, zlibinfo, 0); 
 return true;
}

bool extractor::processCPEG(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream cfile;
 cfile.open(filename.c_str(), ios::binary);
 if(!cfile) return error("CPEG_PC: Failed to open file.");

 // read header
 uint32 h01 = LE_read_uint32(cfile); // GEKV
 uint32 h02 = LE_read_uint32(cfile); // 0x0D
 uint32 h03 = LE_read_uint32(cfile); // cpeg filesize
 uint32 h04 = LE_read_uint32(cfile); // gpeg filesize
 uint32 h05 = LE_read_uint32(cfile); // number of files
 uint16 h06 = LE_read_uint16(cfile); // ???
 uint16 h07 = LE_read_uint16(cfile); // ???

 // validate header
 if(h01 != 0x564B4547) return error("CPEG_PC: Expecting GEKV.");
 if(h02 != 0x0D) return error("CPEG_PC: 0x0D.");
 if(h05 == 0) return true; // sometimes 0!

 // read file information
 deque<boost::shared_array<uint32>> fileinfo;
 for(uint32 i = 0; i < h05; i++) {
     boost::shared_array<uint32> item(new uint32[18]);
     item[ 0] = LE_read_uint32(cfile); // 0x0002AA0B (offset)
     item[ 1] = LE_read_uint32(cfile); // 0x00000000 (0x0000)
     item[ 2] = LE_read_uint32(cfile); // 0x01000100 (dx, dy)
     item[ 3] = LE_read_uint32(cfile); // 0x00000190 (flags)
     item[ 4] = LE_read_uint32(cfile); // 0x00010001 (flags)
     item[ 5] = LE_read_uint32(cfile); // 0x02000001 (flags)
     item[ 6] = LE_read_uint32(cfile); // 0x00000000 (flags)
     item[ 7] = LE_read_uint32(cfile); // 0x00000000 (flags)
     item[ 8] = LE_read_uint32(cfile); // 0x07010000 (number of mipmaps)
     item[ 9] = LE_read_uint32(cfile); // 0x0000AAA8 (datasize)
     item[10] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[11] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[12] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[13] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[14] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[15] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[16] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[17] = LE_read_uint32(cfile); // 0x00000000 (padding)
     fileinfo.push_back(item);
    }

 // read strings
 deque<string> filenames;
 for(uint32 i = 0; i < h05; i++) {
     char buffer[1024];
     read_string(cfile, buffer, 1024);
     filenames.push_back(buffer);
    }

 // create GPEG_PC filename
 string gname = GetPathnameFromFilename(filename);
 gname += GetShortFilenameWithoutExtension(filename);
 gname += ".";
 gname += "gpeg_pc";

 // open GPEG_PC file
 ifstream gfile;
 gfile.open(gname.c_str(), ios::binary);
 if(!gfile) return error("CPEG_PC: Failed to open GPEG_PC file.");

 // save files
 for(uint32 i = 0; i < h05; i++)
    {
     // create filename
     string fpath = GetPathnameFromFilename(filename);
     string fname = fpath;
     fname += GetShortFilenameWithoutExtension(filenames[i]);
     fname += ".dds";

     // obtain texture information
     boost::shared_array<uint32> info = fileinfo[i];
     uint32 GPEG_offset = info[0];
     uint32 GPEG_dx = (info[2] & 0xFFFF);
     uint32 GPEG_dy = (info[2] >> 16);
     uint32 GPEG_type = info[3];
     uint32 GPEG_mipmaps = (info[8] >> 24);
     uint32 GPEG_datasize = info[9];

     // read GPEG data
     boost::shared_array<char> GPEG_data(new char[GPEG_datasize]);
     gfile.seekg(GPEG_offset);
     if(gfile.fail()) return error("CPEG_PC: Failed to seek GPEG_PC offset.");
     gfile.read(GPEG_data.get(), GPEG_datasize);

     // DXT1 (FINISHED)
     if(GPEG_type == 0x190) {
        DDS_HEADER ddsh;
        if(!CreateDXT1Header(GPEG_dx, GPEG_dy, GPEG_mipmaps, FALSE, &ddsh)) return error("CPEG_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CPEG_PC: Failed to save DDS file.");
       }
     // DXT3 (finished)
     else if(GPEG_type == 0x191) {
        DDS_HEADER ddsh;
        if(!CreateDXT3Header(GPEG_dx, GPEG_dy, GPEG_mipmaps, FALSE, &ddsh)) return error("CPEG_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CPEG_PC: Failed to save DDS file.");
       }
     // DXT5 (FINISHED)
     else if(GPEG_type == 0x192) {
        DDS_HEADER ddsh;
        if(!CreateDXT5Header(GPEG_dx, GPEG_dy, GPEG_mipmaps, FALSE, &ddsh)) return error("CPEG_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CPEG_PC: Failed to save DDS file.");
       }
     // R5G6B5 (FINISHED)
     // A0R5G6B5 = 0x10, (0xF800, 0x7E0, 0x1F, 0x0000)
     // A1R5G5B5 = 0x10, (0x7C00, 0x3E0, 0x1F, 0x8000)
     // A4R4G4B4 = 0x10, (0x0F00, 0x0F0, 0x0F, 0xF000)
     else if(GPEG_type == 0x193) {
        DDS_HEADER ddsh;
        if(!CreateUncompressedDDSHeader(GPEG_dx, GPEG_dy, GPEG_mipmaps, 0x10, 0xF800, 0x7E0, 0x1F, 0x0000, FALSE, &ddsh)) return error("CPEG_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CPEG_PC: Failed to save DDS file.");
       }
     // ARGB (FINISHED)
     else if(GPEG_type == 0x197) {
        // cout << " 0x197: " << fname << endl;
        // cout << "  dx = " << GPEG_dx << endl;
        // cout << "  dy = " << GPEG_dy << endl;
        // cout << "  mipmaps = " << GPEG_mipmaps << endl;
        // cout << "  offset = 0x" << hex << GPEG_offset << dec << endl;
        // cout << "  datasize = 0x" << hex << GPEG_datasize << dec << endl;
        DDS_HEADER ddsh;
        if(!CreateUncompressedDDSHeader(GPEG_dx, GPEG_dy, GPEG_mipmaps, 0x00FF0000, 0x0000FF00, 0xFF, 0xFF000000, FALSE, &ddsh)) return error("CPEG_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CPEG_PC: Failed to save DDS file.");
       }
     // GRAYSCALE (FINISHED)
     else if(GPEG_type == 0x19a) {
        DDS_HEADER ddsh;
        if(!CreateUncompressedDDSHeader(GPEG_dx, GPEG_dy, GPEG_mipmaps, 0x08, 0xFF, 0xFF, 0xFF, 0x00, FALSE, &ddsh)) return error("CPEG_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CPEG_PC: Failed to save DDS file.");
       }
     else {
        stringstream ss;
        ss << "Unknown texture format 0x" << hex << GPEG_type << dec;
        return error(ss.str().c_str());
       }
    }

 return true;
}

bool extractor::processCVBM(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream cfile;
 cfile.open(filename.c_str(), ios::binary);
 if(!cfile) return error("CVBM_PC: Failed to open file.");

 // read header
 uint32 h01 = LE_read_uint32(cfile); // GEKV
 uint32 h02 = LE_read_uint32(cfile); // 0x0D
 uint32 h03 = LE_read_uint32(cfile); // cpeg filesize
 uint32 h04 = LE_read_uint32(cfile); // gpeg filesize
 uint32 h05 = LE_read_uint32(cfile); // number of files
 uint16 h06 = LE_read_uint16(cfile); // 0x04
 uint16 h07 = LE_read_uint16(cfile); // 0x10

 // validate header
 if(h01 != 0x564B4547) return error("CVBM_PC: Expecting GEKV.");
 if(h02 != 0x0D) return error("CVBM_PC: 0x0D.");
 if(h05 == 0) return true; // sometimes 0!

 // read file information
 deque<boost::shared_array<uint32>> fileinfo;
 for(uint32 i = 0; i < h05; i++) {
     boost::shared_array<uint32> item(new uint32[18]);
     item[ 0] = LE_read_uint32(cfile); // 0x0002AA0B (offset)
     item[ 1] = LE_read_uint32(cfile); // 0x00000000 (0x0000)
     item[ 2] = LE_read_uint32(cfile); // 0x01000100 (dx, dy)
     item[ 3] = LE_read_uint32(cfile); // 0x00000190 (flags)
     item[ 4] = LE_read_uint32(cfile); // 0x00010001 (flags)
     item[ 5] = LE_read_uint32(cfile); // 0x02000001 (flags)
     item[ 6] = LE_read_uint32(cfile); // 0x00000000 (flags)
     item[ 7] = LE_read_uint32(cfile); // 0x00000000 (flags)
     item[ 8] = LE_read_uint32(cfile); // 0x07010000 (number of mipmaps)
     item[ 9] = LE_read_uint32(cfile); // 0x0000AAA8 (datasize)
     item[10] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[11] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[12] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[13] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[14] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[15] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[16] = LE_read_uint32(cfile); // 0x00000000 (padding)
     item[17] = LE_read_uint32(cfile); // 0x00000000 (padding)
     fileinfo.push_back(item);
    }

 // read strings
 deque<string> filenames;
 for(uint32 i = 0; i < h05; i++) {
     char buffer[1024];
     read_string(cfile, buffer, 1024);
     filenames.push_back(buffer);
    }

 // create GPEG_PC filename
 string gname = GetPathnameFromFilename(filename);
 gname += GetShortFilenameWithoutExtension(filename);
 gname += ".";
 gname += "gvbm_pc";

 // open GPEG_PC file
 ifstream gfile;
 gfile.open(gname.c_str(), ios::binary);
 if(!gfile) return error("CVBM_PC: Failed to open GVBM_PC file.");

 // save files
 for(uint32 i = 0; i < h05; i++)
    {
     // create filename
     string fpath = GetPathnameFromFilename(filename);
     string fname = fpath;
     fname += GetShortFilenameWithoutExtension(filenames[i]);
     fname += ".dds";

     // obtain texture information
     boost::shared_array<uint32> info = fileinfo[i];
     uint32 GPEG_offset = info[0];
     uint32 GPEG_dx = (info[2] & 0xFFFF);
     uint32 GPEG_dy = (info[2] >> 16);
     uint32 GPEG_type = info[3];
     uint32 GPEG_mipmaps = (info[8] >> 24);
     uint32 GPEG_datasize = info[9];

     // read GPEG data
     boost::shared_array<char> GPEG_data(new char[GPEG_datasize]);
     gfile.seekg(GPEG_offset);
     if(gfile.fail()) return error("CVBM_PC: Failed to seek GVBM_PC offset.");
     gfile.read(GPEG_data.get(), GPEG_datasize);

     // DXT1 (FINISHED)
     if(GPEG_type == 0x190) {
        DDS_HEADER ddsh;
        if(!CreateDXT1Header(GPEG_dx, GPEG_dy, GPEG_mipmaps, FALSE, &ddsh)) return error("CVBM_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CVBM_PC: Failed to save DDS file.");
       }
     // DXT3 (finished)
     else if(GPEG_type == 0x191) {
        DDS_HEADER ddsh;
        if(!CreateDXT3Header(GPEG_dx, GPEG_dy, GPEG_mipmaps, FALSE, &ddsh)) return error("CVBM_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CVBM_PC: Failed to save DDS file.");
       }
     // DXT5 (FINISHED)
     else if(GPEG_type == 0x192) {
        DDS_HEADER ddsh;
        if(!CreateDXT5Header(GPEG_dx, GPEG_dy, GPEG_mipmaps, FALSE, &ddsh)) return error("CVBM_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CVBM_PC: Failed to save DDS file.");
       }
     // R5G6B5 (FINISHED)
     // A0R5G6B5 = 0x10, (0xF800, 0x7E0, 0x1F, 0x0000)
     // A1R5G5B5 = 0x10, (0x7C00, 0x3E0, 0x1F, 0x8000)
     // A4R4G4B4 = 0x10, (0x0F00, 0x0F0, 0x0F, 0xF000)
     else if(GPEG_type == 0x193) {
        DDS_HEADER ddsh;
        if(!CreateUncompressedDDSHeader(GPEG_dx, GPEG_dy, GPEG_mipmaps, 0x10, 0xF800, 0x7E0, 0x1F, 0x0000, FALSE, &ddsh)) return error("CVBM_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CVBM_PC: Failed to save DDS file.");
       }
     // ARGB (FINISHED)
     else if(GPEG_type == 0x197) {
        // cout << " 0x197: " << fname << endl;
        // cout << "  dx = " << GPEG_dx << endl;
        // cout << "  dy = " << GPEG_dy << endl;
        // cout << "  mipmaps = " << GPEG_mipmaps << endl;
        // cout << "  offset = 0x" << hex << GPEG_offset << dec << endl;
        // cout << "  datasize = 0x" << hex << GPEG_datasize << dec << endl;
        DDS_HEADER ddsh;
        if(!CreateUncompressedDDSHeader(GPEG_dx, GPEG_dy, GPEG_mipmaps, 0x00FF0000, 0x0000FF00, 0xFF, 0xFF000000, FALSE, &ddsh)) return error("CVBM_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CVBM_PC: Failed to save DDS file.");
       }
     // GRAYSCALE (FINISHED)
     else if(GPEG_type == 0x19a) {
        DDS_HEADER ddsh;
        if(!CreateUncompressedDDSHeader(GPEG_dx, GPEG_dy, GPEG_mipmaps, 0x08, 0xFF, 0xFF, 0xFF, 0x00, FALSE, &ddsh)) return error("CVBM_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CVBM_PC: Failed to save DDS file.");
       }
     else {
        stringstream ss;
        ss << "CVBM_PC: Unknown texture format 0x" << hex << GPEG_type << dec;
        return error(ss.str().c_str());
       }
    }

 return true;
}

bool extractor::processCMSH(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("CCMESH_PC: Failed to open file.");

 // create debug file
 std::ofstream dfile;
 if(debug) {
    stringstream ss;
    ss << GetPathnameFromFilename(filename);
    ss << GetShortFilenameWithoutExtension(filename);
    ss << ".txt"; 
    dfile.open(ss.str().c_str(), ios::binary);
    if(!dfile) return error("CCMESH_PC: Failed to create debug file.");
   }

 // read header
 uint32 h01 = LE_read_uint32(ifile);
 uint32 h02 = LE_read_uint32(ifile); // string table bytes
 uint32 h03 = LE_read_uint32(ifile);
 uint32 h04 = LE_read_uint32(ifile); // number of strings
 if(h01 != 0x00043854) return error("CCMESH_PC: Invalid file.");

 // read string table
 if(debug) dfile << "------------" << endl;
 if(debug) dfile << "STRING TABLE" << endl;
 if(debug) dfile << "------------" << endl;
 if(debug) dfile << endl;

 // move to 0x20
 ifile.seekg(0x20);
 if(ifile.fail()) return error("CCMESH_PC: Seek failure.");

 // read string table
 deque<string> stringTable;
 if(h02 && h04) {
    for(uint32 i = 0; i < h04; i++) {
        char buffer[1024];
        if(!read_string(ifile, buffer, 1024)) return error("CCMESH_PC: Read string failure.");
        stringTable.push_back(string(buffer));
        if(debug) dfile << " " << buffer << endl;
       }
     // read an extra byte
     // when table is 0x10-byte aligned, we need to read another row of zeroes
     char c;
     ifile.read(&c, 1);
    }
 // skip string table
 else if(!h02 && !h04) {
     char buffer[1024];
     ifile.read(&buffer[0], 0x10);
     if(ifile.fail()) return error("CCMESH_PC: Read failure.");
    }
 else return error("CCMESH_PC: Invalid string table.");
 if(debug) dfile << endl;

 // move to next 0x10 aligned position
 uint32 position = (uint32)ifile.tellg();
 position = ((position + 0x0F) & (~0x0F));
 ifile.seekg(position);
 if(ifile.fail()) return error("CCMESH_PC: Seek failure.");

 // read BKD. header
 uint32 BKD_h01 = LE_read_uint32(ifile); // .DKB
 uint32 BKD_h02 = LE_read_uint32(ifile); // 0x2A
 uint16 BKD_h03 = LE_read_uint16(ifile); // 0x00
 uint16 BKD_h04 = LE_read_uint16(ifile); // number of indices in index table (equal to the number of joints)
 uint16 BKD_h05 = LE_read_uint16(ifile); // 0x01, 0x07,
 uint16 BKD_h06 = LE_read_uint16(ifile); // 0x01
 uint16 BKD_h07 = LE_read_uint16(ifile); // ????
 uint16 BKD_h08 = LE_read_uint16(ifile); // ????
 uint32 BKD_h09 = LE_read_uint32(ifile); // ????
 uint32 BKD_h10 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h11 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h12 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h13 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h14 = LE_read_uint32(ifile); // ????
 real32 BKD_h15 = LE_read_real32(ifile); // ????
 real32 BKD_h16 = LE_read_real32(ifile); // ????
 real32 BKD_h17 = LE_read_real32(ifile); // ????
 uint16 BKD_h18 = LE_read_uint16(ifile); // 0x00 or 0x01
 uint16 BKD_h19 = LE_read_uint16(ifile); // number of 0x28 byte entries
 uint32 BKD_h20 = LE_read_uint32(ifile); // ????
 uint32 BKD_h21 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h22 = LE_read_uint32(ifile); // ????
 uint32 BKD_h23 = LE_read_uint32(ifile); // ????
 uint32 BKD_h24 = LE_read_uint32(ifile); // ????
 uint32 BKD_h25 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h26 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h27 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h28 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h29 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h30 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h31 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h32 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h33 = LE_read_uint32(ifile); // 0x01
 uint32 BKD_h34 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h35 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h36 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h37 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h38 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h39 = LE_read_uint32(ifile); // size of secondary string table (if present)
 uint32 BKD_h40 = LE_read_uint32(ifile); // 0x00

 // validate BKD. header
 if(BKD_h01 != 0x424BD00D) return error("CCMESH_PC: Invalid BKD.");

 // skip past secondary string table
 char sstable[2048];
 if(BKD_h39 == 0) ifile.read(&sstable[0], 0x10);
 else if(BKD_h39 < 0x800) {
    ifile.read(&sstable[0], ((BKD_h39 + 0x0F) & (~0x0F)));
    if(!(BKD_h39 % 0x10)) ifile.seekg(0x10, ios::cur);
   }
 else return error("CCMESH_PC: Secondary string table length is too large.");

 // read 0x28-byte entry table
 if(BKD_h18 == 1) {
    uint32 ET_h01 = LE_read_uint32(ifile); 
    uint32 ET_h02 = LE_read_uint32(ifile); 
    real32 ET_h03 = LE_read_real32(ifile); 
    real32 ET_h04 = LE_read_real32(ifile); 
    real32 ET_h05 = LE_read_real32(ifile); 
    real32 ET_h06 = LE_read_real32(ifile); 
    uint32 ET_h07 = LE_read_uint32(ifile); 
    uint32 ET_h08 = LE_read_uint32(ifile);
   }
 if(BKD_h19 != 0) {
    for(uint32 i = 0; i < BKD_h19; i++) {
        char entry[0x28];
        ifile.read(&entry[0], 0x28);
       }
   }

 // move to next 0x10 aligned position
 position = (uint32)ifile.tellg();
 position = ((position + 0x0F) & (~0x0F));
 ifile.seekg(position);
 if(ifile.fail()) return error("CCMESH_PC: Seek failure.");

 // read indices
 boost::shared_array<uint32> indexData;
 if(!(BKD_h04 < 0x800)) return error("CCMESH_PC: Unexpected number of index entries.");
 else if(BKD_h04 != 0) {
    indexData.reset(new uint32[BKD_h04]);
    LE_read_array(ifile, indexData.get(), BKD_h04);
    if(ifile.fail()) return error("CCMESH_PC: Read failure.");
   }

 // read model information
 if(debug) dfile << "-----------------" << endl;
 if(debug) dfile << "MODEL INFORMATION" << endl;
 if(debug) dfile << "-----------------" << endl;
 if(debug) dfile << endl;

 // skip 0x00 and scan for 0x09
 for(;;) {
     uint32 temp = LE_read_uint32(ifile);
     if(ifile.fail()) return error("CCMESH_PC: Read failure.");
     else if(temp == 0x00) continue;
     else if(temp == 0x09) break;
     else {
        stringstream ss;
        ss << "CCMESH_PC: Unexpected padding value at 0x" << hex << ((uint32)ifile.tellg() - 4) << dec;
        return error(ss.str().c_str());
       }
    }

 // read model identifier
 uint32 modelID = LE_read_uint32(ifile);
 if(modelID == 0) return error("CCMESH_PC: Invalid model identifier.");
 if(debug) dfile << " " << "modelID: 0x" << hex << modelID << dec << endl;

 // read size of model information section
 uint32 mis = LE_read_uint32(ifile);
 if(mis < 0x10) return error("CCMESH_PC: Invalid model information data.");
 mis -= 0x04;

 // read model information
 binary_stream mid(boost::shared_array<char>(new char[mis]), mis);
 uint32 mid_offset_begin = (uint32)ifile.tellg();
 ifile.read(&mid[0], mis);
 uint32 mid_offset_end = (uint32)ifile.tellg();

 // read model information header
 uint32 MID_h01 = mid.LE_read_uint32(); // size of gcmesh_pc file
 uint32 MID_h02 = mid.LE_read_uint32(); // 0x03
 uint32 MID_h03 = mid.LE_read_uint32(); // number of 0x30 byte data section after byte sequence
 uint32 MID_h04 = mid.LE_read_uint32(); // offset from 0x09 to 0x30 byte data section
 uint32 MID_h05 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h06 = mid.LE_read_uint32(); // number of vertex buffers
 uint32 MID_h07 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h08 = mid.LE_read_uint32(); // 0x80
 uint32 MID_h09 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h10 = mid.LE_read_uint32(); // number of indices
 uint32 MID_h11 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h12 = mid.LE_read_uint32(); // offset to index data
 uint32 MID_h13 = mid.LE_read_uint32(); // 0x00
 uint16 MID_h14 = mid.LE_read_uint16(); // 0x02
 uint16 MID_h15 = mid.LE_read_uint16(); // number of surfaces
 uint32 MID_h16 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h17 = mid.LE_read_uint32(); // number of bytes in byte sequence
 uint32 MID_h18 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h19 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h20 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h21 = mid.LE_read_uint32(); // 0x01 or 0x02
 uint32 MID_h22 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h23 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h24 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h25 = mid.LE_read_uint32(); // float
 uint32 MID_h26 = mid.LE_read_uint32(); // float
 uint32 MID_h27 = mid.LE_read_uint32(); // float
 uint32 MID_h28 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h29 = mid.LE_read_uint32(); // 0x00
 uint32 MID_h30 = mid.LE_read_uint32(); // 0x00
 if(debug) dfile << " " << "MID_offset_begin = 0x" << hex << mid_offset_begin << dec << endl;
 if(debug) dfile << " " << "MID_offset_end = 0x" << hex << mid_offset_end << dec << endl;
 if(debug) dfile << " " << "MID_h01: 0x" << hex << MID_h01 << dec << endl;
 if(debug) dfile << " " << "MID_h02: 0x" << hex << MID_h02 << dec << endl;
 if(debug) dfile << " " << "MID_h03: 0x" << hex << MID_h03 << dec << endl;
 if(debug) dfile << " " << "MID_h04: 0x" << hex << MID_h04 << dec << endl;
 if(debug) dfile << " " << "MID_h05: 0x" << hex << MID_h05 << dec << endl;
 if(debug) dfile << " " << "MID_h06: 0x" << hex << MID_h06 << dec << endl;
 if(debug) dfile << " " << "MID_h07: 0x" << hex << MID_h07 << dec << endl;
 if(debug) dfile << " " << "MID_h08: 0x" << hex << MID_h08 << dec << endl;
 if(debug) dfile << " " << "MID_h09: 0x" << hex << MID_h09 << dec << endl;
 if(debug) dfile << " " << "MID_h10: 0x" << hex << MID_h10 << dec << endl;
 if(debug) dfile << " " << "MID_h11: 0x" << hex << MID_h11 << dec << endl;
 if(debug) dfile << " " << "MID_h12: 0x" << hex << MID_h12 << dec << endl;
 if(debug) dfile << " " << "MID_h13: 0x" << hex << MID_h13 << dec << endl;
 if(debug) dfile << " " << "MID_h14: 0x" << hex << MID_h14 << dec << endl;
 if(debug) dfile << " " << "MID_h15: 0x" << hex << MID_h15 << dec << endl;
 if(debug) dfile << " " << "MID_h16: 0x" << hex << MID_h16 << dec << endl;
 if(debug) dfile << " " << "MID_h17: 0x" << hex << MID_h17 << dec << endl;
 if(debug) dfile << " " << "MID_h18: 0x" << hex << MID_h18 << dec << endl;
 if(debug) dfile << " " << "MID_h19: 0x" << hex << MID_h19 << dec << endl;
 if(debug) dfile << " " << "MID_h20: 0x" << hex << MID_h20 << dec << endl;
 if(debug) dfile << " " << "MID_h21: 0x" << hex << MID_h21 << dec << endl;
 if(debug) dfile << " " << "MID_h22: 0x" << hex << MID_h22 << dec << endl;
 if(debug) dfile << " " << "MID_h23: 0x" << hex << MID_h23 << dec << endl;
 if(debug) dfile << " " << "MID_h24: 0x" << hex << MID_h24 << dec << endl;
 if(debug) dfile << " " << "MID_h25: 0x" << hex << MID_h25 << dec << endl;
 if(debug) dfile << " " << "MID_h26: 0x" << hex << MID_h26 << dec << endl;
 if(debug) dfile << " " << "MID_h27: 0x" << hex << MID_h27 << dec << endl;
 if(debug) dfile << " " << "MID_h28: 0x" << hex << MID_h28 << dec << endl;
 if(debug) dfile << " " << "MID_h29: 0x" << hex << MID_h29 << dec << endl;
 if(debug) dfile << " " << "MID_h30: 0x" << hex << MID_h30 << dec << endl;
 if(debug) dfile << endl;

 // validate 0x30-byte data section information
 if(!MID_h04) return error("CCMESH_PC: There are no 0x30-byte data sections.");
 if(MID_h04 < 0xC) return error("CCMESH_PC: Invalid offset to 0x30-byte data sections.");

 // validate number of vertex buffers
 uint32 n_vertex_buffers = MID_h06;
 if(n_vertex_buffers < 0x01) return error("CCMESH_PC: Invalid number of vertex buffers.");
 if(n_vertex_buffers > 0xFF) return error("CCMESH_PC: Invalid number of vertex buffers.");

 // validate number of surfaces
 uint32 n_surfaces = MID_h15;
 if(n_surfaces < 0x01) return error("CCMESH_PC: Invalid number of surfaces.");
 if(n_surfaces > 0xFF) return error("CCMESH_PC: Invalid number of surfaces.");

 // read vertex buffer information
 if(debug) dfile << "-------------------------" << endl;
 if(debug) dfile << "VERTEX BUFFER INFORMATION" << endl;
 if(debug) dfile << "-------------------------" << endl;
 if(debug) dfile << endl;

 struct VERTEX_BUFFER_INFO {
  uint32 n_vertices;
  uint32 flags;
  uint32 unk01;
  uint32 unk02;
  uint32 start;
  uint32 unk03;
 };

 deque<VERTEX_BUFFER_INFO> vbinfo;
 for(uint32 i = 0; i < n_vertex_buffers; i++)
    {
     VERTEX_BUFFER_INFO info;
     info.n_vertices = mid.LE_read_uint32();
     info.flags = mid.LE_read_uint32();
     info.unk01 = mid.LE_read_uint32();
     info.unk02 = mid.LE_read_uint32();
     info.start = mid.LE_read_uint32();
     info.unk03 = mid.LE_read_uint32();
     vbinfo.push_back(info);
    }

 // read vertex buffer information
 if(debug) dfile << "---------------" << endl;
 if(debug) dfile << "UNKNOWN SECTION" << endl;
 if(debug) dfile << "---------------" << endl;
 if(debug) dfile << endl;

 // read 0x30-byte data sections
 mid.seek(MID_h04 - 0xC);
 if(mid.fail()) return error("CCMESH_PC: Stream seek failure.");
 for(uint16 i = 0; i < MID_h03; i++) {
     char temp[0x30];
     mid.LE_read_array(&temp[0], 0x30);
    }

 // model container
 ADVANCEDMODELCONTAINER amc;

 // read surface information
 if(debug) dfile << "--------" << endl;
 if(debug) dfile << "SURFACES" << endl;
 if(debug) dfile << "--------" << endl;
 if(debug) dfile << endl;

 // surface information
 typedef map<uint16, deque<AMC_REFERENCE>>::value_type smvt;
 map<uint16, deque<AMC_REFERENCE>> surface_map;

 // read surface information
 for(uint16 i = 0; i < n_surfaces; i++)
    {
     uint16 s01 = mid.LE_read_uint16(); // id
     uint16 s02 = mid.LE_read_uint16(); // vertex buffer index
     uint32 s03 = mid.LE_read_uint32();
     uint32 s04 = mid.LE_read_uint32();
     uint32 s05 = mid.LE_read_uint32();
     uint32 s06 = mid.LE_read_uint32();

     if(debug) {
        dfile << " Surface ID: 0x" << hex << s01 << dec << endl;
        dfile << " Reference to Vertex Buffer: 0x" << hex << s02 << dec << endl;
        dfile << " Index Buffer Start: 0x" << hex << s03 << dec << endl;
        dfile << " Number of Indices: 0x" << hex << s04 << dec << endl;
        dfile << " Vertex Buffer Start: 0x" << hex << s05 << dec << endl;
        dfile << " Number of Vertices: 0x" << hex << s06 << dec << endl;
        dfile << endl;
       }

      // find surface identifier
      auto iter = surface_map.find(i);
      if(iter == surface_map.end()) iter = surface_map.insert(smvt(i, deque<AMC_REFERENCE>())).first;

      // create reference
      AMC_REFERENCE ref;
      ref.vb_index = s02; // vbuffer index
      ref.vb_start = 0;   // vbuffer start index
      ref.vb_width = s06; // number of vertices
      ref.ib_index = 0;   // ibuffer index (only one index buffer)
      ref.ib_start = s03; // ibuffer start index
      ref.ib_width = s04; // number of indices
      ref.jm_index = AMC_INVALID_JMAP_INDEX;

     // insert reference
     iter->second.push_back(ref);
    }

 // process surface map
 deque<AMC_SURFACE> surfaces;
 for(auto iter = surface_map.begin(); iter != surface_map.end(); iter++)
    {
     // create surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << iter->first;

     // create surface
     AMC_SURFACE surface;
     surface.name = ss.str();
     surface.refs = iter->second;
     surface.surfmat = AMC_INVALID_SURFMAT; // no texture yet

     // save surface
     amc.surfaces.push_back(surface);
    }

 // GCMESH_PC filename
 stringstream ss;
 ss << GetPathnameFromFilename(filename);
 ss << GetShortFilenameWithoutExtension(filename);
 ss << ".gcmesh_pc"; 

 // open mesh file
 ifstream mfile(ss.str().c_str(), ios::binary);
 if(!mfile) return error("GCMESH_PC: Failed to open file.");

 // read face information
 if(debug) dfile << "----------------" << endl;
 if(debug) dfile << "FACE INFORMATION" << endl;
 if(debug) dfile << "----------------" << endl;
 if(debug) dfile << endl;

 // move to index buffer
 mfile.seekg(MID_h12);
 if(mfile.fail()) return error("GCMESH_PC: Index buffer seek failure.");

 // index buffer properties
 uint32 n_faces = MID_h10;
 uint32 bytes_per_index = MID_h14;
 if(bytes_per_index != 0x02) return error("GCMESH_PC: Expecting 0x02 byte indices.");

 // read index buffer
 uint32 ibsize = n_faces*bytes_per_index;
 boost::shared_array<char> ibdata(new char[ibsize]);
 mfile.read(ibdata.get(), ibsize);

 // save index buffer
 AMC_IDXBUFFER amc_ib;
 amc_ib.format = AMC_TRISTRIP;
 amc_ib.type = AMC_UINT16;
 amc_ib.wind = AMC_CW;
 amc_ib.name = "none";
 amc_ib.elem = n_faces; // n_faces is actually number of indices
 amc_ib.data = ibdata;
 amc.iblist.push_back(amc_ib);

 // read vertex information
 if(debug) dfile << "--------------" << endl;
 if(debug) dfile << "VERTEX BUFFERS" << endl;
 if(debug) dfile << "--------------" << endl;
 if(debug) dfile << endl;

 // for each vertex buffer
 for(uint32 i = 0; i < n_vertex_buffers; i++)
    {
     // vertex buffer information
     VERTEX_BUFFER_INFO vbi = vbinfo[i];
     if(debug) dfile << "Vertex Buffer #" << i << endl;

     // move to vertex buffer
     mfile.seekg(vbi.start);
     if(mfile.fail()) return error("GCMESH_PC: Vertex buffer seek failure.");

     // vertex buffer properties
     uint32 n_vertices = vbi.n_vertices;
     uint32 vertex_bytes = (vbi.flags & 0xFF);
     uint32 vertex_type = ((vbi.flags & 0xFF00) >> 8);
     if(debug) dfile << " vertices = " << n_vertices << endl;
     if(debug) dfile << " vertex size = " << vertex_bytes << endl;
     if(debug) dfile << " vertex type = " << vertex_type << endl;

     // read vertex buffer
     uint32 vbsize = n_vertices*vertex_bytes;
     boost::shared_array<char> vbdata(new char[vbsize]);
     mfile.read(vbdata.get(), vbsize);
     if(mfile.fail()) return error("GCMESH_PC: Vertex buffer read failure.");

     // print vertex buffer stream
     if(debug) {
        binary_stream bs(vbdata, vbsize);
        for(uint32 i = 0; i < n_vertices; i++) {
            dfile << " " << hex << setfill('0') << setw(4) << (uint32)i << dec << ": ";
            for(uint32 j = 0; j < vertex_bytes; j++) dfile << hex << setfill('0') << setw(2) << (uint32)bs.BE_read_uint08() << dec;
            dfile << endl;
           }
        dfile << endl;
       }

     // initialize vertex buffer
     AMC_VTXBUFFER amc_vb;
     amc_vb.elem = n_vertices;
     amc_vb.data.reset(new AMC_VERTEX[amc_vb.elem]);
     amc_vb.flags = 0;
     amc_vb.flags |= AMC_VERTEX_POSITION;
     amc_vb.flags |= AMC_VERTEX_NORMAL;
     amc_vb.flags |= AMC_VERTEX_UV;
     amc_vb.flags |= AMC_VERTEX_WEIGHTS;
     amc_vb.name = "default";
     amc_vb.uvchan = 1;
     amc_vb.uvtype[0] = AMC_DIFFUSE_MAP;
     amc_vb.uvtype[1] = AMC_INVALID_MAP;
     amc_vb.uvtype[2] = AMC_INVALID_MAP;
     amc_vb.uvtype[3] = AMC_INVALID_MAP;
     amc_vb.uvtype[4] = AMC_INVALID_MAP;
     amc_vb.uvtype[5] = AMC_INVALID_MAP;
     amc_vb.uvtype[6] = AMC_INVALID_MAP;
     amc_vb.uvtype[7] = AMC_INVALID_MAP;
     amc_vb.colors = 0;

     // process vertices
     binary_stream bs(vbdata, vbsize);
     for(uint32 j = 0; j < n_vertices; j++)
        {
         if(vertex_bytes == 0x1C)
           {
            if(vertex_type == 0x01)
              {
               // gang_nudelew
               // <23c6debc-0a4c3d3f-0d15bd3d>-<78-6b-fd-70>-<be-01-41-00>-<00-02-05-ff>-<2500-d601>
               // <987efc3a-7d9d3c3f-6a61bf3d>-<7f-6b-fd-70>-<c5-3a-00-00>-<00-06-ff-ff>-<1f00-cd01>
               // <ec74e2bc-9b26573f-7bc0dd3d>-<77-6b-fd-70>-<84-7a-00-00>-<00-05-ff-ff>-<4500-c101>
               // <8976e23c-604c3d3f-0c15bd3d>-<87-6b-fd-70>-<be-01-41-00>-<00-03-06-ff>-<2500-d601>
               real32 vx = bs.LE_read_real32();
               real32 vy = bs.LE_read_real32();
               real32 vz = bs.LE_read_real32();
               real32 nx = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 ny = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nz = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nw = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 w1 = bs.LE_read_uint08()/255.0f;
               real32 w2 = bs.LE_read_uint08()/255.0f;
               real32 w3 = bs.LE_read_uint08()/255.0f;
               real32 w4 = bs.LE_read_uint08()/255.0f;
               uint32 b1 = bs.LE_read_uint08();
               uint32 b2 = bs.LE_read_uint08();
               uint32 b3 = bs.LE_read_uint08();
               uint32 b4 = bs.LE_read_uint08();
               real32 tu = bs.LE_read_sint16()/1023.0f;
               real32 tv = bs.LE_read_sint16()/1023.0f;
               
               if(debug) {
                  dfile << " " << "<" << vx << ", " << vy << ", " << vz << ">";
                  dfile << " " << "<" << tu << ", " << tv << ">";
                  dfile << " " << "<" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << ">";
                  dfile << " " << "<" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << ">";
                  dfile << endl;
                 }
               
               amc_vb.data[j].vx = vx;
               amc_vb.data[j].vy = vy;
               amc_vb.data[j].vz = vz;
               amc_vb.data[j].nx = nx;
               amc_vb.data[j].ny = ny;
               amc_vb.data[j].nz = nz;
               amc_vb.data[j].wv[0] = w1;
               amc_vb.data[j].wv[1] = w2;
               amc_vb.data[j].wv[2] = w3;
               amc_vb.data[j].wv[3] = w4;
               amc_vb.data[j].wv[4] = 0.0f;
               amc_vb.data[j].wv[5] = 0.0f;
               amc_vb.data[j].wv[6] = 0.0f;
               amc_vb.data[j].wv[7] = 0.0f;
               amc_vb.data[j].wi[0] = (b1 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b1);
               amc_vb.data[j].wi[1] = (b2 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b2);
               amc_vb.data[j].wi[2] = (b3 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b3);
               amc_vb.data[j].wi[3] = (b4 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b4);
               amc_vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].uv[0][0] = tu;
               amc_vb.data[j].uv[0][1] = tv;
              }
            else
               return error("Incorrect 0x1C vertex type.");
           }
         else if(vertex_bytes == 0x20)
           {
            if(vertex_type == 0x01)
              {
               // bank_desk_prop
               // 0000: <8415f0be-24fd7bbe-d2dd413e>-<7f-7f-ff-13>-<ff-00-00-00>-<00-ff-ff-ff>-<23-03-72-00>-<0004-0000>
               // 0001: <b8cd003f-24fd7bbe-f9dd413e>-<7f-7f-ff-13>-<ff-00-00-00>-<00-ff-ff-ff>-<68-01-72-00>-<0004-0004>
               // 0002: <8415f0be-964f703e-d2dd413e>-<7f-7f-ff-13>-<ff-00-00-00>-<00-ff-ff-ff>-<23-03-08-00>-<0000-0000>
               // 0003: <b8cd003f-964f703e-f9dd413e>-<7f-7f-ff-13>-<ff-00-00-00>-<00-ff-ff-ff>-<68-01-08-00>-<0000-0004>
               real32 vx = bs.LE_read_real32();
               real32 vy = bs.LE_read_real32();
               real32 vz = bs.LE_read_real32();
               real32 nx = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 ny = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nz = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nw = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 w1 = bs.LE_read_uint08()/255.0f;
               real32 w2 = bs.LE_read_uint08()/255.0f;
               real32 w3 = bs.LE_read_uint08()/255.0f;
               real32 w4 = bs.LE_read_uint08()/255.0f;
               uint32 b1 = bs.LE_read_uint08();
               uint32 b2 = bs.LE_read_uint08();
               uint32 b3 = bs.LE_read_uint08();
               uint32 b4 = bs.LE_read_uint08();
               uint32 u1 = bs.LE_read_uint08();
               uint32 u2 = bs.LE_read_uint08();
               uint32 u3 = bs.LE_read_uint08();
               uint32 u4 = bs.LE_read_uint08();
               real32 tu = bs.LE_read_sint16()/1023.0f;
               real32 tv = bs.LE_read_sint16()/1023.0f;

               if(debug) {
                  dfile << " " << "<" << vx << ", " << vy << ", " << vz << ">";
                  dfile << " " << "<" << nx << ", " << ny << ", " << nz << ", " << nw << ">";
                  dfile << " " << "<" << tu << ", " << tv << ">";
                  dfile << " " << "<" << u1 << ", " << u2 << ", " << u3 << ", " << u4 << ">";
                  dfile << " " << "<" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << ">";
                  dfile << " " << "<" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << ">";
                  dfile << endl;
                 }
        
               amc_vb.data[j].vx = vx;
               amc_vb.data[j].vy = vy;
               amc_vb.data[j].vz = vz;
               amc_vb.data[j].nx = nx;
               amc_vb.data[j].ny = ny;
               amc_vb.data[j].nz = nz;
               amc_vb.data[j].wv[0] = w1;
               amc_vb.data[j].wv[1] = w2;
               amc_vb.data[j].wv[2] = w3;
               amc_vb.data[j].wv[3] = w4;
               amc_vb.data[j].wv[4] = 0.0f;
               amc_vb.data[j].wv[5] = 0.0f;
               amc_vb.data[j].wv[6] = 0.0f;
               amc_vb.data[j].wv[7] = 0.0f;
               amc_vb.data[j].wi[0] = (b1 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b1);
               amc_vb.data[j].wi[1] = (b2 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b2);
               amc_vb.data[j].wi[2] = (b3 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b3);
               amc_vb.data[j].wi[3] = (b4 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b4);
               amc_vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].uv[0][0] = tu;
               amc_vb.data[j].uv[0][1] = tv;
              }
            else if(vertex_type == 0x03)
              {
               // brianne
               // 0000: <2c2e4c3e-28ca623f-7013aebc>-<6b-01-76-74>-<fd-6b-7e-00>-<55-96-0f-04>-<00-03-01-04>-<1301-4903>
               // 0001: <708c3b3e-3634713f-2f0f63bc>-<f9-a1-6d-74>-<a0-04-73-00>-<6b-47-34-1a>-<00-03-01-04>-<1901-1e03>
               // 0002: <8ae7423e-3a8c603f-428f073d>-<95-02-71-74>-<78-70-fd-00>-<ff-00-00-00>-<03-ff-ff-ff>-<4001-4b03>
               // 0003: <4aa6213e-71e4993f-e061373c>-<fd-7e-91-74>-<7a-02-99-00>-<b0-39-0d-09>-<07-04-0c-00>-<3901-5d02>
               // 0004: <682b0b3e-42c2943f-b732a63d>-<df-99-cf-74>-<8a-03-9b-00>-<1e-34-59-53>-<00-01-04-07>-<6501-8202>
               real32 vx = bs.LE_read_real32();
               real32 vy = bs.LE_read_real32();
               real32 vz = bs.LE_read_real32();
               real32 nx = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 ny = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nz = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nw = (bs.LE_read_uint08() - 127.0f)/128.0f;
               uint32 u1 = bs.LE_read_uint16();
               uint32 u2 = bs.LE_read_uint16();
               real32 w1 = bs.LE_read_uint08()/255.0f;
               real32 w2 = bs.LE_read_uint08()/255.0f;
               real32 w3 = bs.LE_read_uint08()/255.0f;
               real32 w4 = bs.LE_read_uint08()/255.0f;
               uint32 b1 = bs.LE_read_uint08();
               uint32 b2 = bs.LE_read_uint08();
               uint32 b3 = bs.LE_read_uint08();
               uint32 b4 = bs.LE_read_uint08();
               real32 tu = bs.LE_read_sint16()/1023.0f;
               real32 tv = bs.LE_read_sint16()/1023.0f;

               if(debug) {
                  dfile << " " << "<" << vx << ", " << vy << ", " << vz << ">";
                  dfile << " " << "<" << nx << ", " << ny << ", " << nz << ", " << nw << ">";
                  dfile << " " << "<" << tu << ", " << tv << ">";
                  dfile << " " << "<" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << ">";
                  dfile << " " << "<" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << ">";
                  dfile << endl;
                 }
        
               amc_vb.data[j].vx = vx;
               amc_vb.data[j].vy = vy;
               amc_vb.data[j].vz = vz;
               amc_vb.data[j].nx = nx;
               amc_vb.data[j].ny = ny;
               amc_vb.data[j].nz = nz;
               amc_vb.data[j].wv[0] = w1;
               amc_vb.data[j].wv[1] = w2;
               amc_vb.data[j].wv[2] = w3;
               amc_vb.data[j].wv[3] = w4;
               amc_vb.data[j].wv[4] = 0.0f;
               amc_vb.data[j].wv[5] = 0.0f;
               amc_vb.data[j].wv[6] = 0.0f;
               amc_vb.data[j].wv[7] = 0.0f;
               amc_vb.data[j].wi[0] = (b1 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b1);
               amc_vb.data[j].wi[1] = (b2 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b2);
               amc_vb.data[j].wi[2] = (b3 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b3);
               amc_vb.data[j].wi[3] = (b4 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b4);
               amc_vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].uv[0][0] = tu;
               amc_vb.data[j].uv[0][1] = tv;
              }
            else
               return error("Incorrect 0x20 vertex type.");
           }
         else if(vertex_bytes == 0x24)
           {
            if(vertex_type == 0x01)
              {
               // sp_vtol01_parked (TODO: Normals still messed up here!)
               // 0052: <15a618bf-6ca2ed3f-c252ae40>-<e4-40-52-77>-<ff-00-00-00>-<00-ff-ff-ff>-<c902-4c02>-<f8f6-a973>-<f8f6-a973>
               // 0053: <d812dcbe-bb6b0540-ad39b040>-<d9-2d-59-77>-<ff-00-00-00>-<00-ff-ff-ff>-<9002-6002>-<8113-1119>-<8113-1119>
               // 0054: <b70b11bf-07c2df3f-eed2b440>-<e8-45-54-77>-<ff-00-00-00>-<00-ff-ff-ff>-<bf02-9002>-<6597-bc2e>-<6597-bc2e>
               // 0055: <cabacabe-16b1fa3f-c2b7b940>-<df-38-52-77>-<ff-00-00-00>-<00-ff-ff-ff>-<8502-c402>-<da18-1119>-<da18-1119>
               real32 vx = bs.LE_read_real32();
               real32 vy = bs.LE_read_real32();
               real32 vz = bs.LE_read_real32();
               real32 nx = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 ny = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nz = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nw = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 w1 = bs.LE_read_uint08()/255.0f;
               real32 w2 = bs.LE_read_uint08()/255.0f;
               real32 w3 = bs.LE_read_uint08()/255.0f;
               real32 w4 = bs.LE_read_uint08()/255.0f;
               uint32 b1 = bs.LE_read_uint08();
               uint32 b2 = bs.LE_read_uint08();
               uint32 b3 = bs.LE_read_uint08();
               uint32 b4 = bs.LE_read_uint08();
               real32 tu = bs.LE_read_sint16()/1023.0f;
               real32 tv = bs.LE_read_sint16()/1023.0f;
               real32 u1 = bs.LE_read_sint16(); // unknown
               real32 u2 = bs.LE_read_sint16(); // unknown
               real32 u3 = bs.LE_read_sint16(); // unknown
               real32 u4 = bs.LE_read_sint16(); // unknown

               if(debug) {
                  dfile << " " << "<" << vx << ", " << vy << ", " << vz << ">";
                  dfile << " " << "<" << nx << ", " << ny << ", " << nz << ", " << nw << ">";
                  dfile << " " << "<" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << ">";
                  dfile << " " << "<" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << ">";
                  dfile << " " << "<" << tu << ", " << tv << ">";
                  dfile << endl;
                 }
    
               amc_vb.data[j].vx = vx;
               amc_vb.data[j].vy = vy;
               amc_vb.data[j].vz = vz;
               amc_vb.data[j].nx = nx;
               amc_vb.data[j].ny = ny;
               amc_vb.data[j].nz = nz;
               amc_vb.data[j].wv[0] = w1;
               amc_vb.data[j].wv[1] = w2;
               amc_vb.data[j].wv[2] = w3;
               amc_vb.data[j].wv[3] = w4;
               amc_vb.data[j].wv[4] = 0.0f;
               amc_vb.data[j].wv[5] = 0.0f;
               amc_vb.data[j].wv[6] = 0.0f;
               amc_vb.data[j].wv[7] = 0.0f;
               amc_vb.data[j].wi[0] = (b1 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b1);
               amc_vb.data[j].wi[1] = (b2 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b2);
               amc_vb.data[j].wi[2] = (b3 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b3);
               amc_vb.data[j].wi[3] = (b4 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b4);
               amc_vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].uv[0][0] = tu;
               amc_vb.data[j].uv[0][1] = tv;
              }
            else if(vertex_type == 0x03)
              {
               // cyberavatar_npc
               // 0000: <157717bf-37bf7d3f-991fd23d>-<62-11-45-70>-<e6-49-b2-ff>-<ff-00-00-00>-<22-ff-ff-ff>-<3a00-2401>-<fc01-e802>
               // 0001: <6cd518bf-9c027e3f-c8abd53d>-<18-56-41-70>-<af-09-7d-ff>-<ff-00-00-00>-<22-ff-ff-ff>-<3900-1f01>-<fc01-e602>
               // 0002: <03f316bf-c687813f-72cbbb3d>-<56-2f-25-70>-<d6-29-a3-ff>-<2d-d2-00-00>-<18-22-ff-ff>-<3200-1001>-<f201-f002>
               // 0003: <0d77173f-3cbf7d3f-bf1fd23d>-<9c-11-45-70>-<18-49-b2-00>-<ff-00-00-00>-<27-ff-ff-ff>-<3a00-2401>-<fc01-e802>
               real32 vx = bs.LE_read_real32();
               real32 vy = bs.LE_read_real32();
               real32 vz = bs.LE_read_real32();
               real32 nx = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 ny = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nz = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nw = (bs.LE_read_uint08() - 127.0f)/128.0f;
               uint32 u1 = bs.LE_read_uint08();
               uint32 u2 = bs.LE_read_uint08();
               uint32 u3 = bs.LE_read_uint08();
               uint32 u4 = bs.LE_read_uint08();
               real32 w1 = bs.LE_read_uint08()/255.0f;
               real32 w2 = bs.LE_read_uint08()/255.0f;
               real32 w3 = bs.LE_read_uint08()/255.0f;
               real32 w4 = bs.LE_read_uint08()/255.0f;
               uint32 b1 = bs.LE_read_uint08();
               uint32 b2 = bs.LE_read_uint08();
               uint32 b3 = bs.LE_read_uint08();
               uint32 b4 = bs.LE_read_uint08();
               real32 tu = bs.LE_read_sint16()/1023.0f;
               real32 tv = bs.LE_read_sint16()/1023.0f;
               real32 v1 = bs.LE_read_sint16()/1023.0f; // side view of model
               real32 v2 = bs.LE_read_sint16()/1023.0f; // side view of model

               if(debug) {
                  dfile << " " << "<" << vx << ", " << vy << ", " << vz << ">";
                  dfile << " " << "<" << nx << ", " << ny << ", " << nz << ", " << nw << ">";
                  dfile << " " << "<" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << ">";
                  dfile << " " << "<" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << ">";
                  dfile << " " << "<" << tu << ", " << tv << ">";
                  dfile << endl;
                 }
    
               amc_vb.data[j].vx = vx;
               amc_vb.data[j].vy = vy;
               amc_vb.data[j].vz = vz;
               amc_vb.data[j].nx = nx;
               amc_vb.data[j].ny = ny;
               amc_vb.data[j].nz = nz;
               amc_vb.data[j].wv[0] = w1;
               amc_vb.data[j].wv[1] = w2;
               amc_vb.data[j].wv[2] = w3;
               amc_vb.data[j].wv[3] = w4;
               amc_vb.data[j].wv[4] = 0.0f;
               amc_vb.data[j].wv[5] = 0.0f;
               amc_vb.data[j].wv[6] = 0.0f;
               amc_vb.data[j].wv[7] = 0.0f;
               amc_vb.data[j].wi[0] = (b1 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b1);
               amc_vb.data[j].wi[1] = (b2 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b2);
               amc_vb.data[j].wi[2] = (b3 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b3);
               amc_vb.data[j].wi[3] = (b4 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b4);
               amc_vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].uv[0][0] = tu;
               amc_vb.data[j].uv[0][1] = tv;
              }
            else
               return error("Incorrect 0x24 vertex type.");
           }
         else if(vertex_bytes == 0x28)
           {
            // cm_psg_hat_f_drivingcap
            // 0000: <a6a8b8bc-4b56dc3f-603dcf3d>-<79-00-89-74>-<fe-78-77-00>-<ff-00-00-00>-<00-ff-ff-ff>-<7802-e603>-<e805-a90e>-<00000000>
            // 0001: <31e6bdbc-7dc1dc3f-6948cf3d>-<73-fa-9d-74>-<8c-62-fb-00>-<ff-00-00-00>-<00-ff-ff-ff>-<7702-da03>-<e005-4d0e>-<00000000>
            // 0002: <31a82bbd-9a60dc3f-fb91b53d>-<73-00-89-74>-<f4-78-b2-00>-<ff-00-00-00>-<00-ff-ff-ff>-<c502-c703>-<1e08-c50d>-<00000000>
            // 0003: <cfb309bd-6044df3f-78199d3d>-<46-c7-d8-74>-<8e-21-d4-00>-<ff-00-00-00>-<00-ff-ff-ff>-<9902-6903>-<d706-190b>-<00000000>
            if(vertex_type == 0x03)
              {
               real32 vx = bs.LE_read_real32();
               real32 vy = bs.LE_read_real32();
               real32 vz = bs.LE_read_real32();
               real32 nx = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 ny = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nz = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nw = (bs.LE_read_uint08() - 127.0f)/128.0f;
               uint32 u1 = bs.LE_read_uint08();
               uint32 u2 = bs.LE_read_uint08();
               uint32 u3 = bs.LE_read_uint08();
               uint32 u4 = bs.LE_read_uint08();
               real32 w1 = bs.LE_read_uint08()/255.0f;
               real32 w2 = bs.LE_read_uint08()/255.0f;
               real32 w3 = bs.LE_read_uint08()/255.0f;
               real32 w4 = bs.LE_read_uint08()/255.0f;
               uint32 b1 = bs.LE_read_uint08();
               uint32 b2 = bs.LE_read_uint08();
               uint32 b3 = bs.LE_read_uint08();
               uint32 b4 = bs.LE_read_uint08();
               real32 tu = bs.LE_read_sint16()/1023.0f;
               real32 tv = bs.LE_read_sint16()/1023.0f;
               real32 v1 = bs.LE_read_sint16()/1023.0f;
               real32 v2 = bs.LE_read_sint16()/1023.0f;
               uint32 v3 = bs.LE_read_uint32();

               if(debug) {
                  dfile << " " << "<" << vx << ", " << vy << ", " << vz << ">";
                  dfile << " " << "<" << nx << ", " << ny << ", " << nz << ", " << nw << ">";
                  dfile << " " << "<" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << ">";
                  dfile << " " << "<" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << ">";
                  dfile << " " << "<" << tu << ", " << tv << ">";
                  dfile << endl;
                 }
    
               amc_vb.data[j].vx = vx;
               amc_vb.data[j].vy = vy;
               amc_vb.data[j].vz = vz;
               amc_vb.data[j].nx = nx;
               amc_vb.data[j].ny = ny;
               amc_vb.data[j].nz = nz;
               amc_vb.data[j].wv[0] = w1;
               amc_vb.data[j].wv[1] = w2;
               amc_vb.data[j].wv[2] = w3;
               amc_vb.data[j].wv[3] = w4;
               amc_vb.data[j].wv[4] = 0.0f;
               amc_vb.data[j].wv[5] = 0.0f;
               amc_vb.data[j].wv[6] = 0.0f;
               amc_vb.data[j].wv[7] = 0.0f;
               amc_vb.data[j].wi[0] = (b1 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b1);
               amc_vb.data[j].wi[1] = (b2 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b2);
               amc_vb.data[j].wi[2] = (b3 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b3);
               amc_vb.data[j].wi[3] = (b4 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b4);
               amc_vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].uv[0][0] = tu;
               amc_vb.data[j].uv[0][1] = tv;
              }
            else
               return error("Incorrect 0x28 vertex type.");
           }
         else if(vertex_bytes == 0x2C)
           {
            // 0000: <6f2b513f-eeabf83f-5cbf8240>-<57-f7-6c-41>-<ee-ab-ad-ff>-<ff-00-00-00>-<00-ff-ff-ff>-<2001-5e02>-<ae01-6e01>-<4634-caed>-<c604-3509>
            // 0001: <8e51553f-527ff93f-19f78240>-<9c-e9-c0-41>-<d8-3f-bf-ff>-<ff-00-00-00>-<00-ff-ff-ff>-<2001-5f02>-<af01-7001>-<6b34-d5ed>-<c604-3309>
            // 0002: <fe70503f-6ae0f73f-c6a98140>-<24-a6-cf-41>-<d1-79-e0-ff>-<ff-00-00-00>-<00-ff-ff-ff>-<2201-5d02>-<af01-7001>-<6234-6bed>-<c404-3609>
            // 0003: <5938353f-88b20940-aa9a7740>-<b8-de-bd-41>-<e4-34-94-ff>-<ff-00-00-00>-<00-ff-ff-ff>-<2c01-4e02>-<a001-6101>-<8133-2aeb>-<bb04-4d09>
            if(vertex_type == 0x03)
              {
               real32 vx = bs.LE_read_real32();
               real32 vy = bs.LE_read_real32();
               real32 vz = bs.LE_read_real32();
               real32 nx = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 ny = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nz = (bs.LE_read_uint08() - 127.0f)/128.0f;
               real32 nw = (bs.LE_read_uint08() - 127.0f)/128.0f;
               uint32 u1 = bs.LE_read_uint08();
               uint32 u2 = bs.LE_read_uint08();
               uint32 u3 = bs.LE_read_uint08();
               uint32 u4 = bs.LE_read_uint08();
               real32 w1 = bs.LE_read_uint08()/255.0f;
               real32 w2 = bs.LE_read_uint08()/255.0f;
               real32 w3 = bs.LE_read_uint08()/255.0f;
               real32 w4 = bs.LE_read_uint08()/255.0f;
               uint32 b1 = bs.LE_read_uint08();
               uint32 b2 = bs.LE_read_uint08();
               uint32 b3 = bs.LE_read_uint08();
               uint32 b4 = bs.LE_read_uint08();
               real32 tu = bs.LE_read_sint16()/1023.0f;
               real32 tv = bs.LE_read_sint16()/1023.0f;
               real32 v1 = bs.LE_read_sint16()/1023.0f; // alternate UV map?
               real32 v2 = bs.LE_read_sint16()/1023.0f; // alternate UV map?
               real32 v3 = bs.LE_read_sint16()/1023.0f; // alternate UV map?
               real32 v4 = bs.LE_read_sint16()/1023.0f; // alternate UV map?
               real32 v5 = bs.LE_read_sint16()/1023.0f; // alternate UV map?
               real32 v6 = bs.LE_read_sint16()/1023.0f; // alternate UV map?

               if(debug) {
                  dfile << " " << "<" << vx << ", " << vy << ", " << vz << ">";
                  dfile << " " << "<" << nx << ", " << ny << ", " << nz << ", " << nw << ">";
                  dfile << " " << "<" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << ">";
                  dfile << " " << "<" << b1 << ", " << b2 << ", " << b3 << ", " << b4 << ">";
                  dfile << " " << "<" << tu << ", " << tv << ">";
                  dfile << endl;
                 }
    
               amc_vb.data[j].vx = vx;
               amc_vb.data[j].vy = vy;
               amc_vb.data[j].vz = vz;
               amc_vb.data[j].nx = nx;
               amc_vb.data[j].ny = ny;
               amc_vb.data[j].nz = nz;
               amc_vb.data[j].wv[0] = w1;
               amc_vb.data[j].wv[1] = w2;
               amc_vb.data[j].wv[2] = w3;
               amc_vb.data[j].wv[3] = w4;
               amc_vb.data[j].wv[4] = 0.0f;
               amc_vb.data[j].wv[5] = 0.0f;
               amc_vb.data[j].wv[6] = 0.0f;
               amc_vb.data[j].wv[7] = 0.0f;
               amc_vb.data[j].wi[0] = (b1 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b1);
               amc_vb.data[j].wi[1] = (b2 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b2);
               amc_vb.data[j].wi[2] = (b3 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b3);
               amc_vb.data[j].wi[3] = (b4 == 0xFF ? AMC_INVALID_VERTEX_WMAP_INDEX : b4);
               amc_vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               amc_vb.data[j].uv[0][0] = tu;
               amc_vb.data[j].uv[0][1] = tv;
              }
            else
               return error("Incorrect 0x2C vertex type.");
           }
         else {
            stringstream ss;
            ss << "Unimplemented vertex type 0x" << hex << vertex_bytes << dec << ".";
            return error(ss.str().c_str());
           }
        }

     // insert vertex buffer
     amc.vblist.push_back(amc_vb);
     if(debug) dfile << endl;
    }

 // save AMC
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);

 return true;
}

bool extractor::processLMSH(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("CLMESH_PC: Failed to open file.");

 // create debug file
 std::ofstream dfile;
 if(debug) {
    stringstream ss;
    ss << GetPathnameFromFilename(filename);
    ss << GetShortFilenameWithoutExtension(filename);
    ss << ".txt"; 
    dfile.open(ss.str().c_str(), ios::binary);
    if(!dfile) return error("CLMESH_PC: Failed to create debug file.");
   }

 // read header
 uint32 h01 = LE_read_uint32(ifile);
 uint32 h02 = LE_read_uint32(ifile); // string table bytes
 uint32 h03 = LE_read_uint32(ifile);
 uint32 h04 = LE_read_uint32(ifile); // number of strings
 if(h01 != 0x00043854) return error("CLMESH_PC: Invalid file.");

 // read string table
 if(debug) dfile << "------------" << endl;
 if(debug) dfile << "STRING TABLE" << endl;
 if(debug) dfile << "------------" << endl;
 if(debug) dfile << endl;

 // move to 0x20
 ifile.seekg(0x20);
 if(ifile.fail()) return error("CLMESH_PC: Seek failure.");

 // read string table
 deque<string> stringTable;
 if(h02 && h04) {
    for(uint32 i = 0; i < h04; i++) {
        char buffer[1024];
        if(!read_string(ifile, buffer, 1024)) return error("CLMESH_PC: Read string failure.");
        stringTable.push_back(string(buffer));
        if(debug) dfile << " " << buffer << endl;
       }
     // read an extra byte
     // when table is 0x10-byte aligned, we need to read another row of zeroes
     char c;
     ifile.read(&c, 1);
    }
 // skip string table
 else if(!h02 && !h04) {
     char buffer[1024];
     ifile.read(&buffer[0], 0x10);
     if(ifile.fail()) return error("CLMESH_PC: Read failure.");
    }
 else return error("CLMESH_PC: Invalid string table.");

 // move to next 0x10 aligned position
 uint32 position = (uint32)ifile.tellg();
 position = ((position + 0x0F) & (~0x0F));
 ifile.seekg(position);
 if(ifile.fail()) return error("CLMESH_PC: Seek failure.");

 // construct GLMESH_PC filename
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);
 string meshname = pathname;
 meshname += shrtname;
 meshname += ".glmesh_pc";

 // open GLMESH_PC file
 ifstream mfile(meshname.c_str(), ios::binary);
 if(!mfile) return error("GLMESH_PC: Failed to open file.");

 // read GLMESH_PC header
 uint32 GL_h01 = LE_read_uint32(mfile);
 uint32 GL_h02 = LE_read_uint32(mfile);
 uint32 GL_h03 = LE_read_uint32(mfile);
 uint32 GL_h04 = LE_read_uint32(mfile);

 // search for model ID
 for(;;) {
     uint32 modelID = LE_read_uint32(ifile);
     if(ifile.fail()) return error("CLMESH_PC: Failed to find model identifier.");
     if(modelID == GL_h01) break;
    }

 // save position of 0x00000009
 uint32 saved_offset = (uint32)ifile.tellg() - 0x08;

 // read model information
 if(debug) dfile << "-----------------" << endl;
 if(debug) dfile << "MODEL INFORMATION" << endl;
 if(debug) dfile << "-----------------" << endl;
 if(debug) dfile << endl;

 uint32 MID_h01 = LE_read_uint32(ifile); // daedalus: 0x0254
 uint32 MID_h02 = LE_read_uint32(ifile); // daedalus: 0x065374
 uint32 MID_h03 = LE_read_uint32(ifile); // 0x01
 uint32 MID_h04 = LE_read_uint32(ifile); // 0x01 (number of 0x30 byte data sections)
 uint32 MID_h05 = LE_read_uint32(ifile); // 0xB8 (offset from 0x09 to 0x30 byte data section)
 uint32 MID_h06 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h07 = LE_read_uint32(ifile); // 0x02 (number of vertex buffers)
 uint32 MID_h08 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h09 = LE_read_uint32(ifile); // 0x80
 uint32 MID_h10 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h11 = LE_read_uint32(ifile); // 0x7378 (number of indices)
 uint32 MID_h12 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h13 = LE_read_uint32(ifile); // 0x10 (offset to index data)
 uint32 MID_h14 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h15 = LE_read_uint16(ifile); // 0x02 (bytes per index)
 uint32 MID_h16 = LE_read_uint16(ifile); // 0x12 (number of surfaces)
 uint32 MID_h17 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h18 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h19 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h20 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h21 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h22 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h23 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h24 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h25 = LE_read_uint32(ifile); // 0x00
 real32 MID_h26 = LE_read_real32(ifile); // 1.0f
 real32 MID_h27 = LE_read_real32(ifile); // 1.0f
 real32 MID_h28 = LE_read_real32(ifile); // 1.0f
 real32 MID_h29 = LE_read_real32(ifile); // 0.0f
 uint32 MID_h30 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h31 = LE_read_uint32(ifile); // 0x00

 // daedalus: vertex buffer information
 // C3430000 14 00 01 00 00000000 00000000 00E70000 00000000
 // 62010000 18 02 01 00 00000000 00000000 40320600 00000000

 // read vertex buffer information
 struct VERTEX_BUFFER_INFO {
  uint32 n_vertices;
  uint32 flags;
  uint32 unk01;
  uint32 unk02;
  uint32 start;
  uint32 unk03;
 };

 deque<VERTEX_BUFFER_INFO> vbinfo;
 for(uint32 i = 0; i < MID_h07; i++)
    {
     VERTEX_BUFFER_INFO info;
     info.n_vertices = LE_read_uint32(ifile);
     info.flags = LE_read_uint32(ifile);
     info.unk01 = LE_read_uint32(ifile);
     info.unk02 = LE_read_uint32(ifile);
     info.start = LE_read_uint32(ifile);
     info.unk03 = LE_read_uint32(ifile);
     vbinfo.push_back(info);
    }

 // read 0x30-byte data sections
 ifile.seekg(saved_offset + MID_h05);
 if(ifile.fail()) return error("Stream seek failure.");
 for(uint16 i = 0; i < MID_h04; i++) {
     char temp[0x30];
     LE_read_array(ifile, &temp[0], 0x30);
    }

 // model container
 ADVANCEDMODELCONTAINER amc;

 // surface information
 typedef map<uint16, deque<AMC_REFERENCE>>::value_type smvt;
 map<uint16, deque<AMC_REFERENCE>> surface_map;

 // read surface information
 for(uint16 i = 0; i < MID_h16; i++)
    {
     uint16 s01 = LE_read_uint16(ifile); // id (incremeting index)
     uint16 s02 = LE_read_uint16(ifile); // vertex buffer index
     uint32 s03 = LE_read_uint32(ifile); // index buffer start
     uint32 s04 = LE_read_uint32(ifile); // number of indices
     uint32 s05 = LE_read_uint32(ifile); // vertex buffer start
     uint32 s06 = LE_read_uint32(ifile); // number of vertices
     //cout << " Surface ID: 0x" << hex << s01 << dec << endl;
     //cout << " Reference to Vertex Buffer: 0x" << hex << s02 << dec << endl;
     //cout << " Index Buffer Start: 0x" << hex << s03 << dec << endl;
     //cout << " Number of Indices: 0x" << hex << s04 << dec << endl;
     //cout << " Vertex Buffer Start: 0x" << hex << s05 << dec << endl;
     //cout << " Number of Vertices: 0x" << hex << s06 << dec << endl;
     //cout << endl;

     // find surface identifier
     auto iter = surface_map.find(i);
     if(iter == surface_map.end()) iter = surface_map.insert(smvt(i, deque<AMC_REFERENCE>())).first;

      // create reference
      AMC_REFERENCE ref;
      ref.vb_index = s02; // vbuffer index
      ref.vb_start = 0;   // vbuffer start index
      ref.vb_width = s06; // number of vertices
      ref.ib_index = 0;   // ibuffer index (only one index buffer)
      ref.ib_start = s03; // ibuffer start index
      ref.ib_width = s04; // number of indices
      ref.jm_index = AMC_INVALID_JMAP_INDEX;

     // insert reference
     iter->second.push_back(ref);
    }

 // process surface map
 deque<AMC_SURFACE> surfaces;
 for(auto iter = surface_map.begin(); iter != surface_map.end(); iter++)
    {
     // create surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << iter->first;

     // create surface
     AMC_SURFACE surface;
     surface.name = ss.str();
     surface.refs = iter->second;
     surface.surfmat = AMC_INVALID_SURFMAT; // no texture yet

     // save surface
     amc.surfaces.push_back(surface);
    }

 // move to index buffer
 mfile.seekg(MID_h13);
 if(mfile.fail()) return error("Index buffer seek failure.");

 // index buffer properties
 uint32 n_indices = MID_h11;
 uint32 bytes_per_index = MID_h15;
 if(bytes_per_index != 0x02) return error("Expecting 0x02 byte indices.");

 // read index buffer
 uint32 ibsize = n_indices*bytes_per_index;
 boost::shared_array<char> ibdata(new char[ibsize]);
 mfile.read(ibdata.get(), ibsize);

 // save index buffer
 AMC_IDXBUFFER amc_ib;
 amc_ib.format = AMC_TRISTRIP;
 amc_ib.type = AMC_UINT16;
 amc_ib.wind = AMC_CW;
 amc_ib.name = "default";
 amc_ib.elem = n_indices;
 amc_ib.data = ibdata;
 amc.iblist.push_back(amc_ib);

 // for each vertex buffer
 for(uint32 i = 0; i < MID_h07; i++)
    {
     // move to vertex buffer
     VERTEX_BUFFER_INFO vbi = vbinfo[i];
     mfile.seekg(vbi.start);
     if(mfile.fail()) return error("Vertex buffer seek failure.");
     cout << "Vertex Buffer #" << i << endl;

     // vertex buffer properties
     uint32 n_vertices = vbi.n_vertices;
     uint32 vertex_size = (vbi.flags & 0xFF);
     uint32 vertex_type = ((vbi.flags & 0xFF00) >> 8);
     cout << " vertices = 0x" << hex << n_vertices << dec << endl;
     cout << " vertex size = 0x" << hex << vertex_size << dec << endl;
     cout << " vertex type = 0x" << hex << vertex_type << dec << endl;

     // read vertex buffer
     uint32 vbsize = n_vertices*vertex_size;
     boost::shared_array<char> vbdata(new char[vbsize]);
     mfile.read(vbdata.get(), vbsize);
     if(mfile.fail()) return error("Vertex buffer read failure.");

     // initialize vertex buffer
     AMC_VTXBUFFER amc_vb;
     amc_vb.elem = n_vertices;
     amc_vb.data.reset(new AMC_VERTEX[amc_vb.elem]);
     amc_vb.flags = 0;
     amc_vb.flags |= AMC_VERTEX_POSITION;
     amc_vb.flags |= AMC_VERTEX_NORMAL;
     amc_vb.flags |= AMC_VERTEX_UV;
     amc_vb.name = "default";
     amc_vb.uvchan = 1;
     amc_vb.uvtype[0] = AMC_DIFFUSE_MAP;
     amc_vb.uvtype[1] = AMC_INVALID_MAP;
     amc_vb.uvtype[2] = AMC_INVALID_MAP;
     amc_vb.uvtype[3] = AMC_INVALID_MAP;
     amc_vb.uvtype[4] = AMC_INVALID_MAP;
     amc_vb.uvtype[5] = AMC_INVALID_MAP;
     amc_vb.uvtype[6] = AMC_INVALID_MAP;
     amc_vb.uvtype[7] = AMC_INVALID_MAP;
     amc_vb.colors = 0;

     //  {
     //   binary_stream bs(vbdata, vbsize);
     //   for(uint32 j = 0; j < (16 < n_vertices ? n_vertices : n_vertices); j++) {
     //       cout << " " << hex << setfill('0') << setw(4) << (uint32)j << dec << ": ";
     //       for(uint32 j = 0; j < vertex_size; j++) cout << hex << setfill('0') << setw(2) << (uint32)bs.BE_read_uint08() << dec;
     //       cout << endl;
     //      }
     //   cout << endl;
     //  }
     //
     // process vertices
     binary_stream bs(vbdata, vbsize);
     for(uint32 j = 0; j < n_vertices; j++)
        {
         if(vertex_size == 0x14) {
            if(vertex_type == 0x00) {
               amc_vb.data[j].vx = bs.LE_read_real32();
               amc_vb.data[j].vy = bs.LE_read_real32();
               amc_vb.data[j].vz = bs.LE_read_real32();
               amc_vb.data[j].nx = (bs.LE_read_uint08() - 127.0f)/128.0f;
               amc_vb.data[j].ny = (bs.LE_read_uint08() - 127.0f)/128.0f;
               amc_vb.data[j].nz = (bs.LE_read_uint08() - 127.0f)/128.0f;
               amc_vb.data[j].nw = (bs.LE_read_uint08() - 127.0f)/128.0f;
               amc_vb.data[j].uv[0][0] = bs.LE_read_sint16()/1023.0f;
               amc_vb.data[j].uv[0][1] = bs.LE_read_sint16()/1023.0f;
              }
           }
         else if(vertex_size == 0x18) {
            if(vertex_type == 0x02) {
               amc_vb.data[j].vx = bs.LE_read_real32();
               amc_vb.data[j].vy = bs.LE_read_real32();
               amc_vb.data[j].vz = bs.LE_read_real32();
               amc_vb.data[j].nx = (bs.LE_read_uint08() - 127.0f)/128.0f;
               amc_vb.data[j].ny = (bs.LE_read_uint08() - 127.0f)/128.0f;
               amc_vb.data[j].nz = (bs.LE_read_uint08() - 127.0f)/128.0f;
               amc_vb.data[j].nw = (bs.LE_read_uint08() - 127.0f)/128.0f;
               bs.LE_read_uint08();
               bs.LE_read_uint08();
               bs.LE_read_uint08();
               bs.LE_read_uint08();
               amc_vb.data[j].uv[0][0] = bs.LE_read_sint16()/1023.0f;
               amc_vb.data[j].uv[0][1] = bs.LE_read_sint16()/1023.0f;
              }
            else {
               stringstream ss;
               ss << "Unknown vertex type 0x";
               ss << hex << vertex_size << ": 0x";
               ss << hex << vertex_type << ".";
               return error(ss);
              }
           }
         else {
            stringstream ss;
            ss << "Unimplemented vertex size 0x" << hex << vertex_size << dec << ".";
            return error(ss);
           }
        }

     // insert vertex buffer
     amc.vblist.push_back(amc_vb);
     if(debug) dfile << endl;
    }

 // save AMC
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);

 return true;
}

bool extractor::processRIGS(const std::string& filename)
{
 // open rig file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("RIG_PC: Failed to open rig file.");

 // save parameters
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // model container
 ADVANCEDMODELCONTAINER amc;

 // read rig header
 ifile.seekg(0x24);
 uint32 rig_h01 = LE_read_uint32(ifile);
 uint32 rig_h02 = LE_read_uint32(ifile);
 uint32 rig_h03 = LE_read_uint32(ifile);
 uint32 rig_h04 = LE_read_uint32(ifile);

 // rig structure #1
 struct SRTSKELINFO1 {
  uint32 p01;
 };

 // rig structure #2
 struct SRTSKELINFO2 {
  uint32 p01; // identifier
  uint32 p02; // 0x00000000
  real32 p03; // x
  real32 p04; // y
  real32 p05; // z
  real32 p06; // ???
  real32 p07; // ???
  real32 p08; // ???
  uint32 p09; // parent
  uint32 p10; // 0xFFFFFFFF
 };

 // rig structure #3
 struct SRTSKELINFO3 {
  uint32 p01;    // identifier
  uint32 p02;    // 0x00000000
  real32 p03[9]; // 3x3 matrix
  real32 p04[3]; // offset
  uint32 p05;    // parent
  uint32 p06;    // 0xFFFFFFFF
 };

 // rig data
 deque<SRTSKELINFO1> ss1;
 deque<SRTSKELINFO2> ss2;
 deque<SRTSKELINFO3> ss3;
 deque<string> ss4;
    
 // move to rig section #1
 ifile.seekg(0x50);
 if(ifile.fail()) return error("RIG_PC: Seek failure.");
    
 // read rig section #1
 for(uint32 i = 0; i < rig_h01; i++) {
     SRTSKELINFO1 info;
     info.p01 = LE_read_uint32(ifile);
     ss1.push_back(info);
    }

 // output filename
 // stringstream ss;
 // ss << pathname << shrtname << ".obj";
 // ofstream ofile(ss.str().c_str());
 // ofile << "o " << shrtname << endl;

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.tiplen = 0.25f;
 skel.name = "skeleton";

 // move to rig section #2
 uint32 position = (uint32)ifile.tellg();
 position = ((position + 0x07) & ~(0x07));
 ifile.seekg(position);

 // read rig section #2
 for(uint32 i = 0; i < rig_h01; i++)
    {
     // read and save info
     SRTSKELINFO2 info;
     info.p01 = LE_read_uint32(ifile); // ???
     info.p02 = LE_read_uint32(ifile); // 0x00000000
     info.p03 = LE_read_real32(ifile); // x (offset from parent)
     info.p04 = LE_read_real32(ifile); // y (offset from parent)
     info.p05 = LE_read_real32(ifile); // z (offset from parent)
     info.p06 = LE_read_real32(ifile); // ???
     info.p07 = LE_read_real32(ifile); // ???
     info.p08 = LE_read_real32(ifile); // ???
     info.p09 = LE_read_uint32(ifile); // parent
     info.p10 = LE_read_uint32(ifile); // 0xFFFFFFFF
     ss2.push_back(info);

     // save bone
     // real32 x2 = info.p03 + info.p06;
     // real32 y2 = info.p04 + info.p07;
     // real32 z2 = info.p05 + info.p08;
     // ofile << "#" << i << endl;
     // ofile << "v " << info.p03 << " " << info.p04 << " " << info.p05 << endl;
     // ofile << "v " << x2 << " " << y2 << " " << z2 << endl;
     // ofile << "f " << (1 + 2*i) << " " << (2 + 2*i) << endl;
     // ofile << endl;

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;

     // create and insert joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = (info.p09 == 0xFFFFFFFF ? AMC_INVALID_JOINT : info.p09);
     joint.m_rel[0x0] = 1.0;
     joint.m_rel[0x1] = 0.0;
     joint.m_rel[0x2] = 0.0;
     joint.m_rel[0x3] = 0.0;
     joint.m_rel[0x4] = 0.0;
     joint.m_rel[0x5] = 1.0;
     joint.m_rel[0x6] = 0.0;
     joint.m_rel[0x7] = 0.0;
     joint.m_rel[0x8] = 0.0;
     joint.m_rel[0x9] = 0.0;
     joint.m_rel[0xA] = 1.0;
     joint.m_rel[0xB] = 0.0;
     joint.m_rel[0xC] = 0.0;
     joint.m_rel[0xD] = 0.0;
     joint.m_rel[0xE] = 0.0;
     joint.m_rel[0xF] = 1.0;
     joint.p_rel[0] = -info.p03;
     joint.p_rel[1] = -info.p04;
     joint.p_rel[2] = -info.p05;
     joint.p_rel[3] = 1.0f;
     skel.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel);

 // move to rig section #3
 position = (uint32)ifile.tellg();
 position = ((position + 0x07) & ~(0x07));
 ifile.seekg(position);

 // read rig section #3
 for(uint32 i = 0; i < rig_h04; i++)
    {
     // save item
     SRTSKELINFO3 info;
     info.p01 = LE_read_uint32(ifile);
     info.p02 = LE_read_uint32(ifile);
     info.p03[0x0] = LE_read_real32(ifile);
     info.p03[0x1] = LE_read_real32(ifile);
     info.p03[0x2] = LE_read_real32(ifile);
     info.p03[0x3] = LE_read_real32(ifile);
     info.p03[0x4] = LE_read_real32(ifile);
     info.p03[0x5] = LE_read_real32(ifile);
     info.p03[0x6] = LE_read_real32(ifile);
     info.p03[0x7] = LE_read_real32(ifile);
     info.p03[0x8] = LE_read_real32(ifile);
     info.p04[0x0] = LE_read_real32(ifile);
     info.p04[0x1] = LE_read_real32(ifile);
     info.p04[0x2] = LE_read_real32(ifile);
     info.p05 = LE_read_uint32(ifile);
     info.p06 = LE_read_uint32(ifile);
     ss3.push_back(info);
    }

 // move to rig section #4
 position = (uint32)ifile.tellg();
 position = ((position + 0x07) & ~(0x07));
 ifile.seekg(position);

 // read rig section #4
 uint32 n_strings = 0;
 for(;;) {
     char buffer[1024];
     memset(buffer, 0, 1024);
     read_string(ifile, buffer, 1024);
     if(ifile.eof()) break;
     else if(ifile.fail()) break;
     if(strlen(buffer)) ss4.push_back(buffer);
    }

 // save AMC file
 string amcname = shrtname;
 amcname += "_rig";
 SaveAMC(pathname.c_str(), amcname.c_str(), amc);
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