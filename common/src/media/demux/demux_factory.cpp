/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "demux_factory.h"
#include "demux_ffmpeg.h"
#include <memory>

namespace sny
{

DemuxerFactory::~DemuxerFactory()
{
}

std::shared_ptr<IFileDemuxer> FileDemuxerFfmpegFactory::createDemuxer()
{
    return std::make_shared<FileDemuxerFfmpegImpl>();
}

FileDemuxerFfmpegFactory::~FileDemuxerFfmpegFactory()
{
}

} // namespace sny