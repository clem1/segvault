/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Jun.2004 ---*                                          
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : file.c
 * DESCRIPTION: all the function for the parsing of the ADC files.
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>    /* malloc */
#include <string.h>    /* memcmp */

#include <sys/types.h> /* stat */
#include <sys/stat.h>  /* stat */
#include <unistd.h>    /* stat */

#include "debug.h"
#include "conf.h"
#include "chrono.h"
#include "../../include/autodafe.h"



/*---------------------------------------------------------------------------*                                            
 * NAME: check_directory
 * DESC: check if filename is a directory
 * RETN:  0 if ok
 *       -1 if error
 *---------------------------------------------------------------------------*/
int check_directory(config *conf) {
  struct stat *st = NULL; 

  /* debug */
  debug(1, "<-----------------------[enter]\n");

#define PATH_MAX 1024
  /* check the length of the directory - useless but ... */
  if (strlen(conf->fuzz_file_dir) >= PATH_MAX - 16) {
    error_("error path too long\n");
    error_("QUITTING!\n");
    return -1;
  }


  /* init structure */
  st = (struct stat *) malloc_(sizeof(struct stat));

  /* stat (catch the error of the path length (<255)) */
  if (stat(conf->fuzz_file_dir, st)) {
    error_("error with directory: \"%s\": ", conf->fuzz_file_dir);
    perror("");
    error_("QUITTING!\n");
    if (st) {free(st); st = NULL;}
    return -1;
  }

  /* check if it's a directory */
  if (S_ISDIR(st->st_mode) == 0) {
    error_("file \"%s\" is not a directory\n", conf->fuzz_file_dir);
    error_("QUITTING!\n");
    if (st) {free(st); st = NULL;}
    return -1;
  }

  /* free structure */
  if (st) {free(st); st = NULL;}

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return ok */
  return 0;
}


/*---------------------------------------------------------------------------*                                            
 * NAME: create_debugger_file
 * DESC: open the debugger file (ready to write)
 *---------------------------------------------------------------------------*/
int create_debugger_file(config *conf) {

  /* same as filename + ".dbg" */
  char *filename_dbg;
      
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  if (check_directory(conf))
    return -1;

  /* alloc <directory> [<'\'>] <debugger_file> <'\0'> */
  filename_dbg = malloc_(strlen(conf->fuzz_file_dir) + 1 + strlen(DEBUGGER_FILE) + 1);
  
  /* init */
  bzero(filename_dbg, strlen(conf->fuzz_file_dir) + 1 +strlen(DEBUGGER_FILE) + 1);


  /* copy */
  strncpy(filename_dbg, conf->fuzz_file_dir, strlen(conf->fuzz_file_dir));

  /* add the slash at the end of filename */
  if (filename_dbg[strlen(conf->fuzz_file_dir) -1] != '/') {
    filename_dbg[strlen(conf->fuzz_file_dir)] = '/';
    filename_dbg[strlen(conf->fuzz_file_dir) + 1] = '\0';
  }

  strncat(filename_dbg, DEBUGGER_FILE, strlen(DEBUGGER_FILE));
  
  /* just to be sure (but useless) */
  filename_dbg[strlen(conf->fuzz_file_dir) + 1 + strlen(DEBUGGER_FILE)] = '\0';
  
  /* open file */
  conf->fuzz_file_dbg = fopen(filename_dbg, "w");
  if (!conf->fuzz_file_dbg) {
    error_("error writing the dbg file: \"%s\": ", filename_dbg);
    perror("");
    error_("QUITTING!\n");
    sleep(1);
    return -1;
  }
  
  /* verbose */
  verbose_("[!] created debugger's file: \"%s\"\n", filename_dbg);
  
  /* free buffer */
  free(filename_dbg);
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* everything is ok */
  return 0;
  
}



/*---------------------------------------------------------------------------*                                            
 * NAME: create_fuzz_file
 * DESC:  1. open a file in the directory (conf->fuzz_file_dir) with a number
 *           as a name (conf->fuzz_file_id)
 *        2. redirect (conf->fuzz_file) to point on this file
 * RETN:  0 if ok
 *       -1 if error
 *
 * EXPN: We create a filename with a number of 8 digits and the path
 *       (the size if MAX_PATH - 16 + 1 + 8). We open the file (write access)
 *---------------------------------------------------------------------------*/
