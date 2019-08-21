#include "xentax.h"
#include "x_zlib.h"

bool isZLIB(uint08 b1, uint08 b2)
{
 // first byte check
 if(b1 == 0x18) ;
 else if(b1 == 0x28) ;
 else if(b1 == 0x38) ;
 else if(b1 == 0x48) ;
 else if(b1 == 0x58) ;
 else if(b1 == 0x68) ;
 else if(b1 == 0x78) ;
 else return false;

 // second byte check
 uint32 u1 = (uint32)b1;
 uint32 u2 = (uint32)b2;
 return ((u1*256 + u2) % 31) == 0;
}

bool isZLIB(std::ifstream& ifile, size_t n, int windowBits)
{
 // validate
 if(n < 2) return false; // too small
 if(!ifile.is_open()) return false; // can't read

 // save current position
 std::ifstream::pos_type position = ifile.tellg();
 if(ifile.fail()) return false; // can't tell position

 // read data
 boost::shared_array<char> idata(new char[n]);
 ifile.read(idata.get(), n);
 if(ifile.fail()) return false; // can't read

 // allocate inflate buffers
 const uint32 CHUNKSIZE = 65536;
 boost::shared_array<char> odata(new char[CHUNKSIZE]);

 // allocate inflate state
 z_stream strm;
 ZeroMemory(&strm, sizeof(strm));
 int ret = inflateInit2(&strm, windowBits);
 if(ret != Z_OK) return false; // can't initialize

 // initialize inflate state
 strm.avail_in = n;
 strm.next_in = reinterpret_cast<Bytef*>(idata.get());
 strm.avail_out = CHUNKSIZE;
 strm.next_out = reinterpret_cast<Bytef*>(odata.get());

 // run inflate() only once to test
 ret = inflate(&strm, Z_NO_FLUSH);
 if(ret == Z_NEED_DICT) {                
    inflateEnd(&strm);
    return false; // failed
   }
 else if(ret == Z_DATA_ERROR) {
    inflateEnd(&strm);
    return false; // failed
   }
 else if(ret == Z_MEM_ERROR) {
    ret = Z_DATA_ERROR;
    inflateEnd(&strm);
    return false; // failed
   }
 else if(ret == Z_STREAM_ERROR) {
    ret = Z_DATA_ERROR;
    inflateEnd(&strm);
    return false; // failed
   }

 // nothing bad happened
 inflateEnd(&strm);
 return true;
}

bool CompressZLIB(std::ifstream& ifile, std::ofstream& ofile)
{
 return CompressZLIB(ifile, ofile, 0);
}

bool CompressZLIB(std::ifstream& ifile, std::ofstream& ofile, int level)
{
/*
 // allocate deflate state
 z_stream strm;
 ZeroMemory(&strm, sizeof(strm));
 strm.zalloc = Z_NULL;
 strm.zfree = Z_NULL;
 strm.opaque = Z_NULL;
 int ret = deflateInit(&strm, level);
 if(ret != Z_OK) return ret;

 int flush;

 const uint32 CHUNKSIZE = 16384;
 unsigned char out[CHUNKSIZE];

 // compress until end of file
 do
 {
     unsigned char in[CHUNKSIZE];
     strm.avail_in = fread(in, 1, CHUNKSIZE, source);
     if(ferror(source)) {
        deflateEnd(&strm);
        return Z_ERRNO;
       }

     flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
     strm.next_in = in;

     // run deflate() on input until output buffer not full, finish compression if all of source has been read in
     do {

         strm.avail_out = CHUNKSIZE;
         strm.next_out = out;
         ret = deflate(&strm, flush);    // no bad return value
         assert(ret != Z_STREAM_ERROR);  // state not clobbered
         unsigned int have = CHUNKSIZE - strm.avail_out;
         if(fwrite(out, 1, have, dest) != have || ferror(dest)) {
            deflateEnd(&strm);
            return Z_ERRNO;
           }

     } while(strm.avail_out == 0);
     assert(strm.avail_in == 0); // all input will be used

 } while(flush != Z_FINISH);  // done when last data in file processed
 assert(ret == Z_STREAM_END); // stream will be complete

 // clean up and return
 deflateEnd(&strm);
 return Z_OK;
*/

 return true;
}

bool DecompressZLIB(std::ifstream& ifile, std::ofstream& ofile)
{
 return DecompressZLIB(ifile, ofile, 0);
}

