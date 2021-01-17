/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
namespace sny {
class SnyIEventCallback {
 public:
  SnyIEventCallback() {}
  virtual ~SnyIEventCallback() {}

 public:
  virtual void onEventCallback(void* user_data) = 0;
};
}  // namespace sny

