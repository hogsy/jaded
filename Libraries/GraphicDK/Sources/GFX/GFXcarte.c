/*$T GFXline.c GC! 1.081 09/19/00 09:14:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "GFX/GFX.h"
#include "GFX/GFXcarte.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

#ifdef PSX2_TARGET

/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Carte Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Carte_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Carte *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Carte *) MEM_p_Alloc(sizeof(GFX_tdst_Carte));
    L_memset( pst_Data, 0, sizeof(GFX_tdst_Carte) );
    pst_Data->f_EffectTime = 0.2f;
    pst_Data->f_UncoverTime = 1.5;
    pst_Data->c_UncoverPiece = -1;
    pst_Data->c_UpdatablePalette = 1;
    pst_Data->i_CurPieceStatus = pst_Data->i_PieceStatus = 0xFFFFFFFF;
    pst_Data->i_Flags = GFX_C_Carte_Hidden | GFX_C_Carte_HideMap;
	return (void *) pst_Data;
}

/*
 =======================================================================================================================
    Add one destroy function for carte to reinit material
 =======================================================================================================================
 */
void GFX_Carte_Destroy( void *p_Data, void *p_Material )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GFX_tdst_Carte          *pst_Data;
    MAT_tdst_MultiTexture   *pst_Mat;
    MAT_tdst_MTLevel        *pst_MatLevel;
    TEX_tdst_Palette        *pst_Palette;
    TEX_tdst_Data           *pst_Tex;
    int                     i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/ 

    pst_Data = (GFX_tdst_Carte *) p_Data;
    pst_Mat = (MAT_tdst_MultiTexture *) p_Material;

    if ( !pst_Mat || (pst_Mat->st_Id.i->ul_Type != GRO_MaterialMultiTexture) ) return;

    if (pst_Mat->st_Id.l_Ref <= 0) return;

    if ( pst_Data->i_Flags & GFX_C_Carte_Init )
    {
        pst_MatLevel = pst_Mat->pst_FirstLevel;
        if (!pst_MatLevel ) return;
        pst_MatLevel = pst_MatLevel->pst_NextLevel;
        if (!pst_MatLevel ) return;

        pst_Tex = TEX_gst_GlobalList.dst_Texture + pst_MatLevel->s_TextureId;
        if ( pst_Tex->uw_Flags & TEX_uw_RawPal )
        {
            pst_Palette = TEX_gst_GlobalList.dst_Palette + pst_Tex->w_Width;
            if (pst_Palette->pul_Color)
                L_memcpy( pst_Palette->pul_Color, pst_Data->al_PalColor, 3 * 8 * 4 );
        }

        i = 0;
        while ( i < GFX_C_Carte_NbMaxGao )
        {
            pst_MatLevel = pst_MatLevel->pst_NextLevel;
            if (!pst_MatLevel ) break;
            pst_MatLevel->s_TextureId = (SHORT) pst_Data->al_Texture[i];
            i++;
        }
        pst_Data->i_Flags = 0;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Carte_UpdatePalette( GFX_tdst_Carte *pst_Data, short _w_Tex )
{
    TEX_tdst_Data       *pst_Tex;
    TEX_tdst_Palette    *pst_Palette;
    ULONG               ul_Value;
    ULONG               *pul_Color, *pul_Color2;
    int                 i;
    float               blend;
     
    pst_Tex = TEX_gst_GlobalList.dst_Texture + _w_Tex;
    if ( !( pst_Tex->uw_Flags & TEX_uw_RawPal ) ) return;

    pst_Palette = TEX_gst_GlobalList.dst_Palette + pst_Tex->w_Width;
    if ( !pst_Palette->pul_Color ) return;
    if ( !(pst_Palette->uc_Flags & TEX_uc_UpdatablePal) ) return;
    if ( pst_Palette->uc_Flags & TEX_uc_Palette16 ) return;

    ul_Value = pst_Data->i_CurPieceStatus = pst_Data->i_PieceStatus;
    pul_Color = pst_Palette->pul_Color + 8;

    if (pst_Data->i_Flags & GFX_C_Carte_HideMap)
    {
        for (i = 1; i < 32; i++, ul_Value >>= 1, pul_Color += 8 )
        {
            L_memcpy( pul_Color, pst_Data->al_PalColor[ (ul_Value & 1) << 1], 32 );
        }
    }
    else
    {
        for (i = 1; i < 32; i++, ul_Value >>= 1, pul_Color += 8 )
        {
            L_memcpy( pul_Color, pst_Data->al_PalColor[ 1 + (ul_Value & 1) ], 32 );
        }
    }

    if (pst_Data->c_UncoverPiece != -1)
    {
        if ( (pst_Data->c_UncoverPiece < 0) || (pst_Data->c_UncoverPiece >= 32) )
            pst_Data->c_UncoverPiece = -1;
        else
        {
            pst_Data->f_UncoverTime -= TIM_gf_dt;
            if (pst_Data->f_UncoverTime < 0)
            {
                pst_Data->f_UncoverTime = 1.5;
                pst_Data->c_UncoverPiece = -1;
            }
            else
            {
                blend = fAbs( fOptSin( pst_Data->f_UncoverTime * 2 * Cf_Pi ) );
                pul_Color = pst_Palette->pul_Color + (8L * (pst_Data->c_UncoverPiece + 1) );
                pul_Color2 = (ULONG *) &pst_Data->al_PalColor[ (pst_Data->i_Flags & GFX_C_Carte_HideMap) ? 0 : 1 ];
                for (i = 0; i < 8; i++, pul_Color++, pul_Color2++)
                    *pul_Color = COLOR_ul_Blend( *pul_Color, *pul_Color2, blend );
            }
        }
    }
    GDI_gpst_CurDD->st_GDI.pfnl_Request( GDI_Cul_Request_ReloadPalette, pst_Tex->w_Width);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Carte_Render(void *p_Data, void *p_Material, OBJ_tdst_GameObject *p_Owner )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Carte          *pst_Data;
    GEO_Vertex              *Vertex;
    ULONG                   DM;
    float                   x1, y1, x2, y2, f_Time;
    int                     i;
    MAT_tdst_MultiTexture   *pst_Mat;
    MAT_tdst_MTLevel        *pst_MatLevel;
    MATH_tdst_Vector        st_Pos, *pst_Pos, st_Sight;
    MATH_tdst_Vector        st_Pos2, *pst_Pos2;
    MATH_tdst_Vector        V, W, C;
    OBJ_tdst_GameObject     *pst_CenterGao; 
    OBJ_tdst_GameObject     *pst_Owner;
    WOR_tdst_World          *pst_World;
    TEX_tdst_Palette        *pst_Palette;
    TEX_tdst_Data           *pst_Tex;
    static int              i_LockMat = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Carte *) p_Data;
    pst_Mat = (MAT_tdst_MultiTexture *) p_Material;
    pst_Owner = (OBJ_tdst_GameObject *) p_Owner;

    
    if ( !pst_Mat || (pst_Mat->st_Id.i->ul_Type != GRO_MaterialMultiTexture) ) return 1;
    if ( !pst_Owner ) return 1;
    if ( (!pst_Data->l_NbGao) || (!pst_Data->apst_Gao[0] )) return 1;

    if ( !(pst_Data->i_Flags & GFX_C_Carte_Init) )
    {
        pst_MatLevel = pst_Mat->pst_FirstLevel;
        if (!pst_MatLevel ) return 1;
        pst_MatLevel = pst_MatLevel->pst_NextLevel;
        if (!pst_MatLevel ) return 1;
        MAT_SET_ColorOp( pst_MatLevel->ul_Flags, MAT_Cc_ColorOp_FullLight );

        pst_Tex = TEX_gst_GlobalList.dst_Texture + pst_MatLevel->s_TextureId;
        if ( pst_Tex->uw_Flags & TEX_uw_RawPal )
        {
            pst_Palette = TEX_gst_GlobalList.dst_Palette + pst_Tex->w_Width;
            if (pst_Palette->pul_Color)
                L_memcpy( pst_Data->al_PalColor, pst_Palette->pul_Color, 3 * 8 * 4 );
            else
                pst_Data->c_UpdatablePalette = 0;
        }

        i = 0;
        while ( i < GFX_C_Carte_NbMaxGao )
        {
            pst_MatLevel = pst_MatLevel->pst_NextLevel;
            if (!pst_MatLevel ) break;
            pst_Data->al_Texture[i] = pst_MatLevel->s_TextureId;
            i++;
        }
        pst_Data->i_Flags |= GFX_C_Carte_Init;
    }
    
    if ( pst_Data->i_Flags & GFX_C_Carte_Hidden)
        return 1;

   	GFX_NeedGeom(4, 4, 2, 0);

    /* set UV */
    GFX_gpst_Geo->dst_UV[0].fU = 1;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 0;
    GFX_gpst_Geo->dst_UV[1].fV = 0;
    GFX_gpst_Geo->dst_UV[2].fU = 0;
    GFX_gpst_Geo->dst_UV[2].fV = 1;
    GFX_gpst_Geo->dst_UV[3].fU = 1;
    GFX_gpst_Geo->dst_UV[3].fV = 1;

    Vertex = GFX_gpst_Geo->dst_Point;
    
    if (pst_Data->i_Flags & GFX_C_Carte_Pos2)
    {
        x1 = pst_Data->st_AnotherPos[0].x * GDI_gpst_CurDD->st_Camera.f_Width + GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft;
        x2 = pst_Data->st_AnotherPos[1].x * GDI_gpst_CurDD->st_Camera.f_Width + GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft;
        y1 = pst_Data->st_AnotherPos[0].y * GDI_gpst_CurDD->st_Camera.f_Height + GDI_gpst_CurDD->st_Camera.l_ViewportRealTop;
        y2 = pst_Data->st_AnotherPos[1].y * GDI_gpst_CurDD->st_Camera.f_Height + GDI_gpst_CurDD->st_Camera.l_ViewportRealTop;

        C.x = (x1 + x2) * 0.5f;
        C.y = (y1 + y2) * 0.5f;
        
        MATH_InitVector( VCast(Vertex + (pst_Data->c_Pos2Rotation % 4)   ), x1, y1, pst_Data->st_2DPos[0].z );
        MATH_InitVector( VCast(Vertex + ((pst_Data->c_Pos2Rotation + 1) % 4)), x2, y1, pst_Data->st_2DPos[0].z );
        MATH_InitVector( VCast(Vertex + ((pst_Data->c_Pos2Rotation + 2) % 4)), x2, y2, pst_Data->st_2DPos[0].z );
        MATH_InitVector( VCast(Vertex + ((pst_Data->c_Pos2Rotation + 3) % 4)), x1, y2, pst_Data->st_2DPos[0].z );
    }
    else
    {
        pst_CenterGao = pst_Data->apst_Gao[0]; 
    
        pst_World = WOR_World_GetWorldOfObject(pst_CenterGao);
        if (pst_World && pst_World->pst_View)
            MATH_NegVector( &st_Sight, MATH_pst_GetYAxis( &pst_World->pst_View[0].st_ViewPoint ) );
        else
            MATH_CopyVector( &st_Sight, MATH_pst_GetZAxis( &GDI_gpst_CurDD->st_Camera.st_Matrix ) );

        st_Sight.z = 0;
        if ( ( st_Sight.x == 0) && (st_Sight.y == 0) ) return 1;
        MATH_NormalizeEqualVector( &st_Sight );

        /* set vertex */
        x1 = pst_Data->st_2DPos[0].x * GDI_gpst_CurDD->st_Camera.f_Width + GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft;
        x2 = pst_Data->st_2DPos[1].x * GDI_gpst_CurDD->st_Camera.f_Width + GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft;
        y1 = pst_Data->st_2DPos[0].y * GDI_gpst_CurDD->st_Camera.f_Height + GDI_gpst_CurDD->st_Camera.l_ViewportRealTop;
        y2 = pst_Data->st_2DPos[1].y * GDI_gpst_CurDD->st_Camera.f_Height + GDI_gpst_CurDD->st_Camera.l_ViewportRealTop;

        C.x = (x1 + x2) * 0.5f;
        C.y = (y1 + y2) * 0.5f;
    
        MATH_InitVector( &V, x1 - C.x, y1 - C.y, 0 );
        W.x = V.x * st_Sight.y + V.y * st_Sight.x;
        W.y = V.y * st_Sight.y - V.x * st_Sight.x;
        MATH_InitVector( VCast(Vertex    ), W.x + C.x, W.y + C.y, pst_Data->st_2DPos[0].z );

        MATH_InitVector( &V, x2 - C.x, y1 - C.y, 0 );
        W.x = V.x * st_Sight.y + V.y * st_Sight.x;
        W.y = V.y * st_Sight.y - V.x * st_Sight.x;
        MATH_InitVector( VCast(Vertex + 1), W.x + C.x, W.y + C.y, pst_Data->st_2DPos[0].z );
    
        MATH_InitVector( &V, x2 - C.x, y2 - C.y, 0 );
        W.x = V.x * st_Sight.y + V.y * st_Sight.x;
        W.y = V.y * st_Sight.y - V.x * st_Sight.x;
        MATH_InitVector( VCast(Vertex + 2), W.x + C.x, W.y + C.y, pst_Data->st_2DPos[0].z );
    
        MATH_InitVector( &V, x1 - C.x, y2 - C.y, 0 );
        W.x = V.x * st_Sight.y + V.y * st_Sight.x;
        W.y = V.y * st_Sight.y - V.x * st_Sight.x;
        MATH_InitVector( VCast(Vertex + 3), W.x + C.x, W.y + C.y, pst_Data->st_2DPos[0].z );
    }

    /* on/off effect */
    if ( pst_Data->i_Flags & (GFX_C_Carte_TurnOn | GFX_C_Carte_TurnOff) )
    {
        pst_Data->f_CurEffectTime -= TIM_gf_dt;
        f_Time = pst_Data->f_CurEffectTime / pst_Data->f_EffectTime;

        if (pst_Data->i_Flags & GFX_C_Carte_TurnOff)
        {
            if (*(LONG *) &f_Time & 0x80000000 )
            {
                pst_Data->i_Flags &= ~GFX_C_Carte_TurnOff;
                pst_Data->i_Flags |= GFX_C_Carte_Hidden;
                f_Time = 0;
            }
        }
        else /* turn on */
        {
            if (*(LONG *) &pst_Data->f_CurEffectTime & 0x80000000 )
            {
                pst_Data->i_Flags &= ~GFX_C_Carte_TurnOn;
                f_Time = 1;
            }
            else
            {
                f_Time = 1.0f - f_Time;
            }
        }

        C.z = pst_Data->st_2DPos[0].z;
        for (i = 0; i < 4; i++)
        {
            MATH_SubVector( &V, VCast( Vertex + i ), &C );
            MATH_ScaleEqualVector( &V, f_Time );
            MATH_AddVector( VCast( Vertex + i ), &C, &V );
        }
    }

    for (i = 0; i< 4; i++)
        CAM_2Dto3D( &GDI_gpst_CurDD->st_Camera, VCast(&Vertex[i]), VCast(&Vertex[i]) );

    /* set triangles */
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[0] = 0;
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[1] = 1;
	GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[2] = 2;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[0] = 0;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[1] = 1;
    GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[2] = 2;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[0] = 2;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[1] = 3;
	GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[2] = 0;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[0] = 2;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[1] = 3;
    GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[2] = 0;

    if (i_LockMat) goto nomat;

    /*===  set material ===*/
    pst_Data->f_CurTime -= TIM_gf_dt;
    if (pst_Data->f_CurTime < 0)
    {
        pst_Data->f_CurTime = pst_Data->f_BlinkTime;
        if (pst_Data->i_Flags & GFX_C_Carte_BlinkOn)
            pst_Data->i_Flags &= ~GFX_C_Carte_BlinkOn;
        else
            pst_Data->i_Flags |= GFX_C_Carte_BlinkOn;
    }

    pst_MatLevel = pst_Mat->pst_FirstLevel;
    if (!pst_MatLevel ) return 1;

    /* card material */
    pst_MatLevel = pst_MatLevel->pst_NextLevel;
    if (!pst_MatLevel ) return 1;

    if ( ( pst_Data->i_PieceStatus != pst_Data->i_CurPieceStatus ) || (pst_Data->c_UncoverPiece != -1) )
    {
        if (pst_Data->c_UpdatablePalette)
            GFX_Carte_UpdatePalette( pst_Data, pst_MatLevel->s_TextureId );
        pst_Data->i_CurPieceStatus = pst_Data->i_PieceStatus;

        /* a virer quand y'auras toutes les bonnes cartes
        if ( pst_Data->i_PieceStatus != pst_Data->i_CurPieceStatus )
        {
            if (pst_Data->i_Flags & GFX_C_Carte_HideMap)
                MAT_SET_ColorOp( pst_MatLevel->ul_Flags, MAT_Cc_ColorOp_ConstantColor );
            else
                MAT_SET_ColorOp( pst_MatLevel->ul_Flags, MAT_Cc_ColorOp_FullLight );
        }
        else
            MAT_SET_ColorOp( pst_MatLevel->ul_Flags, MAT_Cc_ColorOp_FullLight );
        */
    }
    else
        pst_Data->f_UncoverTime = 1.5;

    if ( pst_Data->i_Flags & GFX_C_Carte_Pos2)
    {
        MAT_SET_Blending( pst_MatLevel->ul_Flags, MAT_Cc_Op_Add);
        MAT_SetScale( &pst_MatLevel->ScaleSPeedPosU, 1.0f );
        MAT_SetScale( &pst_MatLevel->ScaleSPeedPosV, 1.0f );
    }
    else
    {
        MAT_SET_Blending( pst_MatLevel->ul_Flags, MAT_Cc_Op_AlphaDest);
        MAT_SetScale( &pst_MatLevel->ScaleSPeedPosU, pst_Data->f_Ratio );
        MAT_SetScale( &pst_MatLevel->ScaleSPeedPosV, pst_Data->f_Ratio );
    }

    
    if (pst_Data->l_NbGao == 0) goto GFX_endrender;

    if ( pst_Data->i_Flags & GFX_C_Carte_Pos2)
    {
        /* positionnement de la carte */
        MAT_SetPos( &pst_MatLevel->ScaleSPeedPosU, 0 );
        MAT_SetPos( &pst_MatLevel->ScaleSPeedPosV, 0 );

        /* affichage du point de l'objet central */
        if (!(pst_MatLevel = pst_MatLevel->pst_NextLevel) ) goto GFX_endrender;
        
        pst_MatLevel->s_TextureId = (SHORT) pst_Data->al_Texture[ pst_Data->al_Flags[ 0 ] & GFX_C_Carte_TypeMask ];
        pst_Pos = OBJ_pst_GetAbsolutePosition( pst_Data->apst_Gao[0] ); 
        st_Pos.x = -(1.0f - (pst_Pos->x - pst_Data->st_Offset.x) / pst_Data->st_Size.x );
        st_Pos.y = -(1.0f - (pst_Pos->y - pst_Data->st_Offset.y) / pst_Data->st_Size.y );
        MAT_SetPos( &pst_MatLevel->ScaleSPeedPosU, st_Pos.x + pst_Data->st_CursorSize.x );
        MAT_SetPos( &pst_MatLevel->ScaleSPeedPosV, st_Pos.y + pst_Data->st_CursorSize.y );
        pst_Mat->ul_Specular = COLOR_ul_Blend( 0xFF00007F, 0xFF00FFFF, fAbs( fSin( TIM_gf_MainClock * 4) ) );
        MAT_SET_Blending( pst_MatLevel->ul_Flags, MAT_Cc_Op_Sub);
        MAT_SET_ColorOp( pst_MatLevel->ul_Flags, 2 );
        pst_MatLevel->ul_Flags &= ~MAT_Cul_Flag_InActive;
    }
    else
    {
        /* positionnement de la carte */
        pst_Pos = OBJ_pst_GetAbsolutePosition( pst_Data->apst_Gao[0] ); 
        st_Pos.x = ( (1.0f - (pst_Pos->x - pst_Data->st_Offset.x) / pst_Data->st_Size.x ) )/ pst_Data->f_Ratio - 0.5f;
        st_Pos.y = ( (1.0f - (pst_Pos->y - pst_Data->st_Offset.y) / pst_Data->st_Size.y ) ) / pst_Data->f_Ratio - 0.5f;
        MAT_SetPos( &pst_MatLevel->ScaleSPeedPosU, st_Pos.x );
        MAT_SetPos( &pst_MatLevel->ScaleSPeedPosV, st_Pos.y );

        /* affichage du point de l'objet central */
        pst_MatLevel = pst_MatLevel->pst_NextLevel;
        if (!pst_MatLevel ) goto GFX_endrender;
        MAT_SetPos( &pst_MatLevel->ScaleSPeedPosU, -0.5f + pst_Data->st_CursorSize.x );
        MAT_SetPos( &pst_MatLevel->ScaleSPeedPosV, -0.5f + pst_Data->st_CursorSize.y );
        pst_MatLevel->s_TextureId = (SHORT) pst_Data->al_Texture[ pst_Data->al_Flags[ 0 ] & GFX_C_Carte_TypeMask ];
        MAT_SET_ColorOp( pst_MatLevel->ul_Flags, MAT_Cc_ColorOp_FullLight );
        MAT_SET_Blending( pst_MatLevel->ul_Flags, MAT_Cc_Op_AlphaDestPremult);
        pst_Mat->ul_Specular = 0x66000000;
        pst_MatLevel->ul_Flags &= ~MAT_Cul_Flag_InActive;

        /* positionnement des spots des autres GameObjects */
        for (i = 1; i < pst_Data->l_NbGao; i++)
        {
            if (!pst_Data->apst_Gao[i]) goto GFX_endrender;
            pst_MatLevel = pst_MatLevel->pst_NextLevel;
            if (!pst_MatLevel ) goto GFX_endrender;

            if( !(pst_Data->i_Flags & GFX_C_Carte_BlinkOn) && (pst_Data->al_Flags[ i ] & GFX_C_Carte_Blink) )
            {
                pst_MatLevel->ul_Flags |= MAT_Cul_Flag_InActive;
                continue;
            }

            pst_MatLevel->ul_Flags &= ~MAT_Cul_Flag_InActive;

            pst_Pos2 = OBJ_pst_GetAbsolutePosition( pst_Data->apst_Gao[i] ); 
            st_Pos2.x = ( (1.0f - (pst_Pos2->x - pst_Data->st_Offset.x) / pst_Data->st_Size.x ) )/ pst_Data->f_Ratio;
            st_Pos2.y = ( (1.0f - (pst_Pos2->y - pst_Data->st_Offset.y) / pst_Data->st_Size.y ) ) / pst_Data->f_Ratio;
            MAT_SetPos( &pst_MatLevel->ScaleSPeedPosU, st_Pos.x - st_Pos2.x + pst_Data->st_CursorSize.x );
            MAT_SetPos( &pst_MatLevel->ScaleSPeedPosV, st_Pos.y - st_Pos2.y + pst_Data->st_CursorSize.y );
            pst_MatLevel->s_TextureId = (SHORT) pst_Data->al_Texture[ pst_Data->al_Flags[ i ] & GFX_C_Carte_TypeMask ];
        }
    }

    /* desactivation de toutes les couches de matériaux qui ne servent à rien */    
    for( ; ; )
    {
        pst_MatLevel = pst_MatLevel->pst_NextLevel;
        if (!pst_MatLevel ) goto GFX_endrender;
        pst_MatLevel->ul_Flags |= MAT_Cul_Flag_InActive;
    }

nomat:
    
GFX_endrender:

    M_GFX_CheckGeom();

    /* affichage de l'objet */
    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted );
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Carte_Seti( void *p_Data, int _i_Param, int _i_Value )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Carte          *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Carte *) p_Data;    
    
    if (_i_Param == GFX_Carte_FirstPosOnOff ) 
    {
        if ( _i_Value == 1)
        {
            if ( pst_Data->i_Flags & GFX_C_Carte_Pos2 )
            {
                pst_Data->i_Flags &= ~GFX_C_Carte_Pos2;
                pst_Data->i_Flags |= GFX_C_Carte_Hidden;
                //_i_Value = 0;
                //_i_Param = GFX_Carte_TurnOnOff;
            }
        }
        else if (_i_Value == 0)
        {
            if ( !(pst_Data->i_Flags & GFX_C_Carte_Pos2 ) )
            {
                pst_Data->i_Flags |= GFX_C_Carte_Pos2;
                _i_Value = 1;
                _i_Param = GFX_Carte_TurnOnOff;
            }
        }
    }
	if (_i_Param == GFX_Carte_TurnOnOff ) 
    {
        if (_i_Value == 1)
        {
            if ( pst_Data->i_Flags & GFX_C_Carte_Hidden)
            {
                pst_Data->f_CurEffectTime = pst_Data->f_EffectTime;
                pst_Data->i_Flags &= ~GFX_C_Carte_Hidden;
                pst_Data->i_Flags |= GFX_C_Carte_TurnOn;
            }
            else if (pst_Data->i_Flags & GFX_C_Carte_TurnOff)
            {
                pst_Data->f_CurEffectTime = pst_Data->f_EffectTime - pst_Data->f_CurEffectTime;
                pst_Data->i_Flags &= ~GFX_C_Carte_TurnOff;
                pst_Data->i_Flags |= GFX_C_Carte_TurnOn;
            }
        }
        else if (_i_Value == 0)
        {
            if ( !(pst_Data->i_Flags & GFX_C_Carte_Hidden) )
            {
                if (pst_Data->i_Flags & GFX_C_Carte_TurnOn)
                {
                    pst_Data->f_CurEffectTime = pst_Data->f_EffectTime - pst_Data->f_CurEffectTime;
                    pst_Data->i_Flags &= ~GFX_C_Carte_TurnOn;
                    pst_Data->i_Flags |= GFX_C_Carte_TurnOff;
                }
                else if ( !(pst_Data->i_Flags & GFX_C_Carte_TurnOff) )
                {
                    pst_Data->f_CurEffectTime = pst_Data->f_EffectTime;
                    pst_Data->i_Flags |= GFX_C_Carte_TurnOff;
                }
            }
        }
        /*
        else if (_i_Value == 2)
        {
            pst_Data->f_CurEffectTime = pst_Data->f_EffectTime;
            pst_Data->i_Flags &= ~GFX_C_Carte_Hidden;
            pst_Data->i_Flags |= GFX_C_Carte_TurnOn;
        }
        */
    }
    else if (_i_Param == GFX_Carte_ShowMapOnOff ) 
    {
        if ( _i_Value == 1)
        {
            if ( pst_Data->i_Flags & GFX_C_Carte_HideMap )
            {
                pst_Data->i_Flags &= ~GFX_C_Carte_HideMap;
                pst_Data->i_CurPieceStatus = 0xFFFFFFFF;
            }
        }
        else if (_i_Value == 0)
        {
            if ( !(pst_Data->i_Flags & GFX_C_Carte_HideMap) )
            {
                pst_Data->i_Flags |= GFX_C_Carte_HideMap;
                pst_Data->i_CurPieceStatus = 0xFFFFFFFF;
            }
        }
    }
    
}

