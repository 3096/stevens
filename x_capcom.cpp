#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_zlib.h"
#include "x_amc.h"
#include "x_dds.h"
#include "x_con.h"
#include "x_capcom.h"
using namespace std;

//
// UTILITIES
//
#pragma region CAPCOM_UTILITIES
namespace CAPCOM {

template<class T>
bool is_char_numeric(T n)
{
 return ((n > 0x2F) && (n < 0x3A));
}

template<class T>
bool is_char_alpha(T n)
{
 return ((n > 0x40) && (n < 0x5B)) || ((n > 0x60) && (n < 0x7B));
}

template<class T>
bool is_char_alphanumeric(T n)
{
 return is_char_numeric(n) || is_char_alpha(n);
}

}
#pragma endregion

//
// SKELETONS
//

#pragma region CAPCOM_SKELETONS
namespace CAPCOM {

bool CreateSkeleton(AMC_SKELETON2& skeleton, const BONEDATA& bd)
{
 // create skeleton name
 stringstream ss;
 ss << "skeleton";

 // set format
 skeleton.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX | AMC_JOINT_FORMAT_X_BONEAXIS;
 skeleton.name = ss.str();
 skeleton.tiplen = 1.0f;

 for(uint32 i = 0; i < bd.boneinfo.size(); i++)
    {
     // copy the two transformation matrices
     using namespace cs::math;
     matrix4x4<cs::binary32> m1(&bd.m1[i].m[0]);
     matrix4x4<cs::binary32> m2(&bd.m2[i].m[0]);

     // scale and translation properties
     cs::binary32 scale_x = m1(0,0)/m2(0,0);
     cs::binary32 scale_y = m1(1,1)/m2(1,1);
     cs::binary32 scale_z = m1(2,2)/m2(2,2);
     cs::binary32 trans_x = m2(0,3);
     cs::binary32 trans_y = m2(1,3);
     cs::binary32 trans_z = m2(2,3);

     // m2 * T = m1
     // three equations and three unknowns can be solved for by cramer's rule
     // [r00 r01 r02 0] [ 1  0  0 tx] = [m00 m01 m02 m03]
     // [r10 r11 r12 0] [ 0  1  0 ty] = [m10 m11 m12 m13]
     // [r20 r21 r22 0] [ 0  0  1 tz] = [m20 m21 m22 m23]
     // [  0   0   0 1] [ 0  0  0  1] = [m30 m31 m32 m33]

     // find joint position in absolute coordinates using cramer's rule
     cs::binary32 A[9] = {
      m2(0,0), m2(0,1), m2(0,2),
      m2(1,0), m2(1,1), m2(1,2),
      m2(2,0), m2(2,1), m2(2,2),
     };
     cs::binary32 b[3] = {
      -m2(0,3),
      -m2(1,3),
      -m2(2,3),
     };
     cs::binary32 x[3] = { 0.0f, 0.0f, 0.0f };
     cramer_3x3(A, b, x);

     // joints are actually remapped between bones-in-file and bones-in-complete-model
     uint32 jnt_index = i;
     uint32 par_index = bd.boneinfo[i].j02;

     // create joint name
     stringstream ss;
     ss << "jnt_" << setfill('0') << setw(3) << bd.boneinfo[jnt_index].j01;

     // create joint
     AMC_JOINT joint;
     joint.name = ss.str();
     joint.id = bd.boneinfo[jnt_index].j01; // this is actual joint ID
     joint.parent = ((par_index == 0xFF) ? AMC_INVALID_JOINT : bd.boneinfo[par_index].j01);
     joint.m_rel[0x0] = m1[0x0];
     joint.m_rel[0x1] = m1[0x1];
     joint.m_rel[0x2] = m1[0x2];
     joint.m_rel[0x3] = x[0];
     joint.m_rel[0x4] = m1[0x4];
     joint.m_rel[0x5] = m1[0x5];
     joint.m_rel[0x6] = m1[0x6];
     joint.m_rel[0x7] = x[1];
     joint.m_rel[0x8] = m1[0x8];
     joint.m_rel[0x9] = m1[0x9];
     joint.m_rel[0xA] = m1[0xA];
     joint.m_rel[0xB] = x[2];
     joint.m_rel[0xC] = m1[0xC];
     joint.m_rel[0xD] = m1[0xD];
     joint.m_rel[0xE] = m1[0xE];
     joint.m_rel[0xF] = m1[0xF];
     joint.p_rel[0] = x[0];
     joint.p_rel[1] = x[1];
     joint.p_rel[2] = x[2];
     joint.p_rel[3] = 1.0f;
     skeleton.joints.push_back(joint);
    }

 return true;
}

}
#pragma endregion

//
// MESHES
//
#pragma region CAPCOM_VERTEX_MESHES
namespace CAPCOM {

bool ProcessVertexStride0x0C(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 // VERIFIED
 // useless edge geometry
 if(meshinfo.vtx_format == 0xB0983013ul) {
    vb.flags |= AMC_VERTEX_NORMAL;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        vb.data[i].vw = bs.read_sint16()/32767.0f;
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
       }
   }
 // UNVERIFIED (DMC SPECIAL)
 else if(meshinfo.vtx_format == 0xB0983014ul) {
    vb.flags |= AMC_VERTEX_NORMAL;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        vb.data[i].vw = bs.read_sint16()/32767.0f;
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
       }
   }
 // VERIFIED
 else if(meshinfo.vtx_format == 0xDB7DA014ul) {
    vb.flags |= AMC_VERTEX_NORMAL;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        vb.data[i].vw = bs.read_sint16()/32767.0f;
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        bs.read_sint16()/32767.0f; // unknown
        bs.read_sint16()/32767.0f; // unknown
       }
   }
 // unsupported
 else {
   stringstream ss;
   ss << "Unsupported vertex format 0x" << hex << meshinfo.vtx_format << dec << ".";
   return error(ss.str().c_str());
  }

 return true;
}

bool ProcessVertexStride0x10(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 // useless edge geometry
 // 0xDB7DA014 = RESIDENT EVIL
 // 0xDB7DA015 = DMC SPECIAL
 if(meshinfo.vtx_format == 0xDB7DA014ul || meshinfo.vtx_format == 0xDB7DA015ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        vb.data[i].vw = bs.read_sint16()/32767.0f;
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();
        vb.data[i].uv[0][1] = bs.read_real16();
       }
   }
 // unsupported
 else {
   stringstream ss;
   ss << "Unsupported vertex format 0x" << hex << meshinfo.vtx_format << dec << ".";
   return error(ss.str().c_str());
  }

 return true;
}

bool ProcessVertexStride0x14(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 // useless edge geometry
 if(meshinfo.vtx_format == 0x0CB68015ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        vb.data[i].vw = bs.read_sint16()/32767.0f;
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][1] = bs.read_sint16()/32767.0f;
        vb.data[i].uv[0][2] = bs.read_sint16()/32767.0f;
       }
   }
 // VERIFIED
 // EXAMPLE: [PC] Resident Evil 0 HD/model/em/em91/em91.mod
 // USAGE: Used on skeletal models for parts that have all 1.0 weights and only one blend index.
 // 0xA8FAB018 = RESIDENT EVIL
 // 0xA8FAB019 = DMC SPECIAL
 else if(meshinfo.vtx_format == 0xA8FAB018ul || meshinfo.vtx_format == 0xA8FAB019ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f; // B9 27
        vb.data[i].vy = bs.read_sint16()/32767.0f; // F5 48
        vb.data[i].vz = bs.read_sint16()/32767.0f; // 13 0A
        uint16 bi = bs.read_uint16();              // 02 00 (only need one blend index, and it's 16-bits)
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f; // E0 AD C1 FE
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f; // CF 3F 33 FE
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();             // AA 32
        vb.data[i].uv[0][1] = bs.read_real16();             // D2 2E
        vb.data[i].wi[0] = bd.boneinfo[(meshinfo.wmap_index + bi)].j01;
        vb.data[i].wv[0] = 1.0f; // always 1
       }
   }
 // 0xCB68015 = RESIDENT EVIL
 // 0xCB68016 = DMC SPECIAL
 else if(meshinfo.vtx_format == 0x0CB68015u || meshinfo.vtx_format == 0x0CB68016ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f; // 0x02
        vb.data[i].vy = bs.read_sint16()/32767.0f; // 0x04
        vb.data[i].vz = bs.read_sint16()/32767.0f; // 0x06
        vb.data[i].vw = bs.read_sint16()/32767.0f; // 0x08
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f; // 0x09
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f; // 0x0A
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f; // 0x0B
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f; // 0x0C
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f; // 0x0D
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f; // 0x0E
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f; // 0x0F
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f; // 0x10
        vb.data[i].uv[0][0] = bs.read_sint16()/32767.0f; // 0x12 (UV channel)
        vb.data[i].uv[0][1] = bs.read_sint16()/32767.0f; // 0x14 (UV channel)
       }
   }
 // VERIFIED
 // EXAMPLE: [PC] Resident Evil 0 HD\model\ke\ke07b\ke07b.mod.
 // USAGE: Static models. No weights.
 else if(meshinfo.vtx_format == 0xA7D7D036u) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_real32();
        vb.data[i].vy = bs.read_real32();
        vb.data[i].vz = bs.read_real32();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();
        vb.data[i].uv[0][1] = bs.read_real16();
       }
   }
 else {
   stringstream ss;
   ss << "Unsupported vertex format 0x" << hex << meshinfo.vtx_format << dec << ".";
   return error(ss.str().c_str());
  }

 return true;
}

