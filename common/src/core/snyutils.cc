/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */

#include "core/snyutils.h"
#include "core/snyplatform.h"
#include "core/snytype.h"

namespace sny {
namespace SnyUtils {
  SnyUI64 convertTime(SnyUI64 time_value, SnyUI64 from_time_scale, SnyUI64 to_time_scale) {
    if (from_time_scale == 0) return 0;
    double ratio = (double)to_time_scale / (double)from_time_scale;
    return ((SnyUI64)(0.5 + (double)time_value * ratio));
  }
}  // namespace SnyUtils
}  // namespace sny