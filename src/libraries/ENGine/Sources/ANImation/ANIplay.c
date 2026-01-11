/*$T ANIplay.c GC! 1.081 06/18/01 18:03:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/BASsys.h"

#include "MATHs/MATH.h"

#include "ANImation/ANIstruct.h"
#include "ANImation/ANIaccess.h"
#include "ANImation/ANIinit.h"
#include "ANImation/ANIplay.h"
#include "ANImation/ANImain.h"

#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"

#include "ACTions/ACTstruct.h"
#include "ACTions/ACTcompute.h"

#include "OBJects/OBJstruct.h"
#include "OBJects/OBJconst.h"
#include "OBJects/OBJorient.h"
#include "OBJects/OBJinit.h"
#include "OBJects/OBJmain.h"

#ifdef _GAMECUBE
#include "SDK/Sources/GameCube/GC_arammng.h"
#include "SDK/Sources/GameCube/GC_aramheap.h"
#endif


#include <float.h>
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern EVE_tdst_ListTracks	*EVE_gpst_CurrentListTracks;

MATH_tdst_Vector			ANI_gst_PositionBeforeAnimPlayer;

#define ANI_b_PlayAfterEndOfAnim(anim) \
		( \
			((anim)->uc_MiscFlags & ANI_C_MiscFlag_MagicBox_AfterEnd) \
		||	((anim)->uc_MiscFlags & ANI_C_MiscFlag_MagicBox_Interpolation) \
		)

/*$F
	-----------------------------------------------
	ANI_ChangeAction()
	ANI_UpdateFrequency()
	-----------------------------------------------
	ANI_GetEndSpeedOfMB()
	ANI_InterpolateMagicBoxPositionAfterEndOfAnim()
	ANI_b_ComputeMagicBoxPositionForBlendAnim()
	ANI_b_BlendMagicBox()
	-----------------------------------------------
	ANI_PlayBlendAnimationWithoutMagicBox()
	-----------------------------------------------
	ANI_PlayLeadAnimation()
	ANI_PlayGameObjectAnimation()
	-----------------------------------------------
*/


/*
 =======================================================================================================================
    Aim:    We arrived at the end of the Lead anim. We compute the next one to play.
 =======================================================================================================================
 */
void ANI_ChangePartialAction(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ANI_st_GameObjectAnim *_pst_GOAnim;
	int					iActionItemIndex;
	ACT_st_Action		*pst_PartialAction;
	ACT_st_ActionItem	*pst_ActionItem;
	UCHAR				uc_Flag;
	ULONG				ul_DynaFlag;
	DYN_tdst_Dyna		*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!_pst_GO
	||  !(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	) return;

	_pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;

	iActionItemIndex = ACT_C_NoActionItem;
	pst_PartialAction = _pst_GOAnim->pst_CurrentPartialAction;

	if(pst_PartialAction)
	{
		pst_ActionItem = &pst_PartialAction->ast_ActionItem[_pst_GOAnim->uc_CurrentPartialActionItemIndex];
		if((++_pst_GOAnim->uc_CounterForRepetitionPartial) >= pst_ActionItem->uc_Repetition)
		{
			_pst_GOAnim->uc_CounterForRepetitionPartial = 0;

			iActionItemIndex = _pst_GOAnim->uc_CurrentPartialActionItemIndex;
			if((++iActionItemIndex) >= pst_PartialAction->uc_NumberOfActionItem)
			{
				iActionItemIndex = pst_PartialAction->uc_ActionItemNumberForLoop;

				if(iActionItemIndex != ACT_C_NoActionItem)
				{
					if(iActionItemIndex >= pst_PartialAction->uc_NumberOfActionItem)
						iActionItemIndex = pst_PartialAction->uc_NumberOfActionItem - 1;
				}

			}

			if((iActionItemIndex != ACT_C_NoActionItem) && pst_PartialAction->uc_NumberOfActionItem)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~*/
				BOOL	b_SameActionItem;
				/*~~~~~~~~~~~~~~~~~~~~~~~*/

				b_SameActionItem = (_pst_GOAnim->uc_CurrentPartialActionItemIndex == iActionItemIndex);

				_pst_GOAnim->uc_CurrentPartialActionItemIndex = iActionItemIndex;
				pst_ActionItem = &pst_PartialAction->ast_ActionItem[iActionItemIndex];


				ANI_SetTrackList(_pst_GOAnim->apst_PartialAnim[0], pst_ActionItem->pst_TrackList);
				ANI_ChangeShape(_pst_GO, pst_ActionItem->pst_Shape);

				_pst_GOAnim->uc_CounterForRepetitionPartial = 0;

				if(_pst_GOAnim->uc_ForceMode)
					uc_Flag = _pst_GOAnim->uc_ForceMode - 1;
				else
					uc_Flag = pst_ActionItem->uc_Flag;


				/* Set the flags for the dynamics */
				if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Dyna))
				{
					pst_Dyna = _pst_GO->pst_Base->pst_Dyna;

					ul_DynaFlag = DYN_ul_GetDynFlags(pst_Dyna);
					ul_DynaFlag &= ~(DYN_C_IgnoreGravity | DYN_C_IgnoreTraction | DYN_C_IgnoreStream);

					ul_DynaFlag |= (uc_Flag & ACT_C_ActionItemFlag_IgnoreGravity ? DYN_C_IgnoreGravity : 0);
					ul_DynaFlag |= (uc_Flag & ACT_C_ActionItemFlag_IgnoreTraction ? DYN_C_IgnoreTraction : 0);
					ul_DynaFlag |= (uc_Flag & ACT_C_ActionItemFlag_IgnoreStream ? DYN_C_IgnoreStream : 0);

					DYN_SetDynFlags(pst_Dyna, ul_DynaFlag);
				}

				/* Set the mode for the anim */
				_pst_GOAnim->apst_PartialAnim[0]->uw_Flag &= ~ANI_C_AnimFlag_PlayModeMask;
				_pst_GOAnim->apst_PartialAnim[0]->uw_Flag &= ~ANI_C_AnimFlag_DontPlayAnim;
				_pst_GOAnim->apst_PartialAnim[0]->uw_Flag |= (uc_Flag & ANI_C_AnimFlag_PlayModeMask);

				/* If we loop to the same ActionItem and if IA has changed frequency of Animation, we dont want the ANI_ChangeAction to update the frequency */
				if(!b_SameActionItem)
					_pst_GOAnim->apst_PartialAnim[0]->uc_AnimFrequency = (pst_ActionItem->uc_Frequency * 60 / ACT_C_DefaultAnimFrequency);
			}
			else
			{
				/*
				 * The Anim is at its end but we didnt manage to change action. So, we dont change
				 * the Anim but set it as finished.
				 */
				_pst_GOAnim->apst_PartialAnim[0]->uc_BlendTime = _pst_GOAnim->uc_BlendLength_OUT;
				_pst_GOAnim->apst_PartialAnim[0]->uw_BlendCurTime = 0;
				_pst_GOAnim->apst_PartialAnim[0]->uw_Flag |= ANI_C_AnimFlag_DontPlayAnim;
			}
		}
	}
}

static int	Zorglub = 0;
//extern ULONG ACT_ul_ComputeOneACKSize(ACT_st_ActionKit *);
/*
 =======================================================================================================================
    Aim:    We arrived at the end of the Lead anim. We compute the next one to play.
 =======================================================================================================================
 */
