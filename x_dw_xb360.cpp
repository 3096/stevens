#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "xbox360/xgraphics.h"
#include "x_dw_g1m.h"
#include "x_dw_xb360.h"
using namespace std;

#define X_SYSTEM XB360
#define X_GAME   DYNASTY_WARRIORS

static bool EXCLUDE_FILES = true;
static bool DELETE_SMALL_FILES = true;
static UINT SMALL_FILESIZE = 0x100;

//
// STRUCTURES
//

namespace X_SYSTEM { namespace X_GAME {

};};

//
// UTILITIES
//

namespace X_SYSTEM { namespace X_GAME {

bool DWIsBinTypeA(LPCTSTR filename)
{
 // uint32: 0x00102A49
 // uint32: number of files
 // uint32: 0x00000800 (offset scalar)
 // uint32: 0x00 (0x10-byte items) or 0x01 (0x08-byte items)
 // list of items
 // list of files

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x08) return false;

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(magic != 0x00102A49) return false;

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(n_files == 0) return false;
 if(n_files > 0xFFFF) return false; // too many files!!!

 // read scale offset (always 0x800)
 uint32 scale = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(scale != 0x800) return false;

 // read item type
 uint32 type = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 return (type == 0x00 || type == 0x01);
}

bool DWIsGfx_ListA(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x08) return false;

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 return (magic == 0x01332D37);
}

bool DWIsArchiveType1(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #1
 // #FILES, [OFFSET/LENGTH], [DATA]
 //

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x08) return false;

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // validate number of files
 if(n_files == 0) return false;
 if(!(n_files < 0xFFFF)) return false;

 // compute expected header sizes
 uint32 headersize1 = 0x04 + (n_files * 0x08);
 uint32 headersize2 = 0x04 + (n_files * 0x08);
 headersize2 = ((headersize1 + 0x0F) & (~0x0F));

 // data
 struct ARC1ITEM { uint32 offset; uint32 length; };
 typedef pair<uint32, ARC1ITEM> pair_t;
 deque<pair_t> entries;
 uint32 prev_offset = 0xFFFFFFFF;
 uint32 prev_length = 0xFFFFFFFF;
 uint32 prev_finish = 0xFFFFFFFF;

 // read and insert pairs
 for(uint32 i = 0; i < n_files; i++)
    {
     // read pair
     uint32 curr_offset = BE_read_uint32(ifile);
     uint32 curr_length = BE_read_uint32(ifile);
     uint32 curr_finish = curr_offset + curr_length;
     if(ifile.fail()) return false;

     // ignore pair
     if(curr_offset == 0 && curr_length == 0)
        continue;

     // ignore pair
     if(curr_offset != 0 && curr_length == 0)
        continue;

     // invalid pair
     if(curr_offset == 0 && curr_length != 0)
        return false;

     // offset and length must be less than filesize
     if(!(curr_offset < filesize)) return false;
     if(!(curr_length < filesize)) return false;

     // offset + length must be less than or equal to filesize
     if(filesize < curr_finish) return false;

     // compare with previous entries (check for overlapping data)
     if((prev_offset != 0xFFFFFFFF) && (prev_length != 0xFFFFFFFF)) {
        if(!(prev_offset < curr_offset)) return false; // data overlaps
        if(!(prev_finish < curr_offset)) return false; // data overlaps
       }

     // insert pair
     ARC1ITEM item;
     item.offset = curr_offset;
     item.length = curr_length;
     entries.push_back(pair_t(i, item));
    }

 // must have at least one entry
 if(entries.size() == 0) return false;

 // first entry should match header size (non-aligned or aligned)
 uint32 head_offset = entries[0].second.offset;
 if((head_offset != headersize1) && (head_offset != headersize2)) return false;

 // final entry should having finish close to the end
 uint32 last_offset = entries.back().second.offset;
 uint32 last_length = entries.back().second.length;
 if(!((filesize - (last_offset + last_length)) < 0x10)) return false;

 // most likely is
 return true;
}

bool DWIsArchiveType2(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #2
 // [OFFSET/LENGTH], [DATA]
 //

 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0xFF) return false; // most likely not

 // pair types
 struct ARC1ITEM { uint32 offset; uint32 length; };
 typedef pair<uint32, ARC1ITEM> pair_t;
 deque<pair_t> entries;
 uint32 prev_offset = 0xFFFFFFFF;
 uint32 prev_length = 0xFFFFFFFF;
 uint32 prev_finish = 0xFFFFFFFF;

 // read pairs until we hit an invalid pair or until we come to the end of the data
 uint32 n_files = 0;
 for(uint32 index = 0; index < 0xFFFF; index++)
    {
     // read pair
     uint32 curr_offset = BE_read_uint32(ifile);
     uint32 curr_length = BE_read_uint32(ifile);
     uint32 curr_finish = curr_offset + curr_length;
     if(ifile.fail()) break;

     // null entry
     if((curr_offset == 0) && (curr_length == 0)) {
        n_files++;
        continue;
       }

     // null entry
     if((curr_offset != 0) && (curr_length == 0)) {
        n_files++;
        continue;
       }

     // impossible entry
     if((curr_offset == 0) && (curr_length != 0))
        break;

     // offset and length must be less than filesize
     if(!(curr_offset < filesize)) break;
     if(!(curr_length < filesize)) break;

     // offset + length must be less than or equal to filesize
     if(filesize < curr_finish) break;

     // compare with previous entries (check for overlapping data)
     if((prev_offset != 0xFFFFFFFF) && (prev_length != 0xFFFFFFFF)) {
        if(curr_offset < prev_offset) break; // data overlaps
        if(curr_offset < prev_finish) break; // data overlaps
       }

     // assume entry to be valid
     ARC1ITEM item;
     item.offset = curr_offset;
     item.length = curr_length;
     entries.push_back(pair_t(index, item));

     // check for final entry
     n_files++;
     if(curr_finish == filesize) break;

     // entry is now old
     prev_offset = curr_offset;
     prev_length = curr_length;
     prev_finish = curr_finish;
    }

 // we must have at least one entry
 if(!entries.size()) return false;

 // compute expected header sizes
 uint32 headersize1 = 0x04 * n_files;
 uint32 headersize2 = 0x04 * n_files;
 headersize2 = ((headersize1 + 0x0F) & (~0x0F));

 // first entry must match expected header size (non-aligned or aligned)
 uint32 head_offset = entries[0].second.offset;
 if((head_offset != headersize1) && (head_offset != headersize2)) return false;

 // final valid entry should having finish close to the end
 uint32 last_offset = entries.back().second.offset;
 uint32 last_length = entries.back().second.length;
 uint32 last_finish = last_offset + last_length;
 if(!((filesize - last_finish) < 0x10)) return false;

 // most likely
 return true;
}

bool DWIsArchiveType3(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #3
 // #FILES, [LENGTH], [DATA]
 // DATA IS ALWAYS 0x10-BYTE ALIGNED
 //

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for IDX file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x08) return false;

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // validate number of files
 if(n_files == 0) return false;
 if(!(n_files < 0xFFFF)) return false;

 // read sizes
 deque<uint32> sizelist;
 uint64 databytes = 0;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read size
     uint32 size = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // validate size
     if(size == 0) continue; // OK to have zero-length data
     if(!(size < 0x2000000ul)) return false; // size is too long
     if(size % 0x10) return false; // always 0x10-byte aligned

     // insert size
     sizelist.push_back(size);
     databytes += size;
    }

 // sum should not be zero
 if(databytes == 0) return false;

 // computed expected filesize
 uint64 expected_filesize = (0x04*(n_files + 1)) + databytes;
 if(filesize < expected_filesize) return false;
 if((filesize - expected_filesize) > 0x0C) return false;

 // most likely is
 return true;
}

bool DWIsArchiveType4(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #4
 // #FILES, [OFFSET], [DATA]
 // THIS IS CHECKED FOR LAST SINCE WE CAN'T VALIDATE EXPECTED FILESIZE LIKE WE
 // CAN DO WITH THE OTHER ARCHIVES. THIS ARCHIVE IS TYPICALLY FILLED WITH G1A_
 // AND G2A_ FILES. THIS TYPE IS ASSUMED TO BE 0x10-BYTE ALIGNED. ARCHIVE TYPE
 // #5 IS THE NON-ALIGNED VERSION OF THIS ARCHIVE.

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for IDX file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x20) return false;

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // validate number of files
 if(n_files == 0) return false;
 if(!(n_files < 0xFFFF)) return false; // too many!

 // compute expected first offset
 uint32 expected_first = 0x04 + (n_files * 0x04);
 expected_first = ((expected_first + 0x0F) & (~0x0F));

 // read offsets (0x10-byte aligned and can be zero)
 typedef pair<uint32, uint32> pair_t;
 deque<pair_t> offsets;
 uint32 prev_offset = 0xFFFFFFFF;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read offset
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // skip zero offsets
     if(item == 0) continue;

     // offset must be LESS THAN OR EQUAL TO the filesize
     // if offset is EQUAL to the filesize, last file is zero bytes
     if(filesize < item) return false;
        
     // offset must be divisible by 0x10
     if(item % 0x10) return false;
        
     // offset must be LESS THAN OR EQUAL TO the filesize
     // if offset is EQUAL to the filesize, last file is zero bytes
     if(filesize < item) return false;
        
     // offset must be GREATER THAN OR EQUAL to the previous offset
     if((prev_offset != 0xFFFFFFFF) && (item < prev_offset))
        return false;
        
     // insert offset for now
     offsets.push_back(pair_t(i, item));
     prev_offset = item;
    }

 // first offset must be equal to the expected offset
 if(!offsets.size()) return false;
 return (offsets[0].second == expected_first);
}

