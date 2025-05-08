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
#include "ENGine/Sources/WORld/WORupdate.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_GEO.h"
#include "MoDiFier/MDFmodifier_GPG.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "SOFT/SOFTstruct.h"
#include "ENGine/Sources/INTersection/INTmain.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#include "BASe/BENch/BENch.h"

#ifdef JADEFUSION
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEO_SKIN.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#ifdef JADEFUSION
//#if defined(PSX2_TARGET) && defined(__cplusplus)
//extern "C"
//{
//#endif

#define Morphing_C_VersionNumber 3
GEO_tdst_ModifierMorphing_ComputingBuffers g_oMorphComputingBuffers;

GEO_Vertex *    
GEO_tdst_ModifierMorphing_ComputingBuffers::GetNormals(  )
{
    return dst_Normals;
}

GEO_Vertex *  
GEO_tdst_ModifierMorphing_ComputingBuffers::GetTangents( )
{
    return dst_Tangents;
}
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GEO_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct *pst_Gro;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	L_strcpy(_psz_Msg, "GameObject must have a geometry to use that modifier");
	if(!_pst_GO) return FALSE;
    if(!_pst_GO->pst_Base) return FALSE;
	if( !(_pst_GO->ul_IdentityFlags && OBJ_C_IdentityFlag_Visu) ) return FALSE;
	if(!_pst_GO->pst_Base->pst_Visu) return FALSE;
	pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
	if(!pst_Gro || (pst_Gro->i->ul_Type != GRO_Geometric))
	{
		return FALSE;
	}

	return TRUE;
}

#endif

#ifdef JADEFUSION
void GEO_l_ModifierMorphing_ComputeTangentSpaceBasisData( MDF_tdst_Modifier *  _pst_Mod,
                                                          GEO_tdst_ModifierMorphing_Data * pst_MorphData, 
                                                          GEO_tdst_Object * pst_ObjectRef, 
                                                          BOOL b_NormalOnly = FALSE );

#endif
/*$4
 ***********************************************************************************************************************
    Snap
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierSnap	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierSnap));
	if(!p_Data)
	{
		L_memset(_pst_Mod->p_Data, 0, sizeof(GEO_tdst_ModifierSnap));
		return;
	}

	L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierSnap));
	pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;

	if(pst_Data->ul_NbPoints)
	{
		pst_Data->pst_Point = (GEO_tdst_ModifierSnap_OnePoint *) MEM_p_Alloc(sizeof(GEO_tdst_ModifierSnap_OnePoint) * pst_Data->ul_NbPoints);
	}
	else
		pst_Data->pst_Point = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierSnap	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;
	if(pst_Data->ul_NbPoints) MEM_Free(pst_Data->pst_Point);
	MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierSnap			*pst_Data;
	MATH_tdst_Matrix				st_Inv ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Matrix				*pst_Mat;
	GEO_tdst_ModifierSnap_OnePoint	*pst_Point, *pst_LastPoint;
	MATH_tdst_Vector				v;
    GEO_Vertex                      *pst_Vertex;
	GEO_tdst_Object					*pst_ObjDest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;

	pst_Mat = OBJ_pst_GetAbsoluteMatrix(GDI_gpst_CurDD->pst_CurrentGameObject);
	MATH_InvertMatrix(&st_Inv, pst_Mat);
	pst_Mat = OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject *) pst_Data->p_GameObject);
	pst_ObjDest = (GEO_tdst_Object *) OBJ_p_GetGro((OBJ_tdst_GameObject *) pst_Data->p_GameObject);

	pst_Point = pst_Data->pst_Point;
	pst_LastPoint = pst_Point + pst_Data->ul_NbPoints;

	if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer)
		pst_Vertex = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
	else
		pst_Vertex = _pst_Obj->dst_Point;

	for(; pst_Point < pst_LastPoint; pst_Point++)
	{
		MATH_CopyVector(&pst_Point->st_Point, VCast( _pst_Obj->dst_Point + pst_Point->ul_IndexSrc ));
		MATH_TransformVertex(&v, pst_Mat, VCast(pst_ObjDest->dst_Point + pst_Point->ul_IndexTgt) );
		MATH_TransformVertex(VCast( pst_Vertex + pst_Point->ul_IndexSrc ), &st_Inv, &v);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierSnap			*pst_Data;
	GEO_tdst_ModifierSnap_OnePoint	*pst_Point, *pst_LastPoint;
	GEO_Vertex                      *pst_Vertex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;
	pst_Point = pst_Data->pst_Point;
	pst_LastPoint = pst_Point + pst_Data->ul_NbPoints;
	pst_Vertex = _pst_Obj->dst_Point;

	for(; pst_Point < pst_LastPoint; pst_Point++)
		MATH_CopyVector(VCast( pst_Vertex + pst_Point->ul_IndexSrc ), &pst_Point->st_Point);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_ModifierSnap_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char							*pc_Cur;
	ULONG							ul_Key;
	GEO_tdst_ModifierSnap			*pst_Data;
	GEO_tdst_ModifierSnap_OnePoint	*pst_Point, *pst_LastPoint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	LOA_ReadLong_Ed(&pc_Cur, NULL); // skip size

	/* Game object */
	ul_Key = LOA_ReadULong(&pc_Cur);
#if defined(XML_CONV_TOOL)
	pst_Data->p_GameObject = (void* )ul_Key;
#else
	if(ul_Key != BIG_C_InvalidIndex)
	{
		if((int) BIG_ul_SearchKeyToPos(ul_Key) != -1)
			LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Data->p_GameObject, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
		else
			pst_Data->p_GameObject = NULL;
	}
#endif // XML_CONV_TOOL

	/* Number of points */
	pst_Data->ul_NbPoints = LOA_ReadULong(&pc_Cur);

	/* Alloc buffer for points */
	if(pst_Data->ul_NbPoints)
	{
		pst_Data->pst_Point = (GEO_tdst_ModifierSnap_OnePoint *) MEM_p_Alloc(sizeof(GEO_tdst_ModifierSnap_OnePoint) * pst_Data->ul_NbPoints);
	}
	else
		pst_Data->pst_Point = NULL;

	/* Points data */
	pst_Point = pst_Data->pst_Point;
	pst_LastPoint = pst_Point + pst_Data->ul_NbPoints;

	for(; pst_Point < pst_LastPoint; pst_Point++)
	{
		pst_Point->ul_IndexSrc = LOA_ReadULong(&pc_Cur);
		pst_Point->ul_IndexTgt = LOA_ReadULong(&pc_Cur);
	}

	return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierSnap			*pst_Data;
	GEO_tdst_ModifierSnap_OnePoint	*pst_Point, *pst_LastPoint;
	ULONG							ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierSnap *) _pst_Mod->p_Data;

	/* Save size */
	ul_Size = 8 + 8 * pst_Data->ul_NbPoints;
	SAV_Buffer(&ul_Size, 4);

	/* Save game object */
#if defined(XML_CONV_TOOL)
	ul_Size = (ULONG) pst_Data->p_GameObject;
#else
	ul_Size = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GameObject);
