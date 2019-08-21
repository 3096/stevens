#include "xentax.h"
#include "x_file.h"
#include "x_stream.h"
#include "x_alg.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_dsp.h"
#include "xbox360/xgraphics.h"
using namespace std;

bool MortonTest(boost::shared_array<char> data, uint32 dx, uint32 dy, boost::shared_array<char> copy);

bool processDSPTextures(const DESPERADOINFO& dspi);
bool processDSPModels(const DESPERADOINFO& dspi);

void clear(DSPGENERAL& info)
{
 info.filename.clear();
 info.savepath.clear();
 info.endian = DSP_LITTLE_ENDIAN;
}

void clear(DSPVTXBUFFER& info)
{
 info.filename.clear();
 info.identifier = DSP_INVALID_IDENTIFIER;
 info.name = TEXT("");
 info.offset = 0;
 info.size = 0;
}

void clear(DSPIDXBUFFER& info)
{
 info.filename.clear();
 info.identifier = DSP_INVALID_IDENTIFIER;
 info.name = TEXT("");
 info.offset = 0;
 info.size = 0;
}

void clear(DSPINPUTLAYOUT& info)
{
 // general
 info.identifier = DSP_INVALID_IDENTIFIER;
 info.name = TEXT("");
 // positions
 info.positions = 0;
 for(uint32 i = 0; i < 16; i++) {
     info.position_offset[i] = 0;
     info.position_format[i] = DSP_FORMAT_UNKNOWN;
     info.position_endian[i] = DSP_UNSPECIFIED_ENDIAN;
     info.position_slot[i] = 0;
     info.position_bias[i] = 0.0f;
     info.position_scale[i] = 0.0f;
    }
 // normals
 info.normals = 0;
 for(uint32 i = 0; i < 16; i++) {
     info.normal_offset[i] = 0;
     info.normal_format[i] = DSP_FORMAT_UNKNOWN;
     info.normal_endian[i] = DSP_UNSPECIFIED_ENDIAN;
     info.normal_slot[i] = 0;
     info.normal_bias[i] = 0.0f;
     info.normal_scale[i] = 0.0f;
    }
 // binormals
 info.binormals = 0;
 for(uint32 i = 0; i < 16; i++) {
     info.binormal_offset[i] = 0;
     info.binormal_format[i] = DSP_FORMAT_UNKNOWN;
     info.binormal_endian[i] = DSP_UNSPECIFIED_ENDIAN;
     info.binormal_slot[i] = 0;
     info.binormal_bias[i] = 0.0f;
     info.binormal_scale[i] = 0.0f;
    }
 // tangents
 info.tangents = 0;
 for(uint32 i = 0; i < 16; i++) {
     info.tangent_offset[i] = 0;
     info.tangent_format[i] = DSP_FORMAT_UNKNOWN;
     info.tangent_endian[i] = DSP_UNSPECIFIED_ENDIAN;
     info.tangent_slot[i] = 0;
     info.tangent_bias[i] = 0.0f;
     info.tangent_scale[i] = 0.0f;
    }
 // texture coordinates
 info.uvcoords = 0;
 for(uint32 i = 0; i < 16; i++) {
     info.uv_offset[i] = 0;
     info.uv_format[i] = DSP_FORMAT_UNKNOWN;
     info.uv_endian[i] = DSP_UNSPECIFIED_ENDIAN;
     info.uv_slot[i] = 0;
     info.uv_bias[i] = 0.0f;
     info.uv_scale[i] = 0.0f;
    }
 // colors
 info.colors = 0;
 for(uint32 i = 0; i < 16; i++) {
     info.color_offset[i] = 0;
     info.color_format[i] = DSP_FORMAT_UNKNOWN;
     info.color_endian[i] = DSP_UNSPECIFIED_ENDIAN;
     info.color_slot[i] = 0;
     info.color_bias[i] = 0.0f;
     info.color_scale[i] = 0.0f;
    }
 // blend weights
 info.weights = 0;
 info.blendweight_type = DSP_BLENDWEIGHT_UNKNOWN;
 for(uint32 i = 0; i < 8; i++) {
     info.blendweight_offset[i] = 0;
     info.blendweight_format[i] = DSP_FORMAT_UNKNOWN;
     info.blendweight_endian[i] = DSP_UNSPECIFIED_ENDIAN;
     info.blendweight_slot[i] = 0;
     info.blendweight_bias[i] = 0.0f;
     info.blendweight_scale[i] = 0.0f;
    }
 // blend indices
 info.indices = 0;
 info.blendindex_type = DSP_BLENDINDEX_UNKNOWN;
 for(uint32 i = 0; i < 8; i++) {
     info.blendindex_offset[i] = 0;
     info.blendindex_format[i] = DSP_FORMAT_UNKNOWN;
     info.blendindex_endian[i] = DSP_UNSPECIFIED_ENDIAN;
     info.blendindex_slot[i] = 0;
    }
}

