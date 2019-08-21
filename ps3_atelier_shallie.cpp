#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_dw_arc.h"
#include "x_dw_ps3.h" // Dynasty Warriors Archives and Textures
#include "x_dw_g1m.h" // Dynasty Warriors Models
#include "ps3_atelier_eal.h"
#include "ps3_atelier_shallie.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   ATELIER_SHALLIE

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processZIP(LPCTSTR filename)
{
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

 // create output filename
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname;
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // loop forever
 for(;;)
    {
     // read size of data
     uint32 size = BE_read_uint32(ifile);
     if(ifile.eof() || ifile.fail()) break;

     // validate size
     if(!size) continue; // nothing to decompress
     if(size > filesize) return message("WARNING: NOT ZLIB.");

     // save position
     uint32 position = (uint32)ifile.tellg();
     if(ifile.fail()) return error("Tell failure.");

     // test for zlib
     auto b1 = BE_read_uint08(ifile);
     auto b2 = BE_read_uint08(ifile);
     if(!isZLIB(b1, b2)) return message("WARNING: NOT ZLIB.");
     if(ifile.eof() || ifile.fail()) break; 

     // seek position
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[size]);
     ifile.read(data.get(), size);
     if(ifile.fail()) return error("Read failure.");

     // decompress
     bool success = InflateZLIB(data.get(), size, ofile);
     if(!success) return error("ZLIB failure.");
    }

 return true;
}

bool processELX(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // nothing to do
 if(filesize == 0) return true;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create save path
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x45415243) return error("Invalid EARC file.");
 
 // 0x00000000 or 0x01000000
 uint32 unk01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read data size
 uint32 datasize = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x0000001C
 uint32 unk02 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset from 0x1C to the actual data
 uint32 dataoffset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // 0x00000001
 uint32 unk03 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read filenames
 struct EARCFILELIST {
  uint32 p01;
  uint32 p02;
  char p03[0x31];
 };
 deque<EARCFILELIST> filelist;
 for(uint32 i = 0; i < n_files; i++)
    {
     // offset to file
     EARCFILELIST item;
     item.p01 = LE_read_uint32(ifile); // LE!!!
     if(ifile.fail()) return error("Read failure.");

     // read size of file in archive
     item.p02 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read name
     ZeroMemory(&item.p03[0], 0x31);
     if(unk01 == 0x00000000) ifile.read(&item.p03[0], 0x20);
     else if(unk01 == 0x01000000) ifile.read(&item.p03[0], 0x30);
     else return error("Unknown filename size.");
     if(ifile.fail()) return error("Read failure.");
 
     // insert item
     filelist.push_back(item);
    }

 // for each file in archive
 uint32 offset = dataoffset + 0x1C;
 for(uint32 i = 0; i < n_files; i++)
    {
     // move to offset
     ifile.seekg(offset);
     if(ifile.fail()) return error("Seek failure.");

     // create output filename
     STDSTRINGSTREAM ss;
     ss << savepath << filelist[i].p03;
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) {
        wstringstream es;
        es << "Failed to create output file " << ss.str().c_str() << ".";
        return error(es);
       }

     // read data
     boost::shared_array<char> data(new char[filelist[i].p02]);
     ifile.read(data.get(), filelist[i].p02);
     if(ifile.fail()) return error("Read failure.");

     // save data
     ofile.write(data.get(), filelist[i].p02);
     if(ofile.fail()) return error("Write failure.");

     // adjust offset by filesize
     offset += filelist[i].p02;
    }

 return true;
}

bool processG1H(LPCTSTR filename)
{
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

 // create save path
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // read magic
 uint32 h01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h01 != 0x4731485F) return error("Invalid G1H_ file.");

 // read version
 uint32 h02 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read filesize
 uint32 h03 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to offset table
 uint16 h04 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of files
 uint16 h05 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to offset table
 ifile.seekg(h04);
 if(ifile.fail()) return error("Seek failure.");

 // read offset table
 deque<uint32> offsets;
 for(uint32 i = 0; i < h05; i++) {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     offsets.push_back(item);
    }

 // for each offset
 for(uint32 i = 0; i < h05; i++)
    {
     // seek offset
     uint32 position = offsets[i];
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     uint32 endpos = filesize;
     if(i < (h05 - 1u)) endpos = offsets[i + 1];
     uint32 size = endpos - position;
     if(!size) return error("Nothing to save.");
     boost::shared_array<char> data(new char[size]);
     ifile.read(data.get(), size);
     if(ifile.fail()) return error("Read failure.");

     // create filename
     STDSTRINGSTREAM ss;
     ss << savepath;
     ss << setfill(TCHAR('0')) << setw(3) << i;
     ss << TEXT(".g1p");

     // create file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");
     ofile.write(data.get(), size);
     if(ofile.fail()) return error("Write failure.");
    }

 return true;
}

