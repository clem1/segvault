/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Jun.2004 ---*                                          
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : engine.c
 * DESCRIPTION: fetch and parse the adc file
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>     /* malloc */
#include <string.h>     /* memcpy */
#include <netinet/in.h> /* ntohl  */

#include "debug.h"
#include "conf.h"
#include "opcode.h"
#include "network.h"
#include "file.h"
#include "dbg.h"
#include "proto.h"
#include "hash.h"
#include "../../include/autodafe.h"

/*---------------------------------------------------------------------------*                                            
 * NAME: update_block_size
 * DESC: update each size of the opened blocks
 *---------------------------------------------------------------------------*/
void update_block_size(config *conf, unsigned int size) {

  struct struct_block *block;

  /* debug */
  debug(1, "<-----------------------[enter]\n");
  
  for(block=conf->block;block;block=block->next) {
    debug(3, "block[%d] checked if opened.\n", block->id);

    /* opened block */
    if (block->state == 0) {
      debug(2, "block[%d] opened. Modify size: %d + %d -> %d\n", 
	    block->id, block->size, size, block->size + size);
      block->size += size;
    }
    else 
      debug(3, "block[%d] closed. size: %d\n", block->id, block->size);
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}



/*---------------------------------------------------------------------------*                                            
 * NAME: push_data(config *conf, unsigned char *ptr, unsigned int size)
 * DESC: push data to the pkt buffer using realloc
 *---------------------------------------------------------------------------*/
void push_data(config *conf, unsigned char *ptr, unsigned int size) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* realloc the buffer */
  conf->buf_fuzz = (unsigned char *) realloc_(conf->buf_fuzz, conf->buf_fuzz_size + size);

  /* copy the data */
  memcpy(conf->buf_fuzz + conf->buf_fuzz_size, ptr, size);

  /* update the size */
  conf->buf_fuzz_size+=size;

  /* update the size of the blocks */
  update_block_size(conf, size);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}


/*---------------------------------------------------------------------------*                                            
 * NAME: modify_data
 * DESC: modify_data using offset in fuzz buffer
 *---------------------------------------------------------------------------*/
void modify_data(config *conf, unsigned char *ptr, unsigned int size, unsigned int offset) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* copy the data */
  memcpy(conf->buf_fuzz + offset, ptr, size);

  /* debug */
  debug(1, "<-----------------------[quit]\n");
}


/*---------------------------------------------------------------------------*                                            
 * NAME: push_length
 * DESC: add a length in the linked list length structure
 *---------------------------------------------------------------------------*/
