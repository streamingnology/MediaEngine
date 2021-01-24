#include <uv11.hpp>
#include <map>
#include <iostream>
#include <core/snydatabuffer.h>
#include "snyrtmpproxyconf.h"
#include "source/rtmp/rtmp_stream.h"
#include "snyrtmpproxy.h"
#define ELPPP_THREAD_SAFE
#include <easylogging++.h>
INITIALIZE_EASYLOGGINGPP
int main(int argc, char** args) {
  el::Configurations conf("el.conf");
  el::Loggers::reconfigureAllLoggers(conf);

  std::string rtmp_proxy_cnf_file = "config.json";
  std::fstream reader;
  reader.open(rtmp_proxy_cnf_file, std::ios::in);
  reader.seekg (0, std::ios::end);
  int length = reader.tellg();
  reader.seekg (0, std::ios::beg);
  sny::SnyDataBuffer conf_data; conf_data.resize(length);
  reader.read(conf_data.data(), length);
  reader.close();
  auto rtmp_proxy_cnf = app::parse(conf_data);

  std::mutex mutex;
  //std::map<std::string, std::shared_ptr<pvd::RtmpStream>> streams;
  std::map<std::string, std::shared_ptr<app::SnyRTMPProxy>> rtmp_proxys;
  uv::EventLoop* loop = uv::EventLoop::DefaultLoop();
  uv::SocketAddr addr("0.0.0.0", 1935, uv::SocketAddr::Ipv4);
  uv::TcpServer server(loop);
  server.setTimeout(60);
  server.setMessageCallback([&rtmp_proxys, &mutex](uv::TcpConnectionPtr ptr,
                                                   const char* data_buff,
                                                   ssize_t data_size){
                              mutex.lock();
                              std::string conn_name = ptr->Name();
                              auto iter = rtmp_proxys.find(conn_name);
                              mutex.unlock();
                              if (iter != rtmp_proxys.end()) {
                                iter->second->OnDataReceived(data_buff, data_size);
                              }
                            });
  server.setNewConnectCallback([&rtmp_proxys, &rtmp_proxy_cnf, &mutex](
                                   const std::weak_ptr<uv::TcpConnection>& conn) {
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
  server.setConnectCloseCallback([&rtmp_proxys, &mutex](
                                     const std::weak_ptr<uv::TcpConnection>& conn) {
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
  server.bindAndListen(addr);
  loop->run();

  return 0;
}
