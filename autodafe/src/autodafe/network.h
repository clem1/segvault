/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : network.h
 * DESCRIPTION: define send/recv functions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * NAME: client_fuzz()
 * DESC: initialize a tcp/udp client connection and send the buffer
 *---------------------------------------------------------------------------*/
int client_fuzz(config *conf);

/*---------------------------------------------------------------------------*
 * NAME: server_fuzz()
 * DESC: initialize a tcp/udp server connection and send the buffer
 *---------------------------------------------------------------------------*/
int server_fuzz(config *conf);

/*---------------------------------------------------------------------------*
 * NAME: shutdown_socket()
 * DESC: shutdown the socket if opened
 *---------------------------------------------------------------------------*/
void shutdown_socket(config *conf);

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
