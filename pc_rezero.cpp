#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_capcom.h"
#include "pc_rezero.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   REZERO

namespace X_SYSTEM { namespace X_GAME {

struct MATERIALITEM {
 uint32 p01; // signature
 uint32 p02; // id
 uint32 p03; // size of material property data
 uint32 p04[4]; // 1st row of data
 uint32 p05[4]; // 2nd row of data
 uint32 p06[4]; // 3rd row of data
 uint32 t_BM;   // _BM diffuse map
 uint32 t_SM;   // _SM sphere map
 uint32 t_NM;   // _NM normal map
 uint32 t_DM;   // _DM damage normal map
 uint32 t_MM;   // _MM specular map
};

struct MATERIALLIST {
 std::deque<std::string> filelist;
 std::deque<MATERIALITEM> items;
};

bool ExtractMRL(LPCTSTR filename, MATERIALLIST& matlist)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return false;
 if(filesize == 0) return false;

 // read file
 boost::shared_array<char> filedata(new char[filesize]);
 ifile.read(filedata.get(), filesize);
 if(ifile.fail()) return false;

 // close file
 ifile.close();

 // binary stream
 binary_stream bs(filedata, filesize);
 bs.set_endian_mode(ENDIAN_LITTLE);

 // read header
 uint32 h01 = bs.read_uint32(); // magic
 uint32 h02 = bs.read_uint32(); // 0x22
 uint32 h03 = bs.read_uint32(); // number of materials
 uint32 h04 = bs.read_uint32(); // number of textures
 uint32 h05 = bs.read_uint32(); // ???
 uint32 h06 = bs.read_uint32(); // offset to textures
 uint32 h07 = bs.read_uint32(); // offset to materials
 if(bs.fail()) return false;

 // validate
 if(h01 != 0x004C524D) return false;
 if(!h03 || !h04) return false;

 // move to filelist
 bs.seek(h06);
 if(bs.fail()) return false;

 // read filelist
 for(uint32 i = 0; i < h04; i++) {
     uint32 p01 = bs.read_uint32(); // signature
     uint32 p02 = bs.read_uint32(); // 0x00000000
     uint32 p03 = bs.read_uint32(); // 0x00000000
     char p04[0x40];
     bs.read(&p04[0], 0x40);
     string temp = p04;
     temp += ".dds";
     matlist.filelist.push_back(temp);
    }

 // move to material list
 bs.seek(h07);
 if(bs.fail()) return false;

 // read material list (0x3C-bytes per entry)
 // E4EBB05F 1158BA12 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - E0090000 - 00000000
 // E4EBB05F 228F0DF7 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - 300D0000 - 00000000
 // E4EBB05F B4BF0A80 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - 80100000 - 00000000
 // E4EBB05F F5A15E6E 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - D0130000 - 00000000
 // E4EBB05F 0EEE0319 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - 20170000 - 00000000
 // E4EBB05F 48DDEF10 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - 701A0000 - 00000000
 // E4EBB05F 3B4B60F0 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - C01D0000 - 00000000
 // E4EBB05F 9D7B811D 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - 10210000 - 00000000
 // E4EBB05F 811A6969 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - 60240000 - 00000000
 // E4EBB05F AC3C4C9A 50030000 - 78D1B262 AB9113B8 B4F18C10 "24C"00DA0 - 0000808C - zeros - B0270000 - 00000000
 for(uint32 i = 0; i < h03; i++) {
     MATERIALITEM item;
     item.p01 = bs.read_uint32();
     item.p02 = bs.read_uint32();
     item.p03 = bs.read_uint32();
     bs.read_array(&item.p04[0], 4);
     bs.read_array(&item.p05[0], 4);
     bs.read_array(&item.p06[0], 4);
     if(bs.fail()) return error("", __LINE__);
     matlist.items.push_back(item);
    }

