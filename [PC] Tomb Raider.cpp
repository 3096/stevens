#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "[PC] Tomb Raider.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   TOMB_RAIDER

//
// TR2 FILES
//

namespace X_SYSTEM { namespace X_GAME {

bool processTR2(LPCTSTR filename)
{
 bool process_textures = false;
 bool process_rooms = true;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open GDAT file.");

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

 // read version
 uint32 version = bs.read_uint32();
 if(version == 0x2D) ; // TR2
 else if(version == 0xFF080038) ; // TR3
 else if(version == 0xFF180034) ; // TR3
 else if(version == 0xFF180038) ; // TR3
 else return error("Unknown Tomb Raider version.");

 // create palettes
 struct TRRGB { unsigned char color[4]; };
 std::unique_ptr<TRRGB[]> palette1(new TRRGB[256]);
 std::unique_ptr<TRRGB[]> palette2(new TRRGB[256]);

 // read 1st palette
 for(uint32 i = 0; i < 256; i++) {
     bs.read((char*)&palette1[i].color[0], 3);
     palette1[i].color[0] *= 4;
     palette1[i].color[1] *= 4;
     palette1[i].color[2] *= 4;
     palette1[i].color[3] = 255;
     std::swap(palette1[i].color[0], palette1[i].color[2]);
    }

 // read 2st palette
 for(uint32 i = 0; i < 256; i++) {
     bs.read((char*)&palette2[i].color[0], 4);
     palette2[i].color[0] *= 4;
     palette2[i].color[1] *= 4;
     palette2[i].color[2] *= 4;
     palette2[i].color[3] *= 4;
     std::swap(palette2[i].color[0], palette2[i].color[2]);
    }

 // create path to store data
 std::wstringstream ss;
 ss << pathname << shrtname << L"\\";
 std::wstring savepath = ss.str();
 CreateDirectory(ss.str().c_str(), NULL);

 // read textures
 uint32 n_textures = bs.read_uint32();
 if(n_textures)
   {
    const uint32 dx = 256;
    const uint32 dy = 256;
    const uint32 size1 = dx*dy;
    const uint32 size2 = dx*dy*2;
    const uint32 size3 = dx*dy*4;

    boost::shared_array<unsigned char> data1(new unsigned char[size1]);
    boost::shared_array<unsigned char> data2(new unsigned char[size2]);
    boost::shared_array<char> data3(new char[size3]);

    // texture set 1
    for(uint32 i = 0; i < n_textures; i++)
       {
        // read texture
        bs.read((char*)data1.get(), size1);
        if(bs.fail()) return error("Stream read failure.");

        // skip
        if(!process_textures) continue;

        // convert pixel
        uint32 index = 0;
        uint08* src = reinterpret_cast<uint08*>(data1.get());
        uint32* dst = reinterpret_cast<uint32*>(data3.get());
        for(uint32 r = 0; r < dy; r++) {
            for(uint32 c = 0; c < dx; c++) {
                uint32 ref = src[index];
                dst[index] = *(reinterpret_cast<uint32*>(&palette1[ref].color[0]));
                index++;
               }
           }

        // save file
        std::wstringstream out;
        out << pathname << shrtname << L"\\" << "texture_set1_" << i << ".dds";
        DDS_HEADER ddsh;
        CreateB8G8R8A8DDSHeader(dx, dy, 0, FALSE, &ddsh);
        SaveDDSFile(out.str().c_str(), ddsh, data3, size3);
       }

    // texture set 2
    for(uint32 i = 0; i < n_textures; i++)
       {
        // read texture
        bs.read((char*)data2.get(), size2);
        if(bs.fail()) return error("Stream read failure.");

        // skip
        if(!process_textures) continue;

        // convert pixel
        uint32 index = 0;
        uint16* src = reinterpret_cast<uint16*>(data2.get());
        uint32* dst = reinterpret_cast<uint32*>(data3.get());
        for(uint32 r = 0; r < dy; r++) {
            for(uint32 c = 0; c < dx; c++) {
                unsigned char color[4];
                // A
                if(src[index] & 0x8000) color[3] = 255;
                else color[3] = 0;
                // R
                color[2] = 8*((src[index] & 0x7C00) >> 10);
                color[1] = 8*((src[index] & 0x03E0) >> 5);
                color[0] = 8*((src[index] & 0x001F));
                // set
                dst[index] = *(reinterpret_cast<uint32*>(&color[0]));
                index++;
               }
           }

        // save file
        std::wstringstream out;
        out << pathname << shrtname << L"\\" << "texture_set2_" << i << ".dds";
        DDS_HEADER ddsh;
        CreateB8G8R8A8DDSHeader(dx, dy, 0, FALSE, &ddsh);
        SaveDDSFile(out.str().c_str(), ddsh, data3, size3);
       }
   }

 // skip unknown/unused
 bs.move(4);
 if(bs.fail()) return error("Stream seek failure.");

 // create OBJ file
 std::wstring objname = savepath;
 objname += L"rooms.obj";
 std::ofstream ofile(objname.c_str());
 ofile << "o rooms" << endl;
 ofile << "mtllib rooms.mtl" << endl;
 ofile << endl;

 // create MTL file
 std::wstring mtlname = savepath;
 mtlname += L"rooms.mtl";
 std::ofstream mfile(mtlname.c_str());
 for(uint32 i = 0; i < n_textures; i++) {
    }


 // keep track of vertices read
 uint32 vertex_base = 0;

 // read rooms
 uint16 n_rooms = bs.read_uint16();
 if(n_rooms && process_rooms)
   {
    for(uint32 i = 0; i < n_rooms; i++)
       {
        // read room information
        sint32 x_offset = bs.read_sint32();
        sint32 z_offset = bs.read_sint32();
        sint32 y0 = bs.read_sint32();
        sint32 y1 = bs.read_sint32();
        // cout << "x_offset = " << x_offset << endl;
        // cout << "z_offset = " << z_offset << endl;
        // cout << "y0 = " << y0 << endl;
        // cout << "y1 = " << y1 << endl;

        // read word data
        uint32 n_words = bs.read_uint32();

        // read room data
        struct roomvertex {
         real32 position[3];
         sint16 lighting1;
         uint16 flags;
         sint16 lighting2;
        };
        boost::shared_array<roomvertex> verts;
        uint16 n_verts = bs.read_uint16(); // 0xEA
        if(n_verts) {
           verts.reset(new roomvertex[n_verts]);
           for(size_t j = 0; j < n_verts; j++) {
               verts[j].position[0] = ((real32)bs.read_sint16() + (real32)x_offset)/1024.0f;
               verts[j].position[1] = ((real32)bs.read_sint16())/1024.0f;
               verts[j].position[2] = ((real32)bs.read_sint16() + (real32)z_offset)/1024.0f;
               verts[j].lighting1 = bs.read_sint16();
               verts[j].flags = bs.read_uint16();
               verts[j].lighting2 = bs.read_sint16();
               if(bs.fail()) return error("Stream read failure.");
               ofile << "v " << verts[j].position[0] << " ";
               ofile << verts[j].position[1] << " ";
               ofile << verts[j].position[2] << endl;
               ofile << "vt " << 0.0f << " " << 0.0f << endl;
              }
          }

        // reference material
        ofile << "g room_" << setfill('0') << setw(3) << i << endl;
        ofile << "usemtl room_" << setfill('0') << setw(3) << i << endl;

        // define material
        mfile << "newmtl room_" << setfill('0') << setw(3) << i << endl;
        mfile << "map_Kd default.bmp" << endl;
        mfile << endl;

        struct face4 {
         uint16 indices[4];
         uint16 texture; // bit 15 is double-sided flag, bits 0-14 are an index into texture list
        };
        boost::shared_array<face4> rects;
        uint16 n_rects = bs.read_uint16();
        if(n_rects) {
           rects.reset(new face4[n_rects]);
           for(size_t j = 0; j < n_rects; j++) {
               rects[j].indices[0] = bs.read_uint16();
               rects[j].indices[1] = bs.read_uint16();
               rects[j].indices[2] = bs.read_uint16();
               rects[j].indices[3] = bs.read_uint16();
               rects[j].texture = bs.read_uint16();
               if(bs.fail()) return error("Stream read failure.");

               // the low byte is an index into the 256-colour palette, the high byte is in index into the 16-bit palette
               // a textured rectangle will have its vertices mapped onto all 4 vertices of an object texture
               // these are indices into object textures, which haven't been read yet
               uint16 texture = rects[j].texture & 0x7FFF;
               uint16 index08 = texture & 0xFF;
               uint16 index16 = texture >> 8;

               ofile << "f " << (vertex_base + (rects[j].indices[0] + 1)) << " ";
               ofile << (vertex_base + (rects[j].indices[3] + 1)) << " ";
               ofile << (vertex_base + (rects[j].indices[2] + 1)) << endl;

               ofile << "f " << (vertex_base + (rects[j].indices[2] + 1)) << " ";
               ofile << (vertex_base + (rects[j].indices[1] + 1)) << " ";
               ofile << (vertex_base + (rects[j].indices[0] + 1)) << endl;

               // ofile << "f " << (vertex_base + (rects[j].indices[0] + 1)) << " ";
               // ofile << (vertex_base + (rects[j].indices[3] + 1)) << " ";
               // ofile << (vertex_base + (rects[j].indices[2] + 1)) << " ";
               // ofile << (vertex_base + (rects[j].indices[1] + 1)) << endl;
               // if(rects[j].texture & 0x8000) {
               //    ofile << "f " << (vertex_base + (rects[j].indices[0] + 1)) << " ";
               //    ofile << (vertex_base + (rects[j].indices[1] + 1)) << " ";
               //    ofile << (vertex_base + (rects[j].indices[2] + 1)) << " ";
               //    ofile << (vertex_base + (rects[j].indices[3] + 1)) << endl;
               //   }
              }
          }

        struct face3 {
         uint16 indices[3];
         uint16 texture; // bit 15 is double-sided flag, bits 0-14 are an index into texture list
        };
        boost::shared_array<face3> faces;
        uint16 n_faces = bs.read_uint16();
        if(n_faces) {
           faces.reset(new face3[n_faces]);
           for(size_t j = 0; j < n_faces; j++) {
               faces[j].indices[0] = bs.read_uint16();
               faces[j].indices[1] = bs.read_uint16();
               faces[j].indices[2] = bs.read_uint16();
               faces[j].texture = bs.read_uint16();
               if(bs.fail()) return error("Stream read failure.");
               ofile << "f " << (vertex_base + (faces[j].indices[0] + 1)) << " ";
               ofile << (vertex_base + (faces[j].indices[2] + 1)) << " ";
               ofile << (vertex_base + (faces[j].indices[1] + 1)) << endl;
               //if(faces[j].texture & 0x8000) {
               //   ofile << "f " << (vertex_base + (faces[j].indices[0] + 1)) << " ";
               //   ofile << (vertex_base + (faces[j].indices[1] + 1)) << " ";
               //   ofile << (vertex_base + (faces[j].indices[2] + 1)) << endl;
               //  }
              }
          }

        struct sprite {
         sint16 voffset; // offset into vertex list
         sint16 texture; // offset into sprite texture list
        };
        boost::shared_array<sprite> sprites;
        uint16 n_sprites = bs.read_uint16();
        if(n_sprites) {
           sprites.reset(new sprite[n_sprites]);
           for(size_t j = 0; j < n_sprites; j++) {
               sprites[j].voffset = bs.read_sint16();
               sprites[j].texture = bs.read_sint16();
               if(bs.fail()) return error("Stream read failure.");
              }
          }

        // read number of portals
        struct PORTAL {
         uint16 adjroom;
         sint16 normal[3];
         sint16 corners[4][3];
        };
        boost::shared_array<PORTAL> portals;
        uint16 n_portals = bs.read_uint16();
        if(n_portals) {
           portals.reset(new PORTAL[n_portals]);
           for(size_t j = 0; j < n_portals; j++) {
               portals[j].adjroom = bs.read_uint16();
               bs.read_array(&portals[j].normal[0], 3);
               bs.read_array(&portals[j].corners[0][0], 3);
               bs.read_array(&portals[j].corners[1][0], 3);
               bs.read_array(&portals[j].corners[2][0], 3);
               bs.read_array(&portals[j].corners[3][0], 3);
               if(bs.fail()) return error("Stream read failure.");
              }
          }

        // read sector information
        uint16 n_zsectors = bs.read_uint16();
        uint16 n_xsectors = bs.read_uint16();
        if(bs.fail()) return error("Stream read failure.");

        struct SECTOR {
         uint16 floor_data;
         uint16 box;
         uint08 below;
         sint08 floor;
         uint08 above;
         sint08 ceiling;
        };
        boost::shared_array<SECTOR> sectors;
        uint16 n_sectors = n_zsectors*n_xsectors;
        if(n_sectors) {
           sectors.reset(new SECTOR[n_sectors]);
           for(size_t j = 0; j < n_sectors; j++) {
               sectors[j].floor_data = bs.read_uint16();
               sectors[j].box = bs.read_uint16();
               sectors[j].below = bs.read_uint08();
               sectors[j].floor = bs.read_sint08();
               sectors[j].above = bs.read_uint08();
               sectors[j].ceiling = bs.read_sint08();
               if(bs.fail()) return error("Stream read failure.");
              }
          }

        // read light properties
        sint16 ambient1 = bs.read_sint16();
        sint16 ambient2 = bs.read_sint16();
        if(bs.fail()) return error("Stream read failure.");

        // read lights
        struct LIGHT {
         sint32 position[3];
         uint08 color[4];
         uint32 intensity;
         uint32 falloff;
        };
        boost::shared_array<LIGHT> lights;
        uint16 n_lights = bs.read_uint16();
        if(n_lights) {
           lights.reset(new LIGHT[n_lights]);
           for(size_t j = 0; j < n_lights; j++) {
               bs.read_array(&lights[j].position[0], 3);
               bs.read_array(&lights[j].color[0], 4);
               lights[j].intensity = bs.read_uint32();
               lights[j].falloff = bs.read_uint32();
               if(bs.fail()) return error("Stream read failure.");
              }
          }

        // read static meshes
        struct STATICMESH {
         uint32 position[3];
         uint16 rotation;
         uint16 intensity1;
         uint16 intensity2;
         uint16 mesh_id;
        }; 
        boost::shared_array<STATICMESH> smeshlist;
        uint16 n_statics = bs.read_uint16();
        if(n_statics) {
           smeshlist.reset(new STATICMESH[n_statics]);
           for(size_t j = 0; j < n_statics; j++) {
               bs.read_array(&smeshlist[j].position[0], 3);
               smeshlist[j].rotation = bs.read_uint16();
               smeshlist[j].intensity1 = bs.read_uint16();
               smeshlist[j].intensity2 = bs.read_uint16();
               smeshlist[j].mesh_id = bs.read_uint16();
               if(bs.fail()) return error("Stream read failure.");
              }
          }

        // read flags
        sint16 alternate_room = bs.read_sint16();
        sint16 flags = bs.read_sint16();
        uint08 water = bs.read_uint08();
        uint08 reverb = bs.read_uint08();
        uint08 unused = bs.read_uint08();
        if(bs.fail()) return error("Stream read failure.");

        // increment vertex base
        vertex_base += n_verts;
       }
   }

 cout << "at 0x" << hex << bs.tell() << dec << endl;

 // floor data
 uint32 n_floor = bs.read_uint32();
 boost::shared_array<uint16> floordata;
 if(n_floor) {
    floordata.reset(new uint16[n_floor]);
    bs.read_array(floordata.get(), n_floor);
    if(bs.fail()) return error("Stream read failure.");
   }

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
 p1 += TEXT("[PC] Tomb Raider I - V");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where TR2 files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~? GB free space.\n");
 p3 += TEXT("3. ? GB for game + ? GB extraction.\n");
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
 bool doTR2 = true;

 // questions
 if(doTR2) doTR2 = YesNoBox("Process TR2 files?\nSay 'No' if you have already done this before.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 if(doTR2) {
    cout << "Processing .TR2 files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".TR2"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTR2(filelist[i].c_str())) return false;
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

}};