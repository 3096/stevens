#ifndef __CS_CORE_BINARY_TRAITS_H
#define __CS_CORE_BINARY_TRAITS_H

#include<cs/core/types.h>

namespace cs {

// trait classes
template<class T> struct binary_traits;
template<> struct binary_traits<binary32>;
template<> struct binary_traits<binary64>;

// only binary32 and binary64 have binary_traits
template<class T>
struct binary_traits {
 static_assert(is_binary<T>::value, "binary_traits cannot be instantiated with non-binary types.");
};

// binary32 traits
// SIGN BIT: 1 BIT
// EXPONENT WIDTH: 8 BITS
// MANTISSA WIDTH: 23 BITS
template<>
struct binary_traits<binary32> {
 public :
  typedef binary32 value_type;
  typedef uint32 integer_type;
 public :
  static integer_type reinterpret(value_type x)
  {
   return *reinterpret_cast<integer_type*>(&x);
  }
  static value_type reinterpret(integer_type x)
  {
   return *reinterpret_cast<value_type*>(&x);
  }
  static value_type reinterpret_binary16(uint16 x)
  {
   // sign, exponent, and mantissa
   const uint16 s = (x & 0x8000);
   const uint16 e = (x & 0x7C00) >> 10;
   const uint16 m = (x & 0x03FF);

   // zeroes and subnormals
   const value_type sgn = static_cast<value_type>(s ? -1 : 1);
   if(e == 0) {
      if(m == 0) return (s == 0 ? positive_zero() : negative_zero());
      return sgn*std::pow(static_cast<value_type>(2), static_cast<value_type>(-14))*(static_cast<value_type>(m)/static_cast<value_type>(1024));
     }
   // infinities and NaNs
   else if(e == 31) {
      if(m == 0) return (s == 0 ? positive_infinity() : negative_infinity());
      else if(m < 0x200) return (s == 0 ? positive_snan() : negative_snan());
      return (s == 0 ? positive_qnan() : negative_qnan());
     }

   // normal numbers
   value_type exponent = static_cast<value_type>(e) - static_cast<value_type>(15);
   return sgn*std::pow(static_cast<value_type>(2), exponent)*(static_cast<value_type>(1) + (static_cast<value_type>(m)/static_cast<value_type>(1024)));
  }
 public :
  static value_type spacing(value_type x)
  {
   if(is_nan(x) || is_infinity(x)) return x;
   integer_type y = reinterpret(x);
   integer_type z = y++;
   if(is_infinity(reinterpret(z))) return reinterpret(z);
   return (reinterpret(z) - reinterpret(y));
  }
  static value_type prev(value_type x)
  {
   return x; // TODO
  }
  static value_type next_largest(value_type x)
  {
   // special cases
   if(is_nan(x) || is_infinity(x)) return x;
   if(is_zero(x)) {
      integer_type temp = 0x00000001u;
      return reinterpret(temp);
     }
   // positive or negative numbers
   integer_type y = reinterpret(x);
   if(y < 0x80000000) return reinterpret(++y);
   return reinterpret(--y);
  }
 public :
  static value_type positive_zero(void)
  {
   // 0-00000000-00000000000000000000000
   integer_type temp = 0x00000000u;
   return reinterpret(temp);
  }
  static value_type negative_zero(void)
  {
   // 1-00000000-00000000000000000000000
   integer_type temp = 0x80000000u;
   return reinterpret(temp);
  }
  static value_type positive_infinity(void)
  {
   // 0-11111111-00000000000000000000000
   integer_type temp = 0x7F800000u;
   return reinterpret(temp);
  }
  static value_type negative_infinity(void)
  {
   // 1-11111111-00000000000000000000000
   integer_type temp = 0xFF800000u;
   return reinterpret(temp);
  }
  static value_type positive_snan(void)
  {
   // minimum: 0-11111111-00000000000000000000001 (0x7F800001)
   // maximum: 0-11111111-01111111111111111111111 (0x7FBFFFFF)
   integer_type temp = 0x7FBFFFFFu;
   return reinterpret(temp);
  }
  static value_type negative_snan(void)
  {
   // minimum: 1-11111111-00000000000000000000001 (0xFF800001)
   // maximum: 1-11111111-01111111111111111111111 (0xFFBFFFFF)
   integer_type temp = 0xFFBFFFFFu;
   return reinterpret(temp);
  }
  static value_type positive_qnan(void)
  {
   // minimum: 0-11111111-10000000000000000000000 (0x7FC00000)
   // maximum: 0-11111111-11111111111111111111111 (0x7FFFFFFF)
   integer_type temp = 0x7FFFFFFFu;
   return reinterpret(temp);
  }
  static value_type negative_qnan(void)
  {
   // minimum: 1-11111111-10000000000000000000000 (0xFFC00000)
   // maximum: 1-11111111-11111111111111111111111 (0xFFFFFFFF)
   integer_type temp = 0xFFFFFFFFu;
   return reinterpret(temp);
  }
 public :
  static bool is_zero(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0x00000000u) || (temp == 0x80000000u);
  }
  static bool is_positive_zero(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0x00000000u);
  }
  static bool is_negative_zero(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0x80000000u);
  }
  static bool is_infinity(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0x7F800000u) || (temp == 0xFF800000u);
  }
  static bool is_positive_infinity(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0x7F800000u);
  }
  static bool is_negative_infinity(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0xFF800000u);
  }
  static bool is_nan(value_type n)
  {
   // remove sign and all n must be greater than
   // 0-11111111-00000000000000000000000
   integer_type temp = reinterpret(n);
   return ((temp & 0x7FFFFFFFu) > 0x7F800000u);
  }
  static bool is_positive_snan(value_type n)
  {
   integer_type temp = reinterpret(n);
   return ((temp > 0x7F800000u) && (temp < 0x7FC00000u));
  }
  static bool is_negative_snan(value_type n)
  {
   integer_type temp = reinterpret(n);
   return ((temp > 0xFF800000u) && (temp < 0xFFC00000u));
  }
  static bool is_snan(value_type n)
  {
   integer_type temp = reinterpret(n) & 0x7FFFFFFFu;
   return ((temp > 0x7F800000u) && (temp < 0x7FC00000u));
  }
  static bool is_positive_qnan(value_type n)
  {
   integer_type temp = reinterpret(n);
   return ((temp > 0x7FBFFFFFu) && (temp < 0x80000000u));
  }
  static bool is_negative_qnan(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp > 0xFFBFFFFFu);
  }
  static bool is_qnan(value_type n)
  {
   // simply toggle sign bit and check for negative qnan
   integer_type temp = reinterpret(n) & 0x80000000u;
   return (temp > 0xFFBFFFFF);
  }
  static bool is_positive_subnormal(value_type n)
  {
   // minimum subnormal: 0-00000000-00000000000000000000001
   // maximum subnormal: 0-00000000-11111111111111111111111
   integer_type temp = reinterpret(n);
   return (temp > 0x00000000u) && (temp < 0x00800000u);
  }
  static bool is_negative_subnormal(value_type n)
  {
   // minimum subnormal: 1-00000000-00000000000000000000001
   // maximum subnormal: 1-00000000-11111111111111111111111
   integer_type temp = reinterpret(n);
   return (temp > 0x80000000u) && (temp < 0x80800000u);
  }
  static bool is_subnormal(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (((temp > 0x00000000u) && (temp < 0x00800000u)) || ((temp > 0x80000000u) && (temp < 0x80800000u)));
  }
  static bool is_positive_normal(value_type n)
  {
   // minimum normal: 0-00000001-00000000000000000000000
   // maximum normal: 0-11111110-11111111111111111111111
   integer_type temp = reinterpret(n);
   return (temp > 0x7FFFFFu) && (temp < 0x7F800000u);
  }
  static bool is_negative_normal(value_type n)
  {
   // minimum normal: 1-00000001-00000000000000000000000
   // maximum normal: 1-11111110-11111111111111111111111
   integer_type temp = reinterpret(n);
   return (temp > 0x807FFFFFu) && (temp < 0xFF800000u);
  }
  static bool is_normal(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (((temp > 0x7FFFFFu) && (temp < 0x7F800000u)) || ((temp > 0x807FFFFFu) && (temp < 0xFF800000u)));
  }
 public :
  static value_type epsilon(void) { const integer_type temp = 0x34000000ul; return reinterpret(temp); }
  static value_type epsilon_squared(void) { const integer_type temp = 0x28800000ul; return reinterpret(temp); }
  static value_type pi_error(void) { const integer_type temp = 0x33BBBD2Eul; return reinterpret(temp); }
};

