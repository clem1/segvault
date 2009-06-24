/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : network.c
 * DESCRIPTION: all the internet related functions
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

extern int h_errno; /* gethostbyname error */
extern struct sockaddr_in server; /* needed between send/recv_fuzz and client/server_fuzz */


/*---------------------------------------------------------------------------*
 * NAME: shutdown_socket()
 * DESC: shutdown the socket if opened
 *---------------------------------------------------------------------------*/
void shutdown_socket(config *conf) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  if (conf->socket >= 0) {
    shutdown (conf->socket, SHUT_RDWR);
    close (conf->socket);
    conf->socket = -1;
    debug(3,"socket closed\n");
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*
 * NAME: client_fuzz()
 * DESC: initialize a tcp/udp client connection and send the buffer
 * RETN: 0 if everything is ok
 *       1 if sendto detects a Broken pipe (connection closed by foreign host)
 *       2 if recv_from receive 0 data     (connection closed by foreign host)
 *      -1 if error
 *---------------------------------------------------------------------------*/
int client_fuzz(config *conf) {

  struct hostent *h;
  int result;
  int i;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* name resolving */
  h = gethostbyname(conf->host);
  if (!h) {
    error_("%s: \"%s\"\n", hstrerror(h_errno), conf->host);
    error_("QUITTING!\n");
    return -1;
  }

  /* socket tcp */
  if (conf->type == 0) {
    conf->socket = socket(PF_INET, SOCK_STREAM, 0);
  }

  /* socket udp */
  else {
  conf->socket = socket(PF_INET, SOCK_DGRAM, 0);
  }
  
  if (conf->socket < 0) {
    error_("socket(): ");
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* init sockaddr_in structure */
  memset(&server, 0, sizeof(struct sockaddr_in));
  bcopy(h->h_addr, (char *) &server.sin_addr, h->h_length);
  server.sin_family = AF_INET;
  server.sin_port = htons((short) conf->port);

  /* we wait X seconds before connecting, just to let the time to 
     the debugger to fire up the software. */
  if (conf->wait) {
    for(i=conf->wait;i>0;i--) {
      verbose_("\r");
      verbose_("[%d] waiting %d seconds before opening connection...", i, i);
      fflush(stdout);
      sleep(1);
    }
    verbose_("\n");
    /* just the first time */
    conf->wait = 0;
  }

  /* connect */
  if (connect(conf->socket, (struct sockaddr *) &server, sizeof(struct sockaddr_in)) < 0) {
    error_("connect(): ");
    perror("");
    error_("QUITTING!\n");
    shutdown_socket(conf);
    return -1;
  }

  /* verbose */
  verbose_("[*] connected to: %s on port: %d\n", conf->host, conf->port);

  /* parse chrono structure */
  result = parse_chrono(conf);

  /* shutdown socket */
  shutdown_socket(conf);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return value see RETN */
  return result;
}

/*---------------------------------------------------------------------------*
 * NAME: server_fuzz()
 * DESC: initialize a tcp/udp server connection and send the buffer
 * RETN: 0 if everything is ok
 *       1 if sendto detects a Broken pipe (connection closed by foreign host)
 *       2 if recv_from receive 0 data     (connection closed by foreign host)
 *      -1 if error
 *---------------------------------------------------------------------------*/
int server_fuzz(config *conf) {

  struct sockaddr_in client;
  int serv_socket = -1;
  unsigned int len;
  int yes = 1;
  int result;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* init */
  bzero((char *) &client, sizeof(client));
  server.sin_family = AF_INET;
  server.sin_port = htons(conf->port);

  /* tcp socket */
  if (conf->type == 0) {
    serv_socket = socket(PF_INET, SOCK_STREAM, 0);
  }

  /* udp socket */
  else {
    serv_socket = socket(PF_INET, SOCK_DGRAM, 0);
  }

  if (serv_socket < 0) {
    error_("socket(): ");
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* setsockopt - avoid the bind error */
  if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0) {
    error_("setsockopt(): ");
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* bind */
  if ((bind(serv_socket, (struct sockaddr *)&(server), sizeof(server))) < 0) {
    error_("bind(): ");
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* tcp listen */
  if (conf->type == 0) {
    if ((listen(serv_socket, 1)) < 0) {
      error_("listen(): ");
      perror("");
      error_("QUITTING!\n");
      return -1;
    }
  }

  /* waiting for connection */
  verbose_("[*] waiting for a connection on port: %d\n", conf->port);

  /* tcp accept */
  if (conf->type == 0) {
    len = sizeof(client);
    conf->socket = accept(serv_socket, (struct sockaddr *)&client, &len);
    
    /* connection */
    verbose_("[*] connection from: %s\n", inet_ntoa(client.sin_addr));
  }

  /* udp has only one socket */
  else {
    conf->socket = serv_socket;
  }

  /* parse chrono structure */
  result = parse_chrono(conf);
  
  /* close opened socket */
  shutdown_socket(conf);
  if (serv_socket >= 0) {
    shutdown (serv_socket, SHUT_RDWR);
    close (serv_socket);
    serv_socket = -1;
    debug(3,"serv_socket closed\n");
  }
  
  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return value see RETN */
  return result;
}

