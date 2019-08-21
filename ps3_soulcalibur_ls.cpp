#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_namco.h"
#include "ps3_soulcalibur_ls.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   SOUL_CALIBUR_LS

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool isAC1(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // validate number of files
 n_files++;
 if(n_files > 0xFFFF) return false;

 // compute predicted header sizes
 uint32 headsize1 = 4 + 4*n_files;
 uint32 headsize2 = ((headsize1 + 0x0F) & (~0x0F));

 // load offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read offset
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // test 1
     // offsets must be greater than smallest header size
     if(item < headsize1) return false;

     // test 2
     // offset must be greater than or equal to the previous offset
     if((i > 0) && (item < offsets.back()))
        return false;

     // test 3
     // offset must be less or equal to the filesize
     // note that it is possible to have an entry with 0 datasize
     if(filesize < item) return false;

     // save offset
     offsets.push_back(item);
    }

 // final test
 return true;
}

bool isFLIST(LPCTSTR filename)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // read number of filenames
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return false;

 // validate number of filenames
 if(n_files == 0x000) return false; // must have at least one filename
 if(n_files > 0xFFFF) return false; // can't have too many files

 // for each file
 uint32 count = 0;
 for(uint32 i = 0; i < n_files; i++)
    {
     // read string length
     uint32 size = BE_read_uint32(ifile);
     if(ifile.fail()) return false;

     // validate string length
     if(size > 0xFF) return false;
     if(size == 0) continue;

     // read string
     char data[256];
     ifile.read(&data[0], size);
     if(ifile.fail()) return false;

     // string must be null terminated
     // NOTE: TO BE MORE STRICT, WE COULD ALSO REQUIRE
     // AT LEAST ONE '.' IN THE FILENAME
     if(data[size - 1] != 0x00) return false;

     // string must have ascii characters
     for(uint32 j = 0; j < (size - 1); j++) {
         if(data[j]  < 0x20) return false; // less than ' '
         if(data[j]  > 0x7E) return false; // more than '~'
         if(data[j] == 0x22) return false; // no '"'
         if(data[j] == 0x2A) return false; // no '*'
         if(data[j] == 0x2F) return false; // no '/'
         if(data[j] == 0x3A) return false; // no ':'
         if(data[j] == 0x3C) return false; // no '<'
         if(data[j] == 0x3E) return false; // no '>'
         if(data[j] == 0x3F) return false; // no '?'
         if(data[j] == 0x5C) return false; // no '\'
         if(data[j] == 0x7C) return false; // no '|'
        }

     // count number of valid filenames seen
     count++;
    }

 if(count == 0) return false;
 return ((count == n_files) || (count == (n_files - 1)));
}

bool processIDX(LPCTSTR filename)
{
 // temporary begin
 uint32 IDX_version = 2;
 // temporary end

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open IDX file.");

 // determine filesize
 uint32 filesize = 0x00;
 ifile.seekg(0, ios::end);
 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);

 // compute number of IDX entries
 uint32 n_files = filesize / 0x3C;
 if(!n_files) return true;

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // open dat file
 STDSTRING cfname = pathname;
 cfname += shrtname;
 cfname += TEXT(".out");
 ifstream cfile(cfname.c_str(), ios::binary);
 if(!cfile) return error("Failed to open OUT file.");

 // create savepath
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // for version 2 of the IDX files, move to 0x14 before reading
 if(IDX_version == 2) {
    ifile.seekg(0x14);
    if(ifile.fail()) return error("Seek failure.");
   }

 // save files
 for(uint32 i = 0; i < n_files; i++)
    {
     // message
     cout << "Saving file " << (i + 1) << " of " << n_files << "." << endl;

     // read filesize
     uint32 datasize = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read and save data
     if(datasize)
       {
        // read data
        boost::shared_array<char> data(new char[datasize]);
        cfile.read(data.get(), datasize);
        if(cfile.fail()) return error("Read failure.");

        // create output filename
        STDSTRINGSTREAM ss;
        ss << savepath;
        ss << setfill(TEXT('0')) << setw(4) << i << TEXT(".SCX");

        // save data
        ofstream ofile(ss.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create output file.");
        ofile.write(data.get(), datasize);
        if(ofile.fail()) return error("Write failure.");
       }

     // skip to next item
     ifile.seekg(0x38, ios::cur);
     if(ifile.fail()) return error("Seek failure.");
    }

 return true;
}

