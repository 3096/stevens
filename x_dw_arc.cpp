#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_stream.h"
#include "x_dw_arc.h"

static const bool DELETE_SMALL_FILES = false;
static const uint32 SMALL_FILESIZE = 0x100;
static const uint32 LARGE_FILESIZE = 0x2000000;

#pragma region REGION_HEURISTICS
namespace DYNASTY_WARRIORS {

	bool ExcludeExtension(uint32 magic)
	{
		return false;
	}

	bool CheckMagic(uint32 value, uint32 test)
	{
		if (value == test) return true;
		else {
			reverse_byte_order(&value, 1);
			if (value == test) return true;
		}
		return false;
	}

	bool CheckMagic(uint32 value, uint32 a, uint32 b)
	{
		if (value >= a && value <= b) return true;
		else {
			reverse_byte_order(&value, 1);
			if (value >= a && value <= b) return true;
		}
		return false;
	}

	bool IsXLFile(LPCTSTR filename)
	{
		// TODO:
		// FORMAT:
		// UINT32 0x00134C58
		// UINT16 or UINT32 filesize
		// look at ARSLAN\LINKDATA\0001\001\005.unknown for a 32-bit one

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);

		// read first four bytes
		uint32 h01 = LE_read_uint32(ifile);
		if (ifile.fail()) return false;

		// small filesize
		if (filesize > 0xFFFF) {
			if (h01 == 0x00134C58) {
				uint32 h02 = LE_read_uint32(ifile);
				if (ifile.fail()) return false;
				if (h02 == filesize) return true;
			}
			else if (h01 == 0x584C1300) {
				uint32 h02 = BE_read_uint32(ifile);
				if (ifile.fail()) return false;
				if (h02 == filesize) return true;
			}
			else
				return false;
		}
		else {
			if (h01 == 0x00134C58) {
				uint32 h02 = LE_read_uint16(ifile);
				if (ifile.fail()) return false;
				if (h02 == filesize) return true;
			}
			else if (h01 == 0x584C1300) {
				uint32 h02 = BE_read_uint16(ifile);
				if (ifile.fail()) return false;
				if (h02 == filesize) return true;
			}
			else
				return false;
		}

