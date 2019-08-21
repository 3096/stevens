#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_xbox360.h"
#include "ps3_mgrr.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   MGRR

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processDAT(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open DAT file.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return message("Empty DAT file; skipping file.");

 // read magic number (it is safe to delete if not a DAT file)
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 
 if(magic != 0x44415400) {
    ifile.close();
    DeleteFileA(filename.c_str());
    return message("Not a DAT file; skipping file.");
   }

 // number of files (it is safe to delete if not a DAT file)
 uint32 files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 
 if(!files) {
    ifile.close();
    DeleteFileA(filename.c_str());
    return message("DAT file contains no files; skipping file.");
   }

 // offset to locations table
 uint32 locations_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // offset to extension table
 uint32 extensions_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // offset to filenames table
 uint32 filenames_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // offset to filesizes table
 uint32 filesizes_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // offset to unknown table
 uint32 unknown_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read locations
 ifile.seekg(locations_offset);
 if(ifile.fail()) return error("Seek failure.");
 boost::shared_array<uint32> locations(new uint32[files]);
 if(!BE_read_array(ifile, locations.get(), files)) return error("Read failure.");

 // read extensions
 ifile.seekg(extensions_offset);
 if(ifile.fail()) return error("Seek failure.");
 boost::shared_array<uint32> extensions(new uint32[files]);
 if(!BE_read_array(ifile, extensions.get(), files)) return error("Read failure.");

 // seek filenames table
 ifile.seekg(filenames_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read filename length
 uint32 filename_length = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(filename_length < 2) return error("Invalid filename length.");

 // create a set of filenames to keep track of duplicate names
 map<string, uint32> namemap;

 // read filenames
 boost::shared_array<string> filenames(new string[files]);
 for(uint32 i = 0; i < files; i++)
    {
     // read filename
     char buffer[1024];
     ifile.read(&buffer[0], filename_length);
     string currname = buffer;

     // name is a duplicate
     auto iter = namemap.find(currname);
     if(iter != namemap.end())
       {
        // increment count
        uint32 count = iter->second;
        count++;
        iter->second = count;

        // create filename
        stringstream ss;
        ss << GetShortFilenameWithoutExtension(currname);
        ss << "_";
        ss << count;
        ss << GetExtensionFromFilename(currname);

        // rename
        currname = ss.str();
       }
     // name is not a duplicate
     else {
        typedef map<string, uint32>::value_type value_type;
        namemap.insert(value_type(currname, 0));
       }

     // set filename
     filenames[i] = currname;
    }

 // read filesizes
 ifile.seekg(filesizes_offset);
 if(ifile.fail()) return error("Seek failure.");
 boost::shared_array<uint32> filesizes(new uint32[files]);
 if(!BE_read_array(ifile, filesizes.get(), files)) return error("Read failure.");

 // create directory to store files
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // process files
 for(uint32 i = 0; i < files; i++)
    {
     // move to location
     ifile.seekg(locations[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data (if possible)
     if(!filesizes[i]) continue;
     boost::shared_array<char> data(new char[filesizes[i]]);
     ifile.read(data.get(), filesizes[i]);
     if(ifile.fail()) return error("Read failure."); 

     // create file
     stringstream ss;
     ss << savepath << filenames[i];
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save file
     ofile.write(data.get(), filesizes[i]);
     if(ofile.fail()) return error("Write failure."); 
    }

 // close and delete DAT file (so we don't process again)
 ifile.close();
 DeleteFileA(filename.c_str());
 return true;
}

bool processDTT(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open DTT file.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 datsize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(datsize == 0) {
    ifile.close();
    DeleteFileA(filename.c_str());
    return message("Empty DTT file; skipping file.");
   }

 // read magic number
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 
 if(magic != 0x44415400) {
    ifile.close();
    DeleteFileA(filename.c_str());
    return message("Not a DTT file; skipping file.");
   }

 // number of files
 uint32 files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 
 if(!files) return message("DAT file contains no files; skipping file.");

 // offset to locations table
 uint32 locations_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // offset to extension table
 uint32 extensions_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // offset to filenames table
 uint32 filenames_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // offset to filesizes table
 uint32 filesizes_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // offset to unknown table
 uint32 unknown_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read locations
 ifile.seekg(locations_offset);
 if(ifile.fail()) return error("Seek failure.");
 boost::shared_array<uint32> locations(new uint32[files]);
 if(!BE_read_array(ifile, locations.get(), files)) return error("Read failure.");

 // read extensions
 ifile.seekg(extensions_offset);
 if(ifile.fail()) return error("Seek failure.");
 boost::shared_array<uint32> extensions(new uint32[files]);
 if(!BE_read_array(ifile, extensions.get(), files)) return error("Read failure.");

 // seek filenames table
 ifile.seekg(filenames_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read filename length
 uint32 filename_length = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(filename_length < 2) return error("Invalid filename length.");

 // create a set of filenames to keep track of duplicate names
 map<string, uint32> namemap;

 // read filenames
 boost::shared_array<string> filenames(new string[files]);
 for(uint32 i = 0; i < files; i++)
    {
     // read filename
     char buffer[1024];
     ifile.read(&buffer[0], filename_length);
     string currname = buffer;

     // name is a duplicate
     auto iter = namemap.find(currname);
     if(iter != namemap.end())
       {
        // increment count
        uint32 count = iter->second;
        count++;
        iter->second = count;

        // create filename
        stringstream ss;
        ss << GetShortFilenameWithoutExtension(currname);
        ss << "_";
        ss << count;
        ss << GetExtensionFromFilename(currname);

        // rename
        currname = ss.str();
       }
     // name is not a duplicate
     else {
        typedef map<string, uint32>::value_type value_type;
        namemap.insert(value_type(currname, 0));
       }

     // set filename
     filenames[i] = currname;
    }

 // read filesizes
 ifile.seekg(filesizes_offset);
 if(ifile.fail()) return error("Seek failure.");
 boost::shared_array<uint32> filesizes(new uint32[files]);
 if(!BE_read_array(ifile, filesizes.get(), files)) return error("Read failure.");

 // create directory to store files
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // process files
 for(uint32 i = 0; i < files; i++)
    {
     // move to location
     ifile.seekg(locations[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data (if possible)
     if(!filesizes[i]) continue;
     boost::shared_array<char> data(new char[filesizes[i]]);
     ifile.read(data.get(), filesizes[i]);
     if(ifile.fail()) return error("Read failure."); 

     // create file
     stringstream ss;
     ss << savepath << filenames[i];
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save file
     ofile.write(data.get(), filesizes[i]);
     if(ofile.fail()) return error("Write failure."); 
    }

 // close and delete DTT file (so we don't process again)
 ifile.close();
 DeleteFileA(filename.c_str());
 return true;
}

bool processSCR(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open WTB file.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return message("Empty SCR file; skipping file.");

 // read magic number
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 
 if(magic != 0x53435200) return message("Not an SCR file; skipping file.");

 // read unknown
 uint16 unk01 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // read number of files
 uint16 n_models = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // read offset to list
 uint32 list_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 

 // move to offsets
 ifile.seekg(list_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < n_models; i++) {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure."); 
     offsets.push_back(item);
    }

 // for each offset
 for(uint32 i = 0; i < n_models; i++)
    {
     // move to data
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read offset to WMB
     uint32 WMB_offset = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read filename
     char buffer[1024];
     if(!read_string(ifile, buffer, 1024)) return error("Read failure.");

     // compute size of WMB data
     uint32 WMB_size = 0;
     if(i == (n_models - 1)) WMB_size = filesize - WMB_offset;
     else WMB_size = offsets[i + 1] - WMB_offset;

     // move to WMB data
     ifile.seekg(WMB_offset);
     if(ifile.fail()) return error("Seek failure.");

     // read WMB data
     boost::shared_array<char> WMB_data(new char[WMB_size]);
     ifile.read(WMB_data.get(), WMB_size);
     if(ifile.fail()) return error("Read failure.");

     // create output file
     stringstream ss;
     ss << pathname << buffer << ".wmb";
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save WMB data
     ofile.write(WMB_data.get(), WMB_size);
     if(ofile.fail()) return error("Write failure.");
    }

 // close and delete SCR file (so we don't process again)
 ifile.close();
 DeleteFileA(filename.c_str());
 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processWTA(const std::string& filename)
{
 // open WTA file
 ifstream wtafile(filename.c_str(), ios::binary);
 if(!wtafile) return error("Failed to open WTA file.");

 // open WTP file
 stringstream ss;
 ss << GetPathnameFromFilename(filename);
 ss << GetShortFilenameWithoutExtension(filename);
 ss << ".wtp";
 string WTP_filename = ss.str();
 ifstream wtpfile(ss.str().c_str(), ios::binary);
 if(!wtpfile) {
    message("Failed to open WTP file. Some WTA files do not have WTP files in the same directory.");
    return true;
   }

 // size of WTP file
 wtpfile.seekg(0, ios::end);
 uint32 wtpsize = (uint32)wtpfile.tellg();
 wtpfile.seekg(0, ios::beg);

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // read magic
 uint32 magic = BE_read_uint32(wtafile);
 if(wtafile.fail()) return error("Read failure.");
 if(magic != 0x425457) return error("Invalid WTA file.");

 // read unknown
 uint32 unk01 = BE_read_uint32(wtafile);
 if(wtafile.fail()) return error("Read failure.");

 // read number of files
 uint32 n_files = BE_read_uint32(wtafile);
 if(wtafile.fail()) return error("Read failure.");
 if(!n_files) return true; // do nothing

 // read offset to texture offsets
 uint32 offset1 = BE_read_uint32(wtafile);
 if(wtafile.fail()) return error("Read failure.");

 // read offset to texture sizes
 uint32 offset2 = BE_read_uint32(wtafile);
 if(wtafile.fail()) return error("Read failure.");

 // read offset to flags?
 uint32 offset3 = BE_read_uint32(wtafile);
 if(wtafile.fail()) return error("Read failure.");

 // read offset to texture name hashes (identifiers)
 uint32 offset4 = BE_read_uint32(wtafile);
 if(wtafile.fail()) return error("Read failure.");

 // read offset to XPR2 texture information
 uint32 offset5 = BE_read_uint32(wtafile);
 if(wtafile.fail()) return error("Read failure.");

 // read texture offsets
 wtafile.seekg(offset1);
 if(wtafile.fail()) return error("Seek failure.");
 vector<uint32> list1(n_files);
 for(uint32 i = 0; i < n_files; i++) {
     list1[i] = BE_read_uint32(wtafile);
     if(wtafile.fail()) return error("Read failure.");
    }

 // read texture sizes
 wtafile.seekg(offset2);
 if(wtafile.fail()) return error("Seek failure.");
 vector<uint32> list2(n_files);
 for(uint32 i = 0; i < n_files; i++) {
     list2[i] = BE_read_uint32(wtafile);
     if(wtafile.fail()) return error("Read failure.");
    }

 // read XPR2 texture information
 wtafile.seekg(offset5);
 if(wtafile.fail()) return error("Seek failure.");
 uint32 infosize = n_files*0x34;
 boost::shared_array<char> infodata(new char[infosize]);
 if(!BE_read_array(wtafile, infodata.get(), infosize)) return error("Read failure.");

 // create save path
 stringstream savepath;
 savepath << GetPathnameFromFilename(filename);
 savepath << GetShortFilenameWithoutExtension(filename);
 savepath << "\\";
 CreateDirectoryA(savepath.str().c_str(), NULL);

 // for each texture file
 for(uint32 i = 0; i < n_files; i++)
    {
     // create XPR2 file
     stringstream ss;
     ss << savepath.str() << setfill('0') << setw(4) << i;
     ss << ".xpr";
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create XPR file.");
 
     // save XPR2
     BE_write_uint32(ofile, 0x58505232);
     if(ofile.fail()) return error("Write failure.");
     
     // save XPR2 data offset
     BE_write_uint32(ofile, 0x800);
     if(ofile.fail()) return error("Write failure.");
 
     // save XPR2 total texture size
     BE_write_uint32(ofile, list2[i]);
     if(ofile.fail()) return error("Write failure.");
     
     // save XPR2 number of textures
     BE_write_uint32(ofile, 0x01);
     if(ofile.fail()) return error("Write failure.");
 
     // save D2XT
     BE_write_uint32(ofile, 0x54583244);
     if(ofile.fail()) return error("Write failure.");
 
     // save offset from 0xC to texture information
     BE_write_uint32(ofile, 0x20);
     if(ofile.fail()) return error("Write failure.");
 
     // save size of texture info
     BE_write_uint32(ofile, 0x34);
     if(ofile.fail()) return error("Write failure.");
 
     // save offset from 0xC to filename information
     BE_write_uint32(ofile, 0x18);
     if(ofile.fail()) return error("Write failure.");
 
     // save 0x00000000
     BE_write_uint32(ofile, 0x00);
     if(ofile.fail()) return error("Write failure.");
 
     // save filename
     stringstream fname;
     fname << setfill('0') << setw(4) << i;
     char temp[8];
     temp[0] = fname.str()[0];
     temp[1] = fname.str()[1];
     temp[2] = fname.str()[2];
     temp[3] = fname.str()[3];
     temp[4] = 0;
     temp[5] = 0;
     temp[6] = 0;
     temp[7] = 0;
     BE_write_array(ofile, &temp[0], 8);
     if(ofile.fail()) return error("Write failure.");
 
     // seek XPR2 texture information
     wtafile.seekg((offset5 + (i * 0x34)));
     if(wtafile.fail()) return error("Seek failure.");
 
     // read XPR2 texture information
     uint32 p01 = BE_read_uint32(wtafile); // 0x00
     uint32 p02 = BE_read_uint32(wtafile); // 0x04
     uint32 p03 = BE_read_uint32(wtafile); // 0x08
     uint32 p04 = BE_read_uint32(wtafile); // 0x0C
     uint32 p05 = BE_read_uint32(wtafile); // 0x10
     uint32 p06 = BE_read_uint32(wtafile); // 0x14
     uint32 p07 = BE_read_uint32(wtafile); // 0x18
     uint32 p08 = BE_read_uint32(wtafile); // 0x1C
     uint32 p09 = BE_read_uint32(wtafile); // 0x20
     uint32 p10 = BE_read_uint32(wtafile); // 0x24
     uint32 p11 = BE_read_uint32(wtafile); // 0x28
     uint32 p12 = BE_read_uint32(wtafile); // 0x2C
     uint32 p13 = BE_read_uint32(wtafile); // 0x30
     //p12 = 0x00000000;
     //p13 = 0x00000A00;
 
     // save XPR2 texture information
     BE_write_uint32(ofile, p01); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p02); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p03); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p04); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p05); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p06); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p07); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p08); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p09); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p10); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p11); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p12); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p13); if(ofile.fail()) return error("Write failure.");
 
     // save padding
     boost::shared_array<char> padding(new char[0x7AC]);
     ZeroMemory(padding.get(), 0x7AC);
     BE_write_array(ofile, padding.get(), 0x7AC);
     if(ofile.fail()) return error("Write failure.");
 
     // move to texture data
     uint32 position = list1[i];
     wtpfile.seekg(list1[i]);
     if(wtpfile.fail()) return error("Seek failure.");
 
     // read texture data
     uint32 size = list2[i];
     boost::shared_array<char> data(new char[size]);
     wtpfile.read(data.get(), size);
     if(wtpfile.fail()) return error("Read failure.");
 
     // save texture data
     BE_write_array(ofile, data.get(), size);
     if(ofile.fail()) return error("Write failure.");

     // close output file
     ofile.close();

     // unbundle textures
     string modpath = GetModulePathname();
     XB360Unbundle(modpath.c_str(), ss.str().c_str(), savepath.str().c_str());
    }

 // delete WTA file
 // wtafile.close();
 // DeleteFileA(filename.c_str());

 // delete WTB file
 // wtpfile.close();
 // DeleteFileA(WTP_filename.c_str());

 return true;
}

