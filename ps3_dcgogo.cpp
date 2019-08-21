#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "ps3_dcgogo.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   DREAMCLUB_GOGO

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool isCAT(const boost::shared_array<char>& data, uint32 size)
{
 // create binary stream
 if(size < 0x20) return false; // must have at least one file
 binary_stream bs(data, size);

 // determine number of offsets to read, maximum 0x1000
 uint32 n1 = 0x1000;
 uint32 n2 = size / 4;
 uint32 elem = (n1 < n2 ? n1 : n2);

 // read offsets
 boost::shared_array<uint32> offsets(new uint32[elem]);
 bs.BE_read_array(offsets.get(), elem);
 if(bs.fail()) return false;

 // find terminator
 uint32 terminator = 0xFFFFFFFFul;
 uint32 terminator_index = 0xFFFFFFFFul;
 for(uint32 i = 0; i < elem; i++) {
     // found terminator
     if(offsets[i] == terminator_index) {
        terminator_index = i;
        break;
       }
     // validate offset
     if(offsets[i] % 0x10) return false; // must be 0x04-byte aligned
     if(!(offsets[i] < size)) return false; // must be less than datasize
    }

 // validate terminator
 if(terminator_index == 0x00000000ul) return false;
 if(terminator_index == 0xFFFFFFFFul) return false;

 // each offset must be less than the last
 for(uint32 i = 1; i < terminator_index; i++)
     if(!(offsets[i - 1] < offsets[i])) return false;

 return true;
}

STDSTRING DetermineFileExtension(const boost::shared_array<char>& data, uint32 size)
{
 // read uint32
 binary_stream bs(data, size);
 uint32 h01 = bs.BE_read_uint32();
 if(bs.fail()) return STDSTRING(TEXT("unknown"));

 // return file extension
 switch(h01) {
   case(0x6F6D7464) : return STDSTRING(TEXT("dtm")); // DTMO
   case(0x020200FF) : return STDSTRING(TEXT("gtf")); // GTF
   case(0x30646D74) : return STDSTRING(TEXT("tmd")); // TMD0
   default : {
        if(isCAT(data, size)) return STDSTRING(TEXT("cat")); // NESTED CAT FILE
        else return STDSTRING(TEXT("unknown"));
       }
  }

 // unknown
 return STDSTRING(TEXT("unknown"));
}

bool processCAT(LPCTSTR filename)
{
 // CAT TYPE A = no filenames, 0xFFFFFFFF terminator
 // NOTE THAT CAT FILES CAN BE NESTED
 // FORMAT
 // deque<uint32> offsets to data;
 // uint32 0xFFFFFFFF;

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // nothing to do
 if(filesize < 0x20) return true;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read offsets until terminator
 deque<uint32> offsets;
 for(;;) {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     if(item == 0xFFFFFFFF) break;
     offsets.push_back(item);
    }

 // must have offsets
 if(!offsets.size()) return true;

 // create savepath
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // save files
 for(uint32 i = 0; i < offsets.size(); i++)
    {
     // compute offsets
     uint32 a = offsets[i];
     uint32 b = ((i == (offsets.size() - 1)) ? filesize : offsets[i + 1]);
     if(a > b) return error("Invalid CAT file.");

     // move to offset
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     uint32 size = b - a;
     boost::shared_array<char> data(new char[size]);
     ifile.read(data.get(), size);
     if(ifile.fail()) return error("Read failure.");

     // create filename
     STDSTRING extension = DetermineFileExtension(data, size);
     STDSTRINGSTREAM ss;
     ss << savepath;
     ss << setfill(TEXT('0')) << setw(3) << i;
     ss << TEXT('.') << extension;

     // create file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save data
     ofile.write(data.get(), size);
     if(ofile.fail()) return error("Write failure.");
    }

 // delete file
 ifile.close();
 DeleteFile(filename);
 return true;
}

