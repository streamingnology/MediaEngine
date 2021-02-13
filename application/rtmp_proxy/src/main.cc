#include <Ap4FileByteStream.h>
#include <core/snydatabuffer.h>
#include <core/snyeasylogging.h>
#include <core/snyresults.h>
#include <core/snyutils.h>
#include <core/socket_manager/sny_socket_manager.h>
#include <csignal>
#include <filesystem>
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

  // "$cwd/../conf/config.json"
  std::filesystem::path cwd_relative_path = "..";
  auto cnf_relative_path = cwd_relative_path / "conf" / "config.json";

  bool success = false;
  do {
    auto cnf_data = sny::SnyUtils::loadConfig(cnf_relative_path);
    if (cnf_data.isEmpty()) {
      break;
    }

    LOG(INFO) << "try to parse config...";
    auto rtmp_proxy_cnf = app::parse(cnf_data);
    if (rtmp_proxy_cnf == nullptr) {
      LOG(ERROR) << "parse config failed";
      break;
    }
    LOG(INFO) << "parse config ok";

    std::map<std::string, std::shared_ptr<app::SnyMultiRTMPPublish>> rtmp_proxys;
    auto rtmp_proxy = std::make_shared<app::SnyRTMPProxy>();
    rtmp_proxy->setConfigure(rtmp_proxy_cnf);
    auto result = rtmp_proxy->start();
    if (sny::SnySuccess != result) {
      LOG(ERROR) << "start rtmp proxy failed";
      break;
    }
    success = true;
    while (!g_terminated) {
      std::this_thread::sleep_for(100ms);
    }
  } while (false);

  return success ? sny::SnySuccess : sny::SnyFailture;
}
