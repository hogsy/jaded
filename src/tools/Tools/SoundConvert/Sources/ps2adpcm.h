/*$T ps2adpcm.h GC 1.138 05/26/04 17:31:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __ps2adpcm_h__
#define __ps2adpcm_h__

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	CPS2AdpcmCodec
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	typedef struct	VAG_tdst_EncodeStruct_
	{
		int iPredictNr;
		int iShiftFactor;
		int iFlags;
	} VAG_tdst_EncodeStruct;

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
private:
	int						mi_EncoderIdx;
	/**/
	VAG_tdst_EncodeStruct	mst_Arr;
	double					mad_FloatConst[5][2];
	double					md__s_1;
	double					md__s_2;
	double					md_s_1;
	double					md_s_2;
	unsigned int			mu32_Pad[4];
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
	unsigned int	Swap32(unsigned int inp);
	void			ResetCodec(void);
	bool			bIsEncodable(VAG_tdst_EncodeStruct *pst_Dst, void *pst_Src);
	int				ComputeBuffer(void *pvMem_Dst, void *pvMem_Src, int iSize_Src);

	void			InitVagHeader(void);
	void			EncodeBuffer(void);
	void			FindPredict(short *samples, double *d_samples, int *predict_nr, int *shift_factor);
	void			Pack(double *d_samples, short *four_bit, int predict_nr, int shift_factor);
	BOOL			bEncodeMono(void *pvSrc, void *pvDst, unsigned int uiSize);
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
	CPS2AdpcmCodec(void);
	~CPS2AdpcmCodec(void);

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

/*
 =======================================================================================================================
 =======================================================================================================================
 */

__inline unsigned int CPS2AdpcmCodec::Swap32(unsigned int inp)
{
	return((inp & 0xFF) << 24) | ((inp & 0xFF00) << 8) | ((inp & 0xFF0000) >> 8) | ((inp & 0xFF000000) >> 24);
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
