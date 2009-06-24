/*****************************************************************************/
/* AUTODAFE parser - Martin Vuagnoux - University of Cambridge, Computer Lab.*/
/* 2004-04-28                                                                */
/*****************************************************************************/
%{
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "../../include/autodafe.h"
#include "debug.h"

  // #define DEBUG
  
  extern unsigned int lineno; /* line counter */
  FILE * file_adc;            /* output file */
  
  /* this structure is used to check if blocks are correctly defined */
  struct block {
    char name[256];
    unsigned int id;
    unsigned int started;
    unsigned int ended;
    struct block *prev;
    unsigned int size;
    unsigned int variable;
  };

  struct block * last_block; /* pointer to the last block */
  unsigned int block_number;

  unsigned int string_counter = 0;

  unsigned char *one_string = NULL;
  unsigned int one_string_size = 0;

  unsigned char *two_string = NULL;
  unsigned int two_string_size = 0;

  unsigned char *hex_buffer = NULL;
  unsigned int hex_buffer_size = 0;
  unsigned int fuzz_id = 0;

  unsigned int send_word = 0;

  unsigned int block_new(char *string);
  void update_size(unsigned int);
  static void yyerror(const char *s);
  unsigned int block_end(char *string);
  unsigned int block_check_send(char *string);
  unsigned int block_size_check(char *string);
%}

%union {
  char string[256];     /* string buffer */
  unsigned int hex;     /* hex value     */
}

%token F_HEX F_FUZZ_HEX
%token F_STRING F F_FUZZ_STRING 
%token F_BLOCK_BEGIN F_BLOCK_END 
%token F_SEND F_RECV
%token F_BLOCK_SIZE_L_32 F_BLOCK_SIZE_L_16
%token F_BLOCK_SIZE_B_32 F_BLOCK_SIZE_B_16
%token F_BLOCK_SIZE_8
%token F_BLOCK_SIZE_DEC_STRING F_BLOCK_SIZE_HEX_STRING
%token F_BLOCK_CRC32_B F_BLOCK_CRC32_L
%token <string> Q_STRING
%token <hex> HEX
%token OPEN_PAREN CLOSE_PAREN SEMI_COLON COMMA
%%
start    : /* empty */
         | start function
         ;

