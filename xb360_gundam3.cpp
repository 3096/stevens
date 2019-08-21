#include "xb360_gundam3.h"

struct G1MG0044_HEADER {
 uint32 platform;
 uint32 unknown1;
 float32 bbox_min[3];
 float32 bbox_max[3];
 uint32 sections;
};

struct G1MG0044_ITEMDATA {
 uint32 type;
 uint32 size;
 boost::shared_array<char> data;
};

struct G1MG0044_VERTEXDATA {
 uint32 unknown1;
 uint32 vertsize;
 uint32 vertices;
 uint32 unknown2;
};

struct G1MG0044_FACEDATA {
 uint32 numindex;
 uint32 datatype;
 uint32 unknown1;
};

bool XB360ExtractMDLGundam3(ifstream& ifile, const deque<size_t>& list);
bool XB360Gundam3_G1MG0044(ifstream& ifile, uint32 model);
bool XB360Gundam3_G1MF0021(ifstream& ifile);

bool XB360Gundam3_G1MG0044(ifstream& ifile, uint32 index)
{
 // read magic
 uint64 magic = BE_read_uint64(ifile); 
 if(magic != 0x47314D4730303434) return error("Expecting G1MG0044.");

 // read size
 uint32 datasize = BE_read_uint32(ifile); 
 if(!datasize) return error("Unexpected G1MG0044 datasize.");

 // read header
 G1MG0044_HEADER head;
 head.platform = BE_read_uint32(ifile);
 head.unknown1 = BE_read_uint32(ifile);
 BE_read_array(ifile, &head.bbox_min[0], 3);
 BE_read_array(ifile, &head.bbox_max[0], 3);
 head.sections = BE_read_uint32(ifile);

 // read sections
 deque<G1MG0044_ITEMDATA> itemdata;
 for(size_t i = 0; i < head.sections; i++) {
     G1MG0044_ITEMDATA item;
     item.type = BE_read_uint32(ifile);
     item.size = BE_read_uint32(ifile);
     item.data.reset(new char[item.size - 0x8]);
     ifile.read(item.data.get(), item.size - 0x8);
     itemdata.push_back(item);
    }

 deque<VERTEX_BUFFER> vdlist;
 deque<INDEX_BUFFER> fdlist;

 // for each section
 for(size_t i = 0; i < itemdata.size(); i++)
    {
     // vertex section
     if(itemdata[i].type == 0x00010004)
       {
        // stringstream from data
        stringstream ss(ios_base::out | ios_base::in | ios_base::binary);
        ss.write(itemdata[i].data.get(), itemdata[i].size - 0x8);
        ss.seekg(0);

        // read vertex sections
        uint32 n_meshes = BE_read_uint32(ss);
        for(size_t j = 0; j < n_meshes; j++)
           {
            // read mesh vertex info
            G1MG0044_VERTEXDATA vd;
            vd.unknown1 = BE_read_uint32(ss);
            vd.vertsize = BE_read_uint32(ss);
            vd.vertices = BE_read_uint32(ss);
            vd.unknown2 = BE_read_uint32(ss);

            // set vertex buffer properties
            VERTEX_BUFFER vb;
            vb.elem = vd.vertices;
            vb.flags = 0;
            vb.data.reset(new VERTEX[vd.vertices]);

            // set vertex buffer flags
            if(vd.vertsize == 0x10) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x14) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x18) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
              }
            else if(vd.vertsize == 0x1C) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x20) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x24) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else
               return error("Unknown vertex format.");

            // read vertices
            for(size_t k = 0; k < vd.vertices; k++)
               {
                VERTEX vertex;
                if(vd.vertsize == 0x10) {
                   vertex.vx = BE_read_float16(ss);
                   vertex.vy = BE_read_float16(ss);
                   vertex.vz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x14) {
                   vertex.vx = BE_read_float16(ss);
                   vertex.vy = BE_read_float16(ss);
                   vertex.vz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x18) {
                   vertex.vx = BE_read_float32(ss);
                   vertex.vy = BE_read_float32(ss);
                   vertex.vz = BE_read_float32(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float32(ss);
                  }
                else if(vd.vertsize == 0x1C) {
                   vertex.vx = BE_read_float32(ss);
                   vertex.vy = BE_read_float32(ss);
                   vertex.vz = BE_read_float32(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float32(ss);
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x20) {
                   vertex.vx = BE_read_float32(ss);
                   vertex.vy = BE_read_float32(ss);
                   vertex.vz = BE_read_float32(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float32(ss);
                   BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x24) {
                   vertex.vx = BE_read_float32(ss);
                   vertex.vy = BE_read_float32(ss);
                   vertex.vz = BE_read_float32(ss);
                   vertex.nx = BE_read_float32(ss);
                   vertex.ny = BE_read_float32(ss);
                   vertex.nz = BE_read_float32(ss);
                   BE_read_float32(ss);
                   vertex.tu = BE_read_float32(ss);
                   vertex.tv = BE_read_float32(ss);
                  }

                // save vertex
                vb.data[k] = vertex;
               }

            // save vertex data
            cout << " vertices = " << vd.vertices << endl;
            vdlist.push_back(vb);
           }
       }
     // index buffer section
     else if(itemdata[i].type == 0x00010007)
       {
        // stringstream from data
        stringstream ss(ios_base::out | ios_base::in | ios_base::binary);
        ss.write(itemdata[i].data.get(), itemdata[i].size - 0x8);
        ss.seekg(0);

        // read face sections
        uint32 n_meshes = BE_read_uint32(ss);
        uint32 vb_index = 0;
        for(size_t j = 0; j < n_meshes; j++)
           {
            // read face data
            G1MG0044_FACEDATA fd;
            fd.numindex = BE_read_uint32(ss);
            fd.datatype = BE_read_uint32(ss);
            fd.unknown1 = BE_read_uint32(ss);

            // set index buffer properties
            INDEX_BUFFER ib;
            ib.type = FACE_TYPE_TRISTRIP;
            ib.elem = fd.numindex;
            if(fd.datatype == 0x10) ib.format = FACE_FORMAT_UINT_16;
            else if(fd.datatype == 0x20) ib.format = FACE_FORMAT_UINT_32;
            else return error("Unknown index buffer data format.");

            // set index buffer name
            stringstream surface;
            surface << "CNS_G1MG_" << setfill('0') << setw(3) << index << "/" << setfill('0') << setw(3) << j << ends;
            ib.name = surface.str();

            // determine index buffer data type size
            unsigned int typesize = 0;
            if(fd.datatype == 0x10) typesize = sizeof(uint16);
            else if(fd.datatype == 0x20) typesize = sizeof(uint32);
            else return error("Unknown index buffer data type.");

            // read face data
            unsigned int total_bytes = ib.elem * typesize;
            ib.data.reset(new char[total_bytes]);
            if(ib.format == FACE_FORMAT_UINT_16) BE_read_array(ss, reinterpret_cast<uint16*>(ib.data.get()), ib.elem);
            else if(ib.format == FACE_FORMAT_UINT_32) BE_read_array(ss, reinterpret_cast<uint32*>(ib.data.get()), ib.elem);

            // test face data
            uint32 min_index = 0;
            uint32 max_index = 0;
            if(ib.format == FACE_FORMAT_UINT_16) {
               uint16 a; minimum(reinterpret_cast<uint16*>(ib.data.get()), ib.elem, a);
               uint16 b; maximum(reinterpret_cast<uint16*>(ib.data.get()), ib.elem, b);
               min_index = a;
               max_index = b;
              }
            else if(ib.format == FACE_FORMAT_UINT_32) {
               uint32 a; minimum(reinterpret_cast<uint32*>(ib.data.get()), ib.elem, a);
               uint32 b; maximum(reinterpret_cast<uint32*>(ib.data.get()), ib.elem, b);
               min_index = a;
               max_index = b;
              }
            cout << " min index = " << min_index << endl;
            cout << " max index = " << max_index << endl;

            // set vertex buffer reference
            if(min_index == 0) vb_index++;
            if(vb_index > 0) ib.reference = vb_index - 1;
            else return error("Unexpected vertex buffer reference.");

            // save face data
            fdlist.push_back(ib);
           }
       }
    }

 cout << " vertex data = " << vdlist.size() << endl;
 cout << " face data = " << fdlist.size() << endl;

 // create filename
 stringstream ofile;
 ofile << setfill('0') << setw(4) << index << ends;
 ofile << "CNS_G1MG_" << setfill('0') << setw(3) << index << ends;

 // save geometry
 //if(!GeometryToOBJ(ofile.str().c_str(), vdlist, fdlist)) return false;
 return true;
}

