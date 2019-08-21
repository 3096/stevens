#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_namco.h"
#include "xb360_ttt2.h"
using namespace std;

#define X_SYSTEM XBOX360
#define X_GAME   TTT2

//
// ARCHIVES
//

namespace X_SYSTEM { namespace X_GAME {

bool IsCOM_(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // read first two 32-bit values
 uint32 p1 = BE_read_uint32(ifile);
 uint32 p2 = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 return ((!p1) && (p2 == 0x434F4D00));
}

bool IsCRED(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // get filesize
 ifile.seekg(0, ios::end);
 uint64 filesize = ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x20) return false;

 // read offset
 uint32 offset = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(!offset) return false;
 if(!(offset < filesize)) return false;

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return false;
 char title1[0x18];
 ifile.read(&title1[0], 0x18);
 if(strcmpi(title1, "TEKKEN TAG TOURNAMENT 2") == 0) return true;

 // move to offset
 ifile.seekg(offset);
 if(ifile.fail()) return false;
 char title2[0x21];
 ifile.read(&title2[0], 0x21);
 if(strcmpi(title2, "The King of Iron Fist Tournament") == 0) return true;

 // move to offset
 ifile.seekg(offset + 2);
 if(ifile.fail()) return false;
 char title3[0x18];
 ifile.read(&title3[0], 0x18);
 if(strcmpi(title3, "TEKKEN TAG TOURNAMENT 2") == 0) return true;

 return false;
}

bool IsGOST(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // get filesize
 ifile.seekg(0, ios::end);
 uint64 filesize = ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x20) return false;

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(n_files == 0) return false;

 // read first offset
 uint32 offset = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if((offset & 0x0F) != 0) return false;
 if(!(offset < filesize)) return false;

 // move to first offset
 ifile.seekg(offset);
 if(ifile.fail()) return false;

 // read GOST
 uint32 signature = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // GOST
 return (signature == 0x474F5354);
}

bool IsMARC(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // get filesize
 ifile.seekg(0, ios::end);
 uint64 filesize = ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x20) return false;

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(n_files == 0x0) return false;
 if(n_files > 0xFF0) return false;

 // read offset/size pairs
 deque<pair<uint32,uint32>> pairs;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read pair
     uint32 offs = BE_read_uint32(ifile);
     uint32 size = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // offset cannot be zero
     if(offs == 0) return false;

     // offset must be less than filesize
     if(!(offs < filesize)) return false;

     // offset must be greater than previous one
     if(i > 0 && offs < pairs.back().first)
        return false;

     // padding between data should be small
     if(i > 0 && ! ((offs - (pairs.back().first + pairs.back().second)) < 0x10))
        return false;

     // size cannot be zero
     if(size == 0) return false;

     // size must be less than filesize
     if(!(size < filesize)) return false;

     // offset + size must be less than or equal to filesize
     if(filesize < (offs + size)) return false;

     // insert pair
     pairs.push_back(pair<uint32,uint32>(offs, size));
    }

 // last offset + size must be close to filesize
 if(!((filesize - (pairs.back().first + pairs.back().second)) < 0x10))
    return false;

 // // read chunk identifiers
 // for(uint32 i = 0; i < n_files; i++)
 //    {
 //     // move to offset
 //     ifile.seekg(pairs[i].first);
 //     if(ifile.fail()) return false;
 // 
 //     // read type
 //     uint32 type = BE_read_uint32(ifile);
 //     if(ifile.fail()) return false;
 // 
 //     // recognize NTXR
 //     if(type == 0x4E545852) return true;
 // 
 //     // recognize NTXB
 //     else if(type == 0x4E545842) return true;
 // 
 // 
 //     // must be able to recognize archives within archives
 //    }

 return true;
}

