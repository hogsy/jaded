/*$T MDFmodifier_GAO.c GC! 1.081 06/28/02 15:27:38 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/OBJects/OBJgrp.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_GAO.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "SOFT/SOFTstruct.h"

#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ANImation/ANIinit.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#include "SDK/sources/random/perlinnoise.h"
#define MATH_Max fMax
#define MATH_Min fMin

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern struct OBJ_tdst_GameObject_	*OBJ_pst_GroupGetByRank(OBJ_tdst_Group *, int);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

BOOL OBJ_gb_DebugPhotoMode = FALSE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL GAO_Modifier_ApplyAlways(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg)
{
	return TRUE;
}

#endif

/*$4
 ***********************************************************************************************************************
    Leg link
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLegLink_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierLegLink));

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	if(!p_Data)
	{
		((GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data)->p_GAOA = NULL;
		((GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data)->p_GAOC = NULL;
		((GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data)->f_AB = 5.0f;
		((GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data)->f_BC = 5.0f;
		((GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data)->p_GaoOrient = NULL;
		((GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data)->ul_Flags = 0;
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierLegLink));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLegLink_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierLegLink	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data;
	MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
    Aim:    Same as next function with other entries. For explanation See Next.
 =======================================================================================================================
 */
void GAO_ModifierLegLink_Compute(MATH_tdst_Matrix *MA, MATH_tdst_Matrix *MB, MATH_tdst_Matrix *MC, float AB, float BC, MATH_tdst_Vector *_pst_BAxis )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*A, *B, *C, *D, U, V, W, U0;
	MATH_tdst_Vector	DD, XRef;
	float				l0, l1, H, h, n;
	MATH_tdst_Matrix	*M;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	A = &MA->T;
	B = &MB->T;
	C = &MC->T;
	
	MATH_NegVector(&DD, _pst_BAxis ? _pst_BAxis : MATH_pst_GetZAxis(MA));
	D = &DD;

	l0 = AB;
	l1 = BC;
	H = MATH_f_Distance(A, C);
	if ( H > l0 + l1 )
	{
		MATH_SubVector( &XRef, C, A );
		MATH_ScaleEqualVector( &XRef, ((l0 + l1) * 0.99f) / H );
		MATH_AddVector( C, A, &XRef );
	}

	h = ((l0 * l0) - (l1 * l1) + (H * H)) / (2 * H);
	n = fOptSqrt((l0 * l0) - (h * h));

	MATH_SubVector(&V, C, A);
	MATH_NormalizeEqualVector(&V);
	MATH_ScaleEqualVector(&V, h);

	MATH_CrossProduct(&W, D, &V);

	MATH_CrossProduct(&U, &V, &W);
	MATH_NormalizeEqualVector(&U);
	MATH_ScaleEqualVector(&U, n);

	MATH_AddVector(B, &V, A);
	MATH_SubEqualVector(B, &U);
	
	MATH_NegVector( &XRef, &U );

	/* set rotation matrix of leg */
	M = MA;
	MATH_SubVector(&U, B, A);
	MATH_NormalizeVector(MATH_pst_GetZAxis(M), &U);
	
	if ( _pst_BAxis )
	{
		MATH_CrossProduct(&U0, &XRef, &U);
		if ( MATH_b_NulVector( &U0 ) )
			MATH_CrossProduct(&U0, _pst_BAxis, &U);
	}
	else
	{
		MATH_CrossProduct(&U0, MATH_pst_GetYAxis(M), &U);
	}
	MATH_NormalizeVector(MATH_pst_GetXAxis(M), &U0);
	MATH_CrossProduct(MATH_pst_GetYAxis(M), MATH_pst_GetZAxis(M), MATH_pst_GetXAxis(M));

	/* set rotation matrix of knee */
	M = MB;
	MATH_SubVector(&U, C, B);
	MATH_NormalizeVector(MATH_pst_GetZAxis(M), &U);
	
	if ( _pst_BAxis )
	{
		MATH_CrossProduct(&U0, &XRef, &U);
		if ( MATH_b_NulVector( &U0 ) )
			MATH_CrossProduct(&U0, _pst_BAxis, &U);
		if ( h > H )
			MATH_NegEqualVector( &U0 );
	}
	else
	{
		//MATH_CrossProduct(&U0, MATH_pst_GetYAxis(M), &U);
		MATH_CrossProduct(&U0, MATH_pst_GetYAxis(MA), &U);
	}
	MATH_NormalizeVector(MATH_pst_GetXAxis(M), &U0);
	MATH_CrossProduct(MATH_pst_GetYAxis(M), MATH_pst_GetZAxis(M), MATH_pst_GetXAxis(M));

}

/*$F
 =======================================================================================================================
	Aim:    A Cuisse, B Genou, C Pied. On met de l'IK sur le Pied, La cuisse bouge, le Pied aussi et on 
			cherche a calculer la position du genou, ainsi que les nouvelles orientations de A et de B.

			Les seules donnees utilisees sont:
			- les position de la cuisse et du pied.
			- les longueurs des 2 os concernes.
			- Le vecteur Z de la cuisse. (Calcul Position B)	----> (!! PROBLEME DU CHOIX DE LA POSITION DE B !!)
			- Le vecteur Y de la cuisse. (Calcul Orientation Finale de A)

    Note:   l0 - Distance de A a B
			l1 - Distance de B a C
			H  - Distance de A a C.
			n  - Distance de B a la droite (AB)
			h  - Distance de A au projete de B sur (AB)

 =======================================================================================================================
 */

void GAO_ModifierLegLink_DoIt(GAO_tdst_ModifierLegLink *_pst_Data, OBJ_tdst_GameObject *_pst_GAOB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*A, *B, *C, *D, U, V, W, U0;
	float				l0, l1, H, h, n;
	MATH_tdst_Matrix	*M;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Data) return;
	if(!_pst_Data->p_GAOA) return;
	if(!_pst_Data->p_GAOC) return;
	if(!_pst_GAOB) return;

	/*
	 * OBJ_ComputeGlobalWithLocal((OBJ_tdst_GameObject *) _pst_Data->p_GAOA,
	 * OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject *) _pst_Data->p_GAOA), 1);
	 * OBJ_ComputeGlobalWithLocal((OBJ_tdst_GameObject *) _pst_GAOB,
	 * OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject *) _pst_GAOB), 1);
	 * OBJ_ComputeGlobalWithLocal((OBJ_tdst_GameObject *) _pst_Data->p_GAOC,
	 * OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject *) _pst_Data->p_GAOC), 1);
	 */
	OBJ_ComputeGlobalWhenHie((OBJ_tdst_GameObject *) _pst_Data->p_GAOA);
	OBJ_ComputeGlobalWhenHie((OBJ_tdst_GameObject *) _pst_GAOB);
	OBJ_ComputeGlobalWhenHie((OBJ_tdst_GameObject *) _pst_Data->p_GAOC);

	A = OBJ_pst_GetAbsolutePosition((OBJ_tdst_GameObject *) _pst_Data->p_GAOA);
	B = OBJ_pst_GetAbsolutePosition(_pst_GAOB);
	C = OBJ_pst_GetAbsolutePosition((OBJ_tdst_GameObject *) _pst_Data->p_GAOC);

//	pst_GO =  (OBJ_tdst_GameObject *)((_pst_Data->p_GaoOrient == NULL) ? ((OBJ_tdst_GameObject *) _pst_Data->p_GAOA) : _pst_Data->p_GaoOrient;
	pst_GO = (OBJ_tdst_GameObject *)((_pst_Data->p_GaoOrient == NULL) ? ((OBJ_tdst_GameObject *) _pst_Data->p_GAOA) : _pst_Data->p_GaoOrient);
	if(_pst_Data->ul_Flags & MFG_C_LegLink_OrientX)
		D = MATH_pst_GetXAxis(OBJ_pst_GetAbsoluteMatrix(pst_GO));
	else if(_pst_Data->ul_Flags & MFG_C_LegLink_OrientY)
		D = MATH_pst_GetYAxis(OBJ_pst_GetAbsoluteMatrix(pst_GO));
	else
		D = MATH_pst_GetZAxis(OBJ_pst_GetAbsoluteMatrix(pst_GO));

	/*
	 * D = MATH_pst_GetZAxis(OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject *)
	 * _pst_Data->p_GAOA)); £
	 * F { char sz_Text[200]; sprintf( sz_Text, "IK : A(%.2f) B(%.2f) C(%.2f)\n",
	 * A->z, B->z, C->z ); LINK_PrintStatusMsg( sz_Text ); }
	 */
	H = MATH_f_Distance(A, C);
	l0 = _pst_Data->f_AB;
	l1 = _pst_Data->f_BC;
	h = ((l0 * l0) - (l1 * l1) + (H * H)) / (2 * H);
	n = fOptSqrt((l0 * l0) - (h * h));

	MATH_SubVector(&V, C, A);
	MATH_NormalizeEqualVector(&V);
	MATH_ScaleEqualVector(&V, h);

	if(_pst_Data->ul_Flags & MFG_C_LegLink_OrientNeg)
		MATH_CrossProduct(&W, &V, D);
	else
		MATH_CrossProduct(&W, D, &V);

	MATH_CrossProduct(&U, &V, &W);
	MATH_NormalizeEqualVector(&U);
	MATH_ScaleEqualVector(&U, n);

	/* set position of knee */
	MATH_AddVector(B, &V, A);
	MATH_AddEqualVector(B, &U);

	/*$F
    {
        char sz_Text[200];
        sprintf( sz_Text, "B(%.2f)", B->z );
        LINK_PrintStatusMsg( sz_Text );
    }
    */

	/* set rotation matrix of leg */
	pst_GO = (OBJ_tdst_GameObject *) _pst_Data->p_GAOA;
	M = OBJ_pst_GetAbsoluteMatrix(pst_GO);
	MATH_SubVector(&U, B, A);
	MATH_CrossProduct(&U0, MATH_pst_GetYAxis(M), &U);
	MATH_NormalizeVector(MATH_pst_GetXAxis(M), &U0);
	MATH_NormalizeVector(MATH_pst_GetZAxis(M), &U);
	MATH_CrossProduct(MATH_pst_GetYAxis(M), MATH_pst_GetZAxis(M), MATH_pst_GetXAxis(M));
	OBJ_ComputeLocalWhenHie(pst_GO);

	/* set rotation matrix of knee */
	pst_GO = _pst_GAOB;
	M = OBJ_pst_GetAbsoluteMatrix(pst_GO);
	MATH_SubVector(&U, C, B);
	MATH_NormalizeVector(MATH_pst_GetZAxis(M), &U);
	MATH_CrossProduct(&U0, MATH_pst_GetYAxis(M), &U);
	MATH_NormalizeVector(MATH_pst_GetXAxis(M), &U0);
	MATH_CrossProduct(MATH_pst_GetYAxis(M), MATH_pst_GetZAxis(M), MATH_pst_GetXAxis(M));
	OBJ_ComputeLocalWhenHie(pst_GO);

	pst_GO = (OBJ_tdst_GameObject *) _pst_Data->p_GAOC;
	OBJ_ComputeLocalWhenHie(pst_GO);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLegLink_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group				*pst_Group;
	GAO_tdst_ModifierLegLink	*pst_Data;
	int							GAOA, GAOC, GAOO;
	MATH_tdst_Vector			st_Tmp;
