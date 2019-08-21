#include "xentax.h"
#include "x_stream.h"

const binary_stream::size_type binary_stream::npos = std::numeric_limits<size_type>::max();

binary_stream::binary_stream()
{
 elem = 0;
 position = 0;
 mode = ENDIAN_LITTLE;
}

binary_stream::binary_stream(const binary_stream& bs)
{
 data = bs.data;
 elem = bs.elem;
 position = bs.position;
 mode = bs.mode;
}

binary_stream::binary_stream(const boost::shared_array<char>& stream, size_type n)
{
 data = stream;
 elem = n;
 position = 0;
 mode = ENDIAN_LITTLE;
}

binary_stream::~binary_stream()
{
}

binary_stream& binary_stream::operator =(const binary_stream& bs)
{
 if(this == &bs) return *this;
 data = bs.data;
 elem = bs.elem;
 position = bs.position;
 mode = bs.mode;
 return *this;
}

bool binary_stream::can_seek(size_type pos)const
{
 // validate
 if(position == npos) return false; // current position must be valid
 if(pos == npos) return false; // seek position must bevalid
 return !(elem < pos);
}

bool binary_stream::can_move(diff_type pos)const
{
 // validate
 if(position == npos) return false; // current position must be valid
 if(pos == npos) return false; // offset must be valid

 // offset is negative
 if(pos < 0) {
    size_type amount = (size_type)std::abs(pos);
    return !(position < amount);
   }

 // offset is positive
 if(pos > 0) {
    size_type newpos = position + pos;
    return !(elem < newpos);
   }

 return true;
}

void binary_stream::seek(size_type pos)
{
 if(can_seek(pos)) position = pos;
 else position = npos;
}

void binary_stream::seek_reverse(size_type pos)
{
 if(can_seek(pos)) position = elem - pos;
 else position = npos;
}

void binary_stream::move(diff_type pos)
{
 if(pos == 0) return;
 else if(pos > 0) seek(position + pos);
 else if(pos < 0) {
    size_type off = (size_type)std::abs(pos);
    if(!(position < off)) position -= off;
    else position = npos;
   }
}

void binary_stream::move(diff_type pos, size_type from)
{
 if(fail()) return;
 seek(from);
 if(good()) move(pos);
}

void binary_stream::reset(void)
{
 position = 0;
}