bool DWIsArchiveType5(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #5
 // #FILES, [OFFSET], [DATA]
 // THIS IS CHECKED FOR LAST SINCE WE CAN'T VALIDATE EXPECTED FILESIZE LIKE WE
 // CAN DO WITH THE OTHER ARCHIVES. THIS ARCHIVE IS TYPICALLY FILLED WITH G1A_
 // AND G2A_ FILES. THIS TYPE IS THE NON-ALIGNED VERSION OF ARCHIVE TYPE #4.

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for IDX file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x20) return false;

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // validate number of files
 if(n_files == 0) return false;
 if(!(n_files < 0xFFF)) return false; // too many!

 // compute expected first offset
 uint32 expected_first1 = 0x04 + (n_files * 0x04); // aligned
 uint32 expected_first2 = 0x04 + (n_files * 0x04); // non-aligned
 expected_first1 = ((expected_first1 + 0x0F) & (~0x0F));

 // read offsets
 typedef pair<uint32, uint32> pair_t;
 deque<pair_t> offsets;
 uint32 prev_offset = 0xFFFFFFFF;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read offset
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // skip zero offsets
     if(item == 0) continue;

     // offset must be LESS THAN OR EQUAL TO the filesize
     // if offset is EQUAL to the filesize, last file is zero bytes
     if(filesize < item) return false;
        
     // offset must be GREATER THAN OR EQUAL to the previous offset
     if((prev_offset != 0xFFFFFFFF) && (item < prev_offset))
        return false;
        
     // insert offset for now
     offsets.push_back(pair_t(i, item));
     prev_offset = item;
    }

 // must have at least one offset
 if(!offsets.size()) return false;

 // first offset must be equal to one of the expected offsets
 if(offsets[0].second == expected_first1) return true;
 if(offsets[0].second == expected_first2) return true;
 return false;
}

bool DWIsArchiveType6(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #6
 // 0x1EDE, #FILES, SCALE, 0x00, [SCALED OFFSET/LENGTH], [DATA]
 // USED BY OLDER DW GAMES THAT USE ANS, BNS, CNS ARCHIVES

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for IDX file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x20) return false;

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(magic != 0x1EDE) return false;

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(n_files == 0) return false;
 if(!(n_files < 0xFFFF)) return false; // too many!

 // read scale
 uint32 scale = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(scale % 0x10) return false;

 // read zero
 uint32 zero = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(zero != 0x00) return false;

 // compute expected header sizes
 uint32 headersize1 = 0x10 + (n_files * 0x08);
 uint32 headersize2 = 0x10 + (n_files * 0x08);
 headersize2 = ((headersize1 + 0x0F) & (~0x0F));

 // data
 struct ARC1ITEM { uint32 offset; uint32 length; };
 typedef pair<uint32, ARC1ITEM> pair_t;
 deque<pair_t> entries;
 uint32 prev_offset = 0xFFFFFFFF;
 uint32 prev_length = 0xFFFFFFFF;
 uint32 prev_finish = 0xFFFFFFFF;

 // read and insert pairs
 for(uint32 i = 0; i < n_files; i++)
    {
     // read pair
     uint32 curr_offset = BE_read_uint32(ifile) * scale;
     uint32 curr_length = BE_read_uint32(ifile);
     uint32 curr_finish = curr_offset + curr_length;
     if(ifile.fail()) return false;

     // ignore pair
     if(curr_offset == 0 && curr_length == 0)
        continue;

     // ignore pair
     if(curr_offset != 0 && curr_length == 0)
        continue;

     // invalid pair
     if(curr_offset == 0 && curr_length != 0)
        return false;

     // offset and length must be less than filesize
     if(!(curr_offset < filesize)) return false;
     if(!(curr_length < filesize)) return false;

     // offset + length must be less than or equal to filesize
     if(filesize < curr_finish) return false;

     // compare with previous entries (check for overlapping data)
     if((prev_offset != 0xFFFFFFFF) && (prev_length != 0xFFFFFFFF)) {
        if(!(prev_offset < curr_offset)) return false; // data overlaps
        if(!(prev_finish < curr_offset)) return false; // data overlaps
       }

     // insert pair
     ARC1ITEM item;
     item.offset = curr_offset;
     item.length = curr_length;
     entries.push_back(pair_t(i, item));
    }

 // must have at least one entry
 if(entries.size() == 0) return false;

 // first entry should match header size (non-aligned or aligned)
 uint32 head_offset = entries[0].second.offset;
 if((head_offset != headersize1) && (head_offset != headersize2)) return false;

 // final entry should having finish close to the end
 uint32 last_offset = entries.back().second.offset;
 uint32 last_length = entries.back().second.length;
 if(!((filesize - (last_offset + last_length)) < 0x10)) return false;

 // most likely is
 return true;
}

bool DWIsIdxZrc(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for IDX file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filesize must be greater than 0x80
 // this is because the first ZLIB chunk is 0x80-byte aligned
 if(filesize < 0x80) return false;

 // read magic number
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // read number of chunks
 uint32 n_chunks = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // must have chunks
 if(!n_chunks) return false;
 if(!(n_chunks < 0xFFFF)) return false;

 // read decompressed size
 uint32 decombytes = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // decompressed bytes cannot be zero
 if(decombytes == 0) return false;

 // read chunk offsets
 boost::shared_array<uint32> sizes(new uint32[n_chunks]);
 if(!BE_read_array(ifile, sizes.get(), n_chunks)) return false;

 // save current position
 uint32 position = (uint32)ifile.tellg();
 if(ifile.fail()) return false;

 // move to nearest 0x80-byte alignment
 position = ((position + 0x7F) & ~(0x7F));
 ifile.seekg(position);
 if(ifile.fail()) return false;

 // read size of data
 uint32 n = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // must have something to decompress
 if(n == 0) return false;

 // read next two bytes
 uint08 b1 = BE_read_uint08(ifile);
 uint08 b2 = BE_read_uint08(ifile);
 if(ifile.fail()) return false;

 // is the 1st chunk a ZLIB chunk?
 // if(isZLIB(b1, b2)) return true;
 if(b1 != 0x78) return false;
 if(b2 != 0xDA) return false;
 return true;
}

bool DWIsIdxZrc_TypeA(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for IDX file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filesize must be greater than 0x80
 // this is because the first ZLIB chunk is 0x80-byte aligned
 if(filesize < 0x80) return false;

 // read number of chunks
 uint32 n_chunks = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // validate chunks
 // 0x1000 chunks has maximum filesize = (0x1000 * 0xFFFF) = 255 MB
 // 0x4000 chunks has maximum filesize = (0x4000 * 0xFFFF) = 1 GB
 if(!n_chunks) return false; // must have at least one chunk
 if(!(n_chunks < 0x4000)) return false; // must be less than 0x4000 chunks

 // read decompressed size
 uint32 decombytes = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(decombytes == 0) return false; // decompressed size cannot be zero

 // read chunk sizes
 boost::shared_array<uint32> sizes(new uint32[n_chunks]);
 if(!BE_read_array(ifile, sizes.get(), n_chunks)) return false;

 // validate sizes
 for(uint32 i = 0; i < n_chunks; i++) {
     if(!sizes[i]) return false; // can't have zero size
     if(!(sizes[i] < filesize)) return false; // must be less than filesize
     if(sizes[i] > 0xFFFF) return false; // chunks are 0xFFFF bytes or less
    }

 // validate overall chunksize
 uint32 total_chunksize = 0;
 for(uint32 i = 0; i < n_chunks; i++) total_chunksize += sizes[i];
 if(!(total_chunksize < filesize)) return false; // must be less than filesize

 // save current position
 uint32 position = (uint32)ifile.tellg();
 if(ifile.fail()) return false;

 // move to nearest 0x80-byte alignment
 position = ((position + 0x7F) & ~(0x7F));
 ifile.seekg(position);
 if(ifile.fail()) return false;

 // is ZLIB?
 if(!isZLIB(ifile, sizes[0], -15)) return false;
 return true;
}

bool DWIsIdxZrc_TypeB(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // compute filesize for IDX file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filesize must be greater than 0x80
 // this is because the first ZLIB chunk is 0x80-byte aligned
 if(filesize < SMALL_FILESIZE) return false;

 // read unpacked size
 uint32 unpacked_size = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // unpacked size must not be small
 if(unpacked_size < SMALL_FILESIZE)
    return false;

 // loop ZLIB sections
 uint32 sections = 0;
 for(;;)
    {
     // read compressed data size
     uint32 datasize = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // stop when datasize is zero
     if(!datasize) break;

     // datasize must be less than the filesize
     if(!(datasize < filesize)) return false;

     // read next two bytes
     uint08 b1 = BE_read_uint08(ifile);
     uint08 b2 = BE_read_uint08(ifile);
     if(ifile.fail()) return false;

     // is this ZLIB?
     if(b1 != 0x78) return false;
     if(b2 != 0xDA) return false;

     // move to next section
     ifile.seekg(datasize - 2, ios::cur);
     if(ifile.fail()) return error("Seek failure.");

     // increment number of sections
     sections++;
    }

 // OK if number of sections is valid
 return (sections > 0);
}

bool DWExclude(uint32 magic)
{
 // exclude these files
 if(EXCLUDE_FILES) {
    if((magic >= 0x39470000) && (magic <= 0x3947FFFF)) return true; // 9G
    if((magic >= 0x4C580013) && (magic <= 0x4C580014)) return true; // LX__
    if(magic == 0x32444F54) return true; // 2DOT
    if(magic == 0x414C5343) return true; // ALSC
    if(magic == 0x41545350) return true; // ATSP
    if(magic == 0x434F4C4B) return true; // COLK
    if(magic == 0x46455841) return true; // FEXA
    if(magic == 0x4731434F) return true; // G1CO
    if(magic == 0x4731454D) return true; // G1EM
    if(magic == 0x47314658) return true; // G1FX
    if(magic == 0x47314C54) return true; // G1L_
    if(magic == 0x47315046) return true; // G1PF
    if(magic == 0x47315653) return true; // G1VS
    if(magic == 0x47524153) return true; // GRAS
    if(magic == 0x494E464F) return true; // INFO
    if(magic == 0x4B4F435F) return true; // KOC_
    if(magic == 0x4B50535F) return true; // KPS_
    if(magic == 0x4B53484C) return true; // KSHL
    if(magic == 0x4B544643) return true; // KTFC
    if(magic == 0x4B54464B) return true; // KTFK
    if(magic == 0x4D455353) return true; // MESS
    if(magic == 0x4F324F42) return true; // O2OB
    if(magic == 0x4F424A44) return true; // OBJD
    if(magic == 0x4F535447) return true; // OSTG
    if(magic == 0x52494742) return true; // RIGB
    if(magic == 0x534C4F5F) return true; // SLO_
    if(magic == 0x53545F4D) return true; // ST_M
    if(magic == 0x5356445F) return true; // SVD_
    if(magic == 0x5742445F) return true; // WBD_
    if(magic == 0x5742485F) return true; // WBH_
    if(magic == 0x58595748) return true; // XYWH
    if(magic == 0x5A535345) return true; // ZSSE
   }

 // otherwise OK to save out
 return false;
}

