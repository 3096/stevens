#ifndef __XENTAX_SMC_H
#define __XENTAX_SMC_H

//
// HEADER
//

struct SMCHEADER {
 uint32 magic;      // magic number
 uint16 major;      // major version
 uint16 minor;      // minor version
 uint32 n_vlayouts; // number of vertex layouts
 uint32 n_vbuffers; // number of vertex buffers
 uint32 n_ibuffers; // number of index buffers
 uint32 n_textures; // number of textures
 uint32 n_surfaces; // number of mesh objects
 uint32 n_material; // number of mesh materials
 uint32 n_skeleton; // number of skeletons
 uint32 n_jointmap; // number of joint maps
 uint32 n_animated; // number of animations
 uint32 reserved1;  // reserved
 uint32 reserved2;  // reserved
 uint32 reserved3;  // reserved
 uint32 reserved4;  // reserved
 uint32 reserved5;  // reserved
};

//
// TEXTURES
//

#define SMC_TEXTURE_IMAGE    0
#define SMC_TEXTURE_SEQUENCE 1
#define SMC_TEXTURE_MOVIE    2

struct SMCTEXTURE {
 uint32 id;
 boost::shared_array<char> name;
 uint08 type;
 uint08 reserved[3];
};

struct SMCFILETEXTURE : public SMCTEXTURE {
 boost::shared_array<char> filename;
};

struct SMCSEQUENCETEXTURE : public SMCTEXTURE {
 struct SEQUENCEITEM {
  boost::shared_array<char> filename;
  real32 duration;
 };
 boost::shared_array<char> filename;
 std::deque<SEQUENCEITEM> data;
 uint08 loop;
 uint08 reserved[3];
};

struct SMCMOVIETEXTURE : public SMCTEXTURE {
 boost::shared_array<char> filename;
 real32 duration;
 uint08 loop;
 uint08 reserved[3];
};


/*

//
// VERTEX DATA
//

#define VERTEX_POSITION 0x01
#define VERTEX_NORMAL   0x02
#define VERTEX_UV       0x04
#define VERTEX_WEIGHTS  0x08
#define VERTEX_BONEIDX  0x10
#define VERTEX_WMAPIDX  0x20
#define VERTEX_SKELETON 0x40

#define INVALID_VERTEX_BONE_INDEX 0xFFFF
#define INVALID_VERTEX_WMAP_INDEX 0xFFFF
#define INVALID_VERTEX_SKEL_INDEX 0xFFFF

struct SMC_VERTEX {
 real32 vx, vy, vz;
 real32 nx, ny, nz;
 real32 tu, tv;
 real32 wv[8];
 union { uint16 bi[8]; uint16 mi[8]; };
 uint16 si[8];
};

struct SMC_VTX_BUFFER {
 unsigned char flags;
 std::string name;
 uint32 elem;
 boost::shared_array<SMC_VERTEX> data;
};

//
// RENDER DATA
//

#define FACE_FORMAT_UINT_08   0x1
#define FACE_FORMAT_UINT_16   0x2
#define FACE_FORMAT_UINT_32   0x4

#define FACE_TYPE_TRIANGLES   0x1
#define FACE_TYPE_TRISTRIP    0x2
#define FACE_TYPE_TRISTRIPCUT 0x4

struct SMC_IDX_BUFFER {
 unsigned char format;
 unsigned char type;
 std::string name;
 uint32 elem;
 boost::shared_array<char> data;
 uint32 tris;
 uint16 material;
};

//
// REFERENCE DATA
//

struct SMC_REF_BUFFER {
 uint32 vb_index; // vbuffer index
 uint32 ib_index; // ibuffer index
 uint32 vb_start; // position of vbuffer
 uint32 ib_start; // position of ibuffer
 uint32 vb_width; // number of vertices
 uint32 ib_width; // number of indices
};

//
// MATERIAL DATA
//

#define INVALID_MATERIAL 0xFFFF
#define INVALID_TEXTURE_INDEX 0xFFFF

// struct SMC_MATERIAL {
//  std::string id; // identifier
//  uint08 twoside; // double-sided
//  uint16 basemap; // reference to texture id
//  uint16 specmap; // reference to texture id
//  uint16 tranmap; // reference to texture id
//  uint16 bumpmap; // reference to texture id
//  uint16 normmap; // reference to texture id
//  uint16 resmap1; // reserved1
//  uint16 resmap2; // reserved2
//  uint16 resmap3; // reserved3
//  uint16 resmap4; // reserved4
// };

//
// SKELETON DATA
//

//
// MODEL DATA
//

struct SIMPLEMODELCONTAINER {
 std::deque<boost::shared_ptr<SMCTEXTURE>> textures;

//  SMC_VTX_BUFFER vbuffer;
//  std::deque<SMC_IDX_BUFFER> ibuffer;
//  std::deque<SMC_TEXTURE> textures;
//  std::deque<SMC_MATERIAL> materials;
//  std::deque<SMC_SKELETON> skeletons;
};

*/

#endif
