#include "binary_stream.h"

namespace cs {

binary_stream::size_type binary_stream::search(const char* pattern, size_type p_len, size_t from)
{
 // pattern is longer than data
 if(elem < p_len) return npos;

 // initialize bad-character shifts
 std::unique_ptr<unsigned char[]> table(new unsigned char[256]);
 for(size_t i = 0; i < 256; i++) table[i] = p_len;

 // bad-character shifts
 const unsigned char* p_str = reinterpret_cast<const unsigned char*>(pattern);
 for(size_t i = 0; i < p_len - 1; i++) table[p_str[i]] = p_len - i - 1;

 // convert source string
 const unsigned char* s_str = reinterpret_cast<const unsigned char*>(data.get());
 size_type s_len = elem;

 // search for p_str in s_str
 size_t j = from;
 while(j <= (s_len - p_len)) {
       unsigned char c = s_str[j + p_len - 1];
       if(p_str[p_len - 1] == c && memcmp(p_str, s_str + j, p_len - 1) == 0) return j;
       j += table[c];
      }

 return npos;
}

//
// REAL16 ARRAY FUNCTIONS
// LITTLE ENDIAN
//

bool binary_stream::LE_read_real16_array(real32* ptr, size_type n)
{
 if(fail() || !ptr || !n) return false;
 size_type typesize = sizeof(uint16);
 size_type offset = n*typesize;
 if(!(position < npos - offset)) { position = npos; return false; }
 size_type newpos = position + offset;
 if(!can_seek(newpos)) { position = npos; return false; }
 #ifdef __SYS_LITTLE_ENDIAN
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #else
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     reverse_byte_order(&value);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #endif
 position = newpos;
 return true;
}

bool binary_stream::LE_read_real16_array(real64* ptr, size_type n)
{
 if(fail() || !ptr || !n) return false;
 size_type typesize = sizeof(uint16);
 size_type offset = n*typesize;
 if(!(position < npos - offset)) { position = npos; return false; }
 size_type newpos = position + offset;
 if(!can_seek(newpos)) { position = npos; return false; }
 #ifdef __SYS_LITTLE_ENDIAN
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #else
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     reverse_byte_order(&value);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #endif
 position = newpos;
 return true;
}

bool binary_stream::LE_read_real16_array(real32* ptr, size_type n, size_type pos)
{
 if(fail() || !ptr || !n) return false;
 size_type typesize = sizeof(uint16);
 size_type offset = n*typesize;
 if(!(pos < npos - offset)) { position = npos; return false; }
 size_type newpos = pos + offset;
 if(!can_seek(newpos)) { position = npos; return false; }
 #ifdef __SYS_LITTLE_ENDIAN
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #else
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     reverse_byte_order(&value);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #endif
 return true;
}

bool binary_stream::LE_read_real16_array(real64* ptr, size_type n, size_type pos)
{
 if(fail() || !ptr || !n) return false;
 size_type typesize = sizeof(uint16);
 size_type offset = n*typesize;
 if(!(pos < npos - offset)) { position = npos; return false; }
 size_type newpos = pos + offset;
 if(!can_seek(newpos)) { position = npos; return false; }
 #ifdef __SYS_LITTLE_ENDIAN
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #else
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     reverse_byte_order(&value);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #endif
 return true;
}

//
// REAL16 ARRAY FUNCTIONS
// BIG ENDIAN
//

bool binary_stream::BE_read_real16_array(real32* ptr, size_type n)
{
 if(fail() || !ptr || !n) return false;
 size_type typesize = sizeof(uint16);
 size_type offset = n*typesize;
 if(!(position < npos - offset)) { position = npos; return false; }
 size_type newpos = position + offset;
 if(!can_seek(newpos)) { position = npos; return false; }
 #ifdef __SYS_BIG_ENDIAN
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #else
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     reverse_byte_order(&value);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #endif
 position = newpos;
 return true;
}

bool binary_stream::BE_read_real16_array(real64* ptr, size_type n)
{
 if(fail() || !ptr || !n) return false;
 size_type typesize = sizeof(uint16);
 size_type offset = n*typesize;
 if(!(position < npos - offset)) { position = npos; return false; }
 size_type newpos = position + offset;
 if(!can_seek(newpos)) { position = npos; return false; }
 #ifdef __SYS_BIG_ENDIAN
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #else
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     reverse_byte_order(&value);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #endif
 position = newpos;
 return true;
}

bool binary_stream::BE_read_real16_array(real32* ptr, size_type n, size_type pos)
{
 if(fail() || !ptr || !n) return false;
 size_type typesize = sizeof(uint16);
 size_type offset = n*typesize;
 if(!(pos < npos - offset)) { position = npos; return false; }
 size_type newpos = pos + offset;
 if(!can_seek(newpos)) { position = npos; return false; }
 #ifdef __SYS_BIG_ENDIAN
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #else
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     reverse_byte_order(&value);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #endif
 return true;
}

bool binary_stream::BE_read_real16_array(real64* ptr, size_type n, size_type pos)
{
 if(fail() || !ptr || !n) return false;
 size_type typesize = sizeof(uint16);
 size_type offset = n*typesize;
 if(!(pos < npos - offset)) { position = npos; return false; }
 size_type newpos = pos + offset;
 if(!can_seek(newpos)) { position = npos; return false; }
 #ifdef __SYS_BIG_ENDIAN
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #else
 char* src = &(data.get()[position]);
 for(size_type i = 0; i < n; i++) {
     uint16 value = *reinterpret_cast<uint16*>(src);
     reverse_byte_order(&value);
     ptr[i] = binary16_to_32(value);
     src += typesize;
    }
 #endif
 return true;
}

//
// REAL16 ARRAY FUNCTIONS
// MODE ENDIAN
//

bool binary_stream::read_real16_array(real32* ptr, size_type n)
{
 if(mode == ENDIAN_LITTLE) return LE_read_real16_array(ptr, n);
 if(mode == ENDIAN_BIG) return BE_read_real16_array(ptr, n);
 #ifdef __SYS_LITTLE_ENDIAN
 return LE_read_real16_array(ptr, n);
 #else
 return BE_read_real16_array(ptr, n);
 #endif
}

bool binary_stream::read_real16_array(real64* ptr, size_type n)
{
 if(mode == ENDIAN_LITTLE) return LE_read_real16_array(ptr, n);
 if(mode == ENDIAN_BIG) return BE_read_real16_array(ptr, n);
 #ifdef __SYS_LITTLE_ENDIAN
 return LE_read_real16_array(ptr, n);
 #else
 return BE_read_real16_array(ptr, n);
 #endif
}

bool binary_stream::read_real16_array(real32* ptr, size_type n, size_type pos)
{
 if(mode == ENDIAN_LITTLE) return LE_read_real16_array(ptr, n);
 if(mode == ENDIAN_BIG) return BE_read_real16_array(ptr, n);
 #ifdef __SYS_LITTLE_ENDIAN
 return LE_read_real16_array(ptr, n);
 #else
 return BE_read_real16_array(ptr, n);
 #endif
}

bool binary_stream::read_real16_array(real64* ptr, size_type n, size_type pos)
{
 if(mode == ENDIAN_LITTLE) return LE_read_real16_array(ptr, n);
 if(mode == ENDIAN_BIG) return BE_read_real16_array(ptr, n);
 #ifdef __SYS_LITTLE_ENDIAN
 return LE_read_real16_array(ptr, n);
 #else
 return BE_read_real16_array(ptr, n);
 #endif
}

}