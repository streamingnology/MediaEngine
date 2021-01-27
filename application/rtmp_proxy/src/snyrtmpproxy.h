/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <core/snyresults.h>
#include <source/rtmp/snyrtmpserver.h>
#include "snymultirtmppublish.h"
#include "snyrtmpproxyconf.h"
namespace app {
class SnyRTMPProxy : public sny::SnySourceCallback {
 public:
  SnyRTMPProxy();
  ~SnyRTMPProxy() override;

  void setConfigure(std::shared_ptr<SnyRTMPProxyConf> cnf) { cnf_ = std::move(cnf); }

  sny::SnyResult start();
  void stop();

 public:
  void OnRTMPNewConnectCallback(const std::weak_ptr<uv::TcpConnection>& conn, std::string& conn_name);
  void OnRTMPConnectCloseCallback(std::string& conn_name);
  void OnRTMPSendSampleCallback(std::string& conn_name, std::shared_ptr<sny::SnyMediaSample> sample);

  void onRtmpAppStreamName(std::string conn_name, std::string app_name, std::string stream_name) override;
  void onTrack(std::string conn_name, std::map<int32_t, std::shared_ptr<MediaTrack>> tracks) override;
  void onSample(std::string conn_name, std::shared_ptr<sny::SnyMediaSample> sample) override;

 private:
  std::mutex mutex_;
  std::shared_ptr<sny::SnyRTMPServer> sny_rtmp_server_;
  std::map<std::string, std::shared_ptr<SnyMultiRTMPPublish>> sny_multi_rtmp_publish_;
  std::shared_ptr<SnyRTMPProxyConf> cnf_;
};
}  // namespace app