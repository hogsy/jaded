/*$T COLzdx.h GC! 1.081 04/03/00 18:22:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __COL_ZDX__
#define __COL_ZDX__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplcus */

#include "COLvars.h"

BOOL	COL_ZDM_ZDR(COL_tdst_GlobalVars *);
void	COL_ZDx_SpecificSizeSet(struct OBJ_tdst_GameObject_ *, UCHAR, MATH_tdst_Vector *);
void	COL_ZDx_SpecificPosSet(struct OBJ_tdst_GameObject_ *, UCHAR, MATH_tdst_Vector *);
float	COL_ZDx_SizeGet(struct OBJ_tdst_GameObject_ *, UCHAR);
void	COL_ZDx_PosGet(struct OBJ_tdst_GameObject_ *, UCHAR, MATH_tdst_Vector *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* _cplusplcus */

#endif /* __COL_ACTOR_ */
