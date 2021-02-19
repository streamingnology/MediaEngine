/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "sny_connection_ssl.h"
#include <iostream>
#include "core/snyeasylogging.h"
#include "sny_connection_manager_interface.h"

namespace sny {

SnyConnectionSSL::SnyConnectionSSL(asio::io_context& ic, asio::ssl::stream<asio::ip::tcp::socket> socket_ssl,
                                   SnyConnectionManagerInterface* mgr)
    : SnyConnection(ic, mgr), socket_ssl_(std::move(socket_ssl)) {
  setIPAddress(socket_ssl_.lowest_layer().remote_endpoint().address().to_string());
  setIPPort(std::to_string(socket_ssl_.lowest_layer().remote_endpoint().port()));
}

void SnyConnectionSSL::start() {
  SnyConnection::start();
  do_handshake();
}

void SnyConnectionSSL::stop() {
  SnyConnection::stop();
  socket_ssl_.lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both);
  socket_ssl_.lowest_layer().close();
}

void SnyConnectionSSL::on_deliver_event() { do_write(); }

void SnyConnectionSSL::do_read() {
  SnyConnection::do_read();

  auto self(shared_from_this());
  socket_ssl_.async_read_some(asio::buffer(buffer_), [this, self](std::error_code ec, std::size_t bytes_transferred) {
    std::cout << ec.message() << std::endl;
    if (!ec) {
      if (receive_handler_) {
        receive_handler_->onDataReceived(buffer_.data(), bytes_transferred);
      }
    } else if (ec == asio::error::operation_aborted) {
      LOG(WARNING) << "SnyConnectionSSL::do_read, " << getConnectionName() << "." << ec.message();
    } else {
      LOG(WARNING) << "SnyConnectionSSL::do_read, " << getConnectionName() << "." << ec.message();
      conn_mgr_->stopConnection(shared_from_this());
    }
  });
}

void SnyConnectionSSL::do_write() {
  SnyConnection::do_write();

  mutex_.lock();
  std::shared_ptr<asio::const_buffer> buffer_send = buffers_need_send_.front();
  mutex_.unlock();

  reply_.push_back(asio::buffer(buffer_send->data(), buffer_send->size()));

  auto self(shared_from_this());
  asio::async_write(socket_ssl_, reply_, [this, self](std::error_code ec, std::size_t) {
    if (!ec) {
    } else if (ec == asio::error::operation_aborted) {
      LOG(WARNING) << "SnyConnectionSSL::do_write, " << getConnectionName() << "." << ec.message();
    } else {
      LOG(WARNING) << "SnyConnectionSSL::do_write, " << getConnectionName() << "." << ec.message();
      conn_mgr_->stopConnection(shared_from_this());
    }

    mutex_.lock();
    buffers_need_send_.pop_front();
    bool still_write = !buffers_need_send_.empty();
    mutex_.unlock();
    if (still_write) {
      do_write();
    }
  });
}

void SnyConnectionSSL::do_handshake() {

  auto self(shared_from_this());
  socket_ssl_.async_handshake(asio::ssl::stream_base::server, [this, self](const std::error_code& ec) {
    if (!ec) {
      do_read();
    } else if (ec == asio::error::operation_aborted) {
      LOG(WARNING) << "SnyConnectionSSL::do_handshake, " << getConnectionName() << "." << ec.message();
    } else {
      LOG(WARNING) << "SnyConnectionSSL::do_handshake, " << getConnectionName() << "." << ec.message();
      conn_mgr_->stopConnection(shared_from_this());
    }
  });

}

}  // namespace sny
