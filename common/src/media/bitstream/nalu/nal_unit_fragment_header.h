#pragma once

#include "core/data.h"
#include "media/media_buffer.h"
#include "media/media_type.h"
class NalUnitFragmentHeader {
 public:
  NalUnitFragmentHeader();
  ~NalUnitFragmentHeader();

  static bool Parse(const std::shared_ptr<ov::Data> &data, NalUnitFragmentHeader &fragment_hdr);
  static bool Parse(const uint8_t *bitstream, size_t length, NalUnitFragmentHeader &fragment_hdr);

  const FragmentationHeader *GetFragmentHeader() const { return &_fragment_header; }

  FragmentationHeader _fragment_header;
};
