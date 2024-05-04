/*$T ANIrender.c GC! 1.081 11/12/01 18:07:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "GraphicDK/Sources/GRObject/GROrender.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "Engine/Sources/Modifier/MDFstruct.h"
#ifdef ACTIVE_EDITORS
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
#include "GraphicDK/Sources/GRObject/GROrender.h"
#include "GraphicDK/Sources/SOFT/SOFTHelper.h"
#include "GraphicDK/Sources/SELection/Selection.h"
#include "GraphicDK/Sources/GEOmetric/GEOboundingvolume.h"
#include "float.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#include "BASe/BENch/BENch.h"

/* external / prototypes */
extern void ANI_PlayBlendAnimationWithoutMagicBox(OBJ_tdst_GameObject *, ANI_st_GameObjectAnim *, ANI_tdst_Anim *);
extern BOOL sgb_EngineRender;
extern BOOL GRO_gb_ResetGhost;
extern bool ENG_gb_ANIRunning;

ULONG				PushedStack = 0;
OBJ_tdst_GameObject *pst_RenderingStack[256];
ULONG				AI_C_Callback = 0;


#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Render the Hierarchy Links in ACTIVE_EDITORS
 =======================================================================================================================
 */
void ANI_RenderHierarchyLinks(OBJ_tdst_GameObject *_pst_BoneGO)
{
	if
	(
		(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DisplayHierarchy)
	&&	(OBJ_b_TestIdentityFlag(_pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy))
	)
	{
		/*~~~~~~~~~~~~~~~~~~~~*/
		WAY_tdst_Link	st_Link;
		LONG			l;
		/*~~~~~~~~~~~~~~~~~~~~*/

		if(SEL_RetrieveItemWithType(GDI_gpst_CurDD->pst_World->pst_Selection, _pst_BoneGO, SEL_C_SIF_HieLink))
			l = OBJ_C_EditFlags_Selected;
		else
			l = 0;

		st_Link.pst_Next = _pst_BoneGO;
		WAY_Link_Render(_pst_BoneGO->pst_Base->pst_Hierarchy->pst_Father, &st_Link, l, 1, 0);
	}
}

/*$off*/
/*
 =======================================================================================================================
	Aim: Render the BV of the Bone if needed.
 =======================================================================================================================
 */
