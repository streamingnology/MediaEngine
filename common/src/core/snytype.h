/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYTYPE_H
#define STREAMINGNOLOGY_CORE_SNYTYPE_H
#include <cinttypes>
namespace sny {
typedef int SnyInt;
typedef int SnyResult;
typedef bool SnyBool;
typedef long long SnySI64;
typedef unsigned long long SnyUI64;
typedef unsigned int SnyUI32;
typedef signed int SnySI32;
typedef unsigned short SnyUI16;
typedef signed short SnySI16;
typedef unsigned char SnyUI08;

#define SnySI64_MAX INT64_MAX
}  // namespace sny

#endif  // !STREAMINGNOLOGY_CORE_SNYTYPE_H
