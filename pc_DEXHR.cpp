#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_zlib.h"
#include "x_stream.h"
#include "x_con.h"
#include "x_dds.h"
#include "x_amc.h"
#include "pc_DEXHR.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   DEXHR

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool process000(const string& filename, bool delete_useless)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // read virtual bigfile size
 uint32 bigsize = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(bigsize != 0x7FF00000) return error("Invalid virtual bigfile size.");

 // move to 0x44
 ifile.seekg(0x44);
 if(ifile.fail()) return error("Seek failure.");

 // read number of files
 uint32 n_files = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 cout << " n_files = " << n_files << endl;

 // read array of uint32
 boost::shared_array<uint32> head1(new uint32[n_files]);
 if(!LE_read_array(ifile, head1.get(), n_files)) return error("Read array failure.");

 // read array of uint32[4]
 struct DEXHRENTRY {
  uint32 p01; // filesize
  uint32 p02; // offset
  uint32 p03; // type
  uint32 p04; // 0x00000000
 };
 boost::shared_array<DEXHRENTRY> head2(new DEXHRENTRY[n_files]);
 for(uint32 i = 0; i < n_files; i++) {
     head2[i].p01 = LE_read_uint32(ifile); // filesize
     head2[i].p02 = LE_read_uint32(ifile); // offset
     head2[i].p03 = LE_read_uint32(ifile); // type
     head2[i].p04 = LE_read_uint32(ifile); // 0x00
     if(ifile.fail()) return error("Read failure.");
    }

 // 000 file no longer needed
 ifile.close();

 // compute maximum file offset
 uint64 offset_scale = static_cast<uint64>(0x800);
 uint64 max_offset = static_cast<uint64>(head2[0].p02) * offset_scale;
 for(uint32 i = 0; i < n_files; i++) {
     uint64 offset = static_cast<uint64>(head2[i].p02) * offset_scale;
     if(max_offset < offset) max_offset = offset;
    }

 // compute number of bigfile parts
 uint32 n_bigparts = static_cast<uint32>(max_offset/static_cast<uint64>(bigsize) + 1);
 if(!n_bigparts) return error("Unexpected number of bigfile parts.");

 // create save path
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // create ifstream array
 boost::shared_array<ifstream> files(new ifstream[n_bigparts]);
 for(uint32 i = 0; i < n_bigparts; i++) {
     stringstream ss;
     ss << pathname;
     ss << shrtname;
     ss << ".";
     ss << setfill('0') << setw(3) << i;
     files[i].open(ss.str().c_str(), ios::binary);
     if(!files[i]) return error("Failed to open bigfile part.");
    }

 // for each entry
 for(uint32 i = 0; i < n_files; i++)
    {
     // status message
     cout << " Saving file #" << (i + 1) << " of " << n_files << "." << endl;

     // compute offset to data
     uint64 offset_scale = static_cast<uint64>(0x800);
     uint64 offset = static_cast<uint64>(head2[i].p02) * offset_scale;

     // compute file index
     uint64 virtual_size = static_cast<uint64>(bigsize);
     uint32 bigfile_index = static_cast<uint32>(offset/virtual_size);

     // move to file data
     uint32 bigfile_offset = static_cast<uint32>(offset % virtual_size);
     files[bigfile_index].seekg(bigfile_offset);
     if(files[bigfile_index].fail()) return error("Seek failure.");

     // mask highbyte to obtain data size
     uint32 datasize = head2[i].p01;
     if(!datasize) continue;

     // allocate and read data
     boost::shared_array<char> data(new char[datasize]);
     files[bigfile_index].read(data.get(), datasize);
     if(files[bigfile_index].fail()) return error("Read failure.");

     // create filename
     stringstream ss;
     ss << pathname;
     ss << shrtname;
     ss << "\\";
     ss << setfill('0') << setw(4) << hex << i << dec;
     ss << ".dex";

     // create and save file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");
     ofile.write(data.get(), datasize);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
}

inline uint32 get_filesize(ifstream& ifile)
{
 ifstream::pos_type pos = ifile.tellg();
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(pos);
 return filesize;
}

inline void file_align_16(ifstream& ifile)
{
 uint32 pos = static_cast<uint32>(ifile.tellg());
 if(ifile.fail()) return;
 pos = ((pos + 0x0F) & (~0x0F));
 ifile.seekg(pos);
}

