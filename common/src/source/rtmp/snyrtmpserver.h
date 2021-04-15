/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <map>
#include <memory>
#include "core/snyresults.h"
#include "core/snythreads.h"
#include "core/snytype.h"
#include "rtmp_stream.h"
#include <uv11.hpp>
namespace sny {

class SnyRTMPServer final: public uv::TcpServer, SnySourceCallback {
  using OnRTMPNewConnectCallback =
      std::function<void(const std::weak_ptr<uv::TcpConnection>& conn, const std::string& conn_name)>;
  using OnRTMPConnectCloseCallback = std::function<void(const std::string& conn_name)>;

 public:
  SnyRTMPServer(const std::shared_ptr<uv::EventLoop>& loop, const SnyUI16 rtmp_port = 1935);
  SnyRTMPServer(SnyRTMPServer&) = delete;
  virtual ~SnyRTMPServer();

  SnyUI16 getRtmpPort() const;
  void setRtmpPort(const SnyUI16 rtmpPort);

  SnyInt getTimeout() const;
  void setRTMPTimeout(SnyInt timeoutSec);
  void setRTMPNewConnectCallback(const OnRTMPNewConnectCallback& callback);
  void setRTMPConnectCloseCallback(const OnRTMPConnectCloseCallback& callback);
  void setSourceCallback(SnySourceCallback* callback) { source_callback_ = callback; }

  SnyResult start();
  void stop();
  void stopUvLoop() { loop_->stop(); }

 public:
  void onRTMPSendDataCallback(std::string& conn_name, const char* data, const int size);
  int onThreadProc(const int id);

  void onRtmpAppStreamName(const std::string& conn_name, const std::string& app_name,
                           const std::string& stream_name) override;
  void onTrack(const std::string& conn_name, const std::map<int32_t, std::shared_ptr<MediaTrack>>& tracks) override;
  void onSample(const std::string& conn_name, const std::shared_ptr<sny::SnyMediaSample> sample) override;

 private:
  void onRTMPNewConnectCallback(const std::weak_ptr<uv::TcpConnection>& conn);
  void onRTMPConnectCloseCallback(const std::weak_ptr<uv::TcpConnection>& conn);
  void onRTMPMessageReceived(const uv::TcpConnectionPtr& conn, const char* data, const ssize_t size);

 private:
  const SnyInt kRTMPTimeoutSec = 60;
  const int kLoopRunThread = 0;

  SnyUI16 rtmp_port_;
  SnyInt timeout_sec_;
  std::shared_ptr<uv::EventLoop> loop_;
  std::map<std::string, std::shared_ptr<pvd::RtmpStream>> rtmp_streams_;
  sny::Threads<SnyRTMPServer> threads_;
  OnRTMPNewConnectCallback new_connect_callback_;
  OnRTMPConnectCloseCallback connect_close_callback_;
  SnySourceCallback* source_callback_;
};
}  // namespace sny