bool DecompressZLIB(std::ifstream& ifile, std::ofstream& ofile, int windowBits)
{
 // validate
 if(!ifile.is_open()) return error("std::ifstream not open.");
 if(!ofile.is_open()) return error("std::ofstream not open.");

 // save current position
 std::ifstream::pos_type position = ifile.tellg();
 if(ifile.fail()) return error("Failed to retrieve std::ifstream position.");

 // allocate inflate buffers
 const uint32 MAXCHUNKSIZE = 65536;
 boost::shared_array<char> idata(new char[MAXCHUNKSIZE]);
 boost::shared_array<char> odata(new char[MAXCHUNKSIZE]);

 // while we have consecutive streams
 std::ifstream::pos_type bytes_read = 0;
 for(;;)
    {
     // allocate inflate state
     z_stream strm;
     ZeroMemory(&strm, sizeof(strm));
     int ret = inflateInit2(&strm, windowBits);
     if(ret != Z_OK) return error("ZLIB initialization failed.");

     // inflate until stream ends or end of file
     for(;;)
        {
         // get remaining filesize
         ifile.seekg(0, std::ios::end);
         std::ifstream::pos_type endpos = ifile.tellg();
         std::ifstream::pos_type distance = endpos - position;
         ifile.seekg(position);

         // end of file
         if(!distance) {
            inflateEnd(&strm);
            return true;
           }

         // read a chunk of data
         uint32 CHUNKSIZE = ((uint32)distance < MAXCHUNKSIZE ? (uint32)distance : MAXCHUNKSIZE);
         ifile.read(idata.get(), CHUNKSIZE);
         if(ifile.fail()) {
            inflateEnd(&strm);
            return error("ZLIB read error.");
           }

         // check if any data was read
         strm.avail_in = (uInt)ifile.gcount();
         if(strm.avail_in == 0) break;

         // set next input pointer (to buffer)
         strm.next_in = reinterpret_cast<Bytef*>(idata.get());

         // run inflate() on input until output buffer not full
         for(;;)
            {
             strm.avail_out = CHUNKSIZE;
             strm.next_out = reinterpret_cast<Bytef*>(odata.get());

             // decompress data
             ret = inflate(&strm, Z_NO_FLUSH);
             if(ret == Z_NEED_DICT) {                
                inflateEnd(&strm);
                return error("ZLIB need dictionary error.");;
               }
             else if(ret == Z_DATA_ERROR) {
                uLong total_out = strm.total_out;
                inflateEnd(&strm);
                if(!bytes_read || total_out) {
                   std::stringstream ss;
                   ss << "ZLIB data error at position 0x" << std::hex << position << std::dec;
                   return error(ss.str().c_str()); // error
                  }
                else return true; // no more consecutive zlib streams to read
               }
             else if(ret == Z_MEM_ERROR) {
                ret = Z_DATA_ERROR;
                inflateEnd(&strm);
                return error("ZLIB memory error.");
               }
             else if(ret == Z_STREAM_ERROR) {
                ret = Z_DATA_ERROR;
                inflateEnd(&strm);
                return error("ZLIB stream error.");
               }

             // save decompressed data
             unsigned int have = CHUNKSIZE - strm.avail_out;
             ofile.write(odata.get(), have);
             if(ofile.fail()) {
                inflateEnd(&strm);
                return error("ZLIB error writing data.");
               }
             bytes_read += have;
    
             // stop when buffer is not full
             if(strm.avail_out != 0) break;
            }

         // done when inflate() says it's done
         if(ret == Z_STREAM_END) break;
        }

     // clean up and move to end of stream
     position += strm.total_in;
     inflateEnd(&strm);
     ifile.seekg(position);
     if(ifile.fail()) break;
    }

 return true;
}

