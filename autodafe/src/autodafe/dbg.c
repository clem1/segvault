/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Sep.2004 ---*                
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : dbg.c
 * DESCRIPTION: all the debugger related functions
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <netdb.h>        /* gethostbyname, hstrerror */
#include <strings.h>      /* bcopy */
#include <string.h>       /* memset */
#include <sys/types.h>    /* socket */
#include <sys/socket.h>   /* socket, shutdown, inet_ntoa */
#include <netinet/in.h>   /* inet_ntoa */
#include <arpa/inet.h>    /* inet_nota */
#include <unistd.h>       /* close */
#include <stdlib.h>       /* free */
#include <errno.h>        /* errno */

#include "debug.h"
#include "conf.h"
#include "chrono.h"     
#include "proto.h"
#include "../../include/autodafe.h"

extern int h_errno; /* gethostbyname error */



/*---------------------------------------------------------------------------*                                            
 * DESC: recv information using write but detect errors
 * RETN: 0 if ok, -1 if error (fatal)
 *---------------------------------------------------------------------------*/
int dbg_write(config *conf, void *buffer, unsigned int size) {

  unsigned int bytes;
  int i;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  bytes = write(conf->dbg_socket, buffer, size);
  if (bytes <= 0) {
    error_("dbg_write: ");
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* debug */
  debug(3, "[dbg write buffer dump]:\n");
  for(i=0;i<size;i++)
    debug(3, "0x%02x\n", *((unsigned char*)(buffer + i)));
  debug(1, "<-----------------------[quit]\n");

  /* everything is ok */
  return 0;
}

/*---------------------------------------------------------------------------*                                            
 * DESC: send information using read but detect errors
 * RETN: 0 if ok, -1 if error (fatal)
 *---------------------------------------------------------------------------*/
int dbg_read(config *conf, void *buffer, unsigned int size) {

  unsigned int bytes;
  int i;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  bytes = read(conf->dbg_socket, buffer, size);
  if (bytes < 0) {
    error_("dbg_read: ");
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  if (bytes == 0) {
    error_("connection closed by the debugger.\n");
    error_("QUITTING!\n");
    return -1;
  }

  /* debug */
  debug(3, "[dbg read buffer dump]:\n");
  for(i=0;i<size;i++) {
    debug(3, "0x%02x\n", *((unsigned char*)(buffer + i)));
  }
  debug(1, "<-----------------------[quit]\n");
    
  /* everything is ok */
  return 0;
}

/*---------------------------------------------------------------------------*                                            
 * DESC: wait an ACK message
 * RETN: int (0 if ok, -1 if error)
 *---------------------------------------------------------------------------*/
int dbg_wait_for_ack(config *conf) {

  char opcode;

  /* debug */
 debug(1, "<-----------------------[enter]\n");
 
 /* read one byte */
 if (dbg_read(conf, &opcode, sizeof(opcode)) != 0) return -1;

 /* check if it's an ack */
 if (opcode != INET_ACK_MSG) {
   error_("message INET_ACK_MSG expected!\n");
   error_("QUITTING!\n");
   return -1;
 }

  /* debug */
 debug(2, "[INET_ACK_MSG] received.\n");
 debug(1, "<-----------------------[quit]\n");

 /* everything is ok */
 return 0;
}

/*---------------------------------------------------------------------------*                                            
 * DESC: recv a message to the debugger 
 * RETN: 0 if ok
 *      -1 if error (fatal)
 *       1 if quit (but normal i.e. segfault or quit)
 *---------------------------------------------------------------------------*/
int dbg_read_msg(config *conf) {

  char opcode;
  int result = 0;
  int id;
  struct struct_fuzz *fuzz;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the opcode */
  if (dbg_read(conf, &opcode, sizeof(opcode))) return -1;
  
  switch((unsigned int) opcode) {

  case INET_SIGSEGV_MSG: /* the best case 8^P */

    verbose_("[*] the debugger says: [***] Segmentation fault - signal [***]\n");
    result = 1;
    break;
    
  case INET_WEIGHT_MSG: /* we add a weight to the fuzz id sent by this message */

    /* read an big-endian 32 bit value = fuzz id */
    dbg_read(conf, &id, sizeof(int));

    /* if little endian */
    id = ntohl(id);

    /* add the weight of this fuzz */
    for(fuzz = conf->fuzz;fuzz;fuzz = fuzz->next) {
      if (fuzz->id == id) {
	verbose_("[*] the debugger says: increase the weight of the fuzz id: %d\n", id);
	fuzz->weight++;
	break;
      }
    }

      /* check bad id */
    if (fuzz == NULL) {
      error_("the debugger says: increase the weight of fuzz id: %d. But id *NOT* found!\n", id);
      error_("QUITTING!\n");
      return -1;
    }

    result = 0;
    break;

  case INET_EXIT_MSG: /* the worst case :'( */

    verbose_("[*] the debugger says: software exited normally. We stop the fuzzing.\n");
    result = 1;
    break;

  default:
    error_("unknown message received from the debugger: %d\n", opcode);
    error_("QUITTING!\n");
    return -1;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* everything is ok */
  return result;

}

/*---------------------------------------------------------------------------*                                            
 * DESC: send a message to the debugger 
 * RETN: 0 if ok, -1 if error (fatal)
 *---------------------------------------------------------------------------*/
int dbg_send_msg(config *conf, unsigned int type, unsigned int id, void *string, unsigned int size) {

  unsigned int n_size;
  unsigned int n_id;
  unsigned int major;
  unsigned int minor;
  unsigned char opcode;
  char *buffer = NULL;


  /* debug */
  debug(1, "<-----------------------[enter]\n");
  debug(2, "[type]: %d\n", type);

  switch(type) {


  case INET_VERSION_A_MSG: /* send the init. message (version */

    /* create the buffer */
    buffer = malloc_(sizeof(char) + sizeof(int) + sizeof(int));

    /* fill the opcode */
    opcode = (unsigned char) type;
    memcpy(buffer, &opcode, sizeof(char));

    /* fill the major version */
    major = htonl(MAJOR_VERSION);
    memcpy(buffer+1, &major, sizeof(int));

    /* fill the minor version */
    minor = htonl(MINOR_VERSION);
    memcpy(buffer+1+4, &minor, sizeof(int));

    /* send the buffer */
    if (dbg_write(conf, buffer, 1+4+4) != 0) return -1;

    /* free the buffer */
    free(buffer); buffer = NULL;

    /* debug */
    debug(2, "[INET_VERSION_A_MSG]-+-> sent to the debugger.\n");

    break;

  case INET_STRING_A_MSG: /* send the original string of a fuzz (monitored) */

    /* allocate buffer */
    buffer = malloc_(sizeof(char) + sizeof(int) + sizeof(int) + size);

    /* convert network values */
    n_size = htonl(size);
    n_id = htonl(id);
    opcode = (unsigned char) type;

    /* fill the buffer */
    memcpy(buffer      , &opcode, sizeof(char));
    memcpy(buffer+1    , &n_id, sizeof(int));
    memcpy(buffer+1+4  , &n_size, sizeof(int));
    memcpy(buffer+1+4+4, string, size);

    /* send the buffer */
    if (dbg_write(conf, buffer, 1+4+4+size) != 0) return -1;

    /* free buffer*/
    free(buffer);

    /* allocate for a null byte the buffer (debug) */
    buffer = malloc_(strlen(string)+1);
    memcpy(buffer, string, strlen(string));
    buffer[sizeof(buffer)] = '\0';

    /* debug */
    debug(2, "[INET_STRING_A_MSG]-+-> sent to the debugger.\n");
    debug(2, "                    +-> id: %d\n", id);
    debug(2, "                    +-> size: %d\n", size);
    debug(2, "                    +-> string: %s\n", buffer);

    /* free buffer */
    free(buffer); buffer = NULL;

    break;

  case INET_START_A_MSG: /* send the start msg -> the fuzzer cannot speak anymore */

    /* init opcode */
    opcode = (unsigned char) INET_START_A_MSG;

    /* send message */
    if (dbg_write(conf, &opcode, sizeof(opcode)) != 0) return -1;

    /* debug */
    debug(2, "[INET_START_A_MSG]-+-> sent to the debugger.\n");

    break;

  default:
    error_("unknown message to send to the debugger!\n");
    error_("QUITTING!\n");
    return -1;
  }

  /* wait for the INET_ACK_MSG */
  if (dbg_wait_for_ack(conf) != 0) return -1;

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* everything is ok */
  return 0;
}

/*---------------------------------------------------------------------------*                                            
 * DESC: start the connection with the debugger
 * RETN: 0 if ok, -1 if error (fatal)
 *---------------------------------------------------------------------------*/
int dbg_connection(config *conf) {

  struct sockaddr_in dbg;
  struct hostent *h;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* open a tcp connection with the debugger */
  h = gethostbyname(conf->dbg_host);
  if (!h) {
    error_("%s: \"%s\"\n", hstrerror(h_errno), conf->dbg_host);
    error_("QUITTING!\n");
    return -1;
  }

  /* socket */
  conf->dbg_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (conf->dbg_socket < 0) {
    error_("socket(): ");
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* init sockaddr_in structure */
  memset(&dbg, 0, sizeof(struct sockaddr_in));
  bcopy(h->h_addr, (char *) &dbg.sin_addr, h->h_length);
  dbg.sin_family = AF_INET;
  dbg.sin_port = htons((short) conf->dbg_port);

  /* connect */
  if (connect(conf->dbg_socket, (struct sockaddr *) &dbg, sizeof(struct sockaddr_in)) < 0) {
    error_("connect(): ");
    perror("");
    error_("QUITTING!\n");
    shutdown(conf->dbg_socket, SHUT_RDWR);
    close(conf->dbg_socket);
    conf->dbg_socket = -1;
    debug(3, "socket closed\n");
    return -1;
  }

  /* verbose */
  verbose_("[*] debugger connected: %s on port %d\n", conf->dbg_host, conf->dbg_port);

  /* send the INET_VERSION_A_MSG */
  if (dbg_send_msg(conf, INET_VERSION_A_MSG, 0, NULL, 0) == -1) return -1;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* everything is ok */
  return 0;

}
