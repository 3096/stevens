// Standard Headers
#include<fstream>

// CS Headers
#include "texture.h"

namespace cs { namespace cg {

texture::texture()
{
 image_type = TEXTURE_FORMAT_UNKNOWN;
 image_dx = 0;
 image_dy = 0;
 image_pitch = 0;
 image_bytes = 0;
}

texture::texture(uint32 w, uint32 h, texture_format f)
{
 // initialize properties
 image_type = TEXTURE_FORMAT_UNKNOWN;
 image_dx = w;
 image_dy = h;
 image_pitch = 0;
 image_bytes = 0;

 // assign properties
 switch(f) {
   case(TEXTURE_FORMAT_BGRA): {
        image_pitch = 4*image_dx;
        image_bytes = image_pitch*image_dy;
        break;
       }
   case(TEXTURE_FORMAT_BGR): {
        image_pitch = ((0x18ul*image_dx + 0x1Ful) & (~0x1Ful))/8;
        image_bytes = image_pitch*image_dy;
        break;
       }
   case(TEXTURE_FORMAT_UNKNOWN): {
        image_dx = 0;
        image_dy = 0;
        return;
       }
   default : {
        image_dx = 0;
        image_dy = 0;
        return;
       }
  }

 // create image
 image_type = f;
 image_data.reset(new unsigned char[image_bytes], std::default_delete<unsigned char[]>());
}

texture::texture(const texture& obj)
{
 image_data = obj.image_data;
 image_type = obj.image_type;
 image_dx = obj.image_dx;
 image_dy = obj.image_dy;
 image_pitch = obj.image_pitch;
 image_bytes = obj.image_bytes;
}

texture::texture(texture&& other)
{
 // move to this
 image_data = other.image_data;
 image_type = other.image_type;
 image_dx = other.image_dx;
 image_dy = other.image_dy;
 image_pitch = other.image_pitch;
 image_bytes = other.image_bytes;

 // delete other
 other.image_dx = 0;
 other.image_dy = 0;
 other.image_pitch = 0;
 other.image_bytes = 0;
}

texture& texture::operator =(const texture& other)
{
 if(this == &other) return *this;
 image_data = other.image_data;
 image_type = other.image_type;
 image_dx = other.image_dx;
 image_dy = other.image_dy;
 image_pitch = other.image_pitch;
 image_bytes = other.image_bytes;
 return *this;
}

texture& texture::operator =(texture&& other)
{
 // same object
 if(this == &other) return *this;

 // move to this
 image_data = other.image_data;
 image_type = other.image_type;
 image_dx = other.image_dx;
 image_dy = other.image_dy;
 image_pitch = other.image_pitch;
 image_bytes = other.image_bytes;

 // delete other
 other.image_data.reset();
 other.image_type = TEXTURE_FORMAT_UNKNOWN;
 other.image_dx = 0;
 other.image_dy = 0;
 other.image_pitch = 0;
 other.image_bytes = 0;

 // return this
 return *this;
}

texture::~texture()
{
}

bool texture::load(const char* filename)
{
 // open file
 std::ifstream ifile(filename, std::ios::binary);
 if(!ifile) return false;

 // read BITMAPFILEHEADER
 uint16 bfType;
 uint32 bfSize;
 uint16 bfReserved1;
 uint16 bfReserved2;
 uint32 bfOffBits;
 ifile.read((char*)&bfType, sizeof(uint16));
 ifile.read((char*)&bfSize, sizeof(uint32));
 ifile.read((char*)&bfReserved1, sizeof(uint16));
 ifile.read((char*)&bfReserved2, sizeof(uint16));
 ifile.read((char*)&bfOffBits, sizeof(uint32));
 if(ifile.fail()) return false;

 // validate BITMAPFILEHEADER
 if(bfType != 0x4D42) return false;

 // read BITMAPINFOHEADER
 uint32 biSize;
 sint32 biWidth;
 sint32 biHeight;
 uint16 biPlanes;
 uint16 biBitCount;
 uint32 biCompression;
 uint32 biSizeImage;
 sint32 biXPelsPerMeter;
 sint32 biYPelsPerMeter;
 uint32 biClrUsed;
 uint32 biClrImportant;
 ifile.read((char*)&biSize, sizeof(uint32));
 ifile.read((char*)&biWidth, sizeof(sint32));
 ifile.read((char*)&biHeight, sizeof(sint32));
 ifile.read((char*)&biPlanes, sizeof(uint16));
 ifile.read((char*)&biBitCount, sizeof(uint16));
 ifile.read((char*)&biCompression, sizeof(uint32));
 ifile.read((char*)&biSizeImage, sizeof(uint32));
 ifile.read((char*)&biXPelsPerMeter, sizeof(sint32));
 ifile.read((char*)&biYPelsPerMeter, sizeof(sint32));
 ifile.read((char*)&biClrUsed, sizeof(uint32));
 ifile.read((char*)&biClrImportant, sizeof(uint32));
 if(ifile.fail()) return false;

 // validate BITMAPINFOHEADER
 if(biSize != 40) return false;
 if(biWidth < 1 || biHeight < 1) return false;
 if(biPlanes != 1) return false;
 if(!(biBitCount == 24 || biBitCount == 32)) return false;
 if(biCompression != 0) return false;

 // compute offset to data
 uint32 offset = 54;
 if(bfOffBits != 0) offset = bfOffBits;

 // move to data
 ifile.seekg(offset);
 if(ifile.fail()) return false;

 // read data
 uint32 biPitch = ((biBitCount*biWidth + 0x1F) & (~0x1F))/8;
 uint32 biDataSize = biPitch*biHeight;
 std::shared_ptr<unsigned char> biData(new unsigned char[biDataSize], std::default_delete<unsigned char[]>());
 ifile.read(reinterpret_cast<char*>(biData.get()), biDataSize);
 if(ifile.fail()) return false;

 // flip image
 uint32 row_offset1 = 0;
 uint32 row_offset2 = (biHeight - 1)*biPitch;
 for(sint32 r = 0; r < biHeight/2; r++) {
     uint32 index1 = row_offset1;
     uint32 index2 = row_offset2;
     for(uint32 c = 0; c < biPitch; c++) std::swap(biData.get()[index1++], biData.get()[index2++]);
     row_offset1 += biPitch;
     row_offset2 -= biPitch;
    }

 // assign data
 image_data = biData;
 if(biBitCount == 24) image_type = TEXTURE_FORMAT_BGR;
 else if(biBitCount == 32) image_type = TEXTURE_FORMAT_BGRA;
 image_dx = biWidth;
 image_dy = biHeight;
 image_pitch = biPitch;
 image_bytes = biDataSize;
 return true;
}

void texture::free(void)
{
 image_data.reset();
 image_type = TEXTURE_FORMAT_UNKNOWN;
 image_dx = 0;
 image_dy = 0;
 image_pitch = 0;
 image_bytes = 0;
}

array4D<unsigned char> texture::sample(binary32 u, binary32 v)
{
 // obtain image coordinates from UV
 uint32 x = static_cast<uint32>((image_dx - 1)*u);
 uint32 y = static_cast<uint32>((image_dy - 1)*(1.0f - v));

 // sample image (this is very inefficient)
 array4D<unsigned char> result;
 if(image_type == TEXTURE_FORMAT_BGRA) {
    const unsigned char* ptr = image_data.get();
    uint32 index = y*image_pitch + 4*x;
    result[0] = ptr[index++];
    result[1] = ptr[index++];
    result[2] = ptr[index++];
    result[3] = ptr[index];
   }
 else if(image_type == TEXTURE_FORMAT_BGR) {
    const unsigned char* ptr = image_data.get();
    uint32 index = y*image_pitch + 3*x;
    result[0] = ptr[index++];
    result[1] = ptr[index++];
    result[2] = ptr[index];
    result[3] = 255;
   }
 else {
    result[0] = 255;
    result[1] = 0;
    result[2] = 0;
    result[3] = 255;
   }

 return result;
}

}}