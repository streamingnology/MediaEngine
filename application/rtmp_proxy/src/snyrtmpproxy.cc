/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snyrtmpproxy.h"
#include <utility>
#include <uv11.hpp>
#include <core/snyeasylogging.h>

namespace app {
SnyRTMPProxy::SnyRTMPProxy() : sny_rtmp_server_(nullptr), cnf_(nullptr) {}

SnyRTMPProxy::~SnyRTMPProxy() { stop(); }

sny::SnyResult SnyRTMPProxy::start() {
  std::lock_guard lk(mutex_);
  if (sny_rtmp_server_ != nullptr) {
    LOG(ERROR) << "rtmp server already started";
    return sny::SnySuccess;
  }

  if (cnf_ == nullptr) {
    LOG(ERROR) << "no rtmp server configure found";
    return sny::SnyFailture;
  }

  auto loop = std::make_shared<uv::EventLoop>();
  sny_rtmp_server_ = std::make_shared<sny::SnyRTMPServer>(loop, cnf_->rtmp_port);
  sny_rtmp_server_->setSourceCallback(this);
  sny_rtmp_server_->setRTMPNewConnectCallback(
      std::bind(&SnyRTMPProxy::OnRTMPNewConnectCallback, this, std::placeholders::_1, std::placeholders::_2));
  sny_rtmp_server_->setRTMPConnectCloseCallback(
      std::bind(&SnyRTMPProxy::OnRTMPConnectCloseCallback, this, std::placeholders::_1));
  auto result = sny_rtmp_server_->start();
  if (sny::SnySuccess != result) {
    return result;
  }

  return sny::SnySuccess;
}

void SnyRTMPProxy::stop() {
  if (sny_rtmp_server_) {
    sny_rtmp_server_->stop();
  }
}

void SnyRTMPProxy::OnRTMPNewConnectCallback(const weak_ptr<uv::TcpConnection>& conn, const string& conn_name) {

  std::lock_guard lk(mutex_);
  auto rtmp_publish = std::make_shared<SnyMultiRTMPPublish>(conn_name);
  rtmp_publish->setConfigure(cnf_);
  sny_multi_rtmp_publish_.insert(std::make_pair(conn_name, rtmp_publish));

}

void SnyRTMPProxy::OnRTMPConnectCloseCallback(const string& conn_name) {
  
  std::lock_guard lk(mutex_);
  auto item = sny_multi_rtmp_publish_.find(conn_name);
  if (item != sny_multi_rtmp_publish_.end()) {
    item->second->stop();
  }
  sny_multi_rtmp_publish_.erase(conn_name);

}

void SnyRTMPProxy::onRtmpAppStreamName(const std::string& conn_name, const std::string& app_name,
                                       const std::string& stream_name) {
  std::lock_guard lk(mutex_);
  auto item = sny_multi_rtmp_publish_.find(conn_name);
  if (item != sny_multi_rtmp_publish_.end()) {
    item->second->onRtmpAppStreamName(conn_name, app_name, stream_name);
  }

}
void SnyRTMPProxy::onTrack(const std::string& conn_name, const std::map<int32_t, std::shared_ptr<MediaTrack>>& tracks) {
 
  std::lock_guard lk(mutex_);
  auto item = sny_multi_rtmp_publish_.find(conn_name);
  if (item != sny_multi_rtmp_publish_.end()) {
    item->second->onTrack(conn_name, tracks);
  }

}
void SnyRTMPProxy::onSample(const std::string& conn_name, const std::shared_ptr<sny::SnyMediaSample> sample) {
  
  std::shared_ptr<SnyMultiRTMPPublish> rtmp_publish = nullptr;
  {
    std::lock_guard lk(mutex_);
    auto item = sny_multi_rtmp_publish_.find(conn_name);
    if (item != sny_multi_rtmp_publish_.end()) {
      rtmp_publish = item->second;
    }
  }

  if (rtmp_publish) {
    rtmp_publish->onSample(conn_name, sample);
  }

}
}  // namespace app