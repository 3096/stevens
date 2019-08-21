#ifndef __XENTAX_SKELETON_H
#define __XENTAX_SKELETON_H

#define INVALID_JOINT 0xFFFFFFFF

struct JOINT {
 std::string name;
 uint32 parent;
 boost::shared_array<float> matrix;
 float rel_x;
 float rel_y;
 float rel_z;
};

struct JOINTVISITOR {
 typedef const JOINT& argument1;
 typedef uint32 argument2;
 typedef bool return_type;
};

class SKELETON {
 private :
  struct TREENODE {
   public :
    JOINT joint;
    std::deque<uint32> children;
   public :
    TREENODE& operator =(const TREENODE& node);
   public :
    TREENODE();
    TREENODE(const TREENODE& node);
   ~TREENODE();
  };
 public :
  typedef TREENODE node_t;
  typedef std::map<uint32, TREENODE> tree_t;
  typedef std::map<uint32, TREENODE>::iterator tree_iterator;
  typedef std::map<uint32, TREENODE>::const_iterator const_tree_iterator;
 private :
  std::string id;
  uint32 root;
  tree_t tree;
 public :
  bool insert(uint32 index, const JOINT& joint);
  bool remove(uint32 index);
  uint32 joints(void)const { return tree.size(); }
  void set_identifier(const char* name) { id = name; }
  const char* get_identifier(void)const { return id.c_str(); }
 private :
  template<class T>
  void traverse(T& function, uint32 index)const
  {
   const_tree_iterator iter = tree.find(index);
   if(iter == tree.end()) return;
   if(!function(iter->second.joint, index)) return;
   for(size_t i = 0; i < iter->second.children.size(); i++) traverse(function, iter->second.children[i]);
  }
 public :
  template<class T>
  void traverse(T& function)const
  {
   traverse(function, root);
  }
 public :
  void PrintJointTree(std::stringstream& ss)const;
  void PrintJointTree(std::stringstream& ss, uint32 index, uint32 level)const;
  void PrintColladaNodeHeirarchy(const std::string& id, std::stringstream& ss)const;
  void PrintColladaNodeHeirarchy(const std::string& id, std::stringstream& ss, uint32 index, uint32 level)const;
  void PrintColladaJoints(const std::string& id, std::stringstream& ss)const;
  void PrintColladaJoints(std::stringstream& ss, uint32 index)const;
  void PrintColladaBindMatrices(const std::string& id, std::stringstream& ss)const;
  void PrintColladaBindMatrices(std::stringstream& ss, uint32 index)const;
 public :
  SKELETON(const char* name = "skeleton");
 ~SKELETON();
};

typedef std::deque<SKELETON> SKELETON_LIST;
typedef SKELETON_LIST SKELETON_LIST_ITERATOR;

#endif