namespace Xbox360 { namespace DynastyWarriorsGundam3 {

bool ExtractArchiveANS(void)
{
 cout << "Extracting ANS file." << endl;
 ifstream ifile;
 ifile.open("linkdata.ans", ios::binary);
 if(!ifile) return error("Could not open file.");

 // read header
 uint32 headMagic = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headFiles = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headScale = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headUnk01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");

 // validate header
 if(headMagic != 0x1EDE) return error("Invalid header signature.");
 if(headFiles >= 0xFFFF) return error("No dynasty warriors game contains this many files.");
 if(headScale != 0x0800) return error("File assets should occur on 0x800 boundaries.");

 // read file table
 deque<pair<uint32, uint32>> filetable;
 for(size_t i = 0; i < headFiles; i++) {
     pair<uint32, uint32> p;
     p.first  = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     p.second = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     filetable.push_back(p);
    }

 uint32 wave_count = 0;
 uint32 astp_count = 0;
 uint32 arch_count = 0;

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     size_t position = filetable[i].first*headScale;
     size_t filesize = filetable[i].second;
     ifile.seekg(position);
     if(ifile.fail()) return error("Failed to seek file position.");

     // read data
     boost::shared_array<char> data(new char[filesize]);
     ifile.read(data.get(), filesize);
     if(ifile.fail()) return error("Read failure.");

     // save to stringstream
     stringstream ss;
     ss.write(data.get(), filesize);
     if(ss.fail()) return error("Write to stringstream failure.");
     
     // read type of stringstream
     uint32 type = BE_read_uint32(ss);
     if(ifile.fail()) return error("Read failure.");

     // WAVE file
     if(type == 0x52494646) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (WAVE)." << endl;
        stringstream name;
        name << "ANS_" << setfill('0') << setw(4) << wave_count << ".wav" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create wave file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to wave file.");
        wave_count++;
       }
     // ASTP file
     else if(type == 0x41545350) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (ASTP)." << endl;
        stringstream name;
        name << "ANS_" << setfill('0') << setw(4) << astp_count << ".astp" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create astp file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to astp file.");
        astp_count++;
       }
     // ARCH file
     else if(type == 0x2266) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (ARCH)." << endl;
        stringstream name;
        name << "ANS_" << setfill('0') << setw(4) << arch_count << ".arch" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create arch file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to arch file.");
        arch_count++;
       }
     else if(type == 0x5742445F) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (WBD_)." << endl;
     else if(type == 0x5742485F) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (WBH_)." << endl;
     else if(type == 0x04F) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x04F)." << endl;
     else if(type == 0x07D) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x07D)." << endl;
     else if(type == 0x4F0) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x4F0)." << endl;
     else if(type == 0x62C) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x62C)." << endl;
     else return error("Unknown section.");
    }

 cout << endl;
 return true;
}

bool ExtractCNS_G1MG(istream& source, uint32 index)
{
 // read header
 G1MG0044_HEADER head;
 head.platform = BE_read_uint32(source);
 head.unknown1 = BE_read_uint32(source);
 BE_read_array(source, &head.bbox_min[0], 3);
 BE_read_array(source, &head.bbox_max[0], 3);
 head.sections = BE_read_uint32(source);

 // read sections
 deque<G1MG0044_ITEMDATA> itemdata;
 for(size_t i = 0; i < head.sections; i++) {
     G1MG0044_ITEMDATA item;
     item.type = BE_read_uint32(source);
     item.size = BE_read_uint32(source);
     item.data.reset(new char[item.size - 0x8]);
     source.read(item.data.get(), item.size - 0x8);
     itemdata.push_back(item);
    }

 // data to save
 deque<VERTEX_BUFFER> vdlist;
 deque<INDEX_BUFFER> fdlist;

 // process section data
 for(size_t i = 0; i < itemdata.size(); i++)
    {
     // vertex section
     if(itemdata[i].type == 0x00010004)
       {
        // stringstream from data
        stringstream ss(ios_base::out | ios_base::in | ios_base::binary);
        ss.write(itemdata[i].data.get(), itemdata[i].size - 0x8);
        ss.seekg(0);

        // read vertex sections
        uint32 meshes = BE_read_uint32(ss);
        for(size_t j = 0; j < meshes; j++)
           {
            // read mesh vertex info
            G1MG0044_VERTEXDATA vd;
            vd.unknown1 = BE_read_uint32(ss);
            vd.vertsize = BE_read_uint32(ss);
            vd.vertices = BE_read_uint32(ss);
            vd.unknown2 = BE_read_uint32(ss);
                   cout << "VERTEX SIZE = " << vd.vertsize << endl;
            // set vertex buffer properties
            VERTEX_BUFFER vb;
            vb.elem = vd.vertices;
            vb.flags = 0;
            vb.data.reset(new VERTEX[vd.vertices]);

            // set vertex buffer flags
            if(vd.vertsize == 0x10) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x14) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x18) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
              }
            else if(vd.vertsize == 0x1C) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x20) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x24) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else
               return error("Unknown vertex format.");

            // read vertices
            for(size_t k = 0; k < vd.vertices; k++)
               {
                VERTEX vertex;
                if(vd.vertsize == 0x10) {
                   vertex.vx = BE_read_float16(ss);
                   vertex.vy = BE_read_float16(ss);
                   vertex.vz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x14) {
                   vertex.vx = BE_read_float16(ss);
                   vertex.vy = BE_read_float16(ss);
                   vertex.vz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x18) {
                   vertex.vx = BE_read_float32(ss);
                   vertex.vy = BE_read_float32(ss);
                   vertex.vz = BE_read_float32(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float32(ss);
                  }
                else if(vd.vertsize == 0x1C) {
                   vertex.vx = BE_read_float32(ss);
                   vertex.vy = BE_read_float32(ss);
                   vertex.vz = BE_read_float32(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float32(ss);
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x20) {
                   vertex.vx = BE_read_float32(ss);
                   vertex.vy = BE_read_float32(ss);
                   vertex.vz = BE_read_float32(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float32(ss);
                   BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x24) {
                   vertex.vx = BE_read_float32(ss);
                   vertex.vy = BE_read_float32(ss);
                   vertex.vz = BE_read_float32(ss);
                   vertex.nx = BE_read_float32(ss);
                   vertex.ny = BE_read_float32(ss);
                   vertex.nz = BE_read_float32(ss);
                   BE_read_float32(ss);
                   vertex.tu = BE_read_float32(ss);
                   vertex.tv = BE_read_float32(ss);
                  }

                // save vertex
                vb.data[k] = vertex;
               }

            // save vertex data
            cout << " vertices = " << vd.vertices << endl;
            vdlist.push_back(vb);
           }
       }
     // index buffer section
     else if(itemdata[i].type == 0x00010007)
       {
        // stringstream from data
        stringstream ss(ios_base::out | ios_base::in | ios_base::binary);
        ss.write(itemdata[i].data.get(), itemdata[i].size - 0x8);
        ss.seekg(0);

        // read face sections
        uint32 n_meshes = BE_read_uint32(ss);
        uint32 vb_index = 0;
        for(size_t j = 0; j < n_meshes; j++)
           {
            // read face data
            G1MG0044_FACEDATA fd;
            fd.numindex = BE_read_uint32(ss);
            fd.datatype = BE_read_uint32(ss);
            fd.unknown1 = BE_read_uint32(ss);

            // set index buffer properties
            INDEX_BUFFER ib;
            ib.type = FACE_TYPE_TRISTRIP;
            ib.elem = fd.numindex;
            if(fd.datatype == 0x10) ib.format = FACE_FORMAT_UINT_16;
            else if(fd.datatype == 0x20) ib.format = FACE_FORMAT_UINT_32;
            else return error("Unknown index buffer data format.");

            // set index buffer name
            stringstream surface;
            surface << setfill('0') << setw(4) << index << "_" << setw(3) << j << ends;
            ib.name = surface.str();

            // determine index buffer data type size
            unsigned int typesize = 0;
            if(fd.datatype == 0x10) typesize = sizeof(uint16);
            else if(fd.datatype == 0x20) typesize = sizeof(uint32);
            else return error("Unknown index buffer data type.");

            // read face data
            unsigned int total_bytes = ib.elem * typesize;
            ib.data.reset(new char[total_bytes]);
            if(ib.format == FACE_FORMAT_UINT_16) BE_read_array(ss, reinterpret_cast<uint16*>(ib.data.get()), ib.elem);
            else if(ib.format == FACE_FORMAT_UINT_32) BE_read_array(ss, reinterpret_cast<uint32*>(ib.data.get()), ib.elem);

            // test face data
            uint32 min_index = 0;
            uint32 max_index = 0;
            if(ib.format == FACE_FORMAT_UINT_16) {
               uint16 a; minimum(reinterpret_cast<uint16*>(ib.data.get()), ib.elem, a);
               uint16 b; maximum(reinterpret_cast<uint16*>(ib.data.get()), ib.elem, b);
               min_index = a;
               max_index = b;
              }
            else if(ib.format == FACE_FORMAT_UINT_32) {
               uint32 a; minimum(reinterpret_cast<uint32*>(ib.data.get()), ib.elem, a);
               uint32 b; maximum(reinterpret_cast<uint32*>(ib.data.get()), ib.elem, b);
               min_index = a;
               max_index = b;
              }
            cout << " min index = " << min_index << endl;
            cout << " max index = " << max_index << endl;

            // set vertex buffer reference
            if(min_index == 0) vb_index++;
            if(vb_index > 0) ib.reference = vb_index - 1;
            else return error("Unexpected vertex buffer reference.");

            // save face data
            fdlist.push_back(ib);
           }
       }
    }

 cout << " vertex data = " << vdlist.size() << endl;
 cout << " face data = " << fdlist.size() << endl;

 // create filename
 stringstream ofile;
 ofile << "CSN_G1MG_" << setfill('0') << setw(4) << index << ends;

 // save geometry
 //if(!GeometryToOBJ(ofile.str().c_str(), vdlist, fdlist)) return false;
 return true;
}

