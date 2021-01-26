/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 */

#include "core/snyutils.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include "core/snyplatform.h"
#include "core/snytype.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#if defined(Q_OS_WIN32)
#include <fstream>
#include <iostream>
extern "C" {
#include <event2/http.h>
#include <stdio.h>
#include <string.h> /* for strncpy */
#include <sys/types.h>
}
#include <Windows.h>
#include <experimental/filesystem>
#include <filesystem>
using namespace std;
namespace fs = std::experimental::filesystem;
#elif defined(Q_OS_MACX)
extern "C" {
#include <arpa/inet.h>
#include <dirent.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h> /* for strncpy */
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
}
#elif defined(Q_OS_LINUX)
extern "C" {
#include <arpa/inet.h>
#include <dirent.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */
#include <stdio.h>
#include <string.h> /* for strncpy */
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
}
#else
#error You should check your platform
extern "C" {
#include <arpa/inet.h>
#include <dirent.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */
#include <stdio.h>
#include <string.h> /* for strncpy */
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
}
#endif  // Q_OS_WIN32

namespace sny {
namespace SnyUtils {
  SnyUI64 convertTime(SnyUI64 time_value, SnyUI64 from_time_scale, SnyUI64 to_time_scale) {
    if (from_time_scale == 0) return 0;
    double ratio = (double)to_time_scale / (double)from_time_scale;
    return ((SnyUI64)(0.5 + (double)time_value * ratio));
  }

  std::string formatstring(const char* format, ...) {
    const int BUFFER_SIZE = 9216;
    char strbuf[BUFFER_SIZE];
    va_list ap;
    va_start(ap, format);
#if defined(Q_OS_WIN32)
    int result = vsnprintf_s(strbuf, BUFFER_SIZE, format, ap);
#else
    int result = vsnprintf(strbuf, BUFFER_SIZE, format, ap);
#endif
    va_end(ap);
    return std::string(strbuf, result);
  }

  std::vector<std::string> splitUri(std::string uri) {
    std::vector<std::string> vec;

    std::regex words_regex("([^/]+)");
    auto words_begin = std::sregex_iterator(uri.begin(), uri.end(), words_regex);
    auto words_end = std::sregex_iterator();

    for (auto i = words_begin; i != words_end; ++i) {
      std::smatch match = *i;
      std::string match_str = match.str();
      vec.push_back(match_str);
    }

    return vec;
  }