 // load material properties
 for(uint32 i = 0; i < h03; i++)
    {
     // offset to texture information
     uint32 size = (matlist.items[i].p04[3] & 0xFF00ul) >> 12;
     uint16 elem = (matlist.items[i].p04[3] & 0xFF);

     // capcom uses 0 as invalid texture, or map not used, as texture indices are 1-based
     matlist.items[i].t_BM = 0;
     matlist.items[i].t_SM = 0;
     matlist.items[i].t_NM = 0;
     matlist.items[i].t_DM = 0;
     matlist.items[i].t_MM = 0;

     // read entries
     if(size == 0x0C)
       {
        // move to texture information
        bs.seek(matlist.items[i].p06[2]);
        if(bs.fail()) return false;

        // read texture information
        for(uint32 j = 0; j < elem; j++)
           {
            // read texture information parameters
            uint32 p01 = bs.read_uint32();
            uint32 p02 = bs.read_uint32();
            uint32 p03 = bs.read_uint32();
            if(bs.fail()) return false;
            
            // assign texture but do not subtract one just yet
            switch(p01) {
              case(0x350DCDC3): matlist.items[i].t_BM = p02; break; // _BM
              case(0x351DCDC3): matlist.items[i].t_SM = p02; break; // _SM
              case(0x353DCDC3): matlist.items[i].t_NM = p02; break; // _NM
              case(0x355DCDC3): matlist.items[i].t_DM = p02; break; // _DM
              case(0x35BDCDC3): break; // unknown
              case(0x364DCDC3): matlist.items[i].t_MM = p02; break; // _MM
              default : {
                   if((p01 & 0xFFFF) == 0xCDC3) {
                      stringstream ss;
                      ss << "Unknown texture type 0x" << hex << p01 << dec << ".";
                      return error(ss.str().c_str());
                     }
                  }
             }
           }
       }
    }

 return true;
}

bool ExtractMOD(LPCTSTR filename)
{
 //
 // PHASE #1
 // MOD FILE OPEN AND READ
 //

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open ARC file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return error("Empty ARC file.");

 // read arc file
 boost::shared_array<char> filedata(new char[filesize]);
 ifile.read(filedata.get(), filesize);
 if(ifile.fail()) return error("Read failure.");

 // close arc file
 ifile.close();

 // model container
 ADVANCEDMODELCONTAINER amc;

 //
 // PHASE #2
 // MATERIAL ANALYSIS
 //

 // extract material data
 STDSTRINGSTREAM mtlfn;
 mtlfn << pathname << shrtname << TEXT(".mrl");
 bool has_mrl = false;
 MATERIALLIST matlist;
 // if(!ExtractMRL(mtlfn.str().c_str(), matlist)) has_mrl = false;

 // construct materials
 // remember to subtract one since 0 means "doesn't use" or "doesn't exist"
 if(has_mrl)
   {
    // insert images
    for(uint32 i = 0; i < matlist.filelist.size(); i++) {
        AMC_IMAGEFILE image;
        image.filename = Unicode16ToUnicode08(GetShortFilename(Unicode08ToUnicode16(matlist.filelist[i].c_str()).get()).get()).get(); 
        amc.iflist.push_back(image);
       }

    // insert materials
    for(uint32 i = 0; i < matlist.items.size(); i++)
       {
        // create material name
        stringstream ss;
        ss << "material_" << setfill('0') << setw(2) << i;

        // insert material
        AMC_SURFMAT mat;
        mat.name = ss.str().c_str();
        mat.twoside = 0;
        mat.unused1 = 0;
        mat.unused2 = 0;
        mat.unused3 = 0;
        mat.basemap = (matlist.items[i].t_BM > 0 ? (matlist.items[i].t_BM - 1) : AMC_INVALID_TEXTURE);
        mat.specmap = (matlist.items[i].t_MM > 0 ? (matlist.items[i].t_MM - 1) : AMC_INVALID_TEXTURE);
        mat.tranmap = AMC_INVALID_TEXTURE;
        mat.bumpmap = (matlist.items[i].t_DM > 0 ? (matlist.items[i].t_DM - 1) : AMC_INVALID_TEXTURE);
        mat.normmap = (matlist.items[i].t_NM > 0 ? (matlist.items[i].t_NM - 1) : AMC_INVALID_TEXTURE);
        mat.lgthmap = AMC_INVALID_TEXTURE;
        mat.envimap = (matlist.items[i].t_SM > 0 ? (matlist.items[i].t_SM - 1) : AMC_INVALID_TEXTURE);
        mat.glssmap = AMC_INVALID_TEXTURE;
        mat.resmap1 = AMC_INVALID_TEXTURE;
        mat.resmap2 = AMC_INVALID_TEXTURE;
        mat.resmap3 = AMC_INVALID_TEXTURE;
        mat.resmap4 = AMC_INVALID_TEXTURE;
        mat.resmap5 = AMC_INVALID_TEXTURE;
        mat.resmap6 = AMC_INVALID_TEXTURE;
        mat.resmap7 = AMC_INVALID_TEXTURE;
        mat.resmap8 = AMC_INVALID_TEXTURE;
        mat.basemapchan = 0;
        mat.specmapchan = 0;
        mat.tranmapchan = 0;
        mat.bumpmapchan = 0;
        mat.normmapchan = 0;
        mat.lghtmapchan = 0;
        mat.envimapchan = 0;
        mat.glssmapchan = 0;
        mat.resmapchan1 = 0;
        mat.resmapchan2 = 0;
        mat.resmapchan3 = 0;
        mat.resmapchan4 = 0;
        mat.resmapchan5 = 0;
        mat.resmapchan6 = 0;
        mat.resmapchan7 = 0;
        mat.resmapchan8 = 0;
        amc.surfmats.push_back(mat);
       }
   }

 //
 // PHASE #3
 // FILE HEADER ANALYSIS
 //

 // debug
 bool debug = true;
 ofstream ofile;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT(".txt");
    ofile.open(ss.str().c_str());
    if(!ofile) return error("Failed to create debug file.");
    ofile << setfill('0');
   }

