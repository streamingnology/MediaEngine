/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */

#include "core/snyutils.h"
#include <cstdarg>
#include <memory>
#include "Ap4FileByteStream.h"
#include "core/snyplatform.h"
#include "core/snytype.h"
#include "snyeasylogging.h"
#include "snyplatform.h"

namespace sny {
namespace SnyUtils {
  SnyUI64 convertTime(SnyUI64 time_value, SnyUI64 from_time_scale, SnyUI64 to_time_scale) {
    if (from_time_scale == 0) return 0;
    double ratio = (double)to_time_scale / (double)from_time_scale;
    return ((SnyUI64)(0.5 + (double)time_value * ratio));
  }

  std::string formatstring(const char* format, ...) {
    va_list list;
    va_start(list, format);
    std::string s = formatstring(format, &(list[0]));
    va_end(list);
    return s;
  }

  std::string formatstring(const char* fmt, va_list vl) {
    va_list list;
    va_copy(list, vl);
    const int len = std::vsnprintf(nullptr, 0, fmt, vl);
    std::vector<char> buffer(len + 1);
    va_copy(vl, &(list[0]));
    std::vsnprintf(buffer.data(), buffer.size(), fmt, vl);
    va_end(list);

    return std::string(buffer.data(), len);
  }
  SnyDataBuffer loadConfig(std::filesystem::path cnf) {
    SnyDataBuffer data_buffer;
    if (cnf.empty()) {
      LOG(ERROR) << "config relative path is empty";
      return data_buffer;
    }

    auto cwd_absolute = std::filesystem::current_path();
    auto cnf_absolute_path = cwd_absolute / cnf;
    LOG(INFO) << "try to load config... ";
    LOG(INFO) << "config file: " << cnf_absolute_path.string();

    AP4_ByteStream* byte_stream = nullptr;
    do {
      AP4_Result result = AP4_FileByteStream::Create(cnf_absolute_path.string().c_str(),
                                                     AP4_FileByteStream::STREAM_MODE_READ, byte_stream);
      if (AP4_FAILED(result)) {
        LOG(ERROR) << "open config.json failed";
        break;
      }
      LOG(INFO) << "load config ok";
      sny::SnyUI64 size;
      byte_stream->GetSize(size);
      data_buffer.resize(size);
      result = byte_stream->Read(data_buffer.data(), data_buffer.size());
      if (AP4_FAILED(result)) {
        data_buffer.clear();
        LOG(ERROR) << "read config failed";
        break;
      }
    } while (false);
    byte_stream->Release();
    byte_stream = nullptr;

    return data_buffer;
  }
}  // namespace SnyUtils
}  // namespace sny