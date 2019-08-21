#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_xbox360.h"
#include "xb360_shhd.h"
using namespace std;

#define X_SYSTEM XBOX360
#define X_GAME   SILENTHILL

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool processXBF(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // nothing to do
 if(filesize < 4) return true;

 // read first four bytes
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // check for XBDECOMPRESS signature
 if(magic != 0x0FF512ED)
    return message("Expecting XBDECOMPRESS signature 0x0FF512ED (skipping file).");

 // close file
 ifile.close();

 // decompress
 bool result = XB360Decompress(NULL, filename);
 if(!result) return error("XBDECOMPRESS failed.");
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

struct MESHDATA {
 uint16 p01;         // usually 0x0120
 uint16 p02;         // length of mesh name
 char p03[MAX_PATH]; // mesh name
};

struct MATERIAL {
 uint16 p01;         // namelength
 char p02[MAX_PATH]; // name 
 uint16 p03;         // format (usually 0x21)
 real32 p04[6];      // ???
};

struct TEXTURE {
 uint08 p01; // 0x06
 uint16 p02; // name length
 char p03[MAX_PATH]; // name
 uint16 p04; // 0x01
 uint16 p05; // filename length
 char p06[MAX_PATH]; // filename
};

struct MODELDATA {
 public :
  bool debug;
  STDSTRING filename;
  STDSTRING pathname;
  STDSTRING shrtname;
  std::ifstream ifile;
  std::ofstream ofile;
  std::ofstream dfile;
 public :
  uint16 h01; // magic
  uint16 h02; // number of meshes
  deque<MESHDATA> meshlist;
 public :
  ADVANCEDMODELCONTAINER amc;
};

struct MDLDATA {
 public :
  bool debug;
  STDSTRING filename;
  STDSTRING pathname;
  STDSTRING shrtname;
  std::ifstream ifile;
  std::ofstream ofile;
  std::ofstream dfile;
 public :
  uint32 h01; // 0x00
  uint32 h02; // ????
  uint32 h03; // ????
  uint32 h04; // offset to some data
  uint32 h05; // offset to some data (sometimes h04 = h05)
  uint32 h06; // offset to some data (always 0x80 bytes)
  uint32 h07; // ????
  uint32 h08; // ????
 public :
};

bool processBMD(LPCTSTR filename)
{
 // filename properties
 MODELDATA md;
 md.debug = true;
 md.filename = filename;
 md.pathname = GetPathnameFromFilename(filename).get();
 md.shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open file
 md.ifile.open(filename, ios::binary);
 if(!md.ifile) return error("Could not open file.");

 // create debug file
 STDSTRING dfname = md.pathname;
 dfname += md.shrtname;
 dfname += TEXT(".txt");
 if(md.debug) md.dfile.open(dfname.c_str());

 // read magic
 md.h01 = BE_read_uint16(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");
 if(md.h01 != 0x0B09) return error("Execting 0x0B09.");

 // read number of meshes
 md.h02 = BE_read_uint16(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");
 if(md.h02 == 0) return true;

 // for each mesh
 for(uint32 i = 0; i < md.h02; i++)
    {
     // read mesh header
     MESHDATA mesh;
     mesh.p01 = BE_read_uint16(md.ifile); // usually 0x0120
     mesh.p02 = BE_read_uint16(md.ifile); // usually 0x0008 length of mesh name?
     if(md.ifile.fail()) return error("Read failure.");

     // read mesh name
     memset(&mesh.p03[0], 0, MAX_PATH);
     if(!BE_read_array(md.ifile, &mesh.p03[0], mesh.p02)) return error("Read failure.");

     // debug
     if(md.debug) {
        md.dfile << "-----------" << endl;
        md.dfile << " MESH 0x" << setfill('0') << setw(2) << hex << i << dec << endl;
        md.dfile << "-----------" << endl;
        md.dfile << endl;
        md.dfile << " p01 = 0x" << hex << mesh.p01 << dec << " (0x0120)" << endl;
        md.dfile << " p02 = 0x" << hex << mesh.p02 << dec << " (mesh name length)" << endl;
        md.dfile << " p03 = " << mesh.p03 << " (mesh name)" << endl;
       }

     // prepare blank vertex buffer
     AMC_VTXBUFFER vb;
     vb.name = "default";
     vb.elem = 0;
     vb.flags = 0;
     vb.uvchan = 0;
     vb.uvtype[0] = AMC_INVALID_MAP;
     vb.uvtype[1] = AMC_INVALID_MAP;
     vb.uvtype[2] = AMC_INVALID_MAP;
     vb.uvtype[3] = AMC_INVALID_MAP;
     vb.uvtype[4] = AMC_INVALID_MAP;
     vb.uvtype[5] = AMC_INVALID_MAP;
     vb.uvtype[6] = AMC_INVALID_MAP;
     vb.uvtype[7] = AMC_INVALID_MAP;
     vb.colors = 0;

     // prepare blank index buffer
     AMC_IDXBUFFER ib;
     ib.format = AMC_UINT16;
     ib.type = AMC_TRIANGLES;
     ib.wind = AMC_CCW;
     ib.name = "default";
     ib.elem = 0;

     // prepare bland material
     MATERIAL mat;

     // loop buffers
     for(;;)
        {
         // read buffer type
         uint08 type = BE_read_uint08(md.ifile); // 0x02, 0x08
         if(md.ifile.fail()) return error("Read failure.");

         // 0x02
         // POSITION
         // n_items * real32[3] bytes
         if(type == 0x02)
           {
            // read number of items
            uint16 n_items = BE_read_uint16(md.ifile);
            if(md.ifile.fail()) return error("Read failure.");

            // enable position
            vb.flags |= AMC_VERTEX_POSITION;
            vb.elem = n_items;
            vb.data.reset(new AMC_VERTEX[vb.elem]);

            // read points
            for(uint32 j = 0; j < n_items; j++) {
                vb.data[j].vx = BE_read_real32(md.ifile);
                vb.data[j].vy = BE_read_real32(md.ifile);
                vb.data[j].vz = BE_read_real32(md.ifile);
                vb.data[j].vw = 1.0f;
               }

            if(md.debug) {
               md.dfile << " POSITION BUFFER" << endl;
               md.dfile << "  number of vertices = 0x" << hex << n_items << dec << endl;
               for(uint32 j = 0; j < n_items; j++) {
                   md.dfile << "  <" << vb.data[j].vx << ", ";
                   md.dfile << vb.data[j].vy << ", ";
                   md.dfile << vb.data[j].vz << ">" << endl;;
                  }
              }
           }
         // 0x04
         // INDEX BUFFER
         // n_items * uint16[1]
         else if(type == 0x04)
           {
            // read number of items
            uint16 n_items = BE_read_uint16(md.ifile);
            if(md.ifile.fail()) return error("Read failure.");

            // set index buffer
            uint32 size = n_items * sizeof(uint16);
            ib.elem = n_items;
            ib.data.reset(new char[size]);

            // read index buffer
            if(!BE_read_array(md.ifile, reinterpret_cast<uint16*>(ib.data.get()), n_items))
               return error("Read failure.");
           }
         // 0x05
         // TEXCOORD
         else if(type == 0x05)
           {
            // read number of items
            uint16 n_items = BE_read_uint16(md.ifile);
            if(md.ifile.fail()) return error("Read failure.");

            // enable texture coordinates
            vb.flags |= AMC_VERTEX_UV;
            vb.uvchan = 1;
            vb.uvtype[0] = AMC_DIFFUSE_MAP;
            if(vb.elem != n_items) return error("The number of UVs does not match the number of vertices.");

            uint16 unk01 = BE_read_uint16(md.ifile); // usually 0x0001
            for(uint32 j = 0; j < n_items; j++) {
                vb.data[j].uv[0][0] = BE_read_real32(md.ifile);
                vb.data[j].uv[0][1] = BE_read_real32(md.ifile);
               }

            if(md.debug) {
               md.dfile << " TEXCOORD" << endl;
               md.dfile << "  number of vertices = 0x" << hex << n_items << dec << endl;
               md.dfile << "  unknown = 0x" << hex << unk01 << dec << endl;
               for(uint32 j = 0; j < n_items; j++) {
                   md.dfile << "  <" << vb.data[j].uv[0][0] << ", ";
                   md.dfile << vb.data[j].uv[0][1] << ">" << endl;;
                  }
              }
           }
         // 0x07
         // MESH TERMINATOR
         else if(type == 0x07)
           {
            // insert surface
            AMC_SURFACE surface;
            surface.name = mesh.p03;
            surface.surfmat = AMC_INVALID_SURFMAT;
            AMC_REFERENCE ref;
            ref.vb_index = i;
            ref.vb_start = 0;
            ref.vb_width = vb.elem;
            ref.ib_index = i;
            ref.ib_start = 0;
            ref.ib_width = ib.elem;
            ref.jm_index = AMC_INVALID_JMAP_INDEX;
            surface.refs.push_back(ref);
            md.amc.surfaces.push_back(surface);
            break;
           }
         // 0x08
         // UNKNOWN
         // often <0, 0, 0, 1>
         else if(type == 0x08)
           {
            // read number of items
            uint16 n_items = BE_read_uint16(md.ifile);
            if(md.ifile.fail()) return error("Read failure.");

            boost::shared_array<boost::shared_array<real32>> data;
            data.reset(new boost::shared_array<real32>[n_items]);

            for(uint32 j = 0; j < n_items; j++) {
                data[j].reset(new real32[4]);
                data[j][0] = BE_read_real32(md.ifile);
                data[j][1] = BE_read_real32(md.ifile);
                data[j][2] = BE_read_real32(md.ifile);
                data[j][3] = BE_read_real32(md.ifile);
                if(md.ifile.fail()) return error("Read failure.");
               }

            if(md.debug) {
               md.dfile << " UNKNOWN VECTOR4D" << endl;
               md.dfile << "  number of vertices = 0x" << hex << n_items << dec << endl;
               for(uint32 j = 0; j < n_items; j++) {
                   md.dfile << "  <" << data[j][0] << ", ";
                   md.dfile << data[j][1] << ", ";
                   md.dfile << data[j][2] << ", ";
                   md.dfile << data[j][3] << ">" << endl;;
                  }
              }
           }
         // 0x0F
         // BLEND WEIGHTS AND BLEND INDICES
         else if(type == 0x0F)
           {
            // read number of items
            uint16 n_items = BE_read_uint16(md.ifile);
            if(md.ifile.fail()) return error("Read failure.");

            // read number of pairs
            uint16 n_pairs = BE_read_uint16(md.ifile); // usually 0x0001
            if(md.ifile.fail()) return error("Read failure.");

            // number of vertices
            if(!n_pairs) return error("Number of blend pairs cannot be zero.");
            uint16 n_vertices = n_items / n_pairs;

            // enable vertex weights
            vb.flags |= AMC_VERTEX_WEIGHTS;
            if(vb.elem != n_vertices) return error("The number of weights does not match the number of vertices.");

            // allocate data
            typedef std::pair<uint16, real32> pair_t;
            boost::shared_array<boost::shared_array<pair_t>> data;
            data.reset(new boost::shared_array<pair_t>[n_vertices]);
            for(uint32 k = 0; k < n_vertices; k++) data[k].reset(new pair_t[n_pairs]);

            // read data
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // initialize weights
                vb.data[k].wv[0] = 0.0f;
                vb.data[k].wv[1] = 0.0f;
                vb.data[k].wv[2] = 0.0f;
                vb.data[k].wv[2] = 0.0f;
                vb.data[k].wv[3] = 0.0f;
                vb.data[k].wv[4] = 0.0f;
                vb.data[k].wv[5] = 0.0f;
                vb.data[k].wv[6] = 0.0f;
                vb.data[k].wv[7] = 0.0f;
                // initialize indices
                vb.data[k].wi[0] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                vb.data[k].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
                // read data
                for(uint32 m = 0; m < n_pairs; m++)
                   {
                    uint16 bi = BE_read_uint16(md.ifile);
                    real32 bw = BE_read_real32(md.ifile);
                    if(md.ifile.fail()) return error("Read failure.");
                    data[k][m].first = bi;
                    data[k][m].second = bw;
                    // set data
                    vb.data[k].wv[m] = bw;
                    vb.data[k].wi[m] = bi;
                   }
               }

            // debug
            if(md.debug) {
               md.dfile << " BLEND WEIGHTS AND BLEND INDICES" << endl;
               md.dfile << "  number of items = 0x" << hex << n_items << dec << endl;
               md.dfile << "  number of vertices = 0x" << hex << n_vertices << dec << endl;
               md.dfile << "  number of pairs = 0x" << hex << n_pairs << dec << endl;
               for(uint32 j = 0; j < n_vertices; j++) {
                   md.dfile << "  ";
                   for(uint32 k = 0; k < n_pairs; k++)
                       md.dfile << "0x" << setw(2) << hex << data[j][k].first << dec << " ";
                   md.dfile << "- ";
                   for(uint32 k = 0; k < n_pairs; k++)
                       md.dfile << data[j][k].second << " ";
                   md.dfile << endl;
                  }
              }
           }
         // 0x10
         // NORMAL (all points lie on unit sphere)
         // n_items * real32[3]
         else if(type == 0x10)
           {
            // read number of items
            uint16 n_items = BE_read_uint16(md.ifile);
            if(md.ifile.fail()) return error("Read failure.");

            // enable normal
            vb.flags |= AMC_VERTEX_NORMAL;
            if(vb.elem != n_items) return error("The number of normals does not match the number of vertices.");

            // read array of vectors
            for(uint32 j = 0; j < n_items; j++) {
                vb.data[j].nx = BE_read_real32(md.ifile);
                vb.data[j].ny = BE_read_real32(md.ifile);
                vb.data[j].nz = BE_read_real32(md.ifile);
                vb.data[j].nw = 1.0f;
                if(md.ifile.fail()) return error("Read failure.");
               }

            if(md.debug) {
               md.dfile << " NORMAL BUFFER" << endl;
               md.dfile << "  number of vertices = 0x" << hex << n_items << dec << endl;
               for(uint32 j = 0; j < n_items; j++) {
                   md.dfile << "  <" << vb.data[j].nx << ", ";
                   md.dfile << vb.data[j].ny << ", ";
                   md.dfile << vb.data[j].nz << ">" << endl;;
                  }
              }
           }
         // read material
         else if(type == 0x14)
           {
            // read number of items
            uint16 n_items = BE_read_uint16(md.ifile);
            if(md.ifile.fail()) return error("Read failure.");

            // read number of materials and material name
            mat.p01 = n_items;
            memset(&mat.p02[0], 0, MAX_PATH);
            if(!BE_read_array(md.ifile, &mat.p02[0], mat.p01)) return error("Read failure.");

            // read format
            mat.p03 = BE_read_uint08(md.ifile);
            if(md.ifile.fail()) return error("Read failure.");

            if(mat.p03 == 0x21) {
               mat.p04[0] = BE_read_real32(md.ifile);
               mat.p04[1] = BE_read_real32(md.ifile);
               mat.p04[2] = BE_read_real32(md.ifile);
               mat.p04[3] = BE_read_real32(md.ifile);
               mat.p04[4] = BE_read_real32(md.ifile);
               mat.p04[5] = BE_read_real32(md.ifile);
               if(md.ifile.fail()) return error("Read failure.");
              }
            else {
               stringstream ss;
               ss << "Unknown material format 0x" << hex << mat.p03 << dec << ".";
               return error(ss);
              }

            // insert buffers
            md.amc.vblist.push_back(vb);
            md.amc.iblist.push_back(ib);
           }
         // 0x22
         // UNKNOWN
         // n_items * uint16[1]
         else if(type == 0x22)
           {
            // read number of items
            uint16 n_items = BE_read_uint16(md.ifile);
            if(md.ifile.fail()) return error("Read failure.");

            boost::shared_array<uint16> data(new uint16[n_items]);
            BE_read_array(md.ifile, data.get(), n_items);
            if(md.ifile.fail()) return error("Read failure.");

            if(md.debug) {
               md.dfile << " UNKNOWN DATA" << endl;
               md.dfile << "  number of items = 0x" << hex << n_items << dec << endl;
               for(uint32 j = 0; j < n_items; j++)
                   md.dfile << "  0x" << hex << setw(4) << data[j] << dec << endl;
              }
           }
         else {
            stringstream ss;
            ss << "Unknown buffer type 0x" << hex << (uint16)type << dec << ".";
            return error(ss);
           }
        }

     if(md.debug) md.dfile << endl;
    }

 // read texture filename information
 for(;;)
    {
     // read type
     uint08 type = BE_read_uint08(md.ifile);
     if(md.ifile.fail()) return error("Read failure.");

     // TEXTURE ASSOCIATION
     if(type == 0x06)
       {
        // initialize item
        TEXTURE item;
        item.p01 = type;

        // read material name length
        item.p02 = BE_read_uint16(md.ifile);
        if(md.ifile.fail()) return error("Read failure.");

        // read material name
        memset(&item.p03[0], 0, MAX_PATH);
        if(!BE_read_array(md.ifile, &item.p03[0], item.p02)) return error("Read failure.");

        // read 0x01 (number of texture filenames???)
        item.p04 = BE_read_uint16(md.ifile);
        if(md.ifile.fail()) return error("Read failure.");

        // read filename length
        item.p05 = BE_read_uint16(md.ifile);
        if(md.ifile.fail()) return error("Read failure.");

        // read filename
        memset(&item.p06[0], 0, MAX_PATH);
        if(!BE_read_array(md.ifile, &item.p06[0], item.p05)) return error("Read failure.");

        if(md.debug) {
           md.dfile << "TEXTURE INFORMATION" << endl;
           md.dfile << " " << item.p03 << " (material name)" << endl;
           md.dfile << " 0x " << hex << setfill('0') << setw(4) << item.p04 << dec << " (0x01)" << endl;
           md.dfile << " " << item.p06 << " (filename)" << endl;
          }
       }

     // TERMINATOR
     else if(type == 0x0C)
        break;

     // UNKNOWN
     else {
        stringstream ss;
        ss << "Unknown type 0x" << hex << (uint16)type << dec << ".";
        return error(ss);
       }
    }

 // save AMC file
 SaveAMC(md.pathname.c_str(), md.shrtname.c_str(), md.amc);
 SaveOBJ(md.pathname.c_str(), md.shrtname.c_str(), md.amc);
 return true;
}

// C:\Workspace\Xentax\[XBOX360] Silent Hill HD Collection\sh2_360\data\chr2\wp\rwp_chinanife

bool processMDL(LPCTSTR filename)
{
 // filename properties
 MDLDATA md;
 md.debug = true;
 md.filename = filename;
 md.pathname = GetPathnameFromFilename(filename).get();
 md.shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open file
 md.ifile.open(filename, ios::binary);
 if(!md.ifile) return error("Could not open file.");

 // create debug file
 STDSTRING dfname = md.pathname;
 dfname += md.shrtname;
 dfname += TEXT(".txt");
 if(md.debug) md.dfile.open(dfname.c_str());

 // read header
 md.h01 = BE_read_uint32(md.ifile);
 md.h02 = BE_read_uint32(md.ifile);
 md.h03 = BE_read_uint32(md.ifile);
 md.h04 = BE_read_uint32(md.ifile);
 md.h05 = BE_read_uint32(md.ifile);
 md.h06 = BE_read_uint32(md.ifile);
 md.h07 = BE_read_uint32(md.ifile);
 md.h08 = BE_read_uint32(md.ifile);
 if(md.ifile.fail()) return error("Read failure.");

 // move to skeleton information
 md.ifile.seekg(md.h06);
 if(md.ifile.fail()) return error("Seek failure.");

 // read skeleton information (0x80 bytes)
 uint16 si01 = BE_read_uint16(md.ifile); // 0xFFFF
 uint16 si02 = BE_read_uint16(md.ifile); // 0x0003
 uint16 si03 = BE_read_uint32(md.ifile); // ??????
 uint32 si04 = BE_read_uint32(md.ifile); // size of this section (0x80)
 uint32 si05 = BE_read_uint32(md.ifile); // number of joints
 uint32 si06 = BE_read_uint32(md.ifile); // 0x10: offset from 0xFFFF to joint data
 uint32 si07 = BE_read_uint32(md.ifile); // 0x14: number of <index, joint index> pairs
 uint32 si08 = BE_read_uint32(md.ifile); // 0x18: offset from 0xFFFF to pairs
 uint32 si09 = BE_read_uint32(md.ifile); // 0x1C: offset from 0xFFFF to more matrices (n = si07)
 uint32 si10 = BE_read_uint32(md.ifile); // 0x20: number of ???
 uint32 si11 = BE_read_uint32(md.ifile); // 0x24: offset from 0xFFFF to ???
 uint32 si12 = BE_read_uint32(md.ifile); // 0x28: ???
 uint32 si13 = BE_read_uint32(md.ifile); // 0x2C: ???
 uint32 si14 = BE_read_uint32(md.ifile); // 0x30: number of ??? (4 or 8 bytes per entry?)
 uint32 si15 = BE_read_uint32(md.ifile); // 0x34: offset from 0xFFFF to ???
 uint32 si16 = BE_read_uint32(md.ifile); // 0x38: number of ??? (8 bytes per entry)
 uint32 si17 = BE_read_uint32(md.ifile); // 0x3C: offset from 0xFFFF to ???
 if(md.ifile.fail()) return error("Read failure.");

 // move to matrix data
 md.ifile.seekg(md.h06 + si04);
 if(md.ifile.fail()) return error("Seek failure.");
 cout << "offset = 0x" << hex << (md.h06 + si04) << dec << endl;

 // create OBJ file
 STDSTRING ofname = md.pathname;
 ofname += md.shrtname;
 ofname += TEXT(".obj");
 ofstream ofile(ofname.c_str());
 ofile << "o object" << endl;

 // read matrices
 for(uint32 i = 0; i < si05; i++)
    {
     // read matrix
     real32 m[16];
     if(!BE_read_array(md.ifile, &m[0], 16)) return error("Read failure.");

     // transpose matrix
     cs::math::matrix4x4<real32> mat(m);
     mat.transpose();

     ofile << "v " << mat[0x3] << " " << mat[0x7] << " " << mat[0xB] << endl;
    }

 // read parent list
 boost::shared_array<uint08> parents(new uint08[si05]);
 if(!BE_read_array(md.ifile, parents.get(), si05)) return error("Read failure.");

 // render joints
 for(uint32 i = 0; i < si05; i++) {
     if(parents[i] == 0xFF) continue;
     ofile << "f " << (uint16)(parents[i] + 1) << " " << (uint16)(i + 1) << endl;
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
 p1 += TEXT("[XBOX360] Silent Hill HD Collection");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Place unbundler.exe and xbdm.dll in same path as ripper.\n");
 p2 += TEXT("3. Run ripper.\n");
 p2 += TEXT("4. Select game and click Browse.\n");
 p2 += TEXT("5. Select game root directory.\n");
 p2 += TEXT("6. Click OK and answer questions.\n");
 p2 += TEXT("7. Watch console for about an hour.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You must have Unbundler.\n");
 p3 += TEXT("3. Don't ask me for Unbundler; I don't have it.\n");
 p3 += TEXT("4. Find Unbundler yourself.\n");
 p3 += TEXT("2. You need ~15 GB free space.\n");
 p3 += TEXT("3. 5 GB for game + 10 GB extraction.");

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
 bool doXBF = false;
 bool doBMD = false;
 bool doMDL = true;

 // questions
 // if(doXBF) doXBF = YesNoBox("Decompress files?\nClick 'No' if you have already done so.");
 // if(doBMD) doBMD = YesNoBox("Process BMODEL files?\nClick 'No' if you have already done so.");

 // process filetype
 if(doXBF)
   {
    cout << "STAGE 1" << endl;
    cout << "Decompressing .DDS files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".DDS"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 2" << endl;
    cout << "Decompressing .BMODEL files..." << endl;
    filelist.clear();
    BuildFilenameList(filelist, TEXT(".BMODEL"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 3" << endl;
    cout << "Decompressing .MDL files..." << endl;
    BuildFilenameList(filelist, TEXT(".MDL"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 4" << endl;
    cout << "Decompressing .KG1 files..." << endl;
    BuildFilenameList(filelist, TEXT(".KG1"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 5" << endl;
    cout << "Decompressing .ANM files..." << endl;
    BuildFilenameList(filelist, TEXT(".ANM"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 6" << endl;
    cout << "Decompressing .MAP files..." << endl;
    BuildFilenameList(filelist, TEXT(".MAP"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 7" << endl;
    cout << "Decompressing .MDL_ files..." << endl;
    BuildFilenameList(filelist, TEXT(".MDL_"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 8" << endl;
    cout << "Decompressing .TBN2 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TBN2"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 9" << endl;
    cout << "Decompressing .TEX files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 10" << endl;
    cout << "Decompressing .TEX_PAL01 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX_PAL01"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 11" << endl;
    cout << "Decompressing .TEX_PAL02 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX_PAL02"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 12" << endl;
    cout << "Decompressing .TEX_PAL03 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX_PAL03"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 13" << endl;
    cout << "Decompressing .TEX_PAL04 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX_PAL04"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 14" << endl;
    cout << "Decompressing .TEX_PAL05 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX_PAL05"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 15" << endl;
    cout << "Decompressing .TEX_PAL08 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX_PAL08"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 16" << endl;
    cout << "Decompressing .TEX_PAL09 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX_PAL09"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 17" << endl;
    cout << "Decompressing .TEX_PAL10 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX_PAL10"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();

    cout << "STAGE 17" << endl;
    cout << "Decompressing .TEX_PAL11 files..." << endl;
    BuildFilenameList(filelist, TEXT(".TEX_PAL11"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXBF(filelist[i].c_str())) return false;
       }
    cout << endl;
    filelist.clear();
   }

 // process filetype
 if(doBMD) {
    cout << "STAGE 18" << endl;
    cout << "Processing .BMODEL files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BMODEL"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processBMD(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process filetype
 if(doMDL) {
    cout << "STAGE 18" << endl;
    cout << "Processing .MDL files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".MDL"), pathname.c_str());
    sort(filelist.begin(), filelist.end());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processMDL(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 return true;
}

};};