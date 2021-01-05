/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYGPACMP4_H
#define STREAMINGNOLOGY_CORE_SNYGPACMP4_H
extern "C" {
#include <gpac/isomedia.h>
#include <gpac/setup.h>
}
#include <map>
#include <string>
#include "core/snygpacmp4track.h"
#include "core/snyidemuxer.h"

namespace sny {
class SnyGpacMp4 : public SnyIDemuxer {
 public:
  SnyGpacMp4();
  virtual ~SnyGpacMp4();

 public:
  virtual void setUri(std::string uri);
  virtual SnyResult open();
  virtual GpacMediaInfo getMediaInfo();
  virtual SnyResult selectTrack(int track_number);
  virtual SnyResult seek(SnyUI64 position_us);
  virtual SnyResult seek(int track_number, SnyUI64 position_us);
  virtual SnyBool reachEndOfFile();
  virtual SnyBool reachEndOfTrack(int track_number);
  virtual SnySample* readSample(int track_number);

 private:
  std::string uri_;
  GF_ISOFile* ptr_gf_isofile_;
  std::map<int, SnyGpacMp4Track*> selected_tracks_;
};
}  // namespace sny

#endif  // !STREAMINGNOLOGY_CORE_SNYGPACMP4_H