binary_stream::size_type binary_stream::search(const char* pattern, size_type p_len, size_t from)
{
 // pattern is longer than data
 if(elem < p_len) return npos;

 // initialize bad-character shifts
 boost::shared_array<unsigned char> table(new unsigned char[256]);
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

void binary_stream::read(char* ptr, size_type n)
{
 if(fail() || !ptr || !n) return;
 size_type newpos = position + n;
 if(!can_move(n)) { position = npos; return; }
 memmove(ptr, c_pos(), n);
 position = newpos;
}

void binary_stream::read_reverse(char* ptr, size_type n)
{
 read(ptr, n);
 if(good()) std::reverse(ptr, ptr + n);
}

std::string binary_stream::read_string(void)
{
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

std::string binary_stream::read_string(enum string_alignment alignment)
{
 std::string retval = read_string();
 if(alignment == STRING_ALIGNMENT_NONE) return retval;

 // variables
 uint32 bytes_read = retval.length() + 1;
 uint32 bytes_full = 0;

 // compute number of bytes to read
 if(alignment == STRING_ALIGNMENT_02) bytes_full = ((bytes_read + 0x01) & (~0x01));
 else if(alignment == STRING_ALIGNMENT_04) bytes_full = ((bytes_read + 0x03) & (~0x03));
 else if(alignment == STRING_ALIGNMENT_08) bytes_full = ((bytes_read + 0x07) & (~0x07));
 else if(alignment == STRING_ALIGNMENT_16) bytes_full = ((bytes_read + 0x0F) & (~0x0F));
 else if(alignment == STRING_ALIGNMENT_32) bytes_full = ((bytes_read + 0x1F) & (~0x1F));
 else if(alignment == STRING_ALIGNMENT_64) bytes_full = ((bytes_read + 0x3F) & (~0x3F));
 else return retval; // assume no alignment

 // skip past the difference
 if(bytes_read < bytes_full) move(bytes_full - bytes_read);
 return retval;
}

//
// LITTLE ENDIAN PEEKING FUNCTIONS
//

sint08 binary_stream::LE_peek_sint08(void) { sint08 r; LE_peek_value<sint08>(r); return r; }
uint08 binary_stream::LE_peek_uint08(void) { uint08 r; LE_peek_value<uint08>(r); return r; }
sint16 binary_stream::LE_peek_sint16(void) { sint16 r; LE_peek_value<sint16>(r); return r; }
uint16 binary_stream::LE_peek_uint16(void) { uint16 r; LE_peek_value<uint16>(r); return r; }
sint32 binary_stream::LE_peek_sint32(void) { sint32 r; LE_peek_value<sint32>(r); return r; }
uint32 binary_stream::LE_peek_uint32(void) { uint32 r; LE_peek_value<uint32>(r); return r; }
sint64 binary_stream::LE_peek_sint64(void) { sint64 r; LE_peek_value<sint64>(r); return r; }
uint64 binary_stream::LE_peek_uint64(void) { uint64 r; LE_peek_value<uint64>(r); return r; }
real32 binary_stream::LE_peek_real16(void) { return float_16_to_32(LE_peek_uint16()); }
real32 binary_stream::LE_peek_real32(void) { real32 r; LE_peek_value<real32>(r); return r; }
real64 binary_stream::LE_peek_real64(void) { real64 r; LE_peek_value<real64>(r); return r; }
sint08 binary_stream::LE_peek_sint08(size_type pos) { sint08 r; LE_peek_value<sint08>(r, pos); return r; }
uint08 binary_stream::LE_peek_uint08(size_type pos) { uint08 r; LE_peek_value<uint08>(r, pos); return r; }
sint16 binary_stream::LE_peek_sint16(size_type pos) { sint16 r; LE_peek_value<sint16>(r, pos); return r; }
uint16 binary_stream::LE_peek_uint16(size_type pos) { uint16 r; LE_peek_value<uint16>(r, pos); return r; }
sint32 binary_stream::LE_peek_sint32(size_type pos) { sint32 r; LE_peek_value<sint32>(r, pos); return r; }
uint32 binary_stream::LE_peek_uint32(size_type pos) { uint32 r; LE_peek_value<uint32>(r, pos); return r; }
sint64 binary_stream::LE_peek_sint64(size_type pos) { sint64 r; LE_peek_value<sint64>(r, pos); return r; }
uint64 binary_stream::LE_peek_uint64(size_type pos) { uint64 r; LE_peek_value<uint64>(r, pos); return r; }
real32 binary_stream::LE_peek_real16(size_type pos) { return float_16_to_32(LE_peek_uint16(pos)); }
real32 binary_stream::LE_peek_real32(size_type pos) { real32 r; LE_peek_value<real32>(r, pos); return r; }
real64 binary_stream::LE_peek_real64(size_type pos) { real64 r; LE_peek_value<real64>(r, pos); return r; }


//
// BIG ENDIAN PEEKING FUNCTIONS
//

sint08 binary_stream::BE_peek_sint08(void) { sint08 r; BE_peek_value<sint08>(r); return r; }
uint08 binary_stream::BE_peek_uint08(void) { uint08 r; BE_peek_value<uint08>(r); return r; }
sint16 binary_stream::BE_peek_sint16(void) { sint16 r; BE_peek_value<sint16>(r); return r; }
uint16 binary_stream::BE_peek_uint16(void) { uint16 r; BE_peek_value<uint16>(r); return r; }
sint32 binary_stream::BE_peek_sint32(void) { sint32 r; BE_peek_value<sint32>(r); return r; }
uint32 binary_stream::BE_peek_uint32(void) { uint32 r; BE_peek_value<uint32>(r); return r; }
sint64 binary_stream::BE_peek_sint64(void) { sint64 r; BE_peek_value<sint64>(r); return r; }
uint64 binary_stream::BE_peek_uint64(void) { uint64 r; BE_peek_value<uint64>(r); return r; }
real32 binary_stream::BE_peek_real16(void) { return float_16_to_32(BE_peek_uint16()); }
real32 binary_stream::BE_peek_real32(void) { real32 r; BE_peek_value<real32>(r); return r; }
real64 binary_stream::BE_peek_real64(void) { real64 r; BE_peek_value<real64>(r); return r; }
sint08 binary_stream::BE_peek_sint08(size_type pos) { sint08 r; BE_peek_value<sint08>(r, pos); return r; }
uint08 binary_stream::BE_peek_uint08(size_type pos) { uint08 r; BE_peek_value<uint08>(r, pos); return r; }
sint16 binary_stream::BE_peek_sint16(size_type pos) { sint16 r; BE_peek_value<sint16>(r, pos); return r; }
uint16 binary_stream::BE_peek_uint16(size_type pos) { uint16 r; BE_peek_value<uint16>(r, pos); return r; }
sint32 binary_stream::BE_peek_sint32(size_type pos) { sint32 r; BE_peek_value<sint32>(r, pos); return r; }
uint32 binary_stream::BE_peek_uint32(size_type pos) { uint32 r; BE_peek_value<uint32>(r, pos); return r; }
sint64 binary_stream::BE_peek_sint64(size_type pos) { sint64 r; BE_peek_value<sint64>(r, pos); return r; }
uint64 binary_stream::BE_peek_uint64(size_type pos) { uint64 r; BE_peek_value<uint64>(r, pos); return r; }
real32 binary_stream::BE_peek_real16(size_type pos) { return float_16_to_32(BE_peek_uint16(pos)); }
real32 binary_stream::BE_peek_real32(size_type pos) { real32 r; BE_peek_value<real32>(r, pos); return r; }
real64 binary_stream::BE_peek_real64(size_type pos) { real64 r; BE_peek_value<real64>(r, pos); return r; }

//
// LITTLE ENDIAN READING FUNCTIONS
//

sint08 binary_stream::LE_read_sint08(void) { sint08 r; LE_read_value<sint08>(r); return r; }
uint08 binary_stream::LE_read_uint08(void) { uint08 r; LE_read_value<uint08>(r); return r; }
sint16 binary_stream::LE_read_sint16(void) { sint16 r; LE_read_value<sint16>(r); return r; }
uint16 binary_stream::LE_read_uint16(void) { uint16 r; LE_read_value<uint16>(r); return r; }
sint32 binary_stream::LE_read_sint32(void) { sint32 r; LE_read_value<sint32>(r); return r; }
uint32 binary_stream::LE_read_uint32(void) { uint32 r; LE_read_value<uint32>(r); return r; }
sint64 binary_stream::LE_read_sint64(void) { sint64 r; LE_read_value<sint64>(r); return r; }
uint64 binary_stream::LE_read_uint64(void) { uint64 r; LE_read_value<uint64>(r); return r; }
real32 binary_stream::LE_read_real16(void) { return float_16_to_32(LE_read_uint16()); }
real32 binary_stream::LE_read_real32(void) { real32 r; LE_read_value<real32>(r); return r; }
real64 binary_stream::LE_read_real64(void) { real64 r; LE_read_value<real64>(r); return r; }
sint08 binary_stream::LE_read_sint08(size_type pos) { sint08 r; LE_read_value<sint08>(r, pos); return r; }
uint08 binary_stream::LE_read_uint08(size_type pos) { uint08 r; LE_read_value<uint08>(r, pos); return r; }
sint16 binary_stream::LE_read_sint16(size_type pos) { sint16 r; LE_read_value<sint16>(r, pos); return r; }
uint16 binary_stream::LE_read_uint16(size_type pos) { uint16 r; LE_read_value<uint16>(r, pos); return r; }
sint32 binary_stream::LE_read_sint32(size_type pos) { sint32 r; LE_read_value<sint32>(r, pos); return r; }
uint32 binary_stream::LE_read_uint32(size_type pos) { uint32 r; LE_read_value<uint32>(r, pos); return r; }
sint64 binary_stream::LE_read_sint64(size_type pos) { sint64 r; LE_read_value<sint64>(r, pos); return r; }
uint64 binary_stream::LE_read_uint64(size_type pos) { uint64 r; LE_read_value<uint64>(r, pos); return r; }
real32 binary_stream::LE_read_real16(size_type pos) { return float_16_to_32(LE_read_uint16(pos)); }
real32 binary_stream::LE_read_real32(size_type pos) { real32 r; LE_read_value<real32>(r, pos); return r; }
real64 binary_stream::LE_read_real64(size_type pos) { real64 r; LE_read_value<real64>(r, pos); return r; }

//
// BIG ENDIAN READING FUNCTIONS
//

sint08 binary_stream::BE_read_sint08(void) { sint08 r; BE_read_value<sint08>(r); return r; }
uint08 binary_stream::BE_read_uint08(void) { uint08 r; BE_read_value<uint08>(r); return r; }
sint16 binary_stream::BE_read_sint16(void) { sint16 r; BE_read_value<sint16>(r); return r; }
uint16 binary_stream::BE_read_uint16(void) { uint16 r; BE_read_value<uint16>(r); return r; }
sint32 binary_stream::BE_read_sint32(void) { sint32 r; BE_read_value<sint32>(r); return r; }
uint32 binary_stream::BE_read_uint32(void) { uint32 r; BE_read_value<uint32>(r); return r; }
sint64 binary_stream::BE_read_sint64(void) { sint64 r; BE_read_value<sint64>(r); return r; }
uint64 binary_stream::BE_read_uint64(void) { uint64 r; BE_read_value<uint64>(r); return r; }
real32 binary_stream::BE_read_real16(void) { return float_16_to_32(BE_read_uint16()); }
real32 binary_stream::BE_read_real32(void) { real32 r; BE_read_value<real32>(r); return r; }
real64 binary_stream::BE_read_real64(void) { real64 r; BE_read_value<real64>(r); return r; }
sint08 binary_stream::BE_read_sint08(size_type pos) { sint08 r; BE_read_value<sint08>(r, pos); return r; }
uint08 binary_stream::BE_read_uint08(size_type pos) { uint08 r; BE_read_value<uint08>(r, pos); return r; }
sint16 binary_stream::BE_read_sint16(size_type pos) { sint16 r; BE_read_value<sint16>(r, pos); return r; }
uint16 binary_stream::BE_read_uint16(size_type pos) { uint16 r; BE_read_value<uint16>(r, pos); return r; }
sint32 binary_stream::BE_read_sint32(size_type pos) { sint32 r; BE_read_value<sint32>(r, pos); return r; }
uint32 binary_stream::BE_read_uint32(size_type pos) { uint32 r; BE_read_value<uint32>(r, pos); return r; }
sint64 binary_stream::BE_read_sint64(size_type pos) { sint64 r; BE_read_value<sint64>(r, pos); return r; }
uint64 binary_stream::BE_read_uint64(size_type pos) { uint64 r; BE_read_value<uint64>(r, pos); return r; }
real32 binary_stream::BE_read_real16(size_type pos) { return float_16_to_32(BE_read_uint16(pos)); }
real32 binary_stream::BE_read_real32(size_type pos) { real32 r; BE_read_value<real32>(r, pos); return r; }
real64 binary_stream::BE_read_real64(size_type pos) { real64 r; BE_read_value<real64>(r, pos); return r; }

//
// MODE PEEKING FUNCTIONS
//

sint08 binary_stream::peek_sint08(void) { return (mode == ENDIAN_LITTLE ? LE_peek_sint08() : BE_peek_sint08()); }
uint08 binary_stream::peek_uint08(void) { return (mode == ENDIAN_LITTLE ? LE_peek_uint08() : BE_peek_uint08()); }
sint16 binary_stream::peek_sint16(void) { return (mode == ENDIAN_LITTLE ? LE_peek_sint16() : BE_peek_sint16()); }
uint16 binary_stream::peek_uint16(void) { return (mode == ENDIAN_LITTLE ? LE_peek_uint16() : BE_peek_uint16()); }
sint32 binary_stream::peek_sint32(void) { return (mode == ENDIAN_LITTLE ? LE_peek_sint32() : BE_peek_sint32()); }
uint32 binary_stream::peek_uint32(void) { return (mode == ENDIAN_LITTLE ? LE_peek_uint32() : BE_peek_uint32()); }
sint64 binary_stream::peek_sint64(void) { return (mode == ENDIAN_LITTLE ? LE_peek_sint64() : BE_peek_sint64()); }
uint64 binary_stream::peek_uint64(void) { return (mode == ENDIAN_LITTLE ? LE_peek_uint64() : BE_peek_uint64()); }
real32 binary_stream::peek_real16(void) { return (mode == ENDIAN_LITTLE ? LE_peek_real16() : BE_peek_real16()); }
real32 binary_stream::peek_real32(void) { return (mode == ENDIAN_LITTLE ? LE_peek_real32() : BE_peek_real32()); }
real64 binary_stream::peek_real64(void) { return (mode == ENDIAN_LITTLE ? LE_peek_real64() : BE_peek_real64()); }
sint08 binary_stream::peek_sint08(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_sint08(pos) : BE_peek_sint08(pos)); }
uint08 binary_stream::peek_uint08(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_uint08(pos) : BE_peek_uint08(pos)); }
sint16 binary_stream::peek_sint16(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_sint16(pos) : BE_peek_sint16(pos)); }
uint16 binary_stream::peek_uint16(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_uint16(pos) : BE_peek_uint16(pos)); }
sint32 binary_stream::peek_sint32(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_sint32(pos) : BE_peek_sint32(pos)); }
uint32 binary_stream::peek_uint32(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_uint32(pos) : BE_peek_uint32(pos)); }
sint64 binary_stream::peek_sint64(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_sint64(pos) : BE_peek_sint64(pos)); }
uint64 binary_stream::peek_uint64(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_uint64(pos) : BE_peek_uint64(pos)); }
real32 binary_stream::peek_real16(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_real16(pos) : BE_peek_real16(pos)); }
real32 binary_stream::peek_real32(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_real32(pos) : BE_peek_real32(pos)); }
real64 binary_stream::peek_real64(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_peek_real64(pos) : BE_peek_real64(pos)); }

