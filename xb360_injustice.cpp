#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "xb360_injustice.h"
using namespace std;

#define X_SYSTEM XBOX360
#define X_GAME   INJUSTICE

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

struct XXXENTRY {
 uint32 p01; // uncompressed offset
 uint32 p02; // uncompressed size
 uint32 p03; // offset
 uint32 p04; // data size
};

struct XXXSTRINGENTRY {
 uint32 size;
 char name[1024];
 uint32 p01;
 uint32 p02;
 uint32 p03;
 uint32 p04;
};

struct XXXPROPERTY {
 uint32 size;
 char name[1024];
 uint16 p01;
 uint16 p02;
 uint32 p03;
};

struct XXXEXPORT {
 uint32 p01;
 uint32 p02;
 uint32 p03;
 uint32 p04;
 uint32 p05;
 uint32 p06;
 uint32 p07;
};

struct XXXIMPORT {
 uint32 info[17];
 uint32 size;
 boost::shared_array<uint32> data;
};

struct XXXATTRIBUTE {
 uint32 nameref; // index into property table
 uint32 unk01;   // unknown (usually 0)
 uint32 typeref; // index info property table
 uint32 unk02;   // unknown (usually 0)
 uint32 size;    // size of attribute in bytes
 uint32 unk03;   // unknown (usually 0)
 boost::shared_array<char> attribute; // attribute data (of size bytes)
};

bool readAttributeList(ifstream& ifile, const deque<XXXPROPERTY>& proplist, uint32 other, uint32 flags, deque<XXXATTRIBUTE>& attrlist)
{
 set<string> names;
 names.insert("ArrayProperty");
 names.insert("BoolProperty");
 names.insert("ByteProperty");
 names.insert("ClassProperty");
 names.insert("ComponentProperty");
 names.insert("FloatProperty");
 names.insert("IntProperty");
 names.insert("NameProperty");
 names.insert("ObjectProperty");
 names.insert("StrProperty");
 names.insert("StructProperty");

 // save position
 uint32 position = (uint32)ifile.tellg();
 if(ifile.fail()) return error("Tell failure.");

 // read 1st value
 uint32 test1 = BE_read_uint32(ifile);
 cout << "trying 0x" << hex << test1 << dec << endl;
 if(ifile.fail()) return error("Read failure.");
 if(test1 < proplist.size()) {
    if(strcmp(proplist[test1].name, "None") == 0) return true;
   }

 // read 2nd value
 uint32 test2 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read 3rd value
 uint32 test3 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // test values
 bool found = false;
 if((test1 < proplist.size()) && (test2 == 0) && (test3 < proplist.size()))
    if(names.find(proplist[test3].name) != names.end()) found = true;

 while(!found)
      {
       // increment position
       position += 4;

       // read 1st value
       test1 = test2;
       cout << "trying 0x" << hex << test1 << dec << endl;
       if(test1 < proplist.size() && strcmp(proplist[test1].name, "None") == 0)
          return true;

       // read 2nd value
       test2 = test3;

       // read 3rd value
       test3 = BE_read_uint32(ifile);
       if(ifile.fail()) return error("Read failure.");

       // test values
       if((test1 < proplist.size()) && (test2 == 0) && (test3 < proplist.size()))
          if(names.find(proplist[test3].name) != names.end()) found = true;
      }

 ifile.seekg(position);
 cout << "POSITION = 0x" << hex << position << dec << endl;
 cout << endl;

 for(;;)
    {
     // 0x00: read name reference
     XXXATTRIBUTE attr;
     attr.nameref = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure #1.");

     // empty attribute list
     if(!(attr.nameref  < proplist.size()))
        return true;

     // last attribute is "None"
     if(strcmp(proplist[attr.nameref].name, "None") == 0)
        return true;

     // 0x04: read unknown
     attr.unk01 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure #2.");

     // 0x08: read index info property table
     attr.typeref = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure #3.");

     // 0x0C: read unknown
     attr.unk02 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure #4.");

     // 0x10: size of attribute in bytes
     attr.size = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure #5.");

     // 0x14: read unknown
     attr.unk03 = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     
     // 0x18
     if(strcmp(proplist[attr.typeref].name, "ArrayProperty") == 0)
       {
        attr.size += 0;
       }
     else if(strcmp(proplist[attr.typeref].name, "BoolProperty") == 0)
       {
        if(attr.size != 0x00) return error("BoolProperty must have size of 0.");
        attr.size += 4; // marked as 0 even if size is 4
       }
     else if(strcmp(proplist[attr.typeref].name, "ByteProperty") == 0)
       {
        if(attr.size != 0x01) return error("IntProperty must have size of 1.");
        attr.size += 0;
       }
     else if(strcmp(proplist[attr.typeref].name, "FloatProperty") == 0)
       {
        if(attr.size != 0x04) return error("FloatProperty must have size of 4.");
        attr.size += 0;
       }
     else if(strcmp(proplist[attr.typeref].name, "IntProperty") == 0)
       {
        if(attr.size != 0x04) return error("IntProperty must have size of 4.");
        attr.size += 0;
       }
     else if(strcmp(proplist[attr.typeref].name, "NameProperty") == 0)
       {
        attr.size += 0;
       }
     else if(strcmp(proplist[attr.typeref].name, "ObjectProperty") == 0)
       {
        attr.size += 0;
       }
     else if(strcmp(proplist[attr.typeref].name, "StrProperty") == 0)
       {
        attr.size += 0;
       }
     else if(strcmp(proplist[attr.typeref].name, "StructProperty") == 0)
       {
        // increment size of data by 8
        // 4 bytes: structure type (Vector, etc.)
        // 4 bytes: 0
        attr.size += 8;
       }
     else {
        stringstream ss;
        ss << "Unknown property " << proplist[attr.typeref].name << "." << endl;
        return error(ss);
       }

     // read attribute data
     if(attr.size) {
        attr.attribute.reset(new char[attr.size]);
        ifile.read(attr.attribute.get(), attr.size);
        if(ifile.fail()) return error("Read failure #6.");
       }        

     // insert attribute
     attrlist.push_back(attr);
    }

 return false;
}