void ANI_RenderBoneBV(OBJ_tdst_GameObject *_pst_BoneGO)
{
	GDI_gpst_CurDD->ul_DisplayFlags &= ~GDI_Cul_DF_ShowCurrentBV;

	if(!(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DoNotRender))
	{
		if(_pst_BoneGO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
		{
			if(SOFT_l_Helpers_MoversAreVisible(GDI_gpst_CurDD->pst_Helpers))
			{
				if((void *) _pst_BoneGO != SEL_pst_GetFirstItem(GDI_gpst_CurDD->pst_World->pst_Selection, SEL_C_SIF_Object))
					GDI_gpst_CurDD->ul_DisplayFlags |= GDI_Cul_DF_ShowCurrentBV;
			}
			else
				GDI_gpst_CurDD->ul_DisplayFlags |= GDI_Cul_DF_ShowCurrentBV;

			if(GDI_gpst_CurDD->uc_EditBounding) GDI_gpst_CurDD->ul_DisplayFlags |= GDI_Cul_DF_ShowCurrentBV;
		}

		if ( (_pst_BoneGO->ul_EditorFlags & OBJ_C_EditFlags_ShowBV) || !(GDI_gpst_CurDD->ul_CurrentDrawMask & GDI_Cul_DM_DontShowBV) )
			GDI_gpst_CurDD->ul_DisplayFlags |= GDI_Cul_DF_ShowCurrentBV;

		if(GDI_gpst_CurDD->ul_DisplayFlags & (GDI_Cul_DF_ShowCurrentBV))
		{
			/*--------------------------------*/
			MATH_tdst_Matrix	st_TempMatrix ONLY_PSX2_ALIGNED(16);
			/*--------------------------------*/

			MATH_CopyMatrix(&st_TempMatrix, _pst_BoneGO->pst_GlobalMatrix);

			OBJ_ComputeGlobalWithLocal(_pst_BoneGO, _pst_BoneGO->pst_GlobalMatrix, 1);
			GEO_BoundingVolume_Display(GDI_gpst_CurDD, _pst_BoneGO);

			MATH_CopyMatrix(_pst_BoneGO->pst_GlobalMatrix, &st_TempMatrix);
		}
#ifdef JADEFUSION
        // Draw possible collision object helpers
        GEO_Zone_Display( GDI_gpst_CurDD, _pst_BoneGO );
#endif
	}

	GDI_gpst_CurDD->ul_DisplayFlags &= ~GDI_Cul_DF_ShowCurrentBV;
}

#endif
/*$on*/

/*
 =======================================================================================================================
    Aim:    Computes the Bone Absolute Matrix.
 =======================================================================================================================
 */
void ANI_ComputeBoneMatrix
(
	OBJ_tdst_GameObject *_pst_BoneGO,
	OBJ_tdst_GameObject *_pst_RefGO,
	MATH_tdst_Matrix	*_pst_BoneGizmo,
	BOOL				_b_Update
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_BoneFather;
	MATH_tdst_Matrix	*pst_RefMatrix, *pst_FatherGizmo;
	ULONG				ul_FatherID;
	MATH_tdst_Vector	st_RefScale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_RefMatrix = OBJ_pst_GetAbsoluteMatrix(_pst_RefGO);
	MATH_GetScale(&st_RefScale, pst_RefMatrix);

	if(OBJ_pst_GetFather(_pst_BoneGO) == _pst_RefGO)
	{
		/* Main bone case. */
		MATH_CopyMatrix(_pst_BoneGizmo, OBJ_pst_GetLocalMatrix(_pst_BoneGO));
	}
	else
	{
		if((OBJ_b_TestIdentityFlag(_pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy)) && (pst_BoneFather = OBJ_pst_GetFather(_pst_BoneGO)) )
		{
			MATH_tdst_Vector	st_Scale;
			MATH_tdst_Vector	st_Scale2;

			ul_FatherID = TAB_ul_PFtable_GetElemIndexWithPointer
				(
					_pst_RefGO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects,
					pst_BoneFather
				);
			pst_FatherGizmo = OBJ_pst_GetGizmo(_pst_RefGO, ul_FatherID, FALSE);

			MATH_GetScale(&st_Scale, pst_FatherGizmo);
			MATH_GetScale(&st_Scale2, OBJ_pst_GetLocalMatrix(_pst_BoneGO));

			st_Scale.x *= st_Scale2.x;
			st_Scale.y *= st_Scale2.y;
			st_Scale.z *= st_Scale2.z;


			MATH_MulMatrixMatrix(_pst_BoneGizmo, OBJ_pst_GetLocalMatrix(_pst_BoneGO), pst_FatherGizmo);

			MATH_Orthonormalize(_pst_BoneGizmo);
			MATH_SetScale(_pst_BoneGizmo, &st_Scale);


		}
		else
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix st_InvRefGOMatrix	ONLY_PSX2_ALIGNED(16);
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_InvertMatrix(&st_InvRefGOMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_RefGO));
			MATH_MulMatrixMatrix(_pst_BoneGizmo, OBJ_pst_GetAbsoluteMatrix(_pst_BoneGO), &st_InvRefGOMatrix);//*/
		}
	}

	if(_b_Update && !(OBJ_b_TestControlFlag(_pst_BoneGO, OBJ_C_ControlFlag_AnimDontTouch)))
	{
		MATH_MulMatrixMatrix(OBJ_pst_GetAbsoluteMatrix(_pst_BoneGO), _pst_BoneGizmo, pst_RefMatrix);
		if(MATH_b_TestScaleType(_pst_BoneGO->pst_GlobalMatrix)) 
			MATH_Orthonormalize(_pst_BoneGO->pst_GlobalMatrix);
	}
}

/*
 =======================================================================================================================
    Aim:    Render one bone.
 =======================================================================================================================
 */ 
