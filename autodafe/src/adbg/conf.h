/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : conf.h
 * DESCRIPTION: All the configuration values
 *---------------------------------------------------------------------------*/

#define MIN_ARGS 1            // number of min args before print usage 
#define MAX( a , b ) (( a ) > ( b ) ? ( a ) : ( b ))

struct struct_bp {
  struct struct_bp * next;    /* linked list */
  unsigned int id;            /* id of the breakpoint */
  unsigned int type;          /* type of the esp i.e. 0-> addr, 1-> string */
  unsigned int esp;           /* gap between useful value and esp e.g. strcpy = 0x8 */
  char *name;                 /* name of the function */
  unsigned int addr;          /* address of the function */
} bp;

struct struct_string {
  struct struct_string *next; /* linked list */
  unsigned int id;            /* id of the string (from the fuzzer) */
  unsigned char *string;      /* buffer containing a string */
} string;

typedef struct configuration {
  unsigned int pid;             /* pid of the debugged process */
  char *gdb_exe;                /* the gdb execution string */
  char *input;                  /* the file used as an string input */
  FILE *f_input;                /* input file */
  char *program;                /* pointer on the program name */
  FILE *f_gdb_dump;             /* gdb dump file */
  char *gdb_dump;               /* name of the gdb dump file */ 
  FILE *f_string_dump;             /* string dump file */
  char *string_dump;               /* name of the string dump file */ 
  char *args;                   /* pointer on the args of the program */
  int master;                   /* master of the pseudo terminal */
  int slave;                    /* slave of the pseudo terminal */
  unsigned char * gdb_buf;      /* buffer containing the output of gdb */
  struct struct_bp *bp;         /* linked list of the breakpoints */
  struct struct_string *string; /* linked list of the recv. strings */
  unsigned short port;          /* listening port (fuzzer connection) */
  int fuzzer_socket;            /* socket (fuzzer connection) */
} config;

unsigned int verbose; /* shared value for debug.c */
unsigned int debug;   /* shared value for debug.c */
