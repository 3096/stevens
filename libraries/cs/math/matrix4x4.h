#ifndef __CS_MATH_MATRIX4X4__
#define __CS_MATH_MATRIX4X4__

// CS headers
#include<cs/math/functions.h>
#include<cs/math/vector3D.h>

//
// FUNCTIONS
//
namespace cs { namespace math {


template<class char_type, class T>
void matrix4x4_print(std::basic_ostream<char_type>& os, const T* A)
{
 os << "row[0][" << A[0x0] << ", " << A[0x1] << ", " << A[0x2] << ", " << A[0x3] << "] - ";
 os << "row[1][" << A[0x4] << ", " << A[0x5] << ", " << A[0x6] << ", " << A[0x7] << "] - ";
 os << "row[2][" << A[0x8] << ", " << A[0x9] << ", " << A[0xA] << ", " << A[0xB] << "] - ";
 os << "row[3][" << A[0xC] << ", " << A[0xD] << ", " << A[0xE] << ", " << A[0xF] << "]";
}

template<class char_type, class T>
void matrix4x4_println(std::basic_ostream<char_type>& os, const T* A)
{
 os << "row[0][" << A[0x0] << ", " << A[0x1] << ", " << A[0x2] << ", " << A[0x3] << "] - ";
 os << "row[1][" << A[0x4] << ", " << A[0x5] << ", " << A[0x6] << ", " << A[0x7] << "] - ";
 os << "row[2][" << A[0x8] << ", " << A[0x9] << ", " << A[0xA] << ", " << A[0xB] << "] - ";
 os << "row[3][" << A[0xC] << ", " << A[0xD] << ", " << A[0xE] << ", " << A[0xF] << "]" << std::endl;
}

}}

