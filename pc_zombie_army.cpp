#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_zlib.h"
#include "x_stream.h"
#include "x_amc.h"
#include "x_dds.h"
#include "x_gui.h"
#include "pc_zombie_army.h"

#define X_SYSTEM PC
#define X_GAME   ZOMBIE_ARMY_TRILOGY

namespace X_SYSTEM { namespace X_GAME {

inline uint32 reverse_byte_order(uint32 x)
{
 auto data = reinterpret_cast<char*>(&x);
 auto temp = data[0];
 data[0] = data[3];
 data[3] = temp;
 temp = data[1];
 data[1] = data[2];
 data[2] = temp;
 return x;
}

bool ProcessArchive(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint64 filetype = BE_read_uint64(ifile);
 if(ifile.fail()) return error("Read failure.");

 // ASURA
 if(filetype == 0x4173757261202020ull)
   {
    // save position
    uint64 position = (uint64)ifile.tellg();
    if(ifile.fail()) return error("Tell failure.");

    // loop files
    for(;;)
       {
        // read chunk type
        uint32 chunktype = LE_read_uint32(ifile);
        if(ifile.eof()) break;
        else if(ifile.fail()) return error("Read failure.");

        // read chunk size
        uint32 chunksize = LE_read_uint32(ifile);
        if(ifile.eof()) break;
        else if(ifile.fail()) return error("Read failure.");

        // RSCF
        if(chunktype == reverse_byte_order(0x52534346ul))
          {
           // read unknowns
           uint32 unk01 = LE_read_uint32(ifile); // data type (model, environment, texture)
           uint32 unk02 = LE_read_uint32(ifile); // ???
           uint32 unk03 = LE_read_uint32(ifile); // file type (DDS, WAV, extensionless)
           uint32 unk04 = LE_read_uint32(ifile); // 0x05

           // read data size
           uint32 datasize = LE_read_uint32(ifile);
           if(ifile.fail()) return error("Read failure.");

           // process data
           if(datasize)
             {
              // read filename
              char dataname[255];
              dataname[0] = '\0';
              if(!read_string(ifile, &dataname[0], 255)) return error("Read failure.");
              
              // filename aligned to DWORD boundary
              uint64 temp = strlen(dataname) + 1;
              uint64 move = ((temp + 0x03) & (~0x03)) - temp;
              if(move) {
                 ifile.seekg(move, ios::cur);
                 if(ifile.fail()) return error("Seek failure.");
                }
              
              // read data
              boost::shared_array<char> data(new char[datasize]);
              ifile.read(data.get(), datasize);
              if(ifile.fail()) return error("Read failure.");
              
              // create filename
              STDSTRING ofname = pathname;
              ofname += Unicode08ToUnicode16(dataname).get();
              
              // change extension (extensionless)
              if(unk03 == 0x00) {
                 if(unk01 == 0x02) ofname += TEXT(".mrt");
                 else if(unk01 == 0x03) ofname += TEXT(".mdi"); // model file
                 else if(unk01 == 0x04) ofname += TEXT(".mrps");
                 else if(unk01 == 0x06) ofname += TEXT(".mdl"); // model file
                 else if(unk01 == 0x0A) ofname += TEXT(".env"); // model file
                 else { wcout << " check this model rename for " << ofname.c_str() << endl; return false; }
                }
              else if(unk03 == 0x02) {
                 if(unk01 == 0x00) ofname = ChangeFileExtension(ofname.c_str(), TEXT("dds")).get();
                 else if(unk01 == 0x01) ; // properly named DDS file for fonts
                 else { wcout << " check this texture rename for " << ofname.c_str() << endl; return false; }
                }
              
              // create directory
              STDSTRING ofpath = GetPathnameFromFilename(ofname.c_str()).get();
              int result = SHCreateDirectoryEx(0, ofpath.c_str(), NULL);
              if(result == ERROR_SUCCESS) ; // OK
              else if(result == ERROR_ALREADY_EXISTS) ; // OK
              else if(result == ERROR_FILE_EXISTS) ; // OK
              else return error("Call to SHCreateDirectoryEx failed.");
              CreateDirectory(ofpath.c_str(), NULL);
              
              // save data
              ofstream ofile(ofname.c_str(), ios::binary);
              if(!ofile) return error("Failed to create output file.");
              ofile.write(data.get(), datasize);
              if(ofile.fail()) return error("Write failure.");
             }
          }
        // ASTS
        else if(chunktype == reverse_byte_order(0x41535453ul))
          {
           // read unknowns
           uint32 unk01 = LE_read_uint32(ifile); // 0x02
           uint32 unk02 = LE_read_uint32(ifile); // 0x00
           if(ifile.fail()) return error("Read failure.");

           // read number of files
           uint32 n_files = LE_read_uint32(ifile);
           if(ifile.fail()) return error("Read failure.");
           if(!n_files) continue;

           // read unknowns
           uint08 unk03 = LE_read_uint08(ifile); // 0x00
           if(ifile.fail()) return error("Read failure.");

           // read items
           struct ASTSINFO {
            std::string p01;
            uint08 p02;
            uint32 p03;
            uint32 p04;
           };
           deque<ASTSINFO> itemlist;
           for(uint32 i = 0; i < n_files; i++)
              {
               // read filename
               char dataname[255];
               dataname[0] = '\0';
               if(!read_string(ifile, &dataname[0], 255)) return error("Read failure.");
        
               // filename aligned to DWORD boundary
               uint64 temp = strlen(dataname) + 1;
               uint64 move = ((temp + 0x03) & (~0x03)) - temp;
               if(move) {
                  ifile.seekg(move, ios::cur);
                  if(ifile.fail()) return error("Seek failure.");
                 }

               // read remainder of item
               ASTSINFO item;
               item.p01 = dataname;
               item.p02 = LE_read_uint08(ifile);
               item.p03 = LE_read_uint32(ifile);
               item.p04 = LE_read_uint32(ifile);
               if(ifile.fail()) return error("Read failure.");

               // insert item
               itemlist.push_back(item);
              }

           // for each item
           for(uint32 i = 0; i < n_files; i++)
              {
               // actual data in another file (this is an SSM file)
               if(!(itemlist[i].p04 < filesize)) {
                  cout << "Ouch!" << endl;
                  continue;
                 }

               // move to offset
               ifile.seekg(itemlist[i].p04);
               if(ifile.fail()) return error("Seek failure.");

               // nothing to do
               if(itemlist[i].p03 == 0) continue;

               // read data
               boost::shared_array<char> data(new char[itemlist[i].p03]);
               ifile.read(data.get(), itemlist[i].p03);
               if(ifile.fail()) return error("Read failure.");
               
               // create filename
               STDSTRING ofname = pathname;
               ofname += Unicode08ToUnicode16(itemlist[i].p01.c_str()).get();
               
               // create directory
               STDSTRING ofpath = GetPathnameFromFilename(ofname.c_str()).get();
               int result = SHCreateDirectoryEx(0, ofpath.c_str(), NULL);
               if(result == ERROR_SUCCESS) ; // OK
               else if(result == ERROR_ALREADY_EXISTS) ; // OK
               else if(result == ERROR_FILE_EXISTS) ; // OK
               else return error("Call to SHCreateDirectoryEx failed.");
               CreateDirectory(ofpath.c_str(), NULL);
               
               // save data
               ofstream ofile(ofname.c_str(), ios::binary);
               if(!ofile) return error("Failed to create output file.");
               ofile.write(data.get(), itemlist[i].p03);
               if(ofile.fail()) return error("Write failure.");
              }
          }
        // HSKN
        else if(chunktype == reverse_byte_order(0x48534B4Eul))
          {
           // read unknowns
           uint32 unk01 = LE_read_uint32(ifile);
           uint32 unk02 = LE_read_uint32(ifile);
           uint32 unk03 = LE_read_uint32(ifile);
           uint32 unk04 = LE_read_uint32(ifile);

           // read name           
           char name[255];
           if(!read_string(ifile, &name[0], 255)) return error("Read failure.");

           // read chunk
           ifile.seekg(position);
           if(ifile.fail()) return error("Seek failure.");
           boost::shared_array<char> data(new char[chunksize]);
           ifile.read(data.get(), chunksize);
           if(ifile.fail()) return error("Read failure.");

           // create filename
           STDSTRING ofname = pathname;
           ofname += Unicode08ToUnicode16(name).get();
           ofname += TEXT(".HKSN");

           // save file
           ofstream ofile(ofname.c_str(), ios::binary);
           if(!ofile) return error("Failed to create output file.");
           ofile.write(data.get(), chunksize);
           if(ofile.fail()) return error("Write failure.");
          }
        // HSKL
        else if(chunktype == reverse_byte_order(0x48534B4Cul))
          {
           // read unknowns
           uint32 unk01 = LE_read_uint32(ifile); // 0x00
           uint32 unk02 = LE_read_uint32(ifile); // 0x01

           // read name           
           char name1[255];
           if(!read_string(ifile, &name1[0], 255)) return error("Read failure.");

           // filename aligned to DWORD boundary
           uint64 temp = strlen(name1) + 1;
           uint64 move = ((temp + 0x03) & (~0x03)) - temp;
           if(move) {
              ifile.seekg(move, ios::cur);
              if(ifile.fail()) return error("Seek failure.");
             }

           // read name           
           char name2[255];
           if(!read_string(ifile, &name2[0], 255)) return error("Read failure.");

           // read chunk
           ifile.seekg(position);
           if(ifile.fail()) return error("Seek failure.");
           boost::shared_array<char> data(new char[chunksize]);
           ifile.read(data.get(), chunksize);
           if(ifile.fail()) return error("Read failure.");

           // create filename
           STDSTRING ofname = pathname;
           ofname += Unicode08ToUnicode16(name1).get();
           ofname += TEXT(".HKSL");

           // save file
           ofstream ofile(ofname.c_str(), ios::binary);
           if(!ofile) return error("Failed to create output file.");
           ofile.write(data.get(), chunksize);
           if(ofile.fail()) return error("Write failure.");
          }
        // HMPT
        else if(chunktype == reverse_byte_order(0x484D5054ul))
          {
           // read unknowns
           uint32 unk01 = LE_read_uint32(ifile);
           uint32 unk02 = LE_read_uint32(ifile);
           uint32 unk03 = LE_read_uint32(ifile);

           // read name           
           char name[255];
           if(!read_string(ifile, &name[0], 255)) return error("Read failure.");

           // read chunk
           ifile.seekg(position);
           if(ifile.fail()) return error("Seek failure.");
           boost::shared_array<char> data(new char[chunksize]);
           ifile.read(data.get(), chunksize);
           if(ifile.fail()) return error("Read failure.");

           // create filename
           STDSTRING ofname = pathname;
           ofname += Unicode08ToUnicode16(name).get();
           ofname += TEXT(".HMPT");

           // save file
           ofstream ofile(ofname.c_str(), ios::binary);
           if(!ofile) return error("Failed to create output file.");
           ofile.write(data.get(), chunksize);
           if(ofile.fail()) return error("Write failure.");
          }
        // maybe important (NZA3_Horde_Church.inflate) contains model
        else if(chunktype == reverse_byte_order(0x4F434D48ul)) { // OCMH
          }
        // maybe important (Misc/Common.inflate)
        else if(chunktype == reverse_byte_order(0x48534B45ul)) { // HSKE
          }
        // maybe important
        else if(chunktype == reverse_byte_order(0x48534242ul)) { // HSSB
           // paired with HSKL/HSKN
          }
        // maybe important
        else if(chunktype == reverse_byte_order(0x48534E44ul)) { // HSND
          }
        // maybe important
        else if(chunktype == reverse_byte_order(0x494B544Dul)) { // IKTM
          }
        // maybe important
        else if(chunktype == reverse_byte_order(0x52414744ul)) { // RAGD
          }
        // maybe important
        else if(chunktype == reverse_byte_order(0x56544558ul)) { // VTEX
          }
        // not important!
        // let's just skip these chunks
        else if(chunktype == 0x00000000) {
           message("Reached EOF chunk."); // END-OF-FILE PADDING
           break;
          }
        else if(chunktype == reverse_byte_order(0x4144414Dul)) ; // ADAM
        else if(chunktype == reverse_byte_order(0x41445350ul)) ; // ADSP
        else if(chunktype == reverse_byte_order(0x4146534Ful)) ; // AFSO
        else if(chunktype == reverse_byte_order(0x4150464Ful)) ; // APFO
        else if(chunktype == reverse_byte_order(0x414D524Ful)) ; // AMRO
        else if(chunktype == reverse_byte_order(0x41545254ul)) ; // ATRT
        else if(chunktype == reverse_byte_order(0x41554441ul)) ; // AUDA
        else if(chunktype == reverse_byte_order(0x41584154ul)) ; // AXAT
        else if(chunktype == reverse_byte_order(0x41584242ul)) ; // AXBB
        else if(chunktype == reverse_byte_order(0x41584254ul)) ; // AXBT
        else if(chunktype == reverse_byte_order(0x424C5545ul)) ; // BLUE
        else if(chunktype == reverse_byte_order(0x434F4E41ul)) ; // CONA
        else if(chunktype == reverse_byte_order(0x43524E41ul)) ; // CRNA
        else if(chunktype == reverse_byte_order(0x43544143ul)) ; // CTAC
        else if(chunktype == reverse_byte_order(0x43544154ul)) ; // CTAT
        else if(chunktype == reverse_byte_order(0x43544556ul)) ; // CTEV
        else if(chunktype == reverse_byte_order(0x43545452ul)) ; // CTTR
        else if(chunktype == reverse_byte_order(0x43555453ul)) ; // CUTS
        else if(chunktype == reverse_byte_order(0x4443414Cul)) ; // DCAL
        else if(chunktype == reverse_byte_order(0x444C4554ul)) ; // DLET
        else if(chunktype == reverse_byte_order(0x444C4556ul)) ; // DLEV
        else if(chunktype == reverse_byte_order(0x444C4C4Eul)) ; // DLLN
        else if(chunktype == reverse_byte_order(0x444C4C54ul)) ; // DLLT
        else if(chunktype == reverse_byte_order(0x444C5255ul)) ; // DLRU
        else if(chunktype == reverse_byte_order(0x44594D43ul)) ; // DYMC
        else if(chunktype == reverse_byte_order(0x454D4F44ul)) ; // EMOD
        else if(chunktype == reverse_byte_order(0x454E5449ul)) ; // ENTI
        else if(chunktype == reverse_byte_order(0x4641414Eul)) ; // FAAN
        else if(chunktype == reverse_byte_order(0x46414345ul)) ; // FACE
        else if(chunktype == reverse_byte_order(0x464E464Ful)) ; // FNFO
        else if(chunktype == reverse_byte_order(0x464F4720ul)) ; // FOG_
        else if(chunktype == reverse_byte_order(0x464F4E54ul)) ; // FONT
        else if(chunktype == reverse_byte_order(0x46535832ul)) ; // FSX2
        else if(chunktype == reverse_byte_order(0x46584554ul)) ; // FXET
        else if(chunktype == reverse_byte_order(0x46585054ul)) ; // FXPT
        else if(chunktype == reverse_byte_order(0x46585354ul)) ; // FXST
        else if(chunktype == reverse_byte_order(0x46585454ul)) ; // FXTT
        else if(chunktype == reverse_byte_order(0x4749534Eul)) ; // GISN
        else if(chunktype == reverse_byte_order(0x4843414Eul)) ; // HCAN
        else if(chunktype == reverse_byte_order(0x48545854ul)) ; // HTXT
        else if(chunktype == reverse_byte_order(0x494E5354ul)) ; // INST
        else if(chunktype == reverse_byte_order(0x4D415245ul)) ; // MARE
        else if(chunktype == reverse_byte_order(0x4D455441ul)) ; // META
        else if(chunktype == reverse_byte_order(0x4E415631ul)) ; // NAV1
        else if(chunktype == reverse_byte_order(0x5048454Eul)) ; // PHEN
        else if(chunktype == reverse_byte_order(0x504C5554ul)) ; // PLUT
        else if(chunktype == reverse_byte_order(0x52454E44ul)) ; // REND
        else if(chunktype == reverse_byte_order(0x5253464Cul)) ; // RSFL
        else if(chunktype == reverse_byte_order(0x52564250ul)) ; // RVBP
        else if(chunktype == reverse_byte_order(0x53444443ul)) ; // SDDC
        else if(chunktype == reverse_byte_order(0x53444556ul)) ; // SDEV
        else if(chunktype == reverse_byte_order(0x53444753ul)) ; // SDGS
        else if(chunktype == reverse_byte_order(0x53445048ul)) ; // SDPH
        else if(chunktype == reverse_byte_order(0x5344534Dul)) ; // SDSM
        else if(chunktype == reverse_byte_order(0x534B5953ul)) ; // SKYS
        else if(chunktype == reverse_byte_order(0x53444D58ul)) ; // SMDX
        else if(chunktype == reverse_byte_order(0x534D5347ul)) ; // SMSG
        else if(chunktype == reverse_byte_order(0x53554254ul)) ; // SUBT
        else if(chunktype == reverse_byte_order(0x54455854ul)) ; // TEXT
        else if(chunktype == reverse_byte_order(0x54545854ul)) ; // TTXT
        else if(chunktype == reverse_byte_order(0x5458414Eul)) ; // TXAN
        else if(chunktype == reverse_byte_order(0x5458464Cul)) ; // TXFL
        else if(chunktype == reverse_byte_order(0x5549414Eul)) ; // UIAN
        else if(chunktype == reverse_byte_order(0x55494155ul)) ; // UIAU
        else if(chunktype == reverse_byte_order(0x55494D50ul)) ; // UIMP
        else if(chunktype == reverse_byte_order(0x55495450ul)) ; // UITP
        else if(chunktype == reverse_byte_order(0x57505347ul)) ; // WPSG
        else if(chunktype == reverse_byte_order(0x57544852ul)) ; // WTHR
        else if(chunktype == reverse_byte_order(0x646C636Cul)) ; // dlcl
        else if(chunktype == reverse_byte_order(0x6E61766Eul)) ; // navn
        else if(chunktype == reverse_byte_order(0x6F696E66ul)) ; // oinf
        else if(chunktype == reverse_byte_order(0x70617364ul)) ; // pasd
        else {
           cout << "position = 0x" << hex << position << dec << endl;
           cout << "type = 0x" << hex << chunktype << dec << endl;
           return error("Unknown ASURA file data type.");
          }

        // seek new position
        position += chunksize;
        ifile.seekg(position);
        if(ifile.fail()) return error("Seek failure.");
       }
   }
 // ASURAZLB
 else if(filetype == 0x41737572615A6C62ull)
   {
    // read parameters
    uint32 unknown = LE_read_uint32(ifile); // 0x00
    uint32 deflate_size = LE_read_uint32(ifile);
    uint32 inflate_size = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // create output file
    STDSTRING ofname = pathname;
    ofname += shrtname;
    ofname += TEXT(".inflate");
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");

    // setup inflation
    ZLIBINFO2 zli;
    zli.deflatedBytes = deflate_size;
    zli.inflatedBytes = inflate_size;
    zli.offset = 0x14;
    deque<ZLIBINFO2> itemlist;
    itemlist.push_back(zli);

    // inflate
    if(!InflateZLIB(ifile, itemlist, 0, ofile))
       return error("ZLIB error.");

    // close and process
    ofile.close();
    if(!ProcessArchive(ofname.c_str())) return false;
   }
 // unknown
 else
    return error("Unknown ASURA file type.");

 // delete file on success
 ifile.close();
 DeleteFile(filename);
 return true;
}

bool ProcessMDL(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = LE_read_uint32(ifile); // number of meshes
 uint32 h02 = LE_read_uint32(ifile); // number of vertices
 uint32 h03 = LE_read_uint32(ifile); // number of face indices
 uint32 h04 = LE_read_uint32(ifile); // number of triangles
 uint32 h05 = LE_read_uint32(ifile); // 0x00000000 (is this type of vertex buffer or something?)
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // blank model
 if(h01 == 0x00) return true;

 // read mesh items
 struct MESHITEM {
  uint32 p01; // unique identifier
  uint32 p02; // 0x00000000
  uint32 p03; // indices
  uint32 p04; // 0x00000000
  uint32 p05; // 0x00000001
  uint32 p06; // 0x00000000
 };
 deque<MESHITEM> meshlist;
 for(uint32 mesh_id = 0; mesh_id < h01; mesh_id++) {
     MESHITEM mi;
     mi.p01 = LE_read_uint32(ifile);
     mi.p02 = LE_read_uint32(ifile);
     mi.p03 = LE_read_uint32(ifile);
     mi.p04 = LE_read_uint32(ifile);
     mi.p05 = LE_read_uint32(ifile);
     mi.p06 = LE_read_uint32(ifile);
     meshlist.push_back(mi);
    }

 // read bounding box
 real32 bbox[6];
 if(!LE_read_array(ifile, &bbox[0], 6)) return error("Read failure.", __LINE__);

 // read vertex buffer
 uint32 vb_size = 0x28 * h02;
 boost::shared_array<char> vb_data(new char[vb_size]);
 ifile.read(vb_data.get(), vb_size);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(!vb_size) return error("Invalid vertex buffer data.", __LINE__);

 // read index buffer
 uint32 ib_size = 0x02 * h03;
 boost::shared_array<char> ib_data(new char[ib_size]);
 ifile.read(ib_data.get(), ib_size);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(!ib_size) return error("Invalid index buffer data.", __LINE__);

 // binary streams
 binary_stream vbs(vb_data, vb_size);
 binary_stream ibs(ib_data, ib_size);

 // model container
 ADVANCEDMODELCONTAINER amc;

 // initialize vertex buffer
 AMC_VTXBUFFER vb;
 vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV /* | AMC_VERTEX_NORMAL */;
 vb.name = "default";
 vb.uvchan = 2;
 vb.uvtype[0] = AMC_DIFFUSE_MAP;
 for(uint32 i = 1; i < 8; i++) vb.uvtype[i] = AMC_INVALID_MAP;
 vb.colors = 0;
 vb.elem = h02;
 vb.data.reset(new AMC_VERTEX[vb.elem]);

 // process vertices
 for(uint32 i = 0; i < h02; i++)
    {
     vb.data[i].vx = bbox[3] + bbox[0]*(vbs.LE_read_uint16()/65535.0f);
     vb.data[i].vy = bbox[4] + bbox[1]*(vbs.LE_read_uint16()/65535.0f);
     vb.data[i].vz = bbox[5] + bbox[2]*(vbs.LE_read_uint16()/65535.0f);
     vb.data[i].vw = (vbs.LE_read_uint16()/65535.0f);
     vb.data[i].nx = (vbs.LE_read_uint16()/65535.0f);
     vb.data[i].ny = (vbs.LE_read_uint16()/65535.0f);
     vb.data[i].nz = (vbs.LE_read_uint16()/65535.0f);
     vb.data[i].nw = 1.0f;
     vbs.LE_read_uint16(); // unknown #1
     vbs.LE_read_uint16(); // unknown #1
     vbs.LE_read_uint16(); // unknown #1
     vbs.LE_read_uint16(); // unknown #2
     vbs.LE_read_uint16(); // unknown #2
     vb.data[i].uv[0][0] = vbs.LE_read_real16();
     vb.data[i].uv[0][1] = vbs.LE_read_real16();
     vb.data[i].uv[1][0] = vbs.LE_read_real16();
     vb.data[i].uv[1][1] = vbs.LE_read_real16();
     vb.data[i].wv[0] = ((real32)vbs.LE_read_uint08())/255.0f;
     vb.data[i].wv[1] = ((real32)vbs.LE_read_uint08())/255.0f;
     vb.data[i].wv[2] = ((real32)vbs.LE_read_uint08())/255.0f;
     vb.data[i].wv[3] = ((real32)vbs.LE_read_uint08())/255.0f;
     vb.data[i].wv[4] = 0.0f;
     vb.data[i].wv[5] = 0.0f;
     vb.data[i].wv[6] = 0.0f;
     vb.data[i].wv[7] = 0.0f;
     vb.data[i].wi[0] = vbs.LE_read_uint08();
     vb.data[i].wi[1] = vbs.LE_read_uint08();
     vb.data[i].wi[2] = vbs.LE_read_uint08();
     vb.data[i].wi[3] = vbs.LE_read_uint08();
     vb.data[i].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
     vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
     if(vbs.fail()) return error("Stream read failure.", __LINE__);
    }

 // insert vertex buffer
 amc.vblist.push_back(vb);

 // initialize index buffer
 AMC_IDXBUFFER ib;
 ib.format = AMC_UINT16;
 ib.type = AMC_TRIANGLES;
 ib.wind = AMC_CW;
 ib.name = "default";
 ib.elem = h03;
 ib.data = ib_data;

 // insert index buffer
 amc.iblist.push_back(ib);

 // process meshlist
 uint32 ib_position = 0;
 for(uint32 i = 0; i < meshlist.size(); i++)
    {
     // initialize surface
     AMC_SURFACE s;
     stringstream sn;
     sn << "surface_" << setfill('0') << setw(3) << i;
     s.name = sn.str();
     AMC_REFERENCE ref;
     ref.vb_index = 0;
     ref.vb_start = 0;
     ref.vb_width = vb.elem;
     ref.ib_index = 0;
     ref.ib_start = ib_position;
     ref.ib_width = meshlist[i].p03;
     ref.jm_index = AMC_INVALID_JMAP_INDEX;
     s.refs.push_back(ref);
     s.surfmat = AMC_INVALID_SURFMAT;

     // increment ib_position
     if(ref.ib_width % 3) return error("Indices must be divisible by three.", __LINE__);
     ib_position += ref.ib_width;

     // insert surface
     amc.surfaces.push_back(s);
    }

 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 bool result1 = SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 bool result2 = SaveOBJ(pathname.c_str(), shrtname.c_str(), transformed);
 return (result1 && result2);
}

bool ProcessSKN(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(magic != 0x48534B4E) return error("Invalid HSKN file.", __LINE__);

 // read chunksize
 uint32 chunksize = LE_read_uint32(ifile);
 if(!chunksize || chunksize < 0x20) return true;

 // read unknowns
 uint32 unk01 = LE_read_uint32(ifile); // 0x12
 uint32 unk02 = LE_read_uint32(ifile); // 0xC9
 uint32 unk03 = LE_read_uint32(ifile); // 0x00

 // read number of joints
 uint32 n_joints = LE_read_uint32(ifile);
 if(!n_joints) return true;

 // unk02 = 0xC1
 // unk02 = 0xC9
 // unk02 = 0xCB
 // unk02 = 0xCD
 // unk02 = 0x1C9
 // unk02 = 0x1CD

 // read name (1st)
 char name[255];
 name[0] = '\0';
 if(!read_string(ifile, &name[0], 255)) return error("Read failure.", __LINE__);

 // name aligned to DWORD boundary
 uint64 temp = strlen(name) + 1;
 uint64 move = ((temp + 0x03) & (~0x03)) - temp;
 if(move) {
    ifile.seekg(move, ios::cur);
    if(ifile.fail()) return error("Seek failure.", __LINE__);
   }

 // read indices
 boost::shared_array<uint32> indexset1(new uint32[n_joints]);
 if(!LE_read_array(ifile, indexset1.get(), n_joints)) return error("Read failure.", __LINE__);

 // unknown floats
 deque<boost::shared_array<real32>> datalist;
 for(uint32 i = 0; i < n_joints; i++) {
     boost::shared_array<real32> temp(new real32[7]);
     if(!LE_read_array(ifile, temp.get(), 7)) return error("Read failure.", __LINE__);
     datalist.push_back(temp);
     // cout << "joint[" << i << "] = " << temp[0] << ", ";
     // cout << temp[1] << ", ";
     // cout << temp[2] << ", ";
     // cout << temp[3] << ", ";
     // cout << temp[4] << ", ";
     // cout << temp[5] << ", ";
     // cout << temp[6] << endl;
    }

 // read the end-of-section byte marker
 // this can be a 0x00 or a 0x01
 uint32 marker = LE_read_uint08(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // test end-of-section byte marker
 if(marker == 0x00) ;
 else if(marker == 0x01) ;
 else return error("Invalid byte marker.", __LINE__);

 // read joint data
 struct ZATJOINTINFO {
  std::string name;
  uint32 size;
  boost::shared_array<char> data;
 };
 deque<ZATJOINTINFO> namelist;
 for(uint32 i = 0; i < n_joints; i++)
    {
     // initialize item
     ZATJOINTINFO info;
     info.size = 0;

     // read name
     char name[255];
     name[0] = '\0';
     if(!read_string(ifile, &name[0], 255)) return error("Read failure.", __LINE__);
     info.name = name; 

     // name aligned to DWORD boundary
     temp = strlen(name) + 1;
     move = ((temp + 0x03) & (~0x03)) - temp;
     if(move) {
        ifile.seekg(move, ios::cur);
        if(ifile.fail()) return error("Seek failure.", __LINE__);
       }

     // read single-byte (0x00 or 0x01)
     uint32 flag = LE_read_uint08(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // read size of data that follows
     info.size = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // read data
     if(info.size != 0) {
        boost::shared_array<char> data(new char[info.size]);
        ifile.read(data.get(), info.size);
        if(ifile.fail()) return error("Read failure.", __LINE__);
        info.data = data;
       }

     // set name
     namelist.push_back(info);
    }

 // after the joint data is read, the rest of the data
 // format is dependent on some flags
 if(unk02 == 0xCB)
   {
    // occurs in both unk03 == 0 and unk03 != 0
    real32 temp[6];
    LE_read_array(ifile, &temp[0], 6);
   }
 else if(unk02 == 0xCD || unk02 == 0x1CD)
   {
    // read size
    uint32 size = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.", __LINE__);

    // size must be valid
    if(!size) return error("Expecting a data size.", __LINE__);

    // read data
    boost::shared_array<char> data(new char[size]);
    ifile.read(data.get(), size);
    if(ifile.fail()) return error("Read failure.", __LINE__);
   }

 // read second index set (these appear to be unique)
 boost::shared_array<uint32> indexset2(new uint32[n_joints]);
 if(!LE_read_array(ifile, indexset2.get(), n_joints)) return error("Read failure.", __LINE__);

 // example:
 // 01 00 00 00
 // 78 53 2A 0A
 // 78 53 2A 0A 00 00 00 00

 // read 0x01
 uint32 p01 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(!(p01 == 0x00 || p01 == 0x01)) return error("Expecting 0x00 or 0x01.", __LINE__);

 // read identifiers
 boost::shared_array<uint32> p02(new uint32[n_joints]);
 if(!LE_read_array(ifile, p02.get(), n_joints)) return error("Read failure.", __LINE__);
 uint32 p03 = LE_read_uint32(ifile); // +2 uint32 items
 uint32 p04 = LE_read_uint32(ifile); // +2 uint32 items
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // example:
 // 03 00 00 00
 // 26 5F 8D 41 3F 00 00 00
 // 26 5F 0D 42 27 00 00 00
 // DD 7E 3C 42 16 00 00 00

 uint32 p05 = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(p05 != 0x03) return error("Expecting 0x03.", __LINE__);

 for(uint32 i = 0; i < p05; i++) {
     real32 item1 = LE_read_real32(ifile);
     uint32 item2 = LE_read_uint32(ifile);
     //cout << " SECTION item: " << item1 << ", " << item2 << endl;
    }
 
 // example:
 if(unk02 == 0x1C9 || unk02 == 0x1CD)
   {
    // read size
    uint32 size = LE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.", __LINE__);

    // size must be valid
    if(!size) return error("Expecting a data size.", __LINE__);

    // read data
    boost::shared_array<char> data(new char[size]);
    ifile.read(data.get(), size);
    if(ifile.fail()) return error("Read failure.", __LINE__);
   }

 // examples:
 // 01 00 00 00 - 2A 00 00 00 - 06 00 00 00
 // 00          - 00          - 01
 // 00 00 00 00 - 00 00 00 00 - 01 00 00 00

 uint32 p06 = LE_read_uint32(ifile);
 uint32 p07 = LE_read_uint32(ifile);
 uint32 p08 = LE_read_uint08(ifile);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // unk01 is usually 0x12, but it can also be 0x13
 // see envs\Undead\NZA3_LVL02\UD_GutsPile.HKSN
 if(unk01 == 0x13) {
    LE_read_uint32(ifile);
   }

 // should be at EOF now
 uint32 position = (uint32)ifile.tellg();
 if(position < filesize) return error("Unexpected data.", __LINE__);

 // initialize skeleton
 ADVANCEDMODELCONTAINER amc;
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.tiplen = 0.01f;
 skel.name = "skeleton";

 // for each joint
 for(uint32 i = 0; i < n_joints; i++)
    {
     // create quaternion
     real32 x = datalist[i][3];
     real32 y = datalist[i][4];
     real32 z = datalist[i][5];
     real32 w = datalist[i][6];
     cs::math::quaternion<real32> q(w, x, y, z);

     // convert quaternion to matrix
     cs::math::matrix4x4<real32> m;
     cs::math::quaternion_to_matrix4x4(&q[0], &m[0]);

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i; // indexset2[i];

     // create and insert joint
     // note that indexset1 is an array of indices that index indexset2 (ID list)
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i; // indexset2[i];
     joint.parent = (i == 0 ? AMC_INVALID_JOINT : indexset1[i]); // (i == 0 ? AMC_INVALID_JOINT : indexset2[indexset1[i]]);
     joint.m_rel[0x0] = m[0x0];
     joint.m_rel[0x1] = m[0x1];
     joint.m_rel[0x2] = m[0x2];
     joint.m_rel[0x3] = m[0x3];
     joint.m_rel[0x4] = m[0x4];
     joint.m_rel[0x5] = m[0x5];
     joint.m_rel[0x6] = m[0x6];
     joint.m_rel[0x7] = m[0x7];
     joint.m_rel[0x8] = m[0x8];
     joint.m_rel[0x9] = m[0x9];
     joint.m_rel[0xA] = m[0xA];
     joint.m_rel[0xB] = m[0xB];
     joint.m_rel[0xC] = m[0xC];
     joint.m_rel[0xD] = m[0xD];
     joint.m_rel[0xE] = m[0xE];
     joint.m_rel[0xF] = m[0xF];
     joint.p_rel[0] = datalist[i][0];
     joint.p_rel[1] = datalist[i][1];
     joint.p_rel[2] = datalist[i][2];
     joint.p_rel[3] = 1.0f;
     skel.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel);
 STDSTRINGSTREAM ss;
 ss << shrtname << TEXT("_skel");
 return SaveAMC(pathname.c_str(), ss.str().c_str(), amc);
}

bool ProcessENV(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.", __LINE__);

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = LE_read_uint32(ifile); // 0x01
 uint32 h02 = LE_read_uint32(ifile); // number of vertices
 uint32 h03 = LE_read_uint32(ifile); // 0x01
 uint32 h04 = LE_read_uint32(ifile); // number of face indices
 uint32 h05 = LE_read_uint32(ifile); // number of 0x40-byte mesh info items
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // validation
 if(h01 != 0x01) return error("Expecting 0x01.", __LINE__);
 if(h02 == 0x00) return true; // nothing to do
 if(h03 != 0x01) return error("Expecting 0x01.", __LINE__);
 if(h04 == 0x00) return true; // nothing to do
 if(h05 == 0x00) return true; // nothing to do

 // read mesh info
 struct ZATMESHINFO {
  uint32 p01[0xA]; // mesh info
  real32 p02[0x6]; // bounding box
 };
 deque<ZATMESHINFO> meshinfo;
 for(uint32 i = 0; i < h05; i++) {
     ZATMESHINFO item;
     if(!LE_read_array(ifile, &item.p01[0], 0xA)) return error("Read failure.", __LINE__);
     if(!LE_read_array(ifile, &item.p02[0], 0x6)) return error("Read failure.", __LINE__);
     meshinfo.push_back(item);
    }

 // variables
 uint32 n_vertices = h02;
 uint32 n_indices = h04;
 uint32 n_meshes = h05;

 // read vertex buffer
 uint32 vb_size = 0x50 * n_vertices;
 boost::shared_array<char> vb_data(new char[vb_size]);
 ifile.read(vb_data.get(), vb_size);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(!vb_size) return error("Invalid vertex buffer data.", __LINE__);

 // read index buffer
 uint32 ib_size = 0x02 * n_indices;
 boost::shared_array<char> ib_data(new char[ib_size]);
 ifile.read(ib_data.get(), ib_size);
 if(ifile.fail()) return error("Read failure.", __LINE__);
 if(!ib_size) return error("Invalid index buffer data.", __LINE__);

 // binary streams
 binary_stream vbs(vb_data, vb_size);
 binary_stream ibs(ib_data, ib_size);

 // model container
 ADVANCEDMODELCONTAINER amc;

 // initialize vertex buffer
 AMC_VTXBUFFER vb;
 vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV;
 vb.name = "default";
 vb.uvchan = 4; // 4 for now
 vb.uvtype[0] = AMC_DIFFUSE_MAP;
 for(uint32 i = 1; i < 8; i++) vb.uvtype[i] = AMC_INVALID_MAP;
 vb.colors = 0;
 vb.elem = n_vertices;
 vb.data.reset(new AMC_VERTEX[vb.elem]);

 // process vertices
 for(uint32 i = 0; i < n_vertices; i++)
    {
     // 0x00 - 0x0B
     vb.data[i].vx = vbs.LE_read_real32();
     vb.data[i].vy = vbs.LE_read_real32();
     vb.data[i].vz = vbs.LE_read_real32();
     vb.data[i].vw = 1.0f;
     // 0x0C - 0x11
     vbs.LE_read_uint16();
     vbs.LE_read_uint16();
     vbs.LE_read_uint16();
     // 0x12 - 0x17
     vbs.LE_read_uint16();
     vbs.LE_read_uint16();
     vbs.LE_read_uint16();
     // 0x18 - 0x1D
     vbs.LE_read_uint16();
     vbs.LE_read_uint16();
     vbs.LE_read_uint16();
     // 0x1E - 0x23
     vbs.LE_read_uint16();
     vbs.LE_read_uint16();
     vbs.LE_read_uint16();
     // 0x24 - 0x27
     vbs.LE_read_uint16();
     vbs.LE_read_uint16();
     // 0x28 - 0x37
     vb.data[i].uv[0][0] = vbs.LE_read_real32();
     vb.data[i].uv[0][1] = vbs.LE_read_real32();
     vb.data[i].uv[1][0] = vbs.LE_read_real32();
     vb.data[i].uv[1][1] = vbs.LE_read_real32();
     // 0x38 - 0x3F
     vbs.LE_read_uint32(); // unknown #1
     vbs.LE_read_uint32(); // unknown #2
     // 0x40 - 0x4F
     vb.data[i].uv[2][0] = vbs.LE_read_real32();
     vb.data[i].uv[2][1] = vbs.LE_read_real32();
     vb.data[i].uv[3][0] = vbs.LE_read_real32();
     vb.data[i].uv[3][1] = vbs.LE_read_real32();
     if(vbs.fail()) return error("Stream read failure.", __LINE__);
    }

 // insert vertex buffer
 amc.vblist.push_back(vb);

 // initialize index buffer
 AMC_IDXBUFFER ib;
 ib.format = AMC_UINT16;
 ib.type = AMC_TRIANGLES;
 ib.wind = AMC_CW;
 ib.name = "default";
 ib.elem = n_indices;
 ib.data = ib_data;

 // insert index buffer
 amc.iblist.push_back(ib);

 // process meshlist
 for(uint32 i = 0; i < meshinfo.size(); i++)
    {
     // initialize surface
     AMC_SURFACE s;
     stringstream sn;
     sn << "surface_" << setfill('0') << setw(3) << i;
     s.name = sn.str();
     AMC_REFERENCE ref;
     ref.vb_index = 0;
     ref.vb_start = meshinfo[i].p01[4];
     ref.vb_width = meshinfo[i].p01[2];
     ref.ib_index = 0;
     ref.ib_start = meshinfo[i].p01[5];
     ref.ib_width = meshinfo[i].p01[3];
     ref.jm_index = AMC_INVALID_JMAP_INDEX;
     s.refs.push_back(ref);
     s.surfmat = AMC_INVALID_SURFMAT;
     
     // insert surface
     if(ref.ib_width % 3) return error("Indices must be divisible by three.", __LINE__);
     amc.surfaces.push_back(s);
    }

 ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
 bool result1 = SaveAMC(pathname.c_str(), shrtname.c_str(), transformed);
 bool result2 = SaveOBJ(pathname.c_str(), shrtname.c_str(), transformed);
 return (result1 && result2);
}

}} // namespace X_SYSTEM::X_GAME

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Zombie Army Trilogy");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Copy game content into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where game content files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a while.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need 21 GB free space.\n");
 p3 += TEXT("3. 8 GB for game + 13 GB extraction.\n");

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
 using namespace std;
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // booleans
 bool doASR = true;
 bool doMDL = true;
 bool doSKN = true;
 bool doENV = true;