//
// MODE READING FUNCTIONS
//

sint08 binary_stream::read_sint08(void) { return (mode == ENDIAN_LITTLE ? LE_read_sint08() : BE_read_sint08()); }
uint08 binary_stream::read_uint08(void) { return (mode == ENDIAN_LITTLE ? LE_read_uint08() : BE_read_uint08()); }
sint16 binary_stream::read_sint16(void) { return (mode == ENDIAN_LITTLE ? LE_read_sint16() : BE_read_sint16()); }
uint16 binary_stream::read_uint16(void) { return (mode == ENDIAN_LITTLE ? LE_read_uint16() : BE_read_uint16()); }
sint32 binary_stream::read_sint32(void) { return (mode == ENDIAN_LITTLE ? LE_read_sint32() : BE_read_sint32()); }
uint32 binary_stream::read_uint32(void) { return (mode == ENDIAN_LITTLE ? LE_read_uint32() : BE_read_uint32()); }
sint64 binary_stream::read_sint64(void) { return (mode == ENDIAN_LITTLE ? LE_read_sint64() : BE_read_sint64()); }
uint64 binary_stream::read_uint64(void) { return (mode == ENDIAN_LITTLE ? LE_read_uint64() : BE_read_uint64()); }
real32 binary_stream::read_real16(void) { return (mode == ENDIAN_LITTLE ? LE_read_real16() : BE_read_real16()); }
real32 binary_stream::read_real32(void) { return (mode == ENDIAN_LITTLE ? LE_read_real32() : BE_read_real32()); }
real64 binary_stream::read_real64(void) { return (mode == ENDIAN_LITTLE ? LE_read_real64() : BE_read_real64()); }
sint08 binary_stream::read_sint08(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_sint08(pos) : BE_read_sint08(pos)); }
uint08 binary_stream::read_uint08(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_uint08(pos) : BE_read_uint08(pos)); }
sint16 binary_stream::read_sint16(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_sint16(pos) : BE_read_sint16(pos)); }
uint16 binary_stream::read_uint16(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_uint16(pos) : BE_read_uint16(pos)); }
sint32 binary_stream::read_sint32(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_sint32(pos) : BE_read_sint32(pos)); }
uint32 binary_stream::read_uint32(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_uint32(pos) : BE_read_uint32(pos)); }
sint64 binary_stream::read_sint64(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_sint64(pos) : BE_read_sint64(pos)); }
uint64 binary_stream::read_uint64(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_uint64(pos) : BE_read_uint64(pos)); }
real32 binary_stream::read_real16(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_real16(pos) : BE_read_real16(pos)); }
real32 binary_stream::read_real32(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_real32(pos) : BE_read_real32(pos)); }
real64 binary_stream::read_real64(size_type pos) { return (mode == ENDIAN_LITTLE ? LE_read_real64(pos) : BE_read_real64(pos)); }

