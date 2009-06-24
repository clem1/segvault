/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Aug.2004 ---*                                          
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : output.c                                                                                                 
 * DESCRIPTION: all the functions related to user-friendly outputs      
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>  /* malloc */

#include "debug.h"
#include "conf.h"
#include "../../include/autodafe.h"

/*---------------------------------------------------------------------------*                                            
 * NAME: _output_packet_block()
 * DESC: print the content of a packet using stdout
 *---------------------------------------------------------------------------*/
void _output_packet_block(config *conf, struct struct_block *block) {
  int i, j;

  /* hexadecimal value */
  for(i=0;i<block->size;i+=16) {
    verbose__(" ");
    for(j=0;j<16;j++) {
      if (j + i < block->size) {
	if (j == 8) verbose__("  ");
	verbose__("%02x ", *(conf->buf_fuzz + block->offset + j + i));
      }
      else {
	if (j == 8) verbose__("  ");
	verbose__("   ");
      }
    }

    /* ASCII value */
    for(j=0;j<16;j++) {
      if (j + i < block->size) {
	if ((*(conf->buf_fuzz + block->offset + j + i) < 0x20) ||
	    (*(conf->buf_fuzz + block->offset + j + i) > 0x7e))
	  verbose__(".");
	else
	  verbose__("%c", *(conf->buf_fuzz + block->offset + j + i));
      }
      else
	verbose__(" ");
    }
    verbose__("\n");
  }
}


/*---------------------------------------------------------------------------*                                            
 * NAME: _output_recv_packet_compare()
 * DESC: print the content of a packet using stdout
 *---------------------------------------------------------------------------*/
void _output_recv_packet_compare(config *conf, struct struct_block *block, unsigned char *buffer, unsigned int bytes) {
  int i, j;
  unsigned int same = 0;

  /* now with the received packet AND the orig packet */
  verbose__(MAGENTA);
  verbose__("+------------[recv buffer (size: %05d/%05d)]----+----------------+\n", bytes, block->size);

  for(i=0;i<block->size;i+=16) {
    verbose__(" ");

    /* print hexadecimal values */
    for(j=0;j<16;j++) {
      if (j + i < block->size) {
	if (j + i < bytes) {
	  if (j == 8) verbose__("  ");
	  if (*(conf->buf_fuzz + block->offset + j + i) != *(buffer + j + i))
	    verbose__(RED);
	  else
	    same++;
	  verbose__("%02x ", *(buffer  + j + i));
	  verbose__(MAGENTA);
	}
	else {
	  verbose__(WHITE);
	  if (j == 8) verbose__("  ");
	  verbose__("%02x ", *(conf->buf_fuzz + block->offset  + j + i));
	  verbose__(MAGENTA);
	}
      }
      else {
	if (j == 8) verbose__("  ");
	verbose__("   ");
      }
    }
    /* print ASCII values */
    for(j=0;j<16;j++) {
      if (j + i < block->size) {
	if (j + i < bytes) {
	  if (*(conf->buf_fuzz + block->offset + j + i) != *(buffer +j + i))
	    verbose__(RED);
	  if ((*(buffer + j + i) < 0x20) ||
	      (*(buffer + j + i) > 0x7e))
	    verbose__(".");
	  else
	    verbose__("%c", *(buffer + j + i));
	  verbose__(MAGENTA);
	}
	else {
	  verbose__(WHITE);
	  if ((*(conf->buf_fuzz + block->offset + j + i) < 0x20) ||
	      (*(conf->buf_fuzz + block->offset + j + i) > 0x7f))
	    verbose__(".");
	  else
	    verbose__("%c", *(conf->buf_fuzz + block->offset + i + j));
	  verbose__(MAGENTA);
	}
      }
      else
	verbose__(" ");
    }
    verbose__("\n");
  }
  verbose__("+------------[recv buffer (size: %05d/%05d)]----+----------------+", bytes, block->size);
  verbose__(WHITE);
  verbose__("\n");
  verbose_("[*] received packet is similar to block at: %d/%d -> %.1f%%\n",
	   same, block->size, (float)((float) same/( float) block->size)*100);
  verbose__("\n");
}

/*---------------------------------------------------------------------------*                                            
 * NAME: _output_send_packet()
 * DESC: print the content of a packet using stdout (send)
 *---------------------------------------------------------------------------*/
void _output_send_packet(config *conf, struct struct_block *block) {

  verbose__(CYAN);
  verbose__("+------------[send buffer (size: %05d)]----------+----------------+\n", block->size);

  _output_packet_block(conf, block);

  verbose__("+------------[send buffer (size: %05d)]----------+----------------+", block->size);
  verbose__(WHITE);
  verbose__("\n\n");
}


/*---------------------------------------------------------------------------*                                            
 * NAME: _output_send_packet_red()
 * DESC: print in red the content of a packet using stdout (send)
 *---------------------------------------------------------------------------*/
void _output_send_packet_red(config *conf, struct struct_block *block) {

  verbose__(RED);
  verbose__("+------------[send buffer (size: %05d)]----------+----------------+\n", block->size);

  _output_packet_block(conf, block);

  verbose__("+------------[send buffer (size: %05d)]----------+----------------+", block->size);
  verbose__(WHITE);
  verbose__("\n\n");
}



/*---------------------------------------------------------------------------*                                            
 * NAME: _output_recv_packet()
 * DESC: print the content of a packet using stdout (send)
 *---------------------------------------------------------------------------*/
void _output_recv_packet(config *conf, struct struct_block *block) {

  verbose__(MAGENTA);
  verbose__("+------------[orig buffer (size: %05d)]----------+----------------+\n", block->size);

  _output_packet_block(conf, block);

  verbose__("+------------[orig buffer (size: %05d)]----------+----------------+", block->size);
  verbose__(WHITE);
  verbose__("\n\n");
}
