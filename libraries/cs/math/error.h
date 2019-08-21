#ifndef __CS_MATH_ERROR_H
#define __CS_MATH_ERROR_H

#include<cs/core/types.h>
#include<cs/core/binary_traits.h>
#include<cs/math/functions.h>

//
// EPSILON FUNCTIONS
//
namespace cs { namespace math {

template<class T>
struct zero_test : public std::unary_function<T, bool> {
 result_type operator ()(argument_type x) { return equals_zero(x); }
};

template<class T>
struct zero_epsilon_test : public std::unary_function<T, bool> {
 result_type operator ()(argument_type x) { return (abs(x) < binary_traits<T>::epsilon()); }
};

// simple absolute error equality testing
template<class T>
struct absolute_error_equality_test : public std::unary_function<T, bool> {
 result_type operator ()(argument_type x, argument_type y) {
  return (x == y);
 }
 result_type operator ()(argument_type x, argument_type y, argument_type ex, argument_type ey) {
  return (x == y);
 }
};

// simple absolute error equality testing (binary32)
template<>
struct absolute_error_equality_test<binary32> : public std::unary_function<binary32, bool> {
 result_type operator ()(argument_type x, argument_type y) {
  return (abs(x - y) < binary_traits<binary32>::epsilon());
 }
 result_type operator ()(argument_type x, argument_type y, argument_type ex, argument_type ey) {
  return (abs(x - y) < binary_traits<binary32>::epsilon());
 }
};

// simple absolute error equality testing (binary64)
template<>
struct absolute_error_equality_test<binary64> : public std::unary_function<binary64, bool> {
 result_type operator ()(argument_type x, argument_type y) {
  return (abs(x - y) < binary_traits<binary64>::epsilon());
 }
 result_type operator ()(argument_type x, argument_type y, argument_type ex, argument_type ey) {
  return (abs(x - y) < binary_traits<binary64>::epsilon());
 }
};

// simple absolute error equality testing
template<class T>
struct upper_lower_boundary_equality_test : public std::unary_function<T, bool> {
 result_type operator ()(argument_type x, argument_type y, argument_type x_upper, argument_type x_lower, argument_type y_upper, argument_type y_lower) {
  T diff = x - y;
  T diff_upper = x_upper - y_upper;
  T diff_lower = x_lower - y_lower;
 cout << "diff = " << diff << endl;
 cout << "diff_upper = " << diff_upper << endl;
 cout << "diff_lower = " << diff_lower << endl;
  if(diff_lower < diff_upper) return ((diff <= diff_upper) && (diff_lower <= diff));
  return ((diff <= diff_lower) && (diff_upper <= diff));
 }
};

}}

//
// SIMPLE ERROR FUNCTIONS
// ADDITION, MULTIPLICATION, DIVISION
//
namespace cs { namespace math {

template<class T>
inline T mul_absolute_error(T x, T y, T dx, T dy)
{
 // exact formula for (z + dz) = (x + dx)*(y + dy)
 return dx*(y + dy) + x*dy;
}

template<class T>
inline T mul_absolute_average_error(T x, T y, T dx, T dy)
{
 // assumes dx*dy is small
 return (x*y)*(dx/x + dy/y);
}

template<class T>
inline T mul_absolute_stddev_error(T x, T y, T dx, T dy)
{
 // statistical error propagation
 T a = dx/x;
 T b = dy/y;
 return x*y*sqrt(a*a + b*b);
}

template<class T>
inline T mul_relative_error(T x, T y, T dx, T dy)
{
 // exact formula for dz = dx/x + dy/y + (dx*dy)/(x*y)
 T a = dx/x;
 T b = dy/y;
 return (a + b + a*b);
}

template<class T>
inline T mul_relative_average_error(T x, T y, T dx, T dy)
{
 // assumes dx*dy is small
 return (dx/x + dy/y);
}

template<class T>
inline T mul_relative_stddev_error(T x, T y, T dx, T dy)
{
 // statistical error propagation
 T a = dx/x;
 T b = dy/y;
 return sqrt(a*a + b*b);
}

template<class T>
inline T div_absolute_error(T x, T y, T dx, T dy)
{
 // exact formula for dz = (x + dx)/(y + dy) - z
 return (dx*y - dy*x)/((y + dy)*y);
}

template<class T>
inline T div_relative_error(T x, T y, T dx, T dy)
{
 // exact formula for dz = (x + dx)/(y + dy) - z
 return (dx*y - dy*x)/((y + dy)*x);
}

}}

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
