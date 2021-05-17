/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#pragma once

/*
* refer to https://github.com/gpac/gpac/blob/master/src/utils/bitstream.c
*/
#include <cstdint>
#include <cstdio>

namespace sny
{

// clang-format off

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;
typedef int64_t  s64;
typedef int32_t  s32;
typedef int16_t  s16;
typedef int8_t   s8;
typedef double   Double;
typedef float    Float;
typedef u32      Bool;

enum
{
	GF_BITSTREAM_READ = 0,
	GF_BITSTREAM_WRITE,
};

struct GF_BitStream
{
	FILE *stream;
	char *original;
	u64   size;
	u64   bufferSize;
	u64   position;
	u32   current;
	u32   nbBits;
	u32   bsmode;
	void (*EndOfStream)(void *par);
	void  *par;
};

enum GF_Err
{
	GF_SCRIPT_INFO						    = 3,
	GF_PACKED_FRAMES					    = 2,
	GF_EOS								    = 1,
	GF_OK								    = 0,
	GF_BAD_PARAM							= -1,
	GF_OUT_OF_MEM							= -2,
	GF_IO_ERR								= -3,
	GF_NOT_SUPPORTED						= -4,
	GF_CORRUPTED_DATA						= -5,
	GF_SG_UNKNOWN_NODE						= -6,
	GF_SG_INVALID_PROTO						= -7,
	GF_SCRIPT_ERROR							= -8,
	GF_BUFFER_TOO_SMALL						= -9,
	GF_NON_COMPLIANT_BITSTREAM				= -10,
	GF_CODEC_NOT_FOUND						= -11,
	GF_URL_ERROR							= -12,
	GF_SERVICE_ERROR						= -13,
	GF_REMOTE_SERVICE_ERROR					= -14,
	GF_STREAM_NOT_FOUND						= -15,
	GF_ISOM_INVALID_FILE					= -20,
	GF_ISOM_INCOMPLETE_FILE					= -21,
	GF_ISOM_INVALID_MEDIA					= -22,
	GF_ISOM_INVALID_MODE					= -23,
	GF_ISOM_UNKNOWN_DATA_REF				= -24,
	GF_ODF_INVALID_DESCRIPTOR				= -30,
	GF_ODF_FORBIDDEN_DESCRIPTOR				= -31,
	GF_ODF_INVALID_COMMAND					= -32,
	GF_BIFS_UNKNOWN_VERSION					= -33,
	GF_IP_ADDRESS_NOT_FOUND					= -40,
	GF_IP_CONNECTION_FAILURE				= -41,
	GF_IP_NETWORK_FAILURE					= -42,
	GF_IP_CONNECTION_CLOSED					= -43,
	GF_IP_NETWORK_EMPTY						= -44,
	GF_IP_SOCK_WOULD_BLOCK					= -45,
	GF_IP_UDP_TIMEOUT						= -46,
	GF_AUTHENTICATION_FAILURE				= -50,
	GF_SCRIPT_NOT_READY						= -51,
};

GF_BitStream *gf_bs_new(char *buffer, u64 size, u32 mode);
GF_BitStream *gf_bs_from_file(FILE *f, u32 mode);
void          gf_bs_del(GF_BitStream *bs);
u32           gf_bs_read_int(GF_BitStream *bs, u32 nBits);
u64           gf_bs_read_long_int(GF_BitStream *bs, u32 nBits);
Float         gf_bs_read_float(GF_BitStream *bs);
Double        gf_bs_read_double(GF_BitStream *bs);
u32           gf_bs_read_data(GF_BitStream *bs, char *data, u32 nbBytes);
u32           gf_bs_read_u8(GF_BitStream *bs);
u32           gf_bs_read_u16(GF_BitStream *bs);
u32           gf_bs_read_u24(GF_BitStream *bs);
u32           gf_bs_read_u32(GF_BitStream *bs);
u64           gf_bs_read_u64(GF_BitStream *bs);
u32           gf_bs_read_u32_le(GF_BitStream *bs);
u16           gf_bs_read_u16_le(GF_BitStream *bs);
u32           gf_bs_read_vluimsbf5(GF_BitStream *bs);
u32           gf_bs_get_bit_offset(GF_BitStream *bs);
u32           gf_bs_get_bit_position(GF_BitStream *bs);
void          gf_bs_write_int(GF_BitStream *bs, s32 value, s32 nBits);
void          gf_bs_write_long_int(GF_BitStream *bs, s64 value, s32 nBits);
void          gf_bs_write_float(GF_BitStream *bs, Float value);
void          gf_bs_write_double(GF_BitStream *bs, Double value);
u32           gf_bs_write_data(GF_BitStream *bs, char *data, u32 nbBytes);
void          gf_bs_write_u8(GF_BitStream *bs, u32 value);
void          gf_bs_write_u16(GF_BitStream *bs, u32 value);
void          gf_bs_write_u24(GF_BitStream *bs, u32 value);
void          gf_bs_write_u32(GF_BitStream *bs, u32 value);
void          gf_bs_write_u64(GF_BitStream *bs, u64 value);
void          gf_bs_write_u32_le(GF_BitStream *bs, u32 value);
void          gf_bs_write_u16_le(GF_BitStream *bs, u32 value);
void          gf_bs_set_eos_callback(GF_BitStream *bs, void (*EndOfStream)(void *par), void *par);
u8            gf_bs_align(GF_BitStream *bs);
u64           gf_bs_available(GF_BitStream *bs);
void          gf_bs_get_content(GF_BitStream *bs, char **output, u32 *outSize);
void          gf_bs_skip_bytes(GF_BitStream *bs, u64 nbBytes);
GF_Err        gf_bs_seek(GF_BitStream *bs, u64 offset);
u32           gf_bs_peek_bits(GF_BitStream *bs, u32 numBits, u32 byte_offset);
u8            gf_bs_bits_available(GF_BitStream *bs);
u64           gf_bs_get_position(GF_BitStream *bs);
u64           gf_bs_get_size(GF_BitStream *bs);
u64           gf_bs_get_refreshed_size(GF_BitStream *bs);

// clang-format on

} // namespace sny