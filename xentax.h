#ifndef __XENTAX_H
#define __XENTAX_H

// Windows Headers
#define NOMINMAX
#pragma warning (disable : 4307)
#pragma warning (disable : 4995)
#pragma warning (disable : 4996)
#include<windows.h>
#include<windowsx.h>
#include<commctrl.h>
#include<shlobj.h>
#include<tchar.h>
#include<ddraw.h>
#include<atlbase.h>
#include<atlstr.h>
#include<strsafe.h>
#include<comdef.h>
#include<msxml6.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "msxml6.lib")

// Standard Headers
#ifndef RC_INVOKED
#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<deque>
#include<map>
#include<set>
#include<vector>
#include<string>
#endif

// Boost Headers
#ifndef RC_INVOKED
#include<boost/shared_array.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/bimap.hpp>
#include<boost/any.hpp>
#endif

// CS Headers
#ifndef RC_INVOKED
#include<cs/math/2D.h>
#include<cs/math/3D.h>
#include<cs/math/vector2D.h>
#include<cs/math/vector3D.h>
#include<cs/math/matrix4x4.h>
#include<cs/math/quaternion.h>
#include<cs/math/rotation.h>
#endif

// LZO Headers
#ifndef RC_INVOKED
#include "minilzo.h"
#endif

// ZLIB Headers
#ifndef RC_INVOKED
#include "libraries/zlib/zlib.h"
#endif

// Data Types
#ifndef RC_INVOKED
typedef __int8  sint08;
typedef __int16 sint16;
typedef __int32 sint32;
typedef __int64 sint64;
typedef unsigned __int8  uint08;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef float real32;
typedef double real64;
#endif

// TCHAR TYPES
#ifndef RC_INVOKED
typedef std::basic_string<TCHAR> STDSTRING;
typedef std::basic_stringstream<TCHAR> STDSTRINGSTREAM;
#endif

//
// DATA TYPE UTILITIES
//
#ifndef RC_INVOKED

template<class T> struct is_uint08 { static const bool value = false; };
template<class T> struct is_uint16 { static const bool value = false; };
template<class T> struct is_uint32 { static const bool value = false; };
template<> struct is_uint08<uint08> { static const bool value = true; };
template<> struct is_uint16<uint16> { static const bool value = true; };
template<> struct is_uint32<uint32> { static const bool value = true; };

enum endian_mode {
 ENDIAN_LITTLE = 0,
 ENDIAN_BIG = 1,
};

#endif

/*
** MESSAGE FUNCTIONS
*/
#ifndef RC_INVOKED

inline bool fail(void)
{
 std::cout << "Fail in " << __FILE__ << " at line " << __LINE__ << "." << std::endl;
 return false;
}

inline bool fail(const char* str)
{
 std::cout << "Line " << __LINE__ << ": " << str << std::endl;
 return false;
}

inline bool error(const char* str)
{
 std::cout << str << std::endl;
 return false;
}

inline bool error(const char* str, unsigned int linenum)
{
 std::cout << "Line " << linenum << ": " << str << std::endl;
 return false;
}

inline bool error(const std::string& str)
{
 std::cout << str.c_str() << std::endl;
 return false;
}

inline bool error(const std::string& str, unsigned int linenum)
{
 std::cout << "Line " << linenum << ": " << str << std::endl;
 return false;
}

template<class T>
inline bool error(const std::basic_stringstream<T>& ss)
{
 std::wcout << ss.str().c_str() << std::endl;
 return false;
}

template<class T>
inline bool error(const std::basic_stringstream<T>& ss, unsigned int linenum)
{
 std::cout << "Line " << linenum << ": " << ss.str().c_str() << std::endl;
 return false;
}

inline bool error(const char* fun, const char* str)
{
 std::cout << fun << ": " << str << std::endl;
 return false;
}

inline bool message(const char* str)
{
 std::cout << str << std::endl;
 return true;
}

inline bool YesNoBox(const char* message)
{
 return (MessageBoxA(0, message, "Question", MB_YESNO) == IDYES);
}

template<class T>
inline T clamp(T value, T v0, T v1) 
{
 if(value < v0) return v0;
 else if(v1 < value) return v1;
 return value;
}

inline real32 chop(real32 x)
{
 if(std::abs(x) < 1.0e-5f) return 0.0f;
 return x;
}

inline real64 chop(real64 x)
{
 if(std::abs(x) < 1.0e-5) return 0.0;
 return x;
}

inline real32 chop(real32 x, real32 epsilon)
{
 if(std::abs(x) < epsilon) return 0.0f;
 return x;
}

inline real64 chop(real64 x, real64 epsilon)
{
 if(std::abs(x) < epsilon) return 0.0;
 return x;
}

#endif

