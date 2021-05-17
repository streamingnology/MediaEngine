/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once
#include "core/snydatabuffer.h"
#include <cassert>
namespace sny
{

namespace AACUtils
{

/**
 * @brief get aac sample rate by index
 * @param index index
 * @return sample rate
 */
int aac_get_sample_rate_by_index(int index);

/**
 * @brief get aac index by sample rate
 * @param sample_rate sample rate
 * @return index
 */
int aac_get_index_by_sample_rate(int sample_rate);

/**
 * @brief parse aac extra data from ffmpeg
 * @param data extra data from ffmpeg
 * @param size extra data size
 * @param profile aac profile
 * @param sample_rate aac sample rate
 * @param channels aac channels
 * @return true if success, otherwise false
 */
bool aac_parse_extra_data(const char *data, int size, int &profile, int &sample_rate, int &channels);

SnyDataBuffer aac_make_adts_header(int mpeg_version, int profile, int sample_rate, int channels, int aac_data_length);

} // namespace AACUtils

} // namespace sny
