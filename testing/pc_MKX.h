#ifndef __XENTAX_TESTING_PC_MKX_H
#define __XENTAX_TESTING_PC_MKX_H

#define MK_RGBA 0x00
#define MK_ARGB 0x01
#define MK_DXT1 0x02
#define MK_DXT3 0x03
#define MK_DXT5 0x04
#define MK_ATI2 0x05
#define MK_BC7  0x06

#define MK_REVERSE_BYTE_ORDER_NONE 0x00
#define MK_REVERSE_BYTE_ORDER_2 0x01
#define MK_REVERSE_BYTE_ORDER_4 0x02
#define MK_REVERSE_BYTE_ORDER_8 0x03

struct MKTEXTURE2D {
 STDSTRING filename;
 uint32 offset; // offset to data
 uint32 dx;
 uint32 dy;
 uint32 format;
 uint32 rbo;
};

bool MKX_LoadHashtable(LPCTSTR filename, std::map<uint64, std::string>& hashtable);
bool MKX_ProcessStaticMesh(LPCTSTR filename, const uint64* info);
bool MKX_ProcessSkeletalMesh(LPCTSTR filename, const uint64* offsets);
bool MKX_ProcessSkeleton(LPCTSTR filename, const uint64* offsets);
bool MKX_ProcessTexture2D(const MKTEXTURE2D& info);

bool MKX_ProcessTexture2DList(LPCTSTR pathname, const std::map<uint64, std::string>& hashtable);
bool MKX_ProcessTexture2D(LPCTSTR filename, const std::map<uint64, std::string>& hashtable);

bool MKX_ProcessSkeletonList(LPCTSTR pathname, const std::map<uint64, std::string>& hashtable);
bool MKX_ProcessSkeleton(LPCTSTR filename, const std::map<uint64, std::string>& hashtable);

bool MKX_ProcessSkeletalMeshList(LPCTSTR pathname, const std::map<uint64, std::string>& hashtable);
bool MKX_ProcessSkeletalMesh(LPCTSTR filename, const std::map<uint64, std::string>& hashtable);

bool MKX_ProcessStaticMeshList(LPCTSTR pathname, const std::map<uint64, std::string>& hashtable);
bool MKX_ProcessStaticMesh(LPCTSTR filename, const std::map<uint64, std::string>& hashtable);

#endif