bool DecompressZLIB(std::ifstream& ifile, size_t n, std::ofstream& ofile, int windowBits)
{
 // validate
 if(n < 2) return error("Unexpected number of ZLIB bytes to read.");
 if(!ifile.is_open()) return error("std::ifstream not open.");
 if(!ofile.is_open()) return error("std::ofstream not open.");

 // save current position
 std::ifstream::pos_type position = ifile.tellg();
 if(ifile.fail()) return error("Failed to retrieve std::ifstream position.");

 // read data
 boost::shared_array<char> idata(new char[n]);
 ifile.read(idata.get(), n);
 if(ifile.fail()) return error("Read failure.");

 // allocate inflate buffers
 const uint32 CHUNKSIZE = 65536;
 boost::shared_array<char> odata(new char[CHUNKSIZE]);

 // allocate inflate state
 z_stream strm;
 ZeroMemory(&strm, sizeof(strm));
 int ret = inflateInit2(&strm, windowBits);
 if(ret != Z_OK) return error("ZLIB initialization failed.");

 // initialize inflate state
 strm.avail_in = n;
 strm.next_in = reinterpret_cast<Bytef*>(idata.get());

 // run inflate() on input until output buffer not full
 std::ifstream::pos_type bytes_read = 0;
 for(;;)
    {
     // initialize inflate state
     strm.avail_out = CHUNKSIZE;
     strm.next_out = reinterpret_cast<Bytef*>(odata.get());

     // decompress data
     ret = inflate(&strm, Z_NO_FLUSH);
     if(ret == Z_NEED_DICT) {                
        inflateEnd(&strm);
        return error("ZLIB need dictionary error.");;
       }
     else if(ret == Z_DATA_ERROR) {
        inflateEnd(&strm);
        return error("ZLIB data error.");
       }
     else if(ret == Z_MEM_ERROR) {
        ret = Z_DATA_ERROR;
        inflateEnd(&strm);
        return error("ZLIB memory error.");
       }
     else if(ret == Z_STREAM_ERROR) {
        ret = Z_DATA_ERROR;
        inflateEnd(&strm);
        return error("ZLIB stream error.");
       }

     // save decompressed data
     unsigned int have = CHUNKSIZE - strm.avail_out;
     ofile.write(odata.get(), have);
     if(ofile.fail()) {
        inflateEnd(&strm);
        return error("ZLIB error writing data.");
       }
     bytes_read += have;
    
     // stop when buffer is not full
     if(strm.avail_out != 0) break;
    }

 // done when inflate() says it's done
 inflateEnd(&strm);
 return (ret == Z_STREAM_END);
}

bool DecompressZLIB(std::ifstream& ifile, const std::deque<ZLIBINFO>& zinfo, int windowBits)
{
 // validate
 if(!ifile.is_open()) return error("std::ifstream not open.");
 if(!zinfo.size()) return true;

 // save current position
 std::ifstream::pos_type position = ifile.tellg();
 if(ifile.fail()) return error("Failed to retrieve std::ifstream position.");

 // allocate inflate state
 z_stream strm;
 ZeroMemory(&strm, sizeof(strm));
 int ret = inflateInit2(&strm, windowBits);
 if(ret != Z_OK) return error("ZLIB initialization failed.");

 // decompress files
 for(size_t i = 0; i < zinfo.size(); i++)
    {
     // create empty output file
     if(!zinfo[i].inflatedBytes && !zinfo[i].deflatedBytes) {
        std::ofstream ofile(zinfo[i].filename.c_str(), std::ios::binary);
        if(!ofile) return error("ZLIB Error: Failed to create output file.");
        continue;
       }

     // validate
     if(!zinfo[i].inflatedBytes) return error("ZLIB Error: Inflated size is equal to zero.");
     if(!zinfo[i].deflatedBytes) return error("ZLIB Error: Deflated size is equal to zero.");
     if(!zinfo[i].filename.length()) return error("ZLIB Error: Invalid filename.");

     // create output file
     std::ofstream ofile(zinfo[i].filename.c_str(), std::ios::binary);
     if(!ofile) return error("ZLIB Error: Failed to create output file.");

     // allocate inflate buffers
     boost::shared_array<char> idata(new char[zinfo[i].deflatedBytes]);
     boost::shared_array<char> odata(new char[zinfo[i].inflatedBytes]);

     // read data
     ifile.read(idata.get(), zinfo[i].deflatedBytes);
     if(ifile.fail()) {
        inflateEnd(&strm);
        return error("ZLIB Error: Input file read error.");
       }

     // set input pointer
     strm.avail_in = (uInt)zinfo[i].deflatedBytes;
     strm.next_in = reinterpret_cast<Bytef*>(idata.get());

     // inflate until output buffer is full
     for(;;)
        {
         // set output pointer
         strm.avail_out = zinfo[i].inflatedBytes;
         strm.next_out = reinterpret_cast<Bytef*>(odata.get());

         // decompress data
         ret = inflate(&strm, Z_NO_FLUSH);
         if(ret == Z_NEED_DICT) {                
            inflateEnd(&strm);
            return error("ZLIB Error: Dictionary error.");;
           }
         else if(ret == Z_DATA_ERROR) {
            uLong total_out = strm.total_out;
            inflateEnd(&strm);
            if(total_out == zinfo[i].inflatedBytes) return true; // OK
            return error("ZLIB Error: Data error."); // error
           }
         else if(ret == Z_MEM_ERROR) {
            ret = Z_DATA_ERROR;
            inflateEnd(&strm);
            return error("ZLIB Error: Memory error.");
           }
         else if(ret == Z_STREAM_ERROR) {
            ret = Z_DATA_ERROR;
            inflateEnd(&strm);
            return error("ZLIB Error: Stream error.");
           }

         // save decompressed data
         unsigned int have = zinfo[i].inflatedBytes - strm.avail_out;
         ofile.write(odata.get(), have);
         if(ofile.fail()) {
            inflateEnd(&strm);
            return error("ZLIB error writing data.");
           }

         // stop when buffer is full
         if(strm.avail_out == 0) break;
        }
    }

 // clean up
 inflateEnd(&strm);

 return true;
}