void ANI_ChangeAction(OBJ_tdst_GameObject *_pst_GO, ANI_st_GameObjectAnim *_pst_GOAnim)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					iActionItemIndex;
	ACT_st_ActionKit	*pst_ActionKit;
	ACT_st_Action		*pst_Action;
	ACT_st_ActionItem	*pst_ActionItem;
	ACT_tdst_Transition	*pst_Transition;
	ANI_tdst_Anim		*pst_BlendAnim;
	UCHAR				uc_Flag;
	ULONG				ul_DynaFlag;
	DYN_tdst_Dyna		*pst_Dyna;
	float				f_Time, f_Delay;
	USHORT				uw_CurrentEvent, uw_NumEvents;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	iActionItemIndex = ACT_C_NoActionItem;
	pst_Action = _pst_GOAnim->pst_CurrentAction;
	pst_ActionKit = _pst_GOAnim->pst_ActionKit;

	if(pst_Action)
	{
		pst_ActionItem = &pst_Action->ast_ActionItem[_pst_GOAnim->uc_CurrentActionItemIndex];
		if((++_pst_GOAnim->uc_CounterForRepetition) >= pst_ActionItem->uc_Repetition)
		{
			_pst_GOAnim->uc_CounterForRepetition = 0;

			iActionItemIndex = _pst_GOAnim->uc_CurrentActionItemIndex;
			if((++iActionItemIndex) >= pst_Action->uc_NumberOfActionItem)
			{
				if(_pst_GOAnim->pst_Transition)
				{
					/*~~~~~~~~~~~~*/
					USHORT	uw_Next;
					/*~~~~~~~~~~~~*/

					uw_Next = _pst_GOAnim->uw_NextActionIndex;
					_pst_GOAnim->uw_NextActionIndex = (USHORT) - 1;

					ACT_SetAction(_pst_GO, uw_Next, 0, FALSE);
					return;
				}
				else
				{
					iActionItemIndex = pst_Action->uc_ActionItemNumberForLoop;

					if(iActionItemIndex != ACT_C_NoActionItem)
					{
						if(iActionItemIndex >= pst_Action->uc_NumberOfActionItem)
							iActionItemIndex = pst_Action->uc_NumberOfActionItem - 1;
					}
				}
			}

			if((iActionItemIndex != ACT_C_NoActionItem) && pst_Action->uc_NumberOfActionItem)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~*/
				BOOL	b_SameActionItem;
				/*~~~~~~~~~~~~~~~~~~~~~~~*/

				b_SameActionItem = (_pst_GOAnim->uc_CurrentActionItemIndex == iActionItemIndex);

				if(pst_ActionKit->DefaultTrans.uc_Flag & ACT_C_TF_BlendBetweenActionItem)
				{
					pst_Transition = &pst_ActionKit->DefaultTrans;

					/* Compute and store parameters for blend */
					if(pst_Transition->uc_Blend && _pst_GOAnim->apst_Anim[0])
					{
						/* Alloc second anim if not already allocated */
						if(!(_pst_GOAnim->uc_AnimUsed & 2) && !_pst_GOAnim->apst_Anim[1])
							_pst_GOAnim->apst_Anim[1] = ANI_AllocAnim();
						pst_BlendAnim = _pst_GOAnim->apst_Anim[1];
						
						/* There's a blend so anim 2 have to be used */
						_pst_GOAnim->uc_AnimUsed |= 2;

						/* Get all information about old animation and store it into second anim */
						pst_Dyna = (DYN_tdst_Dyna *) pst_BlendAnim->pst_Data;
						L_memcpy(pst_BlendAnim, _pst_GOAnim->apst_Anim[0], sizeof(ANI_tdst_Anim));
						_pst_GOAnim->apst_Anim[0]->pst_Data = (EVE_tdst_Data *) pst_Dyna;
						_pst_GOAnim->apst_Anim[0]->pst_Data->pst_ListTracks = NULL;

						/* Copy flash matrix into second Anim Reference Matrix */
						MATH_CopyMatrix(&pst_BlendAnim->st_Ref, OBJ_pst_GetFlashMatrix(_pst_GO));

						/* Store other parameters : blend time, blended action and action item, blend flags */
						pst_BlendAnim->uc_BlendTime = pst_Transition->uc_Blend;
						pst_BlendAnim->uw_BlendedAction = _pst_GOAnim->uw_CurrentActionIndex;
						pst_BlendAnim->uc_BlendedActionItem = _pst_GOAnim->uc_CurrentActionItemIndex;
						pst_BlendAnim->uc_BlendFlags = pst_Transition->uc_Flag;


						/* Temp STOCK */
						
						pst_BlendAnim->uc_BlendFlags |= ACT_C_TF_BlendStock;
						pst_BlendAnim->uc_BlendFlags &= ~ACT_C_TF_BlendFreezeBones;
						
						/* End Temp STOCK */


						/* Init some other data : current blend time and current flags */
						pst_BlendAnim->uw_BlendCurTime = 256;
						pst_BlendAnim->uc_MiscFlags = 0;

						/* Freeze blended animation if anim have not to be played anymore */
						if(!ANI_b_PlayAnim(pst_BlendAnim))
						{
							pst_BlendAnim->uc_BlendFlags |= ACT_C_TF_BlendFreezeBones;
							pst_BlendAnim->uc_MiscFlags |= ANI_C_MiscFlag_MagicBox_AfterEnd;
						}
						else
						{
							/* Look if Blend Anim is at its very end. */
							uw_CurrentEvent = pst_BlendAnim->pst_Data->pst_ListParam->uw_CurrentEvent;
							uw_NumEvents = pst_BlendAnim->pst_Data->pst_ListTracks->pst_AllTracks->uw_NumEvents - 1;

							if(uw_CurrentEvent == uw_NumEvents)
							{
								f_Time = pst_BlendAnim->pst_Data->pst_ListParam->f_Time;
								f_Delay = EVE_FrameToTime(pst_BlendAnim->pst_Data->pst_ListTracks->pst_AllTracks->pst_AllEvents[uw_CurrentEvent].uw_NumFrames & 0x7FFF);

								if(f_Time == f_Delay)
								{
									pst_BlendAnim->uc_BlendFlags |= ACT_C_TF_BlendFreezeBones;
									pst_BlendAnim->uc_MiscFlags |= ANI_C_MiscFlag_MagicBox_AfterEnd;
								}
							}
						}
					}
				}
				else
					pst_Transition = NULL;


				_pst_GOAnim->uc_CurrentActionItemIndex = iActionItemIndex;
				pst_ActionItem = &pst_Action->ast_ActionItem[iActionItemIndex];

#ifdef ANIMS_USE_ARAM
        		/* If TrackList of ActionItem is not in Cache (and should be), Load it From ARAM */
        		if(pst_ActionItem->pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM)
        		{   
        			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                    extern int ACT_i_Cache_LoadAnim(ACT_st_ActionKit *, EVE_tdst_ListTracks *);
        			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

        			ACT_i_Cache_LoadAnim(_pst_GOAnim->pst_ActionKit, pst_ActionItem->pst_TrackList);
        		}
#endif

				ANI_SetTrackList(_pst_GOAnim->apst_Anim[0], pst_ActionItem->pst_TrackList);
				ANI_ChangeShape(_pst_GO, pst_ActionItem->pst_Shape);

				_pst_GOAnim->uc_CounterForRepetition = 0;

				if(_pst_GOAnim->uc_ForceMode)
					uc_Flag = _pst_GOAnim->uc_ForceMode - 1;
				else
					uc_Flag = pst_ActionItem->uc_Flag;


				/* Set the flags for the dynamics */
				if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Dyna))
				{
					pst_Dyna = _pst_GO->pst_Base->pst_Dyna;

					ul_DynaFlag = DYN_ul_GetDynFlags(pst_Dyna);
					ul_DynaFlag &= ~(DYN_C_IgnoreGravity | DYN_C_IgnoreTraction | DYN_C_IgnoreStream);

					ul_DynaFlag |= (uc_Flag & ACT_C_ActionItemFlag_IgnoreGravity ? DYN_C_IgnoreGravity : 0);
					ul_DynaFlag |= (uc_Flag & ACT_C_ActionItemFlag_IgnoreTraction ? DYN_C_IgnoreTraction : 0);
					ul_DynaFlag |= (uc_Flag & ACT_C_ActionItemFlag_IgnoreStream ? DYN_C_IgnoreStream : 0);

					DYN_SetDynFlags(pst_Dyna, ul_DynaFlag);
				}

				/* Set the mode for the anim */
				_pst_GOAnim->apst_Anim[0]->uw_Flag &= ~ANI_C_AnimFlag_PlayModeMask;
				_pst_GOAnim->apst_Anim[0]->uw_Flag &= ~ANI_C_AnimFlag_DontPlayAnim;
				_pst_GOAnim->apst_Anim[0]->uw_Flag |= (uc_Flag & ANI_C_AnimFlag_PlayModeMask);

				/* If we loop to the same ActionItem and if IA has changed frequency of Animation, we dont want the ANI_ChangeAction to update the frequency */
				if(!b_SameActionItem)
					_pst_GOAnim->apst_Anim[0]->uc_AnimFrequency = (pst_ActionItem->uc_Frequency * 60 / ACT_C_DefaultAnimFrequency);


				if(pst_Transition)
				{
					if(_pst_GOAnim->uw_NextActionIndex == _pst_GOAnim->uw_CurrentActionIndex)
					{
						_pst_GOAnim->pst_Transition = NULL;
						_pst_GOAnim->uw_NextActionIndex = (USHORT) - 1;
					}
				}

			}
			else
			{
				/*
				 * The Anim is at its end but we didnt manage to change action. So, we dont change
				 * the Anim but set it as finished.
				 */
				_pst_GOAnim->apst_Anim[0]->uw_Flag |= ANI_C_AnimFlag_DontPlayAnim;
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    There is an Anim Player Mode (Mode 1) where the frequency of the Anim is based on the current speed of the
            actor. The Frequency computation is done in this Function.
 =======================================================================================================================
 */
_inline_ void ANI_UpdateFrequency
(
	ANI_tdst_Anim		*_pst_Anim,
	MATH_tdst_Vector	*_pst_MagicBoxMove,
	MATH_tdst_Vector	*_pst_Speed
)
{
	/*~~~~~~~~~~~~~~~*/
	float	fFrequency;
	float	fSqrMove;
	/*~~~~~~~~~~~~~~~*/

	fSqrMove = MATH_f_SqrNormVector(_pst_MagicBoxMove);

	if(!fNul(fSqrMove))
		fFrequency = fSqrt(MATH_f_SqrNormVector(_pst_Speed) / fSqrMove);
	else
		fFrequency = 1.0f;

	fFrequency = (fFrequency < 0.0f) ? 0.0f : (fFrequency > 255.0f) ? 255.0f : fFrequency;
	_pst_Anim->uc_AnimFrequency = (unsigned char) fFrequency;
}

/*
 =======================================================================================================================
    Aim:    Computes the speed of the Magic box at the End of the given Animation. (Speed at the END frame)

    Note:   For explanation of the role of this function, look at BlendMagicBox function
 =======================================================================================================================
 */
void ANI_GetEndSpeedOfMB(ANI_tdst_Anim *_pst_Anim, MATH_tdst_Vector *_pst_Speed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Track, i_Evt;
	EVE_tdst_Track		*pst_Track;
	EVE_tdst_Event		*pst_Evt, *pst_Prev;
	MATH_tdst_Vector	*pst_Pos;
	float				f_Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_InitVectorToZero(_pst_Speed);
	
	EVE_gpst_CurrentListTracks = _pst_Anim->pst_Data->pst_ListTracks;	
	pst_Track = EVE_gpst_CurrentListTracks->pst_AllTracks;
	i_Track = 0;
	while(i_Track++ < _pst_Anim->pst_Data->pst_ListTracks->uw_NumTracks)
	{
		EVE_gpst_CurrentTrack = pst_Track;
		if(pst_Track->uw_Gizmo != (USHORT) - 1) 
		{
			EVE_gpst_CurrentTrack = NULL;
			return;
		}

		pst_Evt = pst_Track->pst_AllEvents + pst_Track->uw_NumEvents - 1;
		i_Evt = pst_Track->uw_NumEvents;
		while(i_Evt--)
		{
			if((EVE_w_Event_InterpolationKey_GetFlags(pst_Evt) & EVE_C_EventFlag_Type) == EVE_C_EventFlag_InterpolationKey)
			{
				if(!(EVE_w_Event_InterpolationKey_GetType(pst_Evt) & EVE_InterKeyType_Translation_Mask)) break;

				if(i_Evt != pst_Track->uw_NumEvents - 1) 
				{
					EVE_gpst_CurrentTrack = NULL;
					return;
				}

				pst_Prev = pst_Evt - 1;
				pst_Pos = EVE_pst_Event_InterpolationKey_GetPos(pst_Prev);
				if(!pst_Pos) 
				{
					EVE_gpst_CurrentTrack = NULL;
					return;
				}

				MATH_SubVector(_pst_Speed, EVE_pst_Event_InterpolationKey_GetPos(pst_Evt), pst_Pos);
				f_Scale = ((float) _pst_Anim->uc_AnimFrequency) / (EVE_NumFrames(pst_Prev));
				MATH_ScaleEqualVector(_pst_Speed, f_Scale);
				EVE_gpst_CurrentTrack = NULL;
				return;
			}
			else if((EVE_w_Event_InterpolationKey_GetFlags(pst_Evt) & EVE_C_EventFlag_Type) != EVE_C_EventFlag_Empty) break;
			pst_Evt--;
		}

		pst_Track++;
	}

	EVE_gpst_CurrentTrack = NULL;
}

/*
 =======================================================================================================================
    Note:   We can set the number of blend frames in Jade. If we set this number to 10 and if there is only 5 frames
            before the End of the Blend Anim, we compute the End speed of the Magic box for this Anim and use it to
            compute a logical position evolution for the Magic Box after the 5 last frames. We freeze all the bones in
            their last position.
 =======================================================================================================================
 */
void ANI_InterpolateMagicBoxPositionAfterEndOfAnim
(
	OBJ_tdst_GameObject		*_pst_GO,
	ANI_st_GameObjectAnim	*_pst_GOAnim,
	ANI_tdst_Anim			*_pst_BlendAnim,
	float					_f_dt
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*pst_AnimZeroRef;
	MATH_tdst_Matrix	*pst_Matrix2Update;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$F
	 * We will store info in Anim 0 Reference Matrix. 
	 * (Totally useless Matrix because only Flash Matrix is needed for Anim 0)
	 */
	pst_AnimZeroRef = &_pst_GOAnim->apst_Anim[0]->st_Ref;

	if(_pst_BlendAnim->uc_MiscFlags & ANI_C_MiscFlag_MagicBox_AfterEnd)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_MagicBoxLastSpeed;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ANI_GetEndSpeedOfMB(_pst_BlendAnim, &st_MagicBoxLastSpeed);

		/* The st_MagicBoxLastSpeed is in the Reference Matrix CS. We want it in Global */
		MATH_TransformVector(&st_MagicBoxLastSpeed, &_pst_BlendAnim->st_Ref, &st_MagicBoxLastSpeed);

		/*
		 * Save the End speed of the Magic Box in the first column of the First Anim
		 * Reference Matrix (Totally useless Matrix).
		 */
		MATH_CopyVector((MATH_tdst_Vector *) pst_AnimZeroRef, &st_MagicBoxLastSpeed);

		_pst_BlendAnim->uc_MiscFlags -= ANI_C_MiscFlag_MagicBox_AfterEnd;
		_pst_BlendAnim->uc_MiscFlags |= ANI_C_MiscFlag_MagicBox_Interpolation;
	}

	if(_pst_BlendAnim->uc_MiscFlags & ANI_C_MiscFlag_MagicBox_Interpolation)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Global_Delta;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_InitVector(&st_Global_Delta, 0.0f, 0.0f, 0.0f);

		/* Computes interpolation vector. */
		if(_f_dt)
		{
			MATH_tdst_Vector	*pst_Temp;
			
			pst_Temp = (MATH_tdst_Vector *) pst_AnimZeroRef;
			MATH_ScaleVector(&st_Global_Delta, pst_Temp, _f_dt);
		}

		/* Update Pos and Flash */
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix st_InvFatherMatrix ONLY_PSX2_ALIGNED(16);
			MATH_tdst_Vector					st_Father_Delta;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Matrix2Update = &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix;

			MATH_InvertMatrix(&st_InvFatherMatrix, OBJ_pst_GetAbsoluteMatrix(OBJ_pst_GetFather(_pst_GO)));
			MATH_TransformVector(&st_Father_Delta, &st_InvFatherMatrix, &st_Global_Delta);

			MATH_AddVector(&pst_Matrix2Update->T, &ANI_gst_PositionBeforeAnimPlayer, &st_Father_Delta);
			ANI_UpdateFlashAfterMagicBoxTranslation(_pst_GO, &st_Father_Delta, ANI_C_UpdateBlendAnim);
		}
		else
		{
			pst_Matrix2Update = _pst_GO->pst_GlobalMatrix;
			MATH_AddVector(&pst_Matrix2Update->T, &ANI_gst_PositionBeforeAnimPlayer, &st_Global_Delta);
			ANI_UpdateFlashAfterMagicBoxTranslation(_pst_GO, &st_Global_Delta, ANI_C_UpdateBlendAnim);
		}
	}
}

