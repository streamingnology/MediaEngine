/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snyrtmpproxy.h"
#include <utility>
#include <uv11.hpp>
namespace app {
SnyRTMPProxy::SnyRTMPProxy() {}

SnyRTMPProxy::~SnyRTMPProxy() { stop(); }

sny::SnyResult SnyRTMPProxy::start() {
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

void SnyRTMPProxy::OnRTMPNewConnectCallback(const weak_ptr<uv::TcpConnection>& conn, string& conn_name) {
  mutex_.lock();
  auto rtmp_publish = std::make_shared<SnyMultiRTMPPublish>(conn_name);
  rtmp_publish->setConfigure(cnf_);
  sny_multi_rtmp_publish_.insert(std::make_pair(conn_name, rtmp_publish));
  mutex_.unlock();
}

void SnyRTMPProxy::OnRTMPConnectCloseCallback(string& conn_name) {
  mutex_.lock();
  auto item = sny_multi_rtmp_publish_.find(conn_name);
  if (item != sny_multi_rtmp_publish_.end()) {
    item->second->stop();
  }
  sny_multi_rtmp_publish_.erase(conn_name);
  mutex_.unlock();
}

void SnyRTMPProxy::onRtmpAppStreamName(std::string conn_name, std::string app_name, std::string stream_name) {
  mutex_.lock();
  auto item = sny_multi_rtmp_publish_.find(conn_name);
  if (item != sny_multi_rtmp_publish_.end()) {
    item->second->onRtmpAppStreamName(conn_name, app_name, stream_name);
  }
  mutex_.unlock();
}
void SnyRTMPProxy::onTrack(std::string conn_name, std::map<int32_t, std::shared_ptr<MediaTrack>> tracks) {
  mutex_.lock();
  auto item = sny_multi_rtmp_publish_.find(conn_name);
  if (item != sny_multi_rtmp_publish_.end()) {
    item->second->onTrack(conn_name, tracks);
  }
  mutex_.unlock();
}
void SnyRTMPProxy::onSample(std::string conn_name, std::shared_ptr<sny::SnyMediaSample> sample) {
  mutex_.lock();
  auto item = sny_multi_rtmp_publish_.find(conn_name);
  std::shared_ptr<SnyMultiRTMPPublish> rtmp_publish = nullptr;
  if (item != sny_multi_rtmp_publish_.end()) {
    rtmp_publish = item->second;
  }
  mutex_.unlock();
  if (rtmp_publish) {
    rtmp_publish->onSample(conn_name, sample);
  }
}
}  // namespace app