bool DWGetFileExtension(LPCTSTR filename, LPTSTR extension)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // read first four bytes
 uint32 head01 = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // 2DOT
 if(head01 == 0x32444F54) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("2dot"));
    return true;
   }
 // 9G
 else if((head01 >= 0x39470000) && (head01 <= 0x3947FFFF)) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("9G"));
    return true;
   }
 // ALSC
 // Trinity Souls of Zill Oll (2010)
 else if(head01 == 0x414C5343) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("alsc"));
    return true;
   }
 // ATSP
 else if(head01 == 0x41545350) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("atsp"));
    return true;
   }
 // COLK
 else if(head01 == 0x434F4C4B) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("colk"));
    return true;
   }
 // FEXA
 else if(head01 == 0x46455841) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("fexa"));
    return true;
   }
 // G1A_
 else if(head01 == 0x4731415F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g1a"));
    return true;
   }
 // G1CO
 else if(head01 == 0x4731434F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g1co"));
    return true;
   }
 // G1CT (TEXTURES)
 else if(head01 == 0x47314354) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g1ct"));
    return true;
   }
 // G1EM
 else if(head01 == 0x4731454D) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g1e"));
    return true;
   }
 // G1FX
 else if(head01 == 0x47314658) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g1fx"));
    return true;
   }
 // G1L_
 else if(head01 == 0x47314C54) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g1l"));
    return true;
   }
 // G1M_
 else if(head01 == 0x47314D5F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g1m"));
    return true;
   }
 // G1PF
 else if(head01 == 0x47315046) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("gpf"));
    return true;
   }
 // G1TG
 else if(head01 == 0x47315447) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g1t"));
    return true;
   }
 // G1VS
 else if(head01 == 0x47315653) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g1vs"));
    return true;
   }
 // G2A_
 else if(head01 == 0x4732415F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("g2a"));
    return true;
   }
 // GIMD
 else if(head01 == 0x47494D44) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("gimd"));
    return true;
   }
 // GRAS
 else if(head01 == 0x47524153) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("gras"));
    return true;
   }
 // INFO
 else if(head01 == 0x494E464F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("info"));
    return true;
   }
 // KOC_
 else if(head01 == 0x4B4F435F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("koc"));
    return true;
   }
 // KPS_
 else if(head01 == 0x4B50535F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("kps"));
    return true;
   }
 // KRD_
 else if(head01 == 0x4B52445F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("krd"));
    return true;
   }
 // KSHL
 else if(head01 == 0x4B53484C) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("kshl"));
    return true;
   }
 // KTFC
 else if(head01 == 0x4B544643) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("ktfc"));
    return true;
   }
 // KTFK
 else if(head01 == 0x4B54464B) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("ktfk"));
    return true;
   }
 // LX__
 else if((head01 >= 0x4C580013) && (head01 <= 0x4C580014)) {
    StringCchCopy(extension, 8, TEXT("lx"));
    return true;
   }
 // MDLK
 else if(head01 == 0x4D444C4B) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("mdlk"));
    return true;
   }
 // MESS
 else if(head01 == 0x4D455353) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("mess"));
    return true;
   }
 // O2OB
 else if(head01 == 0x4F324F42) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("o2ob"));
    return true;
   }
 // OBJD
 else if(head01 == 0x4F424A44) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("objd"));
    return true;
   }
 // OSTG
 else if(head01 == 0x4F535447) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("ostg"));
    return true;
   }
 // PNG
 else if(head01 == 0x89504E47) {
    StringCchCopy(extension, 8, TEXT("png"));
    return true;
   }
 // RIFF
 else if(head01 == 0x52494646) {
    StringCchCopy(extension, 8, TEXT("riff"));
    return true;
   }
 // RIGB
 else if(head01 == 0x52494742) {
    StringCchCopy(extension, 8, TEXT("rigb"));
    return true;
   }
 // SLO_
 else if(head01 == 0x534C4F5F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("slo"));
    return true;
   }
 // ST_M
 else if(head01 == 0x53545F4D) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("stm"));
    return true;
   }
 // SVD_
 // Trinity Souls of Zill Oll (2010)
 else if(head01 == 0x5356445F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("svd"));
    return true;
   }
 // WBD_
 else if(head01 == 0x5742445F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("wbd"));
    return true;
   }
 // WBH_
 else if(head01 == 0x5742485F) {
    uint32 head02 = BE_read_uint32(ifile);
    if(ifile.fail()) return false;
    StringCchCopy(extension, 8, TEXT("wbh"));
    return true;
   }
 // XYWH
 else if(head01 == 0x58595748) {
    StringCchCopy(extension, 8, TEXT("xywh"));
    return true;
   }
 // ZFLR
 // Trinity Souls of Zill Oll (2010)
 else if(head01 == 0x5A464C52) {
    StringCchCopy(extension, 8, TEXT("zflr"));
    return true;
   }
 // ZOBJ
 // Trinity Souls of Zill Oll (2010)
 else if(head01 == 0x5A4F424A) {
    StringCchCopy(extension, 8, TEXT("zobj"));
    return true;
   }
 // ZSSE
 // Trinity Souls of Zill Oll (2010)
 else if(head01 == 0x5A535345) {
    StringCchCopy(extension, 8, TEXT("zsse"));
    return true;
   }
 // BIN
 else if(DWIsBinTypeA(filename)) {
    StringCchCopy(extension, 8, TEXT("bin"));
    return true;
   }
 // G1FXLIST
 else if(DWIsGfx_ListA(filename)) {
    StringCchCopy(extension, 8, TEXT("gfxl"));
    return true;
   }
 // ARC1
 else if(DWIsArchiveType1(filename)) {
    StringCchCopy(extension, 8, TEXT("arc1"));
    return true;
   }
 // ARC2
 else if(DWIsArchiveType2(filename)) {
    StringCchCopy(extension, 8, TEXT("arc2"));
    return true;
   }
 // ARC3
 else if(DWIsArchiveType3(filename)) {
    StringCchCopy(extension, 8, TEXT("arc3"));
    return true;
   }
 // ARC4
 else if(DWIsArchiveType4(filename)) {
    StringCchCopy(extension, 8, TEXT("arc4"));
    return true;
   }
 // ARC5
 else if(DWIsArchiveType5(filename)) {
    StringCchCopy(extension, 8, TEXT("arc5"));
    return true;
   }
 // ARC6
 else if(DWIsArchiveType6(filename)) {
    StringCchCopy(extension, 8, TEXT("arc6"));
    return true;
   }
 // IDXZRC
 else if(DWIsIdxZrc(filename)) {
    StringCchCopy(extension, 8, TEXT("idxzrc"));
    return true;
   }
 // IDXZRC TYPE B
 else if(DWIsIdxZrc_TypeB(filename)) {
    StringCchCopy(extension, 8, TEXT("idxzrc"));
    return true;
   }
 // UNKNOWN
 else {
    StringCchCopy(extension, 8, TEXT("unknown"));
    return true;
   }

 // failed
 return false;
}

bool DWRenameIDXOUT(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // delete file given first four bytes
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail() || DWExclude(magic)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // close file
 ifile.close();

 // filesize is zero
 if(!filesize) {
    // delete
    if(DELETE_SMALL_FILES) DeleteFile(filename);
    // rename
    else {
       STDSTRING savename = ChangeFileExtension(filename, TEXT("unknown")).get();
       MoveFileEx(filename, savename.c_str(), MOVEFILE_REPLACE_EXISTING);
      }
    return true;
   }

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    if(!DeleteFile(filename)) return error("Failed to delete IDXOUT file.");
    return true;
   }

 // rename file based on extension
 TCHAR extension[32];
 if(DWGetFileExtension(filename, extension)) {
    auto newname = ChangeFileExtension(filename, extension);
    if(!MoveFileEx(filename, newname.get(), MOVEFILE_REPLACE_EXISTING)) {
       stringstream ss;
       ss << "Failed to rename IDXOUT file. GetLastError() code is 0x" << hex << GetLastError() << dec << ".";
       return error(ss);
      }
   }

 // success
 return true;
}

bool DWDecompressIDXZRC_LE(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // read magic (0x10000, 0x18000, 0x20000)
 uint32 magic = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of files (it is possible to have 0 files)
 uint32 n_files = LE_read_uint32(ifile);
 if(n_files == 0) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // read uncompressed file size
 LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read chunk offsets
 boost::shared_array<uint32> sizes(new uint32[n_files]);
 LE_read_array(ifile, sizes.get(), n_files);

 // create output filename
 STDSTRING shrtname;
 shrtname += GetPathnameFromFilename(filename).get();
 shrtname += GetShortFilenameWithoutExtension(filename).get();

 // create output file
 STDSTRINGSTREAM ss;
 ss << shrtname << TEXT(".idxout");
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // for each chunk
 for(uint32 i = 0; i < n_files; i++)
    {
     // record position
     uint32 position = (uint32)ifile.tellg();
     if(ifile.fail()) return error("Tell failure.");

     // seek to nearest 0x80-byte alignment
     position = ((position + 0x7F) & ~(0x7F));
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read size of data
     uint32 n = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // compressed
     if((sizes[i] > 4) && (n == sizes[i] - 4)) {
        bool status = DecompressZLIB(ifile, n, ofile, 0);
        if(!status) return false;
       }
     // uncompressed
     else {
        ifile.seekg(position);
        boost::shared_array<char> temp(new char[sizes[i]]);
        ifile.read(temp.get(), sizes[i]);
        if(ifile.fail()) return error("Read failure.");
        ofile.write(temp.get(), sizes[i]);
        if(ofile.fail()) return error("Write failure.");
       }
    }

 // cleanup ZRC file
 ifile.close();
 DeleteFile(filename);

 return true;
}

