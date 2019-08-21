#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_dds.h"
#include "x_smc.h"
#include "x_lwo.h"
#include "ps3_touchshot.h"

#define X_SYSTEM PS3
#define X_GAME   TOUCHSHOT

namespace X_SYSTEM { namespace X_GAME {

class extractor {
 private :
  std::string pathname;
 private :
  bool processCAT(const std::string& filename);
  bool processCAN(const std::string& filename);
  bool processGTF(const std::string& filename);
  bool processTMD(const std::string& filename);
 public :
  bool extract(void);
 public :
  extractor(const char* pn) : pathname(pn) {}
 ~extractor() {}
};

};};

namespace X_SYSTEM { namespace X_GAME {

bool extractor::extract(void)
{
 using namespace std;
 bool doCAT = true;
 bool doCAN = true;
 bool doGTF = true;
 bool doTMD = true;

 if(doCAT) {
    cout << "STAGE 1" << std::endl;
    cout << "Processing CAT files..." << std::endl;
    deque<string> filelist;
    BuildFilenameList(filelist, "cat", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processCAT(filelist[i])) return false;
       }
    std::cout << std::endl;
   }

 if(doCAN) {
    cout << "STAGE 2" << std::endl;
    cout << "Processing CAN files..." << std::endl;
    deque<string> filelist;
    BuildFilenameList(filelist, "can", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processCAN(filelist[i])) return false;
       }
    std::cout << std::endl;
   }

 if(doGTF) {
    cout << "STAGE 3" << std::endl;
    cout << "Processing GTF files..." << std::endl;
    deque<string> filelist;
    BuildFilenameList(filelist, "gtf", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processGTF(filelist[i])) return false;
       }
    std::cout << std::endl;
   }

 if(doTMD) {
    cout << "STAGE 4" << std::endl;
    cout << "Processing TMD files..." << std::endl;
    deque<string> filelist;
    BuildFilenameList(filelist, "tmd", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        std::cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
        if(!processTMD(filelist[i])) return false;
       }
    std::cout << std::endl;
   }

 return true;
}

bool extractor::processCAT(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("error");

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
       case(0xFF000202) : ss << "gtf"; break;
       case(0xFF010102) : ss << "gtf"; break;
       default : ss << "tmp";
      }

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     ofile.write(data.get(), header[i].second);
    }

 return true;
}

bool extractor::processCAN(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("error");

 // filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read header
 struct triple { size_t offset; size_t size; string name; };
 std::deque<triple> header;
 size_t position = 0;
 for(;;)
    {
     // seek position
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read item
     uint32 p01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     uint32 p02 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     if(p01 == 0xFFFFFFFF && p02 == 0xFFFFFFFF) break;

     // name follows
     bool is_name = false;
     uint32 namelen = 0;
     if(p01 < p02) {
        if((p02 - p01) == 0x10) { namelen = 0x10; is_name = true; }
        else if((p02 - p01) == 0x20) { namelen = 0x20; is_name = true; }
        else if((p02 - p01) == 0x30) { namelen = 0x30; is_name = true; }
        else if(p02 == 0xFFFFFFFF) { namelen = filesize - p01; is_name = true; }
       }

     // extract name
     if(is_name) {
        ifile.seekg(p01);
        char buffer[1024];
        read_string(ifile, buffer, 1024);
        header.back().name = buffer;
       }
     else {
        triple item;
        item.offset = p01;
        item.size = p02;
        header.push_back(item);
       }

     // move position
     if(is_name) position += 4;
     else position += 8;
    }

 // create directory to store files
 string sendpath = GetPathnameFromFilename(filename);
 sendpath += GetShortFilenameWithoutExtension(filename);
 sendpath += "\\";
 CreateDirectoryA(sendpath.c_str(), NULL);

 // process files
 for(size_t i = 0; i < header.size(); i++)
    {
     // move to data
     ifile.seekg(header[i].offset);
     if(ifile.fail()) return error("Seek failure.");
     if(header[i].size == 0) continue;

     // read data
     boost::shared_array<char> data(new char[header[i].size]);
     ifile.read(data.get(), header[i].size);
     if(ifile.fail()) return error("Read failure.");

     // create filename
     stringstream ss;
     if(header[i].name.length())
        ss << sendpath << header[i].name;
     else {
        ss << sendpath << setfill('0') << setw(3) << i << ".";
        uint32 value = *reinterpret_cast<uint32*>(data.get());
        switch(value) {
          case(0x746D6430) : ss << "tmd"; break;
          case(0x746D6F30) : ss << "tmo"; break;
          case(0xFF000202) : ss << "gtf"; break;
          case(0xFF010102) : ss << "gtf"; break;
          default : ss << "tmp";
         }
       }

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     ofile.write(data.get(), header[i].size);
    }

 return true;
}