bool ProcessVertexStride0x18(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 // 0xC31F201C = RESIDENT EVIL
 // 0xC31F201D = DMC SPECIAL
 if(meshinfo.vtx_format == 0xC31F201Cul || meshinfo.vtx_format == 0xC31F201Dul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        real32 w1 = bs.read_sint16()/32767.0f;
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();
        vb.data[i].uv[0][1] = bs.read_real16();
        uint16 i1 = (uint16)bs.read_real16(); // 0x16 (blend index in half-float, these are always scalars like, 0, 1, 2, 3...)
        uint16 i2 = (uint16)bs.read_real16(); // 0x18 (blend index in half-float, these are always scalars like, 0, 1, 2, 3...)
        if(i1 == i2) {
           vb.data[i].wi[0] = bd.boneinfo[(meshinfo.wmap_index + i1)].j01;
           vb.data[i].wv[0] = 1.0f;
          }
        else {
           vb.data[i].wi[0] = bd.boneinfo[(meshinfo.wmap_index + i1)].j01;
           vb.data[i].wv[0] = w1;
           vb.data[i].wi[1] = bd.boneinfo[(meshinfo.wmap_index + i2)].j01;
           vb.data[i].wv[1] = 1.0f - w1;
          }
       }
   }
 // VERIFIED
 // EXAMPLE: Resident Evil 0 HD (2016)\model\di\di1f\di1f.mod
 // USAGE: Used for static models that move swing. This is no lightmap version of format 0x667B1019.
 else if(meshinfo.vtx_format == 0xCBF6C01Aul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        // uint08 bf = bs.read_uint08();
        // uint08 bi = bs.read_uint08();
        uint16 bi = bs.read_uint16();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();
        vb.data[i].uv[0][1] = bs.read_real16();
        bs.read_uint08();
        bs.read_uint08();
        bs.read_uint08();
        bs.read_uint08();
        vb.data[i].wi[0] = bd.boneinfo[(meshinfo.wmap_index + bi)].j01;
        vb.data[i].wv[0] = 1.0f;
       }
   }
 // VERIFIED
 // EXAMPLE: Resident Evil 0 HD (2016)\model\bo\pl1a\pl1a_00.mod
 // USAGE: Used for static models that move/swing.
 else if(meshinfo.vtx_format == 0x667B1019ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
    vb.uvchan = 2;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    vb.uvtype[1] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        uint08 bf = bs.read_uint08();
        uint08 bi = bs.read_uint08();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16(); // uv map
        vb.data[i].uv[0][1] = bs.read_real16(); // uv map
        vb.data[i].uv[1][0] = bs.read_real16(); // lightmap
        vb.data[i].uv[1][1] = bs.read_real16(); // lightmap
        vb.data[i].wi[0] = bd.boneinfo[(meshinfo.wmap_index + bi)].j01;
        vb.data[i].wv[0] = 1.0f;
       }
   }
 // VERIFIED
 // EXAMPLE: Resident Evil 0 HD (2016)\model\ia\ia62\ia62.mod
 // USAGE: Used for static models. There is usually no skeleton if this is only mesh in file.
 else if(meshinfo.vtx_format == 0xD8297028ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        // read vertex
        vb.data[i].vx = bs.read_real32();                   // EE 7C C3 C0
        vb.data[i].vy = bs.read_real32();                   // 32 E6 EE BF
        vb.data[i].vz = bs.read_real32();                   // B8 AF F4 C0
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f; // 7F 00 7F FF
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f; //
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f; //
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f; //
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f; // EC 7E BE FE
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f; // 
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f; // 
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f; // 
        vb.data[i].uv[0][0] = bs.read_real16();             // 45 38
        vb.data[i].uv[0][1] = bs.read_real16();             // 91 37
       }
   }
 else if(meshinfo.vtx_format == 0x207D6037ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_real32();
        vb.data[i].vy = bs.read_real32();
        vb.data[i].vz = bs.read_real32();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();
        vb.data[i].uv[0][1] = bs.read_real16();
        bs.read_uint08();
        bs.read_uint08();
        bs.read_uint08();
        bs.read_uint08();
       }
   }
 else {
   stringstream ss;
   ss << "Unsupported vertex format 0x" << hex << meshinfo.vtx_format << dec << ".";
   return error(ss.str().c_str());
  }

 return true;
}

