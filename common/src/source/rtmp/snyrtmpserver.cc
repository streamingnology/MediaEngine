/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snyrtmpserver.h"
#include "core/snyeasylogging.h"
#include "core/snyutils.h"
namespace sny {
SnyRTMPServer::SnyRTMPServer(const std::shared_ptr<uv::EventLoop>& loop, SnyUI16 rtmp_port)
    : uv::TcpServer(loop.get()), rtmp_port_(rtmp_port), timeout_sec_(kRTMPTimeoutSec), loop_(loop), threads_(this),
      new_connect_callback_(nullptr), connect_close_callback_(nullptr), source_callback_(nullptr) {

  setNewConnectCallback(std::bind(&SnyRTMPServer::onRTMPNewConnectCallback, this, std::placeholders::_1));
  setConnectCloseCallback(std::bind(&SnyRTMPServer::onRTMPConnectCloseCallback, this, std::placeholders::_1));
  setMessageCallback(std::bind(&SnyRTMPServer::onRTMPMessageReceived, this, std::placeholders::_1,
                               std::placeholders::_2, std::placeholders::_3));

}

SnyRTMPServer::~SnyRTMPServer() { stop(); }

SnyUI16 SnyRTMPServer::getRtmpPort() const { return rtmp_port_; }

void SnyRTMPServer::setRtmpPort(const SnyUI16 rtmpPort) { rtmp_port_ = rtmpPort; }

SnyInt SnyRTMPServer::getTimeout() const { return timeout_sec_; }

void SnyRTMPServer::setRTMPTimeout(SnyInt timeout_sec) {
  timeout_sec_ = timeout_sec;
  setTimeout(timeout_sec_);
}

SnyResult SnyRTMPServer::start() {
  setTimeout(timeout_sec_);

  uv::SocketAddr addr("0.0.0.0", rtmp_port_, uv::SocketAddr::Ipv4);
  int ret = bindAndListen(addr);
  if (ret != 0) {
    LOG(ERROR) << "bind and listen on " << rtmp_port_ << " failed";
    return SnyFailture;
  }
  LOG(INFO) << sny::SnyUtils::formatstring("rtmp server is listening on %d ...", rtmp_port_);
  threads_.start(kLoopRunThread);
  return SnySuccess;
}

void SnyRTMPServer::stop() {
  if (loop_) {
    loop_->runInThisLoop([this]() { this->stopUvLoop(); });
  }
}

void SnyRTMPServer::onRTMPNewConnectCallback(const std::weak_ptr<uv::TcpConnection>& conn) {
  std::shared_ptr<uv::TcpConnection> tcp_connection = conn.lock();
  if (tcp_connection) {
    const std::string& conn_name = tcp_connection->Name();
    LOG(DEBUG) << "new connection: " << conn_name;

    auto rtmp_stream = pvd::RtmpStream::Create(conn_name);
    rtmp_stream->setRTMPCallback(this);
    rtmp_stream->setRTMPSendDataCallback(std::bind(&SnyRTMPServer::onRTMPSendDataCallback, this, std::placeholders::_1,
                                                   std::placeholders::_2, std::placeholders::_3));
    rtmp_stream->start();
    rtmp_streams_.insert(std::make_pair(conn_name, rtmp_stream));
    
    if (new_connect_callback_) {
      new_connect_callback_(conn, conn_name);
    }
  }
  LOG(DEBUG) << "there are " << rtmp_streams_.size() << " input rtmp strems now";
}

void SnyRTMPServer::onRTMPConnectCloseCallback(const std::weak_ptr<uv::TcpConnection>& conn) {
  std::shared_ptr<uv::TcpConnection> tcp_connection = conn.lock();
  if (tcp_connection) {
    const std::string& conn_name = tcp_connection->Name();
    LOG(DEBUG) << "connection close: " << conn_name;
    rtmp_streams_.erase(conn_name);
    if (connect_close_callback_) {
      connect_close_callback_(conn_name);
    }
  } else {
    LOG(WARNING) << "not found this connection";
  }
  LOG(DEBUG) << "there are " << rtmp_streams_.size() << " input rtmp strems now";
}

void SnyRTMPServer::onRTMPMessageReceived(const uv::TcpConnectionPtr& conn, const char* data, ssize_t size) {
  const std::string& conn_name = conn->Name();
  auto item = rtmp_streams_.find(conn_name);
  if (item != rtmp_streams_.end()) {
    item->second->OnDataReceived(data, size);
  }
}

int SnyRTMPServer::onThreadProc(int id) {
  loop_->run();
  return 0;
}

void SnyRTMPServer::onRTMPSendDataCallback(string& conn_name, const char* data, const int size) {
  //TODO: learn uv-cpp how to manage buffer without copy, maybe need to modify uv-cpp to support this feature
  char* ptr = new char[size];
  memcpy(ptr, data, size);
  writeInLoop(conn_name, ptr, size, [](uv::WriteInfo& info) {
    char* p = info.buf;
    delete p;
  });
}

void SnyRTMPServer::setRTMPNewConnectCallback(const SnyRTMPServer::OnRTMPNewConnectCallback& callback) {
  new_connect_callback_ = callback;
}
void SnyRTMPServer::setRTMPConnectCloseCallback(const SnyRTMPServer::OnRTMPConnectCloseCallback& callback) {
  connect_close_callback_ = callback;
}

void SnyRTMPServer::onRtmpAppStreamName(const std::string& conn_name, const std::string& app_name,
                                        const std::string& stream_name) {
  if (source_callback_) {
    source_callback_->onRtmpAppStreamName(conn_name, app_name, stream_name);
  }
}
void SnyRTMPServer::onTrack(const std::string& conn_name,
                            const std::map<int32_t, std::shared_ptr<MediaTrack>>& tracks) {
  if (source_callback_) {
    source_callback_->onTrack(conn_name, tracks);
  }
}
void SnyRTMPServer::onSample(const std::string& conn_name, const std::shared_ptr<sny::SnyMediaSample> sample) {
  if (source_callback_) {
    source_callback_->onSample(conn_name, sample);
  }
}
}  // namespace sny