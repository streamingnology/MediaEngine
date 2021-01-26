#pragma once
#include <cstdint>
#include <vector>
#include "core/bit_reader.h"

// Parses the payload of the NAL unit without the starting byte
class NalUnitBitstreamParser : public BitReader {
 public:
  NalUnitBitstreamParser(const uint8_t *bitstream, size_t length);

  bool ReadU8(uint8_t &value);
  bool ReadU16(uint16_t &value);
  bool ReadU32(uint32_t &value);
  bool ReadUEV(uint32_t &value);
  bool ReadSEV(int32_t &value);
  bool Skip(uint32_t count);

 private:
  std::vector<uint8_t> _bitstream;
};