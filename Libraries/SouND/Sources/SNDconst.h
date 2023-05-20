/*$T SNDconst.h GC 1.138 02/22/05 10:04:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDCONST_H__
#define __SNDCONST_H__

#pragma once
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    INSTANCE
 ***********************************************************************************************************************
 */

/*$2- flags & command ------------------------------------------------------------------------------------------------*/

/* ui_Flags */
#define SND_Cul_OSF_Loaded				0x00000001
#define SND_Cul_OSF_IsPlaying			0x00000002
#define SND_Cul_SF_LoadingSound			0x00000008
#define SND_Cul_OSF_Loop				0x00000004
#define SND_Cul_SF_StreamIsPlayingNow	0x00000010
#define SND_Cul_GlobalSoundReferencing	0x00000020
#define SND_Cul_DSF_Instance			0x00000040
#define SND_Cul_DSF_DestroyWhenFinished 0x00000080
#define SND_Cul_InactiveInstance		0x00000100
#define SND_Cul_DSF_StartedOnce			0x00000200
#define SND_Cul_DSF_Used				0x00000400
#define SND_Cul_DSF_AutoPlay			0x00000800
#define SND_Cul_3						0x00001000	/* not used */
#define SND_Cul_SF_Music				0x00002000
#define SND_Cul_SF_Dialog				0x00004000
#define SND_Cul_SF_Playing				0x00008000
#define SND_Cul_SF_SModifier			0x00010000
#define SND_Cul_5						0x00020000	/* not used */
#define SND_Cul_4						0x00040000	/* not used */
#define SND_Cul_SF_Ambience				0x00080000
#define SND_Cul_SF_HeaderLoaded			0x00100000
#define SND_Cul_SF_FxA					0x00200000
#define SND_Cul_SF_FxB					0x00400000
#define SND_Cul_SF_PlayingLoop			0x00800000
#define SND_Cul_SF_DynVolXaxis			0x01000000
#define SND_Cul_SF_DynVolYaxis			0x02000000
#define SND_Cul_SF_DynVolZaxis			0x04000000
#define SND_Cul_SF_DynPan				0x08000000
#define SND_Cul_SF_DynVolSpheric		0x10000000
#define SND_Cul_SF_EdiPause				0x20000000
#define SND_Cul_SF_UseExtFlag			0x40000000
#define SND_Cte_StreamedFile			0x80000000

/* ExtFlags */
#define SND_Cul_ESF_AutoStopOutsideFar	0x00000001
#define SND_Cul_ESF_AutoPlayInsideFar	0x00000002
#define SND_Cul_ESF_StopByFarTest		0x00000004
#define SND_Cul_ESF_EdiMdFInstance		0x00000008
#define SND_Cul_ESF_PS2StartedOnce		0x00000010
#define SND_Cul_ESF_LockedForLoading	0x00000020
#define SND_Cul_ESF_1					0x00000040	/* not used */
#define SND_Cul_ESF_2					0x00000080	/* not used */
#define SND_Cul_ESF_3					0x00000100	/* not used */
#define SND_Cul_ESF_4					0x00000200	/* not used */
#define SND_Cul_ESF_5					0x00000400	/* not used */
#define SND_Cul_ESF_RandomPlay			0x00000800
#define SND_Cul_ESF_StopRq				0x00001000
#define SND_Cul_ESF_StartedOnceInVol	0x00002000
#define SND_Cul_ESF_Reserved			0x40000000


/* instance->ul_EdiFlags */
#define SND_EdiFlg_Mute					0x00000001
#define SND_EdiFlg_Solo					0x00000002
#define SND_EdiFlg_LastOpeIsGao			0x00000004
#define SND_EdiFlg_LastOpeIsInst		0x00000008
#define SND_EdiFlg_EdiUse				0x00000010
#define SND_EdiFlg_EdiColor1			0x00000020
#define SND_EdiFlg_EdiColor2			0x00000040
#define SND_EdiFlg_EdiColor3			0x00000080
#define SND_EdiFlg_EdiPrefetched		0x00000100
#define SND_EdiFlg_EdiPrefetchedUsed	0x00000200
#define SND_EdiFlg_EdiPrefetching		0x00000400

/*$2- filter ---------------------------------------------------------------------------------------------------------*/

