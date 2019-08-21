#include "xentax.h"
#include "x_dds.h"
#include "../libraries/bc7/avpcl.h"

//
// 08-BIT UNCOMPRESSED FORMAT
//

BOOL CreateL8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 8;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_LUMINANCE;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0xFF;
 header->ddspf.dwGBitMask = 0xFF;
 header->ddspf.dwBBitMask = 0xFF;
 header->ddspf.dwABitMask = 0x00;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateR8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 return CreateL8DDSHeader(dx, dy, mipmaps, cubemap, header);
}

BOOL CreateA4L4DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 8;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_LUMINANCE | DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x0F;
 header->ddspf.dwGBitMask = 0x0F;
 header->ddspf.dwBBitMask = 0x0F;
 header->ddspf.dwABitMask = 0xF0;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateA8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
  // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 8;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_ALPHA;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x00;
 header->ddspf.dwGBitMask = 0x00;
 header->ddspf.dwBBitMask = 0x00;
 header->ddspf.dwABitMask = 0xFF;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateL4A4DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 8;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_LUMINANCE | DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0xF0;
 header->ddspf.dwGBitMask = 0xF0;
 header->ddspf.dwBBitMask = 0xF0;
 header->ddspf.dwABitMask = 0x0F;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateB2G3R3DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
  // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 8;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x07; // 00000111
 header->ddspf.dwGBitMask = 0x38; // 00111000
 header->ddspf.dwBBitMask = 0xC0; // 11000000
 header->ddspf.dwABitMask = 0x00;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateR3G3B2DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
  // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 8;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0xE0;
 header->ddspf.dwGBitMask = 0x1C;
 header->ddspf.dwBBitMask = 0x03;
 header->ddspf.dwABitMask = 0x00;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// OLD CODE
//

BOOL CreateUncompressedDDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, DWORD maskR, DWORD maskG, DWORD maskB, DWORD maskA, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 0;
 if(maskR) bpp += 8;
 if(maskG) bpp += 8;
 if(maskB) bpp += 8;
 if(maskA) bpp += 8;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 if(maskA) header->ddspf.dwFlags |= DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = maskR;
 header->ddspf.dwGBitMask = maskG;
 header->ddspf.dwBBitMask = maskB;
 header->ddspf.dwABitMask = maskA;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateUncompressedDDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, DWORD bitCount, DWORD maskR, DWORD maskG, DWORD maskB, DWORD maskA, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;
 if(!(bitCount == 0x08 || bitCount == 0x10 || bitCount == 0x18 || bitCount == 0x20)) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = bitCount;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 if(maskA) header->ddspf.dwFlags |= DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = maskR;
 header->ddspf.dwGBitMask = maskG;
 header->ddspf.dwBBitMask = maskB;
 header->ddspf.dwABitMask = maskA;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateGrayscaleDDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 uint32 maskR = 0xFF;
 uint32 maskG = 0xFF;
 uint32 maskB = 0xFF;
 uint32 maskA = 0x00;
 return CreateUncompressedDDSHeader(dx, dy, mipmaps, 8, maskR, maskG, maskB, maskA, cubemap, header);
}

BOOL CreateGrayscale16DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 16;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0xFFFF;
 header->ddspf.dwGBitMask = 0xFFFF;
 header->ddspf.dwBBitMask = 0xFFFF;
 header->ddspf.dwABitMask = 0x00;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_A8R8G8B8
// 32-bit RGBA format
//
BOOL CreateA8R8G8B8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
  // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD bpp = 32;
 header->dwPitchOrLinearSize = 4*dx*dy;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x00FF0000ul;
 header->ddspf.dwGBitMask = 0x0000FF00ul;
 header->ddspf.dwBBitMask = 0x000000FFul;
 header->ddspf.dwABitMask = 0xFF000000ul;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_X8R8G8B8
// 32-bit RGB format
//
BOOL CreateX8R8G8B8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
  // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 32;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x00FF0000;
 header->ddspf.dwGBitMask = 0x0000FF00;
 header->ddspf.dwBBitMask = 0x000000FF;
 header->ddspf.dwABitMask = 0x00000000;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_A8B8G8R8
// 32-bit RGBA format
//
BOOL CreateA8B8G8R8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 uint32 maskR = 0x000000FF;
 uint32 maskG = 0x0000FF00;
 uint32 maskB = 0x00FF0000;
 uint32 maskA = 0xFF000000;
 return CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, maskR, maskG, maskB, maskA, cubemap, header);
}

//
// D3DFMT_X8B8G8R8
// 32-bit RGB format
//
BOOL CreateX8B8G8R8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 uint32 maskR = 0x000000FF;
 uint32 maskG = 0x0000FF00;
 uint32 maskB = 0x00FF0000;
 uint32 maskA = 0x00000000; // no alpha
 return CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, maskR, maskG, maskB, maskA, cubemap, header);
}

