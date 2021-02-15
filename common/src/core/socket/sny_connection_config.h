/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

namespace sny {

const int default_timeout_handshake_s = 10;
const int default_timeout_read_s = 50;
const int default_timeout_send_s = 50;

class SnyConnectionConfig {
 public:
  SnyConnectionConfig();
  SnyConnectionConfig(int handshake_s, int read_s, int send_s);
  ~SnyConnectionConfig() {
    timeout_handshake_s = 0;
    timeout_read_s = 0;
    timeout_send_s = 0;
  }
  int timeout_handshake_s;
  int timeout_read_s;
  int timeout_send_s;
};

}  // namespace sny