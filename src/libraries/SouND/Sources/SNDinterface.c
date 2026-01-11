/*$T sndinterface.c GC 1.138 12/16/04 11:57:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDinterface.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDvolume.h"

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_Setf(int id, float val)
{
	if(!SND_gst_Params.l_Available) return -1;

	switch(id)
	{
	case SND_Cte_SetDopplerFactor:		SND_gst_Params.f_DopplerFactor = val; break;
	case SND_Cte_SetDopplerSoundSpeed:	SND_gst_Params.f_DopplerSoundSpeed = val; break;
	case SND_Cte_SetRolloffFactor:		SND_gst_Params.f_RolloffFactor = val; break;
	case SND_Cte_SetSurroundFactor:		SND_gst_Params.f_SurroundFactor = val; break;
	case SND_Cte_SetLoadingFadeInTime:	SND_SetLoadingFadeInTime(val); break;
	case SND_Cte_SetLoadingFadeOutTime: SND_SetLoadingFadeOutTime(val); break;
	default:							return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_Getf(int id)
{
	if(!SND_gst_Params.l_Available) return -1.0f;

	switch(id)
	{
	case SND_Cte_SetDopplerFactor:		return SND_gst_Params.f_DopplerFactor;
	case SND_Cte_SetDopplerSoundSpeed:	return SND_gst_Params.f_DopplerSoundSpeed;
	case SND_Cte_SetRolloffFactor:		return SND_gst_Params.f_RolloffFactor;
	case SND_Cte_SetSurroundFactor:		return SND_gst_Params.f_SurroundFactor;
	case SND_Cte_SetLoadingFadeInTime:	return SND_f_GetLoadingFadeInTime();
	case SND_Cte_SetLoadingFadeOutTime: return SND_f_GetLoadingFadeOutTime();
	default:							return -1.0f;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_Seti(int id, int val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern int	ps2SND_i_ChangePriority(int);
	extern BOOL INO_gb_SavingScreen;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;
	switch(id)
	{
	case -1:
#ifdef _GAMECUBE
		INO_gb_SavingScreen = TRUE;
#elif defined(PSX2_TARGET)
		ps2SND_i_ChangePriority(id);
#endif
		break;

	case -2:
#ifdef _GAMECUBE
        INO_gb_SavingScreen = FALSE;
#endif
#ifdef PSX2_TARGET
		ps2SND_i_ChangePriority(id);
#endif
		break;

	case SND_Cte_SetFreeze3DVol:
		if(val)
		{
			if(!(SND_gst_Params.ul_Flags & SND_Cte_Freeze3DVol) && SND_gst_Params.pst_RefForVol)
			{
				SND_gst_Params.ul_Flags |= SND_Cte_Freeze3DVol;

				MATH_CopyMatrix(&SND_gst_Params.st_FreezeRefForVol, SND_gst_Params.pst_RefForVol);
				SND_gst_Params.pst_SaveRefForVol = SND_gst_Params.pst_RefForVol;
				SND_gst_Params.pst_RefForVol = &SND_gst_Params.st_FreezeRefForVol;
			}
		}
		else
		{
			if(SND_gst_Params.ul_Flags & SND_Cte_Freeze3DVol)
			{
				SND_gst_Params.ul_Flags &= ~SND_Cte_Freeze3DVol;
				SND_gst_Params.pst_RefForVol = SND_gst_Params.pst_SaveRefForVol;
			}
		}
		break;

	case SND_Cte_SetFreezeDynamicPan:
		if(val)
		{
			if(!(SND_gst_Params.ul_Flags & SND_Cte_FreezeDynPan) && SND_gst_Params.pst_RefForPan)
			{
				SND_gst_Params.ul_Flags |= SND_Cte_FreezeDynPan;

				MATH_CopyMatrix(&SND_gst_Params.st_FreezeRefForPan, SND_gst_Params.pst_RefForPan);
				SND_gst_Params.pst_SaveRefForPan = SND_gst_Params.pst_RefForPan;
				SND_gst_Params.pst_RefForPan = &SND_gst_Params.st_FreezeRefForPan;
			}
		}
		else
		{
			if(SND_gst_Params.ul_Flags & SND_Cte_FreezeDynPan)
			{
				SND_gst_Params.ul_Flags &= ~SND_Cte_FreezeDynPan;
				SND_gst_Params.pst_RefForPan = SND_gst_Params.pst_SaveRefForPan;
			}
		}
		break;

	case SND_Cte_SetLoading:
		SND_WacSetLoading(val);
		break;

	case SND_Cte_SetRenderMode:
		return SND_i_ChangeRenderMode(val);

	default:
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_Geti(int id)
{
	/*~~*/
	int f;
	/*~~*/

	if(!SND_gst_Params.l_Available) return -1;
	switch(id)
	{
	case SND_Cte_SetFreezeDynamicPan:	f = (SND_gst_Params.ul_Flags & SND_Cte_FreezeDynPan) ? TRUE : FALSE; break;
	case SND_Cte_SetFreeze3DVol:		f = (SND_gst_Params.ul_Flags & SND_Cte_Freeze3DVol) ? TRUE : FALSE; return -1;
	case SND_Cte_SetRenderMode:			f = SND_gst_Params.ul_RenderMode; break;
	case SND_Cte_GetRenderModeCapacity: f = SND_gst_Params.ul_RenderModeCapacity; break;
	default:							return -1;
	}

	return f;
}

