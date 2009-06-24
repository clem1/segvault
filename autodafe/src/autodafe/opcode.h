/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Jun.2004 ---*                                          
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : opcode.h
 * DESCRIPTION: define the opcode functions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_hex
 * DESC: what to do when we have a hf_hex function
 *---------------------------------------------------------------------------*/
void opcode_hf_hex(config *conf);


/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_fuzz_hex
 * DESC: what to do when we have a hf_fuzz_hex function
 *---------------------------------------------------------------------------*/
void opcode_hf_fuzz_hex(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_string
 * DESC: what to do when we have a hf_string function
 *       if fuzzing, id != 0xffffffff (used for the debugger)
 *---------------------------------------------------------------------------*/
void opcode_hf_string(config *conf, unsigned int id);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_fuzz_string
 * DESC: what to do when we have a hf_fuzz_string function
 *---------------------------------------------------------------------------*/
void opcode_hf_fuzz_string(config *conf);


/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_string_n
 * DESC: what to do when we have a hf_string_n function
 *---------------------------------------------------------------------------*/
//void opcode_hf_string_n(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_string_rn
 * DESC: what to do when we have a hf_string_rn function
 *---------------------------------------------------------------------------*/
//void opcode_hf_string_rn(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_begin
 * DESC: what to do when we have a hf_block_begin function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_begin(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_end
 * DESC: what to do when we have a hf_block_end function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_end(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_l_32
 * DESC: what to do when we have a hf_block_size_l_32 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_l_32(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_l_16
 * DESC: what to do when we have a hf_block_size_l_16 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_l_16(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_b_32
 * DESC: what to do when we have a hf_block_size_b_32 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_b_32(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_b_16
 * DESC: what to do when we have a hf_block_size_b_16 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_b_16(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_block_size_8
 * DESC: what to do when we have a hf_block_size_8 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_8(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_s_16
 * DESC: what to do when we have a hf_block_size_s_16 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_s_16(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_size_s_10
 * DESC: what to do when we have a hf_block_size_s_10 function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_size_s_10(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_crc32_b
 * DESC: what to do when we have a hf_block_crc32_b function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_crc32_b(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_block_crc32_l
 * DESC: what to do when we have a hf_block_crc32_l function
 *---------------------------------------------------------------------------*/
void opcode_hf_block_crc32_l(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_send
 * DESC: what to do when we have a hf_send function
 *---------------------------------------------------------------------------*/
void opcode_hf_send(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: opcode_hf_recv
 * DESC: what to do when we have a hf_recv function
 *---------------------------------------------------------------------------*/
void opcode_hf_recv(config *conf);
