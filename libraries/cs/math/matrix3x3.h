#ifndef __CS_MATH_MATRIX3X3__
#define __CS_MATH_MATRIX3X3__

// CS headers
#include<cs/math/functions.h>
#include<cs/math/vector2D.h>
#include<cs/math/vector3D.h>

//
// FUNCTIONS
//
namespace cs { namespace math {


template<class char_type, class T>
void matrix3x3_print(std::basic_ostream<char_type>& os, const T* A)
{
 os << "row[0][" << A[0x0] << ", " << A[0x1] << ", " << A[0x2] << "] - ";
 os << "row[1][" << A[0x3] << ", " << A[0x4] << ", " << A[0x5] << "] - ";
 os << "row[2][" << A[0x6] << ", " << A[0x7] << ", " << A[0x8] << "]";
}

template<class char_type, class T>
void matrix3x3_println(std::basic_ostream<char_type>& os, const T* A)
{
 os << "row[0][" << A[0x0] << ", " << A[0x1] << ", " << A[0x2] << "] - ";
 os << "row[1][" << A[0x3] << ", " << A[0x4] << ", " << A[0x5] << "] - ";
 os << "row[2][" << A[0x6] << ", " << A[0x7] << ", " << A[0x8] << "]" << std::endl;
}

template<class T>
void matrix3x3_mul(T* X, const T* A, const T* B)
{
 X[0] = A[0]*B[0] + A[1]*B[3] + A[2]*B[6];
 X[1] = A[0]*B[1] + A[1]*B[4] + A[2]*B[7];
 X[2] = A[0]*B[2] + A[1]*B[5] + A[2]*B[8];
 X[3] = A[3]*B[0] + A[4]*B[3] + A[5]*B[6];
 X[4] = A[3]*B[1] + A[4]*B[4] + A[5]*B[7];
 X[5] = A[3]*B[2] + A[4]*B[5] + A[5]*B[8];
 X[6] = A[6]*B[0] + A[7]*B[3] + A[8]*B[6];
 X[7] = A[6]*B[1] + A[7]*B[4] + A[8]*B[7];
 X[8] = A[6]*B[2] + A[7]*B[5] + A[8]*B[8];
}

template<class T>
void matrix3x3_mul(T* X, const T* A)
{
 T temp[] = {
  X[0]*A[0] + X[1]*A[3] + X[2]*A[6];
  X[0]*A[1] + X[1]*A[4] + X[2]*A[7];
  X[0]*A[2] + X[1]*A[5] + X[2]*A[8];
  X[3]*A[0] + X[4]*A[3] + X[5]*A[6];
  X[3]*A[1] + X[4]*A[4] + X[5]*A[7];
  X[3]*A[2] + X[4]*A[5] + X[5]*A[8];
  X[6]*A[0] + X[7]*A[3] + X[8]*A[6];
  X[6]*A[1] + X[7]*A[4] + X[8]*A[7];
  X[6]*A[2] + X[7]*A[5] + X[8]*A[8];
 };
 X[0] = temp[0];
 X[1] = temp[1];
 X[2] = temp[2];
 X[3] = temp[3];
 X[4] = temp[4];
 X[5] = temp[5];
 X[6] = temp[6];
 X[7] = temp[7];
 X[8] = temp[8];
}

template<class T>
void matrix3x3_inv(T* X, const T* A)
{
 T temp[] = {
  (A[4]*A[8] - A[5]*A[7]),  (A[5]*A[6] - A[3]*A[8]),  (A[3]*A[7] - A[4]*A[6]),
  (A[2]*A[7] - A[1]*A[8]),  (A[0]*A[8] - A[2]*A[6]),  (A[1]*A[6] - A[0]*A[7]),
  (A[1]*A[5] - A[2]*A[4]),  (A[2]*A[3] - A[0]*A[5]),  (A[0]*A[4] - A[1]*A[3])
 };
 T determinant = A[0]*temp[0] + A[1]*temp[1] + A[2]*temp[2];
 T scale = inv(determinant);
 X[0] = scale*temp[0];
 X[1] = scale*temp[3];
 X[2] = scale*temp[6];
 X[3] = scale*temp[1];
 X[4] = scale*temp[4];
 X[5] = scale*temp[7];
 X[6] = scale*temp[2];
 X[7] = scale*temp[5];
 X[8] = scale*temp[8];
}

}}

//
// CLASSES
//
namespace cs { namespace math {

}}

#endif
