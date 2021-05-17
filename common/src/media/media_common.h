/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

#include <unordered_map>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/rational.h>
}

namespace sny
{

/**
 * @brief
 * Rational number (pair of numerator and denominator).
 * Auto convert between AVRational and MediaRational
 * If add another struct of timebase, implement its conversion function and ctor
 */
struct MediaRational
{
    MediaRational();
    MediaRational(int num, int den);

    /**
     * @brief consturctor from ffmpeg AVRational
     * @param r ffmpeg AVRational
     */
    MediaRational(const AVRational r);

    MediaRational(const MediaRational &) = default;
    MediaRational &operator=(const MediaRational &) = default;

    /**
     * @brief conversion function, convert MediaRational to ffmpeg AVRational
     */
    operator AVRational() const;

    /**
     * @brief conversion function, convert MediaRational to double
     */
    operator double() const;

    /**
     * @brief Numerator
     */
    int num_;

    /**
     * @brief Denominator
     */
    int den_;
};

const int kTimeScaleMicrosecond = 1000000; // us
const int kTimeScaleMillisecond = 1000;    // ms

/**
 * @brief kAVRationalMicrosecond as Microsecond timebase
 * kAVRationalMillisecond as Millisecond timebase
 */
const MediaRational kAVRationalMicrosecond = {1, kTimeScaleMicrosecond};
const MediaRational kAVRationalMillisecond = {1, kTimeScaleMillisecond};

/**
 * @brief Media Type in this system
 */
enum class MediaType
{
    kUnknown = -1,
    kAudio = 0,
    kVideo,
    kSubtitle,
};

/**
 * @brief Media Codec in this system
 */
enum class MediaCodecId
{
    kUnknown = -1,
    kAac = 0,
    kH264,
    kH265,
};

/**
 * @brief bitstream format for each media track
 * for example, AVCC fmt is used in mp4, flv container. ANNEXB fmt is used in transport stream(TS) container
 * !!! BE CAREFULL its bsf when handle each stream
 */
enum class BitStreamFormat
{
    kUnknwon = -1,
    kH264Avcc = 0,
    kH264Annexb,
    k265Hvcc,
    kH265Annexb,
    kAacLatm,
    kAacAdts,
};

/**
 * @brief Format for audio stream
 */
struct AudioStreamFormat
{
    /**
     * @brief ctor
     * @param ch channels
     * @param sz sample size
     * @param rate sample rate
     * @param bits_per_sample bits per sample
     */
    AudioStreamFormat(int ch, int sz, int rate, int bits_per_sample)
        : channels_(ch), frameSize_(sz), sampleRate_(rate), bitsPerSample_(bits_per_sample)
    {
    }
    AudioStreamFormat(const AudioStreamFormat &) = default;
    AudioStreamFormat &operator=(const AudioStreamFormat &) = default;

    int channels_;
    int frameSize_;
    int sampleRate_;
    int bitsPerSample_;
};

/**
 * @brief Format for video stream
 */
struct VideoStreamFormat
{
    /**
     * @brief ctor
     * @param w width
     * @param h height
     * @param frame_rate frame rate
     */
    VideoStreamFormat(int w, int h, MediaRational frame_rate) : width_(w), height_(h), frameRate_(frame_rate)
    {
    }
    VideoStreamFormat(const VideoStreamFormat &) = default;
    VideoStreamFormat &operator=(const VideoStreamFormat &) = default;

    int width_;
    int height_;
    MediaRational frameRate_;
};

struct StreamFormat
{
    StreamFormat();
    /**
     * @brief
     * @param type MediaType
     * @param codec MediaCodecId
     * @param bsf BitStreamFormat
     */
    StreamFormat(MediaType type, MediaCodecId codec, BitStreamFormat bsf);
    StreamFormat(const StreamFormat &other);
    StreamFormat &operator=(const StreamFormat &other);

    MediaType mediaType_{MediaType::kUnknown};
    MediaCodecId mediaCodecId_{MediaCodecId::kUnknown};
    BitStreamFormat bsf_{BitStreamFormat::kUnknwon};

    union {
        VideoStreamFormat vf_;
        AudioStreamFormat af_;
    };
};

/**
 * @brief information about this media track
 */
struct MediaTrackInfo
{
    /**
     * @brief ctor
     * @param media_type MediaType
     * @param codec_id MediaCodecId
     * @param bsf BitStreamFormat
     * @param stream_fmt StreamFormat
     * @param extra_data extra data used for some codec
     * @param rational MediaRational for this media track
     * @param track_id track id for this media track, it should be unique
     * @param bitrate bitrate of this media track
     */
    MediaTrackInfo::MediaTrackInfo(const MediaType media_type, const MediaCodecId codec_id, const BitStreamFormat bsf,
                                   const StreamFormat &stream_fmt, std::vector<uint8_t> &extra_data,
                                   const MediaRational rational, const int track_id, const int bitrate);

    MediaTrackInfo::MediaTrackInfo(const MediaTrackInfo &other);

    MediaType mediaType_{MediaType::kUnknown};

    MediaCodecId mediaCodecId_{MediaCodecId::kUnknown};

    BitStreamFormat bsf_{BitStreamFormat::kUnknwon};

    StreamFormat streamFmt_;

    std::vector<uint8_t> extraData_;

    MediaRational mediaRational_;

    int trackId_;
    int bitRate_;
};

// clang-format off
static const std::unordered_map<AVCodecID, MediaCodecId> kMediaCodecFromFfmpeg2Sny {
    {AV_CODEC_ID_H264, MediaCodecId::kH264},
    {AV_CODEC_ID_H265, MediaCodecId::kH265},
    {AV_CODEC_ID_AAC,  MediaCodecId::kAac}
};

static const std::unordered_map<MediaCodecId, AVCodecID> kMediaCodecFromSny2Ffmpeg {
    {MediaCodecId::kH264, AV_CODEC_ID_H264},
    {MediaCodecId::kH265, AV_CODEC_ID_H265},
    {MediaCodecId::kAac,  AV_CODEC_ID_AAC}
};
// clang-format on

/**
 * @brief convert codec id from this system to ffmpeg, only support AVC, HEVC and AAC codec, otherwise, return
 * AV_CODEC_ID_NONE
 * @param codec_id codec id in this system
 * @return codec id in ffmpeg
 */
AVCodecID convertMediaCodecIdFromSny2Ffmpeg(const MediaCodecId codec_id);

/**
 * @brief convert codec id from ffmpeg to this system, current only support AVC, HEVC and AAC codec, otherwise, return
 * MediaCodecId::kUnknown
 * @param codec_id codec id in ffmpeg
 * @return codec id in this system
 */
MediaCodecId convertMediaCodecIdFromFfmpeg2Sny(const AVCodecID codec_id);

} // namespace sny