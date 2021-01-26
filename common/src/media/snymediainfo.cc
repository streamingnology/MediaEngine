/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "media/snymediainfo.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <utility>

namespace sny {
SnyAudioTrackInfo::SnyAudioTrackInfo(SnyUI32 track_id, SnySI64 track_duration_ms, SnyMediaType sny_track_type,
                                     SnyCodecType sny_track_codec, std::string language) {
  this->track_id_ = track_id;
  this->track_duration_ms_ = track_duration_ms;
  this->sny_track_mediatype_ = sny_track_type;
  this->sny_track_codectype_ = sny_track_codec;
  this->track_language_ = std::move(language);
}

SnyAudioTrackInfo::~SnyAudioTrackInfo() = default;

SnyAudioTrackInfo::SnyAudioTrackInfo(const SnyAudioTrackInfo& object) { operator=(object); }

SnyAudioTrackInfo& SnyAudioTrackInfo::operator=(const SnyAudioTrackInfo& object) {
  this->track_id_ = object.track_id_;
  this->track_duration_ms_ = object.track_duration_ms_;
  this->sny_track_mediatype_ = object.sny_track_mediatype_;
  this->sny_track_codectype_ = object.sny_track_codectype_;
  this->track_language_ = object.track_language_;
  return *this;
}

SnyVideoTrackInfo::SnyVideoTrackInfo(SnyUI32 track_id, SnySI64 track_duration_ms, SnyMediaType sny_track_type,
                                     SnyCodecType sny_track_codec) {
  this->track_id_ = track_id;
  this->track_duration_ms_ = track_duration_ms;
  this->sny_track_mediatype_ = sny_track_type;
  this->sny_track_codectype_ = sny_track_codec;
}

SnyVideoTrackInfo::~SnyVideoTrackInfo() = default;

SnyVideoTrackInfo::SnyVideoTrackInfo(const SnyVideoTrackInfo& object) { operator=(object); }

SnyVideoTrackInfo& SnyVideoTrackInfo::operator=(const SnyVideoTrackInfo& object) {
  this->track_id_ = object.track_id_;
  this->track_duration_ms_ = object.track_duration_ms_;
  this->sny_track_mediatype_ = object.sny_track_mediatype_;
  this->sny_track_codectype_ = object.sny_track_codectype_;
  return *this;
}

SnyMediaInfo::SnyMediaInfo() {
  this->media_file_path_ = "";
  this->media_file_duration_ms_ = 0;
  this->media_file_size_ = 0;
  this->media_file_error_ = "";
}

SnyMediaInfo::SnyMediaInfo(std::string path, SnyUI64 duration_ms, SnyUI64 size, std::string error) {
  this->media_file_path_ = std::move(path);
  this->media_file_duration_ms_ = duration_ms;
  this->media_file_size_ = size;
  this->media_file_error_ = std::move(error);
}

SnyMediaInfo::~SnyMediaInfo() = default;

SnyMediaInfo::SnyMediaInfo(const SnyMediaInfo& object) { operator=(object); }

SnyMediaInfo& SnyMediaInfo::operator=(const SnyMediaInfo& object) {
  this->media_file_path_ = object.media_file_path_;
  this->media_file_duration_ms_ = object.media_file_duration_ms_;
  this->media_file_size_ = object.media_file_size_;
  this->media_file_error_ = object.media_file_error_;

  for (const auto& trackInfo : object.audio_track_infos_) {
    this->audio_track_infos_.push_back(trackInfo);
  }

  for (const auto& trackInfo : object.video_track_infos_) {
    this->video_track_infos_.push_back(trackInfo);
  }
  return *this;
}

void SnyMediaInfo::addAudioTrackInfo(const SnyAudioTrackInfo& audio_track_info) {
  audio_track_infos_.push_back(audio_track_info);
}

void SnyMediaInfo::addVideoTrackInfo(const SnyVideoTrackInfo& video_track_info) {
  video_track_infos_.push_back(video_track_info);
}

std::string SnyMediaInfo::toJSON() {
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  writer.StartObject();
  writer.Key(kMediaInfoPath.c_str());
  writer.String(media_file_path_.c_str());
  writer.Key(kMediaInfoDuration.c_str());
  writer.Uint64(media_file_duration_ms_);
  writer.Key(kMediaInfoSize.c_str());
  writer.Uint64(media_file_size_);
  writer.Key(kMediaInfoError.c_str());
  writer.String(media_file_error_.c_str());

  writer.Key(kMediaInfoTracks.c_str());
  writer.StartArray();
  size_t audioTrackCnt = audio_track_infos_.size();
  for (size_t i = 0; i < audioTrackCnt; i++) {
    writer.StartObject();
    writer.Key(kMediaInfoTrackId.c_str());
    writer.Uint(audio_track_infos_[i].track_id_);
    writer.Key(kMediaInfoTrackType.c_str());
    // writer.String(mAudioTrackInfos[i]->mTrackType.c_str());
    writer.Key(kMediaInfoTrackCodec.c_str());
    // writer.String(mAudioTrackInfos[i]->mTrackCodec.c_str());
    writer.Key(kMediaInfoTrackLanguage.c_str());
    writer.String(audio_track_infos_[i].track_language_.c_str());
    writer.Key(kMediaInfoDuration.c_str());
    writer.Uint64(audio_track_infos_[i].track_duration_ms_);
    writer.EndObject();
  }
  size_t videoTrackCnt = video_track_infos_.size();
  for (size_t i = 0; i < videoTrackCnt; i++) {
    writer.StartObject();
    writer.Key(kMediaInfoTrackId.c_str());
    writer.Uint(video_track_infos_[i].track_id_);
    writer.Key(kMediaInfoTrackType.c_str());
    // writer.String(mVideoTrackInfos[i]->mTrackType.c_str());
    writer.Key(kMediaInfoTrackCodec.c_str());
    // writer.String(mVideoTrackInfos[i]->mTrackCodec.c_str());
    writer.Key(kMediaInfoDuration.c_str());
    writer.Uint64(video_track_infos_[i].track_duration_ms_);
    writer.EndObject();
  }
  writer.EndArray();

  writer.EndObject();
  std::string JSON = sb.GetString();
  return JSON;
}
}  // namespace sny