#pragma once

#include "media_sample.h"

namespace sny
{

MediaSample::MediaSample(int64_t dts_us, int64_t pts_us, int64_t duration_us, bool is_key,
                         const std::shared_ptr<MediaTrackInfo> &track_info,
                         const std::shared_ptr<ov::Data> &sample_data)
    : dts_(dts_us), pts_(pts_us), duration_(duration_us), isKey_(is_key), trackInfo_(track_info),
      sampleData_(sample_data)
{
}

MediaSample::MediaSample(const MediaSample &other)
{
    trackInfo_ = other.trackInfo_;
    sampleData_ = other.sampleData_;
    dts_ = other.dts_;
    pts_ = other.pts_;
    duration_ = other.duration_;
    isKey_ = other.isKey_;
}

MediaType MediaSample::mediaType() const
{
    if (trackInfo_)
        return trackInfo_->mediaType_;
    return MediaType::kUnknown;
}

int64_t MediaSample::getDts() const
{
    return dts_;
}

void MediaSample::setDts(int64_t dts_us)
{
    dts_ = dts_us;
}

int64_t MediaSample::getPts() const
{
    return pts_;
}

void MediaSample::setPts(int64_t pts_us)
{
    pts_ = pts_us;
}

int64_t MediaSample::getDuartion() const
{
    return duration_;
}

void MediaSample::setDuration(int64_t duration_us)
{
    duration_ = duration_us;
}

const MediaRational &MediaSample::getTimebase() const
{
    return media_rational_;
}

int MediaSample::getTrackId() const
{
    if (trackInfo_)
        return trackInfo_->trackId_;

    return -1;
}

void MediaSample::setKey(bool is_key)
{
    isKey_ = is_key;
}

bool MediaSample::isKey() const
{
    return isKey_;
}

BitStreamFormat MediaSample::getBitStreamFormat() const
{
    if (trackInfo_)
        return trackInfo_->bsf_;

    return BitStreamFormat::kUnknwon;
}

const std::shared_ptr<ov::Data> &MediaSample::getSampleDataBuffer() const
{
    return sampleData_;
}

const uint8_t *MediaSample::getSampleData() const
{
    if (sampleData_)
        return sampleData_->GetDataAs<uint8_t>();

    return nullptr;
}

int MediaSample::getSampleSize() const
{
    if (sampleData_)
        return sampleData_->GetLength();

    return 0;
}

std::shared_ptr<MediaTrackInfo> MediaSample::getMediaTrackInfo() const
{
    return trackInfo_;
}

} // namespace sny