/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <core/snytype.h>
#include <core/snydatabuffer.h>
namespace app {
class SnyRTMPProxyPublishItem {
 public:
  std::string publish_name_;
  std::string publish_url_;
};

class SnyRTMPProxyStream {
 public:
  std::string app_name_;
  std::string stream_name_;
  std::vector<SnyRTMPProxyPublishItem> publish_items_;
};

class SnyRTMPProxyConf {
 public:
  std::string name;
  sny::SnyInt version;
  sny::SnyUI16 rtmp_port;
  std::vector<SnyRTMPProxyStream> streams_;
};

std::shared_ptr<SnyRTMPProxyConf> parse(sny::SnyDataBuffer& cnf_json);

}
