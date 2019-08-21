#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_amc.h"
#include "x_dds.h"
#include "x_namco.h"
using namespace std;

// for vertex formats
#include "x_namco.inl"

void zero_small(real32& value)
{
 if(std::fabs(value) < 1.0e-4f) value = 0.0f;
}

//
// ARCHIVES (PAC)
//

namespace NAMCO {

STDSTRING GetExtensionFHM(boost::shared_array<char> data, uint32 size)
{
 // read magic number
 binary_stream bs(data, size);
 uint32 magic = bs.BE_read_uint32();
 if(bs.fail()) return STDSTRING(TEXT("unknown"));

 switch(magic) {
   case(0x42534446) : return STDSTRING(TEXT("BSDF")); // BSDF
   case(0x4253464F) : return STDSTRING(TEXT("BSFO")); // BSFO
   case(0x43484752) : return STDSTRING(TEXT("CHGR")); // CHGR
   case(0x2C435052) : return STDSTRING(TEXT("CPRM")); // CPRM
   case(0x4446434C) : return STDSTRING(TEXT("DFCL")); // DFCL
   case(0x444D4D59) : return STDSTRING(TEXT("DMMY")); // DMMY
   case(0x444E534F) : return STDSTRING(TEXT("DNSO")); // DNSO
   case(0x45465020) : return STDSTRING(TEXT("EFP")); // EFP
   case(0x45494458) : return STDSTRING(TEXT("EIDX")); // EIDX
   case(0x46484D20) : return STDSTRING(TEXT("FHM")); // FHM
   case(0x4B504B50) : return STDSTRING(TEXT("KPKP")); // KPKP
   case(0x4C4D4200) : return STDSTRING(TEXT("LMB")); // LMB
   case(0x4E445033) : return STDSTRING(TEXT("NDP3")); // NDP3
   case(0x4E464800) : return STDSTRING(TEXT("NFH")); // NFH
   case(0x4E535033) : return STDSTRING(TEXT("NSP3")); // NSP3
   case(0x4E545033) : return STDSTRING(TEXT("NTP3")); // NTP3
   case(0x4E545842) : return STDSTRING(TEXT("NTXB")); // NTXB
   case(0x6E757363) : return STDSTRING(TEXT("NUSC")); // NUSC
   case(0x4F4D4F00) : return STDSTRING(TEXT("OMO")); // OMO
   case(0x89504E47) : return STDSTRING(TEXT("PNG")); // PNG
   case(0x52555445) : return STDSTRING(TEXT("RUTE")); // RUTE
   case(0x53544750) : return STDSTRING(TEXT("STGP")); // STGP
   case(0x53545050) : return STDSTRING(TEXT("STPP")); // STPP
   case(0x56424E20) : return STDSTRING(TEXT("VBN")); // VBN (bones)
   case(0x56434C20) : return STDSTRING(TEXT("VCL")); // VCL
   case(0x564F5400) : return STDSTRING(TEXT("VOT")); // VOT
   case(0x76737364) : return STDSTRING(TEXT("VSSD")); // VSSD
   case(0x56585330) : return STDSTRING(TEXT("VXS0")); // VXS0
   // special
   case(0xB2ACBCBA) : return STDSTRING(TEXT("BABB")); // BABB (lots of these)
   case(0x00020100) : return STDSTRING(TEXT("VAGL")); // VAGL
  }

 // try second number
 uint32 p02 = bs.BE_read_uint32();
 if(bs.fail()) return STDSTRING(TEXT("unknown"));

 switch(p02) {
   case(0x414C454F) : return STDSTRING(TEXT("OELA")); // OELA
 }

 // unknown
 return STDSTRING(TEXT("unknown"));
}

bool ExtractFHM(LPCTSTR filename)
{
 // keep deleting state
 bool do_delete = true;

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 uint32 h01 = BE_read_uint32(ifile);
 uint32 h02 = BE_read_uint32(ifile); // 0x01010010
 uint32 h03 = BE_read_uint32(ifile); // 0x00000000
 uint32 h04 = BE_read_uint32(ifile); // total filesize
 uint32 h05 = BE_read_uint32(ifile); // number of files
 
 // validate header
 if(h01 != 0x46484D20) return error("Expecting FHM.");
 if(h02 != 0x01010010) return error("Unexpected FHM version.");

 // nothing to do
 if(h05 == 0 && do_delete) {
    ifile.close();
    DeleteFile(filename);
    return true;
   }

 // read offsets
 deque<uint32> list1;
 for(uint32 i = 0; i < h05; i++)  {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     list1.push_back(item);
    }
 
 // read filesizes
 deque<uint32> list2;
 for(uint32 i = 0; i < h05; i++)  {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     list2.push_back(item);
    }

 // read filetypes
 deque<uint32> list3;
 for(uint32 i = 0; i < h05; i++)  {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     list3.push_back(item);
    }

 // create save directory
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // for each file
 for(uint32 i = 0; i < h05; i++)
    {
     // construct filename
     STDSTRINGSTREAM ss;
     ss << savepath;
     ss << setfill(TCHAR('0'));
     ss << setw(3) << i << ".";

     // file data
     boost::shared_array<char> data;
     uint32 size = 0;

     // FILE TYPE 0x00
     if(list3[i] == 0x00)
       {
        size = list2[i];
       }
     // FILE TYPE 0x01
     else if(list3[i] == 0x01)
       {
        // this is a strange one
        size = list2[i];
       }
     // FILE TYPE 0x02
     else if(list3[i] == 0x02)
       {
        size = list2[i];
       }
     // FILE TYPE 0x03
     else if(list3[i] == 0x03)
       {
        if(list2[i] != 0) return error("FHM file type 0x03 requires 0-length data.");
        size = h04 - list1[i]; // sometimes wrong

        // compute index to next largest offset (see 0192//0008.FHM)
        uint32 next_index = 0xFFFFFFFF;
        for(uint32 j = 0; j < h05; j++) {
            if(i == j) continue; // don't compare same index
            if(list1[j] < list1[i]) continue; // don't compare smaller offsets
            if(next_index == 0xFFFFFFFF) next_index = j;
            else if(list1[j] < list1[next_index]) next_index = j;
           }

        // compute size
        if(next_index == 0xFFFFFFFF) size = h04 - list1[i];
        else size = list1[next_index] - list1[i];

        // check size against filesize to be sure
        // this can happen because the next largest offset is aligned to 0x10
        // bytes and the data before it is shorter than that (i.e. it doesn't
        // doesn't come to the boundary and there is some padding between the
        // previous and next file)
        if(list1[i] + size > filesize)
           size = filesize - list1[i];
       }
     // FILE TYPE 0x04
     else if(list3[i] == 0x04)
       {
        size = list2[i];
       }
     // OTHERWISE
     else
        return error("Unknown FHM file type.");

     // read and save data (if possible)
     // ignore files beyond range... these are texture files and they have
     // already been taken care of
     if(size && (list1[i] < filesize))
       {
        // seek data
        ifile.seekg(list1[i]);
        if(ifile.fail()) return error("Seek failure.");        

        // read data
        data.reset(new char[size]);
        ifile.read(data.get(), size);
        if(ifile.fail()) return error("Read failure.");

        // determine file extension
        STDSTRING extension = GetExtensionFHM(data, size);
        ss << extension;

        // create output file
        ofstream ofile(ss.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create file.");

        // save data
        ofile.write(data.get(), size);
        if(ofile.fail()) return error("Write failure.");
       }
    }

 // delete file when done
 if(do_delete) {
    ifile.close();
    DeleteFile(filename);
   }

 return true;
}

bool ExtractPAC(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) {
    stringstream ss;
    ss << "Failed to open file " << Unicode16ToUnicode08(filename).get() << ".";
    return error(ss);
   }

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create save directory
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // keep track of current file and offset
 uint32 curr_file = 0;
 uint64 curr_offs = 0;

 // keep track of unknown data seen
 uint32 unknown_size = 0;

 // loop through file
 for(;;)
    {
     // read header (if possible)
     uint32 h01 = BE_read_uint32(ifile);
     if(ifile.fail()) break;

     // FHM_
     if(h01 == 0x46484D20)
       {
        // reset unknown size
        cout << "Processing file #" << curr_file << "." << endl;
        unknown_size = 0;

        // read header
        uint32 h02 = BE_read_uint32(ifile); // 0x01010010
        uint32 h03 = BE_read_uint32(ifile); // 0x00000000
        uint32 h04 = BE_read_uint32(ifile); // total filesize
        uint32 h05 = BE_read_uint32(ifile); // number of files
        
        // validate header
        if(h01 != 0x46484D20) return error("Expecting FHM.");
        if(h02 != 0x01010010) return error("Unexpected FHM version.");

        // read offsets
        deque<uint32> list1;
        for(uint32 i = 0; i < h05; i++)  {
            uint32 item = BE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");
            list1.push_back(item);
           }
        
        // read filesizes
        deque<uint32> list2;
        for(uint32 i = 0; i < h05; i++)  {
            uint32 item = BE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");
            list2.push_back(item);
           }

        // read filetypes
        deque<uint32> list3;
        for(uint32 i = 0; i < h05; i++)  {
            uint32 item = BE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");
            list3.push_back(item);
           }

        // compute datasize
        uint32 datasize = 0;
        if(h05)
          {
           // largest offset
           uint32 max_index = 0;
           for(uint32 i = 1; i < h05; i++) if(list1[max_index] < list1[i]) max_index = i;

           // compute total filesize
           datasize = list1[max_index] + list2[max_index];
           if(datasize < h04) datasize = h04;
          }
        else
           datasize = 0x20; // no files, should be FHM should be 0x20 bytes

        if(datasize)
          {
           // create output filename
           STDSTRINGSTREAM ss;
           ss << savepath << setfill(TCHAR('0')) << setw(4) << curr_file << TEXT(".FHM");
           ofstream ofile(ss.str().c_str(), ios::binary);
           if(!ofile) return error("Failed to create FHM file.");
           
           // move to current offset again
           ifile.seekg(curr_offs);
           if(ifile.fail()) return error("Seek failure.");
           
           // read data
           boost::shared_array<char> data(new char[datasize]);
           ifile.read(data.get(), datasize);
           if(ifile.fail()) return error("Read failure.");

           // write data
           ofile.write(data.get(), datasize);
           if(ofile.fail()) return error("Write failure.");
          }

        // move to next 0x10-byte aligned offset and increment file number
        curr_offs += datasize;
        curr_offs = ((curr_offs + 0x0F) & (~0x0F));
        curr_file++;
       }
     // NTP3
     else if(h01 == 0x4E545033)
       {
        // reset unknown size
        cout << "Processing file #" << curr_file << "." << endl;
        unknown_size = 0;

        // read header
        uint16 h02 = BE_read_uint16(ifile); // 0x0100
        uint16 h03 = BE_read_uint16(ifile); // number of files
        uint32 h04 = BE_read_uint32(ifile); // 0x00000
        uint32 h05 = BE_read_uint32(ifile); // 0x00000

        // keep track of filesize
        uint32 datasize = 0x10;

        // for each file
        for(uint32 i = 0; i < h03; i++)
           {
            // read size
            uint32 size = BE_read_uint32(ifile);
            if(ifile.fail()) return error("Read failure.");

            // increment filesize
            if(size < 0x10) return error("Invalid texture filesize.");
            datasize += size;

            // skip over data to next file
            ifile.seekg((size - 0x04), ios::cur);
           }

        // save data
        if(datasize)
          {
           // create output filename
           STDSTRINGSTREAM ss;
           ss << savepath << setfill(TCHAR('0')) << setw(4) << curr_file << TEXT(".NTP3");
           ofstream ofile(ss.str().c_str(), ios::binary);
           if(!ofile) return error("Failed to create FHM file.");
           
           // move to current offset again
           ifile.seekg(curr_offs);
           if(ifile.fail()) return error("Seek failure.");
           
           // read data
           boost::shared_array<char> data(new char[datasize]);
           ifile.read(data.get(), datasize);
           if(ifile.fail()) return error("Read failure.");

           // write data
           ofile.write(data.get(), datasize);
           if(ofile.fail()) return error("Write failure.");
          }

        // move to next offset and increment file number
        curr_offs += datasize;
        curr_file++;
       }
     else
       {
        // increment unknown size
        unknown_size += 0x10;
        if(unknown_size > 0x800) {
           cout << "curr_offs = 0x" << hex << curr_offs << dec << endl;
           return error("Unknown size is more than 0x800.");
          }

        // skip 0x10 bytes
        curr_offs += 0x10;
       }

     // move to current offset (if possible)
     ifile.seekg(curr_offs);
     if(ifile.fail()) break;
    }

 cout << "Number of files = " << curr_file << endl;
 return true;
}

};

