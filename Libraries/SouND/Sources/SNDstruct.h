/*$T SNDstruct.h GC 1.138 01/14/05 15:07:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDSTRUCT_H__
#define __SNDSTRUCT_H__

/*$4
 ***********************************************************************************************************************
    HEADERS
 ***********************************************************************************************************************
 */

#if defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL) || defined(_PC_RETAIL) || defined(_JADEFUSION)
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include <dsound.h>
#endif
/**/
#if defined(_XBOX)
#include <dsound.h>
#endif
/**/
#ifdef PSX2_IOP
#include "iop/iopBAStypes.h"
#else
#include "BIGfiles/BIGkey.h"
#include "MATHs/MATH.h"
#include "BASe/BAStypes.h"
#endif
/**/
#ifdef _PC_RETAIL
#include "SouND/Sources/pcretail/pcretailSNDfx.h"
#endif
/**/
#include "SouND/Sources/SNDinsert.h"
/**/
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    WIN32-PC
 ***********************************************************************************************************************
 */

#if defined(PCWIN_TOOL) || defined(ACTIVE_EDITORS) || defined(_PC_RETAIL)
/**/
#if defined(ACTIVE_EDITORS)

/*$2- EDITORS --------------------------------------------------------------------------------------------------------*/

typedef struct	SND_tdst_SoundBuffer_
{
	IDirectSoundBuffer8				*pst_DSB;
	struct ediSND_tdst_SoundStream_ *pst_SS;
	int								i_Channel;
	int								i_Format;
	int								i_PanIdx;
	unsigned int					ui_LastStatus;
	LONG							l_Volume;
	LONG							l_Pan;
	LONG							l_Frequency;
	void							*pst_Fx;
	int								i_FxVol;
	int								i_HasFx;
} SND_tdst_SoundBuffer;

typedef struct	SND_tdst_TargetSpecificData_
{
	HWND				h_Wnd;
	IDirectSound8		*pst_DS;
	IDirectSoundBuffer8 *pst_PrimaryDSB;
} SND_tdst_TargetSpecificData;

#elif defined(_PC_RETAIL)

/*$2- _PC_RETAIL -----------------------------------------------------------------------------------------------------*/

typedef struct	SND_tdst_SoundBuffer_
{
	IDirectSoundBuffer8					*pst_DSB;
	struct win32SND_tdst_SoundStream_	*pst_SS;
	int									i_Channel;
	int									i_PanIdx;
	SND_tdst_DirectSoundFX				*pst_DSFX;
	int									hasFx;
	void								*pNextDuplicatedBuffer;
} SND_tdst_SoundBuffer;
typedef struct	SND_tdst_TargetSpecificData_
{
	HWND						h_Wnd;
	IDirectSound8				*pst_DS;
	IDirectSoundBuffer8			*pst_PrimaryDSB;	/* Pointer to direct primary sound buffer. */
	bool						hasFx;				/* Tell if the SoundBuffer to be created is with Fx or not */
	struct SND_tdst_OneSound_	*pOneSound;			/* This pointer is used to hold the original OneSound object for
													 * duplication pourposes */
} SND_tdst_TargetSpecificData;
/**/
#elif defined(PCWIN_TOOL)

/*$2------------------------------------------------------------------------------------------------------------------*/

typedef struct	SND_tdst_SoundBuffer_
{
	IDirectSoundBuffer8					*pst_DSB;
	struct win32SND_tdst_SoundStream_	*pst_SS;
	int									i_Channel;
	int									i_PanIdx;
	unsigned int						ui_LastStatus;
	LONG								l_Volume;
	LONG								l_Pan;
	LONG								l_Frequency;
	void								*pst_Fx;
	int									i_FxVol;
	int									i_HasFx;
} SND_tdst_SoundBuffer;

typedef struct	SND_tdst_TargetSpecificData_
{
	HWND				h_Wnd;
	IDirectSound8		*pst_DS;
	IDirectSoundBuffer8 *pst_PrimaryDSB;
} SND_tdst_TargetSpecificData;

/*$2- DEBUG/RELEASE --------------------------------------------------------------------------------------------------*/

#endif
/*$4
 ***********************************************************************************************************************
    XENON
 ***********************************************************************************************************************
 */

#elif defined(_XENON)

class CVoice;

typedef struct	SND_tdst_SoundBuffer_
{
	CVoice* pVoice;
}
SND_tdst_SoundBuffer;