//
// D3DFMT_B8G8R8A8
// 32-bit RGBA format
//
BOOL CreateB8G8R8A8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 uint32 maskR = 0x0000FF00;
 uint32 maskG = 0x00FF0000;
 uint32 maskB = 0xFF000000;
 uint32 maskA = 0x000000FF;
 return CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, maskR, maskG, maskB, maskA, cubemap, header);
}

//
// D3DFMT_B8G8R8X8
// 32-bit RGB format
//
BOOL CreateB8G8R8X8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 uint32 maskR = 0x0000FF00;
 uint32 maskG = 0x00FF0000;
 uint32 maskB = 0xFF000000;
 uint32 maskA = 0x00000000; // no alpha
 return CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, maskR, maskG, maskB, maskA, cubemap, header);
}

//
// D3DFMT_A2R10G10B10
// 32-bit RGB format
//
BOOL CreateA2R10G10B10DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 uint32 maskR = 0x3FF00000;
 uint32 maskG = 0x000FFC00;
 uint32 maskB = 0x000003FF;
 uint32 maskA = 0xC0000000;
 return CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, maskR, maskG, maskB, maskA, cubemap, header);
}

BOOL CreateR8G8B8A8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 uint32 maskR = 0xFF000000;
 uint32 maskG = 0x00FF0000;
 uint32 maskB = 0x0000FF00;
 uint32 maskA = 0x000000FF;
 return CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, maskR, maskG, maskB, maskA, cubemap, header);
}

BOOL CreateR8G8B8X8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 uint32 maskR = 0xFF000000;
 uint32 maskG = 0x00FF0000;
 uint32 maskB = 0x0000FF00;
 uint32 maskA = 0x00000000; // no alpha
 return CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, maskR, maskG, maskB, maskA, cubemap, header);
}

//
// D3DFMT_A2B10G10R10
// 32-bit RGB format
//
BOOL CreateA2B10G10R10DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 uint32 maskR = 0x000003FF;
 uint32 maskG = 0x000FFC00;
 uint32 maskB = 0x3FF00000;
 uint32 maskA = 0xC0000000;
 return CreateUncompressedDDSHeader(dx, dy, mipmaps, 32, maskR, maskG, maskB, maskA, cubemap, header);
}

BOOL CreateY8U8V8A8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 32;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_YUV | DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0xFF000000;
 header->ddspf.dwGBitMask = 0x00FF0000;
 header->ddspf.dwBBitMask = 0x0000FF00;
 header->ddspf.dwABitMask = 0x000000FF;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateY8U8V8X8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 32;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_YUV;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0xFF000000;
 header->ddspf.dwGBitMask = 0x00FF0000;
 header->ddspf.dwBBitMask = 0x0000FF00;
 header->ddspf.dwABitMask = 0x00000000; // no alpha

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateA8Y8U8V8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 32;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_YUV | DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x00FF0000;
 header->ddspf.dwGBitMask = 0x0000FF00;
 header->ddspf.dwBBitMask = 0x000000FF;
 header->ddspf.dwABitMask = 0xFF000000;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateX8Y8U8V8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 32;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_YUV;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x00FF0000;
 header->ddspf.dwGBitMask = 0x0000FF00;
 header->ddspf.dwBBitMask = 0x000000FF;
 header->ddspf.dwABitMask = 0x00000000; // no alpha

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_R5G6B5
// 24-bit RGB format
//
BOOL CreateR8G8B8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 24;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0xFF0000;
 header->ddspf.dwGBitMask = 0x00FF00;
 header->ddspf.dwBBitMask = 0x0000FF;
 header->ddspf.dwABitMask = 0x000000;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_R5G6B5
// 16-bit RGB format
//
BOOL CreateR5G6B5DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 16;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0xF800; // 1111100000000000
 header->ddspf.dwGBitMask = 0x07E0; // 0000011111100000
 header->ddspf.dwBBitMask = 0x001F; // 0000000000011111
 header->ddspf.dwABitMask = 0x0000; // 0000000000000000

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_A1R5G5B5
// 16-bit ARGB format
//
BOOL CreateA1R5G5B5DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
  // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 16;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x7C00; // 0111110000000000
 header->ddspf.dwGBitMask = 0x03E0; // 0000001111100000
 header->ddspf.dwBBitMask = 0x001F; // 0000000000011111
 header->ddspf.dwABitMask = 0x8000; // 1000000000000000

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_X1R5G5B5
// 16-bit RGB format
//
BOOL CreateX1R5G5B5DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
  // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 16;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x7C00; // 0111110000000000
 header->ddspf.dwGBitMask = 0x03E0; // 0000001111100000
 header->ddspf.dwBBitMask = 0x001F; // 0000000000011111
 header->ddspf.dwABitMask = 0x0000; // 0000000000000000

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_A4R4G4B4
// 16-bit ARGB format
//
BOOL CreateA4R4G4B4DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
  // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 16;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x0F00; // 0000111100000000
 header->ddspf.dwGBitMask = 0x00F0; // 0000000011110000
 header->ddspf.dwBBitMask = 0x000F; // 0000000000001111
 header->ddspf.dwABitMask = 0xF000; // 1111000000000000

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_X4R4G4B4
// 16-bit RGB format
//
BOOL CreateX4R4G4B4DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 16;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_RGB;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x0F00; // 0000111100000000
 header->ddspf.dwGBitMask = 0x00F0; // 0000000011110000
 header->ddspf.dwBBitMask = 0x000F; // 0000000000001111
 header->ddspf.dwABitMask = 0x0000; // 0000000000000000

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_A8L8
// 16-bit Luminosity format
//
BOOL CreateA8L8DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 16;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_LUMINANCE | DDPF_ALPHAPIXELS;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0x00FF;
 header->ddspf.dwGBitMask = 0x0000;
 header->ddspf.dwBBitMask = 0x0000;
 header->ddspf.dwABitMask = 0xFF00;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

