#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_zlib.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "pc_witcher3.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   WITCHER_3

//
// MODELS
//
namespace X_SYSTEM { namespace X_GAME {

bool processW2M(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // string table information
 ifile.seekg(0x28);
 if(ifile.fail()) return error("Seek failure.");
 uint32 strtable_offset = LE_read_uint32(ifile);
 uint32 strtable_size = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to string table
 ifile.seekg(strtable_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read string table
 deque<string> strtable;
 uint32 bytes_read = 0;
 for(;;)
    {
     // read string
     char str[1024];
     if(!read_string(ifile, &str[0], 1024))
        return error("Read failure.");

     // insert string
     cout << "0x" << hex << strtable.size() << dec << " = " << str << endl;
     strtable.push_back(str);

     // increment bytes read
     bytes_read += strlen(str) + 1;
     if(!(bytes_read < strtable_size)) break;
    } 
 cout << "strtable size = 0x" << hex << strtable.size() << dec << endl;

 // material information
 ifile.seekg(0x40);
 if(ifile.fail()) return error("Seek failure.");
 uint32 matinfo_offset = LE_read_uint32(ifile);
 uint32 matinfo_size = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to material information
 ifile.seekg(matinfo_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read material information
 for(uint32 i = 0; i < matinfo_size; i++) {
     LE_read_uint32(ifile); // ???
     LE_read_uint32(ifile); // ???
     if(ifile.fail()) return error("Read failure.");
    }

 // 18 08 00 00 - 04 00 00 00 (CMaterialInstance)
 // 59 08 00 00 - 07 00 00 00 (CMaterialGraph)
 // 8D 08 00 00 - 0A 00 00 00 (CBitmapTexture)
 // CC 08 00 00 - 0A 00 00 00 (CBitmapTexture)
 // 0B 09 00 00 - 0A 00 00 00 (CBitmapTexture)
 // 4A 09 00 00 - 0A 00 00 00 (CBitmapTexture)
 // 92 09 00 00 - 0A 00 00 00 (CBitmapTexture)
 // DA 09 00 00 - 0A 00 00 00 (CBitmapTexture)
 // 19 0A 00 00 - 0A 00 00 00 (CBitmapTexture)
 // 63 0A 00 00 - 0A 00 00 00 (CBitmapTexture)
 // AD 0A 00 00 - 07 00 00 00 (CMaterialGraph)
 // E8 0A 00 00 - 0A 00 00 00 (CBitmapTexture)
 // 32 0B 00 00 - 07 00 00 00 (CMaterialGraph)

 //
 // SCENE INFORMATION
 //

 struct W3MINFO {
  uint32 p01; // ???
  uint32 p02; // ??? type ???
  uint32 p03; // size of data
  uint32 p04; // offset to data
  uint32 p05; // ???
  uint32 p06; // ???
 };
 deque<W3MINFO> infolist;

 // ??? information
 ifile.seekg(0x58);
 if(ifile.fail()) return error("Seek failure.");
 uint32 info_offset = LE_read_uint32(ifile);
 uint32 info_size = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to ??? information
 ifile.seekg(info_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read ??? information
 for(uint32 i = 0; i < info_size; i++) {
     W3MINFO item;
     item.p01 = LE_read_uint32(ifile); // ???
     item.p02 = LE_read_uint32(ifile); // ??? type ???
     item.p03 = LE_read_uint32(ifile); // size of data
     item.p04 = LE_read_uint32(ifile); // offset to data
     item.p05 = LE_read_uint32(ifile); // ???
     item.p06 = LE_read_uint32(ifile); // ???
     if(ifile.fail()) return error("Read failure.");
     infolist.push_back(item);
    }

 //
 // BUFFER INFORMATION
 //

 // buffer information
 ifile.seekg(0x64);
 if(ifile.fail()) return error("Seek failure.");
 uint32 buffinfo_offset = LE_read_uint32(ifile);
 uint32 buffinfo_size = LE_read_uint32(ifile); // number of buffer files
 if(ifile.fail()) return error("Read failure.");

 // move to buffer information
 ifile.seekg(buffinfo_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read buffer information
 for(uint32 i = 0; i < buffinfo_size; i++)
    {
     LE_read_uint32(ifile); // ???
     LE_read_uint32(ifile); // ???
     LE_read_uint32(ifile); // ???
     LE_read_uint32(ifile); // size of buffer file
     LE_read_uint32(ifile); // size of buffer file
     LE_read_uint32(ifile); // ???
     if(ifile.fail()) return error("Read failure.");
    }

 //
 // PROCESS SCENE INFORMATION
 //

 // read ??? information
 for(uint32 i = 0; i < infolist.size(); i++)
    {
     // move to offset
     uint32 offset = infolist[i].p04;
     ifile.seekg(offset);
     if(ifile.fail()) return error("Seek failure.");

     // read a single byte (don't know what this is for)
     LE_read_uint08(ifile); // ???

     // keep track of bytes read
     uint32 datasize = infolist[i].p03;
     uint32 n_chunks = 0;
     bytes_read = 0;

     for(;;)
        {
         // save position
         uint32 position = (uint32)ifile.tellg();
         cout << "offset 0x" << hex << position << dec << endl;

         // read chunk attributes
         uint16 node_name_idx = LE_read_uint16(ifile);
         if(node_name_idx == 0x00) {
            cout << "number of chunks read = 0x" << hex << n_chunks << dec << endl;
            break;
           }
         else
            n_chunks++;

         uint16 node_type_idx = LE_read_uint16(ifile);
         uint32 node_size = LE_read_uint32(ifile);
         bytes_read += 8;

         // validate indices
         if(!(node_name_idx < strtable.size())) {
            stringstream ss;
            ss << "Invalid index at 0x";
            ss << hex << position << dec << ".";
            return error(ss);
           }
         if(!(node_type_idx < strtable.size())) {
            stringstream ss;
            ss << "Invalid index at 0x";
            ss << hex << position << dec << ".";
            return error(ss);
           }

         // chunk attributes
         string node_name = strtable[node_name_idx];
         string node_type = strtable[node_type_idx];

         if(node_name == "autoHideDistance")
           {
            // skip for now
            uint32 skip_bytes = node_size - 0x04;
            bytes_read += skip_bytes;
            ifile.seekg(skip_bytes, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(node_name == "baseMaterial")
           {
            // skip for now
            uint32 skip_bytes = node_size - 0x04;
            bytes_read += skip_bytes;
            ifile.seekg(skip_bytes, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(node_name == "boundingBox")
           {
            // skip for now
            uint32 skip_bytes = node_size - 0x04;
            bytes_read += skip_bytes;
            ifile.seekg(skip_bytes, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(node_name == "chunks")
           {
            // skip for now
            uint32 skip_bytes = node_size - 0x04;
            bytes_read += skip_bytes;
            ifile.seekg(skip_bytes, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(node_name == "cookedData")
           {
            // skip for now (can be a lot of data)
            uint32 skip_bytes = node_size - 0x04;
            bytes_read += skip_bytes;
            ifile.seekg(skip_bytes, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(node_name == "generalizedMeshRadius")
           {
            // skip for now
            uint32 skip_bytes = node_size - 0x04;
            bytes_read += skip_bytes;
            ifile.seekg(skip_bytes, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(node_name == "internalVersion")
           {
            // skip for now
            uint32 skip_bytes = node_size - 0x04;
            bytes_read += skip_bytes;
            ifile.seekg(skip_bytes, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(node_name == "materials")
           {
            // skip for now
            uint32 skip_bytes = node_size - 0x04;
            bytes_read += skip_bytes;
            ifile.seekg(skip_bytes, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else if(node_name == "soundInfo")
           {
            // skip for now
            uint32 skip_bytes = node_size - 0x04;
            bytes_read += skip_bytes;
            ifile.seekg(skip_bytes, ios::cur);
            if(ifile.fail()) return error("Seek failure.");
           }
         else {
            stringstream ss;
            ss << "Unknown node " << node_name << " ";
            ss << "at 0x" << hex << position << dec << ".";
            return error(ss);
           }

         // finished
         if(!(bytes_read < datasize)) break;
        }
    }

 return true;
}

}}

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processBUN(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // header
 struct BUNHEADER {
  uint64 h01; // 0x30374F5441544F50
  uint32 h02; // ???
  uint32 h03; // ???
  uint32 h04; // size of filename info section
  uint16 h05; // ???
  uint16 h06; // ???
  uint32 h07; // padding
  uint32 h08; // padding
 };

 // read header
 BUNHEADER bh;
 bh.h01 = LE_read_uint64(ifile);
 bh.h02 = LE_read_uint32(ifile);
 bh.h03 = LE_read_uint32(ifile);
 bh.h04 = LE_read_uint32(ifile);
 bh.h05 = LE_read_uint16(ifile);
 bh.h06 = LE_read_uint16(ifile);
 bh.h07 = LE_read_uint32(ifile);
 bh.h08 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // file information
 struct BUNFILEINFO {
  sint08 p01[0x100]; // filename
  uint32 p02; // ???
  uint32 p03; // ???
  uint32 p04; // ???
  uint32 p05; // ???
  uint32 p06; // ???
  uint32 p07; // data size???
  uint32 p08; // data size???
  uint32 p09; // ???
  uint32 p10; // ???
  uint32 p11; // ???
  uint32 p12; // ???
  uint32 p13; // ???
  uint32 p14; // ???
  uint32 p15; // ???
  uint32 p16; // ???
  uint32 p17; // ???
 };
 deque<BUNFILEINFO> infolist;

 // read file info
 uint32 n_files = bh.h04/0x140;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read filename
     BUNFILEINFO bfi;
     ifile.read(&bfi.p01[0], 0x100);
     if(ifile.fail()) return error("Read failure.");

     // read parameters
     bfi.p02 = LE_read_uint32(ifile); // ???
     bfi.p03 = LE_read_uint32(ifile); // ???
     bfi.p04 = LE_read_uint32(ifile); // ???
     bfi.p05 = LE_read_uint32(ifile); // ???
     bfi.p06 = LE_read_uint32(ifile); // ???
     bfi.p07 = LE_read_uint32(ifile); // inflated data size
     bfi.p08 = LE_read_uint32(ifile); // deflated data size
     bfi.p09 = LE_read_uint32(ifile); // absolute file offset
     bfi.p10 = LE_read_uint32(ifile); // ???
     bfi.p11 = LE_read_uint32(ifile); // ???
     bfi.p12 = LE_read_uint32(ifile); // ???
     bfi.p13 = LE_read_uint32(ifile); // ???
     bfi.p14 = LE_read_uint32(ifile); // ???
     bfi.p15 = LE_read_uint32(ifile); // ???
     bfi.p16 = LE_read_uint32(ifile); // ???
     bfi.p17 = LE_read_uint32(ifile); // file type
     if(ifile.fail()) return error("Read failure.");

     // append info
     infolist.push_back(bfi);
    }

 // process files
 for(uint32 i = 0; i < n_files; i++)
    {
     // properties
     uint32 inflate_size = infolist[i].p07;
     uint32 deflate_size = infolist[i].p08;
     uint32 offset = infolist[i].p09;
     uint32 filetype = infolist[i].p17;
     if(!deflate_size) continue;
     if(!inflate_size) continue;

     // construct full filename/pathname
     STDSTRING fname = pathname;
     fname += Unicode08ToUnicode16(infolist[i].p01).get();
     STDSTRING pname = GetPathnameFromFilename(fname.c_str()).get();
     if(!CreateShellSavePath(pname.c_str())) return error("Failed to construct save path.");

     // create file
     ofstream ofile(fname.c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // uncompressed
     if(filetype == 0 || filetype == 3)
       {
        // move to offset
        ifile.seekg(offset);
        if(ifile.fail()) return error("Seek failure.");

        // read data
        boost::shared_array<char> data(new char[inflate_size]);
        ifile.read(data.get(), inflate_size);
        if(ifile.fail()) return error("Read failure.");

        // save data
        ofile.write(data.get(), inflate_size);
        if(ofile.fail()) return error("Write failure.");
       }
     // ZLIB
     else if(filetype == 1)
       {
        // prepare compressed item
        ZLIBINFO2 item;
        item.deflatedBytes = deflate_size;
        item.inflatedBytes = inflate_size;
        item.offset = offset;
        cout << "deflate size = 0x" << hex << deflate_size << dec << endl;
        cout << "inflate size = 0x" << hex << inflate_size << dec << endl;
        cout << "offset = 0x" << hex << offset << dec << endl;
        
        // insert compressed item
        deque<ZLIBINFO2> zlist;
        zlist.push_back(item);
        if(!InflateZLIB(ifile, zlist, 0, ofile)) return false;
       }
     else
        return error("Unknown bundle file type.");
    }

 return true;
}

bool processCC3(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // header
 struct CCHEADER {
  uint32 h01; // 0x57334343
  uint32 h02; // 0x09
  uint32 h03; // 0x7DF1A000
  uint32 h04; // 0x03165C2B
  uint32 h05; // offset to 3rd part of file
  uint32 h06; // number of ?
  uint32 h07; // offset to 2nd part of file
  uint32 h08; // number of ?
  uint32 h09;
  uint32 h10;
  uint32 h11;
  uint32 h12;
 };

 // read header
 CCHEADER cch;
 cch.h01 = LE_read_uint32(ifile);
 cch.h02 = LE_read_uint32(ifile);
 cch.h03 = LE_read_uint32(ifile);
 cch.h04 = LE_read_uint32(ifile);
 cch.h05 = LE_read_uint32(ifile);
 cch.h06 = LE_read_uint32(ifile);
 cch.h07 = LE_read_uint32(ifile);
 cch.h08 = LE_read_uint32(ifile);
 cch.h09 = LE_read_uint32(ifile);
 cch.h10 = LE_read_uint32(ifile);
 cch.h11 = LE_read_uint32(ifile);
 cch.h12 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to strings
 ifile.seekg(cch.h07);
 if(ifile.fail()) return error("Seek failure.");

 // read strings
 deque<string> filelist;
 for(uint32 i = 0; i < cch.h06; i++) {
     char buffer[1024];
     if(!read_string(ifile, &buffer[0], 1024)) return error("Read failure.");
     filelist.push_back(buffer);
    }

 // file information
 struct CCFILEINFO {
  uint64 p01; // id
  uint64 p02; // ???
  uint32 p03; // offset to zlib data
  uint32 p04; // size of zlib data
  uint32 p05; // size of decompressed data?
  uint32 p06; // 0x00
  uint64 p07; // ???
  uint64 p08; // ??? typically same for all items
  real32 p09; // ???
  real32 p10; // ???
  real32 p11; // ???
  real32 p12; // ???
  uint32 p13; // ???
  uint32 p14; // ???
 };

 // move to file information data
 ifile.seekg(cch.h05);
 if(ifile.fail()) return error("Seek failure.");

 // read file information
 deque<CCFILEINFO> infolist;
 for(uint32 i = 0; i < cch.h06; i++)
    {
     CCFILEINFO cfi;
     cfi.p01 = LE_read_uint64(ifile);
     cfi.p02 = LE_read_uint64(ifile);
     cfi.p03 = LE_read_uint32(ifile);
     cfi.p04 = LE_read_uint32(ifile);
     cfi.p05 = LE_read_uint32(ifile);
     cfi.p06 = LE_read_uint32(ifile);
     cfi.p07 = LE_read_uint64(ifile);
     cfi.p08 = LE_read_uint64(ifile);
     cfi.p09 = LE_read_real32(ifile);
     cfi.p10 = LE_read_real32(ifile);
     cfi.p11 = LE_read_real32(ifile);
     cfi.p12 = LE_read_real32(ifile);
     cfi.p13 = LE_read_uint32(ifile);
     cfi.p14 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     infolist.push_back(cfi);
    }

 // process files
 for(uint32 i = 0; i < cch.h06; i++)
    {
     // data sizes
     uint32 offset = infolist[i].p03;       // offset to zlib data
     uint32 deflate_size = infolist[i].p04; // size of zlib data
     uint32 inflate_size = infolist[i].p05; // size of decompressed data?
     if(!deflate_size) continue;
     if(!inflate_size) continue;

     // construct full filename/pathname
     STDSTRING fname = pathname;
     fname += Unicode08ToUnicode16(filelist[i].c_str()).get();
     STDSTRING pname = GetPathnameFromFilename(fname.c_str()).get();
     if(!CreateShellSavePath(pname.c_str())) return error("Failed to construct save path.");

     // create file
     ofstream ofile(fname.c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // prepare compressed item
     ZLIBINFO2 item;
     item.deflatedBytes = deflate_size;
     item.inflatedBytes = inflate_size;
     item.offset = offset;
     // cout << "deflate size = 0x" << hex << deflate_size << dec << endl;
     // cout << "inflate size = 0x" << hex << inflate_size << dec << endl;
     // cout << "offset = 0x" << hex << offset << dec << endl;

     // insert compressed item
     deque<ZLIBINFO2> zlist;
     zlist.push_back(item);
     if(!InflateZLIB(ifile, zlist, 0, ofile)) return false;
    }

 return true;
}

}}

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Witcher 3");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where CACHE files are.\n");
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
 LPCTSTR pathname = GetModulePathname().get();
 return extract(pathname);
}

bool extract(LPCTSTR pname)
{
 // set pathname
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // booleans
 bool doBUN = true;
 bool doCC3 = true;
 bool doW2M = true;

 // questions
 // if(doBUN) doBUN = YesNoBox("Process bundle files?");
 // if(doCC3) doCC3 = YesNoBox("Process collision.cache files?");
 if(doW2M) doW2M = YesNoBox("Process W2MESH files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // proces bundle files
 if(doW2M)
   {
    cout << "Processing .W2MESH files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".W2MESH"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        processW2M(filelist[i].c_str());
       }
    cout << endl;
   }

 // // proces bundle files
 // if(doBUN)
 //   {
 //    cout << "Processing .bundle files..." << endl;
 //    deque<STDSTRING> filelist;
 //    BuildFilenameList(filelist, TEXT(".bundle"), pathname.c_str());
 //    for(size_t i = 0; i < filelist.size(); i++) {
 //        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
 //        processBUN(filelist[i].c_str());
 //       }
 //    cout << endl;
 //   }
 // 
 // // process cache files
 // cout << "Processing .cache files..." << endl;
 // deque<STDSTRING> filelist;
 // BuildFilenameList(filelist, TEXT(".cache"), pathname.c_str());
 // for(size_t i = 0; i < filelist.size(); i++) {
 //     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
 //     auto fname = GetShortFilename(filelist[i].c_str());
 //     if(doCC3 && (fname.get() == STDSTRING(TEXT("collision.cache"))))
 //        if(!processCC3(filelist[i].c_str())) return false;
 //    }
 // cout << endl;

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

bool test(void)
{
 return processW2M(TEXT("C:\\Temp\\witcher\\characters\\models\\main_npc\\ciri\\model\\body_01_wa__ciri.w2mesh"));
}

}};