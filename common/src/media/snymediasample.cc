/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "snymediasample.h"

namespace sny {
SnyMediaSample::SnyMediaSample() {
  dts_ = 0;
  pts_ = 0;
  duration_ = 0;
  key_frame_ = true;
  media_type_ = kMediaTypeUnknown;
}

SnyMediaSample::SnyMediaSample(SnyMediaType media_type, SnySI64 dts_us, SnySI64 pts_us, SnySI64 duration_us) {
  this->media_type_ = media_type;
  this->codec_type_ = kCodecUnknown;
  this->bsf_fmt_ = kBitStreamUnknwon;
  this->key_frame_ = false;
  this->dts_ = dts_us;
  this->pts_ = pts_us;
  this->duration_ = duration_us;
  this->media_track_ = nullptr;
}

SnyMediaSample::SnyMediaSample(SnyMediaType media_type, SnyCodecType codec_type, SnyBitStreamFormat bsf_fmt, bool key,
                               SnySI64 dts_us, SnySI64 pts_us, SnySI64 duration_us,
                               std::shared_ptr<MediaTrack> &track) {
  media_type_ = media_type;
  codec_type_ = codec_type;
  bsf_fmt_ = bsf_fmt;
  key_frame_ = key;
  dts_ = dts_us;
  pts_ = pts_us;
  duration_ = duration_us;
  media_track_ = track;
  data_buffer_ = nullptr;
}

SnyMediaSample::~SnyMediaSample() {}

void SnyMediaSample::setMediaType(enum SnyMediaType media_type) { this->media_type_ = media_type; }
enum SnyMediaType SnyMediaSample::getMeidaType() const { return media_type_; }

void SnyMediaSample::setCodecType(SnyCodecType codec_type) { codec_type_ = codec_type; }
SnyCodecType SnyMediaSample::getCodecType() const { return codec_type_; }

void SnyMediaSample::setData(const char *data, SnyInt size) {
  if (!data_buffer_) {
    data_buffer_ = std::make_shared<ov::Data>();
  }
  data_buffer_->Append(data, size);
}

const char *SnyMediaSample::data() const {
  if (data_buffer_) {
    return (const char *)data_buffer_->GetData();
  }
  return nullptr;
}

SnyInt SnyMediaSample::size() const {
  if (data_buffer_) {
    return data_buffer_->GetLength();
  }
  return 0;
}

bool SnyMediaSample::isEmpty() const {
  if (data_buffer_) {
    return data_buffer_->IsEmpty();
  }
  return true;
}

void SnyMediaSample::setDataBuffer(std::shared_ptr<ov::Data> &dataBuffer) { data_buffer_ = dataBuffer; }
std::shared_ptr<ov::Data> SnyMediaSample::getDataBuffer() const { return data_buffer_; }

SnySI64 SnyMediaSample::dts() const { return dts_; }
void SnyMediaSample::setDts(SnySI64 dts) { dts_ = dts; }

SnySI64 SnyMediaSample::pts() const { return pts_; }
void SnyMediaSample::setPts(SnySI64 pts) { pts_ = pts; }

SnySI64 SnyMediaSample::duration() const { return duration_; }
void SnyMediaSample::setDuration(SnySI64 duration) { duration_ = duration; }

void SnyMediaSample::setKey(bool isKey) { key_frame_ = isKey; }
bool SnyMediaSample::isKey() const { return key_frame_; }

void SnyMediaSample::setMediaTrack(std::shared_ptr<MediaTrack> &track) { media_track_ = track; }
std::shared_ptr<MediaTrack> &SnyMediaSample::getMediaTrack() { return media_track_; }

SnyInt SnyMediaSample::getTrackID() {
  if (media_track_) {
    return media_track_->GetId();
  }
  return -1;
}
}  // namespace sny