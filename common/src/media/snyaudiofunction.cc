/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snyaudiofunction.h"
namespace sny{
SnyUI32 getSamplingFrequencyIndex(SnyUI32 sampling_frequency) {
  switch (sampling_frequency) {
    case 96000: return 0;
    case 88200: return 1;
    case 64000: return 2;
    case 48000: return 3;
    case 44100: return 4;
    case 32000: return 5;
    case 24000: return 6;
    case 22050: return 7;
    case 16000: return 8;
    case 12000: return 9;
    case 11025: return 10;
    case 8000:  return 11;
    case 7350:  return 12;
    default:    return 0;
  }
}

void makeAdtsHeader(unsigned char bits[7], unsigned int  frame_size,
               unsigned int  sampling_frequency_index, unsigned int  channel_configuration) {
  bits[0] = 0xFF;
  bits[1] = 0xF1; // 0xF9 (MPEG2)
  bits[2] = (SnyUI08)(0x40 | (sampling_frequency_index << 2) | (channel_configuration >> 2));
  bits[3] = (SnyUI08)(((channel_configuration&0x3)<<6) | ((frame_size+7) >> 11));
  bits[4] = ((frame_size+7) >> 3)&0xFF;
  bits[5] = (((frame_size+7) << 5)&0xFF) | 0x1F;
  bits[6] = 0xFC;

  /*
     0:  syncword 12 always: '111111111111'
     12: ID 1 0: MPEG-4, 1: MPEG-2
     13: layer 2 always: '00'
     15: protection_absent 1
     16: profile 2
     18: sampling_frequency_index 4
     22: private_bit 1
     23: channel_configuration 3
     26: original/copy 1
     27: home 1
     28: emphasis 2 only if ID == 0

     ADTS Variable header: these can change from frame to frame
     28: copyright_identification_bit 1
     29: copyright_identification_start 1
     30: aac_frame_length 13 length of the frame including header (in bytes)
     43: adts_buffer_fullness 11 0x7FF indicates VBR
     54: no_raw_data_blocks_in_frame 2
     ADTS Error check
     crc_check 16 only if protection_absent == 0
     */
}
}