bool DWDecompressIDXZRC(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // read magic (0x10000 or 0x18000 or 0xFFFFFFFF)
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read number of files (it is possible to have 0 files)
 uint32 n_files = BE_read_uint32(ifile);
 if(n_files == 0) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // [PS3] Warriors Orochi 3 Ultimate also uses LE archives
 // is this a little endian archive?
 if((magic == 0x0200) && (n_files > 0xFFFF)) {
    ifile.close();
    return DWDecompressIDXZRC_LE(filename);
   }

 // read uncompressed file size
 BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read chunk sizes
 boost::shared_array<uint32> sizes(new uint32[n_files]);
 BE_read_array(ifile, sizes.get(), n_files);

 // create output filename
 STDSTRING shrtname;
 shrtname += GetPathnameFromFilename(filename).get();
 shrtname += GetShortFilenameWithoutExtension(filename).get();

 // create output file
 STDSTRINGSTREAM ss;
 ss << shrtname << TEXT(".idxout");
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // for each chunk
 for(uint32 i = 0; i < n_files; i++)
    {
     // record position
     uint32 position = (uint32)ifile.tellg();
     if(ifile.fail()) return error("Tell failure.");

     // seek to nearest 0x80-byte alignment
     position = ((position + 0x7F) & ~(0x7F));
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // magic == 0xFFFFFFFF
     // contents are nothing but zlib chunks
     if(magic == 0xFFFFFFFF)
       {
        // read data
        boost::shared_array<char> temp(new char[sizes[i]]);
        ifile.read(temp.get(), sizes[i]);
        if(ifile.fail()) return error("Read failure.");

        // save data
        bool status = InflateZLIB(temp.get(), sizes[i], ofile, -15);
        if(!status) return false;
       }
     else
       {
        // read size of data
        uint32 n = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");

        // compressed
        if((sizes[i] > 4) && (n == sizes[i] - 4)) {
           bool status = DecompressZLIB(ifile, n, ofile, 0);
           if(!status) return false;
          }
        // uncompressed
        else {
           ifile.seekg(position);
           boost::shared_array<char> temp(new char[sizes[i]]);
           ifile.read(temp.get(), sizes[i]);
           if(ifile.fail()) return error("Read failure.");
           ofile.write(temp.get(), sizes[i]);
           if(ofile.fail()) return error("Write failure.");
          }
       }
    }

 // cleanup IDXZRC file
 ifile.close();
 DeleteFile(filename);
 return true;
}

bool DWUnpackArc1(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #1
 // #FILES, [OFFSET/LENGTH], [DATA]
 //

 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offset/length table
 struct ARC1ITEM { uint32 offset; uint32 length; };
 typedef pair<uint32, ARC1ITEM> pair_t;
 deque<pair_t> entries;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read pair
     uint32 offset = BE_read_uint32(ifile);
     uint32 length = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // ignore entry
     if(offset == 0 && length == 0)
        continue;

     // ignore entry
     if(offset != 0 && length == 0)
        continue;

     // invalid entry
     if(offset == 0 && length != 0)
        return error("Invalid ARC1 entry.");

     // insert entry
     ARC1ITEM item;
     item.offset = offset;
     item.length = length;
     entries.push_back(pair_t(i, item));
    }

 // don't even bother saving anything
 if(DELETE_SMALL_FILES) {
    // compute number of small files
    uint32 smallcount = 0;
    for(uint32 i = 0; i < entries.size(); i++) {
        uint32 length = entries[i].second.length;
        if(length < SMALL_FILESIZE) smallcount++;
       }
    // delete file and move on
    if(smallcount == entries.size()) {
       ifile.close();
       DeleteFile(filename);
       return true;
      }
   }

 // create save directory
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // save items
 for(uint32 i = 0; i < entries.size(); i++)
    {
     // skip small files
     uint32 offset = entries[i].second.offset;
     uint32 length = entries[i].second.length;
     if(DELETE_SMALL_FILES && (length < SMALL_FILESIZE))
        continue;

     // move to offset
     ifile.seekg(offset);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[length]);
     ifile.read(data.get(), length);
     if(ifile.fail()) return error("Read failure.");

     // skip saving
     binary_stream bs(data, length);
     if(DWExclude(bs.BE_read_uint32())) continue;

     // save data
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(3) << entries[i].first << TEXT(".idxout");
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create IDXOUT file.");
     ofile.write(data.get(), length);
     if(ofile.fail()) return error("Write failure.");
    }

 // cleanup ARC file
 ifile.close();
 DeleteFile(filename);

 return true;
}

bool DWUnpackArc2(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #2
 // [OFFSET/LENGTH], [DATA]
 //

 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read 1st pair
 uint32 curr_offset = BE_read_uint32(ifile);
 uint32 curr_length = BE_read_uint32(ifile);
 uint32 curr_finish = curr_offset + curr_length;
 if(ifile.fail()) return error("Read failure.");

 // validate 1st pair
 if(filesize < curr_offset) return error("Invalid offset.");
 if(filesize < curr_length) return error("Invalid length.");
 if(filesize < curr_finish) return error("Invalid offset-length pair.");

 // insert 1st pair
 struct ARC2ITEM { uint32 offset; uint32 length; };
 ARC2ITEM item = { curr_offset, curr_length };
 deque<ARC2ITEM> entries;
 entries.push_back(item);

 // keep loading pairs
 for(;;)
    {
     // load previous pair
     uint32 prev_offset = entries[entries.size() - 1].offset;
     uint32 prev_length = entries[entries.size() - 1].length;
     uint32 prev_finish = prev_offset + prev_length;

     // load current pair
     uint32 curr_offset = BE_read_uint32(ifile);
     uint32 curr_length = BE_read_uint32(ifile);
     uint32 curr_finish = curr_offset + curr_length;
     if(ifile.fail()) break;

     // validate curr pair
     if(filesize < curr_offset) break;
     if(filesize < curr_length) break;
     if(filesize < curr_finish) break;

     // offset and length are non-zero
     if(curr_offset != 0 && curr_length != 0)
       {
        // validate order between pairs
        if(curr_offset < prev_offset) break;
        if(curr_offset < prev_finish) break;

        // can't have too must spacing between data
        uint32 spacing = curr_offset - prev_finish;
        if(spacing > 0x0F) break;

        // insert
        ARC2ITEM item = { curr_offset, curr_length };
        entries.push_back(item);
       }
     // offset and length are zero
     else if(curr_offset == 0 && curr_length == 0)
       {
        // continue testing
        continue;
       }
     // only length is zero
     else if(curr_length == 0)
       {
        // curr offset must still be valid
        if(curr_offset < prev_offset) break;
        if(curr_offset < prev_finish) break;

        // continue testing
        continue;
       }
     // only offset is zero
     else
        break;
    }

 // validate number of items
 uint32 items = (uint32)entries.size();
 if((items < 1) || (items > 0xFFFF)) return false;

 // create save directory
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname << TEXT("\\");
 STDSTRING savepath = ss.str();
 CreateDirectory(savepath.c_str(), NULL);

 // save items
 for(uint32 i = 0; i < entries.size(); i++)
    {
     // skip
     if(entries[i].offset == 0x00) continue;
     if(entries[i].length == 0x00) continue;
     if(entries[i].length <= 0xFF) continue; // skip small files

     // move to offset
     ifile.seekg(entries[i].offset);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[entries[i].length]);
     ifile.read(data.get(), entries[i].length);
     if(ifile.fail()) return error("Read failure.");

     // skip saving
     binary_stream bs(data, entries[i].length);
     if(DWExclude(bs.BE_read_uint32())) continue;

     // save data
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(3) << i << TEXT(".idxout");
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create file.");
     ofile.write(data.get(), entries[i].length);
     if(ofile.fail()) return error("Write failure.");
    }

 // cleanup ARC file
 ifile.close();
 DeleteFile(filename);

 return true;
}

bool DWUnpackArc3(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #3
 // #FILES, [LENGTH], [DATA]
 // DATA IS ALWAYS 0x10-BYTE ALIGNED
 //

 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read length table
 deque<uint32> entries;
 for(uint32 i = 0; i < n_files; i++) {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return false;
     entries.push_back(item);
    }

 // don't even bother creating a folder or extracting anything
 if(DELETE_SMALL_FILES) {
    uint32 smallcount = 0;
    for(uint32 i = 0; i < n_files; i++) if(entries[i] < SMALL_FILESIZE) smallcount++;
    if(smallcount == n_files) {
       ifile.close();
       DeleteFile(filename);
       return true;
      }
   }

 // create save directory
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname << "\\";
 STDSTRING savepath = ss.str();
 CreateDirectory(savepath.c_str(), NULL);

 // save items
 for(uint32 i = 0; i < entries.size(); i++)
    {
     // obtain current position
     uint64 position = (uint64)ifile.tellg();
     if(ifile.fail()) return error("Tell failure.");

     // move to next 0x10-byte position
     ifile.seekg(((position + 0x0F) & (~0x0F)));
     if(ifile.fail()) return error("Seek failure.");

     // skip empty files
     if(entries[i] == 0x00) continue;

     // skip small files
     if(DELETE_SMALL_FILES && (entries[i] < SMALL_FILESIZE)) {
        ifile.seekg(entries[i], ios::cur);
        if(ifile.fail()) return error("Seek failure.");
        continue;
       }

     // read data
     boost::shared_array<char> data(new char[entries[i]]);
     ifile.read(data.get(), entries[i]);
     if(ifile.fail()) return error("Read failure.");

     // skip saving
     binary_stream bs(data, entries[i]);
     if(DWExclude(bs.BE_read_uint32())) continue;

     // save data
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(3) << i << TEXT(".idxout");
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create file.");
     ofile.write(data.get(), entries[i]);
     if(ofile.fail()) return error("Write failure.");
    }

 // cleanup ARC file
 ifile.close();
 DeleteFile(filename);

 return true;
}

