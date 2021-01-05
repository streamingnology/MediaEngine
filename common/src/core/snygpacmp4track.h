/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYGPACMP4TRACK_H
#define STREAMINGNOLOGY_CORE_SNYGPACMP4TRACK_H
extern "C" {
#include <gpac/avparse.h>
#include <gpac/constants.h>
#include <gpac/mpeg4_odf.h>
}
#include "core/snyconstants.h"
#include "core/snytype.h"

namespace sny {
class SnyGpacMp4Track {
 public:
  SnyGpacMp4Track(enum SnyMediaType type, SnyInt track_id, SnyInt track_number,
                  GF_DecoderConfig *dcfg, SnyUI32 time_scale,
                  SnyUI32 track_sample_count);
  ~SnyGpacMp4Track();
  SnyResult parse();
  SnyBool reachEnd();

 public:
  enum SnyMediaType type;
  SnyInt track_id;
  SnyInt track_number;
  SnyUI32 next_sample_number;
  SnyUI32 track_sample_count;
  SnyUI32 time_scale;
  SnyBool end_of_track;

  /*AAC*/
  GF_DecoderConfig *dcfg;
  GF_M4ADecSpecInfo a_cfg;
  SnyUI32 aac_mode;
  SnyUI32 aac_type;
};
}  // namespace sny

#endif  // !STREAMINGNOLOGY_CORE_SNYGPACMP4TRACK_H
