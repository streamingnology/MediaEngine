/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
namespace sny{
enum SnyMediaType {
  kMediaTypeUnknown = 0,
  kMediaTypeAudio,
  kMediaTypeVideo,
  kMediaTypeSubtitle,
};

enum SnyCodecType {
  kCodecUnknown = 0,
  kCodecAac,
  kCodecAc3,
  kCodecH264,
  kCodecH265,
};
}