bool ExtractCNS_G1MX(uint32 index, uint32 position, uint32 datasize, uint32 nextsize, boost::shared_array<char> data, boost::shared_array<char> next)
{
 // create a folder for this file
 stringstream name;
 name << "CNS_G1MX_" << setfill('0') << setw(3) << index << ends;
 CreateDirectoryA(name.str().c_str(), NULL);

 // read data into stringstream
 stringstream ss_data(ios::in | ios::out | ios::binary);
 ss_data.write(data.get(), datasize);
 if(ss_data.fail()) return error("Stream write failure.");

 // read next into stringstream
 stringstream ss_next(ios::in | ios::out | ios::binary);
 ss_next.write(next.get(), nextsize);
 if(ss_next.fail()) return error("Stream write failure.");

 struct G1M_HEADER {
  uint32 magic;
  uint32 version;
  uint32 size;
  uint32 offset;
  uint32 unknown1;
  uint32 sections;
 };

 // read header
 G1M_HEADER head;
 head.magic    = BE_read_uint32(ss_data); if(ss_data.fail()) return error("Stream read failure.");
 head.version  = BE_read_uint32(ss_data); if(ss_data.fail()) return error("Stream read failure.");
 head.size     = BE_read_uint32(ss_data); if(ss_data.fail()) return error("Stream read failure.");
 head.offset   = BE_read_uint32(ss_data); if(ss_data.fail()) return error("Stream read failure.");
 head.unknown1 = BE_read_uint32(ss_data); if(ss_data.fail()) return error("Stream read failure.");
 head.sections = BE_read_uint32(ss_data); if(ss_data.fail()) return error("Stream read failure.");

 // validate header
 if(head.magic != 0x47314D5F) return error("Expecting G1M_ section.");
 if(head.version != 0x30303334) return error("Invalid G1M_ version.");
 if(!head.size) return error("Invalid G1M_ data size.");
 if(!head.offset) return error("Invalid G1M_ data offset.");
 if(!head.sections) return error("Expecting at least one G1M_ section.");

 // read sections
 for(size_t i = 0; i < head.sections; i++)
    {
     uint32 section_name = BE_read_uint32(ss_data); if(ss_data.fail()) return error("Stream read failure.");
     uint32 section_vers = BE_read_uint32(ss_data); if(ss_data.fail()) return error("Stream read failure.");
     uint32 section_size = BE_read_uint32(ss_data); if(ss_data.fail()) return error("Stream read failure.");

     // G1MF
     if(section_name == 0x47314D46) {
        ss_data.seekg(section_size - 0xC, ios::cur);
        if(ss_data.fail()) return error("Stream seek failure.");
       }
     // G1MS
     else if(section_name == 0x47314D53) {
        ss_data.seekg(section_size - 0xC, ios::cur);
        if(ss_data.fail()) return error("Stream seek failure.");
       }
     // G1MM
     else if(section_name == 0x47314D4D) {
        ss_data.seekg(section_size - 0xC, ios::cur);
        if(ss_data.fail()) return error("Stream seek failure.");
       }
     // EXTR
     else if(section_name == 0x45585452) {
        ss_data.seekg(section_size - 0xC, ios::cur);
        if(ss_data.fail()) return error("Stream seek failure.");
       }
     // G1MG
     else if(section_name == 0x47314D47) {
        size_t save = (size_t)ss_data.tellg();
        if(!ExtractCNS_G1MG(ss_data, index)) return false;
        ss_data.seekg(save);
        ss_data.seekg(section_size - 0xC, ios::cur);
        if(ss_data.fail()) return error("Stream seek failure.");
       }
     else
        return error("Unknown G1M_ section.");
    }

 return true;
}

