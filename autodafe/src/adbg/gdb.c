/*---------------------------------------------------------------------------*                                          
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*                                          
 *---                                                            Aug.2004 ---*                                          
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*                                          
 * NAME       : gdb.c                                                                                                 
 * DESCRIPTION: All the functions related to gdb
 *---------------------------------------------------------------------------*/

#include <stdio.h>     /* perror */
#include <stdarg.h>
#include <stdlib.h>    /* malloc */
#include <pty.h>       /* openpty */
#include <strings.h>   /* bzero */
#include <sys/types.h> /* fork */
#include <unistd.h>    /* fork, read, write */
#include <strings.h>   /* bzero */
#include <string.h>    /* strncmp */
#include <sys/socket.h>/* shutdown */
#include <signal.h>    /* signal */

#include "debug.h"
#include "conf.h"
#include "network.h"
#include "proto.h"
#include "../../config.h" /* for the path of gdb */


#define GDB_BUF_SIZE 1024*1024
#define GDB_ARGS " --interpreter=mi -tty="
#define GDB_PID  " -p "

#define GDB_LINE_PROMPT     0x01 
#define GDB_LINE_TILDA      0x02
#define GDB_LINE_DONE       0x03
#define GDB_LINE_BP         0x04
#define GDB_LINE_RUNNING    0x05
#define GDB_LINE_ERROR      0x06
#define GDB_LINE_AND        0x07
#define GDB_LINE_EXIT       0x08
#define GDB_LINE_SIGSEGV    0x09
#define GDB_LINE_SIGNAL     0x0a

#define GDB_LINE_PROMPT_S     "(gdb) \r\n" /* DON'T FORGET THE F*CKING SPACE ;-) */ 
#define GDB_LINE_TILDA_S      "~"
#define GDB_LINE_DONE_S       "^done"
#define GDB_LINE_BP_S         "*stopped,reason=\"breakpoint-hit\""
#define GDB_LINE_RUNNING_S    "^running"
#define GDB_LINE_ERROR_S      "^error"
#define GDB_LINE_AND_S        "&"
#define GDB_LINE_EXIT_S       "*stopped,reason=\"exited-normally\""
#define GDB_LINE_SIGSEGV_S_1  "*stopped,reason=\"signal-received\",signal-name=\"SIGSEGV\""
#define GDB_LINE_SIGSEGV_S_2  "^done,reason=\"signal-received\",signal-name=\"SIGSEGV\""
#define GDB_LINE_SIGNAL_S_1   "*stopped,reason=\"signal-received\",signal-name="
#define GDB_LINE_SIGNAL_S_2   "^done,reason=\"signal-received\",signal-name="

#define GDB_ADDR    0x00
#define GDB_STRING  0x01
#define GDB_FORMAT  0x02


/* instance of conf */
config *the_conf;

/* instancing gdb_write_line (loop) */
void gdb_write_line(config *conf, char *fmt, ...);

/*---------------------------------------------------------------------------*                                            
 * DESC: quit gdb, free the basic structures and quit
 * RETN: void
 *---------------------------------------------------------------------------*/
void this_is_the_end(config *conf) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* close the communication with the fuzzer */
  if (conf->fuzzer_socket > 0) {
    shutdown(conf->fuzzer_socket, SHUT_RDWR);
    close(conf->fuzzer_socket);
    conf->fuzzer_socket = -1;
    debug(3, "[socket with fuzzer]: closed\n");
  }

  /* quit gdb (we juste kill) */
  
  /* close the communication between the pty (closed by the kill) */

  /* free the struct_bp structure */
  if (conf->bp) {
    struct struct_bp *bp;
    struct struct_bp *bp_to_free;
    bp = conf->bp;
    while(bp) {
      bp_to_free = bp;
      bp = bp->next;
      debug(3, "free bp[%d]: %s\n", bp_to_free->id, bp_to_free->name);
      free(bp_to_free->name);
      free(bp_to_free);
    }
    conf->bp = NULL;
  }

  /* free the struct_string structure */
  if (conf->string) {
    struct struct_string *string;
    struct struct_string *string_to_free;
    string = conf->string;
    while(string) {
      string_to_free = string;
      string = string->next;
      debug(3, "free string[%d]: %s\n", string_to_free->id, string_to_free->string);
      free(string_to_free->string);
      free(string_to_free);
    }
    conf->string = NULL;
  }



  /* close the input file */
  if (conf->f_input) fclose(conf->f_input);

  /* close the gdb dump file */
  if (conf->f_gdb_dump) fclose(conf->f_gdb_dump);

  /* close the gdb dump file */
  if (conf->f_string_dump) fclose(conf->f_string_dump);

  /* free the gdb's buffer */
  if (conf->gdb_buf) free(conf->gdb_buf); conf->gdb_buf = NULL;

  /* free the gdb_exe string */
  free(conf->gdb_exe);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* free the configuration structure */
  free(conf);

  /* exited normally */
  exit(0);
}

