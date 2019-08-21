#ifndef __CS_MATH_ROTATION__
#define __CS_MATH_ROTATION__

// CS Headers
#include<cs/math/vector2D.h>
#include<cs/math/vector3D.h>
#include<cs/math/quaternion.h>

//
// ROTATION QUATERNIONS
//
namespace cs { namespace math {

template<class T>
inline void rotation_quaternion(T* q, const T* v, const T& angle)
{
 // cached sine and cosine
 typedef T value_type;
 value_type half_angle = angle/constants<T>::two();
 value_type cv = cos(half_angle);
 value_type sv = sin(half_angle);

 // initialize quaternion
 q[0] = cv;
 q[1] = sv * v[0];
 q[2] = sv * v[1];
 q[3] = sv * v[2];
}

template<class T>
inline void rotation_quaternion_X(T* q, const T& angle)
{
 const T half_angle = angle/constants<T>::two();
 q[0] = cos(half_angle);
 q[1] = sin(half_angle);
 q[2] = static_cast<T>(0);
 q[3] = static_cast<T>(0);
}

template<class T>
inline void rotation_quaternion_Y(T* q, const T& angle)
{
 const T half_angle = angle/constants<T>::two();
 q[0] = cos(half_angle);
 q[1] = static_cast<T>(0);
 q[2] = sin(half_angle);
 q[3] = static_cast<T>(0);
}

template<class T>
inline void rotation_quaternion_Z(T* q, const T& angle)
{
 T half_angle = angle/constants<T>::two();
 Q[0] = cos(half_angle);
 Q[1] = static_cast<T>(0);
 Q[2] = static_cast<T>(0);
 Q[3] = sin(half_angle);
}

template<class T>
inline void rotation_quaternion_XYX(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1/static_cast<T>(2));
 T c2 = cos(r2/static_cast<T>(2));
 T c3 = cos(r3/static_cast<T>(2));

 // cached sines
 T s1 = sin(r1/static_cast<T>(2));
 T s2 = sin(r2/static_cast<T>(2));
 T s3 = sin(r3/static_cast<T>(2));

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 - c2_s1*s3);
 q[1] = (c2_s1*c3 + c1_c2*s3);
 q[2] = (c1_s2*c3 + s1_s2*s3);
 q[3] = (s1_s2*c3 - c1_s2*s3);
}

template<class T>
inline void rotation_quaternion_XYZ(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1/static_cast<T>(2));
 T c2 = cos(r2/static_cast<T>(2));
 T c3 = cos(r3/static_cast<T>(2));

 // cached sines
 T s1 = sin(r1/static_cast<T>(2));
 T s2 = sin(r2/static_cast<T>(2));
 T s3 = sin(r3/static_cast<T>(2));

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 - s1_s2*s3);
 q[1] = (c2_s1*c3 + c1_s2*s3);
 q[2] = (c1_s2*c3 - c2_s1*s3);
 q[3] = (s1_s2*c3 + c1_c2*s3);
}

template<class T>
inline void rotation_quaternion_XZX(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1/static_cast<T>(2));
 T c2 = cos(r2/static_cast<T>(2));
 T c3 = cos(r3/static_cast<T>(2));

 // cached sines
 T s1 = sin(r1/static_cast<T>(2));
 T s2 = sin(r2/static_cast<T>(2));
 T s3 = sin(r3/static_cast<T>(2));

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 - c2_s1*s3);
 q[1] = (c2_s1*c3 + c1_c2*s3);
 q[2] = (c1_s2*s3 - s1_s2*c3);
 q[3] = (c1_s2*c3 + s1_s2*s3);
}

template<class T>
inline void rotation_quaternion_XZY(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1/static_cast<T>(2));
 T c2 = cos(r2/static_cast<T>(2));
 T c3 = cos(r3/static_cast<T>(2));

 // cached sines
 T s1 = sin(r1/static_cast<T>(2));
 T s2 = sin(r2/static_cast<T>(2));
 T s3 = sin(r3/static_cast<T>(2));

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 + s1_s2*s3);
 q[1] = (c2_s1*c3 - c1_s2*s3);
 q[2] = (c1_c2*s3 - s1_s2*c3);
 q[3] = (c1_s2*c3 + c2_s1*s3);
}