bool ExtractCNS_G1TG(uint32 index, uint32 position, uint32 datasize, uint32 nextsize, boost::shared_array<char> data, boost::shared_array<char> next)
{
 // create a folder for this file
 stringstream name;
 name << "CNS_G1TG_" << setfill('0') << setw(3) << index << ends;
 CreateDirectoryA(name.str().c_str(), NULL);

 // read data into stringstream
 stringstream ss_data(ios::in | ios::out | ios::binary);
 ss_data.write(data.get(), datasize);
 if(ss_data.fail()) return error("Stream write failure.");

 // read next into stringstream
 stringstream ss_next(ios::in | ios::out | ios::binary);
 ss_next.write(next.get(), nextsize);
 if(ss_next.fail()) return error("Stream write failure.");

 // read header
 uint32 magic = BE_read_uint32(ss_data);
 uint32 version = BE_read_uint32(ss_data);
 uint32 section_size = BE_read_uint32(ss_data);
 uint32 table_offset = BE_read_uint32(ss_data);
 uint32 n_textures = BE_read_uint32(ss_data);

 // validate header
 if(magic != 0x47315447) return error("Expecting G1TG section.");
 if(!(version == 0x30303530 || version == 0x30303630)) return error("Invalid G1TG version.");
 if(n_textures == 0) return error("Invalid number of textures.");

 // move to table
 ss_data.seekg(table_offset);
 if(ss_data.fail()) return error("Stream seek failure.");

 // read offset table
 deque<size_t> offset_list;
 for(size_t i = 0; i < n_textures; i++) {
     offset_list.push_back(BE_read_uint32(ss_data));
     if(ss_data.fail()) return error("Stream read failure.");
    }

 // process textures
 for(size_t i = 0; i < offset_list.size(); i++)
    {
     ss_data.seekg(table_offset + offset_list[i]);
     if(ss_data.fail()) return error("Stream seek failure.");

     // read texture information
     uint08 b1 = BE_read_uint08(ss_data); // unknown
     uint08 b2 = BE_read_uint08(ss_data); // texture type
     uint08 b3 = BE_read_uint08(ss_data); // dx/dy
     uint08 b4 = BE_read_uint08(ss_data); // unknown
     uint16 s1 = BE_read_uint16(ss_data); // unknown
     uint16 s2 = BE_read_uint16(ss_data); // unknown

     // header is extended depending on version
     if(version == 0x30303630) {
        uint32 v1 = BE_read_uint32(ss_data); // 12
        uint32 v2 = BE_read_uint32(ss_data); // 0
        uint32 v3 = BE_read_uint32(ss_data); // 0 or 1
       }

     // compute dimensions
     uint32 temp1 = ((b3 & 0xF0) >> 4);
     uint32 temp2 = ((b3 & 0x0F));
     uint32 dx = 1 << temp1;
     uint32 dy = 1 << temp2;

     // create texture file
     stringstream filename;
     filename << "CNS_G1TG_" << setfill('0') << setw(3) << index << "/" << setfill('0') << setw(3) << i << ".dds" << ends;
     ofstream ofile(filename.str().c_str(), ios::binary);

     // save texture
     if(b2 == 0)
       {
        // read texture
        DWORD filesize = UncompressedDDSFileSize(dx, dy, 0, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        boost::shared_array<char> buffer(new char[filesize]);
        ss_data.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, 0, 0xFF000000, 0xFF0000, 0xFF00, 0xFF, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);

        cout << filename.str() << endl;
        cout << " dx = " << dx << endl;
        cout << " dy = " << dy << endl;
        cout << " filesize = " << filesize << endl;
        if(i != offset_list.size() - 1) cout << " expected = " << (offset_list[i + 1] - offset_list[i] - 0x14) << endl;
       }
     // UNCOMPRESSED DDS (PS3 works good)
     else if(b2 == 1)
       {
        // read texture
        DWORD filesize = UncompressedDDSFileSize(dx, dy, 0, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        boost::shared_array<char> buffer(new char[filesize]);
        ss_data.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, 0, 0xFF000000, 0xFF0000, 0xFF00, 0xFF, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // DXT1 (PS3 works good)
     else if(b2 == 6)
       {
        // read texture
        DWORD filesize = DXT1Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ss_data.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, 0, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // DXT5 (PS3 works good)
     else if(b2 == 8)
       {
        // read texture
        DWORD filesize = DXT5Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ss_data.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, 0, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     else {
        return error("Unsupported texture type.");
       }
    }

 return true;
}

bool ExtractArchiveBNS(void)
{
 // set EXE pathname
 char c_pathname[1024];
 GetModulePathname(c_pathname, 1024);
 string s_pathname(c_pathname);

 // open BNS file
 cout << "Extracting BNS file." << endl;
 stringstream filename;
 filename << s_pathname << "linkdata.bns" << ends;
 ifstream ifile;
 ifile.open(filename.str().c_str(), ios::binary);
 if(!ifile) return error("Could not open file.");

 // create subdirectory to store files
 string datapath = s_pathname;
 datapath += "LINKDATA_BNS\\";
 CreateDirectoryA(datapath.c_str(), NULL);

 // read header
 uint32 headMagic = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headFiles = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headScale = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headUnk01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");

 // validate header
 if(headMagic != 0x1EDE) return error("Invalid header signature.");
 if(headFiles >= 0xFFFF) return error("No dynasty warriors game contains this many files.");
 if(headScale != 0x0800) return error("File assets should occur on 0x800 boundaries.");

 // read file table
 deque<pair<uint32, uint32>> filetable;
 for(size_t i = 0; i < headFiles; i++) {
     pair<uint32, uint32> p;
     p.first  = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     p.second = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     filetable.push_back(p);
    }

 uint32 ping_count = 0;
 uint32 file_count = 0;
 deque<string> filelist;

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     size_t position = filetable[i].first*headScale;
     size_t filesize = filetable[i].second;
     ifile.seekg(position);
     if(ifile.fail()) return error("Failed to seek file position.");

     // read data
     boost::shared_array<char> data(new char[filesize]);
     ifile.read(data.get(), filesize);
     if(ifile.fail()) return error("Read data failure.");

     // extract type from data
     stringstream ss;
     ss.write(data.get(), 4);
     if(ss.fail()) return error("Write to stringstream failure.");
     uint32 type = BE_read_uint32(ss);
     if(ss.fail()) return error("Read from stream failure.");

     // save textures and models
     binary_stream bs(data, filesize);
     if((bs.search("G1TG00", 6) != binary_stream::npos) || (bs.search("G1M_00", 6) != binary_stream::npos))
       {
        // display some information
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (BIN)." << endl;
        file_count++;
        
        // save file data
        stringstream binfile;
        binfile << datapath.c_str() << setfill('0') << setw(3) << file_count << ".bin" << ends;
        ofstream ofile(binfile.str(), ios::binary);
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Write failure.");
        
        // save file list
        filelist.push_back(binfile.str());
       }
    }

 // process each file
 for(size_t i = 0; i < filelist.size(); i++)
    {
     // open file
     cout << "Processing " << filelist[i].c_str() << "." << endl;
     ifstream ifile(filelist[i].c_str(), ios::binary);
     if(!ifile) return error("Error opening file.");

     // read file
     ifile.seekg(0, ios::end);
     size_t size = (size_t)ifile.tellg();
     ifile.seekg(0, ios::beg);
     boost::shared_array<char> data(new char[size]);
     ifile.read(data.get(), size);

     // read type
     binary_stream bs(data, size);
     uint32 type = bs.BE_read_uint32();
     bs.seek(0);

     // G1TG (textures)
     if(type == 0x47315447)
       {
        // make directory
        char c_drive[1024];
        char c_path[1024];
        char c_name[1024];
        _splitpath(filelist[i].c_str(), c_drive, c_path, c_name, nullptr);
        string pathname(c_drive);
        pathname += c_path;
        pathname += c_name;
        pathname += "\\";

        // make directory
        CreateDirectoryA(pathname.c_str(), NULL);

        // extract textures
        ExtractG1TG0060(bs, pathname.c_str(), 0);
       }
     else if(type == 0x1EDE)
       {
        // make directory
        char c_drive[1024];
        char c_path[1024];
        char c_name[1024];
        _splitpath(filelist[i].c_str(), c_drive, c_path, c_name, nullptr);
        string pathname(c_drive);
        pathname += c_path;
        pathname += c_name;
        pathname += "\\";

        // make directory
        CreateDirectoryA(pathname.c_str(), NULL);

        // extract textures
        size_t position = bs.search("G1TG00", 6);
        size_t index = 0;
        while(position != binary_stream::npos) {
              binary_stream temp(bs);
              temp.seek(position);
              ExtractG1TG0060(temp, pathname.c_str(), ++index);
              position = bs.search("G1TG00", 6, position + 1);
              if(position == binary_stream::npos) break;
             }
       }
     else if(type >= 0x1 && type <= 0xFF)
       {
        // make directory
        char c_drive[1024];
        char c_path[1024];
        char c_name[1024];
        _splitpath(filelist[i].c_str(), c_drive, c_path, c_name, nullptr);
        string pathname(c_drive);
        pathname += c_path;
        pathname += c_name;
        pathname += "\\";

        // make directory
        CreateDirectoryA(pathname.c_str(), NULL);

        // extract textures
        size_t position = bs.search("G1TG00", 6);
        size_t index = 0;
        while(position != binary_stream::npos) {
              binary_stream temp(bs);
              temp.seek(position);
              ExtractG1TG0060(temp, pathname.c_str(), ++index);
              position = bs.search("G1TG00", 6, position + 1);
              if(position == binary_stream::npos) break;
             }

        // extract models
        position = bs.search("G1MG00", 6);
        index = 0;
        while(position != binary_stream::npos) {
              binary_stream temp(bs);
              temp.seek(position);
              ExtractModels(temp, pathname.c_str(), ++index);
              position = bs.search("G1MG00", 6, position + 1);
              if(position == binary_stream::npos) break;
             }
       }
    }

 cout << endl;
 return true;
}

bool ExtractArchiveCNS(void)
{
 cout << "Extracting CNS file." << endl;
 ifstream ifile;
 ifile.open("linkdata.cns", ios::binary);
 if(!ifile) return error("Could not open file.");

 // create subdirectory to store files
 CreateDirectoryA("LINKDATA_CNS", NULL);

 // read header
 uint32 headMagic = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headFiles = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headScale = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headUnk01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");

 // validate header
 if(headMagic != 0x1EDE) return error("Invalid header signature.");
 if(headFiles >= 0xFFFF) return error("No dynasty warriors game contains this many files.");
 if(headScale != 0x0800) return error("File assets should occur on 0x800 boundaries.");

 // read file table
 deque<pair<uint32, uint32>> filetable;
 for(size_t i = 0; i < headFiles; i++) {
     pair<uint32, uint32> p;
     p.first  = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     p.second = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     filetable.push_back(p);
    }

 uint32 g1tg_count = 0;
 uint32 g1mx_count = 0;
 uint32 ping_count = 0;
 uint32 d002_count = 0;
 uint32 d003_count = 0;

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     size_t position = filetable[i].first*headScale;
     size_t filesize = filetable[i].second;
     ifile.seekg(position);
     if(ifile.fail()) return error("Failed to seek file position.");

     // read data
     boost::shared_array<char> data(new char[filesize]);
     ifile.read(data.get(), filesize);
     if(ifile.fail()) return error("Read data failure.");

     // read fill data
     boost::shared_array<char> fill;
     uint32 fillsize = 0;
     if((i != (filetable.size() - 1))) {
        uint32 p0 = filetable[i + 0].first*headScale + filetable[i].second;
        uint32 p1 = filetable[i + 1].first*headScale;
        if(p0 < p1) {
           fillsize = p1 - p0;
           fill.reset(new char[fillsize]);
           ifile.read(fill.get(), fillsize);
           if(ifile.fail()) { cout << "fillsize = " << fillsize << endl; return error("Read fill failure."); }
          }
       }

     // extract type from data
     stringstream ss;
     ss.write(data.get(), 4);
     if(ss.fail()) return error("Write to stringstream failure.");
     uint32 type = BE_read_uint32(ss);
     if(ss.fail()) return error("Read from stream failure.");
     
     // G1TG
     if(type == 0x47315447)
       {
        // display some information
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (G1TG)." << endl;
        g1tg_count++;

        // save file data
        stringstream name;
        name << "LINKDATA_CNS/GITG_" << setfill('0') << setw(3) << g1tg_count << ".g1tg" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Write failure.");

        // save fill data
        ofile.write(fill.get(), fillsize);
        if(ofile.fail()) return error("Write failure.");
       }
     // G1M_
     else if(type == 0x47314D5F)
       {
        // display some information
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (G1M_)." << endl;
        g1mx_count++;

        // save file data
        stringstream name;
        name << "LINKDATA_CNS/GIM_" << setfill('0') << setw(3) << g1mx_count << ".g1m" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Write failure.");

        // save fill data
        ofile.write(fill.get(), fillsize);
        if(ofile.fail()) return error("Write failure.");
       }
     // PING
     else if(type == 0x89504E47)
       {
        // display some information
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (PING)." << endl;
        ping_count++;

        // save file data
        stringstream name;
        name << "LINKDATA_CNS/PNG_" << setfill('0') << setw(3) << ping_count << ".png" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Error creating PNG file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Error writing PNG file.");
       }
     else if(type == 0x02)
       {
        // display some information
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (002)." << endl;
        d002_count++;

        // save file data
        stringstream name;
        name << "LINKDATA_CNS/0002_" << setfill('0') << setw(3) << d002_count << ".002" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Write failure.");

        // save fill data
        ofile.write(fill.get(), fillsize);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(type == 0x03)
       {
        // display some information
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (003)." << endl;
        d003_count++;

        // save file data
        stringstream name;
        name << "LINKDATA_CNS/0003_" << setfill('0') << setw(3) << d003_count << ".003" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Write failure.");

        // save fill data
        ofile.write(fill.get(), fillsize);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(type == 0x4B53484C) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (KSHL)." << endl;
     else if(type == 0x5356445F) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (SVD_)." << endl;
     else if(type >= 0x10000 && type <= 0xFFFFF) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x10000 - 0xFFFFF)." << endl;
     else if(type == 0x0B) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x0B)." << endl;
     else if(type == 0x10) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x10)." << endl;
     else if(type == 0x1B) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x1B)." << endl;
     else {
        cout << "curr position = " << filetable[i].first*headScale << endl;
        cout << "curr filesize = " << filetable[i].second << endl;
        cout << "next position = " << filetable[i + 1].first*headScale << endl;
        cout << "next filesize = " << filetable[i + 1].second << endl;
        return false;
       }
    }

 cout << endl;
 return true;
}

