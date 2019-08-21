#ifndef __XENTAX_DSP_H
#define __XENTAX_DSP_H

#define DSP_INVALID_IDENTIFIER 0xFFFFFFFFul
#define DSP_INVALID_OFFSET 0xFFFFFFFFFFFFFFFFull

// ENDIAN MACROS
#define DSP_UNSPECIFIED_ENDIAN 0
#define DSP_LITTLE_ENDIAN 1
#define DSP_BIG_ENDIAN    2

// ENDIAN REVERSAL MACROS
#define DSP_REVERSE_NONE  0
#define DSP_REVERSE_2     1
#define DSP_REVERSE_4     2
#define DSP_REVERSE_8     3

// FORMAT MACROS
#define DSP_FORMAT_UNKNOWN    0x00
#define DSP_FORMAT_CUSTOM     0x01
#define DSP_FORMAT_BIASED     0x02
#define DSP_FORMAT_SCALED     0x03
#define DSP_FORMAT_REAL16     0x04
#define DSP_FORMAT_REAL32     0x05
#define DSP_FORMAT_REAL64     0x06
#define DSP_FORMAT_SINT08     0x07
#define DSP_FORMAT_UINT08     0x08
#define DSP_FORMAT_SINT16     0x09
#define DSP_FORMAT_UINT16     0x0A
#define DSP_FORMAT_SINT32     0x0B
#define DSP_FORMAT_UINT32     0x0C
#define DSP_FORMAT_SINT64     0x0D
#define DSP_FORMAT_UINT64     0x0E
#define DSP_FORMAT_VECTOR2D16 0x0F
#define DSP_FORMAT_VECTOR3D16 0x10
#define DSP_FORMAT_VECTOR4D16 0x11
#define DSP_FORMAT_VECTOR2D32 0x12
#define DSP_FORMAT_VECTOR3D32 0x13
#define DSP_FORMAT_VECTOR4D32 0x14

// BLENDWEIGHT MACROS
// NORMAL is (W1 W2 W3 W4) -> (I1 I2 I3 I4)
// COMPUTE FIRST is (W2 W3 W4) -> (I1 I2 I3 I4) where W1 = 1.0 - (W2 + W3 + W4)
// COMPUTE FINAL is (W1 W2 W3) -> (I1 I2 I3 I4) where W4 = 1.0 - (W1 + W2 + W3)
#define DSP_BLENDWEIGHT_UNKNOWN       0x00
#define DSP_BLENDWEIGHT_NORMAL        0x01
#define DSP_BLENDWEIGHT_COMPUTE_FIRST 0x02
#define DSP_BLENDWEIGHT_COMPUTE_FINAL 0x03

// BLENDINDEX MACROS
#define DSP_BLENDINDEX_UNKNOWN    0x00
#define DSP_BLENDINDEX_BONE_INDEX 0x01
#define DSP_BLENDINDEX_BONEMAP    0x02

// TOPOLOGY MACROS
#define DSP_UNKNOWN_TOPOLOGY  0x00
#define DSP_LINE_LIST         0x01
#define DSP_TRIANGLE_LIST     0x02
#define DSP_TRIANGLE_STRIP    0x04
#define DSP_TRIANGLE_STRIPCUT 0x08

// TEXTURE MACROS
#define DSP_TEXTURE_UNKNOWN 0
#define DSP_TEXTURE_RGBA 1
#define DSP_TEXTURE_BGRA 2
#define DSP_TEXTURE_ARGB 3
#define DSP_TEXTURE_ABGR 4
#define DSP_TEXTURE_DXT1 5
#define DSP_TEXTURE_DXT3 6
#define DSP_TEXTURE_DXT5 7

// TEXTURE CUBEMAP FLAGS
#define DSP_CUBEMAP_NONE  0x00
#define DSP_CUBEMAP_POS_X 0x01
#define DSP_CUBEMAP_POS_Y 0x02
#define DSP_CUBEMAP_POS_Z 0x04
#define DSP_CUBEMAP_NEG_X 0x08
#define DSP_CUBEMAP_NEG_Y 0x10
#define DSP_CUBEMAP_NEG_Z 0x20
#define DSP_CUBEMAP_ALL   0x3F

// TEXTURE SWIZZLE FLAGS
#define DSP_SWIZZLE_NONE   0x0000
#define DSP_SWIZZLE_MORTON 0x0001
#define DSP_SWIZZLE_XBTILE 0x0002

struct DSPGENERAL {
 STDSTRING filename;
 STDSTRING savepath;
 uint32 endian;
};

struct DSPVTXBUFFER {
 STDSTRING filename; // file where buffer exists (optional)
 uint32 identifier;  // unique identifier for buffer
 STDSTRING name;     // unique name for buffer (optional)
 uint64 offset;      // offset to buffer (optional)
 uint32 size;        // size of buffer
};

struct DSPIDXBUFFER {
 STDSTRING filename; // file where buffer exists (optional)
 uint32 identifier;  // unique identifier for buffer
 STDSTRING name;     // unique name for buffer (optional)
 uint64 offset;      // offset to buffer (optional)
 uint32 size;        // size of buffer
};