bool IsTARC(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // get filesize
 ifile.seekg(0, ios::end);
 uint64 filesize = ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x20) return false;

 // read number of files
 uint32 n_files = LE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(n_files == 0x0) return false;
 if(n_files > 0xFF) return false;

 // read offset/size pairs
 deque<pair<uint32,uint32>> pairs;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read pair
     uint32 offs = LE_read_uint32(ifile);
     uint32 size = LE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // validate offset
     // if((offs & 0x0F) != 0) return false;
     if(!(offs < filesize)) return false;

     // validate size
     if(size == 0) return false;
     if(!(size < filesize)) return false;

     // offset must be greater than previous one
     if(i > 0 && offs < pairs.back().first)
        return false;

     // insert pair
     pairs.push_back(pair<uint32,uint32>(offs, size));
    }

 // read chunk identifiers
 for(uint32 i = 0; i < n_files; i++)
    {
     // move to offset
     ifile.seekg(pairs[i].first);
     if(ifile.fail()) return false;

     // read type
     uint32 type = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // recognize MOTA
     if(type == 0x4D4F5441) return true;

     // recognize NTXR
     else if(type == 0x4E545852) return true;
    }

 return false;
}

bool IsMLST(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // get filesize
 ifile.seekg(0, ios::end);
 uint64 filesize = ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(filesize < 0x20) return false;

 // read number of models
 uint32 models = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(models == 0x0) return false;

 // read other values
 uint32 p1 = BE_read_uint32(ifile);
 uint32 p2 = BE_read_uint32(ifile);
 uint32 p3 = BE_read_uint32(ifile);
 if(ifile.fail()) return false;
 if(p1 != 0x01) return false;
 if(p2 != 0x10) return false;
 if(p3 != 0x00) return false;

 // read offsets
 deque<uint32> offslist;
 for(uint32 i = 0; i < models; i++)
    {
     // read offset
     uint32 offset = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // validate offset
     if(offset == 0) return false;
     if(!(offset < filesize)) return false;

     // insert offset
     offslist.push_back(offset);

     // move to next offset
     ifile.seekg(0x3C, ios::cur);
     if(ifile.fail()) return false;
    }

 // test offsets
 for(uint32 i = 0; i < models; i++)
    {
     // move to offset
     ifile.seekg(offslist[i]);
     if(ifile.fail()) return false;

     // read model signature
     uint32 signature = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // test NDXR signature
     if(signature != 0x4E445852) return false;
    }

 return true;
}

