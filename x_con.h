#ifndef __XENTAX_CON_H
#define __XENTAX_CON_H

//
// TODO:
// SHARED MULTIDIMENSIONAL ARRAY
//

// template<class T>
// class shared_multi_array {
//  private :
//   boost::shared_array<boost::shared_array<T>> _data;
//   uint32 _rows;
//   uint32 _cols;
//  public :
//   shared_multi_array() {
//   }
//  ~shared_multi_array() {
//   }
// };

//
// TODO:
// SHARED JAGGED 2D ARRAY
//

// template<class T>
// class shared_jagged_array2D {
// };

//
// SHARED POINT2D ARRAY
//

template<class T>
class point2D_array {
 private :
  boost::shared_array<boost::shared_array<T>> data;
  uint32 elem;
 public :
  boost::shared_array<T>& operator [](uint32 index) {
   return data[index];
  }
  boost::shared_array<T> operator [](uint32 index)const {
   return data[index];
  }
  boost::shared_array<T> get(uint32 index)const {
   return data[index];
  }
  void set(uint32 index, const T& x, const T& y) {
   data[index][0] = x;
   data[index][1] = y;
  }
  uint32 size(void)const {
   return elem;
  }
  void clear(void) {
   data.reset();
   elem = 0;
  }
  void resize(uint32 size)
  {
   if(size == elem) return;
   if(size == 0) {
      data.reset();
      elem = 0;
     }
   else {
      elem = size;
      data.reset(new boost::shared_array<T>[elem]);
      for(uint32 i = 0; i < elem; i++) data[i].reset(new T[2]);
     }
  }
 public :
  point2D_array& operator =(const point2D_array& pa) {
   if(&pa == this) return *this;
   elem = pa.elem;
   data = pa.data;
   return *this;
  }
 public :
  point2D_array() : elem(0) {}
  point2D_array(const point2D_array& pa) {
   elem = pa.elem;
   data = pa.data;
  }
  explicit point2D_array(uint32 size) {
   elem = size;
   if(size == 0) return;
   data.reset(new boost::shared_array<T>[elem]);
   for(uint32 i = 0; i < elem; i++) data[i].reset(new T[2]);
  }
 ~point2D_array() {}
};

//
// SHARED POINT3D ARRAY
//

template<class T>
class point3D_array {
 private :
  boost::shared_array<boost::shared_array<T>> data;
  uint32 elem;
 public :
  boost::shared_array<T>& operator [](uint32 index) {
   return data[index];
  }
  boost::shared_array<T> operator [](uint32 index)const {
   return data[index];
  }
  boost::shared_array<T> get(uint32 index)const {
   return data[index];
  }
  void set(uint32 index, const T& x, const T& y, const T& z) {
   data[index][0] = x;
   data[index][1] = y;
   data[index][2] = z;
  }
  uint32 size(void)const {
   return elem;
  }
  void clear(void) {
   data.reset();
   elem = 0;
  }
  void resize(uint32 size)
  {
   if(size == elem) return;
   if(size == 0) {
      data.reset();
      elem = 0;
     }
   else {
      elem = size;
      data.reset(new boost::shared_array<T>[elem]);
      for(uint32 i = 0; i < elem; i++) data[i].reset(new T[3]);
     }
  }
 public :
  point3D_array& operator =(const point3D_array& pa) {
   if(&pa == this) return *this;
   elem = pa.elem;
   data = pa.data;
   return *this;
  }
 public :
  point3D_array() : elem(0) {}
  point3D_array(const point3D_array& pa) {
   elem = pa.elem;
   data = pa.data;
  }
  explicit point3D_array(uint32 size) {
   elem = size;
   if(size == 0) return;
   data.reset(new boost::shared_array<T>[elem]);
   for(uint32 i = 0; i < elem; i++) data[i].reset(new T[3]);
  }
 ~point3D_array() {}
};

//
// SHARED POINT4D ARRAY
//

