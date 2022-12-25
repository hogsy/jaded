/*$T SONutil.cpp GC 1.138 12/10/03 10:32:01 */


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

#ifndef JADEFUSION
#include "../Extern/DX8/include/dsound.h"
#endif
#include "DIAlogs/DIAorder_dlg.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"
#include "DIAlogs/DIAsnddisp_dlg.h"

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"

#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGgroup.h"
#include "INOut/INO.h"

#include "BASe/MEMory/MEM.h"

#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKmsg.h"

#include "SONutil.h"
#include "SONmsg.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDdebug.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDmusic.h"
#include "SouND/Sources/SNDambience.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDconv.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDmacros.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDconv_xboxadpcm.h"
#include "SouND/Sources/SNDconv_pcretailadpcm.h"
#include "SouND/Sources/SNDbank.h"
#include "SouND/Sources/SNDtrack.h"
#include "SONframe.h"
#include "SONview.h"

#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDImsg.h"
#include "VAVview/VAVlist.h"

#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORvars.h"

#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define SND_C_EdiBadKey			0x00000001
#define SND_C_EdiBadBits		0x00000002
#define SND_C_EdiBadChannel		0x00000004
#define SND_C_EdiBadSize		0x00000008
#define SND_C_EdiBadFormat		0x00000010
#define SND_C_EdiBadBlockAlign	0x00000020
#define SND_C_EdiBadAvg			0x00000040
#define SND_C_EdiBadType		0x00000080
#define SND_C_EdiWavType		0x00000100
#define SND_C_EdiWadType		0x00000200
#define SND_C_EdiWamType		0x00000400
#define SND_C_EdiSnkType		0x00000800
#define SND_C_EdiMskType		0x00001000
#define SND_C_EdiBadTooBig		0x00002000
#define SND_C_EdiBadLowerSize	0x00008000
#define SND_C_EdiBadBiggerSize	0x00010000
#define SND_C_EdiBadSizeForFx	0x00020000
#define SND_C_EdiWaaType		0x00040000
#define SND_C_EdiSmdType		0x00080000
#define SND_C_EdiGCFormat		0x00100000
#define SND_C_EdiWacType		0x00200000
#define SND_C_EdiBadLangNb		0x00400000
#define SND_C_EdiBadElemNb		0x00800000
#define SND_C_EdiBadContents	0x01000000
#define SND_C_EdiTruncated		0x02000000
#define SND_C_EdiSMdIsOld		0x04000000
#define SND_C_EdiXboxFormat		0x10000000
#define SND_C_EdiMSADPCMFormat	0x20000000
#define SND_C_EdiPS2Format		0x80000000

#define SND_C_EdiError \
		( \
			SND_C_EdiBadTooBig | \
				SND_C_EdiBadKey | \
				SND_C_EdiBadBits | \
				SND_C_EdiBadChannel | \
				SND_C_EdiBadSize | \
				SND_C_EdiBadFormat | \
				SND_C_EdiBadBlockAlign | \
				SND_C_EdiBadAvg | \
				SND_C_EdiBadType | \
				SND_C_EdiBadLowerSize | \
				SND_C_EdiBadBiggerSize | \
				SND_C_EdiBadSizeForFx | \
				SND_C_EdiBadLangNb | \
				SND_C_EdiBadContents | \
				SND_C_EdiBadElemNb | \
				SND_C_EdiTruncated | \
				SND_C_EdiSMdIsOld \
		)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef int (*SND_tdpfi_Encoder) (ULONG);


typedef struct	SND_tdst_WaveFormatExtended_
{
	WAVEFORMATEX		st_WAVEFORMATEX;
	SND_tdst_DataChunk	st_data;
} SND_tdst_WaveFormatExtended;

typedef struct	SND_tdst_WaveFormat_
{
	WAVEFORMAT			st_WAVEFORMAT;
	SND_tdst_DataChunk	st_data;
} SND_tdst_WaveFormat;

typedef struct	SND_tdst_PCMWaveFormat_
{
	PCMWAVEFORMAT		st_PCMWAVEFORMAT;
	SND_tdst_DataChunk	st_data;
} SND_tdst_PCMWaveFormat;

typedef union	SND_tdun_WaveSt_
{
	SND_tdst_WaveFormatExtended st_WaveExt;
	SND_tdst_WaveFormat			st_Wave;
	SND_tdst_PCMWaveFormat		st_PCMWave;
} SND_tdun_WaveSt;

typedef struct	SND_tdst_RiffHeader_
{
	SND_tdst_RiffChunk		st_RIFF;
	SND_tdst_FormatChunk	st_fmt;
	SND_tdun_WaveSt			un;
} SND_tdst_RiffHeader;

/*$4
 ***********************************************************************************************************************
    extern
 ***********************************************************************************************************************
 */

extern tdListItems	*EVAV_gpo_ListItems;

#ifdef JADEFUSION
extern BOOL		ENG_gb_AIRunning;
extern BOOL		EDI_gb_ComputeMap;
extern BOOL		EDI_gb_NoVerbose;
extern int		SND_RamRasterGetFree(void);
extern int		SND_RamRasterGetTotal(void);
#else
extern "C" BOOL		ENG_gb_AIRunning;
extern "C" BOOL		EDI_gb_ComputeMap;
extern "C" BOOL		EDI_gb_NoVerbose;
extern "C" int		SND_RamRasterGetFree(void);
extern "C" int		SND_RamRasterGetTotal(void);
#endif

#ifdef JADEFUSION
extern BOOL		LOA_gb_SpeedMode;
#endif

/*$4
 ***********************************************************************************************************************
    C++ fonctions, but not member of sound editor (ie only ACTIVE_EDITORS)
 ***********************************************************************************************************************
 */


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_OrderMdF(MDF_tdst_Modifier *_p_Modifier)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_OrderDialog		o_Dlg("Order Sound MdF", TRUE, FALSE);
	unsigned int			ui_Num;
	MDF_tdst_Modifier		*pst_MdF, *pst_Prev, **ppst_First, *pst_LastNonSnd;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	ULONG					ul_Fat;
	POSITION				pos, pos1;
	CString					o_Str;
	CList<ULONG, ULONG>		o_MdFList, o_SndMdFList;
	OBJ_tdst_GameObject		*pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ui_Num = 0;
	pst_MdF = (MDF_tdst_Modifier *) _p_Modifier;
	pst_GAO = pst_MdF->pst_GO;
	ppst_First = &pst_GAO->pst_Extended->pst_Modifiers;

	/* parse all modifiers, and group all sound modifiers */
	while(pst_MdF)
	{
		if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			o_SndMdFList.AddTail((ULONG) pst_MdF);
		else
			o_MdFList.AddTail((ULONG) pst_MdF);
		pst_MdF = pst_MdF->pst_Next;
	}

	if(o_SndMdFList.GetCount() == 0) return;

	/* put first all non-sound modifiers */
	pos = o_MdFList.GetHeadPosition();
	if(pos)
	{
		pst_MdF = (MDF_tdst_Modifier *) o_MdFList.GetNext(pos);
		*ppst_First = pst_MdF;
		pst_MdF->pst_Next = NULL;
		pst_MdF->pst_Prev = NULL;
		pst_Prev = pst_MdF;
		pst_LastNonSnd = pst_MdF;

		while(pos)
		{
			pst_MdF = (MDF_tdst_Modifier *) o_MdFList.GetNext(pos);
			pst_Prev->pst_Next = pst_MdF;
			pst_MdF->pst_Prev = pst_Prev;
			pst_MdF->pst_Next = NULL;
			pst_Prev = pst_MdF;
			pst_LastNonSnd = pst_MdF;
		}
	}
	else
	{
		pst_Prev = NULL;
		pst_LastNonSnd = NULL;
	}

	/* put then all sound modifiers */
	pos = o_SndMdFList.GetHeadPosition();
	pst_MdF = (MDF_tdst_Modifier *) o_SndMdFList.GetNext(pos);
	if(!pst_Prev)
	{
		*ppst_First = pst_MdF;
		pst_MdF->pst_Next = NULL;
		pst_MdF->pst_Prev = NULL;
		pst_Prev = *ppst_First;
	}
	else
	{
		pst_Prev->pst_Next = pst_MdF;
		pst_MdF->pst_Prev = pst_Prev;
		pst_MdF->pst_Next = NULL;
		pst_Prev = pst_MdF;
	}

	while(pos)
	{
		pst_MdF = (MDF_tdst_Modifier *) o_MdFList.GetNext(pos);
		pst_Prev->pst_Next = pst_MdF;
		pst_MdF->pst_Prev = pst_Prev;
		pst_MdF->pst_Next = NULL;
		pst_Prev = pst_MdF;
	}

	pst_MdF = *ppst_First;
	while(pst_MdF)
	{
		if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
		{
			pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;

			/* if default value */
			if(pst_SndMdF->ui_Id == -1) pst_SndMdF->ui_Id = ui_Num;

			/* holes */
			for(; ui_Num < pst_SndMdF->ui_Id; ui_Num++)
			{
				o_Dlg.AddItem("", 0);
			}

			/* put ref in list */
			ul_Fat = BIG_ul_SearchKeyToFat(pst_SndMdF->ui_FileKey);
			if(ul_Fat == BIG_C_InvalidIndex)
				o_Dlg.AddItem("<Bad Key>", (ULONG) pst_MdF);
			else
				o_Dlg.AddItem(BIG_NameFile(ul_Fat), (ULONG) pst_MdF);

			ui_Num++;
		}

		pst_MdF = pst_MdF->pst_Next;
	}

	o_Dlg.DoModal();

	/* get the result */
	pos = o_Dlg.mo_StrList.GetHeadPosition();
	pos1 = o_Dlg.mo_DataList.GetHeadPosition();
	ui_Num = 0;

	do
	{
		o_Str = o_Dlg.mo_StrList.GetNext(pos);
		pst_MdF = (MDF_tdst_Modifier *) o_Dlg.mo_DataList.GetNext(pos1);
		if(!o_Str.IsEmpty())
		{
			if(pst_LastNonSnd)
			{
				pst_LastNonSnd->pst_Next = pst_MdF;
				pst_MdF->pst_Prev = pst_LastNonSnd;
			}
			else
			{
				*ppst_First = pst_MdF;
				pst_MdF->pst_Prev = NULL;
			}

			pst_MdF->pst_Next = NULL;
			pst_Prev = pst_MdF;
			pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
			pst_SndMdF->ui_Id = ui_Num++;
			break;
		}

		ui_Num++;
	} while(pos);

	while(pos)
	{
		o_Str = o_Dlg.mo_StrList.GetNext(pos);
		pst_MdF = (MDF_tdst_Modifier *) o_Dlg.mo_DataList.GetNext(pos1);
		if(!o_Str.IsEmpty())
		{
			pst_MdF->pst_Prev = pst_Prev;
			pst_Prev->pst_Next = pst_MdF;
			pst_MdF->pst_Next = NULL;
			pst_Prev = pst_MdF;

			pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
			pst_SndMdF->ui_Id = ui_Num;
		}

		ui_Num++;
	}
}

/*$4
 ***********************************************************************************************************************
    C code for editor
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    C variables
 -----------------------------------------------------------------------------------------------------------------------
 */

BOOL			SND_gb_EdiForceKill;	/* F5, F6, F10... manager */
BOOL			SND_gb_EdiPause;

