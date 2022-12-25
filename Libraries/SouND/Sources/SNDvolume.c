/*$T SNDvolume.c GC 1.138 02/21/05 11:09:55 */


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

#include "BASe/BAStypes.h"
#include "MATHs/MATHfloat.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDloadingsound.h"

#ifdef ACTIVE_EDITORS
#include "EDItors/Sources/SOuNd/SONutil.h"
#endif

#if defined(_XENON)
#include "xmp.h"
#endif
/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
#define SND_M_GetGroupVol(a)	(SND_gdst_Group[(a)].b_ByPass ? 1.0f : SND_gdst_Group[(a)].f_Volume)
#else
#define SND_M_GetGroupVol(a)	(SND_gdst_Group[(a)].f_Volume)
#endif

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static float	SND_f_ComputeGroupFactor(SND_tdst_SoundInstance *);
static float	SND_f_SphericComputing(MATH_tdst_Vector *, SND_tdst_SoundInstance *, int *);
static float	SND_f_AxisComputing(MATH_tdst_Vector *, SND_tdst_SoundInstance *, int _i_axis, int *);
static void	SND_GetInstancePosition(SND_tdst_SoundInstance *_pst_SI, MATH_tdst_Vector *_pst_Pos);
void            SND_GroupVolumeBackWrite(int Id);
/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

static LONG		SND_al_AttTable[256];			/* volume table */
static float	SND_af_VolTable[256];			/* volume table */
float			SND_gf_AutoVolumeOff = 1.0f;	/* case 1024 truncated files */
SND_tdst_Group	SND_gdst_Group[SND_e_GrpNumber];

#ifdef _DEBUG
char*SND_gaz_GroupName[] =
{
	"GrpSpecialFX",
	"GrpMusic"	,	
	"GrpAmbience",		
	"GrpDialog"	,	
	"GrpCutScene",		
	"GrpInterface",	
	"GrpA"		,	
	"GrpB"		,	
	"MasterGrp"	,	
	"UserGrpMusic"	,
	"UserGrpDialog"	,
	"UserGrpSpecialFX",
	"UserGrpMaster"	
};
#endif
/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_l_InitVolumeTable(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 256; i++)
	{
		SND_al_AttTable[i] = lFloatToLong((float) SND_Cte_MinAtt * fLongToFloat(i) / 255.0f);
		SND_af_VolTable[i] = (float) L_pow(10.0f, fLongToFloat(SND_al_AttTable[i]) / 2000.0f);
	}

    SND_ReinitGroupRequest();
}


