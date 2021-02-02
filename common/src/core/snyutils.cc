/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */

#include "core/snyutils.h"
#include "core/snyplatform.h"
#include "core/snytype.h"
#include "snyplatform.h"
#include <memory>
#include <cstdarg>

namespace sny {
namespace SnyUtils {
  SnyUI64 convertTime(SnyUI64 time_value, SnyUI64 from_time_scale, SnyUI64 to_time_scale) {
    if (from_time_scale == 0) return 0;
    double ratio = (double)to_time_scale / (double)from_time_scale;
    return ((SnyUI64)(0.5 + (double)time_value * ratio));
  }

  std::string formatstring(const char* format, ...) {
    va_list list;
    va_start(list, format);
    std::string s = formatstring(format, &(list[0]));
    va_end(list);
    return s;
  }

  std::string formatstring(const char* fmt, va_list vl) {
    va_list list;
    va_copy(list, vl);
    const int len = std::vsnprintf(nullptr, 0, fmt, vl);
    std::vector<char> buffer(len + 1);
    va_copy(vl, &(list[0]));
    std::vsnprintf(buffer.data(), buffer.size(), fmt, vl);
    va_end(list);

    return std::string(buffer.data(), len);
  }
}  // namespace SnyUtils
}  // namespace sny