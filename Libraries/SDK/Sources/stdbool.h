#if !defined( INCLUDED_STDBOOL_H )
# define INCLUDED_STDBOOL_H

#if !defined( __cplusplus )
/* Ansi C */

   typedef int _Bool;
#  define bool _Bool
#  define true 1
#  define false 0
#else
/* Ansi C++ */

#  define bool bool
#  define true true
#  define false false

#endif

#endif