template<class T>
inline void rotation_quaternion_YXY(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1)/static_cast<T>(2);
 T c2 = cos(r2)/static_cast<T>(2);
 T c3 = cos(r3)/static_cast<T>(2);

 // cached sines
 T s1 = sin(r1)/static_cast<T>(2);
 T s2 = sin(r2)/static_cast<T>(2);
 T s3 = sin(r3)/static_cast<T>(2);

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 - c2_s1*s3);
 q[1] = (c1_s2*c3 + s1_s2*s3);
 q[2] = (c2_s1*c3 + c1_c2*s3);
 q[3] = (c1_s2*s3 - s1_s2*c3);
}

template<class T>
inline void rotation_quaternion_YXZ(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1)/static_cast<T>(2);
 T c2 = cos(r2)/static_cast<T>(2);
 T c3 = cos(r3)/static_cast<T>(2);

 // cached sines
 T s1 = sin(r1)/static_cast<T>(2);
 T s2 = sin(r2)/static_cast<T>(2);
 T s3 = sin(r3)/static_cast<T>(2);

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 + s1_s2*s3);
 q[1] = (c1_s2*c3 + c2_s1*s3);
 q[2] = (c2_s1*c3 - c1_s2*s3);
 q[3] = (c1_c2*s3 - s1_s2*c3);
}

template<class T>
inline void rotation_quaternion_YZX(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1)/static_cast<T>(2);
 T c2 = cos(r2)/static_cast<T>(2);
 T c3 = cos(r3)/static_cast<T>(2);

 // cached sines
 T s1 = sin(r1)/static_cast<T>(2);
 T s2 = sin(r2)/static_cast<T>(2);
 T s3 = sin(r3)/static_cast<T>(2);

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 - s1_s2*s3);
 q[1] = (s1_s2*c3 + c1_c2*s3);
 q[2] = (c2_s1*c3 + c1_s2*s3);
 q[3] = (c1_s2*c3 - c2_s1*s3);
}

template<class T>
inline void rotation_quaternion_YZY(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1)/static_cast<T>(2);
 T c2 = cos(r2)/static_cast<T>(2);
 T c3 = cos(r3)/static_cast<T>(2);

 // cached sines
 T s1 = sin(r1)/static_cast<T>(2);
 T s2 = sin(r2)/static_cast<T>(2);
 T s3 = sin(r3)/static_cast<T>(2);

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 - c2_s1*s3);
 q[1] = (s1_s2*c3 - c1_s2*s3);
 q[2] = (c2_s1*c3 + c1_c2*s3);
 q[3] = (c1_s2*c3 + s1_s2*s3);
}

template<class T>
inline void rotation_quaternion_ZXY(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1)/static_cast<T>(2);
 T c2 = cos(r2)/static_cast<T>(2);
 T c3 = cos(r3)/static_cast<T>(2);

 // cached sines
 T s1 = sin(r1)/static_cast<T>(2);
 T s2 = sin(r2)/static_cast<T>(2);
 T s3 = sin(r3)/static_cast<T>(2);

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 - s1_s2*s3);
 q[1] = (c1_s2*c3 - c2_s1*s3);
 q[2] = (s1_s2*c3 + c1_c2*s3);
 q[3] = (c2_s1*c3 + c1_s2*s3);
}

template<class T>
inline void rotation_quaternion_ZXZ(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1)/static_cast<T>(2);
 T c2 = cos(r2)/static_cast<T>(2);
 T c3 = cos(r3)/static_cast<T>(2);

 // cached sines
 T s1 = sin(r1)/static_cast<T>(2);
 T s2 = sin(r2)/static_cast<T>(2);
 T s3 = sin(r3)/static_cast<T>(2);

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 - c2_s1*s3);
 q[1] = (c1_s2*c3 + s1_s2*s3);
 q[2] = (s1_s2*c3 - c1_s2*s3);
 q[3] = (c2_s1*c3 + c1_c2*s3);
}

template<class T>
inline void rotation_quaternion_ZYX(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1)/static_cast<T>(2);
 T c2 = cos(r2)/static_cast<T>(2);
 T c3 = cos(r3)/static_cast<T>(2);

 // cached sines
 T s1 = sin(r1)/static_cast<T>(2);
 T s2 = sin(r2)/static_cast<T>(2);
 T s3 = sin(r3)/static_cast<T>(2);

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 + s1_s2*s3);
 q[1] = (c1_c2*s3 - s1_s2*c3);
 q[2] = (c1_s2*c3 + c2_s1*s3);
 q[3] = (c2_s1*c3 - c1_s2*s3);
}

