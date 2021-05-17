/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

#include "core/data.h"
#include "media_common.h"
#include <memory>

namespace sny
{
/**
 * @brief MediaSample
 * you can find extra data need in some codec(avc, hevc etc) in MediaTrackInfo
 * more info about this MediaSample are also in MediaTrackInfo
 * dts, pts, duration are in @kAVRationalMicrosecond timebase
 * timebase in MediaTrackInfo is its original timebase
 */
class MediaSample final
{
  public:
    /**
     * @brief ctor
     * @param dts_us dts in Microsecond
     * @param pts_us pts in Microsecond
     * @param duration_us duration in Microsecond
     * @param is_key IDR frame or not
     * @param track_info MediaTrackInfo of that media track where this sample came from
     * @param sample_data sample data for this MediaSample
     */
    MediaSample(int64_t dts_us, int64_t pts_us, int64_t duration_us, bool is_key,
                const std::shared_ptr<MediaTrackInfo> &track_info, const std::shared_ptr<ov::Data> &sample_data);

    MediaSample(const MediaSample &other);

    MediaSample &operator=(const MediaSample &) = delete;

    MediaType mediaType() const;

    int64_t getDts() const;

    void setDts(int64_t dts_us);

    int64_t getPts() const;

    void setPts(int64_t pts_us);

    int64_t getDuartion() const;

    void setDuration(int64_t duration_us);

    const MediaRational &getTimebase() const;

    int getTrackId() const;

    void setKey(bool is_key);

    bool isKey() const;

    BitStreamFormat getBitStreamFormat() const;

    const std::shared_ptr<ov::Data> &getSampleDataBuffer() const;

    const uint8_t *getSampleData() const;

    int getSampleSize() const;

    std::shared_ptr<MediaTrackInfo> getMediaTrackInfo() const;

  private:
    std::shared_ptr<MediaTrackInfo> trackInfo_{nullptr};
    std::shared_ptr<ov::Data> sampleData_{nullptr};
    
    /*pts, dts, duration are in microsecond timescale*/
    const MediaRational &media_rational_{kAVRationalMicrosecond};
    int64_t pts_{-1};
    int64_t dts_{-1};
    int64_t duration_{-1};

    bool isKey_{false};
};

} // namespace sny