//
// D3DFMT_L16
// 16-bit Luminosity format
//
BOOL CreateL16DDSHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 16;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_LUMINANCE;
 header->ddspf.dwFourCC = 0;
 header->ddspf.dwRGBBitCount = bpp;
 header->ddspf.dwRBitMask = 0xFFFF;
 header->ddspf.dwGBitMask = 0x0000;
 header->ddspf.dwBBitMask = 0x0000;
 header->ddspf.dwABitMask = 0x0000;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateDXT1Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD blocksize = 8;
 DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 header->dwPitchOrLinearSize = rows*cols*blocksize;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x31545844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateDXT2Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD blocksize = 16;
 DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 header->dwPitchOrLinearSize = rows*cols*blocksize;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x32545844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateDXT3Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD blocksize = 16;
 DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 header->dwPitchOrLinearSize = rows*cols*blocksize;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x33545844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateDXT4Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD blocksize = 16;
 DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 header->dwPitchOrLinearSize = rows*cols*blocksize;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x34545844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateDXT5Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD blocksize = 16;
 DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 header->dwPitchOrLinearSize = rows*cols*blocksize;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x35545844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 return TRUE;
}

BOOL CreateATI1Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // same as DXT1 but with different FOURCC
 if(!CreateDXT1Header(dx, dy, mipmaps, cubemap, header)) return FALSE;
 header->ddspf.dwFourCC = 0x31495441;
 return TRUE;
}

BOOL CreateATI2Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header)
{
 // same as DXT5 but with different FOURCC
 if(!CreateDXT5Header(dx, dy, mipmaps, cubemap, header)) return FALSE;
 header->ddspf.dwFourCC = 0x32495441;
 return TRUE;
}

//
// DX10 FORMATS
//

BOOL CreateR16G16B16A16FloatHeader(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend)
{
  // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set pitch
 DWORD bpp = 64;
 header->dwPitchOrLinearSize = (bpp*dx + 7)/8;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x30315844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 // Direct3D header
 extend->dxgiFormat = 10; // DXGI_FORMAT_R16G16B16A16_FLOAT
 extend->resourceDimension = 3; // D3D10_RESOURCE_DIMENSION_TEXTURE2D
 extend->miscFlag = 0;
 extend->arraySize = 0;
 extend->miscFlags2 = 1; // DDS_ALPHA_MODE_STRAIGHT

 return TRUE;
}

BOOL CreateBC4Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD blocksize = 8;
 DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 header->dwPitchOrLinearSize = rows*cols*blocksize;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x30315844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 // Direct3D header
 extend->dxgiFormat = 80; // DXGI_FORMAT_BC4_UNORM (can also use _UNORM_SRGB)
 extend->resourceDimension = 3; // D3D10_RESOURCE_DIMENSION_TEXTURE2D
 extend->miscFlag = 0;
 extend->arraySize = 0;
 extend->miscFlags2 = 0; // DDS_ALPHA_MODE_UNKNOWN

 return TRUE;
}

BOOL CreateBC5Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD blocksize = 16;
 DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 header->dwPitchOrLinearSize = rows*cols*blocksize;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x30315844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 // Direct3D header
 extend->dxgiFormat = 83; // DXGI_FORMAT_BC5_UNORM
 extend->resourceDimension = 3; // D3D10_RESOURCE_DIMENSION_TEXTURE2D
 extend->miscFlag = 0;
 extend->arraySize = 0;
 extend->miscFlags2 = 0; // DDS_ALPHA_MODE_UNKNOWN

 return TRUE;
}

BOOL CreateBC6Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD blocksize = 16;
 DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 header->dwPitchOrLinearSize = rows*cols*blocksize;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x30315844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 // Direct3D header
 extend->dxgiFormat = 95; // DXGI_FORMAT_BC6H_UF16
 extend->resourceDimension = 3; // D3D10_RESOURCE_DIMENSION_TEXTURE2D
 extend->miscFlag = (cubemap ? 4 : 0); // D3D10_RESOURCE_MISC_TEXTURECUBE
 extend->arraySize = (cubemap ? 6 : 0);
 extend->miscFlags2 = 1; // D3D10_ALPHA_MODE_STRAIGHT

 return TRUE;
}