bool ProcessVertexStride0x1C(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 // 0x14D40020 = RESIDENT EVIL
 // 0x14D40021 = DMC SPECIAL
 if(meshinfo.vtx_format == 0x14D40020ul || meshinfo.vtx_format == 0x14D40021ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;           // 46 3A 
        vb.data[i].vy = bs.read_sint16()/32767.0f;           // BE 49 
        vb.data[i].vz = bs.read_sint16()/32767.0f;           // FF 06 
        real32 w1 = bs.read_sint16()/32767.0f;               // 38 39 
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;  // 4A 9F ED FE 
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;  //
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;  //
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;  //
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;  // 0B 6A 4D FE 
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;  //
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;  //
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;  //
        uint16 i1 = bs.read_uint08();                        // 0A 09 0B 00
        uint16 i2 = bs.read_uint08();                        //
        uint16 i3 = bs.read_uint08();                        //
        uint16 i4 = bs.read_uint08();                        //
        vb.data[i].uv[0][0] = bs.read_real16();              // 00 00 00 00
        vb.data[i].uv[0][1] = bs.read_real16();              //
        real32 w2 = bs.read_real16();                        // 86 36
        real32 w3 = bs.read_real16();                        // A4 30
        vb.data[i].wi[0] = bd.boneinfo[(meshinfo.wmap_index + i1)].j01;
        vb.data[i].wi[1] = bd.boneinfo[(meshinfo.wmap_index + i2)].j01;
        vb.data[i].wi[2] = bd.boneinfo[(meshinfo.wmap_index + i3)].j01;
        vb.data[i].wi[3] = bd.boneinfo[(meshinfo.wmap_index + i4)].j01;
        vb.data[i].wv[0] = w1;
        vb.data[i].wv[1] = w2;
        vb.data[i].wv[2] = w3;
        vb.data[i].wv[3] = chop(1.0f - w1 - w2 - w3, 0.0001f);
       }
   }
 // VERIFIED FORMAT 0xA320C016
 // Resident Evil 0 HD Remastered\model\em\em3c\em3c.MOD
 // Useless line geometry. Weights sum to 255. There are 8 weights and 8 indices.
 // 0xA320C016 = RESIDENT EVIL
 // 0xA320C017 = DMC SPECIAL
 else if(meshinfo.vtx_format == 0xA320C016 || meshinfo.vtx_format == 0xA320C017) {
    vb.flags |= AMC_VERTEX_WEIGHTS;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        vb.data[i].vw = bs.read_sint16()/32767.0f;
        vb.data[i].wi[0] = bd.boneinfo[(meshinfo.wmap_index + bs.read_uint08())].j01;
        vb.data[i].wi[1] = bd.boneinfo[(meshinfo.wmap_index + bs.read_uint08())].j01;
        vb.data[i].wi[2] = bd.boneinfo[(meshinfo.wmap_index + bs.read_uint08())].j01;
        vb.data[i].wi[3] = bd.boneinfo[(meshinfo.wmap_index + bs.read_uint08())].j01;
        vb.data[i].wi[4] = bd.boneinfo[(meshinfo.wmap_index + bs.read_uint08())].j01;
        vb.data[i].wi[5] = bd.boneinfo[(meshinfo.wmap_index + bs.read_uint08())].j01;
        vb.data[i].wi[6] = bd.boneinfo[(meshinfo.wmap_index + bs.read_uint08())].j01;
        vb.data[i].wi[7] = bd.boneinfo[(meshinfo.wmap_index + bs.read_uint08())].j01;
        vb.data[i].wv[0] = bs.read_uint08()/255.0f;
        vb.data[i].wv[1] = bs.read_uint08()/255.0f;
        vb.data[i].wv[2] = bs.read_uint08()/255.0f;
        vb.data[i].wv[3] = bs.read_uint08()/255.0f;
        vb.data[i].wv[4] = bs.read_uint08()/255.0f;
        vb.data[i].wv[5] = bs.read_uint08()/255.0f;
        vb.data[i].wv[6] = bs.read_uint08()/255.0f;
        vb.data[i].wv[7] = bs.read_uint08()/255.0f;
        bs.BE_read_uint08(); // 0x19
        bs.BE_read_uint08(); // 0x1A
        bs.BE_read_uint08(); // 0x1B
        bs.BE_read_uint08(); // 0x1C
       }
   }
 // VERIFIED
 // EXAMPLE: Resident Evil 0 HD Remastered\effect\mod\0009\md0009.mod
 // USAGE: Effects models.
 else if(meshinfo.vtx_format == 0x5E7F202C) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 2;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    vb.uvtype[1] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        // read vertex
        vb.data[i].vx = bs.read_real32();
        vb.data[i].vy = bs.read_real32();
        vb.data[i].vz = bs.read_real32();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();
        vb.data[i].uv[0][1] = bs.read_real16();
        vb.data[i].uv[1][0] = bs.read_real16();
        vb.data[i].uv[1][1] = bs.read_real16();
       }
   }
 // VERIFIED
 // EXAMPLE: Resident Evil 0 HD Remastered\effect\mod\000d\md000d_00.mod
 // USAGE: Effects models.
 else if(meshinfo.vtx_format == 0xA14E003Cul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 2;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    vb.uvtype[1] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_real32();
        vb.data[i].vy = bs.read_real32();
        vb.data[i].vz = bs.read_real32();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16(); // (UV)
        vb.data[i].uv[0][1] = bs.read_real16(); // (UV)
        vb.data[i].uv[1][0] = bs.read_real16(); // (AM)
        vb.data[i].uv[1][1] = bs.read_real16(); // (AM)
        bs.read_uint08(); // 0x19 (0x6D)
        bs.read_uint08(); // 0x1A (0x6D)
        bs.read_uint08(); // 0x1B (0x6D)
        bs.read_uint08(); // 0x1C (0xFF)
       }
   }
 // VERIFIED
 // EXAMPLE: Resident Evil 0 HD Remastered\effect\mod\0303\md0303_02.mod
 // USAGE: Effects models.
 else if(meshinfo.vtx_format == 0xAFA6302Dul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 2;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    vb.uvtype[1] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_real32();
        vb.data[i].vy = bs.read_real32();
        vb.data[i].vz = bs.read_real32();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();
        vb.data[i].uv[0][1] = bs.read_real16();
        vb.data[i].uv[1][0] = bs.read_real16();
        vb.data[i].uv[1][1] = bs.read_real16();
       }
   }
 else {
   stringstream ss;
   ss << "Unsupported vertex format 0x" << hex << meshinfo.vtx_format << dec << ".";
   return error(ss.str().c_str());
  }

 return true;
}

bool ProcessVertexStride0x20(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 if(meshinfo.vtx_format == 0xD877801Bul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;
        vb.data[i].vy = bs.read_sint16()/32767.0f;
        vb.data[i].vz = bs.read_sint16()/32767.0f;
        uint08 bf = bs.read_uint08();
        uint08 bi = bs.read_uint08();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();
        vb.data[i].uv[0][1] = bs.read_real16();
        bs.read_uint16(); // 0x16 (0x0000)
        bs.read_uint16(); // 0x18 (0x0000)
        bs.read_real16(); // 0x1A (unknown)
        bs.read_real16(); // 0x1C (unknown)
        bs.read_uint16(); // 0x1E (0x0000)
        bs.read_uint16(); // 0x20 (0x0000)
        if(bf == 0x00) {
           vb.data[i].wi[0] = bd.boneinfo[(meshinfo.wmap_index + bi)].j01;
           vb.data[i].wv[0] = 1.0f;
          }
        else {
           vb.data[i].wi[0] = bd.boneinfo[(meshinfo.wmap_index)].j01;
           vb.data[i].wv[0] = 1.0f;
          }
       }
   }
 // VERIFIED
 // EXAMPLE: [PC] Resident Evil 0 HD\effect\mod\000c\md000c_02.mod
 // EXAMPLE: [PC] Resident Evil 0 HD\effect\mod\001a\md001a_00.mod
 // USAGE: Effect models.
 else if(meshinfo.vtx_format == 0x9399C033ul || meshinfo.vtx_format == 0xB86DE02Aul || meshinfo.vtx_format == 0x926FD02Eul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 2;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    vb.uvtype[1] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_real32();
        vb.data[i].vy = bs.read_real32();
        vb.data[i].vz = bs.read_real32();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16(); // UV
        vb.data[i].uv[0][1] = bs.read_real16(); // UV
        vb.data[i].uv[1][0] = bs.read_real16(); // AM (0x9399C033) or LM(0xB86DE02A) or MM(0x926FD02E)
        vb.data[i].uv[1][1] = bs.read_real16(); // AM (0x9399C033) or LM(0xB86DE02A) or MM(0x926FD02E)
        bs.read_uint08(); // 0x1D (unknown vector)
        bs.read_uint08(); // 0x1E (unknown vector)
        bs.read_uint08(); // 0x1F (unknown vector)
        bs.read_uint08(); // 0x20 (unknown vector)
       }
    }
 else {
   stringstream ss;
   ss << "Unsupported vertex format 0x" << hex << meshinfo.vtx_format << dec << ".";
   return error(ss.str().c_str());
  }

 return true;
}