bool processSCX(LPCTSTR filename)
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

 // rename NDP3
 else if(type == 0x4E445033)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NDP3")).get());

 // rename NDXR
 else if(type == 0x4E445852)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NDXR")).get());

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

 //
 // SOUL CALIBUR SPECIFIC
 //

 // rename BA
 else if(type == 0x05016261)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("BA")).get());

 // rename CDPD
 else if(type == 0x43445044)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("CDPD")).get());

 // rename CPLT
 else if(type == 0x43504C54)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("CPLT")).get());

 // rename CRID (PS3 CPK FILE)
 else if(type == 0x43524944)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("CRID")).get());

 // rename ENST
 else if(type == 0x454E5354)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("ENST")).get());

 // rename IBLD
 else if(type == 0x49424C44)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("IBLD")).get());

 // rename LPB
 else if(type >= 0x6C706200 && type <= 0x6C7062FF)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("LPB")).get());

 // rename MOB
 else if(type >= 0x4D4F4200 && type <= 0x4D4F42FF)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("MOB")).get());

 // rename NMD
 else if(type >= 0x4E4D4400 && type <= 0x4E4D44FF)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NMD")).get());

 // rename NSP3
 else if(type == 0x4E535033)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("NSP3")).get());

 // rename PHYB
 else if(type == 0x50485942)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("PHYB")).get());

 // rename RQSQ
 else if(type == 0x52515351)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("RQSQ")).get());

 // rename RSSH
 else if(type == 0x52535348)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("RSSH")).get());

 // rename SC4L
 else if(type == 0x5343344C)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("SC4L")).get());

 // rename @UTF
 else if(type == 0x40555446)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("UTF")).get());

 // rename VTB
 else if(type == 0x76746200)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("VTB")).get());

 // rename WP10
 else if(type == 0x57503130)
    MoveFile(filename, ChangeFileExtension(filename, TEXT("WP10")).get());


 //
 // ARCHIVES
 //

 // rename AC1
 else if(isAC1(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("AC1")).get());

 //
 // FILELISTS
 //

 // rename FILELIST
 else if(isFLIST(filename))
    MoveFile(filename, ChangeFileExtension(filename, TEXT("filelist")).get());

 //
 // UNKNOWN
 //

 // rename UNKNOWN
 else
    MoveFile(filename, ChangeFileExtension(filename, TEXT("unknown")).get());

 // // rename COM_
 // else if(IsCOM_(filename))
 //    MoveFile(filename, ChangeFileExtension(filename, TEXT("COM_")).get());
 // 
 // // rename CRED
 // else if(IsCRED(filename))
 //    MoveFile(filename, ChangeFileExtension(filename, TEXT("CRED")).get());
 // 
 // // rename GOST
 // else if(IsGOST(filename))
 //    MoveFile(filename, ChangeFileExtension(filename, TEXT("GOST")).get());
 // 
 // // rename MARC
 // else if(IsMARC(filename))
 //    MoveFile(filename, ChangeFileExtension(filename, TEXT("MARC")).get());
 // 
 // // rename TARC
 // else if(IsTARC(filename))
 //    MoveFile(filename, ChangeFileExtension(filename, TEXT("TARC")).get());
 // 
 // // rename MLST
 // else if(IsMLST(filename))
 //    MoveFile(filename, ChangeFileExtension(filename, TEXT("MLST")).get());

 return true;
}