bool DWUnpackArc4(LPCTSTR filename)
{
 //
 // ARCHIVE TYPE #4
 // #FILES, [OFFSET], [DATA]
 //

 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offsets table
 typedef pair<uint32, uint32> pair_t;
 deque<pair_t> offsets;
 for(uint32 i = 0; i < n_files; i++) {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return false;
     if(item) offsets.push_back(pair_t(i, item));
    }

 // nothing to save
 if(!offsets.size()) return true;

 // don't even bother creating a folder or extracting anything
 if(DELETE_SMALL_FILES)
   {
    // compute number of small files
    uint32 smallcount = 0;
    for(uint32 i = 0; i < offsets.size(); i++) {
        uint32 prev_offset = offsets[i].second;
        uint32 curr_offset = ((i == (offsets.size() - 1)) ? filesize : offsets[i + 1].second);
        uint32 datasize = curr_offset - prev_offset;
        if(datasize < SMALL_FILESIZE) smallcount++;
       }
    // test for all small files
    if(smallcount == offsets.size()) {
       ifile.close();
       DeleteFile(filename);
       return true;
      }
   }

 // create save directory
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname << "\\";
 STDSTRING savepath = ss.str();
 CreateDirectory(savepath.c_str(), NULL);

 // save items
 uint32 last_index = offsets.size() - 1;
 for(uint32 i = 0; i < offsets.size(); i++)
    {
     // move to offset
     ifile.seekg(offsets[i].second);
     if(ifile.fail()) return error("Seek failure.");

     // compute size of data
     uint32 size = 0;
     if(i == last_index) size = filesize - offsets[i].second;
     else size = offsets[i + 1].second - offsets[i].second;

     // skip small files
     if(DELETE_SMALL_FILES && (size < SMALL_FILESIZE))
        continue;

     // read data
     boost::shared_array<char> data(new char[size]);
     ifile.read(data.get(), size);
     if(ifile.fail()) return error("Read failure.");

     // skip saving
     binary_stream bs(data, size);
     if(DWExclude(bs.BE_read_uint32())) continue;

     // save data
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(3) << i << TEXT(".idxout");
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create file.");
     ofile.write(data.get(), size);
     if(ofile.fail()) return error("Write failure.");
    }

 // cleanup ARC file
 ifile.close();
 DeleteFile(filename);

 return true;
}

bool DWUnpackArc5(LPCTSTR filename)
{
 // same format just not aligned
 return DWUnpackArc4(filename);
}

bool DWUnpackArc6(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 headMagic = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headFiles = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headScale = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headUnk01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");

 // validate header
 if(headMagic != 0x1EDE) return error("Invalid header signature.");
 if(headFiles >= 0xFFFF) return error("No dynasty warriors game contains this many files.");
 if(headScale % 0x10) return error("File assets should occur on 0x10-byte boundaries.");

 // read file table
 deque<pair<uint32, uint32>> filetable;
 for(size_t i = 0; i < headFiles; i++) {
     pair<uint32, uint32> p;
     p.first  = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     p.second = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
     filetable.push_back(p);
    }

 // create save directory
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname << "\\";
 STDSTRING savepath = ss.str();
 CreateDirectory(savepath.c_str(), NULL);

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     size_t position = filetable[i].first*headScale;
     size_t filesize = filetable[i].second;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // ignore small files
     if(filesize < 0x80) continue;

     // create BIN filename
     cout << "Reading file " << (i + 1) << " of " << filetable.size() << "." << endl;
     STDSTRINGSTREAM name;
     name << savepath << setfill(TCHAR('0')) << setw(4) << i << TEXT(".idxout");

     // create BIN file
     ofstream ofile(name.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create bin file.");

     // small files
     if(filesize < 0x2000000) // 32 MB
       {
        // read data
        boost::shared_array<char> data(new char[filesize]);
        ifile.read(data.get(), filesize);
        if(ifile.fail()) return error("Read failure.");
        
        // save data
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to bin file.");
       }
     // large files
     else
       {
        // create buffer
        cout << " NOTE: This is a large file and might take a while to save." << endl;
        uint32 buffersize = 0x1000000; // 16 MB
        boost::shared_array<char> data(new char[buffersize]);

        // read and copy data
        uint32 bytes_left = filesize;
        while(bytes_left)
             {
              // specify how much to read
              uint32 datasize = buffersize;
              if(bytes_left < buffersize) datasize = bytes_left;

              // read data
              ifile.read(data.get(), datasize);
              if(ifile.fail()) return error("Read failure.");
              
              // save data
              ofile.write(data.get(), datasize);
              if(ofile.fail()) return error("Failed to write to bin file.");

              // update
              bytes_left -= datasize;
             }
       }
    }

 // cleanup ARC file
 ifile.close();
 DeleteFile(filename);

 return true;
}

bool DWUnpackMDLK(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = BE_read_uint32(ifile);
 uint32 h02 = BE_read_uint32(ifile);
 uint32 h03 = BE_read_uint32(ifile); // number of files
 uint32 h04 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // validate header
 if(h01 != 0x4D444C4B) return error("Not an MDLK file.");
 if(h03 == 0x00) return true;
 switch(h02) {
   case(0x30303031) : break;
   case(0x30363030) : break;
   default : return error("Incorrect MDLK version.");
  }
 switch(h04) {
   case(0x48490000) : break; // HI
   case(0x4D490000) : break; // MI
   case(0x50414444) : break; // PADD
   // don't be so strict here since some games use different strings
   // default : return error("Invalid MDLK file.");
  }

 // create save directory
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname << TEXT("_MDLK") << TEXT("\\");
 STDSTRING savepath = ss.str();
 CreateDirectory(savepath.c_str(), NULL);

 // save items
 for(uint32 i = 0; i < h03; i++)
    {
     // save position
     uint64 position = (uint64)ifile.tellg();
     if(ifile.fail()) return error("Tell failure.");

     // read chunk ID
     uint32 p01 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // variables
     uint32 datasize = 0;
     STDSTRING extension = TEXT(".unknown");

     // GIM_
     if(p01 == 0x47314D5F) 
       {
        // seek data size
        ifile.seekg(4, ios::cur);
        if(ifile.fail()) return error("Seek failure.");

        // read data size
        extension = TEXT("g1m");
        datasize = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");
       }
     // G1CO
     else if(p01 == 0x4731434F)
       {
        // seek data size
        ifile.seekg(8, ios::cur);
        if(ifile.fail()) return error("Seek failure.");

        // read data size
        extension = TEXT("g1co");
        datasize = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");
       }
     // unknown
     else {
        stringstream ss;
        ss << "Unknown file type at offset 0x" << hex << position << dec << " in MDLK archive." << endl;
        return error(ss);
       }

     // validate data size
     if(!datasize) return error("Invalid data size.");
     if(!extension.length()) return error("Invalid filename extension.");

     // move back to position
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);
     if(ifile.fail()) return error("Read failure.");

     // save data
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(3) << i << TEXT(".") << extension;
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create file.");
     ofile.write(data.get(), datasize);
     if(ofile.fail()) return error("Write failure.");
    }

 // cleanup MDLK file
 ifile.close();
 DeleteFile(filename);

 return true;
}

//
// IDXZRC TYPE A
// uint32 number of files
// uint32 uncompressed size
// shared_array<uint32> chunk offsets
// shared_array<char> data
//
bool DWUnpackIDXZRC_TypeA(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // read number of files (it is possible to have 0 files)
 uint32 n_files = BE_read_uint32(ifile);
 if(n_files == 0) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // read uncompressed file size
 uint32 uncompressed_size = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read chunk offsets
 boost::shared_array<uint32> sizes(new uint32[n_files]);
 if(!BE_read_array(ifile, sizes.get(), n_files)) return error("Read failure.");

 // create output filename
 STDSTRING shrtname;
 shrtname += GetPathnameFromFilename(filename).get();
 shrtname += GetShortFilenameWithoutExtension(filename).get();

 // create output file
 STDSTRINGSTREAM ss;
 ss << shrtname << TEXT(".idxout");
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // for each chunk
 for(uint32 i = 0; i < n_files; i++)
    {
     // record position
     uint32 position = (uint32)ifile.tellg();
     if(ifile.fail()) return error("Tell failure.");

     // seek to nearest 0x80-byte alignment
     position = ((position + 0x7F) & ~(0x7F));
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // compressed (very important to use -15)
     if(sizes[i] > 4) {
        bool status = DecompressZLIB(ifile, sizes[i], ofile, -15);
        if(!status) return error("ZLIB failure.");
       }
     // uncompressed
     else {
        ifile.seekg(position);
        boost::shared_array<char> temp(new char[sizes[i]]);
        ifile.read(temp.get(), sizes[i]);
        if(ifile.fail()) return error("Read failure.");
        ofile.write(temp.get(), sizes[i]);
        if(ofile.fail()) return error("Write failure.");
       }
    }

 // cleanup ZRC file
 ifile.close();
 DeleteFile(filename);

 return true;
}

//
// IDXZRC TYPE B
// uint32 total inflated size
// do {
//  uint32 deflated size
//  shared_array<char> deflated data
// } while(deflated size > 0)
//
bool DWUnpackIDXZRC_TypeB(LPCTSTR filename)
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

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // total unpacked size
 uint32 unpacked_size = BE_read_uint32(ifile);
 if(!unpacked_size) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // create output file
 STDSTRING ofname;
 ofname += pathname;
 ofname += shrtname;
 ofname += TEXT(".idxout");
 ofstream ofile(ofname.c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // loop forever
 for(;;)
    {
     // read datasize
     uint32 datasize = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // validate datasize
     if(!datasize) break;

     // decompress and save
     // use 0 instead of -15 since these zlib chunks start with 0x78DA
     bool status = DecompressZLIB(ifile, datasize, ofile, 0);
     if(!status) return error("ZLIB failure.");
    }

 // cleanup ZRC file
 ifile.close();
 DeleteFile(filename);
 return true;
}

bool DWUnpackLoopIDXZRC_TypeA(LPCTSTR pathname)
{
 cout << "Decompressing .IDXZRC files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".idxzrc"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!DWUnpackIDXZRC_TypeA(filelist[i].c_str())) return false;
    }
 cout << endl;
 return true;
}

