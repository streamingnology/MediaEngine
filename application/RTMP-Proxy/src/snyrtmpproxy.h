/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <memory>
#include <map>
#include <utility>
#include <vector>
#include <core/snythreads.h>
#include <source/rtmp/rtmp_stream.h>
#include "snyrtmpproxyconf.h"
#include <source/snysourcecallback.h>
#include <media/rtmp_writer.h>
namespace app {
class SnyRTMPProxy : public sny::SnySourceCallback {
 public:
  SnyRTMPProxy(std::string name, std::shared_ptr<pvd::RtmpStream> rtmp_stream);
  ~SnyRTMPProxy();

  void start();
  void stop();
  int onThreadProc(int id);

  void onRtmpAppStreamName(std::string app_name, std::string stream_name) override;
  void onTrack(std::map<int32_t, std::shared_ptr<MediaTrack>> tracks) override;
  void onSample(std::shared_ptr<sny::SnyMediaSample> sample) override;

  void setConfigure(std::shared_ptr<SnyRTMPProxyConf> cnf) { cnf_ = std::move(cnf); }
  void OnDataReceived(const char* data_buff, ssize_t data_size);

 private:
  std::shared_ptr<RtmpWriter> createRtmpMuxer(const std::string& url);
 private:
  std::string name_;
  std::string app_name_;
  std::string stream_name_;
  std::shared_ptr<SnyRTMPProxyConf> cnf_;
  sny::Threads<SnyRTMPProxy> threads_;
  std::shared_ptr<pvd::RtmpStream> rtmp_stream_;
  std::vector<SnyRTMPProxyPublishItem> publish_streams_;
  std::map<int32_t, std::shared_ptr<MediaTrack>> tracks_;
  std::mutex mutex_;
  std::map<int, std::deque<std::shared_ptr<sny::SnyMediaSample>>> samples;
};
}
