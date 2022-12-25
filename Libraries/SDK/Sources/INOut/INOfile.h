/*$T INOfile.h GC 1.138 07/17/03 12:30:04 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __INOFILE_H__
#define __INOFILE_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "INOut/INOsaving.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define INO_Cte_DiscErrorNone			(0)
#define INO_Cte_DiscErrorNoDisc			(-1)
#define INO_Cte_DiscErrorWrongDisc		(-2)
#define INO_Cte_DiscErrorRetry			(-3)
#define INO_Cte_DiscErrorFatal			(-4)
#define INO_Cte_DiscErrorCoverIsOpened	(-5)

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern int	INO_gi_DiscError;


#if defined (__cplusplus) && !defined(JADEFUSION) 
}
#endif
#endif /* __INOFILE_H */
