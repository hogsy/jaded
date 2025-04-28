/*$T GFXString.c GC! 1.081 11/08/01 11:15:46 */


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
#include "GDInterface/GDIrasters.h"
#include "GFX/GFX.h"
#include "GFX/GFXcarte.h"
#include "GEOmetric/GEODebugObject.h"
#include "STRing/STRstruct.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "GFX/GFXstring.h"

#ifdef _XBOX
#include "GX8/RASter/Gx8_CheatFlags.h"
#include "GX8/Gx8.h"
#endif // _XBOX

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
    Functions for gao
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Gao_Create(OBJ_tdst_GameObject *pst_Gao)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Gao	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_Gao) return NULL;
	if(!pst_Gao->pst_Base) return NULL;
	if(!pst_Gao->pst_Base->pst_Visu) return NULL;

	pst_Data = (GFX_tdst_Gao *) MEM_p_Alloc(sizeof(GFX_tdst_Gao));
	pst_Data->pst_GO = pst_Gao;
	pst_Gao->pst_Base->pst_Visu->c_DisplayPos = GRO_DisplayPos_Interface;
	pst_Data->f_Focale = 0;

	return (void *) pst_Data;
}

extern void GRO_Render(OBJ_tdst_GameObject *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */

__inline static void _ifVisibleCall_GRO_Render( OBJ_tdst_GameObject* _pst_GO )
{
    if(_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible)
        GRO_Render(_pst_GO);
}

void GFX_Gao_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Gao	*pst_Data;
	float			f_SaveFov;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef GX8_BENCH 
	if (g_iNoSTR) return; 
#endif // GX8_BENCH

	pst_Data = (GFX_tdst_Gao *) p_Data;

	if(*(LONG *) &pst_Data->f_Focale)
	{
		/* méthode plus rapide mais à régler
		MATH_tdst_Vector	P, PNew, S, SNew;
		float				f;

		MATH_CopyVector( &P, OBJ_pst_GetAbsolutePosition( pst_Data->pst_GO ) );
		MATH_GetScale( &S, OBJ_pst_GetAbsoluteMatrix( pst_Data->pst_GO ) );

		f = GDI_gpst_CurDD->st_Camera.f_FieldOfVision / 2;
		f = fSin( f ) / fCos( f );

		PNew.x = P.x * f;
		PNew.y = P.y;
		PNew.z = P.z * f ;
		MATH_CopyVector( OBJ_pst_GetAbsolutePosition( pst_Data->pst_GO ), &PNew );

		MATH_ScaleVector( &SNew, &S, f );
		MATH_SetScale(OBJ_pst_GetAbsoluteMatrix( pst_Data->pst_GO ), &SNew );

		_ifVisibleCall_GRO_Render( pst_Data->pst_GO );

		MATH_CopyVector( OBJ_pst_GetAbsolutePosition( pst_Data->pst_GO ), &P );
		MATH_SetScale(OBJ_pst_GetAbsoluteMatrix( pst_Data->pst_GO ), &S );
		*/


		f_SaveFov = GDI_gpst_CurDD->st_Camera.f_FieldOfVision;
		GDI_gpst_CurDD->st_Camera.f_FieldOfVision = pst_Data->f_Focale;
		GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix(&GDI_gpst_CurDD->st_Camera);

        _ifVisibleCall_GRO_Render( pst_Data->pst_GO );

		GDI_gpst_CurDD->st_Camera.f_FieldOfVision = f_SaveFov;
		GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix(&GDI_gpst_CurDD->st_Camera);
	}
    else
    {
        _ifVisibleCall_GRO_Render( pst_Data->pst_GO );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Gao_Destroy(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Gao	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Gao *) p_Data;
	pst_Data->pst_GO->pst_Base->pst_Visu->c_DisplayPos = 0;
}

/*$4
 ***********************************************************************************************************************
    Functions for string
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Str_Create(OBJ_tdst_GameObject *pst_Gao)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_String *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_Gao) return NULL;
	if(!pst_Gao->pst_Base) return NULL;
	if(!pst_Gao->pst_Base->pst_Visu) return NULL;
	if(!pst_Gao->pst_Base->pst_Visu->pst_Object) return NULL;
	if(pst_Gao->pst_Base->pst_Visu->pst_Object->i->ul_Type != GRO_2DText) return NULL;

	pst_Data = (GFX_tdst_String *) MEM_p_Alloc(sizeof(GFX_tdst_String));
	pst_Data->pst_GO = pst_Gao;
	pst_Data->i_NbLetter = 0;
	pst_Data->ul_Color1 = 0xFFFFFFFF;
    pst_Data->ul_Color2 = 0xFFFFFF00;
	pst_Data->pst_Vertex = NULL;
	pst_Data->pst_UV = NULL;
    pst_Data->f_Time = 5;

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Str_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_String				*pst_Data;
	GEO_tdst_IndexedTriangle	*T;
	int							i;
	ULONG						DM, col1, col2;
    MATH_tdst_Vector            st_2D;
    float                       factor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_String *) p_Data;

	if(!pst_Data->i_NbLetter) return 1;

    pst_Data->f_Time -= TIM_gf_dt;
    if (pst_Data->f_Time < 0.0f)
        return 0;

    if (pst_Data->f_Time > 4.8f )
        factor = (5.0f - pst_Data->f_Time) * 5.0f;
    else
        factor = 1.0f;

    col1 = COLOR_ul_Blend( 0, pst_Data->ul_Color1, factor );
    col2 = COLOR_ul_Blend( 0, pst_Data->ul_Color2, factor );

	GFX_NeedGeom(pst_Data->i_NbLetter * 4, pst_Data->i_NbLetter * 4, pst_Data->i_NbLetter * 2, 1);

	/* set UV */
	L_memcpy(GFX_gpst_Geo->dst_UV, pst_Data->pst_UV, pst_Data->i_NbLetter * 4 * sizeof(GEO_tdst_UV));

	/* Set vertex */
    for (i = 0; i< pst_Data->i_NbLetter * 4; i++)
    {
        st_2D.x = GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft + pst_Data->pst_Vertex[i].x * GDI_gpst_CurDD->st_Camera.f_Width;
        st_2D.y = GDI_gpst_CurDD->st_Camera.l_ViewportRealTop + pst_Data->pst_Vertex[i].y * GDI_gpst_CurDD->st_Camera.f_Height;
        st_2D.z = pst_Data->pst_Vertex[i].z;
        CAM_2Dto3D( &GDI_gpst_CurDD->st_Camera, VCast(&GFX_gpst_Geo->dst_Point[i]), &st_2D );
    }

	/* Set triangle */
	T = GFX_gpst_Geo->dst_Element->dst_Triangle;
	for(i = 0; i < pst_Data->i_NbLetter; i++)
	{
		T->auw_UV[0] = T->auw_Index[0] = (unsigned short) i * 4;
		T->auw_UV[1] = T->auw_Index[1] = (unsigned short) i * 4 + 1;
		T->auw_UV[2] = T->auw_Index[2] = (unsigned short) i * 4 + 2;
		T++;
		T->auw_UV[0] = T->auw_Index[0] = (unsigned short) i * 4 + 2;
		T->auw_UV[1] = T->auw_Index[1] = (unsigned short) i * 4 + 3;
		T->auw_UV[2] = T->auw_Index[2] = (unsigned short) i * 4;
		T++;

		GFX_gpst_Geo->dul_PointColors[i * 4 + 1] = col1;
		GFX_gpst_Geo->dul_PointColors[i * 4 + 2] = col2;
		GFX_gpst_Geo->dul_PointColors[i * 4 + 3] = col2;
		GFX_gpst_Geo->dul_PointColors[i * 4 + 4] = col1;
	}

    M_GFX_CheckGeom();

	DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
	GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_Lighted);
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
	GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Str_Destroy(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_String *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_String *) p_Data;

	if(pst_Data->i_NbLetter)
	{
		MEM_Free(pst_Data->pst_Vertex);
		pst_Data->pst_Vertex = NULL;
		MEM_Free(pst_Data->pst_UV);
		pst_Data->pst_UV = NULL;
		pst_Data->i_NbLetter = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Str_Seti(void *p_Data, int _i_Param, int _i_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_String			*pst_Data;
	STR_tdst_Struct			*pst_Str;
	STR_tdst_String			*pst_String;
	float					x, y, w, h, dx, dy;
	GEO_Vertex				*V;
	GEO_tdst_UV				*UV;
	STR_tdst_Letter			*pst_Letter;
	STR_tdst_FontLetterDesc *pst_L;
	int						l;
    MATH_tdst_Vector        st_Min,st_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Param != GFX_StrParamsi_First) return;

	pst_Data = (GFX_tdst_String *) p_Data;
	pst_Str = STR_pst_Get(pst_Data->pst_GO);
	if(!pst_Str) return;

	GFX_Str_Destroy(p_Data);

	l = STR_l_GetLength(pst_Str, _i_Value);
	if(l == 0) return;

	pst_String = STR_pst_GetString(pst_Str, _i_Value);
	if(!pst_String) return;

	pst_Data->pst_Vertex = (GEO_Vertex *) MEM_p_Alloc(4 * l * sizeof(GEO_Vertex));
	pst_Data->pst_UV = (GEO_tdst_UV *) MEM_p_Alloc(4 * l * sizeof(GEO_tdst_UV));
    pst_Data->i_NbLetter = l;

	pst_String = STR_pst_GetString(pst_Str, _i_Value);
	pst_Letter = pst_Str->dst_Letter + pst_String->uw_First;
	V = pst_Data->pst_Vertex;
	UV = pst_Data->pst_UV;

    dx = pst_String->st_Pivot.x;
    dy = pst_String->st_Pivot.y;

    if ( pst_String->uw_Flags & (STR_Cuw_SF_XJustifyMask | STR_Cuw_SF_YJustifyMask))
    {
        STR_GetRectShort(pst_Str, _i_Value, 0, -1, &st_Min, &st_Max);
        st_Min.x /= pst_Str->uw_SW;
        st_Min.y /= pst_Str->uw_SH;
        st_Max.x /= pst_Str->uw_SW;
        st_Max.y /= pst_Str->uw_SH;

        if((pst_String->uw_Flags & STR_Cuw_SF_XJustifyMask) == STR_Cuw_SF_CenterWidth)
            dx -= (st_Min.x + st_Max.x) / 2;
        else if((pst_String->uw_Flags & STR_Cuw_SF_XJustifyMask) == STR_Cuw_SF_Right)
            dx -= (st_Max.x - st_Min.x);

        if((pst_String->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_CenterHeight)
            dy -= (st_Min.y + st_Max.y) / 2;
        else if((pst_String->uw_Flags & STR_Cuw_SF_YJustifyMask) == STR_Cuw_SF_Bottom)
            dy -= (st_Max.y - st_Min.y);
    }

	while(l)
	{
		STR_M_SetFont(pst_Str, ((pst_Letter->ul_Flags & STR_Cul_LF_PageMask) >> STR_Cul_LF_PageShift));
		pst_L = pst_Str->pst_Font->pst_Letter + STR_M_A2I(pst_Letter->ul_Flags);
        x = dx + ((float) pst_Letter->x / (float) pst_Str->uw_SW );
        y = dy + ((float) pst_Letter->y / (float) pst_Str->uw_SH );
        w = ((float) pst_Letter->w / (float) pst_Str->uw_SW );
        h = ((float) pst_Letter->h / (float) pst_Str->uw_SH );

		V->x = x;
		V->y = y;
		V->z = 0.06f;
		UV->fU = pst_L->f_U[0];
		UV->fV = pst_L->f_V[1];
		UV++;
		V++;

		V->x = x;
		V->y = ( y + h );
		V->z = 0.06f;
		UV->fU = pst_L->f_U[0];
		UV->fV = pst_L->f_V[0];
		UV++;
		V++;

		V->x = x + w;
		V->y = ( y + h );
		V->z = 0.06f;
		UV->fU = pst_L->f_U[1];
		UV->fV = pst_L->f_V[0];
		UV++;
		V++;

		V->x = x + w;
		V->y = y;
		V->z = 0.06f;
		UV->fU = pst_L->f_U[1];
		UV->fV = pst_L->f_V[1];
		UV++;
		V++;

		pst_Letter++;
		l--;
	}
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
