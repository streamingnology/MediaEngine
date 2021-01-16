#include <uv11.hpp>
#include <source/rtmp/rtmp_stream.h>

using namespace uv;

int main(int argc, char** args)
{
  std::shared_ptr<pvd::RtmpStream> rtmp_stream = nullptr;
  rtmp_stream = std::make_shared<pvd::RtmpStream>(StreamSourceType::Rtmp, 1);
  EventLoop* loop = EventLoop::DefaultLoop();

  SocketAddr addr("0.0.0.0", 1935, SocketAddr::Ipv4);

  std::atomic<uint64_t> dataSize(0);
  uv::TcpServer server(loop);
  server.setMessageCallback([&rtmp_stream](uv::TcpConnectionPtr ptr,const char* data_buff, ssize_t data_size)
                            {
                              rtmp_stream->OnDataReceived(data_buff, data_size);
                            });
  server.setNewConnectCallback([&rtmp_stream](std::weak_ptr<TcpConnection> conn) {
    std::string conn_name;
    std::shared_ptr<uv::TcpConnection> tcp_connection_ptr = conn.lock();
    if (tcp_connection_ptr != nullptr) {
      rtmp_stream->SetConn(tcp_connection_ptr);
      conn_name = tcp_connection_ptr->Name();
    }
    std::cout << "new conn come from: " + conn_name << std::endl;
  });
  server.setConnectCloseCallback([](std::weak_ptr<TcpConnection> conn) {
    std::cout << "close conn: " << std::endl;
  });
  //心跳超时
  //server.setTimeout(15);
  server.bindAndListen(addr);


  loop->run();
}

