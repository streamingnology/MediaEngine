/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snygpacmp4track.h"
#include "core/snyresults.h"
namespace sny {
SnyGpacMp4Track::SnyGpacMp4Track(enum SnyMediaType type, SnyInt track_id,
                                 SnyInt track_number, GF_DecoderConfig *dcfg,
                                 SnyUI32 time_scale,
                                 SnyUI32 track_sample_count) {
  this->type = type;
  this->track_id = track_id;
  this->track_number = track_number;
  this->dcfg = dcfg;
  this->time_scale = time_scale;
  this->track_sample_count = track_sample_count;
  this->next_sample_number = 1;
  this->end_of_track = false;
}

SnyGpacMp4Track::~SnyGpacMp4Track() {
  if (dcfg != nullptr) {
    gf_odf_desc_del((GF_Descriptor *)dcfg);
  }
}

SnyResult SnyGpacMp4Track::parse() {
  if (type == kMediaTypeAudio) {
    if (dcfg == nullptr) {
      return SnyFailture;
    } else {
      u32 dsi_size = 0;
      char *dsi = nullptr;
      switch (dcfg->objectTypeIndication) {
        case GPAC_OTI_AUDIO_AAC_MPEG2_MP:
        case GPAC_OTI_AUDIO_AAC_MPEG2_LCP:
        case GPAC_OTI_AUDIO_AAC_MPEG2_SSRP:
          dsi = dcfg->decoderSpecificInfo->data;
          dcfg->decoderSpecificInfo->data = nullptr;
          dsi_size = dcfg->decoderSpecificInfo->dataLength;
          aac_mode = 1;
          aac_type = dcfg->objectTypeIndication - GPAC_OTI_AUDIO_AAC_MPEG2_MP;
          break;
        case GPAC_OTI_AUDIO_AAC_MPEG4:
          if (!dcfg->decoderSpecificInfo) {
            gf_odf_desc_del((GF_Descriptor *)dcfg);
            return SnyFailture;
          }
          dsi = dcfg->decoderSpecificInfo->data;
          dcfg->decoderSpecificInfo->data = nullptr;
          dsi_size = dcfg->decoderSpecificInfo->dataLength;
          aac_mode = 2;
          break;

        default:
          break;
      }
      if (dsi == nullptr || dsi_size <= 0) {
        return SnyFailture;
      }
      GF_Err e = gf_m4a_get_config(dsi, dsi_size, &a_cfg);
      if (e != GF_OK) {
        return SnyFailture;
      }
    }
  } else if (type == kMediaTypeVideo) {
    return SnySuccess;
  } else {
    return SnyFailture;
  }
  return SnySuccess;
}

SnyBool SnyGpacMp4Track::reachEnd() {
  SnyBool eos_track = end_of_track;
  SnyBool eos_count = next_sample_number > track_sample_count ? true : false;
  return eos_track | eos_count;
}
}  // namespace sny