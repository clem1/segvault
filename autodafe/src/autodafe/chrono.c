/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Jul.2004 ---*                                          
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : chrono.c                                                     
 * DESCRIPTION: all the functions used to parse the chrono structure and try
 *              to detect if there is something relevant. Why chrono ?
 *              because everything is dependant of the time...
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>     /* malloc */
#include <string.h>     /* memset */

#include "debug.h"
#include "conf.h"
#include "network.h"
#include "output.h"

/*---------------------------------------------------------------------------*                                            
 * NAME: parse_chrono
 * DESC: parse the content of the chrono structure (what send or recv)
 * RETN: 0 if everything is ok
 *       1 if sendto detects a Broken pipe (connection closed by foreign host)
 *       2 if recv_from receive 0 data     (connection closed by foreign host)
 *      -1 if error
 *---------------------------------------------------------------------------*/
unsigned int parse_chrono(config *conf) {
  struct struct_chrono *chrono;
  struct struct_block *block;
  unsigned char *buffer;
  int bytes;
  unsigned int type;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the chrono structure */
  for(chrono = conf->chrono;chrono;chrono = chrono->next) {
    
    /* find the corresponding block */
    for(block = conf->block;block;block = block->next)
      if (chrono->id == block->id)
        break;
    
    debug(3, "block[%d] is linked to this chrono[%d]\n", block->id, chrono->id);
    
    /* if we invert */
    type = chrono->type;
    type = (chrono->type + conf->invert) % 2;
    debug(3, "chrono[%d] type is %d\n", chrono->id, type);

    switch(type) {

      /* SEND */
    case 0: 
      if (block->size > 0) {

	/* generic send buffer */
	bytes = send_fuzz(conf, block);
	if (bytes == -2) {

	  /* debug */
	  debug(1, "<-----------------------[quit]\n");

	  return 1;

	}
	else if (bytes < 0) {

	  /* print sended buffer */
	  _output_send_packet_red(conf, block);
	  return bytes;
	}	

	/* print sended buffer */
	_output_send_packet(conf, block);
      }
      break;

      /* RECV */
    case 1:
      /* if we fuzz file (conf->type == 2) do nothing */
      if ((block->size > 0) && (conf->type != 2)) {
        buffer = malloc_(block->size);
	memset(buffer, 0x0, block->size);

	/* print the original packet */
	_output_recv_packet(conf, block);

	/* generic receive buffer */
        bytes = recv_fuzz(conf, block, buffer);
	if (bytes == 0) {

	  /* debug */
	  debug(1, "<-----------------------[quit]\n");

	    return 2;
	  }
	if (bytes < 0) {
	  return bytes;
	}

	/* print compared received and original packet */
	_output_recv_packet_compare(conf, block, buffer, bytes);

	/* secure freeing this buffer (thanks to Valgrind!!!) */
	if (buffer) {
	  free(buffer);
	  buffer = NULL;
	}
      }
      break;
      
      /* should never happen */
    default:
      error_("unknown type of the chrono structure!\n");
      error_("QUITTING!\n");
      shutdown_socket(conf);
      return -1;
    }
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");
  
  return 0;
}
