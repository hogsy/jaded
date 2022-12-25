/*$T MDFmodifier_GEO.c GC! 1.081 06/09/00 09:00:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "GRObject/GROrender.h"
#include "GEOmetric/GEOsubobject.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLmain.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "ENGine/Sources/COLlision/COLreport.h"
#include "ENGine/Sources/COLlision/COLzdx.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "SDK/Sources/TIMer/TIM.h"

#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_GPG.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTstruct.h"
#include "SOFT/SOFTzlist.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#include "BASe/BENch/BENch.h"

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)   ReadLong((char*)_a)
#define _ReadFloat(_a)   ReadFloat((char*)_a)
#else
#define _ReadLong(_a)    *(ULONG*)_a
#define _ReadFloat(_a)   *(float*)_a
#endif

#if defined(_XBOX) 
#include <assert.h>
#include "Gx8/Gx8AddInfo.h"
#include "Gx8/Gx8init.h"
#include "Gx8/Gx8BuildUVs.h"
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static OBJ_tdst_GameObject	GPG_go_DumGAO;
static float				GPG_gf_Rand[256];
static int					GPG_gi_FirstInit = 0;
static GPG_tdst_Renderer	GPG_gt_Rend[GPG_MaxLOD];

GPG_tdst_Perturbator		GPG_gpt_Perturbators[GPG_MaxPerturbators];
int							GPG_gi_NumPerturbators = 0;

static GPG_tdst_Perturbator	GPG_gpt_Perturbators_Cur[GPG_MaxPerturbators];
static int					GPG_gi_NumPerturbators_Cur;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_Reset(void)
{
	GPG_gi_NumPerturbators = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_InitModifier(GPG_tdst_Modifier *pst_GPG)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int	i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!GPG_gi_FirstInit)
	{
		for(i = 0; i < 256; i++) GPG_gf_Rand[i] = fRand(0, 1.0f);
		GPG_gi_FirstInit = 0;
	}

	L_memset(pst_GPG, 0, sizeof(GPG_tdst_Modifier));
	for(i = 0; i < GPG_MaxGeom; i++)
	{
		pst_GPG->apt_Geoms[i].f_ZoomMin = pst_GPG->apt_Geoms[i].f_ZoomMax = 1.0f;
		pst_GPG->apt_Geoms[i].f_RandV = pst_GPG->apt_Geoms[i].f_RandP = 1.0f;
		pst_GPG->apt_Geoms[i].f_PertFactor = 1.5f;
		pst_GPG->apt_Geoms[i].t_LOD[0].f_LOD = 10.0f;
		for(j = 1; j < GPG_MaxLOD; j++) pst_GPG->apt_Geoms[i].t_LOD[j].f_LOD = pst_GPG->apt_Geoms[i].t_LOD[j - 1].f_LOD + 10.0f;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GPG_tdst_Modifier *pst_Data;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GPG_tdst_Modifier));
	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;
	pst_Data = (GPG_tdst_Modifier *) _pst_Mod->p_Data;
	L_memset(pst_Data , 0 , sizeof(GPG_tdst_Modifier));
	
	if(!p_Data)
	{
		GPG_InitModifier(pst_Data);
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GPG_tdst_Modifier));
		for(i = 0; i < GPG_MaxGeom; i++)
		{
			for(j = 0; j < GPG_MaxLOD; j++)
			{
				if(pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom)
				{
					((GRO_tdst_Struct *) pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom)->i->pfn_AddRef(pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom, 1);
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GPG_tdst_Modifier *pst_Data;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GPG_tdst_Modifier *) _pst_Mod->p_Data;
	for(i = 0; i < GPG_MaxGeom; i++)
	{
		for(j = 0; j < GPG_MaxLOD; j++)
		{
			if(pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom)
			{
				((GRO_tdst_Struct *) pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom)->i->pfn_AddRef(pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom, -1);
				((GRO_tdst_Struct *) pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom)->i->pfn_Destroy(pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom);
			}
		}
	}

	MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_Modifier_Render(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < GPG_MaxLOD; i++)
	{
		GPG_go_DumGAO.pst_Base->pst_Visu->pst_Object = (GRO_tdst_Struct *) GPG_gt_Rend[i].pt_Geom;
		for(j = 0; j < GPG_gt_Rend[i].i_Num; j++)
		{
			GPG_go_DumGAO.pst_GlobalMatrix = &GPG_gt_Rend[i].t_Inst[j].M;
			GRO_Render(&GPG_go_DumGAO);
		}

		GPG_gt_Rend[i].i_Num = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_ModifyMatrix(MATH_tdst_Matrix *M, GPG_tdst_Geom *pt_G)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	MATH_tdst_Vector	v, v1;
	float				f;
	MATH_tdst_Vector	st_SumAdd, st_Add, st_Sight, st_Banking;
	BOOL				ok;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ok = FALSE;
	MATH_SetNulVector(&st_SumAdd);
	for(i = 0; i < GPG_gi_NumPerturbators_Cur; i++)
	{
		MATH_CopyVector(&v1, &GPG_gpt_Perturbators_Cur[i].pt_GAO->pst_GlobalMatrix->T);
		MATH_SubVector(&v, &M->T, &v1);
		f = MATH_f_SqrNormVector(&v);
		if(f > GPG_gpt_Perturbators_Cur[i].f_Size * GPG_gpt_Perturbators_Cur[i].f_Size) continue;
		ok = TRUE;

		f = fSqrt(f);
		MATH_CopyVector(&st_Add, &v);
		MATH_ScaleEqualVector(&st_Add, 
			((GPG_gpt_Perturbators_Cur[i].f_Size - f) * (GPG_gpt_Perturbators_Cur[i].f_Size - f)) / 
			((GPG_gpt_Perturbators_Cur[i].f_Size) * (GPG_gpt_Perturbators_Cur[i].f_Size))
			); 
		MATH_ScaleEqualVector(&st_Add, pt_G->f_PertFactor);
		MATH_AddEqualVector(&st_SumAdd, &st_Add);
	}

	if(ok)
	{
		st_Banking.x = st_Banking.y = 0;
		st_Banking.z = 1.0f;
		MATH_AddEqualVector(&st_Banking, &st_SumAdd);
		MATH_NormalizeEqualVector(&st_Banking);
		MATH_NegVector(&st_Sight, (MATH_tdst_Vector *) &M->Jx);
		MATH_OrientMatrix_UsingBanking(M, &st_Sight, &st_Banking);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_Modifier_Apply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GPG_tdst_Modifier	*pst_Data;
	OBJ_tdst_GameObject	*pst_GO;
    GRO_tdst_Visu       *pst_Visu;
	GRO_tdst_Visu		t_MemVisu;
	ULONG				i;
	LONG				j, lod;
	MATH_tdst_Matrix	*M;
	static int			rand = 0;
	MATH_tdst_Vector	v, t_Tmp;
	float				angle, sqrnorm, zoom;
	MATH_tdst_Vector	*pt_Vertex;
	GPG_tdst_Geom		*pt_Geom;
	GPG_tdst_RendererInst *pt_Inst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = GDI_gpst_CurDD->pst_CurrentGameObject;
	pst_Data = (GPG_tdst_Modifier *) _pst_Mod->p_Data;

	/* Save */
	L_memcpy(&GPG_go_DumGAO, pst_GO, sizeof(OBJ_tdst_GameObject));
    pst_Visu = pst_GO->pst_Base->pst_Visu;
	L_memcpy(&t_MemVisu, pst_Visu, sizeof(GRO_tdst_Visu));
	M = pst_GO->pst_GlobalMatrix;

	pst_Visu->dul_VertexColors = NULL;

	/* Compute perturbators */
	GPG_gi_NumPerturbators_Cur = 0;
	for(i = 0; i < (ULONG) GPG_gi_NumPerturbators; i++)
	{
		if(COL_BV_PivotCollide(pst_GO, GPG_gpt_Perturbators[i].pt_GAO, 0))
		{
			L_memcpy(&GPG_gpt_Perturbators_Cur[GPG_gi_NumPerturbators_Cur++], &GPG_gpt_Perturbators[i], sizeof(GPG_gpt_Perturbators[i]));
		}
	}

	rand = 0;
	for(i = 0; i < GPG_MaxGeom; i++)
	{
		pt_Geom = &pst_Data->apt_Geoms[i];
		for(lod = 0; lod < GPG_MaxLOD; lod++) 
		{
			GPG_gt_Rend[lod].i_Num = 0;
			GPG_gt_Rend[lod].pt_Geom = pt_Geom->t_LOD[lod].pt_Geom;
		}

		for(j = 0, pt_Vertex = _pst_Obj->dst_Point; j < _pst_Obj->l_NbPoints; j++, pt_Vertex++)
		{
			MATH_CopyVector(&v, pt_Vertex);
			MATH_AddEqualVector(&v, &M->T);
			if(GPG_gf_Rand[rand] <= pt_Geom->f_RandV)
			{
				for(lod = 0; lod < GPG_MaxLOD; lod++)
				{
					MATH_SubVector(&t_Tmp, &v, &GDI_gpst_CurDD->st_Camera.st_Matrix.T);
					sqrnorm = MATH_f_SqrNormVector(&t_Tmp); // Div 0 ?
					if(sqrnorm < pt_Geom->t_LOD[lod].f_LOD * pt_Geom->t_LOD[lod].f_LOD)
					{
						if(!pt_Geom->t_LOD[lod].pt_Geom) break;
						if(GPG_gt_Rend[lod].i_Num == GPG_MaxRenderInst) GPG_Modifier_Render();

						pt_Inst = &GPG_gt_Rend[lod].t_Inst[GPG_gt_Rend[lod].i_Num++];

						/* Position */
						MATH_SetIdentityMatrix(&pt_Inst->M);
						MATH_CopyVector(&pt_Inst->M.T, &v);

						/* Rotation */
						angle = GPG_gf_Rand[(rand + 1) & 0xFF] * 6.28f;
						pt_Inst->M.Ix = fOptCos(angle);
						pt_Inst->M.Iy = -fOptSin(angle);
						pt_Inst->M.Jx = fOptSin(angle);
						pt_Inst->M.Jy = fOptCos(angle);

						/* Zoom */
						zoom = pt_Geom->f_ZoomMin + ((pt_Geom->f_ZoomMax - pt_Geom->f_ZoomMin) * GPG_gf_Rand[(rand + 2) & 0xFF]);
						MATH_SetZoom(&pt_Inst->M, zoom);

						/* Modifiers de matrice */
						if(!(pt_Geom->ul_Flags & MOD_GPG_DontPerturb)) GPG_ModifyMatrix(&pt_Inst->M, pt_Geom);
						break;
					}
				}
			}

			rand = (rand + 1) & 0xFF;
		}

		/* Residu */
		GPG_Modifier_Render();
	}

	/* Restore */
	L_memcpy(pst_Visu, &t_MemVisu, sizeof(GRO_tdst_Visu));
	pst_GO->pst_GlobalMatrix = M;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_Modifier_Unapply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_Modifier_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GPG_tdst_Modifier	*pst_Data;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GPG_Reset();
	pst_Data = (GPG_tdst_Modifier *) _pst_Mod->p_Data;
	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	if(!(pst_Data->ul_Flags & MOD_GPG_Validate))
	{
		pst_Data->ul_Flags |= MOD_GPG_Validate;
		for(i = 0; i < GPG_MaxGeom; i++)
		{
			for(j = 0; j < GPG_MaxLOD; j++)
			{
				if(pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom)
					((GRO_tdst_Struct *) pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom)->i->pfn_AddRef(pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom, 1);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GPG_ul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GPG_tdst_Modifier				*pst_Data;
	ULONG							ul_Version;
	ULONG							ul_Key;
	unsigned char					*pc_Cur;
	int								i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Cur = (unsigned char*)_pc_Buffer;
	pst_Data = (GPG_tdst_Modifier *) _pst_Mod->p_Data;

   /* Read version */
	ul_Version = LOA_ReadLong((CHAR**)&pc_Cur);
	GPG_InitModifier(pst_Data);

	/* Flags */
	pst_Data->ul_Flags = LOA_ReadLong((CHAR**)&pc_Cur);
#if !defined(XML_CONV_TOOL)
	pst_Data->ul_Flags &= ~MOD_GPG_Validate;
#endif

	/* Geoms */
	for(i = 0; i < GPG_MaxGeom; i++)
	{
		pst_Data->apt_Geoms[i].f_ZoomMin = LOA_ReadFloat((CHAR**)&pc_Cur);
		pst_Data->apt_Geoms[i].f_ZoomMax = LOA_ReadFloat((CHAR**)&pc_Cur);
		pst_Data->apt_Geoms[i].ul_Flags = LOA_ReadLong((CHAR**)&pc_Cur);
		pst_Data->apt_Geoms[i].f_RandV = LOA_ReadFloat((CHAR**)&pc_Cur);
		pst_Data->apt_Geoms[i].f_RandP = LOA_ReadFloat((CHAR**)&pc_Cur);
		pst_Data->apt_Geoms[i].f_PertFactor = LOA_ReadFloat((CHAR**)&pc_Cur);
		for(j = 0; j < GPG_MaxLOD; j++)
		{
			ul_Key = LOA_ReadLong((CHAR**)&pc_Cur);
			if(ul_Key != BIG_C_InvalidKey)
			{
				LOA_MakeFileRef
				(
					ul_Key,
					(ULONG *) &pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom,
					GEO_ul_Load_ObjectCallback,
					LOA_C_MustExists
				);

				pst_Data->apt_Geoms[i].t_LOD[j].f_LOD = LOA_ReadFloat((CHAR**)&pc_Cur);
			}
		}
	}

	return (ULONG)(pc_Cur - (unsigned char*)_pc_Buffer) ;

}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GPG_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GPG_tdst_Modifier	*pst_Data;
	ULONG				ulSize;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GPG_tdst_Modifier *) _pst_Mod->p_Data;

   /* Save version */
	ulSize = 0;
	SAV_Buffer(&ulSize, 4);

	/* Flags */
	SAV_Buffer(&pst_Data->ul_Flags, 4);

	/* Geoms */
	for(i = 0; i < GPG_MaxGeom; i++)
	{
		SAV_Buffer(&pst_Data->apt_Geoms[i].f_ZoomMin, 4);
		SAV_Buffer(&pst_Data->apt_Geoms[i].f_ZoomMax, 4);
		SAV_Buffer(&pst_Data->apt_Geoms[i].ul_Flags, 4);
		SAV_Buffer(&pst_Data->apt_Geoms[i].f_RandV, 4);
		SAV_Buffer(&pst_Data->apt_Geoms[i].f_RandP, 4);
		SAV_Buffer(&pst_Data->apt_Geoms[i].f_PertFactor, 4);
		for(j = 0; j < GPG_MaxLOD; j++)
		{
			if(pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom)
			{
				ulSize = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->apt_Geoms[i].t_LOD[j].pt_Geom);
				SAV_Buffer(&ulSize, 4);
				SAV_Buffer(&pst_Data->apt_Geoms[i].t_LOD[j].f_LOD, 4);
			}
			else
			{
				ulSize = BIG_C_InvalidKey;
				SAV_Buffer(&ulSize, 4);
			}
		}
	}
}

#endif


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