int create_fuzz_file(config *conf) {

  /* +1 (for the added slash)
     +8 (for the number in decimal)
     +1 (for the NULL byte) */
  char filename[strlen(conf->fuzz_file_dir) + 1 + 8 + 1];
  char name[8+1];

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* init */
  bzero(filename, strlen(conf->fuzz_file_dir) + 1 + 8 + 1);
  bzero(name, 9);

  /* write in ascii the name of the current fuzz_file_id */
  snprintf(name, 9, "%08d", conf->fuzz_file_id);
  debug(1, "name: %s\n", name);
  debug(1, "fuzz_file_id: %d\n", conf->fuzz_file_id);

  /* copy the dir name */
  strncpy(filename, conf->fuzz_file_dir, strlen(conf->fuzz_file_dir));

  /* add the slash at the end of filename */
  if (filename[strlen(conf->fuzz_file_dir) -1] != '/') {
    filename[strlen(conf->fuzz_file_dir)] = '/';
    filename[strlen(conf->fuzz_file_dir) + 1] = '\0';
  }

  /* add the number */
  strncat(filename, name, strlen(name));

  /* open the file */
  debug(1, "try to open the file\n");
  conf->fuzz_file = fopen(filename, "w");
  if (!conf->fuzz_file) {
    error_("error writing the file: \"%s\": ", filename);
    perror("");
    error_("QUITTING!\n");
    return -1;
  }
  debug(1,"fuzz file: \"%s\" created.\n", filename);
  verbose_("[!] created file: \"%s\"\n", filename);


  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return ok */
  return 0;
}

/*---------------------------------------------------------------------------*                                            
 * NAME: close_fuzz_file
 * DESC:  
 * RETN: 0 if ok
 *      -1 if error 
 *---------------------------------------------------------------------------*/
