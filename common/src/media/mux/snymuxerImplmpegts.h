/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include <mutex>
#include <string>
#include "snyimuxer.h"
#include "snymuxerImplmpegtshelper.h"

namespace sny {

class SnyMuxerImplTS : public SnyIMuxer {
 public:
  SnyMuxerImplTS();
  virtual ~SnyMuxerImplTS();

  void setPath(std::string path, std::string container_fmt) override;
  std::string getPath() override;

  bool addMediaTrack(const std::shared_ptr<MediaTrack> media_track) override;
  const std::map<SnyInt, std::shared_ptr<MediaTrack>>& getMediaTrack() override;

  bool open() override;
  void close() override;

  bool writeSample(std::shared_ptr<SnyMediaSample> sample) override;

 private:
  AP4_Result WritePAT();
  AP4_Result WritePMT();
  void WriteMPEG2PacketCCTO16(AP4_ByteStream& output);
  AP4_Result CreateByteStream();

 private:
  std::string path_;
  std::string fmt_;
  AP4_ByteStream* output_;
  AP4_UI16 video_stream_pid_;
  AP4_UI16 audio_stream_pid_;
  std::map<SnyInt, std::shared_ptr<MediaTrack>> media_tracks_;     //<MediaTrack.id, std::hsared_ptr<RtmpTrackInfo>>
  std::map<SnyInt, std::shared_ptr<SampleStream>> media_streams_;  //<MediaTrack.id, SampleStream>
  std::shared_ptr<Stream> pat_stream_;
  std::shared_ptr<Stream> pmt_stream_;
};

}  // namespace sny