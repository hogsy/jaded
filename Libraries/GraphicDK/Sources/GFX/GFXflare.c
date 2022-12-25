/*$T GFXflare.c GC! 1.081 09/19/01 17:21:57 */


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
#include "GDInterface/GDIrequest.h"
#include "GFX/GFX.h"
#include "GFX/GFXflare.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTlinear.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/COLlision/COLray.h"

#if defined(_XBOX)
#include "Gx8/Gx8flare.h" 
#endif

#if defined(_PC_RETAIL)
//#include "Dx9/Dx9flare.h" 
#endif

#ifdef PSX2_TARGET
/* mamagouille */
#include "PSX2debug.h"
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
extern GFX_tdst_List	**GFX_ppst_CurrentList;

/*$4
 ***********************************************************************************************************************
    Flare Functions
 ***********************************************************************************************************************
 */

char					FLARE_c_MaxCounter = 5;
char					FLARE_c_CurCounter = 0;
char					FLARE_c_Tested = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Flare_Create(int _i_Complex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_ComplexFlare	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_Complex)
		pst_Data = (GFX_tdst_ComplexFlare *) MEM_p_Alloc(sizeof(GFX_tdst_ComplexFlare));
	else
		pst_Data = (GFX_tdst_ComplexFlare *) MEM_p_Alloc(sizeof(GFX_tdst_Flare));

	MATH_InitVectorToZero(&pst_Data->st_Pos);
	MATH_InitVectorToZero(&pst_Data->st_Offset);
	pst_Data->f_Size = 0.5f;
	pst_Data->f_HiddenTime = 0;
	pst_Data->f_HiddenSpeed = 3;
	pst_Data->l_Flags = GFX_Flare_Hidden | GFX_Flare_HideChangeColor | GFX_Flare_DepthChangeSize | GFX_Flare_Hide;

	if(_i_Complex)
	{
		pst_Data->l_Flags |= GFX_Flare_Complex;

		pst_Data->af_Scale[0] = 0.06f;
		pst_Data->af_Scale[1] = 0.15f;
		pst_Data->af_Scale[2] = 0.30f;
		pst_Data->af_Scale[3] = 0.07f;
		pst_Data->af_Scale[4] = 0.06f;
		pst_Data->af_Scale[5] = 0.11f;
		pst_Data->af_Scale[6] = 0.03f;
		pst_Data->af_Scale[7] = 0.06f;
		pst_Data->af_Scale[8] = 0.04f;

		pst_Data->af_Pos[0] = -.15f;
		pst_Data->af_Pos[1] = 0;
		pst_Data->af_Pos[2] = .25f;
		pst_Data->af_Pos[3] = .4f;
		pst_Data->af_Pos[4] = .5f;
		pst_Data->af_Pos[5] = .625f;
		pst_Data->af_Pos[6] = .7f;
		pst_Data->af_Pos[7] = .8f;
		pst_Data->af_Pos[8] = 1;

		pst_Data->al_Id[0] = 3;
		pst_Data->al_Id[1] = 2;
		pst_Data->al_Id[2] = 1;
		pst_Data->al_Id[3] = 3;
		pst_Data->al_Id[4] = 0;
		pst_Data->al_Id[5] = 4;
		pst_Data->al_Id[6] = 4;
		pst_Data->al_Id[7] = 4;
		pst_Data->al_Id[8] = 4;

		pst_Data->aul_Color[0] = 0xFF009999;
		pst_Data->aul_Color[1] = 0xFF006666;
		pst_Data->aul_Color[2] = 0xFF005050;
		pst_Data->aul_Color[3] = 0xFF005050;
		pst_Data->aul_Color[4] = 0xFF005050;
		pst_Data->aul_Color[5] = 0xFF008080;
		pst_Data->aul_Color[6] = 0xFF009999;
		pst_Data->aul_Color[7] = 0xFF006666;
		pst_Data->aul_Color[8] = 0xFF003333;
	}
	else
	{
		((GFX_tdst_Flare *) pst_Data)->l_Id = 0;
		((GFX_tdst_Flare *) pst_Data)->ul_DeathColor = 0;
        ((GFX_tdst_Flare *) pst_Data)->ul_Color = 0xFFFFFFFF;
	}

	return (void *) pst_Data;
}

