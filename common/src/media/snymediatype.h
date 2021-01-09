/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMEDIATYPE_H
#define STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMEDIATYPE_H
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

#endif  // STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMEDIATYPE_H