SND_tdun_Main	**SND_gap_MainRefList = NULL;
ULONG			SND_gul_MainRefListSize = 0;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    C prototypes
 -----------------------------------------------------------------------------------------------------------------------
 */

int			SND_i_EdiCheckWaveStr(SND_tdst_WaveDesc *, int _i_type, ULONG l);
void		SND_EdiDisplayError(ULONG _key, ULONG error);
BOOL		ESON_b_ImportCallBack(char *, char *, char *, int _i_type);

int			ESON_i_CheckMetaBank(ULONG _ul_Pos);
int			ESON_i_CheckBank(ULONG);
int			ESON_i_CheckSmd(ULONG _ul_FilePos, ULONG _ul_SmdFileKey);


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    C functions
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_ReinitOneWorld(void)
{
	SND_TrackModulateFrequency(1.0f);  
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_BankRegister(SND_tdun_Main *pMain)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	for(i = 0; i < SND_gul_MainRefListSize; i++)
	{
		if(SND_gap_MainRefList[i] == pMain) return;
	}

	if(SND_gul_MainRefListSize)
	{
		SND_gap_MainRefList = (SND_tdun_Main **) L_realloc
			(
				SND_gap_MainRefList,
				(SND_gul_MainRefListSize + 1) * sizeof(SND_tdun_Main *)
			);
		L_memset(&SND_gap_MainRefList[SND_gul_MainRefListSize], 0, sizeof(SND_tdun_Main *));
		SND_gul_MainRefListSize += 1;
	}
	else
	{
		SND_gap_MainRefList = (SND_tdun_Main **) L_malloc(sizeof(SND_tdun_Main *));
		L_memset(SND_gap_MainRefList, 0, sizeof(SND_tdun_Main *));
		SND_gul_MainRefListSize = 1;
	}

	SND_gap_MainRefList[i] = pMain;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_BankDelete(SND_tdun_Main *pMain)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	for(i = 0; i < SND_gul_MainRefListSize; i++)
	{
		if(SND_gap_MainRefList[i] == pMain) break;
	}

	SND_gul_MainRefListSize--;

	if(SND_gul_MainRefListSize)
	{
		for(; i < SND_gul_MainRefListSize; i++)
		{
			SND_gap_MainRefList[i] = SND_gap_MainRefList[i + 1];
		}

		SND_gap_MainRefList = (SND_tdun_Main **) L_realloc
			(
				SND_gap_MainRefList,
				SND_gul_MainRefListSize * sizeof(SND_tdun_Main *)
			);
	}
	else
	{
		L_free(SND_gap_MainRefList);
		SND_gap_MainRefList = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_EdiCheckSoundFile(ULONG _ul_Key, int _i_type, ULONG _ul_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					status;
	SND_tdst_WaveDesc	*pWave;
	ULONG				_ul_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	status = 0;
	_ul_Pos = BIG_ul_SearchKeyToPos(_ul_Key);
	if(_ul_Pos == -1) return -1;
	_ul_Pos += 4;

	switch(_i_type)
	{
	case SND_Cte_Type_Music:
	case SND_Cte_Type_Ambience:
	case SND_Cte_Type_Dialog:
	case SND_Cte_Type_Sound:
	case SND_Cte_Type_LoadingSound:
		if(_ul_Size == 8)
		{
			status = SND_C_EdiTruncated;
		}
		else
		{
			LOA_MakeFileRef(_ul_Key, (ULONG *) &pWave, SND_ul_WaveCallback, LOA_C_MustExists | LOA_C_HasUserCounter);
			LOA_Resolve();
            ESON_LoadAllSounds();

			if(pWave == NULL)
				return SND_C_EdiBadFormat;
			else
				status = 0;

			status |= SND_i_EdiCheckWaveStr(pWave, _i_type, _ul_Size);

			SND_WaveUnload(pWave);
		}
		break;

	case SND_Cte_Type_MetaBank:
		status = ESON_i_CheckMetaBank(_ul_Pos);
		break;

	case SND_Cte_Type_Bank:
		status = ESON_i_CheckBank(_ul_Pos);
		break;

	case SND_Cte_Type_SModifier:
		status = ESON_i_CheckSmd(_ul_Pos-4, _ul_Key);
		break;

	default:
		status = SND_C_EdiBadFormat;
		break;
	}

	return status;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_EdiCheckWaveStr(SND_tdst_WaveDesc *pWave, int _i_type, ULONG _ul_Size)
{
	/*~~~~~~~~~~~~~*/
	int		status;
	float	duration;
	/*~~~~~~~~~~~~~*/

	status = 0;

	if((_i_type == SND_Cte_Type_Sound)||(_i_type == SND_Cte_Type_LoadingSound))
	{
		char log[512];
		sprintf(log, "wav contains loop definition [%x]", pWave->ul_FileKey);
		ERR_X_Warning(!pWave->b_HasLoop, log, NULL);
	}

	/*$2- null file --------------------------------------------------------------------------------------------------*/

	if(pWave->ul_DataSize == 0) status |= SND_C_EdiBadSize;

	/*$2- PC format --------------------------------------------------------------------------------------------------*/

	if(pWave->wFormatTag == WAVE_FORMAT_PCM)
	{
		/* must be 16 bits for convertion */
		if(pWave->wBitsPerSample != 16) status |= SND_C_EdiBadBits;

		/* field consistency */
		if(pWave->wBlockAlign != (pWave->wChannels * pWave->wBitsPerSample / 8)) status |= SND_C_EdiBadBlockAlign;

		/* DirectSound Lib conditions */
		if(pWave->ul_DataSize < DSBSIZE_MIN) status |= SND_C_EdiBadLowerSize;
		if(pWave->ul_DataSize > DSBSIZE_MAX) status |= SND_C_EdiBadBiggerSize;

		duration = (float) pWave->ul_DataSize / (float) (pWave->wBlockAlign * pWave->dwSamplesPerSec);

		if(duration < 0.080f) status |= SND_C_EdiBadLowerSize;

		if(_i_type == SND_Cte_Type_LoadingSound)
			if(pWave->ul_DataSize >= SND_Cte_LoadingSoundMaxSize) status |= SND_C_EdiBadTooBig;
	}

	/*$2- M ----------------------------------------------------------------------------------------------------------*/

	else if(pWave->wFormatTag == WAVE_FORMAT_MSADPCM)
	{
		if(pWave->ul_DataSize < 64)
		{
			status |= SND_C_EdiBadLowerSize;
		}

		if(_ul_Size < (ULONG) (pWave->ul_DataSize + pWave->cbSize + sizeof(SND_tdst_RiffHeader)))
			status |= SND_C_EdiBadLowerSize;

		if(_ul_Size < 100)
		{
			status |= SND_C_EdiBadLowerSize;
		}

		status |= SND_C_EdiMSADPCMFormat;
	}

	/*$2- XBOX format ------------------------------------------------------------------------------------------------*/

	else if(pWave->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
	{
		if(pWave->ul_DataSize < 64)
		{
			status |= SND_C_EdiBadLowerSize;
		}

		if(_ul_Size < 100)
		{
			status |= SND_C_EdiBadLowerSize;
		}

		status |= SND_C_EdiXboxFormat;
	}

	/*$2- PS2 format -------------------------------------------------------------------------------------------------*/

	else if(pWave->wFormatTag == WAVE_FORMAT_PS2)
	{
		status |= SND_C_EdiPS2Format;
		if(pWave->ul_DataSize < 64) status |= SND_C_EdiBadLowerSize;
		if(_ul_Size < 100) status |= SND_C_EdiBadLowerSize;
		if(pWave->cbSize) status |= SND_C_EdiBadSize;
		if(pWave->wBitsPerSample != 4) status |= SND_C_EdiBadBits;
		if(pWave->wBlockAlign != 16) status |= SND_C_EdiBadBlockAlign;
		if(pWave->ul_DataSize % 16) status |= SND_C_EdiBadBlockAlign;
		if(pWave->ul_DataSize < 0x30) status |= SND_C_EdiBadLowerSize;
	}

	/*$2- GC format --------------------------------------------------------------------------------------------------*/

	else if(pWave->wFormatTag == WAVE_FORMAT_GAMECUBE)
	{
		status |= SND_C_EdiGCFormat;
		if(pWave->wBitsPerSample != 4) status |= SND_C_EdiBadBits;
		if(pWave->wBlockAlign != 8) status |= SND_C_EdiBadBlockAlign;
	}

	/*$2- else unknown format ----------------------------------------------------------------------------------------*/

	else
	{
		status |= SND_C_EdiBadFormat;
		return status;
	}

	/*$2- field consistency ------------------------------------------------------------------------------------------*/

	if((pWave->wFormatTag != WAVE_FORMAT_XBOX_ADPCM) && (pWave->wFormatTag != WAVE_FORMAT_MSADPCM))
	{
		if(pWave->dwAvgBytesPerSec != (pWave->dwSamplesPerSec * pWave->wChannels * pWave->wBitsPerSample / 8))
			status |= SND_C_EdiBadAvg;
	}

	/*$2- specific among file type -----------------------------------------------------------------------------------*/

	switch(_i_type)
	{
	/* music */
	case SND_Cte_Type_Music:		if(pWave->wChannels > 2) status |= SND_C_EdiBadChannel; break;
	/* ambience */
	case SND_Cte_Type_Ambience:		if(pWave->wChannels > 2) status |= SND_C_EdiBadChannel; break;
	/* dialog */
	case SND_Cte_Type_Dialog:		if(pWave->wChannels > 2) status |= SND_C_EdiBadChannel; break;
	/* sound */
	case SND_Cte_Type_Sound:		if(pWave->wChannels != 1) status |= SND_C_EdiBadChannel; break;
	case SND_Cte_Type_LoadingSound: if(pWave->wChannels != 1) status |= SND_C_EdiBadChannel; break;
	default:						status |= SND_C_EdiBadType; break;
	}

	return status;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SND_b_CheckSoundFiles(ULONG _ul_FileNb, ULONG *_pul_KeyList, ULONG *_pul_TypeList)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	i;
	ULONG	*pul_Res;
	ULONG	ul_Pos;
	ULONG	ul_Size;
	char	tmp[256];
	ULONG	ps2wavnb;
	ULONG	ps2wadnb;
	ULONG	ps2wamnb;
	ULONG	ps2waanb;
	ULONG	ps2wacnb;
	ULONG	gcwavnb;
	ULONG	gcwadnb;
	ULONG	gcwamnb;
	ULONG	gcwaanb;
	ULONG	gcwacnb;
	ULONG	wavnb;
	ULONG	wadnb;
	ULONG	wamnb;
	ULONG	waanb;
	ULONG	wacnb;
	ULONG	snknb;
	ULONG	msknb;
	ULONG	smdnb;
	ULONG	xbwavnb;
	ULONG	xbwadnb;
	ULONG	xbwamnb;
	ULONG	xbwaanb;
	ULONG	xbwacnb;
	ULONG	adpcmwavnb;
	ULONG	adpcmwadnb;
	ULONG	adpcmwamnb;
	ULONG	adpcmwaanb;
	ULONG	adpcmwacnb;
	/*~~~~~~~~~~~~~~~*/

	pul_Res = NULL;

	ps2wavnb = 0;
	ps2wadnb = 0;
	ps2wamnb = 0;
	ps2waanb = 0;
	ps2wacnb = 0;

	gcwavnb = 0;
	gcwaanb = 0;
	gcwadnb = 0;
	gcwamnb = 0;
	gcwacnb = 0;

	xbwavnb = 0;
	xbwaanb = 0;
	xbwadnb = 0;
	xbwamnb = 0;
	xbwacnb = 0;

	adpcmwavnb = 0;
	adpcmwaanb = 0;
	adpcmwadnb = 0;
	adpcmwamnb = 0;
	adpcmwacnb = 0;

	wavnb = 0;
	waanb = 0;
	wadnb = 0;
	wamnb = 0;
	wacnb = 0;

	snknb = 0;
	msknb = 0;
	smdnb = 0;

	if(_ul_FileNb)
	{
		pul_Res = (ULONG *) L_malloc(_ul_FileNb * sizeof(ULONG));
		L_memset(pul_Res, 0, _ul_FileNb * sizeof(ULONG));

		LINK_PrintStatusMsg("---------------------------------------------");
		LINK_PrintStatusMsg("Checking all sound files :");

		SND_gst_Params.i_EdiCheckError = 0;
		for(i = 0; i < _ul_FileNb; i++)
		{
#ifdef _DEBUG
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				static ULONG	ul_dbgkey = 0;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(ul_dbgkey == _pul_KeyList[i])
				{
					_pul_KeyList[i] = _pul_KeyList[i];
				}
			}
#endif
			if((_pul_KeyList[i] == 0) || (_pul_KeyList[i] == BIG_C_InvalidKey))
			{
				pul_Res[i] = SND_C_EdiBadKey;
				continue;
			}

			ul_Pos = BIG_ul_SearchKeyToPos(_pul_KeyList[i]);
			ul_Size = BIG_ul_GetLengthFile(ul_Pos);
			pul_Res[i] = SND_i_EdiCheckSoundFile(_pul_KeyList[i], _pul_TypeList[i], ul_Size);

			switch(_pul_TypeList[i])
			{
			case SND_Cte_Type_Music:
				wamnb++;
				pul_Res[i] |= SND_C_EdiWamType;
				if(pul_Res[i] & SND_C_EdiPS2Format) ps2wamnb++;
				if(pul_Res[i] & SND_C_EdiGCFormat) gcwamnb++;
				if(pul_Res[i] & SND_C_EdiXboxFormat) xbwamnb++;
				if(pul_Res[i] & SND_C_EdiMSADPCMFormat) adpcmwamnb++;
				break;

			case SND_Cte_Type_Dialog:
				wadnb++;
				pul_Res[i] |= SND_C_EdiWadType;
				if(pul_Res[i] & SND_C_EdiPS2Format) ps2wadnb++;
				if(pul_Res[i] & SND_C_EdiGCFormat) gcwadnb++;
				if(pul_Res[i] & SND_C_EdiXboxFormat) xbwadnb++;
				if(pul_Res[i] & SND_C_EdiMSADPCMFormat) adpcmwadnb++;
				break;

			case SND_Cte_Type_Ambience:
				waanb++;
				pul_Res[i] |= SND_C_EdiWaaType;
				if(pul_Res[i] & SND_C_EdiPS2Format) ps2waanb++;
				if(pul_Res[i] & SND_C_EdiGCFormat) gcwaanb++;
				if(pul_Res[i] & SND_C_EdiXboxFormat) xbwaanb++;
				if(pul_Res[i] & SND_C_EdiMSADPCMFormat) adpcmwaanb++;
				break;

			case SND_Cte_Type_LoadingSound:
				wacnb++;
				pul_Res[i] |= SND_C_EdiWacType;
				if(pul_Res[i] & SND_C_EdiPS2Format) ps2wacnb++;
				if(pul_Res[i] & SND_C_EdiGCFormat) gcwacnb++;
				if(pul_Res[i] & SND_C_EdiXboxFormat) xbwacnb++;
				if(pul_Res[i] & SND_C_EdiMSADPCMFormat) adpcmwacnb++;
				break;

			case SND_Cte_Type_Sound:
				wavnb++;
				pul_Res[i] |= SND_C_EdiWavType;
				if(pul_Res[i] & SND_C_EdiPS2Format) ps2wavnb++;
				if(pul_Res[i] & SND_C_EdiGCFormat) gcwavnb++;
				if(pul_Res[i] & SND_C_EdiXboxFormat) xbwavnb++;
				if(pul_Res[i] & SND_C_EdiMSADPCMFormat) adpcmwavnb++;
				break;

			case SND_Cte_Type_Bank:
				snknb++;
				pul_Res[i] |= SND_C_EdiSnkType;
				break;;

			case SND_Cte_Type_MetaBank:
				msknb++;
				pul_Res[i] |= SND_C_EdiMskType;
				break;

			case SND_Cte_Type_SModifier:
				smdnb++;
				pul_Res[i] |= SND_C_EdiSmdType;
				break;

			default:
				pul_Res[i] |= SND_C_EdiBadType;
				break;
			}

			SND_EdiDisplayError(_pul_KeyList[i], pul_Res[i]);
		}

		LINK_PrintStatusMsg("--------------------------");

		sprintf(tmp, "Meta bank (.msk) : %4d", msknb);
		LINK_PrintStatusMsg(tmp);

		sprintf(tmp, "Bank      (.snk) : %4d", snknb);
		LINK_PrintStatusMsg(tmp);

		sprintf(tmp, "SModifier (.smd) : %4d", smdnb);
		LINK_PrintStatusMsg(tmp);

		LINK_PrintStatusMsg("-------------------  All  PS2  GAMECUBE    XBOX  MS-ADPCM ----");
		sprintf(tmp, "Sound     (.wav) : %4d  %4d      %4d    %4d  %4d", wavnb, ps2wavnb, gcwavnb, xbwavnb, adpcmwavnb);
		LINK_PrintStatusMsg(tmp);

		sprintf(tmp, "Sound     (.wac) : %4d  %4d      %4d    %4d  %4d", wacnb, ps2wacnb, gcwacnb, xbwacnb, adpcmwacnb);
		LINK_PrintStatusMsg(tmp);

		sprintf(tmp, "Dialog    (.wad) : %4d  %4d      %4d    %4d  %4d", wadnb, ps2wadnb, gcwadnb, xbwadnb, adpcmwadnb);
		LINK_PrintStatusMsg(tmp);

		sprintf(tmp, "Music     (.wam) : %4d  %4d      %4d    %4d  %4d", wamnb, ps2wamnb, gcwamnb, xbwamnb, adpcmwamnb);
		LINK_PrintStatusMsg(tmp);

		sprintf(tmp, "Ambience  (.waa) : %4d  %4d      %4d    %4d  %4d", waanb, ps2waanb, gcwaanb, xbwaanb, adpcmwaanb);
		LINK_PrintStatusMsg(tmp);

		sprintf
		(
			tmp,
			"-- Total --------- %4d  %4d      %4d    %4d  %4d ----",
			wamnb + wavnb + waanb + wadnb + wacnb,
			ps2wamnb + ps2wavnb + ps2waanb + ps2wadnb + ps2wacnb,
			gcwamnb + gcwavnb + gcwaanb + gcwadnb + gcwacnb,
			xbwamnb + xbwavnb + xbwaanb + xbwadnb + xbwacnb,
			adpcmwamnb + adpcmwavnb + adpcmwaanb + adpcmwadnb + adpcmwacnb
		);
		LINK_PrintStatusMsg(tmp);
		LINK_PrintStatusMsg("--------------------------------------------------------------");
		LINK_PrintStatusMsg("Command executed.");

		L_free(pul_Res);
		if(SND_gst_Params.i_EdiCheckError)
			return FALSE;
		else
			return TRUE;
	}
	else
		return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_EdiDisplayError(ULONG _key, ULONG error)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*key[] = { "", "Key" };
	char	*bit[] = { "", "Bits" };
	char	*channel[] = { "", "Channel" };
	char	*size[] = { "", "Size" };
	char	*format[] = { "", "Format" };
	char	*block[] = { "", "Block" };
	char	*avg[] = { "", "Avg" };
	char	*type[] = { "", "Type" };
	char	*convert[] = { "", "(PS2)", "(gamecube)", "(xbox)", "(ms-adpcm)" };
	char	*gc[] = { "", "(gc)" };
	char	*toobig[] = { "", "TooBigForWav" };
	char	*lower[] = { "", "TooShort" };
	char	*bigger[] = { "", "TooLong" };
	char	*fxsize[] = { "", "TooShortForFx" };
	char	*msklangnb[] = { "", "BadLangNb" };
	char	*snksize[] = { "", "BadDialogNb" };
	char	*contents[] = { "", "BadContents" };
	char	*trunc[] = { "", "Truncated" };
	char	*smd[] = { "", "OldVersion" };
	int		b_key, b_bit, b_channel, b_format, b_block, b_avg, b_type, b_size, i_convert, b_toobig, b_lower, b_bigger;
	int		b_fxsize;
	int		b_msklangnb;
	int		b_snksize, b_contents;
	int		b_trunc, b_smd;
	char	tmp[512];
	ULONG	ulFat;
	char asz_Path[2*BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(error & SND_C_EdiError)
	{
		b_key = 0;
		b_bit = 0;
		b_channel = 0;
		b_format = 0;
		b_block = 0;
		b_avg = 0;
		b_type = 0;
		b_size = 0;
		i_convert = 0;
		b_toobig = 0;
		b_lower = 0;
		b_bigger = 0;
		b_fxsize = 0;
		b_msklangnb = 0;
		b_snksize = 0;
		b_contents = 0;
		b_trunc = 0;
		b_smd = 0;

		if(error & SND_C_EdiBadKey) b_key = 1;
		if(error & SND_C_EdiBadBits) b_bit = 1;
		if(error & SND_C_EdiBadChannel) b_channel = 1;
		if(error & SND_C_EdiBadSize) b_size = 1;
		if(error & SND_C_EdiBadFormat) b_format = 1;
		if(error & SND_C_EdiBadBlockAlign) b_block = 1;
		if(error & SND_C_EdiBadAvg) b_avg = 1;
		if(error & SND_C_EdiBadType) b_type = 1;
		if(error & SND_C_EdiPS2Format) i_convert = 1;
		if(error & SND_C_EdiGCFormat) i_convert = 2;
		if(error & SND_C_EdiXboxFormat) i_convert = 3;
		if(error & SND_C_EdiMSADPCMFormat) i_convert = 4;
		if(error & SND_C_EdiBadTooBig) b_toobig = 1;
		if(error & SND_C_EdiBadLowerSize) b_lower = 1;
		if(error & SND_C_EdiBadBiggerSize) b_bigger = 1;
		if(error & SND_C_EdiBadSizeForFx) b_fxsize = 1;
		if(error & SND_C_EdiBadLangNb) b_msklangnb = 1;
		if(error & SND_C_EdiBadElemNb) b_snksize = 1;
		if(error & SND_C_EdiBadContents) b_contents = 1;
		if(error & SND_C_EdiTruncated) b_trunc = 1;
		if(error & SND_C_EdiSMdIsOld) b_smd = 1;

		SND_gst_Params.i_EdiCheckError += b_key +
			b_bit +
			b_channel +
			b_format +
			b_block +
			b_avg +
			b_type +
			b_size +
			b_toobig +
			b_lower +
			b_bigger +
			b_fxsize +
			b_msklangnb +
			b_snksize +
			b_contents +
			b_trunc +
			b_smd;

		ulFat = BIG_ul_SearchKeyToFat(_key);
		if(ulFat != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(BIG_ParentFile(ulFat), asz_Path);
			L_strcat(asz_Path, "\\");
			L_strcat(asz_Path, BIG_NameFile(ulFat));
		}
		else
		{
			L_strcpy(asz_Path, "unknown file");
		}

		sprintf
		(
			tmp,
			"Bad file [%08X] : %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
			_key,
			key[b_key],
			bit[b_bit],
			channel[b_channel],
			size[b_size],
			format[b_format],
			block[b_block],
			avg[b_avg],
			type[b_type],
			toobig[b_toobig],
			lower[b_lower],
			bigger[b_bigger],
			fxsize[b_fxsize],
			convert[i_convert],
			msklangnb[b_msklangnb],
			snksize[b_snksize],
			contents[b_contents],
			trunc[b_trunc],
			smd[b_smd],
			asz_Path
		);
		ERR_X_Warning(0, tmp, NULL);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_ImportWACCallBack(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
	return ESON_b_ImportCallBack(_psz_BigPathName, _psz_BigFileName, _psz_ExternName, SND_Cte_Type_LoadingSound);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_ImportWAVCallBack(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
	return ESON_b_ImportCallBack(_psz_BigPathName, _psz_BigFileName, _psz_ExternName, SND_Cte_Type_Sound);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_ImportWADCallBack(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
	return ESON_b_ImportCallBack(_psz_BigPathName, _psz_BigFileName, _psz_ExternName, SND_Cte_Type_Dialog);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_ImportWAMCallBack(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
	return ESON_b_ImportCallBack(_psz_BigPathName, _psz_BigFileName, _psz_ExternName, SND_Cte_Type_Music);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_ImportWAACallBack(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName)
{
	return ESON_b_ImportCallBack(_psz_BigPathName, _psz_BigFileName, _psz_ExternName, SND_Cte_Type_Ambience);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_ImportCallBack(char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName, int _i_type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*pc_Buffer;
	ULONG		l_Size;
	ULONG		l_SizeRead;
	HANDLE		h_File;
	char		*sz_Ext;
	BOOL		b_Result;
    ULONG       ul_File;
    SND_tdst_WaveDesc	*pWave;
    SND_tdst_WaveDesc	*pWave2;
    SND_tdst_WaveDesc	stWave;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	h_File = CreateFile
		(
			_psz_ExternName,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
			NULL
		);

	if(h_File == INVALID_HANDLE_VALUE) return 0;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    read the file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	l_Size = GetFileSize(h_File, NULL);
	if(l_Size == 0xFFFFFFFF) return 0;
	b_Result = SetFilePointer(h_File, 0, 0, FILE_BEGIN);

	pc_Buffer = (char *) L_malloc(l_Size);
	if(pc_Buffer == NULL)
	{
		/*~~~~~~~~~~~~~*/
		char	az[1024];
		/*~~~~~~~~~~~~~*/

		sprintf(az, "There no enougth memory to import the file %s", _psz_BigFileName);
		MessageBox(NULL, az, "Sound File Import Failed !!", MB_OK | MB_ICONSTOP);
		ERR_X_Warning(0, az, NULL);
		CloseHandle(h_File);
		return 0;
	}

	b_Result = ReadFile(h_File, pc_Buffer, l_Size, &l_SizeRead, NULL);

	if(!b_Result)
	{
		/*~~~~~~~~~~~~~*/
		char	az[1024];
		/*~~~~~~~~~~~~~*/

		sprintf(az, "Error while reading the file %s", _psz_BigFileName);
		MessageBox(NULL, az, "Sound File Import Failed !!", MB_OK | MB_ICONSTOP);
		ERR_X_Warning(0, az, NULL);
		CloseHandle(h_File);
		return 0;
	}

	CloseHandle(h_File);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Put extension into lower case
	 -------------------------------------------------------------------------------------------------------------------
	 */

	sz_Ext = strrchr(_psz_BigFileName, '.');
	if(sz_Ext)
	{
		while(*(++sz_Ext)) *sz_Ext = tolower(*sz_Ext);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    save the file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	BIG_UpdateFileFromBuffer(_psz_BigPathName, _psz_BigFileName, pc_Buffer, l_Size);
	L_free(pc_Buffer);

    //$2 update object if yet loaded
	ul_File = BIG_ul_SearchDir(_psz_BigPathName);
	ul_File = BIG_ul_SearchFile(ul_File, _psz_BigFileName);
    pWave = (SND_tdst_WaveDesc*)LOA_ul_SearchAddress(BIG_PosFile(ul_File));
    if(*(int*)&pWave == -1) return 0;

    LOA_DeleteAddress(pWave);
	LOA_MakeFileRef
	(
		BIG_FileKey(ul_File),
		(ULONG *) &pWave2,
		SND_ul_WaveCallback,
		LOA_C_MustExists | LOA_C_HasUserCounter
	);
	LOA_Resolve();

    L_memcpy(&stWave, pWave, sizeof(SND_tdst_WaveDesc));    
    L_memcpy(pWave, pWave2, sizeof(SND_tdst_WaveDesc));    
    L_memcpy(pWave2, &stWave, sizeof(SND_tdst_WaveDesc));    
    SND_WaveUnload(pWave2);
    LOA_AddAddress(ul_File, pWave);

    //
    LONG l_Index = SND_l_GetSoundIndex(BIG_FileKey(ul_File));
    if(l_Index==-1) return 0;

    SND_tdst_OneSound*pst_Sound= SND_gst_Params.dst_Sound + l_Index;
    if((pst_Sound->ul_Flags & SND_Cul_OSF_Loaded)==0) return 0;
    if(pst_Sound->ul_Flags & SND_Cte_StreamedFile) return 0;

    SND_tdst_SoundInstance*pst_SI;
    for(int j=0; j<SND_gst_Params.l_InsertNumber; j++)
    {
        pst_SI = SND_gst_Params.dst_Instance + j;
        if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
        if(pst_SI->l_Sound != l_Index) continue;
        SND_FreeInstance(j);
    }
    
    pst_Sound->pst_LI->pfv_SndBuffStop(pst_Sound->pst_DSB);
	pst_Sound->pst_LI->pfv_SndBuffRelease(pst_Sound->pst_DSB);
    pst_Sound->ul_Flags &= ~SND_Cul_OSF_Loaded ;

    SND_l_LoadData(BIG_FileKey(ul_File));
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

/*
 =======================================================================================================================
    editor only, and only neeeded if "active engine world"
 =======================================================================================================================
 */
void ESON_KillAllSounds(void)
{
	/*~~*/
	int i;
	/*~~*/

	SND_gb_EdiPause = FALSE;
	SND_gb_EdiForceKill = TRUE;

	SND_StreamPrefetchFlushAll();
	SND_StopAll(0);
	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		SND_Release(i);
	}

	for(i = 0; i < SND_gst_Params.l_SoundNumber; i++)
	{
		SND_DeleteByIndex(i);
	}

	SND_gb_EdiForceKill = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_SetSpy(ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ul_Fat;
	char	log[1024];
	/*~~~~~~~~~~~~~~*/

	ul_Fat = BIG_ul_SearchKeyToFat(_ul_Key);

	SND_gst_Debug.b_EnableDebugLog = 1;

	if(ul_Fat == -1)
	{
		SND_gst_Debug.ul_SpyKey = -1;
		sprintf(log, "[SND-SPY] invalid key 0x%x", _ul_Key);
	}
	else
	{
		SND_gst_Debug.ul_SpyKey = _ul_Key;
		sprintf(log, "[SND-SPY] Set to file (0x%x)%s", SND_gst_Debug.ul_SpyKey, BIG_NameFile(ul_Fat));
	}

	LINK_PrintStatusMsg(log);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_ResetSpy(void)
{
	SND_gst_Debug.ul_SpyKey = -1;
	LINK_PrintStatusMsg("[SND-SPY] reset.");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_PauseAll(BOOL _b_ForcePause)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						l_Index, l_Status;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	for(l_Index = 0; l_Index < SND_gst_Params.l_InstanceNumber; l_Index++)
	{
		pst_SI = &SND_gst_Params.dst_Instance[l_Index];
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;

		if(!SND_gb_EdiPause || _b_ForcePause)
		{
			pst_SI->pst_LI->pfi_SndBuffGetStatus(pst_SI->pst_DSB, &l_Status);
			if(l_Status & SND_Cul_SBS_Playing)
			{
				if(pst_SI->ul_Flags & SND_Cul_SF_EdiPause) continue;

				pst_SI->pst_LI->pfv_SndBuffPause(pst_SI->pst_DSB);
				pst_SI->ul_Flags |= SND_Cul_SF_EdiPause;
			}
		}
		else
		{
			if(pst_SI->ul_Flags & SND_Cul_SF_EdiPause)
			{
				pst_SI->pst_LI->pfv_SndBuffPause(pst_SI->pst_DSB);
				pst_SI->ul_Flags &= ~SND_Cul_SF_EdiPause;
			}
		}
	}

	if(!SND_gb_EdiPause || _b_ForcePause)
		SND_gb_EdiPause = TRUE;
	else
		SND_gb_EdiPause = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_BankReport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	int					idx;
	SND_tdun_Main		*pst_MainStruct;
	SND_tdst_Bank		*pBank;
	SND_tdst_OneSound	*pst_Sound;
	int					i_FileSize;
	int					i_BankSize;
	char				az_msg[1024];
	ULONG				ul_Fat;
	ULONG				l_Sound;
	int					i_Stream;
	SND_tdst_SModifierExtPlayer *pst_ExtPlay;
	unsigned short		us;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg("---------------------------------------------");
	LINK_PrintStatusMsg("Sound bank report (only wav size) :");
	LINK_PrintStatusMsg("-----------------------------------");

	for(i = 0; i < (int) SND_gul_MainRefListSize; i++)
	{
		i_BankSize = 0;
		i_Stream = 0;

		pst_MainStruct = SND_gap_MainRefList[i];
		pBank = SND_p_MainGetBank(pst_MainStruct);
		ul_Fat = BIG_ul_SearchKeyToFat(pBank->ul_FileKey);

		for(idx = 0; idx < pBank->i_SoundNb; idx++)
		{
			/* check bank elem */
			if(pBank->pi_Bank[idx] < 0) continue;
			if(pBank->pi_Bank[idx] >= SND_gst_Params.l_SoundNumber) continue;

			/* check sound */
			pst_Sound = SND_gst_Params.dst_Sound + pBank->pi_Bank[idx];
			if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;

			/* special case for SModifier */
			if(pst_Sound->pst_SModifier)
			{
				pst_ExtPlay = (SND_tdst_SModifierExtPlayer *) (SND_pst_SModifierGet(pst_Sound->pst_SModifier, SND_Cte_SModifierExtPlayer, 0)->pv_Data);
				for(us=0; us<pst_ExtPlay->st_PlayList.us_Size; us++)
				{
					l_Sound = SND_l_GetSoundIndex(pst_ExtPlay->st_PlayList.aul_KeyList[us]);
					if(l_Sound == -1) continue;

					pst_Sound = SND_gst_Params.dst_Sound + l_Sound;
					if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;

					if(pst_Sound->ul_Flags & (SND_Cul_SF_Music|SND_Cul_SF_Ambience|SND_Cul_SF_Dialog))
					{
						i_FileSize = 0;
						i_Stream++;
					}
					else if(pst_Sound->ul_Flags & SND_Cul_SF_LoadingSound)
					{
						i_FileSize = 0;
					}
					else
					{
						if(SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
							i_FileSize = SND_ui_GetDecompressedSize(pst_Sound->pst_Wave->ul_DataSize);
						else
							i_FileSize = pst_Sound->pst_Wave->ul_DataSize;
					}

					i_BankSize += i_FileSize;
				}

				continue;
			}

			/* get size */
			if(pst_Sound->ul_Flags & (SND_Cul_SF_Music|SND_Cul_SF_Ambience|SND_Cul_SF_Dialog))
			{
				i_FileSize = 0;
				i_Stream++;
			}
			else if(pst_Sound->ul_Flags & SND_Cul_SF_LoadingSound)
			{
				i_FileSize = 0;
			}
			else
			{
				if(pst_Sound->pst_Wave->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
					i_FileSize = SND_ui_GetDecompressedSize(pst_Sound->pst_Wave->ul_DataSize);
				else
					i_FileSize = pst_Sound->pst_Wave->ul_DataSize;
			}

			i_BankSize += i_FileSize;
		}


		/* only wav size */
		if(i_Stream)
		{
			sprintf
			(
				az_msg,
				"[%08x] %9d oct, %4d references (%d streams) : %s",
				pBank->ul_FileKey,
				i_BankSize,
				pBank->i_SoundNb,
				i_Stream,
				BIG_NameFile(ul_Fat)
			);
		}
		else
		{
			sprintf
			(
				az_msg,
				"[%08x] %9d oct, %4d references : %s",
				pBank->ul_FileKey,
				i_BankSize,
				pBank->i_SoundNb,
				BIG_NameFile(ul_Fat)
			);
		}

		LINK_PrintStatusMsg(az_msg);
	}

	LINK_PrintStatusMsg("(Take care that some files are counted sereval times)");
	LINK_PrintStatusMsg("---------------------------------------------");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_OnDisplayFilter(ULONG *pul_Display_Flags)
{
    EDIA_cl_SndDisp *poFilter;
    BOOL ab_State[SND_Cte_EdiShowIdx_Nb];

    ab_State[SND_Cte_EdiShowIdx_MdFPlayer] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMdFplayer) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_MdFFx] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMdFFx) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_MdFWac] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMdFWac) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_MdFVol]	= (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMdFVol) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_Micro] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMicro) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_Instance] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayInstance) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_OnlySelection] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayOnlySelection) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_Near] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayNear) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_Far] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayFar) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_Middle] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayMiddle) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_Start] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayStart) ? TRUE : FALSE ;
    ab_State[SND_Cte_EdiShowIdx_Stop] = (SND_gst_Params.ul_Flags & SND_Cte_EdiDisplayStop) ? TRUE : FALSE ;

    poFilter = new EDIA_cl_SndDisp(ab_State);
    
    if(poFilter->DoModal() == IDOK)
    {
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayMdFplayer;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayMdFFx;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayMdFWac;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayMdFVol;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayMicro;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayInstance;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayOnlySelection;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayNear;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayFar;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayMiddle;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayStart;
        SND_gst_Params.ul_Flags &= ~SND_Cte_EdiDisplayStop;

        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_MdFPlayer]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayMdFplayer;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_MdFFx]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayMdFFx;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_MdFWac]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayMdFWac;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_MdFVol])	SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayMdFVol;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_Micro]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayMicro;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_Instance]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayInstance;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_OnlySelection]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayOnlySelection;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_Near]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayNear;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_Far]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayFar;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_Middle]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayMiddle;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_Start]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayStart;
        if(poFilter->mab_ShowIt[SND_Cte_EdiShowIdx_Stop]) SND_gst_Params.ul_Flags |= SND_Cte_EdiDisplayStop;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_SoundReport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	SND_tdst_OneSound	*pst_Sound;
	char				az_msg[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg("---------------------------------------------");
	LINK_PrintStatusMsg("Sound report :");
	LINK_PrintStatusMsg("--------------");

	for(i = 0; i < SND_gst_Params.l_SoundNumber; i++)
	{
		pst_Sound = SND_gst_Params.dst_Sound + i;

		if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if((pst_Sound->ul_Flags & SND_Cul_SF_SModifier) != 0) continue;

		/* force loading for size */
		if(pst_Sound->ul_FileKey == -1)
		{
			ERR_X_Warning(0, "[SND(SoundReport)] Bad key in dst_Sound array", NULL);
		}
		else
		{
			/* MicrosoftADPCM */
			if(SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_MSADPCM)
			{
				sprintf
				(
					az_msg,
					"[%x] %9d oct, %6d Hz, %2d Users, File %s",
					pst_Sound->ul_FileKey,
					SND_ui_GetDecompressedSize(pst_Sound->pst_Wave->ul_DataSize),
					pst_Sound->pst_Wave->dwSamplesPerSec,
					pst_Sound->ul_CptUsed,
					BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey))
				);
			}

			/* MicrosoftADPCM */
			if(SND_gst_Params.i_EdiWaveFormat == WAVE_FORMAT_XBOX_ADPCM)
			{
				sprintf
				(
					az_msg,
					"[%x] %9d oct, %6d Hz, %2d Users, File %s",
					pst_Sound->ul_FileKey,
					SND_ui_GetDecompressedSize(pst_Sound->pst_Wave->ul_DataSize),
					pst_Sound->pst_Wave->dwSamplesPerSec,
					pst_Sound->ul_CptUsed,
					BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey))
				);
			}
			else
			{
				sprintf
				(
					az_msg,
					"[%x] %9d oct, %6d Hz, %2d Users, File %s",
					pst_Sound->ul_FileKey,
					pst_Sound->pst_Wave->ul_DataSize,
					pst_Sound->pst_Wave->dwSamplesPerSec,
					pst_Sound->ul_CptUsed,
					BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey))
				);
			}

			LINK_PrintStatusMsg(az_msg);
		}
	}

	LINK_PrintStatusMsg("---------------------------------------------");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_SaveSoundReport(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	SND_tdst_OneSound	*pst_Sound;
	FILE				*gh_FileLog = NULL;
	extern char			EDI_gaz_SlashL[1024];
	ULONG				ul_Size;
	char				asz_Name[512];
	char				asz_Ext[5];
	char				*pz_Temp;
	int					i_Freq;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gh_FileLog = fopen("sound_report.txt", "at");
	if(!gh_FileLog) return;

	for(i = 0; i < SND_gst_Params.l_SoundNumber; i++)
	{
		pst_Sound = SND_gst_Params.dst_Sound + i;

		if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if((pst_Sound->ul_Flags & SND_Cul_SF_SModifier) != 0) continue;

		/* force loading for size */
		if(pst_Sound->ul_FileKey == -1)
		{
			ERR_X_Warning(0, "[SND(SoundReport)] Bad key in dst_Sound array", NULL);
		}
		else
		{
			if(pst_Sound->pst_Wave)
			{
				if(pst_Sound->pst_Wave->wFormatTag != WAVE_FORMAT_PCM)
				{
					ul_Size = SND_ui_GetDecompressedSize(pst_Sound->pst_Wave->ul_DataSize);
				}
				else
				{
					ul_Size = pst_Sound->pst_Wave->ul_DataSize;
				}

				i_Freq = pst_Sound->pst_Wave->dwSamplesPerSec;
			}
			else
			{
				ul_Size = i_Freq = 0;
			}

			if(!ul_Size || !i_Freq)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~*/
				SND_tdst_WaveDesc	*pWave;
				/*~~~~~~~~~~~~~~~~~~~~~~~*/

				LOA_MakeFileRef
				(
					pst_Sound->ul_FileKey,
					(ULONG *) &pWave,
					SND_ul_WaveCallback,
					LOA_C_MustExists | LOA_C_HasUserCounter
				);
				LOA_Resolve();
                ESON_LoadAllSounds();
				if(pWave)
				{
					ul_Size = pWave->ul_DataSize;
					i_Freq = pWave->dwSamplesPerSec;
				}

				SND_WaveUnload(pWave);
			}

			/* get file name */
			L_strcpy(asz_Name, BIG_NameFile(BIG_ul_SearchKeyToFat(pst_Sound->ul_FileKey)));

			/* get file extention */
			pz_Temp = L_strrchr(asz_Name, '.');
			L_strcpy(asz_Ext, pz_Temp + 1);

			/* delete ext from file name */
			*pz_Temp = 0;

			/* map name */
			pz_Temp = L_strrchr(EDI_gaz_SlashL, '/') + 1;

			fprintf
			(
				gh_FileLog,
				"%s;%d;%s;%s;%d;%d;%x\n",
				asz_Name,				/* file */
				pst_Sound->ul_CptUsed,	/* used */
				pz_Temp,				/* map */
				asz_Ext,				/* ext */
				i_Freq,					/* freq */
				ul_Size,				/* size */
				pst_Sound->ul_FileKey	/* key */
			);
		}
	}

	pz_Temp = L_strrchr(EDI_gaz_SlashL, '/') + 1;
	fprintf
	(
		gh_FileLog,
		"%s;%d;%s;%s;%d;%d;%x\n",
		"Free_Space",					/* file */
		SND_RamRasterGetFree(),			/* used */
		pz_Temp,						/* map */
		"stat",						/* ext */
		SND_RamRasterGetFree(),		/* freq */
		SND_RamRasterGetFree(),		/* size */
		SND_RamRasterGetFree()		/* key */
	);

	fprintf
	(
		gh_FileLog,
		"%s;%d;%s;%s;%d;%d;%x\n",
		"Total",					/* file */
		SND_RamRasterGetTotal(),	/* used */
		pz_Temp,					/* map */
		"stat",						/* ext */
		SND_RamRasterGetTotal(),	/* freq */
		SND_RamRasterGetTotal(),	/* size */
		SND_RamRasterGetTotal()		/* key */
	);
	fclose(gh_FileLog);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_i_CheckMetaBank(ULONG _ul_Pos)
{
	return SND_C_EdiBadFormat; // MSK is no longer used
#if 0
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pGrp;
	BIG_tdst_GroupElem	*pGrp1;
	ULONG				ul_Size;
	int					status;
	char				*pc_Buffer;
	char				*pc_Buffer1;
	ULONG				ul_SnkPos;
	ULONG				ul_SnkSizeRef, ul_SnkSizeBis;
	ULONG				ul_DialogNbRef, ul_DialogNbBis;
	int					i_NumLanguage, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Pos == -1) return SND_C_EdiBadFormat;

	status = 0;
	ul_DialogNbRef = 0;
	ul_SnkSizeRef = 0;
	ul_Size = BIG_ul_GetLengthFile(_ul_Pos - 4);
	pc_Buffer = (char *) L_malloc(ul_Size);
	BIG_Read(_ul_Pos, pc_Buffer, ul_Size);

	i_NumLanguage = 0;
	for(i = 0; i < INO_e_MaxLangNb; i++)
	{
		if(INO_b_LanguageIsPresent(i)) i_NumLanguage = i;
	}

	if(ul_Size < (sizeof(BIG_tdst_GroupElem) * i_NumLanguage)) status |= SND_C_EdiBadLangNb;

	for(pGrp = (BIG_tdst_GroupElem *) pc_Buffer; (char *) pGrp < (pc_Buffer + ul_Size); pGrp++)
	{
		if(pGrp->ul_Key == 0) continue;
		if(pGrp->ul_Key == -1) continue;

		if(L_strnicmp((CHAR *) &pGrp->ul_Type, EDI_Csz_ExtSoundBank, 4))
		{
			status |= SND_C_EdiBadContents;
			continue;
		}

		ul_SnkPos = BIG_ul_SearchKeyToPos(pGrp->ul_Key);
		if(ul_SnkPos == -1)
		{
			status |= SND_C_EdiBadContents;
			continue;
		}

		if(ul_SnkPos == pGrp->ul_Key)
		{
			status |= SND_C_EdiBadContents;
			continue;
		}

		pc_Buffer1 = BIG_pc_ReadFileTmp(ul_SnkPos, &ul_SnkSizeBis);

		if(!ul_DialogNbRef)
		{
			ul_SnkSizeRef = ul_SnkSizeBis;

			/* parse 1st snk to get sound Nb */
			for(pGrp1 = (BIG_tdst_GroupElem *) pc_Buffer1; (char *) pGrp1 < (pc_Buffer1 + ul_SnkSizeRef); pGrp1++)
			{
				if(pGrp1->ul_Key == 0) continue;
				if(pGrp1->ul_Key == -1) continue;

				if(L_strnicmp((CHAR *) &pGrp1->ul_Type, EDI_Csz_ExtSoundDialog, 4))
				{
					status |= SND_C_EdiBadContents;
					continue;
				}

				ul_DialogNbRef++;
			}

			continue;
		}

		ul_DialogNbBis = 0;
		for(pGrp1 = (BIG_tdst_GroupElem *) pc_Buffer1; (char *) pGrp1 < (pc_Buffer1 + ul_SnkSizeBis); pGrp1++)
		{
			if(pGrp1->ul_Key == 0) continue;
			if(pGrp1->ul_Key == -1) continue;

			if(L_strnicmp((CHAR *) &pGrp1->ul_Type, EDI_Csz_ExtSoundDialog, 4))
			{
				status |= SND_C_EdiBadContents;
				continue;
			}

			ul_DialogNbBis++;
		}

		if(ul_DialogNbBis != ul_DialogNbRef) status |= SND_C_EdiBadElemNb;
	}

	L_free(pc_Buffer);
	return status;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_i_CheckSmd(ULONG _ul_FilePos, ULONG _ul_SmdFileKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					ul_Size;
	char					*pc_Buff,**_ppc;
	SND_tdst_SModifier		*pst_SModifier;
	char					log[256];
	ULONG ul_FatIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- file access ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SModifier = NULL;
	pc_Buff = BIG_pc_ReadFileTmp(_ul_FilePos, &ul_Size);
	_ppc = &pc_Buff ;

	/*$1- file check ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		SND_tdst_SModifierExtPlayer stExtPlay;
		unsigned short				i;
		SND_tdst_SModifierChunk stFileHeader;
		SND_tdst_SModifierChunk stCurr;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		L_memset(&stExtPlay, 0, sizeof(stExtPlay));

		stFileHeader.i_SModifierId = LOA_ReadULong(_ppc);
		stFileHeader.ui_DataSize = LOA_ReadULong(_ppc);
		stCurr.i_SModifierId = LOA_ReadULong(_ppc);
		stCurr.ui_DataSize = LOA_ReadULong(_ppc);
		stExtPlay.ul_SndKey = LOA_ReadULong(_ppc);
		stExtPlay.ul_SndFlags = LOA_ReadULong(_ppc);
		stExtPlay.i_SndIndex = LOA_ReadLong(_ppc);
		stExtPlay.ui_Version = LOA_ReadULong(_ppc);
		stExtPlay.ul_FileKey = LOA_ReadULong(_ppc);
		stExtPlay.ul_FileKey = LOA_ul_GetCurrentKey();
		stExtPlay.ui_SndExtFlags = LOA_ReadULong(_ppc);
		LOA_ReadLong_Ed(_ppc, NULL);
		stExtPlay.p_Template = NULL;
		stExtPlay.ui_PlayerFlag = LOA_ReadULong(_ppc);
		stExtPlay.f_DryVol = LOA_ReadFloat(_ppc);
		stExtPlay.f_DryVol_FactMin = LOA_ReadFloat(_ppc);
		stExtPlay.f_DryVol_FactMax = LOA_ReadFloat(_ppc);
		stExtPlay.f_FxVolLeft = LOA_ReadFloat(_ppc);
		stExtPlay.ul_FadeInKey = LOA_ReadULong(_ppc);
		LOA_ReadULong_Ed(_ppc, NULL);
		stExtPlay.pst_FadeIn = NULL;
		stExtPlay.ul_FadeOutKey = LOA_ReadULong(_ppc);
		LOA_ReadULong_Ed(_ppc, NULL);
		stExtPlay.pst_FadeOut = NULL;
		stExtPlay.i_Pan = LOA_ReadLong(_ppc);
		stExtPlay.i_Span = LOA_ReadLong(_ppc);
		stExtPlay.f_MinPan = LOA_ReadFloat(_ppc);
        if (stFileHeader.i_SModifierId < 3)
        {
    		stExtPlay.f_FreqCoef = 1.f;
            LOA_ReadULong(_ppc);
        }
        else
    		stExtPlay.f_FreqCoef = LOA_ReadFloat(_ppc);
		stExtPlay.f_Freq_FactMin = LOA_ReadFloat(_ppc);
		stExtPlay.f_Freq_FactMax = LOA_ReadFloat(_ppc);
		stExtPlay.f_Doppler = LOA_ReadFloat(_ppc);
		stExtPlay.af_Near[0] = LOA_ReadFloat(_ppc);
		stExtPlay.af_Near[1] = LOA_ReadFloat(_ppc);
		stExtPlay.af_Near[2] = LOA_ReadFloat(_ppc);
		stExtPlay.af_Far[0] = LOA_ReadFloat(_ppc);
		stExtPlay.af_Far[1] = LOA_ReadFloat(_ppc);
		stExtPlay.af_Far[2] = LOA_ReadFloat(_ppc);
		stExtPlay.af_MiddleBlend[0] = LOA_ReadFloat(_ppc);
		stExtPlay.af_MiddleBlend[1] = LOA_ReadFloat(_ppc);
		stExtPlay.af_MiddleBlend[2] = LOA_ReadFloat(_ppc);
		stExtPlay.f_FarCoeff = LOA_ReadFloat(_ppc);
		stExtPlay.f_MiddleCoeff = LOA_ReadFloat(_ppc);
		stExtPlay.f_CylinderHeight = LOA_ReadFloat(_ppc);
		stExtPlay.f_FxVolRight = LOA_ReadFloat(_ppc);
		LOA_ReadCharArray_Ed(_ppc, stExtPlay.ac_Reserved, SND_Cte_SModifierExtPlayerReservedSize * sizeof(char));
		stExtPlay.st_PlayList.us_Size = LOA_ReadUShort(_ppc);
		stExtPlay.st_PlayList.us_Flags = LOA_ReadUShort(_ppc);
		stExtPlay.st_InsertList.us_Size = LOA_ReadUShort(_ppc);
		stExtPlay.st_InsertList.us_Flags = LOA_ReadUShort(_ppc);
		
		if(stExtPlay.st_PlayList.us_Size)
		{
			stExtPlay.st_PlayList.aul_KeyList = (BIG_KEY *) L_malloc(stExtPlay.st_PlayList.us_Size * sizeof(BIG_KEY));
			stExtPlay.st_PlayList.aul_FlagList = (BIG_KEY *) L_malloc(stExtPlay.st_PlayList.us_Size * sizeof(ULONG));

			L_memset(stExtPlay.st_PlayList.aul_KeyList, -1, stExtPlay.st_PlayList.us_Size * sizeof(BIG_KEY));
			L_memset(stExtPlay.st_PlayList.aul_FlagList, 0, stExtPlay.st_PlayList.us_Size * sizeof(ULONG));

			for(i = 0; i < stExtPlay.st_PlayList.us_Size; i++)
			{
				stExtPlay.st_PlayList.aul_KeyList[i] = LOA_ReadULong(_ppc);
				stExtPlay.st_PlayList.aul_FlagList[i] = LOA_ReadULong(_ppc);
			}

			ERR_X_Warning
			(
				stExtPlay.ul_SndKey == BIG_C_InvalidKey,
				"[SND][SNDmodifier.c] smd can not contain play list AND file ref",
				NULL
			);
		}

		/*$2--------------------------------------------------------------------------------------------------------------*/

		if(stExtPlay.st_InsertList.us_Size)
		{
			stExtPlay.st_InsertList.aul_KeyList = (BIG_KEY *) L_malloc(stExtPlay.st_InsertList.us_Size * sizeof(BIG_KEY));
			L_memset(stExtPlay.st_InsertList.aul_KeyList, -1, stExtPlay.st_InsertList.us_Size * sizeof(BIG_KEY));

			for(i = 0; i < stExtPlay.st_InsertList.us_Size; i++)
			{
				stExtPlay.st_InsertList.aul_KeyList[i] = LOA_ReadULong(_ppc);
			}
		}

		stCurr.i_SModifierId = LOA_ReadULong(_ppc);
		stCurr.ui_DataSize = LOA_ReadULong(_ppc);
		ERR_X_Warning
		(
			stCurr.i_SModifierId  == -1,
			"[SND][SNDmodifier.c] bad smd file format",
			NULL
		);


		//$2-
	   
		if(stExtPlay.st_PlayList.us_Size)
		{
			for(i = 0; i < stExtPlay.st_PlayList.us_Size; i++)
			{
				

				if(stExtPlay.st_PlayList.aul_KeyList[i] == 0) continue;
				if(stExtPlay.st_PlayList.aul_KeyList[i] == BIG_C_InvalidKey) continue;

				ul_FatIndex = BIG_ul_SearchKeyToFat(stExtPlay.st_PlayList.aul_KeyList[i]);
				
				if(ul_FatIndex == BIG_C_InvalidIndex)
				{
					sprintf(log, "smd file [%08x] is referencing unknown key %08x", _ul_SmdFileKey,stExtPlay.st_PlayList.aul_KeyList[i]);
					ERR_X_Warning(0, log, NULL);
					continue;
				}

				if(stExtPlay.st_PlayList.aul_FlagList[i] & SND_Cul_SF_Dialog)
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundDialog))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.st_PlayList.aul_KeyList[i]);
						ERR_X_Warning(0, log, NULL);
					}
				}
				else if(stExtPlay.st_PlayList.aul_FlagList[i] & SND_Cul_SF_Music)
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundMusic))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.st_PlayList.aul_KeyList[i]);
						ERR_X_Warning(0, log, NULL);
					}
				}
				else if(stExtPlay.st_PlayList.aul_FlagList[i] & SND_Cul_SF_Ambience)
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundAmbience))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.st_PlayList.aul_KeyList[i]);
						ERR_X_Warning(0, log, NULL);
					}
				}
				else if(stExtPlay.st_PlayList.aul_FlagList[i] & SND_Cul_SF_LoadingSound)
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtLoadingSound))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.st_PlayList.aul_KeyList[i]);
						ERR_X_Warning(0, log, NULL);
					}
				}
				else
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundFile))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.st_PlayList.aul_KeyList[i]);
						ERR_X_Warning(0, log, NULL);
					}
				}
			}
		}
		else
		{
			ul_FatIndex = BIG_ul_SearchKeyToFat(stExtPlay.ul_SndKey);
			
			if(ul_FatIndex == BIG_C_InvalidIndex)
			{
				sprintf(log, "smd file [%08x] is referencing unknown key %08x", _ul_SmdFileKey,stExtPlay.ul_SndKey);
				ERR_X_Warning(0, log, NULL);
			}
			else
			{
				if(stExtPlay.ul_SndFlags & SND_Cul_SF_Dialog)
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundDialog))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.ul_SndKey);
						ERR_X_Warning(0, log, NULL);
					}
				}
				else if(stExtPlay.ul_SndFlags & SND_Cul_SF_Music)
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundMusic))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.ul_SndKey);
						ERR_X_Warning(0, log, NULL);
					}
				}
				else if(stExtPlay.ul_SndFlags & SND_Cul_SF_Ambience)
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundAmbience))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.ul_SndKey);
						ERR_X_Warning(0, log, NULL);
					}
				}
				else if(stExtPlay.ul_SndFlags & SND_Cul_SF_LoadingSound)
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtLoadingSound))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.ul_SndKey);
						ERR_X_Warning(0, log, NULL);
					}
				}
				else
				{
					if(!BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundFile))
					{
						sprintf(log, "smd file [%08x] is bad referencing this key %08x", _ul_SmdFileKey,stExtPlay.ul_SndKey);
						ERR_X_Warning(0, log, NULL);
					}
				}
			}
		}

		/*$2--------------------------------------------------------------------------------------------------------------*/
		
		if( (stExtPlay.ul_FadeInKey != 0) && (stExtPlay.ul_FadeInKey != BIG_C_InvalidKey) )
		{
			if(BIG_ul_SearchKeyToFat(stExtPlay.ul_FadeInKey)== BIG_C_InvalidIndex)
			{
				sprintf(log, "smd file [%08x] has a bad fade-in key %08x", _ul_SmdFileKey, stExtPlay.ul_FadeInKey);
				ERR_X_Warning(0, log, NULL);
			}
		}

		if( (stExtPlay.ul_FadeOutKey!= 0) && (stExtPlay.ul_FadeOutKey!= BIG_C_InvalidKey) )
		{
			if(BIG_ul_SearchKeyToFat(stExtPlay.ul_FadeOutKey)== BIG_C_InvalidIndex)
			{
				sprintf(log, "smd file [%08x] has a bad fade-out key %08x", _ul_SmdFileKey, stExtPlay.ul_FadeOutKey);
				ERR_X_Warning(0, log, NULL);
			}
		}

		/*$2--------------------------------------------------------------------------------------------------------------*/

		if(stExtPlay.st_InsertList.us_Size)
		{
			for(i = 0; i < stExtPlay.st_InsertList.us_Size; i++)
			{
				if(stExtPlay.st_InsertList.aul_KeyList[i] == BIG_C_InvalidKey) continue;
				if(stExtPlay.st_InsertList.aul_KeyList[i] == 0) continue;
				
				ul_FatIndex = BIG_ul_SearchKeyToFat(stExtPlay.st_InsertList.aul_KeyList[i]);
				if(ul_FatIndex == BIG_C_InvalidIndex)
				{
					sprintf(log, "smd file [%08x] is referencing unknown insert key %08x", _ul_SmdFileKey,stExtPlay.st_InsertList.aul_KeyList[i]);
					ERR_X_Warning(0, log, NULL);
				}
				else
				{
					if( !BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundInsert) && !BIG_b_IsFileExtension(ul_FatIndex, EDI_Csz_ExtSoundFade) )
					{
						sprintf(log, "smd file [%08x] is bad insert referencing this key %08x", _ul_SmdFileKey,stExtPlay.st_InsertList.aul_KeyList[i]);
						ERR_X_Warning(0, log, NULL);
					}
				}
			}

			if(stExtPlay.st_InsertList.us_Size)
			{
				L_free(stExtPlay.st_InsertList.aul_KeyList);
			}
		}

		/*$2--------------------------------------------------------------------------------------------------------------*/

		if(stExtPlay.st_PlayList.us_Size)
		{
			L_free(stExtPlay.st_PlayList.aul_KeyList);
			L_free(stExtPlay.st_PlayList.aul_FlagList);
		}
	}

	return 0;
}