#define SND_Cul_SF_MaskDynFlags \
		( \
			SND_Cul_OSF_Loop | \
				SND_Cul_SF_Music | \
				SND_Cul_SF_Dialog | \
				SND_Cul_SF_Ambience | \
				SND_Cul_SF_LoadingSound | \
				SND_Cul_SF_DynVolXaxis | \
				SND_Cul_SF_DynVolYaxis | \
				SND_Cul_SF_DynVolZaxis | \
				SND_Cul_SF_DynPan | \
				SND_Cul_SF_DynVolSpheric | \
				SND_Cul_SF_SModifier | \
				SND_Cul_SF_UseExtFlag \
		)
#define SND_Cul_SF_MaskFlagsFromSoundToInstance (SND_Cul_SF_LoadingSound | SND_Cul_SF_Music | SND_Cul_SF_Ambience | SND_Cul_SF_Dialog | SND_Cte_StreamedFile)
#define SND_Cul_ESF_MaskDynExtFlags \
		( \
			SND_Cul_ESF_AutoStopOutsideFar | \
				SND_Cul_ESF_AutoPlayInsideFar | \
				SND_Cul_ESF_EdiMdFInstance | \
				SND_Cul_ESF_RandomPlay \
		)
#define SND_Cul_SF_MaskFileType (SND_Cul_SF_Music | SND_Cul_SF_Dialog | SND_Cul_SF_Ambience | SND_Cul_SF_SModifier | SND_Cul_SF_LoadingSound)
#define SND_Cul_SF_MaskDynVol \
		( \
			SND_Cul_SF_DynVolSpheric | \
				SND_Cul_SF_DynVolXaxis | \
				SND_Cul_SF_DynVolYaxis | \
				SND_Cul_SF_DynVolZaxis | \
				SND_Cul_SF_DynPan \
		)

/*$2- status ---------------------------------------------------------------------------------------------------------*/

#if defined(PSX2_TARGET) || defined(_GAMECUBE) || defined(_XENON)
#define SND_Cul_SBS_Playing		0x00000001
#define SND_Cul_SBS_PlayingLoop 0x00000002
#define SND_Cul_SBC_PlayLooping 0x00000004
#else
#define SND_Cul_SBS_Playing		DSBSTATUS_PLAYING
#define SND_Cul_SBS_PlayingLoop DSBSTATUS_LOOPING
#define SND_Cul_SBC_PlayLooping DSBPLAY_LOOPING
#endif
#define SND_Cul_SBC_PlayWithoutDelay	0x10000000
#define SND_Cul_SBC_PlayFxA				0x20000000
#define SND_Cul_SBC_PlayFxB				0x40000000

/*$4
 ***********************************************************************************************************************
    global flags for SND_gst_Params
 ***********************************************************************************************************************
 */

#define SND_Cte_ForceNoSound				0x00000001
#define SND_Cte_ForceNoWav					0x00000002
#define SND_Cte_ForceNoWad					0x00000004
#define SND_Cte_ForceNoWam					0x00000008
#define SND_Cte_ForceNoWaa					0x00000010
#define SND_Cte_ForceNoSmd					0x00000020
#define SND_Cte_EdiDisplayMdFplayer			0x00000040
#define SND_Cte_EdiDisplayMdFFx				0x00000080
#define SND_Cte_EdiDisplayMdFWac			0x00000100
#define SND_Cte_EdiDisplayMdFVol			0x00000200
#define SND_Cte_EdiDisplayMicro				0x00000400
#define SND_Cte_EdiDisplayInstance			0x00000800
#define SND_Cte_Freeze3DVol					0x00001000
#define SND_Cte_FreezeDynPan				0x00002000
#define SND_Cte_EdiDisplayOnlySelection		0x00004000
#define SND_Cte_EdiDisplayNear				0x00008000
#define SND_Cte_ForceNoPlay					0x00010000
#define SND_Cte_ForceNoRq					0x00020000
#define SND_Cte_ForceNoFloatVol				0x00040000
#define SND_Cte_ForceNoFreq					0x00080000
#define SND_Cte_EdiDisplayFar				0x00100000
#define SND_Cte_EdiDisplayMiddle			0x00200000
#define SND_Cte_EdiDisplayStart				0x00400000
#define SND_Cte_EdiDisplayStop				0x01000000
#define SND_Cte_ForceNoRqVol				0x02000000
#define SND_Cte_EdiNoTrackFreqModulation	0x04000000
#define SND_Cte_EdiNoSynchro				0x08000000
#define SND_Cte_UseEAX						0x10000000
#define SND_Cte_UseGlobalIndex				0x20000000

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define SND_Cte_RenderMono					0x00000001
#define SND_Cte_RenderStereo				0x00000002
#define SND_Cte_RenderHeadPhone				0x00000004
#define SND_Cte_RenderDolbyPrologic			0x00000008
#define SND_Cte_RenderDolbyPrologicII		0x00000010
#define SND_Cte_RenderDolbyDigital			0x00000020
#define SND_Cte_RenderUseConsoleSettings	0x00000040
#define SND_Cte_RenderCapacity	            0x80000000
#define SND_Cte_RenderCurrent              (-1)
/*$4
 ***********************************************************************************************************************
    DOPPLER
 ***********************************************************************************************************************
 */

