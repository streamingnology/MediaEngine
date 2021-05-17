/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

#include "demux_ffmpeg.h"
#include "core/snyeasylogging.h"
#include "media/aac_utils.h"
#include "media/ffmpeg_helper.h"
#include "media/media_common.h"
#include <memory>
#include <string>

namespace sny
{

FileDemuxerFfmpegImpl::FileDemuxerFfmpegImpl()
{
}

FileDemuxerFfmpegImpl::~FileDemuxerFfmpegImpl()
{
}

bool FileDemuxerFfmpegImpl::openImpl()
{
    /* open input file, and allocate format context */
    if ((avformat_open_input(&fmt_ctx_, file_path_.c_str(), NULL, NULL)) < 0)
    {
        LOG(ERROR) << "Could not open source file: " << file_path_;
        return false;
    }

    /* retrieve stream information */
    if (avformat_find_stream_info(fmt_ctx_, NULL) < 0)
    {
        LOG(ERROR) << "Could not find stream information";
        return false;
    }

    for (unsigned int i = 0; i < fmt_ctx_->nb_streams; i++)
    {
        AVStream *av_stream = fmt_ctx_->streams[i];

        MediaRational media_rational(av_stream->time_base.num, av_stream->time_base.den);

        int track_id = av_stream->index;
        int bit_rate = av_stream->codecpar->bit_rate;
        sny::MediaCodecId codec_id = convertMediaCodecIdFromFfmpeg2Sny(av_stream->codecpar->codec_id);

        // Make sure extradata is valid here
        std::vector<uint8_t> extra_data;
        if (av_stream->codecpar->extradata_size > 0)
        {
            const int kMaxExtraDataSize = 500;
            if (av_stream->codecpar->extradata_size < kMaxExtraDataSize)
            {
                extra_data.resize(av_stream->codecpar->extradata_size);
                std::copy(av_stream->codecpar->extradata,
                          av_stream->codecpar->extradata + av_stream->codecpar->extradata_size, extra_data.begin());
            }
            else
            {
                LOG(ERROR) << "extradata size is bigger than " << kMaxExtraDataSize << " bytes";
                continue;
            }
        }

        if (av_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && codec_id != MediaCodecId::kUnknown)
        {
            auto media_type = MediaType::kVideo;
            auto bsf = BitStreamFormat::kUnknwon;

            AVBSFContext *bsf_ctx = nullptr;
            if (codec_id == MediaCodecId::kH264)
            {
                bool is_annexb = FfmpegHelper::h264_is_annexb(av_stream);
                if (!is_annexb)
                {
                    int ret = FfmpegHelper::ffmpeg_create_bitstream_filter(av_stream, &bsf_ctx, "h264_mp4toannexb");
                    if (ret == 0)
                    {
                        bsf = BitStreamFormat::kH264Annexb;
                    }
                    else
                    {
                        bsf = BitStreamFormat::kH264Avcc;
                        bsf_ctx = nullptr;
                        LOG(ERROR) << "create h264_mp4toannexb filter failed";
                    }
                }
                else
                {
                    bsf = BitStreamFormat::kH264Annexb;
                }
            }
            else if (codec_id == MediaCodecId::kH265)
            {
                bool is_annexb = FfmpegHelper::h265_is_annexb("", av_stream);
                if (!is_annexb)
                {
                    int ret = FfmpegHelper::ffmpeg_create_bitstream_filter(av_stream, &bsf_ctx, "hevc_mp4toannexb");
                    if (ret == 0)
                    {
                        bsf = BitStreamFormat::kH265Annexb;
                    }
                    else
                    {
                        bsf = BitStreamFormat::k265Hvcc;
                        bsf_ctx = nullptr;
                        LOG(ERROR) << "create hevc_mp4toannexb filter failed";
                    }
                }
                else
                {
                    bsf = BitStreamFormat::kH265Annexb;
                }
            }
            bitstream_filter_ctxs_.insert({track_id, bsf_ctx});

            VideoStreamFormat vfmt(av_stream->codecpar->width, av_stream->codecpar->height, av_stream->avg_frame_rate);
            StreamFormat sfmt(media_type, codec_id, bsf);
            sfmt.vf_ = vfmt;

            auto track_info = std::make_shared<MediaTrackInfo>(media_type, codec_id, bsf, sfmt, extra_data,
                                                               media_rational, track_id, bit_rate);

            media_track_time_bases_.insert({track_info->trackId_, av_stream->time_base});
            media_tracks_.insert({track_info->trackId_, std::move(track_info)});
        }
        else if (av_stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && codec_id != MediaCodecId::kUnknown)
        {
            auto media_type = MediaType::kAudio;
            auto bsf = BitStreamFormat::kUnknwon;

            if (codec_id == MediaCodecId::kAac)
                bsf = BitStreamFormat::kAacAdts;

            int channels = av_stream->codecpar->channels;
            int frame_size = av_stream->codecpar->frame_size;
            int sample_rate = av_stream->codecpar->sample_rate;
            int bits_per_sample = av_stream->codecpar->bits_per_coded_sample;
            AudioStreamFormat afmt(channels, frame_size, sample_rate, bits_per_sample);

            StreamFormat sfmt(media_type, codec_id, bsf);
            sfmt.af_ = afmt;

            auto track_info = std::make_shared<MediaTrackInfo>(media_type, codec_id, bsf, sfmt, extra_data,
                                                               media_rational, track_id, bit_rate);

            media_track_time_bases_.insert({track_info->trackId_, av_stream->time_base});
            media_tracks_.insert({track_info->trackId_, std::move(track_info)});
        }
    }

    if (media_tracks_.empty())
    {
        LOG(ERROR) << "Dont find valid streams in this file : " << file_path_;
        return false;
    }

    LOG(INFO) << "Open file success : " << file_path_;
    return true;
}

void FileDemuxerFfmpegImpl::closeImpl()
{
    if (fmt_ctx_)
    {
        avformat_close_input(&fmt_ctx_);
        fmt_ctx_ = nullptr;
    }

    // free bitstream filters if needed
    for (auto &[k, v] : bitstream_filter_ctxs_)
    {
        if (v)
        {
            FfmpegHelper::ffmpeg_release_bitstream_filter(v);
            v = nullptr;
        }
    }
    bitstream_filter_ctxs_.clear();

    media_tracks_.clear();
    media_track_time_bases_.clear();
}

bool FileDemuxerFfmpegImpl::seekImpl(int64_t pos_us)
{
    while (!cached_samples_.empty())
    {
        cached_samples_.pop();
    }

    // AV_TIME_BASE   1000000
    // Internal time base represented as integer.
    // FFMpeg internal time base is AV_TIME_BASE, same as kTimeScaleMicrosecond 

    int ret = av_seek_frame(fmt_ctx_, -1, pos_us, AVSEEK_FLAG_BACKWARD);

    return ret >= 0;
}

const std::vector<std::shared_ptr<MediaTrackInfo>> FileDemuxerFfmpegImpl::getMediaTrackInfosImpl() const
{
    std::vector<std::shared_ptr<MediaTrackInfo>> res;
    for (auto &[k, v] : media_tracks_)
    {
        res.push_back(v);
    }
    return res;
}

std::shared_ptr<MediaSample> FileDemuxerFfmpegImpl::readSampleImpl()
{
    if (!cached_samples_.empty())
    {
        std::shared_ptr<MediaSample> sample = cached_samples_.front();
        cached_samples_.pop();
        return sample;
    }

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    /*read frames from media file*/
    while (av_read_frame(fmt_ctx_, &pkt) >= 0)
    {
        if (media_tracks_.find(pkt.stream_index) != media_tracks_.end())
        {

            if (bitstream_filter_ctxs_[pkt.stream_index])
            {
                int ret = 0;
                if (ret = av_bsf_send_packet(bitstream_filter_ctxs_[pkt.stream_index], &pkt) < 0)
                {
                    LOG(ERROR) << "av_bsf_send_packet failed, ret= " << ret;
                }
                while ((ret = av_bsf_receive_packet(bitstream_filter_ctxs_[pkt.stream_index], &pkt) == 0))
                {
                    int64_t dts_us =
                        av_rescale_q_rnd(pkt.dts, media_track_time_bases_[pkt.stream_index], kAVRationalMicrosecond,
                                         (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                    int64_t pts_us =
                        av_rescale_q_rnd(pkt.dts, media_track_time_bases_[pkt.stream_index], kAVRationalMicrosecond,
                                         (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                    int64_t duration_us =
                        av_rescale_q_rnd(pkt.dts, media_track_time_bases_[pkt.stream_index], kAVRationalMicrosecond,
                                         (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

                    bool isKey = pkt.flags & AV_PKT_FLAG_KEY ? true : false;
                    auto sample_data = std::make_shared<ov::Data>(pkt.data, pkt.size);

                    auto media_sample = std::make_shared<MediaSample>(dts_us, pts_us, duration_us, isKey,
                                                                      media_tracks_[pkt.stream_index], sample_data);
                    cached_samples_.push(media_sample);
                    av_packet_unref(&pkt);
                    if (ret < 0)
                    {
                        LOG(ERROR) << "av_bsf_send_packet failed, ret= " << ret;
                    }
                }
            }
            else
            {
                int64_t dts_us =
                    av_rescale_q_rnd(pkt.dts, media_track_time_bases_[pkt.stream_index], kAVRationalMicrosecond,
                                     (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                int64_t pts_us =
                    av_rescale_q_rnd(pkt.dts, media_track_time_bases_[pkt.stream_index], kAVRationalMicrosecond,
                                     (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                int64_t duration_us =
                    av_rescale_q_rnd(pkt.dts, media_track_time_bases_[pkt.stream_index], kAVRationalMicrosecond,
                                     (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

                bool isKey = pkt.flags & AV_PKT_FLAG_KEY ? true : false;
                auto sample_data = std::make_shared<ov::Data>();
                if (media_tracks_[pkt.stream_index]->mediaCodecId_ == MediaCodecId::kAac)
                {
                    AVStream *av_stream = fmt_ctx_->streams[pkt.stream_index];

                    if (av_stream->codecpar->extradata_size > 0 && aac_adts_.find(pkt.stream_index) == aac_adts_.end())
                    {

                        int version = 0, profile = 0, sample_rate = 0, channels = 0;
                        if (AACUtils::aac_parse_extra_data((const char *)av_stream->codecpar->extradata,
                                                           av_stream->codecpar->extradata_size, profile, sample_rate,
                                                           channels))
                        {
                            std::shared_ptr<AacAdtsHeader> sp_adts_head =
                                std::make_shared<AacAdtsHeader>(version, profile, sample_rate, channels);
                            aac_adts_.insert({pkt.stream_index, std::move(sp_adts_head)});
                        }
                        else
                        {
                            LOG(ERROR) << "aac parse extra data failed";
                        }
                    }

                    if (aac_adts_.find(pkt.stream_index) != aac_adts_.end())
                    {
                        auto &adts_header = aac_adts_[pkt.stream_index];
                        auto dataBuffer = AACUtils::aac_make_adts_header(adts_header->version, adts_header->profile,
                                                                         adts_header->sample_rate,
                                                                         adts_header->channels, pkt.size + 7);
                        sample_data->Append(dataBuffer.data(), dataBuffer.size());
                    }
                }

                sample_data->Append(pkt.data, pkt.size);

                auto media_sample = std::make_shared<MediaSample>(dts_us, pts_us, duration_us, isKey,
                                                                  media_tracks_[pkt.stream_index], sample_data);
                cached_samples_.push(media_sample);
                av_packet_unref(&pkt);
            }

            if (!cached_samples_.empty())
            {
                std::shared_ptr<MediaSample> sample = cached_samples_.front();
                cached_samples_.pop();
                return sample;
            }
        }
    }

    setEof(true);

    return nullptr;
}

} // namespace sny