int ESON_i_CheckBank(ULONG _ul_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pGrp;
	ULONG				ul_Size;
	int					status;
	char				*pc_Buffer;
	ULONG				ul_Fat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Pos == -1) return SND_C_EdiBadFormat;

	status = 0;
	ul_Size = BIG_ul_GetLengthFile(_ul_Pos - 4);
	pc_Buffer = (char *) L_malloc(ul_Size);
	BIG_Read(_ul_Pos, pc_Buffer, ul_Size);

	for(pGrp = (BIG_tdst_GroupElem *) pc_Buffer; (char *) pGrp < (pc_Buffer + ul_Size); pGrp++)
	{
		if(pGrp->ul_Key == 0) continue;
		if(pGrp->ul_Key == -1) continue;

		ul_Fat = BIG_ul_SearchKeyToFat(pGrp->ul_Key);
		if(ul_Fat != -1)
		{
			if
			(
				!L_strnicmp((CHAR *) &pGrp->ul_Type, EDI_Csz_ExtSoundDialog, 4)
			&&	BIG_b_IsFileExtension(ul_Fat, EDI_Csz_ExtSoundDialog)
			) continue;
			if
			(
				!L_strnicmp((CHAR *) &pGrp->ul_Type, EDI_Csz_ExtSoundMusic, 4)
			&&	BIG_b_IsFileExtension(ul_Fat, EDI_Csz_ExtSoundMusic)
			) continue;
			if
			(
				!L_strnicmp((CHAR *) &pGrp->ul_Type, EDI_Csz_ExtSoundAmbience, 4)
			&&	BIG_b_IsFileExtension(ul_Fat, EDI_Csz_ExtSoundAmbience)
			) continue;
			if
			(
				!L_strnicmp((CHAR *) &pGrp->ul_Type, EDI_Csz_ExtSoundFile, 4)
			&&	BIG_b_IsFileExtension(ul_Fat, EDI_Csz_ExtSoundFile)
			) continue;
			if
			(
				!L_strnicmp((CHAR *) &pGrp->ul_Type, EDI_Csz_ExtSModifier, 4)
			&&	BIG_b_IsFileExtension(ul_Fat, EDI_Csz_ExtSModifier)
			) continue;
			if
			(
				!L_strnicmp((CHAR *) &pGrp->ul_Type, EDI_Csz_ExtLoadingSound, 4)
			&&	BIG_b_IsFileExtension(ul_Fat, EDI_Csz_ExtLoadingSound)
			) continue;
		}

		status |= SND_C_EdiBadContents;
	}

	L_free(pc_Buffer);
	return status;
}