struct DSPINPUTLAYOUT {
 // general
 uint32 identifier;          // unique identifier for layout
 STDSTRING name;             // unique name for layout (optional)
 // positions
 uint16 positions;           // number of positions
 uint32 position_offset[16]; // position offsets
 uint32 position_format[16]; // position formats
 uint16 position_endian[16]; // position endians
 uint32 position_slot[16];   // position slots
 real32 position_bias[16];   // position fixed-point biases
 real32 position_scale[16];  // position fixed-point scaling
 // normals
 uint16 normals;             // number of normals
 uint32 normal_offset[16];   // normal offsets
 uint32 normal_format[16];   // normal formats
 uint16 normal_endian[16];   // normal endians
 uint32 normal_slot[16];     // normal slots
 real32 normal_bias[16];     // normal fixed-point biases
 real32 normal_scale[16];    // normal fixed-point scaling
 // binormals
 uint16 binormals;           // number of binormals
 uint32 binormal_offset[16]; // binormal offsets
 uint32 binormal_format[16]; // binormal formats
 uint16 binormal_endian[16]; // binormal endians
 uint32 binormal_slot[16];   // binormal slots
 real32 binormal_bias[16];   // binormal fixed-point biases
 real32 binormal_scale[16];  // binormal fixed-point scaling
 // tangents
 uint16 tangents;            // number of tangents
 uint32 tangent_offset[16];  // tangent offsets
 uint32 tangent_format[16];  // tangent formats
 uint16 tangent_endian[16];  // tangent endians
 uint32 tangent_slot[16];    // tangent slots
 real32 tangent_bias[16];    // tangent fixed-point biases
 real32 tangent_scale[16];   // tangent fixed-point scaling
 // texture coordinates
 uint16 uvcoords;            // number of uv coordinates
 uint32 uv_offset[16];       // UV offsets
 uint32 uv_format[16];       // UV formats
 uint16 uv_endian[16];       // UV endians
 uint32 uv_slot[16];         // UV slots
 real32 uv_bias[16];         // UV fixed-point biases
 real32 uv_scale[16];        // UV fixed-point scaling
 // colors
 uint16 colors;              // number of color channels 
 uint32 color_offset[16];    // color offsets
 uint32 color_format[16];    // color formats
 uint16 color_endian[16];    // color endians
 uint32 color_slot[16];      // color slots
 real32 color_bias[16];      // color fixed-point biases
 real32 color_scale[16];     // color fixed-point scaling
 // blend weights
 uint16 weights;               // number of blend weights
 uint32 blendweight_type;      // blend weight type
 uint32 blendweight_offset[8]; // blend weight offsets
 uint32 blendweight_format[8]; // blend weight formats
 uint16 blendweight_endian[8]; // blend weight endians
 uint32 blendweight_slot[8];   // blend weight slots
 real32 blendweight_bias[8];   // blend weight biases
 real32 blendweight_scale[8];  // blend weight scaling
 // blend indices
 uint16 indices;              // number of blend indices
 uint32 blendindex_type;      // blend index type
 uint32 blendindex_offset[8]; // blend index offsets
 uint32 blendindex_format[8]; // blend index formats
 uint16 blendindex_endian[8]; // blend index endians
 uint32 blendindex_slot[8];   // blend index slots
};

struct DSPINPUTASSEMBLY {
 // IASetInputLayout
 uint32 input_layout_reference;
 // IASetVertexBuffers
 uint32 vertex_buffers;
 uint32 vertex_buffer_start_slot;
 uint32 vertex_buffer_references[16];
 uint32 vertex_buffer_strides[16];
 uint32 vertex_buffer_offsets[16];
 // IASetIndexBuffer
 uint32 index_buffer_reference;
 uint32 index_buffer_format;
 uint32 index_buffer_offset;
 // IASetPrimitiveTopology
 uint32 topology;
 // DrawIndexed
 uint32 draw_indexed_count;
 uint32 draw_indexed_start;
 uint32 draw_indexed_base_vertex;
 // VSSetConstantBuffers
 uint32 jointmap_reference;
};

struct DSPTEXTUREINFO {
 STDSTRING filename;
 STDSTRING savepath;
 uint32 reversal;
 uint32 identifier;
 STDSTRING name;
 uint64 offset;
 uint32 dx;
 uint32 dy;
 uint32 size;
 uint32 type;
 uint32 mipmaps;
 uint32 cubemap;
 uint32 swizzle;
};

struct DESPERADOINFO {
 DSPGENERAL general;
 std::deque<DSPVTXBUFFER> vblist;
 std::deque<DSPIDXBUFFER> iblist;
 std::deque<DSPINPUTLAYOUT> illist;
 std::deque<DSPINPUTASSEMBLY> ialist;
 std::deque<DSPTEXTUREINFO> textures;
};