bool processWTB(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open WTB file.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return message("Empty WTB file; skipping file.");

 // read magic number
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure."); 
 if(magic != 0x00425457) return message("Not a WTB file; skipping file.");

 // read unknown
 uint32 unk01 = BE_read_uint32(ifile); // 0x01
 if(ifile.fail()) return error("Read failure.");

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(!n_files) return true; // do nothing

 // read offset to texture offsets
 uint32 offset1 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to texture sizes
 uint32 offset2 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to flags
 uint32 offset3 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to texture name hashes (identifiers)
 uint32 offset4 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to XPR2 texture information
 uint32 offset5 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read texture offsets
 ifile.seekg(offset1);
 if(ifile.fail()) return error("Seek failure.");
 vector<uint32> list1(n_files);
 for(uint32 i = 0; i < n_files; i++) {
     list1[i] = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
    }

 // read texture sizes
 ifile.seekg(offset2);
 if(ifile.fail()) return error("Seek failure.");
 vector<uint32> list2(n_files);
 for(uint32 i = 0; i < n_files; i++) {
     list2[i] = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
    }

 // read XPR2 texture information
 ifile.seekg(offset5);
 if(ifile.fail()) return error("Seek failure.");
 uint32 infosize = n_files*0x34;
 boost::shared_array<char> infodata(new char[infosize]);
 if(!BE_read_array(ifile, infodata.get(), infosize)) return error("Read failure.");

 // create save path
 stringstream savepath;
 savepath << GetPathnameFromFilename(filename);
 savepath << GetShortFilenameWithoutExtension(filename);
 savepath << "\\";
 CreateDirectoryA(savepath.str().c_str(), NULL);

 // for each texture file
 for(uint32 i = 0; i < n_files; i++)
    {
     // create XPR2 file
     stringstream ss;
     ss << savepath.str() << setfill('0') << setw(4) << i;
     ss << ".xpr";
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create XPR file.");

     // save XPR2
     BE_write_uint32(ofile, 0x58505232);
     if(ofile.fail()) return error("Write failure.");
     
     // save XPR2 data offset
     BE_write_uint32(ofile, 0x800);
     if(ofile.fail()) return error("Write failure.");
 
     // save XPR2 total texture size
     if(offset5 == 0x00) BE_write_uint32(ofile, list2[i] - 0x34);
     else BE_write_uint32(ofile, list2[i]);
     if(ofile.fail()) return error("Write failure.");
     
     // save XPR2 number of textures
     BE_write_uint32(ofile, 0x01);
     if(ofile.fail()) return error("Write failure.");
 
     // save D2XT
     BE_write_uint32(ofile, 0x54583244);
     if(ofile.fail()) return error("Write failure.");
 
     // save offset from 0xC to texture information
     BE_write_uint32(ofile, 0x20);
     if(ofile.fail()) return error("Write failure.");
 
     // save size of texture info
     BE_write_uint32(ofile, 0x34);
     if(ofile.fail()) return error("Write failure.");
 
     // save offset from 0xC to filename information
     BE_write_uint32(ofile, 0x18);
     if(ofile.fail()) return error("Write failure.");
 
     // save 0x00000000
     BE_write_uint32(ofile, 0x00);
     if(ofile.fail()) return error("Write failure.");
 
     // save filename
     stringstream fname;
     fname << setfill('0') << setw(4) << i;
     char temp[8];
     temp[0] = fname.str()[0];
     temp[1] = fname.str()[1];
     temp[2] = fname.str()[2];
     temp[3] = fname.str()[3];
     temp[4] = 0;
     temp[5] = 0;
     temp[6] = 0;
     temp[7] = 0;
     BE_write_array(ofile, &temp[0], 8);
     if(ofile.fail()) return error("Write failure.");
 
     // seek XPR2 texture information
     if(offset5 == 0x00) ifile.seekg(list1[i]);
     else ifile.seekg((offset5 + (i * 0x34)));
     if(ifile.fail()) return error("Seek failure.");

     // read XPR2 texture information
     uint32 p01 = BE_read_uint32(ifile); // 0x00
     uint32 p02 = BE_read_uint32(ifile); // 0x04
     uint32 p03 = BE_read_uint32(ifile); // 0x08
     uint32 p04 = BE_read_uint32(ifile); // 0x0C
     uint32 p05 = BE_read_uint32(ifile); // 0x10
     uint32 p06 = BE_read_uint32(ifile); // 0x14
     uint32 p07 = BE_read_uint32(ifile); // 0x18
     uint32 p08 = BE_read_uint32(ifile); // 0x1C
     uint32 p09 = BE_read_uint32(ifile); // 0x20
     uint32 p10 = BE_read_uint32(ifile); // 0x24
     uint32 p11 = BE_read_uint32(ifile); // 0x28
     uint32 p12 = BE_read_uint32(ifile); // 0x2C
     uint32 p13 = BE_read_uint32(ifile); // 0x30
     //p12 = 0x00000000;
     //p13 = 0x00000A00;

     // save XPR2 texture information
     BE_write_uint32(ofile, p01); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p02); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p03); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p04); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p05); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p06); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p07); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p08); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p09); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p10); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p11); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p12); if(ofile.fail()) return error("Write failure.");
     BE_write_uint32(ofile, p13); if(ofile.fail()) return error("Write failure.");
 
     // save padding
     boost::shared_array<char> padding(new char[0x7AC]);
     ZeroMemory(padding.get(), 0x7AC);
     BE_write_array(ofile, padding.get(), 0x7AC);
     if(ofile.fail()) return error("Write failure.");

     // move to texture data
     uint32 position = list1[i];
     if(offset5 == 0x00) position += 0x34;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read texture data
     uint32 size = list2[i];
     if(offset5 == 0x00) size -= 0x34;
     boost::shared_array<char> data(new char[size]);
     ifile.read(data.get(), size);
     if(ifile.fail()) return error("Read failure.");

     // save texture data
     BE_write_array(ofile, data.get(), size);
     if(ofile.fail()) return error("Write failure.");

     // close output file
     ofile.close();

     // unbundle textures
     string modpath = GetModulePathname();
     XB360Unbundle(modpath.c_str(), ss.str().c_str(), savepath.str().c_str());
    }

 // delete WTB file
 // ifile.close();
 // DeleteFileA(filename.c_str());

 return true;
}