/*$4
 ***********************************************************************************************************************
    GLOBAL VOLUME / PAN
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float f_Blend(float X, float x1, float y1, float x2, float y2)
{
	return((y1 - y2) / (x1 - x2) * (X - x1)) + y1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG SND_l_GetAttFromVol(float vol)
{
	/*~~~~~~~~~~~*/
	int i1, i2, iM;
	/*~~~~~~~~~~~*/

	i1 = 255;
	i2 = 0;
	if(vol >= SND_af_VolTable[i2]) return SND_Cte_MaxAtt;
	if(vol <= SND_af_VolTable[i1]) return SND_Cte_MinAtt;

	while((i1 - i2) > 1)
	{
		iM = (i2 + i1) / 2;
		if(SND_af_VolTable[iM] < vol)
			i1 = iM;
		else
			i2 = iM;
	}

	return lFloatToLong
		(
			f_Blend
			(
				vol,
				SND_af_VolTable[i1],
				fLongToFloat(SND_al_AttTable[i1]),
				SND_af_VolTable[i2],
				fLongToFloat(SND_al_AttTable[i2])
			)
		);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GetVolFromAtt(LONG _l_Att)
{
	/*~~~~~~~~~~~*/
	int i1, i2, iM;
	/*~~~~~~~~~~~*/

	i1 = 255;
	i2 = 0;
	if(_l_Att >= SND_al_AttTable[i2]) return 1.0f;
	if(_l_Att <= SND_al_AttTable[i1]) return 0.0f;

	while((i1 - i2) > 1)
	{
		iM = (i2 + i1) / 2;
		if(SND_al_AttTable[iM] < _l_Att)
			i1 = iM;
		else
			i2 = iM;
	}

	return f_Blend
		(
			fLongToFloat(_l_Att),
			fLongToFloat(SND_al_AttTable[i1]),
			SND_af_VolTable[i1],
			fLongToFloat(SND_al_AttTable[i2]),
			SND_af_VolTable[i2]
		);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetGlobalVol(float _f_Volume)
{
	if(!SND_gst_Params.l_Available) return;
	SND_gpst_Interface[ISound].pfv_GlobalSetVol(SND_l_GetAttFromVol(_f_Volume));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GetGlobalVol(void)
{
	if(!SND_gst_Params.l_Available) return 0.0f;
	return SND_f_GetVolFromAtt(SND_gpst_Interface[ISound].pfl_GlobalGetVol());
}

/*$4
 ***********************************************************************************************************************
    GROUP
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static float SND_f_ComputeGroupFactor(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~~*/
	float	f_mute;
#ifdef ACTIVE_EDITORS
	BOOL	b_Solo;
	BOOL	b_Mute;
#endif
	/*~~~~~~~~~~~*/

	if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) return 1.0f;

#if defined(_XENON)

	// Xenon Media Player Overriding Title Music (TCR)

	if (SND_M_GetGrp(pst_SI->i_GroupId) == SND_Cte_GrpMusic)
	{
		BOOL bTitlePlaybackControl = TRUE;
		DWORD dwSuccess;

		dwSuccess = XMPTitleHasPlaybackControl(&bTitlePlaybackControl);

		if ((dwSuccess == ERROR_SUCCESS) && !bTitlePlaybackControl)
		{
			return 0.0f;
		}
	}
#endif

#ifdef ACTIVE_EDITORS
	if(SND_gdst_Group[SND_e_MasterGrp].b_Solo)
	{
		switch(SND_M_GetGrp(pst_SI->i_GroupId))
		{
		case SND_Cte_GrpMusic:		b_Solo = SND_gdst_Group[SND_e_GrpMusic].b_Solo; break;
		case SND_Cte_GrpDialog:		b_Solo = SND_gdst_Group[SND_e_GrpDialog].b_Solo; break;
		case SND_Cte_GrpSpecialFX:	b_Solo = SND_gdst_Group[SND_e_GrpSpecialFX].b_Solo; break;
		case SND_Cte_GrpAmbience:	b_Solo = SND_gdst_Group[SND_e_GrpAmbience].b_Solo; break;
		case SND_Cte_GrpCutScene:	b_Solo = SND_gdst_Group[SND_e_GrpCutScene].b_Solo; break;
		case SND_Cte_GrpInterface:	b_Solo = SND_gdst_Group[SND_e_GrpInterface].b_Solo; break;
		default:					b_Solo = FALSE; break;
		}

		if((pst_SI->i_GroupId & SND_Cte_GrpA) && SND_gdst_Group[SND_e_GrpA].b_Solo) b_Solo = TRUE;
		if((pst_SI->i_GroupId & SND_Cte_GrpB) && SND_gdst_Group[SND_e_GrpB].b_Solo) b_Solo = TRUE;
		if(!b_Solo) return 0.0f;
	}
	else if(SND_gdst_Group[SND_e_UserGrpMaster].b_Solo)
	{
		b_Solo = FALSE;
		if((pst_SI->i_GroupId & SND_Cte_UserGrpMusic) && SND_gdst_Group[SND_e_UserGrpMusic].b_Solo) b_Solo = TRUE;
		if((pst_SI->i_GroupId & SND_Cte_UserGrpDialog) && SND_gdst_Group[SND_e_UserGrpDialog].b_Solo) b_Solo = TRUE;
		if((pst_SI->i_GroupId & SND_Cte_UserGrpSpecialFX) && SND_gdst_Group[SND_e_UserGrpSpecialFX].b_Solo)
			b_Solo = TRUE;
		if(!b_Solo) return 0.0f;
	}
	else if(SND_gdst_Group[SND_e_MasterGrp].b_Mute)
	{
		switch(SND_M_GetGrp(pst_SI->i_GroupId))
		{
		case SND_Cte_GrpMusic:		b_Mute = SND_gdst_Group[SND_e_GrpMusic].b_Mute; break;
		case SND_Cte_GrpDialog:		b_Mute = SND_gdst_Group[SND_e_GrpDialog].b_Mute; break;
		case SND_Cte_GrpSpecialFX:	b_Mute = SND_gdst_Group[SND_e_GrpSpecialFX].b_Mute; break;
		case SND_Cte_GrpAmbience:	b_Mute = SND_gdst_Group[SND_e_GrpAmbience].b_Mute; break;
		case SND_Cte_GrpCutScene:	b_Mute = SND_gdst_Group[SND_e_GrpCutScene].b_Mute; break;
		case SND_Cte_GrpInterface:	b_Mute = SND_gdst_Group[SND_e_GrpInterface].b_Mute; break;
		default:					b_Mute = FALSE; break;
		}

		if((pst_SI->i_GroupId & SND_Cte_GrpA) && SND_gdst_Group[SND_e_GrpA].b_Mute) b_Mute = TRUE;
		if((pst_SI->i_GroupId & SND_Cte_GrpB) && SND_gdst_Group[SND_e_GrpB].b_Mute) b_Mute = TRUE;
		if(b_Mute) return 0.0f;
	}
	else if(SND_gdst_Group[SND_e_UserGrpMaster].b_Mute)
	{
		b_Mute = FALSE;
		if((pst_SI->i_GroupId & SND_Cte_UserGrpMusic) && SND_gdst_Group[SND_e_UserGrpMusic].b_Mute) b_Mute = TRUE;
		if((pst_SI->i_GroupId & SND_Cte_UserGrpDialog) && SND_gdst_Group[SND_e_UserGrpDialog].b_Mute) b_Mute = TRUE;
		if((pst_SI->i_GroupId & SND_Cte_UserGrpSpecialFX) && SND_gdst_Group[SND_e_UserGrpSpecialFX].b_Mute)
			b_Mute = TRUE;
		if(b_Mute) return 0.0f;
	}

	pst_SI->ul_EdiFlags &= ~(SND_EdiFlg_Mute | SND_EdiFlg_Solo);
	if(!(pst_SI->ul_EdiFlags & SND_EdiFlg_EdiUse))
	{
		if(ESON_b_SoloIsActive())
		{
			if(ESON_b_InstanceIsSolo(SND_M_GetInstanceFromPtr(pst_SI)))
			{
				pst_SI->ul_EdiFlags |= SND_EdiFlg_Solo;
			}
			else
			{
				pst_SI->ul_EdiFlags |= SND_EdiFlg_Mute;
				return 0.0f;
			}
		}
		else if(ESON_b_MuteIsActive())
		{
			if(ESON_b_InstanceIsMuted(SND_M_GetInstanceFromPtr(pst_SI)))
			{
				pst_SI->ul_EdiFlags |= SND_EdiFlg_Mute;
				return 0.0f;
			}
		}
	}
#endif

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(SND_M_GetGrp(pst_SI->i_GroupId))
	{
	case SND_Cte_GrpMusic:      f_mute = SND_M_GetGroupVol(SND_e_GrpMusic);     break;
	case SND_Cte_GrpSpecialFX:  f_mute = SND_M_GetGroupVol(SND_e_GrpSpecialFX); break;
	case SND_Cte_GrpDialog:     f_mute = SND_M_GetGroupVol(SND_e_GrpDialog)  ;  break;
	case SND_Cte_GrpAmbience:   f_mute = SND_M_GetGroupVol(SND_e_GrpAmbience);  break;
    case SND_Cte_GrpCutScene:   f_mute = SND_M_GetGroupVol(SND_e_GrpCutScene);  break;
	case SND_Cte_GrpInterface:  f_mute = SND_M_GetGroupVol(SND_e_GrpInterface); break;
	default:                    f_mute = 1.0f; break;//SND_M_GetGroupVol(SND_e_GrpSpecialFX); break;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_SI->i_GroupId & SND_Cte_GrpA) f_mute += SND_M_GetGroupVol(SND_e_GrpA);
	if(pst_SI->i_GroupId & SND_Cte_GrpB) f_mute += SND_M_GetGroupVol(SND_e_GrpB);
    

	/*$1- wac ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_mute *= SND_f_GetWacMute(pst_SI);

	/*$1- master ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(SND_M_GetGrp(pst_SI->i_GroupId))
	{
	case SND_Cte_GrpMusic:      f_mute *= SND_M_GetGroupVol(SND_e_UserGrpMusic);        break;
	case SND_Cte_GrpSpecialFX:  f_mute *= SND_M_GetGroupVol(SND_e_UserGrpSpecialFX);    break;
	case SND_Cte_GrpDialog:     f_mute *= SND_M_GetGroupVol(SND_e_UserGrpDialog);       break;
	case SND_Cte_GrpAmbience:   f_mute *= SND_M_GetGroupVol(SND_e_UserGrpSpecialFX);    break;
	case SND_Cte_GrpCutScene:   f_mute *= SND_M_GetGroupVol(SND_e_UserGrpDialog);       break;
	case SND_Cte_GrpInterface:  f_mute *= SND_M_GetGroupVol(SND_e_UserGrpSpecialFX);    break;
	default:                 /*  f_mute *= SND_M_GetGroupVol(SND_e_UserGrpSpecialFX);   */ break;
	}

	f_mute *= SND_M_GetGroupVol(SND_e_UserGrpMaster);
	f_mute *= SND_M_GetGroupVol(SND_e_MasterGrp);

	return f_mute;
}

