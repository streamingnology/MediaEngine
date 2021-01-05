/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYTHREADS_H
#define STREAMINGNOLOGY_CORE_SNYTHREADS_H

#include <condition_variable>
#include <map>
#include <mutex>
#include <thread>

using namespace std;
namespace sny {
class SThread {
 public:
  SThread();
  virtual ~SThread();

 public:
  int start(int priority = 0, const char *name = "");
  void stop();
  bool isStop();
  bool isRunning();
  void wait();

 protected:
  virtual int onThreadLoop() = 0;

 private:
  static int runThreadLoop(void *user);
  virtual int threadLoop();

 private:
  bool is_stop_;
  bool is_running_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::thread *thread_;
};

template <class ThreadHandler>
class Threads {
 public:
  Threads(ThreadHandler *handler) : ptr_thread_handler_(handler) {}
  virtual ~Threads() {}

 public:
  bool start(int id, int priority = 0, const char *name = "");
  void stop(int id);
  bool isStart(int id);
  void stopAll();
  bool isStop(int id);
  bool isRunning(int id);
  void wait(int id);
  void waitAll();

 protected:
  class AThread : public SThread {
   public:
    AThread(Threads *pt, int id) : SThread(), ptr_threads_(pt), id_(id) {}
    int start(int priority = 0, const char *name = "") {
      return SThread::start(priority, name);
    }
    void stop() { SThread::stop(); }
    bool isStop() { return SThread::isStop(); }
    void wait() { SThread::wait(); }

   private:
    Threads *ptr_threads_;
    int id_;
    virtual int onThreadLoop() { return ptr_threads_->onThreadProc(id_); }
  };
  int onThreadProc(int id) { return ptr_thread_handler_->onThreadProc(id); }

 private:
  ThreadHandler *ptr_thread_handler_;
  map<int, AThread *> threads_;
  std::recursive_mutex mutex_;
};

template <class ThreadHandler>
bool Threads<ThreadHandler>::start(int id, int priority, const char *name) {
  std::lock_guard<std::recursive_mutex> lock(mutex_);

  if (threads_.find(id) != threads_.end()) {
    return true;
  }

  AThread *pThread = new AThread(this, id);
  if (pThread == nullptr) {
    return false;
  }
  threads_[id] = pThread;
  if (pThread->start(priority, name) < 0) {
    threads_.erase(id);
    delete pThread;
    return false;
  }
  return true;
}

template <class ThreadHandler>
void Threads<ThreadHandler>::stop(int id) {
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (threads_.find(id) == threads_.end()) {
    return;
  }

  AThread *pThread = threads_[id];
  pThread->stop();
}

template <class ThreadHandler>
bool Threads<ThreadHandler>::isStart(int id) {
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (threads_.find(id) != threads_.end()) {
    return true;
  }
  return false;
}

template <class ThreadHandler>
void Threads<ThreadHandler>::stopAll() {
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  for (typename map<int, AThread *>::iterator iter = threads_.begin();
       iter != threads_.end(); iter++) {
    AThread *pThread = iter->second;
    pThread->stop();
  }
}

template <class ThreadHandler>
bool Threads<ThreadHandler>::isStop(int id) {
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (threads_.find(id) == threads_.end()) {
    return true;
  }
  AThread *pThread = threads_[id];
  return pThread->isStop();
}

template <class ThreadHandler>
bool Threads<ThreadHandler>::isRunning(int id) {
  std::lock_guard<std::recursive_mutex> lock(mutex_);
  if (threads_.find(id) == threads_.end()) {
    return false;
  }
  AThread *pThread = threads_[id];
  return pThread->isRunning();
}

template <class ThreadHandler>
void Threads<ThreadHandler>::wait(int id) {
  mutex_.lock();
  if (threads_.find(id) == threads_.end()) {
    mutex_.unlock();
    return;
  }
  AThread *pThread = threads_[id];
  threads_.erase(id);
  mutex_.unlock();

  pThread->wait();
  delete pThread;
}

template <class ThreadHandler>
void Threads<ThreadHandler>::waitAll() {
  while (true) {
    mutex_.lock();
    if (threads_.empty()) {
      mutex_.unlock();
      break;
    }
    typename map<int, AThread *>::iterator iter = threads_.begin();
    AThread *pThread = iter->second;
    threads_.erase(iter);

    mutex_.unlock();

    pThread->wait();
    delete pThread;
  }
}
}  // namespace sny
#endif  // !STREAMINGNOLOGY_CORE_SNYTHREADS_H
