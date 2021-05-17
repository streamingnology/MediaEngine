#include "aac_utils.h"
#include "core/bitstream.h"

namespace sny
{

namespace AACUtils
{

// clang-format off
int aac_get_sample_rate_by_index(int index)
{
    switch (index)
    {
    case 0:  return 96000;
    case 1:  return 88200;
    case 2:  return 64000;
    case 3:  return 48000;
    case 4:  return 44100;
    case 5:  return 32000;
    case 6:  return 24000;
    case 7:  return 22050;
    case 8:  return 16000;
    case 9:  return 12000;
    case 10: return 11025;
    case 11: return 8000;
    case 12: return 7350;
    }
    return 0;
}

int aac_get_index_by_sample_rate(int sample_rate)
{
    if (92017 <= sample_rate) return 0;
    if (75132 <= sample_rate) return 1;
    if (55426 <= sample_rate) return 2;
    if (46009 <= sample_rate) return 3;
    if (37566 <= sample_rate) return 4;
    if (27713 <= sample_rate) return 5;
    if (23004 <= sample_rate) return 6;
    if (18783 <= sample_rate) return 7;
    if (13856 <= sample_rate) return 8;
    if (11502 <= sample_rate) return 9;
    if (9391 <= sample_rate)  return 10;

    return 11;
}

// clang-format on

bool aac_parse_extra_data(const char *data, int size, int &profile, int &sample_rate, int &channels)
{
    if (size < 2 || !data)
        return false;

    GF_BitStream *bs = gf_bs_new(const_cast<char *>(data), 2, GF_BITSTREAM_READ);

    profile = gf_bs_read_int(bs, 5);

    sample_rate = aac_get_sample_rate_by_index(gf_bs_read_int(bs, 4));

    channels = gf_bs_read_int(bs, 4);

    gf_bs_del(bs);

    return true;
}

SnyDataBuffer aac_make_adts_header(int mpeg_version, int profile, int sample_rate, int channels, int aac_data_length)
{
    SnyDataBuffer data;
    GF_BitStream *bs = gf_bs_new(nullptr, 0, GF_BITSTREAM_WRITE);

    gf_bs_write_int(bs, 0xFFFF, 12);
    gf_bs_write_int(bs, mpeg_version, 1);
    gf_bs_write_int(bs, 0, 2);
    gf_bs_write_int(bs, 1, 1);
    gf_bs_write_int(bs, profile - 1, 2);
    gf_bs_write_int(bs, aac_get_index_by_sample_rate(sample_rate), 4);
    gf_bs_write_int(bs, 0, 1);
    assert(channels >= 1 && channels <= 8); // channels more than 8 is not implemented yet
    if (channels == 8)
        channels = 7;
    gf_bs_write_int(bs, channels, 3);
    gf_bs_write_int(bs, 0, 1);
    gf_bs_write_int(bs, 0, 1);
    gf_bs_write_int(bs, 0, 1);
    gf_bs_write_int(bs, 0, 1);
    gf_bs_write_int(bs, aac_data_length, 13);
    gf_bs_write_int(bs, 0x7FF, 11);
    gf_bs_write_int(bs, 0, 2);

    gf_bs_align(bs);

    char *adts_data = nullptr;
    u32 adts_size = 0;
    gf_bs_get_content(bs, &adts_data, &adts_size);

    if (adts_data != nullptr)
    {
        data = SnyDataBuffer(adts_data, adts_size);
        free(adts_data);
    }

    gf_bs_del(bs);

    return data;
}

} // namespace AACUtils

} // namespace sny