/*$4
 ***********************************************************************************************************************
    INSTANCE VOLUME
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_SetInstVolume(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~*/
	float	f_vol;
	/*~~~~~~~~~~*/

    if(pst_SI->ul_Flags & SND_Cul_InactiveInstance)
        f_vol = 0.0f;
    else
	    f_vol = SND_f_GetInstVolume(pst_SI);
    
	pst_SI->pst_LI->pfv_SndBuffSetVol(pst_SI->pst_DSB, SND_l_GetAttFromVol(f_vol));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GetInstVolume(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~*/
	float	f_vol;
	/*~~~~~~~~~~*/

#ifndef _FINAL_
	if(SND_gst_Params.ul_Flags & SND_Cte_ForceNoFloatVol) return pst_SI->f_Volume;
#endif
	f_vol = pst_SI->pst_LI->pff_Compute3DVol(pst_SI);
	f_vol *= SND_f_ComputeGroupFactor(pst_SI);
	f_vol *= SND_gf_AutoVolumeOff;
	SND_M_VolumeSaturation(f_vol);

	return f_vol;
}

/*$4
 ***********************************************************************************************************************
    3D VOLUME
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void SND_GetInstancePosition(SND_tdst_SoundInstance *_pst_SI, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	stPos, stDist, stNormAxis;
	float				fScale;
	extern void			AI_EvalFunc_OBJGetPos_C(OBJ_tdst_GameObject *, MATH_tdst_Vector *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* get pos */
	if(_pst_SI->p_GameObject)
		AI_EvalFunc_OBJGetPos_C((OBJ_tdst_GameObject *) _pst_SI->p_GameObject, _pst_Pos);
	else
		MATH_CopyVector(_pst_Pos, _pst_SI->pst_GlobalPos);

	/* return if there is no cylinder height */
	if(!_pst_SI->f_CylinderHeight) return;

	/* else compute translation */
	if(_pst_SI->ul_Flags & SND_Cul_SF_DynVolXaxis)
		MATH_CopyVector(&stPos, MATH_pst_GetXAxis(_pst_SI->pst_GlobalMatrix));
	else if(_pst_SI->ul_Flags & SND_Cul_SF_DynVolYaxis)
		MATH_CopyVector(&stPos, MATH_pst_GetYAxis(_pst_SI->pst_GlobalMatrix));
	else if(_pst_SI->ul_Flags & SND_Cul_SF_DynVolZaxis)
		MATH_CopyVector(&stPos, MATH_pst_GetZAxis(_pst_SI->pst_GlobalMatrix));
	else
		MATH_InitVectorToZero(&stPos);
	MATH_NormalizeAnyVector(&stNormAxis, &stPos);

	MATH_SubVector(&stDist, _pst_SI->pst_GlobalPos, MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol));
	fScale = MATH_f_DotProduct(&stDist, &stNormAxis);

	if(fScale >= 0.0f)
		fScale = -fMin(fScale, _pst_SI->f_CylinderHeight);
	else
		fScale = -fMax(fScale, -_pst_SI->f_CylinderHeight);

	MATH_AddScaleVector(_pst_Pos, _pst_SI->pst_GlobalPos, &stNormAxis, fScale);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_Compute3DVol(SND_tdst_SoundInstance *pst_SI)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Factor;
	float				f_Volume;
	LONG				l_Pan, l_PanN;
	MATH_tdst_Vector	*y1, y2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- check format -----------------------------------------------------------------------------------------------*/

	if(!SND_gst_Params.pst_RefForVol) return pst_SI->f_Volume;
	if(!pst_SI->pst_GlobalPos) return pst_SI->f_Volume;
	if(!(pst_SI->ul_Flags & SND_Cul_SF_MaskDynVol)) return pst_SI->f_Volume;

	/*$2- 3D volume --------------------------------------------------------------------------------------------------*/

	if(pst_SI->ul_Flags & SND_Cul_SF_DynVolSpheric)
	{
		/*~~~~~~~~~~*/
		int i_Command;
		/*~~~~~~~~~~*/

		f_Factor = SND_f_SphericComputing(MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol), pst_SI, &i_Command);
		if(i_Command & 1) SND_AutoStop(pst_SI);
		if(i_Command & 2) SND_AutoPlay(pst_SI);
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		float	af_Factor[3];
		int		ai_Command[3] = { SND_Cul_SF_DynVolXaxis, SND_Cul_SF_DynVolYaxis, SND_Cul_SF_DynVolZaxis };
		int		axis;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(axis = 0; axis < 3; axis++)
		{
			if(pst_SI->ul_Flags & ai_Command[axis])
			{
				af_Factor[axis] = SND_f_AxisComputing
					(
						MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol),
						pst_SI,
						axis,
						&ai_Command[axis]
					);
			}
			else
			{
				af_Factor[axis] = 1.0f;
				ai_Command[axis] = 2;
			}
		}

		f_Factor = af_Factor[0] * af_Factor[1] * af_Factor[2];

		if((ai_Command[0] | ai_Command[1] | ai_Command[2]) & 1) SND_AutoStop(pst_SI);
		if((ai_Command[0] & ai_Command[1] & ai_Command[2]) & 2) SND_AutoPlay(pst_SI);
	}

	f_Volume = f_Factor * pst_SI->f_Volume;

	/*$2- Dynamic pan ------------------------------------------------------------------------------------------------*/

	if(pst_SI->ul_Flags & SND_Cul_SF_DynPan)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	stPos;
		/*~~~~~~~~~~~~~~~~~~~~~~*/

		if(SND_gst_Params.pst_RefForPan)
		{
			SND_GetInstancePosition(pst_SI, &stPos);
			MATH_SubVector(&y2, &stPos, MATH_pst_GetTranslation(SND_gst_Params.pst_RefForPan));

			if(MATH_f_NormVector(&y2) == 0.0f)
			{
				l_Pan = 0;
				l_PanN = 0;
			}
			else
			{
				MATH_NormalizeVector(&y2, &y2);

				/* left(-)-right(+) */
				y1 = MATH_pst_GetXAxis(SND_gst_Params.pst_RefForPan);
				f_Factor = MATH_f_DotProduct(y1, &y2);
				f_Factor *= SND_gst_Params.f_SurroundFactor;

				if(f_Factor > 1.0f) f_Factor = 1.0f;
				if(f_Factor < -1.0f) f_Factor = -1.0f;

				if(f_Factor > 0.0f)
				{
					f_Factor = 1.0f - f_Factor + pst_SI->f_MinPan;
					SND_M_VolumeSaturation(f_Factor);
					l_Pan = -SND_l_GetAttFromVol(f_Factor);
				}
				else if(f_Factor < 0.0f)
				{
					f_Factor = 1.0f + f_Factor + pst_SI->f_MinPan;
					SND_M_VolumeSaturation(f_Factor);
					l_Pan = SND_l_GetAttFromVol(f_Factor);
				}
				else
				{
					l_Pan = 0;
				}

				switch(SND_gst_Params.ul_RenderMode)
				{
				case SND_Cte_RenderDolbyDigital:
				case SND_Cte_RenderDolbyPrologic:
				case SND_Cte_RenderDolbyPrologicII:
					/* front(+)/rear(-) */
					y1 = MATH_pst_GetZAxis(SND_gst_Params.pst_RefForPan);
					f_Factor = MATH_f_DotProduct(y1, &y2);
					f_Factor *= 2.0f;	/* SND_gst_Params.f_SurroundFactor; */

					if(f_Factor > 0.0f)
					{
						f_Factor = 1.0f - f_Factor + pst_SI->f_MinPan;
						SND_M_VolumeSaturation(f_Factor);
						l_PanN = -SND_l_GetAttFromVol(f_Factor);
					}
					else if(f_Factor < 0.0f)
					{
						f_Factor = 1.0f + f_Factor + pst_SI->f_MinPan;
						SND_M_VolumeSaturation(f_Factor);
						l_PanN = SND_l_GetAttFromVol(f_Factor);
					}
					else
					{
						l_PanN = 0;
					}
					break;

				default:
					l_PanN = 0;
					break;
				}
			}
		}
		else
		{
			l_Pan = l_PanN = 0;
		}

		pst_SI->pst_LI->pfv_SndBuffSetPan(pst_SI->pst_DSB, l_Pan, l_PanN);
		pst_SI->i_Pan = l_Pan;
		pst_SI->i_Front = l_PanN;
	}

	return f_Volume;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static float SND_f_SphericComputing(MATH_tdst_Vector *_pst_micro_pos, SND_tdst_SoundInstance *_pst_SI, int *_pi_command)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_dist;
	float				f_Vn, f_Dn;
	float				f_Vf, f_Df;
	float				f_Factor;
