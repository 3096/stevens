#ifndef __XENTAX_AMC_H
#define __XENTAX_AMC_H

#define AMC_VERTEX_POSITION 0x01
#define AMC_VERTEX_NORMAL   0x02
#define AMC_VERTEX_TANGENT  0x04
#define AMC_VERTEX_BINORMAL 0x08
#define AMC_VERTEX_WEIGHTS  0x10
#define AMC_VERTEX_UV       0x20
#define AMC_VERTEX_COLORS   0x40
#define AMC_VERTEX_SKELETON 0x80

#define AMC_INVALID_VERTEX_WMAP_INDEX 0xFFFF
#define AMC_INVALID_VERTEX_SKEL_INDEX 0xFFFF

#define AMC_UINT08 0x0
#define AMC_UINT16 0x1
#define AMC_UINT32 0x2
#define AMC_TRIANGLES   0x00
#define AMC_TRISTRIP    0x01
#define AMC_TRISTRIPCUT 0x02
#define AMC_CW  0x00
#define AMC_CCW 0x01

#define AMC_INVALID_SURFMAT 0xFFFF
#define AMC_INVALID_TEXTURE 0xFFFF
#define AMC_INVALID_JMAP_INDEX 0xFFFFFFFF

#define AMC_DIFFUSE_MAP     0x0001
#define AMC_SPECULAR_MAP    0x0002
#define AMC_BUMP_MAP        0x0004
#define AMC_NORMAL_MAP      0x0008
#define AMC_ALPHA_MAP       0x0010
#define AMC_LIGHT_MAP       0x0020
#define AMC_GLOSS_MAP       0x0040
#define AMC_ENVIRONMENT_MAP 0x0080
#define AMC_INVALID_MAP     0x8000

struct AMC_VERTEX {
 real32 vx, vy, vz, vw;
 real32 nx, ny, nz, nw;
 real32 tx, ty, tz, tw;
 real32 bx, by, bz, bw;
 real32 tu, tv;
 real32 wv[8];
 uint16 wi[8];
 uint16 si[8];
 real32 uv[8][2];
 real32 color[8][4];
};

struct AMC_VTXBUFFER {
 uint32 flags;                         // vertex buffer flags
 std::string name;                     // name of vertex buffer
 uint16 uvchan;                        // number of uv channels
 uint16 uvtype[8];                     // type of uv channels
 uint16 colors;                        // number of color channels
 uint32 elem;                          // number of vertices
 boost::shared_array<AMC_VERTEX> data; // vertices
};

struct AMC_IDXBUFFER {
 uint08 format;                  // data type
 uint08 type;                    // primitive type
 uint08 wind;                    // winding order
 uint08 reserved;                // reserved
 std::string name;               // name of index buffer
 uint32 elem;                    // number of indices
 boost::shared_array<char> data; // indices
};

struct AMC_JOINTMAP {
 std::deque<uint32> jntmap;
};

struct AMC_REFERENCE {
 uint32 vb_index; // vbuffer index
 uint32 vb_start; // start index of vbuffer
 uint32 vb_width; // number of vertices
 uint32 ib_index; // ibuffer index
 uint32 ib_start; // start index of ibuffer
 uint32 ib_width; // number of indices
 uint32 jm_index; // joint map index
};

struct AMC_SURFACE {
 std::string name;
 std::deque<AMC_REFERENCE> refs;
 uint16 surfmat;
};

struct AMC_IMAGEFILE {
 std::string filename;
};

struct AMC_SURFMAT {
 std::string name; // name
 uint08 twoside;   // double-sided
 uint08 unused1;   // reserved
 uint08 unused2;   // reserved
 uint08 unused3;   // reserved
 uint16 basemap;   // reference to texture index #1
 uint16 specmap;   // reference to texture index #2
 uint16 tranmap;   // reference to texture index #3
 uint16 bumpmap;   // reference to texture index #4
 uint16 normmap;   // reference to texture index #5
 uint16 lgthmap;   // reference to texture index #6
 uint16 envimap;   // reference to texture index #7
 uint16 glssmap;   // reference to texture index #8
 uint16 resmap1;   // reserved1 #9
 uint16 resmap2;   // reserved2 #10
 uint16 resmap3;   // reserved3 #11
 uint16 resmap4;   // reserved4 #12
 uint16 resmap5;   // reserved5 #13
 uint16 resmap6;   // reserved6 #14
 uint16 resmap7;   // reserved7 #15
 uint16 resmap8;   // reserved8 #16
 uint08 basemapchan; // reference to uv channel
 uint08 specmapchan; // reference to uv channel
 uint08 tranmapchan; // reference to uv channel
 uint08 bumpmapchan; // reference to uv channel
 uint08 normmapchan; // reference to uv channel
 uint08 lghtmapchan; // reference to uv channel
 uint08 envimapchan; // reference to uv channel
 uint08 glssmapchan; // reference to uv channel
 uint08 resmapchan1; // reserved1
 uint08 resmapchan2; // reserved2
 uint08 resmapchan3; // reserved3
 uint08 resmapchan4; // reserved4
 uint08 resmapchan5; // reserved5
 uint08 resmapchan6; // reserved6
 uint08 resmapchan7; // reserved7
 uint08 resmapchan8; // reserved8
};

