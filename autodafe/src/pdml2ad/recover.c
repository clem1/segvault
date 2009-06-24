/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : recover.c                                                      
 * DESCRIPTION: detect with probability if it's relevant to convert a hex
 *              value in a string - detect with probability if it's relevant
 *              to convert a hex value to a block's length value
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>  /* malloc */
#include <string.h>  /* strlen */
#include <ctype.h>   /* isascii */

#include <netinet/in.h> /* htonl, htons */
#include "debug.h"
#include "conf.h"
#include "output.h"
#include "../../include/autodafe.h"


/*---------------------------------------------------------------------------*
 * NAME: convert_asc_string
 * DESC: convert a string in asc-hex value in a char string value.
 * RETN: a buffer-pointer on a allocated memory containing the string in asc.
 * WARN: DON'T FORGET TO FREE THE BUFFER AFTER THIS CALL!
 *       STRING MUST BE A EVEN-NUMBER-SIZE!
 *---------------------------------------------------------------------------*/
char * convert_asc_string(char *string) {

  int i;
  char *hex_value;
  char hex_2[3];
  int c;

  /* malloc hex_value buffer */
  hex_value = malloc_(strlen(string)/2 + 1);

  /* first, convert the string in a hex value */
  for(i=0;i<strlen(string);i+=2) {
    
    /* convert the 2-byte string in a char value */
    hex_2[0] = string[i];
    hex_2[1] = string[i+1];
    hex_2[2] = '\0';
    c = (int) strtoul(hex_2, NULL, 16);
    debug(3, " parsing char: %d (%c)\n", c, (char) c);

    /* check if the value is not a typicall ascii char */
    /* if quote, write it in hexa... */
    if ((c <7) || (c>126)) {
      
      /* free hex_value */
      free(hex_value);
      return 0;
    }
    hex_value[i/2] = (char) c;
    
  }
  hex_value[strlen(string)/2] = '\0';
  return hex_value;
}


/*---------------------------------------------------------------------------*
 * NAME: remove_rn
 * DESC: remove the \r\n or the \r or the \n of a string
 * RETN: 3 = removed \r
 *       2 = removed \r\n
 *       1 = removed \n
 *       0 = removed nothing
 *---------------------------------------------------------------------------*/
unsigned int remove_rn(char *string) {

  unsigned int rn = 0;
  unsigned int size;

  size = strlen(string);

  /* at least one char before \n or \r\n ... */
  if (size > 1) {
    if (string[size-1] == '\r') {
      rn = 3;
      string[size-1] = '\0';
    }
    else if (string[size-1] == '\n') {
      string[size-1] = '\0';
      if (size > 2) {
	if(string[size-2] == '\r') {
	  rn++;
	  string[size-2] = '\0';
	}
      }
      rn++;
    }
  }
  return rn;
}

/*---------------------------------------------------------------------------*
 * NAME: compare_string
 * DESC: compare the string with the value of the attribute
 * RETN: unsigned int > 0 if a string or 0 if definitely not a string
 *---------------------------------------------------------------------------*/
unsigned int compare_string(config *conf, char *b_value, char *attr, unsigned int rn_value) {
  
  if ((attr) && ((attr != "")||(b_value != "")) && (strlen(attr) > 0) && (strlen(b_value) > 0)) {
    debug(3, "b_value's size : %d\n", strlen(b_value));
    debug(3, "attribute size: %d\n", strlen(attr));
    if (strlen(b_value) <= strlen(attr)) {
      if (!strncmp(b_value, attr, strlen(b_value))) {
	verbose_("[*] string value matches with attribute: \"%s\"\n",attr);
	debug(3, "b_value: %s\n", b_value);
	debug(3, "attr   : %s\n", attr);
	output_tab_string(conf->tab, b_value, rn_value);
	return 1;
      }
    }
    else {
      if (!strncmp(b_value, attr, strlen(attr))) {
	verbose_("[*] string value matches with attribute: \"%s\"\n", attr);
	debug(3, "b_value: %s\n", b_value);
	debug(3, "show   : %s\n", attr);
	output_tab_string(conf->tab, b_value, rn_value);
	return 1;
      }
    }
  }
  return 0;

}


/*---------------------------------------------------------------------------*
 * NAME: recover_string
 * DESC: with name, show and showname, try to detect if the hex value could
 *       be a string.
 * RETN: unsigned int > 0 if a string or 0 if definitely not a string
 *---------------------------------------------------------------------------*/
unsigned int recover_string(config *conf, char *name, char *show, char *value, char *showname) {
  
  char *b_value = 0;
  unsigned int rn_value = 0;  
  unsigned int result = 0;

  /* convert value in ascii string */
  if (value) {
    b_value = convert_asc_string(value);
    /* not a good char */
    if (!b_value) {
      debug(2, "the value is not a string\n");
      return 0;
    }
    rn_value = remove_rn(b_value);
    debug(2, "string value recovered: %s\n", b_value);
  }

  /* compare the string with show */
  if (!result) result = compare_string(conf, b_value, show, rn_value);

  /* compare the string with showname */
  if (!result) result = compare_string(conf, b_value, showname, rn_value);

  /* compare the string with name */
  if (!result) result = compare_string(conf, b_value, name, rn_value);

  /* free */
  if (b_value) free(b_value);

  return result;
}


/*---------------------------------------------------------------------------*
 * NAME: recover_raw_string
 * DESC: recover a string if all the values are ASCII printable and the
 *       string is terminated by a 0x0a. (used in raw mode)
 * RETN: unsigned int > 0 if a string or 0 if definitely not a string
 *       1 if not terminated by a 0x0a but looks like a string
 *---------------------------------------------------------------------------*/