namespace cs { namespace math {

//
// FORWARD DECLARATIONS
//

template<class T> class matrix4x4;
template<> class matrix4x4<binary32>;
template<> class matrix4x4<binary64>;

//
// BASE CLASSES
//
typedef struct _c_smatrix4x4 { binary32 m[16]; } c_smatrix4x4;
typedef struct _c_dmatrix4x4 { binary64 m[16]; } c_dmatrix4x4;
template<class T> struct c_tmatrix4x4 { T m[16]; };

//
// MATRIX
//

template<class T>
class matrix4x4 : public c_tmatrix4x4<T> {
 public :
  static const int m11 = 0x0;
  static const int m12 = 0x1;
  static const int m13 = 0x2;
  static const int m14 = 0x3;
  static const int m21 = 0x4;
  static const int m22 = 0x5;
  static const int m23 = 0x6;
  static const int m24 = 0x7;
  static const int m31 = 0x8;
  static const int m32 = 0x9;
  static const int m33 = 0xA;
  static const int m34 = 0xB;
  static const int m41 = 0xC;
  static const int m42 = 0xD;
  static const int m43 = 0xE;
  static const int m44 = 0xF;
 public :
  typedef T value_type;
  typedef T array_type[16];
 public :
  matrix4x4();
  explicit matrix4x4(const T& fill);
  explicit matrix4x4(const T* m);
  matrix4x4(const matrix4x4<T>& other);
  template<class X> explicit matrix4x4(const matrix4x4<X>& other);
 public :
  array_type& get(void) { return this->m; }
  const array_type& get(void)const { return this->m; }
 public :
  const T& at(int r, int c)const;
  T& at(int r, int c);
  T determinant(void)const;
  T invert(void);
  void transpose(void);
 public :
  const T& operator [](size_t index)const;
  T& operator [](size_t index);
 public :
  const T& operator ()(int r, int c)const;
  T& operator ()(int r, int c);
 public :
  matrix4x4<T>& operator =(const matrix4x4<T>& other);
  template<class X> matrix4x4<T>& operator =(const matrix4x4<X>& other);
 public :
  matrix4x4<T>& operator *=(const T& scalar);
  matrix4x4<T>& operator /=(const T& scalar);
 public :
  matrix4x4<T>& operator +=(const matrix4x4<T>& other);
  matrix4x4<T>& operator -=(const matrix4x4<T>& other);
  matrix4x4<T>& operator *=(const matrix4x4<T>& other);
  template<class X> matrix4x4<T>& operator +=(const matrix4x4<X>& other);
  template<class X> matrix4x4<T>& operator -=(const matrix4x4<X>& other);
  template<class X> matrix4x4<T>& operator *=(const matrix4x4<X>& other);
};

template<class T>
inline matrix4x4<T>::matrix4x4()
{
}

template<class T>
inline matrix4x4<T>::matrix4x4(const T& fill)
{
 this->m[ 0] = this->m[ 1] = this->m[ 2] = this->m[ 3] = fill;
 this->m[ 4] = this->m[ 5] = this->m[ 6] = this->m[ 7] = fill;
 this->m[ 8] = this->m[ 9] = this->m[10] = this->m[11] = fill;
 this->m[12] = this->m[13] = this->m[14] = this->m[15] = fill;
}

template<class T>
inline matrix4x4<T>::matrix4x4(const T* m)
{
 this->m[ 0] = m[ 0];
 this->m[ 1] = m[ 1];
 this->m[ 2] = m[ 2];
 this->m[ 3] = m[ 3];
 this->m[ 4] = m[ 4];
 this->m[ 5] = m[ 5];
 this->m[ 6] = m[ 6];
 this->m[ 7] = m[ 7];
 this->m[ 8] = m[ 8];
 this->m[ 9] = m[ 9];
 this->m[10] = m[10];
 this->m[11] = m[11];
 this->m[12] = m[12];
 this->m[13] = m[13];
 this->m[14] = m[14];
 this->m[15] = m[15];
}

template<class T>
inline matrix4x4<T>::matrix4x4(const matrix4x4<T>& other)
{
 this->m[ 0] = other.m[ 0];
 this->m[ 1] = other.m[ 1];
 this->m[ 2] = other.m[ 2];
 this->m[ 3] = other.m[ 3];
 this->m[ 4] = other.m[ 4];
 this->m[ 5] = other.m[ 5];
 this->m[ 6] = other.m[ 6];
 this->m[ 7] = other.m[ 7];
 this->m[ 8] = other.m[ 8];
 this->m[ 9] = other.m[ 9];
 this->m[10] = other.m[10];
 this->m[11] = other.m[11];
 this->m[12] = other.m[12];
 this->m[13] = other.m[13];
 this->m[14] = other.m[14];
 this->m[15] = other.m[15];
}

template<class T> template<class X>
inline matrix4x4<T>::matrix4x4(const matrix4x4<X>& other)
{
 this->m[ 0] = static_cast<T>(other.m[ 0]);
 this->m[ 1] = static_cast<T>(other.m[ 1]);
 this->m[ 2] = static_cast<T>(other.m[ 2]);
 this->m[ 3] = static_cast<T>(other.m[ 3]);
 this->m[ 4] = static_cast<T>(other.m[ 4]);
 this->m[ 5] = static_cast<T>(other.m[ 5]);
 this->m[ 6] = static_cast<T>(other.m[ 6]);
 this->m[ 7] = static_cast<T>(other.m[ 7]);
 this->m[ 8] = static_cast<T>(other.m[ 8]);
 this->m[ 9] = static_cast<T>(other.m[ 9]);
 this->m[10] = static_cast<T>(other.m[10]);
 this->m[11] = static_cast<T>(other.m[11]);
 this->m[12] = static_cast<T>(other.m[12]);
 this->m[13] = static_cast<T>(other.m[13]);
 this->m[14] = static_cast<T>(other.m[14]);
 this->m[15] = static_cast<T>(other.m[15]);
}

template<class T>
inline const T& matrix4x4<T>::at(int r, int c)const
{
 if(r < 0 || r > 3) throw std::out_of_range("matrix4x4 index out of range.");
 return this->m[(r << 2) + c];
}

template<class T>
inline T& matrix4x4<T>::at(int r, int c)
{
 if(r < 0 || r > 3) throw std::out_of_range("matrix4x4 index out of range.");
 return this->m[(r << 2) + c];
}

template<class T>
inline T matrix4x4<T>::determinant(void)const
{
 // cache 2x2 determinants
 T D[12] = {
  this->m[m33]*this->m[m44] - this->m[m34]*this->m[m43],
  this->m[m32]*this->m[m44] - this->m[m34]*this->m[m42],
  this->m[m32]*this->m[m43] - this->m[m33]*this->m[m42],
  this->m[m31]*this->m[m44] - this->m[m34]*this->m[m41],
  this->m[m31]*this->m[m43] - this->m[m33]*this->m[m41],
  this->m[m31]*this->m[m42] - this->m[m32]*this->m[m41],
  this->m[m13]*this->m[m24] - this->m[m14]*this->m[m23],
  this->m[m12]*this->m[m24] - this->m[m14]*this->m[m22],
  this->m[m12]*this->m[m23] - this->m[m13]*this->m[m22],
  this->m[m11]*this->m[m24] - this->m[m14]*this->m[m21],
  this->m[m11]*this->m[m23] - this->m[m13]*this->m[m21],
  this->m[m11]*this->m[m22] - this->m[m12]*this->m[m21] 
 };

 // compute determinant
 return D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
}

template<class T>
inline T matrix4x4<T>::invert(void)
{
 // cache 2x2 determinants
 T D[12] = {
  sarrus_2x2(this->m[m33], this->m[m34], this->m[m43], this->m[m44]),
  sarrus_2x2(this->m[m32], this->m[m34], this->m[m42], this->m[m44]),
  sarrus_2x2(this->m[m32], this->m[m33], this->m[m42], this->m[m43]),
  sarrus_2x2(this->m[m31], this->m[m34], this->m[m41], this->m[m44]),
  sarrus_2x2(this->m[m31], this->m[m33], this->m[m41], this->m[m43]),
  sarrus_2x2(this->m[m31], this->m[m32], this->m[m41], this->m[m42]),
  sarrus_2x2(this->m[m13], this->m[m14], this->m[m23], this->m[m24]),
  sarrus_2x2(this->m[m12], this->m[m14], this->m[m22], this->m[m24]),
  sarrus_2x2(this->m[m12], this->m[m13], this->m[m22], this->m[m23]),
  sarrus_2x2(this->m[m11], this->m[m14], this->m[m21], this->m[m24]),
  sarrus_2x2(this->m[m11], this->m[m13], this->m[m21], this->m[m23]),
  sarrus_2x2(this->m[m11], this->m[m12], this->m[m21], this->m[m22])
 };

 // compute determinant
 T det = D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
 if(is_zero(det)) return det;

 // compute cofactor matrix
 T C[16] = {
  +(this->m[m22]*D[0x0] - this->m[m23]*D[0x1] + this->m[m24]*D[0x2]),
  -(this->m[m21]*D[0x0] - this->m[m23]*D[0x3] + this->m[m24]*D[0x4]),
  +(this->m[m21]*D[0x1] - this->m[m22]*D[0x3] + this->m[m24]*D[0x5]),
  -(this->m[m21]*D[0x2] - this->m[m22]*D[0x4] + this->m[m23]*D[0x5]),
  -(this->m[m12]*D[0x0] - this->m[m13]*D[0x1] + this->m[m14]*D[0x2]),
  +(this->m[m11]*D[0x0] - this->m[m13]*D[0x3] + this->m[m14]*D[0x4]),
  -(this->m[m11]*D[0x1] - this->m[m12]*D[0x3] + this->m[m14]*D[0x5]),
  +(this->m[m11]*D[0x2] - this->m[m12]*D[0x4] + this->m[m13]*D[0x5]),
  +(this->m[m42]*D[0x6] - this->m[m43]*D[0x7] + this->m[m44]*D[0x8]),
  -(this->m[m41]*D[0x6] - this->m[m43]*D[0x9] + this->m[m44]*D[0xA]),
  +(this->m[m41]*D[0x7] - this->m[m42]*D[0x9] + this->m[m44]*D[0xB]),
  -(this->m[m41]*D[0x8] - this->m[m42]*D[0xA] + this->m[m43]*D[0xB]),
  -(this->m[m32]*D[0x6] - this->m[m33]*D[0x7] + this->m[m34]*D[0x8]),
  +(this->m[m31]*D[0x6] - this->m[m33]*D[0x9] + this->m[m34]*D[0xA]),
  -(this->m[m31]*D[0x7] - this->m[m32]*D[0x9] + this->m[m34]*D[0xB]),
  +(this->m[m31]*D[0x8] - this->m[m32]*D[0xA] + this->m[m33]*D[0xB]) 
 };

 // compute inverse matrix inv(M) = inv(det(M)) * transpose(C)
 T invdet = inv(det);
 this->m[m11] = C[m11]*invdet;
 this->m[m21] = C[m12]*invdet;
 this->m[m31] = C[m13]*invdet;
 this->m[m41] = C[m14]*invdet;
 this->m[m12] = C[m21]*invdet;
 this->m[m22] = C[m22]*invdet;
 this->m[m32] = C[m23]*invdet;
 this->m[m42] = C[m24]*invdet;
 this->m[m13] = C[m31]*invdet;
 this->m[m23] = C[m32]*invdet;
 this->m[m33] = C[m33]*invdet;
 this->m[m43] = C[m34]*invdet;
 this->m[m14] = C[m41]*invdet;
 this->m[m24] = C[m42]*invdet;
 this->m[m34] = C[m43]*invdet;
 this->m[m44] = C[m44]*invdet;

 // return determinant
 return det;
}

template<class T>
inline void matrix4x4<T>::transpose(void)
{
 T temp;
 temp = this->m[ 1]; this->m[ 1] = this->m[ 4]; this->m[ 4] = temp;
 temp = this->m[ 2]; this->m[ 2] = this->m[ 8]; this->m[ 8] = temp;
 temp = this->m[ 3]; this->m[ 3] = this->m[12]; this->m[12] = temp;
 temp = this->m[ 6]; this->m[ 6] = this->m[ 9]; this->m[ 9] = temp;
 temp = this->m[ 7]; this->m[ 7] = this->m[13]; this->m[13] = temp;
 temp = this->m[11]; this->m[11] = this->m[14]; this->m[14] = temp;
}

template<class T>
inline const T& matrix4x4<T>::operator [](size_t index)const
{
 return this->m[index];
}

template<class T>
inline T& matrix4x4<T>::operator [](size_t index)
{
 return this->m[index];
}

template<class T>
inline const T& matrix4x4<T>::operator ()(int r, int c)const
{
 return this->m[(r << 2) + c];
}

template<class T>
inline T& matrix4x4<T>::operator ()(int r, int c)
{
 return this->m[(r << 2) + c];
}

template<class T>
inline matrix4x4<T>& matrix4x4<T>::operator =(const matrix4x4<T>& other)
{
 if(this == &other) return *this;
 this->m[ 0] = other.m[ 0];
 this->m[ 1] = other.m[ 1];
 this->m[ 2] = other.m[ 2];
 this->m[ 3] = other.m[ 3];
 this->m[ 4] = other.m[ 4];
 this->m[ 5] = other.m[ 5];
 this->m[ 6] = other.m[ 6];
 this->m[ 7] = other.m[ 7];
 this->m[ 8] = other.m[ 8];
 this->m[ 9] = other.m[ 9];
 this->m[10] = other.m[10];
 this->m[11] = other.m[11];
 this->m[12] = other.m[12];
 this->m[13] = other.m[13];
 this->m[14] = other.m[14];
 this->m[15] = other.m[15];
 return *this;
}

template<class T> template<class X> 
inline matrix4x4<T>& matrix4x4<T>::operator =(const matrix4x4<X>& other)
{
 this->m[ 0] = static_cast<T>(other.m[ 0]);
 this->m[ 1] = static_cast<T>(other.m[ 1]);
 this->m[ 2] = static_cast<T>(other.m[ 2]);
 this->m[ 3] = static_cast<T>(other.m[ 3]);
 this->m[ 4] = static_cast<T>(other.m[ 4]);
 this->m[ 5] = static_cast<T>(other.m[ 5]);
 this->m[ 6] = static_cast<T>(other.m[ 6]);
 this->m[ 7] = static_cast<T>(other.m[ 7]);
 this->m[ 8] = static_cast<T>(other.m[ 8]);
 this->m[ 9] = static_cast<T>(other.m[ 9]);
 this->m[10] = static_cast<T>(other.m[10]);
 this->m[11] = static_cast<T>(other.m[11]);
 this->m[12] = static_cast<T>(other.m[12]);
 this->m[13] = static_cast<T>(other.m[13]);
 this->m[14] = static_cast<T>(other.m[14]);
 this->m[15] = static_cast<T>(other.m[15]);
 return *this;
}

template<class T>
inline matrix4x4<T>& matrix4x4<T>::operator *=(const T& scalar)
{
 this->m[ 0] *= scalar;
 this->m[ 1] *= scalar;
 this->m[ 2] *= scalar;
 this->m[ 3] *= scalar;
 this->m[ 4] *= scalar;
 this->m[ 5] *= scalar;
 this->m[ 6] *= scalar;
 this->m[ 7] *= scalar;
 this->m[ 8] *= scalar;
 this->m[ 9] *= scalar;
 this->m[10] *= scalar;
 this->m[11] *= scalar;
 this->m[12] *= scalar;
 this->m[13] *= scalar;
 this->m[14] *= scalar;
 this->m[15] *= scalar;
 return *this;
}

template<class T>
inline matrix4x4<T>& matrix4x4<T>::operator /=(const T& scalar)
{
 this->m[ 0] /= scalar;
 this->m[ 1] /= scalar;
 this->m[ 2] /= scalar;
 this->m[ 3] /= scalar;
 this->m[ 4] /= scalar;
 this->m[ 5] /= scalar;
 this->m[ 6] /= scalar;
 this->m[ 7] /= scalar;
 this->m[ 8] /= scalar;
 this->m[ 9] /= scalar;
 this->m[10] /= scalar;
 this->m[11] /= scalar;
 this->m[12] /= scalar;
 this->m[13] /= scalar;
 this->m[14] /= scalar;
 this->m[15] /= scalar;
 return *this;
}

template<class T>
inline matrix4x4<T>& matrix4x4<T>::operator +=(const matrix4x4<T>& other)
{
 this->m[ 0] += other.m[ 0];
 this->m[ 1] += other.m[ 1];
 this->m[ 2] += other.m[ 2];
 this->m[ 3] += other.m[ 3];
 this->m[ 4] += other.m[ 4];
 this->m[ 5] += other.m[ 5];
 this->m[ 6] += other.m[ 6];
 this->m[ 7] += other.m[ 7];
 this->m[ 8] += other.m[ 8];
 this->m[ 9] += other.m[ 9];
 this->m[10] += other.m[10];
 this->m[11] += other.m[11];
 this->m[12] += other.m[12];
 this->m[13] += other.m[13];
 this->m[14] += other.m[14];
 this->m[15] += other.m[15];
 return *this;
}

template<class T>
inline matrix4x4<T>& matrix4x4<T>::operator -=(const matrix4x4<T>& other)
{
 this->m[ 0] -= other.m[ 0];
 this->m[ 1] -= other.m[ 1];
 this->m[ 2] -= other.m[ 2];
 this->m[ 3] -= other.m[ 3];
 this->m[ 4] -= other.m[ 4];
 this->m[ 5] -= other.m[ 5];
 this->m[ 6] -= other.m[ 6];
 this->m[ 7] -= other.m[ 7];
 this->m[ 8] -= other.m[ 8];
 this->m[ 9] -= other.m[ 9];
 this->m[10] -= other.m[10];
 this->m[11] -= other.m[11];
 this->m[12] -= other.m[12];
 this->m[13] -= other.m[13];
 this->m[14] -= other.m[14];
 this->m[15] -= other.m[15];
 return *this;
}

template<class T>
inline matrix4x4<T>& matrix4x4<T>::operator *=(const matrix4x4<T>& other)
{
 T temp[16] = {
  this->m[ 0]*other.m[ 0] + this->m[ 1]*other.m[ 4] + this->m[ 2]*other.m[ 8] + this->m[ 3]*other.m[12],
  this->m[ 0]*other.m[ 1] + this->m[ 1]*other.m[ 5] + this->m[ 2]*other.m[ 9] + this->m[ 3]*other.m[13],
  this->m[ 0]*other.m[ 2] + this->m[ 1]*other.m[ 6] + this->m[ 2]*other.m[10] + this->m[ 3]*other.m[14],
  this->m[ 0]*other.m[ 3] + this->m[ 1]*other.m[ 7] + this->m[ 2]*other.m[11] + this->m[ 3]*other.m[15],
  this->m[ 4]*other.m[ 0] + this->m[ 5]*other.m[ 4] + this->m[ 6]*other.m[ 8] + this->m[ 7]*other.m[12],
  this->m[ 4]*other.m[ 1] + this->m[ 5]*other.m[ 5] + this->m[ 6]*other.m[ 9] + this->m[ 7]*other.m[13],
  this->m[ 4]*other.m[ 2] + this->m[ 5]*other.m[ 6] + this->m[ 6]*other.m[10] + this->m[ 7]*other.m[14],
  this->m[ 4]*other.m[ 3] + this->m[ 5]*other.m[ 7] + this->m[ 6]*other.m[11] + this->m[ 7]*other.m[15],
  this->m[ 8]*other.m[ 0] + this->m[ 9]*other.m[ 4] + this->m[10]*other.m[ 8] + this->m[11]*other.m[12],
  this->m[ 8]*other.m[ 1] + this->m[ 9]*other.m[ 5] + this->m[10]*other.m[ 9] + this->m[11]*other.m[13],
  this->m[ 8]*other.m[ 2] + this->m[ 9]*other.m[ 6] + this->m[10]*other.m[10] + this->m[11]*other.m[14],
  this->m[ 8]*other.m[ 3] + this->m[ 9]*other.m[ 7] + this->m[10]*other.m[11] + this->m[11]*other.m[15],
  this->m[12]*other.m[ 0] + this->m[13]*other.m[ 4] + this->m[14]*other.m[ 8] + this->m[15]*other.m[12],
  this->m[12]*other.m[ 1] + this->m[13]*other.m[ 5] + this->m[14]*other.m[ 9] + this->m[15]*other.m[13],
  this->m[12]*other.m[ 2] + this->m[13]*other.m[ 6] + this->m[14]*other.m[10] + this->m[15]*other.m[14],
  this->m[12]*other.m[ 3] + this->m[13]*other.m[ 7] + this->m[14]*other.m[11] + this->m[15]*other.m[15]
 };
 this->m[ 0] = temp[ 0];
 this->m[ 1] = temp[ 1];
 this->m[ 2] = temp[ 2];
 this->m[ 3] = temp[ 3];
 this->m[ 4] = temp[ 4];
 this->m[ 5] = temp[ 5];
 this->m[ 6] = temp[ 6];
 this->m[ 7] = temp[ 7];
 this->m[ 8] = temp[ 8];
 this->m[ 9] = temp[ 9];
 this->m[10] = temp[10];
 this->m[11] = temp[11];
 this->m[12] = temp[12];
 this->m[13] = temp[13];
 this->m[14] = temp[14];
 this->m[15] = temp[15];
 return *this;
}

template<class T> template<class X>
inline matrix4x4<T>& matrix4x4<T>::operator +=(const matrix4x4<X>& other)
{
 this->m[ 0] += static_cast<T>(other.m[ 0]);
 this->m[ 1] += static_cast<T>(other.m[ 1]);
 this->m[ 2] += static_cast<T>(other.m[ 2]);
 this->m[ 3] += static_cast<T>(other.m[ 3]);
 this->m[ 4] += static_cast<T>(other.m[ 4]);
 this->m[ 5] += static_cast<T>(other.m[ 5]);
 this->m[ 6] += static_cast<T>(other.m[ 6]);
 this->m[ 7] += static_cast<T>(other.m[ 7]);
 this->m[ 8] += static_cast<T>(other.m[ 8]);
 this->m[ 9] += static_cast<T>(other.m[ 9]);
 this->m[10] += static_cast<T>(other.m[10]);
 this->m[11] += static_cast<T>(other.m[11]);
 this->m[12] += static_cast<T>(other.m[12]);
 this->m[13] += static_cast<T>(other.m[13]);
 this->m[14] += static_cast<T>(other.m[14]);
 this->m[15] += static_cast<T>(other.m[15]);
 return *this;
}

template<class T> template<class X>
inline matrix4x4<T>& matrix4x4<T>::operator -=(const matrix4x4<X>& other)
{
 this->m[ 0] -= static_cast<T>(other.m[ 0]);
 this->m[ 1] -= static_cast<T>(other.m[ 1]);
 this->m[ 2] -= static_cast<T>(other.m[ 2]);
 this->m[ 3] -= static_cast<T>(other.m[ 3]);
 this->m[ 4] -= static_cast<T>(other.m[ 4]);
 this->m[ 5] -= static_cast<T>(other.m[ 5]);
 this->m[ 6] -= static_cast<T>(other.m[ 6]);
 this->m[ 7] -= static_cast<T>(other.m[ 7]);
 this->m[ 8] -= static_cast<T>(other.m[ 8]);
 this->m[ 9] -= static_cast<T>(other.m[ 9]);
 this->m[10] -= static_cast<T>(other.m[10]);
 this->m[11] -= static_cast<T>(other.m[11]);
 this->m[12] -= static_cast<T>(other.m[12]);
 this->m[13] -= static_cast<T>(other.m[13]);
 this->m[14] -= static_cast<T>(other.m[14]);
 this->m[15] -= static_cast<T>(other.m[15]);
 return *this;
}

template<class T> template<class X>
inline matrix4x4<T>& matrix4x4<T>::operator *=(const matrix4x4<X>& other)
{
 T cast[16] = {
  static_cast<T>(other.m[ 0]);
  static_cast<T>(other.m[ 1]);
  static_cast<T>(other.m[ 2]);
  static_cast<T>(other.m[ 3]);
  static_cast<T>(other.m[ 4]);
  static_cast<T>(other.m[ 5]);
  static_cast<T>(other.m[ 6]);
  static_cast<T>(other.m[ 7]);
  static_cast<T>(other.m[ 8]);
  static_cast<T>(other.m[ 9]);
  static_cast<T>(other.m[10]);
  static_cast<T>(other.m[11]);
  static_cast<T>(other.m[12]);
  static_cast<T>(other.m[13]);
  static_cast<T>(other.m[14]);
  static_cast<T>(other.m[15]);
 };
 T temp[16] = {
  this->m[ 0]*cast[ 0] + this->m[ 1]*cast[ 4] + this->m[ 2]*cast[ 8] + this->m[ 3]*cast[12],
  this->m[ 0]*cast[ 1] + this->m[ 1]*cast[ 5] + this->m[ 2]*cast[ 9] + this->m[ 3]*cast[13],
  this->m[ 0]*cast[ 2] + this->m[ 1]*cast[ 6] + this->m[ 2]*cast[10] + this->m[ 3]*cast[14],
  this->m[ 0]*cast[ 3] + this->m[ 1]*cast[ 7] + this->m[ 2]*cast[11] + this->m[ 3]*cast[15],
  this->m[ 4]*cast[ 0] + this->m[ 5]*cast[ 4] + this->m[ 6]*cast[ 8] + this->m[ 7]*cast[12],
  this->m[ 4]*cast[ 1] + this->m[ 5]*cast[ 5] + this->m[ 6]*cast[ 9] + this->m[ 7]*cast[13],
  this->m[ 4]*cast[ 2] + this->m[ 5]*cast[ 6] + this->m[ 6]*cast[10] + this->m[ 7]*cast[14],
  this->m[ 4]*cast[ 3] + this->m[ 5]*cast[ 7] + this->m[ 6]*cast[11] + this->m[ 7]*cast[15],
  this->m[ 8]*cast[ 0] + this->m[ 9]*cast[ 4] + this->m[10]*cast[ 8] + this->m[11]*cast[12],
  this->m[ 8]*cast[ 1] + this->m[ 9]*cast[ 5] + this->m[10]*cast[ 9] + this->m[11]*cast[13],
  this->m[ 8]*cast[ 2] + this->m[ 9]*cast[ 6] + this->m[10]*cast[10] + this->m[11]*cast[14],
  this->m[ 8]*cast[ 3] + this->m[ 9]*cast[ 7] + this->m[10]*cast[11] + this->m[11]*cast[15],
  this->m[12]*cast[ 0] + this->m[13]*cast[ 4] + this->m[14]*cast[ 8] + this->m[15]*cast[12],
  this->m[12]*cast[ 1] + this->m[13]*cast[ 5] + this->m[14]*cast[ 9] + this->m[15]*cast[13],
  this->m[12]*cast[ 2] + this->m[13]*cast[ 6] + this->m[14]*cast[10] + this->m[15]*cast[14],
  this->m[12]*cast[ 3] + this->m[13]*cast[ 7] + this->m[14]*cast[11] + this->m[15]*cast[15]
 };
 this->m[ 0] = temp[ 0];
 this->m[ 1] = temp[ 1];
 this->m[ 2] = temp[ 2];
 this->m[ 3] = temp[ 3];
 this->m[ 4] = temp[ 4];
 this->m[ 5] = temp[ 5];
 this->m[ 6] = temp[ 6];
 this->m[ 7] = temp[ 7];
 this->m[ 8] = temp[ 8];
 this->m[ 9] = temp[ 9];
 this->m[10] = temp[10];
 this->m[11] = temp[11];
 this->m[12] = temp[12];
 this->m[13] = temp[13];
 this->m[14] = temp[14];
 this->m[15] = temp[15];
 return *this;
}

//
// MATRIX SPECIALIZATION
//

template<>
class matrix4x4<binary32> : public c_smatrix4x4 {
 public :
  static const int m11 = 0x0;
  static const int m12 = 0x1;
  static const int m13 = 0x2;
  static const int m14 = 0x3;
  static const int m21 = 0x4;
  static const int m22 = 0x5;
  static const int m23 = 0x6;
  static const int m24 = 0x7;
  static const int m31 = 0x8;
  static const int m32 = 0x9;
  static const int m33 = 0xA;
  static const int m34 = 0xB;
  static const int m41 = 0xC;
  static const int m42 = 0xD;
  static const int m43 = 0xE;
  static const int m44 = 0xF;
 public :
  typedef binary32 value_type;
  typedef binary32 array_type[16];
 public :
  matrix4x4();
  explicit matrix4x4(binary32 fill);
  explicit matrix4x4(const binary32* m);
  matrix4x4(const matrix4x4<binary32>& other);
  explicit matrix4x4(const matrix4x4<binary64>& other);
 public :
  array_type& get(void) { return this->m; }
  const array_type& get(void)const { return this->m; }
 public :
  const binary32& at(int r, int c)const;
  binary32& at(int r, int c);
  binary32 determinant(void)const;
  binary32 invert(void);
  void transpose(void);
 public :
  const binary32& operator [](size_t index)const;
  binary32& operator [](size_t index);
 public :
  const binary32& operator ()(int r, int c)const;
  binary32& operator ()(int r, int c);
 public :
  matrix4x4<binary32>& operator =(const matrix4x4<binary32>& other);
  template<class X> matrix4x4<binary32>& operator =(const matrix4x4<X>& other);
 public :
  matrix4x4<binary32>& operator *=(binary32 scalar);
  matrix4x4<binary32>& operator /=(binary32 scalar);
 public :
  matrix4x4<binary32>& operator +=(const matrix4x4<binary32>& other);
  matrix4x4<binary32>& operator -=(const matrix4x4<binary32>& other);
  matrix4x4<binary32>& operator *=(const matrix4x4<binary32>& other);
  template<class X> matrix4x4<binary32>& operator +=(const matrix4x4<X>& other);
  template<class X> matrix4x4<binary32>& operator -=(const matrix4x4<X>& other);
  template<class X> matrix4x4<binary32>& operator *=(const matrix4x4<X>& other);
};

inline matrix4x4<binary32>::matrix4x4()
{
}

inline matrix4x4<binary32>::matrix4x4(binary32 fill)
{
 this->m[ 0] = this->m[ 1] = this->m[ 2] = this->m[ 3] = fill;
 this->m[ 4] = this->m[ 5] = this->m[ 6] = this->m[ 7] = fill;
 this->m[ 8] = this->m[ 9] = this->m[10] = this->m[11] = fill;
 this->m[12] = this->m[13] = this->m[14] = this->m[15] = fill;
}

inline matrix4x4<binary32>::matrix4x4(const binary32* m)
{
 this->m[ 0] = m[ 0];
 this->m[ 1] = m[ 1];
 this->m[ 2] = m[ 2];
 this->m[ 3] = m[ 3];
 this->m[ 4] = m[ 4];
 this->m[ 5] = m[ 5];
 this->m[ 6] = m[ 6];
 this->m[ 7] = m[ 7];
 this->m[ 8] = m[ 8];
 this->m[ 9] = m[ 9];
 this->m[10] = m[10];
 this->m[11] = m[11];
 this->m[12] = m[12];
 this->m[13] = m[13];
 this->m[14] = m[14];
 this->m[15] = m[15];
}

inline matrix4x4<binary32>::matrix4x4(const matrix4x4<binary32>& other)
{
 this->m[ 0] = other.m[ 0];
 this->m[ 1] = other.m[ 1];
 this->m[ 2] = other.m[ 2];
 this->m[ 3] = other.m[ 3];
 this->m[ 4] = other.m[ 4];
 this->m[ 5] = other.m[ 5];
 this->m[ 6] = other.m[ 6];
 this->m[ 7] = other.m[ 7];
 this->m[ 8] = other.m[ 8];
 this->m[ 9] = other.m[ 9];
 this->m[10] = other.m[10];
 this->m[11] = other.m[11];
 this->m[12] = other.m[12];
 this->m[13] = other.m[13];
 this->m[14] = other.m[14];
 this->m[15] = other.m[15];
}

inline const binary32& matrix4x4<binary32>::at(int r, int c)const
{
 if(r < 0 || r > 3) throw std::out_of_range("matrix4x4<binary32> index out of range.");
 return this->m[(r << 2) + c];
}

inline binary32& matrix4x4<binary32>::at(int r, int c)
{
 if(r < 0 || r > 3) throw std::out_of_range("matrix4x4<binary32> index out of range.");
 return this->m[(r << 2) + c];
}

inline binary32 matrix4x4<binary32>::determinant(void)const
{
 // cache 2x2 determinants
 binary32 D[12] = {
  this->m[m33]*this->m[m44] - this->m[m34]*this->m[m43],
  this->m[m32]*this->m[m44] - this->m[m34]*this->m[m42],
  this->m[m32]*this->m[m43] - this->m[m33]*this->m[m42],
  this->m[m31]*this->m[m44] - this->m[m34]*this->m[m41],
  this->m[m31]*this->m[m43] - this->m[m33]*this->m[m41],
  this->m[m31]*this->m[m42] - this->m[m32]*this->m[m41],
  this->m[m13]*this->m[m24] - this->m[m14]*this->m[m23],
  this->m[m12]*this->m[m24] - this->m[m14]*this->m[m22],
  this->m[m12]*this->m[m23] - this->m[m13]*this->m[m22],
  this->m[m11]*this->m[m24] - this->m[m14]*this->m[m21],
  this->m[m11]*this->m[m23] - this->m[m13]*this->m[m21],
  this->m[m11]*this->m[m22] - this->m[m12]*this->m[m21] 
 };

 // compute determinant
 return D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
}

inline binary32 matrix4x4<binary32>::invert(void)
{
 // cache 2x2 determinants
 binary32 D[12] = {
  sarrus_2x2(this->m[m33], this->m[m34], this->m[m43], this->m[m44]),
  sarrus_2x2(this->m[m32], this->m[m34], this->m[m42], this->m[m44]),
  sarrus_2x2(this->m[m32], this->m[m33], this->m[m42], this->m[m43]),
  sarrus_2x2(this->m[m31], this->m[m34], this->m[m41], this->m[m44]),
  sarrus_2x2(this->m[m31], this->m[m33], this->m[m41], this->m[m43]),
  sarrus_2x2(this->m[m31], this->m[m32], this->m[m41], this->m[m42]),
  sarrus_2x2(this->m[m13], this->m[m14], this->m[m23], this->m[m24]),
  sarrus_2x2(this->m[m12], this->m[m14], this->m[m22], this->m[m24]),
  sarrus_2x2(this->m[m12], this->m[m13], this->m[m22], this->m[m23]),
  sarrus_2x2(this->m[m11], this->m[m14], this->m[m21], this->m[m24]),
  sarrus_2x2(this->m[m11], this->m[m13], this->m[m21], this->m[m23]),
  sarrus_2x2(this->m[m11], this->m[m12], this->m[m21], this->m[m22])
 };

 // compute determinant
 binary32 det = D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
 if(equals_zero(det)) return det;

 // compute cofactor matrix
 binary32 C[16] = {
  +(this->m[m22]*D[0x0] - this->m[m23]*D[0x1] + this->m[m24]*D[0x2]),
  -(this->m[m21]*D[0x0] - this->m[m23]*D[0x3] + this->m[m24]*D[0x4]),
  +(this->m[m21]*D[0x1] - this->m[m22]*D[0x3] + this->m[m24]*D[0x5]),
  -(this->m[m21]*D[0x2] - this->m[m22]*D[0x4] + this->m[m23]*D[0x5]),
  -(this->m[m12]*D[0x0] - this->m[m13]*D[0x1] + this->m[m14]*D[0x2]),
  +(this->m[m11]*D[0x0] - this->m[m13]*D[0x3] + this->m[m14]*D[0x4]),
  -(this->m[m11]*D[0x1] - this->m[m12]*D[0x3] + this->m[m14]*D[0x5]),
  +(this->m[m11]*D[0x2] - this->m[m12]*D[0x4] + this->m[m13]*D[0x5]),
  +(this->m[m42]*D[0x6] - this->m[m43]*D[0x7] + this->m[m44]*D[0x8]),
  -(this->m[m41]*D[0x6] - this->m[m43]*D[0x9] + this->m[m44]*D[0xA]),
  +(this->m[m41]*D[0x7] - this->m[m42]*D[0x9] + this->m[m44]*D[0xB]),
  -(this->m[m41]*D[0x8] - this->m[m42]*D[0xA] + this->m[m43]*D[0xB]),
  -(this->m[m32]*D[0x6] - this->m[m33]*D[0x7] + this->m[m34]*D[0x8]),
  +(this->m[m31]*D[0x6] - this->m[m33]*D[0x9] + this->m[m34]*D[0xA]),
  -(this->m[m31]*D[0x7] - this->m[m32]*D[0x9] + this->m[m34]*D[0xB]),
  +(this->m[m31]*D[0x8] - this->m[m32]*D[0xA] + this->m[m33]*D[0xB]) 
 };

 // compute inverse matrix inv(M) = inv(det(M)) * transpose(C)
 binary32 invdet = inv(det);
 this->m[m11] = C[m11]*invdet;
 this->m[m21] = C[m12]*invdet;
 this->m[m31] = C[m13]*invdet;
 this->m[m41] = C[m14]*invdet;
 this->m[m12] = C[m21]*invdet;
 this->m[m22] = C[m22]*invdet;
 this->m[m32] = C[m23]*invdet;
 this->m[m42] = C[m24]*invdet;
 this->m[m13] = C[m31]*invdet;
 this->m[m23] = C[m32]*invdet;
 this->m[m33] = C[m33]*invdet;
 this->m[m43] = C[m34]*invdet;
 this->m[m14] = C[m41]*invdet;
 this->m[m24] = C[m42]*invdet;
 this->m[m34] = C[m43]*invdet;
 this->m[m44] = C[m44]*invdet;

 // return determinant
 return det;
}

inline void matrix4x4<binary32>::transpose(void)
{
 binary32 temp;
 temp = this->m[ 1]; this->m[ 1] = this->m[ 4]; this->m[ 4] = temp;
 temp = this->m[ 2]; this->m[ 2] = this->m[ 8]; this->m[ 8] = temp;
 temp = this->m[ 3]; this->m[ 3] = this->m[12]; this->m[12] = temp;
 temp = this->m[ 6]; this->m[ 6] = this->m[ 9]; this->m[ 9] = temp;
 temp = this->m[ 7]; this->m[ 7] = this->m[13]; this->m[13] = temp;
 temp = this->m[11]; this->m[11] = this->m[14]; this->m[14] = temp;
}

inline const binary32& matrix4x4<binary32>::operator [](size_t index)const
{
 return this->m[index];
}

inline binary32& matrix4x4<binary32>::operator [](size_t index)
{
 return this->m[index];
}

inline const binary32& matrix4x4<binary32>::operator ()(int r, int c)const
{
 return this->m[(r << 2) + c];
}

inline binary32& matrix4x4<binary32>::operator ()(int r, int c)
{
 return this->m[(r << 2) + c];
}

inline matrix4x4<binary32>& matrix4x4<binary32>::operator =(const matrix4x4<binary32>& other)
{
 if(this == &other) return *this;
 this->m[ 0] = other.m[ 0];
 this->m[ 1] = other.m[ 1];
 this->m[ 2] = other.m[ 2];
 this->m[ 3] = other.m[ 3];
 this->m[ 4] = other.m[ 4];
 this->m[ 5] = other.m[ 5];
 this->m[ 6] = other.m[ 6];
 this->m[ 7] = other.m[ 7];
 this->m[ 8] = other.m[ 8];
 this->m[ 9] = other.m[ 9];
 this->m[10] = other.m[10];
 this->m[11] = other.m[11];
 this->m[12] = other.m[12];
 this->m[13] = other.m[13];
 this->m[14] = other.m[14];
 this->m[15] = other.m[15];
 return *this;
}

template<class X> 
inline matrix4x4<binary32>& matrix4x4<binary32>::operator =(const matrix4x4<X>& other)
{
 this->m[ 0] = static_cast<binary32>(other.m[ 0]);
 this->m[ 1] = static_cast<binary32>(other.m[ 1]);
 this->m[ 2] = static_cast<binary32>(other.m[ 2]);
 this->m[ 3] = static_cast<binary32>(other.m[ 3]);
 this->m[ 4] = static_cast<binary32>(other.m[ 4]);
 this->m[ 5] = static_cast<binary32>(other.m[ 5]);
 this->m[ 6] = static_cast<binary32>(other.m[ 6]);
 this->m[ 7] = static_cast<binary32>(other.m[ 7]);
 this->m[ 8] = static_cast<binary32>(other.m[ 8]);
 this->m[ 9] = static_cast<binary32>(other.m[ 9]);
 this->m[10] = static_cast<binary32>(other.m[10]);
 this->m[11] = static_cast<binary32>(other.m[11]);
 this->m[12] = static_cast<binary32>(other.m[12]);
 this->m[13] = static_cast<binary32>(other.m[13]);
 this->m[14] = static_cast<binary32>(other.m[14]);
 this->m[15] = static_cast<binary32>(other.m[15]);
 return *this;
}

inline matrix4x4<binary32>& matrix4x4<binary32>::operator *=(binary32 scalar)
{
 this->m[ 0] *= scalar;
 this->m[ 1] *= scalar;
 this->m[ 2] *= scalar;
 this->m[ 3] *= scalar;
 this->m[ 4] *= scalar;
 this->m[ 5] *= scalar;
 this->m[ 6] *= scalar;
 this->m[ 7] *= scalar;
 this->m[ 8] *= scalar;
 this->m[ 9] *= scalar;
 this->m[10] *= scalar;
 this->m[11] *= scalar;
 this->m[12] *= scalar;
 this->m[13] *= scalar;
 this->m[14] *= scalar;
 this->m[15] *= scalar;
 return *this;
}

inline matrix4x4<binary32>& matrix4x4<binary32>::operator /=(binary32 scalar)
{
 this->m[ 0] /= scalar;
 this->m[ 1] /= scalar;
 this->m[ 2] /= scalar;
 this->m[ 3] /= scalar;
 this->m[ 4] /= scalar;
 this->m[ 5] /= scalar;
 this->m[ 6] /= scalar;
 this->m[ 7] /= scalar;
 this->m[ 8] /= scalar;
 this->m[ 9] /= scalar;
 this->m[10] /= scalar;
 this->m[11] /= scalar;
 this->m[12] /= scalar;
 this->m[13] /= scalar;
 this->m[14] /= scalar;
 this->m[15] /= scalar;
 return *this;
}

inline matrix4x4<binary32>& matrix4x4<binary32>::operator +=(const matrix4x4<binary32>& other)
{
 this->m[ 0] += other.m[ 0];
 this->m[ 1] += other.m[ 1];
 this->m[ 2] += other.m[ 2];
 this->m[ 3] += other.m[ 3];
 this->m[ 4] += other.m[ 4];
 this->m[ 5] += other.m[ 5];
 this->m[ 6] += other.m[ 6];
 this->m[ 7] += other.m[ 7];
 this->m[ 8] += other.m[ 8];
 this->m[ 9] += other.m[ 9];
 this->m[10] += other.m[10];
 this->m[11] += other.m[11];
 this->m[12] += other.m[12];
 this->m[13] += other.m[13];
 this->m[14] += other.m[14];
 this->m[15] += other.m[15];
 return *this;
}

inline matrix4x4<binary32>& matrix4x4<binary32>::operator -=(const matrix4x4<binary32>& other)
{
 this->m[ 0] -= other.m[ 0];
 this->m[ 1] -= other.m[ 1];
 this->m[ 2] -= other.m[ 2];
 this->m[ 3] -= other.m[ 3];
 this->m[ 4] -= other.m[ 4];
 this->m[ 5] -= other.m[ 5];
 this->m[ 6] -= other.m[ 6];
 this->m[ 7] -= other.m[ 7];
 this->m[ 8] -= other.m[ 8];
 this->m[ 9] -= other.m[ 9];
 this->m[10] -= other.m[10];
 this->m[11] -= other.m[11];
 this->m[12] -= other.m[12];
 this->m[13] -= other.m[13];
 this->m[14] -= other.m[14];
 this->m[15] -= other.m[15];
 return *this;
}

inline matrix4x4<binary32>& matrix4x4<binary32>::operator *=(const matrix4x4<binary32>& other)
{
 binary32 temp[16] = {
  this->m[ 0]*other.m[ 0] + this->m[ 1]*other.m[ 4] + this->m[ 2]*other.m[ 8] + this->m[ 3]*other.m[12],
  this->m[ 0]*other.m[ 1] + this->m[ 1]*other.m[ 5] + this->m[ 2]*other.m[ 9] + this->m[ 3]*other.m[13],
  this->m[ 0]*other.m[ 2] + this->m[ 1]*other.m[ 6] + this->m[ 2]*other.m[10] + this->m[ 3]*other.m[14],
  this->m[ 0]*other.m[ 3] + this->m[ 1]*other.m[ 7] + this->m[ 2]*other.m[11] + this->m[ 3]*other.m[15],
  this->m[ 4]*other.m[ 0] + this->m[ 5]*other.m[ 4] + this->m[ 6]*other.m[ 8] + this->m[ 7]*other.m[12],
  this->m[ 4]*other.m[ 1] + this->m[ 5]*other.m[ 5] + this->m[ 6]*other.m[ 9] + this->m[ 7]*other.m[13],
  this->m[ 4]*other.m[ 2] + this->m[ 5]*other.m[ 6] + this->m[ 6]*other.m[10] + this->m[ 7]*other.m[14],
  this->m[ 4]*other.m[ 3] + this->m[ 5]*other.m[ 7] + this->m[ 6]*other.m[11] + this->m[ 7]*other.m[15],
  this->m[ 8]*other.m[ 0] + this->m[ 9]*other.m[ 4] + this->m[10]*other.m[ 8] + this->m[11]*other.m[12],
  this->m[ 8]*other.m[ 1] + this->m[ 9]*other.m[ 5] + this->m[10]*other.m[ 9] + this->m[11]*other.m[13],
  this->m[ 8]*other.m[ 2] + this->m[ 9]*other.m[ 6] + this->m[10]*other.m[10] + this->m[11]*other.m[14],
  this->m[ 8]*other.m[ 3] + this->m[ 9]*other.m[ 7] + this->m[10]*other.m[11] + this->m[11]*other.m[15],
  this->m[12]*other.m[ 0] + this->m[13]*other.m[ 4] + this->m[14]*other.m[ 8] + this->m[15]*other.m[12],
  this->m[12]*other.m[ 1] + this->m[13]*other.m[ 5] + this->m[14]*other.m[ 9] + this->m[15]*other.m[13],
  this->m[12]*other.m[ 2] + this->m[13]*other.m[ 6] + this->m[14]*other.m[10] + this->m[15]*other.m[14],
  this->m[12]*other.m[ 3] + this->m[13]*other.m[ 7] + this->m[14]*other.m[11] + this->m[15]*other.m[15]
 };
 this->m[ 0] = temp[ 0];
 this->m[ 1] = temp[ 1];
 this->m[ 2] = temp[ 2];
 this->m[ 3] = temp[ 3];
 this->m[ 4] = temp[ 4];
 this->m[ 5] = temp[ 5];
 this->m[ 6] = temp[ 6];
 this->m[ 7] = temp[ 7];
 this->m[ 8] = temp[ 8];
 this->m[ 9] = temp[ 9];
 this->m[10] = temp[10];
 this->m[11] = temp[11];
 this->m[12] = temp[12];
 this->m[13] = temp[13];
 this->m[14] = temp[14];
 this->m[15] = temp[15];
 return *this;
}

template<class X>
inline matrix4x4<binary32>& matrix4x4<binary32>::operator +=(const matrix4x4<X>& other)
{
 this->m[ 0] += static_cast<binary32>(other.m[ 0]);
 this->m[ 1] += static_cast<binary32>(other.m[ 1]);
 this->m[ 2] += static_cast<binary32>(other.m[ 2]);
 this->m[ 3] += static_cast<binary32>(other.m[ 3]);
 this->m[ 4] += static_cast<binary32>(other.m[ 4]);
 this->m[ 5] += static_cast<binary32>(other.m[ 5]);
 this->m[ 6] += static_cast<binary32>(other.m[ 6]);
 this->m[ 7] += static_cast<binary32>(other.m[ 7]);
 this->m[ 8] += static_cast<binary32>(other.m[ 8]);
 this->m[ 9] += static_cast<binary32>(other.m[ 9]);
 this->m[10] += static_cast<binary32>(other.m[10]);
 this->m[11] += static_cast<binary32>(other.m[11]);
 this->m[12] += static_cast<binary32>(other.m[12]);
 this->m[13] += static_cast<binary32>(other.m[13]);
 this->m[14] += static_cast<binary32>(other.m[14]);
 this->m[15] += static_cast<binary32>(other.m[15]);
 return *this;
}

template<class X>
inline matrix4x4<binary32>& matrix4x4<binary32>::operator -=(const matrix4x4<X>& other)
{
 this->m[ 0] -= static_cast<binary32>(other.m[ 0]);
 this->m[ 1] -= static_cast<binary32>(other.m[ 1]);
 this->m[ 2] -= static_cast<binary32>(other.m[ 2]);
 this->m[ 3] -= static_cast<binary32>(other.m[ 3]);
 this->m[ 4] -= static_cast<binary32>(other.m[ 4]);
 this->m[ 5] -= static_cast<binary32>(other.m[ 5]);
 this->m[ 6] -= static_cast<binary32>(other.m[ 6]);
 this->m[ 7] -= static_cast<binary32>(other.m[ 7]);
 this->m[ 8] -= static_cast<binary32>(other.m[ 8]);
 this->m[ 9] -= static_cast<binary32>(other.m[ 9]);
 this->m[10] -= static_cast<binary32>(other.m[10]);
 this->m[11] -= static_cast<binary32>(other.m[11]);
 this->m[12] -= static_cast<binary32>(other.m[12]);
 this->m[13] -= static_cast<binary32>(other.m[13]);
 this->m[14] -= static_cast<binary32>(other.m[14]);
 this->m[15] -= static_cast<binary32>(other.m[15]);
 return *this;
}

template<class X>
inline matrix4x4<binary32>& matrix4x4<binary32>::operator *=(const matrix4x4<X>& other)
{
 binary32 cast[16] = {
  static_cast<binary32>(other.m[ 0]);
  static_cast<binary32>(other.m[ 1]);
  static_cast<binary32>(other.m[ 2]);
  static_cast<binary32>(other.m[ 3]);
  static_cast<binary32>(other.m[ 4]);
  static_cast<binary32>(other.m[ 5]);
  static_cast<binary32>(other.m[ 6]);
  static_cast<binary32>(other.m[ 7]);
  static_cast<binary32>(other.m[ 8]);
  static_cast<binary32>(other.m[ 9]);
  static_cast<binary32>(other.m[10]);
  static_cast<binary32>(other.m[11]);
  static_cast<binary32>(other.m[12]);
  static_cast<binary32>(other.m[13]);
  static_cast<binary32>(other.m[14]);
  static_cast<binary32>(other.m[15]);
 };
 binary32 temp[16] = {
  this->m[ 0]*cast[ 0] + this->m[ 1]*cast[ 4] + this->m[ 2]*cast[ 8] + this->m[ 3]*cast[12],
  this->m[ 0]*cast[ 1] + this->m[ 1]*cast[ 5] + this->m[ 2]*cast[ 9] + this->m[ 3]*cast[13],
  this->m[ 0]*cast[ 2] + this->m[ 1]*cast[ 6] + this->m[ 2]*cast[10] + this->m[ 3]*cast[14],
  this->m[ 0]*cast[ 3] + this->m[ 1]*cast[ 7] + this->m[ 2]*cast[11] + this->m[ 3]*cast[15],
  this->m[ 4]*cast[ 0] + this->m[ 5]*cast[ 4] + this->m[ 6]*cast[ 8] + this->m[ 7]*cast[12],
  this->m[ 4]*cast[ 1] + this->m[ 5]*cast[ 5] + this->m[ 6]*cast[ 9] + this->m[ 7]*cast[13],
  this->m[ 4]*cast[ 2] + this->m[ 5]*cast[ 6] + this->m[ 6]*cast[10] + this->m[ 7]*cast[14],
  this->m[ 4]*cast[ 3] + this->m[ 5]*cast[ 7] + this->m[ 6]*cast[11] + this->m[ 7]*cast[15],
  this->m[ 8]*cast[ 0] + this->m[ 9]*cast[ 4] + this->m[10]*cast[ 8] + this->m[11]*cast[12],
  this->m[ 8]*cast[ 1] + this->m[ 9]*cast[ 5] + this->m[10]*cast[ 9] + this->m[11]*cast[13],
  this->m[ 8]*cast[ 2] + this->m[ 9]*cast[ 6] + this->m[10]*cast[10] + this->m[11]*cast[14],
  this->m[ 8]*cast[ 3] + this->m[ 9]*cast[ 7] + this->m[10]*cast[11] + this->m[11]*cast[15],
  this->m[12]*cast[ 0] + this->m[13]*cast[ 4] + this->m[14]*cast[ 8] + this->m[15]*cast[12],
  this->m[12]*cast[ 1] + this->m[13]*cast[ 5] + this->m[14]*cast[ 9] + this->m[15]*cast[13],
  this->m[12]*cast[ 2] + this->m[13]*cast[ 6] + this->m[14]*cast[10] + this->m[15]*cast[14],
  this->m[12]*cast[ 3] + this->m[13]*cast[ 7] + this->m[14]*cast[11] + this->m[15]*cast[15]
 };
 this->m[ 0] = temp[ 0];
 this->m[ 1] = temp[ 1];
 this->m[ 2] = temp[ 2];
 this->m[ 3] = temp[ 3];
 this->m[ 4] = temp[ 4];
 this->m[ 5] = temp[ 5];
 this->m[ 6] = temp[ 6];
 this->m[ 7] = temp[ 7];
 this->m[ 8] = temp[ 8];
 this->m[ 9] = temp[ 9];
 this->m[10] = temp[10];
 this->m[11] = temp[11];
 this->m[12] = temp[12];
 this->m[13] = temp[13];
 this->m[14] = temp[14];
 this->m[15] = temp[15];
 return *this;
}

//
// MATRIX SPECIALIZATION
//

template<>
class matrix4x4<binary64> : public c_dmatrix4x4 {
 public :
  static const int m11 = 0x0;
  static const int m12 = 0x1;
  static const int m13 = 0x2;
  static const int m14 = 0x3;
  static const int m21 = 0x4;
  static const int m22 = 0x5;
  static const int m23 = 0x6;
  static const int m24 = 0x7;
  static const int m31 = 0x8;
  static const int m32 = 0x9;
  static const int m33 = 0xA;
  static const int m34 = 0xB;
  static const int m41 = 0xC;
  static const int m42 = 0xD;
  static const int m43 = 0xE;
  static const int m44 = 0xF;
 public :
  typedef binary64 value_type;
  typedef binary64 array_type[16];
 public :
  matrix4x4();
  explicit matrix4x4(binary64 fill);
  explicit matrix4x4(const binary64* m);
  matrix4x4(const matrix4x4<binary64>& other);
  explicit matrix4x4(const matrix4x4<binary32>& other);
 public :
  array_type& get(void) { return this->m; }
  const array_type& get(void)const { return this->m; }
 public :
  const binary64& at(int r, int c)const;
  binary64& at(int r, int c);
  binary64 determinant(void)const;
  binary64 invert(void);
  void transpose(void);
 public :
  const binary64& operator [](size_t index)const;
  binary64& operator [](size_t index);
 public :
  const binary64& operator ()(int r, int c)const;
  binary64& operator ()(int r, int c);
 public :
  matrix4x4<binary64>& operator =(const matrix4x4<binary64>& other);
  template<class X> matrix4x4<binary64>& operator =(const matrix4x4<X>& other);
 public :
  matrix4x4<binary64>& operator *=(binary64 scalar);
  matrix4x4<binary64>& operator /=(binary64 scalar);
 public :
  matrix4x4<binary64>& operator +=(const matrix4x4<binary64>& other);
  matrix4x4<binary64>& operator -=(const matrix4x4<binary64>& other);
  matrix4x4<binary64>& operator *=(const matrix4x4<binary64>& other);
  template<class X> matrix4x4<binary64>& operator +=(const matrix4x4<X>& other);
  template<class X> matrix4x4<binary64>& operator -=(const matrix4x4<X>& other);
  template<class X> matrix4x4<binary64>& operator *=(const matrix4x4<X>& other);
};

inline matrix4x4<binary64>::matrix4x4()
{
}

inline matrix4x4<binary64>::matrix4x4(binary64 fill)
{
 this->m[ 0] = this->m[ 1] = this->m[ 2] = this->m[ 3] = fill;
 this->m[ 4] = this->m[ 5] = this->m[ 6] = this->m[ 7] = fill;
 this->m[ 8] = this->m[ 9] = this->m[10] = this->m[11] = fill;
 this->m[12] = this->m[13] = this->m[14] = this->m[15] = fill;
}

inline matrix4x4<binary64>::matrix4x4(const binary64* m)
{
 this->m[ 0] = m[ 0];
 this->m[ 1] = m[ 1];
 this->m[ 2] = m[ 2];
 this->m[ 3] = m[ 3];
 this->m[ 4] = m[ 4];
 this->m[ 5] = m[ 5];
 this->m[ 6] = m[ 6];
 this->m[ 7] = m[ 7];
 this->m[ 8] = m[ 8];
 this->m[ 9] = m[ 9];
 this->m[10] = m[10];
 this->m[11] = m[11];
 this->m[12] = m[12];
 this->m[13] = m[13];
 this->m[14] = m[14];
 this->m[15] = m[15];
}

inline matrix4x4<binary64>::matrix4x4(const matrix4x4<binary64>& other)
{
 this->m[ 0] = other.m[ 0];
 this->m[ 1] = other.m[ 1];
 this->m[ 2] = other.m[ 2];
 this->m[ 3] = other.m[ 3];
 this->m[ 4] = other.m[ 4];
 this->m[ 5] = other.m[ 5];
 this->m[ 6] = other.m[ 6];
 this->m[ 7] = other.m[ 7];
 this->m[ 8] = other.m[ 8];
 this->m[ 9] = other.m[ 9];
 this->m[10] = other.m[10];
 this->m[11] = other.m[11];
 this->m[12] = other.m[12];
 this->m[13] = other.m[13];
 this->m[14] = other.m[14];
 this->m[15] = other.m[15];
}

inline const binary64& matrix4x4<binary64>::at(int r, int c)const
{
 if(r < 0 || r > 3) throw std::out_of_range("matrix4x4<binary64> index out of range.");
 return this->m[(r << 2) + c];
}

inline binary64& matrix4x4<binary64>::at(int r, int c)
{
 if(r < 0 || r > 3) throw std::out_of_range("matrix4x4<binary64> index out of range.");
 return this->m[(r << 2) + c];
}

inline binary64 matrix4x4<binary64>::determinant(void)const
{
 // cache 2x2 determinants
 binary64 D[12] = {
  this->m[m33]*this->m[m44] - this->m[m34]*this->m[m43],
  this->m[m32]*this->m[m44] - this->m[m34]*this->m[m42],
  this->m[m32]*this->m[m43] - this->m[m33]*this->m[m42],
  this->m[m31]*this->m[m44] - this->m[m34]*this->m[m41],
  this->m[m31]*this->m[m43] - this->m[m33]*this->m[m41],
  this->m[m31]*this->m[m42] - this->m[m32]*this->m[m41],
  this->m[m13]*this->m[m24] - this->m[m14]*this->m[m23],
  this->m[m12]*this->m[m24] - this->m[m14]*this->m[m22],
  this->m[m12]*this->m[m23] - this->m[m13]*this->m[m22],
  this->m[m11]*this->m[m24] - this->m[m14]*this->m[m21],
  this->m[m11]*this->m[m23] - this->m[m13]*this->m[m21],
  this->m[m11]*this->m[m22] - this->m[m12]*this->m[m21] 
 };

 // compute determinant
 return D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
}

inline binary64 matrix4x4<binary64>::invert(void)
{
 // cache 2x2 determinants
 binary64 D[12] = {
  sarrus_2x2(this->m[m33], this->m[m34], this->m[m43], this->m[m44]),
  sarrus_2x2(this->m[m32], this->m[m34], this->m[m42], this->m[m44]),
  sarrus_2x2(this->m[m32], this->m[m33], this->m[m42], this->m[m43]),
  sarrus_2x2(this->m[m31], this->m[m34], this->m[m41], this->m[m44]),
  sarrus_2x2(this->m[m31], this->m[m33], this->m[m41], this->m[m43]),
  sarrus_2x2(this->m[m31], this->m[m32], this->m[m41], this->m[m42]),
  sarrus_2x2(this->m[m13], this->m[m14], this->m[m23], this->m[m24]),
  sarrus_2x2(this->m[m12], this->m[m14], this->m[m22], this->m[m24]),
  sarrus_2x2(this->m[m12], this->m[m13], this->m[m22], this->m[m23]),
  sarrus_2x2(this->m[m11], this->m[m14], this->m[m21], this->m[m24]),
  sarrus_2x2(this->m[m11], this->m[m13], this->m[m21], this->m[m23]),
  sarrus_2x2(this->m[m11], this->m[m12], this->m[m21], this->m[m22])
 };

 // compute determinant
 binary64 det = D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
 if(equals_zero(det)) return det;

 // compute cofactor matrix
 binary64 C[16] = {
  +(this->m[m22]*D[0x0] - this->m[m23]*D[0x1] + this->m[m24]*D[0x2]),
  -(this->m[m21]*D[0x0] - this->m[m23]*D[0x3] + this->m[m24]*D[0x4]),
  +(this->m[m21]*D[0x1] - this->m[m22]*D[0x3] + this->m[m24]*D[0x5]),
  -(this->m[m21]*D[0x2] - this->m[m22]*D[0x4] + this->m[m23]*D[0x5]),
  -(this->m[m12]*D[0x0] - this->m[m13]*D[0x1] + this->m[m14]*D[0x2]),
  +(this->m[m11]*D[0x0] - this->m[m13]*D[0x3] + this->m[m14]*D[0x4]),
  -(this->m[m11]*D[0x1] - this->m[m12]*D[0x3] + this->m[m14]*D[0x5]),
  +(this->m[m11]*D[0x2] - this->m[m12]*D[0x4] + this->m[m13]*D[0x5]),
  +(this->m[m42]*D[0x6] - this->m[m43]*D[0x7] + this->m[m44]*D[0x8]),
  -(this->m[m41]*D[0x6] - this->m[m43]*D[0x9] + this->m[m44]*D[0xA]),
  +(this->m[m41]*D[0x7] - this->m[m42]*D[0x9] + this->m[m44]*D[0xB]),
  -(this->m[m41]*D[0x8] - this->m[m42]*D[0xA] + this->m[m43]*D[0xB]),
  -(this->m[m32]*D[0x6] - this->m[m33]*D[0x7] + this->m[m34]*D[0x8]),
  +(this->m[m31]*D[0x6] - this->m[m33]*D[0x9] + this->m[m34]*D[0xA]),
  -(this->m[m31]*D[0x7] - this->m[m32]*D[0x9] + this->m[m34]*D[0xB]),
  +(this->m[m31]*D[0x8] - this->m[m32]*D[0xA] + this->m[m33]*D[0xB]) 
 };

 // compute inverse matrix inv(M) = inv(det(M)) * transpose(C)
 binary64 invdet = inv(det);
 this->m[m11] = C[m11]*invdet;
 this->m[m21] = C[m12]*invdet;
 this->m[m31] = C[m13]*invdet;
 this->m[m41] = C[m14]*invdet;
 this->m[m12] = C[m21]*invdet;
 this->m[m22] = C[m22]*invdet;
 this->m[m32] = C[m23]*invdet;
 this->m[m42] = C[m24]*invdet;
 this->m[m13] = C[m31]*invdet;
 this->m[m23] = C[m32]*invdet;
 this->m[m33] = C[m33]*invdet;
 this->m[m43] = C[m34]*invdet;
 this->m[m14] = C[m41]*invdet;
 this->m[m24] = C[m42]*invdet;
 this->m[m34] = C[m43]*invdet;
 this->m[m44] = C[m44]*invdet;

 // return determinant
 return det;
}

inline void matrix4x4<binary64>::transpose(void)
{
 binary64 temp;
 temp = this->m[ 1]; this->m[ 1] = this->m[ 4]; this->m[ 4] = temp;
 temp = this->m[ 2]; this->m[ 2] = this->m[ 8]; this->m[ 8] = temp;
 temp = this->m[ 3]; this->m[ 3] = this->m[12]; this->m[12] = temp;
 temp = this->m[ 6]; this->m[ 6] = this->m[ 9]; this->m[ 9] = temp;
 temp = this->m[ 7]; this->m[ 7] = this->m[13]; this->m[13] = temp;
 temp = this->m[11]; this->m[11] = this->m[14]; this->m[14] = temp;
}

inline const binary64& matrix4x4<binary64>::operator [](size_t index)const
{
 return this->m[index];
}

inline binary64& matrix4x4<binary64>::operator [](size_t index)
{
 return this->m[index];
}

inline const binary64& matrix4x4<binary64>::operator ()(int r, int c)const
{
 return this->m[(r << 2) + c];
}

inline binary64& matrix4x4<binary64>::operator ()(int r, int c)
{
 return this->m[(r << 2) + c];
}

inline matrix4x4<binary64>& matrix4x4<binary64>::operator =(const matrix4x4<binary64>& other)
{
 if(this == &other) return *this;
 this->m[ 0] = other.m[ 0];
 this->m[ 1] = other.m[ 1];
 this->m[ 2] = other.m[ 2];
 this->m[ 3] = other.m[ 3];
 this->m[ 4] = other.m[ 4];
 this->m[ 5] = other.m[ 5];
 this->m[ 6] = other.m[ 6];
 this->m[ 7] = other.m[ 7];
 this->m[ 8] = other.m[ 8];
 this->m[ 9] = other.m[ 9];
 this->m[10] = other.m[10];
 this->m[11] = other.m[11];
 this->m[12] = other.m[12];
 this->m[13] = other.m[13];
 this->m[14] = other.m[14];
 this->m[15] = other.m[15];
 return *this;
}

template<class X> 
inline matrix4x4<binary64>& matrix4x4<binary64>::operator =(const matrix4x4<X>& other)
{
 this->m[ 0] = static_cast<binary64>(other.m[ 0]);
 this->m[ 1] = static_cast<binary64>(other.m[ 1]);
 this->m[ 2] = static_cast<binary64>(other.m[ 2]);
 this->m[ 3] = static_cast<binary64>(other.m[ 3]);
 this->m[ 4] = static_cast<binary64>(other.m[ 4]);
 this->m[ 5] = static_cast<binary64>(other.m[ 5]);
 this->m[ 6] = static_cast<binary64>(other.m[ 6]);
 this->m[ 7] = static_cast<binary64>(other.m[ 7]);
 this->m[ 8] = static_cast<binary64>(other.m[ 8]);
 this->m[ 9] = static_cast<binary64>(other.m[ 9]);
 this->m[10] = static_cast<binary64>(other.m[10]);
 this->m[11] = static_cast<binary64>(other.m[11]);
 this->m[12] = static_cast<binary64>(other.m[12]);
 this->m[13] = static_cast<binary64>(other.m[13]);
 this->m[14] = static_cast<binary64>(other.m[14]);
 this->m[15] = static_cast<binary64>(other.m[15]);
 return *this;
}

inline matrix4x4<binary64>& matrix4x4<binary64>::operator *=(binary64 scalar)
{
 this->m[ 0] *= scalar;
 this->m[ 1] *= scalar;
 this->m[ 2] *= scalar;
 this->m[ 3] *= scalar;
 this->m[ 4] *= scalar;
 this->m[ 5] *= scalar;
 this->m[ 6] *= scalar;
 this->m[ 7] *= scalar;
 this->m[ 8] *= scalar;
 this->m[ 9] *= scalar;
 this->m[10] *= scalar;
 this->m[11] *= scalar;
 this->m[12] *= scalar;
 this->m[13] *= scalar;
 this->m[14] *= scalar;
 this->m[15] *= scalar;
 return *this;
}

inline matrix4x4<binary64>& matrix4x4<binary64>::operator /=(binary64 scalar)
{
 this->m[ 0] /= scalar;
 this->m[ 1] /= scalar;
 this->m[ 2] /= scalar;
 this->m[ 3] /= scalar;
 this->m[ 4] /= scalar;
 this->m[ 5] /= scalar;
 this->m[ 6] /= scalar;
 this->m[ 7] /= scalar;
 this->m[ 8] /= scalar;
 this->m[ 9] /= scalar;
 this->m[10] /= scalar;
 this->m[11] /= scalar;
 this->m[12] /= scalar;
 this->m[13] /= scalar;
 this->m[14] /= scalar;
 this->m[15] /= scalar;
 return *this;
}

inline matrix4x4<binary64>& matrix4x4<binary64>::operator +=(const matrix4x4<binary64>& other)
{
 this->m[ 0] += other.m[ 0];
 this->m[ 1] += other.m[ 1];
 this->m[ 2] += other.m[ 2];
 this->m[ 3] += other.m[ 3];
 this->m[ 4] += other.m[ 4];
 this->m[ 5] += other.m[ 5];
 this->m[ 6] += other.m[ 6];
 this->m[ 7] += other.m[ 7];
 this->m[ 8] += other.m[ 8];
 this->m[ 9] += other.m[ 9];
 this->m[10] += other.m[10];
 this->m[11] += other.m[11];
 this->m[12] += other.m[12];
 this->m[13] += other.m[13];
 this->m[14] += other.m[14];
 this->m[15] += other.m[15];
 return *this;
}

inline matrix4x4<binary64>& matrix4x4<binary64>::operator -=(const matrix4x4<binary64>& other)
{
 this->m[ 0] -= other.m[ 0];
 this->m[ 1] -= other.m[ 1];
 this->m[ 2] -= other.m[ 2];
 this->m[ 3] -= other.m[ 3];
 this->m[ 4] -= other.m[ 4];
 this->m[ 5] -= other.m[ 5];
 this->m[ 6] -= other.m[ 6];
 this->m[ 7] -= other.m[ 7];
 this->m[ 8] -= other.m[ 8];
 this->m[ 9] -= other.m[ 9];
 this->m[10] -= other.m[10];
 this->m[11] -= other.m[11];
 this->m[12] -= other.m[12];
 this->m[13] -= other.m[13];
 this->m[14] -= other.m[14];
 this->m[15] -= other.m[15];
 return *this;
}

inline matrix4x4<binary64>& matrix4x4<binary64>::operator *=(const matrix4x4<binary64>& other)
{
 binary64 temp[16] = {
  this->m[ 0]*other.m[ 0] + this->m[ 1]*other.m[ 4] + this->m[ 2]*other.m[ 8] + this->m[ 3]*other.m[12],
  this->m[ 0]*other.m[ 1] + this->m[ 1]*other.m[ 5] + this->m[ 2]*other.m[ 9] + this->m[ 3]*other.m[13],
  this->m[ 0]*other.m[ 2] + this->m[ 1]*other.m[ 6] + this->m[ 2]*other.m[10] + this->m[ 3]*other.m[14],
  this->m[ 0]*other.m[ 3] + this->m[ 1]*other.m[ 7] + this->m[ 2]*other.m[11] + this->m[ 3]*other.m[15],
  this->m[ 4]*other.m[ 0] + this->m[ 5]*other.m[ 4] + this->m[ 6]*other.m[ 8] + this->m[ 7]*other.m[12],
  this->m[ 4]*other.m[ 1] + this->m[ 5]*other.m[ 5] + this->m[ 6]*other.m[ 9] + this->m[ 7]*other.m[13],
  this->m[ 4]*other.m[ 2] + this->m[ 5]*other.m[ 6] + this->m[ 6]*other.m[10] + this->m[ 7]*other.m[14],
  this->m[ 4]*other.m[ 3] + this->m[ 5]*other.m[ 7] + this->m[ 6]*other.m[11] + this->m[ 7]*other.m[15],
  this->m[ 8]*other.m[ 0] + this->m[ 9]*other.m[ 4] + this->m[10]*other.m[ 8] + this->m[11]*other.m[12],
  this->m[ 8]*other.m[ 1] + this->m[ 9]*other.m[ 5] + this->m[10]*other.m[ 9] + this->m[11]*other.m[13],
  this->m[ 8]*other.m[ 2] + this->m[ 9]*other.m[ 6] + this->m[10]*other.m[10] + this->m[11]*other.m[14],
  this->m[ 8]*other.m[ 3] + this->m[ 9]*other.m[ 7] + this->m[10]*other.m[11] + this->m[11]*other.m[15],
  this->m[12]*other.m[ 0] + this->m[13]*other.m[ 4] + this->m[14]*other.m[ 8] + this->m[15]*other.m[12],
  this->m[12]*other.m[ 1] + this->m[13]*other.m[ 5] + this->m[14]*other.m[ 9] + this->m[15]*other.m[13],
  this->m[12]*other.m[ 2] + this->m[13]*other.m[ 6] + this->m[14]*other.m[10] + this->m[15]*other.m[14],
  this->m[12]*other.m[ 3] + this->m[13]*other.m[ 7] + this->m[14]*other.m[11] + this->m[15]*other.m[15]
 };
 this->m[ 0] = temp[ 0];
 this->m[ 1] = temp[ 1];
 this->m[ 2] = temp[ 2];
 this->m[ 3] = temp[ 3];
 this->m[ 4] = temp[ 4];
 this->m[ 5] = temp[ 5];
 this->m[ 6] = temp[ 6];
 this->m[ 7] = temp[ 7];
 this->m[ 8] = temp[ 8];
 this->m[ 9] = temp[ 9];
 this->m[10] = temp[10];
 this->m[11] = temp[11];
 this->m[12] = temp[12];
 this->m[13] = temp[13];
 this->m[14] = temp[14];
 this->m[15] = temp[15];
 return *this;
}

template<class X>
inline matrix4x4<binary64>& matrix4x4<binary64>::operator +=(const matrix4x4<X>& other)
{
 this->m[ 0] += static_cast<binary64>(other.m[ 0]);
 this->m[ 1] += static_cast<binary64>(other.m[ 1]);
 this->m[ 2] += static_cast<binary64>(other.m[ 2]);
 this->m[ 3] += static_cast<binary64>(other.m[ 3]);
 this->m[ 4] += static_cast<binary64>(other.m[ 4]);
 this->m[ 5] += static_cast<binary64>(other.m[ 5]);
 this->m[ 6] += static_cast<binary64>(other.m[ 6]);
 this->m[ 7] += static_cast<binary64>(other.m[ 7]);
 this->m[ 8] += static_cast<binary64>(other.m[ 8]);
 this->m[ 9] += static_cast<binary64>(other.m[ 9]);
 this->m[10] += static_cast<binary64>(other.m[10]);
 this->m[11] += static_cast<binary64>(other.m[11]);
 this->m[12] += static_cast<binary64>(other.m[12]);
 this->m[13] += static_cast<binary64>(other.m[13]);
 this->m[14] += static_cast<binary64>(other.m[14]);
 this->m[15] += static_cast<binary64>(other.m[15]);
 return *this;
}

template<class X>
inline matrix4x4<binary64>& matrix4x4<binary64>::operator -=(const matrix4x4<X>& other)
{
 this->m[ 0] -= static_cast<binary64>(other.m[ 0]);
 this->m[ 1] -= static_cast<binary64>(other.m[ 1]);
 this->m[ 2] -= static_cast<binary64>(other.m[ 2]);
 this->m[ 3] -= static_cast<binary64>(other.m[ 3]);
 this->m[ 4] -= static_cast<binary64>(other.m[ 4]);
 this->m[ 5] -= static_cast<binary64>(other.m[ 5]);
 this->m[ 6] -= static_cast<binary64>(other.m[ 6]);
 this->m[ 7] -= static_cast<binary64>(other.m[ 7]);
 this->m[ 8] -= static_cast<binary64>(other.m[ 8]);
 this->m[ 9] -= static_cast<binary64>(other.m[ 9]);
 this->m[10] -= static_cast<binary64>(other.m[10]);
 this->m[11] -= static_cast<binary64>(other.m[11]);
 this->m[12] -= static_cast<binary64>(other.m[12]);
 this->m[13] -= static_cast<binary64>(other.m[13]);
 this->m[14] -= static_cast<binary64>(other.m[14]);
 this->m[15] -= static_cast<binary64>(other.m[15]);
 return *this;
}

template<class X>
inline matrix4x4<binary64>& matrix4x4<binary64>::operator *=(const matrix4x4<X>& other)
{
 binary64 cast[16] = {
  static_cast<binary64>(other.m[ 0]);
  static_cast<binary64>(other.m[ 1]);
  static_cast<binary64>(other.m[ 2]);
  static_cast<binary64>(other.m[ 3]);
  static_cast<binary64>(other.m[ 4]);
  static_cast<binary64>(other.m[ 5]);
  static_cast<binary64>(other.m[ 6]);
  static_cast<binary64>(other.m[ 7]);
  static_cast<binary64>(other.m[ 8]);
  static_cast<binary64>(other.m[ 9]);
  static_cast<binary64>(other.m[10]);
  static_cast<binary64>(other.m[11]);
  static_cast<binary64>(other.m[12]);
  static_cast<binary64>(other.m[13]);
  static_cast<binary64>(other.m[14]);
  static_cast<binary64>(other.m[15]);
 };
 binary64 temp[16] = {
  this->m[ 0]*cast[ 0] + this->m[ 1]*cast[ 4] + this->m[ 2]*cast[ 8] + this->m[ 3]*cast[12],
  this->m[ 0]*cast[ 1] + this->m[ 1]*cast[ 5] + this->m[ 2]*cast[ 9] + this->m[ 3]*cast[13],
  this->m[ 0]*cast[ 2] + this->m[ 1]*cast[ 6] + this->m[ 2]*cast[10] + this->m[ 3]*cast[14],
  this->m[ 0]*cast[ 3] + this->m[ 1]*cast[ 7] + this->m[ 2]*cast[11] + this->m[ 3]*cast[15],
  this->m[ 4]*cast[ 0] + this->m[ 5]*cast[ 4] + this->m[ 6]*cast[ 8] + this->m[ 7]*cast[12],
  this->m[ 4]*cast[ 1] + this->m[ 5]*cast[ 5] + this->m[ 6]*cast[ 9] + this->m[ 7]*cast[13],
  this->m[ 4]*cast[ 2] + this->m[ 5]*cast[ 6] + this->m[ 6]*cast[10] + this->m[ 7]*cast[14],
  this->m[ 4]*cast[ 3] + this->m[ 5]*cast[ 7] + this->m[ 6]*cast[11] + this->m[ 7]*cast[15],
  this->m[ 8]*cast[ 0] + this->m[ 9]*cast[ 4] + this->m[10]*cast[ 8] + this->m[11]*cast[12],
  this->m[ 8]*cast[ 1] + this->m[ 9]*cast[ 5] + this->m[10]*cast[ 9] + this->m[11]*cast[13],
  this->m[ 8]*cast[ 2] + this->m[ 9]*cast[ 6] + this->m[10]*cast[10] + this->m[11]*cast[14],
  this->m[ 8]*cast[ 3] + this->m[ 9]*cast[ 7] + this->m[10]*cast[11] + this->m[11]*cast[15],
  this->m[12]*cast[ 0] + this->m[13]*cast[ 4] + this->m[14]*cast[ 8] + this->m[15]*cast[12],
  this->m[12]*cast[ 1] + this->m[13]*cast[ 5] + this->m[14]*cast[ 9] + this->m[15]*cast[13],
  this->m[12]*cast[ 2] + this->m[13]*cast[ 6] + this->m[14]*cast[10] + this->m[15]*cast[14],
  this->m[12]*cast[ 3] + this->m[13]*cast[ 7] + this->m[14]*cast[11] + this->m[15]*cast[15]
 };
 this->m[ 0] = temp[ 0];
 this->m[ 1] = temp[ 1];
 this->m[ 2] = temp[ 2];
 this->m[ 3] = temp[ 3];
 this->m[ 4] = temp[ 4];
 this->m[ 5] = temp[ 5];
 this->m[ 6] = temp[ 6];
 this->m[ 7] = temp[ 7];
 this->m[ 8] = temp[ 8];
 this->m[ 9] = temp[ 9];
 this->m[10] = temp[10];
 this->m[11] = temp[11];
 this->m[12] = temp[12];
 this->m[13] = temp[13];
 this->m[14] = temp[14];
 this->m[15] = temp[15];
 return *this;
}

//
// EXPLICIT CONSTRUCTORS
//

inline matrix4x4<binary32>::matrix4x4(const matrix4x4<binary64>& other)
{
 this->m[ 0] = static_cast<binary32>(other.m[ 0]);
 this->m[ 1] = static_cast<binary32>(other.m[ 1]);
 this->m[ 2] = static_cast<binary32>(other.m[ 2]);
 this->m[ 3] = static_cast<binary32>(other.m[ 3]);
 this->m[ 4] = static_cast<binary32>(other.m[ 4]);
 this->m[ 5] = static_cast<binary32>(other.m[ 5]);
 this->m[ 6] = static_cast<binary32>(other.m[ 6]);
 this->m[ 7] = static_cast<binary32>(other.m[ 7]);
 this->m[ 8] = static_cast<binary32>(other.m[ 8]);
 this->m[ 9] = static_cast<binary32>(other.m[ 9]);
 this->m[10] = static_cast<binary32>(other.m[10]);
 this->m[11] = static_cast<binary32>(other.m[11]);
 this->m[12] = static_cast<binary32>(other.m[12]);
 this->m[13] = static_cast<binary32>(other.m[13]);
 this->m[14] = static_cast<binary32>(other.m[14]);
 this->m[15] = static_cast<binary32>(other.m[15]);
}

inline matrix4x4<binary64>::matrix4x4(const matrix4x4<binary32>& other)
{
 this->m[ 0] = static_cast<binary64>(other.m[ 0]);
 this->m[ 1] = static_cast<binary64>(other.m[ 1]);
 this->m[ 2] = static_cast<binary64>(other.m[ 2]);
 this->m[ 3] = static_cast<binary64>(other.m[ 3]);
 this->m[ 4] = static_cast<binary64>(other.m[ 4]);
 this->m[ 5] = static_cast<binary64>(other.m[ 5]);
 this->m[ 6] = static_cast<binary64>(other.m[ 6]);
 this->m[ 7] = static_cast<binary64>(other.m[ 7]);
 this->m[ 8] = static_cast<binary64>(other.m[ 8]);
 this->m[ 9] = static_cast<binary64>(other.m[ 9]);
 this->m[10] = static_cast<binary64>(other.m[10]);
 this->m[11] = static_cast<binary64>(other.m[11]);
 this->m[12] = static_cast<binary64>(other.m[12]);
 this->m[13] = static_cast<binary64>(other.m[13]);
 this->m[14] = static_cast<binary64>(other.m[14]);
 this->m[15] = static_cast<binary64>(other.m[15]);
}

//
// OPERATORS
//

template<class T>
inline matrix4x4<T> operator +(const matrix4x4<T>& lhs, const matrix4x4<T>& rhs)
{
 matrix4x4<T> r;
 r.m[ 0] = lhs.m[ 0] + rhs.m[ 0];
 r.m[ 1] = lhs.m[ 1] + rhs.m[ 1];
 r.m[ 2] = lhs.m[ 2] + rhs.m[ 2];
 r.m[ 3] = lhs.m[ 3] + rhs.m[ 3];
 r.m[ 4] = lhs.m[ 4] + rhs.m[ 4];
 r.m[ 5] = lhs.m[ 5] + rhs.m[ 5];
 r.m[ 6] = lhs.m[ 6] + rhs.m[ 6];
 r.m[ 7] = lhs.m[ 7] + rhs.m[ 7];
 r.m[ 8] = lhs.m[ 8] + rhs.m[ 8];
 r.m[ 9] = lhs.m[ 9] + rhs.m[ 9];
 r.m[10] = lhs.m[10] + rhs.m[10];
 r.m[11] = lhs.m[11] + rhs.m[11];
 r.m[12] = lhs.m[12] + rhs.m[12];
 r.m[13] = lhs.m[13] + rhs.m[13];
 r.m[14] = lhs.m[14] + rhs.m[14];
 r.m[15] = lhs.m[15] + rhs.m[15];
 return r;
}

template<class T>
inline matrix4x4<T> operator -(const matrix4x4<T>& lhs, const matrix4x4<T>& rhs)
{
 matrix4x4<T> r;
 r.m[ 0] = lhs.m[ 0] - rhs.m[ 0];
 r.m[ 1] = lhs.m[ 1] - rhs.m[ 1];
 r.m[ 2] = lhs.m[ 2] - rhs.m[ 2];
 r.m[ 3] = lhs.m[ 3] - rhs.m[ 3];
 r.m[ 4] = lhs.m[ 4] - rhs.m[ 4];
 r.m[ 5] = lhs.m[ 5] - rhs.m[ 5];
 r.m[ 6] = lhs.m[ 6] - rhs.m[ 6];
 r.m[ 7] = lhs.m[ 7] - rhs.m[ 7];
 r.m[ 8] = lhs.m[ 8] - rhs.m[ 8];
 r.m[ 9] = lhs.m[ 9] - rhs.m[ 9];
 r.m[10] = lhs.m[10] - rhs.m[10];
 r.m[11] = lhs.m[11] - rhs.m[11];
 r.m[12] = lhs.m[12] - rhs.m[12];
 r.m[13] = lhs.m[13] - rhs.m[13];
 r.m[14] = lhs.m[14] - rhs.m[14];
 r.m[15] = lhs.m[15] - rhs.m[15];
 return r;
}

template<class T>
inline matrix4x4<T> operator *(const matrix4x4<T>& lhs, const matrix4x4<T>& rhs)
{
 matrix4x4<T> r;
 r.m[ 0] = lhs.m[ 0]*rhs.m[ 0] + lhs.m[ 1]*rhs.m[ 4] + lhs.m[ 2]*rhs.m[ 8] + lhs.m[ 3]*rhs.m[12];
 r.m[ 1] = lhs.m[ 0]*rhs.m[ 1] + lhs.m[ 1]*rhs.m[ 5] + lhs.m[ 2]*rhs.m[ 9] + lhs.m[ 3]*rhs.m[13];
 r.m[ 2] = lhs.m[ 0]*rhs.m[ 2] + lhs.m[ 1]*rhs.m[ 6] + lhs.m[ 2]*rhs.m[10] + lhs.m[ 3]*rhs.m[14];
 r.m[ 3] = lhs.m[ 0]*rhs.m[ 3] + lhs.m[ 1]*rhs.m[ 7] + lhs.m[ 2]*rhs.m[11] + lhs.m[ 3]*rhs.m[15];
 r.m[ 4] = lhs.m[ 4]*rhs.m[ 0] + lhs.m[ 5]*rhs.m[ 4] + lhs.m[ 6]*rhs.m[ 8] + lhs.m[ 7]*rhs.m[12];
 r.m[ 5] = lhs.m[ 4]*rhs.m[ 1] + lhs.m[ 5]*rhs.m[ 5] + lhs.m[ 6]*rhs.m[ 9] + lhs.m[ 7]*rhs.m[13];
 r.m[ 6] = lhs.m[ 4]*rhs.m[ 2] + lhs.m[ 5]*rhs.m[ 6] + lhs.m[ 6]*rhs.m[10] + lhs.m[ 7]*rhs.m[14];
 r.m[ 7] = lhs.m[ 4]*rhs.m[ 3] + lhs.m[ 5]*rhs.m[ 7] + lhs.m[ 6]*rhs.m[11] + lhs.m[ 7]*rhs.m[15];
 r.m[ 8] = lhs.m[ 8]*rhs.m[ 0] + lhs.m[ 9]*rhs.m[ 4] + lhs.m[10]*rhs.m[ 8] + lhs.m[11]*rhs.m[12];
 r.m[ 9] = lhs.m[ 8]*rhs.m[ 1] + lhs.m[ 9]*rhs.m[ 5] + lhs.m[10]*rhs.m[ 9] + lhs.m[11]*rhs.m[13];
 r.m[10] = lhs.m[ 8]*rhs.m[ 2] + lhs.m[ 9]*rhs.m[ 6] + lhs.m[10]*rhs.m[10] + lhs.m[11]*rhs.m[14];
 r.m[11] = lhs.m[ 8]*rhs.m[ 3] + lhs.m[ 9]*rhs.m[ 7] + lhs.m[10]*rhs.m[11] + lhs.m[11]*rhs.m[15];
 r.m[12] = lhs.m[12]*rhs.m[ 0] + lhs.m[13]*rhs.m[ 4] + lhs.m[14]*rhs.m[ 8] + lhs.m[15]*rhs.m[12];
 r.m[13] = lhs.m[12]*rhs.m[ 1] + lhs.m[13]*rhs.m[ 5] + lhs.m[14]*rhs.m[ 9] + lhs.m[15]*rhs.m[13];
 r.m[14] = lhs.m[12]*rhs.m[ 2] + lhs.m[13]*rhs.m[ 6] + lhs.m[14]*rhs.m[10] + lhs.m[15]*rhs.m[14];
 r.m[15] = lhs.m[12]*rhs.m[ 3] + lhs.m[13]*rhs.m[ 7] + lhs.m[14]*rhs.m[11] + lhs.m[15]*rhs.m[15];
 return r;
}

template<class T>
inline matrix4x4<T> operator *(const matrix4x4<T>& lhs, const T& rhs)
{
 matrix4x4<T> r;
 r.m[ 0] = lhs.m[ 0] * rhs;
 r.m[ 1] = lhs.m[ 1] * rhs;
 r.m[ 2] = lhs.m[ 2] * rhs;
 r.m[ 3] = lhs.m[ 3] * rhs;
 r.m[ 4] = lhs.m[ 4] * rhs;
 r.m[ 5] = lhs.m[ 5] * rhs;
 r.m[ 6] = lhs.m[ 6] * rhs;
 r.m[ 7] = lhs.m[ 7] * rhs;
 r.m[ 8] = lhs.m[ 8] * rhs;
 r.m[ 9] = lhs.m[ 9] * rhs;
 r.m[10] = lhs.m[10] * rhs;
 r.m[11] = lhs.m[11] * rhs;
 r.m[12] = lhs.m[12] * rhs;
 r.m[13] = lhs.m[13] * rhs;
 r.m[14] = lhs.m[14] * rhs;
 r.m[15] = lhs.m[15] * rhs;
 return r;
}

template<class T>
inline matrix4x4<T> operator *(const T& lhs, const matrix4x4<T>& rhs)
{
 matrix4x4<T> r;
 r.m[ 0] = rhs.m[ 0] * lhs;
 r.m[ 1] = rhs.m[ 1] * lhs;
 r.m[ 2] = rhs.m[ 2] * lhs;
 r.m[ 3] = rhs.m[ 3] * lhs;
 r.m[ 4] = rhs.m[ 4] * lhs;
 r.m[ 5] = rhs.m[ 5] * lhs;
 r.m[ 6] = rhs.m[ 6] * lhs;
 r.m[ 7] = rhs.m[ 7] * lhs;
 r.m[ 8] = rhs.m[ 8] * lhs;
 r.m[ 9] = rhs.m[ 9] * lhs;
 r.m[10] = rhs.m[10] * lhs;
 r.m[11] = rhs.m[11] * lhs;
 r.m[12] = rhs.m[12] * lhs;
 r.m[13] = rhs.m[13] * lhs;
 r.m[14] = rhs.m[14] * lhs;
 r.m[15] = rhs.m[15] * lhs;
 return r;
}

template<class T>
inline matrix4x4<T> operator /(const matrix4x4<T>& lhs, const T& rhs)
{
 matrix4x4<T> r;
 r.m[ 0] = lhs.m[ 0] / rhs;
 r.m[ 1] = lhs.m[ 1] / rhs;
 r.m[ 2] = lhs.m[ 2] / rhs;
 r.m[ 3] = lhs.m[ 3] / rhs;
 r.m[ 4] = lhs.m[ 4] / rhs;
 r.m[ 5] = lhs.m[ 5] / rhs;
 r.m[ 6] = lhs.m[ 6] / rhs;
 r.m[ 7] = lhs.m[ 7] / rhs;
 r.m[ 8] = lhs.m[ 8] / rhs;
 r.m[ 9] = lhs.m[ 9] / rhs;
 r.m[10] = lhs.m[10] / rhs;
 r.m[11] = lhs.m[11] / rhs;
 r.m[12] = lhs.m[12] / rhs;
 r.m[13] = lhs.m[13] / rhs;
 r.m[14] = lhs.m[14] / rhs;
 r.m[15] = lhs.m[15] / rhs;
 return r;
}

template<class T>
inline matrix4x4<T> operator +(const matrix4x4<T>& rhs)
{
 return rhs;
}

template<class T>
inline matrix4x4<T> operator -(const matrix4x4<T>& rhs)
{
 matrix4x4<T> r;
 r.m[ 0] = -rhs.m[ 0];
 r.m[ 1] = -rhs.m[ 1];
 r.m[ 2] = -rhs.m[ 2];
 r.m[ 3] = -rhs.m[ 3];
 r.m[ 4] = -rhs.m[ 4];
 r.m[ 5] = -rhs.m[ 5];
 r.m[ 6] = -rhs.m[ 6];
 r.m[ 7] = -rhs.m[ 7];
 r.m[ 8] = -rhs.m[ 8];
 r.m[ 9] = -rhs.m[ 9];
 r.m[10] = -rhs.m[10];
 r.m[11] = -rhs.m[11];
 r.m[12] = -rhs.m[12];
 r.m[13] = -rhs.m[13];
 r.m[14] = -rhs.m[14];
 r.m[15] = -rhs.m[15];
 return r;
}

template<class T>
inline vector3D<T> operator *(const matrix4x4<T>& lhs, const vector3D<T>& rhs)
{
 return vector3D<T>(
  lhs.m[0]*rhs[0] + lhs.m[1]*rhs[1] + lhs.m[ 2]*rhs[2] + lhs.m[ 3],
  lhs.m[4]*rhs[0] + lhs.m[5]*rhs[1] + lhs.m[ 6]*rhs[2] + lhs.m[ 7],
  lhs.m[8]*rhs[0] + lhs.m[9]*rhs[1] + lhs.m[10]*rhs[2] + lhs.m[11]
 );
}

template<class T, class charT, class traits>
std::basic_ostream<charT, traits>& operator <<(std::basic_ostream<charT, traits>& os, const matrix4x4<T>& m)
{
 // copy
 std::basic_ostringstream<charT, traits> oss;
 oss.flags(os.flags());
 oss.imbue(os.getloc());
 oss.precision(os.precision());

 // save to stream
 oss << '[' << m[ 0] << ',' << m[ 1] << ',' << m[ 2] << ',' << m[ 3] << ']' << ' ';
 oss << '[' << m[ 4] << ',' << m[ 5] << ',' << m[ 6] << ',' << m[ 7] << ']' << ' ';
 oss << '[' << m[ 8] << ',' << m[ 9] << ',' << m[10] << ',' << m[11] << ']' << ' ';
 oss << '[' << m[12] << ',' << m[13] << ',' << m[14] << ',' << m[15] << ']';

 // output to stream
 std::basic_string<charT, traits> str = oss.str();
 return (os << str.c_str());
}

//
// FUNCTIONS
//

template<class T>
inline void load_identity(matrix4x4<T>& m)
{
 m[ 0] = m[ 5] = m[10] = m[15] = constants<T>::one();
 m[ 1] = m[ 2] = m[ 3] = m[ 4] = constants<T>::zero();
 m[ 6] = m[ 7] = m[ 8] = m[ 9] = constants<T>::zero();
 m[11] = m[12] = m[13] = m[14] = constants<T>::zero();
}

template<class T>
inline void load_translation(matrix4x4<T>& m, T x, T y, T z)
{
 m[ 0] = m[ 5] = m[10] = m[15] = constants<T>::one();
 m[ 1] = m[ 2] = m[ 4] = constants<T>::zero();
 m[ 6] = m[ 8] = m[ 9] = constants<T>::zero();
 m[12] = m[13] = m[14] = constants<T>::zero();
 m[ 3] = x;
 m[ 7] = y;
 m[11] = z;
}

template<class T>
inline void load_translation(matrix4x4<T>& m, const T* v)
{
 m[ 0] = m[ 5] = m[10] = m[15] = constants<T>::one();
 m[ 1] = m[ 2] = m[ 4] = constants<T>::zero();
 m[ 6] = m[ 8] = m[ 9] = constants<T>::zero();
 m[12] = m[13] = m[14] = constants<T>::zero();
 m[ 3] = v[0];
 m[ 7] = v[1];
 m[11] = v[2];
}

template<class T>
inline void load_translation(matrix4x4<T>& m, const vector3D<T>& v)
{
 m[ 0] = m[ 5] = m[10] = m[15] = constants<T>::one();
 m[ 1] = m[ 2] = m[ 4] = constants<T>::zero();
 m[ 6] = m[ 8] = m[ 9] = constants<T>::zero();
 m[12] = m[13] = m[14] = constants<T>::zero();
 m[ 3] = v[0];
 m[ 7] = v[1];
 m[11] = v[2];
}

template<class T>
inline void load_scale(matrix4x4<T>& m, T x, T y, T z)
{
 m[m.m11] = x;
 m[m.m12] = constants<T>::zero();
 m[m.m13] = constants<T>::zero();
 m[m.m14] = constants<T>::zero();
 m[m.m21] = constants<T>::zero();
 m[m.m22] = y;
 m[m.m23] = constants<T>::zero();
 m[m.m24] = constants<T>::zero();
 m[m.m31] = constants<T>::zero();
 m[m.m32] = constants<T>::zero();
 m[m.m33] = z;
 m[m.m34] = constants<T>::zero();
 m[m.m41] = constants<T>::zero();
 m[m.m42] = constants<T>::zero();
 m[m.m43] = constants<T>::zero();
 m[m.m44] = constants<T>::one();
}

template<class T>
inline void load_scale(matrix4x4<T>& m, const T* v)
{
 m[m.m11] = v[0];
 m[m.m12] = constants<T>::zero();
 m[m.m13] = constants<T>::zero();
 m[m.m14] = constants<T>::zero();
 m[m.m21] = constants<T>::zero();
 m[m.m22] = v[1];
 m[m.m23] = constants<T>::zero();
 m[m.m24] = constants<T>::zero();
 m[m.m31] = constants<T>::zero();
 m[m.m32] = constants<T>::zero();
 m[m.m33] = v[2];
 m[m.m34] = constants<T>::zero();
 m[m.m41] = constants<T>::zero();
 m[m.m42] = constants<T>::zero();
 m[m.m43] = constants<T>::zero();
 m[m.m44] = constants<T>::one();
}

template<class T>
inline void load_scale(matrix4x4<T>& m, const vector3D<T>& v)
{
 m[m.m11] = v[0];
 m[m.m12] = constants<T>::zero();
 m[m.m13] = constants<T>::zero();
 m[m.m14] = constants<T>::zero();
 m[m.m21] = constants<T>::zero();
 m[m.m22] = v[1];
 m[m.m23] = constants<T>::zero();
 m[m.m24] = constants<T>::zero();
 m[m.m31] = constants<T>::zero();
 m[m.m32] = constants<T>::zero();
 m[m.m33] = v[2];
 m[m.m34] = constants<T>::zero();
 m[m.m41] = constants<T>::zero();
 m[m.m42] = constants<T>::zero();
 m[m.m43] = constants<T>::zero();
 m[m.m44] = constants<T>::one();
}

template<class T>
inline T inverse(const matrix4x4<T>& src, matrix4x4<T>& dst)
{
 // cache 2x2 determinants
 T D[12] = {
  src[0xA]*src[0xF] - src[0xB]*src[0xE],
  src[0x9]*src[0xF] - src[0xB]*src[0xD],
  src[0x9]*src[0xE] - src[0xA]*src[0xD],
  src[0x8]*src[0xF] - src[0xB]*src[0xC],
  src[0x8]*src[0xE] - src[0xA]*src[0xC],
  src[0x8]*src[0xD] - src[0x9]*src[0xC],
  src[0x2]*src[0x7] - src[0x3]*src[0x6],
  src[0x1]*src[0x7] - src[0x3]*src[0x5],
  src[0x1]*src[0x6] - src[0x2]*src[0x5],
  src[0x0]*src[0x7] - src[0x3]*src[0x4],
  src[0x0]*src[0x6] - src[0x2]*src[0x4],
  src[0x0]*src[0x5] - src[0x1]*src[0x4]
 };

 // compute determinant
 T det = D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
 if(is_zero(det)) return det;

 // inv(M) = inv(det(M)) * transpose(C)
 T invdet = inv(det);
 dst[0x0] = +(src[0x5]*D[0x0] - src[0x6]*D[0x1] + src[0x7]*D[0x2])*invdet;
 dst[0x4] = -(src[0x4]*D[0x0] - src[0x6]*D[0x3] + src[0x7]*D[0x4])*invdet;
 dst[0x8] = +(src[0x4]*D[0x1] - src[0x5]*D[0x3] + src[0x7]*D[0x5])*invdet;
 dst[0xC] = -(src[0x4]*D[0x2] - src[0x5]*D[0x4] + src[0x6]*D[0x5])*invdet;
 dst[0x1] = -(src[0x1]*D[0x0] - src[0x2]*D[0x1] + src[0x3]*D[0x2])*invdet;
 dst[0x5] = +(src[0x0]*D[0x0] - src[0x2]*D[0x3] + src[0x3]*D[0x4])*invdet;
 dst[0x9] = -(src[0x0]*D[0x1] - src[0x1]*D[0x3] + src[0x3]*D[0x5])*invdet;
 dst[0xD] = +(src[0x0]*D[0x2] - src[0x1]*D[0x4] + src[0x2]*D[0x5])*invdet;
 dst[0x2] = +(src[0xD]*D[0x6] - src[0xE]*D[0x7] + src[0xF]*D[0x8])*invdet;
 dst[0x6] = -(src[0xC]*D[0x6] - src[0xE]*D[0x9] + src[0xF]*D[0xA])*invdet;
 dst[0xA] = +(src[0xC]*D[0x7] - src[0xD]*D[0x9] + src[0xF]*D[0xB])*invdet;
 dst[0xE] = -(src[0xC]*D[0x8] - src[0xD]*D[0xA] + src[0xE]*D[0xB])*invdet;
 dst[0x3] = -(src[0x9]*D[0x6] - src[0xA]*D[0x7] + src[0xB]*D[0x8])*invdet;
 dst[0x7] = +(src[0x8]*D[0x6] - src[0xA]*D[0x9] + src[0xB]*D[0xA])*invdet;
 dst[0xB] = -(src[0x8]*D[0x7] - src[0x9]*D[0x9] + src[0xB]*D[0xB])*invdet;
 dst[0xF] = +(src[0x8]*D[0x8] - src[0x9]*D[0xA] + src[0xA]*D[0xB])*invdet;

 // return determinant
 return det;
}

template<class T>
inline void transpose(const matrix4x4<T>& src, matrix4x4<T>& dst)
{
 dst.m[dst.m11] = src.m[src.m11];
 dst.m[dst.m12] = src.m[src.m21];
 dst.m[dst.m13] = src.m[src.m31];
 dst.m[dst.m14] = src.m[src.m41];
 dst.m[dst.m21] = src.m[src.m12];
 dst.m[dst.m22] = src.m[src.m22];
 dst.m[dst.m23] = src.m[src.m32];
 dst.m[dst.m24] = src.m[src.m42];
 dst.m[dst.m31] = src.m[src.m13];
 dst.m[dst.m32] = src.m[src.m23];
 dst.m[dst.m33] = src.m[src.m33];
 dst.m[dst.m34] = src.m[src.m43];
 dst.m[dst.m41] = src.m[src.m14];
 dst.m[dst.m42] = src.m[src.m24];
 dst.m[dst.m43] = src.m[src.m34];
 dst.m[dst.m44] = src.m[src.m44];
}

}};

#endif
