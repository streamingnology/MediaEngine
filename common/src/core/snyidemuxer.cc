/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snyidemuxer.h"
#include "core/snygpacmp4.h"
namespace sny {
SnyIDemuxer *SnyIDemuxer::createInstance() {
  SnyGpacMp4 *sny_gpac_mp4 = new SnyGpacMp4();
  return dynamic_cast<SnyIDemuxer *>(sny_gpac_mp4);
}
}  // namespace sny