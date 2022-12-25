/*$T OGLrequest.h GC! 1.077 03/30/00 15:11:28 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __GSPVIDEO_H__
#define __GSPVIDEO_H__
#include "BASe/BAStypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */
typedef struct {
    u_int d4madr; // 停止したときの D4_MADR レジスタの値
    u_int d4tadr; // 停止したときの D4_TADR レジスタの値
    u_int d4qwc;  // 停止したときの D4_QWC レジスタの値
    u_int d4chcr; // 停止したときの D4_CHCR レジスタの値
    u_int d3madr; // 停止したときの D3_MADR レジスタの値
    u_int d3qwc;  // 停止したときの D3_QWC レジスタの値
    u_int d3chcr; // 停止したときの D3_CHCR レジスタの値
    u_int ipubp;  // 停止したときの IPU_BP レジスタの値
    u_int ipuctrl;// 停止したときの IPU_CTRL レジスタの値
} GSP_sceIpuDmaEnv;
 
#define GSP_StrBuf_Ready_For_IO		0
#define GSP_StrBuf_Ready_For_SOFT	1
#define GSP_StrBuf_Used_By_IO		2
#define GSP_StrBuf_Used_By_SOFT		3
#define GSP_StrBuf_Used_By_Trash	4
#define GSP_StrBuf_Ready_For_Ready_For_SOFT 5

typedef struct GSP_StreamBuffer_
{
	u_char	*pBuffer;
	u_int 	ulState;
	int 	lRequestNumber;
	struct GSP_StreamBuffer_ *p_NextBuffer;
	struct GSP_StreamBuffer_ *p_PreviousBuffer;
}
GSP_StreamBuffer;

#define GSP_Str_RotativBufferNum 32
extern BOOL MTX_b_CurrentFileIsMTX(void);
#define M_GSP_Str_RotativBufferNum()    (MTX_b_CurrentFileIsMTX()?GSP_Str_RotativBufferNum:(GSP_Str_RotativBufferNum>>2))

typedef struct GSP_Stream_
{
	int		iSourceFile;
	GSP_StreamBuffer	stBuffers[GSP_Str_RotativBufferNum];
	GSP_StreamBuffer	*pCurrentBufferSOFT;
	GSP_StreamBuffer	*pCurrentBufferIO;
	u_int	ulCurrentBufferSize;
	u_int	ulBufferSize;
	u_int	ulFileSize;
	u_int	ulBufferCurrentPos;
	u_int 	ulValidBufferNum;
	u_int 	ulBigPosStart;
	u_int 	ulBigPosEnd;
	u_int 	ulBigPosCurrent;
} GSP_Stream;


typedef struct GSP_VideoStream_
{
	u_int W,H;
	u_int ulNP;
	u_int *p_DSTBuffer;
	u_int *p_OutBuffer;
	u_int ulCurrentP;
	GSP_Stream stSrcStream;
	GSP_sceIpuDmaEnv stPushedState;
	BOOL    b_SelfAllocated;
	BOOL	b_Enterlace;
	u_int 	BlackScreenCounter;
} GSP_VideoStream;

u_int GSP_VideoStreamCreate( GSP_VideoStream *p_VideoStr , u_int ulBigPos , u_int MaxImageSize , u_int *p_OutBuffer);
void GSP_VideoStreamClose(GSP_VideoStream *p_VideoStr );
void GSP_VideoStreamSync(GSP_VideoStream *p_VideoStr);
void GSP_VideoScreenSet(GSP_VideoStream *p_VideoStr , signed int X, signed int Y);
void GSP_VideoStreamRestart(GSP_VideoStream *p_VideoStr );


void GSP_VideoFullScreenCreate(ULONG ulBigKey );
void GSP_VideoFullScreenUpdate();
void GSP_VideoFullScreenRestart();
void GSP_VideoFullScreenKill();


#ifdef __cplusplus
}
#endif
#endif
