/*$T pcretailfx.c GC! 1.081 02/06/03 08:35:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifndef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDvolume.h"
#include "Sdk/Sources/BASe/ERRors/ERRasser.h"
#include "Sdk/Sources/BASe/MEMory/MEM.h"
#include "SNDconst.h"
#include "pcretailSNDfx.h"

///*Access to some Library objects*/
//extern SND_tdst_Parameters SND_gst_Params;

#define win32SND_dbg_TranslateReturn(hr)

typedef union	win32SND_tdun_DirectSoundFXParam_
{
	DSFXChorus		st_paramsChorus;
	DSFXCompressor	st_paramsCompressor;
	DSFXDistortion	st_paramsDistortion;
	DSFXEcho		st_paramsEcho;
	DSFXFlanger		st_paramsFlanger;
	DSFXGargle		st_paramsGargle;
	DSFXParamEq		st_paramsParamEq;
	DSFXWavesReverb st_paramsWavesReverb;
	struct I3DL2_
	{
		DSFXI3DL2Reverb st_paramsI3DL2Reverb;
		int				i_Quality;
	};
} win32SND_tdun_DirectSoundFXParam;

typedef enum	win32SND_tden_I3DL2_Preset_
{
	e_I3DL3_DEFAULT			= 0,
	e_I3DL3_GENERIC,
	e_I3DL3_PADDEDCELL,
	e_I3DL3_ROOM,
	e_I3DL3_BATHROOM,
	e_I3DL3_LIVINGROOM,
	e_I3DL3_STONEROOM,
	e_I3DL3_AUDITORIUM,
	e_I3DL3_CONCERTHALL,
	e_I3DL3_CAVE,
	e_I3DL3_ARENA,
	e_I3DL3_HANGAR,
	e_I3DL3_CARPETEDHALLWAY,
	e_I3DL3_HALLWAY,
	e_I3DL3_STONECORRIDOR,
	e_I3DL3_ALLEY,
	e_I3DL3_FOREST,
	e_I3DL3_CITY,
	e_I3DL3_MOUNTAINS,
	e_I3DL3_QUARRY,
	e_I3DL3_PLAIN,
	e_I3DL3_PARKINGLOT,
	e_I3DL3_SEWERPIPE,
	e_I3DL3_UNDERWATER,
	e_I3DL3_SMALLROOM,
	e_I3DL3_MEDIUMROOM,
	e_I3DL3_LARGEROOM,
	e_I3DL3_MEDIUMHALL,
	e_I3DL3_LARGEHALL,
	e_I3DL3_PLATE
}
win32SND_tden_I3DL2_Preset;

/*Manager for FX settings*/

typedef struct pcretailSND_tdst_FxManager_
{
	BOOL				b_FxIsEnabled;
	BOOL				b_FxIsActive;
	SND_tdst_FxParam	st_CurrentFxParam;
} pcretailSND_tdst_FxManager;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

win32SND_tdun_DirectSoundFXParam	win32SND_un_Settings;
float								win32SND_f_WetVolume;
extern BAS_tdst_barray				win32SND_gst_PlayingSB;
pcretailSND_tdst_FxManager			pcretailSND_gst_FxManager;
extern SND_tdst_Parameters			SND_gst_Params;

extern SND_tdst_FxParam SND_gst_FxDefaultParam;

DSFXI3DL2Reverb						win32SND_sast_I3DL2Preset[] =
{
	{ I3DL2_ENVIRONMENT_PRESET_DEFAULT },
	{ I3DL2_ENVIRONMENT_PRESET_GENERIC },
	{ I3DL2_ENVIRONMENT_PRESET_PADDEDCELL },
	{ I3DL2_ENVIRONMENT_PRESET_ROOM },
	{ I3DL2_ENVIRONMENT_PRESET_BATHROOM },
	{ I3DL2_ENVIRONMENT_PRESET_LIVINGROOM },
	{ I3DL2_ENVIRONMENT_PRESET_STONEROOM },
	{ I3DL2_ENVIRONMENT_PRESET_AUDITORIUM },
	{ I3DL2_ENVIRONMENT_PRESET_CONCERTHALL },
	{ I3DL2_ENVIRONMENT_PRESET_CAVE },
	{ I3DL2_ENVIRONMENT_PRESET_ARENA },
	{ I3DL2_ENVIRONMENT_PRESET_HANGAR },
	{ I3DL2_ENVIRONMENT_PRESET_CARPETEDHALLWAY },
	{ I3DL2_ENVIRONMENT_PRESET_HALLWAY },
	{ I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR },
	{ I3DL2_ENVIRONMENT_PRESET_ALLEY },
	{ I3DL2_ENVIRONMENT_PRESET_FOREST },
	{ I3DL2_ENVIRONMENT_PRESET_CITY },
	{ I3DL2_ENVIRONMENT_PRESET_MOUNTAINS },
	{ I3DL2_ENVIRONMENT_PRESET_QUARRY },
	{ I3DL2_ENVIRONMENT_PRESET_PLAIN },
	{ I3DL2_ENVIRONMENT_PRESET_PARKINGLOT },
	{ I3DL2_ENVIRONMENT_PRESET_SEWERPIPE },
	{ I3DL2_ENVIRONMENT_PRESET_UNDERWATER },
	{ I3DL2_ENVIRONMENT_PRESET_SMALLROOM },
	{ I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM },
	{ I3DL2_ENVIRONMENT_PRESET_LARGEROOM },
	{ I3DL2_ENVIRONMENT_PRESET_MEDIUMHALL },
	{ I3DL2_ENVIRONMENT_PRESET_LARGEHALL },
	{ I3DL2_ENVIRONMENT_PRESET_PLATE }
};

/*
 ***********************************************************************************************************************
    PC RETAIL PRIVATE FUNCTIONS
 ***********************************************************************************************************************
*/

void	pcretailSND_FxUpdateAllSB(SND_tdst_FxParam *p);
void	pcretailSND_FxSetParam(SND_tdst_FxParam *p, SND_tdst_DirectSoundFX *pst_DSFX);
void	pcretailSND_FxApplyToSB(SND_tdst_FxParam *p, SND_tdst_SoundBuffer *pst_SB, int looped);
void	pcretailSND_FxUnapplyToSB(SND_tdst_FxParam *p, SND_tdst_SoundBuffer *pst_SB);
void*	GetFXInterface(REFGUID refguidObj, REFGUID refguidinterf);
int		IsThereFXEntry(REFGUID refguidObj, REFGUID refguidinterf);
void	SetFXInterface(REFGUID refguidObj, REFGUID refguidinterf, void **lplpObj);

