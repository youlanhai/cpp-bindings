//////////////////////////////////////////////////////////////////////
/// Desc  ls-config
/// Time  2022/03/09
/// Author youlanhai
/// Home   https://github.com/youlanhai/cpp-bindings
//////////////////////////////////////////////////////////////////////
#pragma once

#define NS_LS_BEGIN namespace lslua{
#define NS_LS_END   }
#define NS_LS       lslua::
#define USING_NS_LS using namespace lslua;

#ifdef LS_BUILD_AS_DLL
    #ifdef _WIN32
        #ifdef LS_API_EXPORT
            #define LS_API __declspec(dllexport)
        #else
            #define LS_API __declspec(dllimport)
        #endif
    #else
        #define LS_API __attribute__((visibility("default")))
    #endif
#else
    #define LS_API
#endif

#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__) ) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
#   define LS_64BIT 1
#else
#   define LS_64BIT 0
#endif

#define DISABLE_COPY_AND_ASSIGN(TYPE) \
    const TYPE& operator = (const TYPE &) = delete; \
    TYPE(const TYPE &) = delete

#ifndef ENABLE_TEST
    #if defined(DEBUG) || defined(_DEBUG)
        #define ENABLE_TEST
    #endif
#endif
