/*$T CLImem.h GC! 1.081 07/15/02 18:32:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif // __cplusplus

#include "malloc.h"
#define L_malloc(a)		malloc(a)
#define L_realloc(a, b) realloc(a, b)
#define L_free(a)		free(a)
