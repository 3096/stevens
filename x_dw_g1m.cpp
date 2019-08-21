#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_dw_g1m.h"
using namespace std;

void matrix4x4_to_euler_XYZ(real32* v, const real32* m)
{
 // m11(0) m12(1) m13(2) m14(3)
 // m21(4) m22(5) m23(6) m24(7)
 // m31(8) m32(9) m33(A) m34(B)
 // m41(C) m42(D) m43(E) m44(F)
 if(m[0x8] == 1.0f) {
    v[2] = 0.0f;
    v[1] = -cs::math::constants<real32>::pi_over_2();
    v[0] = std::atan2(-m[0x1], -m[0x2]);
   }
 else if(m[0x8] == -1.0f) {
    v[2] = 0.0f;
    v[1] = cs::math::constants<real32>::pi_over_2();
    v[0] = std::atan2(m[0x1], m[0x2]);
   }
 else {
    v[1] = -std::asin(m[0x8]);
    real32 div = std::cos(v[1]);
    v[0] = std::atan2(m[0x9]/div, m[0xA]/div);
    v[2] = std::atan2(m[0x4]/div, m[0x0]/div);
   }
}

#pragma region REGION_GLOBALS

static bool debug = false;

#pragma endregion

#pragma region REGION_CLOTH_STRUCTURES

struct DWfloat4 {
 real32 x;
 real32 y;
 real32 z;
 real32 w;
 inline DWfloat4 operator -(void)const
 {
  DWfloat4 retval;
  retval.x = -x;
  retval.y = -y;
  retval.z = -z;
  retval.w = -w;
  return retval;
 }
};

struct DWaddress4 {
 uint32 x;
 uint32 y;
 uint32 z;
 uint32 w;
};

struct DWCLOTHINPUT {
 DWfloat4 v0;  // dcl_position
 DWfloat4 v1;  // dcl_blendweight
 DWfloat4 v2;  // dcl_binormal
 DWfloat4 v3;  // dcl_color1
 DWfloat4 v4;  // dcl_blendindices
 DWfloat4 v5;  // dcl_psize 
 DWfloat4 v6;  // dcl_fog
 DWfloat4 v7;  // dcl_texcoord5
 DWfloat4 v8;  // dcl_normal
 DWfloat4 v9;  // dcl_tangent
 DWfloat4 v10; // dcl_texcoord0
 DWfloat4 v11; // dcl_texcoord1
 DWfloat4 v12; // dcl_texcoord2
};

struct DWCLOTHOUTPUT {
 DWfloat4 o0;
 DWfloat4 o1;
 DWfloat4 o2;
 DWfloat4 o3;
 DWfloat4 o4;
};

#pragma endregion

#pragma region REGION_CLOTH_FUNCTIONS

inline uint32 DWRoundNearest(real32 r)
{
 return static_cast<unsigned int>((r > 0.0f) ? (r + 0.5f) : (0.0f)); 
}

void d3d_copy(DWfloat4& dst, DWfloat4 src, const char* swizzle)
{
 // no swizzle = direct copy
 size_t elem = strlen(swizzle);
 if(elem == 0) {
    dst.x = src.x;
    dst.y = src.y;
    dst.z = src.z;
    dst.w = src.w;
    return;
   }

 // swizzle should always have four elements
 char usage[4];
 if(elem == 1) {
    usage[0] = swizzle[0];
    usage[1] = usage[0];
    usage[2] = usage[1];
    usage[3] = usage[2];
   }
 else if(elem == 2) {
    usage[0] = swizzle[0];
    usage[1] = swizzle[1];
    usage[2] = usage[1];
    usage[3] = usage[2];
   }
 else if(elem == 3) {
    usage[0] = swizzle[0];
    usage[1] = swizzle[1];
    usage[2] = swizzle[2];
    usage[3] = usage[2];
   }
 else {
    usage[0] = swizzle[0];
    usage[1] = swizzle[1];
    usage[2] = swizzle[2];
    usage[3] = swizzle[3];
   }

 // copy based on swizzle
 switch(usage[0]) {
   case('x') : dst.x = src.x; break;
   case('y') : dst.x = src.y; break;
   case('z') : dst.x = src.z; break;
   case('w') : dst.x = src.w; break;
  }
 switch(usage[1]) {
   case('x') : dst.y = src.x; break;
   case('y') : dst.y = src.y; break;
   case('z') : dst.y = src.z; break;
   case('w') : dst.y = src.w; break;
  }
 switch(usage[2]) {
   case('x') : dst.z = src.x; break;
   case('y') : dst.z = src.y; break;
   case('z') : dst.z = src.z; break;
   case('w') : dst.z = src.w; break;
  }
 switch(usage[3]) {
   case('x') : dst.w = src.x; break;
   case('y') : dst.w = src.y; break;
   case('z') : dst.w = src.z; break;
   case('w') : dst.w = src.w; break;
  }
}

void d3d_mask(DWfloat4& dst, DWfloat4 src, const char* mask)
{
 size_t elem = strlen(mask);
 if(!elem) {
    dst.x = src.x;
    dst.y = src.y;
    dst.z = src.z;
    dst.w = src.w;
   }
 else {
    for(size_t i = 0; i < elem; i++) {
        if(mask[i] == 'x') dst.x = src.x;
        else if(mask[i] == 'y') dst.y = src.y;
        else if(mask[i] == 'z') dst.z = src.z;
        else if(mask[i] == 'w') dst.w = src.w;
       }
   }
}

void d3d_mask(DWaddress4& dst, DWaddress4 src, const char* mask)
{
 size_t elem = strlen(mask);
 if(!elem) {
    dst.x = src.x;
    dst.y = src.y;
    dst.z = src.z;
    dst.w = src.w;
   }
 else {
    for(size_t i = 0; i < elem; i++) {
        if(mask[i] == 'x') dst.x = src.x;
        else if(mask[i] == 'y') dst.y = src.y;
        else if(mask[i] == 'z') dst.z = src.z;
        else if(mask[i] == 'w') dst.w = src.w;
       }
   }
}

void d3d_add(DWfloat4& dest, const char* mask, DWfloat4 src0, const char* src0_swizzle, DWfloat4 src1, const char* src1_swizzle)
{
 // temporaries
 DWfloat4 s0; d3d_copy(s0, src0, src0_swizzle);
 DWfloat4 s1; d3d_copy(s1, src1, src1_swizzle);

 // compute value
 DWfloat4 result;
 result.x = s0.x + s1.x;
 result.y = s0.y + s1.y;
 result.z = s0.z + s1.z;
 result.w = s0.w + s1.w;

 // apply mask
 d3d_mask(dest, result, mask);
}

void d3d_dp4(DWfloat4& dst, const char* mask, DWfloat4 src0, const char* src0_swizzle, DWfloat4 src1, const char* src1_swizzle)
{
 // temporaries
 DWfloat4 s0; d3d_copy(s0, src0, src0_swizzle);
 DWfloat4 s1; d3d_copy(s1, src1, src1_swizzle);

 // compute value
 DWfloat4 result;
 result.x = s0.x*s1.x + s0.y*s1.y + s0.z*s1.z + s0.w*s1.w;
 result.y = result.x;
 result.z = result.x;
 result.w = result.x;

 // apply mask
 d3d_mask(dst, result, mask);
}

void d3d_lrp(DWfloat4& dest, const char* mask, DWfloat4 src0, const char* src0_swizzle, DWfloat4 src1, const char* src1_swizzle, DWfloat4 src2, const char* src2_swizzle)
{
 // temporaries
 DWfloat4 s0; d3d_copy(s0, src0, src0_swizzle);
 DWfloat4 s1; d3d_copy(s1, src1, src1_swizzle);
 DWfloat4 s2; d3d_copy(s2, src2, src2_swizzle);

 // compute value
 DWfloat4 result;
 result.x = s0.x * (s1.x - s2.x) + s2.x;
 result.y = s0.y * (s1.y - s2.y) + s2.y;
 result.z = s0.z * (s1.z - s2.z) + s2.z;
 result.w = s0.w * (s1.w - s2.w) + s2.w;

 // apply mask
 d3d_mask(dest, result, mask);
}

void d3d_mov(DWfloat4& dst, const char* mask, DWfloat4 src, const char* swizzle)
{
 DWfloat4 tmp;
 d3d_copy(tmp, src, swizzle);
 d3d_mask(dst, tmp, mask);
}

void d3d_mova(DWaddress4& dst, const char* mask, DWfloat4 src, const char* swizzle)
{
 // copy to temporary
 DWfloat4 tmp;
 d3d_copy(tmp, src, swizzle);

 // compute value
 DWaddress4 result;
 result.x = DWRoundNearest(tmp.x);
 result.y = DWRoundNearest(tmp.y);
 result.z = DWRoundNearest(tmp.z);
 result.w = DWRoundNearest(tmp.w);

 // apply mask
 d3d_mask(dst, result, mask);
}

void d3d_mad(DWfloat4& dest, const char* mask, DWfloat4 src0, const char* src0_swizzle, DWfloat4 src1, const char* src1_swizzle, DWfloat4 src2, const char* src2_swizzle)
{
 // temporaries
 DWfloat4 s0; d3d_copy(s0, src0, src0_swizzle);
 DWfloat4 s1; d3d_copy(s1, src1, src1_swizzle);
 DWfloat4 s2; d3d_copy(s2, src2, src2_swizzle);

 // compute value
 DWfloat4 result;
 result.x = s0.x * s1.x + s2.x;
 result.y = s0.y * s1.y + s2.y;
 result.z = s0.z * s1.z + s2.z;
 result.w = s0.w * s1.w + s2.w;

 // apply mask
 d3d_mask(dest, result, mask);
}

void d3d_mul(DWfloat4& dest, const char* mask, DWfloat4 src0, const char* src0_swizzle, DWfloat4 src1, const char* src1_swizzle)
{
 // temporaries
 DWfloat4 s0; d3d_copy(s0, src0, src0_swizzle);
 DWfloat4 s1; d3d_copy(s1, src1, src1_swizzle);

 // compute value
 DWfloat4 result;
 result.x = s0.x * s1.x;
 result.y = s0.y * s1.y;
 result.z = s0.z * s1.z;
 result.w = s0.w * s1.w;

 // apply mask
 d3d_mask(dest, result, mask);
}

void d3d_slt(DWfloat4& dest, const char* mask, DWfloat4 src0, const char* src0_swizzle, DWfloat4 src1, const char* src1_swizzle)
{
 // temporaries
 DWfloat4 s0; d3d_copy(s0, src0, src0_swizzle);
 DWfloat4 s1; d3d_copy(s1, src1, src1_swizzle);

 // compute value
 DWfloat4 result;
 result.x = (s0.x < s1.x) ? 1.0f : 0.0f;
 result.y = (s0.y < s1.y) ? 1.0f : 0.0f;
 result.z = (s0.z < s1.z) ? 1.0f : 0.0f;
 result.w = (s0.w < s1.w) ? 1.0f : 0.0f;

 // apply mask
 d3d_mask(dest, result, mask);
}

inline DWfloat4* DWIndexedConstant(DWfloat4* c, uint32 base, uint32 index)
{
 // c78[0] is c78
 // c78[1] is c79
 size_t offset = base + index;
 return &c[offset];
}

inline DWfloat4 DWIC(DWfloat4* c, uint32 base, uint32 index)
{
 // c78[0] is c78
 // c78[1] is c79
 size_t offset = base + index;
 return c[offset];
}

