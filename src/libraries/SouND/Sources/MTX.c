/*$T MTX.c GC! 1.081 07/29/02 12:20:12 */


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
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGread.h"
#include "BASe/MEMory/MEM.h"
#include "IOP/RPC_Manager.h"
#include "BIGfiles/BIGkey.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/BIGread.h"
#include "BASe/MEMory/MEM.h"
#include "Sound/Sources/SNDwave.h"
#include "Sound/Sources/SNDconv.h"
#include "Sound/Sources/SNDconst.h"
#include "BASe/CLIbrary/CLImem.h"
#endif
#include "MTX.h"

#ifdef PSX2_TARGET
#include "GS_PS2/GSP_Video.h"
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
#define M_EdiRedWarning(__str) \
	{ \
		char	az_msg[1024]; \
		sprintf(az_msg, "[MTX] %s", __str); \
		ERR_X_Warning(0, az_msg, NULL); \
	}
#define M_EdiBlackWarning(__str) \
	{ \
		char	az_msg[1024]; \
		sprintf(az_msg, "[WARNING][MTX] %s", __str); \
		LINK_PrintStatusMsg(az_msg); \
	}
#else
#define M_EdiRedWarning(__str)
#define M_EdiBlackWarning(__str)
#endif

/*$4
 ***********************************************************************************************************************
    tmp
 ***********************************************************************************************************************
 */

extern char				*BIG_pc_ReadPartOfFileTmp(ULONG _ul_Pos, ULONG ul_ReadSize);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

MTX_tdst_GlobalStruct	MTX_gst_Global;

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

#ifdef PSX2_TARGET
static void ps2MTX_StartMtx(void);
static void ps2MTX_RestartMtx(void);
static void ps2MTX_KillMtx(void);
#endif
static void MTX_DefaultStartMtx(void);
static void MTX_DefaultRestartMtx(void);
static void MTX_DefaultKillMtx(void);


