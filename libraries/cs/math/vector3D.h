#ifndef __CS_MATH_GENERAL_VECTOR3D__
#define __CS_MATH_GENERAL_VECTOR3D__

// Standard C++ Headers
#include<istream>
#include<ostream>

// CS headers
#include<cs/math/functions.h>

namespace cs { namespace math {

//
// FORWARD DECLARATIONS
//

template<class T> class vector3D;
template<> class vector3D<binary32>;
template<> class vector3D<binary64>;

//
// BASE CLASSES
//

typedef struct _c_svector3D { binary32 v[3]; } c_svector3D;
typedef struct _c_dvector3D { binary64 v[3]; } c_dvector3D;
template<class T> struct c_tvector3D { T v[3]; };

//
// VECTOR
//

template<class T>
class vector3D : public c_tvector3D<T> {
 public :
  typedef T value_type;
 public :
  vector3D();
  vector3D(const T& x, const T& y, const T& z);
  explicit vector3D(const T* v);
  vector3D(const vector3D<T>& other);
  template<class X> vector3D(const vector3D<X>& other);
 public :
  T x(void)const;
  T y(void)const;
  T z(void)const;
 public :
  void x(const T& value);
  void y(const T& value);
  void z(const T& value);
  void reset(const T& x, const T& y, const T& z);
  void reset(const T* v);
 public :
  T& operator [](int index);
  const T& operator [](int index)const;
 public :
  vector3D<T>& operator =(const vector3D<T>& other);
  template<class X> vector3D<T>& operator =(const vector3D<X>& other);
 public :
  vector3D<T>& operator +=(const vector3D<T>& other);
  vector3D<T>& operator -=(const vector3D<T>& other);
  vector3D<T>& operator *=(const T& scalar);
  vector3D<T>& operator /=(const T& scalar);
 public :
  template<class X> vector3D<T>& operator +=(const vector3D<X>& other);
  template<class X> vector3D<T>& operator -=(const vector3D<X>& other);
};

template<class T>
inline vector3D<T>::vector3D()
{
}

template<class T>
inline vector3D<T>::vector3D(const T& x, const T& y, const T& z)
{
 this->v[0] = x;
 this->v[1] = y;
 this->v[2] = z;
}

template<class T>
inline vector3D<T>::vector3D(const T* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
 this->v[2] = v[2];
}

template<class T>
inline vector3D<T>::vector3D(const vector3D<T>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
}

template<class T> template<class X>
inline vector3D<T>::vector3D(const vector3D<X>& other)
{
 this->v[0] = static_cast<T>(other.v[0]);
 this->v[1] = static_cast<T>(other.v[1]);
 this->v[2] = static_cast<T>(other.v[2]);
}

template<class T>
inline T vector3D<T>::x(void)const
{
 return this->v[0];
}

template<class T>
inline T vector3D<T>::y(void)const
{
 return this->v[1];
}

template<class T>
inline T vector3D<T>::z(void)const
{
 return this->v[2];
}

template<class T>
inline void vector3D<T>::x(const T& value)
{
 this->v[0] = value;
}

template<class T>
inline void vector3D<T>::y(const T& value)
{
 this->v[1] = value;
}

template<class T>
inline void vector3D<T>::z(const T& value)
{
 this->v[2] = value;
}

template<class T>
inline void vector3D<T>::reset(const T& x, const T& y, const T& z)
{
 this->v[0] = x;
 this->v[1] = y;
 this->v[2] = z;
}

template<class T>
inline void vector3D<T>::reset(const T* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
 this->v[2] = v[2];
}

template<class T>
inline T& vector3D<T>::operator [](int index)
{
 return this->v[index];
}

template<class T>
inline const T& vector3D<T>::operator [](int index)const
{
 return this->v[index];
}

template<class T>
inline vector3D<T>& vector3D<T>::operator =(const vector3D<T>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
 return *this;
}

template<class T> template<class X>
inline vector3D<T>& vector3D<T>::operator =(const vector3D<X>& other)
{
 this->v[0] = static_cast<T>(other.v[0]);
 this->v[1] = static_cast<T>(other.v[1]);
 this->v[2] = static_cast<T>(other.v[2]);
 return *this;
}

template<class T>
inline vector3D<T>& vector3D<T>::operator +=(const vector3D<T>& other)
{
 this->v[0] += other.v[0];
 this->v[1] += other.v[1];
 this->v[2] += other.v[2];
 return *this;
}

template<class T>
inline vector3D<T>& vector3D<T>::operator -=(const vector3D<T>& other)
{
 this->v[0] -= other.v[0];
 this->v[1] -= other.v[1];
 this->v[2] -= other.v[2];
 return *this;
}

template<class T>
inline vector3D<T>& vector3D<T>::operator *=(const T& scalar)
{
 this->v[0] *= scalar;
 this->v[1] *= scalar;
 this->v[2] *= scalar;
 return *this;
}

template<class T>
inline vector3D<T>& vector3D<T>::operator /=(const T& scalar)
{
 this->v[0] /= scalar;
 this->v[1] /= scalar;
 this->v[2] /= scalar;
 return *this;
}

template<class T> template<class X>
inline vector3D<T>& vector3D<T>::operator +=(const vector3D<X>& other)
{
 this->v[0] += static_cast<T>(other.v[0]);
 this->v[1] += static_cast<T>(other.v[1]);
 this->v[2] += static_cast<T>(other.v[2]);
 return *this;
}

template<class T> template<class X>
inline vector3D<T>& vector3D<T>::operator -=(const vector3D<X>& other)
{
 this->v[0] -= static_cast<T>(other.v[0]);
 this->v[1] -= static_cast<T>(other.v[1]);
 this->v[2] -= static_cast<T>(other.v[2]);
 return *this;
}

//
// VECTOR SPECIALIZATION
//

template<>
class vector3D<binary32> : public c_svector3D {
 public :
  typedef binary32 value_type;
 public :
  vector3D();
  vector3D(binary32 x, binary32 y, binary32 z);
  explicit vector3D(const binary32* v);
  vector3D(const vector3D<binary32>& other);
  explicit vector3D(const vector3D<binary64>& other);
 public :
  binary32 x(void)const;
  binary32 y(void)const;
  binary32 z(void)const;
 public :
  void x(binary32 value);
  void y(binary32 value);
  void z(binary32 value);
  void reset(binary32 x, binary32 y, binary32 z);
  void reset(const binary32* v);
 public :
  binary32& operator [](int index);
  const binary32& operator [](int index)const;
 public :
  vector3D<binary32>& operator =(const vector3D<binary32>& other);
  template<class X> vector3D<binary32>& operator =(const vector3D<X>& other);
 public :
  vector3D<binary32>& operator +=(const vector3D<binary32>& other);
  vector3D<binary32>& operator -=(const vector3D<binary32>& other);
  vector3D<binary32>& operator *=(binary32 scalar);
  vector3D<binary32>& operator /=(binary32 scalar);
 public :
  template<class X> vector3D<binary32>& operator +=(const vector3D<X>& other);
  template<class X> vector3D<binary32>& operator -=(const vector3D<X>& other);
};

inline vector3D<binary32>::vector3D()
{
}

inline vector3D<binary32>::vector3D(binary32 x, binary32 y, binary32 z)
{
 this->v[0] = x;
 this->v[1] = y;
 this->v[2] = z;
}

inline vector3D<binary32>::vector3D(const binary32* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
 this->v[2] = v[2];
}

inline vector3D<binary32>::vector3D(const vector3D<binary32>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
}

inline binary32 vector3D<binary32>::x(void)const
{
 return this->v[0];
}

inline binary32 vector3D<binary32>::y(void)const
{
 return this->v[1];
}

inline binary32 vector3D<binary32>::z(void)const
{
 return this->v[2];
}

inline void vector3D<binary32>::x(binary32 value)
{
 this->v[0] = value;
}

inline void vector3D<binary32>::y(binary32 value)
{
 this->v[1] = value;
}

inline void vector3D<binary32>::z(binary32 value)
{
 this->v[2] = value;
}

inline void vector3D<binary32>::reset(binary32 x, binary32 y, binary32 z)
{
 this->v[0] = x;
 this->v[1] = y;
 this->v[2] = z;
}

inline void vector3D<binary32>::reset(const binary32* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
 this->v[2] = v[2];
}

inline binary32& vector3D<binary32>::operator [](int index)
{
 return this->v[index];
}

inline const binary32& vector3D<binary32>::operator [](int index)const
{
 return this->v[index];
}

inline vector3D<binary32>& vector3D<binary32>::operator =(const vector3D<binary32>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
 return *this;
}

template<class X>
inline vector3D<binary32>& vector3D<binary32>::operator =(const vector3D<X>& other)
{
 this->v[0] = static_cast<binary32>(other.v[0]);
 this->v[1] = static_cast<binary32>(other.v[1]);
 this->v[2] = static_cast<binary32>(other.v[2]);
 return *this;
}

inline vector3D<binary32>& vector3D<binary32>::operator +=(const vector3D<binary32>& other)
{
 this->v[0] += other.v[0];
 this->v[1] += other.v[1];
 this->v[2] += other.v[2];
 return *this;
}

inline vector3D<binary32>& vector3D<binary32>::operator -=(const vector3D<binary32>& other)
{
 this->v[0] -= other.v[0];
 this->v[1] -= other.v[1];
 this->v[2] -= other.v[2];
 return *this;
}

inline vector3D<binary32>& vector3D<binary32>::operator *=(binary32 scalar)
{
 this->v[0] *= scalar;
 this->v[1] *= scalar;
 this->v[2] *= scalar;
 return *this;
}

inline vector3D<binary32>& vector3D<binary32>::operator /=(binary32 scalar)
{
 this->v[0] /= scalar;
 this->v[1] /= scalar;
 this->v[2] /= scalar;
 return *this;
}

template<class X>
inline vector3D<binary32>& vector3D<binary32>::operator +=(const vector3D<X>& other)
{
 this->v[0] += static_cast<binary32>(other.v[0]);
 this->v[1] += static_cast<binary32>(other.v[1]);
 this->v[2] += static_cast<binary32>(other.v[2]);
 return *this;
}

template<class X>
inline vector3D<binary32>& vector3D<binary32>::operator -=(const vector3D<X>& other)
{
 this->v[0] -= static_cast<binary32>(other.v[0]);
 this->v[1] -= static_cast<binary32>(other.v[1]);
 this->v[2] -= static_cast<binary32>(other.v[2]);
 return *this;
}

//
// VECTOR SPECIALIZATION
//

template<>
class vector3D<binary64> : public c_dvector3D {
 public :
  typedef binary64 value_type;
 public :
  vector3D();
  vector3D(binary64 x, binary64 y, binary64 z);
  explicit vector3D(const binary64* v);
  vector3D(const vector3D<binary64>& other);
  explicit vector3D(const vector3D<binary32>& other);
 public :
  binary64 x(void)const;
  binary64 y(void)const;
  binary64 z(void)const;
 public :
  void x(binary64 value);
  void y(binary64 value);
  void z(binary64 value);
  void reset(binary64 x, binary64 y, binary64 z);
  void reset(const binary64* v);
 public :
  binary64& operator [](int index);
  const binary64& operator [](int index)const;
 public :
  vector3D<binary64>& operator =(const vector3D<binary64>& other);
  template<class X> vector3D<binary64>& operator =(const vector3D<X>& other);
 public :
  vector3D<binary64>& operator +=(const vector3D<binary64>& other);
  vector3D<binary64>& operator -=(const vector3D<binary64>& other);
  vector3D<binary64>& operator *=(binary64 scalar);
  vector3D<binary64>& operator /=(binary64 scalar);
 public :
  template<class X> vector3D<binary64>& operator +=(const vector3D<X>& other);
  template<class X> vector3D<binary64>& operator -=(const vector3D<X>& other);
};

inline vector3D<binary64>::vector3D()
{
}

inline vector3D<binary64>::vector3D(binary64 x, binary64 y, binary64 z)
{
 this->v[0] = x;
 this->v[1] = y;
 this->v[2] = z;
}

inline vector3D<binary64>::vector3D(const binary64* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
 this->v[2] = v[2];
}

inline vector3D<binary64>::vector3D(const vector3D<binary64>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
}

inline binary64 vector3D<binary64>::x(void)const
{
 return this->v[0];
}

inline binary64 vector3D<binary64>::y(void)const
{
 return this->v[1];
}

inline binary64 vector3D<binary64>::z(void)const
{
 return this->v[2];
}

inline void vector3D<binary64>::x(binary64 value)
{
 this->v[0] = value;
}

inline void vector3D<binary64>::y(binary64 value)
{
 this->v[1] = value;
}

inline void vector3D<binary64>::z(binary64 value)
{
 this->v[2] = value;
}

inline void vector3D<binary64>::reset(binary64 x, binary64 y, binary64 z)
{
 this->v[0] = x;
 this->v[1] = y;
 this->v[2] = z;
}

inline void vector3D<binary64>::reset(const binary64* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
 this->v[2] = v[2];
}

inline binary64& vector3D<binary64>::operator [](int index)
{
 return this->v[index];
}

inline const binary64& vector3D<binary64>::operator [](int index)const
{
 return this->v[index];
}

inline vector3D<binary64>& vector3D<binary64>::operator =(const vector3D<binary64>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 this->v[2] = other.v[2];
 return *this;
}

template<class X>
inline vector3D<binary64>& vector3D<binary64>::operator =(const vector3D<X>& other)
{
 this->v[0] = static_cast<binary64>(other.v[0]);
 this->v[1] = static_cast<binary64>(other.v[1]);
 this->v[2] = static_cast<binary64>(other.v[2]);
 return *this;
}

inline vector3D<binary64>& vector3D<binary64>::operator +=(const vector3D<binary64>& other)
{
 this->v[0] += other.v[0];
 this->v[1] += other.v[1];
 this->v[2] += other.v[2];
 return *this;
}

inline vector3D<binary64>& vector3D<binary64>::operator -=(const vector3D<binary64>& other)
{
 this->v[0] -= other.v[0];
 this->v[1] -= other.v[1];
 this->v[2] -= other.v[2];
 return *this;
}

inline vector3D<binary64>& vector3D<binary64>::operator *=(binary64 scalar)
{
 this->v[0] *= scalar;
 this->v[1] *= scalar;
 this->v[2] *= scalar;
 return *this;
}

inline vector3D<binary64>& vector3D<binary64>::operator /=(binary64 scalar)
{
 this->v[0] /= scalar;
 this->v[1] /= scalar;
 this->v[2] /= scalar;
 return *this;
}

template<class X>
inline vector3D<binary64>& vector3D<binary64>::operator +=(const vector3D<X>& other)
{
 this->v[0] += static_cast<binary64>(other.v[0]);
 this->v[1] += static_cast<binary64>(other.v[1]);
 this->v[2] += static_cast<binary64>(other.v[2]);
 return *this;
}

template<class X>
inline vector3D<binary64>& vector3D<binary64>::operator -=(const vector3D<X>& other)
{
 this->v[0] -= static_cast<binary64>(other.v[0]);
 this->v[1] -= static_cast<binary64>(other.v[1]);
 this->v[2] -= static_cast<binary64>(other.v[2]);
 return *this;
}

//
// EXPLICIT CONSTRUCTORS
//

inline vector3D<binary32>::vector3D(const vector3D<binary64>& other)
{
 this->v[0] = static_cast<binary32>(other.v[0]);
 this->v[1] = static_cast<binary32>(other.v[1]);
 this->v[2] = static_cast<binary32>(other.v[2]);
}

inline vector3D<binary64>::vector3D(const vector3D<binary32>& other)
{
 this->v[0] = static_cast<binary64>(other.v[0]);
 this->v[1] = static_cast<binary64>(other.v[1]);
 this->v[2] = static_cast<binary64>(other.v[2]);
}

//
// NON-MEMBER OPERATORS
//

template<class T>
inline vector3D<T> operator +(const vector3D<T>& lhs, const vector3D<T>& rhs)
{
 return vector3D<T>(
  lhs.v[0] + rhs.v[0],
  lhs.v[1] + rhs.v[1],
  lhs.v[2] + rhs.v[2]
 );
}

template<class T>
inline vector3D<T> operator -(const vector3D<T>& lhs, const vector3D<T>& rhs)
{
 return vector3D<T>(
  lhs.v[0] - rhs.v[0],
  lhs.v[1] - rhs.v[1],
  lhs.v[2] - rhs.v[2]
 );
}

template<class T>
inline vector3D<T> operator *(const vector3D<T>& lhs, const T& rhs)
{
 return vector3D<T>(
  lhs.v[0] * rhs,
  lhs.v[1] * rhs,
  lhs.v[2] * rhs
 );
}

template<class T>
inline vector3D<T> operator *(const T& lhs, const vector3D<T>& rhs)
{
 return vector3D<T>(
  rhs.v[0] * lhs,
  rhs.v[1] * lhs,
  rhs.v[2] * lhs
 );
}

template<class T>
inline vector3D<T> operator /(const vector3D<T>& lhs, const T& rhs)
{
 return vector3D<T>(
  lhs.v[0] / rhs,
  lhs.v[1] / rhs,
  lhs.v[2] / rhs,
  lhs.v[3] / rhs
 );
}

template<class T>
inline vector3D<T> operator +(const vector3D<T>& rhs)
{
 return rhs;
}

template<class T>
inline vector3D<T> operator -(const vector3D<T>& rhs)
{
 return vector3D<T>(-rhs.v[0], -rhs.v[1], -rhs.v[2]);
}

template<class T>
inline vector3D<T> operator ==(const vector3D<T>& lhs, const vector3D<T>& rhs)
{
 return ((lhs.v[0] == rhs.v[0]) &&
         (lhs.v[1] == rhs.v[1]) &&
         (lhs.v[2] == rhs.v[2]));
}

template<class T>
inline vector3D<T> operator !=(const vector3D<T>& lhs, const vector3D<T>& rhs)
{
 return !(lhs == rhs);
}

template<class T, class charT, class traits>
std::basic_ostream<charT, traits>& operator <<(std::basic_ostream<charT, traits>& os, const vector3D<T>& rhs)
{
 // copy
 std::basic_ostringstream<charT, traits> oss;
 oss.flags(os.flags());
 oss.imbue(os.getloc());
 oss.precision(os.precision());
 oss << '<' << rhs.v[0] << ',' << rhs.v[1] << ',' << rhs.v[2] << '>';

 // output to stream
 std::basic_string<charT, traits> str = oss.str();
 return (os << str.c_str());
}

//
// NON-MEMBER FUNCTIONS
//

template<class T>
inline T squared_norm(const vector3D<T>& v)
{
 return (v.v[0]*v.v[0]) + (v.v[1]*v.v[1]) + (v.v[2]*v.v[2]);
}

template<class T>
inline T norm(const vector3D<T>& v)
{
 return sqrt(squared_norm(v));
}

template<class T>
inline vector3D<T> unit(const vector3D<T>& v)
{
 T scalar = inv(norm(v));
 return vector3D<T>(v.v[0]*scalar, v.v[1]*scalar, v.v[2]*scalar);
}

template<class T>
inline void normalize(vector3D<T>& v)
{
 T scalar = inv(norm(v));
 v.v[0] *= scalar;
 v.v[1] *= scalar;
 v.v[2] *= scalar;
}

template<class T>
inline T length(const vector3D<T>& v)
{
 return sqrt(squared_norm(v));
}

template<class T>
inline T scalar_product(const vector3D<T>& v1, const vector3D<T>& v2)
{
 return (v1.v[0]*v2.v[0]) + (v1.v[1]*v2.v[1]) + (v1.v[2]*v2.v[2]);
}

template<class T>
inline vector3D<T> vector_product(const vector3D<T>& v1, const vector3D<T>& v2)
{
 return vector3D<T>(
  v1.v[1]*v2.v[2] - v1.v[2]*v2.v[1],
  v1.v[2]*v2.v[0] - v1.v[0]*v2.v[2],
  v1.v[0]*v2.v[1] - v1.v[1]*v2.v[0]
 );
}

template<class T>
inline T scalar_triple_product(const vector3D<T>& v0, const vector3D<T>& v1, const vector3D<T>& v2)
{
 return (
  (v1.v[1]*v2.v[2] - v1.v[2]*v2.v[1])*v0.v[0] +
  (v1.v[2]*v2.v[0] - v1.v[0]*v2.v[2])*v0.v[1] +
  (v1.v[0]*v2.v[1] - v1.v[1]*v2.v[0])*v0.v[2]
 );
}

//
// TYPEDEFS
//

typedef class vector3D<binary32> vector3D32;
typedef class vector3D<binary64> vector3D64;

}};

#endif