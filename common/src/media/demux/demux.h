/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "media/media_common.h"
#include "media/media_sample.h"

namespace sny
{
/**
 * @brief file demuxer interface
*/
class IFileDemuxer
{
  public:
    IFileDemuxer();
    virtual ~IFileDemuxer();

    virtual void setFilePath(const std::string &file_path) = 0;
    virtual const std::string &getFilePath() const = 0;

    virtual bool open() = 0;
    virtual void close() = 0;

    /**
     * @brief seek to first IDR frame before this position
     * @param pos_us seek to pos_us in kAVRationalMicrosecond time scale
     * @return true if success, otherwise false
    */
    virtual bool seek(int64_t pos_us) = 0;

    /**
     * @brief retrive each media track info found in this media file, only this supported codec tracks are listed
     * @return MediaTrackInfos found in this file, empty if no recognized tracks found
    */
    virtual const std::vector<std::shared_ptr<MediaTrackInfo>> getMediaTrackInfos() const = 0;

    /**
     * @brief read encoded sample from file
     * @return non empty if got one sample, otherwise empty
    */
    virtual std::shared_ptr<MediaSample> readSample() = 0;

    /**
     * @brief check if reach end of file
     * @return true if read to end, otherwise false
    */
    virtual bool isEof() const = 0;
};

class FileDemuxerBase : public IFileDemuxer
{
  public:
    FileDemuxerBase();
    virtual ~FileDemuxerBase();

    virtual void setFilePath(const std::string &file_path) override;
    virtual const std::string &getFilePath() const override;

    virtual bool open() override;
    virtual void close() override;

    virtual bool seek(int64_t pos_us) override;

    virtual const std::vector<std::shared_ptr<MediaTrackInfo>> getMediaTrackInfos() const override;

    virtual std::shared_ptr<MediaSample> readSample();

    virtual bool isEof() const;

  protected:
    void setEof(const bool eof);
    virtual bool openImpl() = 0;
    virtual void closeImpl() = 0;

    virtual bool seekImpl(int64_t pos_us) = 0;

    virtual std::shared_ptr<MediaSample> readSampleImpl() = 0;

    virtual const std::vector<std::shared_ptr<MediaTrackInfo>> getMediaTrackInfosImpl() const = 0;

  protected:
    std::string file_path_{""};
    bool opened_{false};
    bool eof_{false};
};

} // namespace sny