#include "xentax.h"
#include "x_math.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "ps3_re6.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   RESIDENT_EVIL_6

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool testZLIB(uint08 b1, uint08 b2)
{
 // first byte check
 if(b1 == 0x18) ;
 else if(b1 == 0x28) ;
 else if(b1 == 0x38) ;
 else if(b1 == 0x48) ;
 else if(b1 == 0x58) ;
 else if(b1 == 0x68) ;
 else if(b1 == 0x78) ;
 else return false;

 // second byte check
 uint32 u1 = (uint32)b1;
 uint32 u2 = (uint32)b2;
 return ((u1*256 + u2) % 31) == 0;
}

bool processRE6(const std::string& filename)
{
 // open file
 if(!filename.length()) return false;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return false;

 // read first four bytes
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // change file extension
 std::string movename;
 if(magic == 0x00444F4D) movename = ChangeFileExtension(filename, "mod"); // model
 else if(magic == 0x00584554) movename = ChangeFileExtension(filename, "tex"); // texture
 else if(magic == 0x00585452) movename = ChangeFileExtension(filename, "rtx"); // texture
 else if(magic == 0x004E4145) movename = ChangeFileExtension(filename, "ean"); // texture
 else if(magic == 0x00443245) movename = ChangeFileExtension(filename, "e2d"); // effect
 else if(magic == 0x004C4645) movename = ChangeFileExtension(filename, "efl"); // effect
 else if(magic == 0x00534645) movename = ChangeFileExtension(filename, "efs"); // effect
 else if(magic == 0x00575247) movename = ChangeFileExtension(filename, "grw"); // effect
 else if(magic == 0x004C4453) movename = ChangeFileExtension(filename, "sdl"); // effect
 else if(magic == 0x004D434C) movename = ChangeFileExtension(filename, "lcm");
 else if(magic == 0x00544D4C) movename = ChangeFileExtension(filename, "lmt");
 else if(magic == 0x004C524D) movename = ChangeFileExtension(filename, "mrl");
 else if(magic == 0x00444647) movename = ChangeFileExtension(filename, "gfd"); // GUI
 else if(magic == 0x00494947) movename = ChangeFileExtension(filename, "gii"); // GUI
 else if(magic == 0x00444D47) movename = ChangeFileExtension(filename, "gmd"); // GUI
 else if(magic == 0x00495547) movename = ChangeFileExtension(filename, "gui"); // GUI
 else if(magic == 0x00555145) movename = ChangeFileExtension(filename, "equ"); // sound
 else if(magic == 0x00564552) movename = ChangeFileExtension(filename, "rev"); // sound
 else if(magic == 0x00584D53) movename = ChangeFileExtension(filename, "smx"); // sound
 else if(magic == 0x004C5053) movename = ChangeFileExtension(filename, "spl"); // sound
 else if(magic == 0x00534658) movename = ChangeFileExtension(filename, "xfs"); // sound
 else if(magic == 0x52494646) movename = ChangeFileExtension(filename, "wav"); // sound
 else if(magic == 0x00544D42) movename = ChangeFileExtension(filename, "bmt"); // unknown (model related)
 else if(magic == 0x534243FF) movename = ChangeFileExtension(filename, "cbs"); // unknown (stage related)
 else if(magic == 0x004C4343) movename = ChangeFileExtension(filename, "ccl"); // unknown
 else if(magic == 0x00435443) movename = ChangeFileExtension(filename, "ctc"); // unknown
 else if(magic == 0x004D5744) movename = ChangeFileExtension(filename, "dwm"); // unknown (model related)
 else if(magic == 0x326F6567) movename = ChangeFileExtension(filename, "geo"); // unknown (model related)
 else if(magic == 0x00564E4C) movename = ChangeFileExtension(filename, "lnv"); // unknown (stage related)
 else if(magic == 0x0044524C) movename = ChangeFileExtension(filename, "lrd"); // unknown
 else if(magic == 0x0047544D) movename = ChangeFileExtension(filename, "mtg"); // unknown (model related)
 else if(magic == 0x0056414E) movename = ChangeFileExtension(filename, "nav"); // unknown (stage related)
 else if(magic == 0x0043434F) movename = ChangeFileExtension(filename, "occ"); // unknown
 else if(magic == 0x53524750) movename = ChangeFileExtension(filename, "pgr"); // unknown (stage related)
 else if(magic == 0x5A505250) movename = ChangeFileExtension(filename, "prp"); // unknown (stage related)
 else if(magic == 0x00444252) movename = ChangeFileExtension(filename, "rbd"); // unknown
 else if(magic == 0x00444452) movename = ChangeFileExtension(filename, "rdd"); // unknown
 else if(magic == 0x004B5053) movename = ChangeFileExtension(filename, "spk"); // unknown
 else if(magic == 0x004D5753) movename = ChangeFileExtension(filename, "swm"); // unknown
 else if(magic == 0x004B4854) movename = ChangeFileExtension(filename, "thk"); // unknown (model related)
 else if(magic == 0x00000009) movename = ChangeFileExtension(filename, "unk"); // unknown
 else if(magic == 0x00424956) movename = ChangeFileExtension(filename, "vib"); // unknown
 else if(magic == 0x00594157) movename = ChangeFileExtension(filename, "way"); // unknown (stage related)
 else if(magic == 0x006E6F7A) movename = ChangeFileExtension(filename, "zon"); // unknown (sound related)
 else if(magic == 0x53524E44) movename = ChangeFileExtension(filename, "dnrs"); // sound
 else if(magic == 0x54534353) movename = ChangeFileExtension(filename, "scst"); // sound
 else if(magic == 0x54534453) movename = ChangeFileExtension(filename, "sdst"); // sound
 else if(magic == 0x43415053) movename = ChangeFileExtension(filename, "spac"); // sound
 else if(magic == 0x51455253) movename = ChangeFileExtension(filename, "sreq"); // sound
 else if(magic == 0x51525453) movename = ChangeFileExtension(filename, "strq"); // sound
 else return error(filename.c_str());

 // close and delete previous file
 ifile.close();
 DeleteFileA(movename.c_str());

 // rename file
 BOOL success = MoveFileA(filename.c_str(), movename.c_str());
 if(!success) error(filename.c_str());
 return (success == TRUE);
}