bool processCAN(LPCTSTR filename)
{
 // CAN = filenames, 0xFFFFFFFF, 0xFFFFFFFF terminator pair
 // FORMAT
 // deque<tuple<uint32,uint32,uint32>> offset-size-offset tuples;
 // uint32 0xFFFFFFFF;
 // uint32 0xFFFFFFFF;

 // 00 00 00 40 -- offset
 // 00 00 64 48 -- size of data to read
 // 00 00 64 90 -- offset to filename
 // 00 00 64 A0 --
 // 00 00 61 F8 --
 // 00 00 C6 A0 -- offset to filename
 // 00 00 C6 B0 00 05 81 00 00 06 47 B0 00 06 47 C0 00 05 81 00 00 0B C8 C0 FF FF FF FF FF FF FF FF

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // nothing to do
 if(filesize < 0x20) return true;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read offsets until terminator
 deque<tuple<uint32,uint32,uint32>> offsets;
 for(;;) {
     uint32 item1 = BE_read_uint32(ifile);
     uint32 item2 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     if(item1 == 0xFFFFFFFFul && item2 == 0xFFFFFFFFul) break;
     uint32 item3 = BE_read_uint32(ifile);
     offsets.push_back(make_tuple(item1, item2, item3));
    }

 // must have offsets
 if(!offsets.size()) return true;

 // create savepath
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // save files
 for(uint32 i = 0; i < offsets.size(); i++)
    {
     // get tuple data
     uint32 p01 = std::get<0>(offsets[i]);
     uint32 p02 = std::get<1>(offsets[i]);
     uint32 p03 = std::get<2>(offsets[i]);

     // move to offset
     ifile.seekg(p01);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     uint32 size = p02;
     boost::shared_array<char> data(new char[size]);
     ifile.read(data.get(), size);
     if(ifile.fail()) return error("Read failure.");

     // move to filename
     ifile.seekg(p03);
     if(ifile.fail()) return error("Seek failure.");

     // read filename
     boost::shared_array<char> name(new char[0x40]);
     if(!read_string(ifile, name.get(), 0x40)) return error("Read failure.");

     // convert to UTF-16
     STDSTRING utfname = Unicode08ToUnicode16(name.get()).get();
     if(!utfname.length()) return error("Invalid filename associated with this CAN entry.");

     // create filename
     STDSTRINGSTREAM ss;
     ss << savepath << utfname.c_str();

     // create file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save data
     ofile.write(data.get(), size);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
}

}};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processGTF(LPCTSTR filename)
{
 // HEADER (0x0C bytes)
 // ITEM LIST (0x24 bytes per item)
 // ITEM DATA
 // 00000000 00000180 00020000 86010200 0000AAE4 0200 0200 0001 0000 00000000 00000000
 // 00000001 00020180 00000800 86010200 0000AAE4 0040 0040 0001 0000 00000000 00000000
 // 00000002 00020980 00020000 86010200 0000AAE4 0200 0200 0001 0000 00000000 00000000
 // 00000003 00040980 00000800 86010200 0000AAE4 0040 0040 0001 0000 00000000 00000000
 // 00000004 00041180 00020000 86010200 0000AAE4 0200 0200 0001 0000 00000000 00000000
 // 00000005 00061180 00000800 86010200 0000AAE4 0040 0040 0001 0000 00000000 00000000
 // 00000006 00061980 00002000 88010200 0000AAE4 0080 0040 0001 0000 00000000 00000000
 // 0x8601 -> (0x200 * 0x200)/2 = 0x20000 (so DXT1)
 // 0x8801 -> (0x080 * 0x040)/1 = 0x02000 (so DXT5)

 struct GTFITEM {
  uint32 p01; // index
  uint32 p02; // offset
  uint32 p03; // size of texture item
  uint32 p04; // flags
  uint32 p05; // flags
  uint16 p06; // dx
  uint16 p07; // dy
  uint16 p08; // mipmaps
  uint16 p09; // unknown
  uint32 p10; // unknown
  uint32 p11; // unknown
 };

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = BE_read_uint32(ifile); // magic
 uint32 h02 = BE_read_uint32(ifile); // texture data in bytes
 uint32 h03 = BE_read_uint32(ifile); // number of textures
 if(ifile.fail()) return error("Read failure.");

 // validate header
 if(h01 != 0x020200FFul) return error("Invalid magic number.");
 if(h02 == 0) return true; // nothing to do
 if(h03 == 0) return true; // nothing to do

 // read items
 deque<GTFITEM> itemlist;
 for(uint32 i = 0; i < h03; i++) {
     GTFITEM item;
     item.p01 = BE_read_uint32(ifile); // index
     item.p02 = BE_read_uint32(ifile); // offset
     item.p03 = BE_read_uint32(ifile); // size of texture item
     item.p04 = BE_read_uint32(ifile); // flags
     item.p05 = BE_read_uint32(ifile); // flags
     item.p06 = BE_read_uint16(ifile); // dx
     item.p07 = BE_read_uint16(ifile); // dy
     item.p08 = BE_read_uint16(ifile); // mipmaps
     item.p09 = BE_read_uint16(ifile); // unknown
     item.p10 = BE_read_uint32(ifile); // unknown
     item.p11 = BE_read_uint32(ifile); // unknown
     if(ifile.fail()) return error("Read failure.");
     itemlist.push_back(item);
    }

 // create savepath
 STDSTRING savepath = pathname;
 if(h03 > 1) savepath = CreateSavePath(pathname.c_str(), shrtname.c_str());
 if(!savepath.length()) return false;

 // process textures
 for(uint32 i = 0; i < h03; i++)
    {
     // skip item
     GTFITEM item = itemlist[i];
     if(!item.p03) continue;

     // move to data
     ifile.seekg(item.p02);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[item.p03]);
     ifile.read(data.get(), item.p03);
     if(ifile.fail()) return error("Read failure.");

     // create DDS header
     DDS_HEADER ddsh;
     uint32 flag = (item.p04 & 0xFF000000) >> 24;
     uint32 mips = (item.p04 & 0x00FF0000) >> 16;
     switch(flag) {
       case(0x85) : // BGRA morton
       case(0xBE) : {
            if(item.p06 == item.p07) { // only do if equal dimensions
               boost::shared_array<char> copy(new char[item.p03]);
               for(uint32 r = 0; r < item.p07; r++) {
                   for(uint32 c = 0; c < item.p06; c++) {
                       uint32 morton = array_to_morton(r, c);
                       uint32 copy_position = 4*r*item.p06 + 4*c;
                       uint32 data_position = 4*morton;
                       copy[copy_position + 0] = data[data_position + 0];
                       copy[copy_position + 1] = data[data_position + 1];
                       copy[copy_position + 2] = data[data_position + 2];
                       copy[copy_position + 3] = data[data_position + 3];
                      }
                  }
               data = copy;
              }
            else
               message(" Non-square morton. Skipping.");
           }
       case(0x9E) : // BGRA non-morton
       case(0xA5) : {
            // swap channels so shit loads in photoshop
            uint32 n_pixels = item.p06 * item.p07;
            for(uint32 j = 0; j < n_pixels; j++) {
                uint32 index1 = 4*j;
                uint32 index2 = index1 + 1;
                uint32 index3 = index2 + 1;
                uint32 index4 = index3 + 1;
                swap(data[index1], data[index4]); // swap A and B
                swap(data[index2], data[index3]); // swap R and G
               }
            uint32 m1 = 0xFF000000; // A
            uint32 m2 = 0x00FF0000; // R 
            uint32 m3 = 0x0000FF00; // G
            uint32 m4 = 0x000000FF; // B
            CreateUncompressedDDSHeader(item.p06, item.p07, mips - 1, 32, m2, m3, m4, m1, FALSE, &ddsh);
            break;
           }
       // 0x86 = DXT1
       case(0x86) : {
            CreateDXT1Header(item.p06, item.p07, mips - 1, FALSE, &ddsh);
            break;
           }
       // 0x87 = DXT3
       // 00000000 00000080 00010000 87010200 0000AAE4 0100 0100
       case(0x87) : {
            CreateDXT3Header(item.p06, item.p07, mips - 1, FALSE, &ddsh);
            break;
           }
       // 0x88 = DXT5
       case(0x88) : {
            CreateDXT5Header(item.p06, item.p07, mips - 1, FALSE, &ddsh);
            break;
           }
       // 0xA3 = R5G6B5 or A1R5G5B5
       case(0xA3) : {
            CreateR5G6B5DDSHeader(item.p06, item.p07, mips - 1, FALSE, &ddsh);
            break;
           }
       // 0xA6 = DXT1
       case(0xA6): {
            CreateDXT1Header(item.p06, item.p07, mips - 1, FALSE, &ddsh);
            break;
           }
       // 0xA8 = DXT5
       case(0xA8) : {
            CreateDXT5Header(item.p06, item.p07, mips - 1, FALSE, &ddsh);
            break;
           }
       default : return error("Unknown GTF format.");
      }

     // create filename based on index
     STDSTRINGSTREAM ofname;
     ofname << savepath;
     if(h03 > 1) ofname << setfill(TEXT('0')) << setw(4) << item.p01;
     else ofname << shrtname;
     ofname << TEXT(".dds");

     // save DDS file
     if(!SaveDDSFile(ofname.str().c_str(), ddsh, data, item.p03))
        return error("Failed to save DDS file.");
    }

 // delete file
 //ifile.close();
 //DeleteFile(filename);
 return true;
}

}};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