bool InflateZLIB(std::ifstream& ifile, std::ofstream& ofile, int windowBits)
{
 // validate
 using namespace std;
 if(!ifile.is_open()) return error("std::ifstream not open.");
 if(!ofile.is_open()) return error("std::ofstream not open.");

 // save current position
 std::ifstream::pos_type position = ifile.tellg();
 if(ifile.fail()) return error("Failed to retrieve std::ifstream position.");

 // allocate inflate buffers
 const uint32 MAXCHUNKSIZE = 65536;
 boost::shared_array<char> idata(new char[MAXCHUNKSIZE]);
 boost::shared_array<char> odata(new char[MAXCHUNKSIZE]);

 // while we have consecutive streams
 for(;;)
    {
     // allocate inflate state
     z_stream strm;
     ZeroMemory(&strm, sizeof(strm));
     int ret = inflateInit2(&strm, windowBits);
     if(ret != Z_OK) return error("ZLIB initialization failed.");

     // inflate until stream ends or end of file
     for(;;)
        {
         // get remaining filesize
         ifile.seekg(0, std::ios::end);
         std::ifstream::pos_type distance = ifile.tellg() - position;
         if(!distance) {
            inflateEnd(&strm);
            return true;
           }

         // move to data
         ifile.seekg(position);
         if(ifile.fail()) {
            inflateEnd(&strm);
            return true;
           }

         // read a chunk of data
         uint32 CHUNKSIZE = ((uint32)distance < MAXCHUNKSIZE ? (uint32)distance : MAXCHUNKSIZE);
         ifile.read(idata.get(), CHUNKSIZE);
         if(ifile.fail()) {
            inflateEnd(&strm);
            return error("ZLIB read error.");
           }

         // check if any data was read
         strm.avail_in = CHUNKSIZE;
         if(strm.avail_in == 0) break;

         // set next input pointer (to buffer)
         strm.next_in = reinterpret_cast<Bytef*>(idata.get());

         // run inflate() on input until output buffer not full
         for(;;)
            {
             strm.avail_out = CHUNKSIZE;
             strm.next_out = reinterpret_cast<Bytef*>(odata.get());

             // decompress data
             ret = inflate(&strm, Z_NO_FLUSH);
             if(ret == Z_NEED_DICT) {                
                inflateEnd(&strm);
                return error("ZLIB need dictionary error.");;
               }
             else if(ret == Z_DATA_ERROR) {
                unsigned int have = CHUNKSIZE - strm.avail_out;
                if(!have) {
                   inflateEnd(&strm);
                   std::stringstream ss;
                   ss << "ZLIB data error at position 0x" << std::hex << position << std::dec;
                   return error(ss.str().c_str()); // error
                  }
               }
             else if(ret == Z_MEM_ERROR) {
                ret = Z_DATA_ERROR;
                inflateEnd(&strm);
                return error("ZLIB memory error.");
               }
             else if(ret == Z_STREAM_ERROR) {
                ret = Z_DATA_ERROR;
                inflateEnd(&strm);
                return error("ZLIB stream error.");
               }

             // save decompressed data
             unsigned int have = CHUNKSIZE - strm.avail_out;
             ofile.write(odata.get(), have);
             if(ofile.fail()) {
                inflateEnd(&strm);
                return error("ZLIB error writing data.");
               }
    
             // stop when there is zero space left in the output buffer
             if(strm.avail_out != 0) break;
            }

         // done when inflate() says it's done
         if(ret == Z_STREAM_END) break;

         // increment by chunksize
         position += CHUNKSIZE;
        }

     // clean up and move to end of stream
     inflateEnd(&strm);
     ifile.seekg(position);
     if(ifile.fail()) break;
    }

 return true;
}

