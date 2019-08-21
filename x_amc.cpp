#include "xentax.h"
#include "x_math.h"
#include "x_lwo.h"
#include "x_amc.h"

//
// SUPPORT FUNCTIONS
//

#pragma region GENERAL_SUPPORT

// OBJ files loaded in Lightwave with positions that are really small, such as
// 1.0e-27 will crash it.
inline real32 clip(float v)
{
 return (fabs(v) < 1.0e-7f ? 0.0f : v);
}

#pragma endregion

//
// Skeleton I Support
//

#pragma region SKELETON_I 

bool InsertJoint(AMC_SKELETON& skl, const AMC_JOINT& jnt)
{
 // insert node (no parent)
 if(jnt.parent == AMC_INVALID_JOINT) {
    if(skl.tree.size()) return error("InsertJoint: A skeleton cannot have multiple root nodes.");
    AMC_JOINTNODE node;
    node.joint = jnt;
    skl.tree.push_back(node);
   }
 // insert node
 else {
   if(!(jnt.parent < skl.tree.size())) return error("InsertJoint: Parent node does not exist.");
   AMC_JOINTNODE node;
   node.joint = jnt;
   skl.tree.push_back(node);
   skl.tree[jnt.parent].children.push_back(skl.tree.size() - 1);
  }

 return true;
}

#pragma endregion

//
// SKELETON II SUPPORT
//

uint32 GetJointIndexFromID(const AMC_SKELETON2& src, uint16 id)
{
 if(id < src.joints.size() && src.joints[id].id == id) return id; // optimization
 for(uint32 i = 0; i < src.joints.size(); i++) if(src.joints[i].id == id) return i; // linear search
 return AMC_INVALID_JOINT;
}

bool SkeletonRelativeToAbsolute(AMC_SKELETON2& dst, const AMC_SKELETON2& src)
{
 // validate data
 bool is_abs = ((src.format & AMC_JOINT_FORMAT_ABSOLUTE) != 0);
 bool is_rel = ((src.format & AMC_JOINT_FORMAT_RELATIVE) != 0);
 if(is_abs && is_rel) return error("Invalid skeleton flags.", __LINE__);
 if(!src.joints.size()) return error("You must have at least one joint to convert a skeleton.", __LINE__);

 // copy format
 dst.format = src.format;

 // must convert
 if(is_rel)
   {
    // find root joint
    uint32 root = AMC_INVALID_JOINT;
    for(size_t i = 0; i < src.joints.size(); i++) {
        if(src.joints[i].parent == AMC_INVALID_JOINT) {
           root = i;
           break;
          }
       }

    // allocate dst data
    dst.joints.resize(src.joints.size());

    // copy root data
    for(size_t i = 0; i < 16; i++)  dst.joints[root].m_rel[i] = src.joints[root].m_rel[i];
    dst.joints[root].p_rel[0] = src.joints[root].p_rel[0];
    dst.joints[root].p_rel[1] = src.joints[root].p_rel[1];
    dst.joints[root].p_rel[2] = src.joints[root].p_rel[2];

    // insert root into stack
    std::deque<uint32> stack;
    stack.push_front(root);

    // traverse
    while(stack.size())
         {
          // get parent index
          uint32 parent = stack.front();
          stack.pop_front();

          // get parent matrix transform
          // make sure to pull from "dst" as "src" always contains "relative" transforms
          real32 PM[16];
          for(size_t i = 0; i < 16; i++) PM[i] = dst.joints[parent].m_rel[i];

          // find direct descendents
          for(size_t i = 0; i < src.joints.size(); i++) {
              if(src.joints[i].parent == src.joints[parent].id)
                {
                 // get child matrix transform
                 real32 CM[16];
                 for(size_t j = 0; j < 16; j++) CM[j] = src.joints[i].m_rel[j];

                 // multiply PM * CM
                 real32 X[16];
                 cs::math::matrix4x4_mul(X, PM, CM);

                 // assign new child matrix
                 for(size_t j = 0; j < 16; j++) dst.joints[i].m_rel[j] = X[j];
                 dst.joints[i].p_rel[0] = dst.joints[parent].p_rel[0] + src.joints[i].p_rel[0];
                 dst.joints[i].p_rel[1] = dst.joints[parent].p_rel[1] + src.joints[i].p_rel[1];
                 dst.joints[i].p_rel[2] = dst.joints[parent].p_rel[2] + src.joints[i].p_rel[2];

                 // insert children into stack
                 stack.push_front(i);
                }
             }
         }

    // change format
    dst.format &= ~AMC_JOINT_FORMAT_RELATIVE;
    dst.format |= AMC_JOINT_FORMAT_ABSOLUTE;
   }
 else
    return error("Skeleton format is either unknown or not specified", __LINE__);

 // copy values that don't change
 dst.tiplen = src.tiplen;
 dst.name = src.name;
 for(size_t i = 0; i < src.joints.size(); i++) {
     dst.joints[i].name = src.joints[i].name;
     dst.joints[i].id   = src.joints[i].id;
     dst.joints[i].parent = src.joints[i].parent;
    }

 return true;
}

//
// Lightwave Support
//

#pragma region LIGHTWAVE_SUPPORT_SKELETONS

struct AMC_JOINTVISITOR {
 typedef const AMC_SKELETON& argument1;
 typedef uint32 argument2;
 typedef bool return_type;
};

struct AMCJointVisitor : public AMC_JOINTVISITOR {
 public :
  uint32 n_pnts;
  uint32 n_pols;
  uint32 curr_bone;
  boost::shared_array<cs::math::matrix4x4<real32>> mats;
  boost::shared_array<lwVertex3D> pnts;
  boost::shared_array<lwSkelegon> pols;
 public :
  bool operator ()(const AMC_SKELETON& skeleton, uint32 index);
 public :
  AMCJointVisitor(const AMC_SKELETON& skeleton);
 ~AMCJointVisitor();
};

AMCJointVisitor::AMCJointVisitor(const AMC_SKELETON& skeleton)
{
 // create PNTS
 n_pnts = skeleton.tree.size();
 if(n_pnts) pnts.reset(new lwVertex3D[n_pnts]);

 // create POLS
 n_pols = (skeleton.tree.size() > 0 ? skeleton.tree.size() - 1 : 0);
 if(n_pols) pols.reset(new lwSkelegon[n_pols]);

 // create absolute transforms
 if(n_pnts) mats.reset(new cs::math::matrix4x4<real32>[n_pnts]);

 // set current POLS index
 curr_bone = 0;
}

AMCJointVisitor::~AMCJointVisitor()
{
}

bool AMCJointVisitor::operator ()(const AMC_SKELETON& skeleton, uint32 index)
{
 // validate
 if(index == AMC_INVALID_JOINT) return false; // joint must be valid
 if(!skeleton.tree.size()) return true; // must have at least one joint

 // save parent index
 uint32 parent = skeleton.tree[index].joint.parent;

 // save joint
 if(skeleton.format & AMC_JOINT_FORMAT_ABSOLUTE)
   {
    // absolute rotation matrix
    if(skeleton.format & AMC_JOINT_FORMAT_MATRIX) {
       for(uint32 i = 0; i < 16; i++)
           mats[index][i] = skeleton.tree[index].joint.m_abs[i];
      }
    else if(skeleton.format & AMC_JOINT_FORMAT_QUATERNION) {
       cs::math::quaternion<real32> q;
       q.imag(skeleton.tree[index].joint.q_abs[0], skeleton.tree[index].joint.q_abs[1], skeleton.tree[index].joint.q_abs[2]);
       q.real(skeleton.tree[index].joint.q_abs[3]);
       normalize(q);
       cs::math::quaternion_to_matrix4x4(&q.v[0], mats[index].get());
      }
    else
       return error("Joint rotation format unspecified.");

    // save translation component
    mats[index][ 3] = skeleton.tree[index].joint.p_abs[0];
    mats[index][ 7] = skeleton.tree[index].joint.p_abs[1];
    mats[index][11] = skeleton.tree[index].joint.p_abs[2];
    mats[index][15] = 1.0f;

    // already in absolute coordinates
    pnts[index].x = skeleton.tree[index].joint.p_abs[0];
    pnts[index].y = skeleton.tree[index].joint.p_abs[1];
    pnts[index].z = skeleton.tree[index].joint.p_abs[2];
   }
 else if(skeleton.format & AMC_JOINT_FORMAT_RELATIVE)
   {
    // no parent means absolute state = relative state
    if(parent == AMC_INVALID_JOINT)
      {
       // already in absolute coordinates
       pnts[index].x = skeleton.tree[index].joint.p_rel[0];
       pnts[index].y = skeleton.tree[index].joint.p_rel[1];
       pnts[index].z = skeleton.tree[index].joint.p_rel[2];

       // already in absolute coordinates
       if(skeleton.format & AMC_JOINT_FORMAT_MATRIX) {
          for(uint32 i = 0; i < 16; i++)
              mats[index][i] = skeleton.tree[index].joint.m_rel[i];
         }
       else if(skeleton.format & AMC_JOINT_FORMAT_QUATERNION) {
          cs::math::quaternion<real32> q;
          q.imag(skeleton.tree[index].joint.q_rel[0], skeleton.tree[index].joint.q_rel[1], skeleton.tree[index].joint.q_rel[2]);
          q.real(skeleton.tree[index].joint.q_rel[3]);
          normalize(q);
          cs::math::quaternion_to_matrix4x4(&q.v[0], mats[index].get());
         }
       else
          return error("Joint rotation format unspecified.");

       // save translation component
       mats[index][ 3] = skeleton.tree[index].joint.p_rel[0];
       mats[index][ 7] = skeleton.tree[index].joint.p_rel[1];
       mats[index][11] = skeleton.tree[index].joint.p_rel[2];
       mats[index][15] = 1.0f;
      }
    else
      {
       // get absolute parent transform
       cs::math::matrix4x4<real32> R1 = mats[parent];
       cs::math::matrix4x4<real32> R2;

       // get relative joint transform
       if(skeleton.format & AMC_JOINT_FORMAT_MATRIX) {
          for(uint32 i = 0; i < 16; i++)
              R2[i] = skeleton.tree[index].joint.m_rel[i];
         }
       else if(skeleton.format & AMC_JOINT_FORMAT_QUATERNION) {
          cs::math::quaternion<real32> q;
          q.imag(skeleton.tree[index].joint.q_rel[0], skeleton.tree[index].joint.q_rel[1], skeleton.tree[index].joint.q_rel[2]);
          q.real(skeleton.tree[index].joint.q_rel[3]);
          normalize(q);
          cs::math::quaternion_to_matrix4x4(&q.v[0], R2.get());
         }
       else
          return error("Joint rotation format unspecified.");

       // save translation component
       R2[ 3] = skeleton.tree[index].joint.p_rel[0];
       R2[ 7] = skeleton.tree[index].joint.p_rel[1];
       R2[11] = skeleton.tree[index].joint.p_rel[2];
       R2[15] = 1.0f;

       // multiply transforms
       mats[index] = R1 * R2; // sometimes works with R2 * R1 !!!

       // save transformed point
       pnts[index].x = mats[index][ 3];
       pnts[index].y = mats[index][ 7];
       pnts[index].z = mats[index][11];
      }
   }
 else
    return error("Joint position format unspecified.");

 // save skelegon
 if(parent != AMC_INVALID_JOINT) {
    lwSkelegon bone;
    bone.a = parent;
    bone.b = index;
    bone.name += skeleton.tree[parent].joint.name;
    bone.name += "_";
    bone.name += skeleton.tree[index].joint.name;
    pols[curr_bone++] = bone;
   }

 return true;
}