#ifdef ACTIVE_EDITORS
	BIG_KEY						ul_Key;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data;

	GAOA = (int) pst_Data->p_GAOA;
	GAOC = (int) pst_Data->p_GAOC;
	GAOO = (int) pst_Data->p_GaoOrient;

	pst_Group = MDF_Modifier_GetCurGroup(_pst_Mod);
	if(pst_Group)
	{
		pst_Data->p_GAOA = OBJ_pst_GroupGetByRank(pst_Group, (int) pst_Data->p_GAOA);
		pst_Data->p_GAOC = OBJ_pst_GroupGetByRank(pst_Group, (int) pst_Data->p_GAOC);
		pst_Data->p_GaoOrient = OBJ_pst_GroupGetByRank(pst_Group, (int) pst_Data->p_GaoOrient);
	}

#ifdef ACTIVE_EDITORS
	if(!(pst_Data->ul_Flags & MFG_C_LegLink_InGroup))
	{
		if(pst_Data->p_GAOA)
		{
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GAOA);
			if(ul_Key == BIG_C_InvalidKey) pst_Data->p_GAOA = NULL;
		}

		if(pst_Data->p_GAOC)
		{
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GAOC);
			if(ul_Key == BIG_C_InvalidKey) pst_Data->p_GAOC = NULL;
		}

		if(pst_Data->p_GaoOrient)
		{
			ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GaoOrient);
			if(ul_Key == BIG_C_InvalidKey) pst_Data->p_GaoOrient = NULL;
		}
	}

#endif
	/* Compute length of bones depending of init positions */
	if(!(pst_Data->ul_Flags & MFG_C_LegLink_InGroup) || pst_Group)
	{
		if((pst_Data->ul_Flags & MFG_C_LegLink_LengthInit) && !(pst_Data->ul_Flags & MFG_C_LegLink_LengthComputed))
		{
			pst_Data->ul_Flags |= MFG_C_LegLink_LengthComputed;

			if
			(
				(pst_Data->p_GAOA && pst_Data->p_GAOC)
			&&	(OBJ_b_TestIdentityFlag((OBJ_tdst_GameObject*)pst_Data->p_GAOA, OBJ_C_IdentityFlag_HasInitialPos))
			&&	(OBJ_b_TestIdentityFlag(GDI_gpst_CurDD->pst_CurrentGameObject, OBJ_C_IdentityFlag_HasInitialPos))
			&&	(OBJ_b_TestIdentityFlag((OBJ_tdst_GameObject*)pst_Data->p_GAOC, OBJ_C_IdentityFlag_HasInitialPos))
			)
			{
				MATH_SubVector
				(
					&st_Tmp,
					OBJ_pst_GetInitialAbsolutePosition((OBJ_tdst_GameObject*)pst_Data->p_GAOA),
					OBJ_pst_GetInitialAbsolutePosition(GDI_gpst_CurDD->pst_CurrentGameObject)
				);
				pst_Data->f_AB = MATH_f_NormVector(&st_Tmp);
				MATH_SubVector
				(
					&st_Tmp,
					OBJ_pst_GetInitialAbsolutePosition((OBJ_tdst_GameObject*)pst_Data->p_GAOC),
					OBJ_pst_GetInitialAbsolutePosition(GDI_gpst_CurDD->pst_CurrentGameObject)
				);
				pst_Data->f_BC = MATH_f_NormVector(&st_Tmp);
			}
		}
	}

	if(!(pst_Data->ul_Flags & MFG_C_LegLink_InGroup) || pst_Group)
	{
		GAO_ModifierLegLink_DoIt((GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data, GDI_gpst_CurDD->pst_CurrentGameObject);
	}

	pst_Data->p_GAOA = (OBJ_tdst_GameObject *) GAOA;
	pst_Data->p_GAOC = (OBJ_tdst_GameObject *) GAOC;
	pst_Data->p_GaoOrient = (OBJ_tdst_GameObject *) GAOO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLegLink_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ModifierLegLink_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*pc_Cur;
	GAO_tdst_ModifierLegLink	*pst_Data;
	ULONG						ul_Size;
	ULONG						ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	pst_Data = (GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	ul_Size = LOA_ReadULong(&pc_Cur);

	/* GAO ou index */
	ul_Key = LOA_ReadULong(&pc_Cur);
	pst_Data->p_GAOA = (OBJ_tdst_GameObject *) ul_Key;

	/* GAO ou index */
	ul_Key = LOA_ReadULong(&pc_Cur);
	pst_Data->p_GAOC = (OBJ_tdst_GameObject *) ul_Key;

	pst_Data->f_AB = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_BC = LOA_ReadFloat(&pc_Cur);

	/* GAO ou index */
	ul_Key = LOA_ReadULong(&pc_Cur);
	pst_Data->p_GaoOrient = (OBJ_tdst_GameObject *) ul_Key;

	/* Flags */
	pst_Data->ul_Flags = LOA_ReadULong(&pc_Cur);
	pst_Data->ul_Flags &= ~MFG_C_LegLink_LengthComputed;

	if(!(pst_Data->ul_Flags & MFG_C_LegLink_InGroup))
	{
		if((ULONG) pst_Data->p_GAOA && (ULONG) pst_Data->p_GAOA != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef
			(
				(ULONG) pst_Data->p_GAOA,
				(ULONG *) &pst_Data->p_GAOA,
				OBJ_ul_GameObjectCallback,
				LOA_C_MustExists
			);
		}
		else
			pst_Data->p_GAOA = NULL;

		if((ULONG) pst_Data->p_GAOC && (ULONG) pst_Data->p_GAOC != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef
			(
				(ULONG) pst_Data->p_GAOC,
				(ULONG *) &pst_Data->p_GAOC,
				OBJ_ul_GameObjectCallback,
				LOA_C_MustExists
			);
		}
		else
			pst_Data->p_GAOC = NULL;

		if((ULONG) pst_Data->p_GaoOrient && (ULONG) pst_Data->p_GaoOrient != BIG_C_InvalidKey)
		{
			LOA_MakeFileRef
			(
				(ULONG) pst_Data->p_GaoOrient,
				(ULONG *) &pst_Data->p_GaoOrient,
				OBJ_ul_GameObjectCallback,
				LOA_C_MustExists
			);
		}
		else
			pst_Data->p_GaoOrient = NULL;
	}

	return(pc_Cur - _pc_Buffer);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLegLink_Reinit(MDF_tdst_Modifier *_pt_Mdf)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierLegLink	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierLegLink *) _pt_Mdf->p_Data;
	pst_Data->ul_Flags &= ~MFG_C_LegLink_LengthComputed;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLegLink_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierLegLink	*pst_Data;
	ULONG						ul_Size;
	ULONG						ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierLegLink *) _pst_Mod->p_Data;

	ul_Size = 28;
	SAV_Buffer(&ul_Size, 4);

	if(pst_Data->ul_Flags & MFG_C_LegLink_InGroup)
		ul_Key = (ULONG) pst_Data->p_GAOA;
	else if(pst_Data->p_GAOA)
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GAOA);
	else
		ul_Key = BIG_C_InvalidKey;
	SAV_Buffer(&ul_Key, 4);

	if(pst_Data->ul_Flags & MFG_C_LegLink_InGroup)
		ul_Key = (ULONG) pst_Data->p_GAOC;
	else if(pst_Data->p_GAOC)
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GAOC);
	else
		ul_Key = BIG_C_InvalidKey;
	SAV_Buffer(&ul_Key, 4);

	SAV_Buffer(&pst_Data->f_AB, 4);
	SAV_Buffer(&pst_Data->f_BC, 4);

	if(pst_Data->ul_Flags & MFG_C_LegLink_InGroup)
		ul_Key = (ULONG) pst_Data->p_GaoOrient;
	else if(pst_Data->p_GaoOrient)
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GaoOrient);
	else
		ul_Key = BIG_C_InvalidKey;
	SAV_Buffer(&ul_Key, 4);

	SAV_Buffer(&pst_Data->ul_Flags, 4);
}

#endif

/*$4
 ***********************************************************************************************************************
    Semi look at:: look at camera but Z axis of object is always Z axis of world
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSemiLookAt_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	_pst_Mod->p_Data = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSemiLookAt_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSemiLookAt_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	V;
	MATH_tdst_Matrix	*M;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = GDI_gpst_CurDD->pst_CurrentGameObject;
	M = OBJ_pst_GetAbsoluteMatrix(pst_GO);

	MATH_SubVector(&V, OBJ_pst_GetAbsolutePosition(pst_GO), &GDI_gpst_CurDD->st_Camera.st_Matrix.T);

	MATH_CrossProduct(MATH_pst_GetXAxis(M), &V, MATH_pst_GetZAxis(M));
	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(M));
	MATH_CrossProduct(MATH_pst_GetYAxis(M), MATH_pst_GetZAxis(M), MATH_pst_GetXAxis(M));

	OBJ_ComputeLocalWhenHie(pst_GO);

	/*$F old
    V.z = 0;
	MATH_NormalizeEqualVector(&V);
	MATH_CrossProduct(MATH_pst_GetXAxis(M), &V, &MATH_gst_BaseVectorK);
	MATH_CopyVector(MATH_pst_GetYAxis(M), &V);
	MATH_CopyVector(MATH_pst_GetZAxis(M), &MATH_gst_BaseVectorK);
    */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSemiLookAt_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSemiLookAt_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ModifierSemiLookAt_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~*/
	char	*pc_Cur;
	/*~~~~~~~~~~~~*/

	pc_Cur = _pc_Buffer;

#if !defined(XML_CONV_TOOL)
	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;
#endif
	LOA_ReadLong_Ed(&pc_Cur, NULL); /* skip size */
	return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSemiLookAt_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~*/
	ULONG	ul_Size;
	/*~~~~~~~~~~~~*/

	ul_Size = 0;
	SAV_Buffer(&ul_Size, 4);
}

#endif

