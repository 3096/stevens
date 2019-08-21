#ifndef __XENTAX_NAMCO_H
#define __XENTAX_NAMCO_H

#include "x_stream.h"

namespace NAMCO {

#define NAMCO_INVALID     0xFF
#define NAMCO_UNKNOWN     0x00
#define NAMCO_DIFFUSE     0x01
#define NAMCO_NORMAL      0x02
#define NAMCO_SPECULAR    0x03
#define NAMCO_LIGHT       0x04
#define NAMCO_ENVIRONMENT 0x05
#define NAMCO_SHADING     0x06
#define NAMCO_COLORMASK   0x07

struct MATERIALINFO {
 uint32 mtrl_id;
 std::pair<uint32, uint32> list[8];
};
typedef bool (*MATERIALFUNCTION)(binary_stream&, MATERIALINFO&);

// archives
bool ExtractPAC(LPCTSTR filename);
bool ExtractFHM(LPCTSTR filename);

// textures
bool ExtractNTP3(LPCTSTR filename, bool do_delete = false, bool rbo = false);
bool PathExtractNTP3(bool do_delete = false, uint32 start = 0, bool rbo = false);
bool PathExtractNTP3(LPCTSTR pathname, bool do_delete = false, uint32 start = 0, bool rbo = false);

// models
bool ExtractNDP3(LPCTSTR filename, MATERIALFUNCTION mf = nullptr, bool do_delete = false);
bool PathExtractNDP3(MATERIALFUNCTION mf = nullptr, bool do_delete = false, uint32 start = 0, LPCTSTR ext = TEXT("NDP3"));
bool PathExtractNDP3(LPCTSTR pathname, MATERIALFUNCTION mf = nullptr, bool do_delete = false, uint32 start = 0, LPCTSTR ext = TEXT("NDP3"));

// skeletons (BIG ENDIAN)
bool ExtractVBN(LPCTSTR filename, bool do_delete = false);
bool PathExtractVBN(bool do_delete = false, uint32 start = 0);
bool PathExtractVBN(LPCTSTR pathname, bool do_delete = false, uint32 start = 0);

// skeletons (LITTLE ENDIAN)
bool ExtractVBN_LE(LPCTSTR filename, bool do_delete = false);
bool PathExtractVBN_LE(bool do_delete = false, uint32 start = 0);
bool PathExtractVBN_LE(LPCTSTR pathname, bool do_delete = false, uint32 start = 0);

// skeletons (NMD FORMAT)
bool ExtractBonesNMD(LPCTSTR filename, bool do_delete = false);
bool PathExtractBonesNMD(bool do_delete = false, uint32 start = 0);
bool PathExtractBonesNMD(LPCTSTR pathname, bool do_delete = false, uint32 start = 0);

};

#endif
