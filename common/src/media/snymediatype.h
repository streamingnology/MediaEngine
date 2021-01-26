/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
namespace sny {
enum SnyMediaType {
  kMediaTypeUnknown = -1,
  kMediaTypeAudio = 0,
  kMediaTypeVideo,
  kMediaTypeSubtitle,
};

enum SnyCodecType {
  kCodecUnknown = -1,
  kCodecAac = 0,
  kCodecAc3,
  kCodecH264,
  kCodecH265,
};

enum SnyBitStreamFormat {
  kBitStreamUnknwon = -1,
  kBitStreamH264AVCC = 0,
  kBitStreamH264ANNEXB,
  kBitStreamH265HVCC,
  kBitStreamH265ANNEXB,
  kBitStreamAACLATM,
  kBitStreamAACADTS,
};
}  // namespace sny
