/*$T ps2adpcm.cpp GC 1.138 05/26/04 17:31:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "stdafx.h"
#include <DSOUND.H>
#include <math.h>
#include <stdio.h>
#include "ps2adpcm.h"
#include "CWaveFile.h"

#define C_WAV2VAG_BEFORE	56
#define C_WAV2VAG_AFTER		16
#define M_SUCCEEDED(a)		((a) >= 0)

/*
 =======================================================================================================================
    vag struct £
    private struct
 =======================================================================================================================
 */
CPS2AdpcmCodec::CPS2AdpcmCodec(void)
{
	ResetCodec();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CPS2AdpcmCodec::~CPS2AdpcmCodec(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CPS2AdpcmCodec::CreateFormat(WORD nChannels, DWORD nSamplesPerSec, WAVEFORMATEX *pwfxFormat)
{
	memset(&m_WaveFmtEx, 0, sizeof(WAVEFORMATEX));

	m_WaveFmtEx.wFormatTag = SND_Cte_WAVE_FORMAT_PS2;
	m_WaveFmtEx.nSamplesPerSec = nSamplesPerSec;
	m_WaveFmtEx.nChannels = nChannels;
	m_WaveFmtEx.wBitsPerSample = 4;
	m_WaveFmtEx.nBlockAlign = 16;
	m_WaveFmtEx.nAvgBytesPerSec = (m_WaveFmtEx.nChannels * m_WaveFmtEx.nSamplesPerSec * m_WaveFmtEx.wBitsPerSample) / 8;
	m_WaveFmtEx.cbSize = 0;

	memcpy(pwfxFormat, &m_WaveFmtEx, sizeof(WAVEFORMATEX));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CPS2AdpcmCodec::Initialize(WAVEFORMATEX *pwfxEncode, BOOL bMux)
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
BOOL CPS2AdpcmCodec::Convert(void *pvSrc, void *pvDst, unsigned int uiSize)
{
	switch(mi_EncoderIdx)
	{
	case 1:		return bEncodeMono(pvSrc, pvDst, uiSize);
	case 2:		return bEncodeStereoMux(pvSrc, pvDst, uiSize);
	case 3:		return bEncodeStereoConcat(pvSrc, pvDst, uiSize);
	default:	return FALSE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CPS2AdpcmCodec::bEncodeMono(void *pvSrc, void *pvDst, unsigned int uiSize)
{
	ResetCodec();

	mui_DataSize = uiSize;
	mpv_DataPtr = pvSrc;

	mui_EncodedSize = uiGetEncodedFileSize(uiSize);
	mpv_EncodedPtr = pvDst;

	EncodeBuffer();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL CPS2AdpcmCodec::bEncodeStereoMux(void *pvSrc, void *pvDst, unsigned int uiSize)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	*pLeft, *pRight;
	/*~~~~~~~~~~~~~~~~~~~~*/

	pLeft = new char[uiSize / 2];
	pRight = new char[uiSize / 2];
	SplitStereoPcm(pvSrc, uiSize, pLeft, pRight);

	bEncodeMono(pRight, pvDst, uiSize / 2);
    memcpy(pRight, pvDst, uiGetEncodedFileSize(uiSize / 2));

	bEncodeMono(pLeft, (char *) pvDst + uiGetEncodedFileSize(uiSize / 2), uiSize / 2);
    memcpy(pLeft, (char *) pvDst + uiGetEncodedFileSize(uiSize / 2), uiGetEncodedFileSize(uiSize / 2));

    MuxStereoAdpcm(pRight, pLeft, uiGetEncodedFileSize(uiSize / 2), pvDst);

	delete[] pLeft;
	delete[] pRight;

	return TRUE;
}

void CPS2AdpcmCodec::MuxStereoAdpcm(void*pLeft, void*pRight, unsigned int uiSize, void*pvDst)
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
BOOL CPS2AdpcmCodec::bEncodeStereoConcat(void *pvSrc, void *pvDst, unsigned int uiSize)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	char	*pLeft, *pRight;
	/*~~~~~~~~~~~~~~~~~~~~*/

	pLeft = new char[uiSize / 2];
	pRight = new char[uiSize / 2];
	SplitStereoPcm(pvSrc, uiSize, pLeft, pRight);

	bEncodeMono(pRight, pvDst, uiSize / 2);
	bEncodeMono(pLeft, (char *) pvDst + uiGetEncodedFileSize(uiSize / 2), uiSize / 2);

	delete[] pLeft;
	delete[] pRight;

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CPS2AdpcmCodec::SplitStereoPcm(void *pvInBuffer, unsigned int uiSize, void *pvLeftBuffer, void *pvRightBuffer)
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
int CPS2AdpcmCodec::ComputeBuffer(void *pvMem_Dst, void *pvMem_Src, int iSize_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_Flags;
	double			d_SamplesArray[28];
	short			s_FourBitArr[28];
	short			*ps_End;
	short			*ps_Begin;
	short			*ps_Tmp;
	short			*ps_VagBody;
	unsigned int	*pul_Tmp;
	int				k;
	char			*pch_Dst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Flags = mst_Arr.iFlags;
	ps_VagBody = (short *) pvMem_Dst;
	ps_Begin = (short *) pvMem_Src;
	ps_End = (short *) ((unsigned int) pvMem_Src + iSize_Src);

	/* Treat pack of 28 samples */
	for(ps_Tmp = ps_Begin; ps_Tmp < ps_End; ps_Tmp += 28, ps_VagBody += 8)
	{
		FindPredict(ps_Tmp, &d_SamplesArray[0], &mst_Arr.iPredictNr, &mst_Arr.iShiftFactor);
		Pack(&d_SamplesArray[0], &s_FourBitArr[0], mst_Arr.iPredictNr, mst_Arr.iShiftFactor);

		/*
		 * The first byte of a VAG data chunk contain: "id" of a filter. "shift_factor"
		 * The second byte contain a flag.
		 */
		pch_Dst = (char *) &ps_VagBody[0];
		*pch_Dst++ = (mst_Arr.iPredictNr << 4) | mst_Arr.iShiftFactor;
		*pch_Dst++ = i_Flags;
		for(k = 0; k < 28; k += 2, pch_Dst)
		{
			*pch_Dst++ = ((s_FourBitArr[k + 1] >> 8) & 0xf0) | ((s_FourBitArr[k] >> 12) & 0xf);
		}
	}

	/* VAG end file. */
	pul_Tmp = (unsigned int *) &ps_VagBody[0];
	pul_Tmp[0] = (mst_Arr.iPredictNr << 4) | mst_Arr.iShiftFactor | 0x700;
	pul_Tmp[1] = 0;
	pul_Tmp[2] = 0;
	pul_Tmp[3] = 0;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CPS2AdpcmCodec::FindPredict(short *samples, double *d_samples, int *predict_nr, int *shift_factor)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int		i, j;
	double	buffer[28][5];
	double	min;
	/* Store the maximum absolute value of each filter. */
	double	max[5];
	double	ds;
	int		min2;
	int		shift_mask;
	/* These two static variables keep the last decrompression state. */
	double	s_0, s_1, s_2;
	/*~~~~~~~~~~~~~~~~~~*/

	min = 1e10;

	/* Apply 5 differents filter, to each 28 samples. */
	for(i = 0; i < 5; i++)
	{
		max[i] = 0.0;
		s_1 = md__s_1;
		s_2 = md__s_2;

		/* Loop on each sample. */
		for(j = 0; j < 28; j++)
		{
			s_0 = (double) samples[j];	/* s[t-0] */

			/* Each sample has to be clamped between: 30749.0 - -30720.0 */
			if(s_0 > 30719.0) s_0 = 30719.0;
			if(s_0 < -30720.0) s_0 = -30720.0;

			/* Algorithme. It's a 2 degree filter. there are 5 combinations of coefficients. */
			ds = s_0 + s_1 * mad_FloatConst[i][0] + s_2 * mad_FloatConst[i][1];
			buffer[j][i] = ds;
			if(fabs(ds) > max[i]) max[i] = fabs(ds);

			s_2 = s_1;					/* new s[t-2] */
			s_1 = s_0;					/* new s[t-1] */
		}

		/*
		 * "min" retrieve the minimal value of each maximal filter result, and store the
		 * id of the filter.
		 */
		if(max[i] < min)
		{
			min = max[i];
			*predict_nr = i;
		}

		if(min <= 7)
		{
			*predict_nr = 0;
			break;
		}
	}

	/*
	 * store s[t-2] and s[t-1] in a static variable £
	 * these than used in the next function call
	 */
	md__s_1 = s_1;
	md__s_2 = s_2;

	/* Copied sample, are those obtained with the selected filter. */
	for(i = 0; i < 28; i++) d_samples[i] = buffer[i][*predict_nr];

	/*
	 * if ( min > 32767.0 ) £
	 * min = 32767.0; £
	 * Computation of "shift_factor".
	 */
	min2 = (int) min;
	shift_mask = 0x4000;
	*shift_factor = 0;

	/* If "shift_factor" > 12 stop. */
	while(*shift_factor < 12)
	{
		if(shift_mask & (min2 + (shift_mask >> 3))) break;
		(*shift_factor)++;
		shift_mask = shift_mask >> 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CPS2AdpcmCodec::Pack(double *d_samples, short *four_bit, int predict_nr, int shift_factor)
{
	/*~~~~~~~~*/
	double	ds;
	int		di;
	double	s_0;
	int		i;
	/*~~~~~~~~*/

	for(i = 0; i < 28; i++)
	{
		s_0 = d_samples[i] + md_s_1 * mad_FloatConst[predict_nr][0] + md_s_2 * mad_FloatConst[predict_nr][1];
		ds = s_0 * (double) (1 << shift_factor);

		di = ((int) ds + 0x800) & 0xfffff000;

		if(di > 32767) di = 32767;
		if(di < -32768) di = -32768;

		four_bit[i] = (short) di;

		di = di >> shift_factor;
		md_s_2 = md_s_1;
		md_s_1 = (double) di - s_0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int CPS2AdpcmCodec::uiGetEncodedFileSize(int iSize_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	ui_Encode;
	unsigned int	ui_NumFrame;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	ui_NumFrame = iSize_Src / C_WAV2VAG_BEFORE;
	if(iSize_Src % C_WAV2VAG_BEFORE != 0) ui_NumFrame++;

	ui_Encode = ui_NumFrame * C_WAV2VAG_AFTER;
    ui_Encode += 16;
	
	return ui_Encode;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CPS2AdpcmCodec::EncodeBuffer(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	ui_NumFrame;
	unsigned int	ui_RawSize;
	unsigned int	ui_EncSize;
	void			*pvMem_EncCur;
	void			*pvMem_RawCur;
	unsigned char	*pu8_Ctrl;
	char			s8_Buffer[C_WAV2VAG_BEFORE];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* get vag data size + ptr */

	pvMem_EncCur = ((char *) (mpv_EncodedPtr) );

	/* eval pcm + adpcm size rounded to frame size */
	ui_NumFrame = mui_DataSize / C_WAV2VAG_BEFORE;
	ui_RawSize = ui_NumFrame * C_WAV2VAG_BEFORE;
	ui_EncSize = ui_NumFrame * C_WAV2VAG_AFTER;

	/* encode */
	ComputeBuffer(pvMem_EncCur, mpv_DataPtr, ui_RawSize);
	pvMem_EncCur = (char *) pvMem_EncCur + ui_EncSize;
	pvMem_RawCur = (char *) mpv_DataPtr + ui_RawSize;

	/* is remaing any data ? */
	if(mui_DataSize % C_WAV2VAG_BEFORE)
	{
		ui_RawSize = mui_DataSize - ui_RawSize;

		memset(s8_Buffer, 0, sizeof(s8_Buffer));
		memcpy(s8_Buffer, pvMem_RawCur, ui_RawSize);

		ComputeBuffer(pvMem_EncCur, s8_Buffer, sizeof(s8_Buffer));
		pvMem_EncCur = (char *) pvMem_EncCur + C_WAV2VAG_AFTER;
	}

	/* add the last frame */
	pu8_Ctrl = &((unsigned char *) pvMem_EncCur)[-15];
	pu8_Ctrl[0] |= 1;
	memcpy(pvMem_EncCur, mu32_Pad, sizeof(mu32_Pad));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CPS2AdpcmCodec::ResetCodec(void)
{
	mi_EncoderIdx = 0;

	md__s_1 = 0.0;	/* s[t-1] */
	md__s_2 = 0.0;	/* s[t-2] */
	md_s_1 = 0.0;
	md_s_2 = 0.0;
/**/
	mu32_Pad[0] = 0x77770007;
	mu32_Pad[1] = 0x77777777;
	mu32_Pad[2] = 0x77777777;
	mu32_Pad[3] = 0x77777777;
/**/
	mad_FloatConst[0][0] = 0.0;
	mad_FloatConst[0][1] = 0.0;

	mad_FloatConst[1][0] = -60.0 / 64.0;
	mad_FloatConst[1][1] = 0.0;

	mad_FloatConst[2][0] = -115.0 / 64.0;
	mad_FloatConst[2][1] = 52.0 / 64.0;

	mad_FloatConst[3][0] = -98.0 / 64.0;
	mad_FloatConst[3][1] = 55.0 / 64.0;

	mad_FloatConst[4][0] = -122.0 / 64.0;
	mad_FloatConst[4][1] = 60.0 / 64.0;

	mst_Arr.iPredictNr = 0;
	mst_Arr.iShiftFactor = 0;
	mst_Arr.iFlags = 0;

	mui_DataSize = 0;
	mpv_DataPtr = NULL;

	mui_EncodedSize = 0;
	mpv_EncodedPtr = NULL;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
