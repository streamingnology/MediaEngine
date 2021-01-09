/*
 *copyleft (c) 2019 www.streamingnology.com
 *code released under GPL license
 *This file is from Bento4
 */
#ifndef STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMPEG2TS_H
#define STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMPEG2TS_H
#include "Ap4DataBuffer.h"
#include "Ap4Types.h"
class AP4_ByteStream;
class AP4_Sample;
class AP4_SampleDescription;
namespace sny {
const AP4_UI16 AP4_MPEG2_TS_DEFAULT_PID_PMT = 0x100;
const AP4_UI16 AP4_MPEG2_TS_DEFAULT_PID_AUDIO = 0x101;
const AP4_UI16 AP4_MPEG2_TS_DEFAULT_PID_VIDEO = 0x102;
const AP4_UI16 AP4_MPEG2_TS_DEFAULT_STREAM_ID_AUDIO = 0xc0;
const AP4_UI16 AP4_MPEG2_TS_DEFAULT_STREAM_ID_VIDEO = 0xe0;
const AP4_UI16 AP4_MPEG2_TS_STREAM_ID_PRIVATE_STREAM_1 = 0xbd;

const AP4_UI08 AP4_MPEG2_STREAM_TYPE_ISO_IEC_13818_1_PES = 0x06;
const AP4_UI08 AP4_MPEG2_STREAM_TYPE_ISO_IEC_13818_7 = 0x0F;
const AP4_UI08 AP4_MPEG2_STREAM_TYPE_AVC = 0x1B;
const AP4_UI08 AP4_MPEG2_STREAM_TYPE_HEVC = 0x24;
const AP4_UI08 AP4_MPEG2_STREAM_TYPE_ATSC_AC3 = 0x81;
const AP4_UI08 AP4_MPEG2_STREAM_TYPE_ATSC_EAC3 = 0x81;

/**
 * This class is a simple implementation of a converter that can
 * convert MP4 audio and video access units into an MPEG2 transport
 * stream.
 * It currently only supports one audio tracks with MPEG4 AAC LC, and one
 * video track with MPEG4 AVC.
 */
class SnyMpeg2TsWriter {
 public:
  class Stream {
   public:
    Stream(AP4_UI16 pid) : m_PID(pid), m_ContinuityCounter(0) {}
    virtual ~Stream() {}

    AP4_UI16 GetCC() { return (m_ContinuityCounter & 0x0F); }
    AP4_UI16 GetPID() { return m_PID; }
    void WritePacketHeader(bool payload_start, unsigned int& payload_size,
                           bool with_pcr, AP4_UI64 pcr, AP4_ByteStream& output);
    void WriteMPEG2PacketCCTO16(AP4_ByteStream& output);

   private:
    AP4_UI16 m_PID;
    unsigned int m_ContinuityCounter;
  };

  class SampleStream : public Stream {
   public:
    SampleStream(AP4_UI16 pid, AP4_UI08 stream_type, AP4_UI16 stream_id,
                 AP4_UI32 timescale, const AP4_UI08* descriptor,
                 AP4_Size descriptor_length)
        : Stream(pid),
          m_StreamType(stream_type),
          m_StreamId(stream_id),
          m_TimeScale(timescale) {
      if (descriptor && descriptor_length) {
        m_Descriptor.SetData(descriptor, descriptor_length);
      }
    }

    virtual AP4_Result WritePES(const unsigned char* data,
                                unsigned int data_size, AP4_UI64 dts,
                                bool with_dts, AP4_UI64 pts, bool with_pcr,
                                AP4_ByteStream& output);

    virtual AP4_Result WriteSample(AP4_Sample& sample,
                                   AP4_DataBuffer& sample_data,
                                   AP4_SampleDescription* sample_description,
                                   bool with_pcr, AP4_ByteStream& output) = 0;

    AP4_Result WriteSample(AP4_Sample& sample,
                           AP4_SampleDescription* sample_description,
                           bool with_pcr, AP4_ByteStream& output);

    void SetType(AP4_UI08 type) { m_StreamType = type; }
    void SetDescriptor(const AP4_UI08* descriptor, AP4_Size descriptor_length) {
      if (descriptor && descriptor_length) {
        m_Descriptor.SetData(descriptor, descriptor_length);
      }
    }

    AP4_UI08 m_StreamType;
    AP4_UI16 m_StreamId;
    AP4_UI32 m_TimeScale;
    AP4_DataBuffer m_Descriptor;
  };

  SnyMpeg2TsWriter(AP4_UI16 pmt_pid = AP4_MPEG2_TS_DEFAULT_PID_PMT);
  ~SnyMpeg2TsWriter();

  Stream* GetPAT() { return ptr_pat_stream_; }
  Stream* GetPMT() { return ptr_pmt_stream_; }
  AP4_Result WritePAT(AP4_ByteStream& output);
  AP4_Result WritePMT(AP4_ByteStream& output);
  AP4_Result SetAudioStream(AP4_UI32 timescale, AP4_UI08 stream_type,
                            AP4_UI16 stream_id, SampleStream*& stream,
                            AP4_UI16 pid = AP4_MPEG2_TS_DEFAULT_PID_AUDIO,
                            const AP4_UI08* descriptor = nullptr,
                            AP4_Size descriptor_length = 0);
  AP4_Result SetVideoStream(AP4_UI32 timescale, AP4_UI08 stream_type,
                            AP4_UI16 stream_id, SampleStream*& stream,
                            AP4_UI16 pid = AP4_MPEG2_TS_DEFAULT_PID_VIDEO,
                            const AP4_UI08* descriptor = nullptr,
                            AP4_Size descriptor_length = 0);
  void WriteMPEG2PacketCCTO16(AP4_ByteStream& output);

 private:
  Stream* ptr_pat_stream_;
  Stream* ptr_pmt_stream_;
  SampleStream* ptr_audio_sample_stream_;
  SampleStream* ptr_video_sample_stream_;
};
}  // namespace sny

#endif  // !STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYMPEG2TS_H
