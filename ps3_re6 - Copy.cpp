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

// 0x00000000: magic
// 0x00000004: 0xFF2C
// 0x00000006: number of joints (YES)
// 0x00000008: number of meshes (YES)
// 0x0000000A: number of materials (YES)
// 0x0000000C: number of vertices (YES)
// 0x00000010: number of indices (YES)
// 0x00000014: unknown (0x418)
// 0x00000018: size of all vertex buffers (YES)

// 0x00000020: number of ??? (0x20 byte items)
// 0x00000024: unknown
// 0x00000028: offset to unknown data that contains 0x18 byte items + 0x40 byte matrices
// 0x0000002C: offset to ??? (0x20 byte items)
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

struct MESHINFO {
 uint16 p0x00; // mesh type
 uint16 p0x02; // number of vertices
 uint08 p0x04; // 0, 1, 2, 3, 4, etc.
 uint08 p0x05; // always a multiple of 0x10
 uint08 p0x06; // material index
 uint08 p0x07; // level of detail (1, 2, 3, FC, FF)
 uint16 p0x08; // 0x8XXX
 uint08 p0x0A; // vertex size
 uint08 p0x0B; // vertex format
 uint32 p0x0C; // minimum index
 uint32 p0x10; // offset to start vertex
 uint16 p0x14; // 
 uint16 p0x16; // 
 uint32 p0x18; // start index
 uint32 p0x1C; // number of indices
 uint16 p0x20; // 0x0000, 0x0001, etc.
 uint16 p0x22; // 0x0000, 0xEE82, 0xEE3C, ...
 uint16 p0x24; // 
 uint16 p0x26; // 
 uint16 p0x28; // minimum index
 uint16 p0x2A; // maximum index
 uint16 p0x2C; // ??? index
 uint16 p0x2E; // ??? index
 uint32 p0x30; // 
 uint32 p0x34; // 
};

struct RE6TRIANGLE {
 uint16 a;
 uint16 b;
 uint16 c;
};

