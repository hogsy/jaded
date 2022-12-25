/*$T SNDconv_xboxadpcm.c GC! 1.081 02/26/03 15:41:09 */


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

#if defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL) || defined(_PC_RETAIL)

#include <dsound.h>

/*$4
 ***********************************************************************************************************************
    MACROS
 ***********************************************************************************************************************
 */

#define NUMELMS(a)	(sizeof(a) / sizeof(a[0]))

/*$4
 ***********************************************************************************************************************
    PROTOTYPES
 ***********************************************************************************************************************
 */

static int	DecodeSample(int nEncodedSample, int nPredictedSample, int nStepSize);
static BOOL SND_b_DecodeS16(LPBYTE pbSrc, LPBYTE pbDst, UINT cBlocks, UINT nBlockAlignment, UINT cSamplesPerBlock);
static BOOL SND_b_DecodeM16(LPBYTE pbSrc, LPBYTE pbDst, UINT cBlocks, UINT nBlockAlignment, UINT cSamplesPerBlock);

/*$4
 ***********************************************************************************************************************
    PRIVATE
 ***********************************************************************************************************************
 */
/*$off*/
const short m_asStep[89] =
{
		7,	   8,	  9,	10,	   11,	  12,	 13,
	   14,	  16,	 17,	19,	   21,	  23,	 25,
	   28,	  31,	 34,	37,	   41,	  45,	 50,
	   55,	  60,	 66,	73,	   80,	  88,	 97,
	  107,	 118,	130,   143,	  157,	 173,	190,
	  209,	 230,	253,   279,	  307,	 337,	371,
	  408,	 449,	494,   544,	  598,	 658,	724,
	  796,	 876,	963,  1060,	 1166,	1282,  1411,
	 1552,	1707,  1878,  2066,	 2272,	2499,  2749,
	 3024,	3327,  3660,  4026,	 4428,	4871,  5358,
	 5894,	6484,  7132,  7845,	 8630,	9493, 10442,
	11487, 12635, 13899, 15289, 16818, 18500, 20350,
	22385, 24623, 27086, 29794, 32767
};

