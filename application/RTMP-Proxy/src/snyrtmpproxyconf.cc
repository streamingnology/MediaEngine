/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snyrtmpproxyconf.h"
#include <rapidjson/rapidjson.h>
#include <string>
#include <iostream>
#include "rapidjson/document.h"
#include <vector>
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <core/snydatabuffer.h>
#include <easylogging++.h>

namespace app {
std::shared_ptr<SnyRTMPProxyConf> parse(sny::SnyDataBuffer& cnf_json) {
  if (cnf_json.isEmpty()) {
    LOG(ERROR)<<"JSON is empty";
    return nullptr;
  }
  auto cnf = std::make_shared<SnyRTMPProxyConf>();
  rapidjson::Document document;
  document.Parse(cnf_json.data());
  if (!document.IsObject()) {
    LOG(ERROR) << "invalid JSON file";
    return nullptr;
  }
  cnf->name = document["name"].GetString();
  cnf->version = document["version"].GetInt();
  cnf->rtmp_port = document["rtmp_port"].GetInt();

  const rapidjson::Value& proxy = document["proxy"];
  for (rapidjson::SizeType i = 0; i < proxy.Size(); i++) {
    const rapidjson::Value& app_items = proxy[i];
    std::string app_name = app_items["application_name"].GetString();
    std::string stream_name = app_items["stream_name"].GetString();
    for (const auto& item : cnf->streams_) {
      if (item.app_name_==app_name && item.stream_name_==stream_name) {
        LOG(WARNING)<< "there are two same stream: " \
                 << "app name: " << app_name \
                 << "stream name: " << stream_name;
        continue;
      }
    }
    SnyRTMPProxyStream streams;
    streams.app_name_ = app_name; streams.stream_name_ = stream_name;
    const rapidjson::Value& stream_items = app_items["publish_to"];
    for (rapidjson::SizeType j = 0; j < stream_items.Size(); j++) {
      const rapidjson::Value& stream_item = stream_items[j];
      std::string publish_name = stream_item["publish_name"].GetString();
      std::string publish_url  = stream_item["publish_url"].GetString();
      for (const auto& item : streams.publish_items_) {
        if (item.publish_url_ == publish_url) {
          LOG(WARNING)<< "there are two same publish urls: " \
                   << "publish url: " << publish_url;
          continue;
        }
      }
      SnyRTMPProxyPublishItem publish_item;
      publish_item.publish_url_ = publish_url; publish_item.publish_name_ = publish_name;
      streams.publish_items_.push_back(publish_item);
    }
    if (!streams.publish_items_.empty()) {
      cnf->streams_.push_back(streams);
    }
  }
  return cnf;
}

}