//
// LE REAL16 ARRAY FUNCTIONS
//

void binary_stream::LE_read_real16_array(real32* ptr, size_type n)
{
 // validate arguments
 size_type typesize = sizeof(uint16);
 if(fail() || !ptr || !n) return;

 // compute offset
 size_type offset = n*typesize;
 if(!(position < npos - offset)) { position = npos; return; }

 // compute newpos
 size_type newpos = position + offset;
 if(!can_seek(newpos)) { position = npos; return; }

 // copy data from current position
 for(size_type i = 0; i < n; i++) ptr[i] = float_16_to_32(*reinterpret_cast<const uint16*>(c_str(position + i*typesize)));
 position = newpos;
}

void binary_stream::LE_read_real16_array(real64* ptr, size_type n)
{
 size_type typesize = sizeof(uint16);
 if(fail() || !ptr || !n) return;
 size_type offset = n*typesize;
 if(!(position < npos - offset)) { position = npos; return; }
 size_type newpos = position + offset;
 if(!can_seek(newpos)) { position = npos; return; }
 for(size_type i = 0; i < n; i++) ptr[i] = float_16_to_32(*reinterpret_cast<const uint16*>(c_str(position + i*typesize)));
 position = newpos;
}

void binary_stream::LE_read_real16_array(real32* ptr, size_type n, size_type pos)
{
 // validate arguments
 size_type typesize = sizeof(uint16);
 if(fail() || !ptr || !n) return;

 // compute offset
 size_type offset = n*typesize;
 if(!(pos < npos - offset)) { position = npos; return; }

 // compute newpos
 size_type newpos = pos + offset;
 if(!can_seek(newpos)) { position = npos; return; }

 // copy data from selected position
 for(size_type i = 0; i < n; i++) ptr[i] = float_16_to_32(*reinterpret_cast<const uint16*>(c_str(pos + i*typesize)));
}

