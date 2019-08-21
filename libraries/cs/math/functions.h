#ifndef __CS_MATH_FUNCTIONS_H
#define __CS_MATH_FUNCTIONS_H

// Standard Includes
#include<iterator>

// CS Includes
#include<cs/core/types.h>
#include<cs/math/constants.h>

//
// SIMPLE FUNCTIONS
// COMPARISON
//
namespace cs { namespace math {

template<class T> inline bool equals_zero(T x) { return (x == constants<T>::zero()); }
template<class T> inline bool equals_one(T x) { return (x == constants<T>::one()); }
template<class T> inline bool less_than_zero(T x) { return x < constants<T>::zero(); }
template<class T> inline bool less_than_one(T x) { return x < constants<T>::one(); }
template<class T> inline bool greater_than_zero(T x) { return constants<T>::zero() < x; }
template<class T> inline bool greater_than_one(T x) { return constants<T>::one() < x; }

}}

//
// SIMPLE FUNCTIONS
// SCALING
//
namespace cs { namespace math {

// plus/minus one
template<class T> inline T one_plus(T x) { return constants<T>::one() + x; }
template<class T> inline T plus_one(T x) { return x + constants<T>::one(); }
template<class T> inline T one_minus(T x) { return constants<T>::one() - x; }
template<class T> inline T minus_one(T x) { return x - constants<T>::one(); }

// scale by 2
template<class T> inline T dbl(T x) { return static_cast<T>(2)*x; }
template<class T> inline T halve(T x) { return x/static_cast<T>(2); }
template<> inline binary32 halve(binary32 x) { return constants<binary32>::half()*x; }
template<> inline binary64 halve(binary64 x) { return constants<binary64>::half()*x; }

// scale by 4
template<class T> inline T quadruple(T x) { return static_cast<T>(4)*x; }
template<class T> inline T quarter(T x) { return x/static_cast<T>(4); }
template<> inline binary32 quarter(binary32 x) { return constants<binary32>::one_over_4()*x; }
template<> inline binary64 quarter(binary64 x) { return constants<binary64>::one_over_4()*x; }

}}

//
// SIMPLE FUNCTIONS
// TRANSCENDENTALS
//
namespace cs { namespace math {

// not really transcendentals
template<class T> inline T abs(T x) { return std::abs(x); }
template<class T> inline T inv(T x) { return constants<T>::one()/x; }

// power functions
template<class T> inline T sqrt(T x) { return std::sqrt(x); }
template<class T> inline T pow(T x, T y) { return std::pow(x, y); }
template<class T> inline T pow(T x, sint32 y) { return std::pow(x, y); }

// exponentials and logarithms
template<class T> inline T exp(T x) { return std::exp(x); }
template<class T> inline T ln(T x) { return std::log(x); } // natural logarithm (ln is ISO name)
template<class T> inline T lg(T x) { return std::log10(x); } // common logarithm (lg is ISO name)
template<class T> inline T lb(T x) { return std::log(x)*constants<T>::lb_of_e(); } // binary logarithm (lb is ISO name)

// trignometric conversions
template<class T> inline T radians(T d) { return d*constants<T>::pi_over_180(); }
template<class T> inline T degrees(T r) { return r*constants<T>::pi_under_180(); }

// sine, cosine and tangent
template<class T> inline T sin(T x) { return std::sin(x); }
template<class T> inline T cos(T x) { return std::cos(x); }
template<class T> inline T tan(T x) { return std::tan(x); }

// inverse sine, cosine and tangent
template<class T> inline T asin(T x) { return std::asin(x); }
template<class T> inline T acos(T x) { return std::acos(x); }
template<class T> inline T atan(T x) { return std::atan(x); }
template<class T> inline T atan(T y, T x) { return std::atan2(y, x); }

// hyperbolic sine, cosine and tangent
template<class T> inline T sinh(T x) { return std::sinh(x); }
template<class T> inline T cosh(T x) { return std::cosh(x); }
template<class T> inline T tanh(T x) { return std::tanh(x); }

// inverse hyperbolics
template<class T> inline T asinh(T x) { return ln(x + sqrt(plus_one(x*x))); }
template<class T> inline T acosh(T x) { return ln(x + sqrt(minus_one(x*x))); }
template<class T> inline T atanh(T x) { return halve(ln(one_plus(x)/one_minus(x))); }
template<class T> inline T asech(T x) { return ln(inv(x)*one_plus(sqrt(one_minus(x*x)))); }
template<class T> inline T acsch(T x) {
 if(greater_than_zero(x)) return ln(inv(x)*one_plus(sqrt(one_plus(x*x))));
 if(less_than_zero(x)) return ln(inv(x)*one_minus(sqrt(one_plus(x*x))));
 return binary_traits<T>::positive_qnan();
}
template<class T> inline T acoth(T x) { return halve(ln(plus_one(x)/minus_one(x))); }

}}

