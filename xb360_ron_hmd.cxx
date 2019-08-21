#ifndef __XB360_RON_HMD_CXX_
#define __XB360_RON_HMD_CXX_

#define X_SYSTEM XBOX360
#define X_GAME   RON

namespace X_SYSTEM { namespace X_GAME {

struct HMDLHEADER {
 uint32 param01; // magic
 uint32 param02; // bytes of HMDL data
 uint32 param03; // start of GSGM data
 uint32 param04; // 0x1000000D
 uint32 param05; // 0x00
 uint32 param06; // bytes of GSGM data
 uint32 param07; // 0x00
 uint32 param08; // 0x00
};

struct GSGMHEADER {
 uint32 param01; // magic
 uint32 param02; // 0x102 (always)
 uint16 param03; // 0x200 (always)
 uint16 param04; // 0x800 (always)
 uint16 param05; // unknown (changes)
 uint16 param06; // unknown (changes)
 uint16 param07; // unknown (changes)
};

struct GSGMITEM00 {
 boost::shared_array<uint32> param01;
 boost::shared_array<real32> param02;
 boost::shared_array<uint32> param03;
};

struct GSGMITEM01 {
 boost::shared_array<uint32> param01;
 boost::shared_array<real32> param02;
};

struct GSGMITEM02 {
 boost::shared_array<uint32> faceinfo;
};

struct GSGMITEM03 {
 boost::shared_array<uint32> surfinfo;
};

struct GSGMITEM04 {
 boost::shared_array<uint32> param01;
};

struct GSGMITEM05 {
 boost::shared_array<real32> m;
};

struct GSGMITEM06 {
 uint32 param01;
 uint32 param02;
 uint32 param03;
 uint16 param04;
 uint16 param05;
 uint32 param06;
 uint32 param07;
 uint32 param08;
 uint32 param09;
};

struct GSGMITEM07 {
 boost::shared_array<char> data;
};

struct GSGMITEM08 {
 uint32 number;
 std::string name;
};

struct GSGMITEM09 {
 uint32 number;
 std::string name;
};

struct GSGMITEM10 {
 uint32 number;
 std::string name;
};

struct GSGMITEM11 {
 boost::shared_array<char> data;
};

struct GSGMITEM12 {
 boost::shared_array<char> data;
};

struct GSGMITEM13 {
 boost::shared_array<char> data;
};

class HMDProcessor {
 private :
  bool debug;
  std::string filename;
  std::string pathname;
  std::string shrtname;
  std::ifstream ifile;
  std::ofstream ofile;
  std::ofstream dfile;
 private :
  HMDLHEADER hmdl;
  GSGMHEADER gsgm;
  std::deque<GSGMITEM00> section00;
  std::deque<GSGMITEM01> section01;
  std::deque<GSGMITEM02> section02;
  std::deque<GSGMITEM03> section03;
  std::deque<GSGMITEM04> section04;
  std::deque<GSGMITEM05> section05;
  std::deque<GSGMITEM06> section06; // vertex information
  std::deque<GSGMITEM07> section07; // vertex data
  std::deque<GSGMITEM08> section08; // names (surfaces)
  std::deque<GSGMITEM09> section09; // names (???)
  std::deque<GSGMITEM10> section10; // names (bones)
  std::deque<GSGMITEM11> section11; // index data
  std::deque<GSGMITEM12> section12; // data (unknown)
  std::deque<GSGMITEM13> section13; // data (unknown)
 private :
  ADVANCEDMODELCONTAINER amc;
 private :
 public :
  bool extract(void);
 public :
  HMDProcessor(const std::string& fname);
 ~HMDProcessor();
};

HMDProcessor::HMDProcessor(const std::string& fname)
{
 filename = fname;
 pathname = GetPathnameFromFilename(fname);
 shrtname = GetShortFilenameWithoutExtension(fname);
}

HMDProcessor::~HMDProcessor()
{
}

bool HMDProcessor::extract(void)
{
 // open file
 using namespace std;
 ifile.open(filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // create debug file
 std::string dfname = pathname;
 dfname += shrtname;
 dfname += ".txt";
 dfile.open(dfname.c_str());
 if(!dfile.is_open()) return error("Failed to create debug file.");

 // read HMDL header
 hmdl.param01 = LE_read_uint32(ifile);
 hmdl.param02 = LE_read_uint32(ifile);
 hmdl.param03 = LE_read_uint32(ifile);
 hmdl.param04 = LE_read_uint32(ifile);
 hmdl.param05 = LE_read_uint32(ifile);
 hmdl.param06 = LE_read_uint32(ifile);
 hmdl.param07 = LE_read_uint32(ifile);
 hmdl.param08 = LE_read_uint32(ifile);
 if(hmdl.param01 != 0x4C444D48) return error("Invalid HMDL section.");

 // save GSGM position
 uint32 start = (uint32)ifile.tellg();
 if(ifile.fail()) return error("Tell failure.");

 // read GSGM header
 gsgm.param01 = LE_read_uint32(ifile);
 gsgm.param02 = LE_read_uint32(ifile);
 gsgm.param03 = LE_read_uint16(ifile);
 gsgm.param04 = LE_read_uint16(ifile);
 gsgm.param05 = LE_read_uint16(ifile);
 gsgm.param06 = LE_read_uint16(ifile);
 gsgm.param07 = LE_read_uint16(ifile);
 if(gsgm.param01 != 0x4D475347) return error("Invalid GSGM section.");

 dfile << "GSGM HEADER" << endl;
 dfile << "head01 = " << gsgm.param01 << endl;
 dfile << "head02 = " << gsgm.param02 << endl;
 dfile << "head03 = " << gsgm.param03 << endl;
 dfile << "head04 = " << gsgm.param04 << endl;
 dfile << "head05 = " << gsgm.param05 << endl;
 dfile << "head06 = " << gsgm.param06 << endl;
 dfile << "head07 = " << gsgm.param07 << endl;

 // read name
 char name[30];
 ifile.read(&name[0], 30);
 dfile << " name = " << name << endl;
 dfile << endl;

 // read (offset, information) pairs
 dfile << "OFFSET TABLE" << endl;
 uint32 n_offset_pairs = 18;
 uint32 offset[18];
 uint32 inform[18];
 for(uint32 i = 0; i < 14; i++) {
     offset[i] = BE_read_uint32(ifile) + start;
     inform[i] = BE_read_uint32(ifile);
     dfile << " item[" << setfill('0') << setw(2) << i << "] = <0x" << hex << offset[i] << ", 0x" << inform[i] << dec << ">" << endl;
    }
 dfile << endl;

 // floats that come after offset section
 auto temp01 = BE_read_real32(ifile);
 auto temp02 = BE_read_real32(ifile);
 auto temp03 = BE_read_real32(ifile);
 auto temp04 = BE_read_real32(ifile);
 auto temp05 = BE_read_real32(ifile);
 auto temp06 = BE_read_real32(ifile);
 auto temp07 = BE_read_real32(ifile);
 auto temp08 = BE_read_real32(ifile);
 auto temp09 = BE_read_real32(ifile);
 auto temp10 = BE_read_real32(ifile);
 auto temp11 = BE_read_real32(ifile);
 auto temp12 = BE_read_real32(ifile);
 dfile << "FLOATS" << endl;
 dfile << " item = " << temp01 << endl;
 dfile << " item = " << temp02 << endl;
 dfile << " item = " << temp03 << endl;
 dfile << " item = " << temp04 << endl;
 dfile << " item = " << temp05 << endl;
 dfile << " item = " << temp06 << endl;
 dfile << " item = " << temp07 << endl;
 dfile << " item = " << temp08 << endl;
 dfile << " item = " << temp09 << endl;
 dfile << " item = " << temp10 << endl;
 dfile << " item = " << temp11 << endl;
 dfile << " item = " << temp12 << endl;
 dfile << endl;

 // more offset information pairs
 dfile << "OFFSET TABLE" << endl;
 for(uint32 i = 14; i < n_offset_pairs; i++) {
     offset[i] = BE_read_uint32(ifile) + start;
     inform[i] = BE_read_uint32(ifile);
     dfile << " item[" << setfill('0') << setw(2) << i << "] = <0x" << hex << offset[i] << ", 0x" << inform[i] << dec << ">" << endl;
    }
 dfile << endl;

 // move to offset index 0x00
 // skeleton data
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x00 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[0]);
 if(ifile.fail()) return error("Seek failure.");

 for(uint32 i = 0; i < inform[0]; i++)
    {
     // read a bunch on numbers
     boost::shared_array<uint32> p01(new uint32[0x08]);
     BE_read_array(ifile, p01.get(), 0x08);
     print_hex_array(dfile, p01.get(), 0x08);
     dfile << " ";
 
     // read a bunch of floats
     boost::shared_array<real32> p02(new real32[0x14]);
     BE_read_array(ifile, p02.get(), 0x14);
     print_array(dfile, p02.get(), 0x14);
     dfile << " ";

     // read a bunch on numbers
     boost::shared_array<uint32> p03(new uint32[0x04]);
     BE_read_array(ifile, p03.get(), 0x04);
     print_hex_array(dfile, p03.get(), 0x04);
     dfile << endl;

     // save item
     GSGMITEM00 item = { p01, p02, p03 };
     section00.push_back(item);
    }
 dfile << endl;

 // move to offset index 0x01
 // matrices for something
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x01 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[1]);
 if(ifile.fail()) return error("Seek failure.");

