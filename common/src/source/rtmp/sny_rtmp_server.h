/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <memory>
#include "core/socket/sny_connection_manager.h"
#include "source/rtmp/rtmp_stream.h"

namespace sny {

class SnyRTMPServerASIO : public SnyConnectionManager {
 public:
  SnyRTMPServerASIO(const SnyRTMPServerASIO&) = delete;
  SnyRTMPServerASIO& operator=(const SnyRTMPServerASIO&) = delete;
  SnyRTMPServerASIO(const std::string& address, const int port_plain, const int port_ssl, const int n_threads);
  ~SnyRTMPServerASIO();

 private:
  void onNewConnection(std::shared_ptr<SnyConnection> conn) override;

  std::vector<std::shared_ptr<pvd::RtmpStream>> rtmp_streams_;
};

}  // namespace sny