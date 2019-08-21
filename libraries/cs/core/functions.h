#ifndef __CS_CORE_FUNCTIONS_H
#define __CS_CORE_FUNCTIONS_H

// Standard Headers
#include<algorithm>

// CS Headers
#include<cs/core/types.h>
#include<cs/core/binary_traits.h>

//
// BYTE ORDER FUNCTIONS
//
namespace cs {

// single item
template<class T>
inline void reverse_byte_order(T* data)
{
 uint08* ptr = reinterpret_cast<uint08*>(data);
 std::reverse(ptr, ptr + sizeof(T)); 
}

// multiple items
template<class T>
inline void reverse_byte_order(T* data, size_t elem)
{
 for(size_t i = 0; i < elem; i++) {
     uint08* ptr = reinterpret_cast<uint08*>(&data[i]);
     std::reverse(ptr, ptr + sizeof(T));
    }
}

// specializations
template<class T> inline void reverse_byte_order(sint08* data) {}
template<class T> inline void reverse_byte_order(uint08* data) {}
template<class T> inline void reverse_byte_order(sint08* data, size_t elem) {}
template<class T> inline void reverse_byte_order(uint08* data, size_t elem) {}

}

//
// BINARY16 FUNCTIONS
//

namespace cs {

inline binary32 binary16_to_32(uint16 x)
{
 // sign, exponent, and mantissa
 const uint16 s = (x & 0x8000);
 const uint16 e = (x & 0x7C00) >> 10;
 const uint16 m = (x & 0x03FF);

 // zeroes and subnormals
 const binary32 sgn = static_cast<binary32>(s ? -1 : 1);
 if(e == 0) {
    if(m == 0) return (s == 0 ? binary_traits<binary32>::positive_zero() : binary_traits<binary32>::negative_zero());
    return sgn*std::pow(static_cast<binary32>(2), static_cast<binary32>(-14))*(static_cast<binary32>(m)/static_cast<binary32>(1024));
   }
 // infinities and NaNs
 else if(e == 31) {
    if(m == 0) return (s == 0 ? binary_traits<binary32>::positive_infinity() : binary_traits<binary32>::negative_infinity());
    else if(m < 0x200) return (s == 0 ? binary_traits<binary32>::positive_snan() : binary_traits<binary32>::negative_snan());
    return (s == 0 ? binary_traits<binary32>::positive_qnan() : binary_traits<binary32>::negative_qnan());
   }

 // normal numbers
 binary32 exponent = static_cast<binary32>(e) - static_cast<binary32>(15);
 return sgn*std::pow(static_cast<binary32>(2), exponent)*(static_cast<binary32>(1) + (static_cast<binary32>(m)/static_cast<binary32>(1024)));
}

}

//
// CLAMPING AND CHOPPING FUNCTIONS
//
namespace cs {

template<class T>
inline T clamp(const T& v, const T& a, const T& b)
{
 if(a < b) {
    if(v < a) return a;
    if(b < v) return b;
   }
 else {
    if(v < b) return b;
    if(a < v) return a;
   }
 return v;
}

template<class T, class BinaryPredicate>
inline T clamp(const T& v, const T& a, const T& b, BinaryPredicate p)
{
 if(p(a, b)) {
    if(p(v, a)) return a;
    if(p(b, v)) return b;
   }
 else {
    if(p(v, b)) return b;
    if(p(a, v)) return a;
   }
 return v;
}

template<class T>
inline T clamp(const T& v, const T& a, const T& b, const T& ra, const T& rb)
{
 if(a < b) {
    if(v < a) return ra;
    if(b < v) return rb;
   }
 else {
    if(v < b) return rb;
    if(a < v) return ra;
   }
 return v;
}

template<class T, class BinaryPredicate>
inline T clamp(const T& v, const T& a, const T& b, const T& ra, const T& rb, BinaryPredicate p)
{
 if(p(a, b)) {
    if(p(v, a)) return ra;
    if(p(b, v)) return rb;
   }
 else {
    if(p(v, b)) return rb;
    if(p(a, v)) return ra;
   }
 return v;
}

template<class T>
inline T chop(const T& v, distance_type_t<T> ep = binary_traits<distance_type_t<T>>::epsilon())
{
 T zero = binary_traits<distance_type_t<T>>::positive_zero();
 if(std::abs(v) < ep) return T(zero);
 return v;
}

}

#endif
