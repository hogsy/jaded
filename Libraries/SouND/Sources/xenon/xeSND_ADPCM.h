// ****************************************
// Xenon ADPCM Decoder
//
// This code actually decodes XBOX-format
// ADPCM audio files, but is adapted for
// Xenon's Big-Endian internal processing.
//
// By Alexandre David (January 2005)
// ****************************************

#ifndef __xeSND_ADPCM_h__
#define __xeSND_ADPCM_h__

// ***********************************************************************************************************************
//    Prototypes
// ***********************************************************************************************************************

unsigned int	xeSND_ADPCM_GetDecompressedSize(unsigned int Param_uiSize);
unsigned int	xeSND_ADPCM_GetCompressedSize(unsigned int Param_uiSize);
void			xeSND_ADPCM_Decode(char* Param_pSourceBuffer, char* Param_pDestBuffer, unsigned int Param_iBlockCount, int Param_iChannelCount);

// ***********************************************************************************************************************

#endif // __xeSND_ADPCM_h__
 