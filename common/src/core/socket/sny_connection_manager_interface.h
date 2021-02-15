/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

#include <memory>
#include <set>
#include "sny_connection.h"

namespace sny {

class SnyConnectionManagerInterface {
 public:
  virtual void startConnection(std::shared_ptr<SnyConnection> c) = 0;

  virtual void stopConnection(std::shared_ptr<SnyConnection> c) = 0;

  virtual void stopAllConnection() = 0;
};

}  // namespace sny
