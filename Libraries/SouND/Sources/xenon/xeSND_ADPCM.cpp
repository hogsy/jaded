// ****************************************
// Xenon ADPCM Decoder
//
// This code actually decodes XBOX-format
// ADPCM audio files, but is adapted for
// Xenon's Big-Endian internal processing.
//
// By Alexandre David (January 2005)
// ****************************************

#include "Precomp.h"

#if defined(_XENON)

// ***********************************************************************************************************************
//    Headers
// ***********************************************************************************************************************

#include "BIGfiles/LOAding/LOAread.h"

// ***********************************************************************************************************************
//    Macros
// ***********************************************************************************************************************

#define GetElementCount(Param_Array) (sizeof(Param_Array) / sizeof(Param_Array[0]))

// ***********************************************************************************************************************
//    Constants
// ***********************************************************************************************************************

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

// ***********************************************************************************************************************

const short m_asNextStep[16] =
{
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8
};

// ***********************************************************************************************************************
//    Functions
// ***********************************************************************************************************************

unsigned int xeSND_ADPCM_GetDecompressedSize(unsigned int Param_uiSize)
{
	return ((32 * Param_uiSize) / 9);
}

// ***********************************************************************************************************************

unsigned int xeSND_ADPCM_GetCompressedSize(unsigned int Param_uiSize)
{
	return (( 9 * Param_uiSize) / 32);
}

// ***********************************************************************************************************************

int GetNextStepIndex(int Param_iEncodedSample, int Param_iStepIndex)
{
	// Raise Step Index By Increment

	Param_iStepIndex += m_asNextStep[Param_iEncodedSample];

	// Clamp Index To Array Bounds

	if (Param_iStepIndex < 0)
	{
		Param_iStepIndex = 0;
	}

	if (Param_iStepIndex >= GetElementCount(m_asStep))
	{
		Param_iStepIndex = GetElementCount(m_asStep) - 1;
	}

	// Return New Index

	return Param_iStepIndex;
}

// ***********************************************************************************************************************

int DecodeSample(int Param_iEncodedSample, int Param_iPredictedSample, int Param_iStepSize)
{
	// Decode Delta From 4-Bit Nibble

	int iDifference = Param_iStepSize >> 3;

	if (Param_iEncodedSample & 4)
	{
		iDifference += Param_iStepSize;
	}

	if (Param_iEncodedSample & 2)
	{
		iDifference += Param_iStepSize >> 1;
	}

	if (Param_iEncodedSample & 1)
	{
		iDifference += Param_iStepSize >> 2;
	}

	if (Param_iEncodedSample & 8)
	{
		iDifference = -iDifference;
	}

	// Calculate New Sample

	int iNewSample = Param_iPredictedSample + iDifference;

	// Clamp Sample To 16 Bits Signed

	if ((int)(short)iNewSample != iNewSample)
	{
		if (iNewSample < -32768)
		{
			iNewSample = -32768;
		}
		else
		{
			iNewSample = 32767;
		}
	}

	// Return New Sample

	return iNewSample;
}

// ***********************************************************************************************************************

void xeSND_ADPCM_Decode_Mono16(char* pSourceBuffer, char* pDestBuffer, int Param_iBlockCount, int Param_iBlockAlignment, int Param_iSamplesPerBlock)
{
	while (Param_iBlockCount > 0)
	{
		char*	pBlock			= pSourceBuffer;
		int		iSampleCount	= Param_iSamplesPerBlock - 1;

		// Block Header

		int	iHeader	= *(int*)pBlock;
		pBlock		+= sizeof(int);

		SwapDWord(&iHeader);

		int iPredictedSample	= (int)(short)iHeader;
		int iStepIndex			= (iHeader >> 16) & 0xFF;

		// Output First Sample

		*(short*)pDestBuffer	= (short)iPredictedSample;
		pDestBuffer				+= sizeof(short);

		int iEndianCounter = 0;

		// Block Loop

		while (iSampleCount > 0)
		{
			int iSample = (int)*pBlock;

			pBlock++;

			// Sample 1

			int iEncodedSample	= iSample & 0xF;

			iPredictedSample	= DecodeSample(iEncodedSample, iPredictedSample, m_asStep[iStepIndex]);
			iStepIndex			= GetNextStepIndex(iEncodedSample, iStepIndex);

			*(short*)pDestBuffer	= (short)iPredictedSample;
			pDestBuffer				+= sizeof(short);

			iSampleCount--;

			// Sample 2

			if (iSampleCount > 0)
			{
				iEncodedSample		= (iSample >> 4) & 0xF;

				iPredictedSample	= DecodeSample(iEncodedSample, iPredictedSample, m_asStep[iStepIndex]);
				iStepIndex			= GetNextStepIndex(iEncodedSample, iStepIndex);

				*(short*)pDestBuffer	= (short)iPredictedSample;
				pDestBuffer				+= sizeof(short);

				iSampleCount--;
			}
		}

		// Skip Padding

		pSourceBuffer += Param_iBlockAlignment;

		Param_iBlockCount--;
	}
}

