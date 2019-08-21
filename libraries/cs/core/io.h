#ifndef __CS_CORE_IO_H
#define __CS_CORE_IO_H

// Standard Headers
#include<istream>
#include<ostream>
#include<memory>

// CS Headers
#include<cs/core/binary_traits.h>
#include<cs/core/byte_order.h>

//
// Reading
//
namespace cs {

// Binary Fundamental Types
template<class T> inline T LE_read(std::istream& is)
{
 T temp;
 is.read((char*)&temp, sizeof(temp));
 #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 reverse_byte_order(&temp);
 #endif
 return temp;
}
template<class T> inline T BE_read(std::istream& is)
{
 T temp;
 is.read((char*)&temp, sizeof(temp));
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 reverse_byte_order(&temp);
 #endif
 return temp;
}
template<class T> inline T SE_read(std::istream& is)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read<T>(is);
 #else
 return BE_read<T>(is);
 #endif
}
template<class T> inline T LE_read(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return T();
 return LE_read<T>(is);
}
template<class T> inline T BE_read(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return T();
 return BE_read<T>(is);
}
template<class T> inline T SE_read(std::istream& is, std::streampos offset)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read<T>(is, offset);
 #else
 return BE_read<T>(is, offset);
 #endif
}

// Binary Fundamental Types (Specializations)
template<> inline binary32 LE_read<binary32>(std::istream& is)
{
 uint32 temp = LE_read<uint32>(is); // read data as uint to avoid NaN conversion
 return binary_traits<binary32>::reinterpret(temp);
}
template<> inline binary32 BE_read<binary32>(std::istream& is)
{
 uint32 temp = BE_read<uint32>(is); // read data as uint to avoid NaN conversion
 return binary_traits<binary32>::reinterpret(temp);
}
template<> inline binary32 SE_read<binary32>(std::istream& is)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read<binary32>(is);
 #else
 return BE_read<binary32>(is);
 #endif
}
template<> inline binary64 LE_read<binary64>(std::istream& is)
{
 uint64 temp = LE_read<uint64>(is); // read data as uint to avoid NaN conversion
 return binary_traits<binary64>::reinterpret(temp);
}
template<> inline binary64 BE_read<binary64>(std::istream& is)
{
 uint64 temp = BE_read<uint64>(is); // read data as uint to avoid NaN conversion
 return binary_traits<binary64>::reinterpret(temp);
}
template<> inline binary64 SE_read<binary64>(std::istream& is)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read<binary64>(is);
 #else
 return BE_read<binary64>(is);
 #endif
}
template<> inline binary32 LE_read<binary32>(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return binary32();
 return LE_read<binary32>(is);
}
template<> inline binary32 BE_read<binary32>(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return binary32();
 return BE_read<binary32>(is);
}
template<> inline binary32 SE_read<binary32>(std::istream& is, std::streampos offset)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read<binary32>(is, offset);
 #else
 return BE_read<binary32>(is, offset);
 #endif
}
template<> inline binary64 LE_read<binary64>(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return binary64();
 return LE_read<binary64>(is);
}
template<> inline binary64 BE_read<binary64>(std::istream& is, std::streampos offset)
{
 is.seekg(offset);
 if(is.fail()) return binary64();
 return BE_read<binary64>(is);
}
template<> inline binary64 SE_read<binary64>(std::istream& is, std::streampos offset)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read<binary64>(is, offset);
 #else
 return BE_read<binary64>(is, offset);
 #endif
}

// Binary Arrays
template<class T> inline bool LE_read_array(std::istream& is, T* data, size_t n)
{
 is.read((char*)&data[0], n*sizeof(T));
 if(is.fail()) return false;
 #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 reverse_byte_order(data, n);
 #endif
 return true;
}
template<class T> inline bool BE_read_array(std::istream& is, T* data, size_t n)
{
 is.read((char*)&data[0], n*sizeof(T));
 if(is.fail()) return false;
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 reverse_byte_order(data, n);
 #endif
 return true;
}
template<class T> inline bool SE_read_array(std::istream& is, T* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_array(is, data, n);
 #else
 return BE_read_array(is, data, n);
 #endif
}
template<class T> inline bool LE_read_array(std::istream& is, std::streampos offset, T* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return LE_read_array(is, data, n);
}
template<class T> inline bool BE_read_array(std::istream& is, std::streampos offset, T* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return BE_read_array(is, data, n);
}
template<class T> inline bool SE_read_array(std::istream& is, std::streampos offset, T* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_array(is, data, n, offset);
 #else
 return BE_read_array(is, data, n, offset);
 #endif
}

