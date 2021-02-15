/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

namespace sny {
class SnyConnectionDeliverHandler;

class SnyConnectionReceiveHandler {
 public:
  virtual void onDataReceived(const char* data, const int size) = 0;
  virtual void setDeliverHandler(std::shared_ptr<SnyConnectionDeliverHandler> deliver_handler) = 0;
};

class SnyConnectionDeliverHandler {
 public:
  virtual void deliver(const char* data, const int size) = 0;
  virtual void setConnReceiveHandler(std::shared_ptr<SnyConnectionReceiveHandler> receive_handler) = 0;
};

}