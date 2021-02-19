/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "sny_connection_manager.h"
#include "core/snyeasylogging.h"
#include "sny_connection_plain.h"
#include "sny_connection_ssl.h"

namespace sny {

SnyConnectionManager::SnyConnectionManager(const std::string& address, const int port_plain, const int port_ssl,
                                           const int n_threads)
    : io_context_(n_threads),
      acceptor_(io_context_),
      io_context_ssl_(asio::ssl::context::sslv23),
      acceptor_ssl_(io_context_),
      n_threads_(n_threads),
      threads_(this) {
  ip_address_ = address;
  ip_port_plain_ = port_plain;
  ip_port_ssl_ = port_ssl;
}

void SnyConnectionManager::init() {
  conn_cfg_ = std::make_shared<SnyConnectionConfig>();

  do_await_stop();

  // listen on plain socket
  asio::ip::tcp::endpoint endpoint_plain(asio::ip::address::from_string(ip_address_), ip_port_plain_);
  acceptor_.open(endpoint_plain.protocol());
  acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint_plain);
  acceptor_.listen();

  do_accept();

  try {
    // load ssl perm
    io_context_ssl_.set_options(asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 |
                                asio::ssl::context::single_dh_use);
    io_context_ssl_.set_password_callback(std::bind(&SnyConnectionManager::getSSLKeyFilePassword, this));
    io_context_ssl_.use_certificate_chain_file("../conf/ssl/server.crt");
    io_context_ssl_.use_private_key_file("../conf/ssl/server.key", asio::ssl::context::pem);
    io_context_ssl_.use_tmp_dh_file("../conf/ssl/dh2048.pem");
  } catch (std::exception e) {
    LOG(WARNING) << "load ssl perm failed, " << e.what();
  }

  // listen on ssl socket
  asio::ip::tcp::endpoint endpoint_ssl(asio::ip::address::from_string(ip_address_), ip_port_ssl_);
  acceptor_ssl_.open(endpoint_ssl.protocol());
  acceptor_ssl_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_ssl_.bind(endpoint_ssl);
  acceptor_ssl_.listen();

  do_accept_ssl();
}

void SnyConnectionManager::run() {
  // Create a pool of threads to run all of the io_contexts.
  std::vector<std::shared_ptr<std::thread> > threads;
  for (std::size_t i = 0; i < n_threads_; ++i) {
    std::shared_ptr<std::thread> t = std::make_shared<std::thread>([this]() { this->run_io_context(); });
    threads.push_back(t);
  }

  for (std::size_t i = 0; i < threads.size(); ++i) threads[i]->join();
}

void SnyConnectionManager::start() {
  init();
  threads_.start(kServerRunThreadId);
}

void SnyConnectionManager::stop() {}

int SnyConnectionManager::onThreadProc(int id) {
  run();
  return id;
}

void SnyConnectionManager::startConnection(std::shared_ptr<SnyConnection> c) {
  connections_.insert(c);
  c->start();
}

void SnyConnectionManager::stopConnection(std::shared_ptr<SnyConnection> c) {
  connections_.erase(c);
  c->stop();
}

void SnyConnectionManager::stopAllConnection() {
  for (auto c : connections_) c->stop();
  connections_.clear();
}

void SnyConnectionManager::run_io_context() { io_context_.run(); }

void SnyConnectionManager::do_accept() {
  acceptor_.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
    if (!acceptor_.is_open()) {
      return;
    }

    if (!ec) {
#if 0
      auto conn_plain = std::make_shared<SnyConnectionPlain>(io_context_, std::move(socket), this);
      conn_plain->set_connection_config(conn_cfg_);
      auto deliver_handler = std::dynamic_pointer_cast<sny::SnyConnectionDeliverHandler>(conn_plain);

      auto rtmp_stream = std::make_shared<pvd::RtmpStream>("");
      auto receive_handler = std::dynamic_pointer_cast<sny::SnyConnectionReceiveHandler>(rtmp_stream);

      conn_plain->setConnReceiveHandler(receive_handler);
      rtmp_stream->setDeliverHandler(std::move(deliver_handler));

      rtmp_stream->start();
#else
      auto conn_plain = std::make_shared<SnyConnectionPlain>(io_context_, std::move(socket), this);
      conn_plain->set_connection_config(conn_cfg_);
#endif
      onNewConnection(conn_plain);
      startConnection(std::dynamic_pointer_cast<SnyConnection>(conn_plain));
    }

    do_accept();
  });
}

void SnyConnectionManager::do_await_stop() {}

void SnyConnectionManager::do_accept_ssl() {
  acceptor_ssl_.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
    if (!acceptor_ssl_.is_open()) {
      return;
    }

    if (!ec) {
#if 0
      auto stream = asio::ssl::stream<asio::ip::tcp::socket>(std::move(socket), io_context_ssl_);
      auto conn_ssl = std::make_shared<SnyConnectionSSL>(io_context_, std::move(stream), this);
      conn_ssl->set_connection_config(conn_cfg_);
      auto deliver_handler = std::dynamic_pointer_cast<sny::SnyConnectionDeliverHandler>(conn_ssl);

      auto rtmp_stream = std::make_shared<pvd::RtmpStream>("");
      rtmp_stream->setDeliverHandler(std::move(deliver_handler));
      auto receive_handler = std::dynamic_pointer_cast<sny::SnyConnectionReceiveHandler>(rtmp_stream);
#else
      auto stream = asio::ssl::stream<asio::ip::tcp::socket>(std::move(socket), io_context_ssl_);
      auto conn_ssl = std::make_shared<SnyConnectionSSL>(io_context_, std::move(stream), this);
#endif
      onNewConnection(conn_ssl);
      startConnection(std::dynamic_pointer_cast<SnyConnection>(conn_ssl));
    }

    do_accept_ssl();
  });
}

}  // namespace sny
