/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "core/snytype.h"
#include "media/snymediasample.h"
#include "core/snyresults.h"
#include <Ap4.h>
#include <queue>
namespace sny{
class SampleReader {
 public:
  SampleReader(AP4_Track& track, bool selected);
  virtual ~SampleReader();

  virtual AP4_Result ReadSample(AP4_Sample& sample, AP4_DataBuffer& sample_data) = 0;
  AP4_Result GetSample(SnyMediaSample* &sample);
  AP4_Result ReadSample(SnyMediaSample* &sample);

  virtual AP4_Result Seek(AP4_UI64 timestamp_us) = 0;
  AP4_Result OnSeek(AP4_UI64 timestamp_us, AP4_Ordinal& sample_index);

  bool IsEos() const { return eos_; }
  void SetEos(bool flag) { this->eos_ = flag; }

  SnyInt GetTrackId() const { return track_.GetId(); }

  void Select() { this->selected_ = true; }
  void UnSelect() { this->selected_ = false; }
  bool IsSelected() const { return this->selected_; }

 private:
  AP4_Result AudioConvertToSnyMediaSample(AP4_Sample& sample, AP4_DataBuffer& sample_data);
  AP4_Result VideoConvertToSnyMediaSample(AP4_Sample& sample, AP4_DataBuffer& sample_data);

 protected:
  AP4_Track& track_;

 private:
  std::queue<SnyMediaSample*> samples_;
  AP4_Cardinal sample_description_index_;
  AP4_UI08 nalu_length_size_;
  AP4_DataBuffer prefix_;
  bool eos_;
  bool selected_;
};

class TrackSampleReader : public SampleReader {
 public:
  TrackSampleReader(AP4_Track& track, bool selected);

  AP4_Result ReadSample(AP4_Sample& sample, AP4_DataBuffer& sample_data) override;

  AP4_Result Seek(AP4_UI64 timestamp_us) override;

 private:
  AP4_Ordinal sample_index_;
};

class FragmentedSampleReader : public SampleReader {
 public:
  FragmentedSampleReader(AP4_LinearReader& fragment_reader, AP4_Track& track,
                         AP4_UI32 track_id, bool selected);

  AP4_Result ReadSample(AP4_Sample& sample, AP4_DataBuffer& sample_data) override;

  AP4_Result Seek(AP4_UI64 timestamp_us) override;

 private:
  AP4_LinearReader& fragment_reader_;
};
}
