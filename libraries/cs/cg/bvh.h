#ifndef __CS_CG_BVH_H
#define __CS_CG_BVH_H

// STL Headers
#include<cmath>
#include<limits>
#include<vector>

// CS Headers
#include<cs/math/ray.h>
#include<cs/cg/aabb.h>

namespace cs { namespace cg {

using cs::math::ray3D;

struct nonindexed_triangle_mesh_object_tag {};
struct indexed_triangle_mesh_object_tag {};

template<class T>
class AABB_tree {
 private :
  struct AABB_node {
   min_max_AABB<T> aabb;
   uint32 params[2];
  };
  typedef AABB_node node_t;
  std::vector<node_t> tree;
 public :
  AABB_tree()
  {
  }
  AABB_tree(const AABB_tree<T>& other)
  {
   tree = other.tree;
  }
  AABB_tree(AABB_tree<T>&& other)
  {
   tree = std::move(other.tree);
  }
 ~AABB_tree()
  {
  }
 public :
  AABB_tree<T>& operator =(const AABB_tree<T>& other)
  {
   if(this == &other) return *this;
   tree = other.tree;
   return *this;
  }
  AABB_tree<T>& operator =(AABB_tree<T>&& other)
  {
   if(this == &other) return *this;
   tree = std::move(other.tree);
   return *this;
  }
 public :
  size_t size(void)const { return tree.size(); }
 public :
  template<class Iterator, class ZeroTest>
  bool construct(Iterator from, Iterator to, ZeroTest zerotest)
  {
   return construct(from, to, zerotest, Iterator::object_category());
  }
  template<class Iterator, class ZeroTest>
  bool construct(Iterator from, Iterator to, ZeroTest zerotest, indexed_triangle_mesh_object_tag)
  {
   // number of faces
   auto distance = std::distance(from, to);
   if(distance < 1) return false;
   uint32 n_tris = static_cast<uint32>(distance);

   // binning constants
   auto BIN_LIMIT_TEST = [](auto a) { return a < 4; };
   const uint32 n_bins = 8;
   const uint32 n_part = 7;

   // constants
   const T MINVALUE = std::numeric_limits<T>::lowest();
   const T MAXVALUE = std::numeric_limits<T>::max();

   //
   // PHASE #0
   // INITIALIZATION
   //

   // start with root node
   tree.push_back(node_t());

   // special case
   if(BIN_LIMIT_TEST(n_tris))
     {
      // set parameters
      tree.back().params[0] = 0x80000000ul;
      tree.back().params[1] = n_tris;
      // set AABB
      min_max_AABB<T>& aabb = tree.back().aabb;
      aabb.min[0] = aabb.min[1] = aabb.min[2] =  FLT_MAX;
      aabb.max[0] = aabb.max[1] = aabb.max[2] = -FLT_MAX;
      for(Iterator iter = from; iter != to; iter++) {
          aabb.grow(iter.vertex(0));
          aabb.grow(iter.vertex(1));
          aabb.grow(iter.vertex(2));
         }
      return true;
     }

   // per-bin data
   min_max_AABB<T> binlist[n_bins];
   uint32 bintris[n_bins];

   // per-partition data
   T costs[n_part];
   uint32 NL[n_part];
   uint32 NR[n_part];
   min_max_AABB<T> BL[n_part];
   min_max_AABB<T> BR[n_part];

   //
   // PHASE #1
   // PRE-COMPUTE PER-TRIANGLE DATA
   //

   // per-triangle data (AABBs, centroids)
   std::vector<vector3D<T>> clist(n_tris);
   std::vector<min_max_AABB<T>> blist(n_tris);

   // for each face
   for(uint32 i = 0; i < n_tris; i++) {
       blist[i].from(from.vertex(i, 0), from.vertex(i, 1), from.vertex(i, 2));
       clist[i] = centroid(blist[i]);
      }

   //
   // PHASE #2
   // NON-RECURSIVE SPACIAL PARITIONING
   //

   struct BVHSTACKITEM {
    uint32 index;
    uint32 face_index1;
    uint32 face_index2;
   };

   // start with root node
   std::deque<BVHSTACKITEM> stack;
   stack.push_front(BVHSTACKITEM());
   stack.front().index = 0;
   stack.front().face_index1 = 0;
   stack.front().face_index2 = n_tris;

   // non-recursive partitioning
   while(stack.size())
        {
         // pop item from stack
         BVHSTACKITEM item = stack.front();
         uint32 node = item.index;
         uint32 face_index1 = item.face_index1;
         uint32 face_index2 = item.face_index2;
         stack.pop_front();

         //
         // STEP #1
         // CALCULATE PARTITION BOUNDS
         //

         // per-partition data (triangle bounds, centroid bounds)
         min_max_AABB<T> tbounds(MAXVALUE, MINVALUE); // article calls vb (voxel bounds)
         min_max_AABB<T> cbounds(MAXVALUE, MINVALUE); // article calls cb (all centroid bounds)
         for(uint32 i = face_index1; i < face_index2; i++) {
             tbounds.grow(blist[i]);
             cbounds.grow(clist[i]);
            }

         //
         // STEP #2
         // DOMINANT AXIS
         //

         // choose dominant axis from "longest-axis" of centroid bounds
         T dcb[3];
         uint32 axis = cs::cg::dominator(cbounds, dcb);

         // this node is a leaf node
         if(zerotest(dcb[axis])) {
            tree[node].aabb = tbounds;
            tree[node].params[0] = face_index1 | 0x80000000ul;
            tree[node].params[1] = face_index2 - face_index1;
            continue;
           }

         //
         // STEP #3
         // COMPUTE BINS
         //

         // binning constants
         const T k0 = cbounds.min[axis];
         const T k1 = n_bins*(1.0f - 1.0e-6f)/dcb[axis];

         // initialize bins
         for(uint32 i = 0; i < n_bins; i++) {
             bintris[i] = 0;
             binlist[i].from(MAXVALUE, MINVALUE);
            }

         // count number of triangles in bins and grow the bin AABBs to fit the triangles that are in each bin
         for(uint32 i = face_index1; i < face_index2; i++) {
             uint32 binindex = static_cast<uint32>(k1*(clist[i][axis] - k0));
             bintris[binindex]++;
             binlist[binindex].grow(blist[i]);
            }

         //
         // STEP #4
         // PARTITIONING
         //

         // L-side computation
         NL[0] = bintris[0];
         BL[0] = binlist[0];
         for(uint32 i = 1; i < n_part; i++) {
             NL[i] = NL[i - 1] + bintris[i];
             BL[i] = binlist[i];
             BL[i].grow(BL[i - 1]);
            }

         // R-side computation
         uint32 j = n_part;
         uint32 k = n_part - 1;
         NR[k] = bintris[j];
         BR[k] = binlist[j];
         j = k;
         for(uint32 i = 1; i < n_part; i++) {
             k = j - 1;
             NR[k] = NR[j] + bintris[j];
             BR[k] = binlist[j];
             BR[k].grow(BR[j]);
             j = k;
            }

         // compute index of best partition
         uint32 best_index = 0;
         costs[0] = NL[0]*half_surface_area(BL[0]) + NR[0]*half_surface_area(BR[0]);
         for(uint32 i = 1; i < n_part; i++) {
             costs[i] = NL[i]*half_surface_area(BL[i]) + NR[i]*half_surface_area(BR[i]);
             if(costs[i] < costs[best_index]) best_index = i;
            }

         //
         // STEP #5
         // INDEX BUFFER SORTING
         //

         // sort index buffer
         uint32 L_count = 0;
         uint32 L_pivot = face_index1;
         uint32 R_pivot = face_index2 - 1;
         for(;;)
            {
             // i is R
             uint32 i = static_cast<uint32>(k1*(clist[L_pivot][axis] - k0));
             if(best_index < i)
               {
                for(;;)
                   {
                    // j is R
                    uint32 j = static_cast<uint32>(k1*(clist[R_pivot][axis] - k0));
                    if(best_index < j) R_pivot--;
                    // j is L
                    else
                      {
                       // swap faces
                       std::swap(from + L_pivot, from + R_pivot);
                       // swap per-face centroids
                       std::swap(clist[L_pivot][0], clist[R_pivot][0]);
                       std::swap(clist[L_pivot][1], clist[R_pivot][1]);
                       std::swap(clist[L_pivot][2], clist[R_pivot][2]);
                       // swap per-face AABBs
                       std::swap(blist[L_pivot], blist[R_pivot]);
                       // move pivot
                       R_pivot--;
                       L_count++;
                       break;
                      }
                   }
               }
             // i is L
             else
                L_count++;
         
             // stop early if possible
             if(L_count == NL[best_index]) break;
             L_pivot++;
            }

         //
         // STEP #8
         // DIVIDE AND CONQUER
         //

         // this node
         tree[node].aabb = tbounds;
         tree[node].params[0] = tree.size();
         tree[node].params[1] = tree.size() + 1;

         // L = leaf
         tree.push_back(node_t());
         if(BIN_LIMIT_TEST(NL[best_index])) {
            // set parameters
            tree.back().params[0] = (face_index1 | 0x80000000ul);
            tree.back().params[1] = NL[best_index];
            // set AABB
            min_max_AABB<T>& aabb = tree.back().aabb;
            aabb.from(MAXVALUE, MINVALUE);
            for(uint32 i = 0; i < NL[best_index]; i++) aabb.grow(blist[face_index1 + i]);
           }
         // L = node
         else {
            stack.push_front(BVHSTACKITEM());
            stack.front().index = tree[node].params[0];
            stack.front().face_index1 = face_index1;
            stack.front().face_index2 = face_index1 + L_count;
           }

         // R = leaf
         tree.push_back(node_t());
         if(BIN_LIMIT_TEST(NR[best_index])) {
            // set parameters
            uint32 pivot = face_index1 + L_count;
            tree.back().params[0] = (pivot | 0x80000000ul);
            tree.back().params[1] = NR[best_index];
            // set AABB
            min_max_AABB<T>& aabb = tree.back().aabb;
            aabb.from(MAXVALUE, MINVALUE);
            for(uint32 i = 0; i < NR[best_index]; i++) aabb.grow(blist[pivot + i]);
           }
         // R = node
         else {
            stack.push_front(BVHSTACKITEM());
            stack.front().index = tree[node].params[1];
            stack.front().face_index1 = face_index1 + L_count;
            stack.front().face_index2 = face_index2;
           }
        }

   return true;
  }
  template<class Iterator, class ZeroTest>
  bool construct(Iterator from, Iterator to, ZeroTest zerotest, nonindexed_triangle_mesh_object_tag)
  {
   return true;
  }
 private :

 public :
  template<class ZeroTest>
  bool intersect(const ray3D<T>& ray, ZeroTest zerotest)
  {
   return false;
  }
};

}}

#endif