bool processBIN(ifstream& ifile, ofstream& ofile)
{
 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x9E2A83C1) return error("Invalid magic number.");

 // read file signature
 uint32 filesig = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // must be certain type
 if(filesig != 0x00020000)
    return error("Uncompressed package.");

 // total compressed size
 uint32 p01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(p01 == 0x00000) return error("Invalid BIN file.");

 // total uncompressed size
 uint32 p02 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // pair data
 typedef pair<uint32, uint32> pair_t;
 deque<pair_t> table;

 // read pairs
 uint32 bytes_read = 0;
 for(;;) {
     uint32 size1 = BE_read_uint32(ifile);
     uint32 size2 = BE_read_uint32(ifile);
     table.push_back(pair_t(size1, size2));
     bytes_read += size1;
     if(!(bytes_read < p01)) break;
    }

 // for each pair
 for(uint32 i = 0; i < table.size(); i++)
    {
     // sizes
     uint32 srcsize = table[i].first;
     uint32 dstsize = table[i].second;
 
     // NOTE THAT SOME FILES CAN BE COMPRESSED BADLY!!!
     // FOR EXAMPLE: ALREADY COMPRESSED SOUND FILES

     // create buffers
     boost::shared_array<unsigned char> srcdata(new unsigned char[srcsize]);
     boost::shared_array<unsigned char> dstdata(new unsigned char[dstsize]);

     // read compressed data
     ifile.read((char*)srcdata.get(), srcsize);
     if(ifile.fail()) return error("Read failure.");

     // decompress data
     lzo_uint outsize = dstsize;
     int result = lzo1x_decompress_safe(srcdata.get(), srcsize, dstdata.get(), &outsize, NULL);
     if(result != LZO_E_OK) message("LZO decompression failed.");

     // save data
     ofile.write((char*)dstdata.get(), dstsize);
     if(ofile.fail()) return error("Write failure.");

     // NOT LZX!!!

     // // LZX parameters
     // XMEMCODEC_PARAMETERS_LZX lzxinfo;
     // lzxinfo.Flags = 0;
     // lzxinfo.WindowSize = 0x8000;
     // lzxinfo.CompressionPartitionSize = 0x8000;
     // 
     // // create decompression context
     // XMEMDECOMPRESSION_CONTEXT context;
     // HRESULT result = XMemCreateDecompressionContext(XMEMCODEC_LZX, (LPVOID)&lzxinfo, 0, &context);
     // if(FAILED(result)) return error("Failed to create decompression context.");
     // 
     // // decompress
     // SIZE_T isize = srcsize;
     // SIZE_T osize = dstsize;
     // cout << "position = 0x" << hex << (uint32)ifile.tellg() << dec << endl;
     // cout << "outsize = " << osize << endl;
     // result =  XMemDecompress(context, dstdata.get(), &osize, srcdata.get(), isize);
     // if(FAILED(result)) return error("LZX decompression failed.");
     // cout << "outsize = " << osize << endl;
     // 
     // // delete decompression context
     // XMemDestroyDecompressionContext(context);
    }

 return true;
}