/*
 =======================================================================================================================
    => detect truncated file only in speedmode
 =======================================================================================================================
 */
BOOL ESON_b_IsFileTruncated(ULONG ul_Key)
{
	/*~~~~~~~~~~~~~~~~*/
	ULONG	ul_FileSize;
	char	strtmp[1024]; 
	ULONG ulFat;
	char asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~*/

	if(BIG_ul_SearchKeyToPos(ul_Key) == -1)
	{
		sprintf(strtmp, "[SND] Unknown key %08x", ul_Key);
		ERR_X_Warning(0, strtmp, NULL);
		return TRUE;
	}

	ul_FileSize = BIG_ul_EditorGetSizeOfFile(ul_Key);
	if(ul_FileSize > 8) return FALSE;

	if(ul_FileSize == 8)
	{
		if(LOA_gb_SpeedMode || EDI_gb_ComputeMap)
		{
			ulFat = BIG_ul_SearchKeyToFat(ul_Key);
			if(ulFat != BIG_C_InvalidIndex)
			{
				BIG_ComputeFullName(BIG_ParentFile(ulFat), asz_Path);
				sprintf(strtmp, "[SND] File is truncated [%08x] %s\\%s", ul_Key, asz_Path, BIG_NameFile(ulFat));
			}
			else
			{
				BIG_ComputeFullName(BIG_ParentFile(ulFat), asz_Path);
				sprintf(strtmp, "[SND] File is truncated [%08x]", ul_Key);
			}
			ERR_X_Warning(0, strtmp, NULL);
		}

		return TRUE;
	}
	else
	{
		sprintf(strtmp, "[SND] Bad file size [%08x] %s", ul_Key);
		ERR_X_Warning(0, strtmp, NULL);
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_BankUpdate(ULONG _ul_fat)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i;
	SND_tdun_Main	*pst_BankOld;
	int				i_NumSound;
	SND_tdst_Bank	 *pOldBank;
	char	log[512];
    int * piBank;
    ULONG ulSize;
    BIG_tdst_GroupElem*pGroup;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_fat == BIG_C_InvalidIndex) return;
	if(BIG_FileKey(_ul_fat) == BIG_C_InvalidKey) return;
	if((BIG_FileChanged(_ul_fat) & EDI_FHC_Loaded) == 0) return;

	for(i = 0; i < (int) SND_gul_MainRefListSize; i++)
	{
		pst_BankOld = SND_gap_MainRefList[i];

		if(!pst_BankOld) continue;
		if((ULONG) pst_BankOld->st_Bank.ul_FileKey == BIG_FileKey(_ul_fat)) break;
	}

	if(i >= (int) SND_gul_MainRefListSize) return;
	if(!BIG_b_IsFileExtension(_ul_fat, EDI_Csz_ExtSoundBank)) return;
	
	pOldBank = SND_p_MainGetBank(pst_BankOld);
	if(pOldBank->pi_SaveBank) 
	{
		sprintf
		(
			log,
			"The sound bank %s [%x] was merged and is now restored due to file modification",
			BIG_NameFile(_ul_fat),
			BIG_FileKey(_ul_fat)
		);
		ERR_X_Warning(0, log, NULL);
		SND_BankRestore(pOldBank);
	}

    
    pGroup= (BIG_tdst_GroupElem*)BIG_pc_ReadFileTmp(BIG_ul_SearchKeyToPos(BIG_FileKey(_ul_fat)), &ulSize);
    
	i_NumSound = (int)ulSize/sizeof(BIG_tdst_GroupElem);
	piBank = (int*)MEM_p_Alloc(i_NumSound*sizeof(int));
    L_memset(piBank, -1, i_NumSound*sizeof(int));
    
    for(i = 0; i<i_NumSound; i++)
    {
        if(pGroup[i].ul_Key == 0) continue;
        if(pGroup[i].ul_Key == BIG_C_InvalidKey) continue;

		if(L_strnicmp((char *) &pGroup[i].ul_Type, EDI_Csz_ExtSoundDialog, 4) == 0)
			piBank[i] = SND_l_AddDialog(pGroup[i].ul_Key);
		else if(L_strnicmp((char *) &pGroup[i].ul_Type, EDI_Csz_ExtSoundMusic, 4) == 0)
			piBank[i] = SND_l_AddMusic(pGroup[i].ul_Key);
		else if(L_strnicmp((char *) &pGroup[i].ul_Type, EDI_Csz_ExtSoundAmbience, 4) == 0)
			piBank[i] = SND_l_AddAmbience(pGroup[i].ul_Key);
		else if(L_strnicmp((char *) &pGroup[i].ul_Type, EDI_Csz_ExtSModifier, 4) == 0)
			piBank[i] = SND_l_AddSModifier(pGroup[i].ul_Key);
		else if(L_strnicmp((char *) &pGroup[i].ul_Type, EDI_Csz_ExtLoadingSound, 4) == 0)
			piBank[i] = SND_l_AddLoadingSound(pGroup[i].ul_Key);
		else if(L_strnicmp((char *) &pGroup[i].ul_Type, EDI_Csz_ExtSoundFile, 4) == 0)
			piBank[i] = SND_l_AddSound(pGroup[i].ul_Key);
    }
    LOA_Resolve();
    ESON_LoadAllSounds();

    for(i=0; i<pOldBank->i_SoundNb; i++)
    {
        SND_DeleteByIndex(pOldBank->pi_Bank[i]);
    }
    MEM_Free(pOldBank->pi_Bank);

	pOldBank->pi_Bank = piBank;
    pOldBank->i_SoundNb = i_NumSound;	    
}

