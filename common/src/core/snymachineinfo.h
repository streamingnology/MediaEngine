/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <string>
#include <vector>
#include "core/snydatabuffer.h"
#include "core/snytype.h"

namespace sny {

class SnyUtsName {
 public:
  SnyUtsName() {}
  ~SnyUtsName() {}
  SnyUtsName(const SnyUtsName &object);
  void operator=(const SnyUtsName &object);

 public:
  std::string sysname_;
  std::string nodename_;
  std::string release_;
  std::string version_;
  std::string machine_;
};

class SnyMachineInfo {
 public:
  SnyMachineInfo(SnyUI16 lp);
  ~SnyMachineInfo() {}
  void setAppVersion(std::string app_version);  // AppName-1.0.1-Aug.19.2018-git
  std::vector<std::string> getIpAddress();
  SnyUtsName getUtsName();
  SnyDataBuffer getMachineInfoInJSON();

 public:
  std::string app_version_;
  SnyUI16 listening_port_;
};

}  // namespace sny