bool processG1P(LPCTSTR filename)
{
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

 // create save path
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // read magic
 uint32 h01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(h01 != 0x47314850) return error("Invalid G1HP file.");

 // read version
 uint32 h02 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read filesize
 uint32 h03 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset to offset table
 uint16 h04 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read 0x03
 uint16 h05 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of files
 uint16 h06 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to offset table
 ifile.seekg(h04);
 if(ifile.fail()) return error("Seek failure.");

 // read offset table
 deque<uint32> offsets;
 for(uint32 i = 0; i < h06; i++) {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     offsets.push_back(item);
    }

 // for each offset
 for(uint32 i = 0; i < h06; i++)
    {
     // seek offset
     uint32 position = offsets[i];
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     uint32 endpos = filesize;
     if(i < (h06 - 1u)) endpos = offsets[i + 1];
     uint32 size = endpos - position;
     if(!size) return error("Nothing to save.");
     boost::shared_array<char> data(new char[size]);
     ifile.read(data.get(), size);
     if(ifile.fail()) return error("Read failure.");

     // create filename
     STDSTRINGSTREAM ss;
     ss << savepath;
     ss << setfill(TCHAR('0')) << setw(3) << i;
     ss << TEXT(".g1m");

     // create G1M file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");
     ofile.write(data.get(), size);
     if(ofile.fail()) return error("Write failure.");
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
 p1 += TEXT("[PS3] Atelier Shallie: Alchemists of the Dusk Sea (BLES02143)");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where game is in (i.e. USRDIR).\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a while.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~ GB free space.\n");
 p3 += TEXT("3. 8 GB for game +  GB extraction.");

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
 return ATELIER_EAL::extract(pname);

 // // set pathname
 // STDSTRING pathname(pname);
 // if(!pathname.length()) pathname = GetModulePathname().get();
 // 
 // //  // booleans
 // bool doZIP = true;
 // bool doELX = true;
 // bool doG1T = true;
 // bool doG1H = false; // G1H and G1HP files contain vertex control points only and are useless
 // bool doG1P = false; // G1H and G1HP files contain vertex control points only and are useless
 // bool doG1M = true;
 // 
 // // questions
 // if(doZIP) doZIP = YesNoBox("Process ELIXIR.GZ files?\nClick 'No' if you have already done so.");
 // if(doELX) doELX = YesNoBox("Process ELIXIR files?\nClick 'No' if you have already done so.");
 // if(doG1T) doG1T = YesNoBox("Process GIT files?\nClick 'No' if you have already done so.");
 // if(doG1H) doG1H = YesNoBox("Process G1H files?\nClick 'No' if you have already done so.");
 // if(doG1P) doG1P = YesNoBox("Process G1P files?\nClick 'No' if you have already done so.");
 // if(doG1M) doG1M = YesNoBox("Process G1M files?\nClick 'No' if you have already done so.");
 // 
 // // process archive
 // cout << "STAGE 1" << endl;
 // if(doZIP) {
 //    cout << "Processing .GZ files..." << endl;
 //    deque<STDSTRING> filelist;
 //    BuildFilenameList(filelist, TEXT(".GZ"), pathname.c_str());
 //    for(size_t i = 0; i < filelist.size(); i++) {
 //        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
 //        if(!processZIP(filelist[i].c_str())) return false;
 //       }
 //    cout << endl;
 //   }
 // 
 // cout << "STAGE 2" << endl;
 // if(doELX) {
 //    cout << "Processing .ELIXIR files..." << endl;
 //    deque<STDSTRING> filelist;
 //    BuildFilenameList(filelist, TEXT(".ELIXIR"), pathname.c_str());
 //    for(size_t i = 0; i < filelist.size(); i++) {
 //        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
 //        if(!processELX(filelist[i].c_str())) return false;
 //       }
 //    cout << endl;
 //   }
 // 
 // cout << "STAGE 3" << endl;
 // if(doG1T) PS3::DYNASTY_WARRIORS::UnpackTextures(pathname.c_str());
 // 
 // cout << "STAGE 4" << endl;
 // if(doG1H) {
 //    cout << "Processing .G1H files..." << endl;
 //    deque<STDSTRING> filelist;
 //    BuildFilenameList(filelist, TEXT(".G1H"), pathname.c_str());
 //    for(size_t i = 0; i < filelist.size(); i++) {
 //        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
 //        if(!processG1H(filelist[i].c_str())) return false;
 //       }
 //    cout << endl;
 //   }
 // 
 // cout << "STAGE 5" << endl;
 // if(doG1P) {
 //    cout << "Processing .G1HP files..." << endl;
 //    deque<STDSTRING> filelist;
 //    BuildFilenameList(filelist, TEXT(".G1P"), pathname.c_str());
 //    for(size_t i = 0; i < filelist.size(); i++) {
 //        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
 //        if(!processG1P(filelist[i].c_str())) return false;
 //       }
 //    cout << endl;
 //   }
 // 
 // cout << "STAGE 6" << endl;
 // if(doG1M) PS3::DYNASTY_WARRIORS::UnpackModels(pathname.c_str());

 return true;
}

}};