bool TransformClothVertex1(const DWCLOTHINPUT& input, DWfloat4* c, size_t cn, const bool* b, size_t bn, DWCLOTHOUTPUT& output)
{
 // validate constant buffers
 if(!c || (cn < 511)) return false;
 if(!b || (bn < 2)) return false;

 // input and output registers
 DWfloat4 v0 = input.v0;   // dcl_position
 DWfloat4 v1 = input.v1;   // dcl_blendweight
 DWfloat4 v2 = input.v2;   // dcl_binormal
 DWfloat4 v3 = input.v3;   // dcl_color1
 DWfloat4 v4 = input.v4;   // dcl_blendindices
 DWfloat4 v5 = input.v5;   // dcl_psize 
 DWfloat4 v6 = input.v6;   // dcl_fog
 DWfloat4 v7 = input.v7;   // dcl_texcoord5
 DWfloat4 v8 = input.v8;   // dcl_normal
 DWfloat4 v9 = input.v9;   // dcl_texcoord0
 DWfloat4 v10 = input.v10; // dcl_texcoord0
 DWfloat4 v11 = input.v11; // dcl_texcoord1
 DWfloat4 v12 = input.v12; // dcl_texcoord2
 DWfloat4 o0; // dcl_position
 DWfloat4 o1; // dcl_position
 DWfloat4 o2; // dcl_texcoord0
 DWfloat4 o3; // dcl_texcoord1
 DWfloat4 o4; // dcl_texcoord2

 // other registers
 DWfloat4 r[32];  // temporary registers
 DWaddress4 a[1]; // address registers

 // temporary values
 real32 temp = 0.0f;
 DWfloat4* ctmp = nullptr;

 // changed to 500 to make room for more control points, used to be 192
 static const int MW2P_INDEX = 500; // used to be 192, 193, 194, 195
 static const int CV0 = 504; // used to be 196
 static const int CV1 = 505; // used to be 197
 static const int CV2 = 506; // used to be 198

 // default values
 c[CV1].x = 0; // c[197].x = 0;
 c[CV1].y = 1; // c[197].y = 1;
 c[CV1].z = 0; // c[197].z = 0;
 c[CV1].w = 0; // c[197].w = 0;

 c[CV2].x = -0; // c[198].x = -0;
 c[CV2].y = -3; // c[198].y = -3;
 c[CV2].z = -6; // c[198].z = -6;
 c[CV2].w = -9; // c[198].w = -9;

 //
 // APPENDING TO R1
 //

 // VERIFIED
 // [0] dp4 r0.x, v5, v5
 // [1] slt r0.x, c197.x, r0.x
 // [2] mov r0.yzw, v4.xxyz
 r[0].x = v5.x*v5.x + v5.y*v5.y + v5.z*v5.z + v5.w*v5.w;
 r[0].x = (c[CV1].x < r[0].x) ? 1.0f : 0.0f;
 r[0].y = v4.x;
 r[0].z = v4.y;
 r[0].w = v4.z;

 // VERIFIED
 // [3] mova a0.w, r0.z
 // [4] mul r1.xyz, v0.y, c96[a0.w].zxyw
 a[0].w = DWRoundNearest(r[0].z);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[1].x = v0.y * ctmp->z;
 r[1].y = v0.y * ctmp->x;
 r[1].z = v0.y * ctmp->y;

 // VERIFIED
 // [5] mova a0.w, r0.y
 // [6] mad r1.xyz, v0.x, c96[a0.w].zxyw, r1
 a[0].w = DWRoundNearest(r[0].y);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[1].x = v0.x * ctmp->z + r[1].x;
 r[1].y = v0.x * ctmp->x + r[1].y;
 r[1].z = v0.x * ctmp->y + r[1].z;

 // VERIFIED
 // [7] mova a0.w, r0.w
 // [8] mad r1.xyz, v0.z, c96[a0.w].zxyw, r1
 a[0].w = DWRoundNearest(r[0].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[1].x = v0.z * ctmp->z + r[1].x;
 r[1].y = v0.z * ctmp->x + r[1].y;
 r[1].z = v0.z * ctmp->y + r[1].z;

 // VERIFIED
 // [9] mov r1.w, v4.w
 // [10] mova a0.w, r1.w
 // [11] mad r1.xyz, v0.w, c96[a0.w].zxyw, r1
 r[1].w = v4.w;
 a[0].w = DWRoundNearest(r[1].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[1].x = v0.w * ctmp->z + r[1].x;
 r[1].y = v0.w * ctmp->x + r[1].y;
 r[1].z = v0.w * ctmp->y + r[1].z;

 //
 // APPENDING TO R3
 //

 // VERIFIED
 // [12] mov r2, v5
 r[2].x = v5.x;
 r[2].y = v5.y;
 r[2].z = v5.z;
 r[2].w = v5.w;

 // VERIFIED
 // [13] mova a0.w, r2.y
 // [14] mul r3.xyz, v0.y, c96[a0.w].zxyw
 a[0].w = DWRoundNearest(r[2].y);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v0.y * ctmp->z;
 r[3].y = v0.y * ctmp->x;
 r[3].z = v0.y * ctmp->y;

 // VERIFIED
 // [15] mova a0.w, r2.x
 // [16] mad r3.xyz, v0.x, c96[a0.w].zxyw, r3
 a[0].w = DWRoundNearest(r[2].x);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v0.x * ctmp->z + r[3].x;
 r[3].y = v0.x * ctmp->x + r[3].y;
 r[3].z = v0.x * ctmp->y + r[3].z;

 // VERIFIED
 // [17] mova a0.w, r2.z
 // [18] mad r3.xyz, v0.z, c96[a0.w].zxyw, r3
 a[0].w = DWRoundNearest(r[2].z);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v0.z * ctmp->z + r[3].x;
 r[3].y = v0.z * ctmp->x + r[3].y;
 r[3].z = v0.z * ctmp->y + r[3].z;

 // VERIFIED
 // [19] mova a0.w, r2.w
 // [20] mad r3.xyz, v0.w, c96[a0.w].zxyw, r3
 a[0].w = DWRoundNearest(r[2].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v0.w * ctmp->z + r[3].x;
 r[3].y = v0.w * ctmp->x + r[3].y;
 r[3].z = v0.w * ctmp->y + r[3].z;


 //
 // APPENDING TO R4
 //

 // VERIFIED
 // [21] mov r3.w, v6.x
 // [22] mov r4.x, v6.y
 r[3].w = v6.x;
 r[4].x = v6.y;

 // VERIFIED
 // [23] mova a0.w, r4.x
 // [24] mul r4.yzw, v0.y, c96[a0.w].xzxy
 a[0].w = DWRoundNearest(r[4].x);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[4].y = v0.y * ctmp->z;
 r[4].z = v0.y * ctmp->x;
 r[4].w = v0.y * ctmp->y;

 // VERIFIED
 // [25] mova a0.w, r3.w
 // [26] mad r4.yzw, v0.x, c96[a0.w].xzxy, r4
 a[0].w = DWRoundNearest(r[3].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[4].y = v0.x * ctmp->z + r[4].y;
 r[4].z = v0.x * ctmp->x + r[4].z;
 r[4].w = v0.x * ctmp->y + r[4].w;

 // VERIFIED
 // [27] mov r5.xy, v6.zwzw
 r[5].x = v6.z;
 r[5].y = v6.w;

 // VERIFIED
 // [28] mova a0.w, r5.x
 // [29] mad r4.yzw, v0.z, c96[a0.w].xzxy, r4
 a[0].w = DWRoundNearest(r[5].x);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[4].y = v0.z * ctmp->z + r[4].y;
 r[4].z = v0.z * ctmp->x + r[4].z;
 r[4].w = v0.z * ctmp->y + r[4].w;

 // VERIFIED
 // [30] mova a0.w, r5.y
 // [31] mad r4.yzw, v0.w, c96[a0.w].xzxy, r4
 a[0].w = DWRoundNearest(r[5].y);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[4].y = v0.w * ctmp->z + r[4].y;
 r[4].z = v0.w * ctmp->x + r[4].z;
 r[4].w = v0.w * ctmp->y + r[4].w;

 //
 // APPENDING TO R6
 //

 // VERIFIED
 // [32] mov r5.zw, v7.xyxy
 r[5].z = v7.x;
 r[5].w = v7.y;

 // VERIFIED
 // [33] mova a0.w, r5.w
 // [34] mul r6.xyz, v0.y, c96[a0.w].zxyw
 a[0].w = DWRoundNearest(r[5].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[6].x = v0.y * ctmp->z;
 r[6].y = v0.y * ctmp->x;
 r[6].z = v0.y * ctmp->y;

 // VERIFIED
 // [35] mova a0.w, r5.z
 // [36] mad r6.xyz, v0.x, c96[a0.w].zxyw, r6
 a[0].w = DWRoundNearest(r[5].z);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[6].x = v0.x * ctmp->z + r[6].x;
 r[6].y = v0.x * ctmp->x + r[6].y;
 r[6].z = v0.x * ctmp->y + r[6].z;

 // VERIFIED
 // [37] mov r6.w, v7.z
 r[6].w = v7.z;

 // VERIFIED
 // [38] mova a0.w, r6.w
 // [39] mad r6.xyz, v0.z, c96[a0.w].zxyw, r6
 a[0].w = DWRoundNearest(r[6].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[6].x = v0.z * ctmp->z + r[6].x;
 r[6].y = v0.z * ctmp->x + r[6].y;
 r[6].z = v0.z * ctmp->y + r[6].z;

 // VERIFIED
 // [40] mov r7.x, v7.w
 r[7].x = v7.w;

 // VERIFIED
 // [41] mova a0.w, r7.x
 // [42] mad r6.xyz, v0.w, c96[a0.w].zxyw, r6
 a[0].w = DWRoundNearest(r[7].x);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[6].x = v0.w * ctmp->z + r[6].x;
 r[6].y = v0.w * ctmp->x + r[6].y;
 r[6].z = v0.w * ctmp->y + r[6].z;

 //
 // APPENDING TO R7
 //

 // VERIFIED
 // [43] mul r7.yzw, r3.xyzx, v1.y
 r[7].y = r[3].y * v1.y;
 r[7].z = r[3].z * v1.y;
 r[7].w = r[3].x * v1.y;

 // VERIFIED
 // [44] mad r7.yzw, v1.x, r1.xyzx, r7
 r[7].y = v1.x * r[1].y + r[7].y;
 r[7].z = v1.x * r[1].z + r[7].z;
 r[7].w = v1.x * r[1].x + r[7].w;

 // VERIFIED
 // [45] mad r7.yzw, v1.z, r4.xzwy, r7
 r[7].y = v1.z * r[4].z + r[7].y;
 r[7].z = v1.z * r[4].w + r[7].z;
 r[7].w = v1.z * r[4].y + r[7].w;

 // VERIFIED
 // [46] mad r7.yzw, v1.w, r6.xyzx, r7
 r[7].y = v1.w * r[6].y + r[7].y;
 r[7].z = v1.w * r[6].z + r[7].z;
 r[7].w = v1.w * r[6].x + r[7].w;

 //
 // APPENDING TO R1
 //

 // VERIFIED
 // [47] mul r3.xyz, r3, v3.y
 r[3].x = r[3].x * v3.y;
 r[3].y = r[3].y * v3.y;
 r[3].z = r[3].z * v3.y;

 // VERIFIED
 // [48] mad r1.xyz, v3.x, r1, r3
 r[1].x = v3.x * r[1].x + r[3].x;
 r[1].y = v3.x * r[1].y + r[3].y;
 r[1].z = v3.x * r[1].z + r[3].z;

 // VERIFIED
 // [49] mad r1.xyz, v3.z, r4.yzww, r1
 r[1].x = v3.z * r[4].y + r[1].x;
 r[1].y = v3.z * r[4].z + r[1].y;
 r[1].z = v3.z * r[4].w + r[1].z;

 // VERIFIED
 // [50] mad r1.xyz, v3.w, r6, r1
 r[1].x = v3.w * r[6].x + r[1].x;
 r[1].y = v3.w * r[6].y + r[1].y;
 r[1].z = v3.w * r[6].z + r[1].z;

 //
 // APPENDING TO R3, R0
 //

 // VERIFIED
 // [51] mova a0.w, r0.z
 // [52] mul r3.xyz, v2.y, c96[a0.w].yzxw
 a[0].w = DWRoundNearest(r[0].z);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v2.y * ctmp->y;
 r[3].y = v2.y * ctmp->z;
 r[3].z = v2.y * ctmp->x;

 // VERIFIED
 // [53] mova a0.w, r0.y
 // [54] mad r3.xyz, v2.x, c96[a0.w].yzxw, r3
 a[0].w = DWRoundNearest(r[0].y);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v2.x * ctmp->y + r[3].x;
 r[3].y = v2.x * ctmp->z + r[3].y;
 r[3].z = v2.x * ctmp->x + r[3].z;

 // VERIFIED
 // [55] mova a0.w, r0.w
 // [56] mad r0.yzw, v2.z, c96[a0.w].xyzx, r3.xxyz
 a[0].w = DWRoundNearest(r[0].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[0].y = v2.z * ctmp->y + r[3].x;
 r[0].z = v2.z * ctmp->z + r[3].y;
 r[0].w = v2.z * ctmp->x + r[3].z;

 // VERIFIED
 // [57] mova a0.w, r1.w
 // [58] mad r0.yzw, v2.w, c96[a0.w].xyzx, r0
 a[0].w = DWRoundNearest(r[1].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[0].y = v2.w * ctmp->y + r[0].y;
 r[0].z = v2.w * ctmp->z + r[0].z;
 r[0].w = v2.w * ctmp->x + r[0].w;

 //
 // APPENDING TO R3, R2
 //

 // VERIFIED
 // [59] mova a0.w, r2.y
 // [60] mul r3.xyz, v2.y, c96[a0.w].yzxw
 a[0].w = DWRoundNearest(r[2].y);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v2.y * ctmp->y;
 r[3].y = v2.y * ctmp->z;
 r[3].z = v2.y * ctmp->x;

 // VERIFIED
 // [61] mova a0.w, r2.x
 // [62] mad r3.xyz, v2.x, c96[a0.w].yzxw, r3
 a[0].w = DWRoundNearest(r[2].x);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v2.x * ctmp->y + r[3].x;
 r[3].y = v2.x * ctmp->z + r[3].y;
 r[3].z = v2.x * ctmp->x + r[3].z;

 // VERIFIED
 // [63] mova a0.w, r2.z
 // [64] mad r2.xyz, v2.z, c96[a0.w].yzxw, r3
 a[0].w = DWRoundNearest(r[2].z);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[2].x = v2.z * ctmp->y + r[3].x;
 r[2].y = v2.z * ctmp->z + r[3].y;
 r[2].z = v2.z * ctmp->x + r[3].z;

 // VERIFIED
 // [65] mova a0.w, r2.w
 // [66] mad r2.xyz, v2.w, c96[a0.w].yzxw, r2
 a[0].w = DWRoundNearest(r[2].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[2].x = v2.w * ctmp->y + r[2].x;
 r[2].y = v2.w * ctmp->z + r[2].y;
 r[2].z = v2.w * ctmp->x + r[2].z;

 //
 // APPENDING TO R3
 //

 // [67] mova a0.w, r4.x
 // [68] mul r3.xyz, v2.y, c96[a0.w].yzxw
 a[0].w = DWRoundNearest(r[4].x);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v2.y * ctmp->y;
 r[3].y = v2.y * ctmp->z;
 r[3].z = v2.y * ctmp->x;

 // [69] mova a0.w, r3.w
 // [70] mad r3.xyz, v2.x, c96[a0.w].yzxw, r3
 a[0].w = DWRoundNearest(r[3].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v2.x * ctmp->y + r[3].x;
 r[3].y = v2.x * ctmp->z + r[3].y;
 r[3].z = v2.x * ctmp->x + r[3].z;

 // [71] mova a0.w, r5.x
 // [72] mad r3.xyz, v2.z, c96[a0.w].yzxw, r3
 a[0].w = DWRoundNearest(r[5].x);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v2.z * ctmp->y + r[3].x;
 r[3].y = v2.z * ctmp->z + r[3].y;
 r[3].z = v2.z * ctmp->x + r[3].z;

 // [73] mova a0.w, r5.y
 // [74] mad r3.xyz, v2.w, c96[a0.w].yzxw, r3
 a[0].w = DWRoundNearest(r[5].y);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[3].x = v2.w * ctmp->y + r[3].x;
 r[3].y = v2.w * ctmp->z + r[3].y;
 r[3].z = v2.w * ctmp->x + r[3].z;

 //
 // APPENDING TO R4
 //

 // [75] mova a0.w, r5.w
 // [76] mul r4.xyz, v2.y, c96[a0.w].yzxw
 a[0].w = DWRoundNearest(r[5].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[4].x = v2.y * ctmp->y;
 r[4].y = v2.y * ctmp->z;
 r[4].z = v2.y * ctmp->x;

 // [77] mova a0.w, r5.z
 // [78] mad r4.xyz, v2.x, c96[a0.w].yzxw, r4
 a[0].w = DWRoundNearest(r[5].z);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[4].x = v2.x * ctmp->y + r[4].x;
 r[4].y = v2.x * ctmp->z + r[4].y;
 r[4].z = v2.x * ctmp->x + r[4].z;

 // [79] mova a0.w, r6.w
 // [80] mad r4.xyz, v2.z, c96[a0.w].yzxw, r4
 a[0].w = DWRoundNearest(r[6].w);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[4].x = v2.z * ctmp->y + r[4].x;
 r[4].y = v2.z * ctmp->z + r[4].y;
 r[4].z = v2.z * ctmp->x + r[4].z;

 // [81] mova a0.w, r7.x
 // [82] mad r4.xyz, v2.w, c96[a0.w].yzxw, r4
 a[0].w = DWRoundNearest(r[7].x);
 ctmp = DWIndexedConstant(c, 96, a[0].w);
 r[4].x = v2.w * ctmp->y + r[4].x;
 r[4].y = v2.w * ctmp->z + r[4].y;
 r[4].z = v2.w * ctmp->x + r[4].z;

 //
 // APPENDING TO R0, R1
 //

 // VERIFIED
 // [83] mul r2.xyz, r2, v1.y
 r[2].x = r[2].x * v1.y;
 r[2].y = r[2].y * v1.y;
 r[2].z = r[2].z * v1.y;

 // VERIFIED
 // [84] mad r0.yzw, v1.x, r0, r2.xxyz
 r[0].y = v1.x * r[0].y + r[2].x;
 r[0].z = v1.x * r[0].z + r[2].y;
 r[0].w = v1.x * r[0].w + r[2].z;

 // VERIFIED
 // [85] mad r0.yzw, v1.z, r3.xxyz, r0
 r[0].y = v1.z * r[3].x + r[0].y;
 r[0].z = v1.z * r[3].y + r[0].z;
 r[0].w = v1.z * r[3].z + r[0].w;

 // VERIFIED
 // [86] mad r0.yzw, v1.w, r4.xxyz, r0
 r[0].y = v1.w * r[4].x + r[0].y;
 r[0].z = v1.w * r[4].y + r[0].z;
 r[0].w = v1.w * r[4].z + r[0].w;

 // VERIFIED
 // [87] mul r2.xyz, r0.yzww, r1
 r[2].x = r[0].y * r[1].x;
 r[2].y = r[0].z * r[1].y;
 r[2].z = r[0].w * r[1].z;

 // VERIFIED
 // [88] mad r0.yzw, r1.xzxy, r0.xzwy, -r2.xxyz
 temp = r[0].y;
 r[0].y = r[1].z * r[0].z - r[2].x;
 r[0].z = r[1].x * r[0].w - r[2].y;
 r[0].w = r[1].y * temp - r[2].z;

 // VERIFIED
 // [89] mad r1.xyz, v8.w, r0.yzww, r7.yzww
 r[1].x = v8.w * r[0].y + r[7].y;
 r[1].y = v8.w * r[0].z + r[7].z;
 r[1].z = v8.w * r[0].w + r[7].w;

 //
 // SETUP
 //

 // VERIFIED
 // [90] mov r1.w, c197.y
 // [91] lrp r2, r0.x, r1, v0
 r[1].w = c[CV1].y;
 r[2].x = r[0].x * (r[1].x - v0.x) + v0.x;
 r[2].y = r[0].x * (r[1].y - v0.y) + v0.y;
 r[2].z = r[0].x * (r[1].z - v0.z) + v0.z;
 r[2].w = r[0].x * (r[1].w - v0.w) + v0.w;

 // VERIFIED
 // [92] mad r1, r0.x, -v4, v4
 r[1].x = r[0].x * (-v4.x) + v4.x;
 r[1].y = r[0].x * (-v4.y) + v4.y;
 r[1].z = r[0].x * (-v4.z) + v4.z;
 r[1].w = r[0].x * (-v4.w) + v4.w;

 // VERIFIED
 // [93] lrp r3.xyz, r0.x, c197.yxxw, v1
 r[3].x = r[0].x * (c[CV1].y - v1.x) + v1.x;
 r[3].y = r[0].x * (c[CV1].x - v1.y) + v1.y;
 r[3].z = r[0].x * (c[CV1].x - v1.z) + v1.z;

 // VERIFIED
 // [94] mov r0.x, c197.x
 // [95] slt r0.x, r0.x, c196.x
 r[0].x = c[205].x;
 r[0].x = (r[0].x < c[CV0].x) ? 1.0f : 0.0f;

 // VERIFIED
 // [96] add r1, r1, c198
 r[1].x = r[1].x + c[CV2].x;
 r[1].y = r[1].y + c[CV2].y;
 r[1].z = r[1].z + c[CV2].z;
 r[1].w = r[1].w + c[CV2].w;

 // FIXED AND VERIFIED
 // [97] mul r0, r0.x, r1
 temp = r[0].x;
 r[0].x = temp * r[1].x;
 r[0].y = temp * r[1].y;
 r[0].z = temp * r[1].z;
 r[0].w = temp * r[1].w;

 // VERIFIED
 // [98] mova a0.w, r0.x
 // [99] dp4 r1.x, r2, c0[a0.w]
 a[0].w = DWRoundNearest(r[0].x);
 ctmp = DWIndexedConstant(c, 0, a[0].w);
 r[1].x = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

 // VERIFIED
 // [100] mova a0.w, r0.x
 // [101] dp4 r1.y, r2, c1[a0.w]
 a[0].w = DWRoundNearest(r[0].x);
 ctmp = DWIndexedConstant(c, 1, a[0].w);
 r[1].y = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

 // VERIFIED
 // [102] mova a0.w, r0.x
 // [103] dp4 r1.z, r2, c2[a0.w]
 a[0].w = DWRoundNearest(r[0].x);
 ctmp = DWIndexedConstant(c, 2, a[0].w);
 r[1].z = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

 // VERIFIED
 // [104] mul r4.xyz, r3.x, r1
 r[4].x = r[3].x * r[1].x;
 r[4].y = r[3].x * r[1].y;
 r[4].z = r[3].x * r[1].z;

 // [105] if b1
 if(b[1])
   {
    // [106] mova a0.w, r0.y
    // [107] dp4 r5.x, r2, c3[a0.w]
    a[0].w = DWRoundNearest(r[0].y);
    ctmp = DWIndexedConstant(c, 3, a[0].w);
    r[5].x = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // [108] mova a0.w, r0.y
    // [109] dp4 r5.y, r2, c4[a0.w]
    a[0].w = DWRoundNearest(r[0].y);
    ctmp = DWIndexedConstant(c, 4, a[0].w);
    r[5].y = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // [110] mova a0.w, r0.y
    // [111] dp4 r5.z, r2, c5[a0.w]
    a[0].w = DWRoundNearest(r[0].y);
    ctmp = DWIndexedConstant(c, 5, a[0].w);
    r[5].z = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // [112] mad r6.xyz, r3.y, r5, r4
    r[6].x = r[3].y * r[5].x + r[4].x;
    r[6].y = r[3].y * r[5].y + r[4].y;
    r[6].z = r[3].y * r[5].z + r[4].z;

    // [113] mova a0.w, r0.z
    // [114] dp4 r7.x, r2, c6[a0.w]
    a[0].w = DWRoundNearest(r[0].z);
    ctmp = DWIndexedConstant(c, 6, a[0].w);
    r[7].x = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // [115] mova a0.w, r0.z
    // [116] dp4 r7.y, r2, c7[a0.w]
    a[0].w = DWRoundNearest(r[0].z);
    ctmp = DWIndexedConstant(c, 7, a[0].w);
    r[7].y = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // [117] mova a0.w, r0.z
    // [118] dp4 r7.z, r2, c8[a0.w]
    a[0].w = DWRoundNearest(r[0].z);
    ctmp = DWIndexedConstant(c, 8, a[0].w);
    r[7].z = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // [119] mad r6.xyz, r3.z, r7, r6
    r[6].x = r[3].z * r[7].x + r[6].x;
    r[6].y = r[3].z * r[7].y + r[6].y;
    r[6].z = r[3].z * r[7].z + r[6].z;

    // [120] add r0.x, -r3.x, c197.y
    // [121] add r0.x, -r3.y, r0.x
    // [122] add r0.x, -r3.z, r0.x
    r[0].x = -r[3].x + c[CV1].y;
    r[0].x = -r[3].y + r[0].x;
    r[0].x = -r[3].z + r[0].x;

    // [123] mova a0.w, r0.w
    // [124] dp4 r7.x, r2, c9[a0.w]
    a[0].w = DWRoundNearest(r[0].w);
    ctmp = DWIndexedConstant(c, 9, a[0].w);
    r[7].x = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // [125] mova a0.w, r0.w
    // [126] dp4 r7.y, r2, c10[a0.w]
    a[0].w = DWRoundNearest(r[0].w);
    ctmp = DWIndexedConstant(c, 10, a[0].w);
    r[7].y = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // [127] mova a0.w, r0.w
    // [128] dp4 r7.z, r2, c11[a0.w]
    a[0].w = DWRoundNearest(r[0].w);
    ctmp = DWIndexedConstant(c, 11, a[0].w);
    r[7].z = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // [129] mad r6.xyz, r0.x, r7, r6
    r[6].x = r[0].x * r[7].x + r[6].x;
    r[6].y = r[0].x * r[7].y + r[6].y;
    r[6].z = r[0].x * r[7].z + r[6].z;

    // [130] lrp r2.xyz, r3.x, r1, r5
    r[2].x = r[3].x * (r[1].x - r[5].x) + r[5].x;
    r[2].y = r[3].x * (r[1].y - r[5].y) + r[5].y;
    r[2].z = r[3].x * (r[1].z - r[5].z) + r[5].z;
   }
 // [131] else
 else
   {
    // VERIFIED
    // [132] mova a0.w, r0.y
    // [133] dp4 r5.x, r2, c3[a0.w]
    a[0].w = DWRoundNearest(r[0].y);
    ctmp = DWIndexedConstant(c, 3, a[0].w);
    r[5].x = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // VERIFIED
    // [134] mova a0.w, r0.y
    // [135] dp4 r5.y, r2, c4[a0.w]
    a[0].w = DWRoundNearest(r[0].y);
    ctmp = DWIndexedConstant(c, 4, a[0].w);
    r[5].y = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // VERIFIED
    // [136] mova a0.w, r0.y
    // [137] dp4 r5.z, r2, c5[a0.w]
    a[0].w = DWRoundNearest(r[0].y);
    ctmp = DWIndexedConstant(c, 5, a[0].w);
    r[5].z = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // VERIFIED
    // [138] mad r0.xyw, r3.y, r5.xyzz, r4.xyzz
    r[0].x = r[3].y * r[5].x + r[4].x;
    r[0].y = r[3].y * r[5].y + r[4].y;
    r[0].w = r[3].y * r[5].z + r[4].z;

    // VERIFIED
    // [139] add r1.w, -r3.x, c197.y
    // [140] add r1.w, -r3.y, r1.w
    r[1].w = -r[3].x + c[CV1].y;
    r[1].w = -r[3].y + r[1].w;

    // VERIFIED
    // [141] mova a0.w, r0.z
    // [142] dp4 r3.x, r2, c6[a0.w]
    a[0].w = DWRoundNearest(r[0].z);
    ctmp = DWIndexedConstant(c, 6, a[0].w);
    r[3].x = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // VERIFIED
    // [143] mova a0.w, r0.z
    // [144] dp4 r3.y, r2, c7[a0.w]
    a[0].w = DWRoundNearest(r[0].z);
    ctmp = DWIndexedConstant(c, 7, a[0].w);
    r[3].y = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // VERIFIED
    // [145] mova a0.w, r0.z
    // [146] dp4 r3.z, r2, c8[a0.w]
    a[0].w = DWRoundNearest(r[0].z);
    ctmp = DWIndexedConstant(c, 8, a[0].w);
    r[3].z = r[2].x*ctmp->x + r[2].y*ctmp->y + r[2].z*ctmp->z + r[2].w*ctmp->w;

    // VERIFIED
    // [147] mad r6.xyz, r1.w, r3, r0.xyww
    r[6].x = r[1].w * r[3].x + r[0].x;
    r[6].y = r[1].w * r[3].y + r[0].y;
    r[6].z = r[1].w * r[3].z + r[0].w;

    // VERIFIED
    // [148] mov r2.xyz, r1
    r[2].x = r[1].x;
    r[2].y = r[1].y;
    r[2].z = r[1].z;
   }
 // [149] endif

 // VERIFIED
 // [150] if b0
 // [151] mov r2.xyz, r6
 // [152] endif
 if(b[0])
   {
    r[2].x = r[6].x;
    r[2].y = r[6].y;
    r[2].z = r[6].z;
   }

 //
 // MODELVIEW TRANSFORMATION
 //

 // VERIFIED
 // [153] dp4 r0.x, r2, c192
 // [154] dp4 r0.y, r2, c193
 // [155] dp4 r0.z, r2, c194
 // [156] dp4 r0.w, r2, c195
 r[0].x = r[2].x*c[MW2P_INDEX + 0].x + r[2].y*c[MW2P_INDEX + 0].y + r[2].z*c[MW2P_INDEX + 0].z + r[2].w*c[MW2P_INDEX + 0].w;
 r[0].y = r[2].x*c[MW2P_INDEX + 1].x + r[2].y*c[MW2P_INDEX + 1].y + r[2].z*c[MW2P_INDEX + 1].z + r[2].w*c[MW2P_INDEX + 1].w;
 r[0].z = r[2].x*c[MW2P_INDEX + 2].x + r[2].y*c[MW2P_INDEX + 2].y + r[2].z*c[MW2P_INDEX + 2].z + r[2].w*c[MW2P_INDEX + 2].w;
 r[0].w = r[2].x*c[MW2P_INDEX + 3].x + r[2].y*c[MW2P_INDEX + 3].y + r[2].z*c[MW2P_INDEX + 3].z + r[2].w*c[MW2P_INDEX + 3].w;

 //
 // ASSIGN OUTPUT REGISTERS
 //

 // VERIFIED
 // [157] mov o0, r0
 // [158] mov o1, r0
 // [159] mov o2.xy, v9
 o0.x = r[0].x;
 o0.y = r[0].y;
 o0.z = r[0].z;
 o0.w = r[0].w;
 o1.x = r[0].x;
 o1.y = r[0].y;
 o1.z = r[0].z;
 o1.w = r[0].w;
 o2.x = v10.x;
 o2.y = v10.y;
 o2.z = 0.0f;
 o2.w = 0.0f;
 o3.x = v11.x;
 o3.y = v11.y;
 o3.z = 0.0f;
 o3.w = 0.0f;
 o4.x = v12.x;
 o4.y = v12.y;
 o4.z = 0.0f;
 o4.w = 0.0f;

 // success
 output.o0.x = o0.x;
 output.o0.y = o0.y;
 output.o0.z = o0.z;
 output.o0.w = o0.w;
 output.o1.x = o1.x;
 output.o1.y = o1.y;
 output.o1.z = o1.z;
 output.o1.w = o1.w;
 output.o2.x = o2.x;
 output.o2.y = o2.y;
 output.o2.z = o2.z;
 output.o2.w = o2.w;
 output.o3.x = o3.x;
 output.o3.y = o3.y;
 output.o3.z = o3.z;
 output.o3.w = o3.w;
 output.o4.x = o4.x;
 output.o4.y = o4.y;
 output.o4.z = o4.z;
 output.o4.w = o4.w;

 return true;
}

bool TransformClothVertex2(const DWCLOTHINPUT& input, DWfloat4* c, size_t cn, const bool* b, size_t bn, DWCLOTHOUTPUT& output)
{
 // validate constant buffers
 if(!c || (cn < 511)) return false;
 if(!b || (bn < 2)) return false;

 // input and output registers
 DWfloat4 v0 = input.v0;   // dcl_position
 DWfloat4 v1 = input.v1;   // dcl_blendweight
 DWfloat4 v2 = input.v2;   // dcl_binormal
 DWfloat4 v3 = input.v3;   // dcl_color1
 DWfloat4 v4 = input.v4;   // dcl_blendindices
 DWfloat4 v5 = input.v5;   // dcl_psize 
 DWfloat4 v6 = input.v6;   // dcl_fog
 DWfloat4 v7 = input.v7;   // dcl_texcoord5
 DWfloat4 v8 = input.v8;   // dcl_normal
 DWfloat4 v9 = input.v9;   // dcl_texcoord0
 DWfloat4 v10 = input.v10; // dcl_texcoord0
 DWfloat4 v11 = input.v11; // dcl_texcoord1
 DWfloat4 v12 = input.v12; // dcl_texcoord2
 DWfloat4 o0; // dcl_position
 DWfloat4 o1; // dcl_position
 DWfloat4 o2; // dcl_texcoord0
 DWfloat4 o3; // dcl_texcoord1
 DWfloat4 o4; // dcl_texcoord2

 // other registers
 DWfloat4 r[32];  // temporary registers
 DWaddress4 a[1]; // address registers

 // temporary values
 real32 temp = 0.0f;
 DWfloat4* ctmp = nullptr;

 // changed to 500 to make room for more control points, used to be 192
 static const int MW2P_INDEX = 500; // used to be 192, 193, 194, 195
 static const int CV0 = 504; // used to be 196
 static const int CV1 = 505; // used to be 197
 static const int CV2 = 506; // used to be 198

 // default values
 c[CV1].x = 0;
 c[CV1].y = 1;
 c[CV1].z = 0;
 c[CV1].w = 0;

 c[CV2].x = -0;
 c[CV2].y = -3;
 c[CV2].z = -6;
 c[CV2].w = -9;

 //
 // APPENDING TO R1
 //

 // [ 0] dp4 r0.x, v5, v5
 // [ 1] slt r0.x, c197.x, r0.x
 // [ 2] mov r0.yzw, v4.xxyz
 // [ 3] mova a0.w, r0.z
 // [ 4] mul r1.xyz, v0.y, c96[a0.w].zxyw
 // [ 5] mova a0.w, r0.y
 // [ 6] mad r1.xyz, v0.x, c96[a0.w].zxyw, r1
 // [ 7] mova a0.w, r0.w
 // [ 8] mad r1.xyz, v0.z, c96[a0.w].zxyw, r1
 // [ 9] mov r1.w, v4.w
 // [10] mova a0.w, r1.w
 // [11] mad r1.xyz, v0.w, c96[a0.w].zxyw, r1
 d3d_dp4(r[0], "x", v5, "", v5, "");                          
 d3d_slt(r[0], "x", c[CV1], "x", r[0], "x");                  
 d3d_mov(r[0], "yzw", v4, "xxyz");                            
 d3d_mova(a[0], "w", r[0], "z");                              
 d3d_mul(r[1], "xyz", v0, "y", DWIC(c, 96, a[0].w), "zxyw");  
 d3d_mova(a[0], "w", r[0], "y");
 d3d_mad(r[1], "xyz", v0, "x", DWIC(c, 96, a[0].w), "zxyw", r[1], "");
 d3d_mova(a[0], "w", r[0], "w");
 d3d_mad(r[1], "xyz", v0, "z", DWIC(c, 96, a[0].w), "zxyw", r[1], "");
 d3d_mov(r[1], "w", v4, "w");
 d3d_mova(a[0], "w", r[1], "w");
 d3d_mad(r[1], "xyz", v0, "w", DWIC(c, 96, a[0].w), "zxyw", r[1], "");

 //
 // APPENDING TO R3
 //

 // [12] mov r2, v5
 // [13] mova a0.w, r2.y
 // [14] mul r3.xyz, v0.y, c96[a0.w].zxyw
 // [15] mova a0.w, r2.x
 // [16] mad r3.xyz, v0.x, c96[a0.w].zxyw, r3
 // [17] mova a0.w, r2.z
 // [18] mad r3.xyz, v0.z, c96[a0.w].zxyw, r3
 // [19] mova a0.w, r2.w
 // [20] mad r3.xyz, v0.w, c96[a0.w].zxyw, r3
 d3d_mov(r[2], "", v5, "");
 d3d_mova(a[0], "w", r[2], "y");
 d3d_mul(r[3], "xyz", v0, "y", DWIC(c, 96, a[0].w), "zxyw");
 d3d_mova(a[0], "w", r[2], "x");
 d3d_mad(r[3], "xyz", v0, "x", DWIC(c, 96, a[0].w), "zxyw", r[3], "");
 d3d_mova(a[0], "w", r[2], "z");
 d3d_mad(r[3], "xyz", v0, "z", DWIC(c, 96, a[0].w), "zxyw", r[3], "");
 d3d_mova(a[0], "w", r[2], "w");
 d3d_mad(r[3], "xyz", v0, "w", DWIC(c, 96, a[0].w), "zxyw", r[3], "");

 //
 // APPENDING TO R4
 //

 // [21] mov r3.w, v6.x
 // [22] mov r4.x, v6.y
 // [23] mova a0.w, r4.x
 // [24] mul r4.yzw, v0.y, c96[a0.w].xzxy
 // [25] mova a0.w, r3.w
 // [26] mad r4.yzw, v0.x, c96[a0.w].xzxy, r4
 // [27] mov r5.xy, v6.zwzw
 // [28] mova a0.w, r5.x
 // [29] mad r4.yzw, v0.z, c96[a0.w].xzxy, r4
 // [30] mova a0.w, r5.y
 // [31] mad r4.yzw, v0.w, c96[a0.w].xzxy, r4
 d3d_mov(r[3], "w", v6, "x");
 d3d_mov(r[4], "x", v6, "y");
 d3d_mova(a[0], "w", r[4], "x");
 d3d_mul(r[4], "yzw", v0, "y", DWIC(c, 96, a[0].w), "xzxy");
 d3d_mova(a[0], "w", r[3], "w");
 d3d_mad(r[4], "yzw", v0, "x", DWIC(c, 96, a[0].w), "xzxy", r[4], "");
 d3d_mov(r[5], "xy", v6, "zwzw");
 d3d_mova(a[0], "w", r[5], "x");
 d3d_mad(r[4], "yzw", v0, "z", DWIC(c, 96, a[0].w), "xzxy", r[4], "");
 d3d_mova(a[0], "w", r[5], "y");
 d3d_mad(r[4], "yzw", v0, "w", DWIC(c, 96, a[0].w), "xzxy", r[4], "");

 //
 // APPENDING TO R6
 //

 // [32] mov r5.zw, v7.xyxy
 // [33] mova a0.w, r5.w
 // [34] mul r6.xyz, v0.y, c96[a0.w].zxyw
 // [35] mova a0.w, r5.z
 // [36] mad r6.xyz, v0.x, c96[a0.w].zxyw, r6
 // [37] mov r6.w, v7.z
 // [38] mova a0.w, r6.w
 // [39] mad r6.xyz, v0.z, c96[a0.w].zxyw, r6
 // [40] mov r7.x, v7.w
 // [41] mova a0.w, r7.x
 // [42] mad r6.xyz, v0.w, c96[a0.w].zxyw, r6
 d3d_mov(r[5], "zw", v7, "xyxy");
 d3d_mova(a[0], "w", r[5], "w");
 d3d_mul(r[6], "xyz", v0, "y", DWIC(c, 96, a[0].w), "zxyw");
 d3d_mova(a[0], "w", r[5], "z");
 d3d_mad(r[6], "xyz", v0, "x", DWIC(c, 96, a[0].w), "zxyw", r[6], "");
 d3d_mov(r[6], "w", v7, "z");
 d3d_mova(a[0], "w", r[6], "w");
 d3d_mad(r[6], "xyz", v0, "z", DWIC(c, 96, a[0].w), "zxyw", r[6], "");
 d3d_mov(r[7], "x", v7, "w");
 d3d_mova(a[0], "w", r[7], "x");
 d3d_mad(r[6], "xyz", v0, "w", DWIC(c, 96, a[0].w), "zxyw", r[6], "");

 //
 // APPENDING TO R7 AND R1
 //

 // [43] mul r7.yzw, r3.xyzx, v1.y
 // [44] mad r7.yzw, v1.x, r1.xyzx, r7
 // [45] mad r7.yzw, v1.z, r4.xzwy, r7
 // [46] mad r7.yzw, v1.w, r6.xyzx, r7
 // [47] mul r3.xyz, r3, v3.y
 // [48] mad r1.xyz, v3.x, r1, r3
 // [49] mad r1.xyz, v3.z, r4.yzww, r1
 // [50] mad r1.xyz, v3.w, r6, r1
 d3d_mul(r[7], "yzw", r[3], "xyzx", v1, "y");
 d3d_mad(r[7], "yzw", v1, "x", r[1], "xyzx", r[7], "");
 d3d_mad(r[7], "yzw", v1, "z", r[4], "xzwy", r[7], "");
 d3d_mad(r[7], "yzw", v1, "w", r[6], "xyzx", r[7], "");
 d3d_mul(r[3], "xyz", r[3], "", v3, "y");
 d3d_mad(r[1], "xyz", v3, "x", r[1], "", r[3], "");
 d3d_mad(r[1], "xyz", v3, "z", r[4], "yzww", r[1], "");
 d3d_mad(r[1], "xyz", v3, "w", r[6], "", r[1], "");

 //
 // APPENDING TO R3 AND R0
 //

 // [51] mova a0.w, r0.z
 // [52] mul r3.xyz, v2.y, c96[a0.w].yzxw
 // [53] mova a0.w, r0.y
 // [54] mad r3.xyz, v2.x, c96[a0.w].yzxw, r3
 // [55] mova a0.w, r0.w
 // [56] mad r0.yzw, v2.z, c96[a0.w].xyzx, r3.xxyz
 // [57] mova a0.w, r1.w
 // [58] mad r0.yzw, v2.w, c96[a0.w].xyzx, r0
 d3d_mova(a[0], "w", r[0], "z");
 d3d_mul(r[3], "xyz", v2, "y", DWIC(c, 96, a[0].w), "yzxw");
 d3d_mova(a[0], "w", r[0], "y");
 d3d_mad(r[3], "xyz", v2, "x", DWIC(c, 96, a[0].w), "yzxw", r[3], "");
 d3d_mova(a[0], "w", r[0], "w");
 d3d_mad(r[0], "yzw", v2, "z", DWIC(c, 96, a[0].w), "xyzx", r[3], "xxyz");
 d3d_mova(a[0], "w", r[1], "w");
 d3d_mad(r[0], "yzw", v2, "w", DWIC(c, 96, a[0].w), "xyzx", r[0], "");

 //
 // APPENDING TO R3 AND R2
 //

 // [59] mova a0.w, r2.y
 // [60] mul r3.xyz, v2.y, c96[a0.w].yzxw
 // [61] mova a0.w, r2.x
 // [62] mad r3.xyz, v2.x, c96[a0.w].yzxw, r3
 // [63] mova a0.w, r2.z
 // [64] mad r2.xyz, v2.z, c96[a0.w].yzxw, r3
 // [65] mova a0.w, r2.w
 // [66] mad r2.xyz, v2.w, c96[a0.w].yzxw, r2
 d3d_mova(a[0], "w", r[2], "y");
 d3d_mul(r[3], "xyz", v2, "y", DWIC(c, 96, a[0].w), "yzxw");
 d3d_mova(a[0], "w", r[2], "x");
 d3d_mad(r[3], "xyz", v2, "x", DWIC(c, 96, a[0].w), "yzxw", r[3], "");
 d3d_mova(a[0], "w", r[2], "z");
 d3d_mad(r[2], "xyz", v2, "z", DWIC(c, 96, a[0].w), "yzxw", r[3], "");
 d3d_mova(a[0], "w", r[2], "w");
 d3d_mad(r[2], "xyz", v2, "w", DWIC(c, 96, a[0].w), "yzxw", r[2], "");

 //
 // APPENDING TO R3
 //

 // [67] mova a0.w, r4.x
 // [68] mul r3.xyz, v2.y, c96[a0.w].yzxw
 // [69] mova a0.w, r3.w
 // [70] mad r3.xyz, v2.x, c96[a0.w].yzxw, r3
 // [71] mova a0.w, r5.x
 // [72] mad r3.xyz, v2.z, c96[a0.w].yzxw, r3
 // [73] mova a0.w, r5.y
 // [74] mad r3.xyz, v2.w, c96[a0.w].yzxw, r3
 d3d_mova(a[0], "w", r[4], "x");
 d3d_mul(r[3], "xyz", v2, "y", DWIC(c, 96, a[0].w), "yzxw");
 d3d_mova(a[0], "w", r[3], "w");
 d3d_mad(r[3], "xyz", v2, "x", DWIC(c, 96, a[0].w), "yzxw", r[3], "");
 d3d_mova(a[0], "w", r[5], "x");
 d3d_mad(r[3], "xyz", v2, "z", DWIC(c, 96, a[0].w), "yzxw", r[3], "");
 d3d_mova(a[0], "w", r[5], "y");
 d3d_mad(r[3], "xyz", v2, "w", DWIC(c, 96, a[0].w), "yzxw", r[3], "");

 //
 // APPENDING TO R4
 //

 // [75] mova a0.w, r5.w
 // [76] mul r4.xyz, v2.y, c96[a0.w].yzxw
 // [77] mova a0.w, r5.z
 // [78] mad r4.xyz, v2.x, c96[a0.w].yzxw, r4
 // [79] mova a0.w, r6.w
 // [80] mad r4.xyz, v2.z, c96[a0.w].yzxw, r4
 // [81] mova a0.w, r7.x
 // [82] mad r4.xyz, v2.w, c96[a0.w].yzxw, r4
 d3d_mova(a[0], "w", r[5], "w");
 d3d_mul(r[4], "xyz", v2, "y", DWIC(c, 96, a[0].w), "yzxw");
 d3d_mova(a[0], "w", r[5], "z");
 d3d_mad(r[4], "xyz", v2, "x", DWIC(c, 96, a[0].w), "yzxw", r[4], "");
 d3d_mova(a[0], "w", r[6], "w");
 d3d_mad(r[4], "xyz", v2, "z", DWIC(c, 96, a[0].w), "yzxw", r[4], "");
 d3d_mova(a[0], "w", r[7], "x");
 d3d_mad(r[4], "xyz", v2, "w", DWIC(c, 96, a[0].w), "yzxw", r[4], "");

 //
 // APPENDING TO R0, R1
 //

 // [83] mul r2.xyz, r2, v1.y
 // [84] mad r0.yzw, v1.x, r0, r2.xxyz
 // [85] mad r0.yzw, v1.z, r3.xxyz, r0
 // [86] mad r0.yzw, v1.w, r4.xxyz, r0
 // [87] mul r2.xyz, r0.yzww, r1
 // [88] mad r0.yzw, r1.xzxy, r0.xzwy, -r2.xxyz
 // [89] mad r1.xyz, v8.w, r0.yzww, r7.yzww
 d3d_mul(r[2], "xyz", r[2], "", v1, "y");
 d3d_mad(r[0], "yzw", v1, "x", r[0], "", r[2], "xxyz");
 d3d_mad(r[0], "yzw", v1, "z", r[3], "xxyz", r[0], "");
 d3d_mad(r[0], "yzw", v1, "w", r[4], "xxyz", r[0], "");
 d3d_mul(r[2], "xyz", r[0], "yzww", r[1], "");
 d3d_mad(r[0], "yzw", r[1], "xzxy", r[0], "xzwy", -r[2], "xxyz");
 d3d_mad(r[1], "xyz", v8, "w", r[0], "yzww", r[7], "yzww");

 //
 // SETUP
 //

 // [ 90] mov r1.w, c197.y
 // [ 91] lrp r2, r0.x, r1, v0
 // [ 92] mad r1, r0.x, -v4, v4
 // [ 93] lrp r3.xyz, r0.x, c197.yxxw, v1
 // [ 94] mov r0.x, c197.x
 // [ 95] slt r0.x, r0.x, c196.x
 // [ 96] add r1, r1, c198
 // [ 97] mul r0, r0.x, r1
 // [ 98] mova a0.w, r0.x
 // [ 99] dp4 r1.x, r2, c0[a0.w]
 // [100] mova a0.w, r0.x
 // [101] dp4 r1.y, r2, c1[a0.w]
 // [102] mova a0.w, r0.x
 // [103] dp4 r1.z, r2, c2[a0.w]
 // [104] mul r4.xyz, r3.x, r1
 d3d_mov(r[1], "w", c[CV1], "y");
 d3d_lrp(r[2], "", r[0], "x", r[1], "", v0, "");
 d3d_mad(r[1], "", r[0], "x", -v4, "", v4, "");
 d3d_lrp(r[3], "xyz", r[0], "x", c[CV1], "yxxw", v1, "");
 d3d_mov(r[0], "x", c[CV1], "x");
 d3d_slt(r[0], "x", r[0], "x", c[CV0], "x");
 d3d_add(r[1], "", r[1], "", c[CV2], "");
 d3d_mul(r[0], "", r[0], "x", r[1], "");
 d3d_mova(a[0], "w", r[0], "x");
 d3d_dp4(r[1], "x", r[2], "", DWIC(c, 0, a[0].w), "");
 d3d_mova(a[0], "w", r[0], "x");
 d3d_dp4(r[1], "y", r[2], "", DWIC(c, 1, a[0].w), "");
 d3d_mova(a[0], "w", r[0], "x");
 d3d_dp4(r[1], "z", r[2], "", DWIC(c, 2, a[0].w), "");
 d3d_mul(r[4], "xyz", r[3], "x", r[1], "");

 // [105] if b1
 if(b[1])
   {
    // [106] mova a0.w, r0.y
    // [107] dp4 r5.x, r2, c3[a0.w]
    // [108] mova a0.w, r0.y
    // [109] dp4 r5.y, r2, c4[a0.w]
    // [110] mova a0.w, r0.y
    // [111] dp4 r5.z, r2, c5[a0.w]
    // [112] mad r6.xyz, r3.y, r5, r4
    d3d_mova(a[0], "w", r[0], "y");
    d3d_dp4(r[5], "x", r[2], "", DWIC(c, 3, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "y");
    d3d_dp4(r[5], "y", r[2], "", DWIC(c, 4, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "y");
    d3d_dp4(r[5], "z", r[2], "", DWIC(c, 5, a[0].w), "");
    d3d_mad(r[6], "xyz", r[3], "y", r[5], "", r[4], "");

    // [113] mova a0.w, r0.z
    // [114] dp4 r7.x, r2, c6[a0.w]
    // [115] mova a0.w, r0.z
    // [116] dp4 r7.y, r2, c7[a0.w]
    // [117] mova a0.w, r0.z
    // [118] dp4 r7.z, r2, c8[a0.w]
    // [119] mad r6.xyz, r3.z, r7, r6
    d3d_mova(a[0], "w", r[0], "z");
    d3d_dp4(r[7], "x", r[2], "", DWIC(c, 6, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "z");
    d3d_dp4(r[7], "y", r[2], "", DWIC(c, 7, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "z");
    d3d_dp4(r[7], "z", r[2], "", DWIC(c, 8, a[0].w), "");
    d3d_mad(r[6], "xyz", r[3], "z", r[7], "", r[6], "");

    // [120] add r0.x, -r3.x, c197.y
    // [121] add r0.x, -r3.y, r0.x
    // [122] add r0.x, -r3.z, r0.x
    d3d_add(r[0], "x", -r[3], "x", c[CV1], "y");
    d3d_add(r[0], "x", -r[3], "y", r[0], "x");
    d3d_add(r[0], "x", -r[3], "z", r[0], "x");

    // [123] mova a0.w, r0.w
    // [124] dp4 r7.x, r2, c9[a0.w]
    // [125] mova a0.w, r0.w
    // [126] dp4 r7.y, r2, c10[a0.w]
    // [127] mova a0.w, r0.w
    // [128] dp4 r7.z, r2, c11[a0.w]
    // [129] mad r6.xyz, r0.x, r7, r6
    // [130] lrp r2.xyz, r3.x, r1, r5
    d3d_mova(a[0], "w", r[0], "w");
    d3d_dp4(r[7], "x", r[2], "", DWIC(c, 9, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "w");
    d3d_dp4(r[7], "y", r[2], "", DWIC(c, 10, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "w");
    d3d_dp4(r[7], "z", r[2], "", DWIC(c, 11, a[0].w), "");
    d3d_mad(r[6], "xyz", r[0], "x", r[7], "", r[6], "");
    d3d_lrp(r[2], "xyz", r[3], "x", r[1], "", r[5], "");
   }
 // [131] else
 else
   {
    // [132] mova a0.w, r0.y
    // [133] dp4 r5.x, r2, c3[a0.w]
    // [134] mova a0.w, r0.y
    // [135] dp4 r5.y, r2, c4[a0.w]
    // [136] mova a0.w, r0.y
    // [137] dp4 r5.z, r2, c5[a0.w]
    // [138] mad r0.xyw, r3.y, r5.xyzz, r4.xyzz
    // [139] add r1.w, -r3.x, c197.y
    // [140] add r1.w, -r3.y, r1.w
    // [141] mova a0.w, r0.z
    // [142] dp4 r3.x, r2, c6[a0.w]
    // [143] mova a0.w, r0.z
    // [144] dp4 r3.y, r2, c7[a0.w]
    // [145] mova a0.w, r0.z
    // [146] dp4 r3.z, r2, c8[a0.w]
    // [147] mad r6.xyz, r1.w, r3, r0.xyww
    // [148] mov r2.xyz, r1
    d3d_mova(a[0], "w", r[0], "y");
    d3d_dp4(r[5], "x", r[2], "", DWIC(c, 3, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "y");
    d3d_dp4(r[5], "y", r[2], "", DWIC(c, 4, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "y");
    d3d_dp4(r[5], "z", r[2], "", DWIC(c, 5, a[0].w), "");
    d3d_mad(r[0], "xyw", r[3], "y", r[5], "xyzz", r[4], "xyzz");
    d3d_add(r[1], "w", -r[3], "x", c[CV1], "y");
    d3d_add(r[1], "w", -r[3], "y", r[1], "w");
    d3d_mova(a[0], "w", r[0], "z");
    d3d_dp4(r[3], "x", r[2], "", DWIC(c, 6, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "z");
    d3d_dp4(r[3], "y", r[2], "", DWIC(c, 7, a[0].w), "");
    d3d_mova(a[0], "w", r[0], "z");
    d3d_dp4(r[3], "z", r[2], "", DWIC(c, 8, a[0].w), "");
    d3d_mad(r[6], "xyz", r[1], "w", r[3], "", r[0], "xyww");
    d3d_mov(r[2], "xyz", r[1], "");
   }
 // [149] endif

 // [150] if b0
 // [151] mov r2.xyz, r6
 // [152] endif
 if(b[0]) d3d_mov(r[2], "xyz", r[6], "");

 //
 // MODELVIEW TRANSFORMATION
 //

 // [153] dp4 r0.x, r2, c192
 // [154] dp4 r0.y, r2, c193
 // [155] dp4 r0.z, r2, c194
 // [156] dp4 r0.w, r2, c195
 d3d_dp4(r[0], "x", r[2], "", c[MW2P_INDEX + 0], "");
 d3d_dp4(r[0], "y", r[2], "", c[MW2P_INDEX + 1], "");
 d3d_dp4(r[0], "z", r[2], "", c[MW2P_INDEX + 2], "");
 d3d_dp4(r[0], "w", r[2], "", c[MW2P_INDEX + 3], "");

 //
 // ASSIGN OUTPUT REGISTERS
 //

 // VERIFIED
 // [157] mov o0, r0
 // [158] mov o1, r0
 // [159] mov o2.xy, v9
 o0.x = r[0].x;
 o0.y = r[0].y;
 o0.z = r[0].z;
 o0.w = r[0].w;
 o1.x = r[0].x;
 o1.y = r[0].y;
 o1.z = r[0].z;
 o1.w = r[0].w;
 o2.x = v10.x;
 o2.y = v10.y;
 o2.z = 0.0f;
 o2.w = 0.0f;
 o3.x = v11.x;
 o3.y = v11.y;
 o3.z = 0.0f;
 o3.w = 0.0f;
 o4.x = v12.x;
 o4.y = v12.y;
 o4.z = 0.0f;
 o4.w = 0.0f;

 // success
 output.o0.x = o0.x;
 output.o0.y = o0.y;
 output.o0.z = o0.z;
 output.o0.w = o0.w;
 output.o1.x = o1.x;
 output.o1.y = o1.y;
 output.o1.z = o1.z;
 output.o1.w = o1.w;
 output.o2.x = o2.x;
 output.o2.y = o2.y;
 output.o2.z = o2.z;
 output.o2.w = o2.w;
 output.o3.x = o3.x;
 output.o3.y = o3.y;
 output.o3.z = o3.z;
 output.o3.w = o3.w;
 output.o4.x = o4.x;
 output.o4.y = o4.y;
 output.o4.z = o4.z;
 output.o4.w = o4.w;

 return true;
}

void print(const DWCLOTHINPUT& input)
{
 cout << "v0 = <" << input.v0.x << ", " << input.v0.y << ", " << input.v0.z << ", " << input.v0.w << ">" << endl;
 cout << "v1 = <" << input.v1.x << ", " << input.v1.y << ", " << input.v1.z << ", " << input.v1.w << ">" << endl;
 cout << "v2 = <" << input.v2.x << ", " << input.v2.y << ", " << input.v2.z << ", " << input.v2.w << ">" << endl;
 cout << "v3 = <" << input.v3.x << ", " << input.v3.y << ", " << input.v3.z << ", " << input.v3.w << ">" << endl;
 cout << "v4 = <" << input.v4.x << ", " << input.v4.y << ", " << input.v4.z << ", " << input.v4.w << ">" << endl;
 cout << "v5 = <" << input.v5.x << ", " << input.v5.y << ", " << input.v5.z << ", " << input.v5.w << ">" << endl;
 cout << "v6 = <" << input.v6.x << ", " << input.v6.y << ", " << input.v6.z << ", " << input.v6.w << ">" << endl;
 cout << "v7 = <" << input.v7.x << ", " << input.v7.y << ", " << input.v7.z << ", " << input.v7.w << ">" << endl;
 cout << "v8 = <" << input.v8.x << ", " << input.v8.y << ", " << input.v8.z << ", " << input.v8.w << ">" << endl;
 cout << "v9 = <" << input.v9.x << ", " << input.v9.y << ", " << input.v9.z << ", " << input.v9.w << ">" << endl;
 cout << "v10 = <" << input.v10.x << ", " << input.v10.y << ", " << input.v10.z << ", " << input.v10.w << ">" << endl;
 cout << "v11 = <" << input.v11.x << ", " << input.v11.y << ", " << input.v11.z << ", " << input.v11.w << ">" << endl;
 cout << "v12 = <" << input.v12.x << ", " << input.v12.y << ", " << input.v12.z << ", " << input.v12.w << ">" << endl;
 cout << endl;
}

void print(const DWCLOTHOUTPUT& item)
{
 cout << "o0 = <" << item.o0.x << ", " << item.o0.y << ", " << item.o0.z << ", " << item.o0.w << ">" << endl;
 cout << "o1 = <" << item.o1.x << ", " << item.o1.y << ", " << item.o1.z << ", " << item.o1.w << ">" << endl;
 cout << "o2 = <" << item.o2.x << ", " << item.o2.y << ", " << item.o2.z << ", " << item.o2.w << ">" << endl;
 cout << "o3 = <" << item.o3.x << ", " << item.o3.y << ", " << item.o3.z << ", " << item.o3.w << ">" << endl;
 cout << "o4 = <" << item.o4.x << ", " << item.o4.y << ", " << item.o4.z << ", " << item.o4.w << ">" << endl;
 cout << endl;
}

void print(const DWfloat4* data, size_t n)
{
 cout << "buffer = " << endl;
 for(size_t i = 0; i < n; i++) cout << "<" << data[i].x << ", " << data[i].y << ", " << data[i].z << ", " << data[i].w << ">" << endl;
 cout << endl;
}

#pragma endregion

#pragma region REGION_G1MM_STRUCTURES

struct G1MMSECTION {
 uint32 size;
 boost::shared_array<boost::shared_array<real32>> data;
};

#pragma endregion

#pragma region REGION_G1MG_STRUCTURES

struct G1MGSECTION {
 uint32 type;
 uint32 size;
 boost::shared_array<char> data;
 G1MGSECTION() : type(0), size(0) {}
};

struct G1MG0102TEXTURE {
 uint16 p01; // texture identifier
 uint16 p02a; // texture type
 uint16 p02b; // texture type
 uint16 p03; // ????
 uint16 p04; // 0x0004
 uint16 p05; // 0x0004
};

struct G1MG0102ITEM {
 uint32 p01; // ????
 uint32 p02; // number of textures
 uint32 p03; // ????
 uint32 p04; // ????
 boost::shared_array<G1MG0102TEXTURE> p05;
};

struct G1MG0102DATA {
 uint32 elem;
 boost::shared_array<G1MG0102ITEM> data;
};

struct G1MG0103ATTR {
 uint32 p01;
 uint32 p02;
 uint32 p03;
 uint16 p04;
 uint16 p05;
 char p06[1024];
};

struct G1MG0103ITEM {
 uint32 p01;
 boost::shared_array<G1MG0103ATTR> p02;
};

struct G1MG0103DATA {
 uint32 elem;
 boost::shared_array<G1MG0103ITEM> data;
};

struct G1MG0104ITEM {
 uint32 p01;
 uint32 p02;
 uint32 p03;
 uint32 p04;
 boost::shared_array<char> p05;
};

struct G1MG0104DATA {
 uint32 elem;
 boost::shared_array<G1MG0104ITEM> data;
};

struct G1MG0105SEMANTIC {
 uint16 p01;
 uint16 p02;
 uint16 p03;
 uint16 p04;
};

struct G1MG0105ITEM {
 uint32 n_refs;
 boost::shared_array<uint32> refs;
 uint32 n_semantics;
 boost::shared_array<G1MG0105SEMANTIC> semantics;
 bool is_cloth;
};

struct G1MG0105DATA {
 uint32 elem;
 boost::shared_array<G1MG0105ITEM> data;
};

struct G1MG0106ITEMENTRY {
 uint32 p01;
 uint16 p02;
 uint16 p03;
 uint16 p04;
 uint16 p05;
};

struct G1MG0106ITEM {
 uint32 elem;
 boost::shared_array<G1MG0106ITEMENTRY> data;
};

struct G1MG0106DATA {
 uint32 elem;
 boost::shared_array<G1MG0106ITEM> data;
};

struct G1MG0107ITEM {
 uint32 elem;
 uint32 type;
 uint32 unknown;
 boost::shared_array<char> data;
};

struct G1MG0107DATA {
 uint32 elem;
 boost::shared_array<G1MG0107ITEM> data;
};

struct G1MG0108DATA {
 uint32 elem;
 boost::shared_array<boost::shared_array<uint32>> data;
};

#pragma endregion

#pragma region REGION_NUNO_STRUCTURES

// control points
struct NUNO0301SUBENTRY1 {
 real32 p01;
 real32 p02;
 real32 p03;
 real32 p04;
};

// influence data
struct NUNOSUBCHUNK0301ENTRY_DATATYPE1 {
 uint32 p01;
 uint32 p02;
 uint32 p03;
 uint32 p04;
 real32 p05;
 real32 p06;
};

struct NUNOSUBCHUNK0301ENTRY_DATATYPE2 {
 real32 p01[4];
 real32 p02[4];
 uint32 p03;
 uint32 p04;
 uint32 p05;
 uint32 p06;
};

struct NUNOSUBCHUNK0301ENTRY {
 uint32 h01;
 uint32 h02; // number of control points
 uint32 h03;
 uint32 h04;
 uint32 h05; // input layout index
 uint32 h06;
 real32 h07;
 real32 h08;
 uint32 h09;
 real32 h10[4];
 real32 h11[4];
 real32 h12;
 uint32 h13;
 real32 h14[3]; // unsure
 real32 h15[3]; // unsure
 real32 h16[4]; // unsure (only for NUNO type 0x30303235 and on)
 boost::shared_array<NUNO0301SUBENTRY1> p01;
 boost::shared_array<NUNOSUBCHUNK0301ENTRY_DATATYPE1> p02;
 boost::shared_array<NUNOSUBCHUNK0301ENTRY_DATATYPE2> p03;
 boost::shared_array<uint32> p04;
 boost::shared_array<uint32> p05;
 boost::shared_array<uint32> p06;
};

struct NUNOSUBCHUNK0302ENTRY {
};

struct NUNODATA {
 std::deque<NUNOSUBCHUNK0301ENTRY> _0301;
 std::deque<NUNOSUBCHUNK0302ENTRY> _0302;
};

//
// NUNV
//

// control points
struct NUNV0501SUBENTRY1 {
 real32 p01;
 real32 p02;
 real32 p03;
 real32 p04;
};

// influence data
struct NUNV0501SUBENTRY2 {
 uint32 p01; // index
 uint32 p02; // index
 uint32 p03; // index
 uint32 p04; // index
 real32 p05; // unknown
 real32 p06; // unknown
};

// unknown data
struct NUNV0501SUBENTRY3 {
 real32 p01[4];
 real32 p02[4];
 uint32 p03;
 uint32 p04;
 uint32 p05;
 uint32 p06;
};

struct NUNV0501ENTRY {
 uint32 h01;    // 00 00 00 09
 uint32 h02;    // 00 00 00 10 (number of control points)
 uint32 h03;    // 00 00 00 04
 uint32 h04;    // 00 00 00 03
 uint32 h05;    // 00 00 00 04 (input layout index)
 real32 h06;    // 40 00 00 00
 real32 h07[3]; // 3F 00 00 00 3D CC CC CD 3F 33 33 33
 real32 h08[3]; // 3F 00 00 00 3F 80 00 00 3D CC CC CD
 real32 h09[3]; // BF C9 0F DB 3F 32 B8 C2 3F 80 00 00
 real32 h10[3]; // BF 32 B8 C2 3F 32 B8 C2 3F 00 00 00
 real32 h11[3]; // 3F 80 00 00 42 70 00 00 3F 80 00 00
 real32 h12[3]; // 3C 88 89 3B 00 00 00 00 3F 80 00 00
 uint32 h13;    // 00 01 01 00
 boost::shared_array<NUNV0501SUBENTRY1> p01; // h02 * 0x10 bytes
 boost::shared_array<NUNV0501SUBENTRY2> p02; // h02 * 0x18 bytes
 boost::shared_array<NUNV0501SUBENTRY3> p03; // h03 * 0x30 bytes
 boost::shared_array<uint32> p04; // h04 * 0x04 bytes
};

struct NUNV0502ENTRY {
};

struct NUNVDATA {
 std::deque<NUNV0501ENTRY> _0501;
 std::deque<NUNV0502ENTRY> _0502;
};

#pragma endregion

#pragma region REGION_MODEL_STRUCTURE

struct MODELDATA {
 public :
  STDSTRING filename;
  STDSTRING pathname;
  STDSTRING shrtname;
  std::ofstream dfile;
  endian_mode mode;
 public :
  std::deque<binary_stream> G1MF_list;
  std::deque<binary_stream> G1MS_list;
  std::deque<binary_stream> G1MM_list;
  std::deque<binary_stream> G1MG_list;
  std::deque<binary_stream> NUNO_list;
  std::deque<binary_stream> NUNV_list;
  std::deque<AMC_SKELETON2> SKEL_list;
 public :
  NUNODATA nuno;
  NUNVDATA nunv;
 public :
  G1MGSECTION section01;
  G1MGSECTION section02;
  G1MGSECTION section03;
  G1MGSECTION section04;
  G1MGSECTION section05;
  G1MGSECTION section06;
  G1MGSECTION section07;
  G1MGSECTION section08;
  G1MGSECTION section09;
 public :
  G1MG0102DATA data0102;
  G1MG0103DATA data0103;
  G1MG0104DATA data0104;
  G1MG0105DATA data0105;
  G1MG0106DATA data0106;
  G1MG0107DATA data0107;
  G1MG0108DATA data0108;
 public :
  struct DWClothControlPoints {
   uint32 size;
   boost::shared_array<DWfloat4> data;
  };
  std::map<uint32, DWClothControlPoints> clothmap;
  bool Preprocess(void);
  bool IsCloth(uint32 layout)const;
  bool GetControlPoints(uint32 layout, boost::shared_array<DWfloat4>& data, uint32& size)const;
};

#pragma endregion

#pragma region REGION_MODEL_FUNCTIONS

// #define DEBUG_ME
bool MODELDATA::IsCloth(uint32 layout)const
{
 #ifdef DEBUG_ME

 bool ptr[] = { 
   true,  true,  true,  true,  true,  true, false, false, false, false, 
  false, false, false, false, false, false, false, false, false, false, 
 };
 return ptr[layout];

 #else

 return (clothmap.find(layout) != clothmap.end());

 #endif
}

bool MODELDATA::GetControlPoints(uint32 layout, boost::shared_array<DWfloat4>& data, uint32& size)const
{
 #ifdef DEBUG_ME

 if(!IsCloth(layout)) return false;

 // NUNO (1) or NUNV (2)
 int location[] = {
  1, 2, 2, 2, 2, 2, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 };
 int index[] = {
   0,  0,  1,  2,  3,  4, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
 };

 if(location[layout] == 1)
   {
    uint32 n_cp = nuno._0301[index[layout]].h02;
    if(!n_cp) return true;
    boost::shared_array<DWfloat4> temp(new DWfloat4[n_cp]);
    for(uint32 i = 0; i < n_cp; i++) {
        temp[i].x = nuno._0301[index[layout]].p01[i].p01;
        temp[i].y = nuno._0301[index[layout]].p01[i].p02;
        temp[i].z = nuno._0301[index[layout]].p01[i].p03;
        temp[i].w = nuno._0301[index[layout]].p01[i].p04;
       }
    data = temp;
    size = n_cp;
    return true;
   }
 if(location[layout] == 2)
   {
    uint32 n_cp = nunv._0501[index[layout]].h02;
    if(!n_cp) return true;
    boost::shared_array<DWfloat4> temp(new DWfloat4[n_cp]);
    for(uint32 i = 0; i < n_cp; i++) {
        temp[i].x = nunv._0501[index[layout]].p01[i].p01;
        temp[i].y = nunv._0501[index[layout]].p01[i].p02;
        temp[i].z = nunv._0501[index[layout]].p01[i].p03;
        temp[i].w = nunv._0501[index[layout]].p01[i].p04;
       }
    data = temp;
    size = n_cp;
    return true;
   }
 return false;

 #else

 // find cloth
 auto iter = clothmap.find(layout);
 if(iter == clothmap.end()) return false;
 data = iter->second.data;
 size = iter->second.size;
 return true;

 #endif
}

/**
 * bool Preprocess(void)
 * This function is called after all of the chunks have been processed but before any model data is
 * put together.
 * 1.) Scan through each NUNO and NUNV item, and look at the joint map index parameter (jmi, h09 or h05).
 * 2.) Most of the time, (jmi - 1) is used by an input layout (scan surface info). This input layout is cloth.
 * 3.) Sometimes (jmi - 1) isn't used by an input layout, so use (jmi) instead. This input layout is anchorless cloth.
 */
bool MODELDATA::Preprocess(void)
{
 // p04     p03
 // 0x100                 v1 weights
 // 0x700                 v2 weights
 // 0xA01                 v3 weights
 // 0x600                 v9 weights
 // 0x200 -> 0x500, 0x700 v4 indices
 // 0x400 -> 0x500, 0x700 v5 indices
 // 0xB00 -> 0x500, 0x700 v6 indices
 // 0x505 -> 0x500, 0x700 v7 indices

 // get input layout data
 const G1MG0105DATA& il_data = data0105;
 if(!il_data.elem) return true; // sometimes there is only a skeleton in this file

 // get joint map data
 const G1MG0106DATA& jm_data = data0106;
 if(!jm_data.elem) return true; // sometimes there is only a skeleton in this file

 // get surface data
 const G1MG0108DATA& sdata = data0108;
 if(!sdata.elem) return true; // sometimes there is only a skeleton in this file

 // scan input layout
 for(size_t i = 0; i < il_data.elem; i++)
    {
     // scan semantics
     size_t n_bw = 0;
     size_t n_bi = 0;
     for(size_t j = 0; j < il_data.data[i].n_semantics; j++) {
         // data type and semantic
         uint16 dt = il_data.data[i].semantics[j].p03;
         uint16 bs = il_data.data[i].semantics[j].p04;
         // blendweights
         if(bs == 0x100) n_bw++;
         else if(bs == 0x600) n_bw++;
         else if(bs == 0x700) n_bw++;
         else if(bs == 0xA01) n_bw++;
         // blendindices
         else if(bs == 0x200) { if(dt == 0x500 || dt == 0x700) n_bi++; }
         else if(bs == 0x400) { if(dt == 0x500 || dt == 0x700) n_bi++; }
         else if(bs == 0x505) { if(dt == 0x500 || dt == 0x700) n_bi++; }
         else if(bs == 0xB00) { if(dt == 0x500 || dt == 0x700) n_bi++; }
        }
     // input layout is cloth
     if(n_bw == 4 && n_bi == 4) {
        // cout << "input layout 0x" << hex << i << dec << " is cloth." << endl;
        il_data.data[i].is_cloth = true;
       }
     else
        il_data.data[i].is_cloth = false;
    }

 //
 // PHASE 2
 // Once we know what input layouts are cloth, we can now search through our NUNOs and NUNVs
 // and try to associate NUNOs/NUNV items with input layouts. This is done through the joint
 // maps for some odd reason.
 //

 // search NUNO 1st
 for(size_t i = 0; i < nuno._0301.size(); i++)
    {
     // h01 should match with jmap[jmi].p02/p03
     uint32 jmi = nuno._0301[i].h09;
     uint32 p02 = jm_data.data[jmi].data[0].p02;
     uint32 p03 = jm_data.data[jmi].data[0].p03;
     uint32 jmi_ref = ((p02 << 16) | p03);
     if(nuno._0301[i].h01 != jmi_ref) continue;
     
     // find jmi in surface information
     bool found = false;
     for(size_t j = 0; j < sdata.elem; j++) {
         uint32 il_index = sdata.data[j][0x01];
         uint32 jm_index = sdata.data[j][0x02];
         if((jm_index == jmi) && il_data.data[il_index].is_cloth) {
            uint32 n_cp = nuno._0301[i].h02; // number of control points
            if(!n_cp) return true;
            boost::shared_array<DWfloat4> temp(new DWfloat4[n_cp]);
            for(uint32 k = 0; k < n_cp; k++) {
                temp[k].x = nuno._0301[i].p01[k].p01;
                temp[k].y = nuno._0301[i].p01[k].p02;
                temp[k].z = nuno._0301[i].p01[k].p03;
                temp[k].w = nuno._0301[i].p01[k].p04;
               }
            DWClothControlPoints ccp;
            ccp.data = temp;
            ccp.size = n_cp;
            clothmap.insert(std::map<uint32, DWClothControlPoints>::value_type(il_index, ccp));
            found = true;
           }
        }

     // if NUNO joint map index is not found, we have a problem, use jmi - 1
     if(!found && (nuno._0301[i].h09 > 0))
       {
        // h01 should match with jmap[jmi].p02/p03
        jmi = nuno._0301[i].h09 - 1;
        p02 = jm_data.data[jmi].data[0].p02;
        p03 = jm_data.data[jmi].data[0].p03;
        jmi_ref = ((p02 << 16) | p03);
        if(nuno._0301[i].h01 != jmi_ref) continue;

        // find jmi in surface information
        for(size_t j = 0; j < sdata.elem; j++) {
            uint32 il_index = sdata.data[j][0x01];
            uint32 jm_index = sdata.data[j][0x02];
            if((jm_index == jmi) && il_data.data[il_index].is_cloth) {
               uint32 n_cp = nuno._0301[i].h02;
               if(!n_cp) return true;
               boost::shared_array<DWfloat4> temp(new DWfloat4[n_cp]);
               for(uint32 k = 0; k < n_cp; k++) {
                   temp[k].x = nuno._0301[i].p01[k].p01;
                   temp[k].y = nuno._0301[i].p01[k].p02;
                   temp[k].z = nuno._0301[i].p01[k].p03;
                   temp[k].w = nuno._0301[i].p01[k].p04;
                  }
               DWClothControlPoints ccp;
               ccp.data = temp;
               ccp.size = n_cp;
               clothmap.insert(std::map<uint32, DWClothControlPoints>::value_type(il_index, ccp));
               found = true;
              }
           }
        if(!found) {
           std::stringstream ss;
           ss << "Could not find cloth mesh from NUNO joint map reference 0x" << hex << nuno._0301[i].h09 << dec << "." << endl;
           return error(ss.str());
          }
       }
    }

 // search NUNV 2nd
 for(size_t i = 0; i < nunv._0501.size(); i++)
    {
     // h01 should match with jmap[jmi].p02/p03
     uint32 jmi = nunv._0501[i].h05;
     uint32 p02 = jm_data.data[jmi].data[0].p02;
     uint32 p03 = jm_data.data[jmi].data[0].p03;
     uint32 jmi_ref = ((p02 << 16) | p03);
     if(nunv._0501[i].h01 != jmi_ref) continue;
     
     // find jmi in surface information
     bool found = false;
     for(size_t j = 0; j < sdata.elem; j++) {
         uint32 il_index = sdata.data[j][0x01];
         uint32 jm_index = sdata.data[j][0x02];
         if((jm_index == jmi) && il_data.data[il_index].is_cloth) {
            uint32 n_cp = nunv._0501[i].h02; // number of control points
            if(!n_cp) return true;
            boost::shared_array<DWfloat4> temp(new DWfloat4[n_cp]);
            for(uint32 k = 0; k < n_cp; k++) {
                temp[k].x = nunv._0501[i].p01[k].p01;
                temp[k].y = nunv._0501[i].p01[k].p02;
                temp[k].z = nunv._0501[i].p01[k].p03;
                temp[k].w = nunv._0501[i].p01[k].p04;
               }
            DWClothControlPoints ccp;
            ccp.data = temp;
            ccp.size = n_cp;
            clothmap.insert(std::map<uint32, DWClothControlPoints>::value_type(il_index, ccp));
            found = true;
           }
        }

     // if NUNV joint map index is not found, we have a problem, use jmi - 1
     if(!found && (nunv._0501[i].h05 > 0))
       {
        // h01 should match with jmap[jmi].p02/p03
        jmi = nunv._0501[i].h05 - 1;
        p02 = jm_data.data[jmi].data[0].p02;
        p03 = jm_data.data[jmi].data[0].p03;
        jmi_ref = ((p02 << 16) | p03);
        if(nunv._0501[i].h01 != jmi_ref) continue;

        // find jmi in surface information
        for(size_t j = 0; j < sdata.elem; j++) {
            uint32 il_index = sdata.data[j][0x01];
            uint32 jm_index = sdata.data[j][0x02];
            if((jm_index == jmi) && il_data.data[il_index].is_cloth) {
               uint32 n_cp = nunv._0501[i].h02;
               if(!n_cp) return true;
               boost::shared_array<DWfloat4> temp(new DWfloat4[n_cp]);
               for(uint32 k = 0; k < n_cp; k++) {
                   temp[k].x = nunv._0501[i].p01[k].p01;
                   temp[k].y = nunv._0501[i].p01[k].p02;
                   temp[k].z = nunv._0501[i].p01[k].p03;
                   temp[k].w = nunv._0501[i].p01[k].p04;
                  }
               DWClothControlPoints ccp;
               ccp.data = temp;
               ccp.size = n_cp;
               clothmap.insert(std::map<uint32, DWClothControlPoints>::value_type(il_index, ccp));
               found = true;
              }
           }
        if(!found) {
           std::stringstream ss;
           ss << "Could not find cloth mesh from NUNO joint map reference 0x" << hex << nunv._0501[i].h05 << dec << "." << endl;
           return error(ss.str());
          }
       }
    }

 return true;
}

#pragma endregion

#pragma region REGION_SECTION_PROCESSING

bool process0101(MODELDATA& md)
{
 // UNKNOWN SECTION
 // 00010001 00000090 00000002
 // 00000100
 // #1 3F800000 3F800000 3F800000 3F800000 3F333333 3F333333 3F333333 00001111
 //    3F333333 3F333333 3F333333 41F00000 00000000 00000000 00000000 FFFF0000
 // #2 3F800000 3F800000 3F800000 3F800000 3F800000 3F800000 3F800000 00001011
 //    00000000 00000000 00000000 00000000 00000000 00000000 00000000 FFFF0000

 return true;
}

bool process0102(MODELDATA& md)
{
 // nothing to do
 if(!md.section02.data) return true;
 if(!md.section02.size) return true;

 // debug
 if(debug) {
    md.dfile << "-------------------" << endl;
    md.dfile << " MATERIALS SECTION " << endl;
    md.dfile << " 0x00010002        " << endl;
    md.dfile << "-------------------" << endl;
    md.dfile << endl;
   }

 // create binary stream
 binary_stream bs(md.section02.data, md.section02.size);
 bs.set_endian_mode(md.mode);

 // read number of index buffers
 md.data0102.elem = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);
 if(!md.data0102.elem) return true;

 // read entries
 md.data0102.data.reset(new G1MG0102ITEM[md.data0102.elem]);
 for(size_t i = 0; i < md.data0102.elem; i++)
    {
     // read material properties
     md.data0102.data[i].p01 = bs.read_uint32(); // 0x00000000
     md.data0102.data[i].p02 = bs.read_uint32(); // number of textures
     md.data0102.data[i].p03 = bs.read_uint32(); // 0x00000000, 0x00000001, 0xFFFFFFFF
     md.data0102.data[i].p04 = bs.read_uint32(); // 0x00000000, 0x00000001, 0xFFFFFFFF
     if(bs.fail()) return error("Stream read failure.", __LINE__);

     // create texture property data
     if(md.data0102.data[i].p02)
        md.data0102.data[i].p05.reset(new G1MG0102TEXTURE[md.data0102.data[i].p02]);

     // read texture properties
     for(uint32 j = 0; j < md.data0102.data[i].p02; j++) {
        md.data0102.data[i].p05[j].p01 = bs.read_uint16(); // texture identifier
        md.data0102.data[i].p05[j].p02a = bs.read_uint16(); // texture type
        md.data0102.data[i].p05[j].p02b = bs.read_uint16(); // texture type
        md.data0102.data[i].p05[j].p03 = bs.read_uint16(); // ??????
        md.data0102.data[i].p05[j].p04 = bs.read_uint16(); // 0x0004
        md.data0102.data[i].p05[j].p05 = bs.read_uint16(); // 0x0004
        if(bs.fail()) return error("Stream read failure.", __LINE__);
       }
    }

 // debug
 if(debug)
   {
    // number of materials
    md.dfile << setfill('0');
    md.dfile << "number of materials = 0x" << hex << md.data0102.elem << dec << endl;
    md.dfile << endl;
    for(size_t i = 0; i < md.data0102.elem; i++) {
        md.dfile << " MATERIAL 0x" << hex << setw(2) << i << dec << endl;
        md.dfile << "  p01 = 0x" << hex << setw(8) << md.data0102.data[i].p01 << dec << " (???)" << endl;
        md.dfile << "  p02 = 0x" << hex << setw(8) << md.data0102.data[i].p02 << dec << " (number of textures)" << endl;
        md.dfile << "  p03 = 0x" << hex << setw(8) << md.data0102.data[i].p03 << dec << " (???)" << endl;
        md.dfile << "  p04 = 0x" << hex << setw(8) << md.data0102.data[i].p04 << dec << " (???)" << endl;
        for(uint32 j = 0; j < md.data0102.data[i].p02; j++) {
            md.dfile << "  TEXTURE INFO 0x" << hex << setw(2) << j << dec << endl;
            md.dfile << "   p01 = 0x" << hex << setw(4) << md.data0102.data[i].p05[j].p01 << dec << " (texture ID)" << endl;
            md.dfile << "   p02 = 0x" << hex << setw(8) << md.data0102.data[i].p05[j].p02a << dec << " (???)" << endl;
            md.dfile << "   p02 = 0x" << hex << setw(8) << md.data0102.data[i].p05[j].p02b << dec << " (texture type)" << endl;
            md.dfile << "   p03 = 0x" << hex << setw(4) << md.data0102.data[i].p05[j].p03 << dec << " (???)" << endl;
            md.dfile << "   p04 = 0x" << hex << setw(4) << md.data0102.data[i].p05[j].p04 << dec << " (???)" << endl;
            md.dfile << "   p05 = 0x" << hex << setw(4) << md.data0102.data[i].p05[j].p05 << dec << " (???)" << endl;
           }
       }
    md.dfile << endl;
   }

 // success
 return true;
}

bool process0103(MODELDATA& md)
{
 // nothing to do
 if(!md.section03.data) return true;
 if(!md.section03.size) return true;

 // debug
 if(debug) {
    md.dfile << "--------------------" << endl;
    md.dfile << " ATTRIBUTES SECTION " << endl;
    md.dfile << " 0x00010003         " << endl;
    md.dfile << "--------------------" << endl;
    md.dfile << endl;
   }

 // create binary stream
 binary_stream bs(md.section03.data, md.section03.size);
 bs.set_endian_mode(md.mode);

 // read number of index buffers
 md.data0103.elem = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);
 if(!md.data0103.elem) return true;

 // debug
 if(debug) {
    md.dfile << setfill('0');
    md.dfile << "number of entries = 0x" << hex << md.data0103.elem << dec << endl;
    md.dfile << endl;
   }

 // read entries
 md.data0103.data.reset(new G1MG0103ITEM[md.data0103.elem]);
 for(size_t i = 0; i < md.data0103.elem; i++)
    {
     // read number of attributes
     md.data0103.data[i].p01 = bs.read_uint32();
     if(bs.fail()) return error("Stream read failure.", __LINE__);

     // create attributes
     if(md.data0103.data[i].p01)
        md.data0103.data[i].p02.reset(new G1MG0103ATTR[md.data0103.data[i].p01]);

     // read attributes
     for(size_t j = 0; j < md.data0103.data[i].p01; j++) {
         md.data0103.data[i].p02[j].p01 = bs.read_uint32(); // length
         md.data0103.data[i].p02[j].p02 = bs.read_uint32(); // string length
         md.data0103.data[i].p02[j].p03 = bs.read_uint32(); // ????
         md.data0103.data[i].p02[j].p04 = bs.read_uint16(); // ????
         md.data0103.data[i].p02[j].p05 = bs.read_uint16(); // ????
         if(md.data0103.data[i].p02[j].p02) bs.read_array(&md.data0103.data[i].p02[j].p06[0], md.data0103.data[i].p02[j].p02);
         uint32 remaining = md.data0103.data[i].p02[j].p01 - md.data0103.data[i].p02[j].p02 - 0x10;
         bs.move(remaining);
         if(bs.fail()) return error("Stream seek failure.", __LINE__);
        }

     // debug
     if(debug) {
        md.dfile << setfill('0');
        md.dfile << "ENTRY 0x" << hex << setw(2) << i << dec << endl;
        md.dfile << " number of attributes = 0x" << hex << setw(8) << md.data0103.data[i].p01 << dec << endl;
        for(size_t j = 0; j < md.data0103.data[i].p01; j++) {
            md.dfile << " ATTRIBUTE 0x" << hex << setw(4) << j << dec << endl;
            md.dfile << "  p01 = 0x" << hex << setw(4) << md.data0103.data[i].p02[j].p01 << dec << " (length)" << endl;
            md.dfile << "  p02 = 0x" << hex << setw(4) << md.data0103.data[i].p02[j].p02 << dec << " (string length)" << endl;
            md.dfile << "  p03 = 0x" << hex << setw(4) << md.data0103.data[i].p02[j].p03 << dec << " (???)" << endl;
            md.dfile << "  p04 = 0x" << hex << setw(4) << md.data0103.data[i].p02[j].p04 << dec << " (???)" << endl;
            md.dfile << "  p05 = 0x" << hex << setw(4) << md.data0103.data[i].p02[j].p05 << dec << " (???)" << endl;
            md.dfile << "  p06 = " << md.data0103.data[i].p02[j].p06 << " (name)" << endl;
           }
        md.dfile << endl;
       }
    }

 // success
 return true;
}

bool process0104(MODELDATA& md)
{
 // nothing to do
 if(!md.section04.data) return true;
 if(!md.section04.size) return true;

 // debug
 if(debug) {
    md.dfile << "----------------" << endl;
    md.dfile << " VERTEX BUFFERS " << endl;
    md.dfile << " 0x00010004     " << endl;
    md.dfile << "----------------" << endl;
    md.dfile << endl;
   }

 // create binary stream
 binary_stream bs(md.section04.data, md.section04.size);
 bs.set_endian_mode(md.mode);

 // read number of index buffers
 md.data0104.elem = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);
 if(!md.data0104.elem) return true;

 // debug
 if(debug) {
    md.dfile << "number of buffers = 0x" << hex << md.data0104.elem << dec << endl;
    md.dfile << endl;
   }

 // read entries
 md.data0104.data.reset(new G1MG0104ITEM[md.data0104.elem]);
 for(size_t i = 0; i < md.data0104.elem; i++)
    {
     // read properties
     md.data0104.data[i].p01 = bs.read_uint32(); // ????
     md.data0104.data[i].p02 = bs.read_uint32(); // vertsize
     md.data0104.data[i].p03 = bs.read_uint32(); // vertices
     md.data0104.data[i].p04 = bs.read_uint32(); // ????
     if(bs.fail()) return error("Stream read failure.", __LINE__);

     // read vertex buffer
     uint32 size = md.data0104.data[i].p02 * md.data0104.data[i].p03;
     md.data0104.data[i].p05.reset(new char[size]);
     bs.read_array(md.data0104.data[i].p05.get(), size);
     if(bs.fail()) return error("Stream read failure.", __LINE__);

     // debug
     if(debug) {
        md.dfile << setfill('0');
        md.dfile << "VERTEX BUFFER 0x" << hex << i << dec << endl;
        md.dfile << " unknown1 = 0x" << hex << md.data0104.data[i].p01 << dec << endl;
        md.dfile << " vertsize = 0x" << hex << md.data0104.data[i].p02 << dec << endl;
        md.dfile << " vertices = 0x" << hex << md.data0104.data[i].p03 << dec << endl;
        md.dfile << " unknown2 = 0x" << hex << md.data0104.data[i].p04 << dec << endl;
        // NOTE: UNCOMMENT THIS IF YOU WANT TO OUTPUT ENTIRE VERTEX BUFFER TO DEBUG FILE!
        binary_stream cs(md.data0104.data[i].p05, size);
        for(size_t j = 0; j < md.data0104.data[i].p03; j++) {
            md.dfile << " " << hex << setfill('0') << setw(4) << (uint32)j << dec << ": ";
            for(uint32 k = 0; k < md.data0104.data[i].p02; k++)
                md.dfile << hex << setfill('0') << setw(2) << (uint32)cs.BE_read_uint08() << dec;
            md.dfile << endl;
           }
        md.dfile << endl;
       }
    }

 // success
 return true;
}

bool process0105(MODELDATA& md)
{
 // nothing to do
 if(!md.section05.data) return true;
 if(!md.section05.size) return true;

 // debug
 if(debug) {
    md.dfile << "-------------------" << endl;
    md.dfile << " INPUT LAYOUT DATA " << endl;
    md.dfile << " 0x00010005        " << endl;
    md.dfile << "-------------------" << endl;
    md.dfile << endl;
   }

 // create binary stream
 binary_stream bs(md.section05.data, md.section05.size);
 bs.set_endian_mode(md.mode);

 // read number of layouts
 md.data0105.elem = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);
 if(!md.data0105.elem) return true;

 // debug
 if(debug) {
    md.dfile << "number of entries = 0x" << hex << md.data0105.elem << dec << endl;
    md.dfile << endl;
   }

 // create entries
 md.data0105.data.reset(new G1MG0105ITEM[md.data0105.elem]);

 // read entries
 for(size_t i = 0; i < md.data0105.elem; i++)
    {
     // read number of vertex buffer references
     md.data0105.data[i].n_refs = bs.read_uint32(); // usually 0x01 or 0x02
     if(bs.fail()) return error("Stream read failure.", __LINE__);

     // validate number of indices
     if(md.data0105.data[i].n_refs < 1) return error("Invalid number of vertex buffer references.", __LINE__);
     if(md.data0105.data[i].n_refs > 4) return error("Invalid number of vertex buffer references.", __LINE__);

     // read indices
     md.data0105.data[i].refs.reset(new uint32[md.data0105.data[i].n_refs]);
     bs.read_array(md.data0105.data[i].refs.get(), md.data0105.data[i].n_refs);
     if(bs.fail()) return error("Stream read failure.", __LINE__);

     // read number of semantics
     md.data0105.data[i].n_semantics = bs.read_uint32();
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     if(!md.data0105.data[i].n_semantics) return error("At least one vertex semantic must be defined.", __LINE__);

     // read semantics
     md.data0105.data[i].semantics.reset(new G1MG0105SEMANTIC[md.data0105.data[i].n_semantics]);
     for(uint32 j = 0; j < md.data0105.data[i].n_semantics; j++) {
         md.data0105.data[i].semantics[j].p01 = bs.read_uint16(); // buffer index
         md.data0105.data[i].semantics[j].p02 = bs.read_uint16(); // offset
         uint16 b1 = bs.read_uint08(); // same order no matter LE or BE
         uint16 b2 = bs.read_uint08(); // same order no matter LE or BE
         md.data0105.data[i].semantics[j].p03 = (b1 << 8) | b2; // data type
         b1 = bs.read_uint08(); // same order no matter LE or BE
         b2 = bs.read_uint08(); // same order no matter LE or BE
         md.data0105.data[i].semantics[j].p04 = (b1 << 8) | b2; // semantic
         if(bs.fail()) return error("Stream read failure.", __LINE__);
        }

     // debug
     if(debug) {
        md.dfile << setfill('0');
        md.dfile << "ENTRY 0x" << hex << i << dec << endl;
        md.dfile << " number of vertex buffers: 0x" << hex << md.data0105.data[i].n_refs << dec << endl;
        md.dfile << " number of semantics: 0x" << hex << md.data0105.data[i].n_semantics << dec << endl;
        // print references
        for(uint32 j = 0; j < md.data0105.data[i].n_refs; j++)
            md.dfile << " REFERENCE 0x" << hex << j << dec << ": 0x" << hex << setw(4) << md.data0105.data[i].refs[j] << dec << endl;
        // print semantics
        for(uint32 j = 0; j < md.data0105.data[i].n_semantics; j++) {
            md.dfile << " SEMANTIC 0x" << hex << j << dec << endl;
            md.dfile << "  p01: 0x" << hex << setw(4) << md.data0105.data[i].semantics[j].p01 << dec << " (buffer index)" << endl;
            md.dfile << "  p02: 0x" << hex << setw(4) << md.data0105.data[i].semantics[j].p02 << dec << " (offset)" << endl;
            md.dfile << "  p03: 0x" << hex << setw(4) << md.data0105.data[i].semantics[j].p03 << dec << " (data type)" << endl;
            md.dfile << "  p04: 0x" << hex << setw(4) << md.data0105.data[i].semantics[j].p04 << dec << " (semantic)" << endl;
           }
        md.dfile << endl;
       }
    }

 // success
 return true;
}

bool process0106(MODELDATA& md)
{
 // nothing to do
 if(!md.section06.data) return true;
 if(!md.section06.size) return true;

 // debug
 if(debug) {
    md.dfile << "----------------" << endl;
    md.dfile << " JOINT MAP DATA " << endl;
    md.dfile << " 0x00010006     " << endl;
    md.dfile << "----------------" << endl;
    md.dfile << endl;
   }

 // create binary stream
 binary_stream bs(md.section06.data, md.section06.size);
 bs.set_endian_mode(md.mode);

 // read number of index buffers
 md.data0106.elem = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);
 if(!md.data0106.elem) return true;

 // debug
 if(debug) {
    md.dfile << "number of entries = 0x" << hex << md.data0106.elem << dec << endl;
    md.dfile << endl;
   }

 // create entries
 md.data0106.data.reset(new G1MG0106ITEM[md.data0106.elem]);

 // read entries
 for(size_t i = 0; i < md.data0106.elem; i++)
    {
     // read number of items
     md.data0106.data[i].elem = bs.read_uint32();
     if(bs.fail()) return error("Stream read failure.", __LINE__);

     // allocate items
     if(md.data0106.data[i].elem)
        md.data0106.data[i].data.reset(new G1MG0106ITEMENTRY[md.data0106.data[i].elem]);

     // read items
     for(size_t j = 0; j < md.data0106.data[i].elem; j++) {
         md.data0106.data[i].data[j].p01 = bs.read_uint32(); // reference to G1MM matrix
         uint32 t1 = bs.read_uint32();
         uint32 t2 = bs.read_uint32();
         md.data0106.data[i].data[j].p02 = (t1 & 0xFFFF0000ul) >> 16; // ???
         md.data0106.data[i].data[j].p03 = (t1 & 0x0000FFFFul);       // if not 0x0000, this part needs to be moved
         md.data0106.data[i].data[j].p04 = (t2 & 0xFFFF0000ul) >> 16; // ???
         md.data0106.data[i].data[j].p05 = (t2 & 0x0000FFFFul);       // joint[j] maps to this joint
         if(bs.fail()) return error("Stream read failure.", __LINE__);
        }
    }

 // debug entries
 if(debug) {
    for(size_t i = 0; i < md.data0106.elem; i++) {
        md.dfile << "ENTRY[0x" << hex << i << dec << "]" << endl;
        md.dfile << " number of items = 0x" << hex << md.data0106.data[i].elem << dec << endl;
        for(size_t j = 0; j < md.data0106.data[i].elem; j++) {
            md.dfile << " ITEM[0x" << setfill('0') << setw(2) << hex << j << dec << "]: ";
            md.dfile << setfill('0') << setw(8) << hex << md.data0106.data[i].data[j].p01 << dec << " ";
            md.dfile << setfill('0') << setw(4) << hex << md.data0106.data[i].data[j].p02 << dec << " ";
            md.dfile << setfill('0') << setw(4) << hex << md.data0106.data[i].data[j].p03 << dec << " ";
            md.dfile << setfill('0') << setw(4) << hex << md.data0106.data[i].data[j].p04 << dec << " ";
            md.dfile << setfill('0') << setw(4) << hex << md.data0106.data[i].data[j].p05 << dec << " ";
            md.dfile << endl;
           }
       }
    md.dfile << endl;
   }

 // success
 return true;
}

bool process0107(MODELDATA& md)
{
 // nothing to do
 if(!md.section07.data) return true;
 if(!md.section07.size) return true;

 // debug
 if(debug) {
    md.dfile << "-------------------" << endl;
    md.dfile << " INDEX BUFFER LIST " << endl;
    md.dfile << " 0x00010007        " << endl;
    md.dfile << "-------------------" << endl;
    md.dfile << endl;
   }

 // create binary stream
 binary_stream bs(md.section07.data, md.section07.size);
 bs.set_endian_mode(md.mode);

 // read number of index buffers
 md.data0107.elem = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);
 if(!md.data0107.elem) return true;

 // debug
 if(debug) {
    md.dfile << "number of index buffers = 0x" << hex << md.data0107.elem << dec << endl;
    md.dfile << endl;
   }

 // create entries
 md.data0107.data.reset(new G1MG0107ITEM[md.data0107.elem]);
 for(uint32 i = 0; i < md.data0107.elem; i++) {
     md.data0107.data[i].elem = 0;
     md.data0107.data[i].type = 0;
     md.data0107.data[i].unknown = 0;
    }

 // for each mesh
 for(size_t i = 0; i < md.data0107.elem; i++)
    {
     // read index buffer header
     md.data0107.data[i].elem = bs.read_uint32();    // number of indices
     md.data0107.data[i].type = bs.read_uint32();    // 0x08/0x10/0x20
     md.data0107.data[i].unknown = bs.read_uint32(); // 0x00
     if(bs.fail()) return error("Stream read failure.", __LINE__);

     // determine index buffer format
     uint32 indexsize = 0;
     if(md.data0107.data[i].type == 0x08) indexsize = sizeof(uint08);
     else if(md.data0107.data[i].type == 0x10) indexsize = sizeof(uint16);
     else if(md.data0107.data[i].type == 0x20) indexsize = sizeof(uint32);
     else return error("Unknown index buffer data type.", __LINE__);

     // read index buffer
     uint32 total_bytes = md.data0107.data[i].elem * indexsize;
     if(total_bytes) {
        md.data0107.data[i].data.reset(new char[total_bytes]);
        switch(md.data0107.data[i].type) {
          case(0x08) : bs.read_array(reinterpret_cast<uint08*>(md.data0107.data[i].data.get()), md.data0107.data[i].elem); break;
          case(0x10) : bs.read_array(reinterpret_cast<uint16*>(md.data0107.data[i].data.get()), md.data0107.data[i].elem); break;
          case(0x20) : bs.read_array(reinterpret_cast<uint32*>(md.data0107.data[i].data.get()), md.data0107.data[i].elem); break;
         }
        if(bs.fail()) return error("Stream read failure.", __LINE__);
       }

     // debug
     if(debug) {
        md.dfile << setfill('0');
        md.dfile << "INDEX BUFFER[0x" << setw(2) << hex << i << dec << "]" << endl; 
        md.dfile << " elem = 0x" << hex << setw(8) << md.data0107.data[i].elem << dec << endl;
        md.dfile << " type = 0x" << hex << setw(8) << md.data0107.data[i].type << dec << endl;
        md.dfile << " unknown = 0x" << hex << setw(8) << md.data0107.data[i].unknown << dec << endl;
        md.dfile << endl;
       }

     // BEGIN TESTING
     // [PS3] Dragon Quest Heroes\LINKDATA\0139\000\000_MDLK\016.g1m
     auto position = bs.tell();
     if((position % 4) && (md.data0107.elem > 1)) {
        // if(debug) cout << " NOTE: The next index buffer position is not divisible by 4!" << endl;
        position = ((position + 0x03) & (~0x03));
        bs.seek(position);
       }
     // END TESTING
    }

 // success
 return true;
}

bool process0108(MODELDATA& md)
{
 // nothing to do
 if(!md.section08.data) return true;
 if(!md.section08.size) return true;

 // debug (always display so that we can move incorrectly placed parts)
 md.dfile << "---------------------" << endl;
 md.dfile << " SURFACE INFORMATION " << endl;
 md.dfile << " 0x00010008          " << endl;
 md.dfile << "---------------------" << endl;
 md.dfile << endl;

 // create binary stream
 binary_stream bs(md.section08.data, md.section08.size);
 bs.set_endian_mode(md.mode);

 // read number of entries
 md.data0108.elem = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);
 if(!md.data0108.elem) return true; // nothing to do

 // create entries
 md.data0108.data.reset(new boost::shared_array<uint32>[md.data0108.elem]);
 for(uint32 i = 0; i < md.data0108.elem; i++) md.data0108.data[i].reset(new uint32[14]);

 // read entries
 size_t j = 0;
 for(size_t i = 0; i < md.data0108.elem; i++)
    {
     // read first two values
     uint32 p01 = bs.read_uint32();
     uint32 p02 = bs.read_uint32();

     // read valid entry
     if(p02 != 0xFFFFFFFF) {
        md.data0108.data[j][0x00] = p01;              // C00000D0
        md.data0108.data[j][0x01] = p02;              // vertex buffer reference
        md.data0108.data[j][0x02] = bs.read_uint32(); // index into joint map
        md.data0108.data[j][0x03] = bs.read_uint32(); // ???
        md.data0108.data[j][0x04] = bs.read_uint32(); // ???
        md.data0108.data[j][0x05] = bs.read_uint32(); // ???
        md.data0108.data[j][0x06] = bs.read_uint32(); // material reference
        md.data0108.data[j][0x07] = bs.read_uint32(); // index buffer reference
        md.data0108.data[j][0x08] = bs.read_uint32(); // ???
        md.data0108.data[j][0x09] = bs.read_uint32(); // index buffer format (0x1, 0x4)
        md.data0108.data[j][0x0A] = bs.read_uint32(); // vertex buffer start
        md.data0108.data[j][0x0B] = bs.read_uint32(); // number of vertices
        md.data0108.data[j][0x0C] = bs.read_uint32(); // index buffer start
        md.data0108.data[j][0x0D] = bs.read_uint32(); // number of indices
        j++;
       }
     // read invalid entry
     else {
        bs.read_uint32(); // FF FF FF FF
        bs.read_uint32(); // FF FF FF FF
        bs.read_uint32(); // 00 00 00 00
        bs.read_uint32(); // 01 00 00 00
        bs.read_uint32(); // FF FF FF FF
        bs.read_uint32(); // FF FF FF FF
        bs.read_uint32(); // 00 00 00 00
       }
     if(bs.fail()) return error("Stream read failure.", __LINE__);
    }

 // adjust size of data structure
 md.data0108.elem = j;

 // debug
 if(debug) {
    md.dfile << "number of entries = 0x" << hex << md.data0108.elem << dec << endl;
    md.dfile << endl;
    md.dfile << "SURFACE DATA" << endl;
    for(size_t i = 0; i < md.data0108.elem; i++) {
        md.dfile << setfill('0');
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x00] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x01] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x02] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x03] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x04] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x05] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x06] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x07] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x08] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x09] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x0A] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x0B] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x0C] << dec;
        md.dfile << " 0x" << hex << setw(8) << md.data0108.data[i][0x0D] << dec;
        md.dfile << endl;
       }
    md.dfile << endl;
    md.dfile << " CLOTH" << endl;
    for(size_t i = 0; i < md.data0107.elem; i++) if(md.data0105.data[i].is_cloth) md.dfile << " input layout 0x" << hex << i << dec << " is cloth." << endl;
    md.dfile << endl;
    md.dfile << " NUNO" << endl;
    for(size_t i = 0; i < md.nuno._0301.size(); i++) {
        md.dfile << " [" << hex << i << dec << "] = ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nuno._0301[i].h01 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nuno._0301[i].h02 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nuno._0301[i].h03 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nuno._0301[i].h04 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nuno._0301[i].h05 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nuno._0301[i].h06 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nuno._0301[i].h09 << dec << endl;
       }
    md.dfile << endl;
    md.dfile << " NUNV" << endl;
    for(size_t i = 0; i < md.nunv._0501.size(); i++) {
        md.dfile << " [" << hex << i << dec << "] = ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nunv._0501[i].h01 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nunv._0501[i].h02 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nunv._0501[i].h03 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nunv._0501[i].h04 << dec << "  ";
        md.dfile << "0x" << hex << setfill('0') << setw(8) << md.nunv._0501[i].h05 << dec << endl;
       }
    md.dfile << endl;
    md.dfile << " JMAP" << endl;
    for(size_t i = 0; i < md.data0106.elem; i++) {
        md.dfile << " [" << hex << i << dec << "] = ";
        uint32 p02 = md.data0106.data[i].data[0].p02;
        uint32 p03 = md.data0106.data[i].data[0].p03;
        uint32 value = (p02 << 16) | p03;
        md.dfile << "0x" << hex << setfill('0') << setw(8) << value << dec << endl;
       }
    md.dfile << endl;
   }

 // debug (always display so that we can move incorrectly placed parts)
 for(size_t i = 0; i < md.data0108.elem; i++)
    {
     // get joint map index
     uint32 jmap_index = md.data0108.data[i][0x02];
     if(!(jmap_index < md.data0106.elem)) return error("SECTION 0x00010008: Joint map index out of range.", __LINE__);

     // if there are no entries in joint map then we can't do bone association
     if(!md.data0106.data[jmap_index].elem) {
        md.dfile << setfill('0');
        md.dfile << "surface_"  << setw(3) << i << "_ref_00" << " " << "does not have any bone association." << endl;
        continue;
       }

     // get bone this mesh is tied to
     uint32 bone_index = md.data0106.data[jmap_index].data[0].p03;
     md.dfile << setfill('0');
     md.dfile << "surface_"  << setw(3) << i << "_ref_00" << " " << "is tied to bone 0x" << hex << setw(4) << bone_index << dec << "." << endl;
    }
 md.dfile << endl;

 // success
 return true;
}