bool ExtractArchive(const char* filename)
{
/*
 uint32 g1tg_count = 0;
 uint32 g1em_count = 0;
 uint32 g1m__count = 0;
 uint32 ping_count = 0;
 uint32 d002_count = 0;
 uint32 d003_count = 0;

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     size_t position = filetable[i].first*headScale;
     size_t filesize = filetable[i].second;
     ifile.seekg(position);
     if(ifile.fail()) return error("Failed to seek file position.");

     // read data
     boost::shared_array<char> data(new char[filesize]);
     ifile.read(data.get(), filesize);
     if(ifile.fail()) return error("Read failure.");

     // save to stringstream
     stringstream ss;
     ss.write(data.get(), filesize);
     if(ss.fail()) return error("Write to stringstream failure.");
     
     // read type of stringstream
     uint32 type = BE_read_uint32(ss);
     if(ifile.fail()) return error("Read failure.");

     // WAVE file
     if(type == 0x52494646) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (WAVE)." << endl;
        stringstream name;
        name << setfill('0') << setw(4) << wave_count << ".wav" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create wave file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to wave file.");
        wave_count++;
       }
     // ASTP file
     else if(type == 0x41545350) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (ASTP)." << endl;
        stringstream name;
        name << setfill('0') << setw(4) << astp_count << ".astp" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create astp file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to astp file.");
        astp_count++;
       }
     // ARCH file
     else if(type == 0x2266) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (ARCH)." << endl;
        stringstream name;
        name << setfill('0') << setw(4) << arch_count << ".arch" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create arch file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to arch file.");
        arch_count++;
       }
     // G1TG
     else if(type == 0x47315447) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (G1TG)." << endl;
        stringstream name;
        name << setfill('0') << setw(4) << g1tg_count << ".g1tg" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create g1tc file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to g1tg file.");
        g1tg_count++;
       }
     // G1EM
     else if(type == 0x4731454D) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (G1EM)." << endl;
        stringstream name;
        name << setfill('0') << setw(4) << g1em_count << ".g1em" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create g1em file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to g1em file.");
        g1em_count++;
       }
     // G1M_
     else if(type == 0x47314D5F) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (G1M_)." << endl;
        stringstream name;
        name << setfill('0') << setw(4) << g1m__count << ".g1m_" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create g1m file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to g1m_ file.");
        g1m__count++;
       }
     // PING
     else if(type == 0x89504E47) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (PING)." << endl;
        stringstream name;
        name << setfill('0') << setw(4) << ping_count << ".png" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create png file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to png file.");
        ping_count++;
       }

     // IGNORE BNS file
     else if(type == 0x4731415F) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (G1A_)." << endl;
     else if(type == 0x02) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (002)." << endl;
        stringstream name;
        name << setfill('0') << setw(4) << d002_count << ".002" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create 002 file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to 002 file.");
        d002_count++;
       }
     else if(type == 0x03) {
        cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (003)." << endl;
        stringstream name;
        name << setfill('0') << setw(4) << d003_count << ".003" << ends;
        ofstream ofile(name.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create 003 file.");
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to 003 file.");
        d003_count++;
       }
     // INGORE file
     else if(type == 0x0CFFFFFF) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x0CFFFFFF)." << endl;
     else if(type == 0x53FF0A00) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x53FF0A00)." << endl;
     else if(type == 0x64000000) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x64000000)." << endl;

     else if(type == 0x04) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x04)." << endl;
     else if(type == 0x06) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x06)." << endl;
     else if(type == 0x0A) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x0A)." << endl;
     else if(type == 0x0C) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x0C)." << endl;
     else if(type == 0x0D) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x0D)." << endl;
     else if(type == 0x10) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x10)." << filetable[i].first*headScale << endl;
     else if(type == 0x14) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x14)." << endl;
     else if(type == 0x16) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x16)." << endl;
     else if(type == 0x26) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x26)." << endl;
     else if(type == 0x32) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x32)." << endl;
     else if(type == 0x35) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x35)." << endl;
     else if(type == 0x46) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x46)." << endl;
     else if(type == 0x49) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x49)." << endl;
     else if(type == 0x64) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x64)." << endl;
     else if(type == 0x65) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x65)." << endl;
     else if(type == 0x67) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x67)." << endl;
     else if(type == 0x4E) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x4E)." << endl;
     else if(type == 0x72) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x72)." << endl;
     else if(type == 0x80) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x80)." << endl;
     else if(type == 0x149) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x149)." << endl;
     else if(type == 0x190) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x190)." << endl;
     else if(type == 0x1A4) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x1A4)." << endl;
     else if(type == 0x297) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x297)." << endl;
     else if(type == 0x29C) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x29C)." << endl;
     else if(type == 0x2A1) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x2A1)." << endl;
     else if(type == 0x717) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x717)." << endl;
     else if(type >= 0x001 && type <= 0x00F) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (CRAP BETWEEN 0x001 and 0x00F)." << endl;
     else if(type >= 0x010 && type <= 0x0FF) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (CRAP BETWEEN 0x010 and 0x0FF)." << endl;
     else if(type >= 0x100 && type <= 0xFFF) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (CRAP BETWEEN 0x100 and 0xFFF)." << endl;
     else if(type == 0x10063) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x10063)." << endl;
     else if(type == 0x10100) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x10100)." << endl;
     else if(type == 0x20063) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x20063)." << endl;
     else if(type == 0x200A9) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x200A9)." << endl;
     else if(type == 0x300A1) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x300A1)." << endl;
     else if(type == 0x30100) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x30100)." << endl;
     else if(type == 0x40033) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x40033)." << endl;
     else if(type == 0x40080) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x40080)." << endl;
     else if(type == 0x50032) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x50032)." << endl;
     else if(type == 0x50227) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x50227)." << endl;
     else if(type == 0x60041) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x60041)." << endl;
     else if(type == 0x601C2) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x601C2)." << endl;
     else if(type == 0x70023) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x70023)." << endl;
     else if(type == 0x70227) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x70227)." << endl;
     else if(type == 0x8002D) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x8002D)." << endl;
     else if(type == 0x80190) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x80190)." << endl;
     else if(type == 0x9002C) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x9002C)." << endl;
     else if(type == 0x9004D) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x9004D)." << endl;
     else if(type == 0xA0019) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xA0019)." << endl;
     else if(type == 0xA0050) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xA0050)." << endl;
     else if(type == 0xB0011) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xB0011)." << endl;
     else if(type == 0xB0065) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xB0065)." << endl;
     else if(type == 0xC000D) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xC000D)." << endl;
     else if(type == 0xC0032) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xC0032)." << endl; // last one checked... tired! at 0x542D000
     else if(type == 0xD000B) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xD000B)." << endl;
     else if(type == 0xE0100) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xE0100)." << endl;
     else if(type == 0xF00A1) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xF00A1)." << endl;
     else if(type >= 0x10000 && type <= 0xFFFFFF) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0xCRAP)." << endl;
     else if(type == 0x100046) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x100046)." << endl;
     else if(type == 0x110020) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x110020)." << endl;
     else if(type == 0x1202EB) cout << "Reading section " << (i + 1) << " of " << filetable.size() << " (0x1202EB)." << endl;
     else {
        cout << "curr position = " << filetable[i].first*headScale << endl;
        cout << "curr filesize = " << filetable[i].second << endl;
        cout << "next position = " << filetable[i + 1].first*headScale << endl;
        cout << "next filesize = " << filetable[i + 1].second << endl;
        return false;
       }
    }
*/
 cout << endl;
 return true;
}

