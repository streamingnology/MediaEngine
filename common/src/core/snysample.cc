/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snysample.h"

namespace sny {
SnySample::SnySample() {
  dts_ = 0;
  pts_ = 0;
  cts_ = 0;
  key_frame_ = true;
  type_ = kMediaTypeUnknown;
}

SnySample::SnySample(const SnySample &other) {
  dts_ = 0;
  pts_ = 0;
  cts_ = 0;
  key_frame_ = true;
  type_ = kMediaTypeUnknown;
  operator=(other);
}

SnySample::~SnySample() {}

enum SnyMediaType SnySample::type() const { return type_; }

const char *SnySample::data() const { return sny_data_buffer_.data(); }

SnyInt SnySample::size() const { return sny_data_buffer_.size(); }

const SnyDataBuffer &SnySample::dataBuffer() const { return sny_data_buffer_; }

SnySI64 SnySample::dts() const { return dts_; }

SnySI64 SnySample::pts() const { return pts_; }

SnySI64 SnySample::cts() const { return cts_; }

bool SnySample::isKey() const { return key_frame_; }

bool SnySample::isEmpty() const { return sny_data_buffer_.isEmpty(); }

void SnySample::setType(enum SnyMediaType type) { this->type_ = type; }

void SnySample::setDataBuffer(const SnyDataBuffer &dataBuffer) {
  sny_data_buffer_ = dataBuffer;
}

void SnySample::setData(const char *data, SnyInt size) {
  sny_data_buffer_.assign(data, size);
}

void SnySample::setDts(SnySI64 dts) { dts_ = dts; }

void SnySample::setPts(SnySI64 pts) { pts_ = pts; }

void SnySample::setCts(SnySI64 cts) { cts_ = cts; }

void SnySample::setKey(bool isKey) { key_frame_ = isKey; }

void SnySample::operator=(const SnySample &other) {
  type_ = other.type_;
  sny_data_buffer_ = other.sny_data_buffer_;
  dts_ = other.dts_;
  pts_ = other.pts_;
  cts_ = other.cts_;
  key_frame_ = other.key_frame_;
}
}  // namespace sny