void clear(DSPINPUTASSEMBLY& info)
{
 info.input_layout_reference = DSP_INVALID_IDENTIFIER;
 info.vertex_buffers = 0;
 info.vertex_buffer_start_slot = 0;
 for(uint32 i = 0; i < 16; i++) {
     info.vertex_buffer_references[i] = DSP_INVALID_IDENTIFIER;
     info.vertex_buffer_strides[i] = 0;
     info.vertex_buffer_offsets[i] = 0;
    }
 info.index_buffer_reference = DSP_INVALID_IDENTIFIER;
 info.index_buffer_format = DSP_FORMAT_UNKNOWN;
 info.index_buffer_offset = 0;
 info.topology = DSP_UNKNOWN_TOPOLOGY;
 info.draw_indexed_count = 0;
 info.draw_indexed_start = 0;
 info.draw_indexed_base_vertex = 0;
 info.jointmap_reference = DSP_INVALID_IDENTIFIER;
}

void clear(DSPTEXTUREINFO& info)
{
 info.filename.clear();
 info.savepath.clear();
 info.reversal = DSP_REVERSE_NONE;
 info.identifier = DSP_INVALID_IDENTIFIER;
 info.name.clear();
 info.offset = DSP_INVALID_OFFSET;
 info.dx = 0;
 info.dy = 0;
 info.size = 0;
 info.type = DSP_TEXTURE_UNKNOWN;
 info.mipmaps = 0;
 info.cubemap = DSP_CUBEMAP_NONE;
 info.swizzle = DSP_SWIZZLE_NONE;
}

