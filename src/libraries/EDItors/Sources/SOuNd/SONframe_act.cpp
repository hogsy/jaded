/*$T SONframe_act.cpp GC! 1.081 04/04/03 17:21:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include <process.h>

#include "EDImainframe.h"
#include "EDImainframe_act.h"
#include "Res/Res.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/sources/SNDconv.h"
#include "SouND/sources/SNDload.h"
#include "SouND/sources/SNDbank.h"

#include "SONframe.h"
#include "SONframe_act.h"
#include "SONview.h"
#include "SONmsg.h"
#include "SONutil.h"
#include "SONvumeter.h"

#include "DIAlogs/DIAsmodifier_dlg.h"
#include "DIAlogs/DIAsndvumeter.h"
#include "DIAlogs/DIAmixer_dlg.h"
#include "DIAlogs/DIAname_dlg.h"

#include "LINks/LINKmsg.h"



/*$4
 ***********************************************************************************************************************
    extern
 ***********************************************************************************************************************
 */

#ifdef JADEFUSION
extern BOOL BIG_b_CheckAllSoundFiles(void);
extern float SND_gf_AutoVolumeOff;
#else
extern "C" BOOL BIG_b_CheckAllSoundFiles(void);
extern "C" float SND_gf_AutoVolumeOff;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OnDisplayDeviceCapacity(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Log[2048];
	HRESULT hr;
	/*~~~~~~~~~~~~~~~~~*/

	hr = IDirectSound8_GetCaps(SND_gst_Params.pst_SpecificD->pst_DS, &ediSND_gst_SoundManager.st_DriverCapacity);
	ERR_X_Warning((hr == S_OK), "bad return value (IDirectSound8_GetCaps)", NULL);

	LINK_PrintStatusMsg("Current Sound Device :");
	LINK_PrintStatusMsg("----------------------");

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_EMULDRIVER)
		LINK_PrintStatusMsg("No DirectSound driver found, so it will be emulated");
	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_CERTIFIED)
		LINK_PrintStatusMsg("The driver is certified by MS");

	LINK_PrintStatusMsg("");
	LINK_PrintStatusMsg("Primary buffer :");

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_PRIMARY16BIT)
		LINK_PrintStatusMsg(" - 16bits supported");

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_PRIMARY8BIT)
		LINK_PrintStatusMsg(" - 8bits supported");

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_PRIMARYMONO)
		LINK_PrintStatusMsg(" - mono supported");

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_PRIMARYSTEREO)
		LINK_PrintStatusMsg(" - stereo supported");

	sprintf(sz_Log, " - Nb : %d", ediSND_gst_SoundManager.st_DriverCapacity.dwPrimaryBuffers);
	LINK_PrintStatusMsg(sz_Log);

	LINK_PrintStatusMsg("");
	LINK_PrintStatusMsg("Hw-mixed sound buffers:");
	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_SECONDARY16BIT)
		LINK_PrintStatusMsg(" - 16bit supported");

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_SECONDARY8BIT)
		LINK_PrintStatusMsg(" - 16bit supported");

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_SECONDARYMONO)
		LINK_PrintStatusMsg(" - mono supported");

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_SECONDARYSTEREO)
		LINK_PrintStatusMsg(" - stereo supported");

	LINK_PrintStatusMsg("");
	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"Freq. range : %d - %d Hz",
		ediSND_gst_SoundManager.st_DriverCapacity.dwMinSecondarySampleRate,
		ediSND_gst_SoundManager.st_DriverCapacity.dwMaxSecondarySampleRate
	);
	LINK_PrintStatusMsg(sz_Log);

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwFlags & DSCAPS_CONTINUOUSRATE)
		LINK_PrintStatusMsg("   +/- 10 hertz accuracy");

	LINK_PrintStatusMsg("");
	LINK_PrintStatusMsg("Current Hx-mixed buffers allocation :");
	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"static %3d (alloc.) %3d (freed)",
		ediSND_gst_SoundManager.st_DriverCapacity.dwMaxHwMixingStaticBuffers -
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMixingStaticBuffers,
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMixingStaticBuffers
	);
	LINK_PrintStatusMsg(sz_Log);
	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"stream %3d          %3d",
		ediSND_gst_SoundManager.st_DriverCapacity.dwMaxHwMixingStreamingBuffers -
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMixingStreamingBuffers,
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMixingStreamingBuffers
	);
	LINK_PrintStatusMsg(sz_Log);
	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"all    %3d          %3d",
		ediSND_gst_SoundManager.st_DriverCapacity.dwMaxHwMixingAllBuffers -
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMixingAllBuffers,
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMixingAllBuffers
	);
	LINK_PrintStatusMsg(sz_Log);

	LINK_PrintStatusMsg("");
	LINK_PrintStatusMsg("Current 3D buffers allocation :");
	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"static %3d (alloc.) %3d (freed)",
		ediSND_gst_SoundManager.st_DriverCapacity.dwMaxHw3DStaticBuffers -
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHw3DStaticBuffers,
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHw3DStaticBuffers
	);
	LINK_PrintStatusMsg(sz_Log);
	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"stream %3d          %3d",
		ediSND_gst_SoundManager.st_DriverCapacity.dwMaxHw3DStreamingBuffers -
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHw3DStreamingBuffers,
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMixingStreamingBuffers
	);
	LINK_PrintStatusMsg(sz_Log);
	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"all    %3d          %3d",
		ediSND_gst_SoundManager.st_DriverCapacity.dwMaxHw3DAllBuffers -
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHw3DAllBuffers,
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHw3DAllBuffers
	);
	LINK_PrintStatusMsg(sz_Log);

	LINK_PrintStatusMsg("");
	LINK_PrintStatusMsg("On board memory for static Buffers:");

	if(ediSND_gst_SoundManager.st_DriverCapacity.dwTotalHwMemBytes)
	{
		snprintf
		(
			sz_Log,
		    sizeof(sz_Log),
			"Alloc. %d (%d%%), free %d, all %d (oct)",
			ediSND_gst_SoundManager.st_DriverCapacity.dwTotalHwMemBytes - ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMemBytes,
			100 - (ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMemBytes * 100) / ediSND_gst_SoundManager.st_DriverCapacity.dwTotalHwMemBytes,
			ediSND_gst_SoundManager.st_DriverCapacity.dwFreeHwMemBytes,
			ediSND_gst_SoundManager.st_DriverCapacity.dwTotalHwMemBytes
		);
		LINK_PrintStatusMsg(sz_Log);
	}
	else
	{
		LINK_PrintStatusMsg("Alloc. 0 (0%), free 0, all 0 (oct)");
	}

	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"Largest contiguous block : %d oct",
		ediSND_gst_SoundManager.st_DriverCapacity.dwMaxContigFreeHwMemBytes
	);
	LINK_PrintStatusMsg(sz_Log);

	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"Transfert rate           : %d kbytes/s",
		ediSND_gst_SoundManager.st_DriverCapacity.dwUnlockTransferRateHwBuffers
	);
	LINK_PrintStatusMsg(sz_Log);

	LINK_PrintStatusMsg("");
	snprintf
	(
		sz_Log, sizeof(sz_Log),
		"CPU overhead for software buffer : %d%%",
		ediSND_gst_SoundManager.st_DriverCapacity.dwPlayCpuOverheadSwBuffers
	);
	LINK_PrintStatusMsg(sz_Log);

	LINK_PrintStatusMsg("---------------------------------------------------------------------------");
}

