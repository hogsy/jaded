/*$T ps2adpcm.cpp GC 1.138 05/26/04 17:31:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "stdafx.h"
#include <DSOUND.H>
#include <math.h>
#include <stdio.h>
#include "gcadpcm.h"
#include "CWaveFile.h"
#include "main.h"




#define SwapWord(x) (*x = ((*x & 0x00ff) << 8) | ((*x & 0xff00) >> 8))

#define GAMECUBE_USE_ADPCM
#define SND_Csz_GameCubeDll "\\dsptool.dll"


typedef unsigned int (*SND_tdpfi_GCFunc1) (unsigned int);
typedef unsigned int (*SND_tdpfi_GCFunc2) (void);

typedef void (*SND_tdpfv_GCFunc3) (short *, unsigned char *, SND_tdst_GameCubeADPCMInfo *, unsigned int);
typedef void (*SND_tdpfv_GCFunc4) (unsigned char *, short *, SND_tdst_GameCubeADPCMInfo *, unsigned int);
typedef void (*SND_tdpfv_GCFunc5) (unsigned char *, SND_tdst_GameCubeADPCMInfo *, unsigned int);

typedef struct	SND_tdst_GameCubeTools_
{
	BOOL				b_Valid;
	HINSTANCE			h_Dll;
	SND_tdpfi_GCFunc1	pfi_GetBytesForAdpcmBuffer;
	SND_tdpfi_GCFunc1	pfi_GetBytesForAdpcmSamples;
	SND_tdpfi_GCFunc1	pfi_GetBytesForPcmBuffer;
	SND_tdpfi_GCFunc1	pfi_GetBytesForPcmSamples;
	SND_tdpfi_GCFunc1	pfi_GetSampleForAdpcmNibble;
	SND_tdpfi_GCFunc1	pfi_GetNibblesForNSamples;
	SND_tdpfi_GCFunc2	pfi_GetBytesForAdpcmInfo;
	SND_tdpfv_GCFunc3	pfv_Encode;
	SND_tdpfv_GCFunc4	pfv_Decode;
	SND_tdpfv_GCFunc5	pfv_GetLoopContext;
} SND_tdst_GameCubeTools;

SND_tdst_GameCubeTools		SND_gst_GameCubeTools;
int							SND_gi_GameCubeToolsLoaded = 0;

SND_tdst_GameCubeADPCMInfo SND_gst_GameCubeCoefficientsTable = 
{
    {
        (short)0x4ab,
        (short)0xfced,
        (short)0x789,
        (short)0xfedf,
        (short)0x9a2,
        (short)0xfae5,
        (short)0xc90,
        (short)0xfac1,
        (short)0x84d,
        (short)0xfaa4,
        (short)0x982,
        (short)0xfdf7,
        (short)0xaf6,
        (short)0xfafa,
        (short)0xbe6,
        (short)0xfbf5
    }, //coef
    0, //gain
    0, //pred_scale
    0, //yn1
    0, //yn2
    0, //loop_pred_scale
    0, //loop_yn1
    0  //loop_yn2
};


#define C_WAV2VAG_BEFORE	56
#define C_WAV2VAG_AFTER		16
#define M_SUCCEEDED(a)		((a) >= 0)


/*
 =======================================================================================================================
    vag struct £
    private struct
 =======================================================================================================================
 */