bool desperado(const DESPERADOINFO& dspi)
{
 // process textures
 for(uint32 i = 0; i < dspi.textures.size(); i++)
    {
     const DSPTEXTUREINFO& info = dspi.textures[i];

     // obtain identifier
     uint32 identifier = info.identifier; // specified
     if(identifier == DSP_INVALID_IDENTIFIER) identifier = i; // unspecified
     
     // obtain name
     STDSTRING name = info.name; // specified
     if(!name.length()) { // unspecified, use identifier
        STDSTRINGSTREAM ss;
        ss << identifier;
        name = ss.str();
       }

     // obtain filename
     STDSTRING filename;
     if(info.filename.length()) filename = info.filename;
     else if(dspi.general.filename.length()) filename = dspi.general.filename;
     else return error("Filename not specified.");

     // filename properties
     STDSTRING pathname = GetPathnameFromFilename(filename.c_str()).get();
     STDSTRING shrtname = GetShortFilenameWithoutExtension(filename.c_str()).get();

     // obtain savepath
     STDSTRING savepath;
     if(info.savepath.length()) savepath = info.savepath; // use specified
     else if(dspi.general.savepath.length()) savepath = dspi.general.savepath; // use general
     else savepath = pathname; // use filename path

     // append "\\" to savepath if necessary
     if(savepath.back() != TEXT('\\')) savepath += TEXT('\\');

     // open file
     ifstream ifile(filename, ios::binary);
     if(!ifile) return error("Failed to open file.");

     // determine filesize
     uint32 filesize = 0x00;
     ifile.seekg(0, ios::end);
     filesize = (uint32)ifile.tellg();
     ifile.seekg(0, ios::beg);

     // obtain offset
     uint64 offset = info.offset;
     if(info.offset == DSP_INVALID_OFFSET) return error("Invalid offset.");
     if(!(offset < filesize)) return error("Invalid offset.");

     // move to offset
     ifile.seekg(offset);
     if(ifile.fail()) return error("Seek failure.");

     // obtain dimensions
     uint32 dx = info.dx;
     uint32 dy = info.dy;
     if(!dx || dx > 8192) return error("Invalid texture dimensions.");
     if(!dy || dy > 8192) return error("Invalid texture dimensions.");

     // obtain mipmaps, type, and size
     uint32 size = info.size;
     uint32 type = info.type;
     uint32 mipmaps = info.mipmaps;
     switch(type) {
       case(DSP_TEXTURE_RGBA) :
       case(DSP_TEXTURE_BGRA) :
       case(DSP_TEXTURE_ARGB) :
       case(DSP_TEXTURE_ABGR) :
       case(DSP_TEXTURE_DXT1) :
       case(DSP_TEXTURE_DXT3) :
       case(DSP_TEXTURE_DXT5) : break;
       default : return error("Unsupported texture type.");
      }

     // obtain cubemap and compute number of stored cubes
     uint32 cubemap = info.cubemap;
     uint32 cubedim = 0;
     if(cubemap & DSP_CUBEMAP_POS_X) cubedim++;
     if(cubemap & DSP_CUBEMAP_POS_Y) cubedim++;
     if(cubemap & DSP_CUBEMAP_POS_Z) cubedim++;
     if(cubemap & DSP_CUBEMAP_NEG_X) cubedim++;
     if(cubemap & DSP_CUBEMAP_NEG_Y) cubedim++;
     if(cubemap & DSP_CUBEMAP_NEG_Z) cubedim++;

     // compute expected datasize
     uint32 datasize_computed = 0;
     switch(type) {
       case(DSP_TEXTURE_RGBA) : datasize_computed = UncompressedDDSFileSize(dx, dy, mipmaps, 32); break;
       case(DSP_TEXTURE_BGRA) : datasize_computed = UncompressedDDSFileSize(dx, dy, mipmaps, 32); break;
       case(DSP_TEXTURE_ARGB) : datasize_computed = UncompressedDDSFileSize(dx, dy, mipmaps, 32); break;
       case(DSP_TEXTURE_ABGR) : datasize_computed = UncompressedDDSFileSize(dx, dy, mipmaps, 32); break;
       case(DSP_TEXTURE_DXT1) : datasize_computed = DXT1Filesize(dx, dy, mipmaps); break;
       case(DSP_TEXTURE_DXT3) : datasize_computed = DXT3Filesize(dx, dy, mipmaps); break;
       case(DSP_TEXTURE_DXT5) : datasize_computed = DXT5Filesize(dx, dy, mipmaps); break;
       default : return error("Unsupported texture type.");
      }

     // RGBA positions
     uint32 M1 = 0xFF000000ul; // A
     uint32 M2 = 0x00FF0000ul; // R 
     uint32 M3 = 0x0000FF00ul; // G
     uint32 M4 = 0x000000FFul; // B

     // create DDS_HEADER
     DDS_HEADER ddsh;
     switch(type) {
       case(DSP_TEXTURE_RGBA) : CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, M2, M3, M4, M1, FALSE, &ddsh); break;
       case(DSP_TEXTURE_BGRA) : CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, M2, M3, M4, M1, FALSE, &ddsh); break;
       case(DSP_TEXTURE_ARGB) : CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, M2, M3, M4, M1, FALSE, &ddsh); break;
       case(DSP_TEXTURE_ABGR) : CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, M2, M3, M4, M1, FALSE, &ddsh); break;
       case(DSP_TEXTURE_DXT1) : CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh); break;
       case(DSP_TEXTURE_DXT3) : CreateDXT3Header(dx, dy, mipmaps, FALSE, &ddsh); break;
       case(DSP_TEXTURE_DXT5) : CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh); break;
       default : return error("Unsupported texture type.");
      }

     // if size is less than expected datasize
     uint32 datasize = datasize_computed;
     if(!datasize) return error("Invalid texture size.");
     if(filesize < (offset + datasize)) return error("Invalid texture size. Required texture data overruns file.");

     // read texture data into memory
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);
     if(ifile.fail()) return error("Read failure.");

     //
     // BYTE-ORDER REVERSAL
     //

     // determine if reversal is possible
     bool do_reverse = false;
     if(info.reversal != DSP_REVERSE_NONE) {
        if(type == DSP_TEXTURE_DXT1) do_reverse = true;
        else if(type == DSP_TEXTURE_DXT3) do_reverse = true;
        else if(type == DSP_TEXTURE_DXT5) do_reverse = true;
        else do_reverse = false;
       }

     // do the actual reversal
     if(do_reverse)
       {
        switch(info.reversal) {
          case(DSP_REVERSE_NONE) : {
               break;
              }
          case(DSP_REVERSE_2) : {
               if(datasize % 2) return error("Cannot save texture. DSP_REVERSE_2 requires a computed texture datasize that is divisble by 2.");
               uint16* temp = reinterpret_cast<uint16*>(data.get());
               reverse_byte_order(temp, datasize/2);
               break;
              }
          case(DSP_REVERSE_4) : {
               if(datasize % 4) return error("Cannot save texture. DSP_REVERSE_4 requires a computed texture datasize that is divisble by 4.");
               uint32* temp = reinterpret_cast<uint32*>(data.get());
               reverse_byte_order(temp, datasize/4);
               break;
              }
          case(DSP_REVERSE_8) : {
               if(datasize % 8) return error("Cannot save texture. DSP_REVERSE_8 requires a computed texture datasize that is divisble by 8.");
               uint64* temp = reinterpret_cast<uint64*>(data.get());
               reverse_byte_order(temp, datasize/8);
               break;
              }
          }
       }

     //
     // MORTON Z-ORDER CONVERSION
     //

     // determine if conversion is possible
     bool do_morton = false;
     if(info.swizzle == DSP_SWIZZLE_MORTON) {
        if(type == DSP_TEXTURE_RGBA) do_morton = true;
        else if(type == DSP_TEXTURE_BGRA) do_morton = true;
        else if(type == DSP_TEXTURE_ARGB) do_morton = true;
        else if(type == DSP_TEXTURE_ABGR) do_morton = true;
       }

     // determine if dx and dy are powers of two
     bool dx_po2 = (info.dx > 0 && !(info.dx & (info.dx - 1)));
     bool dy_po2 = (info.dy > 0 && !(info.dy & (info.dy - 1)));
     bool dx_equals_dy = (info.dx == info.dy);

     // perform conversion
     if(do_morton)
       {
        // do fast conversion
        if(dx_equals_dy && dx_po2 && dy_po2)
          {
           boost::shared_array<char> copy(new char[datasize]);
           for(uint32 r = 0; r < info.dy; r++) {
               for(uint32 c = 0; c < info.dx; c++) {
                   uint32 morton = array_to_morton(r, c);
                   uint32 copy_position = 4*r*info.dx + 4*c;
                   uint32 data_position = 4*morton;
                   copy[copy_position + 0] = data[data_position + 0];
                   copy[copy_position + 1] = data[data_position + 1];
                   copy[copy_position + 2] = data[data_position + 2];
                   copy[copy_position + 3] = data[data_position + 3];
                  }
              }
           data = copy;
          }
        // do fast conversion
        else if((info.dx == 2*info.dy) && (dx_po2 && dy_po2))
          {
           boost::shared_array<char> copy(new char[datasize]);
           uint32 half_dx = info.dx/2;
           for(uint32 r = 0; r < info.dy; r++) {
               for(uint32 c = 0; c < half_dx; c++) {
                   uint32 morton = array_to_morton(r, c);
                   uint32 copy_position = 8*r*half_dx + 8*c;
                   uint32 data_position = 8*morton;
                   copy[copy_position + 0] = data[data_position + 0];
                   copy[copy_position + 1] = data[data_position + 1];
                   copy[copy_position + 2] = data[data_position + 2];
                   copy[copy_position + 3] = data[data_position + 3];
                   copy[copy_position + 4] = data[data_position + 4];
                   copy[copy_position + 5] = data[data_position + 5];
                   copy[copy_position + 6] = data[data_position + 6];
                   copy[copy_position + 7] = data[data_position + 7];
                  }
              }
           data = copy;
          }
        // do slow conversion
        else {
           boost::shared_array<char> copy(new char[datasize]);
           MortonTest(data, info.dx, info.dy, copy);
           data = copy;
          }
       }

     //
     // XBOX UNTILE CONVERSION
     //

     // determine if conversion is possible
     bool do_xbtile = false;
     if(info.swizzle == DSP_SWIZZLE_XBTILE) {
        if(type == DSP_TEXTURE_RGBA) do_xbtile = true;
        else if(type == DSP_TEXTURE_BGRA) do_xbtile = true;
        else if(type == DSP_TEXTURE_ARGB) do_xbtile = true;
        else if(type == DSP_TEXTURE_ABGR) do_xbtile = true;
       }

     // untile data
     if(do_xbtile) {
        DWORD pitch_bytes = 4*info.dx;
        DWORD format = XGGetGpuFormat(D3DFMT_A8R8G8B8);
        boost::shared_array<char> copy(new char[datasize]);
        XGUntileTextureLevel(dx, dy, 0, format, XGTILE_NONPACKED, copy.get(), pitch_bytes, NULL, data.get(), NULL);
        data = copy;
       }

     //
     // ARGB CONVERSION 
     //

     // Photoshop only support ARGB
     if(type == DSP_TEXTURE_RGBA) {
        uint32 n_pixels = dx * dy;
        for(uint32 j = 0; j < n_pixels; j++) {
            uint32 index1 = 4*j;
            uint32 index2 = index1 + 1;
            uint32 index3 = index2 + 1;
            uint32 index4 = index3 + 1;
            auto temp = data[index1];
            data[index1] = data[index4]; // AGBA
            data[index4] = data[index3]; // AGBB
            data[index3] = data[index2]; // AGGB
            data[index2] = temp;         // ARGB
           }
       }
     else if(type == DSP_TEXTURE_BGRA) {
        uint32 n_pixels = dx * dy;
        for(uint32 j = 0; j < n_pixels; j++) {
            uint32 index1 = 4*j;
            uint32 index2 = index1 + 1;
            uint32 index3 = index2 + 1;
            uint32 index4 = index3 + 1;
            swap(data[index1], data[index4]); // swap A and B
            swap(data[index2], data[index3]); // swap R and G
           }

       }
     else if(type == DSP_TEXTURE_ARGB) {
       }
     else if(type == DSP_TEXTURE_ABGR) {
        uint32 n_pixels = dx * dy;
        for(uint32 j = 0; j < n_pixels; j++) {
            uint32 index1 = 4*j;
            uint32 index2 = index1 + 1;
            uint32 index3 = index2 + 1;
            uint32 index4 = index3 + 1;
            swap(data[index2], data[index3]); // swap R and G
           }
       }

     // create filename
     STDSTRING ofname = savepath;
     ofname += name;
     ofname += TEXT(".dds");

     // create output file
     ofstream ofile(ofname.c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // save DDS file
     if(!SaveDDSFile(ofname.c_str(), ddsh, data, datasize))
        return false;
    }

 if(!processDSPTextures(dspi)) return false;
 if(!processDSPModels(dspi)) return false;
 return true;
}