		return false;
	}

	bool IsArchiveType0(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #0
		// 0x1EDE, #FILES, SCALE, 0x00, [SCALED OFFSET/LENGTH], [DATA]
		// USED BY OLDER DW GAMES THAT USE ANS, BNS, CNS ARCHIVES

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (filesize < 0x20) return false;

		// read magic
		uint32 magic = read_uint32(ifile, mode);
		if (ifile.fail()) return false;
		if (magic != 0x1EDE) return false;

		// read number of files
		uint32 n_files = read_uint32(ifile, mode);
		if (ifile.fail()) return false;
		if (n_files == 0) return false;
		if (!(n_files < 0xFFFF)) return false; // too many!

		// read scale
		uint32 scale = read_uint32(ifile, mode);
		if (ifile.fail()) return false;
		if (scale % 0x10) return false;

		// read zero
		uint32 zero = read_uint32(ifile, mode);
		if (ifile.fail()) return false;
		if (zero != 0x00) return false;

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
		for (uint32 i = 0; i < n_files; i++)
		{
			// read pair
			uint32 curr_offset = read_uint32(ifile, mode) * scale;
			uint32 curr_length = read_uint32(ifile, mode);
			uint32 curr_finish = curr_offset + curr_length;
			if (ifile.fail()) return false;

			// ignore pair
			if (curr_offset == 0 && curr_length == 0)
				continue;

			// ignore pair
			if (curr_offset != 0 && curr_length == 0)
				continue;

			// invalid pair
			if (curr_offset == 0 && curr_length != 0)
				return false;

			// offset and length must be less than filesize
			if (!(curr_offset < filesize)) return false;
			if (!(curr_length < filesize)) return false;

			// offset + length must be less than or equal to filesize
			if (filesize < curr_finish) return false;

			// compare with previous entries (check for overlapping data)
			if ((prev_offset != 0xFFFFFFFF) && (prev_length != 0xFFFFFFFF)) {
				if (!(prev_offset < curr_offset)) return false; // data overlaps
				if (!(prev_finish < curr_offset)) return false; // data overlaps
			}

			// insert pair
			ARC1ITEM item;
			item.offset = curr_offset;
			item.length = curr_length;
			entries.push_back(pair_t(i, item));
		}

		// must have at least one entry
		if (entries.size() == 0) return false;

		// first entry should match header size (non-aligned or aligned)
		uint32 head_offset = entries[0].second.offset;
		if ((head_offset != headersize1) && (head_offset != headersize2)) return false;

		// final entry should having finish close to the end
		uint32 last_offset = entries.back().second.offset;
		uint32 last_length = entries.back().second.length;
		if (!((filesize - (last_offset + last_length)) < 0x10)) return false;

		// most likely is
		return true;
	}

	bool IsArchiveType1(LPCTSTR filename, endian_mode mode)
	{
		// This is a LINKDATA.BIN type file, that has
		// uint32 magic
		// uint32 files
		// uint32 scale = 0x800
		// uint32 table = 0x000 or 0x001 
		// [list of entries]
		// [list of data]

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (filesize < 0x20) return false;

		// read magic
		uint32 magic = LE_read_uint32(ifile);
		if (ifile.fail()) return false;

		// from magic we have to determine endian, no matter what passed in mode is
		if (magic == 0x102A49) mode = ENDIAN_LITTLE;
		else if (magic == 0x492A1000ul) mode = ENDIAN_BIG;
		else return false;

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };
		auto read_uint64 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint64(ifile) : LE_read_uint64(ifile)); };

		// read number of files
		uint32 n_files = read_uint32(ifile, mode);
		if (ifile.fail()) return false;
		if (n_files == 0) return false;
		if (!(n_files < 0xFFFF)) return false; // too many!

		// read scale
		uint32 scale = read_uint32(ifile, mode);
		if (ifile.fail()) return false;
		if (scale % 0x800) return false;

		// read zero
		uint32 table = read_uint32(ifile, mode);
		if (ifile.fail()) return false;
		if (!(table == 0x00 || table == 0x01)) return false;

		// TODO: MORE ADVANCED TESTING OF ENTRIES
		// FOR NOW, LET'S JUST DO SIMPLE TEST
		return true;
	}

	bool IsArchiveType2(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #1
		// #FILES, [OFFSET/LENGTH], [DATA]
		//

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize for file
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (filesize < 0x08) return false;

		// read number of files
		uint32 n_files = read_uint32(ifile, mode);
		if (ifile.fail()) return false;

		// validate number of files
		if (n_files == 0) return false;
		if (!(n_files < 0xFFFF)) return false;

		// compute expected header sizes
		uint32 headersize1 = 0x04 + (n_files * 0x08); // no alignment
		uint32 headersize2;
		uint32 headersize3;
		headersize2 = ((headersize1 + 0x00F) & (~0x00F)); // 0x010-byte alignment
		headersize3 = ((headersize1 + 0x7FF) & (~0x7FF)); // 0x800-byte alignment

		// data
		struct ARC1ITEM { uint32 offset; uint32 length; };
		typedef pair<uint32, ARC1ITEM> pair_t;
		deque<pair_t> entries;
		uint32 prev_offset = 0xFFFFFFFF;
		uint32 prev_length = 0xFFFFFFFF;
		uint32 prev_finish = 0xFFFFFFFF;

		// read and insert pairs
		for (uint32 i = 0; i < n_files; i++)
		{
			// read pair
			uint32 curr_offset = read_uint32(ifile, mode);
			uint32 curr_length = read_uint32(ifile, mode);
			uint32 curr_finish = curr_offset + curr_length;
			if (ifile.fail()) return false;

			// ignore pair
			if (curr_offset == 0 && curr_length == 0)
				continue;

			// ignore pair
			if (curr_offset != 0 && curr_length == 0)
				continue;

			// invalid pair
			if (curr_offset == 0 && curr_length != 0)
				return false;

			// offset and length must be less than filesize
			if (!(curr_offset < filesize)) return false;
			if (!(curr_length < filesize)) return false;

			// offset + length must be less than or equal to filesize
			if (filesize < curr_finish) return false;

			// compare with previous entries (check for overlapping data)
			if ((prev_offset != 0xFFFFFFFF) && (prev_length != 0xFFFFFFFF)) {
				if (!(prev_offset < curr_offset)) return false; // data overlaps
				if (!(prev_finish < curr_offset)) return false; // data overlaps
			}

			// insert pair
			ARC1ITEM item;
			item.offset = curr_offset;
			item.length = curr_length;
			entries.push_back(pair_t(i, item));
		}

		// must have at least one entry
		if (entries.size() == 0) return false;

		// first entry should match header size (non-aligned or aligned)
		uint32 head_offset = entries[0].second.offset;
		if ((head_offset != headersize1) && (head_offset != headersize2) && (head_offset != headersize3)) return false;

		// final entry should having finish close to the end
		// use 0x800 to account for some archives having a lot of 0x00000000 padding
		uint32 last_offset = entries.back().second.offset;
		uint32 last_length = entries.back().second.length;
		if (!((filesize - (last_offset + last_length)) < 0x800)) return false;

		// most likely is
		return true;
	}

	bool IsArchiveType3(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #2
		// [OFFSET/LENGTH], [DATA]

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize for file
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (filesize < 0xFF) return false; // most likely not

		// pair types
		struct ARC1ITEM { uint32 offset; uint32 length; };
		typedef pair<uint32, ARC1ITEM> pair_t;
		deque<pair_t> entries;
		uint32 prev_offset = 0xFFFFFFFF;
		uint32 prev_length = 0xFFFFFFFF;
		uint32 prev_finish = 0xFFFFFFFF;

		// read pairs until we hit an invalid pair or until we come to the end of the data
		uint32 n_files = 0;
		for (uint32 index = 0; index < 0xFFFF; index++)
		{
			// read pair
			uint32 curr_offset = read_uint32(ifile, mode);
			uint32 curr_length = read_uint32(ifile, mode);
			uint32 curr_finish = curr_offset + curr_length;
			if (ifile.fail()) break;

			// null entry
			if ((curr_offset == 0) && (curr_length == 0)) {
				n_files++;
				continue;
			}

			// null entry
			if ((curr_offset != 0) && (curr_length == 0)) {
				n_files++;
				continue;
			}

			// impossible entry
			if ((curr_offset == 0) && (curr_length != 0))
				break;

			// offset and length must be less than filesize
			if (!(curr_offset < filesize)) break;
			if (!(curr_length < filesize)) break;

			// offset + length must be less than or equal to filesize
			if (filesize < curr_finish) break;

			// compare with previous entries (check for overlapping data)
			if ((prev_offset != 0xFFFFFFFF) && (prev_length != 0xFFFFFFFF)) {
				if (curr_offset < prev_offset) break; // data overlaps
				if (curr_offset < prev_finish) break; // data overlaps
			}

			// assume entry to be valid
			ARC1ITEM item;
			item.offset = curr_offset;
			item.length = curr_length;
			entries.push_back(pair_t(index, item));

			// check for final entry
			n_files++;
			if (curr_finish == filesize) break;

			// entry is now old
			prev_offset = curr_offset;
			prev_length = curr_length;
			prev_finish = curr_finish;
		}

		// we must have at least one entry
		if (!entries.size()) return false;

		// compute expected header sizes
		uint32 headersize1 = 0x04 * n_files;
		uint32 headersize2 = 0x04 * n_files;
		headersize2 = ((headersize1 + 0x0F) & (~0x0F));

		// first entry must match expected header size (non-aligned or aligned)
		uint32 head_offset = entries[0].second.offset;
		if ((head_offset != headersize1) && (head_offset != headersize2)) return false;

		// final valid entry should having finish close to the end
		uint32 last_offset = entries.back().second.offset;
		uint32 last_length = entries.back().second.length;
		uint32 last_finish = last_offset + last_length;
		if (!((filesize - last_finish) < 0x10)) return false;

		// most likely
		return true;
	}

	bool IsArchiveType4(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #3
		// #FILES, [LENGTH], [DATA]
		// DATA IS ALWAYS 0x10-BYTE ALIGNED
		//

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (filesize < 0x08) return false;

		// read number of files
		uint32 n_files = read_uint32(ifile, mode);
		if (ifile.fail()) return false;

		// validate number of files
		if (n_files == 0) return false;
		if (!(n_files < 0xFFFF)) return false;

		// read sizes
		deque<uint32> sizelist;
		uint64 databytes = 0;
		for (uint32 i = 0; i < n_files; i++)
		{
			// read size
			uint32 size = read_uint32(ifile, mode);
			if (ifile.fail()) return error("Test: read failure.");

			// validate size
			if (size == 0) continue; // OK to have zero-length data
			if (!(size < 0x8000000ul)) return false; // size is too long (80 MB)
			if (size % 0x10) return false; // always 0x10-byte aligned

			// insert size
			sizelist.push_back(size);
			databytes += size;
		}

		// sum should not be zero
		if (databytes == 0) return false;

		// align amount read to 0x10
		uint32 header_size = 0x04 + 0x04 * n_files;
		header_size = ((header_size + 0x0F) & (~0x0F));

		// computed expected filesize
		uint64 expected_filesize = header_size + databytes;
		if (filesize < expected_filesize) return false;
		if ((filesize - expected_filesize) > 0x0C) return false;

		// most likely is
		return true;
	}

	bool IsArchiveType5(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #4
		// #FILES, [OFFSET], [DATA]
		// THIS IS CHECKED FOR LAST SINCE WE CAN'T VALIDATE EXPECTED FILESIZE LIKE WE
		// CAN DO WITH THE OTHER ARCHIVES. THIS ARCHIVE IS TYPICALLY FILLED WITH G1A_
		// AND G2A_ FILES. THIS TYPE IS ASSUMED TO BE 0x10-BYTE ALIGNED. ARCHIVE TYPE
		// #5 IS THE NON-ALIGNED VERSION OF THIS ARCHIVE.

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize for IDX file
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (filesize < 0x20) return false;

		// read number of files
		uint32 n_files = read_uint32(ifile, mode);
		if (ifile.fail()) return false;

		// validate number of files
		if (n_files == 0) return false;
		if (!(n_files < 0xFFFF)) return false; // too many!

		// compute expected first offset
		uint32 expected_first = 0x04 + (n_files * 0x04);
		expected_first = ((expected_first + 0x0F) & (~0x0F));

		// read offsets (0x10-byte aligned and can be zero)
		typedef pair<uint32, uint32> pair_t;
		deque<pair_t> offsets;
		uint32 prev_offset = 0xFFFFFFFF;
		for (uint32 i = 0; i < n_files; i++)
		{
			// read offset
			uint32 item = read_uint32(ifile, mode);
			if (ifile.fail()) return false;

			// skip zero offsets
			if (item == 0) continue;

			// offset must be LESS THAN OR EQUAL TO the filesize
			// if offset is EQUAL to the filesize, last file is zero bytes
			if (filesize < item) return false;

			// offset must be divisible by 0x10
			if (item % 0x10) return false;

			// offset must be LESS THAN OR EQUAL TO the filesize
			// if offset is EQUAL to the filesize, last file is zero bytes
			if (filesize < item) return false;

			// offset must be GREATER THAN OR EQUAL to the previous offset
			if ((prev_offset != 0xFFFFFFFF) && (item < prev_offset))
				return false;

			// insert offset for now
			offsets.push_back(pair_t(i, item));
			prev_offset = item;
		}

		// first offset must be equal to the expected offset
		if (!offsets.size()) return false;
		return (offsets[0].second == expected_first);
	}

	bool IsArchiveType6(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #5
		// #FILES, [OFFSET], [DATA]
		// THIS IS CHECKED FOR LAST SINCE WE CAN'T VALIDATE EXPECTED FILESIZE LIKE WE
		// CAN DO WITH THE OTHER ARCHIVES. THIS ARCHIVE IS TYPICALLY FILLED WITH G1A_
		// AND G2A_ FILES. THIS TYPE IS THE NON-ALIGNED VERSION OF ARCHIVE TYPE #4.

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (filesize < 0x20) return false;

		// read number of files
		uint32 n_files = read_uint32(ifile, mode);
		if (ifile.fail()) return false;

		// validate number of files
		if (n_files == 0) return false;
		if (!(n_files < 0xFFF)) return false; // too many!

		// compute expected first offset
		uint32 expected_first1 = 0x04 + (n_files * 0x04); // aligned
		uint32 expected_first2 = 0x04 + (n_files * 0x04); // non-aligned
		expected_first1 = ((expected_first1 + 0x0F) & (~0x0F));

		// read offsets
		typedef pair<uint32, uint32> pair_t;
		deque<pair_t> offsets;
		uint32 prev_offset = 0xFFFFFFFF;
		for (uint32 i = 0; i < n_files; i++)
		{
			// read offset
			uint32 item = read_uint32(ifile, mode);
			if (ifile.fail()) return false;

			// skip zero offsets
			if (item == 0) continue;

			// offset must be LESS THAN OR EQUAL TO the filesize
			// if offset is EQUAL to the filesize, last file is zero bytes
			if (filesize < item) return false;

			// offset must be GREATER THAN OR EQUAL to the previous offset
			if ((prev_offset != 0xFFFFFFFF) && (item < prev_offset))
				return false;

			// insert offset for now
			offsets.push_back(pair_t(i, item));
			prev_offset = item;
		}

		// must have at least one offset
		if (!offsets.size()) return false;

		// first offset must be equal to one of the expected offsets
		if (offsets[0].second == expected_first1) return true;
		if (offsets[0].second == expected_first2) return true;
		return false;
	}

	bool IsArchiveType7(LPCTSTR filename, endian_mode mode)
	{
		// ARCHIVE TYPE #7 - DLC BIN FILES, [PC] ONE PIECE PIRATE WARRIORS 3
		// HEADER = 0x10 BYTES
		//  uint32 0x64 is this always 0x64 though?
		//  uint32 #FILES
		//  uint32 0x00
		//  uint32 0x00
		// [uint32 OFFSETS]
		// padding
		// [uint32 FILESIZES]
		// [DATA]

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (filesize < 0x20) return false;

		// read magic
		uint32 magic = LE_read_uint32(ifile);
		uint32 files = LE_read_uint32(ifile);
		uint32 zero1 = LE_read_uint32(ifile);
		uint32 zero2 = LE_read_uint32(ifile);
		if (ifile.fail()) return false;

		// override suggested endian mode
		if (magic == 0x64) mode = ENDIAN_LITTLE;
		else if (magic == 0x64000000ul) mode = ENDIAN_BIG;
		else return false;

		// validate
		if (zero1) return false;
		if (zero2) return false;

		// reverse files
		if (mode == ENDIAN_BIG) reverse_byte_order(&files);
		if (files == 0) return false;
		if (files > 0xFFFF) return false;

		// compute size of offset and sizes sections
		uint32 offsets_size = sizeof(uint32) * files;
		offsets_size = ((offsets_size + 0x7F) & (~0x7F));
		uint32 n_zeroes = offsets_size / 4 - files;

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// read offsets
		vector<uint32> offsets(files);
		for (uint32 i = 0; i < files; i++) {
			offsets[i] = read_uint32(ifile, mode);
			if (offsets[i] > filesize) return false;
			if (i && offsets[i] < offsets[i - 1]) return false;
		}
		// read zeroes
		for (uint32 i = 0; i < n_zeroes; i++) {
			auto z = read_uint32(ifile, mode);
			if (z) return false;
		}

		// read sizes
		vector<uint32> sizes(files);
		for (uint32 i = 0; i < files; i++) {
			sizes[i] = read_uint32(ifile, mode);
			if (sizes[i] > filesize) return false;
			if ((offsets[i] + sizes[i]) > filesize) return false;
		}
		// read zeroes
		for (uint32 i = 0; i < n_zeroes; i++) {
			auto z = read_uint32(ifile, mode);
			if (z) return false;
		}

		// most likely
		return true;
	}

	bool IsArchiveType8(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #6
		// [OFFSET], [DATA]
		// THIS FILE TYPE STARTS WITH AN INCREASING LIST OF FILE OFFSETS. ASSUMING
		// THERE IS NO PADDING BETWEEN HEADER AND DATA, THE NUMBER OF FILES IS EQUAL
		// TO (FIRST_OFFSET/4). HOWEVER, I THINK IT IS BEST TO IGNORE THIS TYPE OF
		// ARCHIVE BECAUSE THE HEURSTIC WILL GENERATE A LOT OF FALSE POSITIVES. FOR
		// EXAMPLE, 00 00 00 04 [data] is an archive with one file.

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize for IDX file
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (filesize < 0x20) return false;

		// read first offset
		uint32 offset = read_uint32(ifile, mode);
		if (offset % 4) return false; // must be divisible by 4

		// compute number of files
		uint32 n_files = offset / 4;
		if (n_files < 0x04) return false; // very rare
		if (n_files > 0x1FFF) return false; // very rare

		// read offsets
		boost::shared_array<uint32> offset_list(new uint32[n_files]);
		offset_list[0] = offset;
		for (uint32 i = 1; i < n_files; i++) {
			// each offset must be greater than previous offset
			uint32 offset = read_uint32(ifile, mode);
			if (ifile.fail()) return false;
			if (!(offset > offset_list[i - 1])) return false;
			offset_list[i] = offset;
		}

		// probably an archive file
		return true;
	}

	bool IsCompressedType1(LPCTSTR filename, endian_mode mode)
	{
		//
		// IDXZRC TYPE #1
		// UINT32 - magic number
		// UINT32 - number of chunks
		// UINT32 - decompressed size
		// [UINT32] - array of chunk sizes
		// [UINT32 + chunk data]

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };
		auto read_uint32_array = [](std::ifstream& ifile, uint32* data, uint32 n, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_array(ifile, data, n) : LE_read_array(ifile, data, n)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize for IDX file
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);

		// filesize must be greater than 0x90
		// this is because the first ZLIB chunk is 0x80-byte aligned
		// plus compressed data that is usually more than 0x10 bytes
		if (filesize < 0x90) return false;

		// read magic number
		uint32 magic = read_uint32(ifile, mode);
		if (ifile.fail()) return false;

		// read number of chunks
		uint32 n_chunks = read_uint32(ifile, mode);
		if (ifile.fail()) return false;

		// must have chunks
		if (!n_chunks) return false; // no chunks
		if (!(n_chunks < 0xFFFF)) return false; // too many chunks

		// read decompressed size
		uint32 decombytes = read_uint32(ifile, mode);
		if (ifile.fail()) return false;

		// decompressed bytes cannot be zero
		if (decombytes == 0) return false;

		// read chunk sizes
		boost::shared_array<uint32> sizes(new uint32[n_chunks]);
		if (!read_uint32_array(ifile, sizes.get(), n_chunks, mode)) return false;
		for (uint32 i = 0; i < n_chunks; i++) if (!(sizes[i] < filesize)) return false; // size too large

		// save current position
		uint32 position = (uint32)ifile.tellg();
		if (ifile.fail()) return false;

		// move to nearest 0x80-byte alignment
		position = ((position + 0x7F) & ~(0x7F));
		ifile.seekg(position);
		if (ifile.fail()) return false;

		// for each chunk
		for (uint32 i = 0; i < n_chunks; i++)
		{
			// read size of data
			uint32 n = read_uint32(ifile, mode);
			if (ifile.fail()) return false;

			// must have something to decompress
			if (n == 0) return false;
			if (n + 4 != sizes[i]) return false;

			// read next two bytes
			uint08 b1 = LE_read_uint08(ifile);
			uint08 b2 = LE_read_uint08(ifile);
			if (ifile.fail()) return false;

			// is this a ZLIB chunk?
			return isZLIB(b1, b2);
		}

		// probably is
		return true;
	}

	bool IsCompressedType2(LPCTSTR filename, endian_mode mode)
	{
		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);

		// filesize must be greater than 0x80
		// this is because the first ZLIB chunk is 0x80-byte aligned
		if (filesize < 0x80) return false;

		// read unpacked size
		uint32 unpacked_size = read_uint32(ifile, mode);
		if (ifile.fail()) return false;

		// unpacked size must not be small
		if (unpacked_size < SMALL_FILESIZE)
			return false;

		// loop ZLIB sections
		uint32 sections = 0;
		for (;;)
		{
			// read compressed data size
			uint32 datasize = read_uint32(ifile, mode);
			if (ifile.fail()) return false;

			// stop when datasize is zero
			if (!datasize) break;

			// datasize must be less than the filesize
			if (!(datasize < filesize)) return false;

			// read next two bytes
			uint08 b1 = LE_read_uint08(ifile);
			uint08 b2 = LE_read_uint08(ifile);
			if (ifile.fail()) return false;

			// is this ZLIB?
			if (!isZLIB(b1, b2)) return false;
			// if(b1 != 0x78) return false;
			// if(b2 != 0xDA) return false;

			// move to next section
			ifile.seekg(datasize - 2, ios::cur);
			if (ifile.fail()) return false;

			// increment number of sections
			sections++;
		}

		// OK if number of sections is valid
		return (sections > 0);
	}

	bool IsCompressedType3(LPCTSTR filename, endian_mode mode)
	{
		// [PS3] Dragon Quest Heroes II (look at unknown files)

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// compute filesize
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);

		// filesize must be greater than 0x800
		// this is because the first ZLIB chunk is 0x800-byte aligned
		if (filesize < 0x800) return false;

		// magic number is unique
		uint32 magic = read_uint32(ifile, mode);
		if (magic == 0x31707A) return true;
		return false;
	}

	bool HeuristicTestA(LPCTSTR filename, LPTSTR extension)
	{
		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return false;

		// read first four bytes
		uint32 head01 = LE_read_uint32(ifile);
		if (ifile.fail()) return false;

		// close file
		ifile.close();

		// ranged
		if (CheckMagic(head01, 0x39470000ul, 0x3947FFFFul)) { StringCchCopy(extension, 8, TEXT("9G")); return true; } // 9G
		if (CheckMagic(head01, 0x4C580013ul, 0x4C580014ul)) { StringCchCopy(extension, 8, TEXT("lx")); return true; } // LX__

		// 3-letter
		if (CheckMagic(head01, 0x3026B275ul)) { StringCchCopy(extension, 8, TEXT("asf")); return true; } // ASF
		if (CheckMagic(head01, 0x4731415Ful)) { StringCchCopy(extension, 8, TEXT("g1a")); return true; } // G1A_
		if (CheckMagic(head01, 0x47314C5Ful)) { StringCchCopy(extension, 8, TEXT("g1l")); return true; } // G1L_
		if (CheckMagic(head01, 0x47314D5Ful)) { StringCchCopy(extension, 8, TEXT("g1m")); return true; } // G1M_
		if (CheckMagic(head01, 0x4731505Ful)) { StringCchCopy(extension, 8, TEXT("g1p")); return true; } // G1P_ = archive of G1PS [PC] Berserk Musou (2017)
		if (CheckMagic(head01, 0x47315447ul)) { StringCchCopy(extension, 8, TEXT("g1t")); return true; } // G1TG
		if (CheckMagic(head01, 0x4732415Ful)) { StringCchCopy(extension, 8, TEXT("g2a")); return true; } // G2A_
		if (CheckMagic(head01, 0x4732535Ful)) { StringCchCopy(extension, 8, TEXT("g2s")); return true; } // G2S_
		if (CheckMagic(head01, 0x4B484D5Ful)) { StringCchCopy(extension, 8, TEXT("khm")); return true; } // KHM_ [PC] Attack on Titan: Wings of Freedom (2016)
		if (CheckMagic(head01, 0x4B4F435Ful)) { StringCchCopy(extension, 8, TEXT("koc")); return true; } // KOC_
		if (CheckMagic(head01, 0x4B50535Ful)) { StringCchCopy(extension, 8, TEXT("kps")); return true; } // KPS_
		if (CheckMagic(head01, 0x4B52445Ful)) { StringCchCopy(extension, 8, TEXT("krd")); return true; } // KRD_
		if (CheckMagic(head01, 0x89504E47ul)) { StringCchCopy(extension, 8, TEXT("png")); return true; } // PNG
		if (CheckMagic(head01, 0x534C4F5Ful)) { StringCchCopy(extension, 8, TEXT("slo")); return true; } // SLO_
		if (CheckMagic(head01, 0x5356445Ful)) { StringCchCopy(extension, 8, TEXT("svd")); return true; } // SVD_
		if (CheckMagic(head01, 0x5742445Ful)) { StringCchCopy(extension, 8, TEXT("wbd")); return true; } // WBD_
		if (CheckMagic(head01, 0x5742485Ful)) { StringCchCopy(extension, 8, TEXT("wbh")); return true; } // WBH_

		// 4-letter
		if (CheckMagic(head01, 0x32444F54ul)) { StringCchCopy(extension, 8, TEXT("2dot")); return true; } // 2DOT
		if (CheckMagic(head01, 0x414C5343ul)) { StringCchCopy(extension, 8, TEXT("alsc")); return true; } // ALSC
		if (CheckMagic(head01, 0x41545350ul)) { StringCchCopy(extension, 8, TEXT("atsp")); return true; } // ATSP
		if (CheckMagic(head01, 0x43425844ul)) { StringCchCopy(extension, 8, TEXT("cbxd")); return true; } // CBXD
		if (CheckMagic(head01, 0x434F4C4Bul)) { StringCchCopy(extension, 8, TEXT("colk")); return true; } // COLK
		if (CheckMagic(head01, 0x43535442ul)) { StringCchCopy(extension, 8, TEXT("cstb")); return true; } // CSTB ([PC] SW4-II) a type of zip file
		if (CheckMagic(head01, 0x46455841ul)) { StringCchCopy(extension, 8, TEXT("fexa")); return true; } // FEXA
		if (CheckMagic(head01, 0x4731434Ful)) { StringCchCopy(extension, 8, TEXT("g1co")); return true; } // G1CO
		if (CheckMagic(head01, 0x47314354ul)) { StringCchCopy(extension, 8, TEXT("g1ct")); return true; } // G1CT
		if (CheckMagic(head01, 0x47314358ul)) { StringCchCopy(extension, 8, TEXT("g1cx")); return true; } // G1CX = archive of G1CO [PC] Berserk Musou (2017)
		if (CheckMagic(head01, 0x4731454Dul)) { StringCchCopy(extension, 8, TEXT("g1em")); return true; } // G1EM
		if (CheckMagic(head01, 0x47314658ul)) { StringCchCopy(extension, 8, TEXT("g1fx")); return true; } // G1FX
		if (CheckMagic(head01, 0x47314D58ul)) { StringCchCopy(extension, 8, TEXT("g1mx")); return true; } // G1MX [PC] Attack on Titan: Wings of Freedom (2016)
		if (CheckMagic(head01, 0x47315045ul)) { StringCchCopy(extension, 8, TEXT("g1pe")); return true; } // G1PE [PC] Berserk Musou (2017)
		if (CheckMagic(head01, 0x47315046ul)) { StringCchCopy(extension, 8, TEXT("g1pf")); return true; } // G1PF
		if (CheckMagic(head01, 0x47315053ul)) { StringCchCopy(extension, 8, TEXT("g1ps")); return true; } // G1PS [PC] Berserk Musou (2017)
		if (CheckMagic(head01, 0x47315653ul)) { StringCchCopy(extension, 8, TEXT("g1vs")); return true; } // G1VS
		if (CheckMagic(head01, 0x47494D44ul)) { StringCchCopy(extension, 8, TEXT("gimd")); return true; } // GIMD
		if (CheckMagic(head01, 0x47524153ul)) { StringCchCopy(extension, 8, TEXT("gras")); return true; } // GRAS
		if (CheckMagic(head01, 0x494E464Ful)) { StringCchCopy(extension, 8, TEXT("info")); return true; } // INFO
		if (CheckMagic(head01, 0x4B505333ul)) { StringCchCopy(extension, 8, TEXT("kps3")); return true; } // KPS3 [PC] Berserk Musou (2017)
		if (CheckMagic(head01, 0x4B53484Cul)) { StringCchCopy(extension, 8, TEXT("kshl")); return true; } // KSHL
		if (CheckMagic(head01, 0x4B544643ul)) { StringCchCopy(extension, 8, TEXT("ktfc")); return true; } // KTFC
		if (CheckMagic(head01, 0x4B54464Bul)) { StringCchCopy(extension, 8, TEXT("ktfk")); return true; } // KTFK
		if (CheckMagic(head01, 0x4B545343ul)) { StringCchCopy(extension, 8, TEXT("ktsc")); return true; } // KTSC [PC] Attack on Titan: Wings of Freedom (2016)
		if (CheckMagic(head01, 0x4B545352ul)) { StringCchCopy(extension, 8, TEXT("ktsr")); return true; } // KTSR [PC] Attack on Titan: Wings of Freedom (2016)
		if (CheckMagic(head01, 0x4D444C4Bul)) { StringCchCopy(extension, 8, TEXT("mdlk")); return true; } // MDLK
		if (CheckMagic(head01, 0x4D455353ul)) { StringCchCopy(extension, 8, TEXT("mess")); return true; } // MESS
		if (CheckMagic(head01, 0x4F324F42ul)) { StringCchCopy(extension, 8, TEXT("o2ob")); return true; } // O2OB
		if (CheckMagic(head01, 0x4F424A44ul)) { StringCchCopy(extension, 8, TEXT("objd")); return true; } // OBJD
		if (CheckMagic(head01, 0x4F535447ul)) { StringCchCopy(extension, 8, TEXT("ostg")); return true; } // OSTG
		if (CheckMagic(head01, 0x52494646ul)) { StringCchCopy(extension, 8, TEXT("riff")); return true; } // RIFF
		if (CheckMagic(head01, 0x52494742ul)) { StringCchCopy(extension, 8, TEXT("rigb")); return true; } // RIGB
		if (CheckMagic(head01, 0x53545F4Dul)) { StringCchCopy(extension, 8, TEXT("st_m")); return true; } // ST_M
		if (CheckMagic(head01, 0x53564F4Bul)) { StringCchCopy(extension, 8, TEXT("svok")); return true; } // SVOK
		if (CheckMagic(head01, 0x53574751ul)) { StringCchCopy(extension, 8, TEXT("swgq")); return true; } // SWGQ
		if (CheckMagic(head01, 0x58595748ul)) { StringCchCopy(extension, 8, TEXT("xywh")); return true; } // XYWH
		if (CheckMagic(head01, 0x5A464C52ul)) { StringCchCopy(extension, 8, TEXT("zflr")); return true; } // ZFLR
		if (CheckMagic(head01, 0x5A4F424Aul)) { StringCchCopy(extension, 8, TEXT("zobj")); return true; } // ZOBJ
		if (CheckMagic(head01, 0x5A535345ul)) { StringCchCopy(extension, 8, TEXT("zsse")); return true; } // ZSSE

		// special
		if (IsXLFile(filename)) {
			StringCchCopy(extension, 8, TEXT("xl"));
			return true;
		}

		// failed
		return false;
	}

	bool HeuristicTestB(LPCTSTR filename, LPTSTR extension)
	{
		// TODO:
		// // BIN
		// if(DWIsBinTypeA(filename)) {
		//    StringCchCopy(extension, 8, L"bin");
		//    return true;
		//   }
		// // G1FXLIST
		// if(DWIsGfx_ListA(filename)) {
		//    StringCchCopy(extension, 8, L"gfxl");
		//    return true;
		//   }
		return false;
	}

	bool HeuristicTestC(LPCTSTR filename, LPTSTR extension)
	{
		// LE
		if (IsArchiveType0(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"larc0"); return true; }
		if (IsArchiveType1(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"larc1"); return true; }
		if (IsArchiveType2(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"larc2"); return true; }
		if (IsArchiveType3(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"larc3"); return true; }
		if (IsArchiveType4(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"larc4"); return true; }
		if (IsArchiveType5(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"larc5"); return true; }
		if (IsArchiveType6(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"larc6"); return true; }
		if (IsArchiveType7(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"larc7"); return true; }

		// BE
		if (IsArchiveType0(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"barc0"); return true; }
		if (IsArchiveType1(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"barc1"); return true; }
		if (IsArchiveType2(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"barc2"); return true; }
		if (IsArchiveType3(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"barc3"); return true; }
		if (IsArchiveType4(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"barc4"); return true; }
		if (IsArchiveType5(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"barc5"); return true; }
		if (IsArchiveType6(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"barc6"); return true; }
		if (IsArchiveType7(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"barc7"); return true; }

		return false;
	}

	bool HeuristicTestD(LPCTSTR filename, LPTSTR extension)
	{
		// LE
		if (IsCompressedType1(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"idxzrc"); return true; }
		if (IsCompressedType2(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"idxzip"); return true; }
		if (IsCompressedType3(filename, ENDIAN_LITTLE)) { StringCchCopy(extension, 8, L"idxlpz"); return true; }

		// BE
		if (IsCompressedType1(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"idxzrc"); return true; }
		if (IsCompressedType2(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"idxzip"); return true; }
		if (IsCompressedType3(filename, ENDIAN_BIG)) { StringCchCopy(extension, 8, L"idxlpz"); return true; }

		return false;
	}

	bool FileExtensionHeuristic(LPCTSTR filename, LPTSTR extension)
	{
		// try these first
		if (HeuristicTestA(filename, extension)) return true; // MAGIC NUMBERS
		if (HeuristicTestB(filename, extension)) return true; // GFXL, IDX/BIN 
		if (HeuristicTestC(filename, extension)) return true; // ARC1, ARC2, etc.
		if (HeuristicTestD(filename, extension)) return true; // IDXZRC

		// and if all else fails
		StringCchCopy(extension, 8, L"unknown");
		return true;
	}

}
#pragma endregion

