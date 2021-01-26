/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <stdio.h>
#include "core/snyplatform.h"

#define SNY_PLATFORM_BYTE_ORDER_BIG_ENDIAN 0
#define SNY_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN 1

#if !defined(SNY_PLATFORM_BYTE_ORDER)
#if defined(__ppc__)
#define SNY_PLATFORM_BYTE_ORDER SNY_PLATFORM_BYTE_ORDER_BIG_ENDIAN
#elif defined(_MSC_VER)
#if defined(_M_IX86) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)
#define SNY_PLATFORM_BYTE_ORDER SNY_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN
#endif
#elif defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__arm64__)
#define SNY_PLATFORM_BYTE_ORDER SNY_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN
#endif
#endif

/* Microsoft Platforms */
#if defined(_MSC_VER)
#define SNY_CONFIG_INT64_TYPE __int64
#if (_MSC_VER >= 1400) && !defined(_WIN32_WCE)
#define SNY_CONFIG_HAVE_FOPEN_S
#define SNY_snprintf(s, c, f, ...) _snprintf_s(s, c, _TRUNCATE, f, __VA_ARGS__)
#define SNY_vsnprintf(s, c, f, a) _vsnprintf_s(s, c, _TRUNCATE, f, a)
#define SNY_fopen fopen
#define SNY_fseek _fseeki64
#define SNY_ftell _ftelli64
#define SNY_fileno _fileno
#else
#define SNY_snprintf _snprintf
#define SNY_vsnprintf _vsnprintf
#endif
#if defined(_WIN32_WCE)
#define SNY_fseek fseek
#define SNY_ftell ftell
#endif
#if defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#endif
#endif

/* Cygwin */
#if defined(Q_OS_CYGWIN)
#define SNY_fopen fopen
#define SNY_fseek fseek
#define SNY_ftell ftell
#define SNY_fileno fileno
#endif

/* Android */
#if defined(ANDROID)
#if !defined(SNY_CONFIG_NO_RTTI)
#define SNY_CONFIG_NO_RTTI
#endif
#if !defined(SNY_CONFIG_NO_EXCEPTIONS)
#define SNY_CONFIG_NO_EXCEPTIONS
#endif
#endif

#if defined(Q_OS_ANDROID)
#define SNY_fopen fopen
#define SNY_ftell ftell
#define SNY_fseek fseeko
#define SNY_fileno fileno
#endif

#if defined(Q_OS_MACX)
#define SNY_fopen fopen
#define SNY_ftell ftello
#define SNY_fseek fseeko
#define SNY_fileno fileno
#endif

/* some compilers (ex: MSVC 8) deprecate those, so we rename them */
#if !defined(SNY_snprintf)
#define SNY_snprintf snprintf
#endif
#if !defined(SNY_vsnprintf)
#define SNY_vsnprintf vsnprintf
#endif
