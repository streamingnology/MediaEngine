/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snyresults.h"
namespace sny {
const char *SnyResult2Text(int result) {
  switch (result) {
    case SnySuccess:
      return "SUCCESS";
    case SnyFailture:
      return "FAILURE";
    default:
      return "UNKNOWN";
  }
}
}  // namespace sny