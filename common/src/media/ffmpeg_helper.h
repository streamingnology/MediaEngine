/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace sny
{
namespace FfmpegHelper
{

/**
 * @brief check if bitstreamformat is annexb or avcc
 * more about h264 video type, refer here https://docs.microsoft.com/en-us/windows/win32/directshow/h-264-video-types
 * FOURCC    Description
 * AVC1      H.264 bitstream without start codes.
 * H264      H.264 bitstream with start codes.
 * h264      Equivalent to MEDIASUBTYPE_H264, with a different FOURCC.
 * X264      Equivalent to MEDIASUBTYPE_H264, with a different FOURCC.
 * x264      Equivalent to MEDIASUBTYPE_H264, with a different FOURCC.
 *
 * @param avstream ffmpeg AVStream
 * @return true if is annexb format, otherwise false
 */
bool h264_is_annexb(const AVStream *avstream);

/**
 * @brief check if bitstreamformat is annexb or hevcc, thanks LAVFilters project
 * @param format container format, deprecated now, just ignor this
 * @param avstream ffmpeg AVStream
 * @return true if is annexb format, otherwise false
 */
bool h265_is_annexb(std::string format, const AVStream *avstream);

/**
 * @brief create one bitstream filter by name 
 * @param stream ffmpeg AVStream
 * @param bsf_ctx bitstream format context
 * @param filter_name [h264_mp4toannexb, hevc_mp4toannexb]
 * @return 0 if success, otherwise < 0
*/
int ffmpeg_create_bitstream_filter(AVStream *stream, AVBSFContext **bsf_ctx, const std::string &filter_name);

/**
 * @brief release bitstream filter
 * @param bsf_ctx resource needed to be release
*/
void ffmpeg_release_bitstream_filter(AVBSFContext *bsf_ctx);

} // namespace FfmpegHelper
} // namespace sny