bool processAC1(LPCTSTR filename)
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

 // read number of files
 uint32 n_files = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 n_files++; // always an extra number

 // read offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < n_files; i++) {
     uint32 item = BE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");
     offsets.push_back(item);
    }

 // create savepath
 STDSTRING savepath = pathname;
 savepath += shrtname;
 savepath += TEXT("\\");
 CreateDirectory(savepath.c_str(), NULL);

 // save files
 for(uint32 i = 0; i < n_files; i++)
    {
     // determine datasize
     uint32 datasize = 0;
     if(i == (n_files - 1)) datasize = filesize - offsets[i];
     else datasize = offsets[i + 1] - offsets[i];

     // nothing to save
     if(!datasize) continue;

     // move to offset
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failure.");

     // read data
     boost::shared_array<char> data(new char[datasize]);
     ifile.read(data.get(), datasize);
     if(ifile.fail()) return error("Read failure.");

     // create output filename
     STDSTRINGSTREAM ss;
     ss << savepath;
     ss << setfill(TEXT('0')) << setw(4) << i << TEXT(".SCX");

     // save data
     ofstream ofile(ss.str().c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");
     ofile.write(data.get(), datasize);
     if(ofile.fail()) return error("Write failure.");
    }

 // delete file
 ifile.close();
 if(!DeleteFile(filename))
    return error("Failed to delete AC1 file. Must stop processing to prevent infinite loop.");

 return true;
}

};};

//
// MATERIALS
//