bool ProcessVertexStride0x24(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 // VERIFIED FORMAT 0xBB424024
 // Resident Evil 0 HD Remastered\model\em\em2a\em2a.MOD
 // This one has up to five weights.
 // 0xBB424024 = RESIDENT EVIL
 // 0xBB424025 = DMC SPECIAL
 if(meshinfo.vtx_format == 0xBB424024ul || meshinfo.vtx_format == 0xBB424025ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_WEIGHTS | AMC_VERTEX_UV;
    vb.uvchan = 1;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_sint16()/32767.0f;                           // 0E 06 
        vb.data[i].vy = bs.read_sint16()/32767.0f;                           // D1 43 
        vb.data[i].vz = bs.read_sint16()/32767.0f;                           // 03 4B 
        vb.data[i].wv[0] = bs.read_sint16()/32767.0f;                        // B6 36 (14006) = 0.4274422437208167
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;                  // 18 
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;                  // 87 
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;                  // C9 
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;                  // FE 
        vb.data[i].wv[1] = bs.read_uint08()/255.0f;                          // 42 (66) = 0.2588235294117647 sum = 0.6862657731325814
        vb.data[i].wv[2] = bs.read_uint08()/255.0f;                          // 30 (48) = 0.1882352941176471 sum = 0.8745010672502285
        vb.data[i].wv[3] = bs.read_uint08()/255.0f;                          // 14 (20) = 0.0784313725490196 sum = 0.9529324397992481
        vb.data[i].wv[4] = bs.read_uint08()/255.0f;                          // 0C (12) = 0.0470588235294118 sum = 0.9999912633286599
        uint08 i1 = bs.read_uint08();                                        // 03 
        uint08 i2 = bs.read_uint08();                                        // 02 
        uint08 i3 = bs.read_uint08();                                        // 07 
        uint08 i4 = bs.read_uint08();                                        // 04 
        uint08 i5 = bs.read_uint08();                                        // 0D 
        bs.read_uint08();                                                    // 00
        bs.read_uint08();                                                    // 00
        bs.read_uint08();                                                    // 00 
        vb.data[i].uv[0][0] = bs.read_real16();                              // 51 36 
        vb.data[i].uv[0][1] = bs.read_real16();                              // 03 3A
        bs.read_uint16(); // 0x1E (0x0000)                                   // 00 00 00 00
        bs.read_uint16(); // 0x20 (0x0000)                                   //
        bs.read_uint08(); // 0x21 (unknown vector)                           // C8 C8 C8 FE
        bs.read_uint08(); // 0x22 (unknown vector)                           //
        bs.read_uint08(); // 0x23 (unknown vector)                           //
        bs.read_uint08(); // 0x24 (unknown vector)                           //
        vb.data[i].wi[0] = (vb.data[i].wv[0] > 0.0f ? bd.boneinfo[(meshinfo.wmap_index + i1)].j01 : AMC_INVALID_VERTEX_WMAP_INDEX);
        vb.data[i].wi[1] = (vb.data[i].wv[1] > 0.0f ? bd.boneinfo[(meshinfo.wmap_index + i2)].j01 : AMC_INVALID_VERTEX_WMAP_INDEX);
        vb.data[i].wi[2] = (vb.data[i].wv[2] > 0.0f ? bd.boneinfo[(meshinfo.wmap_index + i3)].j01 : AMC_INVALID_VERTEX_WMAP_INDEX);
        vb.data[i].wi[3] = (vb.data[i].wv[3] > 0.0f ? bd.boneinfo[(meshinfo.wmap_index + i4)].j01 : AMC_INVALID_VERTEX_WMAP_INDEX);
        vb.data[i].wi[4] = (vb.data[i].wv[4] > 0.0f ? bd.boneinfo[(meshinfo.wmap_index + i5)].j01 : AMC_INVALID_VERTEX_WMAP_INDEX);
        vb.data[i].wi[5] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[6] = AMC_INVALID_VERTEX_WMAP_INDEX;
        vb.data[i].wi[7] = AMC_INVALID_VERTEX_WMAP_INDEX;
       }
   }
 // VERIFIED
 // EXAMPLE: [PC] Resident Evil 0 HD\effect\mod\002b\md002b_00.mod
 // USAGE: Effects models.
 else if(meshinfo.vtx_format == 0xB6681034ul) {
    vb.flags |= AMC_VERTEX_NORMAL | AMC_VERTEX_UV;
    vb.uvchan = 3;
    vb.uvtype[0] = AMC_DIFFUSE_MAP;
    vb.uvtype[1] = AMC_DIFFUSE_MAP;
    vb.uvtype[2] = AMC_DIFFUSE_MAP;
    for(uint32 i = 0; i < meshinfo.vertices; i++) {
        vb.data[i].vx = bs.read_real32();
        vb.data[i].vy = bs.read_real32();
        vb.data[i].vz = bs.read_real32();
        vb.data[i].nx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ny = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].nw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tx = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].ty = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tz = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].tw = (bs.read_uint08() - 127.0f)/128.0f;
        vb.data[i].uv[0][0] = bs.read_real16();
        vb.data[i].uv[0][1] = bs.read_real16();
        vb.data[i].uv[1][0] = bs.read_real16();
        vb.data[i].uv[1][1] = bs.read_real16();
        bs.read_uint08();
        bs.read_uint08();
        bs.read_uint08();
        bs.read_uint08();
        vb.data[i].uv[2][0] = bs.read_real16();
        vb.data[i].uv[2][1] = bs.read_real16();
       }
    }
 else {
   stringstream ss;
   ss << "Unsupported vertex format 0x" << hex << meshinfo.vtx_format << dec << ".";
   return error(ss.str().c_str());
  }

 return true;
}

bool ProcessVertexStride0x28(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 return true;
}

bool ProcessVertexStride0x2C(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 return true;
}

bool ProcessVertexStride0x30(binary_stream& bs, AMC_VTXBUFFER& vb, const BONEDATA& bd, const MESHINFO& meshinfo)
{
 return true;
}

bool CreateMeshData(AMC_VTXBUFFER& vb, AMC_IDXBUFFER& ib, AMC_SURFACE& surface, const BONEDATA& bd, const MESHDATA& meshdata, const MESHINFO& meshinfo)
{
 // type   vert   mi   ld                       vs        vtx index1    base offset   vtx format    base index    n_indices     vtx index2    wm   jm           min max index
 // 2010 - 3000 - 00 - 00 - 00 - FF - 09 - 00 - 0C - C3 - 00 00 00 00 - 00 00 00 00 - 13 30 98 B0 - 00 00 00 00 - 87 00 00 00 - 00 00 00 00 - 00 - 01 - 0B 00 - 00 00 - 2F 00 - 00 00 00 00
 // 2010 - 1C00 - 00 - 00 - 00 - FF - 09 - 00 - 0C - C3 - 30 00 00 00 - 00 00 00 00 - 13 30 98 B0 - 87 00 00 00 - 48 00 00 00 - 00 00 00 00 - 00 - 01 - 13 00 - 30 00 - 4B 00 - 00 00 00 00

 // there is nothing to do here
 if(!meshinfo.vertices || !meshinfo.indices) return false;
 if(!meshdata.vbsize || !meshdata.ibsize) return false;

 //
 // VERTEX BUFFER PROCESSING
 //

 // create binary stream
 binary_stream bs(meshdata.vbdata, meshdata.vbsize);
 bs.set_endian_mode(meshdata.mode);

 // move to vertex data
 uint32 offset = meshinfo.vtx_offset + meshinfo.vtx_start1*meshinfo.vtx_stride;
 bs.seek(offset);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // intialize vertex buffer
 vb.name = "default";
 vb.elem = meshinfo.vertices;
 vb.data.reset(new AMC_VERTEX[vb.elem]);
 vb.flags = AMC_VERTEX_POSITION;
 vb.colors = 0;
 vb.uvchan = 0;
 for(uint32 i = 0; i < 8; i++) vb.uvtype[i] = AMC_INVALID_MAP;

 // initialize weights
 for(uint32 i = 0; i < meshinfo.vertices; i++)
     for(uint32 j = 0; j < 8; j++)
         vb.data[i].wi[j] = AMC_INVALID_VERTEX_WMAP_INDEX;

 // process vertices
 switch(meshinfo.vtx_stride) {
   case(0x0C) : if(!ProcessVertexStride0x0C(bs, vb, bd, meshinfo)) return false; break;
   case(0x10) : if(!ProcessVertexStride0x10(bs, vb, bd, meshinfo)) return false; break;
   case(0x14) : if(!ProcessVertexStride0x14(bs, vb, bd, meshinfo)) return false; break;
   case(0x18) : if(!ProcessVertexStride0x18(bs, vb, bd, meshinfo)) return false; break;
   case(0x1C) : if(!ProcessVertexStride0x1C(bs, vb, bd, meshinfo)) return false; break;
   case(0x20) : if(!ProcessVertexStride0x20(bs, vb, bd, meshinfo)) return false; break;
   case(0x24) : if(!ProcessVertexStride0x24(bs, vb, bd, meshinfo)) return false; break;
   default : {
        stringstream ss;
        ss << "Unsupported vertex stride 0x" << hex << meshinfo.vtx_stride << dec << ".";
        return error(ss.str().c_str());
       }
  }

 //
 // INDEX BUFFER PROCESSING
 //

 // create binary stream
 bs = binary_stream(meshdata.ibdata, meshdata.ibsize);
 bs.set_endian_mode(meshdata.mode);

 // move to vertex data
 offset = meshinfo.base_index*sizeof(uint16);
 bs.seek(offset);
 if(bs.fail()) return error("Stream seek failure.", __LINE__);

 // initialize index buffer
 uint32 ib_size = meshinfo.indices * sizeof(uint16);
 ib.name = "default";
 ib.type = AMC_TRIANGLES;
 ib.wind = AMC_CW;
 ib.format = AMC_UINT16;
 ib.data.reset(new char[ib_size]);
 ib.elem = meshinfo.indices;

 // read index buffer
 uint16* ptr = reinterpret_cast<uint16*>(ib.data.get());
 bs.read_array(ptr, ib.elem);
 if(bs.fail()) return error("Stream read failure.", __LINE__);

 // modify base value (since we are not sharing index buffers)
 for(uint32 i = 0; i < ib.elem; i++)
     ptr[i] -= meshinfo.min_index;

 //
 // SURFACE PROCESSING
 //

 // create name
 stringstream ss;
 ss << setfill('0');
 ss << "m_" << setw(3) << meshinfo.index;
 if(meshinfo.type == 0x1020) ss << "_edge";
 else if(meshinfo.type == 0xE6C8) ss << "_lod";
 // if(meshinfo.lod != 0) ss << "_lod_" << setw(3) << hex << meshinfo.lod << dec;

 // initialize surface
 surface.name = ss.str().c_str();
 surface.surfmat = AMC_INVALID_SURFMAT;
 AMC_REFERENCE ref;
 ref.vb_index = meshinfo.index;
 ref.vb_start = 0;
 ref.vb_width = vb.elem;
 ref.ib_index = meshinfo.index;
 ref.ib_start = 0;
 ref.ib_width = ib.elem;
 ref.jm_index = AMC_INVALID_JMAP_INDEX;
 surface.refs.push_back(ref);

 return true;
}

}
#pragma endregion

