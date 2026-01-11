/*$T BIGgroup.h GC 1.138 03/16/04 10:23:48 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#include "BASe/BAStypes.h"
#include "BIGfiles/BIGdefs.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 -----------------------------------------------------------------------------------------------------------------------
    Structure to describe a group element
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	BIG_tdst_GroupElem_
{
	BIG_KEY ul_Key;
	ULONG	ul_Type;
} BIG_tdst_GroupElem;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Prototypes
 -----------------------------------------------------------------------------------------------------------------------
 */

extern int			BIG_i_IsRefInGroup(BIG_INDEX, BIG_KEY);
extern BIG_INDEX	BIG_ul_GetFatGrpRef(BIG_INDEX, int);
extern int			BIG_i_GetNextGrpType(BIG_tdst_GroupElem *, int, int, char *);

#ifdef ACTIVE_EDITORS
extern void			BIG_AddRefInGroup(BIG_INDEX, char *, BIG_KEY);
extern void			BIG_AddRefInGroupAtRank(BIG_INDEX, char *, BIG_KEY, int);
extern void			BIG_DelRefFromGroup(BIG_INDEX, BIG_KEY);
extern BOOL			BIG_b_IsGrpFile(BIG_INDEX);
#endif /* ACTIVE_EDITORS */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
