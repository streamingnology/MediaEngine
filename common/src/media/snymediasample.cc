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
  this->dts_us_ = dts_us;
  this->pts_us_ = pts_us;
  this->duration_us_ = duration_us;
}

SnyMediaSample::SnyMediaSample(const SnyMediaSample &other) {
  dts_us_ = 0;
  pts_us_ = 0;
  duration_us_ = 0;
  key_frame_ = true;
  media_type_ = kMediaTypeUnknown;
  operator=(other);
}

SnyMediaSample::~SnyMediaSample() {}

enum SnyMediaType SnyMediaSample::type() const { return media_type_; }

const char *SnyMediaSample::data() const { return sny_data_buffer_.data(); }

SnyInt SnyMediaSample::size() const { return sny_data_buffer_.size(); }

const SnyDataBuffer &SnyMediaSample::dataBuffer() const { return sny_data_buffer_; }

SnySI64 SnyMediaSample::dts() const { return dts_us_; }

SnySI64 SnyMediaSample::pts() const { return pts_us_; }

SnySI64 SnyMediaSample::duration() const { return duration_us_; }

bool SnyMediaSample::isKey() const { return key_frame_; }

bool SnyMediaSample::isEmpty() const { return sny_data_buffer_.isEmpty(); }

void SnyMediaSample::setType(enum SnyMediaType type) { this->media_type_ = type; }

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

void SnyMediaSample::operator=(const SnyMediaSample &other) {
  media_type_ = other.media_type_;
  sny_data_buffer_ = other.sny_data_buffer_;
  dts_us_ = other.dts_us_;
  pts_us_ = other.pts_us_;
  duration_us_ = other.duration_us_;
  key_frame_ = other.key_frame_;
}
}  // namespace sny