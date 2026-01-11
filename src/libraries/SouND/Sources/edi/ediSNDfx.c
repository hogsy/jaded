/*$T ediSNDfx.c GC 1.138 11/05/04 09:05:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "LINks/LINKmsg.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/edi/ediSNDdebug.h"
#include "SouND/Sources/edi/ediSND.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef struct	ediSND_tdst_FxDesc_
{
    BOOL                    b_HasChanged_Delay;
    BOOL                    b_HasChanged_Feedback;
    BOOL                    b_HasChanged_WetVol;
    BOOL                    b_HasChanged_Mode;
    BOOL                    b_IsEnabled;
	SND_tdst_FxParam		st_Settings;
} ediSND_tdst_FxDesc;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

ediSND_tdst_FxDesc	ediSND_dst_FxDesc[SND_Cte_FxCoreNb];
BOOL ediSND_gb_FxAlloc = FALSE;
BOOL ediSND_gb_NoFx = FALSE;
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_FxInit(void)
{
	/*~~*/
	int i;
	/*~~*/

	L_memset(ediSND_dst_FxDesc, 0, SND_Cte_FxCoreNb * sizeof(ediSND_tdst_FxDesc));

	for(i = 0; i < SND_Cte_FxCoreNb; i++)
	{
		ediSND_dst_FxDesc[i].st_Settings.i_Mode = SND_Cte_FxMode_Off;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_FxClose(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_FxSetMode(int core, int mode)
{
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][ediSNDfx.c] bad core id", NULL);

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
        if(ediSND_dst_FxDesc[core].st_Settings.i_Mode != mode)
            ediSND_dst_FxDesc[core].b_HasChanged_Mode = TRUE;
		ediSND_dst_FxDesc[core].st_Settings.i_Mode = mode;
		break;

	default:
		ediSND_dst_FxDesc[core].st_Settings.i_Mode = SND_Cte_FxMode_Off;
        ediSND_dst_FxDesc[core].b_HasChanged_Mode = TRUE;
		ERR_X_Warning(0, "[SND][ediSNDfx.c] bad mode", NULL);
		break;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_FxSetFeedback(int core, int feedback)
{
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][ediSNDfx.c] bad core id", NULL);
	ERR_X_Warning((0 <= feedback) || (feedback <= 100), "[SND][ediSNDfx.c] bad feedback value", NULL);

	if(feedback < 0) feedback = 0;
	if(feedback > 100) feedback = 100;

    if(ediSND_dst_FxDesc[core].st_Settings.i_Feedback != feedback)
        ediSND_dst_FxDesc[core].b_HasChanged_Feedback = TRUE;

	ediSND_dst_FxDesc[core].st_Settings.i_Feedback = feedback;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_FxSetDelay(int core, int delay)
{
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][ediSNDfx.c] bad core id", NULL);
	ERR_X_Warning((0 <= delay) || (delay <= 1000), "[SND][ediSNDfx.c] bad delay value", NULL);

	if(delay < 0) delay = 0;
	if(delay > 1000) delay = 1000;

    if(ediSND_dst_FxDesc[core].st_Settings.i_Delay != delay)
        ediSND_dst_FxDesc[core].b_HasChanged_Delay = TRUE;

	ediSND_dst_FxDesc[core].st_Settings.i_Delay = delay;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_FxSetWetVolume(int core, int volume)
{
    float f_New;
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][ediSNDfx.c] bad core id", NULL);

    f_New = SND_f_GetVolFromAtt(volume);
    
    if(ediSND_dst_FxDesc[core].st_Settings.f_WetVol != f_New)
        ediSND_dst_FxDesc[core].b_HasChanged_WetVol = TRUE;

	ediSND_dst_FxDesc[core].st_Settings.f_WetVol = f_New;
    //TODO apply new vol
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_FxSetWetPan(int core, int pan)
{
	ERR_X_Warning((core == SND_Cte_FxCoreA) || (core == SND_Cte_FxCoreB), "[SND][ediSNDfx.c] bad core id", NULL);
	ERR_X_Warning((-10000 <= pan) || (pan <= 10000), "[SND][ediSNDfx.c] bad pan value", NULL);

	if(pan < -10000) pan = -10000;
	if(pan > 10000) pan = 10000;

	ediSND_dst_FxDesc[core].st_Settings.i_WetPan = pan;
	return 0;
}

DSEFFECTDESC    ediSND_gast_EffectDesc[SND_Cte_FxCoreNb];
DSFXWavesReverb reverb[SND_Cte_FxCoreNb];

BOOL g_useglobalfx = FALSE;
DSFXWavesReverb g_reverb;
DSFXI3DL2Reverb g_i3dl2;

#define DS_INCOMPLETE 0x08780014

void ediSND_FxAdd(SND_tdst_SoundBuffer *_pst_SB, int _i_Core, int _i_FxVol)
{
#ifdef USE_FX
    DWORD dwResult;
    HRESULT hr;

    if(ediSND_gb_NoFx) return;
    
    // delete the previous one
    ediSND_FxDel(_pst_SB);
    

    // create the wet buffer
    ediSND_gb_FxAlloc = TRUE;
#ifdef JADEFUSION
	ediSND_SB_Duplicate(SND_gst_Params.pst_SpecificD, _pst_SB, (SND_tdst_SoundBuffer_**)&_pst_SB->pst_Fx);
#else
	ediSND_SB_Duplicate(SND_gst_Params.pst_SpecificD, _pst_SB, &GetFxSB(_pst_SB));
#endif
	ediSND_gb_FxAlloc = FALSE;
    if(!GetFxSB(_pst_SB)) return;

    // register new settings
    _pst_SB->i_HasFx = _i_Core + 1;
    _pst_SB->i_FxVol = _i_FxVol;

    // go ahead...
    if(!ediSND_dst_FxDesc[_i_Core].b_IsEnabled) return;
   
    if(g_useglobalfx == 1)
    {
        ediSND_gast_EffectDesc[_i_Core].dwSize = sizeof(DSEFFECTDESC);
        ediSND_gast_EffectDesc[_i_Core].dwFlags = 0;
        ediSND_gast_EffectDesc[_i_Core].guidDSFXClass = GUID_DSFX_WAVES_REVERB;
        // set Fx to wet buffer
        hr = IDirectSoundBuffer8_SetFX(GetFxSB(_pst_SB)->pst_DSB, 1, &ediSND_gast_EffectDesc[_i_Core], &dwResult);
        ediSND_M_Assert((hr==DS_OK) || (hr==DS_INCOMPLETE));
        ediSND_M_Assert((dwResult!=DSFXR_UNKNOWN) && (dwResult!=DSFXR_FAILED));
        // get Fx interface
#ifdef JADEFUSION
        hr = IDirectSoundBuffer8_GetObjectInPath(GetFxSB(_pst_SB)->pst_DSB, GUID_DSFX_WAVES_REVERB, 0, IID_IDirectSoundFXWavesReverb8, &GetFxInterface(_pst_SB));
#else
		hr = IDirectSoundBuffer8_GetObjectInPath(GetFxSB(_pst_SB)->pst_DSB, &GUID_DSFX_WAVES_REVERB, 0, &IID_IDirectSoundFXWavesReverb8, &GetFxInterface(_pst_SB));
#endif
		ediSND_M_Assert(hr==DS_OK);

        IDirectSoundFXWavesReverb_SetAllParameters((IDirectSoundFXWavesReverb8*)GetFxInterface(_pst_SB), &g_reverb);
        return;
    }
    else if(g_useglobalfx == 2)
    {
        ediSND_gast_EffectDesc[_i_Core].dwSize = sizeof(DSEFFECTDESC);
        ediSND_gast_EffectDesc[_i_Core].dwFlags = 0;
        ediSND_gast_EffectDesc[_i_Core].guidDSFXClass = GUID_DSFX_STANDARD_I3DL2REVERB;
        // set Fx to wet buffer
        hr = IDirectSoundBuffer8_SetFX(GetFxSB(_pst_SB)->pst_DSB, 1, &ediSND_gast_EffectDesc[_i_Core], &dwResult);
        ediSND_M_Assert((hr==DS_OK) || (hr==DS_INCOMPLETE));
        ediSND_M_Assert((dwResult!=DSFXR_UNKNOWN) && (dwResult!=DSFXR_FAILED));
        // get Fx interface
#ifdef JADEFUSION
		hr = IDirectSoundBuffer8_GetObjectInPath(GetFxSB(_pst_SB)->pst_DSB, GUID_DSFX_STANDARD_I3DL2REVERB, 0, IID_IDirectSoundFXI3DL2Reverb8, &GetFxInterface(_pst_SB));
#else
		hr = IDirectSoundBuffer8_GetObjectInPath(GetFxSB(_pst_SB)->pst_DSB, &GUID_DSFX_STANDARD_I3DL2REVERB, 0, &IID_IDirectSoundFXI3DL2Reverb8, &GetFxInterface(_pst_SB));
#endif
		ediSND_M_Assert(hr==DS_OK);

        IDirectSoundFXI3DL2Reverb_SetAllParameters((IDirectSoundFXI3DL2Reverb8*)GetFxInterface(_pst_SB), &g_i3dl2);
        return;
    }

    switch(ediSND_dst_FxDesc[_i_Core].st_Settings.i_Mode)
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
		L_zero( &ediSND_gast_EffectDesc[ _i_Core ], sizeof( DSEFFECTDESC ) );
        ediSND_gast_EffectDesc[_i_Core].dwSize = sizeof(DSEFFECTDESC);
        ediSND_gast_EffectDesc[_i_Core].dwFlags = 0;
        ediSND_gast_EffectDesc[_i_Core].guidDSFXClass = GUID_DSFX_STANDARD_I3DL2REVERB;
        // set Fx to wet buffer
        hr = IDirectSoundBuffer8_SetFX(GetFxSB(_pst_SB)->pst_DSB, 1, &ediSND_gast_EffectDesc[_i_Core], &dwResult);
		if ( hr == DS_OK )
		{
			ediSND_M_Assert( ( dwResult != DSFXR_UNKNOWN ) && ( dwResult != DSFXR_FAILED ) );
			// get Fx interface
			hr = IDirectSoundBuffer8_GetObjectInPath( GetFxSB( _pst_SB )->pst_DSB, &GUID_DSFX_STANDARD_I3DL2REVERB, 0, &IID_IDirectSoundFXI3DL2Reverb8, &GetFxInterface( _pst_SB ) );
			ediSND_M_Assert( hr == DS_OK );
		}
        break;
    case SND_Cte_FxMode_Delay:
	case SND_Cte_FxMode_Echo:
		L_zero( &ediSND_gast_EffectDesc[ _i_Core ], sizeof( DSEFFECTDESC ) );
        ediSND_gast_EffectDesc[_i_Core].dwSize = sizeof(DSEFFECTDESC);
        ediSND_gast_EffectDesc[_i_Core].dwFlags = 0;
        ediSND_gast_EffectDesc[_i_Core].guidDSFXClass = GUID_DSFX_WAVES_REVERB;
        // set Fx to wet buffer
        hr = IDirectSoundBuffer8_SetFX(GetFxSB(_pst_SB)->pst_DSB, 1, &ediSND_gast_EffectDesc[_i_Core], &dwResult);
		if ( hr == DS_OK )
		{
			ediSND_M_Assert( ( dwResult != DSFXR_UNKNOWN ) && ( dwResult != DSFXR_FAILED ) );
			// get Fx interface
			hr = IDirectSoundBuffer8_GetObjectInPath( GetFxSB( _pst_SB )->pst_DSB, &GUID_DSFX_WAVES_REVERB, 0, &IID_IDirectSoundFXWavesReverb8, &GetFxInterface( _pst_SB ) );
			ediSND_M_Assert( hr == DS_OK );
		}
        break;
    }

	if ( hr != DS_OK )
	{
		char buf[ 64 ];
		snprintf( buf, sizeof( buf ), "Failed to set audio effect (%u)!", hr );
		LINK_PrintStatusMsg( buf );
		return;
	}

    switch(ediSND_dst_FxDesc[_i_Core].st_Settings.i_Mode)
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
        reverb[_i_Core].fHighFreqRTRatio = DSFX_WAVESREVERB_HIGHFREQRTRATIO_DEFAULT;
        reverb[_i_Core].fInGain= DSFX_WAVESREVERB_INGAIN_DEFAULT;
        reverb[_i_Core].fReverbMix= DSFX_WAVESREVERB_REVERBMIX_MAX;
        reverb[_i_Core].fReverbTime= DSFX_WAVESREVERB_REVERBTIME_MAX;
        IDirectSoundFXWavesReverb_SetAllParameters((IDirectSoundFXWavesReverb8*)GetFxInterface(_pst_SB), &reverb[_i_Core]);
        break;
    }
#endif
}