 // binary stream
 binary_stream bs(filedata, filesize);
 bs.set_endian_mode(ENDIAN_LITTLE);

 // read header
 uint32 h01 = bs.read_uint32(); // 
 uint16 h02 = bs.read_uint16(); // 
 uint16 h03 = bs.read_uint16(); // number of joints
 uint16 h04 = bs.read_uint16(); // number of entries at offset h15
 uint16 h05 = bs.read_uint16(); // number of surface names at offset h14
 uint32 h06 = bs.read_uint32(); // 
 uint32 h07 = bs.read_uint32(); // number of indices in index buffer
 uint32 h08 = bs.read_uint32(); // 
 uint32 h09 = bs.read_uint32(); // size of vertex buffer
 uint32 h10 = bs.read_uint32(); // 
 uint32 h11 = bs.read_uint32(); // number of 0x20 byte entries at offset h13
 uint32 h12 = bs.read_uint32(); // offset to bone data
 uint32 h13 = bs.read_uint32(); // offset to some data
 uint32 h14 = bs.read_uint32(); // offset to some data (0x0F material names)
 uint32 h15 = bs.read_uint32(); // offset to some data (0x6C entries of 0x30 bytes each)
 uint32 h16 = bs.read_uint32(); // offset to vertex buffer
 uint32 h17 = bs.read_uint32(); // offset to index buffer
 uint32 h18 = bs.read_uint32(); // offset to EOF
 real32 h19 = bs.read_real32(); // bounding box
 real32 h20 = bs.read_real32(); // bounding box
 real32 h21 = bs.read_real32(); // bounding box
 real32 h22 = bs.read_real32(); // 
 real32 h23 = bs.read_real32(); // 
 real32 h24 = bs.read_real32(); // 
 real32 h25 = bs.read_real32(); // 
 real32 h26 = bs.read_real32(); // 
 real32 h27 = bs.read_real32(); // 
 real32 h28 = bs.read_real32(); // 
 real32 h29 = bs.read_real32(); // 
 real32 h30 = bs.read_real32(); // 
 uint32 h31 = bs.read_uint32(); // 
 uint32 h32 = bs.read_uint32(); // 
 uint32 h33 = bs.read_uint32(); // 
 uint32 h34 = bs.read_uint32(); // 
 uint32 h35 = bs.read_uint32(); // 
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // debug
 if(debug) {
    ofile << "----------" << endl;
    ofile << "- HEADER -" << endl;
    ofile << "----------" << endl;
    ofile << endl;
    ofile << "h01 = 0x" << hex << h01 << dec << endl;
    ofile << "h02 = 0x" << hex << h02 << dec << endl;
    ofile << "h03 = 0x" << hex << h03 << dec << " - number of joints" << endl;
    ofile << "h04 = 0x" << hex << h04 << dec << " - number of entries at offset h15" << endl;
    ofile << "h05 = 0x" << hex << h05 << dec << " - number of surface names at offset h14" << endl;
    ofile << "h06 = 0x" << hex << h06 << dec << endl;
    ofile << "h07 = 0x" << hex << h07 << dec << " - number of indices in index buffer" << endl;
    ofile << "h08 = 0x" << hex << h08 << dec << endl;
    ofile << "h09 = 0x" << hex << h09 << dec << " - size of vertex buffer" << endl;
    ofile << "h10 = 0x" << hex << h10 << dec << " - " << endl;
    ofile << "h11 = 0x" << hex << h11 << dec << " - number of 0x20 byte entries at offset h13" << endl;
    ofile << "h12 = 0x" << hex << h12 << dec << " - offset to joint data #1 (0x18 bytes per entry)" << endl;
    ofile << "h13 = 0x" << hex << h13 << dec << " - offset to some data" << endl;
    ofile << "h14 = 0x" << hex << h14 << dec << " - offset to material names" << endl;
    ofile << "h15 = 0x" << hex << h15 << dec << " - offset to surface data" << endl;
    ofile << "h16 = 0x" << hex << h16 << dec << " - offset to vertex buffer" << endl;
    ofile << "h17 = 0x" << hex << h17 << dec << " - offset to index buffer" << endl;
    ofile << "h18 = 0x" << hex << h18 << dec << " - EOF marker" << endl;
    ofile << "h19 = " << h19 << " - bounding box" << endl;
    ofile << "h20 = " << h20 << " - bounding box" << endl;
    ofile << "h21 = " << h21 << " - bounding box" << endl;
    ofile << "h22 = " << h22 << " - bounding box" << endl;
    ofile << "h23 = " << h23 << " - bounding box" << endl;
    ofile << "h24 = " << h24 << " - bounding box" << endl;
    ofile << "h25 = " << h25 << " - bounding box" << endl;
    ofile << "h26 = " << h26 << " - bounding box" << endl;
    ofile << "h27 = " << h27 << " - bounding sphere" << endl;
    ofile << "h28 = " << h28 << " - bounding sphere" << endl;
    ofile << "h29 = " << h29 << " - bounding sphere" << endl;
    ofile << "h30 = " << h30 << " - bounding sphere" << endl;
    ofile << "h31 = 0x" << hex << h31 << dec << endl;
    ofile << "h32 = 0x" << hex << h32 << dec << endl;
    ofile << "h33 = 0x" << hex << h33 << dec << endl;
    ofile << "h34 = 0x" << hex << h34 << dec << endl;
    ofile << "h35 = 0x" << hex << h35 << dec << endl;
    ofile << endl;
   }

