#ifndef __CS_CORE_BYTE_ORDER_H
#define __CS_CORE_BYTE_ORDER_H

// STL Headers
#include<algorithm>

namespace cs {

template<class T>
inline void reverse_byte_order(T* data)
{
 unsigned char* ptr = reinterpret_cast<unsigned char*>(data);
 std::reverse(ptr, ptr + sizeof(T)); 
}

template<class T>
inline void reverse_byte_order(T* data, size_t elem)
{
 for(size_t i = 0; i < elem; i++) {
     unsigned char* ptr = reinterpret_cast<unsigned char*>(&data[i]);
     std::reverse(ptr, ptr + sizeof(T));
    }
}

} // namespace cs

#endif
