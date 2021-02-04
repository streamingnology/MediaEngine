/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snymuxerimplmpegtshelper.h"

namespace sny {

AP4_UI32 ComputeCRC(const unsigned char* data, unsigned int data_size) {
  AP4_UI32 crc = 0xFFFFFFFF;
  for (unsigned int i = 0; i < data_size; i++) {
    crc = (crc << 8) ^ CRC_Table[((crc >> 24) ^ *data++) & 0xFF];
  }
  return crc;
}
void Stream::WritePacketHeader(bool payload_start, unsigned int& payload_size, bool with_pcr, AP4_UI64 pcr,
                               AP4_ByteStream& output) {
  unsigned char header[4];
  header[0] = AP4_MPEG2TS_SYNC_BYTE;
  header[1] = (AP4_UI08)(((payload_start ? 1 : 0) << 6) | (m_PID >> 8));
  header[2] = m_PID & 0xFF;

  unsigned int adaptation_field_size = 0;
  if (with_pcr) adaptation_field_size += 2 + AP4_MPEG2TS_PCR_ADAPTATION_SIZE;

  // clamp the payload size
  if (payload_size + adaptation_field_size > AP4_MPEG2TS_PACKET_PAYLOAD_SIZE) {
    payload_size = AP4_MPEG2TS_PACKET_PAYLOAD_SIZE - adaptation_field_size;
  }

  // adjust the adaptation field to include stuffing if necessary
  if (adaptation_field_size + payload_size < AP4_MPEG2TS_PACKET_PAYLOAD_SIZE) {
    adaptation_field_size = AP4_MPEG2TS_PACKET_PAYLOAD_SIZE - payload_size;
  }

  if (adaptation_field_size == 0) {
    // no adaptation field
    header[3] = (1 << 4) | ((m_ContinuityCounter++) & 0x0F);
    output.Write(header, 4);
  } else {
    // adaptation field present
    header[3] = (3 << 4) | ((m_ContinuityCounter++) & 0x0F);
    output.Write(header, 4);

    if (adaptation_field_size == 1) {
      // just one byte (stuffing)
      output.WriteUI08(0);
    } else {
      // two or more bytes (stuffing and/or PCR)
      output.WriteUI08((AP4_UI08)(adaptation_field_size - 1));
      output.WriteUI08(with_pcr ? (1 << 4) : 0);
      unsigned int pcr_size = 0;
      if (with_pcr) {
        pcr_size = AP4_MPEG2TS_PCR_ADAPTATION_SIZE;
        AP4_UI64 pcr_base = pcr / 300;
        AP4_UI32 pcr_ext = (AP4_UI32)(pcr % 300);
        AP4_BitWriter writer(pcr_size);
        writer.Write((AP4_UI32)(pcr_base >> 32), 1);
        writer.Write((AP4_UI32)pcr_base, 32);
        writer.Write(0x3F, 6);
        writer.Write(pcr_ext, 9);
        output.Write(writer.GetData(), pcr_size);
      }
      if (adaptation_field_size > 2) {
        output.Write(StuffingBytes, adaptation_field_size - pcr_size - 2);
      }
    }
  }
}

void Stream::WriteMPEG2PacketCCTO16(AP4_ByteStream& output) {
  while (GetCC() != 0x0) {
    unsigned int payload_size = 0;
    WritePacketHeader(false, payload_size, true, 0, output);
  }
}

AP4_Result SampleStream::WritePES(const unsigned char* data, unsigned int data_size, AP4_UI64 dts, bool with_dts,
                                  AP4_UI64 pts, bool with_pcr, AP4_ByteStream& output) {
  unsigned int pes_header_size = 14 + (with_dts ? 5 : 0);
  AP4_BitWriter pes_header(pes_header_size);

  // adjust the base timestamp so we don't start at 0
  // dts += 10000;
  // pts += 10000;

  pes_header.Write(0x000001, 24);   // packet_start_code_prefix
  pes_header.Write(m_StreamId, 8);  // stream_id
  pes_header.Write(m_StreamId == AP4_MPEG2_TS_DEFAULT_STREAM_ID_VIDEO ? 0 : (data_size + pes_header_size - 6),
                   16);                      // PES_packet_length
  pes_header.Write(2, 2);                    // '01'
  pes_header.Write(0, 2);                    // PES_scrambling_control
  pes_header.Write(0, 1);                    // PES_priority
  pes_header.Write(1, 1);                    // data_alignment_indicator
  pes_header.Write(0, 1);                    // copyright
  pes_header.Write(0, 1);                    // original_or_copy
  pes_header.Write(with_dts ? 3 : 2, 2);     // PTS_DTS_flags
  pes_header.Write(0, 1);                    // ESCR_flag
  pes_header.Write(0, 1);                    // ES_rate_flag
  pes_header.Write(0, 1);                    // DSM_trick_mode_flag
  pes_header.Write(0, 1);                    // additional_copy_info_flag
  pes_header.Write(0, 1);                    // PES_CRC_flag
  pes_header.Write(0, 1);                    // PES_extension_flag
  pes_header.Write(pes_header_size - 9, 8);  // PES_header_data_length

  pes_header.Write(with_dts ? 3 : 2, 4);        // '0010' or '0011'
  pes_header.Write((AP4_UI32)(pts >> 30), 3);   // PTS[32..30]
  pes_header.Write(1, 1);                       // marker_bit
  pes_header.Write((AP4_UI32)(pts >> 15), 15);  // PTS[29..15]
  pes_header.Write(1, 1);                       // marker_bit
  pes_header.Write((AP4_UI32)pts, 15);          // PTS[14..0]
  pes_header.Write(1, 1);                       // market_bit

  if (with_dts) {
    pes_header.Write(1, 4);                       // '0001'
    pes_header.Write((AP4_UI32)(dts >> 30), 3);   // DTS[32..30]
    pes_header.Write(1, 1);                       // marker_bit
    pes_header.Write((AP4_UI32)(dts >> 15), 15);  // DTS[29..15]
    pes_header.Write(1, 1);                       // marker_bit
    pes_header.Write((AP4_UI32)dts, 15);          // DTS[14..0]
    pes_header.Write(1, 1);                       // market_bit
  }

  bool first_packet = true;
  data_size += pes_header_size;  // add size of PES header
  while (data_size) {
    unsigned int payload_size = data_size;
    if (payload_size > AP4_MPEG2TS_PACKET_PAYLOAD_SIZE) payload_size = AP4_MPEG2TS_PACKET_PAYLOAD_SIZE;

    if (first_packet) {
      WritePacketHeader(first_packet, payload_size, with_pcr, (with_dts ? dts : pts) * 300, output);
      first_packet = false;
      output.Write(pes_header.GetData(), pes_header_size);
      output.Write(data, payload_size - pes_header_size);
      data += payload_size - pes_header_size;
    } else {
      WritePacketHeader(first_packet, payload_size, false, 0, output);
      output.Write(data, payload_size);
      data += payload_size;
    }
    data_size -= payload_size;
  }

  return AP4_SUCCESS;
}

AP4_Result AP4_Mpeg2TsAudioSampleStream::Create(AP4_UI16 pid, AP4_UI32 timescale, AP4_UI08 stream_type,
                                                AP4_UI16 stream_id, SampleStream*& stream, const AP4_UI08* descriptor,
                                                AP4_Size descriptor_length) {
  stream = new AP4_Mpeg2TsAudioSampleStream(pid, timescale, stream_type, stream_id, descriptor, descriptor_length);
  return AP4_SUCCESS;
}

AP4_Result AP4_Mpeg2TsAudioSampleStream::WriteSample(std::shared_ptr<SnyMediaSample> sample,
                                                     AP4_SampleDescription* sample_description, bool with_pcr,
                                                     AP4_ByteStream& output) {
  AP4_UI64 ts = AP4_ConvertTime(sample->dts(), m_TimeScale, 90000);
  WritePES((const unsigned char*)sample->data(), sample->size(), ts, false, ts, with_pcr, output);
  return AP4_SUCCESS;
}

AP4_Result AP4_Mpeg2TsVideoSampleStream::Create(AP4_UI16 pid, AP4_UI32 timescale, AP4_UI08 stream_type,
                                                AP4_UI16 stream_id, SampleStream*& stream, const AP4_UI08* descriptor,
                                                AP4_Size descriptor_length) {
  // create the stream object
  stream = new AP4_Mpeg2TsVideoSampleStream(pid, timescale, stream_type, stream_id, descriptor, descriptor_length);
  return AP4_SUCCESS;
}

AP4_Result AP4_Mpeg2TsVideoSampleStream::WriteSample(std::shared_ptr<SnyMediaSample> sample,
                                                     AP4_SampleDescription* sample_description, bool with_pcr,
                                                     AP4_ByteStream& output) {
  // compute the timestamp
  AP4_UI64 dts = AP4_ConvertTime(sample->dts(), m_TimeScale, 90000);
  AP4_UI64 pts = AP4_ConvertTime(sample->pts(), m_TimeScale, 90000);

  // update counters
  ++m_SamplesWritten;

  unsigned char delimiter[6];
  delimiter[0] = 0;
  delimiter[1] = 0;
  delimiter[2] = 0;
  delimiter[3] = 1;
  delimiter[4] = 9;     // NAL type = Access Unit Delimiter;
  delimiter[5] = 0xF0;  // Slice types = ANY
  AP4_DataBuffer buffer;
  buffer.AppendData(delimiter, sizeof(delimiter));
  buffer.AppendData((const AP4_Byte*)sample->data(), sample->size());
  // write the packet
  return WritePES(buffer.GetData(), buffer.GetDataSize(), dts, true, pts, with_pcr, output);
}
}  // namespace sny