function : F_STRING OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON   
         { 
	   /*   1B   1B         < 255B
	    * +----+----+--             ------+
	    * |    |    |                     |
	    * +----+----+-----             ---+
	    * opcode size       string
	    *
	    * the size of the string is computed on one byte (size)
	    */
	   unsigned char opcode;
	   unsigned char size;
	   opcode = HF_STRING;
	   size = strlen(yylval.string) & 0x000000ff;
	   fwrite(&opcode, sizeof(char), 1, file_adc);
	   fwrite(&size, sizeof(char), 1, file_adc);
	   fwrite(yylval.string, sizeof(char), strlen(yylval.string), file_adc);
	   update_size((unsigned int)size);
#ifdef DEBUG
           printf("function "AD_F_STRING": %s (size:%d)\n", yylval.string, strlen(yylval.string));
#endif
         }
         | F_FUZZ_STRING OPEN_PAREN two_strings CLOSE_PAREN SEMI_COLON   
         { 
	   /*   1B       4B         1B     < 255B        1B       <255B
	    * +----+-------------+----+--        ------+----+--        ------+
	    * |    |             |    |                |    |                |
	    * +----+-------------+----+-----        ---+----+-----        ---+
	    * op.       id        size     filename     size      string
	    *
	    * id is a unsigned 32b number (big endian) to give hierarchy 
	    * (priority fuzzing)
	    * the first size is the size of the path (max 255 bytes) which
	    * contains the filename of the fuzz strings. by default, it's
	    * DEFAULT_FUZZ_STRING_FILE
	    * the size of the string is computed on one byte (size)
	    */
	   unsigned char opcode;
	   unsigned char size;
	   unsigned char size_filename;
	   unsigned int id;
	   opcode = HF_FUZZ_STRING;

	   /* one string */
	   if (two_string == NULL) {
	   size_filename = strlen(DEFAULT_FUZZ_STRING_FILE) & 0x000000ff;
	   size = strlen(one_string) & 0x000000ff;
	   id = htonl(fuzz_id++);
	   fwrite(&opcode, sizeof(char), 1, file_adc); /* opcode */
	   fwrite(&id, sizeof(int), 1, file_adc); /* fuzz id */
	   fwrite(&size_filename, sizeof(char), 1, file_adc); /* size file */
	   fwrite(DEFAULT_FUZZ_STRING_FILE, sizeof(char), (size_t) size_filename, file_adc);
	   fwrite(&size, sizeof(char), 1, file_adc); /* size of the string */
	   fwrite(one_string, sizeof(char), (size_t) size, file_adc); /* string */
	   update_size((unsigned int)size);
#ifdef DEBUG
	   printf("function "AD_F_FUZZ_STRING"[%d]: %s (size:%d)\n", fuzz_id - 1, one_string, (unsigned int)size);
	   printf("source index: %s\n", DEFAULT_FUZZ_STRING_FILE);
#endif
	   }

	   /* two strings */
	   else {
	   size_filename = one_string_size & 0x000000ff;
	   size = two_string_size & 0x000000ff;
	   id = htonl(fuzz_id++);
	   fwrite(&opcode, sizeof(char), 1, file_adc); /* opcode */
	   fwrite(&id, sizeof(int), 1, file_adc); /* fuzz id */
	   fwrite(&size_filename, sizeof(char), 1, file_adc); /* size file */
	   fwrite(one_string, sizeof(char), (size_t) size_filename, file_adc);
	   fwrite(&size, sizeof(char), 1, file_adc); /* size of string */
	   fwrite(two_string, sizeof(char), (size_t) size, file_adc); /* string */
	   update_size((unsigned int)size);
#ifdef DEBUG
           printf("function "AD_F_FUZZ_STRING"[%d]: %s (size:%d)\n", fuzz_id - 1, two_string, (unsigned int)size);
	   printf("source index: %s\n", one_string);
#endif
	   }
	   
	   /* free and init */
	   if (one_string) {free(one_string); one_string = NULL; one_string_size = 0;}
	   if (two_string) {free(two_string); two_string = NULL; two_string_size = 0;}
	   string_counter = 0;
         }
         | F_HEX OPEN_PAREN hexs CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B             < 4GB
	    * +----+----------------+--             ------+
	    * |    |                |                     |
	    * +----+----------------+-----             ---+
	    * opcode    size              hex values
	    *
	    * the size of the "string" hex value is computed on 32 bits.
	    */
	   unsigned int i;
	   unsigned char opcode;
	   unsigned int size;
	   size = htonl(hex_buffer_size);
	   opcode = HF_HEX;
	   fwrite(&opcode, sizeof(char), 1, file_adc); /* opcode */
	   fwrite(&size, sizeof(int), 1, file_adc); /* write the number of hex values (big endian) */
	   fwrite(hex_buffer, sizeof(char), hex_buffer_size, file_adc); /* write the values */
#ifdef DEBUG
	    printf("function "AD_F_HEX" (%d):", hex_buffer_size);
	    for(i=0;i<hex_buffer_size;i++)
	      printf(" %02x", hex_buffer[i]);
	    printf("\n");
#endif
	   update_size(hex_buffer_size);
	   free(hex_buffer); /* free the buffer */
	   hex_buffer = NULL; 
	   hex_buffer_size = 0; /* reset the size */
	   
         }
         | F_FUZZ_HEX OPEN_PAREN hexs CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B         4B         1B        <255B               4B              < 4GB
	    * +----+----------------+----+--           -----+----------------+--             ------+
	    * |    |                |    |                  |                |                     |
	    * +----+----------------+----+-----           --+----------------+-----             ---+
	    * opcode       id        size    filename               size             hex values
	    *                        file
	    *
	    * size_file is the size (max 255) of the filename containing the index source
	    * filename is the name of the index source
	    * the size of the "string" hex value is computed on 32 bits.
	    * id is a unsigned 32b number (big endian) to give hierarchy (priority fuzzing)
	    */
	   unsigned int i;
	   unsigned char opcode;
	   unsigned int size;
	   unsigned char size_filename;
	   unsigned int id;
	   opcode = HF_FUZZ_HEX;
	   size_filename = strlen(DEFAULT_FUZZ_HEX_FILE) & 0x000000ff;
	   size = htonl(hex_buffer_size);
	   id = htonl(fuzz_id++);
	   fwrite(&opcode, sizeof(char), 1, file_adc); /* opcode */
	   fwrite(&id, sizeof(int), 1, file_adc); /* id fuzz number */
	   fwrite(&size_filename, sizeof(char), 1, file_adc); /* size file */
	   fwrite(DEFAULT_FUZZ_HEX_FILE, sizeof(char), (size_t) size_filename, file_adc);
	   fwrite(&size, sizeof(int), 1, file_adc); /* write the number of hex values (big endian) */
	   fwrite(hex_buffer, sizeof(char), hex_buffer_size, file_adc); /* write the values */