/*
 =======================================================================================================================
    Aim:    Computes the Position/Orientation the Magic box would have if the GO had only played the given Animation.

    Note:   Information use to blend the Magic Box Position.
 =======================================================================================================================
 */
void ANI_b_ComputeMagicBoxPositionForBlendAnim
(
	OBJ_tdst_GameObject		*_pst_GO,
	ANI_st_GameObjectAnim	*_pst_GOAnim,
	ANI_tdst_Anim			*_pst_BlendAnim
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BOOL				(*pfnl_Player)    (EVE_tdst_Data *, float *);
	float				f_DT, f_Time;
	USHORT				uw_SaveNumTracks;
	ACT_st_Action		*pst_Action;
	ACT_st_ActionItem	*pst_AI;
	ULONG				ul_SaveParam;
	MATH_tdst_Matrix	*pst_AnimZeroRef;
	BOOL				b_Hierarchy;
	UCHAR				uc_Old_Frequency;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_DT = f_Time = TIM_gf_dt;

	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);


	if
	(
		(TIM_gf_dt > 0.0f)
	&&	!(ENG_gb_EVERunning)
	&&	(_pst_GO->pst_Base)
	&&	(_pst_GO->pst_Base->pst_GameObjectAnim)
	&&	(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	)
		pfnl_Player = EVE_b_RunAnimation;
	else
		pfnl_Player = EVE_b_RunAllTracks;



	/*$F
	 * We may need to store info in Anim 0 Reference Matrix. 
	 * (Totally useless Matrix because only Flash Matrix is needed for Anim 0)
	 */
	pst_AnimZeroRef = &_pst_GOAnim->apst_Anim[0]->st_Ref;

	if(ANI_b_PlayAfterEndOfAnim(_pst_BlendAnim))
	{
		ANI_InterpolateMagicBoxPositionAfterEndOfAnim(_pst_GO, _pst_GOAnim, _pst_BlendAnim, f_Time);
	}
	else
	{
		/* Update Events global vars. */
		EVE_gpst_OwnerGAO = _pst_GO;
		EVE_gpst_CurrentGAO = _pst_GO;
		EVE_gpst_CurrentData = _pst_BlendAnim->pst_Data;
		EVE_gpst_CurrentListTracks = EVE_gpst_CurrentData->pst_ListTracks;
		EVE_gpst_CurrentRefMatrix = &_pst_BlendAnim->st_Ref;

		/* Play only 2 first tracks : Track of magic box */
		uw_SaveNumTracks = EVE_gpst_CurrentListTracks->uw_NumTracks;
		if(EVE_gpst_CurrentListTracks->pst_AllTracks[1].uw_Gizmo == (USHORT) - 1)
			EVE_gpst_CurrentListTracks->uw_NumTracks = 2;
		else
			EVE_gpst_CurrentListTracks->uw_NumTracks = 1;

		/* Run Event Tracks */
		uc_Old_Frequency = 60;
		do
		{

			f_Time *= 60.0f / (float) uc_Old_Frequency;
			TIM_gf_dt = (f_Time * (float) _pst_BlendAnim->uc_AnimFrequency) / 60.0f;

			uc_Old_Frequency = _pst_BlendAnim->uc_AnimFrequency;

#ifdef ANIMS_USE_ARAM
			/* If data is not in Cache (and should be), Load it From ARAM */
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
				ACT_st_ActionKit		*pst_ActionKit;
				EVE_tdst_ListTracks		*pst_TrackList;
				EVE_tdst_Data	*p_Data;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			
				p_Data = _pst_BlendAnim->pst_Data;
				//p_Data = pst_GOAnim->apst_Anim[_iTrackNumber]->pst_Data;
				pst_ActionKit = _pst_GOAnim->pst_ActionKit;	
				pst_TrackList = p_Data->pst_ListTracks;
				if((pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM))
				{		
					ACT_i_Cache_LoadAnim(pst_ActionKit, pst_TrackList);
				}
			}
#endif		
			
			if(pfnl_Player(_pst_BlendAnim->pst_Data, &f_Time))
			{
				pst_Action = _pst_GOAnim->pst_ActionKit->apst_Action[_pst_BlendAnim->uw_BlendedAction];
				if
				(
					(_pst_BlendAnim->uc_BlendedActionItem != ACT_C_NoActionItem)
				&&	((++_pst_BlendAnim->uc_BlendedActionItem) >= pst_Action->uc_NumberOfActionItem)
				)
				{
					_pst_BlendAnim->uc_BlendedActionItem = pst_Action->uc_ActionItemNumberForLoop;
					if(_pst_BlendAnim->uc_BlendedActionItem != ACT_C_NoActionItem)
					{
						if(_pst_BlendAnim->uc_BlendedActionItem >= pst_Action->uc_NumberOfActionItem)
							_pst_BlendAnim->uc_BlendedActionItem = pst_Action->uc_NumberOfActionItem - 1;
					}
				}

				if(_pst_BlendAnim->uc_BlendedActionItem != ACT_C_NoActionItem)
				{
					EVE_gpst_CurrentListTracks->uw_NumTracks = uw_SaveNumTracks;

					pst_AI = &pst_Action->ast_ActionItem[_pst_BlendAnim->uc_BlendedActionItem];
					ul_SaveParam = _pst_BlendAnim->pst_Data->pst_ListParam->i_Param1;
					
					
#ifdef ANIMS_USE_ARAM
            		/* If TrackList of ActionItem is not in Cache (and should be), Load it From ARAM */
            		if(pst_AI->pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM)
            		{        		
            			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                        extern int ACT_i_Cache_LoadAnim(ACT_st_ActionKit *, EVE_tdst_ListTracks *);
            			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

            			ACT_i_Cache_LoadAnim(_pst_GOAnim->pst_ActionKit, pst_AI->pst_TrackList);
            		}
#endif
					
					ANI_SetTrackList(_pst_BlendAnim, pst_AI->pst_TrackList);
					_pst_BlendAnim->pst_Data->pst_ListParam->i_Param1 = ul_SaveParam;

					_pst_BlendAnim->pst_Data->pst_ListParam[1].i_Param1 = *(int *) &f_Time;
					_pst_BlendAnim->uc_MiscFlags |= ANI_C_MiscFlag_UseTimeInParam1;

					EVE_gpst_CurrentData = _pst_BlendAnim->pst_Data;
					EVE_gpst_CurrentListTracks = EVE_gpst_CurrentData->pst_ListTracks;

					/* Play only 2 first tracks : Track of magic box */
					uw_SaveNumTracks = EVE_gpst_CurrentListTracks->uw_NumTracks;
					if(EVE_gpst_CurrentListTracks->pst_AllTracks[1].uw_Gizmo == (USHORT) - 1)
						EVE_gpst_CurrentListTracks->uw_NumTracks = 2;
					else
						EVE_gpst_CurrentListTracks->uw_NumTracks = 1;

					/* Update Flash Matrix if needed */
					if(!f_Time) MATH_CopyMatrix(&_pst_BlendAnim->st_Ref, OBJ_pst_GetAbsoluteMatrix(_pst_GO));
				}
				else
				{
					_pst_BlendAnim->uw_Flag |= ANI_C_AnimFlag_DontPlayAnim;

					_pst_BlendAnim->uc_BlendFlags |= ACT_C_TF_BlendFreezeBones;
					_pst_BlendAnim->uc_MiscFlags |= ANI_C_MiscFlag_MagicBox_AfterEnd;

					if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
					{
						MATH_CopyVector
						(
							&ANI_gst_PositionBeforeAnimPlayer,
							&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T
						);
					}
					else
						MATH_CopyVector(&ANI_gst_PositionBeforeAnimPlayer, OBJ_pst_GetAbsolutePosition(_pst_GO));

					ANI_InterpolateMagicBoxPositionAfterEndOfAnim(_pst_GO, _pst_GOAnim, _pst_BlendAnim, f_Time);
					break;
				}
			}
		} while(f_Time != 0.0f);

		/* Restore number of track */
		EVE_gpst_CurrentListTracks->uw_NumTracks = uw_SaveNumTracks;
	}



	TIM_gf_dt = f_DT;
}

