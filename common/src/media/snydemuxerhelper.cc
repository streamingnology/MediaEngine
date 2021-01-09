/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "media/snydemuxerhelper.h"
#include "media/snyaudiofunction.h"
#include "media/snymediatype.h"
#include <Ap4Mp4AudioInfo.h>
namespace sny{
SampleReader::SampleReader(AP4_Track& track, bool selected)
: track_(track), selected_(selected), eos_(false){
  this->sample_description_index_ = -1;
  this->nalu_length_size_ = 0;
}

SampleReader::~SampleReader() {
  while (!samples_.empty()) {
    auto sample = samples_.front();
    samples_.pop();
    delete sample;
  }
}

AP4_Result SampleReader::GetSample(SnyMediaSample* &sample) {
  if (!samples_.empty()) {
    sample = samples_.front();
    return AP4_SUCCESS;
  }
  if (eos_) {
    return AP4_ERROR_EOS;
  }
  AP4_Sample ap4_sample; AP4_DataBuffer ap4_sampledata;
  AP4_Result result = ReadSample(ap4_sample, ap4_sampledata);
  if (AP4_FAILED(result)) {
    if (result == AP4_ERROR_EOS) {
      eos_ = true;
    }
    return result;
  }
  if (ap4_sampledata.GetDataSize() == 0) {
    return AP4_ERROR_INVALID_FORMAT;
  }
  switch (track_.GetType()) {
    case AP4_Track::TYPE_VIDEO:
      result = VideoConvertToSnyMediaSample(ap4_sample, ap4_sampledata);
      break;
    case AP4_Track::TYPE_AUDIO:
      result = AudioConvertToSnyMediaSample(ap4_sample, ap4_sampledata);
      break;
    default:
      break;
  }
  if (AP4_FAILED(result)) {
    return result;
  }
  if (!samples_.empty()) {
    sample = samples_.front();
    return AP4_SUCCESS;
  }
  return AP4_FAILURE;
}

AP4_Result SampleReader::ReadSample(SnyMediaSample* &sample) {
  if (!samples_.empty()) {
    sample = samples_.front();
    samples_.pop();
    return AP4_SUCCESS;
  }
  return AP4_FAILURE;
}

AP4_Result SampleReader::VideoConvertToSnyMediaSample(AP4_Sample& sample,
                                                 AP4_DataBuffer& sample_data) {
  AP4_SampleDescription* sample_description = track_.GetSampleDescription(sample.GetDescriptionIndex());
  if (!sample_description) {
    return AP4_ERROR_INVALID_PARAMETERS;
  }

  if (sample_description->GetType() == AP4_SampleDescription::TYPE_AVC) {
    // check the sample description
    auto* avc_desc = AP4_DYNAMIC_CAST(AP4_AvcSampleDescription, sample_description);
    if (avc_desc == nullptr) return AP4_ERROR_NOT_SUPPORTED;

    if (sample.GetDescriptionIndex() != sample_description_index_) {
      sample_description_index_ = sample.GetDescriptionIndex();
      nalu_length_size_ = avc_desc->GetNaluLengthSize();

      // make the SPS/PPS prefix
      prefix_.SetDataSize(0);
      for (unsigned int i=0; i<avc_desc->GetSequenceParameters().ItemCount(); i++) {
        AP4_DataBuffer& buffer = avc_desc->GetSequenceParameters()[i];
        unsigned int prefix_size = prefix_.GetDataSize();
        prefix_.SetDataSize(prefix_size+4+buffer.GetDataSize());
        unsigned char* p = prefix_.UseData()+prefix_size;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 1;
        AP4_CopyMemory(p, buffer.GetData(), buffer.GetDataSize());
      }
      for (unsigned int i=0; i<avc_desc->GetPictureParameters().ItemCount(); i++) {
        AP4_DataBuffer& buffer = avc_desc->GetPictureParameters()[i];
        unsigned int prefix_size = prefix_.GetDataSize();
        prefix_.SetDataSize(prefix_size+4+buffer.GetDataSize());
        unsigned char* p = prefix_.UseData()+prefix_size;
        *p++ = 0;
        *p++ = 0;
        *p++ = 0;
        *p++ = 1;
        AP4_CopyMemory(p, buffer.GetData(), buffer.GetDataSize());
      }
    }
  } else if (sample_description->GetType() == AP4_SampleDescription::TYPE_HEVC) {
    // check the sample description
    auto* hevc_desc = AP4_DYNAMIC_CAST(AP4_HevcSampleDescription, sample_description);
    if (hevc_desc == nullptr) return AP4_ERROR_NOT_SUPPORTED;

    if ((int)sample.GetDescriptionIndex() != sample_description_index_) {
      sample_description_index_ = (int)sample.GetDescriptionIndex();
      nalu_length_size_ = hevc_desc->GetNaluLengthSize();

      // make the VPS/SPS/PPS prefix
      prefix_.SetDataSize(0);
      for (unsigned int i=0; i<hevc_desc->GetSequences().ItemCount(); i++) {
        const AP4_HvccAtom::Sequence& seq = hevc_desc->GetSequences()[i];
        if (seq.m_NaluType == AP4_HEVC_NALU_TYPE_VPS_NUT) {
          for (unsigned int j=0; j<seq.m_Nalus.ItemCount(); j++) {
            const AP4_DataBuffer& buffer = seq.m_Nalus[j];
            unsigned int prefix_size = prefix_.GetDataSize();
            prefix_.SetDataSize(prefix_size+4+buffer.GetDataSize());
            unsigned char* p = prefix_.UseData()+prefix_size;
            *p++ = 0;
            *p++ = 0;
            *p++ = 0;
            *p++ = 1;
            AP4_CopyMemory(p, buffer.GetData(), buffer.GetDataSize());
          }
        }
      }

      for (unsigned int i=0; i<hevc_desc->GetSequences().ItemCount(); i++) {
        const AP4_HvccAtom::Sequence& seq = hevc_desc->GetSequences()[i];
        if (seq.m_NaluType == AP4_HEVC_NALU_TYPE_SPS_NUT) {
          for (unsigned int j=0; j<seq.m_Nalus.ItemCount(); j++) {
            const AP4_DataBuffer& buffer = seq.m_Nalus[j];
            unsigned int prefix_size = prefix_.GetDataSize();
            prefix_.SetDataSize(prefix_size+4+buffer.GetDataSize());
            unsigned char* p = prefix_.UseData()+prefix_size;
            *p++ = 0;
            *p++ = 0;
            *p++ = 0;
            *p++ = 1;
            AP4_CopyMemory(p, buffer.GetData(), buffer.GetDataSize());
          }
        }
      }

      for (unsigned int i=0; i<hevc_desc->GetSequences().ItemCount(); i++) {
        const AP4_HvccAtom::Sequence& seq = hevc_desc->GetSequences()[i];
        if (seq.m_NaluType == AP4_HEVC_NALU_TYPE_PPS_NUT) {
          for (unsigned int j=0; j<seq.m_Nalus.ItemCount(); j++) {
            const AP4_DataBuffer& buffer = seq.m_Nalus[j];
            unsigned int prefix_size = prefix_.GetDataSize();
            prefix_.SetDataSize(prefix_size+4+buffer.GetDataSize());
            unsigned char* p = prefix_.UseData()+prefix_size;
            *p++ = 0;
            *p++ = 0;
            *p++ = 0;
            *p++ = 1;
            AP4_CopyMemory(p, buffer.GetData(), buffer.GetDataSize());
          }
        }
      }
    }
  } else {
    return AP4_ERROR_NOT_SUPPORTED;
  }

  // decide if we need to emit the prefix
  bool emit_prefix = false;
  if (sample.IsSync()) {
    emit_prefix = true;
  }

  // write the NAL units
  const unsigned char* data      = sample_data.GetData();
  unsigned int         data_size = sample_data.GetDataSize();

  // allocate a buffer for the PES packet
  AP4_DataBuffer pes_data;

  // output all NALUs
  for (unsigned int nalu_count = 0; data_size; nalu_count++) {
    // sanity check
    if (data_size < nalu_length_size_) break;

    // get the next NAL unit
    AP4_UI32 nalu_size;
    if (nalu_length_size_ == 1) {
      nalu_size = *data++;
      data_size--;
    } else if (nalu_length_size_ == 2) {
      nalu_size = AP4_BytesToInt16BE(data);
      data      += 2;
      data_size -= 2;
    } else if (nalu_length_size_ == 4) {
      nalu_size = AP4_BytesToInt32BE(data);
      data      += 4;
      data_size -= 4;
    } else {
      break;
    }
    if (nalu_size > data_size) break;

    // check if we need to add a delimiter before the NALU
    if (nalu_count == 0 && sample_description->GetType() == AP4_SampleDescription::TYPE_AVC) {
      if (data_size < 1) break;
      if (/* nalu_size != 2 || */ (data[0] & 0x1F) != AP4_AVC_NAL_UNIT_TYPE_ACCESS_UNIT_DELIMITER) {
        // the first NAL unit is not an Access Unit Delimiter, we need to add one
        unsigned char delimiter[6];
        delimiter[0] = 0;
        delimiter[1] = 0;
        delimiter[2] = 0;
        delimiter[3] = 1;
        delimiter[4] = 9;    // NAL type = Access Unit Delimiter;
        delimiter[5] = 0xF0; // Slice types = ANY
        pes_data.AppendData(delimiter, 6);

        if (emit_prefix) {
          pes_data.AppendData(prefix_.GetData(), prefix_.GetDataSize());
          emit_prefix = false;
        }
      }
    } else {
      if (emit_prefix) {
        pes_data.AppendData(prefix_.GetData(), prefix_.GetDataSize());
        emit_prefix = false;
      }
    }

    // add a start code before the NAL unit
    unsigned char start_code[3];
    start_code[0] = 0;
    start_code[1] = 0;
    start_code[2] = 1;
    pes_data.AppendData(start_code, 3);

    // add the NALU
    pes_data.AppendData(data, nalu_size);

    // for AVC streams that do start with a NAL unit delimiter, we need to add the prefix now
    if (emit_prefix) {
      pes_data.AppendData(prefix_.GetData(), prefix_.GetDataSize());
      emit_prefix = false;
    }

    // move to the next NAL unit
    data      += nalu_size;
    data_size -= nalu_size;
  }

  // compute the timestamp
  AP4_UI64 dts_us = AP4_ConvertTime(sample.GetDts(), track_.GetMediaTimeScale(), kTimescaleMicrosecond);
  AP4_UI64 pts_us = AP4_ConvertTime(sample.GetCts(), track_.GetMediaTimeScale(), kTimescaleMicrosecond);
  AP4_UI64 duration_us = AP4_ConvertTime(sample.GetDuration(), track_.GetMediaTimeScale(), kTimescaleMicrosecond);
  auto *media_sample = new SnyMediaSample(kMediaTypeVideo, dts_us, pts_us, duration_us);
  media_sample->setData((const char*)pes_data.GetData(), pes_data.GetDataSize());
  media_sample->setKey(sample.IsSync());
  samples_.push(media_sample);
  return AP4_SUCCESS;
}

AP4_Result SampleReader::AudioConvertToSnyMediaSample(
    AP4_Sample& sample, AP4_DataBuffer& sample_data) {
  AP4_SampleDescription* sample_description = track_.GetSampleDescription(sample.GetDescriptionIndex());
  if (!sample_description) {
    return AP4_ERROR_INVALID_PARAMETERS;
  }

  // check the sample description
  if (sample_description->GetFormat() == AP4_SAMPLE_FORMAT_MP4A) {
    auto* audio_desc = AP4_DYNAMIC_CAST(AP4_MpegAudioSampleDescription, sample_description);

    if (audio_desc == nullptr) return AP4_ERROR_NOT_SUPPORTED;
    if (audio_desc->GetMpeg4AudioObjectType() != AP4_MPEG4_AUDIO_OBJECT_TYPE_AAC_LC   &&
        audio_desc->GetMpeg4AudioObjectType() != AP4_MPEG4_AUDIO_OBJECT_TYPE_AAC_MAIN &&
        audio_desc->GetMpeg4AudioObjectType() != AP4_MPEG4_AUDIO_OBJECT_TYPE_SBR      &&
        audio_desc->GetMpeg4AudioObjectType() != AP4_MPEG4_AUDIO_OBJECT_TYPE_PS) {
      return AP4_ERROR_NOT_SUPPORTED;
    }

    unsigned int sample_rate   = audio_desc->GetSampleRate();
    unsigned int channel_count = audio_desc->GetChannelCount();
    const AP4_DataBuffer& dsi  = audio_desc->GetDecoderInfo();
    if (dsi.GetDataSize()) {
      AP4_Mp4AudioDecoderConfig dec_config;
      AP4_Result result = dec_config.Parse(dsi.GetData(), dsi.GetDataSize());
      if (AP4_SUCCEEDED(result)) {
        sample_rate = dec_config.m_SamplingFrequency;
        channel_count = dec_config.m_ChannelCount;
      }
    }
    unsigned int sampling_frequency_index = getSamplingFrequencyIndex(sample_rate);
    unsigned int channel_configuration    = channel_count;

    auto* buffer = new unsigned char[7+sample_data.GetDataSize()];
    makeAdtsHeader(buffer, sample_data.GetDataSize(), sampling_frequency_index, channel_configuration);
    AP4_CopyMemory(buffer+7, sample_data.GetData(), sample_data.GetDataSize());
    AP4_UI64 dts_us = AP4_ConvertTime(sample.GetDts(), track_.GetMediaTimeScale(), kTimescaleMicrosecond);
    AP4_UI64 duration_us = AP4_ConvertTime(sample.GetDuration(), track_.GetMediaTimeScale(), kTimescaleMicrosecond);
    auto* media_sample = new SnyMediaSample(kMediaTypeAudio, dts_us, dts_us, duration_us);
    media_sample->setData((const char*)buffer, 7+sample.GetSize());
    samples_.push(media_sample);
    //WritePES(buffer, 7+sample.GetSize(), ts, false, ts, with_pcr, output);
    delete[] buffer;
  } else if (sample_description->GetFormat() == AP4_SAMPLE_FORMAT_AC_3 ||
             sample_description->GetFormat() == AP4_SAMPLE_FORMAT_EC_3 ||
             sample_description->GetFormat() == AP4_SAMPLE_FORMAT_AC_4) {
    AP4_UI64 dts_us = AP4_ConvertTime(sample.GetDts(), track_.GetMediaTimeScale(), kTimescaleMicrosecond);
    AP4_UI64 duration_us = AP4_ConvertTime(sample.GetDuration(), track_.GetMediaTimeScale(), kTimescaleMicrosecond);
    auto* media_sample = new SnyMediaSample(kMediaTypeAudio, dts_us, dts_us, duration_us);
    media_sample->setData((const char*)sample_data.GetData(), sample_data.GetDataSize());
    samples_.push(media_sample);
    //WritePES(sample_data.GetData(), sample_data.GetDataSize(), ts, false, ts, with_pcr, output);
  } else {
    return AP4_ERROR_NOT_SUPPORTED;
  }

  return AP4_SUCCESS;
}
AP4_Result SampleReader::OnSeek(AP4_UI64 timestamp_us,
                                AP4_Ordinal& sample_index) {
  eos_ = false;
  while (!samples_.empty()) {
    auto sample = samples_.front();
    samples_.pop(); delete sample;
  }
  AP4_UI64 ts_ms = AP4_ConvertTime(timestamp_us, kTimescaleMicrosecond, kTimescaleMillisecond);
  AP4_Result result = track_.GetSampleIndexForTimeStampMs(ts_ms, sample_index);
  if (AP4_FAILED(result)) {
    eos_ = true;
  }
  return result;
}

TrackSampleReader::TrackSampleReader(AP4_Track& track, bool selected)
    :SampleReader(track, selected), sample_index_(0) {}

AP4_Result TrackSampleReader::ReadSample(AP4_Sample& sample, AP4_DataBuffer& sample_data) {
  if (sample_index_ >= track_.GetSampleCount()) {
    return AP4_ERROR_EOS;
  }
  return track_.ReadSample(sample_index_++, sample, sample_data);
}

AP4_Result TrackSampleReader::Seek(AP4_UI64 timestamp_us) {
  AP4_Cardinal sample_index;
  AP4_Result result = OnSeek(timestamp_us, sample_index);
  if (AP4_FAILED(result)) {
    return result;
  }
  sample_index_ = sample_index;
  return result;
}

FragmentedSampleReader::FragmentedSampleReader(AP4_LinearReader& fragment_reader,
                                               AP4_Track& track, AP4_UI32 track_id,
                                               bool selected)
    :SampleReader(track, selected), fragment_reader_(fragment_reader) {
    fragment_reader.EnableTrack(track_id);
}

AP4_Result FragmentedSampleReader::ReadSample(AP4_Sample& sample, AP4_DataBuffer& sample_data) {
  AP4_Result result =
      fragment_reader_.ReadNextSample(track_.GetId(), sample, sample_data);
  return result;
}
AP4_Result FragmentedSampleReader::Seek(AP4_UI64 timestamp_us) {
  auto ts_ms = AP4_ConvertTime(timestamp_us, kTimescaleMicrosecond, kTimescaleMillisecond);
  return fragment_reader_.SeekTo(ts_ms);
}
}