int close_fuzz_file(config *conf) {
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  if (fclose(conf->fuzz_file)) {
    error_("error closing the file: \"%08d\": ", conf->fuzz_file_id );
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  return 0;
}

/*---------------------------------------------------------------------------*                                            
 * NAME: read_adc_file
 * DESC: parse an adc file
 *---------------------------------------------------------------------------*/
unsigned int read_adc_file(config *conf, char *filename) {
  
  size_t byte_read;
  char magic_bytes[strlen(MAGIC_SEQUENCE)];
  struct stat *st;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* open the adc file */
  conf->adc_file = fopen(filename, "r");
  if (!conf->adc_file) {
    error_("error reading the adc_file: \"%s\": ", filename);
    perror("");
    error_("QUITTING!\n");
    return -1;
  }
  debug(1,"file \"%s\" found and opened.\n", filename);
  
  /* get the size of the file */
  st = (struct stat *) malloc_(sizeof(struct stat));
  stat(filename, st);
  debug(3, "size of the file: %d\n", st->st_size);
  conf->adc->size = st->st_size - strlen(MAGIC_SEQUENCE);
  free(st);

  /* read the magic key */
  byte_read = fread(magic_bytes, sizeof(char), strlen(MAGIC_SEQUENCE), conf->adc_file);
  if ((byte_read != strlen(MAGIC_SEQUENCE)) ||
      (memcmp(MAGIC_SEQUENCE, magic_bytes, strlen(MAGIC_SEQUENCE)))) {
    error_("wrong file type. \"%s\" is *NOT* an ADC file!\n", filename);
    error_("QUITTING!\n");
    return -2;
  }
  debug(2, "Magic sequence found. file type is: ADC.\n");
  verbose_("[*] parsing file: \"%s\"\n", filename);
  
  
  /* position the offset after the magic sequence */
  // fseek(conf->adc_file, strlen(MAGIC_SEQUENCE), SEEK_SET);

  /* create the memory-copy of the file */
  conf->adc->buffer = malloc_(conf->adc->size); /* don't forget to free at the end (fuzzer.c) */
  byte_read = fread(conf->adc->buffer, sizeof(char), conf->adc->size, conf->adc_file);
  debug(3, "read bytes: %d\n", byte_read);

  /* init the end of the structure */
  conf->adc->start  = 0;
  conf->adc->offset = 0;

  /* we don't need the file anymore */
  fclose(conf->adc_file);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return */
  return 0;
}


/*---------------------------------------------------------------------------*                                            
 * NAME: file_fuzz
 * DESC: create a file for the current fuzzing structure
 * RETN:  0 if ok
 *       -1 if error
 *---------------------------------------------------------------------------*/
int file_fuzz(config *conf) {

  int result;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* create the fuzzed file */
  if (create_fuzz_file(conf))
    return -1;

  /* parse the content of the fuzz script */
  result = parse_chrono(conf);
  debug(1, "result of parse_chrono file: %d\n", result);
  
  /* close the fuzzed file */
  if (close_fuzz_file(conf))
    return -1;

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  return result;
}

/*---------------------------------------------------------------------------*                                            
 * NAME: check_fuzz_source
 * DESC: this function is used to check if an entry of the index_source which
 *       contains a fuzz_source *is* a valid file (exists etc.)
 * RETN: return >0 size of the file
 *       return  0 do continue (used in fill_source - useless line, skip)
 *       return -1 if Error (quit)
 *---------------------------------------------------------------------------*/
int check_fuzz_source(unsigned char *temp) {
  FILE *test_file;

  struct stat my_stat;
  struct stat *st;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* init */
  st = &my_stat;
  
  debug(3, "check the fuzz file: \"%s\"\n", temp);
  
  /* fill the stat structure */
  if (stat(temp, st)) {
    error_("source file: \"%s\": ", temp);
    perror("");
    verbose_("[!] source file: \"%s\" is ignored.\n", temp);
    return 0; /* continue */
  }
  
  /* check if it's a regular file */
  if (S_ISREG(st->st_mode) == 0) {
    error_("error with file: \"%s\": Not a regular file\n", temp);
    error_("ignoring this file\n");
    verbose_("fuzzing source file: \"%s\" is ignored (see stderr)\n", temp);
    return 0; /* continue */
  }
  debug(3, "check: \"%s\" REGULAR FILE: OK\n", temp);
  
  /* check if the size is not NULL */
  if (st->st_size == 0) {
    error_("error with file: \"%s\": size is 0 bytes\n", temp);
    error_("ignoring this file\n");
    verbose_("fuzzing source file: \"%s\" is ignored (see stderr)\n", temp);
    return 0; /* continue */
  }
  debug(3, "check: \"%s\" SIZE        : OK  (%d)\n", temp, st->st_size);
  
  /* finally try to open the file */
  test_file = fopen(temp, "r");
  if (!test_file) {
    error_("error opening file: \"%s\": ", temp);
    perror("");
    error_("ignoring this file\n");
    verbose_("fuzzing source file: \"%s\" is ignored (see stderr)\n", temp);
    return 0; /* continue */
  }
  debug(3, "check: \"%s\" OPENING FILE: OK\n", temp);
  
  /* close the index file */
  if (fclose(test_file)) {
    error_("error when closing the file: \"%s\"\n", temp);
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  return st->st_size;
}

/*---------------------------------------------------------------------------*                                            
 * NAME: check_index_source
 * DESC: this function check the filename "filename" if it's a good index 
 *       source. i.e. a list of filename beginning with '/' which exist
 *       comments are supported (with '#') for example a valid index_source:
 *       
 *       /usr/local/etc/autodafe/string/255xA # this is 255 x 'A'
 * 
 * RETN: return  0 ok
 *       return  1 do continue
 *       return -1 if Error (quit)
 *---------------------------------------------------------------------------*/
int check_index_source(unsigned char *filename, unsigned char *temp, unsigned int temp_size) {
  int i;
  int j;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* debug */
  debug(3, "(filename ): \"%s\"\n", filename);
  debug(3, "(temp     ): \"%s\"\n", temp);
  debug(3, "(temp_size): \"%d\"\n\n", temp_size);


  /* check if the line is bigger than 4096 */
  if (strlen(temp) == temp_size - 1) {
    error_("size of the file too long. (max : %d)\n", temp_size);
    error_("QUITTING!\n");
      return -1; /* error */
  }
  
  /* bypass the spaces and tab of the line */
  for(i=0;i<strlen(temp);i++)
    if ((temp[i] != 0x20) &&  /* space */
	(temp[i] != 0x09)) {  /* \t    */
      debug(3, "space or tab detected, ignore it\n");
      break;
    }
  
  /* empty line */
  if (i == strlen(temp)) {
    debug(3, "empty line, ignore it\n");
    return 1; /* continue */
  }
  
  /* first char */
  debug(3, "the first ascii char of the line is: 0x%02x (%c)\n", temp[i], temp[i]);
  
  /* check comment */
  if (temp[i] == '#') {
    debug(3, "comment detected, ignore the line\n");
    return 1; /*continue */
  }
  
  /* check something else than '/' */
  if (temp[i] != '/') {
    return 1; /* continue */
  }

  /* remove any other comment by '\0' */
  for(i=0;i<strlen(temp);i++) {
    if (temp[i] == '#') {
      debug(3, "comment detected, replaced by NULL\n");
      temp[i] = '\0';
      
      /* remove the space and tab before the '#' */
      for(j=i-1;j>0;j--) {
	debug(3, "temp[j]: 0x%02x\n", temp[j]);
	if ((temp[j] == 0x20) || (temp[j] == 0x09)) {
	  debug(3, "remove extra space\n");
	    temp[j] = '\0';
	}
	else
	  break;
      }
    }
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  return 0;
}


/*---------------------------------------------------------------------------*                                            
 * NAME: fill_source
 * DESC: check every fuzz files (index source and fuzz source) to see if
 *       everything is ok. Normally there is no other check, so don't change
 *       them during the fuzz ;-)
 * RETN: return  number of line (exploitable) of the file
 *       return -1 if Error (quit)
 *---------------------------------------------------------------------------*/
int fill_source(struct struct_fuzz *fuzz, unsigned char *filename) {
  
  FILE *index_file;
  struct stat my_stat;
  struct stat *st;

  unsigned char temp[FILENAME_MAX]; /* 4096 in Linux */
  int id = 0;

  unsigned char *result;
  int result_int;

  struct struct_source *source;
  struct struct_source *prev = NULL;
  
  /* debug */
  debug(1, "<-----------------------[enter]\n");


  /* init */
  bzero(temp, sizeof(temp));
  st = &my_stat;

  /* open the file filename */
  index_file = fopen(filename, "r");
  if (!index_file) {
    error_("error reading source file: \"%s\": ", filename);
    perror("");
    error_("QUITTING!\n");
    return -1;
  }
  
  debug(1, "source file: \"%s\" found and opened.\n", filename);
  
  /* MAX size of a line is FILENAME_MAX */
  while (fgets(temp, sizeof(temp), index_file) != 0) {

    /* check and parse the index source file */
    result_int = check_index_source(filename, temp, sizeof(temp));
    if (result_int == -1) return -1;
    if (result_int ==  1) continue;
    
    debug(3, "line seems to be good, try to parse it: \"%s\"\n", temp);

    /* remove the last char if there is a \n */
    if (temp[strlen(temp) -1] == '\n') {
      temp[strlen(temp) -1] = '\0';
    }

    
    /* check and parse the fuzz source file */
    result_int = check_fuzz_source(temp);
    if (result_int == -1) return -1;
    if (result_int ==  0) continue;

    /* the file seems to be correct, we add it */
    debug(1, "adding file: %s (size: %d)\n", temp, strlen(temp));
    
    /* create a source structure - FREED AT THE END - see engine.c */
    source = malloc_(sizeof(struct struct_source));
    
    /* init - thanks to valgrind! I love you valgrind! */
    bzero(source, sizeof(struct struct_source));

    /* update the previous entry */
    if (prev) prev->next = source;
    
    /* update if it's the first entry */
    if (!fuzz->source) fuzz->source = source;
    
    /* malloc the buffer for the filename - DON'T FORGET TO FREE - see engine.c */
    result = malloc_(strlen(temp)+1);
    
    /* copy the string */
    memcpy(result, temp, strlen(temp)+1);
    
    /* init source->filename */
    source->filename = result;
    
    /* init the id */
    source->id = id++;
    
    /* init the size of the file */
    source->size = result_int;
    
    /* verbose */
    verbose_("[!] source: \"%s\"\t(%d bytes)\n", source->filename, source->size);
    
    /* update prev entry */
    prev = source;
    
    /* bzero the temp string */
    bzero(temp, sizeof(temp));
    debug(3, "*END*\n");
  }

  /* close the index file */
  if (fclose(index_file)) {
    error_("error when closing the file: \"%s\"\n", filename);
    perror("");
    error_("QUITTING!\n");
    return -1;
  }


  /* debug */
  debug(1, "<-----------------------[quit]\n");

  return id;
}
