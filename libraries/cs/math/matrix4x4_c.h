#ifndef __CS_MATH_GENERAL_MATRIX4X4_C__
#define __CS_MATH_GENERAL_MATRIX4X4_C__

// Standard C++ Headers
#include<cmath>

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

template<class _RanIt>
inline void matrix4x4_identity(_RanIt X)
{
 typedef typename std::iterator_traits<_RanIt>::value_type value_type;
 value_type one = static_cast<value_type>(1.0);
 value_type zer = static_cast<value_type>(0.0);
 X[0x0] = one;
 X[0x1] = zer;
 X[0x2] = zer;
 X[0x3] = zer;
 X[0x4] = zer;
 X[0x5] = one;
 X[0x6] = zer;
 X[0x7] = zer;
 X[0x8] = zer;
 X[0x9] = zer;
 X[0xA] = one;
 X[0xB] = zer;
 X[0xC] = zer;
 X[0xD] = zer;
 X[0xE] = zer;
 X[0xF] = one;
}

template<class _RanIt>
inline void matrix4x4_transpose(_RanIt X, _RanIt A)
{
 X[0x0] = A[0x0];
 X[0x1] = A[0x4];
 X[0x2] = A[0x8];
 X[0x3] = A[0xC];
 X[0x4] = A[0x1];
 X[0x5] = A[0x5];
 X[0x6] = A[0x9];
 X[0x7] = A[0xD];
 X[0x8] = A[0x2];
 X[0x9] = A[0x6];
 X[0xA] = A[0xA];
 X[0xB] = A[0xE];
 X[0xC] = A[0x3];
 X[0xD] = A[0x7];
 X[0xE] = A[0xB];
 X[0xF] = A[0xF];
}

template<class _RanIt>
inline void matrix4x4_transpose(_RanIt A)
{
 typedef typename std::iterator_traits<_RanIt>::value_type value_type;
 value_type temp;
 temp = A[0x1]; A[0x1] = A[0x4]; A[0x4] = temp;
 temp = A[0x2]; A[0x2] = A[0x8]; A[0x8] = temp;
 temp = A[0x3]; A[0x3] = A[0xC]; A[0xC] = temp;
 temp = A[0x6]; A[0x6] = A[0x9]; A[0x9] = temp;
 temp = A[0x7]; A[0x7] = A[0xD]; A[0xD] = temp;
 temp = A[0xB]; A[0xB] = A[0xE]; A[0xE] = temp;
}

template<class _RanIt>
inline void matrix4x4_add(_RanIt X, _RanIt A, _RanIt B)
{
 X[0x0] = A[0x0] + B[0x0];
 X[0x1] = A[0x1] + B[0x1];
 X[0x2] = A[0x2] + B[0x2];
 X[0x3] = A[0x3] + B[0x3];
 X[0x4] = A[0x4] + B[0x4];
 X[0x5] = A[0x5] + B[0x5];
 X[0x6] = A[0x6] + B[0x6];
 X[0x7] = A[0x7] + B[0x7];
 X[0x8] = A[0x8] + B[0x8];
 X[0x9] = A[0x9] + B[0x9];
 X[0xA] = A[0xA] + B[0xA];
 X[0xB] = A[0xB] + B[0xB];
 X[0xC] = A[0xC] + B[0xC];
 X[0xD] = A[0xD] + B[0xD];
 X[0xE] = A[0xE] + B[0xE];
 X[0xF] = A[0xF] + B[0xF];
}

template<class _RanIt>
inline void matrix4x4_add(_RanIt A, _RanIt B)
{
 A[0x0] += B[0x0];
 A[0x1] += B[0x1];
 A[0x2] += B[0x2];
 A[0x3] += B[0x3];
 A[0x4] += B[0x4];
 A[0x5] += B[0x5];
 A[0x6] += B[0x6];
 A[0x7] += B[0x7];
 A[0x8] += B[0x8];
 A[0x9] += B[0x9];
 A[0xA] += B[0xA];
 A[0xB] += B[0xB];
 A[0xC] += B[0xC];
 A[0xD] += B[0xD];
 A[0xE] += B[0xE];
 A[0xF] += B[0xF];
}

