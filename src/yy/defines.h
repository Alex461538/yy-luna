#if !defined(__hpp_yy_defines__)
#define __hpp_yy_defines__

// Detect OS

#if defined(__unix__) || defined(__unix)
#define YY_OSAPI_UNIX
#endif

#if defined(__linux__)
#define YY_OSAPI_LINUX
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#define YY_OSAPI_WIN32
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define YY_OSAPI_DARWIN
#endif

// Detect architecture

#if defined(__x86_64__) || defined(_M_X64)
#define YY_ISA "x86_64";
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define YY_ISA "x86_32";
#elif defined(__ARM_ARCH_2__)
#define YY_ISA "ARMv2";
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
#define YY_ISA "ARMv3";
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
#define YY_ISA "ARMv4T";
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
#define YY_ISA "ARMv5"
#elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
#define YY_ISA "ARM6vT2";
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
#define YY_ISA "ARMv6";
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#define YY_ISA "ARMv7";
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#define YY_ISA "ARMv7A";
#elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#define YY_ISA "ARMv7R";
#elif defined(__ARM_ARCH_7M__)
#define YY_ISA "ARMv7M";
#elif defined(__ARM_ARCH_7S__)
#define YY_ISA "ARMv7S";
#elif defined(__aarch64__) || defined(_M_ARM64)
#define YY_ISA "ARM64";
#elif defined(mips) || defined(__mips__) || defined(__mips)
#define YY_ISA "MIPS";
#elif defined(__sh__)
#define YY_ISA "SUPERH";
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
#define YY_ISA "POWERPC";
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
#define YY_ISA "POWERPC64";
#elif defined(__sparc__) || defined(__sparc)
#define YY_ISA "SPARC";
#elif defined(__m68k__)
#define YY_ISA "M68K";
#else
#define YY_ISA "UNKNOWN";
#endif

#endif