  SnyDataBuffer listDirectoryContent(std::string path) {
    SnyDataBuffer content;

    std::vector<std::string> fileList;
    std::vector<std::string> folderList;
#if defined(Q_OS_WIN32)
    if (path.empty()) {
      DWORD dwSize = MAX_PATH;
      char szLogicalDrives[MAX_PATH] = {0};
      DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);
      if (dwResult > 0 && dwResult <= MAX_PATH) {
        char* szSingleDrive = szLogicalDrives;
        while (*szSingleDrive) {
          folderList.push_back(szSingleDrive);
          szSingleDrive += strlen(szSingleDrive) + 1;
        }
      }
    } else {
      fs::path u8_path = fs::u8path(path);
      for (auto& fe : fs::directory_iterator(u8_path)) {
        auto fp = fe.path();
        auto fFiename = fp.filename();
        if (fs::is_directory(fe) && !fs::is_empty(fe)) {
          if (fFiename != "." && fFiename != "..") {
            folderList.push_back(fFiename.generic_u8string());
          }
        } else if (fs::is_regular_file(fe)) {
          std::string ext;
          if (fp.has_extension()) {
            ext = fp.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
            if (ext.compare(".mp4") == 0) {
              fileList.push_back(fFiename.generic_u8string());
            }
          }
        }
      }
    }
#elif defined(Q_OS_MACX)
    if (path.empty()) {
      folderList.push_back("/Users");
      folderList.push_back("/Volumes");
    } else {
      DIR *dp;
      struct dirent *dirp;
      if ((dp = opendir(path.c_str())) == nullptr) {
        return content;
      }
      while ((dirp = readdir(dp)) != nullptr) {
        if (dirp->d_type == DT_REG) {
          std::string name(dirp->d_name, dirp->d_namlen);
          if (name.compare(0, 1, ".") != 0) {
            std::string ext = name.substr(name.find_last_of('.') + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
            if (ext.compare("mp4") == 0) {
              fileList.push_back(name);
            }
          }
        } else if (dirp->d_type == DT_DIR) {
          std::string name(dirp->d_name, dirp->d_namlen);
          if (name.compare(".") != 0 && name.compare("..") != 0 && name.compare(0, 1, ".") != 0) {
            folderList.push_back(name);
          }
        }
      }
      closedir(dp);
    }
#elif defined(Q_OS_LINUX)
    if (path.empty()) {
      folderList.push_back("/");
      folderList.push_back("/home");
    } else {
      DIR* dp;
      struct dirent* dirp;
      if ((dp = opendir(path.c_str())) == nullptr) {
        return content;
      }
      while ((dirp = readdir(dp)) != nullptr) {
        long name_max = pathconf(path.c_str(), _PC_NAME_MAX);
        if (name_max == -1) /* Limit not defined, or error */
          name_max = 255;   /* Take a guess */
        size_t len = offsetof(struct dirent, d_name) + name_max + 1;
        if (dirp->d_type == DT_REG) {
          std::string name(dirp->d_name, len);
          if (name.compare(0, 1, ".") != 0) {
            fileList.push_back(name);
          }
        } else if (dirp->d_type == DT_DIR) {
          std::string name(dirp->d_name, len);
          if (name.compare(".") != 0 && name.compare("..") != 0 && name.compare(0, 1, ".") != 0) {
            folderList.push_back(name);
          }
        }
      }
      closedir(dp);
    }
#else
#error You should check your platform
    if (path.empty()) {
      folderList.push_back("/");
      folderList.push_back("/home");
    } else {
      DIR* dp;
      struct dirent* dirp;
      if ((dp = opendir(path.c_str())) == nullptr) {
        return content;
      }
      while ((dirp = readdir(dp)) != nullptr) {
        long name_max = pathconf(path.c_str(), _PC_NAME_MAX);
        if (name_max == -1) /* Limit not defined, or error */
          name_max = 255;   /* Take a guess */
        size_t len = offsetof(struct dirent, d_name) + name_max + 1;
        if (dirp->d_type == DT_REG) {
          std::string name(dirp->d_name, len);
          if (name.compare(0, 1, ".") != 0) {
            fileList.push_back(name);
          }
        } else if (dirp->d_type == DT_DIR) {
          std::string name(dirp->d_name, len);
          if (name.compare(".") != 0 && name.compare("..") != 0 && name.compare(0, 1, ".") != 0) {
            folderList.push_back(name);
          }
        }
      }
      closedir(dp);
    }
#endif  // Q_OS_WIN32

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.Key("status");
    writer.Int(200);

    writer.Key("data");
    writer.StartArray();
    for (auto iter = folderList.begin(); iter != folderList.end(); iter++) {
      std::string fileName = *iter;
      std::string fullName = *iter;
      if (path.empty()) {
        // TODO:
      } else {
#if defined(Q_OS_WIN32)
        std::filesystem::path p = path;
        std::filesystem::path fp = p / *iter;
        fullName = fp.string();
#else
        fullName = path + "/" + *iter;
#endif
      }

      writer.StartObject();
      writer.Key("label");
      writer.String(fileName.data(), static_cast<rapidjson::SizeType>(fileName.size()));
      writer.Key("path");
      writer.String(fullName.data(), static_cast<rapidjson::SizeType>(fullName.size()));
      writer.Key("last");
      writer.Bool(false);
      writer.EndObject();
    }
    for (auto iter = fileList.begin(); iter != fileList.end(); iter++) {
      std::string fileName = *iter;
      std::string fullName = path + "/" + *iter;
      writer.StartObject();
      writer.Key("label");
      writer.String(fileName.data(), static_cast<rapidjson::SizeType>(fileName.size()));
      writer.Key("path");
      writer.String(fullName.data(), static_cast<rapidjson::SizeType>(fullName.size()));
      writer.Key("last");
      writer.Bool(true);
      writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();

    std::string s = buffer.GetString();
    content.append(s.data(), static_cast<int>(s.size()));

    return content;
  }

  std::map<int, std::vector<std::string>> getDirectoryContent(std::string path) {
    std::vector<std::string> folderList;
    std::vector<std::string> fileList;
    std::map<int, std::vector<std::string>> m;

#if defined(Q_OS_WIN32)
    if (path.empty()) {
      DWORD dwSize = MAX_PATH;
      char szLogicalDrives[MAX_PATH] = {0};
      DWORD dwResult = GetLogicalDriveStrings(dwSize, szLogicalDrives);
      if (dwResult > 0 && dwResult <= MAX_PATH) {
        char* szSingleDrive = szLogicalDrives;
        while (*szSingleDrive) {
          folderList.push_back(szSingleDrive);
          szSingleDrive += strlen(szSingleDrive) + 1;
        }
      }
    } else {
      fs::path u8_path = fs::u8path(path);
      for (auto& fe : fs::directory_iterator(u8_path)) {
        auto fp = fe.path();
        auto fFiename = fp.filename();
        if (fs::is_directory(fe) && !fs::is_empty(fe)) {
          if (fFiename != "." && fFiename != "..") {
            folderList.push_back(fFiename.generic_u8string());
          }
        } else if (fs::is_regular_file(fe)) {
          std::string ext;
          if (fp.has_extension()) {
            ext = fp.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
            if (ext.compare(".mp4") == 0) {
              fileList.push_back(fFiename.generic_u8string());
            }
          }
        }
      }
    }
#elif defined(Q_OS_MACX)
    if (path.empty()) {
      folderList.push_back("/Users");
      folderList.push_back("/Volumes");
    } else {
      DIR *dp;
      struct dirent *dirp;
      if ((dp = opendir(path.c_str())) == nullptr) {
        m.insert(std::make_pair(0, folderList));
        m.insert(std::make_pair(1, fileList));
        return m;
      }
      while ((dirp = readdir(dp)) != nullptr) {
        if (dirp->d_type == DT_REG) {
          std::string name(dirp->d_name, dirp->d_namlen);
          if (name.compare(0, 1, ".") != 0) {
            std::string ext = name.substr(name.find_last_of('.') + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return std::tolower(c); });
            fileList.push_back(name);
          }
        } else if (dirp->d_type == DT_DIR) {
          std::string name(dirp->d_name, dirp->d_namlen);
          if (name.compare(".") != 0 && name.compare("..") != 0 && name.compare(0, 1, ".") != 0) {
            folderList.push_back(name);
          }
        }
      }
      closedir(dp);
    }
#elif defined(Q_OS_LINUX)
    if (path.empty()) {
      folderList.push_back("/");
      folderList.push_back("/home");
    } else {
      DIR* dp;
      struct dirent* dirp;
      if ((dp = opendir(path.c_str())) == nullptr) {
        return m;
      }
      while ((dirp = readdir(dp)) != nullptr) {
        long name_max = pathconf(path.c_str(), _PC_NAME_MAX);
        if (name_max == -1) /* Limit not defined, or error */
          name_max = 255;   /* Take a guess */
        size_t len = offsetof(struct dirent, d_name) + name_max + 1;
        if (dirp->d_type == DT_REG) {
          std::string name(dirp->d_name, len);
          if (name.compare(0, 1, ".") != 0) {
            fileList.push_back(name);
          }
        } else if (dirp->d_type == DT_DIR) {
          std::string name(dirp->d_name, len);
          if (name.compare(".") != 0 && name.compare("..") != 0 && name.compare(0, 1, ".") != 0) {
            folderList.push_back(name);
          }
        }
      }
      closedir(dp);
    }
#else
#error You should check your platform
    if (path.empty()) {
      folderList.push_back("/");
      folderList.push_back("/home");
    } else {
      DIR* dp;
      struct dirent* dirp;
      if ((dp = opendir(path.c_str())) == nullptr) {
        return content;
      }
      while ((dirp = readdir(dp)) != nullptr) {
        long name_max = pathconf(path.c_str(), _PC_NAME_MAX);
        if (name_max == -1) /* Limit not defined, or error */
          name_max = 255;   /* Take a guess */
        size_t len = offsetof(struct dirent, d_name) + name_max + 1;
        if (dirp->d_type == DT_REG) {
          std::string name(dirp->d_name, len);
          if (name.compare(0, 1, ".") != 0) {
            fileList.push_back(name);
          }
        } else if (dirp->d_type == DT_DIR) {
          std::string name(dirp->d_name, len);
          if (name.compare(".") != 0 && name.compare("..") != 0 && name.compare(0, 1, ".") != 0) {
            folderList.push_back(name);
          }
        }
      }
      closedir(dp);
    }
#endif  // Q_OS_WIN32
    m.insert(std::make_pair(0, folderList));
    m.insert(std::make_pair(1, fileList));
    return m;
  }

