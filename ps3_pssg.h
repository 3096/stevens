#ifndef __XENTAX_PSSG_H
#define __XENTAX_PSSG_H

#define PS3_PSSG_BEGIN namespace PS3 { namespace PSSG {
#define PS3_PSSG_END }};

PS3_PSSG_BEGIN

bool extract(void);
bool extract(const char* pathname);

PS3_PSSG_END

#endif
