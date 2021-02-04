//==============================================================================
//
//  RtmpProvider
//
//  Created by Kwon Keuk Han
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================
#pragma once
#include <chrono>
#include <deque>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include "core/snyconstants.h"
#include "core/snythreads.h"
#include "core/url.h"
#include "media/common_types.h"
#include "media/media_buffer.h"
#include "media/media_track.h"
#include "media/rtmp/amf_document.h"
#include "media/rtmp/rtmp_chunk_parser.h"
#include "media/rtmp/rtmp_export_chunk.h"
#include "media/rtmp/rtmp_handshake.h"
#include "media/rtmp/rtmp_import_chunk.h"
#include "media/snymediasample.h"
#include "source/snysourcecallback.h"
#define MAX_STREAM_MESSAGE_COUNT (100)
#define BASELINE_PROFILE (66)
#define MAIN_PROFILE (77)

// Fix track id
#define RTMP_VIDEO_TRACK_ID 0
#define RTMP_AUDIO_TRACK_ID 1

namespace pvd {
const int kRtmpStreamParseThread = 0;
using OnRTMPSendDataCallback = std::function<void(std::string &conn_name, const char *data, const int size)>;
class RtmpStream {
 public:
  static std::shared_ptr<RtmpStream> Create(std::string conn_name);

  explicit RtmpStream(std::string conn_name);
  ~RtmpStream();

  bool OnDataReceived(const char *data_buffer, int data_size);
  bool AddTrack(std::shared_ptr<MediaTrack> track);
  std::shared_ptr<MediaTrack> GetTrack(int32_t id);
  void setRTMPCallback(sny::SnySourceCallback *call_back) { call_back_ = call_back; }
  void setRTMPSendDataCallback(OnRTMPSendDataCallback callback) { send_data_callback_ = std::move(callback); }
  int onThreadProc(int id);
  void start();
  void stop();

 protected:
  bool ConvertToSnyMediaSample(std::shared_ptr<MediaTrack> &media_track, std::shared_ptr<MediaPacket> media_packet);
  bool SendFrame(std::shared_ptr<sny::SnyMediaSample> media_sample);

 private:
  // AMF Event
  void OnAmfConnect(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document, double transaction_id);
  void OnAmfCreateStream(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document,
                         double transaction_id);
  void OnAmfFCPublish(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document,
                      double transaction_id);
  void OnAmfPublish(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document, double transaction_id);
  void OnAmfDeleteStream(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document,
                         double transaction_id);
  bool OnAmfMetaData(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document, int32_t object_index);

  // Send messages
  bool SendData(int data_size, uint8_t *data);
  bool SendMessagePacket(std::shared_ptr<RtmpMuxMessageHeader> &message_header,
                         std::shared_ptr<std::vector<uint8_t>> &data);
  bool SendAcknowledgementSize(uint32_t acknowledgement_traffic);

  bool SendUserControlMessage(uint16_t message, std::shared_ptr<std::vector<uint8_t>> &data);
  bool SendWindowAcknowledgementSize(uint32_t size);
  bool SendSetPeerBandwidth(uint32_t bandwidth);
  bool SendStreamBegin();
  bool SendStreamEnd();
  bool SendAcknowledgementSize();
  bool SendAmfCommand(std::shared_ptr<RtmpMuxMessageHeader> &message_header, AmfDocument &document);
  bool SendAmfConnectResult(uint32_t chunk_stream_id, double transaction_id, double object_encoding);
  bool SendAmfOnFCPublish(uint32_t chunk_stream_id, uint32_t stream_id, double client_id);
  bool SendAmfCreateStreamResult(uint32_t chunk_stream_id, double transaction_id);
  bool SendAmfOnStatus(uint32_t chunk_stream_id, uint32_t stream_id, char *level, char *code, char *description,
                       double client_id);

  // Parsing handshake messages
  off_t ReceiveHandshakePacket(const std::shared_ptr<const ov::Data> &data);
  bool SendHandshake(const std::shared_ptr<const ov::Data> &data);

  // Parsing chunk messages
  int32_t ReceiveChunkPacket(const std::shared_ptr<const ov::Data> &data);
  bool ReceiveChunkMessage();

  bool ReceiveSetChunkSize(const std::shared_ptr<const RtmpMessage> &message);
  void ReceiveWindowAcknowledgementSize(const std::shared_ptr<const RtmpMessage> &message);
  void ReceiveAmfCommandMessage(const std::shared_ptr<const RtmpMessage> &message);
  void ReceiveAmfDataMessage(const std::shared_ptr<const RtmpMessage> &message);

  bool ReceiveAudioMessage(const std::shared_ptr<const RtmpMessage> &message);
  bool ReceiveVideoMessage(const std::shared_ptr<const RtmpMessage> &message);

  ov::String GetCodecString(RtmpCodecType codec_type);
  ov::String GetEncoderTypeString(RtmpEncoderType encoder_type);

  bool CheckReadyToPublish();
  bool PublishStream();
  bool SetTrackInfo(const std::shared_ptr<RtmpMediaInfo> &media_info);

  bool CheckSignedPolicy();

  // RTMP related
  RtmpHandshakeState _handshake_state = RtmpHandshakeState::Uninitialized;

  std::shared_ptr<RtmpImportChunk> _import_chunk;
  std::shared_ptr<RtmpExportChunk> _export_chunk;
  std::shared_ptr<RtmpMediaInfo> _media_info;

  std::vector<std::shared_ptr<const RtmpMessage>> _stream_message_cache;
  uint32_t _stream_message_cache_video_count = 0;
  uint32_t _stream_message_cache_audio_count = 0;

  uint32_t _acknowledgement_size = RTMP_DEFAULT_ACKNOWNLEDGEMENT_SIZE / 2;
  uint32_t _acknowledgement_traffic = 0;
  uint32_t _rtmp_stream_id = 1;
  uint32_t _peer_bandwidth = RTMP_DEFAULT_PEER_BANDWIDTH;
  double _client_id = 12345.0;
  // Set from OnAmfPublish
  int32_t _chunk_stream_id = 0;

  // parsed from packet
  std::shared_ptr<ov::Url> _url = nullptr;

  ov::String _full_url;  // with stream_name
  ov::String _tc_url;
  ov::String _app_name;
  ov::String _domain_name;
  ov::String _stream_name;
  ov::String _device_string;

  bool IsPublished() { return published_; }
  bool published_;
  std::string conn_name_;
  OnRTMPSendDataCallback send_data_callback_;
  std::map<int32_t, std::shared_ptr<MediaTrack>> _tracks;
  std::map<int32_t, sny::SnyCodecType> track_codec_types_;
  bool track_info_sent_ = false;
  sny::Threads<RtmpStream> threads_;

  // Received data buffer
  std::mutex mutex_;
  std::mutex mutex_cv_;
  std::condition_variable cv_;
  std::deque<std::shared_ptr<ov::Data>> incomming_data_;
  std::shared_ptr<ov::Data> _remained_data = nullptr;

  sny::SnySourceCallback *call_back_ = nullptr;
  // For statistics
  time_t _stream_check_time = 0;
  uint32_t _key_frame_interval = 0;
  uint32_t _previous_key_frame_timestamp = 0;
  uint32_t _last_video_timestamp = 0;
  uint32_t _last_audio_timestamp = 0;
  uint32_t _previous_last_video_timestamp = 0;
  uint32_t _previous_last_audio_timestamp = 0;
  uint32_t _video_frame_count = 0;
  uint32_t _audio_frame_count = 0;
};
}  // namespace pvd