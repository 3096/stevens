#include "xentax.h"
#include "xbox360/xgraphics.h"
#include "x_bmp.h"

imageRGBA::imageRGBA()
{
 _rows = 0;
 _cols = 0;
}

imageRGBA::~imageRGBA()
{
 clear();
}

void imageRGBA::clear(void)
{
 _data.reset((unsigned char*)0);
 _rows = 0;
 _cols = 0;
}

bool imageRGBA::morton(void)
{
 // validate
 if(_rows != _cols) return false;
 if(!_data) return false;
 if(!_rows) return false;
 if(!_cols) return false;

 // allocate copy
 boost::shared_array<unsigned char> _copy(new unsigned char[total_bytes()]);
 for(uint32 r = 0; r < _rows; r++) {
     for(uint32 c = 0; c < _cols; c++) {
         uint32 morton = array_to_morton(r, c);
         uint32 copy_position = 4*r*_cols + 4*c;
         uint32 data_position = 4*morton;
         _copy[copy_position + 0] = _data[data_position + 0];
         _copy[copy_position + 1] = _data[data_position + 1];
         _copy[copy_position + 2] = _data[data_position + 2];
         _copy[copy_position + 3] = _data[data_position + 3];
        }
    }

 // copy and return
 _data = _copy;
 return true;
}

bool imageRGBA::untile(void)
{
 // validate
 if(_rows != _cols) return false;
 if(!_data) return false;
 if(!_rows) return false;
 if(!_cols) return false;

 // allocate copy
 boost::shared_array<unsigned char> _copy(new unsigned char[total_bytes()]);
 DWORD pitch = pitch_bytes();
 XGUntileSurface(_copy.get(), pitch, NULL, _data.get(), _cols, _rows, NULL, 4);

 // copy and return
 _data = _copy;
 return true;
}

pixelRGBA imageRGBA::get_pixel(size_t r, size_t c)const
{
 size_t i1 = r * pitch_bytes() + (c << 2);
 size_t i2 = i1 + 1;
 size_t i3 = i2 + 1;
 size_t i4 = i3 + 1;
 pixelRGBA retval = {
  _data[i1],
  _data[i2],
  _data[i3],
  _data[i4]
 };
 return retval;
}

pixelRGBA imageRGBA::set_pixel(size_t r, size_t c, const pixelRGBA& p)
{
 size_t i1 = r * pitch_bytes() + (c << 2);
 size_t i2 = i1 + 1;
 size_t i3 = i2 + 1;
 size_t i4 = i3 + 1;
 pixelRGBA retval = {
  _data[i1],
  _data[i2],
  _data[i3],
  _data[i4]
 };
 _data[i1] = p.R;
 _data[i2] = p.G;
 _data[i3] = p.B;
 _data[i4] = p.A;
 return retval;
}

bool imageRGBA::loadBMP(const char* filename)
{
 return true;
}