//
// TEXTURES
//

namespace NAMCO {

bool ExtractNTP3(LPCTSTR filename, bool do_delete, bool rbo)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // read header
 auto h01 = BE_read_uint32(ifile); // NTP3
 auto h02 = BE_read_uint16(ifile); // 0x0100
 auto h03 = BE_read_uint16(ifile); // number of files
 auto h04 = BE_read_uint32(ifile); // 0x00
 auto h05 = BE_read_uint32(ifile); // 0x00

 // validate header
 if(!(h01 == 0x4E545033 || h01 == 0x4E545852)) return error("Invalid NTP3/NTXR file.");
 if(h03 == 0x00 && do_delete) {
    ifile.close();
    DeleteFile(filename);
    return true; // nothing to do
   }

 // create output directory
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("_t\\");
 CreateDirectory(savepath.c_str(), NULL);

 // for each image
 uint32 position = 0x10;
 for(uint32 i = 0; i < h03; i++)
    {
     // seek position
     ifile.seekg(position);
     if(ifile.fail()) return error("Seek failure.");

     // read image header
     auto total_size = BE_read_uint32(ifile);
     auto unk01 = BE_read_uint32(ifile);
     auto image_size = BE_read_uint32(ifile);
     auto ihead_size = BE_read_uint16(ifile);
     auto unk03 = BE_read_uint16(ifile);

     auto mipmaps = BE_read_uint16(ifile); // mipmaps + 1
     auto image_type = BE_read_uint16(ifile);
     auto dx = BE_read_uint16(ifile);
     auto dy = BE_read_uint16(ifile);
     auto unk04 = BE_read_uint32(ifile);
     auto unk05 = BE_read_uint32(ifile);

     // skip past data
     if(ihead_size == 0x50) {
        ifile.seekg(0x10, ios::cur);
        if(ifile.fail()) return error("Seek failure.");
       }
     else if(ihead_size == 0x60) {
        ifile.seekg(0x20, ios::cur);
        if(ifile.fail()) return error("Seek failure.");
       }
     else if(ihead_size == 0x70) {
        ifile.seekg(0x30, ios::cur);
        if(ifile.fail()) return error("Seek failure.");
       }
     else if(ihead_size == 0x80) {
        ifile.seekg(0x40, ios::cur);
        if(ifile.fail()) return error("Seek failure.");
       }

     auto ext01 = BE_read_uint32(ifile); // eXt
     auto ext02 = BE_read_uint32(ifile); // usually < 0x20
     auto ext03 = BE_read_uint32(ifile); // usually < 0x20
     auto ext04 = BE_read_uint32(ifile); // usually < 0x20

     auto gidx01 = BE_read_uint32(ifile); // GIDX
     auto gidx02 = BE_read_uint32(ifile); // usually < 0x10
     auto gidx03 = BE_read_uint32(ifile); // texture identifier
     auto gidx04 = BE_read_uint32(ifile); // 0x00

     // read image data
     if(!image_size) return error("Invalid image.");
     boost::shared_array<char> data(new char[image_size]);
     ifile.read(data.get(), image_size);

     // create output file
     STDSTRINGSTREAM ss;
     ss << savepath << setfill(TCHAR('0')) << setw(8) << hex << gidx03 << dec << TEXT(".dds");
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // DXT1 image
     if(image_type == 0x00)
       {
        // create DDS header
        DDS_HEADER ddsh;
        CreateDXT1Header(dx, dy, mipmaps - 1, FALSE, &ddsh);
        
        // reverse byte order of data
        if(rbo) {
           uint16* temp = reinterpret_cast<uint16*>(data.get());
           reverse_byte_order(temp, image_size/2);
          }
        
        // save DDS header
        ofile.write("DDS ", 4);
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write(data.get(), image_size);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(image_type == 0x01)
       {
        // create DDS header
        DDS_HEADER ddsh;
        CreateDXT3Header(dx, dy, mipmaps - 1, FALSE, &ddsh);
     
        // reverse byte order of data
        if(rbo) {
           uint16* temp = reinterpret_cast<uint16*>(data.get());
           reverse_byte_order(temp, image_size/2);
          }
     
        // save DDS header
        ofile.write("DDS ", 4);
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write(data.get(), image_size);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(image_type == 0x02)
       {
        // create DDS header
        DDS_HEADER ddsh;
        CreateDXT5Header(dx, dy, mipmaps - 1, FALSE, &ddsh);
     
        // reverse byte order of data
        if(rbo) {
           uint16* temp = reinterpret_cast<uint16*>(data.get());
           reverse_byte_order(temp, image_size/2);
          }
     
        // save DDS header
        ofile.write("DDS ", 4);
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write(data.get(), image_size);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(image_type == 0x07)
       {
        // PHOTOSHOP ONLY SUPPORTS ONE TYPE OF 32-BIT UNCOMPRESSED!
        // YOU MUST USE ARGB!
        uint32 m1 = 0xF800; // R
        uint32 m2 = 0x07E0; // G 
        uint32 m3 = 0x001F; // B
        uint32 m4 = 0x0000; // A

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, mipmaps - 1, 16, m1, m2, m3, m4, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)data.get(), image_size);
       }
     else if(image_type == 0x0E)
       {
        // PHOTOSHOP ONLY SUPPORTS ONE TYPE OF 32-BIT UNCOMPRESSED!
        // YOU MUST USE ARGB!
        uint32 m1 = 0xFF000000; // A
        uint32 m2 = 0x00FF0000; // R 
        uint32 m3 = 0x0000FF00; // G
        uint32 m4 = 0x000000FF; // B

        // swap channels so shit loads in photoshop
        uint32 n_pixels = dx * dy;
        for(uint32 j = 0; j < n_pixels; j++) {
            uint32 index1 = 4*j;
            uint32 index2 = index1 + 1;
            uint32 index3 = index2 + 1;
            uint32 index4 = index3 + 1;
            swap(data[index1], data[index4]); // swap A and B
            swap(data[index2], data[index3]); // swap R and G
           }

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, mipmaps - 1, 32, m2, m3, m4, m1, FALSE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)data.get(), image_size);
       }
     // ABGR CUBEMAPS
     else if(image_type == 0x11)
       {
        // PHOTOSHOP ONLY SUPPORTS ONE TYPE OF 32-BIT UNCOMPRESSED!
        // YOU MUST USE ARGB!
        uint32 m1 = 0xFF000000; // A
        uint32 m2 = 0x00FF0000; // R 
        uint32 m3 = 0x0000FF00; // G
        uint32 m4 = 0x000000FF; // B

        // swap channels so shit loads in photoshop
        uint32 n_pixels = dx * dy;
        for(uint32 j = 0; j < n_pixels; j++) {
            uint32 index1 = 4*j;
            uint32 index2 = index1 + 1;
            uint32 index3 = index2 + 1;
            uint32 index4 = index3 + 1;
            swap(data[index1], data[index4]); // swap A and B
            swap(data[index2], data[index3]); // swap R and G
           }

        // save texture
        DDS_HEADER ddsh;
        CreateUncompressedDDSHeader(dx, dy, mipmaps - 1, 32, m2, m3, m4, m1, TRUE, &ddsh);
        DWORD signature = 0x20534444;
        ofile.write((char*)&signature, sizeof(signature));
        ofile.write((char*)&ddsh, sizeof(ddsh));
        ofile.write((char*)data.get(), image_size);
       }
     else if(image_type == 0x13) // TTT2
       {
        cout << "SKIPPING IMAGE TYPE 0x13" << endl;
        do_delete = false;
       }
     else if(image_type == 0x14) // TTT2
       {
        cout << "SKIPPING IMAGE TYPE 0x14" << endl;
        do_delete = false;
       }
     else {
        stringstream ss;
        ss << "Unknown image format 0x" << hex << image_type << dec << endl;
        return error(ss);
       }

     // increment position
     position += total_size;
    }

 // delete file
 if(do_delete) {
    ifile.close();
    DeleteFile(filename);
   }

 return true;
}

bool PathExtractNTP3(bool do_delete, uint32 start, bool rbo)
{
 return PathExtractNTP3(NULL, do_delete, start, rbo);
}