/*$4
 ***********************************************************************************************************************
    Semi look at:: look at camera but Z axis of object is always Z axis of world
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSpecialLookAt_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierSpecialLookAt));

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	if(!p_Data)
	{
		((GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data)->c_Type = MDF_SpecialLookAt_Object;
		((GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data)->c_Dummy[0] = 0;
		((GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data)->c_Dummy[1] = 0;
		((GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data)->c_Dummy[2] = 0;
		((GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data)->f_Z = 5.0f;
		((GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data)->f_Value1 = 5.0f;
		((GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data)->f_Value2 = 5.0f;
		((GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data)->i_GaoRank = -1;
		((GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data)->p_GAO = NULL;
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierSpecialLookAt));
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSpecialLookAt_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	MEM_Free(_pst_Mod->p_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSpecialLookAt_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*pst_GO;
	MATH_tdst_Vector				*P, *C, v2D, v3D;
	MATH_tdst_Matrix				*M, *Cam;
	GAO_tdst_ModifierSpecialLookAt	*pst_Data;
	float							t, r;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data;

	pst_GO = GDI_gpst_CurDD->pst_CurrentGameObject;
	M = OBJ_pst_GetAbsoluteMatrix(pst_GO);
	Cam = &GDI_gpst_CurDD->st_Camera.st_Matrix;

	P = OBJ_pst_GetAbsolutePosition(pst_GO);

	if(pst_Data->i_GaoRank != -1)
		pst_Data->p_GAO = OBJ_pst_GroupGetByRank(MDF_Modifier_GetCurGroup(_pst_Mod), pst_Data->i_GaoRank);

	if(pst_Data->c_Type == MDF_SpecialLookAt_FullScreen)
	{
		MATH_CopyVector(P, &Cam->T);
		MATH_AddScaleVector(P, P, MATH_pst_GetZAxis(Cam), pst_Data->f_Z);

		MATH_CopyVector(MATH_pst_GetXAxis(M), MATH_pst_GetXAxis(Cam));
		MATH_NegVector(MATH_pst_GetYAxis(M), MATH_pst_GetZAxis(Cam));
		MATH_NegVector(MATH_pst_GetZAxis(M), MATH_pst_GetYAxis(Cam));

		MATH_InitVector(&v2D, 0, 0, pst_Data->f_Z);
		CAM_2Dto3DCamera2(&GDI_gpst_CurDD->st_Camera, &v3D, &v2D);

		//P = OBJ_pst_BV_GetLMin(pst_GO->pst_BV);
		P = OBJ_pst_BV_GetGMin(pst_GO->pst_BV);

		MATH_SetScaleType(M);
		M->Sx = v3D.x / P->x;
		M->Sy = 1;
		M->Sz = -(v3D.y / P->z);
	}
	else if(pst_Data->c_Type == MDF_SpecialLookAt_PlanetAura)
	{
		if(pst_Data->p_GAO == NULL) return;
		C = OBJ_pst_GetAbsolutePosition((OBJ_tdst_GameObject *) pst_Data->p_GAO);
		MATH_SubVector(&v3D, &Cam->T, C);
		t = fSqr(pst_Data->f_Z) / MATH_f_SqrVector(&v3D);

		MATH_AddScaleVector(P, C, &v3D, t);
		r = fOptSqrt(fSqr(pst_Data->f_Z) - fSqr(MATH_f_Distance(P, C)));

		MATH_NormalizeVector(MATH_pst_GetYAxis(M), &v3D);
		MATH_NegEqualVector(MATH_pst_GetYAxis(M));
		MATH_CrossProduct(MATH_pst_GetZAxis(M), MATH_pst_GetXAxis(Cam), MATH_pst_GetYAxis(M));
		MATH_NormalizeEqualVector(MATH_pst_GetZAxis(M));
		MATH_CrossProduct(MATH_pst_GetXAxis(M), MATH_pst_GetYAxis(M), MATH_pst_GetZAxis(M));
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			float			f_ExternRadius;
			float			angle, dangle;
			GEO_Vertex		*P;
			GEO_tdst_Object *_pst_Obj;
			int				index;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			_pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
			if(_pst_Obj)
			{
				f_ExternRadius = 100.0f * (r + pst_Data->f_Value1) / r;
				P = _pst_Obj->dst_Point;

				dangle = Cf_2Pi * 2 / _pst_Obj->l_NbPoints;
				for(angle = 0, index = 0; index < _pst_Obj->l_NbPoints; index += 2, P += 2, angle += dangle)
				{
					P->x = fSin(angle) * f_ExternRadius;
					P->y = -pst_Data->f_Value2;
					P->z = fCos(angle) * f_ExternRadius;
				}
			}
		}

		MATH_SetScaleType(M);
		M->Sx = M->Sz = r / 100;
		M->Sy = 1;
	}
	else if(pst_Data->c_Type == MDF_SpecialLookAt_Object)
	{
		if(pst_Data->p_GAO == NULL) return;
		C = OBJ_pst_GetAbsolutePosition((OBJ_tdst_GameObject *) pst_Data->p_GAO);
		MATH_SubVector(&v3D, &M->T, C);

		MATH_NormalizeVector(MATH_pst_GetYAxis(M), &v3D);
		MATH_CrossProduct(MATH_pst_GetZAxis(M), MATH_pst_GetXAxis(Cam), MATH_pst_GetYAxis(M));
		MATH_NormalizeEqualVector(MATH_pst_GetZAxis(M));
		MATH_CrossProduct(MATH_pst_GetXAxis(M), MATH_pst_GetYAxis(M), MATH_pst_GetZAxis(M));

		OBJ_ComputeLocalWhenHie(pst_GO);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSpecialLookAt_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSpecialLookAt_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ModifierSpecialLookAt_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char							*pc_Cur;
	GAO_tdst_ModifierSpecialLookAt	*pst_Data;
	ULONG							ul_Key;
	ULONG							ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	pst_Data = (GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	ul_Size = LOA_ReadULong(&pc_Cur);

	/* *(LONG *) &pst_Data->c_Type = LOA_ReadLong(&pc_Cur); */
	pst_Data->c_Type = LOA_ReadChar(&pc_Cur);
	pst_Data->c_Dummy[0] = LOA_ReadChar(&pc_Cur);
	pst_Data->c_Dummy[1] = LOA_ReadChar(&pc_Cur);
	pst_Data->c_Dummy[2] = LOA_ReadChar(&pc_Cur);

	pst_Data->f_Z = LOA_ReadFloat(&pc_Cur);

	if(ul_Size == 28)
	{
		pst_Data->i_GaoRank = LOA_ReadInt(&pc_Cur);
	}
	else
		pst_Data->i_GaoRank = -1;

	ul_Key = LOA_ReadLong(&pc_Cur);

	if((pst_Data->i_GaoRank == -1) && (ul_Key != 0) && (ul_Key != BIG_C_InvalidKey))
		LOA_MakeFileRef(ul_Key, (ULONG *) &pst_Data->p_GAO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
	else
		pst_Data->p_GAO = NULL;

	if(ul_Size == 4)
	{
		pst_Data->f_Value1 = 1000;
		pst_Data->f_Value2 = 1000;
	}
	else
	{
		pst_Data->f_Value1 = LOA_ReadFloat(&pc_Cur);
		pst_Data->f_Value2 = LOA_ReadFloat(&pc_Cur);
	}

	return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSpecialLookAt_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							ul_Size, ul_Key;
	GAO_tdst_ModifierSpecialLookAt	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierSpecialLookAt *) _pst_Mod->p_Data;

	ul_Size = 28;
	SAV_Buffer(&ul_Size, 4);
	SAV_Buffer(&pst_Data->c_Type, 4);
	SAV_Buffer(&pst_Data->f_Z, 4);

	SAV_Buffer(&pst_Data->i_GaoRank, 4);

	if((pst_Data->i_GaoRank == -1) && (pst_Data->p_GAO))
		ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->p_GAO);
	else
		ul_Key = BIG_C_InvalidKey;
	SAV_Buffer(&ul_Key, 4);

	SAV_Buffer(&pst_Data->f_Value1, 4);
	SAV_Buffer(&pst_Data->f_Value2, 4);
}

#endif

/*$4
 ***********************************************************************************************************************
    Explode:: All face are separated
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierExplode_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
#if !defined(XML_CONV_TOOL)
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object						*pst_Obj;
	GAO_tdst_ModifierExplode			*pst_Data;
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	int									i_Elem, i_Triangle, i_Point;
	GEO_tdst_IndexedTriangle			*t;
	GEO_Vertex							*p;
	MATH_tdst_Vector					*n, N;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetGro(_pst_GO);
	if((!pst_Obj) || (pst_Obj->st_Id.i->ul_Type != GRO_Geometric))
	{
		_pst_Mod->p_Data = NULL;
		return;
	}

	pst_Data = (GAO_tdst_ModifierExplode *) MEM_p_Alloc(sizeof(GAO_tdst_ModifierExplode));
	pst_Data->l_NbTriangles = GEO_l_GetNumberOfTriangles(pst_Obj);
	pst_Data->l_NbPoints = pst_Data->l_NbTriangles * 3;
	pst_Data->l_NbElements = pst_Obj->l_NbElements;
	pst_Data->f_Time = 0;
	pst_Data->f_Grav = -0.5f;
	pst_Data->f_Mul = 1;

	pst_Data->pst_Obj = GEO_pst_Create(pst_Data->l_NbPoints, 0, pst_Data->l_NbElements, 0);
	pst_Data->pst_Obj->l_NbUVs = pst_Obj->l_NbUVs;
	pst_Data->pst_Obj->dst_UV = pst_Obj->dst_UV;

	i_Point = 0;
	p = pst_Data->pst_Obj->dst_Point;
	GEO_UseNormals(pst_Data->pst_Obj);
	n = pst_Data->pst_Obj->dst_PointNormal;

	for(i_Elem = 0; i_Elem < pst_Data->l_NbElements; i_Elem++)
	{
		pst_Elem = pst_Data->pst_Obj->dst_Element + i_Elem;
		pst_Elem->l_NbTriangles = pst_Obj->dst_Element[i_Elem].l_NbTriangles;
		pst_Elem->l_MaterialId = pst_Obj->dst_Element[i_Elem].l_MaterialId;
		pst_Elem->ul_NumberOfUsedIndex = 0;

		GEO_AllocElementContent(pst_Elem);

		L_memcpy
		(
			pst_Elem->dst_Triangle,
			pst_Obj->dst_Element[i_Elem].dst_Triangle,
			pst_Elem->l_NbTriangles * sizeof(GEO_tdst_IndexedTriangle)
		);
		t = pst_Elem->dst_Triangle;
		for(i_Triangle = 0; i_Triangle < pst_Elem->l_NbTriangles; i_Triangle++, t++)
		{
			MATH_CopyVector(VCast(p++), VCast(&pst_Obj->dst_Point[t->auw_Index[0]]));
			MATH_CopyVector(VCast(p++), VCast(&pst_Obj->dst_Point[t->auw_Index[1]]));
			MATH_CopyVector(VCast(p++), VCast(&pst_Obj->dst_Point[t->auw_Index[2]]));

			MATH_AddVector(&N, VCast(p - 3), VCast(p - 2));
			MATH_AddEqualVector(&N, VCast(p - 1));
			MATH_NormalizeAnyVector(&N, &N);

			MATH_CopyVector(n++, &N);
			MATH_CopyVector(n++, &N);
			MATH_CopyVector(n++, &N);

			t->auw_Index[0] = i_Point++;
			t->auw_Index[1] = i_Point++;
			t->auw_Index[2] = i_Point++;
		}
	}

	pst_Data->pst_ObjSave = pst_Obj;

	_pst_Mod->p_Data = pst_Data;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierExplode_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierExplode_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_GO;
	GAO_tdst_ModifierExplode	*pst_Data;
	GEO_Vertex					*pst_Point, *pst_Last;
	MATH_tdst_Vector			*pst_Normal;
	MATH_tdst_Vector			V0, V1;
	float						t0, g0, t1, g1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierExplode *) _pst_Mod->p_Data;
	if(!pst_Data) return;

	pst_GO = GDI_gpst_CurDD->pst_CurrentGameObject;
	pst_Data->pst_ObjSave = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;

	pst_GO->pst_Base->pst_Visu->pst_Object = &pst_Data->pst_Obj->st_Id;

	pst_Point = pst_Data->pst_Obj->dst_Point;
	pst_Last = pst_Point + pst_Data->pst_Obj->l_NbPoints;
	GEO_UseNormals(pst_Data->pst_Obj);
	pst_Normal = pst_Data->pst_Obj->dst_PointNormal;

	t0 = pst_Data->f_Time;
	g0 = t0 * t0 * pst_Data->f_Grav;
	t1 = t0 + TIM_gf_dt;
	g1 = t1 * t1 * pst_Data->f_Grav;

	for(; pst_Point < pst_Last; pst_Point++, pst_Normal++)
	{
		MATH_AddScaleVector(&V0, VCast(pst_Point), pst_Normal, t0);
		V0.z += g0;
		MATH_AddScaleVector(&V1, VCast(pst_Point), pst_Normal, t1);
		V1.z += g1;
		MATH_SubEqualVector(&V1, &V0);
		MATH_ScaleEqualVector(&V1, pst_Data->f_Mul);
		MATH_AddEqualVector(VCast(pst_Point), &V1);
	}

	pst_Data->f_Time = t1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierExplode_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_GO;
	GAO_tdst_ModifierExplode	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierExplode *) _pst_Mod->p_Data;
	if(!pst_Data) return;
	pst_GO = GDI_gpst_CurDD->pst_CurrentGameObject;
	pst_GO->pst_Base->pst_Visu->pst_Object = &pst_Data->pst_ObjSave->st_Id;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierExplode_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierExplode	*pst_Data;
	int							i_Elem, i_Triangle;
	GEO_tdst_IndexedTriangle	*t;
	GEO_Vertex					*p;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierExplode *) _pst_Mod->p_Data;
	if(!pst_Data) 
	{
		GAO_ModifierExplode_Create(_pst_Mod->pst_GO, _pst_Mod, NULL);
		return;
	}

	pst_Data->f_Time = 0;
	p = pst_Data->pst_Obj->dst_Point;
	for(i_Elem = 0; i_Elem < pst_Data->l_NbElements; i_Elem++)
	{
		t = pst_Data->pst_ObjSave->dst_Element[i_Elem].dst_Triangle;

		for(i_Triangle = 0; i_Triangle < pst_Data->pst_ObjSave->dst_Element[i_Elem].l_NbTriangles; i_Triangle++, t++)
		{
			MATH_CopyVector(VCast(p++), VCast(&pst_Data->pst_ObjSave->dst_Point[t->auw_Index[0]]));
			MATH_CopyVector(VCast(p++), VCast(&pst_Data->pst_ObjSave->dst_Point[t->auw_Index[1]]));
			MATH_CopyVector(VCast(p++), VCast(&pst_Data->pst_ObjSave->dst_Point[t->auw_Index[2]]));
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ModifierExplode_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~*/
	char	*pc_Cur;
	/*~~~~~~~~~~~~*/

	pc_Cur = _pc_Buffer;

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;
	LOA_ReadLong_Ed(&pc_Cur, NULL); /* skip size */
	return(pc_Cur - _pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierExplode_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~*/
	ULONG	ul_Size;
	/*~~~~~~~~~~~~*/

	ul_Size = 0;
	SAV_Buffer(&ul_Size, 4);
}

