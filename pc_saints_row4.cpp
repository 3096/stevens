#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_zlib.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "pc_saints_row4.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   SAINTS_ROW_4

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processVPP2(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open VPP_PC file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // h05 flags
 // 0x0002 (uncompressed)
 //  preload_anim.vpp_pc
 //  preload_rigs.vpp_pc
 // 0x4803 (compressed)
 //  soundboot.vpp_pc

 // read header
 uint32 h01 = LE_read_uint32(ifile); // magic
 uint32 h02 = LE_read_uint32(ifile); // version
 uint32 h03 = LE_read_uint32(ifile); // unknown
 uint32 h04 = LE_read_uint32(ifile); // VPP filesize
 uint32 h05 = LE_read_uint32(ifile); // 0x0000, 0x0002, 0x4801, 0x4803
 uint32 h06 = LE_read_uint32(ifile); // number of files
 uint32 h07 = LE_read_uint32(ifile); // size of file information table
 uint32 h08 = LE_read_uint32(ifile); // size of filename table
 uint32 h09 = LE_read_uint32(ifile); // size of data from end of filename table to EOF
 uint32 h10 = LE_read_uint32(ifile); // 0xFFFFFFFF

 // validate header
 if(h01 != 0x51890ACE) return error("Invalid VPP_PC file.");
 if(h02 != 0x0A) return error("Invalid VPP_PC file."); // this changed from 0x6 to 0xA
 if(h06 == 0x00) return true; // OK to have 0 files

 // read file information
 struct VPP2FILEENTRY {
  uint32 p01; // offset into filename table
  uint32 p02; // 0x00
  uint32 p03; // offset into data
  uint32 p04; // size of data / size of decompressed data
  uint32 p05; // size of compressed data or 0xFFFFFFFF
  uint32 p06; // 0x00000100
  std::string filename;
 };
 deque<VPP2FILEENTRY> fileinfo;
 for(uint32 i = 0; i < h06; i++) {
     VPP2FILEENTRY info;
     info.p01 = LE_read_uint32(ifile);
     info.p02 = LE_read_uint32(ifile);
     info.p03 = LE_read_uint32(ifile);
     info.p04 = LE_read_uint32(ifile);
     info.p05 = LE_read_uint32(ifile);
     info.p06 = LE_read_uint32(ifile);
     fileinfo.push_back(info);
    }

 // move to string table
 uint32 strtable_offset = 0x28 + h07; // end-of-header + offset to table
 ifile.seekg(strtable_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read string table
 for(uint32 i = 0; i < h06; i++) {
     // move to string
     ifile.seekg(strtable_offset + fileinfo[i].p01);
     if(ifile.fail()) return error("Seek failure.");
     // read string
     char buffer[1024];
     if(!read_string(ifile, buffer, 1024)) return error("Failed to read string.");
     fileinfo[i].filename = buffer;
    }

 // create directory to store extracted files
 STDSTRING vpp2path = pathname;
 vpp2path += shrtname;
 vpp2path += TEXT("\\");
 CreateDirectory(vpp2path.c_str(), NULL);

 // special case where data is zlib packed
 if(h05 == 0x4803)
   {
    // move to first zlib chunk
    uint32 position = strtable_offset + h08; // ST offset + size of ST
    ifile.seekg(position);
    if(ifile.fail()) return error("Seek failure.");

    // save temporary file
    STDSTRING tname = pathname;
    tname += shrtname;
    tname += TEXT(".bin_pc");

    // create file
    ofstream tfile(tname.c_str(), ios::binary);
    if(!tfile) return error("Failed to create temporary output file.");

    // decompress and save data
    if(!InflateZLIB(ifile, tfile, 0))
       return false;

    // close input and output file
    ifile.close();
    tfile.close();

    // open temporary bin file
    ifile.open(tname.c_str(), ios::binary);
    if(!ifile.is_open()) return error("Failed to open temporary output file.");

    // save files
    for(uint32 i = 0; i < h06; i++)
       {
        // generate filename
        cout << " Saving " << fileinfo[i].filename << "..." << endl;
        STDSTRING tempname = vpp2path;        
        tempname += Unicode08ToUnicode16(fileinfo[i].filename.c_str()).get();

        // move to data
        uint32 position = fileinfo[i].p03; // has no header or stringtable
        ifile.seekg(position);
        if(ifile.fail()) return error("Seek failure.");

        // create file
        ofstream ofile(tempname.c_str(), ios::binary);
        if(!ofile) return error("Failed to create output file.");
    
        // no need to save data
        if(fileinfo[i].p04 == 0x00) continue;
    
        // read data
        boost::shared_array<char> data(new char[fileinfo[i].p04]);
        ifile.read(data.get(), fileinfo[i].p04);
        if(ifile.fail()) return error("Read failure.");
        
        // save data
        ofile.write(data.get(), fileinfo[i].p04);
        if(ofile.fail()) return error("Write failure.");
       }
   }
 // other cases
 else
   {
    // save files
    for(uint32 i = 0; i < h06; i++)
       {
        // generate filename
        cout << " Saving " << fileinfo[i].filename << "..." << endl;
        STDSTRING tempname = vpp2path;
        tempname += Unicode08ToUnicode16(fileinfo[i].filename.c_str()).get();
   
        // move to data
        uint32 position = strtable_offset + h08; // ST offset + size of ST
        position += fileinfo[i].p03; // data offset
        ifile.seekg(position);
        if(ifile.fail()) return error("Seek failure.");
   
        // no need to save data
        if(fileinfo[i].p04 == 0x00) continue;
   
        // uncompressed
        if(fileinfo[i].p05 == 0xFFFFFFFF)
          {
           // create file
           ofstream ofile(tempname.c_str(), ios::binary);
           if(!ofile) return error("Failed to create output file.");
   
           // read data
           boost::shared_array<char> data(new char[fileinfo[i].p04]);
           ifile.read(data.get(), fileinfo[i].p04);
           if(ifile.fail()) return error("Read failure.");
           
           // save data
           ofile.write(data.get(), fileinfo[i].p04);
           if(ofile.fail()) return error("Write failure.");
          }
        // compressed
        else
          {
           // prepare compressed item
           ZLIBINFO item;
           item.deflatedBytes = fileinfo[i].p05;
           item.inflatedBytes = fileinfo[i].p04;
           item.filename = Unicode16ToUnicode08(tempname.c_str()).get();
    
           // insert compressed item
           deque<ZLIBINFO> zlist;
           zlist.push_back(item);
   
           // decompress
           if(!DecompressZLIB(ifile, zlist, 0)) return false;
          }
       }
   }

 return true;
}

bool processSTR2(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open STR2_PC file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read magic
 uint32 magic = LE_read_uint32(ifile);
 if(magic != 0x51890ACE) return error("Invalid STR2_PC file.");

 // read version
 uint32 version = LE_read_uint32(ifile);
 if(version != 0x0A) return error("Invalid STR2_PC file."); // this changed from 0x6 to 0xA

 // move to fileheader data
 ifile.seekg(0x10); // this changed too!
 uint32 h01 = LE_read_uint32(ifile);
 uint32 h02 = LE_read_uint32(ifile); // number of entries
 uint32 h03 = LE_read_uint32(ifile); // offset from end of header to string table
 uint32 h04 = LE_read_uint32(ifile); // stringtable size
 uint32 h05 = LE_read_uint32(ifile); // uncompressed data size
 uint32 h06 = LE_read_uint32(ifile); // compressed data size

 // validate
 if(h02 == 0x0000) return true; // you can have 0 files
 if(h01 != 0x4803) return error("Expecting h01 = 0x4803.");

 // file information data
 struct STR2FILEENTRY {
  uint32 p01, p02, p03;
  uint32 p04, p05, p06;
  std::string filename;
 };
 deque<STR2FILEENTRY> fileinfo;
 deque<ZLIBINFO> zlibinfo;

 // read file information
 for(uint32 i = 0; i < h02; i++)
    {
     // read item
     uint32 p01 = LE_read_uint32(ifile); // filename offset
     uint32 p02 = LE_read_uint32(ifile); // 0x00000000
     uint32 p03 = LE_read_uint32(ifile); // filedata offset
     uint32 p04 = LE_read_uint32(ifile); // uncompressed size (can be zero!)
     uint32 p05 = LE_read_uint32(ifile); // compressed size (can be zero!)
     uint32 p06 = LE_read_uint32(ifile); // 0x01001000

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
 uint32 strtable_offset = 0x28 + h03;
 ifile.seekg(strtable_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read string table
 for(uint32 i = 0; i < h02; i++) {
     // seek string
     ifile.seekg(strtable_offset + fileinfo[i].p01);
     if(ifile.fail()) return error("Seek failure.");
     // read string
     char buffer[1024];
     if(!read_string(ifile, buffer, 1024)) return error("Failed to read string.");
     fileinfo[i].filename = buffer;
     zlibinfo[i].filename = buffer;
     //if(strlen(buffer) % 2 == 0) ifile.seekg(1, ios::cur);
    }

 // move to compressed data
 ifile.seekg((0x28 + h03) + h04);
 if(ifile.fail()) return error("Seek failure.");

 // create directory to store extracted files
 STDSTRING str2path = pathname;
 str2path += shrtname;
 str2path += TEXT("\\");
 CreateDirectory(str2path.c_str(), NULL);

 // set filenames
 for(uint32 i = 0; i < h02; i++) {
     std::string tempname = Unicode16ToUnicode08(str2path.c_str()).get();
     tempname += fileinfo[i].filename;
     zlibinfo[i].filename = tempname;
    }

 // save compressed data
 DecompressZLIB(ifile, zlibinfo, 0); 
 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

#define CPEG_TEXTURE 1
#define CVBM_TEXTURE 2

bool processCPEG(LPCTSTR filename, uint16 type = CPEG_TEXTURE)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = LE_read_uint32(ifile); // GEKV
 uint32 h02 = LE_read_uint32(ifile); // 0x0D
 uint32 h03 = LE_read_uint32(ifile); // cpeg filesize
 uint32 h04 = LE_read_uint32(ifile); // gpeg filesize
 uint32 h05 = LE_read_uint32(ifile); // number of files
 uint16 h06 = LE_read_uint16(ifile); // ???
 uint16 h07 = LE_read_uint16(ifile); // ???

 // validate header
 if(h01 != 0x564B4547) return error("Expecting GEKV.");
 if(h02 != 0x0D) return error("Expecting 0x0D.");
 if(h05 == 0) return true; // sometimes 0!

 // read entries
 struct CPEGENTRY {
  uint32 p01;    // offset
  uint32 p02;    // 0x00000000
  uint16 p03;    // dx
  uint16 p04;    // dy
  uint32 p05;    // image type
  uint32 p06;    // flags
  uint32 p07;    // flags
  uint32 p08;    // flags 0x00000000
  uint32 p09;    // flags 0x00000000
  uint08 p10[4]; // number of total mipmaps
  uint32 p11;    // datasize
  uint32 p12[8]; // 0x00000000
 };
 boost::shared_array<CPEGENTRY> entries(new CPEGENTRY[h05]);
 for(uint32 i = 0; i < h05; i++) {
     entries[i].p01 = LE_read_uint32(ifile);
     entries[i].p02 = LE_read_uint32(ifile);
     entries[i].p03 = LE_read_uint16(ifile);
     entries[i].p04 = LE_read_uint16(ifile);
     entries[i].p05 = LE_read_uint32(ifile);
     entries[i].p06 = LE_read_uint32(ifile);
     entries[i].p07 = LE_read_uint32(ifile);
     entries[i].p08 = LE_read_uint32(ifile);
     entries[i].p09 = LE_read_uint32(ifile);
     entries[i].p10[0] = LE_read_uint08(ifile);
     entries[i].p10[1] = LE_read_uint08(ifile);
     entries[i].p10[2] = LE_read_uint08(ifile);
     entries[i].p10[3] = LE_read_uint08(ifile);
     entries[i].p11 = LE_read_uint32(ifile);
     entries[i].p12[0] = LE_read_uint32(ifile);
     entries[i].p12[1] = LE_read_uint32(ifile);
     entries[i].p12[2] = LE_read_uint32(ifile);
     entries[i].p12[3] = LE_read_uint32(ifile);
     entries[i].p12[4] = LE_read_uint32(ifile);
     entries[i].p12[5] = LE_read_uint32(ifile);
     entries[i].p12[6] = LE_read_uint32(ifile);
     entries[i].p12[7] = LE_read_uint32(ifile);
    }

 // read strings
 deque<STDSTRING> filenames;
 for(uint32 i = 0; i < h05; i++) {
     // read UTF-08 string
     char buffer[1024];
     read_string(ifile, buffer, 1024);
     // save UTF-16 string
     STDSTRING str = Unicode08ToUnicode16(buffer).get();
     filenames.push_back(str);
    }

 // create GPEG_PC filename
 STDSTRING gname = pathname;
 gname += shrtname;
 gname += TEXT(".");
 if(type == CPEG_TEXTURE) gname += TEXT("gpeg_pc");
 else gname += TEXT("gvbm_pc");

 // open GPEG_PC file
 ifile.close();
 ifile.open(gname.c_str(), ios::binary);
 if(!ifile) return error("Failed to open GPEG_PC file.");

 // save files
 for(uint32 i = 0; i < h05; i++)
    {
     // create filename
     STDSTRING fpath = pathname;
     STDSTRING fname = fpath;
     fname += GetShortFilenameWithoutExtension(filenames[i].c_str()).get();
     fname += TEXT(".dds");

     // obtain texture information
     uint32 GPEG_offset = entries[i].p01;
     uint32 GPEG_dx = (entries[i].p03);
     uint32 GPEG_dy = (entries[i].p04);
     uint32 GPEG_type = entries[i].p05;
     uint32 GPEG_mipmaps = (entries[i].p10[3]);
     uint32 GPEG_datasize = entries[i].p11;

     // read GPEG data
     boost::shared_array<char> GPEG_data(new char[GPEG_datasize]);
     ifile.seekg(GPEG_offset);
     if(ifile.fail()) return error("Seek failure.");
     ifile.read(GPEG_data.get(), GPEG_datasize);

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
     else if(GPEG_type == 0x193) {
        DDS_HEADER ddsh;
        if(!CreateUncompressedDDSHeader(GPEG_dx, GPEG_dy, GPEG_mipmaps, 0x10, 0xF800, 0x7E0, 0x1F, 0x0000, FALSE, &ddsh)) return error("CPEG_PC: Failed to create DDS file header.");
        if(!SaveDDSFile(fname.c_str(), ddsh, GPEG_data, GPEG_datasize)) return error("CPEG_PC: Failed to save DDS file.");
       }
     // ARGB (FINISHED)
     else if(GPEG_type == 0x197) {
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

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

bool processCMSH(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug file
 std::ofstream dfile;
 bool debug = true;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname;
    ss << shrtname;
    ss << TEXT(".txt"); 
    dfile.open(ss.str().c_str(), ios::binary);
    if(!dfile) return error("Failed to create debug file.");
   }

 // read header
 uint32 h01 = LE_read_uint32(ifile);
 uint32 h02 = LE_read_uint32(ifile); // string table bytes
 uint32 h03 = LE_read_uint32(ifile);
 uint32 h04 = LE_read_uint32(ifile); // number of strings
 if(h01 != 0x00043854) return error("Invalid file.");

 // read string table
 if(debug) {
    dfile << "------------" << endl;
    dfile << "STRING TABLE" << endl;
    dfile << "------------" << endl;
    dfile << endl;
   }

 // move to 0x20
 ifile.seekg(0x20);
 if(ifile.fail()) return error("Seek failure.");

 // read string table
 deque<string> stringtable;
 if(h02 && h04) {
    for(uint32 i = 0; i < h04; i++) {
        char buffer[1024];
        if(!read_string(ifile, buffer, 1024)) return error("Read failure.");
        stringtable.push_back(string(buffer));
        if(debug) dfile << buffer << endl;
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
     if(ifile.fail()) return error("Read failure.");
    }
 else return error("Invalid string table.");
 if(debug) dfile << endl;

 // move to next 0x10 aligned position
 uint32 position = (uint32)ifile.tellg();
 position = ((position + 0x0F) & (~0x0F));
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");

 // read BKD. header
 uint32 BKD_h01 = LE_read_uint32(ifile); // .DKB
 uint32 BKD_h02 = LE_read_uint32(ifile); // 0x2A
 uint16 BKD_h03 = LE_read_uint16(ifile); // 0x00
 uint16 BKD_h04 = LE_read_uint16(ifile); // number of joints
 uint16 BKD_h05 = LE_read_uint16(ifile); // number of meshes
 uint16 BKD_h06 = LE_read_uint16(ifile); // 0x01
 uint16 BKD_h07 = LE_read_uint16(ifile); // ????
 uint16 BKD_h08 = LE_read_uint16(ifile); // ????
 uint32 BKD_h09 = LE_read_uint32(ifile); // ????
 uint32 BKD_h10 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h11 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h12 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h13 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h14 = LE_read_uint32(ifile); // ????
 real32 BKD_h15 = LE_read_real32(ifile); // position??? scale???
 real32 BKD_h16 = LE_read_real32(ifile); // position??? scale???
 real32 BKD_h17 = LE_read_real32(ifile); // position??? scale???
 uint16 BKD_h18 = LE_read_uint16(ifile); // 0x00 or 0x01
 uint16 BKD_h19 = LE_read_uint16(ifile); // number of 0x28 byte entries
 uint32 BKD_h20 = LE_read_uint32(ifile); // ????
 uint32 BKD_h21 = LE_read_uint32(ifile); // ???? similar to BKD_h23
 uint32 BKD_h22 = LE_read_uint32(ifile); // 0x00
 uint32 BKD_h23 = LE_read_uint32(ifile); // ???? similar to BKD_h21
 uint32 BKD_h24 = LE_read_uint32(ifile); // 0x00
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
 if(debug) {
    dfile << " " << "BKD_h01: 0x" << hex << BKD_h01 << dec << endl;
    dfile << " " << "BKD_h02: 0x" << hex << BKD_h02 << dec << endl;
    dfile << " " << "BKD_h03: 0x" << hex << BKD_h03 << dec << endl;
    dfile << " " << "BKD_h04: 0x" << hex << BKD_h04 << dec << " joints" << endl;
    dfile << " " << "BKD_h05: 0x" << hex << BKD_h05 << dec << " meshes" << endl;
    dfile << " " << "BKD_h06: 0x" << hex << BKD_h06 << dec << endl;
    dfile << " " << "BKD_h07: 0x" << hex << BKD_h07 << dec << endl;
    dfile << " " << "BKD_h08: 0x" << hex << BKD_h08 << dec << endl;
    dfile << " " << "BKD_h09: 0x" << hex << BKD_h09 << dec << endl;
    dfile << " " << "BKD_h10: 0x" << hex << BKD_h10 << dec << endl;
    dfile << " " << "BKD_h11: 0x" << hex << BKD_h11 << dec << endl;
    dfile << " " << "BKD_h12: 0x" << hex << BKD_h12 << dec << endl;
    dfile << " " << "BKD_h13: 0x" << hex << BKD_h13 << dec << endl;
    dfile << " " << "BKD_h14: 0x" << hex << BKD_h14 << dec << endl;
    dfile << " " << "BKD_h15: 0x" << hex << BKD_h15 << dec << endl;
    dfile << " " << "BKD_h16: 0x" << hex << BKD_h16 << dec << endl;
    dfile << " " << "BKD_h17: 0x" << hex << BKD_h17 << dec << endl;
    dfile << " " << "BKD_h18: 0x" << hex << BKD_h18 << dec << endl;
    dfile << " " << "BKD_h19: 0x" << hex << BKD_h19 << dec << endl;
    dfile << " " << "BKD_h20: 0x" << hex << BKD_h20 << dec << endl;
    dfile << " " << "BKD_h21: 0x" << hex << BKD_h21 << dec << endl;
    dfile << " " << "BKD_h22: 0x" << hex << BKD_h22 << dec << endl;
    dfile << " " << "BKD_h23: 0x" << hex << BKD_h23 << dec << endl;
    dfile << " " << "BKD_h24: 0x" << hex << BKD_h24 << dec << endl;
    dfile << " " << "BKD_h25: 0x" << hex << BKD_h25 << dec << endl;
    dfile << " " << "BKD_h26: 0x" << hex << BKD_h26 << dec << endl;
    dfile << " " << "BKD_h27: 0x" << hex << BKD_h27 << dec << endl;
    dfile << " " << "BKD_h28: 0x" << hex << BKD_h28 << dec << endl;
    dfile << " " << "BKD_h29: 0x" << hex << BKD_h29 << dec << endl;
    dfile << " " << "BKD_h30: 0x" << hex << BKD_h30 << dec << endl;
    dfile << " " << "BKD_h31: 0x" << hex << BKD_h31 << dec << endl;
    dfile << " " << "BKD_h32: 0x" << hex << BKD_h32 << dec << endl;
    dfile << " " << "BKD_h33: 0x" << hex << BKD_h33 << dec << endl;
    dfile << " " << "BKD_h34: 0x" << hex << BKD_h34 << dec << endl;
    dfile << " " << "BKD_h35: 0x" << hex << BKD_h35 << dec << endl;
    dfile << " " << "BKD_h36: 0x" << hex << BKD_h36 << dec << endl;
    dfile << " " << "BKD_h37: 0x" << hex << BKD_h37 << dec << endl;
    dfile << " " << "BKD_h38: 0x" << hex << BKD_h38 << dec << endl;
    dfile << " " << "BKD_h39: 0x" << hex << BKD_h39 << dec << endl;
    dfile << " " << "BKD_h40: 0x" << hex << BKD_h40 << dec << endl;
    dfile << endl;
   }

 // validate BKD. header
 if(BKD_h01 != 0x424BD00D) return error("Invalid BKD.");

 // skip past secondary string table and move to next 0x10-byte aligned offset
 // note that there are a few special cases to handle here when there is no
 // secondary string table and when the table size is already 0x10-byte aligned
 char sstable[2048];
 if(BKD_h39 == 0) ifile.read(&sstable[0], 0x10);
 else if(BKD_h39 < 0x800) {
    ifile.read(&sstable[0], ((BKD_h39 + 0x0F) & (~0x0F)));
    if(!(BKD_h39 % 0x10)) ifile.seekg(0x10, ios::cur);
   }
 else return error("Secondary string table length is too large.");

 // read BKD_h18 data
 if(BKD_h18 == 1) {
    uint32 ET_h01 = LE_read_uint32(ifile); // ???
    uint32 ET_h02 = LE_read_uint32(ifile); // ???
    real32 ET_h03 = LE_read_real32(ifile); // vector??? 
    real32 ET_h04 = LE_read_real32(ifile); // vector??? 
    real32 ET_h05 = LE_read_real32(ifile); // vector??? 
    real32 ET_h06 = LE_read_real32(ifile); // vector??? 
    uint32 ET_h07 = LE_read_uint32(ifile); // 0x00
    uint32 ET_h08 = LE_read_uint32(ifile); // 0x00
   }
 else if(BKD_h18 > 1)
    return error("Invalid BKD_h18 value.");

 // read 0x28-byte entry table
 // don't even bother saving the data because it doesn't appear important
 // for extracting the model data
 for(uint32 i = 0; i < BKD_h19; i++) {
     char entry[0x28];
     ifile.read(&entry[0], 0x28);
    }

 // move to next 0x10 aligned position
 position = (uint32)ifile.tellg();
 position = ((position + 0x0F) & (~0x0F));
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");

 // read indices
 boost::shared_array<uint32> indexdata;
 if(!(BKD_h04 < 0x800)) return error("Unexpected number of index entries.");
 else if(BKD_h04 != 0) {
    indexdata.reset(new uint32[BKD_h04]);
    LE_read_array(ifile, indexdata.get(), BKD_h04);
    if(ifile.fail()) return error("Read failure.");
   }

 // read model information
 if(debug) {
    dfile << "-----------------" << endl;
    dfile << "MODEL INFORMATION" << endl;
    dfile << "-----------------" << endl;
    dfile << endl;
   }

 // skip 0x00 and scan for 0x09
 // it is so unpredictable where the next 0x09 is located at
 // sometimes it is 0x08-byte aligned and sometimes it is 0x10-byte aligned
 for(;;) {
     uint32 temp = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     else if(temp == 0x00) continue;
     else if(temp == 0x09) break;
     else {
        stringstream ss;
        ss << "Unexpected padding value at 0x" << hex << ((uint32)ifile.tellg() - 4) << dec;
        return error(ss.str().c_str());
       }
    }

 // save offset to 0x09
 uint32 mistart = (uint32)ifile.tellg();
 mistart -= 0x04;

 // read model identifier
 uint32 modelID = LE_read_uint32(ifile);
 if(modelID == 0) return error("Invalid model identifier.");
 if(debug) dfile << "modelID: 0x" << hex << modelID << dec << endl;

 // read size of model information section
 uint32 mis_size = LE_read_uint32(ifile);
 if(mis_size < 0x10) return error("Invalid model information data.");

 // read model information header
 uint32 MID_h01 = LE_read_uint32(ifile); // size of gcmesh_pc file
 uint32 MID_h02 = LE_read_uint32(ifile); // 0x03
 uint32 MID_h03 = LE_read_uint32(ifile); // number of 0x30 byte data section after byte sequence
 uint32 MID_h04 = LE_read_uint32(ifile); // offset from 0x09 to 0x30 byte data section
 uint32 MID_h05 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h06 = LE_read_uint32(ifile); // number of vertex buffers
 uint32 MID_h07 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h08 = LE_read_uint32(ifile); // 0x80 offset to end of header, start of vertex data
 uint32 MID_h09 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h10 = LE_read_uint32(ifile); // number of indices
 uint32 MID_h11 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h12 = LE_read_uint32(ifile); // offset to index data
 uint32 MID_h13 = LE_read_uint32(ifile); // 0x00
 uint16 MID_h14 = LE_read_uint16(ifile); // 0x02
 uint16 MID_h15 = LE_read_uint16(ifile); // number of surfaces
 uint32 MID_h16 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h17 = LE_read_uint32(ifile); // number of bytes in byte sequence
 uint32 MID_h18 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h19 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h20 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h21 = LE_read_uint32(ifile); // 0x01 or 0x02
 uint32 MID_h22 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h23 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h24 = LE_read_uint32(ifile); // 0x00
 real32 MID_h25 = LE_read_real32(ifile); // real32
 real32 MID_h26 = LE_read_real32(ifile); // real32
 real32 MID_h27 = LE_read_real32(ifile); // real32
 uint32 MID_h28 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h29 = LE_read_uint32(ifile); // 0x00
 uint32 MID_h30 = LE_read_uint32(ifile); // 0x00

 if(debug) {
    dfile << " " << "MID_h01: 0x" << hex << MID_h01 << dec << endl;
    dfile << " " << "MID_h02: 0x" << hex << MID_h02 << dec << endl;
    dfile << " " << "MID_h03: 0x" << hex << MID_h03 << dec << " (number of 0x30-byte items)" << endl;
    dfile << " " << "MID_h04: 0x" << hex << MID_h04 << dec << endl;
    dfile << " " << "MID_h05: 0x" << hex << MID_h05 << dec << endl;
    dfile << " " << "MID_h06: 0x" << hex << MID_h06 << dec << endl;
    dfile << " " << "MID_h07: 0x" << hex << MID_h07 << dec << endl;
    dfile << " " << "MID_h08: 0x" << hex << MID_h08 << dec << endl;
    dfile << " " << "MID_h09: 0x" << hex << MID_h09 << dec << endl;
    dfile << " " << "MID_h10: 0x" << hex << MID_h10 << dec << endl;
    dfile << " " << "MID_h11: 0x" << hex << MID_h11 << dec << endl;
    dfile << " " << "MID_h12: 0x" << hex << MID_h12 << dec << endl;
    dfile << " " << "MID_h13: 0x" << hex << MID_h13 << dec << endl;
    dfile << " " << "MID_h14: 0x" << hex << MID_h14 << dec << endl;
    dfile << " " << "MID_h15: 0x" << hex << MID_h15 << dec << " (number of surfaces)" << endl;
    dfile << " " << "MID_h16: 0x" << hex << MID_h16 << dec << endl;
    dfile << " " << "MID_h17: 0x" << hex << MID_h17 << dec << " (number of joint map bytes)" << endl;
    dfile << " " << "MID_h18: 0x" << hex << MID_h18 << dec << endl;
    dfile << " " << "MID_h19: 0x" << hex << MID_h19 << dec << endl;
    dfile << " " << "MID_h20: 0x" << hex << MID_h20 << dec << endl;
    dfile << " " << "MID_h21: 0x" << hex << MID_h21 << dec << endl;
    dfile << " " << "MID_h22: 0x" << hex << MID_h22 << dec << endl;
    dfile << " " << "MID_h23: 0x" << hex << MID_h23 << dec << endl;
    dfile << " " << "MID_h24: 0x" << hex << MID_h24 << dec << endl;
    dfile << " " << "MID_h25: " << MID_h25 << endl;
    dfile << " " << "MID_h26: " << MID_h26 << endl;
    dfile << " " << "MID_h27: " << MID_h27 << endl;
    dfile << " " << "MID_h28: 0x" << hex << MID_h28 << dec << endl;
    dfile << " " << "MID_h29: 0x" << hex << MID_h29 << dec << endl;
    dfile << " " << "MID_h30: 0x" << hex << MID_h30 << dec << endl;
    dfile << endl;
   }

 // validate 0x30-byte data section information
 if(!MID_h04) return error("There are no 0x30-byte data sections.");
 if(MID_h04 < 0xC) return error("Invalid offset to 0x30-byte data sections.");

 // validate number of vertex buffers
 uint32 n_vertex_buffers = MID_h06;
 if(n_vertex_buffers < 0x01) return error("Invalid number of vertex buffers.");
 if(n_vertex_buffers > 0xFF) return error("Invalid number of vertex buffers.");

 // validate number of surfaces
 uint32 n_surfaces = MID_h15;
 if(n_surfaces < 0x01) return error("Invalid number of surfaces.");
 if(n_surfaces > 0xFF) return error("Invalid number of surfaces.");

 // read vertex buffer information
 if(debug) {
    dfile << "-------------------------" << endl;
    dfile << "VERTEX BUFFER INFORMATION" << endl;
    dfile << "-------------------------" << endl;
    dfile << endl;
   }
 struct VERTEX_BUFFER_INFO {
  uint32 p01; // number of vertices
  uint32 p02; // flags
  uint32 p03; // 0x00
  uint32 p04; // 0x00
  uint32 p05; // offset
  uint32 p06; // 0x00
 };
 deque<VERTEX_BUFFER_INFO> vbinfo;
 for(uint32 i = 0; i < n_vertex_buffers; i++) {
     VERTEX_BUFFER_INFO info;
     info.p01 = LE_read_uint32(ifile);
     info.p02 = LE_read_uint32(ifile);
     info.p03 = LE_read_uint32(ifile);
     info.p04 = LE_read_uint32(ifile);
     info.p05 = LE_read_uint32(ifile);
     info.p06 = LE_read_uint32(ifile);
     vbinfo.push_back(info);
     if(debug) {
        dfile << " item 0x" << hex << i << dec << ":" << endl;
        dfile << " p01 = 0x" << hex << info.p01 << dec << " (vertices)" << endl;
        dfile << " p02 = 0x" << hex << info.p02 << dec << " (flags)" << endl;
        dfile << " p03 = 0x" << hex << info.p03 << dec << " (0x00)" << endl;
        dfile << " p04 = 0x" << hex << info.p04 << dec << " (0x00)" << endl;
        dfile << " p05 = 0x" << hex << info.p05 << dec << " (offset)" << endl;
        dfile << " p06 = 0x" << hex << info.p06 << dec << " (0x00)" << endl;
        dfile << endl;
       }
    }
 if(debug) dfile << endl;

 // read 30-byte items
 if(debug) {
    dfile << "----------------" << endl;
    dfile << " JOINT MAP DATA " << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 if(debug) {
    uint32 position = (uint32)ifile.tellg();
    dfile << "offset = 0x" << hex << position << dec << endl;
    dfile << endl;
   }

 // there is a byte sequence here, typically consisting of incrementing
 // indices; not sure what they are but i think this is the joint map
 boost::shared_array<uint08> jntmap_bytes;
 deque<AMC_JOINTMAP> jmlist;
 if(MID_h17)
   {
    // read joint map bytes
    jntmap_bytes.reset(new uint08[MID_h17]);
    LE_read_array(ifile, jntmap_bytes.get(), MID_h17);
    if(ifile.fail()) return error("Read failure.");

    // size of maps
    deque<uint32> map_sizes;
    if(MID_h21 == 0x00)
       message(" This model has no joint maps!");
    if(MID_h21 == 0x01)
       map_sizes.push_back(MID_h17);
    else if(MID_h21 == 0x02) {
       uint16 v01 = LE_read_uint16(ifile); // 1st map size as 16-bits
       uint08 v02 = LE_read_uint08(ifile); // 2nd map size
       uint08 v03 = LE_read_uint08(ifile); // 1st map size
       map_sizes.push_back(v03);
       map_sizes.push_back(v02);
      }
    else if(MID_h21 == 0x03) {
       uint16 v01 = LE_read_uint16(ifile); // 1st map size as 16-bits
       uint08 v02 = LE_read_uint08(ifile); // 2nd map size
       uint08 v03 = LE_read_uint08(ifile); // 1st map size
       uint08 v04 = LE_read_uint08(ifile); // 3rd map size
       map_sizes.push_back(v03);
       map_sizes.push_back(v02);
       map_sizes.push_back(v04);
      }
    else
       message(" This model has more than 3 joint maps!");

    // assign data to joint maps
    if(MID_h21 == 0x01) {
       AMC_JOINTMAP jm1;
       for(uint32 i = 0; i < map_sizes[0]; i++) jm1.jntmap.push_back(jntmap_bytes[i]);
       jmlist.push_back(jm1);
      }
    else if(MID_h21 == 0x02) {
       AMC_JOINTMAP jm1;
       AMC_JOINTMAP jm2;
       for(uint32 i = 0; i < map_sizes[0]; i++) jm1.jntmap.push_back(jntmap_bytes[i]);
       for(uint32 i = 0; i < map_sizes[1]; i++) jm2.jntmap.push_back(jntmap_bytes[i + map_sizes[0]]);
       jmlist.push_back(jm1);
       jmlist.push_back(jm2);
      }
    else if(MID_h21 == 0x03) {
       AMC_JOINTMAP jm1;
       AMC_JOINTMAP jm2;
       AMC_JOINTMAP jm3;
       for(uint32 i = 0; i < map_sizes[0]; i++) jm1.jntmap.push_back(jntmap_bytes[i]);
       for(uint32 i = 0; i < map_sizes[1]; i++) jm2.jntmap.push_back(jntmap_bytes[i + map_sizes[0]]);
       for(uint32 i = 0; i < map_sizes[2]; i++) jm3.jntmap.push_back(jntmap_bytes[i + map_sizes[0] + map_sizes[1]]);
       jmlist.push_back(jm1);
       jmlist.push_back(jm2);
       jmlist.push_back(jm3);
      }
   }

 if(debug) dfile << endl;

 // read 30-byte items
 if(debug) {
    dfile << "---------------" << endl;
    dfile << "UNKNOWN SECTION" << endl;
    dfile << "---------------" << endl;
    dfile << endl;
   }

 // read 0x30-byte data sections
 ifile.seekg(mistart + MID_h04);
 if(ifile.fail()) return error("Stream seek failure.");
 for(uint16 i = 0; i < MID_h03; i++) {
     char temp[0x30];
     LE_read_array(ifile, &temp[0], 0x30);
     if(debug) {
        for(uint32 j = 0; j < 0x30; j++) {
            uint16 byte = temp[j];
            dfile << setfill('0') << setw(2) << hex << byte << dec;
           }
        dfile << endl;
       }
    }
 if(debug) dfile << endl;

 // model container
 ADVANCEDMODELCONTAINER amc;

 // set joint maps if present
 if(jmlist.size()) amc.jmaplist = jmlist;

 // read surface information
 if(debug) {
    dfile << "--------" << endl;
    dfile << "SURFACES" << endl;
    dfile << "--------" << endl;
    dfile << endl;
   }

 // surface information
 typedef map<uint16, deque<AMC_REFERENCE>>::value_type smvt;
 map<uint16, deque<AMC_REFERENCE>> surface_map;

 // read surface information
 for(uint16 i = 0; i < n_surfaces; i++)
    {
     uint16 s01 = LE_read_uint16(ifile); // id
     uint16 s02 = LE_read_uint16(ifile); // vertex buffer index
     uint32 s03 = LE_read_uint32(ifile);
     uint32 s04 = LE_read_uint32(ifile);
     uint32 s05 = LE_read_uint32(ifile);
     uint32 s06 = LE_read_uint32(ifile);

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
      ref.vb_start = 0;   // vbuffer start index (use whole vertex buffer)
      ref.vb_width = vbinfo[s02].p01; // s06; // number of vertices
      ref.ib_index = 0;   // ibuffer index (only one index buffer)
      ref.ib_start = s03; // ibuffer start index
      ref.ib_width = s04; // number of indices
      ref.jm_index = AMC_INVALID_JMAP_INDEX;

     // insert reference
     iter->second.push_back(ref);
    }

 // read joint map information
 if(debug) {
    dfile << "----------------------" << endl;
    dfile << " JOINT MAP REFERENCES " << endl;
    dfile << "----------------------" << endl;
    dfile << endl;
   }

 // read joint map references (0x8 bytes per surface)
 // this data comes immediately after the surface information
 // the first number is the reference, the second number is 0
 for(uint32 i = 0; i < MID_h15; i++)
    {
     // read joint map reference
     uint32 p01 = LE_read_uint32(ifile); // reference
     uint32 p02 = LE_read_uint32(ifile); // 0x00
     if(debug) dfile << "0x" << hex << p01 << dec << ", 0x" << hex << p02 << dec << endl;

     // set joint map references for all surface references
     auto iter = surface_map.find(i);
     if(iter != surface_map.end()) {
        for(uint32 j = 0; j < iter->second.size(); j++)
            iter->second[j].jm_index = p01;
       }
    }

 if(debug) dfile << endl;

 // and then finally, we run into the model ID again!
 uint32 modelID_copy = LE_read_uint32(ifile);
 // validate ID again?

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
 STDSTRINGSTREAM ss;
 ss << pathname;
 ss << shrtname;
 ss << TEXT(".gcmesh_pc");

 // open mesh file
 ifile.close();
 ifile.open(ss.str().c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read face information
 if(debug) {
    dfile << "----------------" << endl;
    dfile << "FACE INFORMATION" << endl;
    dfile << "----------------" << endl;
    dfile << endl;
   }

 // move to index buffer
 ifile.seekg(MID_h12);
 if(ifile.fail()) return error("Index buffer seek failure.");

 // index buffer properties
 uint32 n_faces = MID_h10;
 uint32 bytes_per_index = MID_h14;
 if(bytes_per_index != 0x02) return error("Expecting 0x02 byte indices.");

 // read index buffer
 uint32 ibsize = n_faces*bytes_per_index;
 boost::shared_array<char> ibdata(new char[ibsize]);
 ifile.read(ibdata.get(), ibsize);

 // save index buffer
 AMC_IDXBUFFER amc_ib;
 amc_ib.format = AMC_UINT16;
 amc_ib.type = AMC_TRISTRIP;
 amc_ib.wind = AMC_CW;
 amc_ib.name = "none";
 amc_ib.elem = n_faces; // n_faces is actually number of indices
 amc_ib.data = ibdata;
 amc.iblist.push_back(amc_ib);

 // read vertex information
 if(debug) {
    dfile << "--------------" << endl;
    dfile << "VERTEX BUFFERS" << endl;
    dfile << "--------------" << endl;
    dfile << endl;
   }

 // DELETE ME BEGIN!!!
 STDSTRINGSTREAM binss;
 binss << pathname << shrtname << TEXT(".vbin");
 ofstream binfile(binss.str().c_str(), ios::binary);
 // DELETE ME END!!!

 // for each vertex buffer
 for(uint32 i = 0; i < n_vertex_buffers; i++)
    {
     // vertex buffer information
     VERTEX_BUFFER_INFO vbi = vbinfo[i];
     if(debug) dfile << "Vertex Buffer #" << i << endl;

     // move to vertex buffer
     ifile.seekg(vbi.p05);
     if(ifile.fail()) return error("Vertex buffer seek failure.");

     // vertex buffer properties
     uint32 n_vertices = vbi.p01;
     uint32 vertex_bytes = (vbi.p02 & 0xFF);
     uint32 vertex_type = ((vbi.p02 & 0xFF00) >> 8);
     if(debug) dfile << " vertices = " << n_vertices << endl;
     if(debug) dfile << " vertex size = " << vertex_bytes << endl;
     if(debug) dfile << " vertex type = " << vertex_type << endl;

     // read vertex buffer
     uint32 vbsize = n_vertices*vertex_bytes;
     boost::shared_array<char> vbdata(new char[vbsize]);
     ifile.read(vbdata.get(), vbsize);
     if(ifile.fail()) return error("Vertex buffer read failure.");

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

     // multiple UV channels
     if(vertex_bytes == 0x24) {
        if(vertex_type == 0x01) {
           amc_vb.uvchan = 3;
           amc_vb.uvtype[1] = AMC_DIFFUSE_MAP;
           amc_vb.uvtype[2] = AMC_DIFFUSE_MAP;
          }
        else if(vertex_type == 0x03) {
           amc_vb.uvchan = 2;
           amc_vb.uvtype[1] = AMC_DIFFUSE_MAP;
          }
       }
     else if(vertex_bytes == 0x28) {
        if(vertex_type == 0x03) {
           amc_vb.uvchan = 2;
           amc_vb.uvtype[1] = AMC_DIFFUSE_MAP;
          }
       }
     else if(vertex_bytes == 0x2C) {
        if(vertex_type == 0x03) {
           amc_vb.uvchan = 4;
           amc_vb.uvtype[1] = AMC_DIFFUSE_MAP;
           amc_vb.uvtype[2] = AMC_DIFFUSE_MAP;
           amc_vb.uvtype[3] = AMC_DIFFUSE_MAP;
          }
       }

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
               real32 u1 = bs.LE_read_sint16()/1023.0f; // unknown
               real32 u2 = bs.LE_read_sint16()/1023.0f; // unknown
               real32 u3 = bs.LE_read_sint16()/1023.0f; // unknown
               real32 u4 = bs.LE_read_sint16()/1023.0f; // unknown

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
               amc_vb.data[j].uv[1][0] = u1;
               amc_vb.data[j].uv[1][1] = u2;
               amc_vb.data[j].uv[2][0] = u3;
               amc_vb.data[j].uv[2][1] = u4;
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
               amc_vb.data[j].uv[1][0] = v1;
               amc_vb.data[j].uv[1][1] = v2;
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
               amc_vb.data[j].uv[1][0] = v1;
               amc_vb.data[j].uv[1][1] = v2;
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
               amc_vb.data[j].uv[1][0] = v1;
               amc_vb.data[j].uv[1][1] = v2;
               amc_vb.data[j].uv[2][0] = v3;
               amc_vb.data[j].uv[2][1] = v4;
               amc_vb.data[j].uv[3][0] = v5;
               amc_vb.data[j].uv[3][1] = v6;
              }
            else
               return error("Incorrect 0x2C vertex type.");
           }
         else {
            stringstream ss;
            ss << "Unimplemented vertex type 0x" << hex << vertex_bytes << dec << ".";
            return error(ss.str().c_str());
           }

         // DELETE ME BEGIN!!!
         binfile.write((char*)&amc_vb.data[j].vx, 4);
         binfile.write((char*)&amc_vb.data[j].vy, 4);
         binfile.write((char*)&amc_vb.data[j].vz, 4);
         // DELETE ME END!!!
        }

     // insert vertex buffer
     amc.vblist.push_back(amc_vb);
     if(debug) dfile << endl;
    }

 // open rig file
 ss.str(TEXT(""));
 ss << pathname << shrtname << TEXT(".rig_pc");
 ifile.close();
 ifile.open(ss.str().c_str(), ios::binary);

 if(ifile.is_open())
   {
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
    if(ifile.fail()) return error("Seek failure.");
       
    // read rig section #1
    for(uint32 i = 0; i < rig_h01; i++) {
        SRTSKELINFO1 info;
        info.p01 = LE_read_uint32(ifile);
        ss1.push_back(info);
       }

    // initialize skeleton
    AMC_SKELETON2 skel;
    skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
    skel.tiplen = 0.25f;
    skel.name = "skeleton";
   
    // move to rig section #2
    position = (uint32)ifile.tellg();
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
   }
 else
   {
    // oh we gotta do something so we don't set the joint map!
   }

 // save AMC
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), transformed);

 return true;
}

bool processCMRP(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug file
 std::ofstream dfile;
 bool debug = true;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT(".txt"); 
    dfile.open(ss.str().c_str(), ios::binary);
    if(!dfile) return error("Failed to create debug file.");
   }

 // read 0x20-byte header
 uint32 h01 = LE_read_uint32(ifile); // 0x1337BEEF
 uint32 h02 = LE_read_uint32(ifile); // 0x00000005 (should we test?)
 uint32 h03 = LE_read_uint32(ifile); // 0x00000000
 uint32 h04 = LE_read_uint32(ifile); // 0x00000000
 uint32 h05 = LE_read_uint32(ifile); // 0x0BADBEEF
 uint32 h06 = LE_read_uint32(ifile); // 0x00000003 (should we test?)
 uint32 h07 = LE_read_uint32(ifile); // 0x00000001 (should we test?)
 uint32 h08 = LE_read_uint32(ifile); // number of morphs

 // validate header
 if(h01 != 0x1337BEEF) return error("Expecting 0x1337BEEF.");
 if(h05 != 0x0BADBEEF) return error("Expecting 0x0BADBEEF.");
 if(h08 == 0x00) return message("This cmorph_pc file contains no morphs.");

 // 
 struct SRIVMORPHVERTEX {
  uint16 index;
  real32 vx;
  real32 vy;
  real32 vz;
  real32 nx;
  real32 ny;
  real32 nz;  
 };

 // read 0x10-byte items
 struct SRIVMORPHITEM {
  public :
   uint32 p01; // 0x00000000
   uint32 p02; // 0x00000000
   sint16 p03a; // id
   sint16 p03b; // id
   uint32 p04; // 0x00000001
  public :
   uint32 p05; // 0x00000000
   uint32 p06; // 0x00000000
   uint32 p07; // 0x00000000
   uint16 p08; // number of morphed vertices
   uint16 p09; // largest vertex index used
   real32 p10; // ???
   real32 p11; // ???
   real32 p12; // ???
   real32 p13; // ???
   real32 p14; // ???
   real32 p15; // ???
  public :
   boost::shared_array<SRIVMORPHVERTEX> p16;
 };
 boost::shared_array<SRIVMORPHITEM> morphlist(new SRIVMORPHITEM[h08]);
 for(uint32 i = 0; i < h08; i++) {
     morphlist[i].p01 = LE_read_uint32(ifile);
     morphlist[i].p02 = LE_read_uint32(ifile);
     morphlist[i].p03a = LE_read_sint16(ifile);
     morphlist[i].p03b = LE_read_sint16(ifile);
     morphlist[i].p04 = LE_read_uint32(ifile);     
    }

 // DELETE ME BEGIN!!!
 STDSTRINGSTREAM binss;
 binss << pathname << shrtname << TEXT(".vbin");
 ofstream binfile(binss.str().c_str(), ios::binary);
 // DELETE ME END!!!

 // read 0x28-byte items
 for(uint32 i = 0; i < h08; i++) {
	 morphlist[i].p05 = LE_read_uint32(ifile);
	 morphlist[i].p06 = LE_read_uint32(ifile);
	 morphlist[i].p07 = LE_read_uint32(ifile);
	 morphlist[i].p08 = LE_read_uint16(ifile);
	 morphlist[i].p09 = LE_read_uint16(ifile);
     morphlist[i].p10 = LE_read_real32(ifile);
     morphlist[i].p11 = LE_read_real32(ifile);
     morphlist[i].p12 = LE_read_real32(ifile);
     morphlist[i].p13 = LE_read_real32(ifile);
     morphlist[i].p14 = LE_read_real32(ifile);
     morphlist[i].p15 = LE_read_real32(ifile);
     if(debug) {
        dfile << " " << morphlist[i].p10 << ",";
        dfile << " " << morphlist[i].p11 << ",";
        dfile << " " << morphlist[i].p12 << ",";
        dfile << " " << morphlist[i].p13 << ",";
        dfile << " " << morphlist[i].p14 << ",";
        dfile << " " << morphlist[i].p15 << endl;
       }
	}
 if(debug) dfile << endl;

 // read morph data blocks
 for(uint32 i = 0; i < h08; i++)
    {
     //
     if(debug) {
        dfile << "MORPH DATA BLOCK 0x: " << hex << i << dec << endl;
        dfile << endl;
       }

     // seek 0x10-aligned position
     uint32 position = (uint32)ifile.tellg();
     position = ((position + 0x0F) & (~0x0F));
	 ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read morphed vertices
     morphlist[i].p16.reset(new SRIVMORPHVERTEX[morphlist[i].p08]);
	 for(uint32 j = 0; j < morphlist[i].p08; j++)
	    {
         // read morph value
		 real32 vx = ( LE_read_sint16(ifile) - (real32)(morphlist[i].p03a))/(real32)(morphlist[i].p03b);
		 real32 vy = ( LE_read_sint16(ifile) - (real32)(morphlist[i].p03a))/(real32)(morphlist[i].p03b);
		 real32 vz = ( LE_read_sint16(ifile) - (real32)(morphlist[i].p03a))/(real32)(morphlist[i].p03b);
		 uint16 index = LE_read_uint16(ifile);
		 real32 nx = (LE_read_uint08(ifile) - 127.0f)/128.0f;
		 real32 ny = (LE_read_uint08(ifile) - 127.0f)/128.0f;
		 real32 nz = (LE_read_uint08(ifile) - 127.0f)/128.0f;
		 real32 nw = (LE_read_uint08(ifile) - 127.0f)/128.0f;

         // set morph value
		 morphlist[i].p16[j].vx = vx;
		 morphlist[i].p16[j].vy = vy;
		 morphlist[i].p16[j].vz = vz;
		 morphlist[i].p16[j].index = index;
		 morphlist[i].p16[j].nx = nx;
		 morphlist[i].p16[j].ny = ny;
		 morphlist[i].p16[j].nz = nz;

         // print morph value
         if(debug) {
            dfile << " 0x" << setfill('0') << setw(4) << hex << j << dec << ":";
            dfile << " 0x" << setfill('0') << setw(4) << hex << index << dec << ":";
            dfile << " <" << vx << ", " << vy << ", " << vz << "> -";
            dfile << " <" << nx << ", " << ny << ", " << nz << ">" << endl;
           }

         // DELETE ME BEGIN!!!
         binfile.write((char*)&vx, 4);
         binfile.write((char*)&vy, 4);
         binfile.write((char*)&vz, 4);
         // DELETE ME END!!!
		}

     // separator
     if(debug) dfile << endl;
    }

 return true;
}

};};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Saints Row IV");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Copy all VPP_PC files to a new folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where VPP_PC files are at.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~??? GB free space.\n");
 p3 += TEXT("3. ??? GB for game + ??? GB extraction.\n");

 // insert title
 AddGameTitle(p1.c_str(), p2.c_str(), p3.c_str(), extract);
 return true;
}

