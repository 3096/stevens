#ifndef __XENTAX_DW_XB360_H
#define __XENTAX_DW_XB360_H

#define X_SYSTEM XB360
#define X_GAME   DYNASTY_WARRIORS

namespace X_SYSTEM { namespace X_GAME {

// unpacking utility functions
bool DWGetFileExtension(LPCTSTR filename, LPTSTR extension);
bool DWRenameIDXOUT(LPCTSTR filename);

// IDXZRC unpacking
bool DWUnpackIDXZRC_TypeA(LPCTSTR filename);
bool DWUnpackIDXZRC_TypeB(LPCTSTR filename);
bool DWUnpackLoopIDXZRC_TypeA(LPCTSTR pathname);
bool DWUnpackLoopIDXZRC_TypeB(LPCTSTR pathname);

// archive file unpacking functions
bool DWUnpackArc1(LPCTSTR filename);
bool DWUnpackArc2(LPCTSTR filename);
bool DWUnpackArc3(LPCTSTR filename);
bool DWUnpackArc4(LPCTSTR filename);
bool DWUnpackArc5(LPCTSTR filename);
bool DWUnpackArc6(LPCTSTR filename);
bool DWUnpackMDLK(LPCTSTR filename);

// unpacking functions
bool UnpackANS(LPCTSTR filename);
bool UnpackABC(LPCTSTR filename);
bool UnpackABC_Type2(LPCTSTR filename);
bool UnpackIDX(LPCTSTR filename);
bool UnpackIDXOUT(LPCTSTR pathname);
bool UnpackIDXZRC(LPCTSTR pathname);
bool IDXArchiveUnpackLoop(LPCTSTR pathname);
//bool UnpackBIN(LPCTSTR filename);

// DLC unpacking
bool UnpackDLC_TYPE_01(LPCTSTR filename);

// models and textures
bool UnpackTextures(LPCTSTR pathname, bool rbo = true);
bool UnpackModels(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