namespace X_SYSTEM { namespace X_GAME {

#include "ps3_soulcalibur_ls.mat"

bool processMaterial(binary_stream& bs, NAMCO::MATERIALINFO& mi)
{
 // initialize material
 mi.list[0] = std::pair<uint32, uint32>(0, NAMCO_INVALID);
 mi.list[1] = std::pair<uint32, uint32>(0, NAMCO_INVALID);
 mi.list[2] = std::pair<uint32, uint32>(0, NAMCO_INVALID);
 mi.list[3] = std::pair<uint32, uint32>(0, NAMCO_INVALID);
 mi.list[4] = std::pair<uint32, uint32>(0, NAMCO_INVALID);
 mi.list[5] = std::pair<uint32, uint32>(0, NAMCO_INVALID);
 mi.list[6] = std::pair<uint32, uint32>(0, NAMCO_INVALID);
 mi.list[7] = std::pair<uint32, uint32>(0, NAMCO_INVALID);

 // read material ID
 uint32 matid = bs.BE_read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // save material ID
 mi.mtrl_id = matid;

 // process material
 switch(matid)
 {
   case(0x00000011) : return material_00000011(bs, mi);
   case(0x04100011) : return material_04100011(bs, mi);
   case(0x08100000) : return material_08100000(bs, mi);
   case(0x0810003b) : return material_0810003b(bs, mi);
   case(0x08200014) : return material_08200014(bs, mi);
   case(0x0820001a) : return material_0820001a(bs, mi);
   case(0x0820001b) : return material_0820001b(bs, mi);
   case(0x08200020) : return material_08200020(bs, mi);
   case(0x08200021) : return material_08200021(bs, mi);
   case(0x21460300) : return material_21460300(bs, mi);
   case(0x21550320) : return material_21550320(bs, mi);
   case(0x22080000) : return material_22080000(bs, mi);
   case(0x22080400) : return material_22080400(bs, mi);
   case(0x22080480) : return material_22080480(bs, mi);
   case(0x22080500) : return material_22080500(bs, mi);
   case(0x22080580) : return material_22080580(bs, mi);
   case(0x22080782) : return material_22080782(bs, mi);
   case(0x22081480) : return material_22081480(bs, mi);
   case(0x22081580) : return material_22081580(bs, mi);
   case(0x22084480) : return material_22084480(bs, mi);
   case(0x22088000) : return material_22088000(bs, mi);
   case(0x22088080) : return material_22088080(bs, mi);
   case(0x22088180) : return material_22088180(bs, mi);
   case(0x22088400) : return material_22088400(bs, mi);
   case(0x22088580) : return material_22088580(bs, mi);
   case(0x220c0400) : return material_220c0400(bs, mi);
   case(0x220c0480) : return material_220c0480(bs, mi);
   case(0x220c0580) : return material_220c0580(bs, mi);
   case(0x220c0680) : return material_220c0680(bs, mi);
   case(0x220c0780) : return material_220c0780(bs, mi);
   case(0x220c0c00) : return material_220c0c00(bs, mi);
   case(0x220c0c80) : return material_220c0c80(bs, mi);
   case(0x220c0d00) : return material_220c0d00(bs, mi);
   case(0x220c0d80) : return material_220c0d80(bs, mi);
   case(0x220c0e00) : return material_220c0e00(bs, mi);
   case(0x220c0e80) : return material_220c0e80(bs, mi);
   case(0x220c0ea0) : return material_220c0ea0(bs, mi);
   case(0x220c0f00) : return material_220c0f00(bs, mi);
   case(0x220c0f20) : return material_220c0f20(bs, mi);
   case(0x220c0f80) : return material_220c0f80(bs, mi);
   case(0x220c0fa0) : return material_220c0fa0(bs, mi);
   case(0x220c3e80) : return material_220c3e80(bs, mi);
   case(0x220c468c) : return material_220c468c(bs, mi);
   case(0x220c8800) : return material_220c8800(bs, mi);
   case(0x220c8880) : return material_220c8880(bs, mi);
   case(0x220c8900) : return material_220c8900(bs, mi);
   case(0x220c8980) : return material_220c8980(bs, mi);
   case(0x220c8c00) : return material_220c8c00(bs, mi);
   case(0x220c8d80) : return material_220c8d80(bs, mi);
   case(0x220d0600) : return material_220d0600(bs, mi);
   case(0x220d0680) : return material_220d0680(bs, mi);
   case(0x221c3f80) : return material_221c3f80(bs, mi);
   case(0x222c0400) : return material_222c0400(bs, mi);
   case(0x222c0480) : return material_222c0480(bs, mi);
   case(0x222c0580) : return material_222c0580(bs, mi);
   case(0x222c0600) : return material_222c0600(bs, mi);
   case(0x222c0680) : return material_222c0680(bs, mi);
   case(0x222c06a0) : return material_222c06a0(bs, mi);
   case(0x222c0700) : return material_222c0700(bs, mi);
   case(0x222c0780) : return material_222c0780(bs, mi);
   case(0x222c8180) : return material_222c8180(bs, mi);
   case(0x223c3780) : return material_223c3780(bs, mi);
   case(0x22480700) : return material_22480700(bs, mi);
   case(0x22480780) : return material_22480780(bs, mi);
   case(0x24080000) : return material_24080000(bs, mi);
   case(0x24080010) : return material_24080010(bs, mi);
   case(0x24080020) : return material_24080020(bs, mi);
   case(0x24080100) : return material_24080100(bs, mi);
   case(0x24080110) : return material_24080110(bs, mi);
   case(0x24080400) : return material_24080400(bs, mi);
   case(0x24080410) : return material_24080410(bs, mi);
   case(0x24080480) : return material_24080480(bs, mi);
   case(0x24080500) : return material_24080500(bs, mi);
   case(0x24080510) : return material_24080510(bs, mi);
   case(0x24081000) : return material_24081000(bs, mi);
   case(0x24081400) : return material_24081400(bs, mi);
   case(0x24081410) : return material_24081410(bs, mi);
   case(0x24081440) : return material_24081440(bs, mi);
   case(0x24082400) : return material_24082400(bs, mi);
   case(0x24082410) : return material_24082410(bs, mi);
   case(0x24083400) : return material_24083400(bs, mi);
   case(0x24084400) : return material_24084400(bs, mi);
   case(0x24084410) : return material_24084410(bs, mi);
   case(0x24085400) : return material_24085400(bs, mi);
   case(0x24088490) : return material_24088490(bs, mi);
   case(0x24090400) : return material_24090400(bs, mi);
   case(0x24090410) : return material_24090410(bs, mi);
   case(0x240c0400) : return material_240c0400(bs, mi);
   case(0x240c0410) : return material_240c0410(bs, mi);
   case(0x240c0600) : return material_240c0600(bs, mi);
   case(0x240c0900) : return material_240c0900(bs, mi);
   case(0x240c0910) : return material_240c0910(bs, mi);
   case(0x240c0a00) : return material_240c0a00(bs, mi);
   case(0x240c0a10) : return material_240c0a10(bs, mi);
   case(0x240c0c00) : return material_240c0c00(bs, mi);
   case(0x240c0c10) : return material_240c0c10(bs, mi);
   case(0x240c0d00) : return material_240c0d00(bs, mi);
   case(0x240c0e00) : return material_240c0e00(bs, mi);
   case(0x240c0e10) : return material_240c0e10(bs, mi);
   case(0x240c0e20) : return material_240c0e20(bs, mi);
   case(0x240c260c) : return material_240c260c(bs, mi);
   case(0x240c261c) : return material_240c261c(bs, mi);
   case(0x240c270c) : return material_240c270c(bs, mi);
   case(0x240c3614) : return material_240c3614(bs, mi);
   case(0x240c4604) : return material_240c4604(bs, mi);
   case(0x240c460c) : return material_240c460c(bs, mi);
   case(0x240c461c) : return material_240c461c(bs, mi);
   case(0x240c470c) : return material_240c470c(bs, mi);
   case(0x240c471c) : return material_240c471c(bs, mi);
   case(0x241c0200) : return material_241c0200(bs, mi);
   case(0x241c0610) : return material_241c0610(bs, mi);
   case(0x242c0100) : return material_242c0100(bs, mi);
   case(0x242c0400) : return material_242c0400(bs, mi);
   case(0x242c0410) : return material_242c0410(bs, mi);
   case(0x242c0600) : return material_242c0600(bs, mi);
   case(0x242c0620) : return material_242c0620(bs, mi);
   case(0x24400101) : return material_24400101(bs, mi);
   case(0x24480200) : return material_24480200(bs, mi);
   case(0x24480400) : return material_24480400(bs, mi);
   case(0x24480600) : return material_24480600(bs, mi);
   case(0x244c0400) : return material_244c0400(bs, mi);
   case(0x244c0600) : return material_244c0600(bs, mi);
   case(0x244c0c00) : return material_244c0c00(bs, mi);
   case(0x244c0e00) : return material_244c0e00(bs, mi);
   case(0x246c0400) : return material_246c0400(bs, mi);
   case(0x246c0600) : return material_246c0600(bs, mi);
   case(0x28000101) : return material_28000101(bs, mi);
   case(0x280083c0) : return material_280083c0(bs, mi);
   case(0x280087c0) : return material_280087c0(bs, mi);
   case(0x280101c0) : return material_280101c0(bs, mi);
   case(0x280103c0) : return material_280103c0(bs, mi);
   case(0x280103d0) : return material_280103d0(bs, mi);
   case(0x280103e0) : return material_280103e0(bs, mi);
   case(0x280105c0) : return material_280105c0(bs, mi);
   case(0x280107c0) : return material_280107c0(bs, mi);
   case(0x280107d0) : return material_280107d0(bs, mi);
   case(0x280107e0) : return material_280107e0(bs, mi);
   case(0x280503c0) : return material_280503c0(bs, mi);
   case(0x280507c0) : return material_280507c0(bs, mi);
   case(0x280543e0) : return material_280543e0(bs, mi);
   case(0x280603c0) : return material_280603c0(bs, mi);
   case(0x280603c4) : return material_280603c4(bs, mi);
   case(0x280605c0) : return material_280605c0(bs, mi);
   case(0x280607c0) : return material_280607c0(bs, mi);
   case(0x280607c4) : return material_280607c4(bs, mi);
   case(0x280607e0) : return material_280607e0(bs, mi);
   case(0x28060fd0) : return material_28060fd0(bs, mi);
   case(0x28081400) : return material_28081400(bs, mi);
   case(0x281085c0) : return material_281085c0(bs, mi);
   case(0x281085d0) : return material_281085d0(bs, mi);
   case(0x281105d0) : return material_281105d0(bs, mi);
   case(0x28280400) : return material_28280400(bs, mi);
   case(0x284607c0) : return material_284607c0(bs, mi);
   case(0x30000000) : return material_30000000(bs, mi);
   case(0x94011063) : return material_94011063(bs, mi); // Wii-U Super Smash Bros
   default : {
        cout << "material 0x" << hex << matid << dec << endl;
        //return error("Invalid NDP3 file."); // to prevent running on Tekken Revolution
        message("Unsupported material.");
        return material_01(bs, mi);
       }
  }

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
 p1 += TEXT("[PS3] Soul Calibur: Lost Swords");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Use HCS's Naruto CPK unpacker on CPK files.\n");
 p2 += TEXT("3. For each XXX.XFBIN output, rename to XXX.OUT.\n");
 p2 += TEXT("4. Place IDX files in same folder as OUT files.\n");
 p2 += TEXT("5. Run ripper and select game.\n");
 p2 += TEXT("6. Select folder where IDX and OUT files are.\n");
 p2 += TEXT("7. Click OK and answer questions. Wait.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~12 GB free space.\n");
 p3 += TEXT("3. 4 GB for game + 8 GB extraction.\n");

 // insert title
 AddGameTitle(p1.c_str(), p2.c_str(), p3.c_str(), extract);
 return true;
}

bool extract(void)
{
 LPCTSTR pathname = GetModulePathname().get();
 return extract(pathname);
}

bool extract(LPCTSTR pname)
{
 // set pathname
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // booleans
 bool doIDX = true;
 bool doSCX = true;
 bool doNTP = true;
 bool doNDP = true;
 bool doNMD = true;

 // questions
 if(doIDX) doIDX = YesNoBox("Process IDX files (archives)?");
 if(doSCX) doSCX = YesNoBox("Process SCX files (archives)?");
 if(doNTP) doNTP = YesNoBox("Process NTP3 files (textures)?");
 if(doNDP) doNDP = YesNoBox("Process NDP3 files (models)?");
 if(doNMD) doNMD = YesNoBox("Process NMD files (bones)?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process archive
 cout << "STAGE 1" << endl;
 if(doIDX) {
    cout << "Processing .IDX files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".IDX"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!processIDX(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // process archives
 cout << "STAGE 2" << endl;
 if(doSCX)
   {
    uint32 index = 0;
    for(;;)
       {
        // keep track of file processed
        uint32 n1 = 0;
        uint32 n2 = 0;

        cout << "Processing .SCX files..." << endl;
        deque<STDSTRING> filelist;
        BuildFilenameList(filelist, TEXT(".SCX"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processSCX(filelist[i].c_str())) return false;
           }
        cout << endl;
        n1 = filelist.size();

        cout << "Processing .AC1 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".AC1"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!processAC1(filelist[i].c_str())) return false;
           }
        cout << endl;
        n2 = filelist.size();

        // terminate
        if((n1 == 0) && (n2 == 0)) break;
        index++;
        if(index > 20) return error("We have become stuck in an infinite loop. Please be sure to run in Administrator mode!");
       }
   }

 // using
 using NAMCO::PathExtractNDP3;
 using NAMCO::MATERIALFUNCTION;

 // process textures
 cout << "STAGE 3" << endl;
 if(doNTP) if(!NAMCO::PathExtractNTP3(pathname.c_str(), false)) return false;

 // process models
 cout << "STAGE 4" << endl;
 if(doNDP) if(!PathExtractNDP3(pathname.c_str(), (MATERIALFUNCTION)processMaterial, false)) return false;

 // process bones
 cout << "STAGE 5" << endl;
 if(doNMD) if(!NAMCO::PathExtractBonesNMD(pathname.c_str(), true)) return false;

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

 // // TEMP BEGIN
 // #include "x_con.h"
 // static frequency_map<uint32> matmap;
 // for(auto iter = matmap.begin(); iter != matmap.end(); iter++) {
 //     cout << setfill('0');
 //     cout << "0x" << hex << setw(8) << iter->first << dec;
 //     cout << " appears ";
 //     cout << "0x" << hex << iter->second << dec << " times." << endl;
 //    }
 // // TEMP END

 return true;
}

}};