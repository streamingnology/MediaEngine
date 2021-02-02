/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <map>
#include <string>
#include <vector>
#include "core/snyconstants.h"
#include "core/snydatabuffer.h"
#include "core/snytype.h"

namespace sny {
namespace SnyUtils {
  SnyUI64 convertTime(SnyUI64 time_value, SnyUI64 from_time_scale, SnyUI64 to_time_scale);
  std::string formatstring(const char *format, ...);
  std::string formatstring(const char *fmt, va_list vl);
}  // namespace SnyUtils
}  // namespace sny
