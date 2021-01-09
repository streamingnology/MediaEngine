/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMEDIATRACK_H
#define STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMEDIATRACK_H
#include "core/snytype.h"
#include "snymediatype.h"
namespace sny{

class snymediatrack {
 public:
  snymediatrack(SnyMediaType mediaType, SnyCodecType codecType, SnyInt trackId,
                SnySI64 trackDuration);
  virtual ~snymediatrack();
  SnyMediaType getMediaType() const;
  SnyCodecType getCodecType() const;
  SnyInt getTrackId() const;
  SnySI64 getTrackDuration() const;

 private:
  SnyMediaType media_type_;
  SnyCodecType codec_type_;
  SnyInt  track_id_;
  SnySI64 track_duration_;
};

}
#endif  // STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMEDIATRACK_H
