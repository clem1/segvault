#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define QUES "QUES"
#define END "END\n"


void read_(int fd, void *buf, size_t count) {

  if (read(fd, buf, count) <= 0) {
    fprintf(stdout, "CONNECTION CLOSED BY FOREIGN HOST\n");
  }
}

void function(int socket) {

  char string[128];
  char buffer4[4];
  unsigned int size;

  /* init */
  bzero(buffer4, 4);
  bzero(string, 128);

  /* read buffer4 */
  read_(socket, buffer4, 4);

  /* check header */
  if (strncmp(buffer4, QUES, strlen(QUES)) != 0) {
    fprintf(stdout, "ERROR:BAD_HEADER\n");
    return;
  }

  /* check size */
  size = 0;
  read_(socket, &size, 4);
  size = ntohl(size);
  fprintf(stdout, "SIZE:%d\n", size);

  /* read string */
  read_(socket, string, size);
  
  /* read end */
  bzero(buffer4, 4);
  read_(socket, buffer4, 4);

  /* check header */
  if (strncmp(buffer4, END, strlen(END)) != 0) {
    fprintf(stdout, "ERROR:BAD_ENDER\n");
    return;
  }
  fprintf(stdout, "OK\n");
}

int main(int argc, char **argv) {

  unsigned int port;
  struct sockaddr_in server;
  struct sockaddr_in client;
  int serv_sock;
  int sock;
  unsigned int len;
  int yes = 1;

  if (argc != 2) {
    printf("%s <listening_port>\n", argv[0]);
    exit(-1);
  }

  /* port */
  port = atoi(argv[1]);

  /* init */
  bzero((char *) &server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons((unsigned short)port);

  /* socket */
  serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (serv_sock < 0) {
    perror("socket(): ");
    exit(-1);
  }

  if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0) {
    perror("setsockopt(): ");
    exit(-1);
  }


  /* bind */
  if ((bind(serv_sock, (struct sockaddr *)&server, sizeof(server))) < 0) {
    perror("bind(): ");
    exit(-1);
  }

  /* listen */
  if ((listen(serv_sock, 1)) < 0) {
    perror("listen(): ");
    exit(-1);
  }

  while(1) {

    /* wait for connection */
    printf("[*] wait for a connection on port: %d\n", port);
    len = sizeof(client);
    sock = accept(serv_sock, (struct sockaddr *)&client, &len);
    
    /* connection */
    printf("[*] connection from: %s\n", inet_ntoa(client.sin_addr));

    /* parse the data */
    function(sock);

  }

  /* shutdown connection */
  shutdown(sock, SHUT_RDWR);
  close(sock);


  return 0;
}
