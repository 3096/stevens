#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "ps3_one_piece_uwr.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   UNLIMITED_WORLD_RED

//
// PHASE 1
// RENAMING
//

bool phase1(const STDSTRING& pathname)
{
 uint32 limit = 5662;
 for(uint32 i = 1; i <= limit; i++)
    {
     // open file
     STDSTRINGSTREAM ss;
     ss << pathname.c_str() << TEXT("FILE_0") << setfill(TEXT('0')) << setw(4) << i;
     wcout << "Renaming " << ss.str().c_str() << "..." << endl;
     ifstream ifile(ss.str().c_str(), ios::binary);
     if(!ifile) {
        message("Warning: Failed to open FILE_XXXXX file."); // for now, just display a warning message
        continue;
       }

     // determine filesize
     uint64 filesize = 0x00;
     ifile.seekg(0, ios::end);
     filesize = (uint64)ifile.tellg();
     ifile.seekg(0, ios::beg);

     // empty file
     if(filesize < 0x10) {
        ifile.close();
        DeleteFile(ss.str().c_str());
        continue;
       }

     // read magic
     uint32 magic = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // close file
     ifile.close();

     // process file
     switch(magic)
       {
        // crid
        case(0x43524944) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("crid"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // @utf
        case(0x40555446) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("utf"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // afs2
        case(0x41465332) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("afs"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // cpef
        case(0x63706566) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("cpef"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // dvlp
        case(0x64766C70) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("dvlp"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // dvrp
        case(0x64767270) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("dvrp"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }

        // farm
        case(0x6661726D) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("farm"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jadf
        case(0x6A414446) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jadf"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jarc
        case(0x6A415243) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jarc"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jcmp (compressed zlib???)
        case(0x6A434D50) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jcmp"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jefc
        case(0x6A454643) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jefc"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jfmt
        case(0x6A464D54) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jfmt"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jfnt
        case(0x4A464E54) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jfnt"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jimg
        case(0x6A494D47) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jimg"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jmdl
        case(0x6A4D444C) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jmdl"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jmmo
        case(0x6A4D4D4F) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jmmo"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jmot
        case(0x6A4D4F54) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jmot"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jmsg
        case(0x6A4D5347) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jmsg"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jpfx
        case(0x6A504658) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jpfx"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jscr
        case(0x6A534352) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jscr"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // jsnd
        case(0x6A534E44) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("jsnd"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // juim
        case(0x6A55494D) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("juim"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
        // stwl
        case(0x7374776C) : {
             auto filename = ChangeFileExtension(ss.str().c_str(), TEXT("stwl"));
             MoveFile(ss.str().c_str(), filename.get());
             break;
            }
       }     
    }

 return true;
}

//
// PHASE 2
// UNPACKING
//

bool phase2(const STDSTRING& pathname)
{
 return true;
}

//
// PHASE 3
// TEXTURES
//

bool phase3(const STDSTRING& pathname)
{
 return true;
}

//
// PHASE 4
// MODELS
//

bool processJMDL(binary_stream& fd)
{
 // read JMDL header
 uint32 h01 = fd.BE_read_uint32();
 uint32 h02 = fd.BE_read_uint32();
 uint32 h03 = fd.BE_read_uint32(); // 0x02200000
 if(h01 != 0x6A4D444C) return error("Expecting JMDL.");
 if(h02 < 0x20) return true; // nothing to do 

 // read header
 uint32 offset1 = fd.BE_read_uint32(); // offset ?
 uint32 offset2 = fd.BE_read_uint32(); // offset ?
 uint32 offset3 = fd.BE_read_uint32(); // offset ?
 uint32 offset4 = fd.BE_read_uint32(); // offset ?
 uint32 offset5 = fd.BE_read_uint32(); // offset to vertex buffer
 uint32 offset6 = fd.BE_read_uint32(); // offset ?

 // now assume there are 0x70 bytes of garbage data
 fd.seek(offset1);
 if(fd.fail()) return error("Seek failure.");

 // vertex buffer information
 struct VTXBUFFERLISTINFO {
  uint32 n_entries;         // number of entries
  uint32 offset_to_entries; // offset from start to entries
  uint32 unk01;             // 0x01
  uint32 section_size;      // size of header + entries + data
 };
 struct VTXBUFFERENTRYINFO {
  uint32 offset;               // offset from start to data
  uint32 size;                 // size of vertex buffer
  uint32 n_subentries;         // number of subentries
  uint32 offset_to_subentries; // offset from start to subentries
  uint32 unk01;                // ???
  uint32 unk02;                // ???
 };
 struct VTXBUFFERENTRY {
  uint32 unk01;
  uint32 flags;
 };

 // all vertex buffer information is stored here
 struct VTXBUFFERSECTION {
  VTXBUFFERLISTINFO vbli;
  std::deque<std::pair<VTXBUFFERENTRYINFO, std::deque<VTXBUFFERENTRY>>> vbel;
  std::deque<boost::shared_array<char>> data;
 };
 VTXBUFFERSECTION vbs;

 // move to vertex buffer data
 uint32 position = offset5;
 fd.seek(position);
 if(fd.fail()) return error("Seek failure.");

 // read vertex buffer list information
 vbs.vbli.n_entries = fd.BE_read_uint32();
 vbs.vbli.offset_to_entries = fd.BE_read_uint32();
 vbs.vbli.unk01 = fd.BE_read_uint32();
 vbs.vbli.section_size = fd.BE_read_uint32();
 if(fd.fail()) return fail("Stream read failure.");

 // validate vertex buffer list information
 if(vbs.vbli.n_entries == 0) return true;
 if(vbs.vbli.offset_to_entries != 0x10) return fail("Unexpected offset.");

 // read vertex buffer entries
 vbs.vbel.resize(vbs.vbli.n_entries);
 for(uint32 i = 0; i < vbs.vbli.n_entries; i++)
    {
     // move to entry
     uint32 offset = position + vbs.vbli.offset_to_entries;
     offset += i * sizeof(VTXBUFFERENTRYINFO);
     fd.seek(offset);
     if(fd.fail()) return fail("Stream seek failure.");

     // read entry
     vbs.vbel[i].first.offset = fd.BE_read_uint32();
     vbs.vbel[i].first.size = fd.BE_read_uint32();
     vbs.vbel[i].first.n_subentries = fd.BE_read_uint32();
     vbs.vbel[i].first.offset_to_subentries = fd.BE_read_uint32();
     vbs.vbel[i].first.unk01 = fd.BE_read_uint32();
     vbs.vbel[i].first.unk02 = fd.BE_read_uint32();
     if(fd.fail()) return fail("Stream read failure.");

     // read subentries
     for(uint32 j = 0; j < vbs.vbel[i].first.n_subentries; j++)
        {
         // move to subentry
         uint32 offset = position + vbs.vbel[i].first.offset_to_subentries;
         offset += j * sizeof(VTXBUFFERENTRY);
         fd.seek(offset);
         if(fd.fail()) return fail("Stream seek failure.");

         // read subentry
         VTXBUFFERENTRY subentry;
         subentry.unk01 = fd.BE_read_uint32();
         subentry.flags = fd.BE_read_uint32();
         vbs.vbel[i].second.push_back(subentry);
        }

     // seek vertex buffer data
     offset = position + vbs.vbel[i].first.offset;
     fd.seek(offset);
     if(fd.fail()) return fail("Stream seek failure.");

     // read vertex buffer data
     uint32 buffersize = vbs.vbel[i].first.size;
     if(!buffersize) return fail("Unexpected vertex buffer size.");
     boost::shared_array<char> data(new char[buffersize]);
     fd.read(data.get(), buffersize);
     if(fd.fail()) return fail("Stream read failure.");
    }

 // move to the index buffer
 position = position + vbs.vbli.section_size;
 fd.seek(position);
 if(fd.fail()) return fail("Stream seek failure.");

 // read index buffer header
 uint32 ib_start = position + fd.BE_read_uint32();
 uint32 ib_size = fd.BE_read_uint32();
 uint32 ib_indices = fd.BE_read_uint32();
 uint32 ib_unknown = fd.BE_read_uint32();
 uint32 ib_size_per_index = ib_size / ib_indices;
 if(fd.fail()) return fail("Stream read failure.");

 // move to index buffer data
 fd.seek(ib_start);
 if(fd.fail()) return fail("Stream seek failure.");

 // read index buffer
 boost::shared_array<char> ib_data(new char[ib_size]);
 if(ib_size_per_index == 1) fd.BE_read_array(reinterpret_cast<uint08*>(ib_data.get()), ib_indices);
 else if(ib_size_per_index == 2) fd.BE_read_array(reinterpret_cast<uint16*>(ib_data.get()), ib_indices);
 else if(ib_size_per_index == 4) fd.BE_read_array(reinterpret_cast<uint32*>(ib_data.get()), ib_indices);
 else return fail("Unexpected index buffer data type.");

 // model container
 ADVANCEDMODELCONTAINER amc;

 // initialize index buffer
 AMC_IDXBUFFER ib;
 ib.format = AMC_UINT16;
 if(ib_size_per_index == 1) ib.format = AMC_UINT08;
 else if(ib_size_per_index == 4) ib.format = AMC_UINT32;
 ib.type = AMC_TRIANGLES;
 ib.wind = AMC_CW;
 ib.reserved = 0;
 ib.name = "default";
 ib.elem = ib_indices;
 ib.data = ib_data;

 // insert index buffer
 amc.iblist.push_back(ib);

 // TODO:
 // MOVE TO TEXTURE DATA AT END OF JMDL SECTION
 return true;
}

bool processJMDL(const STDSTRING& filename)
{
 // open file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read JMDL header
 uint32 h01 = BE_read_uint32(ifile);
 uint32 h02 = BE_read_uint32(ifile);
 if(h01 != 0x6A4D444C) return error("Expecting JMDL.");
 if(h02 < 0x08) return true; // nothing to do

 // move back to start
 ifile.seekg(0);
 if(ifile.fail()) return fail("Seek failure.");

 // read JMDL data into a stream
 uint32 datasize = h02;
 boost::shared_array<char> fd(new char[datasize]);
 ifile.read(fd.get(), datasize);
 if(ifile.fail()) return error("Read failure.");
 binary_stream fs(fd, datasize);
 if(!processJMDL(fs)) return false;

 return true;
}

bool phase4(const STDSTRING& pathname)
{
 cout << "Processing .JMDL files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".JMDL"), pathname.c_str());
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!processJMDL(filelist[i].c_str())) return false;
    }
 cout << endl;
 return true;
}


//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PS3] One Piece: Unlimited World Red");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where LINKDATA files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~12 GB free space.\n");
 p3 += TEXT("3. 4 GB for game + 8 GB extraction.\n");
 p3 += TEXT("4. For DLC, EDATs must be decrypted to DATs.");

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
 bool doPHASE1 = false;
 bool doPHASE4 = true;

 // questions
 // if(doPHASE1) doPHASE1 = YesNoBox("Process FILE_XXXXX files?\nSay 'No' if you have already done this before.");
 // if(doPHASE4) doPHASE4 = YesNoBox("Process JMDL files?\nSay 'No' if you have already done this before.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process data
 if(doPHASE1) phase1(pathname);
 if(doPHASE4) phase4(pathname);

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