#ifdef ACTIVE_EDITORS
	char				az_msg[128];
#endif
	MATH_tdst_Vector	stPos;
	float				f_sqr_Df, f_sqr_Dn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(ACTIVE_EDITORS) && defined(_DEBUG)
	sprintf(az_msg, "Middle distance lower than Near distance, instance %d", SND_M_GetInstanceFromPtr(_pst_SI));
	ERR_X_Warning(_pst_SI->af_Near[0] <= _pst_SI->af_Middle[0], az_msg, NULL);

	sprintf(az_msg, "Far distance lower than Middle distance, instance %d", SND_M_GetInstanceFromPtr(_pst_SI));
	ERR_X_Warning(_pst_SI->af_Middle[0] <= _pst_SI->af_Far[0], az_msg, NULL);

	sprintf(az_msg, "Middle factor lower than Far factor, instance %d", SND_M_GetInstanceFromPtr(_pst_SI));

	/* ERR_X_Warning(_pst_SI->f_MiddleCoeff >= _pst_SI->f_FarCoeff, az_msg, NULL); */
#endif
	*_pi_command = 0;

	SND_GetInstancePosition(_pst_SI, &stPos);
	f_dist = MATH_f_Distance(_pst_micro_pos, &stPos);

	if(f_dist >= _pst_SI->af_Far[0])
	{
		if((_pst_SI->ul_ExtFlags & (SND_Cul_ESF_AutoStopOutsideFar | SND_Cul_ESF_StopByFarTest)) == SND_Cul_ESF_AutoStopOutsideFar)
		{
			if(f_dist > (2 * _pst_SI->f_DeltaFar + _pst_SI->af_Far[0])) *_pi_command |= 1;
		}

		if
		(
			(_pst_SI->ul_ExtFlags & (SND_Cul_ESF_AutoPlayInsideFar | SND_Cul_ESF_StopByFarTest)) ==
				(SND_Cul_ESF_AutoPlayInsideFar | SND_Cul_ESF_StopByFarTest)
		)
		{
			if(f_dist < (_pst_SI->af_Far[0] + _pst_SI->f_DeltaFar)) *_pi_command |= 2;
		}
	}

	f_dist *= SND_gst_Params.f_RolloffFactor;

	if(f_dist <= _pst_SI->af_Near[0])
		return 1.0f;
	else if(f_dist <= _pst_SI->af_Middle[0])
	{
		f_Vn = 1.0f;
		f_Dn = _pst_SI->af_Near[0];
		f_Vf = _pst_SI->f_MiddleCoeff;
		f_Df = _pst_SI->af_Middle[0];
	}
	else if(f_dist < _pst_SI->af_Far[0])
	{
		f_Vf = _pst_SI->f_FarCoeff;
		f_Df = _pst_SI->af_Far[0];
		f_Vn = _pst_SI->f_MiddleCoeff;
		f_Dn = _pst_SI->af_Middle[0];
	}
	else
		return _pst_SI->f_FarCoeff;

	f_sqr_Df = fSqr(f_Df);
	f_sqr_Dn = fSqr(f_Dn);
	f_Factor = (fInv(fSqr(f_dist)) - fInv(f_sqr_Df)) * (f_sqr_Df * f_sqr_Dn) / (f_sqr_Df - f_sqr_Dn) * (f_Vn - f_Vf) + f_Vf;

	return f_Factor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static float SND_f_AxisComputing