/*$4
 ***********************************************************************************************************************
    SOLO / MUTE
 ***********************************************************************************************************************
 */

BAS_tdst_barray ESON_gst_SoloGao;
BAS_tdst_barray ESON_gst_MutedGao;
BAS_tdst_barray ESON_gst_SoloInstance;
BAS_tdst_barray ESON_gst_MutedInstance;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_InitUtilModule(void)
{
	BAS_binit(&ESON_gst_SoloGao, 100);
	BAS_binit(&ESON_gst_MutedGao, 100);
	BAS_binit(&ESON_gst_SoloInstance, 100);
	BAS_binit(&ESON_gst_MutedInstance, 100);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_CloseUtilModule(void)
{
	BAS_bfree(&ESON_gst_SoloGao);
	BAS_bfree(&ESON_gst_MutedGao);
	BAS_bfree(&ESON_gst_SoloInstance);
	BAS_bfree(&ESON_gst_MutedInstance);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_SetSoloGao(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					i;
	ESON_cl_Frame			*po_SoundEditor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(!pst_GO) return;

    if(BAS_bsearch((ULONG) pst_GO, &ESON_gst_SoloGao) == (ULONG) - 1)
	{
		BAS_binsert((ULONG) pst_GO, (ULONG) pst_GO, &ESON_gst_SoloGao);
	}
	else
	{
		BAS_bdelete((ULONG) pst_GO, &ESON_gst_SoloGao);
	}

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;

		SND_SetInstVolume(pst_SI);
	}

	po_SoundEditor = (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
	po_SoundEditor->mpo_View->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_SetSoloInstance(ULONG idx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	int						i;
	void					*pst_GO;
	ESON_cl_Frame			*po_SoundEditor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	pst_SI = SND_gst_Params.dst_Instance + idx;
	if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) return;

	pst_SI->ul_EdiFlags &= ~(SND_EdiFlg_LastOpeIsGao | SND_EdiFlg_LastOpeIsInst);
	pst_SI->ul_EdiFlags |= SND_EdiFlg_LastOpeIsInst;
	pst_GO = pst_SI->p_GameObject;

	if(BAS_bsearch(idx, &ESON_gst_SoloInstance) == (ULONG) - 1)
	{
		BAS_binsert(idx, idx, &ESON_gst_SoloInstance);
		BAS_binsert((ULONG) pst_SI->p_GameObject, 1, &ESON_gst_SoloGao);
	}
	else
	{
		BAS_bdelete(idx, &ESON_gst_SoloInstance);
		if((ESON_gst_SoloInstance.num == 0) && (ESON_gst_SoloGao.num == 1)) ESON_SoloOff();
	}

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;

		SND_SetInstVolume(pst_SI);
	}

	po_SoundEditor = (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
	po_SoundEditor->mpo_View->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_SetMuteGao(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					i;
	ESON_cl_Frame			*po_SoundEditor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(!pst_GO) return;

	if(BAS_bsearch((ULONG) pst_GO, &ESON_gst_MutedGao) == (ULONG) - 1)
	{
		if(ESON_gst_SoloGao.num)
		{
			ESON_SetSoloGao(pst_GO);
		}
		else
		{
			BAS_binsert((ULONG) pst_GO, (ULONG) pst_GO, &ESON_gst_MutedGao);
		}
	}
	else
	{
		BAS_bdelete((ULONG) pst_GO, &ESON_gst_MutedGao);
	}

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;

		SND_SetInstVolume(pst_SI);
	}

	po_SoundEditor = (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
	po_SoundEditor->mpo_View->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_SetMuteInstance(ULONG idx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	ESON_cl_Frame			*po_SoundEditor;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	pst_SI = SND_gst_Params.dst_Instance + idx;
	if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) return;

	pst_SI->ul_EdiFlags &= ~(SND_EdiFlg_LastOpeIsGao | SND_EdiFlg_LastOpeIsInst);
	pst_SI->ul_EdiFlags |= SND_EdiFlg_LastOpeIsInst;

	if(BAS_bsearch(idx, &ESON_gst_MutedInstance) == (ULONG) - 1)
	{
		if(ESON_gst_SoloInstance.num)
		{
			ESON_SetSoloInstance(idx);
		}
		else
		{
			BAS_binsert(idx, idx, &ESON_gst_MutedInstance);
			BAS_binsert((ULONG) pst_SI->p_GameObject, 1, &ESON_gst_MutedGao);
		}
	}
	else
	{
		BAS_bdelete(idx, &ESON_gst_MutedInstance);
	}

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;

		SND_SetInstVolume(pst_SI);
	}

	po_SoundEditor = (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
	po_SoundEditor->mpo_View->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_SoloOff(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					i;
	ESON_cl_Frame			*po_SoundEditor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	BAS_bfree(&ESON_gst_SoloGao);
	BAS_binit(&ESON_gst_SoloGao, 100);
	BAS_bfree(&ESON_gst_SoloInstance);
	BAS_binit(&ESON_gst_SoloInstance, 100);

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;
		SND_SetInstVolume(pst_SI);
	}

	po_SoundEditor = (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
	po_SoundEditor->mpo_View->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_MuteOff(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	LONG					i;
	ESON_cl_Frame			*po_SoundEditor;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;

	BAS_bfree(&ESON_gst_MutedGao);
	BAS_binit(&ESON_gst_MutedGao, 100);
	BAS_bfree(&ESON_gst_MutedInstance);
	BAS_binit(&ESON_gst_MutedInstance, 100);

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) continue;
		SND_SetInstVolume(pst_SI);
	}

	po_SoundEditor = (ESON_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_SOUND, 0);
	po_SoundEditor->mpo_View->UpdateControls();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_GaoIsSolo(OBJ_tdst_GameObject *pst_GO)
{
    OBJ_tdst_GameObject * pFather;

	if(!SND_gst_Params.l_Available) return FALSE;
	if(!pst_GO) return FALSE;

	if(BAS_bsearch((ULONG) pst_GO, &ESON_gst_SoloGao) != (ULONG) - 1) return TRUE;

    pFather = OBJ_pst_GetFather(pst_GO);
    while(pFather)
    {
        pst_GO = pFather;
        pFather = OBJ_pst_GetFather(pst_GO);
    }    

	if(BAS_bsearch((ULONG) pst_GO, &ESON_gst_SoloGao) != (ULONG) - 1) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_GaoIsMuted(OBJ_tdst_GameObject *pst_GO)
{
    OBJ_tdst_GameObject * pFather;

	if(!SND_gst_Params.l_Available) return FALSE;
	if(!pst_GO) return FALSE;
	if(BAS_bsearch((ULONG) pst_GO, &ESON_gst_MutedGao) != (ULONG) - 1) return TRUE;
	
    pFather = OBJ_pst_GetFather(pst_GO);
    while(pFather)
    {
        pst_GO = pFather;
        pFather = OBJ_pst_GetFather(pst_GO);
    }    

	if(BAS_bsearch((ULONG) pst_GO, &ESON_gst_MutedGao) != (ULONG) - 1) return TRUE;
    return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_InstanceIsSolo(ULONG idx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG ul;
	SND_tdst_SoundInstance	*pst_SI;
    OBJ_tdst_GameObject * pFather,*pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return FALSE;
	if(!ESON_b_SoloIsActive()) return FALSE;

	pst_SI = SND_gst_Params.dst_Instance + idx;
	if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) return FALSE;

    pst_GO = (OBJ_tdst_GameObject*)pst_SI->p_GameObject;
    ul = BAS_bsearch((ULONG)pst_GO, &ESON_gst_SoloGao) ;

	if(ul == (ULONG)-1)
	{
        pFather = OBJ_pst_GetFather(pst_GO);
        while(pFather)
        {
            pst_GO = pFather;
            pFather = OBJ_pst_GetFather(pst_GO);
        }
        ul = BAS_bsearch((ULONG)pst_GO, &ESON_gst_SoloGao);
    }

    switch(ul)
    {
    case -1:
        return FALSE;

	case 1:
        if(BAS_bsearch(idx, &ESON_gst_SoloInstance) == (ULONG) - 1) 
            return FALSE; 
        else
			return TRUE;

	default:	
        return TRUE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_InstanceIsMuted(ULONG idx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
    OBJ_tdst_GameObject * pFather,*pst_GO;
    ULONG ul;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return FALSE;
	if(!ESON_b_MuteIsActive()) return FALSE;

	pst_SI = SND_gst_Params.dst_Instance + idx;
	if(!(pst_SI->ul_Flags & SND_Cul_DSF_Used)) return FALSE;

    pst_GO = (OBJ_tdst_GameObject*)pst_SI->p_GameObject;
    ul = BAS_bsearch((ULONG) pst_GO, &ESON_gst_MutedGao);

	if(ul == (ULONG)-1)
	{
        pFather = OBJ_pst_GetFather(pst_GO);
        while(pFather)
        {
            pst_GO = pFather;
            pFather = OBJ_pst_GetFather(pst_GO);
        }
        ul = BAS_bsearch((ULONG)pst_GO, &ESON_gst_MutedGao);
    }

	switch(ul)
	{
	case -1:	return FALSE;
	case 1:		if(BAS_bsearch(idx, &ESON_gst_MutedInstance) == (ULONG) - 1) return FALSE; else
			return TRUE;
	default:	return TRUE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_SoloIsActive(void)
{
	if(!SND_gst_Params.l_Available) return FALSE;
	if(ESON_gst_SoloGao.num) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_SoloInstIsActive(void)
{
	if(!SND_gst_Params.l_Available) return FALSE;
	if(ESON_gst_SoloInstance.num) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_MuteIsActive(void)
{
	if(!SND_gst_Params.l_Available) return FALSE;
	if(ESON_gst_MutedGao.num) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ESON_b_MuteInstIsActive(void)
{
	if(!SND_gst_Params.l_Available) return FALSE;
	if(ESON_gst_MutedInstance.num) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_NotifyGaoIsDeleted(OBJ_tdst_GameObject *pGO)
{
	if(!SND_gst_Params.l_Available) return;
	if(!pGO) return;

	BAS_bdelete((ULONG) pGO, &ESON_gst_SoloGao);
	BAS_bdelete((ULONG) pGO, &ESON_gst_MutedGao);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_NotifyInstanceIsDeleted(ULONG idx)
{
	if(!SND_gst_Params.l_Available) return;

	BAS_bdelete(idx, &ESON_gst_SoloInstance);
	BAS_bdelete(idx, &ESON_gst_MutedInstance);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ESON_LoadAllSounds(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				l_Index;
	SND_tdst_OneSound	*pst_Sound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(!SND_gst_Params.l_Unloaded) return;
	for(l_Index = 0; l_Index < SND_gst_Params.l_SoundNumber; l_Index++)
	{
		pst_Sound = SND_gst_Params.dst_Sound + l_Index;
		if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if(pst_Sound->ul_Flags & SND_Cul_OSF_Loaded) continue;

		SND_l_LoadData(pst_Sound->ul_FileKey);
	}

	SND_gst_Params.l_Unloaded = 0;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