typedef struct	SND_tdst_TargetSpecificData_
{
}
SND_tdst_TargetSpecificData;
/*$4
 ***********************************************************************************************************************
    XBOX
 ***********************************************************************************************************************
 */

#elif defined(_XBOX)
typedef struct	SND_tdst_SoundBuffer_
{
	IDirectSoundBuffer				*pst_DSB;
	void							*pv_RamBuffer;
	unsigned int					ui_BufferSize;

	struct xbSND_tdst_SoundStream_	*pst_SS;

	int								i_CurFrequency;
	int								i_CurPan;
	int								i_CurSPan;
	int								i_CurVol;
	int								i_CurFxVol;
	int								i_CurNoFxVol;
	int								i_MixBinNb;
	DSMIXBINVOLUMEPAIR				dst_MixBinVolumePairs[8];
	XBOXADPCMWAVEFORMAT				st_XBOXADPCMWAVEFORMAT;
} SND_tdst_SoundBuffer;
typedef struct	SND_tdst_TargetSpecificData_
{
	IDirectSound	*pst_DS;
} SND_tdst_TargetSpecificData;

/*$4
 ***********************************************************************************************************************
    PLAYSTATION2
 ***********************************************************************************************************************
 */

#elif defined(PSX2_TARGET)
typedef struct	SND_tdst_SoundBuffer_
{
	int dummy;
} SND_tdst_SoundBuffer;
typedef struct	SND_tdst_TargetSpecificData_
{
	int dummy;
} SND_tdst_TargetSpecificData;

/*$4
 ***********************************************************************************************************************
    GAMECUBE
 ***********************************************************************************************************************
 */

#elif defined(_GAMECUBE)
typedef struct	SND_tdst_SoundBuffer_
{
	int dummy;
} SND_tdst_SoundBuffer;

typedef struct	SND_tdst_TargetSpecificData_
{
	int dummy;
} SND_tdst_TargetSpecificData;

/*$4
 ***********************************************************************************************************************
    UNKNOWN TARGET
 ***********************************************************************************************************************
 */

#else
#pragma message(__FILE__ ">> error : Target is unknown <<")
#endif

/*$4
 ***********************************************************************************************************************
    COMMON DEFINITION
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    One Sound
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_OneSound_
{
	BIG_KEY							ul_FileKey;
	ULONG							ul_Flags;
	ULONG							ul_ExtFlags;
	ULONG							ul_CptUsed;
	SND_tdst_SoundBuffer			*pst_DSB;
	struct SND_tdst_WaveDesc_		*pst_Wave;
	struct SND_tdst_LowInterface_	*pst_LI;
	struct SND_tdst_SModifier_		*pst_SModifier;
} SND_tdst_OneSound;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Instance
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_SoundInstance_
{
	LONG							l_Sound;
	ULONG							ul_Flags;
	ULONG							ul_ExtFlags;
	int								i_InstTrack;

	int								i_BaseFrequency;
	int								i_StartFrequency;
	float							f_Volume;
	int								i_Front;
	int								i_Pan;
	float							f_MinPan;

	float							af_Near[3];
	float							af_Far[3];
	float							f_DeltaFar;
	float							af_Middle[3];
	float							f_FarCoeff;
	float							f_MiddleCoeff;
	float							f_CylinderHeight;

	MATH_tdst_Matrix				*pst_GlobalMatrix;
	MATH_tdst_Vector				*pst_GlobalPos;
	MATH_tdst_Vector				st_UpdatedPos;
	MATH_tdst_Vector				st_LastUpdatedPos;

	SND_tdst_SoundBuffer			*pst_DSB;	/* Pointer to sound buffer. */

	float							f_PlayingStartDate;
	void							*p_GameObject;
	struct SND_tdst_LowInterface_	*pst_LI;
	int								i_GroupId;
	struct SND_tdst_SModifier_		*pst_SModifier;
	float							f_Delay;
	float							f_StartDate;
	float							f_Doppler;
	float							f_FxVolLeft;
	float							f_FxVolRight;
	int								i_LoopNb;

#ifdef ACTIVE_EDITORS
	ULONG							ul_EdiFlags;
	ULONG							ul_SmdKey;
