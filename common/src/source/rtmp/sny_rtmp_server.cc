/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "sny_rtmp_server.h"
#include "source/rtmp/rtmp_stream.h"

namespace sny {
SnyRTMPServerASIO::SnyRTMPServerASIO(const std::string& address, const int port_plain, const int port_ssl,
                                     const int n_threads)
    : SnyConnectionManager(address, port_plain, port_ssl, n_threads) {}

SnyRTMPServerASIO::~SnyRTMPServerASIO() {}

void SnyRTMPServerASIO::onNewConnection(std::shared_ptr<SnyConnection> conn) {
  auto deliver_handler = std::dynamic_pointer_cast<sny::SnyConnectionDeliverHandler>(conn);

  auto rtmp_stream = std::make_shared<pvd::RtmpStream>("");
  auto receive_handler = std::dynamic_pointer_cast<sny::SnyConnectionReceiveHandler>(rtmp_stream);

  conn->setConnReceiveHandler(receive_handler);

  rtmp_stream->setDeliverHandler(std::move(deliver_handler));
  rtmp_streams_.push_back(rtmp_stream);
}

}  // namespace sny