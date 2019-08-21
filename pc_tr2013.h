#ifndef __XENTAX_PC_TR2013_H
#define __XENTAX_PC_TR2013_H

#define X_SYSTEM PC
#define X_GAME   TR2013

namespace X_SYSTEM { namespace X_GAME {

bool extract(void);
bool extract(const char* pathname);

}};

// modding tools
namespace X_SYSTEM { namespace X_GAME {

bool DDSToPCD(const char* dname, const char* pname);
bool PCDToDRM(const char* dname, const char* pname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