bool extract(void)
{
 auto pathname = GetModulePathname();
 if(!pathname) return error("GetModulePathname() failed.", __LINE__);
 return extract(pathname.get());
}

bool extract(LPCTSTR pname)
{
 // set pathname
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // booleans
 // variables
 bool doVPP = false;
 bool doSTR = false;
 bool doCPG = false;
 bool doCVB = false;
 bool doMSH = false;
 bool doMRP = false;

 // questions
 if(YesNoBox("Process VPP_PC (archive) files?"))  doVPP = true;
 if(YesNoBox("Process STR2_PC (archive) files?")) doSTR = true;
 if(YesNoBox("Process CPEG_PC (texture) files?")) doCPG = true;
 if(YesNoBox("Process CVBM_PC (texture) files?")) doCVB = true;
 if(YesNoBox("Process CCMESH_PC (model) files?")) doMSH = true;
 if(YesNoBox("Process CMORPH_PC (morph) files?")) doMRP = true;

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process archive
 cout << "STAGE 0" << endl;
 if(doVPP) {
    cout << "Processing .VPP_PC files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".VPP_PC"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processVPP2(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 1" << endl;
 if(doSTR) {
    cout << "Processing .STR2_PC files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".STR2_PC"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processSTR2(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 2" << endl;
 if(doCPG) {
    cout << "Processing .CPEG_PC files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".CPEG_PC"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCPEG(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 3" << endl;
 if(doCVB) {
    cout << "Processing .CVBM_PC files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".CVBM_PC"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCPEG(filelist[i].c_str(), CVBM_TEXTURE)) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 4" << endl;
 if(doMSH) {
    cout << "Processing .CCMESH_PC files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".CCMESH_PC"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCMSH(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archive
 if(doMRP) {
    cout << "STAGE 5" << endl;
    cout << "Processing .CMORPH_PC files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".CMORPH_PC"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCMRP(filelist[i].c_str())) return false;
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