/*$4
 ***********************************************************************************************************************
    Explode:: All face are separated
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSaveAddMatrix_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *_p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierSaveAddMatrix	*pst_Data;
	int								i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->p_Data = pst_Data = (GAO_tdst_ModifierSaveAddMatrix*)MEM_p_Alloc(sizeof(GAO_tdst_ModifierSaveAddMatrix));
	L_memset(pst_Data, 0, sizeof(GAO_tdst_ModifierSaveAddMatrix));

	_pst_Mod->ul_Flags |= MDF_C_Modifier_NoApply;

	if(!_p_Data)
	{
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)
		{
			pst_Data->l_NbGizmo = _pst_GO->pst_Base->pst_AddMatrix->l_Number;
			if(pst_Data->l_NbGizmo)
			{
				i = pst_Data->l_NbGizmo * sizeof(GAO_tdst_ModifierSaveAddMatrix_Gizmo);
				pst_Data->dst_Gizmo = (GAO_tdst_ModifierSaveAddMatrix_Gizmo *) MEM_p_Alloc(i);

				if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
				{
					pst_Data->c_GizmoPtr = 1;

					for(i = 0; i < pst_Data->l_NbGizmo; i++)
					{
						/*
						 * pst_Data->dst_Gizmo[i].pst_GO =
						 * _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO;
						 */
						pst_Data->dst_Gizmo[i].ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO);

						/*
						 * pst_Data->dst_Gizmo[i].pst_Matrix = OBJ_pst_GetAbsoluteMatrix(
						 * pst_Data->dst_Gizmo[i].pst_GO );
						 */
					}
				}
				else
				{
					pst_Data->c_GizmoPtr = 0;

					for(i = 0; i < pst_Data->l_NbGizmo; i++)
					{
						pst_Data->dst_Gizmo[i].pst_GO = NULL;
						pst_Data->dst_Gizmo[i].ul_Key = BIG_C_InvalidKey;
						pst_Data->dst_Gizmo[i].pst_Matrix = _pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_Matrix;
					}
				}
			}
		}
	}
	else
	{
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierSaveAddMatrix_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierSaveAddMatrix	*pst_Data;
	int								i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierSaveAddMatrix *) _pst_Mod->p_Data;

	if(pst_Data->dst_Gizmo)
	{
		MEM_Free(pst_Data->dst_Gizmo);
		pst_Data->dst_Gizmo = NULL;
	}

	if(pst_Data->dst_Slot)
	{
		for(i = 0; i < pst_Data->l_NbSlot; i++)
		{
			if(pst_Data->dst_Slot[i].dst_Mat) MEM_Free(pst_Data->dst_Slot[i].dst_Mat);
		}

		MEM_Free(pst_Data->dst_Slot);
		pst_Data->dst_Slot = NULL;
	}

	MEM_Free(pst_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ul_ModifierSaveAddMatrix_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char							*pc_Cur;
	GAO_tdst_ModifierSaveAddMatrix	*pst_Data;

	ULONG							ul_Size;
	int								i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierSaveAddMatrix *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	/* Size */
	ul_Size = LOA_ReadULong(&pc_Cur);

	/* read main data */
	pst_Data->c_GizmoPtr = LOA_ReadChar(&pc_Cur);
	pst_Data->c_Init = LOA_ReadChar(&pc_Cur);
	pst_Data->c_Init = 0;
	pst_Data->w_Dummy = LOA_ReadShort(&pc_Cur);
	pst_Data->l_NbGizmo = LOA_ReadLong(&pc_Cur);
	pst_Data->l_NbSlot = LOA_ReadLong(&pc_Cur);

	/* allocate main data */
	j = pst_Data->l_NbGizmo * sizeof(GAO_tdst_ModifierSaveAddMatrix_Gizmo);
	pst_Data->dst_Gizmo = (GAO_tdst_ModifierSaveAddMatrix_Gizmo *) (j ? MEM_p_Alloc(j) : 0);

	j = pst_Data->l_NbSlot * sizeof(GAO_tdst_ModifierSaveAddMatrix_Slot);
	pst_Data->dst_Slot = (GAO_tdst_ModifierSaveAddMatrix_Slot *) (j ? MEM_p_Alloc(j) : 0);

	/* read gizmo */
	for(i = 0; i < pst_Data->l_NbGizmo; i++)
	{
		pst_Data->dst_Gizmo[i].ul_Key = LOA_ReadLong(&pc_Cur);
		pst_Data->dst_Gizmo[i].ul_Key = LOA_ReadLong(&pc_Cur);

		/*
		 * LOA_MakeFileRef(pst_Data->dst_Gizmo[i].ul_Key,(ULONG *)
		 * &pst_Data->dst_Gizmo[i].pst_GO, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
		 */
	}

	/* read slot */
	for(i = 0; i < pst_Data->l_NbSlot; i++)
	{
		LOA_ReadString(&pc_Cur, pst_Data->dst_Slot[i].sz_Name, 64);
		if(pst_Data->l_NbGizmo)
		{
			j = pst_Data->l_NbGizmo * sizeof(MATH_tdst_Matrix);
			pst_Data->dst_Slot[i].dst_Mat = (MATH_tdst_Matrix *) MEM_p_Alloc(j);
			for(j = 0; j < pst_Data->l_NbGizmo; j++)
			{
				LOA_ReadMatrix(&pc_Cur, &pst_Data->dst_Slot[i].dst_Mat[j]);
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
void GAO_ModifierSaveAddMatrix_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierSaveAddMatrix	*pst_Data;

	ULONG							ul_Size;
	int								i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierSaveAddMatrix *) _pst_Mod->p_Data;
	ul_Size = 0;

	/* First compute size of data to save it */
	ul_Size += 4;							/* Matrix Ptr + dummy */
	ul_Size += 4;							/* Number of gizmo */
	ul_Size += 4;							/* Number of slot */
	ul_Size += pst_Data->l_NbGizmo * 8;
	ul_Size += pst_Data->l_NbSlot * (64 + pst_Data->l_NbGizmo * sizeof(MATH_tdst_Matrix));

	SAV_Buffer(&ul_Size, 4);

	/* Now save */
	SAV_Buffer(&pst_Data->c_GizmoPtr, 1);	/* Flags */
	SAV_Buffer(&pst_Data->c_Init, 1);		/* dummy */
	SAV_Buffer(&pst_Data->w_Dummy, 2);		/* dummy */
	SAV_Buffer(&pst_Data->l_NbGizmo, 4);	/* Number of gizmo */
	SAV_Buffer(&pst_Data->l_NbSlot, 4);		/* Number of slot */

	for(i = 0; i < pst_Data->l_NbGizmo; i++)
	{
		SAV_Buffer(&pst_Data->dst_Gizmo[i].ul_Key, 4);
		SAV_Buffer(&pst_Data->dst_Gizmo[i].ul_Key, 4);
	}

	for(i = 0; i < pst_Data->l_NbSlot; i++)
	{
		SAV_Buffer(&pst_Data->dst_Slot[i].sz_Name, 64);
		SAV_Buffer(pst_Data->dst_Slot[i].dst_Mat, pst_Data->l_NbGizmo * sizeof(MATH_tdst_Matrix));
	}
}

#endif /* ACTIVE_EDITORS */
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierPhoto_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierPhoto	*pst_Data;
	GAO_tdst_ModifierPhoto	*pst_SrcData;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	pst_SrcData = (GAO_tdst_ModifierPhoto *) p_Data;
	pst_Data = (GAO_tdst_ModifierPhoto *) MEM_p_Alloc(sizeof(GAO_tdst_ModifierPhoto));

	pst_Data->i_Mission = pst_SrcData ? pst_SrcData->i_Mission : 0;
	pst_Data->i_Info = pst_SrcData ? pst_SrcData->i_Info : 0;

	pst_Data->i_BoneForSpherePivot = pst_SrcData ? pst_SrcData->i_BoneForSpherePivot : -1;
	pst_Data->i_BoneForInfoPivot = pst_SrcData ? pst_SrcData->i_BoneForInfoPivot : -1;

	pst_Data->f_CurrentFrame = 0.0f;
	pst_Data->f_FrameMin = pst_SrcData ? pst_SrcData->f_FrameMin : 0.5f;
	pst_Data->f_FrameMax = 1.001f; /* Hideous Tag to indicate the save/load of the offset vector ... mouarf */

	pst_Data->ul_Flags = 0;

	if(pst_SrcData)
	{
		MATH_CopyVector(&pst_Data->st_SphereOffset, &pst_SrcData->st_SphereOffset);
		MATH_CopyVector(&pst_Data->st_InfoOffset, &pst_SrcData->st_InfoOffset);
	}
	else
	{
		MATH_InitVector(&pst_Data->st_SphereOffset, 0.0f, 0.0f, 0.0f);
		MATH_InitVector(&pst_Data->st_InfoOffset, 0.0f, 0.0f, 0.0f);
	}

	pst_Data->f_CurrentLOD = 0.0f;
	pst_Data->f_LODMin = pst_SrcData ? pst_SrcData->f_LODMin : 0.05f;				/* 5% screen per default */
	pst_Data->f_LODMax = pst_SrcData ? pst_SrcData->f_LODMax : 1.0f;

	pst_Data->f_Radius = pst_SrcData ? pst_SrcData->f_Radius : 0.50f;

	_pst_Mod->p_Data = pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierPhoto_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	if(_pst_Mod->p_Data) MEM_Free(_pst_Mod->p_Data);

#ifdef ACTIVE_EDITORS
	OBJ_gb_DebugPhotoMode = FALSE;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

BOOL GAO_ModifierPhoto_LODAndFrameareOK(GAO_tdst_ModifierPhoto *_pst_ModPhoto)
{
	if(!_pst_ModPhoto) return FALSE;

	return 
		(
			(_pst_ModPhoto->f_CurrentLOD >= _pst_ModPhoto->f_LODMin)
		&&  (_pst_ModPhoto->f_CurrentLOD <= _pst_ModPhoto->f_LODMax)
		&&  !(_pst_ModPhoto->ul_Flags & MDF_InfoPhoto_ForceBadLOD)
		&&	(_pst_ModPhoto->f_CurrentFrame >= _pst_ModPhoto->f_FrameMin)
		&&  (_pst_ModPhoto->f_CurrentFrame <= _pst_ModPhoto->f_FrameMax)
		&&  !(_pst_ModPhoto->ul_Flags & MDF_InfoPhoto_ForceBadFrame)
		);
}


extern BOOL OBJ_Frame_CullingSphere(MATH_tdst_Vector *, float, CAM_tdst_Camera *, LONG, LONG, float *, float *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *GAO_ModifierPhoto_SnapGOGet(OBJ_tdst_GameObject *_pst_GO, GAO_tdst_ModifierPhoto *_pst_ModPhoto, int _i_Mode)
{
	int		i_Pivot;

	i_Pivot = (_i_Mode == 0) ? _pst_ModPhoto->i_BoneForSpherePivot : _pst_ModPhoto->i_BoneForInfoPivot;

	if(i_Pivot == -1)
	{
		return _pst_GO;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject *pst_CanalGO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_CanalGO = NULL;

		if
		(
			(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
		&&	_pst_GO->pst_Base
		&&	_pst_GO->pst_Base->pst_GameObjectAnim
		&&	_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
		&&	_pst_GO->pst_Base->pst_GameObjectAnim->pst_Shape
		)
		{
			pst_CanalGO = ANI_pst_GetObjectByAICanal(_pst_GO, (UCHAR) i_Pivot);
		}
		else
		{
			if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				OBJ_tdst_GameObject *pst_RefGO;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				
				pst_RefGO = ANI_pst_GetReference(_pst_GO);
				if(pst_RefGO)
					pst_CanalGO = ANI_pst_GetObjectByAICanal(pst_RefGO, (UCHAR) i_Pivot);
			}	
		}

		if(pst_CanalGO)
			return pst_CanalGO;
		else
		{
			if(_i_Mode == 0)
				_pst_ModPhoto->i_BoneForSpherePivot = -1;
			else
				_pst_ModPhoto->i_BoneForInfoPivot = -1;

			return _pst_GO;
		}			
		
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

MDF_tdst_Modifier *GAO_ModifierPhoto_Get(OBJ_tdst_GameObject *_pst_GO, BOOL _b_Update)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	CAM_tdst_Camera			*pst_Camera;
	OBJ_tdst_GameObject		*pst_SnapGO;
	MDF_tdst_Modifier		*pst_Modifier;
	GAO_tdst_ModifierPhoto	*pst_Data;
	MATH_tdst_Vector		st_GCS_Center;
	float					f_GCS_Radius;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	if(!_pst_GO) return FALSE;
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	if(!pst_World) return FALSE;
	if (!((GDI_tdst_DisplayData *) pst_World->pst_View->st_DisplayInfo.pst_DisplayDatas) ) return FALSE;
	pst_Camera = &((GDI_tdst_DisplayData *) pst_World->pst_View->st_DisplayInfo.pst_DisplayDatas)->st_Camera;
	if(!pst_Camera) return FALSE;

	/* Firstly tries to find a Info-Photo Modifier directly into the GO structure */
	pst_Modifier = NULL;
	if((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (_pst_GO->pst_Extended))
	{
		pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
		while(pst_Modifier)
		{
			if
			(
				(pst_Modifier->i->ul_Type == MDF_C_Modifier_InfoPhoto) 
			&&	(!(pst_Modifier->ul_Flags & (MDF_C_Modifier_Inactive|MDF_C_Modifier_NoApply))) 
			&&	(pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGao)
			)
			{
				if(_b_Update)
				{
					pst_Data = (GAO_tdst_ModifierPhoto *) pst_Modifier->p_Data;

					pst_SnapGO = GAO_ModifierPhoto_SnapGOGet(_pst_GO, pst_Data, 0);
					MATH_TransformVertex(&st_GCS_Center, pst_SnapGO->pst_GlobalMatrix, &pst_Data->st_SphereOffset);
					
					f_GCS_Radius = pst_Data->f_Radius;
					
					/* Update the Current Frame and LOD Value */
					OBJ_Frame_CullingSphere
					(
						&st_GCS_Center,
						pst_Data->f_Radius,
						pst_Camera,
						512,
						256,
						&pst_Data->f_CurrentFrame,
						&pst_Data->f_CurrentLOD
					);
				}

				return pst_Modifier;
				
			}
			pst_Modifier = pst_Modifier->pst_Next;
		}
	}

	/* Secondly, tries to find a Info-Photo Modifier onto the Canal 0 of the GO's skeleton */
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject		*pst_BoneGO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
		pst_BoneGO = ANI_pst_GetObjectByAICanal(_pst_GO, 0);
		if(!pst_BoneGO) return NULL;
		
		pst_Modifier = NULL;
		if((pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (pst_BoneGO->pst_Extended))
		{
			pst_Modifier = pst_BoneGO->pst_Extended->pst_Modifiers;
			while(pst_Modifier)
			{
				if
				(
					(pst_Modifier->i->ul_Type == MDF_C_Modifier_InfoPhoto) 
				&&	(!(pst_Modifier->ul_Flags & (MDF_C_Modifier_Inactive|MDF_C_Modifier_NoApply))) 
				&&	(pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGao)
				)
				{
					if(_b_Update)
					{
						
						pst_Data = (GAO_tdst_ModifierPhoto *) pst_Modifier->p_Data;
						pst_SnapGO = GAO_ModifierPhoto_SnapGOGet(pst_BoneGO, pst_Data, 0);
						MATH_TransformVertex(&st_GCS_Center, pst_SnapGO->pst_GlobalMatrix, &pst_Data->st_SphereOffset);
						
						f_GCS_Radius = pst_Data->f_Radius;
						
						/* Update the Current Frame and LOD Value */
						OBJ_Frame_CullingSphere
						(
							&st_GCS_Center,
							pst_Data->f_Radius,
							pst_Camera,
							512,
							256,
							&pst_Data->f_CurrentFrame,
							&pst_Data->f_CurrentLOD
						);
					}
					
					return pst_Modifier;
				}
				pst_Modifier = pst_Modifier->pst_Next;
			}
		}
	}

	return NULL;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierPhoto_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierPhoto_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierPhoto_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ModifierPhoto_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char							*pc_Cur;
	GAO_tdst_ModifierPhoto			*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

	pst_Data = (GAO_tdst_ModifierPhoto *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;

	pst_Data->i_Mission = LOA_ReadInt(&pc_Cur);
	pst_Data->i_Info = LOA_ReadInt(&pc_Cur);

	pst_Data->i_BoneForSpherePivot = LOA_ReadInt(&pc_Cur);
	pst_Data->i_BoneForInfoPivot = LOA_ReadInt(&pc_Cur);

	pst_Data->f_LODMin = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_LODMax = LOA_ReadFloat(&pc_Cur);


	pst_Data->f_FrameMin = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_FrameMax = LOA_ReadFloat(&pc_Cur);

	pst_Data->ul_Flags = 0;

	/* bouh, que c'est vilain ... */
	if(pst_Data->f_FrameMax == 1.001f)
	{
		LOA_ReadVector(&pc_Cur, &pst_Data->st_SphereOffset);
		LOA_ReadVector(&pc_Cur, &pst_Data->st_InfoOffset);
	}
	else
	{
		MATH_InitVector(&pst_Data->st_SphereOffset, 0.0f, 0.0f, 0.0f);
		MATH_InitVector(&pst_Data->st_InfoOffset, 0.0f, 0.0f, 0.0f);
	}

	pst_Data->f_Radius = LOA_ReadFloat(&pc_Cur);

	return(pc_Cur - _pc_Buffer);

}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierPhoto_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierPhoto	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GAO_tdst_ModifierPhoto	*)_pst_Mod->p_Data;

	SAV_Buffer(&pst_Data->i_Mission, 4);
	SAV_Buffer(&pst_Data->i_Info, 4);

	SAV_Buffer(&pst_Data->i_BoneForSpherePivot, 4);
	SAV_Buffer(&pst_Data->i_BoneForInfoPivot, 4);

	SAV_Buffer(&pst_Data->f_LODMin, 4);
	SAV_Buffer(&pst_Data->f_LODMax, 4);

	SAV_Buffer(&pst_Data->f_FrameMin, 4);

	if(pst_Data->f_FrameMax == 1.0f) pst_Data->f_FrameMax = 1.001f;
	SAV_Buffer(&pst_Data->f_FrameMax, 4);

	SAV_Buffer(&pst_Data->st_SphereOffset, 12);
	SAV_Buffer(&pst_Data->st_InfoOffset, 12);

	SAV_Buffer(&pst_Data->f_Radius, 4);



}
#endif

/*$4
***********************************************************************************************************************
Rotation paste
***********************************************************************************************************************
*/

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierRotationPaste_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
    GAO_tdst_ModifierRotationPaste *pst_Data;

    _pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierRotationPaste));
    pst_Data = (GAO_tdst_ModifierRotationPaste *) _pst_Mod->p_Data;

    if(!p_Data)
    {
        L_memset(pst_Data , 0 , sizeof(GAO_tdst_ModifierRotationPaste));
    }
    else
    {
        L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierRotationPaste));
    }	

    _pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierRotationPaste_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
    GAO_tdst_ModifierRotationPaste *pst_Data;

    pst_Data = (GAO_tdst_ModifierRotationPaste *) _pst_Mod->p_Data;	
    if(!pst_Data) 
        return;

    MEM_Free(pst_Data);

    _pst_Mod->p_Data = NULL;
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierRotationPaste_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
//#if !defined(XML_CONV_TOOL)//POPOWARNING y etait plus
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Matrix	M;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    GAO_tdst_ModifierRotationPaste *p_RotationPaste = (GAO_tdst_ModifierRotationPaste *) _pst_Mod->p_Data;

    if(!p_RotationPaste->pObjectToPasteFrom) return;

    MATH_GetRotationMatrix(&M, OBJ_pst_GetAbsoluteMatrix(p_RotationPaste->pObjectToPasteFrom));
    MATH_Copy33Matrix(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO), &M);

    OBJ_ComputeLocalWhenHie( GDI_gpst_CurDD->pst_CurrentGameObject );