void ediSND_FxDel(SND_tdst_SoundBuffer *_pst_SB)
{
#ifdef USE_FX
    if(ediSND_gb_NoFx) return;
    if(GetFxSB(_pst_SB))
    {
        IDirectSoundBuffer8_SetFX(GetFxSB(_pst_SB)->pst_DSB, 0, NULL, NULL);
        GetFxInterface(_pst_SB) = NULL;
        
        ediSND_gb_FxAlloc = TRUE;
        ediSND_SB_Release(GetFxSB(_pst_SB));
        ediSND_gb_FxAlloc = FALSE;
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
int ediSND_i_FxEnable(int _i_Core)
{
    ediSND_dst_FxDesc[_i_Core].b_IsEnabled = TRUE;

    if(ediSND_dst_FxDesc[_i_Core].b_HasChanged_Mode || ediSND_dst_FxDesc[_i_Core].b_HasChanged_Delay || ediSND_dst_FxDesc[_i_Core].b_HasChanged_Feedback)
    {
        ediSND_dst_FxDesc[_i_Core].b_HasChanged_Mode = FALSE;
        ediSND_dst_FxDesc[_i_Core].b_HasChanged_Delay = FALSE;
        ediSND_dst_FxDesc[_i_Core].b_HasChanged_Feedback = FALSE;
    }

    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_FxDisable(int _i_Core)
{
    ediSND_dst_FxDesc[_i_Core].b_IsEnabled = FALSE;
#ifdef USE_FX
    int i;
    SND_tdst_SoundInstance* pst_SI;

    if(ediSND_gb_NoFx) return 0;
	ERR_X_Warning((_i_Core == SND_Cte_FxCoreA) || (_i_Core == SND_Cte_FxCoreB), "[SND][ediSNDfx.c] bad core id", NULL);

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

/*
 =======================================================================================================================
    EDITOR - ONLY
 =======================================================================================================================
 */
int ediSND_i_FxGetMode(int core)
{
	return ediSND_dst_FxDesc[core].st_Settings.i_Mode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_FxGetDelay(int core)
{
	return ediSND_dst_FxDesc[core].st_Settings.i_Delay;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_FxGetFeedback(int core)
{
	return ediSND_dst_FxDesc[core].st_Settings.i_Feedback;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float ediSND_f_FxGetWetVol(int core)
{
	return ediSND_dst_FxDesc[core].st_Settings.f_WetVol;
}


/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
