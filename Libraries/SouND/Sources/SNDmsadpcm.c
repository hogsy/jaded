#include "Precomp.h"

typedef struct ms_adpcm_state
{
	unsigned char	predictor;
	unsigned short	delta;
	short		sample1, sample2;
} ms_adpcm_state;

typedef struct ms_adpcm_data
{
	//XTE
	//OLD
	//_Track *track;
	//AFvirtualfile *fh;
	///*
	//	We set framesToIgnore during a reset1 and add it to
	//	framesToIgnore during a reset2.
	//*/
	//AFframecount	framesToIgnore;
	//NEW
	int numChannels;
	//XTE

	int		blockAlign, samplesPerBlock;

	/* a is an array of numCoefficients ADPCM coefficient pairs. */
	int	numCoefficients;
	short	coefficients[256][2];

	//XTE
} ms_adpcm_data;

/*	Compute a linear PCM value from the given differential coded value. */

static short ms_adpcm_decode_sample (struct ms_adpcm_state *state,
	unsigned char code, const short *coefficient)
{
	const int MAX_INT16 = 32767, MIN_INT16 = -32768;
	const int adaptive[] =
	{
		230, 230, 230, 230, 307, 409, 512, 614,
		768, 614, 512, 409, 307, 230, 230, 230
	};
	int	linearSample, delta;

	linearSample = ((state->sample1 * coefficient[0]) +
		(state->sample2 * coefficient[1])) / 256;

	if (code & 0x08)
		linearSample += state->delta * (code-0x10);
	else
		linearSample += state->delta * code;

	/* Clamp linearSample to a signed 16-bit value. */
	if (linearSample < MIN_INT16)
		linearSample = MIN_INT16;
	else if (linearSample > MAX_INT16)
		linearSample = MAX_INT16;

	delta = ((int) state->delta * adaptive[code])/256;
	if (delta < 16)
	{
		delta = 16;
	}

	state->delta = delta;
	state->sample2 = state->sample1;
	state->sample1 = linearSample;

	/*
		Because of earlier range checking, new_sample will be
		in the range of an short.
	*/
	return (short) linearSample;
}

/* Decode one block of MS ADPCM data. */
static int ms_adpcm_decode_block (ms_adpcm_data *msadpcm, unsigned char *encoded,short *decoded)
{
	int		i, outputLength, samplesRemaining;
	int		channelCount;
	short		*coefficient[2];
	ms_adpcm_state	decoderState[2];
	ms_adpcm_state	*state[2];

	//XTE
	//OLD
	///* Calculate the number of bytes needed for decoded data. */
	//outputLength = msadpcm->samplesPerBlock * sizeof (short) *
	//	msadpcm->track->f.channelCount;
	//channelCount = msadpcm->track->f.channelCount;
	//NEW
	/* Calculate the number of bytes needed for decoded data. */
	outputLength = msadpcm->samplesPerBlock * sizeof (short) *
		msadpcm->numChannels;
	channelCount = msadpcm->numChannels;
	//XTE
	state[0] = &decoderState[0];
	if (channelCount == 2)
		state[1] = &decoderState[1];
	else
		state[1] = &decoderState[0];
	/* Initialize predictor. */
	for (i=0; i<channelCount; i++)
	{
		state[i]->predictor = *encoded++;
		if( (state[i]->predictor>7) || (state[i]->predictor<0) )
			i = i;
		//XTE
		//assert(state[i]->predictor < msadpcm->numCoefficients);
		//XTE
	}
	/* Initialize delta. */
	for (i=0; i<channelCount; i++)
	{
		state[i]->delta = (encoded[1]<<8) | encoded[0];
		encoded += sizeof (unsigned short);
	}
	/* Initialize first two samples. */
	for (i=0; i<channelCount; i++)
	{
		state[i]->sample1 = (encoded[1]<<8) | encoded[0];
		encoded += sizeof (unsigned short);
	}
	for (i=0; i<channelCount; i++)
	{
		state[i]->sample2 = (encoded[1]<<8) | encoded[0];
		encoded += sizeof (unsigned short);
	}
	coefficient[0] = msadpcm->coefficients[state[0]->predictor];
	coefficient[1] = msadpcm->coefficients[state[1]->predictor];
	for (i=0; i<channelCount; i++)
		*decoded++ = state[i]->sample2;
	for (i=0; i<channelCount; i++)
		*decoded++ = state[i]->sample1;
	/*
		The first two samples have already been 'decoded' in
		the block header.
	*/
	//XTE
	//OLD
	//samplesRemaining = (msadpcm->samplesPerBlock - 2) *
	//	msadpcm->track->f.channelCount;
	//NEW
	samplesRemaining = (msadpcm->samplesPerBlock - 2) *
		msadpcm->numChannels;
	//XTE
	while (samplesRemaining > 0)
	{
		unsigned char	code;
		short		newSample;

		code = *encoded >> 4;
		newSample = ms_adpcm_decode_sample(state[0], code,
			coefficient[0]);
		*decoded++ = newSample;

		code = *encoded & 0x0f;
		newSample = ms_adpcm_decode_sample(state[1], code,
			coefficient[1]);
		*decoded++ = newSample;

		encoded++;
		samplesRemaining -= 2;
	}

	return outputLength;
}

int DecodeMSADPCMData(char* pInputBuffer, short* pOutputBuffer, unsigned numSamples,unsigned short numChannels, unsigned short blockSize,unsigned short SamplesPerBlock)
{
	struct ms_adpcm_data My_ms_adpcm_data;
	unsigned char *pCurrentInputBuffer;
	short *pCurrentOutputBuffer;
	int numDecoded;
	short coefficients1 [] = {256, 512, 0, 192, 240, 460, 392 };
	short coefficients2 [] = { 0, -256, 0, 64, 0, -208, -232 };
	unsigned numBlocks,residualSamples,i;

	for(i=0;i<7;++i)
	{
		My_ms_adpcm_data.coefficients[i][0] = coefficients1[i];
		My_ms_adpcm_data.coefficients[i][1] = coefficients2[i];
	}
	pCurrentInputBuffer = (unsigned char*) pInputBuffer;
	pCurrentOutputBuffer = pOutputBuffer;
	My_ms_adpcm_data.numChannels = numChannels;
	My_ms_adpcm_data.samplesPerBlock = SamplesPerBlock;

	numBlocks = numSamples / SamplesPerBlock;
	for(i=0;i<numBlocks;++i)
	{
		//decode a block
		numDecoded = ms_adpcm_decode_block(&My_ms_adpcm_data,pCurrentInputBuffer,pCurrentOutputBuffer);
		numDecoded /= 2;
		//Move ahead pointers in the block
		pCurrentInputBuffer += blockSize;
		pCurrentOutputBuffer += numDecoded;
	}
	//Decode last unfilled block
	residualSamples = numSamples % SamplesPerBlock;
	if( residualSamples )
	{
		My_ms_adpcm_data.samplesPerBlock = residualSamples;
		numDecoded = ms_adpcm_decode_block(&My_ms_adpcm_data,pCurrentInputBuffer,pCurrentOutputBuffer);
	}

	return 0;
}

