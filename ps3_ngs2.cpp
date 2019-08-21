#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_alg.h"
#include "x_dds.h"
#include "x_amc.h"
#include "ps3_ngs2.h"

#define X_SYSTEM PS3
#define X_GAME   NGS2

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

}};

//
// TEXTURES
//

bool processTTG(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 return true;
}

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

class mdlextractor {
 private :
  std::string filename;
  std::string pathname;
  std::string shrtname;
  bool debug;
 private :
  std::ifstream ifile;
  std::ofstream dfile;
  std::ofstream ofile;
 private :
  bool processMdlGeo(boost::shared_array<char> data, uint32 size);
  bool processTTG(boost::shared_array<char> data, uint32 size);
  bool processVtxLay(boost::shared_array<char> data, uint32 size);
  bool processIdxLay(boost::shared_array<char> data, uint32 size);
  bool processMtrCol(boost::shared_array<char> data, uint32 size);
  bool processMdlInfo(boost::shared_array<char> data, uint32 size);
  bool processEdGemLy(boost::shared_array<char> data, uint32 size);
  bool processHieLay(boost::shared_array<char> data, uint32 size);
  bool processLHeader(boost::shared_array<char> data, uint32 size);
  bool processMTRLCHNG(boost::shared_array<char> data, uint32 size);
  bool processACSPACK2(boost::shared_array<char> data, uint32 size);
 public :
  bool extract(void);
 public :
  mdlextractor(const std::string& filename, bool debug);
 ~mdlextractor();
};

mdlextractor::mdlextractor(const std::string& filename, bool debug)
{
 // initialize data
 this->filename = filename;
 this->pathname = GetPathnameFromFilename(filename);
 this->shrtname = GetShortFilenameWithoutExtension(filename);
 this->debug = debug;

 // create debug file
 std::string dfname = pathname;
 dfname += shrtname;
 dfname += ".txt";
 if(debug) dfile.open(dfname.c_str());
}

mdlextractor::~mdlextractor()
{
}

bool mdlextractor::processMdlGeo(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::processTTG(boost::shared_array<char> data, uint32 size)
{
 // create binary stream
 using namespace std;
 binary_stream bs(data, size);
 bs.seek(0);

 // read header
 uint32 unk01 = bs.BE_read_uint32();
 uint32 unk02 = bs.BE_read_uint32();

 // read offset to data
 uint32 dataoffset = bs.BE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");
 dataoffset -= 0x08;

 // read number of textures
 uint32 n_textures1 = bs.BE_read_uint32();
 uint32 n_textures2 = bs.BE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");
 if(n_textures1 != n_textures2) return error("Texture counts not the same.");

 // read 0x00
 uint32 unk03 = bs.BE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");

 // read offset to offset table
 uint32 tableoffset = bs.BE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");
 tableoffset -= 0x08;

 // read offset to the sizes table
 uint32 sizesoffset = bs.BE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");
 sizesoffset -= 0x08;

 // read offset to the filenames table
 uint32 namesoffset = bs.BE_read_uint32();
 if(bs.fail()) return error("Stream read failure.");
 namesoffset -= 0x08;

 // read file properties
 bs.seek(namesoffset);
 for(uint32 i = 0; i < n_textures1; i++)
    {
     // read properties
     uint32 p01 = bs.BE_read_uint32();
     uint32 p02 = bs.BE_read_uint32();
     uint32 p03 = bs.BE_read_uint16();
     uint32 p04 = bs.BE_read_uint16();
     uint32 p05 = bs.BE_read_uint32();
     uint32 p06 = bs.BE_read_uint32();
     uint32 p07 = bs.BE_read_uint32();

     // read name
     char name[9];
     bs.read(&name[0], 8);
     name[8] = '\0';
     cout << " name = " << name << endl;
    }

 return true;
}

bool mdlextractor::processVtxLay(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::processIdxLay(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::processMtrCol(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::processMdlInfo(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::processEdGemLy(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::processHieLay(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::processLHeader(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::processMTRLCHNG(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::processACSPACK2(boost::shared_array<char> data, uint32 size)
{
 return true;
}

bool mdlextractor::extract(void)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // head header
 uint32 h01 = BE_read_uint32(ifile);
 uint32 h02 = BE_read_uint32(ifile); // 0x00
 uint32 h03 = BE_read_uint32(ifile); // 0x000100FF
 uint32 h04 = BE_read_uint32(ifile); // 0x30
 uint32 h05 = BE_read_uint32(ifile); // filesize
 uint32 h06 = BE_read_uint32(ifile); // 0x15
 uint32 h07 = BE_read_uint32(ifile); // 0x10
 uint32 h08 = BE_read_uint32(ifile); // 0x00
 uint32 h09 = BE_read_uint32(ifile); // offset

 // move to offset
 ifile.seekg(h09);
 if(ifile.fail()) return error("Seek failure.");

 // read offsets
 boost::shared_array<uint32> olist(new uint32[h06]);
 for(uint32 i = 0; i < h06; i++) {
     olist[i] = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
    }

 // remove zeroes from offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < h06; i++)
     if(olist[i] != 0x00) offsets.push_back(olist[i]);

 // process offsets
 for(uint32 i = 0; i < offsets.size(); i++)
    {
     // move to offset
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read chunktype
     uint64 chunktype = BE_read_uint64(ifile);
     if(ifile.fail()) return error("Read failure.");

     // compute chunksize
     uint32 chunksize = 0x00;
     if(i + 1 == offsets.size()) chunksize = filesize - offsets[i];
     else chunksize = offsets[i + 1] - offsets[i];
     chunksize -= 0x08;

     // read chunk
     boost::shared_array<char> data(new char[chunksize]);
     ifile.read(data.get(), chunksize);

     // process chunk
     switch(chunktype) {
       case(0x4D646C47656F0000L): if(!processMdlGeo(data, chunksize)) return false; break; // MdlGeo
       case(0x5454470000000000L): if(!processTTG(data, chunksize)) return false; break; // TTG
       case(0x5674784C61790000L): if(!processVtxLay(data, chunksize)) return false; break; // VtxLay
       case(0x4964784C61790000L): if(!processIdxLay(data, chunksize)) return false; break; // IdxLay
       case(0x4D7472436F6C0000L): if(!processMtrCol(data, chunksize)) return false; break; // MtrCol
       case(0x4D646C496E666F00L): if(!processMdlInfo(data, chunksize)) return false; break; // MdlInfo
       case(0x456447656D4C7900L): if(!processEdGemLy(data, chunksize)) return false; break; // EdGemLy 
       case(0x4869654C61790000L): if(!processHieLay(data, chunksize)) return false; break; // HieLay
       case(0x4C48656164657200L): if(!processLHeader(data, chunksize)) return false; break; // LHeader
       case(0x4D54524C43484E47L): if(!processMTRLCHNG(data, chunksize)) return false; break; // MTRLCHNG
       case(0x4143535041434B32L): if(!processACSPACK2(data, chunksize)) return false; break; // ACSPACK2
      }
    }

 return true;
}

bool processGMD(const std::string& filename)
{
 mdlextractor e(filename, true);
 return e.extract();
}

}};

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
 bool doTTG = true;
 bool doGMD = true;

 // process archive
 cout << "STAGE 1" << endl;
 if(doTTG) {
    cout << "Processing .TTG files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".TTGL", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTTG(filelist[i])) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 2" << endl;
 if(doGMD) {
    cout << "Processing .GMD files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".GMD", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processGMD(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

};};
