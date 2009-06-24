/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : output.h
 * DESCRIPTION: Define output_
 *---------------------------------------------------------------------------*/
#include <features.h>
/*---------------------------------------------------------------------------*
 * NAME: output_
 * DESC: print the string either in a file (depending of the -o arg) or in
 *       the standard output.
 *---------------------------------------------------------------------------*/
void output_(char *fmt, ...);


/*---------------------------------------------------------------------------*
 * NAME: output_tab_
 * DESC: print the string either in a file (depending of the -o arg) or in
 *       the standard output. With a tab argument = number of tab
 *---------------------------------------------------------------------------*/
void output_tab_(unsigned int tab, char *fmt, ...);

/*---------------------------------------------------------------------------*
 * NAME: output_tab_ether
 * DESC: special function who print hexa values using the ethereal-like style
 *       for example: 0x112233445566778899aabbccddeeff 
 *       become: 00 11 22 33 44 55 66 77 88    99 aa bb cc dd ee ff 
 *---------------------------------------------------------------------------*/
void output_tab_ether(unsigned int tab, char *string);



/*---------------------------------------------------------------------------*
 * NAME: output_tab_string
 * DESC: print the string either in a file (depending of the -o arg) or in
 *       the standard output. With a tab argument = number of tab
 *       If the string is bigger than 255, cut in differents strings
 *       If there is a 0x22 = '"' char, use hex value... (ad compliant)
 *---------------------------------------------------------------------------*/
void output_tab_string(unsigned int tab, char *string, unsigned int rn);
