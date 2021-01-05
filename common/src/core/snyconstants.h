/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_CORE_SNYCONSTANTS_H
#define STREAMINGNOLOGY_CORE_SNYCONSTANTS_H
#include <string>
#include "core/snytype.h"
namespace sny {
static const std::string kCompanyName = "streamingnology";

static const std::string kMediaInfoName = "name";
static const std::string kMediaInfoPath = "path";
static const std::string kMediaInfoDuration = "duration";
static const std::string kMediaInfoSize = "size";
static const std::string kMediaInfoError = "error";
static const std::string kMediaInfoTracks = "tracks";
static const std::string kMediaInfoTrackId = "trackid";
static const std::string kMediaInfoTrackNumber = "tracknumber";
static const std::string kMediaInfoTrackType = "type";
static const std::string kMediaInfoTrackCodec = "codec";
static const std::string kMediaInfoTrackLanguage = "language";

static const std::string kLanguageUndefine = "und";
static const std::string kAudioCodecAac = "aac";
static const std::string kVideoCodecH264 = "h264";

static const SnySI32 kSI32MaxValue = 0x7FFFFFFF;

static const SnyUI64 kTimescaleMicrosecond = 1000 * 1000;
static const SnyUI64 kTimescaleMillisecond = 1000;

static const SnySI64 kHlsMaxSegmentDurationSecond = 10;
static const std::string kNewLineTag = "\n";

static const std::string kMediaServerSystem = "system";
static const std::string kMediaServerMedia = "media";
static const std::string kMediaServerWeb = "web";
static const std::string kMediaServerVod = "vod";
static const std::string kMediaServerLive = "live";

static const std::string kMediaServerGetDirectoryContent =
    "getDirectoryContent";
static const std::string kMediaServerGetSystemInfo = "getSystemInfo";
static const std::string kMediaServerGetMediaInfo = "getMediaInfo";

static const SnySI32 kMediaServerMaxRequestContentSize =
    10 * 1024;  // 10K bytes

static const std::string kMediaServerRequestMediaPath = "path";

static const std::string kPlaylistProtocol = "protocol";
static const std::string kPlaylistProtocolHls = "hls";
static const std::string kPlaylistProtocolDash = "dash";
static const std::string kPlaylistProtocolFile = "file";
static const std::string kPlaylistUri = "uri";

static const std::string kHttp = "http://";
static const std::string kForwardSlash = "/";

enum SnyFileDir { kFileTypeUnknown = 0, kDir, kFile };

enum SnyServiceType {
  kServiceUnknown = 0,
  kServiceSystem,
  kServiceMedia,
  kServiceWeb,

  kServiceSystemGetDirectoryContent = 100,
  kServiceSystemGetSystemInfo,

  kServiceMediaGetMediaInfo = 200,
  kServiceMediaVod,
  kServiceMediaLive,
};

enum SnyMediaType {
  kMediaTypeUnknown = 0,
  kMediaTypeAudio,
  kMediaTypeVideo,
  kMediaTypeSubtitle,
};

enum SnyCodec {
  kCodecUnknown = 0,
  kCodecAac,
  kCodecAc3,
  kCodecH264,
  kCodecH265,
};

}  // namespace sny
#endif  // !STREAMINGNOLOGY_CORE_SNYCONSTANTS_H
