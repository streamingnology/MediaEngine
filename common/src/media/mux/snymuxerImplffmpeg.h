/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <mutex>
#include <string>
#include "media/snyffmpeg.h"
#include "snyimuxer.h"

static std::atomic_bool g_ffmpeg_initialized = false;
namespace sny {

class SnyMuxerImplFFMpeg : public SnyIMuxer {
 public:
  SnyMuxerImplFFMpeg();
  virtual ~SnyMuxerImplFFMpeg();

  void setPath(std::string path, std::string container_fmt) override;
  std::string getPath() override;

  bool addMediaTrack(const std::shared_ptr<MediaTrack> media_track) override;
  const std::map<SnyInt, std::shared_ptr<MediaTrack>>& getMediaTrack() override;

  bool open() override;
  void close() override;

  bool writeSample(std::shared_ptr<SnyMediaSample> sample) override;

 private:
  bool initAVFmtContext();
  bool initMediaTrack();
  bool internalOpen();

 private:
  std::string path_;
  std::string fmt_;
  AVFormatContext* oc_;
  std::map<SnyInt, std::shared_ptr<MediaTrack>> media_tracks_;  //<MediaTrack.id, std::hsared_ptr<RtmpTrackInfo>>
  std::map<SnyInt, SnyInt> mediatrack_to_avstreamindex_;        //<MediaTrack.id, AVStream.index>
};

}  // namespace sny