bool ExtractDLC(const char* filename)
{
 return true;
}

bool Extract002(const binary_stream& bs, const char* path)
{
 /*
 ** 002 SECTION
 ** NOTE: This section appears to contain crap geometry and textures that
 ** are not important. It uses a crazy, fucked up vertex and index buffer
 ** format that I don't really want to mess with.
 */

 // skip
 return true;

 // binary stream iterator
 binary_stream ss(bs);

 // read 0002 header
 uint32 magic = ss.BE_read_uint32(); // magic
 uint32 start = ss.BE_read_uint32(); // start of data
 uint32 size  = ss.BE_read_uint32(); // size of 0002 section
 uint32 unk1  = ss.BE_read_uint32(); // 0xFFFFFFFF

 // read KTFK header
 uint32 ktfk_name = ss.BE_read_uint32(); // KTFK
 uint32 ktfk_vers = ss.BE_read_uint32(); // version
 uint32 ktfk_unk1 = ss.BE_read_uint32(); // 0x02
 uint32 ktfk_size = ss.BE_read_uint32(); // size of KTFK section

 // read four offsets
 uint32 ktfk_offset1 = start + ss.BE_read_uint32(); // offset to G1TG (ignore)
 uint32 ktfk_offset2 = start + ss.BE_read_uint32(); // offset to G1TG (ignore)
 uint32 ktfk_offset3 = start + ss.BE_read_uint32(); // offset to G1TG (ignore)
 uint32 ktfk_offset4 = start + ss.BE_read_uint32(); // offset to KHM_ (ignore)

 // read KTFK header
 struct KTFKHEADER {
  uint32 unk01;     // 0x01 or 0x02
  uint32 unk02;     // 0x04
  uint32 unk03;     // 0x0155
  uint32 unk04;     // 0x0100
  uint32 unk05;     // a section size/0x10
  uint32 unk06;     // number of entries
  uint32 vbuffsize; // v-buffer size
  uint32 ibuffsize; // i-buffer size
 };
 KTFKHEADER ktfk;
 ktfk.unk01 = ss.BE_read_uint32();
 ktfk.unk02 = ss.BE_read_uint32();
 ktfk.unk03 = ss.BE_read_uint32();
 ktfk.unk04 = ss.BE_read_uint32();
 ktfk.unk05 = ss.BE_read_uint32();
 ktfk.unk06 = ss.BE_read_uint32();
 ktfk.vbuffsize = ss.BE_read_uint32();
 ktfk.ibuffsize = ss.BE_read_uint32();

 // skip to vertex data
 binary_stream::size_type position = 0;
 position += 0x10;            // 0002 header
 position += 0x40;            // KTFK header
 position += 0x60;            // unknown data
 position += ktfk.unk01*0x20; // unknown data
 position += 0xC8;            // unknown data
 position += ktfk.unk03*0x40; // unknown data
 position += ktfk.unk05*0x10; // unknown data
 position += ktfk.unk06*0x18; // unknown data
 position += ktfk.unk05*0x08; // unknown data
 ss.seek(position);

 // geometry data
 deque<VERTEX_BUFFER> vblist;
 deque<INDEX_BUFFER> fdlist;

 // create vertex buffer
 VERTEX_BUFFER vb;
 vb.flags = VERTEX_POSITION;
 vb.elem = ktfk.vbuffsize/0x10;
 vb.data.reset(new VERTEX[vb.elem]);

 // fill vertex buffer
 for(size_t i = 0; i < vb.elem; i++)
    {
     // load vertex
     float x = (float)ss.BE_read_sint16()/32767.0f;
     (float)ss.BE_read_sint16()/32767.0f;
     float y = (float)ss.BE_read_sint16()/32767.0f;
     (float)ss.BE_read_sint16()/32767.0f;
     (float)ss.BE_read_sint16()/32767.0f;
     (float)ss.BE_read_sint16()/32767.0f;
     (float)ss.BE_read_sint16()/32767.0f;
     float z = (float)ss.BE_read_sint16()/32767.0f;

     // save vertex
     VERTEX v;
     v.vx = x;
     v.vy = y;
     v.vz = z;
     vb.data[i] = v;
    }

 // fill index buffer
 INDEX_BUFFER ib;
 ib.format = FACE_FORMAT_UINT_16;
 ib.type = FACE_TYPE_TRISTRIP;
 ib.name = "default";
 ib.reference = 0;
 ib.elem = ktfk.ibuffsize/0x02;
 ib.data.reset(new char[ktfk.ibuffsize]);
 ss.BE_read(reinterpret_cast<uint16*>(ib.data.get()), ib.elem);

 // insert buffers and convert to OBJ
 vblist.push_back(vb);
 fdlist.push_back(ib);
 //GeometryToOBJ("test", vblist, fdlist);

 return true;
}

