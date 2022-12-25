/*$T SNDwave.c GC 1.138 07/21/05 15:46:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    Headers
 ***********************************************************************************************************************
 */

#include "Precomp.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "SNDwave.h"
#include "SNDstruct.h"
#include "SNDconst.h"

#ifdef PSX2_TARGET
#include "IOP/RPC_Manager.h"
#include "IOP/ee/eeDebug.h"
#include <eekernel.h>
#endif

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static int	SND_i_WaveReadltxtCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc *pWave);
static int	SND_i_WaveReadnoteCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc *pWave);
static int	SND_i_WaveReadplstCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc *pWave);
static int	SND_i_WaveReadcueCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc *pWave);
static int	SND_i_WaveReadfmtCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc **_ppWave);

/*$4
 ***********************************************************************************************************************
    Public functions
 ***********************************************************************************************************************
 */

SND_tdst_WaveDesc		SND_gst_TruncatedWave;
SND_tdst_CuePointDesc	SND_gst_CuePoints;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void SND_WaveInitModule(void)
{
	L_memset(&SND_gst_TruncatedWave, 0, sizeof(SND_tdst_WaveDesc));
	L_memset(&SND_gst_CuePoints, 0, sizeof(SND_tdst_CuePointDesc));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_WaveReadfmtCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc **_ppWave)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UI32							uiCkSize;
	SND_tdst_WAVEfmtCk_Partcommon	stfmtCk;
	UI16							wBitsPerSample;
	UI16							cbSize;
	UI16							wSamplesPerBlock;
	UI16							wNumCoef, i;
	SND_tdst_WaveDesc				*pWave;
	SND_tdst_MS_ADPCM_COEFF			*aCoeff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- chunck size ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uiCkSize = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);
	if(uiCkSize < sizeof(SND_tdst_WAVEfmtCk_Partcommon)) return -1;

	/*$1- mandatory part ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	stfmtCk.wFormatTag = LOA_ReadUShort(_ppcBuff);
	*_pulOffset += sizeof(USHORT);

	stfmtCk.wChannels = LOA_ReadUShort(_ppcBuff);
	*_pulOffset += sizeof(USHORT);

	stfmtCk.dwSamplesPerSec = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	stfmtCk.dwAvgBytesPerSec = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	stfmtCk.wBlockAlign = LOA_ReadUShort(_ppcBuff);
	*_pulOffset += sizeof(USHORT);

	uiCkSize -= sizeof(SND_tdst_WAVEfmtCk_Partcommon);

	/*$1- extra part among the format tag ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(stfmtCk.wFormatTag)
	{
	case SND_Cte_WAVE_FORMAT_PCM:
		wBitsPerSample = 8 * stfmtCk.wBlockAlign / stfmtCk.wChannels;
		cbSize = 0;
		wSamplesPerBlock = stfmtCk.wChannels;
		if(uiCkSize == 0)
		{
			break;
		}
		else if(uiCkSize == sizeof(SND_tdst_WAVEfmtCk_PartPCMspecific))
		{
			wBitsPerSample = LOA_ReadUShort(_ppcBuff);
		}
		else if(uiCkSize == sizeof(SND_tdst_WAVEfmtCk_PartExtended))
		{
			wBitsPerSample = LOA_ReadUShort(_ppcBuff);
			cbSize = LOA_ReadUShort(_ppcBuff);
		}
		else
		{
			return -1;
		}
		break;

	case SND_Cte_WAVE_FORMAT_MS_ADPCM:
		if(uiCkSize < sizeof(SND_tdst_WAVEfmtCk_PartMS_ADPCM)) return -1;
		wBitsPerSample = LOA_ReadUShort(_ppcBuff);
		cbSize = LOA_ReadUShort(_ppcBuff);
		wSamplesPerBlock = LOA_ReadUShort(_ppcBuff);
		wNumCoef = LOA_ReadUShort(_ppcBuff);

		if(uiCkSize != (wNumCoef - 1) * sizeof(SND_tdst_MS_ADPCM_COEFF) + sizeof(SND_tdst_WAVEfmtCk_PartMS_ADPCM))
			return -1;
		aCoeff = (SND_tdst_MS_ADPCM_COEFF*)MEM_p_Alloc(wNumCoef * sizeof(SND_tdst_MS_ADPCM_COEFF));
		L_memset(aCoeff, 0, wNumCoef * sizeof(SND_tdst_MS_ADPCM_COEFF));
		for(i = 0; i < wNumCoef; i++)
		{
			aCoeff[i].iCoef1 = LOA_ReadInt(_ppcBuff);
			aCoeff[i].iCoef2 = LOA_ReadInt(_ppcBuff);
		}
		break;

	case SND_Cte_WAVE_FORMAT_XBOX_ADPCM:
		if(uiCkSize < sizeof(SND_tdst_WAVEfmtCk_PartExtended)) return -1;
		wBitsPerSample = LOA_ReadUShort(_ppcBuff);
		cbSize = LOA_ReadUShort(_ppcBuff);
		wSamplesPerBlock = 64;
		if(cbSize == sizeof(USHORT))
		{
			cbSize = 0; /* => TODO est-ce que le cbSize est pris en compte dans la CkSize ? */
			wSamplesPerBlock = LOA_ReadUShort(_ppcBuff);
		}
		break;

	case SND_Cte_WAVE_FORMAT_GAMECUBE:
		if(uiCkSize != sizeof(SND_tdst_WAVEfmtCk_PartExtended)) return -1;
		wBitsPerSample = LOA_ReadUShort(_ppcBuff);
		cbSize = LOA_ReadUShort(_ppcBuff);
		cbSize = 0;		/* special case, we have to reset this attribute */
		wSamplesPerBlock = 14;
		break;

	case SND_Cte_WAVE_FORMAT_PS2:
		if(uiCkSize != sizeof(SND_tdst_WAVEfmtCk_PartExtended)) return -1;
		wBitsPerSample = LOA_ReadUShort(_ppcBuff);
		cbSize = LOA_ReadUShort(_ppcBuff);
		wSamplesPerBlock = 28;
		break;

	default:
		return -1;
	}

	*_pulOffset += uiCkSize;

	if(cbSize)
	{
		LOA_ReadCharArray(_ppcBuff, NULL, cbSize);
		*_pulOffset += cbSize;
	}

	/*$1- now we can allocate the wave struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pWave = (SND_tdst_WaveDesc*)MEM_p_Alloc(sizeof(SND_tdst_WaveDesc));
	L_memset(pWave, 0, sizeof(SND_tdst_WaveDesc));

	pWave->wFormatTag = stfmtCk.wFormatTag;
	pWave->wChannels = stfmtCk.wChannels;
	pWave->dwSamplesPerSec = stfmtCk.dwSamplesPerSec;
	pWave->dwAvgBytesPerSec = stfmtCk.dwAvgBytesPerSec;
	pWave->wBlockAlign = stfmtCk.wBlockAlign;
	pWave->wBitsPerSample = wBitsPerSample;
	pWave->cbSize = cbSize;
	pWave->wSamplesPerBlock = wSamplesPerBlock;
	*_ppWave = pWave;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_WaveReadcueCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc *pWave)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiCkSize, i;
	char			fccChunk[4];
	UI32			dwChunkStart;
	UI32			dwBlockStart;
	UI32			dwSampleOffset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	uiCkSize = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	ERR_X_Assert(SND_gst_CuePoints.dst_Table == NULL);
	ERR_X_Assert(SND_gst_CuePoints.dwCuePoints == 0);
	L_memset(&SND_gst_CuePoints, 0, sizeof(SND_tdst_CuePointDesc));

	SND_gst_CuePoints.dwCuePoints = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	if(uiCkSize < SND_gst_CuePoints.dwCuePoints * sizeof(SND_tdst_WAVEcue_point)) return -1;

	SND_gst_CuePoints.dst_Table = (SND_tdst_MyCuePoint *)MEM_p_Alloc(SND_gst_CuePoints.dwCuePoints * sizeof(SND_tdst_MyCuePoint));
	L_memset(SND_gst_CuePoints.dst_Table, 0, SND_gst_CuePoints.dwCuePoints * sizeof(SND_tdst_MyCuePoint));

	for(i = 0; i < SND_gst_CuePoints.dwCuePoints; i++)
	{
		SND_gst_CuePoints.dst_Table[i].dwName = LOA_ReadULong(_ppcBuff);
		*_pulOffset += sizeof(ULONG);

		SND_gst_CuePoints.dst_Table[i].dwPosition = LOA_ReadULong(_ppcBuff);
		*_pulOffset += sizeof(ULONG);

		LOA_ReadCharArray(_ppcBuff, fccChunk, 4);
		*_pulOffset += sizeof(ULONG);

		dwChunkStart = LOA_ReadULong(_ppcBuff);
		*_pulOffset += sizeof(ULONG);

		dwBlockStart = LOA_ReadULong(_ppcBuff);
		*_pulOffset += sizeof(ULONG);

		dwSampleOffset = LOA_ReadULong(_ppcBuff);
		*_pulOffset += sizeof(ULONG);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_WaveReadplstCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc *pWave)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int			uiCkSize, i, dwSegments, dwName;
	SND_tdst_PlaySegment	st_Segment;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_CuePoints.dst_Table) return -1;

	uiCkSize = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	dwSegments = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	if(uiCkSize < dwSegments * sizeof(SND_tdst_WAVEplay_segment)) return -1;

	/* don't care, we just want to skip this chunk */
	for(i = 0; i < dwSegments; i++)
	{
		dwName = LOA_ReadULong(_ppcBuff);
		*_pulOffset += sizeof(ULONG);

		st_Segment.dwLength = LOA_ReadULong(_ppcBuff);
		*_pulOffset += sizeof(ULONG);

		st_Segment.dwLoops = LOA_ReadULong(_ppcBuff);
		*_pulOffset += sizeof(ULONG);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_WaveReadlablCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc *pWave)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiCkSize, dwName, cue;
	char			name[1024] = "", *pz;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_CuePoints.dst_Table) return -1;

	uiCkSize = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	if(uiCkSize < sizeof(SND_tdst_WAVElablCk)) return -1;

	dwName = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	/* find the cue point */
	for(cue = 0; cue < SND_gst_CuePoints.dwCuePoints; cue++)
	{
		if(SND_gst_CuePoints.dst_Table[cue].dwName == dwName) break;
	}

	if(cue == SND_gst_CuePoints.dwCuePoints) return -1;

	/* read name */
	pz = name;
	do
	{
		*pz = LOA_ReadChar(_ppcBuff);
		*_pulOffset += sizeof(char);
	} while(*pz++);

	if((name[0] >= '0') && (name[0] <= '9'))
	{
		/* if name begins with a number => it is an exit point */
		SND_gst_CuePoints.dst_Table[cue].sz_Label = NULL;
	}
	else
	{
		/* "loop", "start", "end" or an event point */
		SND_gst_CuePoints.dst_Table[cue].sz_Label = (char *)MEM_p_Alloc(L_strlen(name) + 1);
		L_strcpy(SND_gst_CuePoints.dst_Table[cue].sz_Label, name);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_WaveReadnoteCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc *pWave)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiCkSize, dwName;
	char			name[1024] = "", *pz;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_CuePoints.dst_Table) return -1;

	uiCkSize = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	if(uiCkSize < sizeof(SND_tdst_WAVEnoteCk)) return -1;

	dwName = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	/* don't care, we just want to skip the chunck */
	pz = name;
	do
	{
		*pz = LOA_ReadChar(_ppcBuff);
		*_pulOffset += sizeof(char);
	} while(*pz++);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_WaveReadltxtCk(char **_ppcBuff, ULONG *_pulOffset, SND_tdst_WaveDesc *pWave)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiCkSize, dwName, cue;
	char			name[1024] = "", *pz;
	char			dwPurpose[4];
	UI16			wCountry;
	UI16			wLanguage;
	UI16			wDialect;
	UI16			wCodePage;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_CuePoints.dst_Table) return -1;

	uiCkSize = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	if(uiCkSize < sizeof(SND_tdst_WAVEltxtCk)) return -1;

	dwName = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	for(cue = 0; cue < SND_gst_CuePoints.dwCuePoints; cue++)
	{
		if(SND_gst_CuePoints.dst_Table[cue].dwName == dwName) break;
	}

	if(cue == SND_gst_CuePoints.dwCuePoints) return -1;

	SND_gst_CuePoints.dst_Table[cue].dwSampleLength = LOA_ReadULong(_ppcBuff);
	*_pulOffset += sizeof(ULONG);

	LOA_ReadCharArray(_ppcBuff, dwPurpose, 4);
	*_pulOffset += sizeof(ULONG);

	wCountry = LOA_ReadUShort(_ppcBuff);
	*_pulOffset += sizeof(USHORT);
	wLanguage = LOA_ReadUShort(_ppcBuff);
	*_pulOffset += sizeof(USHORT);
	wDialect = LOA_ReadUShort(_ppcBuff);
	*_pulOffset += sizeof(USHORT);
	wCodePage = LOA_ReadUShort(_ppcBuff);
	*_pulOffset += sizeof(USHORT);
	uiCkSize -= sizeof(SND_tdst_WAVEltxtCk);

	if(uiCkSize)
	{
		pz = name;
		do
		{
			*pz = LOA_ReadChar(_ppcBuff);
			*_pulOffset += sizeof(char);
		} while(*pz++);
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#if defined(_XENON)

#include "Xenon/xeSND_WaveParser.h"
#include "Xenon/xeSND_Engine.h"

ULONG SND_ul_WaveCallback(ULONG _ulFilePos)
{
	return (ULONG)GetAudioEngine()->GetWaveParser()->ProcessRIFFFile(_ulFilePos, LOA_ul_GetCurrentKey());
}

void SND_WaveUnload(SND_tdst_WaveDesc *pWave)
{
	GetAudioEngine()->GetWaveParser()->DestroyDescriptor(pWave);
}

#else
ULONG SND_ul_WaveCallback(ULONG _ulFilePos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ulSize, ulOffset;
	char				*pcBuff;
	SND_tdst_WaveDesc	*pWave = NULL;
	UI32				uiCkMarker;
	UI32				uiCkSize;
	int					iStep = 0;
	int					i;
	BAS_tdst_barray		st_ExitPointCollection;
	BAS_tdst_barray		st_SignalPointCollection;
	BAS_tdst_barray		st_RegionPointCollection;
	BOOL				b_Exit;
	ULONG				ul_Label;
	BOOL				b_SignalUnaligned, b_SignalUnknown;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    yet loaded ?
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	ulSize = LOA_ul_SearchAddress(_ulFilePos);
#ifdef JADEFUSION
	if((LONG)ulSize != -1)
#else
		if(ulSize != (ULONG) - 1)
#endif
	{
		((SND_tdst_WaveDesc *) ulSize)->ul_UserCount++;
		return ulSize;
	}

	b_SignalUnaligned = b_SignalUnknown = TRUE;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    read the file
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	pcBuff = BIG_pc_ReadFileTmp(_ulFilePos, &ulSize);

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    truncated file ?
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(ulSize == 8)
	{
		SND_gst_TruncatedWave.ul_UserCount++;
		return(ULONG) & SND_gst_TruncatedWave;
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    then parse the content
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	ulOffset = 0;
	b_Exit = FALSE;

	while(!b_Exit)
	{
		uiCkMarker = LOA_ReadULong(&pcBuff);
		ulOffset += sizeof(ULONG);

		switch(uiCkMarker)
		{

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_RIFF:
			uiCkSize = LOA_ReadULong(&pcBuff);
			ulOffset += sizeof(ULONG);
			ERR_X_Warning(iStep == 0, "[SND] bad wave file", NULL);
			if(!LOA_IsBinaryData())
			{
				ERR_X_Warning(uiCkSize + sizeof(ULONG) <= ulSize, "[SND] bad wave file", NULL);
			}

			iStep = 1;
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_WAVE:
			ERR_X_Warning(iStep == 1, "[SND] bad wave file", NULL);
			iStep = 2;
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_fmt:
			ERR_X_Warning(iStep == 2, "[SND] bad wave file", NULL);
			b_Exit = (SND_i_WaveReadfmtCk(&pcBuff, &ulOffset, &pWave) < 0);
			iStep = 3;
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_data:
			ERR_X_Warning(iStep >= 3, "[SND] bad wave file", NULL);
			if(!pWave) return 0;

			pWave->ul_DataSize = LOA_ReadULong(&pcBuff);
			ulOffset += sizeof(ULONG);

			if(LOA_IsBinaryData()) ulSize += pWave->ul_DataSize;
			ERR_X_Warning((ulOffset + pWave->ul_DataSize <= ulSize), "[SND] bad wave file", NULL);
#if defined(_XENON)
			if (LOA_IsBinaryData())
			{
				_ulFilePos = BIG_ul_SearchKeyToPos(LOA_ul_GetCurrentKey());					
			}
#endif
			pWave->ul_DataPosition = ulOffset + _ulFilePos + 4;
			LOA_ReadCharArray_Ed(&pcBuff, NULL, pWave->ul_DataSize);
			ulOffset += pWave->ul_DataSize;
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_fact:
			uiCkSize = LOA_ReadULong(&pcBuff);
			ulOffset += sizeof(ULONG);

			LOA_ReadCharArray_Ed(&pcBuff, NULL, sizeof(SND_tdst_WAVEfactCk));
			ulOffset += sizeof(SND_tdst_WAVEfactCk);
			if(LOA_IsBinaryData()) ulSize += sizeof(SND_tdst_WAVEfactCk);

			if(uiCkSize > sizeof(SND_tdst_WAVEfactCk))
			{
				LOA_ReadCharArray_Ed(&pcBuff, NULL, uiCkSize - sizeof(SND_tdst_WAVEfactCk));
				ulOffset += uiCkSize - sizeof(SND_tdst_WAVEfactCk);
				if(LOA_IsBinaryData()) ulSize += uiCkSize - sizeof(SND_tdst_WAVEfactCk);
			}
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_cue:
            ERR_X_Warning(iStep >= 3, "[SND] bad wave file", NULL);
			if(!pWave) return 0;
			b_Exit = (SND_i_WaveReadcueCk(&pcBuff, &ulOffset, pWave) < 0);
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_plst:
			ERR_X_Warning(iStep >= 3, "[SND] bad wave file", NULL);
			if(!pWave) return 0;
			b_Exit = (SND_i_WaveReadplstCk(&pcBuff, &ulOffset, pWave) < 0);
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_LIST:
			ERR_X_Warning(iStep >= 3, "[SND] bad wave file", NULL);
			uiCkSize = LOA_ReadULong(&pcBuff);
			ulOffset += sizeof(ULONG);
			iStep = 4;
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_INFO:
			ERR_X_Warning(iStep >= 4, "[SND] bad wave file", NULL);
			LOA_ReadCharArray_Ed(&pcBuff, NULL, uiCkSize - sizeof(ULONG));
			ulOffset += uiCkSize - sizeof(ULONG);
			if(LOA_IsBinaryData()) ulSize += uiCkSize - sizeof(ULONG);
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_adtl:
			ERR_X_Warning(iStep >= 4, "[SND] bad wave file", NULL);
			iStep = 5;
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_labl:
			ERR_X_Warning(iStep >= 5, "[SND] bad wave file", NULL);
			if(!pWave) return 0;
			b_Exit = (SND_i_WaveReadlablCk(&pcBuff, &ulOffset, pWave) < 0);
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_note:
			ERR_X_Warning(iStep >= 5, "[SND] bad wave file", NULL);
			if(!pWave) return 0;
			b_Exit = (SND_i_WaveReadnoteCk(&pcBuff, &ulOffset, pWave) < 0);
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_ltxt:
			ERR_X_Warning(iStep >= 5, "[SND] bad wave file", NULL);
			if(!pWave) return 0;
			b_Exit = (SND_i_WaveReadltxtCk(&pcBuff, &ulOffset, pWave) < 0);
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_file:
			ERR_X_Warning(iStep >= 5, "[SND] bad wave file", NULL);
			uiCkSize = LOA_ReadULong(&pcBuff);
			ulOffset += sizeof(ULONG);
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case SND_Cte_Marker_smpl:
		case SND_Cte_Marker_bext:
		case SND_Cte_Marker_MEXT:
		case SND_Cte_Marker_DISP:
		case SND_Cte_Marker_PAD:
		case SND_Cte_Marker_JUNK:
			uiCkSize = LOA_ReadULong(&pcBuff);
			ulOffset += sizeof(ULONG);
			LOA_ReadCharArray_Ed(&pcBuff, NULL, uiCkSize);
			ulOffset += uiCkSize;
			if(LOA_IsBinaryData()) ulSize += uiCkSize;
			break;

		/*$2-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		default:
			if
			(
				L_isprint((uiCkMarker >> 24) & 0xFF)
			&&	L_isprint((uiCkMarker >> 16) & 0xFF)
			&&	L_isprint((uiCkMarker >> 8) & 0xFF)
			&&	L_isprint(uiCkMarker & 0xFF)
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
				char			logg[512];
				extern ULONG	LOA_gul_CurrentKey;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				sprintf(logg, "One wav file has unknown chunk [%x]", LOA_gul_CurrentKey);
				ERR_X_Warning(!b_SignalUnknown, logg, NULL);
				b_SignalUnknown = FALSE;
#endif

				/* unsupported chunck */
				uiCkSize = LOA_ReadULong(&pcBuff);
				ulOffset += sizeof(ULONG);

				LOA_ReadCharArray_Ed(&pcBuff, NULL, uiCkSize);
				ulOffset += uiCkSize;
				if(LOA_IsBinaryData()) ulSize += uiCkSize;
			}
			break;
		}

		if(b_Exit)
		{
			if(pWave) SND_WaveUnload(pWave);
			return 0;
		}

		/* align on 2 oct boundaries */
		if(ulOffset & 1)
		{
			LOA_ReadChar_Ed(&pcBuff, NULL);
			ulOffset += 1;
			if(LOA_IsBinaryData()) ulSize += 1;
		}

		if(ulOffset >= ulSize) break;
	}

	if(!pWave) return 0;

#ifdef _DEBUG

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(pWave->wFormatTag == SND_Cte_WAVE_FORMAT_PCM)
	{
		ERR_X_Assert((pWave->wSamplesPerBlock / pWave->wChannels) == 1);
	}
	else if(pWave->wFormatTag == SND_Cte_WAVE_FORMAT_PS2)
	{
		ERR_X_Assert(pWave->wSamplesPerBlock == 28);
	}
	else if(pWave->wFormatTag == SND_Cte_WAVE_FORMAT_GAMECUBE)
	{
		ERR_X_Assert(pWave->wSamplesPerBlock == 14);
	}
	else if(pWave->wFormatTag == SND_Cte_WAVE_FORMAT_XBOX_ADPCM)
	{
		ERR_X_Assert((pWave->wSamplesPerBlock) == 64);
	}
#endif

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    get regions+markers
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	pWave->ul_StartOffset = 0;
	pWave->ul_EndOffset = 0;

	pWave->b_HasLoop = 0;
	pWave->ul_LoopBeginOffset = 0;
	pWave->ul_LoopEndOffset = 0;

	pWave->aul_ExitPoint = NULL;
	pWave->ul_ExitPointNb = 0;

	pWave->dst_SignalTable = NULL;
	pWave->ul_SignalPointNb = 0;

	pWave->dst_RegionTable = NULL;
	pWave->ul_RegionNb = 0;

	if(SND_gst_CuePoints.dwCuePoints)
	{

		/*$2- parse all cue points -----------------------------------------------------------------------------------*/

		BAS_binit(&st_ExitPointCollection, 1);
		BAS_binit(&st_SignalPointCollection, 1);
		BAS_binit(&st_RegionPointCollection, 1);

		for(i = 0; i < (int) SND_gst_CuePoints.dwCuePoints; i++)
		{
			if(SND_gst_CuePoints.dst_Table[i].sz_Label)
			{
				/* if there is a label, it is a signal point or start/loop/end */
				if(!L_stricmp(SND_gst_CuePoints.dst_Table[i].sz_Label, SND_Csz_LabelStart))
				{
					/* start marker */
					pWave->ul_StartOffset = SND_ui_SampleToSize
						(
							pWave->wFormatTag,
							pWave->wChannels,
							SND_gst_CuePoints.dst_Table[i].dwPosition
						);
				}
				else if(!L_stricmp(SND_gst_CuePoints.dst_Table[i].sz_Label, SND_Csz_LabelEnd))
				{
					/* end marker */
					pWave->ul_EndOffset = SND_ui_SampleToSize
						(
							pWave->wFormatTag,
							pWave->wChannels,
							SND_gst_CuePoints.dst_Table[i].dwPosition
						);
				}
				else if(!L_stricmp(SND_gst_CuePoints.dst_Table[i].sz_Label, SND_Csz_LabelLoop))
				{
					/* loop region */

					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
					char			log[512];
					extern ULONG	LOA_gul_CurrentKey;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					sprintf(log, "One wav file has several loop definition [%x]", LOA_gul_CurrentKey);
					ERR_X_Warning(!pWave->b_HasLoop, log, NULL);
#endif
					pWave->b_HasLoop = 1;
					pWave->ul_LoopBeginOffset = SND_gst_CuePoints.dst_Table[i].dwPosition;
					pWave->ul_LoopEndOffset = pWave->ul_LoopBeginOffset + SND_gst_CuePoints.dst_Table[i].dwSampleLength;

					pWave->ul_LoopBeginOffset = SND_ui_SampleToSize
						(
							pWave->wFormatTag,
							pWave->wChannels,
							pWave->ul_LoopBeginOffset
						);
					pWave->ul_LoopEndOffset = SND_ui_SampleToSize
						(
							pWave->wFormatTag,
							pWave->wChannels,
							pWave->ul_LoopEndOffset
						);

				}
				else
				{
					/* signal marker */
					((char *) &ul_Label)[0] = SND_gst_CuePoints.dst_Table[i].sz_Label[0];
					((char *) &ul_Label)[1] = SND_gst_CuePoints.dst_Table[i].sz_Label[1];
					((char *) &ul_Label)[2] = SND_gst_CuePoints.dst_Table[i].sz_Label[2];
					((char *) &ul_Label)[3] = SND_gst_CuePoints.dst_Table[i].sz_Label[3];

					if(SND_gst_CuePoints.dst_Table[i].dwSampleLength)
						BAS_binsert(SND_gst_CuePoints.dst_Table[i].dwPosition, i, &st_RegionPointCollection);
					else
						BAS_binsert(SND_gst_CuePoints.dst_Table[i].dwPosition, ul_Label, &st_SignalPointCollection);

					/* do not register this point as an exit point */
					continue;
				}
			}

			/* register as an exit point */
			BAS_binsert
			(
				SND_gst_CuePoints.dst_Table[i].dwPosition,
				SND_gst_CuePoints.dst_Table[i].dwPosition,
				&st_ExitPointCollection
			);
		}

		/*$2- set loop points ----------------------------------------------------------------------------------------*/
		if
			(
			(pWave->ul_LoopBeginOffset || pWave->ul_LoopEndOffset) &&
			((pWave->ul_LoopEndOffset- pWave->ul_LoopBeginOffset) <= 128)
			)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
			char			log2[512];
			extern ULONG	LOA_gul_CurrentKey;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			sprintf(log2, "One wav file has small loop size : %d oct [%08x]", pWave->ul_LoopEndOffset-pWave->ul_LoopBeginOffset, LOA_gul_CurrentKey);
			ERR_X_Warning(0, log2, NULL);
#endif
			pWave->ul_LoopBeginOffset = pWave->ul_LoopEndOffset = 0;
		}

		if(!pWave->ul_LoopBeginOffset && !pWave->ul_LoopEndOffset)
		{
			pWave->ul_LoopBeginOffset = 0;
			pWave->ul_LoopEndOffset = pWave->ul_DataSize;
		}
		else
		{
    		// round begin and end loop points
    		if(pWave->ul_LoopBeginOffset && SND_b_SizeIsEqual(pWave->wFormatTag, pWave->wChannels, pWave->ul_LoopBeginOffset, 0))
    		    pWave->ul_LoopBeginOffset = 0;

    		if((pWave->ul_LoopEndOffset != pWave->ul_DataSize) && SND_b_SizeIsEqual(pWave->wFormatTag, pWave->wChannels, pWave->ul_LoopEndOffset, pWave->ul_DataSize))
    		    pWave->ul_LoopEndOffset = pWave->ul_DataSize;
		}

		/*$2- set exit points ----------------------------------------------------------------------------------------*/

		if(st_ExitPointCollection.num)
		{
			pWave->ul_ExitPointNb = st_ExitPointCollection.num;
			pWave->aul_ExitPoint = (ULONG *) MEM_p_Alloc(pWave->ul_ExitPointNb * sizeof(ULONG));

			for(i = 0; i < st_ExitPointCollection.num; i++)
			{
				pWave->aul_ExitPoint[i] = st_ExitPointCollection.base[i].ul_Key;
			}
		}

		BAS_bfree(&st_ExitPointCollection);

		/*$2- set signal points --------------------------------------------------------------------------------------*/

		if(st_SignalPointCollection.num)
		{
			pWave->ul_SignalPointNb = st_SignalPointCollection.num;
			pWave->dst_SignalTable = (SND_tdst_SignalPoint *) MEM_p_Alloc(pWave->ul_SignalPointNb * sizeof(SND_tdst_SignalPoint));
			L_memset(pWave->dst_SignalTable, 0, pWave->ul_SignalPointNb * sizeof(SND_tdst_SignalPoint));

			for(i = 0; i < st_SignalPointCollection.num; i++)
			{
				pWave->dst_SignalTable[i].ul_Position = st_SignalPointCollection.base[i].ul_Key;
				pWave->dst_SignalTable[i].ul_Label = st_SignalPointCollection.base[i].ul_Val;
			}
		}

		BAS_bfree(&st_SignalPointCollection);

		/*$2- set region ---------------------------------------------------------------------------------------------*/

		if(st_RegionPointCollection.num)
		{
			pWave->ul_RegionNb = st_RegionPointCollection.num;
			pWave->dst_RegionTable = (SND_tdst_Region *) MEM_p_Alloc(pWave->ul_RegionNb * sizeof(SND_tdst_Region));
			L_memset(pWave->dst_RegionTable, 0, pWave->ul_RegionNb * sizeof(SND_tdst_Region));

			for(i = 0; i < st_RegionPointCollection.num; i++)
			{
				((char *) &ul_Label)[0] = SND_gst_CuePoints.dst_Table[st_RegionPointCollection.base[i].ul_Val].sz_Label[0];
				((char *) &ul_Label)[1] = SND_gst_CuePoints.dst_Table[st_RegionPointCollection.base[i].ul_Val].sz_Label[1];
				((char *) &ul_Label)[2] = SND_gst_CuePoints.dst_Table[st_RegionPointCollection.base[i].ul_Val].sz_Label[2];
				((char *) &ul_Label)[3] = SND_gst_CuePoints.dst_Table[st_RegionPointCollection.base[i].ul_Val].sz_Label[3];

				pWave->dst_RegionTable[i].ul_Label = ul_Label;
				pWave->dst_RegionTable[i].ul_StartPosition = st_RegionPointCollection.base[i].ul_Key;
				pWave->dst_RegionTable[i].ul_StopPosition = pWave->dst_RegionTable[i].ul_StartPosition + SND_gst_CuePoints.dst_Table[st_RegionPointCollection.base[i].ul_Val].dwSampleLength;
			}
		}

		BAS_bfree(&st_RegionPointCollection);

		/*$2- free all temp ------------------------------------------------------------------------------------------*/

		if(SND_gst_CuePoints.dst_Table)
		{
			for(i = 0; i < (int) SND_gst_CuePoints.dwCuePoints; i++)
			{
				if(SND_gst_CuePoints.dst_Table[i].sz_Label) MEM_Free(SND_gst_CuePoints.dst_Table[i].sz_Label);
				SND_gst_CuePoints.dst_Table[i].sz_Label = NULL;
			}

			MEM_Free(SND_gst_CuePoints.dst_Table);
		}

		SND_gst_CuePoints.dst_Table = NULL;
		SND_gst_CuePoints.dwCuePoints = 0;
	}

	if(!pWave->ul_LoopEndOffset) pWave->ul_LoopEndOffset = pWave->ul_DataSize;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	pWave->ul_FileKey = LOA_ul_GetCurrentKey();
	pWave->ul_UserCount = 1;

	return(ULONG) pWave;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_WaveUnload(SND_tdst_WaveDesc *pWave)
{
	if(!pWave) return;
	if(--pWave->ul_UserCount) return;
	LOA_DeleteAddress(pWave);

	if(pWave->aul_ExitPoint)
	{
		MEM_Free(pWave->aul_ExitPoint);
		pWave->aul_ExitPoint = NULL;
	}

	if(pWave->dst_SignalTable)
	{
		MEM_Free(pWave->dst_SignalTable);
		pWave->dst_SignalTable = NULL;
	}

	if(pWave->dst_RegionTable)
	{
		MEM_Free(pWave->dst_RegionTable);
		pWave->dst_RegionTable = NULL;
	}

	if(pWave != &SND_gst_TruncatedWave) MEM_Free(pWave);
}
#endif // _XENON
/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int SND_ui_SizeToSample(unsigned short usFormat, unsigned short usChannel, unsigned int uiSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiSample;
	float			fBlockSize, fSamplesPerBlock, fChannel, fSample, fSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	fChannel = fLongToFloat(usChannel);
	fSize = fLongToFloat(uiSize);

	switch(usFormat)
	{
	case SND_Cte_WAVE_FORMAT_PCM:			fBlockSize = 2.0f; fSamplesPerBlock = 1.0f; break;
	case SND_Cte_WAVE_FORMAT_MS_ADPCM:		fBlockSize = 36.0f; fSamplesPerBlock = 60.0f; break;
	case SND_Cte_WAVE_FORMAT_XBOX_ADPCM:	fBlockSize = 36.0f; fSamplesPerBlock = 64.0f; break;
	case SND_Cte_WAVE_FORMAT_GAMECUBE:		fBlockSize = 8.0f; fSamplesPerBlock = 14.0f; break;
	case SND_Cte_WAVE_FORMAT_PS2:			fBlockSize = 16.0f; fSamplesPerBlock = 28.0f; break;
	default:								return 0;
	}

	fSample = (fSamplesPerBlock * fSize) / (fBlockSize * fChannel);
	uiSample = lFloatToLong(fSample);

	return uiSample;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int SND_ui_SampleToSize(unsigned short usFormat, unsigned short usChannel, unsigned int uiSample)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiSize;
	float			fBlockSize, fSamplesPerBlock, fChannel, fSample, fSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	fChannel = fLongToFloat(usChannel);
	fSample = fLongToFloat(uiSample);

	switch(usFormat)
	{
	case SND_Cte_WAVE_FORMAT_PCM:			fBlockSize = 2.0f; fSamplesPerBlock = 1.0f; break;
	case SND_Cte_WAVE_FORMAT_MS_ADPCM:		fBlockSize = 36.0f; fSamplesPerBlock = 60.0f; break;
	case SND_Cte_WAVE_FORMAT_XBOX_ADPCM:	fBlockSize = 36.0f; fSamplesPerBlock = 64.0f; break;
	case SND_Cte_WAVE_FORMAT_GAMECUBE:		fBlockSize = 8.0f; fSamplesPerBlock = 14.0f; break;
	case SND_Cte_WAVE_FORMAT_PS2:			fBlockSize = 16.0f; fSamplesPerBlock = 28.0f; break;
	default:								return 0;
	}

	fSize = (fSample * fBlockSize * fChannel) / fSamplesPerBlock;
	uiSize = lFloatToLong(fSize);

	return uiSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SND_b_SizeIsEqual(unsigned short usFormat, unsigned short usChannel, unsigned int uiSize1, unsigned int uiSize2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiBlockSize, uiDelta;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(usFormat)
	{
	case SND_Cte_WAVE_FORMAT_PCM:			uiBlockSize = 2; break;
	case SND_Cte_WAVE_FORMAT_MS_ADPCM:		uiBlockSize = 36; break;
	case SND_Cte_WAVE_FORMAT_XBOX_ADPCM:	uiBlockSize = SND_Cte_OneXboxFrameSize; break;
	case SND_Cte_WAVE_FORMAT_GAMECUBE:		uiBlockSize = SND_Cte_OneGamecubeFrameSize; break;
	case SND_Cte_WAVE_FORMAT_PS2:			uiBlockSize = SND_Cte_OnePs2FrameSize; break;
	default:								return FALSE;
	}

	uiDelta = (uiSize1 < uiSize2) ? (uiSize2 - uiSize1) : (uiSize1 - uiSize2);

	return(uiDelta <= uiBlockSize) ? TRUE : FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SND_b_SampleIsEqual
(
	unsigned short	usFormat,
	unsigned short	usChannel,
	unsigned int	uiSize1,
	unsigned int	uiSize2
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiFrameSize, uiDelta;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(usFormat)
	{
	case SND_Cte_WAVE_FORMAT_PCM:			uiFrameSize = 2; break;
	case SND_Cte_WAVE_FORMAT_MS_ADPCM:		uiFrameSize = 36; break;
	case SND_Cte_WAVE_FORMAT_XBOX_ADPCM:	uiFrameSize = SND_Cte_OneXboxFrameSampleNb; break;
	case SND_Cte_WAVE_FORMAT_GAMECUBE:		uiFrameSize = SND_Cte_OneGamecubeFrameSampleNb; break;
	case SND_Cte_WAVE_FORMAT_PS2:			uiFrameSize = SND_Cte_OnePs2FrameSampleNb; break;
	default:								return FALSE;
	}

	uiDelta = (uiSize1 < uiSize2) ? (uiSize2 - uiSize1) : (uiSize1 - uiSize2);

	return(uiDelta <= uiFrameSize) ? TRUE : FALSE;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
