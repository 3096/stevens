#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "xbox360/xgraphics.h"
#include "x_dw_g1t.h"

#include "libraries/bc7/avpcl.h"

boost::shared_array<char> DWDecodeBC7(const char* data, size_t size, unsigned int dx, unsigned int dy)
{
 // simple test cases
 if(!data) return false;
 if(!size) return false;
 if(!dx || !dy) return boost::shared_array<char>();

 // block properties
 const unsigned int blocksize = 16;
 const unsigned int block_dx = 4;
 const unsigned int block_dy = 4;

 // compute expected size
 unsigned int block_rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 unsigned int block_cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 unsigned int n_blocks = block_rows*block_cols;
 unsigned int expected_size = blocksize*n_blocks;
 if(expected_size < size) return boost::shared_array<char>();

 // create RGBA data
 uint32 outpitch = 4*dx;
 uint32 outsize = outpitch*dy;
 boost::shared_array<char> outdata(new char[outsize]);

 // premultiplied
 uint32 left_offset[4] = { 0, outpitch, 2*outpitch, 3*outpitch };

 // for each block
 const char* block = data;
 Tile t(4, 4);
 for(uint32 i = 0; i < n_blocks; i++)
    {
     // decompress
     AVPCL::decompress(&block[0], t);

     // now map data to RGBA image
     uint32 base_x = (i % block_cols) * block_dx;
     uint32 base_y = (i / block_cols) * block_dy;
     uint32 base_index = 4*(dx*base_y + base_x);

     // convert char* to unsigned char* so we can assign 0 to 255 to items
     unsigned char* ptr = reinterpret_cast<unsigned char*>(outdata.get());

     // row1
     uint32 offset = base_index;
     ptr[offset++] = (uint08)t.data[0][0].Z();
     ptr[offset++] = (uint08)t.data[0][0].Y();
     ptr[offset++] = (uint08)t.data[0][0].X();
     ptr[offset++] = (uint08)t.data[0][0].W();
     ptr[offset++] = (uint08)t.data[0][1].Z();
     ptr[offset++] = (uint08)t.data[0][1].Y();
     ptr[offset++] = (uint08)t.data[0][1].X();
     ptr[offset++] = (uint08)t.data[0][1].W();
     ptr[offset++] = (uint08)t.data[0][2].Z();
     ptr[offset++] = (uint08)t.data[0][2].Y();
     ptr[offset++] = (uint08)t.data[0][2].X();
     ptr[offset++] = (uint08)t.data[0][2].W();
     ptr[offset++] = (uint08)t.data[0][3].Z();
     ptr[offset++] = (uint08)t.data[0][3].Y();
     ptr[offset++] = (uint08)t.data[0][3].X();
     ptr[offset]   = (uint08)t.data[0][3].W();

     // row2
     offset = base_index + left_offset[1];
     ptr[offset++] = (uint08)t.data[1][0].Z();
     ptr[offset++] = (uint08)t.data[1][0].Y();
     ptr[offset++] = (uint08)t.data[1][0].X();
     ptr[offset++] = (uint08)t.data[1][0].W();
     ptr[offset++] = (uint08)t.data[1][1].Z();
     ptr[offset++] = (uint08)t.data[1][1].Y();
     ptr[offset++] = (uint08)t.data[1][1].X();
     ptr[offset++] = (uint08)t.data[1][1].W();
     ptr[offset++] = (uint08)t.data[1][2].Z();
     ptr[offset++] = (uint08)t.data[1][2].Y();
     ptr[offset++] = (uint08)t.data[1][2].X();
     ptr[offset++] = (uint08)t.data[1][2].W();
     ptr[offset++] = (uint08)t.data[1][3].Z();
     ptr[offset++] = (uint08)t.data[1][3].Y();
     ptr[offset++] = (uint08)t.data[1][3].X();
     ptr[offset]   = (uint08)t.data[1][3].W();

     // row3
     offset = base_index + left_offset[2];
     ptr[offset++] = (uint08)t.data[2][0].Z();
     ptr[offset++] = (uint08)t.data[2][0].Y();
     ptr[offset++] = (uint08)t.data[2][0].X();
     ptr[offset++] = (uint08)t.data[2][0].W();
     ptr[offset++] = (uint08)t.data[2][1].Z();
     ptr[offset++] = (uint08)t.data[2][1].Y();
     ptr[offset++] = (uint08)t.data[2][1].X();
     ptr[offset++] = (uint08)t.data[2][1].W();
     ptr[offset++] = (uint08)t.data[2][2].Z();
     ptr[offset++] = (uint08)t.data[2][2].Y();
     ptr[offset++] = (uint08)t.data[2][2].X();
     ptr[offset++] = (uint08)t.data[2][2].W();
     ptr[offset++] = (uint08)t.data[2][3].Z();
     ptr[offset++] = (uint08)t.data[2][3].Y();
     ptr[offset++] = (uint08)t.data[2][3].X();
     ptr[offset]   = (uint08)t.data[2][3].W();

     // row4
     offset = base_index + left_offset[3];
     ptr[offset++] = (uint08)t.data[3][0].Z();
     ptr[offset++] = (uint08)t.data[3][0].Y();
     ptr[offset++] = (uint08)t.data[3][0].X();
     ptr[offset++] = (uint08)t.data[3][0].W();
     ptr[offset++] = (uint08)t.data[3][1].Z();
     ptr[offset++] = (uint08)t.data[3][1].Y();
     ptr[offset++] = (uint08)t.data[3][1].X();
     ptr[offset++] = (uint08)t.data[3][1].W();
     ptr[offset++] = (uint08)t.data[3][2].Z();
     ptr[offset++] = (uint08)t.data[3][2].Y();
     ptr[offset++] = (uint08)t.data[3][2].X();
     ptr[offset++] = (uint08)t.data[3][2].W();
     ptr[offset++] = (uint08)t.data[3][3].Z();
     ptr[offset++] = (uint08)t.data[3][3].Y();
     ptr[offset++] = (uint08)t.data[3][3].X();
     ptr[offset]   = (uint08)t.data[3][3].W();

     // move to next block
     block += blocksize;
    }

 return outdata;
}