/*$F
 =======================================================================================================================
    Aim:    The MagicBox is placed where the first Anim should place it.
			We blend this position with the position where the second anim (Blend Anim) would place it.

    Note:   - THE MAGIC BOX IS ONLY BLENDED IN TRANSLATION (NO ROTATION BLEND)

			- IF THE LEAD ANIM IS IN MODE 3 (SPEED FROM DYNA), THERE WILL BE NO MAGIC BOX BLEND.
 =======================================================================================================================
 */

void ANI_b_BlendMagicBox
(
	OBJ_tdst_GameObject		*_pst_GO,
	ANI_st_GameObjectAnim	*_pst_GOAnim,
	ANI_tdst_Anim			*_pst_BlendAnim,
	float					_f_dt
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*pst_Matrix, M_Lead, M_New;
	MATH_tdst_Vector	st_Delta;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Save the Global/Local Matrix of the Object depending on hierarchy or not */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		pst_Matrix = &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix;
	else
		pst_Matrix = _pst_GO->pst_GlobalMatrix;

	MATH_CopyMatrix(&M_Lead, pst_Matrix);
	MATH_CopyMatrix(&M_New, &M_Lead);

	if((_pst_BlendAnim->uc_BlendFlags & ACT_C_TF_BlendFreezeMagicBox) || (_pst_BlendAnim->uw_Flag & ANI_C_AnimFlag_SpeedFromDyna))
	{
		/*
		 * The MagicBox is in FREEZE Mode. So, there is no MagicBox Blend at all. It is
		 * placed exactly where the LEAD anim has placed it. We have to update the Blend
		 * Reference Matrix so that it fits the MagicBox position given by the Lead Anim
		 * (That means that if the BlendAnim was played alone at its current frame, it
		 * would place the MagicBox at this precise point).
		 */
		MATH_SubVector(&st_Delta, &M_Lead.T, &ANI_gst_PositionBeforeAnimPlayer);
		ANI_UpdateFlashAfterMagicBoxTranslation(_pst_GO, &st_Delta, ANI_C_UpdateBlendAnim);
	}
	else
	{
		/*~~~~~~~~~*/
		float	f_dt;
		/*~~~~~~~~~*/

		f_dt = TIM_gf_dt;

		ANI_b_ComputeMagicBoxPositionForBlendAnim(_pst_GO, _pst_GOAnim, _pst_BlendAnim);

		/* Blend MagicBox position */
		MATH_BlendVector
		(
			&M_New.T,
			&pst_Matrix->T,
			&M_Lead.T,
			*(float *) &_pst_BlendAnim->pst_Data->pst_ListParam->i_Param1	/* f_PercentBlend */
		);

		/* The Magic Box position has been blended, we update the Flash Matrix. */
		MATH_SubVector(&st_Delta, &M_New.T, &M_Lead.T);
		ANI_UpdateFlashAfterMagicBoxTranslation(_pst_GO, &st_Delta, ANI_C_UpdateLeadAnim);

		MATH_SubVector(&st_Delta, &M_New.T, &pst_Matrix->T);
		ANI_UpdateFlashAfterMagicBoxTranslation(_pst_GO, &st_Delta, ANI_C_UpdateBlendAnim);

		/* Place the MagicBox at the Good position */
		MATH_CopyMatrix(pst_Matrix, &M_New);

		TIM_gf_dt = f_dt;
	}
}