_inline_ void ANI_RenderBone(OBJ_tdst_GameObject *_pst_BoneGO)
{
	if(!(OBJ_b_TestControlFlag(_pst_BoneGO, OBJ_C_ControlFlag_ForceInvisible)))
	{
		if(OBJ_b_TestIdentityFlag(_pst_BoneGO, OBJ_C_IdentityFlag_Visu)) 
		{
			pst_RenderingStack[PushedStack++] = _pst_BoneGO;
		}
#ifdef ACTIVE_EDITORS
		else
		{
			GRO_RenderGro(_pst_BoneGO, &GRO_gst_Unknown);
		}
#endif
	}

#ifdef ACTIVE_EDITORS

	/* ANI_RenderHierarchyLinks(_pst_BoneGO); */
	ANI_RenderBoneBV(_pst_BoneGO);
#endif
}

extern void ANI_PlayLeadAnimation(OBJ_tdst_GameObject *, ANI_st_GameObjectAnim *, ANI_tdst_Anim *);

extern void AI_EvalFunc_MATHVecBlendRotate_C
(
	MATH_tdst_Vector	*,
	MATH_tdst_Vector	*,
	float				,
	MATH_tdst_Vector	*
);


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_BeforeRender(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	MATH_tdst_Matrix		*M, *pst_BoneGizmo;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	ANI_tdst_Anim			*pst_Anim;
	OBJ_tdst_Group			*pst_Skeleton;
	OBJ_tdst_GameObject		*pst_BoneGO;
	ULONG					i, ul_Anim;
	float					f_Time, f_Blend;
	UCHAR					uc_AnimUsed;
	MATH_tdst_Matrix		st_TempMat[64] ONLY_PSX2_ALIGNED(16);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_Skeleton = pst_GOAnim->pst_Skeleton;
	if(!pst_Skeleton) return;

	/*  ------------------------------------------------------------------------*/
	/*  ------------------------- Blend Update ---------------------------------*/
	/*  ------------------------------------------------------------------------*/
	ul_Anim = 0;
	uc_AnimUsed = pst_GOAnim->uc_AnimUsed & 0xF;
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	
	while(uc_AnimUsed)
	{
		if((uc_AnimUsed & 1) == 0)
		{
			LINK_PrintStatusMsg("[Compute Blend] Cas étrange ... Ask Ker");
			uc_AnimUsed >>= 1;
			ul_Anim++;
			continue;
		}

		pst_Anim = pst_GOAnim->apst_Anim[ul_Anim];

		if(ul_Anim)
		{
			f_Time = TIM_gf_dt;

			if(pst_Anim->uc_MiscFlags & ANI_C_MiscFlag_UseTimeInParam1)
			{
				TIM_gf_dt = *(float *) &pst_Anim->pst_Data->pst_ListParam[1].i_Param1;
				pst_Anim->uc_MiscFlags -= ANI_C_MiscFlag_UseTimeInParam1;
			}

			if(pst_Anim->uc_BlendFlags & ACT_C_TF_BlendFreezeBones)
			{
				TIM_gf_dt = 0.0f;
			}

			/*
			 * If the following Test is true, we have just decided to set a new Action in IA
			 * that has caused a Blend. IA Module is called AFTER ANI Module. So the Blend
			 * hasnt been computed and there is no "desynchronisation" between MagicBox Tracks
			 * and MainBone Tracks. We compute this "desynchronization" to determine if we
			 * have to move the bones which is the classical Blend procedure. This Test is
			 * true each frame the IA has called a new ACT_ActionSet.
			 */
			if(TIM_gf_dt)
			{
				/*~~~~~~~~~~~~~~~~~~*/
				float	f_Cur0, f_Cur;
				float	f_Save;
				/*~~~~~~~~~~~~~~~~~~*/
				
#ifdef ANIMS_USE_ARAM
				/* If data is not in Cache (and should be), Load it From ARAM */
				{
					ACT_st_ActionKit		*pst_ActionKit;
					EVE_tdst_ListTracks		*pst_TrackList;
	
					pst_ActionKit = pst_GOAnim->pst_ActionKit;	
					pst_TrackList = pst_Anim->pst_Data->pst_ListTracks;
					if((pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM))
						ACT_i_Cache_LoadAnim(pst_ActionKit, pst_TrackList);
				}
#endif
				f_Save = TIM_gf_dt;
				EVE_GetTracksTime(pst_Anim->pst_Data, 2, &f_Cur, NULL);
				EVE_GetTracksTime(pst_Anim->pst_Data, 0, &f_Cur0, NULL);
				TIM_gf_dt = f_Cur0 - f_Cur;
#ifdef JADEFUSION
				if ((TIM_gf_dt > 0.0f) && ((float) pst_Anim->uc_AnimFrequency > 0.0f))
                {
                    TIM_gf_dt = (TIM_gf_dt * 60.0F) /((float) pst_Anim->uc_AnimFrequency) ;
				    if(TIM_gf_dt < 1E-3f) TIM_gf_dt = 0.0f;
                }
                else
                {
                    TIM_gf_dt = 0.0f;
                }
#else
				TIM_gf_dt = (TIM_gf_dt * 60.0F) /((float) pst_Anim->uc_AnimFrequency) ;
				if(TIM_gf_dt < 1E-3f) TIM_gf_dt = 0.0f;
#endif
			}

			ANI_PlayBlendAnimationWithoutMagicBox(_pst_GO, pst_GOAnim, pst_Anim);

			TIM_gf_dt = f_Time;
		}

		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);

		for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
		{
			if((uc_AnimUsed & 1) && (i >= (UCHAR) _pst_GO->pst_Base->pst_AddMatrix->l_Number)) break;

			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));
			pst_BoneGizmo = &(_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo + i)->st_Matrix;
			M = OBJ_pst_GetLocalMatrix(pst_BoneGO);

			if(M)
			{
				if(ul_Anim)
				{
//					if(!(pst_Anim->uc_BlendFlags & ACT_C_TF_BlendShowResult))
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
						MATH_tdst_Vector	st_Scale1, st_Scale2;
						MATH_tdst_Matrix	st_TempM;
						BOOL				b_Scale;
//						float				f_ModifiedBlend;
//						float				f_LastFramePercent;
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

						f_Blend = *(float *) &pst_Anim->pst_Data->pst_ListParam->i_Param1;

						b_Scale = FALSE;
						if(MATH_b_TestScaleType(M) && MATH_b_TestScaleType(st_TempMat + i))
						{
							MATH_GetScale(&st_Scale1, M);
							MATH_GetScale(&st_Scale2, st_TempMat + i);
							if
							(
								(fEqWithEpsilon(st_Scale1.x, st_Scale2.x, (float) 1E-5))
							&&	(fEqWithEpsilon(st_Scale1.y, st_Scale2.y, (float) 1E-5))
							&&	(fEqWithEpsilon(st_Scale1.z, st_Scale2.z, (float) 1E-5))
							)	b_Scale = TRUE;

#ifdef ACTIVE_EDITORS
							if(!b_Scale) 
							{
								LINK_PrintStatusMsg("Problem in ANI_ComputeBlend with scaled bones.");
							}
#endif
						}

						/*
						f_LastFramePercent = f_Blend - (TIM_gf_dt / (pst_Anim->uc_BlendTime  * 0.0166666f));
						if((f_LastFramePercent <= 0.0f) || (f_LastFramePercent >= 1.0f))
							f_ModifiedBlend = f_Blend;
						else
							f_ModifiedBlend = (f_Blend - f_LastFramePercent) / (1.0f - f_LastFramePercent);
						*/


						if(pst_BoneGO && pst_BoneGO->pst_Extended && (pst_BoneGO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_NoBlendBone))
						{
							MATH_CopyMatrix(M, st_TempMat + i);
						}
						else
						{
							MATH_MatrixBlend(st_TempMat + i, M, st_TempMat + i, f_Blend, 1);

							if(b_Scale)
							{
								MATH_SetScale(st_TempMat + i, &st_Scale1);
							}


							MATH_CopyMatrix(&st_TempM, st_TempMat + i);

							if(pst_BoneGO && pst_BoneGO->pst_Extended && (pst_BoneGO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_VecBlendRot))
								AI_EvalFunc_MATHVecBlendRotate_C(&M->T, &(st_TempMat + i)->T, f_Blend, &st_TempM.T);
							else
								MATH_BlendVector(&st_TempM.T, &M->T, &(st_TempMat + i)->T, f_Blend);

							MATH_CopyMatrix(M, &st_TempM);
						}
					}
				}
				else
				{
					MATH_CopyMatrix(st_TempMat + i, M);
				}
			}
		}

		uc_AnimUsed >>= 1;
		ul_Anim++;
	}

	ANI_PartialAnim_PlayAll(_pst_GO, TIM_gf_dt);

	/* Stock Matrixes */
	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));

		if(!OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy))
			continue;
			
		M = OBJ_pst_GetLocalMatrix(pst_BoneGO);
		MATH_CopyMatrix(&pst_GOAnim->dpst_Stock[i], M);
	}


	/*$2- CB IA ------------------------------------------------------------------------------------------------------*/
		AI_C_Callback = 1;
		AI_ExecCallback(_pst_GO, AI_C_Callback_AfterBlend);
		AI_C_Callback = 0;
	/*$2--------------------------------------------------------------------------------------------------------------*/
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ANI_BeforeRendering(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentElem;
	TAB_tdst_PFelem			*pst_EndElem;
	OBJ_tdst_GameObject		*pst_GO;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_Group			*pst_Skeleton;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(!pst_GO->pst_Extended || !(pst_GO->pst_Extended->uw_ExtraFlags & OBJ_C_ExtraFlag_AlwaysPlay))
		{
			if(pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled) continue;
		}
		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) continue;

		pst_GOAnim = pst_GO->pst_Base->pst_GameObjectAnim;	
		pst_Skeleton = pst_GOAnim->pst_Skeleton;
		if(!pst_Skeleton) continue;

		ANI_BeforeRender(pst_GO);
	}
}

