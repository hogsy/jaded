/*$T GRI_display.c GC! 1.081 04/05/00 16:38:48 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/GRId/GRI_display.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_vars.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "GRObject/GROrender.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#include "BIGfiles/BIGkey.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "EDIpaths.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKstruct.h"
#include "GRI_compute.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

ULONG	GRI_ul_CapaColor[128];
int		GRI_ul_CapaColorInit = 0;
BOOL	GRI_gb_UpdateZ = FALSE;
BOOL	GRI_gb_Edit;
float	GRI_gf_Z = 0.0f;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GRI_CapaColor(char capa)
{
	if(!GRI_ul_CapaColorInit)
	{
		GRI_ul_CapaColorInit = 1;

		GRI_ul_CapaColor[1] = 0xFF00FFFF;
		GRI_ul_CapaColor[2] = 0xFF00FF00;
		GRI_ul_CapaColor[3] = 0xFFBF0000;
		GRI_ul_CapaColor[4] = 0xFF90FFFF;
		GRI_ul_CapaColor[5] = 0xFFFFFF00;
		GRI_ul_CapaColor[6] = 0xFFFF00FF;
		GRI_ul_CapaColor[7] = 0xFFFFFFFF;
		GRI_ul_CapaColor[8] = 0xFF0000BF;
		GRI_ul_CapaColor[9] = 0xFFFF0000;
		GRI_ul_CapaColor[10] = 0xFFBF0050;
		GRI_ul_CapaColor[11] = 0xFF00BFBF;
		GRI_ul_CapaColor[12] = 0xFFBFBF00;
		GRI_ul_CapaColor[13] = 0xFFBF00BF;
		GRI_ul_CapaColor[14] = 0xFFBFBFBF;
		GRI_ul_CapaColor[15] = 0xFF000000;

		GRI_ul_CapaColor[15+1] = 0xFF00007F;
		GRI_ul_CapaColor[15+2] = 0xFF007F00;
		GRI_ul_CapaColor[15+3] = 0xFF7F0000;
		GRI_ul_CapaColor[15+5] = 0xFF7F7F00;
		GRI_ul_CapaColor[15+6] = 0xFF7F007F;
		GRI_ul_CapaColor[15+7] = 0xFF007F7F;
		GRI_ul_CapaColor[15+9] = 0xFF00004F;
		GRI_ul_CapaColor[15+10] = 0xFF004F00;
		GRI_ul_CapaColor[15+11] = 0xFF4F0000;
		GRI_ul_CapaColor[15+13] = 0xFF4F4F00;
		GRI_ul_CapaColor[15+14] = 0xFF004F4F;
		GRI_ul_CapaColor[15+15] = 0xFF4F004F;

		GRI_ul_CapaColor[15+4] = 0xFF0000FF;
		GRI_ul_CapaColor[15+8] = 0xFF0000B5;
		GRI_ul_CapaColor[15+12] = 0xFF000060;
	}

	if(capa & MASK_DYN)
		return GRI_ul_CapaColor[15 + (((UCHAR) capa) >> 4)];
	else
		return GRI_ul_CapaColor[capa & 0xF];
}

extern int	gi_MinXSel;
extern int	gi_MinYSel;
extern int	gi_MaxXSel;
extern int	gi_MaxYSel;

extern int	gi_MinXSave;
extern int	gi_MinYSave;
extern int	gi_MaxXSave;
extern int	gi_MaxYSave;

extern int	gi_XCursor;
extern int	gi_YCursor;

extern int	gi_XSave;
extern int	gi_YSave;

extern UCHAR *guc_CapaSel;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_Display(WOR_tdst_World *_pst_World, GRID_tdst_World *pst_Grid, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Pos, v[4], *Quad[4], pos;
	LONG				l, x, y, xStart, yStart, xNumber, yNumber, xMin, xMax, yMin, yMax;
	LONG				x1, y1;
	ULONG				ul_Fat, ul_SaveColor, ul_SaveDF, ul_Color[2];
	GRID_tdst_Elem		*pst_Case;
	unsigned char		capa;
	OBJ_tdst_GameObject *pst_Obj;
	BOOL				b_Ghost;
	int					a, b, starta, startb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_Grid) return;
	if((_pst_World->pst_Grid == pst_Grid) && !(_pst_DD->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show)) return;
	if((_pst_World->pst_Grid1 == pst_Grid) && !(_pst_DD->pst_GridOptions->ul_Flags & GRID_Cul_DO_Show1)) return;

	if
	(
		((gi_XCursor != -1) || (gi_YCursor != -1))
	&&	GRID_2DtoGridCoord
		(
			_pst_DD->pst_World,
			pst_Grid,
			_pst_DD,
			gi_XCursor,
			gi_YCursor,
			&a,
			&b,
			1
		)
	)
	{
		starta = (gi_XSave == gi_MinXSave) ? a : a - (gi_MaxXSave - gi_MinXSave);
		startb = (gi_YSave == gi_MinYSave) ? b : b - (gi_MaxYSave - gi_MinYSave);
	
		b_Ghost = TRUE;
	}
	else
		b_Ghost = FALSE;

	if(_pst_World->pst_Grid->p_GameObject == NULL)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		char	asz_Path[BIG_C_MaxLenPath];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_SaveColor = LOA_ul_SearchIndexWithAddress((ULONG) _pst_World);
		if(ul_SaveColor != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_SaveColor), asz_Path);
			strcat(asz_Path, "/");
			strcat(asz_Path, EDI_Csz_Path_Grid);
			_pst_World->pst_Grid->p_GameObject = (void *) OBJ_GameObject_Create(0);
			((OBJ_tdst_GameObject *) _pst_World->pst_Grid->p_GameObject)->pst_World = _pst_World;
			((OBJ_tdst_GameObject *) _pst_World->pst_Grid->p_GameObject)->sz_Name = (char*)MEM_p_Alloc(50);
			L_strcpy(((OBJ_tdst_GameObject *) _pst_World->pst_Grid->p_GameObject)->sz_Name, "Grid.gao");
			pos.x = _pst_World->pst_Grid->f_MinXTotal;
			pos.y = _pst_World->pst_Grid->f_MinYTotal;
			pos.z = _pst_DD->pst_GridOptions->f_Z;
			OBJ_SetAbsolutePosition((OBJ_tdst_GameObject*)_pst_World->pst_Grid->p_GameObject, &pos);
			LINK_RegisterPointer(_pst_World->pst_Grid->p_GameObject, LINK_C_ENG_GameObjectOriented, "Grid.gao", asz_Path);
		}
	}

	/* for Z grid to z of another game object */
	if(_pst_DD->pst_GridOptions->ul_Flags & GRID_Cul_DO_LockZOnObject)
	{
		if(_pst_DD->pst_GridOptions->ul_KeyForLockedZ != 0xFFFFFFFF)
		{
			ul_Fat = BIG_ul_SearchKeyToFat(_pst_DD->pst_GridOptions->ul_KeyForLockedZ);
			if(ul_Fat != BIG_C_InvalidIndex)
			{
				pst_Obj = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_Fat));
				if(pst_Obj && ((int) pst_Obj != -1))
				{
					OBJ_pst_GetAbsolutePosition((OBJ_tdst_GameObject*)_pst_World->pst_Grid->p_GameObject)->z = OBJ_pst_GetAbsolutePosition(pst_Obj)->z;
					OBJ_pst_GetAbsolutePosition((OBJ_tdst_GameObject*)_pst_World->pst_Grid->p_GameObject)->z += 0.4f;
				}
			}
		}
	}

	/* Retreive pos of grid by pos of gameobject */
	pst_Pos = OBJ_pst_GetAbsolutePosition((OBJ_tdst_GameObject*)_pst_World->pst_Grid->p_GameObject);
	GRI_gf_Z = pst_Pos->z;

	if(pst_Grid == _pst_World->pst_Grid1) pst_Pos->z += 1.0f;
	pst_Grid->f_MinXTotal = pst_Pos->x;
	pst_Grid->f_MinYTotal = pst_Pos->y;
	if(GRI_gb_UpdateZ)
		pst_Pos->z = _pst_DD->pst_GridOptions->f_Z;
	else
		_pst_DD->pst_GridOptions->f_Z = pst_Pos->z;
	GRI_gb_UpdateZ = FALSE;

	ul_SaveDF = _pst_DD->ul_DisplayFlags;
	_pst_DD->ul_DisplayFlags |= GDI_Cul_DF_DisplayInvisible;
	GRO_RenderGro((OBJ_tdst_GameObject *) _pst_World->pst_Grid->p_GameObject, &GRO_gst_Unknown);
	_pst_DD->ul_DisplayFlags = ul_SaveDF;

	SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject*)_pst_World->pst_Grid->p_GameObject));
	if(pst_Grid == _pst_World->pst_Grid1) pst_Pos->z -= 1.0f;
	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	_pst_DD->st_GDI.pfnv_SetViewMatrix(_pst_DD->st_Camera.pst_ObjectToCameraMatrix);

	v[3].z = v[2].z = v[1].z = v[0].z = 0.0f;
	Quad[0] = &v[0];
	Quad[1] = &v[1];
	Quad[2] = &v[2];
	Quad[3] = &v[3];
	ul_SaveColor = _pst_DD->ul_ColorConstant;

	/* Content of grid */
	if(_pst_DD->pst_GridOptions->ul_Flags & GRID_Cul_DO_ShowContent)
	{
		pst_Case = pst_Grid->pst_EditArray;
		xStart = 0;
		yStart = 0;
		xNumber = xStart + ((LONG) pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup);
		yNumber = yStart + ((LONG) pst_Grid->w_NumGroupsY * pst_Grid->c_SizeGroup);

		xMin = pst_Grid->w_XRealGroup * pst_Grid->c_SizeGroup;
		xMax = xMin + ((LONG) pst_Grid->w_NumRealGroupsX * pst_Grid->c_SizeGroup);
		yMin = pst_Grid->w_YRealGroup * pst_Grid->c_SizeGroup;
		yMax = yMin + ((LONG) pst_Grid->w_NumRealGroupsY * pst_Grid->c_SizeGroup);
		l = 0;
		for(y = yStart; y < yNumber; y++)
		{
			for(x = xStart; x < xNumber; x++, pst_Case++)
			{
				if((y >= yMin) && (y < yMax) && (x >= xMin) && (x < xMax))
				{
					x1 = x;
					y1 = y;
					GRI_ConvertTo2DReal(pst_Grid, (int*)&x1, (int*)&y1);
					capa = pst_Grid->pst_RealArray[x1 + (y1 * pst_Grid->w_NumRealGroupsX * pst_Grid->c_SizeGroup)].c_Capacities;
				}
				else
				{
					if(_pst_DD->pst_GridOptions->ul_Flags & GRID_Cul_DO_ShowOnlyReal) continue;
					capa = pst_Case->c_Capacities;
				}

				if((x >= gi_MinXSel) && (x <= gi_MaxXSel) && (y >= gi_MinYSel) && (y <= gi_MaxYSel))
				{
					if((x == gi_MinXSel) || (x == gi_MaxXSel) || (y == gi_MinYSel) || (y == gi_MaxYSel))
					{
						if(!capa)
							_pst_DD->ul_ColorConstant = 0xFFFFFFFF;
						else
							_pst_DD->ul_ColorConstant = GRI_CapaColor(capa) ^ 0xFFFFFF;
					}
					else
					{
						if(!capa)
							_pst_DD->ul_ColorConstant = 0xFFFFFFFF;
						else
							_pst_DD->ul_ColorConstant = GRI_CapaColor(capa) ; //^ 0xFFFFFF;
					}
				}
				else
				{
					_pst_DD->ul_ColorConstant = GRI_CapaColor(capa);
					if(capa == 0) 
					{
						if(b_Ghost && ((x >= starta) && (x <= starta + (gi_MaxXSave - gi_MinXSave + 1))) && ((y >= startb) && (y <= startb + (gi_MaxYSave - gi_MinYSave + 1))))
						{
							_pst_DD->ul_ColorConstant = 0xFFFFFFFF;
						}
						else
							continue;
					}
				}

						if(b_Ghost && ((x >= starta) && (x <= starta + (gi_MaxXSave - gi_MinXSave + 1))) && ((y >= startb) && (y <= startb + (gi_MaxYSave - gi_MinYSave + 1))))
				{
					_pst_DD->ul_ColorConstant = 0xFFFFFFFF;
				}

				v[0].x = x * GRID_CASE;
				v[0].y = y * GRID_CASE;
				v[1].x = (x + 1) * GRID_CASE;
				v[1].y = v[0].y;
				v[2].x = v[1].x;
				v[2].y = (y + 1) * GRID_CASE;
				v[3].x = v[0].x;
				v[3].y = v[2].y;
				_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawQuad, (ULONG) Quad);
			}
		}
	}

	/* First all grid */
	if(!(_pst_DD->pst_GridOptions->ul_Flags & GRID_Cul_DO_ShowOnlyReal))
	{
		xStart = 0;
		yStart = 0;
		xNumber = ((LONG) pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup);
		yNumber = ((LONG) pst_Grid->w_NumGroupsY * pst_Grid->c_SizeGroup);
		if(_pst_DD->pst_GridOptions->ul_Flags & GRID_Cul_DO_ShowEdit)
		{
			ul_Color[0] = 0xFF900000;
			ul_Color[1] = 0xFFFFFFFF;
		}
		else
		{
			ul_Color[0] = 0xFF7F7F7F;
			ul_Color[1] = 0xFFFFFFFF;
		}

		if(GRI_gb_Edit)
		{
			ul_Color[0] = 0xFFA00000;
			ul_Color[1] = 0xFFFF0000;
		}

		v[0].x = xStart * GRID_CASE;
		v[0].y = yStart * GRID_CASE;
		v[1].x = v[0].x;
		v[1].y = v[0].y + yNumber * GRID_CASE;
		l = xNumber + 1;
		while(l--)
		{
			_pst_DD->ul_ColorConstant = ul_Color[(l % pst_Grid->c_SizeGroup) ? 0 : 1];
			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
			v[0].x += GRID_CASE;
			v[1].x += GRID_CASE;
		}

		v[0].x = xStart * GRID_CASE;
		v[0].y = yStart * GRID_CASE;
		v[1].x = v[0].x + xNumber * GRID_CASE;
		v[1].y = v[0].y;
		l = yNumber + 1;
		while(l--)
		{
			_pst_DD->ul_ColorConstant = ul_Color[(l % pst_Grid->c_SizeGroup) ? 0 : 1];
			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
			v[0].y += GRID_CASE;
			v[1].y += GRID_CASE;
		}
	}

	/* Then real grid */
	xStart = pst_Grid->w_XRealGroup * pst_Grid->c_SizeGroup;
	yStart = pst_Grid->w_YRealGroup * pst_Grid->c_SizeGroup;
	xNumber = ((LONG) pst_Grid->w_NumRealGroupsX * pst_Grid->c_SizeGroup);
	yNumber = ((LONG) pst_Grid->w_NumRealGroupsY * pst_Grid->c_SizeGroup);
	ul_Color[0] = 0xFF4040FF;
	ul_Color[1] = 0xFF0000FF;
	if(GRI_gb_Edit)
	{
		ul_Color[0] = 0xFFA00000;
		ul_Color[1] = 0xFFFF0000;
	}

	v[0].x = xStart * GRID_CASE;
	v[0].y = yStart * GRID_CASE;
	v[1].x = v[0].x;
	v[1].y = v[0].y + yNumber * GRID_CASE;
	l = xNumber + 1;
	while(l--)
	{
		_pst_DD->ul_ColorConstant = ul_Color[(l % pst_Grid->c_SizeGroup) ? 0 : 1];
		_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
		v[0].x += GRID_CASE;
		v[1].x += GRID_CASE;
	}

	v[0].x = xStart * GRID_CASE;
	v[0].y = yStart * GRID_CASE;
	v[1].x = v[0].x + xNumber * GRID_CASE;
	v[1].y = v[0].y;
	l = yNumber + 1;
	while(l--)
	{
		_pst_DD->ul_ColorConstant = ul_Color[(l % pst_Grid->c_SizeGroup) ? 0 : 1];
		_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
		v[0].y += GRID_CASE;
		v[1].y += GRID_CASE;
	}

	SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);
	_pst_DD->ul_ColorConstant = ul_SaveColor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GRID_2DtoGridCoord