void binary_stream::LE_read_real16_array(real64* ptr, size_type n, size_type pos)
{
 size_type typesize = sizeof(uint16);
 if(fail() || !ptr || !n) return;
 size_type offset = n*typesize;
 if(!(pos < npos - offset)) { position = npos; return; }
 size_type newpos = pos + offset;
 if(!can_seek(newpos)) { position = npos; return; }
 for(size_type i = 0; i < n; i++) ptr[i] = float_16_to_32(*reinterpret_cast<const uint16*>(c_str(pos + i*typesize)));
}

//
// BE REAL16 ARRAY FUNCTIONS
//

void binary_stream::BE_read_real16_array(real32* ptr, size_type n)
{
 // validate arguments
 size_type typesize = sizeof(uint16);
 if(fail() || !ptr || !n) return;

 // compute offset
 size_type offset = n*typesize;
 if(!(position < npos - offset)) { position = npos; return; }

 // compute newpos
 size_type newpos = position + offset;
 if(!can_seek(newpos)) { position = npos; return; }

 // copy data from current position
 for(size_type i = 0; i < n; i++) {
     uint16 temp = *reinterpret_cast<const uint16*>(c_str(position + i*typesize));
     reverse_byte_order(&temp);
     ptr[i] = float_16_to_32(temp);
    }

 // move to newpos
 position = newpos;
}

