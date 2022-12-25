/*$T TEXanimated.h GC! 1.081 10/23/01 10:00:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __TEXANIMATED_H__
#define __TEXANIMATED_H__

#ifndef PSX2_TARGET
#pragma once
#endif
#include "BIGfiles/BIGkey.h"
#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */
#define TEXANI_Flags_Frozen         1
#define TEXANI_Flags_UseRequested   2

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    animated texture data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	TEX_tdst_AnimTex_
{
	BIG_KEY ul_Key;
	SHORT	w_Time;
	SHORT	w_Index;
} TEX_tdst_AnimTex;

typedef struct	TEX_tdst_Animated_
{
	SHORT				w_Index;
	USHORT				uw_Flags;

	UCHAR				uc_Number;
	UCHAR				uc_Current;
	SHORT				w_Counter;

    // Real time only, not save
    UCHAR               uc_Requested;
    UCHAR               uc_AniFlags;
    USHORT              uw_Dummy;

	TEX_tdst_AnimTex	*dst_Tex;
} TEX_tdst_Animated;

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void				TEX_Anim_Init(void);
void				TEX_Anim_Reinit(struct GDI_tdst_DisplayData_ *);
TEX_tdst_Animated	*TEX_pst_Anim_Add(SHORT, UCHAR, USHORT, void *);
void				TEX_Anim_AfterLoad(struct GDI_tdst_DisplayData_ *);
void				TEX_Anim_Update(struct GDI_tdst_DisplayData_ *);

TEX_tdst_Animated	*TEX_pst_Anim_Get(SHORT);

#ifdef ACTIVE_EDITORS
void				TEX_Anim_Save(void);
void				TEX_Anim_AddTex(void);

#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __TEXANIMATED_H__ */