BOOL CreateBC7Header(DWORD dx, DWORD dy, DWORD mipmaps, BOOL cubemap, DDS_HEADER* header, DDS_HEADER_DXT10* extend)
{
 // validate
 if(!header) return FALSE;
 if(dx == 0) return FALSE;
 if(dy == 0) return FALSE;

 // set header size
 header->dwSize = 124;

 // set flags
 header->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_PIXELFORMAT;
 if(mipmaps) header->dwFlags |= DDSD_MIPMAPCOUNT;

 // set dimensions
 header->dwHeight = dy;
 header->dwWidth  = dx;
 header->dwDepth  = 0;

 // set linear size
 DWORD blocksize = 16;
 DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
 DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));
 header->dwPitchOrLinearSize = rows*cols*blocksize;

 // set mipmap count
 header->dwMipMapCount = mipmaps;

 // set reserved data
 header->dwReserved01 = 0;
 header->dwReserved02 = 0;
 header->dwReserved03 = 0;
 header->dwReserved04 = 0;
 header->dwReserved05 = 0;
 header->dwReserved06 = 0;
 header->dwReserved07 = 0;
 header->dwReserved08 = 0;
 header->dwReserved09 = 0;
 header->dwReserved10 = 0;
 header->dwReserved11 = 0;

 // set pixel format
 header->ddspf.dwSize = 32;
 header->ddspf.dwFlags = DDPF_FOURCC;
 header->ddspf.dwFourCC = 0x30315844;

 // set capabilities
 DWORD DDS_CUBEMAP_ALLFACES = 0xFC00;
 header->dwCaps1 = DDSCAPS_TEXTURE;
 if(mipmaps) header->dwCaps1 |= (DDSCAPS_COMPLEX | DDSCAPS_MIPMAP);
 header->dwCaps2 = 0;
 if(cubemap) header->dwCaps2 |= (DDSCAPS2_CUBEMAP | DDS_CUBEMAP_ALLFACES);
 header->dwCaps3 = 0;
 header->dwCaps4 = 0;
 header->dwReserved12 = 0;

 // Direct3D header
 extend->dxgiFormat = 98; // DXGI_FORMAT_BC7_UNORM (can use 99 as well)
 extend->resourceDimension = 3; // D3D10_RESOURCE_DIMENSION_TEXTURE2D
 extend->miscFlag = 0;
 extend->arraySize = 0;
 extend->miscFlags2 = 1; // DDS_ALPHA_MODE_STRAIGHT

 return TRUE;
}

DWORD GetBC4DataSize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 return DXT1Filesize(dx, dy, mipmaps);
}

DWORD GetBC5DataSize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 return DXT5Filesize(dx, dy, mipmaps);
}

DWORD GetBC6DataSize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 return DXT5Filesize(dx, dy, mipmaps);
}

DWORD GetBC7DataSize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 return DXT5Filesize(dx, dy, mipmaps);
}

DWORD UncompressedDDSFileSize(DWORD dx, DWORD dy, DWORD mipmaps, DWORD bitCount)
{
 // validate
 if(dx == 0) return 0;
 if(dy == 0) return 0;
 if(bitCount == 0) return 0;

 // compute filesize
 DWORD bpp = bitCount;
 DWORD filesize = ((bpp*dx + 7)/8)*dy;

 // compute mipmap contribution
 for(DWORD i = 0; i < mipmaps; i++) {
     dx /= 2;
     dy /= 2;
     filesize += ((bpp*dx + 7)/8)*dy;
    }

 return filesize;
}

DWORD UncompressedDDSFileSize(DWORD dx, DWORD dy, DWORD mipmaps, DWORD maskR, DWORD maskG, DWORD maskB, DWORD maskA)
{
 // validate
 if(dx == 0) return 0;
 if(dy == 0) return 0;

 // compute filesize
 DWORD bpp = 0;
 if(maskR) bpp += 8;
 if(maskG) bpp += 8;
 if(maskB) bpp += 8;
 if(maskA) bpp += 8;
 DWORD filesize = ((bpp*dx + 7)/8)*dy;

 // compute mipmap contribution
 for(DWORD i = 0; i < mipmaps; i++) {
     dx /= 2;
     dy /= 2;
     filesize += ((bpp*dx + 7)/8)*dy;
    }

 return filesize;
}

DWORD DXT1Filesize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 // validate
 if(dx == 0) return 0;
 if(dy == 0) return 0;

 uint32 total_size = 0;
 for(uint32 i = 0; i < (mipmaps + 1); i++)
    {
     // set linear size
     DWORD blocksize = 8;
     DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
     DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));

     // add to total size
     total_size += rows*cols*blocksize;

     // divide size by 2
     dx /= 2; if(!dx) break;
     dy /= 2; if(!dy) break;
    }

 // return total size
 return total_size;
}