bool DWUnpackLoopIDXZRC_TypeB(LPCTSTR pathname)
{
 cout << "Decompressing .IDXZRC files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".idxzrc"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!DWUnpackIDXZRC_TypeB(filelist[i].c_str())) return false;
    }
 cout << endl;
 return true;
}

};};

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool UnpackANS(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

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

 // create output directory
 STDSTRING savepath = pathname;
 savepath += STDSTRING(shrtname);
 savepath += TEXT("_");
 savepath += &(GetExtensionFromFilename(filename)[1]);
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     size_t position = filetable[i].first*headScale;
     size_t filesize = filetable[i].second;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // ignore small files
     if(filesize < 0x80) continue;

     // create BIN filename
     cout << "Reading file " << (i + 1) << " of " << filetable.size() << "." << endl;
     STDSTRINGSTREAM name;
     name << savepath << setfill(TCHAR('0')) << setw(4) << i << TEXT(".idxout");

     // create BIN file
     ofstream ofile(name.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create bin file.");

     // small files
     if(filesize < 0x2000000) // 32 MB
       {
        // read data
        boost::shared_array<char> data(new char[filesize]);
        ifile.read(data.get(), filesize);
        if(ifile.fail()) return error("Read failure.");
        
        // save data
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to bin file.");
       }
     // large files
     else
       {
        // create buffer
        cout << " NOTE: This is a large file and might take a while to save." << endl;
        uint32 buffersize = 0x1000000; // 16 MB
        boost::shared_array<char> data(new char[buffersize]);

        // read and copy data
        uint32 bytes_left = filesize;
        while(bytes_left)
             {
              // specify how much to read
              uint32 datasize = buffersize;
              if(bytes_left < buffersize) datasize = bytes_left;

              // read data
              ifile.read(data.get(), datasize);
              if(ifile.fail()) return error("Read failure.");
              
              // save data
              ofile.write(data.get(), datasize);
              if(ofile.fail()) return error("Failed to write to bin file.");

              // update
              bytes_left -= datasize;
             }
       }
    }

 // finished
 cout << endl;
 return true;
}

bool UnpackABC(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilename(filename).get();

 // read header
 uint32 headMagic = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headFiles = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headScale = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headUnk01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");

 // validate header
 if(headMagic != 0x077DF9) return error("Invalid header signature.");
 if(headFiles >= 0xFFFF) return error("No dynasty warriors game contains this many files.");
 if(headScale != 0x0800) return error("File assets should occur on 0x800 boundaries.");

 // read file table
 deque<pair<uint32, uint32>> filetable;
 for(size_t i = 0; i < headFiles; i++) {
     uint32 param1 = BE_read_uint32(ifile); // 0x00
     uint32 param2 = BE_read_uint32(ifile); // scaled offset
     uint32 param3 = BE_read_uint32(ifile); // filesize
     uint32 param4 = BE_read_uint32(ifile); // 0x00
     pair<uint32, uint32> p;
     p.first  = param2; if(ifile.fail()) return error("Read failure.");
     p.second = param3; if(ifile.fail()) return error("Read failure.");
     filetable.push_back(p);
    }

 // create save path (swap '.' with '_' from savepath)
 STDSTRING savepath = pathname;
 savepath += STDSTRING(shrtname);
 savepath += TEXT("\\");
 for(size_t i = 0; i < savepath.length(); i++)
     if(savepath[i] == TCHAR('.')) savepath[i] = TCHAR('_');
 if(CreateDirectory(savepath.c_str(), NULL) == 0) {
    DWORD code = GetLastError();
    if(code != ERROR_ALREADY_EXISTS) {
       stringstream ss;
       ss << "ERROR: CreateDirectory() failed with error code 0x";
       ss << setfill('0') << setw(8) << hex << GetLastError() << dec << ".";
       return error(ss.str().c_str());
      }
   }

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     size_t position = filetable[i].first*headScale;
     size_t filesize = filetable[i].second;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // ignore small files
     if(filesize < 0x80) continue;

     // create BIN filename
     cout << "Reading file " << (i + 1) << " of " << filetable.size() << "." << endl;
     STDSTRINGSTREAM name;
     name << savepath << setfill(TCHAR('0')) << setw(4) << i << TEXT(".idxout");

     // create BIN file
     ofstream ofile(name.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create bin file.");

     // small files
     if(filesize < 0x2000000) // 32 MB
       {
        // read data
        boost::shared_array<char> data(new char[filesize]);
        ifile.read(data.get(), filesize);
        if(ifile.fail()) return error("Read failure.");
        
        // save data
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to bin file.");
       }
     // large files
     else
       {
        // create buffer
        cout << " NOTE: This is a large file and might take a while to save." << endl;
        uint32 buffersize = 0x1000000; // 16 MB
        boost::shared_array<char> data(new char[buffersize]);

        // read and copy data
        uint32 bytes_left = filesize;
        while(bytes_left)
             {
              // specify how much to read
              uint32 datasize = buffersize;
              if(bytes_left < buffersize) datasize = bytes_left;

              // read data
              ifile.read(data.get(), datasize);
              if(ifile.fail()) return error("Read failure.");
              
              // save data
              ofile.write(data.get(), datasize);
              if(ofile.fail()) return error("Failed to write to bin file.");

              // update
              bytes_left -= datasize;
             }
       }
    }

 // finished
 cout << endl;
 return true;
}

bool UnpackABC_Type2(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilename(filename).get();

 // read header
 uint32 headMagic = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headFiles = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headScale = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
 uint32 headUnk01 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");

 // validate header
 if(headMagic == 0x00011E54) ; // OK
 else if(headMagic == 0x00005460) ; // OK [PS3] Berserk Musou (2016)
 else return error("Invalid header signature.");
 if(headFiles >= 0xFFFF) return error("No dynasty warriors game contains this many files.");
 if(headScale != 0x0800) return error("File assets should occur on 0x800 boundaries.");

 // read file table
 struct ABCENTRY {
  uint64 offset;
  uint32 size1;
  uint32 size2; // 0 = idxout, !0 = idxzrc
 };
 deque<ABCENTRY> filetable;
 for(size_t i = 0; i < headFiles; i++) {
     ABCENTRY item;
     item.offset = BE_read_uint64(ifile);
     item.size1  = BE_read_uint32(ifile);
     item.size2  = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     filetable.push_back(item);
    }

 // create save path (swap '.' with '_' from savepath)
 STDSTRING savepath = pathname;
 savepath += STDSTRING(shrtname);
 savepath += TEXT("\\");
 for(size_t i = 0; i < savepath.length(); i++) if(savepath[i] == TCHAR('.')) savepath[i] = TCHAR('_');

 // create save path
 if(CreateDirectory(savepath.c_str(), NULL) == 0) {
    DWORD code = GetLastError();
    if(code != ERROR_ALREADY_EXISTS) {
       stringstream ss;
       ss << "ERROR: CreateDirectory() failed with error code 0x";
       ss << setfill('0') << setw(8) << hex << GetLastError() << dec << ".";
       return error(ss.str().c_str());
      }
   }

 // for each item in the file table
 for(size_t i = 0; i < filetable.size(); i++)
    {
     // move to file position
     uint64 position = filetable[i].offset*headScale;
     uint32 filesize = filetable[i].size1;
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // ignore small files
     if(filesize < 0x80) continue;

     // create BIN filename
     cout << "Reading file " << (i + 1) << " of " << filetable.size() << "." << endl;
     STDSTRINGSTREAM name;
     name << savepath << setfill(TCHAR('0')) << setw(4) << i << TEXT(".idxout");

     // create BIN file
     ofstream ofile(name.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create bin file.");

     // small files
     if(filesize < 0x2000000) // 32 MB
       {
        // read data
        boost::shared_array<char> data(new char[filesize]);
        ifile.read(data.get(), filesize);
        if(ifile.fail()) return error("Read failure.");
        
        // save data
        ofile.write(data.get(), filesize);
        if(ofile.fail()) return error("Failed to write to bin file.");
       }
     // large files
     else
       {
        // create buffer
        cout << " NOTE: This is a large file and might take a while to save." << endl;
        uint32 buffersize = 0x1000000; // 16 MB
        boost::shared_array<char> data(new char[buffersize]);

        // read and copy data
        uint32 bytes_left = filesize;
        while(bytes_left)
             {
              // specify how much to read
              uint32 datasize = buffersize;
              if(bytes_left < buffersize) datasize = bytes_left;

              // read data
              ifile.read(data.get(), datasize);
              if(ifile.fail()) return error("Read failure.");
              
              // save data
              ofile.write(data.get(), datasize);
              if(ofile.fail()) return error("Failed to write to bin file.");

              // update
              bytes_left -= datasize;
             }
       }
    }

 // finished
 cout << endl;
 return true;
}

bool UnpackIDX(LPCTSTR filename)
{
 cout << "Unpacking .IDX/.BIN file..." << endl;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open IDX file
 ifstream ifile(filename, ios::binary);
 if(!ifile) {
    stringstream ss;
    ss << "Failed to open IDX file." << endl;
    ss << filename;
    return error(ss);
   }

 // compute filesize for IDX file
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // construct BIN filename
 STDSTRING bfname = pathname;
 bfname += shrtname;
 bfname += TEXT(".BIN");

 // open BIN file
 ifstream bfile(bfname.c_str(), ios::binary);
 if(!bfile) return error("Failed to open BIN file.");

 // create destination directory
 STDSTRINGSTREAM ss;
 ss << pathname << shrtname << TEXT("\\");
 STDSTRING savepath = ss.str();
 CreateDirectory(savepath.c_str(), NULL);

 // read file offsets
 struct IDXITEM {
  uint64 p1; // offset
  uint64 p2; // length (uncompressed?)
  uint64 p3; // length (compressed)
  uint64 p4; // filetype (idxout our idxzrc)
 };
 deque<IDXITEM> itemlist;
 for(;;)
    {
     // read item
     IDXITEM item;
     item.p1 = BE_read_uint64(ifile); if(ifile.fail()) break;
     item.p2 = BE_read_uint64(ifile); if(ifile.fail()) break;
     item.p3 = BE_read_uint64(ifile); if(ifile.fail()) break;
     item.p4 = BE_read_uint64(ifile); if(ifile.fail()) break;

     // validate item
     if((item.p4 == 0) && (item.p2 != item.p3)) {
        stringstream ss;
        ss << "IDX file lengths do not match at offset 0x" << hex << ((uint32)ifile.tellg() - 0x20) << dec << ".";
        return error(ss);
       }

     // insert item only if there is something to save
     if(item.p2 == 0) continue;
     if(item.p3 == 0) continue;
     itemlist.push_back(item);
    }

 for(size_t i = 0; i < itemlist.size(); i++)
    {
     // construct output filename
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(4) << i << TEXT(".");
     if(itemlist[i].p4 == 0) ss << TEXT("idxout");
     else if(itemlist[i].p4 == 1) ss << TEXT("idxzrc");
     else return error("Unknown IDX file type.");

     // display information and ignore small files
     wcout << " FILE " << i << " of " << itemlist.size() << ": " << ss.str() << endl;
     if(!(itemlist[i].p3 > SMALL_FILESIZE)) continue; // changed from 0x80 to 0x100 to save a little time

     // read first four bytes and determine whether or not to save file
     bfile.seekg(itemlist[i].p1);
     if(bfile.fail()) return error("Seek failure.");
     uint32 magic = BE_read_uint32(bfile);
     if(bfile.fail() || DWExclude(magic)) continue;

     // create 16 MB buffer and move to bin data
     boost::shared_array<char> data(new char[0x1000000]);
     bfile.seekg(itemlist[i].p1);
     if(bfile.fail()) return error("Seek failure.");

     // keep track of remaining bytes
     uint64 bytes_remaining = itemlist[i].p3;
     if(!(bytes_remaining < 0x2000000)) // 32 MB
        message("NOTE: This is a large file and might take a while to save.");

     // create output file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // transfer data
     while(bytes_remaining) {
           if(bytes_remaining < 0x1000000) {
              // read BIN data
              bfile.read(data.get(), bytes_remaining);
              if(bfile.fail()) return error("Read failure.");
              // save BIN data
              ofile.write(data.get(), bytes_remaining);
              if(ofile.fail()) return error("Write failure.");
              // nothing more to save
              bytes_remaining = 0;
             }
           else {
              // read BIN data
              bfile.read(data.get(), 0x1000000);
              if(bfile.fail()) return error("Read failure.");
              // save BIN data
              ofile.write(data.get(), 0x1000000);
              if(ofile.fail()) return error("Write failure.");
              // more to save
              bytes_remaining -= 0x1000000;
             }
          }
    }

 cout << endl;
 return true;
}

bool UnpackIDXOUT(LPCTSTR pathname)
{
 cout << "Processing .IDXOUT files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".idxout"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!DWRenameIDXOUT(filelist[i].c_str())) return false;
    }
 cout << endl;
 return true;
}

