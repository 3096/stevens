#ifndef __CS_CORE_TYPES_H
#define __CS_CORE_TYPES_H

// Standard Headers
#include<cstddef>
#include<cstdint>
#include<cmath>
#include<complex>

// CS Headers
#include<cs/config.h>

//
// DATA TYPES
//
namespace cs {

// character types
typedef char schar08;
typedef unsigned char uchar08;

// UTF character types
typedef char16_t char16;
typedef char32_t char32;

// 8-bit integers
typedef std::int8_t sint08;
typedef std::uint8_t uint08;

// 16-bit integers
typedef std::int16_t sint16;
typedef std::uint16_t uint16;

// 32-bit integers
typedef std::int32_t sint32;
typedef std::uint32_t uint32;

// 64-bit integers
typedef std::int64_t sint64;
typedef std::uint64_t uint64;

// IEEE 754 formats
typedef float binary32;
typedef double binary64;

// complex numbers
typedef std::complex<binary32> complex32;
typedef std::complex<binary64> complex64;

}

//
// FUNDAMENTAL TYPE TRAITS
//
namespace cs {

// character types
template<class T> struct is_schar08 { static const bool value = false; };
template<class T> struct is_uchar08 { static const bool value = false; };
template<> struct is_schar08<schar08> { static const bool value = true; };
template<> struct is_uchar08<uchar08> { static const bool value = true; };

// character mixed types
template<class T> struct is_byte { static const bool value = false; };
template<> struct is_byte<schar08> { static const bool value = true; };
template<> struct is_byte<uchar08> { static const bool value = true; };

// UTF character types
template<class T> struct is_char16 { static const bool value = false; };
template<class T> struct is_char32 { static const bool value = false; };
template<> struct is_char16<char16> { static const bool value = true; };
template<> struct is_char32<char32> { static const bool value = true; };

// UTF mixed types
template<class T> struct is_utf { static const bool value = false; };
template<> struct is_utf<char16> { static const bool value = true; };
template<> struct is_utf<char32> { static const bool value = true; };

// signed integer types
template<class T> struct is_sint08 { static const bool value = false; };
template<class T> struct is_sint16 { static const bool value = false; };
template<class T> struct is_sint32 { static const bool value = false; };
template<class T> struct is_sint64 { static const bool value = false; };
template<> struct is_sint08<sint08> { static const bool value = true; };
template<> struct is_sint16<sint16> { static const bool value = true; };
template<> struct is_sint32<sint32> { static const bool value = true; };
template<> struct is_sint64<sint64> { static const bool value = true; };

// unsigned integer types
template<class T> struct is_uint08 { static const bool value = false; };
template<class T> struct is_uint16 { static const bool value = false; };
template<class T> struct is_uint32 { static const bool value = false; };
template<class T> struct is_uint64 { static const bool value = false; };
template<> struct is_uint08<uint08> { static const bool value = true; };
template<> struct is_uint16<uint16> { static const bool value = true; };
template<> struct is_uint32<uint32> { static const bool value = true; };
template<> struct is_uint64<uint64> { static const bool value = true; };

// binary types
template<class T> struct is_binary32 { static const bool value = false; };
template<class T> struct is_binary64 { static const bool value = false; };
template<> struct is_binary32<binary32> { static const bool value = true; };
template<> struct is_binary64<binary64> { static const bool value = true; };

// signed integral typed
template<class T> struct is_sint { static const bool value = false; };
template<> struct is_sint<sint08> { static const bool value = true; };
template<> struct is_sint<sint16> { static const bool value = true; };
template<> struct is_sint<sint32> { static const bool value = true; };
template<> struct is_sint<sint64> { static const bool value = true; };

// unsigned integral types
template<class T> struct is_uint { static const bool value = false; };
template<> struct is_uint<uint08> { static const bool value = true; };
template<> struct is_uint<uint16> { static const bool value = true; };
template<> struct is_uint<uint32> { static const bool value = true; };
template<> struct is_uint<uint64> { static const bool value = true; };

// integral types
template<class T> struct is_int { static const bool value = false; };
template<> struct is_int<sint08> { static const bool value = true; };
template<> struct is_int<sint16> { static const bool value = true; };
template<> struct is_int<sint32> { static const bool value = true; };
template<> struct is_int<sint64> { static const bool value = true; };
template<> struct is_int<uint08> { static const bool value = true; };
template<> struct is_int<uint16> { static const bool value = true; };
template<> struct is_int<uint32> { static const bool value = true; };
template<> struct is_int<uint64> { static const bool value = true; };

// binary types
template<class T> struct is_binary { static const bool value = false; };
template<> struct is_binary<binary32> { static const bool value = true; };
template<> struct is_binary<binary64> { static const bool value = true; };

// complex types
template<class T> struct is_complex { static const bool value = false; };
template<class T> struct is_complex32 { static const bool value = false; };
template<class T> struct is_complex64 { static const bool value = false; };
template<class T> struct is_complex<std::complex<T>> { static const bool value = true; };
template<> struct is_complex32<complex32> { static const bool value = true; };
template<> struct is_complex64<complex64> { static const bool value = true; };

}

#endif
