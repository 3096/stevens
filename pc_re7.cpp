#include "xentax.h"
#include "x_math.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "pc_re7.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   RE7

//
// DATA STRUCTURES
//

namespace X_SYSTEM { namespace X_GAME {

struct ARCHEADER {
 char filename[64];
 uint32 unk01;
 uint32 datasize;
 uint32 unk02;
 uint32 offset;
};

}};

static std::set<uint32> unklist;

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

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
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 
 if(magic != 0x435241) return error("Invalid magic number.");

 // read 16-bit unknown (usually 0x07)
 uint16 unk01 = LE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // read number of files
 uint16 n_files = LE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure."); 
 if(n_files == 0) return true; // ARC can be empty file

 // read header
 deque<ARCHEADER> filelist;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read filename
     ARCHEADER item;
     if(!LE_read_array(ifile, &item.filename[0], 64)) return error("Read failure.");

     // read unknown
     item.unk01 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read compressed data size
     item.datasize = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read unknown
     item.unk02 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read file offset
     item.offset = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // insert file
     filelist.push_back(item);
    }

 // arc files may be nested, so to process these we can keep a list
 deque<string> arclist;

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
     uint08 b1 = LE_read_uint08(ifile);
     uint08 b2 = LE_read_uint08(ifile);

     // move to data offset again
     ifile.seekg(curr_offset);
     if(ifile.fail()) return error("Seek failure.");

     // decompress and save data
     if(isZLIB(b1, b2)) {
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
         if(ifile.fail()) return error("Read failure.");
         ofile.write(data.get(), filelist[i].datasize);
         if(ofile.fail()) return error("Write failure.");
        }

     // close output file and rename
     ofile.close();

     // reopen output file
     ifstream re7file(ofname.c_str(), ios::binary);
     if(!re7file) return error("Failed to open RE7 file.");
    
     // read first four bytes
     uint32 magic = LE_read_uint32(re7file);
     if(re7file.fail()) return error("Failed to read RE7 magic number.");
    
     // change file extension
     std::string movename;
     if(magic == 0x00444F4D) movename = ChangeFileExtension(ofname, "mod"); // model
     else if(magic == 0x00584554) movename = ChangeFileExtension(ofname, "tex"); // texture
     else if(magic == 0x00585452) movename = ChangeFileExtension(ofname, "rtx"); // texture
     else if(magic == 0x00435241) {
        movename = ChangeFileExtension(ofname, "arc");
        arclist.push_back(movename);
       }
     else if(magic == 0x53435241) movename = ChangeFileExtension(ofname, "arcs");
     else if(magic == 0x00544C42) movename = ChangeFileExtension(ofname, "blt");
     else if(magic == 0x00544D42) movename = ChangeFileExtension(ofname, "bmt"); // unknown (model related)
     else if(magic == 0x534243FF) movename = ChangeFileExtension(ofname, "cbs"); // unknown (stage related)
     else if(magic == 0x004C4343) movename = ChangeFileExtension(ofname, "ccl"); // unknown
     else if(magic == 0x00435443) movename = ChangeFileExtension(ofname, "ctc"); // unknown
     else if(magic == 0x004D4944) movename = ChangeFileExtension(ofname, "dim");
     else if(magic == 0x004D5744) movename = ChangeFileExtension(ofname, "dwm"); // unknown (model related)
     else if(magic == 0x00443245) movename = ChangeFileExtension(ofname, "e2d"); // effect
     else if(magic == 0x004E4145) movename = ChangeFileExtension(ofname, "ean"); // texture
     else if(magic == 0x00444345) movename = ChangeFileExtension(ofname, "ecd");
     else if(magic == 0x004C4645) movename = ChangeFileExtension(ofname, "efl"); // effect
     else if(magic == 0x00534645) movename = ChangeFileExtension(ofname, "efs"); // effect
     else if(magic == 0x00555145) movename = ChangeFileExtension(ofname, "equ"); // sound
     else if(magic == 0x544C4540) movename = ChangeFileExtension(ofname, "elt");
     else if(magic == 0x4C504540) movename = ChangeFileExtension(ofname, "epl");
     else if(magic == 0x004D4346) movename = ChangeFileExtension(ofname, "fcm");
     else if(magic == 0x00444647) movename = ChangeFileExtension(ofname, "gfd"); // GUI
     else if(magic == 0x00494947) movename = ChangeFileExtension(ofname, "gii"); // GUI
     else if(magic == 0x00504C47) movename = ChangeFileExtension(ofname, "glp");
     else if(magic == 0x00444D47) movename = ChangeFileExtension(ofname, "gmd"); // GUI
     else if(magic == 0x00575247) movename = ChangeFileExtension(ofname, "grw"); // effect
     else if(magic == 0x00495547) movename = ChangeFileExtension(ofname, "gui"); // GUI
     else if(magic == 0x00505449) movename = ChangeFileExtension(ofname, "itp");
     else if(magic == 0x004D434C) movename = ChangeFileExtension(ofname, "lcm");
     else if(magic == 0x00544D4C) movename = ChangeFileExtension(ofname, "lmt");
     else if(magic == 0x3174766C) movename = ChangeFileExtension(ofname, "lvt");
     else if(magic == 0x004D434D) movename = ChangeFileExtension(ofname, "mcm");
     else if(magic == 0x004C524D) movename = ChangeFileExtension(ofname, "mrl");
     else if(magic == 0x004C5050) movename = ChangeFileExtension(ofname, "ppl");
     else if(magic == 0x00564552) movename = ChangeFileExtension(ofname, "rev"); // sound
     else if(magic == 0x004D4D52) movename = ChangeFileExtension(ofname, "rmm");
     else if(magic == 0x00444353) movename = ChangeFileExtension(ofname, "scd");
     else if(magic == 0x004D4353) movename = ChangeFileExtension(ofname, "scm");
     else if(magic == 0x004C4453) movename = ChangeFileExtension(ofname, "sdl"); // effect
     else if(magic == 0x00584D53) movename = ChangeFileExtension(ofname, "smx"); // sound
     else if(magic == 0x004C5053) movename = ChangeFileExtension(ofname, "spl"); // sound
     else if(magic == 0x00414354) movename = ChangeFileExtension(ofname, "tca");
     else if(magic == 0x004D4354) movename = ChangeFileExtension(ofname, "tcm");
     else if(magic == 0x00534658) movename = ChangeFileExtension(ofname, "xfs"); // sound
     else if(magic == 0x52494646) movename = ChangeFileExtension(ofname, "wav"); // sound

     else if(magic == 0x326F6567) movename = ChangeFileExtension(ofname, "geo"); // unknown (model related)
     else if(magic == 0x00564E4C) movename = ChangeFileExtension(ofname, "lnv"); // unknown (stage related)
     else if(magic == 0x0044524C) movename = ChangeFileExtension(ofname, "lrd"); // unknown
     else if(magic == 0x0047544D) movename = ChangeFileExtension(ofname, "mtg"); // unknown (model related)
     else if(magic == 0x0056414E) movename = ChangeFileExtension(ofname, "nav"); // unknown (stage related)
     else if(magic == 0x0043434F) movename = ChangeFileExtension(ofname, "occ"); // unknown
     else if(magic == 0x53524750) movename = ChangeFileExtension(ofname, "pgr"); // unknown (stage related)
     else if(magic == 0x5A505250) movename = ChangeFileExtension(ofname, "prp"); // unknown (stage related)
     else if(magic == 0x00444252) movename = ChangeFileExtension(ofname, "rbd"); // unknown
     else if(magic == 0x00444452) movename = ChangeFileExtension(ofname, "rdd"); // unknown
     else if(magic == 0x004B5053) movename = ChangeFileExtension(ofname, "spk"); // unknown
     else if(magic == 0x004D5753) movename = ChangeFileExtension(ofname, "swm"); // unknown
     else if(magic == 0x004B4854) movename = ChangeFileExtension(ofname, "thk"); // unknown (model related)
     else if(magic == 0x00000009) movename = ChangeFileExtension(ofname, "unk"); // unknown
     else if(magic == 0x00424956) movename = ChangeFileExtension(ofname, "vib"); // unknown
     else if(magic == 0x00594157) movename = ChangeFileExtension(ofname, "way"); // unknown (stage related)
     else if(magic == 0x006E6F7A) movename = ChangeFileExtension(ofname, "zon"); // unknown (sound related)
     else if(magic == 0x53524E44) movename = ChangeFileExtension(ofname, "dnrs"); // sound
     else if(magic == 0x54534353) movename = ChangeFileExtension(ofname, "scst"); // sound
     else if(magic == 0x54534453) movename = ChangeFileExtension(ofname, "sdst"); // sound
     else if(magic == 0x43415053) movename = ChangeFileExtension(ofname, "spac"); // sound
     else if(magic == 0x51455253) movename = ChangeFileExtension(ofname, "sreq"); // sound
     else if(magic == 0x51525453) movename = ChangeFileExtension(ofname, "strq"); // sound

     // new to RE7
     else if(magic == 0x00475241) movename = ChangeFileExtension(ofname, "arg");
     else if(magic == 0x00505441) movename = ChangeFileExtension(ofname, "atp");
     else if(magic == 0x00444143) movename = ChangeFileExtension(ofname, "cad");
     else if(magic == 0x004C5343) movename = ChangeFileExtension(ofname, "csl");
     else if(magic == 0x00637763) movename = ChangeFileExtension(ofname, "cwc");
     else if(magic == 0x4D564540) movename = ChangeFileExtension(ofname, "evm");
     else if(magic == 0x52555145) movename = ChangeFileExtension(ofname, "equr");
     else if(magic == 0x00444347) movename = ChangeFileExtension(ofname, "gcd");
     else if(magic == 0x0078656a) movename = ChangeFileExtension(ofname, "jex");
     else if(magic == 0x004C554A) movename = ChangeFileExtension(ofname, "jul");
     else if(magic == 0x0052414c) movename = ChangeFileExtension(ofname, "lar");
     else if(magic == 0x004c434d) movename = ChangeFileExtension(ofname, "mcl");
     else if(magic == 0x00504650) movename = ChangeFileExtension(ofname, "pfp");
     else if(magic == 0x52564552) movename = ChangeFileExtension(ofname, "revr");
     else if(magic == 0x46464952) movename = ChangeFileExtension(ofname, "riff");
     else if(magic == 0x0A706D72) movename = ChangeFileExtension(ofname, "rmp");
     else if(magic == 0xFF434253) movename = ChangeFileExtension(ofname, "sbc");
     else if(magic == 0x524b4253) movename = ChangeFileExtension(ofname, "sbkr");
     else if(magic == 0x52534353) movename = ChangeFileExtension(ofname, "scsr");
     else if(magic == 0x52515253) movename = ChangeFileExtension(ofname, "srqr");
     else if(magic == 0x004C5353) movename = ChangeFileExtension(ofname, "ssl");
     else if(magic == 0x52515453) movename = ChangeFileExtension(ofname, "stqr");
     else if(magic == 0x00445653) movename = ChangeFileExtension(ofname, "svd");
     else if(magic == 0x006d7773) movename = ChangeFileExtension(ofname, "swm");
     else if(magic == 0x00707773) movename = ChangeFileExtension(ofname, "swp");
     else {
        unklist.insert(magic);
        const char* temp = reinterpret_cast<const char*>(&magic);
        stringstream ss;
        ss << "Unknown RE7 file type 0x" << hex << magic << dec << " - " << ofname << " - ";
        ss << temp[0] << temp[1] << temp[2] << temp[3];
        return error(ss);
       }
    
     // close and delete previous file
     re7file.close();
     DeleteFileA(movename.c_str());
    
     // rename file
     BOOL success = MoveFileA(ofname.c_str(), movename.c_str());
     if(!success) {
        stringstream ss;
        ss << "Failed to rename " << ofname << " to " << movename << ".";
        return error(ss);
       }
    }

 // process nested arch files
 for(uint32 i = 0; i < arclist.size(); i++)
     if(!processARC(datapath, arclist[i])) return false;

 // success
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
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x584554) return error("Invalid TEX file.");

 // read unk01
 // usually 0x2000A09D
 uint32 unk01 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read properties
 uint32 properties = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 uint32 dx = ((properties & 0x000FFF00) >>  6);
 uint32 dy = ((properties & 0xFFF00000) >> 19);
 uint32 mipmaps = (properties & 0xFF) - 1;
 cout << " dx = " << dx << endl;
 cout << " dy = " << dy << endl;
 cout << " mipmaps = " << mipmaps << endl;

 // read image type
 uint32 type = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 uint32 fb1 = (type & 0xFF);
 uint32 fb2 = (type & 0xFF00) >> 8;
 uint32 fb3 = (type & 0xFF0000) >> 16;
 uint32 fb4 = (type & 0xFF000000) >> 24;
 cout << " format byte 1 = 0x" << hex << fb1 << dec << endl;
 cout << " format byte 2 = 0x" << hex << fb2 << dec << endl;
 cout << " format byte 3 = 0x" << hex << fb3 << dec << endl;
 cout << " format byte 4 = 0x" << hex << fb4 << dec << endl;
 if(fb1 != 1) cout << "FUCKED UP!" << endl;

 // read mipmap offsets
 deque<uint32> offsets;
 if(fb1 == 0x01)
   {
    for(uint32 i = 0; i < (mipmaps + 1); i++) {
        uint32 item = LE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");
        offsets.push_back(item);
       }
    if(filesize < offsets[0]) return error("Texture filesize is less than the start offset.");
   }

 // seek data
 uint32 datasize = filesize;
 if(fb1 == 0x01) {
    datasize -= offsets[0];
    ifile.seekg(offsets[0]);
   }
 if(!datasize) return error("Invalid texture data size.");

 // read data
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 //if(ifile.fail()) return error("Read failure.");

 // DDS header depends on format
 DDS_HEADER ddsh;
 uint32 format = ((type & 0xFF00) >> 8);
 uint32 format_byte1 = (type & 0xFF);

 // WORKS 100% for fb1 = 1, but not for fb1 = 6
 // 0x13 = for everything (DM, CM, LM)
 if(format == 0x13) {
    cout << "TYPE 0x13" << endl;
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // WORKS 100%
 else if(format == 0x17) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // WORKS 100%
 else if(format == 0x19) {
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // WORKS 100%
 else if(format == 0x1F) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // WORKS 100%
 // 0x20 = CM and LM (color and light) maps
 else if(format == 0x20) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // WORKS 100%
 // 0x25 = regular purple normal maps
 else if(format == 0x25) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // WORKS 100%
 // 0x27 = for noise, HUD buttons, cursor
 else if(format == 0x27) {
    if(!CreateA8R8G8B8DDSHeader(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // WORKS 100% 
 else if(format == 0x2A) {
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 else {
    stringstream ss;
    ss << "Unknown texture format 0x" << hex << format << dec << ".";
    cout << ss.str() << endl;
    //return error(ss);
   }

 // create outfile
 string ofname = ChangeFileExtension(filename, "dds");
 ofstream ofile(ofname.c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // save DDS header
 ofile.write("DDS ", 4);
 ofile.write((char*)&ddsh, sizeof(ddsh));
 if(ofile.fail()) return error("Write failure.");

 // save DX10 header
 // struct DDS_HEADER_DXT10 {
 //  uint32 dxgiFormat;
 //  uint32 resourceDimension;
 //  uint32 miscFlag;
 //  uint32 arraySize;
 //  uint32 reserved;
 // };
 // if(format == 0x2A) {
 //    DDS_HEADER_DXT10 dxt10;
 //    dxt10.dxgiFormat = 78; // DXGI_FORMAT_BC3_UNORM;
 //    dxt10.resourceDimension = 3; // DDS_DIMENSION_TEXTURE2D 
 //    dxt10.miscFlag = 0;
 //    dxt10.arraySize = 1;
 //    dxt10.reserved = 0;
 //    ofile.write((char*)&dxt10, sizeof(dxt10));
 //   }

 // save data
 ofile.write(data.get(), datasize);
 if(ofile.fail()) return error("Write failure.");

 // success
 return true;
}

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
 datapath += "RE7_extract\\";

 // variables
 bool doARC = false;
 bool doTEX = true;

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

 // // print unknown file extensions
 // for(auto i = unklist.begin(); i != unklist.end(); i++) {
 //     const char* temp = reinterpret_cast<const char*>(&(*i));
 //     stringstream ss;
 //     ss << "Unknown RE7 file type 0x" << hex << (*i) << dec << " - ";
 //     ss << temp[0] << temp[1] << temp[2] << temp[3];
 //     cout << ss.str() << endl;
 //    }

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

 return true;
}

}};

