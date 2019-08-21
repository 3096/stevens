#include "xentax.h"

real32 float_16_to_32(uint16 value)
{
 // sign/exponent/mantissa
 const uint16 s = (value & 0x8000);
 const uint16 e = (value & 0x7C00) >> 10;
 const uint16 m = (value & 0x03FF);

 // ok
 const real32 sgn = (s ? -1.0f : 1.0f);
 if(e == 0) return sgn*(m == 0 ? 0.0f : std::pow(2.0f, -14.0f)*((real32)m/1024.0f));
 if(e < 32) return sgn*std::pow(2.0f, (real32)e - 15.0f)*(1.0f + ((real32)m/1024.0f));

 // not ok!
 if(m == 0) return std::numeric_limits<real32>::quiet_NaN();
 return std::numeric_limits<real32>::quiet_NaN();
}

bool read_string(std::istream& ifile, char* data, size_t size)
{ 
 return read_string(ifile, data, size, (char)0);
}

bool read_string(std::istream& ifile, char* data, size_t size, char delimiter)
{
 if(size == 0) return false;
 for(size_t curr = 0; ; curr++) {
     char c = (char)ifile.peek();
     if(ifile.fail() || ifile.eof()) return false;
     ifile.seekg(1, std::ios::cur);
     if(ifile.fail()) return false;
     if(c == delimiter) { data[curr] = '\0'; break; }
     else data[curr] = c;     
    }
 return true;
}

bool write_string(std::ofstream& ofile, const char* str)
{
 // length of string including null terminator (very important to include terminator!)
 uint32 elem = strlen(str) + 1;
 if(!elem) return false;

 // write string
 ofile.write(str, elem);
 if(ofile.fail()) return false;

 return true;
}

bool write_string(std::ofstream& ofile, const char* str, char delimiter)
{
 // length of string excluding null terminator
 uint32 elem = strlen(str);
 if(!elem) return false;

 // write string
 ofile.write(str, elem);
 if(ofile.fail()) return false;

 // write delimiter
 ofile.write(&delimiter, 1);
 if(ofile.fail()) return false;

 return true;
}

bool write_aligned_string_02(std::ofstream& ofile, const char* str)
{
 // length of string including null terminator (very important to include terminator!)
 uint32 elem = strlen(str) + 1;
 if(!elem) return false;

 // length of aligned string
 uint32 size = align02(elem);
 if(!size) return false;

 // copy string
 boost::shared_array<char> data(new char[size]);
 for(size_t i = 0; i < size; i++) data[i] = '\0';
 memmove(data.get(), str, elem);

 // write string
 ofile.write(data.get(), size);
 if(ofile.fail()) return false;

 return true;
}

bool write_aligned_string_04(std::ofstream& ofile, const char* str)
{
 // length of string including null terminator (very important to include terminator!)
 uint32 elem = strlen(str) + 1;
 if(!elem) return false;

 // length of aligned string
 uint32 size = align04(elem);
 if(!size) return false;

 // copy string
 boost::shared_array<char> data(new char[size]);
 for(size_t i = 0; i < size; i++) data[i] = '\0';
 memmove(data.get(), str, elem);

 // write string
 ofile.write(data.get(), size);
 if(ofile.fail()) return false;

 return true;
}

boost::shared_array<char> Unicode16ToUnicode08(LPCWSTR data)
{
 // validate string
 if((data == nullptr) || (*data == L'\0'))
    return boost::shared_array<char>();

 // number of characters in string
 const size_t limit = INT_MAX - 1;
 size_t n_chars = 0;
 HRESULT result = StringCchLengthW(data, limit, &n_chars);
 if(FAILED(result)) return boost::shared_array<char>();
 ++n_chars; // also consider null terminator

 // fail if invalid input character is encountered (Window Vista+)
 #if (WINVER >= 0x0600)
 DWORD flags = WC_ERR_INVALID_CHARS;
 #else
 DWORD flags = 0;
 #endif

 // determine size of destination buffer
 int size = WideCharToMultiByte(CP_UTF8, flags, data, static_cast<int>(n_chars), NULL, 0, NULL, NULL);
 if(!size) return boost::shared_array<char>();

 // allocate string and convert
 boost::shared_array<char> retval(new char[size]); 
 if(!WideCharToMultiByte(CP_UTF8, flags, data, static_cast<int>(n_chars), retval.get(), size, NULL, NULL))
    return boost::shared_array<char>();

 // return result
 return retval;
}