bool process0109(MODELDATA& md)
{
 return true;
}

#pragma endregion

#pragma region REGION_BUILDING_PHASE

// materials
bool buildPhase1(MODELDATA& md, ADVANCEDMODELCONTAINER& amc)
{
 // nothing to do
 G1MG0102DATA& matdata = md.data0102;
 if(!matdata.elem) return true;

 // filemap type maps a texture ID to a file index
 std::map<uint16, uint16> filemap;
 uint16 texture_index = AMC_INVALID_TEXTURE;

 // for each material
 for(size_t i = 0; i < matdata.elem; i++)
    {
     // create surface name
     stringstream ss;
     ss << "material_0x" << setfill('0') << setw(2) << hex << i << dec;

     // initialize material
     AMC_SURFMAT mat;
     mat.name = ss.str();
     mat.twoside = 0;
     mat.unused1 = 0;
     mat.unused2 = 0;
     mat.unused3 = 0;
     mat.basemap = AMC_INVALID_TEXTURE;
     mat.specmap = AMC_INVALID_TEXTURE;
     mat.tranmap = AMC_INVALID_TEXTURE;
     mat.bumpmap = AMC_INVALID_TEXTURE;
     mat.normmap = AMC_INVALID_TEXTURE;
     mat.lgthmap = AMC_INVALID_TEXTURE;
     mat.envimap = AMC_INVALID_TEXTURE;
     mat.glssmap = AMC_INVALID_TEXTURE;
     mat.resmap1 = AMC_INVALID_TEXTURE;
     mat.resmap2 = AMC_INVALID_TEXTURE;
     mat.resmap3 = AMC_INVALID_TEXTURE;
     mat.resmap4 = AMC_INVALID_TEXTURE;
     mat.resmap5 = AMC_INVALID_TEXTURE;
     mat.resmap6 = AMC_INVALID_TEXTURE;
     mat.resmap7 = AMC_INVALID_TEXTURE;
     mat.resmap8 = AMC_INVALID_TEXTURE;
     mat.basemapchan = 0;
     mat.specmapchan = 0;
     mat.tranmapchan = 0;
     mat.bumpmapchan = 0;
     mat.normmapchan = 0;
     mat.lghtmapchan = 0;
     mat.envimapchan = 0;
     mat.glssmapchan = 0;
     mat.resmapchan1 = 0;
     mat.resmapchan2 = 0;
     mat.resmapchan3 = 0;
     mat.resmapchan4 = 0;
     mat.resmapchan5 = 0;
     mat.resmapchan6 = 0;
     mat.resmapchan7 = 0;
     mat.resmapchan8 = 0;

     // for each texture
     for(uint32 j = 0; j < matdata.data[i].p02; j++)
        {
         // texture properties
         uint16 texture_iden = matdata.data[i].p05[j].p01;
         uint16 texture_type = matdata.data[i].p05[j].p02b;
         uint16 unk01 = matdata.data[i].p05[j].p03; // ??? texture channel ???
         uint16 unk02 = matdata.data[i].p05[j].p04; // 0x0004
         uint16 unk03 = matdata.data[i].p05[j].p05; // 0x0004

         // insert texture into filelist
         auto iter = filemap.find(texture_iden);
         if(iter == filemap.end()) {
            filemap.insert(map<uint16, uint16>::value_type(texture_iden, (uint16)filemap.size()));
            stringstream ss;
            ss << setfill('0') << setw(3) << texture_iden << ".dds";
            AMC_IMAGEFILE aif = { ss.str() };
            amc.iflist.push_back(aif);
            texture_index = static_cast<uint16>(amc.iflist.size() - 1);
           }
         else
            texture_index = iter->second;

         // set texture type
         texture_type &= 0xFFFF;
         switch(texture_type) {
           case(0x00000000) : mat.specmap = texture_index; break;
           case(0x00000001) : mat.basemap = texture_index; break;
           case(0x00000002) : break;
           case(0x00000003) : mat.normmap = texture_index; break;
           case(0x00000005) : break; // BLJM61225 DW8: Empires
           case(0x00000006) : break; // Wii-U Hyrule Warriors
           case(0x00000009) : break; // [PC] Warriors All-Stars
           case(0x00000011) : break;
           case(0x00000013) : break;
           case(0x00000015) : break;
           case(0x00000018) : break; // [PC] One Piece Pirate Warriors 3
           case(0x0000001D) : break; // BLJM61225 DW8: Empires
           case(0x00000025) : break; // [PC] Warriors All-Stars
           case(0x00000026) : break; // [PC] Warriors All-Stars
           case(0x00000027) : break; // [PC] Arslan
           case(0x0000002F) : break; // [PC] Arslan
           case(0x00000038) : break; // [PC] Warriors All-Stars
           case(0x00000039) : break; // [PC] Warriors All-Stars

           // new [PC] Dynasty Warriors 9
           case(0x0000001E) : break;
           case(0x00000029) : break;
           case(0x00000031) : break;
           case(0x0000003E) : break;

           case(0x00000080) : break;
           case(0x00000081) : break;
           case(0x00000082) : break;
           case(0x00000083) : break;
           case(0x00000084) : break;
           case(0x00000085) : break;
           case(0x00000086) : break;
           case(0x00010003) : break; // this might be normal map (two 16-bit values)
           case(0x00010080) : break; // this might be 0x0080 (two 16-bit values with first value as channel?)
           case(0x00010081) : break; // this might be 0x0081 (two 16-bit values with first value as channel?)
           case(0x00010011) : break; // this might be 0x0011 (two 16-bit values with first value as channel?)
           case(0x00020011) : break; // this might be 0x0011 (two 16-bit values with first value as channel?)
           default : {
                stringstream ss;
                ss << "Warning! Unknown texture type 0x" << hex << texture_type << dec << ".";
                message(ss.str().c_str()); // make it just a warning now
                // return error(ss);
               }
          }
        }

     // insert material
     amc.surfmats.push_back(mat);
    }

 return true;
}