/*$4
 ***********************************************************************************************************************
    instance seti/geti setf/getf
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_InstanceSeti(int i_index, int i_Rq, int i_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(i_index, pst_SI, -1);
	switch(i_Rq)
	{
	case SND_Cte_GetInstFxCore:
		pst_SI->ul_Flags &= ~(SND_Cul_SF_FxA | SND_Cul_SF_FxB);
		switch(i_Val)
		{
		case SND_Cte_FxCoreA:	pst_SI->ul_Flags |= SND_Cul_SF_FxA; break;
		case SND_Cte_FxCoreB:	pst_SI->ul_Flags |= SND_Cul_SF_FxB; break;
		default:				break;
		}
		break;

	case SND_Cte_GetInstTrack:
		SND_InstTrackSet(i_index, i_Val);
		break;

	case SND_Cte_SetInstGroup:
		if (i_Val == -1)
		{
			pst_SI->i_GroupId = 0;
			break;
		}

		switch(SND_M_GetGrp(i_Val))
		{
		case SND_Cte_GrpMusic:		pst_SI->i_GroupId = SND_Cte_GrpMusic; break;
		case SND_Cte_GrpSpecialFX:	pst_SI->i_GroupId = SND_Cte_GrpSpecialFX; break;
		case SND_Cte_GrpAmbience:	pst_SI->i_GroupId = SND_Cte_GrpAmbience; break;
		case SND_Cte_GrpDialog:		pst_SI->i_GroupId = SND_Cte_GrpDialog; break;
		case SND_Cte_GrpCutScene:	pst_SI->i_GroupId = SND_Cte_GrpCutScene; break;
		case SND_Cte_GrpInterface:	pst_SI->i_GroupId = SND_Cte_GrpInterface; break;
		default:					break;
		}

		if(i_Val & SND_Cte_GrpA) pst_SI->i_GroupId |= SND_Cte_GrpA;
		if(i_Val & SND_Cte_GrpB) pst_SI->i_GroupId |= SND_Cte_GrpB;
		break;

	default:
		return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_InstanceGeti(int i_index, int i_Rq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					l_Status;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(i_index, pst_SI, -1);
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_i_InstanceSetf);

	switch(i_Rq)
	{
	case SND_Cte_GetInstSoundKey:
		if(pst_SI->l_Sound >= SND_gst_Params.l_SoundNumber) return -1;
		return *(int*)&(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey);
		
	case SND_Cte_GetInstFxCore:
		if(pst_SI->ul_Flags & SND_Cul_SF_FxA)
			return SND_Cte_FxCoreA;
		else if(pst_SI->ul_Flags & SND_Cul_SF_FxA)
			return SND_Cte_FxCoreB;
		else
			return 0;

	case SND_Cte_GetInstFadeOut:
		if(pst_SI->pst_SModifier)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			SND_tdst_SModifier			*pSMd;
			SND_tdst_SModifierExtPlayer *pPlay;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pSMd = SND_pst_SModifierGet(pst_SI->pst_SModifier, SND_Cte_SModifierExtPlayer, 0);
			if(pSMd && pSMd->pv_Data)
			{
				pPlay = (SND_tdst_SModifierExtPlayer*)pSMd->pv_Data;
				if(pPlay->pst_FadeOut) return 1;
			}
		}

		return 0;

	case SND_Cte_GetInstFadeIn:
		if(pst_SI->pst_SModifier)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			SND_tdst_SModifier			*pSMd;
			SND_tdst_SModifierExtPlayer *pPlay;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pSMd = SND_pst_SModifierGet(pst_SI->pst_SModifier, SND_Cte_SModifierExtPlayer, 0);
			if(pSMd && pSMd->pv_Data)
			{
				pPlay = (SND_tdst_SModifierExtPlayer*)pSMd->pv_Data;
				if(pPlay->pst_FadeIn) return 1;
			}
		}

	case SND_Cte_GetInstLoop:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			SND_tdst_OneSound				*pst_Sound;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			SND_M_GetSoundOrReturn(pst_SI->l_Sound, pst_Sound, -1);
			return pst_Sound->pst_Wave->b_HasLoop ? 1 : 0;
		}


		return 0;

	case SND_Cte_GetInstStreamPlayingStatus:
#ifdef JADEFUSION
		pst_SI->pst_LI->pfi_SndBuffGetStatus((SND_tdst_SoundBuffer_*)pst_SI->pst_DSB, (int*)&l_Status);
#else
		pst_SI->pst_LI->pfi_SndBuffGetStatus(pst_SI->pst_DSB, &l_Status);
#endif
		return((l_Status & SND_Cul_SF_StreamIsPlayingNow) ? 1 : 0);

	case SND_Cte_GetInstTrack:
		return pst_SI->i_InstTrack;

	case SND_Cte_GetInstFlags:
		return *(int*)&pst_SI->ul_ExtFlags;

	default:
		return -1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_InstanceSetf(int i_index, int i_Rq, float f_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(i_index, pst_SI, -1);
	_SND_DebugSpy(SND_gst_Params.dst_Sound[pst_SI->l_Sound].ul_FileKey, SND_i_InstanceSetf);

	switch(i_Rq)
	{

	/*$1- near ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstNear0:			pst_SI->af_Near[0] = f_Val; break;
	case SND_Cte_SetInstNear1:			pst_SI->af_Near[1] = f_Val; break;
	case SND_Cte_SetInstNear2:			pst_SI->af_Near[2] = f_Val; break;

	/*$1- far ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstFar0:			pst_SI->af_Far[0] = f_Val; break;
	case SND_Cte_SetInstFar1:			pst_SI->af_Far[1] = f_Val; break;
	case SND_Cte_SetInstFar2:			pst_SI->af_Far[2] = f_Val; break;

	/*$1- middle ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstMiddle0:		pst_SI->af_Middle[0] = f_Val; break;
	case SND_Cte_SetInstMiddle1:		pst_SI->af_Middle[1] = f_Val; break;
	case SND_Cte_SetInstMiddle2:		pst_SI->af_Middle[2] = f_Val; break;

	/*$1- middle coeff ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstCylinderHeight: pst_SI->f_CylinderHeight = f_Val; break;

	/*$1- middle coeff ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstMiddleCoeff:	pst_SI->f_MiddleCoeff = f_Val; break;

	/*$1- far coeff ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstFarCoeff:		pst_SI->f_FarCoeff = f_Val; break;

	/*$1- delta far ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstDeltaFar:		pst_SI->f_DeltaFar = f_Val; break;

	/*$1- minpan ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstMinPan:			pst_SI->f_MinPan = f_Val; break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstDelay:			pst_SI->f_Delay = f_Val; break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstStartDate:		pst_SI->f_StartDate = f_Val; break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstDoppler:		pst_SI->f_Doppler = f_Val; break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstFxVolRight:		SND_M_VolumeSaturation(f_Val); pst_SI->f_FxVolRight = f_Val; break;
	case SND_Cte_SetInstFxVolLeft:		SND_M_VolumeSaturation(f_Val); pst_SI->f_FxVolLeft = f_Val; break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	default:							return -1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_InstanceGetf(int i_index, int i_Rq)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	SND_tdst_OneSound		*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(i_index, pst_SI, -1.0f);

	switch(i_Rq)
	{

	/*$1- near ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstNear0:
		return pst_SI->af_Near[0];

	case SND_Cte_SetInstNear1:
		return pst_SI->af_Near[1];

	case SND_Cte_SetInstNear2:
		return pst_SI->af_Near[2];

	/*$1- far ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstFar0:
		return pst_SI->af_Far[0];

	case SND_Cte_SetInstFar1:
		return pst_SI->af_Far[1];

	case SND_Cte_SetInstFar2:
		return pst_SI->af_Far[2];

	/*$1- middle ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstMiddle0:
		return pst_SI->af_Middle[0];

	case SND_Cte_SetInstMiddle1:
		return pst_SI->af_Middle[1];

	case SND_Cte_SetInstMiddle2:
		return pst_SI->af_Middle[2];

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstCylinderHeight:
		return pst_SI->f_CylinderHeight;

	/*$1- middle coeff ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstMiddleCoeff:
		return pst_SI->f_MiddleCoeff;

	/*$1- far coeff ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstFarCoeff:
		return pst_SI->f_FarCoeff;

	/*$1- delta far ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstDeltaFar:
		return pst_SI->f_DeltaFar;

	/*$1- minpan ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstMinPan:
		return pst_SI->f_MinPan;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstDelay:
		return pst_SI->f_Delay;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstStartDate:
		return pst_SI->f_StartDate;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_GetInstDuration:
		pst_Sound = SND_gst_Params.dst_Sound + pst_SI->l_Sound;
		if(pst_Sound->ul_Flags & SND_Cul_DSF_Used)
		{
			return(fLongToFloat(pst_Sound->pst_Wave->ul_DataSize) / fLongToFloat(pst_Sound->pst_Wave->dwAvgBytesPerSec));
		}
		else
		{
			return 0.0f;
		}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstDoppler:
		return pst_SI->f_Doppler;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case SND_Cte_SetInstFxVolRight:
		return pst_SI->f_FxVolRight;

	case SND_Cte_SetInstFxVolLeft:
		return pst_SI->f_FxVolLeft;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	default:
		return -1.0f;
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
