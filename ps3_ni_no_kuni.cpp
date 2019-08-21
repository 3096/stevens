#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_zlib.h"
#include "ps3_ni_no_kuni.h"

using namespace std;

namespace PS3 { namespace NI_NO_KUNI {

class extractor {
 private :
  std::string pathname;
  std::deque<std::string> arclist;
  std::deque<std::string> datlist;
  std::deque<std::string> hpklist;
  std::deque<std::string> pkclist;
  std::deque<std::string> ipklist;
  std::deque<std::string> imglist;
 private :
  bool process001(void);
  bool process002(void);
  bool process003(void);
  bool process004(void);
  bool process005(void);
  bool process006(void);
  bool processGVEWJPEG(ifstream& ifile, const char* path, const char* name);
  bool processGVEWGVMP(ifstream& ifile, const char* path, const char* name);
 public :
  bool extract(void);
  void clear(void);
 public :
  extractor(const char* path);
 ~extractor();
};

extractor::extractor(const char* path) : pathname(path)
{
}

extractor::~extractor()
{
}

bool extractor::extract(void)
{
 // clear previous
 clear();

 // find all dat files
 cout << "----------" << endl;
 cout << " PHASE #1 " << endl;
 cout << "----------" << endl;
 cout << "Searching for dat files." << endl;
 BuildFilenameList(datlist, "dat", pathname.c_str());
 cout << "Found " << datlist.size() << " dat files." << endl;
 if(datlist.size()) { if(!process001()) return false; }
 cout << endl;

 // find all arc files
 cout << "----------" << endl;
 cout << " PHASE #2 " << endl;
 cout << "----------" << endl;
 cout << "Searching for arc files." << endl;
 BuildFilenameList(arclist, "zarc", pathname.c_str());
 cout << "Found " << arclist.size() << " arc files." << endl;
 if(arclist.size()) { if(!process002()) return false; }
 cout << endl;

 // find all hpk files
 cout << "----------" << endl;
 cout << " PHASE #3 " << endl;
 cout << "----------" << endl;
 cout << "Searching for hpk files." << endl;
 BuildFilenameList(hpklist, "hpk", pathname.c_str());
 cout << "Found " << hpklist.size() << " hpk files." << endl;
 if(hpklist.size()) { if(!process003()) return false; }
 cout << endl;

 // find all pkchr files
 cout << "----------" << endl;
 cout << " PHASE #4 " << endl;
 cout << "----------" << endl;
 cout << "Searching for pkchr files." << endl;
 BuildFilenameList(pkclist, "pkchr", pathname.c_str());
 cout << "Found " << pkclist.size() << " pkchr files." << endl;
 if(pkclist.size()) { if(!process004()) return false; }
 cout << endl;

 // find all imgpak files
 cout << "----------" << endl;
 cout << " PHASE #5 " << endl;
 cout << "----------" << endl;
 cout << "Searching for imgpak files." << endl;
 BuildFilenameList(ipklist, "imgpak", pathname.c_str());
 cout << "Found " << ipklist.size() << " imgpak files." << endl;
 if(ipklist.size()) { if(!process005()) return false; }
 cout << endl;

 // find all img files
 cout << "----------" << endl;
 cout << " PHASE #6 " << endl;
 cout << "----------" << endl;
 cout << "Searching for img files." << endl;
 BuildFilenameList(imglist, "img", pathname.c_str());
 cout << "Found " << imglist.size() << " img files." << endl;
 if(imglist.size()) { if(!process006()) return false; }
 cout << endl;

 return true;
}

bool extractor::process001(void)
{
 struct entry {
  uint32 p1;
  uint32 p2;
  uint32 p3;
  uint32 p4;
  std::string name;
 };

 for(size_t i = 0; i < datlist.size(); i++)
    {
     // open file
     ifstream ifile(datlist[i].c_str(), ios::binary);
     if(!ifile) return error("Could not open DAT file.");

     uint32 tgdt = BE_read_uint32(ifile);
     if(tgdt != 0x54474454) continue;

     uint32 vers = BE_read_uint32(ifile);
     if(vers != 0x30313030) continue;

     uint32 entries = BE_read_uint32(ifile);
     uint32 start_offset = BE_read_uint32(ifile);

     std::deque<entry> entrylist;
     for(size_t j = 0; j < entries; j++)
        {
         entry e;
         e.p1 = BE_read_uint32(ifile);
         e.p2 = BE_read_uint32(ifile);
         e.p3 = BE_read_uint32(ifile); // offset to data section, from start offset
         e.p4 = BE_read_uint32(ifile); // size of actual data section, padding may follow
         entrylist.push_back(e);
        }
     cout << "Found " << entrylist.size() << " entries." << endl;

     // process entries names
     for(size_t j = 0; j < entries; j++)
        {
         // move to name
         ifile.seekg(start_offset + entrylist[j].p1);
         if(ifile.fail()) return error("Failed to seek offset.");

         // extract name
         char name[1024];
         LE_read_array(ifile, &name[0], 1024);
         entrylist[j].name = name;
        }

     // process entries
     for(size_t j = 0; j < entries; j++)
        {
         // move to data
         ifile.seekg(start_offset + entrylist[j].p3);
         if(ifile.fail()) return error("Failed to seek offset.");

         // read magic number #1
         uint64 magic1 = BE_read_uint64(ifile);
         if(magic1 != 0x4756455730313030) return error("Expecting GVEW0100.");

         // read magic number #2
         uint64 magic2 = BE_read_uint64(ifile);
         if(magic2 == 0x4A50454730313030) {
            std::string path = GetPathnameFromFilename(datlist[i]);
            if(!processGVEWJPEG(ifile, path.c_str(), entrylist[j].name.c_str())) return false;
           }
         else if(magic2 == 0x47564D5030313030) {
            std::string path = GetPathnameFromFilename(datlist[i]);
            if(!processGVEWGVMP(ifile, path.c_str(), entrylist[j].name.c_str())) return false;
           }
         else return error("Unknown entry type.");
        }

     // delete DAT file
     //ifile.close();
     //DeleteFileA(datlist[i].c_str());
    }

 return true;
}

bool extractor::process002(void)
{
 struct zarcentry {
  uint16 p1; // amount of zlib data to read
  uint16 p2; // ?
  uint32 p3; // zlib offset + 1
 };

 for(size_t i = 0; i < arclist.size(); i++)
    {
     // open file
     ifstream ifile(arclist[i].c_str(), ios::binary);
     if(!ifile) return error("Could not open ZARC file.");

     // read magic
     uint32 zarc = BE_read_uint32(ifile);
     if(zarc != 0x7A617263) return error("Invalid ZARC file.");
     uint32 vers = BE_read_uint16(ifile);
     uint32 n_streams = BE_read_uint16(ifile);
     BE_read_uint32(ifile);
     BE_read_uint32(ifile);

     // create output file
     std::string path = GetPathnameFromFilename(arclist[i]);
     std::string short_name = GetShortFilenameWithoutExtension(arclist[i]);
     stringstream fname;
     fname << path.c_str() << short_name.c_str();

     // create file
     cout << "Processing " << fname.str() << "." << endl;
     ofstream ofile(fname.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // read entries
     std::deque<zarcentry> entries;
     for(size_t j = 0; j < n_streams; j++) {
         zarcentry e;
         e.p1 = BE_read_uint16(ifile);
         e.p2 = BE_read_uint16(ifile);
         e.p3 = BE_read_uint32(ifile);
         entries.push_back(e);
        }

     // read streams
     for(size_t j = 0; j < entries.size(); j++) {
         ifile.seekg(entries[j].p3 - 1);
         if(!DecompressZLIB(ifile, entries[j].p1, ofile, -15)) return false;
        }

     // delete ZARC file
     //ifile.close();
     //DeleteFileA(arclist[i].c_str());
    }

 return true;
}

bool extractor::process003(void)
{
 for(size_t i = 0; i < hpklist.size(); i++)
    {
     // open file
     cout << "Processing " << hpklist[i].c_str() << "." << endl;
     ifstream ifile(hpklist[i].c_str(), ios::binary);
     if(!ifile) return error("Could not open HPK file.");

     // read magic
     uint32 magic = BE_read_uint32(ifile);
     if(magic != 0x30315048) return error("Expecting 01PH.");

     // read number of files
     uint32 n_files = BE_read_uint32(ifile);

     // read filesize
     uint32 filesize = BE_read_uint32(ifile);

     // read size of section
     uint32 sectionsize = BE_read_uint32(ifile);

     // read offset to filenames
     uint32 fname_offset = BE_read_uint32(ifile);

     // read offset to next section (can be different types)
     uint32 next_section = BE_read_uint32(ifile);

     // read unknowns
     uint16 unk001 = BE_read_uint16(ifile); // size of info data (0x20)
     uint16 unk002 = BE_read_uint16(ifile); // 0x10
     uint32 unk003 = BE_read_uint32(ifile); // 0 or 1

     // read file information
     struct FILEINFO { uint32 offset, filesize; };
     std::deque<FILEINFO> sizelist;
     for(size_t j = 0; j < n_files; j++) {
         uint32 param001 = BE_read_uint32(ifile);
         uint32 param002 = BE_read_uint32(ifile);
         uint32 param003 = BE_read_uint32(ifile);
         uint32 param004 = BE_read_uint32(ifile);
         uint32 param005 = BE_read_uint32(ifile); // offset from next_section
         uint32 param006 = BE_read_uint32(ifile); // filesize
         uint32 param007 = BE_read_uint32(ifile);
         uint32 param008 = BE_read_uint32(ifile);
         FILEINFO info = { param005, param006 };
         sizelist.push_back(info);
        }

     // move to string offset
     ifile.seekg(fname_offset);
     if(ifile.fail()) return error("Seek failure.");

     // read short filenames
     std::deque<std::string> filelist;
     for(size_t j = 0; j < n_files; j++) {
         char buffer[1024];
         read_string(ifile, buffer, 1024);
         if(ifile.fail()) return error("Read failure.");
         filelist.push_back(GetShortFilename(buffer));
        }

     // create directory to save files
     std::string fname_param1 = GetPathnameFromFilename(hpklist[i]);
     std::string fname_param2 = GetShortFilenameWithoutExtension(hpklist[i]);
     stringstream dirname;
     dirname << fname_param1 << fname_param2 << "\\";
     CreateDirectoryA(dirname.str().c_str(), NULL);

     // save files
     for(size_t j = 0; j < n_files; j++)
        {
         // move to data
         ifile.seekg(next_section + sizelist[j].offset);
         if(ifile.fail()) return error("Seek failure.");

         // create file
         stringstream ofname;
         ofname << dirname.str().c_str() << filelist[j].c_str();
         ofstream ofile(ofname.str().c_str(), ios::binary);
         if(!ofile) return error("Failed to create ouptut file.");

         // OK if document contains no data
         if(!sizelist[j].filesize) continue;

         // read data
         boost::shared_array<char> data(new char[sizelist[j].filesize]);
         ifile.read(data.get(), sizelist[j].filesize);
         if(ifile.fail()) return error("Read failure.");

         // save data
         ofile.write(data.get(), sizelist[j].filesize);
         if(ofile.fail()) return error("Write failure.");
        }

     // delete HPK file
     //ifile.close();
     //DeleteFileA(hpklist[i].c_str());
    }

 return true;
}

bool extractor::process004(void)
{
 for(size_t i = 0; i < pkclist.size(); i++)
    {
     // open file
     cout << "Processing " << pkclist[i].c_str() << "." << endl;
     ifstream ifile(pkclist[i].c_str(), ios::binary);
     if(!ifile) return error("Could not open PKCHR file.");

     // read magic
     uint32 magic = BE_read_uint32(ifile);
     if(magic != 0x30315048) return error("Expecting 01PH.");

     // read number of files
     uint32 n_files = BE_read_uint32(ifile);

     // read filesize
     uint32 filesize = BE_read_uint32(ifile);

     // read size of section
     uint32 sectionsize = BE_read_uint32(ifile);

     // read offset to filenames
     uint32 fname_offset = BE_read_uint32(ifile);

     // read offset to next section (can be different types)
     uint32 next_section = BE_read_uint32(ifile);

     // read unknowns
     uint16 unk001 = BE_read_uint16(ifile); // 0x20, 0x800
     uint16 unk002 = BE_read_uint16(ifile); // 0x10, 0x800
     uint32 unk003 = BE_read_uint32(ifile); // 0 or 1

     // read file information
     struct FILEINFO { uint32 offset, filesize; };
     std::deque<FILEINFO> sizelist;
     for(size_t j = 0; j < n_files; j++) {
         uint32 param001 = BE_read_uint32(ifile);
         uint32 param002 = BE_read_uint32(ifile);
         uint32 param003 = BE_read_uint32(ifile);
         uint32 param004 = BE_read_uint32(ifile);
         uint32 param005 = BE_read_uint32(ifile); // offset from next_section
         uint32 param006 = BE_read_uint32(ifile); // filesize
         uint32 param007 = BE_read_uint32(ifile);
         uint32 param008 = BE_read_uint32(ifile);
         FILEINFO info = { param005, param006 };
         sizelist.push_back(info);
        }

     // move to string offset
     ifile.seekg(fname_offset);
     if(ifile.fail()) return error("Seek failure.");

     // read short filenames
     std::deque<std::string> filelist;
     for(size_t j = 0; j < n_files; j++) {
         char buffer[1024];
         read_string(ifile, buffer, 1024);
         if(ifile.fail()) return error("Read failure.");
         filelist.push_back(GetShortFilename(buffer));
        }

     // save files
     std::string dirname = GetPathnameFromFilename(pkclist[i]);
     for(size_t j = 0; j < n_files; j++)
        {
         // move to data
         ifile.seekg(next_section + sizelist[j].offset);
         if(ifile.fail()) return error("Seek failure.");

         // create file
         stringstream ofname;
         ofname << dirname.c_str() << filelist[j].c_str();
         ofstream ofile(ofname.str().c_str(), ios::binary);
         if(!ofile) return error("Failed to create ouptut file.");

         // OK if document contains no data
         if(!sizelist[j].filesize) continue;

         // read data
         boost::shared_array<char> data(new char[sizelist[j].filesize]);
         ifile.read(data.get(), sizelist[j].filesize);
         if(ifile.fail()) return error("Read failure.");

         // save data
         ofile.write(data.get(), sizelist[j].filesize);
         if(ofile.fail()) return error("Write failure.");
        }

     // delete PKCHR file
     //ifile.close();
     //DeleteFileA(pkclist[i].c_str());
    }

 return true;
}

bool extractor::process005(void)
{
 for(size_t i = 0; i < ipklist.size(); i++)
    {
     // open file
     cout << "Processing " << ipklist[i].c_str() << "." << endl;
     ifstream ifile(ipklist[i].c_str(), ios::binary);
     if(!ifile) return error("Could not open IMGPAK file.");

     // read magic
     uint32 magic = BE_read_uint32(ifile);
     if(magic != 0x30315048) return error("Expecting 01PH.");

     // read number of files
     uint32 n_files = BE_read_uint32(ifile);

     // read filesize
     uint32 filesize = BE_read_uint32(ifile);

     // read size of section
     uint32 sectionsize = BE_read_uint32(ifile);

     // read offset to filenames
     uint32 fname_offset = BE_read_uint32(ifile);

     // read offset to next section (can be different types)
     uint32 next_section = BE_read_uint32(ifile);

     // read unknowns
     uint16 unk001 = BE_read_uint16(ifile);
     uint16 unk002 = BE_read_uint16(ifile);
     uint32 unk003 = BE_read_uint32(ifile);

     // read file information
     struct FILEINFO { uint32 offset, filesize; };
     std::deque<FILEINFO> sizelist;
     for(size_t j = 0; j < n_files; j++) {
         uint32 param001 = BE_read_uint32(ifile);
         uint32 param002 = BE_read_uint32(ifile);
         uint32 param003 = BE_read_uint32(ifile);
         uint32 param004 = BE_read_uint32(ifile);
         uint32 param005 = BE_read_uint32(ifile); // offset from next_section
         uint32 param006 = BE_read_uint32(ifile); // filesize
         uint32 param007 = BE_read_uint32(ifile);
         uint32 param008 = BE_read_uint32(ifile);
         FILEINFO info = { param005, param006 };
         sizelist.push_back(info);
        }

     // move to string offset
     ifile.seekg(fname_offset);
     if(ifile.fail()) return error("Seek failure.");

     // read short filenames
     std::deque<std::string> filelist;
     for(size_t j = 0; j < n_files; j++) {
         char buffer[1024];
         read_string(ifile, buffer, 1024);
         if(ifile.fail()) return error("Read failure.");
         filelist.push_back(GetShortFilename(buffer));
        }

     // save files
     std::string dirname = GetPathnameFromFilename(ipklist[i]);
     for(size_t j = 0; j < n_files; j++)
        {
         // move to data
         ifile.seekg(next_section + sizelist[j].offset);
         if(ifile.fail()) return error("Seek failure.");

         // create file
         stringstream ofname;
         ofname << dirname.c_str() << filelist[j].c_str();
         ofstream ofile(ofname.str().c_str(), ios::binary);
         if(!ofile) return error("Failed to create ouptut file.");

         // OK if document contains no data
         if(!sizelist[j].filesize) continue;

         // read data
         boost::shared_array<char> data(new char[sizelist[j].filesize]);
         ifile.read(data.get(), sizelist[j].filesize);
         if(ifile.fail()) return error("Read failure.");

         // save data
         ofile.write(data.get(), sizelist[j].filesize);
         if(ofile.fail()) return error("Write failure.");
        }
    }

 // delete ipklist
 ipklist.clear();
 return true;
}

bool extractor::process006(void)
{
 for(size_t i = 0; i < imglist.size(); i++)
    {
     // open file
     cout << "Processing " << imglist[i].c_str() << "." << endl;
     ifstream ifile(imglist[i].c_str(), ios::binary);
     if(!ifile) return error("Could not open IMG file.");

     // read magic
     uint32 magic = BE_read_uint32(ifile);
     if(magic != 0x494D3300) return error("Expecting IMG.");

     uint32 start = LE_read_uint32(ifile);

     // read number of images
     uint32 n_images = LE_read_uint32(ifile);
     if(!n_images) continue;

     // read strings
     std::deque<std::string> names;
     std::deque<size_t> sizes;
     for(size_t j = 0; j < n_images; j++)
        {
         // move to string
         uint32 position = start + j*0x40;
         ifile.seekg(position);
         if(ifile.fail()) return error("Seek failed.");

         // read string
         char buffer[1024];
         read_string(ifile, buffer, 1024);
         if(ifile.fail()) return error("Read failed.");
         names.push_back(string(buffer));

         // move to size
         position += 0x34;
         ifile.seekg(position);
         if(ifile.fail()) return error("Seek failed.");

         // read size
         uint32 size = LE_read_uint32(ifile);
         if(ifile.fail()) return error("Read failed.");
         sizes.push_back(size);
        }

     // move to first image
     uint32 position = start + n_images*0x40;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failed.");

     // read images
     std::string dirname = GetPathnameFromFilename(imglist[i]);
     for(size_t j = 0; j < n_images; j++)
        {
         // read image
         boost::shared_array<char> data(new char[sizes[j]]);
         ifile.read(data.get(), sizes[j]);
         if(ifile.fail()) return error("Read failed.");

         // skip image
         if(!strcmpi(names[j].c_str(), "#texanime"))
            continue;

         // save image
         stringstream ofname;
         ofname << dirname.c_str() << names[j].c_str() << ".dds";
         cout << ofname.str() << endl;
         ofstream ofile(ofname.str().c_str(), ios::binary);
         if(!ofile) return error("Failed to create DDS file.");
         ofile.write(data.get(), sizes[j]);
        }

     // delete IMG file
     //ifile.close();
     //DeleteFileA(imglist[i].c_str());
    }

 // delete image list
 imglist.clear();
 return true;
}

bool extractor::processGVEWJPEG(ifstream& ifile, const char* path, const char* name)
{
 // create directory
 stringstream ss;
 ss << path << GetShortFilenameWithoutExtension(name).c_str() << "\\";
 cout << ss.str().c_str() << endl;
 CreateDirectoryA(ss.str().c_str(), NULL);

 // read two numbers
 uint32 unk01 = BE_read_uint32(ifile);
 uint32 unk02 = BE_read_uint32(ifile);

 // read 1st block header
 uint32 block1_param1 = BE_read_uint32(ifile); if(block1_param1 != 0x424C4B5F) return error("Expecting GVEW block.");
 uint32 block1_param2 = BE_read_uint32(ifile);
 uint32 block1_param3 = BE_read_uint32(ifile);
 uint32 block1_param4 = BE_read_uint32(ifile);
 uint32 block1_param5 = BE_read_uint32(ifile);
 uint32 block1_param6 = BE_read_uint32(ifile);

 // read 1st block data
 uint32 n_items = (block1_param2 - 8)/32;
 std::deque<uint32> size_list;
  for(uint32 i = 0; i < n_items; i++) {
      uint32 items[8];
      BE_read_array(ifile, &items[0], 8);
      size_list.push_back(items[3]);
     }

 // read next block
 uint32 block2_param1 = BE_read_uint32(ifile);
 uint32 block2_param2 = BE_read_uint32(ifile);
 uint32 block2_param3 = BE_read_uint32(ifile);
 uint32 block2_param4 = BE_read_uint32(ifile);

 // read JPEG files
 for(uint32 i = 0; i < n_items; i++)
    {
     stringstream fname;
     fname << ss.str().c_str() << setfill('0') << setw(4) << i << ".jpg";
     ofstream ofile(fname.str().c_str(), ios::binary);
     if(!ofile) return error("");

     boost::shared_array<char> data(new char[size_list[i]]);
     ifile.read(data.get(), size_list[i]);
     if(ifile.fail()) return error("Read failure.");
     ofile.write(data.get(), size_list[i]);

     uint32 position = (uint32)ifile.tellg();
     position = ((position + 0xF) & ~(0xF));
     ifile.seekg(position);
    }

 return true;
}

bool extractor::processGVEWGVMP(ifstream& ifile, const char* path, const char* name)
{
 // create directory
 stringstream ss;
 ss << path << GetShortFilenameWithoutExtension(name).c_str() << "\\";
 cout << ss.str().c_str() << endl;
 CreateDirectoryA(ss.str().c_str(), NULL);

 // read two numbers
 uint32 unk01 = BE_read_uint32(ifile);
 uint32 unk02 = BE_read_uint32(ifile);

 // read 1st block header
 uint32 block1_param1 = BE_read_uint32(ifile); if(block1_param1 != 0x424C4B5F) return error("Expecting GVEW block.");
 uint32 block1_param2 = BE_read_uint32(ifile);
 uint32 block1_param3 = BE_read_uint32(ifile);
 uint32 block1_param4 = BE_read_uint32(ifile);
 uint32 block1_param5 = BE_read_uint32(ifile);
 uint32 block1_param6 = BE_read_uint32(ifile);

 // read 1st block data
 uint32 n_items = (block1_param2 - 8)/32;
 std::deque<uint32> size_list;
  for(uint32 i = 0; i < n_items; i++) {
      uint32 items[8];
      BE_read_array(ifile, &items[0], 8);
      size_list.push_back(items[3]);
     }

 // read 2nd block header
 uint32 block2_param1 = BE_read_uint32(ifile);
 uint32 block2_param2 = BE_read_uint32(ifile);
 uint32 block2_param3 = BE_read_uint32(ifile);
 uint32 block2_param4 = BE_read_uint32(ifile);

 // read 2nd block data
 for(uint32 i = 0; i < n_items; i++)
    {
     uint32 gvmp_param1 = BE_read_uint32(ifile);
     uint32 gvmp_param2 = BE_read_uint32(ifile);
     uint32 gvmp_param3 = BE_read_uint32(ifile);
     uint32 gvmp_param4 = BE_read_uint32(ifile);
     uint32 gvmp_param5 = BE_read_uint32(ifile);
     uint32 gvmp_param6 = BE_read_uint32(ifile);
     uint32 gvmp_param7 = BE_read_uint32(ifile);
     uint32 gvmp_param8 = BE_read_uint32(ifile);

     stringstream fname;
     fname << ss.str().c_str() << setfill('0') << setw(4) << i << "_a.jpg";
     ofstream ofile(fname.str().c_str(), ios::binary);
     if(!ofile) return error("Error creating output file.");

     uint32 datasize = gvmp_param4;
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);
     if(ifile.fail()) return error("Read failure.");
     ofile.write(data.get(), datasize);

     uint32 position = (uint32)ifile.tellg();
     position = ((position + 0xF) & ~(0xF));
     ifile.seekg(position);

     if(gvmp_param3 != gvmp_param5)
       {
        stringstream fname;
        fname << ss.str().c_str() << setfill('0') << setw(4) << i << "_b.jpg";
        ofstream ofile(fname.str().c_str(), ios::binary);
        if(!ofile) return error("");
        
        uint32 datasize = gvmp_param6;
        boost::shared_array<char> data(new char[datasize]);
        ifile.read(data.get(), datasize);
        if(ifile.fail()) return error("Read failure.");
        ofile.write(data.get(), datasize);
        
        uint32 position = (uint32)ifile.tellg();
        position = ((position + 0xF) & ~(0xF));
        ifile.seekg(position);
       }
    }

 return true;
}

void extractor::clear(void)
{
 arclist.clear();
 datlist.clear();
}

};};

namespace PS3 { namespace NI_NO_KUNI {

bool extract(void)
{
 string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pathname)
{
 return extractor(pathname).extract();
}

};};