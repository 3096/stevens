#ifndef __CS_CG_AABB_H
#define __CS_CG_AABB_H

// STL Headers
#include<cmath>
#include<ostream>

// CS Headers
#include<cs/core/types.h>
#include<cs/math/vector2D.h>
#include<cs/math/vector3D.h>

//
// FORWARD DECLARATIONS
//

namespace cs { namespace cg {

template<class T> class min_dim_AABB;
template<class T> class halfdim_AABB;
template<class T> class min_max_AABB;

}}

//
// AABB CLASSES
//
namespace cs { namespace cg {

using cs::math::vector2D;
using cs::math::vector3D;

template<class T>
class halfdim_AABB {
 public :
  T center[3];
  T radius[3];
};

template<class T>
class min_max_AABB {
 public :
  T min[3];
  T max[3];
 public :
  min_max_AABB() {}
  explicit min_max_AABB(T fill) { from(fill); }
  min_max_AABB(T a, T b) { from(a, b); }
  min_max_AABB(T x1, T y1, T z1, T x2, T y2, T z2) { from(x1, y1, z1, x2, y2, z2); }
  explicit min_max_AABB(const T* A) { from(A); }
  min_max_AABB(const T* A, const T* B) { from(A, B); }
  min_max_AABB(const T* A, const T* B, const T* C) { from(A, B, C); }
  min_max_AABB(const min_max_AABB<T>& other)
  {
   min[0] = other.min[0]; min[1] = other.min[1]; min[2] = other.min[2];
   max[0] = other.max[0]; max[1] = other.max[1]; max[2] = other.max[2];
  }
 public :
  min_max_AABB<T>& operator =(const min_max_AABB<T>& other)
  {
   if(this == &other) return *this;
   min[0] = other.min[0]; min[1] = other.min[1]; min[2] = other.min[2];
   max[0] = other.max[0]; max[1] = other.max[1]; max[2] = other.max[2];
   return *this;
  }
 public :
  void from(T fill)
  {
   min[0] = min[1] = min[2] = fill;
   max[0] = max[1] = max[2] = fill;
  }
  void from(T a, T b)
  {
   min[0] = min[1] = min[2] = a;
   max[0] = max[1] = max[2] = b;
  }
  void from(T x1, T y1, T z1, T x2, T y2, T z2)
  {
   min[0] = x1; min[1] = y1; min[2] = z1;
   max[0] = x2; max[1] = y2; max[2] = z2;
  }
  void from(const T* A)
  {
   min[0] = max[0] = A[0];
   min[1] = max[1] = A[1];
   min[2] = max[2] = A[2];
  }
  void from(const T* A, const T* B)
  {
   // test A
   min[0] = max[0] = A[0];
   min[1] = max[1] = A[1];
   min[2] = max[2] = A[2];
   // test B
   if(B[0] < min[0]) min[0] = B[0]; else if(max[0] < B[0]) max[0] = B[0];
   if(B[1] < min[1]) min[1] = B[1]; else if(max[1] < B[1]) max[1] = B[1];
   if(B[2] < min[2]) min[2] = B[2]; else if(max[2] < B[2]) max[2] = B[2];
  }
  void from(const T* A, const T* B, const T* C)
  {
   // test A
   min[0] = max[0] = A[0];
   min[1] = max[1] = A[1];
   min[2] = max[2] = A[2];
   // test B
   if(B[0] < min[0]) min[0] = B[0]; else if(max[0] < B[0]) max[0] = B[0];
   if(B[1] < min[1]) min[1] = B[1]; else if(max[1] < B[1]) max[1] = B[1];
   if(B[2] < min[2]) min[2] = B[2]; else if(max[2] < B[2]) max[2] = B[2];
   // test C
   if(C[0] < min[0]) min[0] = C[0]; else if(max[0] < C[0]) max[0] = C[0];
   if(C[1] < min[1]) min[1] = C[1]; else if(max[1] < C[1]) max[1] = C[1];
   if(C[2] < min[2]) min[2] = C[2]; else if(max[2] < C[2]) max[2] = C[2];
  }
 public :
  void grow(int index, T value)
  {
   if(value < min[index]) min[index] = value;
   if(max[index] < value) max[index] = value;
  }
  void grow(T x, T y, T z)
  {
   grow(0, x);
   grow(1, y);
   grow(2, z);
  }
  void grow(const T* v)
  {
   grow(0, v[0]);
   grow(1, v[1]);
   grow(2, v[2]);
  }
  void grow(const vector2D<T>& src)
  {
   grow(0, src[0]);
   grow(1, src[1]);
  }
  void grow(const vector3D<T>& src)
  {
   grow(0, src[0]);
   grow(1, src[1]);
   grow(2, src[2]);
  }
  void grow(const min_max_AABB<T>& src)
  {
   // update min bounds
   if(src.min[0] < min[0]) min[0] = src.min[0];
   if(src.min[1] < min[1]) min[1] = src.min[1];
   if(src.min[2] < min[2]) min[2] = src.min[2];

   // update max bounds
   if(max[0] < src.max[0]) max[0] = src.max[0];
   if(max[1] < src.max[1]) max[1] = src.max[1];
   if(max[2] < src.max[2]) max[2] = src.max[2];
  }
 public :
  T dx(void)const
  {
   return max[0] - min[0];
  }
  T dy(void)const
  {
   return max[1] - min[1];
  }
  T dz(void)const
  {
   return max[2] - min[2];
  }
};

template<class T>
inline vector3D<T> centroid(const min_max_AABB<T>& aabb)
{
 T half = static_cast<T>(0.5);
 vector3D<T> retval;
 retval[0] = (aabb.min[0] + aabb.max[0])*half;
 retval[1] = (aabb.min[1] + aabb.max[1])*half;
 retval[2] = (aabb.min[2] + aabb.max[2])*half;
 return retval;
}

template<class T>
inline void centroid(const min_max_AABB<T>& aabb, T* out)
{
 if(!out) return;
 out[0] = (aabb.min[0] + aabb.max[0])*static_cast<T>(0.5);
 out[1] = (aabb.min[1] + aabb.max[1])*static_cast<T>(0.5);
 out[2] = (aabb.min[2] + aabb.max[2])*static_cast<T>(0.5);
}

template<class T>
inline T half_surface_area(const min_max_AABB<T>& aabb)
{
 T dx = aabb.dx();
 T dy = aabb.dy();
 T dz = aabb.dz();
 return dx*dy + dx*dz + dy*dz;
}

template<class T>
inline T surface_area(const min_max_AABB<T>& aabb)
{
 T dx = aabb.dx();
 T dy = aabb.dy();
 T dz = aabb.dz();
 return static_cast<T>(2)*(dx*dy + dx*dz + dy*dz);
}

template<class T>
inline T volume(const min_max_AABB<T>& aabb)
{
 T dx = aabb.dx();
 T dy = aabb.dy();
 T dz = aabb.dz();
 return dx*dy*dz;
}

template<class T>
inline int dominator(const min_max_AABB<T>& aabb)
{
 T _dx = aabb.dx();
 T _dy = aabb.dy();
 T _dz = aabb.dz();
 if(_dx < _dy) return (_dy < _dz ? 2 : 1); // not dx
 return (_dx < _dz ? 2 : 0); // not dy
}

template<class T>
inline int submissor(const min_max_AABB<T>& aabb)
{
 T _dx = aabb.dx();
 T _dy = aabb.dy();
 T _dz = aabb.dz();
 if(_dz < _dy) return (_dz < _dx ? 2 : 0); // not dy
 return (_dy < _dx ? 1 : 0); // not dz
}

template<class T>
inline int dominator(const min_max_AABB<T>& aabb, T* dims)
{
 T _dx = aabb.dx();
 T _dy = aabb.dy();
 T _dz = aabb.dz();
 if(dims) {
    dims[0] = _dx;
    dims[1] = _dy;
    dims[2] = _dz;
   }
 if(_dx < _dy) return (_dy < _dz ? 2 : 1); // not dx
 return (_dx < _dz ? 2 : 0); // not dy
}

template<class T>
inline int submissor(const min_max_AABB<T>& aabb, T* dims)
{
 T _dx = aabb.dx();
 T _dy = aabb.dy();
 T _dz = aabb.dz();
 if(dims) {
    dims[0] = _dx;
    dims[1] = _dy;
    dims[2] = _dz;
   }
 if(_dz < _dy) return (_dz < _dx ? 2 : 0); // not dy
 return (_dy < _dx ? 1 : 0); // not dz
}

template<class T>
inline std::ostream& operator <<(std::ostream& os, const min_max_AABB<T>& obj)
{
 os << "{" << obj.min[0] << ", " << obj.min[1] << ", " << obj.min[2] << "} ";
 os << "{" << obj.max[0] << ", " << obj.max[1] << ", " << obj.max[2] << "}";
 return os;
}

// TODO: move to intersect.h
template<class T>
inline bool AABB_intersect3D(const halfdim_AABB<T>& b1, const halfdim_AABB<T>& b2)
{
 if(std::abs(b1.center[0] - b2.center[0]) > (b1.radius[0] + b2.radius[0])) return false;
 if(std::abs(b1.center[1] - b2.center[1]) > (b1.radius[1] + b2.radius[1])) return false;
 if(std::abs(b1.center[2] - b2.center[2]) > (b1.radius[2] + b2.radius[2])) return false;
 return true;
}

// TODO: move to intersect.h
template<class T, class ZeroTest>
inline bool AABB_ray_intersect3D(const min_max_AABB<T>& B, const T* O, const T* D, ZeroTest zerotest)
{
 //
 // X-AXIS TEST
 //

 if(zerotest(D[0])) { if((O[0] < B.min[0]) || (B.max[0] < O[0])) return false; } // ray is in Y-Z plane
 T scalar = static_cast<T>(1)/D[0];
 T t1 = (B.min[0] - O[0])*scalar;
 T t2 = (B.max[0] - O[0])*scalar;
 if(t2 < t1) std::swap(t1, t2);
 T tmin = t1;
 T tmax = t2;

 //
 // Y-AXIS TEST
 //

 if(zerotest(D[1])) { if((O[1] < B.min[1]) || (B.max[1] < O[1])) return false; } // ray is in X-Z plane
 scalar = static_cast<T>(1)/D[1];
 t1 = (B.min[1] - O[1])*scalar;
 t2 = (B.max[1] - O[1])*scalar;
 if(t2 < t1) std::swap(t1, t2);
 if(tmin < t1) tmin = t1; // take farther one
 if(t2 < tmax) tmax = t2; // take closest one
 if(tmax < tmin) return false;

 //
 // Z-AXIS TEST
 //

 if(zerotest(D[2])) { if((O[2] < B.min[2]) || (B.max[2] < O[2])) return false; } // ray is in X-Y plane
 scalar = static_cast<T>(1)/D[2];
 t1 = (B.min[2] - O[2])*scalar;
 t2 = (B.max[2] - O[2])*scalar;
 if(t2 < t1) std::swap(t1, t2);
 if(tmin < t1) tmin = t1; // take farther one
 if(t2 < tmax) tmax = t2; // take closest one
 if(tmax < tmin) return false;

 // return closest and farther points
 // r1 = O + D*tmin
 // r2 = O + D*tmax
 return !(tmax < static_cast<T>(0));
}

// TODO: move to intersect.h
template<class T, class ZeroTest>
bool AABB_ray_intersect3D(const halfdim_AABB<T>& B, const T* O, const T* D, ZeroTest zerotest)
{
 //
 // X-AXIS TEST
 //

 if(zerotest(D[0])) { if(B.radius[0] < std::abs(B.center[0] - O[0])) return false; } // ray is in Y-Z plane
 T scalar = static_cast<T>(1)/D[0];
 T t1 = ((B.center[0] - B.radius[0]) - O[0])*scalar;
 T t2 = ((B.center[0] + B.radius[0]) - O[0])*scalar;
 if(t2 < t1) std::swap(t1, t2);
 T tmin = t1;
 T tmax = t2;

 //
 // Y-AXIS TEST
 //

 if(zerotest(D[1])) { if(B.radius[1] < std::abs(B.center[1] - O[1])) return false; } // ray is in X-Z plane
 scalar = static_cast<T>(1)/D[1];
 t1 = ((B.center[1] - B.radius[1]) - O[1])*scalar;
 t2 = ((B.center[1] + B.radius[1]) - O[1])*scalar;
 if(t2 < t1) std::swap(t1, t2);
 if(tmin < t1) tmin = t1; // take farther one
 if(t2 < tmax) tmax = t2; // take closest one
 if(tmax < tmin) return false;

 //
 // Z-AXIS TEST
 //

 // ray is in X-Y plane
 if(zerotest(D[2])) { if(B.radius[2] < std::abs(B.center[2] - O[2])) return false; }
 scalar = static_cast<T>(1)/D[2];
 t1 = ((B.center[2] - B.radius[2]) - O[2])*scalar;
 t2 = ((B.center[2] + B.radius[2]) - O[2])*scalar;
 if(t2 < t1) std::swap(t1, t2);
 if(tmin < t1) tmin = t1; // take farther one
 if(t2 < tmax) tmax = t2; // take closest one
 if(tmax < tmin) return false;

 // return closest and farther points
 // r1 = O + D*tmin
 // r2 = O + D*tmax
 return !(tmax < static_cast<T>(0));
}

// triangle2D centroid
template<class T>
void centroid2D(const T* A, const T* B, const T* C, T* out)
{
 out[0] = (A[0] + B[0] + C[0])/T(3);
 out[1] = (A[1] + B[1] + C[1])/T(3);
}

// triangle3D centroid
template<class T>
void centroid3D(const T* A, const T* B, const T* C, T* out)
{
 out[0] = (A[0] + B[0] + C[0])/T(3);
 out[1] = (A[1] + B[1] + C[1])/T(3);
 out[2] = (A[2] + B[2] + C[2])/T(3);
}

}}

#endif
