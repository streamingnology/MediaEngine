/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <core/snythreads.h>
#include <media/mux/snymuxerImplffmpeg.h>
#include <media/rtmp_writer.h>
#include <source/rtmp/rtmp_stream.h>
#include <source/snysourcecallback.h>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "snyrtmpproxyconf.h"
namespace app {
class SnyMultiRTMPPublish : public sny::SnySourceCallback {
 public:
  SnyMultiRTMPPublish(std::string name);
  ~SnyMultiRTMPPublish();

  void start();
  void stop();
  int onThreadProc(int id);

  void onRtmpAppStreamName(std::string conn_name, std::string app_name, std::string stream_name) override;
  void onTrack(std::string conn_name, std::map<int32_t, std::shared_ptr<MediaTrack>> tracks) override;
  void onSample(std::string conn_name, std::shared_ptr<sny::SnyMediaSample> sample) override;

  void setConfigure(std::shared_ptr<SnyRTMPProxyConf> cnf) { cnf_ = std::move(cnf); }

 private:
  std::shared_ptr<sny::SnyIMuxer> createRtmpMuxer(const std::string& url);

 private:
  std::string name_;
  std::string app_name_;
  std::string stream_name_;
  std::shared_ptr<SnyRTMPProxyConf> cnf_;
  sny::Threads<SnyMultiRTMPPublish> threads_;
  std::vector<SnyRTMPProxyPublishItem> publish_streams_;
  std::map<int32_t, std::shared_ptr<MediaTrack>> tracks_;
  std::mutex mutex_;
  std::map<int, std::deque<std::shared_ptr<sny::SnyMediaSample>>> samples;
  std::map<int, std::condition_variable> cv_;
  std::map<int, std::mutex> mutex_cv_;
};
}  // namespace app
