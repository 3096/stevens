#ifndef __CS_CONFIG_H
#define __CS_CONFIG_H

// Architecture Macros
#if _WIN32 || _WIN64
    #if _WIN64
    #define __SYS_X64
    #else
    #define __SYS_X86
    #endif
#elif __GNUC__
   #if __x86_64__ || __ppc64__
   #define __SYS_X64
   #else
   #define __SYS_X86
   #endif
#else
   #error "Unsupported compilation system."
#endif

// System Macros
#define __SYS_LITTLE_ENDIAN 0
#define __SYS_BIG_ENDIAN    1
#define __SYS_ENDIAN        __SYS_LITTLE_ENDIAN

// C++ Support Macros
#define __CPP_NO_CONSTANT_EXPRESSIONS
#define __CPP_NO_DELETED_DEFINITIONS
#undef  __CPP_NO_MOVE_CONSTRUCTORS

#endif
