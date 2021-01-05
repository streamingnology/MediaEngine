/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYGPACMEDIAINFO_H
#define STREAMINGNOLOGY_CORE_SNYGPACMEDIAINFO_H
#include <string>
#include <vector>
#include "core/snyconstants.h"
#include "core/snytype.h"

namespace sny {
class GpacAudioTrackInfo {
 public:
  GpacAudioTrackInfo(SnyUI32 track_id, SnyUI32 track_number,
                     SnySI64 track_duration_ms,
                     enum SnyMediaType sny_track_type,
                     enum SnyCodec sny_track_codec, std::string language);
  ~GpacAudioTrackInfo();

  GpacAudioTrackInfo(const GpacAudioTrackInfo &object);
  void operator=(const GpacAudioTrackInfo &object);
  SnyUI32 track_id_;
  SnyUI32 track_number_;
  SnyUI64 track_duration_ms_;
  enum SnyMediaType sny_track_type_;
  enum SnyCodec sny_track_codec_;
  std::string track_language_;
};

class GpacVideoTrackInfo {
 public:
  GpacVideoTrackInfo(SnyUI32 track_id, SnyUI32 track_number,
                     SnySI64 track_duration_ms,
                     enum SnyMediaType sny_track_type,
                     enum SnyCodec sny_track_codec);
  ~GpacVideoTrackInfo();

  GpacVideoTrackInfo(const GpacVideoTrackInfo &object);
  void operator=(const GpacVideoTrackInfo &object);
  SnyUI32 track_id_;
  SnyUI32 track_number_;
  SnyUI64 track_duration_ms_;
  enum SnyMediaType sny_track_type_;
  enum SnyCodec sny_track_codec_;
};

class GpacMediaInfo {
 public:
  GpacMediaInfo();
  GpacMediaInfo(std::string name, std::string path);
  GpacMediaInfo(std::string name, std::string path, SnyUI64 duration_ms,
                SnyUI64 size, std::string error);
  ~GpacMediaInfo();

  GpacMediaInfo(const GpacMediaInfo &object);
  void operator=(const GpacMediaInfo &object);

  void addAudioTrackInfo(GpacAudioTrackInfo *ptr_audio_track_info);
  void addVideoTrackInfo(GpacVideoTrackInfo *ptr_video_track_info);
  std::string toJSON();

  std::string media_file_name_;
  std::string media_file_path_;
  SnyUI64 media_file_duration_ms_;
  SnyUI64 media_file_size_;
  std::string media_file_error_;
  std::vector<GpacAudioTrackInfo *> audio_track_infos_;
  std::vector<GpacVideoTrackInfo *> video_track_infos_;
};
}  // namespace sny

#endif  // !STREAMINGNOLOGY_CORE_SNYGPACMEDIAINFO_H