#pragma region REGION_DECOMPRESSION
namespace DYNASTY_WARRIORS {

	bool IDXZRC_PathExtract(LPCTSTR pathname, endian_mode mode)
	{
		using namespace std;
		cout << "Processing .IDXZRC files..." << endl;
		deque<STDSTRING> filelist;
		BuildFilenameList(filelist, TEXT(".IDXZRC"), pathname);
		for (size_t i = 0; i < filelist.size(); i++) {
			wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
			if (!IDXZRC_FileExtract(filelist[i].c_str(), mode)) return false;
		}
		cout << endl;
		return true;
	}

	bool IDXZRC_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// 02/14/2018 [PC] Dynasty Warriors 9 (2018)
		// if this is actually an LPZ file, we must detect it here!
		// the last byte can be a 01 or 02
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };
		uint32 temp = read_uint32(ifile, mode);
		temp = temp & 0x00FFFFFF;
		if (temp == 0x0031707Aul) {
			ifile.close();
			return IDXLPZ_FileExtract(filename, mode);
		}

		// read file data
		boost::shared_array<char> data(new char[filesize]);
		ifile.seekg(0);
		ifile.read(data.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// close file
		ifile.close();

		// convert IDX data to stream data
		binary_stream bs(data, filesize);
		bs.set_endian_mode(mode);

		// read magic
		uint32 magic = bs.read_uint32();
		if (bs.fail()) return error("Stream read failure.");

		// read number of files (it is possible to have 0 files)
		uint32 n_files = bs.read_uint32();
		if (n_files == 0) {
			DeleteFile(filename);
			return true;
		}

		// read uncompressed file size
		bs.read_uint32();
		if (bs.fail()) return error("Read failure.");

		// read chunk sizes
		boost::shared_array<uint32> sizes(new uint32[n_files]);
		bs.read_array(sizes.get(), n_files);
		if (bs.fail()) return error("Read failure.");

		// create output filename
		STDSTRING shrtname;
		shrtname += GetPathnameFromFilename(filename).get();
		shrtname += GetShortFilenameWithoutExtension(filename).get();

		// create output file
		STDSTRINGSTREAM ss;
		ss << shrtname << TEXT(".idxout");
		ofstream ofile(ss.str().c_str(), ios::binary);
		if (!ofile) return error("Failed to create output file.");

		// for each chunk
		for (uint32 i = 0; i < n_files; i++)
		{
			// record position
			uint32 position = (uint32)bs.tell();
			if (bs.fail()) return error("Stream tell failure.");

			// seek to nearest 0x80-byte alignment
			position = ((position + 0x7F) & ~(0x7F));
			bs.seek(position);
			if (bs.fail()) return error("Stream seek failure.");

			// magic == 0xFFFFFFFF
			// contents are nothing but zlib chunks
			if (magic == 0xFFFFFFFF)
			{
				// read data
				boost::shared_array<char> temp(new char[sizes[i]]);
				bs.read(temp.get(), sizes[i]);
				if (bs.fail()) return error("Stream read failure.");

				// save data
				bool status = InflateZLIB(temp.get(), sizes[i], ofile, -15);
				if (!status) return false;
			}
			else
			{
				// read size of data
				uint32 n = bs.read_uint32();
				if (bs.fail()) return error("Read failure.");

				// compressed
				if ((sizes[i] > 4) && (n == sizes[i] - 4)) {
					bool status = InflateZLIB(bs.c_pos(), n, ofile, 0);
					if (!status) return false;
				}
				// uncompressed
				else {
					bs.seek(position);
					if (bs.fail()) return error("Stream seek failure.");
					ofile.write(bs.c_pos(), sizes[i]);
					if (ofile.fail()) return error("Write failure.");
				}

				// move to next position
				bs.seek(position + sizes[i]);
				if (bs.fail()) return error("Stream seek failure.");
			}
		}

		// cleanup IDXZRC file
		ifile.close();
		DeleteFile(filename);
		return true;
	}

	bool IDXZIP_PathExtract(LPCTSTR pathname, endian_mode mode)
	{
		using namespace std;
		cout << "Processing .IDXZIP files..." << endl;
		deque<STDSTRING> filelist;
		BuildFilenameList(filelist, TEXT(".IDXZIP"), pathname);
		for (size_t i = 0; i < filelist.size(); i++) {
			wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
			if (!IDXZIP_FileExtract(filelist[i].c_str(), mode)) return false;
		}
		cout << endl;
		return true;
	}

	bool IDXZIP_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// read file data
		boost::shared_array<char> data(new char[filesize]);
		ifile.read(data.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// close file
		ifile.close();

		// convert IDX data to stream data
		binary_stream bs(data, filesize);
		bs.set_endian_mode(mode);

		// total unpacked size
		// if zero then we have to delete this file
		uint32 unpacked_size = bs.read_uint32();
		if (!unpacked_size) {
			DeleteFile(filename);
			return true;
		}

		// create output filename
		STDSTRING shrtname;
		shrtname += GetPathnameFromFilename(filename).get();
		shrtname += GetShortFilenameWithoutExtension(filename).get();

		// create output file
		STDSTRINGSTREAM ss;
		ss << shrtname << TEXT(".idxout");
		ofstream ofile(ss.str().c_str(), ios::binary);
		if (!ofile) return error("Failed to create output file.");

		// loop forever
		for (;;)
		{
			// read datasize
			uint32 datasize = bs.read_uint32();
			if (ifile.fail()) return error("Stream seek failure.");
			if (!datasize) break;

			// make sure we have the right endian!
			// sometimes our guess or suggestion could be wrong!
			// because idxzip files contain compressed chunks, they should be smaller than 0xFFFF!
			if (datasize > 0xFFFF) reverse_byte_order(&datasize);

			// decompress and save
			// use 0 instead of -15 since these zlib chunks start with 0x78DA
			bool status = InflateZLIB(bs.c_pos(), datasize, ofile, 0);
			if (!status) return error("ZLIB failure.");

			// move to next position
			bs.move(datasize);
		}

		// cleanup ZRC file
		ifile.close();
		DeleteFile(filename);
		return true;
	}

	bool IDXLPZ_PathExtract(LPCTSTR pathname, endian_mode mode)
	{
		using namespace std;
		cout << "Processing .IDXLPZ files..." << endl;
		deque<STDSTRING> filelist;
		BuildFilenameList(filelist, TEXT(".IDXLPZ"), pathname);
		for (size_t i = 0; i < filelist.size(); i++) {
			wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
			if (!IDXLPZ_FileExtract(filelist[i].c_str(), mode)) return false;
		}
		cout << endl;
		return true;
	}

	bool IDXLPZ_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// read file data
		boost::shared_array<char> data(new char[filesize]);
		ifile.read(data.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// close file
		ifile.close();

		// convert file data to stream data
		binary_stream bs(data, filesize);
		bs.set_endian_mode(mode);

		// read magic
		uint32 magic = bs.read_uint32();
		magic = magic & 0x00FFFFFF;
		if (magic != 0x31707A) return error("This is not an LPZ file.");

		// read ???
		uint32 usize = bs.read_uint32(); // uncompressed size?
		uint32 unk01 = bs.read_uint32();

		// read number of files
		uint32 files = bs.read_uint32();
		if (!files) return true;

		// read file sizes
		std::vector<uint32> sizelist;
		for (uint32 i = 0; i < files; i++) sizelist.push_back(bs.read_uint32());

		// create output filename
		STDSTRING shrtname;
		shrtname += GetPathnameFromFilename(filename).get();
		shrtname += GetShortFilenameWithoutExtension(filename).get();

		// create output file
		STDSTRINGSTREAM ss;
		ss << shrtname << TEXT(".idxout");
		ofstream ofile(ss.str().c_str(), ios::binary);
		if (!ofile) return error("Failed to create output file.");

		// for each chunk
		for (uint32 i = 0; i < files; i++)
		{
			// record position
			uint32 position = (uint32)bs.tell();
			if (bs.fail()) return error("Stream tell failure.");

			// seek to nearest 0x800 (if first) or 0x80-byte alignment
			if (i == 0) position = ((position + 0x7FF) & ~(0x7FF));
			else position = ((position + 0x7F) & ~(0x7F));
			bs.seek(position);
			if (bs.fail()) return error("Stream seek failure.");

			// read size of data
			uint32 n = bs.read_uint32();
			if (bs.fail()) return error("Read failure.");

			// compressed
			if ((sizelist[i] > 4) && (n == sizelist[i] - 4)) {
				bool status = InflateZLIB(bs.c_pos(), n, ofile, 0);
				if (!status) return false;
			}
			// uncompressed
			else {
				bs.seek(position);
				if (bs.fail()) return error("Stream seek failure.");
				ofile.write(bs.c_pos(), sizelist[i]);
				if (ofile.fail()) return error("Write failure.");
			}

			// move to next position
			bs.seek(position + sizelist[i]);
			if (bs.fail()) return error("Stream seek failure.");
		}

		// cleanup LPZ file
		DeleteFile(filename);
		return true;
	}

}

