/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : engine.h
 * DESCRIPTION: define fuzz_engine functions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * NAME: fuzz_engine
 * DESC: fetch and parse each "ad" instructions
 *---------------------------------------------------------------------------*/
unsigned int fuzz_engine(config *conf);


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
struct struct_fuzz *fuzz_core(config *conf, unsigned int id, unsigned char *filename, unsigned int weight);

/*---------------------------------------------------------------------------*                                            
 * NAME: push_data(config *conf, unsigned char *ptr, unsigned int size)
 * DESC: push data to the pkt buffer using realloc
 *---------------------------------------------------------------------------*/
void push_data(config *conf, unsigned char *ptr, unsigned int size);


/*---------------------------------------------------------------------------*                                               
 * NAME: push_length(config *conf, unsigned int id, unsigned int type)
 * DESC: add a length in the linked list length structure
 *---------------------------------------------------------------------------*/
void push_length(config *conf, unsigned int id, unsigned int type);

/*---------------------------------------------------------------------------*                                               
 * NAME: push_hash(config *conf, unsigned int id, unsigned int type)
 * DESC: add a hash in the linked list hash structure
 *---------------------------------------------------------------------------*/
void push_hash(config *conf, unsigned int id, unsigned int type);

/*---------------------------------------------------------------------------*                                            
 * NAME: update_block_size
 * DESC: update each size of the opened blocks
 *---------------------------------------------------------------------------*/
void update_block_size(config *conf, unsigned int size);


/*---------------------------------------------------------------------------*                                            
 * NAME: read_buffer_char
 * DESC: give one char in the buffer using the offset
 *---------------------------------------------------------------------------*/
unsigned char read_buffer_char(config *conf);


/*---------------------------------------------------------------------------*                                            
 * NAME: read_buffer_int
 * DESC: give one int in the buffer using the offset in local-endian format
 *       because, mainly used for the computation of the size -> compared 
 *       with an id which is in the local-endian type too. (ntohl used)
 *---------------------------------------------------------------------------*/
unsigned int read_buffer_int(config *conf);