struct GENERIC_SECTION {
 uint32 sign;
 uint32 size;
 uint32 offset1;
 uint32 offset2;
};

bool Extract003(const binary_stream& bs, const char* path)
{
 // binary stream iterator
 binary_stream ss(bs);

 deque<size_t> texture_list;
 deque<size_t> model_list;

 binary_stream::size_type index = ss.search("G1TG00", 6, 0);
 while(index != binary_stream::npos) {
       cout << "index = " << index << endl;
       texture_list.push_back(index);
       index = ss.search("G1TG00", 6, index + 1);
       if(index == binary_stream::npos) break;
      }

 index = ss.search("G1M_00", 6, 0);
 while(index != binary_stream::npos) {
       cout << "index = " << index << endl;
       model_list.push_back(index);
       index = ss.search("G1M_00", 6, index + 1);
       if(index == binary_stream::npos) break;
      }

 for(size_t i = 0; i < texture_list.size(); i++)
    {
     ss.seek(texture_list[i]);
     ExtractG1TG0060(ss, path, i);
    }

 return true;
}

bool ExtractModels(const binary_stream& bs, const char* path, size_t item)
{
 // create a folder for this file
 if(CreateDirectoryA(path, NULL) == 0)
    if(GetLastError() == ERROR_PATH_NOT_FOUND)
       return error("Could not create directory for model files.");

 // binary stream iterator and current position
 binary_stream ss(bs);
 binary_stream::size_type start = ss.tell();

 // create model filename (without extension, geometry function will take care of extension)
 stringstream filename;
 filename << path << "\\" << "G1MG_" << setfill('0') << setw(3) << item << ends;

 // read header
 G1MG0044_HEADER head;
 ss.BE_read_uint32(); // magic
 ss.BE_read_uint32(); // version
 ss.BE_read_uint32(); // size
 head.platform = ss.BE_read_uint32();
 head.unknown1 = ss.BE_read_uint32();
 ss.BE_read(&head.bbox_min[0], 3);
 ss.BE_read(&head.bbox_max[0], 3);
 head.sections = ss.BE_read_uint32();

 // read sections
 deque<G1MG0044_ITEMDATA> itemdata;
 for(size_t i = 0; i < head.sections; i++) {
     G1MG0044_ITEMDATA item;
     item.type = ss.BE_read_uint32();
     item.size = ss.BE_read_uint32();
     item.data.reset(new char[item.size - 0x8]);
     ss.read(item.data.get(), item.size - 0x8);
     itemdata.push_back(item);
    }

 // data to save
 deque<VERTEX_BUFFER> vdlist;
 deque<INDEX_BUFFER> fdlist;

 // process section data
 for(size_t i = 0; i < itemdata.size(); i++)
    {
     // vertex section
     if(itemdata[i].type == 0x00010004)
       {
        // stringstream from data
        stringstream ss(ios_base::out | ios_base::in | ios_base::binary);
        ss.write(itemdata[i].data.get(), itemdata[i].size - 0x8);
        ss.seekg(0);

        // read vertex sections
        uint32 meshes = BE_read_uint32(ss);
        for(size_t j = 0; j < meshes; j++)
           {
            // read mesh vertex info
            G1MG0044_VERTEXDATA vd;
            vd.unknown1 = BE_read_uint32(ss);
            vd.vertsize = BE_read_uint32(ss);
            vd.vertices = BE_read_uint32(ss);
            vd.unknown2 = BE_read_uint32(ss);

            // set vertex buffer properties
            VERTEX_BUFFER vb;
            vb.elem = vd.vertices;
            vb.flags = 0;
            vb.data.reset(new VERTEX[vd.vertices]);

            // set vertex buffer flags
            if(vd.vertsize == 0x10) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               //vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x14) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x18) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x1C) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x20) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else if(vd.vertsize == 0x24) {
               vb.flags |= VERTEX_POSITION;
               vb.flags |= VERTEX_NORMAL;
               vb.flags |= VERTEX_UV;
              }
            else
               return error("Unknown vertex format.");

            // read vertices
            for(size_t k = 0; k < vd.vertices; k++)
               {
                VERTEX vertex;
                if(vd.vertsize == 0x10) { // OK
                   vertex.vx = BE_read_float16(ss);
                   vertex.vy = BE_read_float16(ss);
                   vertex.vz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x14) { // OK
                   vertex.vx = BE_read_float16(ss);
                   vertex.vy = BE_read_float16(ss);
                   vertex.vz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = 1.0f - BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x18) { // OK
                   vertex.vx = BE_read_float16(ss);
                   vertex.vy = BE_read_float16(ss);
                   vertex.vz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);            
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = 1.0f - BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x1C) { // OK
                   vertex.vx = BE_read_float16(ss);
                   vertex.vy = BE_read_float16(ss);
                   vertex.vz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = 1.0f - BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x20) { // OK
                   vertex.vx = BE_read_float16(ss);
                   vertex.vy = BE_read_float16(ss);
                   vertex.vz = BE_read_float16(ss);
                   BE_read_float16(ss);
                   BE_read_float32(ss); // ok (sum is 1) possibly weight?
                   BE_read_float32(ss); // ok (sum is 1) possibly weight?
                   BE_read_float16(ss);
                   vertex.nx = BE_read_float16(ss);
                   vertex.ny = BE_read_float16(ss);
                   vertex.nz = BE_read_float16(ss);
                   BE_read_float32(ss);
                   vertex.tu = BE_read_float16(ss);
                   vertex.tv = 1.0f - BE_read_float16(ss);
                  }
                else if(vd.vertsize == 0x24) {
                   vertex.vx = BE_read_float32(ss);
                   vertex.vy = BE_read_float32(ss);
                   vertex.vz = BE_read_float32(ss);
                   vertex.nx = BE_read_float32(ss);
                   vertex.ny = BE_read_float32(ss);
                   vertex.nz = BE_read_float32(ss);
                   BE_read_float32(ss);
                   vertex.tu = BE_read_float32(ss);
                   vertex.tv = BE_read_float32(ss);
                   return error("0x24 VERTEX TYPE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                  }
                else return error("Unknown vertex format.");

                // save vertex
                vb.data[k] = vertex;
               }

            // save vertex data
            vdlist.push_back(vb);
           }
       }
     // index buffer section
     else if(itemdata[i].type == 0x00010007)
       {
        // stringstream from data
        stringstream ss(ios_base::out | ios_base::in | ios_base::binary);
        ss.write(itemdata[i].data.get(), itemdata[i].size - 0x8);
        ss.seekg(0);

        // read face sections
        uint32 n_meshes = BE_read_uint32(ss);
        uint32 vb_index = 0;
        for(size_t j = 0; j < n_meshes; j++)
           {
            // read face data
            G1MG0044_FACEDATA fd;
            fd.numindex = BE_read_uint32(ss);
            fd.datatype = BE_read_uint32(ss);
            fd.unknown1 = BE_read_uint32(ss);

            // set index buffer properties
            INDEX_BUFFER ib;
            ib.type = FACE_TYPE_TRISTRIP;
            ib.elem = fd.numindex;
            if(fd.datatype == 0x10) ib.format = FACE_FORMAT_UINT_16;
            else if(fd.datatype == 0x20) ib.format = FACE_FORMAT_UINT_32;
            else return error("Unknown index buffer data format.");

            // set index buffer name
            stringstream surface;
            surface << setfill('0') << setw(3) << item << "_" << setw(3) << j << ends;
            ib.name = surface.str();

            // determine index buffer data type size
            unsigned int typesize = 0;
            if(fd.datatype == 0x10) typesize = sizeof(uint16);
            else if(fd.datatype == 0x20) typesize = sizeof(uint32);
            else return error("Unknown index buffer data type.");

            // read face data
            unsigned int total_bytes = ib.elem * typesize;
            ib.data.reset(new char[total_bytes]);
            if(ib.format == FACE_FORMAT_UINT_16) BE_read_array(ss, reinterpret_cast<uint16*>(ib.data.get()), ib.elem);
            else if(ib.format == FACE_FORMAT_UINT_32) BE_read_array(ss, reinterpret_cast<uint32*>(ib.data.get()), ib.elem);

            // test face data
            uint32 min_index = 0;
            uint32 max_index = 0;
            if(ib.format == FACE_FORMAT_UINT_16) {
               uint16 a; minimum(reinterpret_cast<uint16*>(ib.data.get()), ib.elem, a);
               uint16 b; maximum(reinterpret_cast<uint16*>(ib.data.get()), ib.elem, b);
               min_index = a;
               max_index = b;
              }
            else if(ib.format == FACE_FORMAT_UINT_32) {
               uint32 a; minimum(reinterpret_cast<uint32*>(ib.data.get()), ib.elem, a);
               uint32 b; maximum(reinterpret_cast<uint32*>(ib.data.get()), ib.elem, b);
               min_index = a;
               max_index = b;
              }

            // set vertex buffer reference
            if(min_index == 0) vb_index++;
            if(vb_index > 0) ib.reference = vb_index - 1;
            else return error("Unexpected vertex buffer reference.");

            // save face data
            fdlist.push_back(ib);
           }
       }
    }

 // save geometry
 stringstream objname;
 objname << "G1MG_" << setfill('0') << setw(3) << item << ends;
 if(!GeometryToOBJ(path, objname.str().c_str(), vdlist, fdlist)) return false;
 return true;
}

