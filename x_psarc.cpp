#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_psarc.h"

/*

bool ExtractPSARC(const char* filename)
{
 // header (0x30 bytes)
 // TOC (variable bytes)

 // open file
 using namespace std;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Could not open PSARC file.");

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(magic != 0x50534152) return error("Invalid PSARC file.");

 // read version (major)
 uint16 v_major = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(v_major != 1) return error("Invalid major PSARC version.");

 // read version (minor)
 uint16 v_minor = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read compression
 uint32 compression = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(compression != 0x7A6C6962) return error("Expecting zlib.");

 // read TOC size (including header)
 uint32 TOC_size = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read TOC entry size (0x1E)
 uint32 TOC_entry_size = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(TOC_entry_size != 0x1E) return error("Unexpected PSARC TOC entry size.");

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(n_files == 0 || n_files > 0xFFFF) return error("Unexpected number of PSARC files.");

 // read block size
 uint32 blocksize = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(!blocksize) return error("Invalid PSARC block size.");

 // read flags
 uint32 flags = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 struct PSARCENTRY {
  char nameDigest[16];
  uint32 blockListStart; // 0, 1, 2, etc.
  uint64 originalSize; // 40-bit (5-byte)
  uint64 startOffset; // 40-bit (5-byte)
 };
 deque<PSARCENTRY> entrylist;

 // read entries
 for(uint32 i = 0; i < n_files; i++)
    {
     // read MD5 name digest
     PSARCENTRY entry;
     ifile.read(&entry.nameDigest[0], 16);
     if(ifile.fail()) return error("Read failure.");

     // read entry index
     entry.blockListStart = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read 5-byte unsigned integer
     char temp[8];
     memset(&temp[0], 0, 8);
     ifile.read(&temp[3], 5);
     if(ifile.fail()) return error("Read failure.");
     entry.originalSize = *reinterpret_cast<uint64*>(&temp[0]);
     reverse_byte_order(&entry.originalSize);

     // read 5-byte unsigned integer
     memset(&temp[0], 0, 8);
     ifile.read(&temp[3], 5);
     if(ifile.fail()) return error("Read failure.");
     entry.startOffset = *reinterpret_cast<uint64*>(&temp[0]);
     reverse_byte_order(&entry.startOffset);

     // insert entry
     entrylist.push_back(entry);

     // display data
     cout << "ENTRY " << i << endl;
     cout << " blockListStart = 0x" << hex << entry.blockListStart << dec << endl;
     cout << " originalSize = 0x" << hex << entry.originalSize << dec << endl;
     cout << " startOffset = 0x" << hex << entry.startOffset << dec << endl;
    }

 // read entries
 for(uint32 i = 0; i < n_files; i++)
    {
     string pathname = GetPathnameFromFilename(filename);
     string shrtname = GetShortFilenameWithoutExtension(filename);

     // create directory based on filename
     stringstream ss;
     ss << pathname << shrtname << "\\";
     CreateDirectoryA(ss.str().c_str(), NULL);     

     // create output file
     ss << setfill('0') << setw(4) << i << ".bin";
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // move to data
     ifile.seekg(entrylist[i].startOffset - 1);
     if(ifile.fail()) return error("Seek failure.");

     // decompress data
     if(!DecompressZLIB(ifile, ofile)) return false;
    }

 return true;
}

*/