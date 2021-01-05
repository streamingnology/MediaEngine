/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYHLSCONSTANTS_H
#define STREAMINGNOLOGY_CORE_SNYHLSCONSTANTS_H
#include <string>
namespace sny {
static const std::string kSnyHlsTagIdentifier = "#EXTM3U";
static const std::string kSnyHlsTagVersion = "#EXT-X-VERSION";
static const std::string kSnyHlsTagTargetDuration = "#EXT-X-TARGETDURATION";
static const std::string kSnyHlsTagMediaSequence = "#EXT-X-MEDIA-SEQUENCE";
static const std::string kSnyHlsTagExtinf = "#EXTINF";
static const std::string kSnyHlsTagEndlist = "#EXT-X-ENDLIST";
}  // namespace sny

#endif  // !STREAMINGNOLOGY_CORE_SNYHLSCONSTANTS_H
