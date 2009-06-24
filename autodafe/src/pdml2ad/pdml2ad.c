/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : xml.c
 * DESCRIPTION: Convert a XML-PDML (Ethereal) file to Autodafe's script.
 * DEPENDANCES: libxml2 library - http://www.xmlsoft.org
 * COMPILATION: gcc -L/usr/include/libxml2 -lxml2 -o file file.c
 *---------------------------------------------------------------------------*/


#define _GNU_SOURCE           // getopt_long

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>           // getopt_long

#include "debug.h"
#include "conf.h"
#include "xml.h"
#include "output.h"
#include "../../include/autodafe.h"

extern unsigned int debug;
extern unsigned int verbose;
extern FILE *output_desc;

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

  fprintf(stderr, "%s-v.%d.%d - convert PDML file to Autodafe's script language\n", argv[0], MAJOR_VERSION, MINOR_VERSION);
  version(argv);
  fprintf(stderr, "Usage: %s [OPTION...] <FILE.pdml> <FILE.ad>\n", argv[0]);
  fprintf(stderr, "    -h, --help                display this help and exit.\n");
  fprintf(stderr, "    -V, --version             output version information and exit.\n");
  fprintf(stderr, "    -e, --no-ethereal-style   not using the ethereal hexadecimal style.\n");
  fprintf(stderr, "    -p  --recover-protocol    try to recover the used protocol.\n");
  fprintf(stderr, "    -s, --recover-strings     try to recover strings in packet.\n");
  fprintf(stderr, "    -i, --invert-send-recv    some servers speak first, some clients speak first.\n");
  fprintf(stderr, "    -v, --verbose             verbose output.\n");
  fprintf(stderr, "    -d  --debug               use 2x or 3x for more debug info.\n");
  fprintf(stderr, "\n\n");
  fprintf(stderr, "FILE is a PDML-XML file (Packet Details Markup Language) generally exported\n");
  fprintf(stderr, "by Ethereal.\n");
  fprintf(stderr, "A file <FILE.AD> is created.\n\n");

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
    {"ethereal-style", 0, NULL, 'e'},
    {"recover-strings", 0, NULL, 's'},
    {"recover-protocol", 0, NULL, 'p'},
    {"invert-send-recv", 0, NULL, 'i'},
    {NULL, 0, NULL, 0}
  };
  int c;
  int option_index = 0;
  char *options = "Vvhdesip";

  if (argc < MIN_ARGS) {
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
      verbose = 1;
      break;

    case 'V': /* version */
      version(argv);
      exit(-1);
      break;

    case 'd':
      debug++;
      break;

    case 'e': /* ethereal hex-style */
      conf->ethereal_hex_style = 0;
      break;

    case 's': /* recover strings mode */
      conf->check_string = 1;
      break;

    case 'p': /* recover strings mode */
      conf->check_proto = 1;
      break;

    case 'i': /* invert mode ie: 1st pkt = recv */
      conf->invert = 1;     
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
  conf->xml_filename = NULL;
  conf->packet_counter = 1;
  conf->proto_counter = 1;
  conf->send = 0;
  conf->ip_client = 0;
  conf->port_client = 0;
  conf->port_server = 0;
  conf->ip_server = 0;
  conf->ip_pkt = 0;
  conf->port_pkt = 0;
  conf->tab = 0;
  output_desc = NULL;      
  conf->transport_type = 0;     /* default = undefined */
  conf->invert = 0;             /* default = 1st packet = send */
  conf->ethereal_hex_style = 1; /* default = yes (easier to read) */
  conf->check_string = 0;       /* default = no */
  conf->check_length = 30;      /* default = 30%     (not 100% sure) */
  conf->check_proto = 0;        /* default = no recovering (not 100% sure) */

  debug = 0;                    /* default = no debug informations */
  verbose = 0;                  /* default = no verbose */

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

  /* init the configuration structure */
  init_configuration(conf);

  /* parse the arguments */
  first_arg = parsing_args(argc, argv, conf);
  conf->xml_filename = argv[first_arg];

  /* copy the name of the .ad file */
  conf->output_name = argv[first_arg+1];

  /* basic check */
  if ((conf->output_name == NULL) || (conf->xml_filename == NULL)) {
    usage(argv, conf);
  }

  /* open the output_name file */
  output_desc = fopen(conf->output_name, "w");
  if (!output_desc) {
    error_("cannot write the file: \"%s\": ", conf->output_name);
    perror("");
    error_("QUITTING!\n");
    free(conf);
    exit(-1);
  }
  

  /* xml parsing of the file */
  xml_parsing(conf);

  /* free the configuration structure */
  free(conf);
  
  return 0;
}
