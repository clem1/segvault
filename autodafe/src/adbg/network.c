/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Sep.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : network.c
 * DESCRIPTION: All the functions used to communicate with the fuzzer
 * DEPENDANCES: 
 * COMPILATION: 
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <sys/socket.h>   /* socket, shutdown, inet_ntoa */
#include <sys/types.h>     /* socket */
#include <netinet/in.h>      /* inet_nota */
#include <arpa/inet.h>    /* inet_nota */
#include <unistd.h>       /* close, read, write */
#include <stdlib.h>       /* free */
#include <strings.h>      /* bzero */
#include <string.h>       /* strlen */
#include <netinet/in.h>   /* ntohl */

#include "debug.h"
#include "conf.h"
#include "proto.h"
#include "../../include/autodafe.h"

/*---------------------------------------------------------------------------*
 * DESC: send information using read and write but detect errors
 * RETN: void (fatal if error)
 *---------------------------------------------------------------------------*/
void inet_read(config *conf, void *buffer, unsigned int size) {
  unsigned int bytes;
  int i;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  bytes = read(conf->fuzzer_socket, buffer, size);
  if (bytes < 0) {
    error_("inet_read: ");
    perror("");
    error_("QUITTING!\n");
    exit(-1);
  }

  if (bytes == 0) {
    error_("connection closed by the fuzzer.\n");
    error_("QUITTING!\n");
    exit(-1);
  }

  /* debug */
  debug(3, "[buffer dump]:\n");
  for (i=0;i<size;i++) {
    debug(3, "0x%02x\n", *((unsigned char *)(buffer + i)));
  }
  debug(1, "<-----------------------[quit]\n");
}

/*---------------------------------------------------------------------------*
 * DESC: recv information using read and write but detect errors
 * RETN: void (fatal error)
 *---------------------------------------------------------------------------*/
void inet_write(config *conf, void *buffer, unsigned int size) {

  unsigned int bytes;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  bytes = write(conf->fuzzer_socket, buffer, size);
  if (bytes <= 0) {
    error_("inet_write: ");
    perror("");
    error_("QUITTING!\n");
    exit(-1);
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");
}

/*---------------------------------------------------------------------------*
 * DESC: listen on a port a message from the fuzzer and parse it
 * RETN: void (fatal if error)
 *---------------------------------------------------------------------------*/
void inet_wait_connection(config *conf) {

  struct sockaddr_in dbg;
  struct sockaddr_in fuzzer;
  int serv_socket = -1;
  int yes = 1;
  unsigned int len;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* init */
  bzero((char *)&fuzzer, sizeof(fuzzer));
  bzero((char *)&dbg, sizeof(dbg));
  dbg.sin_family = AF_INET;
  dbg.sin_port = (htons(conf->port));

  /* tcp socket */
  serv_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (serv_socket < 0) {
    error_("socket(): ");
    perror("");
    error_("QUITTING!\n");
    exit(-1);
  }

  /* setsockopt - avoid the bind error "port already used" */
  if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0) {
    error_("setsockopt(): ");
    perror("");
    error_("QUITTING!\n");
    exit(-1);
  }

  /* bind */
  if ((bind(serv_socket, (struct sockaddr *)&(dbg), sizeof(dbg))) < 0) {
    error_("bind(): ");
    perror("");
    error_("QUITTING!\n");
    exit(-1);
  }

  /* tcp listen */
  if ((listen(serv_socket, 1)) < 0) {
    error_("listen(): ");
    perror("");
    error_("QUITTING!\n");
    exit(-1);
  }

  /* verbose */
  verbose_("[*] waiting for a connection on port: %d\n", conf->port);

  /* tcp accept */
  len = sizeof(fuzzer);
  conf->fuzzer_socket = accept(serv_socket, (struct sockaddr *)&fuzzer, &len);

  /* verbose */
  verbose_("[*] connection from: %s\n", inet_ntoa(fuzzer.sin_addr));

  /* debug */
  debug(1, "<-----------------------[quit]\n");
}


/*---------------------------------------------------------------------------*
 * DESC: send asynchrone msg (SIGSEGV, EXIT, WEIGHT)
 * RETN: void (fatal error)
 *---------------------------------------------------------------------------*/
