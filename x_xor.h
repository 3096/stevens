#ifndef __XENTAX_XOR_H
#define __XENTAX_XOR_H

struct XORFINDINFO {
 unsigned char key[256];
 uint32 keylength;
};

bool xorencrypt(const char* srcfile, const char* dstfile, const char* xorkey);
bool xorfind(const char* srcfile, const char* dstfile, XORFINDINFO* info);

#endif