#ifdef DEBUG
	   printf("function "AD_F_FUZZ_HEX"[%d] :", fuzz_id - 1);
	   for(i=0;i<hex_buffer_size;i++)
	     printf(" %02x", hex_buffer[i]);
	   printf("\n");
	   printf("source index: %s\n", DEFAULT_FUZZ_HEX_FILE);
#endif
	   update_size(hex_buffer_size);
	   free(hex_buffer); /* free the buffer */
	   hex_buffer = NULL; 
	   hex_buffer_size = 0; /* reset the size */
         }
         | F_FUZZ_HEX OPEN_PAREN one_strings COMMA hexs CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B         4B         1B        <255B               4B              < 4GB
	    * +----+----------------+----+--           -----+----------------+--             ------+
	    * |    |                |    |                  |                |                     |
	    * +----+----------------+----+-----           --+----------------+-----             ---+
	    * opcode       id        size    filename               size             hex values
	    *                        file
	    *
	    * size_file is the size (max 255) of the filename containing the index source
	    * filename is the name of the index source
	    * the size of the "string" hex value is computed on 32 bits.
	    * id is a unsigned 32b number (big endian) to give hierarchy (priority fuzzing)
	    */
	   unsigned int i;
	   unsigned char opcode;
	   unsigned int size;
	   unsigned char size_filename;
	   unsigned int id;
	   opcode = HF_FUZZ_HEX;
	   size_filename = one_string_size & 0x000000ff;
	   size = htonl(hex_buffer_size);
	   id = htonl(fuzz_id++);
	   fwrite(&opcode, sizeof(char), 1, file_adc); /* opcode */
	   fwrite(&id, sizeof(int), 1, file_adc); /* id fuzz number */
	   fwrite(&size_filename, sizeof(char), 1, file_adc); /* size file */
	   fwrite(one_string, sizeof(char), one_string_size, file_adc);
	   fwrite(&size, sizeof(int), 1, file_adc); /* write the number of hex values (big endian) */
	   fwrite(hex_buffer, sizeof(char), hex_buffer_size, file_adc); /* write the values */
#ifdef DEBUG
	   printf("function "AD_F_FUZZ_HEX"[%d] :", fuzz_id - 1);
	   for(i=0;i<hex_buffer_size;i++)
	     printf(" %02x", hex_buffer[i]);
	   printf("\n");
	   printf("source index: %s\n", one_string);