bool processDSPTextures(const DESPERADOINFO& dspi)
{
 return true;
}

bool processDSPModels(const DESPERADOINFO& dspi)
{
 // DSPVTXBUFFER map identifer to index
 typedef map<uint32,uint32>::value_type pair_t;
 map<uint32,uint32> vbidmap;
 for(uint32 i = 0; i < dspi.vblist.size(); i++) {
     if(dspi.vblist[i].identifier == DSP_INVALID_IDENTIFIER) continue;
     if(vbidmap.find(dspi.vblist[i].identifier) != vbidmap.end()) return error("Duplicate DSPVTXBUFFER identifier.");
     vbidmap.insert(pair_t(dspi.vblist[i].identifier, i));
    }

 // DSPIDXBUFFER map identifer to index
 map<uint32,uint32> ibidmap;
 for(uint32 i = 0; i < dspi.iblist.size(); i++) {
     if(dspi.iblist[i].identifier == DSP_INVALID_IDENTIFIER) continue;
     if(ibidmap.find(dspi.iblist[i].identifier) != ibidmap.end()) return error("Duplicate DSPIDXBUFFER identifier.");
     ibidmap.insert(pair_t(dspi.iblist[i].identifier, i));
    }

 // DSPINPUTLAYOUT map identifer to index
 map<uint32,uint32> ilidmap;
 for(uint32 i = 0; i < dspi.illist.size(); i++) {
     if(dspi.illist[i].identifier == DSP_INVALID_IDENTIFIER) continue;
     if(ilidmap.find(dspi.illist[i].identifier) != ilidmap.end()) return error("Duplicate DSPINPUTLAYOUT identifier.");
     ilidmap.insert(pair_t(dspi.illist[i].identifier, i));
    }

 // load vertex buffer data
 deque<boost::shared_array<char>> vbdatalist;
 for(uint32 i = 0; i < dspi.vblist.size(); i++)
    {
     // obtain filename
     STDSTRING filename;
     if(dspi.vblist[i].filename.length()) filename = dspi.vblist[i].filename;
     else if(dspi.general.filename.length()) filename = dspi.general.filename;
     else return error("Filename not specified.");

     // open file
     ifstream ifile(filename.c_str(), ios::binary);
     if(!ifile) return error("Failed to open file.");

     // get filesize
     auto filesize = GetFileSize(ifile);
     if(!filesize) return error("File is empty.");

     // validate offset and size
     if(!dspi.vblist[i].size) return error("Invalid vertex buffer size.");
     if(filesize < dspi.vblist[i].size) return error("Invalid vertex buffer size.");

     // determine where data finishes
     uint32 finish = static_cast<uint32>(dspi.vblist[i].offset + dspi.vblist[i].size);
     if(filesize < finish) return error("Invalid vertex buffer size.");

     // seek data
     ifile.seekg(dspi.vblist[i].offset);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[dspi.vblist[i].size]);
     ifile.read(data.get(), dspi.vblist[i].size);
     if(ifile.fail()) return error("Read failure.");

     // append to vertex buffer data list
     vbdatalist.push_back(data);
    }

 // load index buffer data
 deque<boost::shared_array<char>> ibdatalist;
 for(uint32 i = 0; i < dspi.iblist.size(); i++)
    {
     // obtain filename
     STDSTRING filename;
     if(dspi.iblist[i].filename.length()) filename = dspi.iblist[i].filename;
     else if(dspi.general.filename.length()) filename = dspi.general.filename;
     else return error("Filename not specified.");

     // open file
     ifstream ifile(filename.c_str(), ios::binary);
     if(!ifile) return error("Failed to open file.");

     // get filesize
     auto filesize = GetFileSize(ifile);
     if(!filesize) return error("File is empty.");

     // validate offset and size
     if(!dspi.iblist[i].size) return error("Invalid index buffer size.");
     if(filesize < dspi.iblist[i].size) return error("Invalid index buffer size.");

     // determine where data finishes
     uint32 finish = static_cast<uint32>(dspi.iblist[i].offset + dspi.iblist[i].size);
     if(filesize < finish) return error("Invalid index buffer size.");

     // seek data
     ifile.seekg(dspi.iblist[i].offset);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[dspi.iblist[i].size]);
     ifile.read(data.get(), dspi.iblist[i].size);
     if(ifile.fail()) return error("Read failure.");

     // append to vertex buffer data list
     ibdatalist.push_back(data);
    }

 // model container
 ADVANCEDMODELCONTAINER amc;

 // process input assemblies
 for(uint32 i = 0; i < dspi.ialist.size(); i++)
    {
     // skip if input layout reference is invalid or if there are no vertex buffers
     const DSPINPUTASSEMBLY& assembly = dspi.ialist[i];
     if(assembly.input_layout_reference == DSP_INVALID_IDENTIFIER) continue;
     if(assembly.vertex_buffers == 0) continue;
     if(assembly.vertex_buffers > 16) return error("Invalid number of vertex buffers.");

     // find input layout reference
     auto il_reference = assembly.input_layout_reference;
     auto iter = ilidmap.find(il_reference);
     if(iter == ilidmap.end()) return error("Input layout reference not found.");
     if(!(iter->second < dspi.illist.size())) return error("Input layout index out of range.");
     const DSPINPUTLAYOUT& layout = dspi.illist[iter->second];

     // prepare vertex buffer
     AMC_VTXBUFFER curr_vb;
     curr_vb.name = "default";
     curr_vb.flags = 0;
     if(layout.positions > 0) curr_vb.flags |= AMC_VERTEX_POSITION;
     if(layout.normals > 0) curr_vb.flags |= AMC_VERTEX_NORMAL;
     if(layout.tangents > 0) curr_vb.flags |= AMC_VERTEX_TANGENT;
     if(layout.binormals > 0) curr_vb.flags |= AMC_VERTEX_BINORMAL;
     if(layout.weights > 0 || layout.indices > 0) curr_vb.flags |= AMC_VERTEX_WEIGHTS;
     if(layout.uvcoords > 0) curr_vb.flags |= AMC_VERTEX_UV;
     if(layout.colors > 0) curr_vb.flags |= AMC_VERTEX_COLORS;

     // set number of UV channels
     if(layout.uvcoords > 8) return error("Only up to eight UV channels are supported.");
     else if(layout.uvcoords > 0) curr_vb.uvchan = layout.uvcoords;

     // set number of color channels
     if(layout.colors > 8) return error("Only up to eight color channels are supported.");
     else if(layout.colors > 0) curr_vb.colors = layout.colors;

     // merge vertex buffers
     // for each vertex buffer reference in the assembly, we need to look up the
     // input layout and determine what semantics are enabled
     for(uint32 j = 0; j < assembly.vertex_buffers; j++)
        {
         // assembly vertex buffer index
         uint32 index = assembly.vertex_buffer_start_slot + j;
         if(!(index < assembly.vertex_buffers)) return error("Invalid vertex buffer index.");

         // find vertex buffer data
         uint32 vb_reference = assembly.vertex_buffer_references[index];
         auto iter = vbidmap.find(vb_reference);
         if(iter == vbidmap.end()) return error("Vertex buffer index not found.");
         if(!(iter->second < dspi.vblist.size())) return error("Vertex buffer index out of range.");
         const DSPVTXBUFFER& vb = dspi.vblist[iter->second];
         boost::shared_array<char> vbdata = vbdatalist[iter->second];

         // 
         //layout.
         
        }
    }

 // TODO: DELETE
 // validate references
 for(uint32 i = 0; i < dspi.ialist.size(); i++)
    {
     // index buffer reference
     uint32 reference = dspi.ialist[i].index_buffer_reference;
     if(ilidmap.find(reference) == ilidmap.end()) return error("Input layout reference not found.");

     // TODO: joint map reference
     if(dspi.ialist[i].jointmap_reference != DSP_INVALID_IDENTIFIER) {
       }
    }

 return true;
}

