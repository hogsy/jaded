/*$T CWaveFile.cpp GC 1.138 05/14/04 15:22:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "stdafx.h"
#include "CWaveFile.h"

/*$4
 ***********************************************************************************************************************
    chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CChunk::CChunk(UI32 dwCkId, UI32 dwCkSize)
{
	InitCk(dwCkId, dwCkSize);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CChunk::InitCk(UI32 dwCkId, UI32 dwCkSize)
{
	m_dwCkId = dwCkId;
	m_dwCkSize = dwCkSize;
}

/*$4
 ***********************************************************************************************************************
    fmt
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CFormatChunk::InitCommon
(
	UI16	wFormatTag,
	UI16	wChannels,
	UI32	dwSamplesPerSec,
	UI32	dwAvgBytesPerSec,
	UI16	wBlockAlign
)
{
	m_Format.st_PartCommon.wFormatTag = wFormatTag;
	m_Format.st_PartCommon.wChannels = wChannels;
	m_Format.st_PartCommon.dwSamplesPerSec = dwSamplesPerSec;
	m_Format.st_PartCommon.dwAvgBytesPerSec = dwAvgBytesPerSec;
	m_Format.st_PartCommon.wBlockAlign = wBlockAlign;
	m_dwCkSize = sizeof(m_Format.st_PartCommon);

	memset(&m_Format.un_PartFacultative, 0, sizeof(m_Format.un_PartFacultative));
	m_pExtra = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CFormatChunk::InitPCM
(
	UI16	wFormatTag,
	UI16	wChannels,
	UI32	dwSamplesPerSec,
	UI32	dwAvgBytesPerSec,
	UI16	wBlockAlign,
	UI16	wBitsPerSample
)
{
	m_Format.st_PartCommon.wFormatTag = wFormatTag;
	m_Format.st_PartCommon.wChannels = wChannels;
	m_Format.st_PartCommon.dwSamplesPerSec = dwSamplesPerSec;
	m_Format.st_PartCommon.dwAvgBytesPerSec = dwAvgBytesPerSec;
	m_Format.st_PartCommon.wBlockAlign = wBlockAlign;
	m_dwCkSize = sizeof(m_Format.st_PartCommon);

	memset(&m_Format.un_PartFacultative, 0, sizeof(m_Format.un_PartFacultative));

	m_dwCkSize += sizeof(m_Format.un_PartFacultative.st_PCM);
	m_Format.un_PartFacultative.st_PCM.wBitsPerSample = wBitsPerSample;
	m_pExtra = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CFormatChunk::InitEX
(
	UI16	wFormatTag,
	UI16	wChannels,
	UI32	dwSamplesPerSec,
	UI32	dwAvgBytesPerSec,
	UI16	wBlockAlign,
	UI16	wBitsPerSample,
	UI16	cbSize
)
{
	m_Format.st_PartCommon.wFormatTag = wFormatTag;
	m_Format.st_PartCommon.wChannels = wChannels;
	m_Format.st_PartCommon.dwSamplesPerSec = dwSamplesPerSec;
	m_Format.st_PartCommon.dwAvgBytesPerSec = dwAvgBytesPerSec;
	m_Format.st_PartCommon.wBlockAlign = wBlockAlign;
	m_dwCkSize = sizeof(m_Format.st_PartCommon);

	memset(&m_Format.un_PartFacultative, 0, sizeof(m_Format.un_PartFacultative));

	m_dwCkSize += sizeof(m_Format.un_PartFacultative.st_Extended);

	m_Format.un_PartFacultative.st_Extended.wBitsPerSample = wBitsPerSample;
	m_Format.un_PartFacultative.st_Extended.cbSize = cbSize;
	m_pExtra = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CFormatChunk::InitIMA
(
	UI16	wFormatTag,
	UI16	wChannels,
	UI32	dwSamplesPerSec,
	UI32	dwAvgBytesPerSec,
	UI16	wBlockAlign,
	UI16	wBitsPerSample,
	UI16	cbSize,
	UI16	wSamplesPerBlock
)
{
	m_Format.st_PartCommon.wFormatTag = wFormatTag;
	m_Format.st_PartCommon.wChannels = wChannels;
	m_Format.st_PartCommon.dwSamplesPerSec = dwSamplesPerSec;
	m_Format.st_PartCommon.dwAvgBytesPerSec = dwAvgBytesPerSec;
	m_Format.st_PartCommon.wBlockAlign = wBlockAlign;
	m_dwCkSize = sizeof(m_Format.st_PartCommon);

	memset(&m_Format.un_PartFacultative, 0, sizeof(m_Format.un_PartFacultative));

	m_dwCkSize += sizeof(m_Format.un_PartFacultative.st_Xbox);

	m_Format.un_PartFacultative.st_Xbox.wBitsPerSample = wBitsPerSample;
	m_Format.un_PartFacultative.st_Xbox.cbSize = cbSize;
	m_Format.un_PartFacultative.st_Xbox.wSamplesPerBlock = wSamplesPerBlock;
	m_pExtra = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CFormatChunk::InitADPCM
(
	UI16	wFormatTag,
	UI16	wChannels,
	UI32	dwSamplesPerSec,
	UI32	dwAvgBytesPerSec,
	UI16	wBlockAlign,
	UI16	wBitsPerSample,
	UI16	cbSize,
	UI16	wSamplesPerBlock,
	UI16	wNumCoef,
	int		*aCoeff
)
{
	m_Format.st_PartCommon.wFormatTag = wFormatTag;
	m_Format.st_PartCommon.wChannels = wChannels;
	m_Format.st_PartCommon.dwSamplesPerSec = dwSamplesPerSec;
	m_Format.st_PartCommon.dwAvgBytesPerSec = dwAvgBytesPerSec;
	m_Format.st_PartCommon.wBlockAlign = wBlockAlign;
	m_dwCkSize = sizeof(m_Format.st_PartCommon);

	memset(&m_Format.un_PartFacultative, 0, sizeof(m_Format.un_PartFacultative));

	m_dwCkSize += sizeof(m_Format.un_PartFacultative.st_MS_ADPCM);

	m_Format.un_PartFacultative.st_MS_ADPCM.wBitsPerSample = wBitsPerSample;
	m_Format.un_PartFacultative.st_MS_ADPCM.cbSize = cbSize;
	m_Format.un_PartFacultative.st_MS_ADPCM.wSamplesPerBlock = wSamplesPerBlock;
	m_Format.un_PartFacultative.st_MS_ADPCM.wNumCoef = wNumCoef;

	m_Format.un_PartFacultative.st_MS_ADPCM.aCoeff = new UI32[2 * wNumCoef];
	for(UI16 i = 0; i < wNumCoef; i++)
	{
		m_Format.un_PartFacultative.st_MS_ADPCM.aCoeff[2 * i] = aCoeff[2 * i];
		m_Format.un_PartFacultative.st_MS_ADPCM.aCoeff[2 * i + 1] = aCoeff[2 * i + 1];
	}

	m_pExtra = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CFormatChunk::~CFormatChunk(void)
{
	if(m_Format.un_PartFacultative.st_MS_ADPCM.aCoeff) delete[] m_Format.un_PartFacultative.st_MS_ADPCM.aCoeff;
	if(m_pExtra) delete[] m_pExtra;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CFormatChunk::CFormatChunk(HMMIO hFile) :
	CChunk(SND_Cte_Marker_fmt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MMCKINFO			ckIn;
	MMCKINFO			FatherCk;
	WAVEFORMAT			WaveFormat;
	PCMWAVEFORMAT		pcmWaveFormat;
	WAVEFORMATEX		WaveFormatex;
	IMAADPCMWAVEFORMAT	ImaAdpcmFormat;
	LONG				lOffset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	m_Format.st_PartCommon.wFormatTag = 0;
	m_Format.st_PartCommon.wChannels = 0;
	m_Format.st_PartCommon.dwSamplesPerSec = 0;
	m_Format.st_PartCommon.dwAvgBytesPerSec = 0;
	m_Format.st_PartCommon.wBlockAlign = 0;
	memset(&m_Format.un_PartFacultative, 0, sizeof(m_Format.un_PartFacultative));
	m_pExtra = NULL;

	if(!hFile) return;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    find WAVE-RIFF form
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	mmioSeek(hFile, 0, SEEK_SET);
	FatherCk.ckid = SND_Cte_Marker_RIFF;
	FatherCk.fccType = SND_Cte_Marker_WAVE;
	if(mmioDescend(hFile, &FatherCk, NULL, MMIO_FINDRIFF)) return;
	if((FatherCk.ckid != SND_Cte_Marker_RIFF) || (FatherCk.fccType != SND_Cte_Marker_WAVE)) return;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    find fmt chunk
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	ckIn.ckid = m_dwCkId;
	if(mmioDescend(hFile, &ckIn, &FatherCk, MMIO_FINDCHUNK)) return;
	if(ckIn.cksize < (LONG) sizeof(WAVEFORMAT)) return;
	if(mmioRead(hFile, (HPSTR) & WaveFormat, sizeof(WAVEFORMAT)) != sizeof(WAVEFORMAT)) return;
	lOffset = sizeof(WAVEFORMAT);
	mmioSeek(hFile, -lOffset, SEEK_CUR);

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    get format data
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	switch(WaveFormat.wFormatTag)
	{
	case SND_Cte_WAVE_FORMAT_PCM:
		switch(ckIn.cksize)
		{
		case sizeof(WAVEFORMAT):
			if(mmioRead(hFile, (HPSTR) & WaveFormat, sizeof(WAVEFORMAT)) != sizeof(WAVEFORMAT)) return;
			InitCommon
			(
				WaveFormat.wFormatTag,
				WaveFormat.nChannels,
				WaveFormat.nSamplesPerSec,
				WaveFormat.nAvgBytesPerSec,
				WaveFormat.nBlockAlign
			);
			break;

		case sizeof(PCMWAVEFORMAT):
			if(mmioRead(hFile, (HPSTR) & pcmWaveFormat, sizeof(PCMWAVEFORMAT)) != sizeof(PCMWAVEFORMAT)) return;
			InitPCM
			(
				pcmWaveFormat.wf.wFormatTag,
				pcmWaveFormat.wf.nChannels,
				pcmWaveFormat.wf.nSamplesPerSec,
				pcmWaveFormat.wf.nAvgBytesPerSec,
				pcmWaveFormat.wf.nBlockAlign,
				pcmWaveFormat.wBitsPerSample
			);
			break;

		case sizeof(WAVEFORMATEX):
		default:
			if(mmioRead(hFile, (HPSTR) & WaveFormatex, sizeof(WAVEFORMATEX)) != sizeof(WAVEFORMATEX)) return;
			InitEX
			(
				WaveFormatex.wFormatTag,
				WaveFormatex.nChannels,
				WaveFormatex.nSamplesPerSec,
				WaveFormatex.nAvgBytesPerSec,
				WaveFormatex.nBlockAlign,
				WaveFormatex.wBitsPerSample,
				WaveFormatex.cbSize
			);
			if(WaveFormatex.cbSize)
			{
				m_pExtra = new char[WaveFormatex.cbSize];
				mmioRead(hFile, (HPSTR) m_pExtra, WaveFormatex.cbSize);
				m_dwCkSize += WaveFormatex.cbSize;
			}
			break;
		}
		break;

	case SND_Cte_WAVE_FORMAT_GAMECUBE:
	case SND_Cte_WAVE_FORMAT_PS2:
		if(mmioRead(hFile, (HPSTR) & WaveFormatex, sizeof(WAVEFORMATEX)) != sizeof(WAVEFORMATEX)) return;
		InitEX
		(
			WaveFormatex.wFormatTag,
			WaveFormatex.nChannels,
			WaveFormatex.nSamplesPerSec,
			WaveFormatex.nAvgBytesPerSec,
			WaveFormatex.nBlockAlign,
			WaveFormatex.wBitsPerSample,
			WaveFormatex.cbSize
		);
		if(WaveFormatex.cbSize)
		{
			m_pExtra = new char[WaveFormatex.cbSize];
			mmioRead(hFile, (HPSTR) m_pExtra, WaveFormatex.cbSize);
			m_dwCkSize += WaveFormatex.cbSize;
		}
		break;

	case SND_Cte_WAVE_FORMAT_XBOX_ADPCM:
		if(ckIn.cksize != sizeof(IMAADPCMWAVEFORMAT)) return;

	case SND_Cte_WAVE_FORMAT_MS_ADPCM:
		if(mmioRead(hFile, (HPSTR) & ImaAdpcmFormat, sizeof(IMAADPCMWAVEFORMAT)) != sizeof(IMAADPCMWAVEFORMAT))
			return;
		InitIMA
		(
			ImaAdpcmFormat.wfx.wFormatTag,
			ImaAdpcmFormat.wfx.nChannels,
			ImaAdpcmFormat.wfx.nSamplesPerSec,
			ImaAdpcmFormat.wfx.nAvgBytesPerSec,
			ImaAdpcmFormat.wfx.nBlockAlign,
			ImaAdpcmFormat.wfx.wBitsPerSample,
			ImaAdpcmFormat.wfx.cbSize,
			ImaAdpcmFormat.wSamplesPerBlock
		);
		if(ImaAdpcmFormat.wfx.cbSize)
		{
			m_pExtra = new char[ImaAdpcmFormat.wfx.cbSize];
			mmioRead(hFile, (HPSTR) m_pExtra, ImaAdpcmFormat.wfx.cbSize);
			m_dwCkSize += ImaAdpcmFormat.wfx.cbSize;
		}
		break;

	default:
		return;
	}

	mmioAscend(hFile, &ckIn, 0);	/* go back to main RIFF */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CFormatChunk::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	/*~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) InitCommon(SND_Cte_WAVE_FORMAT_PCM, 1, 44100, 44100 * 2, 2);

	ckIn.ckid = m_dwCkId;
	ckIn.cksize = m_dwCkSize;

	if(mmioCreateChunk(hFile, &ckIn, 0)) return -1;
	if(mmioWrite(hFile, (const char *) &m_Format, m_dwCkSize) != m_dwCkSize) return -1;
	if(mmioAscend(hFile, &ckIn, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    fact
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CFactChunk::CFactChunk(HMMIO hFile) :
	CChunk(SND_Cte_Marker_fact)
{
	/*~~~~~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	MMCKINFO	FatherCk;
	UI32		dw;
	/*~~~~~~~~~~~~~~~~~*/

	m_dwFileSize = 0;
	if(!hFile) return;

	mmioSeek(hFile, 0, SEEK_SET);
	FatherCk.ckid = SND_Cte_Marker_RIFF;
	FatherCk.fccType = SND_Cte_Marker_WAVE;
	if(mmioDescend(hFile, &FatherCk, NULL, MMIO_FINDRIFF)) return;
	if((FatherCk.ckid != SND_Cte_Marker_RIFF) || (FatherCk.fccType != SND_Cte_Marker_WAVE)) return;

	ckIn.ckid = m_dwCkId;
	if(mmioDescend(hFile, &ckIn, &FatherCk, MMIO_FINDCHUNK)) return;
	if(mmioRead(hFile, (HPSTR) & dw, sizeof(dw)) != sizeof(dw)) return;
	ASSERT(ckIn.cksize == sizeof(dw));

	m_dwFileSize = dw;
	m_dwCkSize = ckIn.cksize;

	mmioAscend(hFile, &ckIn, 0);	/* go back to main RIFF */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CFactChunk::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	/*~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) return 0;

	ckIn.ckid = m_dwCkId;
	ckIn.cksize = m_dwCkSize;

	if(mmioCreateChunk(hFile, &ckIn, 0)) return -1;
	if(mmioWrite(hFile, (const char *) &m_dwFileSize, sizeof(m_dwFileSize)) != sizeof(m_dwFileSize)) return -1;
	if(mmioAscend(hFile, &ckIn, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    cue
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CCuePointChunk::Init(UI32 dwCuePoints, CCuePoint *aTable)
{
	m_Data.dwCuePoints = dwCuePoints;
	m_Data.dstCuePointTable = NULL;
	if(dwCuePoints) m_Data.dstCuePointTable = aTable;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CCuePointChunk::CCuePointChunk(HMMIO hFile) :
	CChunk(SND_Cte_Marker_cue)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	MMCKINFO	FatherCk;
	UI32		dwCuePoints;
	CCuePoint	*dstCuePointTable;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Init();
	if(!hFile) return;

	mmioSeek(hFile, 0, SEEK_SET);
	FatherCk.ckid = SND_Cte_Marker_RIFF;
	FatherCk.fccType = SND_Cte_Marker_WAVE;
	if(mmioDescend(hFile, &FatherCk, NULL, MMIO_FINDRIFF)) return;
	if((FatherCk.ckid != SND_Cte_Marker_RIFF) || (FatherCk.fccType != SND_Cte_Marker_WAVE)) return;

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    find cue chunk
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	ckIn.ckid = m_dwCkId;
	if(mmioDescend(hFile, &ckIn, &FatherCk, MMIO_FINDCHUNK)) return;
	if(mmioRead(hFile, (HPSTR) & dwCuePoints, sizeof(dwCuePoints)) != sizeof(dwCuePoints)) return;
	ASSERT(ckIn.cksize == sizeof(dwCuePoints) + dwCuePoints * sizeof(CCuePoint));

	if(dwCuePoints)
	{
		dstCuePointTable = new CCuePoint[dwCuePoints];
		if
		(
			mmioRead(hFile, (HPSTR) dstCuePointTable, dwCuePoints * sizeof(CCuePoint)) !=
				(dwCuePoints * sizeof(CCuePoint))
		) return;
		Init(dwCuePoints, dstCuePointTable);
	}

	m_dwCkSize = ckIn.cksize;

	mmioAscend(hFile, &ckIn, 0);	/* go back to main RIFF */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CCuePointChunk::~CCuePointChunk(void)
{
	if(m_Data.dstCuePointTable) delete[] m_Data.dstCuePointTable;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CCuePointChunk::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	/*~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) return 0;

	ckIn.ckid = m_dwCkId;
	ckIn.cksize = m_dwCkSize;

	if(mmioCreateChunk(hFile, &ckIn, 0)) return -1;
	if(mmioWrite(hFile, (const char *) &m_Data.dwCuePoints, sizeof(m_Data.dwCuePoints)) != sizeof(m_Data.dwCuePoints))
		return -1;
	if
	(
		mmioWrite
			(
				hFile,
				(const char *) m_Data.dstCuePointTable,
				m_Data.dwCuePoints * sizeof(CCuePoint)
			) != m_Data.dwCuePoints * sizeof(CCuePoint)
	) return -1;

	if(mmioAscend(hFile, &ckIn, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    play list
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CPlayListChunk::CPlayListChunk(HMMIO hFile) :
	CChunk(SND_Cte_Marker_plst)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MMCKINFO		ckIn;
	MMCKINFO		FatherCk;
	UI32			dwSegments;
	CPlaySegment	*dstSegment;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	Init();
	if(!hFile) return;

	mmioSeek(hFile, 0, SEEK_SET);
	FatherCk.ckid = SND_Cte_Marker_RIFF;
	FatherCk.fccType = SND_Cte_Marker_WAVE;
	if(mmioDescend(hFile, &FatherCk, NULL, MMIO_FINDRIFF)) return;
	if((FatherCk.ckid != SND_Cte_Marker_RIFF) || (FatherCk.fccType != SND_Cte_Marker_WAVE)) return;

	ckIn.ckid = m_dwCkId;
	if(mmioDescend(hFile, &ckIn, &FatherCk, MMIO_FINDCHUNK)) return;
	if(mmioRead(hFile, (HPSTR) & dwSegments, sizeof(dwSegments)) != sizeof(dwSegments)) return;
	ASSERT(ckIn.cksize == sizeof(dwSegments) + dwSegments * sizeof(CPlaySegment));

	if(dwSegments)
	{
		dstSegment = new CPlaySegment[dwSegments];

		if
		(
			mmioRead(hFile, (HPSTR) dstSegment, dwSegments * sizeof(CPlaySegment)) !=
				(dwSegments * sizeof(CPlaySegment))
		) return;
		Init(dwSegments, dstSegment);
	}

	m_dwCkSize = ckIn.cksize;

	mmioAscend(hFile, &ckIn, 0);	/* go back to main RIFF */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CPlayListChunk::~CPlayListChunk(void)
{
	if(m_Data.dstSegment) delete[] m_Data.dstSegment;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CPlayListChunk::Init(UI32 dwSegments, CPlaySegment *dstSegment)
{
	m_Data.dwSegments = dwSegments;
	m_Data.dstSegment = NULL;
	if(dstSegment && dwSegments) m_Data.dstSegment = dstSegment;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CPlayListChunk::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	/*~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) return 0;

	ckIn.ckid = m_dwCkId;
	ckIn.cksize = m_dwCkSize;

	if(mmioCreateChunk(hFile, &ckIn, 0)) return -1;
	if(mmioWrite(hFile, (const char *) &m_Data.dwSegments, sizeof(m_Data.dwSegments)) != sizeof(m_Data.dwSegments))
		return -1;
	if
	(
		mmioWrite
			(
				hFile,
				(const char *) m_Data.dstSegment,
				m_Data.dwSegments * sizeof(CPlaySegment)
			) != m_Data.dwSegments * sizeof(CPlaySegment)
	) return -1;

	if(mmioAscend(hFile, &ckIn, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    label chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CLabelChunk::CLabelChunk(HMMIO hFile) :
	CChunk(SND_Cte_Marker_labl)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	MMCKINFO	ckIn = { 0 };
	UI32		dwName;
	UI32		dwSize;
	char		*data;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	Init();
	if(!hFile) return;

	ckIn.ckid = m_dwCkId;
	if(mmioDescend(hFile, &ckIn, NULL, MMIO_FINDCHUNK)) return;
	if(mmioRead(hFile, (HPSTR) & dwName, sizeof(dwName)) != sizeof(dwName)) return;
	dwSize = ckIn.cksize - sizeof(dwName);

	if(dwSize)
	{
		data = new char[dwSize];
		if(mmioRead(hFile, (HPSTR) data, dwSize) != dwSize) return;
	}
	else
	{
		data = NULL;
	}

	Init(dwName, data);
	m_dwCkSize = ckIn.cksize;

	mmioAscend(hFile, &ckIn, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CLabelChunk::Init(UI32 dwName, char *data)
{
	m_Data.dwName = dwName;
	m_Data.data = NULL;
	if(data) m_Data.data = data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CLabelChunk::~CLabelChunk(void)
{
	if(m_Data.data) delete[] m_Data.data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CLabelChunk::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	/*~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) return 0;

	ckIn.ckid = m_dwCkId;
	ckIn.cksize = m_dwCkSize;

	if(mmioCreateChunk(hFile, &ckIn, 0)) return -1;
	if(mmioWrite(hFile, (const char *) &m_Data.dwName, sizeof(m_Data.dwName)) != sizeof(m_Data.dwName)) return -1;
	if
	(
		mmioWrite(hFile, (const char *) m_Data.data, m_dwCkSize - sizeof(m_Data.dwName)) !=
			(m_dwCkSize - sizeof(m_Data.dwName))
	) return -1;

	if(mmioAscend(hFile, &ckIn, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    note chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CNoteChunk::CNoteChunk(HMMIO hFile) :
	CChunk(SND_Cte_Marker_note)
{
	/*~~~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	UI32		dwName;
	UI32		dwSize;
	char		*data;
	/*~~~~~~~~~~~~~~~*/

	Init();
	if(!hFile) return;

	ckIn.ckid = m_dwCkId;
	if(mmioDescend(hFile, &ckIn, NULL, MMIO_FINDCHUNK)) return;
	if(mmioRead(hFile, (HPSTR) & dwName, sizeof(dwName)) != sizeof(dwName)) return;
	dwSize = ckIn.cksize - sizeof(dwName);

	if(dwSize)
	{
		data = new char[dwSize];
		if(mmioRead(hFile, (HPSTR) data, dwSize) != dwSize) return;
	}
	else
	{
		data = NULL;
	}

	Init(dwName, data);
	m_dwCkSize = ckIn.cksize;

	mmioAscend(hFile, &ckIn, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CNoteChunk::~CNoteChunk(void)
{
	if(m_Data.data) delete[] m_Data.data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CNoteChunk::Init(UI32 dwName, char *data)
{
	m_Data.dwName = dwName;
	m_Data.data = NULL;
	if(data) m_Data.data = data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CNoteChunk::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	/*~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) return 0;

	ckIn.ckid = m_dwCkId;
	ckIn.cksize = m_dwCkSize;

	if(mmioCreateChunk(hFile, &ckIn, 0)) return -1;
	if(mmioWrite(hFile, (const char *) &m_Data.dwName, sizeof(m_Data.dwName)) != sizeof(m_Data.dwName)) return -1;
	if
	(
		mmioWrite(hFile, (const char *) m_Data.data, m_dwCkSize - sizeof(m_Data.dwName)) !=
			(m_dwCkSize - sizeof(m_Data.dwName))
	) return -1;

	if(mmioAscend(hFile, &ckIn, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    textl chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CTextlChunk::CTextlChunk(HMMIO hFile) :
	CChunk(SND_Cte_Marker_ltxt)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MMCKINFO				ckIn = { 0 };
	struct __CTextlChunk	stTemp;
	UI32					dwSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Init();
	if(!hFile) return;

	ckIn.ckid = m_dwCkId;
	if(mmioDescend(hFile, &ckIn, NULL, MMIO_FINDCHUNK)) return;

	dwSize = sizeof(stTemp) - sizeof(stTemp.data);
	if(mmioRead(hFile, (HPSTR) & stTemp, dwSize) != dwSize) return;

	dwSize = ckIn.cksize - dwSize;
	stTemp.data = NULL;
	if(dwSize)
	{
		stTemp.data = new char[dwSize];
		if(mmioRead(hFile, (HPSTR) stTemp.data, dwSize) != dwSize) return;
	}

	Init
	(
		stTemp.dwName,
		stTemp.dwSampleLength,
		stTemp.dwPurpose,
		stTemp.wCountry,
		stTemp.wLanguage,
		stTemp.wDialect,
		stTemp.wCodePage,
		stTemp.data
	);
	m_dwCkSize = ckIn.cksize;

	mmioAscend(hFile, &ckIn, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CTextlChunk::~CTextlChunk(void)
{
	if(m_Data.data) delete[] m_Data.data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CTextlChunk::Init
(
	UI32	dwName,
	UI32	dwSampleLength,
	char	dwPurpose[4],
	UI16	wCountry,
	UI16	wLanguage,
	UI16	wDialect,
	UI16	wCodePage,
	char	*data
)
{
	m_Data.dwName = dwName;
	m_Data.dwSampleLength = dwSampleLength;
	m_Data.dwPurpose[0] = dwPurpose[0];
	m_Data.dwPurpose[1] = dwPurpose[1];
	m_Data.dwPurpose[2] = dwPurpose[2];
	m_Data.dwPurpose[3] = dwPurpose[3];
	m_Data.wCountry = wCountry;
	m_Data.wLanguage = wLanguage;
	m_Data.wDialect = wDialect;
	m_Data.wCodePage = wCodePage;

	if(data)
	{
		m_Data.data = data;
	}
	else
	{
		m_Data.data = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CTextlChunk::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	UI32		dwSize;
	/*~~~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) return 0;

	ckIn.ckid = m_dwCkId;
	ckIn.cksize = m_dwCkSize;

	if(mmioCreateChunk(hFile, &ckIn, 0)) return -1;

	dwSize = sizeof(m_Data) - sizeof(m_Data.data);
	if(mmioWrite(hFile, (const char *) &m_Data, dwSize) != dwSize) return -1;

	dwSize = m_dwCkSize - dwSize;
    if(dwSize && m_Data.data)
    {
	    if(mmioWrite(hFile, (const char *) m_Data.data, dwSize) != dwSize) return -1;
    }
	if(mmioAscend(hFile, &ckIn, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    file chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CFileChunk::CFileChunk(HMMIO hFile) :
	CChunk(SND_Cte_Marker_file)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	UI32		dwSize;
	UI32		dwName;
	UI32		dwMedType;
	UI8			*fileData;
	/*~~~~~~~~~~~~~~~~~~*/

	Init();
	if(!hFile) return;

	ckIn.ckid = m_dwCkId;
	if(mmioDescend(hFile, &ckIn, NULL, MMIO_FINDCHUNK)) return;
	if(mmioRead(hFile, (HPSTR) & dwName, sizeof(dwName)) != sizeof(dwName)) return;
	if(mmioRead(hFile, (HPSTR) & dwMedType, sizeof(dwMedType)) != sizeof(dwMedType)) return;

	dwSize = ckIn.cksize - sizeof(dwName) - sizeof(dwMedType);
	fileData = NULL;
	if(dwSize)
	{
		fileData = new UI8[dwSize];
		if(mmioRead(hFile, (HPSTR) fileData, dwSize) != dwSize) return;
	}

	Init(dwName, dwMedType, fileData);
	m_dwCkSize = ckIn.cksize;

	mmioAscend(hFile, &ckIn, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CFileChunk::~CFileChunk(void)
{
	if(m_Data.fileData) delete[] m_Data.fileData;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CFileChunk::Init(UI32 dwName, UI32 dwMedType, UI8 *fileData)
{
	m_Data.dwName = dwName;
	m_Data.dwMedType = dwMedType;
	m_Data.fileData = fileData;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CFileChunk::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	UI32		dwSize;
	/*~~~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) return 0;

	ckIn.ckid = m_dwCkId;
	ckIn.cksize = m_dwCkSize;

	if(mmioCreateChunk(hFile, &ckIn, 0)) return -1;

	dwSize = sizeof(m_Data) - sizeof(m_Data.fileData);
	if(mmioWrite(hFile, (const char *) &m_Data, dwSize) != dwSize) return -1;

	dwSize = m_dwCkSize - dwSize;
	if(mmioWrite(hFile, (const char *) m_Data.fileData, dwSize) != dwSize) return -1;
	if(mmioAscend(hFile, &ckIn, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    associated data chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CAssociatedDataList::CAssociatedDataList(HMMIO hFile) :
	CForm(SND_Cte_Marker_LIST, SND_Cte_Marker_adtl)
{
	/*~~~~~~~~~~~~~~~*/
	MMCKINFO	RiffCk;
	MMCKINFO	ListCk;
	LONG		lSeek;
	/*~~~~~~~~~~~~~~~*/

	Init();
	if(!hFile) return;

	/* rewind the file */
	mmioSeek(hFile, 0, SEEK_SET);

	/* go into riff form */
	RiffCk.ckid = SND_Cte_Marker_RIFF;
	RiffCk.fccType = SND_Cte_Marker_WAVE;
	if(mmioDescend(hFile, &RiffCk, NULL, MMIO_FINDRIFF)) return;
	if((RiffCk.ckid != SND_Cte_Marker_RIFF) || (RiffCk.fccType != SND_Cte_Marker_WAVE)) return;

	/* go into adtl LIST */
	ListCk.ckid = m_dwFormType;
	ListCk.fccType = m_dwCkId;
	if(mmioDescend(hFile, &ListCk, &RiffCk, MMIO_FINDLIST)) return;
	lSeek = mmioSeek(hFile, 0, SEEK_CUR);

	do
	{
		m_poTextCk = new CTextlChunk(hFile);
		m_TextlList.AddHead(m_poTextCk);
	} while(m_poTextCk->m_dwCkSize);

	mmioSeek(hFile, lSeek, SEEK_SET);
	do
	{
		m_poLabelCk = new CLabelChunk(hFile);
		m_LabelList.AddHead(m_poLabelCk);
	} while(m_poLabelCk->m_dwCkSize);

	mmioSeek(hFile, lSeek, SEEK_SET);
	do
	{
		m_poNoteCk = new CNoteChunk(hFile);
		m_NoteList.AddHead(m_poNoteCk);
	} while(m_poNoteCk->m_dwCkSize);

	mmioSeek(hFile, lSeek, SEEK_SET);
	do
	{
		m_poFileCk = new CFileChunk(hFile);
		m_FileList.AddHead(m_poFileCk);
	} while(m_poFileCk->m_dwCkSize);

	m_dwCkSize = ListCk.cksize;
	mmioAscend(hFile, &ListCk, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CAssociatedDataList::~CAssociatedDataList(void)
{
	/*~~~~~~~~~~~~*/
	POSITION	pos;
	/*~~~~~~~~~~~~*/

	pos = m_LabelList.GetHeadPosition();
	while(pos)
	{
		m_poLabelCk = m_LabelList.GetNext(pos);
		if(m_poLabelCk) delete m_poLabelCk;
	}

	pos = m_NoteList.GetHeadPosition();
	while(pos)
	{
		m_poNoteCk = m_NoteList.GetNext(pos);
		if(m_poNoteCk) delete m_poNoteCk;
	}

	pos = m_TextlList.GetHeadPosition();
	while(pos)
	{
		m_poTextCk = m_TextlList.GetNext(pos);
		if(m_poTextCk) delete m_poTextCk;
	}

	pos = m_FileList.GetHeadPosition();
	while(pos)
	{
		m_poFileCk = m_FileList.GetNext(pos);
		if(m_poFileCk) delete m_poFileCk;
	}

	Init();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAssociatedDataList::Init(void)
{
	m_poLabelCk = NULL;
	m_poNoteCk = NULL;
	m_poTextCk = NULL;
	m_poFileCk = NULL;

	m_LabelList.RemoveAll();
	m_NoteList.RemoveAll();
	m_TextlList.RemoveAll();
	m_FileList.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CAssociatedDataList::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~~~*/
	MMCKINFO	ListCk;
	POSITION	pos;
	/*~~~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) return 0;

	ListCk.ckid = m_dwFormType;
	ListCk.fccType = m_dwCkId;
	if(mmioCreateChunk(hFile, &ListCk, MMIO_CREATELIST)) return -1;

	if(m_LabelList.GetCount() > 1)
	{
		pos = m_LabelList.GetHeadPosition();
		while(pos)
		{
			m_poLabelCk = m_LabelList.GetNext(pos);
			m_poLabelCk->Save(hFile);
		}
	}

	if(m_NoteList.GetCount() > 1)
	{
		pos = m_NoteList.GetHeadPosition();
		while(pos)
		{
			m_poNoteCk = m_NoteList.GetNext(pos);
			m_poNoteCk->Save(hFile);
		}
	}

	if(m_TextlList.GetCount() > 1)
	{
		pos = m_TextlList.GetHeadPosition();
		while(pos)
		{
			m_poTextCk = m_TextlList.GetNext(pos);
			m_poTextCk->Save(hFile);
		}
	}

	if(m_FileList.GetCount() > 1)
	{
		pos = m_FileList.GetHeadPosition();
		while(pos)
		{
			m_poFileCk = m_FileList.GetNext(pos);
			m_poFileCk->Save(hFile);
		}
	}

	if(mmioAscend(hFile, &ListCk, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    instrument chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CInstrumentChunk::CInstrumentChunk
(
	UI8 bUnshiftedNote,
	UI8 chFineTune,
	UI8 chGain,
	UI8 bLowNote,
	UI8 bHighNote,
	UI8 bLowVelocity,
	UI8 bHighVelocity
) :
	CChunk(SND_Cte_Marker_inst)
{
	m_Data.bUnshiftedNote = bUnshiftedNote;
	m_Data.chFineTune = chFineTune;
	m_Data.chGain = chGain;
	m_Data.bLowNote = bLowNote;
	m_Data.bHighNote = bHighNote;
	m_Data.bLowVelocity = bLowVelocity;
	m_Data.bHighVelocity = bHighVelocity;
}

/*$4
 ***********************************************************************************************************************
    sample chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CSampleChunk::CSampleChunk
(
	UI32		dwManufacturer,
	UI32		dwProduct,
	UI32		dwSamplePeriod,
	UI32		dwMIDIUnityNote,
	UI32		dwMIDIPitchFraction,
	UI32		dwSMPTEFormat,
	UI32		dwSMPTEOffset,
	UI32		cSampleLoops,
	UI32		cbSamplerData,
	CSampleLoop *dstSampleLoop,
	UI8			*dstSamplerSpecificData
) :
	CChunk(SND_Cte_Marker_smpl)
{
	m_Data.dwManufacturer = dwManufacturer;
	m_Data.dwProduct = dwProduct;
	m_Data.dwSamplePeriod = dwSamplePeriod;
	m_Data.dwMIDIUnityNote = dwMIDIUnityNote;
	m_Data.dwMIDIPitchFraction = dwMIDIPitchFraction;
	m_Data.dwSMPTEFormat = dwSMPTEFormat;
	m_Data.dwSMPTEOffset = dwSMPTEOffset;
	m_Data.cSampleLoops = cSampleLoops;
	m_Data.cbSamplerData = cbSamplerData;

	m_Data.dstSampleLoop = new CSampleLoop[cSampleLoops];
	for(UI32 i = 0; i < cSampleLoops; i++) m_Data.dstSampleLoop[i] = dstSampleLoop[i];

	m_Data.dstSamplerSpecificData = NULL;
	if(cbSamplerData)
	{
		m_Data.dstSamplerSpecificData = new UI8[cbSamplerData];
		memcpy(m_Data.dstSamplerSpecificData, dstSamplerSpecificData, cbSamplerData);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CSampleChunk::~CSampleChunk(void)
{
	delete[] m_Data.dstSampleLoop;
	if(m_Data.dstSamplerSpecificData) delete[] m_Data.dstSamplerSpecificData;
}

/*$4
 ***********************************************************************************************************************
    data chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CDataChunk::CDataChunk(HMMIO hFile) :
	CChunk(SND_Cte_Marker_data)
{
	/*~~~~~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	MMCKINFO	FatherCk;
	UI8			*data;
	/*~~~~~~~~~~~~~~~~~*/

	Init();
	if(!hFile) return;

	mmioSeek(hFile, 0, SEEK_SET);
	FatherCk.ckid = SND_Cte_Marker_RIFF;
	FatherCk.fccType = SND_Cte_Marker_WAVE;
	if(mmioDescend(hFile, &FatherCk, NULL, MMIO_FINDRIFF)) return;
	if((FatherCk.ckid != SND_Cte_Marker_RIFF) || (FatherCk.fccType != SND_Cte_Marker_WAVE)) return;

	ckIn.ckid = m_dwCkId;
	if(mmioDescend(hFile, &ckIn, &FatherCk, MMIO_FINDCHUNK)) return;

	data = new UI8[ckIn.cksize];
	if(mmioRead(hFile, (HPSTR) data, ckIn.cksize) != ckIn.cksize) return;

	Init(data);

	m_dwCkSize = ckIn.cksize;
	mmioAscend(hFile, &ckIn, 0);

    m_dwPadding = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CDataChunk::~CDataChunk(void)
{
	if(m_pData) delete[] m_pData;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CDataChunk::Init(UI8 *data)
{
	m_pData = data;
}

void CDataChunk::Padding(UI32 padding)
{
    if(m_dwPadding) m_dwCkSize -= m_dwPadding;
    
    m_dwPadding = padding;
    m_dwCkSize += m_dwPadding;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CDataChunk::Save(HMMIO hFile)
{
	/*~~~~~~~~~~~~~*/
	MMCKINFO	ckIn;
	/*~~~~~~~~~~~~~*/

	if(!hFile) return -1;
	if(m_dwCkSize == 0) return 0;

	ckIn.ckid = m_dwCkId;
	ckIn.cksize = m_dwCkSize;

	if(mmioCreateChunk(hFile, &ckIn, 0)) return -1;
	if(mmioWrite(hFile, (const char *) m_pData, m_dwCkSize-m_dwPadding) != (m_dwCkSize-m_dwPadding)) return -1;
    
    if(m_dwPadding)
    {
        char*p=new char[m_dwPadding];
        memset(p, 0, m_dwPadding);
        if(mmioWrite(hFile, (const char *) p, m_dwPadding) != m_dwPadding) return -1;
        delete [] p;
    }
	
	if(mmioAscend(hFile, &ckIn, 0)) return -1;

	return 0;
}

/*$4
 ***********************************************************************************************************************
    silence chunk
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CSilenceChunk::CSilenceChunk(UI32 dwSamples) :
	CChunk(SND_Cte_Marker_slnt)
{
	m_Data.dwSamples = dwSamples;
}

/*$4
 ***********************************************************************************************************************
    data list
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CDataList::CDataList(UI32 size) :
	CForm(SND_Cte_Marker_LIST, SND_Cte_Marker_wavl)
{
	m_pData = NULL;
	if(size) m_pData = new struct __CDataList[size];
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CDataList::~CDataList(void)
{
	if(m_pData) delete[] m_pData;
}

/*$4
 ***********************************************************************************************************************
    RIFF form
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CRIFFForm::CRIFFForm(void) :
	CForm(SND_Cte_Marker_RIFF, SND_Cte_Marker_WAVE)
{
	m_hmmio = NULL;
	m_bReadMode = true;

	m_pFormatCk = NULL;
	m_pFactCk = NULL;
	m_pCueCk = NULL;
	m_pPlayListCk = NULL;
	m_pAssDataLst = NULL;
	m_pDataCk = NULL;
	m_pDataList = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CRIFFForm::~CRIFFForm(void)
{
	if(m_pFormatCk) delete m_pFormatCk;
	if(m_pFactCk) delete m_pFactCk;
	if(m_pCueCk) delete m_pCueCk;
	if(m_pPlayListCk) delete m_pPlayListCk;
	if(m_pAssDataLst) delete m_pAssDataLst;
	if(m_pDataCk) delete m_pDataCk;
	if(m_pDataList) delete m_pDataList;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CRIFFForm::Open(char *strFileName, bool b_readmode)
{
	if(b_readmode)
	{
		m_bReadMode = true;
		m_hmmio = ::mmioOpen(strFileName, NULL, MMIO_ALLOCBUF | MMIO_READ);
	}
	else
	{
		m_bReadMode = false;
		m_hmmio = ::mmioOpen(strFileName, NULL, MMIO_ALLOCBUF | MMIO_READWRITE | MMIO_CREATE);
	}

	return m_hmmio ? 0 : -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRIFFForm::Close(void)
{
	if(m_hmmio) mmioClose(m_hmmio, 0);
	m_hmmio = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CRIFFForm::Reset(void)
{
	if(m_pFormatCk) delete m_pFormatCk;
	if(m_pFactCk) delete m_pFactCk;
	if(m_pCueCk) delete m_pCueCk;
	if(m_pPlayListCk) delete m_pPlayListCk;
	if(m_pAssDataLst) delete m_pAssDataLst;
	if(m_pDataCk) delete m_pDataCk;
	if(m_pDataList) delete m_pDataList;
	m_pFormatCk = NULL;
	m_pFactCk = NULL;
	m_pCueCk = NULL;
	m_pPlayListCk = NULL;
	m_pAssDataLst = NULL;
	m_pDataCk = NULL;
	m_pDataList = NULL;

	memset(&m_ckRiff, 0, sizeof(m_ckRiff));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CRIFFForm::Load(void)
{
	Reset();
	if(!m_hmmio) return -1;

	m_pFormatCk = new CFormatChunk(m_hmmio);
	m_pFactCk = new CFactChunk(m_hmmio);
	m_pCueCk = new CCuePointChunk(m_hmmio);
	m_pPlayListCk = new CPlayListChunk(m_hmmio);
	m_pAssDataLst = new CAssociatedDataList(m_hmmio);
	m_pDataCk = new CDataChunk(m_hmmio);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CRIFFForm::Save(void)
{
	/*~~~~~~~~~~~~~~~*/
	MMCKINFO	RiffCk;
	/*~~~~~~~~~~~~~~~*/

	if(!m_hmmio) return -1;
	if(m_bReadMode) return -1;

	RiffCk.cksize = 0;

	if(m_pFormatCk->bIsValid())
	{
		RiffCk.cksize += m_pFormatCk->m_dwCkSize;
		RiffCk.cksize += sizeof(CChunk);
	}

	if(m_pFactCk->bIsValid())
	{
		RiffCk.cksize += m_pFactCk->m_dwCkSize;
		RiffCk.cksize += sizeof(CChunk);
	}

	if(m_pDataCk->bIsValid())
	{
		RiffCk.cksize += m_pDataCk->m_dwCkSize;
		RiffCk.cksize += sizeof(CChunk);
	}

	if(m_pCueCk->bIsValid())
	{
		RiffCk.cksize += m_pCueCk->m_dwCkSize;
		RiffCk.cksize += sizeof(CChunk);
	}

	if(m_pPlayListCk->bIsValid())
	{
		RiffCk.cksize += m_pPlayListCk->m_dwCkSize;
		RiffCk.cksize += sizeof(CChunk);
	}

	if(m_pAssDataLst->bIsValid())
	{
		RiffCk.cksize += m_pAssDataLst->m_dwCkSize;
		RiffCk.cksize += sizeof(CForm);
	}

	if(!RiffCk.cksize) return 1;
	RiffCk.ckid = SND_Cte_Marker_RIFF;
	RiffCk.fccType = SND_Cte_Marker_WAVE;

	if(mmioCreateChunk(m_hmmio, &RiffCk, MMIO_CREATERIFF)) return -1;

	m_pFormatCk->Save(m_hmmio);
	m_pFactCk->Save(m_hmmio);
	m_pDataCk->Save(m_hmmio);
	m_pCueCk->Save(m_hmmio);
	m_pPlayListCk->Save(m_hmmio);
	m_pAssDataLst->Save(m_hmmio);

	if(mmioAscend(m_hmmio, &RiffCk, 0)) return -1;
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UI32 dwGetIndexFromCueName(UI32 dwName, CCuePoint *dstCueList, UI32 dwCuePoints)
{
	/*~~~~~~*/
	UI32	i;
	/*~~~~~~*/

	if(dstCueList == NULL) return -1;

	for(i = 0; i < dwCuePoints; i++)
	{
		if(dstCueList[i].dwName == dwName) return i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UI32 dwGetIndexFromCueDistance(UI32 dwIndex, UI32 dwDistance, CCuePoint *dstCueList, UI32 dwCuePoints)
{
	/*~~~~~~*/
	UI32	i;
	/*~~~~~~*/

	if(dstCueList == NULL) return -1;

	for(i = 0; i < dwCuePoints; i++)
	{
		if(dstCueList[dwIndex].dwPosition <= dstCueList[i].dwPosition)
		{
			if((dstCueList[i].dwPosition - dstCueList[dwIndex].dwPosition + 1) == dwDistance) return i;
		}
		else
		{
			if((dstCueList[dwIndex].dwPosition - dstCueList[i].dwPosition + 1) == dwDistance) return i;
		}
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int CRIFFForm::AlignMarkers(UI32 dwAlign)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CCuePoint		*dstPointList;
	CPlaySegment	*dstPlayList;
	UI32			dwPointNb, i;
	UI32			dwPosition;
	UI32			dwPosition1;
	UI32			dwPosition2;
	UI32			dwBegin, dwEnd;
	LONG			lBegin, lEnd, lLenght;
//    UI32 dwSample , dwPad;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    //round size
#if 0
    if(m_pDataCk->bIsValid() && m_pFormatCk->bIsValid())
    {
        dwSample = m_pDataCk->m_dwCkSize / m_pFormatCk->m_Format.st_PartCommon.wBlockAlign;
        if(dwSample % dwAlign)
        {
            dwPad = dwAlign - (dwSample % dwAlign);
            m_pDataCk->Padding(dwPad*m_pFormatCk->m_Format.st_PartCommon.wBlockAlign);
        }
    }
#endif

    if(!m_pCueCk->bIsValid()) return 0;

	dwPointNb = m_pCueCk->m_Data.dwCuePoints;
	dstPointList = m_pCueCk->m_Data.dstCuePointTable;

	m_pCueCk->m_Data.dstCuePointTable = new CCuePoint[dwPointNb];
	for(i = 0; i < dwPointNb; i++)
	{
		if
		(
			SND_M_MARKER
				(
					dstPointList[i].fccChunk[0],
					dstPointList[i].fccChunk[1],
					dstPointList[i].fccChunk[2],
					dstPointList[i].fccChunk[3]
				) == SND_Cte_Marker_data
		)
		{
			m_pCueCk->m_Data.dstCuePointTable[i].dwName = dstPointList[i].dwName;

			m_pCueCk->m_Data.dstCuePointTable[i].fccChunk[0] = dstPointList[i].fccChunk[0];
			m_pCueCk->m_Data.dstCuePointTable[i].fccChunk[1] = dstPointList[i].fccChunk[1];
			m_pCueCk->m_Data.dstCuePointTable[i].fccChunk[2] = dstPointList[i].fccChunk[2];
			m_pCueCk->m_Data.dstCuePointTable[i].fccChunk[3] = dstPointList[i].fccChunk[3];

			ASSERT(dstPointList[i].dwChunkStart == 0);
			ASSERT(dstPointList[i].dwBlockStart == 0);
			m_pCueCk->m_Data.dstCuePointTable[i].dwChunkStart = 0;
			m_pCueCk->m_Data.dstCuePointTable[i].dwBlockStart = 0;

			ASSERT(dstPointList[i].dwPosition == dstPointList[i].dwSampleOffset);

			dwPosition = dstPointList[i].dwPosition;
			if(dwPosition % dwAlign)
			{
				dwPosition1 = dwPosition - (dwPosition % dwAlign);
				dwPosition2 = dwPosition1 + dwAlign;
				if((dwPosition2 - dwPosition) < (dwPosition - dwPosition1))
					dwPosition = dwPosition2;
				else
					dwPosition = dwPosition1;
			}

			m_pCueCk->m_Data.dstCuePointTable[i].dwPosition = dwPosition;
			m_pCueCk->m_Data.dstCuePointTable[i].dwSampleOffset = dwPosition;
		}
		else
		{
			ASSERT(0);
			m_pCueCk->m_Data.dstCuePointTable[i] = dstPointList[i];
		}
	}



    if(m_pAssDataLst->bIsValid() && m_pAssDataLst->m_TextlList.GetCount())
    {
        POSITION pos;

        pos = m_pAssDataLst->m_TextlList.GetHeadPosition();
        while(pos)
        {
            m_pAssDataLst->m_poTextCk = m_pAssDataLst->m_TextlList.GetNext(pos);
            if(!m_pAssDataLst->m_poTextCk->bIsValid()) continue;

            
            dwBegin = m_pAssDataLst->m_poTextCk->m_Data.dwName;
            dwBegin = dwGetIndexFromCueName(dwBegin, dstPointList, m_pCueCk->m_Data.dwCuePoints);
            ASSERT(dwBegin!=-1);
            lBegin = (LONG) m_pCueCk->m_Data.dstCuePointTable[dwBegin].dwPosition - (LONG) dstPointList[dwBegin].dwPosition;

			dwEnd = dwGetIndexFromCueDistance
				(
					dwBegin,
					m_pAssDataLst->m_poTextCk->m_Data.dwSampleLength,
					dstPointList,
					m_pCueCk->m_Data.dwCuePoints
				);
			if(dwEnd == -1)
				lEnd = 0;
			else
				lEnd = (LONG) m_pCueCk->m_Data.dstCuePointTable[dwEnd].dwPosition - (LONG) dstPointList[dwEnd].dwPosition;

			lLenght = lEnd - lBegin + (LONG) m_pAssDataLst->m_poTextCk->m_Data.dwSampleLength;

            m_pAssDataLst->m_poTextCk->m_Data.dwSampleLength = (UI32) lLenght;
        }
    }
    
    
    if(m_pPlayListCk->bIsValid())
	{
		dwPointNb = m_pPlayListCk->m_Data.dwSegments;
		dstPlayList = m_pPlayListCk->m_Data.dstSegment;
		m_pPlayListCk->m_Data.dstSegment = new CPlaySegment[dwPointNb];

		for(i = 0; i < dwPointNb; i++)
		{
			m_pPlayListCk->m_Data.dstSegment[i].dwName = dstPlayList[i].dwName;
			m_pPlayListCk->m_Data.dstSegment[i].dwLoops = dstPlayList[i].dwLoops;

			dwBegin = dwGetIndexFromCueName(dstPlayList[i].dwName, dstPointList, m_pCueCk->m_Data.dwCuePoints);
            ASSERT(dwBegin!=-1);
            lBegin = (LONG) m_pCueCk->m_Data.dstCuePointTable[dwBegin].dwPosition - (LONG) dstPointList[dwBegin].dwPosition;

			dwEnd = dwGetIndexFromCueDistance
				(
					dwBegin,
					dstPlayList[i].dwLength,
					dstPointList,
					m_pCueCk->m_Data.dwCuePoints
				);
			if(dwEnd == -1)
				lEnd = 0;
			else
				lEnd = (LONG) m_pCueCk->m_Data.dstCuePointTable[dwEnd].dwPosition - (LONG) dstPointList[dwEnd].dwPosition;

			lLenght = lEnd - lBegin + (LONG) dstPlayList[i].dwLength;

			m_pPlayListCk->m_Data.dstSegment[i].dwLength = (UI32) lLenght;
		}

		delete[] dstPlayList;
	}

	delete[] dstPointList;
	return 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
