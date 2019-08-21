#ifndef __XENTAX_BMP_H
#define __XENTAX_BMP_H

struct pixelRGBA {
 unsigned char R;
 unsigned char G;
 unsigned char B;
 unsigned char A;
};

class imageRGBA {
 private :
  boost::shared_array<unsigned char> _data;
  size_t _rows;
  size_t _cols;
 public :
  void clear(void);
  unsigned char* get(void)const { return _data.get(); }
  size_t rows(void)const { return _rows; }
  size_t cols(void)const { return _cols; }
  size_t width_bytes(void)const { return (_cols << 2); }
  size_t pitch_bytes(void)const { return (_cols << 2); }
  size_t total_bytes(void)const { return (_cols << 2) * _rows; }
  unsigned char& operator [](size_t index) { return _data[index]; }
  const unsigned char& operator [](size_t index)const { return _data[index]; }
  pixelRGBA get_pixel(size_t r, size_t c)const;
  pixelRGBA set_pixel(size_t r, size_t c, const pixelRGBA& p);
 public :
  bool morton(void);
  bool untile(void);
 public :
  bool loadBMP(const char* filename);
  bool loadTGA(const char* filename);
  bool loadDDS(const char* filename);
  bool saveBMP(const char* filename)const;
  bool saveTGA(const char* filename)const;
  bool saveDDS(const char* filename)const;
 public :
  imageRGBA();
 ~imageRGBA();
};

BOOL CreateBMPHeaders(DWORD dx, DWORD dy, DWORD bpp, BITMAPFILEHEADER* fileHeader, BITMAPINFOHEADER* infoHeader);

BOOL SaveYUV411ToBMP(const char* filename, DWORD dx, DWORD dy, const char* data, uint32 size);
BOOL SaveYUV420ToBMP(const char* filename, DWORD dx, DWORD dy, const char* data, uint32 size);
BOOL SaveYUV422ToBMP(const char* filename, DWORD dx, DWORD dy, const char* data, uint32 size);
BOOL SaveYUV444ToBMP(const char* filename, DWORD dx, DWORD dy, const char* data, uint32 size);

#endif