bool processXXX(const string& filename)
{
 // open file
 using namespace std;
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("File open failure.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x9E2A83C1) return error("Invalid magic number.");

 // read file signature
 uint32 filesig = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // compressed files
 if(filesig == 0x0031023D)
   {
    // read header size
    // applies to the file after decompression
    uint32 headersize = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // read EROK
    uint32 unk02 = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // read 0x42
    uint32 unk03 = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // read string length
    uint32 unk04 = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // read string
    char unk05[1024];
    if(!read_string(ifile, &unk05[0], unk04)) return error("Read failure.");

    // read unknowns
    uint16 unk06 = BE_read_uint16(ifile);
    uint16 unk07 = BE_read_uint16(ifile);
    if(ifile.fail()) return error("Read failure.");

    // read property count
    // the number of properties to read, applies to the file after decompression
    uint32 property_count = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // read property offset
    uint32 property_offset = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // read 0x48 bytes
    uint32 unk09[0x11];
    for(uint32 i = 0; i < 0x11; i++) {
        unk09[i] = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");
       }

    // read number of tables
    uint32 n_stringtables = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // read number of entries
    uint32 n_entries = BE_read_uint32(ifile);
    if(ifile.fail()) return error("Read failure.");

    // read entries
    deque<XXXENTRY> itemlist;
    for(uint32 i = 0; i < n_entries; i++) {
        XXXENTRY item;
        item.p01 = BE_read_uint32(ifile);
        item.p02 = BE_read_uint32(ifile);
        item.p03 = BE_read_uint32(ifile);
        item.p04 = BE_read_uint32(ifile);
        if(ifile.fail()) return error("Read failure.");
        itemlist.push_back(item);
       }

    // read some more unknowns
    uint32 unk10 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
    uint32 unk11 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
    uint32 unk12 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
    uint32 unk13 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
    uint32 unk14 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
    uint32 unk15 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");
    uint32 unk16 = BE_read_uint32(ifile); if(ifile.fail()) return error("Read failure.");

    // process string table
    deque<XXXSTRINGENTRY> strtable1;
    uint32 n_strings1 = BE_read_uint32(ifile);
    for(uint32 i = 0; i < n_strings1; i++) {
        XXXSTRINGENTRY item;
        item.size = BE_read_uint32(ifile);
        if(!read_string(ifile, &item.name[0], item.size)) return error("Read failure.");
        item.p01 = BE_read_uint32(ifile);
        item.p02 = BE_read_uint32(ifile);
        item.p03 = BE_read_uint32(ifile);
        item.p04 = BE_read_uint32(ifile);
        strtable1.push_back(item);
       }

    deque<XXXSTRINGENTRY> strtable2;
    uint32 n_strings2 = BE_read_uint32(ifile);
    for(uint32 i = 0; i < n_strings2; i++) {
        XXXSTRINGENTRY item;
        item.size = BE_read_uint32(ifile);
        if(!read_string(ifile, &item.name[0], item.size)) return error("Read failure.");
        item.p01 = BE_read_uint32(ifile);
        item.p02 = BE_read_uint32(ifile);
        item.p03 = BE_read_uint32(ifile);
        item.p04 = BE_read_uint32(ifile);
        strtable2.push_back(item);
       }

    // if there are compressed entries
    if(itemlist.size())
      {
       // create output file
       string ofname = pathname;
       ofname += shrtname;
       ofname += ".pak";
       ofstream ofile(ofname.c_str(), ios::binary);
       if(!ofile) return error("Failed to create output file.");

       // copy header
       BE_write_uint32(ofile, magic);
       if(ofile.fail()) return error("Write failure.");

       BE_write_uint32(ofile, filesig);
       if(ofile.fail()) return error("Write failure.");

       BE_write_uint32(ofile, headersize);
       if(ofile.fail()) return error("Write failure.");

       BE_write_uint32(ofile, unk02);
       if(ofile.fail()) return error("Write failure.");

       BE_write_uint32(ofile, unk03);
       if(ofile.fail()) return error("Write failure.");

       // copy string
       BE_write_uint32(ofile, unk04);
       if(ofile.fail()) return error("Write failure.");
       if(!write_string(ofile, unk05)) return error("Write failure.");

       BE_write_uint16(ofile, unk06);
       if(ofile.fail()) return error("Write failure.");

       BE_write_uint16(ofile, unk07);
       if(ofile.fail()) return error("Write failure.");

       // save property count
       BE_write_uint32(ofile, property_count);
       if(ofile.fail()) return error("Write failure.");

       // save property offset
       BE_write_uint32(ofile, property_offset);
       if(ofile.fail()) return error("Write failure.");

       // save 0x44 bytes
       for(uint32 i = 0; i < 0x11; i++) {
           BE_write_uint32(ofile, unk09[i]);
           if(ofile.fail()) return error("Write failure.");
          }

       // save two 0x00
       BE_write_uint32(ofile, 0x00);
       BE_write_uint32(ofile, 0x00);

       // save more unknowns
       BE_write_uint32(ofile, unk10); if(ofile.fail()) return error("Write failure.");
       BE_write_uint32(ofile, unk11); if(ofile.fail()) return error("Write failure.");
       BE_write_uint32(ofile, unk12); if(ofile.fail()) return error("Write failure.");
       BE_write_uint32(ofile, unk13); if(ofile.fail()) return error("Write failure.");
       BE_write_uint32(ofile, unk14); if(ofile.fail()) return error("Write failure.");
       BE_write_uint32(ofile, unk15); if(ofile.fail()) return error("Write failure.");
       BE_write_uint32(ofile, unk16); if(ofile.fail()) return error("Write failure.");

       // save string table size
       BE_write_uint32(ofile, (uint32)strtable1.size());
       if(ofile.fail()) return error("Write failure.");

       // save string table #1
       for(uint32 i = 0; i < strtable1.size(); i++) {
           const XXXSTRINGENTRY& item = strtable1[i];
           BE_write_uint32(ofile, item.size);
           write_string(ofile, item.name);
           BE_write_uint32(ofile, item.p01);
           BE_write_uint32(ofile, item.p02);
           BE_write_uint32(ofile, item.p03);
           BE_write_uint32(ofile, item.p04);
          }

       // save string table size
       BE_write_uint32(ofile, (uint32)strtable2.size());
       if(ofile.fail()) return error("Write failure.");

       // save string table #2
       for(uint32 i = 0; i < strtable2.size(); i++) {
           const XXXSTRINGENTRY& item = strtable2[i];
           BE_write_uint32(ofile, item.size);
           write_string(ofile, item.name);
           BE_write_uint32(ofile, item.p01);
           BE_write_uint32(ofile, item.p02);
           BE_write_uint32(ofile, item.p03);
           BE_write_uint32(ofile, item.p04);
          }

       // process entries
       for(uint32 i = 0; i < itemlist.size(); i++)
          {
           // nothing to save
           if(!itemlist[i].p04) continue;
      
           // seek data
           ifile.seekg(itemlist[i].p03);
           if(ifile.fail()) return error("Seek failure.");
      
           // decompress data
           if(!processBIN(ifile, ofile)) return false;
          }
      }
    else
      {
       message(" Warning: Uncompressed 0x0031023D XXX.");
      }
   }
 else if(filesig == 0x00020000)
   {
    // create output file
    string ofname = pathname;
    ofname += shrtname;
    ofname += ".pak";
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");

    // move back to beginning
    ifile.seekg(0);
    if(ifile.fail()) return error("Seek failure.");

    // decompress data
    if(!processBIN(ifile, ofile))
       return false;
   }
 else
    message(" Warning: XXX is not an 0x00020000 XXX or 0x0031023D XXX.");

 return true;
}

bool processPAK(const string& filename)
{
 struct PAKHEADER {
  uint32 magic;
  uint32 signature;
  uint32 size;
  uint32 erok;
  uint32 unk01;
  uint32 namelen;
  char name[1024];
  uint16 unk02;
  uint16 unk03;
  uint32 properties;
  uint32 propoffset;
  uint32 imports;
  uint32 imports_offset;
  uint32 exports;
  uint32 exports_offset;
  uint32 unk04; // number of items (points to a bunch of 0's)
  uint32 unk05; // offset (points to a bunch of 0's)
  uint32 unk06; // 0x00000000
  uint32 start_offset; // should be same as header size
  uint32 unk07;
  uint32 unk08;
  uint32 unk09;
  uint32 unk10[4]; // GUID
  uint32 unk11[2]; // 0xA2D, 0xE8
  uint32 unk12[3]; // 0x00, 0x00, 0x00
  uint32 unk13[4]; // GUID
  uint32 unk14;
  uint32 unk15;
  deque<XXXEXPORT> exportlist;
 };
 PAKHEADER header;

 // open file
 ifstream ifile(filename.c_str(), ios::binary);
 if(!ifile) return error("File open failure.");

 // filename properties
 string pathname = GetPathnameFromFilename(filename);
 string shrtname = GetShortFilenameWithoutExtension(filename);

 // debug file
 stringstream ss;
 ss << pathname << shrtname << ".txt";
 ofstream dfile(ss.str().c_str());

 // read magic
 header.magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.magic != 0x9E2A83C1) return error("Invalid magic number.");

 // read file signature
 header.signature = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.signature != 0x0031023D) return error("Invalid PAK file.");

 // read header size
 header.size = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read EROK
 header.erok = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.erok != 0x45524F4B) return error("Invalid PAK file. EROK expected.");

 // read 0x42
 header.unk01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.unk01 != 0x42) return error("Invalid PAK file. 0x42 expected.");

 // read string
 header.namelen = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(!read_string(ifile, &header.name[0], header.namelen)) return error("Read failure.");

 // read unknown
 header.unk02 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown
 header.unk03 = BE_read_uint16(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read property count
 header.properties = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read property offset
 header.propoffset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read import? count
 header.imports = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read import? offset
 header.imports_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read export? count
 header.exports = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read export? offset
 header.exports_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown count
 header.unk04 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown offset
 header.unk05 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown (0x00000000)
 header.unk06 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.unk06 != 0x00) return error("Invalid PAK file. 0x00 expected.");

 // read starting offset
 header.start_offset = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.start_offset == 0x00) return error("Invalid PAK file starting offset.");

 // read unknown (0, 1, 2, ...)
 header.unk07 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown (0, 1, 2, ...)
 header.unk08 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown (0, 1, 2, ...)
 header.unk09 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknowns
 header.unk10[0] = BE_read_uint32(ifile);
 header.unk10[1] = BE_read_uint32(ifile);
 header.unk10[2] = BE_read_uint32(ifile);
 header.unk10[3] = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknowns
 header.unk11[0] = BE_read_uint32(ifile);
 header.unk11[1] = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.unk11[0] != 0x0A2D) return error("Invalid PAK file. 0x0A2D expected.");
 if(header.unk11[1] != 0x00E8) return error("Invalid PAK file. 0x00E8 expected.");

 // read unknowns
 // for compressed files: the number of string tables and compressed entries
 header.unk12[0] = BE_read_uint32(ifile);
 header.unk12[1] = BE_read_uint32(ifile);
 header.unk12[3] = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(header.unk12[0] != 0x0000) return error("Invalid PAK file. 0x00 expected.");
 if(header.unk12[1] != 0x0000) return error("Invalid PAK file. 0x00 expected.");
 if(header.unk12[1] != 0x0000) return error("Invalid PAK file. 0x00 expected.");

 // read unknowns (GUID?)
 header.unk13[0] = BE_read_uint32(ifile);
 header.unk13[1] = BE_read_uint32(ifile);
 header.unk13[2] = BE_read_uint32(ifile);
 header.unk13[3] = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown
 header.unk14 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unknown
 header.unk15 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read string table #1
 deque<XXXSTRINGENTRY> strtable1;
 uint32 n_strings1 = BE_read_uint32(ifile);
 dfile << "There are 0x" << setfill('0') << setw(4) << hex << n_strings1 << dec << " string entries." << endl;
 for(uint32 i = 0; i < n_strings1; i++) {
     XXXSTRINGENTRY item;
     item.size = BE_read_uint32(ifile);
     if(!read_string(ifile, &item.name[0], item.size)) return error("Read failure.");
     item.p01 = BE_read_uint32(ifile);
     item.p02 = BE_read_uint32(ifile);
     item.p03 = BE_read_uint32(ifile);
     item.p04 = BE_read_uint32(ifile);
     strtable1.push_back(item);
     dfile << "0x" << setfill('0') << setw(4) << hex << i << dec << ": " << item.name << endl;
    }
 dfile << endl;

 // read string table #2
 deque<XXXSTRINGENTRY> strtable2;
 uint32 n_strings2 = BE_read_uint32(ifile);
 dfile << "There are 0x" << setfill('0') << setw(4) << hex << n_strings2 << dec << " string entries." << endl;
 for(uint32 i = 0; i < n_strings2; i++) {
     XXXSTRINGENTRY item;
     item.size = BE_read_uint32(ifile);
     if(!read_string(ifile, &item.name[0], item.size)) return error("Read failure.");
     item.p01 = BE_read_uint32(ifile);
     item.p02 = BE_read_uint32(ifile);
     item.p03 = BE_read_uint32(ifile);
     item.p04 = BE_read_uint32(ifile);
     strtable2.push_back(item);
     dfile << "0x" << setfill('0') << setw(4) << hex << i << dec << ": " << item.name << endl;
    }
 dfile << endl;

 //
 // PROPERTIES
 //

 dfile << "There are 0x" << setfill('0') << setw(4) << hex << header.properties << dec << " properties." << endl;

 // move to properties
 ifile.seekg(header.propoffset);
 if(ifile.fail()) return error("Seek failure.");

 // read properties
 deque<XXXPROPERTY> proplist;
 for(uint32 i = 0; i < header.properties; i++) {
     XXXPROPERTY item;
     item.size = BE_read_uint32(ifile);
     if(!read_string(ifile, &item.name[0], item.size)) return error("Read failure.");
     item.p01 = BE_read_uint16(ifile);
     item.p02 = BE_read_uint16(ifile);
     item.p03 = BE_read_uint32(ifile);
     proplist.push_back(item);
     dfile << "property 0x" << setfill('0') << setw(4) << hex << i << dec << ": " << item.name << " ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p01 << dec << " ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p02 << dec << " ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p03 << dec << " ";
     dfile << endl;
    }
 dfile << endl;

 //
 // EXPORTS
 //

 dfile << "-" << endl;
 dfile << "- EXPORT TABLE" << endl;
 dfile << "-" << endl;
 dfile << endl;

 // move to data
 ifile.seekg(header.exports_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read exports
 for(uint32 i = 0; i < header.exports; i++)
    {
     // read item
     XXXEXPORT item;
     item.p01 = BE_read_uint32(ifile);
     item.p02 = BE_read_uint32(ifile);
     item.p03 = BE_read_uint32(ifile);
     item.p04 = BE_read_uint32(ifile);
     item.p05 = BE_read_uint32(ifile);
     item.p06 = BE_read_uint32(ifile);
     item.p07 = BE_read_uint32(ifile);

     // save item
     header.exportlist.push_back(item);

     // debug information
     dfile << "export 0x" << setfill('0') << setw(4) << hex << i << dec << ": ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p01 << dec << " ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p02 << dec << " ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p03 << dec << " ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p04 << dec << " ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p05 << dec << " ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p06 << dec << " ";
     dfile << "0x" << setfill('0') << setw(8) << hex << item.p07 << dec << " ";
     dfile << endl;
    }
 dfile << endl;

 //
 // IMPORTS
 //

 deque<XXXIMPORT> importlist;

 // move to data
 ifile.seekg(header.imports_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read imports
 for(uint32 i = 0; i < header.imports; i++)
    {
     // read information
     XXXIMPORT item;
     if(!BE_read_array(ifile, &item.info[0], 17)) return error("Read failure.");

     // number of tuples
     item.size = 3*item.info[11];
     if(item.size > 0x100) return error("Unexpected number of import items.");

     // read tuples
     item.data.reset(new uint32[item.size]);
     if(!BE_read_array(ifile, item.data.get(), item.size)) return error("Read failure.");

     // save import item
     importlist.push_back(item);

     // save debug data
     dfile << "0x" << setfill('0') << setw(4) << hex << i << dec << ": ";
     for(uint32 j = 0; j < 17; j++)
         dfile << "0x" << setfill('0') << setw(8) << hex << item.info[j] << dec << " ";
     for(uint32 j = 0; j < item.info[11]; j++) {
         dfile << "(";
         dfile << "0x" << setfill('0') << setw(8) << hex << item.data[3*j + 0] << dec << " ";
         dfile << "0x" << setfill('0') << setw(8) << hex << item.data[3*j + 1] << dec << " ";
         dfile << "0x" << setfill('0') << setw(8) << hex << item.data[3*j + 2] << dec;
         dfile << ") ";
        }
     dfile << endl;
    }
 dfile << "at 0x" << hex << (uint32)ifile.tellg() << dec << endl;
 dfile << endl;

 // end of IMPORT table should be beginning of next section
 if(header.unk05 != (uint32)ifile.tellg())
    message("Ooops! Offset is wrong!");

 // create directory for this file
 string savepath = pathname;
 savepath += shrtname;
 savepath += "\\";
 CreateDirectoryA(savepath.c_str(), NULL);

 // for each import
 for(uint32 i = 0; i < header.imports; i++)
    {
     const XXXIMPORT& item = importlist[i];
     uint32 prop_ref = item.info[0x03];
     uint32 size = item.info[0x09];
     uint32 offs = item.info[0x0A];
     uint32 flags = item.info[0x07];

     // get property name
     if(!(prop_ref < proplist.size())) return error("Property reference out of bounds.");
     string propname = proplist[prop_ref].name;

     // move to offset
     ifile.seekg(offs);
     if(ifile.fail()) return error("Seek failure.");

     // read attributes (if any)
     deque<XXXATTRIBUTE> attrlist;
     bool success = readAttributeList(ifile, proplist, item.info[0x05], flags, attrlist);
     if(!success) {
        cout << "Processing import 0x" << hex << i << dec << "." << endl;
        return false;
       }

     // for each attribute
     for(uint32 i = 0; i < (uint32)attrlist.size(); i++)
        {
        }

     // // read first four bytes (either type reference or some 0xFFFFFFFXX value)
     // uint32 type_ref = BE_read_uint32(ifile);
     // if(!(type_ref < proplist.size())) {
     //    dfile << "Property reference out of bounds ";
     //    dfile << "i = 0x" << hex << i << " type ref = " << type_ref << dec << endl;
     //    continue;
     //   }
     // string type_str = proplist[type_ref].name;
     // 
     // if(type_str == "JPEGCompressedAlpha")
     //   {
     //    cout << " FOUND JPG ALPHA" << endl;
     //    if(size != 0x88) message("JPEGCompressedAlpha information should be 0x88 bytes.");
     // 
     //    // read JPEG information
     //    uint32 datasize = size - 0x4;
     //    boost::shared_array<char> data(new char[datasize]);
     //    ifile.read(data.get(), datasize);
     //    if(ifile.fail()) return error("Read failure.");
     //    binary_stream bs(data, datasize);
     // 
     //    // process JPEG information
     //   }
     // else if(type_str == "JPEGCompressedColor")
     //   {
     //    cout << " FOUND JPG COLOR" << endl;
     //    if(size != 0x88) message("JPEGCompressedAlpha information should be 0x88 bytes.");
     // 
     //    // read JPEG information
     //    uint32 datasize = size - 0x4;
     //    boost::shared_array<char> data(new char[datasize]);
     //    ifile.read(data.get(), datasize);
     //    if(ifile.fail()) return error("Read failure.");
     //    binary_stream bs(data, datasize);
     // 
     //    // process JPEG information
     //   }

     // JPEG IMAGE
     // if(magic == 0x000000D0) 
     //   {
     //    // unknown
     //    BE_read_uint32(ifile);
     // 
     //    // size of data to read
     //    uint32 jpgsize = BE_read_uint32(ifile);
     // 
     //    cout << " FOUND JPG" << endl;
     //    
     //    // create file
     //    string ofname = savepath;
     //    ofname += propname;
     //    ofname += ".jpg";
     //    ofstream ofile(ofname.c_str(), ios::binary);
     //    if(!ofile) return error("Failed to create JPEG file.");
     //    
     //    // read data
     //    boost::shared_array<char> data(new char[jpgsize]);
     //    ifile.read(data.get(), jpgsize);
     //    if(ifile.fail()) return error("Read failure.");
     //    
     //    // save data
     //    ofile.write(data.get(), jpgsize);
     //    if(ofile.fail()) return error("Write failure.");
     //   }
    }

 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

};};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool extract(void)
{
 std::string pathname = GetModulePathname();
 return extract(pathname.c_str());
}

bool extract(const char* pname)
{
 // set pathname
 using namespace std;
 std::string pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname();

 // variables
 bool doXXX = false;
 bool doPAK = true;

 // process archive
 cout << "STAGE 1" << endl;
 if(doXXX) {
    cout << "Processing .XXX files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".XXX", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processXXX(filelist[i])) return false;
       }
    cout << endl;
   }

 // process archive
 cout << "STAGE 2" << endl;
 if(doPAK) {
    cout << "Processing .PAK files..." << endl;
    deque<string> filelist;
    BuildFilenameList(filelist, ".PAK", pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        cout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processPAK(filelist[i])) return false;
       }
    cout << endl;
   }

 return true;
}

}};

