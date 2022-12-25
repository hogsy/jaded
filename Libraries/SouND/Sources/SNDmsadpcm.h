#ifndef __SNDmsadpcm_h__
#define __SNDmsadpcm_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" 
{
#endif //__cplusplus
/*__cdecl*/ int DecodeMSADPCMData(char* pInputBuffer, short* pOutputBuffer, unsigned size,unsigned short numChannels, unsigned short blockSize,unsigned short SamplesPerBlock);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif //__cplusplus

#endif //__SNDmsadpcm_h__