// skeleton (joint maps)
bool buildPhase2(MODELDATA& md, ADVANCEDMODELCONTAINER& amc)
{
 // nothing to do
 G1MG0106DATA& data = md.data0106;
 if(!data.elem) return true;

 // for each entry
 for(size_t i = 0; i < data.elem; i++)
    {
     // construct joint map
     G1MG0106ITEM& item = data.data[i];
     deque<uint32> jntmap;
     for(size_t j = 0; j < item.elem; j++) jntmap.push_back(item.data[j].p05);

     // insert joint map
     AMC_JOINTMAP jmap;
     jmap.jntmap = jntmap;
     amc.jmaplist.push_back(jmap);
    }

 return true;
}

// vertex buffers
bool buildPhase3(MODELDATA& md, ADVANCEDMODELCONTAINER& amc)
{
 // nothing to do
 G1MG0104DATA& vb_data = md.data0104;
 if(!vb_data.elem) return true;

 // nothing to do
 G1MG0105DATA& il_data = md.data0105;
 if(!il_data.elem) return true;

 // for each input layout
 for(uint32 i = 0; i < il_data.elem; i++)
    {
     // validate input layout item
     G1MG0105ITEM& il_item = il_data.data[i];
     if(il_item.n_refs == 0) return error("An input layout must contain at least one vertex buffer reference.");

     // cloth variables
     bool is_cloth = md.IsCloth(i);
     uint32 n_cp = 0;
     boost::shared_array<DWfloat4> cpdata;
     if(is_cloth) {
        if(!md.GetControlPoints(i, cpdata, n_cp)) {
           std::stringstream ss;
           ss << "Failed to retrieve control point data for input layout index 0x" << hex << i << dec << ".";
           return error(ss.str().c_str());
          }
       }

     // validate vertex buffer references
     for(uint32 j = 0; j < il_item.n_refs; j++)
         if(!(il_item.refs[j] < vb_data.elem))
            return error("Input layout vertex buffer reference out of range.");

     // validate number of vertices
     for(uint32 j = 1; j < il_item.n_refs; j++) {
         uint32 index1 = il_item.refs[0];
         uint32 index2 = il_item.refs[j];
         G1MG0104ITEM& item1 = vb_data.data[index1];
         G1MG0104ITEM& item2 = vb_data.data[index2];
         if(item1.p03 != item2.p03)
            return error("Input layouts that reference multiple vertex buffers must have buffers with matching number of vertices.");
        }

     // get number of vertices
     const uint32 n_vertices = vb_data.data[il_item.refs[0]].p03;
     if(!n_vertices) return error("Vertex buffer has no vertices.");

     // prepare cloth inputs
     boost::shared_array<DWCLOTHINPUT> cloth;
     if(is_cloth) cloth.reset(new DWCLOTHINPUT[n_vertices]);

     // prepare vertex buffer
     AMC_VTXBUFFER vb;
     vb.name = "default";
     vb.elem = n_vertices;
     vb.data.reset(new AMC_VERTEX[vb.elem]);
     vb.flags = AMC_VERTEX_POSITION;
     vb.uvchan = 0;
     vb.uvtype[0] = AMC_INVALID_MAP;
     vb.uvtype[1] = AMC_INVALID_MAP;
     vb.uvtype[2] = AMC_INVALID_MAP;
     vb.uvtype[3] = AMC_INVALID_MAP;
     vb.uvtype[4] = AMC_INVALID_MAP;
     vb.uvtype[5] = AMC_INVALID_MAP;
     vb.uvtype[6] = AMC_INVALID_MAP;
     vb.uvtype[7] = AMC_INVALID_MAP;
     vb.colors = 0;

     // booleans to keep track of mesh type
     bool has_blendweights = false;
     bool has_blendindices = false;

     // prepare vertex buffer flags
     for(uint32 j = 0; j < il_item.n_semantics; j++)
        {
         G1MG0105SEMANTIC& semantic = il_item.semantics[j];
         switch(semantic.p04) {
           case(0x0100) : {
                if(!is_cloth) {
                   has_blendweights = true;
                   vb.flags |= AMC_VERTEX_WEIGHTS;
                  }
                break;
               }
           case(0x0200) : {
                if(!is_cloth) {
                   has_blendindices = true;
                   vb.flags |= AMC_VERTEX_WEIGHTS;
                  }
                break;
               }
           case(0x0300) : {
                if(!is_cloth) vb.flags |= AMC_VERTEX_NORMAL;
                break;
               }
           case(0x0500) : {
                vb.uvchan++;
                vb.uvtype[0] = AMC_DIFFUSE_MAP;
                break;
               }
           case(0x0501) : {
                vb.uvchan++;
                vb.uvtype[1] = AMC_DIFFUSE_MAP;
                break;
               }
           case(0x0502) : {
                vb.uvchan++;
                vb.uvtype[2] = AMC_DIFFUSE_MAP;
                break;
               }
          }
        }

     // enable UVs
     if(vb.uvchan) vb.flags |= AMC_VERTEX_UV;

     // process vertex semantics
     for(uint32 j = 0; j < il_item.n_semantics; j++)
        {
         // semantic properties
         const G1MG0105SEMANTIC& item = il_item.semantics[j];
         uint16 p01 = item.p01; // buffer reference
         uint16 p02 = item.p02; // offset
         uint16 p03 = item.p03; // datatype
         uint16 p04 = item.p04; // semantic

         // binary stream from buffer
         auto stride = vb_data.data[il_item.refs[p01]].p02; // stride
         auto n_vert = vb_data.data[il_item.refs[p01]].p03; // number of vertices
         auto buffer = vb_data.data[il_item.refs[p01]].p05; // vertex buffer

         // binary stream from buffer
         binary_stream bs(buffer, n_vert * stride);
         bs.set_endian_mode(md.mode);

         // POSITION
         // CODE: 0x0000
         if(p04 == 0x0000)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // 0x0200 STATIC MODELS
                if(p03 == 0x0200) {
                   vb.data[k].vx = bs.read_real32();
                   vb.data[k].vy = bs.read_real32();
                   vb.data[k].vz = bs.read_real32();
                   vb.data[k].vw = 1.0f;
                  }
                // 0x0300 CHARACTER MODELS
                else if(p03 == 0x0300) {
                   vb.data[k].vx = bs.read_real32();
                   vb.data[k].vy = bs.read_real32();
                   vb.data[k].vz = bs.read_real32();
                   vb.data[k].vw = bs.read_real32();
                  }
                // 0x0B00 CHARACTER MODELS
                else if(p03 == 0x0B00) {
                   vb.data[k].vx = bs.read_real16();
                   vb.data[k].vy = bs.read_real16();
                   vb.data[k].vz = bs.read_real16();
                   vb.data[k].vw = bs.read_real16();
                  }
                else
                   return error("Unknown vertex stream 0x0000 type.", __FILE__);

                // copy to cloth data
                if(is_cloth) {
                   cloth[k].v0.x = vb.data[k].vx;
                   cloth[k].v0.y = vb.data[k].vy;
                   cloth[k].v0.z = vb.data[k].vz;
                   cloth[k].v0.w = vb.data[k].vw;
                  }
               }
           }
         // BLEND WEIGHTS
         // CODE: 0x0100
         else if(p04 == 0x0100)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // read values
                real32 values[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
                if(p03 == 0x0000) {
                   values[0] = bs.read_real32();
                   values[1] = 1.0f - values[0];
                   if(values[1] < 0.0001f) values[1] = 0.0f;
                  }
                else if(p03 == 0x0100) {
                   values[0] = bs.read_real32();
                   values[1] = bs.read_real32();
                   values[2] = 1.0f - values[0] - values[1];
                   if(values[2] < 0.0001f) values[2] = 0.0f;
                  }
                else if(p03 == 0x0200) {
                   values[0] = bs.read_real32();
                   values[1] = bs.read_real32();
                   values[2] = bs.read_real32();
                   values[3] = 1.0f - values[0] - values[1] - values[2];
                   if(values[3] < 0.0001f) values[3] = 0.0f;
                  }
                else if(p03 == 0x0300) {
                   values[0] = bs.read_real32();
                   values[1] = bs.read_real32();
                   values[2] = bs.read_real32();
                   values[3] = bs.read_real32();
                  }
                else if(p03 == 0x0A00) {
                   values[0] = bs.read_real16();
                   values[1] = bs.read_real16();
                   values[2] = 1.0f - values[0] - values[1];
                   if(values[2] < 0.0001f) values[2] = 0.0f;
                  }
                else if(p03 == 0x0B00) {
                   values[0] = bs.read_real16();
                   values[1] = bs.read_real16();
                   values[2] = bs.read_real16();
                   values[3] = bs.read_real16();
                  }
                else if(p03 == 0x0D00) {
                   values[0] = (real32)(bs.read_uint08()/255.0f);
                   values[1] = (real32)(bs.read_uint08()/255.0f);
                   values[2] = (real32)(bs.read_uint08()/255.0f);
                   values[3] = (real32)(bs.read_uint08()/255.0f);
                  }
                else
                   return error("Unknown vertex stream 0x0100 type.", __FILE__);

                // copy to cloth or normal blendweights
                if(is_cloth) {
                   cloth[k].v1.x = values[0];
                   cloth[k].v1.y = values[1];
                   cloth[k].v1.z = values[2];
                   cloth[k].v1.w = values[3];
                  }
                else {
                   vb.data[k].wv[0] = values[0];
                   vb.data[k].wv[1] = values[1];
                   vb.data[k].wv[2] = values[2];
                   vb.data[k].wv[3] = values[3];
                   vb.data[k].wv[4] = values[4];
                   vb.data[k].wv[5] = values[5];
                   vb.data[k].wv[6] = values[6];
                   vb.data[k].wv[7] = values[7];
                  }
               }
           }
         // BLEND INDICES
         // CODE: 0x0200
         else if(p04 == 0x0200)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                if(p03 == 0x0500)
                  {
                   // read indices
                   uint16 b1 = bs.read_uint08();
                   uint16 b2 = bs.read_uint08();
                   uint16 b3 = bs.read_uint08();
                   uint16 b4 = bs.read_uint08();

                   // copy to cloth
                   if(is_cloth) {
                      cloth[k].v4.x = (float)b1;
                      cloth[k].v4.y = (float)b2;
                      cloth[k].v4.z = (float)b3;
                      cloth[k].v4.w = (float)b4;
                     }
                   // normal divide-by-three blend indices
                   else if(has_blendweights) {
                      vb.data[k].wi[0] = (b1 / 3);
                      vb.data[k].wi[1] = (b2 / 3);
                      vb.data[k].wi[2] = (b3 / 3);
                      vb.data[k].wi[3] = (b4 / 3);
                      vb.data[k].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
                     }
                   // normal divide-by-three blend indices (no weights specified though)
                   // WARNING!
                   // Eyeballs have weight indices, but do not have any weight values. Since
                   // eyeball are cylindrical, assume that the first weight value is 1.0f.
                   // There should only be one weight index for this.
                   else {
                      // set blendweights
                      vb.data[k].wv[0] = 1.0f;
                      vb.data[k].wv[1] = 0.0f;
                      vb.data[k].wv[2] = 0.0f;
                      vb.data[k].wv[3] = 0.0f;
                      vb.data[k].wv[4] = 0.0f;
                      vb.data[k].wv[5] = 0.0f;
                      vb.data[k].wv[6] = 0.0f;
                      vb.data[k].wv[7] = 0.0f;
                      // set blendindices
                      vb.data[k].wi[0] = b1 / 3;
                      vb.data[k].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
                     }
                  }
                else if(p03 == 0x0700) // NEW! [PC] Warriors All-Stars (2017)
                  {
                   // read indices
                   uint16 b1 = bs.read_uint16();
                   uint16 b2 = bs.read_uint16();
                   uint16 b3 = bs.read_uint16();
                   uint16 b4 = bs.read_uint16();

                   // copy to cloth
                   if(is_cloth) {
                      cloth[k].v4.x = (float)b1;
                      cloth[k].v4.y = (float)b2;
                      cloth[k].v4.z = (float)b3;
                      cloth[k].v4.w = (float)b4;
                     }
                   // normal divide-by-three blend indices
                   else if(has_blendweights) {
                      vb.data[k].wi[0] = (b1 / 3);
                      vb.data[k].wi[1] = (b2 / 3);
                      vb.data[k].wi[2] = (b3 / 3);
                      vb.data[k].wi[3] = (b4 / 3);
                      vb.data[k].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
                     }
                   // normal divide-by-three blend indices (no weights specified though)
                   // WARNING!
                   // Eyeballs have weight indices, but do not have any weight values. Since
                   // eyeball are cylindrical, assume that the first weight value is 1.0f.
                   // There should only be one weight index for this.
                   else {
                      // set blendweights
                      vb.data[k].wv[0] = 1.0f;
                      vb.data[k].wv[1] = 0.0f;
                      vb.data[k].wv[2] = 0.0f;
                      vb.data[k].wv[3] = 0.0f;
                      vb.data[k].wv[4] = 0.0f;
                      vb.data[k].wv[5] = 0.0f;
                      vb.data[k].wv[6] = 0.0f;
                      vb.data[k].wv[7] = 0.0f;
                      // set blendindices
                      vb.data[k].wi[0] = b1 / 3;
                      vb.data[k].wi[1] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[2] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[3] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[4] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
                      vb.data[k].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
                     }
                  }
                else
                   return error("Unknown vertex stream 0x0200 type.", __FILE__);
               }
           }
         // NORMAL
         // CODE: 0x0300
         else if(p04 == 0x0300)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // 0x0200 STATIC MODELS
                if(p03 == 0x0200) {
                   vb.data[k].nx = bs.read_real32();
                   vb.data[k].ny = bs.read_real32();
                   vb.data[k].nz = bs.read_real32();
                   vb.data[k].nw = 1.0f;
                  }
                // 0x0300 CHARACTER MODELS
                else if(p03 == 0x0300) {
                   vb.data[k].nx = bs.read_real32();
                   vb.data[k].ny = bs.read_real32();
                   vb.data[k].nz = bs.read_real32();
                   vb.data[k].nw = bs.read_real32();
                  }
                // 0x0B00 CHARACTER MODELS
                else if(p03 == 0x0B00) {
                   vb.data[k].nx = bs.read_real16();
                   vb.data[k].ny = bs.read_real16();
                   vb.data[k].nz = bs.read_real16();
                   vb.data[k].nw = bs.read_real16();
                  }
                else
                   return error("Unknown vertex stream 0x0300 type.", __FILE__);

                // copy to cloth
                if(is_cloth) {
                   cloth[k].v8.x = vb.data[k].nx;
                   cloth[k].v8.y = vb.data[k].ny;
                   cloth[k].v8.z = vb.data[k].nz;
                   cloth[k].v8.w = vb.data[k].nw;
                  }
               }
           }
         // PSIZE
         // CODE: 0x0400
         else if(p04 == 0x0400)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // read data
                if(p03 == 0x0200) {
                   real32 v1 = bs.read_real32();
                   real32 v2 = bs.read_real32();
                   real32 v3 = bs.read_real32();
                   real32 v4 = 1.0f;
                  }
                else if(p03 == 0x0500)
                  {
                   // read indices
                   uint16 b1 = bs.read_uint08();
                   uint16 b2 = bs.read_uint08();
                   uint16 b3 = bs.read_uint08();
                   uint16 b4 = bs.read_uint08();

                   // copy to cloth
                   if(is_cloth)  {
                      cloth[k].v5.x = (float)b1;
                      cloth[k].v5.y = (float)b2;
                      cloth[k].v5.z = (float)b3;
                      cloth[k].v5.w = (float)b4;
                     }
                  }
                else
                   return error("Unknown vertex stream 0x0400 type.", __FILE__);
               }
           }
         // TEXCOORD0
         // CODE: 0x0500
         else if(p04 == 0x0500)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // read values
                if(p03 == 0x0100) {
                   vb.data[k].uv[0][0] = bs.read_real32();
                   vb.data[k].uv[0][1] = bs.read_real32();
                  }
                else if(p03 == 0x0A00) {
                   vb.data[k].uv[0][0] = bs.read_real16();
                   vb.data[k].uv[0][1] = bs.read_real16();
                  }
                else
                   return error("Unknown vertex stream 0x0500 type.", __FILE__);

                // also copy to cloth
                if(is_cloth) {
                   cloth[k].v10.x = vb.data[k].uv[0][0];
                   cloth[k].v10.y = vb.data[k].uv[0][1];
                   cloth[k].v10.z = 0.0f;
                   cloth[k].v10.w = 0.0f;
                  }
               }
           }
         // TEXCOORD1
         // CODE: 0x0501
         else if(p04 == 0x0501)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // read values
                if(p03 == 0x0100) {
                   vb.data[k].uv[1][0] = bs.read_real32();
                   vb.data[k].uv[1][1] = bs.read_real32();
                  }
                else if(p03 == 0x0A00) {
                   vb.data[k].uv[1][0] = bs.read_real16();
                   vb.data[k].uv[1][1] = bs.read_real16();
                  }
                else
                   return error("Unknown vertex stream 0x0501 type.", __FILE__);

                // also copy to cloth
                if(is_cloth) {
                   cloth[k].v11.x = vb.data[k].uv[1][0];
                   cloth[k].v11.y = vb.data[k].uv[1][1];
                   cloth[k].v11.z = 0.0f;
                   cloth[k].v11.w = 0.0f;
                  }
               }
           }
         // TEXCOORD2
         // CODE: 0x0502
         else if(p04 == 0x0502)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // read values
                if(p03 == 0x0100) {
                   vb.data[k].uv[2][0] = bs.read_real32();
                   vb.data[k].uv[2][1] = bs.read_real32();
                  }
                else if(p03 == 0x0A00) {
                   vb.data[k].uv[2][0] = bs.read_real16();
                   vb.data[k].uv[2][1] = bs.read_real16();
                  }
                else
                   return error("Unknown vertex stream 0x0502 type.", __FILE__);

                // also copy to cloth
                if(is_cloth) {
                   cloth[k].v12.x = vb.data[k].uv[2][0];
                   cloth[k].v12.y = vb.data[k].uv[2][1];
                   cloth[k].v12.z = 0.0f;
                   cloth[k].v12.w = 0.0f;
                  }
               }
           }
         // TEXCOORD4
         // CODE: 0x0504
         else if(p04 == 0x0504)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);
               }
           }
         // TEXCOORD5
         // CODE: 0x0505
         else if(p04 == 0x0505)
           {
            // warning messages
            if(p03 == 0x0500)
               if(!is_cloth) message("TEXCOORD5 is usually used for cloth meshes. This mesh may have anchorless cloth.");

            // process vertices
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // read data
                if(p03 == 0x0300) {
                   bs.read_real32();
                   bs.read_real32();
                   bs.read_real32();
                   bs.read_real32();
                  }
                else if(p03 == 0x0500)
                  {
                   // read indices
                   uint16 b1 = bs.read_uint08();
                   uint16 b2 = bs.read_uint08();
                   uint16 b3 = bs.read_uint08();
                   uint16 b4 = bs.read_uint08();

                   // copy to cloth
                   if(is_cloth)  {
                      cloth[k].v7.x = (float)b1;
                      cloth[k].v7.y = (float)b2;
                      cloth[k].v7.z = (float)b3;
                      cloth[k].v7.w = (float)b4;
                     }
                  }
                else
                   return error("Unknown vertex stream 0x0505 type.", __FILE__);
               }
           }
         // TANGENT
         // CODE: 0x0600
         else if(p04 == 0x0600)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.");

                // read data
                if(p03 == 0x0200) {
                   vb.data[k].tx = bs.read_real32();
                   vb.data[k].ty = bs.read_real32();
                   vb.data[k].tz = bs.read_real32();
                   vb.data[k].tw = 1.0f;
                  }
                else if(p03 == 0x0300) {
                   vb.data[k].tx = bs.read_real32();
                   vb.data[k].ty = bs.read_real32();
                   vb.data[k].tz = bs.read_real32();
                   vb.data[k].tw = bs.read_real32();
                  }
                else if(p03 == 0x0B00) {
                   vb.data[k].tx = bs.read_real16();
                   vb.data[k].ty = bs.read_real16();
                   vb.data[k].tz = bs.read_real16();
                   vb.data[k].tw = bs.read_real16();
                  }
                else
                   return error("Unknown vertex stream 0x0600 type.", __FILE__);

                // also copy to cloth
                if(is_cloth) {
                   cloth[k].v9.x = vb.data[k].tx;
                   cloth[k].v9.y = vb.data[k].ty;
                   cloth[k].v9.z = vb.data[k].tz;
                   cloth[k].v9.w = vb.data[k].tw;
                  }
               }
           }
         // BINORMAL
         // CODE: 0x0700
         else if(p04 == 0x0700)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // read data
                if(p03 == 0x0200) {
                   vb.data[k].bx = bs.read_real32();
                   vb.data[k].by = bs.read_real32();
                   vb.data[k].bz = bs.read_real32();
                   vb.data[k].bw = 1.0f;
                  }
                else if(p03 == 0x0300) {
                   vb.data[k].bx = bs.read_real32();
                   vb.data[k].by = bs.read_real32();
                   vb.data[k].bz = bs.read_real32();
                   vb.data[k].bw = bs.read_real32();
                  }
                else if(p03 == 0x0B00) {
                   vb.data[k].bx = bs.read_real16();
                   vb.data[k].by = bs.read_real16();
                   vb.data[k].bz = bs.read_real16();
                   vb.data[k].bw = bs.read_real16();
                  }
                else
                   return error("Unknown vertex stream 0x0700 type.", __FILE__);

                // also copy to cloth
                // when used for cloth, these are actually weights and not binormals
                if(is_cloth) {
                   cloth[k].v2.x = vb.data[k].bx;
                   cloth[k].v2.y = vb.data[k].by;
                   cloth[k].v2.z = vb.data[k].bz;
                   cloth[k].v2.w = vb.data[k].bw;
                  }
               }
           }
         // COLOR0
         // CODE: 0x0A00
         else if(p04 == 0x0A00)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);
               }
           }
         // COLOR1
         // CODE: 0x0A01
         else if(p04 == 0x0A01)
           {
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // read data
                real32 values[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                if(p03 == 0x0200) {
                   values[0] = bs.read_real32();
                   values[1] = bs.read_real32();
                   values[2] = bs.read_real32();
                   values[3] = 1.0f;
                  }
                else if(p03 == 0x0300) {
                   values[0] = bs.read_real32();
                   values[1] = bs.read_real32();
                   values[2] = bs.read_real32();
                   values[3] = bs.read_real32();
                  }
                else if(p03 == 0x0B00) {
                   values[0] = bs.read_real16();
                   values[1] = bs.read_real16();
                   values[2] = bs.read_real16();
                   values[3] = bs.read_real16();
                  }
                else if(p03 == 0x0D00) {
                   values[0] = (real32)(bs.read_uint08()/255.0f);
                   values[1] = (real32)(bs.read_uint08()/255.0f);
                   values[2] = (real32)(bs.read_uint08()/255.0f);
                   values[3] = (real32)(bs.read_uint08()/255.0f);
                  }
                else
                   return error("Unknown vertex stream 0x0A01 type.", __FILE__);

                // also copy to cloth
                // when used for cloth, these are actually weights and not binormals
                if(is_cloth) {
                   cloth[k].v3.x = values[0];
                   cloth[k].v3.y = values[1];
                   cloth[k].v3.z = values[2];
                   cloth[k].v3.w = values[3];
                  }
               }
           }
         // FOG
         // CODE: 0x0B00
         else if(p04 == 0x0B00)
           {
            // warning messages
            if(p03 == 0x0500)
               if(!is_cloth) message("FOG data type 0x0500 is usually used for cloth meshes. This mesh may have anchorless cloth.");

            // process vertices
            for(uint32 k = 0; k < n_vertices; k++)
               {
                // move to offset
                bs.seek(item.p02 + stride*k);
                if(bs.fail()) return error("Stream seek failure.", __FILE__);

                // read data
                if(p03 == 0x0300) {
                   real32 v1 = bs.read_real32(); // NEW! DW8 static geometry!
                   real32 v2 = bs.read_real32();
                   real32 v3 = bs.read_real32();
                   real32 v4 = bs.read_real32();
                  }
                else if(p03 == 0x0500)
                  {
                   // read indices
                   uint16 b1 = bs.read_uint08();
                   uint16 b2 = bs.read_uint08();
                   uint16 b3 = bs.read_uint08();
                   uint16 b4 = bs.read_uint08();

                   // copy to cloth
                   if(is_cloth)  {
                      cloth[k].v6.x = (float)b1;
                      cloth[k].v6.y = (float)b2;
                      cloth[k].v6.z = (float)b3;
                      cloth[k].v6.w = (float)b4;
                     }
                  }
                else if(p03 == 0x0D00) {
                   real32 v1 = (real32)(bs.read_uint08()/255.0f);
                   real32 v2 = (real32)(bs.read_uint08()/255.0f);
                   real32 v3 = (real32)(bs.read_uint08()/255.0f);
                   real32 v4 = (real32)(bs.read_uint08()/255.0f);
                  }
                else
                   return error("Unknown vertex stream 0x0B00 type.", __FILE__);
               }
           }
        }

     // transform cloth vertices
     if(is_cloth)
       {
        // initialize boolean buffer
        bool bbuffer[2] = { false, false };

        // initialize constant buffer
        static const int BUFFERSIZE = 0x200; // expanded to 0x200 from 0x100 as cloth control points have reached over 256 in 2018
        DWfloat4 cbuffer[BUFFERSIZE];
        for(size_t k = 0; k < BUFFERSIZE; k++) {
            cbuffer[k].x = 0.0f;
            cbuffer[k].y = 0.0f;
            cbuffer[k].z = 0.0f;
            cbuffer[k].w = 0.0f;
           }

       // initialize constant buffer data (mL2W)
       cbuffer[0x00].x = 1.0f; cbuffer[0x00].y = 0.0f; cbuffer[0x00].z = 0.0f; cbuffer[0x00].w = 0.0f; // #1
       cbuffer[0x01].x = 0.0f; cbuffer[0x01].y = 1.0f; cbuffer[0x01].z = 0.0f; cbuffer[0x01].w = 0.0f;
       cbuffer[0x02].x = 0.0f; cbuffer[0x02].y = 0.0f; cbuffer[0x02].z = 1.0f; cbuffer[0x02].w = 0.0f;
       cbuffer[0x03].x = 0.0f; cbuffer[0x03].y = 0.0f; cbuffer[0x03].z = 0.0f; cbuffer[0x03].w = 1.0f;
       cbuffer[0x04].x = 1.0f; cbuffer[0x04].y = 0.0f; cbuffer[0x04].z = 0.0f; cbuffer[0x04].w = 0.0f; // #2
       cbuffer[0x05].x = 0.0f; cbuffer[0x05].y = 1.0f; cbuffer[0x05].z = 0.0f; cbuffer[0x05].w = 0.0f;
       cbuffer[0x06].x = 0.0f; cbuffer[0x06].y = 0.0f; cbuffer[0x06].z = 1.0f; cbuffer[0x06].w = 0.0f;
       cbuffer[0x07].x = 0.0f; cbuffer[0x07].y = 0.0f; cbuffer[0x07].z = 0.0f; cbuffer[0x07].w = 1.0f;
       cbuffer[0x08].x = 1.0f; cbuffer[0x08].y = 0.0f; cbuffer[0x08].z = 0.0f; cbuffer[0x08].w = 0.0f; // #3
       cbuffer[0x09].x = 0.0f; cbuffer[0x09].y = 1.0f; cbuffer[0x09].z = 0.0f; cbuffer[0x09].w = 0.0f;
       cbuffer[0x0A].x = 0.0f; cbuffer[0x0A].y = 0.0f; cbuffer[0x0A].z = 1.0f; cbuffer[0x0A].w = 0.0f;
       cbuffer[0x0B].x = 0.0f; cbuffer[0x0B].y = 0.0f; cbuffer[0x0B].z = 0.0f; cbuffer[0x0B].w = 1.0f;
       cbuffer[0x0C].x = 1.0f; cbuffer[0x0C].y = 0.0f; cbuffer[0x0C].z = 0.0f; cbuffer[0x0C].w = 0.0f; // #4
       cbuffer[0x0D].x = 0.0f; cbuffer[0x0D].y = 1.0f; cbuffer[0x0D].z = 0.0f; cbuffer[0x0D].w = 0.0f;
       cbuffer[0x0E].x = 0.0f; cbuffer[0x0E].y = 0.0f; cbuffer[0x0E].z = 1.0f; cbuffer[0x0E].w = 0.0f;
       cbuffer[0x0F].x = 0.0f; cbuffer[0x0F].y = 0.0f; cbuffer[0x0F].z = 0.0f; cbuffer[0x0F].w = 1.0f;
       cbuffer[0x10].x = 1.0f; cbuffer[0x10].y = 0.0f; cbuffer[0x10].z = 0.0f; cbuffer[0x10].w = 0.0f; // #5
       cbuffer[0x11].x = 0.0f; cbuffer[0x11].y = 1.0f; cbuffer[0x11].z = 0.0f; cbuffer[0x11].w = 0.0f;
       cbuffer[0x12].x = 0.0f; cbuffer[0x12].y = 0.0f; cbuffer[0x12].z = 1.0f; cbuffer[0x12].w = 0.0f;
       cbuffer[0x13].x = 0.0f; cbuffer[0x13].y = 0.0f; cbuffer[0x13].z = 0.0f; cbuffer[0x13].w = 1.0f;
       cbuffer[0x14].x = 1.0f; cbuffer[0x14].y = 0.0f; cbuffer[0x14].z = 0.0f; cbuffer[0x14].w = 0.0f; // #6
       cbuffer[0x15].x = 0.0f; cbuffer[0x15].y = 1.0f; cbuffer[0x15].z = 0.0f; cbuffer[0x15].w = 0.0f;
       cbuffer[0x16].x = 0.0f; cbuffer[0x16].y = 0.0f; cbuffer[0x16].z = 1.0f; cbuffer[0x16].w = 0.0f;
       cbuffer[0x17].x = 0.0f; cbuffer[0x17].y = 0.0f; cbuffer[0x17].z = 0.0f; cbuffer[0x17].w = 1.0f;
       cbuffer[0x18].x = 1.0f; cbuffer[0x18].y = 0.0f; cbuffer[0x18].z = 0.0f; cbuffer[0x18].w = 0.0f; // #7
       cbuffer[0x19].x = 0.0f; cbuffer[0x19].y = 1.0f; cbuffer[0x19].z = 0.0f; cbuffer[0x19].w = 0.0f;
       cbuffer[0x1A].x = 0.0f; cbuffer[0x1A].y = 0.0f; cbuffer[0x1A].z = 1.0f; cbuffer[0x1A].w = 0.0f;
       cbuffer[0x1B].x = 0.0f; cbuffer[0x1B].y = 0.0f; cbuffer[0x1B].z = 0.0f; cbuffer[0x1B].w = 1.0f;
       cbuffer[0x1C].x = 1.0f; cbuffer[0x1C].y = 0.0f; cbuffer[0x1C].z = 0.0f; cbuffer[0x1C].w = 0.0f; // #8
       cbuffer[0x1D].x = 0.0f; cbuffer[0x1D].y = 1.0f; cbuffer[0x1D].z = 0.0f; cbuffer[0x1D].w = 0.0f;
       cbuffer[0x1E].x = 0.0f; cbuffer[0x1E].y = 0.0f; cbuffer[0x1E].z = 1.0f; cbuffer[0x1E].w = 0.0f;
       cbuffer[0x1F].x = 0.0f; cbuffer[0x1F].y = 0.0f; cbuffer[0x1F].z = 0.0f; cbuffer[0x1F].w = 1.0f;

       // initialize constant buffer data (mW2P)
       static const int MW2P_INDEX = 500; // changed to 500 from 200 in 2018, we need more CP! 96 + k where k is now over 256
       cbuffer[MW2P_INDEX + 0].x = 1.0f; cbuffer[MW2P_INDEX + 0].y = 0.0f; cbuffer[MW2P_INDEX + 0].z = 0.0f; cbuffer[MW2P_INDEX + 0].w = 0.0f;
       cbuffer[MW2P_INDEX + 1].x = 0.0f; cbuffer[MW2P_INDEX + 1].y = 1.0f; cbuffer[MW2P_INDEX + 1].z = 0.0f; cbuffer[MW2P_INDEX + 1].w = 0.0f;
       cbuffer[MW2P_INDEX + 2].x = 0.0f; cbuffer[MW2P_INDEX + 2].y = 0.0f; cbuffer[MW2P_INDEX + 2].z = 1.0f; cbuffer[MW2P_INDEX + 2].w = 0.0f;
       cbuffer[MW2P_INDEX + 3].x = 0.0f; cbuffer[MW2P_INDEX + 3].y = 0.0f; cbuffer[MW2P_INDEX + 3].z = 0.0f; cbuffer[MW2P_INDEX + 3].w = 1.0f;

       // initialize constant buffer data (bMatIdx)
       static const int BMATIDX_INDEX = 504; // changed to 504 from 204 in 2018
       cbuffer[BMATIDX_INDEX].x = 0;
       cbuffer[BMATIDX_INDEX].y = 0;
       cbuffer[BMATIDX_INDEX].z = 0;
       cbuffer[BMATIDX_INDEX].w = 0;

       // transfer control point data
       for(uint32 k = 0; k < n_cp; k++) {
           size_t index = 96 + k;
           if(index > MW2P_INDEX) return error("Cloth control points have gone out of bounds. We need to allocate more room in the cloth shader.");
           cbuffer[index].x = cpdata[k].x;
           cbuffer[index].y = cpdata[k].y;
           cbuffer[index].z = cpdata[k].z;
           cbuffer[index].w = cpdata[k].w;
          }

        // transform vertices
        for(size_t k = 0; k < n_vertices; k++) {
            DWCLOTHOUTPUT output;
            TransformClothVertex2(cloth[k], &cbuffer[0], BUFFERSIZE, &bbuffer[0], 2, output);
            vb.data[k].vx = output.o0.x;
            vb.data[k].vy = output.o0.y;
            vb.data[k].vz = output.o0.z;
            vb.data[k].vw = output.o0.w;
            vb.data[k].uv[0][0] = output.o2.x;
            vb.data[k].uv[0][1] = output.o2.y;
            vb.data[k].uv[1][0] = output.o3.x;
            vb.data[k].uv[1][1] = output.o3.y;
            vb.data[k].uv[2][0] = output.o4.x;
            vb.data[k].uv[2][1] = output.o4.y;
           }
       }

     // insert vertex buffer
     amc.vblist.push_back(vb);
    }

 // success
 return true;
}

