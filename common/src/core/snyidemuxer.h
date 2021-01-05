/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYIDEMUXER_H
#define STREAMINGNOLOGY_CORE_SNYIDEMUXER_H
#include <string>
#include "core/snygpacmediainfo.h"
#include "core/snyresults.h"
#include "core/snysample.h"
#include "core/snytype.h"
namespace sny {
class SnyIDemuxer {
 public:
  SnyIDemuxer() {}
  virtual ~SnyIDemuxer() {}
  static SnyIDemuxer* createInstance();

 public:
  virtual void setUri(std::string uri) = 0;
  virtual SnyResult open() = 0;
  virtual GpacMediaInfo getMediaInfo() = 0;
  virtual SnyResult selectTrack(int trackNumber) = 0;
  virtual SnyResult seek(SnyUI64 position_us) = 0;
  virtual SnyResult seek(int trackNumber, SnyUI64 position_us) = 0;
  virtual SnyBool reachEndOfFile() = 0;
  virtual SnyBool reachEndOfTrack(int trackNumber) = 0;
  virtual SnySample* readSample(int trackNumber) = 0;
};
}  // namespace sny
#endif  // !STREAMINGNOLOGY_CORE_SNYIDEMUXER_H