bool ExtractG1TG0060(const binary_stream& bs, const char* path, size_t item)
{
 // create a folder for this file
 if(CreateDirectoryA(path, NULL) == 0)
    if(GetLastError() == ERROR_PATH_NOT_FOUND)
       return error("Could not create directory for texture files.");

 // binary stream iterator and current position
 binary_stream ss(bs);
 binary_stream::size_type start = ss.tell();

 // read header
 uint32 magic = ss.BE_read_uint32();
 uint32 version = ss.BE_read_uint32();
 uint32 section_size = ss.BE_read_uint32();
 uint32 table_offset = ss.BE_read_uint32();
 uint32 n_textures = ss.BE_read_uint32();

 // validate header
 if(magic != 0x47315447) return error("Expecting G1TG section.");
 if(!(version == 0x30303530 || version == 0x30303630)) return error("Invalid G1TG version.");
 if(n_textures == 0) return error("Invalid number of textures.");
 if(n_textures >= 1000) return error("Excessive number of texture. We really don't expect to have this many in a single file.");

 // move to table
 ss.seek(start + table_offset);
 if(ss.fail()) return error("Stream seek failure.");

 // read offset table
 deque<size_t> offset_list;
 for(size_t i = 0; i < n_textures; i++) {
     offset_list.push_back(ss.BE_read_uint32());
     if(ss.fail()) return error("Stream read failure.");
    }

 // process textures
 for(size_t i = 0; i < offset_list.size(); i++)
    {
     ss.seek(start + table_offset + offset_list[i]);
     if(ss.fail()) return error("Stream seek failure.");

     // read texture information
     uint08 b1 = ss.BE_read_uint08(); // unknown
     uint08 b2 = ss.BE_read_uint08(); // texture type
     uint08 b3 = ss.BE_read_uint08(); // dx/dy
     uint08 b4 = ss.BE_read_uint08(); // unknown
     uint16 s1 = ss.BE_read_uint16(); // unknown
     uint16 s2 = ss.BE_read_uint16(); // unknown

     // header is extended depending on version
     size_t header_size = 8;
     if(version == 0x30303630) {
        uint32 v1 = ss.BE_read_uint32(); // 12
        uint32 v2 = ss.BE_read_uint32(); // 0
        uint32 v3 = ss.BE_read_uint32(); // 0 or 1
        header_size += 12;
       }

     // compute dimensions
     uint32 temp1 = ((b3 & 0xF0) >> 4);
     uint32 temp2 = ((b3 & 0x0F));
     uint32 dx = 1 << temp1;
     uint32 dy = 1 << temp2;

     // create texture file
     stringstream filename;
     if(item) filename << path << "/" << "G1TG_" << setfill('0') << setw(3) << item << "_" << setfill('0') << setw(3) << i << ".dds" << ends;
     else filename << path << "/" << "G1TG_" << setfill('0') << setw(3) << i << ".dds" << ends;
     ofstream ofile(filename.str().c_str(), ios::binary);

     // save texture
     if(b2 == 0)
       {
        // read texture
        DWORD filesize = UncompressedDDSFileSize(dx, dy, 0, 0xFF000000, 0xFF0000, 0xFF00, 0xFF);
        boost::shared_array<char> buffer(new char[filesize]);
        ss.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, 0, 0xFF0000, 0xFF00, 0xFF, 0xFF000000, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);

        // LINKDATA_BNS/013/G1TG_000.dds
        // LINKDATA_BNS/018/G1TG_000.dds
        // LINKDATA_BNS/027/G1TG_006.dds
        // LINKDATA_BNS/028/G1TG_000.dds
        // LINKDATA_BNS/028/G1TG_001.dds
        // LINKDATA_BNS/028/G1TG_003.dds
        // LINKDATA_BNS/909/G1TG_000.dds

       }
     // UNCOMPRESSED DDS (PS3 works good)
     else if(b2 == 1)
       {
        // read texture
        DWORD filesize = UncompressedDDSFileSize(dx, dy, 0, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        boost::shared_array<char> buffer(new char[filesize]);
        ss.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, 0, 0xFF000000, 0xFF0000, 0xFF00, 0xFF, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // DXT1 (PS3 works good)
     else if(b2 == 6)
       {
        // read texture
        DWORD filesize = DXT1Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ss.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, 0, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // DXT5 (PS3 works good)
     else if(b2 == 8)
       {
        // read texture
        DWORD filesize = DXT5Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ss.read((char*)buffer.get(), filesize);

        // save texture
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, 0, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // ???
     else if(b2 == 16)
       {
        cout << "FAILED" << endl;
        cout <<  filename.str() << endl;

        cout << "Unknown texture type = " << (int)b2 << endl;
        cout << " dx = " << dx << endl;
        cout << " dy = " << dy << endl;
       }
     // ???
     else if(b2 == 18)
       {
        cout << "FAILED" << endl;
        cout <<  filename.str() << endl;

        cout << "Unknown texture type = " << (int)b2 << endl;
        cout << " dx = " << dx << endl;
        cout << " dy = " << dy << endl;
       }
     else {
        return error("Unsupported texture type.");
       }
    }

 return true;
}

};};