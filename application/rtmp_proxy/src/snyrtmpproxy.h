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
  SnyRTMPProxy(SnyRTMPProxy&) = delete;
  virtual ~SnyRTMPProxy();

  SnyRTMPProxy& operator=(SnyRTMPProxy) = delete;

  void setConfigure(std::shared_ptr<SnyRTMPProxyConf> cnf) { cnf_ = std::move(cnf); }

  sny::SnyResult start();
  void stop();

 public:
  void OnRTMPNewConnectCallback(const std::weak_ptr<uv::TcpConnection>& conn, const std::string& conn_name);
  void OnRTMPConnectCloseCallback(const std::string& conn_name);
  void OnRTMPSendSampleCallback(const std::string& conn_name, const std::shared_ptr<sny::SnyMediaSample> sample);

  void onRtmpAppStreamName(const std::string& conn_name, const std::string& app_name,
                           const std::string& stream_name) override;
  void onTrack(const std::string& conn_name, const std::map<int32_t, std::shared_ptr<MediaTrack>>& tracks) override;
  void onSample(const std::string& conn_name, const std::shared_ptr<sny::SnyMediaSample> sample) override;

 private:
  std::mutex mutex_;
  std::shared_ptr<sny::SnyRTMPServer> sny_rtmp_server_;
  std::map<std::string, std::shared_ptr<SnyMultiRTMPPublish>> sny_multi_rtmp_publish_;
  std::shared_ptr<SnyRTMPProxyConf> cnf_;
};
}  // namespace app