#endif
} SND_tdst_SoundInstance;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Interface definition
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_LowInterface_
{
	int (*pfl_Init) (SND_tdst_TargetSpecificData *);
	void (*pfv_Close) (SND_tdst_TargetSpecificData *);
	/**/
	SND_tdst_SoundBuffer * (*pfp_SndBuffCreate)
		(
			SND_tdst_TargetSpecificData *,
			struct SND_tdst_WaveData_ *,
			unsigned int
		);
	SND_tdst_SoundBuffer * (*pfp_SndBuffReLoad)
		(
			SND_tdst_TargetSpecificData *,
			SND_tdst_SoundBuffer *,
			struct SND_tdst_WaveData_ *,
			unsigned int
		);
	int (*pfi_SndBuffPlay) (struct SND_tdst_SoundBuffer_ *, int, int, int);
	void (*pfv_SndBuffStop) (struct SND_tdst_SoundBuffer_ *);
	void (*pfv_SndBuffPause) (struct SND_tdst_SoundBuffer_ *);
	void (*pfv_SndBuffRelease) (struct SND_tdst_SoundBuffer_ *);
	void (*pfv_SndBuffDuplicate) (SND_tdst_TargetSpecificData *, SND_tdst_SoundBuffer *, SND_tdst_SoundBuffer **);
	void (*pfv_SndBuffSetPos) (struct SND_tdst_SoundBuffer_ *, int);
	void (*pfv_SndBuffSetVol) (struct SND_tdst_SoundBuffer_ *, int);
	void (*pfv_SndBuffSetFreq) (struct SND_tdst_SoundBuffer_ *, int);
	void (*pfv_SndBuffSetPan) (struct SND_tdst_SoundBuffer_ *, int, int);
	int (*pfi_SndBuffGetStatus) (struct SND_tdst_SoundBuffer_ *, int *);
	void (*pfv_SndBuffGetFreq) (struct SND_tdst_SoundBuffer_ *, int *);
	void (*pfv_SndBuffGetPan) (struct SND_tdst_SoundBuffer_ *, int *, int *);
	void (*pfv_SndBuffGetPos) (struct SND_tdst_SoundBuffer_ *, int *, int *);
	/**/
	void (*pfv_SndBuffCreateTrack) (struct SND_tdst_SoundBuffer_ **, struct SND_tdst_SoundBuffer_ **);
	void (*pfv_SndBuffReleaseTrack) (struct SND_tdst_SoundBuffer_ *, struct SND_tdst_SoundBuffer_ *);
	void (*pfv_StreamPrefetch) (unsigned int, unsigned int);
	void (*pfv_StreamPrefetchArray) (unsigned int *, unsigned int *);
	void (*pfv_StreamFlush) (unsigned int);
	void (*pfv_StreamGetPrefetchStatus) (unsigned int *);
	void (*pfv_StreamReinitAndPlay)
		(
			struct SND_tdst_SoundBuffer_ *, /* SB */
			int,							/* flag */
		    int,                            /* format tag */
			int,							/* loop nb */
			unsigned int,					/* DataSize, */
			unsigned int,					/* DataPosition, */
			unsigned int,					/* LoopBeginOffset, */
			unsigned int,					/* LoopEndOffset, */
			int,							/* StartPos, */
			int,							/* StopPos, */
			int,							/* BaseFrequency, */
			int,							/* Pan, */
			int,							/* volume */
			int,							/* fx left vol */
			int								/* fx right vol */
		);
	void (*pfv_StreamChain)
		(
			struct SND_tdst_SoundBuffer_ *, /* SB */
			int,			/* flag */
			int,			/* format tag */
			int,			/* loop number */
			unsigned int,	/* exit point (oct) */
			unsigned int,	/* StartOffset (oct) */
			unsigned int,	/* DataSize (oct) */
			unsigned int,	/* DataPosition (oct) */
			unsigned int,	/* LoopBeginOffset (oct) */
			unsigned int	/* LoopEndOffset (oct) */
		);
	void (*pfv_StreamLoopCountGet) (struct SND_tdst_SoundBuffer_ *, int *);
	void (*pfv_StreamChainDelayGet) (struct SND_tdst_SoundBuffer_ *, float *);
	void (*pfv_StreamShutDownAll) (void);
	/**/
	void (*pfv_GlobalSetVol) (LONG);
	LONG (*pfl_GlobalGetVol) (void);
	/**/
	void (*pfv_ComputeDoppler) (MATH_tdst_Vector *, MATH_tdst_Vector *, SND_tdst_SoundInstance *);
	float (*pff_Compute3DVol) (SND_tdst_SoundInstance *);
	void (*pfv_CommitChange) (struct MATH_tdst_Vector_ *);
	/**/
	int (*pfi_ChangeRenderMode) (int);
	/**/
	int (*pfi_FxInit) (void);
	void (*pfv_FxClose) (void);
	int (*pfi_FxSetMode) (int, int);
	int (*pfi_FxSetDelay) (int, int);
	int (*pfi_FxSetFeedback) (int, int);
	int (*pfi_FxSetWetVolume) (int, int);
	int (*pfi_FxSetWetPan) (int, int);
	int (*pfi_FxEnable) (int);
	int (*pfi_FxDisable) (int);
}
SND_tdst_LowInterface;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    main struct for GAO
 -----------------------------------------------------------------------------------------------------------------------
 */