bool imageRGBA::loadTGA(const char* filename)
{
 // clear previous
 using namespace std;
 clear();

 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return false;

 // read ID length
 uint08 TGA_IdLength = LE_read_uint08(ifile);
 if(ifile.fail()) return false;

 // read map type
 uint08 TGA_ColorMapType = LE_read_uint08(ifile);
 if(ifile.fail()) return false;
 if(!((TGA_ColorMapType == 0) || (TGA_ColorMapType == 1))) return false;

 // read image type
 uint08 TGA_ImageType = LE_read_uint08(ifile);
 if(ifile.fail()) return false;

 // read color map offset
 uint16 TGA_ColorMapOffset = LE_read_uint16(ifile);
 if(ifile.fail()) return false;

 // read color map number of entries
 uint16 TGA_ColorMapLength = LE_read_uint16(ifile);
 if(ifile.fail()) return false;
 if(TGA_ColorMapType == 1 && TGA_ColorMapLength == 0) return false;

 // read color map bits per pixel
 uint08 TGA_ColorMapBitsPerPixel = LE_read_uint08(ifile);
 if(ifile.fail()) return false;
 if(TGA_ColorMapType == 1 && TGA_ColorMapBitsPerPixel == 0) return false;

 // read origin coordinate
 uint16 TGA_XOrigin = LE_read_uint16(ifile);
 uint16 TGA_YOrigin = LE_read_uint16(ifile);
 if(ifile.fail()) return false;

 // read width
 uint16 TGA_ImageWidth = LE_read_uint16(ifile);
 if(ifile.fail()) return false;
 if(TGA_ImageWidth == 0) return false;

 // read height
 uint16 TGA_ImageHeight = LE_read_uint16(ifile);
 if(ifile.fail()) return false;
 if(TGA_ImageHeight == 0) return false;

 // read bits per pixel
 uint08 TGA_BitsPerPixel = LE_read_uint08(ifile);
 if(ifile.fail()) return false;
 if(TGA_BitsPerPixel == 0) return false;

 // read image descriptor
 uint08 TGA_ImageDescriptor = LE_read_uint08(ifile);
 if(ifile.fail()) return false;

 // skip past image ID data
 if(TGA_IdLength) {
    ifile.seekg(TGA_IdLength, ios::cur);
    if(ifile.fail()) return false;
   }

  // set image properties
  _rows = TGA_ImageHeight;
  _cols = TGA_ImageWidth;

 // check image type
 const uint08 type = (TGA_ImageType & 0x07);
 switch(type)
   {
    // no image data
    case(0x00) : {
         return false;
        }
    // uncompressed palette-based
    case(0x01) : {
         return false;
        }
    // uncompressed true-color
    case(0x02) :
        {
         // allocate data
         size_t _total_bytes = 4*_cols*_rows;
         _data.reset(new unsigned char[_total_bytes]);

         // read data
         ifile.read((char*)_data.get(), _total_bytes);
         if(ifile.fail()) {
            clear();
            return false;
           }

         // success
         return true;
        }
    // uncompressed grayscale
    case(0x03) : {
         return false;
        }
    // RLE palette-based
    case(0x09) : {
         return false;
        }
    // RLE true-color
    case(0x0A) : {
         return false;
        }
    // RLE grayscale
    case(0x0B) : {
         return false;
        }
    default : return false;
   }
 
 return true;
}

bool imageRGBA::loadDDS(const char* filename)
{
 return true;
}

bool imageRGBA::saveBMP(const char* filename)const
{
 return true;
}

bool imageRGBA::saveTGA(const char* filename)const
{
 // validate
 using namespace std;
 if(!filename || !strlen(filename)) return false;
 if(!_data.get()) return false;
 if(!_rows) return false;
 if(!_cols) return false;

 // open file
 ofstream ofile(filename, ios::binary);
 if(!ofile) return false;

 // save ID length
 LE_write_uint08(ofile, 0x00);
 if(ofile.fail()) return false;

 // save map type
 LE_write_uint08(ofile, 0x00);
 if(ofile.fail()) return false;

 // save image type
 LE_write_uint08(ofile, 0x02);
 if(ofile.fail()) return false;

 // save color map offset
 LE_write_uint16(ofile, 0x0000);
 if(ofile.fail()) return false;

 // save color map number of entries
 LE_write_uint16(ofile, 0x0000);
 if(ofile.fail()) return false;

 // save color map bits per pixel
 LE_write_uint08(ofile, 0x00);
 if(ofile.fail()) return false;

 // save origin coordinate
 LE_write_uint16(ofile, 0x0000);
 LE_write_uint16(ofile, 0x0000);
 if(ofile.fail()) return false;

 // save width
 LE_write_uint16(ofile, static_cast<uint16>(_cols));
 if(ofile.fail()) return false;

 // save height
 LE_write_uint16(ofile, static_cast<uint16>(_rows));
 if(ofile.fail()) return false;

 // read bits per pixel
 LE_write_uint08(ofile, 0x20);
 if(ofile.fail()) return false;

 // read image descriptor
 LE_write_uint08(ofile, 0x00);
 if(ofile.fail()) return false;

 // write data
 LE_write_array(ofile, _data.get(), total_bytes());
 if(ofile.fail()) return false;

 return true;
}

bool imageRGBA::saveDDS(const char* filename)const
{
 return true;
}

