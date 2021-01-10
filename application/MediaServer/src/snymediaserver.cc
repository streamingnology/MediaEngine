#include <iostream>
#include "media/snyidemuxer.h"
#include "media/snydemuxer.h"
#include "media/snymediainfo.h"
#include <media/snympeg2ts.h>
void printSample(sny::SnyDemuxer* demuxer) {
  for (int j = 0; j < 100000; ++j) {
    sny::SnyMediaSample* media_sample = demuxer->readSample();
    if (media_sample != nullptr) {
      if (media_sample->type() == sny::kMediaTypeVideo) {
        std::cout << "V " << media_sample->dts() << " " << media_sample->pts()<< " "<< media_sample->duration() << std::endl;
      }
      if (media_sample->type() == sny::kMediaTypeAudio) {
        std::cout << "A " << media_sample->dts() << " " << media_sample->pts()<< " "<< media_sample->duration() << std::endl;
      }
      delete media_sample;
    } else {
      if (demuxer->reachEndOfFile()) {
        std::cout<< "end of file" << std::endl;
        break;
      }
    }
  }
}

void convert2ts(sny::SnyDemuxer* demuxer) {
  static unsigned int pmt_pid           = sny::AP4_MPEG2_TS_DEFAULT_PID_PMT;
  static unsigned int video_pid         = sny::AP4_MPEG2_TS_DEFAULT_PID_VIDEO;
  static unsigned int audio_pid         = sny::AP4_MPEG2_TS_DEFAULT_PID_AUDIO;
  static unsigned int audio_stream_type = sny::AP4_MPEG2_STREAM_TYPE_ISO_IEC_13818_7;
  static unsigned int audio_stream_id   = sny::AP4_MPEG2_TS_DEFAULT_STREAM_ID_AUDIO;
  static unsigned int video_stream_type = sny::AP4_MPEG2_STREAM_TYPE_AVC;
  static unsigned int video_stream_id   = sny::AP4_MPEG2_TS_DEFAULT_STREAM_ID_VIDEO;

  sny::SnyMpeg2TsWriter::SampleStream* audio_stream = NULL;
  sny::SnyMpeg2TsWriter::SampleStream* video_stream = NULL;
  AP4_ByteStream* memoryByteStream;
  auto mpeg2Writer         = new sny::SnyMpeg2TsWriter(pmt_pid);

  AP4_Result result;
  std::string filename = "/Users/developer/Documents/E/out.ts";
  result = AP4_FileByteStream::Create(filename.c_str(), AP4_FileByteStream::STREAM_MODE_WRITE, memoryByteStream);
  if (AP4_FAILED(result)) {
    fprintf(stderr, "ERROR: cannot open output (%d)\n", result);
    return;
  }

  result = mpeg2Writer->SetAudioStream(sny::kTimescaleMicrosecond, audio_stream_type, audio_stream_id, audio_stream, audio_pid, NULL, 0);
  if (AP4_FAILED(result)) {
    return;
  }
  result = mpeg2Writer->SetVideoStream(sny::kTimescaleMicrosecond, video_stream_type, video_stream_id, video_stream, video_pid, NULL, 0);
  if (AP4_FAILED(result)) {
    return;
  }
  mpeg2Writer->WritePAT(*memoryByteStream);
  mpeg2Writer->WritePMT(*memoryByteStream);

  for (int j = 0; j < 1000000; ++j) {
    sny::SnyMediaSample* media_sample = demuxer->readSample();
    if (media_sample != nullptr) {
      AP4_Sample Ap4Sample;
      Ap4Sample.SetDts(media_sample->dts());
      Ap4Sample.SetCts(media_sample->pts());
      Ap4Sample.SetSync(media_sample->isKey());
      AP4_DataBuffer sample_data(media_sample->data(), media_sample->size());

      if (media_sample->type() == sny::kMediaTypeVideo) {
        std::cout << "V " << media_sample->dts() << " " << media_sample->pts()<< " "<< media_sample->duration() << std::endl;
        video_stream->WriteSample(Ap4Sample, sample_data, NULL, true, *memoryByteStream);
      }
      if (media_sample->type() == sny::kMediaTypeAudio) {
        audio_stream->WriteSample(Ap4Sample, sample_data, NULL, true, *memoryByteStream);
        std::cout << "A " << media_sample->dts() << " " << media_sample->pts()<< " "<< media_sample->duration() << std::endl;
      }
      delete media_sample;
    } else {
      if (demuxer->reachEndOfFile()) {
        std::cout<< "end of file" << std::endl;
        break;
      }
    }
  }

  memoryByteStream->Release();
  delete mpeg2Writer;
}

int main() {
  std::cout << "MediaServer" << std::endl;
  sny::SnyDemuxer *demuxer = new sny::SnyDemuxer();
  std::string uri = "/Users/developer/Documents/E/fragment.mp4";
  demuxer->setUri(uri);
  demuxer->open();
  auto media_info = demuxer->getMediaInfo();
  std::cout<< "1" << std::endl;
  //convert2ts(demuxer);
  std::cout<< "seek" << std::endl;
  //demuxer->seek(2000000);
  printSample(demuxer);

  std::cout<< "seek" << std::endl;
  demuxer->seek(200000);
  printSample(demuxer);

  std::cout<< "seek" << std::endl;
  demuxer->seek(302500100);
  printSample(demuxer);
  delete demuxer;
  return 0;
}
