/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Jun.2004 ---*                                          
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : opcode.c
 * DESCRIPTION: All the instruction's functions
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>      /* strlen */
#include <stdlib.h>      /* free */

#include "debug.h"
#include "conf.h"
#include "engine.h"
#include "dbg.h"
#include "proto.h"
#include "../../include/autodafe.h"



/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_hex
 * DESC: what to do when we have a hf_hex function
 *---------------------------------------------------------------------------*/
void opcode_hf_hex(config *conf) {

  unsigned int int_value;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the hex size */
  int_value = read_buffer_int(conf);

  /* write the values on the packet */
  push_data(conf, conf->adc->buffer + conf->adc->offset, int_value);

  /* move the offset */
  conf->adc->offset += int_value;

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}


/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_fuzz_hex
 * DESC: what to do when we have a hf_fuzz_hex function
 *---------------------------------------------------------------------------*/
void opcode_hf_fuzz_hex(config *conf) {

  unsigned int int_value;
  unsigned char char_value;
  unsigned char *filename = NULL;
  struct struct_fuzz *fuzz;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the fuzz_id */
  int_value = read_buffer_int(conf);

  /* read the size of the filename containg the fuzz source */
  char_value = read_buffer_char(conf);

  /* copy the string in a local buffer */
  filename = malloc_((unsigned int) char_value + 1); 
  memcpy(filename, conf->adc->buffer + conf->adc->offset, (unsigned int) char_value);
  filename[(unsigned int) char_value] = '\0';

  debug(3, "index file: \"%s\"\n", filename);

  /* move the adc offset */
  conf->adc->offset += (unsigned int) char_value;

  /* use the fuzz core - 1 is the default weight */
  fuzz = fuzz_core(conf, int_value, filename, 1);

  /* free filename */
  free(filename);

  /* we don't fuzz */
  if (!fuzz) {

    /* normal parsing */
    opcode_hf_hex(conf);

    /* debug */
    debug(1, "<-----------------------[quit]\n");

    return;
  }
  
  /* read the hex size */
  int_value = read_buffer_int(conf);
  
  /* we update the offset */
  conf->adc->offset += int_value;

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}


/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_string
 * DESC: what to do when we have a hf_string function (if id!=-1 -> fuzz)
 *---------------------------------------------------------------------------*/
void opcode_hf_string(config *conf, unsigned int id) {

  unsigned char char_value;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the size of the string */
  char_value = read_buffer_char(conf);

  /* copy the string in the packet */
  push_data(conf, conf->adc->buffer + conf->adc->offset, (unsigned int) char_value);

  /* we are in file mode i.e. we fuzz file, so we copy every string in the file
   * 00000000.dbg (only on ring_zero and only for fuzz_string) */
  if ((conf->type == 2) && (!conf->ring_zero) && (id != -1)) {

    unsigned int bytes;

    /* the string */
    bytes = fwrite(conf->adc->buffer + conf->adc->offset, sizeof(char), (unsigned int) char_value, conf->fuzz_file_dbg);
    if (bytes != (unsigned int) char_value) {
      error_("cannot write on the debugger string file!\n");
      error_("QUITTING!\n");
      exit(-1);
    }

    /* add a '\n' */
    bytes = fwrite("\n", sizeof(char), 1, conf->fuzz_file_dbg);
    if (bytes <= 0) {
      error_("cannot write on the debugger string file!\n");
      error_("QUITTING!\n");
      exit(-1);
    }
  }

  /* if we are in debug_mode and ring_zero (i.e. first try) we send the string to
   * the debugger. So it can use it to verify if the string pass through a vulnerable
   * function. */
  if ((conf->dbg_mode) && (!conf->ring_zero) && (id != -1)) {
    
    /* send the string to the debugger */
    if (dbg_send_msg(conf, INET_STRING_A_MSG, id, conf->adc->buffer + conf->adc->offset, (unsigned int) char_value)) {
      error_("cannot send the message: INET_STRING_A_MSG to the debugger!\n");
      error_("QUITTING!\n");

      /* hard quit */
      exit(-1);
    }
  }

  /* move the offset */
  conf->adc->offset += (unsigned int) char_value;

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}