//
// PC ARCHIVES
//
#pragma region CAPCOM_PC_ARCHIVES
namespace CAPCOM { namespace PC {

bool ExtractARC(LPCTSTR filename, LPCTSTR datapath, bool delete_file)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open ARC file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return error("Empty ARC file.");

 // read arc file
 boost::shared_array<char> filedata(new char[filesize]);
 ifile.read(filedata.get(), filesize);
 if(ifile.fail()) return error("Read failure.");

 // close arc file
 ifile.close();

 // binary stream
 binary_stream bs(filedata, filesize);
 bs.set_endian_mode(ENDIAN_LITTLE);

 // read magic 
 uint32 h01 = bs.read_uint32(); // 0x00435241
 uint16 h02 = bs.read_uint16(); // 0x07
 uint16 h03 = bs.read_uint16(); // number of files
 if(bs.fail()) return error("Stream read failure."); 
 if(h01 != 0x00435241) return error("Invalid magic number.");
 if(h03 == 0) return true; // nothing to do

 // file information
 struct ARCFILEINFO {
  char   p01[64]; // filename
  uint32 p02;     // unknown
  uint32 p03;     // deflate size
  uint32 p04;     // flags? 0x00000082 was uncompressed
  uint32 p05;     // offset
 };
 deque<ARCFILEINFO> filelist;

 // read entries
 for(uint32 i = 0; i < h03; i++) {
     ARCFILEINFO item;
     bs.read_array(&item.p01[0], 64);
     item.p02 = bs.read_uint32();
     item.p03 = bs.read_uint32();
     item.p04 = bs.read_uint32();
     item.p05 = bs.read_uint32();
     if(bs.fail()) return error("Read failure.", __LINE__);
     filelist.push_back(item);
    }

 // build directories and decompress data
 for(uint32 i = 0; i < h03; i++)
    {
     // build output filename
     STDSTRING ofname = datapath;
     ofname += Unicode08ToUnicode16(filelist[i].p01).get();
     ofname += TEXT(".capcom");
     if(!ofname.length()) return error("Invalid output filename.");

     // build output pathname
     STDSTRING ofpath = GetPathnameFromFilename(ofname.c_str()).get();
     if(!ofpath.length()) return error("Invalid output pathname.");

     // create directories recursively
     int result = SHCreateDirectoryEx(NULL, ofpath.c_str(), NULL);
     if(result == ERROR_SUCCESS) ;
     else if(result == ERROR_FILE_EXISTS) ;
     else if(result == ERROR_ALREADY_EXISTS) ;
     else if(result == ERROR_BAD_PATHNAME) return error("SHCreateDirectoryExA:ERROR_BAD_PATHNAME");
     else if(result == ERROR_FILENAME_EXCED_RANGE) return error("SHCreateDirectoryExA:ERROR_FILENAME_EXCED_RANGE");
     else if(result == ERROR_PATH_NOT_FOUND) return error("SHCreateDirectoryExA:ERROR_PATH_NOT_FOUND");
     else if(result == ERROR_CANCELLED) return error("SHCreateDirectoryExA:ERROR_CANCELLED");
     else return error("Unknown SHCreateDirectoryExA error.");

     // create output file
     ofstream ofile(ofname.c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // move to data offset
     uint32 curr_offset = filelist[i].p05;
     bs.seek(curr_offset);
     if(bs.fail()) return error("Stream seek failure.");

     // compute next offset
     uint32 next_offset = 0;
     if((i + 1) == h03) next_offset = filesize;
     else next_offset = filelist[i + 1].p05;

     // compute datasize for compression test
     if(next_offset < curr_offset) return error("Failed to compute data size from offsets.");
     uint32 datasize = next_offset - curr_offset;

     // first two bytes of data to determine compression
     uint08 b1 = bs.read_uint08();
     uint08 b2 = bs.read_uint08();
     if(bs.fail()) return error("Stream seek failure.");

     // move to data offset again
     bs.seek(curr_offset);
     if(bs.fail()) return error("Stream seek failure.");

     // decompress and save data
     if(isZLIB(b1, b2)) {
        if(!InflateZLIB(bs.c_pos(), datasize, ofile, 0)) {
           wcout << " " << ofname.c_str() << endl;
           wcout << " offset = 0x" << hex << curr_offset << dec << endl;
           return false;
          }
       }
     //  read and save data
     else {
        ofile.write(bs.c_pos(), datasize);
        if(ofile.fail()) return error("Write failure.");
       }

     // close and reopen output file
     ofile.close();
     ifstream cfile(ofname.c_str(), ios::binary);
     if(!cfile) return error("Failed to open CAPCOM file.");

     // read first four bytes
     uint32 test = LE_read_uint32(cfile);
     if(cfile.fail()) return error("Read failure.", __LINE__);

     // separate number into bytes
     uint08 p1 = (test & 0xFF000000) >> 0x18;
     uint08 p2 = (test & 0x00FF0000) >> 0x10;
     uint08 p3 = (test & 0x0000FF00) >> 0x08;
     uint08 p4 = (test & 0x000000FF);

     // digits in extension
     bool is_ext3 = (!p1 || (p1 == 0x5F)) && is_char_alphanumeric(p2) && is_char_alpha(p3) && is_char_alpha(p4);
     bool is_ext4 = is_char_alphanumeric(p1) && is_char_alpha(p2) && is_char_alpha(p3) && is_char_alpha(p4);

     // change file extension
     STDSTRING movename;
     if(is_ext3) {
        TCHAR ext[4];
        ext[0] = p4;
        ext[1] = p3;
        ext[2] = p2;
        ext[3] = 0;
        movename = ChangeFileExtension(ofname.c_str(), ext).get();
       }
     else if(is_ext4) {
        TCHAR ext[5];
        ext[0] = p4;
        ext[1] = p3;
        ext[2] = p2;
        ext[3] = p1;
        ext[4] = 0;
        movename = ChangeFileExtension(ofname.c_str(), ext).get();
       }
     else
        movename = ChangeFileExtension(ofname.c_str(), TEXT("unknown")).get();

     // close and delete previous file if it exists
     cfile.close();
     DeleteFile(movename.c_str());
     
     // rename file
     BOOL success = MoveFile(ofname.c_str(), movename.c_str());
     if(!success) return error("File renaming failure.");
    }

 // close and delete ARC file (optional)
 if(delete_file) DeleteFile(filename);
 return true;
}

bool ExtractARCFromPath(LPCTSTR pathname, bool delete_file)
{
 // build filename list
 cout << "Searching for .ARC files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".ARC"), pathname)) return true;

 // construct data path
 STDSTRING datapath = pathname;
 datapath += TEXT("EXTRACTED");
 datapath += TEXT("\\");

 // process ARC files
 bool break_on_errors = true;
 cout << "Processing .ARC files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractARC(filelist[i].c_str(), datapath.c_str(), delete_file)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

}}
#pragma endregion

//
// PC TEXTURES
//
#pragma region CAPCOM_PC_TEXTURES
namespace CAPCOM { namespace PC {

bool ExtractTEX(LPCTSTR filename, bool delete_file)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open ARC file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return error("Empty ARC file.");

