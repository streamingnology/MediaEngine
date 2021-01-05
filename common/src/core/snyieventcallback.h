/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYIEVENTCALLBACK_H
#define STREAMINGNOLOGY_CORE_SNYIEVENTCALLBACK_H

namespace sny {
class SnyIEventCallback {
 public:
  SnyIEventCallback() {}
  virtual ~SnyIEventCallback() {}

 public:
  virtual void onEventCallback(void* user_data) = 0;
};
}  // namespace sny

#endif  // !STREAMINGNOLOGY_CORE_SNYIEVENTCALLBACK_H