/*
 =======================================================================================================================
    Aim:    Computes all Bones Local Matrix for this Anim.

    Note:   This function is called in ANI_RenderBlend.
 =======================================================================================================================
 */
void ANI_PlayBlendAnimationWithoutMagicBox
(
	OBJ_tdst_GameObject		*_pst_GO,
	ANI_st_GameObjectAnim	*_pst_GOAnim,
	ANI_tdst_Anim			*_pst_BlendAnim
)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
    BOOL	(*pfnl_Player)    (EVE_tdst_Data *, float *);
	float	f_DT, f_Time;
	USHORT	uw_SaveFlags[4];
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GOAnim) return;

	/* if the Blend Animation is in the BlendStock mode, that means that we dont have to play the blended animation.
		The matrixes of all the previous frame bones (after blendà) have been saved and we just reuse them */
	
	if(_pst_BlendAnim->uc_BlendFlags & ACT_C_TF_BlendStock)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
		OBJ_tdst_Group			*pst_Skeleton;
		OBJ_tdst_GameObject		*pst_BoneGO;
		int						i;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Skeleton = _pst_GOAnim->pst_Skeleton;
		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
		pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
		for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
		{
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));
			
			MATH_CopyMatrix(OBJ_pst_GetLocalMatrix(pst_BoneGO), &_pst_GOAnim->dpst_Stock[i]);		
		}

		return;
	}


	/* Update Events global vars. */
	EVE_gpst_OwnerGAO = _pst_GO;
	EVE_gpst_CurrentGAO = _pst_GO;
	EVE_gpst_CurrentData = _pst_BlendAnim->pst_Data;
	EVE_gpst_CurrentListTracks = EVE_gpst_CurrentData->pst_ListTracks;
	EVE_gpst_CurrentRefMatrix = &_pst_BlendAnim->st_Ref;

	/* Play All tracks except the first 2 (Magic box Tracks) */
	uw_SaveFlags[0] = EVE_gpst_CurrentData->pst_ListParam[0].uw_Flags & EVE_C_Track_Running;
	uw_SaveFlags[1] = EVE_gpst_CurrentData->pst_ListParam[1].uw_Flags & EVE_C_Track_Running;
	uw_SaveFlags[2] = EVE_gpst_CurrentData->pst_ListParam[2].uw_Flags & EVE_C_Track_Running;
	uw_SaveFlags[3] = EVE_gpst_CurrentData->pst_ListParam[3].uw_Flags & EVE_C_Track_Running;

	EVE_gpst_CurrentData->pst_ListParam[0].uw_Flags &= ~EVE_C_Track_Running;
	EVE_gpst_CurrentData->pst_ListParam[1].uw_Flags &= ~EVE_C_Track_Running;

	if
	(
		
		(TIM_gf_dt > 0.0f)
	&&	!(ENG_gb_EVERunning)
	&&	(_pst_GO->pst_Base)
	&&	(_pst_GO->pst_Base->pst_GameObjectAnim)
	&&	(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	)
		pfnl_Player = EVE_b_RunAnimation;
	else
		pfnl_Player = EVE_b_RunAllTracks;


	/*$F
	 * ----------------------------------------------------------------------------------
	 * The Blend Animation is in FREEZE BONES Mode but NOT the MagicBox.
	 * The Magic Box looped and found a Flash event during the MagicBox Blend(-> New Ref Matrix). 
	 * This new Reference Matrix does not fit to the Flash Matrix needed for the Bones 
	 * which are at the Last frame.  In that case, we do not blend the Main bone TRANSLATION. 
	 * We blend only the ROTATION.
	 * ----------------------------------------------------------------------------------
	 * The second Test is there because the Blend of Magic Box may have changed the Blend
	 * Animation. If it is the case, all the blend Animation structure has been
	 * reinitialized and no errors will occur.
	 * ----------------------------------------------------------------------------------
	 */
	/*$off*/
	if((_pst_BlendAnim->uc_BlendFlags & ACT_C_TF_BlendFreezeBones) && !(_pst_BlendAnim->uc_BlendFlags & ACT_C_TF_BlendFreezeMagicBox))
	{
		/*~~~~~~~~~~~~~~~~~*/
		float	f_Cur0, f_Cur2;
		/*~~~~~~~~~~~~~~~~~*/

		EVE_GetTracksTime(_pst_BlendAnim->pst_Data, 0, &f_Cur0, NULL);
		EVE_GetTracksTime(_pst_BlendAnim->pst_Data, 2, &f_Cur2, NULL);

		if(f_Cur0 + 1E-3f < f_Cur2)
		{
			EVE_gpst_CurrentData->pst_ListParam[3].uw_Flags &= ~EVE_C_Track_Running;
		}
	}
	/*$on*/

	/* Run Event Tracks */
	f_DT = f_Time = TIM_gf_dt;
	do
	{
		TIM_gf_dt = (f_Time * (float) _pst_BlendAnim->uc_AnimFrequency) / 60.0f;
		pfnl_Player(_pst_BlendAnim->pst_Data, &f_Time);
	} while(f_Time != 0.0f);

	/* Restore Tracks flags */
	EVE_gpst_CurrentData->pst_ListParam[0].uw_Flags |= uw_SaveFlags[0];
	EVE_gpst_CurrentData->pst_ListParam[1].uw_Flags |= uw_SaveFlags[1];
	EVE_gpst_CurrentData->pst_ListParam[2].uw_Flags |= uw_SaveFlags[2];
	EVE_gpst_CurrentData->pst_ListParam[3].uw_Flags |= uw_SaveFlags[3];

	TIM_gf_dt = f_DT;

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~*/
		float	f_Cur, f_Tot;
		/*~~~~~~~~~~~~~~~~~*/

		EVE_GetTracksTime(_pst_BlendAnim->pst_Data, 0, &f_Cur, &f_Tot);

		_pst_BlendAnim->uw_CurrentFrame = EVE_TimeToFrame(f_Cur);
		_pst_BlendAnim->uw_TotalFrame = EVE_TimeToFrame(f_Tot);
	}

#endif
}

/*
 =======================================================================================================================
    Aim:    Plays the Lead Animation for this GO.
 =======================================================================================================================
 */