ULONG	GFXF_ul_Color;
float	GFXF_f_depth;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Flare_TestDepth(GFX_tdst_ComplexFlare *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	MATH_tdst_Vector	v, w, vcam;
	float				dist, xmin, xmax, ymin, ymax;
	float				f_4Factor[4][2] = { { -0.5, -0.5 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };
	static float		f_9Factor[9][2] =
	{
		{ -1, -1 },
		{ 1, 0 },
		{ 1, 0 },
		{ 0, 1 },
		{ -1, 0 },
		{ -1, 0 },
		{ 0, 1 },
		{ 1, 0 },
		{ 1, 0 }
	};
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_Data->l_Flags & GFX_Flare_PosDontCare))
	{
		MATH_SubEqualVector(&_pst_Data->st_OldPos, &_pst_Data->st_Pos);
		dist = MATH_f_SqrVector(&_pst_Data->st_OldPos);
		MATH_CopyVector(&_pst_Data->st_OldPos, &_pst_Data->st_Pos);
		if(dist > 0.2f)
		{
			_pst_Data->l_Flags |= GFX_Flare_Hidden;
			_pst_Data->f_HiddenTime = 0;
			return 1;
		}
	}

	MATH_AddVector(&w, &_pst_Data->st_Pos, &_pst_Data->st_Offset);
	SOFT_TransformAndProject(&v, &w, 1, &GDI_gpst_CurDD->st_Camera);

	if((*(LONG *) &v.z) & 0x80000000)
	{
		_pst_Data->l_Flags |= GFX_Flare_Hidden;
		_pst_Data->f_HiddenTime = 0;
		return 1;
	}

	xmin = (float) lMax(GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft, 0);
	xmax = (float) lMin
		(
			GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft + (LONG) GDI_gpst_CurDD->st_Camera.f_Width,
			GDI_gpst_CurDD->st_Device.l_Width
		);
	ymin = (float) lMax(0, GDI_gpst_CurDD->st_Camera.l_ViewportRealTop);
	ymax = (float) lMin
		(
			GDI_gpst_CurDD->st_Device.l_Height,
			GDI_gpst_CurDD->st_Camera.l_ViewportRealTop + (LONG) GDI_gpst_CurDD->st_Camera.f_Height
		);

	if((_pst_Data->l_Flags & GFX_Flare_PointMask) == GFX_Flare_1Point)
	{
		if((v.x < xmin) || (v.x > xmax) || (v.y < ymin) || (v.y > ymax)) return 1;

		/*$off
        if (castray)
        {
				
				WOR_tdst_World		*pst_World;
				float				f_Norm;
				int					b_visible;
				

				pst_World = WOR_World_GetWorldOfObject( (OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner);

				MATH_SubVector(&w, &_pst_Data->st_Pos, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
				f_Norm = MATH_f_NormVector(&w);
				w.x /= f_Norm;
				w.y /= f_Norm;
				w.z /= f_Norm;
				COL_Visual_RayCast
				(
				    pst_World,
					NULL,
					&GDI_gpst_CurDD->st_Camera.st_Matrix.T,
					&w,
					f_Norm,
					0xFFFFFFFF,
					0,
					OBJ_C_OR_OR_IdentityFlags,
					&b_visible,
					FALSE
			    );
				if (!b_visible) return 1;
			
        }
        else
        /*$on*/
		{
						MATH_TransformVertex(&w, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix, &_pst_Data->st_Pos);
						GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_ReadPixel, (ULONG) & v);
						if(w.z > GFXF_f_depth) return 1;
		}
		_pst_Data->f_Ratio = 1;
		return 0;
	}

	MATH_TransformVertex(&vcam, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix, &w);
	MATH_TransformVertex(&w, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix, &_pst_Data->st_Pos);
	_pst_Data->f_Ratio = 0;

	if((_pst_Data->l_Flags & GFX_Flare_PointMask) == GFX_Flare_4Points)
	{
		for(i = 0; i < 4; i++)
		{
			vcam.x += f_4Factor[i][0] * _pst_Data->f_Interval;
			vcam.y += f_4Factor[i][1] * _pst_Data->f_Interval;
			SOFT_Project(&v, &vcam, 1, &GDI_gpst_CurDD->st_Camera);

			if((v.x < xmin) || (v.x > xmax) || (v.y < ymin) || (v.y > ymax)) continue;
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_ReadPixel, (ULONG) & v);
			if(w.z > GFXF_f_depth) continue;
			_pst_Data->f_Ratio += 0.25f;
		}

		return (_pst_Data->f_Ratio == 0) ? 1 : 0;
	}

	/* 9 points */
	for(i = 0; i < 9; i++)
	{
		vcam.x += f_9Factor[i][0] * _pst_Data->f_Interval;
		vcam.y += f_9Factor[i][1] * _pst_Data->f_Interval;
		SOFT_Project(&v, &vcam, 1, &GDI_gpst_CurDD->st_Camera);

		if((v.x < xmin) || (v.x > xmax) || (v.y < ymin) || (v.y > ymax)) continue;
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_ReadPixel, (ULONG) & v);
		if(w.z > GFXF_f_depth) continue;
		_pst_Data->f_Ratio += 0.1111f;
	}

	return (_pst_Data->f_Ratio == 0) ? 1 : 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Flare_Test( void *p_Data )
{
	GFX_tdst_ComplexFlare	*pst_Data;

	pst_Data = (GFX_tdst_ComplexFlare *) p_Data;

	if (!(pst_Data->l_Flags & GFX_Flare_Hide))
		return;

	if(((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis == 0)
		return;

	pst_Data->l_Flags &= ~0x08000000;
	if (GFX_i_Flare_TestDepth(pst_Data) )
		pst_Data->l_Flags |= 0x08000000;
	else
		pst_Data->l_Flags &= ~0x08000000;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Flare_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_ComplexFlare	*pst_Data;
	MATH_tdst_Vector		V0, V, X, Y, X0, Y0;
	MATH_tdst_Vector		A;
	float					t, f_Size;
	int						i, i_Hidden;
	ULONG					DM, ul_Color;
	static float			sf_Factor = 0.2f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_ComplexFlare *) p_Data;

	/* test si le flare est dans un secteur invisible */
	if( ((pst_Data->l_Flags & GFX_Flare_NoLODVis) == 0) && ((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis == 0)
	{
		pst_Data->l_Flags |= GFX_Flare_Hidden;
		pst_Data->f_HiddenTime = 0;
		return;
	}

	/* test la visibilité si necessaire */
	if (pst_Data->l_Flags & GFX_Flare_Hide)
	{
        if ( GDI_gpst_CurDD->ul_DisplayFlags & GDI_cul_DF_DepthReadBeforeFlip)
            i_Hidden = pst_Data->l_Flags & 0x08000000;
        else
		    i_Hidden = GFX_i_Flare_TestDepth(pst_Data);

		//if(i_Hidden == -1) goto zap;

		if(i_Hidden)
		{
			if(!(pst_Data->l_Flags & GFX_Flare_Hidden))
			{
			    pst_Data->l_Flags |= GFX_Flare_Hidden;
				if(pst_Data->f_HiddenSpeed)
				    pst_Data->f_HiddenTime = 1;
				else
					pst_Data->f_HiddenTime = 0;
			}
			else
			{
				pst_Data->f_HiddenTime -= TIM_gf_dt / pst_Data->f_HiddenSpeed;
			}

			if(pst_Data->f_HiddenTime <= 0)
			{
				if(pst_Data->l_Flags & GFX_Flare_Death)
				{
					GFX_Del(GFX_ppst_CurrentList, GFX_gpst_Current->uw_Id);
				}
				return;
			}
		}
		else
		{
			if(pst_Data->l_Flags & GFX_Flare_Hidden)
			{
				pst_Data->l_Flags &= ~GFX_Flare_Hidden;
				pst_Data->f_HiddenTime = 1;
			}
		}
	}
	else
	{
		pst_Data->l_Flags &= ~GFX_Flare_Hidden;
		pst_Data->f_HiddenTime = 1;
	}

//zap:
	f_Size = pst_Data->f_Size * sf_Factor;
	if(pst_Data->l_Flags & GFX_Flare_HideChangeSize) f_Size *= pst_Data->f_HiddenTime;

	MATH_AddVector(&V0, &pst_Data->st_Pos, &pst_Data->st_Offset);

	MATH_SubEqualVector(&V0, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
	t = MATH_f_DotProduct(&V0, MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
	//t = MATH_f_NormVector( &V0 );
	MATH_DivEqualVector(&V0, t);
	MATH_AddEqualVector(&V0, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);


	if ( pst_Data->l_Flags & GFX_Flare_DepthChangeSize)
		f_Size /= t;

	if(pst_Data->l_Flags & GFX_Flare_Complex)
	{
		MATH_AddVector(&V0, &pst_Data->st_Pos, &pst_Data->st_Offset);
//		if ( !(pst_Data->l_Flags & GFX_Flare_DepthChangeSize) )
		{
			MATH_SubVector(&A, &V0, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
			t = MATH_f_DotProduct(&A, MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
			f_Size *= t;
		}
	}

	MATH_ScaleVector(&X, MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix), f_Size);
	MATH_ScaleVector(&Y, MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix), f_Size);
	MATH_SubVector(&X0, &X, &Y);
	MATH_AddVector(&Y0, &X, &Y);

	GFX_NeedGeom(4, 4, 2, 1);

	/* set UV and triangles */
	GFX_gpst_Geo->dst_UV[0].fU = 0;
	GFX_gpst_Geo->dst_UV[0].fV = 0;
	GFX_gpst_Geo->dst_UV[1].fU = 1;
	GFX_gpst_Geo->dst_UV[1].fV = 0;
	GFX_gpst_Geo->dst_UV[2].fU = 1;
	GFX_gpst_Geo->dst_UV[2].fV = 1;
	GFX_gpst_Geo->dst_UV[3].fU = 0;
	GFX_gpst_Geo->dst_UV[3].fV = 1;

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

    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_ZTest | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient);

	if(pst_Data->l_Flags & GFX_Flare_Complex)
	{
		MATH_AddVector(&A, &GDI_gpst_CurDD->st_Camera.st_Matrix.T, MATH_pst_GetZAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix));
		MATH_SubVector(&A, &A, &V0);
		MATH_ScaleVector(&A, &A, 2);

		for(i = 0; i < 9; i++)
		{
			MATH_ScaleVector(&X, &X0, pst_Data->af_Scale[i]);
			MATH_ScaleVector(&Y, &Y0, pst_Data->af_Scale[i]);

			MATH_AddScaleVector(&V, &V0, &A, pst_Data->af_Pos[i]);

			MATH_SubVector(VCast(GFX_gpst_Geo->dst_Point + 0), &V, &Y);
			MATH_AddVector(VCast(GFX_gpst_Geo->dst_Point + 1), &V, &X);
			MATH_AddVector(VCast(GFX_gpst_Geo->dst_Point + 2), &V, &Y);
			MATH_SubVector(VCast(GFX_gpst_Geo->dst_Point + 3), &V, &X);

			if((pst_Data->f_HiddenTime == 1) || (!(pst_Data->l_Flags & GFX_Flare_HideChangeColor)))
				ul_Color = pst_Data->aul_Color[i];
			else
				ul_Color = LIGHT_ul_Interpol2Colors(0, pst_Data->aul_Color[i], pst_Data->f_HiddenTime);

			GFX_gpst_Geo->dul_PointColors[1] = ul_Color;
			GFX_gpst_Geo->dul_PointColors[2] = ul_Color;
			GFX_gpst_Geo->dul_PointColors[3] = ul_Color;
			GFX_gpst_Geo->dul_PointColors[4] = ul_Color;

			GFX_gpst_Geo->dst_Element->l_MaterialId = pst_Data->al_Id[i];
			GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
		}
	}
	else
	{
		MATH_SubVector(VCast(GFX_gpst_Geo->dst_Point + 0), &V0, &Y0);
		MATH_AddVector(VCast(GFX_gpst_Geo->dst_Point + 1), &V0, &X0);
		MATH_AddVector(VCast(GFX_gpst_Geo->dst_Point + 2), &V0, &Y0);
		MATH_SubVector(VCast(GFX_gpst_Geo->dst_Point + 3), &V0, &X0);

		if((pst_Data->f_HiddenTime == 1) || (!(pst_Data->l_Flags & GFX_Flare_HideChangeColor)))
			ul_Color = ((GFX_tdst_Flare *) pst_Data)->ul_Color;
		else
		{
			ul_Color = LIGHT_ul_Interpol2Colors
				(
					((GFX_tdst_Flare *) pst_Data)->ul_DeathColor,
					((GFX_tdst_Flare *) pst_Data)->ul_Color,
					pst_Data->f_HiddenTime
				);
		}

		GFX_gpst_Geo->dul_PointColors[1] = ul_Color;
		GFX_gpst_Geo->dul_PointColors[2] = ul_Color;
		GFX_gpst_Geo->dul_PointColors[3] = ul_Color;
		GFX_gpst_Geo->dul_PointColors[4] = ul_Color;

		GFX_gpst_Geo->dst_Element->l_MaterialId = ((GFX_tdst_Flare *) pst_Data)->l_Id;
        SOFT_ZList_SelectZListe(ZListesBase - 3);
		GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
	}

	GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
}

/*$4
 ***********************************************************************************************************************
    Another Flare Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_AnotherFlare_Create( OBJ_tdst_GameObject *_pst_GO )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_AnotherFlare	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (GFX_tdst_AnotherFlare *) MEM_p_Alloc(sizeof(GFX_tdst_AnotherFlare));
#ifdef USE_DOUBLE_RENDERING	
	L_memset(pst_Data , 0 , sizeof(GFX_tdst_AnotherFlare));
#endif
	MATH_CopyVector( &pst_Data->st_Pos, OBJ_pst_GetAbsolutePosition( _pst_GO ) );
    MATH_NegVector( &pst_Data->st_Sight, MATH_pst_GetYAxis( OBJ_pst_GetAbsoluteMatrix( _pst_GO ) ) );

    pst_Data->f_Size = 1;
    pst_Data->f_Limit = 0.8f;
    pst_Data->f_Offset = 0;
    pst_Data->f_InWallFactor = 1.0f;
    pst_Data->f_OutWallFactor = 1.0f;
    pst_Data->f_DeathTime = 0.0f;
    pst_Data->c_Flags = 0;
    pst_Data->c_Type = 0;
    pst_Data->c_State = 0;
    pst_Data->c_Number = 0;
    pst_Data->ul_Color = 0xFFFFFFFF;
    pst_Data->ul_DeathColor = 0;
	return (void *) pst_Data;
}
#ifdef USE_DOUBLE_RENDERING	
void             GFX_AnotherFlare_Interpolate( void *p_Data , u_int Mode , float fInterpoler)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_AnotherFlare	*pst_AF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_AF = (GFX_tdst_AnotherFlare *)p_Data;
    
    if (!((*(ULONG *)&pst_AF->st_Pos2.z) & 1)) Mode = 101;


	if (Mode < 100)
	// Render I
	{
			if (Mode == 0)
			{
				pst_AF->st_Pos2 = pst_AF->st_Pos1;
				pst_AF->st_Sight2 = pst_AF->st_Sight1;
				pst_AF->st_Pos1 = pst_AF->st_Pos;
				pst_AF->st_Sight1 = pst_AF->st_Sight;
			}
			MATH_BlendVector(&pst_AF->st_Pos , &pst_AF->st_Pos2 , &pst_AF->st_Pos1 , fInterpoler);
			MATH_BlendVector(&pst_AF->st_Sight , &pst_AF->st_Sight2 , &pst_AF->st_Sight1 , fInterpoler);
		} else
	// Render K
	if (Mode == 101)
	{
		pst_AF->st_Pos1 	= pst_AF->st_Pos2 	= pst_AF->st_Pos;
		pst_AF->st_Sight1 	= pst_AF->st_Sight2	= pst_AF->st_Sight;
	} else
	{
		pst_AF->st_Pos 		= pst_AF->st_Pos1;
		pst_AF->st_Sight 	= pst_AF->st_Sight1;
	} 

	(*(ULONG *)&pst_AF->st_Pos2.z) |= 1;
    
}
void   GFX_Sun_Interpolate( void *p_Data , u_int Mode , float fInterpoler)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Sun	*pst_AF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_AF = (GFX_tdst_Sun *)p_Data;

	if (Mode < 100)
	// Render I
	{
			if (Mode == 0)
			{
				pst_AF->st_Pos2 = pst_AF->st_Pos1;
				pst_AF->st_Pos1 = pst_AF->st_Pos;
			}
			MATH_BlendVector(&pst_AF->st_Pos , &pst_AF->st_Pos2 , &pst_AF->st_Pos1 , fInterpoler);
		} else
	// Render K
	if (Mode == 101)
	{
		pst_AF->st_Pos1 	= pst_AF->st_Pos;
	} else
	{
		pst_AF->st_Pos 		= pst_AF->st_Pos1;
	} 
    
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_AnotherFlare_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_AnotherFlare	*pst_Data;
	MATH_tdst_Vector		V0, V, CP, W, X1, X2, X3, Y, Tmp, N;
	float					a, da, t, z, size;
	ULONG					ul_Color, DM;
    int                     i, addforback;


    /*$F
    static test = 0;
    static MATH_tdst_Vector Cam;
    /**/

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_AnotherFlare *) p_Data;

    ul_Color = pst_Data->ul_Color;

	if (ul_Color == 0xFF)
		ul_Color = 0xFF;

	size = pst_Data->f_Size;
    if (pst_Data->c_State & GFX_AnotherFlare_Dying)
    {
        pst_Data->f_TimeLeft -= TIM_gf_dt;
        if (pst_Data->f_TimeLeft <= 0) return 0;
		a = (pst_Data->f_TimeLeft / pst_Data->f_DeathTime);
        ul_Color = COLOR_ul_Blend( pst_Data->ul_DeathColor, pst_Data->ul_Color, a );
		if (pst_Data->c_Flags & GFX_AnotherFlare_DeathSize)
			size *= a;
    }
    else if (pst_Data->c_Flags & GFX_AnotherFlare_Die)
    {
        if (pst_Data->f_DeathTime <= 0) return 0;
        pst_Data->c_State |= GFX_AnotherFlare_Dying;
        pst_Data->f_TimeLeft = pst_Data->f_DeathTime;
    }

    /*$F
    if (GetAsyncKeyState( VK_SHIFT ) < 0)
    {
        if (test == 0)
        {
            LINK_PrintStatusMsg( "flare locked" );
            test = 1;
            MATH_CopyVector( &Cam, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
        }
        else
        {
            LINK_PrintStatusMsg( "flare unlocked" );
            test = 0;
        }
        while ( GetAsyncKeyState( VK_SHIFT ) < 0);
    }
    /**/

	/* test si le flare est dans un secteur invisible */
	if(((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis == 0)
		return 1;


    /*$F
    if (test)
        MATH_SubVector(&V, &pst_Data->st_Pos, &Cam);
    else
    /**/

	MATH_SubVector(&V, &pst_Data->st_Pos, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
    addforback = 0;
    MATH_NormalizeVector( &V0, &V );
	MATH_CopyVector( &CP, &V );

    if (pst_Data->c_Type == GFX_AnotherFlare_Simple)
    {
        MATH_ScaleVector( &W, &V0, -pst_Data->f_Offset );
        MATH_ScaleVector( &Y, MATH_pst_GetYAxis( &GDI_gpst_CurDD->st_Camera.st_Matrix ), size );
        MATH_ScaleVector( &X1, MATH_pst_GetXAxis( &GDI_gpst_CurDD->st_Camera.st_Matrix ), size );
        MATH_NegVector( &X2, &X1);
    }
    else
    {
        MATH_CrossProduct( &V, &pst_Data->st_Sight, &V0 );
        MATH_NormalizeEqualVector( &V );

        MATH_ScaleVector( &Y, &V, size );

        MATH_CrossProduct( &X1, &pst_Data->st_Sight, &Y );
		if ( MATH_f_DotProduct( &V0, &pst_Data->st_Sight) > 0)
		{
			MATH_CrossProduct( &X2, &pst_Data->st_Sight, &Y );
		    MATH_NegVector( &X1, &X2 );
		}
		else
		{
			MATH_CrossProduct( &X1, &pst_Data->st_Sight, &Y );
		    MATH_NegVector( &X2, &X1 );
		}
		MATH_NegVector( &X3, &X1 );

        MATH_ScaleVector( &W, &pst_Data->st_Sight, pst_Data->f_Offset );

        z = t = 1 + MATH_f_DotProduct( &pst_Data->st_Sight, &V0 );
        if ( t > pst_Data->f_Limit)
            t = pst_Data->f_Limit;

        if (pst_Data->c_Flags & GFX_AnotherFlare_Cut)
        {
            MATH_ScaleVector( &W, &pst_Data->st_Sight, t );
            MATH_AddScaleVector( &X1, &X1, &W, -pst_Data->f_InWallFactor );
            MATH_ScaleVector( &W, &pst_Data->st_Sight, pst_Data->f_Offset );
        }
        else
        {
            MATH_ScaleVector( &W, &pst_Data->st_Sight, t * pst_Data->f_Offset );
            MATH_AddScaleVector( &X1, &X1, &W, -pst_Data->f_InWallFactor );
        }
        MATH_AddScaleVector( &X2, &X2, &pst_Data->st_Sight, t * size * pst_Data->f_OutWallFactor);
        if ( MATH_f_DotProduct( &X1, &V0) > 0)
            MATH_NegEqualVector( &Y );

        if ((pst_Data->c_Flags & GFX_AnotherFlare_Back) && (z > 1))
        {
			if ( (pst_Data->c_Type != GFX_AnotherFlare_Simple) && (pst_Data->c_Number >= 1) )
			{
				MATH_CrossProduct( &X3, &V, &pst_Data->st_Sight ); 
				MATH_ScaleEqualVector( &X3, size );
	            
				MATH_SubVector( &Tmp, &X3, &X1 );
				MATH_CrossProduct( &N, &Y, &Tmp );
				z = MATH_f_DotProduct( &N, &V0 );
				if (z < 0)
				{
					addforback = 1;
					MATH_SubVector( &Tmp, &X1, &X2 );
					MATH_CrossProduct( &N, &Y, &Tmp );
					z = MATH_f_DotProduct( &N, &V0 );
					if ( z < 0)
						MATH_InitVectorToZero( &X1 );
					else
						MATH_InitVectorToZero( &X2 );
				}
			}
			else
			{
				MATH_tdst_Vector	CX2;
				float				H, Hh, x; 
				
				MATH_AddVector( &CX2, &CP, &X2 );
				H = MATH_f_DotProduct( &X2, &pst_Data->st_Sight);
				Hh = MATH_f_DotProduct( &CX2, &pst_Data->st_Sight);
				if (Hh != 0)
				{
					x = (H / Hh) * (-MATH_f_DotProduct( &CX2, &X3) / MATH_f_DotProduct( &X3, &X3) );
					if (x < -0.5)
						MATH_CopyVector( &X2, &X3 );
					else if (x < 0)
					{
						x = (x + 0.5f) * 2;
						MATH_ScaleEqualVector( &X2, x );
						MATH_AddScaleVector( &X2, &X2, &X3, (1-x) );
					}
				}
			}
        }
    }

    MATH_AddEqualVector( &W, &pst_Data->st_Pos );

    if ( (pst_Data->c_Type != GFX_AnotherFlare_Simple) && (pst_Data->c_Number >= 1) )
    {
        GFX_NeedGeom(3 + addforback  + pst_Data->c_Number, 4, (2+addforback) * (pst_Data->c_Number) , 1);

		if (pst_Data->c_Number == 1)
			da = a = 0;
		else
		{
			da = (pst_Data->f_Angle * 2) / (pst_Data->c_Number - 1);
			a = -pst_Data->f_Angle;
		}
        MATH_ScaleEqualVector( &V, t * size * pst_Data->f_OutWallFactor );

        for (i = 0; i < pst_Data->c_Number; i++, a += da )
        {
            MATH_AddScaleVector( VCast(GFX_gpst_Geo->dst_Point + (3 + i)), &W, &X2, fOptCos( a ) );
            MATH_AddScaleVector( VCast(GFX_gpst_Geo->dst_Point + (3 + i)), VCast(GFX_gpst_Geo->dst_Point + (3 + i)), &V, fOptSin( a ) );

            GFX_gpst_Geo->dul_PointColors[4 + i] = ul_Color;

            GFX_gpst_Geo->dst_Element->dst_Triangle[ i * 2 ].auw_Index[0] = 0;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[ i * 2 ].auw_Index[1] = 1;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[ i * 2 ].auw_Index[2] = 2;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[ i * 2 ].auw_UV[0] = 0;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[ i * 2 ].auw_UV[1] = 1;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[ i * 2 ].auw_UV[2] = 2;

            GFX_gpst_Geo->dst_Element->dst_Triangle[1 + i * 2].auw_Index[0] = 2;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1 + i * 2].auw_Index[1] = 3 + i;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1 + i * 2].auw_Index[2] = 0;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1 + i * 2].auw_UV[0] = 2;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1 + i * 2].auw_UV[1] = 3;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1 + i * 2].auw_UV[2] = 0;
        }

        if (addforback)
        {
            MATH_AddVector( VCast(GFX_gpst_Geo->dst_Point + 3 + pst_Data->c_Number), &W, &X3 );
            GFX_gpst_Geo->dul_PointColors[4 + pst_Data->c_Number] = ul_Color;
            for (i = 0; i < pst_Data->c_Number; i++)
            {
                GFX_gpst_Geo->dst_Element->dst_Triangle[ pst_Data->c_Number * 2 + i ].auw_Index[0] = 2;
	            GFX_gpst_Geo->dst_Element->dst_Triangle[ pst_Data->c_Number * 2 + i ].auw_Index[1] = 3 + pst_Data->c_Number;
	            GFX_gpst_Geo->dst_Element->dst_Triangle[ pst_Data->c_Number * 2 + i ].auw_Index[2] = 0;
	            GFX_gpst_Geo->dst_Element->dst_Triangle[ pst_Data->c_Number * 2 + i ].auw_UV[0] = 2;
	            GFX_gpst_Geo->dst_Element->dst_Triangle[ pst_Data->c_Number * 2 + i ].auw_UV[1] = 3;
	            GFX_gpst_Geo->dst_Element->dst_Triangle[ pst_Data->c_Number * 2 + i ].auw_UV[2] = 0;
            }
        }
    }
    else
    {
        if ( (pst_Data->c_Flags & GFX_AnotherFlare_Cut) && (t * pst_Data->f_InWallFactor > pst_Data->f_Offset) )
        {
            GFX_NeedGeom( 6, 6, 4, 1 );
            //a = fOptInv( t ) - 0.001f;
            a = pst_Data->f_Offset / (t * pst_Data->f_InWallFactor);

            MATH_ScaleVector( &V0, &Y, (1 - a) );
            MATH_AddVector( VCast(GFX_gpst_Geo->dst_Point + 3), &W, &X2 );
            MATH_AddScaleVector( VCast(GFX_gpst_Geo->dst_Point + 4), &W, &X1, a );
            MATH_SubVector( VCast(GFX_gpst_Geo->dst_Point + 5), VCast(GFX_gpst_Geo->dst_Point + 4), &V0 );
            MATH_AddEqualVector( VCast(GFX_gpst_Geo->dst_Point + 4), &V0 );

            MATH_ScaleVector( &V0, &pst_Data->st_Sight, (t * pst_Data->f_InWallFactor) - pst_Data->f_Offset );
            MATH_AddEqualVector( &X1, &V0 );

            GFX_gpst_Geo->dul_PointColors[4] = ul_Color;
            GFX_gpst_Geo->dul_PointColors[5] = ul_Color;

            GFX_gpst_Geo->dst_UV[4].fU = a;
	        GFX_gpst_Geo->dst_UV[4].fV = 0;
            GFX_gpst_Geo->dst_UV[5].fU = 1;
	        GFX_gpst_Geo->dst_UV[5].fV = 1 - a;

            GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[0] = 0;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[1] = 4;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_Index[2] = 2;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[0] = 0;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[1] = 4;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[0].auw_UV[2] = 2;

            GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[0] = 4;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[1] = 5;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_Index[2] = 2;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[0] = 4;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[1] = 5;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[1].auw_UV[2] = 2;

            GFX_gpst_Geo->dst_Element->dst_Triangle[2].auw_Index[0] = 4;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[2].auw_Index[1] = 1;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[2].auw_Index[2] = 5;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[2].auw_UV[0] = 4;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[2].auw_UV[1] = 1;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[2].auw_UV[2] = 5;

            GFX_gpst_Geo->dst_Element->dst_Triangle[3].auw_Index[0] = 2;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[3].auw_Index[1] = 3;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[3].auw_Index[2] = 0;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[3].auw_UV[0] = 2;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[3].auw_UV[1] = 3;
	        GFX_gpst_Geo->dst_Element->dst_Triangle[3].auw_UV[2] = 0;
        }
        else
        {
            GFX_NeedGeom(4 + addforback, 4, 2 + addforback, 1);
            MATH_AddVector( VCast(GFX_gpst_Geo->dst_Point + 3), &W, &X2 );
            GFX_gpst_Geo->dul_PointColors[4] = ul_Color;
            
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

			if (addforback)
			{
				MATH_AddVector( VCast(GFX_gpst_Geo->dst_Point + 4), &W, &X3 );
				GFX_gpst_Geo->dul_PointColors[5] = 0xFF0000FF;
				GFX_gpst_Geo->dst_Element->dst_Triangle[ 2 ].auw_Index[0] = 2;
				GFX_gpst_Geo->dst_Element->dst_Triangle[ 2 ].auw_Index[1] = 4;
				GFX_gpst_Geo->dst_Element->dst_Triangle[ 2 ].auw_Index[2] = 0;
				GFX_gpst_Geo->dst_Element->dst_Triangle[ 2 ].auw_UV[0] = 2;
				GFX_gpst_Geo->dst_Element->dst_Triangle[ 2 ].auw_UV[1] = 3;
				GFX_gpst_Geo->dst_Element->dst_Triangle[ 2 ].auw_UV[2] = 0;
			}
		}
    }

	/* set UV and triangles */
	GFX_gpst_Geo->dst_UV[0].fU = 0;
	GFX_gpst_Geo->dst_UV[0].fV = 0;
	GFX_gpst_Geo->dst_UV[1].fU = 1;
	GFX_gpst_Geo->dst_UV[1].fV = 0;
	GFX_gpst_Geo->dst_UV[2].fU = 1;
	GFX_gpst_Geo->dst_UV[2].fV = 1;
	GFX_gpst_Geo->dst_UV[3].fU = 0;
	GFX_gpst_Geo->dst_UV[3].fV = 1;

    MATH_AddVector( VCast(GFX_gpst_Geo->dst_Point + 0), &W, &Y );
    MATH_AddVector( VCast(GFX_gpst_Geo->dst_Point + 1), &W, &X1 );
    MATH_SubVector( VCast(GFX_gpst_Geo->dst_Point + 2), &W, &Y );

    GFX_gpst_Geo->dul_PointColors[1] = ul_Color;
	GFX_gpst_Geo->dul_PointColors[2] = ul_Color;
	GFX_gpst_Geo->dul_PointColors[3] = ul_Color;

    M_GFX_CheckGeom();

    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient);
    if (pst_Data->c_Flags & GFX_AnotherFlare_NoFog) GDI_gpst_CurDD->ul_CurrentDrawMask &= ~GDI_Cul_DM_Fogged;
	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
	GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    return 1;
}


