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
  virtual void onRtmpAppStreamName(const std::string& conn_name, const std::string& app_name,
                                   const std::string& stream_name) = 0;
  virtual void onTrack(const std::string& conn_name, const std::map<int32_t, std::shared_ptr<MediaTrack>>& tracks) = 0;
  virtual void onSample(const std::string& conn_name, const std::shared_ptr<sny::SnyMediaSample> sample) = 0;
};
}  // namespace sny