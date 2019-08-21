#ifndef __CS_MATH_VECTOR2D__
#define __CS_MATH_VECTOR2D__

// Standard C++ Headers
#include<istream>
#include<ostream>

// CS headers
#include<cs/math/functions.h>

namespace cs { namespace math {

//
// FORWARD DECLARATIONS
//

template<class T> class vector2D;
template<> class vector2D<binary32>;
template<> class vector2D<binary64>;

//
// BASE CLASSES
//

typedef struct _c_svector2D { binary32 v[2]; } c_svector2D;
typedef struct _c_dvector2D { binary64 v[2]; } c_dvector2D;
template<class T> struct c_tvector2D { T v[2]; };

//
// VECTOR
//

template<class T>
class vector2D : public c_tvector2D<T> {
 public :
  typedef T value_type;
 public :
  vector2D();
  vector2D(const T& x, const T& y);
  explicit vector2D(const T* v);
  vector2D(const vector2D<T>& other);
  template<class X> vector2D(const vector2D<X>& other);
 public :
  T x(void)const;
  T y(void)const;
 public :
  void x(const T& value);
  void y(const T& value);
  void reset(const T& x, const T& y);
  void reset(const T* v);
 public :
  T& operator [](int index);
  const T& operator [](int index)const;
 public :
  vector2D<T>& operator =(const vector2D<T>& other);
  template<class X> vector2D<T>& operator =(const vector2D<X>& other);
 public :
  vector2D<T>& operator +=(const vector2D<T>& other);
  vector2D<T>& operator -=(const vector2D<T>& other);
  vector2D<T>& operator *=(const T& scalar);
  vector2D<T>& operator /=(const T& scalar);
 public :
  template<class X> vector2D<T>& operator +=(const vector2D<X>& other);
  template<class X> vector2D<T>& operator -=(const vector2D<X>& other);
};

template<class T>
inline vector2D<T>::vector2D()
{
}

template<class T>
inline vector2D<T>::vector2D(const T& x, const T& y)
{
 this->v[0] = x;
 this->v[1] = y;
}

template<class T>
inline vector2D<T>::vector2D(const T* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
}

template<class T>
inline vector2D<T>::vector2D(const vector2D<T>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
}

template<class T> template<class X>
inline vector2D<T>::vector2D(const vector2D<X>& other)
{
 this->v[0] = static_cast<T>(other.v[0]);
 this->v[1] = static_cast<T>(other.v[1]);
}

template<class T>
inline T vector2D<T>::x(void)const
{
 return this->v[0];
}

template<class T>
inline T vector2D<T>::y(void)const
{
 return this->v[1];
}

template<class T>
inline void vector2D<T>::x(const T& value)
{
 this->v[0] = value;
}

template<class T>
inline void vector2D<T>::y(const T& value)
{
 this->v[1] = value;
}

template<class T>
inline void vector2D<T>::reset(const T& x, const T& y)
{
 this->v[0] = x;
 this->v[1] = y;
}

template<class T>
inline void vector2D<T>::reset(const T* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
}

template<class T>
inline T& vector2D<T>::operator [](int index)
{
 return this->v[index];
}

template<class T>
inline const T& vector2D<T>::operator [](int index)const
{
 return this->v[index];
}
  
template<class T>
inline vector2D<T>& vector2D<T>::operator =(const vector2D<T>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 return *this;
}

template<class T>
inline vector2D<T>& vector2D<T>::operator +=(const vector2D<T>& other)
{
 this->v[0] += other.v[0];
 this->v[1] += other.v[1];
 return *this;
}

template<class T>
inline vector2D<T>& vector2D<T>::operator -=(const vector2D<T>& other)
{
 this->v[0] -= other.v[0];
 this->v[1] -= other.v[1];
 return *this;
}
  
template<class T> template<class X>
inline vector2D<T>& vector2D<T>::operator =(const vector2D<X>& other)
{
 this->v[0] = static_cast<T>(other.v[0]);
 this->v[1] = static_cast<T>(other.v[1]);
 return *this;
}

template<class T>
inline vector2D<T>& vector2D<T>::operator *=(const T& scalar)
{
 this->v[0] *= scalar;
 this->v[1] *= scalar;
 return *this;
}

template<class T>
inline vector2D<T>& vector2D<T>::operator /=(const T& scalar)
{
 this->v[0] /= scalar;
 this->v[1] /= scalar;
 return *this;
}

template<class T> template<class X>
inline vector2D<T>& vector2D<T>::operator +=(const vector2D<X>& other)
{
 this->v[0] += static_cast<T>(other.v[0]);
 this->v[1] += static_cast<T>(other.v[1]);
 return *this;
}

template<class T> template<class X>
inline vector2D<T>& vector2D<T>::operator -=(const vector2D<X>& other)
{
 this->v[0] -= static_cast<T>(other.v[0]);
 this->v[1] -= static_cast<T>(other.v[1]);
 return *this;
}

//
// VECTOR SPECIALIZATION
//

template<>
class vector2D<binary32> : public c_svector2D {
 public :
  typedef binary32 value_type;
 public :
  vector2D();
  vector2D(binary32 x, binary32 y);
  explicit vector2D(const binary32* v);
  vector2D(const vector2D<binary32>& other);
  explicit vector2D(const vector2D<binary64>& other);
 public :
  binary32 x(void)const;
  binary32 y(void)const;
 public :
  void x(binary32 value);
  void y(binary32 value);
  void reset(binary32 x, binary32 y);
  void reset(const binary32* v);
 public :
  binary32& operator [](int index);
  const binary32& operator [](int index)const;
 public :
  vector2D<binary32>& operator =(const vector2D<binary32>& other);
  template<class X> vector2D<binary32>& operator =(const vector2D<X>& other);
 public :
  vector2D<binary32>& operator +=(const vector2D<binary32>& other);
  vector2D<binary32>& operator -=(const vector2D<binary32>& other);
  vector2D<binary32>& operator *=(binary32 scalar);
  vector2D<binary32>& operator /=(binary32 scalar);
 public :
  template<class X> vector2D<binary32>& operator +=(const vector2D<X>& other);
  template<class X> vector2D<binary32>& operator -=(const vector2D<X>& other);
};

inline vector2D<binary32>::vector2D()
{
}

inline vector2D<binary32>::vector2D(binary32 x, binary32 y)
{
 this->v[0] = x;
 this->v[1] = y;
}

inline vector2D<binary32>::vector2D(const binary32* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
}

inline vector2D<binary32>::vector2D(const vector2D<binary32>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
}

inline binary32 vector2D<binary32>::x(void)const
{
 return this->v[0];
}

inline binary32 vector2D<binary32>::y(void)const
{
 return this->v[1];
}

inline void vector2D<binary32>::x(binary32 value)
{
 this->v[0] = value;
}

inline void vector2D<binary32>::y(binary32 value)
{
 this->v[1] = value;
}

inline void vector2D<binary32>::reset(binary32 x, binary32 y)
{
 this->v[0] = x;
 this->v[1] = y;
}

inline void vector2D<binary32>::reset(const binary32* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
}

inline binary32& vector2D<binary32>::operator [](int index)
{
 return this->v[index];
}

inline const binary32& vector2D<binary32>::operator [](int index)const
{
 return this->v[index];
}

inline vector2D<binary32>& vector2D<binary32>::operator =(const vector2D<binary32>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 return *this;
}

template<class X>
inline vector2D<binary32>& vector2D<binary32>::operator =(const vector2D<X>& other)
{
 this->v[0] = static_cast<binary32>(other.v[0]);
 this->v[1] = static_cast<binary32>(other.v[1]);
 return *this;
}

inline vector2D<binary32>& vector2D<binary32>::operator +=(const vector2D<binary32>& other)
{
 this->v[0] += other.v[0];
 this->v[1] += other.v[1];
 return *this;
}

inline vector2D<binary32>& vector2D<binary32>::operator -=(const vector2D<binary32>& other)
{
 this->v[0] -= other.v[0];
 this->v[1] -= other.v[1];
 return *this;
}

inline vector2D<binary32>& vector2D<binary32>::operator *=(binary32 scalar)
{
 this->v[0] *= scalar;
 this->v[1] *= scalar;
 return *this;
}

inline vector2D<binary32>& vector2D<binary32>::operator /=(binary32 scalar)
{
 this->v[0] /= scalar;
 this->v[1] /= scalar;
 return *this;
}

template<class X>
inline vector2D<binary32>& vector2D<binary32>::operator +=(const vector2D<X>& other)
{
 this->v[0] += static_cast<binary32>(other.v[0]);
 this->v[1] += static_cast<binary32>(other.v[1]);
 return *this;
}

template<class X>
inline vector2D<binary32>& vector2D<binary32>::operator -=(const vector2D<X>& other)
{
 this->v[0] -= static_cast<binary32>(other.v[0]);
 this->v[1] -= static_cast<binary32>(other.v[1]);
 return *this;
}

//
// VECTOR SPECIALIZATION
//

template<>
class vector2D<binary64> : public c_dvector2D {
 public :
  typedef binary64 value_type;
 public :
  vector2D();
  vector2D(binary64 x, binary64 y);
  explicit vector2D(const binary64* v);
  vector2D(const vector2D<binary64>& other);
  explicit vector2D(const vector2D<binary32>& other);
 public :
  binary64 x(void)const;
  binary64 y(void)const;
 public :
  void x(binary64 value);
  void y(binary64 value);
  void reset(binary64 x, binary64 y);
  void reset(const binary64* v);
 public :
  binary64& operator [](int index);
  const binary64& operator [](int index)const;
 public :
  vector2D<binary64>& operator =(const vector2D<binary64>& other);
  template<class X> vector2D<binary64>& operator =(const vector2D<X>& other);
 public :
  vector2D<binary64>& operator +=(const vector2D<binary64>& other);
  vector2D<binary64>& operator -=(const vector2D<binary64>& other);
  vector2D<binary64>& operator *=(binary64 scalar);
  vector2D<binary64>& operator /=(binary64 scalar);
 public :
  template<class X> vector2D<binary64>& operator +=(const vector2D<X>& other);
  template<class X> vector2D<binary64>& operator -=(const vector2D<X>& other);
};

inline vector2D<binary64>::vector2D()
{
}

inline vector2D<binary64>::vector2D(binary64 x, binary64 y)
{
 this->v[0] = x;
 this->v[1] = y;
}

inline vector2D<binary64>::vector2D(const binary64* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
}

inline vector2D<binary64>::vector2D(const vector2D<binary64>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
}

inline binary64 vector2D<binary64>::x(void)const
{
 return this->v[0];
}

inline binary64 vector2D<binary64>::y(void)const
{
 return this->v[1];
}

inline void vector2D<binary64>::x(binary64 value)
{
 this->v[0] = value;
}

inline void vector2D<binary64>::y(binary64 value)
{
 this->v[1] = value;
}

inline void vector2D<binary64>::reset(binary64 x, binary64 y)
{
 this->v[0] = x;
 this->v[1] = y;
}

inline void vector2D<binary64>::reset(const binary64* v)
{
 this->v[0] = v[0];
 this->v[1] = v[1];
}

inline binary64& vector2D<binary64>::operator [](int index)
{
 return this->v[index];
}

inline const binary64& vector2D<binary64>::operator [](int index)const
{
 return this->v[index];
}

inline vector2D<binary64>& vector2D<binary64>::operator =(const vector2D<binary64>& other)
{
 this->v[0] = other.v[0];
 this->v[1] = other.v[1];
 return *this;
}

template<class X>
inline vector2D<binary64>& vector2D<binary64>::operator =(const vector2D<X>& other)
{
 this->v[0] = static_cast<binary64>(other.v[0]);
 this->v[1] = static_cast<binary64>(other.v[1]);
 return *this;
}

inline vector2D<binary64>& vector2D<binary64>::operator +=(const vector2D<binary64>& other)
{
 this->v[0] += other.v[0];
 this->v[1] += other.v[1];
 return *this;
}

inline vector2D<binary64>& vector2D<binary64>::operator -=(const vector2D<binary64>& other)
{
 this->v[0] -= other.v[0];
 this->v[1] -= other.v[1];
 return *this;
}

inline vector2D<binary64>& vector2D<binary64>::operator *=(binary64 scalar)
{
 this->v[0] *= scalar;
 this->v[1] *= scalar;
 return *this;
}

inline vector2D<binary64>& vector2D<binary64>::operator /=(binary64 scalar)
{
 this->v[0] /= scalar;
 this->v[1] /= scalar;
 return *this;
}

template<class X>
inline vector2D<binary64>& vector2D<binary64>::operator +=(const vector2D<X>& other)
{
 this->v[0] += static_cast<binary64>(other.v[0]);
 this->v[1] += static_cast<binary64>(other.v[1]);
 return *this;
}

template<class X>
inline vector2D<binary64>& vector2D<binary64>::operator -=(const vector2D<X>& other)
{
 this->v[0] -= static_cast<binary64>(other.v[0]);
 this->v[1] -= static_cast<binary64>(other.v[1]);
 return *this;
}

//
// EXPLICIT CONSTRUCTORS
//

inline vector2D<binary32>::vector2D(const vector2D<binary64>& other)
{
 this->v[0] = static_cast<binary32>(other.v[0]);
 this->v[1] = static_cast<binary32>(other.v[1]);
}

inline vector2D<binary64>::vector2D(const vector2D<binary32>& other)
{
 this->v[0] = static_cast<binary64>(other.v[0]);
 this->v[1] = static_cast<binary64>(other.v[1]);
}

//
// NON-MEMBER OPERATORS
//

template<class T>
inline vector2D<T> operator +(const vector2D<T>& lhs, const vector2D<T>& rhs)
{
 return vector2D<T>(
  lhs.v[0] + rhs.v[0],
  lhs.v[1] + rhs.v[1],
  lhs.v[2] + rhs.v[2]
 );
}

template<class T>
inline vector2D<T> operator -(const vector2D<T>& lhs, const vector2D<T>& rhs)
{
 return vector2D<T>(
  lhs.v[0] - rhs.v[0],
  lhs.v[1] - rhs.v[1],
  lhs.v[2] - rhs.v[2]
 );
}

template<class T>
inline vector2D<T> operator *(const vector2D<T>& lhs, const T& rhs)
{
 return vector2D<T>(
  lhs.v[0] * rhs,
  lhs.v[1] * rhs,
  lhs.v[2] * rhs
 );
}

template<class T>
inline vector2D<T> operator *(const T& lhs, const vector2D<T>& rhs)
{
 return vector2D<T>(
  rhs.v[0] * lhs,
  rhs.v[1] * lhs,
  rhs.v[2] * lhs
 );
}

template<class T>
inline vector2D<T> operator /(const vector2D<T>& lhs, const T& rhs)
{
 return vector2D<T>(
  lhs.v[0] / rhs,
  lhs.v[1] / rhs,
  lhs.v[2] / rhs,
  lhs.v[3] / rhs
 );
}

template<class T>
inline vector2D<T> operator +(const vector2D<T>& rhs)
{
 return rhs;
}

template<class T>
inline vector2D<T> operator -(const vector2D<T>& rhs)
{
 return vector2D<T>(-rhs.v[0], -rhs.v[1], -rhs.v[2]);
}

template<class T>
inline vector2D<T> operator ==(const vector2D<T>& lhs, const vector2D<T>& rhs)
{
 return ((lhs.v[0] == rhs.v[0]) &&
         (lhs.v[1] == rhs.v[1]) &&
         (lhs.v[2] == rhs.v[2]));
}

template<class T>
inline vector2D<T> operator !=(const vector2D<T>& lhs, const vector2D<T>& rhs)
{
 return !(lhs == rhs);
}

template<class T, class charT, class traits>
inline std::basic_ostream<charT, traits>& operator <<(std::basic_ostream<charT, traits>& os, const vector2D<T>& rhs)
{
 // copy
 std::basic_ostringstream<charT, traits> oss;
 oss.flags(os.flags());
 oss.imbue(os.getloc());
 oss.precision(os.precision());
 oss << '<' << rhs.v[0] << ',' << rhs.v[1] << '>';

 // output to stream
 std::basic_string<charT, traits> str = oss.str();
 return (os << str.c_str());
}

//
// NON-MEMBER FUNCTIONS
//

template<class T>
inline T squared_norm(const vector2D<T>& v)
{
 return (v.v[0]*v.v[0]) + (v.v[1]*v.v[1]);
}

template<class T>
inline T norm(const vector2D<T>& v)
{
 return sqrt(squared_norm(v));
}

template<class T>
inline vector2D<T> unit(const vector2D<T>& v)
{
 T scalar = inv(norm(v));
 return vector2D<T>(v.v[0]*scalar, v.v[1]*scalar);
}

template<class T>
inline void normalize(vector2D<T>& v)
{
 T scalar = inv(norm(v));
 v.v[0] *= scalar;
 v.v[1] *= scalar;
}

template<class T>
inline T length(const vector2D<T>& v)
{
 return sqrt(squared_norm(v));
}

template<class T>
inline T scalar_product(const vector2D<T>& v1, const vector2D<T>& v2)
{
 return (v1.v[0]*v2.v[0]) + (v1.v[1]*v2.v[1]);
}

//
// TYPEDEFS
//

typedef class vector2D<binary32> vector2D32;
typedef class vector2D<binary64> vector2D64;

}};

#endif