bool processTMD(LPCTSTR filename)
{
 // uint32 0x30646D74
 // uint32 flags (0x09C7)
 // uint16 0x0100
 // uint16 0x0100
 // uint32 offset to start (always 0x28???)
 // real32[3] bounding box min
 // real32[3] bounding box max

 // uint32 offset (always 0xA0???)
 // uint32 number of 0x20-byte items at offset
 // uint32 offset
 // uint32 number of 0x02-byte items at offset
 // uint32 0x00000000 ????
 // uint32 0x00000000 ????
 // uint32 offset
 // uint32 0x00000000 ????
 // uint32 offset
 // uint32 number of 0xC8-byte items at offset
 // uint32 offset
 // uint32 number of 0x08-byte items at offset
 // uint32 offset to index buffer
 // uint32 number of triangles (this number * 0x2 * 0x3 = number of bytes of index buffer)

 return true;
}

}};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PS3] Dream C Club Gogo!");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where game files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~??? GB free space.\n");
 p3 += TEXT("3. 5 GB for game + ??? GB extraction.\n");

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
 bool doCAN = true;
 bool doCAT = true;
 bool doGTF = true;
 bool doTMD = true;

 // questions
 if(doCAN) doCAN = YesNoBox("Process archive (CAN) files?");
 if(doCAT) doCAT = YesNoBox("Process archive (CAT) files?");
 if(doGTF) doGTF = YesNoBox("Process texture (GTF) files?");
 if(doTMD) doTMD = YesNoBox("Process model (TMD) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process archive
 cout << "STAGE 1" << endl;
 if(doCAN) {
    // process CAN files
    cout << "Processing .CAN files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".CAN"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCAN(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 2" << endl;
 if(doCAT) {
    deque<STDSTRING> filelist;
    uint32 counter = 0;
    for(;;)
       {
        // number of files
        uint32 n = 0;

        // process CMB files (also CAT files)
        cout << "Processing .CMB files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".CMB"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process CMF files (also CAT files)
        cout << "Processing .CMF files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".CMF"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process CMM files (also CAT files)
        cout << "Processing .CMM files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".CMM"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process COS files (also CAT files)
        cout << "Processing .COS files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".COS"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process INR files (also CAT files)
        cout << "Processing .INR files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".INR"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process KMC files (also CAT files)
        cout << "Processing .KMC files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".KMC"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process STG files (also CAT files)
        cout << "Processing .STG files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".STG"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process CKIF files (also CAT files)
        cout << "Processing .CKIF files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".CKIF"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process CLIP files (also CAT files)
        cout << "Processing .CLIP files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".CLIP"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process CSPT files (also CAT files)
        cout << "Processing .CSPT files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".CSPT"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process FACE files (also CAT files)
        cout << "Processing .FACE files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".FACE"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process HAIR files (also CAT files)
        cout << "Processing .HAIR files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".HAIR"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process SOFA files (also CAT files)
        cout << "Processing .SOFA files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".SOFA"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // process CAT files
        cout << "Processing .CAT files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".CAT"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processCAT(filelist[i].c_str())) return false;
           }
        cout << endl;
        n += filelist.size();

        // last loop did nothing
        if(!n) break;

        // count number of unpack loops
        counter++;
        if(counter > 20) return error("Recursive CAT file loop counter reached its limit. Please run as Administrator.");
       }
   }

 // process textures
 cout << "STAGE 3" << endl;
 if(doGTF) {
    // process GTF files
    cout << "Processing .GTF files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".GTF"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processGTF(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process model
 cout << "STAGE 4" << endl;
 if(doTMD) {
    // process TMD files
    cout << "Processing .TMD files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".TMD"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTMD(filelist[i].c_str())) return false;
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