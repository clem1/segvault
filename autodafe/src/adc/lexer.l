/*****************************************************************************/
/* AUTODAFE lexer - Martin Vuagnoux - University of Cambridge, Computer Lab. */
/* 2004-04-28                                                                */
/*****************************************************************************/

%{
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"

static int yywrap(void);
static void skip_comment(void);
static void llerror(const char *);
static unsigned int d_hex2i(char *);

/* init */
unsigned int lineno;

%}

h               [0-9a-fA-F]
h0              {h}{2}          
h1              0[xX]{h}{2}
h2              \\[xX]{h}{2}
qstring         \"[^"\\\n]*(\\.[^"\\\n]*)*\"
qcomment        \/\/.*\n
space           [ \t]+        
nl              \n
void            .
%%
{qcomment}      { lineno++; }
{nl}            { lineno++; }
{space}         /* do nothing */ ;
"/*"            { skip_comment(); }
"("             { return OPEN_PAREN; }
")"             { return CLOSE_PAREN; }
";"             { return SEMI_COLON; }
","             { return COMMA; }
{qstring}       { /* check the max size */
                  if (yyleng > 257) 
                     llerror("String too long, maximum size is 255 bytes\n");
                  yytext[yyleng - 1] = '\0'; /* remove last quote */
                  strncpy(yylval.string, yytext+1, 255); /* put in yylval */
                  yylval.string[255] = '\0'; /* just for security */
                  return Q_STRING; 
                }
{h0}            { yylval.hex = d_hex2i(yytext);
                  return HEX;
                }
{h1}|{h2}       { yylval.hex = d_hex2i(yytext+2);
                  return HEX;
                }
{void}          { llerror(" Character unknown! Write hexa values in two bytes.\n");
                }
hex             { return F_HEX; }
string          { return F_STRING; }
block_begin     { return F_BLOCK_BEGIN; }
block_end       { return F_BLOCK_END; }
send            { return F_SEND; }
recv            { return F_RECV; }
block_size_l32        { return F_BLOCK_SIZE_L_32; }
block_size_b32        { return F_BLOCK_SIZE_B_32; }
block_size_l16        { return F_BLOCK_SIZE_L_16; }
block_size_b16        { return F_BLOCK_SIZE_B_16; }
block_size_dec_string { return F_BLOCK_SIZE_DEC_STRING; }
block_size_hex_string { return F_BLOCK_SIZE_HEX_STRING; }
block_size_8          { return F_BLOCK_SIZE_8; }
fuzz_string           { return F_FUZZ_STRING; }
fuzz_hex              { return F_FUZZ_HEX; }
block_crc32_l         { return F_BLOCK_CRC32_L; }
block_crc32_b         { return F_BLOCK_CRC32_B; }

%%

static int yywrap(void) {
   return 1;
}

static void llerror(const char *s) {
        fprintf(stderr, "[LL-ERROR] at line[%d]: %s\n", lineno, s);
        exit(-5);
}

/* convert a double ascii value (hexa) in int
 * be careful, no security check. the my_string must be "ff"
 */
static unsigned int d_hex2i(char *my_string) {
   unsigned int result;

   result = 0;
   if ((my_string[1] >= 'a') && (my_string[1] <= 'f'))
     result = my_string[1] - 'a' + 10;
   else if ((my_string[1] >= 'A') && (my_string[1] <= 'F'))
     result = my_string[1] - 'A' + 10;
   else if ((my_string[1] >= '0') && (my_string[1] <= '9'))
     result = my_string[1] - '0';

   
   if ((my_string[0] >= 'a') && (my_string[0] <= 'f'))
     result += (my_string[0] - 'a' + 10)*16;
   else if ((my_string[0] >= 'A') && (my_string[0] <= 'F'))
     result += (my_string[0] - 'A' + 10)*16;
   else if ((my_string[0] >= '0') && (my_string[0] <= '9'))
     result += (my_string[0] - '0')*16;



   return result;

}



/*
 * We use this routine instead a lex pattern because we don't need
 * to save the matched comment in the `yytext' buffer.
 */

static void skip_comment(void) {
  int c1, c2;

  c1 = input(); /* take the first next char on yytext */
  c2 = input(); /* take the second next char on yytext */

	while (c2 != EOF && !(c1 == '*' && c2 == '/')) {
		if (c1 == '\n')
			++lineno;
		c1 = c2;
		c2 = input();
	}
}