DWORD DXT2Filesize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 // validate
 if(dx == 0) return 0;
 if(dy == 0) return 0;

 uint32 total_size = 0;
 for(uint32 i = 0; i < (mipmaps + 1); i++)
    {
     // set linear size
     DWORD blocksize = 16;
     DWORD rows = std::max((DWORD)1, (DWORD)((dy + 3)/4));
     DWORD cols = std::max((DWORD)1, (DWORD)((dx + 3)/4));

     // add to total size
     total_size += rows*cols*blocksize;

     // divide size by 2
     dx /= 2; if(!dx) break;
     dy /= 2; if(!dy) break;
    }

 // return total size
 return total_size;
}

DWORD DXT3Filesize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 return DXT2Filesize(dx, dy, mipmaps);
}

DWORD DXT4Filesize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 return DXT2Filesize(dx, dy, mipmaps);
}

DWORD DXT5Filesize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 return DXT2Filesize(dx, dy, mipmaps);
}

DWORD ATI1Filesize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 // 8 bytes per 16 pixels (4x4 block)
 return DXT1Filesize(dx, dy, mipmaps);
}

DWORD ATI2Filesize(DWORD dx, DWORD dy, DWORD mipmaps)
{
 // 16 bytes per 16 pixels (4x4 block)
 return DXT5Filesize(dx, dy, mipmaps);
}

DWORD ComputeMipMapNumber(DWORD dx, DWORD dy)
{
 DWORD retval = 0;
 dx /= 2;
 dy /= 2;
 while(dx && dy) {
       retval++;
       dx /= 2;
       dy /= 2;
      }
 return retval;
}

DWORD ComputeMipMapNumber(DWORD dx, DWORD dy, DWORD min_dx, DWORD min_dy)
{
 // validate arguments
 if(dx < min_dx) return 0;
 if(dy < min_dy) return 0;

 // stop when min is reached
 DWORD retval = 0;
 dx /= 2;
 dy /= 2;
 while(dx && dy) {
       if(dx < min_dx) break;
       if(dy < min_dy) break;
       retval++;
       dx /= 2;
       dy /= 2;
      }
 return retval;
}

BOOL SaveDDSFile(LPCTSTR filename, const DDS_HEADER& ddsh, boost::shared_array<char> data, uint32 size)
{
 // validate
 if(!filename) return FALSE;
 if(!data.get() || !size) return FALSE;

 // create file
 using namespace std;
 ofstream ofile(filename, ios::binary);
 if(!ofile) return FALSE;

 // save data
 BE_write_uint32(ofile, 0x44445320);
 BE_write_array(ofile, (char*)&ddsh, sizeof(ddsh));
 BE_write_array(ofile, data.get(), (size_t)size);

 return TRUE;
}

BOOL SaveDDS10File(LPCTSTR filename, const DDS_HEADER& ddsh, const DDS_HEADER_DXT10& extend, boost::shared_array<char> data, uint32 size)
{
 // validate
 if(!filename) return FALSE;
 if(!data.get() || !size) return FALSE;

 // create file
 using namespace std;
 ofstream ofile(filename, ios::binary);
 if(!ofile) return FALSE;

 // save data
 BE_write_uint32(ofile, 0x44445320);
 LE_write_array(ofile, (char*)&ddsh, sizeof(ddsh));
 LE_write_array(ofile, (char*)&extend, sizeof(extend));
 LE_write_array(ofile, data.get(), (size_t)size);

 return true;
}

//
// Conversion Functions
//

// used by Resident Evil games
BOOL R16G16B16A16FloatToR8G8B8A8Byte(char* dst, const char* src, uint32 dx, uint32 dy)
{
 // convert source to const uint16*
 const uint16* srcptr = reinterpret_cast<const uint16*>(src);
 if(!srcptr) return FALSE;

 // convert destination to unsigned bytes
 uint08* dstptr = reinterpret_cast<uint08*>(dst);
 if(!dstptr) return FALSE;

 // convert
 uint32 elem = 4*dx*dy;
 for(uint32 i = 0; i < elem; i++) {
     real32 value = float_16_to_32(srcptr[i])*255;
     dst[i] = static_cast<uint08>(value);
    }

 return TRUE;
}

// used by Resident Evil games
BOOL R32G32B32A32FloatToR8G8B8A8Byte(char* dst, const char* src, uint32 dx, uint32 dy)
{
 // convert source to const real32*
 const real32* srcptr = reinterpret_cast<const real32*>(src);
 if(!srcptr) return FALSE;

 // convert destination to unsigned bytes
 uint08* dstptr = reinterpret_cast<uint08*>(dst);
 if(!dstptr) return FALSE;

 // convert
 uint32 elem = 4*dx*dy;
 for(uint32 i = 0; i < elem; i++) {
     real32 value = srcptr[i]*255;
     dst[i] = static_cast<uint08>(value);
    }

 return TRUE;
}

