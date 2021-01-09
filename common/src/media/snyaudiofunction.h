/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#ifndef STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYAUDIOFUNCTION_H
#define STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYAUDIOFUNCTION_H
#include "core/snytype.h"
namespace sny{
SnyUI32 getSamplingFrequencyIndex(unsigned int sampling_frequency);
void makeAdtsHeader(unsigned char bits[7], unsigned int  frame_size,
                    unsigned int  sampling_frequency_index, unsigned int  channel_configuration);
}
#endif  // STREAMINGNOLOGY_COMMON_SRC_MEDIA_SNYAUDIOFUNCTION_H