// index buffers
bool buildPhase4(MODELDATA& md, ADVANCEDMODELCONTAINER& amc)
{
 // get index buffer data
 G1MG0107DATA& data = md.data0107;
 if(!data.elem) return true;

 // get surface information
 G1MG0108DATA& data08 = md.data0108;
 if(!data08.elem) return true;

 // for each surface, we need to know what the format of the index buffer is
 map<uint32, uint32> format_map;
 for(size_t i = 0; i < data08.elem; i++) {
     auto item = data08.data[i];
     auto ib_ref = item[0x07];
     auto ib_fmt = item[0x09];
     if(ib_fmt == 0x03) ; // triangle list
     else if(ib_fmt == 0x04) ; // triangle strip
     else return error("Unknown index buffer format.", __LINE__);
     typedef map<uint32, uint32>::value_type value_type;
     format_map.insert(value_type(ib_ref, ib_fmt));
    }

 // for each index buffer
 for(size_t i = 0; i < data.elem; i++)
    {
     // create mesh name
     G1MG0107ITEM& item = data.data[i];
     stringstream ss;
     ss << "ibuffer_" << setfill('0') << setw(4) << hex << i << dec;

     // get index buffer format
     auto fmtmap_iter = format_map.find(i);
     if(fmtmap_iter == format_map.end()) {
        stringstream dss;
        dss << "Index buffer reference 0x" << hex << i << dec << " lookup failed.";
        return error(dss.str().c_str(), __LINE__);
       }

     // save index buffer
     AMC_IDXBUFFER ib;
     if(fmtmap_iter->second == 0x3) ib.type = AMC_TRIANGLES;
     else if(fmtmap_iter->second == 0x4) ib.type = AMC_TRISTRIPCUT; // AMC_TRISTRIP; // sometimes AMC_TRISTRIPCUT
     else return error("Unknown index buffer format.", __LINE__);
     ib.name = ss.str();
     ib.wind = AMC_CW;
     ib.elem = item.elem;
     ib.data = item.data;

     // save index buffer format
     if(item.type == 0x08) ib.format = AMC_UINT08;
     else if(item.type == 0x10) ib.format = AMC_UINT16;
     else if(item.type == 0x20) ib.format = AMC_UINT32;
     else return error("Unknown index buffer data type.");

     // insert index buffer
     amc.iblist.push_back(ib);
    }

 return true;
}