// binary 64 traits
// SIGN BIT: 1 BIT
// EXPONENT WIDTH: 11 BITS
// MANTISSA WIDTH: 52 BITS
// 0-00000000000-0000000000000000000000000000000000000000000000000000
template<>
struct binary_traits<binary64> {
 public :
  typedef binary64 value_type;
  typedef uint64 integer_type;
 public :
  static integer_type reinterpret(value_type x)
  {
   return *reinterpret_cast<integer_type*>(&x);
  }
  static value_type reinterpret(integer_type x)
  {
   return *reinterpret_cast<value_type*>(&x);
  }
  static value_type reinterpret_binary16(uint16 x)
  {
   // sign, exponent, and mantissa
   const uint16 s = (x & 0x8000);
   const uint16 e = (x & 0x7C00) >> 10;
   const uint16 m = (x & 0x03FF);

   // zeroes and subnormals
   const value_type sgn = static_cast<value_type>(s ? -1 : 1);
   if(e == 0) {
      if(m == 0) return (s == 0 ? positive_zero() : negative_zero());
      return sgn*std::pow(static_cast<value_type>(2), static_cast<value_type>(-14))*(static_cast<value_type>(m)/static_cast<value_type>(1024));
     }
   // infinities and NaNs
   else if(e == 31) {
      if(m == 0) return (s == 0 ? positive_infinity() : negative_infinity());
      else if(m < 0x200) return (s == 0 ? positive_snan() : negative_snan());
      return (s == 0 ? positive_qnan() : negative_qnan());
     }

   // normal numbers
   value_type exponent = static_cast<value_type>(e) - static_cast<value_type>(15);
   return sgn*std::pow(static_cast<value_type>(2), exponent)*(static_cast<value_type>(1) + (static_cast<value_type>(m)/static_cast<value_type>(1024)));
  }
 public :
  static value_type positive_zero(void)
  {
   // 0-00000000000-0000000000000000000000000000000000000000000000000000
   integer_type temp = 0x0000000000000000ull;
   return reinterpret(temp);
  }
  static value_type negative_zero(void)
  {
   // 1-00000000000-0000000000000000000000000000000000000000000000000000
   integer_type temp = 0x8000000000000000ull;
   return reinterpret(temp);
  }
  static value_type positive_infinity(void)
  {
   // 0-11111111111-0000000000000000000000000000000000000000000000000000
   integer_type temp = 0x7FF0000000000000ull;
   return reinterpret(temp);
  }
  static value_type negative_infinity(void)
  {
   // 1-11111111111-0000000000000000000000000000000000000000000000000000
   integer_type temp = 0xFFF0000000000000ull;
   return reinterpret(temp);
  }
  static value_type positive_snan(void)
  {
   // minimum: 0-11111111111-0000000000000000000000000000000000000000000000000001 (0x7FF0000000000001)
   // maximum: 0-11111111111-0111111111111111111111111111111111111111111111111111 (0x7FF7FFFFFFFFFFFF)
   integer_type temp = 0x7FF7FFFFFFFFFFFFull;
   return reinterpret(temp);
  }
  static value_type negative_snan(void)
  {
   // minimum: 1-11111111111-0000000000000000000000000000000000000000000000000001 (0xFFF0000000000001)
   // maximum: 1-11111111111-0111111111111111111111111111111111111111111111111111 (0xFFF7FFFFFFFFFFFF)
   integer_type temp = 0xFFF7FFFFFFFFFFFFull;
   return reinterpret(temp);
  }
  static value_type positive_qnan(void)
  {
   // minimum: 0-11111111111-1000000000000000000000000000000000000000000000000000 (0x7FF8000000000000)
   // maximum: 0-11111111111-1111111111111111111111111111111111111111111111111111 (0x7FFFFFFFFFFFFFFF)
   integer_type temp = 0x7FFFFFFFFFFFFFFFull;
   return reinterpret(temp);
  }
  static value_type negative_qnan(void)
  {
   // minimum: 1-11111111111-1000000000000000000000000000000000000000000000000000 (0xFFF8000000000000)
   // maximum: 1-11111111111-1111111111111111111111111111111111111111111111111111 (0xFFFFFFFFFFFFFFFF)
   integer_type temp = 0xFFFFFFFFFFFFFFFFull;
   return reinterpret(temp);
  }
 public :
  static bool is_zero(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0x0000000000000000ull) || (temp == 0x8000000000000000ull);
  }
  static bool is_positive_zero(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0x0000000000000000ull);
  }
  static bool is_negative_zero(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0x8000000000000000ull);
  }
  static bool is_infinity(value_type n)
  {
   integer_type temp = reinterpret(n);
   return (temp == 0x7FF0000000000000ull) || (temp == 0xFFF0000000000000ull);
  }
  static bool is_positive_infinity(value_type n)
  {
   // 0-11111111111-0000000000000000000000000000000000000000000000000000 (0x7FF0000000000000)
   integer_type temp = reinterpret(n);
   return (temp == 0x7FF0000000000000ull);
  }
  static bool is_negative_infinity(value_type n)
  {
   // 1-11111111111-0000000000000000000000000000000000000000000000000000 (0xFFF0000000000000)
   integer_type temp = reinterpret(n);
   return (temp == 0xFFF0000000000000ull);
  }
  static bool is_nan(value_type n)
  {
   // get rid of sign, then all values must be greater than
   // 0-11111111111-0000000000000000000000000000000000000000000000000000 (0x7FF0000000000000)
   integer_type temp = reinterpret(n);
   return ((temp & 0x7FFFFFFFFFFFFFFFull) > 0x7FF0000000000000ull);
  }
  static bool is_positive_snan(value_type n)
  {
   // minimum: 0-11111111111-0000000000000000000000000000000000000000000000000001 (0x7FF0000000000001)
   // maximum: 0-11111111111-0111111111111111111111111111111111111111111111111111 (0x7FF7FFFFFFFFFFFF)
   integer_type temp = reinterpret(n);
   return ((temp > 0x7FF0000000000000ull) && (temp < 0x7FF8000000000000ull));
  }
  static bool is_negative_snan(value_type n)
  {
   // minimum: 1-11111111111-0000000000000000000000000000000000000000000000000001 (0xFFF0000000000001)
   // maximum: 1-11111111111-0111111111111111111111111111111111111111111111111111 (0xFFF7FFFFFFFFFFFF)
   integer_type temp = reinterpret(n);
   return ((temp > 0xFFF0000000000000ull) && (temp < 0xFFF8000000000000ull));
  }
  static bool is_snan(value_type n)
  {
   // strip sign and do positive test
   integer_type temp = reinterpret(n) & 0x7FFFFFFFFFFFFFFFull;
   return ((temp > 0x7FF0000000000000ull) && (temp < 0x7FF8000000000000ull));
  }
  static bool is_positive_qnan(value_type n)
  {
   // minimum: 0-11111111111-1000000000000000000000000000000000000000000000000000 (0x7FF8000000000000)
   // maximum: 0-11111111111-1111111111111111111111111111111111111111111111111111 (0x7FFFFFFFFFFFFFFF)
   integer_type temp = reinterpret(n);
   return ((temp > 0x7FF7FFFFFFFFFFFFull) && (temp < 0x8000000000000000ull));
  }
  static bool is_negative_qnan(value_type n)
  {
   // minimum: 1-11111111111-1000000000000000000000000000000000000000000000000000 (0xFFF8000000000000)
   // maximum: 1-11111111111-1111111111111111111111111111111111111111111111111111 (0xFFFFFFFFFFFFFFFF)
   integer_type temp = reinterpret(n);
   return (temp > 0xFFF7FFFFFFFFFFFFull);
  }
  static bool is_qnan(value_type n)
  {
   // simply toggle sign bit and check for negative qnan
   integer_type temp = reinterpret(n) | 0x8000000000000000ull;
   return (temp > 0xFFF7FFFFFFFFFFFFull);
  }
  static bool is_positive_subnormal(value_type n)
  {
   // minimum: 0-00000000000-0000000000000000000000000000000000000000000000000001 (0x0000000000000001)
   // maximum: 0-00000000000-1111111111111111111111111111111111111111111111111111 (0x000FFFFFFFFFFFFF)
   integer_type temp = reinterpret(n);
   return (temp > 0x0000000000000000ull) && (temp < 0x0010000000000000ull);
  }
  static bool is_negative_subnormal(value_type n)
  {
   // minimum: 1-00000000000-0000000000000000000000000000000000000000000000000001 (0x8000000000000001)
   // maximum: 1-00000000000-1111111111111111111111111111111111111111111111111111 (0x800FFFFFFFFFFFFF)
   integer_type temp = reinterpret(n);
   return (temp > 0x8000000000000000ull) && (temp < 0x8010000000000000ull);
  }
  static bool is_subnormal(value_type n)
  {
   // combination of both tests
   integer_type temp = reinterpret(n);
   return (((temp > 0x0000000000000000ull) && (temp < 0x0010000000000000ull)) ||
           ((temp > 0x8000000000000000ull) && (temp < 0x8010000000000000ull)));
  }
  static bool is_positive_normal(value_type n)
  {
   // minimum: 0-00000000001-0000000000000000000000000000000000000000000000000000 (0x0010000000000000)
   // maximum: 0-11111111110-1111111111111111111111111111111111111111111111111111 (0x7FEFFFFFFFFFFFFF)
   integer_type temp = reinterpret(n);
   return (temp > 0x000FFFFFFFFFFFFFull) && (temp < 0x7FF0000000000000ull);
  }
  static bool is_negative_normal(value_type n)
  {
   // minimum: 1-00000000001-0000000000000000000000000000000000000000000000000000 (0x8010000000000000)
   // maximum: 1-11111111110-1111111111111111111111111111111111111111111111111111 (0xFFEFFFFFFFFFFFFF)
   integer_type temp = reinterpret(n);
   return (temp > 0x800FFFFFFFFFFFFFull) && (temp < 0xFFF0000000000000ull);
  }
  static bool is_normal(value_type n)
  {
   // combination of both tests
   integer_type temp = reinterpret(n);
   return (((temp > 0x000FFFFFFFFFFFFFull) && (temp < 0x7FF0000000000000ull)) ||
           ((temp > 0x800FFFFFFFFFFFFFull) && (temp < 0xFFF0000000000000ull)));
  }
 public :
  static value_type epsilon(void) { const integer_type temp = 0x3CB0000000000000ull; return reinterpret(temp); }
  static value_type epsilon_squared(void) { const integer_type temp = 0x3D10000000000000ull; return reinterpret(temp); }
  static value_type pi_error(void) { const integer_type temp = 0x3CA1A62633145C07ull; return reinterpret(temp); }
};

// shortcuts
typedef binary_traits<binary32> binary32_traits;
typedef binary_traits<binary64> binary64_traits;

}

#endif
