#include "xentax.h"
#include "x_skel.h"

SKELETON::TREENODE::TREENODE()
{
}

SKELETON::TREENODE::TREENODE(const SKELETON::TREENODE& node)
{
 joint = node.joint;
 children.insert(children.end(), node.children.begin(), node.children.end());
}

SKELETON::TREENODE::~TREENODE()
{
}

SKELETON::TREENODE& SKELETON::TREENODE::operator =(const SKELETON::TREENODE& node)
{
 if(this == &node) return *this;
 children.clear();
 joint = node.joint;
 children.insert(children.end(), node.children.begin(), node.children.end());
 return *this;
}

SKELETON::SKELETON(const char* name) : id(name)
{
 root = INVALID_JOINT;
}

SKELETON::~SKELETON()
{
}

bool SKELETON::insert(uint32 index, const JOINT& joint)
{
 // check if already inserted
 tree_t::iterator temp = tree.find(index);
 if(temp != tree.end()) return error("InsertJoint: Joint already exists.");

 // insert root
 if(joint.parent == INVALID_JOINT) {
    node_t node;
    node.joint = joint;
    tree.insert(tree_t::value_type(index, node));
    root = index;
    return true;
   }

 // find parent
 tree_t::iterator parent = tree.find(joint.parent);
 if(parent == tree.end()) return error("InsertJoint: Can't find parent joint.");

 // insert node
 node_t node;
 node.joint = joint;
 tree.insert(tree_t::value_type(index, node));

 // insert into parent's adjacency list
 parent->second.children.push_back(index);
 return true;
}

bool SKELETON::remove(uint32 index)
{
 // TODO
 return true;
}

void SKELETON::PrintJointTree(std::stringstream& ss)const
{
 PrintJointTree(ss, root, 0);
}

void SKELETON::PrintJointTree(std::stringstream& ss, uint32 index, uint32 level)const
{
 // find node
 const_tree_iterator iter = tree.find(index);
 if(iter == tree.end()) return;

 // build string
 const node_t& node = iter->second;
 for(size_t i = 0; i < level; i++) ss << " ";
 ss << node.joint.name << std::endl;

 // visit children of joint tree node
 for(size_t i = 0; i < node.children.size(); i++) PrintJointTree(ss, node.children[i], level + 1);
}

void SKELETON::PrintColladaNodeHeirarchy(const std::string& id, std::stringstream& ss)const
{
 PrintColladaNodeHeirarchy(id, ss, root, 0);
}

void SKELETON::PrintColladaNodeHeirarchy(const std::string& id, std::stringstream& ss, uint32 index, uint32 level)const
{
 // NOTES:
 // Unlike in Collada specification, you must have id attributes for all joint nodes if you want to see the
 // names in 3DS MAX. Solution is to just define id and sid together at the same time.

 // find node
 const_tree_iterator iter = tree.find(index);
 if(iter == tree.end()) return;

 // determine spacing
 std::string spacing = "  ";
 for(size_t i = 0; i < level; i++) spacing += ' ';

 // print node
 const node_t& node = iter->second;
 if(index == root) ss << spacing << "<node id=\"" << id << "\" sid=\"" << node.joint.name << "\">" << std::endl;
 else ss << spacing << "<node id=\"" << node.joint.name << "\" sid=\"" << node.joint.name << "\" type=\"JOINT\">" << std::endl;

 // position node
 ss << spacing << " <matrix>" << std::endl;
 ss << spacing << "  " << "1 0 0 " << node.joint.rel_x << std::endl;
 ss << spacing << "  " << "0 1 0 " << node.joint.rel_y << std::endl;
 ss << spacing << "  " << "0 0 1 " << node.joint.rel_z << std::endl;
 ss << spacing << "  " << "0 0 0 1" << std::endl;
 ss << spacing << " </matrix>" << std::endl;

 // print children
 for(size_t i = 0; i < node.children.size(); i++)
     PrintColladaNodeHeirarchy(id, ss, node.children[i], level + 1);

 // finish node
 ss << spacing << "</node>" << std::endl;
}

void SKELETON::PrintColladaJoints(const std::string& id, std::stringstream& ss)const
{
 ss << "    <source id=\"" << id << "_joints\">" << std::endl;

 ss << "     <Name_array count=\"" << tree.size() << "\">" << std::endl;
 PrintColladaJoints(ss, root);
 ss << "     </Name_array>" << std::endl;

 ss << "     <technique_common>" << std::endl;
 ss << "      <acessor source=\"#" << id << "_joints\" count=\"" << tree.size() << "\" stride=\"1\">" << std::endl;
 ss << "       <param name=\"JOINT\" type=\"name\" />" << std::endl;
 ss << "      </acessor>" << std::endl;
 ss << "     </technique_common>" << std::endl;

 ss << "    </source>" << std::endl;
}

void SKELETON::PrintColladaJoints(std::stringstream& ss, uint32 index)const
{
 // find node
 const_tree_iterator iter = tree.find(index);
 if(iter == tree.end()) return;

 // print node
 const node_t& node = iter->second;
 ss << "      " << node.joint.name << std::endl;

 // print children
 for(size_t i = 0; i < node.children.size(); i++)
     PrintColladaJoints(ss, node.children[i]);
}

void SKELETON::PrintColladaBindMatrices(const std::string& id, std::stringstream& ss)const
{
 ss << "    <source id=\"" << id << "_matrices\">" << std::endl;
 ss << "     <float_array count=\"" << 16*tree.size() << "\">" << std::endl;
 PrintColladaBindMatrices(ss, root);
 ss << "     </float_array>" << std::endl;

 ss << "     <technique_common>" << std::endl;
 ss << "      <acessor source=\"#" << id << "_matrices\" count=\"" << tree.size() << "\" stride=\"16\">" << std::endl;
 ss << "       <param name=\"INV_BIND_MATRIX\" type=\"float4x4\" />" << std::endl;
 ss << "      </acessor>" << std::endl;
 ss << "     </technique_common>" << std::endl;

 ss << "    </source>" << std::endl;
}

void SKELETON::PrintColladaBindMatrices(std::stringstream& ss, uint32 index)const
{
 // find node
 const_tree_iterator iter = tree.find(index);
 if(iter == tree.end()) return;

 // print node
 const node_t& node = iter->second;
 ss << "      ";
 ss << node.joint.matrix[ 0] << " ";
 ss << node.joint.matrix[ 1] << " ";
 ss << node.joint.matrix[ 2] << " ";
 ss << node.joint.matrix[ 3] << " ";
 ss << node.joint.matrix[ 4] << " ";
 ss << node.joint.matrix[ 5] << " ";
 ss << node.joint.matrix[ 6] << " ";
 ss << node.joint.matrix[ 7] << " ";
 ss << node.joint.matrix[ 8] << " ";
 ss << node.joint.matrix[ 9] << " ";
 ss << node.joint.matrix[10] << " ";
 ss << node.joint.matrix[11] << " ";
 ss << node.joint.matrix[12] << " ";
 ss << node.joint.matrix[13] << " ";
 ss << node.joint.matrix[14] << " ";
 ss << node.joint.matrix[14] << " " << std::endl;

 // print children
 for(size_t i = 0; i < node.children.size(); i++)
     PrintColladaBindMatrices(ss, node.children[i]);
}