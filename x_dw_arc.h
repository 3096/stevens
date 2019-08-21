#ifndef __X_DW_ARCHIVES_H
#define __X_DW_ARCHIVES_H

namespace DYNASTY_WARRIORS {

// decompression
bool IDXZRC_PathExtract(LPCTSTR pathname, endian_mode mode);
bool IDXZRC_FileExtract(LPCTSTR filename, endian_mode mode);
bool IDXZIP_PathExtract(LPCTSTR pathname, endian_mode mode);
bool IDXZIP_FileExtract(LPCTSTR filename, endian_mode mode);
bool IDXLPZ_PathExtract(LPCTSTR pathname, endian_mode mode);
bool IDXLPZ_FileExtract(LPCTSTR filename, endian_mode mode);

// archive unpacking (IDX + BIN)
bool UnpackIDX_A(LPCTSTR filename, endian_mode mode, LPCTSTR binprefix = nullptr);
bool UnpackIDX_B(LPCTSTR filename, endian_mode mode);

// archive unpacking (BIN)
bool UnpackBIN_A(LPCTSTR filename, endian_mode mode);
bool UnpackBIN_B(LPCTSTR filename, endian_mode mode);
bool UnpackBIN_C(LPCTSTR filename);

// recursive unpacking
bool IDXOUT_PathExtract(LPCTSTR pathname, endian_mode mode);
bool IDXOUT_FileExtract(LPCTSTR filename, endian_mode mode);

// recursive archive unpacking
bool ARC0_FileExtract(LPCTSTR filename, endian_mode mode);
bool ARC1_FileExtract(LPCTSTR filename, endian_mode mode);
bool ARC2_FileExtract(LPCTSTR filename, endian_mode mode);
bool ARC3_FileExtract(LPCTSTR filename, endian_mode mode);
bool ARC4_FileExtract(LPCTSTR filename, endian_mode mode);
bool ARC5_FileExtract(LPCTSTR filename, endian_mode mode);
bool ARC6_FileExtract(LPCTSTR filename, endian_mode mode);
bool ARC7_FileExtract(LPCTSTR filename, endian_mode mode);
bool MDLK_FileExtract(LPCTSTR filename, endian_mode mode);
bool CSTB_FileExtract(LPCTSTR filename, endian_mode mode);

// remove me
bool IsCompressedType1(LPCTSTR filename, endian_mode mode);
bool IsCompressedType2(LPCTSTR filename, endian_mode mode);
// remove me

}

// NOTES:
// Near MDLK files, is this model placement file?
// 30 00 00 00 50 22 00 00 00 00 00 00 30 5A 00 00
// 30 F1 00 00 C0 26 01 00 70 36 01 00 00 00 00 00
// 30 51 01 00 00 00 00 00 00 00 00 00 00 00 00 00
// data

#endif