#define SND_Cte_DopplerSndSpeed			300.0f	/* sound speed in the air : about 300 m/s (for doppler) */
#define SND_Cte_DopplerMinMicroSpeed	100.0f	/* auto activation of doppler Fx */

/*$4
 ***********************************************************************************************************************
    GLOBAL
 ***********************************************************************************************************************
 */

/*$2- max/min value --------------------------------------------------------------------------------------------------*/

#define SND_Cte_MinAlloc			16				/* array is allocated by step of 16 elements */
#define SND_Cte_MinAllocInst		200				/* array is allocated by step of 200 elements (getunsed) */
#define SND_Cte_MinAllocSound		1000			/* array is allocated by step of 200 elements (getunsed) */
#define SND_Cte_MaxHardBufferNb		48				/* only 48 voices for ps2 */
#define SND_Cte_MaxSoftBufferNb		400				/* arbitrary, max number of soft sounds */

#define SND_Cte_MinVolume			0.0000001f
#define SND_Cte_MaxVolume			1.0f
#define SND_Cte_MinAtt				- 8000
#define SND_Cte_MaxAtt				0

#define SND_Cte_MaxBufferSizePS2	(2 * 16 * 3200) /* # 100 Ko, always 2 * 16 * N */
#define SND_Cte_MaxBufferSizePC		(300 * 1024)	/* # 3x(ps2 value) */
#define SND_Cte_MaxBufferSizeXENON	(12 * 36 * 1024) // Easily subdivisible in 2, 3, 4 or 6 buffers (compatible ADPCM)
#define SND_Cte_MaxBufferSizeXBOX	(2 * 36 * 2048)
#define SND_Cte_MaxBufferSizeGC		(SND_Cte_MaxBufferSizePS2)
#if defined(PSX2_TARGET)
#define SND_Cte_MaxBufferSize	(SND_Cte_MaxBufferSizePS2)
#elif defined(_GAMECUBE)
#define SND_Cte_MaxBufferSize	(SND_Cte_MaxBufferSizeGC)
#elif defined(_XENON)
#define SND_Cte_MaxBufferSize	(SND_Cte_MaxBufferSizeXENON)
#elif defined(_XBOX)
#define SND_Cte_MaxBufferSize	(SND_Cte_MaxBufferSizeXBOX)
#else
#define SND_Cte_MaxBufferSize	(SND_Cte_MaxBufferSizePC)
#endif
#if defined(_PC_RETAIL)
#define SND_Cte_MinBufferLenghtPC	(DSBSIZE_FX_MIN)
#define SND_Cte_MinBufferLenght		(SND_Cte_MinBufferLenghtPC)
#endif

/*$2- default values -------------------------------------------------------------------------------------------------*/

#define SND_Cte_DefaultGlobalVol	1.0f	/* default value for global volume (mdb) */
#define SND_Cte_DefaultGlobalPan	0		/* default value for global pan (0=centered) */
#define SND_Cte_DefaultNear			10.0f
#define SND_Cte_DefaultMiddle		45.0f
#define SND_Cte_DefaultFar			80.0f
#define SND_Cte_DefaultDeltaFar		2.0f

/*$2- format tags ----------------------------------------------------------------------------------------------------*/

#if !defined(_XBOX)
#define WAVE_FORMAT_XBOX_ADPCM	0x0069
#endif
#define WAVE_FORMAT_PS2			0xFFFF
#define WAVE_FORMAT_GAMECUBE	0xFFFE
#define WAVE_FORMAT_MSADPCM		0x0002
#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM			0x0001 // Already Defined in XAudio
#endif


