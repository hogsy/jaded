#ifndef __xeSND_WaveParser_h__
#define __xeSND_WaveParser_h__

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "SouND/Sources/SNDwave.h"

#include "xeSND_Helpers.h"

#include "BASe/MEMory/MEM.h"

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

#define MakeRIFFChunkID(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

const int kChunkID_RIFF			= (MakeRIFFChunkID('R', 'I', 'F', 'F'));
const int kChunkID_Format		= (MakeRIFFChunkID('f', 'm', 't', ' '));
const int kChunkID_Data			= (MakeRIFFChunkID('d', 'a', 't', 'a'));
const int kChunkID_Cue			= (MakeRIFFChunkID('c', 'u', 'e', ' '));
const int kChunkID_List			= (MakeRIFFChunkID('L', 'I', 'S', 'T'));
const int kChunkID_Label		= (MakeRIFFChunkID('l', 'a', 'b', 'l'));
const int kChunkID_LabelledText	= (MakeRIFFChunkID('l', 't', 'x', 't'));

// ***********************************************************************************************************************
//    Structures
// ***********************************************************************************************************************

typedef struct
{
	unsigned short	wFormatTag;
	unsigned short	wChannels;
	unsigned int	dwSamplesPerSec;
	unsigned int	dwAvgBytesPerSec;
	unsigned short	wBlockAlign;
}
stWave_Chunk_Format_Common;

// ***********************************************************************************************************************

typedef struct
{
	unsigned short wBitsPerSample;
}
stWave_Chunk_Format_PCM;

// ***********************************************************************************************************************

typedef struct
{
	unsigned short wBitsPerSample;
	unsigned short cbSize;
}
stWave_Chunk_Format_Extended;

// ***********************************************************************************************************************

typedef struct
{
	int iCoef1;
	int iCoef2;
}
stWave_MSADPCM_Coeff;

// ***********************************************************************************************************************

typedef struct
{
	unsigned int	dwName;
	unsigned int	dwPosition;
	unsigned int	dwSampleLength;
	char*			sz_Label;
}
stWave_Cue_Point;

// ***********************************************************************************************************************

typedef struct
{
	unsigned int		dwCuePoints;
	stWave_Cue_Point*	dst_Table;
}
stWave_Cue_Point_Descriptor;

// ***********************************************************************************************************************
//    Classes
// ***********************************************************************************************************************

class CWaveParser
{
private:
	SND_tdst_WaveDesc m_stTruncatedWave;

	unsigned int GetMarkerOffset(unsigned int Param_uiFormat, unsigned int Param_uiChannelCount, unsigned int Param_uiSampleCount);

public:
    M_DeclareOperatorNewAndDelete();

	CWaveParser();
	~CWaveParser();

	SND_tdst_WaveDesc*	ProcessRIFFFile(unsigned int Param_uiStartPositionInFile, unsigned int Param_uiKey, eSoundBFIndex Param_eSoundBF = eSoundBF_None);
	void				DestroyDescriptor(SND_tdst_WaveDesc* Param_pWaveDescriptor);
};

// ***********************************************************************************************************************

#endif // _xeSND_WaveParser_h_