bool PathExtractNTP3(LPCTSTR pathname, bool do_delete, uint32 start, bool rbo)
{
 // set pathname
 STDSTRING pname(pathname);
 if(!pname.length()) pname = GetModulePathname().get();

 // build filename list
 cout << "Searching for .NTP3 files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".NTP3"), pname.c_str())) return true;
 
 // process NTP3 files
 bool break_on_errors = true;
 cout << "Processing .NTP3 files..." << endl;
 for(size_t i = start; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractNTP3(filelist[i].c_str(), do_delete, rbo)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

};

//
// MODELS (NDP3)
//

namespace NAMCO {

bool ExtractNDP3(LPCTSTR filename, MATERIALFUNCTION mf, bool do_delete)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug file
 bool debug = true;
 ofstream dfile;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT(".debug");
    dfile.open(ss.str().c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // read header 0x00 - 0x0F
 uint32 h01 = BE_read_uint32(ifile); // NDP3
 uint32 h02 = BE_read_uint32(ifile); // filesize
 uint16 h03 = BE_read_uint16(ifile); // 0x200 (GEFB, SCLS) 0x201 (TR)
 uint16 h04 = BE_read_uint16(ifile); // number of 0x30-byte mesh info entries
 uint16 h05 = BE_read_uint16(ifile); // unknown (sum these to get number of bones in base body)
 uint16 h06 = BE_read_uint16(ifile); // unknown (sum these to get number of bones in base body)
 // read header 0x10 - 0x1F
 uint32 h07 = BE_read_uint32(ifile); // size of mesh information
 uint32 h08 = BE_read_uint32(ifile); // size of idx buffer
 uint32 h09 = BE_read_uint32(ifile); // size of vtx buffer #1
 uint32 h10 = BE_read_uint32(ifile); // size of vtx buffer #2
 // read header 0x20 - 0x2F
 real32 h11 = BE_read_real32(ifile);
 real32 h12 = BE_read_real32(ifile);
 real32 h13 = BE_read_real32(ifile);
 real32 h14 = BE_read_real32(ifile);

 // debug output
 if(debug) {
    dfile << "------------" << endl;
    dfile << "-- HEADER --" << endl;
    dfile << "------------" << endl;
    dfile << endl;
    dfile << "h01: 0x" << hex << h01 << dec << endl;
    dfile << "h02: 0x" << hex << h02 << dec << endl;
    dfile << "h03: 0x" << hex << h03 << dec << endl;
    dfile << "h04: 0x" << hex << h04 << dec << endl;
    dfile << "h05: 0x" << hex << h05 << dec << endl;
    dfile << "h06: 0x" << hex << h06 << dec << endl;
    dfile << "h07: 0x" << hex << h07 << dec << endl;
    dfile << "h08: 0x" << hex << h08 << dec << endl;
    dfile << "h09: 0x" << hex << h09 << dec << endl;
    dfile << "h10: 0x" << hex << h10 << dec << endl;
    dfile << "h11: " << h11 << " ???" << endl;
    dfile << "h12: " << h12 << " ???" << endl;
    dfile << "h13: " << h13 << " ???" << endl;
    dfile << "h14: " << h14 << " ???" << endl;
    dfile << endl;
   }

 // validate header
 if(!(h01 == 0x4E445033 || h01 == 0x4E445852)) return error("Invalid NDP3/NDXR.");
 if(!(h03 == 0x200 || h03 == 0x201)) return error("Invalid NDP3. Conversion failed.");

 // read mesh information
 struct MESHINFOENTRY1 {
  real32 p01[8]; // bounding boxes
  uint32 p02;    // offset into mesh names
  uint32 p03;    // 0x2C
  uint16 p04;    // blendindex
  uint16 p05;    // number of parts
  uint32 p06;    // absolute offset to PARTINFOENTRY array
 };
 deque<MESHINFOENTRY1> entrylist1;
 for(uint32 i = 0; i < h04; i++) {
     MESHINFOENTRY1 item;
     BE_read_array(ifile, &item.p01[0], 8);
     item.p02 = BE_read_uint32(ifile);
     item.p03 = BE_read_uint32(ifile);
     item.p04 = BE_read_uint16(ifile);
     item.p05 = BE_read_uint16(ifile);
     item.p06 = BE_read_uint32(ifile);
     entrylist1.push_back(item);
    }

 // debug output
 if(debug) {
    dfile << "----------------------" << endl;
    dfile << "-- MESH INFORMATION --" << endl;
    dfile << "----------------------" << endl;
    dfile << endl;
    for(uint32 i = 0; i < h04; i++) {
        dfile << "MESH 0x" << i << endl;
        dfile << entrylist1[i].p01[0] << " unknown vector" << endl;
        dfile << entrylist1[i].p01[1] << " " << endl;
        dfile << entrylist1[i].p01[2] << " " << endl;
        dfile << entrylist1[i].p01[3] << " " << endl;
        dfile << entrylist1[i].p01[4] << " unknown vector " << endl;
        dfile << entrylist1[i].p01[5] << "  " << endl;
        dfile << entrylist1[i].p01[6] << "  " << endl;
        dfile << entrylist1[i].p01[7] << "  " << endl;
        dfile << "0x" << hex << entrylist1[i].p02 << dec << "  offset into mesh names" << endl;
        dfile << "0x" << hex << entrylist1[i].p03 << dec << "  0x2C" << endl;
        dfile << "0x" << hex << entrylist1[i].p04 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist1[i].p05 << dec << "  number of parts" << endl;
        dfile << "0x" << hex << entrylist1[i].p06 << dec << "  offset to parts" << endl;
        dfile << endl;
       }
   }

 // 00000000 - 00000000 - 00000000 - 0558 - 1312 - 00000210 - 00000288 - 00000330 - 000003D8 - 0851 - 0004 - 00000000 - 00000000 - 00000000
 // 000010A2 - 00002AC0 - 00020100 - 0038 - 1312 - 00000450 - 000004C8 - 00000540 - 000005B8 - 0049 - 0004 - 00000000 - 00000000 - 00000000
 // 00001134 - 00002C80 - 00021600 - 0136 - 1312 - 00000630 - 000006A8 - 00000750 - 000007F8 - 01E7 - 0004 - 00000000 - 00000000 - 00000000
 // 00001502 - 00003630 - 00028A40 - 07B8 - 1312 - 00000870 - 000008E8 - 00000990 - 00000A38 - 0C08 - 0004 - 00000000 - 00000000 - 00000000
 // 00002D12 - 000073F0 - 00056F40 - 093F - 1312 - 00000AB0 - 00000B28 - 00000BD0 - 00000C78 - 0D57 - 0004 - 00000000 - 00000000 - 00000000
 // 000047C0 - 0000BDE8 - 0008E6E0 - 0025 - 1312 - 00000CF0 - 00000D68 - 00000E10 - 00000EB8 - 0065 - 0004 - 00000000 - 00000000 - 00000000
 // 0000488A - 0000BF10 - 0008F4C0 - 02AC - 1312 - 00000F30 - 00000FA8 - 00001050 - 000010F8 - 036D - 0004 - 00000000 - 00000000 - 00000000
 // 00004F64 - 0000D470 - 0009F540 - 06FC - 1312 - 00001170 - 000011E8 - 00001290 - 00001338 - 0AED - 0004 - 00000000 - 00000000 - 00000000
 // 0000653E - 00010C50 - 000C93C0 - 037C - 1312 - 000013B0 - 00001428 - 000014D0 - 00001578 - 0504 - 0004 - 00000000 - 00000000 - 00000000
 // OK         OK         OK         OK     OK     OK         OK         OK         OK         OK     OK
 // read part information
 struct PARTINFOENTRY {
  uint32 mesh_id;
  uint32 part_id;
  uint32 p01; // 0x00: offset into ib
  uint32 p02; // 0x04: offset into vb #1
  uint32 p03; // 0x08: offset into vb #2
  uint16 p04; // 0x0C: number of vertices
  uint16 p05; // 0x0E: vertex format
  uint32 p06; // 0x10: absolute offset into material property data
  uint32 p07; // 0x14: zero? 
  uint32 p08; // 0x18: zero?
  uint32 p09; // 0x1C: zero?
  uint16 p10; // 0x1E: number of indices
  uint16 p11; // 0x22: index buffer format
  uint32 p12; // 0x24: zero?
  uint32 p13; // 0x28: zero?
  uint32 p14; // 0x2C: zero?
 };
 deque<PARTINFOENTRY> entrylist2;
 for(uint32 i = 0; i < entrylist1.size(); i++)
    {
     // seek offset
     uint32 offset = entrylist1[i].p06;
     ifile.seekg(offset);

     // for each entry #2
     uint16 n_item = entrylist1[i].p05;
     for(uint32 j = 0; j < n_item; j++)
        {
         // read entry
         PARTINFOENTRY item;
         item.mesh_id = i;
         item.part_id = j;
         item.p01 = BE_read_uint32(ifile);
         item.p02 = BE_read_uint32(ifile);
         item.p03 = BE_read_uint32(ifile);
         item.p04 = BE_read_uint16(ifile);
         item.p05 = BE_read_uint16(ifile);
         item.p06 = BE_read_uint32(ifile);
         item.p07 = BE_read_uint32(ifile);
         item.p08 = BE_read_uint32(ifile);
         item.p09 = BE_read_uint32(ifile);
         item.p10 = BE_read_uint16(ifile);
         item.p11 = BE_read_uint16(ifile);
         item.p12 = BE_read_uint32(ifile);
         item.p13 = BE_read_uint32(ifile);
         item.p14 = BE_read_uint32(ifile);

         // save entry
         entrylist2.push_back(item);
        }
    }

 // debug output
 if(debug) {
    dfile << "----------------------" << endl;
    dfile << "-- PART INFORMATION --" << endl;
    dfile << "----------------------" << endl;
    dfile << endl;
    for(uint32 i = 0; i < entrylist2.size(); i++) {
        dfile << "0x" << hex << entrylist2[i].mesh_id << dec << "  mesh index" << endl; 
        dfile << "0x" << hex << entrylist2[i].part_id << dec << "  part index" << endl;
        dfile << "0x" << hex << entrylist2[i].p01 << dec << "  offset into index buffer" << endl;
        dfile << "0x" << hex << entrylist2[i].p02 << dec << "  offset into vertex buffer #1" << endl;
        dfile << "0x" << hex << entrylist2[i].p03 << dec << "  offset into vertex buffer #2" << endl;
        dfile << "0x" << hex << entrylist2[i].p04 << dec << "  number of vertices" << endl;
        dfile << "0x" << hex << entrylist2[i].p05 << dec << "  vertex format" << endl;
        dfile << "0x" << hex << entrylist2[i].p06 << dec << "  offset into material property data" << endl;
        dfile << "0x" << hex << entrylist2[i].p07 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p08 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p09 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p10 << dec << "  number of indices" << endl;
        dfile << "0x" << hex << entrylist2[i].p11 << dec << "  index buffer format" << endl;
        dfile << "0x" << hex << entrylist2[i].p12 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p13 << dec << "  unknown" << endl;
        dfile << "0x" << hex << entrylist2[i].p14 << dec << "  unknown" << endl;
        uint32 matdatasize = 0x00;
        if((i + 1) == entrylist2.size()) matdatasize = (h07 + 0x30) - entrylist2[i].p06;
        else matdatasize = entrylist2[i + 1].p06 - entrylist2[i].p06;
        dfile << "0x" << hex << matdatasize << dec << " material data size" << endl;
        dfile << endl;
       }
   }

 // read material information
 struct MESHINFOENTRY3 {
  boost::shared_array<char> data;
  uint32 size;
  MATERIALINFO minfo;
 };
 deque<MESHINFOENTRY3> entrylist3;
 for(uint32 i = 0; i < entrylist2.size(); i++)
    {
     // seek offset
     uint32 offset = entrylist2[i].p06;
     ifile.seekg(offset);

     // compute item size
     MESHINFOENTRY3 item;
     if((i + 1) == entrylist2.size()) item.size = (h07 + 0x30) - offset;
     else item.size = entrylist2[i + 1].p06 - offset;

     // read item
     item.data.reset(new char[item.size]);
     ifile.read(item.data.get(), item.size);
     if(ifile.fail()) return error("Read failure.");

     // process material function
     if(mf)
       {
        if(!((*mf)(binary_stream(item.data, item.size), item.minfo)))
           return false;
       }
     // process material
     else
       {
        // convert item to binary stream
        binary_stream bs(item.data, item.size);
        uint32 id = bs.BE_read_uint32();
        if(bs.fail()) return error("Stream read failure.", __LINE__);
        
        // read diffuse texture ID (always at 0x20-byte offset)
        bs.seek(0x20);
        uint32 texture_id = bs.BE_read_uint32();
        if(bs.fail()) return error("Stream read failure.", __LINE__);

        // set material
        item.minfo.list[0] = std::pair<uint32, uint32>(texture_id, NAMCO_DIFFUSE);
       }

     // insert item
     entrylist3.push_back(item);
    }

 //
 // PHASE #
 // TEXTURE FILENAME PROCESSING
 //

 // model container
 ADVANCEDMODELCONTAINER amc;

 // construct a set of filenames
 map<uint32, uint32> filemap;
 uint32 fmidx = 0;
 for(uint32 i = 0; i < entrylist3.size(); i++) {
     auto id0 = entrylist3[i].minfo.list[0];
     auto id1 = entrylist3[i].minfo.list[1];
     auto id2 = entrylist3[i].minfo.list[2];
     auto id3 = entrylist3[i].minfo.list[3];
     auto id4 = entrylist3[i].minfo.list[4];
     auto id5 = entrylist3[i].minfo.list[5];
     auto id6 = entrylist3[i].minfo.list[6];
     auto id7 = entrylist3[i].minfo.list[7];
     if(id0.second != NAMCO_INVALID && filemap.find(id0.first) == filemap.end()) filemap.insert(map<uint32, uint32>::value_type(id0.first, fmidx++));
     if(id1.second != NAMCO_INVALID && filemap.find(id1.first) == filemap.end()) filemap.insert(map<uint32, uint32>::value_type(id1.first, fmidx++));
     if(id2.second != NAMCO_INVALID && filemap.find(id2.first) == filemap.end()) filemap.insert(map<uint32, uint32>::value_type(id2.first, fmidx++));
     if(id3.second != NAMCO_INVALID && filemap.find(id3.first) == filemap.end()) filemap.insert(map<uint32, uint32>::value_type(id3.first, fmidx++));
     if(id4.second != NAMCO_INVALID && filemap.find(id4.first) == filemap.end()) filemap.insert(map<uint32, uint32>::value_type(id4.first, fmidx++));
     if(id5.second != NAMCO_INVALID && filemap.find(id5.first) == filemap.end()) filemap.insert(map<uint32, uint32>::value_type(id5.first, fmidx++));
     if(id6.second != NAMCO_INVALID && filemap.find(id6.first) == filemap.end()) filemap.insert(map<uint32, uint32>::value_type(id6.first, fmidx++));
     if(id7.second != NAMCO_INVALID && filemap.find(id7.first) == filemap.end()) filemap.insert(map<uint32, uint32>::value_type(id7.first, fmidx++));
    }

 // set size of filelist
 amc.iflist.resize(filemap.size());

 // for each file in the filemap
 for(auto iter = filemap.begin(); iter != filemap.end(); iter++) {
     stringstream sstex;
     sstex << hex << setfill('0') << setw(8) << iter->first << dec << ".dds";
     amc.iflist[iter->second].filename = sstex.str();
    }

 //
 // PHASE #
 // READ INDEX BUFFER
 //

 // move to idx buffer
 uint32 ib_offset = 0x30 + h07;
 ifile.seekg(ib_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read idx buffer
 uint32 ib_size = h08;
 if(!ib_size) return error("Invalid index buffer.");
 boost::shared_array<char> ib_data(new char[ib_size]);
 ifile.read(ib_data.get(), ib_size);
 if(ifile.fail()) return error("Read failure.");

 //
 // PHASE #
 // READ VERTEX BUFFERS
 //

 // move to vtx buffer #1
 uint32 vb1_offset = ib_offset + ib_size;
 ifile.seekg(vb1_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vtx buffer #1
 uint32 vb1_size = h09;
 boost::shared_array<char> vb1_data;
 if(vb1_size) {
    vb1_data.reset(new char[vb1_size]);
    ifile.read(vb1_data.get(), vb1_size);
    if(ifile.fail()) return error("Read failure.");
   }

 // move to vtx buffer #2
 uint32 vb2_offset = vb1_offset + vb1_size;
 ifile.seekg(vb2_offset);
 if(ifile.fail()) return error("Seek failure.");

 // read vtx buffer #2
 uint32 vb2_size = h10;
 boost::shared_array<char> vb2_data;
 if(vb2_size) {
    vb2_data.reset(new char[vb2_size]);
    ifile.read(vb2_data.get(), vb2_size);
    if(ifile.fail()) return error("Read failure.");
   } 

 // BEGIN DEBUG
 if(debug) {
    dfile << "-------------------------------" << endl;
    dfile << "-- VERTEX BUFFER INFORMATION --" << endl;
    dfile << "-------------------------------" << endl;
    dfile << endl;
    dfile << "vertex buffer #1 offset: 0x" << hex << vb1_offset << dec << endl;
    dfile << "vertex buffer #2 offset: 0x" << hex << vb2_offset << dec << endl;
    dfile << endl;
   }
 // END DEBUG

 //
 // PHASE #
 // PROCESS SURFACES
 //

 // binary streams
 binary_stream ibs(ib_data, ib_size);
 binary_stream vs1(vb1_data, vb1_size);
 binary_stream vs2(vb2_data, vb2_size);

 // for each mesh entry
 for(uint32 i = 0; i < entrylist2.size(); i++)
    {
     // important information
     uint32 ib_offset = entrylist2[i].p01; // offset into ib
     uint32 v1_offset = entrylist2[i].p02; // offset into vb #1
     uint32 v2_offset = entrylist2[i].p03; // offset into vb #2
     uint16 n_vertices = entrylist2[i].p04; // number of vertices
     uint16 vb_format = entrylist2[i].p05; // vertex format
     uint32 mat_index = entrylist2[i].p06; // offset into material property data
     uint32 n_indices = entrylist2[i].p10; // number of indices
     uint16 ib_format = entrylist2[i].p11; // index buffer format

     // initialize vertex buffer
     AMC_VTXBUFFER vb;
     vb.flags = AMC_VERTEX_POSITION | AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
     vb.name = "default";
     vb.uvchan = 1;
     vb.uvtype[0] = AMC_DIFFUSE_MAP;
     vb.uvtype[1] = AMC_INVALID_MAP;
     vb.uvtype[2] = AMC_INVALID_MAP;
     vb.uvtype[3] = AMC_INVALID_MAP;
     vb.uvtype[4] = AMC_INVALID_MAP;
     vb.uvtype[5] = AMC_INVALID_MAP;
     vb.uvtype[6] = AMC_INVALID_MAP;
     vb.uvtype[7] = AMC_INVALID_MAP;
     vb.colors = 0;
     vb.elem = n_vertices;
     vb.data.reset(new AMC_VERTEX[vb.elem]);

     // single-weight blending      
     if(entrylist1[entrylist2[i].mesh_id].p04 != 0xFFFF) {
        vb.flags |= AMC_VERTEX_WEIGHTS;
        for(uint32 j = 0; j < n_vertices; j++) {
            // single blendindex
            vb.data[j].wi[0] = entrylist1[entrylist2[i].mesh_id].p04;
            vb.data[j].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            // single blendweight
            vb.data[j].wv[0] = 1.0f;
            vb.data[j].wv[1] = 0.0f;
            vb.data[j].wv[2] = 0.0f;
            vb.data[j].wv[3] = 0.0f;
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }

     // TYPE 0x0010
     // 0x14 BYTES PER VERTEX
     if(vb_format == 0x0010)
       {
        // remove normal flags
        vb.flags &= ~AMC_VERTEX_NORMAL;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vs1.BE_read_real32();
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // texcoord
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // texcoord
           }
       }
     // TYPE 0x0012
     // 0x18 BYTES PER VERTEX
     else if(vb_format == 0x0012)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].vw = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_sint08()/127.0f; // normal (should verify)
            vb.data[j].ny = vs1.BE_read_sint08()/127.0f; // normal (should verify)
            vb.data[j].nz = vs1.BE_read_sint08()/127.0f; // normal (should verify)
            vb.data[j].nw = vs1.BE_read_sint08()/127.0f; // normal (should verify)
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV
           }
       }
     // TYPE 0x0020
     // 0x18 BYTES PER VERTEX
     else if(vb_format == 0x0020)
       {
        // no normals
        vb.flags &= ~AMC_VERTEX_NORMAL;

        // extra UV-channel
        vb.uvchan = 2;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].vw = vs1.BE_read_real32(); // position
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[1][0] = vs1.BE_read_real16(); // UV2
            vb.data[j].uv[1][1] = vs1.BE_read_real16(); // UV2
           }
       }
     // TYPE 0x0022
     // 0x1C BYTES PER VERTEX
     else if(vb_format == 0x0022)
       {
        // no normals
        vb.flags &= ~AMC_VERTEX_NORMAL;

        // extra UV-channel
        vb.uvchan = 2;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].vw = vs1.BE_read_real32(); // position
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[1][0] = vs1.BE_read_real16(); // UV2
            vb.data[j].uv[1][1] = vs1.BE_read_real16(); // UV2
           }
       }
     // TYPE 0x0110
     // 0x24 BYTES PER VERTEX
     else if(vb_format == 0x0110)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].vw = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real32(); // normal
            vb.data[j].ny = vs1.BE_read_real32(); // normal
            vb.data[j].nz = vs1.BE_read_real32(); // normal
            vb.data[j].nw = vs1.BE_read_real32(); // normal
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV
           }
       }
     // TYPE 0x0112
     // 0x28 BYTES PER VERTEX
     else if(vb_format == 0x0112)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real32(); // normal
            vb.data[j].ny = vs1.BE_read_real32(); // normal
            vb.data[j].nz = vs1.BE_read_real32(); // normal
            vs1.BE_read_real32(); // -1 (might be part of normal)
            vs1.BE_read_real32(); // +1
            vs1.BE_read_uint32(); // 0xFFFFFFFF
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV
           }
       }
     // TYPE 0x0610
     // 0x18 BYTES PER VERTEX
     else if(vb_format == 0x0610)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV
           }
       }
     // TYPE 0x0612
     // 0x1C BYTES PER VERTEX
     else if(vb_format == 0x0612)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV
           }
       }
     // TYPE 0x0614
     // 0x20 BYTES PER VERTEX
     // EXAMPLE: [WiiU] Super Smash Bros\content\output\ui\model\diffi\dyr_background_set\model.nud
     else if(vb_format == 0x0614)
       {
        // extra UV-channel
        vb.colors = 1;
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vb.data[j].color[0][0] = vs1.BE_read_real16(); // color?
            vb.data[j].color[0][1] = vs1.BE_read_real16();
            vb.data[j].color[0][2] = vs1.BE_read_real16();
            vb.data[j].color[0][3] = vs1.BE_read_real16();
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV
           }
       }
     // TYPE 0x0620
     // 0x1C BYTES PER VERTEX
     else if(vb_format == 0x0620)
       {
        // extra UV-channel
        vb.uvchan = 2;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[1][0] = vs1.BE_read_real16(); // UV2
            vb.data[j].uv[1][1] = vs1.BE_read_real16(); // UV2
           }
       }
     // TYPE 0x0622
     // 0x20 BYTES PER VERTEX
     else if(vb_format == 0x0622)
       {
        // extra UV-channel
        vb.uvchan = 2;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // texcoord1
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // texcoord1
            vb.data[j].uv[1][0] = vs1.BE_read_real16(); // texcoord2
            vb.data[j].uv[1][1] = vs1.BE_read_real16(); // texcoord2
           }
       }
     // TYPE 0x0632
     // [WiiU] Super Smash Bros\content\output\ui\result\background_model\model.nud
     // EXAMPLE: 0x24 BYTES PER VERTEX
     else if(vb_format == 0x0632)
       {
        // extra UV-channel
        vb.uvchan = 3;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;
        vb.uvtype[2] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vs1.BE_read_uint08(); // unknown
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // texcoord1
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // texcoord1
            vb.data[j].uv[1][0] = vs1.BE_read_real16(); // texcoord2
            vb.data[j].uv[1][1] = vs1.BE_read_real16(); // texcoord2
            vb.data[j].uv[2][0] = vs1.BE_read_real16(); // texcoord3
            vb.data[j].uv[2][1] = vs1.BE_read_real16(); // texcoord3
           }
       }
     else if(vb_format == 0x0710)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }
       }
     else if(vb_format == 0x0712)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08(); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }
       }
     // TYPE 0x0714
     // 0x30 BYTES PER VERTEX
     // EXAMPLE: [WiiU] Super Smash Bros\content\output\ui\model\order\result_order_final\stc_Sky_set\model.nud
     else if(vb_format == 0x0714)
       {
        // extra UV-channel
        vb.colors = 1;
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            // position
            vb.data[j].vx = vs1.BE_read_real32();
            vb.data[j].vy = vs1.BE_read_real32();
            vb.data[j].vz = vs1.BE_read_real32();
            // normal
            vb.data[j].nx = vs1.BE_read_real16();
            vb.data[j].ny = vs1.BE_read_real16();
            vb.data[j].nz = vs1.BE_read_real16();
            vb.data[j].nw = vs1.BE_read_real16();
            // tangent?
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            // binormal?
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            // color?
            vb.data[j].color[0][0] = vs1.BE_read_real16();
            vb.data[j].color[0][1] = vs1.BE_read_real16();
            vb.data[j].color[0][2] = vs1.BE_read_real16();
            vb.data[j].color[0][3] = vs1.BE_read_real16();
            // UVs
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }
       }

     // TYPE 0x0720
     // 0x2C BYTES PER VERTEX
     else if(vb_format == 0x0720)
       {
        // extra UV-channel
        vb.uvchan = 2;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[1][0] = vs1.BE_read_real16(); // UV2
            vb.data[j].uv[1][1] = vs1.BE_read_real16(); // UV2
           }
       }
     // TYPE 0x0722
     // 0x30 BYTES PER VERTEX
     else if(vb_format == 0x0722)
       {
        // extra UV-channel
        vb.uvchan = 2;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32(); // position
            vb.data[j].vz = vs1.BE_read_real32(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_uint08(); // 0xFF
            vs1.BE_read_uint08(); // 0xFF
            vs1.BE_read_uint08(); // 0xFF
            vs1.BE_read_uint08(); // 0xFF
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[1][0] = vs1.BE_read_real16(); // UV2
            vb.data[j].uv[1][1] = vs1.BE_read_real16(); // UV2
           }
       }
     // TYPE 0x0732
     // 0x34 BYTES PER VERTEX
     else if(vb_format == 0x0732)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // set UV channels
        vb.uvchan = 3;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;
        vb.uvtype[2] = AMC_DIFFUSE_MAP;

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {            
            vb.data[j].vx = vs1.BE_read_real32(); // position
            vb.data[j].vy = vs1.BE_read_real32();
            vb.data[j].vz = vs1.BE_read_real32();
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16();
            vb.data[j].nz = vs1.BE_read_real16();
            vb.data[j].nw = vs1.BE_read_real16();
            vs1.BE_read_real16(); // tangent?
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            vs1.BE_read_real16(); // binormal?
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            vs1.BE_read_real16();
            vs1.BE_read_uint08(); // color?
            vs1.BE_read_uint08();
            vs1.BE_read_uint08();
            vs1.BE_read_uint08();
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV1
            vb.data[j].uv[1][0] = vs1.BE_read_real16(); // UV2
            vb.data[j].uv[1][1] = vs1.BE_read_real16(); // UV2
            vb.data[j].uv[2][0] = vs1.BE_read_real16(); // UV3
            vb.data[j].uv[2][1] = vs1.BE_read_real16(); // UV3
           }
       }
     // TYPE 0x0812
     // 0x18 BYTES PER VERTEX
     // NOTE! CHECK FILE AS I ACCIDENTALLY DELETED THIS BEFORE TESTING!
     // C:\Users\semory\Desktop\TEMP\Soul Calibur Lost Swords\stage\0001\0000\0002.NDP3
     else if(vb_format == 0x0812)
       {
        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs1.BE_read_real16(); // position
            vb.data[j].vy = vs1.BE_read_real16(); // position
            vb.data[j].vz = vs1.BE_read_real16(); // position
            vb.data[j].vw = vs1.BE_read_real16(); // position
            vb.data[j].nx = vs1.BE_read_real16(); // normal
            vb.data[j].ny = vs1.BE_read_real16(); // normal
            vb.data[j].nz = vs1.BE_read_real16(); // normal
            vb.data[j].nw = vs1.BE_read_real16(); // normal
            vs1.BE_read_uint08(); // unknown vector
            vs1.BE_read_uint08(); // unknown vector
            vs1.BE_read_uint08(); // unknown vector
            vs1.BE_read_uint08(); // unknown vector
            vb.data[j].uv[0][0] = vs1.BE_read_real16(); // UV
            vb.data[j].uv[0][1] = vs1.BE_read_real16(); // UV
           }
       }

     // TYPE 0x1010
     // 0x30 BYTES PER VERTEX
     else if(vb_format == 0x1010)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.flags &= ~AMC_VERTEX_NORMAL;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32();
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }
     // TYPE 0x1012
     // 0x30 BYTES PER VERTEX
     else if(vb_format == 0x1012)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.flags &= ~AMC_VERTEX_NORMAL;
        vb.colors = 1;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].color[0][0] = (vs1.BE_read_uint08()/255.0f);
            vb.data[j].color[0][1] = (vs1.BE_read_uint08()/255.0f);
            vb.data[j].color[0][2] = (vs1.BE_read_uint08()/255.0f);
            vb.data[j].color[0][3] = (vs1.BE_read_uint08()/255.0f);
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32(); // ???
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }
     else if(vb_format == 0x1110)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32(); // ???
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }
     else if(vb_format == 0x1112)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.colors = 1;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].color[0][0] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][1] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][2] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][3] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32(); // ???
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }
     // new to 1.02 update
     // data062\AC1_0002\017.NDP3
     else if(vb_format == 0x1114)
       {
        // UV map data is 0x0C bytes per vertex
        // vertex data is 0x40 bytes per vertex

        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.colors = 1;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            // color
            vb.data[j].color[0][0] = vs1.BE_read_real16(); // color?
            vb.data[j].color[0][1] = vs1.BE_read_real16(); // color?
            vb.data[j].color[0][2] = vs1.BE_read_real16(); // color?
            vb.data[j].color[0][3] = vs1.BE_read_real16(); // color?
            // UV
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32(); // ???
            vb.data[j].nx = vs2.BE_read_real32(); // normal
            vb.data[j].ny = vs2.BE_read_real32(); // normal
            vb.data[j].nz = vs2.BE_read_real32(); // normal
            vs2.BE_read_real32(); // ???
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }
     else if(vb_format == 0x1310)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32();
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;

            // BLENDING EXAMPLE #1
            //  0000000E 00000011 00000034 00000038 ( 14  17  52  56)
            //  3ECCCCCD 3ECCCCCD 3DCCCCCD 3DCCCCCD (0.4 0.4 0.1 0.1) = 1.0
            // BLENDING EXAMPLE #2
            //  0000000E 00000034 00000001 00000001 (  14   52 1 1)
            //  3F47AE14 3E6147AE 00000000 00000000 (0.78 0.22 0 0) = 1.0
           }
       }
     else if(vb_format == 0x1312)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.colors = 1;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].color[0][0] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][1] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][2] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][3] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            // these numbers can be large
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vb.data[j].vw = vs2.BE_read_real32(); // 0x42C00000
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }
     else if(vb_format == 0x1322)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.colors = 1;

        // extra UV-channel
        vb.uvchan = 2;
        vb.uvtype[1] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].color[0][0] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][1] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][2] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][3] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
            vb.data[j].uv[1][0] = vs1.BE_read_real16();
            vb.data[j].uv[1][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vs2.BE_read_real32(); // 0x42C00000
            vb.data[j].nx = vs2.BE_read_real32(); // normal?
            vb.data[j].ny = vs2.BE_read_real32(); // normal?
            vb.data[j].nz = vs2.BE_read_real32(); // normal?
            vb.data[j].nw = vs2.BE_read_real32(); // normal?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // tangent?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vs2.BE_read_real32(); // binormal?
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice?
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight?
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }
     // TYPE 0x1712
     // 0x44 BYTES PER VERTEX
     // EXAMPLE: [WiiU] Super Smash Bros\content\worldsmash\model\mii\model.nud
     else if(vb_format == 0x1712)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.colors = 1;

        // single UV-channel
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].color[0][0] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][1] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][2] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][3] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position 0x00
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vb.data[j].nx = vs2.BE_read_real16(); // normal 0x0C
            vb.data[j].ny = vs2.BE_read_real16(); // normal
            vb.data[j].nz = vs2.BE_read_real16(); // normal
            vb.data[j].nw = vs2.BE_read_real16(); // normal
            vs2.BE_read_real16(); // unknown #1 0x14
            vs2.BE_read_real16(); // unknown #1
            vs2.BE_read_real16(); // unknown #1
            vs2.BE_read_real16(); // unknown #1
            vs2.BE_read_real16(); // unknown #2 0x1C
            vs2.BE_read_real16(); // unknown #2
            vs2.BE_read_real16(); // unknown #2
            vs2.BE_read_real16(); // unknown #2
            vb.data[j].wi[0] = vs2.BE_read_uint32(); // blendindice 0x24
            vb.data[j].wi[1] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint32(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_real32(); // blendweight 0x34
            vb.data[j].wv[1] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[2] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[3] = vs2.BE_read_real32(); // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f;
           }
       }
     // TYPE 0x4012
     // 0x18 BYTES PER VERTEX
     // EXAMPLE: [WiiU] Super Smash Bros\content\output\ui\model\diffi\dyr_background_set\model.nud
     else if(vb_format == 0x4012)
       {
        // optional flags
        vb.flags |= AMC_VERTEX_WEIGHTS;
        vb.flags &= ~AMC_VERTEX_NORMAL;
        vb.colors = 1;

        // single UV-channel
        vb.uvchan = 1;
        vb.uvtype[0] = AMC_DIFFUSE_MAP;

        // seek vertex data
        vs1.seek(v1_offset);
        if(vs1.fail()) return error("Stream seek failure.", __LINE__);

        // read UV map
        for(uint32 j = 0; j < n_vertices; j++) {
            vb.data[j].color[0][0] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][1] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][2] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].color[0][3] = (vs1.BE_read_uint08()/255.0f); // color?
            vb.data[j].uv[0][0] = vs1.BE_read_real16();
            vb.data[j].uv[0][1] = vs1.BE_read_real16();
           }

        // seek vertex data
        vs2.seek(v2_offset);
        if(vs2.fail()) return error("Stream seek failure.", __LINE__);

        // read vertices
        for(uint32 j = 0; j < n_vertices; j++)
           {
            vb.data[j].vx = vs2.BE_read_real32(); // position 0x00
            vb.data[j].vy = vs2.BE_read_real32(); // position
            vb.data[j].vz = vs2.BE_read_real32(); // position
            vb.data[j].vw = vs2.BE_read_real32(); // position
            vb.data[j].wi[0] = vs2.BE_read_uint08(); // blendindice 0x14
            vb.data[j].wi[1] = vs2.BE_read_uint08(); // blendindice
            vb.data[j].wi[2] = vs2.BE_read_uint08(); // blendindice
            vb.data[j].wi[3] = vs2.BE_read_uint08(); // blendindice
            vb.data[j].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
            vb.data[j].wv[0] = vs2.BE_read_uint08()/255.0f; // blendweight 0x18
            vb.data[j].wv[1] = vs2.BE_read_uint08()/255.0f; // blendweight
            vb.data[j].wv[2] = vs2.BE_read_uint08()/255.0f; // blendweight
            vb.data[j].wv[3] = vs2.BE_read_uint08()/255.0f; // blendweight
            vb.data[j].wv[4] = 0.0f;
            vb.data[j].wv[5] = 0.0f;
            vb.data[j].wv[6] = 0.0f;
            vb.data[j].wv[7] = 0.0f; 
           }
       }

     else if(vb_format == 0x1612) namco_fvf_1612(vb, vs1, vs2, v1_offset, v2_offset);
     else if(vb_format == 0x4610) namco_fvf_4610(vb, vs1, vs2, v1_offset, v2_offset);
     else if(vb_format == 0x4612) namco_fvf_4612(vb, vs1, vs2, v1_offset, v2_offset);
     else if(vb_format == 0x4622) namco_fvf_4622(vb, vs1, vs2, v1_offset, v2_offset);
     else if(vb_format == 0x4632) namco_fvf_4632(vb, vs1, vs2, v1_offset, v2_offset);
     else if(vb_format == 0x4710) namco_fvf_4710(vb, vs1, vs2, v1_offset, v2_offset);
     else if(vb_format == 0x4712) namco_fvf_4712(vb, vs1, vs2, v1_offset, v2_offset);
     else if(vb_format == 0x4722) namco_fvf_4722(vb, vs1, vs2, v1_offset, v2_offset);

     else {
        stringstream ss;
        ss << "Unknown vertex format 0x" << hex << vb_format << dec << endl;
        return error(ss);
       }

     // initialize index buffer
     AMC_IDXBUFFER ib;
     ib.format = AMC_UINT16;
     ib.type = AMC_TRIANGLES;
     ib.wind = AMC_CW;
     ib.reserved = 0;
     ib.name = "default";
     ib.elem = n_indices;

     // seek index buffer data
     ibs.seek(ib_offset);
     if(ibs.fail()) return error("Stream seek failure.", __LINE__);

     // read index buffer data
     ib.data.reset(new char[n_indices*sizeof(uint16)]);
     ibs.BE_read_array(reinterpret_cast<uint16*>(ib.data.get()), n_indices);
     if(ibs.fail()) return error("Stream read failure.", __LINE__);

     // TRI-STRIP WITH CUT INDICES
     if(ib_format == 0x00 || ib_format == 0x04)
       {       
        // convert triangle strip cut to triangle list
        AMCTriangleList trilist;
        if(!ConvertStripCutToTriangleList(reinterpret_cast<uint16*>(ib.data.get()), n_indices, trilist, (uint16)0xFFFF))
           return error("Failed to convert triangle strip to triangle list.", __LINE__);
        
        // adjust and insert index buffer
        ib.elem = trilist.n_indices;
        ib.data = trilist.data;
       }
     // TRIANGLE LIST
     else if(ib_format == 0x4000 || ib_format == 0x4004)
       {
        // no need to do anything
       }
     // UNKNOWN INDEX BUFFER FORMAT
     else
        return error("Unknown index buffer format.");

     // insert vertex and index buffer
     amc.vblist.push_back(vb);
     amc.iblist.push_back(ib);

     // create surface name
     stringstream mns;
     mns << Unicode16ToUnicode08(shrtname.c_str()).get() << "_";
     mns << "m" << setfill('0') << setw(3) << entrylist2[i].mesh_id << "_";
     mns << "p" << setfill('0') << setw(2) << entrylist2[i].part_id;

     // initialize surface
     AMC_SURFACE sur;
     sur.name = mns.str();
     AMC_REFERENCE ref;
     ref.vb_index = i;
     ref.vb_start = 0;
     ref.vb_width = n_vertices;
     ref.ib_index = i;
     ref.ib_start = 0;
     ref.ib_width = ib.elem;
     ref.jm_index = AMC_INVALID_JMAP_INDEX;
     sur.refs.push_back(ref);
     sur.surfmat = i;

     // insert surface
     amc.surfaces.push_back(sur);

     // create surface material
     AMC_SURFMAT mat;
     mat.name = mns.str();
     mat.twoside = 0;
     mat.unused1 = 0;
     mat.unused2 = 0;
     mat.unused3 = 0;
     mat.basemap = AMC_INVALID_TEXTURE;
     mat.specmap = AMC_INVALID_TEXTURE;
     mat.tranmap = AMC_INVALID_TEXTURE;
     mat.bumpmap = AMC_INVALID_TEXTURE;
     mat.normmap = AMC_INVALID_TEXTURE;
     mat.lgthmap = AMC_INVALID_TEXTURE;
     mat.envimap = AMC_INVALID_TEXTURE;
     mat.glssmap = AMC_INVALID_TEXTURE;
     mat.resmap1 = AMC_INVALID_TEXTURE;
     mat.resmap2 = AMC_INVALID_TEXTURE;
     mat.resmap3 = AMC_INVALID_TEXTURE;
     mat.resmap4 = AMC_INVALID_TEXTURE;
     mat.resmap5 = AMC_INVALID_TEXTURE;
     mat.resmap6 = AMC_INVALID_TEXTURE;
     mat.resmap7 = AMC_INVALID_TEXTURE;
     mat.resmap8 = AMC_INVALID_TEXTURE;
     mat.basemapchan = 0xFF;
     mat.specmapchan = 0xFF;
     mat.tranmapchan = 0xFF;
     mat.bumpmapchan = 0xFF;
     mat.normmapchan = 0xFF;
     mat.lghtmapchan = 0xFF;
     mat.envimapchan = 0xFF;
     mat.glssmapchan = 0xFF;
     mat.resmapchan1 = 0xFF;
     mat.resmapchan2 = 0xFF;
     mat.resmapchan3 = 0xFF;
     mat.resmapchan4 = 0xFF;
     mat.resmapchan5 = 0xFF;
     mat.resmapchan6 = 0xFF;
     mat.resmapchan7 = 0xFF;
     mat.resmapchan8 = 0xFF;

     // modify material     
     if(entrylist3[i].minfo.list[0].second == NAMCO_DIFFUSE) {
        mat.basemap = filemap.find(entrylist3[i].minfo.list[0].first)->second;
        mat.basemapchan = 0;
       }
     else if(entrylist3[i].minfo.list[1].second == NAMCO_DIFFUSE) {
        mat.basemap = filemap.find(entrylist3[i].minfo.list[1].first)->second;
        mat.basemapchan = 0;
       }
     else if(entrylist3[i].minfo.list[2].second == NAMCO_DIFFUSE) {
        mat.basemap = filemap.find(entrylist3[i].minfo.list[2].first)->second;
        mat.basemapchan = 0;
       }
     else if(entrylist3[i].minfo.list[3].second == NAMCO_DIFFUSE) {
        mat.basemap = filemap.find(entrylist3[i].minfo.list[3].first)->second;
        mat.basemapchan = 0;
       }
     else if(entrylist3[i].minfo.list[4].second == NAMCO_DIFFUSE) {
        mat.basemap = filemap.find(entrylist3[i].minfo.list[4].first)->second;
        mat.basemapchan = 0;
       }
     // UNFORTUNATELY WE CAN'T DO THIS WITH THE SMC FORMAT BECAUSE WE NEED TO BE
     // ABLE TO SUPPORT MULTI-CHANNEL TEXTURES! MULTIPLE COLOR MAPS ETC.
     // if(entrylist3[i].minfo.norm_id != 0xFFFFFFFF) {
     //    mat.normmap = filemap.find(entrylist3[i].minfo.norm_id)->second;
     //    mat.normmapchan = 0;
     //   }
     // if(entrylist3[i].minfo.spec_id != 0xFFFFFFFF) {
     //    mat.specmap = filemap.find(entrylist3[i].minfo.spec_id)->second;
     //    mat.specmapchan = 0;
     //   }

     // insert material
     amc.surfmats.push_back(mat);
    }

 SaveAMC(pathname.c_str(), shrtname.c_str(), amc);
 SaveOBJ(pathname.c_str(), shrtname.c_str(), amc);

 // delete file
 if(do_delete) {
    ifile.close();
    DeleteFile(filename);
   }

 return true;
}