 //
 // BUILD SKELETON
 //

 // debug
 if(debug) {
    ofile << "-------------" << endl;
    ofile << "- BONE DATA -" << endl;
    ofile << "-------------" << endl;
    ofile << endl;
   }

 // move to bone data
 bs.seek(h12);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // bonedata
 CAPCOM::BONEDATA bd;

 // load bone information
 deque<CAPCOM::BONEINFO> boneinfo;
 for(uint32 i = 0; i < h03; i++) {
     CAPCOM::BONEINFO item;
     item.j01 = bs.read_uint08();
     item.j02 = bs.read_uint08();
     item.j03 = bs.read_uint08();
     item.j04 = bs.read_uint08();
     bs.read_array(&item.j05[0], 5);
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     boneinfo.push_back(item);
     if(debug) {
        ofile << "jnt[0x" << hex << setw(3) << i << dec << "] = ";
        ofile << setw(2) << hex << item.j01 << dec << " - ";
        ofile << setw(2) << hex << item.j02 << dec << " - ";
        ofile << setw(2) << hex << item.j03 << dec << " - ";
        ofile << setw(2) << hex << item.j04 << dec << " - ";
        ofile << item.j05[0] << dec << ", ";
        ofile << item.j05[1] << dec << ", ";
        ofile << item.j05[2] << dec << ", ";
        ofile << item.j05[3] << dec << ", ";
        ofile << item.j05[4] << dec << endl;
       }
    }
 if(debug) ofile << endl;

