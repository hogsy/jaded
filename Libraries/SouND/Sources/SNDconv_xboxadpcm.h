/*$T SNDconv_xboxadpcm.h GC! 1.081 02/18/03 15:17:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __SNDconv_xboxadpcm_h__
#define __SNDconv_xboxadpcm_h__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


/*$4
 ***********************************************************************************************************************
    Prototypes
 ***********************************************************************************************************************
 */
_inline_ unsigned int SND_ui_GetDecompressedSize(unsigned int _ui_SizeIn)
{
	return ((32*_ui_SizeIn)/9);
}

_inline_ unsigned int SND_ui_GetCompressedSize(unsigned int _ui_SizeIn)
{
	return ((9*_ui_SizeIn)/32);
}

BOOL	SND_b_Decode(void* pvSrc, void*pvDst, unsigned int cBlocks, int channel);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SNDconv_xboxadpcm_h__ */ 
 