#pragma endregion

#pragma region LIGHTWAVE_SUPPORT_MODELS

template<class T>
void TraverseSkeleton(const AMC_SKELETON& skel, T& func, uint32 index)
{
 if(!(index < skel.tree.size())) return;
 const AMC_JOINTNODE& node = skel.tree[index];
 if(!func(skel, index)) return;
 for(size_t i = 0; i < node.children.size(); i++) TraverseSkeleton(skel, func, node.children[i]);
}

template<class T>
void TraverseSkeleton(const AMC_SKELETON& skel, T& func)
{
 TraverseSkeleton(skel, func, 0);
}

template<class T>
bool SaveTriangleList(const AMC_IDXBUFFER& ib, const AMC_REFERENCE& rb, std::deque<lwTriangle>& facedata)
{
 // validate
 if(ib.elem < 3) return error("Index buffer is not a triangle list.");
 if(rb.ib_width % 3) return error("Index buffer is not a triangle list.");
 if(ib.type != AMC_TRIANGLES) return error("Index buffer is not a triangle list.");

 // pointer to data
 const T* data = reinterpret_cast<const T*>(ib.data.get());
 if(!data) return error("Invalid data pointer.");

 // transfer data
 uint32 n_triangles = rb.ib_width/3;
 uint32 index = 0;
 for(uint32 i = 0; i < n_triangles; i++) {
     lwTriangle temp;
     temp.a = rb.vb_start + data[rb.ib_start + index++];
     temp.b = rb.vb_start + data[rb.ib_start + index++];
     temp.c = rb.vb_start + data[rb.ib_start + index++];
     facedata.push_back(temp);
    }

 return true;
}

template<class T>
bool SaveTriangleStrip(const AMC_IDXBUFFER& ib, const AMC_REFERENCE& rb, std::deque<lwTriangle>& facedata)
{
 // validate
 if(ib.elem < 3) return error("Index buffer is not a triangle strip.");
 if(ib.type != AMC_TRISTRIP) return error("Index buffer is not a triangle strip.");

 // pointer to data
 const T* data = reinterpret_cast<const T*>(ib.data.get());
 if(!data) return error("Invalid data pointer.");

 // first triangle
 uint32 a = rb.vb_start + data[rb.ib_start + 0];
 uint32 b = rb.vb_start + data[rb.ib_start + 1];
 uint32 c = rb.vb_start + data[rb.ib_start + 2];

 // save first triangle
 if(!(a == b || a == c || b == c)) {
    lwTriangle temp;
    temp.a = a;
    temp.b = b;
    temp.c = c;
    facedata.push_back(temp);
   }

 // other triangles
 for(size_t i = 3; i < rb.ib_width; i++)
    {
     // next triangle
     a = b;
     b = c;
     c = rb.vb_start + data[rb.ib_start + i];

     if(c < rb.vb_start) {
        std::stringstream ss;
        ss << "0x" << std::hex << c << std::dec << " is less than min of 0x" << std::hex << rb.vb_start << std::dec;
        std::cout << ss.str() << std::endl;
       }
     if(c > rb.vb_start + rb.vb_width) {
        std::stringstream ss;
        ss << "0x" << std::hex << c << std::dec << " is greater than max of 0x" << std::hex << (rb.vb_start + rb.vb_width) << std::dec;
        std::cout << ss.str() << std::endl;
       }

     // save triangle
     if(!(a == b || a == c || b == c)) {
        if(i % 2) {
           lwTriangle temp;
           temp.a = a;
           temp.b = c;
           temp.c = b;
           facedata.push_back(temp);
          }
        else {
           lwTriangle temp;
           temp.a = a;
           temp.b = b;
           temp.c = c;
           facedata.push_back(temp);
          }
       }
    }

 return true;
}

bool SaveLWO(const char* path, const char* name, const ADVANCEDMODELCONTAINER& data)
{
 // validate
 using namespace std;
 if(!path || !strlen(path)) return error("SaveLWO: A path must be specified.");
 if(!name || !strlen(name)) return error("SaveLWO: A name must be specified.");

 // save LAYR and CLIP
 lwFileSaver saver;
 saver.insertLayr(0, nullptr);
 for(size_t i = 0; i < data.iflist.size(); i++) saver.insertClip((i + 1), data.iflist[i].filename.c_str());

 // save PNTS and POLS
 for(size_t pointset = 0; pointset < data.vblist.size(); pointset++)
    {
     // save PNTS
     const AMC_VTXBUFFER& vbuffer = data.vblist[pointset];
     if(vbuffer.flags & AMC_VERTEX_POSITION)
       {
        // create point data
        uint32 n_points = vbuffer.elem;
        boost::shared_array<lwVertex3D> points(new lwVertex3D[n_points]);

        // transfer point data
        for(uint32 i = 0; i < n_points; i++) {
            points[i].x = vbuffer.data[i].vx;
            points[i].y = vbuffer.data[i].vy;
            points[i].z = vbuffer.data[i].vz;
           }

        // save point data
        saver.insertPnts(0, pointset, points, n_points);
       }

     // save WGHT
     if(vbuffer.flags & AMC_VERTEX_WEIGHTS)
       {
        typedef map<uint32, deque<lwRefVertex1D>> wmap_type;
        typedef map<uint32, deque<lwRefVertex1D>>::value_type wmap_pair;
        wmap_type wmap;

        // construct WGHT maps
        for(uint32 i = 0; i < vbuffer.elem; i++) {
            const AMC_VERTEX& vertex = vbuffer.data[i];
            for(uint32 j = 0; j < 8; j++) {
                real32 weight_value = vertex.wv[j];
                uint16 weight_index = vertex.wi[j];
                if(weight_index != AMC_INVALID_VERTEX_WMAP_INDEX) {
                   auto iter = wmap.find(weight_index);
                   if(iter == wmap.end()) iter = wmap.insert(wmap_pair(weight_index, deque<lwRefVertex1D>())).first;
                   lwRefVertex1D item;
                   item.reference = i;
                   item.x = weight_value;
                   iter->second.push_back(item);
                  }
               }
           }
 
        // save WGHT maps
        for(auto iter = wmap.begin(); iter != wmap.end(); iter++) {
            if(iter->second.size()) {
               boost::shared_array<lwRefVertex1D> wset(new lwRefVertex1D[iter->second.size()]);
               uint32 index = 0;
               for(size_t i = 0; i < iter->second.size(); i++) {
                   wset[index].reference = iter->second[i].reference;
                   wset[index].x = iter->second[i].x;
                   index++;
                  }
               stringstream ss;
               ss << "wmap_" << setfill('0') << setw(3) << iter->first;
               saver.insertWght(0, pointset, ss.str().c_str(), wset, iter->second.size());
              }
           }
       }
    }

 // save POLS
 for(size_t surface_index = 0; surface_index < data.surfaces.size(); surface_index++)
    {
     // get surface
     const AMC_SURFACE& surface = data.surfaces[surface_index];
     if(!surface.refs.size()) break;

     // for each polygon set
     for(size_t i = 0; i < surface.refs.size(); i++)
        {
         // get polygon set
         const AMC_REFERENCE& polyset = surface.refs[i];
         if(!(polyset.vb_index < data.vblist.size())) {
            stringstream ss;
            ss << "Invalid AMC_VTXBUFFER reference (" << polyset.vb_index << "). There are " << data.vblist.size() << " vertex buffers.";
            return error(ss.str().c_str());
           }
         if(!(polyset.ib_index < data.iblist.size())) {
            stringstream ss;
            ss << "Invalid AMC_IDXBUFFER reference (" << polyset.ib_index << "). There are " << data.iblist.size() << " index buffers.";
            return error(ss.str().c_str());
           }

         // get vertex and index buffer
         const AMC_VTXBUFFER& vbuffer = data.vblist[polyset.vb_index];
         const AMC_IDXBUFFER& ibuffer = data.iblist[polyset.ib_index];

         // save POLS
         deque<lwTriangle> facedata;
         if(ibuffer.type == AMC_TRIANGLES)
           {
            // convert geometry data
            if(ibuffer.format == AMC_UINT08) SaveTriangleList<uint08>(ibuffer, polyset, facedata);
            else if(ibuffer.format == AMC_UINT16) SaveTriangleList<uint16>(ibuffer, polyset, facedata);
            else if(ibuffer.format == AMC_UINT32) SaveTriangleList<uint32>(ibuffer, polyset, facedata);
       
            // save geometry data
            boost::shared_array<lwTriangle> temp(new lwTriangle[facedata.size()]);
            for(size_t j = 0; j < facedata.size(); j++) temp[j] = facedata[j];
            saver.insertPols(0, polyset.vb_index, surface.name.c_str(), temp, facedata.size());
           }
         else if(ibuffer.type == AMC_TRISTRIP)
           {
            // convert geometry data
            if(ibuffer.format == AMC_UINT08) SaveTriangleStrip<uint08>(ibuffer, polyset, facedata);
            else if(ibuffer.format == AMC_UINT16) SaveTriangleStrip<uint16>(ibuffer, polyset, facedata);
            else if(ibuffer.format == AMC_UINT32) SaveTriangleStrip<uint32>(ibuffer, polyset, facedata);
       
            // save geometry data
            boost::shared_array<lwTriangle> temp(new lwTriangle[facedata.size()]);
            for(size_t j = 0; j < facedata.size(); j++) temp[j] = facedata[j];
            saver.insertPols(0, polyset.vb_index, surface.name.c_str(), temp, facedata.size());
           }

         // save TXUV
         if(vbuffer.flags & AMC_VERTEX_UV)
           {
            // construct TXUV map
            map<uint32, lwVertex2D> uvmap;
            for(uint32 j = 0; j < facedata.size(); j++)
               {
                // convert points
                uint32 ref_a = facedata[j].a;
                uint32 ref_b = facedata[j].b;
                uint32 ref_c = facedata[j].c;
                lwVertex2D pnt_a = { vbuffer.data[ref_a].tu, vbuffer.data[ref_a].tv };
                lwVertex2D pnt_b = { vbuffer.data[ref_b].tu, vbuffer.data[ref_b].tv };
                lwVertex2D pnt_c = { vbuffer.data[ref_c].tu, vbuffer.data[ref_c].tv };
                // save points
                uvmap.insert(map<uint32, lwVertex2D>::value_type(ref_a, pnt_a));
                uvmap.insert(map<uint32, lwVertex2D>::value_type(ref_b, pnt_b));
                uvmap.insert(map<uint32, lwVertex2D>::value_type(ref_c, pnt_c));
               }

            // save TXUV map
            boost::shared_array<lwRefVertex2D> uvset(new lwRefVertex2D[uvmap.size()]);
            uint32 index = 0;
            for(auto iter = uvmap.begin(); iter != uvmap.end(); iter++) {
                uvset[index].reference = iter->first;
                uvset[index].x = iter->second.x;
                uvset[index].y = iter->second.y;
                index++;
               }
            saver.insertTxuv(0, polyset.vb_index, surface.name.c_str(), uvset, uvmap.size());

            // associate surface with TXUV map of same name
            saver.enableSurfColrImag(surface.name.c_str());
            saver.setSurfColrVmap(surface.name.c_str(), surface.name.c_str());

            // set material
            uint16 surfmat_index = surface.surfmat;
            if(surfmat_index != AMC_INVALID_SURFMAT) {
               const AMC_SURFMAT& material = data.surfmats[surfmat_index];
               if(material.basemap != AMC_INVALID_TEXTURE) {
                  saver.setSurfColrImag(surface.name.c_str(), material.basemap + 1);
                 }
               if(material.specmap != AMC_INVALID_TEXTURE) {
                  saver.enableSurfSpecImag(surface.name.c_str());
                  saver.setSurfSpecVmap(surface.name.c_str(), surface.name.c_str());
                  saver.setSurfSpecImag(surface.name.c_str(), material.specmap + 1);
                 }
               if(material.tranmap != AMC_INVALID_TEXTURE) {
                  saver.enableSurfTranImag(surface.name.c_str());
                  saver.setSurfTranVmap(surface.name.c_str(), surface.name.c_str());
                  saver.setSurfTranImag(surface.name.c_str(), material.tranmap + 1);
                  saver.setSurfTranNega(surface.name.c_str(), 1);
                 }
               if(material.bumpmap != AMC_INVALID_TEXTURE) {
                  //saver.enableSurfBumpImag(surface.name.c_str());
                  //saver.setSurfBumpVmap(surface.name.c_str(), surface.name.c_str());
                  //saver.setSurfBumpImag(surface.name.c_str(), material.bumpmap + 1);
                 }
               if(material.normmap != AMC_INVALID_TEXTURE) {
                  // TODO
                 }
              }
           }

         // save NORM
         if(vbuffer.flags & AMC_VERTEX_NORMAL)
           {
            // set surface normal vertex map
            saver.setSurfNorm(surface.name.c_str(), surface.name.c_str());

            // construct NORM map
            map<uint32, lwVertex3D> nmap;
            for(uint32 j = 0; j < facedata.size(); j++)
               {
                // convert points
                uint32 ref_a = facedata[j].a;
                uint32 ref_b = facedata[j].b;
                uint32 ref_c = facedata[j].c;
                lwVertex3D pnt_a = { vbuffer.data[ref_a].nx, vbuffer.data[ref_a].ny, vbuffer.data[ref_a].nz };
                lwVertex3D pnt_b = { vbuffer.data[ref_b].nx, vbuffer.data[ref_b].ny, vbuffer.data[ref_b].nz };
                lwVertex3D pnt_c = { vbuffer.data[ref_c].nx, vbuffer.data[ref_c].ny, vbuffer.data[ref_c].nz };
                // save points
                nmap.insert(map<uint32, lwVertex3D>::value_type(ref_a, pnt_a));
                nmap.insert(map<uint32, lwVertex3D>::value_type(ref_b, pnt_b));
                nmap.insert(map<uint32, lwVertex3D>::value_type(ref_c, pnt_c));
               }

            // save NORM map
            boost::shared_array<lwRefVertex3D> nset(new lwRefVertex3D[nmap.size()]);
            uint32 index = 0;
            for(map<uint32, lwVertex3D>::iterator iter = nmap.begin(); iter != nmap.end(); iter++) {
                nset[index].reference = iter->first;
                nset[index].x = iter->second.x;
                nset[index].y = iter->second.y;
                nset[index].z = iter->second.z;
                index++;
               }
            saver.insertNorm(0, polyset.vb_index, surface.name.c_str(), nset, nmap.size());
           }
        }
    }

 // save SKEL
 for(size_t i = 0; i < data.skllist.size(); i++) {
     AMCJointVisitor jv(data.skllist[i]);
     TraverseSkeleton(data.skllist[i], jv);
     saver.insertJnts(0, i, jv.pnts, jv.n_pnts);
     saver.insertSkel(0, 0, data.skllist[i].name.c_str(), jv.pols, jv.n_pols);
    }

 // save file
 std::stringstream ss;
 ss << path << name << ".lwo";
 return saver.save(ss.str().c_str());
}