 // load two sets of bone matrices
 deque<CAPCOM::BONEMATRIX> bmset1;
 deque<CAPCOM::BONEMATRIX> bmset2;
 for(uint32 i = 0; i < h03; i++) {
     CAPCOM::BONEMATRIX item;
     bs.read_array(&item.m[0], 16);
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     cs::math::matrix4x4_transpose(&item.m[0]);
     bmset1.push_back(item);
    }
 for(uint32 i = 0; i < h03; i++) {
     CAPCOM::BONEMATRIX item;
     bs.read_array(&item.m[0], 16);
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     cs::math::matrix4x4_transpose(&item.m[0]);
     bmset2.push_back(item);
    }

 // read 0x100-byte bone map
 CAPCOM::BONEMAP bonemap;
 bonemap.elem = 0x100;
 bonemap.data.reset(new uint16[bonemap.elem]);
 for(uint32 i = 0; i < bonemap.elem; i++) {
     bonemap.data[i] = bs.read_uint08();
     if(bs.fail()) return error("Stream read failure.", __LINE__);
    }
 if(debug) {
    ofile << "BONEMAP" << endl;
    uint32 k = 0;
    for(uint32 i = 0; i < 0x10; i++) {
        for(uint32 j = 0; j < 0x10; j++) {
            ofile << setw(2) << hex << bonemap.data[k] << dec << " ";
            k++;
           }
        ofile << endl;
       }
    ofile << endl;
   }

 // build skeleton
 AMC_SKELETON2 skeleton;
 if(h03) {
    bd.boneinfo = boneinfo;
    bd.m1 = bmset1;
    bd.m2 = bmset2;
    bd.bonemap = bonemap;
    bd.bonemaplist.clear(); // nothing
    if(CreateSkeleton(skeleton, bd)) amc.skllist2.push_back(skeleton);
   }

 //
 // BUILD MATERIALS
 //

 // debug
 if(debug) {
    ofile << "------------------------" << endl;
    ofile << "- MATERIAL IDENTIFIERS -" << endl;
    ofile << "------------------------" << endl;
    ofile << endl;
   }

 // move to unknown data
 bs.seek(h13);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read unknown data
 for(uint32 i = 0; i < h11; i++) {
     uint32 p01 = bs.read_uint32();
     uint32 p02 = bs.read_uint32();
     uint32 p03 = bs.read_uint32();
     uint32 p04 = bs.read_uint32();
     real32 p05 = bs.read_real32();
     real32 p06 = bs.read_real32();
     real32 p07 = bs.read_real32();
     real32 p08 = bs.read_real32();
    if(debug) {
       ofile << "entry[0x" << hex << i << dec << "] = <";
       ofile << "0x" << hex << p01 << dec << ", ";
       ofile << "0x" << hex << p02 << dec << ", ";
       ofile << "0x" << hex << p03 << dec << ", ";
       ofile << "0x" << hex << p04 << dec << ", ";
       ofile << p05 << ", ";
       ofile << p06 << ", ";
       ofile << p07 << ", ";
       ofile << p08 << ">" << endl;
      }
    }
 if(debug) ofile << endl;

 // debug
 if(debug) {
    ofile << "------------------" << endl;
    ofile << "- MATERIAL NAMES -" << endl;
    ofile << "------------------" << endl;
    ofile << endl;
   }

 // move to material names
 bs.seek(h14);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read material names
 for(uint32 i = 0; i < h05; i++) {
     char name[0x80];
     bs.read(&name[0], 0x80);
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     if(debug) {
        ofile << "entry[0x" << hex << i << dec << "] = ";
        ofile << name << endl;
       }
    }
 if(debug) ofile << endl;

 //
 // BUILD MESH
 //

 // debug
 if(debug) {
    ofile << "----------------" << endl;
    ofile << "- SURFACE INFO -" << endl;
    ofile << "----------------" << endl;
    ofile << endl;
   }