#pragma endregion

#pragma region REGION_ARCHIVE_FUNCTIONS1
namespace DYNASTY_WARRIORS {

	//
	// IDX ARCHIVE FORMAT #1
	// GAMES:
	//
	// [
	//  uint64 p1; // offset
	//  uint64 p2; // length (uncompressed)
	//  uint64 p3; // length (compressed)
	//  uint64 p4; // filetype (idxout our idxzrc)
	// ]
	//
	bool UnpackIDX_A(LPCTSTR filename, endian_mode mode, LPCTSTR binprefix)
	{
		// unpacking messages
		using namespace std;
		message("Unpacking .IDX/.BIN file...");

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// open IDX file
		ifstream ifile(filename, ios::binary);
		if (!ifile) {
			STDSTRINGSTREAM ss;
			ss << L"Failed to open IDX file." << endl;
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
		if (binprefix) bfname += binprefix;
		else bfname += shrtname;
		bfname += TEXT(".BIN");

		// open BIN file
		ifstream bfile(bfname.c_str(), ios::binary);
		if (!bfile) return error("Failed to open BIN file.");

		// create destination directory
		STDSTRINGSTREAM ss;
		ss << pathname << shrtname << L"\\";
		STDSTRING savepath = ss.str();
		CreateDirectoryW(savepath.c_str(), NULL);

		// read IDX file
		boost::shared_array<char> idxdata(new char[filesize]);
		ifile.read(idxdata.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// convert IDX data to stream data
		binary_stream bs(idxdata, filesize);
		bs.set_endian_mode(mode);

		// read file offsets
		struct IDXITEM {
			uint64 p1; // offset
			uint64 p2; // length (uncompressed)
			uint64 p3; // length (compressed)
			uint64 p4; // filetype (idxout our idxzrc)
		};
		deque<IDXITEM> itemlist;
		for (;;)
		{
			// read item
			IDXITEM item;
			item.p1 = bs.read_uint64(); if (bs.fail()) break;
			item.p2 = bs.read_uint64(); if (bs.fail()) break;
			item.p3 = bs.read_uint64(); if (bs.fail()) break;
			item.p4 = bs.read_uint64(); if (bs.fail()) break;

			// p4 must be 0 or 1
			if (!(item.p4 == 0 || item.p4 == 1)) {
				STDSTRINGSTREAM ss;
				ss << "Unknown IDX file type 0x" << hex << item.p4 << dec << ".";
				return error(ss);
			}

			// validate item
			if ((item.p4 == 0) && (item.p2 != item.p3)) {
				stringstream ss;
				ss << "IDX file lengths do not match at offset 0x" << hex << ((uint32)ifile.tellg() - 0x20) << dec << ".";
				return error(ss);
			}

			itemlist.push_back(item);
		}

		// save items
		for (size_t i = 0; i < itemlist.size(); i++)
		{
			// skip empty idx
			if (itemlist[i].p2 == 0 || itemlist[i].p3 == 0) continue;

			// construct output filename
			STDSTRINGSTREAM ss;
			ss << savepath << i << L".";
			if (itemlist[i].p4 == 0) ss << L"idxout";
			else if (itemlist[i].p4 == 1) ss << L"idxzrc";
			else {
				STDSTRINGSTREAM ss;
				ss << "Unknown IDX file type 0x" << hex << itemlist[i].p4 << dec << ".";
				return error(ss);
			}

			// display information and ignore small files
			wcout << " FILE " << (i + 1) << " of " << itemlist.size() << ": " << ss.str() << endl;
			if (!(itemlist[i].p3 > SMALL_FILESIZE)) continue;

			// read first four bytes and determine whether or not to save file
			bfile.seekg(itemlist[i].p1);
			if (bfile.fail()) return error("Seek failure.");
			uint32 magic = (mode == ENDIAN_LITTLE ? LE_read_uint32(bfile) : BE_read_uint32(bfile));
			if (bfile.fail() || ExcludeExtension(magic)) continue;

			// create 16 MB buffer and move to bin data
			boost::shared_array<char> data(new char[0x1000000]);
			bfile.seekg(itemlist[i].p1);
			if (bfile.fail()) return error("Seek failure.");

			// keep track of remaining bytes
			uint64 bytes_remaining = itemlist[i].p3;
			if (!(bytes_remaining < LARGE_FILESIZE))
				message("NOTE: This is a large file and might take a while to save.");

			// create output file
			ofstream ofile(ss.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create output file.");

			// transfer data
			while (bytes_remaining) {
				if (bytes_remaining < 0x1000000) {
					// read BIN data
					bfile.read(data.get(), bytes_remaining);
					if (bfile.fail()) return error("Read failure.");
					// save BIN data
					ofile.write(data.get(), bytes_remaining);
					if (ofile.fail()) return error("Write failure.");
					// nothing more to save
					bytes_remaining = 0;
				}
				else {
					// read BIN data
					bfile.read(data.get(), 0x1000000);
					if (bfile.fail()) return error("Read failure.");
					// save BIN data
					ofile.write(data.get(), 0x1000000);
					if (ofile.fail()) return error("Write failure.");
					// more to save
					bytes_remaining -= 0x1000000;
				}
			}
		}

		cout << endl;
		return true;
	}

	//
	// IDX ARCHIVE FORMAT #2
	// GAMES:
	//  [PC] Dynasty Warriors Xtreme Legends Complete Edition
	// FORMAT:
	//  SINGLE LINKDATA.IDX
	//  LINKDATA0.BIN
	//  LINKDATA1.BIN
	//  etc.
	//  [
	//   uint64 p1; // offset (move to next data file on 0x00000000)
	//   uint64 p2; // length (uncompressed)
	//   uint64 p3; // length (compressed)
	//   uint64 p4; // filetype (idxout our idxzrc)
	//  ]
	//
	bool UnpackIDX_B(LPCTSTR filename, endian_mode mode)
	{
		// unpacking messages
		using namespace std;
		message("Unpacking .IDX/.BIN file...");

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// open IDX file
		ifstream ifile(filename, ios::binary);
		if (!ifile) {
			STDSTRINGSTREAM ss;
			ss << L"Failed to open IDX file." << endl;
			ss << filename;
			return error(ss);
		}

		// compute filesize for IDX file
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);

		// read IDX file
		boost::shared_array<char> idxdata(new char[filesize]);
		ifile.read(idxdata.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// convert IDX data to stream data
		binary_stream bs(idxdata, filesize);
		bs.set_endian_mode(mode);

		// read file offsets
		struct IDXITEM {
			uint64 p1; // offset
			uint64 p2; // length (uncompressed)
			uint64 p3; // length (compressed)
			uint64 p4; // filetype (idxout our idxzrc)
		};
		deque<IDXITEM> itemlist;
		for (;;)
		{
			// read item
			IDXITEM item;
			item.p1 = bs.read_uint64(); if (bs.fail()) break;
			item.p2 = bs.read_uint64(); if (bs.fail()) break;
			item.p3 = bs.read_uint64(); if (bs.fail()) break;
			item.p4 = bs.read_uint64(); if (bs.fail()) break;

			// p4 must be 0 or 1
			if (!(item.p4 == 0 || item.p4 == 1)) {
				STDSTRINGSTREAM ss;
				ss << "Unknown IDX file type 0x" << hex << item.p4 << dec << ".";
				return error(ss);
			}

			// validate item
			if ((item.p4 == 0) && (item.p2 != item.p3)) {
				stringstream ss;
				ss << "IDX file lengths do not match at offset 0x" << hex << ((uint32)ifile.tellg() - 0x20) << dec << ".";
				return error(ss);
			}

			// insert item only if there is something to save
			if (item.p2 == 0) continue;
			if (item.p3 == 0) continue;
			itemlist.push_back(item);
		}

		// current BIN file
		uint32 BIN_index = 0;
		uint32 start_ref = 0;

		// construct BIN filename
		STDSTRINGSTREAM bfname;
		bfname << pathname << shrtname << BIN_index;
		bfname << TEXT(".BIN");

		// open BIN file
		ifstream bfile(bfname.str().c_str(), ios::binary);
		if (!bfile) return error("Failed to open BIN file.");

		// create destination directory
		STDSTRINGSTREAM ss;
		ss << pathname << shrtname << BIN_index << TEXT("\\");
		STDSTRING savepath = ss.str();
		CreateDirectory(savepath.c_str(), NULL);

		// save items
		for (size_t i = 0; i < itemlist.size(); i++)
		{
			// change BIN file
			if (i > 0 && itemlist[i].p1 == 0)
			{
				// increment BIN index and close file
				BIN_index++;
				start_ref = i;
				bfile.close();

				// construct BIN filename
				STDSTRINGSTREAM bfname;
				bfname << pathname << shrtname << BIN_index;
				bfname << TEXT(".BIN");

				// open next BIN file
				bfile.open(bfname.str().c_str(), ios::binary);
				if (!bfile) return error("Failed to open BIN file.");

				// create destination directory
				STDSTRINGSTREAM ss;
				ss << pathname << shrtname << BIN_index << TEXT("\\");
				savepath = ss.str();
				CreateDirectory(savepath.c_str(), NULL);
			}

			// construct output filename
			STDSTRINGSTREAM ss;
			ss << savepath << setfill(TEXT('0')) << setw(4) << (i - start_ref) << TEXT(".");
			if (itemlist[i].p4 == 0) ss << L"idxout";
			else if (itemlist[i].p4 == 1) ss << L"idxzrc";
			else {
				STDSTRINGSTREAM ss;
				ss << "Unknown IDX file type 0x" << hex << itemlist[i].p4 << dec << ".";
				return error(ss);
			}

			// display information and ignore small files
			wcout << " FILE " << (i + 1) << " of " << itemlist.size() << ": " << ss.str() << endl;
			if (!(itemlist[i].p3 > SMALL_FILESIZE)) continue;

			// read first four bytes and determine whether or not to save file
			bfile.seekg(itemlist[i].p1);
			if (bfile.fail()) return error("Seek failure.");
			uint32 magic = (mode == ENDIAN_LITTLE ? LE_read_uint32(bfile) : BE_read_uint32(bfile));
			if (bfile.fail() || ExcludeExtension(magic)) continue;

			// create 16 MB buffer and move to bin data
			boost::shared_array<char> data(new char[0x1000000]);
			bfile.seekg(itemlist[i].p1);
			if (bfile.fail()) return error("Seek failure.");

			// keep track of remaining bytes
			uint64 bytes_remaining = itemlist[i].p3;
			if (!(bytes_remaining < LARGE_FILESIZE))
				message("NOTE: This is a large file and might take a while to save.");

			// create output file
			ofstream ofile(ss.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create output file.");

			// transfer data
			while (bytes_remaining) {
				if (bytes_remaining < 0x1000000) {
					// read BIN data
					bfile.read(data.get(), bytes_remaining);
					if (bfile.fail()) return error("Read failure.");
					// save BIN data
					ofile.write(data.get(), bytes_remaining);
					if (ofile.fail()) return error("Write failure.");
					// nothing more to save
					bytes_remaining = 0;
				}
				else {
					// read BIN data
					bfile.read(data.get(), 0x1000000);
					if (bfile.fail()) return error("Read failure.");
					// save BIN data
					ofile.write(data.get(), 0x1000000);
					if (ofile.fail()) return error("Write failure.");
					// more to save
					bytes_remaining -= 0x1000000;
				}
			}
		}

		cout << endl;
		return true;
	}

}
#pragma endregion

#pragma region REGION_ARCHIVE_FUNCTIONS2
namespace DYNASTY_WARRIORS {

	/*
	** BIN FORMAT #1
	**  uint32 0x00001EDE (older games) 0x00102A49 (newer games)
	**  uint32 number of files
	**  uint32 alignment
	**  uint32 unknown 0x01
	**  [uint32, uint32]  [scaled offset, filesize]
	*/
	bool UnpackBIN_A(LPCTSTR filename, endian_mode mode)
	{
		// unpacking messages
		using namespace std;
		message("Unpacking BIN file...");

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// open BIN file
		ifstream ifile(filename, ios::binary);
		if (!ifile) {
			STDSTRINGSTREAM ss;
			ss << L"Failed to open BIN file." << endl;
			ss << filename;
			return error(ss);
		}

		// compute filesize for BIN file
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);

		// read header
		boost::shared_array<char> header(new char[0x10]);
		ifile.read(header.get(), 0x10);
		if (ifile.fail()) return error("Read failure.");

		// binary stream
		binary_stream bs(header, 0x10);
		bs.set_endian_mode(mode);

		// read header
		uint32 headMagic = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");
		uint32 headFiles = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");
		uint32 headScale = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");
		uint32 headTable = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");

		// validate magic
		switch (headMagic) {
		case(0x00001EDE): break;
		case(0x00005460): break; // [PS3] Berserk Musou (2016)
		case(0x00102A49): break;
		case(0x00011E54): break;
		case(0x00077DF9): break; // [PS3] Attack on Titan (2015), [PC] Attack on Titan (2016)
		default: return message("Invalid BIN file header signature. Skipping...");
		}

		// validate header
		if (headFiles >= 0xFFFF) return error("No dynasty warriors game contains this many files.");
		if (headScale != 0x0800) return error("File assets should occur on 0x800 boundaries.");

		// read file table
		uint32 tablesize = headFiles * (headTable == 0x01 ? 0x08 : 0x10);
		boost::shared_array<char> tabledata(new char[tablesize]);
		ifile.read(tabledata.get(), tablesize);
		if (ifile.fail()) return error("Read failure.");

		// binary stream
		binary_stream ts(tabledata, tablesize);
		ts.set_endian_mode(mode);

		// file table data
		struct TABLEENTRY {
			uint64 offset;
			uint32 size1;
			uint32 size2;
		};
		deque<TABLEENTRY> filetable;

		// read file table
		if (headTable == 0x01) {
			for (size_t i = 0; i < headFiles; i++) {
				TABLEENTRY entry;
				entry.offset = ts.read_uint32(); if (ts.fail()) return error("Stream read failure.");
				entry.size1 = ts.read_uint32(); if (ts.fail()) return error("Stream read failure.");
				entry.size2 = 0; // not compressed
				filetable.push_back(entry);
			}
		}
		else if (headTable == 0x00) {
			for (size_t i = 0; i < headFiles; i++) {
				TABLEENTRY entry;
				entry.offset = ts.read_uint64(); if (ts.fail()) return error("Stream read failure.");
				entry.size1 = ts.read_uint32(); if (ts.fail()) return error("Stream read failure.");
				entry.size2 = ts.read_uint32(); if (ts.fail()) return error("Stream read failure.");
				filetable.push_back(entry);
			}
		}
		else
			return error("Unknown file table type.");

		// create output directory
		// sometimes these files are named LINKDATA.A, LINKDATA.B, etc.
		STDSTRING savepath = pathname;
		savepath += STDSTRING(shrtname);
		savepath += TEXT("_");
		savepath += &(GetExtensionFromFilename(filename)[1]);
		savepath += TEXT("\\");
		CreateDirectory(savepath.c_str(), NULL);

		// for each item in the file table
		for (size_t i = 0; i < filetable.size(); i++)
		{
			// move to file position
			uint64 position = filetable[i].offset * headScale;
			uint32 filesize = filetable[i].size1;
			ifile.seekg(position);
			if (ifile.fail()) return error("Seek failure.");

			// ignore small files
			if (filesize < 0x80) continue;

			// create BIN filename
			cout << "Reading file " << (i + 1) << " of " << filetable.size() << "." << endl;
			STDSTRINGSTREAM name;
			name << savepath << setfill(TCHAR('0')) << setw(4) << i;
			if (filetable[i].size2 == 0x00) name << TEXT(".idxout");
			else name << TEXT(".idxout"); // can be idxzrc or idxzip

			// create BIN file
			ofstream ofile(name.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create bin file.");

			// small files
			if (filesize < 0x2000000) // 32 MB
			{
				// read data
				boost::shared_array<char> data(new char[filesize]);
				ifile.read(data.get(), filesize);
				if (ifile.fail()) return error("Read failure.");

				// save data
				ofile.write(data.get(), filesize);
				if (ofile.fail()) return error("Failed to write to bin file.");
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
				while (bytes_left)
				{
					// specify how much to read
					uint32 datasize = buffersize;
					if (bytes_left < buffersize) datasize = bytes_left;

					// read data
					ifile.read(data.get(), datasize);
					if (ifile.fail()) return error("Read failure.");

					// save data
					ofile.write(data.get(), datasize);
					if (ofile.fail()) return error("Failed to write to bin file.");

					// update
					bytes_left -= datasize;
				}
			}
		}

		// finished
		cout << endl;
		return true;
	}

	bool UnpackBIN_B(LPCTSTR filename, endian_mode mode)
	{
		// EXAMPLES:
		//  [PC] SAMURAI WARRIORS 4-II
		//  TYPICALLY USED FOR SMALL DLC FILES
		// HEADER EXAMPLE
		//  01059401 -- magic
		//  00000003 -- number of entries
		//  00000100 -- offset scale
		//  00000000 -- ???
		// ENTRY TABLE EXAMPLE
		//  00000000 00000001 00000008 000000C9 0005621D FFFFFFFF
		//  00000000 00000564 00000009 000000C9 001D937C FFFFFFFF
		//  00000000 000022F8 00000020 00000066 0000000C FFFFFFFF
		// ENTRY
		//  00000000
		//  000022F8 -- offset divided by scale
		//  00000020 -- ???
		//  00000066 -- ??? file type
		//  0000000C -- datasize
		//  FFFFFFFF -- terminator

		// 0100000000000000 12000000 00000000 4CCB0200 FFFFFFFF
		// CD02000000000000 13000000 00000000 9FF91000 FFFFFFFF
		// C713000000000000 1D000000 00000000 05000000 FFFFFFFF
		// C813000000000000 14000000 00000000 1A5E0000 FFFFFFFF

		// unpacking messages
		using namespace std;
		message("Unpacking BIN file...");

		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Could not open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// read magic
		uint32 magic[4] = {
		 LE_read_uint08(ifile),
		 LE_read_uint08(ifile),
		 LE_read_uint08(ifile),
		 LE_read_uint08(ifile)
		};
		if (ifile.fail()) return error("Read failure.");

		// IMPORTANT! ADJUST ENDIAN MODE!
		// from magic we have to determine endian (IT DOESN'T MATTER WHAT CURRENT ENDIAN PASSED IN IS)
		if (magic[0] == 0x01 && magic[1] == 0x94 && magic[2] == 0x05 && magic[3] == 0x01) mode = ENDIAN_LITTLE;
		else if (magic[0] == 0x01 && magic[1] == 0x05 && magic[2] == 0x94 && magic[3] == 0x01) mode = ENDIAN_BIG;
		else return error("This is not a BIN Type-B file.");

		// lambdas for reading
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };
		auto read_uint64 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint64(ifile) : LE_read_uint64(ifile)); };

		// read header
		uint32 n_entries = read_uint32(ifile, mode);
		uint32 scale = read_uint32(ifile, mode);
		uint32 unk01 = read_uint32(ifile, mode); // 0x00
		if (ifile.fail()) return error("Read failure.");

		// read file table
		struct BINENTRY {
			uint64 p01; // offset divided by scalar
			uint32 p02; // ???
			uint32 p03; // datatype
			uint32 p04; // datasize
			uint32 p05; // terminator
		};
		deque<BINENTRY> filetable;
		for (size_t i = 0; i < n_entries; i++) {
			BINENTRY item;
			item.p01 = read_uint64(ifile, mode); if (ifile.fail()) return error("Read failure.");
			item.p02 = read_uint32(ifile, mode); if (ifile.fail()) return error("Read failure.");
			item.p03 = read_uint32(ifile, mode); if (ifile.fail()) return error("Read failure.");
			item.p04 = read_uint32(ifile, mode); if (ifile.fail()) return error("Read failure.");
			item.p05 = read_uint32(ifile, mode); if (ifile.fail()) return error("Read failure.");
			filetable.push_back(item);
		}

		// create output directory
		STDSTRING savepath = pathname;
		savepath += shrtname;
		savepath += TEXT("\\");
		CreateDirectory(savepath.c_str(), NULL);

		// for each item in the file table
		for (size_t i = 0; i < filetable.size(); i++)
		{
			// move to file position
			size_t position = (size_t)(filetable[i].p01 * scale);
			size_t filesize = (size_t)(filetable[i].p04);
			ifile.seekg(position);
			if (ifile.fail()) return error("Seek failure.");

			// ignore small files
			if (filesize < 0x80) continue;

			// create BIN filename
			cout << "Reading file " << (i + 1) << " of " << filetable.size() << "." << endl;
			STDSTRINGSTREAM ss;
			ss << savepath << setfill(TCHAR('0')) << setw(4) << i;
			ss << TEXT(".idxout");

			// create BIN file
			ofstream ofile(ss.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create bin file.");

			// read data
			boost::shared_array<char> data(new char[filesize]);
			ifile.read(data.get(), filesize);
			if (ifile.fail()) return error("Read failure.");

			// save data
			ofile.write(data.get(), filesize);
			if (ofile.fail()) return error("Write failure.");
		}

		return true;
	}

	/*
	** BIN FORMAT #3
	** This format is simply an IDXOUT or IDXZRC file.
	** [PC] Berserk Musou uses this for its DLC files.
	** Just simply rename it.
	*/
	bool UnpackBIN_C(LPCTSTR filename)
	{
		STDSTRING savename = ChangeFileExtension(filename, TEXT("idxout")).get();
		MoveFileEx(filename, savename.c_str(), MOVEFILE_REPLACE_EXISTING);
		return true;
	}

}
#pragma endregion

#pragma region REGION_RECURSIVE_UNPACKING
namespace DYNASTY_WARRIORS {

	bool IDXOUT_PathExtract(LPCTSTR pathname, endian_mode mode)
	{
		// recursive archive processing
		using namespace std;
		uint32 loop_counter = 1;
		for (;;)
		{
			// keep track of how many files were processed
			cout << "ARCHIVE UNPACK LOOP #" << loop_counter << ": " << endl;
			uint32 processed = 0;
			deque<STDSTRING> filelist;

			// process IDXOUT files
			cout << "Processing .IDXOUT files..." << endl;
			BuildFilenameList(filelist, TEXT(".IDXOUT"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!IDXOUT_FileExtract(filelist[i].c_str(), mode)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process IDXZRC files
			cout << "Processing .IDXZRC files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".IDXZRC"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!IDXZRC_FileExtract(filelist[i].c_str(), mode)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process IDXZIP files
			cout << "Processing .IDXZIP files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".IDXZIP"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!IDXZIP_FileExtract(filelist[i].c_str(), mode)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process IDXLPZ files
			cout << "Processing .IDXLPZ files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".IDXLPZ"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!IDXLPZ_FileExtract(filelist[i].c_str(), mode)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process LARC0 files
			cout << "Processing .LARC0 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".LARC0"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC0_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process BARC0 files
			cout << "Processing .BARC0 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".BARC0"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC0_FileExtract(filelist[i].c_str(), ENDIAN_BIG)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process LARC1 files
			cout << "Processing .LARC1 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".LARC1"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC1_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process BARC1 files
			cout << "Processing .BARC1 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".BARC1"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC1_FileExtract(filelist[i].c_str(), ENDIAN_BIG)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process LARC2 files
			cout << "Processing .LARC2 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".LARC2"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC2_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process BARC2 files
			cout << "Processing .BARC2 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".BARC2"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC2_FileExtract(filelist[i].c_str(), ENDIAN_BIG)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process LARC3 files
			cout << "Processing .LARC3 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".LARC3"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC3_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process BARC3 files
			cout << "Processing .BARC3 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".BARC3"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC3_FileExtract(filelist[i].c_str(), ENDIAN_BIG)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process LARC4 files
			cout << "Processing .LARC4 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".LARC4"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC4_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process BARC4 files
			cout << "Processing .BARC4 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".BARC4"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC4_FileExtract(filelist[i].c_str(), ENDIAN_BIG)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process LARC5 files
			cout << "Processing .LARC5 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".LARC5"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC5_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process BARC5 files
			cout << "Processing .BARC5 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".BARC5"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC5_FileExtract(filelist[i].c_str(), ENDIAN_BIG)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process LARC6 files
			cout << "Processing .LARC6 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".LARC6"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC6_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process BARC6 files
			cout << "Processing .BARC6 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".BARC6"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC6_FileExtract(filelist[i].c_str(), ENDIAN_BIG)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process LARC7 files
			cout << "Processing .LARC7 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".LARC7"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC7_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process BARC7 files
			cout << "Processing .BARC7 files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".BARC7"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!ARC7_FileExtract(filelist[i].c_str(), ENDIAN_BIG)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process MDLK files
			cout << "Processing .MDLK files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".MDLK"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!MDLK_FileExtract(filelist[i].c_str(), mode)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// process CSTB files
			cout << "Processing .CSTB files..." << endl;
			filelist.clear();
			BuildFilenameList(filelist, TEXT(".CSTB"), pathname);
			for (size_t i = 0; i < filelist.size(); i++) {
				wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
				if (!CSTB_FileExtract(filelist[i].c_str(), mode)) return false;
			}
			cout << endl;
			processed += filelist.size();

			// finished or not finished
			if (processed == 0) break;
			cout << "Processed a total of " << processed << " files in unpack loop #" << loop_counter << "." << endl;
			loop_counter++;
		}

		return true;
	}

	bool IDXOUT_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// 3096: File size can be 0
		//// delete file given first four bytes (little endian)
		//uint32 magic = LE_read_uint32(ifile);
		//if (ifile.fail() || ExcludeExtension(magic)) {
		//	ifile.close();
		//	DeleteFile(filename);
		//	return true;
		//}
		//
		//// delete file given first four bytes (big endian)
		//reverse_byte_order(&magic);
		//if (ifile.fail() || ExcludeExtension(magic)) {
		//	ifile.close();
		//	DeleteFile(filename);
		//	return true;
		//}

		// close file
		ifile.close();

		// filesize is zero (delete or rename)
		if (!filesize) {
			if (DELETE_SMALL_FILES)
				DeleteFile(filename);
			else {
				STDSTRING savename = ChangeFileExtension(filename, TEXT("unknown")).get();
				MoveFileEx(filename, savename.c_str(), MOVEFILE_REPLACE_EXISTING);
			}
			return true;
		}

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			if (!DeleteFile(filename)) return error("Failed to delete IDXOUT file.");
			return true;
		}

		// rename file based on extension
		TCHAR extension[32];
		if (FileExtensionHeuristic(filename, extension)) {
			STDSTRING newname = ChangeFileExtension(filename, extension).get();
			if (!MoveFileEx(filename, newname.c_str(), MOVEFILE_REPLACE_EXISTING)) {
				stringstream ss;
				ss << "Failed to rename IDXOUT file. GetLastError() code is 0x" << hex << GetLastError() << dec << ".";
				return error(ss);
			}
		}

		// success
		return true;
	}

}
#pragma endregion

#pragma region REGION_RECURSIVE_ARCHIVE_UNPACKING
namespace DYNASTY_WARRIORS {

	bool ARC0_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Could not open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// read data
		boost::shared_array<char> filedata(new char[filesize]);
		ifile.read(filedata.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// create binary stream
		binary_stream bs(filedata, filesize);
		bs.set_endian_mode(mode);

		// read header
		uint32 headMagic = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");
		uint32 headFiles = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");
		uint32 headScale = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");
		uint32 headUnk01 = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");

		// validate header
		if (headMagic != 0x1EDE) return error("Invalid header signature.");
		if (headFiles >= 0xFFFF) return error("No Dynasty Warriors game contains this many files.");
		if (headScale % 0x10) return error("File assets should occur on 0x10-byte boundaries.");

		// read file table
		deque<pair<uint32, uint32>> filetable;
		for (size_t i = 0; i < headFiles; i++) {
			pair<uint32, uint32> p;
			p.first = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");
			p.second = bs.read_uint32(); if (bs.fail()) return error("Stream read failure.");
			filetable.push_back(p);
		}

		// create save directory
		STDSTRINGSTREAM ss;
		ss << pathname << shrtname << TEXT("\\");
		STDSTRING savepath = ss.str();
		CreateDirectory(savepath.c_str(), NULL);

		// for each item in the file table
		for (size_t i = 0; i < filetable.size(); i++)
		{
			// move to file position
			size_t position = filetable[i].first * headScale;
			size_t filesize = filetable[i].second;
			ifile.seekg(position);
			if (ifile.fail()) return error("Seek failure.");

			// ignore small files
			if (filesize < 0x80) continue;

			// create BIN filename
			cout << "Reading file " << (i + 1) << " of " << filetable.size() << "." << endl;
			STDSTRINGSTREAM name;
			name << savepath << setfill(TEXT('0')) << setw(4) << i << TEXT(".idxout");

			// create BIN file
			ofstream ofile(name.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create bin file.");

			// small files
			if (filesize < 0x2000000) // 32 MB
			{
				// save data
				ofile.write(bs.c_pos(), filesize);
				if (ofile.fail()) return error("Failed to write to bin file.");
			}
			// large files
			else
			{
				// create buffer
				cout << " NOTE: This is a large file and might take a while to save." << endl;
				uint32 buffersize = 0x1000000; // 16 MB

				// read and copy data
				uint32 bytes_left = filesize;
				while (bytes_left)
				{
					// specify how much to read
					uint32 datasize = buffersize;
					if (bytes_left < buffersize) datasize = bytes_left;

					// save data
					ofile.write(bs.c_pos(), datasize);
					if (ofile.fail()) return error("Failed to write to bin file.");

					// update
					bs.move(datasize);
					bytes_left -= datasize;
				}
			}
		}

		// cleanup ARC file
		ifile.close();
		DeleteFile(filename);

		return true;
	}

	bool ARC1_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #1
		// HEADER = MAGIC, #FILES, SCALE, TABLE TYPE
		// [ENTRIES]
		// [DATA]
		//

		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Could not open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// read magic
		uint32 magic = LE_read_uint32(ifile);
		if (ifile.fail()) return error("Read failure.");

		// IMPORTANT! ADJUST ENDIAN MODE!
		// from magic we have to determine endian (IT DOESN'T MATTER WHAT CURRENT ENDIAN PASSED IN IS)
		if (magic == 0x102A49) mode = ENDIAN_LITTLE;
		else if (magic == 0x492A1000ul) mode = ENDIAN_BIG;
		else return error("This is not an ARC Type-1 file.");

		// read header
		uint32 files = (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile));
		uint32 scale = (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile));
		uint32 table = (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile));
		if (ifile.fail()) return error("Read failure.");