/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_fuzz_string
 * DESC: what to do when we have a hf_fuzz_string function
 *---------------------------------------------------------------------------*/
void opcode_hf_fuzz_string(config *conf) {

  unsigned int int_value;
  unsigned char char_value;
  unsigned char *filename = NULL;
  struct struct_fuzz *fuzz;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the fuzz_id */
  int_value = read_buffer_int(conf);

  /* read the size of the filename containing the fuzz data */
  char_value = read_buffer_char(conf);

  /* copy the string in a local buffer */
  filename = malloc_((unsigned int) char_value + 1);
  memcpy(filename, conf->adc->buffer + conf->adc->offset, (unsigned int) char_value);
  filename[(unsigned int) char_value] = '\0';

  debug(3, "index file: \"%s\"\n", filename);

  /* move the adc offset */
  conf->adc->offset += (unsigned int) char_value;

  /* use the fuzz core - 0 is the default weight */
  fuzz = fuzz_core(conf, int_value, filename, 0);

  /* free filename */
  free(filename);

  /* we don't fuzz */
  if (!fuzz) {
    opcode_hf_string(conf, int_value); 

    /* debug */
    debug(1, "<-----------------------[quit]\n");

    return;
  }

  /* read the string size */
  char_value = read_buffer_char(conf);

  /* update the offset */
  conf->adc->offset += char_value;

  /* debug */
  debug(1, "<-----------------------[quit]\n");
}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_begin
 * DESC: what to do when we have a hf_block_begin function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_begin(config *conf) {

  unsigned int int_value;
  struct struct_block *block;
  struct struct_block *prev = NULL;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  debug(2, "opcode \""AD_F_BLOCK_BEGIN"\" found.\n");

  /* read the int value */
  int_value = read_buffer_int(conf);
  
  /* go to the last block */
  for(block = conf->block;block;block = block->next) {
    debug(3, "current block[%d]\n", block->id);
    prev = block; /* save the previous */
  }

  /* create a block entry in the linked list */
  block = malloc_(sizeof(struct struct_block));

  /* update the previous block's field next */
  if (prev) 
    prev->next = block;

  /* fill the other fields */
  block->id = int_value;            /* put the value */
  block->size = 0;                  /* init the size */
  block->offset = conf->buf_fuzz_size;  /* current size of the fuzz_buffer */
  block->state = 0;                 /* block opened */
  block->next = NULL;               /* init the next pointer */

  /* update if it's the first block */
  if (!conf->block)
    conf->block = block;

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_end
 * DESC: what to do when we have a hf_block_end function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_end(config *conf) {

  unsigned int int_value;
  struct struct_block *block;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* look which block has this id */
  for(block = conf->block;block;block = block->next)
    if (block->id == int_value) {
      block->state = 1;
      debug(2, "closing block[%d] with fixed size: %d\n", block->id, block->size);
      debug(3, "dump of the block:\n");
      for(int_value=0;int_value<block->size;int_value++)
	debug(3,"0x%02x (%c)\n", (unsigned char) conf->buf_fuzz[block->offset + int_value], 
	      (unsigned char) conf->buf_fuzz[block->offset + int_value]);
      break; /* time optimisation 8^P */
    }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_l_32
 * DESC: what to do when we have a hf_block_size_l_32 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_l_32(config *conf) {
  
  unsigned int int_value;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* add the length in the linked list and alloc space in the fuzz buffer */
  push_length(conf, int_value, HF_BLOCK_SIZE_L_32);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_l_16
 * DESC: what to do when we have a hf_block_size_l_16 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_l_16(config *conf) {

  unsigned int int_value;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* add the length in the linked list and alloc space in the fuzz buffer */
  push_length(conf, int_value, HF_BLOCK_SIZE_L_16);


  /* debug */
  debug(1, "<-----------------------[quit]\n");


}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_b_32
 * DESC: what to do when we have a hf_block_size_b_32 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_b_32(config *conf) {

  unsigned int int_value;


  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* add the length in the linked list and alloc space in the fuzz buffer */
  push_length(conf, int_value, HF_BLOCK_SIZE_B_32);

  /* debug */
  debug(1, "<-----------------------[quit]\n");


}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_b_16
 * DESC: what to do when we have a hf_block_size_b_16 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_b_16(config *conf) {

  unsigned int int_value;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* add the length in the linked list and alloc space in the fuzz buffer */
  push_length(conf, int_value, HF_BLOCK_SIZE_B_16);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_block_size_8
 * DESC: what to do when we have a hf_block_size_8 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_8(config *conf) {

  unsigned int int_value;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* add the length in the linked list and alloc space in the fuzz buffer */
  push_length(conf, int_value, HF_BLOCK_SIZE_8);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_s_16
 * DESC: what to do when we have a hf_block_size_s_16 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_s_16(config *conf) {

  unsigned int int_value;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* add the length in the linked list and alloc space in the fuzz buffer */
  push_length(conf, int_value, HF_BLOCK_SIZE_S_16);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_s_10
 * DESC: what to do when we have a hf_block_size_s_10 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_s_10(config *conf) {

  unsigned int int_value;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* add the length in the linked list and alloc space in the fuzz buffer */
  push_length(conf, int_value, HF_BLOCK_SIZE_S_10);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_crc32_b
 * DESC: what to do when we have a hf_block_crc32_b function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_crc32_b(config *conf) {

  unsigned int int_value;


  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* add the length in the linked list and alloc space in the fuzz buffer */
  push_hash(conf, int_value, HF_BLOCK_CRC32_B);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_crc32_l
 * DESC: what to do when we have a hf_block_crc32_l function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_crc32_l(config *conf) {

  unsigned int int_value;


  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* add the length in the linked list and alloc space in the fuzz buffer */
  push_hash(conf, int_value, HF_BLOCK_CRC32_L);

  /* debug */
  debug(1, "<-----------------------[quit]\n");


}




/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_send_
 * DESC: what to do when we have a hf_send function
 *---------------------------------------------------------------------------*/
void opcode_hf_send(config *conf) {

  unsigned int int_value;
  struct struct_chrono *chrono;
  struct struct_chrono *prev=NULL;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* go to last chrono */
  for(chrono = conf->chrono;chrono;chrono = chrono->next) {
    debug(3, "current chrono (connected with block[%d])\n", chrono->id);
    prev = chrono; /* save the previous */
  }
  debug(2, "create new chrono structure\n");
  /* create a chrono entry in the linked list */
  chrono = malloc_(sizeof(struct struct_chrono));

  /* update the previous block's field next */
  if (prev)
    prev->next = chrono;

  /* fill the other fields */
  chrono->id = int_value;
  chrono->type = 0;  /* send */
  chrono->next = NULL;

  /* update if it's the first chrono */
  if (!conf->chrono)
    conf->chrono = chrono;

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_recv
 * DESC: what to do when we have a hf_recv function
 *---------------------------------------------------------------------------*/
void opcode_hf_recv(config *conf) {

  unsigned int int_value;
  struct struct_chrono *chrono;
  struct struct_chrono *prev=NULL;

  /* debug */
  debug(1, "<-----------------------[enter]\n");  

  /* read the int value */
  int_value = read_buffer_int(conf);

  /* go to last chrono */
  for(chrono = conf->chrono;chrono;chrono = chrono->next) {
    debug(3, "current chrono (connected with block[%d])\n", chrono->id);
    prev = chrono; /* save the previous */
  }

  debug(2, "create new chrono structure\n");
  /* create a chrono entry in the linked list */
  chrono = malloc_(sizeof(struct struct_chrono));

  /* update the previous block's field next */
  if (prev)
    prev->next = chrono;

  /* fill the other fields */
  chrono->id = int_value;
  chrono->type = 1;  /* recv */
  chrono->next = NULL;

  /* update if it's the first chrono */
  if (!conf->chrono)
    conf->chrono = chrono;

  /* debug */
  debug(1, "<-----------------------[quit]\n");  

}
