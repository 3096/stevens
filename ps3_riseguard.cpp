#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "ps3_riseguard.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   ROG

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

struct HNKHEADER {
 uint32 p01; // offset to first piece of data
 uint16 p02; // 0x0070
 uint16 p03; // 0x0004
 uint16 p04; // 0x0008
 uint16 p05; // 0x0001
 uint16 p06; // 0x0001
 uint16 p07; // 0x0002, 0x0009 (variable)
 uint32 p08; // unknown
 uint32 p09; // number of 0x48 byte sections after header
};

bool processHNK(const std::string& filename)
{
 // filename to analyze
 string shrtname = GetShortFilenameWithoutExtension(filename);
 string pathname = GetPathnameFromFilename(filename);

 // create save directory
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // open file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read header
 HNKHEADER header;
 header.p01 = LE_read_uint32(ifile); // offset to first piece of data
 header.p02 = LE_read_uint16(ifile); // 0x70
 header.p03 = LE_read_uint16(ifile); // 0x04
 header.p04 = LE_read_uint16(ifile); // 0x08
 header.p05 = LE_read_uint16(ifile); // 0x01
 header.p06 = LE_read_uint16(ifile); // 0x01
 header.p07 = LE_read_uint16(ifile); // 0x02
 header.p08 = LE_read_uint32(ifile); // ????
 header.p09 = LE_read_uint32(ifile); // number of 0x48 byte sections after header

 // read post-header data
 for(uint32 i = 0; i < header.p09; i++)
    {
     char name[0x40];
     ifile.read(&name[0], 0x40);
     if(ifile.fail()) return error("Read failure.");
     cout << "name = " << name << endl;

     auto p01 = LE_read_uint32(ifile);
     auto p02 = LE_read_uint32(ifile);
     cout << "p01 = 0x" << hex << p01 << dec << endl;
     cout << "p02 = 0x" << hex << p02 << dec << endl;
     cout << endl;
    }

 // move to first data item
 ifile.seekg(header.p01);
 if(ifile.fail()) return error("Seek failure.");

 struct HNKITEM {
  uint32 p01; // 0x00000000 (always)
  uint16 p02; // unknown
  uint16 p03; // unknown
  uint32 p04; // unknown
  uint16 p05; // 0x0071
  uint16 p06; // 0x0004
  uint16 p07; // 0x0001
  uint16 p08; // 0x000D
  uint16 p09; // 0x0002
  uint16 p10; // 0x000B (number of textures)
  uint16 p11; // 0x0007 (length of item type)
  char p12[255]; // type name
  char p13[255]; // item name
 };

 struct HNKTEXTUREINFO {
  uint32 p01; // 0x5C
  uint32 p02; // 0x041150
  uint32 p03; // type
  uint32 p04; // texture data size in BE
  uint32 p05; // 0x00000000
  uint16 p06; // dx in BE
  uint16 p07; // dy in BE
  uint32 p08; // 0x3B
  uint32 p09; // 0x3B
  uint16 p10; // 0x0000
  uint16 p11; // 0x0008
  uint32 p12; // 0x00000001
  uint32 p13; // 0x00000000
  uint32 p14; // 0x00000000
  char p15[0x20];
  uint32 p16; // 0x00000001
  uint32 p17; // 0x00000001
  uint32 p18; // 0x00000002
  uint32 p19; // 0x00000002
  uint32 p20; // 0x00000002
  uint32 p21; // texture data size in LE
  uint32 p22; // 0x00080151
 };

 // loop items
 uint32 n_items = 0;
 for(;;)
    {
     HNKITEM item;
     item.p01 = LE_read_uint32(ifile);
     item.p02 = LE_read_uint16(ifile);
     item.p03 = LE_read_uint16(ifile);
     item.p04 = LE_read_uint32(ifile);
     item.p05 = LE_read_uint16(ifile);
     item.p06 = LE_read_uint16(ifile);
     item.p07 = LE_read_uint16(ifile);
     item.p08 = LE_read_uint16(ifile);
     item.p09 = LE_read_uint16(ifile);
     item.p10 = LE_read_uint16(ifile);
     item.p11 = LE_read_uint16(ifile);

     // read type name
     item.p12[0] = '\0';
     if(item.p10) read_string(ifile, &item.p12[0], item.p10);

     // read item name
     item.p13[0] = '\0';
     if(item.p11) read_string(ifile, &item.p13[0], item.p11);

     // TSETexture
     if(strcmp(item.p12, "TSETexture") == 0)
       {
        // read texture information
        HNKTEXTUREINFO info;
        info.p01 = LE_read_uint32(ifile);
        info.p02 = LE_read_uint32(ifile);
        info.p03 = LE_read_uint32(ifile);
        info.p04 = BE_read_uint32(ifile);
        info.p05 = LE_read_uint32(ifile);
        info.p06 = BE_read_uint16(ifile);
        info.p07 = BE_read_uint16(ifile);
        info.p08 = LE_read_uint32(ifile);
        info.p09 = LE_read_uint32(ifile);
        info.p10 = LE_read_uint16(ifile);
        info.p11 = LE_read_uint16(ifile);
        info.p12 = LE_read_uint32(ifile);
        info.p13 = LE_read_uint32(ifile);
        info.p14 = LE_read_uint32(ifile);
        info.p15[0] = '\0';
        ifile.read(&info.p15[0], 0x20);
        if(ifile.fail()) return error("Read failure.");
        info.p16 = LE_read_uint32(ifile);
        info.p17 = LE_read_uint32(ifile);
        info.p18 = LE_read_uint32(ifile);
        info.p19 = LE_read_uint32(ifile);
        info.p20 = LE_read_uint32(ifile);
        info.p21 = LE_read_uint32(ifile); // same as info.p04 but as LE
        info.p22 = LE_read_uint32(ifile);

        // read texture data
        boost::shared_array<char> data(new char[info.p21]);
        ifile.read(data.get(), info.p21);
        if(ifile.fail()) return error("Read failure.");

        // create output file
        stringstream ss;
        ss << savepath << info.p15 << ".dds";
        ofstream ofile(ss.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create file.");

        // DXT1 header
        DDS_HEADER ddsh;
        if(info.p03 == 0xF93DBCA1) {
           if(!CreateDXT1Header(info.p06, info.p07, ComputeMipMapNumber(info.p06, info.p07), FALSE, &ddsh))
              return error("Failed to create DDS header.");
          }
        // DXT5 header
        else if(info.p03 == 0xD33ABC9F) {
           if(!CreateDXT5Header(info.p06, info.p07, ComputeMipMapNumber(info.p06, info.p07), FALSE, &ddsh))
              return error("Failed to create DDS header.");
          }
        // DXT1 no mipmap header (I think... looks OK but there is only one file like it so hard to tell if it is correct or not)
        else if(info.p03 == 0x6F7478E9) {
           if(!CreateDXT1Header(info.p06, info.p07, 0, FALSE, &ddsh))
              return error("Failed to create DDS header.");
          }
        else {
           stringstream ss;
           ss << "Unknown error format 0x" << hex << info.p03 << dec << ".";
           return error(ss);
          }

        // save texture
        ofile.write("DDS ", 4);
        ofile.write((char*)&ddsh, sizeof(ddsh));
        if(ofile.fail()) return error("Write failure.");
        ofile.write(data.get(), info.p21);
        if(ofile.fail()) return error("Write failure.");
       }
     // RenderModelTemplate
     else if(strcmp(item.p12, "RenderModelTemplate") == 0)
       {
        // Jack overall size is 0x47DFE
        struct HNKRMTINFO {
         uint32 p01; // 0x00002F10 (0x00) 0x000007D0
         uint32 p02; // 0x001010B0 (0x04) 0x001010B0
         uint32 p03; // 0x00000000 (0x08) 0x00000000
         uint16 p04; // 0x0006     (0x0C) 0x0001 (BE)
         uint16 p05; // 0x0046     (0x0E) 0x000A (BE)
         uint16 p06; // 0x0006     (0x10) 0x0001 (BE)
         uint16 p07; // 0x0006     (0x12) 0x0001 (BE)
         uint16 p08; // 0x0046     (0x14) 0x0009 (BE)
         uint16 p09; // 0x0005     (0x16) 0x0000 (BE)
         uint16 p10; // 0x002A     (0x18) 0x0000 (BE)
         uint16 p11; // 0x0001     (0x1A) 0x0001 (BE)
         uint16 p12; // 0x0000     (0x1C) 0x0000 (BE)
         uint16 p13; // 0x0002     (0x1E) 0x0001 (BE)
         uint16 p14; // 0x0002     (0x20) 0x0001 (BE)
         uint16 p15; // 0x0003     (0x22) 0x0001 (BE)
         uint16 p16; // 0x0006     (0x24) 0x0001 (BE)
         uint16 p17; // 0x0000     (0x26) 0x0000 (BE)
         uint32 p18; // 0x000002C8 (0x28) 0x000000E8
         uint32 p19; // 0x00000090 (0x2C) 0x00000090
         uint32 p20; // 0x00000548 (0x30) 0x00000144
         uint32 p21; // 0x000002F0 (0x34) 0x000000FC
         uint32 p22; // 0x00000520 (0x38) 0x00000000
         uint32 p23; // 0x000015E6 (0x3C) 0x000003CC
         uint32 p24; // 0x00001680 (0x40) 0x000003E0
         uint32 p25; // 0x00000000 (0x44) 0x00000000
         uint32 p26; // 0x00000000 (0x48) 0x00000000
         uint32 p27; // 0x00002D44 (0x4C) 0x00000724
         uint32 p28; // 0x00002800 (0x50) 0x00000000
         uint32 p29; // 0x000028A8 (0x54) 0x00000660
        };

        break;
       }
     //
     else
        break;
    }

 cout << "n_items = " << n_items << endl;

/*
        // read filename
        char texture_name1[1024];
        if(!read_string(ifile, &texture_name1[0], 1024)) return error("Read failure.");
        //cout << "texture_name1 = " << texture_name1 << endl;

        // 00 00 00 00 00 00 00 00 2F 00 00 00 71 00 04 00 01 00 0D 00 02 00 0B 00 1A 00
        // 00 00 00 00 72 00 04 00 2F 00 00 00 71 00 04 00 01 00 0D 00 02 00 0B 00 1A 00

        // 00 00 00 00 00 00 00 00 1C 00 00 00 71 00 04 00 01 00 0D 00 02 00 0B 00 07 00 texture
        // 00 00 00 00 72 00 04 00 29 00 00 00 71 00 04 00 03 00 05 00 07 00 14 00 0B 00 model

        // 00 00 00 00 72 00 04 00 // end of file
       }
     else if(strcmp(section_type, "RenderModelTemplate") == 0)
       {
        // read name
        char name[1024];
        if(!read_string(ifile, &name[0], 1024)) return error("Read failure.");
        //cout << "name = " << name << endl;

        // model header
        auto m01 = LE_read_uint32(ifile); // ???
        auto m02 = LE_read_uint32(ifile); // 0x001010B0
        auto m03 = LE_read_uint32(ifile); // 0x00000000

        auto m04 = LE_read_uint16(ifile); // 0006
        auto m05 = LE_read_uint16(ifile); // 0046
        auto m06 = LE_read_uint16(ifile); // 0006
        auto m07 = LE_read_uint16(ifile); // 0006
        auto m08 = LE_read_uint16(ifile); // 0046
        auto m09 = LE_read_uint16(ifile); // 0005
        auto m10 = LE_read_uint16(ifile); // 002A
        auto m11 = LE_read_uint16(ifile); // 0001
        auto m12 = LE_read_uint16(ifile); // 0000
        auto m13 = LE_read_uint16(ifile); // 0002
        auto m14 = LE_read_uint16(ifile); // 0002
        auto m15 = LE_read_uint16(ifile); // 0003
        auto m16 = LE_read_uint16(ifile); // 0006
        auto m17 = LE_read_uint16(ifile); // 0000
        auto m18 = LE_read_uint32(ifile); // C8020000
        auto m19 = LE_read_uint32(ifile); // 90000000
        auto m20 = LE_read_uint32(ifile); // 48050000
        auto m21 = LE_read_uint32(ifile); // F0020000
        auto m22 = LE_read_uint32(ifile); // 20050000
        auto m23 = LE_read_uint32(ifile); // E6150000
        auto m24 = LE_read_uint32(ifile); // 80160000
        auto m25 = LE_read_uint32(ifile); // 00000000
        auto m26 = LE_read_uint32(ifile); // 00000000
        auto m27 = LE_read_uint32(ifile); // 442D0000
        auto m28 = LE_read_uint32(ifile); // 00280000
        auto m29 = LE_read_uint32(ifile); // A8280000

        //
        // 0x28A8 ... from 0x16A8A3E
        // ...
        // 0x00010 (unknown)

        // 0x0A5DC (vertex buffer, 0x20 byte vertices)
        // 0x00008 (index buffer header)
        // 0x03A1A (index buffer data)

        // 0x00008 (vertex buffer header) 
        // 0x289CC (vertex buffer, 0x1C byte vertices)
        // 0x00008 (index buffer header)
        // 0x0E2EC (index buffer data)
       }
     else {
        cout << "Not a texture." << endl;
        cout << "section_type = " << section_type << endl;
        return false;
       }
    }
*/
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
 std::string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pname)
{
 // set pathname
 using namespace std;
 std::string pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname();

 // process archive
 cout << "Processing .HNK files..." << endl;
 deque<string> filelist;
 BuildFilenameList(filelist, ".HNK", pathname.c_str());
 for(size_t i = 83; i < filelist.size(); i++) {
     cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!processHNK(filelist[i])) return false;
    }
 cout << endl;

 return true;
}

}};