/*
 =======================================================================================================================
    Aim:    Render an GameObject that has gizmos, with or without an anim.
 =======================================================================================================================
 */
void ANI_Render(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	MATH_tdst_Matrix		st_SaveMat ONLY_PSX2_ALIGNED(16);
	ANI_st_GameObjectAnim	*pst_GOAnim;
	ANI_tdst_Anim			*pst_Anim;
	OBJ_tdst_Group			*pst_Skeleton;
	OBJ_tdst_GameObject		*pst_BoneGO;
	WOR_tdst_View			*pst_View;
	MATH_tdst_Vector		*pst_CameraPosition;
	ULONG					i;
	BOOL					b_SkinBone, b_Anim;
	MATH_tdst_Matrix		*pst_BoneGizmo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_GSP_BeginRaster(5);

	b_SkinBone = FALSE;
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	pst_View = GDI_gpst_CurDD->pst_World->pst_View;
	pst_CameraPosition = WOR_View_GetAbsolutePosition(pst_View);

	pst_Anim = pst_GOAnim->apst_Anim[0];
	pst_Skeleton = pst_GOAnim->pst_Skeleton;

	b_Anim = pst_Anim ? TRUE : FALSE;


	/* Synchronise Anim tracks if needed and return */
	/* The GameObject was culled and only its MagicBox tracks were played to move it */
	/* Now, the GameObject reappears. We synchronise its tracks and return. No blend needed there */
	if(!(pst_GOAnim->uc_Synchro) && pst_GOAnim->apst_Anim[0])
	{
		/*~~~~~~~~~~~~~~~~~~~~~~*/
		float			f_Time;
		EVE_tdst_Data	*pst_Data;
		/*~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Data = pst_GOAnim->apst_Anim[0]->pst_Data;
		EVE_GetTracksTime(pst_Data, 0, &f_Time, NULL);
		EVE_SetTracksTime(pst_Data, f_Time);

		/* If we are in a Blend. We stop it. */
		if(pst_GOAnim->uc_AnimUsed != 1) pst_GOAnim->uc_AnimUsed = 1;

		pst_GOAnim->uc_Synchro = 1;

		f_Time = TIM_gf_dt;
		TIM_gf_dt = 0.0f;
		ANI_PlayLeadAnimation(_pst_GO, pst_GOAnim, pst_GOAnim->apst_Anim[0]);
		TIM_gf_dt = f_Time;

	/*$2- CB IA ------------------------------------------------------------------------------------------------------*/
		AI_C_Callback = 1;
		AI_ExecCallback(_pst_GO, AI_C_Callback_AfterBlend);
		AI_C_Callback = 0;
	/*$2--------------------------------------------------------------------------------------------------------------*/

	}

	if(!pst_Skeleton)
	{
		_pst_GO->pst_Base->pst_GameObjectAnim->uc_Signal = 0;
		_GSP_EndRaster(5);
		return;
	}

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
	{
		if((b_Anim) && (i >= (UCHAR) _pst_GO->pst_Base->pst_AddMatrix->l_Number)) break;

		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		pst_BoneGizmo = &(_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo + i)->st_Matrix;

#ifdef ACTIVE_EDITORS
		if(ENG_gb_EngineRunning)
#endif
		if((pst_BoneGO->pst_Extended) && (pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (pst_BoneGO->pst_Extended->pst_Modifiers))
		{
			MDF_ApplyAllGao(pst_BoneGO);
			MDF_UnApplyAllGao(pst_BoneGO);
		}

#ifdef ACTIVE_EDITORS
//		if(ENG_gb_EngineRunning)
#endif
			ANI_ComputeBoneMatrix(pst_BoneGO, _pst_GO, pst_BoneGizmo, TRUE);


#ifdef ACTIVE_EDITORS
		if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Ghost && !GRO_gb_ResetGhost) pst_BoneGO->ul_EditorFlags |= OBJ_C_EditFlags_Ghost;
		if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GhostODE && !GRO_gb_ResetGhost) pst_BoneGO->ul_EditorFlags |= OBJ_C_EditFlags_GhostODE;
		if(GRO_gb_ResetGhost) 
		{
			pst_BoneGO->ul_EditorFlags &= ~OBJ_C_EditFlags_Ghost;
			pst_BoneGO->ul_EditorFlags &= ~OBJ_C_EditFlags_GhostODE;
		}
#endif
		pst_BoneGO->uc_LOD_Vis = _pst_GO->uc_LOD_Vis;
		pst_BoneGO->ul_StatusAndControlFlags &= ~OBJ_C_StatusFlag_RTL;
		pst_BoneGO->ul_StatusAndControlFlags |= _pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_RTL;
		pst_BoneGO->pst_Base->pst_AddMaterial = _pst_GO->pst_Base->pst_AddMaterial;
#ifdef ACTIVE_EDITORS
		if(!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_HideSkin))
