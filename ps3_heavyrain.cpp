#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "ps3_heavyrain.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   HEAVYRAIN

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processDAT(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize || filesize < 0x800) return error("Nothing to process.");

 // create file list
 struct DATINFO {
   uint32 offset;
   uint32 type;
 };
 deque<DATINFO> infolist;

 // build file list
 uint32 offset = 0x800;
 for(;;)
    {
     // seek offset
     ifile.seekg(offset);
     if(ifile.fail()) break;

     // read 4 bytes
     char data[5];
     ifile.read(&data[0], 4);
     if(ifile.fail()) break;
     data[4] = '\0';

     // insert file
     if(strcmp(data, "PART") == 0) {
        DATINFO item;
        item.offset = offset;
        item.type = 0;
        infolist.push_back(item);
       }
     else if(strcmp(data, "QZIP") == 0) {
        DATINFO item;
        item.offset = offset;
        item.type = 1;
        infolist.push_back(item);
       }
     else if(strcmp(data, "segs") == 0) {
        DATINFO item;
        item.offset = offset;
        item.type = 2;
        infolist.push_back(item);
       }

     // move offset
     offset += 0x800;
    }

 // reset input file
 ifile.clear(ios::goodbit);

 // important strings
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create save path
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("_");
 savepath += STDSTRING(GetExtensionFromFilename(filename).get() + 1);
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // save data
 cout << "Processing " << infolist.size() << " files inside archive." << endl;
 for(uint32 i = 0; i < infolist.size(); i++)
    {
     // calculate from-to
     uint32 a = infolist[i].offset;
     uint32 b = ((i == infolist.size() - 1) ? filesize : infolist[i + 1].offset);

     // compute filesize
     uint32 savesize = b - a;
     if(!savesize) continue;

     // seek data
     ifile.seekg(a);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[savesize]);
     ifile.read(data.get(), savesize);
     if(ifile.fail()) return error("Read failure.");

     // create output filename
     STDSTRINGSTREAM ss;
     ss << savepath;
     ss << setfill(TEXT('0')) << setw(4) << i;
     ss << TEXT(".");
     if(infolist[i].type == 0) ss << TEXT("part");
     else if(infolist[i].type == 1) ss << TEXT("qzip");
     else if(infolist[i].type == 2) ss << TEXT("segs");
     else return error("Unknown archive file type.");

     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save data
     ofile.write(data.get(), savesize);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
}

bool processSEG(LPCTSTR filename)
{
 // compressed chunk information
 struct SEGSINFO {
  uint16 deflate_size;
  uint16 inflate_size;
  uint32 offset;
 };

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // important strings
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create save path
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // initialize segment index
 uint32 segs_index = 0;
 for(;;)
    {
     cout << "at offset 0x" << hex << (uint32)ifile.tellg() << dec << endl;

     // read magic
     uint32 magic = BE_read_uint32(ifile);
     if(ifile.fail()) {
        if(ifile.eof()) return true;
        return error("Read failure.");
       }
     if(magic != 0x73656773) {
        if(segs_index == 0) return error("Expecting SEGS.");
        else return true; // stop
       }
     
     // read unknown
     uint16 unk01 = BE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.");
     
     // read chunks
     uint16 n_chunks = BE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.");
     
     // read inflated size
     uint32 size_inflated = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     
     // read deflated size
     uint32 size_deflated = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // for each chunk
     deque<ZLIBINFO2> itemlist;
     for(uint32 i = 0; i < n_chunks; i++)
        {
         // read segs information
         ZLIBINFO2 info;
         info.deflatedBytes = BE_read_uint16(ifile);
         info.inflatedBytes = BE_read_uint16(ifile);
         info.offset = BE_read_uint32(ifile);
         info.offset = info.offset - 1;
     
         // adjust inflate buffer size
         if(info.inflatedBytes == 0x0000) info.inflatedBytes = 0x10000;
     
         // insert item
         itemlist.push_back(info);
        }
     
     // compute starting offset
     uint32 start = (uint32)ifile.tellg();
     if(n_chunks % 2) start += 0x08;
     
     // adjust offsets
     for(uint32 i = 0; i < n_chunks; i++) {
         itemlist[i].offset += start;
         //cout << "0x" << hex << itemlist[i].deflatedBytes << dec << " - ";
         //cout << "0x" << hex << itemlist[i].inflatedBytes << dec << " - ";
         //cout << "0x" << hex << itemlist[i].offset << dec << endl;
        }
     
     // create output filename
     STDSTRINGSTREAM ss;
     ss << savepath;
     ss << setfill(TEXT('0')) << setw(3) << segs_index;
     ss << TEXT(".out");
     
     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");
     
     // deflate
     if(!InflateZLIB(ifile, itemlist, -15, ofile)) return false;

     // increase segment index
     segs_index++;
    }

 if(segs_index > 1) cout << "File has " << (segs_index + 1) << " segments." << endl;

 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

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
 STDSTRING pathname = GetModulePathname().get();
 return extract(pathname.c_str());
}

bool extract(LPCTSTR pname)
{
 // set pathname
 using namespace std;
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // PART
 // QZIP
 // segs

 // construct filenames to process
 deque<STDSTRING> filelist;
 filelist.push_back(pathname + STDSTRING(TEXT("BigFile_PS3.dat")));
 filelist.push_back(pathname + STDSTRING(TEXT("BigFile_PS3.d01")));
 filelist.push_back(pathname + STDSTRING(TEXT("BigFile_PS3.d02")));
 filelist.push_back(pathname + STDSTRING(TEXT("BigFile_PS3.d03")));
 filelist.push_back(pathname + STDSTRING(TEXT("BigFile_PS3.d04")));
 filelist.push_back(pathname + STDSTRING(TEXT("BigFile_PS3.d05")));
 filelist.push_back(pathname + STDSTRING(TEXT("BigFile_PS3.d06")));
 filelist.push_back(pathname + STDSTRING(TEXT("BigFile_PS3.d07")));
 filelist.push_back(pathname + STDSTRING(TEXT("BigFile_PS3.d08")));

 // variables
 bool doDAT = true;
 bool doSEG = true;

 // process archives
 cout << "STAGE 1" << endl;
 if(doDAT) {
    cout << "Processing .DAT files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processDAT(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archives
 cout << "STAGE 2" << endl;
 if(doSEG) {
    cout << "Processing .SEGS files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".SEGS"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processSEG(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 return true;
}

}};

