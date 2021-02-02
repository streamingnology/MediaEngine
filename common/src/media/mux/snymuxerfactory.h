/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "snyimuxer.h"

namespace sny {

class SnyMuxerFactory {
 public:
  enum SnyMuxType {
    kFFMPEGMUX = 0,
    kBENTO4MUX = 1,
    kMPEGTSMUX = 2,
  };
  static std::shared_ptr<SnyIMuxer> createMux(SnyMuxType mux_type);

 private:
  SnyMuxerFactory() {}
};
}  // namespace sny