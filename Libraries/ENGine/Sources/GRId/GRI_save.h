/*$T GRI_save.h GC! 1.092 09/04/00 11:12:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GRID_SAVE_H__
#define __GRID_SAVE_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* __cplusplus */

extern void						GRI_CreateGrid(struct WOR_tdst_World_ *, int);
extern struct GRID_tdst_World_	*GRI_CreateGridGrid(ULONG, char *);
extern void						GRI_SaveGrid(struct GRID_tdst_World_ *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplus */

#endif
