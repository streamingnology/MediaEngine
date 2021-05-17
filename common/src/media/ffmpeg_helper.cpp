/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include <string>
#include <cassert>
#include "ffmpeg_helper.h"
#include "core/snyeasylogging.h"

extern "C"
{
#include <libavutil/intreadwrite.h>
#include <libavutil/avutil.h>
}

namespace sny
{
namespace FfmpegHelper
{
const int FOURCC_MAX_STRING_SIZE = 32;
const std::string avc1 = "avc1";

bool h264_is_annexb(const AVStream *avstream)
{
    assert(avstream->codecpar->codec_id == AV_CODEC_ID_H264);

    char buff[FOURCC_MAX_STRING_SIZE] = {0};
    std::string fourcc(av_fourcc_make_string(buff, avstream->codecpar->codec_tag));

    return fourcc != avc1;
}

bool h265_is_annexb(std::string format, const AVStream *avstream)
{
    assert(avstream->codecpar->codec_id == AV_CODEC_ID_HEVC);
    if (avstream->codecpar->extradata_size < 23)
        return true;
    if (avstream->codecpar->extradata[0] || avstream->codecpar->extradata[1] || avstream->codecpar->extradata[2] > 1)
        return false;
    /*if (format == "avi") {
      BYTE *src = avstream->codec->extradata;
      unsigned startcode = AV_RB32(src);
      if (startcode == 0x00000001 || (startcode & 0xffffff00) == 0x00000100)
        return true;
    }*/
    return true;
}

int ffmpeg_create_bitstream_filter(AVStream *stream, AVBSFContext **bsf_ctx, const std::string &filter_name)
{
    int ret = -1;
    do
    {
        const AVBitStreamFilter *filter = av_bsf_get_by_name(filter_name.c_str());
        if (!filter)
        {
            LOG(ERROR) << "Unknow bitstream filter: " << filter_name;
            break;
        }
        if ((ret = av_bsf_alloc(filter, bsf_ctx) < 0))
        {
            LOG(ERROR) << "av_bsf_alloc failed: " << filter_name << ", ret = " << ret;
            break;
        }
        if ((ret = avcodec_parameters_copy((*bsf_ctx)->par_in, stream->codecpar)) < 0)
        {
            LOG(ERROR) << "avcodec_parameters_copy failed: " << filter_name << ", ret = " << ret;
            break;
        }
        if ((ret = av_bsf_init(*bsf_ctx)) < 0)
        {
            LOG(ERROR) << "av_bsf_init failed: " << filter_name << ", ret = " << ret;
            break;
        }
        ret = 0;
    } while (false);

    return ret;
}

void ffmpeg_release_bitstream_filter(AVBSFContext *bsf_ctx)
{
    if (!bsf_ctx)
        av_bsf_free(&bsf_ctx);
}

} // namespace FfmpegHelper
} // namespace sny