template<class T>
class point4D_array {
 private :
  boost::shared_array<boost::shared_array<T>> data;
  uint32 elem;
 public :
  boost::shared_array<T>& operator [](uint32 index) {
   return data[index];
  }
  boost::shared_array<T> operator [](uint32 index)const {
   return data[index];
  }
  boost::shared_array<T> get(uint32 index)const {
   return data[index];
  }
  void set(uint32 index, const T& x, const T& y, const T& z, const T& w) {
   data[index][0] = x;
   data[index][1] = y;
   data[index][2] = z;
   data[index][3] = w;
  }
  uint32 size(void)const {
   return elem;
  }
  void clear(void) {
   data.reset();
   elem = 0;
  }
  void resize(uint32 size)
  {
   if(size == elem) return;
   if(size == 0) {
      data.reset();
      elem = 0;
     }
   else {
      elem = size;
      data.reset(new boost::shared_array<T>[elem]);
      for(uint32 i = 0; i < elem; i++) data[i].reset(new T[4]);
     }
  }
 public :
  point4D_array& operator =(const point4D_array& pa) {
   if(&pa == this) return *this;
   elem = pa.elem;
   data = pa.data;
   return *this;
  }
 public :
  point4D_array() : elem(0) {}
  point4D_array(const point4D_array& pa) {
   elem = pa.elem;
   data = pa.data;
  }
  explicit point4D_array(uint32 size) {
   elem = size;
   if(size == 0) return;
   data.reset(new boost::shared_array<T>[elem]);
   for(uint32 i = 0; i < elem; i++) data[i].reset(new T[4]);
  }
  point4D_array(uint32 size, const T& v) {
   elem = size;
   if(size == 0) return;
   data.reset(new boost::shared_array<T>[elem]);
   for(uint32 i = 0; i < elem; i++) {
       data[i].reset(new T[4]);
       data[i][0] = v;
       data[i][1] = v;
       data[i][2] = v;
       data[i][3] = v;
      }
  }
  point4D_array(uint32 size, const T& x, const T& y, const T& z, const T& w) {
   elem = size;
   if(size == 0) return;
   data.reset(new boost::shared_array<T>[elem]);
   for(uint32 i = 0; i < elem; i++) {
       data[i].reset(new T[4]);
       data[i][0] = x;
       data[i][1] = y;
       data[i][2] = z;
       data[i][3] = w;
      }
  }
  point4D_array(uint32 size, const T* ptr) {
   elem = size;
   if(size == 0) return;
   data.reset(new boost::shared_array<T>[elem]);
   for(uint32 i = 0; i < elem; i++) {
       data[i].reset(new T[4]);
       data[i][0] = ptr[0];
       data[i][1] = ptr[1];
       data[i][2] = ptr[2];
       data[i][3] = ptr[3];
      }
  }
 ~point4D_array() {}
};

//
//
//

