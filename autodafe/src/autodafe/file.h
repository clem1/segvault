/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : file.h
 * DESCRIPTION: define functions
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*                                            
 * NAME: create_debugger_file
 * DESC: open the debugger file (ready to write)
 * RETN: 0 if ok, -1 if error
 *---------------------------------------------------------------------------*/
int create_debugger_file(config *conf);


/*---------------------------------------------------------------------------*
 * NAME: read_adc_file
 * DESC: parse an adc file
 *---------------------------------------------------------------------------*/
unsigned int read_adc_file(config *conf, char *filename);

/*---------------------------------------------------------------------------*                                            
 * NAME: file_fuzz
 * DESC: create a file for the current fuzzing structure
 * RETN:  0 if ok
 *       -1 if error
 *---------------------------------------------------------------------------*/
int file_fuzz(config *conf);

/*---------------------------------------------------------------------------*                                            
 * NAME: fill_source
 * DESC: return  number of line (exploitable) in the file
 *       return -1 if Error (quit)
 *---------------------------------------------------------------------------*/
int fill_source(struct struct_fuzz *fuzz, unsigned char *filename);
