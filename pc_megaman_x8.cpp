#include "xentax.h"
#include "pc_megaman_x8.h"

PC_MEGAMAN_X8_BEGIN

bool extractWPG(const char* filename);
bool extractWSX(const char* filename);

bool extract(void)
{
 char pathname[MAX_PATH];
 GetModulePathname(pathname, MAX_PATH);
 return extract(pathname);
}

bool extract(const char* pathname)
{
 // process WPG files
 cout << "Searching for WPG files... please wait." << endl;
 deque<string> namelist;
 if(!BuildFilenameList(namelist, "wpg", pathname)) return false;
 cout << "Found " << namelist.size() << " WPG files." << endl;
 uint32 successful = 0;
 for(size_t i = 0; i < namelist.size(); i++) if(extractWPG(namelist[i].c_str())) successful++;
 cout << "Processed " << successful << " out of " << namelist.size() << " WPG files." << endl;


 //extractWSX("C:\\Temp\\Megaman X8\\opk\\pl\\PL_COS_X.wsx");
 //extractWSX("C:\\Temp\\Megaman X8\\opk\\st00\\EL00_01.wsx");

 return true;
}

bool extractWSX(const char* filename)
{
 // open file
 cout << "Processing " << filename << "." << endl;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Error opening WSX file.");

 // read number of offsets
 uint32 n_offsets = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read data size
 uint32 datasize = LE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read offsets
 boost::shared_array<uint32> offsets(new uint32[n_offsets]);
 LE_read_array(ifile, offsets.get(), n_offsets);
 if(ifile.fail()) return error("Read failure.");

 // process data at offsets
 for(size_t i = 0; i < n_offsets; i++)
    {
     // seek offset
     ifile.seekg(offsets[i]);
     if(ifile.fail()) return error("Seek failed.");

     cout << offsets[i] << endl;

     uint32 v1 = LE_read_uint32(ifile);

     if(v1 == 0x0101006C)
       {
       }
     else
      {
       return error("Expecting 0x0101006C.");
      }
    }
 

 return true;
}

bool extractWPG(const char* filename)
{
 // open file
 cout << "Processing " << filename << "." << endl;
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Error opening WPG file.");

 // compute filesize
 ifile.seekg(0, ios::end);
 size_t filesize = (size_t)ifile.tellg();
 ifile.seekg(0);
 if(filesize == 32) return true;

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(magic != 0x77706700) return error("Not a WPG file.");

 // move to data
 ifile.seekg(0x20);
 if(ifile.fail()) return error("Failed to move to WPG data.");

 // make directory for images
 char c_drive[1024];
 char c_path[1024];
 char c_name[1024];
 _splitpath(filename, c_drive, c_path, c_name, nullptr);
 string pathname(c_drive);
 pathname += c_path;
 pathname += c_name;
 CreateDirectoryA(pathname.c_str(), NULL);

 // read multiple image
 size_t image_index = 0;
 for(;;)
    {
     // read until we can't read anymore
     ifile.peek();
     if(ifile.eof()) break;
     if(ifile.fail()) break;

     // read file type
     uint32 filetype = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read unknown #1
     uint32 unk1 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read unknown #2
     uint32 unk2 = LE_read_uint32(ifile);
     if(ifile.fail()) return error("Read failure.");

     // read dimensions
     uint16 dx = LE_read_uint16(ifile);
     uint16 dy = LE_read_uint16(ifile);
     if(dx == 0) return error("Invalid WPG dimensions.");
     if(dy == 0) return error("Invalid WPG dimensions.");

     // read unknown #3
     uint16 unk3 = LE_read_uint16(ifile);
     if(ifile.fail()) return error("Read failure.");

     // build BMP filename
     stringstream ofilename;
     ofilename << pathname.c_str() << "\\" << setfill('0') << setw(3) << image_index++ << ".bmp" << ends;
     cout << ofilename.str() << endl;

     // BGRA
     if(filetype == 0x20000)
       {
        // read BMP data
        size_t datasize = 4*dx*dy;
        boost::shared_array<char> data(new char[datasize]);
        ifile.read(data.get(), datasize);
        if(ifile.fail()) return error("Reading WPG data failed.");
    
        // create BMP headers
        BITMAPFILEHEADER fileHeader;
        BITMAPINFOHEADER infoHeader;
        if(!CreateBMPHeaders(dx, dy, 32, &fileHeader, &infoHeader)) return error("Failed to create BMP headers.");
    
        // open BMP file
        ofstream ofile(ofilename.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create BMP file.");
    
        // save BMP headers
        ofile.write((char*)&fileHeader, sizeof(fileHeader));
        ofile.write((char*)&infoHeader, sizeof(infoHeader));
    
        // save BMP data
        ofile.write(data.get(), datasize);
       }
     else if(filetype == 0x10100)
       {
        // read palette
        size_t size1 = 768;
        boost::shared_array<char> data1(new char[size1]);
        ifile.read(data1.get(), size1);
        if(ifile.fail()) return error("Reading WPG palette failed.");
    
        // read data
        size_t size2 = dx*dy;
        boost::shared_array<char> data2(new char[size2]);
        ifile.read(data2.get(), size2);
    
        // convert palette
        RGBQUAD palette[256];
        size_t index = 0;
        for(size_t i = 0; i < 256; i++) {
            BYTE b1 = data1[index++];
            BYTE b2 = data1[index++];
            BYTE b3 = data1[index++];
            palette[i].rgbRed = b3;
            palette[i].rgbGreen = b2;
            palette[i].rgbBlue = b1;
            palette[i].rgbReserved = 0;
           }
    
        // create BMP headers
        BITMAPFILEHEADER fileHeader;
        BITMAPINFOHEADER infoHeader;
        if(!CreateBMPHeaders(dx, dy, 8, &fileHeader, &infoHeader)) return error("Failed to create BMP headers.");
    
        // open BMP file
        ofstream ofile(ofilename.str().c_str(), ios::binary);
        if(!ofile) return error("Failed to create BMP file.");
    
        // save BMP headers
        ofile.write((char*)&fileHeader, sizeof(fileHeader));
        ofile.write((char*)&infoHeader, sizeof(infoHeader));
    
        // save BMP data
        ofile.write((char*)&palette[0], 256*sizeof(RGBQUAD));
        ofile.write(data2.get(), size2);
       }
     else
        return error("Unknown WPG file type.");
    }

 return true;
}

PC_MEGAMAN_X8_END