template<class K, class V>
class map_deque {
 public :
  typedef size_t size_type;
  typedef K key_type;
  typedef V mapped_type;
  typedef std::deque<V> con_type;
  typedef std::map<key_type, con_type> map_type;
  typedef std::pair<K, V> value_type;
 public :
  typedef typename map_type::iterator iterator;
  typedef typename map_type::const_iterator const_iterator;
  typedef typename map_type::reverse_iterator reverse_iterator;
  typedef typename map_type::const_reverse_iterator const_reverse_iterator;
 private :
  map_type data;
 // iterator functions
 public :
  iterator begin(void) {
   return data.begin();
  }
  iterator end(void) {
   return data.end();
  }
  const_iterator begin()const {
   return data.begin();
  }
  const_iterator end()const {
   return data.end();
  }
  const_iterator cbegin()const {
   return data.begin();
  }
  const_iterator cend()const {
   return data.cend();
  }
  reverse_iterator rbegin(void) {
   return data.rbegin();
  }
  reverse_iterator rend(void) {
   return data.rend();
  }
  const_reverse_iterator rbegin(void)const {
   return data.rbegin();
  }
  const_reverse_iterator rend(void)const {
   return data.rend();
  }
  const_reverse_iterator crbegin(void)const {
   return data.crbegin();
  }
  const_reverse_iterator crend(void)const {
   return data.crend();
  }
 public :
  size_type count(const key_type& key) {
   return data.count();
  }
  bool empty(void)const {
   return data.empty();
  }
  size_type size(void)const {
   return data.size();
  }
 // removal functions
 public :
  void clear(void) {
   data.clear();
  }
  void erase(iterator position) {
   data.erase(position);
  }
  size_type erase(const key_type& k) {
   return data.erase(k);
  }
  void erase(iterator first, iterator last) {
   data.erase(first, last);
  }
 // insertion functions
 public :
  void insert(const value_type& item) {
   auto map_iter = data.find(item.first);
   if(map_iter != data.end()) map_iter->second.push_back(item.second);
   else data.insert(map_type::value_type(item.first, con_type(1, item.second)));
  }
 // access functions
 public :
  mapped_type& at(const key_type& k) {
   return data.at(k);
  }
  const mapped_type& at(const key_type& k)const {
   return data.at(k);
  }
  mapped_type& operator[](const key_type& k) {
   return data[k];
  }
  const mapped_type& operator[](const key_type& k)const {
   return data[k];
  }
  iterator find(const key_type& k) {
   return data.find(k);
  }
  const_iterator find(const key_type& k)const {
   return data.find(k);
  }
 public :
  void swap(map_type& x) {
   data.swap(x);
  }
 public :
  map_deque() {
  }
 ~map_deque() {
  }
};

//
// FREQUENCY MAP
//

template<class K>
class frequency_map {
 public :
  typedef K key_type;
  typedef size_t mapped_type;
  typedef size_t size_type;
  typedef std::pair<key_type, mapped_type> value_type;
  typedef std::map<key_type, mapped_type> map_type;
 public :
  typedef typename map_type::iterator iterator;
  typedef typename map_type::const_iterator const_iterator;
  typedef typename map_type::reverse_iterator reverse_iterator;
  typedef typename map_type::const_reverse_iterator const_reverse_iterator;
 private :
  map_type data;
 // iterator functions
 public :
  iterator begin(void) {
   return data.begin();
  }
  iterator end(void) {
   return data.end();
  }
  const_iterator begin()const {
   return data.begin();
  }
  const_iterator end()const {
   return data.end();
  }
  const_iterator cbegin()const {
   return data.begin();
  }
  const_iterator cend()const {
   return data.cend();
  }
  reverse_iterator rbegin(void) {
   return data.rbegin();
  }
  reverse_iterator rend(void) {
   return data.rend();
  }
  const_reverse_iterator rbegin(void)const {
   return data.rbegin();
  }
  const_reverse_iterator rend(void)const {
   return data.rend();
  }
  const_reverse_iterator crbegin(void)const {
   return data.crbegin();
  }
  const_reverse_iterator crend(void)const {
   return data.crend();
  }
 public :
  mapped_type count(const key_type& key)const {
   auto iter = data.find(key);
   return (iter == data.end() ? static_cast<mapped_type>(0) : iter->second);
  }
  bool empty(void)const {
   return data.empty();
  }
  size_type size(void)const {
   return data.size();
  }
 // insertion function
 public :
  std::pair<iterator, bool> insert(const key_type& key) {
   return data.insert(std::make_pair(key, static_cast<size_type>(0)));
  }
  void increment(const key_type& key) {
   insert(key).first->second++;
  }
 // removal functions
 public :
  void clear(void) {
   data.clear();
  }
  void erase(iterator position) {
   data.erase(position);
  }
  size_type erase(const key_type& k) {
   return data.erase(k);
  }
  void erase(iterator first, iterator last) {
   data.erase(first, last);
  }
 public :
  frequency_map() {
  }
 ~frequency_map() {
  }
};

//
// TESTS
//

void test_map_deque(void);
void test_frequency_map(void);

#endif