bool processDEX(const string& filename, bool delete_useless)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // read first four bytes
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // test magic number CDRM
 // if not a CDRM, just ignore and return true
 if(magic != 0x4D524443) return true;

 // read unknown
 uint32 unk01 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of files
 uint32 n_files = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(!n_files) return true; // ok if there is nothing to process

 // read unknown
 uint32 unk02 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // CDRM entries
 struct DEXHRCDRMENTRY {
  uint32 p01;
  uint32 p02;
 };
 deque<DEXHRCDRMENTRY> entries;

 // read CDRM entries
 for(uint32 i = 0; i < n_files; i++) {
     DEXHRCDRMENTRY item;
     item.p01 = LE_read_uint32(ifile);
     item.p02 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     entries.push_back(item);
    }

 // create save path
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // for each CDRM entry
 boost::shared_array<string> drmlist(new string[n_files]);
 for(uint32 i = 0; i < n_files; i++)
    {
     // seek next 0x10-aligned position
     file_align_16(ifile);
     if(ifile.fail()) return error("Seek failure.");
     uint32 position = (uint32)ifile.tellg();

     // compression byte-uncompressed size + compressed size
     uint32 p01 = entries[i].p01;
     uint32 p02 = entries[i].p02;

     // uncompressed and compresed size
     uint32 inflate_size = (p01 >> 8);
     uint32 deflate_size = (p02);

     // read deflate_data
     boost::shared_array<char> data(new char[deflate_size]);
     ifile.read(data.get(), deflate_size);
     if(ifile.fail()) {
        stringstream es;
        es << "Read failure for item " << i << " at offset 0x" << hex << position << dec << ".";
        return error(es);
       }

     // create filename
     stringstream ss;
     ss << savepath;
     ss << setfill('0') << setw(4) << i;
     ss << ".drm";

     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create file.");

     // uncompressed data
     uint32 compression = (p01 & 0xFF);
     if(compression == 0x01) {
        ofile.write(data.get(), deflate_size);
        if(ofile.fail()) return error("Write failure.");
       }
     // compressed data
     else if(compression == 0x02) {
        if(!InflateZLIB(data, deflate_size, ofile))
           return false;
       }
     // unknown compression
     else
        return error("Unknown CDRM compression.");

     // save DRM filename
     drmlist[i] = ss.str();
    }

 // open first DRM file
 ifstream drm0(drmlist[0].c_str(), ios::binary);
 if(!drm0) return error("Failed to open first 0000.DRM file.");

 // read 0x15
 uint32 drm0_h01 = LE_read_uint32(drm0); // 0x15
 if(drm0.fail()) return error("Read failure.");
 if(drm0_h01 != 0x15) return error("0000.DRM must start with 0x15.");

 // read unknowns
 uint32 drm0_h02 = LE_read_uint32(drm0);
 uint32 drm0_h03 = LE_read_uint32(drm0);
 uint32 drm0_h04 = LE_read_uint32(drm0);
 uint32 drm0_h05 = LE_read_uint32(drm0);
 uint32 drm0_h06 = LE_read_uint32(drm0);
 if(drm0.fail()) return error("Read failure.");

 // number of overall files
 uint32 drm0_h07 = LE_read_uint32(drm0);
 if(drm0.fail()) return error("Read failure.");
 if(drm0_h07 > n_files) return error("Invalid number of files.");
 if(drm0_h07 == 0) return true; // nothing to do

 // read unknown
 uint32 drm0_h08 = LE_read_uint32(drm0);
 if(drm0.fail()) return error("Read failure.");

 // read file information
 struct DEXHRDRMFILEINFO {
  uint32 p01; // main file size
  uint32 p02; // XX-YYYYYY where XX is file type and YYYYYY is unknown
  uint32 p03; // XX-YYYYYY where XX is file subtype and YYYYYY is head file size
  uint32 p04; // file_id
  uint32 p05; // type_id
 };
 deque<DEXHRDRMFILEINFO> fileinfo;
 for(uint32 i = 0; i < drm0_h07; i++) {
     DEXHRDRMFILEINFO info;
     info.p01 = LE_read_uint32(drm0);
     info.p02 = LE_read_uint32(drm0);
     info.p03 = LE_read_uint32(drm0);
     info.p04 = LE_read_uint32(drm0);
     info.p05 = LE_read_uint32(drm0);
     fileinfo.push_back(info);
    }

 // combine DRM files
 uint32 drm_index = 1;
 for(uint32 i = 0; i < drm0_h07; i++)
    {
     // create a file name for this entry
     stringstream ss;
     ss << savepath;
     ss << setfill('0') << setw(4) << i;
     ss << ".";

     // create an extension for this entry
     bool is_useful = true;
     uint32 ft1 = (fileinfo[i].p02 & 0xFF);
     uint32 ft2 = (fileinfo[i].p03 & 0xFF);

     // texture
     if(ft1 == 0x05 && ft2 == 0x0A) {
        ss.str("");
        ss << savepath;
        ss << setfill('0') << setw(8) << hex << fileinfo[i].p04;
        ss << ".";
        if(fileinfo[i].p05 == 0xFFFFFFFF) ss << "pcd";
        else return error("Unknown texture type.");
       }
     // sound
     else if(ft1 == 0x06 && ft2 == 0x00) {
        ss << "fsb";
        is_useful = false;
       }
      // material
     else if(ft1 == 0x0A && ft2 == 0x24) {
        ss.str("");
        ss << savepath;
        ss << setfill('0') << setw(8) << hex << fileinfo[i].p04;
        ss << ".";
        if(fileinfo[i].p05 == 0xBFFFFFFF) ss << "mtl_a";
        else if(fileinfo[i].p05 == 0x7FFFFFFF) ss << "mtl_b";
        else return error("Unknown material type.");
       }
     // scene
     else if(ft1 == 0x0C && ft2 == 0x30) {
        ss.str("");
        ss << savepath;
        ss << setfill('0') << setw(8) << hex << fileinfo[i].p04;
        ss << ".";
        ss << "scene";
       }
     // model
     else if(ft1 == 0x0C && ft2 == 0x34) {
        ss.str("");
        ss << savepath;
        ss << setfill('0') << setw(8) << hex << fileinfo[i].p04;
        ss << ".";
        ss << "model";
       }
     else {
        is_useful = false;
        ss << setfill('0') << setw(2) << hex << ft1 << dec << "_";
        ss << setfill('0') << setw(2) << hex << ft2 << dec;
       }

     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create file.");

     // copy DRM header
     uint32 headsize = (fileinfo[i].p03 >> 0x08);
     if(headsize)
       {
        // open current DRM file
        stringstream drmname;
        drmname << savepath;
        drmname << setfill('0') << setw(4) << drm_index;
        drmname << ".";
        drmname << "drm";
        ifstream headfile(drmname.str().c_str(), ios::binary);
        if(!headfile) return error("Failed to open DRM file.");

        // compare expected filesizes
        uint32 size = get_filesize(headfile);
        if(headsize != size) {
           stringstream ss;
           ss << "Unexpected DRM (header) filesize for entry 0x";
           ss << hex << i << dec << ".";
           return error(ss);
          }

        // read data from DRM file
        boost::shared_array<char> src(new char[headsize]);
        headfile.read(src.get(), headsize);
        if(headfile.fail()) return error("Read failure.");

        // save DRM file to output file
        ofile.write(src.get(), headsize);
        if(ofile.fail()) return error("Write failure.");

        // increment drm_index
        drm_index++;

        // close and delete DRM file
        headfile.close();
        DeleteFileA(drmname.str().c_str());
       }

     // copy one or more DRM files to output file
     uint32 current_filesize = 0;
     const uint32 overall_filesize = fileinfo[i].p01;
     while(current_filesize < overall_filesize)
          {
           // open current DRM file
           stringstream drmname;
           drmname << savepath;
           drmname << setfill('0') << setw(4) << drm_index;
           drmname << ".";
           drmname << "drm";
           ifstream drmfile(drmname.str().c_str(), ios::binary);
           if(!drmfile) return error("Failed to open DRM file.");

           // read data from DRM file
           uint32 drmsize = get_filesize(drmfile);
           boost::shared_array<char> src(new char[drmsize]);
           drmfile.read(src.get(), drmsize);
           if(drmfile.fail()) return error("Read failure.");

           // save DRM file to output file
           ofile.write(src.get(), drmsize);
           if(ofile.fail()) return error("Write failure.");

           // increment drm_index
           drm_index++;

           // sum total current filesize
           current_filesize += drmsize;

           // close and delete DRM file
           drmfile.close();
           DeleteFileA(drmname.str().c_str());
          }

     // when done, current_filesize should equal overall_filesize
     if(current_filesize != overall_filesize) {
        stringstream ss;
        ss << "Unexpected DRM (body) filesize for entry 0x";
        ss << hex << i << dec << ": ";
        ss << "0x" << hex << current_filesize << dec;
        ss << " versus ";
        ss << "0x" << hex << overall_filesize << dec << ".";
        return error(ss);
       }

     // delete shit files
     if(delete_useless && !is_useful) {
        ofile.close();
        DeleteFileA(ss.str().c_str());
       }
    }

 // close and delete first DRM file
 if(delete_useless) {
    drm0.close();
    DeleteFileA(drmlist[0].c_str());
   }

 // close and delete DEX file
 if(delete_useless) {
    ifile.close();
    DeleteFileA(filename.c_str());
   }

 // delete empty directories
 // don't worry, it won't delete non-empty directories!
 if(delete_useless)
    RemoveDirectoryA(savepath.c_str());

 return true;
}

