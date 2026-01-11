/*$T GFXcarte.h GC! 1.081 01/23/01 14:37:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __GFXCARTE_H__
#define __GFXCARTE_H__

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
#define GFX_C_Carte_NbMaxGao 16

#define GFX_C_Carte_TypeMask 0xF
#define GFX_C_Carte_Blink    0x10

#define GFX_C_Carte_Init     0x01
#define GFX_C_Carte_BlinkOn  0x02
#define GFX_C_Carte_Hidden   0x04
#define GFX_C_Carte_TurnOn   0x08
#define GFX_C_Carte_TurnOff  0x10
#define GFX_C_Carte_HideMap  0x20
#define GFX_C_Carte_Pos2     0x40

#define GFX_C_CarteRequest_TurnOn       1
#define GFX_C_CarteRequest_TurnOff      2
#define GFX_C_CarteRequest_ShowMap      3
#define GFX_C_CarteRequest_HideMap      4
#define GFX_C_CarteRequest_UseFirstPos  5
#define GFX_C_CarteRequest_UseSecondPos 6


/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct GFX_tdst_Carte_
{
	MATH_tdst_Vector                st_2DPos[2];
    MATH_tdst_Vector                st_Offset;
    MATH_tdst_Vector                st_Size;
    MATH_tdst_Vector                st_CursorSize;

    MATH_tdst_Vector                st_AnotherPos[2];

    float                           f_Ratio;
    float                           f_BlinkTime;
    float                           f_EffectTime;
    float                           f_CurEffectTime;

    char                            c_UncoverPiece;
    char                            c_UpdatablePalette;
    char                            c_Dummy2;
    char                            c_Pos2Rotation;
    int                             i_PieceStatus;
    int                             i_CurPieceStatus;

    struct OBJ_tdst_GameObject_     *apst_Gao[GFX_C_Carte_NbMaxGao];
    LONG                            al_Flags[GFX_C_Carte_NbMaxGao];
    LONG                            al_Texture[GFX_C_Carte_NbMaxGao];
    LONG                            l_NbGao;

    LONG                            al_PalColor[ 3 ][ 8 ];
    
    float                           f_CurTime;
    float                           f_UncoverTime;

    int                             i_Flags;

} GFX_tdst_Carte;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

void	*GFX_Carte_Create(void);
int		GFX_i_Carte_Render(void *, void *, OBJ_tdst_GameObject *p_Owner );
void    GFX_Carte_Seti( void *, int , int );
void    GFX_Carte_Destroy( void *, void * );


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GFXCARTE_H__ */
