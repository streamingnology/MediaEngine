/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */
#include "core/snymachineinfo.h"
#include "core/snyplatform.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#if defined(Q_OS_WIN32)
#include "utsname.h"
extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
}
#else
extern "C" {
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>
}
#endif  // Q_OS_WIN32

namespace sny {
SnyMachineInfo::SnyMachineInfo(SnyUI16 lp) { this->listening_port_ = lp; }

void SnyUtsName::operator=(const SnyUtsName &object) {
  this->sysname_ = object.sysname_;
  this->nodename_ = object.nodename_;
  this->release_ = object.release_;
  this->version_ = object.version_;
  this->machine_ = object.machine_;
}

SnyUtsName::SnyUtsName(const SnyUtsName &object) { operator=(object); }

SnyUtsName SnyMachineInfo::getUtsName() {
  SnyUtsName uts_name;
  struct utsname buf;
  if (uname(&buf)) {
    return uts_name;
  }

  uts_name.sysname_ = buf.sysname;
  uts_name.nodename_ = buf.nodename;
  uts_name.release_ = buf.release;
  uts_name.version_ = buf.version;
  uts_name.machine_ = buf.machine;

  return uts_name;
}

std::vector<std::string> SnyMachineInfo::getIpAddress() {
  std::vector<std::string> ips;
#if defined(Q_OS_WIN32)
#else
  const char *network_name[] = {"eth0", "wlan0", "en0"};
  for (int i = 0; i < 3; i++) {
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, network_name[i], IFNAMSIZ - 1);
    int err = ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    if (err != -1) {
      std::string ip(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
      ips.push_back(ip);
    }
  }
#endif  // Q_OS_WIN32
  return ips;
}

SnyDataBuffer SnyMachineInfo::getMachineInfoInJSON() {
  std::vector<std::string> ips = getIpAddress();
  SnyUtsName utsName = getUtsName();

  SnyDataBuffer playlist_buffer;
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  writer.StartObject();

  writer.Key("appVersion");
  writer.String(app_version_.c_str());

  writer.Key("uts");
  writer.StartArray();
  writer.StartObject();
  writer.Key("sysname");
  writer.String(utsName.sysname_.c_str());
  writer.Key("nodename");
  writer.String(utsName.nodename_.c_str());
  writer.Key("release");
  writer.String(utsName.release_.c_str());
  writer.Key("version");
  writer.String(utsName.version_.c_str());
  writer.Key("machine");
  writer.String(utsName.machine_.c_str());
  writer.EndObject();
  writer.EndArray();

  writer.Key("listeningPort");
  writer.Uint(listening_port_);

  writer.Key("ip");
  writer.StartArray();

  int i = 0;
  for (auto iter = ips.begin(); iter != ips.end(); iter++, i++) {
    writer.StartObject();
    writer.Key(std::to_string(i).data());
    writer.String(iter->data());
    writer.EndObject();
  }
  if (ips.empty()) {
    writer.StartObject();
    writer.Key(std::to_string(i).data());
    writer.String("localhost");
    writer.EndObject();
  }
  writer.EndArray();

  writer.EndObject();

  std::string s = buffer.GetString();
  playlist_buffer.append(s.data(), static_cast<int>(s.size()));

  return playlist_buffer;
}

void SnyMachineInfo::setAppVersion(std::string app_version) { this->app_version_ = app_version; }
}  // namespace sny