/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include <chrono>
#include <thread>
#include "snyrtmpproxy.h"
#include <easylogging++.h>
namespace app {
SnyRTMPProxy::SnyRTMPProxy(std::string name,
                           std::shared_ptr<pvd::RtmpStream> rtmp_stream):threads_(this) {
  name_ = name;
  rtmp_stream_ = rtmp_stream;
  rtmp_stream_->setRTMPCallback(this);
}

SnyRTMPProxy::~SnyRTMPProxy() {
  stop();
}

void SnyRTMPProxy::start() {
  for (int i = 0; i < publish_streams_.size(); i++) {
    threads_.start(i);
  }
}

void SnyRTMPProxy::stop() {
  threads_.stopAll();
  threads_.waitAll();
}

int SnyRTMPProxy::onThreadProc(int id) {
  std::string publish_name = publish_streams_[id].publish_name_;
  std::string publish_url  = publish_streams_[id].publish_url_;
  auto rtmp_muxer = createRtmpMuxer(publish_url);
  if (!rtmp_muxer) {
    LOG(ERROR)<<"failed to create muxer for "<<publish_url;
    return id;
  }
  while (threads_.isRunning(id)) {
    mutex_.lock();
    std::shared_ptr<sny::SnyMediaSample> sample = nullptr;
    if (!samples[id].empty()) {
      sample = samples[id].front();
      samples[id].pop_front();
    }
    mutex_.unlock();
    if (sample) {
      rtmp_muxer->PutData(sample);
    } else {
      std::this_thread::sleep_for(5ms);
    }
  }
  return id;
}

void SnyRTMPProxy::onRtmpAppStreamName(std::string app_name, std::string stream_name) {
  LOG(DEBUG)<<"/"<<app_name<<"/"<<stream_name;
  app_name_ = app_name;
  stream_name_ = stream_name;
}

void SnyRTMPProxy::onTrack(std::map<int32_t, std::shared_ptr<MediaTrack>> tracks) {
  tracks_ = tracks;
  auto& streams = cnf_->streams_;
  for (auto& item : cnf_->streams_) {
    if (item.app_name_==app_name_ && item.stream_name_==stream_name_) {
      publish_streams_ = item.publish_items_;
      break;
    }
  }
  for (int i = 0; i < publish_streams_.size(); i++) {
    std::deque<std::shared_ptr<sny::SnyMediaSample>> q;
    samples.insert(std::make_pair(i, q));
  }
  start();
}

void SnyRTMPProxy::onSample(std::shared_ptr<sny::SnyMediaSample> sample) {
  mutex_.lock();
  for (auto& item : samples) {
    item.second.push_back(sample);
  }
  mutex_.unlock();
}

void SnyRTMPProxy::OnDataReceived(const char *data_buff, ssize_t data_size) {
  rtmp_stream_->OnDataReceived(data_buff, data_size);
}

std::shared_ptr<RtmpWriter> SnyRTMPProxy::createRtmpMuxer(std::string url) {
  auto muxer = std::make_shared<RtmpWriter>();
  ov::String path(url.c_str());
  muxer->SetPath(path,"flv");
  for (auto item : tracks_) {
    auto &track = item.second;
    auto quality = RtmpTrackInfo::Create();
    quality->SetCodecId( track->GetCodecId() );
    quality->SetBitrate( track->GetBitrate() );
    quality->SetTimeBase( track->GetTimeBase() );
    quality->SetWidth( track->GetWidth() );
    quality->SetHeight( track->GetHeight() );
    quality->SetSample( track->GetSample() );
    quality->SetChannel( track->GetChannel() );
    quality->SetExtradata(track->GetCodecExtradata() );
    muxer->AddTrack(track->GetMediaType(), track->GetId(), quality);
  }
  muxer->Start();
  return muxer;
}
}