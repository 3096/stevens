#include "xentax.h"
#include "x_file.h"
#include "x_zlib.h"
#include "x_segs.h"

bool extractSEGS(LPCTSTR ifname, LPCTSTR ofname)
{
 using std::ios;
 using std::ifstream;
 using std::ofstream;
 ifstream ifile(ifname, ios::binary);
 if(!ifile) return error("Failed to open file.");
 ofstream ofile(ofname, ios::binary);
 if(!ofile) return error("Failed to create file.");
 return extractSEGS(ifile, ofile);
}

bool extractSEGS(std::ifstream& ifile, std::ofstream& ofile)
{
 return extractSEGS(ifile, (uint32)ifile.tellg(), ofile);
}

bool extractSEGS(std::ifstream& ifile, uint32 offset, std::ofstream& ofile)
{
 // move to offset
 using namespace std;
 ifile.seekg(offset);
 if(ifile.fail()) return error("Seek failure.");

 // loop SEGS
 uint32 segs_index = 0;
 for(;;)
    {
     // save position
     uint32 position = (uint32)ifile.tellg();
     if(ifile.fail()) return error("Tell failure.");

     // read magic
     uint32 magic = BE_read_uint32(ifile);
     if(ifile.fail()) {
        if(ifile.eof()) return true;
        return error("Read failure.");
       }
     if(magic != 0x73656773) {
        if(segs_index == 0) return error("Expecting SEGS.");
        else return true; // stop
       }
     
     // read unknown
     uint16 unk01 = BE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.");
     
     // read chunks
     uint16 n_chunks = BE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.");
     
     // read inflated size
     uint32 size_inflated = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     
     // read deflated size
     uint32 size_deflated = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // for each chunk
     deque<ZLIBINFO2> itemlist;
     for(uint32 i = 0; i < n_chunks; i++)
        {
         uint32 p01 = BE_read_uint16(ifile); // compressed size (can be zero)
         uint32 p02 = BE_read_uint16(ifile); // uncompressed size (can be zero)
         uint32 p03 = BE_read_uint32(ifile); // offset
         p03 += position;

         // special case (compressed)
         if(p01 && (p02 == 0x00)) {
            p02 = 0x10000;
            p03 = p03 - 1;
           }
         // special case (uncompressed)
         else if((p01 == 0x00) && (p02 == 0x00)) {
            p01 = 0x10000;
            p02 = 0x10000;
           }
         // compressed
         else
            p03 = p03 - 1;

         // read segs information
         ZLIBINFO2 info;
         info.deflatedBytes = p01;
         info.inflatedBytes = p02;
         info.offset = p03;
         itemlist.push_back(info);
        }
     
     // deflate and increment segment index
     if(!InflateZLIB(ifile, itemlist, -15, ofile)) return false;
     segs_index++;
    }

 if(segs_index > 1) cout << "File has " << (segs_index + 1) << " segments." << endl;
 return true;
}

/*
bool processSEG(const string& filename)
{
 // compressed chunk information
 struct SEGSINFO {
  uint16 deflate_size;
  uint16 inflate_size;
  uint32 offset;
 };
*/