/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYSAMPLE_H
#define STREAMINGNOLOGY_CORE_SNYSAMPLE_H

#include "core/snyconstants.h"
#include "core/snydatabuffer.h"
#include "core/snytype.h"

namespace sny {
class SnySample {
 public:
  SnySample();

  SnySample(const SnySample &other);

  virtual ~SnySample();

 public:
  enum SnyMediaType type() const;

  const char *data() const;

  SnyInt size() const;

  const SnyDataBuffer &dataBuffer() const;

  SnySI64 dts() const;

  SnySI64 pts() const;

  SnySI64 cts() const;

  SnyBool isKey() const;

  SnyBool isEmpty() const;

  void setType(enum SnyMediaType type);

  void setDataBuffer(const SnyDataBuffer &dataBuffer);

  void setData(const char *data, SnyInt size);

  void setDts(SnySI64 dts);

  void setPts(SnySI64 pts);

  void setCts(SnySI64 cts);

  void setKey(SnyBool isKey);

  void operator=(const SnySample &other);

 private:
  enum SnyMediaType type_;
  SnyDataBuffer sny_data_buffer_;
  SnySI64 dts_;
  SnySI64 pts_;
  SnySI64 cts_;
  SnyBool key_frame_;
};
}  // namespace sny

#endif  // !STREAMINGNOLOGY_CORE_SNYSAMPLE_H
