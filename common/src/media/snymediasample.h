/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "core/snyconstants.h"
#include "core/snydatabuffer.h"
#include "core/snytype.h"
#include "media/snymediatype.h"
#include "media/media_track.h"

namespace sny {
class SnyMediaSample {
 public:
  [[deprecated]] SnyMediaSample(SnyMediaType media_type, SnySI64 dts_us, SnySI64 pts_us, SnySI64 duration_us);
  SnyMediaSample(SnyMediaType media_type, SnyCodecType codec_type, SnyBitStreamFormat bsf_fmt,
                 bool key, SnySI64 dts_us, SnySI64 pts_us, SnySI64 duration_us,
                 std::shared_ptr<MediaTrack>& track);
  ~SnyMediaSample();

 public:
  void setMediaType(SnyMediaType media_type);
  SnyMediaType getMeidaType() const;
  void setCodecType(SnyCodecType codec_type);
  SnyCodecType getCodecType() const;
  void setBitStreamFormat(SnyBitStreamFormat bsf_fmt) { bsf_fmt_ = bsf_fmt; }
  SnyBitStreamFormat getBitStreamFormat() { return bsf_fmt_; }
  const char *data() const;
  SnyInt size() const;
  const SnyDataBuffer &dataBuffer() const;
  SnySI64 dts() const;
  SnySI64 pts() const;
  SnySI64 duration() const;
  SnyBool isKey() const;
  SnyBool isEmpty() const;
  void setDataBuffer(const SnyDataBuffer &dataBuffer);
  void setData(const char *data, SnyInt size);
  void setDts(SnySI64 dts_us);
  void setPts(SnySI64 pts_us);
  void setDuration(SnySI64 duration_us);
  void setKey(SnyBool isKey);
  void setMediaTrack(std::shared_ptr<MediaTrack>& track);
  std::shared_ptr<MediaTrack>& getMediaTrack();
  SnyInt getTrackID();

 protected:
  SnyMediaSample();

 private:
  SnyMediaType media_type_;
  SnyCodecType codec_type_;
  SnyBitStreamFormat bsf_fmt_;
  SnyDataBuffer sny_data_buffer_;
  SnySI64 dts_us_;
  SnySI64 pts_us_;
  SnySI64 duration_us_;
  SnyBool key_frame_;
  std::shared_ptr<MediaTrack> media_track_;
};
}  // namespace sny