template<class _RanIt>
inline void matrix4x4_sub(_RanIt X, _RanIt A, _RanIt B)
{
 X[0x0] = A[0x0] - B[0x0];
 X[0x1] = A[0x1] - B[0x1];
 X[0x2] = A[0x2] - B[0x2];
 X[0x3] = A[0x3] - B[0x3];
 X[0x4] = A[0x4] - B[0x4];
 X[0x5] = A[0x5] - B[0x5];
 X[0x6] = A[0x6] - B[0x6];
 X[0x7] = A[0x7] - B[0x7];
 X[0x8] = A[0x8] - B[0x8];
 X[0x9] = A[0x9] - B[0x9];
 X[0xA] = A[0xA] - B[0xA];
 X[0xB] = A[0xB] - B[0xB];
 X[0xC] = A[0xC] - B[0xC];
 X[0xD] = A[0xD] - B[0xD];
 X[0xE] = A[0xE] - B[0xE];
 X[0xF] = A[0xF] - B[0xF];
}

template<class _RanIt>
inline void matrix4x4_sub(_RanIt A, _RanIt B)
{
 A[0x0] -= B[0x0];
 A[0x1] -= B[0x1];
 A[0x2] -= B[0x2];
 A[0x3] -= B[0x3];
 A[0x4] -= B[0x4];
 A[0x5] -= B[0x5];
 A[0x6] -= B[0x6];
 A[0x7] -= B[0x7];
 A[0x8] -= B[0x8];
 A[0x9] -= B[0x9];
 A[0xA] -= B[0xA];
 A[0xB] -= B[0xB];
 A[0xC] -= B[0xC];
 A[0xD] -= B[0xD];
 A[0xE] -= B[0xE];
 A[0xF] -= B[0xF];
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
inline void matrix4x4_mul(T* A, const T* B)
{
 T X[16] = {
  A[0x0]*B[0x0] + A[0x1]*B[0x4] + A[0x2]*B[0x8] + A[0x3]*B[0xC],
  A[0x0]*B[0x1] + A[0x1]*B[0x5] + A[0x2]*B[0x9] + A[0x3]*B[0xD],
  A[0x0]*B[0x2] + A[0x1]*B[0x6] + A[0x2]*B[0xA] + A[0x3]*B[0xE],
  A[0x0]*B[0x3] + A[0x1]*B[0x7] + A[0x2]*B[0xB] + A[0x3]*B[0xF],
  A[0x4]*B[0x0] + A[0x5]*B[0x4] + A[0x6]*B[0x8] + A[0x7]*B[0xC],
  A[0x4]*B[0x1] + A[0x5]*B[0x5] + A[0x6]*B[0x9] + A[0x7]*B[0xD],
  A[0x4]*B[0x2] + A[0x5]*B[0x6] + A[0x6]*B[0xA] + A[0x7]*B[0xE],
  A[0x4]*B[0x3] + A[0x5]*B[0x7] + A[0x6]*B[0xB] + A[0x7]*B[0xF],
  A[0x8]*B[0x0] + A[0x9]*B[0x4] + A[0xA]*B[0x8] + A[0xB]*B[0xC],
  A[0x8]*B[0x1] + A[0x9]*B[0x5] + A[0xA]*B[0x9] + A[0xB]*B[0xD],
  A[0x8]*B[0x2] + A[0x9]*B[0x6] + A[0xA]*B[0xA] + A[0xB]*B[0xE],
  A[0x8]*B[0x3] + A[0x9]*B[0x7] + A[0xA]*B[0xB] + A[0xB]*B[0xF],
  A[0xC]*B[0x0] + A[0xD]*B[0x4] + A[0xE]*B[0x8] + A[0xF]*B[0xC],
  A[0xC]*B[0x1] + A[0xD]*B[0x5] + A[0xE]*B[0x9] + A[0xF]*B[0xD],
  A[0xC]*B[0x2] + A[0xD]*B[0x6] + A[0xE]*B[0xA] + A[0xF]*B[0xE],
  A[0xC]*B[0x3] + A[0xD]*B[0x7] + A[0xE]*B[0xB] + A[0xF]*B[0xF]
 };
 A[0x0] = X[0x0];
 A[0x1] = X[0x1];
 A[0x2] = X[0x2];
 A[0x3] = X[0x3];
 A[0x4] = X[0x4];
 A[0x5] = X[0x5];
 A[0x6] = X[0x6];
 A[0x7] = X[0x7];
 A[0x8] = X[0x8];
 A[0x9] = X[0x9];
 A[0xA] = X[0xA];
 A[0xB] = X[0xB];
 A[0xC] = X[0xC];
 A[0xD] = X[0xD];
 A[0xE] = X[0xE];
 A[0xF] = X[0xF];
}

template<class T>
inline void matrix4x4_vector3_mul(T* X, const T* A, const T* B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x1] + A[0x2]*B[0x2];
 X[0x1] = A[0x4]*B[0x0] + A[0x5]*B[0x1] + A[0x6]*B[0x2];
 X[0x2] = A[0x8]*B[0x0] + A[0x9]*B[0x1] + A[0xA]*B[0x2];
}