#if defined( PSX2_TARGET )
#	define SND_Cte_DefaultWaveFormat WAVE_FORMAT_PS2
#elif defined( _GAMECUBE )
#	define SND_Cte_DefaultWaveFormat WAVE_FORMAT_GAMECUBE
#elif defined( _XENON )
#	define SND_Cte_DefaultWaveFormat WAVE_FORMAT_PCM
#elif defined( _XBOX )
#	define SND_Cte_DefaultWaveFormat WAVE_FORMAT_XBOX_ADPCM
#elif defined( _PC_RETAIL )
#	define SND_Cte_DefaultWaveFormat WAVE_FORMAT_MSADPCM
#elif defined( WIN32 )
#	define SND_Cte_DefaultWaveFormat WAVE_FORMAT_PCM
#elif defined( ACTIVE_EDITORS )
#	define SND_Cte_DefaultWaveFormat WAVE_FORMAT_XBOX_ADPCM
#else
#	error Unknown Platform.
#endif
#define SND_Cte_OneXboxFrameSize			36
#define SND_Cte_OneXboxFrameSampleNb		64
#define SND_Cte_OnePs2FrameSize				16
#define SND_Cte_OnePs2FrameSampleNb			28
#define SND_Cte_OneGamecubeFrameSize		8
#define SND_Cte_OneGamecubeFrameSampleNb	14

// hogsy: let's simplify this...
#define SND_M_IsGoodFormat( f ) ( ( ( f ) == SND_Cte_DefaultWaveFormat ) || ( ( f ) == WAVE_FORMAT_MSADPCM ) || ( ( f ) == WAVE_FORMAT_XBOX_ADPCM ) )

/*$2- Interface ID ---------------------------------------------------------------------------------------------------*/

#define ISound	0
#define ITrack	1
#define IStream 2
#define ISize	3

/*$2- for editors use ------------------------------------------------------------------------------------------------*/

#define SND_Cte_Type_Music			0x00000001
#define SND_Cte_Type_Ambience		0x00000002
#define SND_Cte_Type_Dialog			0x00000003
#define SND_Cte_Type_Sound			0x00000004
#define SND_Cte_Type_Bank			0x00000005
#define SND_Cte_Type_MetaBank		0x00000006
#define SND_Cte_Type_SModifier		0x00000007
#define SND_Cte_Type_LoadingSound	0x00000008

/*$4
 ***********************************************************************************************************************
    MACROS
 ***********************************************************************************************************************
 */

#define SND_M_VolumeSaturation(_f) \
	{ \
		if(_f < SND_Cte_MinVolume) \
			_f = 0.0f; \
		else if(_f > SND_Cte_MaxVolume) \
			_f = SND_Cte_MaxVolume; \
	}

#define SND_M_GetInstanceOrReturn(Instance, p, Return) \
	{ \
		if((Instance < 0) || (Instance >= SND_gst_Params.l_InstanceNumber)) return Return; \
		p = SND_gst_Params.dst_Instance + Instance; \
		if(!(p->ul_Flags & SND_Cul_DSF_Used)) return Return; \
	}

#define SND_M_GetSoundOrReturn(snd, p, Return) \
	{ \
		if((snd < 0) || (snd >= SND_gst_Params.l_SoundNumber)) return Return; \
		p = SND_gst_Params.dst_Sound + snd; \
		if(!(p->ul_Flags & SND_Cul_DSF_Used)) return Return; \
	}

#define SND_M_GetInsertOrReturn(Instance, p, Return) \
	{ \
		if((Instance < 0) || (Instance >= SND_gst_Params.l_InsertNumber)) return Return; \
		p = SND_gst_Params.dst_Insert + Instance; \
		if(!(p->ul_Flags & SND_Cul_DSF_Used)) return Return; \
	}

extern struct SND_tdst_Parameters_	SND_gst_Params;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ LONG SND_M_GetInstanceFromPtr(struct SND_tdst_SoundInstance_ *ptr)
{
	/*~~~~~~*/
	LONG	l;
	/*~~~~~~*/

	l = (((int) ptr - (int) SND_gst_Params.dst_Instance) / sizeof(struct SND_tdst_SoundInstance_));
	if(l < 0) return -1;
	if(l >= SND_gst_Params.l_InstanceNumber) return -1;
	return l;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ LONG SND_M_GetInsertFromPtr(SND_tdst_Insert *ptr)
{
	/*~~~~~~*/
	LONG	l;
	/*~~~~~~*/

	l = (((LONG) ptr - (LONG) SND_gst_Params.dst_Insert) / sizeof(SND_tdst_Insert));
	if(l < 0) return -1;
	if(l >= SND_gst_Params.l_InsertNumber) return -1;
	return l;
}

#define SND_M_GoodPointer(_p)	(_p != NULL)

/*$4
 ***********************************************************************************************************************
    global variables
 ***********************************************************************************************************************
 */

extern char SND_gc_NoSound;

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDCONST_H */