bool extractor::processGTF(const std::string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("error");

 // filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // 0x020101FF (magic)
 // 0x000FD200 (filesize) 1036800
 // 0x00000001 (number of files)
 // 0x00000000

 // 0x020101FF
 // 00000080 000FD200 A6-01-0200 0000AAE4 0780-0438 00010000 00000F00 00000000 00000000 (x*y/2 = filesize)
 // usually x*y*4 = filesize
 // so filesize is shrunk by 8!
 // so instead of 32 bits per pixel this is 4 bits per pixel!

 // 0x020200FF
 // 00000100 00080000 86-01-0200 0000AAE4 0400-0400 00010000 00000000 00000000 00000001
 // 00080100 00010000 88-01-0200 0000AAE4 0100-0100 00010000 00000000 00000000 00000002
 // 00090100 00000800 86-01-0200 0000AAE4 0040-0040 00010000 00000000 00000000 00000003
 // 00090900 00100000 88-01-0200 0000AAE4 0400-0400 00010000 00000000 00000000 00000004
 // 00190900 00008000 86-01-0200 0000AAE4 0100-0100 00010000 00000000 00000000 00000005
 // 00198900 00000800 86-01-0200 0000AAE4 0040-0040 00010000 00000000 00000000 00000000

 struct GTFITEM {
  uint32 offset;
  uint32 filesize;
  uint08 unk01;
  uint08 unk02; // number of mipmap levels including top level
  uint16 unk03;
  uint32 unk04; // 0xAAE4
  uint16 dx;
  uint16 dy;
  uint32 unk05; // 0x10000
  uint32 unk06; // 0x00
  uint32 unk07; // 0x00
  uint32 index;
 };

 // read header
 uint32 magic = BE_read_uint32(ifile);
 uint32 datasize = BE_read_uint32(ifile);
 uint32 numfiles = BE_read_uint32(ifile);
 BE_read_uint32(ifile);

 // read file information
 deque<GTFITEM> gtflist;
 for(uint32 i = 0; i < numfiles; i++) {
     GTFITEM item = {
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
      BE_read_uint32(ifile),
     };
     gtflist.push_back(item);
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
     if(gtflist[i].dx == 0 || gtflist[i].dy == 0) return error("Invalid image dimensions.");
     uint32 bpp = 8*gtflist[i].filesize/(gtflist[i].dx*gtflist[i].dy);

     // move to offset
     ifile.seekg(gtflist[i].offset);
     if(ifile.fail()) { error("Seek failure."); continue; }

     // read data
     uint32 ofsize = gtflist[i].filesize;
     boost::shared_array<char> ofdata(new char[ofsize]);
     BE_read_array(ifile, ofdata.get(), ofsize);
     if(ifile.fail()) { error("Read failure."); continue; }

     // DDS format
     uint32 format = gtflist[i].unk01;
     uint32 mipmap = gtflist[i].unk02 - 1;

     // create DDS header
     DDS_HEADER ddsh;
     if(format == 134 || format == 166) CreateDXT1Header(gtflist[i].dx, gtflist[i].dy, mipmap, FALSE, &ddsh);
     else if(format == 135 || format == 136 || format == 168) CreateDXT5Header(gtflist[i].dx, gtflist[i].dy, mipmap, FALSE, &ddsh);
     else if(format == 190) CreateUncompressedDDSHeader(gtflist[i].dx, gtflist[i].dy, 0, 0xFF00, 0xFF0000, 0xFF000000, 0xFF, FALSE, &ddsh);
     else return error("Unknown DDS format.");

     // save data
     stringstream ofname;
     ofname << savepath << setfill('0') << setw(3) << i << ".dds";
     SaveDDSFile(ofname.str().c_str(), ddsh, ofdata, ofsize);
    }

 return true;
}

