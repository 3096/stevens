#ifndef __CS_MATH_CARTESIAN3D_H
#define __CS_MATH_CARTESIAN3D_H

#include<cs/core/types.h>
#include<cs/math/constants.h>
#include<cs/math/error.h>

namespace cs { namespace math {

template<class T>
struct cartesian3D {
 public :
  typedef T value_type;
  value_type origin[3];
  value_type x[3];
  value_type y[3];
  value_type z[3];
 public :
  template<class ZeroTest> bool fromXY(const T* px, const T* py, ZeroTest zt);
  template<class ZeroTest> bool fromXZ(const T* px, const T* pz, ZeroTest zt);
  template<class ZeroTest> bool fromYX(const T* py, const T* px, ZeroTest zt);
  template<class ZeroTest> bool fromYZ(const T* py, const T* pz, ZeroTest zt);
  template<class ZeroTest> bool fromZX(const T* pz, const T* px, ZeroTest zt);
  template<class ZeroTest> bool fromZY(const T* pz, const T* py, ZeroTest zt);
  bool fromXY(const T* px, const T* py);
  bool fromXZ(const T* px, const T* pz);
  bool fromYX(const T* py, const T* px);
  bool fromYZ(const T* py, const T* pz);
  bool fromZX(const T* pz, const T* px);
  bool fromZY(const T* pz, const T* py);
 // TODO: remove constructors
 public :
  cartesian3D();
  cartesian3D(const T* px, const T* py);
  cartesian3D(const T& x0, const T& x1, const T& x2, const T& y0, const T& y1, const T& y2);
};

template<class T>
inline cartesian3D<T>::cartesian3D()
{
 const T c0 = constants<T>::zero();
 const T c1 = constants<T>::one();
 origin[0] = c0;
 origin[1] = c0;
 origin[2] = c0;
 x[0] = c1;
 x[1] = c0;
 x[2] = c0;
 y[0] = c0;
 y[1] = c1;
 y[2] = c0;
 z[0] = c0;
 z[1] = c0;
 z[2] = c1;
}

template<class T>
inline cartesian3D<T>::cartesian3D(const T* px, const T* py)
{
 fromXY(px, py);
}

template<class T>
inline cartesian3D<T>::cartesian3D(const T& x0, const T& x1, const T& x2, const T& y0, const T& y1, const T& y2)
{
 x[0] = x0; x[1] = x1; x[2] = x2;
 y[0] = y0; y[1] = y1; y[2] = y2;
 fromXY(x, y);
}

template<class T> template<class ZeroTest>
inline bool cartesian3D<T>::fromXY(const T* px, const T* py, ZeroTest zt)
{
 // normalize X-axis
 T X[3];
 if(!vector3D_normalize(X, px, zt)) return false;

 // normalize Y-axis
 T Y[3];
 if(!vector3D_normalize(Y, py, zt)) return false;

 // compute Z = X cross Y
 T Z[3];
 vector3D_vector_product(Z, X, Y);
 if(!vector3D_normalize(Z, zt)) return false;

 // assign X and Z and recompute Y = Z cross X
 vector3D_copy(x, X);
 vector3D_copy(z, Z);
 vector3D_vector_product(y, z, x);
 return true;
}

template<class T> template<class ZeroTest>
inline bool cartesian3D<T>::fromXZ(const T* px, const T* pz, ZeroTest zt)
{
 // normalize X-axis
 T X[3];
 if(!vector3D_normalize(X, px, zt)) return false;

 // normalize Z-axis
 T Z[3];
 if(!vector3D_normalize(Z, pz, zt)) return false;

 // compute Y = Z cross X
 T Y[3];
 vector3D_vector_product(Y, Z, X);
 if(!vector3D_normalize(Y, zt)) return false;

 // assign X and Y and recompute Z = X cross Y
 vector3D_copy(x, X);
 vector3D_copy(y, Y);
 vector3D_vector_product(z, x, y);
 return true;
}

template<class T> template<class ZeroTest>
inline bool cartesian3D<T>::fromYX(const T* py, const T* px, ZeroTest zt)
{
 return fromXY(px, py, zero_test<T>());
}

template<class T> template<class ZeroTest>
inline bool cartesian3D<T>::fromYZ(const T* py, const T* pz, ZeroTest zt)
{
 // normalize Y-axis
 T Y[3];
 if(!vector3D_normalize(Y, py, zt)) return false;

 // normalize Z-axis
 T Z[3];
 if(!vector3D_normalize(Z, pz, zt)) return false;

 // compute X = Y cross Z 
 T X[3];
 vector3D_vector_product(X, Y, Z);
 if(!vector3D_normalize(X, zt)) return false;

 // assign X and Y and recompute Z = X cross Y
 vector3D_copy(x, X);
 vector3D_copy(y, Y);
 vector3D_vector_product(z, x, y);
 return true;
}

template<class T> template<class ZeroTest>
inline bool cartesian3D<T>::fromZX(const T* pz, const T* px, ZeroTest zt)
{
 return fromXZ(px, pz, zero_test<T>());
}

template<class T> template<class ZeroTest>
inline bool cartesian3D<T>::fromZY(const T* pz, const T* py, ZeroTest zt)
{
 return fromYZ(py, pz, zero_test<T>());
}

template<class T>
inline bool cartesian3D<T>::fromXY(const T* px, const T* py)
{
 return fromXY(px, py, zero_test<T>());
}

template<class T>
inline bool cartesian3D<T>::fromXZ(const T* px, const T* pz)
{
 return fromXY(px, pz, zero_test<T>());
}

template<class T>
inline bool cartesian3D<T>::fromYX(const T* py, const T* px)
{
 return fromXY(px, py);
}

template<class T>
inline bool cartesian3D<T>::fromYZ(const T* py, const T* pz)
{
 return true;
}

template<class T>
inline bool cartesian3D<T>::fromZX(const T* pz, const T* px)
{
 return fromXZ(px, pz);
}

template<class T>
inline bool cartesian3D<T>::fromZY(const T* pz, const T* py)
{
 return fromZY(pz, py);
}

template<class C, class T>
inline std::basic_ostream<C>& operator <<(std::basic_ostream<C>& os, const cartesian3D<T>& obj)
{
 os << "origin<" << obj.origin[0] << ", " << obj.origin[1] << ", " << obj.origin[2] << "> - ";
 os << "x<" << obj.x[0] << ", " << obj.x[1] << ", " << obj.x[2] << "> - ";
 os << "y<" << obj.y[0] << ", " << obj.y[1] << ", " << obj.y[2] << "> - ";
 os << "z<" << obj.z[0] << ", " << obj.z[1] << ", " << obj.z[2] << ">";
 return os;
}

template<class T>
inline void transform(T* X, cartesian3D<T>& from, const cartesian3D<T>& to)
{
 // translate "from" to "reference"
 T A[16] = {
  1, 0, 0, from.origin[0],
  0, 1, 0, from.origin[1],
  0, 0, 1, from.origin[2],
  0, 0, 0,              1,
 };
 // transform "from" to "reference"
 T B[16] = {
  from.x[0], from.x[1], from.x[2], 0,
  from.y[0], from.y[1], from.y[2], 0,
  from.z[0], from.z[1], from.z[2], 0,
          0,         0,         0, 1,
 };
 // transform to new frame
 T C[16] = {
  to.x[0], to.y[0], to.x[0], 0,
  to.x[1], to.y[1], to.x[1], 0,
  to.x[2], to.y[2], to.z[2], 0,
        0,       0,       0, 1,
 };
 // move to new origin
 T D[16] = {
  1, 0, 0, -to.origin[0],
  0, 1, 0, -to.origin[1],
  0, 0, 1, -to.origin[2],
  0, 0, 0,            1,
 };
 // result = D*C*B*A
 matrix4x4_mul(X, D, C); // X = D*C
 matrix4x4_mul(X, B);    // X = (D*C)*B
 matrix4x4_mul(X, A);    // X = (D*C*B)*A
 return;

 // a b c d   A B C D = (a*A + b*E + c*I + d*M) (a*B + b*F + c*J + d*N) (a*C + b*G + c*K + d*O) (a*D + b*H + c*L + d*P)
 // e f g h   E F G H = (e*A + f*E + g*I + h*M) (e*B + f*F + g*J + h*N) (e*C + f*G + g*K + h*O) (e*D + f*H + g*L + h*P)
 // i j k l   I J K L = (i*A + j*E + k*I + l*M) (i*B + j*F + k*J + l*N) (i*C + j*G + k*K + l*O) (i*D + j*H + k*L + l*P)  
 // m n o p   M N O P = (m*A + n*E + o*I + p*M) (m*B + n*F + o*J + p*N) (m*C + n*G + o*K + p*O) (m*D + n*H + o*L + p*P)

 // constants
 const T zer = constants<T>::zero();
 const T one = constants<T>::one();

 // 1st row
 X[0x0] = to.x[0]*from.x[0] + to.x[1]*from.x[1] + to.x[2]*from.x[2];
 X[0x1] = to.x[0]*from.y[0] + to.x[1]*from.y[1] + to.x[2]*from.y[2];
 X[0x2] = to.x[0]*from.z[0] + to.x[1]*from.z[1] + to.x[2]*from.z[2];
 X[0x3] = to.origin[0] - X[0x0]*from.origin[0] - X[0x1]*from.origin[1] - X[0x2]*from.origin[2];

 // 2nd row
 X[0x4] = to.y[0]*from.x[0] + to.y[1]*from.x[1] + to.y[2]*from.x[2];
 X[0x5] = to.y[0]*from.y[0] + to.y[1]*from.y[1] + to.y[2]*from.y[2];
 X[0x6] = to.y[0]*from.z[0] + to.y[1]*from.z[1] + to.y[2]*from.z[2];
 X[0x7] = to.origin[1] - X[0x4]*from.origin[0] - X[0x5]*from.origin[1] - X[0x6]*from.origin[2];

 // 3rd row
 X[0x8] = to.z[0]*from.x[0] + to.z[1]*from.x[1] + to.z[2]*from.x[2];
 X[0x9] = to.z[0]*from.y[0] + to.z[1]*from.y[1] + to.z[2]*from.y[2];
 X[0xA] = to.z[0]*from.z[0] + to.z[1]*from.z[1] + to.z[2]*from.z[2];
 X[0xB] = to.origin[2] - X[0x8]*from.origin[0] - X[0x9]*from.origin[1] - X[0xA]*from.origin[2];

 // 4th row
 X[0xC] = zer;
 X[0xD] = zer;
 X[0xE] = zer;
 X[0xF] = one;
}

}}

#endif
