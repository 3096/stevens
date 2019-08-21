#ifndef __XENTAX_KARY_H
#define __XENTAX_KARY_H

class kary_node;
class kary_tree;
class kary_iterator;

class kary_tree {
 private :
  struct kary_node {
   typedef kary_node& reference;
   typedef kary_node* pointer;
   friend class kary_tree;
   pointer parent;
   std::deque<pointer> children;
   kary_node() : parent(nullptr) {}
   virtual ~kary_node() {}
   virtual const std::type_info& type(void)const = 0;
   virtual pointer clone(void)const = 0;
  };
  template<class T>
  struct node_type : public kary_node {
   T held;
   node_type(const T& value) : held(value) {}
   virtual const std::type_info& type(void)const { return typeid(T); }
   virtual kary_node* clone(void)const { return new node_type(held); }
   private : node_type(const node_type&);
   private : node_type& operator =(const node_type&);
  };
 public :
  class kary_iterator {
   public :
    typedef kary_node::reference reference;
    typedef kary_node::pointer pointer;
   private :
    friend class kary_tree;
    pointer node;
   public :
    kary_iterator& operator =(const kary_iterator& other)
    {
     if(this == &other) return *this;
     node = other.node;
     return *this;
    }
    bool operator ==(const kary_iterator& other)const { return (node == other.node); }
    bool operator !=(const kary_iterator& other)const { return (node != other.node); }
   public :
    reference operator *(void) { return *node; }
    pointer operator ->(void) { return (&**this); }
   public :
    kary_iterator() : node(nullptr) {}
    kary_iterator(kary_node* n) : node(n) {}
    kary_iterator(const kary_iterator& other) : node(other.node) {}
   ~kary_iterator() {}
  };
 public :
  typedef kary_iterator iterator;
  typedef size_t size_type;
 private :
  kary_node* root;
  size_type  elem;
 public :
  kary_tree& operator =(const kary_tree& kt)
  {
   if(this == &kt) return *this;
   return *this;
  }
  kary_tree& operator =(kary_tree&& kt)
  {
   return *this;
  }
 public :
  void clear(void)
  {
   if(root) delete root;
   root = nullptr;
   elem = 0;
  }
  size_type size(void)const
  {
   return elem;
  }
 public :
  iterator begin(void) { return iterator(root); }
  iterator end(void) { return iterator(nullptr); }
 public :
  template<class T>
  iterator insert(iterator parent, const T& item)
  {
   kary_node* node = new node_type<T>(item);
   if(parent != end()) {
      node->parent = parent.node;
      node->parent->children.push_back(node);
     }
   else root = node; // double check to make sure no elements

   elem++;
   return iterator(node);
  }
 public :
  kary_tree() : root(nullptr) {}
  kary_tree(const kary_tree& kt)
  {
  }
  kary_tree(kary_tree&& kt)
  {
  }
 ~kary_tree() { clear(); }
};

#endif
