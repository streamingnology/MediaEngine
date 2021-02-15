/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "sny_connection.h"

namespace sny {

class SnyConnectionManagerInterface;

class SnyConnectionSSL : public SnyConnection {
 public:
  SnyConnectionSSL(const SnyConnectionSSL&) = delete;
  SnyConnectionSSL& operator=(const SnyConnectionSSL&) = delete;

  explicit SnyConnectionSSL(asio::io_context& ic, asio::ssl::stream<asio::ip::tcp::socket> socket_ssl,
                            SnyConnectionManagerInterface* mgr);

  virtual void start();

  virtual void stop();

  virtual void on_deliver_event();

 private:
  virtual void do_read();

  virtual void do_write();

  void do_handshake();

  asio::ssl::stream<asio::ip::tcp::socket> socket_ssl_;
};

}  // namespace sny