/*
 =======================================================================================================================
    Specific AI functions : add or remove an object from list (_i_Oper = 1 => add, -1 => remove )
 =======================================================================================================================
 */
void GFX_Carte_Object( GFX_tdst_List *_pst_GFX, USHORT _uw_Id, OBJ_tdst_GameObject *_pst_GO, int _i_Oper, int _i_Type )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Carte          *pst_Data;
    int                     i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GFX_SetCurrent(_pst_GFX, _uw_Id);
	if(!GFX_gpst_Current) return;

    pst_Data = (GFX_tdst_Carte *) GFX_gpst_Current->p_Data;

    /* set object 0 : center of card */
    if (_i_Oper == 0)
    {
        if (!pst_Data->l_NbGao) pst_Data->l_NbGao = 1;
        pst_Data->apst_Gao[0] = _pst_GO;
        pst_Data->al_Flags[0] = _i_Type;
    }
    /* add object */
    else if (_i_Oper == 1)
    {
        if (pst_Data->l_NbGao >= GFX_C_Carte_NbMaxGao) return;
        pst_Data->apst_Gao[ pst_Data->l_NbGao ] = _pst_GO;
        pst_Data->al_Flags[ pst_Data->l_NbGao++ ] = _i_Type;
    }
    /* remove object */
    else if (_i_Oper == -1)
    {
        for (i = 0; i < pst_Data->l_NbGao; i++)
            if ( pst_Data->apst_Gao[i] == _pst_GO ) break;
        if (i == pst_Data->l_NbGao) return;
        pst_Data->apst_Gao[ i ] = pst_Data->apst_Gao[ --pst_Data->l_NbGao ];
        pst_Data->al_Flags[ i ] = pst_Data->al_Flags[ pst_Data->l_NbGao ];
    }
}



#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

