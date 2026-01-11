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
#include "GFX/GFXspeed.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
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
void *GFX_Speed_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Speed *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Speed *) MEM_p_Alloc(sizeof(GFX_tdst_Speed));
    L_memset( pst_Data, 0, sizeof(GFX_tdst_Speed) );
    pst_Data->f_Time = 1;
    pst_Data->f_Size = 1;
    pst_Data->ul_Color = 0xFFFFFFFF;

    *( LONG * ) &pst_Data->st_OldPos.x = -1;

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Speed_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Speed      *pst_Data;
	MATH_tdst_Vector	X;
    MATH_tdst_Vector    v, w;
    MATH_tdst_Vector    P1, P2;
    ULONG               DM;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Speed *) p_Data;

    pst_Data->f_Time -= TIM_gf_dt;
    if (pst_Data->f_Time < 0)
        return 0;

    if (*( LONG *) &pst_Data->st_OldPos.x == -1)
    {
        MATH_TransformVertex( &pst_Data->st_OldPos, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix, &pst_Data->st_Pos );
        return 1;
    }
    
    if (pst_Data->st_OldPos.z < 0)
        return 0;

    MATH_TransformVertex( &P1, &GDI_gpst_CurDD->st_Camera.st_Matrix, &pst_Data->st_OldPos );
    MATH_CopyVector( &P2, &pst_Data->st_Pos );
    MATH_SubVector( &v, &P2, &P1 );

    MATH_TransformVertex( &pst_Data->st_OldPos, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix, &pst_Data->st_Pos );
    
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
    MATH_SubVector( &w, &P2, &GDI_gpst_CurDD->st_Camera.st_Matrix.T );
    MATH_CrossProduct( &X, &v, &w );
    MATH_NormalizeEqualVector( &X );
    MATH_ScaleEqualVector( &X, pst_Data->f_Size * 0.5f );

    MATH_AddVector(VCast( GFX_gpst_Geo->dst_Point), &P2, &X);
    MATH_AddVector(VCast( GFX_gpst_Geo->dst_Point + 1), &P1, &X);
    MATH_SubVector(VCast( GFX_gpst_Geo->dst_Point + 2), &P1, &X);
    MATH_SubVector(VCast( GFX_gpst_Geo->dst_Point + 3), &P2, &X);
	
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

    GFX_gpst_Geo->dul_PointColors[1] = pst_Data->ul_Color;
    GFX_gpst_Geo->dul_PointColors[2] = pst_Data->ul_Color;
    GFX_gpst_Geo->dul_PointColors[3] = pst_Data->ul_Color;
    GFX_gpst_Geo->dul_PointColors[4] = pst_Data->ul_Color;

	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient);
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    return 1;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