BOOL BC7ToA8R8G8B8Byte(char* dst, const char* src, uint32 dx, uint32 dy)
{
 // block properties
 const unsigned int blocksize = 16;
 const unsigned int block_dx = 4;
 const unsigned int block_dy = 4;

 // convert destination to unsigned bytes
 uint08* dstptr = reinterpret_cast<uint08*>(dst);
 if(!dstptr) return FALSE;

 // compute BC7 data size
 uint32 block_rows = std::max((DWORD)1, (DWORD)((dy + 3)/block_dy));
 uint32 block_cols = std::max((DWORD)1, (DWORD)((dx + 3)/block_dx));
 uint32 n_blocks = block_rows*block_cols;
 uint32 datasize = blocksize*n_blocks;

 // premultiplied
 uint32 pitch = 4*dx;
 uint32 left_offset[4] = { 0, pitch, 2*pitch, 3*pitch };

 // for each block
 const char* block = src;
 Tile t(4, 4);
 for(uint32 i = 0; i < n_blocks; i++)
    {
     // decompress
     AVPCL::decompress(&block[0], t);

     // now map data to RGBA image
     uint32 base_x = (i % block_cols) * block_dx;
     uint32 base_y = (i / block_cols) * block_dy;
     uint32 base_index = 4*(dx*base_y + base_x);

     // row1
     uint32 offset = base_index;
     dstptr[offset++] = (uint08)t.data[0][0].W();
     dstptr[offset++] = (uint08)t.data[0][0].X();
     dstptr[offset++] = (uint08)t.data[0][0].Y();
     dstptr[offset++] = (uint08)t.data[0][0].Z();
     dstptr[offset++] = (uint08)t.data[0][1].W();
     dstptr[offset++] = (uint08)t.data[0][1].X();
     dstptr[offset++] = (uint08)t.data[0][1].Y();
     dstptr[offset++] = (uint08)t.data[0][1].Z();
     dstptr[offset++] = (uint08)t.data[0][2].W();
     dstptr[offset++] = (uint08)t.data[0][2].X();
     dstptr[offset++] = (uint08)t.data[0][2].Y();
     dstptr[offset++] = (uint08)t.data[0][2].Z();
     dstptr[offset++] = (uint08)t.data[0][3].W();
     dstptr[offset++] = (uint08)t.data[0][3].X();
     dstptr[offset++] = (uint08)t.data[0][3].Y();
     dstptr[offset]   = (uint08)t.data[0][3].Z();
   
     // row2
     offset = base_index + left_offset[1];
     dstptr[offset++] = (uint08)t.data[1][0].W();
     dstptr[offset++] = (uint08)t.data[1][0].X();
     dstptr[offset++] = (uint08)t.data[1][0].Y();
     dstptr[offset++] = (uint08)t.data[1][0].Z();
     dstptr[offset++] = (uint08)t.data[1][1].W();
     dstptr[offset++] = (uint08)t.data[1][1].X();
     dstptr[offset++] = (uint08)t.data[1][1].Y();
     dstptr[offset++] = (uint08)t.data[1][1].Z();
     dstptr[offset++] = (uint08)t.data[1][2].W();
     dstptr[offset++] = (uint08)t.data[1][2].X();
     dstptr[offset++] = (uint08)t.data[1][2].Y();
     dstptr[offset++] = (uint08)t.data[1][2].Z();
     dstptr[offset++] = (uint08)t.data[1][3].W();
     dstptr[offset++] = (uint08)t.data[1][3].X();
     dstptr[offset++] = (uint08)t.data[1][3].Y();
     dstptr[offset]   = (uint08)t.data[1][3].Z();
   
     // row3
     offset = base_index + left_offset[2];
     dstptr[offset++] = (uint08)t.data[2][0].W();
     dstptr[offset++] = (uint08)t.data[2][0].X();
     dstptr[offset++] = (uint08)t.data[2][0].Y();
     dstptr[offset++] = (uint08)t.data[2][0].Z();
     dstptr[offset++] = (uint08)t.data[2][1].W();
     dstptr[offset++] = (uint08)t.data[2][1].X();
     dstptr[offset++] = (uint08)t.data[2][1].Y();
     dstptr[offset++] = (uint08)t.data[2][1].Z();
     dstptr[offset++] = (uint08)t.data[2][2].W();
     dstptr[offset++] = (uint08)t.data[2][2].X();
     dstptr[offset++] = (uint08)t.data[2][2].Y();
     dstptr[offset++] = (uint08)t.data[2][2].Z();
     dstptr[offset++] = (uint08)t.data[2][3].W();
     dstptr[offset++] = (uint08)t.data[2][3].X();
     dstptr[offset++] = (uint08)t.data[2][3].Y();
     dstptr[offset]   = (uint08)t.data[2][3].Z();
   
     // row4
     offset = base_index + left_offset[3];
     dstptr[offset++] = (uint08)t.data[3][0].W();
     dstptr[offset++] = (uint08)t.data[3][0].X();
     dstptr[offset++] = (uint08)t.data[3][0].Y();
     dstptr[offset++] = (uint08)t.data[3][0].Z();
     dstptr[offset++] = (uint08)t.data[3][1].W();
     dstptr[offset++] = (uint08)t.data[3][1].X();
     dstptr[offset++] = (uint08)t.data[3][1].Y();
     dstptr[offset++] = (uint08)t.data[3][1].Z();
     dstptr[offset++] = (uint08)t.data[3][2].W();
     dstptr[offset++] = (uint08)t.data[3][2].X();
     dstptr[offset++] = (uint08)t.data[3][2].Y();
     dstptr[offset++] = (uint08)t.data[3][2].Z();
     dstptr[offset++] = (uint08)t.data[3][3].W();
     dstptr[offset++] = (uint08)t.data[3][3].X();
     dstptr[offset++] = (uint08)t.data[3][3].Y();
     dstptr[offset]   = (uint08)t.data[3][3].Z();
   
     // move to next block
     block += blocksize;
    }

 return TRUE;
}

