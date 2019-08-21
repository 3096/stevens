#ifndef _XENTAX_CAPCOM_H
#define _XENTAX_CAPCOM_H

namespace CAPCOM {

struct BONEINFO {
 uint16 j01;    // jntmap index (uint08 in-file)
 uint16 j02;    // parent index (uint08 in-file)
 uint16 j03;    // index of mirrored joint (FF if none) (uint08 in-file)
 uint16 j04;    // 0x00 (uint08 in-file)
 real32 j05[5]; // unknown floating-point values
};

struct BONEMATRIX {
 real32 m[16];
};

struct BONEMAP {
 uint32 elem;
 boost::shared_array<uint16> data;
};

struct BONEDATA {
 std::deque<BONEINFO> boneinfo;
 std::deque<BONEMATRIX> m1;
 std::deque<BONEMATRIX> m2;
 BONEMAP bonemap;
 std::deque<BONEMAP> bonemaplist;
};

struct MESHDATA {
 endian_mode mode;
 boost::shared_array<char> vbdata;
 boost::shared_array<char> ibdata;
 uint32 vbsize; // size of buffer in bytes
 uint32 ibsize; // size of buffer in bytes
};

struct MESHINFO {
 uint16 index;
 uint16 type;
 uint16 vertices;
 uint16 mtl_id;
 uint16 lod;
 uint16 vtx_stride;
 uint32 vtx_format;
 uint32 vtx_start1;
 uint32 vtx_start2;
 uint32 vtx_offset;
 uint32 base_index;
 uint32 indices;
 uint16 wmap_index;
 uint16 jmap_index;
 uint32 min_index;
 uint32 max_index;
};

bool CreateSkeleton(AMC_SKELETON2& skeleton, const BONEDATA& bd);
bool CreateMeshData(AMC_VTXBUFFER& vb, AMC_IDXBUFFER& ib, AMC_SURFACE& surface, const BONEDATA& bd, const MESHDATA& meshdata, const MESHINFO& meshinfo);

}

namespace CAPCOM { namespace PS3 {

bool ExtractARC(LPCTSTR filename, LPCTSTR datapath, bool delete_file = false);
bool ExtractARCFromPath(LPCTSTR pathname, bool delete_file = false);

bool ExtractTEX(LPCTSTR filename, bool delete_file = false);
bool ExtractTEXFromPath(LPCTSTR pathname, bool delete_file = false);

}}

namespace CAPCOM { namespace PC {

bool ExtractARC(LPCTSTR filename, LPCTSTR datapath, bool delete_file = false);
bool ExtractARCFromPath(LPCTSTR pathname, bool delete_file = false);

bool ExtractTEX(LPCTSTR filename, bool delete_file = false);
bool ExtractTEXFromPath(LPCTSTR pathname, bool delete_file = false);

}}

#endif