#endif
	   update_size(hex_buffer_size);
	   free(hex_buffer); /* free the buffer */
	   free(one_string);
	   one_string = NULL;
	   hex_buffer = NULL; 
	   hex_buffer_size = 0; /* reset the size */
	   one_string_size = 0;
         }
         | F_BLOCK_BEGIN OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_BEGIN;
	   block_id = htonl(block_new(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
#ifdef DEBUG
	   printf("function "AD_F_BLOCK_BEGIN": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }                                                             
         | F_BLOCK_END OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_END;
	   block_id = htonl(block_end(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
#ifdef DEBUG
	   printf("function "AD_F_BLOCK_END": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_BLOCK_CRC32_B OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_CRC32_B;
	   block_id = htonl(block_size_check(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   update_size(sizeof(int));
#ifdef DEBUG
	   printf("function "AD_F_CRC32_B": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_BLOCK_CRC32_L OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_CRC32_L;
	   block_id = htonl(block_size_check(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   update_size(sizeof(int));
#ifdef DEBUG
	   printf("function "AD_F_CRC32_L": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_BLOCK_SIZE_B_32 OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_SIZE_B_32;
	   block_id = htonl(block_size_check(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   update_size(sizeof(int));
#ifdef DEBUG
	   printf("function "AD_F_SIZE_B32": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_BLOCK_SIZE_L_32 OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_SIZE_L_32;
	   block_id = htonl(block_size_check(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   update_size(sizeof(int));
#ifdef DEBUG
           printf("function "AD_F_SIZE_L32": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
	 }
         | F_BLOCK_SIZE_B_16 OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_SIZE_B_16;
	   block_id = htonl(block_size_check(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   update_size(sizeof(short));
#ifdef DEBUG
	   printf("function "AD_F_SIZE_B16": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_BLOCK_SIZE_L_16 OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_SIZE_L_16;
	   block_id = htonl(block_size_check(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   update_size(sizeof(short));
#ifdef DEBUG
	   printf("function "AD_F_SIZE_L16": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_BLOCK_SIZE_HEX_STRING OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_SIZE_S_16;
	   block_id = htonl(block_size_check(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   update_size(-1);
#ifdef DEBUG
	    printf("function "AD_F_SIZE_S16": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_BLOCK_SIZE_DEC_STRING OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_SIZE_S_10;
	   block_id = htonl(block_size_check(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   update_size(-1);
#ifdef DEBUG
	    printf("function "AD_F_SIZE_S10": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_BLOCK_SIZE_8 OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_BLOCK_SIZE_8;
	   block_id = htonl(block_size_check(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   update_size(sizeof(char));
#ifdef DEBUG
	   printf("function "AD_F_SIZE_8": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_SEND OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_SEND;
	   block_id = htonl(block_check_send(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   send_word++;
#ifdef DEBUG
	   printf("function "AD_F_SEND": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         | F_RECV OPEN_PAREN Q_STRING CLOSE_PAREN SEMI_COLON 
         {
	   /*   1B        4B
	    * +----+----------------+
	    * |    |                |
	    * +----+----------------+
	    * opcode      id
	    *
	    * id is a 32b number written in BIG-ENDIAN (Network)
	    */
	   unsigned char opcode;
	   unsigned int block_id;
	   opcode = HF_RECV;
	   block_id = htonl(block_check_send(yylval.string));
	   fwrite(&opcode, sizeof(char), 1, file_adc);
           fwrite(&block_id, sizeof(int), 1, file_adc);
	   send_word++;
#ifdef DEBUG
	   printf("function "AD_F_RECV": %s (id:%d)\n", yylval.string, ntohl(block_id));
#endif
         }
         ;

one_strings: Q_STRING
           {
	     one_string_size = strlen(yylval.string);
	     // printf("one_strings: %s (size: %d)\n", yylval.string, one_string_size);
	     one_string = malloc(one_string_size+1);
	     strncpy(one_string, yylval.string, one_string_size+1);
	     one_string[one_string_size] = '\0';
	   }
           ;
two_strings: Q_STRING 
           {
	     string_counter++;
	     one_string_size = strlen(yylval.string);
	     // printf("first_string: %s (size: %d)\n", yylval.string, one_string_size);
	     one_string = malloc(one_string_size+1);
             strncpy(one_string, yylval.string, one_string_size+1); 
	     one_string[one_string_size] = '\0';
           }
           | two_strings COMMA Q_STRING
           {
	     string_counter++;
	     if (string_counter > 2) {
	       yyerror("more than two strings in a fuzz_string function");
	     }
	     two_string_size = strlen(yylval.string);
	     // printf("second_string: %s (size: %d)\n", yylval.string, two_string_size);
	     two_string = malloc(two_string_size+1);
             strncpy(two_string, yylval.string, two_string_size+1); 
	     two_string[two_string_size] = '\0';
	   }
           ;

hexs      : HEX 
          { 
	    hex_buffer_size++;
	    hex_buffer = realloc(hex_buffer, hex_buffer_size);
	    if (!hex_buffer)
	      yyerror("error in malloc hex buffer");
	    hex_buffer[hex_buffer_size-1] = (unsigned char) yylval.hex & 0x000000ff;
	    // printf(""AD_F_HEX" value[%05d]: 0x%02x (%03d)\n", hex_buffer_size, yylval.hex, yylval.hex);
          }
          | hexs HEX 
          {
	    hex_buffer_size++;
	    hex_buffer = realloc(hex_buffer, hex_buffer_size);
	    if (!hex_buffer)
	      yyerror("error in malloc hex buffer");
	    hex_buffer[hex_buffer_size-1] = (unsigned char) yylval.hex & 0x000000ff;
	    // printf(""AD_F_HEX" value[%05d]: 0x%02x (%03d)\n", hex_buffer_size, yylval.hex, yylval.hex);
	  }
          ;

%%


/* NAME: yyerror
 * DESC: default error function for yacc/bison.
 */
static void yyerror(const char *s) {
        fprintf(stderr, "[YY-ERROR] at line[%d]: %s\n", lineno, s);
	exit(-6);
}


/* NAME: block_check
 * DESC: check if the name of a block is already used. if yes
 *       return a pointer to this structure.
 */
struct block * block_check(char *string) {
  
  struct block * bp;
  
  /* search down the list */
  if (last_block != 0) {
    for(bp=last_block;bp;bp=bp->prev) {
      if (strncmp(bp->name, string, 256) == 0)
	return bp;
    }
  }
  return 0;
}

/* NAME: update_size
 * DESC: add the size value to every opened blocks
 *       if size = -1 -> variable = 1 (cannot compute the size)
 * RETN: void
 */
void update_size(unsigned int size) {
  
  struct block *bp;
  struct block *bp2;
  
  for(bp=last_block;bp;bp=bp->prev) {
    
    /* if there is a variable on the size (i.e. block_size_(dec/hex)_string) */
    if ((size == -1))
      bp->variable = 1;
    
    /* normal case */
    else if ((bp->started) && (!bp->ended)) {
      bp->size+=size;
    }
  }
}

/* NAME: block_size_check
 * DESC: check if the name of the block exists.
 * RETN: The id of the block.
 */
unsigned int block_size_check(char *string) {
  
  struct block *bp;
  
  /* check the name */
  bp = block_check(string);
  
  /* null -> not opened, we open the block now but started = 0 */
  if (!bp) {

    /* allocate the memory for this block */
    bp = (struct block *) malloc(sizeof(struct block));
    if (bp == 0)
      yyerror("Malloc() failed");
  
    /* clear the memory */
    bzero(bp, sizeof(struct block));

    /* init */
    bp->size = 0; 
    bp->started = 0; /* NOT STARTED! */
    bp->ended = 0;
    bp->variable = 0; /* fixed */
  
    bp->prev = last_block; /* next pointer */
    strncpy(bp->name, string, 256); /* copy string */
    last_block = bp; /* update last_block */
    bp->id = block_number++; /* put an id to the block */
  }
  return bp->id;
}

/* NAME: block_new
 * DESC: Create a linked list entry block.
 * RETN: The id of the block.
 */
unsigned int block_new(char *string) {
  
  struct block * bp;
  
  /* check if the name is not already used */
  bp = block_check(string);
  if (bp) {
    
    /* check if it is already started (i.e. only created by size) */
    if (bp->started == 1) {
      yyerror("the block's name is already used, choose another one.");
    }

    /* it's probably a block opened by a size function */
    bp->started = 1;

    /* return the id */
    return bp->id;

  }
  
  /* allocate the memory for this block */
  bp = (struct block *) malloc(sizeof(struct block));
  if (bp == 0)
    yyerror("Malloc() failed");
  
  /* clear the memory */
  bzero(bp, sizeof(struct block));

  /* init */
  bp->size = 0; 
  bp->started = 1;
  bp->ended = 0;
  bp->variable = 0; /* fixed */
  
  bp->prev = last_block; /* next pointer */
  strncpy(bp->name, string, 256); /* copy string */
  last_block = bp; /* update last_block */
  bp->id = block_number++; /* put an id to the block */
 
  /* return the id value */
  return bp->id;
}


/* NAME: block_end
 * DESC: check if the block has been started and put the flag to 1
 */
unsigned int block_end(char *string) {
  
  struct block * bp;
  bp = block_check(string);
  if ((bp != 0) && (bp->started = 1)) {
    bp->ended = 1;
  }
  else {
    yyerror("this block has never been started.");
  }
  return bp->id;
}


/* NAME: block_check_send
 * DESC: check if the block has been started and ended (send/recv)
 */
unsigned int block_check_send(char *string) {
  
  struct block * bp;
  bp = block_check(string);
  if ((bp == 0) || (bp->started == 0))
    yyerror("this block doesn't exist.");
  else if (bp->ended == 0)
    yyerror("this block hasn't been ended yet.");
  return bp->id;
}



/* NAME: block_check_final
 * DESC: Verify if each block are started and ended
 */
void block_check_ended(void) {
  
  struct block * bp;
  int error = 0;
  
  for(bp=last_block;bp;bp=bp->prev) {
    if (bp->started == 0) {
      fprintf(stderr,"[YY-ERROR] block: \"%s\" has not been started\n", bp->name);
      error++;
    }
    if (bp->ended == 0) {
      fprintf(stderr,"[YY-ERROR] block: \"%s\" has not been ended\n", bp->name);
      error++;
    }
    
    /* check if it's a dec_string or hex_string */  
    fprintf(stdout, "[!] block: \"%s\" size: %d (0x%x)\n", bp->name, bp->size, bp->size); 
    if (bp->variable)
      fprintf(stdout,"[!] block: \"%s\" contains variable size! (block_size_***_string)\n", bp->name);
  }
  if (error)
    yyerror("See block(s) error(s) above");
}


/* NAME: block_free
 * DESC: free all blocks
 */
void block_free(void) {
  
  struct block * bp;
  struct block * to_free;
  
  bp = last_block;
  while(bp) {
    to_free = bp;
    bp = bp->prev;
    free(to_free);
  }
}




/* NAME: main
 * DESC: go back to school ;-)
 */
int main(int argc, char **argv) {

   extern FILE *yyin;
   char magic_bytes[8];
   char null_bytes[8];
   char name[FILENAME_MAX];

   /* init */
   lineno = 1;
   last_block = 0;
   block_number = 1;
   memcpy(magic_bytes, MAGIC_SEQUENCE, 8);
   bzero(null_bytes, 8);
   bzero(name, FILENAME_MAX);

   /* usage */
   if((argc < 2)) {
     fprintf(stderr, "%s-v.%d.%d - Autodafe's script [.ad] -> Autodafe's compiled script [.adc]\n", argv[0], MAJOR_VERSION, MINOR_VERSION);
     fprintf(stderr, "%s-v.%d.%d ", argv[0], MAJOR_VERSION, MINOR_VERSION);
     fprintf(stderr, "- (c) 2004-2006 "NAME" <"EMAIL">\n\n");
     fprintf(stderr, "Usage: %s <file.ad> [<file.adc>]\n\n", argv[0]);
     exit(-1);
   }
   
   /* input */
   yyin = fopen(argv[1], "r");
   if (yyin == NULL) {
     fprintf(stderr, "[E] error with file \"%s\"", argv[1]);
     perror(" ");
     exit(-2);
   }

   /* check if the name is finished by .ad */
   if ((strlen(argv[1]) < 4) || 
       (argv[1][strlen(argv[1]) -3] != '.') ||
       (argv[1][strlen(argv[1]) -2] != 'a') ||
       (argv[1][strlen(argv[1]) -1] != 'd')) {
     fprintf(stderr, "[E] bad name, please rename using \".ad\" at the end\n");
     exit(-5);
     }


   /* copy the string in the name buffer */
   if (argv[2] == NULL) {
     if (strlen(argv[1]) > FILENAME_MAX - 2) {
       fprintf(stderr, "[E] Name too long\n");
       exit(-5);
     }
     strncpy(name, argv[1], FILENAME_MAX -2);
     name[strlen(argv[1])] = 'c';
     name[strlen(argv[1]) + 1] = '\0';
     argv[2] = name;

   }

   /* output */
   file_adc = fopen(argv[2], "w");
   if (file_adc == NULL) {
     fprintf(stderr, "[E] error, cannot write file \"%s\" ", argv[2]);
     perror(" ");
     exit(-3);
   }  

   /* first, write the first 8 bytes of the file */
   fwrite(null_bytes, sizeof(char), 8, file_adc);

   /* yacc parsing */
   yyparse();

   /* check final errors */
   block_check_ended();

   /* free buffers */
   block_free();

   /* check if there is at least one send */
   if (!send_word) {
     fprintf(stderr, "[!] Warning! there is no \"send\" or \"recv\" function in this script.\n");
   }

   /* all is correct, we write the header of the file */
   fseek(file_adc, 0, SEEK_SET); /* beginning of the file */
   fwrite(magic_bytes, sizeof(char), 8, file_adc);

   /* say it's ok */
   //   printf("[*] parsing complete - no errors.\n");

   /* close file */
   fclose(file_adc);

   return 0;
}