bool processBIN(LPCTSTR pathname)
{
 // list of filenames to process
 const TCHAR* filelist[42] = {
  TEXT("data000.bin"),
  TEXT("data001.bin"),
  TEXT("data002.bin"),
  TEXT("data003.bin"),
  TEXT("data004.bin"),
  TEXT("data005.bin"),
  TEXT("data006.bin"),
  TEXT("data007.bin"),
  TEXT("data008.bin"),
  TEXT("data009.bin"),
  TEXT("data010.bin"),
  TEXT("data011.bin"),
  TEXT("data012.bin"),
  TEXT("data013.bin"),
  TEXT("data014.bin"),
  TEXT("data015.bin"),
  TEXT("data016.bin"),
  TEXT("data017.bin"),
  TEXT("data018.bin"),
  TEXT("data019.bin"),
  TEXT("data025.bin"),
  TEXT("data026.bin"),
  TEXT("data027.bin"),
  TEXT("data028.bin"),
  TEXT("data034.bin"),
  TEXT("data035.bin"),
  TEXT("data036.bin"),
  TEXT("data037.bin"),
  TEXT("data038.bin"),
  TEXT("data039.bin"),
  TEXT("data040.bin"),
  TEXT("data041.bin"),
  TEXT("data042.bin"),
  TEXT("data043.bin"),
  TEXT("data044.bin"),
  TEXT("data045.bin"),
  TEXT("data046.bin"),
  TEXT("data047.bin"),
  TEXT("data050.bin"),
  TEXT("data051.bin"),
  TEXT("data052.bin"),
  TEXT("data053.bin"),
 };

 // open nrfo file
 STDSTRING nrfofile = pathname;
 nrfofile += TEXT("nrfo.bin");
 ifstream nfile(nrfofile.c_str(), ios::binary);
 if(!nfile) return error("Failed to open NRFO file.");

 // read number of BIN files
 uint32 n_files = BE_read_uint32(nfile);
 if(n_files != 0x2A) return error("Expecting 0x2A.");

 // read file offsets and sizes
 vector<pair<uint32, uint32>> pairs(n_files);
 for(uint32 i = 0; i < n_files; i++) {
     uint32 a = BE_read_uint32(nfile);
     uint32 b = BE_read_uint32(nfile);
     pairs[i] = pair<uint32, uint32>(a, b);
    }

 // loop files
 for(uint32 index = 0; index < n_files; index++)
    {
     // read ftypnrfo
     struct FTYPNRFO {
      uint32 size;
      uint64 signature;
      uint16 unk1;
      uint16 unk2;
      uint32 nrfo;
     };
     FTYPNRFO ftypnrfo;
     ftypnrfo.size = BE_read_uint32(nfile);
     ftypnrfo.signature = BE_read_uint64(nfile);
     ftypnrfo.unk1 = BE_read_uint16(nfile);
     ftypnrfo.unk2 = BE_read_uint16(nfile);
     ftypnrfo.nrfo = BE_read_uint32(nfile);
     if(ftypnrfo.size != 0x14) return error("Invalid FTYPNRFO size.");
     if(ftypnrfo.signature != 0x667479706E72666FUL) return error("Invalid FTYPNRFO signature.");

     // read ftyphead
     struct FTYPHEAD {
      uint32 size;
      uint32 signature;
      uint32 unk1;
      uint32 unk2;
      uint32 unk3;
      uint32 unk4;
      uint32 unk5;
     };
     FTYPHEAD ftyphead;
     ftyphead.size = BE_read_uint32(nfile);
     ftyphead.signature = BE_read_uint32(nfile);
     ftyphead.unk1 = BE_read_uint32(nfile);
     ftyphead.unk2 = BE_read_uint32(nfile);
     ftyphead.unk3 = BE_read_uint32(nfile);
     ftyphead.unk4 = BE_read_uint32(nfile);
     ftyphead.unk5 = BE_read_uint32(nfile);
     if(ftyphead.size != 0x1C) return error("Invalid FTYPHEAD size.");
     if(ftyphead.signature != 0x68656164) return error("Invalid FTYPHEAD signature.");

     // read ftypofsi
     struct FTYPOFSI {
      uint32 size;
      uint32 signature;
      deque<pair<uint32,uint32>> items;
     };
     FTYPOFSI ftypofsi;
     ftypofsi.size = BE_read_uint32(nfile);
     ftypofsi.signature = BE_read_uint32(nfile);
     if(ftypofsi.signature != 0x6F667369) return error("Invalid FTYPOFSI signature.");
     if(ftypofsi.size > 0x08) {
        for(uint32 i = 0; i < (ftypofsi.size - 0x08)/0x08; i++) {
            uint32 a = BE_read_uint32(nfile);
            uint32 b = BE_read_uint32(nfile);
            ftypofsi.items.push_back(pair<uint32,uint32>(a, b));
           }
        }

     // read ftypofsd
     struct FTYPOFSD {
      uint32 size;
      uint32 signature;
      deque<uint32> items;
     };
     FTYPOFSD ftypofsd;
     ftypofsd.size = BE_read_uint32(nfile);
     ftypofsd.signature = BE_read_uint32(nfile);
     if(ftypofsd.signature != 0x6F667364) return error("Invalid FTYPOFSD signature.");
     if(ftypofsd.size > 0x08) {
        for(uint32 i = 0; i < (ftypofsd.size - 0x08)/0x04; i++)
            ftypofsd.items.push_back(BE_read_uint32(nfile));
       }
     
     // number of OFSI and OFSD items must be equal
     if((ftypofsi.items.size() + 1) != ftypofsd.items.size())
        return error("Mismatching FTYPOFSI/FTYPOFSD tables.");

     // construct long filename
     STDSTRING fname;
     fname += pathname;
     fname += filelist[index];
     wcout << "Processing file " << fname.c_str() << "." << endl;

     // create output directory
     STDSTRING odir;
     odir += pathname;
     odir += GetShortFilenameWithoutExtension(fname.c_str()).get();
     odir += TEXT("\\");
     CreateDirectory(odir.c_str(), NULL);

     // open BIN file
     ifstream bfile(fname.c_str(), ios::binary);
     if(!bfile) return error("Failed to open BIN file.");

     // for each subfile
     for(uint32 i = 0; i < ftypofsd.items.size() - 1; i++)
        {
         uint32 offset1 = ftypofsd.items[i + 0] & 0xFFFFFFF0;
         uint32 offset2 = ftypofsd.items[i + 1] & 0xFFFFFFF0;

         // move to offset
         uint32 dataoffs = 0x100 + offset1;
         bfile.seekg(dataoffs);
         if(bfile.fail()) return error("Seek failure.");

         // read data
         uint32 datasize = offset2 - offset1;
         boost::shared_array<char> data(new char[datasize]);
         bfile.read(data.get(), datasize);
         if(bfile.fail()) return error("Read failure.");

         // create output filename
         STDSTRINGSTREAM ss;
         ss << odir << setfill(TEXT('0')) << setw(4) << i << TEXT(".ttt2");

         // create output file
         ofstream ofile(ss.str().c_str(), ios::binary);
         if(!ofile) return error("Failed to create output file.");

         // write data
         ofile.write(data.get(), datasize);
         if(ofile.fail()) return error("Write failure.");
        }
    }

 return true;
}

