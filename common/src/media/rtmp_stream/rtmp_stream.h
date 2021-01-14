//==============================================================================
//
//  RtmpProvider
//
//  Created by Kwon Keuk Han
//  Copyright (c) 2018 AirenSoft. All rights reserved.
//
//==============================================================================

#pragma once
#include <iostream>
#include "../rtmp/helper/common_types.h"
#include "../rtmp/helper/media_track.h"
#include "../rtmp/helper/url.h"
#include "../rtmp/helper/media_buffer.h"

#include "../rtmp/amf_document.h"
#include "../rtmp/rtmp_chunk_parser.h"
#include "../rtmp/rtmp_export_chunk.h"
#include "../rtmp/rtmp_handshake.h"
#include "../rtmp/rtmp_import_chunk.h"
#include <uv11.hpp>
#include <media/snympeg2ts.h>
#include <Ap4FileByteStream.h>
#include <core/snyconstants.h>
#include "media/bitstream/h264/h264_avcc_to_annexb.h"
#include <media/bitstream/aac/aac_latm_to_adts.h>
#include <Ap4.h>
#include "core/event.h"
#include <memory>
#define MAX_STREAM_MESSAGE_COUNT (100)
#define BASELINE_PROFILE (66)
#define MAIN_PROFILE (77)

// Fix track id
#define RTMP_VIDEO_TRACK_ID		0
#define RTMP_AUDIO_TRACK_ID		1

namespace pvd
{
	class RtmpStream
	{
	public:
		static std::shared_ptr<RtmpStream> Create(StreamSourceType source_type, uint32_t channel_id);
		
		explicit RtmpStream(StreamSourceType source_type, uint32_t channel_id);
		~RtmpStream();

    bool OnDataReceived(const std::shared_ptr<const ov::Data> &data);
    void SetConn(std::shared_ptr<uv::TcpConnection> conn) {
      this->conn_ = conn;
    }
	protected:
		bool ConvertToVideoData(const std::shared_ptr<ov::Data> &data, int64_t &cts);
		bool ConvertToAudioData(const std::shared_ptr<ov::Data> &data);
	private:
		// AMF Event
		void OnAmfConnect(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document, double transaction_id);
		void OnAmfCreateStream(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document, double transaction_id);
		void OnAmfFCPublish(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document, double transaction_id);
		void OnAmfPublish(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document, double transaction_id);
		void OnAmfDeleteStream(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document, double transaction_id);
		bool OnAmfMetaData(const std::shared_ptr<const RtmpChunkHeader> &header, AmfDocument &document, int32_t object_index);