 // read arc file
 boost::shared_array<char> filedata(new char[filesize]);
 ifile.read(filedata.get(), filesize);
 if(ifile.fail()) return error("Read failure.");

 // close arc file
 ifile.close();

 // binary stream
 binary_stream bs(filedata, filesize);
 bs.set_endian_mode(ENDIAN_LITTLE);

 // read header
 uint32 h01 = bs.read_uint32(); // magic
 uint32 h02 = bs.read_uint32(); // flags
 uint32 h03 = bs.read_uint32(); // properties
 uint32 h04 = bs.read_uint32(); // image format
 if(bs.fail()) return error("Stream read failure.");

 // cubemap
 bool cubemap = ((h04 & 0xFF) == 0x06);
 if(cubemap) {
    cout << "CUBEMAP... SKIPPING" << endl;
    return true;
   }

 // derive dx, dy, and mipmaps
 uint32 dx = ((h03 & 0x000FFF00) >>  6);
 uint32 dy = ((h03 & 0xFFF00000) >> 19);
 uint32 n_images = (h03 & 0xFF);
 if(!n_images) return true; // nothing to do
 
 // read mipmap offsets
 deque<uint32> offsets;
 for(uint32 i = 0; i < n_images; i++) {
     uint32 temp = bs.read_uint32();
     offsets.push_back(temp);
     if(bs.fail()) return error("Stream read failure.");
    }

 // move to first offset
 bs.seek(offsets[0]);
 if(bs.fail()) return error("Seek failure.");

 // image variables
 uint32 format = ((h04 & 0xFF00) >> 8);

 // FORMAT 0x0E
 // FLOATING-POINT FORMAT
 if(format == 0x0E)
   {
    // read data
    uint32 n_pixels = dx*dy;
    uint32 datasize = 4*n_pixels;

    // reverse floating-point byte-order
    const real32* src = reinterpret_cast<const real32*>(bs.c_pos());
    // reverse_byte_order(src, pixels);

    // convert D3DFMT_R32F to D3DFMT_R8G8B8A8 data
    boost::shared_array<char> dst(new char[datasize]);
    for(uint32 i = 0; i < n_pixels; i++) {
        uint32 src_index = i;
        uint32 dst_index = i * 4;
        real32 c1 = 255.0f * src[src_index + 0];
        real32 c2 = 255.0f * src[src_index + 1];
        real32 c3 = 255.0f * src[src_index + 2];
        real32 c4 = 255.0f * src[src_index + 3];
        dst[dst_index + 0] = (uint08)(c1 < 0 ? 0 : (c1 > 255 ? 255 : c1));
        dst[dst_index + 1] = (uint08)(c2 < 0 ? 0 : (c2 > 255 ? 255 : c2));
        dst[dst_index + 2] = (uint08)(c3 < 0 ? 0 : (c3 > 255 ? 255 : c3));
        dst[dst_index + 3] = (uint08)(c4 < 0 ? 0 : (c4 > 255 ? 255 : c4));
       }

    // create DDS header
    DDS_HEADER ddsh;
    if(!CreateR8G8B8A8DDSHeader(dx, dy, 0, FALSE, &ddsh))
       return error("Failed to create DDS header.");

    // create ouptfile
    STDSTRING ofname = ChangeFileExtension(filename, TEXT("dds")).get();
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");
    
    // save DDS header
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    if(ofile.fail()) return error("Write failure.");
    
    // save data
    ofile.write(dst.get(), datasize);
    if(ofile.fail()) return error("Write failure.");
   }
 // FORMAT 0x13, 0x14, 0x19
 // 0x13 - [PC] DMC3
 // 0x19 - [PC] DMC3 (weird, supposed to be a spec map)
 // DXT1
 else if(format == 0x13 || format == 0x14 || format == 0x19)
   {
    // create DDS header
    DDS_HEADER ddsh;
    if(!CreateDXT1Header(dx, dy, 0, FALSE, &ddsh)) return error("Failed to create DDS header.");

    // create ouptfile
    STDSTRING ofname = ChangeFileExtension(filename, TEXT("dds")).get();
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");

    // // reverse byte order?
    // if(format == 0x19) {
    //    uint32 datasize = DXT1Filesize(dx, dy, 0);
    //    reverse_byte_order(reinterpret_cast<uint32*>(&bs[bs.tell()]), datasize/4);
    //   }
    
    // save DDS header
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    if(ofile.fail()) return error("Write failure.");
    
    // save data
    uint32 datasize = DXT1Filesize(dx, dy, 0);
    ofile.write(bs.c_pos(), datasize);
    if(ofile.fail()) return error("Write failure.");
   }
 // FORMAT 0x18, 0x1F, 0x2B
 // DXT5
 else if(format == 0x18 || format == 0x1F || format == 0x2B)
   {
    // 0x1F is a normal map format
    // 0x2B is a strange one, used for fonts
    // if(format == 0x2B) delete_file = false;
    // else delete_file = true;

    // create DDS header
    DDS_HEADER ddsh;
    if(!CreateDXT5Header(dx, dy, 0, FALSE, &ddsh)) return error("Failed to create DDS header.");

    // create ouptfile
    STDSTRING ofname = ChangeFileExtension(filename, TEXT("dds")).get();
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");
    
    // save DDS header
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    if(ofile.fail()) return error("Write failure.");
    
    // save data
    uint32 datasize = DXT5Filesize(dx, dy, 0);
    ofile.write(bs.c_pos(), datasize);
    if(ofile.fail()) return error("Write failure.");
   }
 // FORMAT 0x28
 // MORTON SWIZZLED
 else if(format == 0x27 || format == 0x28)
   {
    // read data
    uint32 datasize = 4*dx*dy;
    boost::shared_array<char> data(new char[datasize]);
    bs.read(data.get(), datasize);
    if(bs.fail()) return error("Stream read failure.");

    // unswizzle data
    boost::shared_array<char> copy(new char[datasize]);
    if(dx == dy) {
       for(uint32 r = 0; r < dy; r++) {
           for(uint32 c = 0; c < dx; c++) {
               uint32 morton = array_to_morton(r, c);
               uint32 copy_position = 4*r*dx + 4*c;
               uint32 data_position = 4*morton;
               copy[copy_position + 0] = data[data_position + 0];
               copy[copy_position + 1] = data[data_position + 1];
               copy[copy_position + 2] = data[data_position + 2];
               copy[copy_position + 3] = data[data_position + 3];
              }
          }
       data = copy;
      }
    else {
       // what about non-square images?
       // just ignore for now
       cout << "NONSQUARE TYPE 0x28" << endl;
      }

    // create DDS header
    DDS_HEADER ddsh;
    if(!CreateR8G8B8X8DDSHeader(dx, dy, 0, FALSE, &ddsh))
       return error("Failed to create DDS header.");

    // create ouptfile
    STDSTRING ofname = ChangeFileExtension(filename, TEXT("dds")).get();
    ofstream ofile(ofname.c_str(), ios::binary);
    if(!ofile) return error("Failed to create output file.");
    
    // save DDS header
    ofile.write("DDS ", 4);
    ofile.write((char*)&ddsh, sizeof(ddsh));
    if(ofile.fail()) return error("Write failure.");
    
    // save data
    ofile.write(data.get(), datasize);
    if(ofile.fail()) return error("Write failure.");
   }
 else {
    std::stringstream ss;
    ss << "Unknown image format 0x" << hex << format << dec << ".";
    return error(ss.str().c_str());
   }