template<class _MatIt, class _VecIt>
inline void matrix4x4_vector3_mul(_VecIt X, _MatIt A, _VecIt B)
{
 typedef typename std::iterator_traits<_VecIt>::value_type value_type;
 X[0x0] = static_cast<_VecIt>(A[0x0]*B[0x0] + A[0x1]*B[0x1] + A[0x2]*B[0x2]);
 X[0x1] = static_cast<_VecIt>(A[0x4]*B[0x0] + A[0x5]*B[0x1] + A[0x6]*B[0x2]);
 X[0x2] = static_cast<_VecIt>(A[0x8]*B[0x0] + A[0x9]*B[0x1] + A[0xA]*B[0x2]);
}

template<class _RanIt>
inline void matrix4x4_vector4_mul(_RanIt X, _RanIt A, _RanIt B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x1] + A[0x2]*B[0x2] + A[0x3]*B[0x3];
 X[0x1] = A[0x4]*B[0x0] + A[0x5]*B[0x1] + A[0x6]*B[0x2] + A[0x7]*B[0x3];
 X[0x2] = A[0x8]*B[0x0] + A[0x9]*B[0x1] + A[0xA]*B[0x2] + A[0xB]*B[0x3];
 X[0x3] = A[0xC]*B[0x0] + A[0xD]*B[0x1] + A[0xE]*B[0x2] + A[0xF]*B[0x3];
}

template<class _MatIt, class _VecIt>
inline void matrix4x4_vector4_mul(_VecIt X, _MatIt A, _VecIt B)
{
 typedef typename std::iterator_traits<_VecIt>::value_type value_type;
 X[0x0] = static_cast<_VecIt>(A[0x0]*B[0x0] + A[0x1]*B[0x1] + A[0x2]*B[0x2] + A[0x3]*B[0x3]);
 X[0x1] = static_cast<_VecIt>(A[0x4]*B[0x0] + A[0x5]*B[0x1] + A[0x6]*B[0x2] + A[0x7]*B[0x3]);
 X[0x2] = static_cast<_VecIt>(A[0x8]*B[0x0] + A[0x9]*B[0x1] + A[0xA]*B[0x2] + A[0xB]*B[0x3]);
 X[0x3] = static_cast<_VecIt>(A[0xC]*B[0x0] + A[0xD]*B[0x1] + A[0xE]*B[0x2] + A[0xF]*B[0x3]);
}

}};

#endif