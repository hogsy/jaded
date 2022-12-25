 
/*$T GSPinit.c GC! 1.081 05/04/00 15:08:05 */

/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
/*#pragma global_optimizer on
#pragma optimization_level 4*/


#include <eeregs.h>
#include <eestruct.h>
#include <libgraph.h>
#include <libdma.h>
#include <libvu0.h>
#include <sifdev.h>
#include <libpc.h>


#include "Gsp.h"
#include "GSPinit.h"
#include "GSPtex.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "MATerial/MATstruct.h"

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_FCLONE.h"

#include "Gsp_Bench.h"

#ifdef PSX2_TARGET
#include "ENGvars.h"

#define GSP_DRAWP_ASSEMBLY
extern ULONG VU0_JumpTatble[5];
extern ULONG VU0_JumpTatble_STRIPS[5];
extern ULONG VU0_JumpTatble_SYM_STRIPS[5];
extern ULONG ShowNormals;
/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined PSX2_TARGET && defined __cplusplus
extern "C"
{
#endif
extern void GSP_SetViewMatrix_PlusLight( MATH_tdst_Matrix *_pst_Matrix);
extern int NbrObjectClone;
extern BOOL b_CloneCulled[MAXCLONE];
extern MyMatrixFormat *Gsp_SetDrawBuffer_LIGHT(Gsp_BigStruct	  *p_BIG, Gsp_tdst_DrawBuffer *p_MDB );

void GSP_SetLIGHT(OBJ_tdst_GameObject					*_pst_GO)
{
	MyMatrixFormat 						*p_MyMatrix;
	MyVectorFormat 						*Writer;
	
	/* Save GIF Tag in point 0*/
	p_MyMatrix = Gsp_SetDrawBuffer_LIGHT( &gs_st_Globals, GspGlobal_ACCESS(MDB) );
	Writer = (MyVectorFormat *)p_MyMatrix;
	*(u32 *)&Writer -> x = 2;
	*(u32 *)&Writer -> y = 0xc1de;
	*(u32 *)&Writer -> z = 0xc2de;
	*(u32 *)&Writer -> w = 0xc3de;
	Writer++;
	//AMBIENT_COL
	Writer -> x = (float)((GDI_gpst_CurDD_SPR.pst_World->ul_AmbientColor2 & 0xff));
	Writer -> y = (float)((GDI_gpst_CurDD_SPR.pst_World->ul_AmbientColor2 & 0xff00)>>8);
	Writer -> z = (float)((GDI_gpst_CurDD_SPR.pst_World->ul_AmbientColor2 & 0xff0000)>>16);
	Writer -> w = 0;
	Writer++;
	//DIRECT_COL
	Writer -> x = 0;
	Writer -> y = 0;
	Writer -> z = 0;
	Writer -> w = 0;
	Writer++;
	//DIRECT_VEC
	Writer -> x = 0.0f;
	Writer -> y = 0.0f;
	Writer -> z = 0.0f;
	Writer -> w = 0.0f;
	Writer++;
	//POINT_COL
	Writer -> x = 0;
	Writer -> y = 0;
	Writer -> z = 0;
	Writer -> w = 0;
	Writer++;
	//POINT_VEC
	Writer -> x = 0;
	Writer -> y = 0;
	Writer -> z = 0;
	Writer -> w = 0;
	Writer++;
	if (_pst_GO->CloneLightList)
	{
		LightCloneListeGao		*CloneLightList;
		u32 NbLiGhT;
		CloneLightList = _pst_GO->CloneLightList;
		Writer = (MyVectorFormat *)p_MyMatrix;
		Writer += 2;

		while (CloneLightList)
		{
			LIGHT_tdst_Light *Light;
			Light = (LIGHT_tdst_Light*)CloneLightList->p_Gao->pst_Extended->pst_Light;
			if ((Light -> ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct)
			{
				/* Light Color */
				Writer[0].x = (Light->ul_Color & 0xff)>>0;
				Writer[0].y = (Light->ul_Color & 0xff00)>>8;
				Writer[0].z = (Light->ul_Color & 0xff0000)>>16;
				Writer[0].w = 0;
				/* Light Vector */
				Writer[1].x = 0;
				Writer[1].y = 0;
				Writer[1].z = 1;
				Writer[1].w = 0;
			}
			else
			if ((Light -> ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni)
			{
				/* Light Color */
				Writer[2].x = (Light->ul_Color & 0xff)>>0;
				Writer[2].y = (Light->ul_Color & 0xff00)>>8;
				Writer[2].z = (Light->ul_Color & 0xff0000)>>16;
				Writer[2].w = 0;
				/* Light Vector */
				Writer[3].x = 0;
				Writer[3].y = 0;
				Writer[3].z = 1;
				Writer[3].w = 0;
			}

			CloneLightList = CloneLightList->p_Next;
		}
	}
	

	
	
/*
DTCTOR	NumP
AMBIENT_COL	AR		AG		AB 		AA
DIRECT_COL 	DR		DG		DB		DA
DIRECT_VEC	DX		DY		DZ		__
POINT_COL	S1R		S1G		S1B		OoNear
POINT_VEC	S1X		S1Y		S1Z		OoFar
*/


}


LONG GSP_DrawIndexedTriangles_Clone
(
	GEO_tdst_Object						*pst_Obj,
	GEO_tdst_ElementIndexedTriangles	*_pst_Element,
	GEO_Vertex				*_pst_Point,
	GEO_tdst_UV				*_pst_UV,
	ULONG 					ulNbPoints,
	MAT_tdst_MTLevel		*pst_MLTTXLVL
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject					*_pst_GO,*_pst_GOList;
	MATH_tdst_Matrix	st_Current __attribute__((aligned(16)));
	MATH_tdst_Matrix	*pUp;
	u32 *p_Colors;
	ULONG AddFlags;
	GEO_tdst_Object	*pst_OriginalGeo,*pst_Geo;
	extern void MAT_UV_Compute_PLANAR_GIZMO_NOSTORE(GDI_tdst_DisplayData	*pst_CurDD,GEO_tdst_Object	*pst_Obj,GEO_tdst_ElementIndexedTriangles	*pst_Element,ULONG CurrentAddFlag , ULONG StoreUV);
	
	AddFlags = 0; 
	if((MAT_GET_UVSource(pst_MLTTXLVL->ul_Flags) == MAT_Cc_UV_Planar_GZMO)/* && (MAT_GET_MatrixFrom(pst_MLTTXLVL->s_AditionalFlags) == MAT_CC_WORLD)*/)
			AddFlags = pst_MLTTXLVL->s_AditionalFlags;

	_pst_GO = GDI_gpst_CurDD_SPR.pst_CurrentGameObject;
	
	if (GspGlobal_ACCESS(Status) & GSP_Status_VU1DontDraw) return 0;
	
	MATH_SetIdentityMatrix(&st_Current);
	pUp = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix-1;

	// ********** CULLING ************

	_pst_GOList = _pst_GO;
	pst_OriginalGeo = _pst_GOList->pst_Base->pst_Visu->pst_Object;
	while (_pst_GOList)
	{
		if ((_pst_GOList->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled) == 0)
		{
			GRO_tdst_Visu					*pst_Visu;
			pst_Visu = _pst_GOList->pst_Base->pst_Visu;
			/* Special Case for LOD */
			if (pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
			{
				extern GRO_tdst_Struct *GEO_pst_StaticLOD_GetLOD(GEO_tdst_StaticLOD *_pst_LOD, UCHAR _uc_Distance);
				GEO_tdst_StaticLOD *pst_LOD;
				/* 1 Find actual LOD */
			    pst_LOD = (GEO_tdst_StaticLOD *) pst_Visu->pst_Object;
				pst_Geo = (GEO_tdst_Object	*)GEO_pst_StaticLOD_GetLOD(pst_LOD, _pst_GOList->uc_LOD_Vis);
				if ((u32 *)pst_OriginalGeo != (u32 *)pst_Geo) 
				{
					_pst_GOList=_pst_GOList->p_CloneNextGao;
					if (_pst_GOList == _pst_GO) _pst_GOList=NULL;
					continue;
				}
			}
			_pst_GOList->ulMoreCloneInfo |= 1; // <- Has Been Drawn
		
			MATH_MulMatrixMatrix(&st_Current, _pst_GOList->pst_GlobalMatrix, pUp);
			GSP_SetViewMatrix_PlusLight(&st_Current);
			GSP_SetLIGHT(_pst_GOList);
			GSP_SetCullingMask(pst_Visu->c_CullingMask);
			if(AddFlags)
			{
				GDI_gpst_CurDD->pst_CurrentGameObject = _pst_GOList;
				MAT_UV_Compute_PLANAR_GIZMO_NOSTORE(GDI_gpst_CurDD,NULL,_pst_Element,AddFlags , 0);				
				GDI_gpst_CurDD->pst_CurrentGameObject = _pst_GO;
			}
			if (pst_Obj->p_CompressedNormals)
			GDI_gpst_CurDD_SPR.pst_ComputingBuffers->CurrentColorField = pst_Obj->p_CompressedNormals;
			GSP_l_DrawElementIndexedTriangles(_pst_Element,_pst_Point,_pst_UV,ulNbPoints);
		}

		_pst_GOList = _pst_GOList->p_CloneNextGao;
		if (_pst_GOList == _pst_GO) _pst_GOList = NULL;
	}

	MATH_MulMatrixMatrix(&st_Current, _pst_GO->pst_GlobalMatrix, pUp);
	GSP_SetViewMatrix(&st_Current);

	return 0;
	
}


#if defined PSX2_TARGET && defined __cplusplus
}
#endif

