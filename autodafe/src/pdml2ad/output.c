/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : output.c                                                      
 * DESCRIPTION: Choose if the output must in stdout or in a file (depending 
 *              if the -o argument is filled)      
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>  /* malloc */
#include <string.h>  /* strlen */
#include "debug.h"
#include "conf.h"
#include "../../include/autodafe.h"
FILE *output_desc;

/*---------------------------------------------------------------------------*
 * NAME: output_
 * DESC: print the string either in a file (depending of the -o arg) or in
 *       the standard output.
 *---------------------------------------------------------------------------*/
void output_(char *fmt, ...) {
  char buf[MAX_OUTPUT_SIZE];
  va_list args;
  
    va_start(args, fmt);
    vsnprintf(buf, MAX_OUTPUT_SIZE, fmt, args);
    va_end(args);
    fprintf(output_desc, "%s", buf);
}

/*---------------------------------------------------------------------------*
 * NAME: output_tab_
 * DESC: print the string either in a file (depending of the -o arg) or in
 *       the standard output. With a tab argument = number of tab
 *---------------------------------------------------------------------------*/
void output_tab_(unsigned int tab, char *fmt, ...) {
  char buf[MAX_OUTPUT_SIZE];
  va_list args;
  int i;
  
  va_start(args, fmt);
  vsnprintf(buf, MAX_OUTPUT_SIZE, fmt, args);
  va_end(args);
  for(i = 0;i<tab;i++)
    fprintf(output_desc,"   ");
  fprintf(output_desc, "%s", buf);
}

/*---------------------------------------------------------------------------*
 * NAME: output_tab_ether
 * DESC: special function who print hexa values using the ethereal-like style
 *       for example: 0x112233445566778899aabbccddeeff 
 *       become: 00 11 22 33 44 55 66 77 88    99 aa bb cc dd ee ff 
 *---------------------------------------------------------------------------*/
void output_tab_ether(unsigned int tab, char *string) {
  int i;
  int j;
  int k;

  debug(3, "entering in ethereal style\n");
  for(i=0;i<strlen(string);i+=16*2) {

    /* make the tabs.. */
    for(k = 0;k<tab;k++)
      fprintf(output_desc,"   ");
    
    /* print the chars */
    for(j=0;j<8;j++) {
      if (string[i+j*2] == 0x0)
	goto output_tab_ether_end;
      fprintf(output_desc, "%c%c ", string[i+j*2], string[i+j*2+1]);
    }
    fprintf(output_desc, "   ");
    for(j=8;j<16;j++) {
      if (string[i+j*2] == 0x0)
	goto output_tab_ether_end;
      fprintf(output_desc, "%c%c ", string[i+j*2], string[i+j*2+1]);
    }
  output_tab_ether_end:
    fprintf(output_desc, "\n");
  }
}

/*---------------------------------------------------------------------------*
 * NAME: output_tab_string
 * DESC: print the string either in a file (depending of the -o arg) or in
 *       the standard output. With a tab argument = number of tab
 *       If the string is bigger than 255, cut in differents strings
 *       If there is a 0x22 = '"' char, use hex value... (ad compliant)
 *---------------------------------------------------------------------------*/
void output_tab_string(unsigned int tab, char *string, unsigned int rn) {

  unsigned int size = 0;
  int i;
  int j;
  int k;

  /* check the string size */
  size = strlen(string);

  /* if it's bigger than MAX_LENGTH_STRING, fragment */
  for(i=0; i<size;i+=MAX_LENGTH_STRING) {
    for(j = 0;j<tab;j++) 
      fprintf(output_desc,"   ");

    /* start with string function */
    fprintf(output_desc, AD_F_STRING"(\"");
 
    for(k=0;k<MAX_LENGTH_STRING;k++) {
      if (string[i+k] == '\0')
	break;

      /* quote detected..yeah i know it's ugly.. */
      if (string[i+k] == '\"') {
	fprintf(output_desc, "\");\n");
	for(j = 0;j<tab;j++) 
	  fprintf(output_desc,"   ");
	fprintf(output_desc, AD_F_HEX"(22); /* quote */\n");
	for(j = 0;j<tab;j++) 
	  fprintf(output_desc,"   ");
	fprintf(output_desc, AD_F_STRING"(\"");
      }
      else {
	fprintf(output_desc, "%c", string[i+k]);
      }
    }
    /* the end of the string_XX function */
    fprintf(output_desc, "\");\n");
  }    
  /* print the \n or \rn at the end */
  switch (rn) {
    
  case 1: /* \n */
    for(j = 0;j<tab;j++) 
      fprintf(output_desc,"   ");
    fprintf(output_desc, AD_F_HEX"(0a); /* \\n */\n");
    break;
    
  case 2: /* \r\n */
    for(j = 0;j<tab;j++) 
      fprintf(output_desc,"   ");
    fprintf(output_desc, AD_F_HEX"(0d 0a); /* \\r\\n */\n");
    break;

  case 3: /* \r */
    for(j = 0;j<tab;j++) 
      fprintf(output_desc,"   ");
    fprintf(output_desc, AD_F_HEX"(0d); /* \\r */\n");
    break;

    
  default: /* nothing */
    break;
    
  };
  
}

