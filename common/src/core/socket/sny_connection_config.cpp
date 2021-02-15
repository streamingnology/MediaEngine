/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "sny_connection_config.h"

namespace sny {

SnyConnectionConfig::SnyConnectionConfig() {
  timeout_handshake_s = default_timeout_handshake_s;
  timeout_read_s = default_timeout_read_s;
  timeout_send_s = default_timeout_send_s;
}

SnyConnectionConfig::SnyConnectionConfig(int handshake_s, int read_s, int send_s) {
  timeout_handshake_s = handshake_s;
  timeout_read_s = read_s;
  timeout_send_s = send_s;
}

}  // namespace sny