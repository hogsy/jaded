/*$T GFXripple.h GC! 1.081 11/27/01 15:58:06 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXRIPPLE_H__
#define __GFXRIPPLE_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */
#define GFX_C_Ripple_Die            1
#define GFX_C_Ripple_DieAfterGenAll 2

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GFX_tdst_OneRipple_
{
	MATH_tdst_Vector	st_Pos;
	MATH_tdst_Vector	st_Dir;
	MATH_tdst_Vector	st_Banking;
    float               f_Time;
} GFX_tdst_OneRipple;

typedef struct	GFX_tdst_Ripple_
{
	MATH_tdst_Vector	st_Pos;
	MATH_tdst_Vector	st_Dir;
	MATH_tdst_Vector	st_Banking;

	float				f_LifeTime;
	float				f_IncreaseRate;
	float				f_GenerationRate;
	float				f_Speed;
    float               f_StartSize;

	ULONG				ul_NbMax;
	ULONG				ul_StartColor;
	ULONG				ul_EndColor;

	ULONG				ul_CurNb;
    ULONG               ul_Flags;
    ULONG               ul_GenNb;

	GFX_tdst_OneRipple	*dst_Ripple;
    float               f_GenTimeLeft;
} GFX_tdst_Ripple;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Ripple_Create(struct OBJ_tdst_GameObject_ *);
void	GFX_Ripple_Destroy(void *);
int		GFX_i_Ripple_Render(void *, struct OBJ_tdst_GameObject_ *);

void	GFX_Ripple_Seti( void *, int, int);
void    GFX_Ripple_Setf( void *, int, float );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXRIPPLE_H__ */