		// Send messages
		bool SendData(int data_size, uint8_t *data);
		bool SendMessagePacket(std::shared_ptr<RtmpMuxMessageHeader> &message_header, std::shared_ptr<std::vector<uint8_t>> &data);
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
		bool SendAmfOnStatus(uint32_t chunk_stream_id,
							uint32_t stream_id,
							char *level,
							char *code,
							char *description,
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

		ov::String _full_url; // with stream_name
		ov::String _tc_url;
		ov::String _app_name;
		ov::String _domain_name;
		ov::String _stream_name;
		ov::String _device_string;

    AP4_ByteStream* output_ = nullptr;
    std::string output_file_name = "/Users/developer/Desktop/tmp/rtmp.264";

    bool IsPublished() {
      return published_;
    }
    bool published_;
    std::vector<uint8_t> video_extra_data_;
    std::vector<uint8_t> audio_extra_data_;

    bool SendFrame(const std::shared_ptr<MediaPacket> &packet) {
      //TODO:
      if (false) {
        AP4_Result result = AP4_FileByteStream::Create(output_file_name.c_str(), AP4_FileByteStream::STREAM_MODE_WRITE, output_);
      }
      if (ts_muxer_ == nullptr) {
        ts_muxer_ = createTsMuxer();
      }
      if (ts_muxer_) {
        if (packet->GetMediaType() == cmn::MediaType::Video) {
          std::vector<uint8_t> extradata;
          if( H264AvccToAnnexB::GetExtradata(packet->GetPacketType(), packet->GetData(), extradata) == true)
          {
            video_extra_data_ = extradata;
            return false;
          }

          if(H264AvccToAnnexB::Convert(packet->GetPacketType(), packet->GetData(),
                                        video_extra_data_) == false)
          {
            logte("Failed to change bitstream format ");

            return false;
          }
          AP4_Sample ap4Sample;
          ap4Sample.SetDts(packet->GetDts());
          ap4Sample.SetCts(packet->GetPts());
          ap4Sample.SetSync(false);
          AP4_DataBuffer sample_data(packet->GetData()->GetData(), packet->GetDataLength());
          video_stream_->WriteSample(ap4Sample, sample_data, nullptr, true, *fileByteStream_);
          auto d = std::to_string(packet->GetDts());
          //output_->Write(packet->GetData()->GetData(), packet->GetDataLength());
        }
        if (packet->GetMediaType() == cmn::MediaType::Audio) {
          std::vector<uint8_t> extradata;
          if( AACLatmToAdts::GetExtradata(packet->GetPacketType(), packet->GetData(), extradata) == true)
          {
            audio_extra_data_ = extradata;
            return false;
          }

          if(AACLatmToAdts::Convert(packet->GetPacketType(), packet->GetData(), audio_extra_data_) == false)
          {
            logte("Failed to change bitstream format");
            return false;
          }

          AP4_Sample ap4Sample;
          ap4Sample.SetDts(packet->GetDts());
          ap4Sample.SetCts(packet->GetPts());
          ap4Sample.SetSync(true);
          AP4_DataBuffer sample_data(packet->GetData()->GetData(), packet->GetDataLength());
          audio_stream_->WriteSample(ap4Sample, sample_data, nullptr, true, *fileByteStream_);
        }
      }
        return true;
    }
    std::shared_ptr<uv::TcpConnection> conn_ = nullptr;

    std::map<int32_t, std::shared_ptr<MediaTrack>> _tracks;
    bool AddTrack(std::shared_ptr<MediaTrack> track)
    {
      _tracks.insert(std::make_pair(track->GetId(), track));
      return true;
    }

    const std::shared_ptr<MediaTrack> GetTrack(int32_t id) const
    {
      auto item = _tracks.find(id);
      if (item == _tracks.end())
      {
        return nullptr;
      }

      return item->second;
    }

    sny::SnyMpeg2TsWriter* createTsMuxer() {
      static unsigned int pmt_pid           = sny::AP4_MPEG2_TS_DEFAULT_PID_PMT;
      static unsigned int video_pid         = sny::AP4_MPEG2_TS_DEFAULT_PID_VIDEO;
      static unsigned int audio_pid         = sny::AP4_MPEG2_TS_DEFAULT_PID_AUDIO;
      static unsigned int audio_stream_type = sny::AP4_MPEG2_STREAM_TYPE_ISO_IEC_13818_7;
      static unsigned int audio_stream_id   = sny::AP4_MPEG2_TS_DEFAULT_STREAM_ID_AUDIO;
      static unsigned int video_stream_type = sny::AP4_MPEG2_STREAM_TYPE_AVC;
      static unsigned int video_stream_id   = sny::AP4_MPEG2_TS_DEFAULT_STREAM_ID_VIDEO;

      auto mpeg2Writer         = new sny::SnyMpeg2TsWriter(pmt_pid);

      AP4_Result result;
      std::string filename = "/Users/developer/Desktop/tmp/rtmp.ts";
      result = AP4_FileByteStream::Create(filename.c_str(), AP4_FileByteStream::STREAM_MODE_WRITE,
                                          fileByteStream_);
      if (AP4_FAILED(result)) {
        fprintf(stderr, "ERROR: cannot open output (%d)\n", result);
        return nullptr;
      }

      result = mpeg2Writer->SetAudioStream(sny::kTimescaleMillisecond, audio_stream_type, audio_stream_id,
                                           audio_stream_, audio_pid, nullptr, 0);
      if (AP4_FAILED(result)) {
        return nullptr;
      }
      result = mpeg2Writer->SetVideoStream(sny::kTimescaleMillisecond, video_stream_type, video_stream_id,
                                           video_stream_, video_pid, nullptr, 0);
      if (AP4_FAILED(result)) {
        return nullptr;
      }
      mpeg2Writer->WritePAT(*fileByteStream_);
      mpeg2Writer->WritePMT(*fileByteStream_);
      return mpeg2Writer;
    }

    sny::SnyMpeg2TsWriter* ts_muxer_ = nullptr;
    AP4_ByteStream* fileByteStream_ = nullptr;
    sny::SnyMpeg2TsWriter::SampleStream* audio_stream_ = nullptr;
    sny::SnyMpeg2TsWriter::SampleStream* video_stream_ = nullptr;

		// For sending data
		//std::shared_ptr<ov::Socket> _remote = nullptr;

		// Received data buffer
		std::shared_ptr<ov::Data> 	_remained_data = nullptr;

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
}