 for(uint32 i = 0; i < inform[1]; i++)
    {
     // read four numbers
     boost::shared_array<uint32> p01(new uint32[0x04]);
     BE_read_array(ifile, p01.get(), 0x04);
     print_hex_array(dfile, p01.get(), 0x04);
     dfile << " ";

     // read a bunch of floats
     boost::shared_array<real32> p02(new real32[0x0C]);
     BE_read_array(ifile, p02.get(), 0x0C);
     print_array(dfile, p02.get(), 0x0C);
     dfile << endl;

     // save item
     GSGMITEM01 item = { p01, p02 };
     section01.push_back(item);
    }
 dfile << endl;

 // move to offset index 0x02
 // face table
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x02 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[2]);
 if(ifile.fail()) return error("Seek failure.");

 for(uint32 i = 0; i < inform[2]; i++)
    {
     // read a bunch of numbers
     boost::shared_array<uint32> p01(new uint32[0x10]);
     BE_read_array(ifile, p01.get(), 0x10);
     print_hex_array(dfile, p01.get(), 0x10);
     dfile << endl;

     // save item
     GSGMITEM02 item = { p01 };
     section02.push_back(item);
    }
 dfile << endl;

 // move to offset index 0x03
 // surface information
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x03 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[3]);
 if(ifile.fail()) return error("Seek failure.");

 for(uint32 i = 0; i < inform[3]; i++)
    {
     // read a bunch of numbers
     boost::shared_array<uint32> p01(new uint32[0x20]);
     BE_read_array(ifile, p01.get(), 0x20);
     print_hex_array(dfile, p01.get(), 0x20);
     dfile << endl;

     // save item
     GSGMITEM03 item = { p01 };
     section03.push_back(item);

     // create surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << i;

     // create surface material
     AMC_SURFMAT mat;
     mat.name = "";
     mat.twoside = 0x00;
     mat.basemap = (p01[0x08] == 0xFFFF ? AMC_INVALID_TEXTURE : p01[0x08]);
     mat.specmap = AMC_INVALID_TEXTURE;
     mat.tranmap = AMC_INVALID_TEXTURE;
     mat.bumpmap = AMC_INVALID_TEXTURE;
     mat.normmap = AMC_INVALID_TEXTURE;
     mat.resmap1 = AMC_INVALID_TEXTURE;
     mat.resmap2 = AMC_INVALID_TEXTURE;
     mat.resmap3 = AMC_INVALID_TEXTURE;
     mat.resmap4 = AMC_INVALID_TEXTURE;
     if(mat.basemap == AMC_INVALID_TEXTURE && p01[0x09] != 0xFFFF) mat.basemap = p01[0x09]; // use cubemap instead
     amc.surfmats.push_back(mat);

     // save surface
     AMC_SURFACE surface;
     surface.name = ss.str();
     surface.surfmat = i;
     amc.surfaces.push_back(surface);
    }
 dfile << endl;

 // move to offset index 0x04
 // unknown
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x04 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[4]);
 if(ifile.fail()) return error("Seek failure.");

 for(uint32 i = 0; i < inform[4]; i++)
    {
     // read a bunch of numbers
     boost::shared_array<uint32> p01(new uint32[0x04]);
     BE_read_array(ifile, p01.get(), 0x04);
     print_hex_array(dfile, p01.get(), 0x04);
     dfile << endl;

     // save item
     GSGMITEM04 item = { p01 };
     section04.push_back(item);
    }
 dfile << endl;

 // move to offset index 0x05
 // absolute joint position/rotation matrices
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x05 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[5]);
 if(ifile.fail()) return error("Seek failure.");

 for(uint32 i = 0; i < inform[5]; i++)
    {
     // read matrix
     boost::shared_array<real32> p01(new real32[0x10]);
     BE_read_array(ifile, p01.get(), 0x10);
     dfile << "m[0x" << hex << setfill('0') << setw(2) << i << "] = " << dec;
     print_array(dfile, p01.get(), 0x10);
     dfile << endl;

     // save item
     GSGMITEM05 item = { p01 };
     section05.push_back(item);
    }
 dfile << endl;

 // move to offset index 0x06
 // vertex format
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x06 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[6]);
 if(ifile.fail()) return error("Seek failure.");

 for(uint32 i = 0; i < inform[6]; i++)
    {
     GSGMITEM06 item;
     item.param01 = BE_read_uint32(ifile);
     item.param02 = BE_read_uint32(ifile);
     item.param03 = BE_read_uint32(ifile);
     item.param04 = BE_read_uint16(ifile);
     item.param05 = BE_read_uint16(ifile);
     item.param06 = BE_read_uint32(ifile);
     item.param07 = BE_read_uint32(ifile);
     item.param08 = BE_read_uint32(ifile);
     item.param09 = BE_read_uint32(ifile);
     section06.push_back(item);

     dfile << "VERTEX INFO #" << i << endl;
     dfile << " param01 = " << item.param01 << endl; // index
     dfile << " param02 = " << item.param02 << endl; // vertices
     dfile << " param03 = " << item.param03 << endl; // unknown
     dfile << " param04 = " << item.param04 << endl; // unknown
     dfile << " param05 = " << item.param05 << endl; // unknown
     dfile << " param06 = " << item.param06 << endl; // offset
     dfile << " param07 = " << item.param07 << endl; // length
     dfile << " param08 = " << item.param08 << endl; // vertex size
     dfile << " param09 = " << item.param09 << endl; // unknown
    }
 dfile << endl;

 // move to offset index 0x07
 // vertex data
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x07 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[0x07]);
 if(ifile.fail()) return error("Seek failure.");

 for(size_t i = 0; i < section06.size(); i++)
    {
     // read vertex data
     boost::shared_array<char> vtxdata(new char[section06[i].param07]);
     ifile.read(vtxdata.get(), section06[i].param07);

     // save item
     GSGMITEM07 item = { vtxdata };
     section07.push_back(item);

     // create binary stream from data
     binary_stream bs(vtxdata, section06[i].param07);

     // create vertex buffer
     AMC_VTXBUFFER vbuffer;
     vbuffer.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV;
     vbuffer.name = ""; // not important
     vbuffer.elem = section06[i].param02;
     vbuffer.data.reset(new AMC_VERTEX[vbuffer.elem]);

     // weights and bones
     if(section06[i].param03 == 0x04) {
        vbuffer.flags |= AMC_VERTEX_WEIGHTS;
        vbuffer.flags |= AMC_VERTEX_BONEIDX;
       }

     // read vertex buffer
     for(uint32 j = 0; j < vbuffer.elem; j++)
        {
         if(section06[i].param08 == 0x14)
           {
            // print hex
            binary_stream cs(bs);
            for(uint32 k = 0; k < 0x14; k++) dfile << hex << setfill('0') << setw(2) << (uint32)cs.BE_read_uint08() << dec;
            dfile << " - 0x" << hex << setfill('0') << section06[i].param03 << " 0x" << setfill('0') << section06[i].param04 << " 0x" << setfill('0') << section06[i].param05 << dec;
            dfile << endl;

            vbuffer.data[j].vx = bs.BE_read_real32(); //  4
            vbuffer.data[j].vy = bs.BE_read_real32(); //  8
            vbuffer.data[j].vz = bs.BE_read_real32(); // 12
            bs.BE_read_uint08(); // 13
            bs.BE_read_uint08(); // 14
            bs.BE_read_uint08(); // 15
            bs.BE_read_uint08(); // 16
            vbuffer.data[j].tu = bs.BE_read_real16(); // 18
            vbuffer.data[j].tv = bs.BE_read_real16(); // 20
           }
         else if(section06[i].param08 == 0x1C)
           {
            // print hex
            binary_stream cs(bs);
            for(uint32 k = 0; k < 0x1C; k++) dfile << hex << setfill('0') << setw(2) << (uint32)cs.BE_read_uint08() << dec;
            dfile << " - 0x" << hex << setfill('0') << section06[i].param03 << " 0x" << setfill('0') << section06[i].param04 << " 0x" << setfill('0') << section06[i].param05 << dec;
            dfile << endl;

            if(section06[i].param03 == 0x04)
              {
               vbuffer.data[j].vx = bs.BE_read_real32(); //  4
               vbuffer.data[j].vy = bs.BE_read_real32(); //  8
               vbuffer.data[j].vz = bs.BE_read_real32(); // 12
               vbuffer.data[j].wv[0] = bs.BE_read_uint08()/255.0f; // 13
               vbuffer.data[j].wv[1] = bs.BE_read_uint08()/255.0f; // 14
               vbuffer.data[j].wv[2] = bs.BE_read_uint08()/255.0f; // 15
               vbuffer.data[j].wv[3] = bs.BE_read_uint08()/255.0f; // 16
               vbuffer.data[j].wv[4] = 0.0f;
               vbuffer.data[j].wv[5] = 0.0f;
               vbuffer.data[j].wv[6] = 0.0f;
               vbuffer.data[j].wv[7] = 0.0f;
               vbuffer.data[j].wi[0] = bs.BE_read_uint08(); // 17
               vbuffer.data[j].wi[1] = bs.BE_read_uint08(); // 18
               vbuffer.data[j].wi[2] = bs.BE_read_uint08(); // 19
               vbuffer.data[j].wi[3] = bs.BE_read_uint08(); // 20
               vbuffer.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vbuffer.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vbuffer.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vbuffer.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               bs.BE_read_uint08(); // 21
               bs.BE_read_uint08(); // 22
               bs.BE_read_uint08(); // 23
               bs.BE_read_uint08(); // 24
               vbuffer.data[j].tu = bs.BE_read_real16(); // 26
               vbuffer.data[j].tv = bs.BE_read_real16(); // 28
              }
            else if(section06[i].param03 == 0xEFFFFFF4)
              {
               vbuffer.data[j].vx = bs.BE_read_real32(); //  4
               vbuffer.data[j].vy = bs.BE_read_real32(); //  8
               vbuffer.data[j].vz = bs.BE_read_real32(); // 12
               bs.BE_read_uint08(); // 13
               bs.BE_read_uint08(); // 14
               bs.BE_read_uint08(); // 15
               bs.BE_read_uint08(); // 16
               bs.BE_read_uint08(); // 17
               bs.BE_read_uint08(); // 18
               bs.BE_read_uint08(); // 19
               bs.BE_read_uint08(); // 20
               vbuffer.data[j].tu = bs.BE_read_real16(); // 22
               vbuffer.data[j].tv = bs.BE_read_real16(); // 24
               bs.BE_read_uint16(); // 26
               bs.BE_read_uint16(); // 28
              }
            else
               return error("Unknown 0x1C vertex format.");
           }
         else if(section06[i].param08 == 0x20)
           {
            // print hex
            binary_stream cs(bs);
            for(uint32 k = 0; k < 0x20; k++) dfile << hex << setfill('0') << setw(2) << (uint32)cs.BE_read_uint08() << dec;
            dfile << " - 0x" << hex << setfill('0') << section06[i].param03 << " 0x" << setfill('0') << section06[i].param04 << " 0x" << setfill('0') << section06[i].param05 << dec;
            dfile << endl;

            if(section06[i].param03 == 0x04)
              {
               vbuffer.data[j].vx = bs.BE_read_real32(); //  4
               vbuffer.data[j].vy = bs.BE_read_real32(); //  8
               vbuffer.data[j].vz = bs.BE_read_real32(); // 12
               vbuffer.data[j].wv[0] = bs.BE_read_uint08()/255.0f; // 13
               vbuffer.data[j].wv[1] = bs.BE_read_uint08()/255.0f; // 14
               vbuffer.data[j].wv[2] = bs.BE_read_uint08()/255.0f; // 15
               vbuffer.data[j].wv[3] = bs.BE_read_uint08()/255.0f; // 16
               vbuffer.data[j].wv[4] = 0.0f;
               vbuffer.data[j].wv[5] = 0.0f;
               vbuffer.data[j].wv[6] = 0.0f;
               vbuffer.data[j].wv[7] = 0.0f;
               vbuffer.data[j].wi[0] = bs.BE_read_uint08(); // 17
               vbuffer.data[j].wi[1] = bs.BE_read_uint08(); // 18
               vbuffer.data[j].wi[2] = bs.BE_read_uint08(); // 19
               vbuffer.data[j].wi[3] = bs.BE_read_uint08(); // 20
               vbuffer.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vbuffer.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vbuffer.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vbuffer.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               bs.BE_read_sint08(); // 21
               bs.BE_read_sint08(); // 22
               bs.BE_read_sint08(); // 23
               bs.BE_read_sint08(); // 24
               bs.BE_read_sint08(); // 25
               bs.BE_read_sint08(); // 26
               bs.BE_read_sint08(); // 27
               bs.BE_read_sint08(); // 28
               vbuffer.data[j].tu = bs.BE_read_real16(); // 30
               vbuffer.data[j].tv = bs.BE_read_real16(); // 32
              }
            else if(section06[i].param03 == 0xEFFFFFF4)
              {
               vbuffer.data[j].vx = bs.BE_read_real32(); //  4
               vbuffer.data[j].vy = bs.BE_read_real32(); //  8
               vbuffer.data[j].vz = bs.BE_read_real32(); // 12
               bs.BE_read_uint32(); // 16
               bs.BE_read_uint32(); // 20
               bs.BE_read_uint32(); // 24
               vbuffer.data[j].tu = bs.BE_read_real16(); // 26
               vbuffer.data[j].tv = bs.BE_read_real16(); // 28
               bs.BE_read_uint32(); // 32
              }
            else if(section06[i].param03 == 0xEFFFFF44)
              {
               vbuffer.data[j].vx = bs.BE_read_real32(); //  4
               vbuffer.data[j].vy = bs.BE_read_real32(); //  8
               vbuffer.data[j].vz = bs.BE_read_real32(); // 12
               bs.BE_read_uint32(); // 16
               bs.BE_read_uint32(); // 20
               bs.BE_read_real16(); // 22
               bs.BE_read_real16(); // 24
               vbuffer.data[j].tu = bs.BE_read_real16(); // 26
               vbuffer.data[j].tv = bs.BE_read_real16(); // 28
               bs.BE_read_uint32(); // 32
              }
            else if(section06[i].param03 == 0xEFFF4FF4)
              {
               vbuffer.data[j].vx = bs.BE_read_real32(); //  4
               vbuffer.data[j].vy = bs.BE_read_real32(); //  8
               vbuffer.data[j].vz = bs.BE_read_real32(); // 12
               bs.BE_read_uint32(); // 16
               bs.BE_read_uint32(); // 20
               bs.BE_read_real16(); // 22
               bs.BE_read_real16(); // 24
               vbuffer.data[j].tu = bs.BE_read_real16(); // 26
               vbuffer.data[j].tv = bs.BE_read_real16(); // 28
               bs.BE_read_uint32(); // 32
              }
            else
               return error("Unknown 0x20 vertex format.");
           }
         else if(section06[i].param08 == 0x24)
           {
            // print hex
            binary_stream cs(bs);
            for(uint32 k = 0; k < 0x24; k++) dfile << hex << setfill('0') << setw(2) << (uint32)cs.BE_read_uint08() << dec;
            dfile << " - 0x" << hex << setfill('0') << section06[i].param03 << " 0x" << setfill('0') << section06[i].param04 << " 0x" << setfill('0') << section06[i].param05 << dec;
            dfile << endl;

            if(section06[i].param03 == 0xEFFFFF44)
              {
               vbuffer.data[j].vx = bs.BE_read_real32(); //  4
               vbuffer.data[j].vy = bs.BE_read_real32(); //  8
               vbuffer.data[j].vz = bs.BE_read_real32(); // 12
               bs.BE_read_uint32(); // 16
               bs.BE_read_uint32(); // 20
               bs.BE_read_uint32(); // 24
               bs.BE_read_real16(); // 26
               bs.BE_read_real16(); // 28
               vbuffer.data[j].tu = bs.BE_read_real16(); // 30
               vbuffer.data[j].tv = bs.BE_read_real16(); // 32
               bs.BE_read_uint32(); // 36
              }
            else
               return error("Unknown 0x24 vertex format.");
           }
         else if(section06[i].param08 == 0x28)
           {
            // print hex
            binary_stream cs(bs);
            for(uint32 k = 0; k < 0x28; k++) dfile << hex << setfill('0') << setw(2) << (uint32)cs.BE_read_uint08() << dec;
            dfile << " - 0x" << hex << setfill('0') << section06[i].param03 << " 0x" << setfill('0') << section06[i].param04 << " 0x" << setfill('0') << section06[i].param05 << dec;
            dfile << endl;

            if(section06[i].param03 == 0x04)
              {
               vbuffer.data[j].vx = bs.BE_read_real32(); //  4
               vbuffer.data[j].vy = bs.BE_read_real32(); //  8
               vbuffer.data[j].vz = bs.BE_read_real32(); // 12
               vbuffer.data[j].wv[0] = bs.BE_read_uint08()/255.0f; // 13
               vbuffer.data[j].wv[1] = bs.BE_read_uint08()/255.0f; // 14
               vbuffer.data[j].wv[2] = bs.BE_read_uint08()/255.0f; // 15
               vbuffer.data[j].wv[3] = bs.BE_read_uint08()/255.0f; // 16
               vbuffer.data[j].wv[4] = 0.0f;
               vbuffer.data[j].wv[5] = 0.0f;
               vbuffer.data[j].wv[6] = 0.0f;
               vbuffer.data[j].wv[7] = 0.0f;
               vbuffer.data[j].wi[0] = bs.BE_read_uint08(); // 17
               vbuffer.data[j].wi[1] = bs.BE_read_uint08(); // 18
               vbuffer.data[j].wi[2] = bs.BE_read_uint08(); // 19
               vbuffer.data[j].wi[3] = bs.BE_read_uint08(); // 20
               vbuffer.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vbuffer.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vbuffer.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
               vbuffer.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
               bs.BE_read_uint32(); // 24
               bs.BE_read_uint32(); // 28
               bs.BE_read_uint32(); // 32
               bs.BE_read_uint32(); // 36
               vbuffer.data[j].tu = bs.BE_read_real16(); // 38
               vbuffer.data[j].tv = bs.BE_read_real16(); // 40
              }
            else
               return error("Unknown 0x28 vertex format.");
           }
         else {
            stringstream ss;
            ss << "Unknown vertex size 0x" << hex << section06[i].param08 << dec << "." << endl;
            return error(ss.str().c_str());
           }
        }

     // save vertex buffer
     amc.vblist.push_back(vbuffer);
    }
 dfile << endl;

 // move to offset index 0x08
 // texture association
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x08 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[0x08]);
 if(ifile.fail()) return error("Seek failure.");

 // read surface names
 for(uint32 i = 0; i < inform[0x08]; i++)
    {
     // read number/name pair
     uint16 number = BE_read_uint16(ifile);
     char name[30];     
     ifile.read(&name[0], 30);
     dfile << "name[0x" << setfill('0') << setw(2) << hex << i << "] = (0x" << number << dec << ", " << name << ")" << endl;

     // save item
     GSGMITEM08 item = { number, name };
     section08.push_back(item);

     // insert texture filename
     AMC_IMAGEFILE file;
     file.filename = name;
     file.filename += ".tga";
     amc.iflist.push_back(file);
    }
 dfile << endl;

 // move to offset index 0x09
 // ???
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x09 -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[0x09]);
 if(ifile.fail()) return error("Seek failure.");

 // read names
 for(uint32 i = 0; i < inform[0x09]; i++)
    {
     // read number/name pair
     uint16 number = BE_read_uint16(ifile);
     char name[30];     
     ifile.read(&name[0], 30);
     dfile << "name[0x" << setfill('0') << setw(2) << hex << i << "] = (0x" << number << dec << ", " << name << ")" << endl;

     // save item
     GSGMITEM09 item = { number, name };
     section09.push_back(item);
    }
 dfile << endl;

 // move to offset index 0x0A
 // bone names
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x0A -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[0x0A]);
 if(ifile.fail()) return error("Seek failure.");

 // read names
 for(uint32 i = 0; i < inform[0x0A]; i++)
    {
     // read number/name pair
     uint16 number = BE_read_uint16(ifile);
     char name[30];     
     ifile.read(&name[0], 30);
     dfile << "name[0x" << setfill('0') << setw(2) << hex << i << "] = (0x" << number << dec << ", " << name << ")" << endl;

     // save item
     GSGMITEM10 item = { number, name };
     section10.push_back(item);
    }
 dfile << endl;

 // move to offset index 0x0B
 // ???
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x0B -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[0x0B]);
 if(ifile.fail()) return error("Seek failure.");

 // count indices
 uint32 idxelem = inform[0x0B];
 dfile << "number of indices = " << idxelem << endl;
 dfile << endl;

 // read indices
 boost::shared_array<char> idxdata(new char[idxelem*sizeof(uint16)]);
 BE_read_array(ifile, reinterpret_cast<uint16*>(idxdata.get()), idxelem);

 // save indices
 GSGMITEM11 item = { idxdata };
 section11.push_back(item);

 // for each item in the face table
 size_t level = 0; // 0, 1, or 2
 for(size_t i = 0; i < section02.size(); i++)
    {
     // index buffer information
     boost::shared_array<uint32> faceinfo = section02[i].faceinfo;
     uint32 lengths[3] = { faceinfo[4], faceinfo[6], faceinfo[8] };
     uint32 offsets[3] = { faceinfo[5], faceinfo[7], faceinfo[9] };

     // surface index
     uint32 surface_index = faceinfo[1];
     if(!(surface_index < section03.size())) return error("Invalid surface index.");
     boost::shared_array<uint32> surface_info = section03[surface_index].surfinfo;

     // vertex buffer index
     uint32 vbuffer_index = faceinfo[2];
     if(!(vbuffer_index < section06.size())) return error("Invalid vertex buffer index.");

     // color map information
     uint32 color_map_index = surface_info[8];
     bool has_color_map = true;
     if(color_map_index == 0xFFFF) has_color_map = false;
     else if(!(color_map_index < section08.size())) return error("Invalid color map index.");

     // cube map information
     uint32 cube_map_index = surface_info[9];
     bool has_cube_map = true;
     if(cube_map_index == 0xFFFF) has_cube_map = false;
     else if(!(cube_map_index < section08.size())) return error("Invalid cube map index.");

     // process LOD #0
     uint32 curr_length = lengths[0];
     uint32 curr_offset = offsets[0];
     if(curr_length % 3) return error("LOD0 is not a triangle list.");

     // match index buffer name with surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << surface_index;

     // save index buffer
     AMC_IDXBUFFER ibuffer;
     ibuffer.format = AMC_UINT16;
     ibuffer.type = AMC_TRIANGLES;
     ibuffer.name = ss.str();
     ibuffer.elem = curr_length;
     ibuffer.data = idxdata;
     amc.iblist.push_back(ibuffer);

     // save buffer reference
     AMC_REFERENCE ref;
     ref.vb_index = vbuffer_index;
     ref.vb_start = 0;
     ref.vb_width = section06[vbuffer_index].param02;
     ref.ib_index = 0; // LOD #0
     ref.ib_start = curr_offset;
     ref.ib_width = curr_length;
     amc.surfaces[surface_index].refs.push_back(ref);
    }

 // move to offset index 0x0C
 // ???
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x0C -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[0x0C]);
 if(ifile.fail()) return error("Seek failure.");

 // read data
 if(inform[0x0C]) {
    boost::shared_array<char> data(new char[inform[0x0C]]);
    ifile.read(data.get(), inform[0x0C]);
    GSGMITEM12 item = { data };
    section12.push_back(item);
   }
 dfile << endl;

 // move to offset index 0x0D
 // ???
 dfile << "---------------------" << endl;
 dfile << "- OFFSET INDEX 0x0D -" << endl;
 dfile << "---------------------" << endl;

 ifile.seekg(offset[0x0D]);
 if(ifile.fail()) return error("Seek failure.");

 // read data
 if(inform[0x0D]) {
    boost::shared_array<char> data(new char[inform[0x0D]]);
    ifile.read(data.get(), inform[0x0D]);
    GSGMITEM13 item = { data };
    section13.push_back(item);
   }
 dfile << endl;

 // now that we've read all the data
 // align pointer to 0x10 bytes
 uint32 position = (uint32)ifile.tellg();
 position = ((position + 0x0F) & ~0x0F);
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.");

 // skip 0x10 bytes at a time until we reach something important
 for(;;)
    {
     // read line
     uint32 temp[4];
     LE_read_array(ifile, &temp[0], 4);
     if(ifile.fail()) return error("Read failure.");

     // process HMHD
     if(temp[0] == 0x44484D48) {
        ifile.seekg((temp[1] + temp[2] - 0x10), ios::cur);
        if(ifile.fail()) return error("Seek failure.");
        break;
       }

     // process EOFC
     if(temp[0] == 0x43464F45)
        break;
    }

 // save LWO
 SaveLWO(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

}};

#endif