//#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierRotationPaste_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierRotationPaste_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG GAO_ModifierRotationPaste_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
    ULONG	ul_Version;
    UCHAR * pc_Cur = (UCHAR*)_pc_Buffer;
    GAO_tdst_ModifierRotationPaste *p_RotationPaste;
    ULONG ulKey;

    _pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

    ul_Version = LOA_ReadULong((CHAR**)&pc_Cur);

    p_RotationPaste = (GAO_tdst_ModifierRotationPaste *) _pst_Mod->p_Data;

    ulKey = LOA_ReadULong((CHAR**)&pc_Cur);

    if (ulKey && ulKey != BIG_C_InvalidKey)
    {
        LOA_MakeFileRef(ulKey, (ULONG *) &p_RotationPaste->pObjectToPasteFrom, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
    }
    else
    {
        p_RotationPaste->pObjectToPasteFrom = NULL;
    }

    return (pc_Cur - (UCHAR*)_pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierRotationPaste_Save(MDF_tdst_Modifier *_pst_Mod)
{
    GAO_tdst_ModifierRotationPaste *p_RotationPaste;
    ULONG ulData;
    ULONG	ul_Version = 0;
    
    SAV_Buffer(&ul_Version, 4);

    p_RotationPaste = (GAO_tdst_ModifierRotationPaste *) _pst_Mod->p_Data;

    if (p_RotationPaste->pObjectToPasteFrom)
        ulData = LOA_ul_SearchKeyWithAddress((ULONG) p_RotationPaste->pObjectToPasteFrom);
    else
        ulData = 0;

    SAV_Buffer(&ulData, 4);
}

#endif

/*$4
***********************************************************************************************************************
Translation paste
***********************************************************************************************************************
*/

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierTranslationPaste_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
    GAO_tdst_ModifierTranslationPaste *pst_Data;

    _pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierTranslationPaste));
    pst_Data = (GAO_tdst_ModifierTranslationPaste *) _pst_Mod->p_Data;

    if(!p_Data)
    {
        L_memset(pst_Data , 0 , sizeof(GAO_tdst_ModifierTranslationPaste));
    }
    else
    {
        L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierTranslationPaste));
    }	

    _pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierTranslationPaste_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
    GAO_tdst_ModifierTranslationPaste *pst_Data;

    pst_Data = (GAO_tdst_ModifierTranslationPaste *) _pst_Mod->p_Data;	
    if(!pst_Data) 
        return;

    MEM_Free(pst_Data);

    _pst_Mod->p_Data = NULL;
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void GAO_ModifierTranslationPaste_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
//#if !defined(XML_CONV_TOOL) //POPOWARNING a plus
    GAO_tdst_ModifierTranslationPaste *p_TranslationPaste = (GAO_tdst_ModifierTranslationPaste *) _pst_Mod->p_Data;

    if(!p_TranslationPaste->pObjectToPasteFrom) return;

    MATH_CopyVector(MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)), MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(p_TranslationPaste->pObjectToPasteFrom)));

    OBJ_ComputeLocalWhenHie( GDI_gpst_CurDD->pst_CurrentGameObject );