//
// SIMPLE FUNCTIONS
// NORMS AND DISTANCES
// TODO: Move to norm.h and distance.h
//
namespace cs { namespace math {

// n-dimensional
template<class ForwardIterator>
inline typename std::iterator_traits<ForwardIterator>::value_type taxicab_norm(ForwardIterator begin, ForwardIterator end)
{
 typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
 value_type sum = constants<value_type>::zero();
 for(; begin != end; begin++) sum += abs(*begin);
 return sum;
}

template<class ForwardIterator>
inline typename std::iterator_traits<ForwardIterator>::value_type taxicab_norm(ForwardIterator begin, size_t n)
{
 typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
 value_type sum = constants<value_type>::zero();
 for(size_t i = 0; i < n; i++) sum += abs(*(begin++));
 return sum;
}

// n-dimensional
template<class ForwardIterator>
inline typename std::iterator_traits<ForwardIterator>::value_type taxicab_distance(ForwardIterator b1, ForwardIterator b2, size_t n)
{
 typedef typename std::iterator_traits<ForwardIterator>::value_type value_type;
 value_type sum = constants<value_type>::zero();
 for(size_t i = 0; i < n; i++) sum += abs(*(b1++) - *(b2++));
 return sum;
}

// n-dimensional
template<class ForwardIterator>
inline typename std::iterator_traits<ForwardIterator>::value_type euclidean_norm(ForwardIterator begin, ForwardIterator end)
{
 if(std::next(begin) == end) return abs(*begin); // no need for sqrt
 typedef typename std::iterator_traits<RandomAccessIterator>::value_type value_type;
 value_type sum = constants<value_type>::zero();
 for(; begin != end; begin++) sum += (*begin)*(*begin);
 return sqrt(sum);
}

}}

//
// SIMPLE FUNCTIONS
// RULE OF SARRUS
//
namespace cs { namespace math {

template<class T>
inline T sarrus_2x2(T m11, T m12, T m21, T m22)
{
 return m11*m22 - m12*m21;
}

template<class T>
inline T sarrus_2x2(const T* m)
{
 return m[0]*m[3] - m[1]*m[2];
}

template<class T>
inline T sarrus_3x3(T m11, T m12, T m13, T m21, T m22, T m23, T m31, T m32, T m33)
{
 return (m11*m22*m33) + (m12*m23*m31) + (m13*m21*m32) - (m13*m22*m31) - (m12*m21*m33) - (m11*m23*m32);
}

template<class T>
inline T sarrus_3x3(const T* m)
{
 return m[0]*m[4]*m[8] + m[1]*m[5]*m[6] + m[2]*m[3]*m[7] - m[2]*m[4]*m[6] - m[1]*m[3]*m[8] - m[0]*m[5]*m[7];
}

}}

//
// 2D VECTOR FUNCTIONS
// SIMPLE FUNCTIONS
//
namespace cs { namespace math {

template<class T>
inline void vector2D_copy(T* dst, const T* src)
{
 dst[0] = src[0];
 dst[1] = src[1];
}

}}

