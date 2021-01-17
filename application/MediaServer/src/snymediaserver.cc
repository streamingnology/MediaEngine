#include <uv11.hpp>
#include <source/rtmp/rtmp_stream.h>
#include "map"
using namespace uv;

int main(int argc, char** args)
{
  std::mutex mutex;
  std::map<std::string, std::shared_ptr<pvd::RtmpStream>> streams;

  EventLoop* loop = EventLoop::DefaultLoop();

  SocketAddr addr("0.0.0.0", 1935, SocketAddr::Ipv4);

  std::atomic<uint64_t> dataSize(0);
  uv::TcpServer server(loop);
  server.setMessageCallback([&streams, &mutex](uv::TcpConnectionPtr ptr,const char* data_buff, ssize_t data_size)
                            {
                              mutex.lock();
                              std::string conn_name = ptr->Name();
                              auto iter = streams.find(conn_name);
                              mutex.unlock();
                              if (iter != streams.end()) {
                                iter->second->OnDataReceived(data_buff, data_size);
                              }
                            });
  server.setNewConnectCallback([&streams, &mutex](std::weak_ptr<TcpConnection> conn) {
    mutex.lock();
    std::string conn_name;
    auto rtmp_stream = std::make_shared<pvd::RtmpStream>(StreamSourceType::Rtmp, 1);
    std::shared_ptr<uv::TcpConnection> tcp_connection_ptr = conn.lock();
    if (tcp_connection_ptr != nullptr) {
      rtmp_stream->SetConn(tcp_connection_ptr);
      conn_name = tcp_connection_ptr->Name();
      streams.insert(std::make_pair(conn_name, rtmp_stream));
    }
    std::cout << "new conn come from: " + conn_name << std::endl;
    std::cout << "there are " + std::to_string(streams.size()) + " streams." << std::endl;
    mutex.unlock();
  });
  server.setConnectCloseCallback([&streams, &mutex](std::weak_ptr<TcpConnection> conn) {
    mutex.lock();
    std::string conn_name;
    std::shared_ptr<uv::TcpConnection> tcp_connection_ptr = conn.lock();
    if (tcp_connection_ptr != nullptr) {
      conn_name = tcp_connection_ptr->Name();
      auto iter = streams.find(conn_name);
      if (iter != streams.end()) {
        streams.erase(iter);
      }
    }
    std::cout << "close conn: " + conn_name << std::endl;

    std::cout << "there are " + std::to_string(streams.size()) + " streams left." << std::endl;
    mutex.unlock();
  });
  //心跳超时
  //server.setTimeout(15);
  server.bindAndListen(addr);


  loop->run();
}