#endif
			ANI_RenderBone(pst_BoneGO);
	}

	_GSP_EndRaster(5);
	if (PushedStack) 
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_GameObject		*pst_LocalGO;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef JADEFUSION
        GDI_gpst_CurDD->pst_CurrentAnim = _pst_GO;
#endif
		while (PushedStack--)	
		{
			pst_LocalGO = pst_RenderingStack[PushedStack];
			if(pst_LocalGO->pst_Base)
			{
				if(pst_LocalGO->pst_Base->pst_AddMatrix && (pst_LocalGO->pst_Base->pst_AddMatrix->l_Number > 1))
				{
					/* Skining algo. need to have GO Ref Matrix in Skinned GO Absolute matrix */
					MATH_CopyMatrix(&st_SaveMat, OBJ_pst_GetAbsoluteMatrix(pst_LocalGO));
					MATH_CopyMatrix(OBJ_pst_GetAbsoluteMatrix(pst_LocalGO), OBJ_pst_GetAbsoluteMatrix(_pst_GO));
#ifdef JADEFUSION
					// For skinned animated objects, match real-time lights to the gao bounding volume
                    // instead of the element mesh bounding volume
                    ULONG ulOldFlags = GDI_gpst_CurDD->ul_LightCullingFlags;
                    GDI_gpst_CurDD->ul_LightCullingFlags &= ~GDI_Cul_Light_Cull_Element;
#endif
					GRO_Render(pst_LocalGO);
#ifdef JADEFUSION
                    GDI_gpst_CurDD->ul_LightCullingFlags = ulOldFlags;
#endif
					MATH_CopyMatrix(OBJ_pst_GetAbsoluteMatrix(pst_LocalGO), &st_SaveMat);
				} 
				else 
					GRO_Render(pst_LocalGO);
			}

#ifdef ACTIVE_EDITORS
				pst_LocalGO->ul_EditorFlags &= ~OBJ_C_EditFlags_Ghost;
				pst_LocalGO->ul_EditorFlags &= ~OBJ_C_EditFlags_GhostODE;
#endif

		}
		PushedStack = 0;
#ifdef JADEFUSION
		GDI_gpst_CurDD->pst_CurrentAnim = NULL;
#endif
	}

		_pst_GO->pst_Base->pst_GameObjectAnim->uc_Signal = 0;

#ifdef ACTIVE_EDITORS
	if(GRO_gb_ResetGhost) _pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_Ghost;
#endif
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif

