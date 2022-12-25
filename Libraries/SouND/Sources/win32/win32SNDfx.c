/*$T win32SNDfx.c GC 1.138 11/05/04 09:05:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef PCWIN_TOOL

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/win32/win32SNDdebug.h"
#include "SouND/Sources/win32/win32SND.h"
#include "SouND/Sources/win32/win32SNDfx.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef struct	win32SND_tdst_FxDesc_
{
    BOOL                    b_HasChanged_Delay;
    BOOL                    b_HasChanged_Feedback;
    BOOL                    b_HasChanged_WetVol;
    BOOL                    b_HasChanged_Mode;
    BOOL                    b_IsEnabled;
	SND_tdst_FxParam		st_Settings;
} win32SND_tdst_FxDesc;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

win32SND_tdst_FxDesc	win32SND_dst_FxDesc[SND_Cte_FxCoreNb];
BOOL win32SND_gb_FxAlloc = FALSE;
BOOL win32SND_gb_NoFx = FALSE;
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32SND_i_FxInit(void)
{
	/*~~*/
	int i;
	/*~~*/

	L_memset(win32SND_dst_FxDesc, 0, SND_Cte_FxCoreNb * sizeof(win32SND_tdst_FxDesc));

	for(i = 0; i < SND_Cte_FxCoreNb; i++)
	{
		win32SND_dst_FxDesc[i].st_Settings.i_Mode = SND_Cte_FxMode_Off;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void win32SND_FxClose(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32SND_i_FxSetMode(int core, int mode)
{
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][win32SNDfx.c] bad core id", NULL);

	switch(mode)
	{
	case SND_Cte_FxMode_Off:
	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Echo:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_City:
	case SND_Cte_FxMode_Mountains:
        if(win32SND_dst_FxDesc[core].st_Settings.i_Mode != mode)
            win32SND_dst_FxDesc[core].b_HasChanged_Mode = TRUE;
		win32SND_dst_FxDesc[core].st_Settings.i_Mode = mode;
		break;

	default:
		win32SND_dst_FxDesc[core].st_Settings.i_Mode = SND_Cte_FxMode_Off;
        win32SND_dst_FxDesc[core].b_HasChanged_Mode = TRUE;
		ERR_X_Warning(0, "[SND][win32SNDfx.c] bad mode", NULL);
		break;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32SND_i_FxSetFeedback(int core, int feedback)
{
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][win32SNDfx.c] bad core id", NULL);
	ERR_X_Warning((0 <= feedback) || (feedback <= 100), "[SND][win32SNDfx.c] bad feedback value", NULL);

	if(feedback < 0) feedback = 0;
	if(feedback > 100) feedback = 100;

    if(win32SND_dst_FxDesc[core].st_Settings.i_Feedback != feedback)
        win32SND_dst_FxDesc[core].b_HasChanged_Feedback = TRUE;

	win32SND_dst_FxDesc[core].st_Settings.i_Feedback = feedback;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32SND_i_FxSetDelay(int core, int delay)
{
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][win32SNDfx.c] bad core id", NULL);
	ERR_X_Warning((0 <= delay) || (delay <= 1000), "[SND][win32SNDfx.c] bad delay value", NULL);

	if(delay < 0) delay = 0;
	if(delay > 1000) delay = 1000;

    if(win32SND_dst_FxDesc[core].st_Settings.i_Delay != delay)
        win32SND_dst_FxDesc[core].b_HasChanged_Delay = TRUE;

	win32SND_dst_FxDesc[core].st_Settings.i_Delay = delay;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32SND_i_FxSetWetVolume(int core, int volume)
{
    float f_New;
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][win32SNDfx.c] bad core id", NULL);

    f_New = SND_f_GetVolFromAtt(volume);
    
    if(win32SND_dst_FxDesc[core].st_Settings.f_WetVol != f_New)
        win32SND_dst_FxDesc[core].b_HasChanged_WetVol = TRUE;

	win32SND_dst_FxDesc[core].st_Settings.f_WetVol = f_New;
    //TODO apply new vol
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32SND_i_FxSetWetPan(int core, int pan)
{
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][win32SNDfx.c] bad core id", NULL);
	ERR_X_Warning((-10000 <= pan) || (pan <= 10000), "[SND][win32SNDfx.c] bad pan value", NULL);

	if(pan < -10000) pan = -10000;
	if(pan > 10000) pan = 10000;

	win32SND_dst_FxDesc[core].st_Settings.i_WetPan = pan;
	return 0;
}

DSEFFECTDESC    win32SND_gast_EffectDesc[SND_Cte_FxCoreNb];
DSFXWavesReverb reverb[SND_Cte_FxCoreNb];

void win32SND_FxAdd(SND_tdst_SoundBuffer *_pst_SB, int _i_Core, int _i_FxVol)
{
#ifdef USE_FX
    DWORD dwResult;
    HRESULT hr;
    
    if(win32SND_gb_NoFx) return;

    // delete the previous one
    win32SND_FxDel(_pst_SB);
    

    // create the wet buffer
    win32SND_gb_FxAlloc = TRUE;
#ifdef JADEFUSION
	win32SND_SB_Duplicate(SND_gst_Params.pst_SpecificD, _pst_SB, (SND_tdst_SoundBuffer_**)_pst_SB->pst_Fx);
#else
	win32SND_SB_Duplicate(SND_gst_Params.pst_SpecificD, _pst_SB, &GetFxSB(_pst_SB));
#endif
	win32SND_gb_FxAlloc = FALSE;
    if(!GetFxSB(_pst_SB)) return;

    // register new settings
    _pst_SB->i_HasFx = _i_Core + 1;
    _pst_SB->i_FxVol = _i_FxVol;

    // go ahead...
    if(!win32SND_dst_FxDesc[_i_Core].b_IsEnabled) return;
   
    
    switch(win32SND_dst_FxDesc[_i_Core].st_Settings.i_Mode)
	{
    default:
	case SND_Cte_FxMode_Off:
        break;

	case SND_Cte_FxMode_Room:
	case SND_Cte_FxMode_StudioA:
	case SND_Cte_FxMode_StudioB:
	case SND_Cte_FxMode_StudioC:
	case SND_Cte_FxMode_Hall:
	case SND_Cte_FxMode_Space:
	case SND_Cte_FxMode_Pipe:
	case SND_Cte_FxMode_City:
	case SND_Cte_FxMode_Mountains:
        win32SND_gast_EffectDesc[_i_Core].dwSize = sizeof(DSEFFECTDESC);
        win32SND_gast_EffectDesc[_i_Core].dwFlags = 0;
        win32SND_gast_EffectDesc[_i_Core].guidDSFXClass = GUID_DSFX_STANDARD_I3DL2REVERB;
        // set Fx to wet buffer
 #ifdef JADEFUSION
		hr = GetFxSB(_pst_SB)->pst_DSB->SetFX(1, &win32SND_gast_EffectDesc[_i_Core], &dwResult);
#else  
		hr = IDirectSoundBuffer8_SetFX(GetFxSB(_pst_SB)->pst_DSB, 1, &win32SND_gast_EffectDesc[_i_Core], &dwResult);
#endif
		win32SND_M_Assert((hr==DS_OK) || (hr==DS_INCOMPLETE));
        win32SND_M_Assert((dwResult!=DSFXR_UNKNOWN) && (dwResult!=DSFXR_FAILED));
        // get Fx interface
#ifdef JADEFUSION
		hr = GetFxSB(_pst_SB)->pst_DSB->GetObjectInPath(GUID_DSFX_STANDARD_I3DL2REVERB, 0, IID_IDirectSoundFXI3DL2Reverb8, &GetFxInterface(_pst_SB));
#else
		hr = IDirectSoundBuffer8_GetObjectInPath(GetFxSB(_pst_SB)->pst_DSB, &GUID_DSFX_STANDARD_I3DL2REVERB, 0, &IID_IDirectSoundFXI3DL2Reverb8, &GetFxInterface(_pst_SB));
#endif
		win32SND_M_Assert(hr==DS_OK);
        break;
    case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Echo:
        /*
        win32SND_gast_EffectDesc[_i_Core].dwSize = sizeof(DSEFFECTDESC);
        win32SND_gast_EffectDesc[_i_Core].dwFlags = 0;
        win32SND_gast_EffectDesc[_i_Core].guidDSFXClass = GUID_DSFX_STANDARD_ECHO;
        // set Fx to wet buffer
        hr = IDirectSoundBuffer8_SetFX(GetFxSB(_pst_SB)->pst_DSB, 1, &win32SND_gast_EffectDesc[_i_Core], &dwResult);
        win32SND_M_Assert((hr==DS_OK) || (hr==DS_INCOMPLETE));
        win32SND_M_Assert((dwResult!=DSFXR_UNKNOWN) && (dwResult!=DSFXR_FAILED));
        // get Fx interface
        hr = IDirectSoundBuffer8_GetObjectInPath(GetFxSB(_pst_SB)->pst_DSB, &GUID_DSFX_STANDARD_ECHO, 0, &IID_IDirectSoundFXEcho, &GetFxInterface(_pst_SB));
        win32SND_M_Assert(hr==DS_OK);
        */
        win32SND_gast_EffectDesc[_i_Core].dwSize = sizeof(DSEFFECTDESC);
        win32SND_gast_EffectDesc[_i_Core].dwFlags = 0;
        win32SND_gast_EffectDesc[_i_Core].guidDSFXClass = GUID_DSFX_WAVES_REVERB;
        // set Fx to wet buffer
#ifdef JADEFUSION
		hr = GetFxSB(_pst_SB)->pst_DSB->SetFX(1, &win32SND_gast_EffectDesc[_i_Core], &dwResult);
#else
		hr = IDirectSoundBuffer8_SetFX(GetFxSB(_pst_SB)->pst_DSB, 1, &win32SND_gast_EffectDesc[_i_Core], &dwResult);
#endif
		win32SND_M_Assert((hr==DS_OK) || (hr==DS_INCOMPLETE));
        win32SND_M_Assert((dwResult!=DSFXR_UNKNOWN) && (dwResult!=DSFXR_FAILED));
        // get Fx interface
        hr = IDirectSoundBuffer8_GetObjectInPath(GetFxSB(_pst_SB)->pst_DSB, &GUID_DSFX_WAVES_REVERB, 0, &IID_IDirectSoundFXWavesReverb8, &GetFxInterface(_pst_SB));
        win32SND_M_Assert(hr==DS_OK);
        break;
    }



    switch(win32SND_dst_FxDesc[_i_Core].st_Settings.i_Mode)
	{
    default:
	case SND_Cte_FxMode_Off:
        break;

	case SND_Cte_FxMode_Room:
        IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_ROOM);
        break;
	case SND_Cte_FxMode_StudioA:
        IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_SMALLROOM);
        break;
	case SND_Cte_FxMode_StudioB:
        IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_MEDIUMROOM);
        break;
	case SND_Cte_FxMode_StudioC:
        IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEROOM);
        break;
	case SND_Cte_FxMode_Hall:
        IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_HALLWAY);
        break;
	case SND_Cte_FxMode_Space:
        IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_GENERIC);
        break;
	case SND_Cte_FxMode_Pipe:
        IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_SEWERPIPE);
        break;
	case SND_Cte_FxMode_City:
        IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_CITY);
        break;
	case SND_Cte_FxMode_Mountains:
        IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_MOUNTAINS);
        break;

    case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Echo:
        //IDirectSoundFXI3DL2Reverb_SetPreset((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), DSFX_I3DL2_ENVIRONMENT_PRESET_LARGEHALL);
        /*
        echo[_i_Core].fFeedback = DSFXECHO_FEEDBACK_MAX;
        echo[_i_Core].fLeftDelay= DSFXECHO_LEFTDELAY_MAX;
        echo[_i_Core].fRightDelay = DSFXECHO_RIGHTDELAY_MAX;
        echo[_i_Core].fWetDryMix = DSFXECHO_WETDRYMIX_MAX;
        echo[_i_Core].lPanDelay = 0;
        IDirectSoundFXEcho_SetAllParameters((IDirectSoundFXEcho*)GetFxInterface(_pst_SB), &echo[_i_Core]);
        */
        reverb[_i_Core].fHighFreqRTRatio = DSFX_WAVESREVERB_HIGHFREQRTRATIO_DEFAULT;
        reverb[_i_Core].fInGain= DSFX_WAVESREVERB_INGAIN_DEFAULT;
        reverb[_i_Core].fReverbMix= DSFX_WAVESREVERB_REVERBMIX_MAX;
        reverb[_i_Core].fReverbTime= DSFX_WAVESREVERB_REVERBTIME_MAX;
        IDirectSoundFXWavesReverb_SetAllParameters((IDirectSoundFXWavesReverb8*)GetFxInterface(_pst_SB), &reverb[_i_Core]);
        break;
    }
