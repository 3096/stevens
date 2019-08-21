#ifndef __CS_MATH_QUATERNION__
#define __CS_MATH_QUATERNION__

// Standard C++ Headers
#include<istream>
#include<ostream>

// CS headers
#include<cs/math/functions.h>
#include<cs/math/vector3D.h>

namespace cs { namespace math {

//
// FORWARD DECLARATIONS
//

template<class T> class quaternion;
template<> class quaternion<binary32>;
template<> class quaternion<binary64>;

//
// BASE CLASSES
//

typedef struct _c_squaternion { binary32 v[4]; } c_squaternion;
typedef struct _c_dquaternion { binary64 v[4]; } c_dquaternion;
template<class T> struct c_tquaternion { T _value[4]; };

//
// QUATERNION
//

template<class T>
class quaternion : public c_tquaternion<T> {
 public :
  typedef T value_type;
 public :
  quaternion();
  quaternion(const T& w, const T& x, const T& y, const T& z);
  quaternion(const T& w, const T* v);
  explicit quaternion(const T* v);
  quaternion(const quaternion<T>& other);
  template<class X> explicit quaternion(const quaternion<X>& other);
 public :
  T real(void)const;
  const T* imag(void)const;
  void real(const T& w);
  void imag(const T& x, const T& y, const T& z);
  void imag(const T* v);
 public :
  T& operator [](int index);
  const T& operator [](int index)const;
 public :
  quaternion<T>& operator =(const T& w);
  quaternion<T>& operator =(const quaternion<T>& other);
  template<class X> quaternion<T>& operator =(const quaternion<X>& other);
 public :
  quaternion<T>& operator +=(const T& w);
  quaternion<T>& operator -=(const T& w);
  quaternion<T>& operator *=(const T& w);
  quaternion<T>& operator /=(const T& w);
 public :
  quaternion<T>& operator +=(const quaternion<T>& other);
  quaternion<T>& operator -=(const quaternion<T>& other);
  quaternion<T>& operator *=(const quaternion<T>& other);
  quaternion<T>& operator /=(const quaternion<T>& other);
  template<class X> quaternion<T>& operator +=(const quaternion<X>& other);
  template<class X> quaternion<T>& operator -=(const quaternion<X>& other);
  template<class X> quaternion<T>& operator *=(const quaternion<X>& other);
  template<class X> quaternion<T>& operator /=(const quaternion<X>& other);
};

template<class T>
inline quaternion<T>::quaternion()
{
}

template<class T>
inline quaternion<T>::quaternion(const T& w, const T& x, const T& y, const T& z)
{
 this->v[0] = w;
 this->v[1] = x;
 this->v[2] = y;
 this->v[3] = z;
}

template<class T>
inline quaternion<T>::quaternion(const T& w, const T* v)
{
 this->v[0] = w;
 this->v[1] = v[0];
 this->v[2] = v[1];
 this->v[3] = v[2];
}

template<class T>
inline quaternion<T>::quaternion(const T* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
 this->v[2] = v[2];
 this->v[3] = v[3];
}

template<class T>
inline quaternion<T>::quaternion(const quaternion<T>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
 this->v[3] = other.v[3];
}

template<class T> template<class X>
inline quaternion<T>::quaternion(const quaternion<X>& other)
{
 this->v[0] = static_cast<T>(other.v[0]);
 this->v[1] = static_cast<T>(other.v[1]);
 this->v[2] = static_cast<T>(other.v[2]);
 this->v[3] = static_cast<T>(other.v[3]);
}

template<class T>
inline T quaternion<T>::real(void)const
{
 return this->v[0];
}

template<class T>
inline const T* quaternion<T>::imag(void)const
{
 return &(this->v[1]);
}

template<class T>
inline void quaternion<T>::real(const T& w)
{
 this->v[0] = w;
}

template<class T>
inline void quaternion<T>::imag(const T& x, const T& y, const T& z)
{
 this->v[1] = x;
 this->v[2] = y;
 this->v[3] = z;
}

template<class T>
inline void quaternion<T>::imag(const T* v)
{
 this->v[1] = v[0];
 this->v[2] = v[1];
 this->v[3] = v[2];
}

template<class T>
inline T& quaternion<T>::operator [](int index)
{
 return this->v[index];
}

template<class T>
inline const T& quaternion<T>::operator [](int index)const
{
 return this->v[index];
}

template<class T>
inline quaternion<T>& quaternion<T>::operator =(const T& w)
{
 this->v[0] = w;
 this->v[1] = constants<T>::zero();
 this->v[2] = constants<T>::zero();
 this->v[3] = constants<T>::zero();
 return *this;
}

template<class T>
inline quaternion<T>& quaternion<T>::operator =(const quaternion<T>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
 this->v[3] = other.v[3];
 return *this;
}

template<class T> template<class X>
inline quaternion<T>& quaternion<T>::operator =(const quaternion<X>& other)
{
 this->v[0] = static_cast<T>(other.v[0]);
 this->v[1] = static_cast<T>(other.v[1]);
 this->v[2] = static_cast<T>(other.v[2]);
 this->v[3] = static_cast<T>(other.v[3]);
 return *this;
}

template<class T>
quaternion<T>& quaternion<T>::operator +=(const T& w)
{
 this->v[0] += w;
 return *this;
}

template<class T>
quaternion<T>& quaternion<T>::operator -=(const T& w)
{
 this->v[0] -= w;
 return *this;
}

template<class T>
quaternion<T>& quaternion<T>::operator *=(const T& w)
{
 this->v[0] *= w;
 this->v[1] *= w;
 this->v[2] *= w;
 this->v[3] *= w;
 return *this;
}

template<class T>
quaternion<T>& quaternion<T>::operator /=(const T& w)
{
 this->v[0] /= w;
 this->v[1] /= w;
 this->v[2] /= w;
 this->v[3] /= w;
 return *this;
}

template<class T>
inline quaternion<T>& quaternion<T>::operator +=(const quaternion<T>& other)
{
 this->v[0] += other.v[0];
 this->v[1] += other.v[1];
 this->v[2] += other.v[2];
 this->v[3] += other.v[3];
 return *this;
}

template<class T>
inline quaternion<T>& quaternion<T>::operator -=(const quaternion<T>& other)
{
 this->v[0] -= other.v[0];
 this->v[1] -= other.v[1];
 this->v[2] -= other.v[2];
 this->v[3] -= other.v[3];
 return *this;
}

template<class T>
inline quaternion<T>& quaternion<T>::operator *=(const quaternion<T>& other)
{
 // copy
 T a1 = this->v[0];
 T b1 = this->v[1];
 T c1 = this->v[2];
 T d1 = this->v[3];

 // copy
 T a2 = other.v[0];
 T b2 = other.v[1];
 T c2 = other.v[2];
 T d2 = other.v[3];

 // compute
 this->v[0] = a1*a2 - b1*b2 - c1*c2 - d1*d2;
 this->v[1] = a1*b2 + b1*a2 + c1*d2 - d1*c2;
 this->v[2] = a1*c2 - b1*d2 + c1*a2 + d1*b2;
 this->v[3] = a1*d2 + b1*c2 - c1*b2 + d1*a2;

 return *this;
}

template<class T>
inline quaternion<T>& quaternion<T>::operator /=(const quaternion<T>& other)
{
 // copy
 T a1 = this->v[0];
 T b1 = this->v[1];
 T c1 = this->v[2];
 T d1 = this->v[3];

 // copy
 T a2 = other.v[0];
 T b2 = other.v[1];
 T c2 = other.v[2];
 T d2 = other.v[3];

 // compute
 T scalar = inv(a2*a2 + b2*b2 + c2*c2 + d2*d2);
 this->v[0] = ((a1*a2) + (b1*b2) + (c1*c2) + (d1*d2)) * scalar;
 this->v[1] = ((b1*a2) - (a1*b2) + (d1*c2) - (c1*d2)) * scalar;
 this->v[2] = ((c1*a2) - (d1*b2) - (a1*c2) + (b1*d2)) * scalar;
 this->v[3] = ((d1*a2) + (c1*b2) - (b1*c2) - (a1*d2)) * scalar;

 return *this;
}

template<class T> template<class X>
inline quaternion<T>& quaternion<T>::operator +=(const quaternion<X>& other)
{
 this->v[0] += static_cast<T>(other.v[0]);
 this->v[1] += static_cast<T>(other.v[1]);
 this->v[2] += static_cast<T>(other.v[2]);
 this->v[3] += static_cast<T>(other.v[3]);
 return *this;
}

template<class T> template<class X>
inline quaternion<T>& quaternion<T>::operator -=(const quaternion<X>& other)
{
 this->v[0] -= static_cast<T>(other.v[0]);
 this->v[1] -= static_cast<T>(other.v[1]);
 this->v[2] -= static_cast<T>(other.v[2]);
 this->v[3] -= static_cast<T>(other.v[3]);
 return *this;
}

template<class T> template<class X>
inline quaternion<T>& quaternion<T>::operator *=(const quaternion<X>& other)
{
 // copy
 T a1 = this->v[0];
 T b1 = this->v[1];
 T c1 = this->v[2];
 T d1 = this->v[3];

 // copy
 T a2 = static_cast<T>(other.v[0]);
 T b2 = static_cast<T>(other.v[1]);
 T c2 = static_cast<T>(other.v[2]);
 T d2 = static_cast<T>(other.v[3]);

 // compute
 this->v[0] = a1*a2 - b1*b2 - c1*c2 - d1*d2;
 this->v[1] = a1*b2 + b1*a2 + c1*d2 - d1*c2;
 this->v[2] = a1*c2 - b1*d2 + c1*a2 + d1*b2;
 this->v[3] = a1*d2 + b1*c2 - c1*b2 + d1*a2;

 return *this;
}

template<class T> template<class X>
inline quaternion<T>& quaternion<T>::operator /=(const quaternion<X>& other)
{
 // copy
 T a1 = this->v[0];
 T b1 = this->v[1];
 T c1 = this->v[2];
 T d1 = this->v[3];

 // copy
 T a2 = static_cast<T>(other.v[0]);
 T b2 = static_cast<T>(other.v[1]);
 T c2 = static_cast<T>(other.v[2]);
 T d2 = static_cast<T>(other.v[3]);

 // compute
 T scalar = inv(a2*a2 + b2*b2 + c2*c2 + d2*d2);
 this->v[0] = ((a1*a2) + (b1*b2) + (c1*c2) + (d1*d2)) * scalar;
 this->v[1] = ((b1*a2) - (a1*b2) + (d1*c2) - (c1*d2)) * scalar;
 this->v[2] = ((c1*a2) - (d1*b2) - (a1*c2) + (b1*d2)) * scalar;
 this->v[3] = ((d1*a2) + (c1*b2) - (b1*c2) - (a1*d2)) * scalar;

 return *this;
}

//
// QUATERNION SPECIALIZATION
//

template<>
class quaternion<binary32> : public c_squaternion {
 public :
  typedef binary32 value_type;
 public :
  quaternion();
  quaternion(binary32 w, binary32 x, binary32 y, binary32 z);
  quaternion(binary32 w, const binary32* v);
  explicit quaternion(const binary32* v);
  explicit quaternion(const quaternion<binary64>& other);
 public :
  binary32 real(void)const;
  const binary32* imag(void)const;
  void real(binary32 w);
  void imag(binary32 x, binary32 y, binary32 z);
  void imag(const binary32* v);
 public :
  binary32& operator [](int index);
  const binary32& operator [](int index)const;
 public :
  quaternion<binary32>& operator =(binary32 w);
  quaternion<binary32>& operator =(const quaternion<binary32>& other);
  template<class X> quaternion<binary32>& operator =(const quaternion<X>& other);
 public :
  quaternion<binary32>& operator +=(binary32 w);
  quaternion<binary32>& operator -=(binary32 w);
  quaternion<binary32>& operator *=(binary32 w);
  quaternion<binary32>& operator /=(binary32 w);
 public :
  quaternion<binary32>& operator +=(const quaternion<binary32>& other);
  quaternion<binary32>& operator -=(const quaternion<binary32>& other);
  quaternion<binary32>& operator *=(const quaternion<binary32>& other);
  quaternion<binary32>& operator /=(const quaternion<binary32>& other);
  template<class X> quaternion<binary32>& operator +=(const quaternion<X>& other);
  template<class X> quaternion<binary32>& operator -=(const quaternion<X>& other);
  template<class X> quaternion<binary32>& operator *=(const quaternion<X>& other);
  template<class X> quaternion<binary32>& operator /=(const quaternion<X>& other);
};

inline quaternion<binary32>::quaternion()
{
}

inline quaternion<binary32>::quaternion(binary32 w, binary32 x, binary32 y, binary32 z)
{
 this->v[0] = w;
 this->v[1] = x;
 this->v[2] = y;
 this->v[3] = z;
}

inline quaternion<binary32>::quaternion(binary32 w, const binary32* v)
{
 this->v[0] = w;
 this->v[1] = v[0];
 this->v[2] = v[1];
 this->v[3] = v[2];
}

inline quaternion<binary32>::quaternion(const binary32* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
 this->v[2] = v[2];
 this->v[3] = v[3];
}

inline binary32 quaternion<binary32>::real(void)const
{
 return this->v[0];
}

inline const binary32* quaternion<binary32>::imag(void)const
{
 return &(this->v[1]);
}

inline void quaternion<binary32>::real(binary32 w)
{
 this->v[0] = w;
}


inline void quaternion<binary32>::imag(binary32 x, binary32 y, binary32 z)
{
 this->v[1] = x;
 this->v[2] = y;
 this->v[3] = z;
}

inline void quaternion<binary32>::imag(const binary32* v)
{
 this->v[1] = v[0];
 this->v[2] = v[1];
 this->v[3] = v[2];
}

inline binary32& quaternion<binary32>::operator [](int index)
{
 return this->v[index];
}

inline const binary32& quaternion<binary32>::operator [](int index)const
{
 return this->v[index];
}

inline quaternion<binary32>& quaternion<binary32>::operator =(binary32 w)
{
 this->v[0] = w;
 this->v[1] = 0.0f;
 this->v[2] = 0.0f;
 this->v[3] = 0.0f;
 return *this;
}

inline quaternion<binary32>& quaternion<binary32>::operator =(const quaternion<binary32>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
 this->v[3] = other.v[3];
 return *this;
}

template<class X>
inline quaternion<binary32>& quaternion<binary32>::operator =(const quaternion<X>& other)
{
 this->v[0] = static_cast<binary32>(other.v[0]);
 this->v[1] = static_cast<binary32>(other.v[1]);
 this->v[2] = static_cast<binary32>(other.v[2]);
 this->v[3] = static_cast<binary32>(other.v[3]);
 return *this;
}

inline quaternion<binary32>& quaternion<binary32>::operator +=(binary32 w)
{
 this->v[0] += w;
 return *this;
}

inline quaternion<binary32>& quaternion<binary32>::operator -=(binary32 w)
{
 this->v[0] -= w;
 return *this;
}

inline quaternion<binary32>& quaternion<binary32>::operator *=(binary32 w)
{
 this->v[0] *= w;
 this->v[1] *= w;
 this->v[2] *= w;
 this->v[3] *= w;
 return *this;
}

inline quaternion<binary32>& quaternion<binary32>::operator /=(binary32 w)
{
 this->v[0] /= w;
 this->v[1] /= w;
 this->v[2] /= w;
 this->v[3] /= w;
 return *this;
}

inline quaternion<binary32>& quaternion<binary32>::operator +=(const quaternion<binary32>& other)
{
 this->v[0] += other.v[0];
 this->v[1] += other.v[1];
 this->v[2] += other.v[2];
 this->v[3] += other.v[3];
 return *this;
}

inline quaternion<binary32>& quaternion<binary32>::operator -=(const quaternion<binary32>& other)
{
 this->v[0] -= other.v[0];
 this->v[1] -= other.v[1];
 this->v[2] -= other.v[2];
 this->v[3] -= other.v[3];
 return *this;
}

inline quaternion<binary32>& quaternion<binary32>::operator *=(const quaternion<binary32>& other)
{
 // copy
 binary32 a1 = this->v[0];
 binary32 b1 = this->v[1];
 binary32 c1 = this->v[2];
 binary32 d1 = this->v[3];

 // copy
 binary32 a2 = other.v[0];
 binary32 b2 = other.v[1];
 binary32 c2 = other.v[2];
 binary32 d2 = other.v[3];

 // compute
 this->v[0] = a1*a2 - b1*b2 - c1*c2 - d1*d2;
 this->v[1] = a1*b2 + b1*a2 + c1*d2 - d1*c2;
 this->v[2] = a1*c2 - b1*d2 + c1*a2 + d1*b2;
 this->v[3] = a1*d2 + b1*c2 - c1*b2 + d1*a2;

 return *this;
}

inline quaternion<binary32>& quaternion<binary32>::operator /=(const quaternion<binary32>& other)
{
 // copy
 binary32 a1 = this->v[0];
 binary32 b1 = this->v[1];
 binary32 c1 = this->v[2];
 binary32 d1 = this->v[3];

 // copy
 binary32 a2 = other.v[0];
 binary32 b2 = other.v[1];
 binary32 c2 = other.v[2];
 binary32 d2 = other.v[3];

 // compute
 binary32 scalar = inv(a2*a2 + b2*b2 + c2*c2 + d2*d2);
 this->v[0] = ((a1*a2) + (b1*b2) + (c1*c2) + (d1*d2)) * scalar;
 this->v[1] = ((b1*a2) - (a1*b2) + (d1*c2) - (c1*d2)) * scalar;
 this->v[2] = ((c1*a2) - (d1*b2) - (a1*c2) + (b1*d2)) * scalar;
 this->v[3] = ((d1*a2) + (c1*b2) - (b1*c2) - (a1*d2)) * scalar;

 return *this;
}

template<class X>
inline quaternion<binary32>& quaternion<binary32>::operator +=(const quaternion<X>& other)
{
 this->v[0] += static_cast<binary32>(other.v[0]);
 this->v[1] += static_cast<binary32>(other.v[1]);
 this->v[2] += static_cast<binary32>(other.v[2]);
 this->v[3] += static_cast<binary32>(other.v[3]);
 return *this;
}

template<class X>
inline quaternion<binary32>& quaternion<binary32>::operator -=(const quaternion<X>& other)
{
 this->v[0] -= static_cast<binary32>(other.v[0]);
 this->v[1] -= static_cast<binary32>(other.v[1]);
 this->v[2] -= static_cast<binary32>(other.v[2]);
 this->v[3] -= static_cast<binary32>(other.v[3]);
 return *this;
}

template<class X>
inline quaternion<binary32>& quaternion<binary32>::operator *=(const quaternion<X>& other)
{
 // copy
 binary32 a1 = this->v[0];
 binary32 b1 = this->v[1];
 binary32 c1 = this->v[2];
 binary32 d1 = this->v[3];

 // copy
 binary32 a2 = static_cast<binary32>(other.v[0]);
 binary32 b2 = static_cast<binary32>(other.v[1]);
 binary32 c2 = static_cast<binary32>(other.v[2]);
 binary32 d2 = static_cast<binary32>(other.v[3]);

 // compute
 this->v[0] = a1*a2 - b1*b2 - c1*c2 - d1*d2;
 this->v[1] = a1*b2 + b1*a2 + c1*d2 - d1*c2;
 this->v[2] = a1*c2 - b1*d2 + c1*a2 + d1*b2;
 this->v[3] = a1*d2 + b1*c2 - c1*b2 + d1*a2;

 return *this;
}

template<class X>
inline quaternion<binary32>& quaternion<binary32>::operator /=(const quaternion<X>& other)
{
 // copy
 binary32 a1 = this->v[0];
 binary32 b1 = this->v[1];
 binary32 c1 = this->v[2];
 binary32 d1 = this->v[3];

 // copy
 binary32 a2 = other.v[0];
 binary32 b2 = other.v[1];
 binary32 c2 = other.v[2];
 binary32 d2 = other.v[3];

 // compute
 binary32 scalar = inv(a2*a2 + b2*b2 + c2*c2 + d2*d2);
 this->v[0] = ((a1*a2) + (b1*b2) + (c1*c2) + (d1*d2)) * scalar;
 this->v[1] = ((b1*a2) - (a1*b2) + (d1*c2) - (c1*d2)) * scalar;
 this->v[2] = ((c1*a2) - (d1*b2) - (a1*c2) + (b1*d2)) * scalar;
 this->v[3] = ((d1*a2) + (c1*b2) - (b1*c2) - (a1*d2)) * scalar;

 return *this;
}

//
// QUATERNION SPECIALIZATION
//

template<>
class quaternion<binary64> : public c_dquaternion {
 public :
  typedef binary64 value_type;
 public :
  quaternion();
  quaternion(binary64 w, binary64 x, binary64 y, binary64 z);
  quaternion(binary64 w, const binary64* v);
  explicit quaternion(const binary64* v);
  explicit quaternion(const quaternion<binary32>& other);
 public :
  binary64 real(void)const;
  const binary64* imag(void)const;
  void real(binary64 w);
  void imag(binary64 x, binary64 y, binary64 z);
  void imag(const binary64* v);
 public :
  binary64& operator [](int index);
  const binary64& operator [](int index)const;
 public :
  quaternion<binary64>& operator =(binary64 w);
  quaternion<binary64>& operator =(const quaternion<binary64>& other);
  template<class X> quaternion<binary64>& operator =(const quaternion<X>& other);
 public :
  quaternion<binary64>& operator +=(binary64 w);
  quaternion<binary64>& operator -=(binary64 w);
  quaternion<binary64>& operator *=(binary64 w);
  quaternion<binary64>& operator /=(binary64 w);
 public :
  quaternion<binary64>& operator +=(const quaternion<binary64>& other);
  quaternion<binary64>& operator -=(const quaternion<binary64>& other);
  quaternion<binary64>& operator *=(const quaternion<binary64>& other);
  quaternion<binary64>& operator /=(const quaternion<binary64>& other);
  template<class X> quaternion<binary64>& operator +=(const quaternion<X>& other);
  template<class X> quaternion<binary64>& operator -=(const quaternion<X>& other);
  template<class X> quaternion<binary64>& operator *=(const quaternion<X>& other);
  template<class X> quaternion<binary64>& operator /=(const quaternion<X>& other);
};

inline quaternion<binary64>::quaternion()
{
}

inline quaternion<binary64>::quaternion(binary64 w, binary64 x, binary64 y, binary64 z)
{
 this->v[0] = w;
 this->v[1] = x;
 this->v[2] = y;
 this->v[3] = z;
}

inline quaternion<binary64>::quaternion(binary64 w, const binary64* v)
{
 this->v[0] = w;
 this->v[1] = v[0];
 this->v[2] = v[1];
 this->v[3] = v[2];
}

inline quaternion<binary64>::quaternion(const binary64* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
 this->v[2] = v[2];
 this->v[3] = v[3];
}

inline binary64 quaternion<binary64>::real(void)const
{
 return this->v[0];
}

inline const binary64* quaternion<binary64>::imag(void)const
{
 return &(this->v[1]);
}

inline void quaternion<binary64>::real(binary64 w)
{
 this->v[0] = w;
}

inline void quaternion<binary64>::imag(binary64 x, binary64 y, binary64 z)
{
 this->v[1] = x;
 this->v[2] = y;
 this->v[3] = z;
}

inline void quaternion<binary64>::imag(const binary64* v)
{
 this->v[1] = v[0];
 this->v[2] = v[1];
 this->v[3] = v[2];
}

inline binary64& quaternion<binary64>::operator [](int index)
{
 return this->v[index];
}

inline const binary64& quaternion<binary64>::operator [](int index)const
{
 return this->v[index];
}

inline quaternion<binary64>& quaternion<binary64>::operator =(binary64 w)
{
 this->v[0] = w;
 this->v[1] = 0.0;
 this->v[2] = 0.0;
 this->v[3] = 0.0;
 return *this;
}

inline quaternion<binary64>& quaternion<binary64>::operator =(const quaternion<binary64>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
 this->v[3] = other.v[3];
 return *this;
}

template<class X>
inline quaternion<binary64>& quaternion<binary64>::operator =(const quaternion<X>& other)
{
 this->v[0] = static_cast<binary64>(other.v[0]);
 this->v[1] = static_cast<binary64>(other.v[1]);
 this->v[2] = static_cast<binary64>(other.v[2]);
 this->v[3] = static_cast<binary64>(other.v[3]);
 return *this;
}

inline quaternion<binary64>& quaternion<binary64>::operator +=(binary64 w)
{
 this->v[0] += w;
 return *this;
}

inline quaternion<binary64>& quaternion<binary64>::operator -=(binary64 w)
{
 this->v[0] -= w;
 return *this;
}

inline quaternion<binary64>& quaternion<binary64>::operator *=(binary64 w)
{
 this->v[0] *= w;
 this->v[1] *= w;
 this->v[2] *= w;
 this->v[3] *= w;
 return *this;
}

inline quaternion<binary64>& quaternion<binary64>::operator /=(binary64 w)
{
 this->v[0] /= w;
 this->v[1] /= w;
 this->v[2] /= w;
 this->v[3] /= w;
 return *this;
}

inline quaternion<binary64>& quaternion<binary64>::operator +=(const quaternion<binary64>& other)
{
 this->v[0] += other.v[0];
 this->v[1] += other.v[1];
 this->v[2] += other.v[2];
 this->v[3] += other.v[3];
 return *this;
}

inline quaternion<binary64>& quaternion<binary64>::operator -=(const quaternion<binary64>& other)
{
 this->v[0] -= other.v[0];
 this->v[1] -= other.v[1];
 this->v[2] -= other.v[2];
 this->v[3] -= other.v[3];
 return *this;
}

inline quaternion<binary64>& quaternion<binary64>::operator *=(const quaternion<binary64>& other)
{
 // copy
 binary64 a1 = this->v[0];
 binary64 b1 = this->v[1];
 binary64 c1 = this->v[2];
 binary64 d1 = this->v[3];

 // copy
 binary64 a2 = other.v[0];
 binary64 b2 = other.v[1];
 binary64 c2 = other.v[2];
 binary64 d2 = other.v[3];

 // compute
 this->v[0] = a1*a2 - b1*b2 - c1*c2 - d1*d2;
 this->v[1] = a1*b2 + b1*a2 + c1*d2 - d1*c2;
 this->v[2] = a1*c2 - b1*d2 + c1*a2 + d1*b2;
 this->v[3] = a1*d2 + b1*c2 - c1*b2 + d1*a2;

 return *this;
}

inline quaternion<binary64>& quaternion<binary64>::operator /=(const quaternion<binary64>& other)
{
 // copy
 binary64 a1 = this->v[0];
 binary64 b1 = this->v[1];
 binary64 c1 = this->v[2];
 binary64 d1 = this->v[3];

 // copy
 binary64 a2 = other.v[0];
 binary64 b2 = other.v[1];
 binary64 c2 = other.v[2];
 binary64 d2 = other.v[3];

 // compute
 binary64 scalar = inv(a2*a2 + b2*b2 + c2*c2 + d2*d2);
 this->v[0] = ((a1*a2) + (b1*b2) + (c1*c2) + (d1*d2)) * scalar;
 this->v[1] = ((b1*a2) - (a1*b2) + (d1*c2) - (c1*d2)) * scalar;
 this->v[2] = ((c1*a2) - (d1*b2) - (a1*c2) + (b1*d2)) * scalar;
 this->v[3] = ((d1*a2) + (c1*b2) - (b1*c2) - (a1*d2)) * scalar;

 return *this;
}

template<class X>
inline quaternion<binary64>& quaternion<binary64>::operator +=(const quaternion<X>& other)
{
 this->v[0] += static_cast<binary64>(other.v[0]);
 this->v[1] += static_cast<binary64>(other.v[1]);
 this->v[2] += static_cast<binary64>(other.v[2]);
 this->v[3] += static_cast<binary64>(other.v[3]);
 return *this;
}

template<class X>
inline quaternion<binary64>& quaternion<binary64>::operator -=(const quaternion<X>& other)
{
 this->v[0] -= static_cast<binary64>(other.v[0]);
 this->v[1] -= static_cast<binary64>(other.v[1]);
 this->v[2] -= static_cast<binary64>(other.v[2]);
 this->v[3] -= static_cast<binary64>(other.v[3]);
 return *this;
}

template<class X>
inline quaternion<binary64>& quaternion<binary64>::operator *=(const quaternion<X>& other)
{
 // copy
 binary64 a1 = this->v[0];
 binary64 b1 = this->v[1];
 binary64 c1 = this->v[2];
 binary64 d1 = this->v[3];

 // copy
 binary64 a2 = static_cast<binary64>(other.v[0]);
 binary64 b2 = static_cast<binary64>(other.v[1]);
 binary64 c2 = static_cast<binary64>(other.v[2]);
 binary64 d2 = static_cast<binary64>(other.v[3]);

 // compute
 this->v[0] = a1*a2 - b1*b2 - c1*c2 - d1*d2;
 this->v[1] = a1*b2 + b1*a2 + c1*d2 - d1*c2;
 this->v[2] = a1*c2 - b1*d2 + c1*a2 + d1*b2;
 this->v[3] = a1*d2 + b1*c2 - c1*b2 + d1*a2;

 return *this;
}

template<class X>
inline quaternion<binary64>& quaternion<binary64>::operator /=(const quaternion<X>& other)
{
 // copy
 binary64 a1 = this->v[0];
 binary64 b1 = this->v[1];
 binary64 c1 = this->v[2];
 binary64 d1 = this->v[3];

 // copy
 binary64 a2 = static_cast<binary64>(other.v[0]);
 binary64 b2 = static_cast<binary64>(other.v[1]);
 binary64 c2 = static_cast<binary64>(other.v[2]);
 binary64 d2 = static_cast<binary64>(other.v[3]);

 // compute
 binary64 scalar = inv(a2*a2 + b2*b2 + c2*c2 + d2*d2);
 this->v[0] = ((a1*a2) + (b1*b2) + (c1*c2) + (d1*d2)) * scalar;
 this->v[1] = ((b1*a2) - (a1*b2) + (d1*c2) - (c1*d2)) * scalar;
 this->v[2] = ((c1*a2) - (d1*b2) - (a1*c2) + (b1*d2)) * scalar;
 this->v[3] = ((d1*a2) + (c1*b2) - (b1*c2) - (a1*d2)) * scalar;

 return *this;
}

//
// EXPLICIT CONSTRUCTORS
//

inline quaternion<binary32>::quaternion(const quaternion<binary64>& other)
{
 this->v[0] = static_cast<binary32>(other.v[0]);
 this->v[1] = static_cast<binary32>(other.v[1]);
 this->v[2] = static_cast<binary32>(other.v[2]);
 this->v[3] = static_cast<binary32>(other.v[3]);
}

inline quaternion<binary64>::quaternion(const quaternion<binary32>& other)
{
 this->v[0] = static_cast<binary64>(other.v[0]);
 this->v[1] = static_cast<binary64>(other.v[1]);
 this->v[2] = static_cast<binary64>(other.v[2]);
 this->v[3] = static_cast<binary64>(other.v[3]);
}

//
// NON-MEMBER OPERATORS
//

template<class T>
inline quaternion<T> operator +(const quaternion<T>& lhs, const quaternion<T>& rhs)
{
 return quaternion<T>(
  lhs.v[0] + rhs.v[0],
  lhs.v[1] + rhs.v[1],
  lhs.v[2] + rhs.v[2],
  lhs.v[3] + rhs.v[3]
 );
}

template<class T>
inline quaternion<T> operator +(const quaternion<T>& lhs, const T& rhs)
{
 return quaternion<T>(
  lhs.v[0] + rhs,
  lhs.v[1],
  lhs.v[2],
  lhs.v[3]
 );
}

template<class T>
inline quaternion<T> operator +(const T& lhs, const quaternion<T>& rhs)
{
 return quaternion<T>(
  rhs.v[0] + lhs,
  rhs.v[1],
  rhs.v[2],
  rhs.v[3]
 );
}

template<class T>
inline quaternion<T> operator -(const quaternion<T>& lhs, const quaternion<T>& rhs)
{
 return quaternion<T>(
  lhs.v[0] - rhs.v[0],
  lhs.v[1] - rhs.v[1],
  lhs.v[2] - rhs.v[2],
  lhs.v[3] - rhs.v[3]
 );
}

template<class T>
inline quaternion<T> operator -(const quaternion<T>& lhs, const T& rhs)
{
 return quaternion<T>(
  lhs.v[0] - rhs,
  lhs.v[1],
  lhs.v[2],
  lhs.v[3]
 );
}

template<class T>
inline quaternion<T> operator -(const T& lhs, const quaternion<T>& rhs)
{
 return quaternion<T>(
  -rhs.v[0] + lhs,
  -rhs.v[1],
  -rhs.v[2],
  -rhs.v[3]
 );
}

template<class T>
inline quaternion<T> operator *(const quaternion<T>& lhs, const quaternion<T>& rhs)
{
 return quaternion<T>(
  lhs.v[0]*rhs.v[0] - lhs.v[1]*rhs.v[1] - lhs.v[2]*rhs.v[2] - lhs.v[3]*rhs.v[3],
  lhs.v[0]*rhs.v[1] + lhs.v[1]*rhs.v[0] + lhs.v[2]*rhs.v[3] - lhs.v[3]*rhs.v[2],
  lhs.v[0]*rhs.v[2] - lhs.v[1]*rhs.v[3] + lhs.v[2]*rhs.v[0] + lhs.v[3]*rhs.v[1],
  lhs.v[0]*rhs.v[3] + lhs.v[1]*rhs.v[2] - lhs.v[2]*rhs.v[1] + lhs.v[3]*rhs.v[0]
 );
}

template<class T>
inline quaternion<T> operator *(const quaternion<T>& lhs, const T& rhs)
{
 return quaternion<T>(
  lhs.v[0] * rhs,
  lhs.v[1] * rhs,
  lhs.v[2] * rhs,
  lhs.v[3] * rhs
 );
}

template<class T>
inline quaternion<T> operator *(const T& lhs, const quaternion<T>& rhs)
{
 return quaternion<T>(
  rhs.v[0] * lhs,
  rhs.v[1] * lhs,
  rhs.v[2] * lhs,
  rhs.v[3] * lhs
 );
}

template<class T>
inline quaternion<T> operator /(const quaternion<T>& lhs, const quaternion<T>& rhs)
{
 T scalar = inv(rhs.v[0]*rhs.v[0] + rhs.v[1]*rhs.v[1] + rhs.v[2]*rhs.v[2] + rhs.v[3]*rhs.v[3]);
 return quaternion<T>(
  (lhs.v[0]*rhs.v[0] + lhs.v[1]*rhs.v[1] + lhs.v[2]*rhs.v[2] + lhs.v[3]*rhs.v[3]) * scalar,
  (lhs.v[1]*rhs.v[0] - lhs.v[0]*rhs.v[1] + lhs.v[3]*rhs.v[2] - lhs.v[2]*rhs.v[3]) * scalar,
  (lhs.v[2]*rhs.v[0] - lhs.v[3]*rhs.v[1] - lhs.v[0]*rhs.v[2] + lhs.v[1]*rhs.v[3]) * scalar,
  (lhs.v[3]*rhs.v[0] + lhs.v[2]*rhs.v[1] - lhs.v[1]*rhs.v[2] - lhs.v[0]*rhs.v[3]) * scalar
 );
}

template<class T>
inline quaternion<T> operator /(const quaternion<T>& lhs, const T& rhs)
{
 return quaternion<T>(
  lhs.v[0] / rhs,
  lhs.v[1] / rhs,
  lhs.v[2] / rhs,
  lhs.v[3] / rhs
 );
}

template<class T>
inline quaternion<T> operator /(const T& lhs, const quaternion<T>& rhs)
{
 T scalar = inv(rhs.v[0]*rhs.v[0] + rhs.v[1]*rhs.v[1] + rhs.v[2]*rhs.v[2] + rhs.v[3]*rhs.v[3])*lhs;
 return quaternion<T>(
   rhs.v[0] * scalar,
  -rhs.v[1] * scalar,
  -rhs.v[2] * scalar,
  -rhs.v[3] * scalar
 );
}

template<class T>
inline quaternion<T> operator +(const quaternion<T>& rhs)
{
 return rhs;
}

template<class T>
inline quaternion<T> operator -(const quaternion<T>& rhs)
{
 return quaternion<T>(-rhs.v[0], -rhs.v[1], -rhs.v[2], -rhs.v[3]);
}

template<class T>
inline bool operator ==(const quaternion<T>& lhs, const quaternion<T>& rhs)
{
 return ((lhs.v[0] == rhs.v[0]) &&
         (lhs.v[1] == rhs.v[1]) &&
         (lhs.v[2] == rhs.v[2]) &&
         (lhs.v[3] == rhs.v[3]));
}

template<class T>
inline bool operator ==(const quaternion<T>& lhs, const T& rhs)
{
 return ((lhs.v[0] == rhs) && !lhs.v[1] && !lhs.v[2] && !lhs.v[3]);
}

template<class T>
inline bool operator ==(const T& lhs, const quaternion<T>& rhs)
{
 return ((rhs.v[0] == lhs) && !rhs.v[1] && !rhs.v[2] && !rhs.v[3]);
}

template<class T>
inline bool operator !=(const quaternion<T>& lhs, const quaternion<T>& rhs)
{
 return !(lhs == rhs);
}

template<class T>
inline bool operator !=(const quaternion<T>& lhs, const T& rhs)
{
 return !(lhs == rhs);
}

template<class T>
inline bool operator !=(const T& lhs, const quaternion<T>& rhs)
{
 return !(lhs == rhs);
}

template<class T, class charT, class traits>
std::basic_ostream<charT, traits>& operator <<(std::basic_ostream<charT, traits>& os, const quaternion<T>& rhs)
{
 // copy
 std::basic_ostringstream<charT, traits> oss;
 oss.flags(os.flags());
 oss.imbue(os.getloc());
 oss.precision(os.precision());

 // format w
 oss << real(rhs);

 // format x
 if(imag(rhs)[0] < T(0)) oss << '-' << -imag(rhs)[0] << 'i';
 else oss << '+' << imag(rhs)[0] << 'i';

 // format y
 if(imag(rhs)[1] < T(0)) oss << '-' << -imag(rhs)[1] << 'j';
 else oss << '+' << imag(rhs)[1] << 'j';

 // format z
 if(imag(rhs)[2] < T(0)) oss << '-' << -imag(rhs)[2] << 'k';
 else oss << '+' << imag(rhs)[2] << 'k';

 // output to stream
 std::basic_string<charT, traits> str = oss.str();
 return (os << str.c_str());
}

//
// NON-MEMBER FUNCTIONS
//

template<class T>
inline T real(const quaternion<T>& q)
{
 return q.v[0];
}

template<class T>
inline const T* imag(const quaternion<T>& q)
{
 return &(q.v[1]);
}

template<class T>
inline quaternion<T> conj(const quaternion<T>& q)
{
 return quaternion<T>(q.v[0], -q.v[1], -q.v[2], -q.v[3]);
}

template<class T>
inline T squared_norm(const quaternion<T>& q)
{
 return (q.v[0]*q.v[0]) + (q.v[1]*q.v[1]) + (q.v[2]*q.v[2]) + (q.v[3]*q.v[3]);
}

template<class T>
inline T norm(const quaternion<T>& q)
{
 return sqrt(squared_norm(q));
}

template<class T>
inline quaternion<T> unit(const quaternion<T>& q)
{
 T scalar = inv(norm(q));
 return quaternion<T>(
  q.v[0] * scalar,
  q.v[1] * scalar,
  q.v[2] * scalar,
  q.v[3] * scalar
 );
}

template<class T>
inline void normalize(quaternion<T>& q)
{
 T scalar = inv(norm(q));
 q.v[0] *= scalar;
 q.v[1] *= scalar;
 q.v[2] *= scalar;
 q.v[3] *= scalar;
}

template<class T>
inline T length(const quaternion<T>& q)
{
 return sqrt(squared_norm(q));
}

template<class T>
inline quaternion<T> square(const quaternion<T>& q)
{
 T scalar = constants<T>::two()*q.v[0];
 return quaternion<T>(
  q.v[0]*q.v[0] - q.v[1]*q.v[1] - q.v[2]*q.v[2] - q.v[3]*q.v[3],
  scalar*q.v[1],
  scalar*q.v[2],
  scalar*q.v[3]
 );
}
 
template<class T>
inline quaternion<T> inv(const quaternion<T>& q)
{
 T scalar = inv(squared_norm(q));
 return quaternion<T>(
   q.v[0] * scalar,
  -q.v[1] * scalar,
  -q.v[2] * scalar,
  -q.v[3] * scalar
 );
}

template<class T>
inline T scalar_product(const quaternion<T>& q1, const quaternion<T>& q2)
{
 return (q1.v[0]*q2.v[0]) +
        (q1.v[1]*q2.v[1]) +
        (q1.v[2]*q2.v[2]) +
        (q1.v[3]*q2.v[3]);
}

//
// TYPEDEFS
//

typedef class quaternion<binary32> quaternion32;
typedef class quaternion<binary64> quaternion64;

}};

#endif
