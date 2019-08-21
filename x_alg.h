#ifndef __XENTAX_ALG_H
#define __XENTAX_ALG_H

template<class T>
bool minimum(const T* data, size_t elem, T& item)
{
 if(!data) return false;
 if(!elem) return false;
 item = data[0];
 for(size_t i = 1; i < elem; i++) if(data[i] < item) item = data[i];
 return true;
}

template<class T>
bool maximum(const T* data, size_t elem, T& item)
{
 if(!data) return false;
 if(!elem) return false;
 item = data[0];
 for(size_t i = 1; i < elem; i++) if(item < data[i]) item = data[i];
 return true;
}

//
// FUNCTION NAME
//  tristrip_to_trilist
// DESCRIPTION
//  Converts a triangle strip to a triangle list. For example, this function
//  will convert [0, 1, 2, 3, 4] to [0, 1, 2, 1, 3, 2, 2, 3, 4]. The number of
//  elements in the return value will always be divisible by three.
// ARGUMENTS
//  r0: Random access iterator to beginning of triangle strip indices.
//  r1: Random access iterator to end of triangle strip indices
//  clean: Boolean to indicate whether or not to remove degenerate triangles.
// RETURN VALUE
//  Returns an std::vector of indices in the format of a triangle list.
//
template<class _RanIt>
std::vector<typename std::iterator_traits<_RanIt>::value_type> tristrip_to_trilist(_RanIt r0, _RanIt r1, bool clean = true)
{
 typedef std::iterator_traits<_RanIt> traits_type;
 typedef typename std::iterator_traits<_RanIt>::difference_type difference_type;
 typedef typename std::iterator_traits<_RanIt>::value_type value_type;

 // compute number of indices
 difference_type n_indices = r1 - r0;
 if(n_indices < 0) return std::vector<value_type>();

 // clean degenerate triangles
 if(clean)
   {
    // variables
    std::vector<value_type> result;
    difference_type n_triangles = 0;

    // check first triangle
    value_type a = r0[0];
    value_type b = r0[1];
    value_type c = r0[2];
    if((a != b) && (a != c) && (b != c)) n_triangles++;

    // check other triangles
    // note that only comparisons with 'c' are necessary
    for(difference_type i = 3; i < n_indices; i++) {
        a = b;
        b = c;
        c = r0[i];
        if((a != c) && (b != c)) n_triangles++;
       }

    // nothing to do
    if(n_triangles < 1) return result;

    // create vector
    difference_type n_output_indices = 3 * n_triangles;
    result.resize(n_output_indices);

    // set first triangle
    difference_type index = 0;
    value_type a = r0[0];
    value_type b = r0[1];
    value_type c = r0[2];
    if((a != b) && (a != c) && (b != c)) {
       result[index++] = a;
       result[index++] = b;
       result[index++] = c;
      }
    
    // set other triangles
    for(difference_type i = 3; i < n_indices; i++) {
        a = b;
        b = c;
        c = r0[i];
        if((a != c) && (b != c)) {
           if(i % 2) {
              result[index++] = a;
              result[index++] = c;
              result[index++] = b;
             }
           else {
              result[index++] = a;
              result[index++] = b;
              result[index++] = c;
             }
          }
       }

    // return result
    return result;
   }
 // leave degenerate triangles
 else
   {
    // compute number of triangles
    difference_type n_triangles = n_indices - 2;
    if(n_triangles < 1) return std::vector<value_type>();

    // create vector
    difference_type n_output_indices = 3 * n_triangles;
    result.resize(n_output_indices);
    
    // set first triangle
    difference_type index = 0;
    value_type a = r0[0];
    value_type b = r0[1];
    value_type c = r0[2];
    result[index++] = a;
    result[index++] = b;
    result[index++] = c;
    
    // set other triangles
    for(difference_type i = 3; i < n_indices; i++) {
        a = b;
        b = c;
        c = r0[i];
        if(i % 2) {
           result[index++] = a;
           result[index++] = c;
           result[index++] = b;
          }
        else {
           result[index++] = a;
           result[index++] = b;
           result[index++] = c;
          }
       }

    // return result
    return result;
   }

 // unreachable code
 return std::vector<value_type>();
}

template<class _RanIt>
std::vector<typename std::iterator_traits<_RanIt>::value_type> stripcut_to_trilist(
 _RanIt r0, _RanIt r1,
 typename std::iterator_traits<_RanIt>::value_type cutvalue,
 bool clean = true)
{
 // unreachable code
 return std::vector<value_type>();
}

#endif