//
// Skeleton Support
//

#define AMC_JOINT_FORMAT_ABSOLUTE   0x0001
#define AMC_JOINT_FORMAT_RELATIVE   0x0002
#define AMC_JOINT_FORMAT_MATRIX     0x0004
#define AMC_JOINT_FORMAT_QUATERNION 0x0008
#define AMC_JOINT_FORMAT_TIP_LENGTH 0x0010
#define AMC_JOINT_FORMAT_X_BONEAXIS 0x0020
#define AMC_JOINT_FORMAT_Y_BONEAXIS 0x0040
#define AMC_JOINT_FORMAT_Z_BONEAXIS 0x0080
#define AMC_INVALID_JOINT           0xFFFFFFFFul

struct AMC_JOINT {
 std::string name;
 uint32 id;
 uint32 parent;
 union {
  real32 m_abs[16];
  real32 m_rel[16];
 };
 union {
  real32 q_abs[4];
  real32 q_rel[4];
 };
 union {
  real32 p_abs[4];
  real32 p_rel[4];
 };
};

//
// Skeleton I Support
// This type of skeleton stores the tree, so the order in which joints are seen
// becomes important. For example you cannot insert a child joint if its parent
// is not already in the deque.
//

struct AMC_JOINTNODE {
 AMC_JOINT joint;
 std::deque<uint32> children;
};

struct AMC_SKELETON {
 uint08 format;
 std::string name;
 std::deque<AMC_JOINTNODE> tree;
};

bool InsertJoint(AMC_SKELETON& skl, const AMC_JOINT& jnt);

//
// Skeleton II Support
//

struct AMC_SKELETON2 {
 uint16 format;
 real32 tiplen;
 std::string name;
 std::deque<AMC_JOINT> joints;
};

uint32 GetJointIndexFromID(const AMC_SKELETON2& src, uint16 id);
bool SkeletonRelativeToAbsolute(AMC_SKELETON2& dst, const AMC_SKELETON2& src);

//
// Model Support
//

struct ADVANCEDMODELCONTAINER {
 std::deque<AMC_VTXBUFFER> vblist; // VTX buffers
 std::deque<AMC_IDXBUFFER> iblist; // IDX buffers
 std::deque<AMC_IMAGEFILE> iflist; // image files
 std::deque<AMC_SURFACE> surfaces; // surface list
 std::deque<AMC_SURFMAT> surfmats; // surface materials
 std::deque<AMC_SKELETON> skllist; // skeletons
 std::deque<AMC_SKELETON2> skllist2; // skeletons
 std::deque<AMC_JOINTMAP> jmaplist; // joint maps
};

ADVANCEDMODELCONTAINER TransformAMC(const ADVANCEDMODELCONTAINER& data);
bool SaveAMC(LPCTSTR path, LPCTSTR name, const ADVANCEDMODELCONTAINER& data);
bool SaveLWO(LPCTSTR path, LPCTSTR name, const ADVANCEDMODELCONTAINER& data);
bool SaveOBJ(LPCTSTR path, LPCTSTR name, const ADVANCEDMODELCONTAINER& data);
bool TestAMC(void);

struct AMCTriangleList {
 boost::shared_array<char> data;
 uint08 type;
 uint32 n_indices;
 uint32 triangles;
};