bool processMOD(const std::string& filename)
{
 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // debug support
 bool debug = true;
 ofstream dfile;
 if(debug) {
    string ofname = ChangeFileExtension(filename, "txt");
    dfile.open(ofname.c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // open file
 if(!filename.length()) return error("Invalid filename.");
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // 0x00
 // read magic
 uint32 h_magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h_magic != 0x444F4D) return error("Invalid magic number.");

 // 0x04
 // read 0x2CFF
 uint16 h_unk0x04 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h_unk0x04 != 0xFF2C) return error("Expecting 0x2CFF.");

 // 0x06
 // read number of joints
 uint16 h_joints = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x08
 // read number of meshes
 uint16 h_meshes = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x0A
 // read number of materials
 uint16 h_materials = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x0C
 // read number of vertices
 uint32 h_vertices = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x10
 // read number of indices
 uint32 h_indices = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x14
 // read unknown
 uint32 h_unk0x14 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x18
 // read total vertex buffer sizes in bytes
 uint32 h_totalvbsize = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x1C
 // read unknown
 uint32 h_unk0x1C = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h_unk0x1C != 0) return error("Expecting 0x00000000 in header.");

 // 0x20
 // read number of ???
 uint32 h_unk0x20 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x24
 // read unknown (0x00)
 uint32 h_unk0x24 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x28
 // read unknown bone data
 // contains 0x18 byte items + 0x40 byte matrices
 uint32 h_joint_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x2C
 // read offset to unknown data
 uint32 h_unk0x2C = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x30
 // read offset to material data
 uint32 h_material_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x34
 // read offset to mesh information
 // contains an array of 0x40 byte matrices
 uint32 h_meshinfo_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x38
 // read vertex buffer offset
 uint32 h_vb_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(!h_vb_offset) return error("Vertex buffer not present.");

 // 0x3C
 // read index buffer offset
 uint32 h_ib_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(!h_ib_offset) return error("Index buffer not present.");

 // 0x40
 // read offset to filesize - 0x4
 uint32 h_eof_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x44
 // read zeros
 uint32 h_pad1 = BE_read_uint32(ifile);
 uint32 h_pad2 = BE_read_uint32(ifile);
 uint32 h_pad3 = BE_read_uint32(ifile);
 if(h_pad1 != 0 || h_pad2 != 0 || h_pad3 != 0) return error("Expecting 0x00 padding in header.");
 if(ifile.fail()) return error("Read failure.");

 // 0x50
 // read bounding sphere
 real32 h_sphere_cx = BE_read_real32(ifile);
 real32 h_sphere_cy = BE_read_real32(ifile);
 real32 h_sphere_cz = BE_read_real32(ifile);
 real32 h_sphere_cr = BE_read_real32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x60
 // read min bounding box
 real32 h_bbox_min_x = BE_read_real32(ifile);
 real32 h_bbox_min_y = BE_read_real32(ifile);
 real32 h_bbox_min_z = BE_read_real32(ifile);
 real32 h_bbox_min_w = BE_read_real32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x70
 // read max bounding box
 real32 h_bbox_max_x = BE_read_real32(ifile);
 real32 h_bbox_max_y = BE_read_real32(ifile);
 real32 h_bbox_max_z = BE_read_real32(ifile);
 real32 h_bbox_max_w = BE_read_real32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x80
 // read end of header
 uint32 h_end01 = BE_read_uint32(ifile);
 uint32 h_end02 = BE_read_uint32(ifile);
 uint32 h_end03 = BE_read_uint32(ifile);
 uint32 h_end04 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h_end04 != 0x000) return error("Unexpected end of header value. Expecting 0x000.");

 // debug header data
 if(debug) {
    dfile << "HEADER" << endl;
    dfile << "unknown 0x04: 0x" << hex << h_unk0x04 << dec << endl;
    dfile << "number of joints: 0x" << hex << h_joints << dec << endl;
    dfile << "number of meshes: 0x" << hex << h_meshes << dec << endl;
    dfile << "number of materials: 0x" << hex << h_materials << dec << endl;
    dfile << "number of vertices: 0x" << hex << h_vertices << dec << endl;
    dfile << "number of indices: 0x" << hex << h_indices << dec << endl;
    dfile << "unknown 0x14: 0x" << hex << h_unk0x14 << dec << endl;
    dfile << "total vertex buffer size: 0x" << hex << h_totalvbsize << dec << endl;
    dfile << "unknown 0x1C: 0x" << hex << h_unk0x1C << dec << endl;
    dfile << "unknown 0x20: 0x" << hex << h_unk0x20 << dec << endl;
    dfile << "unknown 0x24: 0x" << hex << h_unk0x24 << dec << endl;
    dfile << "offset to joint data: 0x" << hex << h_joint_offset << dec << endl;
    dfile << "unknown 0x2C: 0x" << hex << h_unk0x2C << dec << endl;
    dfile << "offset to material data: 0x" << hex << h_material_offset << dec << endl;
    dfile << "offset to mesh information: 0x" << hex << h_meshinfo_offset << dec << endl;
    dfile << "vertex buffer offset: 0x" << hex << h_vb_offset << dec << endl;
    dfile << "index buffer offset: 0x" << hex << h_ib_offset << dec << endl;
    dfile << "EOF offset: 0x" << hex << h_eof_offset << dec << endl;
    dfile << "padding: 0x" << hex << h_pad1 << dec << endl;
    dfile << "padding: 0x" << hex << h_pad2 << dec << endl;
    dfile << "padding: 0x" << hex << h_pad3 << dec << endl;
    dfile << "sphere_cx: " << h_sphere_cx << endl;
    dfile << "sphere_cy: " << h_sphere_cy << endl;
    dfile << "sphere_cz: " << h_sphere_cz << endl;
    dfile << "sphere_cr: " << h_sphere_cr << endl;
    dfile << "bbox_min_x: " << h_bbox_min_x << endl;
    dfile << "bbox_min_y: " << h_bbox_min_y << endl;
    dfile << "bbox_min_z: " << h_bbox_min_z << endl;
    dfile << "bbox_min_w: " << h_bbox_min_w << endl;
    dfile << "bbox_max_x: " << h_bbox_max_x << endl;
    dfile << "bbox_max_y: " << h_bbox_max_y << endl;
    dfile << "bbox_max_z: " << h_bbox_max_z << endl;
    dfile << "bbox_max_w: " << h_bbox_max_w << endl;
    dfile << "end01: 0x" << hex << h_end01 << dec << endl;
    dfile << "end02: 0x" << hex << h_end02 << dec << endl;
    dfile << "end03: 0x" << hex << h_end03 << dec << endl;
    dfile << "end04: 0x" << hex << h_end04 << dec << endl;
    dfile << endl;
   }

 //
 // MESH INFORMATION
 //
 deque<MESHINFO> meshlist;
 if(h_meshinfo_offset)
   {
    // move to offset
    ifile.seekg(h_meshinfo_offset);
    if(ifile.fail()) return error("Seek failure.");

    // read mesh information
    uint32 mbsize = h_meshes * 0x38;
    boost::shared_array<char> mbdata(new char[mbsize]);
    ifile.read(mbdata.get(), mbsize);
    if(ifile.fail()) return error("Read failure.");

    // save mesh information (DEBUG MODE ONLY)
    if(debug) {
       string fname = ChangeFileExtension(filename, "mbuffer");
       ofstream temp(fname.c_str(), ios::binary);
       if(!temp) return error("Failed to create MBUFFER file.");
       temp.write(mbdata.get(), mbsize);
       if(temp.fail()) return error("Write failure.");
      }

    // read mesh information
    binary_stream bs(mbdata, mbsize);
    for(uint32 i = 0; i < h_meshes; i++) {
        MESHINFO info;
        info.p0x00 = bs.BE_read_uint16();
        info.p0x02 = bs.BE_read_uint16(); // number of vertices
        info.p0x04 = bs.BE_read_uint08();
        info.p0x05 = bs.BE_read_uint08();
        info.p0x06 = bs.BE_read_uint08(); // material index
        info.p0x07 = bs.BE_read_uint08(); // 0xFF
        info.p0x08 = bs.BE_read_uint16(); //
        info.p0x0A = bs.BE_read_uint08(); // vertex size
        info.p0x0B = bs.BE_read_uint08(); // vertex format
        info.p0x0C = bs.BE_read_uint32(); //
        info.p0x10 = bs.BE_read_uint32(); // offset to start vertex
        info.p0x14 = bs.BE_read_uint16(); // 
        info.p0x16 = bs.BE_read_uint16(); // 
        info.p0x18 = bs.BE_read_uint32(); // start index
        info.p0x1C = bs.BE_read_uint32(); // number of indices
        info.p0x20 = bs.BE_read_uint16(); //
        info.p0x22 = bs.BE_read_uint16(); //
        info.p0x24 = bs.BE_read_uint16(); // 
        info.p0x26 = bs.BE_read_uint16(); //
        info.p0x28 = bs.BE_read_uint16(); // minimum index
        info.p0x2A = bs.BE_read_uint16(); // maximum index
        info.p0x2C = bs.BE_read_uint16(); // ??? index
        info.p0x2E = bs.BE_read_uint16(); // ??? index (multiple of 16)
        info.p0x30 = bs.BE_read_uint32(); //
        info.p0x34 = bs.BE_read_uint32(); //
        meshlist.push_back(info);
       }

    if(debug) {
       for(uint32 i = 0; i < h_meshes; i++) {
           const MESHINFO& info = meshlist[i];
           dfile << "MESHINFO #" << i << " at 0x" << hex << ((uint32)ifile.tellg()) << dec << endl;
           dfile << "info.p0x00 = 0x" << hex << info.p0x00 << dec << endl;
           dfile << "info.p0x02 = 0x" << hex << info.p0x02 << dec << " (number of vertices)" << endl;
           dfile << "info.p0x04 = 0x" << hex << (uint16)(info.p0x04) << dec << endl;
           dfile << "info.p0x05 = 0x" << hex << (uint16)(info.p0x05) << dec << endl;
           dfile << "info.p0x06 = 0x" << hex << (uint16)(info.p0x06) << dec << " (material index)" << endl;
           dfile << "info.p0x07 = 0x" << hex << (uint16)(info.p0x07) << dec << " (level of detail)" << endl;
           dfile << "info.p0x08 = 0x" << hex << info.p0x08 << dec << endl;
           dfile << "info.p0x0A = 0x" << hex << (uint16)(info.p0x0A) << dec << " (vertex size)" << endl;
           dfile << "info.p0x0B = 0x" << hex << (uint16)(info.p0x0B) << dec << " (vertex format)" << endl;
           dfile << "info.p0x0C = 0x" << hex << info.p0x0C << dec << endl;
           dfile << "info.p0x10 = 0x" << hex << info.p0x10 << dec << " (offset to start vertex)" << endl;
           dfile << "info.p0x14 = 0x" << hex << info.p0x14 << dec << endl;
           dfile << "info.p0x16 = 0x" << hex << info.p0x16 << dec << endl;
           dfile << "info.p0x18 = 0x" << hex << info.p0x18 << dec << " (start index)" << endl;
           dfile << "info.p0x1C = 0x" << hex << info.p0x1C << dec << " (number of indices)" << endl;
           dfile << "info.p0x20 = 0x" << hex << info.p0x20 << dec << endl;
           dfile << "info.p0x22 = 0x" << hex << info.p0x22 << dec << endl;
           dfile << "info.p0x24 = 0x" << hex << info.p0x24 << dec << " (group ID)" << endl;
           dfile << "info.p0x26 = 0x" << hex << info.p0x26 << dec << " (unique ID)" << endl;
           dfile << "info.p0x28 = 0x" << hex << info.p0x28 << dec << " (minimum index)" << endl;
           dfile << "info.p0x2A = 0x" << hex << info.p0x2A << dec << " (maximum index)" << endl;
           dfile << "info.p0x2C = 0x" << hex << info.p0x2C << dec << " (???)" << endl;
           dfile << "info.p0x2E = 0x" << hex << info.p0x2E << dec << " (???)" << endl;
           dfile << "info.p0x30 = 0x" << hex << info.p0x30 << dec << " (0x0)" << endl;
           dfile << "info.p0x34 = 0x" << hex << info.p0x34 << dec << " (0x0)" << endl;
           dfile << endl;
          }
       dfile << endl;
      }
   }

 // seek vertex buffer offset
 ifile.seekg(h_vb_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vertex buffer data
 boost::shared_array<char> vbdata(new char[h_totalvbsize]);
 ifile.read(vbdata.get(), h_totalvbsize);
 if(ifile.fail()) return error("Read failure.");

 // save vertex buffer (DEBUG MODE ONLY)
 if(debug) {
    string fname = ChangeFileExtension(filename, "vbuffer");
    ofstream temp(fname.c_str(), ios::binary);
    if(!temp) return error("Failed to create VBUFFER file.");
    temp.write(vbdata.get(), h_totalvbsize);
    if(temp.fail()) return error("Write failure.");
   }

 // seek index buffer offset
 ifile.seekg(h_ib_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read index buffer data
 boost::shared_array<uint16> ibdata(new uint16[h_indices]);
 if(!BE_read_array(ifile, ibdata.get(), h_indices)) return error("Read failure.");

 // save index buffer (DEBUG MODE ONLY)
 if(debug) {
    string fname = ChangeFileExtension(filename, "ibuffer");
    ofstream temp(fname.c_str(), ios::binary);
    if(!temp) return error("Failed to create IBUFFER file.");
    temp.write((const char*)ibdata.get(), (h_indices*sizeof(uint16)));
    if(temp.fail()) return error("Write failure.");
   }

 // model container
 ADVANCEDMODELCONTAINER amc;

 // create vertex buffer stream
 binary_stream vbstream(vbdata, h_totalvbsize);

 // for each mesh
 for(uint32 i = 0; i < meshlist.size(); i++)
    {
     // vertex size and type
     uint16 vertex_elem = meshlist[i].p0x02;
     uint16 vertex_size = meshlist[i].p0x0A;
     uint16 vertex_type = (meshlist[i].p0x2C & 0xFF00) >> 8;

     // move to offset
     uint32 vertex_offset = meshlist[i].p0x10 + (vertex_size * meshlist[i].p0x0C);
     vbstream.seek(vertex_offset);
     if(vbstream.fail()) return error("Stream seek failure.");

     // create vertex buffer for mesh
     AMC_VTXBUFFER vbuffer;
     vbuffer.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
     vbuffer.name = "default";
     vbuffer.elem = vertex_elem;
     vbuffer.data.reset(new AMC_VERTEX[vbuffer.elem]);

     // edgeline meshes
     uint16 mesh_type = meshlist[i].p0x00;
     if(mesh_type == 0x0000)
       {
        // these meshes have no normals or texture coordinates
        real32 nx = 0.0f;
        real32 ny = 1.0f;
        real32 nz = 0.0f;
        real32 tu = 0.0f;
        real32 tv = 0.0f;

        if(vertex_size == 0x0C)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               sint16 u1 = vbstream.BE_read_sint16(); // 0x08
               sint16 u2 = vbstream.BE_read_sint16(); // 0x0A
               sint16 u3 = vbstream.BE_read_sint16(); // 0x0C
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x10)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               vbstream.BE_read_uint08(); // 0x09
               vbstream.BE_read_uint08(); // 0x0A
               vbstream.BE_read_uint08(); // 0x0B
               vbstream.BE_read_uint08(); // 0x0C
               vbstream.BE_read_uint08(); // 0x0D
               vbstream.BE_read_uint08(); // 0x0E
               vbstream.BE_read_uint08(); // 0x0F
               vbstream.BE_read_uint08(); // 0x10
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x14)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               vbstream.BE_read_uint32(); // 0x0C
               vbstream.BE_read_uint32(); // 0x10
               vbstream.BE_read_uint32(); // 0x14
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else {
           stringstream ss;
           ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0x0000 mesh.";
           return error(ss.str().c_str());
          }
       }
     else if(mesh_type == 0x1020)
       {
        // these meshes have no normals or texture coordinates
        real32 nx = 0.0f;
        real32 ny = 1.0f;
        real32 nz = 0.0f;
        real32 tu = 0.0f;
        real32 tv = 0.0f;

        if(vertex_size == 0x0C)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               vbstream.BE_read_sint16(); // 0x08
               vbstream.BE_read_sint16(); // 0x0A
               vbstream.BE_read_sint16(); // 0x0C
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x10)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               vbstream.BE_read_uint32(); // 0x0C
               vbstream.BE_read_uint32(); // 0x10
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x14)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               vbstream.BE_read_uint32(); // 0x0C
               vbstream.BE_read_uint32(); // 0x10
               vbstream.BE_read_uint32(); // 0x14
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x1C)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               vbstream.BE_read_uint32(); // 0x0C
               vbstream.BE_read_uint32(); // 0x10
               vbstream.BE_read_uint32(); // 0x14
               vbstream.BE_read_uint32(); // 0x18
               vbstream.BE_read_uint08(); // 0x19
               vbstream.BE_read_uint08(); // 0x1A
               vbstream.BE_read_uint08(); // 0x1B
               vbstream.BE_read_uint08(); // 0x1C
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else {
           stringstream ss;
           ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0x1020 mesh.";
           return error(ss.str().c_str());
          }
       }
     else if(mesh_type == 0xE4C0 || mesh_type == 0xE4C1)
       {
        if(vertex_size == 0x14)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x18)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               real32 u1 = vbstream.BE_read_real16(); // 0x16 (0x4CC0)
               real32 u2 = vbstream.BE_read_real16(); // 0x18 (0x4CC0)
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x1C)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               sint16 u1 = vbstream.BE_read_sint16(); // 0x12 (0x0000)
               sint16 u2 = vbstream.BE_read_sint16(); // 0x14 (0x0000)
               real32 tu = vbstream.BE_read_real16(); // 0x16
               real32 tv = vbstream.BE_read_real16(); // 0x18
               sint16 u3 = vbstream.BE_read_sint16(); // 0x1A (0x0000)
               sint16 u4 = vbstream.BE_read_sint16(); // 0x1C (0x0000)
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else {
           stringstream ss;
           if(mesh_type == 0xE4C0)
              ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0xE4C0 mesh.";
           else
              ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0xE4C1 mesh.";
           return error(ss.str().c_str());
          }
       }
     // geometry
     else if(mesh_type == 0xE6C8)
       {
        if(vertex_size == 0x14)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x18)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               real32 u1 = vbstream.BE_read_real16(); // 0x16 (0x4CC0)
               real32 u2 = vbstream.BE_read_real16(); // 0x18 (0x4CC0)
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x1C)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               sint16 u1 = vbstream.BE_read_sint16(); // 0x12 (0x0000)
               sint16 u2 = vbstream.BE_read_sint16(); // 0x14 (0x0000)
               real32 tu = vbstream.BE_read_real16(); // 0x16
               real32 tv = vbstream.BE_read_real16(); // 0x18
               sint16 u3 = vbstream.BE_read_sint16(); // 0x1A (0x0000)
               sint16 u4 = vbstream.BE_read_sint16(); // 0x1C (0x0000)
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else {
           stringstream ss;
           ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0xE6C8 mesh.";
           return error(ss.str().c_str());
          }
       }
     // geometry
     else if(mesh_type == 0xF5F5 || mesh_type == 0xF5F7)
       {
        if(vertex_size == 0x14)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x18)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               sint16 u1 = vbstream.BE_read_sint16(); // 0x16
               sint16 u2 = vbstream.BE_read_sint16(); // 0x18
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x1C)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               uint16 u1 = vbstream.BE_read_uint08(); // 0x11
               uint16 u2 = vbstream.BE_read_uint08(); // 0x12
               uint16 u3 = vbstream.BE_read_uint08(); // 0x13
               uint16 u4 = vbstream.BE_read_uint08(); // 0x14
               real32 tu = vbstream.BE_read_real16(); // 0x16 UV
               real32 tv = vbstream.BE_read_real16(); // 0x18 UV
               real32 u5 = vbstream.BE_read_real16(); // 0x1A
               real32 u6 = vbstream.BE_read_real16(); // 0x1C
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x24)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               vbstream.BE_read_uint16(); // 0x12 (0x4000)
               vbstream.BE_read_uint16(); // 0x14 (0x4B00)
               real32 tu = vbstream.BE_read_real16(); // 0x16
               real32 tv = vbstream.BE_read_real16(); // 0x18
               vbstream.BE_read_real16(); // 0x1A
               vbstream.BE_read_real16(); // 0x1C
               vbstream.BE_read_uint32(); // 0x20
               vbstream.BE_read_uint32(); // 0x24
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x28)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               uint08 i1 = vbstream.BE_read_uint08(); // 0x11
               uint08 i2 = vbstream.BE_read_uint08(); // 0x12
               uint08 i3 = vbstream.BE_read_uint08(); // 0x13
               uint08 i4 = vbstream.BE_read_uint08(); // 0x14
               real32 w1 = vbstream.BE_read_real16(); // 0x16 (this is actually UV)
               real32 w2 = vbstream.BE_read_real16(); // 0x18 (this is actually UV)
               real32 w3 = vbstream.BE_read_real16(); // 0x1A
               real32 w4 = vbstream.BE_read_real16(); // 0x1C
               vbstream.BE_read_uint32();             // 0x20
               real32 tu = vbstream.BE_read_real16(); // 0x22
               real32 tv = vbstream.BE_read_real16(); // 0x24
               vbstream.BE_read_uint32();             // 0x28
               tu = w1;
               tv = w2;
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else {
           stringstream ss;
           if(mesh_type == 0xF5F5)
              ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0xF5F5 mesh.";
           else
              ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0xF5F7 mesh.";
           return error(ss.str().c_str());
          }
       }
     // geometry
     else if(mesh_type == 0xFCF3)
       {
        if(vertex_size == 0x1C)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               uint16 u1 = vbstream.BE_read_uint08(); // 0x11
               uint16 u2 = vbstream.BE_read_uint08(); // 0x12
               uint16 u3 = vbstream.BE_read_uint08(); // 0x13
               uint16 u4 = vbstream.BE_read_uint08(); // 0x14
               real32 tu = vbstream.BE_read_real16(); // 0x16 UV
               real32 tv = vbstream.BE_read_real16(); // 0x18 UV
               real32 u5 = vbstream.BE_read_real16(); // 0x1A UV OFFSETS?
               real32 u6 = vbstream.BE_read_real16(); // 0x1C UV OFFSETS?
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else {
           stringstream ss;
           ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0xFCF3 mesh.";
           return error(ss.str().c_str());
          }
       }
     // geometry
     else if(mesh_type == 0xFDE7)
       {
        // same as 0xFDF7
        if(vertex_size == 0x1C)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               sint16 u1 = vbstream.BE_read_sint16(); // 0x12 (0x0000)
               sint16 u2 = vbstream.BE_read_sint16(); // 0x14 (0x0000)
               real32 tu = vbstream.BE_read_real16(); // 0x16
               real32 tv = vbstream.BE_read_real16(); // 0x18
               sint16 u3 = vbstream.BE_read_sint16(); // 0x1A (0x0000)
               sint16 u4 = vbstream.BE_read_sint16(); // 0x1C (0x0000)
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else {
           stringstream ss;
           ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0xFDE7 mesh.";
           return error(ss.str().c_str());
          }
       }
     // geometry
     else if(mesh_type == 0xFDF7)
       {
        if(vertex_size == 0x14)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x18)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               sint16 u1 = vbstream.BE_read_sint16(); // 0x16
               sint16 u2 = vbstream.BE_read_sint16(); // 0x18
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x1C)
          {
           cout << "mesh " << i << " and 0xFDF7 0x1C and 0x" << hex << meshlist[i].p0x08 << dec << endl;
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               sint16 u1 = vbstream.BE_read_sint16(); // 0x12 (0x0000)
               sint16 u2 = vbstream.BE_read_sint16(); // 0x14 (0x0000)
               real32 tu = vbstream.BE_read_real16(); // 0x16
               real32 tv = vbstream.BE_read_real16(); // 0x18
               sint16 u3 = vbstream.BE_read_sint16(); // 0x1A (0x0000)
               sint16 u4 = vbstream.BE_read_sint16(); // 0x1C (0x0000)
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x24)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               vbstream.BE_read_uint16(); // 0x12 (0x4000)
               vbstream.BE_read_uint16(); // 0x14 (0x4B00)
               real32 tu = vbstream.BE_read_real16(); // 0x16
               real32 tv = vbstream.BE_read_real16(); // 0x18
               vbstream.BE_read_real16(); // 0x1A
               vbstream.BE_read_real16(); // 0x1C
               vbstream.BE_read_uint32(); // 0x20
               vbstream.BE_read_uint32(); // 0x24
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x28)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               uint08 i1 = vbstream.BE_read_uint08(); // 0x11
               uint08 i2 = vbstream.BE_read_uint08(); // 0x12
               uint08 i3 = vbstream.BE_read_uint08(); // 0x13
               uint08 i4 = vbstream.BE_read_uint08(); // 0x14
               real32 w1 = vbstream.BE_read_real16(); // 0x16 (this is actually UV)
               real32 w2 = vbstream.BE_read_real16(); // 0x18 (this is actually UV)
               real32 w3 = vbstream.BE_read_real16(); // 0x1A
               real32 w4 = vbstream.BE_read_real16(); // 0x1C
               vbstream.BE_read_uint32();             // 0x20
               real32 tu = vbstream.BE_read_real16(); // 0x22
               real32 tv = vbstream.BE_read_real16(); // 0x24
               vbstream.BE_read_uint32();             // 0x28
               tu = w1;
               tv = w2;
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x40)
          {
           // for each vertex
           for(uint32 j = 0; j < vertex_elem; j++)
              {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               vbstream.BE_read_uint08(); // 0x11
               vbstream.BE_read_uint08(); // 0x12
               vbstream.BE_read_uint08(); // 0x13
               vbstream.BE_read_uint08(); // 0x14
               real32 tu = vbstream.BE_read_real16(); // 0x16
               real32 tv = vbstream.BE_read_real16(); // 0x18
               vbstream.BE_read_real16(); // 0x1A
               vbstream.BE_read_real16(); // 0x1C
               vbstream.BE_read_uint32(); // 0x20
               vbstream.BE_read_uint32(); // 0x24
               vbstream.BE_read_uint32(); // 0x28
               vbstream.BE_read_uint32(); // 0x2C
               vbstream.BE_read_uint32(); // 0x30
               vbstream.BE_read_uint32(); // 0x34
               vbstream.BE_read_real16(); // 0x36 blend weights
               vbstream.BE_read_real16(); // 0x38
               vbstream.BE_read_real16(); // 0x3A
               vbstream.BE_read_real16(); // 0x3C
               vbstream.BE_read_uint08(); // 0x3D blend indices
               vbstream.BE_read_uint08(); // 0x3E
               vbstream.BE_read_uint08(); // 0x3F
               vbstream.BE_read_uint08(); // 0x40
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else {
           stringstream ss;
           ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0xFDF7 mesh.";
           return error(ss.str().c_str());
          }
       }
     // geometry
     else if(mesh_type == 0xFFFF)
       {
        if(vertex_size == 0x14)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x18)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               sint16 u1 = vbstream.BE_read_sint16(); // 0x16
               sint16 u2 = vbstream.BE_read_sint16(); // 0x18
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x1C)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               sint16 u1 = vbstream.BE_read_sint16(); // 0x12 (0x0000)
               sint16 u2 = vbstream.BE_read_sint16(); // 0x14 (0x0000)
               real32 tu = vbstream.BE_read_real16(); // 0x16
               real32 tv = vbstream.BE_read_real16(); // 0x18
               sint16 u3 = vbstream.BE_read_sint16(); // 0x1A (0x0000)
               sint16 u4 = vbstream.BE_read_sint16(); // 0x1C (0x0000)
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x20)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               real32 tu = vbstream.BE_read_real16(); // 0x12
               real32 tv = vbstream.BE_read_real16(); // 0x14
               sint16 u3 = vbstream.BE_read_sint16(); // 0x16 (0x0000)
               sint16 u4 = vbstream.BE_read_sint16(); // 0x18 (0x0000)
               vbstream.BE_read_real16(); // 0x1A
               vbstream.BE_read_real16(); // 0x1C
               sint16 u5 = vbstream.BE_read_sint16(); // 0x1E (0x0000)
               sint16 u6 = vbstream.BE_read_sint16(); // 0x20 (0x0000)
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else if(vertex_size == 0x24)
          {
           cout << "0x24 SIZE 0xFFFF TYPE" << endl;
           for(uint32 j = 0; j < vertex_elem; j++) {
               if((meshlist[i].p0x08 & 0xFF00) == 0x8200) {
                  real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
                  real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
                  real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
                  real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
                  real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
                  real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
                  real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
                  real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
                  real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
                  real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
                  real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
                  real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
                  real32 u1 = vbstream.BE_read_real16(); // 0x12 (0x3C00 or 0x4000)
                  real32 u2 = vbstream.BE_read_real16(); // 0x14 (0x3C00 or 0x4000)
                  real32 tu = vbstream.BE_read_real16(); // 0x16 U
                  real32 tv = vbstream.BE_read_real16(); // 0x18 V
                  real32 u3 = vbstream.BE_read_real16(); // 0x1A
                  real32 u4 = vbstream.BE_read_real16(); // 0x1C
                  sint16 u5 = vbstream.BE_read_sint16(); // 0x1E (0x0000)
                  sint16 u6 = vbstream.BE_read_sint16(); // 0x20 (0x0000)
                  sint16 u7 = vbstream.BE_read_sint16(); // 0x22 (0x0000)
                  sint16 u8 = vbstream.BE_read_sint16(); // 0x24 (0x0000)
                  tv = 1.0f - tv;
                  vbuffer.data[j].vx = vx;
                  vbuffer.data[j].vy = vy;
                  vbuffer.data[j].vz = vz;
                  vbuffer.data[j].nx = nx;
                  vbuffer.data[j].ny = ny;
                  vbuffer.data[j].nz = nz;
                  vbuffer.data[j].tu = tu;
                  vbuffer.data[j].tv = tv;
                 }
               else if((meshlist[i].p0x08 & 0xFF00) == 0x8500) {
                  real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
                  real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
                  real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
                  real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
                  real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
                  real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
                  real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
                  real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
                  real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
                  real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
                  real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
                  real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
                  real32 u1 = vbstream.BE_read_real16(); // 0x12 (0x3C00 or 0x4000)
                  real32 u2 = vbstream.BE_read_real16(); // 0x14 (0x3C00 or 0x4000)
                  real32 u3 = vbstream.BE_read_real16(); // 0x16
                  real32 u4 = vbstream.BE_read_real16(); // 0x18
                  real32 tu = vbstream.BE_read_real16(); // 0x1A U
                  real32 tv = vbstream.BE_read_real16(); // 0x1C V
                  sint16 u5 = vbstream.BE_read_sint16(); // 0x1E (0x0000)
                  sint16 u6 = vbstream.BE_read_sint16(); // 0x20 (0x0000)
                  sint16 u7 = vbstream.BE_read_sint16(); // 0x22
                  sint16 u8 = vbstream.BE_read_sint16(); // 0x24
                  tv = 1.0f - tv;
                  vbuffer.data[j].vx = vx;
                  vbuffer.data[j].vy = vy;
                  vbuffer.data[j].vz = vz;
                  vbuffer.data[j].nx = nx;
                  vbuffer.data[j].ny = ny;
                  vbuffer.data[j].nz = nz;
                  vbuffer.data[j].tu = tu;
                  vbuffer.data[j].tv = tv;
                 }
               else
                  return error("SHIT");
              }
          }
        else if(vertex_size == 0x28)
          {
           for(uint32 j = 0; j < vertex_elem; j++) {
               real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
               real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
               real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
               real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
               real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
               real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
               real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
               real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
               real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
               real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
               real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
               real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
               uint08 i1 = vbstream.BE_read_uint08(); // 0x11
               uint08 i2 = vbstream.BE_read_uint08(); // 0x12
               uint08 i3 = vbstream.BE_read_uint08(); // 0x13
               uint08 i4 = vbstream.BE_read_uint08(); // 0x14
               real32 w1 = vbstream.BE_read_real16(); // 0x16 (this is actually UV)
               real32 w2 = vbstream.BE_read_real16(); // 0x18 (this is actually UV)
               real32 w3 = vbstream.BE_read_real16(); // 0x1A
               real32 w4 = vbstream.BE_read_real16(); // 0x1C
               vbstream.BE_read_uint32();             // 0x20
               real32 tu = vbstream.BE_read_real16(); // 0x22
               real32 tv = vbstream.BE_read_real16(); // 0x24
               vbstream.BE_read_uint32();             // 0x28
               tu = w1;
               tv = w2;
               tv = 1.0f - tv;
               vbuffer.data[j].vx = vx;
               vbuffer.data[j].vy = vy;
               vbuffer.data[j].vz = vz;
               vbuffer.data[j].nx = nx;
               vbuffer.data[j].ny = ny;
               vbuffer.data[j].nz = nz;
               vbuffer.data[j].tu = tu;
               vbuffer.data[j].tv = tv;
              }
          }
        else {
           stringstream ss;
           ss << "Unknown vertex size 0x" << setfill('0') << setw(4) << hex << vertex_size << dec << " for 0xFFFF mesh " << i << ".";
           return error(ss.str().c_str());
          }
       }
     else {
        stringstream ss;
        ss << "Unknown mesh type 0x" << setfill('0') << setw(4) << hex << mesh_type << dec << ".";
        return error(ss.str().c_str());
       }

     // append vertex buffer
     amc.vblist.push_back(vbuffer);

     //else if(vertex_size == 0x14)
     //  {
     //   for(uint32 j = 0; j < vertex_elem; j++)
     //      {
     //       // md0122.mod OK
     //       if(meshlist[i].p0x00 == 0xFFFF && meshlist[i].p0x08 == 0x8000)
     //         {
     //          real32 vx = vbstream.BE_read_real32(); // 0x04
     //          real32 vy = vbstream.BE_read_real32(); // 0x08
     //          real32 vz = vbstream.BE_read_real32(); // 0x0C
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          real32 tu = vbstream.BE_read_real16(); // 0x12
     //          real32 tv = vbstream.BE_read_real16(); // 0x14
     //          tv = 1.0f - tv;
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //       else
     //         {
     //          real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
     //          real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
     //          real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
     //          real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
     //          real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          real32 tu = vbstream.BE_read_real16(); // 0x12
     //          real32 tv = vbstream.BE_read_real16(); // 0x14
     //          tv = 1.0f - tv;
     //
     //          // 0x14 can be outlines as well
     //          if(meshlist[i].p0x00 == 0x0000) {
     //             nx = 0.0f;
     //             ny = 1.0f;
     //             nz = 0.0f;
     //             tu = 0.0f;
     //             tv = 0.0f;
     //            }
     //          else if(meshlist[i].p0x00 == 0x1020) {
     //             nx = 0.0f;
     //             ny = 1.0f;
     //             nz = 0.0f;
     //             tu = 0.0f;
     //             tv = 0.0f;
     //            }
     //
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //      }
     //  }
     //// WEAPONS: OK
     //else if(vertex_size == 0x18)
     //  {
     //   // for each vertex
     //   for(uint32 j = 0; j < vertex_elem; j++)
     //      {
     //       if(meshlist[i].p0x08 == 0x8000 || meshlist[i].p0x08 == 0x8001)
     //         {
     //          real32 vx = vbstream.BE_read_real32(); // 0x04
     //          real32 vy = vbstream.BE_read_real32(); // 0x08
     //          real32 vz = vbstream.BE_read_real32(); // 0x0C
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x15
     //          real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x16
     //          real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x17
     //          real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x18
     //          real32 tu = vbstream.BE_read_real16(); // 0x12
     //          real32 tv = vbstream.BE_read_real16(); // 0x14
     //          tv = 1.0f - tv;
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //       // OK! (s0007-01.mod)
     //       else
     //         {
     //          real32 vx = vbstream.BE_read_real32(); // 0x04
     //          real32 vy = vbstream.BE_read_real32(); // 0x08
     //          real32 vz = vbstream.BE_read_real32(); // 0x0C
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          real32 tu = vbstream.BE_read_real16(); // 0x12
     //          real32 tv = vbstream.BE_read_real16(); // 0x14
     //          real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x15
     //          real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x16
     //          real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x17
     //          real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x18
     //          tv = 1.0f - tv;
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //      }
     //  }
     //// WEAPONS: OK
     //else if(vertex_size == 0x1C)
     //  {
     //   // for each vertex
     //   for(uint32 j = 0; j < vertex_elem; j++)
     //      {
     //       // stages
     //       if(meshlist[i].p0x08 == 0x8000)
     //         {
     //          real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
     //          real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
     //          real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
     //          real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
     //          real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          sint16 u1 = vbstream.BE_read_sint16(); // 0x12 (0x0000)
     //          sint16 u2 = vbstream.BE_read_sint16(); // 0x14 (0x0000)
     //          sint16 u3 = vbstream.BE_read_sint16(); // 0x16 (0x0000)
     //          sint16 u4 = vbstream.BE_read_sint16(); // 0x18 (0x0000)
     //          real32 tu = vbstream.BE_read_sint16()/32767.0f; // 0x1A
     //          real32 tv = vbstream.BE_read_sint16()/32767.0f; // 0x1C
     //          tv = 1.0f - tv;
     //          tu = 0.0f;
     //          tv = 0.0f; // unknown UV format
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //       // OK!
     //       else if(meshlist[i].p0x08 == 0x8300 || meshlist[i].p0x08 == 0x8400)
     //         {
     //          real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
     //          real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
     //          real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
     //          real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
     //          real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          sint16 u1 = vbstream.BE_read_sint16(); // 0x12 (0x0000)
     //          sint16 u2 = vbstream.BE_read_sint16(); // 0x14 (0x0000)
     //          real32 tu = vbstream.BE_read_real16(); // 0x16
     //          real32 tv = vbstream.BE_read_real16(); // 0x18
     //          sint16 u3 = vbstream.BE_read_sint16(); // 0x1A (0x0000)
     //          sint16 u4 = vbstream.BE_read_sint16(); // 0x1C (0x0000)
     //          tv = 1.0f - tv;
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //        // TEMPORARY!
     //        else {
     //          real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
     //          real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
     //          real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
     //          real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
     //          real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          sint16 u1 = vbstream.BE_read_sint16(); // 0x12 (0x0000)
     //          sint16 u2 = vbstream.BE_read_sint16(); // 0x14 (0x0000)
     //          real32 tu = vbstream.BE_read_real16(); // 0x16
     //          real32 tv = vbstream.BE_read_real16(); // 0x18
     //          sint16 u3 = vbstream.BE_read_sint16(); // 0x1A (0x0000)
     //          sint16 u4 = vbstream.BE_read_sint16(); // 0x1C (0x0000)
     //          tv = 1.0f - tv;
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //      }
     //  }
     //// WEAPONS: OK
     //else if(vertex_size == 0x20)
     //  {
     //   if(meshlist[i].p0x00 != 0xFFFF)
     //      message("Warning: Untested vertex size 0x20.");
     //
     //   // for each vertex
     //   for(uint32 j = 0; j < vertex_elem; j++)
     //      {
     //       // md002b_01.mod works
     //       if(meshlist[i].p0x0B == 0x10)
     //         {
     //          // md002b_00.mod and md002b_02.mod works
     //          real32 vx = vbstream.BE_read_real32(); // 0x04
     //          real32 vy = vbstream.BE_read_real32(); // 0x08
     //          real32 vz = vbstream.BE_read_real32(); // 0x0C
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x11
     //          real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x12
     //          real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x13
     //          real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x14
     //          real32 tu = vbstream.BE_read_real16(); // 0x16 (1st uv channel)
     //          real32 tv = vbstream.BE_read_real16(); // 0x18 (1st uv channel)
     //          vbstream.BE_read_real16(); // 0x1A (2nd uv channel)
     //          vbstream.BE_read_real16(); // 0x1C (2nd uv channel)
     //          vbstream.BE_read_sint08()/128.0f; // 0x1D
     //          vbstream.BE_read_sint08()/128.0f; // 0x1E
     //          vbstream.BE_read_sint08()/128.0f; // 0x1F
     //          vbstream.BE_read_sint08()/128.0f; // 0x20
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //       //
     //       else if(meshlist[i].p0x00 == 0xFFFF) {
     //          real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
     //          real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
     //          real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
     //          real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
     //          real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          real32 tu = vbstream.BE_read_real16(); // 0x12
     //          real32 tv = vbstream.BE_read_real16(); // 0x14
     //          sint16 u1 = vbstream.BE_read_sint16(); // 0x16
     //          sint16 u2 = vbstream.BE_read_sint16(); // 0x18
     //          sint16 u3 = vbstream.BE_read_sint16(); // 0x1A
     //          sint16 u4 = vbstream.BE_read_sint16(); // 0x1C
     //          sint16 u5 = vbstream.BE_read_sint16(); // 0x1E
     //          sint16 u6 = vbstream.BE_read_sint16(); // 0x20
     //          tv = 1.0f - tv;
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //       else {
     //          real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
     //          real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
     //          real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
     //          real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
     //          real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
     //          real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
     //          real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
     //          real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
     //          real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //          real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //          real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //          real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //          sint16 u1 = vbstream.BE_read_sint16(); // 0x12
     //          sint16 u2 = vbstream.BE_read_sint16(); // 0x14
     //          sint16 u3 = vbstream.BE_read_sint16(); // 0x16
     //          sint16 u4 = vbstream.BE_read_sint16(); // 0x18
     //          sint16 u5 = vbstream.BE_read_sint16(); // 0x1A
     //          real32 tu = vbstream.BE_read_real32(); // 0x1C
     //          real32 tv = vbstream.BE_read_real32(); // 0x20
     //          tv = 1.0f - tv;
     //          objfile << "v " << vx << " " << vy << " " << vz << endl;
     //          objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //          objfile << "vt " << tu << " " << tv << endl;
     //         }
     //      }
     //  }
     //else if(vertex_size == 0x24)
     //  {
     //   // for each vertex
     //   for(uint32 j = 0; j < vertex_elem; j++)
     //      {
     //       // md002b_00.mod and md002b_02.mod works
     //       real32 vx = vbstream.BE_read_real32(); // 0x04
     //       real32 vy = vbstream.BE_read_real32(); // 0x08
     //       real32 vz = vbstream.BE_read_real32(); // 0x0C
     //       real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //       real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //       real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //       real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //       real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x11
     //       real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x12
     //       real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x13
     //       real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x14
     //       real32 tu = vbstream.BE_read_real16(); // 0x16 (1st uv channel)
     //       real32 tv = vbstream.BE_read_real16(); // 0x18 (1st uv channel)
     //       vbstream.BE_read_real16(); // 0x1A (2nd uv channel)
     //       vbstream.BE_read_real16(); // 0x1C (2nd uv channel)
     //       vbstream.BE_read_sint08()/128.0f; // 0x1D
     //       vbstream.BE_read_sint08()/128.0f; // 0x1E
     //       vbstream.BE_read_sint08()/128.0f; // 0x1F
     //       vbstream.BE_read_sint08()/128.0f; // 0x20
     //       vbstream.BE_read_real16(); // 0x22 (3rd uv channel)
     //       vbstream.BE_read_real16(); // 0x24 (3rd uv channel)
     //       objfile << "v " << vx << " " << vy << " " << vz << endl;
     //       objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //       objfile << "vt " << tu << " " << tv << endl;
     //      }
     //  }
     //else if(vertex_size == 0x28)
     //  {
     //   // for each vertex
     //   for(uint32 j = 0; j < vertex_elem; j++)
     //      {
     //       // pl0000.mod
     //       real32 vx = vbstream.BE_read_sint16()/32767.0f; // 0x02
     //       real32 vy = vbstream.BE_read_sint16()/32767.0f; // 0x04
     //       real32 vz = vbstream.BE_read_sint16()/32767.0f; // 0x06
     //       real32 vw = vbstream.BE_read_sint16()/32767.0f; // 0x08
     //       real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x09
     //       real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0A
     //       real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0B
     //       real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
     //       real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //       real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //       real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //       real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //       uint08 i1 = vbstream.BE_read_uint08(); // 0x11
     //       uint08 i2 = vbstream.BE_read_uint08(); // 0x12
     //       uint08 i3 = vbstream.BE_read_uint08(); // 0x13
     //       uint08 i4 = vbstream.BE_read_uint08(); // 0x14
     //       real32 w1 = vbstream.BE_read_real16(); // 0x16
     //       real32 w2 = vbstream.BE_read_real16(); // 0x18
     //       real32 w3 = vbstream.BE_read_real16(); // 0x1A
     //       real32 w4 = vbstream.BE_read_real16(); // 0x1C
     //       vbstream.BE_read_uint32();             // 0x20
     //       real32 tu = vbstream.BE_read_real16(); // 0x22
     //       real32 tv = vbstream.BE_read_real16(); // 0x24
     //       vbstream.BE_read_uint32();             // 0x28
     //       tv = 1.0f - tv;
     //       objfile << "v " << vx << " " << vy << " " << vz << endl;
     //       objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //       objfile << "vt " << tu << " " << tv << endl;
     //      }
     //  }
     //else if(vertex_size == 0x30)
     //  {
     //   // for each vertex
     //   for(uint32 j = 0; j < vertex_elem; j++)
     //      {
     //       real32 vx = vbstream.BE_read_real32(); // 0x04
     //       real32 vy = vbstream.BE_read_real32(); // 0x08
     //       real32 vz = vbstream.BE_read_real32(); // 0x0C
     //       real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //       real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //       real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //       real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //       real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x11
     //       real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x12
     //       real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x13
     //       real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x14
     //       real32 tu = vbstream.BE_read_real16(); // 0x16
     //       real32 tv = vbstream.BE_read_real16(); // 0x18
     //       vbstream.BE_read_real16(); // 0x1A
     //       vbstream.BE_read_real16(); // 0x1C
     //       vbstream.BE_read_sint16(); // 0x1E
     //       vbstream.BE_read_sint16(); // 0x20
     //       vbstream.BE_read_sint16(); // 0x22
     //       vbstream.BE_read_real16(); // 0x24
     //       vbstream.BE_read_real16(); // 0x26
     //       vbstream.BE_read_real16(); // 0x28
     //       vbstream.BE_read_uint32(); // 0x2A
     //       vbstream.BE_read_uint32(); // 0x30
     //       tv = 1.0f - tv;
     //       objfile << "v " << vx << " " << vy << " " << vz << endl;
     //       objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //       objfile << "vt " << tu << " " << tv << endl;
     //      }
     //  }
     //else if(vertex_size == 0x40)
     //  {
     //   // for each vertex
     //   for(uint32 j = 0; j < vertex_elem; j++)
     //      {
     //       // md0023_02.mod works
     //       real32 vx = vbstream.BE_read_real32(); // 0x00
     //       real32 vy = vbstream.BE_read_real32(); // 0x04
     //       real32 vz = vbstream.BE_read_real32(); // 0x08
     //       real32 nx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0C
     //       real32 ny = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0D
     //       real32 nz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0E
     //       real32 nw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x0F
     //       real32 tx = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x10
     //       real32 ty = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x11
     //       real32 tz = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x12
     //       real32 tw = (vbstream.BE_read_uint08() - 127.0f)/128.0f; // 0x13
     //       real32 tu = vbstream.BE_read_real16(); // 0x14 (1st uv channel)
     //       real32 tv = vbstream.BE_read_real16(); // 0x16 (1st uv channel)
     //       vbstream.BE_read_real16(); // 0x18 (2nd uv channel)
     //       vbstream.BE_read_real16(); // 0x1A (2nd uv channel)
     //       vbstream.BE_read_sint16(); // 0x1C
     //       vbstream.BE_read_sint16(); // 0x1E
     //       vbstream.BE_read_sint16(); // 0x20
     //       vbstream.BE_read_real16(); // 0x22
     //       vbstream.BE_read_real16(); // 0x24
     //       vbstream.BE_read_real16(); // 0x26
     //       vbstream.BE_read_uint32(); // 0x28 0x00000000
     //       vbstream.BE_read_uint32(); // 0x2C 0x00000000
     //       vbstream.BE_read_uint32(); // 0x30 0x00000000
     //       vbstream.BE_read_real16(); // 0x34
     //       vbstream.BE_read_real16(); // 0x36
     //       vbstream.BE_read_real16(); // 0x38
     //       vbstream.BE_read_real16(); // 0x3A
     //       vbstream.BE_read_real16(); // 0x3C 0x00000000
     //       vbstream.BE_read_real16(); // 0x3E 0x00000000
     //       tv = 1.0f - tv;
     //       objfile << "v " << vx << " " << vy << " " << vz << endl;
     //       objfile << "vn " << nx << " " << ny << " " << nz << endl;
     //       objfile << "vt " << tu << " " << tv << endl;
     //      }
     //  }
    }

 // for each mesh
 //uint32 vertex_offset = 0;
 for(uint32 i = 0; i < meshlist.size(); i++)
    {
     // index buffer properties (use uint32 as indices can go over 0xFFFF)
     uint32 start = meshlist[i].p0x18;
     uint32 indices = meshlist[i].p0x1C;

     // create mesh and material names
     stringstream ss;
     ss << "m_" << setfill('0') << setw(3) << i;
     ss << "_g_" << setfill('0') << meshlist[i].p0x24;
     ss << "_lod_" << setfill('0') << setw(2) << hex << (uint16)(meshlist[i].p0x07) << dec;
     ss << "_t_" << setfill('0') << setw(4) << hex << meshlist[i].p0x00 << dec;

     // container to store triangles
     deque<RE6TRIANGLE> triangles;

     // compute vertex base offset
     // if(vertex_offset < meshlist[i].p0x0C) return error("Invalid vertex base offset.");
     // uint32 vertex_base = vertex_offset - meshlist[i].p0x0C;
     uint32 vertex_base = meshlist[i].p0x0C;

     // create a deque to store triangles
     deque<uint32> trilist;
     for(uint32 j = 0; j < indices; j++)
        {
         // first things first... add index to triangle list
         if(ibdata[start + j] != 0xFFFF)
            trilist.push_back(ibdata[start + j]);

         // now test if processing is required
         if((ibdata[start + j] == 0xFFFF) || (j == indices - 1))
           {
            // must have at least three elements
            if(trilist.size() < 3) {
               stringstream ss;
               ss << "Expecting at least three tristrip indices. ";
               ss << "Stopping at index 0x" << hex << (start + j) << dec << " ";
               ss << "because ";
               if(ibdata[start + j] == 0xFFFF) ss << "we hit a 0xFFFF.";
               else if(j == indices - 1) ss << "came to the end of the index buffer.";
               return error(ss.str().c_str());
              }

            // process first triangle
            // uint16 a = trilist[0] + vertex_base;
            // uint16 b = trilist[1] + vertex_base;
            // uint16 c = trilist[2] + vertex_base;
            if(vertex_base > trilist[0]) return error("Invalid index.");
            if(vertex_base > trilist[1]) return error("Invalid index.");
            if(vertex_base > trilist[2]) return error("Invalid index.");
            uint16 a = trilist[0] - vertex_base;
            uint16 b = trilist[1] - vertex_base;
            uint16 c = trilist[2] - vertex_base;

            // save first triangle
            RE6TRIANGLE item;
            item.a = a;
            item.b = b;
            item.c = c;
            triangles.push_back(item);

            // process remaining triangles
            for(uint32 k = 3; k < trilist.size(); k++) {
                if(vertex_base > trilist[k]) return error("Invalid index.");
                a = b;
                b = c;
                //c = trilist[k] + vertex_base;
                c = trilist[k] - vertex_base;
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
         uint32 offset = 3 * j;
         ptr[offset + 0] = triangles[j].a;
         ptr[offset + 1] = triangles[j].b;
         ptr[offset + 2] = triangles[j].c;
        }

     // save index buffer
     amc.iblist.push_back(ibuffer);

     // initialize surface
     AMC_SURFACE surf;
     surf.name = ss.str().c_str();
     surf.surfmat = AMC_INVALID_SURFMAT;
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

     // increment number of vertices processed
     //vertex_offset += meshlist[i].p0x02;
    }

 // move to joint offset
 if(h_joints && h_joint_offset)
   {
    // move to joint data
    ifile.seekg(h_joint_offset);
    if(ifile.fail()) return error("Seek failure.");

    // initialize skeleton
    AMC_SKELETON skel;
    skel.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX;
    skel.name = "skeleton";

    struct RE6JOINTINFO {
     uint16 j01;
     uint16 j02;
     uint16 j03;
     uint16 j04;
     real32 j05;
     real32 j06;
     real32 j07;
     real32 j08;
     real32 j09;     
    };
    deque<RE6JOINTINFO> jinfo;

    // read 0x18 byte items
    for(uint32 i = 0; i < h_joints; i++)
       {
        RE6JOINTINFO item;
        item.j01 = BE_read_uint08(ifile); // 0x01
        item.j02 = BE_read_uint08(ifile); // 0x02
        item.j03 = BE_read_uint08(ifile); // 0x03
        item.j04 = BE_read_uint08(ifile); // 0x04
        item.j05 = BE_read_real32(ifile);
        item.j06 = BE_read_real32(ifile);
        item.j07 = BE_read_real32(ifile);
        item.j08 = BE_read_real32(ifile);
        item.j09 = BE_read_real32(ifile);
        //cout << "i = " << i << endl;
        //cout << "j01 = " << j01 << endl;
        //cout << "j02 = " << j02 << endl;
        //cout << "j03 = " << j03 << endl;
        //cout << "j04 = " << j04 << endl;
        //cout << "j05 = " << j05 << endl;
        //cout << "j06 = " << j06 << endl;
        //cout << "j07 = " << j07 << endl;
        //cout << "j08 = " << j08 << endl;
        //cout << "j09 = " << j09 << endl;
        //cout << endl;
        jinfo.push_back(item);
       }

    // read 0x40-byte matrices
    boost::shared_array<boost::shared_array<real32>> mat1(new boost::shared_array<real32>[h_joints]);
    for(uint32 i = 0; i < h_joints; i++) {
        mat1.reset(new boost::shared_array<real32>[16]);
        if(!BE_read_array(ifile, &mat1[0], 16)) return error("Read failure.");
        matrix4x4_transpose(&mat1[0]);
       }

    // read 0x40-byte matrices
    boost::shared_array<boost::shared_array<real32>> mat2(new boost::shared_array<real32>[h_joints]);
    for(uint32 i = 0; i < h_joints; i++) {
        mat2.reset(new boost::shared_array<real32>[16]);
        if(!BE_read_array(ifile, &mat2[0], 16)) return error("Read failure.");
        matrix4x4_transpose(&mat2[0]);
       }

    // process 0x40 matrices
    for(uint32 i = 0; i < h_joints; i++)
       {
        // create joint name
        stringstream ss;
        ss << "joint_" << setfill('0') << setw(3) << i;
   
        // create and insert joint
        AMC_JOINT joint;
        joint.name = ss.str();
        joint.parent = (jinfo[i].j02 == 0xFF ? AMC_INVALID_JOINT : jinfo[i].j02);
        joint.m_rel[0x0] = mat1[i][0x0];
        joint.m_rel[0x1] = mat1[i][0x1];
        joint.m_rel[0x2] = mat1[i][0x2];
        joint.m_rel[0x3] = mat1[i][0x3];
        joint.m_rel[0x4] = mat1[i][0x4];
        joint.m_rel[0x5] = mat1[i][0x5];
        joint.m_rel[0x6] = mat1[i][0x6];
        joint.m_rel[0x7] = mat1[i][0x7];
        joint.m_rel[0x8] = mat1[i][0x8];
        joint.m_rel[0x9] = mat1[i][0x9];
        joint.m_rel[0xA] = mat1[i][0xA];
        joint.m_rel[0xB] = mat1[i][0xB];
        joint.m_rel[0xC] = mat1[i][0xC];
        joint.m_rel[0xD] = mat1[i][0xD];
        joint.m_rel[0xE] = mat1[i][0xE];
        joint.m_rel[0xF] = mat1[i][0xF];
        joint.p_rel[0] = mat1[i][0x3];
        joint.p_rel[1] = mat1[i][0x7];
        joint.p_rel[2] = mat1[i][0xB];
        joint.p_rel[3] = 1.0f;
        InsertJoint(skel, joint);
       }

    // save skeleton
    amc.skllist.push_back(skel);
   }

 // save LWO
 SaveLWO(pathname.c_str(), shrtname.c_str(), amc);
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

 // pathname to save archive data
 string datapath = pathname;
 datapath += "RE6_extract\\";

 // variables
 bool doARC = false;
 bool doTEX = false;
 bool doMOD = false;

 // process ARC files?
 //int retval = MessageBoxA(0, "Process ARC files?\nChoose 'Yes' if you haven't done so already.\nChoose 'No' if run once before.\nChoose 'Cancel' to quit.", "Question?", MB_YESNOCANCEL);
 //if(retval == IDYES) doARC = true;
 //else if(retval == IDNO) doARC = false;
 //else return 0;

 // process TEX files?
 //retval = MessageBoxA(0, "Process TEX files?\nChoose 'Yes' if you haven't done so already.\nChoose 'No' if run once before.\nChoose 'Cancel' to quit.", "Question?", MB_YESNOCANCEL);
 //if(retval == IDYES) doTEX = true;
 //else if(retval == IDNO) doTEX = false;
 //else return 0;

 // process MOD files?
 int retval = MessageBoxA(0, "Process MOD files?\nChoose 'Yes' if you haven't done so already.\nChoose 'No' if run once before.\nChoose 'Cancel' to quit.", "Question?", MB_YESNOCANCEL);
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
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMOD(filelist[i])) return false;
       }
    cout << endl;
   }

 return 0;
}

}};