  std::vector<std::string> getIpAddress() {
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

  std::string getCurrentAppDirectory() {
    std::string pwd;
    char* path;
    path = getenv("PWD");
    if (path != nullptr) {
      pwd = path;
    }
    return pwd;
  }

  SnyServiceType getServiceTypeByPath(const std::string path) {
    SnyServiceType snyServiceType = kServiceUnknown;
    if (path.compare(kMediaServerSystem) == 0) {
      snyServiceType = kServiceSystem;
    } else if (path.compare(kMediaServerMedia) == 0) {
      snyServiceType = kServiceMedia;
    } else if (path.compare(kMediaServerWeb) == 0) {
      snyServiceType = kServiceWeb;
    } else if (path.compare(kMediaServerVod) == 0) {
      snyServiceType = kServiceMediaVod;
    } else if (path.compare(kMediaServerLive) == 0) {
      snyServiceType = kServiceMediaLive;
    } else if (path.compare(kMediaServerGetMediaInfo) == 0) {
      snyServiceType = kServiceMediaGetMediaInfo;
    } else if (path.compare(kMediaServerGetDirectoryContent) == 0) {
      snyServiceType = kServiceSystemGetDirectoryContent;
    } else if (path.compare(kMediaServerGetSystemInfo) == 0) {
      snyServiceType = kServiceSystemGetSystemInfo;
    } else {
      snyServiceType = kServiceUnknown;
    }
    return snyServiceType;
  }

  std::string ByteArrayToHexString(const uint8_t* in_buffer, size_t length) {
    static const char kHexChars[] = "0123456789ABCDEF";
    // Each input byte creates two output hex characters.
    std::string out_buffer(length * 2, '\0');
    for (size_t i = 0; i < length; ++i) {
      char byte = in_buffer[i];
      out_buffer[(i * 2)] = kHexChars[(byte >> 4) & 0xf];
      out_buffer[(i * 2) + 1] = kHexChars[byte & 0xf];
    }
    return out_buffer;
  }

  SnyFileDir isFileorDir(std::string& path) {
#if defined(Q_OS_WIN32)
#error You should check your platform
#elif defined(Q_OS_MACX) || defined(Q_OS_LINUX)
    struct stat s;
    if (stat(path.data(), &s) == 0) {
      if (s.st_mode & S_IFDIR) {
        return SnyFileDir::kDir;
      } else if (s.st_mode & S_IFREG) {
        return SnyFileDir::kFile;
      } else {
        return SnyFileDir::kFileTypeUnknown;
      }
    } else {
      return SnyFileDir::kFileTypeUnknown;
    }
#else
#error You should check your platform
#endif  // Q_OS_WIN32
  }

}  // namespace SnyUtils
}  // namespace sny