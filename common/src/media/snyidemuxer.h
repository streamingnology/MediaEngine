/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "core/snytype.h"
#include "media/snymediainfo.h"
#include "media/snymediasample.h"
#include "media/snymediatype.h"
namespace sny {

class SnyIDemuxer {
 public:
  virtual void setUri(std::string uri) = 0;
  virtual SnyResult open() = 0;
  virtual SnyMediaInfo getMediaInfo() = 0;
  virtual SnyResult selectTrack(SnyInt track_id) = 0;
  virtual SnyResult unselectTrack(SnyInt track_id) = 0;
  virtual SnyResult seek(SnyUI64 position_us) = 0;
  virtual SnyBool reachEndOfFile() = 0;
  virtual SnyMediaSample* readSample() = 0;
};

}  // namespace sny
