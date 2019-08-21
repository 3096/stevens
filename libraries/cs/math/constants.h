#ifndef __CS_MATH_CONSTANTS_H
#define __CS_MATH_CONSTANTS_H

#include<cs/core/types.h>
#include<cs/core/binary_traits.h>

namespace cs { namespace math {

template<class T> struct constants;
template<> struct constants<binary32>;
template<> struct constants<binary64>;

template<class T>
struct constants {
 static const bool is_specialized = false;
};

template<>
struct constants<binary32> {
 // specialized
 static const bool is_specialized = true;
 // simple
 static binary32 zero(void) { return  0.0f; }
 static binary32 half(void) { return  0.5f; }
 static binary32 one(void) { return 1.0f; }
 static binary32 two(void) { return 2.0f; }
 static binary32 neg_one(void) { return -1.0f; }
 static binary32 neg_two(void) { return -2.0f; }
 // simple fractions
 static binary32 one_over_2(void) { uint32 x = 0x3F000000ul; return binary_traits<binary32>::reinterpret(x); }
 static binary32 one_over_3(void) { uint32 x = 0x3EAAAAABul; return binary_traits<binary32>::reinterpret(x); }
 static binary32 one_over_4(void) { uint32 x = 0x3E800000ul; return binary_traits<binary32>::reinterpret(x); }
 static binary32 one_over_5(void) { uint32 x = 0x3E4CCCCDul; return binary_traits<binary32>::reinterpret(x); }
 static binary32 one_over_6(void) { uint32 x = 0x3E2AAAABul; return binary_traits<binary32>::reinterpret(x); }
 static binary32 one_over_7(void) { uint32 x = 0x3E124925ul; return binary_traits<binary32>::reinterpret(x); }
 static binary32 one_over_8(void) { uint32 x = 0x3E000000ul; return binary_traits<binary32>::reinterpret(x); }
 static binary32 one_over_9(void) { uint32 x = 0x3DE38E39ul; return binary_traits<binary32>::reinterpret(x); }
 // trigonometry
 static binary32 pi(void) { uint32 x = 0x40490FDBul; return binary_traits<binary32>::reinterpret(x); } // A000796
 static binary32 pi_times_2(void) { return 6.28318530718f; } // A019692
 static binary32 pi_over_2(void) { return 1.57079632679f; } // A019669
 static binary32 pi_over_3(void) { return 1.04719755120f; } // A019670
 static binary32 pi_over_4(void) { return 0.78539816340f; } // A003881
 static binary32 pi_over_180(void) { return 0.01745329252f; } // A019685
 static binary32 pi_under_180(void) { return 57.2957795131f; } // A072097
 // square root
 static binary32 sqrt_of_pi(void) { return 1.77245385091f; } // A002161
 static binary32 sqrt_of_2(void) { return 1.41421356237f; } // A002193
 static binary32 sqrt_of_3(void) { return 1.73205080757f; } // A002194
 static binary32 sqrt_of_5(void) { return 2.23606797750f; } // A002163
 static binary32 golden_ratio(void) { return 1.61803398875f; } // A001622
 static binary32 silver_ratio(void) { return 0.61803398875f; } // A094214
 // logarithmic
 static binary32 e(void) { return 2.71828182846f; } // A001113
 static binary32 ln_of_2(void) { return 0.69314718056f; } // A002162
 static binary32 ln_of_3(void) { return 1.09861228867f; } // A002391
 static binary32 ln_of_10(void) { return 2.30258509299f; } // A002392
 static binary32 lb_of_e(void) { return 1.44269504089f; } // A007525
 static binary32 lb_of_10(void) { return 3.32192809489f; } // A020862
 static binary32 lg_of_2(void) { return 0.30102999566f; } // A007524
 static binary32 lg_of_e(void) { return 0.43429448190f; } // A002285
};

template<>
struct constants<binary64> {
 // specialized
 static const bool is_specialized = true;
 // simple
 static binary64 zero(void) { return  0.0; }
 static binary64 half(void) { return  0.5; }
 static binary64 one(void) { return  1.0; }
 static binary64 two(void) { return  2.0; }
 static binary64 neg_one(void) { return -1.0; }
 static binary64 neg_two(void) { return -2.0; }
 // simple fractions
 static binary64 one_over_2(void) { return 1.0/2.0; }
 static binary64 one_over_3(void) { return 1.0/3.0; }
 static binary64 one_over_4(void) { return 1.0/4.0; }
 static binary64 one_over_5(void) { return 1.0/5.0; }
 static binary64 one_over_6(void) { return 1.0/6.0; }
 static binary64 one_over_7(void) { return 1.0/7.0; }
 static binary64 one_over_8(void) { return 1.0/8.0; }
 static binary64 one_over_9(void) { return 1.0/9.0; }
 // trigonometry
 static binary64 pi(void) { uint64 x = 0x400921FB54442D18ull; return binary_traits<binary64>::reinterpret(x); } // A000796
 static binary64 pi_times_2(void) { return 6.28318530717958647692528676656; } // A019692
 static binary64 pi_over_2(void) { return 1.57079632679489661923132169164; } // A019669
 static binary64 pi_over_3(void) { return 1.04719755119659774615421446109; } // A019670
 static binary64 pi_over_4(void) { return 0.78539816339744830961566084582; } // A003881
 static binary64 pi_over_180(void) { return 0.01745329251994329576923690768; } // A019685
 static binary64 pi_under_180(void) { return 57.2957795130823208767981548141; } // A072097
 // square root
 static binary64 sqrt_of_pi(void) { return 1.77245385090551602729816748334; } // A002161
 static binary64 sqrt_of_2(void) { return 1.41421356237309504880168872421; } // A002193
 static binary64 sqrt_of_3(void) { return 1.73205080756887729352744634151; } // A002194
 static binary64 sqrt_of_5(void) { return 2.23606797749978969640917366873; } // A002163
 static binary64 golden_ratio(void) { return 1.61803398874989484820458683437; } // A001622
 static binary64 silver_ratio(void) { return 0.61803398874989484820458683437; } // A094214
 // logarithmic
 static binary64 e(void) { return 2.71828182845904523536028747135; } // A001113
 static binary64 ln_of_2(void) { return 0.69314718055994530941723212146; } // A002162
 static binary64 ln_of_3(void) { return 1.09861228866810969139524523692; } // A002391
 static binary64 ln_of_10(void) { return 2.30258509299404568401799145468; } // A002392
 static binary64 lb_of_e(void) { return 1.44269504088896340735992468100; } // A007525
 static binary64 lb_of_10(void) { return 3.32192809488736234787031942949; } // A020862
 static binary64 lg_of_2(void) { return 0.30102999566398119521373889472; } // A007524
 static binary64 lg_of_e(void) { return 0.43429448190325182765112891892; } // A002285
};

}}

#endif
