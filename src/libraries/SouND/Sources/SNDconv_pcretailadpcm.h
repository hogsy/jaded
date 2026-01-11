/*$T SNDconv_xboxadpcm.h GC! 1.081 02/18/03 15:17:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDconv_pcretailadpcm_h__
#define __SNDconv_pcretailadpcm_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
/*
#ifdef C_Standard_MicrosoftADPCM_BlockSize

#define C_MSADPCM_MONO_BLOCKSIZE			 512
#define C_MSADPCM_MONO_SAMPLESPERBLOCK		1012
#define C_MSADPCM_STEREO_BLOCKSIZE			2048
#define C_MSADPCM_STEREO_SAMPLESPERBLOCK	2034

#else
*/
#define C_MSADPCM_MONO_BLOCKSIZE			  36
#define C_MSADPCM_MONO_SAMPLESPERBLOCK		  60
#define C_MSADPCM_STEREO_BLOCKSIZE			  72
#define C_MSADPCM_STEREO_SAMPLESPERBLOCK	  60

//#define C_MSADPCM_STEREO_SAMPLESPERBLOCK	  58
/*
#endif //C_Standard_MicrosoftADPCM_BlockSize
*/
#define C_MSADPCM_HeaderSize				  50

/*$4
 ***********************************************************************************************************************
    Prototypes
 ***********************************************************************************************************************
 */
unsigned int pcretailSND_ui_GetDecompressedSize(unsigned int _ui_SizeIn, int numChannels);
unsigned int pcretailSND_ui_GetCompressedSize(unsigned int _ui_SizeIn, int numChannels);
int	pcretailSND_b_Decode(void* pvSrc, void*pvDst, unsigned int cBlocks, int channel);
int pcretailSND_GetBlocksBySize(int size, int channel);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDconv_pcretailadpcm_h__ */ 
 