/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : conf.h
 * DESCRIPTION: All the configuration values
 *---------------------------------------------------------------------------*/

#include <netinet/in.h> /* sockaddr_in */

#define MIN_ARGS 2            // number of min args before print usage 
#define MAX_DEBUG_LEVEL 3     // number of debug level 0->MAX_DEBUG_LEVEL (int)
#define DBG_TIMEOUT_SEC 1
#define DBG_TIMEOUT_USEC 0
#define DEBUGGER_FILE "debugger.info"

unsigned int verbose;      /* shared value for debug.c */
unsigned int debug;        /* shared value for debug.c */
struct sockaddr_in server; /* shared value for send/recv_fuzz and client/server_fuzz */
/*---------------------------------------------------------------------------*
 * NAME       : adc
 * DESCRIPTION: struct of the memory copy of the adc file
 *---------------------------------------------------------------------------*/
struct struct_adc {
  void * buffer;          /* address of the buffer */ 
  unsigned int start;     /* where to start to read the buffer */
  unsigned int offset;    /* offset in the buffer */
  unsigned int size;      /* size of the buffer */
};



/*---------------------------------------------------------------------------*
 * NAME       : block
 * DESCRIPTION: struct of the linked list block
 *---------------------------------------------------------------------------*/
struct struct_block {
  unsigned int id;           /* id of the block */
  unsigned int size;         /* size of the block */
  unsigned int offset;       /* offset value in the packet */
  unsigned int state;        /* 0: opened 1: closed */
  struct struct_block *next; /* linked list */
};


/*---------------------------------------------------------------------------*
 * NAME       : length
 * DESCRIPTION: struct of the linked list length
 *---------------------------------------------------------------------------*/
struct struct_length {
  unsigned int id;            /* redundant information with block->id */
  struct struct_block *block; /* the block affiliated to the length */
  unsigned int offset;        /* offset in the fuzz_buffer */
  struct struct_length *next; /* linked list */
  unsigned int type;          /* 0x14 = b32 big-endian    32 bits
				 0x12 = l32 little-endian 32 bits
				 0x15 = b16 big-endian    16 bits
				 0x13 = l16 litle-endian  16 bits
				 0x18 = 8                  8 bits
				 0x16 = s16 string in hexadecimal
				 0x17 = s10 string in decimal */
};

/*---------------------------------------------------------------------------*
 * NAME       : hash
 * DESCRIPTION: struct of the linked list hash
 *---------------------------------------------------------------------------*/
struct struct_hash {
  unsigned int id;            /* redundant information with block->id */
  struct struct_block *block; /* the block affiliated to the length */
  unsigned int offset;        /* offset in the fuzz_buffer */
  struct struct_hash *next;   /* linked list */
  unsigned int type;          /* 0x20 = crc32 big endian 
                                 0x21 = crc32 little endian */
};


/*---------------------------------------------------------------------------*
 * NAME       : chrono
 * DESCRIPTION: linked list of the block to send/recv chronologically
 *---------------------------------------------------------------------------*/
struct struct_chrono {
  struct struct_chrono *next;  /* linked list (now i love it hehe) */
  unsigned int id;             /* the id of the selected block */
  unsigned int type;           /* type of the block: 
				  0 = send (inet or file)
				  1 = recv (inet) */

};

/*---------------------------------------------------------------------------*
 * NAME       : source
 * DESCRIPTION: linked list of the files containing fuzz source
 *              for example: we give as an argument to the fuzz function
 *              a file which contains a list of different files, containing
 *              fuzz data.
 *---------------------------------------------------------------------------*/
struct struct_source {
  struct struct_source *next;      /* linked list */
  unsigned int id;                 /* id of the filename */
  unsigned char *filename;         /* pointer on a heap buffer containing the name */
  unsigned int size;               /* size of the content of the file */
};

/*---------------------------------------------------------------------------*
 * NAME       : fuzz
 * DESCRIPTION: linked list of the fuzzed functions
 *---------------------------------------------------------------------------*/
struct struct_fuzz {
  struct struct_fuzz *next;     /* linked list (now i love it hehe) */
  unsigned int id;              /* the id of the fuzz function (for priority) */
  unsigned int total;           /* the total number of the lines in the fuzz file */
  unsigned int current;         /* the current line of the fuzz file */
  unsigned int weight;          /* weight of the string (used with the debugger) */
  struct struct_source *source; /* beginning of the source fuzz linked list */ 
};


/*---------------------------------------------------------------------------*
 * NAME       : config
 * DESCRIPTION: All the configuration values
 *---------------------------------------------------------------------------*/
typedef struct configuration {
  unsigned int timeout;            /* timeout in sec if nothing is received-> go to next fuzz */
  unsigned int invert;             /* 0 = send->send, 1 = send<->recv */
  unsigned int mode;               /* mode the the fuzzer: 0=client (default)  1=server */
  unsigned int type;               /* 0 = tcp; 1 = udp; 2 = file */
  unsigned int ring_zero;          /* 0 -> first passage (ring zero mode). After > 0 */
  unsigned int bruteforce;         /* fuzz every strings/hex */
  unsigned int wait;               /* wait "wait" seconds before starting the connection (debugger) */
  unsigned char *dbg_host;         /* hostname of the autodafe's debugger */
  unsigned short dbg_port;         /* port of the autodafe's debugger */
  unsigned int dbg_mode;           /* if the autodafe's debugger is used or not (resp. 1 or 0) */
  FILE *fuzz_file;                 /* the current filename created by fuzz file */
  FILE *fuzz_file_dbg;             /* the debugger's file (contain strings) */
  unsigned int fuzz_file_id;       /* id used to create multiple fuzz files */
  unsigned char *fuzz_file_dir;    /* where put the fuzzed files */
  unsigned short port;             /* listening port in server mode or host port in client mode */
  unsigned int fuzz_current;       /* the current fuzzed linked list fuzz */
  int socket;                      /* socket descriptor */
  int dbg_socket;                  /* dbg socket descriptor */
  char *host;                      /* hostname to connect */
  FILE *adc_file;                  /* the opened file */
  unsigned char *buf_fuzz;         /* pointer to the current buffer fuzz */
  unsigned int buf_fuzz_size;      /* size of the current buffer fuzz */
  struct struct_adc *adc;          /* the adc structure */
  struct struct_block *block;      /* first entry of the linked list block */
  struct struct_length *length;    /* first entry of the linked list length */
  struct struct_chrono *chrono;    /* first entry of the linked list chrono */
  struct struct_fuzz *fuzz;        /* first entry of the linked list fuzz */
  struct struct_hash *hash;        /* first entry of the linked list hash */  
} config;

