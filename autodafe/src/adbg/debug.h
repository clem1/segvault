/*---------------------------------------------------------------------------*
 *--- (c) Martin Vuagnoux, Cambridge University, UK.                      ---*
 *---                                                            Jun.2004 ---*
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * NAME       : debug.h
 * DESCRIPTION: Define debug_, malloc_, verbose_
 *---------------------------------------------------------------------------*/

#include <features.h>
/*---------------------------------------------------------------------------*
 * NAME: debug_
 * DESC: Print the message with the name of the file.c and the line. The
 *       number of debug level is defined by debug_level (extern value)
 *---------------------------------------------------------------------------*/
void debug_(int level, char *file, int line, const char *func, const char *fmt, ...);

/*---------------------------------------------------------------------------*
 * NAME: malloc_
 * DESC: Verify if the system is able to alloc this size of memory
 *---------------------------------------------------------------------------*/
void *malloc_(size_t size);

/*---------------------------------------------------------------------------*
 * NAME: verbose_
 * DESC: Print user-friendly messages.
 *---------------------------------------------------------------------------*/
void verbose_(char *fmt, ...);

/*---------------------------------------------------------------------------*                                            
 * NAME: verbose__
 * DESC: Print user-friendly messages.
 *---------------------------------------------------------------------------*/
void verbose__(char *fmt, ...);

/*---------------------------------------------------------------------------*                                            
 * NAME: error_
 * DESC: Print user-friendy error message
 *---------------------------------------------------------------------------*/
void error_(char *fmt, ...);


# define debug(level, expr...) debug_(level, __FILE__, __LINE__, DEBUG_FUNCTION, expr)

/* Version 2.4 and later of GCC define a magical variable `__PRETTY_FUNCTION__'
   which contains the name of the function currently being defined.
   This is broken in G++ before version 2.6.
   C9x has a similar variable called __func__, but prefer the GCC one since
   it demangles C++ function names.  */
# if __GNUC_PREREQ (2, 4)
#   define DEBUG_FUNCTION	__PRETTY_FUNCTION__
# else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define DEBUG_FUNCTION	__func__
#  else
#   define DEBUG_FUNCTION	((__const char *) 0)
#  endif
# endif
