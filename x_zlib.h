#ifndef __XENTAX_ZLIB_H
#define __XENTAX_ZLIB_H

bool isZLIB(uint08 b1, uint08 b2);
bool isZLIB(std::ifstream& ifile, size_t n, int windowBits = -15);

bool CompressZLIB(std::ifstream& ifile, std::ofstream& ofile);
bool CompressZLIB(std::ifstream& ifile, std::ofstream& ofile, int level);

bool DecompressZLIB(std::ifstream& ifile, std::ofstream& ofile);
bool DecompressZLIB(std::ifstream& ifile, std::ofstream& ofile, int windowBits);
bool DecompressZLIB(std::ifstream& ifile, size_t n, std::ofstream& ofile, int windowBits);

struct ZLIBINFO {
 uint32 deflatedBytes; // deflated size
 uint32 inflatedBytes; // inflated size
 std::string filename; // output file
};
bool DecompressZLIB(std::ifstream& ifile, const std::deque<ZLIBINFO>& zinfo, int windowBits);

bool InflateZLIB(std::ifstream& ifile, std::ofstream& ofile, int windowBits);

struct ZLIBINFO2 {
 uint32 deflatedBytes; // deflated size
 uint32 inflatedBytes; // inflated size
 uint64 offset; // offset to data
};
bool InflateZLIB(std::ifstream& ifile, const std::deque<ZLIBINFO2>& zinfo, int windowBits, std::ofstream& ofile);

// inflate and deflate
// from memory to output file
bool DeflateZLIB(const char* data, uint32 size, std::ofstream& ofile);
bool InflateZLIB(const char* data, uint32 size, std::ofstream& ofile, int windowBits = 0);

#endif
