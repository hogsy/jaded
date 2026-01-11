/*$T GFXsmoke.h GC! 1.081 09/21/00 11:59:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXSPARK_H__
#define __GFXSPARK_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

// flags for spark system
#define SPARK_EnlargeFromOld    0
#define SPARK_EnlargeFromCenter 1
#define SPARK_EnlargeFromNew    2
#define SPARK_EnlargeMask       3
#define SPARK_NoMoreGeneration  4
#define SPARK_GenerateOnlyOnce  8
#define SPARK_ColorDeath        16
#define SPARK_OneTriangle       32
#define	SPARK_MaxDist			64

// flags for particule
#define SPARK_Dying             1


/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */


typedef struct GFX_tdst_OneSpark_
{
    LONG                l_Flags;
    MATH_tdst_Vector    st_Pos;
    MATH_tdst_Vector    st_Speed;
    float               f_Time;
    float               f_Factor;
} GFX_tdst_OneSpark;

typedef struct	GFX_tdst_Spark_
{
	MATH_tdst_Vector	        st_Pos;
    MATH_tdst_Vector            st_Norm;
    MATH_tdst_Vector            st_Gravity;

    float                       f_Angle;
    float                       f_SpeedMin;
    float                       f_SpeedMax;
    float                       f_Friction;
    float                       f_Size;
    float                       f_TimeMin;
    float                       f_TimeMax;
    float                       f_Scale;
    float                       f_Period;
    float                       f_DeathMin;
    float                       f_DeathMax;
    float						f_MaxDist;

    LONG                        l_Nb;
    ULONG                       ul_Color;
    LONG                        l_Flags;
    LONG                        Zorder;

    LONG                        l_NbMax;
    LONG                        l_CurNb;
    float                       f_GenTime;

    struct GFX_tdst_OneSpark_   *dst_Spark;



} GFX_tdst_Spark;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Spark_Create(void);
void    GFX_Spark_Destroy( void *);
int     GFX_i_Spark_Render(void *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXSPARK_H__ */