//
// INEFFICIENT MORTON Z-ORDER CONVERSION
// FOR RGBA IMAGE DATA
//
void MortonTestTransfer1(boost::shared_array<char> data, uint32 dx, uint32 dy, boost::shared_array<char> copy, uint32 x0, uint32 y0, uint32 src_index)
{
 // data is swizzled data, and src_index maps to data
 uint32 dst_index = y0*dx + x0;
 uint32 i = 4*src_index;
 uint32 j = 4*dst_index;
 copy[j++] = data[i++];
 copy[j++] = data[i++];
 copy[j++] = data[i++];
 copy[j] = data[i];
}

void MortonTestTransfer2(boost::shared_array<char> data, uint32 dx, uint32 dy, boost::shared_array<char> copy, uint32 x0, uint32 y0, uint32 src_index)
{
 uint32 dst_index = y0*dx + x0;
 uint32 i = 4*src_index;
 uint32 j = 4*dst_index;
 copy[i++] = data[j++];
 copy[i++] = data[j++];
 copy[i++] = data[j++];
 copy[i] = data[j];
}

bool MortonTest(boost::shared_array<char> data, uint32 dx, uint32 dy, boost::shared_array<char> copy)
{
 // data types
 typedef unsigned __int32 uint32;
 typedef unsigned __int64 uint64;
 typedef std::tuple<uint32, uint32, uint32, uint32> tuple_t;

 // image properties
 if(!dx) return false;
 if(!dy) return false;
 uint32 datasize = 4*dx*dy;

 // dimensions cannot be zero
 if(dx == 0) return false;
 if(dy == 0) return false;

 // base case #1
 if((dx == 2) && (dy == 2)) {
    std::copy(data.get(), data.get() + datasize, copy.get());
    return true;
   }

 // base case #2
 if((dx == 1) && (dy == 1)) {
    std::copy(data.get(), data.get() + datasize, copy.get());
    return true;
   }

 // base case #3
 if((dx == 2) && (dy == 1)) {
    std::copy(data.get(), data.get() + datasize, copy.get());
    return true;
   }

 // base case #4
 if((dx == 1) && (dy == 2)) {
    std::copy(data.get(), data.get() + datasize, copy.get());
    return true;
   }

 // keep track of remapping index
 uint32 x0 = 0;
 uint32 y0 = 0;
 uint32 index = 0;

 // insert first item
 auto t0 = std::make_tuple(x0, y0, dx, dy);
 std::deque<tuple_t> stack;
 stack.push_back(t0);

 // while something is on the stack
 while(stack.size())
      {
       // get item from top of stack
       auto item = stack.front();
       stack.pop_front();

       // half dimensions
       uint32 dx1 = std::get<2>(item) / 2;
       uint32 dy1 = std::get<3>(item) / 2;
       uint32 dx2 = std::get<2>(item) - dx1; // if dx0 was 9, dx1 = 4 and dx2 = 5
       uint32 dy2 = std::get<3>(item) - dy1; // if dy0 was 9, dy1 = 4 and dy2 = 5

       // half positions
       uint32 hx1 = std::get<0>(item);
       uint32 hx2 = hx1 + dx1;
       uint32 hy1 = std::get<1>(item);
       uint32 hy2 = hy1 + dy1;

       // divide space into four Z sections
       tuple_t zt[4] = {
        std::make_tuple(hx1, hy1, dx1, dy1),
        std::make_tuple(hx2, hy1, dx2, dy1),
        std::make_tuple(hx1, hy2, dx1, dy2),
        std::make_tuple(hx2, hy2, dx2, dy2),
       };

       // process tuples
       for(uint32 i = 0; i < 4; i++)
          {
           uint32 curr_x0 = std::get<0>(zt[i]);
           uint32 curr_y0 = std::get<1>(zt[i]);
           uint32 curr_dx = std::get<2>(zt[i]);
           uint32 curr_dy = std::get<3>(zt[i]);

           // skip processing
           if(curr_dx == 0) continue;
           if(curr_dy == 0) continue;

           // requires further subdivision
           if(curr_dx > 2 || curr_dy > 2)
             {
              stack.push_back(zt[i]);
             }
           // does not require further subdivision
           else if(curr_dx == 1)
             {
              // (dx,dy) = (1,1)
              if(curr_dy == 1)
                {
                 MortonTestTransfer1(data, dx, dy, copy, curr_x0, curr_y0, index++);
                }
              // (dx,dy) = (1,2)
              else if(curr_dy == 2)
                {
                 MortonTestTransfer1(data, dx, dy, copy, curr_x0, curr_y0, index++);
                 MortonTestTransfer1(data, dx, dy, copy, curr_x0, curr_y0 + 1, index++);
                }
              // should never happen
              else
                 return false;
             }
           // does not require further subdivision
           else if(curr_dx == 2)
             {
              // (dx,dy) = (2,1)
              if(curr_dy == 1)
                {
                 MortonTestTransfer1(data, dx, dy, copy, curr_x0, curr_y0, index++);
                 MortonTestTransfer1(data, dx, dy, copy, curr_x0 + 1, curr_y0, index++);
                }
              // (dx,dy) = (2,2)
              else if(curr_dy == 2)
                {
                 uint32 px1 = curr_x0;
                 uint32 py1 = curr_y0;
                 uint32 px2 = curr_x0 + 1;
                 uint32 py2 = curr_y0 + 1;

                 // 12 (01.dds)
                 // 34
                 MortonTestTransfer1(data, dx, dy, copy, px1, py1, index++);
                 MortonTestTransfer1(data, dx, dy, copy, px2, py1, index++);
                 MortonTestTransfer1(data, dx, dy, copy, px1, py2, index++);
                 MortonTestTransfer1(data, dx, dy, copy, px2, py2, index++);

                 // 13 (02.dds)
                 // 24
                 // MortonTestTransfer1(data, dx, dy, copy, px1, py1, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px1, py2, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px2, py1, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px2, py2, index++);

                 // 21
                 // 43
                 // MortonTestTransfer1(data, dx, dy, copy, px2, py1, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px1, py1, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px2, py2, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px1, py2, index++);


                 // 24
                 // 13
                 // MortonTestTransfer1(data, dx, dy, copy, px1, py2, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px1, py1, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px2, py2, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px2, py1, index++);

                 // 21
                 // 43
                 // MortonTestTransfer1(data, dx, dy, copy, px2, py1, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px1, py1, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px2, py2, index++);
                 // MortonTestTransfer1(data, dx, dy, copy, px1, py2, index++);
                }
              // should never happen
              else
                 return false;
             }
           // should never happen
           else
              return false;
          }
      }

 return true;
}