// ***********************************************************************************************************************

void xeSND_ADPCM_Decode_Stereo16(char* pSourceBuffer, char* pDestBuffer, int Param_iBlockCount, int Param_iBlockAlignment, int Param_iSamplesPerBlock)
{
	while (Param_iBlockCount > 0)
	{
		char*	pBlock			= pSourceBuffer;
		int		iSampleCount	= Param_iSamplesPerBlock - 1;

		// Left Channel Header

		int	iHeader	= *(int*)pBlock;
		pBlock		+= sizeof(int);

		SwapDWord(&iHeader);

		int iPredictedSampleLeft	= (int)(short)iHeader;
		int iStepIndexLeft			= (iHeader >> 16) & 0xFF;

		// Right Channel Header

		iHeader	= *(int*)pBlock;
		pBlock	+= sizeof(int);

		SwapDWord(&iHeader);

		int iPredictedSampleRight	= (int)(short)iHeader;
		int iStepIndexRight			= (iHeader >> 16) & 0xFF;

		// Output First Sample

		*(int*)pDestBuffer	= MAKELONG(iPredictedSampleLeft, iPredictedSampleRight);
		pDestBuffer			+= sizeof(int);

		while (iSampleCount > 0)
		{
			// Read 4 Left Samples

			int iSampleLeft		= *(int*)pBlock;
			pBlock				+= sizeof(int);

			SwapDWord(&iSampleLeft);

			// Read 4 Right Samples

			int iSampleRight	= *(int*)pBlock;
			pBlock				+= sizeof(int);

			SwapDWord(&iSampleRight);

			// Process Subsamples

			int iSubSampleCount = min(iSampleCount, 8);

			for (int iLoop = 0; iLoop < iSubSampleCount; iLoop++)
			{
				// Left Channel

				int iEncodedSample		= (iSampleLeft & 0xF);
				int iStepSize			= m_asStep[iStepIndexLeft];

				iPredictedSampleLeft	= DecodeSample(iEncodedSample, iPredictedSampleLeft, iStepSize);
				iStepIndexLeft			= GetNextStepIndex(iEncodedSample, iStepIndexLeft);

				// Right Channel

				iEncodedSample			= (iSampleRight & 0xF);
				iStepSize				= m_asStep[iStepIndexRight];

				iPredictedSampleRight	= DecodeSample(iEncodedSample, iPredictedSampleRight, iStepSize);
				iStepIndexRight			= GetNextStepIndex(iEncodedSample, iStepIndexRight);

				// Output Sample

				*(int*)pDestBuffer = MAKELONG(iPredictedSampleLeft, iPredictedSampleRight);
				pDestBuffer += sizeof(int);

				// Shift Next Input Samples Into Lower 4 Bits

				iSampleLeft		>>= 4;
				iSampleRight	>>= 4;
			}

			iSampleCount -= iSubSampleCount;
		}

		// Skip Padding

		pSourceBuffer += Param_iBlockAlignment;

		Param_iBlockCount--;
	}
}

// ***********************************************************************************************************************

void xeSND_ADPCM_Decode(char* Param_pSourceBuffer, char* Param_pDestBuffer, unsigned int Param_iBlockCount, int Param_iChannelCount)
{
	// Error Checking

	if ((Param_pSourceBuffer == NULL) || (Param_pDestBuffer == NULL))
	{
		return;
	}

	if (Param_iChannelCount == 1)
	{
		xeSND_ADPCM_Decode_Mono16(Param_pSourceBuffer, Param_pDestBuffer, Param_iBlockCount, 36 * Param_iChannelCount, 64);
	}
	else
	{
		xeSND_ADPCM_Decode_Stereo16(Param_pSourceBuffer, Param_pDestBuffer, Param_iBlockCount, 36 * Param_iChannelCount, 64);
	}
}

// ***********************************************************************************************************************

#endif // _XENON
