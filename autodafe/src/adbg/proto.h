/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Sep.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : proto.h
 * DESCRIPTION: define the protocol between the fuzzer and the debugger
 *---------------------------------------------------------------------------*/

#define INET_VERSION_A_MSG         0x00 /* (ack) [1byte: 0x00][4bytes: proto major][4bytes: proto minor] */
#define INET_ACK_MSG               0x01 /* (ack) [1byte: 0x01] */
#define INET_START_A_MSG           0x02 /* (ack) [1byte: 0x02] */
#define INET_STRING_A_MSG          0x03 /* (ack) [1byte: 0x03][4bytes: size][string...] */
#define INET_SIGSEGV_MSG           0x04 /* [1byte: 0x04] */
#define INET_WEIGHT_MSG            0x05 /* [1byte: 0x05][4bytes: fuzz_id] */
#define INET_EXIT_MSG              0x06 /* [1byte: 0x06] */
#define INET_NOP_MSG               0x90 /* [1byte: 0x90] */

