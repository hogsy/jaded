/*$T iopSND_Stream.h GC 1.138 06/30/04 09:19:21 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP
#ifndef __iopSND_Stream_h__
#define __iopSND_Stream_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int iopSND_i_InitStreamModule(void);
int iopSND_i_CloseStreamModule(void);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void	iopSND_ReinitAndPlayStreamLong(void *);
int		iopSND_i_CreateStreamBuffer(void *);
int		iopSND_i_ReinitAndPlayStream_r(int dma, int _i_SndBuffId);
void	iopSND_StopAllStream(void);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void			iopSND_StreamIopPrefetch(struct RPC_tdst_VArg_SndStreamPrefetch_ *);
int				iopSND_i_StreamPrefetchArray(struct RPC_tdst_VArg_SndStreamPrefetch_ *, unsigned int *);

void			iopSND_StreamPrefetchDo(void);

unsigned int	iopSND_ui_StreamPrefetchGet(char*p, unsigned int _ui_Position, unsigned int);

void			iopSND_StreamIopFlush(struct RPC_tdst_VArg_SndStreamFlush_ *);
void			iopSND_StreamIopFlushAll(void);
void			iopSND_StreamIopPrefetchStatus(unsigned int *auiPos);

void			iopSND_StreamIopPrefetchLock(struct RPC_tdst_VArg_SndStreamPrefetch_ *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void	iopSND_StreamSetPos(void *);
void	iopSND_StreamGetPos(int id, void *p2);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void	iopSND_StreamChain(struct RPC_tdst_SndStreamChain_ *data);
void	iopSND_StreamLoopCountGet(int id, int *piLoop);
void	iopSND_StreamChainDelayGet(int id, int *piSize);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __iopSND_Stream_h__ */
#endif /* PSX2_IOP */
