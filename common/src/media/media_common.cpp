#include "media_common.h"

namespace sny
{

MediaRational::MediaRational() : MediaRational(-1, -1)
{
}

MediaRational::MediaRational(int num, int den) : num_(num), den_(den)
{
}

MediaRational::MediaRational(const AVRational r) : num_(r.num), den_(r.den)
{
}

MediaRational::operator AVRational() const
{
    AVRational r;
    r.num = this->num_;
    r.den = this->den_;
    return r;
}

MediaRational::operator double() const
{
    return static_cast<double>(this->num_) / static_cast<double>(this->den_);
}

MediaTrackInfo::MediaTrackInfo(const MediaType media_type, const MediaCodecId codec_id, const BitStreamFormat bsf,
                               const StreamFormat &stream_fmt, std::vector<uint8_t> &extra_data,
                               const MediaRational rational, const int track_id, const int bitrate)
    : mediaType_(media_type), mediaCodecId_(codec_id), bsf_(bsf), streamFmt_(stream_fmt), extraData_(extra_data),
      mediaRational_(rational), trackId_(track_id), bitRate_(bitrate)
{
}

MediaTrackInfo::MediaTrackInfo(const MediaTrackInfo &other)
{
    this->mediaType_ = other.mediaType_;
    this->mediaCodecId_ = other.mediaCodecId_;
    this->bsf_ = other.bsf_;
    this->streamFmt_ = other.streamFmt_;
    this->extraData_ = other.extraData_;
    this->mediaRational_ = other.mediaRational_;
    this->trackId_ = other.trackId_;
    this->bitRate_ = other.bitRate_;
}

StreamFormat::StreamFormat() : StreamFormat(MediaType::kUnknown, MediaCodecId::kUnknown, BitStreamFormat::kUnknwon)
{
}

StreamFormat::StreamFormat(MediaType type, MediaCodecId codec, BitStreamFormat bsf)
    : mediaType_(type), mediaCodecId_(codec), bsf_(bsf)
{
}

StreamFormat::StreamFormat(const StreamFormat &other)
{
    operator=(other);
}

StreamFormat &StreamFormat::operator=(const StreamFormat &other)
{
    if (this != &other)
    {
        this->mediaType_ = other.mediaType_;
        this->mediaCodecId_ = other.mediaCodecId_;
        this->bsf_ = other.bsf_;

        switch (mediaType_)
        {
        case MediaType::kVideo:
            vf_ = other.vf_;
        case MediaType::kAudio:
            af_ = other.af_;
        default:
            break;
        }
    }

    return *this;
}

AVCodecID convertMediaCodecIdFromSny2Ffmpeg(const MediaCodecId codec_id)
{
    AVCodecID res = AV_CODEC_ID_NONE;

    if (kMediaCodecFromSny2Ffmpeg.find(codec_id) != kMediaCodecFromSny2Ffmpeg.end())
    {
        res = kMediaCodecFromSny2Ffmpeg.at(codec_id);
    }

    return res;
}

MediaCodecId convertMediaCodecIdFromFfmpeg2Sny(const AVCodecID codec_id)
{
    MediaCodecId res = MediaCodecId::kUnknown;

    if (kMediaCodecFromFfmpeg2Sny.find(codec_id) != kMediaCodecFromFfmpeg2Sny.end())
    {
        res = kMediaCodecFromFfmpeg2Sny.at(codec_id);
    }

    return res;
}

} // namespace sny
