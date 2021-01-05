/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYREFOBJ_H
#define STREAMINGNOLOGY_CORE_SNYREFOBJ_H

#include <atomic>
namespace sny {
class SnyRefObj {
 public:
  SnyRefObj();
  virtual ~SnyRefObj();

 public:
  virtual void addRef();
  virtual void release();
  virtual int refCount();

 protected:
  virtual void onObjRelease();

 public:
  static int globalRefCount();

 private:
  std::atomic<int> ref_count_;
  static std::atomic<int> global_ref_count_;
};
}  // namespace sny

#endif  // !STREAMINGNOLOGY_CORE_SNYREFOBJ_H
