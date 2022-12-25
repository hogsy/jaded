#ifndef _CWATER_FFT_
#define _CWATER_FFT_

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
#define SPECTRE_SIZE_MAX 12 // 2048
#else
#define SPECTRE_SIZE_MAX 8 // 1024
#endif


typedef struct FFT_Complex_
{
	float re,im;
} FFT_Complex;

typedef struct FFT_ComplexCOS_
{
	float re1,im1;
	float re2,im2;
#ifdef PSX2_TARGET
	float re3,im3;
	float re4,im4;
#endif	
}	FFT_ComplexCOS;

typedef struct Vector2_
{	
	float X,Y;
} Vector2;

typedef struct tdst_FFT_
{
	/* ******************************** // BEGIN */
	/* User have to set that */
	unsigned int	BorderSizePo2;		// Big map 
	unsigned int	DXDYBorderSizePo2; 	// Choppy Map (muste be <= BorderSizePo2)
	Vector2			Wind;				//
	float			WindBalance;		// 1 BACK and FRONT , 0 FRONT 
	float			WorldSizeInMeters;	// move faster if small
	float			P_Factor;			// Nothing
	
	/* ******************************** // END */
	

	/* ********************************* PRIVATE, muste be set to 0 */
	FFT_Complex		*pSpectroMap;
	unsigned int	ulSpectromapCompressedSize;
	FFT_Complex		*pDXDYMap;
	unsigned int    ulDXDYCompressedSize;
	int				lSuperShift0,lSuperShift1,lSuperShift2;
	FFT_ComplexCOS 	FFT_COSTABLE[(1 << SPECTRE_SIZE_MAX) + 100] ONLY_PSX2_ALIGNED(32);
	FFT_ComplexCOS 	FFT_COSTABLEDXDY[(1 << SPECTRE_SIZE_MAX) + 100] ONLY_PSX2_ALIGNED(32);
	/* ********************************* PRIVATE end */

} tdst_FFT;


void WATERFFT_Init_Spectre(tdst_FFT *p_FFT);
void WATERFFT_Destroy_Spectre(tdst_FFT *p_FFT);
void WATERFFT_Turn_Spectre(tdst_FFT *p_FFTSRC , FFT_Complex *pDST , FFT_Complex *pDX, FFT_Complex *pDY , float timeinsecond);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif	