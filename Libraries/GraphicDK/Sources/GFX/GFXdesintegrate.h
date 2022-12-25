/*$T GFXdesintegrate.h GC! 1.081 10/12/00 11:41:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXDESINTEGRATE_H__
#define __GFXDESINTEGRATE_H__

#include "BASe/BAStypes.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define GFX_C_Desintegrate_Init       0x000000001
#define GFX_C_Desintegrate_Collision  0x000000002


/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	GFX_tdst_Desintegrate_
{
    MATH_tdst_Vector        st_Grav;
    
    float                   f_MinTime;
    float                   f_MaxTime;
    float                   f_MulMin;
    float                   f_MulMax;
    float                   f_Friction;
    float                   f_ZMin;
    float                   f_ZMinStrength;

    ULONG                   ul_Flags;

    MATH_tdst_Matrix        st_Matrix;
    struct OBJ_tdst_GameObject_ *pst_GO;
    struct GEO_tdst_Object_ *pst_Obj;
    struct GRO_tdst_Struct_ *pst_Material;
    LONG                    l_NbTriangles;
    LONG                    l_NbPoints;
    LONG                    l_NbElements;
    LONG                    *dl_NbTriangles;
    float                   f_Time;

    struct PAG_tdst_Struct_ *pst_Particles;
} GFX_tdst_Desintegrate;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Desintegrate_Create( void * );
void    GFX_Desintegrate_Destroy( void * );
int     GFX_i_Desintegrate_Render( void * );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXDESINTEGRATE_H__ */