// Binary Arrays (Specializations)
template<> inline bool LE_read_array(std::istream& is, binary32* data, size_t n)
{
 return LE_read_array(is, reinterpret_cast<uint32*>(data), n);
}
template<> inline bool BE_read_array(std::istream& is, binary32* data, size_t n)
{
 return BE_read_array(is, reinterpret_cast<uint32*>(data), n);
}
template<> inline bool SE_read_array(std::istream& is, binary32* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_array(is, data, n);
 #else
 return BE_read_array(is, data, n);
 #endif
}
template<> inline bool LE_read_array(std::istream& is, binary64* data, size_t n)
{
 return LE_read_array(is, reinterpret_cast<uint32*>(data), n);
}
template<> inline bool BE_read_array(std::istream& is, binary64* data, size_t n)
{
 return BE_read_array(is, reinterpret_cast<uint32*>(data), n);
}
template<> inline bool SE_read_array(std::istream& is, binary64* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_array(is, data, n);
 #else
 return BE_read_array(is, data, n);
 #endif
}
template<> inline bool LE_read_array(std::istream& is, std::streampos offset, binary32* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return LE_read_array(is, data, n);
}
template<> inline bool BE_read_array(std::istream& is, std::streampos offset, binary32* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return BE_read_array(is, data, n);
}
template<> inline bool SE_read_array(std::istream& is, std::streampos offset, binary32* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_array(is, offset, data, n);
 #else
 return BE_read_array(is, offset, data, n);
 #endif
}
template<> inline bool LE_read_array(std::istream& is, std::streampos offset, binary64* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return LE_read_array(is, data, n);
}
template<> inline bool BE_read_array(std::istream& is, std::streampos offset, binary64* data, size_t n)
{
 is.seekg(offset);
 if(is.fail()) return false;
 return BE_read_array(is, data, n);
}
template<> inline bool SE_read_array(std::istream& is, std::streampos offset, binary64* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_array(is, offset, data, n);
 #else
 return BE_read_array(is, offset, data, n);
 #endif
}

// Binary Byte Strings
inline size_t SE_read_string(std::istream& is, schar08* data, size_t size, schar08 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     schar08 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}
inline size_t LE_read_string(std::istream& is, schar08* data, size_t size, schar08 delimiter = 0)
{
 return SE_read_string(is, data, size, delimiter);
}
inline size_t BE_read_string(std::istream& is, schar08* data, size_t size, schar08 delimiter = 0)
{
 return SE_read_string(is, data, size, delimiter);
}
inline size_t SE_read_string(std::istream& is, uchar08* data, size_t size, uchar08 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     uchar08 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}
inline size_t LE_read_string(std::istream& is, uchar08* data, size_t size, uchar08 delimiter = 0)
{
 return SE_read_string(is, data, size, delimiter);
}
inline size_t BE_read_string(std::istream& is, uchar08* data, size_t size, uchar08 delimiter = 0)
{
 return SE_read_string(is, data, size, delimiter);
}
inline size_t SE_read_string(std::istream& is, std::streampos offset, schar08* data, size_t size, schar08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return SE_read_string(is, data, size, delimiter);
}
inline size_t LE_read_string(std::istream& is, std::streampos offset, schar08* data, size_t size, schar08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return LE_read_string(is, data, size, delimiter);
}
inline size_t BE_read_string(std::istream& is, std::streampos offset, schar08* data, size_t size, schar08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return BE_read_string(is, data, size, delimiter);
}
inline size_t SE_read_string(std::istream& is, std::streampos offset, uchar08* data, size_t size, uchar08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return SE_read_string(is, data, size, delimiter);
}
inline size_t LE_read_string(std::istream& is, std::streampos offset, uchar08* data, size_t size, uchar08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return LE_read_string(is, data, size, delimiter);
}
inline size_t BE_read_string(std::istream& is, std::streampos offset, uchar08* data, size_t size, uchar08 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return BE_read_string(is, data, size, delimiter);
}