//
// 2D VECTOR FUNCTIONS
// NORM AND DISTANCE MEASURES
//
namespace cs { namespace math {

template<class T>
inline T vector2D_squared_norm(const T* v)
{
 return v[0]*v[0] + v[1]*v[1];
}

template<class T>
inline T vector2D_norm(const T* v)
{
 return sqrt(vector2D_squared_norm(v));
}

template<class T, class ZeroTest>
inline bool vector2D_normalize(T* v, ZeroTest zt)
{
 T norm = vector2D_norm(v);
 if(zt(norm)) return false;
 T temp = inv(norm);
 v[0] *= temp;
 v[1] *= temp;
 return true;
}

template<class T>
inline bool vector2D_normalize(T* v)
{
 return vector2D_normalize(v, zero_test<T>());
}

template<class T, class ZeroTest>
inline bool vector2D_normalize(T* dst, const T* src, ZeroTest zt)
{
 T norm = vector2D_norm(src);
 if(zt(norm)) return false;
 T temp = inv(norm);
 dst[0] = src[0]*temp;
 dst[1] = src[1]*temp;
 return true;
}

template<class T>
inline bool vector2D_normalize(T* dst, const T* src)
{
 return vector2D_normalize(dst, src, zero_test<T>());
}

template<class T>
inline T vector2D_distance(const T* v1, const T* v2)
{
 T dx = v2[0] - v1[0];
 T dy = v2[1] - v1[1];
 return sqrt(dx*dx + dy*dy);
}

}}

//
// 2D VECTOR FUNCTIONS
// PRODUCTS
//
namespace cs { namespace math {

template<class T>
inline T vector2D_scalar_product(const T* v1, const T* v2)
{
 return v1[0]*v2[0] + v1[1]*v2[1];
}

template<class T>
inline T vector2D_vector_product(const T* v1, const T* v2)
{
 return (v1[0]*v2[1] - v1[1]*v2[0]);
}

template<class T>
inline T vector2D_angle(const T* v1, const T* v2)
{
 T n1 = vector2D_norm(v1);
 T n2 = vector2D_norm(v2);
 return acos(vector2D_scalar_product(v1, v2)/(n1*n2));
}

template<class T>
inline T vector2D_angle(const T* v1, const T* v2, const T& n1, const T& n2)
{
 return acos(vector2D_scalar_product(v1, v2)/(n1*n2));
}

}}


//
// 2D VECTOR FUNCTIONS
// MATHEMATICAL OPERATIONS
//
namespace cs { namespace math {

template<class T>
inline void vector2D_add(T* X, const T* A, const T* B)
{
 X[0] = A[0] + B[0];
 X[1] = A[1] + B[1];
}

template<class T>
inline void vector2D_add(T* X, const T* A)
{
 X[0] += A[0];
 X[1] += A[1];
}

template<class T>
inline void vector2D_add(T* X, const T* A, const T* B, const T& t)
{
 X[0] = A[0] + t*B[0];
 X[1] = A[1] + t*B[1];
}

template<class T>
inline void vector2D_add(T* X, const T* A, const T& t)
{
 X[0] += t*A[0];
 X[1] += t*A[1];
}

template<class T>
inline void vector2D_sub(T* X, const T* A, const T* B)
{
 X[0] = A[0] - B[0];
 X[1] = A[1] - B[1];
}

template<class T>
inline void vector2D_sub(T* X, const T* A)
{
 X[0] -= A[0];
 X[1] -= A[1];
}

template<class T>
inline void vector2D_sub(T* X, const T* A, const T* B, const T& t)
{
 X[0] = A[0] - t*B[0];
 X[1] = A[1] - t*B[1];
}

template<class T>
inline void vector2D_sub(T* X, const T* A, const T& t)
{
 X[0] -= t*A[0];
 X[1] -= t*A[1];
}

template<class T>
inline void vector2D_mul(T* X, const T* A, const T& scalar)
{
 X[0] = scalar*A[0];
 X[1] = scalar*A[1];
}

template<class T>
inline void vector2D_mul(T* X, const T& scalar, const T* A)
{
 X[0] = scalar*A[0];
 X[1] = scalar*A[1];
}

template<class T>
inline void vector2D_mul(T* X, const T& scalar)
{
 X[0] *= scalar;
 X[1] *= scalar;
}

template<class T>
inline void vector2D_div(T* X, const T* A, const T& scalar)
{
 X[0] = A[0]/scalar;
 X[1] = A[1]/scalar;
}

template<class T>
inline void vector2D_div(T* X, const T& scalar)
{
 X[0] /= scalar;
 X[1] /= scalar;
}

}}