#pragma endregion

//
// MODEL SUPPORT
//

#pragma region MODEL_SUPPORT

inline std::map<uint32, uint32> CreateZeroIndexedMap(const uint16* data, uint32 elem, uint32 start)
{
 typedef std::set<uint16> set_type;
 typedef std::map<uint32, uint32> map_type;
 typedef std::map<uint32, uint32>::value_type value_type;

 // mark used vertex indices
 std::set<uint16> used;
 for(uint32 i = 0; i < elem; i++)
     if(data[start + i] != 0xFFFF) used.insert(data[start + i]);

 // map old vertex index to new vertex index
 uint32 curr_index = 0;
 map_type retval;
 for(set_type::iterator iter = used.begin(); iter != used.end(); iter++)
     retval.insert(value_type(*iter, curr_index++));

 return retval;
}

//
// TransformAMC
// The problem with Blender is that vertex buffers cannot be shared between mesh
// objects. This is a problem because vertex buffers in games are usually shared
// between meshes. This function transforms an existing model container so that
// it does not share vertex buffers.
//
ADVANCEDMODELCONTAINER TransformAMC(const ADVANCEDMODELCONTAINER& data)
{
 // return value
 using namespace std;
 ADVANCEDMODELCONTAINER amc;

 //
 // STEP #1
 // Go through each surface, and map old indices to new ones. For example, if we
 // have a vertex buffer with 100 vertices, and a single triangle in the surface
 // where ibuffer = { 8, 7, 9, 4, 3, 2, 3, 5, 6 }.
 //
 // vb_index =   0; // vbuffer index
 // vb_start =  10; // start index of vbuffer
 // vb_width = 100; // number of vertices
 // ib_index =   0; // ibuffer index
 // ib_start =   6; // start index of ibuffer
 // ib_width =   3; // number of indices
 //
 // So which vertices are used given an index?
 // vb_start + ibuffer[ib_start + index]
 // 10 + ibuffer[6 + 0] = 10 + 3 = 13 (these are vertex indices)
 // 10 + ibuffer[6 + 1] = 10 + 5 = 15 (these are vertex indices)
 // 10 + ibuffer[6 + 2] = 10 + 6 = 16 (these are vertex indices)
 //
 // so now we create a map that maps new and old vertex indices
 // 3 -> 0
 // 5 -> 1
 // 6 -> 2

 // for each surface
 for(uint32 surface_index = 0; surface_index < data.surfaces.size(); surface_index++)
    {
     // TODO: WORK ON THESE
     const AMC_SURFACE& surface = data.surfaces[surface_index];
     const std::deque<AMC_REFERENCE>& reflist = data.surfaces[surface_index].refs;

     // for each surface reference
     for(uint32 ref_index = 0; ref_index < data.surfaces[surface_index].refs.size(); ref_index++)
        {
         // create an index map to map used indices
         std::map<uint32, uint32> imap;
         const AMC_REFERENCE& reference = reflist[ref_index];
         const AMC_VTXBUFFER& vbuffer = data.vblist[reference.vb_index];
         const AMC_IDXBUFFER& ibuffer = data.iblist[reference.ib_index];
         uint32 curr_index = 0;

         // remap old indices to new indices
         if(ibuffer.format == AMC_UINT08) {
            const uint08* ibptr = reinterpret_cast<const uint08*>(ibuffer.data.get());
            for(uint32 i = 0; i < reference.ib_width; i++) {
                uint08 local_index = ibptr[reference.ib_start + i];
                if(ibuffer.type == AMC_TRISTRIPCUT && local_index == 0xFF) continue; // ignore strip cut index
                auto iter = imap.find(local_index);
                if(iter == imap.end()) {
                   imap.insert(std::map<uint32, uint32>::value_type(local_index, curr_index));
                   curr_index++;
                  }
               }
           }
         else if(ibuffer.format == AMC_UINT16) {
            const uint16* ibptr = reinterpret_cast<const uint16*>(ibuffer.data.get());
            imap = CreateZeroIndexedMap(ibptr, reference.ib_width, reference.ib_start);
           }
         else if(ibuffer.format == AMC_UINT32) {
            const uint32* ibptr = reinterpret_cast<const uint32*>(ibuffer.data.get());
            for(uint32 i = 0; i < reference.ib_width; i++) {
                uint32 local_index = ibptr[reference.ib_start + i];
                if(ibuffer.type == AMC_TRISTRIPCUT && local_index == 0xFFFFFFFF) continue; // ignore strip cut index
                auto iter = imap.find(local_index);
                if(iter == imap.end()) {
                   imap.insert(std::map<uint32, uint32>::value_type(local_index, curr_index));
                   curr_index++;
                  }
               }
           }

         // create a new vertex buffer
         AMC_VTXBUFFER vbnew;
         vbnew.flags = vbuffer.flags;
         vbnew.name = vbuffer.name;
         vbnew.uvchan = vbuffer.uvchan;
         for(uint32 i = 0; i < 8; i++) vbnew.uvtype[i] = vbuffer.uvtype[i];
         vbnew.colors = vbuffer.colors;
         vbnew.elem = imap.size();
         vbnew.data.reset(new AMC_VERTEX[vbnew.elem]);

         // create a new index buffer
         AMC_IDXBUFFER ibnew;
         ibnew.format = ibuffer.format;
         ibnew.type = ibuffer.type;
         ibnew.wind = ibuffer.wind;
         ibnew.reserved = ibuffer.reserved;
         ibnew.name = ibuffer.name;
         ibnew.elem = reference.ib_width;
         uint32 size = 0;
         if(ibnew.format == AMC_UINT08) size = ibnew.elem;
         else if(ibnew.format == AMC_UINT16) size = ibnew.elem * sizeof(uint16);
         else if(ibnew.format == AMC_UINT32) size = ibnew.elem * sizeof(uint32);
         ibnew.data.reset(new char[size]);

         // create a new surface
         AMC_SURFACE surfnew;
         surfnew.name = surface.name;
         surfnew.surfmat = surface.surfmat;
         AMC_REFERENCE refnew;
         refnew.vb_index = amc.vblist.size();
         refnew.vb_start = 0;
         refnew.vb_width = vbnew.elem;
         refnew.ib_index = amc.iblist.size();
         refnew.ib_start = 0;
         refnew.ib_width = ibnew.elem;
         refnew.jm_index = reference.jm_index;
         surfnew.refs.push_back(refnew);

         // transform vertex buffer
         for(auto iter = imap.begin(); iter != imap.end(); iter++)
            {
             // get indices
             uint32 old_index = reference.vb_start + iter->first;
             if(!(old_index < vbuffer.elem)) cout << "ERROR! OLD INDEX " << old_index << " OUT OF BOUNDS!" << endl;

             uint32 new_index = iter->second;
             if(!(new_index < vbnew.elem)) cout << "ERROR! NEW INDEX " << new_index << " OUT OF BOUNDS!" << endl;

             // copy position
             vbnew.data[new_index].vx = vbuffer.data[old_index].vx;
             vbnew.data[new_index].vy = vbuffer.data[old_index].vy;
             vbnew.data[new_index].vz = vbuffer.data[old_index].vz;

             // copy normal
             vbnew.data[new_index].nx = vbuffer.data[old_index].nx;
             vbnew.data[new_index].ny = vbuffer.data[old_index].ny;
             vbnew.data[new_index].nz = vbuffer.data[old_index].nz;

             // copy tangent
             vbnew.data[new_index].tx = vbuffer.data[old_index].tx;
             vbnew.data[new_index].ty = vbuffer.data[old_index].ty;
             vbnew.data[new_index].tz = vbuffer.data[old_index].tz;

             // copy binormal
             vbnew.data[new_index].bx = vbuffer.data[old_index].bx;
             vbnew.data[new_index].by = vbuffer.data[old_index].by;
             vbnew.data[new_index].bz = vbuffer.data[old_index].bz;

             // copy blend weights
             vbnew.data[new_index].wv[0] = vbuffer.data[old_index].wv[0];
             vbnew.data[new_index].wv[1] = vbuffer.data[old_index].wv[1];
             vbnew.data[new_index].wv[2] = vbuffer.data[old_index].wv[2];
             vbnew.data[new_index].wv[3] = vbuffer.data[old_index].wv[3];
             vbnew.data[new_index].wv[4] = vbuffer.data[old_index].wv[4];
             vbnew.data[new_index].wv[5] = vbuffer.data[old_index].wv[5];
             vbnew.data[new_index].wv[6] = vbuffer.data[old_index].wv[6];
             vbnew.data[new_index].wv[7] = vbuffer.data[old_index].wv[7];

             // copy blend indices
             vbnew.data[new_index].wi[0] = vbuffer.data[old_index].wi[0];
             vbnew.data[new_index].wi[1] = vbuffer.data[old_index].wi[1];
             vbnew.data[new_index].wi[2] = vbuffer.data[old_index].wi[2];
             vbnew.data[new_index].wi[3] = vbuffer.data[old_index].wi[3];
             vbnew.data[new_index].wi[4] = vbuffer.data[old_index].wi[4];
             vbnew.data[new_index].wi[5] = vbuffer.data[old_index].wi[5];
             vbnew.data[new_index].wi[6] = vbuffer.data[old_index].wi[6];
             vbnew.data[new_index].wi[7] = vbuffer.data[old_index].wi[7];

             // copy texture coordinates
             for(uint32 i = 0; i < 8; i++) {
                 vbnew.data[new_index].uv[i][0] = vbuffer.data[old_index].uv[i][0];
                 vbnew.data[new_index].uv[i][1] = vbuffer.data[old_index].uv[i][1];
                }

             // copy vertex colors
             for(uint32 i = 0; i < 8; i++) {
                 vbnew.data[new_index].color[i][0] = vbuffer.data[old_index].color[i][0];
                 vbnew.data[new_index].color[i][1] = vbuffer.data[old_index].color[i][1];
                 vbnew.data[new_index].color[i][2] = vbuffer.data[old_index].color[i][2];
                 vbnew.data[new_index].color[i][3] = vbuffer.data[old_index].color[i][3];
                }
            }

         // transform index buffer
         if(ibuffer.format == AMC_UINT08) {
            const uint08* ib_src = reinterpret_cast<const uint08*>(ibuffer.data.get());
            uint08* ib_dst = reinterpret_cast<uint08*>(ibnew.data.get());
            for(uint32 i = 0; i < reference.ib_width; i++) {
                // copy strip-cut indices
                uint08 local_index = ib_src[reference.ib_start + i];
                if(ibuffer.type == AMC_TRISTRIPCUT && local_index == 0xFF) {
                   ib_dst[i] = local_index;
                   continue;
                  }
                // look up and set new index
                auto iter = imap.find(local_index);
                if(iter != imap.end()) ib_dst[i] = (uint08)iter->second;
               }
           }
         else if(ibuffer.format == AMC_UINT16) {
            const uint16* ib_src = reinterpret_cast<const uint16*>(ibuffer.data.get());
            uint16* ib_dst = reinterpret_cast<uint16*>(ibnew.data.get());
            for(uint32 i = 0; i < reference.ib_width; i++) {
                // copy strip-cut indices
                uint16 local_index = ib_src[reference.ib_start + i];
                if(ibuffer.type == AMC_TRISTRIPCUT) {
                   if(local_index == 0xFFFF) {
                      ib_dst[i] = 0xFFFF;
                      continue;
                     }
                  }
                // look up and set new index
                auto iter = imap.find(local_index);
                if(iter != imap.end()) ib_dst[i] = (uint16)iter->second;
                else throw std::exception("Local index not found."); // crashes if n_vb = n_ib = 1
               }
           }
         else if(ibuffer.format == AMC_UINT32) {
            const uint32* ib_src = reinterpret_cast<const uint32*>(ibuffer.data.get());
            uint32* ib_dst = reinterpret_cast<uint32*>(ibnew.data.get());
            for(uint32 i = 0; i < reference.ib_width; i++) {
                // copy strip-cut indices
                uint32 local_index = ib_src[reference.ib_start + i];
                if(ibuffer.type == AMC_TRISTRIPCUT && local_index == 0xFFFFFFFF) {
                   ib_dst[i] = local_index;
                   continue;
                  }
                // look up and set new index
                auto iter = imap.find(local_index);
                if(iter != imap.end()) ib_dst[i] = (uint32)iter->second;
               }
           }

         // save new data
         amc.vblist.push_back(vbnew);
         amc.iblist.push_back(ibnew);
         amc.surfaces.push_back(surfnew);
        }
    }

 // copy data that doesn't change
 amc.iflist = data.iflist;
 amc.surfmats = data.surfmats;
 amc.skllist2 = data.skllist2;
 amc.jmaplist = data.jmaplist;
 return amc;
}

