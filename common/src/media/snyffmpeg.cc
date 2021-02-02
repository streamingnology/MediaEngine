/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snyffmpeg.h"
#include <mutex>
#include "core/log.h"
#include "core/snyeasylogging.h"
#include "core/string.h"
#include "core/snyutils.h"

namespace sny {
static std::mutex g_ffmpeg_mutex;
static bool g_ffmpeg_initialized = false;

bool initialize_ffmpeg() {
  // TODO:: deprecated? check later
  // ffmpeg thread safe in 4.3.1 version?
  g_ffmpeg_mutex.lock();
  do {
    if (g_ffmpeg_initialized) {
      break;
    }
    av_log_set_callback(ffmpeg_log_func);
    g_ffmpeg_initialized = true;
  } while (false);
  g_ffmpeg_mutex.unlock();
  return true;
}

void ffmpeg_log_func(void *ptr, int level, const char *fmt, va_list vl) {
  va_list vl2;
  char line[1024];
  static int print_prefix = 1;

  va_copy(vl2, vl);
  av_log_default_callback(ptr, level, fmt, vl);
  av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
  va_end(vl2);

  std::string log = SnyUtils::formatstring(fmt, vl);
  switch (level) {
    case AV_LOG_QUIET:
    case AV_LOG_DEBUG:
    case AV_LOG_VERBOSE:
      LOG(DEBUG)<<log;
      break;
    case AV_LOG_INFO:
      LOG(INFO)<<log;
      break;
    case AV_LOG_WARNING:
      LOG(WARNING)<<log;
      break;
    case AV_LOG_ERROR:
    case AV_LOG_FATAL:
    case AV_LOG_PANIC:
      LOG(ERROR)<<log;
      break;
    case AV_LOG_TRACE:
      //LOG(TRACE) << log;
      break;
    default:
      break;
  }
}

}  // namespace sny