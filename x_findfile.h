#ifndef __XENTAX_FINDFILE_H
#define __XENTAX_FINDFILE_H

class find_file {
 protected :
  HANDLE          handle;
  WIN32_FIND_DATA find32;
 public :
  void close(void);
 public :
  bool find(LPCTSTR filename);
  bool next(void);
 public :
  bool is_dots(void);
  bool is_directory(void);
  bool is_encrypted(void);
  bool is_compressed(void);
  bool is_normal(void);
  bool is_hidden(void);
  bool is_system(void);
  bool is_archived(void);
  bool is_readonly(void);
 public :
  LPCTSTR filename(void);
  size_t filesize(void);
 public :
  bool operator !(void)const;
 public :
  find_file();
 ~find_file();
 private :
  find_file(const find_file&);
  void operator =(const find_file&);
};

#endif