bool processWTP(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open WTP file.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return message("Empty WTP file; skipping file.");

 // create output directory
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // while we have data to read
 uint32 image_curr = 0;
 uint32 bytes_read = 0;
 while(bytes_read < filesize)
      {
       // read magic number
       uint32 magic = BE_read_uint32(ifile);
       if(ifile.fail()) return error("Read failure."); 
       if(magic != 0x02010100) return message("Not a WTP file; skipping file.");

       // read header
       uint32 h01 = BE_read_uint32(ifile); // 0x04: data size + 0x10
       uint32 h02 = BE_read_uint32(ifile); // 0x08: 0x01
       uint32 h03 = BE_read_uint32(ifile); // 0x0C: 0x00
       uint32 h04 = BE_read_uint32(ifile); // 0x10: offset to data, should be 0x80
       uint32 h05 = BE_read_uint32(ifile); // 0x14: data size
       uint08 h06 = BE_read_uint08(ifile); // 0x18: data type
       uint08 h07 = BE_read_uint08(ifile); // 0x19: mipmaps
       uint08 h08 = BE_read_uint08(ifile); // 0x1A: 0x02
       uint08 h09 = BE_read_uint08(ifile); // 0x1B: 0x00
       uint32 h10 = BE_read_uint32(ifile); // 0x1C: 0xAAE4
       uint16 h11 = BE_read_uint16(ifile); // 0x20: dx
       uint16 h12 = BE_read_uint16(ifile); // 0x22: dy
       uint32 h13 = BE_read_uint32(ifile); // ???
       uint32 h14 = BE_read_uint32(ifile); // ???
       uint32 h15 = BE_read_uint32(ifile); // ???

       // skip to data
       ifile.seekg((h04 - 0x30), ios::cur);
       if(ifile.fail()) return error("Seek failure.");
       bytes_read += h04;

       // read data
       boost::shared_array<char> data(new char[h05]);
       ifile.read(data.get(), h05);
       if(ifile.fail()) return error("Read failure.");
       bytes_read += h05;

       // deswizzle
       if(h06 == 0x85)
         {
          uint32 dx = h11;
          uint32 dy = h12;
          boost::shared_array<char> copy(new char[h05]);
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

       // create output filename
       stringstream ofname;
       ofname << savepath << setfill('0') << setw(3) << image_curr << ".dds";
       image_curr++;

       // create output file
       ofstream ofile(ofname.str().c_str(), ios::binary);
       if(!ofile) return error("Failed to create output file.");

       // create and save DDS header
       DDS_HEADER ddsh;
       if(h06 == 0x83) CreateL16DDSHeader(h11, h12, (h07 ? (h07 - 1): 0), FALSE, &ddsh);
       else if(h06 == 0x85) CreateUncompressedDDSHeader(h11, h12, (h07 ? (h07 - 1): 0), 32, 0xFF0000, 0xFF00, 0xFF, 0xFF000000, FALSE, &ddsh);
       else if(h06 == 0x86) CreateDXT1Header(h11, h12, (h07 ? (h07 - 1): 0), FALSE, &ddsh);
       else if(h06 == 0x87) CreateDXT3Header(h11, h12, (h07 ? (h07 - 1): 0), FALSE, &ddsh);
       else if(h06 == 0x88) CreateDXT5Header(h11, h12, (h07 ? (h07 - 1): 0), FALSE, &ddsh);
       else return error("Unknown texture format.");
       ofile.write("DDS ", 4);
       ofile.write((char*)&ddsh, sizeof(ddsh));
       if(ofile.fail()) return error("Write failure.");

       // save data
       ofile.write(data.get(), h05);
       if(ofile.fail()) return error("Write failure.");

       // skip to next 0x1000-byte boundary
       bytes_read = ((bytes_read + 0xFFF) & (~0xFFF));
       ifile.seekg(bytes_read);
       if(ifile.fail()) return error("Seek failure.");
      }

 return true;
}

bool processCTX(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open CTX file.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return message("Empty CTX file; skipping file.");

 return true;
}

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

bool processWMB(const std::string& filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // read magic number
 uint32 magic = BE_read_uint32(ifile);
 if(magic == 0x574D4232) return message("This is a WMB2 file.");
 if(magic != 0x574D4234) return error("Not a WMB4 file.");

 // read unknowns
 uint32 unk01 = BE_read_uint32(ifile); // 0xFFFFFFFF
 uint32 unk02 = BE_read_uint32(ifile); // flags
 uint16 unk03 = BE_read_uint16(ifile); // flags
 uint16 unk04 = BE_read_uint16(ifile); // 0xFFFF

 // vertex format
 // determined by unk02?
 uint32 vertex_format = (unk02 & 0xFF);

 // move to important header information
 ifile.seekg(0x28);
 if(ifile.fail()) return error("Seek failure.");

 uint32 offset    = BE_read_uint32(ifile); // 0x28
 uint32 blocks    = BE_read_uint32(ifile); // 0x2C
 uint32 si_offset = BE_read_uint32(ifile); // 0x30
 uint32 si_size   = BE_read_uint32(ifile); // 0x34
 uint32 sj_offset = BE_read_uint32(ifile); // 0x38 surface to joint map associations
 uint32 jl_offset = BE_read_uint32(ifile); // 0x3C offset to joint data
 uint32 jl_joints = BE_read_uint32(ifile); // 0x40 number of joints
 uint32 u2_offset = BE_read_uint32(ifile); // 0x44 unknown offset
 uint32 u2_size   = BE_read_uint32(ifile); // 0x48 size of data at offset
 uint32 jm_offset = BE_read_uint32(ifile); // 0x4C joint map offset
 uint32 jm_size   = BE_read_uint32(ifile); // 0x50 number of joint maps

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // read block information
 struct MGRRBLOCKINFO {
  uint32 offset1; // offset to vertex buffer
  uint32 offset2; // offset to UVs?
  uint32 zero1;
  uint32 zero2;
  uint32 vertices; // number of vertices
  uint32 offset3;  // index buffer
  uint32 indices;  // number of indices
 };
 deque<MGRRBLOCKINFO> blockinfo;
 for(uint32 i = 0; i < blocks; i++) {
     MGRRBLOCKINFO item;
     item.offset1  = BE_read_uint32(ifile);
     item.offset2  = BE_read_uint32(ifile);
     item.zero1    = BE_read_uint32(ifile);
     item.zero2    = BE_read_uint32(ifile);
     item.vertices = BE_read_uint32(ifile);
     item.offset3  = BE_read_uint32(ifile);
     item.indices  = BE_read_uint32(ifile);
     blockinfo.push_back(item);
    }

 //
 // SURFACE INFORMATION
 //

 // move to offset
 ifile.seekg(si_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read surface information
 struct MGRRSURFACEINFO {
  uint32 vb_index;
  uint32 vb_start;
  uint32 ib_start;
  uint32 vb_elem;
  uint32 ib_elem;
 };
 deque<MGRRSURFACEINFO> surfinfo;
 for(uint32 i = 0; i < si_size; i++) {
     MGRRSURFACEINFO item;
     item.vb_index = BE_read_uint32(ifile);
     item.vb_start = BE_read_uint32(ifile);
     item.ib_start = BE_read_uint32(ifile);
     item.vb_elem  = BE_read_uint32(ifile);
     item.ib_elem  = BE_read_uint32(ifile);
     if(!item.vb_elem) return error("Invalid vertex buffer elements.");
     surfinfo.push_back(item);
    }

 //
 // JOINT MAP INFORMATION
 //

 // joint map data
 deque<pair<uint32, uint32>> jmapinfo;
 deque<deque<uint16>> jmapdata;

 // if joint map exists
 if(jm_offset && jm_size)
   {
    // move to offset
    ifile.seekg(jm_offset);
    if(ifile.fail()) return error("Seek failure.");

    // for each joint map
    for(uint32 i = 0; i < jm_size; i++) {
        uint32 p01 = BE_read_uint32(ifile);
        uint32 p02 = BE_read_uint32(ifile);
        p02 >>= 24;
        jmapinfo.push_back(pair<uint32, uint32>(p01, p02));
       }

    // for each joint map
    for(uint32 i = 0; i < jm_size; i++)
       {
        // move to offset
        ifile.seekg(jmapinfo[i].first);
        if(ifile.fail()) return error("Seek failure.");

        // read joint map
        deque<uint16> mapdata(jmapinfo[i].second);
        for(uint32 j = 0; j < jmapinfo[i].second; j++) {
            uint16 item = LE_read_uint08(ifile);
            if(ifile.fail()) return error("Read failure.");
             mapdata[j] = item;
           }

        // set joint map
        jmapdata.push_back(mapdata);
       }
   }

 //
 // SURFACE TO JOINT MAP ASSOCIATIONS
 //

 struct MGRRSURFACEINFO2 {
  uint32 surf_index; // 0, 1, 2, ..., n
  uint32 unk01;      // index to an item whose data offset is specified at 0x64
  uint16 unk02;
  uint16 jmap_index; // joint map index
  uint16 unk03;      // appear to be 1 for surface, 0 for bloddy gibbs
  uint16 unk04;
 };
 deque<pair<uint32, uint32>> groups;
 map<uint32, MGRRSURFACEINFO2> surfinfo2;

 if(sj_offset)
   {
    // move to offset
    ifile.seekg(sj_offset);
    if(ifile.fail()) return error("Seek failure.");
    
    // read groups
    for(;;) {
        uint32 g1 = BE_read_uint32(ifile); // offset to group
        uint32 g2 = BE_read_uint32(ifile); // number of items in group
        if(g1 == 0x00) break;
        groups.push_back(pair<uint32, uint32>(g1, g2));
       } 
    
    // read secondary surface information
    for(uint32 i = 0; i < groups.size(); i++)
       {
        // move to offset
        ifile.seekg(groups[i].first);
        if(ifile.fail()) return error("Seek failure.");

        // for each group
        for(uint32 j = 0; j < groups[i].second; j++)
           {
            // read item
            MGRRSURFACEINFO2 item;
            item.surf_index = BE_read_uint32(ifile);
            item.unk01 = BE_read_uint32(ifile);
            item.unk02 = BE_read_uint16(ifile);
            item.jmap_index = BE_read_uint16(ifile);
            item.unk03 = BE_read_uint16(ifile);
            item.unk04 = BE_read_uint16(ifile);

            // validate item
            // if we are using joint maps, the joint map index must be valid!
            if(jm_size && !(item.jmap_index < jm_size)) {
               cout << item.surf_index << endl;
               cout << item.unk01 << endl;
               cout << item.unk02 << endl;
               cout << item.jmap_index << endl;
               cout << item.unk03 << endl;
               cout << item.unk04 << endl;
               stringstream ss;
               ss << "Joint map index out of bounds. ";
               ss << "There are " << jm_size << " joint maps. ";
               ss << "Encountered an index with value " << item.jmap_index << " at offset 0x" << hex << groups[i].first << dec << ".";
               return error(ss);
              }

            // insert item
            typedef map<uint32, MGRRSURFACEINFO2>::value_type value_type;
            surfinfo2.insert(value_type(item.surf_index, item));
           }
       }
   }

 // model container
 ADVANCEDMODELCONTAINER amc;

 // process joint maps
 for(uint32 i = 0; i < jm_size; i++) {
     AMC_JOINTMAP jmap;
     jmap.jntmap.resize(jmapdata[i].size());
     for(uint32 j = 0; j < jmapdata[i].size(); j++) jmap.jntmap[j] = jmapdata[i][j];
     amc.jmaplist.push_back(jmap);
    }

 // for each block
 uint32 index_offset = 0;
 for(uint32 i = 0; i < blocks; i++)
    {
     // retrieve block information
     MGRRBLOCKINFO item;
     item = blockinfo[i];

     // move to vertices
     ifile.seekg(item.offset1);
     if(ifile.fail()) return error("Seek failure.");

     // 0x1C bytes per vertex
     if(unk02 == 0x00010107)
       {
        // allocate vertex buffer
        AMC_VTXBUFFER vb;
        vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV;
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
        vb.elem = item.vertices;
        vb.data.reset(new AMC_VERTEX[vb.elem]);

        // process vertices
        for(uint32 j = 0; j < item.vertices; j++)
           {
            vb.data[j].vx = BE_read_real32(ifile);       // 0x04
            vb.data[j].vy = BE_read_real32(ifile);       // 0x08
            vb.data[j].vz = BE_read_real32(ifile);       // 0x0C
            vb.data[j].uv[0][0] = BE_read_real16(ifile); // 0x0E
            vb.data[j].uv[0][1] = BE_read_real16(ifile); // 0x10
            real32 u01 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x11
            real32 u02 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x12
            real32 u03 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x13
            real32 u04 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x14
            real32 u05 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x15
            real32 u06 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x16
            real32 u07 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x17
            real32 u08 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x18
            real32 u09 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x19
            real32 u10 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x1A
            real32 u11 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x1B
            real32 u12 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x1C
           }

        // insert vertex buffer
        amc.vblist.push_back(vb);
       }
     else if(unk02 == 0x00010307)
       {
        // allocate vertex buffer
        AMC_VTXBUFFER vb;
        vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV;
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
        vb.elem = item.vertices;
        vb.data.reset(new AMC_VERTEX[vb.elem]);

        // process vertices
        for(uint32 j = 0; j < item.vertices; j++)
           {
            vb.data[j].vx = BE_read_real32(ifile);       // 4
            vb.data[j].vy = BE_read_real32(ifile);       // 8
            vb.data[j].vz = BE_read_real32(ifile);       // 12
            vb.data[j].uv[0][0] = BE_read_real16(ifile); // 14
            vb.data[j].uv[0][1] = BE_read_real16(ifile); // 16
            real32 u01 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 20
            real32 u02 = (BE_read_uint08(ifile) - 127.0f)/128.0f; //
            real32 u03 = (BE_read_uint08(ifile) - 127.0f)/128.0f; //
            real32 u04 = (BE_read_uint08(ifile) - 127.0f)/128.0f; //
            real32 u05 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 24
            real32 u06 = (BE_read_uint08(ifile) - 127.0f)/128.0f; //
            real32 u07 = (BE_read_uint08(ifile) - 127.0f)/128.0f; //
            real32 u08 = (BE_read_uint08(ifile) - 127.0f)/128.0f; //
            real32 u09 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 28
            real32 u10 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 
            real32 u11 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 
            real32 u12 = (BE_read_uint08(ifile) - 127.0f)/128.0f; // 
            real32 u13 = BE_read_real16(ifile); // 30
            real32 u14 = BE_read_real16(ifile); // 32
           }
   
        // insert vertex buffer
        amc.vblist.push_back(vb);
       }
     // unk02 == 0x00000107
     else if(unk02 == 0x00000107)
       {
        // allocate vertex buffer
        AMC_VTXBUFFER vb;
        vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV;
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
        vb.elem = item.vertices;
        vb.data.reset(new AMC_VERTEX[vb.elem]);

        // process vertices
        for(uint32 j = 0; j < item.vertices; j++)
           {
            vb.data[j].vx = BE_read_real32(ifile); // 0x04
            vb.data[j].vy = BE_read_real32(ifile); // 0x08
            vb.data[j].vz = BE_read_real32(ifile); // 0x0C
            vb.data[j].uv[0][0] = BE_read_real16(ifile); // 0x0E // U
            vb.data[j].uv[0][1] = BE_read_real16(ifile); // 0x10 // V
            real32 p06 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x11
            real32 p07 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x12
            real32 p08 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x13
            real32 p09 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x14
            real32 p10 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x15
            real32 p11 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x16
            real32 p12 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x17
            real32 p13 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x18
           }
   
        // insert vertex buffer
        amc.vblist.push_back(vb);
       }
     // works great for 0x00000137 (flag1)
     else if(unk02 == 0x00000137 || unk02 == 0x00000337 || unk02 == 0x00010337 || unk02 == 0x00010137)
       {
        // allocate vertex buffer
        AMC_VTXBUFFER vb;
        vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV | AMC_VERTEX_WEIGHTS;
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
        vb.elem = item.vertices;
        vb.data.reset(new AMC_VERTEX[vb.elem]);

        // process vertices
        for(uint32 j = 0; j < item.vertices; j++)
           {
            vb.data[j].vx = BE_read_real32(ifile); // 0x04
            vb.data[j].vy = BE_read_real32(ifile); // 0x08
            vb.data[j].vz = BE_read_real32(ifile); // 0x0C
            vb.data[j].uv[0][0] = BE_read_real16(ifile); // 0x0E // U
            vb.data[j].uv[0][1] = BE_read_real16(ifile); // 0x10 // V
            real32 p06 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x11
            real32 p07 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x12
            real32 p08 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x13
            real32 p09 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x14
            real32 p10 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 0x18
            real32 p11 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 
            real32 p12 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 
            real32 p13 = ((real32)BE_read_uint08(ifile) - 127.0f)/128.0f; // 
            uint16 p14 = BE_read_uint08(ifile); // 0x1C (blend indices)
            uint16 p15 = BE_read_uint08(ifile); //
            uint16 p16 = BE_read_uint08(ifile); //
            uint16 p17 = BE_read_uint08(ifile); //
            real32 p18 = ((real32)BE_read_uint08(ifile))/255.0f; // 0x20 (blend weights)
            real32 p19 = ((real32)BE_read_uint08(ifile))/255.0f; //
            real32 p20 = ((real32)BE_read_uint08(ifile))/255.0f; //
            real32 p21 = ((real32)BE_read_uint08(ifile))/255.0f; //
   
            // initialize weights
            vb.data[j].wi[0] = p14;
            vb.data[j].wi[1] = p15;
            vb.data[j].wi[2] = p16;
            vb.data[j].wi[3] = p17;
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            
            // set weights
            vb.data[j].wv[0] = p18;
            vb.data[j].wv[1] = p19;
            vb.data[j].wv[2] = p20;
            vb.data[j].wv[3] = p21;
           }

        // insert vertex buffer
        amc.vblist.push_back(vb);
       }
     else {
        stringstream ss;
        ss << "Unknown vertex format 0x" << hex << vertex_format << dec << "." << endl;
        return error(ss);
       }       

     // move to UV map
     // if(item.offset2)
     //   {
     //    ifile.seekg(item.offset2);
     //    if(ifile.fail()) return error("Seek failure.");
     //    
     //    // process UV map
     //    for(uint32 j = 0; j < item.vertices; j++) {
     //        uint32 p01 = BE_read_uint32(ifile);
     //        amc.vblist[i].data[j].uv[0][0] = BE_read_real16(ifile);
     //        amc.vblist[i].data[j].uv[0][1] = BE_read_real16(ifile);
     //       }
     //   }

     // move to indices
     ifile.seekg(item.offset3);
     if(ifile.fail()) {
        stringstream ss;
        ss << "Seek failure. Offset 0x" << hex << item.offset3 << dec << ".";
        return error(ss);
       }

     // read index buffer
     boost::shared_array<char> ibuffer(new char[item.indices * sizeof(uint16)]);
     BE_read_array(ifile, reinterpret_cast<uint16*>(ibuffer.get()), item.indices);

     // primitive type
     uint08 primitive = AMC_TRIANGLES;
     if(unk03 == 0x00) primitive = AMC_TRIANGLES;
     else if(unk03 == 0x01) primitive = AMC_TRISTRIPCUT;
     else return error("Unknown primitive type.");

     // allocate index buffer
     AMC_IDXBUFFER ib;
     ib.format = AMC_UINT16;
     ib.type = primitive;
     ib.wind = AMC_CCW;
     ib.name = "default";
     ib.elem = item.indices;
     ib.data = ibuffer;

     // insert index buffer
     amc.iblist.push_back(ib);
    }

 // process surfaces
 for(uint32 i = 0; i < si_size; i++)
    {
     // generate surface name
     stringstream ss;
     ss << "surface" << setfill('0') << setw(3) << i;

     // lookup joint map information
     uint32 jm_index = AMC_INVALID_JMAP_INDEX;
     if(jm_size && surfinfo2.size()) {
        auto iter = surfinfo2.find(i);
        if(iter != surfinfo2.end()) jm_index = iter->second.jmap_index;
       }

     // create surface
     AMC_SURFACE surf;
     surf.name = ss.str();
     AMC_REFERENCE ref;
     ref.vb_index = surfinfo[i].vb_index;
     ref.vb_start = surfinfo[i].vb_start;
     ref.vb_width = surfinfo[i].vb_elem;
     ref.ib_index = surfinfo[i].vb_index; // same as vb index
     ref.ib_start = surfinfo[i].ib_start;
     ref.ib_width = surfinfo[i].ib_elem;
     ref.jm_index = jm_index; // not associated with a joint map
     surf.refs.push_back(ref);
     surf.surfmat = AMC_INVALID_SURFMAT;

     // insert surface
     amc.surfaces.push_back(surf);
    }

 // if skeleton is present
 if(jl_offset != 0x0000)
   {
    // move to offset
    ifile.seekg(jl_offset);
    if(ifile.fail()) return error("Seek failure (joint data offset).");

    // initialize skeleton
    AMC_SKELETON2 skel;
    skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
    skel.tiplen = 0.25f;
    skel.name = "skeleton";

    for(uint32 i = 0; i < jl_joints; i++)
       {
        // read data
        uint16 p01 = BE_read_uint16(ifile); // unknown
        uint16 p02 = BE_read_uint16(ifile); // unknown
        uint16 p03 = BE_read_uint16(ifile); // parent index
        uint16 p04 = BE_read_uint16(ifile); // unknown
        real32 p05 = BE_read_real32(ifile); // 
        real32 p06 = BE_read_real32(ifile); // 
        real32 p07 = BE_read_real32(ifile); // 
        real32 p08 = BE_read_real32(ifile); // x
        real32 p09 = BE_read_real32(ifile); // y
        real32 p10 = BE_read_real32(ifile); // z

        // create joint name
        stringstream ss;
        ss << "jnt_" << setfill('0') << setw(3) << i;
   
        // create and insert joint
        AMC_JOINT joint;
        joint.name = ss.str();
        joint.id = i;
        joint.parent = (p03 == 0xFFFF ? AMC_INVALID_JOINT : p03);
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
        joint.p_rel[0] = p08; // p05;
        joint.p_rel[1] = p09; // p06;
        joint.p_rel[2] = p10; // p07;
        joint.p_rel[3] = 1.0f;
        skel.joints.push_back(joint);
       }

    // insert skeleton
    amc.skllist2.push_back(skel);
   }

 // save parameters
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // save transformed AMC file
 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);

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

 // CTX = textures
 // DAT = archives
 // DTT = textures
 // EFF = DAT file
 // EFT = DTT file
 // EVN = DAT file
 // MOT = animation
 // SCR = models
 // WMB = model
 // WTA = textures
 // WTB = textures
 // WTP = textures

 // BNK = useless
 // BRD = useless
 // BXM = useless
 // EST = useless
 // EXP = useless
 // GAD = useless
 // HKX = useless
 // LY2 = useless
 // MCD = useless
 // MKD = useless
 // OPD = useless
 // POS = useless
 // PSO = useless
 // RAD = useless
 // RBD = useless
 // SAB = useless
 // SAE = useless
 // SAS = useless
 // SOP = useless
 // SST = useless
 // SYN = useless
 // TGS = useless
 // TRG = useless
 // UID = useless
 // UVD = useless
 // VCA = useless
 // VCD = useless
 // VSO = useless

 // // variables
 bool doDAT = false; // done
 bool doDTT = false; // done
 bool doEFF = false; // done
 bool doEFT = false; // done
 bool doEVN = false; // done
 bool doSCR = false; // contains models but not a DAT file
 bool doWTA = false; // textures (XBOX360)
 bool doWTB = false; // textures (XBOX360)
 bool doCTX = false; // textures (XBOX360)
 bool doWMB = true;

 // loop forever
 for(;;)
    {
     uint32 DAT_proc = 0;
     uint32 DTT_proc = 0;
     uint32 EFF_proc = 0;
     uint32 EFT_proc = 0;
     uint32 EVN_proc = 0;

     // process DAT
     if(doDAT) {
        cout << "STAGE 1" << endl;
        cout << "Processing .DAT files..." << endl;
        deque<string> filelist;
        BuildFilenameList(filelist, ".DAT", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processDAT(filelist[i])) return false;
           }
        cout << endl;
        DAT_proc = filelist.size();
       }

     // process DTT
     if(doDTT) {
        cout << "STAGE 2" << endl;
        cout << "Processing .DTT files..." << endl;
        deque<string> filelist;
        BuildFilenameList(filelist, ".DTT", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processDTT(filelist[i])) return false;
           }
        cout << endl;
        DTT_proc = filelist.size();
       }

     // process EFF
     if(doEFF) {
        cout << "STAGE 3" << endl;
        cout << "Processing .EFF files..." << endl;
        deque<string> filelist;
        BuildFilenameList(filelist, ".EFF", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processDAT(filelist[i])) return false;
           }
        cout << endl;
        cout << endl;
        EFF_proc = filelist.size();
       }
     
     // process EFT
     if(doEFT) {
        cout << "STAGE 4" << endl;
        cout << "Processing .EFT files..." << endl;
        deque<string> filelist;
        BuildFilenameList(filelist, ".EFT", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processDTT(filelist[i])) return false;
           }
        cout << endl;
        cout << endl;
        EFT_proc = filelist.size();
       }
     
     // process EVN
     if(doEVN) {
        cout << "STAGE 5" << endl;
        cout << "Processing .EVN files..." << endl;
        deque<string> filelist;
        BuildFilenameList(filelist, ".EVN", pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processDAT(filelist[i])) return false;
           }
        cout << endl;
        cout << endl;
        EVN_proc = filelist.size();
       }

     // terminate when no more files to process
     uint32 ALL_proc = DAT_proc + DTT_proc + EFF_proc + EFT_proc + EVN_proc;
     if(!ALL_proc) break;
    }

 // process SCR
 if(doSCR) {
    cout << "STAGE 6" << endl;
    cout << "Processing .SCR files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".SCR", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processSCR(filelist[i])) return false;
       }
    cout << endl;
   }

 // process WTA
 if(doWTA) {
    cout << "STAGE 7" << endl;
    cout << "Processing .WTA files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".WTA", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processWTA(filelist[i])) return false;
       }
    cout << endl;
   }
 
 // process CTX
 if(doCTX) {
    cout << "STAGE 8" << endl;
    cout << "Processing .CTX files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".CTX", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCTX(filelist[i])) return false;
       }
    cout << endl;
   }

 // process WTB
 if(doWTB) {
    cout << "STAGE 9" << endl;
    cout << "Processing .WTB files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".WTB", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processWTB(filelist[i])) return false;
       }
    cout << endl;
   }

 // process WMB
 // XBOX360 ONLY
 if(doWMB) {
    cout << "STAGE 10" << endl;
    cout << "Processing .WMB files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".WMB", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processWMB(filelist[i])) return false;
       }
    cout << endl;
   }


 return true;
}

}};