(
	WOR_tdst_World			*_pst_World,
	GRID_tdst_World			*pst_Grid,
	GDI_tdst_DisplayData	*_pst_DD,
	int						x,
	int						y,
	int						*a,
	int						*b,
	char					_b_Vis
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v, st_3D, st_CamPosIso, *pst_CamPos, *pst_GridPos;
	float				t, z;
	LONG				xMin, xMax, yMin, yMax;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_Grid) return FALSE;

	xMin = -1;
	if(_b_Vis)
	{
		if(_pst_DD->pst_GridOptions->ul_Flags & GRID_Cul_DO_ShowOnlyReal)
		{
			xMin = (LONG) pst_Grid->w_XRealGroup * pst_Grid->c_SizeGroup;
			xMax = xMin + (LONG) pst_Grid->w_NumRealGroupsX * pst_Grid->c_SizeGroup;
			yMin = (LONG) pst_Grid->w_YRealGroup * pst_Grid->c_SizeGroup;
			yMax = yMin + (LONG) pst_Grid->w_NumRealGroupsY * pst_Grid->c_SizeGroup;
		}
	}

	if(xMin == -1)
	{
		xMin = yMin = 0;
		xMax = (LONG) pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup;
		yMax = (LONG) pst_Grid->w_NumGroupsY * pst_Grid->c_SizeGroup;
	}

	if(_pst_World->pst_Grid->p_GameObject == NULL) return FALSE;

	if (_pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
	{
		MATH_InitVector(&v, (float) x, (float) y, 1.0f);
		CAM_2Dto3D(&_pst_DD->st_Camera, &st_3D, &v);
		
		pst_CamPos = &_pst_DD->st_Camera.st_Matrix.T;
	}
	else
	{
		MATH_InitVector(&v, (float) x, (float) y, -10000.0f);
		CAM_2Dto3D(&_pst_DD->st_Camera, &st_CamPosIso, &v);
		MATH_InitVector(&v, (float) x, (float) y, 10000.0f);
		CAM_2Dto3D(&_pst_DD->st_Camera, &st_3D, &v);
		
		pst_CamPos = &st_CamPosIso;
	}
	if(pst_CamPos->z == st_3D.z) return FALSE;
	
	pst_GridPos = OBJ_pst_GetAbsolutePosition((OBJ_tdst_GameObject*)_pst_World->pst_Grid->p_GameObject);
	z = pst_GridPos->z;
	if(pst_Grid == _pst_World->pst_Grid1) z += 1.0f;
	t = (z - pst_CamPos->z) / (st_3D.z - pst_CamPos->z);
	if(t < 0) return FALSE;
	
	v.x = pst_CamPos->x + (st_3D.x - pst_CamPos->x) * t;
	v.y = pst_CamPos->y + (st_3D.y - pst_CamPos->y) * t;

	v.x -= pst_GridPos->x;
	v.y -= pst_GridPos->y;

	if((v.x < 0) || (v.y < 0)) return FALSE;

	*a = (int) (v.x / GRID_CASE);
	if((*a < xMin) || (*a >= xMax)) return FALSE;

	*b = (int) (v.y / GRID_CASE);
	if((*b < yMin) || (*b >= yMax)) return FALSE;
	return TRUE;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

#endif /* ACTIVE_EDITORS */