bool InflateZLIB(std::ifstream& ifile, const std::deque<ZLIBINFO2>& zinfo, int windowBits, std::ofstream& ofile)
{
 // validate
 if(!ifile.is_open()) return error("std::ifstream not open.");
 if(!zinfo.size()) return true;

 // save current position
 std::ifstream::pos_type position = ifile.tellg();
 if(ifile.fail()) return error("Failed to retrieve std::ifstream position.");

 // decompress files
 for(size_t i = 0; i < zinfo.size(); i++)
    {
     // validate
     if(!zinfo[i].inflatedBytes) return error("ZLIB Error: Inflated size is equal to zero.");
     if(!zinfo[i].deflatedBytes) return error("ZLIB Error: Deflated size is equal to zero.");

     // allocate inflate buffers
     boost::shared_array<char> idata(new char[zinfo[i].deflatedBytes]);
     boost::shared_array<char> odata(new char[zinfo[i].inflatedBytes]);

     // seek data
     ifile.seekg(zinfo[i].offset);
     if(ifile.fail()) return error("ZLIB Error: Seek failure.");

     // read data
     ifile.read(idata.get(), zinfo[i].deflatedBytes);
     if(ifile.fail()) return error("ZLIB Error: Input file read error.");

     // no compression so copy data
     if(zinfo[i].inflatedBytes == zinfo[i].deflatedBytes) {
        ofile.write(idata.get(), zinfo[i].deflatedBytes);
        continue;
       }

     // allocate inflate state
     z_stream strm;
     ZeroMemory(&strm, sizeof(strm));
     int ret = inflateInit2(&strm, windowBits);
     if(ret != Z_OK) return error("ZLIB initialization failed.");

     // set input pointer
     strm.avail_in = (uInt)zinfo[i].deflatedBytes;
     strm.next_in = reinterpret_cast<Bytef*>(idata.get());

     // inflate until output buffer is full
     for(;;)
        {
         // set output pointer
         strm.avail_out = zinfo[i].inflatedBytes;
         strm.next_out = reinterpret_cast<Bytef*>(odata.get());

         // decompress data
         ret = inflate(&strm, Z_NO_FLUSH);
         if(ret == Z_NEED_DICT) {                
            inflateEnd(&strm);
            return error("ZLIB Error: Dictionary error.");;
           }
         else if(ret == Z_DATA_ERROR) {
            uLong total_out = strm.total_out;
            inflateEnd(&strm);
            if(total_out == zinfo[i].inflatedBytes) break; // done
            return error("ZLIB Error: Data error."); // error
           }
         else if(ret == Z_MEM_ERROR) {
            ret = Z_DATA_ERROR;
            inflateEnd(&strm);
            return error("ZLIB Error: Memory error.");
           }
         else if(ret == Z_STREAM_ERROR) {
            ret = Z_DATA_ERROR;
            inflateEnd(&strm);
            return error("ZLIB Error: Stream error.");
           }

         // save decompressed data
         unsigned int have = zinfo[i].inflatedBytes - strm.avail_out;
         ofile.write(odata.get(), have);
         if(ofile.fail()) {
            inflateEnd(&strm);
            return error("ZLIB error writing data.");
           }

         // stop when buffer is full
         if(strm.avail_out > 0) break;
        }

     // clean up
     inflateEnd(&strm);
    }

 return true;
}