BOOL CreateBMPHeaders(DWORD dx, DWORD dy, DWORD bpp, BITMAPFILEHEADER* fileHeader, BITMAPINFOHEADER* infoHeader)
{
 // validate
 if(!fileHeader) return FALSE;
 if(!infoHeader) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;
 if(!(bpp == 1 || bpp == 4 || bpp == 8 || bpp == 16 || bpp == 24 || bpp == 32)) return FALSE;

 // set file header
 fileHeader->bfType = 0x4D42; 
 fileHeader->bfSize = 0;
 fileHeader->bfReserved1 = 0;
 fileHeader->bfReserved2 = 0;
 fileHeader->bfOffBits = 0;

 // set info header
 infoHeader->biSize = sizeof(BITMAPINFOHEADER);
 infoHeader->biWidth = dx;
 infoHeader->biHeight = dy;
 infoHeader->biPlanes = 1;
 infoHeader->biBitCount = (WORD)bpp;
 infoHeader->biCompression = BI_RGB;
 infoHeader->biSizeImage = 0;
 infoHeader->biXPelsPerMeter = 0;
 infoHeader->biYPelsPerMeter = 0;
 infoHeader->biClrUsed = 0;
 infoHeader->biClrImportant = 0;

 return TRUE;
}

BOOL SaveYUV411ToBMP(const char* filename, DWORD dx, DWORD dy, const char* data, uint32 size)
{
 return TRUE;
}

BOOL SaveYUV420ToBMP(const char* filename, DWORD dx, DWORD dy, const char* data, uint32 size)
{
 return TRUE;
}

BOOL SaveYUV422ToBMP(const char* filename, DWORD dx, DWORD dy, const char* data, uint32 size)
{
 // validate
 if(!filename || !strlen(filename)) return FALSE;
 if(!dx) return FALSE;
 if(!dy) return FALSE;
 if(!data || !size) return FALSE;

 // TODO: validate size of data

 // create bitmap headers
 BITMAPFILEHEADER fileHeader;
 BITMAPINFOHEADER infoHeader;
 if(!CreateBMPHeaders(dx, dy, 32, &fileHeader, &infoHeader)) return FALSE;

 // create BGRA data
 uint32 n = dx*dy*4;
 boost::shared_array<uint08> ptr(new uint08[n]);

 // for every 4 bytes
 uint32 index1 = 0;
 uint32 index2 = 0;
 while(index1 < size)
      {
       // constants
       real32 y1 = (real32)data[index1++];
       real32  u = (real32)data[index1++];
       real32 y2 = (real32)data[index1++];
       real32  v = (real32)data[index1++];

       // convert pixel #1
       real32 r1 = y1 + 1.13983f*v;
       real32 g1 = y1 - 0.39465f*u - 0.58060f*v;
       real32 b1 = y1 + 2.03211f*u;
       ptr[index2++] = (uint08)clamp(b1, 0.0f, 255.0f);
       ptr[index2++] = (uint08)clamp(g1, 0.0f, 255.0f);
       ptr[index2++] = (uint08)clamp(r1, 0.0f, 255.0f);
       ptr[index2++] = (uint08)255;

       // convert pixel #2
       real32 r2 = y2 + 1.13983f*v;
       real32 g2 = y2 - 0.39465f*u - 0.58060f*v;
       real32 b2 = y2 + 2.03211f*u;
       ptr[index2++] = (uint08)clamp(b2, 0.0f, 255.0f);
       ptr[index2++] = (uint08)clamp(g2, 0.0f, 255.0f);
       ptr[index2++] = (uint08)clamp(r2, 0.0f, 255.0f);
       ptr[index2++] = (uint08)255;
      }

 // create file
 using namespace std;
 ofstream ofile(filename, ios::binary);
 if(!ofile) return FALSE;

 // save file header
 ofile.write((char*)&fileHeader, sizeof(fileHeader));
 if(ofile.fail()) return FALSE;

 // save info header
 ofile.write((char*)&infoHeader, sizeof(infoHeader));
 if(ofile.fail()) return FALSE;

 // save data
 ofile.write((char*)ptr.get(), n);
 if(ofile.fail()) return FALSE;

 // success
 return TRUE;
}

BOOL SaveYUV444ToBMP(const char* filename, DWORD dx, DWORD dy, const char* data, uint32 size)
{
 return TRUE;
}