unsigned int recover_raw_string(config *conf, char *value) {

  char *string = NULL;
  unsigned int rn_value = 0;
  int i;
  unsigned int size = 0;
  char *offset = NULL;

  /* convert the string */
  string = convert_asc_string(value);
  if (string == 0) {
    free(string);
    return 0;
  }

  /* check the last character */
  if (string[strlen(string)-1] != 0x0a) {
    debug(2, "last character is 0x%02x and not 0x0a\n", string[strlen(string) -1]);
    return 1;
  }


  /* remove the rn value */
  rn_value = remove_rn(string);
  debug(2, "raw string recovered: %s\n", string);

  /* now check if there is sub strings */
  offset = string;
  size = strlen(string);
  
  for(i=0;i<size;i++) {

    /* end of string */
    if (i == size - 1) {
      output_tab_string(conf->tab, offset, rn_value);      
      verbose_("[*] string recovered: %s", offset);
      if (rn_value == 1) 
	verbose_("\\n\n");
      else if (rn_value == 2) 
	verbose_("\\r\\n\n");
      else if (rn_value == 3) 
	verbose_("\\r\n");
      else
	verbose_("\n");
    }

    else if (string[i] == '\r') {
      
      /* we have a \r\n */
      if (string[i+1] = '\n') {
	string[i] = '\0';
	string[i] = '\0';
	output_tab_string(conf->tab, offset, 2);
	verbose_("[*] string recovered: %s\\r\\n\n", offset);
	offset = string + i + 1 + 1;
	i++;
      }
      
      /* we have a \r */
      else{
	string[i] = '\0';
	output_tab_string(conf->tab, offset, 3);
	verbose_("string recovered: %s\\r\n", offset);
	offset = string + i + 1;
      }
      
    }

    /* we have a \n */
    else if (string[i] == '\n') {
      string[i] == '\0';
      output_tab_string(conf->tab, offset, 1);
      verbose_("string recovered: %s\\n\n", offset);
      offset = string + i + 1;
    }
  }

  return 100;
}


/*---------------------------------------------------------------------------*
 * NAME: recover_length
 * DESC: try to detect if this value is a length
 * RETN: unsigned int = pourcentage (0-100)% of the probability
 *---------------------------------------------------------------------------*/
unsigned int recover_length(config *conf, char *name, char *show, char *value, char *showname) {

  int i;
  unsigned int show_value_10;
  unsigned int result = 0;

  /* different kind of "length" words */
  char *length_string[4] = { "_length",
			     ".length",
			     "_Length",
			     ".Length"};
  
  debug(3, "recovering the length field\n");
  /* compare the name value with the length strings */
  if (name) {
    for(i=0;i<4;i++) {
      if (strstr(name, length_string[i])) {
	debug(2, "length string \"%s\" found in name: %s\n", length_string[i], name);
	result+=33;
      }
    }
  }

  /* compare the showname value with the length strings */
  if (showname) {
    for(i=0;i<4;i++) {
      if (strstr(showname, length_string[i])) {
	debug(2, "length string \"%s\" found in showname: %s\n", length_string[i], showname);
	result+=33;
      }
    }
  }

  /* compare the show value with the length strings */
  if (show) {
    for(i=0;i<4;i++) {
      if (strstr(show, length_string[i])) {
	debug(2, "length string \"%s\" found in show: %s\n", length_string[i], show);
	result+=33;
      }
    }
  }

  /* check if we accept the value */
  if (result >= conf->check_length) {

    /* check what kind of length it is (little endian, big endian) */
    result = strtoul(value, NULL, 16);
    show_value_10 = strtoul(show, NULL, 10);
    debug(3, "result       : %d (%p)\n", result, result);
    debug(3, "show_value   : %d (%p)\n", show_value_10, show_value_10);

    /* check what is the size of length ( 8bits, 16bits, 32bits) */
    switch(strlen(value)/2) {
    case 1:
      /* 8 bits - don't care big/little endian */
      output_tab_(conf->tab, AD_F_HEX"(%s); /* "AD_F_SIZE_8"(\"XXXFIND_MY_BLOCKXXX\"); */\n", value);
      break;
    case 2:
      /* 16 bits */
      if (result == show_value_10)
	output_tab_(conf->tab, AD_F_HEX"(%s); /* "AD_F_SIZE_B16"(\"XXXFIND_MY_BLOCKXXX\"); */\n", value);

      else if ( result == htons(show_value_10))
	output_tab_(conf->tab, AD_F_HEX"(%s); /* "AD_F_SIZE_L16"(\"XXXFIND_MY_BLOCKXXX\"); */\n", value);
      else result = 0;
      break;
    case 4:
      /* 32 bits */
      if (result == show_value_10)
	output_tab_(conf->tab, AD_F_HEX"(%s); /* "AD_F_SIZE_B32"(\"XXXFIND_MY_BLOCKXXX\"); */\n", value);
      else if (result == htonl(show_value_10))
	output_tab_(conf->tab, AD_F_HEX"(%s); /* "AD_F_SIZE_L32"(\"XXXFIND_MY_BLOCKXXX\"); */\n", value);
      else result = 0;
      break;
    default:
      debug(2, "it could be a length value\n");
      output_tab_(conf->tab, "/**** it could be a length value ****/\n");
      result = 0;
    }
  }
  else 
    result = 0;
  
  
  /* return */
  return result;
}
