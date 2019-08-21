#ifndef __CS_CORE_CHOP_H
#define __CS_CORE_CHOP_H

#include<cs/core/types.h>
#include<cs/core/binary_traits.h>

namespace cs {

inline binary32 chop(binary32 v, binary32 ep = binary_traits<binary32>::epsilon())
{
 if(std::abs(v) < binary_traits<binary32>::epsilon()) return 0;
 return v;
}

inline binary64 chop(binary64 v, binary64 ep = binary_traits<binary64>::epsilon())
{
 if(std::abs(v) < binary_traits<binary64>::epsilon()) return 0;
 return v;
}

inline std::complex<binary32> chop(const std::complex<binary32>& v, binary32 ep = binary_traits<binary32>::epsilon())
{
 binary32 zero = static_cast<binary32>(0);
 if(std::abs(v) < binary_traits<binary32>::epsilon()) return std::complex<binary32>(zero, zero);
 return v;
}

inline std::complex<binary64> chop(const std::complex<binary64>& v, binary64 ep = binary_traits<binary64>::epsilon())
{
 binary64 zero = static_cast<binary64>(0);
 if(std::abs(v) < binary_traits<binary64>::epsilon()) return std::complex<binary64>(zero, zero);
 return v;
}

}

#endif