bool processARC(const std::string& datapath, const std::string& filename)
{
 // open file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open ARC file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 arcsize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(arcsize == 0) return error("Empty ARC file.");

 // read magic number
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure 0x01."); 
 if(magic != 0x435241) return error("Invalid magic number.");

 // read 16-bit unknown (usually 0x08)
 uint16 unk01 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure 0x02."); 
 if(unk01 != 0x08) return error("Warning: ARC header unknown is not 0x08.");

 // read number of files
 uint16 n_files = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure 0x03."); 
 if(n_files == 0) return error("Invalid number of files in ARC file.");

 // header
 struct ARCHEADER {
  char filename[64];
  uint32 unk01;
  uint32 datasize;
  uint32 unk02;
  uint32 offset;
 };
 deque<ARCHEADER> filelist;

 // read header
 for(uint32 i = 0; i < n_files; i++)
    {
     // read filename
     ARCHEADER item;
     if(!BE_read_array(ifile, &item.filename[0], 64)) return error("Read failure 0x04.");

     // read unknown
     item.unk01 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure 0x05.");

     // read compressed data size
     item.datasize = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure 0x06.");

     // read unknown
     item.unk02 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure 0x07.");

     // read file offset
     item.offset = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure 0x08.");

     // insert file
     filelist.push_back(item);
    }

 // build directories and decompress data
 for(uint32 i = 0; i < n_files; i++)
    {
     // build output filename
     string ofname = datapath;
     ofname += filelist[i].filename;
     ofname += ".re6";
     if(!ofname.length()) return error("Invalid output filename.");

     // build output pathname
     string ofpath = GetPathnameFromFilename(ofname);
     if(!ofpath.length()) return error("Invalid output pathname.");

     // create directories
     int result = SHCreateDirectoryExA(NULL, ofpath.c_str(), NULL);
     if(result == ERROR_SUCCESS) ;
     else if(result == ERROR_FILE_EXISTS) ;
     else if(result == ERROR_ALREADY_EXISTS) ;
     else if(result == ERROR_BAD_PATHNAME) return error("SHCreateDirectoryExA:ERROR_BAD_PATHNAME");
     else if(result == ERROR_FILENAME_EXCED_RANGE) return error("SHCreateDirectoryExA:ERROR_FILENAME_EXCED_RANGE");
     else if(result == ERROR_PATH_NOT_FOUND) return error("SHCreateDirectoryExA:ERROR_PATH_NOT_FOUND");
     else if(result == ERROR_CANCELLED) return error("SHCreateDirectoryExA:ERROR_CANCELLED");
     else return error("Unknown SHCreateDirectoryExA error.");

     // create output file
     ofstream ofile(ofname.c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // move to data offset
     uint32 curr_offset = filelist[i].offset;
     ifile.seekg(curr_offset);
     if(ifile.fail()) return error("Seek failure.");

     // compute next offset
     uint32 next_offset = 0;
     if((i + 1) == n_files) next_offset = arcsize;
     else next_offset = filelist[i + 1].offset;

     // compute datasize for compression test
     if(next_offset < curr_offset) return error("Failed to compute data size from offsets.");
     uint32 datasize = next_offset - curr_offset;

     // first two bytes of data to determine compression
     uint08 b1 = BE_read_uint08(ifile);
     uint08 b2 = BE_read_uint08(ifile);

     // move to data offset again
     ifile.seekg(curr_offset);
     if(ifile.fail()) return error("Seek failure.");

     // decompress and save data
     if(testZLIB(b1, b2)) {
        if(!DecompressZLIB(ifile, filelist[i].datasize, ofile, 0)) {
           cout << " " << ofname.c_str() << endl;
           cout << " offset = 0x" << hex << filelist[i].offset << ", 0x" << filelist[i].unk01 << ", 0x" << filelist[i].unk02 << dec << endl;
           return error("Failed to decompress ZLIB data.");
          }
       }
     //  read and save data
      else
       {
         // can we read all of the data?
         uint32 bytes_remaining = arcsize - filelist[i].offset;
         if(bytes_remaining < filelist[i].datasize) {
            cout << "offset = 0x" << hex << curr_offset << dec << endl;
            cout << "datasize = 0x" << hex <<  filelist[i].datasize << dec << endl;
            filelist[i].datasize = bytes_remaining;
            message("WARNING! There is not enough data in the ARC file to read all bytes of the file.");
           }

         // read data
         boost::shared_array<char> data(new char[filelist[i].datasize]);
         ifile.read(data.get(), filelist[i].datasize);
         if(ifile.fail()) return error("Read failure 0x09.");
         ofile.write(data.get(), filelist[i].datasize);
         if(ofile.fail()) return error("Write failure 0x0A.");
        }

     // close output file and rename
     ofile.close();
     if(!processRE6(ofname)) return error("Failed to rename RE6 file.");
    }

 // close and delete ARC file (optional)
 ifile.close();
 //DeleteFileA(filename.c_str());
 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processTEX(const std::string& filename)
{
 // open file
 if(!filename.length()) return error("Invalid filename.");
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unk01
 uint32 unk01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(unk01 == 0x6000009A) return true; // TODO:CUBEMAP! data always starts at 0x10

 // dx - dy - mipmaps
 uint32 props = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 uint32 dx = ((props & 0x000FFF00) >>  6);
 uint32 dy = ((props & 0xFFF00000) >> 19);
 uint32 mipmaps = (props & 0xFF) - 1;
 // cout << "dx = " << dx << endl;
 // cout << "dy = " << dy << endl;
 // cout << "mipmaps = " << mipmaps << endl;

 // read type
 uint32 type = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // offset to first mipmap
 uint32 start = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to first offset
 ifile.seekg(start);
 if(ifile.fail()) return error("Seek failure.");

 // read data
 if(filesize < start) return error("Texture filesize is less than the start offset.");
 uint32 datasize = filesize - start;
 if(!datasize) return error("Invalid texture data size.");
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 if(ifile.fail()) return error("Read failure.");

 // create DDS header
 DDS_HEADER ddsh;
 uint32 ddstype = ((type & 0xFF00) >> 8);

 // OK (alpha masks)
 if(ddstype == 0x0E)
   {
    // reverse floating-point byte-order
    uint32 pixels = dx * dy;
    real32* src = reinterpret_cast<real32*>(data.get());
    reverse_byte_order(src, pixels);

    // convert D3DFMT_R32F to D3DFMT_R8G8B8A8 data
    boost::shared_array<char> dst(new char[datasize]);
    for(uint32 i = 0; i < pixels; i++) {
        uint32 src_index = i;
        uint32 dst_index = i * 4;
        real32 c1 = 255.0f * src[src_index + 0];
        real32 c2 = 255.0f * src[src_index + 1];
        real32 c3 = 255.0f * src[src_index + 2];
        real32 c4 = 255.0f * src[src_index + 3];
        dst[dst_index + 0] = (uint08)(c1 < 0 ? 0 : (c1 > 255 ? 255 : c1));
        dst[dst_index + 1] = (uint08)(c2 < 0 ? 0 : (c2 > 255 ? 255 : c2));
        dst[dst_index + 2] = (uint08)(c3 < 0 ? 0 : (c3 > 255 ? 255 : c3));
        dst[dst_index + 3] = (uint08)(c4 < 0 ? 0 : (c4 > 255 ? 255 : c4));
       }
    data = dst;

    // create DDS header
    if(!CreateR8G8B8A8DDSHeader(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 else if(ddstype == 0x13) {
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 else if(ddstype == 0x14) {
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 else if(ddstype == 0x17) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 else if(ddstype == 0x18) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 else if(ddstype == 0x19) {
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (normal maps _DM and _NM)
 else if(ddstype == 0x1E) {
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (only em5900_20Dmap_DM_HQ)
 else if(ddstype == 0x1F) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (interior textures)
 else if(ddstype == 0x23) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (crack normal maps)
 else if(ddstype == 0x25) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (noise)
 else if(ddstype == 0x27) {
    dx = ((props & 0x0000FF00) >>  6);
    dy = ((props & 0xFFFF0000) >> 19);
    if(!CreateR8G8B8X8DDSHeader(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (SWIZZLED UNFINISHED)
 else if(ddstype == 0x28)
   {
    // deswizzle
    if(dx == dy) {
       boost::shared_array<char> copy(new char[datasize]);
       for(uint32 r = 0; r < dy; r++) {
           for(uint32 c = 0; c < dx; c++) {
               uint32 morton = array_to_morton(r, c);
               uint32 copy_position = 4*r*dx + 4*c;
               uint32 data_position = 4*morton;
               copy[copy_position + 0] = data[data_position + 0];
               copy[copy_position + 1] = data[data_position + 1];
               copy[copy_position + 2] = data[data_position + 2];
               copy[copy_position + 3] = data[data_position + 3];
              }
          }
       data = copy;
      }
    else {
       // what about non-square images?
       // just ignore for now
       return true;
      }

    // create DDS header
    if(!CreateR8G8B8X8DDSHeader(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 else if(ddstype == 0x2F) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 else
    return error("Unknown texture format.");

 // create ouptfile
 string ofname = ChangeFileExtension(filename, "dds");
 ofstream ofile(ofname.c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // save DDS header
 ofile.write("DDS ", 4);
 ofile.write((char*)&ddsh, sizeof(ddsh));
 if(ofile.fail()) return error("Write failure.");

 // save data
 ofile.write(data.get(), datasize);
 if(ofile.fail()) return error("Write failure.");

 return true;
}

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

// FILE FORMAT EXAMPLE (LEON PL0000.MOD)
// 0x000000 - 0x000090: header
// 0x000090 - 0x0005A0: bone information part 1
// 0x0005A0 - 0x001320: bone information part 2
// 0x001320 - 0x0020A0: bone information part 3
// 0x0020A0 - 0x0021A0: unknown data (0x100 bytes) bone data?
// 0x0021A0 - 0x0024C0: unknown data (0x320 bytes, each item is 0x20 bytes so 0x19 entries, this is what header data at unknown 0x20 is for)
// 0x0024C0 - 0x0024D4: material data
// 0x0024D4 - 0x003B5C: mesh information (0x1688 bytes, each item is 0x38 bytes so 0x67 entries)
// 0x003B5C - 0x003B60: number of 0x90 byte entries
// 0x003B64 - 0x017F60: 0x90 byte entries
// 0x017F60 - 0x0AA538: vertex buffer list
// 0x0AA538 - 0x0C2FB0: index buffer list
// 0x0C2FB0 - 0x0C2FB4: end of file marker

// 0x00000000: magic
// 0x00000004: 0xFF2C
// 0x00000006: number of joints (YES)
// 0x00000008: number of meshes (YES)
// 0x0000000A: number of materials (YES)
// 0x0000000C: number of vertices (YES)
// 0x00000010: number of indices (YES)
// 0x00000014: number of triangles excluding non-planar edge geometry (YES)
// 0x00000018: size of all vertex buffers (YES)
// 0x00000020: number of ??? entries (0x20 byte items)
// 0x00000024: number of joint maps
// 0x00000028: offset to joint data that contains 0x18 byte items + 0x40 byte matrices
// 0x0000002C: offset to ??? entries (0x20 byte items)
// 0x00000030: offset to material information (0x4 bytes per item)
// 0x00000034: offset to mesh information (0x38 bytes per item)
// 0x00000038: offset to vertex buffer data
// 0x0000003C: offset to index buffer data
// 0x00000040: offset to end of file
// 0x00000044: 0x00000000
// 0x00000048: 0x00000000
// 0x0000004C: 0x00000000
// 0x00000050: float, float, float, float
// 0x00000060: float, float, float, float
// 0x00000070: float, float, float, float
// 0x00000080: 0x03E8
// 0x00000084: 0x0BB8
// 0x00000088: unknown
// 0x0000008C: 0x0000

// BONE MAP LIST
// BLEND INDEX TO JOINT INDEX MAPPING
// EXAMPLE FROM EM4100.MOD
// 2A - 01 02 03 04 05 06 08 09 0F 10 12 13 14 15 16 17
//      18 19 1A 1B 1C 1D 1E 20 21 22 23 24 25 27 47 48
//      4A 4B 4C 50 51 52 53 54 55 56
// 29 - 01 02 03 04 05 06 08 09 0A 0B 0C 0D 0E 0F 10 12
//      13 14 15 16 17 18 19 1A 1B 1C 1D 1E 20 21 22 23
//      24 25 27 42 51 52 53 54 55 00 (00 is padding)
// 29 - 01 02 03 04 05 06 07 09 0A 11 12 27 28 29 2A 2B
//      2C 2D 2E 2F 30 31 32 33 35 36 37 38 39 3A 49 4A
//      4B 4E 50 51 52 56 57 58 59 00 (00 is padding)
// 27 - 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 12
//      13 14 15 16 17 18 19 1A 1C 27 3E 3F 40 41 42 46
//      4C 51 52 53 54 55 56 00 00 00 (00 is padding)
// 25 - 01 02 03 04 05 06 08 09 0A 0F 10 12 13 14 15 16
//      17 18 19 1A 1B 1C 1D 1E 20 21 22 23 24 47 4C 51
//      52 53 54 55 56 00 00 00 00 00 (00 is padding)
// 29 - 01 02 03 04 05 06 07 09 0A 0B 0C 0D 0E 11 12 27
//      28 29 2A 2B 2C 2D 2E 2F 30 31 32 33 35 36 37 38
//      39 3A 42 51 52 56 57 58 59 00 (00 is padding)
// 29 - 00 01 02 03 04 05 06 07 09 0A 0B 0C 0D 0E 12 27
//      28 29 2A 2B 2C 2D 2E 2F 30 31 35 3F 40 42 44 45
//      46 4D 4E 51 52 56 57 58 59 00 (00 is padding)

struct RE6HEADER {
 uint32 magic;
 uint16 signature;
 uint16 joints;
 uint16 meshes;
 uint16 materials;
 uint32 vertices;
 uint32 indices;
 uint32 triangles;
 uint32 vbtotalbytes;
 uint32 unk01;
 uint32 unkentries;
 uint32 n_jointmaps;
 uint32 jointoffset;
 uint32 unkoffset;
 uint32 material_offset;
 uint32 meshinfo_offset;
 uint32 vertdata_offset;
 uint32 facedata_offset;
 uint32 eofoffset;
 uint32 pad01;
 uint32 pad02;
 uint32 pad03;
 real32 sphere_cx;
 real32 sphere_cy;
 real32 sphere_cz;
 real32 sphere_cr;
 real32 bbox_min_x;
 real32 bbox_min_y;
 real32 bbox_min_z;
 real32 bbox_min_w;
 real32 bbox_max_x;
 real32 bbox_max_y;
 real32 bbox_max_z;
 real32 bbox_max_w;
 uint32 unk03;
 uint32 unk04;
 uint32 unk05;
 uint32 unk06;
};

struct RE6BONEINFO {
 uint08 j01; // jntmap index
 uint08 j02; // parent index
 uint08 j03; // index of mirrored joint (if present)
 uint08 j04; // 0x00
 real32 j05; // unknown floating-point values
 real32 j06; // unknown floating-point values
 real32 j07; // unknown floating-point values
 real32 j08; // unknown floating-point values
 real32 j09; // unknown floating-point values
};

struct RE6BONEDATA {
 std::deque<RE6BONEINFO> boneinfo;
 std::deque<boost::shared_array<real32>> m1;
 std::deque<boost::shared_array<real32>> m2;
 std::deque<boost::shared_array<uint16>> jointmaplist;
};

struct RE6MESHINFO {
 uint16 meshtype;      // 0x00: mesh type
 uint16 vertices;      // 0x02: number of vertices in mesh
 uint08 unkbyteflag1;  // 0x04: ?
 uint08 unkbyteflag2;  // 0x05: ?
 uint08 material;      // 0x06: material index
 uint08 lodvalue;      // 0x07: level of detail
 uint08 unkbyteflag3;  // 0x08: ?
 uint08 unkbyteflag4;  // 0x09: ?
 uint08 vertexsize;    // 0x0A: vertex size
 uint08 unkbyteflag5;  // 0x0B: ?
 uint32 vertexindex1;  // 0x0C: vertex base index (lower)
 uint32 vertexoffset;  // 0x10: offset to vertex buffer base 0
 uint16 vertexformat1; // 0x14: vertex format #1
 uint16 vertexformat2; // 0x16: vertex format #2
 uint32 startindex;    // 0x18: index buffer base index
 uint32 indices;       // 0x1C: number of indices
 uint32 vertexindex2;  // 0x20: vertex base index (upper)
 uint08 jointmapindex; // 0x24: index to blend index/joint index map
 uint08 weightmaps;    // 0x25: number of weight maps used
 uint08 unkflag2;      // 0x26: ?
 uint08 unkflag3;      // 0x27: unique identifier
 uint16 min_index;
 uint16 max_index;
 uint16 unkshort1;
 uint16 unkshort2;
 uint32 zero1;
 uint32 zero2;
};

struct RE6MESHDATA {
 std::deque<RE6MESHINFO> meshinfo;
 boost::shared_array<char> vbuffer;
 boost::shared_array<uint16> ibuffer;
};

struct RE6MTRLINFO {
 uint32 signature; // 0x5FB0EBE4
 uint32 id;        // id
 uint32 size1;     // size of data
 uint32 unk01;     // unknown
 uint32 unk02;     // unknown
 uint32 unk03;     // unknown
 uint16 entryinfo; // XXXY (X = number of entries / Y = size of entry)
 uint16 unk04;     // unknown
 uint16 unk05;     // unknown
 uint16 unk06;     // unknown
 uint32 zero1;     // zero #1
 uint32 zero2;     // zero #2
 uint32 zero3;     // zero #3
 uint32 zero4;     // zero #4
 uint32 size2;     // size of data at bottom of file
 uint32 offset1;   // offset to texture association data
 uint32 offset2;   // offset to data at bottom of file
};

struct RE6MTRLDATA {
 std::deque<uint32> idlist;
 std::deque<std::string> filelist;
 std::deque<RE6MTRLINFO> mtrlinfo;
 std::map<uint32,uint32> mdiffuse;
};

struct RE6TRIANGLE {
 uint32 a; // vertex buffers can go beyond 0xFFFF points
 uint32 b; // vertex buffers can go beyond 0xFFFF points
 uint32 c; // vertex buffers can go beyond 0xFFFF points
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
  RE6HEADER header;
  RE6BONEDATA bonedata;
  RE6MESHDATA meshdata;
  RE6MTRLDATA mtrldata;
  ADVANCEDMODELCONTAINER amc;
 private :
  uint32 wmap_base;
 private :
  bool processVBx0C(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
  bool processVBx10(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
  bool processVBx14(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
  bool processVBx18(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
  bool processVBx1C(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
  bool processVBx20(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
  bool processVBx24(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
  bool processVBx28(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
  bool processVBx30(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
  bool processVBx40(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb);
 private :
  bool process001(void); // read header
  bool process002(void); // read bone data
  bool process003(void); // read mesh data
  bool process004(void); // read material data
  bool process005(void); // process vbuffers
  bool process006(void); // process ibuffers
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

bool modelizer::process001(void)
{
 // debug header
 if(debug) {
    dfile << "------" << endl;
    dfile << "HEADER" << endl;
    dfile << "------" << endl;
    dfile << endl;
   }

 // read magic
 header.magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.magic != 0x444F4D) return error("Invalid magic number.");

 // read signature (0x2CFF)
 header.signature = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.signature != 0xFF2C) return error("Expecting 0x2CFF.");

 // read number of joints
 header.joints = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of meshes
 header.meshes = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of materials
 header.materials = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of vertices
 header.vertices = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of indices
 header.indices = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of triangles excluding non-planar edge geometry
 header.triangles = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read total vertex buffer sizes in bytes
 header.vbtotalbytes = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown
 header.unk01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.unk01 != 0) return error("Expecting 0x00000000 in header.");

 // read number of 0x20 byte entries
 header.unkentries = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown (the number of something... not sure)
 header.n_jointmaps = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read joint data (0x18 byte parenting information + 0x40 byte matrices)
 header.jointoffset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to unknown data
 header.unkoffset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to material data
 header.material_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to mesh information (contains an array of 0x3A byte entries)
 header.meshinfo_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read vertex buffer offset
 header.vertdata_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read index buffer offset
 header.facedata_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to filesize - 0x4
 header.eofoffset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read padding zeros
 header.pad01 = BE_read_uint32(ifile); // 0x00
 header.pad02 = BE_read_uint32(ifile); // 0x00
 header.pad03 = BE_read_uint32(ifile); // 0x00
 if(ifile.fail()) return error("Read failure.");

 // read bounding sphere
 header.sphere_cx = BE_read_real32(ifile);
 header.sphere_cy = BE_read_real32(ifile);
 header.sphere_cz = BE_read_real32(ifile);
 header.sphere_cr = BE_read_real32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read min bounding box
 header.bbox_min_x = BE_read_real32(ifile);
 header.bbox_min_y = BE_read_real32(ifile);
 header.bbox_min_z = BE_read_real32(ifile);
 header.bbox_min_w = BE_read_real32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read max bounding box
 header.bbox_max_x = BE_read_real32(ifile);
 header.bbox_max_y = BE_read_real32(ifile);
 header.bbox_max_z = BE_read_real32(ifile);
 header.bbox_max_w = BE_read_real32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknowns at end of header
 header.unk03 = BE_read_uint32(ifile);
 header.unk04 = BE_read_uint32(ifile);
 header.unk05 = BE_read_uint32(ifile);
 header.unk06 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // debug header data
 if(debug) {
    dfile << "magic: 0x" << hex << header.magic << dec << endl;
    dfile << "signature: 0x" << hex << header.signature << dec << endl;
    dfile << "number of joints: 0x" << hex << header.joints << dec << endl;
    dfile << "number of meshes: 0x" << hex << header.meshes << dec << endl;
    dfile << "number of materials: 0x" << hex << header.materials << dec << endl;
    dfile << "number of vertices: 0x" << hex << header.vertices << dec << endl;
    dfile << "number of indices: 0x" << hex << header.indices << dec << endl;
    dfile << "number of triangles: 0x" << hex << header.triangles << dec << endl;
    dfile << "total vertex buffer size: 0x" << hex << header.vbtotalbytes << dec << endl;
    dfile << "unknown #1: 0x" << hex << header.unk01 << dec << endl;
    dfile << "number of unknown entries: 0x" << hex << header.unkentries << dec << endl;
    dfile << "number of joint maps: 0x" << hex << header.n_jointmaps << dec << endl;
    dfile << "offset to joint data: 0x" << hex << header.jointoffset << dec << endl;
    dfile << "offset to unknown entries: 0x" << hex << header.unkoffset << dec << endl;
    dfile << "offset to material data: 0x" << hex << header.material_offset << dec << endl;
    dfile << "offset to mesh information: 0x" << hex << header.meshinfo_offset << dec << endl;
    dfile << "vertex buffer offset: 0x" << hex << header.vertdata_offset << dec << endl;
    dfile << "index buffer offset: 0x" << hex << header.facedata_offset << dec << endl;
    dfile << "EOF offset: 0x" << hex << header.eofoffset << dec << endl;
    dfile << "padding: 0x" << hex << header.pad01 << dec << endl;
    dfile << "padding: 0x" << hex << header.pad02 << dec << endl;
    dfile << "padding: 0x" << hex << header.pad03 << dec << endl;
    dfile << "sphere_cx: " << header.sphere_cx << endl;
    dfile << "sphere_cy: " << header.sphere_cy << endl;
    dfile << "sphere_cz: " << header.sphere_cz << endl;
    dfile << "sphere_cr: " << header.sphere_cr << endl;
    dfile << "bbox_min_x: " << header.bbox_min_x << endl;
    dfile << "bbox_min_y: " << header.bbox_min_y << endl;
    dfile << "bbox_min_z: " << header.bbox_min_z << endl;
    dfile << "bbox_min_w: " << header.bbox_min_w << endl;
    dfile << "bbox_max_x: " << header.bbox_max_x << endl;
    dfile << "bbox_max_y: " << header.bbox_max_y << endl;
    dfile << "bbox_max_z: " << header.bbox_max_z << endl;
    dfile << "bbox_max_w: " << header.bbox_max_w << endl;
    dfile << "unknown #3: 0x" << hex << header.unk03 << dec << endl;
    dfile << "unknown #4: 0x" << hex << header.unk04 << dec << endl;
    dfile << "unknown #5: 0x" << hex << header.unk05 << dec << endl;
    dfile << "unknown #6: 0x" << hex << header.unk06 << dec << endl;
    dfile << endl;
   }

 return true;
}

bool modelizer::process002(void)
{
 // debug
 if(debug) {
    dfile << "---------" << endl;
    dfile << "BONE DATA" << endl;
    dfile << "---------" << endl;
    dfile << endl;
   }

 // contains no joints
 if(!header.jointoffset) return true;

 // move to offset
 ifile.seekg(header.jointoffset);
 if(ifile.fail()) return error("Seek failure.");

 // read joint information
 uint32 size = header.joints * (0x18 + 0x40 + 0x40);
 boost::shared_array<char> data(new char[size]);
 ifile.read(data.get(), size);
 if(ifile.fail()) return error("Read failure.");

 // save mesh information (DEBUG MODE ONLY)
 if(debug) {
    string fname = ChangeFileExtension(filename, "jbuffer");
    ofstream temp(fname.c_str(), ios::binary);
    if(!temp) return error("Failed to create JBUFFER file.");
    temp.write(data.get(), size);
    if(temp.fail()) return error("Write failure.");
   }

 // read 0x18 byte items
 binary_stream bs(data, size);
 for(uint32 i = 0; i < header.joints; i++)
    {
     RE6BONEINFO item;
     item.j01 = bs.BE_read_uint08();
     item.j02 = bs.BE_read_uint08();
     item.j03 = bs.BE_read_uint08();
     item.j04 = bs.BE_read_uint08();
     item.j05 = bs.BE_read_real32();
     item.j06 = bs.BE_read_real32();
     item.j07 = bs.BE_read_real32();
     item.j08 = bs.BE_read_real32();
     item.j09 = bs.BE_read_real32();
     bonedata.boneinfo.push_back(item);
     if(debug) {
        dfile << "joint #i = " << i << endl;
        dfile << "j01 = " << (uint16)item.j01 << endl;
        dfile << "j02 = " << (uint16)item.j02 << endl;
        dfile << "j03 = " << (uint16)item.j03 << endl;
        dfile << "j04 = " << (uint16)item.j04 << endl;
        dfile << "j05 = " << item.j05 << endl;
        dfile << "j06 = " << item.j06 << endl;
        dfile << "j07 = " << item.j07 << endl;
        dfile << "j08 = " << item.j08 << endl;
        dfile << "j09 = " << item.j09 << endl;
        dfile << endl;
       }
    }

 // read 0x40-byte matrices (position, orientation)
 for(uint32 i = 0; i < header.joints; i++) {
     boost::shared_array<real32> temp(new real32[16]);
     bs.BE_read_array(&temp[0], 16);
     if(bs.fail()) return error("Stream read failure.");
     matrix4x4_transpose(&temp[0]);
     bonedata.m1.push_back(temp);
    }

 // read 0x40-byte matrices (scale, translation)
 for(uint32 i = 0; i < header.joints; i++) {
     boost::shared_array<real32> temp(new real32[16]);
     bs.BE_read_array(&temp[0], 16);
     if(bs.fail()) return error("Stream read failure.");
     matrix4x4_transpose(&temp[0]);
     bonedata.m2.push_back(temp);
    }

 // move to joint offset
 ifile.seekg(header.jointoffset);
 if(ifile.fail()) return error("Seek failure.");

 // initialize skeleton
 AMC_SKELETON skel;
 skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX;
 skel.name = "skeleton";

 // read 0x18 byte items
 deque<RE6BONEINFO> jinfo;
 for(uint32 i = 0; i < header.joints; i++)
    {
     RE6BONEINFO item;
     item.j01 = BE_read_uint08(ifile); // 0x01
     item.j02 = BE_read_uint08(ifile); // 0x02
     item.j03 = BE_read_uint08(ifile); // 0x03 (if i is L_ankle, j03 is R_ankle)
     item.j04 = BE_read_uint08(ifile); // 0x04
     item.j05 = BE_read_real32(ifile);
     item.j06 = BE_read_real32(ifile);
     item.j07 = BE_read_real32(ifile);
     item.j08 = BE_read_real32(ifile);
     item.j09 = BE_read_real32(ifile);
     jinfo.push_back(item);
     if(debug) {
        dfile << "joint #i = " << i << endl;
        dfile << "j01 = " << item.j01 << endl;
        dfile << "j02 = " << item.j02 << endl;
        dfile << "j03 = " << item.j03 << endl;
        dfile << "j04 = " << item.j04 << endl;
        dfile << "j05 = " << item.j05 << endl;
        dfile << "j06 = " << item.j06 << endl;
        dfile << "j07 = " << item.j07 << endl;
        dfile << "j08 = " << item.j08 << endl;
        dfile << "j09 = " << item.j09 << endl;
        dfile << endl;
       }
    }

 // read 0x40-byte matrices (position, orientation)
 boost::shared_array<boost::shared_array<real32>> mat1(new boost::shared_array<real32>[header.joints]);
 for(uint32 i = 0; i < header.joints; i++) {
     mat1[i].reset(new real32[16]);
     if(!BE_read_array(ifile, &mat1[i][0], 16)) return error("Read failure.");
     matrix4x4_transpose(&mat1[i][0]);
    }

 // read 0x40-byte matrices (scale, translation)
 boost::shared_array<boost::shared_array<real32>> mat2(new boost::shared_array<real32>[header.joints]);
 for(uint32 i = 0; i < header.joints; i++) {
     mat2[i].reset(new real32[16]);
     if(!BE_read_array(ifile, &mat2[i][0], 16)) return error("Read failure.");
     matrix4x4_transpose(&mat2[i][0]);
    }

 // skip past unknown 0x100 bytes of data
 ifile.seekg(0x100, ios::cur);
 if(ifile.fail()) return error("Seek failure.");

 // read list of joint maps
 for(uint32 i = 0; i < header.n_jointmaps; i++)
    {
     // skip past 0x00
     while(ifile.peek() == 0x00) {
           ifile.seekg(1, ios::cur);
           if(ifile.fail()) return error("Seek failure.");
          }

     // read size of joint map
     uint16 size = BE_read_uint08(ifile);
     if(ifile.fail()) return error("Read failure.");
     if(size == 0) return error("The size of a joint map cannot be zero.");

     // read values
     boost::shared_array<uint16> jointmap(new uint16[size]);
     for(uint32 j = 0; j < size; j++) {
         jointmap[j] = BE_read_uint08(ifile);
         if(ifile.fail()) return error("Read failure.");
        }

     // set joint map
     bonedata.jointmaplist.push_back(jointmap);
    }

 if(header.n_jointmaps) cout << " MODEL HAS JOINTMAPS" << endl;

 // process 0x40 matrices
 for(uint32 i = 0; i < header.joints; i++)
    {
     // copy the two transformation matrices
     using namespace cs::math;
     matrix4x4<binary32> m1(mat1[i].get());
     matrix4x4<binary32> m2(mat2[i].get());
     // cout << "m1 = " << m1 << endl;
     // cout << "m2 = " << m2 << endl;
     // cout << endl;

     binary32 scale_x = m1(0,0)/m2(0,0);
     binary32 scale_y = m1(1,1)/m2(1,1);
     binary32 scale_z = m1(2,2)/m2(2,2);
     binary32 trans_x = m2(0,3);
     binary32 trans_y = m2(1,3);
     binary32 trans_z = m2(2,3);

     // m2 * T = m1
     // three equations and three unknowns can be solved for by cramer's rule
     // [r00 r01 r02 0] [ 1  0  0 tx] = [m00 m01 m02 m03]
     // [r10 r11 r12 0] [ 0  1  0 ty] = [m10 m11 m12 m13]
     // [r20 r21 r22 0] [ 0  0  1 tz] = [m20 m21 m22 m23]
     // [  0   0   0 1] [ 0  0  0  1] = [m30 m31 m32 m33]

     // find joint position in absolute coordinates using cramer's rule
     binary32 A[9] = {
      m2(0,0), m2(0,1), m2(0,2),
      m2(1,0), m2(1,1), m2(1,2),
      m2(2,0), m2(2,1), m2(2,2),
     };
     binary32 b[3] = {
      -m2(0,3),
      -m2(1,3),
      -m2(2,3),
     };
     binary32 x[3] = { 0.0f, 0.0f, 0.0f };
     cramer_3x3(A, b, x);

     // create joint name
     stringstream ss;
     ss << "joint_" << setfill('0') << setw(3) << i;

     // create and insert joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.parent = (jinfo[i].j02 == 0xFF ? AMC_INVALID_JOINT : jinfo[i].j02);
     joint.m_rel[0x0] = m1[0x0]; // mat1[i][0x0];
     joint.m_rel[0x1] = m1[0x1]; // mat1[i][0x1];
     joint.m_rel[0x2] = m1[0x2]; // mat1[i][0x2];
     joint.m_rel[0x3] = m1[0x3]; // mat1[i][0x3];
     joint.m_rel[0x4] = m1[0x4]; // mat1[i][0x4];
     joint.m_rel[0x5] = m1[0x5]; // mat1[i][0x5];
     joint.m_rel[0x6] = m1[0x6]; // mat1[i][0x6];
     joint.m_rel[0x7] = m1[0x7]; // mat1[i][0x7];
     joint.m_rel[0x8] = m1[0x8]; // mat1[i][0x8];
     joint.m_rel[0x9] = m1[0x9]; // mat1[i][0x9];
     joint.m_rel[0xA] = m1[0xA]; // mat1[i][0xA];
     joint.m_rel[0xB] = m1[0xB]; // mat1[i][0xB];
     joint.m_rel[0xC] = m1[0xC]; // mat1[i][0xC];
     joint.m_rel[0xD] = m1[0xD]; // mat1[i][0xD];
     joint.m_rel[0xE] = m1[0xE]; // mat1[i][0xE];
     joint.m_rel[0xF] = m1[0xF]; // mat1[i][0xF];
     joint.p_rel[0] = x[0]; // m2(0,3)*scale_x; // scale_x*m1(0,3); // mat1[i][0x3];
     joint.p_rel[1] = x[1]; // m2(1,3)*scale_y; // scale_y*m1(1,3); // mat1[i][0x7];
     joint.p_rel[2] = x[2]; // m2(2,3)*scale_z; // scale_z*m1(2,3); // mat1[i][0xB];
     joint.p_rel[3] = 1.0f;
     InsertJoint(skel, joint);
    }

 // save skeleton
 amc.skllist.push_back(skel);
 return true;
}

bool modelizer::process003(void)
{
 // debug
 if(debug) {
    dfile << "---------" << endl;
    dfile << "MESH DATA" << endl;
    dfile << "---------" << endl;
    dfile << endl;
   }

 // contains no meshes
 if(!header.meshinfo_offset) return true;

 // move to offset
 ifile.seekg(header.meshinfo_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read mesh information
 uint32 size = header.meshes * 0x38;
 boost::shared_array<char> data(new char[size]);
 ifile.read(data.get(), size);
 if(ifile.fail()) return error("Read failure.");

 // save mesh information (DEBUG MODE ONLY)
 if(debug) {
    string fname = ChangeFileExtension(filename, "mbuffer");
    ofstream temp(fname.c_str(), ios::binary);
    if(!temp) return error("Failed to create MBUFFER file.");
    temp.write(data.get(), size);
    if(temp.fail()) return error("Write failure.");
   }

 // read mesh information
 binary_stream bs(data, size);
 for(uint32 i = 0; i < header.meshes; i++) {
     RE6MESHINFO info;
     info.meshtype      = bs.BE_read_uint16();
     info.vertices      = bs.BE_read_uint16();
     info.unkbyteflag1  = bs.BE_read_uint08();
     info.unkbyteflag2  = bs.BE_read_uint08();
     info.material      = bs.BE_read_uint08();
     info.lodvalue      = bs.BE_read_uint08();
     info.unkbyteflag3  = bs.BE_read_uint08();
     info.unkbyteflag4  = bs.BE_read_uint08();
     info.vertexsize    = bs.BE_read_uint08();
     info.unkbyteflag5  = bs.BE_read_uint08();
     info.vertexindex1  = bs.BE_read_uint32();
     info.vertexoffset  = bs.BE_read_uint32();
     info.vertexformat1 = bs.BE_read_uint16();
     info.vertexformat2 = bs.BE_read_uint16();
     info.startindex    = bs.BE_read_uint32();
     info.indices       = bs.BE_read_uint32();
     info.vertexindex2  = bs.BE_read_uint32();
     info.jointmapindex = bs.BE_read_uint08();
     info.weightmaps    = bs.BE_read_uint08();
     info.unkflag2      = bs.BE_read_uint08();
     info.unkflag3      = bs.BE_read_uint08();
     info.min_index     = bs.BE_read_uint16();
     info.max_index     = bs.BE_read_uint16();
     info.unkshort1     = bs.BE_read_uint16();
     info.unkshort2     = bs.BE_read_uint16();
     info.zero1         = bs.BE_read_uint32();
     info.zero2         = bs.BE_read_uint32();
     meshdata.meshinfo.push_back(info);
    }

 if(debug) {
    for(uint32 i = 0; i < header.meshes; i++) {
        const RE6MESHINFO& info = meshdata.meshinfo[i];
        dfile << "MESHINFO #" << i << dec << endl;
        dfile << "meshtype = 0x" << hex << info.meshtype << dec << endl;
        dfile << "vertices = 0x" << hex << info.vertices << dec << endl;
        dfile << "unkbyteflag1 = 0x" << hex << (uint16)(info.unkbyteflag1) << dec << endl;
        dfile << "unkbyteflag2 = 0x" << hex << (uint16)(info.unkbyteflag2) << dec << endl;
        dfile << "material = 0x" << hex << (uint16)(info.material) << dec << endl;
        dfile << "lodvalue = 0x" << hex << (uint16)(info.lodvalue) << dec << endl;
        dfile << "unkbyteflag3 = 0x" << hex << (uint16)(info.unkbyteflag3) << dec << endl;
        dfile << "unkbyteflag4 = 0x" << hex << (uint16)(info.unkbyteflag4) << dec << endl;
        dfile << "vertexsize = 0x" << hex << (uint16)(info.vertexsize) << dec << endl;
        dfile << "unkbyteflag5 = 0x" << hex << (uint16)(info.unkbyteflag5) << dec << endl;
        dfile << "vertexindex1 = 0x" << hex << info.vertexindex1 << dec << endl;
        dfile << "vertexoffset = 0x" << hex << info.vertexoffset << dec << endl;
        dfile << "vertexformat1 = 0x" << hex << info.vertexformat1 << dec << endl;
        dfile << "vertexformat2 = 0x" << hex << info.vertexformat2 << dec << endl;
        dfile << "startindex = 0x" << hex << info.startindex << dec << endl;
        dfile << "indices = 0x" << hex << info.indices << dec << endl;
        dfile << "vertexindex2 = 0x" << hex << info.vertexindex2 << dec << endl;
        dfile << "jointmapindex = 0x" << hex << (uint16)info.jointmapindex << dec << endl;
        dfile << "weightmaps = 0x" << hex << (uint16)info.weightmaps << dec << endl;
        dfile << "unkflag2 = 0x" << hex << (uint16)info.unkflag2 << dec << endl;
        dfile << "unkflag3 = 0x" << hex << (uint16)info.unkflag3 << dec << endl;
        dfile << "min_index = 0x" << hex << info.min_index << dec << endl;
        dfile << "max_index = 0x" << hex << info.max_index << dec << endl;
        dfile << "unkshort1 = 0x" << hex << info.unkshort1 << dec << endl;
        dfile << "unkshort2 = 0x" << hex << info.unkshort2 << dec << endl;
        dfile << "zero1 = 0x" << hex << info.zero1 << dec << endl;
        dfile << "zero2 = 0x" << hex << info.zero2 << dec << endl;
        dfile << endl;
       }
    dfile << endl;
   }

 // seek vertex buffer offset
 ifile.seekg(header.vertdata_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vertex buffer data
 uint32 vbsize = header.vbtotalbytes;
 boost::shared_array<char> vbdata(new char[vbsize]);
 ifile.read(vbdata.get(), vbsize);
 if(ifile.fail()) return error("Read failure.");

 // seek index buffer offset
 ifile.seekg(header.facedata_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read index buffer data
 boost::shared_array<uint16> ibdata(new uint16[header.indices]);
 if(!BE_read_array(ifile, ibdata.get(), header.indices)) return error("Read failure.");

 // set buffers
 meshdata.vbuffer = vbdata;
 meshdata.ibuffer = ibdata;

 // save vertex buffer (DEBUG MODE ONLY)
 // if(debug) {
 //    string fname = ChangeFileExtension(filename, "vbuffer");
 //    ofstream temp(fname.c_str(), ios::binary);
 //    if(!temp) return error("Failed to create VBUFFER file.");
 //    temp.write(vbdata.get(), vbsize);
 //    if(temp.fail()) return error("Write failure.");
 //   }

 // save index buffer (DEBUG MODE ONLY)
 if(debug) {
    string fname = ChangeFileExtension(filename, "ibuffer");
    ofstream temp(fname.c_str(), ios::binary);
    if(!temp) return error("Failed to create IBUFFER file.");
    temp.write((const char*)ibdata.get(), header.indices * sizeof(uint16));
    if(temp.fail()) return error("Write failure.");
   }

 // save individual vertex buffers (DEBUG MODE ONLY)
 binary_stream vs(meshdata.vbuffer, vbsize);
 for(uint32 i = 0; i < meshdata.meshinfo.size(); i++)
    {
     // vertex size and type
     const RE6MESHINFO& item = meshdata.meshinfo[i];
     uint16 vertex_elem = item.vertices;
     uint16 vertex_size = item.vertexsize;

     // DEBUG: save vertex buffer
     if(debug) {
        stringstream sv;
        sv << pathname.c_str() << shrtname.c_str() << ".vb" << setfill('0') << setw(3) << i;
        ofstream temp(sv.str().c_str(), ios::binary);
        if(!temp) return error("Failed to create VBUFFER file.");
        uint32 datasize = vertex_size * vertex_elem;
        temp.write(vs.c_pos(), datasize);
        vs.move(datasize);
        if(temp.fail()) return error("Write failure.");
       }
    }

 return true;
}

bool modelizer::process004(void)
{
 // debug
 if(debug) {
    dfile << "-------------" << endl;
    dfile << "MATERIAL DATA" << endl;
    dfile << "-------------" << endl;
    dfile << endl;
   }

 // contains no materials
 if(!header.material_offset) return true;

 // move to offset
 ifile.seekg(header.material_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read material identifiers used by this model
 for(uint32 i = 0; i < header.materials; i++) {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     mtrldata.idlist.push_back(item);
     if(debug) dfile << "material used[" << i << "] = " << item << endl;
    }
 if(debug) dfile << endl;

 // filename to open depends on a few things
 stringstream nametest;
 nametest << GetPathnameFromFilename(filename);
 nametest << GetShortFilenameWithoutExtension(filename);
 nametest << "_0";
 nametest << ".mrl";

 // open MRL file
 string mfname = (FileExists(nametest.str()) ? nametest.str() : ChangeFileExtension(filename, "mrl"));
 ifstream mfile(mfname.c_str(), ios::binary);
 if(!mfile) return true; // special case: no MRL file (FBIKHuman.mod)

 // read magic number
 uint32 magic = BE_read_uint32(mfile);
 if(magic != 0x4C524D) return error("Invalid MRL file.");

 // read 0x21
 uint32 hunk1 = BE_read_uint32(mfile);
 if(hunk1 != 0x21) return error("Invalid MRL file; expecting 0x21.");

 // read number of materials and textures
 uint32 n_mats = BE_read_uint32(mfile);
 uint32 n_texs = BE_read_uint32(mfile);

 // no materials or textures
 if(!n_mats || !n_texs) {
    // TODO: special processing
    return true;
   }

 // read 0xA14BE3B6
 uint32 hunk2 = BE_read_uint32(mfile);
 if(hunk2 != 0xA14BE3B6) return error("Invalid MRL file; expecting 0xA14BE3B6.");

 // read data offsets
 uint32 filelist_offset = BE_read_uint32(mfile);
 uint32 mtrllist_offset = BE_read_uint32(mfile);
 if(mfile.fail()) return error("Read failure.");

 // move to file listing offset
 mfile.seekg(filelist_offset);
 if(mfile.fail()) return error("Seek failure.");

 // read textures
 for(uint32 i = 0; i < n_texs; i++)
    {
     // read texture information
     uint32 p1 = BE_read_uint32(mfile); // 0x241F5DEB
     uint32 p2 = BE_read_uint32(mfile); // 0x00000000
     uint32 p3 = BE_read_uint32(mfile); // 0x00000000
     if(!(p1 == 0x241F5DEB || p1 == 0x7808EA10)) return error("Invalid MRL file; expecting 0x241F5DEB or 0x7808EA10.");

     // read filename
     char p4[0x40];
     mfile.read(&p4[0], 0x40);
     if(mfile.fail()) return error("Read failure.");
     if(debug) dfile << "texture[" << i << "] = " << p4 << ".dds" << endl;

     // insert filename
     string temp = p4;
     temp += ".dds";
     mtrldata.filelist.push_back(temp);

     // insert filename into model container
     AMC_IMAGEFILE imgfile;
     imgfile.filename = GetShortFilename(temp);
     amc.iflist.push_back(imgfile);
    }
 if(debug) dfile << endl;

 // move to material list offset
 mfile.seekg(mtrllist_offset);
 if(mfile.fail()) return error("Seek failure.");

 // read material list
 for(uint32 i = 0; i < n_mats; i++)
    {
     RE6MTRLINFO info;
     info.signature = BE_read_uint32(mfile); // 0x5FB0EBE4
     info.id = BE_read_uint32(mfile); // id
     info.size1 = BE_read_uint32(mfile); // size of data
     info.unk01 = BE_read_uint32(mfile);
     info.unk02 = BE_read_uint32(mfile);
     info.unk03 = BE_read_uint32(mfile);
     info.entryinfo = BE_read_uint16(mfile); // XXXY (number of entries/size of entry)
     info.unk04 = BE_read_uint16(mfile);
     info.unk05 = BE_read_uint16(mfile);
     info.unk06 = BE_read_uint16(mfile);
     info.zero1 = BE_read_uint32(mfile); // zero #1
     info.zero2 = BE_read_uint32(mfile); // zero #2
     info.zero3 = BE_read_uint32(mfile); // zero #3
     info.zero4 = BE_read_uint32(mfile); // zero #4
     info.size2 = BE_read_uint32(mfile); // size of data at bottom of file
     info.offset1 = BE_read_uint32(mfile); // offset to texture association data
     info.offset2 = BE_read_uint32(mfile); // offset to data at bottom of file
     mtrldata.mtrlinfo.push_back(info);
    }

 // read secondary material information
 for(uint32 i = 0; i < n_mats; i++)
    {
     // extract number and size of association entries
     RE6MTRLINFO& info = mtrldata.mtrlinfo[i];
     uint16 n_entries = (info.entryinfo >> 4);
     uint16 entrysize = (info.entryinfo & 0x000F);

     // move to texture association data
     mfile.seekg(info.offset1);
     if(mfile.fail()) return error("Seek failure.");

     // read entries
     // find the diffuse map entry and save it
     for(uint32 j = 0; j < n_entries; j++)
        {
         if(entrysize == 0x0C)
           {
            uint32 type = BE_read_uint32(mfile);
            uint32 texture = BE_read_uint32(mfile);
            uint16 unk01 = BE_read_uint16(mfile);
            uint16 unk02 = BE_read_uint16(mfile);

            // _BM
            if(type == 0x3DEDE347) {
               typedef std::map<uint32, uint32>::value_type value_type;
               if(texture == 0) mtrldata.mdiffuse.insert(value_type(info.id, AMC_INVALID_TEXTURE));
               else if(texture > n_texs) mtrldata.mdiffuse.insert(value_type(info.id, AMC_INVALID_TEXTURE));
               else mtrldata.mdiffuse.insert(value_type(info.id, texture - 1));
              }
            // _NM
            else if(type == 0x3DEDE34A) {
               // do nothing for now
              }
            // _MM
            else if(type == 0x3DEDE35A) {
               // do nothing for now
              }
            // _LM
            // else if(type == ???) {
               // do nothing for now
            //   }
            // _VTF
            else if(type == 0x3DEDE366) {
               // do nothing for now
              }
            // _MVTF
            else if(type == 0x3DEDE367) {
               // do nothing for now
              }
           }
         else
            return error("Invalid material entry size. Expecting 0x0C.");
        }

     // material name
     stringstream ss;
     ss << "material_0x" << setfill('0') << setw(2) << hex << i << dec;

     // create surface material
     AMC_SURFMAT surfmat;
     surfmat.name == ss.str();
     surfmat.twoside = 0;
     surfmat.basemap = AMC_INVALID_TEXTURE;
     surfmat.specmap = AMC_INVALID_TEXTURE;
     surfmat.tranmap = AMC_INVALID_TEXTURE;
     surfmat.bumpmap = AMC_INVALID_TEXTURE;
     surfmat.normmap = AMC_INVALID_TEXTURE;

     // find diffuse map index
     auto iter = mtrldata.mdiffuse.find(info.id);
     if(iter != mtrldata.mdiffuse.end()) surfmat.basemap = iter->second;

     // insert surface material
     amc.surfmats.push_back(surfmat);
    }

 return true;
}

bool modelizer::process005(void)
{
 // debug
 if(debug) {
    dfile << "-----------" << endl;
    dfile << "VERTEX DATA" << endl;
    dfile << "-----------" << endl;
    dfile << endl;
   }

 // create binary stream to read data from vertex buffer
 if(!header.vertdata_offset || !header.vbtotalbytes) return true;
 binary_stream vbstream(meshdata.vbuffer, header.vbtotalbytes);

 // initialize bounding box variables
 real32 global_min_x, global_min_y, global_min_z;
 real32 global_max_x, global_max_y, global_max_z;
 global_min_x = global_min_y = global_min_z = std::numeric_limits<real32>::max();
 global_max_x = global_max_y = global_max_z = std::numeric_limits<real32>::min();

 // initialize weight map base index
 wmap_base = 0;

 // process vertex buffers
 for(uint32 i = 0; i < meshdata.meshinfo.size(); i++)
    {
     // vertex size and type
     const RE6MESHINFO& meshinfo = meshdata.meshinfo[i];
     uint16 mesh_type = meshinfo.meshtype;
     uint16 vertex_elem = meshinfo.vertices;
     uint16 vertex_size = meshinfo.vertexsize;

     // reset weight map base index
     if(meshinfo.vertexindex1 == 0)
        wmap_base = 0;

     // create vertex buffer
     AMC_VTXBUFFER vbuffer;
     vbuffer.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
     vbuffer.name = "default";
     vbuffer.elem = vertex_elem;
     vbuffer.data.reset(new AMC_VERTEX[vbuffer.elem]);

     // process vertex data
     if(vertex_size == 0x0C) {
        if(!processVBx0C(vbstream, meshinfo, vbuffer))
           return false;
       }
     else if(vertex_size == 0x10) {
        if(!processVBx10(vbstream, meshinfo, vbuffer))
           return false;
       }
     else if(vertex_size == 0x14) {
        if(!processVBx14(vbstream, meshinfo, vbuffer))
           return false;
       }
     else if(vertex_size == 0x18) {
        if(!processVBx18(vbstream, meshinfo, vbuffer))
           return false;
       }
     else if(vertex_size == 0x1C) {
        if(!processVBx1C(vbstream, meshinfo, vbuffer))
           return false;
       }
     else if(vertex_size == 0x20) {
        if(!processVBx20(vbstream, meshinfo, vbuffer))
           return false;
       }
     else if(vertex_size == 0x24) {
        if(!processVBx24(vbstream, meshinfo, vbuffer))
           return false;
       }
     else if(vertex_size == 0x28) {
        if(!processVBx28(vbstream, meshinfo, vbuffer))
           return false;
       }
     else if(vertex_size == 0x30) {
        if(!processVBx30(vbstream, meshinfo, vbuffer))
           return false;
       }
     else if(vertex_size == 0x40) {
        if(!processVBx40(vbstream, meshinfo, vbuffer))
           return false;
       }
     else {
        stringstream ss;
        ss << "Unknown vertex size 0x" << setfill('0') << setw(2) << hex << vertex_size << dec << ".";
        return error(ss.str().c_str());
       }

     // adjust weight map base index
     wmap_base += meshinfo.weightmaps;

     // bounding box variables
     real32 min_x, min_y, min_z;
     real32 max_x, max_y, max_z;
     min_x = min_y = min_z = std::numeric_limits<real32>::max();
     max_x = max_y = max_z = std::numeric_limits<real32>::min();

     // find bounding box of vertex buffer
     for(uint32 j = 0; j < vbuffer.elem; j++) {
         if(vbuffer.data[j].vx < min_x) min_x = vbuffer.data[j].vx;
         if(vbuffer.data[j].vy < min_y) min_y = vbuffer.data[j].vy;
         if(vbuffer.data[j].vz < min_z) min_z = vbuffer.data[j].vz;
         if(max_x < vbuffer.data[j].vx) max_x = vbuffer.data[j].vx;
         if(max_y < vbuffer.data[j].vy) max_y = vbuffer.data[j].vy;
         if(max_z < vbuffer.data[j].vz) max_z = vbuffer.data[j].vz;
        }

     // global adjustment
     if(min_x < global_min_x) global_min_x = min_x;
     if(min_y < global_min_y) global_min_y = min_y;
     if(min_z < global_min_z) global_min_z = min_z;
     if(global_max_x < max_x) global_max_x = max_x;
     if(global_max_y < max_y) global_max_y = max_y;
     if(global_max_z < max_z) global_max_z = max_z;

     // insert vertex buffer
     amc.vblist.push_back(vbuffer);
    }

 // bounding box adjustment only if there are joints
 // if(header.joints)
 //   {
 //    // bounding box dimensions of scaled model
 //    real32 box_dx = header.bbox_max_x - header.bbox_min_x;
 //    real32 box_dy = header.bbox_max_y - header.bbox_min_y;
 //    real32 box_dz = header.bbox_max_z - header.bbox_min_z;
 //    
 //    // bounding box dimensions of actual model
 //    real32 mdl_dx = global_max_x - global_min_x;
 //    real32 mdl_dy = global_max_y - global_min_y;
 //    real32 mdl_dz = global_max_z - global_min_z;
 //    
 //    // bounding box scaling factor
 //    real32 scale_dx = box_dx/mdl_dx;
 //    real32 scale_dy = box_dy/mdl_dy;
 //    real32 scale_dz = box_dz/mdl_dz;
 // 
 //    cout << "scale = 0x" << hex << interpret_as_uint32(scale_dx) << dec << endl;
 //    cout << "scale = 0x" << hex << interpret_as_uint32(scale_dy) << dec << endl;
 //    cout << "scale = 0x" << hex << interpret_as_uint32(scale_dz) << dec << endl;
 // 
 //    // mesh adjustment
 //    for(size_t i = 0; i < amc.vblist.size(); i++) {
 //        for(size_t j = 0; j < amc.vblist[i].elem; j++) {
 //            amc.vblist[i].data[j].vx *= scale_dx;
 //            amc.vblist[i].data[j].vy *= scale_dy;
 //            amc.vblist[i].data[j].vz *= scale_dz;
 //            amc.vblist[i].data[j].vx += (header.bbox_min_x - global_min_x * scale_dx);
 //            amc.vblist[i].data[j].vy += (header.bbox_min_y - global_min_y * scale_dy);
 //            amc.vblist[i].data[j].vz += (header.bbox_min_z - global_min_z * scale_dz);
 //           }
 //       }
 //   }

 return true;
}

bool modelizer::process006(void)
{
 // debug
 if(debug) {
    dfile << "----------" << endl;
    dfile << "INDEX DATA" << endl;
    dfile << "----------" << endl;
    dfile << endl;
   }

 // process index buffers
 for(uint32 i = 0; i < header.meshes; i++)
    {
     // container to store triangles
     deque<RE6TRIANGLE> triangles;

     // vertex size and type
     const RE6MESHINFO& mi = meshdata.meshinfo[i];
     uint16 vertex_elem = mi.vertices;
     uint16 vertex_size = mi.vertexsize;

     // vertex bases
     uint32 vertex_base1 = mi.vertexindex1;
     uint32 vertex_base2 = mi.vertexindex2;
     uint32 vertex_base_index = vertex_base1 + vertex_base2;

     // index buffer properties
     boost::shared_array<uint16> ib_data = meshdata.ibuffer;
     uint32 ib_base = mi.startindex;
     uint32 ib_elem = mi.indices;
     uint32 ib_min_index = mi.min_index;
     uint32 ib_max_index = mi.max_index;

     // create mesh and material names
     stringstream ss;
     ss << "msh_" << setfill('0') << setw(3) << i << "_";
     ss << "opt_" << setfill('0') << setw(4) << hex << mi.meshtype << dec << "_";
     ss << "lod_" << setfill('0') << setw(2) << hex << (uint16)mi.lodvalue << dec;

     // IMPORTANT! SOMETIMES THERE ARE NO FACES!
     if(ib_elem == 0) {
        // RE6TRIANGLE item;
        // item.a = 0;
        // item.b = 0;
        // item.c = 0;
        // triangles.push_back(item);
       }

     // create a deque to store triangles
     deque<uint32> trilist;
     for(uint32 j = 0; j < ib_elem; j++)
        {
         // insert index into triangle list
         uint32 index = ib_base + j;
         if(ib_data[index] != 0xFFFF) trilist.push_back(ib_data[index]);

         // now test if processing is required
         if((ib_data[index] == 0xFFFF) || (j == (ib_elem - 1)))
           {
            // must have at least three elements
            if(trilist.size() < 3) {
               stringstream ss;
               ss << "Expecting at least three tristrip indices. ";
               ss << "Stopping at index 0x" << hex << index << dec << " ";
               ss << "because ";
               if(ib_data[index] == 0xFFFF) ss << "we hit a 0xFFFF.";
               else if(j == (mi.indices - 1)) ss << "came to the end of the index buffer.";
               return error(ss.str().c_str());
              }

            // process first triangle
            if((trilist[0] < ib_min_index) || (trilist[0] > ib_max_index)) return error("Invalid index.");
            if((trilist[1] < ib_min_index) || (trilist[1] > ib_max_index)) return error("Invalid index.");
            if((trilist[2] < ib_min_index) || (trilist[2] > ib_max_index)) return error("Invalid index.");
            uint32 a = trilist[0] - ib_min_index + (ib_min_index - vertex_base1); // add difference
            uint32 b = trilist[1] - ib_min_index + (ib_min_index - vertex_base1); // add difference
            uint32 c = trilist[2] - ib_min_index + (ib_min_index - vertex_base1); // add difference

            // save first triangle
            RE6TRIANGLE item;
            item.a = a;
            item.b = b;
            item.c = c;
            triangles.push_back(item);

            // process remaining triangles
            for(uint32 k = 3; k < trilist.size(); k++)
               {
                a = b;
                b = c;
                c = trilist[k] - mi.min_index + (mi.min_index - vertex_base1);

                if(k % 2) {
                   RE6TRIANGLE item;
                   item.a = a;
                   item.b = c;
                   item.c = b;
                   triangles.push_back(item);
                  }
                else {
                   RE6TRIANGLE item;
                   item.a = a;
                   item.b = b;
                   item.c = c;
                   triangles.push_back(item);
                  }
               }

            // clear deque
            trilist.erase(trilist.begin(), trilist.end());
            trilist.clear();
           }
        }

     // initialize index buffer
     AMC_IDXBUFFER ibuffer;
     ibuffer.format = AMC_UINT16;
     ibuffer.type = AMC_TRIANGLES;
     ibuffer.name = ss.str().c_str();
     ibuffer.elem = 3 * triangles.size();
     ibuffer.data.reset(new char[ibuffer.elem * sizeof(uint16)]);

     // transfer triangle indices
     for(uint32 j = 0; j < triangles.size(); j++) {
         uint16* ptr = reinterpret_cast<uint16*>(ibuffer.data.get());
         ptr[3*j + 0] = triangles[j].a;
         ptr[3*j + 1] = triangles[j].b;
         ptr[3*j + 2] = triangles[j].c;
        }

     // save index buffer
     amc.iblist.push_back(ibuffer);

     // find material index using ID and assign correct surface material
     uint32 mtrlID = mtrldata.idlist[mi.material];
     uint32 surfmt = AMC_INVALID_SURFMAT;
     for(uint32 j = 0; j < mtrldata.mtrlinfo.size(); j++) {
         if(mtrlID == mtrldata.mtrlinfo[j].id) {
            surfmt = j;
            break;
           }
        }

     // construct surface name from material, group ID and LOD
     stringstream matname;
     matname << "mat_0x" << hex << setfill('0') << setw(2) << (uint16)mi.material << dec << "_";
     matname << "msh_0x" << hex << setfill('0') << setw(2) << (uint16)mi.meshtype << dec << "_";
     matname << "lod_0x" << hex << setfill('0') << setw(2) << (uint16)mi.lodvalue;

     // initialize surface
     AMC_SURFACE surf;
     surf.name = ss.str().c_str(); // matname.str().c_str();
     surf.surfmat = surfmt;
     AMC_REFERENCE ref;
     ref.vb_index = i;
     ref.vb_start = 0;
     ref.vb_width = amc.vblist[i].elem;
     ref.ib_index = i;
     ref.ib_start = 0;
     ref.ib_width = amc.iblist[i].elem;
     surf.refs.push_back(ref);

     // save surface
     amc.surfaces.push_back(surf);
    }

 return true;
}

bool modelizer::processVBx0C(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0xB098 && mi.vertexformat2 == 0x3013) format = 1;
 else return error("Unknown vertex type for size 0x0C.");

 // format #1
 // STATE: POSITION OK
 // NOTES: For useless edge geometry.
 if(format == 1)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 vw = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = 0.0f;
        vb.data[j].tv = 0.0f;
       }
   }

 return true;
}

bool modelizer::processVBx10(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0xDB7D && mi.vertexformat2 == 0xA014) format = 1;
 else return error("Unknown vertex type for size 0x10.");

 // format #1
 // STATE: POSITION OK
 // NOTES: For useless edge geometry.
 if(format == 1)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 vw = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tu = bs.BE_read_real16(); // 0x0E (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x10 (UV channel)
    
        // save vertex
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

 return true;
}

bool modelizer::processVBx14(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0xA8FA && mi.vertexformat2 == 0xB018) format = 1;
 else if(mi.vertexformat1 == 0x0CB6 && mi.vertexformat2 == 0x8015) format = 2;
 else if(mi.vertexformat1 == 0xA7D7 && mi.vertexformat2 == 0xD036) format = 3;
 else return error("Unknown vertex type for size 0x14.");

 // format #1
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: Used by enemies and sm models. This format is for skeletal models who
 // have meshes with weights all set to one.
 if(format == 1)
   {
    vb.flags |= AMC_VERTEX_WEIGHTS;

    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        uint16 bf = bs.BE_read_uint08(); // 0x07
        uint16 bi = bs.BE_read_uint08(); // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x14 (UV channel)
    
        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

        // set weights
        vb.data[j].wi[0] = wmap_base + bi;
        vb.data[j].wv[0] = 1.0f;
       }
   }

 // format #2
 // STATE: POSITION OK
 // NOTES: For useless edge geometry.
 else if(format == 2)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 vw = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_sint16()/32767.0f; // 0x12 (UV channel)
        real32 tv = bs.BE_read_sint16()/32767.0f; // 0x14 (UV channel)
    
        // save vertex
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

 // format #3
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by sm, id, stages and effects.
 else if(format == 3)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12
        real32 tv = bs.BE_read_real16(); // 0x14
    
        // save vertex
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

 return true;
}

bool modelizer::processVBx18(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0x207D && mi.vertexformat2 == 0x6037) format = 1;
 else if(mi.vertexformat1 == 0xD829 && mi.vertexformat2 == 0x7028) format = 2;
 else if(mi.vertexformat1 == 0xD1A4 && mi.vertexformat2 == 0x7038) format = 3;
 else if(mi.vertexformat1 == 0xC66F && mi.vertexformat2 == 0xA03A) format = 4;
 else if(mi.vertexformat1 == 0xC31F && mi.vertexformat2 == 0x201C) format = 5;
 else if(mi.vertexformat1 == 0xCBF6 && mi.vertexformat2 == 0xC01A) format = 6;
 else if(mi.vertexformat1 == 0x667B && mi.vertexformat2 == 0x1019) format = 7;
 else return error("Unknown vertex type for size 0x18.");

 // format #1
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by sm, stages and effects.
 if(format == 1)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x16
        real32 tv = bs.BE_read_real16(); // 0x18
        bs.BE_read_uint08(); // 0x11
        bs.BE_read_uint08(); // 0x12
        bs.BE_read_uint08(); // 0x13
        bs.BE_read_uint08(); // 0x14

        // save vertex
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

 // format #2
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by enemies, sm, id and stages.
 else if(format == 2)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        bs.BE_read_uint08(); // 0x11
        bs.BE_read_uint08(); // 0x12
        bs.BE_read_uint08(); // 0x13
        bs.BE_read_uint08(); // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel)

        // save vertex
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

 // format #3
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by stages only.
 else if(format == 3)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12 (UV channel #1)
        real32 tv = bs.BE_read_real16(); // 0x14 (UV channel #1)
        bs.BE_read_real16(); // 0x16 (UV channel #2)
        bs.BE_read_real16(); // 0x18 (UV channel #2)

        // save vertex
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

 // format #4
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by stages and effects.
 else if(format == 4)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12
        real32 tv = bs.BE_read_real16(); // 0x14
        bs.BE_read_real16(); // 0x16 (UV channel repeat)
        bs.BE_read_real16(); // 0x18 (UV channel repeat)

        // save vertex
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

 // format #5
 // STATE: POSITION OK, NORMALS OK, UVS OK (EXCEPT FOR A FEW MODELS), WEIGHTS OK
 // NOTES: This one is a strange format. The UVs work on all models except
 // for Helena's DLC hands. This one also uses half-float blend indices. There
 // is also at most only two weights.
 else if(format == 5)
   {
    vb.flags |= AMC_VERTEX_WEIGHTS;

    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12
        real32 tv = bs.BE_read_real16(); // 0x14
        uint16 i1 = (uint16)bs.BE_read_real16(); // 0x16 (blend index in half-float, these are always scalars like, 0, 1, 2, 3...)
        uint16 i2 = (uint16)bs.BE_read_real16(); // 0x18 (blend index in half-float, these are always scalars like, 0, 1, 2, 3...)

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

        // set weights
        if(i1 == i2) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = 1.0f;
          }
        else {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = 1.0f - w1;
          }
       }
   }

 // format #6
 // NOTES: POSITION OK, NORMALS OK, UV OK, WEIGHTS OK
 // NOTES: The weights are always equal to one for rigid models like sm1468.
 else if(format == 6)
   {
    // joints and weights
    vb.flags |= AMC_VERTEX_WEIGHTS;

    // process vertices
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        uint08 bf = bs.BE_read_uint08(); // 0x07
        uint08 bi = bs.BE_read_uint08(); // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12
        real32 tv = bs.BE_read_real16(); // 0x14
        bs.BE_read_uint08(); // 0x15
        bs.BE_read_uint08(); // 0x16
        bs.BE_read_uint08(); // 0x17
        bs.BE_read_uint08(); // 0x18

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

        // set weights
        vb.data[j].wi[0] = wmap_base + bi;
        vb.data[j].wv[0] = 1.0f;
       }
   }

 // format #7
 // STATE: POSITION OK, NORMALS OK, UVS OK, LIGHTMAPS OK
 // NOTES: Used by sm models only.
 else if(format == 7)
   {
    // joints and weights
    vb.flags |= AMC_VERTEX_WEIGHTS;

    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        uint08 bf = bs.BE_read_uint08(); // 0x07
        uint08 bi = bs.BE_read_uint08(); // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12 (uv map)
        real32 tv = bs.BE_read_real16(); // 0x14 (uv map)
        real32 lu = bs.BE_read_real16(); // 0x16 (lightmap)
        real32 lv = bs.BE_read_real16(); // 0x18 (lightmap)

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

        // set weights
        vb.data[j].wi[0] = wmap_base + bi;
        vb.data[j].wv[0] = 1.0f;
       }
   }

 return true;
}

bool modelizer::processVBx1C(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0xA14E && mi.vertexformat2 == 0x003C) format = 1;
 else if(mi.vertexformat1 == 0x5E7F && mi.vertexformat2 == 0x202C) format = 2;
 else if(mi.vertexformat1 == 0x49B4 && mi.vertexformat2 == 0xF029) format = 3;
 else if(mi.vertexformat1 == 0x14D4 && mi.vertexformat2 == 0x0020) format = 4;
 else if(mi.vertexformat1 == 0xA320 && mi.vertexformat2 == 0xC016) format = 5;
 else if(mi.vertexformat1 == 0x0D9E && mi.vertexformat2 == 0x801D) format = 6;
 else if(mi.vertexformat1 == 0xA013 && mi.vertexformat2 == 0x501E) format = 7;
 else if(mi.vertexformat1 == 0x2082 && mi.vertexformat2 == 0xF03B) format = 8;
 else if(mi.vertexformat1 == 0xAFA6 && mi.vertexformat2 == 0x302D) format = 9;
 else return error("Unknown vertex type for size 0x1C.");

 // format #1
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by stages and effects.
 if(format == 1)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x14 (UV channel)
        real32 au = bs.BE_read_real16(); // 0x16 (alpha channel)
        real32 av = bs.BE_read_real16(); // 0x18 (alpha channel)
        bs.BE_read_uint08(); // 0x19 (0x6D)
        bs.BE_read_uint08(); // 0x1A (0x6D)
        bs.BE_read_uint08(); // 0x1B (0x6D)
        bs.BE_read_uint08(); // 0x1C (0xFF)

        // save vertex
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

 // format #2
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by items, stages and effects.
 else if(format == 2)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        bs.BE_read_uint08(); // 0x11
        bs.BE_read_uint08(); // 0x12
        bs.BE_read_uint08(); // 0x13
        bs.BE_read_uint08(); // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16
        real32 tv = bs.BE_read_real16(); // 0x18
        bs.BE_read_real16(); // 0x1A (UV repeat)
        bs.BE_read_real16(); // 0x1C (UV repeat)

        // save vertex
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

 // format #3
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by sm, stages and effects.
 else if(format == 3)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (((sint32)bs.BE_read_uint08()) - 127)/128.0f; // 0x0D
        real32 ny = (((sint32)bs.BE_read_uint08()) - 127)/128.0f; // 0x0E
        real32 nz = (((sint32)bs.BE_read_uint08()) - 127)/128.0f; // 0x0F
        real32 nw = (((sint32)bs.BE_read_uint08()) - 127)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16
        real32 tv = bs.BE_read_real16(); // 0x18
        real32 ux = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x19 (0x6D)
        real32 uy = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x1A (0x6D)
        real32 uz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x1B (0x6D)
        real32 uw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x1C (0xFF)

        // save vertex
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

 // format #4
 // STATE: POSITION OK, NORMAL OK, UV OK, WEIGHTS OK
 // NOTES: Used by enemies, players, sm and stages.
 else if(format == 4)
   {
    vb.flags |= AMC_VERTEX_WEIGHTS;

    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02 (x)
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04 (y)
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06 (z)
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08 (blend weights)
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        uint16 i1 = bs.BE_read_uint08(); // 0x11 (blend indices)
        uint16 i2 = bs.BE_read_uint08(); // 0x12 (blend indices)
        uint16 i3 = bs.BE_read_uint08(); // 0x13 (blend indices)
        uint16 i4 = bs.BE_read_uint08(); // 0x14 (blend indices)
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel)
        real32 w2 = bs.BE_read_real16(); // 0x1A (blend weights)
        real32 w3 = bs.BE_read_real16(); // 0x1C (blend weights)

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // number of weights in this vertex
        uint32 weights = 0;
        if(w2 == 0) weights = 1;
        else if(w3 == 0) weights = 2;
        else weights = 3;

        // blending
        if(weights > 0) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = w2;
          }
        if(weights > 2) {
           vb.data[j].wi[2] = wmap_base + i3;
           vb.data[j].wv[2] = w3;
          }
       }
   }

 // format #5 (STRANGE BUT IT WORKS)
 // STATE: POSITION OK, WEIGHTS OK
 // NOTES: For useless edge geometry. Does not have normals or UV coordinates.
 // There are eight blend pairs, with all weights summing to 255.
 else if(format == 5)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 vw = bs.BE_read_sint16()/32767.0f; // 0x08
        uint16 i1 = bs.BE_read_uint08(); // 0x09 (blend indices)
        uint16 i2 = bs.BE_read_uint08(); // 0x0A (blend indices)
        uint16 i3 = bs.BE_read_uint08(); // 0x0B (blend indices)
        uint16 i4 = bs.BE_read_uint08(); // 0x0C (blend indices)
        uint16 i5 = bs.BE_read_uint08(); // 0x0D (blend indices)
        uint16 i6 = bs.BE_read_uint08(); // 0x0E (blend indices)
        uint16 i7 = bs.BE_read_uint08(); // 0x0F (blend indices)
        uint16 i8 = bs.BE_read_uint08(); // 0x10 (blend indices)
        real32 w1 = bs.BE_read_uint08()/255.0f; // 0x11 (blend weights)
        real32 w2 = bs.BE_read_uint08()/255.0f; // 0x12 (blend weights)
        real32 w3 = bs.BE_read_uint08()/255.0f; // 0x13 (blend weights)
        real32 w4 = bs.BE_read_uint08()/255.0f; // 0x14 (blend weights)
        real32 w5 = bs.BE_read_uint08()/255.0f; // 0x15 (blend weights)
        real32 w6 = bs.BE_read_uint08()/255.0f; // 0x16 (blend weights)
        real32 w7 = bs.BE_read_uint08()/255.0f; // 0x17 (blend weights)
        real32 w8 = bs.BE_read_uint08()/255.0f; // 0x18 (blend weights)
        bs.BE_read_uint08(); // 0x19
        bs.BE_read_uint08(); // 0x1A
        bs.BE_read_uint08(); // 0x1B
        bs.BE_read_uint08(); // 0x1C

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = 0.0f;
        vb.data[j].ny = 1.0f;
        vb.data[j].nz = 0.0f;
        vb.data[j].tu = 0.0f;
        vb.data[j].tv = 0.0f;
       }
   }

 // format #6
 // NOTES: POSITION OK, NORMAL OK, UV OK, WEIGHTS OK
 // NOTES: This format is used in skeletal models. There are a maximum of two
 // blending pairs, with blend indices encoded as half-floats.
 else if(format == 6)
   {
    vb.flags |= AMC_VERTEX_WEIGHTS;

    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12 (UV channel #1)
        real32 tv = bs.BE_read_real16(); // 0x14 (UV channel #1)
        uint16 i1 = (uint16)bs.BE_read_real16(); // 0x16 (blend indices as half-floats)
        uint16 i2 = (uint16)bs.BE_read_real16(); // 0x18 (blend indices as half-floats)
        bs.BE_read_real16(); // 0x1A (UV channel #2)
        bs.BE_read_real16(); // 0x1C (UV channel #2)

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // number of weights in this vertex
        uint32 weights = 0;
        if(w1 == 1.0f) weights = 1;
        else weights = 2;

        // blending
        if(weights > 0) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = 1.0f - w1;
          }
       }
   }

 // format #7
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: This format is used in skeletal models. There are a maximum of two
 // blending pairs, with blend indices encoded as half-floats.
 else if(format == 7)
   {
    vb.flags |= AMC_VERTEX_WEIGHTS;

    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x14 (UV channel)
        uint16 i1 = (uint16)bs.BE_read_real16(); // 0x16 (blend indices)
        uint16 i2 = (uint16)bs.BE_read_real16(); // 0x18 (blend indices)
        bs.BE_read_uint08(); // 0x19 (unknown vector)
        bs.BE_read_uint08(); // 0x1A (unknown vector)
        bs.BE_read_uint08(); // 0x1B (unknown vector)
        bs.BE_read_uint08(); // 0x1C (unknown vector)

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // number of weights in this vertex
        uint32 weights = 0;
        if(w1 == 1.0f) weights = 1;
        else weights = 2;

        // blending
        if(weights > 0) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = 1.0f - w1;
          }
       }
   }

 // format #8
 // STATE: POSITION OK, NORMALS OK, UVS OK, LMS OK
 // NOTES: Used by stages.
 else if(format == 8)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (((sint32)bs.BE_read_uint08()) - 127)/128.0f; // 0x0D
        real32 ny = (((sint32)bs.BE_read_uint08()) - 127)/128.0f; // 0x0E
        real32 nz = (((sint32)bs.BE_read_uint08()) - 127)/128.0f; // 0x0F
        real32 nw = (((sint32)bs.BE_read_uint08()) - 127)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x14 (UV channel)
        bs.BE_read_real16(); // 0x16 (UV channel repeat)
        bs.BE_read_real16(); // 0x18 (UV channel repeat)
        real32 lu = bs.BE_read_real16(); // 0x1A (LM channel)
        real32 lv = bs.BE_read_real16(); // 0x1C (LM channel)

        // save vertex
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

 // format #9
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by effects only.
 else if(format == 9)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        bs.BE_read_uint08(); // 0x11 (unknown vector)
        bs.BE_read_uint08(); // 0x12 (unknown vector)
        bs.BE_read_uint08(); // 0x13 (unknown vector)
        bs.BE_read_uint08(); // 0x14 (unknown vector)
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel)
        bs.BE_read_real16(); // 0x1A (UV channel repeat)
        bs.BE_read_real16(); // 0x1C (UV channel repeat)

        // save vertex
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

 return true;
}

bool modelizer::processVBx20(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0x9399 && mi.vertexformat2 == 0xC033) format = 1;
 else if(mi.vertexformat1 == 0xB86D && mi.vertexformat2 == 0xE02A) format = 2;
 else if(mi.vertexformat1 == 0x926F && mi.vertexformat2 == 0xD02E) format = 3;
 else if(mi.vertexformat1 == 0x1255 && mi.vertexformat2 == 0x3032) format = 4;
 else if(mi.vertexformat1 == 0xD877 && mi.vertexformat2 == 0x801B) format = 5;
 else if(mi.vertexformat1 == 0xDA55 && mi.vertexformat2 == 0xA021) format = 6;
 else if(mi.vertexformat1 == 0x77D8 && mi.vertexformat2 == 0x7022) format = 7;
 else if(mi.vertexformat1 == 0x747D && mi.vertexformat2 == 0x1031) format = 8;
 else return error("Unknown vertex type for size 0x20.");

 // format #1
 // STATE: POSITION OK, NORMALS OK, UV OK
 // NOTES: This one is exclusively used for stage geometry. It's the same format
 // as format #2, but without the lightmap channel; UV map channel is repeated.
 if(format == 1)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel)
        bs.BE_read_real16(); // 0x1A (UV channel repeated)
        bs.BE_read_real16(); // 0x1C (UV channel repeated)
        bs.BE_read_uint08(); // 0x1D (unknown vector)
        bs.BE_read_uint08(); // 0x1E (unknown vector)
        bs.BE_read_uint08(); // 0x1F (unknown vector)
        bs.BE_read_uint08(); // 0x20 (unknown vector)
 
        // save vertex
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

 // format #2
 // STATE: POSITION OK, NORMALS OK, UV OK, LM OK
 // NOTES: This one is exclusively used for stage geometry.
 if(format == 2)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel)
        real32 lu = bs.BE_read_real16(); // 0x1A (LM channel)
        real32 lv = bs.BE_read_real16(); // 0x1C (LM channel)
        bs.BE_read_uint08(); // 0x1D (unknown vector)
        bs.BE_read_uint08(); // 0x1E (unknown vector)
        bs.BE_read_uint08(); // 0x1F (unknown vector)
        bs.BE_read_uint08(); // 0x20 (unknown vector)
 
        // save vertex
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

 // format #3
 // STATE: POSITION OK, NORMALS OK, UV OK
 // NOTES: This one is exclusively used for stage geometry. It's the same format
 // as format #2, but without the lightmap channel; UV map channel is repeated.
 else if(format == 3)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16 (UV)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV)
        bs.BE_read_real16(); // 0x1A (UV repeat)
        bs.BE_read_real16(); // 0x1C (UV repeat)
        bs.BE_read_uint08(); // 0x1D (unknown vector)
        bs.BE_read_uint08(); // 0x1E (unknown vector)
        bs.BE_read_uint08(); // 0x1F (unknown vector)
        bs.BE_read_uint08(); // 0x20 (unknown vector)
 
        // save vertex
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

 // format #4
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: This one is exclusively used for stage geometry. 
 else if(format == 4)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel #1)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel #1)
        bs.BE_read_real16(); // 0x1A (UV channel #2)
        bs.BE_read_real16(); // 0x1C (UV channel #2)
        bs.BE_read_real16(); // 0x1E (UV channel #3)
        bs.BE_read_real16(); // 0x20 (UV channel #3)
 
        // save vertex
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

 // format #5
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: Used for all types of skeletal models where weights are equal to one.
 else if(format == 5)
   {
    vb.flags |= AMC_VERTEX_WEIGHTS;

    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        uint08 bf = bs.BE_read_uint08(); // 0x07
        uint08 bi = bs.BE_read_uint08(); // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tu = bs.BE_read_real16(); // 0x12 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x14 (UV channel)
        bs.BE_read_uint16(); // 0x16 (0x0000)
        bs.BE_read_uint16(); // 0x18 (0x0000)
        bs.BE_read_real16(); // 0x1A (unknown)
        bs.BE_read_real16(); // 0x1C (unknown)
        bs.BE_read_uint16(); // 0x1E (0x0000)
        bs.BE_read_uint16(); // 0x20 (0x0000)
 
        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;

        // set weights
        if(bf == 0x00) {
           vb.data[j].wi[0] = wmap_base + bi;
           vb.data[j].wv[0] = 1.0f;
          }
        else {
           vb.data[j].wi[0] = wmap_base;
           vb.data[j].wv[0] = 1.0f;
          }
       }
   }

 // format #6
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: Very standard vertex format for skeleton-based models.
 else if(format == 6)
   {
    // joints and weights
    vb.flags |= AMC_VERTEX_WEIGHTS;
 
    // process vertices
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        uint16 i1 = bs.BE_read_uint08(); // 0x11
        uint16 i2 = bs.BE_read_uint08(); // 0x12
        uint16 i3 = bs.BE_read_uint08(); // 0x13
        uint16 i4 = bs.BE_read_uint08(); // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16
        real32 tv = bs.BE_read_real16(); // 0x18
        real32 w2 = bs.BE_read_real16(); // 0x1A
        real32 w3 = bs.BE_read_real16(); // 0x1C
        bs.BE_read_real16(); // 0x1E
        bs.BE_read_real16(); // 0x20
 
        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;
 
        // determine number of weights
        uint32 weights = 1;
        if(i1 == i2) weights = 1;
        else if(i2 == i3) weights = 2;
        else if(i3 == i4) weights = 3;
        else weights = 4;

        // initialize blend indices
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // save blend weights
        if(weights > 0) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = w2;
          }
        if(weights > 2) {
           vb.data[j].wi[2] = wmap_base + i3;
           vb.data[j].wv[2] = w3;
          }
        if(weights > 3) {
           vb.data[j].wi[3] = wmap_base + i4;
           vb.data[j].wv[3] = 1.0f - (w1 + w2 + w3);
          }
       }
   }

 // format #7
 // STATE: POSITION OK, NORMALS OK, UV OK, WEIGHTS OK
 // NOTES: Used by static models that have skeletons (cranes, etc.)
 else if(format == 7)
   {
    // joints and weights
    vb.flags |= AMC_VERTEX_WEIGHTS;
 
    // process vertices
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        uint16 i1 = bs.BE_read_uint08(); // 0x11 (blend indices)
        uint16 i2 = bs.BE_read_uint08(); // 0x12 (blend indices)
        uint16 i3 = bs.BE_read_uint08(); // 0x13 (blend indices)
        uint16 i4 = bs.BE_read_uint08(); // 0x14 (blend indices)
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel)
        real32 w2 = bs.BE_read_real16(); // 0x1A (blend weights)
        real32 w3 = bs.BE_read_real16(); // 0x1C (blend weights)
        bs.BE_read_uint08(); // 0x1D (not blend indices)
        bs.BE_read_uint08(); // 0x1E (not blend indices)
        bs.BE_read_uint08(); // 0x1F (not blend indices)
        bs.BE_read_uint08(); // 0x20 (not blend indices)
 
        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;
 
        // determine number of weights
        uint32 weights = 1;
        if(i1 == i2) weights = 1;
        else if(i2 == i3) weights = 2;
        else if(i3 == i4) weights = 3;
        else weights = 4;

        // initialize blend indices
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // save blend weights
        if(weights > 0) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = w2;
          }
        if(weights > 2) {
           vb.data[j].wi[2] = wmap_base + i3;
           vb.data[j].wv[2] = w3;
          }
        if(weights > 3) {
           vb.data[j].wi[3] = wmap_base + i4;
           vb.data[j].wv[3] = 1.0f - (w1 + w2 + w3);
          }
       }
   }

 // format #8
 // STATE: POSITION OK, NORMALS OK, UV OK 
 // NOTES: Noticed in file md0022_03.mod, an effects file. Used by effects only.
 // Both texture channels apppear to work fine.
 else if(format == 8)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel #1)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel #1)
        bs.BE_read_real16(); // 0x1A (UV channel #2)
        bs.BE_read_real16(); // 0x1C (UV channel #2)
        bs.BE_read_real16(); // 0x1E (UV channel #3)
        bs.BE_read_real16(); // 0x20 (UV channel #3)
 
        // save vertex
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

 return true;
}