void binary_stream::BE_read_real16_array(real64* ptr, size_type n)
{
 size_type typesize = sizeof(uint16);
 if(fail() || !ptr || !n) return;
 size_type offset = n*typesize;
 if(!(position < npos - offset)) { position = npos; return; }
 size_type newpos = position + offset;
 if(!can_seek(newpos)) { position = npos; return; }
 for(size_type i = 0; i < n; i++) {
     uint16 temp = *reinterpret_cast<const uint16*>(c_str(position + i*typesize));
     reverse_byte_order(&temp);
     ptr[i] = static_cast<real64>(float_16_to_32(temp));
    }
 position = newpos;
}

void binary_stream::BE_read_real16_array(real32* ptr, size_type n, size_type pos)
{
 // validate arguments
 size_type typesize = sizeof(uint16);
 if(fail() || !ptr || !n) return;

 // compute offset
 size_type offset = n*typesize;
 if(!(pos < npos - offset)) { position = npos; return; }

 // compute newpos
 size_type newpos = pos + offset;
 if(!can_seek(newpos)) { position = npos; return; }

 // copy data from selected position
 for(size_type i = 0; i < n; i++) {
     uint16 temp = *reinterpret_cast<const uint16*>(c_str(pos + i*typesize));
     reverse_byte_order(&temp);
     ptr[i] = float_16_to_32(temp);
    }
}

