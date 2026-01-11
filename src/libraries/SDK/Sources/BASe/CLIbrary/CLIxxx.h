/*$T CLIxxx.h GC! 1.097 02/01/01 14:41:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __CLIXXX_H__
#define __CLIXXX_H__
#include "BASe/BAStypes.h"

#include "time.h"
#include "setjmp.h"
#include "stdlib.h"
#ifndef _GAMECUBE
#include "sys/timeb.h"
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    PS2 and PC functions
 -----------------------------------------------------------------------------------------------------------------------
 */

#define L_abs	abs
#define L_qsort qsort
#define L_struct_tm struct tm

#ifdef PSX2_TARGET

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    PS2 functions
 -----------------------------------------------------------------------------------------------------------------------
 */

#include <libcdvd.h>
#define L_time_t	sceCdCLOCK
L_time_t	L_time(L_time_t *timer);
L_struct_tm *L_localtime(L_time_t *timer);

#else

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    PC functions
 -----------------------------------------------------------------------------------------------------------------------
 */

#define L_time_t	time_t
#define L_strtime	_strtime
#define L_strdate	_strdate
#define L_strftime	strftime
#define L_time		time
#define L_tm		tm
#define L_asctime	asctime
#define L_localtime localtime
#define L_jmp_buf	jmp_buf
#define L_setjmp	setjmp
#define L_longjmp	longjmp
#define L_exit		exit
#endif
#endif /* __CLIXXX_H__ */