// Binary Unicode Strings
inline size_t LE_read_unicode_string(std::istream& is, char16* data, size_t size, char16 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     char16 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
     reverse_byte_order(&item);
     #endif
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}
inline size_t BE_read_unicode_string(std::istream& is, char16* data, size_t size, char16 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     char16 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
     reverse_byte_order(&item);
     #endif
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}
inline size_t SE_read_unicode_string(std::istream& is, char16* data, size_t size, char16 delimiter = 0)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_unicode_string(is, data, size, delimiter);
 #else
 return BE_read_unicode_string(is, data, size, delimiter);
 #endif
}
inline size_t LE_read_unicode_string(std::istream& is, char32* data, size_t size, char32 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     char32 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
     reverse_byte_order(&item);
     #endif
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}
inline size_t BE_read_unicode_string(std::istream& is, char32* data, size_t size, char32 delimiter = 0)
{
 // nothing to do
 if(data == nullptr) return 0;
 if(size == 0) return 0;

 // for each
 for(size_t curr = 0; curr < size; curr++)
    {
     // read character
     char32 item;
     is.read((char*)&item, sizeof(item));

     // on failure, assume we read delimiter
     if(is.fail() || is.eof()) {
        data[curr] = delimiter;
        return (curr + 1);
       }

     // set character
     #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
     reverse_byte_order(&item);
     #endif
     data[curr] = item;
     if(item == delimiter) return (curr + 1);
    }

 // always terminate with delimiter
 data[size - 1] = delimiter;
 return size;
}
inline size_t SE_read_unicode_string(std::istream& is, char32* data, size_t size, char32 delimiter = 0)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_unicode_string(is, data, size, delimiter);
 #else
 return BE_read_unicode_string(is, data, size, delimiter);
 #endif
}

inline size_t LE_read_unicode_string(std::istream& is, std::streampos offset, char16* data, size_t size, char16 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return LE_read_unicode_string(is, data, size, delimiter);
}
inline size_t BE_read_unicode_string(std::istream& is, std::streampos offset, char16* data, size_t size, char16 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return BE_read_unicode_string(is, data, size, delimiter);
}
inline size_t SE_read_unicode_string(std::istream& is, std::streampos offset, char16* data, size_t size, char16 delimiter = 0)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_unicode_string(is, offset, data, size, delimiter);
 #else
 return BE_read_unicode_string(is, offset, data, size, delimiter);
 #endif
}
inline size_t LE_read_unicode_string(std::istream& is, std::streampos offset, char32* data, size_t size, char32 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return LE_read_unicode_string(is, data, size, delimiter);
}
inline size_t BE_read_unicode_string(std::istream& is, std::streampos offset, char32* data, size_t size, char32 delimiter = 0)
{
 is.seekg(offset);
 if(is.fail()) return 0;
 return BE_read_unicode_string(is, data, size, delimiter);
}
inline size_t SE_read_unicode_string(std::istream& is, std::streampos offset, char32* data, size_t size, char32 delimiter = 0)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_read_unicode_string(is, offset, data, size, delimiter);
 #else
 return BE_read_unicode_string(is, offset, data, size, delimiter);
 #endif
}

} // namespace cs

//
// Writing
//
namespace cs {

// Binary Fundamental Types
template<class T> inline void LE_write(std::ostream& os, T value)
{
 #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 reverse_byte_order(&value);
 #endif
 os.write((char*)&value, sizeof(value));
}
template<class T> inline void BE_write(std::ostream& os, T value)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 reverse_byte_order(&value);
 #endif
 os.write((char*)&value, sizeof(value));
}
template<class T> inline void SE_write(std::ostream& os, T value)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 LE_write(os, &value);
 #else
 BE_write(os, &value);
 #endif
}

// Binary Fundamental Types (Specializations)
template<> inline void LE_write(std::ostream& os, binary32 value)
{
 #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 uint32 rever = *reinterpret_cast<uint32*>(&value);  
 reverse_byte_order(&rever);
 os.write((char*)&rever, sizeof(rever));
 #else
 os.write((char*)&value, sizeof(value));
 #endif
}
template<> inline void BE_write(std::ostream& os, binary32 value)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 uint32 rever = *reinterpret_cast<uint32*>(&value);  
 reverse_byte_order(&rever);
 os.write((char*)&rever, sizeof(rever));
 #else
 os.write((char*)&value, sizeof(value));
 #endif
}
template<> inline void SE_write(std::ostream& os, binary32 value)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 LE_write(os, &value);
 #else
 BE_write(os, &value);
 #endif
}
template<> inline void LE_write(std::ostream& os, binary64 value)
{
 #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 uint64 rever = *reinterpret_cast<uint64*>(&value);  
 reverse_byte_order(&rever);
 os.write((char*)&rever, sizeof(rever));
 #else
 os.write((char*)&value, sizeof(value));
 #endif
}
template<> inline void BE_write(std::ostream& os, binary64 value)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 uint64 rever = *reinterpret_cast<uint64*>(&value);  
 reverse_byte_order(&rever);
 os.write((char*)&rever, sizeof(rever));
 #else
 os.write((char*)&value, sizeof(value));
 #endif
}
template<> inline void SE_write(std::ostream& os, binary64 value)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 LE_write(os, &value);
 #else
 BE_write(os, &value);
 #endif
}

