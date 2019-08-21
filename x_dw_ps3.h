#ifndef __XENTAX_DW_PS3_H
#define __XENTAX_DW_PS3_H

#define X_SYSTEM PS3
#define X_GAME   DYNASTY_WARRIORS

namespace X_SYSTEM { namespace X_GAME {

// unpacking utility functions
bool DWGetFileExtension(LPCTSTR filename, LPTSTR extension);
bool DWRenameIDXOUT(LPCTSTR filename);

bool DWUnpackIDXZRC_TypeA(LPCTSTR filename);
bool DWUnpackIDXZRC_TypeB(LPCTSTR filename);

bool UnpackANS(LPCTSTR filename);
bool UnpackABC(LPCTSTR filename);
bool UnpackABC_Type2(LPCTSTR filename);
bool UnpackIDX(LPCTSTR filename);

// archive file unpacking functions
bool DWUnpackArc1(LPCTSTR filename);
bool DWUnpackArc2(LPCTSTR filename);
bool DWUnpackArc3(LPCTSTR filename);
bool DWUnpackArc4(LPCTSTR filename);
bool DWUnpackArc5(LPCTSTR filename);
bool DWUnpackArc6(LPCTSTR filename);
bool DWUnpackMDLK(LPCTSTR filename);

// DLC FUNCTIONS
bool UnpackDAT_TypeA(LPCTSTR filename);
bool UnpackDAT_TypeB(LPCTSTR filename);
bool UnpackDAT(LPCTSTR filename);
bool UnpackDLC_TypeA(LPCTSTR pathname);
bool UnpackDLC_TypeB(LPCTSTR pathname);
bool UnpackDLC_TypeC(LPCTSTR pathname);
bool UnpackDLC_TypeD(LPCTSTR pathname);
bool UnpackDLC(LPCTSTR pathname);

// UNPACK LOOPING
bool UnpackIDXOUT(LPCTSTR filename);
bool UnpackIDXZRC(LPCTSTR pathname);
bool IDXArchiveUnpackLoop(LPCTSTR pathname);

// TEXTURE AND MODEL LOOPS
bool UnpackTextures(LPCTSTR pathname);
bool UnpackModels(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