		// validate header
		if (files == 0) return false; // nothing to do
		if (!(files < 0xFFFF)) return error("Too many files.", __LINE__);
		if (scale % 0x800) return error("Expecting 0x800.", __LINE__);
		if (!(table == 0x00 || table == 0x01)) return error("Invalid table type.", __LINE__);

		// read file table
		uint32 tablesize = files * 0x10;
		boost::shared_array<char> tabledata(new char[tablesize]);
		ifile.read(tabledata.get(), tablesize);
		if (ifile.fail()) return error("Read failure.");

		// binary stream
		binary_stream bs(tabledata, tablesize);
		bs.set_endian_mode(mode);

		// file table data
		struct TABLEENTRY {
			uint64 offset;
			uint32 size1;
			uint32 size2;
		};
		deque<TABLEENTRY> filetable;

		// read file table
		if (table == 0x01) {
			for (size_t i = 0; i < files; i++) {
				TABLEENTRY entry;
				entry.offset = bs.read_uint32();
				entry.size1 = bs.read_uint32();
				entry.size2 = 0; // not compressed
				if (bs.fail()) return error("Stream read failure.");
				filetable.push_back(entry);
			}
		}
		else if (table == 0x00) {
			for (size_t i = 0; i < files; i++) {
				TABLEENTRY entry;
				entry.offset = bs.read_uint64();
				entry.size1 = bs.read_uint32();
				entry.size2 = bs.read_uint32();
				if (bs.fail()) return error("Stream read failure.");
				filetable.push_back(entry);
			}
		}
		else
			return error("Invalid table type.");