bool PathExtractNDP3(MATERIALFUNCTION mf, bool do_delete, uint32 start, LPCTSTR ext)
{
 return false; // PathExtractNDP3(NULL, do_delete, start, ext);
}

bool PathExtractNDP3(LPCTSTR pathname, MATERIALFUNCTION mf, bool do_delete, uint32 start, LPCTSTR ext)
{
 // set pathname
 STDSTRING pname(pathname);
 if(!pname.length()) pname = GetModulePathname().get();

 // build filename list
 wcout << "Searching for ." << ext << " files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, ext, pname.c_str())) return true;

 // process NTP3 files
 bool break_on_errors = true;
 wcout << "Processing ." << ext << " files..." << endl;
 for(size_t i = start; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractNDP3(filelist[i].c_str(), mf, do_delete)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

};

//
// SKELETONS (VBN)
//

namespace NAMCO {

bool ExtractVBN(LPCTSTR filename, bool do_delete)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug file
 bool debug = false;
 ofstream dfile;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT(".debug");
    dfile.open(ss.str().c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x56424E20) return error("Not a VBN file.");

 // read header
 uint16 h01 = BE_read_uint16(ifile); // 0x0001 (no matrices) or 0x0002 (matrices)
 uint16 h02 = BE_read_uint16(ifile); // 0x0000
 uint32 h03 = BE_read_uint32(ifile); // ??????
 uint32 h04 = BE_read_uint32(ifile); // total number of bones
 uint32 h05 = BE_read_uint32(ifile); // number of bones A
 uint32 h06 = BE_read_uint32(ifile); // number of bones B
 uint32 h07 = BE_read_uint32(ifile); // ??????
 uint32 h08 = BE_read_uint32(ifile); // ??????

 // validate header
 if(!h04) return true;
 uint32 n_joints = h04;

 // for each bone
 boost::shared_array<uint32> parentlist(new uint32[n_joints]);
 for(uint32 i = 0; i < n_joints; i++)
    {
     // string: 0x10 bytes (name)
     char name[0x10];
     ifile.read(&name[0], 0x10);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // uint32: 0x04 bytes (part)
     uint32 part = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);

     // uint32: 0x04 bytes (parent)
     auto p1 = BE_read_uint16(ifile);
     auto p2 = BE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
     parentlist[i] = (p2 == 0xFFFF ? AMC_INVALID_JOINT : p2);
    }

 // read positions 1
 if(debug) dfile << "FLOAT SET #1" << endl;
 typedef cs::math::vector3D<real32> vector_t;
 boost::shared_array<vector_t> coords1(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     coords1[i][0] = BE_read_real32(ifile);
     coords1[i][1] = BE_read_real32(ifile);
     coords1[i][2] = BE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
     if(debug) dfile << "pos " << i << ": " << coords1[i][0] << ", " << coords1[i][1] << ", " << coords1[i][2] << endl;
    }
 if(debug) dfile << endl;

 // read positions 2
 if(debug) dfile << "FLOAT SET #2" << endl;
 boost::shared_array<vector_t> coords2(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     coords2[i][0] = BE_read_real32(ifile);
     coords2[i][1] = BE_read_real32(ifile);
     coords2[i][2] = BE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
     if(debug) dfile << coords2[i][0] << ", " << coords2[i][1] << ", " << coords2[i][2] << endl;
    }
 if(debug) dfile << endl;

 // read positions 3
 if(debug) dfile << "FLOAT SET #3" << endl;
 boost::shared_array<vector_t> coords3(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     coords3[i][0] = BE_read_real32(ifile);
     coords3[i][1] = BE_read_real32(ifile);
     coords3[i][2] = BE_read_real32(ifile);
     if(ifile.fail()) return error("Read failure.", __LINE__);
     if(debug) dfile << coords3[i][0] << ", " << coords3[i][1] << ", " << coords3[i][2] << endl;
    }
 if(debug) dfile << endl;

 // move to next 0x10-aligned position
 uint32 position = (uint32)ifile.tellg();
 position = ((position + 0x0F) & (~0x0F));
 ifile.seekg(position);
 if(ifile.fail()) return error("Seek failure.", __LINE__);

 // matrix data
 typedef cs::math::matrix4x4<real32> matrix_t;
 boost::shared_array<matrix_t> mset1(new matrix_t[n_joints]);
 boost::shared_array<matrix_t> mset2(new matrix_t[n_joints]);

 // read matrix 1
 if(h01 == 0x0002) {
    if(debug) dfile << "MATRIX SET #1" << endl;
    for(uint32 i = 0; i < n_joints; i++) {
        BE_read_array(ifile, &mset1[i][0], 0x10);
        if(ifile.fail()) return error("Read failure.", __LINE__);
       }
    if(debug) dfile << endl;
   }

 // read matrix 2
 if(h01 == 0x0002) {
    if(debug) dfile << "MATRIX SET #2" << endl;
    for(uint32 i = 0; i < n_joints; i++) {
        BE_read_array(ifile, &mset2[i][0], 0x10);
        if(ifile.fail()) return error("Read failure.", __LINE__);
       }
    if(debug) dfile << endl;
   }

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = Unicode16ToUnicode08(shrtname.c_str()).get();
 skel.name += "_SKELETON";
 skel.tiplen = 1.0f;

 // for each joint
 for(uint32 i = 0; i < n_joints; i++)
    {
     // convert euler to quaternion
     // note: angles are in reverse order in the file!
     cs::math::quaternion<real32> q;
     cs::math::rotation_quaternion_ZYX(&q[0], coords2[i][2], coords2[i][1], coords2[i][0]);
     cs::math::normalize(q);

     // convert quaternion to matrix
     cs::math::matrix4x4<real32> m;
     cs::math::quaternion_to_matrix4x4(&q[0], &m[0]);

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;

     // create joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = parentlist[i];
     if(h01 == 0x02) {
        cs::math::matrix4x4<real32> temp(mset1[i].get());
        temp.transpose();
        joint.m_rel[0x0] = temp[0x0];
        joint.m_rel[0x1] = temp[0x1];
        joint.m_rel[0x2] = temp[0x2];
        joint.m_rel[0x3] = 0.0f;
        joint.m_rel[0x4] = temp[0x4];
        joint.m_rel[0x5] = temp[0x5];
        joint.m_rel[0x6] = temp[0x6];
        joint.m_rel[0x7] = 0.0f;
        joint.m_rel[0x8] = temp[0x8];
        joint.m_rel[0x9] = temp[0x9];
        joint.m_rel[0xA] = temp[0xA];
        joint.m_rel[0xB] = 0.0f;
        joint.m_rel[0xC] = temp[0xC];
        joint.m_rel[0xD] = temp[0xD];
        joint.m_rel[0xE] = temp[0xE];
        joint.m_rel[0xF] = 1.0f;
        joint.p_rel[0] = temp[0x3];
        joint.p_rel[1] = temp[0x7];
        joint.p_rel[2] = temp[0xB];
        joint.p_rel[3] = 1.0f;
       }
     else {
        joint.m_rel[0x0] = m[0x0];
        joint.m_rel[0x1] = m[0x1];
        joint.m_rel[0x2] = m[0x2];
        joint.m_rel[0x3] = m[0x3];
        joint.m_rel[0x4] = m[0x4];
        joint.m_rel[0x5] = m[0x5];
        joint.m_rel[0x6] = m[0x6];
        joint.m_rel[0x7] = m[0x7];
        joint.m_rel[0x8] = m[0x8];
        joint.m_rel[0x9] = m[0x9];
        joint.m_rel[0xA] = m[0xA];
        joint.m_rel[0xB] = m[0xB];
        joint.m_rel[0xC] = m[0xC];
        joint.m_rel[0xD] = m[0xD];
        joint.m_rel[0xE] = m[0xE];
        joint.m_rel[0xF] = m[0xF];
        joint.p_rel[0] = coords1[i][0];
        joint.p_rel[1] = coords1[i][1];
        joint.p_rel[2] = coords1[i][2];
        joint.p_rel[3] = 1.0f;
       }

     // insert joint
     skel.joints.push_back(joint);
    }

 // create model container
 ADVANCEDMODELCONTAINER amc;
 amc.skllist2.push_back(skel);
 SaveAMC(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

bool PathExtractVBN(bool do_delete, uint32 start)
{
 return PathExtractVBN(NULL, do_delete, start);
}

bool PathExtractVBN(LPCTSTR pathname, bool do_delete, uint32 start)
{
 // set pathname
 STDSTRING pname(pathname);
 if(!pname.length()) pname = GetModulePathname().get();

 // build filename list
 cout << "Searching for .VBN files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".VBN"), pname.c_str())) return true;
 
 // process NTP3 files
 bool break_on_errors = true;
 cout << "Processing .VBN files..." << endl;
 for(size_t i = start; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractVBN(filelist[i].c_str(), do_delete)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

bool ExtractBonesNMD(LPCTSTR filename, bool do_delete)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // create debug file
 bool debug = false;
 ofstream dfile;
 if(debug) {
    STDSTRINGSTREAM ss;
    ss << pathname << shrtname << TEXT(".debug");
    dfile.open(ss.str().c_str());
    if(!dfile) return error("Failed to create debug file.");
   }

 // read magic
 uint08 m01 = BE_read_uint08(ifile); // N
 uint08 m02 = BE_read_uint08(ifile); // M
 uint08 m03 = BE_read_uint08(ifile); // D
 uint08 m04 = BE_read_uint08(ifile); // unknown - possibly type of NMD file?
 if(ifile.fail()) return error("Read failure.");

 // validate magic
 if(!(m01 == 0x4E && m02 == 0x4D && m03 == 0x44)) return error("Not an NMD file.");
 switch(m04) {
   case(0x00): break; // cout << " TYPE 0x00" << endl; break; // stage
   case(0x02): break; // cout << " TYPE 0x02" << endl; break; // weapon
   case(0x03): break; // cout << " TYPE 0x03" << endl; break; // creation
   case(0x04): break; // cout << " TYPE 0x04" << endl; break; // cdata
   case(0x05): break; // cout << " TYPE 0x05" << endl; break; // stage
   case(0x2E): break; // cout << " TYPE 0x2E" << endl; break; // creation + weapon
   default : return error("Invalid NMD file.");
  }

 // read header
 uint32 m05 = BE_read_uint32(ifile);
 switch(m05) {
   case(0x0C060104): break;
   case(0x0C070111): break;
   case(0x0C070112): break;
   case(0x0C070113): break;
   case(0x0C080114): break;
   default : return error("Invalid NMD file.");
  }

 //
 // CREATION MODELS
 //
 if(m04 == 0x03)
   {
    uint16 h01 = BE_read_uint16(ifile); // 0x01
    uint16 h02 = BE_read_uint16(ifile); // number of bones
    uint32 h03 = BE_read_uint16(ifile); // 0x00, sometimes something else (creation/0009/0000.NMD) (creation/0013/0000.NMD has both)
    uint32 h04 = BE_read_uint16(ifile); // 0x00, sometimes something else (creation/0002/0000.NMD) (creation/0013/0000.NMD has both)
    if(ifile.fail()) return error("Read failure.");

    uint32 h05 = BE_read_uint32(ifile); // number of ?????
    uint32 h06 = BE_read_uint32(ifile); // offset to 0x50-byte item information
    uint32 h07 = BE_read_uint32(ifile); // offset to 0x40-byte item matrices
    uint32 h08 = BE_read_uint32(ifile); // offset to ?????
    if(ifile.fail()) return error("Read failure.");

    uint32 h09 = BE_read_uint32(ifile); // offset to ?????
    uint32 h10 = BE_read_uint32(ifile); // offset to ????? (look at offset in creation/0009/0000.NMD)
    uint32 h11 = BE_read_uint32(ifile); // offset to ?????
    uint32 h12 = BE_read_uint32(ifile); // 0x00, sometimes something else (creation/0002/0000.NMD)
    if(ifile.fail()) return error("Read failure.");

    // validate header
    if(h02 == 0x00) return true;  // nothing to do, no bones
    if(h01 != 0x01) return error("Invalid NMD.", __LINE__); // expecting 0x01
    if(h05 == 0x00) return error("Invalid NMD.", __LINE__); // should not be zero
    if(h06 == 0x00) return error("Invalid NMD.", __LINE__); // should not be zero
    if(h07 == 0x00) return error("Invalid NMD.", __LINE__); // should not be zero
    if(h08 == 0x00) return error("Invalid NMD.", __LINE__); // should not be zero
    if(h09 == 0x00) return error("Invalid NMD.", __LINE__); // should not be zero
    if(h10 == 0x00) return error("Invalid NMD.", __LINE__); // should not be zero (0x38, 0x3C)
    if(h11 == 0x00) return error("Invalid NMD.", __LINE__); // should not be zero
    if(h04 && !h12) return error("Invlaid NMD.", __LINE__); // h04 and h12 associated with each other
    if(h12 && !h04) return error("Invlaid NMD.", __LINE__); // h04 and h12 associated with each other

    // move to 0x50-byte item information
    ifile.seekg(h06);
    if(ifile.fail()) return error("Seek failure.");

    // read 0x50-byte item information
    uint32 h13 = BE_read_uint32(ifile); // offset to 0x80-byte items
    uint16 h14 = BE_read_uint16(ifile); // number of bones
    uint16 h15 = BE_read_uint16(ifile); // 0x00
    if(ifile.fail()) return error("Read failure.");

    // validate 0x50-byte item information
    if(h13 % 0x10) return error("Expecting offset to be divisible by 0x10.", __LINE__);
    if(h02 != h14) return error("Number of bones do not match.", __LINE__);
    if(h15 != 0x00) return error("Invalid NMD.", __LINE__);

    // read a list of offsets
    boost::shared_array<uint32> h16;
    if(h03) {
       h16.reset(new uint32[h03]);
       for(uint32 i = 0; i < h03; i++) {
           h16[i] = BE_read_uint32(ifile);
           if(ifile.fail()) return error("Read failure.");
           if(!h16) return error("Invalid NMD.", __LINE__); // h03 and h16 associated with each other
          }
       }

    // move to 0x50-byte item data
    ifile.seekg(h13);
    if(ifile.fail()) return error("Seek failure.");

    // read 0x80-byte data
    uint32 n_bones = h14;
    uint32 datasize = n_bones * 0x50;
    boost::shared_array<char> data(new char[datasize]);
    ifile.read(data.get(), datasize);
    if(ifile.fail()) return error("Read failure.");

    // initialize skeleton
    AMC_SKELETON2 skel;
    skel.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
    skel.name = Unicode16ToUnicode08(shrtname.c_str()).get();
    skel.name += "_SKELETON";
    skel.tiplen = 1.0f;

    // create placeholder joints
    for(uint32 i = 0; i < (n_bones + 1); i++) {
        stringstream ss;
        ss << "jnt_" << setfill('0') << setw(3) << i;
        AMC_JOINT joint;
        joint.name = (i == n_bones ? "root" : ss.str());
        joint.id = AMC_INVALID_JOINT;
        joint.parent = AMC_INVALID_JOINT;
        joint.m_rel[0x0] = 1.0f;
        joint.m_rel[0x1] = 0.0f;
        joint.m_rel[0x2] = 0.0f;
        joint.m_rel[0x3] = 0.0f;
        joint.m_rel[0x4] = 0.0f;
        joint.m_rel[0x5] = 1.0f;
        joint.m_rel[0x6] = 0.0f;
        joint.m_rel[0x7] = 0.0f;
        joint.m_rel[0x8] = 0.0f;
        joint.m_rel[0x9] = 0.0f;
        joint.m_rel[0xA] = 1.0f;
        joint.m_rel[0xB] = 0.0f;
        joint.m_rel[0xC] = 0.0f;
        joint.m_rel[0xD] = 0.0f;
        joint.m_rel[0xE] = 0.0f;
        joint.m_rel[0xF] = 1.0f;
        joint.p_rel[0] = 0.0f;
        joint.p_rel[1] = 0.0f;
        joint.p_rel[2] = 0.0f;
        joint.p_rel[3] = 1.0f;
        skel.joints.push_back(joint);
       }

    // save position data
    boost::shared_array<cs::math::vector3D<real32>> posdata;
    posdata.reset(new cs::math::vector3D<real32>[n_bones]);

    // create binary stream from data and process data
    binary_stream bs(data, datasize);
    for(uint32 i = 0; i < n_bones; i++)
       {
        real32 p01 = bs.BE_read_real32(); // vector3D
        real32 p02 = bs.BE_read_real32(); // vector3D
        real32 p03 = bs.BE_read_real32(); // vector3D
        real32 p04 = bs.BE_read_real32(); // 1.0
        zero_small(p01);
        zero_small(p02);
        zero_small(p03);
        zero_small(p04);

        real32 p05 = bs.BE_read_real32(); // vector3D
        real32 p06 = bs.BE_read_real32(); // vector3D
        real32 p07 = bs.BE_read_real32(); // vector3D
        real32 p08 = bs.BE_read_real32(); // 1.0
        zero_small(p05);
        zero_small(p06);
        zero_small(p07);
        zero_small(p08);
        posdata[i][0] = p05;
        posdata[i][1] = p06;
        posdata[i][2] = p07;

        real32 p09 = bs.BE_read_real32(); // euler angle?
        real32 p10 = bs.BE_read_real32(); // euler angle?
        real32 p11 = bs.BE_read_real32(); // euler angle?
        zero_small(p09);
        zero_small(p10);
        zero_small(p11);

        uint32 p12 = bs.BE_read_uint32(); // offset to data later in file

        real32 p13 = bs.BE_read_real32(); // scale? sometimes not a real32
        real32 p14 = bs.BE_read_real32(); // scale? sometimes not a real32
        real32 p15 = bs.BE_read_real32(); // scale? sometimes not a real32
        zero_small(p13);
        zero_small(p14);
        zero_small(p15);

        uint32 p16 = bs.BE_read_uint32(); // unknown 0x00
        uint32 p17 = bs.BE_read_uint32(); // unknown 0x00
        uint32 p18 = bs.BE_read_uint32(); // unknown 0x00
        uint32 p19 = bs.BE_read_uint32(); // unknown

        uint16 p20 = bs.BE_read_uint16(); // parent
        uint16 p21 = bs.BE_read_uint16(); // bone identifier

        if(debug) {
           dfile << "item[0x" << hex << i << dec << "] = " << endl;
           dfile << " " << p01 << " " << p02 << " " << p03 << " " << p04 << endl;
           dfile << " " << p05 << " " << p06 << " " << p07 << " " << p08 << endl;
           dfile << " " << p09 << " " << p10 << " " << p11 << endl;
           dfile << " " << p13 << " " << p14 << " " << p15 << endl;
           dfile << endl;
          }

        // convert euler to quaternion
        // note: angles are in reverse order in the file!
        cs::math::quaternion<real32> q;
        cs::math::rotation_quaternion_ZYX(&q[0], p09, p10, p11);
        cs::math::normalize(q);
        
        // convert quaternion to matrix
        cs::math::matrix4x4<real32> m;
        cs::math::quaternion_to_matrix4x4(&q[0], &m[0]);

        // modify joint
        skel.joints[i].id = i;
        skel.joints[i].parent = (p20 == 0xFFFF ? n_bones : p20);
       }

    // move to matrices
    ifile.seekg(h07);
    if(ifile.fail()) return error("Seek failure.");

    // process matrices
    for(uint32 i = 0; i < n_bones; i++)
       {
        // read matrix
        real32 mat[16];
        BE_read_array(ifile, &mat[0], 16);
        if(ifile.fail()) return error("Read failure.", __LINE__);

        // transpose matrix
        real32 tsp[16];
        cs::math::matrix4x4_transpose(&tsp[0], &mat[0]);
        for(uint32 j = 0; j < 16; j++) if(std::fabs(tsp[j]) < 1.0e-4f) tsp[j] = 0.0f;

        cs::math::matrix4x4<real32> m1(&tsp[0]);
        m1.invert();

        // set matrix data
        skel.joints[i].m_rel[0x0] = m1[0x0];
        skel.joints[i].m_rel[0x1] = m1[0x1];
        skel.joints[i].m_rel[0x2] = m1[0x2];
        skel.joints[i].m_rel[0x3] = 0.0f;
        skel.joints[i].m_rel[0x4] = m1[0x4];
        skel.joints[i].m_rel[0x5] = m1[0x5];
        skel.joints[i].m_rel[0x6] = m1[0x6];
        skel.joints[i].m_rel[0x7] = 0.0f;
        skel.joints[i].m_rel[0x8] = m1[0x8];
        skel.joints[i].m_rel[0x9] = m1[0x9];
        skel.joints[i].m_rel[0xA] = m1[0xA];
        skel.joints[i].m_rel[0xB] = 0.0f;
        skel.joints[i].m_rel[0xC] = m1[0xC];
        skel.joints[i].m_rel[0xD] = m1[0xD];
        skel.joints[i].m_rel[0xE] = m1[0xE];
        skel.joints[i].m_rel[0xF] = 1.0f;
        skel.joints[i].p_rel[0] = m1[0x3];
        skel.joints[i].p_rel[1] = m1[0x7];
        skel.joints[i].p_rel[2] = m1[0xB];
        skel.joints[i].p_rel[3] = 1.0f;

        if(debug) {
           dfile << "matrix[0x" << hex << i << dec << "] = " << endl;
           dfile << setw(10) << tsp[0x0] << " " << setw(10) << tsp[0x1] << " " << setw(10) << tsp[0x2] << " " << setw(10) << tsp[0x3] << endl;
           dfile << setw(10) << tsp[0x4] << " " << setw(10) << tsp[0x5] << " " << setw(10) << tsp[0x6] << " " << setw(10) << tsp[0x7] << endl;
           dfile << setw(10) << tsp[0x8] << " " << setw(10) << tsp[0x9] << " " << setw(10) << tsp[0xA] << " " << setw(10) << tsp[0xB] << endl;
           dfile << setw(10) << tsp[0xC] << " " << setw(10) << tsp[0xD] << " " << setw(10) << tsp[0xE] << " " << setw(10) << tsp[0xF] << endl;
           dfile << endl;
          }
       }

    // modify root joint
    skel.joints[n_bones].id = n_bones;
    skel.joints[n_bones].parent = AMC_INVALID_JOINT;

    // save skeleton
    ADVANCEDMODELCONTAINER amc;
    amc.skllist2.push_back(skel);
    SaveAMC(pathname.c_str(), shrtname.c_str(), amc);
   }
 //
 //
 //

 return true;
}

bool PathExtractBonesNMD(bool do_delete, uint32 start)
{
 return PathExtractBonesNMD(NULL, do_delete, start);
}

bool PathExtractBonesNMD(LPCTSTR pathname, bool do_delete, uint32 start)
{
 // set pathname
 STDSTRING pname(pathname);
 if(!pname.length()) pname = GetModulePathname().get();

 // build filename list
 cout << "Searching for .NMD files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".NMD"), pname.c_str())) return true;
 
 // process NTP3 files
 bool break_on_errors = true;
 cout << "Processing .NMD files..." << endl;
 for(size_t i = start; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractBonesNMD(filelist[i].c_str(), do_delete)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

};