/*---------------------------------------------------------------------------*                                            
 * DESC: catch CTRL-C and free everything before quitting
 * RETN: void
 *---------------------------------------------------------------------------*/
void sigint_handler(int sig) {

  printf("[CTRL-C]\n");
  this_is_the_end(the_conf);

}



/*---------------------------------------------------------------------------*                                            
 * DESC: read until a '\n' is received by gdb
 * RETN: void
 *---------------------------------------------------------------------------*/
void gdb_read_line(config *conf) {

  char byte;
  unsigned int counter = 0;

  /* debug */
  debug(1, "<-----------------------[enter]\n");
  debug(3, "[gdb read buffer dump]\n");
  /* init the buffer */
  bzero(conf->gdb_buf, GDB_BUF_SIZE);

  /* read a line from the slave */
  do {
    byte = read(conf->master, conf->gdb_buf + counter, sizeof(char));
    if (byte != 1) {
      error_("gdb_read_line (with gdb): ");
      perror("");
      error_("QUITTING!\n");
      exit(-1);
    }
    byte = *(conf->gdb_buf + counter);
    
    /* mega debug */
    debug(3, "0x%02x (%c)\n", byte, byte);

    counter++;

    /* check if the max size of the buffer is not overflowed (it happen with fuzzing */
    if (counter >= GDB_BUF_SIZE) {
      error_("gdb buffer overflowed! increase the GDB_BUF_SIZE in file gdb.c\n");
      error_("actual size: %d\n", GDB_BUF_SIZE);
      error_("QUITTING!\n");
      exit(-31337);
    }
  } while (byte != '\n');

  /* verbose */
  //  verbose_("[GDB]: %s", conf->gdb_buf);

  /* dump */
  if (conf->gdb_dump) {
    fprintf(conf->f_gdb_dump, "%s", conf->gdb_buf);

    /* if not closed, it's better to have something */
    fflush(conf->f_gdb_dump);
  }
  
  /* debug */
  debug(1, "<-----------------------[quit]\n");

}


/*---------------------------------------------------------------------------*                                            
 * DESC: read a value using the interface/mi ie : value="yodel"
 * RETN: char * on the "yodel".
 * WARN: don't forget to free the buffer after.
 *       if the string is not found, fatal error so be careful! 
 *---------------------------------------------------------------------------*/
char * gdb_look_for(config *conf, char * value) {
  
  char *result;
  char *begin;
  char *end;
  
  /* debug */
  debug(1, "<-----------------------[quit]\n");
  
  /* check the first occurence of the value */
  begin = strstr(conf->gdb_buf, value);
  
  /* check (should never happen) */
  if (begin == NULL) {
    error_("cannot find \"%s\" in \"%s\"", value, conf->gdb_buf);
    error_("QUITTING!\n");
    exit(-1);
  }
  
  /* locate the first '"' */
  begin = strchr(begin, '\"');
  
  /* check (should never happen) */
  if (begin == NULL) {
    error_("cannot find the first '\"' in \"%s\"", value, conf->gdb_buf);
    error_("QUITTING!\n");
    exit(-1);
  }
  
  /* locate the second '\"' */
  end = strchr(begin+1, '\"');
  
  /* remove if it's a "\"" and not a '\"' */
  while (*(end - 1) == '\\') {
    end = strchr(end + 1, '\"');
  }

  /* check (should never happen) */
  if (begin == NULL) {
    error_("cannot find the last '\"' in \"%s\"", value, conf->gdb_buf);
    error_("QUITTING!\n");
    exit(-1);
  }

  /* malloc buffer */
  result = malloc_(end - begin);
  
  /* copy the value (remove the quotes) */
  memcpy(result, begin + 1, end - begin - 1);
  
  /* add the null byte */
  result[end - begin - 1] = '\0';
  
  /* debug */
  debug(2, "[value]: %s=\"%s\"\n", value, result);
  debug(1, "<-----------------------[quit]\n");
  
  /* everything is ok (don't forget to free the buffer! */
  return result;
  
}



/*---------------------------------------------------------------------------*                                            
 * DESC: what to do if we have a segmentation fault
 * RETN: void error are fatal
 *---------------------------------------------------------------------------*/
void gdb_event_sigsegv(config *conf) {

  unsigned int eip;
  char *result;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* check the address */
  result = gdb_look_for(conf, "addr"); /* don't forget to free */
  eip =  strtol(result, NULL, 16);
  printf("[***] EIP: %p\n", (void *) eip);

  /* free */
  free(result);

  /* send a message to the fuzzer */
  if (conf->port) inet_send_msg(conf, INET_SIGSEGV_MSG, 0);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}