template<class T>
inline void rotation_quaternion_ZYZ(T* q, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1)/static_cast<T>(2);
 T c2 = cos(r2)/static_cast<T>(2);
 T c3 = cos(r3)/static_cast<T>(2);

 // cached sines
 T s1 = sin(r1)/static_cast<T>(2);
 T s2 = sin(r2)/static_cast<T>(2);
 T s3 = sin(r3)/static_cast<T>(2);

 // composite values
 T c1_c2 = c1 * c2;
 T s1_s2 = s1 * s2;
 T c1_s2 = c1 * s2;
 T c2_s1 = c2 * s1;

 // initialize quaternion
 q[0] = (c1_c2*c3 - c2_s1*s3);
 q[1] = (c1_s2*s3 - s1_s2*c3);
 q[2] = (c1_s2*c3 + s1_s2*s3);
 q[3] = (c2_s1*c3 + c1_c2*s3);
}

}}

//
// ROTATION
//
namespace cs { namespace math {

template<class T>
inline void quaternion_rotate(T* result, const T* v, const T* axis, const T& angle)
{
 // cached values
 typedef T value_type;
 const value_type half_angle = angle/constants<T>::two();
 const value_type cv = cos(half_angle);
 const value_type sv = sin(half_angle);

 // cached values
 const value_type q1[4] = { cv, sv*axis[0], sv*axis[1], sv*axis[2] };
 const value_type t1[4] = {
 -q1[1]*v[0] - q1[2]*v[1] - q1[3]*v[2],
  q1[0]*v[0] + q1[2]*v[2] - q1[3]*v[1],
  q1[0]*v[1] - q1[1]*v[2] + q1[3]*v[0],
  q1[0]*v[2] + q1[1]*v[1] - q1[2]*v[0]
 };

 // compute result
 result[0] = -t1[0]*q1[1] + t1[1]*q1[0] - t1[2]*q1[3] + t1[3]*q1[2];
 result[1] = -t1[0]*q1[2] + t1[1]*q1[3] + t1[2]*q1[0] - t1[3]*q1[1];
 result[2] = -t1[0]*q1[3] - t1[1]*q1[2] + t1[2]*q1[1] + t1[3]*q1[0];
}

template<class T>
inline void quaternion_rotate(T* result, const T* v, const T* origin, const T* axis, const T& angle)
{
 // cached values
 typedef T value_type;
 const value_type half_angle = angle/constants<T>::two();
 const value_type cv = cos(half_angle);
 const value_type sv = sin(half_angle);

 // cached values
 const value_type v1[3] = { v[0] - origin[0], v[1] - origin[1], v[2] - origin[2] };
 const value_type q1[4] = { cv, sv*axis[0], sv*axis[1], sv*axis[2] };
 const value_type t1[4] = {
 -q1[1]*v1[0] - q1[2]*v1[1] - q1[3]*v1[2],
  q1[0]*v1[0] + q1[2]*v1[2] - q1[3]*v1[1],
  q1[0]*v1[1] - q1[1]*v1[2] + q1[3]*v1[0],
  q1[0]*v1[2] + q1[1]*v1[1] - q1[2]*v1[0]
 };

 // compute result
 result[0] = -t1[0]*q1[1] + t1[1]*q1[0] - t1[2]*q1[3] + t1[3]*q1[2] + origin[0];
 result[1] = -t1[0]*q1[2] + t1[1]*q1[3] + t1[2]*q1[0] - t1[3]*q1[1] + origin[1];
 result[2] = -t1[0]*q1[3] - t1[1]*q1[2] + t1[2]*q1[1] + t1[3]*q1[0] + origin[2];
}

}}

