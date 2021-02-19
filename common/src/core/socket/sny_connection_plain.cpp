/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "sny_connection_plain.h"
#include <iostream>
#include "core/snyeasylogging.h"
#include "sny_connection_manager_interface.h"

namespace sny {

SnyConnectionPlain::SnyConnectionPlain(asio::io_context& ic, asio::ip::tcp::socket socket,
                                       SnyConnectionManagerInterface* mgr)
    : SnyConnection(ic, mgr), socket_(std::move(socket)) {
  setIPAddress(socket_.remote_endpoint().address().to_string());
  setIPPort(std::to_string(socket_.remote_endpoint().port()));
}

void SnyConnectionPlain::start() {
  SnyConnection::start();
  do_read();
}

void SnyConnectionPlain::stop() {
  SnyConnection::stop();
  socket_.lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both);
  socket_.lowest_layer().close();
}

void SnyConnectionPlain::on_deliver_event() { do_write(); }

void SnyConnectionPlain::do_read() {
  SnyConnection::do_read();

  auto self(shared_from_this());
  socket_.async_read_some(asio::buffer(buffer_), [this, self](std::error_code ec, std::size_t bytes_transferred) {
    if (!ec) {
      if (receive_handler_) {
        receive_handler_->onDataReceived(buffer_.data(), bytes_transferred);
      }
      do_read();
    } else if (ec == asio::error::operation_aborted) {
      LOG(WARNING) << "SnyConnectionPlain::do_read, " << getConnectionName() << "." << ec.message();
    } else {
      LOG(WARNING) << "SnyConnectionPlain::do_read, " << getConnectionName() << "." << ec.message();
      conn_mgr_->stopConnection(shared_from_this());
    }
  });
}

void SnyConnectionPlain::do_write() {
  SnyConnection::do_write();

  mutex_.lock();
  std::shared_ptr<asio::const_buffer> buffer_send = buffers_need_send_.front();
  mutex_.unlock();

  reply_.push_back(asio::buffer(buffer_send->data(), buffer_send->size()));

  auto self(shared_from_this());
  asio::async_write(socket_, reply_, [this, self](std::error_code ec, std::size_t) {
    reply_.clear();
    if (!ec) {
    } else if (ec == asio::error::operation_aborted) {
      LOG(WARNING) << "SnyConnectionPlain::do_write, " << getConnectionName() << "." << ec.message();
    } else {
      LOG(WARNING) << "SnyConnectionPlain::do_write, " << getConnectionName() << "." << ec.message();
      conn_mgr_->stopConnection(shared_from_this());
    }
    mutex_.lock();
    buffers_need_send_.pop_front();
    buffers_saved_.pop_front();
    bool still_write = !buffers_need_send_.empty();
    mutex_.unlock();
    if (still_write) {
      do_write();
    }
  });
}

}  // namespace sny