//
// CDRM NOTES:
// First file in CDRM is a list of files.
// Header of first file is 0x20 bytes. In example 0x0D is number of files.
// After header is a list of 0x14 byte items.
// SAMPLE
// missing_link\bigfile.000\0000.dex
// 15 00 00 00 - 00 00 00 00 - 00 00 00 00 - 00 00 00 00 (HEADER)
// 00 00 00 00 - 01 00 00 00 - 0D 00 00 00 - FF FF FF FF (HEADER)
// B4 00 00 00 - 09 00 2B 10 - 00 00 00 00 - 99 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// 8C 14 00 00 - 09 00 AD 52 - 00 00 00 00 - 9A 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// D0 01 00 00 - 09 00 2B 10 - 00 00 00 00 - A1 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// 34 07 00 00 - 09 00 5D 49 - 00 00 00 00 - A2 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// 4C 01 00 00 - 09 00 18 10 - 00 00 00 00 - A3 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// 04 11 00 00 - 09 00 E8 46 - 00 00 00 00 - A4 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// 8C 14 00 00 - 09 00 BE 44 - 00 00 00 00 - 98 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// A0 02 00 00 - 09 00 05 10 - 00 00 00 00 - 9D 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// A0 0A 00 00 - 09 00 0F 3B - 00 00 00 00 - 9E 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// 40 01 00 00 - 09 00 F2 0F - 00 00 00 00 - 9B 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// 24 16 00 00 - 09 00 86 38 - 00 00 00 00 - 9C 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// B4 00 00 00 - 09 00 A5 0F - 00 00 00 00 - 9F 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id) 
// 8C 06 00 00 - 09 00 36 2F - 00 00 00 00 - A0 10 00 00 - FF FF FF 7F (filesize - type/??? - 0 - file_id - type_id)

// If the file type is 0x00, then the file is a continuation of the previous file!
// SAMPLE
// bigfile.000\2A18.dex
// The first new file here is 0x109A80 + 0x6970 + 0x28 bytes = 0x110418 bytes
// 80 9A 10 00 - 07 00 78 45 - 00 E8 10 01 - AC 4C 00 00 - FF FF FF FF (0001.drm - 0006.drm)
// 70 69 00 00 - 00 00 00 00 - 00 7C 0D 00 - 00 00 00 00 - FF FF FF FF (0007.drm - 0008.drm)
// 28 00 00 00 - 00 00 00 00 - 00 3C 00 00 - 00 00 00 00 - FF FF FF FF (0009.drm - 0010.drm)
// DC 0F 05 00 - 0D 00 00 00 - 00 00 00 00 - 00 00 00 00 - FF FF FF FF (0011.drm - 0012.drm) 
// F8 7E 01 00 - 0D 00 00 00 - 00 00 00 00 - 00 00 00 00 - FF FF FF FF