namespace DYNASTY_WARRIORS {

bool G1T_PathExtract(LPCTSTR pathname, bool rbo)
{
 using namespace std;
 cout << "Processing .G1T files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".G1T"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!G1T_FileExtract(filelist[i].c_str(), rbo)) return false;
    }
 cout << endl;
 return true;
}

bool G1T_FileExtract(LPCTSTR filename, bool rbo)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // read magic number
 if(ifile.fail()) return error("Seek failure.");
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 ifile.seekg(0);

 // read file into memory
 boost::shared_array<char> data(new char[filesize]);
 ifile.read(data.get(), filesize);
 if(ifile.fail()) return error("Read failure.");
 ifile.close();

 // delete state
 bool delete_this = true;
 bool debug = false;

 // set storage mode
 endian_mode mode;
 if(magic == 0x47315447ul) mode = ENDIAN_LITTLE;
 else if(magic == 0x47543147ul) mode = ENDIAN_BIG;
 else return error("Not a G1T file.");

 // convert to binary stream
 binary_stream bs(data, filesize);
 bs.set_endian_mode(mode);

 // read header
 uint32 h01 = bs.read_uint32(); // magic
 uint32 h02 = bs.read_uint32(); // version
 uint32 h03 = bs.read_uint32(); // section size
 uint32 h04 = bs.read_uint32(); // table offset
 uint32 h05 = bs.read_uint32(); // number of textures
 uint32 h06 = bs.read_uint32(); // unknown (when this is 0x09, textures are funky???)
 uint32 h07 = bs.read_uint32(); // size of section after offset table, contains 0x24-byte entries
 if(bs.fail()) return error("Stream read failure.");

 // validate
 if(h01 != 0x47315447ul) return error("Not a G1T file.");
 if(h05 == 0) return true; // nothing to do

 // read flag table
 boost::shared_array<uint32> flagtable(new uint32[h05]);
 bs.read_array(flagtable.get(), h05);
 if(bs.fail()) return error("Stream read failure.");

 // create save path
 // only do so if number of files is greater than one
 STDSTRING savepath = pathname;
 if(h05 > 1) {
    savepath += shrtname;
    savepath += L"_t"; // to indicate this is a texture folder
    savepath += L"\\";
    CreateDirectory(savepath.c_str(), NULL);
   }

 // move to table
 bs.seek(h04);
 if(bs.fail()) return error("Stream seek failure.");

 // read offset table
 deque<size_t> offset_list;
 for(size_t i = 0; i < h05; i++) {
     uint32 item = bs.read_uint32();
     offset_list.push_back(item);
     if(bs.fail()) return error("Stream read failure.");
    }

 // process textures
 for(size_t i = 0; i < offset_list.size(); i++)
    {
     // move to texture header
     uint32 position = h04 + offset_list[i];
     bs.seek(position);
     if(bs.fail()) return error("Stream seek failure.");

     // compute next texture header position to measure size
     uint32 texture_section_size = 0;
     if(i < (offset_list.size() - 1)) texture_section_size = offset_list[i + 1] - offset_list[i];
     else texture_section_size = filesize - offset_list[i];

     // read texture information
     // 0x08 bytes
     uint08 b1 = bs.read_uint08(); // unknown
     uint08 b2 = bs.read_uint08(); // texture type
     uint08 b3 = bs.read_uint08(); // dx/dy
     uint08 b4 = bs.read_uint08(); // unknown
     uint16 s1 = bs.read_uint08(); // unknown
     uint16 s2 = bs.read_uint08(); // unknown
     uint16 s3 = bs.read_uint08(); // unknown
     uint16 s4 = bs.read_uint08(); // unknown
     if(bs.fail()) return error("Stream read failure.");

/*
     // 10 5B 75 00 00 10 21 00 (not extended)
     // 10 5B AB 00 00 10 21 10 (extended)
     // header is extended depending on s4 by 0x0C bytes
     bool extended = ((s4 & 0x10) > 0);
     if(s1 == 0x11 && s2 == 0x11 && s3 == 0x12 && s4 == 0x01) extended = true;
     else if(s1 == 0x00 && s2 == 0x01 && s3 == 0x12 && s4 == 0x01) extended = true;
     if(extended) {
        uint32 v1 = bs.read_uint32(); // 0x0C
        uint32 v2 = bs.read_uint32(); // 0
        uint32 v3 = bs.read_uint32(); // 0x00 or 0x01
        if(bs.fail()) return error("Stream read failure.");
       }
*/

     // is header extended heuristic
     size_t saved = bs.tell();
     uint32 v1 = bs.read_uint32(); // 0x0C
     uint32 v2 = bs.read_uint32(); // 0
     uint32 v3 = bs.read_uint32(); // 0x00 or 0x01, but can be anything
     // 3096: this fails for some reason, so I need it to ingore this
     // if(bs.fail()) return error("Stream read failure.");
     if(bs.fail()) return true;
     bool extended = false;
     if((v1 == 0x0C) && (v2 == 0x00) /*&& (v3 == 0x00 || v3 == 0x01 || v3 == 0x01000000)*/) extended = true;
     if(!extended) bs.seek(saved); // restore pointer

     // compute dimensions
     uint32 temp1 = ((b3 & 0xF0) >> 4);
     uint32 temp2 = ((b3 & 0x0F));
     uint32 dx = 1 << temp1;
     uint32 dy = 1 << temp2;

     // swap dx and dy if little endian
     if(mode == ENDIAN_LITTLE)
        swap(dx, dy);

     // debug
     if(debug) {
        cout << "position = 0x" << hex << position << dec << endl;
        cout << "texture_section_size = 0x" << hex << texture_section_size << dec << endl;
        cout << "extended = " << (extended ? "true" : "false") << endl;
        cout << "dx = 0x" << hex << dx << dec << endl;
        cout << "dy = 0x" << hex << dy << dec << endl;
        cout << "type = 0x" << hex << (uint16)b2 << dec << endl;
       }

     // create texture filename
     STDSTRINGSTREAM ss;
     ss << savepath;
     if(h05 > 1) ss << setfill(TEXT('0')) << setw(3) << i << TEXT(".dds"); // name by index
     else ss << shrtname << TEXT(".dds"); // name same as original

     // TEXTURE TYPE 0x00
     // UNCOMPRESSED ABGR
     // WORKS PERFECT
     // EXAMPLE: [PS3] Berserk Musou (2016)
     if(b2 == 0x00)
       {
        // read texture
        DWORD filesize = 4*dx*dy;
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // PHOTOSHOP ONLY SUPPORTS ONE TYPE OF 32-BIT UNCOMPRESSED!
        // YOU MUST USE ARGB COMPRESSED WITH LINEARSIZE (NOT PITCH)!
        // swap channels so shit loads in photoshop
        // RGBA -> ARGB
        uint32 n_pixels = dx * dy;
        for(uint32 j = 0; j < n_pixels; j++) {
            uint32 index = 4*j;
            auto c1 = buffer[4*j + 0]; // this is A
            auto c2 = buffer[4*j + 1]; // this is B
            auto c3 = buffer[4*j + 2]; // this is G
            auto c4 = buffer[4*j + 3]; // this is R
            buffer[4*j + 0] = c2; // B must be here
            buffer[4*j + 1] = c3; // G must be here
            buffer[4*j + 2] = c4; // R must be here
            buffer[4*j + 3] = c1; // A must be here
           }

        // save texture
        DDS_HEADER ddsh;
        CreateA8R8G8B8DDSHeader(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x01
     // UNCOMPRESSED BGRA 32-BIT (no mipmaps)
     // WORKS PERFECT
     else if(b2 == 0x01)
       {
        // read texture
        DWORD filesize = 4*dx*dy;
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // PHOTOSHOP ONLY SUPPORTS ONE TYPE OF 32-BIT UNCOMPRESSED!
        // YOU MUST USE ARGB!
        uint32 mask1 = 0xFF000000; // A
        uint32 mask2 = 0x00FF0000; // R 
        uint32 mask3 = 0x0000FF00; // G
        uint32 mask4 = 0x000000FF; // B

        // swap channels so shit loads in photoshop
        uint32 n_pixels = dx * dy;
        for(uint32 j = 0; j < n_pixels; j++) {
            uint32 index1 = 4*j;
            uint32 index2 = index1 + 1;
            uint32 index3 = index2 + 1;
            uint32 index4 = index3 + 1;
            swap(buffer[index1], buffer[index4]); // swap A and B
            swap(buffer[index2], buffer[index3]); // swap R and G
           }

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, 0, 0x20, mask2, mask3, mask4, mask1, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x06
     // DXT1
     // WORKS PERFECT
     else if(b2 == 0x06)
       {
        // read texture
        DWORD filesize = DXT1Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // save texture
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x07
     // DXT3
     // WORKS PERFECT
     else if(b2 == 0x07)
       {
        // read texture
        DWORD filesize = DXT3Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // save texture
        DDS_HEADER ddsh;
        CreateDXT3Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x08
     // DXT5
     // WORKS PERFECT
     else if(b2 == 0x08)
       {
        // read texture
        DWORD filesize = DXT5Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // save texture
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x10
     // DXT1 XBOX360 SWIZZLED (only seen on xbox360 and PC)
     // WORKS PERFECT
     else if(b2 == 0x10)
       {
        // read texture
        DWORD filesize = DXT1Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);
        if(ifile.fail()) return error("Read failure.");

        // get pitch and GPU format
        DWORD blocksize = 8;
        DWORD pitch = (dx/4)* blocksize;
        DWORD format = XGGetGpuFormat(D3DFMT_DXT1);

        // untile data
        boost::shared_array<char> untile(new char[filesize]);
        XGEndianSwapSurface(untile.get(), pitch, buffer.get(), pitch, dx, dy, D3DFMT_DXT1);
        swap(buffer, untile);
        XGUntileTextureLevel(dx, dy, 0, format, XGTILE_NONPACKED, untile.get(), pitch, NULL, buffer.get(), NULL);

        // save texture
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x12
     // DXT5 XBOX360 SWIZZLED (only seen on xbox360 and PC)
     // WORKS PERFECT
     else if(b2 == 0x12)
       {
        // read texture
        DWORD filesize = DXT5Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);
        if(ifile.fail()) return error("Read failure.");

        // get pitch and GPU format
        DWORD blocksize = 16;
        DWORD pitch = (dx/4)* blocksize;
        DWORD format = XGGetGpuFormat(D3DFMT_DXT5);

        // untile data
        boost::shared_array<char> untile(new char[filesize]);
        XGEndianSwapSurface(untile.get(), pitch, buffer.get(), pitch, dx, dy, D3DFMT_DXT5);
        swap(buffer, untile);
        XGUntileTextureLevel(dx, dy, 0, format, XGTILE_NONPACKED, untile.get(), pitch, NULL, buffer.get(), NULL);

        // save texture
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x59
     // DXT1 (may or mar not have mipmaps down to 0x4-by-0x4)
     // WORKS PERFECT
     // [PC] TOUKIDEN KIWAMI (2015)
     else if(b2 == 0x59)
       {
        // read texture
        DWORD filesize = DXT1Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // save texture
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x5B
     // DXT5
     // WORKS PERFECT
     // [PC] TOUKIDEN KIWAMI (2015)
     else if(b2 == 0x5B)
       {
        // read texture
        DWORD filesize = DXT5Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // save texture
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x5C
     // ATI1
     // WORKS PERFECT
     // [PC] ATTACK ON TITAN: WINGS OF FREEDOM (2016)
     else if(b2 == 0x5C)
       {
        // read texture
        DWORD filesize = ATI1Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // create texture filename
        STDSTRINGSTREAM ss;
        ss << savepath;
        if(h05 > 1) ss << setfill(TEXT('0')) << setw(3) << i << TEXT("_ATI1.dds"); // name by index
        else ss << shrtname << TEXT("_ATI1.dds"); // name same as original

        // save texture
        DDS_HEADER ddsh;
        CreateATI1Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x5D
     // DXT5 SWIZZLED or ATI2
     // UNSUPPORTED
     // [PC] TOUKIDEN KIWAMI (2015)
     else if(b2 == 0x5D)
       {
        delete_this = false;

        // read texture
        DWORD filesize = ATI2Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // create texture filename
        STDSTRINGSTREAM ss;
        ss << savepath;
        if(h05 > 1) ss << setfill(TEXT('0')) << setw(3) << i << TEXT("_ATI2.dds"); // name by index
        else ss << shrtname << TEXT("_ATI2.dds"); // name same as original

        // save texture
        DDS_HEADER ddsh;
        CreateATI2Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x5E
     // BC6H CUBEMAPS
     // WORKS PERFECT
     // [PC] TOUKIDEN KIWAMI (2015)
     else if(b2 == 0x5E)
       {
        // is this a 1x or a 6x cubemap
        bool six = true;
        UINT32 datasize = (extended ? (texture_section_size - 0x14) : (texture_section_size - 0x08));
        UINT n = datasize/GetBC6DataSize(dx, dy, 0);
        if(n == 0) return error("The number of BC6H images cannot be zero.");
        n = clamp(n, (UINT)1, (UINT)6);
        if(n == 6) cout << "CUBEMAP!" << endl;

        // determine filesize
        DWORD filesize = GetBC6DataSize(dx, dy, 0)*n;
        if(debug) cout << "reading 0x" << hex << filesize << dec << " bytes" << endl;

        // read texture
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // create texture filename
        STDSTRINGSTREAM ss;
        ss << savepath;
        if(h05 > 1) ss << setfill(TEXT('0')) << setw(3) << i << TEXT("_BC6H.dds");
        else ss << shrtname << TEXT("_BC6H.dds"); // name same as original

        // save texture
        DDS_HEADER ddsh;
        DDS_HEADER_DXT10 extend;
        if(n == 6) CreateBC6Header(dx, dy, 0, TRUE, &ddsh, &extend);
        else CreateBC6Header(dx, dy, 0, FALSE, &ddsh, &extend);
        if(!SaveDDS10File(ss.str().c_str(), ddsh, extend, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x5F
     // BC7
     // WORKS PERFECT
     // [PC] ATTACK ON TITAN: WINGS OF FREEDOM (2016)
     else if(b2 == 0x5F)
       {
        // read texture
        DWORD filesize = GetBC7DataSize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // create texture filename
        STDSTRINGSTREAM ss;
        ss << savepath;
        if(h05 > 1) ss << setfill(TEXT('0')) << setw(3) << i << TEXT("_BC7.dds");
        else ss << shrtname << TEXT("_BC7.dds"); // name same as original

        // save texture
        DDS_HEADER ddsh;
        DDS_HEADER_DXT10 extend;
        CreateBC7Header(dx, dy, 0, FALSE, &ddsh, &extend);
        if(!SaveDDS10File(ss.str().c_str(), ddsh, extend, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x60
     // DXT1 SWIZZLED?
     // UNSUPPORTED
     // [PC] ATTACK ON TITAN: WINGS OF FREEDOM (2016)
     else if(b2 == 0x60)
       {
        delete_this = false;

        cout << "position = 0x" << hex << bs.tell() << dec << endl;
        cout << "dx = 0x" << hex << dx << dec << endl;
        cout << "dy = 0x" << hex << dy << dec << endl;
        cout << "format = 0x" << hex << (uint32)b2 << dec << endl;
        cout << "computed filesize = 0x" << hex << DXT1Filesize(dx, dy, ComputeMipMapNumber(dx, dy)) << dec << endl;

        // read texture
        DWORD filesize = DXT1Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // create texture filename
        STDSTRINGSTREAM ss;
        ss << savepath;
        if(h05 > 1) ss << setfill(TEXT('0')) << setw(3) << i << TEXT("_") << hex << setw(2) << (uint16)b2 << dec << TEXT(".dds");
        else ss << shrtname << TEXT("_") << hex << setw(2) << (uint16)b2 << dec << TEXT(".dds");

        // save texture
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }
     // TEXTURE TYPE 0x62
     // DXT5 SWIZZLED
     // WORKS PERFECT BUT SWIZZLED
     // [PC] TOUKIDEN KIWAMI (2015)
     else if(b2 == 0x62)
       {
        delete_this = false;

        // don't delete
        cout << "position = 0x" << hex << bs.tell() << dec << endl;
        cout << "dx = 0x" << hex << dx << dec << endl;
        cout << "dy = 0x" << hex << dy << dec << endl;
        cout << "format = 0x" << hex << (uint32)b2 << dec << endl;

        // read texture
        DWORD filesize = DXT5Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        bs.read((char*)buffer.get(), filesize);
        if(bs.fail()) return error("Stream read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // create texture filename
        STDSTRINGSTREAM ss;
        ss << savepath;
        if(h05 > 1) ss << setfill(TEXT('0')) << setw(3) << i << TEXT("_") << hex << setw(2) << (uint16)b2 << dec << TEXT(".dds");
        else ss << shrtname << TEXT("_") << hex << setw(2) << (uint16)b2 << dec << TEXT(".dds");

        // save texture
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, 0, FALSE, &ddsh);
        if(!SaveDDSFile(ss.str().c_str(), ddsh, buffer, filesize)) return error("DDS save failure.", __LINE__);
       }

     // NOT REVERIFIED AFTER THIS!!!

     // TEXTURE TYPE 0x02
     // UNKNOWN FLOAT32 FORMAT
     // [2017] Dynasty Warriors All-Stars\0271.g1t - 0297.g1t
     else if(b2 == 0x02)
       {
        // don't delete
        delete_this = false;
       }
     // TEXTURE TYPE 0x03
     // UNKNOWN
     // ARSLAN\LINKDATA\0127-0138\000.g1t
     else if(b2 == 0x03)
       {
        delete_this = false;
       }
     // TEXTURE TYPE 0x04
     // UNKNOWN
     // [2018] Dynasty Warriors 9 [PC]\LINKIDX_001\000.g1t
     else if(b2 == 0x04)
       {
        delete_this = false;
       }
    else
        return error("Unsupported texture format.");

     // new line
     if(debug) cout << endl;
    }

 // delete G1T file
 if(delete_this)
    DeleteFile(filename);

 return true;
}

}