		// create output directory
		STDSTRING savepath = pathname;
		savepath += STDSTRING(shrtname);
		savepath += TEXT("\\");
		CreateDirectory(savepath.c_str(), NULL);

		// for each item in the file table
		for (size_t i = 0; i < filetable.size(); i++)
		{
			// move to file position
			uint64 position = filetable[i].offset * scale;
			uint32 filesize = filetable[i].size1;
			ifile.seekg(position);
			if (ifile.fail()) return error("Seek failure.");

			// ignore small files
			if (filesize < 0x80) continue;

			// create BIN filename
			STDSTRINGSTREAM name;
			name << savepath << setfill(TCHAR('0')) << setw(4) << i;
			if (filetable[i].size2 == 0x00) name << TEXT(".idxout");
			else name << TEXT(".idxout"); // can be idxzrc or idxzip

			// create BIN file
			ofstream ofile(name.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create bin file.");

			// small files
			if (filesize < 0x2000000) // 32 MB
			{
				// read data
				boost::shared_array<char> data(new char[filesize]);
				ifile.read(data.get(), filesize);
				if (ifile.fail()) return error("Read failure.");

				// save data
				ofile.write(data.get(), filesize);
				if (ofile.fail()) return error("Failed to write to bin file.");
			}
			// large files
			else
			{
				// create buffer
				uint32 buffersize = 0x1000000; // 16 MB
				boost::shared_array<char> data(new char[buffersize]);

				// read and copy data
				uint32 bytes_left = filesize;
				while (bytes_left)
				{
					// specify how much to read
					uint32 datasize = buffersize;
					if (bytes_left < buffersize) datasize = bytes_left;

					// read data
					ifile.read(data.get(), datasize);
					if (ifile.fail()) return error("Read failure.");

					// save data
					ofile.write(data.get(), datasize);
					if (ofile.fail()) return error("Failed to write to bin file.");

					// update
					bytes_left -= datasize;
				}
			}
		}

		// cleanup ARC file
		ifile.close();
		DeleteFile(filename);

		// finished
		return true;
	}