const short m_asNextStep[16] =
{
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};
/*$on*/

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SND_b_Decode(void *pvSrc, void *pvDst, unsigned int cBlocks, int channel)
{
	switch(channel)
	{
	case 1:
		return SND_b_DecodeM16((LPBYTE) pvSrc, (LPBYTE) pvDst, cBlocks, 36 * channel, 64);
	case 2:
		return SND_b_DecodeS16((LPBYTE) pvSrc, (LPBYTE) pvDst, cBlocks, 36 * channel, 64);
	default: return FALSE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__inline BOOL ValidStepIndex(int nStepIndex)
{
	return (nStepIndex >= 0) && (nStepIndex < NUMELMS(m_asStep));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
__inline int NextStepIndex(int nEncodedSample, int nStepIndex)
{
	nStepIndex += m_asNextStep[nEncodedSample];

	if(nStepIndex < 0)
	{
		nStepIndex = 0;
	}
	else if(nStepIndex >= NUMELMS(m_asStep))
	{
		nStepIndex = NUMELMS(m_asStep) - 1;
	}

	return nStepIndex;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL SND_b_DecodeM16(LPBYTE pbSrc, LPBYTE pbDst, UINT cBlocks, UINT nBlockAlignment, UINT cSamplesPerBlock)
{
	/*~~~~~~~~~~~~~~~~*/
	BOOL	fSuccess;
	LPBYTE	pbBlock;
	UINT	cSamples;
	BYTE	bSample;
	int		nStepSize;
	int		nEncSample;
	int		nPredSample;
	int		nStepIndex;
	DWORD	dwHeader;
	/*~~~~~~~~~~~~~~~~*/

	fSuccess = TRUE;

	/* Enter the main loop */
	while(cBlocks--)
	{
		pbBlock = pbSrc;
		cSamples = cSamplesPerBlock - 1;

		/* Block header */
		dwHeader = *(LPDWORD) pbBlock;
		pbBlock += sizeof(DWORD);

		nPredSample = (int) (short) LOWORD(dwHeader);
		nStepIndex = (int) (BYTE) HIWORD(dwHeader);

		if(!ValidStepIndex(nStepIndex))
		{
			/*
			 * The step index is out of range - this is considered a fatal error as the input
			 * stream is corrupted. We fail by returning zero bytes converted.
			 */
			fSuccess = FALSE;
			break;
		}

		/* Write out first sample */
		*(short *) pbDst = (short) nPredSample;
		pbDst += sizeof(short);

		/* Enter the block loop */
		while(cSamples)
		{
			bSample = *pbBlock++;

			/* Sample 1 */
			nEncSample = (bSample & (BYTE) 0x0F);
			nStepSize = m_asStep[nStepIndex];
			nPredSample = DecodeSample(nEncSample, nPredSample, nStepSize);
			nStepIndex = NextStepIndex(nEncSample, nStepIndex);

			*(short *) pbDst = (short) nPredSample;
			pbDst += sizeof(short);

			cSamples--;

			/* Sample 2 */
			if(cSamples)
			{
				nEncSample = (bSample >> 4);
				nStepSize = m_asStep[nStepIndex];
				nPredSample = DecodeSample(nEncSample, nPredSample, nStepSize);
				nStepIndex = NextStepIndex(nEncSample, nStepIndex);

				*(short *) pbDst = (short) nPredSample;
				pbDst += sizeof(short);

				cSamples--;
			}
		}

		/* Skip padding */
		pbSrc += nBlockAlignment;
	}

	return fSuccess;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL SND_b_DecodeS16(LPBYTE pbSrc, LPBYTE pbDst, UINT cBlocks, UINT nBlockAlignment, UINT cSamplesPerBlock)
{
	/*~~~~~~~~~~~~~~~~~*/
	BOOL	fSuccess;
	LPBYTE	pbBlock;
	UINT	cSamples;
	UINT	cSubSamples;
	int		nStepSize;
	DWORD	dwHeader;
	DWORD	dwLeft;
	DWORD	dwRight;
	int		nEncSampleL;
	int		nPredSampleL;
	int		nStepIndexL;
	int		nEncSampleR;
	int		nPredSampleR;
	int		nStepIndexR;
	UINT	i;
	/*~~~~~~~~~~~~~~~~~*/

	fSuccess = TRUE;

	/* Enter the main loop */
	while(cBlocks--)
	{
		pbBlock = pbSrc;
		cSamples = cSamplesPerBlock - 1;

		/* LEFT channel header */
		dwHeader = *(LPDWORD) pbBlock;
		pbBlock += sizeof(DWORD);

		nPredSampleL = (int) (short) LOWORD(dwHeader);
		nStepIndexL = (int) (BYTE) HIWORD(dwHeader);

		if(!ValidStepIndex(nStepIndexL))
		{
			/*
			 * The step index is out of range - this is considered a fatal error as the input
			 * stream is corrupted. We fail by returning zero bytes converted.
			 */
			fSuccess = FALSE;
			break;
		}

		/* RIGHT channel header */
		dwHeader = *(LPDWORD) pbBlock;
		pbBlock += sizeof(DWORD);

		nPredSampleR = (int) (short) LOWORD(dwHeader);
		nStepIndexR = (int) (BYTE) HIWORD(dwHeader);

		if(!ValidStepIndex(nStepIndexR))
		{
			/*
			 * The step index is out of range - this is considered a fatal error as the input
			 * stream is corrupted. We fail by returning zero bytes converted.
			 */
			fSuccess = FALSE;
			break;
		}

		/* Write out first sample */
		*(LPDWORD) pbDst = MAKELONG(nPredSampleL, nPredSampleR);
		pbDst += sizeof(DWORD);

		/*
		 * The first DWORD contains 4 left samples, the second DWORD contains 4 right
		 * samples. We process the source in 8-byte chunks to make it easy to interleave
		 * the output correctly.
		 */
		while(cSamples)
		{
			dwLeft = *(LPDWORD) pbBlock;
			pbBlock += sizeof(DWORD);
			dwRight = *(LPDWORD) pbBlock;
			pbBlock += sizeof(DWORD);

			cSubSamples = min(cSamples, 8);

			for(i = 0; i < cSubSamples; i++)
			{
				/* LEFT channel */
				nEncSampleL = (dwLeft & 0x0F);
				nStepSize = m_asStep[nStepIndexL];
				nPredSampleL = DecodeSample(nEncSampleL, nPredSampleL, nStepSize);
				nStepIndexL = NextStepIndex(nEncSampleL, nStepIndexL);

				/* RIGHT channel */
				nEncSampleR = (dwRight & 0x0F);
				nStepSize = m_asStep[nStepIndexR];
				nPredSampleR = DecodeSample(nEncSampleR, nPredSampleR, nStepSize);
				nStepIndexR = NextStepIndex(nEncSampleR, nStepIndexR);

				/* Write out sample */
				*(LPDWORD) pbDst = MAKELONG(nPredSampleL, nPredSampleR);
				pbDst += sizeof(DWORD);

				/* Shift the next input sample into the low-order 4 bits. */
				dwLeft >>= 4;
				dwRight >>= 4;
			}

			cSamples -= cSubSamples;
		}

		/* Skip padding */
		pbSrc += nBlockAlignment;
	}

	return fSuccess;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int DecodeSample(int nEncodedSample, int nPredictedSample, int nStepSize)
{
	/*~~~~~~~~~~~~~~~~*/
	LONG	lDifference;
	LONG	lNewSample;
	/*~~~~~~~~~~~~~~~~*/

	lDifference = nStepSize >> 3;

	if(nEncodedSample & 4)
	{
		lDifference += nStepSize;
	}

	if(nEncodedSample & 2)
	{
		lDifference += nStepSize >> 1;
	}

	if(nEncodedSample & 1)
	{
		lDifference += nStepSize >> 2;
	}

	if(nEncodedSample & 8)
	{
		lDifference = -lDifference;
	}

	lNewSample = nPredictedSample + lDifference;

	if((LONG) (short) lNewSample != lNewSample)
	{
		if(lNewSample < -32768)
		{
			lNewSample = -32768;
		}
		else
		{
			lNewSample = 32767;
		}
	}

	return (int) lNewSample;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
