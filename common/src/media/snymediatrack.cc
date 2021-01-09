/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */

#include "snymediatrack.h"
namespace sny{

snymediatrack::snymediatrack(SnyMediaType mediaType, SnyCodecType codecType,
                             SnyInt trackId, SnySI64 trackDuration)
    : media_type_(mediaType),
      codec_type_(codecType),
      track_id_(trackId),
      track_duration_(trackDuration) {}

snymediatrack::~snymediatrack() {}

SnyMediaType snymediatrack::getMediaType() const { return media_type_; }
SnyCodecType snymediatrack::getCodecType() const { return codec_type_; }
SnyInt snymediatrack::getTrackId() const { return track_id_; }
SnySI64 snymediatrack::getTrackDuration() const { return track_duration_; }

}