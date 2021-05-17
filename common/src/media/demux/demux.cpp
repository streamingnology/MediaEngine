/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "demux.h"
#include "core/snyeasylogging.h"

namespace sny
{

IFileDemuxer::IFileDemuxer()
{
}

IFileDemuxer::~IFileDemuxer()
{
}


FileDemuxerBase::FileDemuxerBase()
{
}

FileDemuxerBase::~FileDemuxerBase()
{
}

void FileDemuxerBase::setFilePath(const std::string &file_path)
{
    file_path_ = file_path;
}

const std::string &FileDemuxerBase::getFilePath() const
{
    return file_path_;
}

bool FileDemuxerBase::open()
{
    if (opened_)
    {
        LOG(ERROR) << "File already opened: " << file_path_;
        return true;
    }

    if (openImpl())
    {
        opened_ = true;
    }

    return opened_;
}

void FileDemuxerBase::close()
{
    if (!opened_)
    {
        return;
    }
    closeImpl();
    opened_ = false;
    eof_ = false;
}

bool FileDemuxerBase::seek(int64_t pos_us)
{
    if (opened_)
    {
        eof_ = false;
        return seekImpl(pos_us);
    }

    return false;
}

const std::vector<std::shared_ptr<MediaTrackInfo>> FileDemuxerBase::getMediaTrackInfos() const
{
    if (opened_)
        return getMediaTrackInfosImpl();
    
    std::vector<std::shared_ptr<MediaTrackInfo>> res;
    return res;
}

std::shared_ptr<MediaSample> FileDemuxerBase::readSample()
{
    if (!opened_)
    {
        return nullptr;
    }
    return readSampleImpl();
}

void FileDemuxerBase::setEof(const bool eof)
{
    this->eof_ = eof;
}

bool FileDemuxerBase::isEof() const
{
    return this->eof_;
}

} // namespace sny