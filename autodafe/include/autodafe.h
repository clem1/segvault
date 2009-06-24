/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * NAME       : autodafe.h
 * DESCRIPTION: all the common values
 *---------------------------------------------------------------------------*/

/* author */
#define NAME "Martin Vuagnoux"
#define EMAIL "autodafe@vuagnoux.com"

/* version */
#define MAJOR_VERSION 0
#define MINOR_VERSION 1

/* default fuzz files */
#define DEFAULT_FUZZ_STRING_FILE "/usr/local/etc/autodafe/string.fuzz"
#define DEFAULT_FUZZ_HEX_FILE    "/usr/local/etc/autodafe/hex.fuzz"

/* the magic sequence */
#define MAGIC_SEQUENCE "AD-V.1.0"

/* colors */
#define RED     "\x1b\x5b\x33\x31\x6d"
#define MAGENTA "\x1b\x5b\x33\x35\x6d"
#define CYAN    "\x1b\x5b\x33\x36\x6d"
#define WHITE   "\x1b\x5b\x33\x37\x6d"



/* opcodes */
#define HF_HEX              0x00
#define HF_STRING           0x01
#define HF_STRING_U         0x02

#define HF_BLOCK_BEGIN      0x10
#define HF_BLOCK_END        0x11
#define HF_BLOCK_SIZE_L_32  0x12
#define HF_BLOCK_SIZE_L_16  0x13
#define HF_BLOCK_SIZE_B_32  0x14
#define HF_BLOCK_SIZE_B_16  0x15
#define HF_BLOCK_SIZE_S_16  0x16
#define HF_BLOCK_SIZE_S_10  0x17
#define HF_BLOCK_SIZE_8     0x18

#define HF_BLOCK_CRC32_B    0x20
#define HF_BLOCK_CRC32_L    0x21

#define HF_FUZZ_HEX         0x30
#define HF_FUZZ_STRING      0x31
#define HF_FUZZ_STRING_U    0x32


#define HF_SEND             0xf0
#define HF_RECV             0xf1
#define HF_WRITE            0xf2

/* names of the functions - think to change in lexel.l too */
#define AD_F_STRING      "string"
#define AD_F_BLOCK_BEGIN "block_begin"
#define AD_F_STRING_U    "string_unicode"
#define AD_F_BLOCK_END   "block_end"
#define AD_F_HEX         "hex"
#define AD_F_SEND        "send"
#define AD_F_RECV        "recv"
#define AD_F_WRITE       "write"
#define AD_F_SIZE_8      "block_size_8"
#define AD_F_SIZE_B16    "block_size_b16"
#define AD_F_SIZE_B32    "block_size_b32"
#define AD_F_SIZE_L16    "block_size_l16"
#define AD_F_SIZE_L32    "block_size_l32"
#define AD_F_SIZE_S16    "block_size_hex_string"
#define AD_F_SIZE_S10    "block_size_dec_string"

#define AD_F_CRC32_B     "block_crc32_b"
#define AD_F_CRC32_L     "block_crc32_l"

#define AD_F_FUZZ_STRING      "fuzz_string"
#define AD_F_FUZZ_STRING_U    "fuzz_string_unicode"
#define AD_F_FUZZ_HEX         "fuzz_hex"

