/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "sny_connection.h"

namespace sny {

class SnyConnectionManagerInterface;

class SnyConnectionPlain : public SnyConnection {
 public:
  SnyConnectionPlain(const SnyConnectionPlain&) = delete;
  SnyConnectionPlain& operator=(const SnyConnectionPlain&) = delete;

  explicit SnyConnectionPlain(asio::io_context& ic, asio::ip::tcp::socket socket, SnyConnectionManagerInterface* mgr);

  virtual void start();

  virtual void stop();

  virtual void on_deliver_event();

 private:
  virtual void do_read();

  virtual void do_write();

  asio::ip::tcp::socket socket_;
};

}  // namespace sny