void inet_send_msg(config *conf, unsigned int type, unsigned id) {

  unsigned int n_id;
  unsigned char opcode = (unsigned char) type;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  switch(type) {

  case INET_SIGSEGV_MSG: /* just send a byte and quit */
    inet_write(conf, &opcode, sizeof(opcode));
    break;

  case INET_EXIT_MSG: /* just send a byte and quit */
    inet_write(conf, &opcode, sizeof(opcode));
    break;

  case INET_WEIGHT_MSG: /* send a byte and the id of the fuzz */
    n_id = htonl(id);
    inet_write(conf, &opcode, sizeof(opcode));
    inet_write(conf, &n_id, sizeof(n_id));
    break;

  default:
    error_("unknown asynchrone message to send to the fuzzer.\n");
    error_("QUITTING!\n");
    exit(-1);
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*
 * DESC: parse the received message with ack
 * RETN: void (fatal error)
 *---------------------------------------------------------------------------*/
void inet_decode_msg(config *conf, unsigned int opcode) {

  unsigned char ack = INET_ACK_MSG;
  int major = 0;
  int minor = 0;
  unsigned int id = 0;
  unsigned int size = 0;
  unsigned char *recv_string;
  struct struct_string *string;
  struct struct_string *prev = NULL;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  switch(opcode) {
  case INET_VERSION_A_MSG:  /* send the version (init. message) */

    /* read the version (2x 4 bytes (big-endian)) */
    inet_read(conf, &major, sizeof(int));
    inet_read(conf, &minor, sizeof(int));

    /* debug */
    debug(2, "[INET_VERSION_A_MSG]: v.%d.%d\n", ntohl(major), ntohl(minor));

    /* check version */
    if ((ntohl(major) != MAJOR_VERSION) || (ntohl(minor) != MINOR_VERSION)) {
      error_("incompatible version between fuzzer (v.%d.%d) and debugger (v.%d.%d)\n", 
	     major, minor, MAJOR_VERSION, MINOR_VERSION);
      error_("QUITTING!\n");
      exit(-1);
    }
    
    /* verbose */
    verbose_("[*] fuzzer authenticated and connected. (v.%d.%d)\n", MAJOR_VERSION, MINOR_VERSION);

    break;

  case INET_START_A_MSG: /* send the start msg. After, the fuzzer cannot speak anymore (and no more ack) */

    /* debug */
    debug(2, "[INET_START_A_MSG]\n");

    /* just verbose */
    verbose_("[*] targeted software running... (analysis in progress)\n");

    break;

  case INET_STRING_A_MSG: /* the fuzzer send which string has to be monitored by the debugger */

    /* read the id of the string */
    inet_read(conf, &id, sizeof(int));

    /* convert the network value in host value */
    id = ntohl(id);

    /* read the size of the string */
    inet_read(conf, &size, sizeof(int));

    /* convert the network value in host value */
    size = ntohl(size);

    /* debug */
    debug(2, "[INET_STRING_A_MSG] id: %d, size: %d\n", id, size);

    /* allocate buffer (don't forget to free when free string struct) */
    recv_string = malloc_((unsigned int) size + 1);

    /* init buffer (thanks to valgrind! Really this is a amazing tool!) */
    bzero(recv_string, (unsigned int) size + 1);

    /* read the string */
    inet_read(conf, recv_string, size);

    /* add the null byte (string) */
    recv_string[size] = '\0';

    /* fill the string linked list */
    for(string=conf->string;string;string=string->next) {
      prev = string;

      if (id ==string->id) {
	error_("string sended by the fuzzer is already monitored: %s\n", recv_string);
	goto send_ack;
      }
    }
    
    /* create the new entry */
    string = malloc_(sizeof(struct struct_string));

    /* init buffer (thanks to valgrind! Really this is a amazing tool!) */
    bzero(string, sizeof(struct struct_string));

    /* init the structure */
    string->next = NULL;
    string->string = recv_string; 
    string->id = id;

    /* update the previous entry */
    if (prev) prev->next = string;

    /* update if it's the first entry */
    if (!conf->string) conf->string = string;

    /* verbose */
    verbose_ ("[+]-+-> add monitored string: \"%s\"\n", string->string);
    verbose__("    +-> id: %d\n", string->id);

    break;

  default:
    error_("unknown message sent by the fuzzer to the debugger!\n");
    error_("QUITTING!\n");
    exit(-1);
  }

  
 send_ack:

  /* for theses messages, the fuzzer wait for an INET_ACK_MSG */
  debug(2, "[INET_ACK_MSG] sent to the fuzzer.\n");
  inet_write(conf, &ack, sizeof(ack));

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*
 * DESC: wait for an opcode from the fuzzer
 * RETN: return the value of the opcode (u_int)
 *---------------------------------------------------------------------------*/
unsigned int inet_fetch_msg(config *conf) {

  unsigned char opcode;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the first byte of the message */
  inet_read(conf, &opcode, sizeof(char));

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return u_int */
  return (unsigned int) opcode;
}



/*---------------------------------------------------------------------------*
 * DESC: parse the tcp connection with the fuzzer
 * RETN: void (fatal if error)
 *---------------------------------------------------------------------------*/
void inet_connection(config *conf) {

  unsigned int opcode;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* start the connection */
  inet_wait_connection(conf);

  /* wait for a INET_VERSION_A_MSG */
  opcode = inet_fetch_msg(conf);
  if (opcode != INET_VERSION_A_MSG) {
    error_("bad protocol: the fuzzer cannot be identified.\n");
    error_("QUITTING!\n");
    exit(-1);
  }

  /* parse the message */
  inet_decode_msg(conf, opcode);

  /* wait for a INET_START_A_MSG (and parse if it's another one i.e. STRING_MSG */
  do {
    opcode = inet_fetch_msg(conf);
    inet_decode_msg(conf, opcode);
  } while(opcode != INET_START_A_MSG);
  
  /* debug */
  debug(1, "<-----------------------[quit]\n");
}