bool UnpackIDXZRC(LPCTSTR pathname)
{
 cout << "Decompressing .IDXZRC files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".idxzrc"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!DWDecompressIDXZRC(filelist[i].c_str())) return false;
    }
 cout << endl;
 return true;
}

bool IDXArchiveUnpackLoop(LPCTSTR pathname)
{
 uint32 loop_counter = 1;
 for(;;)
    {
     // keep track of how many files were processed
     cout << "ARCHIVE UNPACK LOOP #" << loop_counter << ": " << endl;
     uint32 processed = 0;

     // process ARC1 files
     cout << "Processing .ARC1 files..." << endl;
     deque<STDSTRING> filelist;
     BuildFilenameList(filelist, TEXT(".arc1"), pathname);
     for(size_t i = 0; i < filelist.size(); i++) {
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         if(!DWUnpackArc1(filelist[i].c_str())) return false;
        }
     cout << endl;
     processed += filelist.size();
     
     // process ARC2 files
     cout << "Processing .ARC2 files..." << endl;
     filelist.clear();
     BuildFilenameList(filelist, TEXT(".arc2"), pathname);
     for(size_t i = 0; i < filelist.size(); i++) {
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         if(!DWUnpackArc2(filelist[i].c_str())) return false;
        }
     cout << endl;
     processed += filelist.size();
     
     // process ARC3 files
     cout << "Processing .ARC3 files..." << endl;
     filelist.clear();
     BuildFilenameList(filelist, TEXT(".arc3"), pathname);
     for(size_t i = 0; i < filelist.size(); i++) {
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         if(!DWUnpackArc3(filelist[i].c_str())) return false;
        }
     cout << endl;
     processed += filelist.size();

     // process ARC4 files
     cout << "Processing .ARC4 files..." << endl;
     filelist.clear();
     BuildFilenameList(filelist, TEXT(".arc4"), pathname);
     for(size_t i = 0; i < filelist.size(); i++) {
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         if(!DWUnpackArc4(filelist[i].c_str())) return false;
        }
     cout << endl;
     processed += filelist.size();

     // process ARC5 files
     cout << "Processing .ARC5 files..." << endl;
     filelist.clear();
     BuildFilenameList(filelist, TEXT(".arc5"), pathname);
     for(size_t i = 0; i < filelist.size(); i++) {
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         if(!DWUnpackArc5(filelist[i].c_str())) return false;
        }
     cout << endl;
     processed += filelist.size();

     // process ARC6 files
     cout << "Processing .ARC6 files..." << endl;
     filelist.clear();
     BuildFilenameList(filelist, TEXT(".arc6"), pathname);
     for(size_t i = 0; i < filelist.size(); i++) {
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         if(!DWUnpackArc6(filelist[i].c_str())) return false;
        }
     cout << endl;
     processed += filelist.size();

     // process MDLK files
     cout << "Processing .MDLK files..." << endl;
     filelist.clear();
     BuildFilenameList(filelist, TEXT(".MDLK"), pathname);
     for(size_t i = 0; i < filelist.size(); i++) {
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         if(!DWUnpackMDLK(filelist[i].c_str())) return false;
        }
     cout << endl;
     processed += filelist.size();

     // process IDXOUT files
     cout << "Processing .IDXOUT files..." << endl;
     filelist.clear();
     BuildFilenameList(filelist, TEXT(".idxout"), pathname);
     for(size_t i = 0; i < filelist.size(); i++) {
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         if(!DWRenameIDXOUT(filelist[i].c_str())) return false;
        }
     cout << endl;
     processed += filelist.size();

     // process IDXZRC files
     cout << "Processing .IDXZRC files..." << endl;
     filelist.clear();
     BuildFilenameList(filelist, TEXT(".idxzrc"), pathname);
     for(size_t i = 0; i < filelist.size(); i++) {
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         if(!DWDecompressIDXZRC(filelist[i].c_str())) return false;
        }
     cout << endl;
     processed += filelist.size();

     // finished or not finished
     if(processed == 0) break;
     cout << "Processed a total of " << processed << " files in unpack loop #" << loop_counter << "." << endl;
     loop_counter++;
    }

 return true;
}

};};

//
// DLC ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

