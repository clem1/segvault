/*
 * NAME: overflow.c
 * DATE: 2004-04-22 - Cambridge, Computer Labs.
 * DESC: Tired about `perl -e 'print "A"x500'`
 *       Just write overflow <numbah> -> 'A' x <numbah> 
 *       Or overflow <numba> <string>
 * COMPILE: gcc -Wall -O3 -o x x.c
 */

#include <stdio.h>
#include <stdlib.h> /* atoi */

int main(int argc, char **argv) {

  unsigned int i;
  unsigned int number;
  char default_string[] = "A";
  char *string;

  if(argc < 2) {  
    fprintf(stderr,"Usage: %s <number> [<string>]\n", argv[0]);
    fprintf(stderr,"\t<number>\t repeated times for the string.\n");
    fprintf(stderr,"\t<string>\t default: \"A\".\n");
    return -1;
  }

  number = atoi(argv[1]);

  if(argc == 2)
    string = default_string;
  else
    string = argv[2];

  for(i=0;i<number;i++)
    printf("%s", string);
  
  return 0;
}