bool SaveAMC(LPCTSTR path, LPCTSTR name, const ADVANCEDMODELCONTAINER& data)
{
 // validate
 using namespace std;
 if(!path) return error("Invalid pathname.");
 if(!name) return error("Invalid filename.");

 //
 // VALIDATION
 //

 // validate skeleton
 if(data.skllist2.size() > 1)
    return error("The number of skeletons must be either 0 or 1.");

 // validate skeleton
 for(uint32 i = 0; i < data.skllist2.size(); i++)
    {
     // validate skeleton format (coordinate system)
     uint16 format = data.skllist2[i].format;
     if(format & AMC_JOINT_FORMAT_ABSOLUTE)
        if(format & AMC_JOINT_FORMAT_RELATIVE)
           return error("Absolute and relative skeletons are mutually exclusive flags.");
     else if(format & AMC_JOINT_FORMAT_RELATIVE)
        if(format & AMC_JOINT_FORMAT_ABSOLUTE)
           return error("Absolute and relative skeletons are mutually exclusive flags.");
     else
        return error("Skeletons must be absolute or relative.");

     // validate skeleton format (rotation transform)
     if(format & AMC_JOINT_FORMAT_QUATERNION) ;
     else if(format & AMC_JOINT_FORMAT_MATRIX) ;
     else return error("Skeletons must define the type of rotation transform used.");

     // validate skeleton format (bone axis)
     if(format & AMC_JOINT_FORMAT_X_BONEAXIS) ;
     else if(format & AMC_JOINT_FORMAT_Y_BONEAXIS) ;
     else if(format & AMC_JOINT_FORMAT_Z_BONEAXIS) ;
     else return error("Skeletons must define a bone axis.");
    }

 // create file
 STDSTRINGSTREAM ss;
 ss << path << name << TEXT(".smc");
 ofstream ofile(ss.str().c_str(), ios::binary);
 if(!ofile) return error("Failed to create output file.");

 //
 // HEADER
 //

 // save magic
 ofile.write("SMC ", 4);
 if(ofile.fail()) return error("Write failure.");

 // save version
 uint16 major = 1;
 uint16 minor = 0;
 ofile.write((char*)&major, sizeof(major));
 ofile.write((char*)&minor, sizeof(minor));
 if(ofile.fail()) return error("Write failure.");

 // save number buffers
 uint32 nvb = data.vblist.size();
 uint32 nib = data.iblist.size();
 ofile.write((char*)&nvb, sizeof(nvb));
 ofile.write((char*)&nib, sizeof(nib));
 if(ofile.fail()) return error("Write failure.");

 // save number of images, surfaces, materials, skeletons, and joint maps
 uint32 nim = data.iflist.size();
 uint32 nsf = data.surfaces.size();
 uint32 nsm = data.surfmats.size();
 uint32 nsk = data.skllist2.size();
 uint32 njm = data.jmaplist.size();
 ofile.write((char*)&nim, sizeof(nim));
 ofile.write((char*)&nsf, sizeof(nsf));
 ofile.write((char*)&nsm, sizeof(nsm));
 ofile.write((char*)&nsk, sizeof(nsk));
 ofile.write((char*)&njm, sizeof(njm));
 if(ofile.fail()) return error("Write failure.");

 // save reserved area
 uint32 reserved[7] = { 0, 0, 0, 0, 0, 0, 0 };
 for(uint32 i = 0; i < 7; i++) ofile.write((char*)&reserved[i], sizeof(reserved[i]));
 if(ofile.fail()) return error("Write failure.");

 // save image list
 for(uint32 i = 0; i < data.iflist.size(); i++)
    {
     // non-padded length
     uint32 len = data.iflist[i].filename.length() + 1;
     if(!len) return error("Invalid filename length.");

     // construct padded filename
     uint32 padlen = ((len + 0x3) & (~0x3));
     boost::shared_array<char> padstr(new char[padlen]);
     for(uint32 j = 0; j < len; j++) padstr[j] = data.iflist[i].filename[j];
     for(uint32 j = len; j < padlen; j++) padstr[j] = '\0';

     // save length and filename
     ofile.write((char*)&padlen, sizeof(padlen));
     ofile.write(padstr.get(), padlen);
     if(ofile.fail()) return error("Write failure.");
    }

 // save vertex buffers
 for(uint32 i = 0; i < data.vblist.size(); i++)
    {
     // save flags
     const AMC_VTXBUFFER& vb = data.vblist[i];
     ofile.write((char*)&vb.flags, sizeof(vb.flags));
     if(ofile.fail()) return error("Write failure.");

     // save name
     uint32 len = vb.name.length();
     if(len) {
        uint32 padlen = ((len + 0x3) & (~0x3));
        boost::shared_array<char> padstr(new char[padlen]);
        for(uint32 j = 0; j < len; j++) padstr[j] = vb.name[j];
        for(uint32 j = len; j < padlen; j++) padstr[j] = '\0';
        ofile.write((char*)&padlen, sizeof(padlen));
        ofile.write(padstr.get(), padlen);
        if(ofile.fail()) return error("Write failure.");
       }
     else {
        ofile.write((char*)&len, sizeof(len));
        if(ofile.fail()) return error("Write failure.");
       }

     // validate number of texture channels
     if(vb.uvchan > 8) return error("Invalid number of UV channels.");

     // save number of texture channels
     ofile.write((char*)&vb.uvchan, sizeof(vb.uvchan));
     if(ofile.fail()) return error("Write failure.");
        
     // save texture channel information
     ofile.write((char*)&vb.uvtype[0], sizeof(vb.uvtype[0]));
     ofile.write((char*)&vb.uvtype[1], sizeof(vb.uvtype[1]));
     ofile.write((char*)&vb.uvtype[2], sizeof(vb.uvtype[2]));
     ofile.write((char*)&vb.uvtype[3], sizeof(vb.uvtype[3]));
     ofile.write((char*)&vb.uvtype[4], sizeof(vb.uvtype[4]));
     ofile.write((char*)&vb.uvtype[5], sizeof(vb.uvtype[5]));
     ofile.write((char*)&vb.uvtype[6], sizeof(vb.uvtype[6]));
     ofile.write((char*)&vb.uvtype[7], sizeof(vb.uvtype[7]));
     if(ofile.fail()) return error("Write failure.");

     // save number of color channels
     ofile.write((char*)&vb.colors, sizeof(vb.colors));
     if(ofile.fail()) return error("Write failure.");

     // save number of vertices
     ofile.write((char*)&vb.elem, sizeof(vb.elem));
     if(ofile.fail()) return error("Write failure.");

     // save vertices
     for(uint32 j = 0; j < vb.elem; j++)
        {
         // position
         if(vb.flags & AMC_VERTEX_POSITION) {
            ofile.write((char*)&vb.data[j].vx, sizeof(vb.data[j].vx));
            ofile.write((char*)&vb.data[j].vy, sizeof(vb.data[j].vy));
            ofile.write((char*)&vb.data[j].vz, sizeof(vb.data[j].vz));
            if(ofile.fail()) return error("Write failure.");
           }

         // normal
         if(vb.flags & AMC_VERTEX_NORMAL) {
            ofile.write((char*)&vb.data[j].nx, sizeof(vb.data[j].nx));
            ofile.write((char*)&vb.data[j].ny, sizeof(vb.data[j].ny));
            ofile.write((char*)&vb.data[j].nz, sizeof(vb.data[j].nz));
            if(ofile.fail()) return error("Write failure.");
           }

         // tangent
         if(vb.flags & AMC_VERTEX_TANGENT) {
            ofile.write((char*)&vb.data[j].tx, sizeof(vb.data[j].tx));
            ofile.write((char*)&vb.data[j].ty, sizeof(vb.data[j].ty));
            ofile.write((char*)&vb.data[j].tz, sizeof(vb.data[j].tz));
            if(ofile.fail()) return error("Write failure.");
           }

         // binormal
         if(vb.flags & AMC_VERTEX_BINORMAL) {
            ofile.write((char*)&vb.data[j].bx, sizeof(vb.data[j].bx));
            ofile.write((char*)&vb.data[j].by, sizeof(vb.data[j].by));
            ofile.write((char*)&vb.data[j].bz, sizeof(vb.data[j].bz));
            if(ofile.fail()) return error("Write failure.");
           }

         // blend weights
         if(vb.flags & AMC_VERTEX_WEIGHTS) {
            for(uint32 k = 0; k < 8; k++) {
                ofile.write((char*)&vb.data[j].wv[k], sizeof(vb.data[j].wv[k]));
                if(ofile.fail()) return error("Write failure.");
               }
            for(uint32 k = 0; k < 8; k++) {
                ofile.write((char*)&vb.data[j].wi[k], sizeof(vb.data[j].wi[k]));
                if(ofile.fail()) return error("Write failure.");
               }
           }

         // texture coordinates
         if(vb.flags & AMC_VERTEX_UV) {
            for(uint32 k = 0; k < vb.uvchan; k++) {
                ofile.write((char*)&vb.data[j].uv[k][0], sizeof(vb.data[j].uv[k][0]));
                ofile.write((char*)&vb.data[j].uv[k][1], sizeof(vb.data[j].uv[k][1]));
                if(ofile.fail()) return error("Write failure.");
               }
           }

         // colors
         for(uint32 k = 0; k < vb.colors; k++) {
             ofile.write((char*)&vb.data[j].color[k][0], sizeof(vb.data[j].color[k][0]));
             ofile.write((char*)&vb.data[j].color[k][1], sizeof(vb.data[j].color[k][1]));
             ofile.write((char*)&vb.data[j].color[k][2], sizeof(vb.data[j].color[k][2]));
             ofile.write((char*)&vb.data[j].color[k][3], sizeof(vb.data[j].color[k][3]));
             if(ofile.fail()) return error("Write failure.");
            }
        }
    }

 // save index buffers
 for(uint32 i = 0; i < data.iblist.size(); i++)
    {
     // save data format
     auto ib = data.iblist[i];
     ofile.write((char*)&ib.format, sizeof(ib.format));
     if(ofile.fail()) return error("Write failure.");

     // save primitive type
     ofile.write((char*)&ib.type, sizeof(ib.type));
     if(ofile.fail()) return error("Write failure.");

     // save winding order
     ofile.write((char*)&ib.wind, sizeof(ib.wind));
     if(ofile.fail()) return error("Write failure.");

     // save reserved
     ofile.write((char*)&ib.reserved, sizeof(ib.reserved));
     if(ofile.fail()) return error("Write failure.");

     // save name
     uint32 len = ib.name.length();
     if(len) {
        uint32 padlen = ((len + 0x3) & (~0x3));
        boost::shared_array<char> padstr(new char[padlen]);
        for(uint32 j = 0; j < len; j++) padstr[j] = ib.name[j];
        for(uint32 j = len; j < padlen; j++) padstr[j] = '\0';
        ofile.write((char*)&padlen, sizeof(padlen));
        ofile.write(padstr.get(), padlen);
        if(ofile.fail()) return error("Write failure.");
       }
     else {
        ofile.write((char*)&len, sizeof(len));
        if(ofile.fail()) return error("Write failure.");
       }

     // save number of indices
     ofile.write((char*)&ib.elem, sizeof(ib.elem));
     if(ofile.fail()) return error("Write failure.");

     // save indices
     if(ib.format == AMC_UINT08) {
        ofile.write((char*)ib.data.get(), sizeof(uint08)*ib.elem);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(ib.format == AMC_UINT16) {
        ofile.write((char*)ib.data.get(), sizeof(uint16)*ib.elem);
        if(ofile.fail()) return error("Write failure.");
       }
     else if(ib.format == AMC_UINT32) {
        ofile.write((char*)ib.data.get(), sizeof(uint32)*ib.elem);
        if(ofile.fail()) return error("Write failure.");
       }
     else return error("Invalid index buffer format.");

     // save extra byte for odd number of AMC_UINT08 indices
     if(ib.format == AMC_UINT08 && (ib.elem % 2)) {
        uint08 zero = 0;
        ofile.write((char*)&zero, sizeof(zero));
        if(ofile.fail()) return error("Write failure.");
       }
    }

 // save materials
 for(uint32 i = 0; i < data.surfmats.size(); i++)
    {
     // save name
     auto sm = data.surfmats[i];
     uint32 len = sm.name.length();
     if(len) {
        uint32 padlen = ((len + 0x3) & (~0x3));
        boost::shared_array<char> padstr(new char[padlen]);
        for(uint32 j = 0; j < len; j++) padstr[j] = sm.name[j];
        for(uint32 j = len; j < padlen; j++) padstr[j] = '\0';
        ofile.write((char*)&padlen, sizeof(padlen));
        ofile.write(padstr.get(), padlen);
        if(ofile.fail()) return error("Write failure.");
       }
     else {
        ofile.write((char*)&len, sizeof(len));
        if(ofile.fail()) return error("Write failure.");
       }

     // save two-side state
     ofile.write((char*)&sm.twoside, sizeof(sm.twoside));
     if(ofile.fail()) return error("Write failure.");

     // save reserved
     uint08 zero = 0;
     ofile.write((char*)&zero, sizeof(zero));
     ofile.write((char*)&zero, sizeof(zero));
     ofile.write((char*)&zero, sizeof(zero));

     // save texture references
     ofile.write((char*)&sm.basemap, sizeof(sm.basemap));
     ofile.write((char*)&sm.specmap, sizeof(sm.specmap));
     ofile.write((char*)&sm.tranmap, sizeof(sm.tranmap));
     ofile.write((char*)&sm.bumpmap, sizeof(sm.bumpmap));
     ofile.write((char*)&sm.normmap, sizeof(sm.normmap));
     ofile.write((char*)&sm.lgthmap, sizeof(sm.lgthmap));
     ofile.write((char*)&sm.envimap, sizeof(sm.envimap));
     ofile.write((char*)&sm.glssmap, sizeof(sm.glssmap));
     ofile.write((char*)&sm.resmap1, sizeof(sm.resmap1));
     ofile.write((char*)&sm.resmap2, sizeof(sm.resmap2));
     ofile.write((char*)&sm.resmap3, sizeof(sm.resmap3));
     ofile.write((char*)&sm.resmap4, sizeof(sm.resmap4));
     ofile.write((char*)&sm.resmap5, sizeof(sm.resmap5));
     ofile.write((char*)&sm.resmap6, sizeof(sm.resmap6));
     ofile.write((char*)&sm.resmap7, sizeof(sm.resmap7));
     ofile.write((char*)&sm.resmap8, sizeof(sm.resmap8));
     if(ofile.fail()) return error("Write failure.");

     // save texture channel references
     ofile.write((char*)&sm.basemapchan, sizeof(sm.basemapchan));
     ofile.write((char*)&sm.specmapchan, sizeof(sm.specmapchan));
     ofile.write((char*)&sm.tranmapchan, sizeof(sm.tranmapchan));
     ofile.write((char*)&sm.bumpmapchan, sizeof(sm.bumpmapchan));
     ofile.write((char*)&sm.normmapchan, sizeof(sm.normmapchan));
     ofile.write((char*)&sm.lghtmapchan, sizeof(sm.lghtmapchan));
     ofile.write((char*)&sm.envimapchan, sizeof(sm.envimapchan));
     ofile.write((char*)&sm.glssmapchan, sizeof(sm.glssmapchan));
     ofile.write((char*)&sm.resmapchan1, sizeof(sm.resmapchan1));
     ofile.write((char*)&sm.resmapchan2, sizeof(sm.resmapchan2));
     ofile.write((char*)&sm.resmapchan3, sizeof(sm.resmapchan3));
     ofile.write((char*)&sm.resmapchan4, sizeof(sm.resmapchan4));
     ofile.write((char*)&sm.resmapchan5, sizeof(sm.resmapchan5));
     ofile.write((char*)&sm.resmapchan6, sizeof(sm.resmapchan6));
     ofile.write((char*)&sm.resmapchan7, sizeof(sm.resmapchan7));
     ofile.write((char*)&sm.resmapchan8, sizeof(sm.resmapchan8));
     if(ofile.fail()) return error("Write failure.");
    }

 // save surfaces
 for(uint32 i = 0; i < data.surfaces.size(); i++)
    {
     // save name
     auto surface = data.surfaces[i];
     uint32 len = surface.name.length();
     if(len) {
        uint32 padlen = ((len + 0x3) & (~0x3));
        boost::shared_array<char> padstr(new char[padlen]);
        for(uint32 j = 0; j < len; j++) padstr[j] = surface.name[j];
        for(uint32 j = len; j < padlen; j++) padstr[j] = '\0';
        ofile.write((char*)&padlen, sizeof(padlen));
        ofile.write(padstr.get(), padlen);
        if(ofile.fail()) return error("Write failure.");
       }
     else {
        ofile.write((char*)&len, sizeof(len));
        if(ofile.fail()) return error("Write failure.");
       }

     // save number of references
     uint32 references = (uint32)surface.refs.size();
     ofile.write((char*)&references, sizeof(references));
     if(ofile.fail()) return error("Write failure.");

     // save buffer references
     for(uint32 j = 0; j < references; j++) {
        ofile.write((char*)&surface.refs[j].vb_index, sizeof(surface.refs[j].vb_index));
        ofile.write((char*)&surface.refs[j].vb_start, sizeof(surface.refs[j].vb_start));
        ofile.write((char*)&surface.refs[j].vb_width, sizeof(surface.refs[j].vb_width));
        ofile.write((char*)&surface.refs[j].ib_index, sizeof(surface.refs[j].ib_index));
        ofile.write((char*)&surface.refs[j].ib_start, sizeof(surface.refs[j].ib_start));
        ofile.write((char*)&surface.refs[j].ib_width, sizeof(surface.refs[j].ib_width));
        ofile.write((char*)&surface.refs[j].jm_index, sizeof(surface.refs[j].jm_index));
        if(ofile.fail()) return error("Write failure.");
        }

     // save material reference
     ofile.write((char*)&surface.surfmat, sizeof(surface.surfmat));
     if(ofile.fail()) return error("Write failure.");
    }

 // save skeletons
 for(uint32 i = 0; i < data.skllist2.size(); i++)
    {
     // save format
     const AMC_SKELETON2& skel = data.skllist2[i];
     ofile.write((char*)&skel.format, sizeof(skel.format));
     if(ofile.fail()) return error("Write failure.");

     // save tip length (if required)
     if(skel.format & AMC_JOINT_FORMAT_TIP_LENGTH) {
        ofile.write((char*)&skel.tiplen, sizeof(skel.tiplen));
        if(ofile.fail()) return error("Write failure.");
       }

     // save name
     uint32 len = skel.name.length();
     if(len) {
        uint32 padlen = ((len + 0x3) & (~0x3));
        boost::shared_array<char> padstr(new char[padlen]);
        for(uint32 j = 0; j < len; j++) padstr[j] = skel.name[j];
        for(uint32 j = len; j < padlen; j++) padstr[j] = '\0';
        ofile.write((char*)&padlen, sizeof(padlen));
        ofile.write(padstr.get(), padlen);
        if(ofile.fail()) return error("Write failure.");
       }
     // length of zero means use default name "skeleton"
     else {
        uint32 zero = 0;
        ofile.write((char*)&zero, sizeof(zero));
        if(ofile.fail()) return error("Write failure.");
       }

     // save number of joints
     uint32 n_joints = (uint32)skel.joints.size();
     ofile.write((char*)&n_joints, sizeof(n_joints));
     if(ofile.fail()) return error("Write failure.");

     // save joints
     for(uint32 j = 0; j < skel.joints.size(); j++)
        {
         // save name
         const AMC_JOINT& jnt = skel.joints[j];
         uint32 len = jnt.name.length();
         if(len) {
            uint32 padlen = ((len + 0x3) & (~0x3));
            boost::shared_array<char> padstr(new char[padlen]);
            for(uint32 k = 0; k < len; k++) padstr[k] = jnt.name[k];
            for(uint32 k = len; k < padlen; k++) padstr[k] = '\0';
            ofile.write((char*)&padlen, sizeof(padlen));
            ofile.write(padstr.get(), padlen);
            if(ofile.fail()) return error("Write failure.");
           }
         // length of zero means use default name "jnt_" + id
         else {
            uint32 zero = 0;
            ofile.write((char*)&zero, sizeof(zero));
            if(ofile.fail()) return error("Write failure.");
           }

         // save id
         ofile.write((char*)&jnt.id, sizeof(jnt.id));
         if(ofile.fail()) return error("Write failure.");

         // save parent
         ofile.write((char*)&jnt.parent, sizeof(jnt.parent));
         if(ofile.fail()) return error("Write failure.");

         // save matrix or quaternion
         if(skel.format & AMC_JOINT_FORMAT_MATRIX) {
            ofile.write((char*)&jnt.m_abs[0], 16 * sizeof(real32));
            if(ofile.fail()) return error("Write failure.");
           }
         else {
            ofile.write((char*)&jnt.q_abs[0], 4 * sizeof(real32));
            if(ofile.fail()) return error("Write failure.");
           }

         // save position
         ofile.write((char*)&jnt.p_abs[0], 4 * sizeof(real32));
         if(ofile.fail()) return error("Write failure.");
        }
    }

 // save joint maps
 for(uint32 i = 0; i < data.jmaplist.size(); i++)
    {
     // save number of items
     uint32 n_items = data.jmaplist[i].jntmap.size();
     ofile.write((char*)&n_items, sizeof(n_items));
     if(ofile.fail()) return error("Write failure.");

     // save items
     for(uint32 j = 0; j < data.jmaplist[i].jntmap.size(); j++) {
         uint32 temp = data.jmaplist[i].jntmap[j];
         ofile.write((char*)&temp, sizeof(temp));
         if(ofile.fail()) return error("Write failure.");
        }
    }

 return true;
}

bool SaveOBJ(LPCTSTR path, LPCTSTR name, const ADVANCEDMODELCONTAINER& data)
{
 // validate
 using namespace std;
 if(!path) return error("Invalid path name.");
 if(!name) return error("Invalid file name.");

 // create MTL file
 STDSTRING mtlfname = path;
 mtlfname += name;
 mtlfname += TEXT(".mtl");
 ofstream mtlfile(mtlfname.c_str());
 if(!mtlfile) return error("Failed to create output file.");

 // save materials
 for(uint32 i = 0; i < data.surfaces.size(); i++) {
     // save material name
     const string& name = data.surfaces[i].name;
     mtlfile << "newmtl " << name << endl;

     // save material
     const uint32 surfmat_index = data.surfaces[i].surfmat;
     if(surfmat_index != AMC_INVALID_SURFMAT) {
        if(data.surfmats[surfmat_index].basemap != AMC_INVALID_TEXTURE)
           mtlfile << "map_Kd " << data.iflist[data.surfmats[surfmat_index].basemap].filename << endl;
       }
     mtlfile << endl;
    }

 // create OBJ file
 STDSTRING objfname = path;
 objfname += name;
 objfname += TEXT(".obj");
 ofstream objfile(objfname.c_str());
 if(!objfile) return error("Failed to create output file.");

 // convert name to char string for Wavefront OBJ standard
 auto nameUTF8 = Unicode16ToUnicode08(name);
 if(!nameUTF8) return error("Failed to create ASCII object name for OBJ file.");

 // save object name
 objfile << "#" << endl;
 objfile << "# OBJECT NAME" << endl;
 objfile << "#" << endl;
 objfile << "o model_" << nameUTF8.get() << endl;
 objfile << "mtllib " << nameUTF8.get() << ".mtl" << endl;
 objfile << endl;

 // initialize vertex information
 struct OBJVERTEXINFO {
  uint32 p01;
  uint32 p02;
  uint32 p03;
 };
 deque<OBJVERTEXINFO> objvertinfo;
 OBJVERTEXINFO item;
 item.p01 = 0;
 item.p02 = 0;
 item.p03 = 0;
 objvertinfo.push_back(item);

 // save vertex buffers
 for(uint32 i = 0; i < data.vblist.size(); i++)
    {
     // comment
     objfile << "#" << endl;
     objfile << "# VERTEX BUFFER " << setfill('0') << setw(3) << i << endl;
     objfile << "#" << endl;

     // get vertex buffer
     const AMC_VTXBUFFER& vb = data.vblist[i];

     // insert vertex information
     OBJVERTEXINFO item;
     item.p01 = objvertinfo[i].p01;
     item.p02 = objvertinfo[i].p02;
     item.p03 = objvertinfo[i].p03;
     if(vb.flags & AMC_VERTEX_POSITION) item.p01 += vb.elem;
     if(vb.flags & AMC_VERTEX_NORMAL) item.p02 += vb.elem;
     if(vb.flags & AMC_VERTEX_UV) item.p03 += vb.elem;
     objvertinfo.push_back(item);

     // save position
     if(vb.flags & AMC_VERTEX_POSITION) {
        for(uint32 j = 0; j < vb.elem; j++) {
            objfile << "v ";
            objfile << clip(vb.data[j].vx) << " ";
            objfile << clip(vb.data[j].vy) << " ";
            objfile << clip(vb.data[j].vz) << endl;
           }
        objfile << endl;
       }
     // save normal
     if(vb.flags & AMC_VERTEX_NORMAL) {
        for(uint32 j = 0; j < vb.elem; j++) {
            objfile << "vn ";
            objfile << clip(vb.data[j].nx) << " ";
            objfile << clip(vb.data[j].ny) << " ";
            objfile << clip(vb.data[j].nz) << endl;
           }
        objfile << endl;
       }
     // save texture coordinates
     if(vb.flags & AMC_VERTEX_UV) {
        // find diffuse channel texture coordinates
        uint16 diffuse_channel = 0;
        for(uint32 j = 0; j < vb.uvchan; j++) {
            if(vb.uvtype[j] == AMC_DIFFUSE_MAP) {
               diffuse_channel = j;
               break;
              }
           }
        // save texture coordinates
        for(uint32 j = 0; j < vb.elem; j++) {
            objfile << "vt ";
            objfile << clip(vb.data[j].uv[diffuse_channel][0]) << " ";
            objfile << clip(1.0f - vb.data[j].uv[diffuse_channel][1]) << endl;
           }
        objfile << endl;
       }

     // spacing
     objfile << endl;
    }

 // save face data
 for(uint32 i = 0; i < data.surfaces.size(); i++)
    {
     // comment
     objfile << "#" << endl;
     objfile << "# MESH " << setfill('0') << setw(3) << i << endl;
     objfile << "#" << endl;

     // save mesh name
     auto surface = data.surfaces[i];
     objfile << "g " << surface.name << endl;

     // save material
     if(surface.surfmat != AMC_INVALID_SURFMAT)
        objfile << "usemtl " << surface.name << endl;
     else {
        // save default material
        const string& name = surface.name;
        mtlfile << "newmtl " << name << endl;
        mtlfile << endl;
        // use default material
        objfile << "usemtl " << name << endl;
       }

     // save buffer references
     uint32 references = (uint32)surface.refs.size();
     for(uint32 j = 0; j < references; j++)
        {
         // get surface information
         uint32 vb_index = surface.refs[j].vb_index;
         uint32 vb_start = surface.refs[j].vb_start;
         uint32 vb_width = surface.refs[j].vb_width;
         uint32 ib_index = surface.refs[j].ib_index;
         uint32 ib_start = surface.refs[j].ib_start;
         uint32 ib_width = surface.refs[j].ib_width;

         // get vertex buffer
         const AMC_VTXBUFFER& vb = data.vblist[vb_index];
         bool has_position = (vb.flags & AMC_VERTEX_POSITION) != 0;
         bool has_normal = (vb.flags & AMC_VERTEX_NORMAL) != 0;
         bool has_texcoord = (vb.flags & AMC_VERTEX_UV) != 0;

         // get vertex information
         OBJVERTEXINFO item;
         item = objvertinfo[vb_index];

         // compute vertex buffer bases
         uint32 b01 = item.p01 + vb_start; // position
         uint32 b02 = item.p02 + vb_start; // normal
         uint32 b03 = item.p03 + vb_start; // texcoord

         // get index buffer
         const AMC_IDXBUFFER& ib = data.iblist[ib_index];

         if(ib.format == AMC_UINT08) {
            const uint08* ptr = reinterpret_cast<const uint08*>(ib.data.get());
            if(ib.type == AMC_TRIANGLES) {
               return error("Not supported yet.", __LINE__);
              }
            else if(ib.type == AMC_TRISTRIP) {
               return error("Not supported yet.", __LINE__);
              }
            else if(ib.type == AMC_TRISTRIPCUT) {
               return error("Not supported yet.", __LINE__);
              }
            else
               return error("Invalid index buffer primitive type.");
           }
         else if(ib.format == AMC_UINT16)
           {
            const uint16* ptr = reinterpret_cast<const uint16*>(ib.data.get());
            if(ib.type == AMC_TRIANGLES) {
               const uint16* startptr = &ptr[ib_start];
               uint32 n_triangles = ib_width/3;
               for(uint32 k = 0; k < n_triangles; k++) {
                   // original indices
                   uint32 a = startptr[3*k + 0];
                   uint32 b = startptr[3*k + 1];
                   uint32 c = startptr[3*k + 2];
                   // original indices + vertex base + OBJ one offset
                   uint32 aP = a + b01 + 1;
                   uint32 aN = a + b02 + 1;
                   uint32 aT = a + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 bP = b + b01 + 1;
                   uint32 bN = b + b02 + 1;
                   uint32 bT = b + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 cP = c + b01 + 1;
                   uint32 cN = c + b02 + 1;
                   uint32 cT = c + b03 + 1;
                   // print position index
                   objfile << "f ";
                   if(has_position && has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << "/" << aN << " ";
                      objfile << bP << "/" << bT << "/" << bN << " ";
                      objfile << cP << "/" << cT << "/" << cN << endl;
                     }
                   else if(has_position && has_normal && !has_texcoord) {
                      objfile << aP << "//" << aN << " ";
                      objfile << bP << "//" << bN << " ";
                      objfile << cP << "//" << cN << endl;
                     }
                   else if(has_position && !has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << " ";
                      objfile << bP << "/" << bT << " ";
                      objfile << cP << "/" << cT << endl;
                     }
                   else if(has_position && !has_normal && !has_texcoord) {
                      objfile << aP << " ";
                      objfile << bP << " ";
                      objfile << cP << endl;
                     }
                  }
              }
            else if(ib.type == AMC_TRISTRIP)
              {
               AMCTriangleList trilist;
               bool success = ConvertTriStripToTriangleList(reinterpret_cast<const uint16*>(ib.data.get()) + ib_start, ib_width, trilist);
               const uint16* ptr = reinterpret_cast<const uint16*>(trilist.data.get());
               uint32 n_triangles = trilist.triangles;
               for(uint32 k = 0; k < n_triangles; k++) {
                   // original indices
                   uint32 a = ptr[3*k + 0];
                   uint32 b = ptr[3*k + 1];
                   uint32 c = ptr[3*k + 2];
                   // original indices + vertex base + OBJ one offset
                   uint32 aP = a + b01 + 1;
                   uint32 aN = a + b02 + 1;
                   uint32 aT = a + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 bP = b + b01 + 1;
                   uint32 bN = b + b02 + 1;
                   uint32 bT = b + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 cP = c + b01 + 1;
                   uint32 cN = c + b02 + 1;
                   uint32 cT = c + b03 + 1;
                   // print position index
                   objfile << "f ";
                   if(has_position && has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << "/" << aN << " ";
                      objfile << bP << "/" << bT << "/" << bN << " ";
                      objfile << cP << "/" << cT << "/" << cN << endl;
                     }
                   else if(has_position && has_normal && !has_texcoord) {
                      objfile << aP << "//" << aN << " ";
                      objfile << bP << "//" << bN << " ";
                      objfile << cP << "//" << cN << endl;
                     }
                   else if(has_position && !has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << " ";
                      objfile << bP << "/" << bT << " ";
                      objfile << cP << "/" << cT << endl;
                     }
                   else if(has_position && !has_normal && !has_texcoord) {
                      objfile << aP << " ";
                      objfile << bP << " ";
                      objfile << cP << endl;
                     }
                  }
              }
            else if(ib.type == AMC_TRISTRIPCUT)
              {
               AMCTriangleList trilist;
               bool success = ConvertStripCutToTriangleList(reinterpret_cast<const uint16*>(ib.data.get()) + ib_start, ib_width, trilist, (uint16)0xFFFF);
               const uint16* ptr = reinterpret_cast<const uint16*>(trilist.data.get());
               uint32 n_triangles = trilist.triangles;
               for(uint32 k = 0; k < n_triangles; k++) {
                   // original indices
                   uint32 a = ptr[3*k + 0];
                   uint32 b = ptr[3*k + 1];
                   uint32 c = ptr[3*k + 2];
                   // original indices + vertex base + OBJ one offset
                   uint32 aP = a + b01 + 1;
                   uint32 aN = a + b02 + 1;
                   uint32 aT = a + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 bP = b + b01 + 1;
                   uint32 bN = b + b02 + 1;
                   uint32 bT = b + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 cP = c + b01 + 1;
                   uint32 cN = c + b02 + 1;
                   uint32 cT = c + b03 + 1;
                   // print position index
                   objfile << "f ";
                   if(has_position && has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << "/" << aN << " ";
                      objfile << bP << "/" << bT << "/" << bN << " ";
                      objfile << cP << "/" << cT << "/" << cN << endl;
                     }
                   else if(has_position && has_normal && !has_texcoord) {
                      objfile << aP << "//" << aN << " ";
                      objfile << bP << "//" << bN << " ";
                      objfile << cP << "//" << cN << endl;
                     }
                   else if(has_position && !has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << " ";
                      objfile << bP << "/" << bT << " ";
                      objfile << cP << "/" << cT << endl;
                     }
                   else if(has_position && !has_normal && !has_texcoord) {
                      objfile << aP << " ";
                      objfile << bP << " ";
                      objfile << cP << endl;
                     }
                  }
              }
            else
               return error("Invalid index buffer primitive type.");
           }
         else if(ib.format == AMC_UINT32)
           {
            const uint32* ptr = reinterpret_cast<const uint32*>(ib.data.get());
            if(ib.type == AMC_TRIANGLES)
              {
               const uint32* startptr = &ptr[ib_start];
               uint32 n_triangles = ib_width/3;
               for(uint32 k = 0; k < n_triangles; k++)
                  {
                   // original indices
                   uint32 a = startptr[3*k + 0];
                   uint32 b = startptr[3*k + 1];
                   uint32 c = startptr[3*k + 2];
                   // original indices + vertex base + OBJ one offset
                   uint32 aP = a + b01 + 1;
                   uint32 aN = a + b02 + 1;
                   uint32 aT = a + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 bP = b + b01 + 1;
                   uint32 bN = b + b02 + 1;
                   uint32 bT = b + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 cP = c + b01 + 1;
                   uint32 cN = c + b02 + 1;
                   uint32 cT = c + b03 + 1;
                   // print position index
                   objfile << "f ";
                   if(has_position && has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << "/" << aN << " ";
                      objfile << bP << "/" << bT << "/" << bN << " ";
                      objfile << cP << "/" << cT << "/" << cN << endl;
                     }
                   else if(has_position && has_normal && !has_texcoord) {
                      objfile << aP << "//" << aN << " ";
                      objfile << bP << "//" << bN << " ";
                      objfile << cP << "//" << cN << endl;
                     }
                   else if(has_position && !has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << " ";
                      objfile << bP << "/" << bT << " ";
                      objfile << cP << "/" << cT << endl;
                     }
                   else if(has_position && !has_normal && !has_texcoord) {
                      objfile << aP << " ";
                      objfile << bP << " ";
                      objfile << cP << endl;
                     }
                  }
              }
            else if(ib.type == AMC_TRISTRIP)
              {
               AMCTriangleList trilist;
               bool success = ConvertTriStripToTriangleList(reinterpret_cast<const uint32*>(ib.data.get()) + ib_start, ib_width, trilist);
               const uint32* ptr = reinterpret_cast<const uint32*>(trilist.data.get());
               uint32 n_triangles = trilist.triangles;
               for(uint32 k = 0; k < n_triangles; k++) {
                   // original indices
                   uint32 a = ptr[3*k + 0];
                   uint32 b = ptr[3*k + 1];
                   uint32 c = ptr[3*k + 2];
                   // original indices + vertex base + OBJ one offset
                   uint32 aP = a + b01 + 1;
                   uint32 aN = a + b02 + 1;
                   uint32 aT = a + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 bP = b + b01 + 1;
                   uint32 bN = b + b02 + 1;
                   uint32 bT = b + b03 + 1;
                   // original indices + vertex base + OBJ one offset
                   uint32 cP = c + b01 + 1;
                   uint32 cN = c + b02 + 1;
                   uint32 cT = c + b03 + 1;
                   // print position index
                   objfile << "f ";
                   if(has_position && has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << "/" << aN << " ";
                      objfile << bP << "/" << bT << "/" << bN << " ";
                      objfile << cP << "/" << cT << "/" << cN << endl;
                     }
                   else if(has_position && has_normal && !has_texcoord) {
                      objfile << aP << "//" << aN << " ";
                      objfile << bP << "//" << bN << " ";
                      objfile << cP << "//" << cN << endl;
                     }
                   else if(has_position && !has_normal && has_texcoord) {
                      objfile << aP << "/" << aT << " ";
                      objfile << bP << "/" << bT << " ";
                      objfile << cP << "/" << cT << endl;
                     }
                   else if(has_position && !has_normal && !has_texcoord) {
                      objfile << aP << " ";
                      objfile << bP << " ";
                      objfile << cP << endl;
                     }
                  }
              }
            else if(ib.type == AMC_TRISTRIPCUT) {
               return error("Not supported yet.", __LINE__);
              }
            else
               return error("Invalid index buffer primitive type.");
           }
         else
            return error("Invalid index buffer format.");
        }

     // spacing
     objfile << endl;
    }

 return true;
}

bool TestAMC(void)
{
 // create AMC
 ADVANCEDMODELCONTAINER amc;

 // insert images
 amc.iflist.resize(3, AMC_IMAGEFILE());
 amc.iflist[0].filename = "image01.tga";
 amc.iflist[1].filename = "image02.tga";
 amc.iflist[2].filename = "image03.tga";

 // vbuffer #1
 AMC_VTXBUFFER vb1;
 vb1.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV | AMC_VERTEX_WEIGHTS;
 vb1.name = "not_important";
 vb1.elem = 3;
 vb1.data.reset(new AMC_VERTEX[vb1.elem]);
 vb1.data[0].vx = 0.0f; vb1.data[0].vy = 0.0f; vb1.data[0].vz = 0.0f;
 vb1.data[1].vx = 1.0f; vb1.data[1].vy = 1.0f; vb1.data[1].vz = 0.0f;
 vb1.data[2].vx = 2.0f; vb1.data[2].vy = 0.0f; vb1.data[2].vz = 0.0f;
 vb1.data[0].tu = 0.0f; vb1.data[0].tv = 0.0f;
 vb1.data[1].tu = 0.5f; vb1.data[1].tv = 1.0f;
 vb1.data[2].tu = 1.0f; vb1.data[2].tv = 0.0f;

 // point #1
 for(uint32 i = 0; i < 8; i++) vb1.data[0].wi[i] = AMC_INVALID_VERTEX_WMAP_INDEX;
 vb1.data[0].wv[0] = 0.0f; vb1.data[0].wi[0] = 0;
 vb1.data[0].wv[1] = 1.0f; vb1.data[0].wi[1] = 1;

 // point #2
 for(uint32 i = 0; i < 8; i++) vb1.data[1].wi[i] = AMC_INVALID_VERTEX_WMAP_INDEX;
 vb1.data[1].wv[0] = 0.5f; vb1.data[1].wi[0] = 0;
 vb1.data[1].wv[1] = 0.5f; vb1.data[1].wi[1] = 1;

 // point #3
 for(uint32 i = 0; i < 8; i++) vb1.data[2].wi[i] = AMC_INVALID_VERTEX_WMAP_INDEX;
 vb1.data[2].wv[0] = 1.0f; vb1.data[2].wi[0] = 0;
 vb1.data[2].wv[1] = 0.0f; vb1.data[2].wi[1] = 1;

 // vbuffer #2
 AMC_VTXBUFFER vb2;
 vb2.flags = AMC_VERTEX_POSITION | AMC_VERTEX_UV | AMC_VERTEX_WEIGHTS;
 vb2.name = "not_important";
 vb2.elem = 3;
 vb2.data.reset(new AMC_VERTEX[vb2.elem]);
 vb2.data[0].vx = 0.0f; vb2.data[0].vy = 1.0f; vb2.data[0].vz = 0.0f;
 vb2.data[1].vx = 1.0f; vb2.data[1].vy = 2.0f; vb2.data[1].vz = 0.0f;
 vb2.data[2].vx = 2.0f; vb2.data[2].vy = 1.0f; vb2.data[2].vz = 0.0f;
 vb2.data[0].tu = 0.0f; vb2.data[0].tv = 0.0f;
 vb2.data[1].tu = 0.5f; vb2.data[1].tv = 1.0f;
 vb2.data[2].tu = 1.0f; vb2.data[2].tv = 0.0f;

 // point #1
 for(uint32 i = 0; i < 8; i++) vb2.data[0].wi[i] = AMC_INVALID_VERTEX_WMAP_INDEX;
 vb2.data[0].wv[0] = 0.0f; vb2.data[0].wi[0] = 2;
 vb2.data[0].wv[1] = 1.0f; vb2.data[0].wi[1] = 3;

 // point #2
 for(uint32 i = 0; i < 8; i++) vb2.data[1].wi[i] = AMC_INVALID_VERTEX_WMAP_INDEX;
 vb2.data[1].wv[0] = 0.5f; vb2.data[1].wi[0] = 2;
 vb2.data[1].wv[1] = 0.5f; vb2.data[1].wi[1] = 3;

 // point #3
 for(uint32 i = 0; i < 8; i++) vb2.data[2].wi[i] = AMC_INVALID_VERTEX_WMAP_INDEX;
 vb2.data[2].wv[0] = 1.0f; vb2.data[2].wi[0] = 2;
 vb2.data[2].wv[1] = 0.0f; vb2.data[2].wi[1] = 3;

 // vbuffer #3
 AMC_VTXBUFFER vb3;
 vb3.flags = AMC_VERTEX_POSITION;
 vb3.name = "not_important";
 vb3.elem = 3;
 vb3.data.reset(new AMC_VERTEX[vb3.elem]);
 vb3.data[0].vx = 0.0f; vb3.data[0].vy = 2.0f; vb3.data[0].vz = 0.0f;
 vb3.data[1].vx = 1.0f; vb3.data[1].vy = 3.0f; vb3.data[1].vz = 0.0f;
 vb3.data[2].vx = 2.0f; vb3.data[2].vy = 2.0f; vb3.data[2].vz = 0.0f;

 // ibuffer #1 (we can reuse the index buffer)
 AMC_IDXBUFFER ib1;
 ib1.format = AMC_UINT08;
 ib1.type = AMC_TRIANGLES;
 ib1.name = "not important";
 ib1.elem = 3;
 ib1.data.reset(new char[ib1.elem*sizeof(uint08)]);
 ib1.data[0] = 0;
 ib1.data[1] = 1;
 ib1.data[2] = 2;

 // insert buffers
 amc.vblist.push_back(vb1);
 amc.vblist.push_back(vb2);
 amc.vblist.push_back(vb3);
 amc.iblist.push_back(ib1);

 // define surfaces
 AMC_SURFACE s1;
 s1.name = "triangle1";
 s1.surfmat = AMC_INVALID_SURFMAT;
 s1.refs.push_back(AMC_REFERENCE());
 s1.refs[0].vb_index = 0;
 s1.refs[0].vb_start = 0;
 s1.refs[0].vb_width = 3;
 s1.refs[0].ib_index = 0;
 s1.refs[0].ib_start = 0;
 s1.refs[0].ib_width = 3;

 AMC_SURFACE s2;
 s2.name = "triangle2";
 s2.surfmat = AMC_INVALID_SURFMAT;
 s2.refs.push_back(AMC_REFERENCE());
 s2.refs[0].vb_index = 1;
 s2.refs[0].vb_start = 0;
 s2.refs[0].vb_width = 3;
 s2.refs[0].ib_index = 0;
 s2.refs[0].ib_start = 0;
 s2.refs[0].ib_width = 3;

 AMC_SURFACE s3;
 s3.name = "triangle1"; // use same surface
 s3.surfmat = AMC_INVALID_SURFMAT;
 s3.refs.push_back(AMC_REFERENCE());
 s3.refs[0].vb_index = 2;
 s3.refs[0].vb_start = 0;
 s3.refs[0].vb_width = 3;
 s3.refs[0].ib_index = 0;
 s3.refs[0].ib_start = 0;
 s3.refs[0].ib_width = 3;

 // insert surfaces
 amc.surfaces.push_back(s1);
 amc.surfaces.push_back(s2);
 amc.surfaces.push_back(s3);

 // create root joint
 AMC_JOINT j1;
 j1.name = "joint1";
 j1.parent = AMC_INVALID_JOINT;
 j1.m_abs[ 0] = 1.0f; j1.m_abs[ 1] = 0.0f; j1.m_abs[ 2] = 0.0f; j1.m_abs[ 3] = 0.0f;
 j1.m_abs[ 4] = 0.0f; j1.m_abs[ 5] = 1.0f; j1.m_abs[ 6] = 0.0f; j1.m_abs[ 7] = 0.0f;
 j1.m_abs[ 8] = 0.0f; j1.m_abs[ 9] = 0.0f; j1.m_abs[10] = 1.0f; j1.m_abs[11] = 0.0f;
 j1.m_abs[12] = 0.0f; j1.m_abs[13] = 0.0f; j1.m_abs[14] = 0.0f; j1.m_abs[16] = 1.0f;
 j1.p_abs[0] = 0.0f;
 j1.p_abs[1] = 0.0f;
 j1.p_abs[2] = 0.0f;

 // create joint #2
 AMC_JOINT j2;
 j2.name = "joint2";
 j2.parent = 0;
 j2.m_abs[ 0] = 1.0f; j1.m_abs[ 1] = 0.0f; j1.m_abs[ 2] = 0.0f; j1.m_abs[ 3] = 0.0f;
 j2.m_abs[ 4] = 0.0f; j1.m_abs[ 5] = 1.0f; j1.m_abs[ 6] = 0.0f; j1.m_abs[ 7] = 0.0f;
 j2.m_abs[ 8] = 0.0f; j1.m_abs[ 9] = 0.0f; j1.m_abs[10] = 1.0f; j1.m_abs[11] = 0.0f;
 j2.m_abs[12] = 0.0f; j1.m_abs[13] = 0.0f; j1.m_abs[14] = 0.0f; j1.m_abs[16] = 1.0f;
 j2.p_abs[0] = 1.0f;
 j2.p_abs[1] = 1.0f;
 j2.p_abs[2] = 0.0f;

 // create joint #3
 AMC_JOINT j3;
 j3.name = "joint3";
 j3.parent = 1;
 j3.m_abs[ 0] = 1.0f; j1.m_abs[ 1] = 0.0f; j1.m_abs[ 2] = 0.0f; j1.m_abs[ 3] = 0.0f;
 j3.m_abs[ 4] = 0.0f; j1.m_abs[ 5] = 1.0f; j1.m_abs[ 6] = 0.0f; j1.m_abs[ 7] = 0.0f;
 j3.m_abs[ 8] = 0.0f; j1.m_abs[ 9] = 0.0f; j1.m_abs[10] = 1.0f; j1.m_abs[11] = 0.0f;
 j3.m_abs[12] = 0.0f; j1.m_abs[13] = 0.0f; j1.m_abs[14] = 0.0f; j1.m_abs[16] = 1.0f;
 j3.p_abs[0] = 2.0f;
 j3.p_abs[1] = 0.0f;
 j3.p_abs[2] = 0.0f;

 // create skeleton
 AMC_SKELETON skeleton;
 skeleton.format = AMC_JOINT_FORMAT_ABSOLUTE | AMC_JOINT_FORMAT_MATRIX;
 skeleton.name = "skeleton1";
 InsertJoint(skeleton, j1);
 InsertJoint(skeleton, j2);
 InsertJoint(skeleton, j3);
 amc.skllist.push_back(skeleton);

 return SaveLWO("c:\\users\\semory\\desktop\\", "test", amc);
}

#pragma endregion