/*---------------------------------------------------------------------------*                                            
 * DESC: give the type of a gdb's line
 * RETN: the type (int):
 *       0x1 (GDB_LINE_PROMPT)    : (gdb) \r\n
 *       0x2 (GDB_LINE_TILDA)     : ~<something>
 *       0x3 (GDB_LINE_DONE)      : ^done (must be checked after SIGSEGV)
 *       0x4 (GDB_LINE_BP)        : *stopped,reason="breakpoint-hit"
 *       0x5 (GDB_LINE_RUNNING)   : ^running
 *       0x6 (GDB_LINE_ERROR)     : ^error
 *       0x7 (GDB_LINE_AND)       : &<something>
 *       0x8 (GDB_LINE_EXIT)      : *stopped,reason="exited-normally"
 *       0x9 (GDB_LINE_SIGSEGV)   : *stopped,reason="signal-received",signal-name="SIGSEGV"
 *                                  ^done,reason="signal-received",signal-name="SIGSEGV"
 *       0xa (GDB_LINE_SIGNAL)    : *stopped,reason="signal-received",signal-name= 
 *        0 if we close normally the program
 *       -1 if unknown
 *---------------------------------------------------------------------------*/
int gdb_type_line(config *conf) {

  int result;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  if (strncmp(conf->gdb_buf, GDB_LINE_PROMPT_S, strlen(GDB_LINE_PROMPT_S)) == 0) {
    result = GDB_LINE_PROMPT;
    debug(2, "[line_type]: prompt\n");
  }
  else if (strncmp(conf->gdb_buf, GDB_LINE_TILDA_S, strlen(GDB_LINE_TILDA_S)) == 0) {
    result = GDB_LINE_TILDA;
    debug(2, "[line_type]: comment (tilda)\n");
  }

  /* check if it's a segmentation fault (asynchrone) */
  else if ((strncmp(conf->gdb_buf, GDB_LINE_SIGSEGV_S_1, strlen(GDB_LINE_SIGSEGV_S_1)) == 0) ||
	   (strncmp(conf->gdb_buf, GDB_LINE_SIGSEGV_S_2, strlen(GDB_LINE_SIGSEGV_S_2)) == 0)) {
    
    result = GDB_LINE_SIGSEGV;
    debug(2, "[line_type]: SEGMENTATION FAULT!\n");
    
    /* parsed here because asynchrone */
    printf("[***] SEGMENTATION FAULT!\n");
    
    /* print informations */
    gdb_event_sigsegv(conf);
    
    /* debug */
    debug(1, "<-----------------------[quit]\n");
    
    /* stop we don't know where we are so we free and close */
    this_is_the_end(conf);
    
  }

  /* check if it's another signal (asynchrone) */
  else if ((strncmp(conf->gdb_buf, GDB_LINE_SIGNAL_S_1, strlen(GDB_LINE_SIGNAL_S_1)) == 0) ||
	   (strncmp(conf->gdb_buf, GDB_LINE_SIGNAL_S_2, strlen(GDB_LINE_SIGNAL_S_2)) == 0)) {
    result = GDB_LINE_SIGNAL;
    debug(2, "[line_type]: signal (not segfault) ignore\n");

    /* read the next line which is a prompt */
    gdb_read_line(conf);

    /* write continue */
    gdb_write_line(conf, "-exec-continue\n");

  }

  else if (strncmp(conf->gdb_buf, GDB_LINE_DONE_S, strlen(GDB_LINE_DONE_S)) == 0) {
    result = GDB_LINE_DONE;
    debug(2, "[line_type]: done\n");
  }
  else if (strncmp(conf->gdb_buf, GDB_LINE_BP_S, strlen(GDB_LINE_BP_S)) == 0) {
    result = GDB_LINE_BP;
    debug(2, "[line_type]: breakpoint\n");
  } 
  else if (strncmp(conf->gdb_buf, GDB_LINE_RUNNING_S, strlen(GDB_LINE_RUNNING_S)) == 0) {
    result = GDB_LINE_RUNNING;
    debug(2, "[line_type]: running\n");
  }
  else if (strncmp(conf->gdb_buf, GDB_LINE_ERROR_S, strlen(GDB_LINE_ERROR_S)) == 0) {
    result = GDB_LINE_ERROR;
    debug(2, "[line_type]: error\n");

    /* parsed here because asynchrone */
    error_("gdb error: %s", conf->gdb_buf);
    error_("QUITTING!\n");
    
    /* debug */
    debug(1, "<-----------------------[quit]\n");
    
    /* we don't know where we are so we free and close */
    this_is_the_end(conf); 
  }
  else if (strncmp(conf->gdb_buf, GDB_LINE_AND_S, strlen(GDB_LINE_AND_S)) == 0) {
    result = GDB_LINE_AND;
    debug(2, "[line_type]: &\n");
    
  } 
  
  else if (strncmp(conf->gdb_buf, "&\"ptrace: No such process.\n\"", strlen("&\"ptrace: No such process.\n\"")) == 0) {
    result = -1;
    if (conf->pid) {
      error_("no process found with the pid: %d\n", conf->pid);
      error_("QUITTING!\n");
      exit(-1);
    }
  } 

  else if (strncmp(conf->gdb_buf, GDB_LINE_EXIT_S, strlen(GDB_LINE_EXIT_S)) == 0) {
    result = GDB_LINE_EXIT;
    debug(2, "[line_type]: exit-normally\n");

    /* parsed here because asynchrone */
    verbose_("[*] program exited normally.\n");
    verbose_("[*] stop the debugger.\n");

    /* send the exit message to the debugger */
    if (conf->port) inet_send_msg(conf, INET_EXIT_MSG, 0);

    /* debug */
    debug(1, "<-----------------------[quit]\n");

    /* we don't know where we are so we free and close */
    this_is_the_end(conf);

  } 

  else {
    /* unknown line probably output of the software, we check the next */
    debug(2, "unknown gdb line: %s", conf->gdb_buf);
    result = -1;
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* return result */
  return result;

}

/*---------------------------------------------------------------------------*                                            
 * DESC: wait for a special type of message (cf read_type_line)
 * RETN: void
 *---------------------------------------------------------------------------*/
void gdb_wait_for(config *conf, unsigned int type) {

  /* debug */
  debug(1, "<-----------------------[enter]\n");
  debug(2, "[type]: %d\n", type);

  while (gdb_type_line(conf) != type) {
    gdb_read_line(conf);
  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");
  
}

/*---------------------------------------------------------------------------*                                            
 * DESC: write a string on gdb
 * RETN: void
 *---------------------------------------------------------------------------*/
void gdb_write_line(config *conf, char *fmt, ...) {
  
  char cmd[1024];
  va_list args;
  unsigned bytes;
  int i;

  
  /* debug */
  debug(1, "<-----------------------[enter]\n");

  bzero(cmd, sizeof(cmd));
  va_start(args, fmt);
  vsnprintf(cmd, 1024, fmt, args);
  va_end(args);
  
  /* wait for a prompt */
  gdb_wait_for(conf, GDB_LINE_PROMPT);

  /* write the command */
  bytes = write(conf->master, cmd, strlen(cmd));
  
  /* check */
  if (bytes != strlen(cmd)) {
    error_("gdb_write_line: ");
    perror("");
    error_("QUITTING!\n");
    exit(-1);
  }
  /* debug */
  debug(3, "[gdb write buffer dump]\n");
  debug(3, "gdb> %s\n", cmd);
  debug(1, "<-----------------------[quit]\n");
  
}



/*---------------------------------------------------------------------------*                                            
 * DESC: insert a breakpoint and fill the conf->break linked list.
 *       esp : the offset for an interresting value from $esp.
 *       name: the name of the function. 
 *       type: if the interresting value is a value or a string:
 *             GDB_STRING 0x01
 *             GDB_ADDR   0x00
 * RETN: void (quit if there is an error)
 *---------------------------------------------------------------------------*/
void gdb_break(config *conf, char *name, unsigned int esp, unsigned int type) {

  struct struct_bp *bp;
  struct struct_bp *prev = NULL;
  unsigned int size;
  char *result;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* check if the fonction's name isn't already in the linked list */
  for(bp = conf->bp;bp;bp = bp->next) {
    prev = bp;

    /* compare with the biggest name */
    size = MAX(strlen(bp->name), strlen(name));
    if (strncmp(bp->name, name, size) == 0) {
      error_("function \"%s\" has already a breakpoint\n", name);
      return;
    }
  }

  /* malloc the structure */
  bp = malloc_(sizeof(struct struct_bp));

  /* init the structure */
  bp->next = NULL;
  bp->name = malloc_(strlen(name)+1); /* don't forget to free */
  strncpy(bp->name, name, strlen(name));
  bp->name[strlen(name)] = '\0';

  /* update the previous entry */
  if (prev) prev->next = bp;


  /* update if it's the first entry */
  if (!conf->bp) conf->bp = bp;

  /* send to gdb */
  gdb_write_line(conf, "-break-insert *%s\n", name);

  /* wait the done */
  gdb_wait_for(conf, GDB_LINE_DONE);
  
  /* read "number" value */
  result = gdb_look_for(conf, "number"); /* don't forget to free */
  bp->id = strtol(result, NULL, 10);
  free(result); result = NULL;

  /* read "addr" value */
  result = gdb_look_for(conf, "addr");
  bp->addr = strtol(result, NULL, 16);
  free(result); result = NULL;
  
  /* copy the esp value in the linked list */
  bp->esp = esp;

  /* copy the type value */
  bp->type = type;

  /* verbose */
  verbose_ ("[*]-+-> breakpoint on: \"%s\"\n", name);
  verbose__("    +-> id  : %d\n", bp->id);
  verbose__("    +-> addr: %p\n", bp->addr);
  verbose__("    +-> esp : 0x%02x\n", bp->esp);
  verbose__("    +-> type: 0x%02x\n", bp->type);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

}



/*---------------------------------------------------------------------------*                                            
 * DESC: parse the initialization of gdb
 * RETN: 0 if everything is ok
 *      -1 if error (fatal)
 *---------------------------------------------------------------------------*/
int gdb_init(config *conf) {

   /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* create the line buffer (big because we fuzz ;-) (don't forget to free) */
  conf->gdb_buf = malloc_(GDB_BUF_SIZE);

  /* init the buffer (thanks to valgrind) */
  bzero(conf->gdb_buf, GDB_BUF_SIZE);

  /* wait for a prompt */
  gdb_wait_for(conf, GDB_LINE_PROMPT);

  /* set the heigh to zero (no "type <return> to...") */
  gdb_write_line(conf, "set heigh 0\n");

  /* wait for a prompt */
  gdb_wait_for(conf, GDB_LINE_DONE);

  /* wait for a prompt */
  gdb_wait_for(conf, GDB_LINE_PROMPT);

  /* set the pending to yes (if libary not resolved yet, try during the run */
  gdb_write_line(conf, "set breakpoint pending on\n");

  /* wait for a prompt */
  gdb_wait_for(conf, GDB_LINE_DONE);

  /* wait for a prompt */
  gdb_wait_for(conf, GDB_LINE_PROMPT);

  /* if we launch a program */
  if (conf->program) {

    /* break on _init to resolve pending breakpoints (for stripped files) */ 
    /* BUG1: You cannot set breakpoint pending on using "-break-insert"
             that's why we use "break".
       BUG2: You cannot break *_init with pending on, that's why we
             break on "_init". It's not very important, it's just to resolve
             library names. It's a little hack, but... who cares :-) */

    // TODOXXXFIXMEXXX
        gdb_write_line(conf, "break _init\n");
        gdb_wait_for(conf, GDB_LINE_DONE);
        gdb_wait_for(conf, GDB_LINE_PROMPT);

    //gdb_break(conf, "_init", 0, 0);

    /* run the program */
    if (conf->args)
      gdb_write_line(conf, "-exec-run %s\n", conf->args);
    else
      gdb_write_line(conf, "-exec-run\n");  
  }
  
  // TODOXXXFIXMEXXX add the vuln functions */
  /* string (stack/heap overflow based) */
  gdb_break(conf, "strcpy", 0x08, GDB_STRING); /* $esp+8 and string value */
  gdb_break(conf, "strcat", 0x08, GDB_STRING); /* $esp+8 and string value */
  gdb_break(conf, "gets", 0x04, GDB_STRING);   /* $esp+4 and string value */
  gdb_break(conf, "sprintf", 0x08, GDB_STRING);   /* $esp+8 and string value */
  gdb_break(conf, "getenv", 0x04, GDB_STRING);   /* $esp+4 and string value */
  gdb_break(conf, "stpcpy", 0x08, GDB_STRING);   /* $esp+8 and string value */
  gdb_break(conf, "memcpy", 0x08, GDB_STRING);   /* $esp+8 and string value */
  gdb_break(conf, "memccpy", 0x08, GDB_STRING);   /* $esp+8 and string value */
  gdb_break(conf, "bcopy", 0x04, GDB_STRING);   /* $esp+4 and string value */
  gdb_break(conf, "memmove", 0x08, GDB_STRING);   /* $esp+8 and string value */

  /* format string overflow based */
  gdb_break(conf, "printf", 0x04, GDB_STRING);   /* $esp+4 and string value */
  gdb_break(conf, "syslog", 0x04, GDB_STRING);   /* $esp+4 and string value */
  
  /* open the connection with the fuzzer */
  if (conf->port) inet_connection(conf);

  /* wait for a prompt */
  gdb_wait_for(conf, GDB_LINE_PROMPT);

  /* send "continue */
  gdb_write_line(conf, "-exec-continue\n");

  /* wait for a "^running" */
  gdb_wait_for(conf, GDB_LINE_RUNNING);

  /* wait for a prompt */
  gdb_wait_for(conf, GDB_LINE_PROMPT);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* everything is ok */
  return 0;
}


/*---------------------------------------------------------------------------*                                            
 * DESC: We compare the string of a function (bp->esp) with our string
 *       database. If it's the same, send message to the fuzzer.
 * RETN: void error are fatal
 *---------------------------------------------------------------------------*/
void gdb_break_string(config *conf, struct struct_bp *bp) {

  char *result;
  struct struct_string *string;
  

  /* debug */
  debug(1, "<-----------------------[enter]\n");
  
  /* write the string */
  gdb_write_line(conf, "printf \"%%s\\n\", *((char**)($esp+%d))\n", bp->esp);
  
  /* read the result (start with '~') and put it in a buffer */
  gdb_wait_for(conf, GDB_LINE_TILDA);
  
  do {
    /* we remove the ~" (2bytes) and at the end the \n"\r\n (5 bytes) */
    result = malloc_(strlen(conf->gdb_buf)-2-5+1);
    
    /* init */
    bzero(result, strlen(conf->gdb_buf)-2-5+1);
    
    /* copy */
    memcpy(result, conf->gdb_buf + 2, strlen(conf->gdb_buf)-2-5);
    result[strlen(conf->gdb_buf)-2-5] = '\0';
    
    verbose__("    +-> check the string: \"%s\"\n", result);
    
    /* copy the string in the string_dump file */
    if (conf->string_dump) {
      fprintf(conf->f_string_dump, "%s:%s\n", bp->name, result);
      // fprintf(conf->f_string_dump, "%s\n", result);
      
      /* if not closed, it's better to have something */
      fflush(conf->f_string_dump);
    }
    
    /* compare the string with every entries of the string linked list */
    for(string=conf->string;string;string=string->next) {
      if (strstr(result, string->string) != 0) {
	printf("[**] function: \"%s\" use string: \"%s\"\n", bp->name, string->string);
	
	/* send a message to the fuzzer */
	if (conf->port) inet_send_msg(conf, INET_WEIGHT_MSG, string->id);
	
	break;
      }
    }
    if (string == NULL) {
      // verbose__("    +-> no match\n", result);
    }
    
    /* free result */
    free(result); result = NULL;
    
    /* read the next line */
    gdb_read_line(conf);
    
  } while (gdb_type_line(conf) != GDB_LINE_DONE);
  
  /* wait for ^done */
  gdb_wait_for(conf, GDB_LINE_DONE);
  
  /* debug */
  debug(1, "<-----------------------[quit]\n");

}

/*---------------------------------------------------------------------------*                                            
 * DESC: what to do if we have a breakpoint as event
 * RETN: void error are fatal
 *---------------------------------------------------------------------------*/
void gdb_event_breakpoint(config *conf) {

  char *result;
  unsigned int bkptno;
  struct struct_bp *bp;

  /* debug */
  debug(1, "<-----------------------[enter]\n");
  
  /* we check the breakpoint id */
  result = gdb_look_for(conf, "bkptno"); /* don't forget to free */
  bkptno = strtol(result, NULL, 10);
  free(result); result = NULL;
  
  /* check the breakpoint structure */
  for(bp=conf->bp;bp;bp=bp->next) {
    if (bp->id == bkptno) break;
  }

  if (bp == NULL) {
    error_("breakpoint with the id: %d not found!\n", bkptno);
    error_("QUITTING!\n");
    exit(-1);
  }

  /* verbose */
  verbose_ ("[*]-+-> breakpoint on: \"%s\"\n", bp->name);


  /* retrieve the interesting value if initialized */
  if (bp->esp) {
    
    /* wait for a prompt */
    gdb_wait_for(conf, GDB_LINE_PROMPT);
    
    switch(bp->type) {

    case GDB_STRING:
      gdb_break_string(conf, bp);
      break;

    case GDB_ADDR:
      // TODOXXXFIXMEXXX gdb_break_addr(conf, bp);
      break;

    case GDB_FORMAT:
      // TODOXXXFIXMEXXX gdb_break_format(conf, bp);
      break;

    default:
      error_("unknown breakpoint type!\n");
      error_("QUITTING!\n");
      exit(-1);

    }
  }

  /* wait for the prompt */
  gdb_wait_for(conf, GDB_LINE_PROMPT);

  /* send "continue */
  gdb_write_line(conf, "-exec-continue\n");

  /* wait for a "^running" */
  gdb_wait_for(conf, GDB_LINE_RUNNING);

  /* wait for a prompt */
  gdb_wait_for(conf, GDB_LINE_PROMPT);

  /* debug */
  debug(1, "<-----------------------[quit]\n");
}

/*---------------------------------------------------------------------------*                                            
 * DESC: parse the event sent by the debugger and send them to the fuzzer
 * RETN: 0 if everything is ok
 *      -1 if error (fatal)
 *       1 exit (normal)
 *---------------------------------------------------------------------------*/
int gdb_event(config *conf) {

  unsigned int type;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* read the next line */
  gdb_read_line(conf);

  /* now we don't know what can happen. If it's exited-normally, finish program
     if it's segmentation fault, print $eip and finish program (both asynchrone) */
  type = gdb_type_line(conf);

  /* if breakpoint (can be parsed normally) */
  if (type == GDB_LINE_BP) gdb_event_breakpoint(conf);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* everything is ok */ 
  return 0;

}
/*---------------------------------------------------------------------------*                                            
 * DESC: endless loop used to parse the gdb's data
 * RETN: 0 if everything is ok
 *      -1 if error (fatal)
 *       1 exit (normal)
 *---------------------------------------------------------------------------*/
int gdb_loop(config *conf) {

  int result;

  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* instance of the conf (used by the CTRL-C catcher) */
  the_conf = conf;

  /* catch the CTRL-C signal to close properly (i.e. free) */
  signal(SIGINT, sigint_handler);

  /* initialize the debugger gdb */
  if(gdb_init(conf)) return -1;

  /* main loop */
  do {
    /* parse the event and send them to the fuzzer */
    result = gdb_event(conf);
  } while (result == 0);

  /* debug */
  debug(1, "<-----------------------[quit]\n");

  /* everything is ok */
  return result;
}
/*---------------------------------------------------------------------------*                                            
 * DESC: fire up gdb using a free tty (fork used)
 * RETN: 0 if everything is ok
 *      -1 if error (fatal)
 *---------------------------------------------------------------------------*/
int gdb_core(config *conf) {

  char ttyname[256]; /* i know it's not secure ;-( */
  int pid;
  int result;
  unsigned int gdb_exe_size;
    struct struct_string *string;
    struct struct_string *prev = NULL;
    char line[512];
    unsigned int counter = 0;



  /* debug */
  debug(1, "<-----------------------[enter]\n");

  /* open the dump file of gdb */
  if (conf->gdb_dump) {
    conf->f_gdb_dump = fopen(conf->gdb_dump, "w");
    if (!conf->f_gdb_dump) {
      error_("cannot gdb dump file: \"%s\": ", conf->gdb_dump);
      perror("");
      error_("QUITTING!\n");
      return -1;
    }
  }

  /* open the string file of gdb */
  if (conf->string_dump) {
    conf->f_string_dump = fopen(conf->string_dump, "w");
    if (!conf->f_string_dump) {
      error_("cannot string dump file: \"%s\": ", conf->gdb_dump);
      perror("");
      error_("QUITTING!\n");
      return -1;
    }
  }

  /* open the .dbg file */
  if (conf->input) {
    conf->f_input = fopen(conf->input, "r");
    if (!conf->f_input) {
      error_("cannot read the input file: \"%s\": ", conf->input);
      perror("");
      error_("QUITTING!\n");
      return -1;
    }

    /* fill the string linked list structure with each lines of the .dbg file */

    /* init */
    bzero(line, 512);

    /* debug */
    debug(2, "[file opened]: %s\n", conf->input);

    /* read a line (normally, the max size is 255, so we take 512 and check 
       for overflow ;-) */
    while (fgets(line, 512, conf->f_input) != NULL) {

      if (line[511] != '\0') {
	error_("a line in the file: \"%\" is longer than 511 bytes!\n", conf->input);
	error_("you've joking right? Don't try to fuzz a fuzzer. 8^)\n");
	error_("QUITTING!\n");
	exit(-1);
      }

      /* debug */
      debug(2, "[line]: %s\n", line);

      /* counter */
      counter++;

      for(string=conf->string;string;string=string->next) {
	prev = string;
	if (strncmp(string->string, line, sizeof(line)) == 0) {
	  error_("the string is already monitored: %s\n", string);
	  goto input_next;
	}
      }

      /* create the new entry */
      string = malloc_(sizeof(struct struct_string));
      
      /* init the buffer */
      bzero(string, sizeof(struct struct_string));

      /* init the structure */
      string->next = NULL;
      string->id = 0; /* don't care in input mode */

      /* create the string buffer */
      string->string = malloc_(strlen(line)+1);

      /* init the buffer */
      bzero(string->string, strlen(line)+1);

      /* copy the string */
      strncpy(string->string, line, strlen(line));
      
      /* to be sure ;) */
      string->string[strlen(line)] = '\0';

      /* check if there is a \n at the end */
      if (string->string[strlen(line)-1] == '\n')
	string->string[strlen(line)-1] = '\0';

      /* update the previous entry */
      if (prev) prev->next = string;

      /* update if it's the first entry */
      if (!conf->string) conf->string = string;

      /* verbose */
      verbose_ ("[+]-+-> add monitored string: \"%s\"\n", string->string);
      verbose__("    +-> file: %s\n", conf->input);
      verbose__("    +-> line: %d\n", counter);

    input_next:
      continue;
    }
  }

  /* init */
  bzero(ttyname, sizeof(ttyname));

  /* open a free pty */
  if (openpty(&conf->master, &conf->slave, ttyname, NULL, NULL) != 0) {
    error_("openpty: ");
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* basic check for buffer overflow in pty */
  if (ttyname[255] != '\0') {
    error_("tty name overflowed! you've joking, right? ;-)\n");
    error_("QUITTING!\n");
    return -31337;
  }

  /* check tty */
  if (!(isatty(conf->master)) || !(isatty(conf->slave))) {
    error_("master or slave is NOT a tty\n");
    error_("QUITTING!\n");
    return -1;
  }
  
  /* creating the gdb execution string */
  if (conf->pid) {
    
    /* compute the size */
    gdb_exe_size = strlen(GDB_PATH) + strlen(GDB_ARGS) + strlen(ttyname) + strlen(GDB_PID) + 11 + 1;
    
    /* allocate buffer */
    conf->gdb_exe = malloc_(gdb_exe_size);
    
    /* init */
    bzero(conf->gdb_exe, gdb_exe_size);

    /* copy the name of gdb */
    strncpy(conf->gdb_exe, GDB_PATH, strlen(GDB_PATH));
    
    /* copy the arguments of gdb */
    strncat(conf->gdb_exe, GDB_ARGS, strlen(GDB_ARGS));

    /* copy the ttyname */
    strncat(conf->gdb_exe, ttyname, strlen(ttyname));
    
    /* copy the pid */
    strncat(conf->gdb_exe, GDB_PID, strlen(GDB_PID));
    
    
    /* parse the pid (max is -2147483647 (11 bytes)) */
    char pid_string[11+1];      
    bzero(pid_string, sizeof(pid_string));
    snprintf(pid_string, sizeof(pid_string)-1, "%d", conf->pid);
    pid_string[12] = '\0';
    strncat(conf->gdb_exe, pid_string, strlen(pid_string));
  }
  else  {  
    /* compute the size */
    gdb_exe_size = strlen(GDB_PATH) + strlen(GDB_ARGS) + strlen(ttyname) + 1 + strlen(conf->program) + 1;
    
    /* allocate buffer */
    conf->gdb_exe = malloc_(gdb_exe_size);
    
    /* init */
    bzero(conf->gdb_exe, gdb_exe_size);

    /* copy the name of gdb */
    strncpy(conf->gdb_exe, GDB_PATH, strlen(GDB_PATH));
    
    /* copy the arguments of gdb */
    strncat(conf->gdb_exe, GDB_ARGS, strlen(GDB_ARGS));
    
    /* copy the ttyname */
    strncat(conf->gdb_exe, ttyname, strlen(ttyname));
    
    /* copy the program name */
    strncat(conf->gdb_exe, " ", strlen(" "));
    strncat(conf->gdb_exe, conf->program, strlen(conf->program));
  }
  
  /* debug */
  debug(2, "[gdb_execution]: %s\n", conf->gdb_exe);
  debug(2, "[pty]: %s\n", ttyname);
  
  /* fork */
  if ((pid = fork()) == -1) {
    error_("fork: ");
    perror("");
    error_("QUITTING!\n");
    return -1;
  }

  /* Typically, a program will open the master side of a pty for
     read/write and then launch another program (e.g. a Shell) on the
     corresponding slave device. The slave program will think it's
     running on a normal tty device. Nevertheless, all the data that
     the slave writes will go to the master and all the data that the
     slave reads will really be data generated by the master. */
  
  /* PARENT (MASTER PTY) */
  if (pid) {

    /* close slave */
    close(conf->slave);

    /* start the parsing of gdb */
    result = gdb_loop(conf);

    /* kill the child */
    // TODOXXXFIXMEXXX

    /* return */
    if (result == 1) result = 0;
    return result;
  }

  /* CHILD (SLAVE PTY) */
  else {

    /* close master */
    close(conf->master);

    /* close stdin, stdout and stderr */
    close(0);
    close(1);
    close(2);

    /* set no echo (otherwise stdin<->stdout loop) */
    struct termios termio;
    ioctl(conf->slave, TCGETA, &termio);
    termio.c_lflag &= ~ECHO;        /* No echo */
    termio.c_oflag &= ~ONLCR;       /* Do not map NL to CR-NL on output */
    ioctl(conf->slave, TCSETA, &termio);

    /* redirect stdin, stdout and stderr to master */
    dup2(conf->slave, 0);
    dup2(conf->slave, 1);
    dup2(conf->slave, 2);

    /* close slave */
    if (conf->slave > 2) close(conf->slave);

    /* execute gdb */
    system(conf->gdb_exe);
  
    /* everything is ok */
    return 0;

  }

  /* debug */
  debug(1, "<-----------------------[quit]\n");


  /* everything is ok */
  return 0;
}
