/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 *this code referced from AirenSoft
 */

#pragma once
#include <cstdint>
#include "media/media_buffer.h"

namespace sny {
class SnyH264AvccToAnnexB {
 public:
  static bool GetExtradata(const cmn::PacketType type, const std::shared_ptr<ov::Data> &data,
                           std::vector<uint8_t> &extradata);
  static bool Convert(cmn::PacketType type, const std::shared_ptr<ov::Data> &data,
                      const std::vector<uint8_t> &extradata, std::shared_ptr<ov::Data> &annexb_data);
};
}  // namespace sny