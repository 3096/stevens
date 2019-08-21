#ifndef __CS_CORE_BINARY_STREAM_H
#define __CS_CORE_BINARY_STREAM_H

// CS Headers
#include<cs/core/types.h>
#include<cs/core/functions.h>

namespace cs {

enum endian_mode {
 ENDIAN_SYSTEM = 0,
 ENDIAN_LITTLE = 1,
 ENDIAN_BIG    = 2,
};
enum string_alignment {
 STRING_ALIGNMENT_NONE,
 STRING_ALIGNMENT_02,
 STRING_ALIGNMENT_04,
 STRING_ALIGNMENT_08,
 STRING_ALIGNMENT_16,
 STRING_ALIGNMENT_32,
 STRING_ALIGNMENT_64,
};

class binary_stream {
 #pragma region PUBLIC_DATA
 public :
  typedef std::shared_ptr<char> data_type;
  typedef size_t size_type;
  typedef ptrdiff_t diff_type;
  static const size_type npos = std::numeric_limits<size_type>::max();
 #pragma endregion
 #pragma region PRIVATE_DATA
 private :
  data_type data;
  size_type elem;
  size_type position;
  endian_mode mode;
 #pragma endregion
 #pragma region CONSTRUCTORS
 public :
  binary_stream() {
   elem = 0;
   position = 0;
   mode = ENDIAN_SYSTEM;
  }
  explicit binary_stream(endian_mode m) {
   elem = 0;
   position = 0;
   mode = m;
  }
  binary_stream(const binary_stream& bs) {
   data = bs.data;
   elem = bs.elem;
   position = bs.position;
   mode = bs.mode;
  }
  binary_stream(const std::shared_ptr<char>& stream, size_type n, endian_mode m = ENDIAN_SYSTEM) {
   data = stream;
   elem = n;
   position = 0;
   mode = m;
  }
 ~binary_stream() {}
 #pragma endregion
 #pragma region OPERATORS
 public :
  char& operator [](size_t index) { return data.get()[index]; }
  const char& operator [](size_t index)const { return data.get()[index]; }
  binary_stream& operator =(const binary_stream& bs) {
   if(this == &bs) return *this;
   data = bs.data;
   elem = bs.elem;
   position = bs.position;
   mode = bs.mode;
   return *this;
  }
 #pragma endregion
 #pragma region GENERAL_MEMBER_FUNCTIONS
 public :
  size_type size(void)const { return elem; }
  const char* c_str(void)const { return data.get(); }
  const char* c_str(size_type pos)const { return data.get() + pos; }
  const char* c_pos(void)const { return data.get() + position; }
  size_type tell(void)const { return position; }
  bool at_begin(void)const { return position == 0; }
  bool at_end(void)const { return position == elem; }
  bool fail(void)const { return position == npos; }
  bool good(void)const { return position != npos; }
  endian_mode get_endian_mode(void)const { return mode; }
  void set_endian_mode(endian_mode m) { mode = m; }
 #pragma endregion
 #pragma region SEEKING_FUNCTIONS
 public :
  bool can_seek(size_type pos)const {
   if(position == npos || pos == npos) return false; // positions must be valid
   return !(elem < pos);
  }
  bool can_move(diff_type pos)const {
   if(position == npos || pos == npos) return false; // position/offset must be valid
   if(pos < 0) return !(position < (size_type)std::abs(pos));
   return !(elem < (position + pos));
  }
  void seek(size_type pos) {
   if(can_seek(pos)) position = pos;
   else position = npos;
  }
  void seek_reverse(size_type pos) {
   if(can_seek(pos)) position = elem - pos;
   else position = npos;
  }
  void move(diff_type pos) {
   if(pos == 0) return;
   else if(pos > 0) seek(position + pos);
   else if(pos < 0) {
      size_type off = (size_type)std::abs(pos);
      if(!(position < off)) position -= off;
      else position = npos;
     }
  }
  void move(diff_type pos, size_type from) {
   if(fail()) return;
   seek(from);
   if(good()) move(pos);
  }
  void reset(void) {
   position = 0;
  }
 #pragma endregion
 public :
  size_type search(const char* pattern, size_type n, size_t from = 0);
 #pragma region PUBLIC_READING_FUNCTIONS_(RAW)
 public :
  bool read(char* ptr, size_type n) {
   if(fail() || !ptr || !n) return false;
   size_type newpos = position + n;
   if(!can_move(n)) { position = npos; return false; }
   memmove(ptr, c_pos(), n);
   position = newpos;
   return true;
  }
  bool read(char* ptr, size_type n, size_type pos) {
   if(fail() || !ptr || !n) return false;
   if(!(pos < npos - n)) { position = npos; return false; }
   size_type newpos = pos + n;
   if(!can_seek(newpos)) { position = npos; return false; }
   memmove(ptr, c_str(pos), n);
   return true;
  }
  std::string read_string(void) {
   std::string retval;
   for(;;) {
       char c;
       read(&c, 1);
       if(fail()) break;
       if(c == '\0') break;
       retval += c;
      }
   return retval;
  }
  std::string read_string(enum string_alignment alignment) {
   std::string retval = read_string();
   if(alignment == STRING_ALIGNMENT_NONE) return retval;
   uint32 bytes_read = static_cast<uint32>(retval.length()) + 1;
   uint32 bytes_full = 0;
   if(alignment == STRING_ALIGNMENT_02) bytes_full = ((bytes_read + 0x01) & (~0x01));
   else if(alignment == STRING_ALIGNMENT_04) bytes_full = ((bytes_read + 0x03) & (~0x03));
   else if(alignment == STRING_ALIGNMENT_08) bytes_full = ((bytes_read + 0x07) & (~0x07));
   else if(alignment == STRING_ALIGNMENT_16) bytes_full = ((bytes_read + 0x0F) & (~0x0F));
   else if(alignment == STRING_ALIGNMENT_32) bytes_full = ((bytes_read + 0x1F) & (~0x1F));
   else if(alignment == STRING_ALIGNMENT_64) bytes_full = ((bytes_read + 0x3F) & (~0x3F));
   else return retval; // assume no alignment
   if(bytes_read < bytes_full) move(bytes_full - bytes_read);
   return retval;
  }
 #pragma endregion
 #pragma region PRIVATE_PEEKING_FUNCTIONS
 private :
  template<class T>
  T LE_peek_value(void)const {
   if(fail() || !(position < (npos - sizeof(T)))) return T();
   size_type newpos = position + sizeof(T);
   if(!can_seek(newpos)) return T(); // don't fail
   return *reinterpret_cast<const T*>(c_pos());
  }
  template<class T>
  T BE_peek_value(void)const {
   if(fail() || !(position < (npos - sizeof(T)))) return T();
   size_type newpos = position + sizeof(T);
   if(!can_seek(newpos)) return T(); // don't fail
   T r = *reinterpret_cast<const T*>(c_pos());
   reverse_byte_order(&r);
   return r;
  }
  template<class T>
  T LE_peek_value(size_type pos)const {
   if(fail() || !(pos < (npos - sizeof(T)))) return T();
   size_type newpos = pos + sizeof(T);
   if(!can_seek(newpos)) return T(); // don't fail
   return *reinterpret_cast<const T*>(c_str(pos));
  }
  template<class T>
  T BE_peek_value(size_type pos)const {
   if(fail() || !(pos < (npos - sizeof(T)))) return T();
   size_type newpos = pos + sizeof(T);
   if(!can_seek(newpos)) return T(); // don't fail
   T r = *reinterpret_cast<const T*>(c_str(pos));
   reverse_byte_order(&r);
   return r;
  }
  template<>
  real32 BE_peek_value<real32>(void)const {
   uint32 r = BE_peek_value<uint32>();   
   return *reinterpret_cast<const real32*>(&r);
  }
  template<>
  real64 BE_peek_value<real64>(void)const {
   uint64 r = BE_peek_value<uint64>();
   return *reinterpret_cast<const real64*>(&r);
  }
  template<>
  real32 BE_peek_value<real32>(size_type pos)const {
   uint32 r = BE_peek_value<uint32>(pos);   
   return *reinterpret_cast<const real32*>(&r);
  }
  template<>
  real64 BE_peek_value<real64>(size_type pos)const {
   uint64 r = BE_peek_value<uint64>(pos);
   return *reinterpret_cast<const real64*>(&r);
  }
  template<class T>
  T peek_value(void)const {
   if(mode == ENDIAN_LITTLE) return LE_peek_value<T>();
   if(mode == ENDIAN_BIG) return BE_peek_value<T>();
   #ifdef __SYS_LITTLE_ENDIAN
   return LE_peek_value<T>();
   #else
   return BE_peek_value<T>();
   #endif
  }
  template<class T>
  T peek_value(size_type pos)const {
   if(mode == ENDIAN_LITTLE) return LE_peek_value<T>(pos);
   if(mode == ENDIAN_BIG) return BE_peek_value<T>(pos);
   #ifdef __SYS_LITTLE_ENDIAN
   return LE_peek_value<T>(pos);
   #else
   return BE_peek_value<T>(pos);
   #endif
  }
 #pragma endregion
 #pragma region PRIVATE_READING_FUNCTIONS
 private :
  template<class T>
  bool LE_read_value(T& value) {
   if(fail() || !(position < (npos - sizeof(T)))) return false;
   size_type newpos = position + sizeof(T);
   if(!can_seek(newpos)) { position = npos; return false; }
   T r = *reinterpret_cast<const T*>(c_pos());
   position = newpos;
   value = r;
   return true;
  }
  template<class T>
  bool BE_read_value(T& value) {
   if(fail() || !(position < (npos - sizeof(T)))) return false;
   size_type newpos = position + sizeof(T);
   if(!can_seek(newpos)) { position = npos; return false; }
   T r = *reinterpret_cast<const T*>(c_pos());
   reverse_byte_order(&r);
   position = newpos;
   value = r;
   return true;
  }
  template<class T>
  bool LE_read_value(T& value, size_type pos) {
   if(fail() || !(pos < (npos - sizeof(T)))) return false;
   size_type newpos = pos + sizeof(T);
   if(!can_seek(newpos)) { position = npos; return false; }
   T r = *reinterpret_cast<const T*>(c_str(pos));
   position = newpos;
   value = r;
   return true;
  }
  template<class T>
  bool BE_read_value(T& value, size_type pos) {
   if(fail() || !(pos < (npos - sizeof(T)))) return false;
   size_type newpos = pos + sizeof(T);
   if(!can_seek(newpos)) { position = npos; return false; }
   T r = *reinterpret_cast<const T*>(c_str(pos));
   reverse_byte_order(&r);
   position = newpos;
   value = r;
   return true;
  }
 public :
  template<>
  bool BE_read_value(binary32& value) {
   uint32 r;
   if(!BE_read_value(r)) return false;
   value = *reinterpret_cast<const binary32*>(&r);
   return true;
  }
  template<>
  bool BE_read_value(binary64& value) {
   uint64 r;
   if(!BE_read_value(r)) return false;
   value = *reinterpret_cast<const binary64*>(&r);
   return true;
  }
  template<>
  bool BE_read_value(binary32& value, size_type pos) {
   uint32 r;
   if(!BE_read_value(r, pos)) return false;
   value = *reinterpret_cast<const binary32*>(&r);
   return true;
  }
  template<>
  bool BE_read_value(binary64& value, size_type pos) {
   uint64 r;
   if(!BE_read_value(r, pos)) return false;
   value = *reinterpret_cast<const binary64*>(&r);
   return true;
  }
 public :
  template<class T>
  T read_value(void) {
   T value;
   if(mode == ENDIAN_LITTLE) { LE_read_value(value); return value; }
   if(mode == ENDIAN_BIG) { BE_read_value(value); return value; }
   #ifdef __SYS_LITTLE_ENDIAN
   LE_read_value(value); return value;
   #else
   BE_read_value(value); return value;
   #endif
  }
  template<class T>
  T read_value(size_type pos) {
   T value;
   if(mode == ENDIAN_LITTLE) { LE_read_value(value, pos); return value; }
   if(mode == ENDIAN_BIG) { BE_read_value(value, pos); return value; }
   #ifdef __SYS_LITTLE_ENDIAN
   LE_read_value(value, pos); return value;
   #else
   BE_read_value(value, pos); return value;
   #endif
  }
 #pragma endregion
 #pragma region PUBLIC_PEEKING_FUNCTIONS_(LE)
 public :
  sint08 LE_peek_sint08(void) { return LE_peek_value<sint08>(); }
  uint08 LE_peek_uint08(void) { return LE_peek_value<uint08>(); }
  sint16 LE_peek_sint16(void) { return LE_peek_value<sint16>(); }
  uint16 LE_peek_uint16(void) { return LE_peek_value<uint16>(); }
  sint32 LE_peek_sint32(void) { return LE_peek_value<sint32>(); }
  uint32 LE_peek_uint32(void) { return LE_peek_value<uint32>(); }
  sint64 LE_peek_sint64(void) { return LE_peek_value<sint64>(); }
  uint64 LE_peek_uint64(void) { return LE_peek_value<uint64>(); }
  real32 LE_peek_real16(void) { return binary16_to_32(LE_peek_uint16()); }
  real32 LE_peek_real32(void) { return LE_peek_value<real32>(); }
  real64 LE_peek_real64(void) { return LE_peek_value<real64>(); }
 public :
  sint08 LE_peek_sint08(size_type pos) { return LE_peek_value<sint08>(pos); }
  uint08 LE_peek_uint08(size_type pos) { return LE_peek_value<uint08>(pos); }
  sint16 LE_peek_sint16(size_type pos) { return LE_peek_value<sint16>(pos); }
  uint16 LE_peek_uint16(size_type pos) { return LE_peek_value<uint16>(pos); }
  sint32 LE_peek_sint32(size_type pos) { return LE_peek_value<sint32>(pos); }
  uint32 LE_peek_uint32(size_type pos) { return LE_peek_value<uint32>(pos); }
  sint64 LE_peek_sint64(size_type pos) { return LE_peek_value<sint64>(pos); }
  uint64 LE_peek_uint64(size_type pos) { return LE_peek_value<uint64>(pos); }
  real32 LE_peek_real16(size_type pos) { return binary16_to_32(LE_peek_uint16(pos)); }
  real32 LE_peek_real32(size_type pos) { return LE_peek_value<real32>(pos); }
  real64 LE_peek_real64(size_type pos) { return LE_peek_value<real64>(pos); }
 #pragma endregion
 #pragma region PUBLIC_PEEKING_FUNCTIONS_(BE)
 public :
  sint08 BE_peek_sint08(void) { return BE_peek_value<sint08>(); }
  uint08 BE_peek_uint08(void) { return BE_peek_value<uint08>(); }
  sint16 BE_peek_sint16(void) { return BE_peek_value<sint16>(); }
  uint16 BE_peek_uint16(void) { return BE_peek_value<uint16>(); }
  sint32 BE_peek_sint32(void) { return BE_peek_value<sint32>(); }
  uint32 BE_peek_uint32(void) { return BE_peek_value<uint32>(); }
  sint64 BE_peek_sint64(void) { return BE_peek_value<sint64>(); }
  uint64 BE_peek_uint64(void) { return BE_peek_value<uint64>(); }
  real32 BE_peek_real16(void) { return binary16_to_32(BE_peek_uint16()); }
  real32 BE_peek_real32(void) { return BE_peek_value<binary32>(); }
  real64 BE_peek_real64(void) { return BE_peek_value<binary64>(); }
 public :
  sint08 BE_peek_sint08(size_type pos) { return BE_peek_value<sint08>(pos); }
  uint08 BE_peek_uint08(size_type pos) { return BE_peek_value<uint08>(pos); }
  sint16 BE_peek_sint16(size_type pos) { return BE_peek_value<sint16>(pos); }
  uint16 BE_peek_uint16(size_type pos) { return BE_peek_value<uint16>(pos); }
  sint32 BE_peek_sint32(size_type pos) { return BE_peek_value<sint32>(pos); }
  uint32 BE_peek_uint32(size_type pos) { return BE_peek_value<uint32>(pos); }
  sint64 BE_peek_sint64(size_type pos) { return BE_peek_value<sint64>(pos); }
  uint64 BE_peek_uint64(size_type pos) { return BE_peek_value<uint64>(pos); }
  real32 BE_peek_real16(size_type pos) { return binary16_to_32(BE_peek_uint16(pos)); }
  real32 BE_peek_real32(size_type pos) { return BE_peek_value<binary32>(pos); }
  real64 BE_peek_real64(size_type pos) { return BE_peek_value<binary64>(pos); }
 #pragma endregion
 #pragma region PUBLIC_PEEKING_FUNCTIONS_(MODE)
 public :
  sint08 peek_sint08(void) { return peek_value<sint08>(); }
  uint08 peek_uint08(void) { return peek_value<uint08>(); }
  sint16 peek_sint16(void) { return peek_value<sint16>(); }
  uint16 peek_uint16(void) { return peek_value<uint16>(); }
  sint32 peek_sint32(void) { return peek_value<sint32>(); }
  uint32 peek_uint32(void) { return peek_value<uint32>(); }
  sint64 peek_sint64(void) { return peek_value<sint64>(); }
  uint64 peek_uint64(void) { return peek_value<uint64>(); }
  real32 peek_real16(void) { return binary16_to_32(peek_uint16()); }
  real32 peek_real32(void) { return peek_value<real32>(); }
  real64 peek_real64(void) { return peek_value<real64>(); }
 public :
  sint08 peek_sint08(size_type pos) { return peek_value<sint08>(pos); }
  uint08 peek_uint08(size_type pos) { return peek_value<uint08>(pos); }
  sint16 peek_sint16(size_type pos) { return peek_value<sint16>(pos); }
  uint16 peek_uint16(size_type pos) { return peek_value<uint16>(pos); }
  sint32 peek_sint32(size_type pos) { return peek_value<sint32>(pos); }
  uint32 peek_uint32(size_type pos) { return peek_value<uint32>(pos); }
  sint64 peek_sint64(size_type pos) { return peek_value<sint64>(pos); }
  uint64 peek_uint64(size_type pos) { return peek_value<uint64>(pos); }
  real32 peek_real16(size_type pos) { return binary16_to_32(peek_uint16(pos)); }
  real32 peek_real32(size_type pos) { return peek_value<real32>(pos); }
  real64 peek_real64(size_type pos) { return peek_value<real64>(pos); }
 #pragma endregion
 #pragma region PUBLIC_READING_FUNCTIONS_(LE)
 public :
  sint08 LE_read_sint08(void) { sint08 r; LE_read_value(r); return r; }
  uint08 LE_read_uint08(void) { uint08 r; LE_read_value(r); return r; }
  sint16 LE_read_sint16(void) { sint16 r; LE_read_value(r); return r; }
  uint16 LE_read_uint16(void) { uint16 r; LE_read_value(r); return r; }
  sint32 LE_read_sint32(void) { sint32 r; LE_read_value(r); return r; }
  uint32 LE_read_uint32(void) { uint32 r; LE_read_value(r); return r; }
  sint64 LE_read_sint64(void) { sint64 r; LE_read_value(r); return r; }
  uint64 LE_read_uint64(void) { uint64 r; LE_read_value(r); return r; }
 public :
  sint08 LE_read_sint08(size_type pos) { sint08 r; LE_read_value(r, pos); return r; }
  uint08 LE_read_uint08(size_type pos) { uint08 r; LE_read_value(r, pos); return r; }
  sint16 LE_read_sint16(size_type pos) { sint16 r; LE_read_value(r, pos); return r; }
  uint16 LE_read_uint16(size_type pos) { uint16 r; LE_read_value(r, pos); return r; }
  sint32 LE_read_sint32(size_type pos) { sint32 r; LE_read_value(r, pos); return r; }
  uint32 LE_read_uint32(size_type pos) { uint32 r; LE_read_value(r, pos); return r; }
  sint64 LE_read_sint64(size_type pos) { sint64 r; LE_read_value(r, pos); return r; }
  uint64 LE_read_uint64(size_type pos) { uint64 r; LE_read_value(r, pos); return r; }
  binary32 LE_read_real16(void) { return binary16_to_32(LE_read_uint16()); }
  binary32 LE_read_real32(void) { binary32 r; LE_read_value(r); return r; }
  binary64 LE_read_real64(void) { binary64 r; LE_read_value(r); return r; }
  binary32 LE_read_real16(size_type pos) { return binary16_to_32(LE_read_uint16(pos)); }
  binary32 LE_read_real32(size_type pos) { binary32 r; LE_read_value(r, pos); return r; }
  binary64 LE_read_real64(size_type pos) { binary64 r; LE_read_value(r, pos); return r; }
 #pragma endregion
 #pragma region PUBLIC_READING_FUNCTIONS_(BE)
 public :
  sint08 BE_read_sint08(void) { sint08 r; BE_read_value(r); return r; }
  uint08 BE_read_uint08(void) { uint08 r; BE_read_value(r); return r; }
  sint16 BE_read_sint16(void) { sint16 r; BE_read_value(r); return r; }
  uint16 BE_read_uint16(void) { uint16 r; BE_read_value(r); return r; }
  sint32 BE_read_sint32(void) { sint32 r; BE_read_value(r); return r; }
  uint32 BE_read_uint32(void) { uint32 r; BE_read_value(r); return r; }
  sint64 BE_read_sint64(void) { sint64 r; BE_read_value(r); return r; }
  uint64 BE_read_uint64(void) { uint64 r; BE_read_value(r); return r; }
  real32 BE_read_real16(void) { return binary16_to_32(BE_read_uint16()); }
  real32 BE_read_real32(void) { real32 r; BE_read_value(r); return r; }
  real64 BE_read_real64(void) { real64 r; BE_read_value(r); return r; }
 public :
  sint08 BE_read_sint08(size_type pos) { sint08 r; BE_read_value(r, pos); return r; }
  uint08 BE_read_uint08(size_type pos) { uint08 r; BE_read_value(r, pos); return r; }
  sint16 BE_read_sint16(size_type pos) { sint16 r; BE_read_value(r, pos); return r; }
  uint16 BE_read_uint16(size_type pos) { uint16 r; BE_read_value(r, pos); return r; }
  sint32 BE_read_sint32(size_type pos) { sint32 r; BE_read_value(r, pos); return r; }
  uint32 BE_read_uint32(size_type pos) { uint32 r; BE_read_value(r, pos); return r; }
  sint64 BE_read_sint64(size_type pos) { sint64 r; BE_read_value(r, pos); return r; }
  uint64 BE_read_uint64(size_type pos) { uint64 r; BE_read_value(r, pos); return r; }
  real32 BE_read_real16(size_type pos) { return binary16_to_32(BE_read_uint16(pos)); }
  real32 BE_read_real32(size_type pos) { real32 r; BE_read_value(r, pos); return r; }
  real64 BE_read_real64(size_type pos) { real64 r; BE_read_value(r, pos); return r; }
 #pragma endregion
 #pragma region PUBLIC_READING_FUNCTIONS_(MODE)
 public :
  sint08 read_sint08(void) { return read_value<sint08>(); }
  uint08 read_uint08(void) { return read_value<uint08>(); }
  sint16 read_sint16(void) { return read_value<sint16>(); }
  uint16 read_uint16(void) { return read_value<uint16>(); }
  sint32 read_sint32(void) { return read_value<sint32>(); }
  uint32 read_uint32(void) { return read_value<uint32>(); }
  sint64 read_sint64(void) { return read_value<sint64>(); }
  uint64 read_uint64(void) { return read_value<uint64>(); }
  real32 read_real16(void) { return binary16_to_32(read_value<uint16>()); }
  real32 read_real32(void) { return read_value<real32>(); }
  real64 read_real64(void) { return read_value<real64>(); }
 public :
  sint08 read_sint08(size_type pos) { return read_value<sint08>(pos); }
  uint08 read_uint08(size_type pos) { return read_value<uint08>(pos); }
  sint16 read_sint16(size_type pos) { return read_value<sint16>(pos); }
  uint16 read_uint16(size_type pos) { return read_value<uint16>(pos); }
  sint32 read_sint32(size_type pos) { return read_value<sint32>(pos); }
  uint32 read_uint32(size_type pos) { return read_value<uint32>(pos); }
  sint64 read_sint64(size_type pos) { return read_value<sint64>(pos); }
  uint64 read_uint64(size_type pos) { return read_value<uint64>(pos); }
  real32 read_real16(size_type pos) { return binary16_to_32(read_value<uint16>(pos)); }
  real32 read_real32(size_type pos) { return read_value<real32>(pos); }
  real64 read_real64(size_type pos) { return read_value<real64>(pos); }
 #pragma endregion
 #pragma region PUBLIC_READING_FUNCTIONS_(ARRAYS)
 public :
  template<class T>
  bool LE_read_array(T* ptr, size_type n) {
   if(!read((char*)ptr, n*sizeof(T))) return false;
   #ifdef __SYS_BIG_ENDIAN
   reverse_byte_order(ptr, n);
   #endif
   return true;
  }
  template<class T>
  bool BE_read_array(T* ptr, size_type n) {
   if(!read((char*)ptr, n*sizeof(T))) return false;
   #ifdef __SYS_LITTLE_ENDIAN
   reverse_byte_order(ptr, n);
   #endif
   return true;
  }
  template<class T>
  bool LE_read_array(T* ptr, size_type n, size_type pos) {
   if(!read((char*)ptr, n*sizeof(T), pos)) return false;
   #ifdef __SYS_BIG_ENDIAN
   reverse_byte_order(ptr, n);
   #endif
   return true;
  }
  template<class T>
  bool BE_read_array(T* ptr, size_type n, size_type pos) {
   if(!read((char*)ptr, n*sizeof(T), pos)) return false;
   #ifdef __SYS_LITTLE_ENDIAN
   reverse_byte_order(ptr, n);
   #endif
   return true;
  }
 public :
  template<> bool LE_read_array(sint08* ptr, size_type n) { return read((char*)ptr, n); }
  template<> bool BE_read_array(sint08* ptr, size_type n) { return read((char*)ptr, n); }
  template<> bool LE_read_array(uint08* ptr, size_type n) { return read((char*)ptr, n); }
  template<> bool BE_read_array(uint08* ptr, size_type n) { return read((char*)ptr, n); }
  #ifdef __SYS_LITTLE_ENDIAN
  template<> bool BE_read_array(real32* ptr, size_type n) { return BE_read_array(reinterpret_cast<uint32*>(ptr), n); }
  template<> bool BE_read_array(real64* ptr, size_type n) { return BE_read_array(reinterpret_cast<uint64*>(ptr), n); }
  template<> bool BE_read_array(real32* ptr, size_type n, size_type pos) { return BE_read_array(reinterpret_cast<uint32*>(ptr), n, pos); }
  template<> bool BE_read_array(real64* ptr, size_type n, size_type pos) { return BE_read_array(reinterpret_cast<uint64*>(ptr), n, pos); }
  #else
  template<> bool LE_read_array(real32* ptr, size_type n) { return LE_read_array(reinterpret_cast<uint32*>(ptr), n); }
  template<> bool LE_read_array(real64* ptr, size_type n) { return LE_read_array(reinterpret_cast<uint64*>(ptr), n); }
  template<> bool LE_read_array(real32* ptr, size_type n, size_type pos) { return LE_read_array(reinterpret_cast<uint32*>(ptr), n, pos); }
  template<> bool LE_read_array(real64* ptr, size_type n, size_type pos) { return LE_read_array(reinterpret_cast<uint64*>(ptr), n, pos); }
  #endif
 public :
  template<class T>
  bool read_array(T* ptr, size_type n) {
    if(mode == ENDIAN_LITTLE) return LE_read_array(ptr, n);
    if(mode == ENDIAN_BIG) return BE_read_array(ptr, n);
    #ifdef __SYS_LITTLE_ENDIAN
    return LE_read_array(ptr, n);
    #else
    return BE_read_array(ptr, n);
    #endif
  }
  template<class T>
  bool read_array(T* ptr, size_type n, size_type pos) {
    if(mode == ENDIAN_LITTLE) return LE_read_array(ptr, n, pos);
    if(mode == ENDIAN_BIG) return BE_read_array(ptr, n, pos);
    #ifdef __SYS_LITTLE_ENDIAN
    return LE_read_array(ptr, n, pos);
    #else
    return BE_read_array(ptr, n, pos);
    #endif
  }
 #pragma endregion
 #pragma region PUBLIC_READING_FUNCTIONS_(BINARY16_ARRAYS)
 public :
  bool LE_read_real16_array(real32* ptr, size_type n);
  bool LE_read_real16_array(real64* ptr, size_type n);
  bool LE_read_real16_array(real32* ptr, size_type n, size_type pos);
  bool LE_read_real16_array(real64* ptr, size_type n, size_type pos);
 public :
  bool BE_read_real16_array(real32* ptr, size_type n);
  bool BE_read_real16_array(real64* ptr, size_type n);
  bool BE_read_real16_array(real32* ptr, size_type n, size_type pos);
  bool BE_read_real16_array(real64* ptr, size_type n, size_type pos);
 public :
  bool read_real16_array(real32* ptr, size_type n);
  bool read_real16_array(real64* ptr, size_type n);
  bool read_real16_array(real32* ptr, size_type n, size_type pos);
  bool read_real16_array(real64* ptr, size_type n, size_type pos);
 #pragma endregion
};

class binary_ostream {
 #pragma region PUBLIC_DATA
 public :
  typedef std::shared_ptr<char> data_type;
  typedef size_t size_type;
  typedef ptrdiff_t diff_type;
  static const size_type npos = std::numeric_limits<size_type>::max();
 #pragma endregion
 #pragma region PRIVATE_DATA
 private :
 #pragma endregion
};

}

#endif
