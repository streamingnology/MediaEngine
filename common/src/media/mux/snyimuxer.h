/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <memory>
#include <string>
#include "media/media_track.h"
#include "media/snymediasample.h"

namespace sny {
class SnyIMuxer {
 public:
  enum MuxOutType {
    kMuxOutHardDrive = 0,
    kMuxOutMemory = 1,
  };
  SnyIMuxer() { mux_out_type_ = kMuxOutHardDrive; }
  virtual ~SnyIMuxer() {}

  virtual void setPath(std::string path, std::string container_fmt) = 0;
  virtual std::string getPath() = 0;

  void setMuxOutType(MuxOutType type) { mux_out_type_ = type; }
  MuxOutType getMuxOutType() { return mux_out_type_; }

  virtual bool addMediaTrack(const std::shared_ptr<MediaTrack> media_track) = 0;
  virtual const std::map<SnyInt, std::shared_ptr<MediaTrack>>& getMediaTrack() = 0;

  virtual bool open() = 0;
  virtual void close() = 0;

  virtual bool writeSample(std::shared_ptr<SnyMediaSample> sample) = 0;

 private:
  MuxOutType mux_out_type_;
};
}  // namespace sny