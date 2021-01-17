/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "snyidemuxer.h"
#include "media/snydemuxerhelper.h"
#include "media/snymediainfo.h"
#include <vector>
namespace sny {
class SnyDemuxer : public SnyIDemuxer {
 public:
  SnyDemuxer();
  ~SnyDemuxer();

  void setUri(std::string uri) override;
  SnyResult open() override;
  SnyMediaInfo getMediaInfo() override;
  SnyResult selectTrack(SnyInt track_id) override;
  SnyResult unselectTrack(SnyInt track_id) override;
  SnyResult seek(SnyUI64 position_us) override;
  SnyBool reachEndOfFile() override;
  SnyMediaSample* readSample() override;

 private:
  static SnyAudioTrackInfo* getAudioTrackInfo(AP4_Track* track);
  static SnyVideoTrackInfo* getVideoTrackInfo(AP4_Track* track);

 private:
  std::string uri_;
  AP4_ByteStream* input_;
  AP4_File* input_file_;
  AP4_LinearReader* linear_reader_;
  std::vector<SampleReader*> sample_readers_;
  bool eos_;
  SnyMediaInfo media_info_;
};
}  // namespace sny
