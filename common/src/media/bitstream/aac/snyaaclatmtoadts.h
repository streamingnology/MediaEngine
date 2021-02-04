/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 *this code referced from AirenSoft
 */
#pragma once
#include <cstdint>
#include "media/media_buffer.h"

namespace sny {
class SnyAACLatmToAdts {
 public:
  static bool GetExtradata(const cmn::PacketType type, const std::shared_ptr<ov::Data> &data,
                           std::vector<uint8_t> &extradata);
  static bool Convert(const cmn::PacketType type, const std::shared_ptr<ov::Data> &data,
                      const std::vector<uint8_t> &extradata, std::shared_ptr<ov::Data> &adts_data);
  static std::shared_ptr<ov::Data> MakeHeader(uint8_t aac_profile, uint8_t aac_sample_rate, uint8_t aac_channels,
                                              int16_t data_length);
};
}  // namespace sny