 // initialize mesh data
 CAPCOM::MESHDATA md;
 md.mode = ENDIAN_LITTLE;
 md.vbsize = h09;
 md.ibsize = h07*sizeof(uint16);
 md.vbdata.reset(new char[md.vbsize]);
 md.ibdata.reset(new char[md.ibsize]);

 // read vertex buffer
 bs.seek(h16);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);
 bs.read(md.vbdata.get(), md.vbsize);
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // read index buffer
 bs.seek(h17);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);
 bs.read(md.ibdata.get(), md.ibsize);
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // store surface information
 struct SURFACEINFO {
   uint16 p01; // type
   uint16 p02; // number of vertices in vertex buffer
   uint08 p03; // ???
   uint08 p04; // material identifier (0x00, 0x10, 0x20, ..., 0xF0) >> 4
   uint08 p05; // LOD
   uint08 p06; // ???
   uint08 p07; // ???
   uint08 p08; // ???
   uint08 p09; // vertex stride
   uint08 p10; // ???
   uint32 p11; // vertex buffer start index #1
   uint32 p12; // vertex buffer start offset
   uint32 p13; // vertex buffer format
   uint32 p14; // base index
   uint32 p15; // number of indices
   uint32 p16; // vertex buffer start index #2
   uint08 p17; // blend index base???
   uint08 p18; // joint map index???
   uint08 p19; // ???
   uint08 p20; // ???
   uint16 p21; // min index
   uint16 p22; // max index
   uint32 p23; // ???
 };
 deque<SURFACEINFO> sidata;

 // seek surface information
 bs.seek(h15);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read surface information
 for(uint32 i = 0; i < h04; i++)
    {
     SURFACEINFO item;
     item.p01 = bs.read_uint16(); // type
     item.p02 = bs.read_uint16(); // number of vertices in vertex buffer
     item.p03 = bs.read_uint08(); // ???
     item.p04 = bs.read_uint08(); // material identifier
     item.p05 = bs.read_uint08(); // LOD
     item.p06 = bs.read_uint08(); // ???
     item.p07 = bs.read_uint08(); // ???
     item.p08 = bs.read_uint08(); // ???
     item.p09 = bs.read_uint08(); // vertex stride
     item.p10 = bs.read_uint08(); // ???
     item.p11 = bs.read_uint32(); // vertex buffer start index #1
     item.p12 = bs.read_uint32(); // vertex buffer start offset
     item.p13 = bs.read_uint32(); // vertex buffer format
     item.p14 = bs.read_uint32(); // base index
     item.p15 = bs.read_uint32(); // number of indices
     item.p16 = bs.read_uint32(); // vertex buffer start index #2
     item.p17 = bs.read_uint08(); // blend index base
     item.p18 = bs.read_uint08(); // joint map index
     item.p19 = bs.read_uint08(); // ???
     item.p20 = bs.read_uint08(); // ???
     item.p21 = bs.read_uint16(); // min index
     item.p22 = bs.read_uint16(); // max index
     item.p23 = bs.read_uint32(); // ???
     sidata.push_back(item);

     // debug, display everything as integers
     if(debug) {
        ofile << setfill('0');
        ofile << setw(4) << hex << (uint32)item.p01 << dec << " ";
        ofile << setw(4) << hex << (uint32)item.p02 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p03 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p04 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p05 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p06 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p07 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p08 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p09 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p10 << dec << " ";
        ofile << setw(8) << hex << (uint32)item.p11 << dec << " ";
        ofile << setw(8) << hex << (uint32)item.p12 << dec << " ";
        ofile << setw(8) << hex << (uint32)item.p13 << dec << " ";
        ofile << setw(8) << hex << (uint32)item.p14 << dec << " ";
        ofile << setw(8) << hex << (uint32)item.p15 << dec << " ";
        ofile << setw(8) << hex << (uint32)item.p16 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p17 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p18 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p19 << dec << " ";
        ofile << setw(2) << hex << (uint32)item.p20 << dec << " ";
        ofile << setw(4) << hex << (uint32)item.p21 << dec << " ";
        ofile << setw(4) << hex << (uint32)item.p22 << dec << " ";
        ofile << setw(8) << hex << (uint32)item.p23 << dec << " ";
        ofile << "m_" << setw(3) << i << "_ref_0" << " ";
        ofile << "voffset = 0x" << setw(8) << hex << (item.p12 + item.p11*item.p09 + h16) << dec << " ";
        ofile << "vsize = 0x" << setw(8) << hex << (item.p02*item.p09) << dec << " ";
        ofile << endl;
       }
    }
 if(debug) ofile << endl;

 // process surface information
 for(uint32 i = 0; i < h04; i++)
    {
     // initialize mesh information
     CAPCOM::MESHINFO mi;
     mi.index      = i;
     mi.type       = sidata[i].p01;
     mi.vertices   = sidata[i].p02;
     mi.mtl_id     = sidata[i].p03;
     mi.lod        = sidata[i].p05;
     mi.vtx_stride = sidata[i].p09;
     mi.vtx_format = sidata[i].p13;
     mi.vtx_start1 = sidata[i].p11;
     mi.vtx_start2 = sidata[i].p15;
     mi.vtx_offset = sidata[i].p12;
     mi.base_index = sidata[i].p14;
     mi.indices    = sidata[i].p15;
     mi.wmap_index = sidata[i].p17;
     mi.jmap_index = sidata[i].p18;
     mi.min_index  = sidata[i].p21;
     mi.max_index  = sidata[i].p22;
 
     // create mesh data
     AMC_VTXBUFFER vb;
     AMC_IDXBUFFER ib;
     AMC_SURFACE surface;
     if(CreateMeshData(vb, ib, surface, bd, md, mi))
       {
        // material association
        if(has_mrl) {
           uint32 material_index = (sidata[i].p04/16);
           if(material_index < matlist.items.size()) surface.surfmat = material_index;
          }

        // insert data
        amc.vblist.push_back(vb);
        amc.iblist.push_back(ib);
        amc.surfaces.push_back(surface);
       }
     else
       return error("Failed.");
    }

 // save model
 SaveAMC(pathname.c_str(), shrtname.c_str(), amc);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