(
	MATH_tdst_Vector		*_pst_micro_pos,
	SND_tdst_SoundInstance	*_pst_SI,
	int						_i_axis,
	int						*_pi_command
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Vect;
	float				f_dist;
	float				f_Vn, f_Dn;
	float				f_Vf, f_Df;
	float				f_Factor;
	float				f_sqr_Df, f_sqr_Dn;
#if defined(ACTIVE_EDITORS) && defined(_DEBUG)
	char				az_msg[128];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_SI->af_Near[_i_axis] > _pst_SI->af_Middle[_i_axis])
	{
		sprintf(az_msg, "Middle distance lower than Near distance, instance %d", SND_M_GetInstanceFromPtr(_pst_SI));
		ERR_X_Warning(0, az_msg, NULL);
	}

	if(_pst_SI->af_Middle[_i_axis] > _pst_SI->af_Far[_i_axis])
	{
		sprintf(az_msg, "Far distance lower than Middle distance, instance %d", SND_M_GetInstanceFromPtr(_pst_SI));
		ERR_X_Warning(0, az_msg, NULL);
	}

	if(_pst_SI->af_Near[_i_axis] > _pst_SI->af_Far[_i_axis])
	{
		sprintf(az_msg, "Far distance lower than Near distance, instance %d", SND_M_GetInstanceFromPtr(_pst_SI));
		ERR_X_Warning(0, az_msg, NULL);
	}

	if(_pst_SI->f_MiddleCoeff < _pst_SI->f_FarCoeff)
	{
		sprintf(az_msg, "Middle factor lower than Far factor, instance %d", SND_M_GetInstanceFromPtr(_pst_SI));
		ERR_X_Warning(0, az_msg, NULL);
	}
