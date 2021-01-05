/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYRESULTS_H
#define STREAMINGNOLOGY_CORE_SNYRESULTS_H
#include "core/snytype.h"

namespace sny {
const SnyInt SnySuccess = 0;
const SnyInt SnyFailture = -1;

const char* SnyResult2Text(SnyInt result);

#define SNYFAILED(result) ((result) != SnySuccess)
#define SNYSUCCEEDED(result) ((result) == SnySuccess)
}  // namespace sny

#endif  // !STREAMINGNOLOGY_CORE_SNYRESULTS_H