bool DeflateZLIB(const char* data, uint32 size, std::ofstream& ofile)
{
 // output buffer
 const uint32 MAXCHUNKSIZE = 0x40000; // 256 KB
 boost::shared_array<char> buffer(new char[MAXCHUNKSIZE]);

 // stream properties
 z_stream strm;
 strm.zalloc = Z_NULL;
 strm.zfree = Z_NULL;
 strm.opaque = Z_NULL;

 // initialize deflate
 int ret = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
 if(ret != Z_OK) return error("ZLIB deflate initialization failed.");

 // loop forever
 uint32 position = 0;
 while(position < size)
      {
       // how much input data should we use
       uint32 in_bytes = ((size - position < MAXCHUNKSIZE) ? (size - position) : MAXCHUNKSIZE);
       strm.avail_in = in_bytes;
       strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data) + position);

       // move to next position
       position += in_bytes;
       int flush = ((size == position) ? Z_FINISH : Z_NO_FLUSH);

       // run deflate on input
       for(;;)
          {
           // make output buffer freely available
           strm.avail_out = MAXCHUNKSIZE;
           strm.next_out = reinterpret_cast<Bytef*>(buffer.get());

           // run deflate
           ret = deflate(&strm, flush);
           if(ret == Z_STREAM_ERROR) return error("ZLIB deflate failed.");

           // save compressed output
           uint32 written = MAXCHUNKSIZE - strm.avail_out;
           ofile.write(buffer.get(), written);
           if(ofile.fail()) {
              deflateEnd(&strm);
              return error("ZLIB write failure.");
             }

           // if strm.avail_out = 0, the output buffer was completely written to
           // and we can't write anymore to it so we need to repeat this loop to
           // start with a fresh output buffer. if strm.avail_out > 0, all of the
           // input data was compressed so we can safely terminate the inner loop
           // and collect more input.
           if(strm.avail_out > 0) {
              break;
             }
          }
      }

 // finish deflate
 deflateEnd(&strm);
 if(ret != Z_STREAM_END) return error("ZLIB stream not complete.");
 return true;
}

bool InflateZLIB(const char* data, uint32 size, std::ofstream& ofile, int windowBits)
{
 // allocate inflate buffers
 const uint32 MAXCHUNKSIZE = 65536;
 boost::shared_array<char> idata(new char[MAXCHUNKSIZE]);
 boost::shared_array<char> odata(new char[MAXCHUNKSIZE]);

 // allocate inflate state
 z_stream strm;
 ZeroMemory(&strm, sizeof(strm));
 int ret = inflateInit2(&strm, windowBits);
 if(ret != Z_OK) return error("ZLIB initialization failed.");

 // inflate until stream ends or end of file
 uint32 total_bytes_read = 0;
 for(;;)
    {
     // finished
     uint32 bytes_remaining = size - total_bytes_read;
     if(!bytes_remaining) break;

     // copy a chunk of data
     uint32 CHUNKSIZE = ((uint32)bytes_remaining < MAXCHUNKSIZE ? (uint32)bytes_remaining : MAXCHUNKSIZE);
     memmove(idata.get(), (data + total_bytes_read), CHUNKSIZE);
     total_bytes_read += CHUNKSIZE;

     // set amount to be processed and set next input pointer
     strm.avail_in = CHUNKSIZE;
     strm.next_in = reinterpret_cast<Bytef*>(idata.get());

     // run inflate() on input until output buffer not full
     for(;;)
        {
         // output buffer properties
         strm.avail_out = MAXCHUNKSIZE;
         strm.next_out = reinterpret_cast<Bytef*>(odata.get());

         // decompress data
         ret = inflate(&strm, Z_NO_FLUSH);
         if(ret == Z_NEED_DICT) {                
            inflateEnd(&strm);
            return error("ZLIB need dictionary error.");
           }
         else if(ret == Z_DATA_ERROR) {
            uLong total_out = strm.total_out;
            inflateEnd(&strm);
            if(total_out) { // in other words, if we didn't output anything, we came to the end of the input buffer
               std::stringstream ss;
               ss << "ZLIB data error." << std::dec;
               return error(ss.str().c_str());
              }
            // else return true; // no more consecutive zlib streams to read
            else return error("ZLIB data error.");
           }
         else if(ret == Z_MEM_ERROR) {
            ret = Z_DATA_ERROR;
            inflateEnd(&strm);
            return error("ZLIB memory error.");
           }
         else if(ret == Z_STREAM_ERROR) {
            ret = Z_DATA_ERROR;
            inflateEnd(&strm);
            return error("ZLIB stream error.");
           }

         // save decompressed data
         unsigned int have = MAXCHUNKSIZE - strm.avail_out;
         ofile.write(odata.get(), have);
         if(ofile.fail()) {
            inflateEnd(&strm);
            return error("ZLIB error writing data.");
           }

         // stop when buffer is not full
         if(strm.avail_out != 0) break;
        }

     // done when inflate() says it's done
     if(ret == Z_STREAM_END) break;
    }

 // finished
 inflateEnd(&strm);
 return true;
}