#endif
	*_pi_command = 0;

	/* eval dist between the plane and the micro */
	MATH_SubVector(&st_Vect, _pst_micro_pos, _pst_SI->pst_GlobalPos);
	switch(_i_axis)
	{
	case 0:		f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetXAxis(_pst_SI->pst_GlobalMatrix))); break;
	case 1:		f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetYAxis(_pst_SI->pst_GlobalMatrix))); break;
	case 2:		f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetZAxis(_pst_SI->pst_GlobalMatrix))); break;
	default:	ERR_X_Warning(0, "Bad axis id (SND_f_AxisComputing)", NULL); f_dist = 0.0f; return 0.0f;
	}

	/* (in)activation test */
	if((_pst_SI->ul_ExtFlags & (SND_Cul_ESF_AutoStopOutsideFar | SND_Cul_ESF_StopByFarTest)) == SND_Cul_ESF_AutoStopOutsideFar)
	{
		if(f_dist > (2 * _pst_SI->f_DeltaFar + _pst_SI->af_Far[_i_axis])) *_pi_command |= 1;
	}

	if
	(
		(_pst_SI->ul_ExtFlags & (SND_Cul_ESF_AutoPlayInsideFar | SND_Cul_ESF_StopByFarTest)) ==
			(SND_Cul_ESF_AutoPlayInsideFar | SND_Cul_ESF_StopByFarTest)
	)
	{
		if(f_dist < (_pst_SI->af_Far[_i_axis] + _pst_SI->f_DeltaFar)) *_pi_command |= 2;
	}

	f_dist *= SND_gst_Params.f_RolloffFactor;

	if(f_dist <= _pst_SI->af_Near[_i_axis])
		return 1.0f;
	else if(f_dist <= _pst_SI->af_Middle[_i_axis])
	{
		f_Vn = 1.0f;
		f_Dn = _pst_SI->af_Near[_i_axis];
		f_Vf = _pst_SI->f_MiddleCoeff;
		f_Df = _pst_SI->af_Middle[_i_axis];
	}
	else if(f_dist < _pst_SI->af_Far[_i_axis])
	{
		f_Vf = _pst_SI->f_FarCoeff;
		f_Df = _pst_SI->af_Far[_i_axis];
		f_Vn = _pst_SI->f_MiddleCoeff;
		f_Dn = _pst_SI->af_Middle[_i_axis];
	}
	else
		return _pst_SI->f_FarCoeff;

	f_sqr_Df = fSqr(f_Df);
	f_sqr_Dn = fSqr(f_Dn);
	f_Factor = (fInv(fSqr(f_dist)) - fInv(f_sqr_Df)) * (f_sqr_Df * f_sqr_Dn) / (f_sqr_Df - f_sqr_Dn) * (f_Vn - f_Vf) + f_Vf;
	return f_Factor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SND_b_IsInActiveVolume(LONG _i_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float					y;
	SND_tdst_SoundInstance	*pst_SI;
	MATH_tdst_Vector		stPos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.pst_RefForVol) return TRUE;

	SND_M_GetInstanceOrReturn(_i_Index, pst_SI, FALSE);

	/* dyn vol */
	if
	(
		!(
			pst_SI->ul_Flags &
				(
					SND_Cul_SF_DynVolSpheric |
					SND_Cul_SF_DynVolXaxis |
					SND_Cul_SF_DynVolYaxis |
					SND_Cul_SF_DynVolZaxis
				)
		)
	) return FALSE;

	/* need pos */
	if(!pst_SI->pst_GlobalPos) return FALSE;

	/* if axis, need matrix */
	if
	(
		!pst_SI->pst_GlobalMatrix
	&&	(pst_SI->ul_Flags & (SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis))
	) return FALSE;

	if(pst_SI->ul_Flags & SND_Cul_SF_DynVolSpheric)
	{
		SND_GetInstancePosition(pst_SI, &stPos);
		y = MATH_f_Distance(MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol), &stPos);
		if(y < (pst_SI->af_Far[0] + pst_SI->f_DeltaFar)) return TRUE;

		return FALSE;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		int					ai_Command[3] = { SND_Cul_SF_DynVolXaxis, SND_Cul_SF_DynVolYaxis, SND_Cul_SF_DynVolZaxis };
		MATH_tdst_Vector	st_Vect;
		float				f_dist;
		int					axis;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_SubVector(&st_Vect, MATH_pst_GetTranslation(SND_gst_Params.pst_RefForVol), pst_SI->pst_GlobalPos);
		for(axis = 0; axis < 3; axis++)
		{
			if(pst_SI->ul_Flags & ai_Command[axis])
			{
				switch(axis)
				{
				case 0:
					f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetXAxis(pst_SI->pst_GlobalMatrix)));
					break;

				case 1:
					f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetYAxis(pst_SI->pst_GlobalMatrix)));
					break;

				case 2:
					f_dist = fAbs(MATH_f_DotProduct(&st_Vect, MATH_pst_GetZAxis(pst_SI->pst_GlobalMatrix)));
					break;

				default:
					f_dist = 0.0f;
				}

				if(f_dist < (pst_SI->af_Far[axis] + pst_SI->f_DeltaFar))
					ai_Command[axis] |= 2;
				else
					ai_Command[axis] = 0;
			}
			else
			{
				ai_Command[axis] = 2;
			}
		}

		if((ai_Command[0] & ai_Command[1] & ai_Command[2]) & 2) return TRUE;

		return FALSE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_MuteAll(BOOL b_Mute)
{
	int i;

	// TRC only
	for(i = 0; i < SND_e_GrpNumber; i++)
	{
		SND_gdst_Group[i].ul_VolRqFlags |= 0x80000000; // mark as used
		SND_GroupSendRequest(i, SND_Cte_GrpVolRqMax-1, b_Mute ? 0.0f : 1.0f);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_GroupVolumeSet(int Id, float vol)
{
	SND_M_VolumeSaturation(vol);
	switch(Id)
	{
	case SND_e_GrpSpecialFX:
	case SND_e_GrpMusic:
	case SND_e_GrpAmbience:
	case SND_e_GrpDialog:
	case SND_e_GrpCutScene:
	case SND_e_GrpInterface:
	/**/
	case SND_e_GrpA:
	case SND_e_GrpB:
	/**/
	case SND_e_MasterGrp:
	/**/
	case SND_e_UserGrpMusic:
	case SND_e_UserGrpDialog:
	case SND_e_UserGrpSpecialFX:
	case SND_e_UserGrpMaster:
        SND_GroupSendRequest(Id, 0, vol);
		break;

	default:
		ERR_X_Warning(0, "[SND] SNDvolume.c : bad group id", NULL);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_GroupVolumeBackWrite(int Id)
{
	/*~~*/
	int i;
	/*~~*/

	if(SND_gdst_Group[Id].ul_VolRqChanged)
	{
        SND_gdst_Group[Id].f_Volume = SND_gdst_Group[Id].af_VolRq[0];
        
		for(i = 1; i < SND_Cte_GrpVolRqMax; i++)
		{
			if(SND_gdst_Group[Id].ul_VolRqFlags & (1 << i))
				SND_gdst_Group[Id].f_Volume *= SND_gdst_Group[Id].af_VolRq[i];
		}
	}

	SND_gdst_Group[Id].ul_VolRqChanged = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_GroupGetRequestId(int Id)
{
	/*~~*/
	int i;
	/*~~*/

	// 0 => is the group volume
	// SND_Cte_GrpVolRqMax-1 => is reserved for TRC needs
	for(i = 1; i < SND_Cte_GrpVolRqMax-1; i++) 
	{
		if(SND_gdst_Group[Id].ul_VolRqFlags & (1 << i)) continue;

		SND_gdst_Group[Id].ul_VolRqFlags |= (1 << i);

		return i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_GroupFreeRequestId(int Id, int i)
{
	SND_gdst_Group[Id].ul_VolRqFlags &= ~(1 << i);
    SND_GroupVolumeBackWrite(Id);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_GroupSendRequest(int Id, int RqId, float f_Volume)
{

#ifdef _DEBUG
	if(SND_gdst_Group[Id].ul_WatchRq & (1 << RqId))
	{
		char log[512];
		
		sprintf
			(
			log, "[SND_GroupWatcher] %s send %d vol %.2f", 
			SND_gaz_GroupName[Id],
			RqId,
			f_Volume
			);
		SND_TraceStr(log);
	}
#endif

	SND_gdst_Group[Id].af_VolRq[RqId] = f_Volume;
	SND_gdst_Group[Id].ul_VolRqChanged |= (1 << RqId);

	SND_GroupVolumeBackWrite(Id);
}

void SND_ReinitGroupRequest(void)
{
    int i;

	for(i = 0; i < SND_e_GrpNumber; i++)
	{

#ifdef _DEBUG
	if(SND_gdst_Group[i].ul_WatchRq )
	{
		char log[512];
		sprintf(log, "[SND_GroupWatcher] %s reinit all send", SND_gaz_GroupName[i]);
		SND_TraceStr(log);
	}
#endif

		SND_gdst_Group[i].ul_VolRqChanged = 0;
		SND_gdst_Group[i].ul_VolRqFlags = 0;

		L_memset(SND_gdst_Group[i].af_VolRq, 0, SND_Cte_GrpVolRqMax*sizeof(float));
		SND_gdst_Group[i].af_VolRq[0] = 1.0f;
        SND_gdst_Group[i].f_Volume = 1.0f;
		SND_gdst_Group[i].f_FxVolume = 1.0f;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GroupVolumeGet(int Id)
{
	SND_GroupVolumeBackWrite(Id);
	switch(Id)
	{
	case SND_e_GrpSpecialFX:
	case SND_e_GrpMusic:
	case SND_e_GrpAmbience:
	case SND_e_GrpDialog:
	case SND_e_GrpCutScene:
	case SND_e_GrpInterface:
	/**/
	case SND_e_GrpA:
	case SND_e_GrpB:
	/**/
	case SND_e_MasterGrp:
	/**/
	case SND_e_UserGrpMusic:
	case SND_e_UserGrpDialog:
	case SND_e_UserGrpSpecialFX:
	case SND_e_UserGrpMaster:
		return SND_gdst_Group[Id].f_Volume;

	default:
		ERR_X_Warning(0, "[SND] SNDvolume.c : bad group id", NULL);
		return 1.0f;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_GroupFxVolumeSet(int Id, float vol)
{
	SND_M_VolumeSaturation(vol);
	switch(Id)
	{
	case SND_e_GrpSpecialFX:
	case SND_e_GrpMusic:
	case SND_e_GrpAmbience:
	case SND_e_GrpDialog:
	case SND_e_GrpCutScene:
	case SND_e_GrpInterface:
	/**/
	case SND_e_GrpA:
	case SND_e_GrpB:
	/**/
	case SND_e_MasterGrp:
	/**/
	case SND_e_UserGrpMusic:
	case SND_e_UserGrpDialog:
	case SND_e_UserGrpSpecialFX:
	case SND_e_UserGrpMaster:
		SND_gdst_Group[Id].f_FxVolume = vol;
		break;

	default:
		ERR_X_Warning(0, "[SND] SNDvolume.c : bad group id", NULL);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_GroupFxVolumeGet(int Id)
{
	switch(Id)
	{
	case SND_e_GrpSpecialFX:
	case SND_e_GrpMusic:
	case SND_e_GrpAmbience:
	case SND_e_GrpDialog:
	case SND_e_GrpCutScene:
	case SND_e_GrpInterface:
	/**/
	case SND_e_GrpA:
	case SND_e_GrpB:
	/**/
	case SND_e_MasterGrp:
	/**/
	case SND_e_UserGrpMusic:
	case SND_e_UserGrpDialog:
	case SND_e_UserGrpSpecialFX:
	case SND_e_UserGrpMaster:
		return SND_gdst_Group[Id].f_FxVolume;

	default:
		ERR_X_Warning(0, "[SND] SNDvolume.c : bad group id", NULL);
		return 1.0f;
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
