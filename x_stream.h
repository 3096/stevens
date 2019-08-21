#ifndef __XENTAX_STREAM_H
#define __XENTAX_STREAM_H

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
 public :
  typedef boost::shared_array<char> data_type;
  typedef size_t size_type;
  typedef ptrdiff_t diff_type;
  static const size_type npos;
 private :
  data_type data;
  size_type elem;
  size_type position;
  endian_mode mode;
 public :
  char& operator [](size_t index) { return data.get()[index]; }
  const char& operator [](size_t index)const { return data.get()[index]; }
 public :
  size_type size(void)const { return elem; }
  const char* c_str(void)const { return data.get(); }
  const char* c_str(size_type pos)const { return data.get() + pos; }
  const char* c_pos(void)const { return data.get() + position; }
  size_type tell(void)const { return position; }
  bool at_begin(void)const { return position == 0; }
  bool at_end(void)const { return position == elem; }
  bool can_seek(size_type pos)const;
  bool can_move(diff_type pos)const;
  bool fail(void)const { return position == npos; }
  bool good(void)const { return position != npos; }
 public :
  void seek(size_type pos);
  void seek_reverse(size_type pos);
  void move(diff_type pos);
  void move(diff_type pos, size_type from);
  void reset(void);
 public :
  size_type search(const char* pattern, size_type n, size_t from = 0);
 public :
  void read(char* ptr, size_type n);
  void read_reverse(char* ptr, size_type n);
  std::string read_string(void);
  std::string read_string(enum string_alignment alignment);
 public :
  endian_mode get_endian_mode(void)const { return mode; }
  void set_endian_mode(endian_mode m) { mode = m; }
 // template functions (little endian)
 private :
  template<class T>
  bool LE_peek_value(T& value) {
   if(fail() || !(position < (npos - sizeof(T)))) return false;
   size_type newpos = position + sizeof(T);
   if(!can_seek(newpos)) return false; // don't fail
   value = *reinterpret_cast<const T*>(c_pos());
   return true;
  }
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
  bool LE_peek_value(T& value, size_type pos) {
   if(fail() || !(pos < (npos - sizeof(T)))) return false;
   size_type newpos = pos + sizeof(T);
   if(!can_seek(newpos)) return false; // don't fail
   value = *reinterpret_cast<const T*>(c_str(pos));
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
 // template functions (big endian)
 private :
  template<class T>
  bool BE_peek_value(T& value) {
   if(fail() || !(position < (npos - sizeof(T)))) return false;
   size_type newpos = position + sizeof(T);
   if(!can_seek(newpos)) return false; // don't fail
   T r = *reinterpret_cast<const T*>(c_pos());
   reverse_byte_order(&r);
   value = r;
   return true;
  }
  template<>
  bool BE_peek_value(real32& value) {
   // if(fail() || !(position < (npos - sizeof(real32)))) return false;
   // size_type newpos = position + sizeof(real32);
   // if(!can_seek(newpos)) return false; // don't fail
   // uint32 r = *reinterpret_cast<const uint32*>(c_pos());
   // reverse_byte_order(&r);
   // value = *reinterpret_cast<const real32*>(&r);
   // return true;
   uint32 r;
   if(!BE_peek_value(r)) return false;
   value = *reinterpret_cast<const real32*>(&r);
   return true;
  }
  template<>
  bool BE_peek_value(real64& value) {
   // if(fail() || !(position < (npos - sizeof(real64)))) return false;
   // size_type newpos = position + sizeof(real64);
   // if(!can_seek(newpos)) return false; // don't fail
   // uint64 r = *reinterpret_cast<const uint64*>(c_pos());
   // reverse_byte_order(&r);
   // value = *reinterpret_cast<const real64*>(&r);
   // return true;
   uint64 r;
   if(!BE_peek_value(r)) return false;
   value = *reinterpret_cast<const real64*>(&r);
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
  template<>
  bool BE_read_value(real32& value) {
   if(fail() || !(position < (npos - sizeof(real32)))) return false;
   size_type newpos = position + sizeof(real32);
   if(!can_seek(newpos)) { position = npos; return false; }
   uint32 r = *reinterpret_cast<const uint32*>(c_pos());
   reverse_byte_order(&r);
   position = newpos;
   value = *reinterpret_cast<const real32*>(&r);
   return true;
  }
  template<>
  bool BE_read_value(real64& value) {
   if(fail() || !(position < (npos - sizeof(real64)))) return false;
   size_type newpos = position + sizeof(real64);
   if(!can_seek(newpos)) { position = npos; return false; }
   uint64 r = *reinterpret_cast<const uint64*>(c_pos());
   reverse_byte_order(&r);
   position = newpos;
   value = *reinterpret_cast<const real64*>(&r);
   return true;
  }
  template<class T>
  bool BE_peek_value(T& value, size_type pos) {
   if(fail() || !(pos < (npos - sizeof(T)))) return false;
   size_type newpos = pos + sizeof(T);
   if(!can_seek(newpos)) return false; // don't fail
   T r = *reinterpret_cast<const T*>(c_str(pos));
   reverse_byte_order(&r);
   value = r;
   return true;
  }
  template<>
  bool BE_peek_value(real32& value, size_type pos) {
   if(fail() || !(pos < (npos - sizeof(real32)))) return false;
   size_type newpos = pos + sizeof(real32);
   if(!can_seek(newpos)) return false; // don't fail
   uint32 r = *reinterpret_cast<const uint32*>(c_str(pos));
   reverse_byte_order(&r);
   value = *reinterpret_cast<const real32*>(&r);
   return true;
  }
  template<>
  bool BE_peek_value(real64& value, size_type pos) {
   if(fail() || !(pos < (npos - sizeof(real64)))) return false;
   size_type newpos = pos + sizeof(real64);
   if(!can_seek(newpos)) return false; // don't fail
   uint64 r = *reinterpret_cast<const uint64*>(c_str(pos));
   reverse_byte_order(&r);
   value = *reinterpret_cast<const real64*>(&r);
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
  template<>
  bool BE_read_value(real32& value, size_type pos) {
   if(fail() || !(pos < (npos - sizeof(real32)))) return false;
   size_type newpos = pos + sizeof(real32);
   if(!can_seek(newpos)) { position = npos; return false; }
   uint32 r = *reinterpret_cast<const uint32*>(c_str(pos));
   reverse_byte_order(&r);
   position = newpos;
   value = *reinterpret_cast<const real32*>(&r);
   return true;
  }
  template<>
  bool BE_read_value(real64& value, size_type pos) {
   if(fail() || !(pos < (npos - sizeof(real64)))) return false;
   size_type newpos = pos + sizeof(real64);
   if(!can_seek(newpos)) { position = npos; return false; }
   uint64 r = *reinterpret_cast<const uint64*>(c_str(pos));
   reverse_byte_order(&r);
   position = newpos;
   value = *reinterpret_cast<const real64*>(&r);
   return true;
  }
 // LE peeking functions
 public :
  sint08 LE_peek_sint08(void);
  uint08 LE_peek_uint08(void);
  sint16 LE_peek_sint16(void);
  uint16 LE_peek_uint16(void);
  sint32 LE_peek_sint32(void);
  uint32 LE_peek_uint32(void);
  sint64 LE_peek_sint64(void);
  uint64 LE_peek_uint64(void);
  real32 LE_peek_real16(void);
  real32 LE_peek_real32(void);
  real64 LE_peek_real64(void);
  sint08 LE_peek_sint08(size_type pos);
  uint08 LE_peek_uint08(size_type pos);
  sint16 LE_peek_sint16(size_type pos);
  uint16 LE_peek_uint16(size_type pos);
  sint32 LE_peek_sint32(size_type pos);
  uint32 LE_peek_uint32(size_type pos);
  sint64 LE_peek_sint64(size_type pos);
  uint64 LE_peek_uint64(size_type pos);
  real32 LE_peek_real16(size_type pos);
  real32 LE_peek_real32(size_type pos);
  real64 LE_peek_real64(size_type pos);
 // BE peeking functions
 public :
  sint08 BE_peek_sint08(void);
  uint08 BE_peek_uint08(void);
  sint16 BE_peek_sint16(void);
  uint16 BE_peek_uint16(void);
  sint32 BE_peek_sint32(void);
  uint32 BE_peek_uint32(void);
  sint64 BE_peek_sint64(void);
  uint64 BE_peek_uint64(void);
  real32 BE_peek_real16(void);
  real32 BE_peek_real32(void);
  real64 BE_peek_real64(void);
  sint08 BE_peek_sint08(size_type pos);
  uint08 BE_peek_uint08(size_type pos);
  sint16 BE_peek_sint16(size_type pos);
  uint16 BE_peek_uint16(size_type pos);
  sint32 BE_peek_sint32(size_type pos);
  uint32 BE_peek_uint32(size_type pos);
  sint64 BE_peek_sint64(size_type pos);
  uint64 BE_peek_uint64(size_type pos);
  real32 BE_peek_real16(size_type pos);
  real32 BE_peek_real32(size_type pos);
  real64 BE_peek_real64(size_type pos);
 // LE reading functions
 public :
  sint08 LE_read_sint08(void);
  uint08 LE_read_uint08(void);
  sint16 LE_read_sint16(void);
  uint16 LE_read_uint16(void);
  sint32 LE_read_sint32(void);
  uint32 LE_read_uint32(void);
  sint64 LE_read_sint64(void);
  uint64 LE_read_uint64(void);
  real32 LE_read_real16(void);
  real32 LE_read_real32(void);
  real64 LE_read_real64(void);
  sint08 LE_read_sint08(size_type pos);
  uint08 LE_read_uint08(size_type pos);
  sint16 LE_read_sint16(size_type pos);
  uint16 LE_read_uint16(size_type pos);
  sint32 LE_read_sint32(size_type pos);
  uint32 LE_read_uint32(size_type pos);
  sint64 LE_read_sint64(size_type pos);
  uint64 LE_read_uint64(size_type pos);
  real32 LE_read_real16(size_type pos);
  real32 LE_read_real32(size_type pos);
  real64 LE_read_real64(size_type pos);
 // BE reading functions
 public :
  sint08 BE_read_sint08(void);
  uint08 BE_read_uint08(void);
  sint16 BE_read_sint16(void);
  uint16 BE_read_uint16(void);
  sint32 BE_read_sint32(void);
  uint32 BE_read_uint32(void);
  sint64 BE_read_sint64(void);
  uint64 BE_read_uint64(void);
  real32 BE_read_real16(void);
  real32 BE_read_real32(void);
  real64 BE_read_real64(void);
  sint08 BE_read_sint08(size_type pos);
  uint08 BE_read_uint08(size_type pos);
  sint16 BE_read_sint16(size_type pos);
  uint16 BE_read_uint16(size_type pos);
  sint32 BE_read_sint32(size_type pos);
  uint32 BE_read_uint32(size_type pos);
  sint64 BE_read_sint64(size_type pos);
  uint64 BE_read_uint64(size_type pos);
  real32 BE_read_real16(size_type pos);
  real32 BE_read_real32(size_type pos);
  real64 BE_read_real64(size_type pos);
 // mode peeking functions
 public :
  sint08 peek_sint08(void);
  uint08 peek_uint08(void);
  sint16 peek_sint16(void);
  uint16 peek_uint16(void);
  sint32 peek_sint32(void);
  uint32 peek_uint32(void);
  sint64 peek_sint64(void);
  uint64 peek_uint64(void);
  real32 peek_real16(void);
  real32 peek_real32(void);
  real64 peek_real64(void);
  sint08 peek_sint08(size_type pos);
  uint08 peek_uint08(size_type pos);
  sint16 peek_sint16(size_type pos);
  uint16 peek_uint16(size_type pos);
  sint32 peek_sint32(size_type pos);
  uint32 peek_uint32(size_type pos);
  sint64 peek_sint64(size_type pos);
  uint64 peek_uint64(size_type pos);
  real32 peek_real16(size_type pos);
  real32 peek_real32(size_type pos);
  real64 peek_real64(size_type pos);
 // mode reading functions
 public :
  sint08 read_sint08(void);
  uint08 read_uint08(void);
  sint16 read_sint16(void);
  uint16 read_uint16(void);
  sint32 read_sint32(void);
  uint32 read_uint32(void);
  sint64 read_sint64(void);
  uint64 read_uint64(void);
  real32 read_real16(void);
  real32 read_real32(void);
  real64 read_real64(void);
  sint08 read_sint08(size_type pos);
  uint08 read_uint08(size_type pos);
  sint16 read_sint16(size_type pos);
  uint16 read_uint16(size_type pos);
  sint32 read_sint32(size_type pos);
  uint32 read_uint32(size_type pos);
  sint64 read_sint64(size_type pos);
  uint64 read_uint64(size_type pos);
  real32 read_real16(size_type pos);
  real32 read_real32(size_type pos);
  real64 read_real64(size_type pos);
 // LE array functions
 public :
  template<class T>
  void LE_read_array(T* ptr, size_type n)
  {
   if(fail() || !ptr || !n) return;
   size_type offset = n*sizeof(T);
   size_type newpos = position + offset;
   if(!can_seek(newpos)) { position = npos; return; }
   memmove(ptr, c_pos(), offset);
   position = newpos;
  }
  template<class T>
  void LE_read_array(T* ptr, size_type n, size_type pos)
  {
   if(fail() || !ptr || !n) return;
   size_type offset = n*sizeof(T);
   if(!(pos < npos - offset)) { position = npos; return; }
   size_type newpos = pos + offset;
   if(!can_seek(newpos)) { position = npos; return; }
   memmove(ptr, c_str(pos), offset);
  }
 // BE array functions
 public :
  template<class T> void BE_read_array(T* ptr, size_type n) {
   LE_read_array(ptr, n);
   if(good()) reverse_byte_order(ptr, n);
  }
  template<> void BE_read_array(sint08* ptr, size_type n) {
   LE_read_array(ptr, n);
  }
  template<> void BE_read_array(uint08* ptr, size_type n) {
   LE_read_array(ptr, n);
  }
  template<> void BE_read_array(real32* ptr, size_type n) {
   uint32* tmp = reinterpret_cast<uint32*>(ptr);
   LE_read_array(tmp, n);
   if(good()) reverse_byte_order(tmp, n);
  }
  template<> void BE_read_array(real64* ptr, size_type n) {
   uint64* tmp = reinterpret_cast<uint64*>(ptr);
   LE_read_array(tmp, n);
   if(good()) reverse_byte_order(tmp, n);
  }
  template<class T> void BE_read_array(T* ptr, size_type n, size_type pos) {
   LE_read_array(ptr, n, pos);
   if(good()) reverse_byte_order(ptr, n);
  }
  template<> void BE_read_array(real32* ptr, size_type n, size_type pos) {
   uint32* tmp = reinterpret_cast<uint32*>(ptr);
   LE_read_array(tmp, n, pos);
   if(good()) reverse_byte_order(tmp, n);
  }
  template<> void BE_read_array(real64* ptr, size_type n, size_type pos) {
   uint64* tmp = reinterpret_cast<uint64*>(ptr);
   LE_read_array(tmp, n, pos);
   if(good()) reverse_byte_order(tmp, n);
  }
 // mode array functions
 public :
  template<class T> void read_array(T* ptr, size_type n) {
   if(mode == ENDIAN_LITTLE) LE_read_array(ptr, n);
   else BE_read_array(ptr, n);
  }
  template<class T> void read_array(T* ptr, size_type n, size_type pos) {
   if(mode == ENDIAN_LITTLE) LE_read_array(ptr, n, pos);
   else BE_read_array(ptr, n, pos);
  }
 // LE real16 array functions
 public :
  void LE_read_real16_array(real32* ptr, size_type n);
  void LE_read_real16_array(real64* ptr, size_type n);
  void LE_read_real16_array(real32* ptr, size_type n, size_type pos);
  void LE_read_real16_array(real64* ptr, size_type n, size_type pos);
 // BE real16 array functions
 public :
  void BE_read_real16_array(real32* ptr, size_type n);
  void BE_read_real16_array(real64* ptr, size_type n);
  void BE_read_real16_array(real32* ptr, size_type n, size_type pos);
  void BE_read_real16_array(real64* ptr, size_type n, size_type pos);
 // mode real16 array functions
 public :
  void read_real16_array(real32* ptr, size_type n);
  void read_real16_array(real64* ptr, size_type n);
  void read_real16_array(real32* ptr, size_type n, size_type pos);
  void read_real16_array(real64* ptr, size_type n, size_type pos);
 public :
  binary_stream& operator =(const binary_stream& bs);
 public :
  binary_stream();
  binary_stream(const binary_stream& bs);
  binary_stream(const boost::shared_array<char>& stream, size_type n);
 ~binary_stream();
};

#endif
