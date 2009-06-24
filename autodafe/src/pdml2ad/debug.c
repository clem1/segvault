/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Jun.2004 ---*                                          
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : debug.c                                                                                                 
 * DESCRIPTION: Define debug_, malloc_, verbose_      
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>  /* malloc */

#include "debug.h"
#include "conf.h"

extern unsigned int debug;
extern unsigned int verbose;

/*---------------------------------------------------------------------------*                                            
 * NAME: debug_
 * DESC: Print the message with the name of the file.c and the line. The
 *       number of debug level is defined by debug_level (extern value)
 *---------------------------------------------------------------------------*/
void debug_(int level, char *file, int line, const char *func, const char *fmt, ...) {
  char buf[1024];
  va_list args;
  
  if (level <= debug) {
    va_start(args, fmt);
    vsnprintf(buf, 1024, fmt, args);
    va_end(args);

    if (level == 1)
      fprintf(stderr, "[D]->%s:%d:%s():%s", file, line, func, buf);
    else
      fprintf(stderr, "[D] %s", buf);
  }
}

/*---------------------------------------------------------------------------*                                            
 * NAME: malloc_
 * DESC: Verify if the system is able to alloc this size of memory
 *---------------------------------------------------------------------------*/
void * malloc_(size_t size) {
  void *result;
  
  result = malloc(size);
  if (result == NULL) {
    perror("malloc_");
    exit(-2);
  }
  return result;
}

/*---------------------------------------------------------------------------*                                            
 * NAME: verbose_
 * DESC: Print user-friendly messages.
 *---------------------------------------------------------------------------*/
void verbose_(char *fmt, ...) {
  char buf[1024];
  va_list args;
  
  if (verbose) {
    va_start(args, fmt);
    vsnprintf(buf, 1024, fmt, args);
    va_end(args);
    fprintf(stderr, "%s", buf);
  }
}

/*---------------------------------------------------------------------------*                                            
 * NAME: error_
 * DESC: Print user-friendy error message
 *---------------------------------------------------------------------------*/
void error_(char *fmt, ...) {
  char buf[1024];
  va_list args;
  
  va_start(args, fmt);
  vsnprintf(buf, 1024, fmt, args);
  va_end(args);
  fprintf(stderr, "[E] %s", buf);
}