bool processTTT(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // read chunk type (don't return error on failure)
 uint32 type = BE_read_uint32(ifile);
 if(ifile.fail()) return true;

 // close file
 ifile.close();

 // rename MBB_
 if(type == 0x4D424200)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("MBB_")).get());

 // rename LMB_
 else if(type == 0x4C4D4200)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("LMB_")).get());

 // rename NFH_
 else if(type == 0x4E464800)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NFH_")).get());

 // rename NTXR
 else if(type == 0x4E545852)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NTXR")).get());

 // rename NTXB
 else if(type == 0x4E545842)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NTXB")).get());

 // rename NDXR
 else if(type == 0x4E445852)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NDXR")).get());

 // rename NDP3
 else if(type == 0x4E445033)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NDP3")).get());

 // rename NTP3
 else if(type == 0x4E545033)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NTP3")).get());

 // rename BONE
 else if(type == 0x424F4E45)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("BONE")).get());

 // rename 40A0
 else if(type == 0x40A00000)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("40A0")).get());

 // rename CLOT (cloth)
 else if(type == 0x1E0DB0CA)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("CLOT")).get());

 // rename MOTA
 else if(type == 0x4D4F5441)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("MOTA")).get());

 // rename PCTB
 else if(type == 0x50435442)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("PCTB")).get());

 // rename PAOF
 else if(type == 0x70616F66)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("PAOF")).get());

 // rename OELA
 else if(type == 0x4F454C41)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("OELA")).get());

 // rename NSXR
 else if(type == 0x4E535852)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NSXR")).get());

 // rename MVLT
 else if(type == 0x4D564C54)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("MVLT")).get());

 // rename NUS3 (audio)
 else if(type == 0x4E555333)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NUS3")).get());

 // rename XTAL
 else if(type == 0x7874616C)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("XTAL")).get());

 // rename BXML
 else if(type == 0x42584D4C)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("BXML")).get());

 // rename BX01
 else if(type == 0x42583031)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("BX01")).get());

 // rename XML
 else if(type == 0x3C3F786D)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("XML")).get());

 // rename ASF/WMV
 else if(type == 0x3026B275)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("ASF")).get());

 // rename XML (without header)
 else if(type == 0x3C726F6F)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("XML")).get());

 // rename XML (with BOM)
 else if(type == 0xEFBBBF3C)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("XML")).get());

 // rename PNG
 else if(type == 0x89504E47)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("PNG")).get());

 // need a way to tell if a file holds a model
 // usually in an archive file like data001\0064.TTT2

 // rename COM_
 else if(IsCOM_(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("COM_")).get());

 // rename CRED
 else if(IsCRED(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("CRED")).get());

 // rename GOST
 else if(IsGOST(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("GOST")).get());

 // rename MARC
 else if(IsMARC(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("MARC")).get());

 // rename TARC
 else if(IsTARC(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("TARC")).get());

 // rename MLST
 else if(IsMLST(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("MLST")).get());

 return true;
}

};};

//
// TEXTURES
//

