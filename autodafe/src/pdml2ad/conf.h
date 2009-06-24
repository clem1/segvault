/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : conf.h
 * DESCRIPTION: All the configuration values
 *---------------------------------------------------------------------------*/

#define MIN_ARGS 2              // number of min args before print usage 
#define MAX_OUTPUT_SIZE 2*65535 // max size of a outputed string using output.c
#define MAX_LENGTH_STRING 70    // the max size of a string (user-friendly)

#include <libxml/xmlmemory.h> // libxml2 */
#include <libxml/parser.h>    // libxml2 */


typedef struct configuration {
  char * xml_filename;              // pointer on the XML-PDML file
  xmlDocPtr doc;                    // pointer to a structure containing the tree
  xmlNodePtr cur;                   // pointer to a structure containing a single node
  unsigned int packet_counter;      // the number of packet in the sniffed communication
  unsigned int proto_counter;       // the number of protocols in a packet
  unsigned int transport_type;      // undefined = 0; tcp = 1; udp = 2;
  int invert;                       // first packet > send = 0; second packet > send = 1
  unsigned int ip_client;           // IP address of the client
  unsigned short port_client;       // port of the client
  unsigned int ip_server;           // IP address of the server
  unsigned short port_server;       // port of the server
  unsigned int ip_pkt;              // IP address of the current packet 
  unsigned short port_pkt;          // port of the current packet
  unsigned int send;                // if = 1 we send the current packet, otherwise, receive
  unsigned int tab;                 // just to be user-friendly, num of tab in output_tab_ 
  unsigned int ethereal_hex_style;  // to have the hexa values in ethereal-like
  unsigned int check_proto;         // if = 1 we try to recover the protocol using the ethereal engine
  unsigned int check_string;        // if = 1 we check if hex value can be a string and if
                                    // yes, we convert the hex value in a string function
  unsigned int check_length;        // try to discover the length fields (probabilistic)
  char *output_name;                // name of the output file
} config;

unsigned int verbose; /* shared value for debug.c */
unsigned int debug;   /* shared value for debug.c */