// surfaces
bool buildPhase5(MODELDATA& md, ADVANCEDMODELCONTAINER& amc)
{
 // nothing to do
 G1MG0108DATA& data = md.data0108;
 if(!data.elem) return true;

 // process surfaces
 for(uint32 i = 0; i < data.elem; i++)
    {
     // validate input layout index
     boost::shared_array<uint32> item = data.data[i];
     uint32 il_index = item[0x01];
     if(!(il_index < md.data0105.elem)) return error("Input layout reference out of range."); 

     // validate vertex buffer index
     uint32 vb_index = md.data0105.data[il_index].refs[0];
     if(!(vb_index < md.data0104.elem)) return error("Vertex buffer reference out of range.");

     // validate number of vertices
     uint32 n_vertex = md.data0104.data[vb_index].p03;
     uint32 min_vertex_index = item[0x0A];
     uint32 max_vertex_index = item[0x0A] + item[0x0B];
     if(max_vertex_index > n_vertex) {
        stringstream ss;
        ss << "Vertex reference out of range. ";
        ss << "0x" << hex << item[0x0A] << dec << " + ";
        ss << "0x" << hex << item[0x0B] << " is greater than ";
        ss << "0x" << hex << n_vertex << dec << " where vertex buffer index = ";
        ss << "0x" << hex << vb_index << dec << ".";
        return error(ss);
       }

     // create surface name
     stringstream ss;
     ss << "surface_" << setfill('0') << setw(3) << i;

     // create surface
     AMC_SURFACE surface;
     surface.name = ss.str();
     surface.surfmat = static_cast<uint16>(item[0x06]);
     AMC_REFERENCE ref;
     ref.vb_index = il_index;   // vbuffer index (use input layout since non-interleaved buffers possible)
     ref.vb_start = 0;          // use full vertex buffer
     ref.vb_width = n_vertex;   // use full vertex buffer
     ref.ib_index = item[0x07]; // ibuffer index
     ref.ib_start = item[0x0C]; // ibuffer start index
     ref.ib_width = item[0x0D]; // number of indices
     ref.jm_index = item[0x02]; // joint map index
     surface.refs.push_back(ref);
     
     // save surface
     amc.surfaces.push_back(surface);
    }

 return true;
}

#pragma endregion

#pragma region REGION_CHUNK_PROCESSING

