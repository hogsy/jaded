/*$T OBJslowaccess.c GC! 1.081 07/07/04 10:44:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* All accesses to the structures */
#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "EDIpaths.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
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
    Aim:    Get graphic object of a game object
 =======================================================================================================================
 */
void *OBJ_p_GetGro(OBJ_tdst_GameObject *_pst_Object)
{
	if(!_pst_Object) return NULL;
	if(!(_pst_Object->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return NULL;
	if(!_pst_Object->pst_Base) return NULL;
	if(!_pst_Object->pst_Base->pst_Visu) return NULL;
	return _pst_Object->pst_Base->pst_Visu->pst_Object;
}

/*
 =======================================================================================================================
    Aim:    Get graphic material of a game object
 =======================================================================================================================
 */
void *OBJ_p_GetGrm(OBJ_tdst_GameObject *_pst_Object)
{
	if(!(_pst_Object->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return NULL;
	return _pst_Object->pst_Base->pst_Visu->pst_Material;
}

/*
 =======================================================================================================================
    Aim:    Get current Geo (Gro if it's a geometric, Current LOD if gro is a static LOD, NULL otherwise)
 =======================================================================================================================
 */
void *OBJ_p_GetCurrentGeo(OBJ_tdst_GameObject *_pst_Object)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		*pst_Gro;
	GEO_tdst_StaticLOD	*pst_LOD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Object) return NULL;
	if(!(_pst_Object->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return NULL;

	pst_Gro = _pst_Object->pst_Base->pst_Visu->pst_Object;
	if(!pst_Gro) return NULL;
	if(pst_Gro->i->ul_Type == GRO_Geometric) return pst_Gro;
	if(pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
	{
		pst_LOD = (GEO_tdst_StaticLOD *) pst_Gro;
#ifdef ACTIVE_EDITORS
		pst_Gro = pst_LOD->dpst_Id[_pst_Object->ul_ForceLODIndex % pst_LOD->uc_NbLOD];
#else
		pst_Gro = (GRO_tdst_Struct *) GEO_pst_StaticLOD_GetLOD(pst_LOD, pst_LOD->uc_Distance);
#endif
		if(!pst_Gro) return NULL;
		if(pst_Gro->i->ul_Type == GRO_Geometric) return pst_Gro;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_b_IsChild(OBJ_tdst_GameObject *_pst_Object)
{
	if(!(_pst_Object->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)) return FALSE;
	if(!_pst_Object->pst_Base) return FALSE;
	if(!_pst_Object->pst_Base->pst_Hierarchy) return FALSE;
	if(!_pst_Object->pst_Base->pst_Hierarchy->pst_Father) return FALSE;
	return TRUE;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_b_AllGao_GetBV(ULONG _ul_Index, MATH_tdst_Vector *_pst_Min, MATH_tdst_Vector *_pst_Max)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHAR				*pc_Buffer;
	ULONG				ul_Id;
	ULONG				ul_Size;
	BIG_KEY				ul_Key;
	MATH_tdst_Matrix	st_Matrix;
	OBJ_tdst_GameObject *pst_Gao;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* gao already loaded */
	pst_Gao = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(_ul_Index));
	if((ULONG) pst_Gao != BIG_C_InvalidKey)
	{
		MATH_CopyVector(_pst_Min, OBJ_pst_BV_GetGMinInit(pst_Gao->pst_BV));
		MATH_CopyVector(_pst_Max, OBJ_pst_BV_GetGMaxInit(pst_Gao->pst_BV));
		return TRUE;
	}

	/* gao not loaded : load buffer, and get BV inside */
	pc_Buffer = BIG_pc_ReadFileTmp(BIG_PosFile(_ul_Index), &ul_Size);

	LOA_ReadCharArray(&pc_Buffer, (CHAR *) &ul_Key, 4);
	if(strncmp((CHAR *) &ul_Key, EDI_Csz_ExtGameObject, 4)) return FALSE;

	/* Version number : old, not used anymore */
	LOA_ReadLong_Ed(&pc_Buffer, NULL);

	/* zap editors flags */
	LOA_ReadULong(&pc_Buffer);

	/* Get identity flags */
	ul_Id = LOA_ReadULong(&pc_Buffer);

	/* zap control flag, secto, visi coef */
	LOA_ReadLong(&pc_Buffer);
	LOA_ReadUChar(&pc_Buffer);
	LOA_ReadUChar(&pc_Buffer);

	/* We skip the 2 extra bytes added for alignment */
	LOA_ReadShort_Ed(&pc_Buffer, NULL);

	/* zap LOD vis, LOD dist, design flags, fix flags */
	LOA_ReadUChar(&pc_Buffer);
	LOA_ReadUChar(&pc_Buffer);
	LOA_ReadUChar(&pc_Buffer);
	LOA_ReadChar(&pc_Buffer);

	/* Load orientation / position */
	LOA_ReadMatrix(&pc_Buffer, &st_Matrix);

	/* Load bounding volume */
	LOA_ReadLong_Ed(&pc_Buffer, NULL);	/* skip 4 dummy bytes (== 0xFFFFFFFF) */
	if(ul_Id & OBJ_C_IdentityFlag_OBBox)
	{
		LOA_ReadVector(&pc_Buffer, _pst_Min);
		LOA_ReadVector(&pc_Buffer, _pst_Max);
	}
	else
	{
		LOA_ReadVector(&pc_Buffer, _pst_Min);
		LOA_ReadVector(&pc_Buffer, _pst_Max);
	}

	return TRUE;
}

u8 OBJ_i_RLIlocation (OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	u8				res = 0;
	GRO_tdst_Struct *pst_Gro;
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	if (_pst_GO && (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) {
		if (_pst_GO->pst_Base->pst_Visu->dul_VertexColors)
			res |= 0x01;
		pst_Gro = (GRO_tdst_Struct *) OBJ_p_GetGro(_pst_GO);
		if (pst_Gro) {
			if (pst_Gro->i->ul_Type==GRO_Geometric    && ((GEO_tdst_Object *)pst_Gro)->dul_PointColors)
				res |= 0x02;
			if (pst_Gro->i->ul_Type==GRO_GeoStaticLOD && GEO_StaticLOD_HasRLI((GEO_tdst_StaticLOD *)pst_Gro))
				res |= 0x04;
		}
	}
	return res;
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_VertexColor_ChangePointer( OBJ_tdst_GameObject *_pst_GO, ULONG *_pul_New, int _i_Free )
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Visu	*pst_Visu;
#ifdef ACTIVE_EDITORS
	ULONG			ul_Index;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	
	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)) return;
	pst_Visu = _pst_GO->pst_Base->pst_Visu;
	
	if (pst_Visu->dul_VertexColors)
	{
#ifdef ACTIVE_EDITORS
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Visu->dul_VertexColors);
		LOA_DeleteAddress(pst_Visu->dul_VertexColors);
		if(ul_Index != BIG_C_InvalidIndex) 
			LOA_AddAddress(ul_Index, _pul_New);
#endif
		if (_i_Free)
			MEM_Free( pst_Visu->dul_VertexColors );
	}

	pst_Visu->dul_VertexColors = _pul_New;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG *OBJ_VertexColor_Realloc(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_NewNumber)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Visu	*pst_Visu;
	LONG			l_Size;
#ifdef ACTIVE_EDITORS
	ULONG			ul_Index;
#else	
	ULONG			ul_Pos = 0;
	extern void LOA_DeleteAddressGetPos(void *_pv_Address, ULONG*pul_Pos);
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)) return NULL;
	pst_Visu = _pst_GO->pst_Base->pst_Visu;

#ifdef ACTIVE_EDITORS
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Visu->dul_VertexColors);
	LOA_DeleteAddress(pst_Visu->dul_VertexColors);
#else
	LOA_DeleteAddressGetPos(pst_Visu->dul_VertexColors, &ul_Pos);
#endif

	l_Size = (_ul_NewNumber + 1) * sizeof(ULONG);
	pst_Visu->dul_VertexColors = (ULONG *) MEM_p_VMRealloc(pst_Visu->dul_VertexColors, l_Size);
	pst_Visu->dul_VertexColors[0] = _ul_NewNumber;

#ifdef ACTIVE_EDITORS
	if(ul_Index != BIG_C_InvalidIndex) LOA_AddAddress(ul_Index, pst_Visu->dul_VertexColors);
#else
	if(ul_Pos) 	LOA_AddPosAddress(ul_Pos, pst_Visu->dul_VertexColors);
#endif

	return pst_Visu->dul_VertexColors;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_VertexColor_Free( OBJ_tdst_GameObject *_pst_GO )
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Visu	*pst_Visu;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)) return;
	pst_Visu = _pst_GO->pst_Base->pst_Visu;
	if(!pst_Visu->dul_VertexColors)  return;
	
//#ifdef ACTIVE_EDITORS
	LOA_DeleteAddress(pst_Visu->dul_VertexColors);
//#endif

	MEM_SafeFree( pst_Visu->dul_VertexColors );
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
