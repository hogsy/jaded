/*$T GRI_display.h GC! 1.092 09/04/00 14:21:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef ACTIVE_EDITORS

#pragma once
#ifndef __GRID_DISPLAY_H__
#define __GRID_DISPLAY_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* __cplusplus */

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define GRID_Cul_DO_Show			0x00000001
#define GRID_Cul_DO_ShowOnlyReal	0x00000002
#define GRID_Cul_DO_ShowContent		0x00000004
#define GRID_Cul_DO_ShowEdit		0x00000008
#define GRID_Cul_DO_LockZOnObject	0x00000010
#define GRID_Cul_DO_Show1			0x00000020

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GRID_tdst_DisplayOptions_
{
	ULONG	ul_Flags;
	float	f_Z;
	ULONG	ul_KeyForLockedZ;
} GRID_tdst_DisplayOptions;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

extern BOOL GRI_gb_UpdateZ;
extern void GRI_Display(struct WOR_tdst_World_ *, struct GRID_tdst_World_ *, struct GDI_tdst_DisplayData_ *);
extern BOOL GRID_2DtoGridCoord
			(
				struct WOR_tdst_World_ *,
				struct GRID_tdst_World_ *,
				struct GDI_tdst_DisplayData_ *,
				int,
				int,
				int *,
				int *,
				char
			);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* __cplusplus */

#endif /* __GRID_DISPLAY_H__ */

#endif /* ACTIVE_EDITORS */
