/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snyrtmpserver.h"
#include "core/snyeasylogging.h"
namespace sny {
SnyRTMPServer::SnyRTMPServer(const std::shared_ptr<uv::EventLoop>& loop, SnyUI16 rtmp_port)
    : threads_(this), uv::TcpServer(loop.get()), loop_(loop), rtmp_port_(rtmp_port), timeout_sec_(kRTMPTimeoutSec) {
  setNewConnectCallback(std::bind(&SnyRTMPServer::onRTMPNewConnectCallback, this, std::placeholders::_1));
  setConnectCloseCallback(std::bind(&SnyRTMPServer::onRTMPConnectCloseCallback, this, std::placeholders::_1));
  setMessageCallback(std::bind(&SnyRTMPServer::onRTMPMessageReceived, this, std::placeholders::_1,
                               std::placeholders::_2, std::placeholders::_3));
}

SnyRTMPServer::~SnyRTMPServer() { stop(); }

SnyUI16 SnyRTMPServer::getRtmpPort() const { return rtmp_port_; }

void SnyRTMPServer::setRtmpPort(unsigned short rtmpPort) { rtmp_port_ = rtmpPort; }

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
    LOG(DEBUG) << tcp_connection->Name();
    auto rtmp_stream = pvd::RtmpStream::Create(tcp_connection->Name());
    rtmp_stream->start();
    rtmp_stream->setRTMPSendDataCallback(std::bind(&SnyRTMPServer::onRTMPSendDataCallback, this, std::placeholders::_1,
                                                   std::placeholders::_2, std::placeholders::_3));
    rtmp_streams_.insert(std::make_pair(tcp_connection->Name(), rtmp_stream));
  }
}

void SnyRTMPServer::onRTMPConnectCloseCallback(const std::weak_ptr<uv::TcpConnection>& conn) {
  std::shared_ptr<uv::TcpConnection> tcp_connection = conn.lock();
  if (tcp_connection) {
    std::string conn_name = tcp_connection->Name();
    LOG(DEBUG) << conn_name;
    rtmp_streams_.erase(conn_name);
  } else {
    LOG(WARNING) << "not found this connection";
  }
}

void SnyRTMPServer::onRTMPMessageReceived(const uv::TcpConnectionPtr& conn, const char* data, ssize_t size) {
  auto conn_name = conn->Name();
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
  char* ptr = new char[size];
  memcpy(ptr, data, size);
  writeInLoop(conn_name, ptr, size, [](uv::WriteInfo& info) {
    char* p = info.buf;
    delete p;
  });
}
}  // namespace sny