#endif
	SAV_Buffer(&ul_Size, 4);

	/* Save number of points */
	SAV_Buffer(&pst_Data->ul_NbPoints, 4);

	/* Save points */
	pst_Point = pst_Data->pst_Point;
	pst_LastPoint = pst_Point + pst_Data->ul_NbPoints;
	for(; pst_Point < pst_LastPoint; pst_Point++)
	{
		SAV_Buffer(&pst_Point->ul_IndexSrc, 4);
		SAV_Buffer(&pst_Point->ul_IndexTgt, 4);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSnap_ComputeWithTresh
(
	OBJ_tdst_GameObject *_pst_ObjSrc,
	OBJ_tdst_GameObject *_pst_ObjTgt,
	float				_f_Tresh,
	MDF_tdst_Modifier	*_pst_Mod
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex              *pst_PtSrc, *pst_PtTgt;
	MATH_tdst_Vector		st_MinSrc, st_MaxSrc, st_MinTgt, st_MaxTgt;
	MATH_tdst_Vector		v;
    GEO_Vertex              *A, *B, *C;
	GEO_tdst_Object			*pst_GeoSrc, *pst_GeoTgt;
	MATH_tdst_Matrix		*M;
	int						i, j, i_TgtInSrc, i_SrcInTgt;
	ULONG					*pul_SrcIndex, *pul_TgtIndex, *pul_SrcTgt, *pi, *pj;
	float					f_Dist, f_DistMax;
	GEO_tdst_ModifierSnap	st_SnapData, *pst_SnapData;
	MDF_tdst_Modifier		*pst_NewMod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Test data */
	if(!(_pst_ObjSrc->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return;
	pst_GeoSrc = (GEO_tdst_Object *) _pst_ObjSrc->pst_Base->pst_Visu->pst_Object;
	if(pst_GeoSrc == NULL) return;
	if(pst_GeoSrc->l_NbPoints == 0) return;
	if(!(_pst_ObjTgt->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return;
	pst_GeoTgt = (GEO_tdst_Object *) _pst_ObjTgt->pst_Base->pst_Visu->pst_Object;
	if(pst_GeoTgt == NULL) return;
	if(pst_GeoTgt->l_NbPoints == 0) return;

	/* Alloc temp data */
	pst_PtSrc = (GEO_Vertex *) L_malloc(sizeof(GEO_Vertex) * pst_GeoSrc->l_NbPoints);
	pst_PtTgt = (GEO_Vertex *) L_malloc(sizeof(GEO_Vertex) * pst_GeoTgt->l_NbPoints);

	/* Compute Src BV */
	A = pst_PtSrc;
	B = pst_PtSrc + pst_GeoSrc->l_NbPoints;
	C = pst_GeoSrc->dst_Point;
	M = OBJ_pst_GetAbsoluteMatrix(_pst_ObjSrc);
	MATH_InitVector(&st_MinSrc, Cf_Infinit, Cf_Infinit, Cf_Infinit);
	MATH_NegVector(&st_MaxSrc, &st_MinSrc);
	for(; A < B; A++, C++)
	{
		MATH_TransformVertex(VCast(A), M, VCast( C ));

		if(A->x < st_MinSrc.x) st_MinSrc.x = A->x;
		if(A->x > st_MaxSrc.x) st_MaxSrc.x = A->x;
		if(A->y < st_MinSrc.y) st_MinSrc.y = A->y;
		if(A->y > st_MaxSrc.y) st_MaxSrc.y = A->y;
		if(A->z < st_MinSrc.z) st_MinSrc.z = A->z;
		if(A->z > st_MaxSrc.z) st_MaxSrc.z = A->z;
	}

	/* Include treshold in BV */
	MATH_InitVector(&v, _f_Tresh, _f_Tresh, _f_Tresh);
	MATH_AddEqualVector(&st_MaxSrc, &v);
	MATH_SubEqualVector(&st_MinSrc, &v);

	/* Compute Tgt BV */
	A = pst_PtTgt;
	B = pst_PtTgt + pst_GeoTgt->l_NbPoints;
	C = pst_GeoTgt->dst_Point;
	M = OBJ_pst_GetAbsoluteMatrix(_pst_ObjTgt);
	MATH_InitVector(&st_MinTgt, Cf_Infinit, Cf_Infinit, Cf_Infinit);
	MATH_NegVector(&st_MaxTgt, &st_MinTgt);
	i_TgtInSrc = 0;
	for(; A < B; A++, C++)
	{
		MATH_TransformVertex(VCast(A), M, VCast(C) );

		if(A->x < st_MinTgt.x) st_MinTgt.x = A->x;
		if(A->x > st_MaxTgt.x) st_MaxTgt.x = A->x;
		if(A->y < st_MinTgt.y) st_MinTgt.y = A->y;
		if(A->y > st_MaxTgt.y) st_MaxTgt.y = A->y;
		if(A->z < st_MinTgt.z) st_MinTgt.z = A->z;
		if(A->z > st_MaxTgt.z) st_MaxTgt.z = A->z;

		if((A->x < st_MinSrc.x) || (A->x > st_MaxSrc.x))
			*(LONG *) A = -1;
		else if((A->y < st_MinSrc.y) || (A->y > st_MaxSrc.y))
			*(LONG *) A = -1;
		else if((A->z < st_MinSrc.z) || (A->z > st_MaxSrc.z))
			*(LONG *) A = -1;
		else
			i_TgtInSrc++;
	}

	/* Include treshold in BV */
	MATH_AddEqualVector(&st_MaxTgt, &v);
	MATH_SubEqualVector(&st_MinTgt, &v);

	/* Find all point of src in tgt BV */
	A = pst_PtSrc;
	B = pst_PtSrc + pst_GeoSrc->l_NbPoints;
	i_SrcInTgt = 0;
	for(; A < B; A++)
	{
		if((A->x < st_MinTgt.x) || (A->x > st_MaxTgt.x))
			*(LONG *) A = -1;
		else if((A->y < st_MinTgt.y) || (A->y > st_MaxTgt.y))
			*(LONG *) A = -1;
		else if((A->z < st_MinTgt.z) || (A->z > st_MaxTgt.z))
			*(LONG *) A = -1;
		else
			i_SrcInTgt++;
	}

	pul_SrcIndex = (ULONG *) L_malloc(4 * i_SrcInTgt);
	pul_SrcTgt = (ULONG *) L_malloc(4 * i_SrcInTgt);
	pul_TgtIndex = (ULONG *) L_malloc(4 * i_TgtInSrc);

	pi = pul_TgtIndex;
	for(i = 0; i < pst_GeoTgt->l_NbPoints; i++)
	{
		if(*(LONG *) (pst_PtTgt + i) != -1) *pi++ = i;
	}

	_f_Tresh *= _f_Tresh;
	pi = pul_SrcIndex;
	pj = pul_SrcTgt;
	for(i = 0; i < pst_GeoSrc->l_NbPoints; i++)
	{
		A = pst_PtSrc + i;
		if(*(LONG *) A == -1) continue;

		f_DistMax = _f_Tresh;
		*pj = 0xFFFFFFFF;
		for(j = 0; j < i_TgtInSrc; j++)
		{
			B = pst_PtTgt + pul_TgtIndex[j];
			MATH_SubVector(&v, VCast(A), VCast(B));
			f_Dist = MATH_f_SqrVector(&v);
			if(f_Dist <= f_DistMax)
			{
				*pj = pul_TgtIndex[j];
				f_DistMax = f_Dist;
			}
		}

		if(*pj != 0xFFFFFFFF)
		{
			*pi++ = i;
			pj++;
		}
		else
			i_SrcInTgt--;
	}

	if(i_SrcInTgt)
	{
		st_SnapData.p_GameObject = (void *) _pst_ObjTgt;
		st_SnapData.ul_NbPoints = i_SrcInTgt;
		pst_NewMod = MDF_pst_Modifier_Create(_pst_ObjSrc, MDF_C_Modifier_Snap, &st_SnapData);
		pst_SnapData = (GEO_tdst_ModifierSnap*)pst_NewMod->p_Data;
		for(i = 0; i < i_SrcInTgt; i++)
		{
			pst_SnapData->pst_Point[i].ul_IndexSrc = pul_SrcIndex[i];
			pst_SnapData->pst_Point[i].ul_IndexTgt = pul_SrcTgt[i];
		}

		if(_pst_Mod)
		{
			MDF_Modifier_ReplaceInGameObject(_pst_ObjSrc, _pst_Mod, pst_NewMod);
			MDF_Modifier_Destroy(_pst_Mod);
		}
		else
		{
			MDF_Modifier_AddToGameObject(_pst_ObjSrc, pst_NewMod);
		}
	}
}

#endif ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    Ondule Ton Corps
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierOnduleTonCorps_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierOnduleTonCorps *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierOnduleTonCorps));
	pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;
	pst_Data->ul_Flags = 0;

	if(!p_Data)
	{
		pst_Data->f_Amplitude = 1.0f;
		pst_Data->f_Angle = 0.0f;
		pst_Data->f_Delta = 0.1f;
		pst_Data->f_Factor = 1.0f;
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierOnduleTonCorps));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierOnduleTonCorps_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierOnduleTonCorps *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;
	MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierOnduleTonCorps_Apply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierOnduleTonCorps *pst_Data;
    GEO_Vertex                      *pst_Point, *pst_Last;
	ULONG							*pul_RLI;
	float							f;
	GEO_tdst_StaticLOD                  *pst_LOD;
	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;

    if ( _pst_Obj->st_Id.i->ul_Type == GRO_GeoStaticLOD )
    {
        pst_LOD = ((GEO_tdst_StaticLOD *) _pst_Obj);
        if ( ( pst_LOD->auc_EndDistance[0] >= pst_LOD->uc_Distance ) && (pst_LOD->uc_NbLOD > 1) )
            return;
        _pst_Obj = (GEO_tdst_Object *) pst_LOD->dpst_Id[0];
    }

#ifdef _XENON_RENDER
    if (GDI_b_IsXenonGraphics())
    {
        // Store flags and values
        // Calculations will be done in vertex shader
        if(_pst_Obj->m_pWYB1 == NULL)
        {
            _pst_Obj->m_pWYB1 = pst_Data;
        }
        else
        {
            ERR_X_Assert((_pst_Obj->m_pWYB2 == NULL) && "More than 2 WYB on object!");

            _pst_Obj->m_pWYB2 = pst_Data;
        }
    }
    else
#endif
	{
	/* Save points if needed */
	if(!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer))
	{
		GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseSpecialVertexBuffer;
		GDI_gpst_CurDD->p_Current_Vertex_List = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
		L_memcpy( GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB, _pst_Obj->dst_Point, _pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
	}

	pst_Point = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
	pst_Last = pst_Point + _pst_Obj->l_NbPoints;
	pul_RLI = GDI_gpst_CurDD->pst_CurrentGameObject->pst_Base->pst_Visu->dul_VertexColors;

	if((pst_Data->ul_Flags & GEO_C_OTCF_UseAlphaOfRLI) && pul_RLI)
	{
        pul_RLI++;
		if(pst_Data->ul_Flags & GEO_C_OTCF_InvertAlpha)
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_X)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->x += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->x += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->y += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptCos((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
			else if(pst_Data->ul_Flags & GEO_C_OTCF_Y)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->y += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->y += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->x += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptCos((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
			else if(pst_Data->ul_Flags & GEO_C_OTCF_Z)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->z += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->x += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->y += f * fOptCos((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
		}
		else
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_X)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->x += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->x += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->y += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptCos((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
			else if(pst_Data->ul_Flags & GEO_C_OTCF_Y)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->y += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->y += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->x += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptCos((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
			else if(pst_Data->ul_Flags & GEO_C_OTCF_Z)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->z += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->x += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->y += f * fOptCos((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
		}
	}
	else
	{
		if(pst_Data->ul_Flags & GEO_C_OTCF_X)
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
			{
				for(; pst_Point < pst_Last; pst_Point++ )
				{
					pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
			else
			{
				for(; pst_Point < pst_Last; pst_Point++ )
				{
					pst_Point->y += pst_Data->f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->z += pst_Data->f_Amplitude * fOptCos((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
		}
		else if(pst_Data->ul_Flags & GEO_C_OTCF_Y)
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
			{
				for(; pst_Point < pst_Last; pst_Point++ )
				{
					pst_Point->y += pst_Data->f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->y += pst_Data->f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
			else
			{
				for(; pst_Point < pst_Last; pst_Point++ )
				{
					pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->z += pst_Data->f_Amplitude * fOptCos((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
		}
		else if(pst_Data->ul_Flags & GEO_C_OTCF_Z)
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
			{
                f = pst_Data->f_Amplitude * 2 * fOptSin( pst_Data->f_Angle );
                for(; pst_Point < pst_Last; pst_Point++ )
				    pst_Point->z += f * ( fOptSin(pst_Point->x * pst_Data->f_Factor) + fOptSin( pst_Point->y * pst_Data->f_Factor) );

			}
			else
			{
				for(; pst_Point < pst_Last; pst_Point++ )
				{
					pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->y += pst_Data->f_Amplitude * fOptCos((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
		}
	}

#ifdef ACTIVE_EDITORS
	if(_pst_Mod->ul_Flags & GEO_C_ModifierFlags_ApplyInGeom)
        L_memcpy( _pst_Obj->dst_Point, GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB, _pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
#endif
	}
#ifdef ACTIVE_EDITORS
    // Two renders -> move twice slower.
	if (GDI_gpst_CurDD->ul_WiredMode & 1)
    	pst_Data->f_Angle += (5.0f*pst_Data->f_Delta*TIM_gf_dt);
    else
#endif
    	pst_Data->f_Angle += (10.0f*pst_Data->f_Delta*TIM_gf_dt);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierOnduleTonCorps_Unapply
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
ULONG GEO_ul_ModifierOnduleTonCorps_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char							*pc_Cur;
	GEO_tdst_ModifierOnduleTonCorps *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	LOA_ReadULong_Ed(&pc_Cur, NULL); // skip size

	pst_Data->ul_Flags = LOA_ReadULong(&pc_Cur);
	pst_Data->f_Angle = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_Amplitude = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_Factor = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_Delta = LOA_ReadFloat(&pc_Cur);
#if !defined(XML_CONV_TOOL)
	LOA_ReadLong_Ed(&pc_Cur, NULL); // Skip dummy
#endif
    pst_Data->f_Angle = 0.0f;

    return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierOnduleTonCorps_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierOnduleTonCorps *pst_Data;
	ULONG							ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) _pst_Mod->p_Data;

	ul_Size = 24;
	SAV_Buffer(&ul_Size, 4);

	SAV_Buffer(&pst_Data->ul_Flags, 4);
	SAV_Buffer(&pst_Data->f_Angle, 4);
	SAV_Buffer(&pst_Data->f_Amplitude, 4);
	SAV_Buffer(&pst_Data->f_Factor, 4);
	SAV_Buffer(&pst_Data->f_Delta, 4);

	/* dummy value */
	ul_Size = 0;
	SAV_Buffer(&ul_Size, 4);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_ModifierOnduleTonCorps_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src )
{
	L_memcpy( _pst_Dst->p_Data, _pst_Src->p_Data, sizeof( GEO_tdst_ModifierOnduleTonCorps ) );
	return sizeof( GEO_tdst_ModifierOnduleTonCorps );
}

#endif

/*$4
 ***********************************************************************************************************************
    Perturb
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierPerturb_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierPerturb *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierPerturb));
	pst_Data = (GEO_tdst_ModifierPerturb *) _pst_Mod->p_Data;
	pst_Data->ul_Flags = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierPerturb_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	MEM_Free( _pst_Mod->p_Data );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierPerturb_Apply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
#if defined(_XENON) && defined(_DEBUG)
    ERR_X_Assert("Perturb modifier not supported on Xenon!" && 0);
#endif
	int						i, i_FirstComputing;
	OBJ_tdst_GameObject		*me, *perturb;
	float					radius, r2, f;
	MATH_tdst_Matrix		InvertMat;
	MATH_tdst_Vector		*perturbpos, localperturbpos;
	GEO_Vertex              *pst_Point, *pst_Last, temp;
	
	if ( !GPG_gi_NumPerturbators )
		return;
	
	me = _pst_Mod->pst_GO;
	i_FirstComputing = 1;
	for (i = 0; i < GPG_gi_NumPerturbators; i++)
	{
		perturb = GPG_gpt_Perturbators[i].pt_GAO;
		if (!perturb) 
			continue;
		radius = GPG_gpt_Perturbators[i].f_Size;
		r2 = radius * radius;
		perturbpos = OBJ_pst_GetAbsolutePosition( perturb );
		
		MATH_SubVector( &temp, perturbpos, OBJ_pst_GetAbsolutePosition( me ) );
		if ( !INT_SphereAABBox( &temp, radius, OBJ_pst_BV_GetGMin( me->pst_BV ), OBJ_pst_BV_GetGMax( me->pst_BV ) ) )
       		continue;
       		
       	/* move perturb into object system axis */
       	if( i_FirstComputing )
		{
			i_FirstComputing = 0;
			MATH_InvertMatrix(&InvertMat, me->pst_GlobalMatrix);
			
			/* Save points if needed */
			if(!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer))
			{
				GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseSpecialVertexBuffer;
				GDI_gpst_CurDD->p_Current_Vertex_List = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
				L_memcpy( GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB, _pst_Obj->dst_Point, _pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
			}
		}
		
		MATH_TransformVertex( &localperturbpos, &InvertMat, OBJ_pst_GetAbsolutePosition( perturb ));
		
		pst_Point = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
		pst_Last = pst_Point + _pst_Obj->l_NbPoints;
		for( ; pst_Point < pst_Last; pst_Point++ )
		{
			MATH_SubVector( &temp, pst_Point, &localperturbpos );
			f = MATH_f_SqrVector( &temp );
			if ( f < r2 )
			{
				if (f == 0)
					pst_Point->z += radius;
				else
				{
					f = radius / fSqrt( f );
					MATH_AddScaleVector( pst_Point, &localperturbpos, &temp, f );
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierPerturb_Unapply
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
ULONG GEO_ul_ModifierPerturb_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*pc_Cur;
	GEO_tdst_ModifierPerturb	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierPerturb *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	LOA_ReadULong_Ed(&pc_Cur, NULL); // skip size

	pst_Data->ul_Flags = LOA_ReadULong(&pc_Cur);
    return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierPerturb_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierPerturb *pst_Data;
	ULONG							ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierPerturb *) _pst_Mod->p_Data;

	ul_Size = 4;
	SAV_Buffer(&ul_Size, 4);

	SAV_Buffer(&pst_Data->ul_Flags, 4);
}

#endif

/*$4
 ***********************************************************************************************************************
    Symetrie
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSymetrie_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierSymetrie *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierSymetrie));
	pst_Data = (GEO_tdst_ModifierSymetrie *) _pst_Mod->p_Data;

	if(!p_Data)
	{
        pst_Data->ul_Flags = 1;
        pst_Data->f_Offset = 0;
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierSymetrie));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSymetrie_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	MEM_Free( _pst_Mod->p_Data );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSymetrie_Apply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierSymetrie   *pst_Data;
    GEO_Vertex                  *pst_Dest, *pst_Point, *pst_Last;
    MATH_tdst_Vector            *pst_Normals;
    float                       f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierSymetrie *) _pst_Mod->p_Data;

    // SC: Done in a vertex shader, but allowing points to be moved anyway
    //     to allow SPG2 to be generated at the right location
#if defined(_XENON_RENDER)
    if (GDI_b_IsXenonGraphics())
    {
        if (OBJ_b_TestIdentityFlag(_pst_Mod->pst_GO, OBJ_C_IdentityFlag_Visu))
        {
            _pst_Mod->pst_GO->pst_Base->pst_Visu->ucFlag |= GRO_VISU_FLAG_SYMMETRY;
        }
    }
#endif

    pst_Dest = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;

	GEO_UseNormals(_pst_Obj);
    pst_Normals = _pst_Obj->dst_PointNormal;
	if(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer)
		pst_Point = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
	else
    {
		pst_Point = _pst_Obj->dst_Point;
        L_memcpy( pst_Dest, pst_Point, _pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
    }
    pst_Last = pst_Point + _pst_Obj->l_NbPoints;

    if ( pst_Data->ul_Flags & GEO_C_Symetrie_Y)
    {
        pst_Point = (GEO_Vertex *) &pst_Point->y;
        pst_Dest = (GEO_Vertex *) &pst_Dest->y;
        pst_Normals = (MATH_tdst_Vector *) &pst_Normals->y;
    }
    else if ( pst_Data->ul_Flags & GEO_C_Symetrie_Z)
    {
        pst_Point = (GEO_Vertex *) &pst_Point->z;
        pst_Dest = (GEO_Vertex *) &pst_Dest->z;
        pst_Normals = (MATH_tdst_Vector *) &pst_Normals->z;
    }

    if ( pst_Data->f_Offset == 0)
    {
	    for(; pst_Point < pst_Last; pst_Point++, pst_Dest++, pst_Normals++)
        {
            pst_Dest->x = -pst_Dest->x;
            pst_Normals->x = -pst_Normals->x;
        }
    }
    else
    {
        f = 2 * pst_Data->f_Offset;
        for(; pst_Point < pst_Last; pst_Point++, pst_Dest++, pst_Normals++)
        {
	        pst_Dest->x = f - pst_Dest->x;
            pst_Normals->x = -pst_Normals->x;
        }
    }

#ifdef ACTIVE_EDITORS
	if(_pst_Mod->ul_Flags & GEO_C_ModifierFlags_ApplyInGeom)
	{
		pst_Point = _pst_Obj->dst_Point;
		pst_Last = pst_Point + _pst_Obj->l_NbPoints;
		pst_Dest = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
		for(; pst_Point < pst_Last; pst_Point++, pst_Dest++) 
            MATH_CopyVector( VCast(pst_Point), VCast(pst_Dest) );
	}

#endif
	GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseSpecialVertexBuffer;
	GDI_gpst_CurDD->p_Current_Vertex_List = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSymetrie_Unapply
(
	MDF_tdst_Modifier		*_pst_Mod,
	GEO_tdst_Object			*_pst_Obj
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierSymetrie   *pst_Data;
    MATH_tdst_Vector            *pst_Last, *pst_Normals;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(_XENON_RENDER)
    if (GDI_b_IsXenonGraphics())
    {
        if (OBJ_b_TestIdentityFlag(_pst_Mod->pst_GO, OBJ_C_IdentityFlag_Visu))
        {
            _pst_Mod->pst_GO->pst_Base->pst_Visu->ucFlag &= ~GRO_VISU_FLAG_SYMMETRY;
        }
    }
#endif

    pst_Data = (GEO_tdst_ModifierSymetrie *) _pst_Mod->p_Data;

	GEO_UseNormals(_pst_Obj);
    pst_Normals = _pst_Obj->dst_PointNormal;
    pst_Last = pst_Normals + _pst_Obj->l_NbPoints;

    if ( pst_Data->ul_Flags & GEO_C_Symetrie_Y)
        pst_Normals = (MATH_tdst_Vector *) &pst_Normals->y;
    else if ( pst_Data->ul_Flags & GEO_C_Symetrie_Z)
        pst_Normals = (MATH_tdst_Vector *) &pst_Normals->z;

    for(; pst_Normals < pst_Last; pst_Normals++)
        pst_Normals->x = -pst_Normals->x;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_ModifierSymetrie_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char                        *pc_Cur;
	GEO_tdst_ModifierSymetrie   *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierSymetrie *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	LOA_ReadLong_Ed(&pc_Cur, NULL); // skip size

	pst_Data->ul_Flags = LOA_ReadULong(&pc_Cur);
	pst_Data->f_Offset = LOA_ReadFloat(&pc_Cur);

	return (pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSymetrie_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierSymetrie   *pst_Data;
	ULONG                       ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierSymetrie *) _pst_Mod->p_Data;

	ul_Size = 8;
	SAV_Buffer(&ul_Size, 4);

	SAV_Buffer(&pst_Data->ul_Flags, 4);
	SAV_Buffer(&pst_Data->f_Offset, 4);
}

#endif

/*$4
 ***********************************************************************************************************************
    Morphing
 ***********************************************************************************************************************
 */
#ifdef USE_DOUBLE_RENDERING	
void GEO_ModifierMorphing_Interpolate(struct MDF_tdst_Modifier_ * p_Mod, u_int Globalmode , float fInterpolatedValue)
{
	/* MODE : 
			101 -> 	Must copy current in S0  				// Camera Cut Mode 
			000 -> 	Must copy S0 in S1 , and Current in S0	// First I frame
			0XX ->  Must blend S1 -> S0 with interpolvalue	// I Frame
			100 -> 	Must copy S0 in current					// K frame
	*/
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing			*pst_Data;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	int									i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierMorphing *) p_Mod->p_Data;

	if((!pst_Data->l_NbChannel) || (!pst_Data->l_NbPoints)) return;

	pst_Channel = pst_Data->dst_MorphChannel;
	for(i = 0; i < pst_Data->l_NbChannel; i++, pst_Channel++)
	{
		u_int mode;
		mode = Globalmode;
		if ((*(ULONG *)&pst_Channel->f_Blend_0 & 1) == 0) mode = 101; // Mean last was inactive
		if (pst_Channel->f_ChannelBlend <= 0.0f) mode = 101; // 
		if (mode < 100)
		{
			float fCMin,fCMax;
			if (mode == 0)		
			{
				pst_Channel->f_Blend_1 = pst_Channel->f_Blend_0;
				pst_Channel->f_Blend_0 = pst_Channel->f_Blend;
				pst_Channel->f_ChannelBlend_1 = pst_Channel->f_ChannelBlend_0;
				pst_Channel->f_ChannelBlend_0 = pst_Channel->f_ChannelBlend;
			}
			pst_Channel->f_Blend = pst_Channel->f_Blend_1 * (1.0f - fInterpolatedValue) + pst_Channel->f_Blend_0 * (fInterpolatedValue);
			if (pst_Channel->f_ChannelBlend_1 < pst_Channel->f_ChannelBlend_0)
			{
				fCMin = pst_Channel->f_ChannelBlend_1;
				fCMax = pst_Channel->f_ChannelBlend_0;
			} else
			{
				fCMin = pst_Channel->f_ChannelBlend_0;
				fCMax = pst_Channel->f_ChannelBlend_1;
			}
			
			if ((fCMax - fCMin) > 0.8f) 
				pst_Channel->f_ChannelBlend_1 = pst_Channel->f_ChannelBlend_0 = pst_Channel->f_ChannelBlend;
			else
			 	pst_Channel->f_ChannelBlend = pst_Channel->f_ChannelBlend_1 * (1.0f - fInterpolatedValue) + pst_Channel->f_ChannelBlend_0 * (fInterpolatedValue);
			 	
			*(ULONG *)&pst_Channel->f_Blend_0 |= 1;
		} else
		if (mode == 100) // K 1
		{
			pst_Channel->f_Blend = pst_Channel->f_Blend_0;
			pst_Channel->f_ChannelBlend = pst_Channel->f_ChannelBlend_0;//*/
			*(ULONG *)&pst_Channel->f_Blend_0 |= 1;
		} else
		if (mode == 101) // K 2 (camera cut)
		{
			pst_Channel->f_Blend_1 = pst_Channel->f_Blend_0 = pst_Channel->f_Blend;
			pst_Channel->f_ChannelBlend_1 = pst_Channel->f_ChannelBlend_0 = pst_Channel->f_ChannelBlend;
			*(ULONG *)&pst_Channel->f_Blend_0 &= ~1;
		} 
	}
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing	*pst_Morph, *pst_Src;
    LONG l_Size;
    int i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierMorphing));
#if !defined(XML_CONV_TOOL)	
	pst_Morph = (GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data;

#ifdef JADEFUSION
    if(  _pst_GO->pst_Base && _pst_GO->pst_Base->pst_Visu )
    {
        _pst_GO->pst_Base->pst_Visu->ul_DrawMask |= GDI_Cul_DM_DontRecomputeNormales;
    }
#endif

	if(!p_Data)
	{
		L_memset(pst_Morph, 0, sizeof(GEO_tdst_ModifierMorphing));
		if(!_pst_GO->pst_Base->pst_Visu) 
			return;
		if(!_pst_GO->pst_Base->pst_Visu->pst_Object) return;
        if (_pst_GO->pst_Base->pst_Visu->pst_Object->i->ul_Type != GRO_Geometric ) return;
		pst_Morph->l_NbPoints = ((GEO_tdst_Object *) _pst_GO->pst_Base->pst_Visu->pst_Object)->l_NbPoints;
	}
	else
    {
        pst_Src = (GEO_tdst_ModifierMorphing *) p_Data;
		L_memcpy(pst_Morph, pst_Src, sizeof(GEO_tdst_ModifierMorphing));
#ifdef JADEFUSION    
        // Duplicate target data
        //merdo pst_Morph->dst_MorphData = GEO_ModifierMorphing_DuplicateData( pst_Src );
        ERR_X_Assert( pst_Morph->dst_MorphData );
#else
		if( pst_Morph->l_NbMorphData)
        {
            l_Size = pst_Morph->l_NbMorphData * sizeof( GEO_tdst_ModifierMorphing_Data );
            pst_Morph->dst_MorphData = (GEO_tdst_ModifierMorphing_Data *) MEM_p_Alloc( l_Size );
            L_memcpy( pst_Morph->dst_MorphData, pst_Src->dst_MorphData, l_Size );
            for ( i = 0; i < pst_Morph->l_NbMorphData; i++)
            {
                l_Size = pst_Morph->dst_MorphData[i].l_NbVector;
                if ( l_Size )
                {
                    pst_Morph->dst_MorphData[i].dl_Index = (LONG *) MEM_p_Alloc( l_Size * 4 );
                    L_memcpy( pst_Morph->dst_MorphData[i].dl_Index, pst_Src->dst_MorphData[i].dl_Index, l_Size * 4 );
                    l_Size *= sizeof(MATH_tdst_Vector);
                    pst_Morph->dst_MorphData[i].dst_Vector = (MATH_tdst_Vector *) MEM_p_Alloc( l_Size );
                    L_memcpy( pst_Morph->dst_MorphData[i].dst_Vector, pst_Src->dst_MorphData[i].dst_Vector, l_Size );
                }
            }
        }
#endif
        if (pst_Morph->l_NbChannel)
        {
            l_Size = pst_Morph->l_NbChannel * sizeof( GEO_tdst_ModifierMorphing_Channel );
            pst_Morph->dst_MorphChannel = (GEO_tdst_ModifierMorphing_Channel *) MEM_p_Alloc( l_Size );
            L_memcpy( pst_Morph->dst_MorphChannel, pst_Src->dst_MorphChannel, l_Size );
            for ( i = 0; i < pst_Morph->l_NbChannel; i++)
            {
                l_Size = pst_Morph->dst_MorphChannel[i].l_NbData * 4;
                if ( l_Size )
                {
                    pst_Morph->dst_MorphChannel[i].dl_DataIndex = (LONG *) MEM_p_Alloc( l_Size );
                    L_memcpy( pst_Morph->dst_MorphChannel[i].dl_DataIndex, pst_Src->dst_MorphChannel[i].dl_DataIndex, l_Size );
                }
            }
        }
    }
#endif //XMLTOOL
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing	*pst_Data;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data;

	if(pst_Data->l_NbMorphData)
	{
#ifdef JADEFUSION
        //merdo GEO_ModifierMorphing_DestroyData( pst_Data->dst_MorphData, pst_Data->l_NbMorphData );
#else
		for(i = 0; i < pst_Data->l_NbMorphData; i++)
		{
			if(pst_Data->dst_MorphData[i].l_NbVector)
			{
				MEM_Free(pst_Data->dst_MorphData[i].dl_Index);
				MEM_Free(pst_Data->dst_MorphData[i].dst_Vector);
			}
		}

		MEM_Free(pst_Data->dst_MorphData);
#endif
	}

	if(pst_Data->l_NbChannel)
	{
		for(i = 0; i < pst_Data->l_NbChannel; i++)
		{
			if(pst_Data->dst_MorphChannel[i].l_NbData) MEM_Free(pst_Data->dst_MorphChannel[i].dl_DataIndex);
		}

		MEM_Free(pst_Data->dst_MorphChannel);
	}

	MEM_Free(pst_Data);

#if defined XENON_RENDER
    // Clear current Xenon mesh so it will have to be recreated as normal mesh
    ERR_X_Assert( _pst_Mod->pst_GO  );

    if( _pst_Mod->pst_GO && _pst_Mod->pst_GO->pst_Base && _pst_Mod->pst_GO->pst_Base->pst_Visu )
    {
        GEO_tdst_Object * pst_Object = OBJ_p_GetGro( _pst_Mod->pst_GO );
        ERR_X_Assert( pst_Object );

        GRO_tdst_Visu * pst_Visu = _pst_Mod->pst_GO->pst_Base->pst_Visu; 

        GEO_ClearXenonMesh( pst_Visu, pst_Object, FALSE, FALSE );
    }
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
#ifdef JADEFUSION
void GEO_ModifierMorphing_ChangeNumberOfPoints( MDF_tdst_Modifier * _pst_Modifier, GEO_tdst_ModifierMorphing * _pst_Data, GEO_tdst_Object * _pst_Obj )
#else
void GEO_ModifierMorphing_ChangeNumberOfPoints(GEO_tdst_ModifierMorphing *_pst_Data, GEO_tdst_Object *_pst_Obj )
#endif
{
    int     n, i, j;
    char    sz_Text[1024];

    n = _pst_Obj->l_NbPoints;

    if (n < _pst_Data->l_NbPoints)
    {
        for (i = 0; i < _pst_Data->l_NbMorphData; i++)
        {
            for (j = 0; j < _pst_Data->dst_MorphData[i].l_NbVector; j++)
            {
                if (_pst_Data->dst_MorphData[i].dl_Index[j] >= n)
                    _pst_Data->dst_MorphData[i].dl_Index[j] = n-1;
            }
        }
    }

    _pst_Data->l_NbPoints = n;

#ifdef JADEFUSION
    // Recompute tangent space basis data
    GEO_ModifierMorphing_RecomputeAllTangentSpaceData( _pst_Modifier );
#endif

    sprintf( sz_Text, "[Warning] Change number of point of morphing used by (%s), you must save data", _pst_Obj->st_Id.sz_Name ? _pst_Obj->st_Id.sz_Name : "Unknown" );
    LINK_PrintStatusMsg( sz_Text );
}
#ifdef JADEFUSION
#define M_4Edit_TestNumberOfPoints( _mod, _morph, _obj )\
    if (_morph->l_NbPoints != _obj->l_NbPoints) GEO_ModifierMorphing_ChangeNumberOfPoints( _mod, _morph, _obj );
#else
#define M_4Edit_TestNumberOfPoints( _morph, _obj )\
    if (_morph->l_NbPoints != _obj->l_NbPoints) GEO_ModifierMorphing_ChangeNumberOfPoints( _morph, _obj );
#endif //JADFUSION
#else
#ifdef JADEFUSION
#define M_4Edit_TestNumberOfPoints(a, b, c)
#else //JADFUSION
#define M_4Edit_TestNumberOfPoints(a, b)
#endif //JADFUSION
#endif //editor

#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void GEO_ModifierMorphing_Apply_TangentSpaceBasis( GEO_tdst_ModifierMorphing * _pst_Data, 
                                                   GEO_tdst_Object *           _pst_Obj,
                                                   GRO_tdst_Visu *             _pst_Visu )
{
    ERR_X_Assert( _pst_Visu && _pst_Obj->l_NbPoints < SOFT_Cul_ComputingBufferSize );

    LONG * pst_Mark = (LONG*) GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB2;
    memset( pst_Mark, 0, _pst_Obj->l_NbPoints * sizeof(LONG) );

    MATH_tdst_Quaternion ** pst_QuatPtr = (MATH_tdst_Quaternion **) GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB3;

    // Init the computing buffers for morphing
   
    memset( g_oMorphComputingBuffers.dst_NbrRotations, 0, _pst_Obj->l_NbPoints * sizeof(BYTE) );
    for( LONG i=0; i<_pst_Obj->l_NbPoints; ++i )
    {
        MATH_InitQuaternion( &g_oMorphComputingBuffers.dst_Quaternions[i], 0.0f, 0.0f, 0.0f, 0.0f );
    }

    MATH_tdst_Quaternion quatIdentity;
    MATH_InitQuaternion( &quatIdentity, 0.0f, 0.0f, 0.0f, 1.0f );

    // Accumulate contribution from every channel
    GEO_tdst_ModifierMorphing_Channel * pst_Channel = _pst_Data->dst_MorphChannel;
    for(LONG i = 0; i < _pst_Data->l_NbChannel; i++, pst_Channel++)
    {
        if(pst_Channel->l_NbData == 0) continue;
        if(pst_Channel->f_Blend == 0.0f) continue;

        ERR_X_Assert( pst_Channel->dst_CurrMorphChannel1 != NULL );

        GEO_tdst_ModifierMorphing_Data * pst_Basis1 = pst_Channel->dst_CurrMorphChannel1;
        GEO_tdst_ModifierMorphing_Data * pst_Basis2 = pst_Channel->dst_CurrMorphChannel2;
 
        MATH_tdst_Quaternion qOut;

        // Mark every vertex that has rotation data in target 1
        if( pst_Channel->f_CurrChannelBlend > 1e-3f )
        {
            float fBlend = pst_Channel->f_CurrChannelBlend * pst_Channel->f_Blend;

            for(LONG j = 0; j < pst_Basis1->l_NbQuaternions; j++)
            {
                LONG l_IndexQuat1 = pst_Basis1->dl_IndexQuat[j];
                ERR_X_Assert( l_IndexQuat1 < _pst_Obj->l_NbPoints );

                MATH_tdst_Quaternion * pQuat = &pst_Basis1->dst_Quaternions[j];
                MATH_tdst_Quaternion * pQuatFinal = &g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat1];

                if( pst_Basis2 )
                {
                    // Mark
                    pst_Mark[l_IndexQuat1] = 1;
                    pst_QuatPtr[l_IndexQuat1] = pQuat;
                }
                else
                {
                    // We can slerp directly
                    MATH_QuaternionBlend( &qOut, &quatIdentity, pQuat, fBlend, true );

                    if( g_oMorphComputingBuffers.dst_NbrRotations[l_IndexQuat1] > 0 )
                    {
                        // Consider rotation small enough to be commutative
                        MATH_MulQuaternion( &g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat1], 
                            &g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat1],
                            &qOut );
                    }
                    else
                    {
                        g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat1] = qOut;
                    }
                    ++g_oMorphComputingBuffers.dst_NbrRotations[l_IndexQuat1];
                }
            }
        }

        if( pst_Basis2 )
        {
            // For every rotation data in target 2, slerp with target 1 if marked, else slerp with Identity
            if( (1.0f - pst_Channel->f_CurrChannelBlend) > 1e-3f )
            {
                BOOL bNeedsBlend = (fabs( 1.0f - pst_Channel->f_Blend ) > 0.03f);

                for(LONG j = 0; j < pst_Basis2->l_NbQuaternions; j++)
                {
                    LONG l_IndexQuat2 = pst_Basis2->dl_IndexQuat[j];
                    ERR_X_Assert( l_IndexQuat2 < _pst_Obj->l_NbPoints );

                    MATH_tdst_Quaternion * pst_Quat2 = &pst_Basis2->dst_Quaternions[j];
                    MATH_tdst_Quaternion * pst_Quat1 = &quatIdentity;

                    if( pst_Mark[l_IndexQuat2] == 1 )
                    {
                        pst_Mark[l_IndexQuat2] = 0;
                        pst_Quat1 = pst_QuatPtr[l_IndexQuat2];
                    }

                    // Slerp with target 1
                    MATH_QuaternionBlend( &qOut, pst_Quat2, pst_Quat1, pst_Channel->f_CurrChannelBlend, true );

                    if( bNeedsBlend )
                    {
                        MATH_QuaternionBlend( &qOut, &quatIdentity, &qOut, pst_Channel->f_Blend, true );
                    }

                    if( g_oMorphComputingBuffers.dst_NbrRotations[l_IndexQuat2] > 0 )
                    {
                        // Consider rotation small enough to be commutative
                        MATH_MulQuaternion( &g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat2], 
                            &g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat2],
                            &qOut );
                    }
                    else
                    {
                        g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat2] = qOut;
                    }
                    ++g_oMorphComputingBuffers.dst_NbrRotations[l_IndexQuat2];
                }
            }

            // Now all vertices in target 1 that are still marked, need to be slerped with Identity
            if( pst_Channel->f_CurrChannelBlend > 1e-3f )
            {
                float fBlend = pst_Channel->f_CurrChannelBlend * pst_Channel->f_Blend;

                for(LONG j = 0; j < pst_Basis1->l_NbQuaternions; j++)
                {
                    LONG l_IndexQuat1 = pst_Basis1->dl_IndexQuat[j];
                    ERR_X_Assert( l_IndexQuat1 < _pst_Obj->l_NbPoints );

                    if( pst_Mark[l_IndexQuat1] == 1 )
                    {
                        pst_Mark[l_IndexQuat1] = 0;
                        MATH_tdst_Quaternion * pQuat = &pst_Basis1->dst_Quaternions[j];
                        MATH_QuaternionBlend( &qOut, &quatIdentity, pQuat, fBlend, true );
                        if( g_oMorphComputingBuffers.dst_NbrRotations[l_IndexQuat1] > 0 )
                        {
                            // Consider rotation small enough to be commutative
                            MATH_MulQuaternion( &g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat1], 
                                &g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat1],
                                &qOut );
                        }
                        else
                        {
                            g_oMorphComputingBuffers.dst_Quaternions[l_IndexQuat1] = qOut;
                        }
                        ++g_oMorphComputingBuffers.dst_NbrRotations[l_IndexQuat1];
                    }
                }
            }
        }
    } // end foreach channel

    // Now we can calculate the normals and tangents for this element and store it in the corect computing buffer
    GEO_Vertex * pNormals   = g_oMorphComputingBuffers.GetNormals( );
    GEO_Vertex * pTangents  = g_oMorphComputingBuffers.GetTangents( );

    ERR_X_Assert( pNormals && pTangents && _pst_Visu->p_XeElements[0].dst_TangentSpace );
    for( LONG p=0; p<_pst_Obj->l_NbPoints; ++p )
    {
        if( g_oMorphComputingBuffers.dst_NbrRotations[p] > 0 )
        {
            MATH_tdst_Quaternion * pQuatResult = &g_oMorphComputingBuffers.dst_Quaternions[p];
            MATH_QuaternionTransformVector( &pNormals[p], pQuatResult, &_pst_Visu->p_XeElements[0].dst_TangentSpace[p].SxT );
            MATH_QuaternionTransformVector( &pTangents[p], pQuatResult, &_pst_Visu->p_XeElements[0].dst_TangentSpace[p].S );
        }
        else
        {
            // Just copy initial value
            pNormals[p]  = _pst_Visu->p_XeElements[0].dst_TangentSpace[p].SxT;
            pTangents[p] = _pst_Visu->p_XeElements[0].dst_TangentSpace[p].S;
        }
    }
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector					V, *pst_SrcMorph;
    GEO_Vertex                          *pst_Dest;
	float								f_ChannelBlend, f_Blend;
	GEO_tdst_ModifierMorphing			*pst_Data;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	GEO_tdst_ModifierMorphing_Data		*pst_Morph;
    ULONG                               *pul_Data;
	LONG								l_Data, l_End;
	int									i, j;
    GEO_tdst_StaticLOD                  *pst_LOD;

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
    GRO_tdst_Visu                       *pst_Visu = NULL;
    ERR_X_Assert( _pst_Mod != NULL && _pst_Obj != NULL );
#endif

	pst_Data = (GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data;

#ifdef JADEFUSION
    ERR_X_Assert( pst_Data != NULL );

    if( _pst_Mod->ul_Flags & MDF_C_Modifier_Inactive )
    {
        return;
    }

    if( _pst_Mod->pst_GO->pst_Base && _pst_Mod->pst_GO->pst_Base->pst_Visu )
    {
        pst_Visu = _pst_Mod->pst_GO->pst_Base->pst_Visu;

        pst_Visu->ul_DrawMask |= GDI_Cul_DM_DontRecomputeNormales;
    }
    else
    {
        return;
    }
    ERR_X_Assert( pst_Visu );

	if( 
#ifndef _XENON_RENDER
        (!pst_Data->l_NbChannel) || 
#endif
        (!pst_Data->l_NbPoints)
        )
    {
        return;
    }
#else //JADEFUSION
	if((!pst_Data->l_NbChannel) || (!pst_Data->l_NbPoints)) return;
#endif

	pst_Dest = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;

    if (!_pst_Obj) return;
    if ( _pst_Obj->st_Id.i->ul_Type == GRO_GeoStaticLOD )
    {
        pst_LOD = ((GEO_tdst_StaticLOD *) _pst_Obj);
        if ( ( pst_LOD->auc_EndDistance[0] >= pst_LOD->uc_Distance ) && (pst_LOD->uc_NbLOD > 1) )
            return;
        _pst_Obj = (GEO_tdst_Object *) pst_LOD->dpst_Id[0];
    }
    if ( _pst_Obj->st_Id.i->ul_Type != GRO_Geometric )
        return;
        
	JADED_PROFILER_START();
        
#ifdef JADEFUSION
    // Allocate elements if needed
    if( pst_Visu->p_XeElements == NULL )
    {
        pst_Visu->p_XeElements = (GRO_tdst_XeElement*)MEM_p_Alloc( _pst_Obj->l_NbElements * sizeof(GRO_tdst_XeElement) );
        ERR_X_Assert( pst_Visu->p_XeElements );

        L_memset(pst_Visu->p_XeElements, 0, (_pst_Obj->l_NbElements * sizeof(GRO_tdst_XeElement)));
        pst_Visu->l_NbXeElements = _pst_Obj->l_NbElements;
    }
    
    bool bNeedsRecompute = (_pst_Obj->l_NbPoints   != pst_Data->l_NbPoints);

    if( bNeedsRecompute )
    {
#if defined ACTIVE_EDITORS
        GEO_ModifierMorphing_ChangeNumberOfPoints( _pst_Mod, pst_Data, _pst_Obj );
#else
        return;
#endif
    }

    // Tangent space always needs to be present
    ERR_X_Assert( pst_Visu->p_XeElements );
    if( pst_Visu->p_XeElements[0].dst_TangentSpace == NULL )
    {
        // Allocate storage for tangent space data
        pst_Visu->p_XeElements[0].dst_TangentSpace = (GEO_tdst_TextureSpaceBasis*) MEM_p_Alloc( _pst_Obj->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));
        L_memset( pst_Visu->p_XeElements[0].dst_TangentSpace, 0, _pst_Obj->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));

        // Compute the tangent space basis data for the reference mesh
        GEO_CalculateTangentSpaceBasis( _pst_Obj, 0, pst_Visu->p_XeElements[0].dst_TangentSpace, _pst_Obj->dst_Point );
    }

#ifdef ACTIVE_EDITORS
    // If the tangent space basis was not created at load time or the number of elements do not match,
    // we need to recompute now
    pst_Morph = pst_Data->dst_MorphData;
   
    for(i = 0; i < pst_Data->l_NbMorphData; i++, pst_Morph++)
    {
        ERR_X_Assert( pst_Morph );
        if( pst_Morph->l_NbQuaternions == 0 ) 
        {
            GEO_l_ModifierMorphing_ComputeTangentSpaceBasisData( _pst_Mod, pst_Morph, _pst_Obj );
        }
    }
#endif
#else
    M_4Edit_TestNumberOfPoints( pst_Data, _pst_Obj)
#endif
    /* Save points if needed */
	if(!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer))
	{
		GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseSpecialVertexBuffer;
#ifdef JADEFUSION
        ERR_X_Assert( _pst_Obj->l_NbPoints < SOFT_Cul_ComputingBufferSize );
#endif		
		GDI_gpst_CurDD->p_Current_Vertex_List = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
		L_memcpy( pst_Dest, _pst_Obj->dst_Point, _pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
	}

	pst_Channel = pst_Data->dst_MorphChannel;
	for(i = 0; i < pst_Data->l_NbChannel; i++, pst_Channel++)
	{
		if(pst_Channel->l_NbData == 0) continue;
		if(pst_Channel->f_Blend == 0.0f) continue;
#ifndef JADEFUSION
		if(pst_Channel->f_ChannelBlend == 0.0f) continue;
#endif
		pul_Data = (ULONG *) pst_Channel->dl_DataIndex;

		l_Data = (LONG) (pst_Channel->f_ChannelBlend);
        l_End = l_Data;
        
        while ( (l_End < pst_Channel->l_NbData) && ( pul_Data[l_End] >= (ULONG) pst_Data->l_NbMorphData) )
            l_End++;

		if(l_End >= pst_Channel->l_NbData)
		{
			f_ChannelBlend = 1.0f;
			l_End = pst_Channel->l_NbData - 1;
            while ( (l_End > 0) && ( pul_Data[l_End] >= (ULONG) pst_Data->l_NbMorphData) )
                l_End--;
            if (pul_Data[l_End] >= (ULONG) pst_Data->l_NbMorphData) continue;
		}
		else
        {
            l_Data--;
             while ( (l_Data >= 0) && (pul_Data[l_Data] >= (ULONG) pst_Data->l_NbMorphData) )
                l_Data--;
			f_ChannelBlend = (pst_Channel->f_ChannelBlend - (float) l_Data - 1) / (float)( l_End - l_Data);
        }

#ifdef JADEFUSION
        // Cache the pointers to targets that this channel will blend 
        // These are reused when blending tangent space basis per element
        pst_Channel->f_CurrChannelBlend = f_ChannelBlend;
        pst_Channel->dst_CurrMorphChannel1 = pst_Data->dst_MorphData + pul_Data[l_End];
        pst_Channel->dst_CurrMorphChannel2 = NULL;
        if( (l_Data >= 0) && (1.0f - f_ChannelBlend) > 0.0f)
        {
            pst_Channel->dst_CurrMorphChannel2 = pst_Data->dst_MorphData + pul_Data[l_Data];
        }

        if(pst_Channel->f_ChannelBlend == 0.0f) continue;
#endif

#ifdef USE_DOUBLE_RENDERING	
		*(ULONG *)&pst_Channel->f_Blend_0 |= 1;
#endif		

#ifdef JADEFUSION
        // Blend positions
		if ( f_ChannelBlend > 0.0f )
		{
   			f_Blend = pst_Channel->f_Blend * f_ChannelBlend;
         	pst_SrcMorph = pst_Channel->dst_CurrMorphChannel1->dst_Vector;


            if (f_Blend == 1.0f )
            {
                // No blend, just copy the morph target data to destination
                for(j = 0; j < pst_Channel->dst_CurrMorphChannel1->l_NbVector; j++)
                {
                    MATH_AddEqualVector(VCast( pst_Dest + pst_Channel->dst_CurrMorphChannel1->dl_Index[j]), pst_SrcMorph + j);
                }
            }
            else
            {
                // Add this channel's contribution 
			    for(j = 0; j < pst_Channel->dst_CurrMorphChannel1->l_NbVector; j++)
			    {
				    MATH_ScaleVector(&V, pst_SrcMorph + j, f_Blend);
				    MATH_AddEqualVector(VCast( pst_Dest + pst_Channel->dst_CurrMorphChannel1->dl_Index[j]), &V);
			    }
            }
		}

		f_ChannelBlend = 1.0f - f_ChannelBlend;
		if( pst_Channel->dst_CurrMorphChannel2 != NULL )
		{
			f_Blend = pst_Channel->f_Blend * f_ChannelBlend;
			pst_SrcMorph = pst_Channel->dst_CurrMorphChannel2->dst_Vector;


            if (f_Blend == 1)
            {
                for(j = 0; j < pst_Channel->dst_CurrMorphChannel2->l_NbVector; j++)
                {
        	        MATH_AddEqualVector(VCast( pst_Dest + pst_Channel->dst_CurrMorphChannel2->dl_Index[j]), pst_SrcMorph + j);
                }
            }
            else
            {
    	        for(j = 0; j < pst_Channel->dst_CurrMorphChannel2->l_NbVector; j++)
	    	    {
		    	    MATH_ScaleVector(&V, pst_SrcMorph + j, f_Blend);
			    	MATH_AddEqualVector(VCast( pst_Dest + pst_Channel->dst_CurrMorphChannel2->dl_Index[j] ), &V);
                }
            }
		}
#else
		if ( f_ChannelBlend > 0 )
		{
			f_Blend = pst_Channel->f_Blend * f_ChannelBlend;
            pst_Morph = pst_Data->dst_MorphData + pul_Data[l_End];
			pst_SrcMorph = pst_Morph->dst_Vector;

            if (f_Blend == 1)
            {
                for(j = 0; j < pst_Morph->l_NbVector; j++)
				    MATH_AddEqualVector(VCast( pst_Dest + pst_Morph->dl_Index[j]), pst_SrcMorph + j);
            }
            else
            {
			    for(j = 0; j < pst_Morph->l_NbVector; j++)
			    {
				    MATH_ScaleVector(&V, pst_SrcMorph + j, f_Blend);
				    MATH_AddEqualVector(VCast( pst_Dest + pst_Morph->dl_Index[j]), &V);
			    }
            }
		}

		f_ChannelBlend = 1.0f - f_ChannelBlend;
		if((l_Data >= 0) && (f_ChannelBlend > 0))
		{
			f_Blend = pst_Channel->f_Blend * f_ChannelBlend;
		    pst_Morph = pst_Data->dst_MorphData + pul_Data[l_Data];
			pst_SrcMorph = pst_Morph->dst_Vector;

            if (f_Blend == 1)
            {
                for(j = 0; j < pst_Morph->l_NbVector; j++)
        	        MATH_AddEqualVector(VCast( pst_Dest + pst_Morph->dl_Index[j]), pst_SrcMorph + j);
            }
            else
            {
    	        for(j = 0; j < pst_Morph->l_NbVector; j++)
	    	    {
		    	    MATH_ScaleVector(&V, pst_SrcMorph + j, f_Blend);
			    	MATH_AddEqualVector(VCast( pst_Dest + pst_Morph->dl_Index[j] ), &V);
                }
            }
		}
#endif
	}

#ifdef ACTIVE_EDITORS
    if(_pst_Mod->ul_Flags & GEO_C_ModifierFlags_ApplyInGeom)
        L_memcpy( _pst_Obj->dst_Point, pst_Dest, _pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
#endif

#ifdef JADEFUSION
    // Now compute the tangent space basis
    if( pst_Visu )
    {
        GEO_ModifierMorphing_Apply_TangentSpaceBasis( pst_Data, _pst_Obj, pst_Visu );
    }

    _pst_Obj->b_CreateMorphedMesh = TRUE;
#endif

	JADED_PROFILER_END();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}
#if defined(XML_CONV_TOOL)
ULONG gMorphingMdfSize;
ULONG gMorphingMdfVersion;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_ModifierMorphing_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char								*pc_Cur;
	GEO_tdst_ModifierMorphing			*pst_Data;
	GEO_tdst_ModifierMorphing_Data		*pst_MorphData;
	GEO_tdst_ModifierMorphing_Channel	*pst_MorphChannel;
	ULONG								ul_Size;
	int									i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	ul_Size = LOA_ReadULong(&pc_Cur);
#ifdef JADEFUSION
    ULONG ul_Version = 1;

    if( ul_Size == 0 )
    {
        ul_Version = LOA_ReadULong(&pc_Cur);
    }
#if defined(XML_CONV_TOOL)
	gMorphingMdfSize = ul_Size;
	gMorphingMdfVersion = ul_Version;
#endif
#endif
	/* read main data */
	pst_Data->ul_Flags = LOA_ReadULong(&pc_Cur);
	pst_Data->l_NbPoints = LOA_ReadLong(&pc_Cur);
	pst_Data->l_NbMorphData = LOA_ReadLong(&pc_Cur);
	pst_Data->l_NbChannel = LOA_ReadLong(&pc_Cur);

#ifdef JADEFUSION
    ULONG l_NbElements = 0;
    if( ul_Version == 2 )
    {
        l_NbElements = LOA_ReadLong(&pc_Cur);
    }

	/* allocate morph data array */
	j = pst_Data->l_NbMorphData * sizeof(GEO_tdst_ModifierMorphing_Data);
    if( j > 0 )
    {
        pst_Data->dst_MorphData = (GEO_tdst_ModifierMorphing_Data *) MEM_p_Alloc(j);
        memset( pst_Data->dst_MorphData, 0, j );
    }

    /* allocate morph channel array */
    j = pst_Data->l_NbChannel * sizeof(GEO_tdst_ModifierMorphing_Channel);
    if( j > 0 )
    {
        pst_Data->dst_MorphChannel = (GEO_tdst_ModifierMorphing_Channel *) MEM_p_Alloc(j);
        memset( pst_Data->dst_MorphChannel, 0, j );
    }
#else //JADEFUSION
	/* allocate main data */
	j = pst_Data->l_NbMorphData * sizeof(GEO_tdst_ModifierMorphing_Data);
	pst_Data->dst_MorphData = (GEO_tdst_ModifierMorphing_Data *) (j ? MEM_p_Alloc(j) : 0);

	j = pst_Data->l_NbChannel * sizeof(GEO_tdst_ModifierMorphing_Channel);
	pst_Data->dst_MorphChannel = (GEO_tdst_ModifierMorphing_Channel *) (j ? MEM_p_Alloc(j) : 0);
#endif

	/* get morph data */
	pst_MorphData = pst_Data->dst_MorphData;
	for(i = 0; i < pst_Data->l_NbMorphData; i++, pst_MorphData++)
	{
		pst_MorphData->l_NbVector = LOA_ReadLong(&pc_Cur);
		LOA_ReadString_Ed(&pc_Cur, pst_MorphData->sz_Name, 64);

		if(pst_MorphData->l_NbVector)
		{
			LONG * p;
			MATH_tdst_Vector * q;
			pst_MorphData->dl_Index = p = (LONG *) MEM_p_Alloc(pst_MorphData->l_NbVector * sizeof(LONG));
			for(j = 0; j < pst_MorphData->l_NbVector; ++j, ++p)
			{
				*p = LOA_ReadLong(&pc_Cur);
			}

			pst_MorphData->dst_Vector = q = (MATH_tdst_Vector *) MEM_p_Alloc(pst_MorphData->l_NbVector * sizeof(MATH_tdst_Vector));

			for(j = 0; j < pst_MorphData->l_NbVector; ++j, ++q)
			{
				LOA_ReadVector(&pc_Cur, q);
			}
		}
#ifdef JADEFUSION
        if( ul_Version == 2 )
        {
            // Allocate and read tangent space basis target data
            if( l_NbElements > 0 )
            {
                j = l_NbElements * sizeof(GEO_tdst_ModifierMorphing_TangentSpaceBasis);
                
                // Create array for every element
                for( LONG e=0; e<l_NbElements; ++e )
                {
                    LONG l_NbQuaternions = LOA_ReadLong( &pc_Cur );

                    if( l_NbQuaternions )
                    {
                        MATH_tdst_Quaternion quat;
                        // Read quaternions
                        for( LONG q=0; q<l_NbQuaternions; ++q)
                        {
                            LOA_ReadQuaternion( &pc_Cur, &quat );
                        }
                        // Read index
                        for( LONG q=0; q<l_NbQuaternions; ++q)
                        {
                            LOA_ReadLong( &pc_Cur );
                        }
                    }
                }
            }
        }
        if( ul_Version >= 3 )
        {
            pst_MorphData->l_NbQuaternions = LOA_ReadLong( &pc_Cur );

            // Create array for every element
            if( pst_MorphData->l_NbQuaternions > 0 )
            {
                // Alloc
                pst_MorphData->dl_IndexQuat     = (LONG *) MEM_p_Alloc(pst_MorphData->l_NbQuaternions * sizeof(LONG));
                pst_MorphData->dst_Quaternions  = (MATH_tdst_Quaternion *) MEM_p_Alloc(pst_MorphData->l_NbQuaternions * sizeof(MATH_tdst_Quaternion));

                // Read quaternions
                for( LONG q=0; q<pst_MorphData->l_NbQuaternions; ++q)
                {
                    LOA_ReadQuaternion( &pc_Cur, &pst_MorphData->dst_Quaternions[q] );
                }

                // Read index
                for( LONG q=0; q<pst_MorphData->l_NbQuaternions; ++q)
                {
                     pst_MorphData->dl_IndexQuat[q] = LOA_ReadLong( &pc_Cur );
                }
            }
        }
#else		
		else
		{
			pst_MorphData->dl_Index = NULL;
			pst_MorphData->dst_Vector = NULL;
		}
#endif
	}

	/* get channel data */
	pst_MorphChannel = pst_Data->dst_MorphChannel;
	for(i = 0; i < pst_Data->l_NbChannel; i++, pst_MorphChannel++)
	{
		pst_MorphChannel->l_NbData = LOA_ReadLong(&pc_Cur);
		pst_MorphChannel->f_Blend = LOA_ReadFloat(&pc_Cur);
		pst_MorphChannel->f_ChannelBlend = LOA_ReadFloat(&pc_Cur);
#ifdef USE_DOUBLE_RENDERING	
		pst_MorphChannel->f_Blend_1 = 
		pst_MorphChannel->f_Blend_0 = 
		pst_MorphChannel->f_ChannelBlend_1 = 
		pst_MorphChannel->f_ChannelBlend_0 = 0.0f;
#endif		

		LOA_ReadString_Ed(&pc_Cur, pst_MorphChannel->sz_Name, 64);

		if(pst_MorphChannel->l_NbData)
		{
			LONG * p;

			pst_MorphChannel->dl_DataIndex = p = (LONG *) MEM_p_Alloc(sizeof(LONG) * pst_MorphChannel->l_NbData);
			for(j = 0; j < pst_MorphChannel->l_NbData; ++j, ++p)
			{
				*p = LOA_ReadLong(&pc_Cur);
			}
		}
	}

	return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing			*pst_Data;
	GEO_tdst_ModifierMorphing_Data		*pst_MorphData;
	GEO_tdst_ModifierMorphing_Channel	*pst_MorphChannel;
	ULONG								ul_Size;
	int									i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data;
	ul_Size = 0;

#ifdef JADEFUSION
	ULONG ul_Version = Morphing_C_VersionNumber;
#if defined(XML_CONV_TOOL)
	ul_Size = gMorphingMdfSize;
	ul_Version = gMorphingMdfVersion;
#endif
    SAV_Buffer(&ul_Size, 4);
    if (ul_Size == 0)
		SAV_Buffer(&ul_Version, 4);
#else
	/* First compute size of data to save it */
	ul_Size += 4;	/* Flags */
	ul_Size += 4;	/* Number of points */
	ul_Size += 4;	/* Number of morph data */
	ul_Size += 4;	/* Number of channel */
	pst_MorphData = pst_Data->dst_MorphData;
	for(i = 0; i < pst_Data->l_NbMorphData; i++, pst_MorphData++)
	{
		ul_Size += 4;																		/* Number of data */
		ul_Size += 64;																		/* Name */
		ul_Size += pst_MorphData->l_NbVector * (sizeof(LONG) + sizeof(MATH_tdst_Vector));	/* Data */
	}

	pst_MorphChannel = pst_Data->dst_MorphChannel;
	for(i = 0; i < pst_Data->l_NbChannel; i++)
	{
		ul_Size += 12;																		/* Number of data + blend +
																							 * progression */
		ul_Size += 64;																		/* Name */
		ul_Size += sizeof(LONG) * pst_MorphChannel->l_NbData;								/* Data */
	}

	SAV_Buffer(&ul_Size, 4);
#endif
	/* Now save */
	SAV_Buffer(&pst_Data->ul_Flags, 4);														/* Flags */
	SAV_Buffer(&pst_Data->l_NbPoints, 4);													/* Number of points */
	SAV_Buffer(&pst_Data->l_NbMorphData, 4);												/* Number of morph data */
	SAV_Buffer(&pst_Data->l_NbChannel, 4);													/* Number of channel */

	pst_MorphData = pst_Data->dst_MorphData;
	for(i = 0; i < pst_Data->l_NbMorphData; i++, pst_MorphData++)
	{
		SAV_Buffer(&pst_MorphData->l_NbVector, 4);											/* Number of data */
		SAV_Buffer(pst_MorphData->sz_Name, 64);												/* Name */
		if(pst_MorphData->l_NbVector)
		{
			SAV_Buffer(pst_MorphData->dl_Index, 4 * pst_MorphData->l_NbVector);
			SAV_Buffer(pst_MorphData->dst_Vector, sizeof(MATH_tdst_Vector) * pst_MorphData->l_NbVector);
		}

#ifdef JADEFUSION
        SAV_Buffer( &pst_MorphData->l_NbQuaternions, 4 );

        if( pst_MorphData->l_NbQuaternions > 0 )
        {
            SAV_Buffer( pst_MorphData->dst_Quaternions, pst_MorphData->l_NbQuaternions * sizeof(MATH_tdst_Quaternion) );
            SAV_Buffer( pst_MorphData->dl_IndexQuat, pst_MorphData->l_NbQuaternions * sizeof(LONG) );
        }
#endif
	}

	pst_MorphChannel = pst_Data->dst_MorphChannel;
	for(i = 0; i < pst_Data->l_NbChannel; i++, pst_MorphChannel++)
	{
		SAV_Buffer(&pst_MorphChannel->l_NbData, 4);
		SAV_Buffer(&pst_MorphChannel->f_Blend, 4);
		SAV_Buffer(&pst_MorphChannel->f_ChannelBlend, 4);
		SAV_Buffer(pst_MorphChannel->sz_Name, 64);
		if(pst_MorphChannel->l_NbData)
			SAV_Buffer(pst_MorphChannel->dl_DataIndex, sizeof(LONG) * pst_MorphChannel->l_NbData);
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_Swap( GEO_tdst_ModifierMorphing *pst_Morph, int data1, int data2, BOOL _b_UpdChannel )
{
    GEO_tdst_ModifierMorphing_Data *M1, *M2, M;

    if (data1 == data2) return;
    if ( (data1 < 0) || (data1 >= pst_Morph->l_NbMorphData ) ) return;
    if ( (data2 < 0) || (data2 >= pst_Morph->l_NbMorphData ) ) return;

    M1 = pst_Morph->dst_MorphData + data1;
    M2 = pst_Morph->dst_MorphData + data2;


    L_memcpy( &M, M1, sizeof( GEO_tdst_ModifierMorphing_Data ) );
    L_memcpy( M1, M2, sizeof( GEO_tdst_ModifierMorphing_Data ) );
    L_memcpy( M2, &M, sizeof( GEO_tdst_ModifierMorphing_Data ) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_Move( GEO_tdst_ModifierMorphing *pst_Morph, int from, int to, BOOL _b_UpdChannel )
{
    GEO_tdst_ModifierMorphing_Data  *M1, *M2, M;
    int                             inc;

    if (from == to) return;
    if ( (from < 0) || (from >= pst_Morph->l_NbMorphData ) ) return;
    if ( (to < 0) || (to >= pst_Morph->l_NbMorphData ) ) return;

    inc = (from < to) ? 1 : -1;
    M1 = pst_Morph->dst_MorphData + from;
    M2 = pst_Morph->dst_MorphData + to;
    
    L_memcpy( &M, M1, sizeof( GEO_tdst_ModifierMorphing_Data ) );

    while ( M1 != M2 )
    {
        L_memcpy( M1, M1 + inc, sizeof( GEO_tdst_ModifierMorphing_Data ) );
        M1 += inc;
    }

    L_memcpy( M1, &M, sizeof( GEO_tdst_ModifierMorphing_Data ) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
typedef struct GEO_PickMorph_Data_
{
    short   UV[6];
    short   nbUVs;
    short   Point;
    float   dist;
    char    Sym;
    char    treated;
} GEO_PickMorph_Data;

static GEO_tdst_UV *GEO_PickMorph_UV0;
static GEO_tdst_UV *GEO_PickMorph_UV1;

float GEO_PickMorph_Dist( GEO_PickMorph_Data *D0, GEO_PickMorph_Data *D1 )
{
    float           f, temp, temptemp;
    GEO_tdst_UV     *UV0, *UV1;
    int             i, j;

    if ( D0->nbUVs < D1->nbUVs ) 
        return GEO_PickMorph_Dist( D1, D0 );

    f = 0;
    for (i = 0; i < D0->nbUVs; i++)
    {
        UV0 = GEO_PickMorph_UV0 + D0->UV[i];
        temp = Cf_Infinit;
        for (j = 0; j < D1->nbUVs; j++)
        {
            UV1 = GEO_PickMorph_UV1 + D1->UV[j];
            temptemp = fSqr( UV0->fU - UV1->fU) + fSqr( UV0->fV - UV1->fV);
            if (temptemp < temp) temp = temptemp;
        }
        f += temp;
    }
    return f;

    f = Cf_Infinit;
    for (i = 0; i < D0->nbUVs; i++)
    {
        UV0 = GEO_PickMorph_UV0 + D0->UV[i];
        for (j = 0; j < D1->nbUVs; j++)
        {
            UV1 = GEO_PickMorph_UV1 + D1->UV[j];
            temp = fSqr( UV0->fU - UV1->fU) + fSqr( UV0->fV - UV1->fV);
            if (temp < f) f = temp;
        }
    }
    return f;

    f = 0;
    if ( D0->nbUVs > D1->nbUVs )
    {
        for (i = 0; i < D0->nbUVs; i++)
        {
            UV0 = GEO_PickMorph_UV0 + D0->UV[i];
            UV1 = GEO_PickMorph_UV1 + D1->UV[i];
            f += fSqr( UV0->fU - UV1->fU) + fSqr( UV0->fV - UV1->fV);
        }
        for ( ; i < D1->nbUVs; i++)
        {
            UV1 = GEO_PickMorph_UV1 + D0->UV[i];
            f+= fSqr( UV1->fU ) + fSqr( UV1->fV );
        }
    }
    else
    {
        for (i = 0; i < D1->nbUVs; i++)
        {
            UV0 = GEO_PickMorph_UV0 + D0->UV[i];
            UV1 = GEO_PickMorph_UV1 + D1->UV[i];
            f += fSqr( UV0->fU - UV1->fU) + fSqr( UV0->fV - UV1->fV);
        }
        for ( ; i < D0->nbUVs; i++)
        {
            UV0 = GEO_PickMorph_UV0 + D0->UV[i];
            f+= fSqr( UV0->fU ) + fSqr( UV0->fV );
        }
    }
    return f;
}


LONG GEO_l_ModifierMorphing_AddDataExt(GEO_tdst_Object *pst_TgtGeo, GEO_tdst_Object *pst_SrcGeo, char *pc_Sel, MATH_tdst_Vector *V )
{
    GEO_tdst_IndexedTriangle    *T, *TLast;
    GEO_PickMorph_Data          *D, *DSym, *pst_TgtData, *pst_SrcData;
    GEO_tdst_UV                 *UV0, *UV1;
    int                         i, j, k, l , m, nb;
    float                       f, f1, f2;
    MATH_tdst_Vector            VSym;
    char                        *pc_SrcSel;

    GEO_PickMorph_UV0 = pst_TgtGeo->dst_UV;
    GEO_PickMorph_UV1 = pst_SrcGeo->dst_UV;

    if ( (pc_Sel) && (pst_SrcGeo->pst_SubObject) )
        pc_SrcSel = pst_SrcGeo->pst_SubObject->dc_VSel;
    else
        pc_SrcSel = NULL;

    /* alloc data */
    pst_TgtData = (GEO_PickMorph_Data *) L_malloc( sizeof(GEO_PickMorph_Data) * pst_TgtGeo->l_NbPoints );
    L_memset( pst_TgtData, 0, sizeof(GEO_PickMorph_Data) * pst_TgtGeo->l_NbPoints );
    pst_SrcData = (GEO_PickMorph_Data *) L_malloc( sizeof(GEO_PickMorph_Data) * pst_SrcGeo->l_NbPoints );
    L_memset( pst_SrcData, 0, sizeof(GEO_PickMorph_Data) * pst_SrcGeo->l_NbPoints );

    /* get UV used by each point */
    for (i = 0; i < pst_TgtGeo->l_NbElements; i++)
    {
        T = pst_TgtGeo->dst_Element[i].dst_Triangle;
        TLast = T + pst_TgtGeo->dst_Element[i].l_NbTriangles;
        while( T < TLast )
        {
            for ( j = 0; j < 3; j++)
            {
                D = pst_TgtData + T->auw_Index[j];
                if ( D->nbUVs == 6 ) continue;
                for ( k = 0; k < D->nbUVs; k++)
                {
                    if (D->UV[k] == T->auw_UV[j]) break;
                }
                if (k == D->nbUVs)
                    D->UV[ D->nbUVs++ ] = T->auw_UV[j];
            }
            T++;
        }
    }

    for (i = 0; i < pst_SrcGeo->l_NbElements; i++)
    {
        T = pst_SrcGeo->dst_Element[i].dst_Triangle;
        TLast = T + pst_SrcGeo->dst_Element[i].l_NbTriangles;
        while( T < TLast )
        {
            for ( j = 0; j < 3; j++)
            {
                D = pst_SrcData + T->auw_Index[j];
                if ( D->nbUVs == 6 ) continue;
                for ( k = 0; k < D->nbUVs; k++)
                {
                    if (D->UV[k] == T->auw_UV[j]) break;
                }
                if (k == D->nbUVs)
                    D->UV[ D->nbUVs++ ] = T->auw_UV[j];
            }
            T++;
        }
    }

    /* pour chaque point source trie les UV par distance */
    for (i = 0; i < pst_SrcGeo->l_NbPoints; i++)
    {
        D = pst_SrcData + i;

        if (pc_SrcSel && !(pc_SrcSel[i] & 1) )
            D->nbUVs = 0;

        for (j = 1; j < D->nbUVs; j++)
        {
            UV0 = &pst_SrcGeo->dst_UV[ D->UV[ j ] ];
            for( k = 0; k < j; k++)
            {
                UV1 = &pst_SrcGeo->dst_UV[ D->UV[ k ] ];
                if (UV0->fU > UV1->fU) continue;
                if (UV0->fU < UV1->fU) break;
                if (UV0->fV <= UV1->fV) break;
            }
            if ( k < j )
            {
                l = D->UV[j];
                for ( m = j - 1; m >= k; m--)
                    D->UV[ m + 1] = D->UV[m];
                D->UV[ k ] = l;
            }
        }
    }

    /* pour chaque point target trie les UV par distance */
    for (i = 0; i < pst_TgtGeo->l_NbPoints; i++)
    {
        D = pst_TgtData + i;

        if (pc_Sel && !(pc_Sel[i] & 1) )
            D->nbUVs = 0;

        for (j = 1; j < D->nbUVs; j++)
        {
            UV0 = &pst_TgtGeo->dst_UV[ D->UV[ j ] ];
            for( k = 0; k < j; k++)
            {
                UV1 = &pst_TgtGeo->dst_UV[ D->UV[ k ] ];
                if (UV0->fU > UV1->fU) continue;
                if (UV0->fU < UV1->fU) break;
                if (UV0->fV <= UV1->fV) break;
            }
            if ( k < j )
            {
                l = D->UV[j];
                for ( m = j - 1; m >= k; m--)
                    D->UV[ m + 1] = D->UV[m];
                D->UV[ k ] = l;
            }
        }
    }

    /* pour chaque point */
    for (D = pst_TgtData, i = 0; i < pst_TgtGeo->l_NbPoints; i++, D++)
    {
        if (D->nbUVs == 0) 
        {
            D->Point = -1;
            continue;
        }

        D->dist = GEO_PickMorph_Dist( D, pst_SrcData );
        D->Point = 0;
        for ( j = 1; j < pst_SrcGeo->l_NbPoints; j++)
        {
            if ( pst_SrcData[ j ].nbUVs == 0)
                continue;

            f = GEO_PickMorph_Dist( D, pst_SrcData + j );

            /* si meme distance on regarde la distance des points */
            if (f == D->dist)
            {
                f1 = MATH_f_Distance( pst_SrcGeo->dst_Point + j, pst_TgtGeo->dst_Point + i);
                f2 = MATH_f_Distance( pst_SrcGeo->dst_Point + D->Point, pst_TgtGeo->dst_Point + i);
                if (f1 < f2)
                    D->dist = f + 1.0f;
                //l = lAbs( j - i );
                //m = lAbs( D->Point - i );
                //if ( l < m ) D->dist = f + 1.0f;
            }

            if ( f < D->dist )
            {
                if ( (pst_SrcGeo->dst_Point[j].x * pst_TgtGeo->dst_Point[i].x) <= -Cf_EpsilonLow )
                    D->Sym = 1;
                else
                    D->Sym = 0;
                D->Point = j;
                D->dist = f;
            }
        }

        /*
        if ( ( D->Point != i) || D->Sym )
        {
            m = 0;
        }
        */
    }

    /* cleanage */
    for (i = 0; i < pst_TgtGeo->l_NbPoints; i++)
    {
        D = DSym = pst_TgtData + i;
        if (D->Point == -1) continue;
        if (D->treated) continue;

        k = D->Point;

        if (D->Sym)
            D = NULL;
        else
            DSym = NULL;

        for (j = i+1; j < pst_TgtGeo->l_NbPoints; j++)
        {
            if ( pst_TgtData[j].Point == k )
            {
                if (pst_TgtData[j].Sym == 0)
                {
                    if ( !D )
                    {
                        D = pst_TgtData + j;
                    }
                    else
                    {
                        if (D->dist <= pst_TgtData[j].dist)
                        {
                            pst_TgtData[j].Point = -1;
                        }
                        else
                        {
                            D->Point = -1;
                            D = pst_TgtData + j;
                        }
                    }
                }
                else
                {
                    if ( !DSym )
                    {
                        DSym = pst_TgtData + j;
                    }
                    else
                    {
                        if (DSym->dist <= pst_TgtData[j].dist)
                        {
                            pst_TgtData[j].Point = -1;
                        }
                        else
                        {
                            DSym->Point = -1;
                            DSym = pst_TgtData + j;
                        }
                    }
                }
            }
        }

        if (D) D->treated = 1;
        if (DSym) DSym->treated = 1;
    }

    /* morph */
    nb = 0;
    for (i = 0; i < pst_TgtGeo->l_NbPoints; i++)
    {
        D = pst_TgtData + i;
        if (D->Point == -1) continue;

        if (D->Sym)
        {
            MATH_CopyVector( &VSym, VCast(pst_SrcGeo->dst_Point + D->Point) );
            VSym.x = -VSym.x;
            if(MATH_f_Distance( VCast(pst_TgtGeo->dst_Point + i), &VSym) > Cf_Epsilon)
            {
		        //I[nb] = i;
		        MATH_SubVector(V + i, &VSym, VCast(pst_TgtGeo->dst_Point + i));
		        //nb++;
		    }
        }
        else
        {
            if(MATH_f_Distance( VCast(pst_TgtGeo->dst_Point + i), VCast(pst_SrcGeo->dst_Point + D->Point)) > Cf_Epsilon)
            {
		        //I[nb] = i;
		        MATH_SubVector(V + i, VCast(pst_SrcGeo->dst_Point + D->Point), VCast(pst_TgtGeo->dst_Point + i));
		        //nb++;
		    }
        }
    }

    return nb;
}
#ifdef JADEFUSION
void
GEO_l_ModifierMorphing_ComputeTangentSpaceBasisData( MDF_tdst_Modifier *                _pst_Mod,
                                                     GEO_tdst_ModifierMorphing_Data *   pst_MorphData, 
                                                     GEO_tdst_Object *                  pst_ObjectRef,
                                                     BOOL                               b_NormalOnly )
{
    // From the morphing data that represents all the diffs from the reference position,
    // we can reconstruct a geometric object, recompute the basis and if basis 
    // is different enough in the target from the original, compute and store difference as quaternion

    ERR_X_Assert( pst_MorphData != NULL && pst_ObjectRef != NULL && pst_ObjectRef != NULL );

    // Get reference visu
    GRO_tdst_Visu * pst_Visu = NULL;

    if(  _pst_Mod->pst_GO && _pst_Mod->pst_GO->pst_Base )
    {
        pst_Visu = _pst_Mod->pst_GO->pst_Base->pst_Visu; 
    }
    ERR_X_Assert( pst_Visu );

    // Remove original data if any
    if( pst_MorphData->dl_IndexQuat )
    {
        MEM_Free( pst_MorphData->dl_IndexQuat );
        pst_MorphData->dl_IndexQuat = NULL;
    }
    if( pst_MorphData->dst_Quaternions )
    {
        MEM_Free( pst_MorphData->dst_Quaternions );
        pst_MorphData->dst_Quaternions = NULL;
    }
    pst_MorphData->l_NbQuaternions = 0;

    // Duplicate original (temporary object)
    GEO_tdst_Object * pst_Object = (GEO_tdst_Object *) GEO_p_Duplicate( pst_ObjectRef, NULL, NULL, 0 );

    //
    // Apply position diff to obtain target final positions
    //
    for( LONG i=0; i<pst_MorphData->l_NbVector; ++i )
    {
        LONG ulIndexOfPoint = pst_MorphData->dl_Index[i];
        if( ulIndexOfPoint < pst_Object->l_NbPoints )
        {
            MATH_AddEqualVector( &pst_Object->dst_Point[ ulIndexOfPoint ], &pst_MorphData->dst_Vector[i] );
        }
    }
   
    // Alloc space for all tangent space basis morphing data
    if( pst_Object->l_NbPoints <= 0 )
    {
        return;
    }

    ERR_X_Assert( pst_Visu->p_XeElements && pst_Visu->l_NbXeElements > 0 );
    ERR_X_Assert( pst_Object->l_NbPoints == pst_ObjectRef->l_NbPoints );

    // Alloc a working buffers for elements
    MATH_tdst_Quaternion * dst_QuatBuffer = (MATH_tdst_Quaternion *) MEM_p_Alloc( pst_Object->l_NbPoints * sizeof(MATH_tdst_Quaternion) );
    L_memset( dst_QuatBuffer, 0, pst_Object->l_NbPoints * sizeof(MATH_tdst_Quaternion) );

    LONG * dst_IndexBuffer = (LONG*) MEM_p_Alloc( pst_Object->l_NbPoints * sizeof(LONG) );
    L_memset( dst_IndexBuffer, 0, pst_Object->l_NbPoints * sizeof(LONG) );

    GEO_tdst_TextureSpaceBasis * pst_TargetTangentSpace = (GEO_tdst_TextureSpaceBasis*) MEM_p_Alloc( pst_Object->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));
    L_memset( pst_TargetTangentSpace, 0, pst_Object->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));

    LONG ul_NbQuat = 0;
   
    // Compute basis for reference and target objects
    if( pst_Visu->p_XeElements[0].dst_TangentSpace == NULL )
    {
        pst_Visu->p_XeElements[0].dst_TangentSpace = (GEO_tdst_TextureSpaceBasis*) MEM_p_Alloc( pst_ObjectRef->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));
        L_memset( pst_Visu->p_XeElements[0].dst_TangentSpace, 0, pst_ObjectRef->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis));

        // Compute tangent space for all elements
        GEO_CalculateTangentSpaceBasis( pst_ObjectRef, i, pst_Visu->p_XeElements[0].dst_TangentSpace, NULL );
    }

    // Compute tangent space basis for target
    GEO_CalculateTangentSpaceBasis( pst_Object, 0, pst_TargetTangentSpace, NULL );

    // For each vertex, compute the rotation from reference to target
    for( LONG p=0; p<pst_ObjectRef->l_NbPoints; ++p )
    {
        MATH_tdst_Vector Iref, Jref, Kref;
        MATH_tdst_Quaternion quatRef, quatTarget;

        MATH_tdst_Vector oNullVector;
        MATH_InitVector( &oNullVector, 0.0f, 0.0f, 0.0f );

        if( pst_Visu->p_XeElements[0].dst_TangentSpace[ p ].Active == 0  ||
            pst_TargetTangentSpace[ p ].Active == 0 ||
            MATH_b_EqVectorWithEpsilon( &pst_Visu->p_XeElements[0].dst_TangentSpace[ p ].S, &oNullVector, 1e-3f ) ||
            MATH_b_EqVectorWithEpsilon( &pst_TargetTangentSpace[ p ].S, &oNullVector, 1e-3f ) )
        {
            continue;
        }

        const float fAngleThreshold = 0.2f;
        float fAngle = 0.0f;
        MATH_tdst_Quaternion quatDiff;
        float fDot = 0.0f;

        if( b_NormalOnly )
        {
            ERR_X_Assert( pst_ObjectRef->dst_PointNormal && pst_Object->dst_PointNormal );

            // See if normals differs enough 
            // Note: Tangent will be undefined (pixel lighting will be bad, vertex lighting will be ok)
            fDot = MATH_f_DotProduct( &pst_ObjectRef->dst_PointNormal[ p ], &pst_Object->dst_PointNormal[ p ] );
            if( fDot >= 1.0f )
            {
                fAngle = 0.0f;
            }
            else
            {
                fAngle = acos(fDot);
            }

            if( fAngle >= fAngleThreshold )
            {
                MATH_tdst_Vector vAxis;
                MATH_CrossProduct( &vAxis, &pst_ObjectRef->dst_PointNormal[ p ], &pst_Object->dst_PointNormal[ p ] );
                MATH_NormalizeAnyVector( &vAxis, &vAxis );

                // Build quaternion from axis, angle
                quatDiff.w = cos( fAngle / 2.0f );
                float sinThetaOver2 = sin( fAngle / 2.0f );
                quatDiff.x = vAxis.x * sinThetaOver2;
                quatDiff.y = vAxis.y * sinThetaOver2;
                quatDiff.z = vAxis.z * sinThetaOver2;
            }
        }
        else
        {
            // Build reference matrix
            MATH_tdst_Matrix mMatrixRef;
            Iref = pst_Visu->p_XeElements[0].dst_TangentSpace[ p ].S;
            Kref = pst_Visu->p_XeElements[0].dst_TangentSpace[ p ].SxT;
            MATH_CrossProduct( &Jref, &Kref, &Iref );
            MATH_NormalizeAnyVector( &Jref, &Jref );

            MATH_SetIdentityMatrix( &mMatrixRef );
            MATH_SetXAxis( &mMatrixRef, &Iref );
            MATH_SetYAxis( &mMatrixRef, &Jref );
            MATH_SetZAxis( &mMatrixRef, &Kref );
            MATH_SetType( &mMatrixRef, MATH_Ci_Rotation );
            MATH_ConvertMatrixToQuaternion( &quatRef, &mMatrixRef );

            // Build target matrix
            MATH_tdst_Matrix mMatrixTarget;
            MATH_tdst_Vector Itgt, Jtgt, Ktgt;
            Itgt = pst_TargetTangentSpace[ p ].S;
            Ktgt = pst_TargetTangentSpace[ p ].SxT;
            MATH_CrossProduct( &Jtgt, &Ktgt, &Itgt );
            MATH_NormalizeAnyVector( &Jtgt, &Jtgt );

            MATH_SetIdentityMatrix( &mMatrixTarget );
            MATH_SetXAxis( &mMatrixTarget, &Itgt );
            MATH_SetYAxis( &mMatrixTarget, &Jtgt );
            MATH_SetZAxis( &mMatrixTarget, &Ktgt );
            MATH_SetType( &mMatrixTarget, MATH_Ci_Rotation );
            MATH_ConvertMatrixToQuaternion( &quatTarget, &mMatrixTarget );

            // Find the quaternion to go from reference to target
            MATH_tdst_Matrix mMatrixRefInvert;
            MATH_SetIdentityMatrix( &mMatrixRefInvert );
            MATH_Invert33Matrix( &mMatrixRefInvert, &mMatrixRef );

            MATH_tdst_Matrix mMatrixRefToTarget;
            MATH_SetIdentityMatrix( &mMatrixRefToTarget );
            MATH_MulMatrixMatrix( &mMatrixRefToTarget, &mMatrixRefInvert, &mMatrixTarget );

            // Keep the quaternion diff to be morphed
            MATH_ConvertMatrixToQuaternion( &quatDiff, &mMatrixRefToTarget );

            fAngle = 2.0f * acos( quatDiff.w );
        }

        // See if the rotation is significant enough to be kept
        if( fAngle < fAngleThreshold )
        {
            // Not enough change between reference and target to consider
            continue;
        }

        // Add data to working buffer
        dst_QuatBuffer[ul_NbQuat]   = quatDiff;
        dst_IndexBuffer[ul_NbQuat]  = p;
        ++ul_NbQuat;
    }

    // If we accumulated any difference, create the data
    ERR_X_Assert(ul_NbQuat < pst_Object->l_NbPoints );

    if( ul_NbQuat == 0 )
    {
        pst_MorphData->l_NbQuaternions = 1;
    }
    else
    {
        pst_MorphData->l_NbQuaternions = ul_NbQuat;
    }

    // Allocate and fill newly created entry with basis data
    pst_MorphData->dst_Quaternions  = (MATH_tdst_Quaternion*) MEM_p_Alloc( pst_MorphData->l_NbQuaternions * sizeof(MATH_tdst_Quaternion) );
    pst_MorphData->dl_IndexQuat     = (LONG*) MEM_p_Alloc( pst_MorphData->l_NbQuaternions * sizeof(LONG) );

    if( ul_NbQuat == 0 )
    {
        MATH_InitQuaternion( &pst_MorphData->dst_Quaternions[0], 0.0f, 0.0f, 0.0f, 1.0f );
        pst_MorphData->dl_IndexQuat[0] = 0;
    }
    else
    {
        memcpy( pst_MorphData->dst_Quaternions, dst_QuatBuffer, pst_MorphData->l_NbQuaternions * sizeof(MATH_tdst_Quaternion) );
        memcpy( pst_MorphData->dl_IndexQuat, dst_IndexBuffer, pst_MorphData->l_NbQuaternions * sizeof(LONG) );
    }

    // Free working buffers
    GEO_Free( pst_Object );
    MEM_Free( dst_QuatBuffer );
    MEM_Free( dst_IndexBuffer );
    MEM_Free( pst_TargetTangentSpace );

    return;
}


void           
GEO_ModifierMorphing_RecomputeAllTangentSpaceData( MDF_tdst_Modifier *  _pst_Mod )
{
    ERR_X_Assert( _pst_Mod );

    GEO_tdst_ModifierMorphing * pst_Data = NULL;
    pst_Data = (GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data;

    GEO_tdst_ModifierMorphing_Data	* pst_MorphData = NULL;

    GEO_tdst_Object * pst_RefObj = (GEO_tdst_Object *) OBJ_p_GetGro( _pst_Mod->pst_GO );
    ERR_X_Assert( pst_RefObj );

    // We need to recompute all current tangent space data
    for( LONG i=0; i<pst_Data->l_NbMorphData; ++i )
    {
        pst_MorphData = &pst_Data->dst_MorphData[i];
        GEO_l_ModifierMorphing_ComputeTangentSpaceBasisData( _pst_Mod, pst_MorphData, pst_RefObj );
    }
}

#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_ModifierMorphing_AddData(MDF_tdst_Modifier *_pst_Mod, struct OBJ_tdst_GameObject_ *_pst_GO, LONG _l_Data, BOOL b_Sel, BOOL b_Ext )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing		*pst_Data;
	GEO_tdst_Object					*pst_RefObj, *pst_Obj;
	GEO_tdst_ModifierMorphing_Data	*pst_MorphData, *pst_SrcMorph;
	GEO_Vertex                      *pst_Ref, *pst_Tgt;
	int								i, j;
    char                            *pc_Sel;
    MATH_tdst_Vector                *V;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data;

#ifdef JADEFUSION
    pst_RefObj = (GEO_tdst_Object *) OBJ_p_GetGro( _pst_Mod->pst_GO );
    ERR_X_Assert( pst_RefObj );
#endif

    if ( !_pst_GO )
    {
        if(_l_Data >= pst_Data->l_NbMorphData) return 0;
        if(pst_Data->l_NbMorphData)
		{
			pst_Data->dst_MorphData = (GEO_tdst_ModifierMorphing_Data *) MEM_p_Realloc
				(
					pst_Data->dst_MorphData,
					(pst_Data->l_NbMorphData + 1) * sizeof(GEO_tdst_ModifierMorphing_Data)
				);
		}
		else
		{
			pst_Data->dst_MorphData = (GEO_tdst_ModifierMorphing_Data *) MEM_p_Alloc(sizeof(GEO_tdst_ModifierMorphing_Data));
		}

        pst_MorphData = pst_Data->dst_MorphData + pst_Data->l_NbMorphData++;
        pst_SrcMorph = pst_Data->dst_MorphData + _l_Data;

        L_memcpy( pst_MorphData, pst_SrcMorph, sizeof( GEO_tdst_ModifierMorphing_Data ) );
        if (pst_SrcMorph->l_NbVector)
        {
            pst_MorphData->dl_Index = (LONG*)MEM_p_Alloc(pst_SrcMorph->l_NbVector * 4);
            pst_MorphData->dst_Vector = (MATH_tdst_Vector*)MEM_p_Alloc(pst_SrcMorph->l_NbVector * sizeof(MATH_tdst_Vector));
        }
        return 1;
    }

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return 0;
	pst_Obj = (GEO_tdst_Object *) _pst_GO->pst_Base->pst_Visu->pst_Object;
	if(pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!b_Ext && (pst_Obj->l_NbPoints != pst_Data->l_NbPoints) ) return 0;

    pst_RefObj = (GEO_tdst_Object *) _pst_Mod->pst_GO->pst_Base->pst_Visu->pst_Object;
    if ( (b_Sel) && (pst_RefObj->pst_SubObject) )
        pc_Sel = pst_RefObj->pst_SubObject->dc_VSel;
    else
        pc_Sel = NULL;
    
    //I = MEM_p_Alloc(pst_Data->l_NbPoints * 4);
	V = (MATH_tdst_Vector*)MEM_p_Alloc(pst_Data->l_NbPoints * sizeof(MATH_tdst_Vector));
    L_memset( V, 0, pst_Data->l_NbPoints * sizeof(MATH_tdst_Vector));
    //j = 0;

	if(_l_Data == -1)
	{
		if(pst_Data->l_NbMorphData)
		{
			pst_Data->dst_MorphData = (GEO_tdst_ModifierMorphing_Data *) MEM_p_Realloc
				(
					pst_Data->dst_MorphData,
					(pst_Data->l_NbMorphData + 1) * sizeof(GEO_tdst_ModifierMorphing_Data)
				);
		}
		else
		{
			pst_Data->dst_MorphData = (GEO_tdst_ModifierMorphing_Data *) MEM_p_Alloc(sizeof(GEO_tdst_ModifierMorphing_Data));
		}

		pst_MorphData = pst_Data->dst_MorphData + pst_Data->l_NbMorphData;

		if(_pst_GO->sz_Name)
			sprintf(pst_MorphData->sz_Name, "%s", _pst_GO->sz_Name);
		else
			sprintf(pst_MorphData->sz_Name, "Morph Data %d", pst_Data->l_NbMorphData);
		pst_Data->l_NbMorphData++;
	}
	else
	{
		if(_l_Data >= pst_Data->l_NbMorphData) return 0;

		pst_MorphData = pst_Data->dst_MorphData + _l_Data;
        if(pst_MorphData->l_NbVector)
		{
            for (i = 0; i < pst_MorphData->l_NbVector; i++)
                MATH_CopyVector( V + pst_MorphData->dl_Index[ i ], pst_MorphData->dst_Vector + i );
            /*
            if( pc_Sel )
            {
                for ( i = 0; i < pst_MorphData->l_NbVector; i++)
                {
                    if (pc_Sel[ pst_MorphData->dl_Index[ i ] ] & 1) continue;

                    I[j] = pst_MorphData->dl_Index[ i ];
			        MATH_CopyVector(V + j, pst_MorphData->dst_Vector + i );
			        j++;
                }
            }
            */
            MEM_Free(pst_MorphData->dl_Index);
			MEM_Free(pst_MorphData->dst_Vector);
		}
	}

    if ( b_Ext )
    {
        j = GEO_l_ModifierMorphing_AddDataExt( pst_RefObj, pst_Obj, pc_Sel, V );
    }
    else
    {
	    pst_Tgt = pst_Obj->dst_Point;
	    pst_Ref = pst_RefObj->dst_Point;
	    for(i = 0; i < pst_Data->l_NbPoints; i++, pst_Ref++, pst_Tgt++)
	    {
            if (pc_Sel && !(pc_Sel[i] & 1) ) continue;

		    if(MATH_f_Distance(VCast(pst_Ref), VCast(pst_Tgt)) > Cf_Epsilon)
		    {
                MATH_SubVector( V + i, VCast(pst_Tgt), VCast(pst_Ref));
			    //I[j] = i;
			    //MATH_SubVector(V + j, VCast(pst_Tgt), VCast(pst_Ref));
			    //j++;
		    }
	    }
    }

    for (i = 0, j = 0; i < pst_Data->l_NbPoints; i++)
    {
        if ( MATH_b_NulVector( V + i ) )
            *(LONG *) (V + i) = -1;
        else
            j++;
    }

	pst_MorphData->l_NbVector = j;
	if(j)
	{
		pst_MorphData->dl_Index = (LONG*)MEM_p_Alloc( j * 4);
		pst_MorphData->dst_Vector = (MATH_tdst_Vector*)MEM_p_Alloc( j * sizeof(MATH_tdst_Vector));

        for (i = 0, j = 0; i < pst_Data->l_NbPoints; i++)
        {
            if ( *(LONG *) (V + i ) == -1 ) continue;
            MATH_CopyVector( pst_MorphData->dst_Vector + j, V + i );
            pst_MorphData->dl_Index[ j++ ] = i;
        }
	}
    else
    {
        pst_MorphData->dl_Index = NULL;
		pst_MorphData->dst_Vector = NULL;
    }
	
    MEM_Free(V);

#ifdef JADEFUSION
    // We need to compute the data tangent space basis
    GEO_l_ModifierMorphing_ComputeTangentSpaceBasisData( _pst_Mod, pst_MorphData, pst_RefObj );
#endif
	return 1;
}

#ifdef JADEFUSION
#ifdef ava
LONG GEO_l_ModifierMorphing_AddChannel(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing			*pst_Data;
	GEO_tdst_ModifierMorphing_Channel	*pst_MorphChannel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data;

	if(pst_Data->l_NbChannel)
	{
		pst_Data->dst_MorphChannel = (GEO_tdst_ModifierMorphing_Channel*)MEM_p_Realloc
			(
				pst_Data->dst_MorphChannel,
				(pst_Data->l_NbChannel + 1) * sizeof(GEO_tdst_ModifierMorphing_Channel)
			);
	}
	else
		pst_Data->dst_MorphChannel = (GEO_tdst_ModifierMorphing_Channel*)MEM_p_Alloc(sizeof(GEO_tdst_ModifierMorphing_Channel));

	pst_MorphChannel = pst_Data->dst_MorphChannel + (pst_Data->l_NbChannel++);

	pst_MorphChannel->f_Blend = 0.0f;
	pst_MorphChannel->l_NbData = 0;
	pst_MorphChannel->f_ChannelBlend = 1.0f;
	pst_MorphChannel->dl_DataIndex = NULL;
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_ModifierMorphing_AddChannelData(MDF_tdst_Modifier *_pst_Mod, int _i_Channel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing			*pst_Data;
	GEO_tdst_ModifierMorphing_Channel	*pst_MorphChannel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data;
	pst_MorphChannel = pst_Data->dst_MorphChannel + _i_Channel;

	if(pst_MorphChannel->l_NbData)
	{
		pst_MorphChannel->dl_DataIndex = (LONG*)MEM_p_Realloc
			(
				pst_MorphChannel->dl_DataIndex,
				(pst_MorphChannel->l_NbData + 1) * 4
			);
	}
	else
		pst_MorphChannel->dl_DataIndex = (LONG*)MEM_p_Alloc(4);

	pst_MorphChannel->dl_DataIndex[pst_MorphChannel->l_NbData++] = 0;
	return 1;
}
#endif
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
static LONG GEO_l_ModifierMorphing_AddChannelMorph(GEO_tdst_ModifierMorphing *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing_Channel	*pst_MorphChannel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Data->l_NbChannel)
	{
		_pst_Data->dst_MorphChannel = (GEO_tdst_ModifierMorphing_Channel*)MEM_p_Realloc
			(
				_pst_Data->dst_MorphChannel,
				(_pst_Data->l_NbChannel + 1) * sizeof(GEO_tdst_ModifierMorphing_Channel)
			);
	}
	else
		_pst_Data->dst_MorphChannel = (GEO_tdst_ModifierMorphing_Channel*)MEM_p_Alloc(sizeof(GEO_tdst_ModifierMorphing_Channel));

	pst_MorphChannel = _pst_Data->dst_MorphChannel + (_pst_Data->l_NbChannel++);

	pst_MorphChannel->f_Blend = 0.0f;
	pst_MorphChannel->l_NbData = 0;
	pst_MorphChannel->f_ChannelBlend = 1.0f;
	pst_MorphChannel->dl_DataIndex = NULL;
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static LONG GEO_l_ModifierMorphing_AddChannelDataMorph(GEO_tdst_ModifierMorphing *_pst_Data, int _i_Channel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing_Channel	*pst_MorphChannel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_MorphChannel = _pst_Data->dst_MorphChannel + _i_Channel;

	if(pst_MorphChannel->l_NbData)
	{
		pst_MorphChannel->dl_DataIndex = (LONG*)MEM_p_Realloc
			(
				pst_MorphChannel->dl_DataIndex,
				(pst_MorphChannel->l_NbData + 1) * 4
			);
	}
	else
		pst_MorphChannel->dl_DataIndex = (LONG*)MEM_p_Alloc(4);

	pst_MorphChannel->dl_DataIndex[pst_MorphChannel->l_NbData++] = 0;
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_ModifierMorphing_AddChannel(MDF_tdst_Modifier *_pst_Mod)
{
	return GEO_l_ModifierMorphing_AddChannelMorph((GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_ModifierMorphing_AddChannelData(MDF_tdst_Modifier *_pst_Mod, int _i_Channel)
{
	return GEO_l_ModifierMorphing_AddChannelDataMorph(
		(GEO_tdst_ModifierMorphing *) _pst_Mod->p_Data,
		_i_Channel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_UpdateWhenGeoChange(GEO_tdst_ModifierMorphing	*pst_Morph, WOR_tdst_Update_RLI *_pst_Upd, LONG l_NbPoints )
{
    LONG                            i, j, n, l_NbVector, l_Op, *pl_I0, *pl_I1, l_NewNbPoints;
    float                           *pf_Ratio, f_vNorm2, t;
    GEO_tdst_ModifierMorphing_Data  *pst_Data, *olddata;
    MATH_tdst_Vector                *pst_Vector, *pst_Vertex, V, Point, B, *C, *Scale, *pst_OldVertex;
	MATH_tdst_Matrix				*M;
	GEO_tdst_Object					*pst_Obj;

    if ( !pst_Morph || !pst_Morph->l_NbMorphData ) return;
    pst_Data = (GEO_tdst_ModifierMorphing_Data *) pst_Morph->dst_MorphData;
    if (!pst_Data) return;
    l_Op = _pst_Upd->l_Op & WOR_Update_RLI_OpMask;
	pst_Obj = (GEO_tdst_Object *) _pst_Upd->p_Geo;

	if (l_Op == WOR_Update_RLI_RotateVertices)
	{
		C = (MATH_tdst_Vector *) _pst_Upd->l_Ind0;
		M = (MATH_tdst_Matrix *) _pst_Upd->l_Ind1;

		for (i = 0; i < pst_Morph->l_NbMorphData; i++, pst_Data++)
		{
			for (j = 0; j < pst_Data->l_NbVector; j++)
			{
				if ( pst_Obj->pst_SubObject->dc_VSel[ pst_Data->dl_Index[ j ] ] & 1)
				{
					pst_Vertex = pst_Obj->dst_Point + pst_Data->dl_Index[ j ];
					MATH_SubVector( &Point, VCast(pst_Vertex), C);
					MATH_TransformVertex( &Point, M, &Point);

					MATH_AddVector( &V, pst_Vertex, &pst_Data->dst_Vector[ j ] );
					MATH_SubEqualVector( &V, C);
					MATH_TransformVertex( &V, M, &V);

					MATH_SubVector( &pst_Data->dst_Vector[ j ], &V, &Point );
				}
			}
		}
		return;
	}
	if (l_Op == WOR_Update_RLI_ScaleVertices )
	{
		C = (MATH_tdst_Vector *) _pst_Upd->l_Ind0;
		Scale = (MATH_tdst_Vector *) _pst_Upd->l_Ind1;

		for (i = 0; i < pst_Morph->l_NbMorphData; i++, pst_Data++)
		{
			for (j = 0; j < pst_Data->l_NbVector; j++)
			{
				if ( pst_Obj->pst_SubObject->dc_VSel[ pst_Data->dl_Index[ j ] ] & 1)
				{
					pst_Vertex = pst_Obj->dst_Point + pst_Data->dl_Index[ j ];
					MATH_SubVector(&Point, VCast(pst_Vertex), C);
					MATH_MulEqualTwoVectors(&Point, Scale);

					MATH_AddVector( &V, pst_Vertex, &pst_Data->dst_Vector[ j ] );
					MATH_SubEqualVector(&V, C);
					MATH_MulEqualTwoVectors(&V, Scale);

					MATH_SubVector( &pst_Data->dst_Vector[ j ], &V, &Point );
				}
			}
		}
		return;
	}
	if (l_Op == WOR_Update_RLI_ScaleVerticesComplex )
	{
		C = (MATH_tdst_Vector *) _pst_Upd->l_Ind0;
		Scale = (MATH_tdst_Vector *) _pst_Upd->l_Ind1;
		pst_OldVertex = (MATH_tdst_Vector *) _pst_Upd->l_Ind2;
		olddata = (GEO_tdst_ModifierMorphing_Data *) (*(LONG *) &_pst_Upd->f0);
		if (!olddata ) return;

		f_vNorm2 = MATH_f_SqrVector( Scale );
		for (i = 0; i < pst_Morph->l_NbMorphData; i++, pst_Data++, olddata++)
		{
			for (j = 0; j < pst_Data->l_NbVector; j++)
			{
				if ( pst_Obj->pst_SubObject->dc_VSel[ pst_Data->dl_Index[ j ] ] & 1)
				{
					MATH_SubVector(&Point, VCast(pst_OldVertex + pst_Data->dl_Index[ j ]), C);
					t = MATH_f_DotProduct( &Point, Scale ) / f_vNorm2;
					MATH_ScaleVector( &B, Scale, t );
					MATH_SubEqualVector( &Point, &B );
					MATH_ScaleEqualVector( &B, _pst_Upd->f_Blend );
					MATH_AddEqualVector( &Point, &B );

					MATH_AddVector( &V, VCast(pst_OldVertex + pst_Data->dl_Index[ j ]), &olddata->dst_Vector[ j ] );
					MATH_SubEqualVector(&V, C);
					t = MATH_f_DotProduct( &V, Scale ) / f_vNorm2;
					MATH_ScaleVector( &B, Scale, t );
					MATH_SubEqualVector( &V, &B );
					MATH_ScaleEqualVector( &B, _pst_Upd->f_Blend );
					MATH_AddEqualVector( &V, &B );

					MATH_SubVector( &pst_Data->dst_Vector[ j ], &V, &Point );
				}
			}
		}
		return;
	}
	
    l_NbVector = (l_NbPoints < pst_Morph->l_NbPoints) ? pst_Morph->l_NbPoints : l_NbPoints;
    pst_Vector = (MATH_tdst_Vector *) L_malloc( l_NbVector * sizeof( MATH_tdst_Vector ) );

    for (i = 0; i < pst_Morph->l_NbMorphData; i++, pst_Data++)
    {
        if (pst_Data->l_NbVector == 0) continue;
        L_memset( pst_Vector, 0, l_NbVector * sizeof( MATH_tdst_Vector ) );

        for (j = 0; j < pst_Data->l_NbVector; j++)
            MATH_CopyVector( pst_Vector + pst_Data->dl_Index[ j ], pst_Data->dst_Vector + j );

        l_NewNbPoints = l_NbPoints;

        if(l_Op == WOR_Update_RLI_Add)
	    {   
            if(_pst_Upd->l_Op & WOR_Update_RLI_Barycentre)
            {
                MATH_ScaleVector( &V, pst_Vector + _pst_Upd->l_Ind0, _pst_Upd->f0 );
                MATH_AddScaleVector( &V, &V, pst_Vector + _pst_Upd->l_Ind1, _pst_Upd->f1 );
                MATH_AddScaleVector( &V, &V, pst_Vector + _pst_Upd->l_Ind2, _pst_Upd->f2 );
            }
            else
            {
                MATH_ScaleVector( &V, pst_Vector + _pst_Upd->l_Ind0, _pst_Upd->f_Blend );
                MATH_AddScaleVector( &V, &V, pst_Vector + _pst_Upd->l_Ind1, 1 - _pst_Upd->f_Blend );
            }
            MATH_CopyVector( pst_Vector + (l_NbPoints - 1), &V );
        }
        else if (l_Op == WOR_Update_RLI_AddSome)
        {
            pl_I0 = (LONG *) _pst_Upd->l_Ind1;
            if (pl_I0)
            {
                for (j = 0; j < _pst_Upd->l_OldNbPoints; j++)
                {
                    if ( pl_I0[ j ] == -1) break;
                    MATH_CopyVector( pst_Vector + pl_I0[ j ], pst_Vector + j );
                }
            }
        }
        else if (l_Op == WOR_Update_RLI_AddSomeCenter)
        {
            pl_I0 = (LONG *) _pst_Upd->l_Ind0;
            pl_I1 = (LONG *) _pst_Upd->l_Ind1;
            pf_Ratio = (float *) _pst_Upd->l_Ind2;
            for ( j = 0; j < _pst_Upd->l_NbAdded; j++)
            {
                MATH_ScaleVector( &V, pst_Vector + pl_I0[ j ], 1.0f - pf_Ratio[ j ] );
                MATH_AddScaleVector( &V, &V, pst_Vector + pl_I1[ j ], pf_Ratio[ j ] );
                MATH_CopyVector( pst_Vector + (_pst_Upd->l_OldNbPoints + j ), &V );
            }
        }
        else if ( l_Op == WOR_Update_RLI_Del )
        {
            MATH_CopyVector( pst_Vector + _pst_Upd->l_Ind0, pst_Vector + _pst_Upd->l_Ind1 );
            if (_pst_Upd->l_Ind1 < l_NbPoints )
                MATH_InitVectorToZero( pst_Vector + _pst_Upd->l_Ind1 );
            l_NewNbPoints--;
        }
        else if (l_Op == WOR_Update_RLI_DelSome )
        {
            pl_I0 = (LONG *) _pst_Upd->l_Ind1;
            for ( j = 0; j < l_NbPoints; j++)
            {
                if(pl_I0[j] == -1) continue;
			    MATH_CopyVector( pst_Vector + pl_I0[ j ], pst_Vector + j );
            }
            l_NewNbPoints = _pst_Upd->l_Ind0;
        }


        for (n = 0, j = 0; j < l_NewNbPoints; j++)
        {
            if ( MATH_b_NulVector( pst_Vector + j ) )
                *(LONG *) (pst_Vector + j) = -1;
            else
                n++;
        }

        MEM_Free( pst_Data->dl_Index );
        MEM_Free( pst_Data->dst_Vector );
        pst_Data->l_NbVector = n;
        if (n == 0)
        {
            pst_Data->dl_Index = NULL;
            pst_Data->dst_Vector = NULL;
        }
        else
        {
            pst_Data->dl_Index = (LONG*)MEM_p_Alloc( n * sizeof( LONG ) );
            pst_Data->dst_Vector = (MATH_tdst_Vector*)MEM_p_Alloc( n * sizeof( MATH_tdst_Vector ) );
            for ( n = 0, j = 0; j < l_NewNbPoints; j++)
            {
                if ( *(LONG *) (pst_Vector + j ) == -1 ) continue;
                MATH_CopyVector( pst_Data->dst_Vector + n, pst_Vector + j );
                pst_Data->dl_Index[ n++ ] = j;
            }
        }
    }

    pst_Morph->l_NbPoints = l_NewNbPoints;
    L_free( pst_Vector );

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_ModifierMorphing_Data *GEO_ModifierMorphing_DuplicateData( GEO_tdst_ModifierMorphing *_pst_Morph )
{
    GEO_tdst_ModifierMorphing_Data *p_Ret;
    int                             i;

    if (!_pst_Morph) return NULL;
    if (!_pst_Morph->dst_MorphData ) return NULL;

    p_Ret = (GEO_tdst_ModifierMorphing_Data *) MEM_p_Alloc( _pst_Morph->l_NbMorphData * sizeof( GEO_tdst_ModifierMorphing_Data ) );

#ifdef JADEFUSION
    L_memcpy( p_Ret, _pst_Morph->dst_MorphData, _pst_Morph->l_NbMorphData * sizeof( GEO_tdst_ModifierMorphing_Data ) );
#endif

	for (i = 0; i < _pst_Morph->l_NbMorphData; i++)
    {
#ifdef JADEFUSION
        GEO_tdst_ModifierMorphing_Data * pst_MorphData = &p_Ret[i];

        if( pst_MorphData->l_NbVector)
        {
            pst_MorphData->dl_Index = (LONG*)MEM_p_Alloc( pst_MorphData->l_NbVector * sizeof( LONG ) );
            L_memcpy( pst_MorphData->dl_Index, _pst_Morph->dst_MorphData[i].dl_Index, pst_MorphData->l_NbVector * sizeof( LONG ) );
            pst_MorphData->dst_Vector = (MATH_tdst_Vector*)MEM_p_Alloc( pst_MorphData->l_NbVector * sizeof( MATH_tdst_Vector ) );
            L_memcpy(pst_MorphData->dst_Vector , _pst_Morph->dst_MorphData[i].dst_Vector, pst_MorphData->l_NbVector * sizeof( MATH_tdst_Vector ) );
        }

        if( pst_MorphData->l_NbQuaternions > 0 )
        {
            // Copy quaternions
            pst_MorphData->dst_Quaternions = (MATH_tdst_Quaternion *) MEM_p_Alloc(pst_MorphData->l_NbQuaternions * sizeof( MATH_tdst_Quaternion ) );
            L_memcpy( pst_MorphData->dst_Quaternions, _pst_Morph->dst_MorphData[i].dst_Quaternions, pst_MorphData->l_NbQuaternions * sizeof(MATH_tdst_Quaternion));

            // Copy indices
            pst_MorphData->dl_IndexQuat = (LONG *) MEM_p_Alloc( pst_MorphData->l_NbQuaternions * sizeof(LONG) );
            L_memcpy( pst_MorphData->dl_IndexQuat, _pst_Morph->dst_MorphData[i].dl_IndexQuat, pst_MorphData->l_NbQuaternions * sizeof(LONG) );
        }
#else
		L_memcpy( p_Ret + i, _pst_Morph->dst_MorphData + i, sizeof( GEO_tdst_ModifierMorphing_Data ) );
        if (p_Ret[i].l_NbVector)
        {
			p_Ret[ i ].dl_Index = ( LONG * ) MEM_p_Alloc( p_Ret[ i ].l_NbVector * sizeof( LONG ) );
            L_memcpy( p_Ret[i].dl_Index, _pst_Morph->dst_MorphData[i].dl_Index, p_Ret[i].l_NbVector * sizeof( LONG ) );
			p_Ret[ i ].dst_Vector = ( MATH_tdst_Vector * ) MEM_p_Alloc( p_Ret[ i ].l_NbVector * sizeof( MATH_tdst_Vector ) );
            L_memcpy( p_Ret[i].dst_Vector , _pst_Morph->dst_MorphData[i].dst_Vector, p_Ret[i].l_NbVector * sizeof( MATH_tdst_Vector ) );
        }
#endif
	}
    return p_Ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_DestroyData( GEO_tdst_ModifierMorphing_Data *_pst_Data, LONG _l_Nb )
{
    int i;
    for (i = 0; i < _l_Nb; i++)
    {
#ifdef JADEFUSION
		if(_pst_Data[i].l_NbVector)
        {
            MEM_Free( _pst_Data[i].dl_Index );
            MEM_Free( _pst_Data[i].dst_Vector );
        }
        if( _pst_Data[i].l_NbQuaternions > 0 )
        {
            MEM_Free( _pst_Data[i].dl_IndexQuat );
            MEM_Free( _pst_Data[i].dst_Quaternions );
        }
#else		
		if (!_pst_Data[i].l_NbVector) continue;
        MEM_Free( _pst_Data[i].dl_Index );
        MEM_Free( _pst_Data[i].dst_Vector );
#endif
	}
    MEM_Free( _pst_Data );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierMorphing_GetVertexMorphVector( GEO_tdst_ModifierMorphing *_pst_Data, LONG _i_Index, MATH_tdst_Vector *_pv_Morph )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector					V;
	float								f_ChannelBlend, f_Blend;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	GEO_tdst_ModifierMorphing_Data		*pst_Morph;
    ULONG                               *pul_Data;
	LONG								l_Data, l_End;
	int									i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (!_pv_Morph ) return;
	if (!_pst_Data ) return;

	MATH_InitVectorToZero( _pv_Morph );

	pst_Channel = _pst_Data->dst_MorphChannel;
	for(i = 0; i < _pst_Data->l_NbChannel; i++, pst_Channel++)
	{
		if(pst_Channel->l_NbData == 0) continue;
		if(pst_Channel->f_Blend == 0.0f) continue;
		if(pst_Channel->f_ChannelBlend == 0.0f) continue;
        pul_Data = (ULONG *) pst_Channel->dl_DataIndex;

		l_Data = (LONG) (pst_Channel->f_ChannelBlend);
        l_End = l_Data;
        
        while ( (l_End < pst_Channel->l_NbData) && ( pul_Data[l_End] >= (ULONG) _pst_Data->l_NbMorphData) )
            l_End++;

		if(l_End >= pst_Channel->l_NbData)
		{
			f_ChannelBlend = 1.0f;
			l_End = pst_Channel->l_NbData - 1;
            while ( (l_End > 0) && ( pul_Data[l_End] >= (ULONG) _pst_Data->l_NbMorphData) )
                l_End--;
            if (pul_Data[l_End] >= (ULONG) _pst_Data->l_NbMorphData) continue;
		}
		else
        {
            l_Data--;
             while ( (l_Data >= 0) && (pul_Data[l_Data] >= (ULONG) _pst_Data->l_NbMorphData) )
                l_Data--;
			f_ChannelBlend = (pst_Channel->f_ChannelBlend - (float) l_Data - 1) / (float)( l_End - l_Data);
        }

		if ( f_ChannelBlend > 0 )
		{
			f_Blend = pst_Channel->f_Blend * f_ChannelBlend;
            pst_Morph = _pst_Data->dst_MorphData + pul_Data[l_End];

			for ( j = 0; j < pst_Morph->l_NbVector; j++)
			{
				if (pst_Morph->dl_Index[ j ] == _i_Index)
				{
					MATH_ScaleVector(&V, pst_Morph->dst_Vector + j, f_Blend);
					MATH_AddEqualVector(_pv_Morph, &V);
					break;
				}
			}
		}

		f_ChannelBlend = 1.0f - f_ChannelBlend;
		if((l_Data >= 0) && (f_ChannelBlend > 0))
		{
			f_Blend = pst_Channel->f_Blend * f_ChannelBlend;
		    pst_Morph = _pst_Data->dst_MorphData + pul_Data[l_Data];

			for ( j = 0; j < pst_Morph->l_NbVector; j++)
			{
				if (pst_Morph->dl_Index[ j ] == _i_Index)
				{
					MATH_ScaleVector(&V, pst_Morph->dst_Vector + j, f_Blend);
					MATH_AddEqualVector(_pv_Morph, &V);
					break;
				}
			}
		}
	}
}

/*
 =======================================================================================================================
	Returns TRUE if dummy channels are at the right place with the right name
 =======================================================================================================================
 */
BOOL GEO_b_ModifierMorphing_CheckDummyChannels( GEO_tdst_ModifierMorphing *_pst_Data )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									i_ChIdx, i_DumChIdx, j;
	GEO_tdst_ModifierMorphing_Channel	*pst_Ch;
	char								*pz_DumChName;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for (i_ChIdx=0; i_ChIdx<_pst_Data->l_NbChannel; i_ChIdx++)
	{
		pst_Ch = _pst_Data->dst_MorphChannel + i_ChIdx;
		if (pst_Ch->sz_Name &&
			pst_Ch->sz_Name[0] == 'D' &&
			pst_Ch->sz_Name[1] == '_')
		{
			// This is the first dummy channel. Check that there is no other real channels after this one
			return (i_ChIdx * 4) == _pst_Data->l_NbChannel;
		}
		if (((i_ChIdx + 1) * 4) > _pst_Data->l_NbChannel)
			return FALSE;
		// Compute index where the dummy channels associated to this one should be
		i_DumChIdx = _pst_Data->l_NbChannel / 4 + 3 * i_ChIdx;
		for (j = 0; j <  3; j++, i_DumChIdx++)
		{
			pz_DumChName = ((char *)_pst_Data->dst_MorphChannel[i_DumChIdx].sz_Name) + 2;
			// Check names
			if ( (L_strlen(pz_DumChName) - L_strlen(pst_Ch->sz_Name)) != 2)
				return FALSE;
			if (pz_DumChName[L_strlen(pz_DumChName)-2] != '_' || pz_DumChName[L_strlen(pz_DumChName)-1] != ('1' + (char)j))
				return FALSE;
			if (L_memcmp(pst_Ch->sz_Name, pz_DumChName, L_strlen(pst_Ch->sz_Name)))
				return FALSE;
		}
	}
	return TRUE;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */

// Delete Channels that have a name beginning with "D_"
void GEO_ModifierMorphing_DeleteDummyChannels( GEO_tdst_ModifierMorphing *_pst_Data )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									i_ChIdx;
	GEO_tdst_ModifierMorphing_Channel	*pst_Ch;
	long								l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_ChIdx=0;
	while (i_ChIdx<_pst_Data->l_NbChannel)
	{
		pst_Ch = _pst_Data->dst_MorphChannel + i_ChIdx;
		if (pst_Ch->sz_Name &&
			pst_Ch->sz_Name[0] == 'D' &&
			pst_Ch->sz_Name[1] == '_')
		{
			if(i_ChIdx < _pst_Data->l_NbChannel - 1) {
				MEM_Free( _pst_Data->dst_MorphChannel[i_ChIdx].dl_DataIndex );
				l_Size = (_pst_Data->l_NbChannel - 1 - i_ChIdx) * sizeof(GEO_tdst_ModifierMorphing_Channel);
				memmove(_pst_Data->dst_MorphChannel + i_ChIdx, _pst_Data->dst_MorphChannel + i_ChIdx + 1, l_Size);
			}
			_pst_Data->l_NbChannel--;
		}
		else
		{
			i_ChIdx++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

// Create 3 dummy channels for each channel. The dummy channels have 2 datas in their data chain and
// their name is "D_" + the name initial channel + "1", "2" or "3".
void GEO_ModifierMorphing_CreateDummyChannels( GEO_tdst_ModifierMorphing *_pst_Data )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int									i_ChIdx, i, i_RealChNb;
	char								az_ChName[64];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GEO_ModifierMorphing_DeleteDummyChannels( _pst_Data );

	i_RealChNb = _pst_Data->l_NbChannel;
	for (i_ChIdx = 0; i_ChIdx < i_RealChNb; i_ChIdx++)
	{
		strcpy(az_ChName, _pst_Data->dst_MorphChannel[i_ChIdx].sz_Name);
		for (i=1; i<=3; i++)
		{
			GEO_l_ModifierMorphing_AddChannelMorph(_pst_Data);
			sprintf(
				_pst_Data->dst_MorphChannel[_pst_Data->l_NbChannel - 1].sz_Name ,
				"D_%s_%i", az_ChName, i);
			GEO_l_ModifierMorphing_AddChannelDataMorph(_pst_Data, _pst_Data->l_NbChannel - 1);
			GEO_l_ModifierMorphing_AddChannelDataMorph(_pst_Data, _pst_Data->l_NbChannel - 1);
		}
	}
	
}
#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_ModifierMorphing *GEO_pst_ModifierMorphing_Get(OBJ_tdst_GameObject *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = MDF_pst_GetByType(_pst_Obj, MDF_C_Modifier_Morphing);
	if((!pst_Mod) || (!pst_Mod->p_Data)) return NULL;
	return (GEO_tdst_ModifierMorphing *) pst_Mod->p_Data;
}

/*$4
 ***********************************************************************************************************************
    Store Transformed Points
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSTP_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierStoreTransformedPoint	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierStoreTransformedPoint));
	if(!p_Data)
	{
		L_memset(_pst_Mod->p_Data, 0, sizeof(GEO_tdst_ModifierStoreTransformedPoint));
		return;
	}

	L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierStoreTransformedPoint));
	pst_Data = (GEO_tdst_ModifierStoreTransformedPoint *) _pst_Mod->p_Data;

	if(pst_Data->ul_NbPoints)
		pst_Data->dst_Points = (GEO_Vertex *) MEM_p_Alloc(sizeof(GEO_Vertex) * pst_Data->ul_NbPoints);
	else
		pst_Data->dst_Points = NULL;

    pst_Data->i_PointsAreValid = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSTP_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierStoreTransformedPoint	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierStoreTransformedPoint *) _pst_Mod->p_Data;
	if(pst_Data->ul_NbPoints) MEM_Free(pst_Data->dst_Points);
	MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSTP_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierStoreTransformedPoint			*pst_Data;
 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierStoreTransformedPoint *) _pst_Mod->p_Data;

    if (pst_Data->ul_NbPoints != (ULONG) _pst_Obj->l_NbPoints )
    {
        if (pst_Data->ul_NbPoints )
            MEM_Free( pst_Data->dst_Points );
        if (_pst_Obj->l_NbPoints )
            pst_Data->dst_Points = (GEO_Vertex *) MEM_p_Alloc(sizeof(GEO_Vertex) * _pst_Obj->l_NbPoints);
        else
		    pst_Data->dst_Points = NULL;
        pst_Data->ul_NbPoints = (ULONG) _pst_Obj->l_NbPoints;
    }

    pst_Data->i_PointsAreValid = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSTP_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierStoreTransformedPoint			*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierStoreTransformedPoint *) _pst_Mod->p_Data;

    if (GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer)
    {
        L_memcpy( pst_Data->dst_Points, GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB, sizeof( GEO_Vertex ) * pst_Data->ul_NbPoints );
        pst_Data->i_PointsAreValid = 1;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_ModifierSTP_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char							        *pc_Cur;
	GEO_tdst_ModifierStoreTransformedPoint	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierStoreTransformedPoint *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	LOA_ReadLong_Ed(&pc_Cur, NULL); // skip size

	/* Number of points */
	pst_Data->ul_NbPoints = LOA_ReadULong(&pc_Cur);
    pst_Data->i_PointsAreValid = 0;

	/* Alloc buffer for points */
	if(pst_Data->ul_NbPoints)
		pst_Data->dst_Points = (GEO_Vertex *) MEM_p_Alloc(sizeof(GEO_Vertex) * pst_Data->ul_NbPoints);
	else
		pst_Data->dst_Points = NULL;
	return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierSTP_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierStoreTransformedPoint  *pst_Data;
	ULONG							        ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierStoreTransformedPoint *) _pst_Mod->p_Data;

	/* Save size */
	ul_Size = 4;
	SAV_Buffer(&ul_Size, 4);

	/* Save number of points */
	SAV_Buffer(&pst_Data->ul_NbPoints, 4);
}

#endif ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    Crush points
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierCrush_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierCrush *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierCrush));
	if(!p_Data)
	{
		L_memset(_pst_Mod->p_Data, 0, sizeof(GEO_tdst_ModifierCrush));
		return;
	}

	L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierCrush));
	pst_Data = (GEO_tdst_ModifierCrush *) _pst_Mod->p_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierCrush_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierCrush *pst_Data;
 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierCrush *) _pst_Mod->p_Data;
	MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierCrush_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierCrush *pst_Data;
	GEO_Vertex             *pst_Point, *pst_Last;
	float					f_Limit;

 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierCrush *) _pst_Mod->p_Data;

	/* Save points if needed */
	if(!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer))
	{
		GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseSpecialVertexBuffer;
		GDI_gpst_CurDD->p_Current_Vertex_List = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
		L_memcpy( GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB, _pst_Obj->dst_Point, _pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
	}

	pst_Point = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
	pst_Last = pst_Point + _pst_Obj->l_NbPoints;

	if ( pst_Data->ul_Flags & GEO_C_Crush_NegAxis )
	{
		f_Limit = pst_Data->f_Min + ( pst_Data->f_Level *( pst_Data->f_Max - pst_Data->f_Min ) );
		if (pst_Data->ul_Flags & GEO_C_Crush_Z)
		{
			for( ; pst_Point < pst_Last; pst_Point++)
			{
				if (pst_Point->z < f_Limit )
					pst_Point->z = f_Limit;
			}
		}
		else if (pst_Data->ul_Flags & GEO_C_Crush_Y)
		{
			for( ; pst_Point < pst_Last; pst_Point++)
			{
				if (pst_Point->y < f_Limit )
					pst_Point->y = f_Limit;
			}
		}
		else
		{
			for( ; pst_Point < pst_Last; pst_Point++)
			{
				if (pst_Point->x < f_Limit )
					pst_Point->x = f_Limit;
			}
		}
	}
	else
	{
		f_Limit = pst_Data->f_Max + ( pst_Data->f_Level *( pst_Data->f_Min - pst_Data->f_Max ) );
		if (pst_Data->ul_Flags & GEO_C_Crush_Z)
		{
			for( ; pst_Point < pst_Last; pst_Point++)
			{
				if (pst_Point->z > f_Limit )
					pst_Point->z = f_Limit;
			}
		}
		else if (pst_Data->ul_Flags & GEO_C_Crush_Y)
		{
			for( ; pst_Point < pst_Last; pst_Point++)
			{
				if (pst_Point->y > f_Limit )
					pst_Point->y = f_Limit;
			}
		}
		else
		{
			for( ; pst_Point < pst_Last; pst_Point++)
			{
				if (pst_Point->x > f_Limit )
					pst_Point->x = f_Limit;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierCrush_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierCrush_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierCrush *pst_Data;
 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierCrush *) _pst_Mod->p_Data;
	pst_Data->f_Level = pst_Data->f_LevelInit;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_ModifierCrush_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pc_Cur;
	GEO_tdst_ModifierCrush	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierCrush *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	LOA_ReadLong_Ed(&pc_Cur, NULL); // skip size

	/* Parameters */
	pst_Data->f_Max			= LOA_ReadFloat(&pc_Cur);
	pst_Data->f_Min			= LOA_ReadFloat(&pc_Cur);
	pst_Data->f_LevelInit	= LOA_ReadFloat(&pc_Cur);
    
	pst_Data->f_Level= pst_Data->f_LevelInit;

	return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierCrush_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierCrush	*pst_Data;
	ULONG					ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierCrush *) _pst_Mod->p_Data;

	/* Save size */
	ul_Size = 12;
	SAV_Buffer(&ul_Size, 4);

	/* Parameters */
	SAV_Buffer(&pst_Data->f_Max, 4);
	SAV_Buffer(&pst_Data->f_Min, 4);
	SAV_Buffer(&pst_Data->f_LevelInit, 4);
}

#endif ACTIVE_EDITORS


/*$4
 ***********************************************************************************************************************
    RLI Carte
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierRLICarte_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierRLICarte	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GEO_tdst_ModifierRLICarte));
	if(p_Data)
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GEO_tdst_ModifierRLICarte));
		return;
	}

	pst_Data = (GEO_tdst_ModifierRLICarte *) _pst_Mod->p_Data;
	L_memset( pst_Data, 0, sizeof(GEO_tdst_ModifierRLICarte));
	L_memset( pst_Data->aul_Color, 0xFF, sizeof( pst_Data->aul_Color ) );
	L_memset( pst_Data->aul_ColorInit, 0xFF, sizeof( pst_Data->aul_ColorInit ) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierRLICarte_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierRLICarte *pst_Data;
 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierRLICarte *) _pst_Mod->p_Data;
	if (pst_Data->duc_PtGroup )
		MEM_Free( pst_Data->duc_PtGroup );
	pst_Data->duc_PtGroup = NULL;
	if (pst_Data->dul_Colors )
		MEM_Free( pst_Data->dul_Colors );
	pst_Data->dul_Colors = NULL;
	pst_Data->ul_NbPoints = 0;
	MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS

void GEO_ModifierRLICarte_ChangeNumberOfPoints(GEO_tdst_ModifierRLICarte *_pst_Data, GEO_tdst_Object *_pst_Obj )
{
    int     n;
    char    sz_Text[1024];

	if ( !_pst_Data->ul_NbPoints ) return;
	
	n = _pst_Obj->l_NbPoints;
	if (n == 0)
	{
		if (_pst_Data->duc_PtGroup)
			MEM_Free( _pst_Data->duc_PtGroup );
		_pst_Data->duc_PtGroup = NULL;
		if ( _pst_Data->dul_Colors )
			MEM_Free( _pst_Data->dul_Colors );
		_pst_Data->dul_Colors = NULL;
	}
	else 
	{
		if (_pst_Data->duc_PtGroup )
			_pst_Data->duc_PtGroup = (UCHAR *) MEM_p_Realloc( _pst_Data->duc_PtGroup, n );
		if ( _pst_Data->dul_Colors )
			_pst_Data->dul_Colors = (ULONG *) MEM_p_Realloc( _pst_Data->dul_Colors, n * sizeof(ULONG) );
	}

    _pst_Data->ul_NbPoints = (ULONG) n;

    sprintf( sz_Text, "[Warning] Change number of point of RLICarte modifier used by (%s), you must save data", _pst_Obj->st_Id.sz_Name ? _pst_Obj->st_Id.sz_Name : "Unknown" );
    LINK_PrintStatusMsg( sz_Text );
}

#define M_4Edit_RLICarteTestNumberOfPoints( _rlicarte, _obj )\
    if ((int) _rlicarte->ul_NbPoints != _obj->l_NbPoints) GEO_ModifierRLICarte_ChangeNumberOfPoints( _rlicarte, _obj );

#else

#define M_4Edit_RLICarteTestNumberOfPoints(a, b)

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierRLICarte_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierRLICarte	*pst_Data;
	ULONG						*PC, *PCSave;
	UCHAR						*PGroup;
	OBJ_tdst_GameObject			*pst_GO;
	int							i, nb;
 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierRLICarte *) _pst_Mod->p_Data;
	pst_Data->uc_InternalFlags = 0;

	M_4Edit_RLICarteTestNumberOfPoints( pst_Data, _pst_Obj );

	/* check */
	if (!pst_Data->ul_NbPoints || !pst_Data->duc_PtGroup || !pst_Data->dul_Colors) 
		return;

	pst_GO = _pst_Mod->pst_GO;
	if (!pst_GO || !pst_GO->pst_Base || !pst_GO->pst_Base->pst_Visu || !pst_GO->pst_Base->pst_Visu->dul_VertexColors )
		return;
	if ( !OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu) )
		return;

	/* do op */
	if (pst_Data->uc_Op == GEO_C_RLIOp_None) 
		return;

	PC = pst_GO->pst_Base->pst_Visu->dul_VertexColors;
	PCSave = pst_Data->dul_Colors; 
	PGroup = pst_Data->duc_PtGroup;
	nb = (PC[0] < pst_Data->ul_NbPoints) ? PC[0] : pst_Data->ul_NbPoints;

	PC++;
	if (pst_Data->uc_Op == GEO_C_RLIOp_Set) 
	{
		pst_Data->uc_InternalFlags = 1;
		for (i = 0; i < nb; i++, PC++, PCSave++, PGroup++)
		{
			*PCSave = *PC;
			*PC = pst_Data->aul_Color[ *PGroup ];
		}
	}
	else if (pst_Data->uc_Op == GEO_C_RLIOp_Add)
	{
		pst_Data->uc_InternalFlags = 1;
		for (i = 0; i < nb; i++, PC++, PCSave++, PGroup++)
		{
			*PCSave = *PC;
			LIGHT_AddColor( PC, pst_Data->aul_Color[ *PGroup ] );
		}
	}
	else if (pst_Data->uc_Op == GEO_C_RLIOp_Sub)
	{
		pst_Data->uc_InternalFlags = 1;
		for (i = 0; i < nb; i++, PC++, PCSave++, PGroup++)
		{
			*PCSave = *PC;
			LIGHT_SubColor( PC, pst_Data->aul_Color[ *PGroup ] );
		}
	}
	
    /* Save points if needed */
	if(!(GDI_gpst_CurDD->ul_DisplayInfo & GDI_Cul_DI_UseSpecialVertexBuffer))
	{
		GDI_gpst_CurDD->ul_DisplayInfo |= GDI_Cul_DI_UseSpecialVertexBuffer;
		GDI_gpst_CurDD->p_Current_Vertex_List = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
		L_memcpy( GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB, _pst_Obj->dst_Point, _pst_Obj->l_NbPoints * sizeof( GEO_Vertex ) );
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierRLICarte_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierRLICarte	*pst_Data;
	ULONG						*PC, *PCSave;
	int							nb;
 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierRLICarte *) _pst_Mod->p_Data;
	if (!pst_Data->uc_InternalFlags)
		return;
	pst_Data->uc_InternalFlags = 0;

	PC = _pst_Mod->pst_GO->pst_Base->pst_Visu->dul_VertexColors;
	PCSave = pst_Data->dul_Colors; 
	nb = (PC[0] < pst_Data->ul_NbPoints) ? PC[0] : pst_Data->ul_NbPoints;

	PC++;
	L_memcpy( PC, PCSave, nb * sizeof(ULONG) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierRLICarte_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierRLICarte *pst_Data;
 	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierRLICarte *) _pst_Mod->p_Data;
	L_memcpy( pst_Data->aul_Color, pst_Data->aul_ColorInit, sizeof( pst_Data->aul_Color ) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_ModifierRLICarte_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*pc_Cur;
	GEO_tdst_ModifierRLICarte	*pst_Data;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierRLICarte *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	LOA_ReadLong_Ed(&pc_Cur, NULL); // skip size

	// read color value
	for (i = 0; i < 64; i++)
		pst_Data->aul_ColorInit[ i ] = LOA_ReadLong(&pc_Cur ); 

	// read flags & autres
	pst_Data->uc_Flags = LOA_ReadUChar(&pc_Cur );
	pst_Data->uc_Op = LOA_ReadUChar(&pc_Cur );
	pst_Data->uc_InternalFlags = LOA_ReadUChar(&pc_Cur );
	pst_Data->uc_Dummy2 = LOA_ReadUChar(&pc_Cur );

#if !defined(XML_CONV_TOOL)
	if (pst_Data->uc_Op > GEO_C_RLIOp_Sub)
		pst_Data->uc_Op = GEO_C_RLIOp_None;
#endif

	// read number of points
	pst_Data->ul_NbPoints = LOA_ReadULong(&pc_Cur );

	/* read point index */
	if (pst_Data->ul_NbPoints)
	{
		pst_Data->duc_PtGroup = (UCHAR *) MEM_p_Alloc( pst_Data->ul_NbPoints );
		LOA_ReadCharArray((CHAR**)&pc_Cur, (CHAR*)pst_Data->duc_PtGroup, pst_Data->ul_NbPoints);//rouna
		pst_Data->dul_Colors = (ULONG *) MEM_p_Alloc( pst_Data->ul_NbPoints * sizeof( ULONG ));
	}
	else
		pst_Data->duc_PtGroup = NULL;
		

	return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierRLICarte_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierRLICarte	*pst_Data;
	ULONG					ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GEO_tdst_ModifierRLICarte *) _pst_Mod->p_Data;

	/* Save size */
	ul_Size = sizeof( pst_Data->aul_ColorInit ) + 8 + pst_Data->ul_NbPoints;
	SAV_Buffer(&ul_Size, 4);
#ifdef JADEFUSION
	/* Parameters */
	SAV_Buffer( pst_Data->aul_ColorInit, sizeof( pst_Data->aul_ColorInit) );

	// WARNING: moved this block after the writing of the colorInit.. 
	// It the read function, it is read after, and in here, it was written before.. 
	// It did not make any sense!
#endif
	SAV_Buffer( &pst_Data->uc_Flags, 1 );
	SAV_Buffer( &pst_Data->uc_Op, 1 );
	SAV_Buffer( &pst_Data->uc_InternalFlags, 1 );
	SAV_Buffer( &pst_Data->uc_Dummy2, 1 );

#ifndef JADEFUSION
	/* Parameters */
	SAV_Buffer( pst_Data->aul_ColorInit, sizeof( pst_Data->aul_ColorInit) );
#endif
	if (pst_Data->ul_NbPoints && pst_Data->duc_PtGroup )
	{
		SAV_Buffer( &pst_Data->ul_NbPoints, 4);
		SAV_Buffer( pst_Data->duc_PtGroup, pst_Data->ul_NbPoints );
	}
	else
	{
		pst_Data->ul_NbPoints = 0;
		SAV_Buffer( &pst_Data->ul_NbPoints, 4);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ModifierRLICarte_UpdateWhenGeoChange( MDF_tdst_Modifier *_pst_Mod, WOR_tdst_Update_RLI *_pst_Upd, LONG l_NbPoints )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierRLICarte		*pst_Data;
	LONG                            j, l_Op, *pl_I0, *pl_I1;
    float                           *pf_Ratio;
	UCHAR							g, g0, g1, g2;
	GEO_tdst_Object					*pst_Geo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if( !_pst_Mod ) return;
	pst_Data = (GEO_tdst_ModifierRLICarte *) _pst_Mod->p_Data;
	if (!pst_Data ) return;

	
    l_Op = _pst_Upd->l_Op & WOR_Update_RLI_OpMask;
	pst_Geo = (GEO_tdst_Object *) _pst_Upd->p_Geo;
   
    if(l_Op == WOR_Update_RLI_Add)
    {   
		g0 = pst_Data->duc_PtGroup[ _pst_Upd->l_Ind0 ];
		g1 = pst_Data->duc_PtGroup[ _pst_Upd->l_Ind1 ];
		if(_pst_Upd->l_Op & WOR_Update_RLI_Barycentre)
        {
			g2 = pst_Data->duc_PtGroup[ _pst_Upd->l_Ind2 ];
			if ( ( g0 == g1 ) || (g0 == g2 ) )
				g = g0;
			else if (g1 == g2 )
				g = g1;
			else if ( _pst_Upd->f0 > _pst_Upd->f1 )
				g = (_pst_Upd->f0 > _pst_Upd->f2 ) ? g0 : g2;
			else
				g = ( _pst_Upd->f1 > _pst_Upd->f2 )? g1 : g2;
		}
        else
        {
			g = (_pst_Upd->f_Blend > 0.5 ) ? g0 : g1;
		}
				
		pst_Data->duc_PtGroup = ( UCHAR *) MEM_p_Realloc( pst_Data->duc_PtGroup, pst_Geo->l_NbPoints );
		pst_Data->dul_Colors = (ULONG *) MEM_p_Realloc( pst_Data->dul_Colors, pst_Geo->l_NbPoints * sizeof( ULONG ));
		pst_Data->duc_PtGroup[ pst_Geo->l_NbPoints - 1] = g;
		pst_Data->ul_NbPoints = pst_Geo->l_NbPoints;
	}
    else if (l_Op == WOR_Update_RLI_AddSome)
	{
		pst_Data->duc_PtGroup = ( UCHAR *) MEM_p_Realloc( pst_Data->duc_PtGroup, pst_Geo->l_NbPoints );
		pst_Data->dul_Colors = (ULONG *) MEM_p_Realloc( pst_Data->dul_Colors, pst_Geo->l_NbPoints * sizeof( ULONG ));
		pl_I0 = (LONG *) _pst_Upd->l_Ind1;
        if (pl_I0)
		{
    		for(j = 0; j < _pst_Upd->l_OldNbPoints; j++)
	    	{
				if ( pl_I0[ j ] == -1) break;
				pst_Data->duc_PtGroup[ pl_I0[ j ] ] = pst_Data->duc_PtGroup[ j ];
			}
		}
		pst_Data->ul_NbPoints = pst_Geo->l_NbPoints;
	}
	else if (l_Op == WOR_Update_RLI_AddSomeCenter)
    {
		pst_Data->duc_PtGroup = (UCHAR *) MEM_p_Realloc(pst_Data->duc_PtGroup, pst_Geo->l_NbPoints);
		pst_Data->dul_Colors = (ULONG *) MEM_p_Realloc( pst_Data->dul_Colors, pst_Geo->l_NbPoints * sizeof( ULONG ));
		pl_I0 = (LONG *) _pst_Upd->l_Ind0;
        pl_I1 = (LONG *) _pst_Upd->l_Ind1;
        pf_Ratio = (float *) _pst_Upd->l_Ind2;
        for ( j = 0; j < _pst_Upd->l_NbAdded; j++)
        {
			g = (pf_Ratio[ j ] < 0.5) ? pst_Data->duc_PtGroup[ pl_I0[ j ] ] : pst_Data->duc_PtGroup[ pl_I1[ j ] ];
			pst_Data->duc_PtGroup[ _pst_Upd->l_OldNbPoints + j ] = g;
		}
		pst_Data->ul_NbPoints = pst_Geo->l_NbPoints;
	}
	else if(l_Op == WOR_Update_RLI_Del)
	{
		pst_Data->duc_PtGroup[ _pst_Upd->l_Ind0 ] = pst_Data->duc_PtGroup[ _pst_Upd->l_Ind1 ];
		pst_Data->ul_NbPoints--;
	}
	else if(l_Op == WOR_Update_RLI_DelSome)
	{
		pl_I0 = (LONG *) _pst_Upd->l_Ind1;
        for ( j = 0; j < l_NbPoints; j++)
        {
			if(pl_I0[j] == -1) continue;
			pst_Data->duc_PtGroup[ pl_I0[ j ]] = pst_Data->duc_PtGroup[ j ];
		}
		pst_Data->ul_NbPoints = pst_Geo->l_NbPoints;
	}
}

#endif ACTIVE_EDITORS


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