namespace X_SYSTEM { namespace X_GAME {

bool processNTXR(LPCTSTR filename, bool do_delete = false)
{
 return NAMCO::ExtractNTP3(filename, do_delete, true);
}

};};

//
// MODELS
//

namespace X_SYSTEM { namespace X_GAME {

bool processNDXR(LPCTSTR filename, bool do_delete = false)
{
 return NAMCO::ExtractNDP3(filename, nullptr, do_delete);
}

bool processBONE(LPCTSTR filename, bool do_delete = false)
{
 // works
 // AMC_JOINT_FORMAT_RELATIVE
 // joint.p_rel[0] = coords1[i][0];
 // joint.p_rel[1] = coords1[i][1];
 // joint.p_rel[2] = coords1[i][2];

 // doesn't work
 // AMC_JOINT_FORMAT_ABSOLUTE
 // joint.p_rel[0] = coords2[i][0];
 // joint.p_rel[1] = coords2[i][1];
 // joint.p_rel[2] = coords2[i][2];

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

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(magic != 0x424F4E45) return error("Not a BONE file.");

 // read number of joints
 ifile.seekg(0x10);
 uint32 n_joints = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 if(n_joints == 0) return true;

 // read offsets
 ifile.seekg(0x18);
 uint32 offset1 = LE_read_uint32(ifile); // parent list
 uint32 offset2 = LE_read_uint32(ifile); // positions
 uint32 offset3 = LE_read_uint32(ifile); // angles
 uint32 offset4 = LE_read_uint32(ifile); // ???
 uint32 offset5 = LE_read_uint32(ifile); // ???
 uint32 offset6 = LE_read_uint32(ifile); // names
 uint32 offset7 = LE_read_uint32(ifile); // ???
 if(ifile.fail()) return error("Read failure.");

 // offset4
 // 0D 00 00 00
 // 00 00 00 00 AF 00 00 00 B0 00 00 00 B1 00 00 00 (  0 175 176 177)
 // B4 00 00 00 B5 00 00 00 B6 00 00 00 B9 00 00 00 (180 181 182 185)
 // BA 00 00 00 BB 00 00 00 BE 00 00 00 BF 00 00 00 (186 187 190 191)
 // C0 00 00 00                                     (192)

 // read parent list
 ifile.seekg(offset1);
 boost::shared_array<uint32> plist(new uint32[n_joints]);
 if(!LE_read_array(ifile, plist.get(), n_joints)) return error("Read failure.");

 // read positions 1
 if(debug) dfile << "float set #1" << endl;
 ifile.seekg(offset2);
 typedef cs::math::vector3D<real32> vector_t;
 boost::shared_array<vector_t> coords1(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     coords1[i][0] = LE_read_real32(ifile);
     coords1[i][1] = LE_read_real32(ifile);
     coords1[i][2] = LE_read_real32(ifile);
     if(debug) dfile << "pos " << i << ": " << coords1[i][0] << ", " << coords1[i][1] << ", " << coords1[i][2] << endl;
    }
 if(debug) dfile << endl;

 // read positions 2
 if(debug) dfile << "float set #2" << endl;
 ifile.seekg(offset3);
 boost::shared_array<vector_t> coords2(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     coords2[i][0] = LE_read_real32(ifile);
     coords2[i][1] = LE_read_real32(ifile);
     coords2[i][2] = LE_read_real32(ifile);
     if(debug) dfile << coords2[i][0] << ", " << coords2[i][1] << ", " << coords2[i][2] << endl;
    }
 if(debug) dfile << endl;

 // read names
 if(debug) dfile << "names" << endl;
 ifile.seekg(offset6);
 boost::shared_array<string> namelist(new string[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     char buffer[0x20];
     ifile.read(&buffer[0], 0x20);
     namelist[i] = buffer;
     if(debug) dfile << buffer << endl;
    }
 if(debug) dfile << endl;

 // read positions 3
 if(debug) dfile << "float set #3" << endl;
 ifile.seekg(offset7);
 boost::shared_array<vector_t> coords3(new vector_t[n_joints]);
 for(uint32 i = 0; i < n_joints; i++) {
     coords3[i][0] = LE_read_real32(ifile);
     coords3[i][1] = LE_read_real32(ifile);
     coords3[i][2] = LE_read_real32(ifile);
     if(debug) dfile << coords3[i][0] << ", " << coords3[i][1] << ", " << coords3[i][2] << endl;
    }
 if(debug) dfile << endl;

 // create model container
 ADVANCEDMODELCONTAINER amc;

 // initialize skeleton
 AMC_SKELETON2 skel;
 skel.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skel.name = Unicode16ToUnicode08(shrtname.c_str()).get();
 skel.name += "_SKELETON";
 skel.tiplen = 1.0f;

 // process joints
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

     // convert euler angle
     if(debug) {
        dfile << "matrix " << i << ":" << endl;
        dfile << m[0x0] << " " << m[0x1] << " " << m[0x2] << " " << m[0x3] << endl;
        dfile << m[0x4] << " " << m[0x5] << " " << m[0x6] << " " << m[0x7] << endl;
        dfile << m[0x8] << " " << m[0x9] << " " << m[0xA] << " " << m[0xB] << endl;
        dfile << m[0xC] << " " << m[0xD] << " " << m[0xE] << " " << m[0xF] << endl;
        dfile << endl;
       }

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << i;

     // create joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = i;
     joint.parent = plist[i];
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

     // insert joint
     skel.joints.push_back(joint);
    }

 // insert skeleton
 amc.skllist2.push_back(skel);

 // save model
 SaveAMC(pathname.c_str(), shrtname.c_str(), amc);
 return true;
}

};};

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[XBOX360] Tekken Tag Tournament 2");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Decompress BIN files with xbdecompress.\n");
 p2 += TEXT("3. Run ripper.\n");
 p2 += TEXT("4. Select game and click Browse.\n");
 p2 += TEXT("5. Select folder where all BIN files are.\n");
 p2 += TEXT("6. Click OK and answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need 32 GB free space.\n");
 p3 += TEXT("3. 12 GB for game + 20 GB extraction.");

 // insert title
 AddGameTitle(p1.c_str(), p2.c_str(), p3.c_str(), extract);
 return true;
}

