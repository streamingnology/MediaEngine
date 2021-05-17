/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

#include "media/demux/demux.h"
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

extern "C"
{
#include <libavformat/avformat.h>
}

namespace sny
{
class FileDemuxerFfmpegImpl : public FileDemuxerBase
{
  public:
    FileDemuxerFfmpegImpl();
    virtual ~FileDemuxerFfmpegImpl();

    virtual bool openImpl() override;
    virtual void closeImpl() override;

    virtual bool seekImpl(int64_t pos_us) override;

    virtual const std::vector<std::shared_ptr<MediaTrackInfo>> getMediaTrackInfosImpl() const override;

    virtual std::shared_ptr<MediaSample> readSampleImpl() override;

  private:
    struct AacAdtsHeader
    {
        AacAdtsHeader(int v, int p, int r, int c) : version(v), profile(p), sample_rate(r), channels(c)
        {
        }
        int version{0};
        int profile{0};
        int sample_rate{0};
        int channels{0};
    };

    AVFormatContext *fmt_ctx_{nullptr};

    std::unordered_map<int, std::shared_ptr<MediaTrackInfo>> media_tracks_; // <id, MediaTrack>
    std::unordered_map<int, AVRational> media_track_time_bases_;            // <id, AVRational>

    std::unordered_map<int, std::shared_ptr<AacAdtsHeader>> aac_adts_;

    /**
     * @brief If this stream need to be converted, its AVBSFContext* should NOT BE nullptr.
     * otherwise, set to nullptr
     */
    std::unordered_map<int, AVBSFContext *> bitstream_filter_ctxs_; // <id, AVBSFContext*>

    std::queue<std::shared_ptr<MediaSample>> cached_samples_;
};

} // namespace sny