//
// ROTATION MATRICES
//
namespace cs { namespace math {

template<class T>
inline void rotation_matrix2x2(T* m, const T& r)
{
 // cached values
 T cr = cos(r);
 T sr = sin(r);

 // matrix
 m[0x0] =  cr;
 m[0x1] = -sr;
 m[0x2] =  sr;
 m[0x3] =  cr;
}

template<class T>
inline void rotation_matrix3x3_X(T* m, const T& r)
{
 // cached values
 T cr = cos(r);
 T sr = sin(r);

 // matrix
 m[0x0] = static_cast<T>(1);
 m[0x1] = static_cast<T>(0);
 m[0x2] = static_cast<T>(0);
 m[0x3] = static_cast<T>(0);
 m[0x4] =  cr;
 m[0x5] = -sr;
 m[0x6] = static_cast<T>(0);
 m[0x7] =  sr;
 m[0x8] =  cr;
}

template<class T>
inline void rotation_matrix3x3_Y(T* m, const T& r)
{
 // cached values
 T cr = cos(r);
 T sr = sin(r);

 // matrix
 m[0x0] =  cr;
 m[0x1] = static_cast<T>(0);
 m[0x2] = sr;
 m[0x3] = static_cast<T>(0);
 m[0x4] = static_cast<T>(1);
 m[0x5] = static_cast<T>(0);
 m[0x6] = -sr;
 m[0x7] = static_cast<T>(0);
 m[0x8] =  cr;
}

template<class T>
inline void rotation_matrix3x3_Z(T* m, const T& r)
{
 // cached values
 T cr = cos(r);
 T sr = sin(r);

 // matrix
 m[0x0] =  cr;
 m[0x1] = -sr;
 m[0x2] = static_cast<T>(0);
 m[0x3] =  sr;
 m[0x4] =  cr;
 m[0x5] = static_cast<T>(0);
 m[0x6] = static_cast<T>(0);
 m[0x7] = static_cast<T>(0);
 m[0x8] = static_cast<T>(1);
}

template<class T>
inline void rotation_matrix3x3_XYX(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] =  c2;
 m[0x1] =  (s2 * s3);
 m[0x2] =  (c3 * s2);
 m[0x3] =  (s1 * s2);
 m[0x4] =  (c1 * c3) - (s1 * c2_s3);
 m[0x5] = -(c1 * s3) - (s1 * c2_c3);
 m[0x6] = -(c2 * s2);
 m[0x7] =  (c3 * s1) + (c1 * c2_s3);
 m[0x8] = -(s1 * s3) + (c1 * c2_c3);
}

template<class T>
inline void rotation_matrix3x3_XYZ(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c2 * c3);
 m[0x1] = -(c2 * s3);
 m[0x2] =  s2;
 m[0x3] =  (c1 * s3) + (s1 * c3_s2);
 m[0x4] =  (c1 * c3) - (s1 * s2_s3);
 m[0x5] = -(c2 * s1);
 m[0x6] =  (s1 * s3) - (c1 * c3_s2);
 m[0x7] =  (c3 * s1) + (c1 * s2_s3);
 m[0x8] =  (c1 * c2);
}

template<class T>
inline void rotation_matrix3x3_XZX(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] =  c2;
 m[0x1] = -(c3 * s2);
 m[0x2] =  (s2 * s3);
 m[0x3] =  c1*s2;
 m[0x4] = -(s1 * s3) + (c1 * c2_c3);
 m[0x5] = -(c3 * s1) - (c1 * c2_s3);
 m[0x6] =  (s1 * s2);
 m[0x7] =  (c1 * s3) + (s1 * c2_c3);
 m[0x8] =  (c1 * c3) - (s1 * c2_s3);
}

template<class T>
inline void rotation_matrix3x3_XZY(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c2 * c3);
 m[0x1] = -s2;
 m[0x2] =  (c2 * s3);
 m[0x3] =  (s1 * s3) + (c1 * c3_s2);
 m[0x4] =  (c1 * c2);
 m[0x5] = -(c3 * s1) + (c1 * s2_s3);
 m[0x6] = -(c1 * s3) + (s1 * c3_s2);
 m[0x7] =  (c2 * s1);
 m[0x8] =  (c1 * c3) + (s1 * s2_s3);
}

template<class T>
inline void rotation_matrix3x3_YXY(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] =  (c1 * c3) - (s1 * c2_s3);
 m[0x1] =  (s1 * s2);
 m[0x2] =  (c1 * s3) + (s1 * c2_c3);
 m[0x3] =  (s2 * s3);
 m[0x4] =  c2;
 m[0x5] = -(c3 * s2);
 m[0x6] = -(c3 * s1) - (c1 * c2_s3);
 m[0x7] =  (c1 * c2);
 m[0x8] = -(s1 * s3) + (c1 * c2_c3);
}

