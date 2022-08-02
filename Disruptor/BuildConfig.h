#pragma once

#include <boost/config.hpp>

// detect current compiler
#if defined(BOOST_MSVC)
# define DISRUPTOR_VC_COMPILER
#elif defined(__GNUC__)
# define DISRUPTOR_GNUC_COMPILER
#endif // defined(BOOST_MSVC)

// detect environment
#if defined(DISRUPTOR_VC_COMPILER)

# define DISRUPTOR_VC_COMPILER_VERSION_2013 1800
# define DISRUPTOR_VC_COMPILER_VERSION_2015 1900

// we need at least Visual Studio 2013 C++ compiler to support C++11
# if _MSC_VER >= DISRUPTOR_VC_COMPILER_VERSION_2013
#  define DISRUPTOR_CPP_11
# endif

# if defined(_WIN64)
#  define DISRUPTOR_CPU_64

# else

#  define DISRUPTOR_CPU_32
# endif

#elif defined(DISRUPTOR_GNUC_COMPILER)

# define DISRUPTOR_CPP_11

# if __arm__
#  define DISRUPTOR_CPU_ARM
#  define DISRUPTOR_CPU_ARM_32
# #elif __aarch64__
#  define DISRUPTOR_CPU_ARM
#  define DISRUPTOR_CPU_ARM_64
# elif __x86_64__ || __ppc64__
#  define DISRUPTOR_CPU_64
# else
#  define DISRUPTOR_CPU_32
# endif

#endif

// calling conventions
#ifdef DISRUPTOR_VC_COMPILER
# define DISRUPTOR_STDCALL        __stdcall
# define DISRUPTOR_CDECL          __cdecl
# define DISRUPTOR_FASTCALL       __fastcall
#elif defined DISRUPTOR_GNUC_COMPILER
# define DISRUPTOR_STDCALL        __attribute((stdcall))
# define DISRUPTOR_CDECL          /* */
# define DISRUPTOR_FASTCALL       __attribute((fastcall))
#endif

// struct alignment
#ifdef DISRUPTOR_VC_COMPILER
# define DISRUPTOR_ALIGN(x)       __declspec(align(x))
#elif defined DISRUPTOR_GNUC_COMPILER
# define DISRUPTOR_ALIGN(x)       __attribute(aligned(x))
#endif

#ifdef WIN32
# define DISRUPTOR_OS_FAMILY_WINDOWS
#else
# define DISRUPTOR_OS_FAMILY_UNIX
# ifdef __APPLE__
#  define DISRUPTOR_OS_FAMILY_MACOS
# else
#  define DISRUPTOR_OS_FAMILY_LINUX
# endif
#endif

#ifdef _DEBUG
# define DISRUPTOR_BUILD_CONFIGURATION "Debug"
#else
# define DISRUPTOR_BUILD_CONFIGURATION "Release"
#endif

// winnt.h already defines these
#ifndef _WINNT_
# define UNREFERENCED_PARAMETER(P)          (P)
# define DBG_UNREFERENCED_PARAMETER(P)      (P)
# define DBG_UNREFERENCED_LOCAL_VARIABLE(V) (V)
#endif

// detect C++17
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
  # define DISRUPTOR_CPP_17
#endif