CGCAdpcmCodec::CGCAdpcmCodec(void)
{
	if(SND_gi_GameCubeToolsLoaded) return;
	SND_gi_GameCubeToolsLoaded = -1;

	memset(&SND_gst_GameCubeTools, 0, sizeof(SND_tdst_GameCubeTools));

	strcat(MAIN_gst_Command.asz_DllPath, SND_Csz_GameCubeDll);
    SND_gst_GameCubeTools.h_Dll = LoadLibrary(MAIN_gst_Command.asz_DllPath);

	if(SND_gst_GameCubeTools.h_Dll)
	{
		if
		(
			!(
				SND_gst_GameCubeTools.pfi_GetBytesForAdpcmBuffer = (SND_tdpfi_GCFunc1) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"getBytesForAdpcmBuffer"
					)
			)
		) return;

		if
		(
			!(
				SND_gst_GameCubeTools.pfi_GetBytesForAdpcmSamples = (SND_tdpfi_GCFunc1) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"getBytesForAdpcmSamples"
					)
			)
		) return;

		if
		(
			!(
				SND_gst_GameCubeTools.pfi_GetBytesForPcmBuffer = (SND_tdpfi_GCFunc1) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"getBytesForPcmBuffer"
					)
			)
		) return;

		if
		(
			!(
				SND_gst_GameCubeTools.pfi_GetBytesForPcmSamples = (SND_tdpfi_GCFunc1) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"getBytesForPcmSamples"
					)
			)
		) return;

		if
		(
			!(
				SND_gst_GameCubeTools.pfi_GetNibblesForNSamples = (SND_tdpfi_GCFunc1) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"getNibbleAddress"
					)
			)
		) return;

		if
		(
			!(
				SND_gst_GameCubeTools.pfi_GetSampleForAdpcmNibble = (SND_tdpfi_GCFunc1) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"getSampleForAdpcmNibble"
					)
			)
		) return;

		if
		(
			!(
				SND_gst_GameCubeTools.pfi_GetBytesForAdpcmInfo = (SND_tdpfi_GCFunc2) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"getBytesForAdpcmInfo"
					)
			)
		) return;

		if
		(
			!(
				SND_gst_GameCubeTools.pfv_Encode = (SND_tdpfv_GCFunc3) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"encode"
					)
			)
		) return;

		if
		(
			!(
				SND_gst_GameCubeTools.pfv_Decode = (SND_tdpfv_GCFunc4) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"decode"
					)
			)
		) return;

		if
		(
			!(
				SND_gst_GameCubeTools.pfv_GetLoopContext = (SND_tdpfv_GCFunc5) GetProcAddress
					(
						SND_gst_GameCubeTools.h_Dll,
						"getLoopContext"
					)
			)
		) return;

		SND_gst_GameCubeTools.b_Valid = TRUE;
		SND_gi_GameCubeToolsLoaded = 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CGCAdpcmCodec::~CGCAdpcmCodec(void)
{
//	if(SND_gst_GameCubeTools.h_Dll) FreeLibrary(SND_gst_GameCubeTools.h_Dll);
//	SND_gst_GameCubeTools.b_Valid = FALSE;
//	SND_gi_GameCubeToolsLoaded = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CGCAdpcmCodec::CreateFormat(WORD nChannels, DWORD nSamplesPerSec, WAVEFORMATEX *pwfxFormat)
{
	memset(&m_WaveFmtEx, 0, sizeof(WAVEFORMATEX));

    m_WaveFmtEx.wFormatTag = SND_Cte_WAVE_FORMAT_GAMECUBE;
	m_WaveFmtEx.nSamplesPerSec = nSamplesPerSec;
	m_WaveFmtEx.nChannels = nChannels;
	m_WaveFmtEx.wBitsPerSample = 4;
	m_WaveFmtEx.nBlockAlign = 8;
	m_WaveFmtEx.nAvgBytesPerSec = (m_WaveFmtEx.nChannels * m_WaveFmtEx.nSamplesPerSec * m_WaveFmtEx.wBitsPerSample) / 8;
	m_WaveFmtEx.cbSize = 0;

	memcpy(pwfxFormat, &m_WaveFmtEx, sizeof(WAVEFORMATEX));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CGCAdpcmCodec::Initialize(WAVEFORMATEX *pwfxEncode, BOOL bMux)
{
	if(pwfxEncode->nChannels == 1)
		mi_EncoderIdx = 1;
	else if(bMux)
		mi_EncoderIdx = 2;
	else
		mi_EncoderIdx = 3;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CGCAdpcmCodec::Convert(void *pvSrc, void *pvDst, unsigned int uiSize)
{
	switch(mi_EncoderIdx)
	{
	case 1:		return bEncodeMono(pvSrc, uiSize, pvDst, uiGetEncodedFileSize(uiSize));
	case 2:		return bEncodeStereoMux(pvSrc, pvDst, uiSize);
	case 3:		return bEncodeStereoConcat(pvSrc, pvDst, uiSize);
	default:	return FALSE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CGCAdpcmCodec::bEncodeMono(void *pvSrc, unsigned int uiSrcSize, void *pvDst, unsigned int uiDstSize)
{
	mui_DataSize = uiSrcSize;
	mpv_DataPtr = pvSrc;

	mui_EncodedSize = uiDstSize;
	mpv_EncodedPtr = pvDst;

	EncodeBuffer();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CGCAdpcmCodec::bEncodeStereoMux(void *pvSrc, void *pvDst, unsigned int uiExpandedSize)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	*pLeft, *pRight;
    unsigned int uiExpandedSizeByChanel;
    unsigned int uiEncodedSizeByChanel;
	/*~~~~~~~~~~~~~~~~~~~~*/

    uiExpandedSizeByChanel = uiExpandedSize / 2;
    uiEncodedSizeByChanel = uiGetEncodedFileSize(uiExpandedSizeByChanel);

	pLeft = new char[uiExpandedSizeByChanel];
	pRight = new char[uiExpandedSizeByChanel];
	SplitStereoPcm(pvSrc, uiExpandedSize, pLeft, pRight);

    
    bEncodeMono(pRight, uiExpandedSizeByChanel, pvDst, uiEncodedSizeByChanel);
    memcpy(pRight, pvDst, uiEncodedSizeByChanel);

    
    bEncodeMono(pLeft, uiExpandedSizeByChanel, (char *) pvDst + uiEncodedSizeByChanel, uiEncodedSizeByChanel);
    memcpy(pLeft, (char *) pvDst + uiEncodedSizeByChanel, uiEncodedSizeByChanel);

    MuxStereoAdpcm(pRight, pLeft, uiEncodedSizeByChanel, pvDst);

	delete[] pLeft;
	delete[] pRight;

	return TRUE;
}

void CGCAdpcmCodec::MuxStereoAdpcm(void*pLeft, void*pRight, unsigned int uiSize, void*pvDst)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	uiBlockSize;
	char*pOut, *pInL, *pInR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pOut = (char*) pvDst;
	pInL = (char*) pLeft;
	pInR = (char*) pRight;

    uiBlockSize = 25600; // 25600 = 1600x16 = 448 samples x 100
    while(uiSize > uiBlockSize)
    {
	    memcpy(pOut, pInL, uiBlockSize);
        pOut += uiBlockSize;
        pInL += uiBlockSize;

        memcpy(pOut, pInR, uiBlockSize);
        pOut += uiBlockSize;
        pInR += uiBlockSize;

        uiSize -= uiBlockSize;
	}

    if(uiSize)
    {
	    memcpy(pOut, pInL, uiSize);
        pOut += uiSize;
        pInL += uiSize;

        memcpy(pOut, pInR, uiSize);
        pOut += uiSize;
        pInR += uiSize;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CGCAdpcmCodec::bEncodeStereoConcat(void *pvSrc, void *pvDst, unsigned int uiSize)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	*pLeft, *pRight;
	/*~~~~~~~~~~~~~~~~~~~~*/

	pLeft = new char[uiSize / 2];
	pRight = new char[uiSize / 2];
	SplitStereoPcm(pvSrc, uiSize, pLeft, pRight);

	bEncodeMono(pRight, uiSize / 2, pvDst, uiGetEncodedFileSize(uiSize / 2));
	bEncodeMono(pLeft, uiSize / 2, (char *) pvDst + uiGetEncodedFileSize(uiSize / 2), uiGetEncodedFileSize(uiSize / 2));

	delete[] pLeft;
	delete[] pRight;

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CGCAdpcmCodec::SplitStereoPcm(void *pvInBuffer, unsigned int uiSize, void *pvLeftBuffer, void *pvRightBuffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	i;
	short			*pIn, *pOutL, *pOutR;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pIn = (short *) pvInBuffer;
	pOutL = (short *) pvLeftBuffer;
	pOutR = (short *) pvRightBuffer;

	for(i = 0; i < uiSize; i += (2 * sizeof(short)))
	{
		*pOutL++ = *pIn++;
		*pOutR++ = *pIn++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int CGCAdpcmCodec::uiGetEncodedFileSize(int iSize_Src)
{
    unsigned int uiSize;

    uiSize = (unsigned int)SND_gst_GameCubeTools.pfi_GetBytesForAdpcmBuffer(iSize_Src / sizeof(short)) ;
    return uiSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CGCAdpcmCodec::EncodeBuffer(void)
{
    // reset the destination
    memset(mpv_EncodedPtr, 0, mui_EncodedSize);

    // reset pred...
    SND_gst_GameCubeCoefficientsTable.gain = 0;
    SND_gst_GameCubeCoefficientsTable.loop_pred_scale= 0;
    SND_gst_GameCubeCoefficientsTable.loop_yn1= 0;
    SND_gst_GameCubeCoefficientsTable.loop_yn2= 0;
    SND_gst_GameCubeCoefficientsTable.pred_scale= 0;
    SND_gst_GameCubeCoefficientsTable.yn1= 0;
    SND_gst_GameCubeCoefficientsTable.yn2= 0;
    
    SND_gst_GameCubeTools.pfv_Encode
			(
				(short *) mpv_DataPtr,
				(unsigned char *)mpv_EncodedPtr ,
				&SND_gst_GameCubeCoefficientsTable,
				mui_DataSize / 2
			);
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