// Binary Arrays
template<class T> inline bool LE_write_array(std::ostream& os, const T* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 os.write((char*)data.get(), n*sizeof(T));
 #elif(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 std::shared_ptr<T> copy(new T[n], std::default_delete<T[]>());
 for(size_t i = 0; i < n; i++) copy.get()[i] = data[i];
 reverse_byte_order(copy.get(), n);
 os.write((char*)copy.get(), n*sizeof(T));
 #else
 #error "__SYS_ENDIAN has not been defined."
 #endif
 if(os.fail()) return false;
 return true;
}
template<class T> inline bool BE_write_array(std::ostream& os, const T* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 std::shared_ptr<T> copy(new T[n], std::default_delete<T[]>());
 for(size_t i = 0; i < n; i++) copy.get()[i] = data[i];
 reverse_byte_order(copy.get(), n);
 os.write((char*)copy.get(), n*sizeof(T));
 #elif(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 os.write((char*)data.get(), n*sizeof(T));
 #else
 #error "__SYS_ENDIAN has not been defined."
 #endif
 if(os.fail()) return false;
 return true;
}
template<class T> inline bool SE_write_array(std::ostream& os, const T* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_write_array(os, data, n);
 #else
 return BE_write_array(os, data, n);
 #endif
}

// Binary Arrays (Specializations)
template<> inline bool LE_write_array(std::ostream& os, const binary32* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 os.write((char*)data, n*sizeof(binary32));
 #else
 std::shared_ptr<uint32> copy(new uint32[n], std::default_delete<uint32[]>());
 for(size_t i = 0; i < n; i++) copy.get()[i] = *reinterpret_cast<const uint32*>(&data[i]);
 reverse_byte_order(copy.get(), n);
 os.write((char*)copy.get(), n*sizeof(binary32));
 #endif
 if(os.fail()) return false;
 return true;
}
template<> inline bool BE_write_array(std::ostream& os, const binary32* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 os.write((char*)data, n*sizeof(binary32));
 #else
 std::shared_ptr<uint32> copy(new uint32[n], std::default_delete<uint32[]>());
 for(size_t i = 0; i < n; i++) copy.get()[i] = *reinterpret_cast<const uint32*>(&data[i]);
 reverse_byte_order(copy.get(), n);
 os.write((char*)copy.get(), n*sizeof(binary32));
 #endif
 if(os.fail()) return false;
 return true;
}
template<> inline bool SE_write_array(std::ostream& os, const binary32* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_write_array(os, data, n);
 #else
 return BE_write_array(os, data, n);
 #endif
}
template<> inline bool LE_write_array(std::ostream& os, const binary64* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 os.write((char*)data, n*sizeof(binary64));
 #else
 std::shared_ptr<uint64> copy(new uint64[n], std::default_delete<uint64[]>());
 for(size_t i = 0; i < n; i++) copy.get()[i] = *reinterpret_cast<const uint64*>(&data[i]);
 reverse_byte_order(copy.get(), n);
 os.write((char*)copy.get(), n*sizeof(binary64));
 #endif
 if(os.fail()) return false;
 return true;
}
template<> inline bool BE_write_array(std::ostream& os, const binary64* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 os.write((char*)data, n*sizeof(binary64));
 #else
 std::shared_ptr<uint64> copy(new uint64[n], std::default_delete<uint64[]>());
 for(size_t i = 0; i < n; i++) copy.get()[i] = *reinterpret_cast<const uint64*>(&data[i]);
 reverse_byte_order(copy.get(), n);
 os.write((char*)copy.get(), n*sizeof(binary64));
 #endif
 if(os.fail()) return false;
 return true;
}
template<> inline bool SE_write_array(std::ostream& os, const binary64* data, size_t n)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_write_array(os, data, n);
 #else
 return BE_write_array(os, data, n);
 #endif
}

// Binary Byte Strings
inline bool SE_write_string(std::ostream& os, const schar08* data, schar08 delimiter = 0)
{
 if(!data) return true; // nothing to write
 if(delimiter == 0) {
    size_t size = (std::char_traits<schar08>::length(data) + 1);
    os.write(reinterpret_cast<const char*>(data), size);
   }
 else {
   size_t index = 0;
   while(data[index++] != delimiter);
   os.write(reinterpret_cast<const char*>(data), index);
  }
 return !os.fail();
}
inline bool LE_write_string(std::ostream& os, const schar08* data, schar08 delimiter = 0)
{
 return SE_write_string(os, data, delimiter);
}
inline bool BE_write_string(std::ostream& os, const schar08* data, schar08 delimiter = 0)
{
 return SE_write_string(os, data, delimiter);
}
inline bool SE_write_string(std::ostream& os, const uchar08* data, uchar08 delimiter = 0)
{
 if(!data) return true; // nothing to write
 if(delimiter == 0) {
    size_t size = (std::char_traits<uchar08>::length(data) + 1);
    os.write(reinterpret_cast<const char*>(data), size);
   }
 else {
   size_t index = 0;
   while(data[index++] != delimiter);
   os.write(reinterpret_cast<const char*>(data), index);
  }
 return !os.fail();
}
inline bool LE_write_string(std::ostream& os, const uchar08* data, uchar08 delimiter = 0)
{
 return SE_write_string(os, data, delimiter);
}
inline bool BE_write_string(std::ostream& os, const uchar08* data, uchar08 delimiter = 0)
{
 return SE_write_string(os, data, delimiter);
}