/*
** BYTE ORDER FUNCTIONS
*/
#ifndef RC_INVOKED

template<class T>
inline void reverse_byte_order(T* data)
{
 unsigned char* ptr = reinterpret_cast<unsigned char*>(data);
 std::reverse(ptr, ptr + sizeof(T)); 
}

template<class T>
inline void reverse_byte_order(T* data, size_t elem)
{
 for(size_t i = 0; i < elem; i++) {
     unsigned char* ptr = reinterpret_cast<unsigned char*>(&data[i]);
     std::reverse(ptr, ptr + sizeof(T));
    }
}

#endif

/*
** DATA CONVERSION FUNCTIONS
*/
#ifndef RC_INVOKED

real32 float_16_to_32(unsigned short value);

inline uint32 interpret_as_uint32(real32 x)
{
 return *(reinterpret_cast<uint32*>(&x));
}

inline uint64 interpret_as_uint64(real64 x)
{
 return *(reinterpret_cast<uint64*>(&x));
}

inline real32 interpret_as_real32(uint32 x)
{
 return *(reinterpret_cast<real32*>(&x));
}

inline real64 interpret_as_real64(uint64 x)
{
 return *(reinterpret_cast<real64*>(&x));
}

#endif

/*
** ENDIAN FUNCTIONS
*/
#ifndef RC_INVOKED