#define SND_Cte_Bank		0x00000001
#define SND_Cte_Metabank	0x00000002
typedef struct	SND_tdst_Bank_
{
	ULONG	ul_Flags;
	ULONG	ul_UserCount;
	ULONG	ul_FileKey;

	int		i_SoundNb;
	int		*pi_Bank;
	int		*pi_SaveBank;
} SND_tdst_Bank;
typedef struct	SND_tdst_Metabank_
{
	ULONG			ul_Flags;
	ULONG			ul_UserCount;
	ULONG			ul_FileKey;

	ULONG			*dst_BankKey;
	SND_tdst_Bank	*pst_Curr;
} SND_tdst_Metabank;
/**/
typedef union	SND_tdun_Main_
{
	SND_tdst_Bank		st_Bank;
	SND_tdst_Metabank	st_Metabank;
} SND_tdun_Main;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Parameters
 -----------------------------------------------------------------------------------------------------------------------
 */

#define SND_Cte_SoundNbMax	2000
#define SND_Cte_InsertNbMax	100

typedef struct	SND_tdst_Parameters_
{
	ULONG						ul_Flags;
	LONG						l_Available;
	SND_tdst_TargetSpecificData *pst_SpecificD;		/* pointer to target (win32/ps2) specific data */
	LONG						l_SoundNumber;
	LONG						l_InstanceNumber;
	LONG						l_MaxInstanceNumber;
	SND_tdst_SoundInstance		*dst_Instance;
	struct MATH_tdst_Matrix_	*pst_RefForPan;		/* camera */
	struct MATH_tdst_Matrix_	*pst_RefForVol;		/* main actor */
	struct MATH_tdst_Matrix_	*pst_SaveRefForPan; /* camera */
	struct MATH_tdst_Matrix_	*pst_SaveRefForVol; /* main actor */
	struct MATH_tdst_Matrix_	st_FreezeRefForPan; /* frozen camera */
	struct MATH_tdst_Matrix_	st_FreezeRefForVol; /* frozen main actor */
	float						f_RolloffFactor;
	float						f_DopplerFactor;
	float						f_SurroundFactor;
	float						f_DopplerSoundSpeed;
	ULONG						ul_RenderModeCapacity;
	ULONG						ul_RenderMode;
	LONG						l_InsertNumber;
	SND_tdst_Metabank			**dp_Metabank;
	LONG						l_MetabankNumber;
	LONG						l_Unloaded;
#ifdef ACTIVE_EDITORS
	int							i_EdiCheckError;
	struct OBJ_tdst_GameObject_ *pst_EdiMicroGao;
#endif
	SND_tdst_OneSound			dst_Sound[SND_Cte_SoundNbMax];
	struct SND_tdst_Insert_		dst_Insert[SND_Cte_InsertNbMax];
} SND_tdst_Parameters;

/**/
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
#pragma pack(1)
#else
#pragma pack(push, 1)
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    RIFF chunk
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_RiffChunk_
{
	unsigned int	ui_RiffTag;
	unsigned int	ui_RiffSize;
} SND_tdst_RiffChunk;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    format chunk
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_FormatChunk_
{
	unsigned int	ui_FormatType;
	unsigned int	ui_FormatTag;
	unsigned int	ui_FormatHeaderSize;
} SND_tdst_FormatChunk;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    data chunk
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef struct	SND_tdst_DataChunk_
{
	unsigned int	ui_DataTag;
	unsigned int	ui_DataSize;
} SND_tdst_DataChunk;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined(PSX2_TARGET) || defined(_GAMECUBE)
#pragma pack(1)
#else
#pragma pack(pop, 1)
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDSTRUCT_H__ */
