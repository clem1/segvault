/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Aug.2004 ---*                                          
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : transmit.c
 * DESCRIPTION: all the transmission related functions
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <netdb.h>        /* gethostbyname, hstrerror */
#include <strings.h>      /* bcopy */
#include <string.h>       /* memset */
#include <sys/types.h>    /* socket, select */
#include <sys/socket.h>   /* socket, shutdown, inet_ntoa */
#include <netinet/in.h>   /* inet_ntoa */
#include <arpa/inet.h>    /* inet_nota */
#include <unistd.h>       /* close, select */
#include <stdlib.h>       /* free */
#include <errno.h>        /* errno */
#include <sys/select.h>   /* select */
#include <sys/time.h>     /* select */


#include "debug.h"
#include "conf.h"
#include "chrono.h"     
#include "dbg.h"

extern struct sockaddr_in server; /* needed between send/recv_fuzz and client/server_fuzz */

/*---------------------------------------------------------------------------*                                            
 * NAME: send_fuzz()
 * DESC: send a buffer using tcp/udp or file method:
 *       tcp/udp: use sendto 
 * RETN: >=0 if everything is OK
 *        -1 if there is a big error (not recoverable)
 *        -2 connection closed by foreign host.
 *---------------------------------------------------------------------------*/
int send_fuzz(config *conf, struct struct_block *block) {

  unsigned int bytes;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* send using fwrite (file) */
  if (conf->type == 2) {
    bytes = fwrite(conf->buf_fuzz + block->offset, sizeof(char), block->size, conf->fuzz_file);
    debug(3, "--->bytes written: %d\n", bytes);
    if (bytes <= 0)
      return -1;
  }
  /* send using sendto (compatible tcp *and* udp) */
  else {
    bytes = sendto(conf->socket, 
		   conf->buf_fuzz + block->offset, 
		   block->size, 
		   0,
		   (struct sockaddr *)&(server), 
		   sizeof(server));
    debug(1, "send_fuzz bytes: %d\n", bytes);

    if (bytes == -1) {
      
      /* this is a "Broken pipe" ie: connection closed by foreign host. We
	 detect it and reconnect to the server with return value -2 */
      if (errno == EPIPE) { /* EPIPE = 32 cf. /usr/include/asm/errno.h */
      verbose_("[!] connection closed by foreign host (sendto)\n");
      return -2;
      }
      error_("sendto error: ");
      perror("");
      return -1;
    }

    /* debugger detection. We wait for at least DBG_TIMEOUT
       seconds an answer of the debugger. Even if the answer arrives
       in the next check it's ok (independant messages) */
    if (conf->dbg_mode) {
      fd_set rfds;
      struct timeval tv;
      int retval;
      
      tv.tv_sec = DBG_TIMEOUT_SEC;
      tv.tv_usec = DBG_TIMEOUT_USEC;
      FD_ZERO(&rfds);
      FD_SET(conf->dbg_socket, &rfds);
      retval = select(conf->dbg_socket+1, &rfds, NULL, NULL, &tv);
      
      /* error */
      if (retval == -1) {
	error_("select error: ");
	perror("");
	return -1;
      }
      
      /* there is something from the dbg */
      if (retval) {
	verbose_("[*] the debugger has something to say!\n");
	
	if (dbg_read_msg(conf)) bytes = -1;

      } 
    }
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  return bytes;
}


/*---------------------------------------------------------------------------*                                            
 * NAME: recv_fuzz()
 * DESC: recv a buffer using tcp/udp or file method:
 *       tcp/udp: use recvfrom
 * RETN: > 0 if everything is OK
 *       0 if connection closed by foreign host
 *      -1 if major error
 *---------------------------------------------------------------------------*/
int recv_fuzz(config *conf, struct struct_block *block, unsigned char *buffer) {
  int bytes;
  unsigned int len;

  /* recv using *nothing* (file) */
  if (conf->type == 2) {
    bytes = 1;    /* do nothing */
  }
    /* recv using recvfrom (compatible tcp *and* udp) */
  else {

    /* timeout for receiving an answer */
    fd_set rfds;
    struct timeval tv;
    int retval;

    tv.tv_sec = conf->timeout;
    tv.tv_usec = 0;

    FD_ZERO(&rfds);
    FD_SET(conf->socket, &rfds);
    retval = select(conf->socket+1, &rfds, NULL, NULL, &tv);

    /* error */
    if (retval == -1) {
      error_("select error: ");
      perror("");
      return -1;
    }

    /* there is data */
    if (retval) {

      len = sizeof(server);
      debug(3, "len: %d\n", len);
      debug(3, "block->size :%d\n", block->size); 
      bytes = recvfrom(conf->socket, 
		       buffer, 
		       block->size, 
		       0, 
		       (struct sockaddr *)&server, 
		       &len);
      debug(1, "recv_fuzz bytes: %d\n", bytes);
      if (bytes == -1) {
	error_("recvfrom error: ");
	perror("");
	free(buffer);
	buffer = NULL;
	return 0;
      }
      if (bytes == 0) {
	verbose_("[!] connection closed by foreign host (recv_from)\n");
	free(buffer);
	buffer = NULL;
      }
      return bytes;
    }

    /* nothing has been received for RECV_TIMEOUT_SEC and RECV_TIMEOUT_USEC */
    else {
      verbose_("[!] timeout of %d [sec] expired... launch the next attack.\n", conf->timeout);
      free(buffer);
      buffer = NULL;

      return 0;
    }
  }
}