void ANI_PlayLeadAnimation
(
	OBJ_tdst_GameObject		*_pst_GO,
	ANI_st_GameObjectAnim	*_pst_GOAnim,
	ANI_tdst_Anim			*_pst_Anim
)
{
	/*~~~~~~~~~~~~*/
    BOOL    (*pfnl_Player)    (EVE_tdst_Data *, float *);
	float	f_DT;
	float	f_Time;
	BOOL	b_ChangeAction;
	BOOL	b_HasLooped;
#ifdef ACTIVE_EDITORS
	float	f_First;
#endif
	/*~~~~~~~~~~~~*/

	/* Update Events global vars. */
	EVE_gpst_OwnerGAO = _pst_GO;
	EVE_gpst_CurrentData =	_pst_Anim->pst_Data;
	EVE_gpst_CurrentListTracks = EVE_gpst_CurrentData->pst_ListTracks;
	EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(_pst_GO);

	/*
	if(Zorglub && (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims) && _pst_GO->pst_Base && _pst_GO->pst_Base->pst_GameObjectAnim && _pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit)
	{
		int globox;
		globox = ACT_ul_ComputeOneACKSize(_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit);
		globox += 1;	
	}
	*/


#ifdef ACTIVE_EDITORS
	MATH_CheckMatrix(_pst_GO->pst_GlobalMatrix, "Animation speed set");
#endif

	/* Run Event Tracks */
	f_DT = f_Time = TIM_gf_dt;


	if((_pst_GO->pst_Base) && (_pst_GO->pst_Base->pst_GameObjectAnim) && (_pst_GO->pst_Base->pst_GameObjectAnim->uc_PauseAnim))
	{
		f_DT = 0.0f;
		f_Time = 0.0f;
	}

	if
	(
		
		(TIM_gf_dt > 0.0f)
	&&	!(ENG_gb_EVERunning)
	&&	(_pst_GO->pst_Base)
	&&	(_pst_GO->pst_Base->pst_GameObjectAnim)
	&&	(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
	)
		pfnl_Player = EVE_b_RunAnimation;
	else
		pfnl_Player = EVE_b_RunAllTracks;
		
#ifdef ANIMS_USE_ARAM
    if(EVE_gpst_CurrentListTracks && EVE_gpst_CurrentListTracks->pst_Cache)
    {
        EVE_gpst_CurrentListTracks->pst_Cache->l_Counter = 0;    
    }
#endif


	b_HasLooped = FALSE;

	do
	{
		TIM_gf_dt = (f_Time * (float) _pst_Anim->uc_AnimFrequency) / 60.0f;

#ifdef ACTIVE_EDITORS
		f_First = f_Time;
#endif
		/*$F
		 * Cinematics Issue:
		 * -----------------
		 * The Cinematics GO may have asked for a SetAction on this GameObject at a
		 * precised dt. We must use this dt to perform the SetAction and continue the 
		 * player with the remaining dt.
		 */
		if(_pst_GOAnim->uc_Frequency) TIM_gf_dt = _pst_GOAnim->f_NextActionDT;

		EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(_pst_GO);

		b_ChangeAction = FALSE;
		if(ANI_b_PlayAnim(_pst_Anim) && pfnl_Player(_pst_Anim->pst_Data, &f_Time))
		{
			f_Time = (f_Time * 60.0f) / (float) _pst_Anim->uc_AnimFrequency;
			ANI_ChangeAction(_pst_GO, _pst_GO->pst_Base->pst_GameObjectAnim);
			b_ChangeAction = TRUE;
			b_HasLooped = TRUE;

			/*
			 * The ANI_ChangeAction has "created" or "updated" a Blend Animation. We store the
			 * remaining time that corresponds to the time the new starting animation will be
			 * played.
			 */
			if(ANI_b_Blend(_pst_GO) && f_Time)
			{
				_pst_GOAnim->apst_Anim[1]->pst_Data->pst_ListParam[1].i_Param1 = *(int *) &f_Time;
				_pst_GOAnim->apst_Anim[1]->uc_MiscFlags |= ANI_C_MiscFlag_UseTimeInParam1;
			}
		}

		/* Cinematics Issue: See above */
		if(_pst_GOAnim->uc_Frequency)
		{
			/*~~~~~~~~~~~~~~~~~*/
			USHORT				uw_Action;
			UCHAR				uc_Frequency;
			MATH_tdst_Matrix	st_Save_Matrix;
			/*~~~~~~~~~~~~~~~~~*/

			uw_Action = _pst_GOAnim->uw_NextActionIndex;
			uc_Frequency = _pst_GOAnim->uc_Frequency;
			_pst_GOAnim->uc_Frequency = 0;

			_pst_GOAnim->uw_NextActionIndex = 0xFFFF;

			MATH_CopyMatrix(&st_Save_Matrix, _pst_GO->pst_GlobalMatrix);
			ACT_SetAction(_pst_GO, uw_Action, 0, FALSE);

			if(_pst_GOAnim->uw_Frame)
			{
				/*~~~~~~~~~~~~~~~~~~~~*/
				EVE_tdst_Data	*p_Data;
				/*~~~~~~~~~~~~~~~~~~~~*/

				p_Data = _pst_GOAnim->apst_Anim[0]->pst_Data;
				EVE_SetTracksTime(p_Data, _pst_GOAnim->uw_Frame * (1.0f / 60.0f));
			}

			if(uc_Frequency != 255) _pst_GOAnim->apst_Anim[0]->uc_AnimFrequency = uc_Frequency;

			MATH_CopyMatrix(_pst_GO->pst_GlobalMatrix, &st_Save_Matrix);
			ANI_UpdateFlash(_pst_GO, ANI_C_UpdateAllAnims);

			f_Time = f_DT - _pst_GOAnim->f_NextActionDT;
			_pst_GOAnim->f_NextActionDT = 0.0f;
		}

#ifdef ACTIVE_EDITORS
		else
		{
			if(!b_ChangeAction && f_Time && (f_Time == f_First))
			{
				
				ERR_X_Warning(0, "An Anim seems to loop !!!", _pst_GO->sz_Name);
				f_Time = 0;
			}
		}

#endif
		/* Global Vars */
		EVE_gpst_CurrentData = _pst_Anim->pst_Data;
		EVE_gpst_CurrentListTracks = EVE_gpst_CurrentData->pst_ListTracks;
		EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(_pst_GO);
	} while((f_Time != 0.0f) && (!(_pst_Anim->uw_Flag & ANI_C_AnimFlag_DontPlayAnim)));
	TIM_gf_dt = f_DT;

#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~*/
		float	f_Cur, f_Tot;
		/*~~~~~~~~~~~~~~~~~*/

		EVE_GetTracksTime(_pst_Anim->pst_Data, 0, &f_Cur, &f_Tot);

		_pst_Anim->uw_CurrentFrame = EVE_TimeToFrame(f_Cur);
		_pst_Anim->uw_TotalFrame = EVE_TimeToFrame(f_Tot);
	}

#endif
}

/*
 =======================================================================================================================
    Aim:    Go _f_dt second forward and compute the new position of all the animations of the game object
 =======================================================================================================================
 */
