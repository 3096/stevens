#ifndef __XENTAX_DDS_H
#define __XENTAX_DDS_H

typedef struct {
  DWORD dwSize;
  DWORD dwFlags;
  DWORD dwFourCC;
  DWORD dwRGBBitCount;
  DWORD dwRBitMask;
  DWORD dwGBitMask;
  DWORD dwBBitMask;
  DWORD dwABitMask;
} DDS_PIXELFORMAT, DDSPF;

typedef struct {
  DWORD dwSize;
  DWORD dwFlags;
  DWORD dwHeight;
  DWORD dwWidth;
  DWORD dwPitchOrLinearSize;
  DWORD dwDepth;
  DWORD dwMipMapCount;
  DWORD dwReserved01;
  DWORD dwReserved02;
  DWORD dwReserved03;
  DWORD dwReserved04;
  DWORD dwReserved05;
  DWORD dwReserved06;
  DWORD dwReserved07;
  DWORD dwReserved08;
  DWORD dwReserved09;
  DWORD dwReserved10;
  DWORD dwReserved11;
  DDSPF ddspf;
  DWORD dwCaps1;
  DWORD dwCaps2;
  DWORD dwCaps3;
  DWORD dwCaps4;
  DWORD dwReserved12;
} DDS_HEADER;

typedef struct {
  INT dxgiFormat;
  INT resourceDimension;
  UINT miscFlag;
  UINT arraySize;
  UINT miscFlags2;
} DDS_HEADER_DXT10;

BOOL CreateUncompressedDDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, DWORD maskR, DWORD maskG, DWORD maskB, DWORD maskA, BOOL cubemap, DDS_HEADER* header);
BOOL CreateUncompressedDDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, DWORD bitCount, DWORD maskR, DWORD maskG, DWORD maskB, DWORD maskA, BOOL cubemap, DDS_HEADER* header);
BOOL CreateGrayscaleDDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateGrayscale16DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);

// 08-bit RGBA formats
BOOL CreateA8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateL8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateR8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateA4L4DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateL4A4DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateB2G3R3DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateR3G3B2DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);

// 32-bit RGBA formats
BOOL CreateA8R8G8B8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateX8R8G8B8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateA8B8G8R8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateX8B8G8R8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateB8G8R8A8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateB8G8R8X8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateR8G8B8A8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateR8G8B8X8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateA2R10G10B10DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateA2B10G10R10DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);

// 32-bit YUVA formats
BOOL CreateY8U8V8A8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateY8U8V8X8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateA8Y8U8V8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateX8Y8U8V8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);

// 24-bit RGBA formats
BOOL CreateR8G8B8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);

// 16-bit RGBA formats
BOOL CreateR5G6B5DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateA1R5G5B5DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateX1R5G5B5DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateA4R4G4B4DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateX4R4G4B4DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);

// Luminosity formats
BOOL CreateA8L8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateL16DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);

// DX10 formats
BOOL CreateR16G16B16A16FloatHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend);
BOOL CreateBC4Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend);
BOOL CreateBC5Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend);
BOOL CreateBC6Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend);
BOOL CreateBC7Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend);

// DX10 data sizes
DWORD GetBC4DataSize(DWORD dx, DWORD dy, DWORD mipmaps);
DWORD GetBC5DataSize(DWORD dx, DWORD dy, DWORD mipmaps);
DWORD GetBC6DataSize(DWORD dx, DWORD dy, DWORD mipmaps);
DWORD GetBC7DataSize(DWORD dx, DWORD dy, DWORD mipmaps);

// DXT formats
BOOL CreateDXT1Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateDXT2Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateDXT3Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateDXT4Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateDXT5Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateATI1Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);
BOOL CreateATI2Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header);

// DDS File Sizes
DWORD UncompressedDDSFileSize(DWORD dx, DWORD dy, DWORD mipmaps, DWORD bitCount);
DWORD DXT1Filesize(DWORD dx, DWORD dy, DWORD mipmaps);
DWORD DXT2Filesize(DWORD dx, DWORD dy, DWORD mipmaps);
DWORD DXT3Filesize(DWORD dx, DWORD dy, DWORD mipmaps);
DWORD DXT4Filesize(DWORD dx, DWORD dy, DWORD mipmaps);
DWORD DXT5Filesize(DWORD dx, DWORD dy, DWORD mipmaps);
DWORD ATI1Filesize(DWORD dx, DWORD dy, DWORD mipmaps);
DWORD ATI2Filesize(DWORD dx, DWORD dy, DWORD mipmaps);

DWORD ComputeMipMapNumber(DWORD dx, DWORD dy);
DWORD ComputeMipMapNumber(DWORD dx, DWORD dy, DWORD min_dx, DWORD min_dy);
BOOL SaveDDSFile(LPCTSTR filename, const DDS_HEADER& ddsh, boost::shared_array<char> data, uint32 size);
BOOL SaveDDS10File(LPCTSTR filename, const DDS_HEADER& ddsh, const DDS_HEADER_DXT10& extend, boost::shared_array<char> data, uint32 size);

// Conversion Functions
BOOL R16G16B16A16FloatToR8G8B8A8Byte(char* dst, const char* src, uint32 dx, uint32 dy);
BOOL R32G32B32A32FloatToR8G8B8A8Byte(char* dst, const char* src, uint32 dx, uint32 dy);
BOOL BC7ToA8R8G8B8Byte(char* dst, const char* src, uint32 dx, uint32 dy);
BOOL BC7ToB8G8R8A8Byte(char* dst, const char* src, uint32 dx, uint32 dy);
BOOL DXT1BlockUnswizzle(char* dst, const char* src, uint32 dx, uint32 dy);

#endif
