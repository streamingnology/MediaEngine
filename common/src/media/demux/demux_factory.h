/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <memory>
#include "demux.h"

namespace sny
{

class DemuxerFactory
{
  public:
    virtual std::shared_ptr<IFileDemuxer> createDemuxer() = 0;
    virtual ~DemuxerFactory();
};

class FileDemuxerFfmpegFactory : public DemuxerFactory
{
  public:
    virtual std::shared_ptr<IFileDemuxer> createDemuxer() override;
    ~FileDemuxerFfmpegFactory();
};

} // namespace sny