/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MTX_InitModule(void)
{
	L_memset(&MTX_gst_Global, 0, sizeof(MTX_tdst_GlobalStruct));

	MTX_gst_Global.i.pfv_StartMtx = MTX_DefaultStartMtx;
	MTX_gst_Global.i.pfv_RestartMtx = MTX_DefaultRestartMtx;
	MTX_gst_Global.i.pfv_KillMtx = MTX_DefaultKillMtx;

#ifdef PSX2_TARGET
	MTX_gst_Global.i.pfv_StartMtx = ps2MTX_StartMtx;
	MTX_gst_Global.i.pfv_RestartMtx = ps2MTX_RestartMtx;
	MTX_gst_Global.i.pfv_KillMtx = ps2MTX_KillMtx;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MTX_CloseModule(void)
{
	L_memset(&MTX_gst_Global, 0, sizeof(MTX_tdst_GlobalStruct));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG MTX_ul_CallbackLoadFile(ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MTX_tdst_MtxFileHeader	st_FileHeader;
	ULONG					ul_CurrentPos;
	char					*pc_Buffer,*pc;
	ULONG					_ul_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	if(_ul_Key == -1) return 0;

	_ul_Pos = BIG_ul_SearchKeyToPos(_ul_Key);
	if(_ul_Pos == -1)
	{
		M_EdiRedWarning("Loading failed, bad position, check the BigKey");
		return -1;
	}

	// bin + clean final => file is absent
	if(_ul_Pos == _ul_Key) return -1;
	
	// file is not complete
	if(BIG_ul_GetLengthFile(_ul_Pos) == 8) return -1; 
	

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    file header
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ul_CurrentPos = _ul_Pos + 4;
	
	//pc = pc_Buffer = MEM_p_Alloc(sizeof(MTX_tdst_MtxFileHeader));
	pc = pc_Buffer = (char*)MEM_p_Alloc(sizeof(MTX_tdst_MtxFileHeader));
	BIG_Read(ul_CurrentPos, pc_Buffer, sizeof(MTX_tdst_MtxFileHeader));
	
	ul_CurrentPos += sizeof(MTX_tdst_MtxFileHeader);

	st_FileHeader.ui_FileID = LOA_ReadULong(&pc_Buffer);
	st_FileHeader.ui_FileVersion = LOA_ReadULong(&pc_Buffer);
	st_FileHeader.ui_FileSize = LOA_ReadULong(&pc_Buffer);

	if(st_FileHeader.ui_FileID != MTX_Cte_FileId)
	{
		M_EdiRedWarning("Loading failed, the file is not an MTX");
		MEM_Free(pc);
		return 0;
	}

	if(st_FileHeader.ui_FileVersion != MTX_Cte_FileVersion)
	{
		M_EdiRedWarning("Loading failed, the file version is incorrect");
		MEM_Free(pc);
		return 0;
	}

	MTX_gst_Global.ui_FileKey = _ul_Key;
	MTX_gst_Global.ui_FilePos = _ul_Pos + 4;
	MTX_gst_Global.ui_FileSize = st_FileHeader.ui_FileSize;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    get video sub headers
	 -------------------------------------------------------------------------------------------------------------------
	 */

	MTX_gst_Global.st_VideoHeader.ui_VideoBufferSize = LOA_ReadULong(&pc_Buffer);
	MTX_gst_Global.st_VideoHeader.ui_FileSize = LOA_ReadULong(&pc_Buffer);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    get sound sub header
	 -------------------------------------------------------------------------------------------------------------------
	 */

	MTX_gst_Global.st_SoundHeader.ui_SoundBufferSize = LOA_ReadULong(&pc_Buffer);
	MTX_gst_Global.st_SoundHeader.ui_Channel = LOA_ReadULong(&pc_Buffer);
	MTX_gst_Global.st_SoundHeader.ui_DataSize = LOA_ReadULong(&pc_Buffer);
	MTX_gst_Global.st_SoundHeader.ui_Freq = LOA_ReadULong(&pc_Buffer);
	MTX_gst_Global.st_SoundHeader.f_Vol = LOA_ReadFloat(&pc_Buffer);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */

	MTX_gst_Global.ui_DataPosition = ul_CurrentPos;
	MTX_gst_Global.ui_CurrentValidity = 1;

    MEM_Free(pc);
	return (ULONG) &MTX_gst_Global;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MTX_Start(void)
{
	if(!MTX_gst_Global.ui_CurrentValidity) return;
	MTX_gst_Global.i.pfv_StartMtx();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MTX_Restart(void)
{
	if(!MTX_gst_Global.ui_CurrentValidity) return;
	MTX_gst_Global.i.pfv_RestartMtx();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MTX_Kill(void)
{
	if(!MTX_gst_Global.ui_CurrentValidity) return;
	MTX_gst_Global.i.pfv_KillMtx();
	MTX_gst_Global.ui_CurrentValidity = 0;
	MTX_gst_Global.ui_FilePos = 0;
	MTX_gst_Global.ui_FileSize = 0;
	MTX_gst_Global.st_VideoHeader.ui_VideoBufferSize = 0;
	MTX_gst_Global.st_VideoHeader.ui_FileSize = 0;
	MTX_gst_Global.st_SoundHeader.ui_SoundBufferSize = 0;
	MTX_gst_Global.st_SoundHeader.ui_Channel = 0;
	MTX_gst_Global.st_SoundHeader.ui_DataSize = 0;
	MTX_gst_Global.st_SoundHeader.ui_Freq = 0;
	MTX_gst_Global.st_SoundHeader.f_Vol = 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL MTX_b_CurrentFileIsMTX(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return TRUE;
	else
		return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int MTX_ui_GetVideoBufferSize(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return MTX_gst_Global.st_VideoHeader.ui_VideoBufferSize;
	else
		return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int MTX_ui_GetVideoFilePosition(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return MTX_gst_Global.ui_FilePos;
	else
		return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int MTX_ui_GetVideoFileSize(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return MTX_gst_Global.st_VideoHeader.ui_FileSize;
	else
		return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int MTX_ui_GetStreamPosition(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return MTX_gst_Global.ui_DataPosition;
	else
		return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int MTX_ui_GetStreamSize(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return(MTX_gst_Global.ui_FileSize - (MTX_gst_Global.ui_DataPosition - MTX_gst_Global.ui_FilePos));
	else
		return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int MTX_ui_GetSoundBufferSize(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return MTX_gst_Global.st_SoundHeader.ui_SoundBufferSize;
	else
		return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int MTX_ui_GetSoundFrequency(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return MTX_gst_Global.st_SoundHeader.ui_Freq;
	else
		return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int MTX_ui_GetSoundChannelNb(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return MTX_gst_Global.st_SoundHeader.ui_Channel;
	else
		return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float MTX_f_GetSoundVolume(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
	{
		return MTX_gst_Global.st_SoundHeader.f_Vol;
	}
	else
		return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int MTX_ui_GetSoundDataSize(void)
{
	if(MTX_gst_Global.ui_CurrentValidity)
		return MTX_gst_Global.st_SoundHeader.ui_DataSize;
	else
		return 0;
}

/*$4
 ***********************************************************************************************************************
    TARGET SPECIFIC FUNCTIONS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    DEFAULT
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void MTX_DefaultStartMtx(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void MTX_DefaultRestartMtx(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void MTX_DefaultKillMtx(void)
{
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    PSX2
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef PSX2_TARGET

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ps2MTX_StartMtx(void)
{
	GSP_VideoFullScreenCreate(MTX_gst_Global.ui_FileKey);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ps2MTX_RestartMtx(void)
{
	GSP_VideoFullScreenRestart();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void ps2MTX_KillMtx(void)
{
}

#endif

/*$4
 ***********************************************************************************************************************
    input file parser
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int MTX_i_ParseSoundFile(MTX_tdst_SoundInfo *pst_SoundInfo, ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Pos;
	ULONG				ul_Size;
	char				*pc_Buffer;
	SND_tdst_WaveData	st_Wave;
	WAVEFORMATEX		*p_WAVEFORMATEX;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Key == -1) return -1;

	ul_Pos = BIG_ul_SearchKeyToPos(_ul_Key);
	if((ul_Pos == -1) || (ul_Pos == _ul_Key)) return -2;

	ul_Size = BIG_ul_GetLengthFile(ul_Pos);

	pc_Buffer = (char *) L_malloc(ul_Size);
	if(!pc_Buffer) return -3;

	BIG_Read(ul_Pos + 4, pc_Buffer, ul_Size);
	if(!SND_b_WAV_ParseMemory(pc_Buffer, &p_WAVEFORMATEX, &st_Wave.pbData, (ULONG *) &st_Wave.i_Size, NULL))
	{
		L_free(pc_Buffer);
		return -4;
	}

	if(p_WAVEFORMATEX->wFormatTag != WAVE_FORMAT_PS2)
	{
		ERR_X_Warning(0, "Sound file is not converted into PS2 format, operation failed", NULL);
		L_free(pc_Buffer);
		return -5;
	}

	pst_SoundInfo->ui_FileKey = _ul_Key;
	pst_SoundInfo->ui_FilePos = ul_Pos;
	pst_SoundInfo->ui_FileSize = ul_Size;
	pst_SoundInfo->pc_Buffer = pc_Buffer;
	pst_SoundInfo->ui_Channel = p_WAVEFORMATEX->nChannels;
	pst_SoundInfo->ui_OctBySeconds = p_WAVEFORMATEX->nSamplesPerSec * p_WAVEFORMATEX->wBitsPerSample / 8;
	pst_SoundInfo->f_Duration = fLongToFloat(st_Wave.i_Size) / fLongToFloat(pst_SoundInfo->ui_OctBySeconds * pst_SoundInfo->ui_Channel);
	pst_SoundInfo->i_Freq = p_WAVEFORMATEX->nSamplesPerSec;
	pst_SoundInfo->ui_Channel = p_WAVEFORMATEX->nChannels;
	pst_SoundInfo->ui_DataPos = ul_Pos + (unsigned int) st_Wave.pbData - (unsigned int) pc_Buffer;
	pst_SoundInfo->ui_DataSize = st_Wave.i_Size / p_WAVEFORMATEX->nChannels;
	pst_SoundInfo->pc_Data = (char *) st_Wave.pbData;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int MTX_i_ParseVideoFile(MTX_tdst_VideoInfo *pst_VideoInfo, ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Pos, ul_Size, ul_SizeOfFrameCounter;
	ULONG				*p_TempBuffer;
	UCHAR				*p_FinalFrameFinder;
	ULONG				LastRead;
	int					i_ReadSize, i_TrueReadSize;
	MTX_tdst_IPUHeader	st_Header;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Pos = BIG_ul_SearchKeyToPos(_ul_Key);
	if(ul_Pos != (ULONG) - 1)
	{
		ul_Size = BIG_ul_GetLengthFile(ul_Pos);
		ul_Pos += 4;

		BIG_Read(ul_Pos, (void *) &st_Header, sizeof(MTX_tdst_IPUHeader));

		if(*(int *) &st_Header.asz_ID != MTX_Cte_VideoFileId)
		{
			return -1;
		}

		i_ReadSize = 8192 * 4;
		p_TempBuffer = (ULONG *) L_malloc(i_ReadSize);

		pst_VideoInfo->ui_FileKey = _ul_Key;
		pst_VideoInfo->ui_FilePos = ul_Pos;
		pst_VideoInfo->ui_FileSize = ul_Size;
		pst_VideoInfo->ui_ImageNb = st_Header.ui_NumberOfFrames;
		pst_VideoInfo->aui_ImageOffset = (unsigned int *) L_malloc((1+st_Header.ui_NumberOfFrames) * sizeof(unsigned int));

		L_memset(pst_VideoInfo->aui_ImageOffset, 0, st_Header.ui_NumberOfFrames * sizeof(unsigned int));
		pst_VideoInfo->aui_ImageSize = (unsigned int *) L_malloc((1+st_Header.ui_NumberOfFrames) * sizeof(unsigned int));
		L_memset(pst_VideoInfo->aui_ImageSize, 0, st_Header.ui_NumberOfFrames * sizeof(unsigned int));

		ul_Size -= sizeof(MTX_tdst_VideoHeader);
		ul_Pos += sizeof(MTX_tdst_VideoHeader);

		pst_VideoInfo->ui_AvgOctBySeconds = (ul_Size) / (st_Header.ui_NumberOfFrames);

		ul_SizeOfFrameCounter = 0;
		LastRead = 0;

		i = 0;
		pst_VideoInfo->aui_ImageOffset[i] = 0;
		while(ul_Size)
		{
			BIG_Read(ul_Pos, (void *) p_TempBuffer, i_ReadSize);
			i_ReadSize = i_TrueReadSize = lMin(ul_Size, i_ReadSize);
			p_FinalFrameFinder = (UCHAR *) p_TempBuffer;

			while(i_TrueReadSize--)
			{
				if(*(unsigned int *) p_FinalFrameFinder == MTX_Cte_FrameDelimiter)
				{
					pst_VideoInfo->aui_ImageSize[i] = LastRead;
					pst_VideoInfo->aui_ImageOffset[i + 1] = pst_VideoInfo->aui_ImageOffset[i] + LastRead;
					i++;
					pst_VideoInfo->ui_ImageSizeMax = lMax(pst_VideoInfo->ui_ImageSizeMax, LastRead);
					LastRead = 0;
				}
				else
					LastRead++;

				p_FinalFrameFinder++;
			}

			ul_Size -= lMin(ul_Size, i_ReadSize);
			ul_Pos += lMin(ul_Size, i_ReadSize);
		}

		L_free(p_TempBuffer);
		return 0;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MTX_FreeVideoFileInfo(MTX_tdst_VideoInfo *pst_VideoInfo)
{
	if(!pst_VideoInfo) return;

	if(pst_VideoInfo->aui_ImageOffset) L_free(pst_VideoInfo->aui_ImageOffset);
	pst_VideoInfo->aui_ImageOffset = NULL;

	if(pst_VideoInfo->aui_ImageSize) L_free(pst_VideoInfo->aui_ImageSize);
	pst_VideoInfo->aui_ImageSize = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MTX_FreeSoundFileInfo(MTX_tdst_SoundInfo *pst_SoundInfo)
{
	if(!pst_SoundInfo) return;

	if(pst_SoundInfo->pc_Buffer) L_free(pst_SoundInfo->pc_Buffer);
	pst_SoundInfo->pc_Buffer = NULL;
}

#endif /* ACTIVE_EDITORS */

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