bool processG1MS(MODELDATA& md)
{
 // debug information (always display so that we can move incorrectly placed parts)
 md.dfile << "-----------" << endl;
 md.dfile << " SKELETONS " << endl;
 md.dfile << "-----------" << endl;
 md.dfile << endl;
 md.dfile << "number of skeletons = 0x" << hex << md.G1MS_list.size() << dec << endl;
 md.dfile << endl;

 for(size_t i = 0; i < md.G1MS_list.size(); i++)
    {
     // read chunk properties
     binary_stream bs = md.G1MS_list[i];
     uint32 chunkname = bs.read_uint32();
     uint32 chunkvers = bs.read_uint32();
     uint32 chunksize = bs.read_uint32();
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     if(debug) {
        md.dfile << "SKELETON[0x" << setfill('0') << setw(2) << i << "]" << endl;
        md.dfile << " HEADER" << endl;
        md.dfile << "  chunkname = 0x" << setfill('0') << setw(8) << hex << chunkname << dec << endl;
        md.dfile << "  chunkvers = 0x" << setfill('0') << setw(8) << hex << chunkvers << dec << endl;
        md.dfile << "  chunksize = 0x" << setfill('0') << setw(8) << hex << chunksize << dec << endl;
       }

     // read skeleton parameters
     uint32 p01 = bs.read_uint32(); // offset to joint data
     uint32 p02 = bs.read_uint32(); // ????
     uint16 p03 = bs.read_uint16(); // number of joints in this file
     uint16 p04 = bs.read_uint16(); // number of joints indices
     uint16 p05 = bs.read_uint16(); // number of 0x8000 parents
     uint16 p06 = bs.read_uint16(); // 0x0000
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     if(debug) {
        md.dfile << " PARAMETERS" << endl;
        md.dfile << "  p01 = 0x" << setfill('0') << setw(8) << hex << p01 << dec << " (offset to joint data)" << endl;
        md.dfile << "  p02 = 0x" << setfill('0') << setw(8) << hex << p02 << dec << " (????)" << endl;
        md.dfile << "  p03 = 0x" << setfill('0') << setw(4) << hex << p03 << dec << " (number of joints)" << endl;
        md.dfile << "  p04 = 0x" << setfill('0') << setw(4) << hex << p04 << dec << " (number of joint indices)" << endl;
        md.dfile << "  p05 = 0x" << setfill('0') << setw(4) << hex << p05 << dec << " (number of joints under roots?)" << endl;
        md.dfile << "  p06 = 0x" << setfill('0') << setw(4) << hex << p06 << dec << " (0x00)" << endl;
       }

     // read identifiers
     boost::shared_array<uint16> idlist(new uint16[p04]);
     bs.read_array(idlist.get(), p04);
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     if(debug) {
        md.dfile << " JOINT IDENTIFIERS" << endl;
        for(uint32 j = 0; j < p04; j++) {
            md.dfile << "  id[0x" << hex << setfill('0') << setw(4) << j << dec << "] = ";
            md.dfile << "0x" << hex << setfill('0') << setw(4) << idlist[j] << endl;
           }
       }

     // if there are joints and this is a regular skeleton chunk
     // EDIT: in some models, p03 can be less than p04 and there
     // can still be a valid skeleton
     if(p03 && p04)
       {
        // seek joints
        bs.seek(p01);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);

        // create skeleton name
        stringstream ss;
        ss << "skeleton_";
        ss << setfill('0') << setw(2) << hex << i << dec << endl;

        // initialize skeleton
        AMC_SKELETON2 skel;
        skel.format = AMC_JOINT_FORMAT_RELATIVE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
        skel.name = ss.str();

        // read joints
        struct DWJOINTITEM {
          uint32 id;
          real32 scale[3];
          uint32 parent;
          real32 rotation[4];
          real32 position[4];
        };
        deque<DWJOINTITEM> jointlist;
        for(uint32 j = 0; j < p04; j++) {
            if(idlist[j] == 0xFFFF) continue;
            DWJOINTITEM item;
            item.id = idlist[j];
            item.scale[0]    = bs.read_real32();
            item.scale[1]    = bs.read_real32();
            item.scale[2]    = bs.read_real32();
            item.parent      = bs.read_uint32();
            item.rotation[0] = bs.read_real32();
            item.rotation[1] = bs.read_real32();
            item.rotation[2] = bs.read_real32();
            item.rotation[3] = bs.read_real32();
            item.position[0] = bs.read_real32();
            item.position[1] = bs.read_real32();
            item.position[2] = bs.read_real32();
            item.position[3] = bs.read_real32();
            if(bs.fail()) return error("Stream read failure.", __LINE__);
            jointlist.push_back(item);
           }

        // debug
        if(debug) {
           md.dfile << " JOINT DATA" << endl;
           for(uint32 j = 0; j < jointlist.size(); j++) {
               uint32 sx = *reinterpret_cast<uint32*>(&jointlist[j].scale[0]);
               uint32 sy = *reinterpret_cast<uint32*>(&jointlist[j].scale[1]);
               uint32 sz = *reinterpret_cast<uint32*>(&jointlist[j].scale[2]);
               uint32 qx = *reinterpret_cast<uint32*>(&jointlist[j].rotation[0]);
               uint32 qy = *reinterpret_cast<uint32*>(&jointlist[j].rotation[1]);
               uint32 qz = *reinterpret_cast<uint32*>(&jointlist[j].rotation[2]);
               uint32 qw = *reinterpret_cast<uint32*>(&jointlist[j].rotation[3]);
               uint32 px = *reinterpret_cast<uint32*>(&jointlist[j].position[0]);
               uint32 py = *reinterpret_cast<uint32*>(&jointlist[j].position[1]);
               uint32 pz = *reinterpret_cast<uint32*>(&jointlist[j].position[2]);
               uint32 pw = *reinterpret_cast<uint32*>(&jointlist[j].position[3]);
               md.dfile << hex << setfill('0');
               md.dfile << "  joint 0x" << setw(4) << jointlist[j].id << ":";
               md.dfile << " <" << setw(8) << sx << " " << setw(8) << sy << " " << setw(8) << sz << ">";
               md.dfile << " <" << setfill('0') << setw(8) << jointlist[j].parent << ">";
               md.dfile << " <" << setw(8) << qx << " " << setw(8) << qy << " " << setw(8) << qz << " " << setw(8) << qw << ">";
               md.dfile << " <" << setw(8) << px << " " << setw(8) << py << " " << setw(8) << pz << " " << setw(8) << pw << ">";
               md.dfile << dec << endl;
              }
           md.dfile << endl;
          }

        // test for attachment system
        bool attachments = false;
        for(uint32 j = 0; j < jointlist.size(); j++) {
            uint32 parent = jointlist[j].parent;
            uint32 hi = (parent & 0xFFFF0000ul) >> 16;
            uint32 lo = (parent & 0x0000FFFFul);
            if(hi == 0xFFFF) continue;
            if(hi == 0x0000) continue;
            attachments = true;
            break;
           }

        // if there are attachments usually p03 != p04 and the skeleton is usually somewhere else
        // if(p03 != p04) message(" p03 != p04");
        // if(attachments) message(" attachments");

        if(attachments)
          {
          }
        else
          {
           // process joints
           for(uint32 j = 0; j < jointlist.size(); j++)
              {
               // parent
               uint32 parent = jointlist[j].parent;
               if(parent == 0xFFFFFFFF) parent = AMC_INVALID_JOINT;
           
               // local rotation
               real32 qx = jointlist[j].rotation[0];
               real32 qy = jointlist[j].rotation[1];
               real32 qz = jointlist[j].rotation[2];
               real32 qw = jointlist[j].rotation[3];
           
               // local position
               real32 px = jointlist[j].position[0];
               real32 py = jointlist[j].position[1];
               real32 pz = jointlist[j].position[2];
               real32 pw = jointlist[j].position[3];
           
               // rotation matrix
               cs::math::quaternion<real32> Q(qw, qx, qy, qz);
               cs::math::matrix4x4<real32> R;
               cs::math::quaternion_to_matrix4x4(R.get(), &Q.v[0]);

               // create joint name
               stringstream ss;
               ss << "jnt_" << setfill('0') << setw(3) << jointlist[j].id;
           
               // create joint
               AMC_JOINT joint;
               joint.name = ss.str();
               joint.id = jointlist[j].id;
               joint.parent = parent;
               joint.m_rel[0x0] = R[0x0];
               joint.m_rel[0x1] = R[0x1];
               joint.m_rel[0x2] = R[0x2];
               joint.m_rel[0x3] = px; // R[0x3];
               joint.m_rel[0x4] = R[0x4];
               joint.m_rel[0x5] = R[0x5];
               joint.m_rel[0x6] = R[0x6];
               joint.m_rel[0x7] = py; // R[0x7];
               joint.m_rel[0x8] = R[0x8];
               joint.m_rel[0x9] = R[0x9];
               joint.m_rel[0xA] = R[0xA];
               joint.m_rel[0xB] = pz; // R[0xB];
               joint.m_rel[0xC] = R[0xC];
               joint.m_rel[0xD] = R[0xD];
               joint.m_rel[0xE] = R[0xE];
               joint.m_rel[0xF] = R[0xF];
               joint.p_rel[0] = px;
               joint.p_rel[1] = py;
               joint.p_rel[2] = pz;
               joint.p_rel[3] = 1.0f;
               skel.joints.push_back(joint);
              }
           
           // insert skeleton into model container
           ADVANCEDMODELCONTAINER amc;
           amc.skllist2.push_back(skel);

           // convert to absolute matrix
           AMC_SKELETON2 skel_abs;
           SkeletonRelativeToAbsolute(skel_abs, skel);
           md.SKEL_list.push_back(skel_abs);

           // debug
           md.dfile << " JOINT DATA (ABSOLUTE)" << endl;
           for(uint32 j = 0; j < skel_abs.joints.size(); j++) {
               md.dfile << hex << setfill('0');
               md.dfile << "  joint[0x" << setw(4) << j << "]: ";
               md.dfile << chop(skel_abs.joints[j].m_abs[0x0]) << ", " << chop(skel_abs.joints[j].m_abs[0x1]) << ", " << chop(skel_abs.joints[j].m_abs[0x2]) << ", " << chop(skel_abs.joints[j].m_abs[0x3]) << ", ";
               md.dfile << chop(skel_abs.joints[j].m_abs[0x4]) << ", " << chop(skel_abs.joints[j].m_abs[0x5]) << ", " << chop(skel_abs.joints[j].m_abs[0x6]) << ", " << chop(skel_abs.joints[j].m_abs[0x7]) << ", ";
               md.dfile << chop(skel_abs.joints[j].m_abs[0x8]) << ", " << chop(skel_abs.joints[j].m_abs[0x9]) << ", " << chop(skel_abs.joints[j].m_abs[0xA]) << ", " << chop(skel_abs.joints[j].m_abs[0xB]) << ", ";
               md.dfile << chop(skel_abs.joints[j].m_abs[0xC]) << ", " << chop(skel_abs.joints[j].m_abs[0xD]) << ", " << chop(skel_abs.joints[j].m_abs[0xE]) << ", " << chop(skel_abs.joints[j].m_abs[0xF]) << endl;
               real32 euler[3];
               matrix4x4_to_euler_XYZ(euler, skel_abs.joints[j].m_abs);
               euler[0] = chop(euler[0]);
               euler[1] = chop(euler[1]);
               euler[2] = chop(euler[2]);
               md.dfile << "  rotation XYZ[0x" <<  setw(4) << j << "]: ";
               md.dfile << cs::math::degrees(euler[0]) << ", " << cs::math::degrees(euler[1]) << ", " << cs::math::degrees(euler[2]) << endl;
               md.dfile << "  translation[0x" <<  setw(4) << j << "]: ";
               md.dfile << skel_abs.joints[j].m_abs[3] << ", " << skel_abs.joints[j].m_abs[7] << ", " << skel_abs.joints[j].m_abs[11] << endl;
               md.dfile << endl;
              }
           md.dfile << endl;

           // save AMC file
           STDSTRINGSTREAM fnss;
           fnss << md.shrtname << TEXT("_skel_") << setfill(TCHAR('0')) << setw(2) << i;
           SaveAMC(md.pathname.c_str(), fnss.str().c_str(), amc);
          }
       }
    }

 return true;
}

bool processG1MM(MODELDATA& md)
{
 // skip
 if(md.G1MM_list.size() == 0) return true;

 // if there is a G1MM section, there should only be one
 if(md.G1MM_list.size() != 1)
    return error("G1M files should only have one G1MM (matrix list) section.", __LINE__);

 // DEBUG BEGIN
 if(debug) {
    md.dfile << "------" << endl;
    md.dfile << " G1MM " << endl;
    md.dfile << "------" << endl;
    md.dfile << endl;
    md.dfile << "number of G1MM chunks = 0x" << hex << md.G1MM_list.size() << dec << endl;
    md.dfile << endl;
   }
 // DEBUG END

 // read chunk properties
 binary_stream bs = md.G1MM_list[0];
 uint32 chunkname = bs.read_uint32();
 uint32 chunkvers = bs.read_uint32();
 uint32 chunksize = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // debug chunk properties
 if(debug) {
    md.dfile << "MATRIX[0x" << setfill('0') << setw(2) << 0 << "]" << endl;
    md.dfile << " HEADER" << endl;
    md.dfile << "  chunkname = 0x" << setfill('0') << setw(8) << hex << chunkname << dec << endl;
    md.dfile << "  chunkvers = 0x" << setfill('0') << setw(8) << hex << chunkvers << dec << endl;
    md.dfile << "  chunksize = 0x" << setfill('0') << setw(8) << hex << chunksize << dec << endl;
   }

 // read number of entries
 uint32 n_entries = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // nothing to do
 if(!n_entries) return true;

 // read matrices
 boost::shared_array<boost::shared_array<real32>> data(new boost::shared_array<real32>[n_entries]);
 for(uint32 i = 0; i < n_entries; i++) {
     boost::shared_array<real32> item(new real32[16]);
     bs.read_array(item.get(), 16);
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     for(uint32 j = 0; j < 16; j++) item[j] = chop(item[j]);
     data[i] = item;
    }

 // debug matrices
 if(debug) {
    for(uint32 i = 0; i < n_entries; i++) {
        md.dfile << hex << setfill('0') << "  ";
        md.dfile << "matrix[0x" << setw(2) << i << "] =";
        md.dfile << " <" << data[i][0x0] << ", " << data[i][0x1] << ", " << data[i][0x2] << ", " << data[i][0x3] << ">";
        md.dfile << " <" << data[i][0x4] << ", " << data[i][0x5] << ", " << data[i][0x6] << ", " << data[i][0x7] << ">";
        md.dfile << " <" << data[i][0x8] << ", " << data[i][0x9] << ", " << data[i][0xA] << ", " << data[i][0xB] << ">";
        md.dfile << " <" << data[i][0xC] << ", " << data[i][0xD] << ", " << data[i][0xE] << ", " << data[i][0xF] << ">";
        md.dfile << dec << endl;
       }
    md.dfile << endl;
   }

 return true;
}

bool processNUNO(MODELDATA& md)
{
 // skip
 if(md.NUNO_list.size() == 0) return true;

 // DEBUG BEGIN
 if(debug) {
    md.dfile << "------" << endl;
    md.dfile << " NUNO " << endl;
    md.dfile << "------" << endl;
    md.dfile << endl;
    md.dfile << "number of NUNO chunks = 0x" << hex << md.NUNO_list.size() << dec << endl;
    md.dfile << endl;
   }
 // DEBUG END

 // number of NUNO chunks must be 0 or 1
 uint32 n_NUNO = md.NUNO_list.size();
 if(n_NUNO != 0 && n_NUNO != 1) return error("Number of NUNOs must be 0 or 1.", __LINE__);

 // read header
 binary_stream& bs = md.NUNO_list[0];
 uint32 h01 = bs.read_uint32(); // 0x4E554E4F (NUNO)
 uint32 h02 = bs.read_uint32(); // 0x30303234 (0024)
 uint32 h03 = bs.read_uint32(); // chunksize
 uint32 h04 = bs.read_uint32(); // number of subchunks
 if(bs.fail()) return error("Stream read failure.", __LINE__);
 if(debug) {
    md.dfile << "HEADER" << endl;
    md.dfile << hex << setfill('0');
    md.dfile << " chunkname = 0x" << setw(8) << h01 << endl;
    md.dfile << " chunkvers = 0x" << setw(8) << h02 << endl;
    md.dfile << " chunksize = 0x" << setw(8) << h03 << endl;
    md.dfile << " subchunks = 0x" << setw(8) << h04 << endl;
    md.dfile << dec;
   }

 // validate header
 if(h01 != 0x4E554E4F) return error("Not a NUNO chunk.", __LINE__);
 switch(h02) {
   case(0x30303234) : break;
   case(0x30303235) : break; // [2017] Warriors All-Stars
   case(0x30303236) : break; // [2017] Warriors All-Stars
   default : return error("Unsupported NUNO version.", __LINE__);
  }
 if(h03 == 0) return error("Invalid NUNO chunksize.", __LINE__);
 if(h04 == 0) return error("Invalid number of NUNO subchunks.", __LINE__);

 // save offsets to subchunks
 binary_stream::size_type s00030001_offset = 0x00;
 binary_stream::size_type s00030002_offset = 0x00;
 binary_stream::size_type s00030003_offset = 0x00;

 // scan subchunks
 if(debug) md.dfile << "SUBCHUNK LIST" << endl;
 for(uint32 i = 0; i < h04; i++)
    {
     // read subchunk properties
     uint32 subchunk_type = bs.read_uint32();
     uint32 subchunk_size = bs.read_uint32();
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     if(debug) {
        md.dfile << hex << setfill('0');
        md.dfile << " SUBCHUNK[0x" << i << "]" << endl;
        md.dfile << "  type = 0x" << setw(8) << subchunk_type << endl;
        md.dfile << "  size = 0x" << setw(8) << subchunk_size << endl;
        md.dfile << dec;
       }

     // save offset
     switch(subchunk_type) {
       case(0x00030001) : s00030001_offset = bs.tell(); break;
       case(0x00030002) : s00030002_offset = bs.tell(); break;
       case(0x00030003) : s00030003_offset = bs.tell(); break; // [PC] Warriors All-Stars (2017)
       default : return error("Unsupported NUNO subchunk type.", __LINE__);
      }

     // move to next subchunk
     bs.move(subchunk_size - 0x08);
     if(bs.fail()) return error("Stream seek failure.", __LINE__);
    }

 // if there is an 0x00030001 subchunk
 if(s00030001_offset)
   {
    // move to 0x00030001 subchunk
    bs.seek(s00030001_offset);
    if(bs.fail()) return error("Stream seek failure.", __LINE__);
   
    // read number of 0x00030001 entries
    uint32 n_entries = bs.read_uint32();
    if(bs.fail()) return error("Stream read failure.", __LINE__);

    // debug
    if(debug) {
       md.dfile << "SUBCHUNK DATA 0x00030001" << endl;
       md.dfile << hex << setfill('0');
       md.dfile << " number of entries = 0x" << setw(8) << n_entries << endl;
       md.dfile << dec;
      }

    // for each 0x00030001 entry
    for(uint32 i = 0; i < n_entries; i++)
       {
        // read entry header
        NUNOSUBCHUNK0301ENTRY item;
        item.h01 = bs.read_uint32();
        item.h02 = bs.read_uint32();
        item.h03 = bs.read_uint32();
        item.h04 = bs.read_uint32();
        item.h05 = bs.read_uint32();
        item.h06 = bs.read_uint32();
        item.h07 = bs.read_real32();
        item.h08 = bs.read_real32();
        item.h09 = bs.read_uint32();
        bs.read_array(&item.h10[0], 4);
        bs.read_array(&item.h11[0], 4);
        item.h12 = bs.read_real32();
        item.h13 = bs.read_uint32();
        bs.read_array(&item.h14[0], 3);
        bs.read_array(&item.h15[0], 3);
        if(h02 >= 0x30303235) bs.read_array(&item.h16[0], 4);
        if(bs.fail()) return error("Stream read failure.", __LINE__);
   
        // debug
        if(debug) {
           md.dfile << setfill('0');
           md.dfile << " ENTRY[0x" << hex << i << dec << "]" << endl;
           md.dfile << "  h01 = 0x" << hex << setw(8) << item.h01 << dec << " ()" << endl;
           md.dfile << "  h02 = 0x" << hex << setw(8) << item.h02 << dec << " (number of control points)" << endl;
           md.dfile << "  h03 = 0x" << hex << setw(8) << item.h03 << dec << " ()" << endl;
           md.dfile << "  h04 = 0x" << hex << setw(8) << item.h04 << dec << " ()" << endl;
           md.dfile << "  h05 = 0x" << hex << setw(8) << item.h05 << dec << " ()" << endl;
           md.dfile << "  h06 = 0x" << hex << setw(8) << item.h06 << dec << " ()" << endl;
           md.dfile << "  h07 = " << item.h07 << " ()" << endl;
           md.dfile << "  h08 = " << item.h08 << " ()" << endl;
           md.dfile << "  h09 = 0x" << hex << setw(8) << item.h09 << dec << " ()" << endl;
           md.dfile << "  h10 = <" << item.h10[0] << ", " << item.h10[1] << ", " << item.h10[2] << ", " << item.h10[3] << ">" << " ()" << endl;
           md.dfile << "  h11 = <" << item.h11[0] << ", " << item.h11[1] << ", " << item.h11[2] << ", " << item.h11[3] << ">" << " ()" << endl;
           md.dfile << "  h12 = " << item.h12 << " ()" << endl;
           md.dfile << "  h13 = 0x" << hex << item.h13 << dec << " ()" << endl;
           md.dfile << "  h14 = <" << item.h14[0] << ", " << item.h14[1] << ", " << item.h14[2] << ">" << " ()" << endl;
           md.dfile << "  h15 = <" << item.h15[0] << ", " << item.h15[1] << ", " << item.h15[2] << ">" << " ()" << endl;
           if(h02 >= 0x30303235) md.dfile << "  h16 = <" << item.h16[0] << ", " << item.h16[1] << ", " << item.h16[2] << ", " << item.h16[3] << ">" << " ()" << endl;
          }

        // the number of control points must be valid
        if(!item.h02) return error("The number of NUNO control points cannot be zero.", __LINE__);
   
        // read control points
        item.p01.reset(new NUNO0301SUBENTRY1[item.h02]);
        for(uint32 j = 0; j < item.h02; j++) {
            item.p01[j].p01 = bs.read_real32();
            item.p01[j].p02 = bs.read_real32();
            item.p01[j].p03 = bs.read_real32();
            item.p01[j].p04 = bs.read_real32();
            if(bs.fail()) return error("Stream read failure.", __LINE__);
           }
        // debug
        if(debug) {
           md.dfile << "  CONTROL POINTS" << endl;
           for(uint32 j = 0; j < item.h02; j++) {
               md.dfile << "   point[0x" << hex << j << dec << "] = ";
               md.dfile << "<" << item.p01[j].p01 << ", " << item.p01[j].p02 << ", " << item.p01[j].p03 << ", " << item.p01[j].p04 << ">" << endl;
              }
          }

        // read influence data
        item.p02.reset(new NUNOSUBCHUNK0301ENTRY_DATATYPE1[item.h02]);
        for(uint32 j = 0; j < item.h02; j++) {
            item.p02[j].p01 = bs.read_uint32(); // index to another point
            item.p02[j].p02 = bs.read_uint32(); // index to another point
            item.p02[j].p03 = bs.read_uint32(); // index to another point
            item.p02[j].p04 = bs.read_uint32(); // index to another point
            item.p02[j].p05 = bs.read_real32(); // ???
            item.p02[j].p06 = bs.read_real32(); // ???
            if(bs.fail()) return error("Stream read failure.", __LINE__);
           }
        // debug
        if(debug) {
           md.dfile << "  INFLUENCE DATA" << endl;
           for(uint32 j = 0; j < item.h02; j++) {
               md.dfile << "   influence[0x" << hex << j << dec << "] = ";
               md.dfile << setfill('0');
               md.dfile << "<" << setw(8) << hex << item.p02[j].p01 << dec << ", " <<
                                  setw(8) << hex << item.p02[j].p02 << dec << ", " <<
                                  setw(8) << hex << item.p02[j].p03 << dec << ", " <<
                                  setw(8) << hex << item.p02[j].p04 << dec << "> - ";
               md.dfile << "<" << item.p02[j].p05 << ", " << item.p02[j].p06 << ">";
               md.dfile << endl;
              }
          }

        // read ??? data
        item.p03.reset(new NUNOSUBCHUNK0301ENTRY_DATATYPE2[item.h03]);
        for(uint32 j = 0; j < item.h03; j++) {
            bs.read_array(&item.p03[j].p01[0], 4);
            bs.read_array(&item.p03[j].p02[0], 4);
            item.p03[j].p03 = bs.read_uint32();
            item.p03[j].p04 = bs.read_uint32();
            item.p03[j].p05 = bs.read_uint32();
            item.p03[j].p06 = bs.read_uint32();
            if(bs.fail()) return error("Stream read failure.", __LINE__);
           }
        // debug
        if(debug) {
           md.dfile << "  UNKNOWN DATA" << endl;
           for(uint32 j = 0; j < item.h03; j++) {
               md.dfile << "   unknown[0x" << hex << j << dec << "] = ";
               md.dfile << setfill('0');
               md.dfile << setw(8) << hex << item.p03[j].p03 << dec << " ";
               md.dfile << setw(8) << hex << item.p03[j].p04 << dec << " ";
               md.dfile << setw(8) << hex << item.p03[j].p05 << dec << " ";
               md.dfile << setw(8) << hex << item.p03[j].p06 << dec << " ";
               md.dfile << "<" << item.p03[j].p01[0] << ", " <<
                                  item.p03[j].p01[1] << ", " <<
                                  item.p03[j].p01[2] << ", " <<
                                  item.p03[j].p01[3] << ">";
               md.dfile << " ";
               md.dfile << "<" << item.p03[j].p02[0] << ", " <<
                                  item.p03[j].p02[1] << ", " <<
                                  item.p03[j].p02[2] << ", " <<
                                  item.p03[j].p02[3] << ">";
               md.dfile << endl;
              }
          }

        // read 1st index set
        if(item.h04) {
           item.p04.reset(new uint32[item.h04]);
           bs.read_array(&item.p04[0], item.h04);
           if(bs.fail()) return error("Stream read failure.", __LINE__);
          }
        // debug
        if(debug) {
           md.dfile << "  INDEX SET #1" << endl;
           for(uint32 j = 0; j < item.h04; j++) {
               md.dfile << "   index[0x" << hex << j << dec << "] = ";
               md.dfile << setfill('0') << setw(8) << hex << item.p04[j] << dec << endl;
              }
          }

        // read 2nd index set
        if(item.h05) {
           item.p05.reset(new uint32[item.h05]);
           bs.read_array(&item.p05[0], item.h05);
           if(bs.fail()) return error("Stream read failure.", __LINE__);
          }
        // debug
        if(debug) {
           md.dfile << "  INDEX SET #2" << endl;
           for(uint32 j = 0; j < item.h05; j++) {
               md.dfile << "   index[0x" << hex << j << dec << "] = ";
               md.dfile << setfill('0') << setw(8) << hex << item.p05[j] << dec << endl;
              }
          }

        // read 3rd index set
        if(item.h06) {
           item.p06.reset(new uint32[item.h06]);
           bs.read_array(&item.p06[0], item.h06);
           if(bs.fail()) return error("Stream read failure.", __LINE__);
          }
        // debug
        if(debug) {
           md.dfile << "  INDEX SET #3" << endl;
           for(uint32 j = 0; j < item.h06; j++) {
               md.dfile << "   index[0x" << hex << j << dec << "] = ";
               md.dfile << setfill('0') << setw(8) << hex << item.p06[j] << dec << endl;
              }
          }

        // insert item
        md.nuno._0301.push_back(item);
       }
   
    // TODO: DELETE ME
    // save obj file
   
    STDSTRINGSTREAM objname;
    objname << md.pathname << md.shrtname << TEXT("_NUNO.obj");
    ofstream objfile(objname.str().c_str());
    objfile << "o " << "object_" << md.shrtname.c_str() << endl;
   
    // save control points
    for(uint32 i = 0; i < md.nuno._0301.size(); i++) {
        objfile << "# NUNO ENTRY 0x" << hex << i << dec << endl;
        NUNOSUBCHUNK0301ENTRY& item = md.nuno._0301[i];
        for(uint32 j = 0; j < item.h02; j++)
            objfile << "v " << item.p01[j].p01 << " " << item.p01[j].p02 << " " << item.p01[j].p03 << endl;
        objfile << endl;
       }
   
    // save control connectivities
    uint32 vertex_index = 1;
    for(uint32 i = 0; i < md.nuno._0301.size(); i++) {
        objfile << "# NUNO ENTRY 0x" << hex << i << dec << endl;
        objfile << "g group_" << i << endl;
        NUNOSUBCHUNK0301ENTRY& item = md.nuno._0301[i];
        for(uint32 j = 0; j < item.h02; j++) {
            uint32 a[4] = { item.p02[j].p01, item.p02[j].p02, item.p02[j].p03, item.p02[j].p04 };
            if(a[0] != 0xFFFFFFFF) objfile << "f " << (vertex_index + j) << " " << (vertex_index + a[0]) << endl;
            if(a[1] != 0xFFFFFFFF) objfile << "f " << (vertex_index + j) << " " << (vertex_index + a[1]) << endl;
            if(a[2] != 0xFFFFFFFF) objfile << "f " << (vertex_index + j) << " " << (vertex_index + a[2]) << endl;
            if(a[3] != 0xFFFFFFFF) objfile << "f " << (vertex_index + j) << " " << (vertex_index + a[3]) << endl;
           }
        objfile << endl;
        vertex_index += item.h02;
       }
   }
 // if there is an 0x00030001 subchunk
 else if(s00030002_offset)
   {
    // move to 0x00030002 subchunk
    bs.seek(s00030001_offset);
    if(bs.fail()) return error("Stream seek failure.", __LINE__);
   
    // read number of 0x00030002 entries
    uint32 n_entries = bs.read_uint32();
    if(bs.fail()) return error("Stream read failure.", __LINE__);

    // debug
    if(debug) {
       md.dfile << "SUBCHUNK DATA 0x00030002" << endl;
       md.dfile << hex << setfill('0');
       md.dfile << " number of entries = 0x" << setw(8) << n_entries << endl;
       md.dfile << dec;
      }

    // for each 0x00030002 entry
    for(uint32 i = 0; i < n_entries; i++)
       {
       }
   }

 // finished
 if(debug) md.dfile << endl;
 return true;
}

