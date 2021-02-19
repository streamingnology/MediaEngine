/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

#include <array>
#include <asio.hpp>
#include <asio/ssl.hpp>
#include <memory>
#include "core/snydatabuffer.h"
#include "sny_connection_callback.h"
#include "sny_connection_config.h"

namespace sny {

class SnyConnectionManagerInterface;

class SnyConnection : public SnyConnectionDeliverHandler, public std::enable_shared_from_this<SnyConnection> {
 public:
  SnyConnection(const SnyConnection&) = delete;
  SnyConnection& operator=(const SnyConnection&) = delete;

  explicit SnyConnection(asio::io_context& ic, SnyConnectionManagerInterface* mgr);

  virtual void start();

  virtual void stop();

  void setConnReceiveHandler(std::shared_ptr<sny::SnyConnectionReceiveHandler> handler);

  void deliver(const char* data, const int size);
  virtual void on_deliver_event() = 0;

  std::string getConnectionName() { return ip_address_ + ":" + ip_port_; }
  std::string getIPAddress() { return ip_address_; }
  std::string getIPPort() { return ip_port_; }

  void set_connection_config(std::shared_ptr<SnyConnectionConfig> cfg) { conn_cfg_ = cfg; }

 protected:
  asio::io_context& io_context_;

  std::shared_ptr<SnyConnectionConfig> conn_cfg_;
  asio::steady_timer timer_;
  void initialize_timer();
  void set_timer(int s);
  void stop_timer();
  void handle_timeout(const std::error_code& ec);

  std::string ip_address_;
  std::string ip_port_;
  void setIPAddress(std::string ip) { ip_address_ = ip; }
  void setIPPort(std::string port) { ip_port_ = port; }

  virtual void do_read();

  virtual void do_write();

  SnyConnectionManagerInterface* conn_mgr_;

  std::array<char, 8192> buffer_;

  std::recursive_mutex mutex_;
  std::list<std::shared_ptr<asio::const_buffer>> buffers_need_send_;
  std::list<SnyDataBuffer> buffers_saved_;

  std::shared_ptr<sny::SnyConnectionReceiveHandler> receive_handler_;

  std::vector<asio::const_buffer> reply_;

  bool stopped_;
};

}  // namespace sny