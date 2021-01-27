/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <map>
#include <memory>
#include <uv11.hpp>
#include "core/snyresults.h"
#include "core/snythreads.h"
#include "core/snytype.h"
#include "rtmp_stream.h"
namespace sny {
const SnyInt kRTMPTimeoutSec = 60;
const int kLoopRunThread = 0;
class SnyRTMPServer : public uv::TcpServer {
 public:
  SnyRTMPServer(const std::shared_ptr<uv::EventLoop>& loop, SnyUI16 rtmp_port = 1935);
  ~SnyRTMPServer();

  SnyUI16 getRtmpPort() const;
  void setRtmpPort(SnyUI16 rtmpPort);

  SnyInt getTimeout() const;
  void setRTMPTimeout(SnyInt timeoutSec);

  SnyResult start();
  void stop();

  void stopUvLoop() { loop_->stop(); }

 public:
  void onRTMPSendDataCallback(std::string& conn_name, const char* data, int size);
  int onThreadProc(int id);

 private:
  void onRTMPNewConnectCallback(const std::weak_ptr<uv::TcpConnection>& conn);
  void onRTMPConnectCloseCallback(const std::weak_ptr<uv::TcpConnection>& conn);
  void onRTMPMessageReceived(const uv::TcpConnectionPtr& conn, const char* data, ssize_t size);

 private:
  SnyUI16 rtmp_port_;
  SnyInt timeout_sec_;
  std::shared_ptr<uv::EventLoop> loop_;
  std::map<std::string, std::shared_ptr<pvd::RtmpStream>> rtmp_streams_;
  sny::Threads<SnyRTMPServer> threads_;
  std::map<std::string, std::deque<ov::Data>> saved_data_to_send_;
  std::map<std::any, std::shared_ptr<ov::Data>> mmm_;
};
}  // namespace sny