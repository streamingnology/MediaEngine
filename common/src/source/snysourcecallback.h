/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <memory>
#include <string>
#include "media/snymediasample.h"
namespace sny {
class SnySourceCallback {
 public:
  SnySourceCallback() = default;
  virtual ~SnySourceCallback() = default;
  virtual void onRtmpAppStreamName(std::string app_name, std::string stream_name) = 0;
  virtual void onTrack(std::map<int32_t, std::shared_ptr<MediaTrack>> tracks) = 0;
  virtual void onSample(std::shared_ptr<sny::SnyMediaSample> sample) = 0;
};
}  // namespace sny