template<class T>
inline void rotation_matrix3x3_YXZ(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] = +(c1 * c3)(s1 * s2_s3);
 m[0x1] = -(c1 * s3)(s1 * c3_s2);
 m[0x2] =  (c2 * s1);
 m[0x3] =  (c2 * s3);
 m[0x4] =  (c2 * c3);
 m[0x5] = -s2;
 m[0x6] = -(c3 * s1) + (c1 * s2_s3);
 m[0x7] =  (s1 * s3) + (c1 * c3_s2);
 m[0x8] =  (c1 * c2);
}

template<class T>
inline void rotation_matrix3x3_YZX(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c1 * c2);
 m[0x1] =  (s1 * s3) - (c1 * c3_s2);
 m[0x2] =  (c3 * s1) + (c1 * s2_s3);
 m[0x3] =  s2;
 m[0x4] =  (c2 * c3);
 m[0x5] = -(c2 * s3);
 m[0x6] = -(c2 * s1);
 m[0x7] =  (c1 * s3) + (s1 * c3_s2);
 m[0x8] =  (c1 * c3) - (s1 * s2_s3);
}

template<class T>
inline void rotation_matrix3x3_YZY(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] = -(s1 * s3) + (c1 * c2_c3);
 m[0x1] = -(c1 * s2);
 m[0x2] =  (c3 * s1) + (c1 * c2_s3);
 m[0x3] =  (c3 * s2);
 m[0x4] =  c2;
 m[0x5] =  (s2 * s3);
 m[0x6] = -(c1 * s3) - (s1 * c2_c3);
 m[0x7] =  (s1 * s2);
 m[0x8] =  (c1 * c3) - (s1 * c2_s3);
}

template<class T>
inline void rotation_matrix3x3_ZXY(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c1 * c3) - (s1 * s2_s3);
 m[0x1] = -(c2 * s1);
 m[0x2] =  (c1 * s3) + (s1 * c3_s2);
 m[0x3] =  (c3 * s1) + (c1 * s2_s3);
 m[0x4] =  (c1 * c2);
 m[0x5] =  (s1 * s3) - (c1 * c3_s2);
 m[0x6] = -(c2 * s3);
 m[0x7] =  s2;
 m[0x8] =  (c2 * c3);
}

template<class T>
inline void rotation_matrix3x3_ZXZ(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] =  (c1 * c3) - (s1 * c2_s3);
 m[0x1] = -(c1 * s3) - (s1 * c2_c3);
 m[0x2] =  (s1 * s2);
 m[0x3] =  (c3 * s1) + (c1 * c2_s3);
 m[0x4] = -(s1 * s3) + (c1 * c2_c3);
 m[0x5] = -(c1 * s2);
 m[0x6] =  (s2 * s3);
 m[0x7] =  (c3 * s2);
 m[0x8] =  c2;
}

template<class T>
inline void rotation_matrix3x3_ZYX(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c1 * c2);
 m[0x1] = -(c3 * s1) + (c1 * s2_s3);
 m[0x2] =  (s1 * s3) + (c1 * c3_s2);
 m[0x3] =  (c2 * s1);
 m[0x4] =  (c1 * c3) + (s1 * s2_s3);
 m[0x5] = -(c1 * s3) + (s1 * c3_s2);
 m[0x6] = -s2;
 m[0x7] =  (c2 * s3);
 m[0x8] =  (c2 * c3);
}

template<class T>
inline void rotation_matrix3x3_ZYZ(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] = -(s1 * s3) + (c1 * c2_c3);
 m[0x1] = -(c3 * s1) - (c1 * c2_s3);
 m[0x2] =  (c1 * s2);
 m[0x3] =  (c1 * s3) + (s1 * c2_c3);
 m[0x4] =  (c1 * c3) - (s1 * c2_s3);
 m[0x5] =  (s1 * s2);
 m[0x6] = -(c3 * s2);
 m[0x7] =  (s2 * s3);
 m[0x8] =  c2;
}

}}