void clear(DSPGENERAL& info);
void clear(DSPVTXBUFFER& info);
void clear(DSPIDXBUFFER& info);
void clear(DSPINPUTLAYOUT& info);
void clear(DSPINPUTASSEMBLY& info);
void clear(DSPTEXTUREINFO& info);
bool desperado(const DESPERADOINFO& dspi);

/*

struct DESPERADOINFO {
 DSPGENERAL general;
 std::deque<DSPVTXBUFFER> vblist;
 std::deque<DSPIDXBUFFER> iblist;
 std::deque<DSPINPLAYOUT> illist;
 std::deque<DSPINPUTASSEMBLY> assembly;
};

bool SaveOBJ(const DESPERADOINFO& dspi);
bool TestDesperado(void);

#define DSP_JOINTS_CUSTOM 0x0001
#define DSP_JOINTS_UINT08 0x01
#define DSP_JOINTS_UINT16 0x02
#define DSP_JOINTS_UINT32 0x03

#define DSP_TEXTURE_CUSTOM  0x0001
#define DSP_TEXTURE_BIASED  0x0002
#define DSP_TEXTURE_SCALED  0x0004
#define DSP_TEXTURE_UINVERT 0x0008
#define DSP_TEXTURE_VINVERT 0x0010
#define DSP_TEXTURE_REAL16  0x01
#define DSP_TEXTURE_REAL32  0x02
#define DSP_TEXTURE_REAL64  0x03
#define DSP_TEXTURE_SINT08  0x04
#define DSP_TEXTURE_UINT08  0x05
#define DSP_TEXTURE_SINT16  0x06
#define DSP_TEXTURE_UINT16  0x07
#define DSP_TEXTURE_SINT32  0x08
#define DSP_TEXTURE_UINT32  0x09
#define DSP_TEXTURE_SINT64  0x0A
#define DSP_TEXTURE_UINT64  0x0B

#define DSP_INVALID_OFFSET 0xFFFF

struct DSPPOSITION {
 real32 x;
 real32 y;
 real32 z;
 real32 w;
};
typedef DSPPOSITION (*DSPCUSTOMPOSITION)(binary_stream& bs);

struct DSPNORMAL {
 real32 x;
 real32 y;
 real32 z;
 real32 w;
};
typedef DSPNORMAL (*DSPCUSTOMNORMAL)(binary_stream& bs);

struct DSPWEIGHTS {
 real32 wv[8];
};
typedef DSPWEIGHTS (*DSPCUSTOMWEIGHTS)(binary_stream& bs);

struct DSPJOINTS {
 uint32 ji[8];
};
typedef DSPJOINTS (*DSPCUSTOMJOINTS)(binary_stream& bs);

struct DSPTEXTURE {
 real32 u;
 real32 v;
};
typedef DSPTEXTURE (*DSPCUSTOMTEXTURE)(binary_stream& bs);

struct DSPFILEINFO {
 char filename[MAX_PATH];
 uint16 endianess;
 char name[MAX_PATH];
};

struct DSPVTXBUFFER {
 // general information
 char name[1024];
 uint64 offset;
 uint32 size;
 uint32 vertexsize;
 uint32 vertices;
 // position information
 uint16 position_vertex_offset;
 uint16 position_flags;
 uint16 position_stride;
 real32 position_bias;
 real32 position_scale;
 DSPCUSTOMPOSITION position_custom;
 // normal information
 uint16 normal_vertex_offset;
 uint16 normal_flags;
 uint16 normal_stride;
 uint16 normal_type;
 real32 normal_bias;
 real32 normal_scale;
 DSPCUSTOMNORMAL normal_custom;
 // weight information
 uint16 weights_vertex_offset;
 uint16 weights_flags;
 uint16 weights_stride;
 uint16 weights_elem;
 uint16 weights_type;
 DSPCUSTOMWEIGHTS weights_custom;
 // weight map information
 uint16 joints_vertex_offset;
 uint16 joints_flags;
 uint16 joints_stride;
 uint16 joints_elem;
 uint16 joints_type;
 DSPCUSTOMJOINTS joints_custom;
 // texture information
 uint16 texture_channels;
 uint16 texture_vertex_offset[16];
 uint16 texture_size[16];
 uint16 texture_type[16];
 real32 texture_bias[16];
 real32 texture_scale[16];
 real32 texture_uinvert[16];
 real32 texture_vinvert[16];
 DSPCUSTOMTEXTURE texture_custom[16];
};

struct DSPIDXBUFFER {
};

struct DSPVERTEX {
 real32 vx, vy, vz, vw;
 real32 nx, ny, nz, nw;
 real32 tu, tv;
 real32 wv[8];
 uint16 wi[8];
};

bool SaveLWO(const DSPFILEINFO& fd, const std::deque<DSPVTXBUFFER> vd, const std::deque<DSPIDXBUFFER> id);
bool SaveOBJ(const DSPFILEINFO& fd, const std::deque<DSPVTXBUFFER> vd, const std::deque<DSPIDXBUFFER> id);
bool TestDSP(void);
*/

#endif
