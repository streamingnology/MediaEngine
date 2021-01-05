/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snyrefobj.h"
namespace sny {
std::atomic<int> SnyRefObj::global_ref_count_(0);

SnyRefObj::SnyRefObj() {
  ref_count_ = 1;
  SnyRefObj::global_ref_count_++;
}

SnyRefObj::~SnyRefObj() {}

void SnyRefObj::addRef() {
  ref_count_++;
  SnyRefObj::global_ref_count_++;
}

void SnyRefObj::release() {
  ref_count_--;
  SnyRefObj::global_ref_count_--;
  if (ref_count_ == 0) {
    onObjRelease();
    return;
  }
}

int SnyRefObj::refCount() { return ref_count_; }

void SnyRefObj::onObjRelease() { delete this; }

int SnyRefObj::globalRefCount() { return global_ref_count_; }
}  // namespace sny