//
// 4x4 MATRIX FUNCTIONS
//
namespace cs { namespace math {

template<class T>
inline void rotation_matrix4x4_X(T* m, const T& r)
{
 // cached values
 T cr = cos(r);
 T sr = sin(r);

 // matrix
 m[0x0] = one;
 m[0x1] = static_cast<T>(0);
 m[0x2] = static_cast<T>(0);
 m[0x3] = static_cast<T>(0);
 m[0x4] = static_cast<T>(0);
 m[0x5] =  cr;
 m[0x6] = -sr;
 m[0x7] = static_cast<T>(0);
 m[0x8] = static_cast<T>(0);
 m[0x9] =  sr;
 m[0xA] =  cr;
 m[0xB] = static_cast<T>(0);
 m[0xC] = static_cast<T>(0);
 m[0xD] = static_cast<T>(0);
 m[0xE] = static_cast<T>(0);
 m[0xF] = static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_Y(T* m, const T& r)
{
 // cached values
 T cr = cos(r);
 T sr = sin(r);

 // matrix
 m[0x0] =  cr;
 m[0x1] = static_cast<T>(0);
 m[0x2] = sr;
 m[0x3] = static_cast<T>(0);
 m[0x4] = static_cast<T>(0);
 m[0x5] = static_cast<T>(1);
 m[0x6] = static_cast<T>(0);
 m[0x7] = static_cast<T>(0);
 m[0x8] = -sr;
 m[0x9] = static_cast<T>(0);
 m[0xA] =  cr;
 m[0xB] = static_cast<T>(0);
 m[0xC] = static_cast<T>(0);
 m[0xD] = static_cast<T>(0);
 m[0xE] = static_cast<T>(0);
 m[0xF] = static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_Z(T* m, const T& r)
{
 // cached values
 T cr = cos(r);
 T sr = sin(r);

 // matrix
 m[0x0] =  cr;
 m[0x1] = -sr;
 m[0x2] = static_cast<T>(0);
 m[0x3] = static_cast<T>(0);
 m[0x4] =  sr;
 m[0x5] =  cr;
 m[0x6] = static_cast<T>(0);
 m[0x7] = static_cast<T>(0);
 m[0x8] = static_cast<T>(0);
 m[0x9] = static_cast<T>(0);
 m[0xA] = static_cast<T>(1);
 m[0xB] = static_cast<T>(0);
 m[0xC] = static_cast<T>(0);
 m[0xD] = static_cast<T>(0);
 m[0xE] = static_cast<T>(0);
 m[0xF] = static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_XYX(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] =  c2;
 m[0x1] =  (s2 * s3);
 m[0x2] =  (c3 * s2);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (s1 * s2);
 m[0x5] =  (c1 * c3) - (s1 * c2_s3);
 m[0x6] = -(c1 * s3) - (s1 * c2_c3);
 m[0x7] =  static_cast<T>(0);
 m[0x8] = -(c2 * s2);
 m[0x9] =  (c3 * s1) + (c1 * c2_s3);
 m[0xA] = -(s1 * s3) + (c1 * c2_c3);
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_XYZ(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c2 * c3);
 m[0x1] = -(c2 * s3);
 m[0x2] =  s2;
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (c1 * s3) + (s1 * c3_s2);
 m[0x5] =  (c1 * c3) - (s1 * s2_s3);
 m[0x6] = -(c2 * s1);
 m[0x7] =  static_cast<T>(0);
 m[0x8] =  (s1 * s3) - (c1 * c3_s2);
 m[0x9] =  (c3 * s1) + (c1 * s2_s3);
 m[0xA] =  (c1 * c2);
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_XZX(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] =  c2;
 m[0x1] = -(c3 * s2);
 m[0x2] =  (s2 * s3);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  c1*s2;
 m[0x5] = -(s1 * s3) + (c1 * c2_c3);
 m[0x6] = -(c3 * s1) - (c1 * c2_s3);
 m[0x7] =  static_cast<T>(0);
 m[0x8] =  (s1 * s2);
 m[0x9] =  (c1 * s3) + (s1 * c2_c3);
 m[0xA] =  (c1 * c3) - (s1 * c2_s3);
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_XZY(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c2 * c3);
 m[0x1] = -s2;
 m[0x2] =  (c2 * s3);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (s1 * s3) + (c1 * c3_s2);
 m[0x5] =  (c1 * c2);
 m[0x6] = -(c3 * s1) + (c1 * s2_s3);
 m[0x7] =  static_cast<T>(0);
 m[0x8] = -(c1 * s3) + (s1 * c3_s2);
 m[0x9] =  (c2 * s1);
 m[0xA] =  (c1 * c3) + (s1 * s2_s3);
 m[0xB] = static_cast<T>(0);
 m[0xC] = static_cast<T>(0);
 m[0xD] = static_cast<T>(0);
 m[0xE] = static_cast<T>(0);
 m[0xF] = static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_YXY(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] =  (c1 * c3) - (s1 * c2_s3);
 m[0x1] =  (s1 * s2);
 m[0x2] =  (c1 * s3) + (s1 * c2_c3);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (s2 * s3);
 m[0x5] =  c2;
 m[0x6] = -(c3 * s2);
 m[0x7] =  static_cast<T>(0);
 m[0x8] = -(c3 * s1) - (c1 * c2_s3);
 m[0x9] =  (c1 * c2);
 m[0xA] = -(s1 * s3) + (c1 * c2_c3);
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_YXZ(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] = +(c1 * c3)(s1 * s2_s3);
 m[0x1] = -(c1 * s3)(s1 * c3_s2);
 m[0x2] =  (c2 * s1);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (c2 * s3);
 m[0x5] =  (c2 * c3);
 m[0x6] = -s2;
 m[0x7] =  static_cast<T>(0);
 m[0x8] = -(c3 * s1) + (c1 * s2_s3);
 m[0x9] =  (s1 * s3) + (c1 * c3_s2);
 m[0xA] =  (c1 * c2);
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_YZX(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c1 * c2);
 m[0x1] =  (s1 * s3) - (c1 * c3_s2);
 m[0x2] =  (c3 * s1) + (c1 * s2_s3);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  s2;
 m[0x5] =  (c2 * c3);
 m[0x6] = -(c2 * s3);
 m[0x7] =  static_cast<T>(0);
 m[0x8] = -(c2 * s1);
 m[0x9] =  (c1 * s3) + (s1 * c3_s2);
 m[0xA] =  (c1 * c3) - (s1 * s2_s3);
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_YZY(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] = -(s1 * s3) + (c1 * c2_c3);
 m[0x1] = -(c1 * s2);
 m[0x2] =  (c3 * s1) + (c1 * c2_s3);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (c3 * s2);
 m[0x5] =  c2;
 m[0x6] =  (s2 * s3);
 m[0x7] =  static_cast<T>(0);
 m[0x8] = -(c1 * s3) - (s1 * c2_c3);
 m[0x9] =  (s1 * s2);
 m[0xA] =  (c1 * c3) - (s1 * c2_s3);
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_ZXY(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c1 * c3) - (s1 * s2_s3);
 m[0x1] = -(c2 * s1);
 m[0x2] =  (c1 * s3) + (s1 * c3_s2);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (c3 * s1) + (c1 * s2_s3);
 m[0x5] =  (c1 * c2);
 m[0x6] =  (s1 * s3) - (c1 * c3_s2);
 m[0x7] =  static_cast<T>(0);
 m[0x8] = -(c2 * s3);
 m[0x9] =  s2;
 m[0xA] =  (c2 * c3);
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_ZXZ(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] =  (c1 * c3) - (s1 * c2_s3);
 m[0x1] = -(c1 * s3) - (s1 * c2_c3);
 m[0x2] =  (s1 * s2);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (c3 * s1) + (c1 * c2_s3);
 m[0x5] = -(s1 * s3) + (c1 * c2_c3);
 m[0x6] = -(c1 * s2);
 m[0x7] =  static_cast<T>(0);
 m[0x8] =  (s2 * s3);
 m[0x9] =  (c3 * s2);
 m[0xA] =  c2;
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_ZYX(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c3_s2 = c3 * s2;
 T s2_s3 = s2 * s3;

 // set matrix
 m[0x0] =  (c1 * c2);
 m[0x1] = -(c3 * s1) + (c1 * s2_s3);
 m[0x2] =  (s1 * s3) + (c1 * c3_s2);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (c2 * s1);
 m[0x5] =  (c1 * c3) + (s1 * s2_s3);
 m[0x6] = -(c1 * s3) + (s1 * c3_s2);
 m[0x7] =  static_cast<T>(0);
 m[0x8] = -s2;
 m[0x9] =  (c2 * s3);
 m[0xA] =  (c2 * c3);
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

template<class T>
inline void rotation_matrix4x4_ZYZ(T* m, const T& r1, const T& r2, const T& r3)
{
 // cached cosines
 T c1 = cos(r1);
 T c2 = cos(r2);
 T c3 = cos(r3);

 // cached sines
 T s1 = sin(r1);
 T s2 = sin(r2);
 T s3 = sin(r3);

 // composite values
 T c2_c3 = c2 * c3;
 T c2_s3 = c2 * s3;

 // set matrix
 m[0x0] = -(s1 * s3) + (c1 * c2_c3);
 m[0x1] = -(c3 * s1) - (c1 * c2_s3);
 m[0x2] =  (c1 * s2);
 m[0x3] =  static_cast<T>(0);
 m[0x4] =  (c1 * s3) + (s1 * c2_c3);
 m[0x5] =  (c1 * c3) - (s1 * c2_s3);
 m[0x6] =  (s1 * s2);
 m[0x7] =  static_cast<T>(0);
 m[0x8] = -(c3 * s2);
 m[0x9] =  (s2 * s3);
 m[0xA] =  c2;
 m[0xB] =  static_cast<T>(0);
 m[0xC] =  static_cast<T>(0);
 m[0xD] =  static_cast<T>(0);
 m[0xE] =  static_cast<T>(0);
 m[0xF] =  static_cast<T>(1);
}

}}

//
// MATRIX-QUATERNION CONVERSION
//
namespace cs { namespace math {

template<class T>
inline void quaternion_to_matrix3x3(T* m, const T* q)
{
 // factors
 T a2 = q[0] + q[0];
 T b2 = q[1] + q[1];
 T c2 = q[2] + q[2];
 T d2 = q[3] + q[3];

 // factors
 T ab2 = q[0] * b2;
 T ac2 = q[0] * c2;
 T ad2 = q[0] * d2;
 T bb2 = q[1] * b2;
 T bc2 = q[1] * c2;
 T bd2 = q[1] * d2;
 T cc2 = q[2] * c2;
 T cd2 = q[2] * d2;
 T dd2 = q[3] * d2;

 // convert to matrix
 m[0x0] = static_cast<T>(1) - cc2 - dd2;
 m[0x1] = bc2 - ad2;
 m[0x2] = bd2 + ac2;
 m[0x3] = bc2 + ad2;
 m[0x4] = static_cast<T>(1) - bb2 - dd2;
 m[0x5] = cd2 - ab2;
 m[0x6] = bd2 - ac2;
 m[0x7] = cd2 + ab2;
 m[0x8] = static_cast<T>(1) - bb2 - cc2;
}

template<class T>
inline void quaternion_to_matrix4x4(T* m, const T* q)
{
 // factors
 T a2 = q[0] + q[0];
 T b2 = q[1] + q[1];
 T c2 = q[2] + q[2];
 T d2 = q[3] + q[3];

 // factors
 T ab2 = q[0] * b2;
 T ac2 = q[0] * c2;
 T ad2 = q[0] * d2;
 T bb2 = q[1] * b2;
 T bc2 = q[1] * c2;
 T bd2 = q[1] * d2;
 T cc2 = q[2] * c2;
 T cd2 = q[2] * d2;
 T dd2 = q[3] * d2;

 // reinterpret matrix
 m[0x0] = static_cast<T>(1) - cc2 - dd2;
 m[0x1] = bc2 - ad2;
 m[0x2] = bd2 + ac2;
 m[0x3] = static_cast<T>(0);
 m[0x4] = bc2 + ad2;
 m[0x5] = static_cast<T>(1) - bb2 - dd2;
 m[0x6] = cd2 - ab2;
 m[0x7] = static_cast<T>(0);
 m[0x8] = bd2 - ac2;
 m[0x9] = cd2 + ab2;
 m[0xA] = static_cast<T>(1) - bb2 - cc2;
 m[0xB] = static_cast<T>(0);
 m[0xC] = static_cast<T>(0);
 m[0xD] = static_cast<T>(0);
 m[0xE] = static_cast<T>(0);
 m[0xF] = static_cast<T>(1);
}

}}

#endif