void push_length(config *conf, unsigned int id, unsigned int type) {

  struct struct_length *length;
  struct struct_length *prev = NULL;

  /* dummy length values */
  unsigned int dummy_int     = 0xffffffff;
  unsigned short dummy_short = 0xffff;
  unsigned char dummy_char   = 0xff;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  debug(3," entering push_length\n");

  /* go to the last length */
  for(length = conf->length;length;length = length->next) {
    debug(3, "current struct length of block[%d]\n", length->id);
    prev = length; /* save the preview */
  }

  debug(3," add length structure\n");

  /* create a length entry in the linked list */
  length = malloc_(sizeof(struct struct_length));
  
  /* update the previous block's field next */
  if (prev)
    prev->next = length;

  /* fill the other fields */
  length->id = id;                  /* used next to find the struct_block */
  length->block = NULL;             /* init the struct_block pointer */
  length->offset = conf->buf_fuzz_size; /* current size of the buffer */
  length->next = NULL;              /* init the next pointer */
  length->type = type;              /* put the correct length type */

  /* update if it's the first length */
  if (!conf->length)
    conf->length = length;

  /* add the correct number of byte in the fuzz buffer */
  switch(type) {
  case HF_BLOCK_SIZE_B_32 : /* big-endian 32bits */
    push_data(conf, (unsigned char *) &dummy_int, sizeof(int)); 
    break;
  case HF_BLOCK_SIZE_L_32 : /* little-endian 32bits */
    push_data(conf, (unsigned char *) &dummy_int, sizeof(int)); 
    break; 
  case HF_BLOCK_SIZE_B_16 : /* big-endian 16bits */
    push_data(conf, (unsigned char *) &dummy_short, sizeof(short)); 
    break; 
  case HF_BLOCK_SIZE_L_16 : /* little-endian 16bits */
    push_data(conf, (unsigned char *) &dummy_short, sizeof(short)); 
    break; 
  case HF_BLOCK_SIZE_8 :    /* 8bits */
    push_data(conf, &dummy_char, sizeof(char)); 
    break; 
  case HF_BLOCK_SIZE_S_16 : /* string in hexadecimal */
    /* don't add bytes in buffer now. See after */ 
    break;
  case HF_BLOCK_SIZE_S_10 : /* string in decimal */
    /* don't add bytes in buffer now. See after */ 
    break;
  default: /* should never happen */ 
    break;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}


/*---------------------------------------------------------------------------*                                            
 * NAME: push_hash
 * DESC: add a hash in the linked list hash structure
 *---------------------------------------------------------------------------*/
void push_hash(config *conf, unsigned int id, unsigned int type) {

  struct struct_hash *hash;
  struct struct_hash *prev = NULL;

  /* dummy hash values equal to zero for the hash computation */
  unsigned int dummy_int = 0x00000000;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* go to the last hash */
  for(hash = conf->hash;hash;hash = hash->next) {
    debug(3, "current struct hash of block[%d]\n", hash->id);
    prev = hash; /* save the preview */
  }

  debug(3," add hash structure\n");

  /* create a hash entry in the linked list */
  hash = malloc_(sizeof(struct struct_hash));
  
  /* update the previous block's field next */
  if (prev)
    prev->next = hash;

  /* fill the other fields */
  hash->id = id;                  /* used next to find the struct_block */
  hash->block = NULL;             /* init the struct_block pointer */
  hash->offset = conf->buf_fuzz_size; /* current size of the buffer */
  hash->next = NULL;              /* init the next pointer */
  hash->type = type;              /* put the correct hash type */

  /* update if it's the first hash */
  if (!conf->hash)
    conf->hash = hash;

  /* add the correct number of byte in the fuzz buffer */
  switch(type) {
  case HF_BLOCK_CRC32_B : /* big-endian 32 bits */
    push_data(conf, (unsigned char *) &dummy_int, sizeof(int)); 
    break;
  case HF_BLOCK_CRC32_L : /* little-endian 32 bits */
    push_data(conf, (unsigned char *) &dummy_int, sizeof(int)); 
    break;

  default: /* should never happen */ 
    break;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}


/*---------------------------------------------------------------------------*                                            
 * NAME: read_buffer_char
 * DESC: give one char in the buffer using the offset
 *---------------------------------------------------------------------------*/
unsigned char read_buffer_char(config *conf) {

  unsigned char *buffer;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* buffer local value */
  buffer = conf->adc->buffer;

  /* update offset */
  conf->adc->offset = conf->adc->offset + 1;

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return value */
  return buffer[conf->adc->offset - 1];
}


/*---------------------------------------------------------------------------*                                            
 * NAME: read_buffer_int
 * DESC: give one int in the buffer using the offset in local-endian format
 *       because, mainly used for the computation of the size -> compared 
 *       with an id which is in the local-endian type too. (ntohl used)
 *---------------------------------------------------------------------------*/
unsigned int read_buffer_int(config *conf) {

  unsigned char *buffer;
  unsigned int result = 0;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* buffer local value */
  buffer = conf->adc->buffer;

  /* compute 4xchar -> int */
  result += buffer[conf->adc->offset];
  result += buffer[conf->adc->offset + 1] << 8;
  result += buffer[conf->adc->offset + 2] << 16;
  result += buffer[conf->adc->offset + 3] << 24;

  
  debug(3, "reading ntohl(int): 0x%x at addr: %p\n", ntohl(result), conf->adc->offset);

  /* update offset */
  conf->adc->offset = conf->adc->offset + sizeof(int);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return value */
  return ntohl(result);
}



/*---------------------------------------------------------------------------*                                            
 * NAME: update_length
 * DESC: update each length value, even string length (tricky!)
 *---------------------------------------------------------------------------*/
void update_length(config *conf) {
  
  struct struct_length *length;
  struct struct_block  *block;
  unsigned int int_big_endian;
  unsigned int int_little_endian;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* verbose */
  verbose__("[*] computing the block's length.\n");

  /* for each entry, find the block and fill the size */
  for(length=conf->length;length;length=length->next) {
    for(block=conf->block;block;block=block->next) {
      if (block->id == length->id) {
	length->block = block;
	debug(3, "length (%d) and block[%d] connected\n", block->size, length->id);

	/* size in big-endian */
	int_big_endian = htonl(block->size);

	/* convert in little-endian */
	memcpy((unsigned char*) &int_little_endian    , (unsigned char *) &int_big_endian + 3, sizeof(char));
	memcpy((unsigned char*) &int_little_endian + 1, (unsigned char *) &int_big_endian + 2, sizeof(char));
	memcpy((unsigned char*) &int_little_endian + 2, (unsigned char *) &int_big_endian + 1, sizeof(char));
	memcpy((unsigned char*) &int_little_endian + 3, (unsigned char *) &int_big_endian, sizeof(char));
	
	switch(length->type) {

	case HF_BLOCK_SIZE_B_32 : /* big-endian 32bits */
	  modify_data(conf, (unsigned char *) &int_big_endian, sizeof(int), length->offset);
	  debug(3, "big-endian-32b length: 0x%02x%02x%02x%02x\n",
		*(conf->buf_fuzz + length->offset),
		*(conf->buf_fuzz + length->offset+1),
		*(conf->buf_fuzz + length->offset+2),
		*(conf->buf_fuzz + length->offset+3)); 
	  break;
	case HF_BLOCK_SIZE_L_32 : /* little-endian 32bits */
	  modify_data(conf, (unsigned char *) &int_little_endian, sizeof(int), length->offset);
	  debug(3, "little-endian-32b length: 0x%02x%02x%02x%02x\n",
		*(conf->buf_fuzz + length->offset),
		*(conf->buf_fuzz + length->offset+1),
		*(conf->buf_fuzz + length->offset+2),
		*(conf->buf_fuzz + length->offset+3)); 

	  break; 
	case HF_BLOCK_SIZE_B_16 : /* big-endian 16bits */
	  modify_data(conf, (unsigned char *) &int_big_endian + 2, sizeof(short), length->offset);
	  debug(3, "big-endian-16b length: 0x%02x%02x\n",
		*(conf->buf_fuzz + length->offset),
		*(conf->buf_fuzz + length->offset+1)); 

	  break; 
	case HF_BLOCK_SIZE_L_16 : /* little-endian 16bits */
	  modify_data(conf, (unsigned char *) &int_little_endian + 2, sizeof(short), length->offset);
	  debug(3, "big-endian-16b length: 0x%02x%02x\n",
		*(conf->buf_fuzz + length->offset),
		*(conf->buf_fuzz + length->offset+1)); 
	  break; 
	case HF_BLOCK_SIZE_8 :    /* 8bits */
	  modify_data(conf, (unsigned char *) &int_big_endian + 3, sizeof(char), length->offset);
	  debug(3, "8b length : 0x%02x\n", *(conf->buf_fuzz + length->offset));
	  break; 
	case HF_BLOCK_SIZE_S_16 : /* string in hexadecimal */

	  /* TODOXXXFIXMEXXX */
	  break;
	case HF_BLOCK_SIZE_S_10 : /* string in decimal */
	  
	  /* TODOXXXFIXMEXXX */
	  break;
	default: /* should never happen */ 
	  break;
	  
	}
	break; /* optimization 8^) */
      }
    }
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: update_hash
 * DESC: update each hash value.
 *---------------------------------------------------------------------------*/
void update_hash(config *conf) {
  
  struct struct_hash *hash;
  struct struct_block  *block;
  unsigned int int_big_endian;
  unsigned int int_little_endian;
  unsigned int crc = 0;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* verbose */
  verbose__("[*] computing the block's hash.\n");

  /* for each entry, find the block and fill the size */
  for(hash=conf->hash;hash;hash=hash->next) {
    for(block=conf->block;block;block=block->next) {
      if (block->id == hash->id) {
	hash->block = block;
	debug(3, "hash (%d) and block[%d] connected\n", block->size, hash->id);

	switch(hash->type) {

	case HF_BLOCK_CRC32_B : /* CRC32 big-endian 32bits */

	  /* compute the CRC32 of the block */
	  crc = hash_crc32(conf->buf_fuzz + block->offset, block->size);

	  /* convert in big endian */
	  int_big_endian = htonl(crc);

          /* write the hash value */
          modify_data(conf, (unsigned char *) &int_big_endian, sizeof(int), hash->offset);
	  
	  debug(3, "big-endian-32b CRC32 hash: 0x%02x%02x%02x%02x\n",
		*(conf->buf_fuzz + hash->offset),
		*(conf->buf_fuzz + hash->offset+1),
		*(conf->buf_fuzz + hash->offset+2),
		*(conf->buf_fuzz + hash->offset+3)); 

	  break;

	case HF_BLOCK_CRC32_L : /* CRC32 big-endian 32bits */

	  /* compute the CRC32 of the block */
	  crc = hash_crc32(conf->buf_fuzz + block->offset, block->size);

	  /* convert in big endian */
	  int_big_endian = htonl(crc);

	  /* convert in little-endian */
	  memcpy((unsigned char*) &int_little_endian    , (unsigned char *) &int_big_endian + 3, sizeof(char));
	  memcpy((unsigned char*) &int_little_endian + 1, (unsigned char *) &int_big_endian + 2, sizeof(char));
	  memcpy((unsigned char*) &int_little_endian + 2, (unsigned char *) &int_big_endian + 1, sizeof(char));
	  memcpy((unsigned char*) &int_little_endian + 3, (unsigned char *) &int_big_endian, sizeof(char));


          /* write the hash value */
          modify_data(conf, (unsigned char *) &int_little_endian, sizeof(int), hash->offset);

	  debug(3, "little-endian-32b CRC32 hash: 0x%02x%02x%02x%02x\n",
		*(conf->buf_fuzz + hash->offset),
		*(conf->buf_fuzz + hash->offset+1),
		*(conf->buf_fuzz + hash->offset+2),
		*(conf->buf_fuzz + hash->offset+3)); 
	  break;

	default: /* should never happen */ 
	  break;
	  
	}
	break; /* optimization 8^) */
      }
    }
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}



/*---------------------------------------------------------------------------*                                            
 * NAME: fetch_instruction
 * DESC: fetch each instructions and fill block and fuzz structures
 *---------------------------------------------------------------------------*/
unsigned int fetch_instruction(config *conf) {

  unsigned char opcode;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the byte pointed by the offset in adc->buffer */
  opcode = read_buffer_char(conf);

  debug(3, "INSTRUCTION: %d\n", opcode);


  /* big switch for each instructions */
  switch(opcode) {
  case HF_HEX            : opcode_hf_hex(conf);             break;
  case HF_FUZZ_HEX       : opcode_hf_fuzz_hex(conf);        break;
  case HF_STRING         : opcode_hf_string(conf, -1);      break;
  case HF_FUZZ_STRING    : opcode_hf_fuzz_string(conf);     break;
  case HF_BLOCK_BEGIN    : opcode_hf_block_begin(conf);     break;
  case HF_BLOCK_END      : opcode_hf_block_end(conf);       break;
  case HF_BLOCK_SIZE_L_32: opcode_hf_block_size_l_32(conf); break;
  case HF_BLOCK_SIZE_L_16: opcode_hf_block_size_l_16(conf); break;
  case HF_BLOCK_SIZE_B_32: opcode_hf_block_size_b_32(conf); break;
  case HF_BLOCK_SIZE_B_16: opcode_hf_block_size_b_16(conf); break;
  case HF_BLOCK_SIZE_8   : opcode_hf_block_size_8(conf);    break;
  case HF_BLOCK_SIZE_S_16: opcode_hf_block_size_s_16(conf); break;
  case HF_BLOCK_SIZE_S_10: opcode_hf_block_size_s_10(conf); break;
  case HF_BLOCK_CRC32_B  : opcode_hf_block_crc32_b(conf);   break;
  case HF_BLOCK_CRC32_L  : opcode_hf_block_crc32_l(conf);   break;
  case HF_SEND           : opcode_hf_send(conf);            break;
  case HF_RECV           : opcode_hf_recv(conf);            break;


  default:
    error_("NOT SUPPORTED instruction detected!\n");
    error_("QUITTING!\n");
    return -1;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  return 0;
}

/*---------------------------------------------------------------------------*  
 * NAME: start_fuzz()                                   
 * DESC: send the fuzz buffer in a file or in a tcp/udp connection
 * RETN: 0 if everything is ok
 *       1 if sendto detects a Broken pipe (connection closed by foreign host)
 *       2 if recv_from receive 0 bytes    (connection closed by foreign host) 
 *      -1 if error
 *---------------------------------------------------------------------------*/
int start_fuzz(config *conf) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* TCP connection */
  if (conf->type == 0 || (conf->type == 1)) {
    if (!conf->mode) return client_fuzz(conf); /* client mode */
    else             return server_fuzz(conf); /* server mode */
  }

  /* FILE connection */  
  else if (conf->type == 2) 
    return file_fuzz(conf);

  /* should never happen (unknown type) */
  return -1;
}


/*---------------------------------------------------------------------------*                                            
 * NAME: fuzz_finished
 * DESC: return 0 if not finished, otherwise return 1
 *---------------------------------------------------------------------------*/
unsigned int fuzz_finished(config *conf) {

  struct struct_fuzz *fuzz;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  for (fuzz = conf->fuzz;fuzz;fuzz = fuzz->next)
    if (fuzz->current < fuzz->total) {

      /* debug */
      debug(1, "<-----------------------[quit]\n");

      return 0;
    }
  debug(1, "no more fuzz.. this is the end\n");

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  return 1;
}

/*---------------------------------------------------------------------------*                                            
 * NAME: fuzz_core
 * DESC: the heart of the fuzzing function.
 *
 *       id is the id of the fuzz
 *       filename is the file used to fuzz
 *       weight is the default weight of the fuzz
 *
 *       RETURN 0     -> don't fuzz.
 *       RETURN struct struct_fuzz *fuzz of the fuzzed value
 *---------------------------------------------------------------------------*/
struct struct_fuzz *fuzz_core(config *conf, unsigned int id, unsigned char *filename, unsigned int weight) {

  struct struct_fuzz *fuzz;
  struct struct_fuzz *prev = NULL;
  struct struct_source *source;
  FILE *file;
  unsigned char *file_content;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* check if there is a fuzz structure for this id */
  debug(3, "value of conf->fuzz: %p\n", conf->fuzz);
  for (fuzz=conf->fuzz;fuzz;fuzz=fuzz->next) {
    prev = fuzz;
    if (fuzz->id == id) break;
  }

  /* there is no entry, we add a new one */
  if (!fuzz) {
    debug(2, "Adding a new fuzz entry\n");

    /* malloc_ */
    fuzz = malloc_(sizeof(struct struct_fuzz));

    /* init the structure */
    fuzz->next = NULL;
    fuzz->source = NULL;
    fuzz->id = id;
    fuzz->current = 0;
    fuzz->weight = weight;
    
    /* update previous entry */
    if (prev) prev->next = fuzz;

    /* update if it's the first entry */
    if (!conf->fuzz) conf->fuzz = fuzz;

    /* init total + parse the source files */
    fuzz->total = fill_source(fuzz, filename);

    /* bad (memory leak, but who cares?) */
    if (fuzz->total == -1) {
      exit(-31339);
    }
  }


  /* there is an entry */
  else {
    debug(2, "fuzz[%d] found\n", fuzz->id);
    
    /* we are currently fuzzing this structure */
    if (conf->fuzz_current == fuzz->id) {


      /* check if we use the debugger mode, if yes, check if we
	 don't use the bruteforce mode, if yes, check if the
	 weight is > 0, if not fuzz->current = fuzz->total i.e.
	 we don't fuzz this entry */
      if ((conf->dbg_mode) && (!conf->bruteforce) && (fuzz->weight <= 0)) {
	fuzz->current = fuzz->total;
	verbose_("[!] this fuzz is *NOT* relevant, we don't fuzz id: %d\n", fuzz->id);
      }
      
      /* there is something to fuzz */
      if (fuzz->current < fuzz->total) {

	debug(1, "fuzzing: %d/%d\n", fuzz->current+1, fuzz->total);

	/* open the file pointed by fuzz->source->filename where id == fuzz->current */
	for (source = fuzz->source; source; source = source->next) {
	  debug(3, "source: %p source->id: %d fuzz->current: %d\n", source, source->id, fuzz->current);
	  if (source->id == fuzz->current) {
	    break;
	  }
	}

	debug(3,"we catch the source->id: %d and fuzz->current: %d\n", source->id, fuzz->current); 
	debug(3,"try to open the file: \"%s\"\n", source->filename);
	/* open the file */
	file = fopen(source->filename, "r");

	/* should never happen */
	if (!file) {
	  error_(" ??? cannot read another time this file: \"%s\" ???: ", source->filename);
	  perror("");
	  error_("QUITTING!\n");
	  exit(-31337);
	}

	/* allocate buffer */
	file_content = malloc_(source->size);

	/* copy the content of the file in the buffer */
	if (fread(file_content, source->size, 1, file) <= 0) {
	  error_(" ??? cannot read the content of this file: \"%s\" ???: ", source->filename);
	  perror("");
	  error_("QUITTING!\n");
	  exit(-31338);
	}

	/* close the file */
	fclose(file);

	/* copy the content in the fuzz buffer */
	push_data(conf, file_content, source->size);

	/* free file_content */
	free(file_content);

	/* update the fuzzer counter */
	fuzz->current++;

	/* debug */
	debug(1, "<-----------------------[quit]\n");

	return fuzz;
      }

      /* we can fuzz the next structure */
      conf->fuzz_current++; 
      debug(2, "we have fuzzed all the data for this function...\n");
      return 0;
    }
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* we are not currently fuzzing this structure */
  return 0;
}


/*---------------------------------------------------------------------------*                                            
 * NAME: free_block
 * DESC: free all the linked block structure
 *---------------------------------------------------------------------------*/
void free_block(config *conf) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  if (conf->block) {   
    struct struct_block *block;
    struct struct_block *block_to_free;
    block = conf->block;
    while(block) {
      block_to_free = block;
      block = block->next;
      debug(3, "free block[%d]\n", block_to_free->id);
      free(block_to_free);
    }
    conf->block = NULL;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: free_length
 * DESC: free all the linked length structure
 *---------------------------------------------------------------------------*/
void free_length(config *conf) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  if (conf->length) {   
    struct struct_length *length;
    struct struct_length *length_to_free;
    length = conf->length;
    while(length) {
      length_to_free = length;
      length = length->next;
      debug(3, "free length of block[%d]\n", length_to_free->id);
      free(length_to_free);
    }
    conf->length = NULL;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: free_hash
 * DESC: free all the linked hash structure
 *---------------------------------------------------------------------------*/
void free_hash(config *conf) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  if (conf->hash) {   
    struct struct_hash *hash;
    struct struct_hash *hash_to_free;
    hash = conf->hash;
    while(hash) {
      hash_to_free = hash;
      hash = hash->next;
      debug(3, "free hash of block[%d]\n", hash_to_free->id);
      free(hash_to_free);
    }
    conf->hash = NULL;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * NAME: free_chrono
 * DESC: free all the linked chrono structure
 *---------------------------------------------------------------------------*/
void free_chrono(config *conf) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  if (conf->chrono) {   
    struct struct_chrono *chrono;
    struct struct_chrono *chrono_to_free;
    chrono = conf->chrono;
    while(chrono) {
      chrono_to_free = chrono;
      chrono = chrono->next;
      debug(3, "free chrono of block[%d]\n", chrono_to_free->id);
	free(chrono_to_free);
    }
    conf->chrono = NULL;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");


}

/*---------------------------------------------------------------------------*                                            
 * NAME: fuzz_engine
 * DESC: fetch and parse each ad instructions
 *---------------------------------------------------------------------------*/
unsigned int fuzz_engine(config *conf) {

  int result;
  int error_occured = 0;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* if mode file, open the debugger file */
  if (conf->type == 2) {
    if(create_debugger_file(conf)) return -1;
  }

  do {

    /* parse all the file */
    while (conf->adc->offset < conf->adc->size) {

      /* fetch the instruction */
      if (fetch_instruction(conf)) return -200; 
    }

    /* compute the lengths */
    update_length(conf);

    /* compute the hash values */
    update_hash(conf);

    /* send the start message to the debugger. i.e. the fuzzer cannot speak 
       with the debugger anymore, only the debugger send messages to the fuzzer */
    if ((conf->dbg_mode) && (!conf->ring_zero))
      if (dbg_send_msg(conf, INET_START_A_MSG, 0 , NULL, 0)) return -1;
    
    /* start the fuzzing */
    result = start_fuzz(conf);
    
    if (result == -1)
      error_occured = 1;

    /* free the (big) fuzz buffer */
    if (conf->buf_fuzz) { 
      free(conf->buf_fuzz); 
      conf->buf_fuzz = NULL;
    }

    /* free the blocks */
    free_block(conf);

    /* free the lengths */
    free_length(conf);

    /* free the hashes */
    free_hash(conf);

    /* free the chronos */
    free_chrono(conf);

    /* reset the offset */
    conf->adc->offset = conf->adc->start;

    /* add a value to the fuzz counter */
    conf->fuzz_file_id++;


    /* we close the debugger file */
    if ((conf->type == 2) && (!conf->ring_zero)) {
      if(fclose(conf->fuzz_file_dbg)) {
	error_("error closing the debugger file.\n");
	perror("");
	error_("QUITTING!\n");
	return -1;
      }
    }

    /* we are no more in the ring zero mode */
    conf->ring_zero = 1;

  } while (!fuzz_finished(conf) && (error_occured == 0));


  /* free the fuzz */
  if (conf->fuzz) {
    struct struct_fuzz *fuzz;
    struct struct_fuzz *fuzz_to_free;
    fuzz = conf->fuzz;
    while (fuzz) {
      fuzz_to_free = fuzz;
      fuzz = fuzz->next;

      /* before freeing fuzz, free source */
      if (fuzz_to_free->source) {
	struct struct_source *source;
	struct struct_source *source_to_free;
	source = fuzz_to_free->source;	
	/* free the filename first */


	while(source) {
	  source_to_free = source;
	  source = source->next;
	  debug(3, "free source: %d filename: \"%s\"\n", source_to_free->id, source_to_free->filename);
	  if (source_to_free->filename) free(source_to_free->filename);
	  free(source_to_free);
	}

      }
      debug(3, "free fuzz[%d]\n", fuzz_to_free->id);
      free(fuzz_to_free);
    }
    conf->fuzz = NULL;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return value */
  return 0;
}