void ANI_PlayGameObjectAnimation
(
	OBJ_tdst_GameObject		*_pst_GO,
	ANI_st_GameObjectAnim	*_pst_GOAnim,
	float					_f_dt,
	MATH_tdst_Vector		*_pst_Speed
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_MagicBoxMove, st_NegMagicBoxMove;
	MATH_tdst_Vector	st_MagicBoxOldPosition;
	ANI_tdst_Anim		*pst_LeadAnim, *pst_BlendAnim;
	float				f_DT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * Saves position of the Magic box before the Anim Player. It may be used to
	 * compute a speed at the End, depending on the Anim Mode.
	 */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		MATH_CopyVector(&st_MagicBoxOldPosition, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
	else
		MATH_CopyVector(&st_MagicBoxOldPosition, OBJ_pst_GetAbsolutePosition(_pst_GO));

	MATH_CopyVector(&ANI_gst_PositionBeforeAnimPlayer, &st_MagicBoxOldPosition);

	f_DT = TIM_gf_dt;
	TIM_gf_dt = _f_dt;

	/*$F----------------------------------------------------------------------------------------------------

											 LEAD Animation												

		-------------------------------------------------------------------------------------------------------*/
	pst_LeadAnim = _pst_GOAnim->apst_Anim[0];
	
#ifdef ANIMS_USE_ARAM
		/* If TrackList of ActionItem is not in Cache (and should be), Load it From ARAM */
		if
		(
			_pst_GOAnim->pst_ActionKit 
		&& 	pst_LeadAnim 
		&& ((ULONG) pst_LeadAnim != (ULONG) - 1)
		&& 	pst_LeadAnim->pst_Data 
		&& 	pst_LeadAnim->pst_Data->pst_ListTracks 
		&& 	(pst_LeadAnim->pst_Data->pst_ListTracks->ul_GC_Flags & EVE_C_ListTracks_UseARAM) 
		&& !(pst_LeadAnim->pst_Data->pst_ListTracks->ul_GC_Flags & EVE_C_ListTracks_InRAMGC)
		)
		{		
			ACT_i_Cache_LoadAnim(_pst_GOAnim->pst_ActionKit, pst_LeadAnim->pst_Data->pst_ListTracks);
		}
#endif
	

	/* Anti-Bug. */
	if(!pst_LeadAnim || ((ULONG) pst_LeadAnim == (ULONG) - 1)) return;

	/* We update the Animation frequency if it is enslaved to the Speed */
	if(ANI_b_PlayAnim(pst_LeadAnim) && _pst_Speed && (ANI_b_TestPlayMode(pst_LeadAnim, ANI_C_AnimFlag_UpdateFrequency)))
	{
		MATH_ScaleVector(&st_MagicBoxMove, _pst_Speed, _f_dt);
		ANI_UpdateFrequency(pst_LeadAnim, &st_MagicBoxMove, _pst_Speed);
	}

	if(ANI_b_PlayAnim(pst_LeadAnim) || (!ANI_b_PlayAnim(pst_LeadAnim) && _pst_GOAnim->uc_Frequency))
	{
		ANI_PlayLeadAnimation(_pst_GO, _pst_GOAnim, pst_LeadAnim);
	}

	/*$F----------------------------------------------------------------------------------------------------

											 BLEND Animation												

		-------------------------------------------------------------------------------------------------------*/
	if(ANI_b_Blend(_pst_GO))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		float	f_BlendTime, f_BlendLength;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_BlendAnim = _pst_GOAnim->apst_Anim[1];

		/*
		 * The Lead animation player may have just created/updated a Blend. If so, it has
		 * stored in i_Param1 the time the new/updated Blended animation has to be played.
		 */
		if(pst_BlendAnim->uc_MiscFlags & ANI_C_MiscFlag_UseTimeInParam1)
		{
			TIM_gf_dt = *(float *) &pst_BlendAnim->pst_Data->pst_ListParam[1].i_Param1;

			/* We dont remove the Flag because the ANI_RenderBlend will also use it. */
		}

		f_BlendLength = (pst_BlendAnim->uc_BlendTime + 1) / (60.0f);
		f_BlendTime = (pst_BlendAnim->uw_BlendCurTime) / ((float) 60 * 256);
		f_BlendTime += TIM_gf_dt;

		/* Test End of blending time */
		if(f_BlendTime < f_BlendLength)
		{
			/* Store Percent of Blend done */
			*(float *) &pst_BlendAnim->pst_Data->pst_ListParam->i_Param1 = f_BlendTime / f_BlendLength;

#ifdef ACTIVE_EDITORS
			pst_BlendAnim->uw_CurrentBlendFrame = EVE_TimeToFrame(f_BlendTime);
#endif
			/* Store current blending time */
			f_BlendTime *= ((float) 60 * 256);
			pst_BlendAnim->uw_BlendCurTime = (USHORT) f_BlendTime;

			/* We update the Animation frequency if it is enslaved to the Speed */
			if(_pst_Speed && (ANI_b_TestPlayMode(pst_BlendAnim, ANI_C_AnimFlag_UpdateFrequency)))
			{
				MATH_ScaleVector(&st_MagicBoxMove, _pst_Speed, TIM_gf_dt);
				ANI_UpdateFrequency(pst_BlendAnim, &st_MagicBoxMove, _pst_Speed);
			}

			/* Blend magic box position */
			ANI_b_BlendMagicBox(_pst_GO, _pst_GOAnim, pst_BlendAnim, TIM_gf_dt);
		}
		else
		{
			/* Blend is over. */
			ANI_StopBlend(_pst_GO);

#ifdef ACTIVE_EDITORS
			LINK_UpdatePointer(_pst_GO);
#endif
		}
	}

	/*
	 * If the GO has hierarchy, we have only updated the Local Matrix. We update its
	 * Global
	 */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy)) OBJ_ComputeGlobalWhenHie(_pst_GO);
	/* Compute the move of the Magic Box. */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		MATH_SubVector(&st_MagicBoxMove, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_MagicBoxOldPosition);
	else
		MATH_SubVector(&st_MagicBoxMove, OBJ_pst_GetAbsolutePosition(_pst_GO), &st_MagicBoxOldPosition);

	/*$F
	 *						---- SPEED FROM DYNA (JUMP :  MODE 3) ---
	 *
	 *			We DONT want the Anim player  to move the Object or 
	 *			to update any speed, it will be done by IA or whatever.
	 *			So, we move back the MagicBox to where it was BEFORE
	 *			the Anim Player.
	 */
	if(ANI_b_TestPlayMode(pst_LeadAnim, ANI_C_AnimFlag_SpeedFromDyna))
	{
		/* Replace MagicBox to where it was BEFORE the Anim Player. */

		if(!(pst_LeadAnim->uw_Flag & ANI_C_AnimFlag_Developped))
		{
			if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
				MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_MagicBoxOldPosition);
			else
				MATH_CopyVector(OBJ_pst_GetAbsolutePosition(_pst_GO), &st_MagicBoxOldPosition);
		}

		MATH_NegVector(&st_NegMagicBoxMove, &st_MagicBoxMove);

		if(!(pst_LeadAnim->uw_Flag & ANI_C_AnimFlag_Developped))
		{
			ANI_UpdateFlashAfterMagicBoxTranslation(_pst_GO, &st_NegMagicBoxMove, ANI_C_UpdateAllAnims);
		}
	}

	/*$F
	 *						---- SPEED FROM ANIM (RUN : MODE 0) ---
	 *
	 *			The Anim may have updated itself the position of the Magic Box. 
	 *			We move back the Magic box to its position BEFORE the Anim player and update
	 *			the speed of the Object so that the DYN module can move the Object to the Exact 
	 *			position defined by the Anim.
	 */
	if(ANI_b_TestPlayMode(pst_LeadAnim, ANI_C_AnimFlag_SpeedFromAnim) && _pst_Speed && _f_dt)
	{
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy)) OBJ_ComputeGlobalWhenHie(_pst_GO);

		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
			MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_MagicBoxOldPosition);
		else
			MATH_CopyVector(OBJ_pst_GetAbsolutePosition(_pst_GO), &st_MagicBoxOldPosition);

		MATH_NegVector(&st_NegMagicBoxMove, &st_MagicBoxMove);

		ANI_UpdateFlashAfterMagicBoxTranslation(_pst_GO, &st_NegMagicBoxMove, ANI_C_UpdateAllAnims);

		/* Compute MagicBox Move in GCS for Speed Update */
		if((OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy)) && _pst_GO->pst_Base && _pst_GO->pst_Base->pst_Hierarchy && _pst_GO->pst_Base->pst_Hierarchy->pst_Father)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Temp;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			
			MATH_CopyVector(&st_Temp, &st_MagicBoxMove);
			MATH_TransformVector(&st_MagicBoxMove, _pst_GO->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix, &st_Temp);
		}

		MATH_ScaleVector(_pst_Speed, &st_MagicBoxMove, fInv(_f_dt));
		DYN_SetHorizontalSpeedComponent(_pst_GO->pst_Base->pst_Dyna, _pst_Speed);
	}

	TIM_gf_dt = f_DT;
}
/*------------------------------------------------------------------------
		Anim partielles sur certains bones uniquement. JAMAIS la MagicBox
-------------------------------------------------------------------------*/
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ANI_b_GizmoInMask(ULONG *_pul_Mask, USHORT _uw_Gizmo)
{
	if(!_pul_Mask) return TRUE;

	if(_uw_Gizmo == 0xFFFF) return FALSE;


    if (_uw_Gizmo < 32)
      return (_pul_Mask[0] & (1 << _uw_Gizmo));
    if (_uw_Gizmo < 64)
      return (_pul_Mask[1] & (1 << (_uw_Gizmo & 0x001F)));
	else
		ERR_X_Warning(0, "Partial Anim Gizmo out of range", NULL)

	return TRUE;
}

/*
 =======================================================================================================================
 Aim: Creates the animation mask of the given Anim (i.e add all gizmo present in the animation in the mask)
 =======================================================================================================================
 */
void ANI_ComputeAnimMask(ULONG *_pul_Mask, ANI_tdst_Anim *_pst_PartialAnim)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	USHORT		i, uw_Gizmo;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!_pul_Mask
	||	!_pst_PartialAnim
	||	!_pst_PartialAnim->pst_Data
	||	!_pst_PartialAnim->pst_Data->pst_ListTracks
	||	!_pst_PartialAnim->pst_Data->pst_ListTracks->pst_AllTracks
	) return;


	_pul_Mask[0] = 0;
	_pul_Mask[1] = 0;

	for(i = 0;  i < _pst_PartialAnim->pst_Data->pst_ListTracks->uw_NumTracks; i++)
	{
		uw_Gizmo = _pst_PartialAnim->pst_Data->pst_ListTracks->pst_AllTracks[i].uw_Gizmo;

		if(uw_Gizmo == 0xFFFF) continue;

		if (uw_Gizmo < 32)
		_pul_Mask[0] |= 1 << uw_Gizmo;
		else
		{
			if (uw_Gizmo < 64)
			_pul_Mask[1] |= 1 << (uw_Gizmo & 0x001F);
			else
			{
				ERR_X_Warning(0, "Partial Anim Gizmo out of range", NULL)
			}
		}
	}
}
/*
 =======================================================================================================================
 Note: Lead Anim A is playing. We want to add an partial animation B only onto certain bones.
 By default, _pul_Mask is NULL and the "mix" will occur on ALL the gizmo present in Animation B. 
 (Animation B cannot have a MagicBox track)
 If you want to "mix" only on certain bones present in animation B, use the _pul_Mask.
 =======================================================================================================================
 */
void ANI_PartialAnim_PlayAll(OBJ_tdst_GameObject *_pst_GO, float _f_DT)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    BOOL				(*pfnl_Player)    (EVE_tdst_Data *, float *);
	ANI_tdst_Anim		**ppst_PartialAnim;
	TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_Group		*pst_Skeleton;
	OBJ_tdst_GameObject	*pst_BoneGO;
	MATH_tdst_Matrix	st_TempMatrix[64], st_Final;
	MATH_tdst_Matrix	*pst_Stock, *pst_Blend;
	MATH_tdst_Vector	st_Scale;
	BOOL				b_ChangeAction;
	ULONG				aul_Mask[2];
	float				f_Time, f_SaveDT, f_SaveDT2;