//
// 3D VECTOR FUNCTIONS
// SIMPLE FUNCTIONS
//
namespace cs { namespace math {

template<class T>
inline void vector3D_copy(T* dst, const T* src)
{
 dst[0] = src[0];
 dst[1] = src[1];
 dst[2] = src[2];
}

}}

//
// 3D VECTOR FUNCTIONS
// NORM AND DISTANCE MEASURES
//
namespace cs { namespace math {

template<class T>
inline T vector3D_squared_norm(const T* v)
{
 return v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
}

template<class T>
inline T vector3D_norm(const T* v)
{
 return sqrt(vector3D_squared_norm(v));
}

template<class T, class ZeroTest>
inline bool vector3D_normalize(T* v, ZeroTest zt)
{
 T norm = vector3D_norm(v);
 if(zt(norm)) return false;
 T temp = inv(norm);
 v[0] *= temp;
 v[1] *= temp;
 v[2] *= temp;
 return true;
}

template<class T>
inline bool vector3D_normalize(T* v)
{
 return vector3D_normalize(v, zero_test<T>());
}

template<class T, class ZeroTest>
inline bool vector3D_normalize(T* dst, const T* src, ZeroTest zt)
{
 T norm = vector3D_norm(src);
 if(zt(norm)) return false;
 T temp = inv(norm);
 dst[0] = src[0]*temp;
 dst[1] = src[1]*temp;
 dst[2] = src[2]*temp;
 return true;
}

template<class T>
inline bool vector3D_normalize(T* dst, const T* src)
{
 return vector3D_normalize(dst, src, zero_test<T>());
}

template<class T>
inline T vector3D_distance(const T* v1, const T* v2)
{
 T dx = v2[0] - v1[0];
 T dy = v2[1] - v1[1];
 T dz = v2[2] - v1[2];
 return sqrt(dx*dx + dy*dy + dz*dz); 
}

}}

//
// 3D VECTOR FUNCTIONS
// PRODUCTS
//
namespace cs { namespace math {

#define DOT_PRODUCT(u, v) (u[0]*v[0] + u[1]*v[1] + u[2]*v[2])

#define CROSS_PRODUCT(r, u, v)   \
 r[0] = (u[1]*v[2] - u[2]*v[1]); \
 r[1] = (u[2]*v[0] - u[0]*v[2]); \
 r[2] = (u[0]*v[1] - u[1]*v[0]);

template<class T>
inline T vector3D_scalar_product(const T* v1, const T* v2)
{
 return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

template<class T>
inline void vector3D_vector_product(T* result, const T* v1, const T* v2)
{
 result[0] = v1[1]*v2[2] - v1[2]*v2[1];
 result[1] = v1[2]*v2[0] - v1[0]*v2[2];
 result[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

template<class T>
inline void vector3D_vector_product(T* v1, const T* v2)
{
 T temp[3] = {
  v1[1]*v2[2] - v1[2]*v2[1],
  v1[2]*v2[0] - v1[0]*v2[2],
  v1[0]*v2[1] - v1[1]*v2[0]
 };
 v1[0] = temp[0];
 v1[1] = temp[1];
 v1[2] = temp[2];
}

template<class T>
inline T vector3D_angle(const T* v1, const T* v2)
{
 T n1 = vector3D_norm(v1);
 T n2 = vector3D_norm(v2);
 return acos(vector3D_scalar_product(v1, v2)/(n1*n2));
}

template<class T>
inline T vector3D_angle(const T* v1, const T* v2, const T& n1, const T& n2)
{
 return acos(vector3D_scalar_product(v1, v2)/(n1*n2));
}

}}

//
// 3D VECTOR FUNCTIONS
// MATHEMATICAL OPERATIONS
//
namespace cs { namespace math {

template<class T>
inline void vector3D_add(T* X, const T* A, const T* B)
{
 X[0] = A[0] + B[0];
 X[1] = A[1] + B[1];
 X[2] = A[2] + B[2];
}

template<class T>
inline void vector3D_add(T* X, const T* A)
{
 X[0] += A[0];
 X[1] += A[1];
 X[2] += A[2];
}

template<class T>
inline void vector3D_add(T* X, const T* A, const T* B, const T& t)
{
 X[0] = A[0] + t*B[0];
 X[1] = A[1] + t*B[1];
 X[2] = A[2] + t*B[2];
}

template<class T>
inline void vector3D_add(T* X, const T* A, const T& t)
{
 X[0] += t*A[0];
 X[1] += t*A[1];
 X[2] += t*A[2];
}

template<class T>
inline void vector3D_sub(T* X, const T* A, const T* B)
{
 X[0] = A[0] - B[0];
 X[1] = A[1] - B[1];
 X[2] = A[2] - B[2];
}

template<class T>
inline void vector3D_sub(T* X, const T* A)
{
 X[0] -= A[0];
 X[1] -= A[1];
 X[2] -= A[2];
}

template<class T>
inline void vector3D_sub(T* X, const T* A, const T* B, const T& t)
{
 X[0] = A[0] - t*B[0];
 X[1] = A[1] - t*B[1];
 X[2] = A[2] - t*B[2];
}

template<class T>
inline void vector3D_sub(T* X, const T* A, const T& t)
{
 X[0] -= t*A[0];
 X[1] -= t*A[1];
 X[2] -= t*A[2];
}

template<class T>
inline void vector3D_mul(T* X, const T* A, const T& scalar)
{
 X[0] = scalar*A[0];
 X[1] = scalar*A[1];
 X[2] = scalar*A[2];
}

template<class T>
inline void vector3D_mul(T* X, const T& scalar)
{
 X[0] *= scalar;
 X[1] *= scalar;
 X[2] *= scalar;
}

template<class T>
inline void vector3D_div(T* X, const T* A, const T& scalar)
{
 X[0] = A[0]/scalar;
 X[1] = A[1]/scalar;
 X[2] = A[2]/scalar;
}

template<class T>
inline void vector3D_div(T* X, const T& scalar)
{
 X[0] /= scalar;
 X[1] /= scalar;
 X[2] /= scalar;
}

}}

//
// MATRIX FUNCTIONS
// 4x4 MATRICES
//
namespace cs { namespace math {

template<class T>
inline void matrix4x4_copy(T* X, const T* A)
{
 X[0x0] = A[0x0];
 X[0x1] = A[0x1];
 X[0x2] = A[0x2];
 X[0x3] = A[0x3];
 X[0x4] = A[0x4];
 X[0x5] = A[0x5];
 X[0x6] = A[0x6];
 X[0x7] = A[0x7];
 X[0x8] = A[0x8];
 X[0x9] = A[0x9];
 X[0xA] = A[0xA];
 X[0xB] = A[0xB];
 X[0xC] = A[0xC];
 X[0xD] = A[0xD];
 X[0xE] = A[0xE];
 X[0xF] = A[0xF];
}

template<class T>
inline void matrix4x4_transpose(T* X)
{
 T temp;
 temp = X[ 1]; X[ 1] = X[ 4]; X[ 4] = temp;
 temp = X[ 2]; X[ 2] = X[ 8]; X[ 8] = temp;
 temp = X[ 3]; X[ 3] = X[12]; X[12] = temp;
 temp = X[ 6]; X[ 6] = X[ 9]; X[ 9] = temp;
 temp = X[ 7]; X[ 7] = X[13]; X[13] = temp;
 temp = X[11]; X[11] = X[14]; X[14] = temp;
}

template<class T>
inline void matrix4x4_transpose(T* X, const T* A)
{
 X[0x0] = A[0x0]; // row 1
 X[0x1] = A[0x4];
 X[0x2] = A[0x8];
 X[0x3] = A[0xC];
 X[0x4] = A[0x1]; // row 2
 X[0x5] = A[0x5];
 X[0x6] = A[0x9];
 X[0x7] = A[0xD];
 X[0x8] = A[0x2]; // row 3
 X[0x9] = A[0x6];
 X[0xA] = A[0xA];
 X[0xB] = A[0xE];
 X[0xC] = A[0x3]; // row 4
 X[0xD] = A[0x7];
 X[0xE] = A[0xB];
 X[0xF] = A[0xF];
}

template<class T>
inline void matrix4x4_mul(T* X, const T* A, const T* B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x4] + A[0x2]*B[0x8] + A[0x3]*B[0xC];
 X[0x1] = A[0x0]*B[0x1] + A[0x1]*B[0x5] + A[0x2]*B[0x9] + A[0x3]*B[0xD];
 X[0x2] = A[0x0]*B[0x2] + A[0x1]*B[0x6] + A[0x2]*B[0xA] + A[0x3]*B[0xE];
 X[0x3] = A[0x0]*B[0x3] + A[0x1]*B[0x7] + A[0x2]*B[0xB] + A[0x3]*B[0xF];
 X[0x4] = A[0x4]*B[0x0] + A[0x5]*B[0x4] + A[0x6]*B[0x8] + A[0x7]*B[0xC];
 X[0x5] = A[0x4]*B[0x1] + A[0x5]*B[0x5] + A[0x6]*B[0x9] + A[0x7]*B[0xD];
 X[0x6] = A[0x4]*B[0x2] + A[0x5]*B[0x6] + A[0x6]*B[0xA] + A[0x7]*B[0xE];
 X[0x7] = A[0x4]*B[0x3] + A[0x5]*B[0x7] + A[0x6]*B[0xB] + A[0x7]*B[0xF];
 X[0x8] = A[0x8]*B[0x0] + A[0x9]*B[0x4] + A[0xA]*B[0x8] + A[0xB]*B[0xC];
 X[0x9] = A[0x8]*B[0x1] + A[0x9]*B[0x5] + A[0xA]*B[0x9] + A[0xB]*B[0xD];
 X[0xA] = A[0x8]*B[0x2] + A[0x9]*B[0x6] + A[0xA]*B[0xA] + A[0xB]*B[0xE];
 X[0xB] = A[0x8]*B[0x3] + A[0x9]*B[0x7] + A[0xA]*B[0xB] + A[0xB]*B[0xF];
 X[0xC] = A[0xC]*B[0x0] + A[0xD]*B[0x4] + A[0xE]*B[0x8] + A[0xF]*B[0xC];
 X[0xD] = A[0xC]*B[0x1] + A[0xD]*B[0x5] + A[0xE]*B[0x9] + A[0xF]*B[0xD];
 X[0xE] = A[0xC]*B[0x2] + A[0xD]*B[0x6] + A[0xE]*B[0xA] + A[0xF]*B[0xE];
 X[0xF] = A[0xC]*B[0x3] + A[0xD]*B[0x7] + A[0xE]*B[0xB] + A[0xF]*B[0xF];
}

template<class T>
inline void matrix4x4_mul(T* X, const T* A)
{
 T temp[16] = {
  X[0x0]*A[0x0] + X[0x1]*A[0x4] + X[0x2]*A[0x8] + X[0x3]*A[0xC],
  X[0x0]*A[0x1] + X[0x1]*A[0x5] + X[0x2]*A[0x9] + X[0x3]*A[0xD],
  X[0x0]*A[0x2] + X[0x1]*A[0x6] + X[0x2]*A[0xA] + X[0x3]*A[0xE],
  X[0x0]*A[0x3] + X[0x1]*A[0x7] + X[0x2]*A[0xB] + X[0x3]*A[0xF],
  X[0x4]*A[0x0] + X[0x5]*A[0x4] + X[0x6]*A[0x8] + X[0x7]*A[0xC],
  X[0x4]*A[0x1] + X[0x5]*A[0x5] + X[0x6]*A[0x9] + X[0x7]*A[0xD],
  X[0x4]*A[0x2] + X[0x5]*A[0x6] + X[0x6]*A[0xA] + X[0x7]*A[0xE],
  X[0x4]*A[0x3] + X[0x5]*A[0x7] + X[0x6]*A[0xB] + X[0x7]*A[0xF],
  X[0x8]*A[0x0] + X[0x9]*A[0x4] + X[0xA]*A[0x8] + X[0xB]*A[0xC],
  X[0x8]*A[0x1] + X[0x9]*A[0x5] + X[0xA]*A[0x9] + X[0xB]*A[0xD],
  X[0x8]*A[0x2] + X[0x9]*A[0x6] + X[0xA]*A[0xA] + X[0xB]*A[0xE],
  X[0x8]*A[0x3] + X[0x9]*A[0x7] + X[0xA]*A[0xB] + X[0xB]*A[0xF],
  X[0xC]*A[0x0] + X[0xD]*A[0x4] + X[0xE]*A[0x8] + X[0xF]*A[0xC],
  X[0xC]*A[0x1] + X[0xD]*A[0x5] + X[0xE]*A[0x9] + X[0xF]*A[0xD],
  X[0xC]*A[0x2] + X[0xD]*A[0x6] + X[0xE]*A[0xA] + X[0xF]*A[0xE],
  X[0xC]*A[0x3] + X[0xD]*A[0x7] + X[0xE]*A[0xB] + X[0xF]*A[0xF]
 };
 X[0x0] = temp[0x0];
 X[0x1] = temp[0x1];
 X[0x2] = temp[0x2];
 X[0x3] = temp[0x3];
 X[0x4] = temp[0x4];
 X[0x5] = temp[0x5];
 X[0x6] = temp[0x6];
 X[0x7] = temp[0x7];
 X[0x8] = temp[0x8];
 X[0x9] = temp[0x9];
 X[0xA] = temp[0xA];
 X[0xB] = temp[0xB];
 X[0xC] = temp[0xC];
 X[0xD] = temp[0xD];
 X[0xE] = temp[0xE];
 X[0xF] = temp[0xF];
}

template<class T>
inline void matrix4x4_mul_vector2D(T* X, const T* A, const T* B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x1];
 X[0x1] = A[0x4]*B[0x0] + A[0x5]*B[0x1];
 X[0x2] = A[0x8]*B[0x0] + A[0x9]*B[0x1];
}

template<class T>
inline void matrix4x4_mul_vector3D(T* X, const T* A, const T* B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x1] + A[0x2]*B[0x2] + A[0x3]; // w = 1 (implied)
 X[0x1] = A[0x4]*B[0x0] + A[0x5]*B[0x1] + A[0x6]*B[0x2] + A[0x7]; // w = 1 (implied)
 X[0x2] = A[0x8]*B[0x0] + A[0x9]*B[0x1] + A[0xA]*B[0x2] + A[0xB]; // w = 1 (implied)
}

template<class T>
inline void matrix4x4_mul_vector4D(T* X, const T* A, const T* B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x1] + A[0x2]*B[0x2] + A[0x3]*B[0x3];
 X[0x1] = A[0x4]*B[0x0] + A[0x5]*B[0x1] + A[0x6]*B[0x2] + A[0x7]*B[0x3];
 X[0x2] = A[0x8]*B[0x0] + A[0x9]*B[0x1] + A[0xA]*B[0x2] + A[0xB]*B[0x3];
 X[0x3] = A[0xC]*B[0x0] + A[0xD]*B[0x1] + A[0xE]*B[0x2] + A[0xF]*B[0x3];
}

}}