bool modelizer::processVBx24(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0xB668 && mi.vertexformat2 == 0x1034) format = 1;
 else if(mi.vertexformat1 == 0x63B6 && mi.vertexformat2 == 0xC02F) format = 2;
 else if(mi.vertexformat1 == 0x37A4 && mi.vertexformat2 == 0xE035) format = 3;
 else if(mi.vertexformat1 == 0xBB42 && mi.vertexformat2 == 0x4024) format = 4;
 else if(mi.vertexformat1 == 0xB392 && mi.vertexformat2 == 0x101F) format = 5;
 else return error("Unknown vertex type for size 0x24.");

 // format #1
 // STATE: POSITION OK, NORMALS OK, UVS OK
 // NOTES: Used by stages and effects.
 if(format == 1)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel #1)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel #1)
        bs.BE_read_real16(); // 0x1A (UV channel #2)
        bs.BE_read_real16(); // 0x1C (UV channel #2)
        bs.BE_read_uint08(); // 0x1D (unknown vector)
        bs.BE_read_uint08(); // 0x1E (unknown vector)
        bs.BE_read_uint08(); // 0x1F (unknown vector)
        bs.BE_read_uint08(); // 0x20 (unknown vector)
        bs.BE_read_real16(); // 0x22 (UV repeat)
        bs.BE_read_real16(); // 0x24 (UV repeat)

        // save vertex
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

 // format #2
 // STATE: POSITION OK, NORMALS OK, UVS OK, LIGHTMAPS OK
 // NOTES: Used by stages only.
 else if(format == 2)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16
        real32 tv = bs.BE_read_real16(); // 0x18
        bs.BE_read_real16(); // 0x1A (UV repeat)
        bs.BE_read_real16(); // 0x1C (UV repeat)
        real32 lu = bs.BE_read_real16(); // 0x1E (LM UV)
        real32 lv = bs.BE_read_real16(); // 0x20 (LM UV)
        bs.BE_read_uint32(); // 0x24 (0xFF000000)
   
        // save vertex
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

 // format #3
 // STATE: POSITION OK, NORMALS OK, UVS OK, LIGHTMAPS OK
 // NOTES: Used by stages only.
 else if(format == 3)
   {
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel #1)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel #1)
        bs.BE_read_real16(); // 0x1A (UV channel #2)
        bs.BE_read_real16(); // 0x1C (UV channel #2)
        bs.BE_read_real16(); // 0x1E (UV channel #1 repeat)
        bs.BE_read_real16(); // 0x20 (UV channel #1 repeat)
        real32 lu = bs.BE_read_real16(); // 0x22 (LM channel)
        real32 lv = bs.BE_read_real16(); // 0x24 (LM channel)
   
        // save vertex
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

 // format #4
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: Used by enemies, players, sm, and stages.
 else if(format == 4)
   {
    // joints and weights
    vb.flags |= AMC_VERTEX_WEIGHTS;

    // process vertices
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08 (blendweight #1)
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 w2 = bs.BE_read_uint08()/255.0f; // 0x0D (blendweight #2)
        real32 w3 = bs.BE_read_uint08()/255.0f; // 0x0E (blendweight #3)
        real32 w4 = bs.BE_read_uint08()/255.0f; // 0x0F (blendweight #4)
        real32 w5 = bs.BE_read_uint08()/255.0f; // 0x10 (blendweight #5)
        uint08 i1 = bs.BE_read_uint08(); // 0x11 (blendindex #1)
        uint08 i2 = bs.BE_read_uint08(); // 0x12 (blendindex #2)
        uint08 i3 = bs.BE_read_uint08(); // 0x13 (blendindex #3)
        uint08 i4 = bs.BE_read_uint08(); // 0x14 (blendindex #4)
        uint08 i5 = bs.BE_read_uint08(); // 0x15 (blendindex #5)
        uint08 i6 = bs.BE_read_uint08(); // 0x16 (blendindex #6)
        bs.BE_read_uint08(); // 0x17
        bs.BE_read_uint08(); // 0x18
        real32 tu = bs.BE_read_real16(); // 0x1A (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x1C (UV channel)
        bs.BE_read_uint16(); // 0x1E (0x0000)
        bs.BE_read_uint16(); // 0x20 (0x0000)
        bs.BE_read_uint08(); // 0x21 (unknown vector)
        bs.BE_read_uint08(); // 0x22 (unknown vector)
        bs.BE_read_uint08(); // 0x23 (unknown vector)
        bs.BE_read_uint08(); // 0x24 (unknown vector)

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // determine number of weights
        uint32 weights = 1;
        if(w2 == 0.0f) weights = 1;
        else if(w3 == 0.0f) weights = 2;
        else if(w4 == 0.0f) weights = 3;
        else if(w5 == 0.0f) weights = 4;
        else weights = 5;

        // initialize blend indices
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // save blend weights
        if(weights > 0) {
           if(header.n_jointmaps) i1 = bonedata.jointmaplist[mi.jointmapindex][i1];
           vb.data[j].wi[0] = i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           if(header.n_jointmaps) i2 = bonedata.jointmaplist[mi.jointmapindex][i2];
           vb.data[j].wi[1] = i2;
           vb.data[j].wv[1] = w2;
          }
        if(weights > 2) {
           if(header.n_jointmaps) i3 = bonedata.jointmaplist[mi.jointmapindex][i3];
           vb.data[j].wi[2] = i3;
           vb.data[j].wv[2] = w3;
          }
        if(weights > 3) {
           if(header.n_jointmaps) i4 = bonedata.jointmaplist[mi.jointmapindex][i4];
           vb.data[j].wi[3] = i4;
           vb.data[j].wv[3] = w4;
          }
        if(weights > 4) {
           if(header.n_jointmaps) i5 = bonedata.jointmaplist[mi.jointmapindex][i5];
           vb.data[j].wi[4] = i5;
           vb.data[j].wv[4] = w5;
          }

        // debug
        if(debug) {
           dfile << "p = (" << vx << ", " << vy << ", " << vz << ") ";
           dfile << "w = (" << w1 << ", " << w2 << ", " << w3 << ", " << w4 << ", " << w5 << ") ";
           dfile << "u = (" << (uint16)i1 << ", " << (uint16)i2 << ", " << (uint16)i3 << ", " << (uint16)i4 << ", " << (uint16)i5 << ") ";
           dfile << endl;
          }
       }
   }

 // format #5
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: This one is a strange one. It uses half-floats for blend indices and
 // always contains two weight/index pairs. The second UV channel appears to be
 // the diffuse UV channel. The first one looks OK but doesn't always appear to
 // be correct.
 else if(format == 5)
   {
    // joints and weights
    vb.flags |= AMC_VERTEX_WEIGHTS;

    // process vertices
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/127.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/127.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/127.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/127.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        uint16 i1 = (uint16)bs.BE_read_real16(); // 0x12 blend indices (as half-floats)
        uint16 i2 = (uint16)bs.BE_read_real16(); // 0x14 blend indices (as half-floats)
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel #1)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel #1)
        tu = bs.BE_read_real16(); // 0x1A (UV channel #2)
        tv = bs.BE_read_real16(); // 0x1C (UV channel #2)
        bs.BE_read_uint32(); // 0x20 (0x0000)
        bs.BE_read_uint32(); // 0x24 (0x0000)

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;

        // initialize blend indices
        vb.data[j].wi[0] = wmap_base + i1;
        vb.data[j].wi[1] = wmap_base + i2;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // save blend weights
        vb.data[j].wv[0] = w1;
        vb.data[j].wv[1] = 1.0f - w1;
       }
   }

 return true;
}