template<class T>
bool ConvertTriStripToTriangleList(const T* src, uint32 n, AMCTriangleList& dst)
{
 // compute number of triangles
 if(!src || (n < 3)) return false;
 uint32 n_triangles = n - 2;

 // data type we are using
 if(is_uint16<T>::value) dst.type = AMC_UINT16;
 else if(is_uint32<T>::value) dst.type = AMC_UINT32;

 // initialize destination data
 dst.triangles = n_triangles;
 dst.n_indices = n_triangles * 3;
 dst.data.reset(new char[dst.n_indices * sizeof(T)]);

 // initialize second pass data
 T* ptr = reinterpret_cast<T*>(dst.data.get());
 uint32 dst_index = 0;
 uint32 src_index = 0;

 // copy first triangle
 T a = src[src_index++];
 T b = src[src_index++];
 T c = src[src_index++];
 ptr[dst_index++] = a;
 ptr[dst_index++] = b;
 ptr[dst_index++] = c;
    
 // copy other triangles
 while(src_index < n) {
     a = b;
     b = c;
     if(src_index % 2) {
        c = src[src_index++];
        ptr[dst_index++] = a;
        ptr[dst_index++] = c;
        ptr[dst_index++] = b;
       }
     else {
        c = src[src_index++];
        ptr[dst_index++] = a;
        ptr[dst_index++] = b;
        ptr[dst_index++] = c;
       }
    }

 return true;
}

template<class T>
bool ConvertStripCutToTriangleList(const T* src, uint32 n, AMCTriangleList& dst, T terminal)
{
 // validate
 if(!src || !n) return false;
 if(n < 3) return false;

 // to compute number of triangles
 size_t n_triangles = 0;

 // PASS #1
 // COMPUTE NUMBER OF TRIANGLES
 size_t index0 = 0;
 size_t index1 = 0;
 for(;;)
    {
     // stop when right index reaches terminator or EOA
     while((index1 < n) && (src[index1] != terminal))
           index1++;

     // must have at least three indices to process triangle strip
     size_t distance = index1 - index0;
     if(distance > 2)
       {
        // check first triangle
        T a = src[index0 + 0];
        T b = src[index0 + 1];
        T c = src[index0 + 2];
        if((a != b) && (a != c) && (b != c)) n_triangles++;

        // check other triangles
        // note that only comparisons with the new value 'c' are necessary
        for(size_t i = index0 + 3; i < index1; i++) {
            a = b;
            b = c;
            c = src[i];
            if((a != c) && (b != c)) n_triangles++;
           }
       }

     // stop processing or continue
     if(index1 == n) break;
     index0 = ++index1;
    }

  // nothing to do
 if(n_triangles < 1) return false;

 // initialize destination data
 if(is_uint08<T>::value) dst.type = AMC_UINT08;
 else if(is_uint16<T>::value) dst.type = AMC_UINT16;
 else if(is_uint32<T>::value) dst.type = AMC_UINT32;
 dst.triangles = n_triangles;
 dst.n_indices = n_triangles * 3;
 dst.data.reset(new char[dst.n_indices * sizeof(T)]);

 // PASS #2
 // CONVERT TRISTRIPS TO TRIANGLES
 size_t result_index = 0;
 index0 = 0;
 index1 = 0;
 for(;;)
    {
     // stop when right index reaches terminator or EOA
     while((index1 < n) && (src[index1] != terminal))
           index1++;

     // must have at least three indices to process triangle strip
     size_t distance = index1 - index0;
     if(distance > 2)
       {
        // save 1st triangle
        T a = src[index0 + 0];
        T b = src[index0 + 1];
        T c = src[index0 + 2];
        if((a != b) && (a != c) && (b != c)) {
           T* ptr = reinterpret_cast<T*>(dst.data.get());
           ptr[result_index++] = a;
           ptr[result_index++] = b;
           ptr[result_index++] = c;
          }

        // save other triangles
        for(size_t i = 3; i < distance; i++)
           {
            // check triangle
            a = b;
            b = c;
            c = src[index0 + i];
            if((a != c) && (b != c)) {
               if(i % 2) {
                  T* ptr = reinterpret_cast<T*>(dst.data.get());
                  ptr[result_index++] = a;
                  ptr[result_index++] = c;
                  ptr[result_index++] = b;
                 }
               else {
                  T* ptr = reinterpret_cast<T*>(dst.data.get());
                  ptr[result_index++] = a;
                  ptr[result_index++] = b;
                  ptr[result_index++] = c;
                 }
              }
           }
       }

     // stop processing or continue
     if(index1 == n) break;
     index0 = ++index1;
    }

 return true;
}