 // questions
 if(doASR) doASR = YesNoBox("Process ASR (archive) files?");
 if(doMDL) doMDL = YesNoBox("Process MDL (model) files?");
 if(doSKN) doSKN = YesNoBox("Process HKSN (skeleton) files?");
 if(doENV) doENV = YesNoBox("Process ENV (environment) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 if(doASR)
   {
    // build filename list
    wcout << "Searching for .ASR files... please wait." << endl;
    deque<STDSTRING> filelist;
    if(!BuildFilenameList(filelist, TEXT("ASR"), pathname.c_str())) return true;

    // process files
    bool break_on_errors = true;
    wcout << "Processing .ASR files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessArchive(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;

    // build filename list
    wcout << "Searching for .ASR_EN files... please wait." << endl;
    filelist.clear();
    if(!BuildFilenameList(filelist, TEXT("ASR_EN"), pathname.c_str())) return true;
    
    // process files
    wcout << "Processing .ASR_EN files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessArchive(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;

    // build filename list
    wcout << "Searching for .PC files... please wait." << endl;
    filelist.clear();
    if(!BuildFilenameList(filelist, TEXT("PC"), pathname.c_str())) return true;
   
    // process files
    wcout << "Processing .PC files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessArchive(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;
   
    // build filename list
    wcout << "Searching for .PC_EN files... please wait." << endl;
    filelist.clear();
    if(!BuildFilenameList(filelist, TEXT("PC_EN"), pathname.c_str())) return true;
   
    // process files
    wcout << "Processing .PC_EN files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessArchive(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;
   
    // build filename list
    wcout << "Searching for .PC_TEXTURES files... please wait." << endl;
    filelist.clear();
    if(!BuildFilenameList(filelist, TEXT("PC_TEXTURES"), pathname.c_str())) return true;
   
    // process files
    wcout << "Processing .PC_TEXTURES files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessArchive(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;
   
    // build filename list
    wcout << "Searching for .MIMG files... please wait." << endl;
    filelist.clear();
    if(!BuildFilenameList(filelist, TEXT("MIMG"), pathname.c_str())) return true;
   
    // process files
    wcout << "Processing .MIMG files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessArchive(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;
   
    // build filename list
    wcout << "Searching for .GUI files... please wait." << endl;
    filelist.clear();
    if(!BuildFilenameList(filelist, TEXT("GUI"), pathname.c_str())) return true;
   
    // process files
    wcout << "Processing .GUI files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessArchive(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;
   }

 if(doMDL)
   {
    // build filename list
    wcout << "Searching for .MDL files... please wait." << endl;
    deque<STDSTRING> filelist;
    if(!BuildFilenameList(filelist, TEXT("MDL"), pathname.c_str())) return true;
 
    // process files
    bool break_on_errors = true;
    wcout << "Processing .MDL files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessMDL(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;
   }

 if(doSKN)
   {
    // build filename list
    wcout << "Searching for .HKSN files... please wait." << endl;
    deque<STDSTRING> filelist;
    if(!BuildFilenameList(filelist, TEXT("HKSN"), pathname.c_str())) return true;
 
    // process files
    bool break_on_errors = true;
    wcout << "Processing .HKSN files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessSKN(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;
   }

 if(doENV)
   {
    // build filename list
    wcout << "Searching for .ENV files... please wait." << endl;
    deque<STDSTRING> filelist;
    if(!BuildFilenameList(filelist, TEXT("ENV"), pathname.c_str())) return true;
 
    // process files
    bool break_on_errors = true;
    wcout << "Processing .ENV files..." << endl;
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!ProcessENV(filelist[i].c_str())) {
           if(break_on_errors) return false;
          }
       }
    cout << endl;
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

}}