bool extract(void)
{
 STDSTRING pathname = GetModulePathname().get();
 return extract(pathname.c_str());
}

bool extract(LPCTSTR pname)
{
 // set pathname
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // booleans
 bool doBIN = true;
 bool doTTT = true;
 bool doNTX = true;
 bool doNDX = true;
 bool doBON = true;

 // questions
 if(doBIN) doBIN = YesNoBox("Process BIN (archive) files?");
 if(doTTT) doTTT = YesNoBox("Process TTT (archive) files?");
 if(doNTX) doNTX = YesNoBox("Process NTXR (texture) files?");
 if(doNDX) doNDX = YesNoBox("Process NDXR (model) files?");
 if(doBON) doBON = YesNoBox("Process BONE (skeleton) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process BIN files
 cout << "STAGE 1" << endl;
 if(doBIN) {
    if(!processBIN(pathname.c_str())) return false;
    cout << endl;
   }

 // process TTT files
 cout << "STAGE 2" << endl;
 if(doTTT) {
    cout << "Processing .TTT files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".TTT2"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processTTT(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process NTXR files
 cout << "STAGE 3" << endl;
 if(doNTX) {
    cout << "Processing .NTXR files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".NTXR"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processNTXR(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process NDXR files
 cout << "STAGE 4" << endl;
 if(doNDX) {
    cout << "Processing .NDXR files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".NDXR"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processNDXR(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process BONE files
 cout << "STAGE 5" << endl;
 if(doBON) {
    cout << "Processing .BONE files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BONE"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processBONE(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // record end time
 uint64 t1 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t1);

 // determine time
 uint64 hz;
 QueryPerformanceFrequency((LARGE_INTEGER*)&hz);
 uint64 dt = (t1 - t0);
 real64 dt_sec = (real64)(t1 - t0)/(real64)hz;
 real64 dt_min = dt_sec/60.0;
 real64 dt_hrs = dt_min/60.0;
 cout << "Elapsed time = " << dt_min << " minutes." << endl;

 return true;
}

}};