template<class T>
std::vector<T> tristrip_to_trilist(const T* ibuffer, size_t n_indices)
{
 // nothing to do
 std::vector<T> retval;
 if(!ibuffer || (n_indices < 3)) return retval;

 // to compute number of triangles
 size_t n_triangles = 0;

 // PASS #1
 // COMPUTE NUMBER OF NON-DEGENERATE TRIANGLES

 // check first triangle
 T a = ibuffer[0];
 T b = ibuffer[1];
 T c = ibuffer[2];
 if((a != b) && (a != c) && (b != c)) n_triangles++;

 // check other triangles
 // note that only comparisons with the new value 'c' are necessary
 for(size_t i = 3; i < n_indices; i++) {
     a = b;
     b = c;
     c = ibuffer[i];
     if((a != c) && (b != c)) n_triangles++;
    }

  // nothing to do
 if(n_triangles < 1) return retval;

 // resize vector
 size_t n = n_triangles*3;
 retval.resize(n);

 // PASS #2
 // CONVERT TRISTRIPS TO NON-DEGENERATE TRIANGLES

 // save 1st triangle
 size_t result_index = 0;
 a = ibuffer[0];
 b = ibuffer[1];
 c = ibuffer[2];
 if((a != b) && (a != c) && (b != c)) {
    retval[result_index++] = a;
    retval[result_index++] = b;
    retval[result_index++] = c;
   }

 // save other triangles
 for(size_t i = 3; i < n_indices; i++)
    {
     a = b;
     b = c;
     c = ibuffer[i];
     if((a != c) && (b != c)) {
        if(i % 2) {
           retval[result_index++] = a;
           retval[result_index++] = c;
           retval[result_index++] = b;
          }
        else {
           retval[result_index++] = a;
           retval[result_index++] = b;
           retval[result_index++] = c;
          }
       }
    }

 return retval;
}

template<class T>
std::vector<T> tristripcut_to_trilist(const T* ibuffer, size_t n_indices, T terminator)
{
 // nothing to do
 std::vector<T> retval;
 if(!ibuffer || (n_indices < 3)) return retval;

 // to compute number of triangles
 size_t n_triangles = 0;

 // PASS #1
 // COMPUTE NUMBER OF TRIANGLES
 size_t index0 = 0;
 size_t index1 = 0;
 for(;;)
    {
     // stop when right index reaches terminator or EOA
     while((index1 < n_indices) && (ibuffer[index1] != terminator))
           index1++;

     // must have at least three indices to process triangle strip
     size_t distance = index1 - index0;
     if(distance > 2)
       {
        // check first triangle
        T a = ibuffer[index0 + 0];
        T b = ibuffer[index0 + 1];
        T c = ibuffer[index0 + 2];
        if((a != b) && (a != c) && (b != c)) n_triangles++;

        // check other triangles
        // note that only comparisons with the new value 'c' are necessary
        for(size_t i = index0 + 3; i < index1; i++) {
            a = b;
            b = c;
            c = ibuffer[i];
            if((a != c) && (b != c)) n_triangles++;
           }
       }

     // stop processing or continue
     if(index1 == n_indices) break;
     index0 = ++index1;
    }

  // nothing to do
 if(n_triangles < 1) return retval;

 // resize vector
 size_t n = n_triangles*3;
 retval.resize(n);

 // PASS #2
 // CONVERT TRISTRIPS TO TRIANGLES
 size_t result_index = 0;
 index0 = 0;
 index1 = 0;
 for(;;)
    {
     // stop when right index reaches terminator or EOA
     while((index1 < n_indices) && (ibuffer[index1] != terminator))
           index1++;

     // must have at least three indices to process triangle strip
     size_t distance = index1 - index0;
     if(distance > 2)
       {
        // save 1st triangle
        T a = ibuffer[index0 + 0];
        T b = ibuffer[index0 + 1];
        T c = ibuffer[index0 + 2];
        if((a != b) && (a != c) && (b != c)) {
           retval[result_index++] = a;
           retval[result_index++] = b;
           retval[result_index++] = c;
          }

        // save other triangles
        for(size_t i = 3; i < distance; i++)
           {
            // check triangle
            a = b;
            b = c;
            c = ibuffer[index0 + i];
            if((a != c) && (b != c)) {
               if(i % 2) {
                  retval[result_index++] = a;
                  retval[result_index++] = c;
                  retval[result_index++] = b;
                 }
               else {
                  retval[result_index++] = a;
                  retval[result_index++] = b;
                  retval[result_index++] = c;
                 }
              }
           }
       }

     // stop processing or continue
     if(index1 == n_indices) break;
     index0 = ++index1;
    }

 return retval;
}

#endif
