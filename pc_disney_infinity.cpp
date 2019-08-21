#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "pc_disney_infinity.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   DISNEY_INFINITY

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

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

bool processOCT(const string& filename)
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
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // read header
 auto h01 = LE_read_uint32(ifile); // 0x45017629
 auto h02 = LE_read_real32(ifile); // ????
 auto h03 = LE_read_uint32(ifile); // 0x00
 auto h04 = LE_read_uint32(ifile); // size of string table
 auto h05 = LE_read_uint32(ifile); // size of data section
 if(h01 != 0x45017629) return error("Not an OCT file."); // not an OCT file
 if(h04 == 0x00) return true; // nothing to do

 // move to 0x3C
 ifile.seekg(0x3C);
 if(ifile.fail()) return error("Seek failure.");

 // read stringtable
 boost::shared_array<char> stringtable(new char[h04]);
 ifile.read(stringtable.get(), h04);
 if(ifile.fail()) return error("Read failure.");

 // extract strings from stringtable
 deque<string> strlist;
 binary_stream bs(stringtable, h04);
 for(;;) {
     string item = bs.read_string();
     if(bs.fail()) break;
     strlist.push_back(item);
     cout << " " << item << endl;
    }
 cout << " Number of strings = 0x" << hex << strlist.size() << dec << endl;
return true;

 // read data
 boost::shared_array<char> data(new char[h05]);
 ifile.read(data.get(), h05);
 if(ifile.fail()) return error("Read failure.");

 // extract information from data
 binary_stream ds(data, h05);
 for(;;)
    {
     // read stringtable index
     auto p01 = ds.LE_read_uint16();
     if(ds.fail()) break;

     // validate stringtable index
     if(!(p01 < strlist.size()))
        return error("Stringtable index out of bounds");

     //
     cout << strlist[p01] << endl;

     //
     if(strlist[p01] == "ExporterDate")
       {
        // read index into stringtable
        auto item1_index = ds.LE_read_uint16();
        if(ds.fail()) break;

        // 
        auto item1_data = strlist[item1_index];
       }
     else if(strlist[p01] == "Translation")
       {
        auto e1 = ds.LE_read_uint08(); // 3 or 4
        auto e2 = ds.LE_read_real32(); // x
        auto e3 = ds.LE_read_real32(); // y
        auto e4 = ds.LE_read_real32(); // z
        auto e5 = (e1 == 4 ? ds.LE_read_real32() : 0.0f);
        auto e6 = ds.LE_read_uint16(); // 0x0C01
       }
     else {
        stringstream ss;
        ss << "Unknown property '" << strlist[p01] << ".'";
        return error(ss);
       }

     // 01 00 00 00 01 04
     // 01 00 05 08 02 00
     // 03 00 - 1B 0C
     // 04 00 - 3F 0A 0C
     // 05 00 3F - 
     //       06 00 07 00 08 00 09 00
     //       0A 00 0B 00 0C 00 0D 00
     //       0E 00 0F 00 10 00 11 00
     //       12 00 13 00 14 00 15 00
     //       16 00 17 00 18 00 19 00 
     //       1A 00 1B 00 1C 00 1D 00 
     //       1E 00 1F 00 20 00 21 00 
     //       22 00 23 00 24 00 25 00
     //       26 00 27 00 28 00 29 00
     //       2A 00 2B 00 2C 00 2D 00 
     //       2E 00 2F 00 30 00 31 00 
     //       32 00 33 00 34 00 35 00 
     //       36 00 37 00 38 00 39 00 
     //       3A 00 3B 00 3C 00 3D 00 
     //       3E 00 3F 00 40 00 41 00 
     //       42 00 43 00 44 00 01 04

     // 45 00 - 05 08 SubNetworkPool
     // 46 00 - 03 00 0B 0C SubNetwork
     // 47 00 - Name
     // 48 00 - 0B 0C RB_lowerBody_Mesh
     // 49 00 - Type
     // 4A 00 - 01 0C Scene
     // 4B 00 - 05 10 BasisPool
     // 4C 00 - 03 00 0B 14 49 00 Basis
     // 4D 00 - 0B 14 - Geometry3d
     // 4E 00 - Behavior
     // 4F 00 - 05 10 - 4C 00 - Root
     // 50 00 - 0B 14 - 47 00 - 48 00 - 1B 14
     // 51 00 - 00 0B 14 4E 00
     // 52 00 - 05 10 4C 00
     // 53 00 - 0B 14 49 00
     // 54 00 - 0B 14 4E 00 4F 00 0A 0C
     // 55 00 - 02 48 00
     // 56 00 - 1A 0C
     // 57 00 - 02 00 01 1A 0C
     // 58 00 - 04 00 01 01 01 01 0C
     // 59 00 - 05 10
     // 5A 00 - 03 00 1B 14
     // 5B 00 - 02 0B 14 49 00
     // 5C 00 - 1B 14
     // 5D 00 - 01 1B 14
     // 5E 00 - 00 01 14
     // 5F 00 - 12 18
     // 60 00 - 03 - 00 00 00 00 - E1 4D 68 3F - 00 00 00 00 - 01 0C - Translation
     // 61 00 - 01 0C - ProcessorNodePool
     // 62 00 - 01 10 - BasisConversionPool
     // 63 00 - 1B 14 - BasisConversion
     // 64 00 - 01 1B 14 - FromBasisRef
     // 65 00 - 00 1B 14 - ToBasisRef
     // 66 00 - 00 01 0C - DataNodeRef
     // 67 00 - 01 0C - ComponentFamilyPool
     // 68 00 - 05 08 46 00 50 00 0B 0C
     // 47 00 - 69 00 0B 0C
     // 49 00 - 4A 00 01 0C
     // 4B 00 - 05 10
     // 4C 00 - 03 00 0B 14
     // 49 00 - 4D 00 0B 14
     // 4E 00 - 4F 00 05 10
     // 4C 00 - 50 00 0B 14
     // 47 00 - 69 00 1A 0C
     // 57 00 - 02 01 00 1A 0C
     // 58 00 - 04 00 01 01 01 01 0C
     // 59 00 - 05 10
     // 5A 00 - 03 00 1B 14
     // 5B 00 - 02 0B 14
     // 49 00 - 5C 00 1B 14
     // 5D 00 - 01 1B 14
     // 5E 00 - 00 01 14
     // 5F 00 - 12 18
     // 60 00 - 03 CA 56 C6 3D 52 B9 92 3F 00 00 00 00 01 0C - Translation
     // now at data offset 0x1FB

     // first vertex buffer has 0x036B vertices
     // so look for 6B03!

     // C3 00 - Vdata
     //      08 02 00
     //      6B 03 - number of vertices
     //      00 00 00 00
     //      10 00 00 00
     //      B0 36 - size of data
     //      10 00 - vertex stride
     //      12 0C
     // C4 00 - LocalToParentMatrix
     //         10
     //         00 00 80 3F 00 00 00 00 00 00 00 00 00 00 00 00
     //         00 00 00 00 00 00 80 3F 00 00 00 00 00 00 00 00
     //         00 00 00 00 00 00 00 00 00 00 80 3F 00 00 00 00
     //         00 00 00 00 E1 4D 68 3F 00 00 00 00 00 00 80 3F
    }

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

 // variables
 bool doOCT = true;

 // process archive
 cout << "STAGE 1" << endl;
 if(doOCT) {
    cout << "Processing .OCT files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".OCT", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processOCT(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

}};

