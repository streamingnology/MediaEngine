/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMEDIASAMPLE_H
#define STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMEDIASAMPLE_H

#include "core/snyconstants.h"
#include "core/snydatabuffer.h"
#include "core/snytype.h"
#include "media/snymediatype.h"

namespace sny {
class SnyMediaSample {
 public:
  SnyMediaSample();

  SnyMediaSample(SnyMediaType media_type, SnySI64 dts_us, SnySI64 pts_us, SnySI64 duration_us);

  SnyMediaSample(const SnyMediaSample &other);

  virtual ~SnyMediaSample();

 public:
  SnyMediaType type() const;

  const char *data() const;

  SnyInt size() const;

  const SnyDataBuffer &dataBuffer() const;

  SnySI64 dts() const;

  SnySI64 pts() const;

  SnySI64 duration() const;

  SnyBool isKey() const;

  SnyBool isEmpty() const;

  void setType(enum SnyMediaType type);

  void setDataBuffer(const SnyDataBuffer &dataBuffer);

  void setData(const char *data, SnyInt size);

  void setDts(SnySI64 dts_us);

  void setPts(SnySI64 pts_us);

  void setDuration(SnySI64 duration_us);

  void setKey(SnyBool isKey);

  void operator=(const SnyMediaSample &other);

 private:
  SnyMediaType media_type_;
  SnyDataBuffer sny_data_buffer_;
  SnySI64 dts_us_;
  SnySI64 pts_us_;
  SnySI64 duration_us_;
  SnyBool key_frame_;
};
}  // namespace sny

#endif  // !STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMEDIASAMPLE_H
