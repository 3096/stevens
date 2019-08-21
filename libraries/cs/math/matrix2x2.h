#ifndef __CS_MATH_MATRIX2X2__
#define __CS_MATH_MATRIX2X2__

// CS headers
#include<cs/math/functions.h>
#include<cs/math/vector2D.h>

//
// FUNCTIONS
//
namespace cs { namespace math {

template<class char_type, class T>
void matrix2x2_print(std::basic_ostream<char_type>& os, const T* A)
{
 os << "row[0][" << A[0x0] << ", " << A[0x1] << "] - ";
 os << "row[1][" << A[0x2] << ", " << A[0x3] << "]";
}

template<class char_type, class T>
void matrix2x2_println(std::basic_ostream<char_type>& os, const T* A)
{
 os << "row[0][" << A[0x0] << ", " << A[0x1] << "] - ";
 os << "row[1][" << A[0x2] << ", " << A[0x3] << "]" << std::endl;
}

template<class T>
void matrix2x2_copy(T* X, const T* A)
{
 X[0x0] = A[0x0];
 X[0x1] = A[0x1];
 X[0x2] = A[0x2];
 X[0x3] = A[0x3];
}

template<class T>
void matrix2x2_transpose(T* X, const T* A)
{
 X[0x0] = A[0x0];
 X[0x1] = A[0x2];
 X[0x2] = A[0x1];
 X[0x3] = A[0x3];
}

template<class T>
void matrix2x2_transpose(T* X)
{
 T temp = X[0x1];
 X[0x1] = X[0x2];
 X[0x2] = temp;
}

template<class T>
void matrix2x2_add(T* X, const T* A, const T* B)
{
 X[0x0] = A[0x0] + B[0x0];
 X[0x1] = A[0x1] + B[0x1];
 X[0x2] = A[0x2] + B[0x2];
 X[0x3] = A[0x3] + B[0x3];
}

template<class T>
void matrix2x2_add(T* X, const T* A)
{
 X[0x0] += A[0x0];
 X[0x1] += A[0x1];
 X[0x2] += A[0x2];
 X[0x3] += A[0x3];
}

template<class T>
void matrix2x2_sub(T* X, const T* A, const T* B)
{
 X[0x0] = A[0x0] - B[0x0];
 X[0x1] = A[0x1] - B[0x1];
 X[0x2] = A[0x2] - B[0x2];
 X[0x3] = A[0x3] - B[0x3];
}

template<class T>
void matrix2x2_sub(T* X, const T* A)
{
 X[0x0] -= A[0x0];
 X[0x1] -= A[0x1];
 X[0x2] -= A[0x2];
 X[0x3] -= A[0x3];
}

template<class T>
void matrix2x2_mul(T* X, const T* A, const T* B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x2];
 X[0x1] = A[0x0]*B[0x1] + A[0x1]*B[0x3];
 X[0x2] = A[0x2]*B[0x0] + A[0x3]*B[0x2];
 X[0x3] = A[0x2]*B[0x1] + A[0x3]*B[0x3];
}

template<class T>
void matrix2x2_mul(T* X, const T* A)
{
 T temp[0x4] = {
  X[0x0]*A[0x0] + X[0x1]*A[0x2],
  X[0x0]*A[0x1] + X[0x1]*A[0x3],
  X[0x2]*A[0x0] + X[0x3]*A[0x2],
  X[0x2]*A[0x1] + X[0x3]*A[0x3]
 };
 X[0x0] = temp[0x0];
 X[0x1] = temp[0x1];
 X[0x2] = temp[0x2];
 X[0x3] = temp[0x3];
}

template<class T>
void matrix2x2_mul(T* X, const T* A, const T& scalar)
{
 X[0x0] = A[0x0]*scalar;
 X[0x1] = A[0x1]*scalar;
 X[0x2] = A[0x2]*scalar;
 X[0x3] = A[0x3]*scalar;
}

template<class T>
void matrix2x2_mul(T* X, const T& scalar, const T* A)
{
 X[0x0] = A[0x0]*scalar;
 X[0x1] = A[0x1]*scalar;
 X[0x2] = A[0x2]*scalar;
 X[0x3] = A[0x3]*scalar;
}

template<class T>
void matrix2x2_mul(T* X, const T& scalar)
{
 X[0x0] *= scalar;
 X[0x1] *= scalar;
 X[0x2] *= scalar;
 X[0x3] *= scalar;
}

template<class T>
void matrix2x2_div(T* X, const T* A, const T& scalar)
{
 X[0x0] = A[0x0]/scalar;
 X[0x1] = A[0x1]/scalar;
 X[0x2] = A[0x2]/scalar;
 X[0x3] = A[0x3]/scalar;
}

template<class T>
void matrix2x2_div(T* X, const T& scalar, const T* A)
{
 T factor = scalar*inv(A[0]*A[3] - A[1]*A[2]);
 X[0x0] =  factor*A[0x3];
 X[0x1] = -factor*A[0x1];
 X[0x2] = -factor*A[0x2];
 X[0x3] =  factor*A[0x0];
}

template<class T>
void matrix2x2_div(T* X, const T& scalar)
{
 X[0x0] /= scalar;
 X[0x1] /= scalar;
 X[0x2] /= scalar;
 X[0x3] /= scalar;
}

template<class T>
void matrix2x2_inv(T* X, const T* A)
{
 T factor = inv(A[0]*A[3] - A[1]*A[2]);
 X[0x0] =  factor*A[0x3];
 X[0x1] = -factor*A[0x1];
 X[0x2] = -factor*A[0x2];
 X[0x3] =  factor*A[0x0];
}

template<class T>
void matrix2x2_inv(T* X)
{
 T factor = inv(X[0]*X[3] - X[1]*X[2]);
 X[0x0] *=  factor;
 X[0x1] *= -factor;
 X[0x2] *= -factor;
 X[0x3] *=  factor;
}

}}

//
// CLASSES
//
namespace cs { namespace math {

}}

#endif
