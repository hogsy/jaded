#include "Precomp.h"

#if defined(_PC_RETAIL) || defined(ACTIVE_EDITORS)

#include "SouND\Sources\SNDmsadpcm.h"
#include "SouND\Sources\SNDconv_pcretailadpcm.h"

//Decode the PCM data to Microsoft ADPCM

int pcretailSND_b_Decode(void* pvSrc, void*pvDst, unsigned int cBlocks, int channel)
{
	unsigned samplesPerBlock = ( (channel==1) ? C_MSADPCM_MONO_SAMPLESPERBLOCK : C_MSADPCM_STEREO_SAMPLESPERBLOCK );
	unsigned numSamples = cBlocks * samplesPerBlock;
	unsigned blockSize = (channel==1) ? C_MSADPCM_MONO_BLOCKSIZE : C_MSADPCM_STEREO_BLOCKSIZE;

	DecodeMSADPCMData((char*)pvSrc,(short*)pvDst,numSamples,channel,blockSize,samplesPerBlock);

	return 1;
}

unsigned int pcretailSND_ui_GetDecompressedSize(unsigned int _ui_SizeIn, int numChannels)
{
	if( numChannels == 1 )
		return ( (_ui_SizeIn / C_MSADPCM_MONO_BLOCKSIZE) * C_MSADPCM_MONO_SAMPLESPERBLOCK ) * 2;
	else
		return (_ui_SizeIn/C_MSADPCM_STEREO_BLOCKSIZE) * C_MSADPCM_STEREO_SAMPLESPERBLOCK * numChannels * 2;
}

unsigned int pcretailSND_ui_GetCompressedSize(unsigned int _ui_SizeIn, int numChannels)
{
	unsigned samples = (_ui_SizeIn / 2) / numChannels;


	if( numChannels == 1 )
		return ( (samples / C_MSADPCM_MONO_SAMPLESPERBLOCK) * C_MSADPCM_MONO_BLOCKSIZE );
	else
		return ( (samples / C_MSADPCM_STEREO_SAMPLESPERBLOCK) * C_MSADPCM_STEREO_BLOCKSIZE );
}

int pcretailSND_GetBlocksBySize(int size, int channel)
{
	if( channel == 1 )
		return size / C_MSADPCM_MONO_BLOCKSIZE;
	else
		return size / C_MSADPCM_STEREO_BLOCKSIZE;
}

#endif //_PC_RETAIL