bool extractor::processTMD(const std::string& filename)
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
 string fn_path = GetPathnameFromFilename(filename);
 string fn_name = GetShortFilenameWithoutExtension(filename);

 // enable debug mode
 bool debug = true;
 ofstream dfile;
 if(debug) {
    stringstream ss;
    ss << fn_path << fn_name << ".txt";
    dfile.open(ss.str().c_str());
   }

 //
 // PROCESS #00
 // INITIALIZE MODEL CONTAINER
 //

 SIMPLEMODELCONTAINER1 smc;
 smc.vbuffer.flags = 0;
 smc.vbuffer.name = "mesh";
 smc.vbuffer.elem = 0;

 //
 // PROCESS #01
 // READ AND PROCESS FILE HEADER
 //

 // read header
 auto head01 = BE_read_uint32(ifile);
 auto head02 = BE_read_uint32(ifile);
 auto head03 = BE_read_uint32(ifile);
 auto head04 = BE_read_uint32(ifile);
 auto head05 = BE_read_real32(ifile);
 auto head06 = BE_read_real32(ifile);
 auto head07 = BE_read_real32(ifile);
 auto head08 = BE_read_real32(ifile);
 auto head09 = BE_read_real32(ifile);
 auto head10 = BE_read_real32(ifile);
 if(debug) dfile << "HEADER" << endl;
 if(debug) dfile << head01 << endl;
 if(debug) dfile << head02 << endl;
 if(debug) dfile << head03 << endl;
 if(debug) dfile << head04 << endl;
 if(debug) dfile << head05 << endl;
 if(debug) dfile << head06 << endl;
 if(debug) dfile << head07 << endl;
 if(debug) dfile << head08 << endl;
 if(debug) dfile << head09 << endl;
 if(debug) dfile << head10 << endl;
 if(debug) dfile << endl;

 // validate header
 if(head01 != 0x30646D74) return error("Invalid MDL file.");
 if(head04 != 0x28) return error("Invalid MDL file.");

 // determine vertex size
 uint32 vertexsize = 0;
 uint08 indextype = FACE_FORMAT_UINT_16;
 uint08 indexsize = 2;
 switch(head02) {
   case(0x01A7) : vertexsize = 0x0C; cout << "0x01A7" << endl; break; // OK
   case(0x0905) : vertexsize = 0x0C; cout << "0x0905" << endl; break; // OK
   case(0x0925) : vertexsize = 0x0C; cout << "0x0925" << endl; break; // OK
   case(0x09A3) : vertexsize = 0x0C; cout << "0x09A3" << endl; break; // OK
   case(0x09A7) : vertexsize = 0x0C; cout << "0x09A7" << endl; break; // OK
   case(0x09AF) : vertexsize = 0x0C; cout << "0x09AF" << endl; break; // OK
   case(0x09BF) : vertexsize = 0x0C; cout << "0x09BF" << endl; break; // OK
   case(0x09E7) : vertexsize = 0x20; cout << "0x09E7" << endl; break; // OK
   default : {
        stringstream msg;
        msg << "Unknown vertex type " << head02 << ".";
        return error(msg.str().c_str());
       }
  }

 //
 // PROCESS #02
 // READ AND SAVE FILE INFORMATION
 //

 if(debug) dfile << "FILE INFORMATION" << std::endl;
 uint32 fileinfo[30];
 for(size_t i = 0; i < 30; i++) fileinfo[i] = BE_read_uint32(ifile);
 if(debug) {
    // 0x20
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
 deque<SURFACEGROUP> surface_data;

 // move to surface information
 if(fileinfo[0])
   {
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
        surface_data.push_back(sg);
       }
   }

 //
 // PROCESS #04
 // READ SURFACE-TEXTURE ASSOCIATION DATA
 //

 struct TEXTASSINFO {
  uint32 material_index;
  uint32 trigroup_index;
 };
 deque<TEXTASSINFO> textass_data;

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

 if(fileinfo[2])
   {
    // move to surface information
    if(debug) dfile << "SURFACE-TEXTURE ASSOCIATION" << endl;
    ifile.seekg(fileinfo[2]);
    if(ifile.fail()) return error("Seek failure.");
   
    // read surface information
    for(size_t i = 0; i < surface_data.size(); i++)
       {
        // read group information
        boost::shared_array<uint16> data(new uint16[surface_data[i].elem]);
        BE_read_array(ifile, data.get(), surface_data[i].elem);
        if(ifile.fail()) return error("Read failure.");
   
        uint16 curr_0x2000 = 0xFFFF;
        uint16 curr_0x3000 = 0xFFFF;
        uint16 curr_0x4000 = 0xFFFF;
   
        // for each item read
        for(size_t j = 0; j < surface_data[i].elem; j++)
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
               TEXTASSINFO info = { curr_0x2000, curr_0x3000 };
               textass_data.push_back(info);
              }
            else if((data[j] & 0xFF00) == 0x4000)
              {
               curr_0x4000 = (uint16)(data[j] & 0x00FF);
               if(debug) dfile << "0x" << hex << data[j] << " ";
              }
           }
       }
    if(debug) dfile << dec << endl;
    if(debug) dfile << "Number of material-trigroup associations = " << textass_data.size() << endl;
    if(debug) for(size_t i = 0; i < textass_data.size(); i++)  dfile << textass_data[i].material_index << "," << textass_data[i].trigroup_index << endl;
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
        SMC_MATERIAL mat;
        mat.id = name.str();
        mat.twoside = false;
        mat.basemap = p03;
        mat.normmap = p21;
        mat.specmap = p27;
        mat.tranmap = INVALID_TEXTURE_INDEX;//p03;
        mat.bumpmap = INVALID_TEXTURE_INDEX;
        mat.resmap1 = INVALID_TEXTURE_INDEX;
        mat.resmap2 = INVALID_TEXTURE_INDEX;
        mat.resmap3 = INVALID_TEXTURE_INDEX;
        mat.resmap4 = INVALID_TEXTURE_INDEX;

        // save material
        smc.materials.push_back(mat);
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
    if(debug) dfile << "TRIANGLE GROUPS (TRIANGLES, POSITION)" << endl;
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
        if(debug) dfile << "trigroup[" << i << "] = (" << info.offset << ", " << info.triangles << ")" << endl;
       }

    if(debug) dfile << endl;
   }

 //
 // PROCESS #08
 // READ INDEX BUFFER
 //

 if(fileinfo[12])
   {
    // move to index buffer
    if(debug) dfile << "INDEX BUFFER DATA" << endl;
    ifile.seekg(fileinfo[12]);
    if(ifile.fail()) return error("Seek failure.");
    
    // number of triangles
    uint32 n_triangles = fileinfo[13];
    if(n_triangles == 0) return error("Invalid number of triangles.");
    
    // process triangle groups
    for(size_t i = 0; i < trilist_data.size(); i++)
       {
        // get texture-material association data
        TEXTASSINFO ai = textass_data[i];
        if(!(ai.material_index < smc.materials.size())) return error("Texture association data is invalid.");
        uint32 ib_material = ai.material_index;
    
        // surface name
        stringstream ss;
        ss << "trigroup_" << setfill('0') << setw(3) << i;
    
        // move to data offset
        uint32 ib_offset = 3*indexsize*trilist_data[i].offset;
        ifile.seekg(fileinfo[12] + ib_offset);
        if(ifile.fail()) return error("Seek failure.");
    
        // read index buffer
        uint32 ib_tris = trilist_data[i].triangles;
        uint32 ib_elem = ib_tris*3;
        uint32 ib_char = ib_elem*indexsize;
        boost::shared_array<char> ib_data(new char[ib_char]);
        if(indextype == FACE_FORMAT_UINT_16) BE_read_array(ifile, reinterpret_cast<uint16*>(ib_data.get()), ib_elem);
        else BE_read_array(ifile, reinterpret_cast<uint32*>(ib_data.get()), ib_elem);
        if(ifile.fail()) return error("Read failure.");
    
        // create index buffer
        SMC_IDX_BUFFER ib;
        ib.format = indextype;
        ib.type = FACE_TYPE_TRIANGLES;
        ib.name = ss.str();
        ib.elem = ib_elem;
        ib.data = ib_data;
        ib.tris = ib_tris;
        ib.material = ib_material;
    
        // save index buffer
        smc.ibuffer.push_back(ib);
       }
    if(debug) dfile << endl;
   }

 //
 // PROCESS #09
 // READ VERTEX BUFFER
 //

 if(fileinfo[15])
   {
    // number of vertices
    uint32 n_vertices = fileinfo[14];
    if(n_vertices == 0) return error("Invalid number of vertices.");

    // move to vertex buffer
    if(debug) dfile << "VERTEX BUFFER DATA" << std::endl;
    ifile.seekg(fileinfo[15]);
    if(ifile.fail()) return error("Seek failure.");

    // create vertex buffer
    smc.vbuffer.flags = VERTEX_POSITION;
    smc.vbuffer.name = "vbuffer";
    smc.vbuffer.elem = n_vertices;
    smc.vbuffer.data.reset(new SMC_VERTEX[n_vertices]);

    // vertex flags
    if(head02 == 0x01A7) {
       smc.vbuffer.flags |= VERTEX_UV;
    //   smc.vbuffer.flags |= VERTEX_WEIGHTS;
    //   smc.vbuffer.flags |= VERTEX_WMAPIDX;
      }
    else if(head02 == 0x0905) {
       smc.vbuffer.flags |= VERTEX_UV;
      }
    else if(head02 == 0x0925) {
       smc.vbuffer.flags |= VERTEX_UV;
      }
    else if(head02 == 0x09A7) {
       smc.vbuffer.flags |= VERTEX_UV;
      }
    else if(head02 == 0x09AF) {
       smc.vbuffer.flags |= VERTEX_UV;
      }
    else if(head02 == 0x09BF) {
       smc.vbuffer.flags |= VERTEX_UV;
      }
    else if(head02 == 0x09E7) {
       smc.vbuffer.flags |= VERTEX_UV;
      }

    // read vertices
    for(size_t i = 0; i < n_vertices; i++)
       {
        if(head02 == 0x01A7)
          {
           float v01 = BE_read_real32(ifile); //  4
           float v02 = BE_read_real32(ifile); //  8
           float v03 = BE_read_real32(ifile); // 12
           smc.vbuffer.data[i].vx = v01;
           smc.vbuffer.data[i].vy = v02;
           smc.vbuffer.data[i].vz = v03;
           if(debug) dfile << v01 << endl;
           if(debug) dfile << v02 << endl;
           if(debug) dfile << v03 << endl;
           if(debug) dfile << endl;
          }
        else if(head02 == 0x0905)
          {
           float v01 = BE_read_real32(ifile); //  4
           float v02 = BE_read_real32(ifile); //  8
           float v03 = BE_read_real32(ifile); // 12
           smc.vbuffer.data[i].vx = v01;
           smc.vbuffer.data[i].vy = v02;
           smc.vbuffer.data[i].vz = v03;
           if(debug) dfile << v01 << endl;
           if(debug) dfile << v02 << endl;
           if(debug) dfile << v03 << endl;
           if(debug) dfile << endl;
          }
        else if(head02 == 0x0925)
          {
           float v01 = BE_read_real32(ifile); //  4
           float v02 = BE_read_real32(ifile); //  8
           float v03 = BE_read_real32(ifile); // 12
           smc.vbuffer.data[i].vx = v01;
           smc.vbuffer.data[i].vy = v02;
           smc.vbuffer.data[i].vz = v03;
           if(debug) dfile << v01 << endl;
           if(debug) dfile << v02 << endl;
           if(debug) dfile << v03 << endl;
           if(debug) dfile << endl;
          }
        else if(head02 == 0x09A3) // ??? WRONG?
          {
           float v01 = BE_read_real32(ifile); //  4
           float v02 = BE_read_real32(ifile); //  8
           float v03 = BE_read_real32(ifile); // 12
           smc.vbuffer.data[i].vx = v01;
           smc.vbuffer.data[i].vy = v02;
           smc.vbuffer.data[i].vz = v03;
           if(debug) dfile << v01 << endl;
           if(debug) dfile << v02 << endl;
           if(debug) dfile << v03 << endl;
           if(debug) dfile << endl;
          }
        else if(head02 == 0x09A7)
          {
           float v01 = BE_read_real32(ifile); //  4
           float v02 = BE_read_real32(ifile); //  8
           float v03 = BE_read_real32(ifile); // 12
           smc.vbuffer.data[i].vx = v01;
           smc.vbuffer.data[i].vy = v02;
           smc.vbuffer.data[i].vz = v03;
           if(debug) dfile << v01 << endl;
           if(debug) dfile << v02 << endl;
           if(debug) dfile << v03 << endl;
           if(debug) dfile << endl;
          }
        else if(head02 == 0x09AF)
          {
           float v01 = BE_read_real32(ifile); //  4
           float v02 = BE_read_real32(ifile); //  8
           float v03 = BE_read_real32(ifile); // 12
           smc.vbuffer.data[i].vx = v01;
           smc.vbuffer.data[i].vy = v02;
           smc.vbuffer.data[i].vz = v03;
           if(debug) dfile << v01 << endl;
           if(debug) dfile << v02 << endl;
           if(debug) dfile << v03 << endl;
           if(debug) dfile << endl;
          }
        else if(head02 == 0x09BF)
          {
           float v01 = BE_read_real32(ifile); //  4
           float v02 = BE_read_real32(ifile); //  8
           float v03 = BE_read_real32(ifile); // 12
           smc.vbuffer.data[i].vx = v01;
           smc.vbuffer.data[i].vy = v02;
           smc.vbuffer.data[i].vz = v03;
           if(debug) dfile << v01 << endl;
           if(debug) dfile << v02 << endl;
           if(debug) dfile << v03 << endl;
           if(debug) dfile << endl;
          }
        else if(head02 == 0x09E7)
          {
           auto v01 = BE_read_real32(ifile);
           auto v02 = BE_read_real32(ifile);
           auto v03 = BE_read_real32(ifile);
           auto v04 = BE_read_uint16(ifile);
           auto v05 = BE_read_uint16(ifile);
           auto v06 = BE_read_uint16(ifile);
           auto v07 = BE_read_uint16(ifile);
           auto v08 = BE_read_uint16(ifile);
           auto v09 = BE_read_uint16(ifile);
           auto v10 = BE_read_uint32(ifile);
           auto v11 = BE_read_sint16(ifile);
           auto v12 = BE_read_sint16(ifile);
           smc.vbuffer.data[i].vx = v01;
           smc.vbuffer.data[i].vy = v02;
           smc.vbuffer.data[i].vz = v03;
           smc.vbuffer.data[i].tu =  v11/1023.0f;
           smc.vbuffer.data[i].tv = -v12/1023.0f;
           if(debug) dfile << v01 << endl;
           if(debug) dfile << v02 << endl;
           if(debug) dfile << v03 << endl;
           if(debug) dfile << v04 << endl;
           if(debug) dfile << v05 << endl;
           if(debug) dfile << v06 << endl;
           if(debug) dfile << v07 << endl;
           if(debug) dfile << v08 << endl;
           if(debug) dfile << v09 << endl;
           if(debug) dfile << v10 << endl;
           if(debug) dfile << v11 << endl;
           if(debug) dfile << v12 << endl;
           if(debug) dfile << endl;
          }
       }
   }

 //
 // PROCESS #10
 // READ SEPARATE UV DATA
 //

 if(fileinfo[17])
   {
    // move to offset
    if(debug) dfile << "SEPARATE UV DATA" << std::endl;
    ifile.seekg(fileinfo[17]);
    if(ifile.fail()) return error("Seek failure.");
    if(head02 == 0x01A7 || head02 == 0x0905 || head02 == 0x0925 || head02 == 0x09A7 || head02 == 0x09AF || head02 == 0x09BF)
      {
       uint32 n_vertices = fileinfo[14];
       for(size_t i = 0; i < n_vertices; i++) {
           auto v1 = BE_read_sint16(ifile);
           auto v2 = BE_read_sint16(ifile);
           if(debug) dfile << "v1 = " << v1 << endl;
           if(debug) dfile << "v2 = " << v2 << endl;
           smc.vbuffer.data[i].tu =  v1/1024.0f;
           smc.vbuffer.data[i].tv = -v2/1024.0f;
          }
      }
    if(debug) dfile << std::endl;
   }

 //
 // PROCESS #11
 // READ BONE MATRICES
 //
