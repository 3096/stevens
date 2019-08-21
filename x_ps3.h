#ifndef __XENTAX_PS3_H
#define __XENTAX_PS3_H

bool ps3_edge_lzma_decompress(const uint08* prp, uint32 prpsize, const uint08* src, uint32 srcsize, uint08* dst, uint32 dstsize);

struct PS3EDGEINDEXCOMPDATA {
 uint16 n_indices; // number of indices in index table
 uint16 base;      // base value
 uint16 n_onebits; // number of bytes in one-bit stream
 uint08 bpi;       // bits per index
 uint08 pad;       // always zero
 boost::shared_array<char> onebitdata;
 boost::shared_array<char> twobitdata;
 boost::shared_array<char> indextable;
};
bool ps3_edge_index_compress(const uint16* data, uint32 size, PS3EDGEINDEXCOMPDATA& eicd);

#endif