 // delete file
 if(delete_file) DeleteFile(filename);
 return true;
}

bool ExtractTEXFromPath(LPCTSTR pathname, bool delete_file)
{
 // build filename list
 cout << "Searching for .TEX files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".TEX"), pathname)) return true;

 // process TEX files
 bool break_on_errors = true;
 cout << "Processing .TEX files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractTEX(filelist[i].c_str(), delete_file)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

}}
#pragma endregion

//
// PS3 ARCHIVES
//
namespace CAPCOM { namespace PS3 {

bool ExtractARC(LPCTSTR filename, LPCTSTR datapath, bool delete_file)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open ARC file.");

 // filename properties
 STDSTRING pathname = GetPathnameFromFilename(filename).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename).get();

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");
 if(filesize == 0) return error("Empty ARC file.");

 // read arc file
 boost::shared_array<char> filedata(new char[filesize]);
 ifile.read(filedata.get(), filesize);
 if(ifile.fail()) return error("Read failure.");

 // close arc file
 ifile.close();

 // binary stream
 binary_stream bs(filedata, filesize);
 bs.set_endian_mode(ENDIAN_BIG);

 // determine ARC file type
 uint32 arc_type = 0;
 uint32 offset_bytes = 0;
 uint32 magic = bs.peek_uint32();
 if(magic == 0x00435241) arc_type = 1;
 if(magic == 0x00534648) {
    arc_type = 2;
    offset_bytes = 0x10;
   }

 // skip SFH header
 if(arc_type == 2) {
    bs.seek(offset_bytes);
    if(bs.fail()) return error("Stream seek failure.");
   }

 // read magic number
 uint32 h01 = bs.read_uint32(); // magic
 uint16 h02 = bs.read_uint16(); // 0x08
 uint16 h03 = bs.read_uint16(); // number of files
 if(bs.fail()) return error("Stream read failure."); 
 if(h01 != 0x435241) return error("Invalid magic number.");
 if(h03 == 0) return true; // nothing to do

 // file information
 struct ARCFILEINFO {
  char   p01[64]; // filename
  uint32 p02;     // unknown
  uint32 p03;     // deflate size
  uint32 p04;     // flags? 0x00000082 was uncompressed
  uint32 p05;     // offset
 };
 deque<ARCFILEINFO> filelist;

 // read entries
 for(uint32 i = 0; i < h03; i++) {
     ARCFILEINFO item;
     bs.read_array(&item.p01[0], 64);
     item.p02 = bs.read_uint32();
     item.p03 = bs.read_uint32();
     item.p04 = bs.read_uint32();
     item.p05 = bs.read_uint32();
     if(bs.fail()) return error("Read failure.", __LINE__);
     filelist.push_back(item);
    }

 // build directories and decompress data
 for(uint32 i = 0; i < h03; i++)
    {
     // build output filename
     STDSTRING ofname = datapath;
     ofname += Unicode08ToUnicode16(filelist[i].p01).get();
     ofname += TEXT(".capcom");
     if(!ofname.length()) return error("Invalid output filename.");

     // build output pathname
     STDSTRING ofpath = GetPathnameFromFilename(ofname.c_str()).get();
     if(!ofpath.length()) return error("Invalid output pathname.");

     // create directories recursively
     int result = SHCreateDirectoryEx(NULL, ofpath.c_str(), NULL);
     if(result == ERROR_SUCCESS) ;
     else if(result == ERROR_FILE_EXISTS) ;
     else if(result == ERROR_ALREADY_EXISTS) ;
     else if(result == ERROR_BAD_PATHNAME) return error("SHCreateDirectoryExA:ERROR_BAD_PATHNAME");
     else if(result == ERROR_FILENAME_EXCED_RANGE) return error("SHCreateDirectoryExA:ERROR_FILENAME_EXCED_RANGE");
     else if(result == ERROR_PATH_NOT_FOUND) return error("SHCreateDirectoryExA:ERROR_PATH_NOT_FOUND");
     else if(result == ERROR_CANCELLED) return error("SHCreateDirectoryExA:ERROR_CANCELLED");
     else return error("Unknown SHCreateDirectoryExA error.");

     // create output file
     ofstream ofile(ofname.c_str(), ios::binary);
     if(!ofile) return error("Failed to create output file.");

     // move to data offset
     uint32 curr_offset = filelist[i].p05 + offset_bytes;
     bs.seek(curr_offset);
     if(bs.fail()) return error("Stream seek failure.");

     // compute next offset
     uint32 next_offset = 0;
     if((i + 1) == h03) next_offset = filesize;
     else next_offset = filelist[i + 1].p05 + offset_bytes;

     // compute datasize for compression test
     // do not use datasize as it is unreliable
     if(next_offset < curr_offset) return error("Failed to compute data size from offsets.");
     uint32 datasize = next_offset - curr_offset;

     // first two bytes of data to determine compression
     uint08 b1 = bs.read_uint08();
     uint08 b2 = bs.read_uint08();
     if(bs.fail()) return error("Stream seek failure.");

     // move to data offset again
     bs.seek(curr_offset);
     if(bs.fail()) return error("Stream seek failure.");

     // decompress and save data
     if(isZLIB(b1, b2) && !InflateZLIB(bs.c_pos(), datasize, ofile, 0)) {
        wcout << " " << ofname.c_str() << endl;
        wcout << " offset = 0x" << hex << curr_offset << dec << endl;
        return false; // error
       }
     //  read and save data
     else {
        ofile.write(bs.c_pos(), datasize);
        if(ofile.fail()) return error("Write failure.");
       }

     // close and reopen output file
     ofile.close();
     ifstream cfile(ofname.c_str(), ios::binary);
     if(!cfile) return error("Failed to open CAPCOM file.");

     // read first four bytes
     uint32 test = BE_read_uint32(cfile);
     if(cfile.fail()) return error("Read failure.", __LINE__);

     // separate number into bytes
     uint08 p1 = (test & 0xFF000000) >> 0x18;
     uint08 p2 = (test & 0x00FF0000) >> 0x10;
     uint08 p3 = (test & 0x0000FF00) >> 0x08;
     uint08 p4 = (test & 0x000000FF);

     // digits in extension
     bool is_ext3 = (!p1 || (p1 == 0x5F)) && is_char_alphanumeric(p2) && is_char_alpha(p3) && is_char_alpha(p4);
     bool is_ext4 = is_char_alphanumeric(p1) && is_char_alpha(p2) && is_char_alpha(p3) && is_char_alpha(p4);

     // change file extension
     STDSTRING movename;
     if(is_ext3) {
        TCHAR ext[4];
        ext[0] = p4;
        ext[1] = p3;
        ext[2] = p2;
        ext[3] = 0;
        movename = ChangeFileExtension(ofname.c_str(), ext).get();
       }
     else if(is_ext4) {
        TCHAR ext[5];
        ext[0] = p4;
        ext[1] = p3;
        ext[2] = p2;
        ext[3] = p1;
        ext[4] = 0;
        movename = ChangeFileExtension(ofname.c_str(), ext).get();
       }
     else
        movename = ChangeFileExtension(ofname.c_str(), TEXT("unknown")).get();

     // close and delete previous file if it exists
     cfile.close();
     DeleteFile(movename.c_str());
     
     // rename file
     BOOL success = MoveFile(ofname.c_str(), movename.c_str());
     if(!success) return error("File renaming failure.");
    }

 // close and delete ARC file (optional)
 // if(delete_file) {
 //    ifile.close();
 //    DeleteFile(filename);
 //   }