//
// SIMPLE LINEAR EQUATION SOLVERS
//

namespace cs { namespace math {

template<class T>
T cramer_3x3(const T* A, const T* b, T* x)
{
 // compute determinant of 3x3 matrix
 // | m[0] m[1] m[2] | = m[0] * | m[4] m[5] | - m[1] * | m[3] m[5] | + m[2] * | m[3] m[4] |
 // | m[3] m[4] m[5] |          | m[7] m[8] |          | m[6] m[8] |          | m[6] m[7] |
 // | m[6] m[7] m[8] | 

 // X
 // | b[0] m[1] m[2] | = b[0] * | m[4] m[5] | - m[1] * | b[1] m[5] | + m[2] * | b[1] m[4] |
 // | b[1] m[4] m[5] |          | m[7] m[8] |          | b[2] m[8] |          | b[2] m[7] |
 // | b[2] m[7] m[8] | 

 // Y
 // | m[0] b[0] m[2] | = m[0] * | b[1] m[5] | - b[0] * | m[3] m[5] | + m[2] * | m[3] b[1] |
 // | m[3] b[1] m[5] |          | b[2] m[8] |          | m[6] m[8] |          | m[6] b[2] |
 // | m[6] b[2] m[8] |

 // Z
 // | m[0] m[1] b[0] | = m[0] * | m[4] b[1] | - m[1] * | m[3] b[1] | + b[0] * | m[3] m[4] |
 // | m[3] m[4] b[1] |          | m[7] b[2] |          | m[6] b[2] |          | m[6] m[7] |
 // | m[6] m[7] b[2] | 

 // cache determinants
 T D[6] = {
  sarrus_2x2(A[4], A[5], A[7], A[8]), // D[0]
  sarrus_2x2(A[3], A[5], A[6], A[8]), // D[1]
  sarrus_2x2(A[3], A[4], A[6], A[7]), // D[2]
  sarrus_2x2(b[1], A[3], b[2], A[6]), // D[3]
  sarrus_2x2(b[1], A[4], b[2], A[7]), // D[4]
  sarrus_2x2(b[1], A[5], b[2], A[8]), // D[5]
 };

 // compute divisor (using first row)
 T det = A[0]*D[0] - A[1]*D[1] + A[2]*D[2];
 if(det == constants<T>::zero()) return det;

 // solve
 x[0] = (b[0]*D[0] - A[1]*D[5] + A[2]*D[4])/det;
 x[1] = (A[0]*D[5] - b[0]*D[1] - A[2]*D[3])/det;
 x[2] = (A[1]*D[3] - A[0]*D[4] + b[0]*D[2])/det;

 // return determinant
 return det;
}

}}

//
// TRIANGLE FUNCTIONS
// AREA
// TODO: MOVE TO "AREA"
//
namespace cs { namespace math {
    
// given base and height
template<class T>
inline T triangle_area(T h, T b)
{
 return halve(h * b);
}

// given lengths (Heron's Formula)
template<class T>
inline T triangle_area(T a, T b, T c)
{
 T s = halve(a + b + c);
 return sqrt(s * (s - a) * (s - b) * (s - c));
}

// given points in 2D space
template<class T>
inline T triangle_area_2D(const T* a, const T* b, const T* c)
{
 return halve(a[0]*(b[1] - c[1]) - b[0]*(a[1] - c[1]) + c[0]*(a[1] - b[1]));
}

// given points in 3D space
template<class T>
inline T triangle_area_3D(const T* a, const T* b, const T* c)
{
 T s1[3] = { c[0] - a[0], c[1] - a[1], c[2] - a[2] };
 T s2[3] = { c[0] - b[0], c[1] - b[1], c[2] - b[2] };
 T s3[3];
 CROSS_PRODUCT(s3, s1, s2);
 return halve(norm3D(s3));
}

}}

#endif
