/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snymuxerfactory.h"
#include "snymuxerImplffmpeg.h"
#include "snymuxerImplmpegts.h"

namespace sny {

std::shared_ptr<SnyIMuxer> SnyMuxerFactory::createMux(SnyMuxType mux_type) {
  switch (mux_type) {
    case kFFMPEGMUX:
      return std::make_shared<SnyMuxerImplFFMpeg>();
    case kBENTO4MUX:
      return nullptr;
    case kMPEGTSMUX:
      return std::make_shared<SnyMuxerImplTS>();
    default:
      return nullptr;
  }
}
}  // namespace sny