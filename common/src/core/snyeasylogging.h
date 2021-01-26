/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#define ELPP_AS_DLL          // Tells Easylogging++ that it's used for DLL
#define ELPP_EXPORT_SYMBOLS  // Tells Easylogging++ to export symbols
#include <easylogging++.h>
/*
#if defined(_MSC_VER)
//  Microsoft
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif
*/

// EXPORT el::base::type::StoragePointer sharedLoggingRepository();