/*$4
 ***********************************************************************************************************************
    Sun Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Sun_ComputeRay( GFX_tdst_Sun *pst_Data )
{
	float			angle, dangle, a;
	int				i;

	if (pst_Data->i_Number & 1)
		pst_Data->i_Number &= ~1;
	if (pst_Data->i_Number > 128)
		pst_Data->i_Number = 128;

	dangle = Cf_2Pi / (pst_Data->i_Number >> 1);
	for(i = 0, angle = 0; i < pst_Data->i_Number; i++, angle += dangle)
	{
		a = angle + fRand(-pst_Data->f_BigRayAngleTresh, pst_Data->f_BigRayAngleTresh);
		if (a < 0) a+= Cf_2Pi;
		pst_Data->ast_Ray[i].ratio = a / Cf_PiBy4;
		pst_Data->ast_Ray[i].Pt0 = ((char) pst_Data->ast_Ray[i].ratio) & 0x7;
		pst_Data->ast_Ray[i].Pt1 = (pst_Data->ast_Ray[i].Pt0 + 1) & 0x7;
		pst_Data->ast_Ray[i].ratio -= pst_Data->ast_Ray[i].Pt0;
		pst_Data->ast_Ray[i].st_Axe.x = fOptCos(a);
		pst_Data->ast_Ray[i].st_Axe.y = fOptSin(a);
		pst_Data->ast_Ray[i].l = fRand(pst_Data->f_BigRayLMin, pst_Data->f_BigRayLMax);
		pst_Data->ast_Ray[i].w = fRand(pst_Data->f_BigRayWMin, pst_Data->f_BigRayWMax);

		i++;
		a = angle + fRand(-pst_Data->f_SmallRayAngleTresh, pst_Data->f_SmallRayAngleTresh);
        if (a < 0) a+= Cf_2Pi;
		pst_Data->ast_Ray[i].ratio = a / Cf_PiBy4;
		pst_Data->ast_Ray[i].Pt0 = ((char) pst_Data->ast_Ray[i].ratio) & 0x7;
		pst_Data->ast_Ray[i].Pt1 = (pst_Data->ast_Ray[i].Pt0 + 1) & 0x7;
		pst_Data->ast_Ray[i].ratio -= pst_Data->ast_Ray[i].Pt0;
		pst_Data->ast_Ray[i].st_Axe.x = fOptCos(a);
		pst_Data->ast_Ray[i].st_Axe.y = fOptSin(a);
		pst_Data->ast_Ray[i].l = fRand(pst_Data->f_SmallRayLMin, pst_Data->f_SmallRayLMax);
		pst_Data->ast_Ray[i].w = fRand(pst_Data->f_SmallRayWMin, pst_Data->f_SmallRayWMax);
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Sun_InitRayLum( GFX_tdst_Sun *pst_Data )
{
	int		i;
	float	lum;

	if (pst_Data->l_Flags & GFX_Flare_Hide)
		lum = 0;
	else
		lum = 1;

	for	 (i = 0; i < 8; i++)
		pst_Data->af_Lum[i] = lum;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Sun_UpdateRayLum( GFX_tdst_Sun *pst_Data )
{
#if defined(_XBOX)

	Gx8_TestSunVisibility(GDI_gpst_CurDD, pst_Data);

/*#elif defined(_PC_RETAIL)

	Dx9_TestSunVisibility(GDI_gpst_CurDD, pst_Data);
*/
#else	// defined(_XBOX) // defined(_PC_RETAIL)
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	MATH_tdst_Vector	v, w, vcam;
	float				xmin, xmax, ymin, ymax;
	float				f_Factor[8][2] ={ { 1, 0 }, { -0.3f, 0.7f }, { -0.7f, 0.3f }, { -0.7f, -0.3f }, { -0.3f, -0.7f }, { 0.3f, -0.7f }, { 0.7f, -0.3f }, { 0.7f, 0.3f } };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SOFT_TransformAndProject(&v, &pst_Data->st_Pos, 1, &GDI_gpst_CurDD->st_Camera);

    pst_Data->l_HideFlags <<= 16;

	if((*(LONG *) &v.z) & 0x80000000)
	{
		pst_Data->l_Flags |= GFX_Flare_Hidden;
        pst_Data->l_HideFlags |= 0x1FF;
		return;
	}

	pst_Data->l_Flags &= ~GFX_Flare_Hidden;

	xmin = (float) lMax(GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft, 0);
	xmax = (float) lMin
		(
			GDI_gpst_CurDD->st_Camera.l_ViewportRealLeft + (LONG) GDI_gpst_CurDD->st_Camera.f_Width,
			GDI_gpst_CurDD->st_Device.l_Width
		);
	ymin = (float) lMax(0, GDI_gpst_CurDD->st_Camera.l_ViewportRealTop);
	ymax = (float) lMin
		(
			GDI_gpst_CurDD->st_Device.l_Height,
			GDI_gpst_CurDD->st_Camera.l_ViewportRealTop + (LONG) GDI_gpst_CurDD->st_Camera.f_Height
		);

	MATH_TransformVertex(&w, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix, &pst_Data->st_Pos);
	MATH_TransformVertex(&vcam, GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix, &pst_Data->st_Pos);

	/* le point du center */
	if((v.x < xmin) || (v.x > xmax) || (v.y < ymin) || (v.y > ymax)) 
    {
        pst_Data->l_HideFlags |= 0x100;
    }
	else
	{
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_ReadPixel, (ULONG) & v);
		if(w.z > GFXF_f_depth)
            pst_Data->l_HideFlags |= 0x100;
	}

	/* les 8 points du tour*/
	for(i = 0; i < 8; i++)
	{
		vcam.x += f_Factor[i][0] * pst_Data->f_Interval;
		vcam.y += f_Factor[i][1] * pst_Data->f_Interval;
		SOFT_Project(&v, &vcam, 1, &GDI_gpst_CurDD->st_Camera);

		if((v.x < xmin) || (v.x > xmax) || (v.y < ymin) || (v.y > ymax))
        {
            pst_Data->l_HideFlags |= 1 << i;
        }
		else
		{
			GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_ReadPixel, (ULONG) & v);
			if(w.z > GFXF_f_depth)
                pst_Data->l_HideFlags |= 1 << i;
		}
	}
