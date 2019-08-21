#ifndef __CS_CG_TEXTURE_H
#define __CS_CG_TEXTURE_H

// Standard Headers
#include<memory>

// CS Headers
#include<cs/core/types.h>

//
// SAMPLER CLASSES
//
namespace cs { namespace cg {
}}

//
// TEXTURE CLASS
//
namespace cs { namespace cg {

enum texture_format {
 TEXTURE_FORMAT_UNKNOWN = 0x00,
 TEXTURE_FORMAT_BGRA    = 0x01,
 TEXTURE_FORMAT_BGR     = 0x02,
};

class texture {
 private:
  std::shared_ptr<unsigned char> image_data;
  cs::uint32 image_dx;
  cs::uint32 image_dy;
  texture_format image_type;
  cs::uint32 image_pitch;
  cs::uint32 image_bytes;
 public :
  const unsigned char* c_ptr(void)const { return image_data.get(); }
  texture_format format(void)const { return image_type; }
  uint32 dx(void)const { return image_dx; }
  uint32 dy(void)const { return image_dy; }
  uint32 pitch(void)const { return image_pitch; }
  uint32 bytes(void)const { return image_bytes; }
 public :
  bool load(const char* filename);
  void free(void);
  array4D<unsigned char> sample(binary32 u, binary32 v);
 public :
  unsigned char& operator [](size_t index) { return image_data.get()[index]; }
  const unsigned char& operator [](size_t index)const { return image_data.get()[index]; }
 public :
  texture& operator =(const texture& other);
  texture& operator =(texture&& other);
 public :
  texture();
  texture(uint32 w, uint32 h, texture_format f);
  texture(const texture& obj);
  texture(texture&& other);
 ~texture();
};

}}

#endif