bool processNUNV(MODELDATA& md)
{
 // skip
 if(md.NUNV_list.size() == 0) return true;

 // DEBUG BEGIN
 if(debug) {
    md.dfile << "------" << endl;
    md.dfile << " NUNV " << endl;
    md.dfile << "------" << endl;
    md.dfile << endl;
    md.dfile << "number of NUNV chunks = 0x" << hex << md.NUNV_list.size() << dec << endl;
    md.dfile << endl;
   }
 // DEBUG END

 // number of NUNV chunks must be 0 or 1
 uint32 n_NUNV = md.NUNV_list.size();
 if(n_NUNV != 1) return error("Number of NUNVs must be 0 or 1.", __LINE__);

 // read header
 binary_stream& bs = md.NUNV_list[0];
 uint32 h01 = bs.read_uint32(); // 0x4E554E56 (NUNV)
 uint32 h02 = bs.read_uint32(); // 0x30303130 (0010)
 uint32 h03 = bs.read_uint32(); // chunksize
 uint32 h04 = bs.read_uint32(); // number of subchunks
 if(bs.fail()) return error("Stream read failure.", __LINE__);
 if(debug) {
    md.dfile << "HEADER" << endl;
    md.dfile << hex << setfill('0');
    md.dfile << " chunkname = 0x" << setw(8) << h01 << endl;
    md.dfile << " chunkvers = 0x" << setw(8) << h02 << endl;
    md.dfile << " chunksize = 0x" << setw(8) << h03 << endl;
    md.dfile << " subchunks = 0x" << setw(8) << h04 << endl;
    md.dfile << dec;
   }

 // validate header
 if(h01 != 0x4E554E56) return error("Not a NUNV chunk.", __LINE__);
 switch(h02) {
   case(0x30303130) : break;
   case(0x30303131) : break; // [PS3] Berserk Musou (2016)
   default : return error("Unsupported NUNV version.", __LINE__);
  }
 if(h03 == 0) return error("Invalid NUNV chunksize.", __LINE__);
 if(h04 == 0) return error("Invalid number of NUNV subchunks.", __LINE__);

 // save offsets to subchunks
 binary_stream::size_type s0501_offset = 0x00;
 binary_stream::size_type s0502_offset = 0x00;

 // scan subchunks
 if(debug) md.dfile << "SUBCHUNK LIST" << endl;
 for(uint32 i = 0; i < h04; i++)
    {
     // read subchunk properties
     uint32 subchunk_type = bs.read_uint32();
     uint32 subchunk_size = bs.read_uint32();
     if(bs.fail()) return error("Stream read failure.", __LINE__);
     if(debug) {
        md.dfile << hex << setfill('0');
        md.dfile << " SUBCHUNK[0x" << i << "]" << endl;
        md.dfile << "  type = 0x" << setw(8) << subchunk_type << endl;
        md.dfile << "  size = 0x" << setw(8) << subchunk_size << endl;
        md.dfile << dec;
       }

     // save offset
     switch(subchunk_type) {
       case(0x00050001) : s0501_offset = bs.tell(); break;
       case(0x00050002) : s0502_offset = bs.tell(); break;
       default : return error("Unsupported NUNV subchunk type.", __LINE__);
      }

     // move to next subchunk
     bs.move(subchunk_size - 0x08);
     if(bs.fail()) return error("Stream seek failure.", __LINE__);
    }

 // if there is an 0x00050001 subchunk
 if(s0501_offset)
   {
    // move to 0x00050001 subchunk
    bs.seek(s0501_offset);
    if(bs.fail()) return error("Stream seek failure.", __LINE__);
   
    // read number of 0x00050001 entries
    uint32 n_entries = bs.read_uint32();
    if(bs.fail()) return error("Stream read failure.", __LINE__);

    // debug
    if(debug) {
       md.dfile << "SUBCHUNK DATA 0x00050001" << endl;
       md.dfile << hex << setfill('0');
       md.dfile << " number of entries = 0x" << setw(8) << n_entries << endl;
       md.dfile << dec;
      }

    // for each 0x00050001 entry
    for(uint32 i = 0; i < n_entries; i++)
       {
        // read entry header
        NUNV0501ENTRY item;
        item.h01 = bs.read_uint32();
        item.h02 = bs.read_uint32();
        item.h03 = bs.read_uint32();
        item.h04 = bs.read_uint32();
        item.h05 = bs.read_uint32();
        item.h06 = bs.read_real32();
        bs.read_array(&item.h07[0], 3);
        bs.read_array(&item.h08[0], 3);
        bs.read_array(&item.h09[0], 3);
        bs.read_array(&item.h10[0], 3);
        bs.read_array(&item.h11[0], 3);
        bs.read_array(&item.h12[0], 3);
        item.h13 = bs.read_uint32(); // 0x00010100
        if(bs.fail()) return error("Stream read failure.", __LINE__);

        // some unknown parameters, skip since usually all 0's
        if(h02 == 0x30303131) {
           bs.move(0x10);
           if(bs.fail()) return error("Stream seek failure.", __LINE__);
          }

        // debug
        if(debug) {
           md.dfile << setfill('0');
           md.dfile << " ENTRY[0x" << hex << i << dec << "]" << endl;
           md.dfile << "  h01 = 0x" << hex << setw(8) << item.h01 << dec << " ()" << endl;
           md.dfile << "  h02 = 0x" << hex << setw(8) << item.h02 << dec << " (number of control points)" << endl;
           md.dfile << "  h03 = 0x" << hex << setw(8) << item.h03 << dec << " ()" << endl;
           md.dfile << "  h04 = 0x" << hex << setw(8) << item.h04 << dec << " ()" << endl;
           md.dfile << "  h05 = 0x" << hex << setw(8) << item.h05 << dec << " ()" << endl;
           md.dfile << "  h06 = " << item.h06 << " ()" << endl;
           md.dfile << "  h07 = <" << item.h07[0] << ", " << item.h07[1] << ", " << item.h07[2] << ">" << " ()" << endl;
           md.dfile << "  h08 = <" << item.h08[0] << ", " << item.h08[1] << ", " << item.h08[2] << ">" << " ()" << endl;
           md.dfile << "  h09 = <" << item.h09[0] << ", " << item.h09[1] << ", " << item.h09[2] << ">" << " ()" << endl;
           md.dfile << "  h10 = <" << item.h10[0] << ", " << item.h10[1] << ", " << item.h10[2] << ">" << " ()" << endl;
           md.dfile << "  h11 = <" << item.h11[0] << ", " << item.h11[1] << ", " << item.h11[2] << ">" << " ()" << endl;
           md.dfile << "  h12 = <" << item.h12[0] << ", " << item.h12[1] << ", " << item.h12[2] << ">" << " ()" << endl;
           md.dfile << "  h13 = 0x" << hex << item.h13 << dec << " ()" << endl;
          }

        // the number of control points must be valid
        if(!item.h02) return error("The number of NUNV control points cannot be zero.", __LINE__);
        if(!item.h03) return error("NUNV h03 == 0.", __LINE__);
   
        // read control points
        item.p01.reset(new NUNV0501SUBENTRY1[item.h02]);
        for(uint32 j = 0; j < item.h02; j++) {
            item.p01[j].p01 = bs.read_real32();
            item.p01[j].p02 = bs.read_real32();
            item.p01[j].p03 = bs.read_real32();
            item.p01[j].p04 = bs.read_real32();
            if(bs.fail()) return error("Stream read failure.", __LINE__);
           }
        // debug
        if(debug) {
           md.dfile << "  CONTROL POINTS" << endl;
           for(uint32 j = 0; j < item.h02; j++) {
               md.dfile << "   point[0x" << hex << j << dec << "] = ";
               md.dfile << "<" << item.p01[j].p01 << ", " << item.p01[j].p02 << ", " << item.p01[j].p03 << ", " << item.p01[j].p04 << ">" << endl;
              }
          }

        // read influence data
        item.p02.reset(new NUNV0501SUBENTRY2[item.h02]);
        for(uint32 j = 0; j < item.h02; j++) {
            item.p02[j].p01 = bs.read_uint32(); // index to another point
            item.p02[j].p02 = bs.read_uint32(); // index to another point
            item.p02[j].p03 = bs.read_uint32(); // index to another point
            item.p02[j].p04 = bs.read_uint32(); // index to another point
            item.p02[j].p05 = bs.read_real32(); // ???
            item.p02[j].p06 = bs.read_real32(); // ???
            if(bs.fail()) return error("Stream read failure.", __LINE__);
           }
        // debug
        if(debug) {
           md.dfile << "  INFLUENCE DATA" << endl;
           for(uint32 j = 0; j < item.h02; j++) {
               md.dfile << "   influence[0x" << hex << j << dec << "] = ";
               md.dfile << setfill('0');
               md.dfile << "<" << setw(8) << hex << item.p02[j].p01 << dec << ", " <<
                                  setw(8) << hex << item.p02[j].p02 << dec << ", " <<
                                  setw(8) << hex << item.p02[j].p03 << dec << ", " <<
                                  setw(8) << hex << item.p02[j].p04 << dec << "> - ";
               md.dfile << "<" << item.p02[j].p05 << ", " << item.p02[j].p06 << ">";
               md.dfile << endl;
              }
          }

        // read ??? data
        item.p03.reset(new NUNV0501SUBENTRY3[item.h03]);
        for(uint32 j = 0; j < item.h03; j++) {
            bs.read_array(&item.p03[j].p01[0], 4);
            bs.read_array(&item.p03[j].p02[0], 4);
            item.p03[j].p03 = bs.read_uint32();
            item.p03[j].p04 = bs.read_uint32();
            item.p03[j].p05 = bs.read_uint32();
            item.p03[j].p06 = bs.read_uint32();
            if(bs.fail()) return error("Stream read failure.", __LINE__);
           }
        // debug
        if(debug) {
           md.dfile << "  UNKNOWN DATA" << endl;
           for(uint32 j = 0; j < item.h03; j++) {
               md.dfile << "   unknown[0x" << hex << j << dec << "] = ";
               md.dfile << setfill('0');
               md.dfile << setw(8) << hex << item.p03[j].p03 << dec << " ";
               md.dfile << setw(8) << hex << item.p03[j].p04 << dec << " ";
               md.dfile << setw(8) << hex << item.p03[j].p05 << dec << " ";
               md.dfile << setw(8) << hex << item.p03[j].p06 << dec << " ";
               md.dfile << "<" << item.p03[j].p01[0] << ", " <<
                                  item.p03[j].p01[1] << ", " <<
                                  item.p03[j].p01[2] << ", " <<
                                  item.p03[j].p01[3] << ">";
               md.dfile << " ";
               md.dfile << "<" << item.p03[j].p02[0] << ", " <<
                                  item.p03[j].p02[1] << ", " <<
                                  item.p03[j].p02[2] << ", " <<
                                  item.p03[j].p02[3] << ">";
               md.dfile << endl;
              }
          }

        // read 1st index set
        if(item.h04) {
           item.p04.reset(new uint32[item.h04]);
           bs.read_array(&item.p04[0], item.h04);
           if(bs.fail()) return error("Stream read failure.", __LINE__);
          }
        // debug
        if(debug) {
           md.dfile << "  INDEX SET #1" << endl;
           for(uint32 j = 0; j < item.h04; j++) {
               md.dfile << "   index[0x" << hex << j << dec << "] = ";
               md.dfile << setfill('0') << setw(8) << hex << item.p04[j] << dec << endl;
              }
          }
   
        // insert into NUNV data
        md.nunv._0501.push_back(item);
       }
   
    // TODO: DELETE ME
    // save obj file
   
    STDSTRINGSTREAM objname;
    objname << md.pathname << md.shrtname << TEXT("_NUNV.obj");
    ofstream objfile(objname.str().c_str());
    objfile << "o " << "object_" << md.shrtname.c_str() << endl;
   
    // save control points
    for(uint32 i = 0; i < md.nunv._0501.size(); i++) {
        objfile << "# NUNV ENTRY 0x" << hex << i << dec << endl;
        NUNV0501ENTRY& item = md.nunv._0501[i];
        for(uint32 j = 0; j < item.h02; j++)
            objfile << "v " << item.p01[j].p01 << " " << item.p01[j].p02 << " " << item.p01[j].p03 << endl;
        objfile << endl;
       }
   
    // save control connectivities
    uint32 vertex_index = 1;
    for(uint32 i = 0; i < md.nunv._0501.size(); i++) {
        objfile << "# NUNV ENTRY 0x" << hex << i << dec << endl;
        objfile << "g group_" << i << endl;
        NUNV0501ENTRY& item = md.nunv._0501[i];
        for(uint32 j = 0; j < item.h02; j++) {
            uint32 a[4] = { item.p02[j].p01, item.p02[j].p02, item.p02[j].p03, item.p02[j].p04 };
            if(a[0] != 0xFFFFFFFF) objfile << "f " << (vertex_index + j) << " " << (vertex_index + a[0]) << endl;
            if(a[1] != 0xFFFFFFFF) objfile << "f " << (vertex_index + j) << " " << (vertex_index + a[1]) << endl;
            if(a[2] != 0xFFFFFFFF) objfile << "f " << (vertex_index + j) << " " << (vertex_index + a[2]) << endl;
            if(a[3] != 0xFFFFFFFF) objfile << "f " << (vertex_index + j) << " " << (vertex_index + a[3]) << endl;
           }
        objfile << endl;
        vertex_index += item.h02;
       }
   }

 // finished
 if(debug) md.dfile << endl;
 return true;
}

bool processG1MG(MODELDATA& md)
{
 // skip
 if(md.G1MG_list.size() == 0) return true;

 // DEBUG BEGIN
 if(debug) {
    md.dfile << "------" << endl;
    md.dfile << " G1MG " << endl;
    md.dfile << "------" << endl;
    md.dfile << endl;
    md.dfile << "number of G1MG chunks = 0x" << hex << md.G1MG_list.size() << dec << endl;
    md.dfile << endl;
   }
 // DEBUG END

 // number of NUNV chunks must be 0 or 1
 if(md.G1MG_list.size() != 1) return error("Number of G1MGs must be 0 or 1.", __LINE__);

 // read header
 binary_stream& bs = md.G1MG_list[0];
 uint32 chunkname = bs.read_uint32();
 uint32 chunkvers = bs.read_uint32();
 uint32 chunksize = bs.read_uint32();
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // validate version
 switch(chunkvers) {
   case(0x30303434) : break;
   default: return error("Unknown G1MG version!", __LINE__);
  }

 // read header
 uint32 head01 = bs.read_uint32(); // platform
 uint32 head02 = bs.read_uint32(); // 0x00
 real32 head03 = bs.read_real32(); // min_x
 real32 head04 = bs.read_real32(); // min_y
 real32 head05 = bs.read_real32(); // min_z
 real32 head06 = bs.read_real32(); // max_x
 real32 head07 = bs.read_real32(); // max_y
 real32 head08 = bs.read_real32(); // max_z
 uint32 head09 = bs.read_uint32(); // sections
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // validate header
 switch(head01) {
   case(0x00395844) : break; // DX9  (should be LE)
   case(0x31315844) : break; // DX11 (should be LE)
   case(0x50533300) : break; // PS3
   case(0x00345350) : break; // PS4  (should be LE)
   case(0x0050474E) : break; // NGP  (should be LE)
   case(0x58333630) : break; // X360
   case(0x57696955) : break; // WiiU
   default: return error("Only PC, PS3, PS4, XBox360, and WiiU versions of this game are supported.", __LINE__);
  }
 if(head09 == 0) return error("Invalid number of G1MG sections.", __LINE__);

 // read sections
 for(uint32 i = 0; i < head09; i++)
    {
     // section information
     uint32 section_type = bs.read_uint32();
     uint32 section_size = bs.read_uint32();
     section_size -= 0x8;

     // read section
     boost::shared_array<char> section_data(new char[section_size]);
     bs.read_array(section_data.get(), section_size);
     if(bs.fail()) return error("Stream read failure.", __LINE__);

     if(section_type == 0x00010001) {
        md.section01.type = section_type;
        md.section01.data = section_data;
        md.section01.size = section_size;
       }
     else if(section_type == 0x00010002) {
        md.section02.type = section_type;
        md.section02.data = section_data;
        md.section02.size = section_size;
       }
     else if(section_type == 0x00010003) {
        md.section03.type = section_type;
        md.section03.data = section_data;
        md.section03.size = section_size;
       }
     else if(section_type == 0x00010004) {
        md.section04.type = section_type;
        md.section04.data = section_data;
        md.section04.size = section_size;
       }
     else if(section_type == 0x00010005) {
        md.section05.type = section_type;
        md.section05.data = section_data;
        md.section05.size = section_size;
       }
     else if(section_type == 0x00010006) {
        md.section06.type = section_type;
        md.section06.data = section_data;
        md.section06.size = section_size;
       }
     else if(section_type == 0x00010007) {
        md.section07.type = section_type;
        md.section07.data = section_data;
        md.section07.size = section_size;
       }
     else if(section_type == 0x00010008) {
        md.section08.type = section_type;
        md.section08.data = section_data;
        md.section08.size = section_size;
       }
     else if(section_type == 0x00010009) {
        md.section09.type = section_type;
        md.section09.data = section_data;
        md.section09.size = section_size;
       }
     else {
        stringstream ss;
        ss << "Unknown G1MG section 0x" << hex << section_type << dec << ".";
        return error(ss);
       }
    }

 // process sections
 if(!process0101(md)) return false;
 if(!process0102(md)) return false;
 if(!process0103(md)) return false;
 if(!process0104(md)) return false;
 if(!process0105(md)) return false;
 if(!process0106(md)) return false;
 if(!process0107(md)) return false;
 if(!process0108(md)) return false;
 if(!process0109(md)) return false;

 // preprocess model
 if(!md.Preprocess()) {
    cout << "Preprocessing failed." << endl;
    return false;
   }

 // construct model
 ADVANCEDMODELCONTAINER amc;
 if(!buildPhase1(md, amc)) return false; // materials and textures
 if(!buildPhase2(md, amc)) return false; // joint mapping
 if(!buildPhase3(md, amc)) return false; // vtx buffers
 if(!buildPhase4(md, amc)) return false; // idx buffers
 if(!buildPhase5(md, amc)) return false; // surfaces

 // transform and save AMC file
 if(md.data0108.elem == 1) {
    SaveAMC(md.pathname.c_str(), md.shrtname.c_str(), amc);
    SaveOBJ(md.pathname.c_str(), md.shrtname.c_str(), amc);
   }
 else
   {
    ADVANCEDMODELCONTAINER transformed = TransformAMC(amc);
    for(uint32 i = 0; i < md.data0108.elem; i++)
       {
        // validate joint map index from surface data (data0108)
        boost::shared_array<uint32> item = md.data0108.data[i];
        uint32 jm_index = item[0x02];
        if(!(jm_index < md.data0106.elem)) return error("Joint map reference out of range.", __LINE__);
        
        // get joint map
        G1MG0106ITEM jmap = md.data0106.data[jm_index];
        if(!jmap.elem) break;

        // TODO: FIX THIS!
        // if no skeleton in this file
        if(md.SKEL_list.empty()) break;

        // // get bone index from identifer
        // uint32 bone_index = GetJointIndexFromID(md.SKEL_list[0], jmap.data[0].p03);
        // if(bone_index == AMC_INVALID_JOINT) return error("Joint lookup failed.", __LINE__);

        // get bone index
        uint32 bone_index = jmap.data[0].p03;
        if(!(bone_index < md.SKEL_list[0].joints.size())) return error("Joint index out of range.", __LINE__);

        // get bone transform
        real32 m1[16];
        real32 m2[16];
        for(size_t j = 0; j < 16; j++) m1[j] = md.SKEL_list[0].joints[bone_index].m_abs[j];
        for(size_t j = 0; j < 16; j++) m2[j] = md.SKEL_list[0].joints[bone_index].m_abs[j];

        // don't translate
        m2[0x3] = 0.0f;
        m2[0x7] = 0.0f;
        m2[0xB] = 0.0f;

        // transform vertices
        for(uint32 j = 0; j < transformed.vblist[i].elem; j++)
           {
            // original data
            real32 pos[3] = {
             transformed.vblist[i].data[j].vx,
             transformed.vblist[i].data[j].vy,
             transformed.vblist[i].data[j].vz
            };
            real32 newpos[3] = { 0, 0, 0 };
            cs::math::matrix4x4_mul_vector3D(newpos, m1, pos);
            transformed.vblist[i].data[j].vx = newpos[0];
            transformed.vblist[i].data[j].vy = newpos[1];
            transformed.vblist[i].data[j].vz = newpos[2];
            transformed.vblist[i].data[j].vw = 1.0f;
    
            // transform normal
            real32 nor[3] = {
             transformed.vblist[i].data[j].nx,
             transformed.vblist[i].data[j].ny,
             transformed.vblist[i].data[j].nz
            };
            real32 newnor[3] = { 0, 0, 0 };
            cs::math::matrix4x4_mul_vector3D(newnor, m2, nor);
            cs::math::vector3D_normalize(newnor);
            // transformed.vblist[i].data[j].nx = newnor[0];
            // transformed.vblist[i].data[j].ny = newnor[1];
            // transformed.vblist[i].data[j].nz = newnor[2];
            // transformed.vblist[i].data[j].nw = 1.0f;
           }
       }
    SaveAMC(md.pathname.c_str(), md.shrtname.c_str(), transformed);
    SaveOBJ(md.pathname.c_str(), md.shrtname.c_str(), transformed);
   }

 // success
 return true;
}

#pragma endregion

#pragma region REGION_GLOBAL_FUNCTIONS

bool DWGetDebugModelMode(void)
{
 return ::debug;
}

bool DWSetDebugModelMode(bool state)
{
 bool prev = ::debug;
 ::debug = state;
 return prev;
}

bool DWConvertModel(LPCTSTR filename)
{
 // model data
 MODELDATA md;
 md.filename = filename;
 md.pathname = GetPathnameFromFilename(md.filename.c_str()).get();
 md.shrtname = GetShortFilenameWithoutExtension(md.filename.c_str()).get();

 // open file
 ifstream ifile(md.filename.c_str(), ios::binary);
 if(!ifile) return error("Could not open file.", __LINE__);

 // debug
 md.dfile.close();
 STDSTRINGSTREAM ss;
 ss << md.pathname << md.shrtname << TEXT(".txt");
 md.dfile.open(ss.str().c_str());

 // compute filesize
 size_t filesize = 0;
 ifile.seekg(0, ios::end);
 filesize = (size_t)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(!filesize) return true; // nothing to do

 // read file
 boost::shared_array<char> filedata(new char[filesize]);
 ifile.read(filedata.get(), filesize);
 if(ifile.fail()) return error("Read failure.", __LINE__);

 // set endian mode
 binary_stream bs(filedata, filesize);
 if(bs[0] == 0x5F) md.mode = ENDIAN_LITTLE;
 else if(bs[0] == 0x47) md.mode = ENDIAN_BIG;
 else return error("Invalid G1M file.", __LINE__);
 bs.set_endian_mode(md.mode);

 // read header
 uint32 head01 = bs.read_uint32(); // magic
 uint32 head02 = bs.read_uint32(); // version
 uint32 head03 = bs.read_uint32(); // filesize
 uint32 head04 = bs.read_uint32(); // offset to 1st chunk
 uint32 head05 = bs.read_uint32(); // 0x00
 uint32 head06 = bs.read_uint32(); // number of chunks to read
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // validate header
 if(head01 != 0x47314D5F) return error("Expecting G1M_ section.", __LINE__);
 if(head03 == 0) return error("Invalid G1M_.", __LINE__);
 if(head04 == 0) return error("Invalid G1M_.", __LINE__);
 if(head06 == 0) return error("Invalid G1M_.", __LINE__);

 // validate version
 switch(head02) {
   case(0x30303334) : break; // OK, OPKM 1
   case(0x30303335) : break; // OK, OPKM 2
   case(0x30303336) : break; // OK, Shin Gundam Musou
   case(0x30303337) : break; // OK, [PC] Warriors All-Stars (2017)
   default : return error("Invalid G1M_ version.", __LINE__);
  }

 // move to start
 bs.seek(head04);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // read chunks
 for(uint32 i = 0; i < head06; i++)
    {
     // read first chunk
     uint32 cposition = (uint32)bs.tell(); // save position
     uint32 chunkname = bs.read_uint32();  // chunk name
     uint32 chunkvers = bs.read_uint32();  // chunk version
     uint32 chunksize = bs.read_uint32();  // chunk size

     // G1MF
     if(chunkname == 0x47314D46)
       {
        // seek data
        bs.seek(cposition);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);

        // read data
        boost::shared_array<char> data(new char[chunksize]);
        bs.read_array(data.get(), chunksize);
        if(bs.fail()) return error("Stream read failure.", __LINE__);
        
        // save data
        binary_stream chunk(data, chunksize);
        chunk.set_endian_mode(md.mode);
        md.G1MF_list.push_back(chunk);
       }
     // G1MS
     else if(chunkname == 0x47314D53)
       {
        // seek data
        bs.seek(cposition);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);

        // read data
        boost::shared_array<char> data(new char[chunksize]);
        bs.read_array(data.get(), chunksize);
        if(bs.fail()) return error("Stream read failure.", __LINE__);
        
        // save data
        binary_stream chunk(data, chunksize);
        chunk.set_endian_mode(md.mode);
        md.G1MS_list.push_back(chunk);
       }
     // G1MM
     else if(chunkname == 0x47314D4D)
       {
        // seek data
        bs.seek(cposition);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);

        // read data
        boost::shared_array<char> data(new char[chunksize]);
        bs.read_array(data.get(), chunksize);
        if(bs.fail()) return error("Stream read failure.", __LINE__);
        
        // save data
        binary_stream chunk(data, chunksize);
        chunk.set_endian_mode(md.mode);
        md.G1MM_list.push_back(chunk);
       }
     // G1MG
     else if(chunkname == 0x47314D47)
       {
        // seek data
        bs.seek(cposition);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);

        // read data
        boost::shared_array<char> data(new char[chunksize]);
        bs.read_array(data.get(), chunksize);
        if(bs.fail()) return error("Stream read failure.", __LINE__);
        
        // save data
        binary_stream chunk(data, chunksize);
        chunk.set_endian_mode(md.mode);
        md.G1MG_list.push_back(chunk);
       }
     // COLL
     else if(chunkname == 0x434F4C4C)
       {
        // skip data
        bs.move(chunksize - 0x0C);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);
       }
     // HAIR (2015 games)
     else if(chunkname == 0x48414952)
       {
        // skip data
        bs.move(chunksize - 0x0C);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);
       }
     // NUNO
     else if(chunkname == 0x4E554E4F)
       {
        // seek data
        bs.seek(cposition);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);

        // read data
        boost::shared_array<char> data(new char[chunksize]);
        bs.read_array(data.get(), chunksize);
        if(bs.fail()) return error("Stream read failure.", __LINE__);
        
        // save data
        binary_stream chunk(data, chunksize);
        chunk.set_endian_mode(md.mode);
        md.NUNO_list.push_back(chunk);
       }
     // NUNS (2015 games)
     else if(chunkname == 0x4E554E53)
       {
        // skip data
        bs.move(chunksize - 0x0C);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);
       }
     // NUNV
     else if(chunkname == 0x4E554E56)
       {
        // seek data
        bs.seek(cposition);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);

        // read data
        boost::shared_array<char> data(new char[chunksize]);
        bs.read_array(data.get(), chunksize);
        if(bs.fail()) return error("Stream read failure.", __LINE__);
        
        // save data
        binary_stream chunk(data, chunksize);
        chunk.set_endian_mode(md.mode);
        md.NUNV_list.push_back(chunk);
       }
     // EXTR
     else if(chunkname == 0x45585452)
       {
        // skip data
        bs.move(chunksize - 0x0C);
        if(bs.fail()) return error("Stream seek failure.", __LINE__);
       }
     // UNKN
     else {
        stringstream ss;
        ss << "Unknown chunk 0x" << hex << chunkname << dec << " at offset 0x" << hex << cposition << dec << ".";
        return error(ss);
       }
    }

 // validate chunks
 if(md.G1MF_list.size() != 1) return error("Unexpected number of G1MF chunks.");
 if(md.G1MM_list.size() != 1) return error("Unexpected number of G1MM chunks.");
 if(md.G1MG_list.size() != 1) return error("Unexpected number of G1MG chunks.");

 //
 // PROCESS G1MS (skeleton) and G1MM (matrices)
 //

 if(!processG1MM(md)) return false;
 if(!processG1MS(md)) return false;

 //
 // PROCESS NUNO and NUNV (cloth)
 //

 if(!processNUNO(md)) return false;
 if(!processNUNV(md)) return false;

 //
 // PROCESS G1MG (model)
 //

 if(!processG1MG(md)) return false;

 // finished
 bool do_delete = false;
 if(do_delete) {
    ifile.close();
    DeleteFile(md.filename.c_str());
   }
 return true;
}

#pragma endregion