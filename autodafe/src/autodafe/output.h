/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Aug.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : output.h
 * DESCRIPTION: ...
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * NAME: _output_send_packet()
 * DESC: print the content of a sent packet using stdout
 *---------------------------------------------------------------------------*/
void _output_send_packet(config *conf, struct struct_block *block);

/*---------------------------------------------------------------------------*
 * NAME: _output_send_packet_red()
 * DESC: print in red the content of a sent packet using stdout
 *---------------------------------------------------------------------------*/
void _output_send_packet_red(config *conf, struct struct_block *block);

/*---------------------------------------------------------------------------*
 * NAME: _output_recv_packet()
 * DESC: print the content of a recv packet using stdout
 *---------------------------------------------------------------------------*/
void _output_recv_packet(config *conf, struct struct_block *block);

/*---------------------------------------------------------------------------*
 * NAME: _output_recv_packet_compare()
 * DESC: print the content of a packet using stdout
 *---------------------------------------------------------------------------*/
void _output_recv_packet_compare(config *conf, struct struct_block *block, unsigned char *buffer, unsigned int bytes);