#endif	// defined(_XBOX) // defined(_PC_RETAIL)

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Sun_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Sun	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Sun *) MEM_p_Alloc(sizeof(GFX_tdst_Sun));
	
	L_memset(pst_Data , 0 , sizeof(GFX_tdst_Sun));
	
	MATH_InitVectorToZero(&pst_Data->st_Pos);
    MATH_InitVectorToZero(&pst_Data->st_OldPos );
	pst_Data->f_BigRayAngleTresh = 0.2f;
	pst_Data->f_BigRayLMax = 15;
	pst_Data->f_BigRayLMin = 10;
	pst_Data->f_BigRayWMax = 2;
	pst_Data->f_BigRayWMin = 1;
	pst_Data->f_SmallRayAngleTresh = 0.2f;
	pst_Data->f_SmallRayLMax = 4;
	pst_Data->f_SmallRayLMin = 2;
	pst_Data->f_SmallRayWMax = 3;
	pst_Data->f_SmallRayWMin = 2;
	pst_Data->f_LengthRatio = 3;
	pst_Data->f_HideTime = 0.1f;
	pst_Data->f_Interval = 1;
	pst_Data->f_Dist = 10;

	pst_Data->i_Number = 32;
	pst_Data->l_Flags = GFX_Flare_DepthChangeSize | GFX_Flare_Hide;
    pst_Data->l_HideFlags = 0;
    pst_Data->c_Die = 0;
    pst_Data->c_FirstHideCheck = 1;
	GFX_Sun_ComputeRay( pst_Data );
	GFX_Sun_InitRayLum( pst_Data );

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Sun_Seti( void *p_Data, int _i_Param, int _i_Value )
{
	if (_i_Param != GFX_Sun_Compute) return;
	GFX_Sun_ComputeRay( (GFX_tdst_Sun *) p_Data );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Sun_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Sun				*pst_Data;
	MATH_tdst_Vector			Pos, V0, V1, V2, V, X, Y;
	int							i, j;
	ULONG						*rli;
	GEO_tdst_IndexedTriangle	*T;
	GEO_Vertex					*Pt;
	ULONG						DM, ul_Color, aul_Color[ 8 ];
    float                       f, af[8];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Sun *) p_Data;

    if (pst_Data->c_Die)
        pst_Data->l_HideFlags = (pst_Data->l_HideFlags << 16) | 0x1FF;
    else if (pst_Data->l_Flags & GFX_Flare_Hide )
		GFX_Sun_UpdateRayLum( pst_Data );
    else
        pst_Data->l_HideFlags = 0;

	if (pst_Data->l_Flags & GFX_Flare_Hidden)
		return;

	if ( !(pst_Data->l_Flags & GFX_Flare_DepthChangeSize) )
	{
		MATH_SubVector(&Pos, &pst_Data->st_Pos, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
		MATH_NormalizeEqualVector( &Pos );
		MATH_ScaleEqualVector( &Pos, pst_Data->f_Dist );
		MATH_AddEqualVector(&Pos, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
	}
	else
	{
		MATH_CopyVector( &Pos, &pst_Data->st_Pos );
	}

	GFX_NeedGeom(pst_Data->i_Number * 3, 3, pst_Data->i_Number, 1);

	/* set UV and triangles */
	GFX_gpst_Geo->dst_UV[0].fU = 0.5f;
	GFX_gpst_Geo->dst_UV[0].fV = 0;
	GFX_gpst_Geo->dst_UV[1].fU = 0;
	GFX_gpst_Geo->dst_UV[1].fV = 1;
	GFX_gpst_Geo->dst_UV[2].fU = 1;
	GFX_gpst_Geo->dst_UV[2].fV = 1;

	T = GFX_gpst_Geo->dst_Element->dst_Triangle;
	Pt = GFX_gpst_Geo->dst_Point;
	rli = &GFX_gpst_Geo->dul_PointColors[1];

    f = pst_Data->l_HideFlags & 0x100 ? 0.5f : 1.0f;

    /* calcule de la luminosité de chaque portion */
    for (i = 0; i < 8; i++)
    {
        af[i] = (pst_Data->l_HideFlags & (1 << i) ) ? 0 : 2.0f;
        j = (i + 7) % 8;
        af[i] += (pst_Data->l_HideFlags & (1 << j) ) ? 0 : 1.0f;
        j = (i + 1) % 8;
        af[i] += (pst_Data->l_HideFlags & (1 << j) ) ? 0 : 1.0f;

        af[i] = f * (af[i] / 4.0f );
    }

    /* estimation du temps pour se cacher / apparaitre */
    f = MATH_f_Distance( &pst_Data->st_OldPos, &pst_Data->st_Pos );
    MATH_CopyVector( &pst_Data->st_OldPos, &pst_Data->st_Pos );

    if ( pst_Data->c_FirstHideCheck )
    {
        pst_Data->c_FirstHideCheck = 0;
        f = 1;
    }
    else if ( (f == 0) || ((pst_Data->l_HideFlags & 0xFF) == 0) || ((pst_Data->l_HideFlags & 0xFF) == 0xFF) ) 
        f = pst_Data->f_HideTime / TIM_gf_dt;
    else
        f = pst_Data->f_Interval / f;

    ul_Color = 0;
    for ( i = 0; i < 8; i++)
    {
        pst_Data->af_Lum[i] = pst_Data->af_Lum[i] + ( (af[i] - pst_Data->af_Lum[i]) / f);

        if (pst_Data->af_Lum[i] > 1)
            pst_Data->af_Lum[i] = 1;
        else if (pst_Data->af_Lum[i] < 0)
            pst_Data->af_Lum[i] = 0;
        
        aul_Color[i] = LIGHT_ul_Interpol2Colors( 0, pst_Data->ul_Color, pst_Data->af_Lum[i] );
        ul_Color |= aul_Color[i];
    }

    if (ul_Color == 0)
        return;

	for(i = 0; i < pst_Data->i_Number; i++)
	{
        ul_Color = LIGHT_ul_Interpol2Colors( aul_Color[ pst_Data->ast_Ray[i].Pt0 ], aul_Color[ pst_Data->ast_Ray[i].Pt1 ], pst_Data->ast_Ray[i].ratio );
        if (ul_Color == 0)
        {
            GFX_gpst_Geo->dst_Element->l_NbTriangles--;
            continue;
        }

		*rli++ = ul_Color; 
		*rli++ = ul_Color;
		*rli++ = ul_Color;

		T->auw_Index[0] = Pt - GFX_gpst_Geo->dst_Point;
		T->auw_Index[1] = T->auw_Index[0] + 1;
		T->auw_Index[2] = T->auw_Index[0] + 2;

		T->auw_UV[0] = 0;
		T->auw_UV[1] = 1;
		T->auw_UV[2] = 2;
		T++;

		MATH_ScaleVector(&X, MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix), pst_Data->ast_Ray[i].st_Axe.x);
		MATH_ScaleVector(&Y, MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix), pst_Data->ast_Ray[i].st_Axe.y);
		MATH_AddVector(&V, &X, &Y);
		MATH_ScaleEqualVector(&V, pst_Data->ast_Ray[i].l);
		MATH_SubVector(&V0, &Pos, &V);
		MATH_AddScaleVector(&V, &Pos, &V, pst_Data->f_LengthRatio);

		MATH_ScaleVector(&X, MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix), pst_Data->ast_Ray[i].st_Axe.y);
		MATH_ScaleVector(&Y, MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix), -pst_Data->ast_Ray[i].st_Axe.x);
		MATH_AddVector(&V2, &X, &Y);
		MATH_ScaleEqualVector(&V2, pst_Data->ast_Ray[i].w);
		MATH_SubVector(&V1, &V, &V2);
		MATH_AddVector(&V2, &V, &V2);

		MATH_CopyVector(VCast(Pt), &V0);
		MATH_CopyVector(VCast(Pt + 1), &V1);
		MATH_CopyVector(VCast(Pt + 2), &V2);
		Pt += 3;
	}

    M_GFX_CheckGeom();

    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
	GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient | GDI_Cul_DM_ZTest);
    SOFT_ZList_SelectZListe(ZListesBase - 4);
    //SOFT_ZList_SelectZListe( 0 );
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
	GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif



