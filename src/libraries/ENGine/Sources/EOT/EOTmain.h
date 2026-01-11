/*$T EOTmain.h GC! 1.078 03/13/00 16:51:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Main functions of the EOT module */
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __EOTMAIN_H__
#define __EOTMAIN_H__

#include "BASe/BAStypes.h"

#include "ENGine/Sources/WORld/WOR.h"
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern void	EOT_SetOfEOT_Clear(EOT_tdst_SetOfEOT *);
extern void	EOT_SetOfEOT_AddOneElem(EOT_tdst_SetOfEOT *, struct OBJ_tdst_GameObject_ *);
extern void	EOT_SetOfEOT_Build(EOT_tdst_SetOfEOT *, TAB_tdst_PFtable *);
extern void	EOT_SetOfEOT_OneBuild(TAB_tdst_PFtable *, ULONG, TAB_tdst_PFtable *);
extern void	EOT_UpdateIdentityFlagsOfTable(TAB_tdst_PFtable *);
extern void	EOT_RemoveObjectFromTableIfBadFlag(TAB_tdst_PFtable *, ULONG);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __EOTMAIN_H__ */
