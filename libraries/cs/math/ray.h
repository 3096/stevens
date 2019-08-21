#ifndef __CS_MATH_RAY_H
#define __CS_MATH_RAY_H

namespace cs { namespace math {

template<class T>
struct ray2D {
 T origin[2];
 T direction[2];
};

template<class T>
struct ray3D {
 T origin[3];
 T direction[3];
};

template<class T>
struct ray3D_plane3D_intersect_result {
 bool intersect;
 T distance;
};

template<class T, class CloseToZeroTest>
inline void ray3D_plane3D_intersect(ray3D_plane3D_intersect_result<T>& result, const ray3D<T>& ray, const T* point, const T* normal, CloseToZeroTest zerotest)
{
 // compute numerator
 T numer = (point[0] - ray.origin[0]) * normal[0] +
           (point[1] - ray.origin[1]) * normal[1] +
           (point[2] - ray.origin[2]) * normal[2];

 // compute denominator
 T denom = (ray.direction[0] * normal[0]) +
           (ray.direction[1] * normal[1]) +
           (ray.direction[2] * normal[2]);

 // ray is parallel (or close to parallel)...
 if(zerotest(denom)) {
    // but contained in plane
    if(zerotest(numer)) {
       result.intersect = true;
       result.distance = static_cast<T>(0);
      }
    // but does not intersect
    else
       result.intersect = false;
   }
 // otherwise
 else {
    result.intersect = true;
    result.distance = numer/denom;
   }
}

template<class T>
struct ray3D_triangle3D_intersect_result {
 bool intersect;
 T t;
 T u;
 T v;
};

template<class T, class CloseToZeroTest>
inline void ray3D_triangle3D_intersect(ray3D_triangle3D_intersect_result<T>& result, const ray3D<T>& ray, const T* A, const T* B, const T* C, bool cull, CloseToZeroTest zerotest)
{
 // compute E = C - A
 // compute F = C - B
 T E[3] = { C[0] - A[0], C[1] - A[1], C[2] - A[2] };
 T F[3] = { C[0] - B[0], C[1] - B[1], C[2] - B[2] };

 // compute F cross D
 T F_cross_D[3] = {
  (F[1]*ray.direction[2] - F[2]*ray.direction[1]),
  (F[2]*ray.direction[0] - F[0]*ray.direction[2]),
  (F[0]*ray.direction[1] - F[1]*ray.direction[0])
 };

 // compute determinant
 // if this dot product is zero, then the ray and normal vector of the triangle are orthogonal,
 // which means that the ray and triangle edge vectors are parallel
 T determinant = E[0]*F_cross_D[0] + E[1]*F_cross_D[1] + E[2]*F_cross_D[2];
 if(zerotest(determinant))
   {
    // compute L = C - O
    T L[3] = {
     C[0] - ray.origin[0],
     C[1] - ray.origin[1],
     C[2] - ray.origin[2]
    };

    // compute (L dot (F cross D))
    // if this triple scalar product is zero, the ray and triangle are in the same plane
    T L_dot_F_cross_D = (L[0]*F_cross_D[0] + L[1]*F_cross_D[1] + L[2]*F_cross_D[2]);
    if(zerotest(L_dot_F_cross_D))
      {
       // may or may not intersect, but just say no anyways
       result.intersect = false;
      }
    else
       result.intersect = false;

    return;
   }

 // 
 if(cull)
   {
   }
 // not culling
 else
   {
    // compute L = C - O
    T inverse_determinant = static_cast<T>(1)/determinant;
    T L[3] = {
     C[0] - ray.origin[0],
     C[1] - ray.origin[1],
     C[2] - ray.origin[2]
    };

    // compute u = (L dot (F cross D))/determinant
    T u = (L[0]*F_cross_D[0] + L[1]*F_cross_D[1] + L[2]*F_cross_D[2])*inverse_determinant;
    if(u < static_cast<T>(0) || u > static_cast<T>(1)) {
       result.intersect = false;
       return;
      }

    // compute E cross L
    T E_cross_L[3] = {
     (E[1]*L[2] - E[2]*L[1]),
     (E[2]*L[0] - E[0]*L[2]),
     (E[0]*L[1] - E[1]*L[0])
    };

    // compute v = (D dot (E cross L))/determinant
    T v = (ray.direction[0]*E_cross_L[0] + ray.direction[1]*E_cross_L[1] + ray.direction[2]*E_cross_L[2])*inverse_determinant;
    if((v < static_cast<T>(0)) || ((u + v) > static_cast<T>(1))) {
       result.intersect = false;
       return;
      }

    // compute t = (F dot (L cross E))/determinant
    result.intersect = true;
    result.t = -(F[0]*E_cross_L[0] + F[1]*E_cross_L[1] + F[2]*E_cross_L[2])*inverse_determinant;
    result.u = u;
    result.v = v;
   }
}

}}

#endif