//#endif
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierTranslationPaste_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierTranslationPaste_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG GAO_ModifierTranslationPaste_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
    ULONG	ul_Version;
    GAO_tdst_ModifierTranslationPaste *p_TranslationPaste;
    ULONG ulKey;
    UCHAR * pc_Cur = (UCHAR*)_pc_Buffer;

    _pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

    ul_Version = LOA_ReadULong((CHAR**)&pc_Cur);

    p_TranslationPaste = (GAO_tdst_ModifierTranslationPaste *) _pst_Mod->p_Data;

    ulKey = LOA_ReadULong((CHAR**)&pc_Cur);

    if (ulKey && ulKey != BIG_C_InvalidKey)
    {
        LOA_MakeFileRef(ulKey, (ULONG *) &p_TranslationPaste->pObjectToPasteFrom, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
    }
    else
    {
        p_TranslationPaste->pObjectToPasteFrom = NULL;
    }

    return (pc_Cur - (UCHAR*)_pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierTranslationPaste_Save(MDF_tdst_Modifier *_pst_Mod)
{
    ULONG	ul_Version = 0;
    GAO_tdst_ModifierTranslationPaste *p_TranslationPaste;
    ULONG ulData;

    SAV_Buffer(&ul_Version, 4);

    p_TranslationPaste = (GAO_tdst_ModifierTranslationPaste *) _pst_Mod->p_Data;

    if (p_TranslationPaste->pObjectToPasteFrom)
        ulData = LOA_ul_SearchKeyWithAddress((ULONG) p_TranslationPaste->pObjectToPasteFrom);
    else
        ulData = 0;

    SAV_Buffer(&ulData, 4);
}

#endif


/*$4
***********************************************************************************************************************
Animated GAO modifier
***********************************************************************************************************************
*/

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierAnimatedGAO_Refresh(MDF_tdst_Modifier *_pst_Mod)
{
    GAO_tdst_ModifierAnimatedGAO *p_AnimatedGAO = (GAO_tdst_ModifierAnimatedGAO *) _pst_Mod->p_Data;
    int i;

    for(i=0; i<9; i++)
    {
        ULONG ulFlag = 1<<((i+1)+(i/3));
        if(
#ifdef ACTIVE_EDITORS
            p_AnimatedGAO->eLastAnimationTypes[i] != p_AnimatedGAO->eAnimationTypes[i] || 
#endif
            (!(p_AnimatedGAO->pParams[i]) && (p_AnimatedGAO->ulRawFlags & ulFlag)))
        {
            if(p_AnimatedGAO->pParams[i])
                MEM_Free(p_AnimatedGAO->pParams[i]);

            switch(p_AnimatedGAO->eAnimationTypes[i])
            {
            case MDF_AnimatedGAO_Linear:										
                {
                    GAO_tdst_ModifierAnimatedGAOLinearParams* pParam = (GAO_tdst_ModifierAnimatedGAOLinearParams*)MEM_p_Alloc(sizeof(GAO_tdst_ModifierAnimatedGAOLinearParams));
                    p_AnimatedGAO->pParams[i] = pParam;
#ifdef ACTIVE_EDITORS
                    p_AnimatedGAO->eLastAnimationTypes[i] = MDF_AnimatedGAO_Linear;
#endif
                    pParam->f_Max = pParam->f_Min = pParam->f_StartTime = 0.0f;
                    pParam->f_StopTime = pParam->f_TotalTime = 1.0f;					
                    pParam->b_BackAndForth = TRUE;
                }
                break;

            case MDF_AnimatedGAO_Noise:
                {
                    GAO_tdst_ModifierAnimatedGAONoiseParams* pParam = (GAO_tdst_ModifierAnimatedGAONoiseParams*)MEM_p_Alloc(sizeof(GAO_tdst_ModifierAnimatedGAONoiseParams));
                    p_AnimatedGAO->pParams[i] = pParam;
#ifdef ACTIVE_EDITORS
                    p_AnimatedGAO->eLastAnimationTypes[i] = MDF_AnimatedGAO_Noise;
#endif
                    pParam->f_Min = 0.0f;
                    pParam->f_Max = 1.0f;
                    pParam->f_Var = pParam->f_VarSpeed = 1.0f;	
                }
                break;

            case MDF_AnimatedGAO_Sinus:
                {
                    GAO_tdst_ModifierAnimatedGAOSinusParams* pParam = (GAO_tdst_ModifierAnimatedGAOSinusParams*)MEM_p_Alloc(sizeof(GAO_tdst_ModifierAnimatedGAOSinusParams));
                    p_AnimatedGAO->pParams[i] = pParam;
#ifdef ACTIVE_EDITORS
                    p_AnimatedGAO->eLastAnimationTypes[i] = MDF_AnimatedGAO_Sinus;
#endif
                    pParam->f_TotalTime = 1.0f;
                    pParam->f_Angle = 1.0f;
                    //pParam->b_BackAndForth = TRUE;
                    pParam->f_TimeBias = 0.0f;
                }
                break;
            }			
        }
    }

    if(OBJ_b_TestIdentityFlag(_pst_Mod->pst_GO, OBJ_C_IdentityFlag_HasInitialPos))
        MATH_CopyMatrix(&p_AnimatedGAO->st_Original, OBJ_pst_GetInitialAbsoluteMatrix(_pst_Mod->pst_GO));
}


/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierAnimatedGAO_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
    GAO_tdst_ModifierAnimatedGAO *pst_Data;
    GAO_tdst_ModifierAnimatedGAO *pst_DataSrc;

    _pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierAnimatedGAO));
    pst_Data = (GAO_tdst_ModifierAnimatedGAO *) _pst_Mod->p_Data;
    pst_DataSrc = (GAO_tdst_ModifierAnimatedGAO *) p_Data;

    if(!p_Data)
    {
        L_memset(pst_Data , 0 , sizeof(GAO_tdst_ModifierAnimatedGAO));
    }
    else
    {
        int i;
        int iSize;

        L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierAnimatedGAO));

        for(i=0; i<9; i++)
        {
            pst_Data->pParams[i] = NULL;
        }

        GAO_ModifierAnimatedGAO_Refresh(_pst_Mod);		

        for(i=0; i<9; i++)
        {
            if(pst_Data->pParams[i] && pst_DataSrc->pParams[i])
            {
                switch(pst_Data->eAnimationTypes[i])
                {
                case MDF_AnimatedGAO_Linear: iSize = sizeof(GAO_tdst_ModifierAnimatedGAOLinearParams); break;
                case MDF_AnimatedGAO_Noise: iSize = sizeof(GAO_tdst_ModifierAnimatedGAONoiseParams); break;
                case MDF_AnimatedGAO_Sinus: iSize = sizeof(GAO_tdst_ModifierAnimatedGAOSinusParams); break;
                default : continue;
                }

                L_memcpy(pst_Data->pParams[i], pst_DataSrc->pParams[i], iSize);
            }
        }
    }	

    MATH_CopyMatrix(&pst_Data->st_Original, OBJ_pst_GetInitialAbsoluteMatrix(_pst_Mod->pst_GO));

    _pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierAnimatedGAO_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
    GAO_tdst_ModifierAnimatedGAO *pst_Data;
    int i;

    pst_Data = (GAO_tdst_ModifierAnimatedGAO *) _pst_Mod->p_Data;	
    if(!pst_Data) 
        return;

    if(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)) // if we destroy te modifier (not the gao), revert to original position
        MATH_CopyMatrix(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO), &pst_Data->st_Original);

    for(i=0; i<9; i++)
    {
        if(pst_Data->pParams[i])
            MEM_Free(pst_Data->pParams[i]);
    }

    MEM_Free(pst_Data);

    _pst_Mod->p_Data = NULL;
}
//#ifndef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*//*
void GAO_ModifierAnimatedGAO_Save(MDF_tdst_Modifier *_pst_Mod)
{
}