/*$4
 ***********************************************************************************************************************
    PCRETAIL_USE_DSFX
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxInit(void)
{
	SND_gst_FxParam.i_SBNbMax = SND_Cte_MinAlloc;
	//XTE: Support for DefaultParams too
	SND_gst_FxDefaultParam.i_SBNbMax = SND_Cte_MinAlloc;
	SND_gst_FxDefaultParam.apst_SBList = (SND_tdst_SoundBuffer **) MEM_p_Alloc(SND_gst_FxDefaultParam.i_SBNbMax * sizeof(SND_tdst_SoundBuffer *));
	//XTE: Support for DefaultParams too
	SND_gst_FxParam.apst_SBList = (SND_tdst_SoundBuffer **) MEM_p_Alloc(SND_gst_FxParam.i_SBNbMax * sizeof(SND_tdst_SoundBuffer *));
	L_memset(SND_gst_FxParam.apst_SBList, 0, SND_gst_FxParam.i_SBNbMax * sizeof(SND_tdst_SoundBuffer *));

	L_memset(&win32SND_un_Settings, 0, sizeof(win32SND_tdun_DirectSoundFXParam));
	win32SND_f_WetVolume = 0.0f;
	
	/*Reset manager*/
	L_memset(&pcretailSND_gst_FxManager,0,sizeof(pcretailSND_tdst_FxManager));
	pcretailSND_gst_FxManager.b_FxIsEnabled = TRUE;


	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void pcretail_FxClose(void)
{
	if(SND_gst_FxParam.apst_SBList) 
		MEM_Free(SND_gst_FxParam.apst_SBList);
	SND_gst_FxParam.apst_SBList = NULL;
	if(SND_gst_FxDefaultParam.apst_SBList)
		MEM_Free(SND_gst_FxDefaultParam.apst_SBList);
	SND_gst_FxDefaultParam.apst_SBList = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxSetMode(SND_tdst_FxParam *p_param, int mode)
{
	switch(mode)
	{
	case SND_Cte_FxMode_Off:
		p_param->i_Mode = mode;
		break;

	/*$2- actualy do one FX but not the expected one -----------------------------------------------------------------*/

	case SND_Cte_FxMode_Max:
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_DEFAULT]);
		win32SND_un_Settings.i_Quality = 2;
		break;

	/*$2- temptative emulation of PSX2 FX ----------------------------------------------------------------------------*/

	case SND_Cte_FxMode_Delay:
		p_param->i_Mode = mode;
		win32SND_un_Settings.st_paramsChorus.fWetDryMix = 50.0f;
		win32SND_un_Settings.st_paramsChorus.fDepth = 0.0f;
		win32SND_un_Settings.st_paramsChorus.fFeedback = 0.0f;
		win32SND_un_Settings.st_paramsChorus.fFrequency = 0.0f;
		win32SND_un_Settings.st_paramsChorus.lWaveform = DSFXCHORUS_WAVE_SIN;
		win32SND_un_Settings.st_paramsChorus.fDelay = 0.0f;
		win32SND_un_Settings.st_paramsChorus.lPhase = DSFXCHORUS_PHASE_ZERO;
		break;
	case SND_Cte_FxMode_Pipe:
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_SEWERPIPE]);
		win32SND_un_Settings.i_Quality = 2;
		break;
	case SND_Cte_FxMode_Room:
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_ROOM]);
		win32SND_un_Settings.i_Quality = 2;
		break;
	case SND_Cte_FxMode_StudioA:
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_SMALLROOM]);
		win32SND_un_Settings.i_Quality = 2;
		break;
	case SND_Cte_FxMode_StudioB:
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_MEDIUMROOM]);
		win32SND_un_Settings.i_Quality = 2;
		break;
	case SND_Cte_FxMode_StudioC:
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_LARGEROOM]);
		win32SND_un_Settings.i_Quality = 2;
		break;
	case SND_Cte_FxMode_Hall:
		//p_param->i_Mode = SND_Cte_FxMode_Off;
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_MEDIUMHALL]);
		//HACK//
		//win32SND_un_Settings.st_paramsI3DL2Reverb.flDecayTime = DSFX_I3DL2REVERB_DECAYTIME_MAX;
		//HACK//
		win32SND_un_Settings.i_Quality = 2;
		break;
	case SND_Cte_FxMode_Space:
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_LARGEHALL]);
		win32SND_un_Settings.i_Quality = 2;
		break;
	case SND_Cte_FxMode_Mountains:
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_MOUNTAINS]);
		win32SND_un_Settings.i_Quality = 2;
		break;
	case SND_Cte_FxMode_City:
		p_param->i_Mode = SND_Cte_FxMode_win32I3DL2Reverb;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_CITY]);
		win32SND_un_Settings.i_Quality = 2;
		break;
	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_win32Echo:
		p_param->i_Mode = SND_Cte_FxMode_win32Echo;
		win32SND_un_Settings.st_paramsEcho.fWetDryMix = 50.0f;
		win32SND_un_Settings.st_paramsEcho.fFeedback = 0.0f;
		win32SND_un_Settings.st_paramsEcho.fLeftDelay = 333.0f;
		win32SND_un_Settings.st_paramsEcho.fRightDelay = 333.0f;
		win32SND_un_Settings.st_paramsEcho.lPanDelay = 0;
		break;

	/*$2- only win32 mode --------------------------------------------------------------------------------------------*/
	case SND_Cte_FxMode_win32Chorus:
		p_param->i_Mode = mode;
		win32SND_un_Settings.st_paramsChorus.fWetDryMix = 50.0f;
		win32SND_un_Settings.st_paramsChorus.fDepth = 25.0f;
		win32SND_un_Settings.st_paramsChorus.fFeedback = 0.0f;
		win32SND_un_Settings.st_paramsChorus.fFrequency = 0.0f;
		win32SND_un_Settings.st_paramsChorus.lWaveform = DSFXCHORUS_WAVE_SIN;
		win32SND_un_Settings.st_paramsChorus.fDelay = 0.0f;
		win32SND_un_Settings.st_paramsChorus.lPhase = DSFXCHORUS_PHASE_ZERO;
		break;
	case SND_Cte_FxMode_win32Compressor:
		p_param->i_Mode = mode;
		win32SND_un_Settings.st_paramsCompressor.fGain = 0.0f;
		win32SND_un_Settings.st_paramsCompressor.fAttack = 0.01f;
		win32SND_un_Settings.st_paramsCompressor.fRelease = 50.0f;
		win32SND_un_Settings.st_paramsCompressor.fThreshold = -10.0f;
		win32SND_un_Settings.st_paramsCompressor.fRatio = 10.0f;
		win32SND_un_Settings.st_paramsCompressor.fPredelay = 0.0f;
		break;
	case SND_Cte_FxMode_win32Distortion:
		p_param->i_Mode = mode;
		win32SND_un_Settings.st_paramsDistortion.fGain = 0.0f;
		win32SND_un_Settings.st_paramsDistortion.fEdge = 50.0f;
		win32SND_un_Settings.st_paramsDistortion.fPostEQCenterFrequency = 4000.0f;
		win32SND_un_Settings.st_paramsDistortion.fPostEQBandwidth = 4000.0f;
		win32SND_un_Settings.st_paramsDistortion.fPreLowpassCutoff = 4000.0f;
		break;
	case SND_Cte_FxMode_win32Flanger:
		p_param->i_Mode = mode;
		win32SND_un_Settings.st_paramsFlanger.fWetDryMix = 50.0f;
		win32SND_un_Settings.st_paramsFlanger.fDepth = 25.0f;
		win32SND_un_Settings.st_paramsFlanger.fFeedback = 0.0f;
		win32SND_un_Settings.st_paramsFlanger.fFrequency = 0.0f;
		win32SND_un_Settings.st_paramsFlanger.lWaveform = DSFXFLANGER_WAVE_SIN;
		win32SND_un_Settings.st_paramsFlanger.fDelay = 0.0f;
		win32SND_un_Settings.st_paramsFlanger.lPhase = DSFXFLANGER_PHASE_ZERO;
		break;
	case SND_Cte_FxMode_win32Gargle:
		win32SND_un_Settings.st_paramsGargle.dwRateHz = 1;
		win32SND_un_Settings.st_paramsGargle.dwWaveShape = DSFXGARGLE_WAVE_TRIANGLE;
		p_param->i_Mode = mode;
		break;
	case SND_Cte_FxMode_win32ParamEq:
		win32SND_un_Settings.st_paramsParamEq.fCenter = 1000.0f;
		win32SND_un_Settings.st_paramsParamEq.fBandwidth = 10.0f;
		win32SND_un_Settings.st_paramsParamEq.fGain = -15.0f;
		p_param->i_Mode = mode;
		break;
	case SND_Cte_FxMode_win32WavesReverb:
		win32SND_un_Settings.st_paramsWavesReverb.fInGain = 0.0f;
		win32SND_un_Settings.st_paramsWavesReverb.fReverbMix = 0.0f;
		win32SND_un_Settings.st_paramsWavesReverb.fReverbTime = 1000.0f;
		win32SND_un_Settings.st_paramsWavesReverb.fHighFreqRTRatio = 0.001f;
		p_param->i_Mode = mode;
		break;
	case SND_Cte_FxMode_win32I3DL2Reverb:
		p_param->i_Mode = mode;
		M_I3DL2Assign(win32SND_un_Settings.st_paramsI3DL2Reverb, win32SND_sast_I3DL2Preset[e_I3DL3_DEFAULT]);
		break;
	default:
		p_param->i_Mode = SND_Cte_FxMode_Off;
		ERR_X_ErrorAssert(0, "Fx Mode unknown", NULL);
		break;
	}

	//For now, set directly the mode also into the manager
	pcretailSND_gst_FxManager.st_CurrentFxParam.i_Mode = p_param->i_Mode;


	pcretailSND_FxUpdateAllSB(p_param);

	return 0;
}
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxGetMode(SND_tdst_FxParam *p)
{
	return p->i_Mode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxSetFeedback(struct SND_tdst_FxParam_ *p_param, int d)
{
	//OLD
	//p->i_Feedback = d;
	//OLD
	if(d < -99)
		d = -99;
	else if(d > 99)
		d = 99;
	switch(p_param->i_Mode)
	{
	case SND_Cte_FxMode_Off:
		return 0;
	/* no feedback for those modes */
	case SND_Cte_FxMode_Max:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_win32Compressor:
	case SND_Cte_FxMode_win32Distortion:
	case SND_Cte_FxMode_win32Gargle:
	case SND_Cte_FxMode_win32ParamEq:
	case SND_Cte_FxMode_win32WavesReverb:
	case SND_Cte_FxMode_win32I3DL2Reverb:
		return 0;
	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_win32Chorus:
		win32SND_un_Settings.st_paramsChorus.fFeedback = fLongToFloat(d);
		break;
	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_win32Echo:
		if(d < 0) d = -d;
		win32SND_un_Settings.st_paramsEcho.fFeedback = fLongToFloat(d);
		break;
	case SND_Cte_FxMode_win32Flanger:
		win32SND_un_Settings.st_paramsFlanger.fFeedback = fLongToFloat(d);
		break;
	default:
		p_param->i_Mode = SND_Cte_FxMode_Off;
		ERR_X_ErrorAssert(0, "Fx Mode unknown", NULL);
		break;
	}
	pcretailSND_FxUpdateAllSB(p_param);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxGetFeedback(struct SND_tdst_FxParam_ *p_param)
{
	/*~~*/
	int i;
	/*~~*/

	switch(p_param->i_Mode)
	{
	case SND_Cte_FxMode_Off:
		i = 0;
		break;

	/* no feedback for those modes */
	case SND_Cte_FxMode_Max:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_win32Compressor:
	case SND_Cte_FxMode_win32Distortion:
	case SND_Cte_FxMode_win32Gargle:
	case SND_Cte_FxMode_win32ParamEq:
	case SND_Cte_FxMode_win32WavesReverb:
	case SND_Cte_FxMode_win32I3DL2Reverb:
		i = 0;
		break;

	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_win32Chorus:
		i = lFloatToLong(win32SND_un_Settings.st_paramsChorus.fFeedback);
		break;

	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_win32Echo:
		i = lFloatToLong(win32SND_un_Settings.st_paramsEcho.fFeedback);
		break;

	case SND_Cte_FxMode_win32Flanger:
		i = lFloatToLong(win32SND_un_Settings.st_paramsFlanger.fFeedback);
		break;

	default:
		i = 0;
		p_param->i_Mode = SND_Cte_FxMode_Off;
		ERR_X_ErrorAssert(0, "Fx Mode unknown", NULL);
		break;
	}

	return i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxSetDelay(struct SND_tdst_FxParam_ *p_param, int d)
{

	switch(p_param->i_Mode)
	{
	case SND_Cte_FxMode_Off:
		return 0;

	/* no delay */
	case SND_Cte_FxMode_Max:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_win32Compressor:
	case SND_Cte_FxMode_win32Distortion:
	case SND_Cte_FxMode_win32Flanger:
	case SND_Cte_FxMode_win32Gargle:
	case SND_Cte_FxMode_win32ParamEq:
	case SND_Cte_FxMode_win32WavesReverb:
	case SND_Cte_FxMode_win32I3DL2Reverb:
		return 0;

	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_win32Chorus:
		if(d < 0) d = 0;
		if(d > 20) d = 20;
		win32SND_un_Settings.st_paramsChorus.fDelay = fLongToFloat(d);
		break;

	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_win32Echo:
		if(d < 0) d = 0;
		if(d > 2000) d = 2000;
		win32SND_un_Settings.st_paramsEcho.fLeftDelay = fLongToFloat(d);
		win32SND_un_Settings.st_paramsEcho.fRightDelay = fLongToFloat(d);
		break;

	default:
		p_param->i_Mode = SND_Cte_FxMode_Off;
		ERR_X_ErrorAssert(0, "Fx Mode unknown", NULL);
		return -1;
	}

	pcretailSND_FxUpdateAllSB(p_param);
	return 0;

//OLD
//	p->i_Delay = d;
//	return 0;
//
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxGetDelay(struct SND_tdst_FxParam_ *p_param)
{

	/*~~*/
	int i;
	/*~~*/

	switch(p_param->i_Mode)
	{
	case SND_Cte_FxMode_Off:
		i = 0;
		break;

	/* no delay */
	case SND_Cte_FxMode_Max:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_win32Compressor:
	case SND_Cte_FxMode_win32Distortion:
	case SND_Cte_FxMode_win32Flanger:
	case SND_Cte_FxMode_win32Gargle:
	case SND_Cte_FxMode_win32ParamEq:
	case SND_Cte_FxMode_win32WavesReverb:
	case SND_Cte_FxMode_win32I3DL2Reverb:
		i = 0;
		break;

	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_win32Chorus:
		i = lFloatToLong(win32SND_un_Settings.st_paramsChorus.fDelay);
		break;

	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_win32Echo:
		i = lFloatToLong(win32SND_un_Settings.st_paramsEcho.fLeftDelay);
		break;

	default:
		p_param->i_Mode = SND_Cte_FxMode_Off;
		ERR_X_ErrorAssert(0, "Fx Mode unknown", NULL);
		i = -1;
		break;
	}

	return i;



	//OLD
	//return p->i_Delay;
	//OLD
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxSetWetVolume(SND_tdst_FxParam *p, int _i_Level)
{
	//OLD
	//p->f_WetVol = SND_f_GetVolFromAtt(d);
	//OLD
	/*~~~~~~~~~~~~~~~~~~~*/
	float	f_Dry, f_Ratio;
	/*~~~~~~~~~~~~~~~~~~~*/

	f_Dry = SND_f_GetGlobalVol();
	win32SND_f_WetVolume = SND_f_GetVolFromAtt(_i_Level);

	if(f_Dry == 0.0f)
		f_Ratio = 0.0f;
	else
		f_Ratio = win32SND_f_WetVolume / f_Dry;

	switch(p->i_Mode)
	{
	case SND_Cte_FxMode_win32Chorus:
		win32SND_un_Settings.st_paramsChorus.fWetDryMix = f_Ratio;
		break;

	case SND_Cte_FxMode_win32Compressor:
	case SND_Cte_FxMode_win32Distortion:
	case SND_Cte_FxMode_win32Gargle:
	case SND_Cte_FxMode_win32ParamEq:
	case SND_Cte_FxMode_Off:
		return 0;

	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_win32Echo:
		win32SND_un_Settings.st_paramsEcho.fWetDryMix = f_Ratio;
		break;

	case SND_Cte_FxMode_win32Flanger:
		win32SND_un_Settings.st_paramsFlanger.fWetDryMix = f_Ratio;
		break;

	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Max:
	case SND_Cte_FxMode_win32I3DL2Reverb:
		win32SND_un_Settings.st_paramsI3DL2Reverb.lRoom = (_i_Level / SND_l_GetAttFromVol(0.0f)) * -10000;
		break;

	case SND_Cte_FxMode_win32WavesReverb:
		win32SND_un_Settings.st_paramsWavesReverb.fReverbMix = (float) _i_Level / 100.0f;
		break;

	default:
		ERR_X_ErrorAssert(0, "Mauvais boulot, on ne doit pas venir ici", NULL);
		break;
	}

	pcretailSND_FxUpdateAllSB(p);
	return 0;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void pcretailSND_FxUpdateAllSB(SND_tdst_FxParam *p)
{
	/*
	VER 1.0
	if(SND_gst_FxParam.ui_Flag & SND_Flg_FxEnable)
	{
		int i;
		for(i = 0; i < SND_gst_FxParam.i_SBNbMax; i++)
		{
			if(SND_gst_FxParam.apst_SBList[i] == NULL) continue;
			pcretailSND_FxSetParam(SND_gst_FxParam.apst_SBList[i]->pst_DSFX);
		}
	}
	*/
	if(p->ui_Flag & SND_Flg_FxEnable)
	{
		int i;
		for(i = 0; i < p->i_SBNbMax; i++)
		{
			if(p->apst_SBList[i] == NULL) continue;
			pcretailSND_FxSetParam(p,p->apst_SBList[i]->pst_DSFX);
		}
	}

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxGetWetPan(SND_tdst_FxParam *p)
{
	return p->i_WetPan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxGetWetVolume(SND_tdst_FxParam *p)
{
	//OLD
	//return SND_l_GetAttFromVol(p->f_WetVol);
	//OLD
	/*~~~~~~~~~~~~~~~~~*/
	float	f_Wet, f_Dry;
	/*~~~~~~~~~~~~~~~~~*/

	f_Dry = SND_f_GetGlobalVol();
	switch(p->i_Mode)
	{
	case SND_Cte_FxMode_win32Chorus:
		f_Wet = win32SND_un_Settings.st_paramsChorus.fWetDryMix * f_Dry;
		break;

	case SND_Cte_FxMode_win32Compressor:
		f_Wet = f_Dry;
		break;

	case SND_Cte_FxMode_win32Distortion:
		f_Wet = f_Dry;
		break;

	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_win32Echo:
		f_Wet = win32SND_un_Settings.st_paramsEcho.fWetDryMix * f_Dry;
		break;

	case SND_Cte_FxMode_win32Flanger:
		f_Wet = win32SND_un_Settings.st_paramsFlanger.fWetDryMix * f_Dry;
		break;

	case SND_Cte_FxMode_win32Gargle:
		f_Wet = f_Dry;
		break;

	case SND_Cte_FxMode_win32ParamEq:
		f_Wet = f_Dry;
		break;

	case SND_Cte_FxMode_Off:
		f_Wet = 0.0f;
		break;

	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Max:
	case SND_Cte_FxMode_win32I3DL2Reverb:
		f_Wet = (float) (win32SND_un_Settings.st_paramsI3DL2Reverb.lRoom * SND_l_GetAttFromVol(0)) / -10000.0f;
		break;

	case SND_Cte_FxMode_win32WavesReverb:
		f_Wet = win32SND_un_Settings.st_paramsWavesReverb.fReverbMix * 100;
		break;

	default:
		f_Wet = f_Dry;
		ERR_X_ErrorAssert(0, "Mauvais boulot, on ne doit pas venir ici", NULL);
		break;
	}

	win32SND_f_WetVolume = f_Wet;
	return SND_l_GetAttFromVol(f_Wet);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxSetWetPan(SND_tdst_FxParam *p, int d)
{
	p->i_WetPan = d;
	return 0;
}

void pcretailSND_FxApplyToSB(SND_tdst_FxParam *p, SND_tdst_SoundBuffer *pst_SB,int looped)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						play;
	SND_tdst_DirectSoundFX	*pst_DSFX;
	DSEFFECTDESC			dsFXdesc;
	HRESULT					hr, ret;
	const GUID				*refguidObject;
	const GUID				*refguidInterface;
	LPVOID					*lplpObject;
	//XTE: This integer holds the number of samples necessary to hold the Fx tailZs//This is initially set with the number of seconds necessary for the specific Fx
	int						iAdditionalSamples = 0;
	static unsigned totalAllocated = 0;
	//XTE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/*$2- stop if playing --------------------------------------------------------------------------------------------*/

	pst_SB->pst_DSB->lpVtbl->GetStatus(pst_SB->pst_DSB, &play);
	if(play & DSBSTATUS_PLAYING) 
		pst_SB->pst_DSB->lpVtbl->Stop(pst_SB->pst_DSB);

	/*$2- release last FX state of the sound buffer ------------------------------------------------------------------*/

	if(pst_SB->pst_DSFX)
	{
		pst_SB->pst_DSB->lpVtbl->SetFX(pst_SB->pst_DSB, 0, NULL, NULL);
		MEM_Free(pst_SB->pst_DSFX);
		pst_SB->pst_DSFX = NULL;
	}

	/*$2- alloc a new one --------------------------------------------------------------------------------------------*/

	pst_DSFX = (SND_tdst_DirectSoundFX *) MEM_p_Alloc(sizeof(SND_tdst_DirectSoundFX));
	totalAllocated += sizeof(SND_tdst_DirectSoundFX);

	/*$2- set all new fx parameters ----------------------------------------------------------------------------------*/

	L_memset(&dsFXdesc, 0, sizeof(DSEFFECTDESC));
	dsFXdesc.dwSize = sizeof(DSEFFECTDESC);

	switch(p->i_Mode)
	{
	case SND_Cte_FxMode_win32Chorus:
		refguidObject = &GUID_DSFX_STANDARD_CHORUS;
		refguidInterface = &IID_IDirectSoundFXChorus8;
		lplpObject = &pst_DSFX->pst_DSFXChorus;
		break;

	case SND_Cte_FxMode_win32Compressor:
		refguidObject = &GUID_DSFX_STANDARD_COMPRESSOR;
		refguidInterface = &IID_IDirectSoundFXCompressor8;
		lplpObject = &pst_DSFX->pst_DSFXCompressor;
		break;

	case SND_Cte_FxMode_win32Distortion:
		refguidObject = &GUID_DSFX_STANDARD_DISTORTION;
		refguidInterface = &IID_IDirectSoundFXDistortion8;
		lplpObject = &pst_DSFX->pst_DSFXDistortion;
		break;

	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_win32Echo:
		refguidObject = &GUID_DSFX_STANDARD_ECHO;
		refguidInterface = &IID_IDirectSoundFXEcho8;
		lplpObject = &pst_DSFX->pst_DSFXEcho;
		iAdditionalSamples = 2;
		break;

	case SND_Cte_FxMode_win32Flanger:
		refguidObject = &GUID_DSFX_STANDARD_FLANGER;
		refguidInterface = &IID_IDirectSoundFXFlanger8;
		lplpObject = &pst_DSFX->pst_DSFXFlanger;
		break;

	case SND_Cte_FxMode_win32Gargle:
		refguidObject = &GUID_DSFX_STANDARD_GARGLE;
		refguidInterface = &IID_IDirectSoundFXGargle8;
		lplpObject = &pst_DSFX->pst_DSFXGargle;
		break;

	case SND_Cte_FxMode_win32ParamEq:
		refguidObject = &GUID_DSFX_STANDARD_PARAMEQ;
		refguidInterface = &IID_IDirectSoundFXParamEq8;
		lplpObject = &pst_DSFX->pst_DSFXParamEq;
		break;

	case SND_Cte_FxMode_Off:
		MEM_Free(pst_DSFX);
		return;
		break;

	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Max:
	case SND_Cte_FxMode_win32I3DL2Reverb:
		refguidObject = &GUID_DSFX_STANDARD_I3DL2REVERB;
		refguidInterface = &IID_IDirectSoundFXI3DL2Reverb8;
		lplpObject = &pst_DSFX->pst_DSFXI3DL2Reverb;
		//XTE
		iAdditionalSamples = 0;
		//XTE
		break;

	case SND_Cte_FxMode_win32WavesReverb:
		refguidObject = &GUID_DSFX_WAVES_REVERB;
		refguidInterface = &IID_IDirectSoundFXWavesReverb8;
		lplpObject = &pst_DSFX->pst_DSFXWavesReverb;
		break;

	default:
		ERR_X_ErrorAssert(0, "Mauvais boulot, on ne doit pas venir ici", NULL);
		MEM_Free(pst_DSFX);
		break;
	}
	

	//#######Before setting the Fx, create a new buffer bigger than the previous######
	//First of all, stop the SoundBuffer if necessary
	pst_SB->pst_DSB->lpVtbl->GetStatus(pst_SB->pst_DSB,&play);
	if( play & DSBSTATUS_PLAYING )
	{
		pst_SB->pst_DSB->lpVtbl->Stop(pst_SB->pst_DSB);
	}

	//Re-create the buffer only if it is not streamed, if not already Fxed and not looped
	if( (pst_SB->pst_SS == NULL) && (!(pst_SB->hasFx)) && !looped )
	{

		DSBCAPS dsbcaps;
		WAVEFORMATEX wavefmt;
		DWORD size1,size2,writtenbytes;
		void *ptr1,*ptr2;
		char *pTempBuffer;
		int minBuffer,iOriginalSize;
		LPDIRECTSOUNDBUFFER8 pNewBuffer;
		DSBUFFERDESC dsbd;
		//Before everything, check if the buffer can have Fx
		L_memset(&dsbcaps,0,sizeof(DSBCAPS));
		dsbcaps.dwSize = sizeof(DSBCAPS);
		L_memset(&wavefmt,0,sizeof(WAVEFORMATEX));
		//Acquire info about SoundBuffer
		hr = pst_SB->pst_DSB->lpVtbl->GetCaps(pst_SB->pst_DSB,&dsbcaps);
			hr = pst_SB->pst_DSB->lpVtbl->GetFormat(pst_SB->pst_DSB,&wavefmt,sizeof(WAVEFORMATEX),&writtenbytes);
			minBuffer = GetMinimumBufferSize(wavefmt.nAvgBytesPerSec);
			if( !SetFxFlag(minBuffer) )
				//The buffer is too small, Fx cannot be applied
				return;
			//Stop the Sound
			//hr = pst_SB->pst_DSB->lpVtbl->Stop(pst_SB->pst_DSB);
			//Lock the buffer
			hr = pst_SB->pst_DSB->lpVtbl->Lock(pst_SB->pst_DSB,0,dsbcaps.dwBufferBytes,&ptr1,&size1,&ptr2,&size2,DSBLOCK_ENTIREBUFFER);
			if( hr != DS_OK )
				return;
			//Read from the buffer (!!??Dangerous??!!) the original wave
			pTempBuffer = MEM_p_Alloc(size1);
			memcpy(pTempBuffer,ptr1,size1);
			iOriginalSize = size1;
			hr = pst_SB->pst_DSB->lpVtbl->Unlock(pst_SB->pst_DSB,ptr1,size1,ptr2,size2);

			//XTE HACK
			if( hr != DS_OK )
				return;
			//XTE HACK

			//Create a new SoundBuffer
			iAdditionalSamples *= wavefmt.nAvgBytesPerSec;
			memset(&dsbd,0,sizeof(DSBUFFERDESC));
			dsbd.dwSize = sizeof(DSBUFFERDESC);
			dsbd.dwFlags = (DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLFX);

			dsbd.dwBufferBytes = size1 + iAdditionalSamples;
			dsbd.lpwfxFormat = &wavefmt;
			dsbd.guid3DAlgorithm = GUID_NULL;
			hr = SND_gst_Params.pst_SpecificD->pst_DS->lpVtbl->CreateSoundBuffer(SND_gst_Params.pst_SpecificD->pst_DS,&dsbd,(IDirectSoundBuffer **) &pNewBuffer,NULL);
			if( hr != DS_OK )
				return;
			hr = pNewBuffer->lpVtbl->Lock(pNewBuffer,0,dsbcaps.dwBufferBytes,&ptr1,&size1,&ptr2,&size2,DSBLOCK_ENTIREBUFFER);
			if( hr != DS_OK )
				return;
			//Copy original buffer
			L_memcpy(ptr1,pTempBuffer,iOriginalSize);
			//Copy some silence
			L_memset(((char *) ptr1)+iOriginalSize,(wavefmt.wBitsPerSample==16) ? 0 : 128,iAdditionalSamples);
			hr = pNewBuffer->lpVtbl->Unlock(pNewBuffer,ptr1,size1,ptr2,size2);
			//Release the previous SoundBuffer
			hr = IDirectSound8_Release(pst_SB->pst_DSB);
			//XTE HACK
			if( hr != DS_OK )
				return;
			//XTE HACK
			//Save the new SoundBuffer pointer
			pst_SB->pst_DSB = pNewBuffer;
			//Release the wave buffer
			MEM_Free(pTempBuffer);
			//Tell the buffer has been enlarged
			pst_SB->hasFx = 1;
	}

	L_memcpy(&dsFXdesc.guidDSFXClass, refguidObject, sizeof(GUID));
	//@@@@@MEMORYBUG
	if( !pst_SB->hasFx )
	{
	//@@@@@MEMORYBUG
		hr = pst_SB->pst_DSB->lpVtbl->SetFX(pst_SB->pst_DSB, 1, &dsFXdesc, &ret);
		pst_SB->hasFx = 1;
	}
	//@@@@@MEMORYBUG
	win32SND_dbg_TranslateReturn(hr);
	//DEBUG TEST
	if(FAILED(hr))
	{
		MEM_Free(pst_DSFX);
		return;
	}

	/*$2- get FX interface -------------------------------------------------------------------------------------------*/
//	if( IsThereFXEntry(refguidObject,refguidInterface) )
//	{
//		//Use the preset one
//		*lplpObject = GetFXInterface(refguidObject,refguidInterface);
//	}
//	else
//	{
	hr = pst_SB->pst_DSB->lpVtbl->GetObjectInPath(pst_SB->pst_DSB, refguidObject, 0, refguidInterface, lplpObject);
//		SetFXInterface(refguidObject,refguidInterface,lplpObject);
//	}
	win32SND_dbg_TranslateReturn(hr);
	if( hr != DS_OK )
	{
		pst_SB->hasFx = 0;
		//If some problems while retrieving the Object, do nothing
		switch(hr)
		{
		case DSERR_OBJECTNOTFOUND:
			{
				int jj=0;
				break;
			}
		default:
			{
				int hh = 0;
			}
		}
		return;
	}
	else
	{
		int kk = 0;
	}
	/*$2- set FX parameters ------------------------------------------------------------------------------------------*/

	pst_DSFX->i_Id = p->i_Mode;
	pcretailSND_FxSetParam(p,pst_DSFX);
	pst_SB->pst_DSFX = pst_DSFX;

	/*$2- force the update of buffer (see DX8 note about DirectSoundFX) ----------------------------------------------*/

	//pst_SB->pst_DSB->lpVtbl->Stop(pst_SB->pst_DSB);


	/*$2- restart playing ? ------------------------------------------------------------------------------------------*/

	if(play & DSBSTATUS_PLAYING)
	{
		play = (play & DSBSTATUS_LOOPING) ? DSBPLAY_LOOPING : 0;
		pst_SB->pst_DSB->lpVtbl->Play(pst_SB->pst_DSB, 0, 0, play);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void pcretailSND_FxSetParam(SND_tdst_FxParam *p, SND_tdst_DirectSoundFX *pst_DSFX)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	switch(p->i_Mode)
	{
	case SND_Cte_FxMode_Off:
		return;

	case SND_Cte_FxMode_win32Chorus:
		hr = pst_DSFX->pst_DSFXChorus->lpVtbl->SetAllParameters
			(
				pst_DSFX->pst_DSFXChorus,
				&win32SND_un_Settings.st_paramsChorus
			);
		break;

	case SND_Cte_FxMode_win32Compressor:
		hr = pst_DSFX->pst_DSFXCompressor->lpVtbl->SetAllParameters
			(
				pst_DSFX->pst_DSFXCompressor,
				&win32SND_un_Settings.st_paramsCompressor
			);
		break;

	case SND_Cte_FxMode_win32Distortion:
		hr = pst_DSFX->pst_DSFXDistortion->lpVtbl->SetAllParameters
			(
				pst_DSFX->pst_DSFXDistortion,
				&win32SND_un_Settings.st_paramsDistortion
			);
		break;

	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_win32Echo:
		hr = pst_DSFX->pst_DSFXEcho->lpVtbl->SetAllParameters
			(
				pst_DSFX->pst_DSFXEcho,
				&win32SND_un_Settings.st_paramsEcho
			);
		break;

	case SND_Cte_FxMode_win32Flanger:
		hr = pst_DSFX->pst_DSFXFlanger->lpVtbl->SetAllParameters
			(
				pst_DSFX->pst_DSFXFlanger,
				&win32SND_un_Settings.st_paramsFlanger
			);
		break;

	case SND_Cte_FxMode_win32Gargle:
		hr = pst_DSFX->pst_DSFXGargle->lpVtbl->SetAllParameters
			(
				pst_DSFX->pst_DSFXGargle,
				&win32SND_un_Settings.st_paramsGargle
			);
		break;

	case SND_Cte_FxMode_win32ParamEq:
		hr = pst_DSFX->pst_DSFXParamEq->lpVtbl->SetAllParameters
			(
				pst_DSFX->pst_DSFXParamEq,
				&win32SND_un_Settings.st_paramsParamEq
			);
		break;

	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_Max:
	case SND_Cte_FxMode_win32I3DL2Reverb:
		hr = pst_DSFX->pst_DSFXI3DL2Reverb->lpVtbl->SetAllParameters
			(
				pst_DSFX->pst_DSFXI3DL2Reverb,
				&win32SND_un_Settings.st_paramsI3DL2Reverb
			);
		hr = pst_DSFX->pst_DSFXI3DL2Reverb->lpVtbl->SetQuality
			(
				pst_DSFX->pst_DSFXI3DL2Reverb,
				win32SND_un_Settings.i_Quality
			);
		break;

	case SND_Cte_FxMode_win32WavesReverb:
		hr = pst_DSFX->pst_DSFXWavesReverb->lpVtbl->SetAllParameters
			(
				pst_DSFX->pst_DSFXWavesReverb,
				&win32SND_un_Settings.st_paramsWavesReverb
			);
		break;
	default:
		ERR_X_ErrorAssert(0, "Mauvais boulot, on ne doit pas venir ici", NULL);
		break;
	}

	win32SND_dbg_TranslateReturn(hr);
}

void pcretailSND_FxUnapplyToSB(SND_tdst_FxParam *p, SND_tdst_SoundBuffer *pst_SB)
{
	/*~~~~~*/
	int play;
	/*~~~~~*/

	/*$2- check if FX is applied on this buffef ----------------------------------------------------------------------*/

	if(pst_SB->pst_DSFX == NULL) return;

	/*$2- stop playing ? ---------------------------------------------------------------------------------------------*/

	pst_SB->pst_DSB->lpVtbl->GetStatus(pst_SB->pst_DSB, &play);
	if(play & DSBSTATUS_PLAYING) 
		pst_SB->pst_DSB->lpVtbl->Stop(pst_SB->pst_DSB);

	/*$2- reset the FX -----------------------------------------------------------------------------------------------*/

	pst_SB->pst_DSB->lpVtbl->SetFX(pst_SB->pst_DSB, 0, NULL, NULL);
	MEM_Free(pst_SB->pst_DSFX);
	pst_SB->pst_DSFX = NULL;

	/*$2- force the update of buffer ---------------------------------------------------------------------------------*/

	pst_SB->pst_DSB->lpVtbl->Stop(pst_SB->pst_DSB);

	/*$2- restart playing ? ------------------------------------------------------------------------------------------*/

	if(play & DSBSTATUS_PLAYING)
	{
		play = (play & DSBSTATUS_LOOPING) ? DSBPLAY_LOOPING : 0;
		pst_SB->pst_DSB->lpVtbl->Play(pst_SB->pst_DSB, 0, 0, play);
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxEnable(SND_tdst_FxParam *p)
{
	/*~~~~~*/
	int i;
	//int play;
//	SND_tdst_SoundBuffer *pSB;
	/*~~~~~*/

	//play = 0;

	p->ui_Flag |= SND_Flg_FxEnable;

	for(i = 0; i < p->i_SBNbMax; i++)
	{
		if(p->apst_SBList[i] == NULL) continue;
		pcretailSND_FxApplyToSB(p, p->apst_SBList[i],0);
	}

/*
	for(i=0;i<win32SND_gst_PlayingSB.num;++i)
	{
		if(win32SND_gst_PlayingSB.base[i].ul_Val == -1) continue;
		
		pSB = (SND_tdst_SoundBuffer *) win32SND_gst_PlayingSB.base[i].ul_Val;
		//Check FX onto SoundBuffer
		if(pSB->pst_DSB)
		{
			DSBCAPS dsbcaps;
			pSB->pst_DSB->lpVtbl->GetCaps(pSB->pst_DSB,&dsbcaps);
			if( dsbcaps.dwFlags | DSBCAPS_CTRLFX )
			{
				pcretailSND_FxApplyToSB(p, pSB);
			}
		}
	}
	*/
	return 0;

	//OLD
	//return 0;
	//OLD
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int pcretail_i_FxDisable(SND_tdst_FxParam *p)
{
	/*~~~~~*/
	int i;
	int play;
	/*~~~~~*/

	play = 0;

	p->ui_Flag &= ~SND_Flg_FxEnable;

	for(i = 0; i < p->i_SBNbMax; i++)
	{
		if(p->apst_SBList[i] == NULL) continue;
		pcretailSND_FxUnapplyToSB(p, p->apst_SBList[i]);
	}

	return 0;
	//OLD
	//return 0;
	//OLD
}

int	pcretailSND_i_FxAddSB(SND_tdst_FxParam *p, SND_tdst_SoundBuffer *_pst_SBdry,int looped)
{
	/*~~*/
	int i;
	/*~~*/

	if(p->i_Mode == SND_Cte_FxMode_Off) return -1;

	for(i = 0; i < p->i_SBNbMax; i++)
	{
		if(p->apst_SBList[i] == NULL)
			break;
		if(p->apst_SBList[i] == _pst_SBdry)
			break;
	}

	if(i == p->i_SBNbMax)
	{
		p->i_SBNbMax += SND_Cte_MinAlloc;
		p->apst_SBList = (SND_tdst_SoundBuffer **) MEM_p_Realloc
			(
				p->apst_SBList,
				p->i_SBNbMax * sizeof(SND_tdst_SoundBuffer *)
			);
		L_memset(&p->apst_SBList[i], 0, SND_Cte_MinAlloc * sizeof(SND_tdst_SoundBuffer *));
	}

	p->apst_SBList[i] = _pst_SBdry;
	if(p->ui_Flag & SND_Flg_FxEnable)
		pcretailSND_FxApplyToSB(p, p->apst_SBList[i], looped);
	return 0;
}

int	pcretailSND_i_FxDelSB(struct SND_tdst_FxParam_ *p, SND_tdst_SoundBuffer *pst_DSB)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < p->i_SBNbMax; i++)
	{
		if(p->apst_SBList[i] == pst_DSB) break;
	}

	if(i != p->i_SBNbMax)
	{
		pcretailSND_FxUnapplyToSB(p, pst_DSB);
		p->apst_SBList[i] = NULL;
	}

	return 0;
}

DWORD SetFxFlag(bool isBufferEnoughtBig)
{
	//Apply Sound Fx only if the buffer is big enough, otherwise set nothing
	if(isBufferEnoughtBig)
		return DSBCAPS_CTRLFX;
	else
		return 0;
}

int	GetMinimumBufferSize(DWORD nAverageBytePerSecond)
{
	return (SND_Cte_MinBufferLenght*nAverageBytePerSecond) / 1000;
}

struct SND_tdst_FXInterfaces
{
	void	*pObject;
	REFGUID	guidobject;
	REFGUID	guidinterf;

} FXInterfaces;

struct SND_tdst_FXInterfaces SND_gst_FXInterfaces[] =
{
	{ &IID_IDirectSoundFXEcho8,			&GUID_DSFX_STANDARD_ECHO, NULL },
	{ &IID_IDirectSoundFXI3DL2Reverb8,	&GUID_DSFX_STANDARD_I3DL2REVERB, NULL }
};

void SetFXInterface(REFGUID refguidObj, REFGUID refguidinterf, void **lplpObj)
{
	if( IsEqualGUID(refguidObj,&GUID_DSFX_STANDARD_ECHO) && IsEqualGUID(refguidinterf,&IID_IDirectSoundFXEcho8) )
		SND_gst_FXInterfaces[1].pObject = *lplpObj;
	if( IsEqualGUID(refguidObj,&GUID_DSFX_STANDARD_I3DL2REVERB) && IsEqualGUID(refguidinterf,&IID_IDirectSoundFXI3DL2Reverb8) )
		SND_gst_FXInterfaces[2].pObject = *lplpObj; 
}

void* GetFXInterface(REFGUID refguidObj, REFGUID refguidinterf)
{
	if( IsEqualGUID(refguidObj,&GUID_DSFX_STANDARD_ECHO) && IsEqualGUID(refguidinterf,&IID_IDirectSoundFXEcho8) )
		return SND_gst_FXInterfaces[1].pObject;
	if( IsEqualGUID(refguidObj,&GUID_DSFX_STANDARD_I3DL2REVERB) && IsEqualGUID(refguidinterf,&IID_IDirectSoundFXI3DL2Reverb8) )
		return SND_gst_FXInterfaces[2].pObject;
	return NULL;
}

int IsThereFXEntry(REFGUID refguidObj, REFGUID refguidinterf)
{
	if( IsEqualGUID(refguidObj,&GUID_DSFX_STANDARD_ECHO) && IsEqualGUID(refguidinterf,&IID_IDirectSoundFXEcho8) )
	{
		if( SND_gst_FXInterfaces[1].pObject != NULL )
			return 1;
		else
			return 0;
	}
	if( IsEqualGUID(refguidObj,&GUID_DSFX_STANDARD_I3DL2REVERB) && IsEqualGUID(refguidinterf,&IID_IDirectSoundFXI3DL2Reverb8) )
	{
		if( SND_gst_FXInterfaces[2].pObject != NULL )
			return 1;
		else
			return 0;
	}

	return 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* defined(PCWIN_TOOL) || defined(ACTIVE_EDITORS) */
