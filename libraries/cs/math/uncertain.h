#ifndef __CS_MATH_UNCERTAIN_H
#define __CS_MATH_UNCERTAIN_H

// Standard Includes

// CS Includes
#include<cs/math/functions.h>

//
// UNCERTAIN NUMBERS
//

namespace cs { namespace math {

template<class T>
struct uncertain_number {
 public :
  T value;
  T error;
 public :
  uncertain_number()
  {
  }
  explicit uncertain_number(const T& v, const T& u = T(0))
  {
   value = v;
   error = abs(u);
  }
 public :
  uncertain_number& operator +=(const uncertain_number& x)
  {
   (*this) = (*this) + x;
   return *this;
  }
  uncertain_number& operator -=(const uncertain_number& x)
  {
   (*this) = (*this) - x;
   return *this;
  }
  uncertain_number& operator *=(const uncertain_number& x)
  {
   (*this) = (*this) * x;
   return *this;
  }
  uncertain_number& operator /=(const uncertain_number& x)
  {
   (*this) = (*this) / x;
   return *this;
  }
 public :
  friend uncertain_number operator +(const uncertain_number& x, const uncertain_number& y)
  {
   uncertain_number result;
   result.value = x.value + y.value;
   result.error = x.error + y.error;
   return result;
  }
  friend uncertain_number operator -(const uncertain_number& x, const uncertain_number& y)
  {
   uncertain_number result;
   result.value = x.value - y.value;
   result.error = x.error + y.error;
   return result;
  }
  friend uncertain_number operator *(const uncertain_number& x, const uncertain_number& y)
  {
   uncertain_number result;
   result.value = x.value*y.value;
   result.error = abs(x.value)*y.error + x.error*(abs(y.value) + y.error);
   return result;
  }
  friend uncertain_number operator /(const uncertain_number& x, const uncertain_number& y)
  {
   T x_abs = abs(x.value);
   T y_abs = abs(y.value);
   uncertain_number result;
   result.value = x.value/y.value;
   result.error = (x.error*y_abs + y.error*x_abs)/abs((y_abs - y.error)*y_abs);
   return result;
  }
  friend uncertain_number inv(const uncertain_number& x)
  {
   uncertain_number y(constants<T>::one(), constants<T>::zero());
   return y/x;
  }
  friend uncertain_number sqrt(const uncertain_number& x)
  {
   uncertain_number result;
   result.value = sqrt(x.value);
   result.error = result.value - sqrt(x.value - x.error);
   return result;
  }
};

}}

#endif
