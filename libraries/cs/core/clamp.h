#ifndef __CS_CORE_CLAMP_H
#define __CS_CORE_CLAMP_H

namespace cs {

template<class T>
inline T clamp(const T& v, const T& a, const T& b)
{
 if(a < b) {
    if(v < a) return a;
    if(b < v) return b;
   }
 else {
    if(v < b) return b;
    if(a < v) return a;
   }
 return v;
}

template<class T, class BinaryPredicate>
inline T clamp(const T& v, const T& a, const T& b, BinaryPredicate p)
{
 if(p(a, b)) {
    if(p(v, a)) return a;
    if(p(b, v)) return b;
   }
 else {
    if(p(v, b)) return b;
    if(p(a, v)) return a;
   }
 return v;
}

template<class T>
inline T clamp(const T& v, const T& a, const T& b, const T& ra, const T& rb)
{
 if(a < b) {
    if(v < a) return ra;
    if(b < v) return rb;
   }
 else {
    if(v < b) return rb;
    if(a < v) return ra;
   }
 return v;
}

template<class T, class BinaryPredicate>
inline T clamp(const T& v, const T& a, const T& b, const T& ra, const T& rb, BinaryPredicate p)
{
 if(p(a, b)) {
    if(p(v, a)) return ra;
    if(p(b, v)) return rb;
   }
 else {
    if(p(v, b)) return rb;
    if(p(a, v)) return ra;
   }
 return v;
}

}

#endif
