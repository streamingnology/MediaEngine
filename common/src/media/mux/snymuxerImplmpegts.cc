/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snymuxerImplmpegts.h"
#include "core/snyeasylogging.h"
#include "core/snyutils.h"
#include "media/bitstream/aac/snyaaclatmtoadts.h"
#include "media/bitstream/h264/snyh264avcctoannexb.h"
#include "media/snyffmpeg.h"
namespace sny {
SnyMuxerImplTS::SnyMuxerImplTS() {
  path_ = "";
  fmt_ = "";
  output_ = nullptr;
  pat_stream_ = nullptr;
  pmt_stream_ = nullptr;
  video_stream_pid_ = AP4_MPEG2_TS_DEFAULT_PID_VIDEO;
  audio_stream_pid_ = AP4_MPEG2_TS_DEFAULT_PID_AUDIO;
  pat_stream_ = std::make_shared<Stream>(0);
  pmt_stream_ = std::make_shared<Stream>(AP4_MPEG2_TS_DEFAULT_PID_PMT);
}

SnyMuxerImplTS::~SnyMuxerImplTS() { close(); }

void SnyMuxerImplTS::setPath(std::string path, std::string container_fmt) {
  this->path_ = path;
  this->fmt_ = container_fmt;
}

std::string SnyMuxerImplTS::getPath() { return this->path_; }

bool SnyMuxerImplTS::addMediaTrack(const std::shared_ptr<MediaTrack> media_track) {
  media_tracks_.insert(std::make_pair(media_track->GetId(), media_track));
  return true;
}

const std::map<SnyInt, std::shared_ptr<MediaTrack>>& SnyMuxerImplTS::getMediaTrack() {
  // TODO: insert return statement here
  return media_tracks_;
}

bool SnyMuxerImplTS::open() {
  if (media_tracks_.empty()) {
    LOG(ERROR) << "SnyMuxerImplTS::open, media tracks is empty";
    return false;
  }
  AP4_Result result;
  for (auto& item : media_tracks_) {
    auto media_track = item.second;
    auto track_id = media_track->GetId();
    if (media_track->GetMediaType() == cmn::MediaType::Audio) {
      if (media_track->GetCodecId() == cmn::MediaCodecId::Aac) {
        SampleStream* stream;
        result = AP4_Mpeg2TsAudioSampleStream::Create(audio_stream_pid_++, media_track->GetTimeBase().GetTimescale(),
                                                      AP4_MPEG2_STREAM_TYPE_ISO_IEC_13818_7,
                                                      AP4_MPEG2_TS_DEFAULT_STREAM_ID_AUDIO, stream, nullptr, 0);
        if (AP4_SUCCEEDED(result)) {
          media_streams_.insert(std::make_pair(track_id, stream));
        } else {
          LOG(WARNING) << "SnyMuxerImplTS::open, create audio stream failed";
        }
      } else {
        LOG(WARNING) << SnyUtils::formatstring("SnyMuxerImplTS::open, not support audio codec: %d",
                                               media_track->GetCodecId());
      }
    } else if (media_track->GetMediaType() == cmn::MediaType::Video) {
      if (media_track->GetCodecId() == cmn::MediaCodecId::H264) {
        SampleStream* stream;
        result = AP4_Mpeg2TsVideoSampleStream::Create(video_stream_pid_++, media_track->GetTimeBase().GetTimescale(),
                                                      AP4_MPEG2_STREAM_TYPE_AVC, AP4_MPEG2_TS_DEFAULT_STREAM_ID_VIDEO,
                                                      stream, nullptr, 0);
        if (AP4_SUCCEEDED(result)) {
          media_streams_.insert(std::make_pair(track_id, stream));
        } else {
          LOG(WARNING) << "SnyMuxerImplTS::open, create video stream failed";
        }
      } else {
        LOG(WARNING) << SnyUtils::formatstring("SnyMuxerImplTS::open, not support video codec: %d",
                                               media_track->GetCodecId());
      }
    } else {
      LOG(WARNING) << SnyUtils::formatstring("SnyMuxerImplTS::open, not support media type: %d",
                                             media_track->GetMediaType());
    }
  }

  if (media_streams_.empty()) {
    LOG(ERROR) << "SnyMuxerImplTS::open, there is no valid stream found";
    return false;
  }

  result = CreateByteStream();
  if (AP4_FAILED(result)) {
    LOG(ERROR) << SnyUtils::formatstring("SnyMuxerImplTS::open, create %s in %s failed", path_.c_str(),
                                         getMuxOutType() == kMuxOutHardDrive ? "HardDrive" : "Memory");
    return false;
  }

  WritePAT();
  WritePMT();
  return true;
}

void SnyMuxerImplTS::close() {
  if (output_) {
    output_->Release();
    output_ = nullptr;
  }
}

bool SnyMuxerImplTS::writeSample(std::shared_ptr<SnyMediaSample> sample) {
  auto track_id = sample->getTrackID();
  if (media_tracks_.find(track_id) == media_tracks_.end()) {
    LOG(ERROR) << SnyUtils::formatstring("SnyMuxerImplTS::writeSample, no track id %d found in this file.", track_id);
    return true;
  }

  if (sample->getBitStreamFormat() == kBitStreamH264AVCC) {
    std::shared_ptr<ov::Data> annexb_data;
    if (!SnyH264AvccToAnnexB::Convert(cmn::PacketType::NALU, sample->getDataBuffer(),
                                      media_tracks_[track_id]->GetCodecExtradata(), annexb_data)) {
      LOG(ERROR) << "SnyMuxerImplTS::writeSample, convert from avcc to annexb failed";
      return false;
    }

    sample->setBitStreamFormat(kBitStreamH264ANNEXB);
    sample->setDataBuffer(annexb_data);
  }

  if (sample->getBitStreamFormat() == kBitStreamAACLATM) {
    std::shared_ptr<ov::Data> adts_data;
    if (!SnyAACLatmToAdts::Convert(cmn::PacketType::RAW, sample->getDataBuffer(),
                                   media_tracks_[track_id]->GetCodecExtradata(), adts_data)) {
      LOG(ERROR) << "SnyMuxerImplTS::writeSample, convert from latm to adts failed";
      return false;
    }

    sample->setBitStreamFormat(kBitStreamAACADTS);
    sample->setDataBuffer(adts_data);
  }

  media_streams_[track_id]->WriteSample(sample, nullptr, true, *output_);

  return true;
}

AP4_Result SnyMuxerImplTS::WritePAT() {
  unsigned int payload_size = AP4_MPEG2TS_PACKET_PAYLOAD_SIZE;
  pat_stream_->WritePacketHeader(true, payload_size, false, 0, *output_);

  AP4_BitWriter writer(1024);

  writer.Write(0, 8);                       // pointer
  writer.Write(0, 8);                       // table_id
  writer.Write(1, 1);                       // section_syntax_indicator
  writer.Write(0, 1);                       // '0'
  writer.Write(3, 2);                       // reserved
  writer.Write(13, 12);                     // section_length
  writer.Write(1, 16);                      // transport_stream_id
  writer.Write(3, 2);                       // reserved
  writer.Write(0, 5);                       // version_number
  writer.Write(1, 1);                       // current_next_indicator
  writer.Write(0, 8);                       // section_number
  writer.Write(0, 8);                       // last_section_number
  writer.Write(1, 16);                      // program number
  writer.Write(7, 3);                       // reserved
  writer.Write(pmt_stream_->GetPID(), 13);  // program_map_PID
  writer.Write(ComputeCRC(writer.GetData() + 1, 17 - 1 - 4), 32);

  output_->Write(writer.GetData(), 17);
  output_->Write(StuffingBytes, AP4_MPEG2TS_PACKET_PAYLOAD_SIZE - 17);
  return AP4_SUCCESS;
}

AP4_Result SnyMuxerImplTS::WritePMT() {
  // check that we have at least one media stream
  if (media_streams_.empty()) {
    return AP4_ERROR_INVALID_STATE;
  }

  unsigned int payload_size = AP4_MPEG2TS_PACKET_PAYLOAD_SIZE;
  pmt_stream_->WritePacketHeader(true, payload_size, false, 0, *output_);

  AP4_BitWriter writer(1024);

  unsigned int section_length = 13;
  unsigned int pcr_pid = 0;
  for (auto& item : media_streams_) {
    auto& stream = item.second;
    section_length += 5 + stream->m_Descriptor.GetDataSize();
    pcr_pid = stream->GetPID();
  }

  writer.Write(0, 8);                // pointer
  writer.Write(2, 8);                // table_id
  writer.Write(1, 1);                // section_syntax_indicator
  writer.Write(0, 1);                // '0'
  writer.Write(3, 2);                // reserved
  writer.Write(section_length, 12);  // section_length
  writer.Write(1, 16);               // program_number
  writer.Write(3, 2);                // reserved
  writer.Write(0, 5);                // version_number
  writer.Write(1, 1);                // current_next_indicator
  writer.Write(0, 8);                // section_number
  writer.Write(0, 8);                // last_section_number
  writer.Write(7, 3);                // reserved
  writer.Write(pcr_pid, 13);         // PCD_PID
  writer.Write(0xF, 4);              // reserved
  writer.Write(0, 12);               // program_info_length

  for (auto& item : media_streams_) {
    auto& stream = item.second;
    writer.Write(stream->m_StreamType, 8);                 // stream_type
    writer.Write(0x7, 3);                                  // reserved
    writer.Write(stream->GetPID(), 13);                    // elementary_PID
    writer.Write(0xF, 4);                                  // reserved
    writer.Write(stream->m_Descriptor.GetDataSize(), 12);  // ES_info_length
    for (unsigned int i = 0; i < stream->m_Descriptor.GetDataSize(); i++) {
      writer.Write(stream->m_Descriptor.GetData()[i], 8);
    }
  }

  writer.Write(ComputeCRC(writer.GetData() + 1, section_length - 1), 32);  // CRC

  output_->Write(writer.GetData(), section_length + 4);
  output_->Write(StuffingBytes, AP4_MPEG2TS_PACKET_PAYLOAD_SIZE - (section_length + 4));

  return AP4_SUCCESS;
}

void SnyMuxerImplTS::WriteMPEG2PacketCCTO16(AP4_ByteStream& output) {
  for (auto& item : media_streams_) {
    item.second->WriteMPEG2PacketCCTO16(output);
  }
  while (pat_stream_->GetCC() != 0x0) {
    WritePAT();
  }
  while (pmt_stream_->GetCC() != 0x0) {
    WritePMT();
  }
}

AP4_Result SnyMuxerImplTS::CreateByteStream() {
  AP4_ByteStream* byte_stream = nullptr;
  if (getMuxOutType() == kMuxOutMemory) {
    byte_stream = new AP4_MemoryByteStream();
  } else {
    AP4_Result result = AP4_FileByteStream::Create(path_.c_str(), AP4_FileByteStream::STREAM_MODE_WRITE, byte_stream);
    if (AP4_FAILED(result)) return result;
  }
  output_ = byte_stream;
  return AP4_SUCCESS;
}

}  // namespace sny