/*
=======================================================================================================================
=======================================================================================================================
*//*
void GAO_ModifierAnimatedGAO_Reinit(MDF_tdst_Modifier *_pt_Mdf)
{
}
/*
=======================================================================================================================
=======================================================================================================================
*//*
ULONG GAO_ModifierAnimatedGAO_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	return -1;
}
/*
=======================================================================================================================
=======================================================================================================================
*//*
void GAO_ModifierAnimatedGAO_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}
/*
=======================================================================================================================
=======================================================================================================================
*//*
void GAO_ModifierAnimatedGAO_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}
#endif*/

/*
=======================================================================================================================
=======================================================================================================================
*/
FLOAT GAO_ModifierAnimatedGAO_GetValue(MDF_tdst_Modifier *_pst_Mod, GAO_tdst_ModifierAnimatedGAO *p_AnimatedGAO, int iArg)
{
    FLOAT ret = 0.0f;
    FLOAT fTime = TIM_f_Clock_TrueRead();

    // put here because random and linear share the same cumputing code (except the randomization of parameters)
    GAO_tdst_ModifierAnimatedGAOLinearParams *pLinearParam;

    switch(p_AnimatedGAO->eAnimationTypes[iArg])
    {
    case MDF_AnimatedGAO_Linear:
        {
            if(p_AnimatedGAO->eAnimationTypes[iArg]==MDF_AnimatedGAO_Linear)
            {
                pLinearParam = (GAO_tdst_ModifierAnimatedGAOLinearParams *)p_AnimatedGAO->pParams[iArg];

                if(pLinearParam->f_TotalTime<TIM_gf_dt) 
                    break;
            }

            if(pLinearParam->f_CurTime<0.0f)
                pLinearParam->f_CurTime += pLinearParam->f_TotalTime;

            if(pLinearParam->f_CurTime == 0.0f || pLinearParam->f_CurTime>pLinearParam->f_TotalTime)
            {
                // at reset time (iteration is over), we synch with global time so we can synchronize many effects
                //if(p_AnimatedGAO->ulFlags.bSyncWithGameTime)
                pLinearParam->f_CurTime = fTime - (((int)(fTime / pLinearParam->f_TotalTime))*pLinearParam->f_TotalTime);
                /*else
                pLinearParam->f_CurTime -= (((int)(pLinearParam->f_CurTime / pLinearParam->f_TotalTime))*pLinearParam->f_TotalTime);
                */
            }

            if(pLinearParam->b_BackAndForth)
            {
                if(pLinearParam->f_CurTime<pLinearParam->f_StartTime || pLinearParam->f_CurTime>pLinearParam->f_StopTime)
                {
                    ret = pLinearParam->f_Min;
                }
                else if(pLinearParam->f_CurTime<pLinearParam->f_StopTime*0.5f)
                {
                    ret = pLinearParam->f_Min + (pLinearParam->f_Max-pLinearParam->f_Min)*((pLinearParam->f_CurTime*2.0f)/pLinearParam->f_StopTime);
                }
                else
                {
                    ret = pLinearParam->f_Max + (pLinearParam->f_Min-pLinearParam->f_Max)*((pLinearParam->f_CurTime-(pLinearParam->f_StopTime*0.5f))/(pLinearParam->f_StopTime*0.5f));
                }
            }
            else
            {
                if(pLinearParam->f_CurTime<pLinearParam->f_StartTime)
                {
                    ret = pLinearParam->f_Min;
                }
                else if(pLinearParam->f_CurTime>pLinearParam->f_StopTime)
                {
                    ret = pLinearParam->f_Max;
                }
                else
                {
                    ret = pLinearParam->f_Min + (pLinearParam->f_Max-pLinearParam->f_Min)*(pLinearParam->f_CurTime/pLinearParam->f_StopTime);
                }
            }
        }
        break;		

    case MDF_AnimatedGAO_Noise:
        {
			GAO_tdst_ModifierAnimatedGAONoiseParams *pParam = (GAO_tdst_ModifierAnimatedGAONoiseParams *)p_AnimatedGAO->pParams[iArg];
            FLOAT fNoise;

            if (pParam == NULL)
                break;

            fNoise = (iArg*6.11f) + fTime * pParam->f_VarSpeed;
#ifdef JADEFUSION
			ret = pParam->f_Min + pParam->f_Var * (g_PerlinNoiseMaker.noise1(fNoise) * 0.5f + 0.5f) * (pParam->f_Max-pParam->f_Min);
#else
			ret = pParam->f_Min + pParam->f_Var * (PerlinNoise1(fNoise) * 0.5f + 0.5f) * (pParam->f_Max-pParam->f_Min);
#endif
			ret = MATH_Max(pParam->f_Min, MATH_Min(pParam->f_Max, ret));
		}
        break;

    case MDF_AnimatedGAO_Sinus:
        {
            GAO_tdst_ModifierAnimatedGAOSinusParams *pParam = (GAO_tdst_ModifierAnimatedGAOSinusParams *)p_AnimatedGAO->pParams[iArg];

            if(pParam->f_TotalTime<TIM_gf_dt) 
                break;

            ret = fSin(((fTime+pParam->f_TimeBias)*3.1416f)/pParam->f_TotalTime)*(pParam->f_Angle/(2.0f));
        }
        break;
    }

    return ret;
}

//#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierAnimatedGAO_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
    MATH_tdst_Matrix	*M;
    MATH_tdst_Vector    vScale;
    MATH_tdst_Vector    vTranslation;

    GAO_tdst_ModifierAnimatedGAO *p_AnimatedGAO = (GAO_tdst_ModifierAnimatedGAO *) _pst_Mod->p_Data;

    if((_pst_Mod->pst_GO->ul_StatusAndControlFlags  & OBJ_C_ControlFlag_InPause))
        return;

    if(p_AnimatedGAO->ulFlags.bUseReferenceGAO && p_AnimatedGAO->p_GAOReference)
    {
        MATH_CopyMatrix(&p_AnimatedGAO->st_Original, OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject*)p_AnimatedGAO->p_GAOReference));

        if(p_AnimatedGAO->ulFlags.bUsePositionOffset)
        {
            MATH_AddEqualVector(&p_AnimatedGAO->st_Original.T, &p_AnimatedGAO->vOffset);
        }
    }

    M = OBJ_pst_GetAbsoluteMatrix(GDI_gpst_CurDD->pst_CurrentGameObject);
    //MATH_CopyMatrix(M, &p_AnimatedGAO->st_Original);

    if(p_AnimatedGAO->ulFlags.bApplyToRotation)
    {
        MATH_tdst_Matrix stFullRotation;
        MATH_tdst_Matrix stNewRotation;
        MATH_tdst_Matrix stRotation;
        MATH_tdst_Matrix stXRotation;
        MATH_tdst_Matrix stYRotation;
        MATH_tdst_Matrix stZRotation;

        MATH_SetIdentityMatrix(&stFullRotation);

        if(p_AnimatedGAO->ulFlags.bApplyToRotationX)
        {
            MATH_MakeRotationMatrix_XAxis(&stXRotation, GAO_ModifierAnimatedGAO_GetValue(_pst_Mod, p_AnimatedGAO, 0), TRUE);
            MATH_CopyMatrix(&stFullRotation, &stXRotation);			
        }

        if(p_AnimatedGAO->ulFlags.bApplyToRotationY)
        {
            MATH_MakeRotationMatrix_YAxis(&stYRotation, GAO_ModifierAnimatedGAO_GetValue(_pst_Mod, p_AnimatedGAO, 1), TRUE);
            MATH_CopyMatrix(&stRotation, &stFullRotation);			
            MATH_MulMatrixMatrix(&stFullRotation, &stYRotation, &stRotation);
        }

        if(p_AnimatedGAO->ulFlags.bApplyToRotationZ)
        {
            MATH_MakeRotationMatrix_ZAxis(&stZRotation, GAO_ModifierAnimatedGAO_GetValue(_pst_Mod, p_AnimatedGAO, 2), TRUE);
            MATH_CopyMatrix(&stRotation, &stFullRotation);			
            MATH_MulMatrixMatrix(&stFullRotation, &stZRotation, &stRotation);
        }

        MATH_MulMatrixMatrix(&stNewRotation, &stFullRotation, &p_AnimatedGAO->st_Original);
        // let the scale and translation components intact
        MATH_Copy33Matrix(M, &stNewRotation);
    }

    if(p_AnimatedGAO->ulFlags.bApplyToTranslation)
    {
        if(p_AnimatedGAO->ulFlags.bApplyToTranslationX)
        {
            vTranslation.x = p_AnimatedGAO->st_Original.T.x + GAO_ModifierAnimatedGAO_GetValue(_pst_Mod, p_AnimatedGAO, 3);
        }
        else
            vTranslation.x = M->T.x;

        if(p_AnimatedGAO->ulFlags.bApplyToTranslationY)
        {
            vTranslation.y = p_AnimatedGAO->st_Original.T.y + GAO_ModifierAnimatedGAO_GetValue(_pst_Mod, p_AnimatedGAO, 4);
        }
        else
            vTranslation.y = M->T.y;

        if(p_AnimatedGAO->ulFlags.bApplyToTranslationZ)
        {
            vTranslation.z = p_AnimatedGAO->st_Original.T.z + GAO_ModifierAnimatedGAO_GetValue(_pst_Mod, p_AnimatedGAO, 5);
        }
        else
            vTranslation.z = M->T.z;

        MATH_SetTranslation(M, &vTranslation);
    }

    if(p_AnimatedGAO->ulFlags.bApplyToScale)
    {
        MATH_tdst_Vector    vOldScale;
        MATH_GetScale(&vOldScale, &p_AnimatedGAO->st_Original);

        if(p_AnimatedGAO->ulFlags.bApplyToScaleX)
        {
            vScale.x = vOldScale.x * GAO_ModifierAnimatedGAO_GetValue(_pst_Mod, p_AnimatedGAO, 6);
        }
        else
            vScale.x = vOldScale.x;

        if(p_AnimatedGAO->ulFlags.bApplyToScaleY)
        {
            vScale.y = vOldScale.y * GAO_ModifierAnimatedGAO_GetValue(_pst_Mod, p_AnimatedGAO, 7);
        }
        else
            vScale.y = vOldScale.y;

        if(p_AnimatedGAO->ulFlags.bApplyToScaleZ)
        {
            vScale.z = vOldScale.z * GAO_ModifierAnimatedGAO_GetValue(_pst_Mod, p_AnimatedGAO, 8);
        }
        else
            vScale.z = vOldScale.z;

        MATH_SetScale(M, &vScale);
    }

    OBJ_ComputeLocalWhenHie( GDI_gpst_CurDD->pst_CurrentGameObject );
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierAnimatedGAO_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	int iArg;

	GAO_tdst_ModifierAnimatedGAO *p_AnimatedGAO = (GAO_tdst_ModifierAnimatedGAO *) _pst_Mod->p_Data;

    if((_pst_Mod->pst_GO->ul_StatusAndControlFlags  & OBJ_C_ControlFlag_InPause))
        return;

    // update timer
    for(iArg=0; iArg<9; iArg++)
    {
        ULONG ulFlag = 1<<((iArg+1)+(iArg/3));
        if(p_AnimatedGAO->pParams[iArg] && (p_AnimatedGAO->ulRawFlags & ulFlag))
        {
            switch(p_AnimatedGAO->eAnimationTypes[iArg])
            {
            case MDF_AnimatedGAO_Linear:
                {
                    GAO_tdst_ModifierAnimatedGAOLinearParams *pParam = (GAO_tdst_ModifierAnimatedGAOLinearParams *)p_AnimatedGAO->pParams[iArg];
                    pParam->f_CurTime += TIM_gf_dt;
                }
                break;
            }
        }
    }
}