 return true;
}

bool ExtractARCFromPath(LPCTSTR pathname, bool delete_file)
{
 // build filename list
 cout << "Searching for .ARC files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".ARC"), pathname)) return true;

 // construct data path
 STDSTRING datapath = pathname;
 datapath += TEXT("EXTRACTED");
 datapath += TEXT("\\");

 // process ARC files
 bool break_on_errors = true;
 cout << "Processing .ARC files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractARC(filelist[i].c_str(), datapath.c_str(), delete_file)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

}}

//
// PS3 TEXTURES
//
namespace CAPCOM { namespace PS3 {

bool ExtractTEX(LPCTSTR filename, bool delete_file)
{
 // open file
 ifstream ifile(filename, ios::binary);
 if(!ifile) return error("Failed to open file.");

 // get filesize
 ifile.seekg(0, ios::end);
 uint32 filesize = (uint32)ifile.tellg();
 ifile.seekg(0, ios::beg);
 if(ifile.fail()) return error("Seek failure.");

 // read magic
 uint32 magic = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // read unk01
 uint32 unk01 = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // TODO: cubemaps
 if(unk01 == 0x6000009A) return true; // data always starts at 0x10

 // dx - dy - mipmaps
 uint32 props = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");
 uint32 dx = ((props & 0x000FFF00) >>  6);
 uint32 dy = ((props & 0xFFF00000) >> 19);
 uint32 mipmaps = (props & 0xFF) - 1;
 // cout << "dx = " << dx << endl;
 // cout << "dy = " << dy << endl;
 // cout << "mipmaps = " << mipmaps << endl;

 // read type
 // type is read as 00 - 01 - 14 - 06 (?? - ?? - DDS type - ??)
 uint32 type = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // cubemap
 bool cubemap = ((type & 0xFF) == 0x06);
 if(cubemap) {
    cout << "CUBEMAP... SKIPPING" << endl;
    return true;
   }

 // offset to first mipmap
 uint32 start = BE_read_uint32(ifile);
 if(ifile.fail()) return error("Read failure.");

 // move to first offset
 ifile.seekg(start);
 if(ifile.fail()) return error("Seek failure.");

 // read data
 if(filesize < start) return error("Texture filesize is less than the start offset.");
 uint32 datasize = filesize - start;
 if(!datasize) return error("Invalid texture data size.");
 boost::shared_array<char> data(new char[datasize]);
 ifile.read(data.get(), datasize);
 if(ifile.fail()) return error("Read failure.");

 // create DDS header
 DDS_HEADER ddsh;
 uint32 ddstype = ((type & 0xFF00) >> 8);

 // OK (alpha masks)
 if(ddstype == 0x0E)
   {
    // reverse floating-point byte-order
    uint32 pixels = dx * dy;
    real32* src = reinterpret_cast<real32*>(data.get());
    reverse_byte_order(src, pixels);

    // convert D3DFMT_R32F to D3DFMT_R8G8B8A8 data
    boost::shared_array<char> dst(new char[datasize]);
    for(uint32 i = 0; i < pixels; i++) {
        uint32 src_index = i;
        uint32 dst_index = i * 4;
        real32 c1 = 255.0f * src[src_index + 0];
        real32 c2 = 255.0f * src[src_index + 1];
        real32 c3 = 255.0f * src[src_index + 2];
        real32 c4 = 255.0f * src[src_index + 3];
        dst[dst_index + 0] = (uint08)(c1 < 0 ? 0 : (c1 > 255 ? 255 : c1));
        dst[dst_index + 1] = (uint08)(c2 < 0 ? 0 : (c2 > 255 ? 255 : c2));
        dst[dst_index + 2] = (uint08)(c3 < 0 ? 0 : (c3 > 255 ? 255 : c3));
        dst[dst_index + 3] = (uint08)(c4 < 0 ? 0 : (c4 > 255 ? 255 : c4));
       }
    data = dst;

    // create DDS header
    if(!CreateR8G8B8A8DDSHeader(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 else if(ddstype == 0x13) {
    // cout << "TYPE 0x13" << endl;
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 else if(ddstype == 0x14) {
    // cout << "TYPE 0x14" << endl;
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 else if(ddstype == 0x17) {
cout << "TYPE 0x17" << endl;
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 else if(ddstype == 0x18) {
    // cout << "TYPE 0x18" << endl;
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 else if(ddstype == 0x19) {
    // cout << "TYPE 0x19" << endl;
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (blue normal maps _DM and _NM)
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 else if(ddstype == 0x1E) {
    // cout << "TYPE 0x1E" << endl;
    if(!CreateDXT1Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (pink normal maps _DM_HQ and _NM_HQ)
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 else if(ddstype == 0x1F) {
    // cout << "TYPE 0x1F" << endl;
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (interior textures)
 else if(ddstype == 0x23) {
cout << "TYPE 0x23" << endl;
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (crack normal maps)
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 else if(ddstype == 0x25) {
    // cout << "TYPE 0x25" << endl;
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (noise)
 else if(ddstype == 0x27) {
cout << "TYPE 0x27" << endl;
    dx = ((props & 0x0000FF00) >>  6);
    dy = ((props & 0xFFFF0000) >> 19);
    if(!CreateR8G8B8X8DDSHeader(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK (SWIZZLED UNFINISHED FOR NONSQUARE IMAGES)
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 else if(ddstype == 0x28)
   {
    // cout << "TYPE 0x28" << endl;
    // deswizzle
    if(dx == dy) {
       boost::shared_array<char> copy(new char[datasize]);
       for(uint32 r = 0; r < dy; r++) {
           for(uint32 c = 0; c < dx; c++) {
               uint32 morton = array_to_morton(r, c);
               uint32 copy_position = 4*r*dx + 4*c;
               uint32 data_position = 4*morton;
               copy[copy_position + 0] = data[data_position + 0];
               copy[copy_position + 1] = data[data_position + 1];
               copy[copy_position + 2] = data[data_position + 2];
               copy[copy_position + 3] = data[data_position + 3];
              }
          }
       data = copy;
      }
    else {
       // what about non-square images?
       // just ignore for now
       cout << "NONSQUARE TYPE 0x28" << endl;
       return true;
      }

    // create DDS header
    if(!CreateR8G8B8X8DDSHeader(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 // EITHER DXT3 or DXT5 (channels are weird)
 else if(ddstype == 0x2B)
   {
    // cout << "TYPE 0x2B" << endl;
    if(!CreateDXT5Header(dx, dy, 0, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 // OK
 // WORKS: DRAGON'S DOGMA DARK ARISEN
 else if(ddstype == 0x2F) {
    // cout << "TYPE 0x2F" << endl;
    if(!CreateDXT5Header(dx, dy, mipmaps, FALSE, &ddsh))
       return error("Failed to create DDS header.");
   }
 else {
    cout << "UNKNOWN TYPE 0x" << hex << ddstype << dec << endl;
    cout << "dx = " << dx << endl;
    cout << "dy = " << dy << endl;
    cout << "mipmaps = " << mipmaps << endl;
    return error("Unknown texture format.");
   }

 // create ouptfile
 STDSTRING ofname = ChangeFileExtension(filename, TEXT("dds")).get();
 ofstream ofile(ofname.c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 // save DDS header
 ofile.write("DDS ", 4);
 ofile.write((char*)&ddsh, sizeof(ddsh));
 if(ofile.fail()) return error("Write failure.");

 // save data
 ofile.write(data.get(), datasize);
 if(ofile.fail()) return error("Write failure.");

 // close and delete input file
 if(delete_file) {
    ifile.close();
    DeleteFile(filename);
   }

 return true;
}

bool ExtractTEXFromPath(LPCTSTR pathname, bool delete_file)
{
 // build filename list
 cout << "Searching for .TEX files... please wait." << endl;
 deque<STDSTRING> filelist;
 if(!BuildFilenameList(filelist, TEXT(".TEX"), pathname)) return true;

 // process TEX files
 bool break_on_errors = true;
 cout << "Processing .TEX files..." << endl;
 for(size_t i = 0; i < filelist.size(); i++) {
     wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
     if(!ExtractTEX(filelist[i].c_str(), delete_file)) {
        if(break_on_errors) return false;
       }
    }
 cout << endl;
 return true;
}

}}