inline sint08 LE_read_sint08(std::istream& ifile)
{
 sint08 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline sint08 BE_read_sint08(std::istream& ifile)
{
 sint08 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline uint08 LE_read_uint08(std::istream& ifile)
{
 uint08 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline uint08 BE_read_uint08(std::istream& ifile)
{
 uint08 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline sint16 LE_read_sint16(std::istream& ifile)
{
 sint16 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline sint16 BE_read_sint16(std::istream& ifile)
{
 sint16 temp;
 ifile.read((char*)&temp, sizeof(temp));
 reverse_byte_order(&temp);
 return temp;
}

inline uint16 LE_read_uint16(std::istream& ifile)
{
 uint16 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline uint16 BE_read_uint16(std::istream& ifile)
{
 uint16 temp;
 ifile.read((char*)&temp, sizeof(temp));
 reverse_byte_order(&temp);
 return temp;
}

inline sint32 LE_read_sint32(std::istream& ifile)
{
 sint32 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline sint32 BE_read_sint32(std::istream& ifile)
{
 sint32 temp;
 ifile.read((char*)&temp, sizeof(temp));
 reverse_byte_order(&temp);
 return temp;
}

inline uint32 LE_read_uint32(std::istream& ifile)
{
 uint32 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline uint32 BE_read_uint32(std::istream& ifile)
{
 uint32 temp;
 ifile.read((char*)&temp, sizeof(temp));
 reverse_byte_order(&temp);
 return temp;
}

inline sint64 LE_read_sint64(std::istream& ifile)
{
 sint64 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline sint64 BE_read_sint64(std::istream& ifile)
{
 sint64 temp;
 ifile.read((char*)&temp, sizeof(temp));
 reverse_byte_order(&temp);
 return temp;
}

inline uint64 LE_read_uint64(std::istream& ifile)
{
 uint64 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline uint64 BE_read_uint64(std::istream& ifile)
{
 uint64 temp;
 ifile.read((char*)&temp, sizeof(temp));
 reverse_byte_order(&temp);
 return temp;
}

inline sint08 LE_read_sint08(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_sint08(ifile);
}

inline sint08 BE_read_sint08(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_sint08(ifile);
}

inline uint08 LE_read_uint08(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_uint08(ifile);
}

inline uint08 BE_read_uint08(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_uint08(ifile);
}

inline sint16 LE_read_sint16(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_sint16(ifile);
}

inline sint16 BE_read_sint16(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_sint16(ifile);
}

inline uint16 LE_read_uint16(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_uint16(ifile);
}

inline uint16 BE_read_uint16(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_uint16(ifile);
}

inline sint32 LE_read_sint32(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_sint32(ifile);
}

inline sint32 BE_read_sint32(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_sint32(ifile);
}

inline uint32 LE_read_uint32(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_uint32(ifile);
}

inline uint32 BE_read_uint32(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_uint32(ifile);
}

inline sint64 LE_read_sint64(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_sint64(ifile);
}

inline sint64 BE_read_sint64(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_sint64(ifile);
}

inline uint64 LE_read_uint64(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_uint64(ifile);
}

inline uint64 BE_read_uint64(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_uint64(ifile);
}

inline real32 LE_read_real16(std::istream& ifile)
{
 unsigned short temp;
 ifile.read((char*)&temp, sizeof(temp));
 return float_16_to_32(temp);
}

inline real32 BE_read_real16(std::istream& ifile)
{
 unsigned short temp = LE_read_uint16(ifile);
 reverse_byte_order(&temp);
 return float_16_to_32(temp);
}

inline real32 LE_read_real32(std::istream& ifile)
{
 real32 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline real32 BE_read_real32(std::istream& ifile)
{
 // it is very important to read data as a uint32 to avoid loss of precision
 // 0xffa8c744 -> real32 = NaN (incorrect)
 // 0xffa8c744 -> 0x44c7a8ff = 1597.281128 (correct)
 uint32 temp = LE_read_uint32(ifile);
 reverse_byte_order(&temp);
 return *reinterpret_cast<real32*>(&temp);
}

inline real64 LE_read_real64(std::istream& ifile)
{
 real64 temp;
 ifile.read((char*)&temp, sizeof(temp));
 return temp;
}

inline real64 BE_read_real64(std::istream& ifile)
{
 // it is very important to read data as a uint64 to avoid loss of precision
 uint64 temp = LE_read_uint64(ifile);
 reverse_byte_order(&temp);
 return *reinterpret_cast<real64*>(&temp);
}

inline real32 LE_read_real16(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_real16(ifile);
}

inline real32 BE_read_real16(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_real16(ifile);
}

inline real32 LE_read_real32(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_real32(ifile);
}

inline real32 BE_read_real32(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_real32(ifile);
}

inline real64 LE_read_real64(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return LE_read_real64(ifile);
}

inline real64 BE_read_real64(std::istream& ifile, unsigned int offset)
{
 ifile.seekg(offset);
 return BE_read_real64(ifile);
}

template<class T>
inline bool LE_read_array(std::istream& ifile, T* data, size_t n)
{
 ifile.read((char*)&data[0], n*sizeof(T));
 if(ifile.fail()) return false;
 return true;
}

template<class T>
inline bool BE_read_array(std::istream& ifile, T* data, size_t n)
{
 ifile.read((char*)&data[0], n*sizeof(T));
 if(ifile.fail()) return false;
 reverse_byte_order(data, n);
 return true;
}

inline bool BE_read_array(std::istream& ifile, real32* data, size_t n)
{
 uint32* temp = reinterpret_cast<uint32*>(data);
 ifile.read((char*)&temp[0], n*sizeof(uint32));
 if(ifile.fail()) return false;
 reverse_byte_order(temp, n);
 return true;
}

inline bool BE_read_array(std::istream& ifile, real64* data, size_t n)
{
 uint64* temp = reinterpret_cast<uint64*>(data);
 ifile.read((char*)&temp[0], n*sizeof(uint64));
 if(ifile.fail()) return false;
 reverse_byte_order(temp, n);
 return true;
}

template<class T>
inline bool LE_read_array(std::istream& ifile, unsigned int offset, T* data, unsigned int n)
{
 ifile.seekg(offset);
 if(ifile.fail()) return false;
 return LE_read_array(ifile, data, n);
}

template<class T>
inline bool BE_read_array(std::istream& ifile, unsigned int offset, T* data, unsigned int n)
{
 ifile.seekg(offset);
 if(ifile.fail()) return false;
 return BE_read_array(ifile, data, n);
}

inline void LE_write_uint08(std::ostream& ofile, uint08 value)
{
 ofile.write((char*)&value, sizeof(value));
}

inline void BE_write_uint08(std::ostream& ofile, uint08 value)
{
 reverse_byte_order(&value);
 ofile.write((char*)&value, sizeof(value));
}

inline void LE_write_uint16(std::ostream& ofile, uint16 value)
{
 ofile.write((char*)&value, sizeof(value));
}

inline void BE_write_uint16(std::ostream& ofile, uint16 value)
{
 reverse_byte_order(&value);
 ofile.write((char*)&value, sizeof(value));
}

inline void LE_write_uint32(std::ostream& ofile, uint32 value)
{
 ofile.write((char*)&value, sizeof(value));
}

inline void BE_write_uint32(std::ostream& ofile, uint32 value)
{
 reverse_byte_order(&value);
 ofile.write((char*)&value, sizeof(value));
}

inline void LE_write_uint64(std::ostream& ofile, uint64 value)
{
 ofile.write((char*)&value, sizeof(value));
}

inline void BE_write_uint64(std::ostream& ofile, uint64 value)
{
 reverse_byte_order(&value);
 ofile.write((char*)&value, sizeof(value));
}

inline void LE_write_real32(std::ostream& ofile, real32 value)
{
 ofile.write((char*)&value, sizeof(value));
}

inline void BE_write_real32(std::ostream& ofile, real32 value)
{
 reverse_byte_order(&value);
 ofile.write((char*)&value, sizeof(value));
}

template<class T>
inline bool LE_write_array(std::ostream& ofile, T* data, size_t n)
{
 // save data
 ofile.write((char*)&data[0], n*sizeof(T));
 if(ofile.fail()) return false;
 return true;
}

template<class T>
inline bool BE_write_array(std::ostream& ofile, T* data, size_t n)
{
 // copy and reverse data
 boost::shared_array<T> copy(new T[n]);
 for(size_t i = 0; i < n; i++) copy[i] = data[i];
 reverse_byte_order(copy.get(), n);
 // save data
 ofile.write((char*)&data[0], n*sizeof(T));
 if(ofile.fail()) return false;
 return true;
}

// TODO:
// WE NEED SPECIAL BE_write_array functions for real32 and real64 since the
// compiler probably won't like saving NaN values to a file!

#endif

/*
** DATA ALIGNMENT FUNCTIONS
*/
#ifndef RC_INVOKED

template<class T>
inline T align02(T value)
{
 return ((value + 0x01) & (~(0x01)));
}

template<class T>
inline T align04(T value)
{
 return ((value + 0x03) & (~(0x03)));
}

template<class T>
inline T align08(T value)
{
 return ((value + 0x07) & (~(0x07)));
}

template<class T>
inline T align16(T value)
{
 return ((value + 0x0F) & (~(0x0F)));
}

template<class T>
inline T align32(T value)
{
 return ((value + 0x1F) & (~(0x1F)));
}

template<class T>
inline T align64(T value)
{
 return ((value + 0x3F) & (~(0x3F)));
}

#endif

/*
** STRING FUNCTIONS
*/
#ifndef RC_INVOKED

bool read_string(std::istream& ifile, char* data, size_t size);
bool read_string(std::istream& ifile, char* data, size_t size, char delimiter);
bool write_string(std::ofstream& ofile, const char* str);
bool write_string(std::ofstream& ofile, const char* str, char delimiter);
bool write_aligned_string_02(std::ofstream& ofile, const char* str);
bool write_aligned_string_04(std::ofstream& ofile, const char* str);

inline boost::shared_array<char> string_to_shared_array(const std::string& str)
{
 if(str.length() == 0) return boost::shared_array<char>();
 boost::shared_array<char> retval(new char[str.length() + 1]);
 memmove(retval.get(), str.c_str(), str.length() + 1);
 return retval;
}

#endif

/*
** PRINT FUNCTIONS
*/
#ifndef RC_INVOKED

template<class T>
void print_array(std::ostream& os, const T* data, size_t n)
{
 os << "[";
 for(size_t i = 0; i < n - 1; i++) os << data[i] << ", ";
 os << data[n - 1] << "]";
}

template<class T>
void print_hex_array(std::ostream& os, const T* data, size_t n)
{
 uint32 w = 2*sizeof(T);
 os << "[";
 for(size_t i = 0; i < n - 1; i++) os << "0x" << setfill('0') << setw(w) << hex << data[i] << dec << ", ";
 os << "0x" << setfill('0') << setw(w) << hex << data[n - 1] << dec << "]";
}

#endif

/*
** MORTON COORDINATE FUNCTIONS
*/

inline uint32 dilate_2(uint32 r)
{
 r = (r & 0x0000FFFF);
 r = (r ^ (r << 8)) & 0x00FF00FF;
 r = (r ^ (r << 4)) & 0x0F0F0F0F;
 r = (r ^ (r << 2)) & 0x33333333;
 r = (r ^ (r << 1)) & 0x55555555;
 return r;
}

inline uint16 undilate_2(uint32 r)
{
 r = (r & 0x55555555);
 r = ((r ^ (r >> 1)) & 0x33333333);
 r = ((r ^ (r >> 2)) & 0x0F0F0F0F);
 r = ((r ^ (r >> 4)) & 0x00FF00FF);
 r = ((r ^ (r >> 8)) & 0x0000FFFF); 
 return static_cast<uint16>(r);
}

inline uint32 array_to_morton(uint32 row, uint32 col) 
{
 return (dilate_2(col) | (dilate_2(row) << 1));
}

inline uint16 morton_to_row(uint32 n)
{ 
 return undilate_2(n >> 1);
}

inline uint16 morton_to_col(uint32 n)
{ 
 return undilate_2(n);
}

//
// UNICODE FUNCTIONS
//

boost::shared_array<char> Unicode16ToUnicode08(LPCWSTR data);
boost::shared_array<WCHAR> Unicode08ToUnicode16(LPCSTR data);

//
// NORMAL DECODING FUNCTIONS
//

cs::math::vector3D<cs::binary32> ConvertNormal_S11S11S10(uint32 value);
cs::math::vector3D<cs::binary32> ConvertNormal_S11S10S11(uint32 value);
cs::math::vector3D<cs::binary32> ConvertNormal_S10S11S11(uint32 value);

#endif