#ifdef ACTIVE_EDITORS
	float				f_First;
#endif
	float				f_BlendLength, f_BlendTime, f_Blend;
	int					i;
	BOOL				b_InverseBlend;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		!_pst_GO 
	||	!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->apst_PartialAnim[0]
	) return;


	if(OBJ_b_TestControlFlag(_pst_GO, OBJ_C_ControlFlag_InPause)) return;


	f_SaveDT = TIM_gf_dt;
	f_SaveDT2 = _f_DT;
	b_InverseBlend = FALSE;

	ppst_PartialAnim = &_pst_GO->pst_Base->pst_GameObjectAnim->apst_PartialAnim[0];
	while(*ppst_PartialAnim)
	{
		if((*ppst_PartialAnim)->uw_Flag & ANI_C_AnimFlag_DontPlayAnim)
		{
			if(!(*ppst_PartialAnim)->uc_BlendTime)
			{
				ppst_PartialAnim++;
				continue;
			}
			else
			{
				_f_DT = 0.0f;
				(*ppst_PartialAnim)->uc_BlendFlags |= ACT_C_TF_BlendFreezeBones;
				(*ppst_PartialAnim)->uw_Flag &=  ~ANI_C_AnimFlag_DontPlayAnim;
				b_InverseBlend = TRUE;
			}
		}

		/* Save current matrix of bones */
		pst_Skeleton = _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
		pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
		for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
		{
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

			ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));
			ERR_X_Assert(pst_BoneGO && pst_BoneGO->pst_Base && pst_BoneGO->pst_Base->pst_Hierarchy);

			MATH_CopyMatrix(&st_TempMatrix[i], &pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix);
		}

		/* Update Events global vars. */
		EVE_gpst_OwnerGAO = _pst_GO;
		EVE_gpst_CurrentGAO = _pst_GO;
		EVE_gpst_CurrentData = (*ppst_PartialAnim)->pst_Data;
		EVE_gpst_CurrentListTracks = EVE_gpst_CurrentData->pst_ListTracks;
		EVE_gpst_CurrentRefMatrix = &(*ppst_PartialAnim)->st_Ref;

		if
		(
			(_pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask[0] != 0)
		||	(_pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask[1] != 0)
		 )
			EVE_gpul_PartialMask = _pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask;
		else
		{
			EVE_gpul_PartialMask = aul_Mask;
			ANI_ComputeAnimMask(EVE_gpul_PartialMask, *ppst_PartialAnim);
		}

		if
		(		
			(_f_DT > 0.0f)
		&&	!(ENG_gb_EVERunning)
		&&	(_pst_GO->pst_Base)
		&&	(_pst_GO->pst_Base->pst_GameObjectAnim)
		&&	(_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton)
		)
			pfnl_Player = EVE_b_RunAnimation;
		else
			pfnl_Player = EVE_b_RunAllTracks;

		f_Time = _f_DT;


		/* Run Event Tracks */
		do
		{
			TIM_gf_dt = (f_Time * (float) (*ppst_PartialAnim)->uc_AnimFrequency) / 60.0f;

#ifdef ACTIVE_EDITORS
			f_First = f_Time;
#endif

			EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(_pst_GO);

			b_ChangeAction = FALSE;
			
			
#ifdef ANIMS_USE_ARAM
	/* If data is not in Cache (and should be), Load it From ARAM */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EVE_tdst_Data	*p_Data;
		ANI_st_GameObjectAnim	*pst_GOAnim;
		ACT_st_ActionKit		*pst_ActionKit;
		EVE_tdst_ListTracks		*pst_TrackList;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
		if (_pst_GO->pst_Base && _pst_GO->pst_Base->pst_GameObjectAnim)
		{
	
			pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
			p_Data = (*ppst_PartialAnim)->pst_Data;
	
			pst_ActionKit = pst_GOAnim->pst_ActionKit;	
			pst_TrackList = p_Data->pst_ListTracks;
			if((pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM))
			{		
				ACT_i_Cache_LoadAnim(pst_ActionKit, pst_TrackList);
			}
		}
	}
#endif
					
			if(ANI_b_PlayAnim((*ppst_PartialAnim)) && pfnl_Player((*ppst_PartialAnim)->pst_Data, &f_Time))
			{
				f_Time = (f_Time * 60.0f) / (float) (*ppst_PartialAnim)->uc_AnimFrequency;
				ANI_ChangePartialAction(_pst_GO);
				b_ChangeAction = TRUE;

			}

#ifdef ACTIVE_EDITORS
			if(!b_ChangeAction && f_Time && (f_Time == f_First))
			{
				
				ERR_X_Warning(0, "An Anim seems to loop !!!", _pst_GO->sz_Name);
				f_Time = 0;
			}

#endif
		/* Global Vars */
		EVE_gpst_CurrentData = (*ppst_PartialAnim)->pst_Data;
		EVE_gpst_CurrentListTracks = EVE_gpst_CurrentData->pst_ListTracks;
		EVE_gpst_CurrentRefMatrix = OBJ_pst_GetFlashMatrix(_pst_GO);


		} while((f_Time != 0.0f) && (!((*ppst_PartialAnim)->uw_Flag & ANI_C_AnimFlag_DontPlayAnim)));

		if((*ppst_PartialAnim)->uw_Flag & ANI_C_AnimFlag_DontPlayAnim)
		{
			b_InverseBlend = TRUE;
		}


#ifdef ACTIVE_EDITORS
		{
			/*~~~~~~~~~~~~~~~~~*/
			float	f_Cur, f_Tot;
			/*~~~~~~~~~~~~~~~~~*/

			EVE_GetTracksTime((*ppst_PartialAnim)->pst_Data, 0, &f_Cur, &f_Tot);

			(*ppst_PartialAnim)->uw_CurrentFrame = EVE_TimeToFrame(f_Cur);
			(*ppst_PartialAnim)->uw_TotalFrame = EVE_TimeToFrame(f_Tot);
		}
#endif


		f_BlendLength = ((*ppst_PartialAnim)->uc_BlendTime + 1) / (60.0f);
		f_BlendTime = ((*ppst_PartialAnim)->uw_BlendCurTime) / ((float) 60 * 256);
		f_BlendTime += f_SaveDT2;


			/* Test End of blending time */
		if(f_BlendTime < f_BlendLength)
		{
			f_Blend = f_BlendTime / f_BlendLength;

	#ifdef ACTIVE_EDITORS
			(*ppst_PartialAnim)->uw_CurrentBlendFrame = EVE_TimeToFrame(f_BlendTime);
	#endif
			/* Store current blending time */
			f_BlendTime *= ((float) 60 * 256);
			(*ppst_PartialAnim)->uw_BlendCurTime = (USHORT) f_BlendTime;
		}
		else
		{
			(*ppst_PartialAnim)->uc_BlendTime = 0;
			f_Blend = 1.0f;
		}


		/* Merge between lead animation and partial anim */

		pst_Stock = _pst_GO->pst_Base->pst_GameObjectAnim->dpst_Stock;
		pst_Skeleton = _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
		pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
		for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
		{
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

			ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));
			ERR_X_Assert(pst_BoneGO && pst_BoneGO->pst_Base && pst_BoneGO->pst_Base->pst_Hierarchy);

			if(!ANI_b_GizmoInMask(EVE_gpul_PartialMask, i)) 
			{
				pst_Stock ++;
				continue;
			}

			if(b_InverseBlend)
			{
				pst_Blend = &st_TempMatrix[i];
			}
			else
				pst_Blend = pst_Stock;

			/* Rotation */
			MATH_MatrixBlend(&st_Final, pst_Blend, &pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix, b_InverseBlend ? (1.0f - f_Blend) : f_Blend, 1);

			/* Translation */
			MATH_BlendVector(&st_Final.T, &pst_Blend->T, &pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, b_InverseBlend ? (1.0f - f_Blend) : f_Blend);

			/* Copy final result */
			MATH_CopyMatrix(&pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix, &st_Final);

			/* Scale/Zoom */
			MATH_GetScale(&st_Scale, pst_Blend);
			MATH_SetScale(&pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix, &st_Scale);

			pst_Stock ++;
		}

		if((*ppst_PartialAnim)->uc_BlendFlags & ACT_C_TF_BlendFreezeBones)
				(*ppst_PartialAnim)->uw_Flag |=  ANI_C_AnimFlag_DontPlayAnim;

		ppst_PartialAnim++;
	}

	EVE_gpul_PartialMask = NULL;

	TIM_gf_dt = f_SaveDT;
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