// Binary UNICODE Strings
inline bool LE_write_unicode_string(std::ostream& os, const char16* data, char16 delimiter = 0)
{
 if(!data) return true; // nothing to write
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 if(delimiter == 0) {
    size_t elem = std::char_traits<char16>::length(data) + 1;
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 else {
    size_t elem = 0;
    while(data[elem++] != delimiter);
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 #else
 size_t elem = 0;
 if(delimiter == 0) elem = std::char_traits<char16>::length(data) + 1;
 else while(data[elem++] != delimiter);
 std::shared_ptr<char16> copy(new char16[elem], std::default_delete<char16[]>());
 std::copy(data, data + elem, copy.get());
 reverse_byte_order(copy.get(), elem);
 os.write(reinterpret_cast<const char*>(copy.get()), elem*sizeof(char16));
 #endif
 return !os.fail();
}
inline bool BE_write_unicode_string(std::ostream& os, const char16* data, char16 delimiter = 0)
{
 if(!data) return true; // nothing to write
 #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 if(delimiter == 0) {
    size_t elem = (std::char_traits<char16>::length(data) + 1);
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 else {
    size_t elem = 0;
    while(data[elem++] != delimiter);
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 #else
 size_t elem = 0;
 if(delimiter == 0) elem = std::char_traits<char16>::length(data) + 1;
 else while(data[elem++] != delimiter);
 std::shared_ptr<char16> copy(new char16[elem], std::default_delete<char16[]>());
 std::copy(data, data + elem, copy.get());
 reverse_byte_order(copy.get(), elem);
 os.write(reinterpret_cast<const char*>(copy.get()), elem*sizeof(char16));
 #endif
 return !os.fail();
}
inline bool SE_write_unicode_string(std::ostream& os, const char16* data, char16 delimiter = 0)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_write_unicode_string(os, data, delimiter);
 #else
 return BE_write_unicode_string(os, data, delimiter);
 #endif
}
inline bool LE_write_unicode_string(std::ostream& os, const char32* data, char32 delimiter = 0)
{
 if(!data) return true; // nothing to write
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 if(delimiter == 0) {
    size_t elem = std::char_traits<char32>::length(data) + 1;
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 else {
    size_t elem = 0;
    while(data[elem++] != delimiter);
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 #else
 size_t elem = 0;
 if(delimiter == 0) elem = std::char_traits<char32>::length(data) + 1;
 else while(data[elem++] != delimiter);
 std::shared_ptr<char32> copy(new char32[elem], std::default_delete<char32[]>());
 std::copy(data, data + elem, copy.get());
 reverse_byte_order(copy.get(), elem);
 os.write(reinterpret_cast<const char*>(copy.get()), elem*sizeof(char32));
 #endif
 return !os.fail();
}
inline bool BE_write_unicode_string(std::ostream& os, const char32* data, char32 delimiter = 0)
{
 if(!data) return true; // nothing to write
 #if(__SYS_ENDIAN == __SYS_BIG_ENDIAN)
 if(delimiter == 0) {
    size_t elem = std::char_traits<char32>::length(data) + 1;
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 else {
    size_t elem = 0;
    while(data[elem++] != delimiter);
    os.write(reinterpret_cast<const char*>(data), elem*sizeof(char16));
   }
 #else
 size_t elem = 0;
 if(delimiter == 0) elem = std::char_traits<char32>::length(data) + 1;
 else while(data[elem++] != delimiter);
 std::shared_ptr<char32> copy(new char32[elem], std::default_delete<char32[]>());
 std::copy(data, data + elem, copy.get());
 reverse_byte_order(copy.get(), elem);
 os.write(reinterpret_cast<const char*>(copy.get()), elem*sizeof(char32));
 #endif
 return !os.fail();
}
inline bool SE_write_unicode_string(std::ostream& os, const char32* data, char32 delimiter = 0)
{
 #if(__SYS_ENDIAN == __SYS_LITTLE_ENDIAN)
 return LE_write_unicode_string(os, data, delimiter);
 #else
 return BE_write_unicode_string(os, data, delimiter);
 #endif
}

} // namespace cs

//
// Alternate Syntax
//
namespace cs {

// Binary Character Reading
inline schar08 LE_read_schar08(std::istream& is) { return LE_read<schar08>(is); }
inline schar08 BE_read_schar08(std::istream& is) { return BE_read<schar08>(is); }
inline schar08 SE_read_schar08(std::istream& is) { return SE_read<schar08>(is); }
inline uchar08 LE_read_uchar08(std::istream& is) { return LE_read<uchar08>(is); }
inline uchar08 BE_read_uchar08(std::istream& is) { return BE_read<uchar08>(is); }
inline uchar08 SE_read_uchar08(std::istream& is) { return SE_read<uchar08>(is); }
inline schar08 LE_read_schar08(std::istream& is, std::streampos offset) { return LE_read<schar08>(is, offset); }
inline schar08 BE_read_schar08(std::istream& is, std::streampos offset) { return BE_read<schar08>(is, offset); }
inline schar08 SE_read_schar08(std::istream& is, std::streampos offset) { return SE_read<schar08>(is, offset); }
inline uchar08 LE_read_uchar08(std::istream& is, std::streampos offset) { return LE_read<uchar08>(is, offset); }
inline uchar08 BE_read_uchar08(std::istream& is, std::streampos offset) { return BE_read<uchar08>(is, offset); }
inline uchar08 SE_read_uchar08(std::istream& is, std::streampos offset) { return SE_read<uchar08>(is, offset); }

// Binary Character Writing
inline void LE_write_schar08(std::ostream& os, schar08 value) { LE_write(os, value); }
inline void BE_write_schar08(std::ostream& os, schar08 value) { BE_write(os, value); }
inline void SE_write_schar08(std::ostream& os, schar08 value) { SE_write(os, value); }
inline void LE_write_uchar08(std::ostream& os, uchar08 value) { LE_write(os, value); }
inline void BE_write_uchar08(std::ostream& os, uchar08 value) { BE_write(os, value); }
inline void SE_write_uchar08(std::ostream& os, uchar08 value) { SE_write(os, value); }

// Binary UNICODE Reading
inline char16 LE_read_char16(std::istream& is) { return LE_read<char16>(is); }
inline char16 BE_read_char16(std::istream& is) { return BE_read<char16>(is); }
inline char16 SE_read_char16(std::istream& is) { return SE_read<char16>(is); }
inline char32 LE_read_char32(std::istream& is) { return LE_read<char32>(is); }
inline char32 BE_read_char32(std::istream& is) { return BE_read<char32>(is); }
inline char32 SE_read_char32(std::istream& is) { return SE_read<char32>(is); }
inline char16 LE_read_char16(std::istream& is, std::streampos offset) { return LE_read<char16>(is, offset); }
inline char16 BE_read_char16(std::istream& is, std::streampos offset) { return BE_read<char16>(is, offset); }
inline char16 SE_read_char16(std::istream& is, std::streampos offset) { return SE_read<char16>(is, offset); }
inline char32 LE_read_char32(std::istream& is, std::streampos offset) { return LE_read<char32>(is, offset); }
inline char32 BE_read_char32(std::istream& is, std::streampos offset) { return BE_read<char32>(is, offset); }
inline char32 SE_read_char32(std::istream& is, std::streampos offset) { return SE_read<char32>(is, offset); }

// Binary UNICODE Writing
inline void LE_write_char16(std::ostream& os, char16 value) { LE_write(os, value); }
inline void BE_write_char16(std::ostream& os, char16 value) { BE_write(os, value); }
inline void SE_write_char16(std::ostream& os, char16 value) { SE_write(os, value); }
inline void LE_write_char32(std::ostream& os, char32 value) { LE_write(os, value); }
inline void BE_write_char32(std::ostream& os, char32 value) { BE_write(os, value); }
inline void SE_write_char32(std::ostream& os, char32 value) { SE_write(os, value); }

// Binary Integer Reading
inline sint08 LE_read_sint08(std::istream& is) { return LE_read<sint08>(is); }
inline sint08 BE_read_sint08(std::istream& is) { return BE_read<sint08>(is); }
inline sint08 SE_read_sint08(std::istream& is) { return SE_read<sint08>(is); }
inline uint08 LE_read_uint08(std::istream& is) { return LE_read<uint08>(is); }
inline uint08 BE_read_uint08(std::istream& is) { return BE_read<uint08>(is); }
inline uint08 SE_read_uint08(std::istream& is) { return SE_read<uint08>(is); }
inline sint16 LE_read_sint16(std::istream& is) { return LE_read<sint16>(is); }
inline sint16 BE_read_sint16(std::istream& is) { return BE_read<sint16>(is); }
inline sint16 SE_read_sint16(std::istream& is) { return SE_read<sint16>(is); }
inline uint16 LE_read_uint16(std::istream& is) { return LE_read<uint16>(is); }
inline uint16 BE_read_uint16(std::istream& is) { return BE_read<uint16>(is); }
inline uint16 SE_read_uint16(std::istream& is) { return SE_read<uint16>(is); }
inline sint32 LE_read_sint32(std::istream& is) { return LE_read<sint32>(is); }
inline sint32 BE_read_sint32(std::istream& is) { return BE_read<sint32>(is); }
inline sint32 SE_read_sint32(std::istream& is) { return SE_read<sint32>(is); }
inline uint32 LE_read_uint32(std::istream& is) { return LE_read<uint32>(is); }
inline uint32 BE_read_uint32(std::istream& is) { return BE_read<uint32>(is); }
inline uint32 SE_read_uint32(std::istream& is) { return SE_read<uint32>(is); }
inline sint64 LE_read_sint64(std::istream& is) { return LE_read<sint64>(is); }
inline sint64 BE_read_sint64(std::istream& is) { return BE_read<sint64>(is); }
inline sint64 SE_read_sint64(std::istream& is) { return SE_read<sint64>(is); }
inline uint64 LE_read_uint64(std::istream& is) { return LE_read<uint64>(is); }
inline uint64 BE_read_uint64(std::istream& is) { return BE_read<uint64>(is); }
inline uint64 SE_read_uint64(std::istream& is) { return SE_read<uint64>(is); }
inline sint08 LE_read_sint08(std::istream& is, std::streampos offset) { return LE_read<sint08>(is, offset); }
inline sint08 BE_read_sint08(std::istream& is, std::streampos offset) { return BE_read<sint08>(is, offset); }
inline sint08 SE_read_sint08(std::istream& is, std::streampos offset) { return SE_read<sint08>(is, offset); }
inline uint08 LE_read_uint08(std::istream& is, std::streampos offset) { return LE_read<uint08>(is, offset); }
inline uint08 BE_read_uint08(std::istream& is, std::streampos offset) { return BE_read<uint08>(is, offset); }
inline uint08 SE_read_uint08(std::istream& is, std::streampos offset) { return SE_read<uint08>(is, offset); }
inline sint16 LE_read_sint16(std::istream& is, std::streampos offset) { return LE_read<sint16>(is, offset); }
inline sint16 BE_read_sint16(std::istream& is, std::streampos offset) { return BE_read<sint16>(is, offset); }
inline sint16 SE_read_sint16(std::istream& is, std::streampos offset) { return SE_read<sint16>(is, offset); }
inline uint16 LE_read_uint16(std::istream& is, std::streampos offset) { return LE_read<uint16>(is, offset); }
inline uint16 BE_read_uint16(std::istream& is, std::streampos offset) { return BE_read<uint16>(is, offset); }
inline uint16 SE_read_uint16(std::istream& is, std::streampos offset) { return SE_read<uint16>(is, offset); }
inline sint32 LE_read_sint32(std::istream& is, std::streampos offset) { return LE_read<sint32>(is, offset); }
inline sint32 BE_read_sint32(std::istream& is, std::streampos offset) { return BE_read<sint32>(is, offset); }
inline sint32 SE_read_sint32(std::istream& is, std::streampos offset) { return SE_read<sint32>(is, offset); }
inline uint32 LE_read_uint32(std::istream& is, std::streampos offset) { return LE_read<uint32>(is, offset); }
inline uint32 BE_read_uint32(std::istream& is, std::streampos offset) { return BE_read<uint32>(is, offset); }
inline uint32 SE_read_uint32(std::istream& is, std::streampos offset) { return SE_read<uint32>(is, offset); }
inline sint64 LE_read_sint64(std::istream& is, std::streampos offset) { return LE_read<sint64>(is, offset); }
inline sint64 BE_read_sint64(std::istream& is, std::streampos offset) { return BE_read<sint64>(is, offset); }
inline sint64 SE_read_sint64(std::istream& is, std::streampos offset) { return SE_read<sint64>(is, offset); }
inline uint64 LE_read_uint64(std::istream& is, std::streampos offset) { return LE_read<uint64>(is, offset); }
inline uint64 BE_read_uint64(std::istream& is, std::streampos offset) { return BE_read<uint64>(is, offset); }
inline uint64 SE_read_uint64(std::istream& is, std::streampos offset) { return SE_read<uint64>(is, offset); }

// Binary Integer Writing
inline void LE_write_sint08(std::ostream& os, sint08 value) { LE_write(os, value); }
inline void BE_write_sint08(std::ostream& os, sint08 value) { BE_write(os, value); }
inline void SE_write_sint08(std::ostream& os, sint08 value) { SE_write(os, value); }
inline void LE_write_uint08(std::ostream& os, uint08 value) { LE_write(os, value); }
inline void BE_write_uint08(std::ostream& os, uint08 value) { BE_write(os, value); }
inline void SE_write_uint08(std::ostream& os, uint08 value) { SE_write(os, value); }
inline void LE_write_sint16(std::ostream& os, sint16 value) { LE_write(os, value); }
inline void BE_write_sint16(std::ostream& os, sint16 value) { BE_write(os, value); }
inline void SE_write_sint16(std::ostream& os, sint16 value) { SE_write(os, value); }
inline void LE_write_uint16(std::ostream& os, uint16 value) { LE_write(os, value); }
inline void BE_write_uint16(std::ostream& os, uint16 value) { BE_write(os, value); }
inline void SE_write_uint16(std::ostream& os, uint16 value) { SE_write(os, value); }
inline void LE_write_sint32(std::ostream& os, sint32 value) { LE_write(os, value); }
inline void BE_write_sint32(std::ostream& os, sint32 value) { BE_write(os, value); }
inline void SE_write_sint32(std::ostream& os, sint32 value) { SE_write(os, value); }
inline void LE_write_uint32(std::ostream& os, uint32 value) { LE_write(os, value); }
inline void BE_write_uint32(std::ostream& os, uint32 value) { BE_write(os, value); }
inline void SE_write_uint32(std::ostream& os, uint32 value) { SE_write(os, value); }
inline void LE_write_sint64(std::ostream& os, sint64 value) { LE_write(os, value); }
inline void BE_write_sint64(std::ostream& os, sint64 value) { BE_write(os, value); }
inline void SE_write_sint64(std::ostream& os, sint64 value) { SE_write(os, value); }
inline void LE_write_uint64(std::ostream& os, uint64 value) { LE_write(os, value); }
inline void BE_write_uint64(std::ostream& os, uint64 value) { BE_write(os, value); }
inline void SE_write_uint64(std::ostream& os, uint64 value) { SE_write(os, value); }

// Binary Real Reading
inline binary32 LE_read_real16(std::istream& is) { uint16 v = LE_read<uint16>(is); return binary_traits<binary32>::reinterpret_binary16(v); }
inline binary32 BE_read_real16(std::istream& is) { uint16 v = BE_read<uint16>(is); return binary_traits<binary32>::reinterpret_binary16(v); }
inline binary32 SE_read_real16(std::istream& is) { uint16 v = SE_read<uint16>(is); return binary_traits<binary32>::reinterpret_binary16(v); }
inline binary32 LE_read_real32(std::istream& is) { return LE_read<binary32>(is); }
inline binary32 BE_read_real32(std::istream& is) { return BE_read<binary32>(is); }
inline binary32 SE_read_real32(std::istream& is) { return SE_read<binary32>(is); }
inline binary64 LE_read_real64(std::istream& is) { return LE_read<binary64>(is); }
inline binary64 BE_read_real64(std::istream& is) { return BE_read<binary64>(is); }
inline binary64 SE_read_real64(std::istream& is) { return SE_read<binary64>(is); }
inline binary32 LE_read_real32(std::istream& is, std::streampos offset) { return LE_read<binary32>(is, offset); }
inline binary32 BE_read_real32(std::istream& is, std::streampos offset) { return BE_read<binary32>(is, offset); }
inline binary32 SE_read_real32(std::istream& is, std::streampos offset) { return SE_read<binary32>(is, offset); }
inline binary64 LE_read_real64(std::istream& is, std::streampos offset) { return LE_read<binary64>(is, offset); }
inline binary64 BE_read_real64(std::istream& is, std::streampos offset) { return BE_read<binary64>(is, offset); }
inline binary64 SE_read_real64(std::istream& is, std::streampos offset) { return SE_read<binary64>(is, offset); }

// Binary Real Writing
inline void LE_write_real32(std::ostream& os, binary32 value) { LE_write(os, value); }
inline void BE_write_real32(std::ostream& os, binary32 value) { BE_write(os, value); }
inline void SE_write_real32(std::ostream& os, binary32 value) { SE_write(os, value); }
inline void LE_write_real64(std::ostream& os, binary64 value) { LE_write(os, value); }
inline void BE_write_real64(std::ostream& os, binary64 value) { BE_write(os, value); }
inline void SE_write_real64(std::ostream& os, binary64 value) { SE_write(os, value); }

} // namespace cs

#endif
