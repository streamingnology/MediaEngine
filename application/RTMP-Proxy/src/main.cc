#include <Ap4FileByteStream.h>
#include <core/snydatabuffer.h>
#include <core/snyeasylogging.h>
#include <core/snyresults.h>
#include <iostream>
#include <map>
#include <uv11.hpp>
#include "snyrtmpproxy.h"
#include "snyrtmpproxyconf.h"
#include "source/rtmp/rtmp_stream.h"

int main(int argc, char** args) {
  el::Configurations conf("../conf/el.conf");
  el::Loggers::reconfigureAllLoggers(conf);

  std::string rtmp_proxy_cnf_file = "../conf/config.json";
  bool success = false;
  AP4_ByteStream* byte_stream = nullptr;
  do {
    AP4_Result result =
        AP4_FileByteStream::Create(rtmp_proxy_cnf_file.c_str(), AP4_FileByteStream::STREAM_MODE_READ, byte_stream);
    if (AP4_FAILED(result)) {
      LOG(ERROR) << "open config.json failed";
      break;
    }
    sny::SnyUI64 size;
    byte_stream->GetSize(size);
    sny::SnyDataBuffer conf_data;
    conf_data.resize(size);
    result = byte_stream->Read(conf_data.data(), conf_data.size());
    if (AP4_FAILED(result)) {
      LOG(ERROR) << "read config.json failed";
      break;
    }
    byte_stream->Release();
    byte_stream = nullptr;

    auto rtmp_proxy_cnf = app::parse(conf_data);
    if (rtmp_proxy_cnf == nullptr) {
      LOG(ERROR) << "parse config.json failed";
      break;
    }

    std::mutex mutex;
    std::map<std::string, std::shared_ptr<app::SnyRTMPProxy>> rtmp_proxys;
    uv::EventLoop* loop = uv::EventLoop::DefaultLoop();
    uv::SocketAddr addr("0.0.0.0", rtmp_proxy_cnf->rtmp_port, uv::SocketAddr::Ipv4);
    uv::TcpServer server(loop);
    server.setTimeout(60);
    server.setMessageCallback(
        [&rtmp_proxys, &mutex](uv::TcpConnectionPtr ptr, const char* data_buff, ssize_t data_size) {
          mutex.lock();
          std::string conn_name = ptr->Name();
          auto iter = rtmp_proxys.find(conn_name);
          mutex.unlock();
          if (iter != rtmp_proxys.end()) {
            iter->second->OnDataReceived(data_buff, data_size);
          }
        });
    server.setNewConnectCallback([&rtmp_proxys, &rtmp_proxy_cnf, &mutex](const std::weak_ptr<uv::TcpConnection>& conn) {
      mutex.lock();
      std::string conn_name;
      std::shared_ptr<uv::TcpConnection> tcp_connection_ptr = conn.lock();
      if (tcp_connection_ptr != nullptr) {
        auto rtmp_stream = std::make_shared<pvd::RtmpStream>(StreamSourceType::Rtmp, 1);
        rtmp_stream->SetConn(tcp_connection_ptr);
        conn_name = tcp_connection_ptr->Name();
        auto rtmp_proxy = std::make_shared<app::SnyRTMPProxy>(conn_name, rtmp_stream);
        rtmp_proxy->setConfigure(rtmp_proxy_cnf);
        rtmp_proxys.insert(std::make_pair(conn_name, rtmp_proxy));
      }
      std::cout << "new conn come from: " + conn_name << std::endl;
      std::cout << "there are " + std::to_string(rtmp_proxys.size()) + " streams." << std::endl;
      mutex.unlock();
    });
    server.setConnectCloseCallback([&rtmp_proxys, &mutex](const std::weak_ptr<uv::TcpConnection>& conn) {
      mutex.lock();
      std::string conn_name;
      std::shared_ptr<uv::TcpConnection> tcp_connection_ptr = conn.lock();
      if (tcp_connection_ptr != nullptr) {
        conn_name = tcp_connection_ptr->Name();
        auto iter = rtmp_proxys.find(conn_name);
        if (iter != rtmp_proxys.end()) {
          rtmp_proxys.erase(iter);
        }
      }
      std::cout << "close conn: " + conn_name << std::endl;
      std::cout << "there are " + std::to_string(rtmp_proxys.size()) + " streams left." << std::endl;
      mutex.unlock();
    });

    int ret = server.bindAndListen(addr);
    if (ret != 0) {
      LOG(ERROR) << "bind and listen on " << rtmp_proxy_cnf->rtmp_port << " failed";
      break;
    }
    loop->run();
    success = true;
  } while (false);

  if (byte_stream) {
    byte_stream->Release();
  }
  return success ? sny::SnySuccess : sny::SnyFailture;
}