	bool ARC2_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #2
		// #FILES, [OFFSET/LENGTH], [DATA]
		//

		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();
		STDSTRING extension = GetExtensionFromFilename(filename).get();

		// read data
		boost::shared_array<char> filedata(new char[filesize]);
		ifile.read(filedata.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// create binary stream
		binary_stream bs(filedata, filesize);
		bs.set_endian_mode(mode);

		// read number of files
		uint32 n_files = bs.read_uint32();
		if (ifile.fail()) return error("Read failure.");

		// read offset/length table
		struct ARC1ITEM { uint32 offset; uint32 length; };
		typedef pair<uint32, ARC1ITEM> pair_t;
		deque<pair_t> entries;
		for (uint32 i = 0; i < n_files; i++)
		{
			// read pair
			uint32 offset = bs.read_uint32();
			uint32 length = bs.read_uint32();
			if (bs.fail()) return false;

			// 3096: what the hell is this check? It's just if length == 0 dude.
			//       also I'm gonna need to allow length to be 0 thank you

			//// ignore entry
			//if (offset == 0 && length == 0)
			//	continue;

			//// ignore entry
			//if (offset != 0 && length == 0)
			//	continue;

			// invalid entry
			if (offset == 0 && length != 0)
				return error("Invalid ARC1 entry.");

			// insert entry
			ARC1ITEM item;
			item.offset = offset;
			item.length = length;
			entries.push_back(pair_t(i, item));
		}

		// don't even bother saving anything
		if (DELETE_SMALL_FILES) {
			// compute number of small files
			uint32 smallcount = 0;
			for (uint32 i = 0; i < entries.size(); i++) {
				uint32 length = entries[i].second.length;
				if (length < SMALL_FILESIZE) smallcount++;
			}
			// delete file and move on
			if (smallcount == entries.size()) {
				ifile.close();
				DeleteFile(filename);
				return true;
			}
		}

		// create save directory
		STDSTRING savepath = pathname;
		savepath += shrtname;
		savepath += TEXT("-");
		savepath += extension.substr(1);
		savepath += TEXT("\\");
		CreateDirectory(savepath.c_str(), NULL);

		// save items
		for (uint32 i = 0; i < entries.size(); i++)
		{
			// skip small files
			uint32 offset = entries[i].second.offset;
			uint32 length = entries[i].second.length;
			if (DELETE_SMALL_FILES && (length < SMALL_FILESIZE))
				continue;

			// move to offset
			bs.seek(offset);
			if (bs.fail()) return error("Seek failure.");

			// skip saving
			if (ExcludeExtension(bs.peek_uint32())) continue;

			// save data
			STDSTRINGSTREAM ss;
			ss << savepath << setfill(TCHAR('0')) << setw(3) << entries[i].first << TEXT(".idxout");
			ofstream ofile(ss.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create IDXOUT file.");
			ofile.write(bs.c_pos(), length);
			if (ofile.fail()) return error("Write failure.");
		}

		// cleanup ARC file
		ifile.close();
		DeleteFile(filename);

		return true;
	}

	bool ARC3_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #3
		// [OFFSET/LENGTH], [DATA]
		//

		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// read data
		boost::shared_array<char> filedata(new char[filesize]);
		ifile.read(filedata.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// create binary stream
		binary_stream bs(filedata, filesize);
		bs.set_endian_mode(mode);

		// read 1st pair
		uint32 curr_offset = bs.read_uint32();
		uint32 curr_length = bs.read_uint32();
		uint32 curr_finish = curr_offset + curr_length;
		if (ifile.fail()) return error("Read failure.");

		// validate 1st pair
		if (filesize < curr_offset) return error("Invalid offset.");
		if (filesize < curr_length) return error("Invalid length.");
		if (filesize < curr_finish) return error("Invalid offset-length pair.");

		// insert 1st pair
		struct ARC2ITEM { uint32 offset; uint32 length; };
		ARC2ITEM item = { curr_offset, curr_length };
		deque<ARC2ITEM> entries;
		entries.push_back(item);

		// keep loading pairs
		for (;;)
		{
			// load previous pair
			uint32 prev_offset = entries[entries.size() - 1].offset;
			uint32 prev_length = entries[entries.size() - 1].length;
			uint32 prev_finish = prev_offset + prev_length;

			// load current pair
			uint32 curr_offset = bs.read_uint32();
			uint32 curr_length = bs.read_uint32();
			uint32 curr_finish = curr_offset + curr_length;
			if (ifile.fail()) break;

			// validate curr pair
			if (filesize < curr_offset) break;
			if (filesize < curr_length) break;
			if (filesize < curr_finish) break;

			// offset and length are non-zero
			if (curr_offset != 0 && curr_length != 0)
			{
				// validate order between pairs
				if (curr_offset < prev_offset) break;
				if (curr_offset < prev_finish) break;

				// can't have too must spacing between data
				uint32 spacing = curr_offset - prev_finish;
				if (spacing > 0x0F) break;

				// insert
				ARC2ITEM item = { curr_offset, curr_length };
				entries.push_back(item);
			}
			// offset and length are zero
			else if (curr_offset == 0 && curr_length == 0)
			{
				// continue testing
				continue;
			}
			// only length is zero
			else if (curr_length == 0)
			{
				// curr offset must still be valid
				if (curr_offset < prev_offset) break;
				if (curr_offset < prev_finish) break;

				// continue testing
				continue;
			}
			// only offset is zero
			else
				break;
		}

		// validate number of items
		uint32 items = (uint32)entries.size();
		if ((items < 1) || (items > 0xFFFF)) return false;

		// create save directory
		STDSTRINGSTREAM ss;
		ss << pathname << shrtname << TEXT("\\");
		STDSTRING savepath = ss.str();
		CreateDirectoryW(savepath.c_str(), NULL);

		// save items
		for (uint32 i = 0; i < entries.size(); i++)
		{
			// skip
			if (entries[i].offset == 0x00) continue;
			if (entries[i].length == 0x00) continue;
			if (entries[i].length <= 0xFF) continue; // skip small files

			// move to offset
			bs.seek(entries[i].offset);
			if (bs.fail()) return error("Stream seek failure.");

			// skip saving
			if (ExcludeExtension(bs.peek_uint32())) continue;

			// save data
			STDSTRINGSTREAM ss;
			ss << savepath << setfill(TCHAR('0')) << setw(3) << i << TEXT(".idxout");
			ofstream ofile(ss.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create file.");
			ofile.write(bs.c_pos(), entries[i].length);
			if (ofile.fail()) return error("Write failure.");
		}

		// cleanup ARC file
		ifile.close();
		DeleteFile(filename);

		return true;
	}

	bool ARC4_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #4
		// #FILES, [LENGTH], [DATA]
		// DATA IS ALWAYS 0x10-BYTE ALIGNED
		//

		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// read data
		boost::shared_array<char> filedata(new char[filesize]);
		ifile.read(filedata.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// close file
		ifile.close();

		// create binary stream
		binary_stream bs(filedata, filesize);
		bs.set_endian_mode(mode);

		// read number of files
		uint32 n_files = bs.read_uint32();
		if (bs.fail()) return error("Read failure.");

		// read length table
		deque<uint32> entries;
		for (uint32 i = 0; i < n_files; i++) {
			uint32 item = bs.read_uint32();
			if (bs.fail()) return false;
			entries.push_back(item);
		}

		// don't even bother creating a folder or extracting anything
		if (DELETE_SMALL_FILES) {
			uint32 smallcount = 0;
			for (uint32 i = 0; i < n_files; i++) if (entries[i] < SMALL_FILESIZE) smallcount++;
			if (smallcount == n_files) {
				ifile.close();
				DeleteFile(filename);
				return true;
			}
		}

		// create save directory
		STDSTRINGSTREAM ss;
		ss << pathname << shrtname << TEXT("\\");
		STDSTRING savepath = ss.str();
		CreateDirectory(savepath.c_str(), NULL);

		// save items
		for (uint32 i = 0; i < entries.size(); i++)
		{
			// obtain current position
			uint64 position = (uint64)bs.tell();
			if (bs.fail()) return error("Stream tell failure.");

			// move to next 0x10-byte position
			bs.seek(((position + 0x0F) & (~0x0F)));
			if (bs.fail()) return error("Stream seek failure.");

			// skip empty files
			if (entries[i] == 0x00) continue;

			// skip small files
			if (DELETE_SMALL_FILES && (entries[i] < SMALL_FILESIZE)) {
				bs.move(entries[i]);
				if (bs.fail()) return error("Stream seek failure.");
				continue;
			}

			// skip saving
			if (ExcludeExtension(bs.peek_uint32())) continue;

			// save data
			STDSTRINGSTREAM ss;
			ss << savepath << setfill(TEXT('0')) << setw(3) << i << TEXT(".idxout");
			ofstream ofile(ss.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create file.");
			ofile.write(bs.c_pos(), entries[i]);
			if (ofile.fail()) return error("Write failure.");

			// move to new position
			bs.move(entries[i]);
			if (bs.fail()) return error("Stream seek failure.");
		}

		// cleanup ARC file
		ifile.close();
		DeleteFile(filename);

		return true;
	}

	bool ARC5_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		//
		// ARCHIVE TYPE #5
		// #FILES, [OFFSET], [DATA]
		//

		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// read data
		boost::shared_array<char> filedata(new char[filesize]);
		ifile.read(filedata.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// create binary stream
		binary_stream bs(filedata, filesize);
		bs.set_endian_mode(mode);

		// read number of files
		uint32 n_files = bs.read_uint32();
		if (bs.fail()) return error("Stream read failure.");

		// read offsets table
		typedef pair<uint32, uint32> pair_t;
		deque<pair_t> offsets;
		for (uint32 i = 0; i < n_files; i++) {
			uint32 item = bs.read_uint32();
			if (bs.fail()) return false;
			if (item) offsets.push_back(pair_t(i, item));
		}

		// nothing to save
		if (!offsets.size()) return true;

		// don't even bother creating a folder or extracting anything
		if (DELETE_SMALL_FILES)
		{
			// compute number of small files
			uint32 smallcount = 0;
			for (uint32 i = 0; i < offsets.size(); i++) {
				uint32 prev_offset = offsets[i].second;
				uint32 curr_offset = ((i == (offsets.size() - 1)) ? filesize : offsets[i + 1].second);
				uint32 datasize = curr_offset - prev_offset;
				if (datasize < SMALL_FILESIZE) smallcount++;
			}
			// test for all small files
			if (smallcount == offsets.size()) {
				ifile.close();
				DeleteFile(filename);
				return true;
			}
		}

		// create save directory
		STDSTRINGSTREAM ss;
		ss << pathname << shrtname << TEXT("\\");
		STDSTRING savepath = ss.str();
		CreateDirectory(savepath.c_str(), NULL);

		// save items
		uint32 last_index = offsets.size() - 1;
		for (uint32 i = 0; i < offsets.size(); i++)
		{
			// move to offset
			bs.seek(offsets[i].second);
			if (bs.fail()) return error("Stream seek failure.");

			// compute size of data
			uint32 size = 0;
			if (i == last_index) size = filesize - offsets[i].second;
			else size = offsets[i + 1].second - offsets[i].second;

			// skip small files
			if (DELETE_SMALL_FILES && (size < SMALL_FILESIZE))
				continue;

			// skip saving
			if (ExcludeExtension(bs.peek_uint32())) continue;

			// save data
			STDSTRINGSTREAM ss;
			ss << savepath << setfill(TEXT('0')) << setw(3) << i << TEXT(".idxout");
			ofstream ofile(ss.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create file.");
			ofile.write(bs.c_pos(), size);
			if (ofile.fail()) return error("Write failure.");
		}

		// cleanup ARC file
		ifile.close();
		DeleteFile(filename);

		return true;
	}

	bool ARC6_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		// same format just not aligned
		return ARC5_FileExtract(filename, mode);
	}

	bool ARC7_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		// ARCHIVE TYPE #7 - DLC BIN FILES, [PC] ONE PIECE PIRATE WARRIORS 3
		// HEADER = 0x10 BYTES
		//  uint32 0x64 is this always 0x64 though?
		//  uint32 #FILES
		//  uint32 0x00
		//  uint32 0x00
		// [uint32 OFFSETS]
		// padding
		// [uint32 FILESIZES]
		// [DATA]

		// ARCHIVE TYPE #7 - DLC BIN FILES, [PC] ONE PIECE PIRATE WARRIORS 3
		// HEADER = 0x10 BYTES
		//  uint32 0x64 is this always 0x64 though?
		//  uint32 #FILES
		//  uint32 0x00
		//  uint32 0x00
		// [uint32 OFFSETS]
		// padding
		// [uint32 FILESIZES]
		// [DATA]

		// open file
		using namespace std;
		ifstream ifile(filename, ios::binary);
		if (!ifile) return error("Failed to open file.", __LINE__);

		// compute filesize
		uint32 filesize = 0x00;
		ifile.seekg(0, ios::end);
		filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.", __LINE__);

		// filesize can be small
		if (DELETE_SMALL_FILES && (filesize < SMALL_FILESIZE)) {
			ifile.close();
			DeleteFile(filename);
			return true;
		}

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// read magic
		uint32 magic = LE_read_uint32(ifile);
		uint32 files = LE_read_uint32(ifile);
		uint32 zero1 = LE_read_uint32(ifile);
		uint32 zero2 = LE_read_uint32(ifile);
		if (ifile.fail()) return error("Read failure.", __LINE__);

		// override suggested endian mode
		if (magic == 0x64) mode = ENDIAN_LITTLE;
		else if (magic == 0x64000000ul) mode = ENDIAN_BIG;
		else return error("Not an ARC7 file.", __LINE__);

		// validate
		if (zero1) return error("Expecting zeroes.", __LINE__);
		if (zero2) return error("Expecting zeroes.", __LINE__);

		// reverse files
		if (mode == ENDIAN_BIG) reverse_byte_order(&files);
		if (files == 0) return true;
		if (files > 0xFFFF) return error("Unexpected number of files.", __LINE__);

		// compute size of offset and sizes sections
		uint32 offsets_size = sizeof(uint32) * files;
		offsets_size = ((offsets_size + 0x7F) & (~0x7F));
		uint32 n_zeroes = offsets_size / 4 - files;

		// lambda for reading values
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };

		// read offsets
		vector<uint32> offsets(files);
		for (uint32 i = 0; i < files; i++) {
			offsets[i] = read_uint32(ifile, mode);
			if (offsets[i] > filesize) return error("A file in this archive goes beyond EOF boundary.", __LINE__);
			if (i && offsets[i] < offsets[i - 1]) return error("Offsets are out of order.", __LINE__);
		}
		// read zeroes
		for (uint32 i = 0; i < n_zeroes; i++) {
			auto z = read_uint32(ifile, mode);
			if (z) return error("Expecting zeroes.", __LINE__);
		}

		// read sizes
		vector<uint32> sizes(files);
		for (uint32 i = 0; i < files; i++) {
			sizes[i] = read_uint32(ifile, mode);
			if (sizes[i] > filesize) return error("A file in this archive goes beyond EOF boundary.", __LINE__);
			if ((offsets[i] + sizes[i]) > filesize) return error("A file in this archive goes beyond EOF boundary.", __LINE__);
		}
		// read zeroes
		for (uint32 i = 0; i < n_zeroes; i++) {
			auto z = read_uint32(ifile, mode);
			if (z) return error("Expecting zeroes.", __LINE__);
		}

		// check final offset/size
		uint32 offset1 = offsets.back();
		uint32 offset2 = offset1 + sizes.back();
		uint32 difference = filesize - offset2;
		if (difference > 0x10) return error("This is probably not an ARC7 archive file. Skipping.", __LINE__);

		// create save directory
		STDSTRINGSTREAM ss;
		ss << pathname << shrtname << TEXT("\\");
		STDSTRING savepath = ss.str();
		CreateDirectory(savepath.c_str(), NULL);

		// process files
		for (uint32 i = 0; i < files; i++)
		{
			// move to offset
			ifile.seekg(offsets[i]);
			if (ifile.fail()) return error("Stream seek failure.");

			// skip small files
			if (DELETE_SMALL_FILES && (sizes[i] < SMALL_FILESIZE))
				continue;

			// read data
			boost::shared_array<char> data(new char[sizes[i]]);
			ifile.read(data.get(), sizes[i]);
			if (ifile.fail()) return error("Read failure.");

			// skip saving
			binary_stream bs(data, sizes[i]);
			bs.set_endian_mode(mode);
			if (ExcludeExtension(bs.peek_uint32())) continue;

			// save data
			STDSTRINGSTREAM ss;
			ss << savepath << setfill(TEXT('0')) << setw(3) << i << TEXT(".idxout");
			ofstream ofile(ss.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create file.");
			ofile.write(data.get(), sizes[i]);
			if (ofile.fail()) return error("Write failure.");
		}

		// cleanup ARC file
		ifile.close();
		DeleteFile(filename);
		return true;
	}

	bool MDLK_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// read data
		boost::shared_array<char> filedata(new char[filesize]);
		ifile.read(filedata.get(), filesize);
		if (ifile.fail()) return error("Read failure.");

		// close file
		ifile.close();

		// create binary stream
		binary_stream bs(filedata, filesize);
		bs.set_endian_mode(mode);

		// read header
		uint32 h01 = bs.read_uint32();
		uint32 h02 = bs.read_uint32();
		uint32 h03 = bs.read_uint32(); // number of files
		uint32 h04 = bs.read_uint32();
		if (bs.fail()) return error("Read failure.");

		// nothing to do
		// 3096: just do it anyways so you don't get stuck in a loop when small files are included
		// if (h03 == 0x00) return true;

		// validate MDLK
		switch (h01) {
		case(0x4D444C4B): break; // same order, no matter what endian
		case(0x4B4C444D): break; // same order, no matter what endian
		default: return error("Not an MDLK file.");
		}
		// validate version
		switch (h02) {
		case(0x30303031): break; // same order, no matter what endian
		case(0x30363030): break; // same order, no matter what endian
		case(0x31303030): break; // same order, no matter what endian
		case(0x30303630): break; // same order, no matter what endian
		default: return error("Incorrect MDLK version.");
		}
		// validate ???
		switch (h04) {
		case(0x48490000): break; // same order, no matter what endian
		case(0x4D490000): break; // same order, no matter what endian
		case(0x50414444): break; // same order, no matter what endian
		case(0x00004948): break; // same order, no matter what endian
		case(0x0000494D): break; // same order, no matter what endian
		case(0x44444150): break; // same order, no matter what endian
		// don't be so strict here since some games use different strings
		// default : return error("Invalid MDLK file.");
		}

		// create save directory
		STDSTRINGSTREAM ss;
		ss << pathname << shrtname << TEXT("_MDLK") << TEXT("\\");
		STDSTRING savepath = ss.str();
		CreateDirectory(savepath.c_str(), NULL);

		// save items
		for (uint32 i = 0; i < h03; i++)
		{
			// save position
			uint64 position = (uint64)bs.tell();
			if (bs.fail()) return error("Stream tell failure.");

			// read chunk ID
			uint32 p01 = bs.read_uint32();
			if (bs.fail()) return error("Stream read failure.");

			// variables
			uint32 datasize = 0;
			STDSTRING extension = TEXT(".unknown");

			// GIM_
			if (p01 == 0x47314D5Ful)
			{
				// seek data size
				bs.move(4);
				if (bs.fail()) return error("Stream seek failure.");

				// read data size
				extension = TEXT("g1m");
				datasize = bs.read_uint32();
				if (bs.fail()) return error("Stream read failure.");
			}
			// G1CO
			else if (p01 == 0x4731434Ful)
			{
				// seek data size
				bs.move(8);
				if (bs.fail()) return error("Stream seek failure.");

				// read data size
				extension = TEXT("g1co");
				datasize = bs.read_uint32();
				if (bs.fail()) return error("Stream read failure.");
			}
			// unknown
			else {
				stringstream ss;
				ss << "Unknown file type 0x" << hex << p01 << dec << " at offset 0x" << hex << position << dec << " in MDLK archive." << endl;
				return error(ss);
			}

			// validate data size
			if (!datasize) return error("Invalid data size.");
			if (!extension.length()) return error("Invalid filename extension.");

			// move back to position
			bs.seek(static_cast<size_t>(position));
			if (bs.fail()) return error("Stream seek failure.");

			// save data
			STDSTRINGSTREAM ss;
			ss << savepath << setfill(TCHAR('0')) << setw(3) << i << TEXT(".") << extension;
			ofstream ofile(ss.str().c_str(), ios::binary);
			if (!ofile) return error("Failed to create file.");
			ofile.write(bs.c_pos(), datasize);
			if (ofile.fail()) return error("Write failure.");
		}

		// cleanup MDLK file
		ifile.close();
		DeleteFile(filename);

		return true;
	}

	bool CSTB_FileExtract(LPCTSTR filename, endian_mode mode)
	{
		// open file
		using namespace std;
		ifstream ifile;
		ifile.open(filename, ios::binary);
		if (!ifile) return error("Failed to open file.");

		// compute filesize
		ifile.seekg(0, ios::end);
		if (ifile.fail()) return error("Seek failure.");
		uint32 filesize = (uint32)ifile.tellg();
		ifile.seekg(0, ios::beg);
		if (ifile.fail()) return error("Seek failure.");

		// filename properties
		STDSTRING pathname = GetPathnameFromFilename(filename).get();
		STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

		// read magic
		uint08 magic[4] = {
		 LE_read_uint08(ifile),
		 LE_read_uint08(ifile),
		 LE_read_uint08(ifile),
		 LE_read_uint08(ifile)
		};

		// set endian mode no matter mode what was passed in
		if (magic[0] == 0x42 && magic[1] == 0x54 && magic[2] == 0x53 && magic[3] == 0x43)
			mode = ENDIAN_LITTLE;
		else if (magic[0] == 0x43 && magic[1] == 0x53 && magic[2] == 0x54 && magic[3] == 0x42)
			mode = ENDIAN_BIG;
		else
			return error("Not a CSTB file.");

		// read header
		auto read_uint32 = [](std::ifstream& ifile, endian_mode mode) { return (mode == ENDIAN_BIG ? BE_read_uint32(ifile) : LE_read_uint32(ifile)); };
		uint32 total_filesize = read_uint32(ifile, mode); // filesize
		uint32 n_files = read_uint32(ifile, mode); // number of files
		if (ifile.fail()) return error("Read failure.");

		// read offsets
		deque<uint32> offsets;
		for (uint32 i = 0; i < n_files; i++) {
			offsets.push_back(read_uint32(ifile, mode));
			if (ifile.fail()) return error("Read failure.");
		}

		// // create save directory
		// STDSTRINGSTREAM ss;
		// ss << pathname << shrtname << TEXT("_MDLK") << TEXT("\\");
		// STDSTRING savepath = ss.str();
		// CreateDirectory(savepath.c_str(), NULL);

		// create output file
		STDSTRINGSTREAM ss;
		ss << pathname << shrtname << TEXT(".idxout");
		ofstream ofile(ss.str().c_str(), ios::binary);
		if (!ofile) return error("Failed to create file.");

		// decompress data
		for (uint32 i = 0; i < n_files; i++)
		{
			// move to offset
			ifile.seekg(offsets[i]);
			if (ifile.fail()) return error("Seek failure.");

			// read datasize
			// NOTE: datasize can be large, so we must use the same endian that the header uses.
			uint32 datasize = read_uint32(ifile, mode);
			if (ifile.fail()) return error("Seek failure.");
			if (!datasize) continue;

			// read data
			boost::shared_array<char> data(new char[datasize]);
			ifile.read(data.get(), datasize);
			if (ifile.fail()) return error("Read failure.");

			// decompress and save
			// use 0 instead of -15 since these zlib chunks start with 0x78DA
			bool status = InflateZLIB(data.get(), datasize, ofile, 0);
			if (!status) return error("ZLIB failure.");
		}

		// cleanup CSTB file
		ifile.close();
		DeleteFile(filename);

		// success
		return true;
	}

}
#pragma endregion