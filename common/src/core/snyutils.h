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
  std::string formatstring(const char* format, ...);
  std::vector<std::string> splitUri(std::string uri);
  SnyDataBuffer listDirectoryContent(std::string path);
  std::map<int, std::vector<std::string>> getDirectoryContent(std::string path);
  std::vector<std::string> getIpAddress();
  std::string getCurrentAppDirectory();
  SnyServiceType getServiceTypeByPath(const std::string path);
  SnyFileDir isFileorDir(std::string& path);
}  // namespace SnyUtils
}  // namespace sny
