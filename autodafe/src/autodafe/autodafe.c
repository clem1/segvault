/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : fuzzer.c
 * DESCRIPTION: main fuzzer engine. Read a .adc file and fuzz a server/client
 *              or file.
 * DEPENDANCES: 
 * COMPILATION: 
 *---------------------------------------------------------------------------*/


#define _GNU_SOURCE           /* getopt_long */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>           /* getopt_long */
#include <signal.h>           /* signal */

#include "debug.h"
#include "conf.h"
#include "file.h"
#include "engine.h"
#include "dbg.h"
#include "../../include/autodafe.h"

extern unsigned int debug;
extern unsigned int verbose;

/*---------------------------------------------------------------------------*
 * NAME: sigpipe_handler
 * DESC: do nothing when a sigpipe is detected because it's probably a
 *       connection closed by foreign host (yes it happens :))
 *---------------------------------------------------------------------------*/
static void sigpipe_handler(int signo) {

  /* do nothing */

}

/*---------------------------------------------------------------------------*
 * NAME: version
 * DESC: Print the version of the program
 *---------------------------------------------------------------------------*/
void version(char **argv) {
  fprintf(stdout, "%s-v.%d.%d ", argv[0], MAJOR_VERSION, MINOR_VERSION ); 
  fprintf(stdout, "(c) 2004-2006 "NAME" <"EMAIL">\n\n");
}

/*---------------------------------------------------------------------------*
 * NAME: usage
 * DESC: Print the usage of the program
 *---------------------------------------------------------------------------*/
void usage(char **argv, config *conf) {
  fprintf(stderr, "%s-v.%d.%d - This is the Autodafe Fuzzer Engine\n", argv[0], MAJOR_VERSION, MINOR_VERSION);
  version(argv);
  fprintf(stderr, "Usage: %s [OPTION...] <file.adc>\n\n", argv[0]);
  fprintf(stderr, "  List of the basic functions:\n");
  fprintf(stderr, "    -h, --help          display this help and exit.\n");
  fprintf(stderr, "    -V, --version       output version information and exit.\n");
  fprintf(stderr, "    -v, --verbose       verbose output. Use twice for more verbose.\n");
  fprintf(stderr, "    -d  --debug         print debug info. Use 2x or 3x for more debug info.\n\n");
  fprintf(stderr, "  List of the autodafe's functions:\n");
  fprintf(stderr, "    -t  --timeout=SEC   if no response, wait SEC seconds before go on.\n");
  fprintf(stderr, "    -u  --udp           use an udp connection - default is tcp.\n");
  fprintf(stderr, "    -f  --file=DIR      create multiple fuzzed files in directory DIR.\n");
  fprintf(stderr, "    -r  --remote=HOST   mode *client*: HOST = hostname/ip address. \n");
  fprintf(stderr, "    -p  --port=PORT     mode *client*: PORT = port to connect to.\n");
  fprintf(stderr, "                        mode *server*: PORT = listening port [-r not defined].\n");
  fprintf(stderr, "    -D  --with-dbg=HOST debugger: HOST = hostname/ip of the debugger.\n");
  fprintf(stderr, "    -P  --dbg-port=PORT debugger: PORT = listening port of the debugger.\n\n");
  fprintf(stderr, "  List of the internal's autodafe's functions:\n");
  fprintf(stderr, "    -b  --bruteforce    fuzz every fields.\n");
  fprintf(stderr, "    -i  --invert        send() becomes recv() and recv() becomes send().\n");
  fprintf(stderr, "    -w  --wait=SEC      wait SEC seconds before opening connection.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "<file.adc> is a type of file used by the fuzzer.\n");
  fprintf(stderr, "The standard output gives the results of the fuzzing.\n\n");

  /* free and exit */
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
    {"invert", 0, NULL, 'i'},
    {"remote", 1, NULL, 'r'},
    {"with-dbg", 1, NULL, 'D'},
    {"wait", 1, NULL, 'w'},
    {"dbg-port", 1, NULL, 'P'},
    {"port", 1, NULL, 'p'},
    {"file", 1, NULL, 'f'},
    {"udp", 0, NULL, 'u'},
    {"bruteforce", 0, NULL, 'b'},
    {"timeout", 1, NULL, 't'},
    {NULL, 0, NULL, 0}
  };
  int c;
  int option_index = 0;
  char *options = "Vvhdis:r:p:uf:D:P:w:bt:";

  if (argc < MIN_ARGS) {
    debug(3, "Not enough argument\n");
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

    case 'd': /* debug */
      debug++;
      break;

    case 'i': /* invert */
      conf->invert = 1;
      break;

      break;

    case 'r': /* client mode */
      conf->mode = 0; /* if defined -> client, otherwise -> server */
      conf->host = optarg;
      break;

    case 'p': /* client mode - port value */
      conf->port = (unsigned short) atoi(optarg);
      break;

    case 'u': /* udp */
      conf->type = 1;
      break;

    case 'f': /* file fuzzing */
      conf->type = 2;
      conf->mode = 2;
      conf->fuzz_file_dir = optarg;
      break;

    case 'D': /* Autodafe's debugger mode activated */
      conf->dbg_mode = 1;
      conf->dbg_host = optarg;
      break;

    case 'P': /* Autodafe's debugger mode activated */
      conf->dbg_port = (unsigned short) atoi(optarg);
      break;

    case 'w': /* wait time */
      conf->wait = atoi(optarg);
      break;

    case 'b': /* bruteforce */
      conf->bruteforce = 1;
      break;

    case 't': /* timeout */
      conf->timeout = atoi(optarg);
      break;


    default:
      usage(argv, conf);
      break;
    }
  }
  /* send the index to the first non-optional argument */
  debug(3,"optind value: %d\n", optind);
  return optind;
}