bool modelizer::processVBx28(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0x6459 && mi.vertexformat2 == 0x3023) format = 1;
 else if(mi.vertexformat1 == 0x75C3 && mi.vertexformat2 == 0xE025) format = 2;
 else if(mi.vertexformat1 == 0xD84E && mi.vertexformat2 == 0x3026) format = 3;
 else return error("Unknown vertex type for size 0x28.");

 // format #1
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: There are two UV channels; the first channel is the diffuse channel.
 // This format is used in enemy, player, and sm models. It will always employ
 // four blend weight/index pairs.
 if(format == 1)
   {
    vb.flags |= AMC_VERTEX_WEIGHTS;

    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        uint08 i1 = bs.BE_read_uint08(); // 0x11 (blendindex #1)
        uint08 i2 = bs.BE_read_uint08(); // 0x12 (blendindex #2)
        uint08 i3 = bs.BE_read_uint08(); // 0x13 (blendindex #3)
        uint08 i4 = bs.BE_read_uint08(); // 0x14 (blendindex #4)
        real32 tu = bs.BE_read_real16(); // 0x16 (uv channel #1)
        real32 tv = bs.BE_read_real16(); // 0x18 (uv channel #1)
        real32 w2 = bs.BE_read_real16(); // 0x1A (blendweight #2)
        real32 w3 = bs.BE_read_real16(); // 0x1C (blendweight #3)
        bs.BE_read_real16(); // 0x1E (0x00)
        bs.BE_read_real16(); // 0x20 (0x00)
        bs.BE_read_real16(); // 0x22 (uv channel #2)
        bs.BE_read_real16(); // 0x24 (uv channel #2)
        bs.BE_read_real16(); // 0x26 (0x00)
        bs.BE_read_real16(); // 0x28 (0x00)
 
        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;
 
        // initialize blend indices
        vb.data[j].wi[0] = wmap_base + i1;
        vb.data[j].wi[1] = wmap_base + i2;
        vb.data[j].wi[2] = wmap_base + i3;
        vb.data[j].wi[3] = wmap_base + i4;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // save blend weights
        vb.data[j].wv[0] = w1;
        vb.data[j].wv[1] = w2;
        vb.data[j].wv[2] = w3;
        vb.data[j].wv[3] = 1.0f - (w1 + w2 + w3);
       }
   }

 // format #2
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS SEEM OK
 // NOTES: Used by only one model, enemy em4801. This one has a very special
 // blend weight format in that it uses sint32, uint08, and real16 types for
 // storing weights. It is somewhat difficult to tell how many weights there
 // are per vertex, but in all cases I've seen it ends when an index repeats.
 else if(format == 2)
   {
    vb.flags |= AMC_VERTEX_WEIGHTS;

    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/127.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/127.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/127.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/127.0f; // 0x0C
        real32 w2 = bs.BE_read_uint08()/255.0f; // 0x0D (blendweight #2)
        real32 w3 = bs.BE_read_uint08()/255.0f; // 0x0E (blendweight #3)
        real32 w4 = bs.BE_read_uint08()/255.0f; // 0x0F (blendweight #4)
        real32 w5 = bs.BE_read_uint08()/255.0f; // 0x10 (blendweight #5)
        uint16 i1 = bs.BE_read_uint08(); // 0x11 (blendindex #1)
        uint16 i2 = bs.BE_read_uint08(); // 0x12 (blendindex #2)
        uint16 i3 = bs.BE_read_uint08(); // 0x13 (blendindex #3)
        uint16 i4 = bs.BE_read_uint08(); // 0x14 (blendindex #4)
        uint16 i5 = bs.BE_read_uint08(); // 0x15 (blendindex #5)
        uint16 i6 = bs.BE_read_uint08(); // 0x16 (blendindex #6)
        uint16 i7 = bs.BE_read_uint08(); // 0x17 (blendindex #7)
        uint16 i8 = bs.BE_read_uint08(); // 0x18 (blendindex #8)
        real32 tu = bs.BE_read_real16(); // 0x1A (UV channel #1)
        real32 tv = bs.BE_read_real16(); // 0x1C (UV channel #1)
        real32 w6 = bs.BE_read_real16(); // 0x1E (unknown)
        real32 w7 = bs.BE_read_real16(); // 0x20 (unknown)
        bs.BE_read_uint08(); // 0x21 (unknown vector)
        bs.BE_read_uint08(); // 0x22 (unknown vector)
        bs.BE_read_uint08(); // 0x23 (unknown vector)
        bs.BE_read_uint08(); // 0x24 (unknown vector)
        real32 u3 = bs.BE_read_real16(); // 0x26 (UV channel #2)
        real32 u4 = bs.BE_read_real16(); // 0x28 (UV channel #2)

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;
 
        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // number of weights in this vertex
        uint32 weights = 0;
        if(w2 == 0.0f) weights = 1;
        else if(w3 == 0.0f) weights = 2;
        else if(w4 == 0.0f) weights = 3;
        else if(w5 == 0.0f) weights = 4;
        else if(w6 == 0.0f) weights = 5;
        else if(w7 == 0.0f) weights = 6;
        else weights = 7;
 
        // blending
        if(weights > 1) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = w2;
          }
        if(weights > 2) {
           vb.data[j].wi[2] = wmap_base + i3;
           vb.data[j].wv[2] = w3;
          }
        if(weights > 3) {
           vb.data[j].wi[3] = wmap_base + i4;
           vb.data[j].wv[3] = w4;
          }
        if(weights > 4) {
           vb.data[j].wi[4] = wmap_base + i5;
           vb.data[j].wv[4] = w5;
          }
        if(weights > 5) {
           vb.data[j].wi[5] = wmap_base + i6;
           vb.data[j].wv[5] = w6;
          }
        if(weights > 6) {
           vb.data[j].wi[6] = wmap_base + i7;
           vb.data[j].wv[6] = w7;
          }
       }
   }

 // format #3
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: Used by only one model, enemy sm1586. When wx is 0x00, you can stop
 // counting weights. This vertex format does not use the 1.0 - sum of weights
 // calculation.
 else if(format == 3)
   {
    // joints and weights
    vb.flags |= AMC_VERTEX_WEIGHTS;

    // process vertices
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 w2 = bs.BE_read_uint08()/255.0f; // 0x0D (blendweight #2)
        real32 w3 = bs.BE_read_uint08()/255.0f; // 0x0E (blendweight #3)
        real32 w4 = bs.BE_read_uint08()/255.0f; // 0x0F (blendweight #4)
        real32 w5 = bs.BE_read_uint08()/255.0f; // 0x10 (blendweight #5)
        uint08 i1 = bs.BE_read_uint08(); // 0x11 (blendindex #1)
        uint08 i2 = bs.BE_read_uint08(); // 0x12 (blendindex #2)
        uint08 i3 = bs.BE_read_uint08(); // 0x13 (blendindex #3)
        uint08 i4 = bs.BE_read_uint08(); // 0x14 (blendindex #4)
        uint08 i5 = bs.BE_read_uint08(); // 0x15 (blendindex #5)
        bs.BE_read_uint08(); // 0x16 (padding)
        bs.BE_read_uint08(); // 0x17 (padding)
        bs.BE_read_uint08(); // 0x18 (padding)
        real32 tu = bs.BE_read_real16(); // 0x1A (UV channel)
        real32 tv = bs.BE_read_real16(); // 0x1C (UV channel)
        bs.BE_read_real16(); // 0x1E (0x00)
        bs.BE_read_real16(); // 0x20 (0x00)
        bs.BE_read_uint08(); // 0x21 (unknown vector)
        bs.BE_read_uint08(); // 0x22 (unknown vector)
        bs.BE_read_uint08(); // 0x23 (unknown vector)
        bs.BE_read_uint08(); // 0x24 (unknown vector)
        bs.BE_read_uint08(); // 0x25 (unknown vector)
        bs.BE_read_uint08(); // 0x26 (unknown vector)
        bs.BE_read_uint08(); // 0x27 (unknown vector)
        bs.BE_read_uint08(); // 0x28 (unknown vector)

        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;
 
        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
 
        // number of weights in this vertex
        uint32 weights = 1;
        if(w2 == 0.0f) weights = 1;
        else if(w3 == 0.0f) weights = 2;
        else if(w4 == 0.0f) weights = 3;
        else if(w5 == 0.0f) weights = 4;
        else weights = 5;
 
        // blending
        if(weights > 0) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = w2;
          }
        if(weights > 2) {
           vb.data[j].wi[2] = wmap_base + i3;
           vb.data[j].wv[2] = w3;
          }
        if(weights > 3) {
           vb.data[j].wi[3] = wmap_base + i4;
           vb.data[j].wv[3] = w4;
          }
        if(weights > 4) {
           vb.data[j].wi[4] = wmap_base + i5;
           vb.data[j].wv[4] = w5;
          }
       }
   }

 return true;
}