BOOL BC7ToB8G8R8A8Byte(char* dst, const char* src, uint32 dx, uint32 dy)
{
 // block properties
 const unsigned int blocksize = 16;
 const unsigned int block_dx = 4;
 const unsigned int block_dy = 4;

 // convert destination to unsigned bytes
 uint08* dstptr = reinterpret_cast<uint08*>(dst);
 if(!dstptr) return FALSE;

 // compute BC7 data size
 uint32 block_rows = std::max((DWORD)1, (DWORD)((dy + 3)/block_dy));
 uint32 block_cols = std::max((DWORD)1, (DWORD)((dx + 3)/block_dx));
 uint32 n_blocks = block_rows*block_cols;
 uint32 datasize = blocksize*n_blocks;

 // premultiplied
 uint32 pitch = 4*dx;
 uint32 left_offset[4] = { 0, pitch, 2*pitch, 3*pitch };

 // for each block
 const char* block = src;
 Tile t(4, 4);
 for(uint32 i = 0; i < n_blocks; i++)
    {
     // decompress
     AVPCL::decompress(&block[0], t);

     // now map data to RGBA image
     uint32 base_x = (i % block_cols) * block_dx;
     uint32 base_y = (i / block_cols) * block_dy;
     uint32 base_index = 4*(dx*base_y + base_x);

     // row1
     uint32 offset = base_index;
     dstptr[offset++] = (uint08)t.data[0][0].Z();
     dstptr[offset++] = (uint08)t.data[0][0].Y();
     dstptr[offset++] = (uint08)t.data[0][0].X();
     dstptr[offset++] = (uint08)t.data[0][0].W();
     dstptr[offset++] = (uint08)t.data[0][1].Z();
     dstptr[offset++] = (uint08)t.data[0][1].Y();
     dstptr[offset++] = (uint08)t.data[0][1].X();
     dstptr[offset++] = (uint08)t.data[0][1].W();
     dstptr[offset++] = (uint08)t.data[0][2].Z();
     dstptr[offset++] = (uint08)t.data[0][2].Y();
     dstptr[offset++] = (uint08)t.data[0][2].X();
     dstptr[offset++] = (uint08)t.data[0][2].W();
     dstptr[offset++] = (uint08)t.data[0][3].Z();
     dstptr[offset++] = (uint08)t.data[0][3].Y();
     dstptr[offset++] = (uint08)t.data[0][3].X();
     dstptr[offset]   = (uint08)t.data[0][3].W();
   
     // row2
     offset = base_index + left_offset[1];
     dstptr[offset++] = (uint08)t.data[1][0].Z();
     dstptr[offset++] = (uint08)t.data[1][0].Y();
     dstptr[offset++] = (uint08)t.data[1][0].X();
     dstptr[offset++] = (uint08)t.data[1][0].W();
     dstptr[offset++] = (uint08)t.data[1][1].Z();
     dstptr[offset++] = (uint08)t.data[1][1].Y();
     dstptr[offset++] = (uint08)t.data[1][1].X();
     dstptr[offset++] = (uint08)t.data[1][1].W();
     dstptr[offset++] = (uint08)t.data[1][2].Z();
     dstptr[offset++] = (uint08)t.data[1][2].Y();
     dstptr[offset++] = (uint08)t.data[1][2].X();
     dstptr[offset++] = (uint08)t.data[1][2].W();
     dstptr[offset++] = (uint08)t.data[1][3].Z();
     dstptr[offset++] = (uint08)t.data[1][3].Y();
     dstptr[offset++] = (uint08)t.data[1][3].X();
     dstptr[offset]   = (uint08)t.data[1][3].W();
   
     // row3
     offset = base_index + left_offset[2];
     dstptr[offset++] = (uint08)t.data[2][0].Z();
     dstptr[offset++] = (uint08)t.data[2][0].Y();
     dstptr[offset++] = (uint08)t.data[2][0].X();
     dstptr[offset++] = (uint08)t.data[2][0].W();
     dstptr[offset++] = (uint08)t.data[2][1].Z();
     dstptr[offset++] = (uint08)t.data[2][1].Y();
     dstptr[offset++] = (uint08)t.data[2][1].X();
     dstptr[offset++] = (uint08)t.data[2][1].W();
     dstptr[offset++] = (uint08)t.data[2][2].Z();
     dstptr[offset++] = (uint08)t.data[2][2].Y();
     dstptr[offset++] = (uint08)t.data[2][2].X();
     dstptr[offset++] = (uint08)t.data[2][2].W();
     dstptr[offset++] = (uint08)t.data[2][3].Z();
     dstptr[offset++] = (uint08)t.data[2][3].Y();
     dstptr[offset++] = (uint08)t.data[2][3].X();
     dstptr[offset]   = (uint08)t.data[2][3].W();
   
     // row4
     offset = base_index + left_offset[3];
     dstptr[offset++] = (uint08)t.data[3][0].Z();
     dstptr[offset++] = (uint08)t.data[3][0].Y();
     dstptr[offset++] = (uint08)t.data[3][0].X();
     dstptr[offset++] = (uint08)t.data[3][0].W();
     dstptr[offset++] = (uint08)t.data[3][1].Z();
     dstptr[offset++] = (uint08)t.data[3][1].Y();
     dstptr[offset++] = (uint08)t.data[3][1].X();
     dstptr[offset++] = (uint08)t.data[3][1].W();
     dstptr[offset++] = (uint08)t.data[3][2].Z();
     dstptr[offset++] = (uint08)t.data[3][2].Y();
     dstptr[offset++] = (uint08)t.data[3][2].X();
     dstptr[offset++] = (uint08)t.data[3][2].W();
     dstptr[offset++] = (uint08)t.data[3][3].Z();
     dstptr[offset++] = (uint08)t.data[3][3].Y();
     dstptr[offset++] = (uint08)t.data[3][3].X();
     dstptr[offset]   = (uint08)t.data[3][3].W();
   
     // move to next block
     block += blocksize;
    }

 return TRUE;
}

