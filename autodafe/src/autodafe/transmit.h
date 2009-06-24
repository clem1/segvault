/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Aug.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : transmit.h
 * DESCRIPTION: define send/recv functions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*                                            
 * NAME: send_fuzz()
 * DESC: send a buffer using tcp/udp or file method:
 *       tcp: just use send()
 *       udp: use sendto with sockaddr initialized in udp_client/server_fuzz
 *       file: use fwrite with stream opened in file_fuzz()
 * RETN: 0 if everything is OK, -1 if ERROR.
 *---------------------------------------------------------------------------*/
int send_fuzz(config *conf, struct struct_block *block);

/*---------------------------------------------------------------------------*                                            
 * NAME: recv_fuzz()
 * DESC: recv a buffer using tcp/udp or file method:
 *       tcp: just use recv()
 *       udp: use recvfrom with sockaddr initialized in udp_client/server_fuzz
 *       file: ignored
 * RETN: number of bytes received
 *---------------------------------------------------------------------------*/
int recv_fuzz(config *conf, struct struct_block *block, unsigned char *buffer);
