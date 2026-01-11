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
#include "GFX/GFX.h"
#include "GFX/GFXwater.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
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
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Water_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Water *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Water *) MEM_p_Alloc(sizeof(GFX_tdst_Water));
    L_memset( pst_Data, 0, sizeof(GFX_tdst_Water) );
    pst_Data->f_Time = 5;
    pst_Data->f_Size = 1;
    pst_Data->f_EndSize = 4;
    pst_Data->ul_Color = 0xFFFFFFFF;
    pst_Data->f_YoX = 1;

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Water_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Water      *pst_Data;
	MATH_tdst_Vector	X, Y;
    ULONG               ul_Color;
    float               f_Size, f_Norm;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Water *) p_Data;

    pst_Data->f_CurTime += TIM_gf_dt;
    if (pst_Data->f_CurTime > pst_Data->f_Time)
        return 0;

    f_Size = pst_Data->f_CurTime / pst_Data->f_Time;
    ul_Color = LIGHT_ul_Interpol2Colors( pst_Data->ul_Color, 0, f_Size );
    f_Size = pst_Data->f_Size + f_Size * (pst_Data->f_EndSize - pst_Data->f_Size );

    pst_Data->st_Pos.x += pst_Data->st_Normal.x * TIM_gf_dt;
    pst_Data->st_Pos.y += pst_Data->st_Normal.y * TIM_gf_dt;

	GFX_NeedGeom(4, 4, 2, 1);

    /* set UV */
    GFX_gpst_Geo->dst_UV[0].fU = 0;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 1;
    GFX_gpst_Geo->dst_UV[1].fV = 0;
    GFX_gpst_Geo->dst_UV[2].fU = 1;
    GFX_gpst_Geo->dst_UV[2].fV = 1;
    GFX_gpst_Geo->dst_UV[3].fU = 0;
    GFX_gpst_Geo->dst_UV[3].fV = 1;

    /* set vertex */
    f_Norm = MATH_f_NormVector( &pst_Data->st_Normal );
    MATH_ScaleVector( &X, &pst_Data->st_Normal, -f_Size / f_Norm );
    MATH_CrossProduct( &Y, &X, &MATH_gst_BaseVectorK ); 
    MATH_ScaleEqualVector( &Y, pst_Data->f_YoX );

	MATH_SubScaleVector(VCast( GFX_gpst_Geo->dst_Point), &pst_Data->st_Pos, &X, 0.5);
    MATH_AddScaleVector(VCast( GFX_gpst_Geo->dst_Point), VCast( GFX_gpst_Geo->dst_Point), &Y, 0.5);
	MATH_AddVector(VCast( GFX_gpst_Geo->dst_Point + 1), VCast( GFX_gpst_Geo->dst_Point), &X);
    MATH_SubVector(VCast( GFX_gpst_Geo->dst_Point + 2), VCast( GFX_gpst_Geo->dst_Point + 1), &Y);
    MATH_SubVector(VCast( GFX_gpst_Geo->dst_Point + 3), VCast( GFX_gpst_Geo->dst_Point + 2), &X);
	
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

    GFX_gpst_Geo->dul_PointColors[1] = ul_Color;
    GFX_gpst_Geo->dul_PointColors[2] = ul_Color;
    GFX_gpst_Geo->dul_PointColors[3] = ul_Color;
    GFX_gpst_Geo->dul_PointColors[4] = ul_Color;

	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    return 1;
}

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Water2_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Water2 *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Water2 *) MEM_p_Alloc(sizeof(GFX_tdst_Water2));
    L_memset( pst_Data, 0, sizeof(GFX_tdst_Water2) );
	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Water2_Destroy(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Water2 *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

    if (!p_Data) return;
    pst_Data = (GFX_tdst_Water2 *) p_Data;
    if (pst_Data->dst_Water)
	    MEM_Free( pst_Data->dst_Water );
    pst_Data->dst_Water = NULL;
    pst_Data->l_NbCur = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Water2_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GFX_tdst_Water      *pst_Water;
	GFX_tdst_Water2     *pst_Data;
	MATH_tdst_Vector	X, Y;
    ULONG               ul_Color, DM;
    float               f_Size, f_Norm;
    GEO_Vertex		    *V;
    GEO_tdst_IndexedTriangle *T;
    ULONG               *RLI;
    int                 i, Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Water2 *) p_Data;

    if (pst_Data->l_NbCur == 0)
        return 1;

    GFX_NeedGeom( pst_Data->l_NbCur * 3, 3, pst_Data->l_NbCur, 1);

    /* set UV */
    GFX_gpst_Geo->dst_UV[0].fU = 0;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 1;
    GFX_gpst_Geo->dst_UV[1].fV = 0;
    GFX_gpst_Geo->dst_UV[2].fU = 1;
    GFX_gpst_Geo->dst_UV[2].fV = 1;

    V = GFX_gpst_Geo->dst_Point;
    T = GFX_gpst_Geo->dst_Element->dst_Triangle;
    RLI = GFX_gpst_Geo->dul_PointColors + 1;
    Index = 0;

    for (i = 0; i < pst_Data->l_NbCur; )
    {
        pst_Water = pst_Data->dst_Water + i;

        f_Size = pst_Water->f_CurTime / pst_Water->f_Time;
        ul_Color = LIGHT_ul_Interpol2Colors( pst_Water->ul_Color, 0, f_Size );
        f_Size = pst_Water->f_Size + f_Size * (pst_Water->f_EndSize - pst_Water->f_Size );

        pst_Water->st_Pos.x += pst_Water->st_Normal.x * TIM_gf_dt;
        pst_Water->st_Pos.y += pst_Water->st_Normal.y * TIM_gf_dt;

        /* set vertex */
        f_Norm = MATH_f_NormVector( &pst_Water->st_Normal );
        MATH_ScaleVector( &X, &pst_Water->st_Normal, -f_Size / f_Norm );
        MATH_CrossProduct( &Y, &X, &MATH_gst_BaseVectorK ); 
        MATH_ScaleEqualVector( &Y, pst_Water->f_YoX );

	    MATH_SubScaleVector(VCast( V ), &pst_Water->st_Pos, &X, 0.5);
        MATH_AddEqualVector(VCast( V ), &Y);
	    MATH_AddVector(VCast( V + 1), VCast( V ), &X);
        MATH_SubVector(VCast( V + 2), VCast( V + 1), &Y);
        V += 3;
	    
	    T->auw_Index[0] = (unsigned short)Index + 0;
	    T->auw_Index[1] = (unsigned short)Index + 1;
	    T->auw_Index[2] = (unsigned short)Index + 2;
        T->auw_UV[0] = 0;
        T->auw_UV[1] = 1;
        T->auw_UV[2] = 2;
        T++;
        Index += 3;

        *RLI++ = ul_Color;
        *RLI++ = ul_Color;
        *RLI++ = ul_Color;

        pst_Water->f_CurTime += TIM_gf_dt;
        if (pst_Water->f_CurTime > pst_Water->f_Time)
        {
            pst_Data->l_NbCur--;
            if (i < pst_Data->l_NbCur)
                L_memcpy( pst_Water, pst_Data->dst_Water + pst_Data->l_NbCur, sizeof( GFX_tdst_Water ) );
            continue;
        }
        i++;
    }

    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~GDI_Cul_DM_TestBackFace;
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Water2_Seti( void *p_Data, int i_Param, int w_Offset, int i_Value )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GFX_tdst_Water2     *pst_Data;
    GFX_tdst_Water      *pst_Water;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    pst_Data = (GFX_tdst_Water2 *) p_Data;

    if ( i_Param == GFX_Water2Add)
    {
        if (pst_Data->l_NbCur < pst_Data->l_NbMax )
        {
            if (!pst_Data->dst_Water)
                pst_Data->dst_Water = (GFX_tdst_Water *) MEM_p_Alloc( sizeof( GFX_tdst_Water ) * pst_Data->l_NbMax );
            pst_Data->l_NbCur++;
        }
        pst_Water = pst_Data->dst_Water + (pst_Data->l_NbCur - 1);
        pst_Water->f_CurTime = 0;
        return;
    }

    if ( !pst_Data->l_NbCur ) return;

    pst_Water = pst_Data->dst_Water + (pst_Data->l_NbCur - 1);
    *(int *) ( ((char *) pst_Water) + w_Offset) = i_Value;
    
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Water2_Setf( void *p_Data, int i_Param, int w_Offset, float f_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GFX_tdst_Water2     *pst_Data;
    GFX_tdst_Water      *pst_Water;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GFX_tdst_Water2 *) p_Data;

    if ( !pst_Data->l_NbCur ) return;

    pst_Water = pst_Data->dst_Water + (pst_Data->l_NbCur - 1);
    *(float *) ( ((char *) pst_Water) + w_Offset) = f_Value;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Water2_Setv( void *p_Data, int i_Param, int w_Offset, MATH_tdst_Vector *V )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GFX_tdst_Water2     *pst_Data;
    GFX_tdst_Water      *pst_Water;
    MATH_tdst_Vector *dst;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GFX_tdst_Water2 *) p_Data;

    if ( !pst_Data->l_NbCur ) return;

    pst_Water = pst_Data->dst_Water + (pst_Data->l_NbCur - 1);
    dst = (MATH_tdst_Vector *) ( ((char *) pst_Water) + w_Offset );
    MATH_CopyVector( dst, V );
}


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

