/*$T GFXsmoke.h GC! 1.081 09/21/00 11:59:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXSMOKE_H__
#define __GFXSMOKE_H__

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

typedef struct GFX_tdst_OneSmoke_
{
    MATH_tdst_Vector    st_Pos;
    MATH_tdst_Vector    st_Speed;
    float               f_Time;
} GFX_tdst_OneSmoke;

typedef struct	GFX_tdst_Smoke_
{
    MATH_tdst_Vector    st_Pos;
    MATH_tdst_Vector    st_Speed;
    MATH_tdst_Vector	st_Wind;

    float               f_LifeTime;
    float               f_Friction;
    float               f_StartSize;
    float               f_EndSize;

    ULONG               ul_StartColor;
    ULONG               ul_EndColor;
    
    ULONG               ul_NbMax;
    ULONG               ul_CurNb;

    char                c_AddOne;
    char                c_Flags;
    char                c_Dummy1;
    char                c_Dummy2;

    GFX_tdst_OneSmoke   *dst_Smoke;
    float               f_TimeLeft;
} GFX_tdst_Smoke;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Smoke_Create(void);
void    GFX_Smoke_Destroy( void *);
int     GFX_i_Smoke_Render(void *);

void    GFX_Smoke_Seti( void *, int, int);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXSMOKE_H__ */
