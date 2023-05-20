/*$T ediSND.c GC 1.138 12/21/04 08:42:44 */


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

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"

#include "BIGfiles/BIGio.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "ENGine/Sources/OBJects/OBJaccess.h"

#include "ENGine/Sources/ENGvars.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDrasters.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/edi/ediSNDdebug.h"
#include "SouND/Sources/SNDconv_xboxadpcm.h"
#include "EDItors/Sources/SOuNd/SONutil.h"
#include "SouND/Sources/SNDconv_pcretailadpcm.h"

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

extern void BIG_ReadNoSeek(ULONG, void *, ULONG);

static int	ediSND_l_InitPrimaryBuffer(SND_tdst_TargetSpecificData *);

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */
/*$off*/
int							SNDPanTableL[128] =
{
	0,		0,		-10,	-10,	-10,	-20,	-20,	-20,	-30,	-30,
	-40,	-40,	-40,	-50,	-50,	-50,	-60,	-60,	-70,	-70,
	-70,	-80,	-80,	-90,	-90,	-100,	-100,	-100,	-110,	-110,
	-120,	-120,	-130,	-130,	-140,	-140,	-140,	-150,	-150,	-160,
	-160,	-170,	-170,	-180,	-180,	-190,	-200,	-200,	-210,	-210,
	-220,	-220,	-230,	-230,	-240,	-250,	-250,	-260,	-260,	-270,
	-280,	-280,	-290,	-300,	-300,	-310,	-320,	-330,	-330,	-340,
	-350,	-360,	-360,	-370,	-380,	-390,	-400,	-400,	-410,	-420,
	-430,	-440,	-450,	-460,	-470,	-480,	-490,	-500,	-510,	-520,
	-540,	-550,	-560,	-570,	-590,	-600,	-610,	-630,	-640,	-660,
	-670,	-690,	-710,	-720,	-740,	-760,	-780,	-800,	-830,	-850,
	-870,	-900,	-930,	-960,	-990,	-1020,	-1060,	-1100,	-1150,	-1200,
	-1260,	-1330,	-1400,	-1500,	-1630,	-1800,	-2100,	-9040
};
/*$on*/
extern HANDLE		ediSND_gh_ThreadHandle;
BAS_tdst_barray		ediSND_gst_PlayingSB;
ediSND_tdst_Manager ediSND_gst_SoundManager;
int					ediSND_gi_DoClose = 0;
BAS_tdst_barray		ediSND_dst_CoreUser[SND_Cte_FxCoreNb];
extern BOOL			ediSND_gb_FxAlloc;
extern BOOL			ediSND_gb_NoFx;
BOOL				ediSND_gb_Duplicate = FALSE;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_l_Init(SND_tdst_TargetSpecificData *_pst_SpecificD)
{
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    global var reset
	 -------------------------------------------------------------------------------------------------------------------
	 */

	BAS_binit(&ediSND_gst_PlayingSB, 48);
	BAS_binit(&ediSND_dst_CoreUser[SND_Cte_FxCoreA], SND_Cte_FxMaxVoiceNbByCore);
	BAS_binit(&ediSND_dst_CoreUser[SND_Cte_FxCoreB], SND_Cte_FxMaxVoiceNbByCore);

	L_memset(&ediSND_gst_SoundManager, 0, sizeof(ediSND_tdst_Manager));

	ediSND_gh_ThreadHandle = NULL;
	ediSND_gst_SoundManager.l_GlobalVol = 0;

	_pst_SpecificD->h_Wnd = MAI_gh_MainWindow;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    DirectSound init
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(ediSND_gi_DoClose) ediSND_Close(SND_gst_Params.pst_SpecificD);

	hr = CoInitialize(NULL);
	ERR_X_Warning((hr == S_OK), "bad return value (ediSND_l_Init)", NULL);
	// hogsy:	ah windows... 
	//			because we init this once and then again when switching project
	//			it returned S_FALSE next time, rather than S_OK, which is FINE!
	//			so we'll check for all three of these...
	if(	hr != S_OK && 
		hr != S_FALSE && 
		hr != RPC_E_CHANGED_MODE) 
	{
		return 0;
	}

	hr = DirectSoundCreate8(NULL, &_pst_SpecificD->pst_DS, NULL);
	if(hr == DS_OK)
	{
		ediSND_gi_DoClose = 1;
		ediSND_gst_SoundManager.st_DriverCapacity.dwSize = sizeof(DSCAPS);
		hr = IDirectSound8_GetCaps(_pst_SpecificD->pst_DS, &ediSND_gst_SoundManager.st_DriverCapacity);
		ERR_X_Warning((hr == S_OK), "bad return value (ediSND_l_Init)", NULL);

		hr = IDirectSound8_SetCooperativeLevel(_pst_SpecificD->pst_DS, _pst_SpecificD->h_Wnd, DSSCL_PRIORITY);
		ERR_X_Warning((hr == S_OK), "bad return value (ediSND_l_Init)", NULL);

		if(!ediSND_l_InitPrimaryBuffer(_pst_SpecificD)) 
		{
			return 0;
		}
		ediSND_StreamInitModule();
		return 1;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_Close(SND_tdst_TargetSpecificData *_pst_SD)
{
	ediSND_gi_DoClose = 0;
	ediSND_StreamCloseModule();

	BAS_bfree(&ediSND_gst_PlayingSB);
	BAS_bfree(&ediSND_dst_CoreUser[SND_Cte_FxCoreA]);
	BAS_bfree(&ediSND_dst_CoreUser[SND_Cte_FxCoreB]);

    if (_pst_SD->pst_DS)
    {
		IDirectSound8_Release(_pst_SD->pst_DS);
    }

	CoUninitialize();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int ediSND_l_InitPrimaryBuffer(SND_tdst_TargetSpecificData *_pst_SpecificD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DSBUFFERDESC	dsbd;
	WAVEFORMATEX	st_Format;
	HRESULT			hr;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2- set capacities ---------------------------------------------------------------------------------------------*/

	L_memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;
	dsbd.dwBufferBytes = 0;		/* only for primary buffer settings */
	dsbd.lpwfxFormat = NULL;	/* only for primary buffer settings */
	dsbd.guid3DAlgorithm = GUID_NULL;

	hr = IDirectSound8_CreateSoundBuffer
		(
			_pst_SpecificD->pst_DS,
			&dsbd,
			(IDirectSoundBuffer **) &_pst_SpecificD->pst_PrimaryDSB,
			NULL
		);
	ediSND_M_Assert(hr == DS_OK);
	if(FAILED(hr)) return 0;

	/*$2- set format -------------------------------------------------------------------------------------------------*/

	L_memset(&st_Format, 0, sizeof(WAVEFORMATEX));

	st_Format.nSamplesPerSec = 48000;
	st_Format.wBitsPerSample = 16;
	st_Format.nChannels = 2;
	st_Format.nBlockAlign = st_Format.nChannels * sizeof(short);
	st_Format.nAvgBytesPerSec = st_Format.nBlockAlign * st_Format.nSamplesPerSec;
	st_Format.cbSize = 0;
	st_Format.wFormatTag = WAVE_FORMAT_PCM;

	hr = IDirectSoundBuffer8_SetFormat(_pst_SpecificD->pst_PrimaryDSB, &st_Format);
	ediSND_M_Assert(hr == DS_OK);
	if(hr != DS_OK) return 0;

	/*$2- start playing ----------------------------------------------------------------------------------------------*/

	hr = IDirectSoundBuffer8_Play(_pst_SpecificD->pst_PrimaryDSB, 0, 0, DSBPLAY_LOOPING);

	if(hr != DS_OK) return 0;

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *ediSND_pst_SB_Create
(
	SND_tdst_TargetSpecificData *_pst_SpecificD,
	SND_tdst_WaveData			*_pst_WI,
	unsigned int				_ui_Pos
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DSBUFFERDESC			dsbd;
	SND_tdst_SoundBuffer	*pst_SB;
	BYTE					*pbData;
	BYTE					*pbData2;
	DWORD					dwLength;
	DWORD					dwLength2;
	HRESULT					hr;
	char					*pc_Data;
	extern BOOL				LOA_gb_SpeedMode;
	int						cbSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SpecificD->pst_DS) return NULL;
	pc_Data = NULL;

	if(!SND_M_IsGoodFormat(_pst_WI->st_WaveFmtx.wFormatTag) && !LOA_gb_SpeedMode) 
	{
		return NULL;
	}

	switch(_pst_WI->st_WaveFmtx.wFormatTag)
	{
	case WAVE_FORMAT_PCM:
	case WAVE_FORMAT_XBOX_ADPCM:
		cbSize = 0;
		break;

	default:
		cbSize = _pst_WI->st_WaveFmtx.cbSize;
		break;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    read data
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(LOA_gb_SpeedMode)
	{
		if(_ui_Pos)
		{
			pc_Data = (char*)L_malloc(_pst_WI->i_Size + cbSize);
			if(!pc_Data) return NULL;

			if(!LOA_IsBinaryData())
			{
				BIG_ReadNoSeek(_ui_Pos, pc_Data, _pst_WI->i_Size + cbSize);
			}

			if(LOA_IsBinarizing())
			{
				/*~~~~~~~~~~~~*/
				CHAR	*pc_Buf;
				/*~~~~~~~~~~~~*/

				if(_pst_WI->st_WaveFmtx.nChannels == 1)
				{
					if(cbSize)
					{
						pc_Buf = pc_Data;
						LOA_FetchBuffer(0);
						LOA_ReadCharArray(&pc_Buf, pc_Data, cbSize);
					}

					pc_Buf = pc_Data + _pst_WI->st_WaveFmtx.cbSize;
					LOA_FetchBuffer(0);
					LOA_ReadCharArray(&pc_Buf, pc_Data + cbSize, _pst_WI->i_Size);
				}
				else
				{
					_pst_WI->st_WaveFmtx.cbSize >>= 1;
					cbSize >>= 1;
					_pst_WI->i_Size >>= 1;

					if(cbSize)
					{
						pc_Buf = pc_Data;
						LOA_FetchBuffer(0);
						LOA_ReadCharArray(&pc_Buf, pc_Data, cbSize);
					}

					pc_Buf = pc_Data + cbSize;
					LOA_FetchBuffer(0);
					LOA_ReadCharArray(&pc_Buf, pc_Data + cbSize, _pst_WI->i_Size);

					if(cbSize)
					{
						pc_Buf = pc_Data + cbSize + _pst_WI->i_Size;
						LOA_FetchBuffer(0);
						LOA_ReadCharArray(&pc_Buf, pc_Data + cbSize + _pst_WI->i_Size, _pst_WI->st_WaveFmtx.cbSize);
					}

					pc_Buf = pc_Data + cbSize + _pst_WI->i_Size + cbSize;
					LOA_FetchBuffer(0);
					LOA_ReadCharArray(&pc_Buf, pc_Data + (2 * cbSize) + _pst_WI->i_Size, _pst_WI->i_Size);
				}
			}

			if(LOA_IsBinaryData())
			{
				/*~~~~~~~~~~~~*/
				CHAR	*pc_Buf;
				/*~~~~~~~~~~~~*/

				if(_pst_WI->st_WaveFmtx.nChannels == 1)
				{
					if(cbSize)
					{
						pc_Buf = LOA_FetchBuffer(cbSize);
						LOA_ReadCharArray(&pc_Buf, pc_Data, cbSize);
					}

					pc_Buf = LOA_FetchBuffer(_pst_WI->i_Size);
					LOA_ReadCharArray(&pc_Buf, pc_Data + cbSize, _pst_WI->i_Size);
				}
				else
				{
					_pst_WI->st_WaveFmtx.cbSize >>= 1;
					cbSize >>= 1;
					_pst_WI->i_Size >>= 1;

					if(cbSize)
					{
						pc_Buf = LOA_FetchBuffer(cbSize);
						LOA_ReadCharArray(&pc_Buf, pc_Data, cbSize);
					}

					pc_Buf = LOA_FetchBuffer(_pst_WI->i_Size);
					LOA_ReadCharArray(&pc_Buf, pc_Data + cbSize, _pst_WI->i_Size);

					if(cbSize)
					{
						pc_Buf = LOA_FetchBuffer(cbSize);
						LOA_ReadCharArray(&pc_Buf, pc_Data + cbSize + _pst_WI->i_Size, cbSize);
					}

					pc_Buf = LOA_FetchBuffer(_pst_WI->i_Size);
					LOA_ReadCharArray(&pc_Buf, pc_Data + (2 * cbSize) + _pst_WI->i_Size, _pst_WI->i_Size);
				}
			}
		}
	}
	else
	{
		if(_ui_Pos)
		{
			pc_Data = (char*)L_malloc(_pst_WI->i_Size);
			if(!pc_Data) return NULL;

			BIG_ReadNoSeek(_ui_Pos, pc_Data, _pst_WI->i_Size);
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    check the format only here because of PS2/GC format must be in bin file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!SND_M_IsGoodFormat(_pst_WI->st_WaveFmtx.wFormatTag))
	{
		/* rasterize sound RAM during binarization */
		if(!ediSND_gb_Duplicate)
		{
			if(_pst_WI->st_WaveFmtx.wFormatTag == WAVE_FORMAT_PS2)
				SND_RamRasterAdd(-1, (_pst_WI->i_Size * 2 * 28) / (16));
			else if(_pst_WI->st_WaveFmtx.wFormatTag == WAVE_FORMAT_GAMECUBE)
				SND_RamRasterAdd(-1, (_pst_WI->i_Size * 2 * 14) / (8));
			else if(_pst_WI->st_WaveFmtx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
				SND_RamRasterAdd(-1, (_pst_WI->i_Size * 32) / (9));
		}

		if(pc_Data) L_free(pc_Data);
		return NULL;
	}

	if((_pst_WI->st_WaveFmtx.wFormatTag == WAVE_FORMAT_XBOX_ADPCM))
	{
		/*~~~~~~~~~~~~*/
		char	*pTemp;
		int		newsize;
		/*~~~~~~~~~~~~*/

		if(_ui_Pos)
		{
			newsize = SND_ui_GetDecompressedSize(_pst_WI->i_Size);
			pTemp = (char*)L_malloc(newsize);
			L_memset(pTemp, 0, newsize);

			SND_b_Decode
			(
				pc_Data,
				pTemp,
				_pst_WI->i_Size / _pst_WI->st_WaveFmtx.nBlockAlign,
				_pst_WI->st_WaveFmtx.nChannels
			);

			L_free(pc_Data);
			pc_Data = pTemp;
		}

		_pst_WI->i_Size = SND_ui_GetDecompressedSize(_pst_WI->i_Size);
		_pst_WI->st_WaveFmtx.nBlockAlign = _pst_WI->st_WaveFmtx.nChannels * sizeof(short);
		_pst_WI->st_WaveFmtx.wBitsPerSample = 16;
		_pst_WI->st_WaveFmtx.wFormatTag = WAVE_FORMAT_PCM;
		_pst_WI->st_WaveFmtx.cbSize = 0;
		_pst_WI->st_WaveFmtx.nAvgBytesPerSec = _pst_WI->st_WaveFmtx.nSamplesPerSec * _pst_WI->st_WaveFmtx.nBlockAlign;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    init var
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pbData = NULL;
	pbData2 = NULL;
	pst_SB = (SND_tdst_SoundBuffer *) MEM_p_Alloc(sizeof(SND_tdst_SoundBuffer));
	L_memset(pst_SB, 0, sizeof(SND_tdst_SoundBuffer));
	pst_SB->i_PanIdx = 64;
	L_memset(&dsbd, 0, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags =
		(
			DSBCAPS_CTRLVOLUME |
			DSBCAPS_CTRLPAN |
			DSBCAPS_CTRLFREQUENCY |
#ifdef USE_FX
            DSBCAPS_CTRLFX |
#endif
			DSBCAPS_LOCSOFTWARE
		);
	dsbd.dwBufferBytes = _pst_WI->i_Size;
	dsbd.lpwfxFormat = &_pst_WI->st_WaveFmtx;
	dsbd.guid3DAlgorithm = GUID_NULL;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Create the buffer
	 -------------------------------------------------------------------------------------------------------------------
	 */

	hr = IDirectSound8_CreateSoundBuffer(_pst_SpecificD->pst_DS, &dsbd, (IDirectSoundBuffer **) &pst_SB->pst_DSB, NULL);
	if
	(
		(hr != DS_OK)
	&&	(
			dsbd.dwFlags ==
				(DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLFX | DSBCAPS_LOCSOFTWARE)
		)
	)
	{
		ediSND_gb_NoFx = TRUE;
		L_memset(&dsbd, 0, sizeof(DSBUFFERDESC));
		dsbd.dwSize = sizeof(DSBUFFERDESC);
		dsbd.dwFlags = (DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_LOCSOFTWARE);
		dsbd.dwBufferBytes = _pst_WI->i_Size;
		dsbd.lpwfxFormat = &_pst_WI->st_WaveFmtx;
		dsbd.guid3DAlgorithm = GUID_NULL;
		hr = IDirectSound8_CreateSoundBuffer
			(
				_pst_SpecificD->pst_DS,
				&dsbd,
				(IDirectSoundBuffer **) &pst_SB->pst_DSB,
				NULL
			);
	}

	ediSND_M_Assert(hr == DS_OK);

	if(hr != DS_OK)
	{
		if(pc_Data) L_free(pc_Data);
		MEM_Free(pst_SB);
		return NULL;
	}

	pst_SB->i_Channel	= dsbd.lpwfxFormat->nChannels;
	pst_SB->l_Frequency = dsbd.lpwfxFormat->nSamplesPerSec;
	pst_SB->i_Format    = dsbd.lpwfxFormat->wFormatTag;
	pst_SB->l_Volume	= 0;
	pst_SB->l_Pan		= 0;

	if(!ediSND_gb_Duplicate) SND_RamRasterAdd((int) pst_SB->pst_DSB, dsbd.dwBufferBytes);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    fill in the DirectSound buffer
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(pc_Data)
	{
		if
		(
			DS_OK == IDirectSoundBuffer8_Lock
				(
					pst_SB->pst_DSB,
					0,
					_pst_WI->i_Size,
					(void **) &pbData,
					&dwLength,
					(void **) &pbData2,
					&dwLength2,
					0L
				)
		)
		{
			L_memcpy(pbData, pc_Data, dwLength);
			if(dwLength2) L_memcpy(pbData2, pc_Data + dwLength, dwLength2);
			IDirectSoundBuffer8_Unlock(pst_SB->pst_DSB, pbData, dwLength, pbData2, dwLength2);
		}
		else
		{
			IDirectSoundBuffer8_Release(pst_SB->pst_DSB);
			MEM_Free(pst_SB);
			pst_SB = NULL;
		}

		L_free(pc_Data);
	}

	return pst_SB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_SoundBuffer *ediSND_SB_ReLoad
(
	SND_tdst_TargetSpecificData *_pst_TargetSpeData,
	SND_tdst_SoundBuffer		*_pst_SB,
	SND_tdst_WaveData			*_pst_WaveData,
	unsigned int				_ui_DataPos
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundBuffer	*pst_NewSB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!ediSND_gb_Duplicate) SND_RamRastersDel((int) _pst_SB->pst_DSB);

	pst_NewSB = ediSND_pst_SB_Create(_pst_TargetSpeData, _pst_WaveData, _ui_DataPos);
	if(pst_NewSB)
	{
		ediSND_SB_Release(_pst_SB);
	}

	return pst_NewSB;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_Duplicate
(
	SND_tdst_TargetSpecificData *_pst_SpeData,
	SND_tdst_SoundBuffer		*_pst_SrcSB,
	SND_tdst_SoundBuffer		**_ppst_DstSB
)
{
#ifdef USE_FX
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_WaveData		st_WaveFile;
	SND_tdst_OneSound		*pst_Sound;
	SND_tdst_SoundInstance	*pst_SI;
	LONG					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_ppst_DstSB = NULL;
	if(_pst_SrcSB->pst_SS) return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    search the original SB from the sound array
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i = 0; i < SND_gst_Params.l_SoundNumber; i++)
	{
		pst_Sound = SND_gst_Params.dst_Sound + i;
		if((pst_Sound->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if(pst_Sound->pst_DSB != _pst_SrcSB) continue;

		st_WaveFile.st_WaveFmtx.wFormatTag = pst_Sound->pst_Wave->wFormatTag;
		st_WaveFile.st_WaveFmtx.nChannels = pst_Sound->pst_Wave->wChannels;
		st_WaveFile.st_WaveFmtx.nSamplesPerSec = pst_Sound->pst_Wave->dwSamplesPerSec;
		st_WaveFile.st_WaveFmtx.nAvgBytesPerSec = pst_Sound->pst_Wave->dwAvgBytesPerSec;
		st_WaveFile.st_WaveFmtx.nBlockAlign = pst_Sound->pst_Wave->wBlockAlign;
		st_WaveFile.st_WaveFmtx.wBitsPerSample = pst_Sound->pst_Wave->wBitsPerSample;
		st_WaveFile.st_WaveFmtx.cbSize = pst_Sound->pst_Wave->cbSize;
		st_WaveFile.i_Size = pst_Sound->pst_Wave->ul_DataSize;
#ifdef JADEFUSION
		st_WaveFile.pbData = (BYTE *) pst_Sound->pst_Wave->ul_DataPosition;
#else
		st_WaveFile.pbData = (void *) pst_Sound->pst_Wave->ul_DataPosition;
#endif
		ediSND_gb_Duplicate = TRUE;
		*_ppst_DstSB = ediSND_pst_SB_Create(_pst_SpeData, &st_WaveFile, pst_Sound->pst_Wave->ul_DataPosition);
		ediSND_gb_Duplicate = FALSE;

		if(*_ppst_DstSB)
		{
			(*_ppst_DstSB)->i_Channel = _pst_SrcSB->i_Channel;
			(*_ppst_DstSB)->i_PanIdx = 64;
			(*_ppst_DstSB)->l_Volume = 0;
			(*_ppst_DstSB)->l_Pan = 0;
			(*_ppst_DstSB)->l_Frequency = _pst_SrcSB->l_Frequency;

			if(SND_gf_AutoVolumeOff)
				IDirectSoundBuffer8_SetVolume((*_ppst_DstSB)->pst_DSB, (*_ppst_DstSB)->l_Volume);
			else
				IDirectSoundBuffer8_SetVolume((*_ppst_DstSB)->pst_DSB, DSBVOLUME_MIN);

			IDirectSoundBuffer8_SetPan((*_ppst_DstSB)->pst_DSB, (*_ppst_DstSB)->l_Pan);
			IDirectSoundBuffer8_SetFrequency((*_ppst_DstSB)->pst_DSB, (*_ppst_DstSB)->l_Frequency);
		}

		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    search the original SB from the instance array
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if(pst_SI->pst_DSB != _pst_SrcSB) continue;

		pst_Sound = SND_gst_Params.dst_Sound + pst_SI->l_Sound;

		st_WaveFile.st_WaveFmtx.wFormatTag = pst_Sound->pst_Wave->wFormatTag;
		st_WaveFile.st_WaveFmtx.nChannels = pst_Sound->pst_Wave->wChannels;
		st_WaveFile.st_WaveFmtx.nSamplesPerSec = pst_Sound->pst_Wave->dwSamplesPerSec;
		st_WaveFile.st_WaveFmtx.nAvgBytesPerSec = pst_Sound->pst_Wave->dwAvgBytesPerSec;
		st_WaveFile.st_WaveFmtx.nBlockAlign = pst_Sound->pst_Wave->wBlockAlign;
		st_WaveFile.st_WaveFmtx.wBitsPerSample = pst_Sound->pst_Wave->wBitsPerSample;
		st_WaveFile.st_WaveFmtx.cbSize = pst_Sound->pst_Wave->cbSize;
		st_WaveFile.i_Size = pst_Sound->pst_Wave->ul_DataSize;
#ifdef JADEFUSION
		st_WaveFile.pbData = (BYTE *) pst_Sound->pst_Wave->ul_DataPosition;
#else
		st_WaveFile.pbData = (void *) pst_Sound->pst_Wave->ul_DataPosition;
#endif
		ediSND_gb_Duplicate = TRUE;
		*_ppst_DstSB = ediSND_pst_SB_Create(_pst_SpeData, &st_WaveFile, pst_Sound->pst_Wave->ul_DataPosition);
		ediSND_gb_Duplicate = FALSE;

		if(*_ppst_DstSB)
		{
			(*_ppst_DstSB)->i_Channel = _pst_SrcSB->i_Channel;
			(*_ppst_DstSB)->i_PanIdx = 64;
			(*_ppst_DstSB)->l_Volume = 0;
			(*_ppst_DstSB)->l_Pan = 0;
			(*_ppst_DstSB)->l_Frequency = _pst_SrcSB->l_Frequency;

			if(SND_gf_AutoVolumeOff)
				IDirectSoundBuffer8_SetVolume((*_ppst_DstSB)->pst_DSB, (*_ppst_DstSB)->l_Volume);
			else
				IDirectSoundBuffer8_SetVolume((*_ppst_DstSB)->pst_DSB, DSBVOLUME_MIN);

			IDirectSoundBuffer8_SetPan((*_ppst_DstSB)->pst_DSB, (*_ppst_DstSB)->l_Pan);
			IDirectSoundBuffer8_SetFrequency((*_ppst_DstSB)->pst_DSB, (*_ppst_DstSB)->l_Frequency);
		}

		return;
	}
#else
	/*~~~~~~~*/
	HRESULT hr;
	/*~~~~~~~*/

	*_ppst_DstSB = NULL;
	if(_pst_SrcSB->pst_SS) 
		return;

	*_ppst_DstSB = (SND_tdst_SoundBuffer *) MEM_p_Alloc(sizeof(SND_tdst_SoundBuffer));
	if(*_ppst_DstSB)
	{
		L_memset(*_ppst_DstSB, 0, sizeof(SND_tdst_SoundBuffer));
		hr = IDirectSound8_DuplicateSoundBuffer
			(
				_pst_SpeData->pst_DS,
				(IDirectSoundBuffer *) _pst_SrcSB->pst_DSB,
				(IDirectSoundBuffer **) &((*_ppst_DstSB)->pst_DSB)
			);
		ediSND_M_Assert(hr == DS_OK);
		(*_ppst_DstSB)->i_Channel = _pst_SrcSB->i_Channel;
		(*_ppst_DstSB)->i_PanIdx = 64;
		(*_ppst_DstSB)->l_Volume = 0;
		(*_ppst_DstSB)->l_Pan = 0;
		(*_ppst_DstSB)->l_Frequency = _pst_SrcSB->l_Frequency;

        IDirectSoundBuffer8_SetVolume((*_ppst_DstSB)->pst_DSB, (*_ppst_DstSB)->l_Volume);
        IDirectSoundBuffer8_SetPan((*_ppst_DstSB)->pst_DSB, (*_ppst_DstSB)->l_Pan);
        IDirectSoundBuffer8_SetFrequency((*_ppst_DstSB)->pst_DSB, (*_ppst_DstSB)->l_Frequency);
	}
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_Release(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Found;
	extern void BAS_bdeletegetval(ULONG, BAS_tdst_barray *, ULONG *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_SB == NULL) return;

	ediSND_FreeCoreBuffer(_pst_SB);
	BAS_bdeletegetval((ULONG) _pst_SB, &ediSND_gst_PlayingSB, &ul_Found);

	ediSND_FxDel(_pst_SB);

	if(_pst_SB->pst_SS)
	{
		ediSND_StreamRelease(_pst_SB);
	}
	else
	{
		SND_RamRastersDel((int) _pst_SB->pst_DSB);
		IDirectSoundBuffer8_Release(_pst_SB->pst_DSB);
	}

	MEM_Free(_pst_SB);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_SetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int _i_Pos)
{
	if(!_pst_SB) return;
	IDirectSoundBuffer8_SetCurrentPosition(_pst_SB->pst_DSB, _i_Pos);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_Pause(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~*/
	int i_status;
	/*~~~~~~~~~*/

	if(_pst_SB->pst_SS)
	{
		ediSND_StreamPause(_pst_SB);
	}
	else
	{
#ifdef JADEFUSION
		_pst_SB->pst_DSB->GetStatus((LPDWORD)&i_status);
#else
		IDirectSoundBuffer8_GetStatus(_pst_SB->pst_DSB, &i_status);
#endif
		if(i_status & SND_Cul_SBS_Playing)
		{
			IDirectSoundBuffer8_Stop(_pst_SB->pst_DSB);
			if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
			{
				IDirectSoundBuffer8_Stop(GetFxSB(_pst_SB)->pst_DSB);
			}
		}
		else
		{
			for(i_status = 0; i_status < SND_gst_Params.l_InstanceNumber; i_status++)
			{
				if(SND_gst_Params.dst_Instance[i_status].pst_DSB == _pst_SB) break;
			}

			ERR_X_Warning((i_status != SND_gst_Params.l_InstanceNumber), "pause failed (ediSND_SB_Pause)", NULL);
			i_status = (SND_gst_Params.dst_Instance[i_status].ul_Flags & SND_Cul_SF_PlayingLoop);
			i_status = i_status ? DSBPLAY_LOOPING : 0;
			IDirectSoundBuffer8_Play(_pst_SB->pst_DSB, 0, 0, i_status);

			if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
			{
				/*~~~~~~~~~~~~~~~~~~*/
				int ivol, ipan, ifreq;
				/*~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
                GetFxSB(_pst_SB)->pst_DSB->GetVolume((LPLONG)&ivol);
                GetFxSB(_pst_SB)->pst_DSB->GetFrequency((LPDWORD)&ifreq);
                GetFxSB(_pst_SB)->pst_DSB->GetPan((LPLONG)&ipan);
#else
				IDirectSoundBuffer8_GetVolume(GetFxSB(_pst_SB)->pst_DSB, &ivol);
				IDirectSoundBuffer8_GetFrequency(GetFxSB(_pst_SB)->pst_DSB, &ifreq);
				IDirectSoundBuffer8_GetPan(GetFxSB(_pst_SB)->pst_DSB, &ipan);
#endif
				if(SND_gf_AutoVolumeOff)
					IDirectSoundBuffer8_SetVolume(GetFxSB(_pst_SB)->pst_DSB, ivol);
				else
#ifdef JADEFUSION
                	GetFxSB(_pst_SB)->pst_DSB->SetVolume(DSBVOLUME_MIN);
                GetFxSB(_pst_SB)->pst_DSB->SetFrequency(ifreq);
                GetFxSB(_pst_SB)->pst_DSB->SetPan(ipan);


	            GetFxSB(_pst_SB)->pst_DSB->Play(0, 0, i_status);
#else
					IDirectSoundBuffer8_SetVolume(GetFxSB(_pst_SB)->pst_DSB, DSBVOLUME_MIN);

				IDirectSoundBuffer8_SetFrequency(GetFxSB(_pst_SB)->pst_DSB, ifreq);
				IDirectSoundBuffer8_SetPan(GetFxSB(_pst_SB)->pst_DSB, ipan);

				IDirectSoundBuffer8_Play(GetFxSB(_pst_SB)->pst_DSB, 0, 0, i_status);
#endif
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_SB_Play(SND_tdst_SoundBuffer *_pst_SB, int _i_Flag, int iFxL, int iFxR)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	HRESULT hr;
	float	fFxL, fFxR, fFx;
	int		iFxVol;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SB) return -1;
	if(_pst_SB->pst_SS) return -1;

	fFxL = SND_f_GetVolFromAtt(iFxL);
	fFxR = SND_f_GetVolFromAtt(iFxR);
	fFx = 0.5f * (fFxL + fFxR);
	if(fFx < 0.5f) fFx = 0.0f;	/* 0% wet + 100% dry */
	else if(fFx < 0.7f)
		fFx = 1.0f;				/* 100% wet + 100% dry */
	else
		fFx = 1.0f;				/* 100% wet + 0% dry */
	iFxVol = 0;					/* SND_l_GetAttFromVol(fFx); */

	if(_i_Flag & SND_Cul_SBC_PlayFxA)
	{
		if(ediSND_AllocCoreBuffer(_pst_SB, SND_Cte_FxCoreA) < 0) return -1;
		ediSND_FxAdd(_pst_SB, SND_Cte_FxCoreA, iFxVol);
	}
	else if(_i_Flag & SND_Cul_SBC_PlayFxB)
	{
		if(ediSND_AllocCoreBuffer(_pst_SB, SND_Cte_FxCoreB) < 0) return -1;
		ediSND_FxAdd(_pst_SB, SND_Cte_FxCoreB, iFxVol);
	}
	else
	{
		if(ediSND_AllocCoreBuffer(_pst_SB, -1) < 0) return -1;
		ediSND_FxDel(_pst_SB);
	}

	if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
	{
		/*~~~~~~~~~~~~~~~~~~*/
		int ivol, ipan, ifreq;
		/*~~~~~~~~~~~~~~~~~~*/
        IDirectSoundBuffer8_GetVolume(GetFxSB(_pst_SB)->pst_DSB, (LPLONG)&ivol);
        IDirectSoundBuffer8_GetFrequency(GetFxSB(_pst_SB)->pst_DSB, (LPDWORD)&ifreq);
        IDirectSoundBuffer8_GetPan(GetFxSB(_pst_SB)->pst_DSB, (LPLONG)&ipan);

		if(SND_gf_AutoVolumeOff)
			IDirectSoundBuffer8_SetVolume(GetFxSB(_pst_SB)->pst_DSB, ivol + _pst_SB->i_FxVol);
		else
			IDirectSoundBuffer8_SetVolume(GetFxSB(_pst_SB)->pst_DSB, DSBVOLUME_MIN);

		IDirectSoundBuffer8_SetFrequency(GetFxSB(_pst_SB)->pst_DSB, ifreq);
		IDirectSoundBuffer8_SetPan(GetFxSB(_pst_SB)->pst_DSB, ipan);

		hr = IDirectSoundBuffer8_Play
			(
				GetFxSB(_pst_SB)->pst_DSB,
				0,
				0,
				(_i_Flag & SND_Cul_SBC_PlayLooping) ? DSBPLAY_LOOPING : 0
			);
		ERR_X_Warning(hr == DS_OK, "[SND] play command failed", NULL);
	}

	hr = IDirectSoundBuffer8_Play(_pst_SB->pst_DSB, 0, 0, (_i_Flag & SND_Cul_SBC_PlayLooping) ? DSBPLAY_LOOPING : 0);
	ERR_X_Warning(hr == DS_OK, "[SND] play command failed", NULL);

	BAS_binsert((ULONG) _pst_SB, (ULONG) _pst_SB, &ediSND_gst_PlayingSB);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_Stop(SND_tdst_SoundBuffer *_pst_SB)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		ul_Found;
	extern void BAS_bdeletegetval(ULONG, BAS_tdst_barray *, ULONG *);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SB) return;
	BAS_bdeletegetval((ULONG) _pst_SB, &ediSND_gst_PlayingSB, &ul_Found);
	ediSND_FreeCoreBuffer(_pst_SB);

	if(_pst_SB->pst_SS)
	{
		ediSND_StreamStop(_pst_SB);
	}
	else
	{
        if(ul_Found)
		    IDirectSoundBuffer8_Stop(_pst_SB->pst_DSB);
		if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
		{
			IDirectSoundBuffer8_Stop(GetFxSB(_pst_SB)->pst_DSB);
		}
	}

	_pst_SB->ui_LastStatus = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_SetVolume(SND_tdst_SoundBuffer *_pst_SB, int _i_Vol)
{
	/*~~~~~~~~*/
	LONG	idx;
	/*~~~~~~~~*/

	if(!_pst_SB) return;

	_i_Vol += ediSND_gst_SoundManager.l_GlobalVol;

	if(SND_gst_Params.ul_RenderMode == SND_Cte_RenderStereo)
	{
		idx = _pst_SB->i_PanIdx;
		if(idx > 64)
		{
			_i_Vol += SNDPanTableL[127 - idx];
		}
		else if(idx < 64)
		{
			_i_Vol += SNDPanTableL[idx];
		}
		else
		{
			_i_Vol += SNDPanTableL[64];
		}
	}
	else
	{
		_i_Vol += SNDPanTableL[64];
	}

	if(_i_Vol > DSBVOLUME_MAX) _i_Vol = DSBVOLUME_MAX;
	if(_i_Vol < DSBVOLUME_MIN) _i_Vol = DSBVOLUME_MIN;
	if(_pst_SB->l_Volume != _i_Vol)
	{
		if(SND_gf_AutoVolumeOff)
			IDirectSoundBuffer8_SetVolume(_pst_SB->pst_DSB, _i_Vol);
		else
			IDirectSoundBuffer8_SetVolume(_pst_SB->pst_DSB, DSBVOLUME_MIN);

		if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
		{
			if(SND_gf_AutoVolumeOff)
				IDirectSoundBuffer8_SetVolume(GetFxSB(_pst_SB)->pst_DSB, _i_Vol + _pst_SB->i_FxVol);
			else
				IDirectSoundBuffer8_SetVolume(GetFxSB(_pst_SB)->pst_DSB, DSBVOLUME_MIN);
		}
	}

	_pst_SB->l_Volume = _i_Vol;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_SetFrequency(SND_tdst_SoundBuffer *_pst_SB, int _i_Freq)
{
	if(!_pst_SB) return;
	if(_i_Freq > DSBFREQUENCY_MAX) _i_Freq = DSBFREQUENCY_MAX;
	if(_i_Freq < DSBFREQUENCY_MIN) _i_Freq = DSBFREQUENCY_MIN;

	if(lAbs(_pst_SB->l_Frequency - _i_Freq)  >= 100)
	{
		IDirectSoundBuffer8_SetFrequency(_pst_SB->pst_DSB, _i_Freq);
		if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
		{
			IDirectSoundBuffer8_SetFrequency(GetFxSB(_pst_SB)->pst_DSB, _i_Freq);
		}
		_pst_SB->l_Frequency = _i_Freq;
	}

	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_SetPan(SND_tdst_SoundBuffer *_pst_SB, int _i_Pan, int front)
{
	/*~~~~~~~~~~~~~*/
	LONG	l_Volume;
	LONG	l_Pan;
	LONG	idx;
	float	f;
	/*~~~~~~~~~~~~~*/

	if(!_pst_SB) return;

	/* get user volume settings */
	IDirectSoundBuffer8_GetVolume(_pst_SB->pst_DSB, &l_Volume);
	idx = _pst_SB->i_PanIdx;
	if(idx > 64)
	{
		l_Volume = l_Volume - SNDPanTableL[127 - idx];
	}
	else if(idx < 64)
	{
		l_Volume = l_Volume - SNDPanTableL[idx];
	}
	else
	{
		l_Volume = l_Volume - SNDPanTableL[64];
	}

	/* eval the new corrected pan and volume */
	if(_i_Pan > 0)
	{
		f = SND_f_GetVolFromAtt(-_i_Pan);
		idx = (int) (127.0f - (63.0f * f));

		if(idx < 0) idx = 0;
		if(idx > 127) idx = 127;
		_pst_SB->i_PanIdx = idx;

		l_Pan = -SNDPanTableL[idx] + SNDPanTableL[127 - idx];
		l_Volume = l_Volume + SNDPanTableL[127 - idx];
	}
	else if(_i_Pan < 0)
	{
		f = SND_f_GetVolFromAtt(_i_Pan);
		idx = (int) (64.0f * f);
		if(idx < 0) idx = 0;
		if(idx > 127) idx = 127;
		_pst_SB->i_PanIdx = idx;

		l_Pan = SNDPanTableL[127 - idx] - SNDPanTableL[idx];
		l_Volume = l_Volume + SNDPanTableL[idx];
	}
	else
	{
		_pst_SB->i_PanIdx = 64;
		l_Pan = 0;
		l_Volume = l_Volume + SNDPanTableL[64];
	}

	if(SND_gst_Params.ul_RenderMode == SND_Cte_RenderMono)
	{
		/* if mono => no panning */
		if(_pst_SB->l_Pan != DSBPAN_CENTER)
		{
			IDirectSoundBuffer8_SetPan(_pst_SB->pst_DSB, DSBPAN_CENTER);
			if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
			{
				IDirectSoundBuffer8_SetPan(GetFxSB(_pst_SB)->pst_DSB, DSBPAN_CENTER);
			}
		}

		_pst_SB->l_Pan = DSBPAN_CENTER;
	}
	else
	{
		/* if stereo => panning */
		if(l_Volume > DSBVOLUME_MAX) l_Volume = DSBVOLUME_MAX;
		if(l_Volume < DSBVOLUME_MIN) l_Volume = DSBVOLUME_MIN;
		if(l_Pan > DSBPAN_RIGHT) l_Pan = DSBPAN_RIGHT;
		if(l_Pan < DSBPAN_LEFT) l_Pan = DSBPAN_LEFT;

		if(_pst_SB->l_Pan != l_Pan)
		{
			IDirectSoundBuffer8_SetPan(_pst_SB->pst_DSB, l_Pan);
			if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
			{
				IDirectSoundBuffer8_SetPan(GetFxSB(_pst_SB)->pst_DSB, l_Pan);
			}
		}

		if(_pst_SB->l_Volume != l_Volume)
		{
			if(SND_gf_AutoVolumeOff)
				IDirectSoundBuffer8_SetVolume(_pst_SB->pst_DSB, l_Volume);
			else
				IDirectSoundBuffer8_SetVolume(_pst_SB->pst_DSB, DSBVOLUME_MIN);

			if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
			{
				if(SND_gf_AutoVolumeOff)
					IDirectSoundBuffer8_SetVolume(GetFxSB(_pst_SB)->pst_DSB, l_Volume + _pst_SB->i_FxVol);
				else
					IDirectSoundBuffer8_SetVolume(GetFxSB(_pst_SB)->pst_DSB, DSBVOLUME_MIN);
			}
		}

		_pst_SB->l_Pan = l_Pan;
		_pst_SB->l_Volume = l_Volume;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_SB_GetStatus(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Status)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			ret;
	extern void ediSND_StreamUpdateStatus(SND_tdst_SoundBuffer *_pst_SB);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_SB) return 0;
	ret = 0;

	if(SND_gb_EdiPause)
	{
		*_pi_Status = (int) _pst_SB->ui_LastStatus;
	}
	else
	{
		if(_pst_SB->pst_SS) ediSND_StreamUpdateStatus(_pst_SB);
#ifdef JADEFUSION
		ret = _pst_SB->pst_DSB->GetStatus((LPDWORD)_pi_Status);
#else
		ret = IDirectSoundBuffer8_GetStatus(_pst_SB->pst_DSB, _pi_Status);
#endif
		if(_pst_SB->pst_SS && *_pi_Status & SND_Cul_SBS_Playing) *_pi_Status |= SND_Cul_SF_StreamIsPlayingNow;

		_pst_SB->ui_LastStatus = (unsigned int) *_pi_Status;

		if(GetFxSB(_pst_SB) && GetFxInterface(_pst_SB))
		{
			/*~~~~~~~*/
			int status;
			/*~~~~~~~*/

		    IDirectSoundBuffer8_GetStatus(GetFxSB(_pst_SB)->pst_DSB, (LPDWORD)&status);
			if(status & SND_Cul_SBS_Playing)
			{
				_pst_SB->ui_LastStatus |= SND_Cul_SBS_Playing;
				(*_pi_Status) |= SND_Cul_SBS_Playing;
			}
		}
	}

	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_GetFrequency(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Freq)
{
	if(!_pst_SB) return;
#ifdef JADEFUSION
	_pst_SB->pst_DSB->GetFrequency((LPDWORD)_pi_Freq);
#else
	IDirectSoundBuffer8_GetFrequency(_pst_SB->pst_DSB, _pi_Freq);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_GetPan(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pan, int *_pi_front)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	if(!_pst_SB) return;

	*_pi_front = 0;
	*_pi_Pan = 0;

	if(SND_gst_Params.ul_RenderMode == SND_Cte_RenderStereo)
	{
		if(_pst_SB->i_PanIdx > 64)
		{
			f = (127.0f - (float) _pst_SB->i_PanIdx) / 63.0f;
			*_pi_Pan = SND_l_GetAttFromVol(f);
		}
		else if(_pst_SB->i_PanIdx < 64)
		{
			f = (float) _pst_SB->i_PanIdx / 64.0f;
			*_pi_Pan = SND_l_GetAttFromVol(f);
		}
		else
		{
			*_pi_Pan = 0;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SB_GetCurrPos(SND_tdst_SoundBuffer *_pst_SB, int *_pi_Pos, int *_pi_Write)
{
	if(_pst_SB->pst_SS)
	{
		ediSND_StreamGetCurrPos(_pst_SB, _pi_Pos, _pi_Write);
	}
	else
	{
		IDirectSoundBuffer8_GetCurrentPosition( _pst_SB->pst_DSB, ( LPDWORD ) _pi_Pos, ( LPDWORD ) _pi_Write );
	}
	
	if(_pi_Pos)
	{
		*_pi_Pos = SND_ui_SizeToSample
			(
		        ( unsigned short ) _pst_SB->i_Format,
				(unsigned short) _pst_SB->i_Channel,
				*_pi_Pos
			);
	}

	if(_pi_Write)
	{
		*_pi_Write = SND_ui_SizeToSample
			(
		        ( unsigned short ) _pst_SB->i_Format,
				(unsigned short) _pst_SB->i_Channel,
				*_pi_Write
			);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_SetGlobalVol(LONG _l_Volume)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ediSND_gst_SoundManager.l_GlobalVol == _l_Volume) return;
	if(_l_Volume < DSBVOLUME_MIN) _l_Volume = DSBVOLUME_MIN;
	if(_l_Volume > DSBVOLUME_MAX) _l_Volume = DSBVOLUME_MAX;

	ediSND_gst_SoundManager.l_GlobalVol = _l_Volume;

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pst_SI = SND_gst_Params.dst_Instance + i;
		if((pst_SI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		SND_SetInstVolume(pst_SI);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG ediSND_l_GetGlobalVol(void)
{
	return ediSND_gst_SoundManager.l_GlobalVol;
}

/*$4
 ***********************************************************************************************************************
    only debug
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_i_ChangeRenderMode(int _i_Mode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	int						l_Volume, l_Pan;
	int						idx;
	SND_tdst_SoundBuffer	*pSB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_gst_Params.ul_RenderMode = (ULONG) _i_Mode;

	for(i = 0; i < ediSND_gst_PlayingSB.num; i++)
	{
		if(ediSND_gst_PlayingSB.base[i].ul_Key == -1) continue;
		pSB = (SND_tdst_SoundBuffer *) ediSND_gst_PlayingSB.base[i].ul_Key;

		IDirectSoundBuffer8_GetVolume( pSB->pst_DSB, ( LPLONG ) &l_Volume );

		idx = pSB->i_PanIdx;

		if(SND_gst_Params.ul_RenderMode == SND_Cte_RenderMono)
		{
			if(idx > 64)
			{
				l_Volume = l_Volume - SNDPanTableL[127 - idx];
			}
			else if(idx < 64)
			{
				l_Volume = l_Volume - SNDPanTableL[idx];
			}
			else
			{
				l_Volume = l_Volume - SNDPanTableL[64];
			}

			l_Pan = 0;
			l_Volume = l_Volume + SNDPanTableL[64];
		}
		else
		{
			l_Volume = l_Volume - SNDPanTableL[64];

			if(idx > 64)
			{
				l_Pan = -SNDPanTableL[idx] + SNDPanTableL[127 - idx];
				l_Volume = l_Volume + SNDPanTableL[127 - idx];
			}
			else if(idx < 64)
			{
				l_Pan = SNDPanTableL[127 - idx] - SNDPanTableL[idx];
				l_Volume = l_Volume + SNDPanTableL[idx];
			}
			else
			{
				l_Pan = 0;
				l_Volume = l_Volume + SNDPanTableL[64];
			}
		}

		if(pSB->l_Pan != l_Pan)
		{
			IDirectSoundBuffer8_SetPan(pSB->pst_DSB, l_Pan);
			if(GetFxSB(pSB) && GetFxInterface(pSB))
			{
				IDirectSoundBuffer8_SetPan(GetFxSB(pSB)->pst_DSB, l_Pan);
			}
		}

		if(pSB->l_Volume != l_Volume)
		{
			if(SND_gf_AutoVolumeOff)
				IDirectSoundBuffer8_SetVolume(pSB->pst_DSB, l_Volume);
			else
				IDirectSoundBuffer8_SetVolume(pSB->pst_DSB, DSBVOLUME_MIN);

			if(GetFxSB(pSB) && GetFxInterface(pSB))
			{
				if(SND_gf_AutoVolumeOff)
					IDirectSoundBuffer8_SetVolume(GetFxSB(pSB)->pst_DSB, l_Volume + pSB->i_FxVol);
				else
					IDirectSoundBuffer8_SetVolume(GetFxSB(pSB)->pst_DSB, DSBVOLUME_MIN);
			}
		}

		pSB->l_Pan = l_Pan;
		pSB->l_Volume = l_Volume;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_CommitChange(MATH_tdst_Vector *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pSI;
	int						i, ll;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_gst_Params.l_InstanceNumber; i++)
	{
		pSI = SND_gst_Params.dst_Instance + i;
		if((pSI->ul_Flags & SND_Cul_DSF_Used) == 0) continue;

		/* update prefecth state */
		switch(SND_i_StreamSoundIsPrefetched(pSI->l_Sound))
		{
		case 1:
			pSI->ul_EdiFlags &= ~(SND_EdiFlg_EdiPrefetched | SND_EdiFlg_EdiPrefetchedUsed | SND_EdiFlg_EdiPrefetching);
			pSI->ul_EdiFlags |= SND_EdiFlg_EdiPrefetching;
			break;

		case 2:
		case 3:
			pSI->ul_EdiFlags &= ~(SND_EdiFlg_EdiPrefetched | SND_EdiFlg_EdiPrefetchedUsed | SND_EdiFlg_EdiPrefetching);
			pSI->ul_EdiFlags |= SND_EdiFlg_EdiPrefetched;
			break;

		default:
			if(pSI->ul_EdiFlags & SND_EdiFlg_EdiPrefetched)
			{
				pSI->ul_EdiFlags &= ~(SND_EdiFlg_EdiPrefetched | SND_EdiFlg_EdiPrefetchedUsed | SND_EdiFlg_EdiPrefetching);
				pSI->ul_EdiFlags |= SND_EdiFlg_EdiPrefetchedUsed;
			}
			break;
		}

		/* update play state */
		pSI->pst_LI->pfi_SndBuffGetStatus(pSI->pst_DSB, &ll);
		if(ll & SND_Cul_SBS_Playing) continue;

		pSI->ul_Flags &= ~(SND_Cul_SF_Playing | SND_Cul_SF_PlayingLoop);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ediSND_AllocCoreBuffer(SND_tdst_SoundBuffer *_pst_SB, int core)
{
	/*~~*/
	int i;
	/*~~*/

	switch(core)
	{
	case SND_Cte_FxCoreA:
		if(ediSND_dst_CoreUser[SND_Cte_FxCoreA].num + _pst_SB->i_Channel > SND_Cte_FxMaxVoiceNbByCore)
		{
			/*
			 * ERR_X_Warning(0, "[SND] Overflow of max number of Fx buffer played by core A
			 * (play failed)", NULL);
			 */
			if(ediSND_dst_CoreUser[SND_Cte_FxCoreB].num + _pst_SB->i_Channel > SND_Cte_FxMaxVoiceNbByCore)
			{
				ERR_X_Warning(0, "[SND] No more free buffer (play failed)", NULL);
				return -1;
			}
			else
				return ediSND_AllocCoreBuffer(_pst_SB, SND_Cte_FxCoreB);
		}

		BAS_binsert((ULONG) _pst_SB, (ULONG) _pst_SB->i_Channel, &ediSND_dst_CoreUser[SND_Cte_FxCoreA]);
		break;

	case SND_Cte_FxCoreB:
		if(ediSND_dst_CoreUser[SND_Cte_FxCoreB].num + _pst_SB->i_Channel > SND_Cte_FxMaxVoiceNbByCore)
		{
			/*
			 * ERR_X_Warning(0, "[SND] Overflow of max number of Fx buffer played by core B
			 * (play failed)", NULL);
			 */
			if(ediSND_dst_CoreUser[SND_Cte_FxCoreA].num + _pst_SB->i_Channel > SND_Cte_FxMaxVoiceNbByCore)
			{
				ERR_X_Warning(0, "[SND] No more free buffer (play failed)", NULL);
				return -1;
			}
			else
				return ediSND_AllocCoreBuffer(_pst_SB, SND_Cte_FxCoreA);
		}

		BAS_binsert((ULONG) _pst_SB, (ULONG) _pst_SB->i_Channel, &ediSND_dst_CoreUser[SND_Cte_FxCoreB]);
		break;

	default:
		core =
			(ediSND_dst_CoreUser[SND_Cte_FxCoreB].num > ediSND_dst_CoreUser[SND_Cte_FxCoreA].num)
				? SND_Cte_FxCoreA : SND_Cte_FxCoreB;

		if(ediSND_dst_CoreUser[core].num + _pst_SB->i_Channel > SND_Cte_FxMaxVoiceNbByCore) core = 1 - core;

		if(ediSND_dst_CoreUser[core].num + _pst_SB->i_Channel > SND_Cte_FxMaxVoiceNbByCore)
		{
			ERR_X_Warning(0, "[SND] No more free buffer (play failed)", NULL);
			return -1;
		}

		BAS_binsert((ULONG) _pst_SB, (ULONG) _pst_SB->i_Channel, &ediSND_dst_CoreUser[core]);
		break;
	}

	ediSND_gst_SoundManager.i_Playing = 0;
	for(i = 0; i < ediSND_dst_CoreUser[SND_Cte_FxCoreA].num; i++)
		ediSND_gst_SoundManager.i_Playing += ediSND_dst_CoreUser[SND_Cte_FxCoreA].base[i].ul_Val;

	for(i = 0; i < ediSND_dst_CoreUser[SND_Cte_FxCoreB].num; i++)
		ediSND_gst_SoundManager.i_Playing += ediSND_dst_CoreUser[SND_Cte_FxCoreB].base[i].ul_Val;

	if(ediSND_gst_SoundManager.i_Playing > ediSND_gst_SoundManager.i_PlayingMax)
		ediSND_gst_SoundManager.i_PlayingMax = ediSND_gst_SoundManager.i_Playing;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ediSND_FreeCoreBuffer(SND_tdst_SoundBuffer *_pst_SB)
{
	BAS_bdelete((ULONG) _pst_SB, &ediSND_dst_CoreUser[SND_Cte_FxCoreA]);
	BAS_bdelete((ULONG) _pst_SB, &ediSND_dst_CoreUser[SND_Cte_FxCoreB]);
	ediSND_gst_SoundManager.i_Playing = ediSND_dst_CoreUser[SND_Cte_FxCoreA].num + ediSND_dst_CoreUser[SND_Cte_FxCoreB].num;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* PSX2_TARGET */
