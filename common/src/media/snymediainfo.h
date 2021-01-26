/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <string>
#include <vector>
#include "core/snyconstants.h"
#include "core/snytype.h"
#include "media/snymediatype.h"

namespace sny {
class SnyAudioTrackInfo {
 public:
  SnyAudioTrackInfo(SnyUI32 track_id, SnySI64 track_duration_ms, SnyMediaType sny_track_type,
                    SnyCodecType sny_track_codec, std::string language);
  ~SnyAudioTrackInfo();

  SnyAudioTrackInfo(const SnyAudioTrackInfo &object);
  SnyAudioTrackInfo &operator=(const SnyAudioTrackInfo &object);
  SnyUI32 track_id_;
  SnyUI64 track_duration_ms_;
  SnyMediaType sny_track_mediatype_;
  SnyCodecType sny_track_codectype_;
  std::string track_language_;
};

class SnyVideoTrackInfo {
 public:
  SnyVideoTrackInfo(SnyUI32 track_id, SnySI64 track_duration_ms, SnyMediaType sny_track_type,
                    SnyCodecType sny_track_codec);
  ~SnyVideoTrackInfo();

  SnyVideoTrackInfo(const SnyVideoTrackInfo &object);
  SnyVideoTrackInfo &operator=(const SnyVideoTrackInfo &object);
  SnyUI32 track_id_;
  SnyUI64 track_duration_ms_;
  SnyMediaType sny_track_mediatype_;
  SnyCodecType sny_track_codectype_;
  SnyInt width_;
  SnyInt height_;
};

class SnyMediaInfo {
 public:
  SnyMediaInfo();
  SnyMediaInfo(std::string path, SnyUI64 duration_ms, SnyUI64 size, std::string error);
  ~SnyMediaInfo();

  SnyMediaInfo(const SnyMediaInfo &object);
  SnyMediaInfo &operator=(const SnyMediaInfo &object);

  void addAudioTrackInfo(const SnyAudioTrackInfo &audio_track_info);
  void addVideoTrackInfo(const SnyVideoTrackInfo &video_track_info);
  std::string toJSON();

  std::string media_file_path_;
  SnyUI64 media_file_duration_ms_;
  SnyUI64 media_file_size_;
  std::string media_file_error_;
  std::vector<SnyAudioTrackInfo> audio_track_infos_;
  std::vector<SnyVideoTrackInfo> video_track_infos_;
};
}  // namespace sny
