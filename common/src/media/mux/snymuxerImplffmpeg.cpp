/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snymuxerImplffmpeg.h"
#include "core/snyeasylogging.h"
#include "media/snyffmpeg.h"
namespace sny {
SnyMuxerImplFFMpeg::SnyMuxerImplFFMpeg() {
  path_ = "";
  fmt_ = "";
  oc_ = nullptr;
}

SnyMuxerImplFFMpeg::~SnyMuxerImplFFMpeg() { close(); }

void SnyMuxerImplFFMpeg::setPath(std::string path, std::string container_fmt) {
  this->path_ = path;
  this->fmt_ = container_fmt;
}

std::string SnyMuxerImplFFMpeg::getPath() { return this->path_; }

bool SnyMuxerImplFFMpeg::addMediaTrack(const std::shared_ptr<MediaTrack> media_track) {
  media_tracks_.insert(std::make_pair(media_track->GetId(), media_track));
  return true;
}

const std::map<SnyInt, std::shared_ptr<MediaTrack>> &SnyMuxerImplFFMpeg::getMediaTrack() {
  // TODO: insert return statement here
  return media_tracks_;
}

bool SnyMuxerImplFFMpeg::open() {
  if (!initialize_ffmpeg()) {
    return false;
  }
  if (!initAVFmtContext()) {
    return false;
  }
  if (!initMediaTrack()) {
    return false;
  }
  if (!internalOpen()) {
    return false;
  }
  return true;
}

void SnyMuxerImplFFMpeg::close() {
  if (oc_ != nullptr) {
    if (oc_->pb != nullptr) {
      av_write_trailer(oc_);
    }
    avformat_close_input(&oc_);
    avformat_free_context(oc_);
    oc_ = nullptr;
  }
  return;
}

bool SnyMuxerImplFFMpeg::writeSample(std::shared_ptr<SnyMediaSample> sample) {
  if (oc_ == nullptr) {
    return false;
  }
  int stream_index;
  auto iter = mediatrack_to_avstreamindex_.find(sample->getTrackID());
  if (iter == mediatrack_to_avstreamindex_.end()) {
    auto log = ov::String::FormatString("There is no track id %d. Ignore", sample->getTrackID());
    LOG(WARNING) << log;
    return true;
  }
  stream_index = iter->second;
  AVStream *stream = oc_->streams[stream_index];
  if (stream == nullptr) {
    LOG(WARNING) << "There is no stream";
    return false;
  }

  auto track_info = media_tracks_[sample->getTrackID()];
  AVPacket pkt = {nullptr};
  av_init_packet(&pkt);
  pkt.stream_index = stream_index;
  pkt.flags = sample->isKey() ? AV_PKT_FLAG_KEY : 0;
  pkt.pts =
      av_rescale_q(sample->pts(), AVRational{track_info->GetTimeBase().GetNum(), track_info->GetTimeBase().GetDen()},
                   stream->time_base);
  pkt.dts =
      av_rescale_q(sample->dts(), AVRational{track_info->GetTimeBase().GetNum(), track_info->GetTimeBase().GetDen()},
                   stream->time_base);

  // TODO: Depending on the extension, the bitstream format should be changed.
  // format(mpegts)
  // 	- H264 : Passthrough(AnnexB)
  //  - H265 : Passthrough(AnnexB)
  //  - AAC : Passthrough(ADTS)
  //  - OPUS : Passthrough (?)
  //  - VP8 : Passthrough (unknown name)
  // format(flv)
  //	- H264 : AnnexB -> AVCC
  //	- AAC : to LATM
  // format(mp4)
  //	- H264 : Passthrough
  //	- AAC : to LATM
  if (sample->getBitStreamFormat() == sny::kBitStreamH264AVCC ||
      sample->getBitStreamFormat() == sny::kBitStreamAACLATM) {
    pkt.size = sample->size();
    pkt.data = (uint8_t *)sample->data();
  }
  int error = av_write_frame(oc_, &pkt);
  if (error != 0) {
    char errbuf[256];
    av_strerror(error, errbuf, sizeof(errbuf));
    auto log = ov::String::FormatString("Send packet error(%d:%s)", error, errbuf);
    LOG(WARNING) << log;
    return false;
  }

  return true;
}

bool SnyMuxerImplFFMpeg::initAVFmtContext() {
  bool success = false;
  do {
    if (path_.empty()) {
      LOG(ERROR) << "invalid empty path.";
      break;
    }

    AVOutputFormat *output_format = nullptr;
    if (!fmt_.empty()) {
      output_format = av_guess_format(fmt_.c_str(), nullptr, nullptr);
      if (!output_format) {
        LOG(ERROR) << "Unknown format. format(" << fmt_ << ")";
        break;
      }
    }
    int error = avformat_alloc_output_context2(&oc_, output_format, nullptr, path_.c_str());
    if (error < 0) {
      char errbuf[256];
      av_strerror(error, errbuf, sizeof(errbuf));
      auto log = ov::String::FormatString("Could not create output context. error(%d:%s), path(%s)", error, errbuf,
                                          path_.c_str());
      LOG(ERROR) << log;
      break;
    }
    success = true;
  } while (false);
  return success;
}

bool SnyMuxerImplFFMpeg::initMediaTrack() {
  AVStream *stream;
  for (auto &item : media_tracks_) {
    auto &track_info = item.second;
    switch (track_info->GetMediaType()) {
      case cmn::MediaType::Video: {
        stream = avformat_new_stream(oc_, nullptr);
        AVCodecParameters *codecpar = stream->codecpar;
        codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        codecpar->codec_id =
            (track_info->GetCodecId() == cmn::MediaCodecId::H264)
                ? AV_CODEC_ID_H264
                : (track_info->GetCodecId() == cmn::MediaCodecId::H265)
                      ? AV_CODEC_ID_H265
                      : (track_info->GetCodecId() == cmn::MediaCodecId::Vp8)
                            ? AV_CODEC_ID_VP8
                            : (track_info->GetCodecId() == cmn::MediaCodecId::Vp9) ? AV_CODEC_ID_VP9 : AV_CODEC_ID_NONE;
        codecpar->codec_tag = 0;
        codecpar->bit_rate = track_info->GetBitrate();
        codecpar->width = track_info->GetWidth();
        codecpar->height = track_info->GetHeight();
        codecpar->format = AV_PIX_FMT_YUV420P;
        codecpar->sample_aspect_ratio = AVRational{1, 1};

        // set extradata for avc_decoder_configuration_record
        if (!track_info->GetCodecExtradata().empty()) {
          codecpar->extradata_size = track_info->GetCodecExtradata().size();
          codecpar->extradata = (uint8_t *)av_malloc(codecpar->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
          memset(codecpar->extradata, 0, codecpar->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
          memcpy(codecpar->extradata, &track_info->GetCodecExtradata()[0], codecpar->extradata_size);
        } else {
          LOG(WARNING) << "there is no extra data found in video track";
        }

        stream->display_aspect_ratio = AVRational{1, 1};
        stream->time_base = AVRational{track_info->GetTimeBase().GetNum(), track_info->GetTimeBase().GetDen()};

        mediatrack_to_avstreamindex_[track_info->GetId()] = stream->index;
      } break;
      case cmn::MediaType::Audio: {
        stream = avformat_new_stream(oc_, nullptr);
        AVCodecParameters *codecpar = stream->codecpar;

        codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        codecpar->codec_id =
            (track_info->GetCodecId() == cmn::MediaCodecId::Aac)
                ? AV_CODEC_ID_AAC
                : (track_info->GetCodecId() == cmn::MediaCodecId::Mp3)
                      ? AV_CODEC_ID_MP3
                      : (track_info->GetCodecId() == cmn::MediaCodecId::Opus) ? AV_CODEC_ID_OPUS : AV_CODEC_ID_NONE;
        codecpar->bit_rate = track_info->GetBitrate();
        codecpar->channels = static_cast<int>(track_info->GetChannel().GetCounts());
        codecpar->channel_layout =
            (track_info->GetChannel().GetLayout() == cmn::AudioChannel::Layout::LayoutMono)
                ? AV_CH_LAYOUT_MONO
                : (track_info->GetChannel().GetLayout() == cmn::AudioChannel::Layout::LayoutStereo)
                      ? AV_CH_LAYOUT_STEREO
                      : 0;  // <- Unknown
        codecpar->sample_rate = track_info->GetSample().GetRateNum();
        codecpar->frame_size = 1024;  // TODO: Need to Frame Size
        codecpar->codec_tag = 0;

        // set extradata for aac_specific_config
        if (!track_info->GetCodecExtradata().empty()) {
          codecpar->extradata_size = track_info->GetCodecExtradata().size();
          codecpar->extradata = (uint8_t *)av_malloc(codecpar->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
          memset(codecpar->extradata, 0, codecpar->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
          memcpy(codecpar->extradata, &track_info->GetCodecExtradata()[0], codecpar->extradata_size);
        }

        stream->time_base = AVRational{track_info->GetTimeBase().GetNum(), track_info->GetTimeBase().GetDen()};

        mediatrack_to_avstreamindex_[track_info->GetId()] = stream->index;
      } break;
      default: {
        auto log =
            ov::String::FormatString("This media type is not supported. media_type(%d)", track_info->GetMediaType());
        LOG(WARNING) << log;
        return false;
      } break;
    }
  }

  return true;
}

bool SnyMuxerImplFFMpeg::internalOpen() {
  AVDictionary *options = nullptr;
  // Examples
  // av_dict_set(&out_options, "timeout", "1000000", 0);
  // av_dict_set(&out_options, "tcp_nodelay", "1", 0);
  // _format_context->flags = AVFMT_FLAG_NOBUFFER | AVFMT_FLAG_FLUSH_PACKETS;
  if (!(oc_->oformat->flags & AVFMT_NOFILE)) {
    int error = avio_open2(&oc_->pb, path_.c_str(), AVIO_FLAG_WRITE, nullptr, &options);
    if (error < 0) {
      char errbuf[256];
      av_strerror(error, errbuf, sizeof(errbuf));
      auto log =
          ov::String::FormatString("Error opening file. error(%d:%s), filename(%s)", error, errbuf, path_.c_str());
      LOG(ERROR) << log;
      return false;
    }
  }

  if (avformat_write_header(oc_, nullptr) < 0) {
    LOG(ERROR) << "Could not create header";
    return false;
  }

  av_dump_format(oc_, 0, path_.c_str(), 1);

  if (oc_->oformat != nullptr) {
    auto oformat = oc_->oformat;
    auto log = ov::String::FormatString("name : %s, long_name : %s, mime_type : %s, audio_codec : %d, video_codec : %d",
                                        oformat->name, oformat->long_name, oformat->mime_type, oformat->audio_codec,
                                        oformat->video_codec);
    LOG(INFO) << log.CStr();
  }
  return true;
}
}  // namespace sny