// 0001.drm = 0x110E8 bytes ENTRY 0x01 (p03/0x100)
// 0002.drm = 0x40000 bytes ENTRY 0x01 (p01)
// 0003.drm = 0x40000 bytes ENTRY 0x01 (p01)
// 0004.drm = 0x40000 bytes ENTRY 0x01 (p01)
// 0005.drm = 0x40000 bytes ENTRY 0x01 (p01)
// 0006.drm = 0x09A80 bytes ENTRY 0x01 (p01)
// 0007.drm = 0x00D7C bytes ENTRY 0x02 (p03/0x100)
// 0008.drm = 0x06970 bytes ENTRY 0x02 (p01)
// 0009.drm = 0x3C bytes ENTRY 0x03 (p03/0x100)
// 0010.drm = 0x28 bytes ENTRY 0x03 (p01)
// 0011.drm = 0x40000 bytes ENTRY 0x04 (p01)
// 0012.drm = 0x10FDC bytes ENTRY 0x04 (p01)
// 0013.drm = 0x17EF8 bytes ENTRY 0x05 (p01)
// 0014.drm to 0026.drm single entries
// 0027.drm 28
// 0029.drm to 0042.drm single entries
// 0043.drm 44 45 46 47
// 0048.drm 49
// 0050.drm to 0071.drm single entries
// 0072.drm 73 = first scene file


};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processPCD(const string& filename)
{
 // open CDRM file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open PCD9 file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // read magic
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x39444350) {
    stringstream ss;
    ss << pathname;
    ss << shrtname;
    ss << ".dds";
    MoveFileA(filename.c_str(), ss.str().c_str()); // temporary change extension
    error("Not a PCD9 file.");
    return true;
   }

 // read type
 uint32 type = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read datasize
 uint32 datasize = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown (number of mipmaps perhaps)
 uint32 unk01 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read width and height
 uint16 dx = LE_read_uint16(ifile);
 uint16 dy = LE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown
 uint32 unk02 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown
 uint32 unk03 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // A8R8G8B8
 DDS_HEADER ddsh;
 ZeroMemory(&ddsh, sizeof(ddsh));
 if(type == 0x00000015) {
    if(!CreateA8R8G8B8DDSHeader(dx, dy, unk01, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // DXT1
 else if(type == 0x31545844) {
    if(!::CreateDXT1Header(dx, dy, unk01, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // DXT3
 else if(type == 0x33545844) {
    if(!::CreateDXT3Header(dx, dy, unk01, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // DXT5
 else if(type == 0x35545844) {
    if(!::CreateDXT5Header(dx, dy, unk01, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 else
    return error("Unknown texture format.");

 // read data
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 if(ifile.fail()) return error("Read failure.");

 // close input file so we can delete
 ifile.close();
 DeleteFileA(filename.c_str());

 // create output file
 stringstream ss;
 ss << pathname;
 ss << shrtname;
 ss << ".dds";
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create DDS file.");

 // write DDS header
 ofile.write("DDS ", 4);
 ofile.write((char*)&ddsh, sizeof(ddsh));

 // write DDS data
 ofile.write(data.get(), datasize);
 ofile.close();

 return true;
}

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

static ofstream hahaha("c:\\users\\semory\\desktop\\hahaha2.txt");
static map_deque<uint32, string> fmap;
static deque<pair<uint32, string>> bonelist;

bool processMOD(const string& filename)
{
 // open model file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open MESH file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // read header
 uint32 h01 = LE_read_uint32(ifile); // number of 0xEBBEEBBE
 uint32 h02 = LE_read_uint32(ifile); // 0x00
 uint32 h03 = LE_read_uint32(ifile); // 0x00
 uint32 h04 = LE_read_uint32(ifile); // 0x00
 uint32 h05 = LE_read_uint32(ifile); // number of materials

 // read header entries part 1
 deque<pair<uint32, uint32>> h06;
 for(uint32 i = 0; i < h01; i++) {
     uint32 p01 = LE_read_uint32(ifile);
     uint32 p02 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     h06.push_back(pair<uint32, uint32>(p01, p02));
    }

 // read header entries part 2
 deque<uint32> h07;
 for(uint32 i = 0; i < h05; i++) {
     uint32 item = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
      h07.push_back(item);
    }

 // starting offset
 uint32 start = 0x14 + (h01*0x08 + h05*0x04);

 // -----------------------------------
 // MODEL CONTAINER
 // -----------------------------------

 ADVANCEDMODELCONTAINER amc;

 // -----------------------------------
 // MATERIAL PROCESSING
 // -----------------------------------

 // if materials exist
 if(h05)
   {
    // move to 1st offset
    uint32 offset1 = h06[2].first;
    ifile.seekg(start + offset1);
    if(ifile.fail()) return error("Seek failure.");

    // read three values
    uint32 p01 = LE_read_uint32(ifile);
    uint32 p02 = LE_read_uint32(ifile);
    uint32 p03 = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // move to 2nd offset
    uint32 offset2 = h06[2].second;
    ifile.seekg(start + offset2);
    if(ifile.fail()) return error("Seek failure.");

    // read number of materials
    uint32 p04 = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // double-check number of materials
    if(p04 != h05) return error("Number of materials do not match.");

    // for each material
    for(uint32 i = 0; i < p04; i++)
       {
        // read material ID
        uint32 id = LE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");

        // create material name
        stringstream ss;
        ss << "mat_" << setfill('0') << setw(8) << hex << id << dec;

        // insert default surface
        AMC_SURFMAT mat;
        mat.name = ss.str();
        mat.twoside = 0;
        mat.unused1 = 0;
        mat.unused2 = 0;
        mat.unused3 = 0;
        mat.basemap = AMC_INVALID_TEXTURE;
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
        mat.specmapchan = 0;
        mat.tranmapchan = 0;
        mat.bumpmapchan = 0;
        mat.normmapchan = 0;
        mat.lghtmapchan = 0;
        mat.envimapchan = 0;
        mat.glssmapchan = 0;
        mat.resmapchan1 = 0;
        mat.resmapchan2 = 0;
        mat.resmapchan3 = 0;
        mat.resmapchan4 = 0;
        mat.resmapchan5 = 0;
        mat.resmapchan6 = 0;
        mat.resmapchan7 = 0;
        mat.resmapchan8 = 0;

        // create material file name
        ss.str("");
        ss << pathname;
        ss << setfill('0') << setw(8) << hex << id << dec;
        ss << ".mtl_a";

        // TODO:
        // PROCESS MATERIAL FILE

        // insert material
        amc.surfmats.push_back(mat);
       }
   }

 // -----------------------------------
 // SKELEON PROCESSING
 // -----------------------------------

 // for adam jensen h01 = 0x4E
 // his skeleton data starts at 0x3140 from the first 0xEBBEEBBE

 // if skeleton exists
 if(h01 == 0x06 || h01 == 0x4E)
   {
    // initialize skeleton
    AMC_SKELETON2 skel2;
    skel2.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
    skel2.name = "skeleton";

    // seek skeleton data
    uint32 start = 0x14 + (h01*0x08 + h05*0x04);
    uint32 position = start + h06[(h01 - 0x02)].second - 0x08;
    ifile.seekg(position);
    if(ifile.fail()) return error("Seek failure.");
    
    // read number of joints
    uint32 p01 = LE_read_uint32(ifile); // number of joints
    uint32 p02 = LE_read_uint32(ifile); // 0xEBBEEBBE
    if(ifile.fail()) return error("Read failure.");
    if(p02 != 0xBEEBBEEB) return error("Expecting 0xBEEBBEEB.");

    // if there are joints
    uint32 n_joints = p01;
    if(n_joints > 0 && n_joints < 1024)
      { 
       // for each joint
       for(uint32 i = 0; i < n_joints; i++)
          {
           // seek joint position data
           ifile.seekg(0x20, ios::cur);
           if(ifile.fail()) return error("Seek failure.");

           // read joint position
           real32 jx = LE_read_real32(ifile);
           real32 jy = LE_read_real32(ifile);
           real32 jz = LE_read_real32(ifile);
           if(ifile.fail()) return error("Read failure.");

           // seek joint parent data
           ifile.seekg(0x0C, ios::cur);
           if(ifile.fail()) return error("Seek failure.");

           // read parent data
           uint32 parent = LE_read_uint32(ifile);
           if(ifile.fail()) return error("Read failure.");

           // seek end of joint data
           ifile.seekg(0x04, ios::cur);
           if(ifile.fail()) return error("Seek failure.");

           // create joint name
           stringstream ss;
           ss << "jnt_" << setfill('0') << setw(3) << i;
           
           // create and insert joint
           AMC_JOINT joint;
           joint.name = ss.str();
           joint.id = i;
           joint.parent = (parent == 0xFFFFFFFF ? AMC_INVALID_JOINT : parent);
           joint.m_rel[0x0] = 1.0f;
           joint.m_rel[0x1] = 0.0f;
           joint.m_rel[0x2] = 0.0f;
           joint.m_rel[0x3] = 0.0f;
           joint.m_rel[0x4] = 0.0f;
           joint.m_rel[0x5] = 1.0f;
           joint.m_rel[0x6] = 0.0f;
           joint.m_rel[0x7] = 0.0f;
           joint.m_rel[0x8] = 0.0f;
           joint.m_rel[0x9] = 0.0f;
           joint.m_rel[0xA] = 1.0f;
           joint.m_rel[0xB] = 0.0f;
           joint.m_rel[0xC] = 0.0f;
           joint.m_rel[0xD] = 0.0f;
           joint.m_rel[0xE] = 0.0f;
           joint.m_rel[0xF] = 1.0f;
           joint.p_rel[0] = jx;
           joint.p_rel[1] = jy;
           joint.p_rel[2] = jz;
           joint.p_rel[3] = 1.0f;
           skel2.joints.push_back(joint);
          }

       // insert skeleton
       amc.skllist2.push_back(skel2);
      }
    else
       return error("Invalid skeleton.");
   }

 // -----------------------------------
 // MESH HEADER
 // -----------------------------------

 // move to mesh header
 uint32 position = start + h06[0x00].second;
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");
 uint32 mesh_offset = static_cast<uint32>(ifile.tellg());

 // define mesh header
 struct DEXMESHHEADER {
  uint32 p00; // Mesh
  uint32 p01; // ???
  uint32 p02; // size of Mesh data
  uint32 p03; // number of indices
  real32 p04[4]; // ???
  real32 p05[4]; // ???
  real32 p06[4]; // ???
  real32 p07[3]; // ???
  uint32 p08; // ???
  uint32 p09; // 0x00 (always)
  uint32 p10; // offset to mesh information
  uint32 p11; // offsets (p11 - 0x80 = size of index table)
  uint32 p12; // offsets (0x80)
  uint32 p13; // offset to index buffer
  uint16 p14; // number of parts
  uint16 p15; // number of items
  uint32 p16; // number of joints
  uint32 p17; // EOH = 0xFFFFFFFF
 };

 // read mesh header
 DEXMESHHEADER mh;
 mh.p00 = LE_read_uint32(ifile); // Mesh
 mh.p01 = LE_read_uint32(ifile); // ???
 mh.p02 = LE_read_uint32(ifile); // size of Mesh data
 mh.p03 = LE_read_uint32(ifile); // number of indices
 if(!mh.p03) return true; // nothing to do
 LE_read_array(ifile, &mh.p04[0], 4);
 LE_read_array(ifile, &mh.p05[0], 4);
 LE_read_array(ifile, &mh.p06[0], 4);
 LE_read_array(ifile, &mh.p07[0], 3); // 4th number is an integer?
 mh.p08 = LE_read_uint32(ifile); // ???
 mh.p09 = LE_read_uint32(ifile); // 0x00000000
 mh.p10 = LE_read_uint32(ifile); // offset to 0x40-byte item data
 mh.p11 = LE_read_uint32(ifile); // offset to 0x60-byte item data
 mh.p12 = LE_read_uint32(ifile); // size of header, offset to joint identifiers (0x80)
 mh.p13 = LE_read_uint32(ifile); // offset to index buffer
 mh.p14 = LE_read_uint16(ifile); // number of 0x40-byte items
 mh.p15 = LE_read_uint16(ifile); // number of 0x60-byte items
 mh.p16 = LE_read_uint32(ifile); // number of joints
 mh.p17 = LE_read_uint32(ifile); // 0xFFFFFFFF

 // validate header
 if(ifile.fail()) return error("Read failure.");
 if(mh.p00 != 0x6873654D) return error("Expecting Mesh.");
 if(mh.p12 != 0x80) message(" Expected 0x80.");
 if(mh.p17 != 0xFFFFFFFF) message(" Unexpected end-of-header value.");

 // DEBUG BEGIN
 if(mh.p16) bonelist.push_back(pair<uint32,string>(mh.p16, filename));
 // DEBUG END

 // -----------------------------------
 // MESH JOINT IDENTIFIERS
 // -----------------------------------

 boost::shared_array<uint32> jntids;
 uint32 n_joints = mh.p16;
 if(n_joints) {
    // move to joint data
    ifile.seekg(mesh_offset + mh.p12);
    if(ifile.fail()) return error("Seek failure.");
    // read joint data
    jntids.reset(new uint32[n_joints]);
    if(!LE_read_array(ifile, jntids.get(), n_joints))
       return error("Read failure.");
   }

 // -----------------------------------
 // MESH 0x60-BYTE BUFFER INFORMATION
 // -----------------------------------

 // buffer information must be present
 if(mh.p15 == 0) return error("Buffer information is missing.");

 // move to joint data
 ifile.seekg(mesh_offset + mh.p11);
 if(ifile.fail()) return error("Seek failure.");

 struct DEXMESHBUFFERINFO {
  real32 p01[4]; // ???
  real32 p02[4]; // ???
  uint32 p03[4]; // ???
  uint32 p04[4]; // [0] = number of mesh parts
                 // [1] = number of joint map idices
                 // [2] = offset from mesh to joint map
                 // [3] = offset from mesh to vertex buffer data
  uint32 p05[3]; // all 0x00
  uint32 p06[5]; // [0] = offset from mesh to vertex buffer info
                 // [1] = number of vertices in vertex buffer
                 // [2] = index buffer start index
                 // [3] = number of triangles
 };
 deque<DEXMESHBUFFERINFO> bufferinfo;

 uint32 n_buffers = mh.p15;
 for(uint32 i = 0; i < n_buffers; i++) {
     DEXMESHBUFFERINFO info;
     LE_read_array(ifile, &info.p01[0], 4);
     LE_read_array(ifile, &info.p02[0], 4);
     LE_read_array(ifile, &info.p03[0], 4);
     LE_read_array(ifile, &info.p04[0], 4);
     LE_read_array(ifile, &info.p05[0], 3);
     LE_read_array(ifile, &info.p06[0], 5);
     if(ifile.fail()) return error("Read failure.");
     bufferinfo.push_back(info);
    }

 // -----------------------------------
 // MESH JOINT MAPS
 // -----------------------------------

 struct DEXMESHJOINTMAP {
  uint32 elem;
  boost::shared_array<uint32> data;
 };
 deque<DEXMESHJOINTMAP> jointmaps;

 // read joint map data
 for(uint32 i = 0; i < n_buffers; i++)
    {
     DEXMESHJOINTMAP item;
     item.elem = bufferinfo[i].p04[1];
     if(item.elem) {
        // move to joint map data
        ifile.seekg(mesh_offset + bufferinfo[i].p04[2]);
        if(ifile.fail()) return error("Seek failure.");
        // read joint map data
        item.data.reset(new uint32[item.elem]);
        if(!LE_read_array(ifile, item.data.get(), item.elem))
           return error("Read failure.");
       }
     jointmaps.push_back(item);
    }

 // copy joint map data
 for(uint32 i = 0; i < n_buffers; i++) {
     if(jointmaps[i].elem) {
        AMC_JOINTMAP jm;
        for(uint32 j = 0; j < jointmaps[i].elem; j++) jm.jntmap.push_back(jointmaps[i].data[j]);
        amc.jmaplist.push_back(jm);
       }
    }

 // -----------------------------------
 // MESH VERTEX INFORMATION
 // -----------------------------------

 struct DEXMESHVBSEMANTIC {
  uint32 p01; // ???
  uint16 p02; // semantic offset
  uint16 p03; // semantic type
 };

 struct DEXMESHVBINFO {
  uint32 p01; // ???
  uint32 p02; // ???
  uint16 p03; // number of vertex semantics
  uint16 p04; // vertex size
  uint32 p05; // ???
  boost::shared_array<DEXMESHVBSEMANTIC> p06; // vertex semantics
 };
 deque<DEXMESHVBINFO> vbinfo;

 for(uint32 i = 0; i < n_buffers; i++) {
     // move to vertex info data
     ifile.seekg(mesh_offset + bufferinfo[i].p06[0]);
     if(ifile.fail()) return error("Seek failure.");
     // read vertex info
     DEXMESHVBINFO info;
     info.p01 = LE_read_uint32(ifile);
     info.p02 = LE_read_uint32(ifile);
     info.p03 = LE_read_uint16(ifile);
     info.p04 = LE_read_uint16(ifile);
     info.p05 = LE_read_uint32(ifile);
     info.p06.reset(new DEXMESHVBSEMANTIC[info.p03]);
     for(uint32 j = 0; j < info.p03; j++) {
         info.p06[j].p01 = LE_read_uint32(ifile);
         info.p06[j].p02 = LE_read_uint16(ifile);
         info.p06[j].p03 = LE_read_uint16(ifile);
        }
     if(ifile.fail()) return error("Read failure.");
     // insert vertex info
     vbinfo.push_back(info);
    }

 // -----------------------------------
 // MESH VERTEX BUFFER
 // -----------------------------------

 for(uint32 i = 0; i < n_buffers; i++)
    {
     // vertex format information
     uint16 position_offset = 0xFFFF;
     uint16 position_type = 0xFFFF;
     uint16 normal_offset = 0xFFFF;
     uint16 normal_type = 0xFFFF;
     uint16 uvcoor1_offset = 0xFFFF;
     uint16 uvcoor1_type = 0xFFFF;
     uint16 uvcoor2_offset = 0xFFFF;
     uint16 uvcoor2_type = 0xFFFF;
     uint16 blendweights_offset = 0xFFFF;
     uint16 blendweights_type = 0xFFFF;
     uint16 blendindices_offset = 0xFFFF;
     uint16 blendindices_type = 0xFFFF;

     // number of UV channels
     uint16 uv_channels = 0;

     // set vertex format information
     uint16 n_semantics = vbinfo[i].p03;
     for(uint32 j = 0; j < n_semantics; j++)
        {
         // determine vertex semantic
         DEXMESHVBSEMANTIC dms = vbinfo[i].p06[j];
         uint32 semantic = dms.p01;
         uint16 semantic_offs = dms.p02;
         uint16 semantic_type = dms.p03;

         // position (matches with TR2013)
         if(semantic == 0xD2F7D823) {
            position_offset = semantic_offs;
            position_type = semantic_type;
           }
         // unknown???
         else if(semantic == 0x36F5E414) {
           }
         // normal (mathces with TR2013)
         else if(semantic == 0x3E7F6149) {
            normal_offset = semantic_offs;
            normal_type = semantic_type;
           }
         // texture coordinates (matches with TR2013)
         else if(semantic == 0x8317902A) {
            uvcoor1_offset = semantic_offs;
            uvcoor1_type = semantic_type;
            uv_channels++;
           }
         // texture coordinates (???)
         else if(semantic == 0x8E54B6F3) {
            uvcoor2_offset = semantic_offs;
            uvcoor2_type = semantic_type;
            uv_channels++;
           }
         // blend weights (matches with TR2013)
         else if(semantic == 0x48E691C0) {
            blendweights_offset = semantic_offs;
            blendweights_type = semantic_type;
           }
         // blend indices (matches with TR2013)
         else if(semantic == 0x5156D8D3) {
            blendindices_offset = semantic_offs;
            blendindices_type = semantic_type;
           }
         // unknown ???
         else if(semantic == 0x7E7DD623) {
           }
         // unknown ???
         else if(semantic == 0x64A86F01) {
           }
         // unknown ???
         else if(semantic == 0xF1ED11C3) {
           }
         else {
            stringstream ss;
            ss << "Unknown vertex semantic 0x";
            ss << hex << semantic << dec << ".";
            return error(ss);
           }
        }

     // move to vertex buffer data
     ifile.seekg(mesh_offset + bufferinfo[i].p04[3]);
     if(ifile.fail()) return error("Seek failure.");

     // allocate data to hold one vertex
     boost::shared_array<char> vertex(new char[vbinfo[i].p04]);
     binary_stream bs(vertex, vbinfo[i].p04);

     // allocate vertex buffer
     AMC_VTXBUFFER vb;
     vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV;
     if(normal_type != 0xFFFF) vb.flags |= AMC_VERTEX_NORMAL;
     if(blendweights_type != 0xFFFF) vb.flags |= AMC_VERTEX_WEIGHTS;
     vb.name = "default";
     vb.uvchan = uv_channels;
     vb.uvtype[0] = (uv_channels > 0 ? AMC_DIFFUSE_MAP : AMC_INVALID_MAP);
     vb.uvtype[1] = (uv_channels > 1 ? AMC_DIFFUSE_MAP : AMC_INVALID_MAP);
     vb.uvtype[2] = AMC_INVALID_MAP;
     vb.uvtype[3] = AMC_INVALID_MAP;
     vb.uvtype[4] = AMC_INVALID_MAP;
     vb.uvtype[5] = AMC_INVALID_MAP;
     vb.uvtype[6] = AMC_INVALID_MAP;
     vb.uvtype[7] = AMC_INVALID_MAP;
     vb.colors = 0;
     vb.elem = bufferinfo[i].p06[1];
     vb.data.reset(new AMC_VERTEX[vb.elem]);

     // process vertices
     for(uint32 j = 0; j < vb.elem; j++)
        {
         // read vertex
         LE_read_array(ifile, vertex.get(), vbinfo[i].p04);
         if(ifile.fail()) return error("Read failure.");

         // for each vertex semantic
         for(uint32 k = 0; k < vbinfo[i].p03; k++)
            {
             // determine vertex semantic
             DEXMESHVBSEMANTIC dms = vbinfo[i].p06[k];
             uint32 semantic = dms.p01;
             uint16 semantic_offs = dms.p02;
             uint16 semantic_type = dms.p03;

             // seek vertex semantic offset
             bs.seek(semantic_offs);
             if(bs.fail()) return error("Stream seek failure.");

             // position (matches with TR2013)
             if(semantic == 0xD2F7D823)
               {
                if(semantic_type == 0x02) {
                   vb.data[j].vx = bs.LE_read_real32();
                   vb.data[j].vy = bs.LE_read_real32();
                   vb.data[j].vz = bs.LE_read_real32();
                   vb.data[j].vw = 1.0f;
                  }
                else {
                   stringstream ss;
                   ss << "Unknown vertex position semantic type 0x";
                   ss << hex << semantic_type << dec << ".";
                   return error(ss);
                  }
               }
             // unknown ???
             else if(semantic == 0x36F5E414)
               {
               }
             // normal (mathces with TR2013)
             else if(semantic == 0x3E7F6149)
               {
                if(semantic_type == 0x02) {
                   vb.data[j].nx = bs.LE_read_real32();
                   vb.data[j].ny = bs.LE_read_real32();
                   vb.data[j].nz = bs.LE_read_real32();
                   vb.data[j].nw = 1.0f;
                  }
                else {
                   stringstream ss;
                   ss << "Unknown vertex normal semantic type 0x";
                   ss << hex << semantic_type << dec << ".";
                   return error(ss);
                  }
               }
             // texture coordinates (matches with TR2013)
             else if(semantic == 0x8317902A)
               {
                if(semantic_type == 0x01) {
                   vb.data[j].uv[0][0] = bs.LE_read_real32();
                   vb.data[j].uv[0][1] = bs.LE_read_real32();
                  }
                else if(semantic_type == 0x13) {
                   vb.data[j].uv[0][0] = bs.LE_read_sint16()/2048.0f;
                   vb.data[j].uv[0][1] = bs.LE_read_sint16()/2048.0f;
                  }
                else {
                   stringstream ss;
                   ss << "Unknown vertex texture coordinate semantic type 0x";
                   ss << hex << semantic_type << dec << ".";
                   return error(ss);
                  }
               }
             // texture coordinates (???)
             else if(semantic == 0x8E54B6F3)
               {
                if(uv_channels == 2)
                  {
                   if(semantic_type == 0x01) {
                      vb.data[j].uv[1][0] = bs.LE_read_real32(); // oops! had uv[0][0]
                      vb.data[j].uv[1][1] = bs.LE_read_real32(); // oops! had uv[0][1]
                     }
                   else if(semantic_type == 0x13) {
                      vb.data[j].uv[1][0] = bs.LE_read_sint16()/2048.0f; // oops! had uv[0][0]
                      vb.data[j].uv[1][1] = bs.LE_read_sint16()/2048.0f; // oops! had uv[0][1]
                     }
                   else {
                      stringstream ss;
                      ss << "Unknown vertex texture coordinate (2) semantic type 0x";
                      ss << hex << semantic_type << dec << ".";
                      return error(ss);
                     }
                  }
                else if(uv_channels == 1)
                  {
                   if(semantic_type == 0x01) {
                      vb.data[j].uv[0][0] = bs.LE_read_real32();
                      vb.data[j].uv[0][1] = bs.LE_read_real32();
                     }
                   else if(semantic_type == 0x13) {
                      vb.data[j].uv[0][0] = bs.LE_read_sint16()/2048.0f;
                      vb.data[j].uv[0][1] = bs.LE_read_sint16()/2048.0f;
                     }
                   else {
                      stringstream ss;
                      ss << "Unknown vertex texture coordinate (2) semantic type 0x";
                      ss << hex << semantic_type << dec << ".";
                      return error(ss);
                     }
                  }
               }
             // blend weights (matches with TR2013)
             else if(semantic == 0x48E691C0)
               {
                if(semantic_type == 0x06) {
                   vb.data[j].wv[0] = bs.LE_read_uint08()/255.0f;
                   vb.data[j].wv[1] = bs.LE_read_uint08()/255.0f;
                   vb.data[j].wv[2] = bs.LE_read_uint08()/255.0f;
                   vb.data[j].wv[3] = bs.LE_read_uint08()/255.0f;
                  }
                else {
                   stringstream ss;
                   ss << "Unknown vertex blendweight semantic type 0x";
                   ss << hex << semantic_type << dec << ".";
                   return error(ss);
                  }
               }
             // blend indices (matches with TR2013)
             else if(semantic == 0x5156D8D3)
               {
                if(semantic_type == 0x07) {
                   vb.data[j].wi[0] = static_cast<uint16>(bs.LE_read_uint08());
                   vb.data[j].wi[1] = static_cast<uint16>(bs.LE_read_uint08());
                   vb.data[j].wi[2] = static_cast<uint16>(bs.LE_read_uint08());
                   vb.data[j].wi[3] = static_cast<uint16>(bs.LE_read_uint08());
                   vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                   vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                   vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                   vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
                  }
                else {
                   stringstream ss;
                   ss << "Unknown vertex blendindex semantic type 0x";
                   ss << hex << semantic_type << dec << ".";
                   return error(ss);
                  }
               }
             // unknown ???
             else if(semantic == 0x7E7DD623)
               {
               }
             // unknown ???
             else if(semantic == 0x64A86F01)
               {
               }
             // unknown ???
             else if(semantic == 0xF1ED11C3)
               {
               }
             // error!!!
             else {
                stringstream ss;
                ss << "Unknown vertex semantic 0x";
                ss << hex << semantic << dec << ".";
                return error(ss);
               }
            }
        }

     // insert vertex buffer
     amc.vblist.push_back(vb);
    }

 // -----------------------------------
 // MESH INDEX BUFFER
 // -----------------------------------

 // move to index buffer data
 ifile.seekg(mesh_offset + mh.p13);
 if(ifile.fail()) return error("Seek failure.");

 // read index buffer
 uint32 ib_elem = mh.p03;
 uint32 ib_size = mh.p03 * sizeof(uint16);
 boost::shared_array<char> ib_data(new char[ib_size]);
 LE_read_array(ifile, reinterpret_cast<uint16*>(ib_data.get()), ib_elem);
 if(ifile.fail()) return error("Read failure.");

 // save index buffer
 AMC_IDXBUFFER ib;
 ib.format = AMC_UINT16;
 ib.type = AMC_TRIANGLES;
 ib.wind = AMC_CW;
 ib.name = "default";
 ib.elem = ib_elem;
 ib.data = ib_data;
 ib.reserved = 0;
 amc.iblist.push_back(ib);

 // -----------------------------------
 // MESH SURFACES
 // -----------------------------------

 struct DEXMESHSURFACE {
  real32 p01[4]; // ??? vector
  uint32 p02[3]; // [0] = start index, [1] = triangle count, [2] = vertex count
  uint32 p03[3]; // ??? 0x00, 0x02
  uint32 p04[6]; // ??? material
 };
 deque<DEXMESHSURFACE> surfaces;

 // move to surface data
 ifile.seekg(mesh_offset + mh.p10);
 if(ifile.fail()) return error("Seek failure.");

 // read surfaces
 for(uint32 i = 0; i < mh.p14; i++) {
     DEXMESHSURFACE item;
     LE_read_array(ifile, &item.p01[0], 4);
     LE_read_array(ifile, &item.p02[0], 3);
     LE_read_array(ifile, &item.p03[0], 3);
     LE_read_array(ifile, &item.p04[0], 6);
     if(ifile.fail()) return error("Read failure.");
     surfaces.push_back(item);
    }

 // for each vertex buffer
 uint32 surface_index = 0;
 for(uint32 i = 0; i < n_buffers; i++)
    {
     // number of surfaces for this vertex buffer
     uint32 n_surfaces = bufferinfo[i].p04[0];
     if(!n_surfaces) continue;

     for(uint32 j = 0; j < n_surfaces; j++)
        {
         // surface properties
         uint32 ib_start = surfaces[surface_index].p02[0];
         uint32 ib_width = surfaces[surface_index].p02[1] * 3;
         uint32 vb_width = surfaces[surface_index].p02[2];
         vb_width = bufferinfo[i].p06[1]; // use full vertex buffer instead

         // surface material index
         uint32 surfmat = surfaces[surface_index].p04[0];
         if(!(surfmat < h05)) return error("Surface material index out of range.");

         // create mesh name
         stringstream ss;
         ss << "mesh_" << setfill('0') << setw(3) << i << "_";
         ss << "part_" << setfill('0') << setw(3) << j;

         // create surface
         AMC_SURFACE sur;
         sur.name = ss.str();
         AMC_REFERENCE ref;
         ref.vb_index = i;
         ref.vb_start = 0;
         ref.vb_width = vb_width;
         ref.ib_index = 0;
         ref.ib_start = ib_start;
         ref.ib_width = ib_width;
         ref.jm_index = (jointmaps[i].elem ? i : AMC_INVALID_JMAP_INDEX);
         sur.refs.push_back(ref);
         sur.surfmat = surfmat; // AMC_INVALID_SURFMAT;
         amc.surfaces.push_back(sur);

         // increment surface index
         surface_index++;
        }
    }

 // mesh information
 // BIGFILE.000/0x3CEA/0049.MODEL (0x40 byte items) 0x1B items
 // 73959CC2 B3F01A45 3B8A4FC6 0000803F - 00000000 28380000 A3640000 - 00000000 00000000 00000000 - 01000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 7C0D3A43 3C2B2645 C75892C6 0000803F - 78A80000 F5190000 921B0000 - 00000000 00000000 00000000 - 02000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 98578BBF 2EB2F044 A9EF9DC6 0000803F - 57F60000 34000000 68000000 - 00000000 00000000 00000000 - 03000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 29B99F45 7329CE45 C8689EC6 0000803F - F3F60000 FE070000 D1080000 - 00000000 00000000 00000000 - 04000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // BFF003C0 7C8BDE44 83139CC6 0000803F - ED0E0100 46030000 4C060000 - 00000000 00000000 00000000 - 05000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000 
 // 00A0C8BA 57AD4B44 3ACD68C6 0000803F - BF180100 70000000 E0000000 - 00000000 00000000 00000000 - 07000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 7A88C2C5 850F0146 87C375C6 0000803F - 0F1A0100 C4000000 EE000000 - 02000000 00000000 00000000 - 08000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 1D702F44 87900B46 91CC75C6 0000803F - 5B1C0100 B4040000 C6050000 - 02000000 00000000 00000000 - 09000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 4337E045 89CFB445 9252C6C5 0000803F - 772A0100 34000000 67000000 - 00000000 00000000 00000000 - 0A000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // EEB7E245 1D248D45 AE5149C6 0000803F - 132B0100 2A000000 39000000 - 00000000 00000000 00000000 - 0B000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 019010C4 F1734345 627579C6 0000803F - 912B0100 8C000000 9A000000 - 00000000 00000000 00000000 - 0D000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // DD249B38 D7F3CE44 3F3780C6 0000803F - 352D0100 40000000 58000000 - 00000000 00000000 00000000 - 0E000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 0000403A 4C3F1E45 00EA76C6 0000803F - F52D0100 10000000 18000000 - 00000000 00000000 00000000 - 0F000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 00000000 D357E544 66269EC6 0000803F - 252E0100 04000000 08000000 - 00000000 00000000 00000000 - 10000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // EFFEF4C5 20E81445 A2C787C6 0000803F - 312E0100 C5050000 29080000 - 00000000 00000000 00000000 - 00000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // EE4C3840 96E4E544 FEEC9CC6 0000803F - 803F0100 A8010000 42030000 - 00000000 00000000 00000000 - 06000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // FE95C543 47B59044 2AD2A5C6 0000803F - 78440100 52050000 A2060000 - 00000000 00000000 00000000 - 02000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // A2EBCC45 169ACF45 070E5BC6 0000803F - 6E540100 6C030000 D8020000 - 00000000 00000000 00000000 - 04000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 66B9D245 16B9C145 159A08C6 0000803F - B25E0100 38000000 70000000 - 00000000 00000000 00000000 - 01000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 3C41C045 A308C145 EF033FC6 0000803F - 5A5F0100 0C000000 18000000 - 00000000 00000000 00000000 - 0C000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // C4ACD744 85EB8D45 94EA32C6 0000803F - 7E5F0100 78030000 C3020000 - 00000000 00000000 00000000 - 02000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // F6EAAF45 08F02146 1E7952C6 0000803F - E6690100 E0000000 FC000000 - 00000000 00000000 00000000 - 04000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 019010C4 F1734345 627579C6 0000803F - 866C0100 8C000000 9A000000 - 00000000 00000000 00000000 - 0D000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // DD249B38 D7F3CE44 3F3780C6 0000803F - 2A6E0100 40000000 58000000 - 00000000 00000000 00000000 - 0E000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // 0000403A 4C3F1E45 00EA76C6 0000803F - EA6E0100 10000000 18000000 - 00000000 00000000 00000000 - 0F000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000 
 // 00000000 D357E544 66269EC6 0000803F - 1A6F0100 04000000 08000000 - 00000000 00000000 00000000 - 10000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000
 // EE4C3840 96E4E544 FEEC9CC6 0000803F - 266F0100 A8010000 42030000 - 00000000 00000000 00000000 - 06000000 - FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF - 00000000

 // save SMC file
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 return true;
}

bool processMTL(const string& filename)
{
 // TODO: MATERIAL PROCESSING UNFINISHED
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("error");

 // read header
 uint32 h01 = LE_read_uint32(ifile); // A
 uint32 h02 = LE_read_uint32(ifile); // 0
 uint32 h03 = LE_read_uint32(ifile); // 0
 uint32 h04 = LE_read_uint32(ifile); // 0
 uint32 h05 = LE_read_uint32(ifile); // B
 if(ifile.fail()) return error("error");

 // read header entries part 1
 deque<pair<uint32, uint32>> h06;
 for(uint32 i = 0; i < h01; i++) {
     uint32 p01 = LE_read_uint32(ifile);
     uint32 p02 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     h06.push_back(pair<uint32, uint32>(p01, p02));
    }

 // read header entries part 2
 deque<uint32> h07;
 for(uint32 i = 0; i < h05; i++) {
     uint32 item = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     h07.push_back(item);
    }

 // start offset
 uint32 start = 0x14 + (h01 * 0x08) + (h05 * 0x04);

 // BEGIN DEBUG
 // add code to frequency map
 fmap.insert(make_pair(h01, filename));
 // END DEBUG

 // code = 0x16
 if(h01 == 0x16)
   {
    // seek offset1
    uint32 offset1 = h06[0x08].second;
    ifile.seekg(start + offset1);
    if(ifile.fail()) return error("Seek failure.");

    // read data
    uint32 p01 = LE_read_uint32(ifile); // number ???
    uint32 p02 = LE_read_uint32(ifile); // number ???
    uint32 p03 = LE_read_uint32(ifile); // 0
    uint32 p04 = LE_read_uint32(ifile); // 0
    uint32 p05 = LE_read_uint32(ifile); // 2
    uint16 p06 = LE_read_uint08(ifile); // number of textures
    uint16 p07 = LE_read_uint08(ifile); // number ???
    uint16 p08 = LE_read_uint08(ifile); // number ???
    uint16 p09 = LE_read_uint08(ifile); // number ???

    // seek offset2
    uint32 offset2 = h06[0x09].second;
    ifile.seekg(start + offset2);
    if(ifile.fail()) return error("Seek failure.");

    for(uint32 i = 0; i < p06; i++) {
        uint32 t01 = LE_read_uint32(ifile);
        uint32 t02 = LE_read_uint32(ifile);
        uint32 t03 = LE_read_uint32(ifile);
        uint16 t04 = LE_read_uint08(ifile);
        uint16 t05 = LE_read_uint08(ifile);
        uint16 t06 = LE_read_uint08(ifile);
        uint16 t07 = LE_read_uint08(ifile);
        if(ifile.fail()) return error("Read failure.");
       }
   }

 // code = 0xb
 // code = 0xc
 // code = 0xd
 // code = 0xe
 // code = 0xf
 // code = 0x11
 // code = 0x13
 // code = 0x15 (no textures)
 // code = 0x16
 // code = 0x17
 // code = 0x18
 // code = 0x19
 // code = 0x1a
 // code = 0x1b
 // code = 0x1c
 // code = 0x1d
 // code = 0x1e
 // code = 0x20
 // code = 0x27

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
 using namespace std;
 std::string pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname();

 // variables
 bool delete_useless = true;
 bool do000 = true;
 bool doDEX = true;
 bool doPCD = true;
 bool doMOD = true;

 // ask questions
 if(!YesNoBox("This ripping program generates millions of intermediate files.\nIs it OK to delete useless ones?")) delete_useless = false;
 if(!YesNoBox("Process 000 (archive) files?\nClick 'No' if you have already done so.")) do000 = false;
 if(!YesNoBox("Process DEX (CDRM, compressed DRM) files?\nClick 'No' if you have already done so.")) doDEX = false;
 if(!YesNoBox("Process PCD (texture) files?\nClick 'No' if you have already done so.")) doPCD = false;
 if(!YesNoBox("Process MOD (model) files?\nClick 'No' if you have already done so.")) doMOD = false;

 // process archive
 cout << "STAGE 1" << endl;
 if(do000) {
    cout << "Processing .000 files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".000", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!process000(filelist[i], delete_useless)) return false;
       }
    cout << endl;
   }

 // process DEX
 cout << "STAGE 2" << endl;
 if(doDEX) {
    cout << "Processing .DEX files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".dex", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processDEX(filelist[i], delete_useless)) return false;
       }
    cout << endl;
   }

 // process PCD
 cout << "STAGE 3" << endl;
 if(doPCD) {
    cout << "Processing .PCD files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".pcd", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processPCD(filelist[i])) return false;
       }
    cout << endl;
   }

 // process MOD
 cout << "STAGE 4" << endl;
 if(doMOD) {
    cout << "Processing .MODEL files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".model", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMOD(filelist[i])) return false;
       }
    cout << endl;
   }

 // process MTL_A
 if(0) {
    cout << "STAGE 5" << endl;
    cout << "Processing .MTL_A files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".mtl_a", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMTL(filelist[i])) return false;
       }
    cout << endl;
   }

 // create bone count file
 string bfname = pathname;
 bfname += "bonecount.txt";
 ofstream ofile(bfname.c_str());
 if(!ofile) return error("Failed to create bone count file.");

 // sort bone counts
 struct bonepred {
  typedef pair<uint32,string> value_type;
  bool operator ()(const value_type& p1, const value_type& p2)const {
   return p1.first < p2.first;
  }
 };
 sort(bonelist.begin(), bonelist.end(), bonepred());

 // print bone counts
 for(uint32 i = 0; i < bonelist.size(); i++)
     ofile << bonelist[i].first << " - " << bonelist[i].second << endl;

 // // DEBUG BEGIN
 // for(auto miter = fmap.begin(); miter != fmap.end(); miter++) {
 //     hahaha << "code = 0x" << hex << miter->first << dec << endl;
 //     for(auto diter = miter->second.begin(); diter != miter->second.end(); diter++)
 //         hahaha << " " << *diter << endl;
 //    }
 // // DEBUG END

 return true;
}

}};