boost::shared_array<WCHAR> Unicode08ToUnicode16(LPCSTR data)
{
 // validate string
 if((data == nullptr) || (*data == L'\0'))
    return boost::shared_array<WCHAR>();

 // number of characters in string
 const size_t limit = INT_MAX - 1;
 size_t n_chars = 0;
 HRESULT result = StringCchLengthA(data, limit, &n_chars);
 if(FAILED(result)) return boost::shared_array<WCHAR>();
 ++n_chars; // also consider null terminator

 // determine size of destination buffer
 int size = MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(n_chars), NULL, 0);
 if(!size) return boost::shared_array<WCHAR>();

 // allocate string and convert
 boost::shared_array<WCHAR> retval(new WCHAR[size]); 
 if(!MultiByteToWideChar(CP_UTF8, 0, data, static_cast<int>(n_chars), retval.get(), size))
    return boost::shared_array<WCHAR>();

 return retval;
}

cs::math::vector3D<cs::binary32> ConvertNormal_S11S11S10(uint32 value)
{
 uint32 p1 = (value & 0xFFE00000ul) >> 21;
 uint32 p2 = (value & 0x001FFC00ul) >> 10;
 uint32 p3 = (value & 0x000003FFul);

 cs::binary32 d1 = static_cast<cs::binary32>(0x3FF);
 cs::binary32 d2 = d1;
 cs::binary32 d3 = static_cast<cs::binary32>(0x1FF);

 cs::binary32 r1;
 if(p1 & 0x400) r1 = -static_cast<cs::binary32>((0x400 - (p1 & 0x3FF)))/d1;
 else r1 = static_cast<cs::binary32>(p1)/d1;

 cs::binary32 r2;
 if(p2 & 0x400) r2 = -static_cast<cs::binary32>((0x400 - (p2 & 0x3FF)))/d2;
 else r2 = static_cast<cs::binary32>(p2)/d2;

 cs::binary32 r3;
 if(p3 & 0x200) r3 = -static_cast<cs::binary32>((0x200 - (p3 & 0x1FF)))/d3;
 else r3 = static_cast<cs::binary32>(p3)/d3;

 return cs::math::vector3D<cs::binary32>(r1, r2, r3);
}

cs::math::vector3D<cs::binary32> ConvertNormal_S11S10S11(uint32 value)
{
 uint32 p1 = (value & 0xFFE00000ul) >> 21;
 uint32 p2 = (value & 0x001FF800ul) >> 11;
 uint32 p3 = (value & 0x000007FFul);

 cs::binary32 d1 = static_cast<cs::binary32>(0x3FF);
 cs::binary32 d2 = static_cast<cs::binary32>(0x1FF);
 cs::binary32 d3 = d1;

 cs::binary32 r1;
 if(p1 & 0x400) r1 = -static_cast<cs::binary32>((0x400 - (p1 & 0x3FF)))/d1;
 else r1 = static_cast<cs::binary32>(p1)/d1;

 cs::binary32 r2;
 if(p2 & 0x200) r2 = -static_cast<cs::binary32>((0x200 - (p2 & 0x1FF)))/d2;
 else r2 = static_cast<cs::binary32>(p2)/d2;

 cs::binary32 r3;
 if(p3 & 0x400) r3 = -static_cast<cs::binary32>((0x400 - (p3 & 0x3FF)))/d3;
 else r3 = static_cast<cs::binary32>(p3)/d3;

 return cs::math::vector3D<cs::binary32>(r1, r2, r3);
}

cs::math::vector3D<cs::binary32> ConvertNormal_S10S11S11(uint32 value)
{
 uint32 p1 = (value & 0xFFC00000ul) >> 22;
 uint32 p2 = (value & 0x003FF800ul) >> 11;
 uint32 p3 = (value & 0x000007FFul);

 cs::binary32 d1 = static_cast<cs::binary32>(0x1FF);
 cs::binary32 d2 = static_cast<cs::binary32>(0x3FF);
 cs::binary32 d3 = d2;

 cs::binary32 r1;
 if(p1 & 0x200) r1 = -static_cast<cs::binary32>((0x200 - (p1 & 0x1FF)))/d1;
 else r1 = static_cast<cs::binary32>(p1)/d1;

 cs::binary32 r2;
 if(p2 & 0x400) r2 = -static_cast<cs::binary32>((0x400 - (p2 & 0x3FF)))/d2;
 else r2 = static_cast<cs::binary32>(p2)/d2;

 cs::binary32 r3;
 if(p3 & 0x400) r3 = -static_cast<cs::binary32>((0x400 - (p3 & 0x3FF)))/d3;
 else r3 = static_cast<cs::binary32>(p3)/d3;

 return cs::math::vector3D<cs::binary32>(r1, r2, r3);
}