/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierAnimatedGAO_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
    GAO_tdst_ModifierAnimatedGAO *p_AnimatedGAO;
    GAO_ModifierAnimatedGAO_Refresh(_pst_Mod);
	p_AnimatedGAO = (GAO_tdst_ModifierAnimatedGAO *) _pst_Mod->p_Data;

    if(p_AnimatedGAO->ulFlags.bUseReferenceGAO && p_AnimatedGAO->p_GAOReference)
    {
        MATH_CopyMatrix(&p_AnimatedGAO->st_Original, OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject*)p_AnimatedGAO->p_GAOReference));
    }
    else
        MATH_CopyMatrix(&p_AnimatedGAO->st_Original, OBJ_pst_GetInitialAbsoluteMatrix(_pst_Mod->pst_GO));

    if(p_AnimatedGAO->ulFlags.bUsePositionOffset)
    {
        MATH_AddEqualVector(&p_AnimatedGAO->st_Original.T, &p_AnimatedGAO->vOffset);
    }
}

/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG GAO_ModifierAnimatedGAO_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
    ULONG	ul_Version;
    ULONG   ul_Key;
    UCHAR * pc_Cur = (UCHAR*)_pc_Buffer;
	int iArg;
	GAO_tdst_ModifierAnimatedGAO *p_AnimatedGAO;

    _pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;

    ul_Version = LOA_ReadULong((CHAR**)&pc_Cur);

    p_AnimatedGAO = (GAO_tdst_ModifierAnimatedGAO *) _pst_Mod->p_Data;

    p_AnimatedGAO->ulRawFlags = LOA_ReadULong((CHAR**)&pc_Cur);

    if(ul_Version >= 2)
    {
        ul_Key = LOA_ReadULong((CHAR**)&pc_Cur);

        if (ul_Key && ul_Key != BIG_C_InvalidKey)
        {
            LOA_MakeFileRef
                (
                (ULONG) ul_Key,
                (ULONG *) &p_AnimatedGAO->p_GAOReference,
                OBJ_ul_GameObjectCallback,
                LOA_C_MustExists
                );
        }
        else
            p_AnimatedGAO->p_GAOReference = NULL;

        LOA_ReadVector((CHAR**)&pc_Cur, &p_AnimatedGAO->vOffset);
    }

    if(p_AnimatedGAO->ulFlags.bUseReferenceGAO && p_AnimatedGAO->p_GAOReference)
    {
        MATH_CopyMatrix(&p_AnimatedGAO->st_Original, OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject*)p_AnimatedGAO->p_GAOReference));
    }
    else
        MATH_CopyMatrix(&p_AnimatedGAO->st_Original, OBJ_pst_GetInitialAbsoluteMatrix(_pst_Mod->pst_GO));

    if(p_AnimatedGAO->ulFlags.bUsePositionOffset)
    {
        MATH_AddEqualVector(&p_AnimatedGAO->st_Original.T, &p_AnimatedGAO->vOffset);
    }

    for(iArg=0; iArg<9; iArg++)
    {
        ULONG ulFlag = 1<<((iArg+1)+(iArg/3));
        if(p_AnimatedGAO->ulRawFlags & ulFlag)
        {
            p_AnimatedGAO->eAnimationTypes[iArg] = (MDF_AnimatedGAO_Interpolation_Types)LOA_ReadULong((CHAR**)&pc_Cur);

            switch(p_AnimatedGAO->eAnimationTypes[iArg])
            {
            case MDF_AnimatedGAO_Linear:
                {
                    GAO_tdst_ModifierAnimatedGAOLinearParams* pParam = (GAO_tdst_ModifierAnimatedGAOLinearParams*)MEM_p_Alloc(sizeof(GAO_tdst_ModifierAnimatedGAOLinearParams));
                    L_memset(pParam, 0, sizeof(GAO_tdst_ModifierAnimatedGAOLinearParams));
                    p_AnimatedGAO->pParams[iArg] = pParam;
#ifdef ACTIVE_EDITORS
                    p_AnimatedGAO->eLastAnimationTypes[iArg] = MDF_AnimatedGAO_Linear;
#endif
                    pParam->f_Min = LOA_ReadFloat((CHAR**)&pc_Cur);
                    pParam->f_Max = LOA_ReadFloat((CHAR**)&pc_Cur);
                    pParam->f_StartTime = LOA_ReadFloat((CHAR**)&pc_Cur);
                    pParam->f_StopTime = LOA_ReadFloat((CHAR**)&pc_Cur);
                    pParam->f_TotalTime = LOA_ReadFloat((CHAR**)&pc_Cur);
                    pParam->b_BackAndForth = LOA_ReadULong((CHAR**)&pc_Cur);
}
                break;
            case MDF_AnimatedGAO_Noise:
                {
                    GAO_tdst_ModifierAnimatedGAONoiseParams* pParam = (GAO_tdst_ModifierAnimatedGAONoiseParams*)MEM_p_Alloc(sizeof(GAO_tdst_ModifierAnimatedGAONoiseParams));
                    L_memset(pParam, 0, sizeof(GAO_tdst_ModifierAnimatedGAONoiseParams));
                    p_AnimatedGAO->pParams[iArg] = pParam;
#ifdef ACTIVE_EDITORS
                    p_AnimatedGAO->eLastAnimationTypes[iArg] = MDF_AnimatedGAO_Noise;
#endif
                    pParam->f_Min = LOA_ReadFloat((CHAR**)&pc_Cur);
                    pParam->f_Max = LOA_ReadFloat((CHAR**)&pc_Cur);
                    pParam->f_Var = LOA_ReadFloat((CHAR**)&pc_Cur);
                    pParam->f_VarSpeed = LOA_ReadFloat((CHAR**)&pc_Cur);
                }
                break;
            case MDF_AnimatedGAO_Sinus:
                {
                    GAO_tdst_ModifierAnimatedGAOSinusParams* pParam = (GAO_tdst_ModifierAnimatedGAOSinusParams*)MEM_p_Alloc(sizeof(GAO_tdst_ModifierAnimatedGAOSinusParams));
                    L_memset(pParam, 0, sizeof(GAO_tdst_ModifierAnimatedGAOSinusParams));
                    p_AnimatedGAO->pParams[iArg] = pParam;
#ifdef ACTIVE_EDITORS
                    p_AnimatedGAO->eLastAnimationTypes[iArg] = MDF_AnimatedGAO_Sinus;
#endif
                    pParam->f_Angle = LOA_ReadFloat((CHAR**)&pc_Cur);
                    pParam->f_TotalTime = LOA_ReadFloat((CHAR**)&pc_Cur);

                    if(ul_Version>=1)
                        pParam->f_TimeBias = LOA_ReadFloat((CHAR**)&pc_Cur);

                    pParam->b_BackAndForth = LOA_ReadULong((CHAR**)&pc_Cur);
                }
                break;
            }
        }
    }

    return (pc_Cur - (UCHAR*)_pc_Buffer);
}

#ifdef ACTIVE_EDITORS

/*
=======================================================================================================================
=======================================================================================================================
*/
void GAO_ModifierAnimatedGAO_Save(MDF_tdst_Modifier *_pst_Mod)
{	
	GAO_tdst_ModifierAnimatedGAO *p_AnimatedGAO;
    ULONG	ul_Key;
	int iArg;
    ULONG	ul_Version = 2;
    SAV_Buffer(&ul_Version, 4);

    p_AnimatedGAO = (GAO_tdst_ModifierAnimatedGAO *) _pst_Mod->p_Data;

    // make sure we save the object at its initial position / rotation / scale
    if(!(_pst_Mod->ul_Flags & MDF_C_Modifier_Inactive))
        GAO_ModifierAnimatedGAO_Reinit(_pst_Mod);

    SAV_Buffer(&p_AnimatedGAO->ulRawFlags, 4);

    if(p_AnimatedGAO->p_GAOReference)
        ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) p_AnimatedGAO->p_GAOReference);
    else
        ul_Key = BIG_C_InvalidKey;
    SAV_Buffer(&ul_Key, 4);

    SAV_Buffer(&p_AnimatedGAO->vOffset, sizeof(MATH_tdst_Vector));

    for(iArg=0; iArg<9; iArg++)
    {
        ULONG ulFlag = 1<<((iArg+1)+(iArg/3));
        if(p_AnimatedGAO->pParams[iArg] && (p_AnimatedGAO->ulRawFlags & ulFlag))
        {
            SAV_Buffer(&p_AnimatedGAO->eAnimationTypes[iArg], 4);

            switch(p_AnimatedGAO->eAnimationTypes[iArg])
            {
            case MDF_AnimatedGAO_Linear:
                {
                    GAO_tdst_ModifierAnimatedGAOLinearParams *pParam = (GAO_tdst_ModifierAnimatedGAOLinearParams *)p_AnimatedGAO->pParams[iArg];
                    SAV_Buffer(&pParam->f_Min, 4);
                    SAV_Buffer(&pParam->f_Max, 4);
                    SAV_Buffer(&pParam->f_StartTime, 4);
                    SAV_Buffer(&pParam->f_StopTime, 4);
                    SAV_Buffer(&pParam->f_TotalTime, 4);
                    SAV_Buffer(&pParam->b_BackAndForth, 4);
                }
                break;
            case MDF_AnimatedGAO_Noise:
                {
                    GAO_tdst_ModifierAnimatedGAONoiseParams *pParam = (GAO_tdst_ModifierAnimatedGAONoiseParams *)p_AnimatedGAO->pParams[iArg];
                    SAV_Buffer(&pParam->f_Min, 4);
                    SAV_Buffer(&pParam->f_Max, 4);
                    SAV_Buffer(&pParam->f_Var, 4);
                    SAV_Buffer(&pParam->f_VarSpeed, 4);
                }
                break;
            case MDF_AnimatedGAO_Sinus:
                {
                    GAO_tdst_ModifierAnimatedGAOSinusParams *pParam = (GAO_tdst_ModifierAnimatedGAOSinusParams *)p_AnimatedGAO->pParams[iArg];
                    SAV_Buffer(&pParam->f_Angle, 4);
                    SAV_Buffer(&pParam->f_TotalTime, 4);
                    SAV_Buffer(&pParam->f_TimeBias, 4);
                    SAV_Buffer(&pParam->b_BackAndForth, 4);
                }
                break;
            }
        }
    }
}

#endif
//#endif //JADEFUSION

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
