/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snygpacmp4.h"
#include "core/snyconfig.h"
#include "core/snyconstants.h"
#include "core/snygpacmediainfo.h"
#include "core/snyresults.h"
#include "core/snyutils.h"
#include "easylogging++.h"

namespace sny {
SnyGpacMp4::SnyGpacMp4() {
  uri_ = "";
  ptr_gf_isofile_ = nullptr;
  return;
}

SnyGpacMp4::~SnyGpacMp4() {
  uri_ = "";
  if (ptr_gf_isofile_ != nullptr) {
    gf_isom_close(ptr_gf_isofile_);
    ptr_gf_isofile_ = nullptr;
  }
  while (!selected_tracks_.empty()) {
    auto iter = selected_tracks_.begin();
    auto sny_gpac_mp4_track = iter->second;
    delete sny_gpac_mp4_track;
    selected_tracks_.erase(iter);
  }
  return;
}

void SnyGpacMp4::setUri(std::string uri) {
  LOG(DEBUG) << uri;
  uri_ = uri;
}

SnyResult SnyGpacMp4::open() {
  if ((ptr_gf_isofile_ =
           gf_isom_open(uri_.c_str(), GF_ISOM_OPEN_READ, nullptr)) == nullptr) {
    LOG(ERROR) << "Failed to open " << uri_;
    return SnyFailture;
  }
  return SnySuccess;
}

GpacMediaInfo SnyGpacMp4::getMediaInfo() {
  if (ptr_gf_isofile_ == nullptr) {
    GpacMediaInfo mediainfo(uri_, uri_);
    return mediainfo;
  }
  u32 file_timescale = gf_isom_get_timescale(ptr_gf_isofile_);
  u64 file_duration = gf_isom_get_duration(ptr_gf_isofile_);
  u64 file_size = gf_isom_get_file_size(ptr_gf_isofile_);
  u32 track_count = gf_isom_get_track_count(ptr_gf_isofile_);

  u64 duration = 0;
  if (file_duration != 0 || file_timescale != 0) {
    duration = file_duration * 1000 / file_timescale;
  }

  GpacMediaInfo mediainfo(uri_, uri_, duration, file_size, "");

  for (u32 i = 0; i < track_count; i++) {
    const u32 track_number = i + 1;
    const u32 track_id = gf_isom_get_track_id(ptr_gf_isofile_, track_number);
    const u32 media_type =
        gf_isom_get_media_type(ptr_gf_isofile_, track_number);
    const u32 media_subtype =
        gf_isom_get_media_subtype(ptr_gf_isofile_, track_number, 1);
    const u32 mpeg4_subtype =
        gf_isom_get_mpeg4_subtype(ptr_gf_isofile_, track_number, 1);
    const u64 media_duration =
        gf_isom_get_media_duration(ptr_gf_isofile_, track_number);
    const u64 media_timescale =
        gf_isom_get_media_timescale(ptr_gf_isofile_, track_number);
    duration = 0;
    if (media_duration != 0 && media_timescale != 0) {
      duration = media_duration * 1000 / media_timescale;
    }
    if (media_type == GF_ISOM_MEDIA_AUDIO &&
        mpeg4_subtype == GF_4CC('m', 'p', '4', 'a')) {
      char *lang = nullptr;
      std::string language = "";
      GF_Err e =
          gf_isom_get_media_language(ptr_gf_isofile_, track_number, &lang);
      if (e != GF_OK) {
        language = kLanguageUndefine;
      } else {
        language = std::string(lang);
      }
      std::string lan = language;
      GpacAudioTrackInfo *audio = new GpacAudioTrackInfo(
          track_id, track_number, duration, kMediaTypeAudio, kCodecAac, lan);
      mediainfo.addAudioTrackInfo(audio);
    }
    if (media_type == GF_ISOM_MEDIA_VISUAL &&
        media_subtype == GF_ISOM_SUBTYPE_AVC_H264) {
      GpacVideoTrackInfo *video = new GpacVideoTrackInfo(
          track_id, track_number, duration, kMediaTypeVideo, kCodecH264);
      mediainfo.addVideoTrackInfo(video);
    }
  }
  return mediainfo;
}

SnyResult SnyGpacMp4::selectTrack(int track_number) {
  if (ptr_gf_isofile_ == nullptr) {
    return SnyFailture;
  }

  std::map<int, SnyGpacMp4Track *>::iterator it =
      selected_tracks_.find(track_number);
  if (it != selected_tracks_.end()) {
    return SnySuccess;
  }

  u32 nalu_extract_mode =
      gf_isom_get_nalu_extract_mode(ptr_gf_isofile_, track_number);
  nalu_extract_mode = nalu_extract_mode | GF_ISOM_NALU_EXTRACT_TILE_ONLY |
                      GF_ISOM_NALU_EXTRACT_ANNEXB_FLAG |
                      GF_ISOM_NALU_EXTRACT_INBAND_PS_FLAG;
  GF_Err e = gf_isom_set_nalu_extract_mode(ptr_gf_isofile_, track_number,
                                           nalu_extract_mode);
  if (e != GF_OK) {
    return SnyFailture;
  }

  SnyBool is_valid_track = false;
  const u32 track_id = gf_isom_get_track_id(ptr_gf_isofile_, track_number);
  const u32 media_type = gf_isom_get_media_type(ptr_gf_isofile_, track_number);
  const u32 media_subtype =
      gf_isom_get_media_subtype(ptr_gf_isofile_, track_number, 1);
  const u32 mpeg4_subtype =
      gf_isom_get_mpeg4_subtype(ptr_gf_isofile_, track_number, 1);
  const u32 media_timescale =
      gf_isom_get_media_timescale(ptr_gf_isofile_, track_number);
  const u32 sample_count =
      gf_isom_get_sample_count(ptr_gf_isofile_, track_number);
  SnyMediaType type = kMediaTypeUnknown;
  GF_DecoderConfig *dcfg = nullptr;
  if (media_type == GF_ISOM_MEDIA_AUDIO &&
      mpeg4_subtype == GF_4CC('m', 'p', '4', 'a')) {
    is_valid_track = true;
    type = kMediaTypeAudio;
    dcfg = gf_isom_get_decoder_config(ptr_gf_isofile_, track_number, 1);
  }
  if (media_type == GF_ISOM_MEDIA_VISUAL &&
      media_subtype == GF_ISOM_SUBTYPE_AVC_H264) {
    is_valid_track = true;
    type = kMediaTypeVideo;
  }
  if (is_valid_track) {
    SnyGpacMp4Track *track = new SnyGpacMp4Track(
        type, track_id, track_number, dcfg, media_timescale, sample_count);
    SnyResult result = track->parse();
    if (result != SnySuccess) {
      delete track;
      track = nullptr;
      return result;
    }
    selected_tracks_.insert(std::make_pair(track_number, track));
  }
  return SnySuccess;
}

SnyResult SnyGpacMp4::seek(SnyUI64 position_us) {
  for (auto it = selected_tracks_.begin(); it != selected_tracks_.end(); it++) {
    SnyResult result = seek(it->first, position_us);
    if (result != SnySuccess) {
      return SnyFailture;
    }
  }
  return SnySuccess;
}

SnyResult SnyGpacMp4::seek(int track_number, SnyUI64 position_us) {
  if (ptr_gf_isofile_ == nullptr) {
    LOG(ERROR) << uri_ << " ,file not Opened";
    return SnyFailture;
  }

  if (selected_tracks_.find(track_number) == selected_tracks_.end()) {
    return SnyFailture;
  }

  const u32 media_timescale =
      gf_isom_get_media_timescale(ptr_gf_isofile_, track_number);
  u64 desired_time = SnyUtils::convertTime(position_us, kTimescaleMicrosecond,
                                           media_timescale);
  u32 stream_desc_index;
  GF_ISOSample *sample = nullptr;
  u32 num;
  GF_Err e = gf_isom_get_sample_for_media_time(
      ptr_gf_isofile_, track_number, desired_time, &stream_desc_index,
      GF_ISOM_SEARCH_FORWARD, &sample, &num);
  if (e == GF_EOS) {
    selected_tracks_[track_number]->end_of_track = true;
    return SnyFailture;
  }
  selected_tracks_[track_number]->next_sample_number = num;
  selected_tracks_[track_number]->end_of_track = false;
  return SnySuccess;
}

SnyBool SnyGpacMp4::reachEndOfFile() { return false; }

SnyBool SnyGpacMp4::reachEndOfTrack(int track_number) {
  if (selected_tracks_.find(track_number) == selected_tracks_.end()) {
    return true;
  }
  return selected_tracks_[track_number]->reachEnd();
}

SnySample *SnyGpacMp4::readSample(int track_number) {
  SnySample *sample = nullptr;
  if (ptr_gf_isofile_ == nullptr) {
    LOG(ERROR) << "File not Opened";
    return sample;
  }

  if (selected_tracks_.find(track_number) == selected_tracks_.end()) {
    LOG(ERROR) << "Track " << track_number << "is Not Selected";
    return sample;
  }

  SnyGpacMp4Track *track = selected_tracks_[track_number];
  GF_ISOSample *iso_sample;
  u32 sample_description_index;
  iso_sample =
      gf_isom_get_sample(ptr_gf_isofile_, track_number,
                         track->next_sample_number, &sample_description_index);
  track->next_sample_number++;
  if (iso_sample != nullptr) {
    SnyBool sync = iso_sample->IsRAP;
    SnyUI64 dts_us = SnyUtils::convertTime(iso_sample->DTS, track->time_scale,
                                           kTimescaleMicrosecond);
    SnyUI64 pts_us =
        SnyUtils::convertTime(iso_sample->DTS + iso_sample->CTS_Offset,
                              track->time_scale, kTimescaleMicrosecond);
    const char *iso_sample_data = iso_sample->data;
    const u32 iso_sample_size = iso_sample->dataLength;
    sample = new SnySample();
    sample->setDts(dts_us);
    sample->setPts(pts_us);
    sample->setKey(sync);

    if (track->type == kMediaTypeAudio) {
      sample->setType(track->type);
      /*assemble ADTS header*/
      GF_BitStream *bs;
      bs = gf_bs_new(nullptr, 0, GF_BITSTREAM_WRITE);
      gf_bs_write_int(bs, 0xFFF, 12);                         /*sync*/
      gf_bs_write_int(bs, (track->aac_mode == 1) ? 1 : 0, 1); /*mpeg2 aac*/
      gf_bs_write_int(bs, 0, 2);                              /*layer*/
      gf_bs_write_int(bs, 1, 1); /* protection_absent*/
      gf_bs_write_int(bs, track->aac_type, 2);
      gf_bs_write_int(bs, track->a_cfg.base_sr_index, 4);
      gf_bs_write_int(bs, 0, 1);
      gf_bs_write_int(bs, track->a_cfg.nb_chan, 3);
      gf_bs_write_int(bs, 0, 4);
      gf_bs_write_int(bs, 7 + iso_sample->dataLength, 13);
      gf_bs_write_int(bs, 0x7FF, 11);
      gf_bs_write_int(bs, 0, 2);
      gf_bs_write_data(bs, iso_sample_data, iso_sample_size);

      char *data = nullptr;
      u32 size = 0;
      if (bs != nullptr) {
        gf_bs_get_content(bs, &data, &size);
        if (data != nullptr && size > 0) {
          sample->setData(data, size);
          gf_free(data);
        }
      }

      gf_bs_del(bs);
    } else if (track->type == kMediaTypeVideo) {
      sample->setType(track->type);
      sample->setData(iso_sample_data, iso_sample_size);
    } else {
      delete sample;
      sample = nullptr;
      LOG(ERROR) << "";
    }

    gf_isom_sample_del(&iso_sample);
  } else {
    selected_tracks_[track_number]->end_of_track = true;
    GF_Err e = gf_isom_last_error(ptr_gf_isofile_);
    const char *error = gf_error_to_string(e);
    LOG(ERROR) << error;
    return sample;
  }
  return sample;
}
}  // namespace sny