bool modelizer::processVBx30(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0xCBCF && mi.vertexformat2 == 0x7027) format = 1;
 else return error("Unknown vertex type for size 0x30.");

 // format #1
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: Used by enemies and by skeletal sm models. This one does not use the
 // one minus the rest of the weights method. You can always tell the number of
 // weights by checking the first zero weight. 2 and 10 are usually terminating
 // blend indices.
 if(format == 1)
   {
    // joints and weights
    vb.flags |= AMC_VERTEX_WEIGHTS;

    // process vertices
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 w2 = bs.BE_read_uint08()/255.0f; // 0x0D (blendweight)
        real32 w3 = bs.BE_read_uint08()/255.0f; // 0x0E (blendweight)
        real32 w4 = bs.BE_read_uint08()/255.0f; // 0x0F (blendweight)
        real32 w5 = bs.BE_read_uint08()/255.0f; // 0x10 (blendweight)
        uint08 i1 = bs.BE_read_uint08(); // 0x11 (blendindex)
        uint08 i2 = bs.BE_read_uint08(); // 0x12 (blendindex)
        uint08 i3 = bs.BE_read_uint08(); // 0x13 (blendindex)
        uint08 i4 = bs.BE_read_uint08(); // 0x14 (blendindex)
        uint08 i5 = bs.BE_read_uint08(); // 0x15 (blendindex)
        uint08 i6 = bs.BE_read_uint08(); // 0x16 (blendindex)
        uint08 i7 = bs.BE_read_uint08(); // 0x17 (blendindex)
        uint08 i8 = bs.BE_read_uint08(); // 0x18 (blendindex)
        real32 tu = bs.BE_read_real16(); // 0x1A (uv channel)
        real32 tv = bs.BE_read_real16(); // 0x1C (uv channel)
        real32 w6 = bs.BE_read_real16(); // 0x1E (blendweight)
        real32 w7 = bs.BE_read_real16(); // 0x20 (blendweight)
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x21
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x22
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x23
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x24
        bs.BE_read_uint32(); // 0x28 (0x00)
        bs.BE_read_real16(); // 0x2A (uv channel #2)
        bs.BE_read_real16(); // 0x2C (uv channel #2)
        bs.BE_read_uint32(); // 0x30 (0x00)
    
        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;
    
        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
    
        // number of weights in this vertex
        uint32 weights = 0;
        if(w2 == 0.0f) weights = 1;
        else if(w3 == 0.0f) weights = 2;
        else if(w4 == 0.0f) weights = 3;
        else if(w5 == 0.0f) weights = 4;
        else if(w6 == 0.0f) weights = 5;
        else if(w7 == 0.0f) weights = 6;
        else weights = 7;
    
        // blending
        if(weights > 0) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = w2;
          }
        if(weights > 2) {
           vb.data[j].wi[2] = wmap_base + i3;
           vb.data[j].wv[2] = w3;
          }
        if(weights > 3) {
           vb.data[j].wi[3] = wmap_base + i4;
           vb.data[j].wv[3] = w4;
          }
        if(weights > 4) {
           vb.data[j].wi[4] = wmap_base + i5;
           vb.data[j].wv[4] = w5;
          }
        if(weights > 5) {
           vb.data[j].wi[5] = wmap_base + i6;
           vb.data[j].wv[5] = w6;
          }
        if(weights > 6) {
           vb.data[j].wi[6] = wmap_base + i7;
           vb.data[j].wv[6] = w7;
          }
       }
   }

 return true;
}