/*---------------------------------------------------------------------------* 
 * NAME: init_configuration  
 * DESC: Initialize with the default values the configuration structure
 *---------------------------------------------------------------------------*/
void init_configuration(config *conf) {
  conf->invert = 0;      /* default is send -> send */
  conf->type = 0;        /* default = tcp */
  conf->mode = 1;        /* if no HOST defined -> server mode */
  conf->dbg_mode = 0;    /* by default, no autodafe's debugger */
  conf->wait = 3;        /* default is to wait 3 seconds (debugger) */
  conf->bruteforce = 0;  /* default is NO bruteforce */
  conf->timeout = 0;     /* default is NO timeout */

  /* init */
  conf->ring_zero = 0; 
  conf->dbg_host = NULL;
  conf->dbg_port = 0;
  conf->fuzz_current = 0;
  conf->fuzz_file = NULL;
  conf->fuzz_file_dbg = NULL;
  conf->fuzz_file_id = 0;
  conf->fuzz_file_dir = NULL;
  conf->adc_file = NULL;
  conf->block = NULL;
  conf->length = NULL;
  conf->hash = NULL;
  conf->chrono = NULL;
  conf->fuzz = NULL;
  conf->buf_fuzz = NULL;
  conf->buf_fuzz_size = 0;
  conf->port = 0;
  conf->host = NULL;
  conf->socket = -1;
  conf->dbg_socket = -1;
}



/*---------------------------------------------------------------------------*
 * NAME: main
 * DESC: Main Entry Point
 *---------------------------------------------------------------------------*/
int main(int argc, char **argv) {

  config *conf;
  unsigned int first_arg;

  /* create the configuration structure */
  conf = malloc_(sizeof(config));

  /* create the adc structure */
  conf->adc = malloc_(sizeof(struct struct_adc));

  /* initialize the configuration structure */
  init_configuration(conf);

  /* parse the arguments */
  first_arg = parsing_args(argc, argv, conf);

  /* check if selected mode is well defined (dirty) */
  if (
      /* client mode -> port AND host */
      ((conf->mode == 0) && ((conf->port == 0) || 
			     (conf->host == NULL)))
      ||
      /* server mode -> port */
      ((conf->mode == 1) && (conf->port == 0))
      ||
      /* no file.ad defined */
      (argv[first_arg] == NULL) 
      ||
      /* debugger and file */
      ((conf->type == 2) && (conf->dbg_mode))
      ||
      /* fuzz udp or tcp or file */
      ((conf->fuzz_file_dir != NULL) && ((conf->host != NULL) ||
					 (conf->port != 0)    ||
					 (conf->type != 2)))
      ) {
    usage(argv, conf);
  }
  
  /* verbose messages */

  /* autodafe's debugger */
    if (conf->dbg_mode) {
    verbose_("[*] Autodafe's debugger mode activated.\n");
  }

  /* file mode */
  if (conf->type == 2) {
    verbose_("[*] mode *file* - all fuzzed files will be in %s\n", conf->fuzz_file_dir);
  }

  /* network mode */
  else {
    if (conf->mode) {
      if (!conf->type) /* tcp */
	verbose_("[*] mode *server* - listening on port: %d (tcp)\n", conf->port);
      else /* udp */
	verbose_("[*] mode *server* - listening on port: %d (udp)\n", conf->port);
    }
    else {
      if (!conf->type) /* tcp */
	verbose_("[*] mode *client* - connection to %s on port: %d (tcp)\n", conf->host, conf->port);
      else /* udp */
	verbose_("[*] mode *client* - connection to %s on port: %d (udp)\n", conf->host, conf->port);
    }
  }
  /* read the file */
  if(read_adc_file(conf, argv[first_arg])) goto fuzzer_end;

  /* ignore the SIGPIPE signal (ie. "Connection closed by foreign host") */
  signal(SIGPIPE, sigpipe_handler);


  /* start the connection with the debugger */
  if (conf->dbg_mode) 
    if (dbg_connection(conf)) goto fuzzer_end;

  /* start the fuzz engine */
  fuzz_engine(conf);


  if (conf->buf_fuzz)    free(conf->buf_fuzz);    /* free the fuzz buffer (in case of error) */
 fuzzer_end:
  if (conf->adc->buffer) free(conf->adc->buffer); /* free the memory-copy of adc file */
  if (conf->adc)         free(conf->adc);         /* free the adc structure */
  if (conf)              free(conf);              /* free the configuration structure */
  
  return 0;
}