BOOL DXT1BlockUnswizzle(char* dst, const char* src, uint32 dx, uint32 dy)
{
 // block properties
 const unsigned int blocksize = 8;
 const unsigned int block_dx = 4;
 const unsigned int block_dy = 4;

 // compute DXT1 data size
 uint32 block_rows = std::max((DWORD)1, (DWORD)((dy + 3)/block_dy));
 uint32 block_cols = std::max((DWORD)1, (DWORD)((dx + 3)/block_dx));
 uint32 n_blocks = block_rows*block_cols;
 uint32 datasize = blocksize*n_blocks;

 // normally each 4x4 block is
 // 0x0 0x1 0x2 0x3
 // 0x4 0x5 0x6 0x7
 // 0x8 0x9 0xA 0xB
 // 0xC 0xD 0xE 0xF
 // but it is remapped to
 // 0x0 0x1 0x4 0x5
 // 0x2 0x3 0x6 0x7
 // 0x8 0x9 0xC 0xD
 // 0xA 0xB 0xE 0xF
 // and we must remap back

 // 2 colors x and y followed by 16 3-bit indices
 //                                      byte 4     5         6       7
 // xxxxxxxx-xxxxxxxx-yyyyyyyy-yyyyyyyy-00114455-22336677-8899CCDD-AABBEEFF (src)
 // xxxxxxxx-xxxxxxxx-yyyyyyyy-yyyyyyyy-00112233-44556677-8899AABB-CCDDEEFF

 // for each block
 char* dstptr = &dst[0];
 const char* srcptr = &src[0];
 for(uint32 i = 0; i < n_blocks; i++)
    {
     // bytes 0-3
     dstptr[0] = srcptr[0];
     dstptr[1] = srcptr[1];
     dstptr[2] = srcptr[2];
     dstptr[3] = srcptr[3];

     // byte 4
     dstptr[4] = ((srcptr[4] & 0xF0)) |     // keep 00110000 from srcptr[4]
                 ((srcptr[5] & 0xF0) >> 4); // keep 22330000 from srcptr[5]

     // byte 5
     dstptr[5] = ((srcptr[5] & 0x0F)) |     // keep 00006677 from srcptr[5]
                 ((srcptr[4] & 0x0F) << 4); // keep 00004455 from srcptr[4]

     // byte 6
     dstptr[6] = ((srcptr[6] & 0xF0)) |     // keep 88990000 from srcptr[6]
                 ((srcptr[7] & 0xF0) >> 4); // keep AABB0000 from srcptr[7]

     // byte 7
     dstptr[7] = ((srcptr[7] & 0x0F)) |     // keep 0000EEFF from srcptr[7]
                 ((srcptr[6] & 0x0F) << 4); // keep 0000CCDD from srcptr[6]

     // increment offset by blocksize
     dstptr += blocksize;
     srcptr += blocksize;
    }

 return true;
}
