#ifndef _XENTAX_COMPILE_HEART_H
#define _XENTAX_COMPILE_HEART_H

/*

// texture files
bool CompileHeartProcessTIDFiles(const std::string& pathname);
bool CompileHeartConvertTID(const std::string& filename);

// model files
bool CompileHeartProcessISM2Files(const std::string& pathname);
bool CompileHeartConvertISM2(const std::string& filename);

struct NODEHEADER_0x05 {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of children
 uint32 p04; // string table index
 uint32 p05; // string table index
 uint32 p06; // string table index
 uint32 p07; // ???
 uint32 p08; // parent section 0x05 offset
 uint32 p09; // number of children
 uint32 p10; // ???
 uint32 p11; // ???
 uint32 p12; // bone index
 uint32 p13; // ???
 uint32 p14; // index into section 0x03 offset array
 uint32 p15; // ???
 uint32 p16; // ???
};

struct NODEHEADER_0x0A {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of children
 uint32 p04; // string table index
 uint32 p05; // string table index
 uint32 p06; // ???
 uint32 p07; // ???
 uint32 p08; // ???
};

struct NODEHEADER_0x0B {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of children
};

struct NODEHEADER_0x0D {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of children
 uint32 p04; // string table index
 uint32 p05; // string table index
 uint32 p06; // string table index
 uint32 p07; // string table index
};

struct NODEHEADER_0x0F {
 uint32 p01; // signature
};

struct NODEDATA_0x21 {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of strings
 boost::shared_array<std::string> p04; // string table
};

struct NODEHEADER_0x2D {
 uint32 p01; // signature
};

struct NODEHEADER_0x2E {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of children
};

struct NODEHEADER_0x30 {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of children
 uint32 p04; // string table index
 uint32 p05; // string table index
 real32 p06[16]; // matrix
};

struct NODEHEADER_0x31 {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of children
 uint32 p04; // string table index
 uint32 p05; // string table index
};

struct NODEHEADER_0x32 {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of children
 uint32 p04; // 0x00 usually, string table index???
 uint32 p05; // 0x00 usually, string table index???
};

struct NODEHEADER_0x34 {
 uint32 p01; // signature
};

struct NODEHEADER_0x44 {
 uint32 p01; // signature
 uint32 p02; // size of signature + header
 uint32 p03; // number of items
 uint32 p04; // 0x00
 uint32 p05; // type of data 5 (shorts?) or 18 (floats?), 12 (matrix?)
 uint32 p06; // group size (typically 1, 5, 13, or 16)
 uint32 p07; // 0x00
 uint32 p08; // 0x00
};

*/

#endif
