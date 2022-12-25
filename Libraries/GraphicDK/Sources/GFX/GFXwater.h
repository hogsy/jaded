/*$T GFXsmoke.h GC! 1.081 09/21/00 11:59:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXWATER_H__
#define __GFXWATER_H__

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

typedef struct	GFX_tdst_Water_
{
	MATH_tdst_Vector	st_Pos;
    MATH_tdst_Vector	st_Normal;
    float               f_Time;
	float				f_Size;
    float               f_EndSize;
    float               f_YoX;
    float               f_CurTime;
    ULONG               ul_Color;
} GFX_tdst_Water;

typedef struct	GFX_tdst_Water2_
{
    LONG                l_NbMax;
    ULONG               ul_Color;
    
    LONG                l_NbCur;

    GFX_tdst_Water      *dst_Water;
} GFX_tdst_Water2;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Water_Create(void);
int     GFX_i_Water_Render(void *);

void	*GFX_Water2_Create(void);
void    GFX_Water2_Destroy( void *);
int     GFX_i_Water2_Render(void *);
void    GFX_Water2_Seti( void *, int, int, int );
void    GFX_Water2_Setf( void *, int, int, float );
void    GFX_Water2_Setv( void *, int, int, MATH_tdst_Vector * );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXWATER_H__ */