bool ExtractMODFromPath(LPCTSTR pathname)
{
 // build filename list
 cout << "Searching for .MOD files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".MOD"), pathname)) return true;

 // process ARC files
 bool break_on_errors = true;
 cout << "Processing .MOD files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractMOD(filelist[i].c_str())) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
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
 p1 += TEXT("[PC] Resident Evil Zero HD");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where ARC files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~16 GB free space.\n");
 p3 += TEXT("3. 12 GB for game + 4 GB extraction.\n");
 p3 += TEXT("4. Will likely not work with other Capcom games.\n");

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
 bool doARC = true;
 bool doTEX = true;
 bool doMOD = true;
 bool doDEL = true;

 // questions
 if(doARC) doARC = YesNoBox("Process ARC files?\nSay 'No' if you have already done this before.");
 if(doTEX) doTEX = YesNoBox("Process texture (TEX) files?");
 if(doMOD) doMOD = YesNoBox("Process model (MOD) files?");
 if(doDEL) doDEL = YesNoBox("Delete empty folders, subfolders and other useless files?\nSay 'No' if you don't trust me or if 'c:/Windows/ was your path lol.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack archives
 if(doARC) {
    if(!CAPCOM::PC::ExtractARCFromPath(pathname.c_str()))
       return false;
   }

 // unpack archives
 if(doTEX) {
    if(!CAPCOM::PC::ExtractTEXFromPath(pathname.c_str()))
       return false;
   }

 // model extraction
 if(doMOD) {
    if(!ExtractMODFromPath(pathname.c_str()))
       return false;
   }

 // cleanup phase
 if(doDEL) {
    std::set<STDSTRING> filelist;
    filelist.insert(TEXT("mod"));
    filelist.insert(TEXT("tex"));
    filelist.insert(TEXT("mrl"));
    filelist.insert(TEXT("dds"));
    filelist.insert(TEXT("obj"));
    filelist.insert(TEXT("mtl"));
    filelist.insert(TEXT("smc"));
    DeleteFilesExcludingExtensions(pathname.c_str(), filelist);
    DeleteEmptyFolders(pathname.c_str());
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