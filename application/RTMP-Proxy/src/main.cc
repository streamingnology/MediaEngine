#include <Ap4FileByteStream.h>
#include <core/snydatabuffer.h>
#include <core/snyeasylogging.h>
#include <core/snyresults.h>
#include <csignal>
#include <iostream>
#include <map>
#include "snymultirtmppublish.h"
#include "snyrtmpproxy.h"
#include "snyrtmpproxyconf.h"

bool g_terminated = false;

void signalHandler(int signal_number) {
  LOG(WARNING) << "received signal: " << signal_number;
  g_terminated = true;
}

int main(int argc, char** args) {
  signal(SIGINT, signalHandler);

  el::Configurations conf("../conf/el.conf");
  el::Loggers::reconfigureAllLoggers(conf);

  std::string rtmp_proxy_cnf_file = "../conf/config.json";
  bool success = false;
  AP4_ByteStream* byte_stream = nullptr;
  do {
    AP4_Result result =
        AP4_FileByteStream::Create(rtmp_proxy_cnf_file.c_str(), AP4_FileByteStream::STREAM_MODE_READ, byte_stream);
    if (AP4_FAILED(result)) {
      LOG(ERROR) << "open config.json failed";
      break;
    }
    sny::SnyUI64 size;
    byte_stream->GetSize(size);
    sny::SnyDataBuffer conf_data;
    conf_data.resize(size);
    result = byte_stream->Read(conf_data.data(), conf_data.size());
    if (AP4_FAILED(result)) {
      LOG(ERROR) << "read config.json failed";
      break;
    }
    byte_stream->Release();
    byte_stream = nullptr;

    auto rtmp_proxy_cnf = app::parse(conf_data);
    if (rtmp_proxy_cnf == nullptr) {
      LOG(ERROR) << "parse config.json failed";
      break;
    }

    std::map<std::string, std::shared_ptr<app::SnyMultiRTMPPublish>> rtmp_proxys;
    auto rtmp_proxy = std::make_shared<app::SnyRTMPProxy>();
    rtmp_proxy->setConfigure(rtmp_proxy_cnf);
    result = rtmp_proxy->start();
    if (sny::SnySuccess != result) {
      LOG(ERROR) << "start rtmp proxy failed";
      break;
    }
    success = true;
    while (!g_terminated) {
      std::this_thread::sleep_for(100ms);
    }
  } while (false);

  if (byte_stream) {
    byte_stream->Release();
  }
  return success ? sny::SnySuccess : sny::SnyFailture;
}
