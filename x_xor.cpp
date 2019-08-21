#include "xentax.h"
#include "x_xor.h"

struct XORPAIR {
 unsigned char c;
 uint32 count;
};

struct XORPAIRCOMPARE {
 bool operator ()(const XORPAIR& p1, const XORPAIR& p2)const {
  return p2.count < p1.count;
 }
};

bool xorencrypt(const char* srcfile, const char* dstfile, const char* xorkey)
{
 // open source file
 using namespace std;
 ifstream ifile(srcfile, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 ifile.seekg(0, ios::end);
 uint64 filesize = (uint64)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // create output file
 ofstream ofile(dstfile, ios::binary);
 if(!ofile) return error("Failed to create file.");

 // size of xorkey
 uint32 keysize = strlen(xorkey);
 if(!keysize) return error("Invalid XOR key.");

 // create data buffer
 boost::shared_array<char> buffer(new char[keysize]);

 // loop through file
 uint64 bytes_read = 0;
 while(bytes_read < filesize)
      {
       // adjust keysize if needed
       uint64 bytes_remaining = (filesize - bytes_read);
       if(bytes_remaining < keysize) keysize = (uint32)bytes_remaining;

       // read data into buffer
       ifile.read(buffer.get(), keysize);
       if(ifile.fail()) return error("Read failure.");

       // xor buffer
       for(uint32 i = 0; i < keysize; i++)
           buffer[i] ^= xorkey[i];

       // save data
       ofile.write(buffer.get(), keysize);
       if(ofile.fail()) return error("Write failure.");

       // keep track of bytes read
       bytes_read += keysize;
      } 

 return true;
}

bool xorfind(const char* srcfile, const char* dstfile, XORFINDINFO* info)
{
 // open source file
 using namespace std;
 ifstream ifile(srcfile, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 if(!filesize) return error("Cannot compute XOR key because file is empty.");
 ifile.seekg(0, ios::beg);

 // read entire file into memory
 boost::shared_array<unsigned char> srcdata(new unsigned char[filesize]);
 ifile.read((char*)srcdata.get(), filesize);
 if(ifile.fail()) return error("Read failure.");

 // compute maximum offset
 uint32 max_offset = 256;
 if(filesize < max_offset) max_offset = filesize;

 // compute index of coincidence for each offset
 boost::shared_array<unsigned char> coincidence(new unsigned char[filesize]);
 uint32 max_0x00 = 0;
 uint32 max_0x00_offset = 0;
 for(uint32 offset = 1; offset < 256; offset++) {
     uint32 count_0x00 = 0;
     for(uint32 i = 0; i < filesize; i++) {
         coincidence[i] = srcdata[i] ^ srcdata[(i + offset) % filesize];
         if(coincidence[i] == 0x00) count_0x00++;
        }
     // update maximum values
     if(count_0x00 > max_0x00) {
         max_0x00 = count_0x00;
         max_0x00_offset = offset;
        }
    }

 cout << "max_0x00_offset = 0x" << hex << max_0x00_offset << dec << endl;

 // initialize key and key length
 memset(&info->key[0], 0, 256);
 info->keylength = max_0x00_offset;

 // trellis graph node
 struct TRELLISNODE {
  unsigned char c;
  uint32 local_cost; // unused, 0
  uint32 total_cost; // best total cost to this node
  uint32 previous;
 };

 // construct trellis graph
 const uint32 TRELLIS_DEPTH = 8;
 boost::shared_array<boost::shared_array<TRELLISNODE>> keytable(new boost::shared_array<TRELLISNODE>[info->keylength]);
 for(uint32 i = 0; i < info->keylength; i++) {
     keytable[i].reset(new TRELLISNODE[TRELLIS_DEPTH]);
     for(uint32 j = 0; j < TRELLIS_DEPTH; j++) {
         keytable[i][j].local_cost = 0;
         keytable[i][j].total_cost = 0;
         keytable[i][j].previous = 0;
        }
    }

 // for each character in the key
 for(uint32 i = 0; i < info->keylength; i++)
    {
     // initialize character counters
     XORPAIR count[256];
     for(uint32 j = 0; j < 256; j++) {
         count[j].c = (unsigned char)j;
         count[j].count = 0;
        }

     // for each character in the source file
     for(uint32 j = i; j < filesize; j += info->keylength) {
         for(uint32 k = 0; k < 256; k++) {
             unsigned char testchar = (unsigned char)k;
             unsigned char result = srcdata[j] ^ testchar;
             if(result == 0x00) count[k].count++;
             else if(result == 0xFF) count[k].count++;
            }
        }

     // sort counts and add best characters to table
     std::sort(&count[0], &count[0] + 256, XORPAIRCOMPARE());
     for(uint32 j = 0; j < TRELLIS_DEPTH; j++) keytable[i][j].c = count[j].c;

     // display characters
     for(uint32 j = 0; j < TRELLIS_DEPTH; j++) cout << setfill('0') << setw(2) << hex << (uint32)count[j].c << dec << " - "; 
     cout << endl;
    }

 // viterbi algorithm
 for(uint32 i = 1; i < info->keylength; i++)
    {
     // progress
     cout << i << " of " << info->keylength << " done." << endl;

     // left and right side key indices
     uint32 L = i - 1;
     uint32 R = i;

     // for each state in T[R][0, TRELLIS_DEPTH - 1]
     for(uint32 R_index = 0; R_index < TRELLIS_DEPTH; R_index++)
        {
         // compute edgeweight from T[L][0] to T[R][R_index]
         uint32 L_index = 0;
         unsigned char keyL = keytable[L][L_index].c;
         unsigned char keyR = keytable[R][R_index].c;
         uint32 srcindexL = L;
         uint32 srcindexR = R;
         uint32 edgecost = 0;
         while(srcindexR < filesize) {
             unsigned char charL = srcdata[srcindexL] ^ keyL;
             unsigned char charR = srcdata[srcindexR] ^ keyR;
             if(!charL && !charR) edgecost += 2;
             else if(charL == 0x00 && charR != 0x00) edgecost += 1;
             else if(charL != 0x00 && charR == 0x00) edgecost -= (edgecost > 1 ? 1 : 0);
             else if(charL == 0xFF && charR == 0xFF) edgecost += 2;
             srcindexL = srcindexL + info->keylength;
             srcindexR = srcindexL + 1;
            }

         // compute best cost from T[L][0] to T[R][R_index]
         uint32 B_value = keytable[L][L_index].total_cost + edgecost;
         uint32 B_index = L_index;

         // for each state in T[L][0, TRELLIS_DEPTH - 1]
         for(uint32 L_index = 1; L_index < TRELLIS_DEPTH; L_index++)
            {
             // compute edgeweight from T[L][L_index] to T[R][R_index]
             unsigned char keyL = keytable[L][L_index].c;
             unsigned char keyR = keytable[R][R_index].c;
             uint32 srcindexL = L;
             uint32 srcindexR = R;
             uint32 edgecost = 0;
             while(srcindexR < filesize) {
                 unsigned char charL = srcdata[srcindexL] ^ keyL;
                 unsigned char charR = srcdata[srcindexR] ^ keyR;
                 if(!charL && !charR) edgecost += 2;
                 else if(charL == 0x00 && charR != 0x00) edgecost += 1;
                 else if(charL != 0x00 && charR == 0x00) edgecost -= (edgecost > 1 ? 1 : 0);
                 else if(charL == 0xFF && charR == 0xFF) edgecost += 2;
                 srcindexL = srcindexL + info->keylength;
                 srcindexR = srcindexL + 1;
                }
             
             // compute best path to T[R][R_index]
             uint32 T_value = keytable[L][L_index].total_cost + edgecost;
             if(B_value < T_value) {
                B_value = T_value; // is this right?
                B_index = L_index; // is this right?
               }
            }

         // set best path
         keytable[R][R_index].total_cost = B_value;
         keytable[R][R_index].previous = B_index;
        }
    }

 // find best total cost in last column
 uint32 B_index = 0;
 uint32 B_value = keytable[info->keylength - 1][B_index].total_cost;
 for(uint32 i = 1; i < TRELLIS_DEPTH; i++) {
     if(keytable[info->keylength - 1][i].total_cost > B_value) {
        B_index = i;
        B_value = keytable[info->keylength - 1][i].total_cost;
       }
    }

 // trace optimal path in reverse
 deque<uint32> path;
 for(uint32 i = 0; i < info->keylength; i++) {
     path.push_front(B_index);
     B_index = keytable[(info->keylength - 1) - i][B_index].previous;
    }

 // output key
 for(uint32 i = 0; i < info->keylength; i++) {
     info->key[i] = keytable[i][path[i]].c;
     cout << "index = " << path[i] << endl;
    }

 // display cost table
 for(uint32 i = 0; i < info->keylength; i++) {
     for(uint32 j = 0; j < TRELLIS_DEPTH; j++) cout << setfill('0') << setw(4) << hex << keytable[i][j].total_cost << dec << " - ";
     cout << endl;
    }

 return true;
}