void binary_stream::BE_read_real16_array(real64* ptr, size_type n, size_type pos)
{
 size_type typesize = sizeof(uint16);
 if(fail() || !ptr || !n) return;
 size_type offset = n*typesize;
 if(!(pos < npos - offset)) { position = npos; return; }
 size_type newpos = pos + offset;
 if(!can_seek(newpos)) { position = npos; return; }
 for(size_type i = 0; i < n; i++) {
     uint16 temp = *reinterpret_cast<const uint16*>(c_str(pos + i*typesize));
     reverse_byte_order(&temp);
     ptr[i] = static_cast<real64>(float_16_to_32(temp));
    }
}

//
// MODE REAL16 ARRAY FUNCTIONS
//

void binary_stream::read_real16_array(real32* ptr, size_type n)
{
 if(mode == ENDIAN_LITTLE) LE_read_real16_array(ptr, n);
 else BE_read_real16_array(ptr, n);
}

void binary_stream::read_real16_array(real64* ptr, size_type n)
{
 if(mode == ENDIAN_LITTLE) LE_read_real16_array(ptr, n);
 else BE_read_real16_array(ptr, n);
}

void binary_stream::read_real16_array(real32* ptr, size_type n, size_type pos)
{
 if(mode == ENDIAN_LITTLE) LE_read_real16_array(ptr, n, pos);
 else BE_read_real16_array(ptr, n, pos);
}

void binary_stream::read_real16_array(real64* ptr, size_type n, size_type pos)
{
 if(mode == ENDIAN_LITTLE) LE_read_real16_array(ptr, n, pos);
 else BE_read_real16_array(ptr, n, pos);
}