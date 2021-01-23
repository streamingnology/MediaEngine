/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "snymediasample.h"

namespace sny {
SnyMediaSample::SnyMediaSample() {
  dts_us_ = 0;
  pts_us_ = 0;
  duration_us_ = 0;
  key_frame_ = true;
  media_type_ = kMediaTypeUnknown;
}

SnyMediaSample::SnyMediaSample(SnyMediaType media_type, SnySI64 dts_us,
                               SnySI64 pts_us, SnySI64 duration_us) {
  this->media_type_ = media_type;
  this->codec_type_ = kCodecUnknown;
  this->bsf_fmt_ = kBitStreamUnknwon;
  this->key_frame_ = false;
  this->dts_us_ = dts_us;
  this->pts_us_ = pts_us;
  this->duration_us_ = duration_us;
  this->media_track_ = nullptr;
}

SnyMediaSample::SnyMediaSample(SnyMediaType media_type, SnyCodecType codec_type,
                               SnyBitStreamFormat bsf_fmt, bool key,
                               SnySI64 dts_us, SnySI64 pts_us, SnySI64 duration_us,
                               std::shared_ptr<MediaTrack>& track) {
  media_type_ = media_type;
  codec_type_ = codec_type;
  bsf_fmt_ = bsf_fmt;
  key_frame_ = key;
  dts_us_ = dts_us;
  pts_us_ = pts_us;
  duration_us_ = duration_us;
  media_track_ = track;
}

SnyMediaSample::~SnyMediaSample() {}

void SnyMediaSample::setMediaType(enum SnyMediaType media_type) { this->media_type_ = media_type; }
enum SnyMediaType SnyMediaSample::getMeidaType() const { return media_type_; }

void SnyMediaSample::setCodecType(SnyCodecType codec_type) { codec_type_ = codec_type; }
SnyCodecType SnyMediaSample::getCodecType() const { return codec_type_; }

const char *SnyMediaSample::data() const { return sny_data_buffer_.data(); }

SnyInt SnyMediaSample::size() const { return sny_data_buffer_.size(); }

const SnyDataBuffer &SnyMediaSample::dataBuffer() const { return sny_data_buffer_; }

SnySI64 SnyMediaSample::dts() const { return dts_us_; }

SnySI64 SnyMediaSample::pts() const { return pts_us_; }

SnySI64 SnyMediaSample::duration() const { return duration_us_; }

bool SnyMediaSample::isKey() const { return key_frame_; }

bool SnyMediaSample::isEmpty() const { return sny_data_buffer_.isEmpty(); }

void SnyMediaSample::setDataBuffer(const SnyDataBuffer &dataBuffer) {
  sny_data_buffer_ = dataBuffer;
}

void SnyMediaSample::setData(const char *data, SnyInt size) {
  sny_data_buffer_.assign(data, size);
}

void SnyMediaSample::setDts(SnySI64 dts_us) { dts_us_ = dts_us; }

void SnyMediaSample::setPts(SnySI64 pts_us) { pts_us_ = pts_us; }

void SnyMediaSample::setDuration(SnySI64 duration_us) { duration_us_ = duration_us; }

void SnyMediaSample::setKey(bool isKey) { key_frame_ = isKey; }

void SnyMediaSample::setMediaTrack(std::shared_ptr<MediaTrack>& track) { media_track_ = track; }

std::shared_ptr<MediaTrack>& SnyMediaSample::getMediaTrack() { return media_track_; }

SnyInt SnyMediaSample::getTrackID() {
  if (media_track_) {
    return media_track_->GetId();
  }
  return -1;
}
}  // namespace sny