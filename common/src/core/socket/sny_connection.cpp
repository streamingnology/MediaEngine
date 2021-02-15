/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "sny_connection.h"
#include "core/snyeasylogging.h"
#include "core/snyutils.h"
#include "sny_connection_manager_interface.h"

namespace sny {
SnyConnection::SnyConnection(asio::io_context& ic, SnyConnectionManagerInterface* mgr)
    : io_context_(ic), conn_mgr_(mgr), timer_(ic) {
  stopped_ = false;
  receive_handler_ = nullptr;
}

void SnyConnection::start() {
  std::string log = SnyUtils::formatstring("new connection coming, %s : %s", ip_address_.c_str(), ip_port_.c_str());
  LOG(INFO) << log;
  initialize_timer();
  set_timer(conn_cfg_->timeout_handshake_s);
}

void SnyConnection::stop() {
  std::string log = SnyUtils::formatstring("del connection, %s : %s", ip_address_.c_str(), ip_port_.c_str());
  LOG(INFO) << log;
  stopped_ = true;
  stop_timer();
}

void SnyConnection::setConnReceiveHandler(std::shared_ptr<sny::SnyConnectionReceiveHandler> handler) {
  receive_handler_ = handler;
}

void SnyConnection::deliver(const char* data, const int size) {
  // LOG(DEBUG) << "deliver, " << size;
  SnyDataBuffer data_bufer(data, size);
  mutex_.lock();
  bool write_in_process = !buffers_need_send_.empty();
  buffers_saved_.push_back(data_bufer);
  auto buffer = std::make_shared<asio::const_buffer>(data_bufer.data(), data_bufer.size());
  buffers_need_send_.push_back(buffer);
  mutex_.unlock();
  if (!write_in_process) {
    on_deliver_event();
  }
}

void SnyConnection::do_read() { set_timer(conn_cfg_->timeout_read_s); }

void SnyConnection::do_write() { set_timer(conn_cfg_->timeout_send_s); }

void SnyConnection::initialize_timer() {
  // timer_.async_wait(std::bind(&connection::handle_timeout, this, std::placeholders::_1));
}

void SnyConnection::set_timer(int s) {
  timer_.expires_after(std::chrono::seconds(s));
  timer_.async_wait(std::bind(&SnyConnection::handle_timeout, this, std::placeholders::_1));
}

void SnyConnection::stop_timer() { timer_.cancel(); }

void SnyConnection::handle_timeout(const std::error_code& ec) {
  if (!ec) {
    if (timer_.expiry() <= asio::steady_timer::clock_type::now()) {
      // There is no longer an active deadline. The expiry is set to the
      // maximum time point so that the actor takes no action until a new
      // deadline is set.
      timer_.expires_at(asio::steady_timer::time_point::max());

      // The deadline has passed. The socket is closed so that any outstanding
      // asynchronous operations are cancelled.
      conn_mgr_->stopConnection(shared_from_this());
    }
  }
}

}  // namespace sny