#endif
}

void win32SND_FxDel(SND_tdst_SoundBuffer *_pst_SB)
{
 #ifdef USE_FX
    if(win32SND_gb_NoFx) return;

    if(GetFxSB(_pst_SB))
    {
#ifdef JADEFUSION
		 GetFxSB(_pst_SB)->pst_DSB->SetFX(0, NULL, NULL);
#else
		 IDirectSoundBuffer8_SetFX(GetFxSB(_pst_SB)->pst_DSB, 0, NULL, NULL);
#endif
		 GetFxInterface(_pst_SB) = NULL;
        
        win32SND_gb_FxAlloc = TRUE;
        win32SND_SB_Release(GetFxSB(_pst_SB));
        win32SND_gb_FxAlloc = FALSE;
#ifdef JADEFUSION
		_pst_SB->pst_Fx = NULL;
#else
		GetFxSB(_pst_SB) = NULL;
#endif
}

    _pst_SB->i_HasFx = 0;
    _pst_SB->i_FxVol = 0;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32SND_i_FxEnable(int _i_Core)
{
    win32SND_dst_FxDesc[_i_Core].b_IsEnabled = TRUE;

    if(win32SND_dst_FxDesc[_i_Core].b_HasChanged_Mode || win32SND_dst_FxDesc[_i_Core].b_HasChanged_Delay || win32SND_dst_FxDesc[_i_Core].b_HasChanged_Feedback)
    {
        win32SND_dst_FxDesc[_i_Core].b_HasChanged_Mode = FALSE;
        win32SND_dst_FxDesc[_i_Core].b_HasChanged_Delay = FALSE;
        win32SND_dst_FxDesc[_i_Core].b_HasChanged_Feedback = FALSE;
    }

    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int win32SND_i_FxDisable(int _i_Core)
{
    win32SND_dst_FxDesc[_i_Core].b_IsEnabled = FALSE;
#ifdef USE_FX
    int i;
    SND_tdst_SoundInstance* pst_SI;

    if(win32SND_gb_NoFx) return 0;

	ERR_X_Warning((_i_Core == SND_Cte_FxCoreA) || (_i_Core == SND_Cte_FxCoreB), "[SND][win32SNDfx.c] bad core id", NULL);

    for(i=0; i<SND_gst_Params.l_InstanceNumber; i++)
    {
        pst_SI = SND_gst_Params.dst_Instance + i;

        // skip free instance
        if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
        // skip bad DSB pointers
        if(pst_SI->pst_DSB == NULL) continue;
        if(pst_SI->pst_LI == &SND_gpst_Interface[IStream]) continue;
        // no fx
        if(pst_SI->pst_DSB->i_HasFx != (_i_Core+1)) continue;

        IDirectSoundBuffer8_SetFX(pst_SI->pst_DSB->pst_DSB, 0, NULL, NULL);
        pst_SI->pst_DSB->pst_Fx = NULL;
    }
#endif
    return 0;
}



/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
