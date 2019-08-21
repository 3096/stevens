#ifndef __CS_MATH_MATRIX_SOLVE_H
#define __CS_MATH_MATRIX_SOLVE_H

#include<cs/math/functions.h>
#include<cs/math/error.h>

namespace cs { namespace math { namespace matrix {

template<class T>
void residuals_2x2(const T* A, const T* b, const T* x, T* r)
{
 r[0] = A[0]*x[0] + A[1]*x[1] - b[0];
 r[1] = A[2]*x[0] + A[3]*x[1] - b[1];
}

template<class T, class ZeroEpsilonTest>
bool solve_GE_2x2(const T* A, const T* b, T* x, ZeroEpsilonTest test)
{
 // pivot
 if(abs(A[0]) < abs(A[2])) {
    if(test(A[2])) return false;
    T m1 = A[0]/A[2];
    T m2 = (A[1] - A[3]*m1);
    if(test(m2)) return false;
    x[1] = (b[0] - b[1]*m1)/m2;
    x[0] = (b[1] - A[3]*x[1])/A[2];
   }
 // don't pivot
 else {
    if(test(A[0])) return false;
    T m1 = A[2] / A[0];
    T m2 = A[3] - A[1]*m1;
    if(test(m2)) return false;
    x[1] = (b[1] - m1*b[0])/m2;
    x[0] = (b[0] - A[1]*x[1])/A[0];
   }
 return true;
}

template<class T>
bool solve_GE_2x2(const T* A, const T* b, T* x)
{
 return solve_GE_2x2(A, b, x, zero_epsilon_test<T>());
}

template<class T, class ZeroEpsilonTest>
T solve_2x2(const T* A, const T* b, T* x, ZeroEpsilonTest test)
{
 T det = sarrus_2x2(A);
 if(test(det)) return det;
 x[0] = (b[0]*A[3] - A[1]*b[1])/det;
 x[1] = (A[0]*b[1] - b[0]*A[2])/det;
 return det;
}

template<class T>
T solve_2x2(const T* A, const T* b, T* x)
{
 return solve_2x2(A, b, x, zero_epsilon_test<T>());
}

template<class T>
T solve_3x3(const T* A, const T* b, T* x)
{
 // compute determinant of 3x3 matrix
 // | m[0] m[1] m[2] | = m[0] * | m[4] m[5] | - m[1] * | m[3] m[5] | + m[2] * | m[3] m[4] |
 // | m[3] m[4] m[5] |          | m[7] m[8] |          | m[6] m[8] |          | m[6] m[7] |
 // | m[6] m[7] m[8] | 

 // X
 // | b[0] m[1] m[2] | = b[0] * | m[4] m[5] | - m[1] * | b[1] m[5] | + m[2] * | b[1] m[4] |
 // | b[1] m[4] m[5] |          | m[7] m[8] |          | b[2] m[8] |          | b[2] m[7] |
 // | b[2] m[7] m[8] | 

 // Y
 // | m[0] b[0] m[2] | = m[0] * | b[1] m[5] | - b[0] * | m[3] m[5] | + m[2] * | m[3] b[1] |
 // | m[3] b[1] m[5] |          | b[2] m[8] |          | m[6] m[8] |          | m[6] b[2] |
 // | m[6] b[2] m[8] |

 // Z
 // | m[0] m[1] b[0] | = m[0] * | m[4] b[1] | - m[1] * | m[3] b[1] | + b[0] * | m[3] m[4] |
 // | m[3] m[4] b[1] |          | m[7] b[2] |          | m[6] b[2] |          | m[6] m[7] |
 // | m[6] m[7] b[2] | 

 // cache determinants
 T D[6] = {
  sarrus_2x2(A[4], A[5], A[7], A[8]), // D[0]
  sarrus_2x2(A[3], A[5], A[6], A[8]), // D[1]
  sarrus_2x2(A[3], A[4], A[6], A[7]), // D[2]
  sarrus_2x2(b[1], A[3], b[2], A[6]), // D[3]
  sarrus_2x2(b[1], A[4], b[2], A[7]), // D[4]
  sarrus_2x2(b[1], A[5], b[2], A[8]), // D[5]
 };

 // compute divisor (using first row)
 T det = A[0]*D[0] - A[1]*D[1] + A[2]*D[2];
 if(det == constants<T>::zero()) return det;

 // solve
 x[0] = (b[0]*D[0] - A[1]*D[5] + A[2]*D[4])/det;
 x[1] = (A[0]*D[5] - b[0]*D[1] - A[2]*D[3])/det;
 x[2] = (A[1]*D[3] - A[0]*D[4] + b[0]*D[2])/det;

 // return determinant
 return det;
}

}}}

#endif
