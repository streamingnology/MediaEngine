#pragma once
#include <cstdint>
#include "media/media_buffer.h"

class H264AvccToAnnexB {
 public:
  static bool GetExtradata(const cmn::PacketType type, const std::shared_ptr<ov::Data> &data,
                           std::vector<uint8_t> &extradata);
  static bool Convert(cmn::PacketType type, const std::shared_ptr<ov::Data> &data,
                      const std::vector<uint8_t> &extradata);
};
