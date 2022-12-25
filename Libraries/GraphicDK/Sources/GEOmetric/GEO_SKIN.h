/*$T GEO_SKIN.h GC! 1.081 05/02/02 10:59:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __GEO_SKIN__
#define __GEO_SKIN__
#include "BASe/BAStypes.h"

#include "GRObject/GROstruct.h"

#ifndef PSX2_TARGET
#pragma once
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#define GEO_SKN_MAXNUMBEROFMATRIX	100	/* Used internaly,for computations. You can change it here, no problems */

/* Engine functions */
void						GEO_SKN_DestroyObjPonderation(GEO_tdst_ObjectPonderation *);				/* OK */
GEO_tdst_ObjectPonderation	*GEO_SKN_Duplicate(GEO_tdst_ObjectPonderation *);							/* OK */
ULONG						GEO_SKN_Load(char *, GEO_tdst_Object *);									/* OK */
#ifdef ACTIVE_EDITORS
void						GEO_SKN_Save(GEO_tdst_Object *);											/* OK */
#endif
void						GEO_SKN_Compute4Display(struct OBJ_tdst_GameObject_ *, GEO_tdst_Object *);	/* ~OK */
void						GEO_SKN_ComputeNormals(GEO_tdst_Object *, GEO_Vertex *);

#if defined(ACTIVE_EDITORS) && defined(_XENON_RENDER)
void GEO_SKN_ComputeTangents(GEO_tdst_Object* _pst_Geo, GEO_Vertex* _pst_Points, GRO_tdst_Visu * _pst_Visu );
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ ULONG GEO_SKN_IsSkinned(GEO_tdst_Object *pst_Object)
{
	if(pst_Object->p_SKN_Objectponderation == NULL) return 0;
	return 1;
}

/* OK */
GEO_tdst_ObjectPonderation	*GEO_SKN_CreateObjPonderation
							(
								GEO_tdst_Object *,
								ULONG NumberOfMatrix,
								ULONG NumberOfPoints
							);	/* OK */

#ifdef ACTIVE_EDITORS

/* Creation - modification */
void	GEO_SKN_SetNumberOfMatrix(GEO_tdst_Object *, ULONG ulNubrOfMatrix);					/* OK */
void	GEO_SKN_UpdateAfterVertexEdition(GEO_tdst_Object *, struct WOR_tdst_Update_RLI_ *); /* OK */

void	GEO_SKN_FlashGizmo(OBJ_tdst_GameObject *, GEO_tdst_Object *, ULONG ulNum2Flhs);		/* OK */
void	GEO_SKN_RestoreGizmo(OBJ_tdst_GameObject *, GEO_tdst_Object *, ULONG ulNum2Flhs);	/* OK */
void	GEO_SKN_R_180_Gizmo(OBJ_tdst_GameObject *, GEO_tdst_Object *, ULONG ulNum2Flhs);	/* OK */

/* push - pop */
void	GEO_SKN_Push(GEO_tdst_Object *);													/* OK */
void	GEO_SKN_Pop(GEO_tdst_Object *);														/* OK */
void	GEO_SKN_REDO(GEO_tdst_Object *);													/* OK */
ULONG	GEO_SKN_Get_UnRe_Sate(GEO_tdst_Object *);											/* OK */

/* Compress - Expand */
ULONG	GEO_SKN_IsExpanded(GEO_tdst_Object *);												/* OK */
void	GEO_SKN_Compress_VP(GEO_tdst_VertexPonderationList *);								/* OK */
void	GEO_SKN_Expand_VP(GEO_tdst_VertexPonderationList *, ULONG ulNumberOfPoint);			/* OK */
void	GEO_SKN_Compress(GEO_tdst_Object *);												/* OK */
void	GEO_SKN_Expand(GEO_tdst_Object *);													/* OK */

/* Actions flags & function */
#define SKN_Cul_UseSubSel	1
#define SKN_Cul_UseStraight 2
#define SKN_Cul_InvertSel	4
void	GEO_SKN_PopedSelBend(GEO_tdst_Object *, float fRadius, ULONG ulFlags);								/* OK */
void	GEO_SKN_CopySelBend(GEO_tdst_Object *, float fRadius, ULONG ulFlags);								/* OK */
void	GEO_SKN_Paint(GEO_tdst_Object *, ULONG *, float *, ULONG ulFlags);									/* OK */
void	GEO_SKN_Paint_ADD(GEO_tdst_Object *, ULONG *, float *, ULONG ulFlags);								/* OK */
void	GEO_SKN_Paint_SUB(GEO_tdst_Object *, ULONG *, float *, ULONG ulFlags);								/* OK */
void	GEO_SKN_Reset(GEO_tdst_Object *, float Value, ULONG ulFlags);										/* OK */
void	GEO_SKN_Smooth(GEO_tdst_Object *, float fRadius, ULONG ulFlags);									/* later */
void	GEO_SKN_RemoveIsolated(GEO_tdst_Object *, ULONG ulFlags);											/* later */
void	GEO_SKN_ForceNMatrixMax(GEO_tdst_Object *, ULONG NumberOfMaxMatrix, ULONG ulFlags);					/* OK */
void	GEO_SKN_ForceLimitMin(GEO_tdst_Object *, float LimitMin, ULONG ulFlags);							/* OK */
void	GEO_SKN_Contrast(GEO_tdst_Object *, float f_Contrast, ULONG ulFlags);								/* OK */
void	GEO_SKN_Normalize(GEO_tdst_Object *, ULONG ulFlags);												/* OK */
void	GEO_SKN_SelectGizmo(GEO_tdst_Object *, ULONG Gizmo, float thresh, ULONG ulFlags);					/* OK */
void	GEO_SKN_SwitchGizmo(GEO_tdst_Object *, ULONG Gizmo1, ULONG Gizmo2, ULONG ulFlags);					/* OK */
void	GEO_SKN_Skin2Colors(GEO_tdst_Object *, ULONG ulR, ULONG ulG, ULONG ulB, ULONG ulFlags);				/* OK */
void	GEO_SKN_Colors2Skin(GEO_tdst_Object *, ULONG ulR, ULONG ulG, ULONG ulB, ULONG ulFlags);				/* OK */
void	GEO_SKN_ExpandGizmo(GEO_tdst_Object *, ULONG Gizmo, float thresh, ULONG ulFlags);					/* OK */
void	GEO_SKN_InsertGizmo(GEO_tdst_Object *, ULONG Gizmo, ULONG A, ULONG B, float thresh, ULONG ulFlags); /* OK */

void	GEO_SKN_Deduct_Proxy(OBJ_tdst_GameObject *, GEO_tdst_Object *, ULONG ulFlag);						/* OK */
void	GEO_SKN_Symetrise_X(OBJ_tdst_GameObject *, GEO_tdst_Object *, ULONG ulFlag);						/* OK */

/* Editor - Skin interactions */
ULONG	GEO_SKN_GetInfoAboutSelected(GEO_tdst_Object *, ULONG *, float *, ULONG ulFlag);					/* OK */
void	GEO_SKN_UpdateSkinSelection(GEO_tdst_Object *);														/* OK */

void	GEO_SKN_AdaptToAnotherSkin
		(
			struct OBJ_tdst_GameObject_ *,
			struct OBJ_tdst_GameObject_ *,
			GEO_tdst_Object *,
			GEO_tdst_Object *,
			char,
			MATH_tdst_Matrix *,
			MATH_tdst_Matrix *
		);

#endif /* ACTIVE_EDITORS */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEO_SKIN__ */
