/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <string>
#include "core/snythreads.h"
#include "sny_connection_config.h"
#include "sny_connection_manager_interface.h"

namespace sny {
static const int kServerRunThreadId = 0;
class SnyConnectionManager : public SnyConnectionManagerInterface {
 public:
  SnyConnectionManager(const SnyConnectionManager&) = delete;
  SnyConnectionManager& operator=(const SnyConnectionManager&) = delete;

  explicit SnyConnectionManager(const std::string& address, const int port_plain, const int port_ssl,
                                const int n_threads);

  void setSSLKeyFilePassword(std::string ssl_password) { ssl_key_file_password_ = ssl_password; }
  std::string getSSLKeyFilePassword() const { return ssl_key_file_password_; }

  void start();

  void stop();

  int onThreadProc(int id);

  void startConnection(std::shared_ptr<SnyConnection> c) override;

  void stopConnection(std::shared_ptr<SnyConnection> c) override;

  void stopAllConnection() override;

 protected:
  void init();
  void run();
  void run_io_context();

  void do_accept();
  void do_accept_ssl();
  void do_await_stop();

  virtual void onNewConnection(std::shared_ptr<SnyConnection> conn) = 0;

  asio::io_context io_context_;
  asio::ssl::context io_context_ssl_;

  asio::ip::tcp::acceptor acceptor_;
  asio::ip::tcp::acceptor acceptor_ssl_;

  std::string ip_address_;
  int ip_port_plain_;
  int ip_port_ssl_;
  int n_threads_;
  std::string ssl_key_file_password_;
  std::shared_ptr<SnyConnectionConfig> conn_cfg_;

  Threads<SnyConnectionManager> threads_;

  std::set<std::shared_ptr<SnyConnection>> connections_;
  std::recursive_mutex mutex_;
};

}  // namespace sny
