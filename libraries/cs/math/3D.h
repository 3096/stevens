#ifndef __CS_MATH_3D__
#define __CS_MATH_3D__

// CS Headers
#include<cs/math/functions.h>
#include<cs/math/error.h>

//
// INTERSECTION
//
namespace cs { namespace math {

template<class T>
inline void ray3D_traverse(T* X, const T* O, const T* V, const T& distance)
{
 X[0] = O[0] - V[0]*distance;
 X[1] = O[1] - V[1]*distance;
 X[2] = O[2] - V[2]*distance;
}
 
template<class T>
inline bool ray3D_intersect_uncertain(T* X, const T* O1, const T* V1, const T* O2, const T* V2)
{
 // origin #1
 uncertain_number<T> o1[3] = {
  uncertain_number<T>(O1[0], binary_traits<T>::spacing(O1[0])),
  uncertain_number<T>(O1[1], binary_traits<T>::spacing(O1[1])),
  uncertain_number<T>(O1[2], binary_traits<T>::spacing(O1[2])),
 };

 // origin #2
 uncertain_number<T> o2[3] = {
  uncertain_number<T>(O2[0], binary_traits<T>::spacing(O2[0])),
  uncertain_number<T>(O2[1], binary_traits<T>::spacing(O2[1])),
  uncertain_number<T>(O2[2], binary_traits<T>::spacing(O2[2])),
 };

 // vector #1
 uncertain_number<T> v1[3] = {
  uncertain_number<T>(V1[0], binary_traits<T>::spacing(V1[0])),
  uncertain_number<T>(V1[1], binary_traits<T>::spacing(V1[1])),
  uncertain_number<T>(V1[2], binary_traits<T>::spacing(V1[2])),
 };

 // vector #2
 uncertain_number<T> v2[3] = {
  uncertain_number<T>(V2[0], binary_traits<T>::spacing(V2[0])),
  uncertain_number<T>(V2[1], binary_traits<T>::spacing(V2[1])),
  uncertain_number<T>(V2[2], binary_traits<T>::spacing(V2[2])),
 };

 // normalize vectors
 vector3D_normalize(v1);
 vector3D_normalize(v2);
 cout << "UNCERTAIN:" << endl;
 cout << " <" << v1[0].value << ", " << v1[1].value << ", " << v1[2].value << ">" << endl;
 cout << " <" << v2[0].value << ", " << v2[1].value << ", " << v2[2].value << ">" << endl;

 // cached values (dx + uncertainties)
 uncertain_number<T> dx1 = v1[0] - o1[0];
 uncertain_number<T> dx2 = v2[0] - o2[0];

 // cached values (dy + uncertainties)
 uncertain_number<T> dy1 = v1[1] - o1[1];
 uncertain_number<T> dy2 = v2[1] - o2[1];

 // cached values (denominator + uncertainties)
 uncertain_number<T> part1 = dx2*dy1;
 uncertain_number<T> part2 = dx1*dy2;

 // test denominator
 // if() {
 //    return false;
 //   }

 // compute denominator (denominator + uncertainties)
 uncertain_number<T> denom = part1 - part2;
 uncertain_number<T> cx = o2[0] - o1[0];
 uncertain_number<T> cy = o2[1] - o1[1];
 uncertain_number<T> t1 = (cy*dx2 - cx*dy2)/denom;
 uncertain_number<T> t2 = (cy*dx1 - cx*dy1)/denom;

 // cached values (dz + uncertainties)
 uncertain_number<T> dz1 = v1[2] - o1[2];
 uncertain_number<T> dz2 = v2[2] - o2[2];

 // check z
 uncertain_number<T> z1 = o1[2] + dz1*t1;
 uncertain_number<T> z2 = o2[2] + dz2*t2;

 cout << " denom = " << denom.value << endl;
 cout << " z1 = " << z1.value << endl;
 cout << " z2 = " << z2.value << endl;
 cout << " error = " << abs(z1.value - z2.value) << endl;
 cout << " z1_uncertainty = " << z1.error << endl;
 cout << " z2_uncertainty = " << z2.error << endl;

 // lower bound must be greater than zero
 uncertain_number<T> difference = z1 - z2;
 if(greater_than_zero(difference.value - difference.error)) return false;

 // success, close enough
 X[0] = o1[0].value + v1[0].value*t1.value;
 X[1] = o1[1].value + v1[1].value*t1.value;
 X[2] = o1[2].value + v1[2].value*t1.value;
 cout << "X[0] = " << X[0] << endl;
 cout << "X[1] = " << X[1] << endl;
 cout << "X[2] = " << X[2] << endl;
 return true;
}

template<class T, class EqualityTest>
inline bool ray3D_intersect(T* X, const T* O1, const T* V1, const T* O2, const T* V2, EqualityTest eqtest)
{
 // uncertainties
 T u_O1x = binary_traits<T>::spacing(O1[0]);
 T u_O1y = binary_traits<T>::spacing(O1[1]);
 T u_O1z = binary_traits<T>::spacing(O1[2]);
 T u_O2x = binary_traits<T>::spacing(O2[0]);
 T u_O2y = binary_traits<T>::spacing(O2[1]);
 T u_O2z = binary_traits<T>::spacing(O2[2]);
 T u_V1x = binary_traits<T>::spacing(V1[0]);
 T u_V1y = binary_traits<T>::spacing(V1[1]);
 T u_V1z = binary_traits<T>::spacing(V1[2]);
 T u_V2x = binary_traits<T>::spacing(V2[0]);
 T u_V2y = binary_traits<T>::spacing(V2[1]);
 T u_V2z = binary_traits<T>::spacing(V2[2]);

 // (O + uncertainties)
 T o1_upper[3] = { O1[0] + u_O1x, O1[1] + u_O1y, O1[2] + u_O1z };
 T o1_lower[3] = { O1[0] - u_O1x, O1[1] - u_O1y, O1[2] - u_O1z };
 if(o1_upper[0] < o1_lower[0]) std::swap(o1_upper[0], o1_lower[0]);
 if(o1_upper[1] < o1_lower[1]) std::swap(o1_upper[1], o1_lower[1]);
 if(o1_upper[2] < o1_lower[2]) std::swap(o1_upper[2], o1_lower[2]);

 T o2_upper[3] = { O2[0] + u_O2x, O2[1] + u_O2y, O2[2] + u_O2z };
 T o2_lower[3] = { O2[0] - u_O2x, O2[1] - u_O2y, O2[2] - u_O2z };
 if(o2_upper[0] < o2_lower[0]) std::swap(o2_upper[0], o2_lower[0]);
 if(o2_upper[1] < o2_lower[1]) std::swap(o2_upper[1], o2_lower[1]);
 if(o2_upper[2] < o2_lower[2]) std::swap(o2_upper[2], o2_lower[2]);

 // normalize vectors
 T v1[3] = { V1[0], V1[1], V1[2] };
 T v2[3] = { V2[0], V2[1], V2[2] };
 vector3D_normalize(v1);
 vector3D_normalize(v2);

 T v1_upper[3] = { V1[0] + u_V1x, V1[1] + u_V1y, V1[2] + u_V1z };
 T v1_lower[3] = { V1[0] - u_V1x, V1[1] - u_V1y, V1[2] - u_V1z };
 if(v1_upper[0] < v1_lower[0]) std::swap(v1_upper[0], v1_lower[0]);
 if(v1_upper[1] < v1_lower[1]) std::swap(v1_upper[1], v1_lower[1]);
 if(v1_upper[2] < v1_lower[2]) std::swap(v1_upper[2], v1_lower[2]);
 vector3D_normalize(v1_upper);
 vector3D_normalize(v1_lower);

 T v2_upper[3] = { V2[0] + u_V2x, V2[1] + u_V2y, V2[2] + u_V2z };
 T v2_lower[3] = { V2[0] - u_V2x, V2[1] - u_V2y, V2[2] - u_V2z };
 if(v2_upper[0] < v2_lower[0]) std::swap(v2_upper[0], v2_lower[0]);
 if(v2_upper[1] < v2_lower[1]) std::swap(v2_upper[1], v2_lower[1]);
 if(v2_upper[2] < v2_lower[2]) std::swap(v2_upper[2], v2_lower[2]);
 vector3D_normalize(v2_upper);
 vector3D_normalize(v2_lower);

 // cached values (X + uncertainties)
 T dx1 = v1[0] - O1[0];
 T dx1_upper = v1_upper[0] - o1_upper[0];
 T dx1_lower = v1_lower[0] - o1_lower[0];
 if(dx1_upper < dx1_lower) std::swap(dx1_upper, dx1_lower);

 T dx2 = v2[0] - O2[0];
 T dx2_upper = v2_upper[0] - o2_upper[0];
 T dx2_lower = v2_lower[0] - o2_lower[0];
 if(dx2_upper < dx2_lower) std::swap(dx2_upper, dx2_lower);

 // cached values (Y + uncertainties)
 T dy1 = v1[1] - O1[1];
 T dy1_upper = v1_upper[1] - o1_upper[1];
 T dy1_lower = v1_lower[1] - o1_lower[1];
 if(dy1_upper < dy1_lower) std::swap(dy1_upper, dy1_lower);

 T dy2 = v2[1] - O2[1];
 T dy2_upper = v2_upper[1] - o2_upper[1];
 T dy2_lower = v2_lower[1] - o2_lower[1];
 if(dy2_upper < dy2_lower) std::swap(dy2_upper, dy2_lower);

 // cached values (denominator + uncertainties)
 T part1 = dx2*dy1;
 T part1_upper = dx2_upper*dy1_upper;
 T part1_lower = dx2_lower*dy1_lower;
 if(part1_upper < part1_lower) std::swap(part1_upper, part1_lower);

 T part2 = dx1*dy2;
 T part2_upper = dx1_upper*dy2_upper;
 T part2_lower = dx1_lower*dy2_lower;
 if(part2_upper < part2_lower) std::swap(part2_upper, part2_lower);

 // test denominator
 if(eqtest(part1, part2))
   {
    return false; // special case
   }

 // compute denominator (denominator + uncertainties)
 T denom = part1 - part2;
 T denom_upper = part1_upper - part2_upper;
 T denom_lower = part1_lower - part2_lower;

 T cx = O2[0] - O1[0];
 T cx_upper = cx + (u_O2x + u_O1x);
 T cx_lower = cx - (u_O2x + u_O1x);

 T cy = O2[1] - O1[1];
 T cy_upper = cy + (u_O2y + u_O1y);
 T cy_lower = cy - (u_O2y + u_O1y);

 T t1 = (cy*dx2 - cx*dy2)/denom;
 T t1_upper = (cy_upper*dx2_upper - cx_upper*dy2_upper)/denom_upper;
 T t1_lower = (cy_lower*dx2_lower - cx_lower*dy2_lower)/denom_lower;

 T t2 = (cy*dx1 - cx*dy1)/denom;
 T t2_upper = (cy_upper*dx1_upper - cx_upper*dy1_upper)/denom_upper;
 T t2_lower = (cy_lower*dx1_lower - cx_lower*dy1_lower)/denom_lower;

 // cached values (Z + uncertainties)
 T dz1 = v1[2] - O1[2];
 T dz1_upper = v1_upper[2] - o1_upper[2];
 T dz1_lower = v1_lower[2] - o1_lower[2];

 T dz2 = v2[2] - O2[2];
 T dz2_upper = v2_upper[2] - o2_upper[2];
 T dz2_lower = v2_lower[2] - o2_lower[2];

 // check z
 T z1 = O1[2] + dz1*t1;
 T z1_upper = o1_upper[2] + dz1_upper*t1_upper;
 T z1_lower = o1_lower[2] + dz1_lower*t1_lower;

 T z2 = O2[2] + dz2*t2;
 T z2_upper = o2_upper[2] + dz2_upper*t2_upper;
 T z2_lower = o2_lower[2] + dz2_lower*t2_lower;

 cout << "denom = " << denom << endl;
 cout << "z1 = " << z1 << endl;
 cout << "z2 = " << z2 << endl;
 cout << "error = " << abs(z1 - z2) << endl;

 // equals test
 //if(!eqtest(z1, z2)) return false;

 upper_lower_boundary_equality_test<T> test;
 if(!test(z1, z2, z1_upper, z1_lower, z2_upper, z2_lower)) return false;

 // success, close enough
 X[0] = O1[0] + v1[0]*t1;
 X[1] = O1[1] + v1[1]*t1;
 X[2] = O1[2] + v1[2]*t1;
 cout << "X[0] = " << X[0] << endl;
 cout << "X[1] = " << X[1] << endl;
 cout << "X[2] = " << X[2] << endl;
 return true;
}

template<class T>
inline bool ray3D_intersect(T* X, const T* O1, const T* V1, const T* O2, const T* V2)
{
 return ray3D_intersect(X, O1, V1, O2, V2, absolute_error_equality_test<T>());
}

template<class T>
inline bool ray3D_plane_intersect(T* X, const T* O, const T* V, const T* P)
{
 T denom = vector3D_scalar_product(P, V);
 if(std::abs(denom) == constants<T>::zero()) return false;
 ray3D_traverse(X, O, V, -(vector3D_scalar_product(P, O) + P[3])/denom);
 return true;
}

template<class T>
inline T ray3D_plane_intersect_distance(T* X, const T* O, const T* V, const T* P)
{
 T denom = vector3D_scalar_product(P, V);
 if(std::abs(denom) == constants<T>::zero()) return binary_traits<T>::infinity();
 T numer = vector3D_scalar_product(P, O) + P[3];
 T distance = -numer/denom;
 ray3D_traverse(X, O, V, distance);
 return distance;
}

}}

//
//
//
namespace cs { namespace math {

}}

#endif
