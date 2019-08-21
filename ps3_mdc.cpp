#include "xentax.h"
#include "x_math.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "ps3_mdc.h"

#define X_SYSTEM PS3
#define X_GAME   MAHJONG_DREAM_CLUB

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processCAT(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file");

 // read header
 std::deque<std::pair<size_t, size_t>> header;
 for(;;) {
     uint32 p01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     uint32 p02 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     if(p01 == 0xFFFFFFFF && p02 == 0xFFFFFFFF) break;
     if(header.size() > 1000) return error("Unexpected number of CAT sections.");
     header.push_back(std::pair<size_t, size_t>(p01, p02));
    }

 // create directory to store files
 string pakpath = GetPathnameFromFilename(filename);
 pakpath += GetShortFilenameWithoutExtension(filename);
 pakpath += "\\";
 CreateDirectoryA(pakpath.c_str(), NULL);

 // process files
 for(size_t i = 0; i < header.size(); i++)
    {
     // move to data
     ifile.seekg(header[i].first);
     if(ifile.fail()) return error("Seek failure.");
     if(header[i].second == 0) continue;

     // read data
     boost::shared_array<char> data(new char[header[i].second]);
     ifile.read(data.get(), header[i].second);
     if(ifile.fail()) return error("Read failure.");

     // create filename
     stringstream ss;
     ss << pakpath << setfill('0') << setw(3) << i << ".";
     uint32 value = *reinterpret_cast<uint32*>(data.get());
     switch(value) {
       case(0x746D6430) : ss << "tmd"; break;
       case(0x746D6F30) : ss << "tmo"; break;
       case(0xFF000202) : ss << "xpr"; break;
       case(0xFF010102) : ss << "xpr"; break;
       default : ss << "tmp";
      }

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     ofile.write(data.get(), header[i].second);
    }

 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processXPR(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(!(magic == 0x020101FF || magic == 0x020200FF)) return error("Invalid XPR file.");

 // read datasize
 uint32 datasize = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of files
 uint32 numfiles = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // texture information structure
 struct XPRITEM {
  uint32 index; // index (TODO: USE THIS TO DETERMINE OUTPUT FILENAME)
  uint32 offset;
  uint32 filesize;
  uint08 unk01; // format
  uint08 unk02; // number of mipmap levels including top level
  uint16 unk03; // 0x02
  uint32 unk04; // 0xAAE4
  uint16 dx;    // dx
  uint16 dy;    // dy
  uint32 unk05; // 0x10000
  uint32 unk06; // 0x00
  uint32 unk07; // 0x00
 };

 // read file information
 deque<XPRITEM> xprlist;
 for(uint32 i = 0; i < numfiles; i++) {
     XPRITEM item = {
      BE_read_uint32(ifile),
      BE_read_uint32(ifile),
      BE_read_uint32(ifile),
      BE_read_uint08(ifile),
      BE_read_uint08(ifile),
      BE_read_uint16(ifile),
      BE_read_uint32(ifile),
      BE_read_uint16(ifile),
      BE_read_uint16(ifile),
      BE_read_uint32(ifile),
      BE_read_uint32(ifile),
      BE_read_uint32(ifile),
     };
     xprlist.push_back(item);
    }

 // create directory to store files
 string savepath = GetPathnameFromFilename(filename);
 savepath += GetShortFilenameWithoutExtension(filename);
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // read file
 for(uint32 i = 0; i < numfiles; i++)
    {
     // compute bits per pixel
     if(xprlist[i].dx == 0 || xprlist[i].dy == 0) return error("Invalid XPR dimensions.");
     uint32 bpp = 8*xprlist[i].filesize/(xprlist[i].dx*xprlist[i].dy);

     // move to offset
     ifile.seekg(xprlist[i].offset);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     uint32 ofsize = xprlist[i].filesize;
     boost::shared_array<char> ofdata(new char[ofsize]);
     BE_read_array(ifile, ofdata.get(), ofsize);
     if(ifile.fail()) return error("Read failure.");

     // DDS format
     uint32 format = xprlist[i].unk01;
     uint32 mipmap = xprlist[i].unk02 - 1;

     // create DDS header
     DDS_HEADER ddsh;
     if(format == 0x85) CreateUncompressedDDSHeader(xprlist[i].dx, xprlist[i].dy, 0, 0xFF00, 0xFF0000, 0xFF000000, 0xFF, FALSE, &ddsh);
     else if(format == 0xA5) CreateUncompressedDDSHeader(xprlist[i].dx, xprlist[i].dy, 0, 0xFF00, 0xFF0000, 0xFF000000, 0xFF, FALSE, &ddsh);
     else if(format == 134 || format == 166) CreateDXT1Header(xprlist[i].dx, xprlist[i].dy, mipmap, FALSE, &ddsh);
     else if(format == 135 || format == 136 || format == 168) CreateDXT5Header(xprlist[i].dx, xprlist[i].dy, mipmap, FALSE, &ddsh);
     else if(format == 190) CreateUncompressedDDSHeader(xprlist[i].dx, xprlist[i].dy, 0, 0xFF00, 0xFF0000, 0xFF000000, 0xFF, FALSE, &ddsh);
     else return error("Unknown DDS format.");

     // deswizzle
     if(format == 0x85) {
        boost::shared_array<char> copy(new char[ofsize]);
        for(uint32 r = 0; r < xprlist[i].dy; r++) {
            for(uint32 c = 0; c < xprlist[i].dx; c++) {
                uint32 morton = array_to_morton(r, c);
                uint32 copy_position = 4*r*xprlist[i].dx + 4*c;
                uint32 data_position = 4*morton;
                copy[copy_position++] = ofdata[data_position + 0]; // b
                copy[copy_position++] = ofdata[data_position + 1]; // g
                copy[copy_position++] = ofdata[data_position + 2]; // r
                copy[copy_position++] = ofdata[data_position + 3]; // a
               }
           }
        ofdata = copy;
       }

     // save data
     stringstream ofname;
     ofname << savepath << setfill('0') << setw(3) << i << ".dds";
     SaveDDSFile(ofname.str().c_str(), ddsh, ofdata, ofsize);
    }

 // NEW TEXTURE ID TO MAHJONG DREAM CLUB
 // 0x85 (133)
 // 001D2580 00010000 0080 0080
 // filesize = 65536
 // dx = 128
 // dy = 128
 // 4 * dx * dy = 65536 (uncompressed)
 // swizzled

 // NEW TEXTURE ID TO MAHJONG DREAM CLUB
 // 0xA5 (165)
 // 0006DD00 00118000 0200 0230
 // filesize = 1146880
 // dx = 512
 // dy = 560
 // 4 * dx * dy = 1146880 (uncompressed)

 return true;
}

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

static uint32 current_model = 0;

bool processTMD(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("error");

 // filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // variables
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // mark current model
 current_model++;

 // debug informations
 bool debug = true;
 ofstream dfile;
 if(debug) {
    stringstream ss;
    ss << pathname << shrtname << ".txt";
    dfile.open(ss.str().c_str());
   }

 //
 // PROCESS #00
 // INITIALIZE MODEL CONTAINER
 //

 ADVANCEDMODELCONTAINER amc;

 //
 // PROCESS #01
 // READ AND PROCESS FILE HEADER
 //

 // read header
 auto head01 = BE_read_uint32(ifile); // magic
 auto head02 = BE_read_uint32(ifile); // vertex type
 auto head03 = BE_read_uint32(ifile); // unknown
 auto head04 = BE_read_uint32(ifile); // 0x28
 auto head05 = BE_read_real32(ifile);
 auto head06 = BE_read_real32(ifile);
 auto head07 = BE_read_real32(ifile);
 auto head08 = BE_read_real32(ifile);
 auto head09 = BE_read_real32(ifile);
 auto head10 = BE_read_real32(ifile);
 if(debug) dfile << "HEADER" << endl;
 if(debug) dfile << "0x" << hex << head01 << dec << endl;
 if(debug) dfile << "0x" << hex << head02 << dec << endl;
 if(debug) dfile << "0x" << hex << head03 << dec << endl;
 if(debug) dfile << "0x" << hex << head04 << dec << endl;
 if(debug) dfile << head05 << endl;
 if(debug) dfile << head06 << endl;
 if(debug) dfile << head07 << endl;
 if(debug) dfile << head08 << endl;
 if(debug) dfile << head09 << endl;
 if(debug) dfile << head10 << endl;
 if(debug) dfile << endl;

 // validate header
 if(head01 != 0x30646D74) return error("Invalid TMD file.");
 if(head04 != 0x28) return error("Invalid TMD file.");

 // determine vertex size
 uint32 vertexsize = 0;
 uint08 indextype = AMC_UINT16;
 uint08 indexsize = 2;
 switch(head02) {
   case(0x000001A7) : vertexsize = 0x0C; break; // OK
   case(0x000001E7) : vertexsize = 0x18; break; // OK
   case(0x000008E7) : vertexsize = 0x20; break; // OK
   case(0x000009C7) : vertexsize = 0x1C; break; // OK
   case(0x000009E7) : vertexsize = 0x20; break; // OK
   case(0x001D09E7) : vertexsize = 0x20; break; // OK
   case(0x1D0009E7) : vertexsize = 0x20; break; // OK
   default : {
        stringstream msg;
        msg << "Unknown vertex type 0x" << hex << head02 << dec << ".";
        return error(msg.str().c_str());
       }
  }

 if(debug) dfile << "FILE INFORMATION" << std::endl;
 uint32 fileinfo[30];
 for(size_t i = 0; i < 30; i++) fileinfo[i] = BE_read_uint32(ifile);
 if(debug) {
    // 0x28
    dfile << "offset to surface data: " << fileinfo[0] << endl;
    dfile << "number of surface data items: " << fileinfo[1] << endl;
    // 0x30
    dfile << "offset to surface info : " << fileinfo[2] << endl;
    dfile << "number of surface info items: " << fileinfo[3] << endl;
    dfile << "always 0: " << fileinfo[4] << endl;
    dfile << "always 0: " << fileinfo[5] << endl;
    // 0x40
    dfile << "offset to unknown surface info: " << fileinfo[6] << endl;
    dfile << "number of unknown surface info items: " << fileinfo[7] << endl; // each item is 0x84 bytes
    dfile << "offset to material info: " << fileinfo[8] << endl;
    dfile << "number of material info items: " << fileinfo[9] << endl;
    // 0x50
    dfile << "offset to triangle group info: " << fileinfo[10] << endl;
    dfile << "number of triangle group info items: " << fileinfo[11] << endl;
    dfile << "offset to index buffer: " << fileinfo[12] << endl;
    dfile << "number of triangles: " << fileinfo[13] << endl;
    // 0x60
    dfile << "number of vertices: " << fileinfo[14] << endl;
    dfile << "offset to vertex buffer: " << fileinfo[15] << endl;
    dfile << "offset to unknown data: " << fileinfo[16] << endl; // 0x01A7, 0x09A3, 0x09A7, 0x09AF, 0x09BF, 0x09E7
    dfile << "offset to separate UV data: " << fileinfo[17] << endl; // 0x01A7, 0x0905, 0x0925, 0x09A7, 0x09AF, 0x09BF, 0x09E7
    // 0x70
    dfile << "offset to unknown data: " << fileinfo[18] << endl; // 0x09AF, 0x09BF 
    dfile << "offset to unknown data: " << fileinfo[19] << endl; // 0x09BF
    dfile << "offset to unknown data: " << fileinfo[20] << endl; // 0x01A7, 0x0925, 0x09A3, 0x09A7, 0x09AF, 0x09BF, 0x09E7
    dfile << "offset to unknown data: " << fileinfo[21] << endl; // 0x0905, 0x0925, 0x09A3, 0x09A7, 0x09AF, 0x09BF, 0x09E7
    // 0x80
    dfile << "offset to unknown data: " << fileinfo[22] << endl; // 0x0905, 0x0925, 0x09A3, 0x09A7, 0x09AF, 0x09BF, 0x09E7
    dfile << "offset to joint matrix: " << fileinfo[23] << endl;
    dfile << "number of joint matrix items: " << fileinfo[24] << endl;
    dfile << "offset to joint data: " << fileinfo[25] << endl;
    // 0x90
    dfile << "number of joint items: " << fileinfo[26] << endl;
    dfile << "offset to texture data: " << fileinfo[27] << endl;
    dfile << "number of texture items: " << fileinfo[28] << endl;
    dfile << "always 0: " << fileinfo[29] << endl;
    dfile << std::endl;
   }

 //
 // PROCESS #03
 // READ SURFACE GROUP DATA
 //

 struct SURFACEGROUP {
  real32 bbox_xmin;
  real32 bbox_ymin;
  real32 bbox_zmin;
  real32 bbox_xmax;
  real32 bbox_ymax;
  real32 bbox_zmax;
  uint32 index; // 0, 6, 10, 14, 18, 22
  uint16 elem;  // 6, 4,  4,  4,  4,  4 (22 + 4 = 26 texture associations)
  uint16 type;  // 1, 1,  1,  1,  1,  1
 };

 deque<SURFACEGROUP> surfdata;
 if(fileinfo[0])
   {
    // move to surface information
    if(debug) dfile << "SURFACE GROUPS" << endl;
    ifile.seekg(fileinfo[0]);
    if(ifile.fail()) return error("Seek failure.");

    // process surfaces
    uint32 n_groups = fileinfo[1];
    if(n_groups == 0 || n_groups > 1000) return error("Unexpected number of surfaces.");
    if(debug) dfile << "Number of groups = " << n_groups << std::endl;

    // process surfaces
    for(uint32 i = 0; i < n_groups; i++)
       {
        if(debug) dfile << "SURFACE GROUP " << i << endl;
        auto param01 = BE_read_real32(ifile);
        auto param02 = BE_read_real32(ifile);
        auto param03 = BE_read_real32(ifile);
        auto param04 = BE_read_real32(ifile);
        auto param05 = BE_read_real32(ifile);
        auto param06 = BE_read_real32(ifile);
        if(debug) dfile << "box_min_x = " << param01 << endl;
        if(debug) dfile << "box_min_y = " << param02 << endl;
        if(debug) dfile << "box_min_z = " << param03 << endl;
        if(debug) dfile << "box_max_x = " << param04 << endl;
        if(debug) dfile << "box_max_y = " << param05 << endl;
        if(debug) dfile << "box_max_z = " << param06 << endl;
   
        // read index
        auto param07 = BE_read_uint32(ifile);
        if(debug) dfile << "index = " << param07 << endl;
   
        // read other parameters
        auto param08 = BE_read_uint16(ifile);
        auto param09 = BE_read_uint16(ifile);
        if(debug) dfile << "elem = " << param08 << endl;
        if(debug) dfile << "type = " << param09 << endl;
        if(debug) dfile << endl;

        // set surface information
        SURFACEGROUP sg;
        sg.bbox_xmin = param01;
        sg.bbox_ymin = param02;
        sg.bbox_zmin = param03;
        sg.bbox_xmax = param04;
        sg.bbox_ymax = param05;
        sg.bbox_zmax = param06;
        sg.index = param07;
        sg.elem = param08;
        sg.type = param09;
        surfdata.push_back(sg);
       }
   }

 //
 // PROCESS #04
 // READ SURFACE-TEXTURE ASSOCIATION DATA
 //

 // There are 6 section groups for this data
 // There are 6 textures for this data.
 // There are 3 materials for this data.
 // There are 7 unknown surface information items for this data.
 // There are 7 triangle groups for this data.
 // 0x20XX is a material reference index.
 // 0x30XX is a triangle group reference index.
 // 0x40XX is a ??? reference index.
 // 0x2000 0x4000 0x3000 0x4001 0x3001 0x1000
 // 0x2001 0x4002 0x3002 0x1000
 // 0x2001 0x4003 0x3003 0x1000
 // 0x2001 0x4004 0x3004 0x1000
 // 0x2001 0x4005 0x3005 0x1000
 // 0x2002 0x4006 0x3006 0x1000

 map<uint32, uint32> assmap; // K = trigroup index, V = material index
 if(fileinfo[2])
   {
    // move to surface information
    if(debug) dfile << "SURFACE-TEXTURE ASSOCIATION" << endl;
    ifile.seekg(fileinfo[2]);
    if(ifile.fail()) return error("Seek failure.");

    // for ea
    for(size_t i = 0; i < surfdata.size(); i++)
       {
        // read group information
        boost::shared_array<uint16> data(new uint16[surfdata[i].elem]);
        BE_read_array(ifile, data.get(), surfdata[i].elem);
        if(ifile.fail()) return error("Read failure.");
   
        uint16 curr_0x2000 = 0xFFFF;
        uint16 curr_0x3000 = 0xFFFF;
        uint16 curr_0x4000 = 0xFFFF;
   
        // for each item read
        for(size_t j = 0; j < surfdata[i].elem; j++)
           {
            if(data[j] == 0x1000)
              {
               curr_0x2000 = 0xFFFF;
               curr_0x3000 = 0xFFFF;
               curr_0x4000 = 0xFFFF;
               if(debug) dfile << "0x1000" << endl;
              }
            else if((data[j] & 0xFF00) == 0x2000)
              {
               curr_0x2000 = (uint16)(data[j] & 0x00FF);
               if(debug) dfile << "0x" << hex << data[j] << " ";
              }
            else if((data[j] & 0xFF00) == 0x3000)
              {
               curr_0x3000 = (uint16)(data[j] & 0x00FF);
               if(debug) dfile << "0x" << hex << data[j] << " ";
               uint32 M_index = curr_0x2000;
               uint32 G_index = curr_0x3000;
               typedef map<uint32, uint32>::value_type value_type;
               assmap.insert(value_type(G_index, M_index));
              }
            else if((data[j] & 0xFF00) == 0x4000)
              {
               curr_0x4000 = (uint16)(data[j] & 0x00FF);
               if(debug) dfile << "0x" << hex << data[j] << " ";
              }
           }
       }
    if(debug) dfile << dec << endl;
    if(debug) dfile << "Number of material-trigroup associations = " << assmap.size() << endl;
    if(debug) dfile << endl;
   }

 //
 // PROCESS #05
 // READ UNKNOWN SURFACE INFORMATION
 //

 if(fileinfo[6] != 0)
   {
    // move to surface information
    if(debug) dfile << "UNKNOWN SURFACE INFORMATION" << endl;
    ifile.seekg(fileinfo[6]);
    if(ifile.fail()) return error("Seek failure.");
   
    // read data
    uint32 n_items = fileinfo[7];
    if(debug) dfile << "Number of items = " << n_items << endl;
    for(uint32 i = 0; i < n_items; i++) {
        for(size_t j = 0; j < 132; j++) if(debug) dfile << setw(4) << (uint16)BE_read_uint08(ifile) << " ";
        if(debug) dfile << endl;
       }
    if(debug) dfile << endl;
   }

 //
 // PROCESS #06
 // READ MATERIALS
 //

 if(fileinfo[8] != 0)
   {
    // move to surface information
    if(debug) dfile << "MATERIALS" << endl;
    ifile.seekg(fileinfo[8]);
    if(ifile.fail()) return error("Seek failure.");
    
    // read data
    uint32 n_materials = fileinfo[9];
    if(debug) dfile << "Number of materials = " << n_materials << endl;

    for(uint32 i = 0; i < n_materials; i++)
       {
        // read material
        auto p01 = BE_read_uint32(ifile); // 0x05
        auto p02 = BE_read_uint32(ifile); // 0x00
        auto p03 = BE_read_uint32(ifile); // base texture index
        auto p04 = BE_read_real32(ifile); // crap
        auto p05 = BE_read_real32(ifile); // crap
        auto p06 = BE_read_real32(ifile); // crap
        auto p07 = BE_read_real32(ifile); // crap
        auto p08 = BE_read_uint32(ifile); // 0xFFFFFFFF
        auto p09 = BE_read_uint32(ifile); // 0xFFFFFFFF
        auto p10 = BE_read_real32(ifile); // crap
        auto p11 = BE_read_real32(ifile); // crap
        auto p12 = BE_read_real32(ifile); // crap
        auto p13 = BE_read_real32(ifile); // crap
        auto p14 = BE_read_uint32(ifile); // 0xFFFFFFFF
        auto p15 = BE_read_uint32(ifile); // 0xFFFFFFFF
        auto p16 = BE_read_real32(ifile); // crap
        auto p17 = BE_read_real32(ifile); // crap
        auto p18 = BE_read_real32(ifile); // crap
        auto p19 = BE_read_real32(ifile); // std::endl of 3 4xfloats
        auto p20 = BE_read_uint32(ifile); // 0x03
        auto p21 = BE_read_uint32(ifile); // normal map index
        auto p22 = BE_read_real32(ifile); // crap
        auto p23 = BE_read_real32(ifile); // crap
        auto p24 = BE_read_real32(ifile); // crap
        auto p25 = BE_read_real32(ifile); // crap
        auto p26 = BE_read_uint32(ifile); // 0x04
        auto p27 = BE_read_uint32(ifile); // specular map index
        auto p28 = BE_read_real32(ifile); // crap
        auto p29 = BE_read_real32(ifile); // crap
        auto p30 = BE_read_real32(ifile); // crap
        auto p31 = BE_read_real32(ifile); // crap
        auto p32 = BE_read_uint32(ifile); // 0x01
        auto p33 = BE_read_uint32(ifile); // 0x00
        auto p34 = BE_read_uint32(ifile); // 0x01
        auto p35 = BE_read_uint32(ifile); // 0x01
        auto p36 = BE_read_uint32(ifile); // 0x01
        auto p37 = BE_read_real32(ifile); // crap
        auto p38 = BE_read_real32(ifile); // crap
        auto p39 = BE_read_real32(ifile); // crap
        auto p40 = BE_read_real32(ifile); // crap
        auto p41 = BE_read_real32(ifile); // crap
        auto p42 = BE_read_real32(ifile); // crap
        auto p43 = BE_read_real32(ifile); // crap
        auto p44 = BE_read_real32(ifile); // crap
        auto p45 = BE_read_real32(ifile); // crap
        auto p46 = BE_read_real32(ifile); // crap
        auto p47 = BE_read_real32(ifile); // crap
        auto p48 = BE_read_real32(ifile); // crap
        auto p49 = BE_read_real32(ifile); // crap
        auto p50 = BE_read_real32(ifile); // crap
        if(debug) dfile << p01 << " ";
        if(debug) dfile << p02 << " ";
        if(debug) dfile << p03 << " ";
        if(debug) dfile << p08 << " ";
        if(debug) dfile << p09 << " ";
        if(debug) dfile << p14 << " ";
        if(debug) dfile << p15 << " ";
        if(debug) dfile << p20 << " ";
        if(debug) dfile << p21 << " ";
        if(debug) dfile << p26 << " ";
        if(debug) dfile << p27 << " ";
        if(debug) dfile << p32 << " ";
        if(debug) dfile << p33 << " ";
        if(debug) dfile << p34 << " ";
        if(debug) dfile << p35 << " ";
        if(debug) dfile << p36 << " ";
        if(debug) dfile << endl;

        // build material name
        stringstream name;
        name << "material_" << setfill('0') << setw(3) << i;

        // create material
        AMC_SURFMAT mat;
        mat.name = name.str();
        mat.twoside = false;
        mat.basemap = (p01 == 0xFFFFFFFF ? AMC_INVALID_TEXTURE : p03);
        mat.normmap = (p20 == 0xFFFFFFFF ? AMC_INVALID_TEXTURE : p21);
        mat.specmap = (p26 == 0xFFFFFFFF ? AMC_INVALID_TEXTURE : p27);
        mat.tranmap = AMC_INVALID_TEXTURE;
        mat.bumpmap = AMC_INVALID_TEXTURE;
        mat.resmap1 = AMC_INVALID_TEXTURE;
        mat.resmap2 = AMC_INVALID_TEXTURE;
        mat.resmap3 = AMC_INVALID_TEXTURE;
        mat.resmap4 = AMC_INVALID_TEXTURE;

        // save material
        amc.surfmats.push_back(mat);
       }
    if(debug) dfile << endl;
   }

 //
 // PROCESS #07
 // READ TRIANGLE GROUP DATA
 //

 struct TRILISTINFO {
  uint32 triangles;
  uint32 offset;
 };

 deque<TRILISTINFO> trilist_data;
 if(fileinfo[10])
   {
    // move to triangle information
    if(debug) dfile << "TRIANGLE GROUPS" << endl;
    ifile.seekg(fileinfo[10]);
    if(ifile.fail()) return error("Seek failure.");

    // number of groups
    uint32 n_groups = fileinfo[11];
    if(n_groups == 0) return error("Invalid number of triangle groups.");

    // read triangle information
    for(uint32 i = 0; i < n_groups; i++) {
        uint16 p1 = BE_read_uint16(ifile); // number of triangles
        uint16 p2 = BE_read_uint16(ifile); // always 0x0508
        uint32 p3 = BE_read_uint32(ifile); // triangle offset
        TRILISTINFO info = { p1, p3 };
        trilist_data.push_back(info);
        if(debug) dfile << "(offset, triangles) = [" << i << "] = (" << info.offset << ", " << info.triangles << ")" << endl;
       }

    if(debug) dfile << endl;
   }

 //
 // PROCESS #08
 // READ INDEX BUFFER
 //

 // move to index buffer
 if(debug) dfile << "INDEX BUFFER DATA" << std::endl;
 ifile.seekg(fileinfo[12]);
 if(ifile.fail()) return error("Seek failure.");

 // number of triangles
 uint32 n_triangles = fileinfo[13];
 if(n_triangles == 0) return error("Invalid number of triangles.");
 if(debug) dfile << "Number of triangles: 0x" << hex << n_triangles << dec << "." << endl;

 // create index buffer
 AMC_IDXBUFFER ibuffer;
 ibuffer.format = indextype;
 ibuffer.type = AMC_TRIANGLES;
 ibuffer.name = "ibuffer";
 ibuffer.elem = n_triangles * 3;

 // create index buffer data
 if(indextype == AMC_UINT16) ibuffer.data.reset(new char[ibuffer.elem * 2]);
 else if(indextype == AMC_UINT32) ibuffer.data.reset(new char[ibuffer.elem * 4]);
 else return error("Invalid index buffer type.");

 // read index buffer data
 uint32 n_indices = n_triangles * 3;
 uint32 n_bytes = n_indices*indexsize;
 boost::shared_array<char> ib_data(new char[n_bytes]);
 if(indextype == AMC_UINT16) BE_read_array(ifile, reinterpret_cast<uint16*>(ibuffer.data.get()), n_indices);
 else BE_read_array(ifile, reinterpret_cast<uint32*>(ibuffer.data.get()), n_indices);
 if(ifile.fail()) return error("Read failure.");

 // debug information
 if(debug) dfile << std::endl;

 //
 // PROCESS #09
 // READ VERTEX BUFFER
 //

 // move to vertex buffer
 if(debug) dfile << "VERTEX BUFFER DATA" << std::endl;
 ifile.seekg(fileinfo[15]);
 if(ifile.fail()) return error("Seek failure.");

 // number of vertices
 uint32 n_vertices = fileinfo[14];
 if(n_vertices == 0) return error("Invalid number of vertices.");

 // create vertex buffer
 AMC_VTXBUFFER vbuffer;
 vbuffer.flags = AMC_VERTEX_POSITION;
 vbuffer.name = "vbuffer";
 vbuffer.elem = n_vertices;
 vbuffer.data.reset(new AMC_VERTEX[n_vertices]);

 if(head02 == 0x000001E7) {
    vbuffer.flags |= AMC_VERTEX_UV;
   }
 if(head02 == 0x000008E7) {
    vbuffer.flags |= AMC_VERTEX_UV;
    vbuffer.flags |= AMC_VERTEX_WEIGHTS;
    vbuffer.flags |= AMC_VERTEX_BONEIDX;
   }
 else if(head02 == 0x000009C7) {
    vbuffer.flags |= AMC_VERTEX_UV;
    vbuffer.flags |= AMC_VERTEX_WEIGHTS;
    vbuffer.flags |= AMC_VERTEX_BONEIDX;
   }
 else if(head02 == 0x000009E7) {
    vbuffer.flags |= AMC_VERTEX_UV;
    vbuffer.flags |= AMC_VERTEX_WEIGHTS;
    vbuffer.flags |= AMC_VERTEX_BONEIDX;
   }
 else if(head02 == 0x001D09E7) {
    vbuffer.flags |= AMC_VERTEX_UV;
    vbuffer.flags |= AMC_VERTEX_WEIGHTS;
    vbuffer.flags |= AMC_VERTEX_BONEIDX;
   }
 else if(head02 == 0x1D0009E7) {
    vbuffer.flags |= AMC_VERTEX_UV;
    vbuffer.flags |= AMC_VERTEX_WEIGHTS;
    vbuffer.flags |= AMC_VERTEX_BONEIDX;
   }

 // read vertices
 for(size_t i = 0; i < n_vertices; i++)
    {
     // initialize weights
     for(size_t j = 0; j < 8; j++) {
         vbuffer.data[i].wv[j] = 0.0f;
         vbuffer.data[i].wi[j] = AMC_INVALID_VERTEX_WMAP_INDEX;
        }

     // OK
     if(head02 == 0x000001A7)
       {
        return error("0x000001A7");
       }
     // OK
     else if(head02 == 0x000001E7)
       {
        // position
        vbuffer.data[i].vx = BE_read_real32(ifile);
        vbuffer.data[i].vy = BE_read_real32(ifile);
        vbuffer.data[i].vz = BE_read_real32(ifile);

        // normals (don't know how they are encoded)
        BE_read_uint08(ifile);
        vbuffer.data[i].nx = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].ny = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].nz = (BE_read_uint08(ifile) - 127.0f)/128.0f;

        // unknown
        BE_read_sint08(ifile);
        BE_read_sint08(ifile);
        BE_read_sint08(ifile);
        BE_read_sint08(ifile);

        // texture coordinates
        vbuffer.data[i].tu = BE_read_sint16(ifile)/1023.0f;
        vbuffer.data[i].tv = BE_read_sint16(ifile)/1023.0f;
       }
     // OK
     else if(head02 == 0x000008E7)
       {
        // position
        vbuffer.data[i].vx = BE_read_real32(ifile);
        vbuffer.data[i].vy = BE_read_real32(ifile);
        vbuffer.data[i].vz = BE_read_real32(ifile);

        // blend weights
        vbuffer.data[i].wv[0] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[1] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[2] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[3] = BE_read_uint08(ifile)/255.0f;

        // blend indices
        vbuffer.data[i].wi[0] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[1] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[2] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[3] = BE_read_uint08(ifile);

        // normals (don't know how they are encoded)
        BE_read_uint08(ifile);
        vbuffer.data[i].nx = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].ny = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].nz = (BE_read_uint08(ifile) - 127.0f)/128.0f;

        // 0xFFFFFFFF
        BE_read_uint32(ifile);

        // texture coordinates
        vbuffer.data[i].tu = BE_read_sint16(ifile)/1023.0f;
        vbuffer.data[i].tv = BE_read_sint16(ifile)/1023.0f;
       }
     // OK
     else if(head02 == 0x000009C7)
       {
        // position
        vbuffer.data[i].vx = BE_read_real32(ifile);
        vbuffer.data[i].vy = BE_read_real32(ifile);
        vbuffer.data[i].vz = BE_read_real32(ifile);

        // blend weights
        vbuffer.data[i].wv[0] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[1] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[2] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[3] = BE_read_uint08(ifile)/255.0f;

        // blend indices
        vbuffer.data[i].wi[0] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[1] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[2] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[3] = BE_read_uint08(ifile);

        // normals (don't know how they are encoded)
        BE_read_uint08(ifile);
        vbuffer.data[i].nx = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].ny = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].nz = (BE_read_uint08(ifile) - 127.0f)/128.0f;

        // texture coordinates
        vbuffer.data[i].tu = BE_read_sint16(ifile)/1023.0f;
        vbuffer.data[i].tv = BE_read_sint16(ifile)/1023.0f;
       }
     // OK
     else if(head02 == 0x000009E7)
       {
        // position
        vbuffer.data[i].vx = BE_read_real32(ifile);
        vbuffer.data[i].vy = BE_read_real32(ifile);
        vbuffer.data[i].vz = BE_read_real32(ifile);

        // blend weights
        vbuffer.data[i].wv[0] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[1] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[2] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[3] = BE_read_uint08(ifile)/255.0f;

        // blend indices
        vbuffer.data[i].wi[0] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[1] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[2] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[3] = BE_read_uint08(ifile);

        // normals (don't know how they are encoded)
        BE_read_uint08(ifile);
        vbuffer.data[i].nx = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].ny = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].nz = (BE_read_uint08(ifile) - 127.0f)/128.0f;

        // 0xFFFFFFFF
        BE_read_uint32(ifile);

        // texture coordinates
        vbuffer.data[i].tu = BE_read_sint16(ifile)/1023.0f;
        vbuffer.data[i].tv = BE_read_sint16(ifile)/1023.0f;
       }
     // NEW!!!
     else if(head02 == 0x001D09E7) // same as above?
       {
        // position
        vbuffer.data[i].vx = BE_read_real32(ifile);
        vbuffer.data[i].vy = BE_read_real32(ifile);
        vbuffer.data[i].vz = BE_read_real32(ifile);

        // blend weights
        vbuffer.data[i].wv[0] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[1] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[2] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[3] = BE_read_uint08(ifile)/255.0f;

        // blend indices
        vbuffer.data[i].wi[0] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[1] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[2] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[3] = BE_read_uint08(ifile);

        // normals (don't know how they are encoded)
        BE_read_uint08(ifile);
        vbuffer.data[i].nx = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].ny = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].nz = (BE_read_uint08(ifile) - 127.0f)/128.0f;

        // 0xFFFFFFFF
        BE_read_uint32(ifile);

        // texture coordinates
        vbuffer.data[i].tu = BE_read_sint16(ifile)/1023.0f;
        vbuffer.data[i].tv = BE_read_sint16(ifile)/1023.0f;
       }
     // OK
     else if(head02 == 0x1D0009E7)
       {
        // position
        vbuffer.data[i].vx = BE_read_real32(ifile);
        vbuffer.data[i].vy = BE_read_real32(ifile);
        vbuffer.data[i].vz = BE_read_real32(ifile);

        // blend weights
        vbuffer.data[i].wv[0] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[1] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[2] = BE_read_uint08(ifile)/255.0f;
        vbuffer.data[i].wv[3] = BE_read_uint08(ifile)/255.0f;

        // blend indices
        vbuffer.data[i].wi[0] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[1] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[2] = BE_read_uint08(ifile);
        vbuffer.data[i].wi[3] = BE_read_uint08(ifile);

        // normals (don't know how they are encoded)
        BE_read_uint08(ifile);
        vbuffer.data[i].nx = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].ny = (BE_read_uint08(ifile) - 127.0f)/128.0f;
        vbuffer.data[i].nz = (BE_read_uint08(ifile) - 127.0f)/128.0f;

        // 0xFFFFFFFF
        BE_read_uint32(ifile);

        // texture coordinates
        vbuffer.data[i].tu = BE_read_sint16(ifile)/1023.0f;
        vbuffer.data[i].tv = BE_read_sint16(ifile)/1023.0f;
       }
    }

 //
 // PROCESS #10
 // READ SEPARATE UV DATA
 //

 if(head02 == 0x000009C7)
   {
    // move to offset
    if(debug) dfile << "SEPARATE UV DATA" << std::endl;
    ifile.seekg(fileinfo[17]);
    if(ifile.fail()) return error("Seek failure.");

    // set UV
    // vbuffer.flags |= AMC_VERTEX_UV;

    // read UVs
    //for(size_t i = 0; i < n_vertices; i++) {
    //    auto v1 = BE_read_sint16(ifile);
    //    auto v2 = BE_read_sint16(ifile);
    //    vbuffer.data[i].tu =  v1/1023.0f;
    //    vbuffer.data[i].tv = -v2/1023.0f;
    //   }

    if(debug) dfile << std::endl;
   }

 //
 // PROCESS #11
 // CONSTRUCT INPUT ASSEMBLY
 //

 if(fileinfo[10])
   {
    // number of groups
    uint32 n_groups = fileinfo[11];
    if(n_groups == 0) return error("Invalid number of triangle groups.");

    // read triangle information
    for(uint32 i = 0; i < n_groups; i++)
       {
        TRILISTINFO info = trilist_data[i];

        // construct name
        stringstream ss;
        ss << "m" << setfill('0') << setw(4) << hex << current_model << dec;
        ss << "s" << setfill('0') << setw(3) << i;

        // create buffer reference
        AMC_REFERENCE ref;
        ref.vb_index = 0; // vbuffer index
        ref.vb_start = 0; // vbuffer offset
        ref.vb_width = vbuffer.elem; // number of vertices
        ref.ib_index = 0; // ibuffer index
        ref.ib_start = info.offset * 3; // ibuffer offset
        ref.ib_width = info.triangles * 3; // number of indices

        // group index -> material index
        uint32 G_index = i;
        uint32 M_index = AMC_INVALID_SURFMAT;
        map<uint32, uint32>::iterator iter = assmap.find(G_index);
        if(iter == assmap.end()) return error("Failed to find triangle list group index");
        else M_index = iter->second;

        // create surface and add reference
        AMC_SURFACE surface;
        surface.name = ss.str();
        surface.surfmat = M_index;
        surface.refs.push_back(ref);

        // insert surface
        amc.surfaces.push_back(surface);
       }
   }

 //
 // PROCESS #12
 // READ BONE MATRICES
 //

 // process bones
 if(fileinfo[23] && (fileinfo[26] > 1))
   {
    // move to bone matrices
    if(debug) dfile << "BONE MATRICES" << std::endl;
    ifile.seekg(fileinfo[23]);
    if(ifile.fail()) return error("Seek failure.");

    // create skeleton name
    stringstream ss;
    ss << "m" << setfill('0') << setw(4) << hex << current_model << dec;
    ss << "skeleton";

    // data to save
    std::deque<boost::shared_array<float>> mlist;
    AMC_SKELETON skel;
    skel.name = ss.str();
    skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX;

    // read matrices
    uint32 n_bones = fileinfo[24];
    if(debug) dfile << "number of bones = " << n_bones << std::endl;
    for(size_t i = 0; i < n_bones; i++) {
        boost::shared_array<float> m(new real32[16]);
        m[ 0] = BE_read_real32(ifile);
        m[ 1] = BE_read_real32(ifile);
        m[ 2] = BE_read_real32(ifile);
        m[ 3] = BE_read_real32(ifile);
        m[ 4] = BE_read_real32(ifile);
        m[ 5] = BE_read_real32(ifile);
        m[ 6] = BE_read_real32(ifile);
        m[ 7] = BE_read_real32(ifile);
        m[ 8] = BE_read_real32(ifile);
        m[ 9] = BE_read_real32(ifile);
        m[10] = BE_read_real32(ifile);
        m[11] = BE_read_real32(ifile);
        m[12] = BE_read_real32(ifile);
        m[13] = BE_read_real32(ifile);
        m[14] = BE_read_real32(ifile);
        m[15] = BE_read_real32(ifile);
        matrix4x4_transpose(&m[0]);
        mlist.push_back(m);
        if(debug) dfile << m[ 0] << " " << m[ 1] << " " << m[ 2] << " " << m[ 3] << std::endl;
        if(debug) dfile << m[ 4] << " " << m[ 5] << " " << m[ 6] << " " << m[ 7] << std::endl;
        if(debug) dfile << m[ 8] << " " << m[ 9] << " " << m[10] << " " << m[11] << std::endl;
        if(debug) dfile << m[12] << " " << m[13] << " " << m[14] << " " << m[15] << std::endl;
        if(debug) dfile << std::endl;
       }

    // move to bone information
    if(debug) dfile << "BONES" << std::endl;
    ifile.seekg(fileinfo[25]);
    if(ifile.fail()) return error("Seek failure.");
    if(n_bones != fileinfo[26]) return error("Number of joints do not match.");

    // read bone information
    for(size_t i = 0; i < n_bones; i++)
       {
        // initialize joint
        AMC_JOINT joint;
        joint.parent = AMC_INVALID_JOINT;

        // unknown
        BE_read_real32(ifile);

        // joint name
        std::stringstream jnss;
        jnss << "m" << setfill('0') << setw(4) << hex << current_model << dec;
        jnss << "j" << std::setfill('0') << std::setw(3) << i;
        joint.name = jnss.str();

        // joint matrix
        for(uint32 j = 0; j < 16; j++) joint.m_rel[j] = mlist[i][j];

        // joint relative positions
        real32 x = BE_read_real32(ifile);
        real32 y = BE_read_real32(ifile);
        real32 z = BE_read_real32(ifile);
        joint.p_rel[0] = -mlist[i][0x03]; // x;
        joint.p_rel[1] = -mlist[i][0x07]; // y;
        joint.p_rel[2] = -mlist[i][0x0B]; // z;
        joint.p_rel[3] = 1.0f;

        // set parent
        uint32 parent = BE_read_uint32(ifile);
        uint32 xxxxxx = BE_read_uint32(ifile);
        joint.parent = parent;
   
        // insert joint
        if(!InsertJoint(skel, joint))
           return false;

        // debug information
        if(debug) dfile << "index = " << i << std::endl;   
        if(debug) dfile << "x = " << x << std::endl;
        if(debug) dfile << "y = " << y << std::endl;
        if(debug) dfile << "z = " << z << std::endl;
        if(debug) dfile << "parent = " << parent << std::endl;
        if(debug) dfile << "xxxxxx = " << xxxxxx << std::endl;
        if(debug) dfile << std::endl;
       }

    amc.skllist.push_back(skel);
   }

 //
 // PROCESS #13
 // READ TEXTURES
 //

 if(fileinfo[27])
   {
    // move to texture information
    if(debug) dfile << "TEXTURES" << std::endl;
    ifile.seekg(fileinfo[27]);
    if(ifile.fail()) return error("Seek failure.");

    // number of textures
    uint32 n_textures = fileinfo[28];
    if(n_textures > 1000) return error("Invalid number of textures.");

    // create texture list
    deque<AMC_IMAGEFILE> imagelist;
    imagelist.resize(n_textures);

    // read texture information
    for(uint32 i = 0; i < n_textures; i++)
       {
        uint32 param1 = BE_read_uint32(ifile); // index
        uint32 param2 = BE_read_uint32(ifile); // 0
        uint16 param3 = BE_read_uint16(ifile); // 0x0001
        uint16 param4 = BE_read_uint16(ifile); // dx
        uint16 param5 = BE_read_uint16(ifile); // dy
        uint16 param6 = BE_read_uint16(ifile); // 0x2000 | 0xFF00
        if(debug) dfile << "param1 = " << param1 << std::endl;
        if(debug) dfile << "param2 = " << param2 << std::endl;
        if(debug) dfile << "param3 = " << param3 << std::endl;
        if(debug) dfile << "param4 = " << param4 << std::endl;
        if(debug) dfile << "param5 = " << param5 << std::endl;
        if(debug) dfile << "param6 = " << param6 << std::endl;
        if(debug) dfile << std::endl;
   
        // texture filename
        stringstream fn;
        fn << setfill('0') << setw(3) << param1 << ".dds";
   
        // insert texture
        AMC_IMAGEFILE file;
        file.filename = fn.str();
        imagelist[param1] = file;
       }

    // add image list
    amc.iflist = imagelist;
    if(debug) dfile << std::endl;
   }

 // insert buffers
 amc.vblist.push_back(vbuffer);
 amc.iblist.push_back(ibuffer);

 // save geometry
 return SaveLWO(pathname.c_str(), shrtname.c_str(), amc);
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

bool extract(const char* pathname)
{
 // set pathname
 using namespace std;
 std::string pname(pathname);
 if(!pname.length()) pname = GetModulePathname();

 // variables
 bool doCAT = true;
 bool doXPR = true;
 bool doTMD = true;

 // process archive
 cout << "STAGE 1" << endl;
 if(doCAT) {
    cout << "Processing .CAT files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".CAT", pname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processCAT(filelist[i])) return false;
       }
    cout << endl;
   }

 // process textures
 cout << "STAGE 2" << endl;
 if(doXPR) {
    cout << "Processing .XPR files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".XPR", pname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXPR(filelist[i])) return false;
       }
    cout << endl;
   }

 // process models
 cout << "STAGE 3" << endl;
 if(doTMD) {
    cout << "Processing .TMD files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".TMD", pname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTMD(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

}};