#ifdef JADEFUSION
extern BOOL g_useglobalfx ;
extern DSFXWavesReverb g_reverb;
extern DSFXI3DL2Reverb g_i3dl2;
#else
extern "C" BOOL g_useglobalfx ;
extern "C" DSFXWavesReverb g_reverb;
extern "C" DSFXI3DL2Reverb g_i3dl2;
#endif

void OnFxSet(void)
{
    char log[512];
    EDIA_cl_NameDialog	o_Dialog("Enter a new SModifier name");

    if(!g_useglobalfx) g_useglobalfx++;

    if(GetAsyncKeyState(VK_CONTROL) < 0)
    {
        switch(g_useglobalfx)
        {
        case 0:g_useglobalfx = 1;break;
        case 1:g_useglobalfx = 2;break;
        case 2:g_useglobalfx = 0;break;
        }
    }

    if(g_useglobalfx == 0)
    {
        LINK_PrintStatusMsg("Use normal reverbs");
    }
    else if(g_useglobalfx == 1)
    {
        LINK_PrintStatusMsg("Use WAVE reverb, previous settings :");
        
        sprintf(log, "fHighFreqRTRatio %.3f", g_reverb.fHighFreqRTRatio);
        LINK_PrintStatusMsg(log);

        sprintf(log, "fInGain %.3f", g_reverb.fInGain);
        LINK_PrintStatusMsg(log);
        
        sprintf(log, "fReverbMix %.3f", g_reverb.fReverbMix);
        LINK_PrintStatusMsg(log);
        
        sprintf(log, "fReverbTime %.3f", g_reverb.fReverbTime);
        LINK_PrintStatusMsg(log);

        o_Dialog.mo_Title.Format("fHighFreqRTRatio %.3f - %.3f", DSFX_WAVESREVERB_HIGHFREQRTRATIO_MIN, DSFX_WAVESREVERB_HIGHFREQRTRATIO_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_reverb.fHighFreqRTRatio);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_reverb.fHighFreqRTRatio);
            
            if(g_reverb.fHighFreqRTRatio < DSFX_WAVESREVERB_HIGHFREQRTRATIO_MIN)
                g_reverb.fHighFreqRTRatio = DSFX_WAVESREVERB_HIGHFREQRTRATIO_MIN;

            if(g_reverb.fHighFreqRTRatio > DSFX_WAVESREVERB_HIGHFREQRTRATIO_MAX)
                g_reverb.fHighFreqRTRatio = DSFX_WAVESREVERB_HIGHFREQRTRATIO_MAX;
        }
        
        o_Dialog.mo_Title.Format("fInGain %.3f - %.3f", DSFX_WAVESREVERB_INGAIN_MIN, DSFX_WAVESREVERB_INGAIN_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_reverb.fInGain);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_reverb.fInGain);
            
            if(g_reverb.fInGain < DSFX_WAVESREVERB_INGAIN_MIN)
                g_reverb.fInGain = DSFX_WAVESREVERB_INGAIN_MIN;

            if(g_reverb.fInGain > DSFX_WAVESREVERB_INGAIN_MAX)
                g_reverb.fInGain = DSFX_WAVESREVERB_INGAIN_MAX;
        }
        
        o_Dialog.mo_Title.Format("fReverbMix %.3f - %.3f", DSFX_WAVESREVERB_REVERBMIX_MIN, DSFX_WAVESREVERB_REVERBMIX_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_reverb.fReverbMix);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_reverb.fReverbMix);
            
            if(g_reverb.fReverbMix < DSFX_WAVESREVERB_REVERBMIX_MIN)
                g_reverb.fReverbMix = DSFX_WAVESREVERB_REVERBMIX_MIN;

            if(g_reverb.fReverbMix > DSFX_WAVESREVERB_REVERBMIX_MAX)
                g_reverb.fReverbMix = DSFX_WAVESREVERB_REVERBMIX_MAX;
        }
        
        o_Dialog.mo_Title.Format("fReverbTime %.3f - %.3f", DSFX_WAVESREVERB_REVERBTIME_MIN, DSFX_WAVESREVERB_REVERBTIME_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_reverb.fReverbTime);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_reverb.fReverbTime);
            
            if(g_reverb.fReverbTime < DSFX_WAVESREVERB_REVERBTIME_MIN)
                g_reverb.fReverbTime = DSFX_WAVESREVERB_REVERBTIME_MIN;

            if(g_reverb.fReverbTime > DSFX_WAVESREVERB_REVERBTIME_MAX)
                g_reverb.fReverbTime = DSFX_WAVESREVERB_REVERBTIME_MAX;
        }
        
        LINK_PrintStatusMsg("----------------------------------------");
        LINK_PrintStatusMsg("new settings :");
        
        sprintf(log, "fHighFreqRTRatio %.3f", g_reverb.fHighFreqRTRatio);
        LINK_PrintStatusMsg(log);

        sprintf(log, "fInGain %.3f", g_reverb.fInGain);
        LINK_PrintStatusMsg(log);
        
        sprintf(log, "fReverbMix %.3f", g_reverb.fReverbMix);
        LINK_PrintStatusMsg(log);
        
        sprintf(log, "fReverbTime %.3f", g_reverb.fReverbTime);
        LINK_PrintStatusMsg(log);
        
    }
    else if(g_useglobalfx == 2)
    {
        LINK_PrintStatusMsg("Use I3DL2 reverb, previous settings");
        
        sprintf(log, "flDecayHFRatio %.3f", g_i3dl2.flDecayHFRatio);
        LINK_PrintStatusMsg(log);
        sprintf(log, "flDecayTime %.3f", g_i3dl2.flDecayTime);
        LINK_PrintStatusMsg(log);
        sprintf(log, "flDensity %.3f", g_i3dl2.flDensity);
        LINK_PrintStatusMsg(log);
        sprintf(log, "flDiffusion %.3f", g_i3dl2.flDiffusion);
        LINK_PrintStatusMsg(log);
        sprintf(log, "flHFReference %.3f", g_i3dl2.flHFReference);
        LINK_PrintStatusMsg(log);
        sprintf(log, "flReflectionsDelay %.3f", g_i3dl2.flReflectionsDelay);
        LINK_PrintStatusMsg(log);
        sprintf(log, "flReverbDelay%.3f", g_i3dl2.flReverbDelay);
        LINK_PrintStatusMsg(log);
        sprintf(log, "flRoomRolloffFactor %.3f", g_i3dl2.flRoomRolloffFactor);
        LINK_PrintStatusMsg(log);
        sprintf(log, "lReflections %d", g_i3dl2.lReflections);
        LINK_PrintStatusMsg(log);
        sprintf(log, "lReverb %d", g_i3dl2.lReverb);
        LINK_PrintStatusMsg(log);
        sprintf(log, "lRoom %d", g_i3dl2.lRoom);
        LINK_PrintStatusMsg(log);
        sprintf(log, "lRoomHF %d", g_i3dl2.lRoomHF);
        LINK_PrintStatusMsg(log);
        //
        o_Dialog.mo_Title.Format("flDecayHFRatio %.3f - %.3f", DSFX_I3DL2REVERB_DECAYHFRATIO_MIN, DSFX_I3DL2REVERB_DECAYHFRATIO_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_i3dl2.flDecayHFRatio);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_i3dl2.flDecayHFRatio);
            
            if(g_i3dl2.flDecayHFRatio < DSFX_I3DL2REVERB_DECAYHFRATIO_MIN)
                g_i3dl2.flDecayHFRatio = DSFX_I3DL2REVERB_DECAYHFRATIO_MIN;

            if(g_i3dl2.flDecayHFRatio > DSFX_I3DL2REVERB_DECAYHFRATIO_MAX)
                g_i3dl2.flDecayHFRatio = DSFX_I3DL2REVERB_DECAYHFRATIO_MAX;
        }
        o_Dialog.mo_Title.Format("flDecayTime %.3f - %.3f", DSFX_I3DL2REVERB_DECAYTIME_MIN, DSFX_I3DL2REVERB_DECAYTIME_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_i3dl2.flDecayTime);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_i3dl2.flDecayTime);
            
            if(g_i3dl2.flDecayTime < DSFX_I3DL2REVERB_DECAYTIME_MIN)
                g_i3dl2.flDecayTime = DSFX_I3DL2REVERB_DECAYTIME_MIN;

            if(g_i3dl2.flDecayTime > DSFX_I3DL2REVERB_DECAYTIME_MAX)
                g_i3dl2.flDecayTime = DSFX_I3DL2REVERB_DECAYTIME_MAX;
        }
        o_Dialog.mo_Title.Format("flDensity %.3f - %.3f", DSFX_I3DL2REVERB_DENSITY_MIN, DSFX_I3DL2REVERB_DENSITY_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_i3dl2.flDensity);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_i3dl2.flDensity);
            
            if(g_i3dl2.flDensity < DSFX_I3DL2REVERB_DENSITY_MIN)
                g_i3dl2.flDensity = DSFX_I3DL2REVERB_DENSITY_MIN;

            if(g_i3dl2.flDensity > DSFX_I3DL2REVERB_DENSITY_MAX)
                g_i3dl2.flDensity = DSFX_I3DL2REVERB_DENSITY_MAX;
        }
        o_Dialog.mo_Title.Format("flDiffusion %.3f - %.3f", DSFX_I3DL2REVERB_DIFFUSION_MIN, DSFX_I3DL2REVERB_DIFFUSION_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_i3dl2.flDiffusion);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_i3dl2.flDiffusion);
            
            if(g_i3dl2.flDiffusion < DSFX_I3DL2REVERB_DIFFUSION_MIN)
                g_i3dl2.flDiffusion = DSFX_I3DL2REVERB_DIFFUSION_MIN;

            if(g_i3dl2.flDiffusion > DSFX_I3DL2REVERB_DIFFUSION_MAX)
                g_i3dl2.flDiffusion = DSFX_I3DL2REVERB_DIFFUSION_MAX;
        }
        o_Dialog.mo_Title.Format("flHFReference %.3f - %.3f", DSFX_I3DL2REVERB_HFREFERENCE_MIN, DSFX_I3DL2REVERB_HFREFERENCE_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_i3dl2.flHFReference);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_i3dl2.flHFReference);
            
            if(g_i3dl2.flHFReference < DSFX_I3DL2REVERB_HFREFERENCE_MIN)
                g_i3dl2.flHFReference = DSFX_I3DL2REVERB_HFREFERENCE_MIN;

            if(g_i3dl2.flHFReference > DSFX_I3DL2REVERB_HFREFERENCE_MAX)
                g_i3dl2.flHFReference = DSFX_I3DL2REVERB_HFREFERENCE_MAX;
        }
        o_Dialog.mo_Title.Format("flReflectionsDelay %.3f - %.3f", DSFX_I3DL2REVERB_REFLECTIONS_MIN, DSFX_I3DL2REVERB_REFLECTIONS_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_i3dl2.flReflectionsDelay);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_i3dl2.flReflectionsDelay);
            
            if(g_i3dl2.flReflectionsDelay < DSFX_I3DL2REVERB_REFLECTIONS_MIN)
                g_i3dl2.flReflectionsDelay = DSFX_I3DL2REVERB_REFLECTIONS_MIN;

            if(g_i3dl2.flReflectionsDelay > DSFX_I3DL2REVERB_REFLECTIONS_MAX)
                g_i3dl2.flReflectionsDelay = DSFX_I3DL2REVERB_REFLECTIONS_MAX;
        }
        o_Dialog.mo_Title.Format("flReverbDelay %.3f - %.3f", DSFX_I3DL2REVERB_REVERB_MIN, DSFX_I3DL2REVERB_REVERB_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_i3dl2.flReverbDelay);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_i3dl2.flReverbDelay);
            
            if(g_i3dl2.flReverbDelay < DSFX_I3DL2REVERB_REVERB_MIN)
                g_i3dl2.flReverbDelay = DSFX_I3DL2REVERB_REVERB_MIN;

            if(g_i3dl2.flReverbDelay > DSFX_I3DL2REVERB_REVERB_MAX)
                g_i3dl2.flReverbDelay = DSFX_I3DL2REVERB_REVERB_MAX;
        }
        o_Dialog.mo_Title.Format("lReflections %d - %d", DSFX_I3DL2REVERB_REFLECTIONS_MIN, DSFX_I3DL2REVERB_REFLECTIONS_MAX);
        o_Dialog.mo_Name.Format("%d", g_i3dl2.lReflections);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%d", &g_i3dl2.lReflections);
            
            if(g_i3dl2.lReflections < DSFX_I3DL2REVERB_REFLECTIONS_MIN)
                g_i3dl2.lReflections = DSFX_I3DL2REVERB_REFLECTIONS_MIN;

            if(g_i3dl2.lReflections > DSFX_I3DL2REVERB_REFLECTIONS_MAX)
                g_i3dl2.lReflections = DSFX_I3DL2REVERB_REFLECTIONS_MAX;
        }
        o_Dialog.mo_Title.Format("flRoomRolloffFactor %.3f - %.3f", DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MIN, DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MAX);
        o_Dialog.mo_Name.Format("%.4f", g_i3dl2.flRoomRolloffFactor);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%f", &g_i3dl2.flRoomRolloffFactor);
            
            if(g_i3dl2.flRoomRolloffFactor < DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MIN)
                g_i3dl2.flRoomRolloffFactor = DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MIN;

            if(g_i3dl2.flRoomRolloffFactor > DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MAX)
                g_i3dl2.flRoomRolloffFactor = DSFX_I3DL2REVERB_ROOMROLLOFFFACTOR_MAX;
        }
        o_Dialog.mo_Title.Format("lRoomHF %d - %d", DSFX_I3DL2REVERB_ROOMHF_MIN, DSFX_I3DL2REVERB_ROOMHF_MAX);
        o_Dialog.mo_Name.Format("%d", g_i3dl2.lRoomHF);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%d", &g_i3dl2.lRoomHF);
            
            if(g_i3dl2.lRoomHF < DSFX_I3DL2REVERB_ROOMHF_MIN)
                g_i3dl2.lRoomHF = DSFX_I3DL2REVERB_ROOMHF_MIN;

            if(g_i3dl2.lRoomHF > DSFX_I3DL2REVERB_ROOMHF_MAX)
                g_i3dl2.lRoomHF = DSFX_I3DL2REVERB_ROOMHF_MAX;
        }
        o_Dialog.mo_Title.Format("lRoom %d - %d", DSFX_I3DL2REVERB_ROOM_MIN, DSFX_I3DL2REVERB_ROOM_MAX);
        o_Dialog.mo_Name.Format("%d", g_i3dl2.lRoom);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%d", &g_i3dl2.lRoom);
            
            if(g_i3dl2.lRoom < DSFX_I3DL2REVERB_ROOM_MIN)
                g_i3dl2.lRoom = DSFX_I3DL2REVERB_ROOM_MIN;

            if(g_i3dl2.lRoom > DSFX_I3DL2REVERB_ROOM_MAX)
                g_i3dl2.lRoom = DSFX_I3DL2REVERB_ROOM_MAX;
        }
        o_Dialog.mo_Title.Format("lReverb %d - %d", DSFX_I3DL2REVERB_REVERB_MIN, DSFX_I3DL2REVERB_REVERB_MAX);
        o_Dialog.mo_Name.Format("%d", g_i3dl2.lReverb);
        if(o_Dialog.DoModal() == IDOK)
        {
            sscanf(LPCSTR(o_Dialog.mo_Name), "%d", &g_i3dl2.lReverb);
            
            if(g_i3dl2.lReverb < DSFX_I3DL2REVERB_REVERB_MIN)
                g_i3dl2.lReverb = DSFX_I3DL2REVERB_REVERB_MIN;

            if(g_i3dl2.lReverb > DSFX_I3DL2REVERB_REVERB_MAX)
                g_i3dl2.lReverb = DSFX_I3DL2REVERB_REVERB_MAX;
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnAction(ULONG _ul_Action)
{
	switch(_ul_Action)
	{
	
		//$1- insert
	case ESON_ACTION_INS_NEW:
		Insert_OnNew();
		break;

	case ESON_ACTION_INS_OPEN:
		Insert_OnOpen();
		break;
	
	/*$1- option ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ACTION_NO_SYNCHRO:
		if(mst_Ini.i_Option & ESON_Cte_NoSynchro)
		{
			mst_Ini.i_Option &= ~ESON_Cte_NoSynchro;
			SND_gst_Params.ul_Flags &= ~SND_Cte_EdiNoSynchro;
		}
		else
		{
			if(
			MessageBox
			(
				"Disable Synchronization ?",
				"Please confirm your action",
				MB_ICONQUESTION | MB_YESNO
			) == IDYES)
			{
				mst_Ini.i_Option |= ESON_Cte_NoSynchro;
				SND_gst_Params.ul_Flags |= SND_Cte_EdiNoSynchro;
			}
		}
		break;

	case ESON_ACTION_ENABLE_DEBUGLOG:
		mst_Ini.i_Option ^= ESON_Cte_EnableDebugLog;
#ifdef SND_DEBUG
		SND_gst_Debug.b_EnableDebugLog = (mst_Ini.i_Option & ESON_Cte_EnableDebugLog) ? 1 : 0;
#endif
		break;

	case ESON_ACTION_CLOSEALLWHENDESTROYWORLD:
		mst_Ini.i_Option ^= ESON_Cte_CloseAllWhenDestroyWorld;
		break;

	case ESON_ACTION_AUTOPLAY:
		mst_Ini.i_Option ^= ESON_Cte_AutoPlay;
		break;

	case ESON_ACTION_DISPLAY_EFFVOL:
		mst_Ini.i_Option ^= ESON_Cte_DispEffVol;
		break;

	case ESON_ACTION_NOFREQTRACKMODULATION:
		SND_gst_Params.ul_Flags ^= SND_Cte_EdiNoTrackFreqModulation;
		break;

	case ESON_ACTION_AUTOSAVE:
		mst_Ini.i_Option ^= ESON_Cte_AutoSave;
		mst_SModifierDesc.b_AutoSave = (mst_Ini.i_Option & ESON_Cte_AutoSave) ? TRUE : FALSE;
		break;

	case ESON_ACTION_SMODIFIERAUTOOPEN:
		mst_Ini.i_Option ^= ESON_Cte_AutoOpen;
		mst_SModifierDesc.b_AutoSave = (mst_Ini.i_Option & ESON_Cte_AutoOpen) ? TRUE : FALSE;
		break;
		break;

	case ESON_ACTION_VOLUMEOFF:
		OnVolumeOff();
		break;

	case ESON_ACTION_EDITOR:
		External_SetEditor();
		break;

	case ESON_ACTION_TEMPDIR:
		External_SetTempDir();
		break;

	case ESON_ACTION_EDIT:
		External_Edit();
		break;

	case ESON_ACTION_RELOAD:
		External_Reload();
		break;

	/*$1- general ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ACTION_CHECKALLFILES:
		BIG_b_CheckAllSoundFiles();
		break;

	case ESON_ACTION_SOUNDSPY:
		Sound_SetSpy(FALSE);
		break;

	/*$1- SModifier ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ACTION_SMODIFIER_NEW:
		SModifier_New();
		break;

	case ESON_ACTION_SMODIFIER_OPEN:
		SModifier_Open();
		break;

	case ESON_ACTION_SMODIFIER_SAVE:
		SModifier_SaveCurrent();
		break;

	case ESON_ACTION_SMODIFIER_CLOSE:
		if(b_SModifier_CanClose()) SModifier_CloseCurrent();
		break;

	case ESON_ACTION_SMODIFIER_FINDFILE:
		SModifier_OnFindFile();
		break;

	case ESON_ACTION_SMODIFIER_USER_REPORT:
		SModifier_UserReport();
		break;

	case ESON_ACTION_SOUND_FINDREF:
		Sound_FindReference();
		break;

	case ESON_ACTION_SMODIFIER_PLAY:
		SModifier_Play();
		break;

	case ESON_ACTION_SMODIFIER_PAUSE:
		SModifier_Pause();
		break;

	case ESON_ACTION_SMODIFIER_STOP:
		SModifier_Stop();
		break;

	/*$1- display ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ACTION_DISPLAY_EQUALSPLIT:
		Display_EqualSplit();
		break;

	case ESON_ACTION_CLOSEBANK:
		SoundBank_Close();
		break;

	case ESON_ACTION_CLOSEALL:
		if(b_SModifier_CanClose()) SModifier_CloseCurrent();
		SoundBank_Close();
		Sound_Close();
		break;

	/*$1- sound ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ACTION_CLOSESND:
		Sound_Close();
		break;

	case ESON_ACTION_SOUNDSETSPY:
		Sound_SetSpy(TRUE);
		break;

	case ESON_ACTION_SOUNDUNSPY:
		Sound_ResetSpy();
		break;

	case ESON_ACTION_SOUND_PLAY:
		Sound_Play();
		break;

	case ESON_ACTION_SOUND_PAUSE:
		Sound_Pause();
		break;

	case ESON_ACTION_SOUND_STOP:
		Sound_Stop();
		break;

	/*$1- instance ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ACTION_INST_SET:
		Instance_Set();
		break;

	case ESON_ACTION_INST_ADDAIWATCH:
		Instance_AddWatch();
		break;

	case ESON_ACTION_INST_SPY:
		Instance_Spy();
		break;

	case ESON_ACTION_INST_EDITSOUND:
		Instance_OnEditSound();
		break;

	case ESON_ACTION_INST_EDITSMD:
		Instance_OnEditSmd();
		break;

	case ESON_ACTION_INST_FINDSMD:
		Instance_OnFindSmd();
		break;

	case ESON_ACTION_INST_FINDGAO:
		Instance_OnFindGao();
		break;

	case ESON_ACTION_INST_FINDSOUND:
		Instance_OnFindSound();
		break;

	case ESON_ACTION_SOUND_REPORT:
		ESON_SoundReport();
		break;

	case ESON_ACTION_SOUND_USER_REPORT:
		Sound_UserReport();
		break;

	case ESON_ACTION_SOUND_FINDFILE:
		Sound_OnFindFile();
		break;

	case ESON_ACTION_INST_SOLOGAO:
		Instance_OnSetSoloGao();
		break;

	case ESON_ACTION_INST_MUTEGAO:
		Instance_OnSetMuteGao();
		break;

	case ESON_ACTION_INST_SOLOINST:
		Instance_OnSetSolo();
		break;

	case ESON_ACTION_INST_MUTEINST:
		Instance_OnSetMute();
		break;

	case ESON_ACTION_INST_SOLOOFF:
		ESON_SoloOff();
		break;

	case ESON_ACTION_INST_MUTEOFF:
		ESON_MuteOff();
		break;

	/*$1- bank ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ACTION_BANK_REPORT:
		ESON_BankReport();
		break;

	case ESON_ACTION_BANK_CONTENTS_REPORT:
		SoundBank_ContentsReport();
		break;

	case ESON_ACTION_BANK_USER_REPORT:
		SoundBank_UserReport();
		break;

	case ESON_ACTION_MODIFYBANK:
		SoundBank_Modify();
		break;

	case ESON_ACTION_BANK_FINDFILE:
		SoundBank_OnFindFile();
		break;	
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT ESON_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~*/
	UINT	ui_State;
	/*~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;

	switch(_ul_Action)
	{
	case ESON_ACTION_AUTOPLAY:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.i_Option & ESON_Cte_AutoPlay) ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_DISPLAY_EFFVOL:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.i_Option & ESON_Cte_DispEffVol) ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_NOFREQTRACKMODULATION:
		ui_State = DFCS_BUTTONCHECK | ((SND_gst_Params.ul_Flags & SND_Cte_EdiNoTrackFreqModulation) ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_AUTOSAVE:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.i_Option & ESON_Cte_AutoSave) ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_ENABLE_DEBUGLOG:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.i_Option & ESON_Cte_EnableDebugLog) ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_NO_SYNCHRO:
		ui_State = DFCS_BUTTONCHECK | ((SND_gst_Params.ul_Flags & SND_Cte_EdiNoSynchro) ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_CLOSEALLWHENDESTROYWORLD:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.i_Option & ESON_Cte_CloseAllWhenDestroyWorld) ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_SMODIFIERAUTOOPEN:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.i_Option & ESON_Cte_AutoOpen) ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_VOLUMEOFF:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.i_Option & ESON_Cte_VolumeOff) ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_SOUNDSPY:
		ui_State = DFCS_BUTTONCHECK;
		if(SND_gst_Debug.ul_SpyKey != BIG_C_InvalidKey) ui_State |= DFCS_CHECKED;
		break;
	case ESON_ACTION_INST_SOLOGAO:
		ui_State = DFCS_BUTTONCHECK | (Instance_b_GaoIsSolo() ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_INST_MUTEGAO:
		ui_State = DFCS_BUTTONCHECK | (Instance_b_GaoIsMute() ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_INST_SOLOINST:
		ui_State = DFCS_BUTTONCHECK | (Instance_b_IsSolo() ? DFCS_CHECKED : 0);
		break;
	case ESON_ACTION_INST_MUTEINST:
		ui_State = DFCS_BUTTONCHECK | (Instance_b_IsMute() ? DFCS_CHECKED : 0);
		break;
	}

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	if(!SND_gst_Params.l_Available) return FALSE;

	switch(_ul_Action)
	{
	case ESON_ACTION_NOFREQTRACKMODULATION:
	case ESON_ACTION_AUTOPLAY:
	case ESON_ACTION_DISPLAY_EFFVOL:
	case ESON_ACTION_CLOSEALLWHENDESTROYWORLD:
	case ESON_ACTION_ENABLE_DEBUGLOG:
	case ESON_ACTION_NO_SYNCHRO:
	case ESON_ACTION_AUTOSAVE:
	case ESON_ACTION_SMODIFIERAUTOOPEN:
	case ESON_ACTION_VOLUMEOFF:
	case ESON_ACTION_SETSOLO:
	case ESON_ACTION_INS_NEW:
	case ESON_ACTION_INS_OPEN:
		break;

	case ESON_ACTION_SOUNDSPY:
#ifndef SND_DEBUG_SPY
		return FALSE;
#endif
		break;

	case ESON_ACTION_EDIT:
	case ESON_ACTION_RELOAD:
		if(*mst_Ini.sz_SoundEditor == 0) return FALSE;
		if(*mst_Ini.sz_TempDir == 0) return FALSE;
		return(mst_SDesc.ul_Key != BIG_C_InvalidKey);

	case ESON_ACTION_SMODIFIER_NEW:
		break;
	case ESON_ACTION_SMODIFIER_OPEN:
	case ESON_ACTION_SOUND_FINDREF:
		break;

	case ESON_ACTION_SMODIFIER_SAVE:
		if(mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex) return FALSE;
		if(!mst_SModifierDesc.b_NeedSaving) return FALSE;
		break;

	case ESON_ACTION_SMODIFIER_FINDFILE:
	case ESON_ACTION_SMODIFIER_CLOSE:
	case ESON_ACTION_SMODIFIER_USER_REPORT:
	case ESON_ACTION_SMODIFIER_PLAY:
	case ESON_ACTION_SMODIFIER_PAUSE:
	case ESON_ACTION_SMODIFIER_STOP:
		if(mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex) return FALSE;
		break;

	case ESON_ACTION_BANK_REPORT:
		if(!SND_gul_MainRefListSize) return FALSE;
		break;

	case ESON_ACTION_BANK_FINDFILE:
	case ESON_ACTION_BANK_CONTENTS_REPORT:
	case ESON_ACTION_BANK_USER_REPORT:
	case ESON_ACTION_MODIFYBANK:
	case ESON_ACTION_CLOSEBANK:
		if(mst_SndBankDesc.ul_Fat == BIG_C_InvalidIndex) return FALSE;
		break;

	case ESON_ACTION_SOUND_FINDFILE:
	case ESON_ACTION_SOUND_USER_REPORT:
	case ESON_ACTION_SOUNDSETSPY:
	case ESON_ACTION_CLOSESND:
	case ESON_ACTION_SOUND_PLAY:
	case ESON_ACTION_SOUND_PAUSE:
	case ESON_ACTION_SOUND_STOP:
		if(mst_SDesc.ul_Fat == BIG_C_InvalidIndex) return FALSE;
		break;

	case ESON_ACTION_SOUND_REPORT:
	case ESON_ACTION_INST_SOLOOFF:
	case ESON_ACTION_INST_MUTEOFF:
		if(!SND_gst_Params.l_SoundNumber) return FALSE;
		break;

	case ESON_ACTION_INST_SET:
	case ESON_ACTION_INST_ADDAIWATCH:
	case ESON_ACTION_INST_EDITSOUND:
	case ESON_ACTION_INST_SPY:
	case ESON_ACTION_INST_SOLOGAO:
	case ESON_ACTION_INST_MUTEGAO:
	case ESON_ACTION_INST_SOLOINST:
	case ESON_ACTION_INST_MUTEINST:
	case ESON_ACTION_INST_FINDGAO:
		if(!mpo_View) return FALSE;
		if(mpo_View->i_GetCurSel(IDC_LIST_INSTANCE) == -1) return FALSE;
		break;
	case ESON_ACTION_INST_EDITSMD:
	case ESON_ACTION_INST_FINDSMD:
		{
			/*~~~~~~~~~~~~~~~*/
			int			idx;
			CListCtrl	*po_LC;
			/*~~~~~~~~~~~~~~~*/

			if(!mpo_View) return FALSE;
			idx = mpo_View->i_GetCurSel(IDC_LIST_INSTANCE);
			if(idx == -1) return FALSE;
			po_LC = (CListCtrl *) mpo_View->GetDlgItem(IDC_LIST_INSTANCE);
			idx = po_LC->GetItemData(idx);
			if(idx == -1) return FALSE;
			if(!(SND_gst_Params.dst_Instance[idx].ul_Flags & SND_Cul_DSF_Used)) return FALSE;
			if(SND_gst_Params.dst_Instance[idx].ul_SmdKey == -1) return FALSE;
		}
		break;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnActionUI(ULONG _ul_Action, CString &_o_Ref, CString &o_Ret)
{
	/*~~~~~~~~~~~~~~~~~~*/
	char	sz_Value[260];
	/*~~~~~~~~~~~~~~~~~~*/

	o_Ret = _o_Ref;

	switch(_ul_Action)
	{
	case ESON_ACTION_EDITOR:
		sprintf(sz_Value, " (%s)", mst_Ini.sz_SoundEditor);
		o_Ret += sz_Value;
		break;

	case ESON_ACTION_TEMPDIR:
		sprintf(sz_Value, " (%s)", mst_Ini.sz_TempDir);
		o_Ret += sz_Value;
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnToolBarCommand(UINT nID)
{
	switch(nID)
	{
	case ESON_ID_STOP:
		Sound_Stop();
		break;
	case ESON_ID_PAUSE:
		Sound_Pause();
		break;
	case ESON_ID_PLAY:
		Sound_Play();
		break;
	case ESON_ID_PLAYLOOPED:
		Sound_Play(TRUE);
		break;
	case ESON_ID_CLOSE:
		Sound_Close();
		break;
	case ESON_ID_EQUALSPLIT:
		Display_EqualSplit();
		break;
	case ESON_ID_JOCKER:
        if(GetAsyncKeyState(VK_SHIFT) < 0 )
            OnFxSet();
        else
		    OnDisplayDeviceCapacity();
		break;
	case ESON_ID_SAVESMODIFIER:
		OnAction(ESON_ACTION_SMODIFIER_SAVE);
		break;
	case ESON_ID_CLOSESMODIFIER:
		OnAction(ESON_ACTION_SMODIFIER_CLOSE);
		break;
	case ESON_ID_NEWMODIFIER:
		OnAction(ESON_ACTION_SMODIFIER_NEW);
		break;
	case ESON_ID_SMD_STOP:
		SModifier_Stop();
		break;
	case ESON_ID_SMD_PAUSE:
		SModifier_Pause();
		break;
	case ESON_ID_SMD_PLAY:
		SModifier_Play();
		break;
	case ESON_ID_SMD_PLAYLOOPED:
		SModifier_Play(TRUE);
		break;
	case ESON_ID_CLOSEBANK:
		SoundBank_Close();
		break;
	case ESON_ID_CLOSEALL:
		OnAction(ESON_ACTION_CLOSEALL);
		break;
	case ESON_ID_VUMETER:
		OnVumeter();
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnToolBarCommandUI(UINT nID, CCmdUI *pUI)
{
	if(!SND_gst_Params.l_Available)
	{
		pUI->SetCheck(0);
		pUI->Enable(FALSE);
		return;
	}

	switch(nID)
	{

	/*$1- bank ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ID_CLOSEBANK:
		if(mst_SndBankDesc.ul_Fat == BIG_C_InvalidIndex)
		{
			pUI->SetCheck(0);
			pUI->Enable(FALSE);
		}
		else
		{
			pUI->SetCheck(0);
			pUI->Enable(TRUE);
		}
		break;

	/*$1- sound ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ID_PLAY:
		if(mst_SDesc.ul_Fat == BIG_C_InvalidIndex || !mst_SDesc.po_SoundBuffer)
		{
			pUI->SetCheck(0);
			pUI->Enable(FALSE);
		}
		else
		{
			/*~~~~~~~~~*/
			ULONG	play;
			/*~~~~~~~~~*/

			pUI->Enable(TRUE);

			if(mst_SDesc.b_PlayLoop)
				pUI->SetCheck(0);
			else
			{
				IDirectSoundBuffer8_GetStatus(mst_SDesc.po_SoundBuffer, &play);
				if(play & DSBSTATUS_PLAYING)
					pUI->SetCheck(1);
				else
					pUI->SetCheck(0);
			}
		}
		break;

	case ESON_ID_PLAYLOOPED:
		if(mst_SDesc.ul_Fat == BIG_C_InvalidIndex || !mst_SDesc.po_SoundBuffer)
		{
			pUI->SetCheck(0);
			pUI->Enable(FALSE);
		}
		else
		{
			/*~~~~~~~~~*/
			ULONG	play;
			/*~~~~~~~~~*/

			pUI->Enable(TRUE);

			if(!mst_SDesc.b_PlayLoop)
				pUI->SetCheck(0);
			else
			{
				IDirectSoundBuffer8_GetStatus(mst_SDesc.po_SoundBuffer, &play);
				if(play & DSBSTATUS_PLAYING)
					pUI->SetCheck(1);
				else
					pUI->SetCheck(0);
			}
		}
		break;

	case ESON_ID_PAUSE:
		if(mst_SDesc.ul_Fat == BIG_C_InvalidIndex || !mst_SDesc.po_SoundBuffer)
		{
			pUI->SetCheck(0);
			pUI->Enable(FALSE);
		}
		else
		{
			pUI->Enable(TRUE);
			if(mst_SDesc.b_Paused)
				pUI->SetCheck(1);
			else
				pUI->SetCheck(0);
		}
		break;

	case ESON_ID_STOP:
		pUI->SetCheck(0);
		if(mst_SDesc.ul_Fat == BIG_C_InvalidIndex || !mst_SDesc.po_SoundBuffer)
			pUI->Enable(FALSE);
		else
			pUI->Enable(TRUE);
		break;

	case ESON_ID_CLOSE:
		pUI->SetCheck(0);
		if(mst_SDesc.ul_Fat == BIG_C_InvalidIndex)
			pUI->Enable(FALSE);
		else
			pUI->Enable(TRUE);
		break;

	/*$1- smodifier ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ID_SMD_PLAY:
	case ESON_ID_SMD_PLAYLOOPED:
		if(mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex)
		{
			pUI->SetCheck(0);
			pUI->Enable(FALSE);
		}
		else
		{
			pUI->Enable(TRUE);
			switch(SND_i_IsPlaying(mst_SModifierDesc.i_Instance))
			{
			case 1:
				pUI->SetCheck((nID == ESON_ID_SMD_PLAY) ? 1 : 0);
				break;
			case 2:
				pUI->SetCheck((nID == ESON_ID_SMD_PLAY) ? 0 : 1);
				break;
			default:
				pUI->SetCheck(0);
				break;
			}
		}
		break;

	case ESON_ID_SMD_STOP:
	case ESON_ID_SMD_PAUSE:
	case ESON_ID_CLOSESMODIFIER:
		pUI->SetCheck(0);
		if(mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex)
			pUI->Enable(FALSE);
		else
			pUI->Enable(TRUE);
		break;

	case ESON_ID_SAVESMODIFIER:
		if((mst_SModifierDesc.ul_Fat == BIG_C_InvalidIndex) || (!mst_SModifierDesc.b_NeedSaving))
			pUI->Enable(FALSE);
		else
			pUI->Enable(TRUE);
		pUI->SetCheck(0);
		break;

	case ESON_ID_NEWMODIFIER:
		pUI->Enable(TRUE);
		pUI->SetCheck(0);
		break;

	/*$1- general ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ID_CLOSEALL:
	case ESON_ID_EQUALSPLIT:
		pUI->SetCheck(0);
		pUI->Enable(TRUE);
		break;

	case ESON_ID_JOCKER:
		if(SND_gst_Params.l_Available)
			pUI->Enable(TRUE);
		else
			pUI->Enable(FALSE);
		pUI->SetCheck(0);
		break;

	/*$1- vumeter ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_ID_VUMETER:
		pUI->Enable(TRUE);
		if(mpo_VUmeter)
		{
			if(mpo_VUmeter->mpo_SndVumeterDialog)
				pUI->SetCheck(1);
			else
				pUI->SetCheck(0);
		}
		else
			pUI->SetCheck(0);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	switch(_uw_Key)
	{
	case VK_RETURN:
	case VK_DELETE:
	case VK_UP:
	case VK_DOWN:
		return TRUE;

	case VK_SPACE:
		return FALSE;

	default: if(M_MF()->b_EditKey(_uw_Key)) return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnVumeter(void)
{
	if(!SND_gst_Params.l_Available) return;

#if 0
	if(GetAsyncKeyState(VK_SHIFT) < 0)
	{
		OnMixer();
		return;
	}

#endif
	if(!mpo_VUmeter)
	{
		mpo_VUmeter = new ESON_cl_VUmeter;
		mpo_VUmeter->mi_Falloff = mst_Ini.i_VuFalloff;
		mpo_VUmeter->mi_PeakTTL = mst_Ini.i_VuPeakTTL;
	}

	if(mpo_VUmeter->mpo_SndVumeterDialog)
		mpo_VUmeter->OnHideWindow();
	else
		mpo_VUmeter->OnShowWindow();
}

EDIA_cl_MixerDialog *po_Mixer = NULL;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_cl_Frame::OnMixer(void)
{
	_spawnlp(_P_NOWAIT, "sndvol32.exe", "sndvol32.exe", NULL);
	if(po_Mixer)
	{
		delete po_Mixer;
		po_Mixer = NULL;
	}
	else
	{
		po_Mixer = new EDIA_cl_MixerDialog;
		po_Mixer->DoModeless();
	}
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
