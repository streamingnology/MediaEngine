/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snydemuxer.h"
#include "core/snyresults.h"
#include <Ap4.h>
namespace sny{
SnyDemuxer::SnyDemuxer() {
  this->eos_ = false;
  this->uri_ = "";
  this->input_ = nullptr;
  this->input_file_ = nullptr;
  this->linear_reader_ = nullptr;
}

SnyDemuxer::~SnyDemuxer() {
  if (input_file_) {
    delete input_file_; input_file_ = nullptr;
  }
  if (input_) {
    input_->Release();
  }
  if (linear_reader_) {
    delete linear_reader_; linear_reader_ = nullptr;
  }
  for (auto item : sample_readers_) {
    delete item;
  }
  sample_readers_.clear();
}

void SnyDemuxer::setUri(std::string uri) {
  this->uri_ = uri;
}

SnyResult SnyDemuxer::open() {
  AP4_Result result = AP4_FileByteStream::Create(uri_.c_str(), AP4_FileByteStream::STREAM_MODE_READ, input_);
  if (AP4_FAILED(result)) {
    return SnyFailture;
  }
  input_file_ = new AP4_File(*input_, true);
  //AP4_SampleDescription* sample_description;
  AP4_Movie* movie = input_file_->GetMovie();
  if (movie == nullptr) {
    return SnyFailture;
  }
  media_info_ = SnyMediaInfo(uri_, movie->GetDurationMs(), 0, "");
  const int kMaxTrackNumber = 10; //TODO
  for (int i = 0; i < kMaxTrackNumber; i++) {
    AP4_Track* audio_track = movie->GetTrack(AP4_Track::TYPE_AUDIO, i);
    AP4_Track* video_track = movie->GetTrack(AP4_Track::TYPE_VIDEO, i);
    SampleReader* audio_reader;
    SampleReader* video_reader;
    if (audio_track == nullptr && video_track == nullptr) {
      break;
    }

    SnyAudioTrackInfo* audio_track_info = getAudioTrackInfo(audio_track);
    SnyVideoTrackInfo* video_track_info = getVideoTrackInfo(video_track);
    if (movie->HasFragments()) {
      // create a linear reader to get the samples
      if (linear_reader_ == nullptr) {
        linear_reader_ = new AP4_LinearReader(*movie, input_);
      }
      if (audio_track && audio_track_info) {
        linear_reader_->EnableTrack(audio_track->GetId());
        audio_reader = new FragmentedSampleReader(*linear_reader_, *audio_track,
                                                  audio_track->GetId(),
                                                  true);
        sample_readers_.push_back(audio_reader);
      }
      if (video_track && video_track_info) {
        linear_reader_->EnableTrack(video_track->GetId());
        video_reader = new FragmentedSampleReader(*linear_reader_, *video_track,
                                                  video_track->GetId(),
                                                  true);
        sample_readers_.push_back(video_reader);
      }
    } else {
      if (audio_track && audio_track_info) {
        audio_reader = new TrackSampleReader(*audio_track, true);
        sample_readers_.push_back(audio_reader);
      }
      if (video_track && video_track_info) {
        video_reader = new TrackSampleReader(*video_track, true);
        sample_readers_.push_back(video_reader);
      }
    }
    if (audio_track_info){
      media_info_.addAudioTrackInfo(*audio_track_info);
      delete audio_track_info;
    }
    if (video_track_info) {
      media_info_.addVideoTrackInfo(*video_track_info);
      delete video_track_info;
    }
  }
  if (sample_readers_.empty() && sample_readers_.empty()) {
    return SnyFailture;
  }
  return SnySuccess;
}

SnyMediaInfo SnyDemuxer::getMediaInfo() {
  return media_info_;
}

SnyResult SnyDemuxer::selectTrack(SnyInt track_id) {
  for (auto iter : sample_readers_) {
    if (iter->GetTrackId() == track_id) {
      iter->Select();
      return SnySuccess;
    }
  }
  return SnyFailture;
}

SnyResult SnyDemuxer::unselectTrack(SnyInt track_id) {
  for (auto iter : sample_readers_) {
    if (iter->GetTrackId() == track_id) {
      iter->UnSelect();
      return SnySuccess;
    }
  }
  return SnyFailture;
}

SnyResult SnyDemuxer::seek(SnyUI64 position_us) {
  for (auto item : sample_readers_) {
    item->Seek(position_us);
  }
  return 0;
}

SnyBool SnyDemuxer::reachEndOfFile() { return this->eos_; }

SnyMediaSample* SnyDemuxer::readSample() {
  SnyMediaSample* sample = nullptr;
  SnySI64 min_dts = SnySI64_MAX;
  SnyInt index = -1, i = 0;
  for (auto iter : sample_readers_) {
    if (iter->IsSelected() && !iter->IsEos()) {
      SnyResult  result = iter->GetSample(sample);
      if (AP4_FAILED(result)) {
        continue;
      } else {
        if (sample != nullptr && sample->dts() < min_dts) {
          index = i;
          min_dts = sample->dts();
        }
      }
      i++;
    }
  }
  if (index != -1) {
    SnyResult result = sample_readers_[index]->ReadSample(sample);
    if (AP4_FAILED(result)) {
      if (sample) {
        delete sample; sample = nullptr;
      }
    }
  } else {
    eos_ = true;
  }
  return sample;
}

SnyAudioTrackInfo* SnyDemuxer::getAudioTrackInfo(AP4_Track* track) {
  SnyAudioTrackInfo* audio_track_info = nullptr;
  if (track) {
    auto sample_description = track->GetSampleDescription(0);
    if (sample_description) {
      do {
        SnyCodecType codec_type;
        if (sample_description->GetFormat() == AP4_SAMPLE_FORMAT_MP4A) {
          codec_type = kCodecAac;
        } else if (sample_description->GetFormat() == AP4_SAMPLE_FORMAT_AC_3 ||
                   sample_description->GetFormat() == AP4_SAMPLE_FORMAT_EC_3) {
          codec_type = kCodecAc3;
        } else {
          //TODO: log unknown codec
          break;
        }
        SnySI64 dur_ms = AP4_ConvertTime(track->GetMediaDuration(), track->GetMediaTimeScale(), kTimescaleMillisecond);
        audio_track_info = new SnyAudioTrackInfo(track->GetId(), dur_ms, kMediaTypeAudio, codec_type, "default");
      } while (false);
    }
  }
  return audio_track_info;
}

SnyVideoTrackInfo* SnyDemuxer::getVideoTrackInfo(AP4_Track* track) {
  SnyVideoTrackInfo* video_track_info = nullptr;
  if (track) {
    auto sample_description = track->GetSampleDescription(0);
    if (sample_description) {
      do {
        SnyCodecType codec_type;
        if (sample_description->GetFormat() == AP4_SAMPLE_FORMAT_AVC1 ||
            sample_description->GetFormat() == AP4_SAMPLE_FORMAT_AVC2 ||
            sample_description->GetFormat() == AP4_SAMPLE_FORMAT_AVC3 ||
            sample_description->GetFormat() == AP4_SAMPLE_FORMAT_AVC4 ||
            sample_description->GetFormat() == AP4_SAMPLE_FORMAT_DVAV ||
            sample_description->GetFormat() == AP4_SAMPLE_FORMAT_DVA1) {
          codec_type = kCodecH264;
        } else if (sample_description->GetFormat() == AP4_SAMPLE_FORMAT_HEV1 ||
                   sample_description->GetFormat() == AP4_SAMPLE_FORMAT_HVC1 ||
                   sample_description->GetFormat() == AP4_SAMPLE_FORMAT_DVHE ||
                   sample_description->GetFormat() == AP4_SAMPLE_FORMAT_DVH1) {
          codec_type = kCodecH265;
        } else {
          //TODO: log unknown codec
          break;
        }
        SnySI64 dur_ms = AP4_ConvertTime(track->GetMediaDuration(), track->GetMediaTimeScale(), kTimescaleMillisecond);
        video_track_info = new SnyVideoTrackInfo(track->GetId(), dur_ms, kMediaTypeAudio, codec_type);
      } while (false);
    }
  }
  return video_track_info;
}
}