bool modelizer::processVBx40(binary_stream& bs, const RE6MESHINFO& mi, AMC_VTXBUFFER& vb)
{
 // determine vertex format
 uint32 format = 1;
 if(mi.vertexformat1 == 0x2F55 && mi.vertexformat2 == 0xC03D) format = 1;
 else if(mi.vertexformat1 == 0x4325 && mi.vertexformat2 == 0xA03E) format = 2;
 else return error("Unknown vertex type for size 0x40.");

 // format #1
 // STATE: POSITION OK, NORMALS OK, UVS OK, WEIGHTS OK
 // NOTES: Used by enemies, players, and sm models.
 if(format == 1)
   {
    // joints and weights
    vb.flags |= AMC_VERTEX_WEIGHTS;
    
    // process vertices
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_sint16()/32767.0f; // 0x02
        real32 vy = bs.BE_read_sint16()/32767.0f; // 0x04
        real32 vz = bs.BE_read_sint16()/32767.0f; // 0x06
        real32 w1 = bs.BE_read_sint16()/32767.0f; // 0x08
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x09
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        uint08 i1 = bs.BE_read_uint08(); // 0x11 (blendindex #1)
        uint08 i2 = bs.BE_read_uint08(); // 0x12 (blendindex #2)
        uint08 i3 = bs.BE_read_uint08(); // 0x13 (blendindex #3)
        uint08 i4 = bs.BE_read_uint08(); // 0x14 (blendindex #4)
        real32 tu = bs.BE_read_real16(); // 0x16 (uv channel #1)
        real32 tv = bs.BE_read_real16(); // 0x18 (uv channel #1)
        real32 w2 = bs.BE_read_real16(); // 0x1A (blendweight #2)
        real32 w3 = bs.BE_read_real16(); // 0x1C (blendweight #3)
        real32 w4 = bs.BE_read_real16(); // 0x1E (blendweight #3)
        bs.BE_read_uint16(); // 0x20 (unknown)
        bs.BE_read_uint16(); // 0x22 (unknown)
        bs.BE_read_uint32(); // 0x26 (0x00)
        bs.BE_read_uint32(); // 0x2A (0x00)
        bs.BE_read_uint32(); // 0x2E (0x00)
        bs.BE_read_uint32(); // 0x32 (0x00)
        bs.BE_read_uint16(); // 0x34 (0x00)
        bs.BE_read_uint32(); // 0x38 (unknown)
        bs.BE_read_uint32(); // 0x3C (0x00)
        bs.BE_read_uint32(); // 0x40 (0x00)
 
        // save vertex
        vb.data[j].vx = vx;
        vb.data[j].vy = vy;
        vb.data[j].vz = vz;
        vb.data[j].nx = nx;
        vb.data[j].ny = ny;
        vb.data[j].nz = nz;
        vb.data[j].tu = tu;
        vb.data[j].tv = tv;
 
        // initialize weights
        vb.data[j].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
    
        // number of weights in this vertex
        uint32 weights = 0;
        if(w2 == 0.0f) weights = 1;
        else if(w3 == 0.0f) weights = 2;
        else if(w4 == 0.0f) weights = 3;
        else weights = 1;
    
        // blending
        if(weights > 0) {
           vb.data[j].wi[0] = wmap_base + i1;
           vb.data[j].wv[0] = w1;
          }
        if(weights > 1) {
           vb.data[j].wi[1] = wmap_base + i2;
           vb.data[j].wv[1] = w2;
          }
        if(weights > 2) {
           vb.data[j].wi[2] = wmap_base + i3;
           vb.data[j].wv[2] = w3;
          }
        if(weights > 3) {
           vb.data[j].wi[3] = wmap_base + i4;
           vb.data[j].wv[3] = w4;
          }
       }
   }

 // format #2
 // STATE: 
 // NOTES: Used by effects.
 else if(format == 2)
   {
    // process vertices
    for(uint32 j = 0; j < mi.vertices; j++)
       {
        // read vertex
        real32 vx = bs.BE_read_real32(); // 0x04
        real32 vy = bs.BE_read_real32(); // 0x08
        real32 vz = bs.BE_read_real32(); // 0x0C
        real32 nx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
        real32 ny = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
        real32 nz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
        real32 nw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x10
        real32 tx = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x11
        real32 ty = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x12
        real32 tz = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x13
        real32 tw = (bs.BE_read_uint08() - 127.0f)/128.0f; // 0x14
        real32 tu = bs.BE_read_real16(); // 0x16 (UV channel #1)
        real32 tv = bs.BE_read_real16(); // 0x18 (UV channel #1)
        bs.BE_read_real16(); // 0x1A (UV channel #2)
        bs.BE_read_real16(); // 0x1C (UV channel #2)
        bs.BE_read_real16(); // 0x1E
        bs.BE_read_real16(); // 0x20
        bs.BE_read_real16(); // 0x22
        bs.BE_read_real16(); // 0x24
        bs.BE_read_real16(); // 0x26
        bs.BE_read_real16(); // 0x28
        bs.BE_read_uint32(); // 0x2C
        bs.BE_read_uint32(); // 0x30
        bs.BE_read_uint32(); // 0x34
        bs.BE_read_real16(); // 0x36
        bs.BE_read_real16(); // 0x38
        bs.BE_read_real16(); // 0x3A
        bs.BE_read_real16(); // 0x3C
        bs.BE_read_uint32(); // 0x40
   
        // save vertex
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

 return true;
}

bool modelizer::extract(void)
{
 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // open file 
 if(!filename.length()) return error("Invalid filename.");
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // create debug file
 if(debug) {
    string ofname = ChangeFileExtension(filename, "txt");
    dfile.open(ofname.c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // processes
 if(!process001()) return false;
 if(!process002()) return false;
 if(!process003()) return false;
 if(!process004()) return false;
 if(!process005()) return false;
 if(!process006()) return false;

 // save LWO
 SaveLWO(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

bool processMOD(const std::string& filename)
{
 modelizer obj(filename);
 return obj.extract();
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

 // pathname to save archive data
 string datapath = pathname;
 datapath += "RE6_extract\\";

 // variables
 bool doARC = false;
 bool doTEX = false;
 bool doMOD = false;

 // process ARC files?
 int retval = MessageBoxA(0, "Process ARC files?\nChoose 'Yes' if you haven't done so already.\nChoose 'No' if run once before.\nChoose 'Cancel' to quit.", "Question?", MB_YESNOCANCEL);
 if(retval == IDYES) doARC = true;
 else if(retval == IDNO) doARC = false;
 else return 0;

 // process TEX files?
 retval = MessageBoxA(0, "Process TEX files?\nChoose 'Yes' if you haven't done so already.\nChoose 'No' if run once before.\nChoose 'Cancel' to quit.", "Question?", MB_YESNOCANCEL);
 if(retval == IDYES) doTEX = true;
 else if(retval == IDNO) doTEX = false;
 else return 0;

 // process MOD files?
 retval = MessageBoxA(0, "Process MOD files?\nChoose 'Yes' if you haven't done so already.\nChoose 'No' if run once before.\nChoose 'Cancel' to quit.", "Question?", MB_YESNOCANCEL);
 if(retval == IDYES) doMOD = true;
 else if(retval == IDNO) doMOD = false;
 else return 0;

 // process ARC files
 cout << "STAGE 1" << endl;
 if(doARC) {
    cout << "Processing .ARC files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".ARC", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processARC(datapath, filelist[i])) return false;
       }
    cout << endl;
   }

 // process TEX files
 cout << "STAGE 2" << endl;
 if(doTEX) {
    cout << "Processing .TEX files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".TEX", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTEX(filelist[i])) return false;
       }
    cout << endl;
   }

 // process MOD files
 cout << "STAGE 3" << endl;
 if(doMOD) {
    cout << "Processing .MOD files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".MOD", pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMOD(filelist[i])) return false;
       }
    cout << endl;
   }

 return 0;
}

}};

