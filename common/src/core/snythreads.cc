/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snythreads.h"
#include <chrono>
#include <thread>

namespace sny {
SThread::SThread() {
  is_stop_ = true;
  is_running_ = false;
  thread_ = nullptr;
}

SThread::~SThread() {}

int SThread::runThreadLoop(void *user) {
  SThread *pThis = (SThread *)user;
  return pThis->threadLoop();
}

int SThread::threadLoop() {
  mutex_.lock();
  cv_.notify_one();
  mutex_.unlock();
  int ret = onThreadLoop();
  is_running_ = false;
  return ret;
}

int SThread::start(int priority, const char *name) {
  std::unique_lock<std::mutex> lock(mutex_);
  is_stop_ = false;
  is_running_ = true;
  // TODO:reset cv here

  thread_ = new thread(SThread::runThreadLoop, this);
  cv_.wait(lock);
  lock.unlock();
  return 0;
}

void SThread::stop() { is_stop_ = true; }

bool SThread::isStop() { return is_stop_; }

bool SThread::isRunning() { return is_running_; }

void SThread::wait() {
  if (thread_ != nullptr && thread_->joinable()) {
    thread_->join();
    delete thread_;
    thread_ = nullptr;
  }
  while (is_running_) {
    std::this_thread::sleep_for(std::chrono::microseconds(10 * 1000));
  }
}
}  // namespace sny