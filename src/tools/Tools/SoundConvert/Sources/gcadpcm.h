/*$T ps2adpcm.h GC 1.138 05/26/04 17:31:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __gcadpcm_h__
#define __gcadpcm_h__

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

typedef struct	SND_tdst_GameCubeADPCMInfo_
{
	/* start context */
	short			coef[16];
	unsigned short	gain;
	unsigned short	pred_scale;
	short			yn1;
	short			yn2;

	/* loop context */
	unsigned short	loop_pred_scale;
	short			loop_yn1;
	short			loop_yn2;
} SND_tdst_GameCubeADPCMInfo;
class	CGCAdpcmCodec
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	int						mi_EncoderIdx;
	/**/
	/**/
	unsigned int			mui_DataSize;
	void					*mpv_DataPtr;
	/**/
	unsigned int			mui_EncodedSize;
	void					*mpv_EncodedPtr;

	WAVEFORMATEX			m_WaveFmtEx;

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	void			EncodeBuffer(void);
	BOOL			bEncodeMono(void *pvSrc, unsigned int uiSrcSize, void *pvDst, unsigned int uiDstSize);
	BOOL			bEncodeStereoMux(void *pvSrc, void *pvDst, unsigned int uiSize);
	BOOL			bEncodeStereoConcat(void *pvSrc, void *pvDst, unsigned int uiSize);
	void			SplitStereoPcm(void *pvInBuffer, unsigned int uiSize, void *pvLeftBuffer, void *pvRightBuffer);
    void            MuxStereoAdpcm(void*pLeft, void*pRight, unsigned int uiSize, void*pvDst);

	int				EncodeFileBuffer(void *pvMem_Wav, int iSize_Wav, void **ppvMem_Vag, int *piSize_Vag);

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	CGCAdpcmCodec(void);
	~CGCAdpcmCodec(void);

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	unsigned int	uiGetEncodedFileSize(int);
	void			CreateFormat(WORD nChannels, DWORD nSamplesPerSec, WAVEFORMATEX *pwfxFormat);
	BOOL			Initialize(WAVEFORMATEX *pwfxEncode, BOOL bMux);
	BOOL			Convert(void *pvSrc, void *pvDst, unsigned int uiSize);
};



/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