/*
 // process bones
 if(fileinfo[23])
   {
    // move to bone matrices
    if(debug) dfile << "BONE MATRICES" << endl;
    ifile.seekg(fileinfo[23]);
    if(ifile.fail()) return error("Seek failure.");

    // data to save
    std::deque<boost::shared_array<float>> mlist;
    SKELETON skeleton;
    skeleton.set_identifier("skeleton");

    // read matrices
    uint32 n_bones = fileinfo[24];
    if(debug) dfile << "number of bones = " << n_bones << endl;
    for(size_t i = 0; i < n_bones; i++) {
        boost::shared_array<float> m(new float[16]);
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
        mlist.push_back(m);
        if(debug) dfile << m[ 0] << " " << m[ 1] << " " << m[ 2] << " " << m[ 3] << endl;
        if(debug) dfile << m[ 4] << " " << m[ 5] << " " << m[ 6] << " " << m[ 7] << endl;
        if(debug) dfile << m[ 8] << " " << m[ 9] << " " << m[10] << " " << m[11] << endl;
        if(debug) dfile << m[12] << " " << m[13] << " " << m[14] << " " << m[15] << endl;
        if(debug) dfile << endl;
       }

    // move to bone information
    if(debug) dfile << "BONES" << std::endl;
    ifile.seekg(fileinfo[25]);
    if(ifile.fail()) return error("Seek failure.");
    if(n_bones != fileinfo[26]) return error("Number of joints do not match.");

    // read bone information
    for(size_t i = 0; i < n_bones; i++)
       {
        // joint
        JOINT joint;
        uint32 index = (uint32)i;
   
        // joint name
        char name[5];
        ifile.read(&name[0], 4);
        std::reverse(&name[0], &name[4]);
        name[4] = '\0';

        // make joint name unique
        std::stringstream jnss;
        jnss << name << "_" << setfill('0') << setw(3) << i;
        joint.name = jnss.str();
   
        // joint matrix
        joint.matrix = mlist[i];

        // joint relative positions
        float x = BE_read_real32(ifile);
        float y = BE_read_real32(ifile);
        float z = BE_read_real32(ifile);
        joint.rel_x = x;
        joint.rel_y = y;
        joint.rel_z = z;

        // set parent
        uint32 parent = BE_read_uint32(ifile);
        uint32 xxxxxx = BE_read_uint32(ifile);
        joint.parent = parent;
   
        // debug information
        if(debug) dfile << "index = " << index << endl;   
        if(debug) dfile << "name = " << name << endl;   
        if(debug) dfile << "x = " << x << endl;
        if(debug) dfile << "y = " << y << endl;
        if(debug) dfile << "z = " << z << endl;
        if(debug) dfile << "parent = " << parent << endl;
        if(debug) dfile << "xxxxxx = " << xxxxxx << endl;
        if(debug) dfile << std::endl;

        // insert joint
        if(parent == INVALID_JOINT && i != 0) continue;
        else skeleton.insert(index, joint);
       }

    // print joint tree in debug mode
    if(debug) {
       stringstream ss;
       skeleton.PrintJointTree(ss);
       dfile << ss.str() << endl;
      }

    // create skeleton
    smc.skeletons.push_back(skeleton);
   }
*/

 //
 // PROCESS #12
 // READ TEXTURES
 //

 if(fileinfo[27])
   {
    // move to texture information
    if(debug) dfile << "TEXTURES" << endl;
    ifile.seekg(fileinfo[27]);
    if(ifile.fail()) return error("Seek failure.");
   
    // read texture information
    uint32 n_textures = fileinfo[28];
    for(uint32 i = 0; i < n_textures; i++)
       {
        uint32 param1 = BE_read_uint32(ifile); // index
        uint32 param2 = BE_read_uint32(ifile); // 0
        uint16 param3 = BE_read_uint16(ifile); // 0x0001
        uint16 param4 = BE_read_uint16(ifile); // 0x0040 | 0x0100 | 0x0200 | 0x0400
        uint16 param5 = BE_read_uint16(ifile); // 0x0040 | 0x0100 | 0x0200 | 0x0400
        uint16 param6 = BE_read_uint16(ifile); // 0x2000 | 0xFF00
        if(debug) dfile << "param1 = " << param1 << endl;
        if(debug) dfile << "param2 = " << param2 << endl;
        if(debug) dfile << "param3 = " << param3 << endl;
        if(debug) dfile << "param4 = " << param4 << endl;
        if(debug) dfile << "param5 = " << param5 << endl;
        if(debug) dfile << "param6 = " << param6 << endl;
        if(debug) dfile << std::endl;
   
        // texture id
        stringstream id;
        id << "texture_" << setfill('0') << setw(3) << param1;
   
        // texture filename
        stringstream fn;
        fn << setfill('0') << setw(3) << param1 << ".tga";
   
        // insert texture
        SMC_TEXTURE item = { id.str(), fn.str() };
        smc.textures.push_back(item);
       }
    if(debug) dfile << endl;
   }

 // save geometry
 return SaveLWO(fn_path.c_str(), fn_name.c_str(), smc);
}

};};

namespace X_SYSTEM { namespace X_GAME {

bool extract(void)
{
 std::string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pathname)
{
 return extractor(pathname).extract();
}

};};