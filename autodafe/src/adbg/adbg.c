/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Aug.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : dbg.c
 * DESCRIPTION: 
 * DEPENDANCES: 
 * COMPILATION: 
 *---------------------------------------------------------------------------*/


#define _GNU_SOURCE           /* getopt_long */
#define DBG_MIN_ARGS 3      

#include <stdio.h>
#include <string.h>           /* memcpy */
#include <stdlib.h>
#include <getopt.h>           /* getopt_long */

#include "debug.h"
#include "conf.h"
#include "gdb.h"
#include "../../include/autodafe.h"

extern unsigned int debug;
extern unsigned int verbose;

/*---------------------------------------------------------------------------*
 * NAME: version
 * DESC: Print the version of the program
 *---------------------------------------------------------------------------*/
void version(char **argv) {
  fprintf(stderr, "%s-v.%d.%d ", argv[0], MAJOR_VERSION, MINOR_VERSION);
  fprintf(stderr, "- (c) 2004-2006 "NAME" <"EMAIL">\n\n");
}

/*---------------------------------------------------------------------------*
 * NAME: usage
 * DESC: Print the usage of the program
 *---------------------------------------------------------------------------*/
void usage(char **argv, config *conf) {
  fprintf(stderr, "%s-v.%d.%d - debugger which listen the targeted software.\n", argv[0], MAJOR_VERSION, MINOR_VERSION);
  version(argv);
  fprintf(stderr, "Usage: %s [OPTION...] [<program>] [\"<args>\"]\n\n", argv[0]);
  fprintf(stderr, "  List of the basic functions:\n");
  fprintf(stderr, "    -h, --help             display this help and exit.\n");
  fprintf(stderr, "    -V, --version          output version information and exit.\n");
  fprintf(stderr, "    -v, --verbose          verbose output (use twice for more verbose info).\n");
  fprintf(stderr, "    -d  --debug            debug output (use 2x or 3x for more debug info).\n\n");
  fprintf(stderr, "  List of the internal's debugger' s functions:\n");
  fprintf(stderr, "    -G  --gdb-dump=FILE    copy the gdb's output in a file.\n");
  fprintf(stderr, "    -s  --string-dump=FILE copy the vulnerables strings in a file.\n\n");
  fprintf(stderr, "  List of the debugger's functions:\n");
  fprintf(stderr, "    -p  --port=NUM         The listening TCP port of the debugger.\n");  
  fprintf(stderr, "    -P  --pid=NUM          the pid of the program you want to debug.\n");
  fprintf(stderr, "    -i  --input=FILE       file which contains the strings used to break.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "The standard output give the informations.\n\n");

  /* free the structure and exit */
  free(conf);
  exit(-1);
}

/*---------------------------------------------------------------------------*
 * NAME: parsing_args
 * DESC: Parse the arguments using getopt_long return the first non-optionnal
 *       index in argv[]. 
 *---------------------------------------------------------------------------*/
unsigned int parsing_args(int argc, char **argv, config *conf) {
  
  static struct option long_options[] = {
    {"help", 0, NULL, 'h'},
    {"version", 0, NULL, 'V'},
    {"verbose", 0, NULL, 'v'},
    {"debug", 0, NULL, 'd'},
    {"port", 1, NULL, 'p'},
    {"input", 1, NULL, 'i'},
    {"gdb-dump", 1, NULL, 'G'},
    {"string-dump", 1, NULL, 's'},
    {"pid", 1, NULL, 'P'},
    {NULL, 0, NULL, 0}
  };
  int c;
  int option_index = 0;
  char *options = "Vvhdp:i:G:P:s:";

  if (argc < DBG_MIN_ARGS) {
    usage(argv, conf);
  }
  
  while (1) {
    c = getopt_long(argc, argv, options, long_options, &option_index);
    if (c == -1) 
      break;

    /* check the arguments */
    switch(c) {

    case 'h': /* help */
      usage(argv, conf);
      break;

    case 'v': /* verbose */
      verbose++;
      break;

    case 'V': /* version */
      version(argv);
      exit(-1);
      break;

    case 'd':
      debug++;
      break;

    case 'p':
      conf->port = (unsigned short) atoi(optarg);
      break;

    case 'i':
      conf->input = optarg;
      break;

    case 'G':
      conf->gdb_dump = optarg;
      break;

    case 'P':
      conf->pid = atoi(optarg);
      break;

    case 's':
      conf->string_dump = optarg;
      break;

    default:
      usage(argv, conf);
      break;
    }
  }

  /* send the index to the first non-optional argument */
  return optind;
}

/*---------------------------------------------------------------------------*
 * NAME: init_configuration  
 * DESC: Initialize with the default values the configuration structure
 *---------------------------------------------------------------------------*/
void init_configuration(config *conf) {
  conf->gdb_exe = NULL;
  conf->master = 0;
  conf->slave  = 0;
  conf->bp = NULL;
  debug = 3;
  verbose = 0;
  conf->string_dump = 0;
  conf->f_string_dump = NULL;
  conf->port = 0;
  conf->pid = 0;
  conf->input = NULL;
  conf->f_input = NULL;
  conf->fuzzer_socket = -1;
  conf->program = NULL;
  conf->args = NULL;
  conf->string = NULL;
  conf->gdb_dump = NULL;
  conf->f_gdb_dump = NULL;
  conf->gdb_buf = NULL;

}


char *create_args(char **av)
{
    char *a = malloc_(1024);
    int  i;

    a[0] = '\0';
    for ( i = 0; av[i]; i++ )
    {
        strcat(a, av[i]);
        strcat(a, " ");
    }

    return a;
}

/*---------------------------------------------------------------------------*
 * NAME: main
 * DESC: Main Entry Point
 *---------------------------------------------------------------------------*/
int main(int argc, char **argv) {

  config *conf;
  unsigned int first_arg;
  unsigned int result;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* create the configuration structure */
  conf = malloc_(sizeof(config));

  /* initialize the configuration structure */
  init_configuration(conf);

  /* parse the arguments */
  first_arg = parsing_args(argc, argv, conf);

  /* parse the program */
  conf->program = argv[first_arg];
  if (conf->program) debug(2, "[program name]: %s\n", argv[first_arg]);

  /* parse the args (don't forget to free!) */
  conf->args = create_args(&argv[first_arg + 1]);
  if (conf->args) debug(2, "[arg string]: %s\n", conf->args);

  /* basic check */
  if (
      ((conf->port ==0) && (conf->input == NULL) && (conf->pid == 0) && (conf->string_dump == NULL)) || /* at least one thing  */
      ((conf->port != 0) && (conf->input != NULL))       || /* port or input */
      ((conf->port != 0) && (conf->pid != 0))            || /* port or pid */
      ((conf->input != NULL) && (conf->pid != 0))        || /* input or pid */
      ((conf->pid != 0) && (conf->program != NULL))      || /* no program with pid */
      ((conf->port != 0) && (conf->program == NULL))     || /* program with port */
      ((conf->input != NULL) && (conf->program == NULL))    /* program with input */
      ) 
    {
      usage(argv, conf);
    }

  /* start the debugger (gdb-based) core */
  result = gdb_core(conf);

  /* quit */
  this_is_the_end(conf);

  /* should never happen */
  return 0;
}