//
// WIP
//
bool UnpackDLC_TYPE_01(LPCTSTR filename)
{
 // open file
 using namespace std;
 ifstream ifile;
 ifile.open(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 if(ifile.fail()) return error("Seek failure.");
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // filesize can be small
 if(DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create folder
 STDSTRINGSTREAM ss;
 ss << pathname.c_str() << shrtname.c_str() << TEXT("\\");
 CreateDirectory(ss.str().c_str(), NULL);

 // repeat forever
 uint32 archive_index = 0;
 uint32 start = 0x00;
 for(;;)
    {
     // read 0x64
     uint32 magic = BE_read_uint32(ifile);
     if(ifile.eof()) break;
     else if(ifile.fail()) return error("Read failure.");

     // validate
     if(magic != 0x64) {
        std::stringstream ss;
        ss << "Unexpected magic number at offset 0x";
        ss << setfill('0') << hex << (uint64)ifile.tellg() - 4 << dec << ".";
        return error(ss);
       }

     // move to start + 0x34
     ifile.seekg(start + 0x34);
     if(ifile.fail()) return error("Seek failure.");
     
     // read number of files
     uint32 n_files = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     if(n_files > 0xFFFF) return error("Unexpected number of files.");

     // move to offset list
     ifile.seekg(start + 0x40);
     if(ifile.fail()) return error("Seek failure.");
     
     // read offsets
     boost::shared_array<uint32> offsets(new uint32[n_files]);
     if(!BE_read_array(ifile, offsets.get(), n_files)) return error("Read failure.");
     
     // is there any spacing here?
     
     // read sizes
     boost::shared_array<uint32> sizes(new uint32[n_files]);
     if(!BE_read_array(ifile, sizes.get(), n_files)) return error("Read failure.");

     // read and save data
     for(uint32 i = 0; i < n_files; i++)
        {
         // move to offset
         uint32 position = start + 0x30 + offsets[i];
         ifile.seekg(position);
         if(ifile.fail()) return error("Seek failure.");
    
         // read data
         boost::shared_array<char> data(new char[sizes[i]]);
         ifile.read(data.get(), sizes[i]);
         if(ifile.fail()) return error("Read failure.");
    
         // save filename
         STDSTRINGSTREAM ss;
         ss << pathname.c_str() << shrtname.c_str() << TEXT("\\");
         ss << setfill(TEXT('0')) << setw(4) << i;
         ss << TEXT(".");
         ss << TEXT("idxout");
    
         // save data
         ofstream ofile(ss.str().c_str(), ios::binary);
         if(!ofile) return error("Failed to create output file.");
         ofile.write(data.get(), sizes[i]);
         if(ofile.fail()) return error("Write failure.");
        }
    }

 return true;
}

}}

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool DWUnpackTexture(LPCTSTR filename, bool rbo)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open file.");

 // delete boolean
 bool do_delete = true;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = BE_read_uint32(ifile); // magic
 uint32 h02 = BE_read_uint32(ifile); // version
 uint32 h03 = BE_read_uint32(ifile); // section size
 uint32 h04 = BE_read_uint32(ifile); // table offset
 uint32 h05 = BE_read_uint32(ifile); // number of textures
 uint32 h06 = BE_read_uint32(ifile); // unknown (when this is 0x09, textures are funky???)
 uint32 h07 = BE_read_uint32(ifile); // unknown
 if(ifile.fail()) return error("Read failure.");

 // validate
 if(h01 != 0x47315447) return error("Not a G1T file.");
 if(h05 == 0) return true; // nothing to do

 // read flag table
 boost::shared_array<uint32> flagtable(new uint32[h05]);
 if(!BE_read_array(ifile, flagtable.get(), h05)) return error("Read failure.");

 // create save path
 STDSTRING savepath = pathname;
 if(h05 > 1) {
    savepath += shrtname;
    savepath += TEXT("_t"); // to indicate this is a texture folder
    savepath += TEXT("\\");
    CreateDirectory(savepath.c_str(), NULL);
   }

 // move to table
 ifile.seekg(h04);
 if(ifile.fail()) return error("Seek failure.");

 // read offset table
 deque<size_t> offset_list;
 for(size_t i = 0; i < h05; i++) {
     offset_list.push_back(BE_read_uint32(ifile));
     if(ifile.fail()) return error("Read failure.");
    }

 // process textures
 for(size_t i = 0; i < offset_list.size(); i++)
    {
     // move to texture header
     ifile.seekg(h04 + offset_list[i]);
     if(ifile.fail()) return error("Stream seek failure.");

     // read texture information
     // 0x08 bytes
     uint08 b1 = BE_read_uint08(ifile); // unknown
     uint08 b2 = BE_read_uint08(ifile); // texture type
     uint08 b3 = BE_read_uint08(ifile); // dx/dy
     uint08 b4 = BE_read_uint08(ifile); // unknown
     uint16 s1 = BE_read_uint16(ifile); // unknown
     uint16 s2 = BE_read_uint16(ifile); // unknown
     if(ifile.fail()) return error("Read failure.");

     // header is extended depending on s2 by 0x0C bytes
     bool extended = false;
     if(s2 == 0x0000) extended = false;
     else if(s2 == 0x1000) extended = false;
     else if(s2 == 0x1200) extended = false;
     else if(s2 == 0x0001) extended = true;
     else if(s2 == 0x0201) extended = true;
     else if(s2 == 0x1001) extended = true;
     else if(s2 == 0x1101) extended = true;
     else if(s2 == 0x1201) extended = true;
     else if(s2 == 0x1211) extended = true;
     else if(s2 == 0x2201) extended = true;
     else if(s2 == 0x2211) extended = true;
     else if(s2 == 0x2221) extended = true;
     else {
        stringstream ss;
        ss << " Unknown s2 value 0x" << setfill('0') << setw(4) << s2 << dec;
        ss << " at offset 0x" << hex << (h04 + offset_list[i]) << dec << ".";
        ss << " h04 = 0x" << hex << h04 << dec << ".";
        ss << " offset_list[i] = 0x" << hex << offset_list[i] << dec << ".";
        return error(ss);
       }

     if(extended) {
        uint32 v1 = BE_read_uint32(ifile); // 0x0C
        uint32 v2 = BE_read_uint32(ifile); // 0
        uint32 v3 = BE_read_uint32(ifile); // 0x00 or 0x01
        if(ifile.fail()) return error("Read failure.");
       }

     // compute dimensions
     uint32 temp1 = ((b3 & 0xF0) >> 4);
     uint32 temp2 = ((b3 & 0x0F));
     uint32 dx = 1 << temp1;
     uint32 dy = 1 << temp2;

     // create texture filename
     STDSTRINGSTREAM ss;
     ss << savepath;
     if(h05 > 1) ss << setfill(TCHAR('0')) << setw(3) << i << TEXT(".dds"); // name by index
     else ss << shrtname << TEXT(".dds"); // name same as original

     // create texture file
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create file.");

     // TEXTURE TYPE 0x00
     // UNCOMPRESSED ARGB 32-BIT
     // WORKS PERFECT
     if(b2 == 0x00)
       {
        // read texture
        DWORD filesize = 4*dx*dy;
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);
        if(ifile.fail()) return error("Read failure.");

        // PHOTOSHOP ONLY SUPPORTS ONE TYPE OF 32-BIT UNCOMPRESSED!
        // YOU MUST USE ARGB!
        uint32 mask1 = 0xFF000000; // A
        uint32 mask2 = 0x00FF0000; // R 
        uint32 mask3 = 0x0000FF00; // G
        uint32 mask4 = 0x000000FF; // B

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, 0, 0x20, mask2, mask3, mask4, mask1, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // TEXTURE TYPE 0x01
     // UNCOMPRESSED BGRA 32-BIT
     // WORKS PERFECT
     else if(b2 == 0x01)
       {
        // read texture
        DWORD filesize = 4*dx*dy;
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);
        if(ifile.fail()) return error("Read failure.");

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
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
        if(ofile.fail()) return error("Write failure.");
       }
     // TEXTURE TYPE 0x06
     // DXT1
     // WORKS PERFECT
     else if(b2 == 0x06)
       {
        // read texture
        DWORD filesize = DXT1Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // save texture
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, 0, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
       }
     // TEXTURE TYPE 0x07
     // DXT3
     // WORKS PERFECT
     else if(b2 == 0x07)
       {
        // read texture
        DWORD filesize = DXT3Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);
        if(ifile.fail()) return error("Read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // save texture
        DDS_HEADER ddsh;
        CreateDXT3Header(dx, dy, 0, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
        if(ofile.fail()) return error("Write failure.");
       }
     // TEXTURE TYPE 0x08
     // DXT5
     // WORKS PERFECT
     else if(b2 == 0x08)
       {
        // read texture
        DWORD filesize = DXT5Filesize(dx, dy, 0);
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);
        if(ifile.fail()) return error("Read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // save texture
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, 0, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)buffer.get(), filesize);
        if(ofile.fail()) return error("Write failure.");
       }
     // TEXTURE TYPE 0x09
     // UNKNOWN 32-BITS PER PIXEL
     // THIS IS NOT A MORTON OR AN XBOX360 SWIZZLE
     else if(b2 == 0x09)
       {
        do_delete = false;
        stringstream ss;
        ss << " Unsupported texture type 0x" << hex << (uint32)b2 << dec << " at offset 0x" << hex << (uint32)ifile.tellg() << dec << ".";
        ss << " DX/DY = " << dx << "/" << dy;
        message(ss.str().c_str());
       }
     else if(b2 == 0x0A)
       {
        do_delete = false;
        stringstream ss;
        ss << " Unsupported texture type 0x" << hex << (uint32)b2 << dec << " at offset 0x" << hex << (uint32)ifile.tellg() << dec << ".";
        ss << " DX/DY = " << dx << "/" << dy;
        message(ss.str().c_str());
       }
     // TEXTURE TYPE 0x0C
     // HALF-FLOAT
     // XBOX360
     else if(b2 == 0x0C)
       {
        message(" Strange half-float texture format. This is fucked up.");
        do_delete = false;

        // read texture
        DWORD filesize = 8*dx*dy;
        boost::shared_array<char> buffer(new char[filesize]);
        ifile.read((char*)buffer.get(), filesize);
        if(ifile.fail()) return error("Read failure.");

        // reverse byte order
        if(rbo) {
           uint32 n = filesize / 2;
           reverse_byte_order((uint16*)buffer.get(), n);
          }

        // save texture
        DDS_HEADER ddsh;
        DDS_HEADER_DXT10 ddse;
        CreateR16G16B16A16FloatHeader(dx, dy, 0, FALSE, &ddsh, &ddse);

        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)&ddse, sizeof(ddse));
        ofile.write((char*)buffer.get(), filesize);
        if(ofile.fail()) return error("Write failure.");
       }
     // TEXTURE TYPE 0x10
     // DXT1 XBOX360 SWIZZLED (only seen on xbox360)
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
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)untile.get(), filesize);
        if(ofile.fail()) return error("Write failure.");
       }
     // TEXTURE TYPE 0x12
     // DXT5 XBOX360 SWIZZLED (only seen on xbox360)
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
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)untile.get(), filesize);
        if(ofile.fail()) return error("Write failure.");
       }
     // TEXTURE TYPE 0x60
     // UNKNOWN 4 BITS PER PIXEL
     else if(b2 == 0x60)
       {
        do_delete = false;
        stringstream ss;
        ss << " Unsupported texture type 0x" << hex << (uint32)b2 << dec << " at offset 0x" << hex << (uint32)ifile.tellg() << dec << ".";
        ss << " DX/DY = " << dx << "/" << dy;
        message(ss.str().c_str());
       }
     // TEXTURE TYPE 0x62
     // UNKNOWN 8 BITS PER PIXEL
     else if(b2 == 0x62)
       {
        do_delete = false;
        stringstream ss;
        ss << " Unsupported texture type 0x" << hex << (uint32)b2 << dec << " at offset 0x" << hex << (uint32)ifile.tellg() << dec << ".";
        ss << " DX/DY = " << dx << "/" << dy;
        message(ss.str().c_str());
       }
     else {
        stringstream ss;
        ss << " Unsupported texture type 0x" << hex << (uint32)b2 << dec << " at offset 0x" << hex << (uint32)ifile.tellg() << dec << ".";
        ss << " DX/DY = " << dx << "/" << dy;
        return error(ss.str().c_str());
       }
    }

 // delete G1T file on success
 if(do_delete) {
    ifile.close();
    DeleteFile(filename);
   }

 return true;
}

bool UnpackTextures(LPCTSTR pathname, bool rbo)
{
 cout << "Processing .G1T files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".g1t"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!DWUnpackTexture(filelist[i].c_str(), rbo)) return false;
    }
 cout << endl;
 return true;
}

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

bool UnpackModels(LPCTSTR pathname)
{
 bool break_on_errors = true;
 cout << "Processing .G1M files..." << endl;
 deque<STDSTRING> filelist;
 BuildFilenameList(filelist, TEXT(".g1m"), pathname);
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!DWConvertModel(filelist[i].c_str())) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

};};