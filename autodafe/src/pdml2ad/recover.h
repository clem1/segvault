/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : recover.h
 * DESCRIPTION: Define recover functions
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * NAME: recover_string
 * DESC: with name, show and showname, try to detect if the hex value could
 *       be a string.
 * RETN: unsigned int > 0 if a string or 0 if definitely not a string
 *---------------------------------------------------------------------------*/
unsigned int recover_string(config *conf, char *name, char *show, char *value, char *showname);


/*---------------------------------------------------------------------------*
 * NAME: recover_length
 * DESC: try to detect if this value is a length
 * RETN: 0 if nothing, a pourcentage number (1-100)
 *---------------------------------------------------------------------------*/
unsigned int recover_length(config *conf, char *name, char *show, char *value, char *showname);


/*---------------------------------------------------------------------------*
 * NAME: recover_raw_string
 * DESC: recover a string if all the values are ASCII printable and the
 *       string is terminated by a 0x0a. (used in raw mode)
 * RETN: unsigned int > 0 if a string or 0 if definitely not a string
 *       1 if not terminated by a 0x0a but looks like a string
 *---------------------------------------------------------------------------*/
unsigned int recover_raw_string(config *conf, char *value);
