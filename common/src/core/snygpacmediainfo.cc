/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snygpacmediainfo.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace sny {
GpacAudioTrackInfo::GpacAudioTrackInfo(SnyUI32 track_id, SnyUI32 track_number,
                                       SnySI64 track_duration_ms,
                                       enum SnyMediaType sny_track_type,
                                       enum SnyCodec sny_track_codec,
                                       std::string language) {
  this->track_id_ = track_id;
  this->track_number_ = track_number;
  this->track_duration_ms_ = track_duration_ms;
  this->sny_track_type_ = sny_track_type;
  this->sny_track_codec_ = sny_track_codec;
  this->track_language_ = language;
}

GpacAudioTrackInfo::~GpacAudioTrackInfo() {}

GpacAudioTrackInfo::GpacAudioTrackInfo(const GpacAudioTrackInfo &object) {
  operator=(object);
}

void GpacAudioTrackInfo::operator=(const GpacAudioTrackInfo &object) {
  this->track_id_ = object.track_id_;
  this->track_number_ = object.track_number_;
  this->track_duration_ms_ = object.track_duration_ms_;
  this->sny_track_type_ = object.sny_track_type_;
  this->sny_track_codec_ = object.sny_track_codec_;
  this->track_language_ = object.track_language_;
}

GpacVideoTrackInfo::GpacVideoTrackInfo(SnyUI32 track_id, SnyUI32 track_number,
                                       SnySI64 track_duration_ms,
                                       enum SnyMediaType sny_track_type,
                                       enum SnyCodec sny_track_codec) {
  this->track_id_ = track_id;
  this->track_number_ = track_number;
  this->track_duration_ms_ = track_duration_ms;
  this->sny_track_type_ = sny_track_type;
  this->sny_track_codec_ = sny_track_codec;
}

GpacVideoTrackInfo::~GpacVideoTrackInfo() {}

GpacVideoTrackInfo::GpacVideoTrackInfo(const GpacVideoTrackInfo &object) {
  operator=(object);
}

void GpacVideoTrackInfo::operator=(const GpacVideoTrackInfo &object) {
  this->track_id_ = object.track_id_;
  this->track_number_ = object.track_number_;
  this->track_duration_ms_ = object.track_duration_ms_;
  this->sny_track_type_ = object.sny_track_type_;
  this->sny_track_codec_ = object.sny_track_codec_;
}

GpacMediaInfo::GpacMediaInfo() {
  this->media_file_name_ = "";
  this->media_file_path_ = "";
  this->media_file_duration_ms_ = 0;
  this->media_file_size_ = 0;
  this->media_file_error_ = "";
}

GpacMediaInfo::GpacMediaInfo(std::string name, std::string path) {
  GpacMediaInfo(name, path, 0, 0, "");
}

GpacMediaInfo::GpacMediaInfo(std::string name, std::string path,
                             SnyUI64 duration_ms, SnyUI64 size,
                             std::string error) {
  this->media_file_name_ = name;
  this->media_file_path_ = path;
  this->media_file_duration_ms_ = duration_ms;
  this->media_file_size_ = size;
  this->media_file_error_ = error;
}

GpacMediaInfo::~GpacMediaInfo() {
  while (!audio_track_infos_.empty()) {
    GpacAudioTrackInfo *audio = audio_track_infos_.back();
    delete audio;
    audio_track_infos_.pop_back();
  }
  while (!video_track_infos_.empty()) {
    GpacVideoTrackInfo *video = video_track_infos_.back();
    delete video;
    video_track_infos_.pop_back();
  }
}

GpacMediaInfo::GpacMediaInfo(const GpacMediaInfo &object) { operator=(object); }

void GpacMediaInfo::operator=(const GpacMediaInfo &object) {
  this->media_file_name_ = object.media_file_name_;
  this->media_file_path_ = object.media_file_path_;
  this->media_file_duration_ms_ = object.media_file_duration_ms_;
  this->media_file_size_ = object.media_file_size_;
  this->media_file_error_ = object.media_file_error_;

  for (auto iter = object.audio_track_infos_.begin();
       iter != object.audio_track_infos_.end(); iter++) {
    auto trackInfo = new GpacAudioTrackInfo(**iter);
    this->audio_track_infos_.push_back(trackInfo);
  }

  for (auto iter = object.video_track_infos_.begin();
       iter != object.video_track_infos_.end(); iter++) {
    auto trackInfo = new GpacVideoTrackInfo(**iter);
    this->video_track_infos_.push_back(trackInfo);
  }
}

void GpacMediaInfo::addAudioTrackInfo(
    GpacAudioTrackInfo *ptr_audio_track_info) {
  audio_track_infos_.push_back(ptr_audio_track_info);
}

void GpacMediaInfo::addVideoTrackInfo(
    GpacVideoTrackInfo *ptr_video_track_info) {
  video_track_infos_.push_back(ptr_video_track_info);
}

std::string GpacMediaInfo::toJSON() {
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  writer.StartObject();
  writer.Key(kMediaInfoName.c_str());
  writer.String(media_file_name_.c_str());
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
    writer.Uint(audio_track_infos_[i]->track_id_);
    writer.Key(kMediaInfoTrackNumber.c_str());
    writer.Uint(audio_track_infos_[i]->track_number_);
    writer.Key(kMediaInfoTrackType.c_str());
    // writer.String(mAudioTrackInfos[i]->mTrackType.c_str());
    writer.Key(kMediaInfoTrackCodec.c_str());
    // writer.String(mAudioTrackInfos[i]->mTrackCodec.c_str());
    writer.Key(kMediaInfoTrackLanguage.c_str());
    writer.String(audio_track_infos_[i]->track_language_.c_str());
    writer.Key(kMediaInfoDuration.c_str());
    writer.Uint64(audio_track_infos_[i]->track_duration_ms_);
    writer.EndObject();
  }
  size_t videoTrackCnt = video_track_infos_.size();
  for (size_t i = 0; i < videoTrackCnt; i++) {
    writer.StartObject();
    writer.Key(kMediaInfoTrackId.c_str());
    writer.Uint(video_track_infos_[i]->track_id_);
    writer.Key(kMediaInfoTrackNumber.c_str());
    writer.Uint(video_track_infos_[i]->track_number_);
    writer.Key(kMediaInfoTrackType.c_str());
    // writer.String(mVideoTrackInfos[i]->mTrackType.c_str());
    writer.Key(kMediaInfoTrackCodec.c_str());
    // writer.String(mVideoTrackInfos[i]->mTrackCodec.c_str());
    writer.Key(kMediaInfoDuration.c_str());
    writer.Uint64(video_track_infos_[i]->track_duration_ms_);
    writer.EndObject();
  }
  writer.EndArray();

  writer.EndObject();
  std::string JSON = sb.GetString();
  return JSON;
}
}  // namespace sny