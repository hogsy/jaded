/*$T iopSND_DmaScheduler.h GC 1.138 06/02/04 09:57:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP

/*$T iopSND_DmaScheduler.h GC 1.138 06/02/04 09:57:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __iopSND_DmaScheduler_h__
#define __iopSND_DmaScheduler_h__

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define DMA_ID1 1
#define DMA_ID0 0

/* command & status */
#define iopSND_Cte_MskDma							0x000000FF
#define iopSND_Cte_MskCmd							0xFFFFFF00

#define iopSND_Cte_DmaFree							0x00000000
#define iopSND_Cte_DmaWaitingOneShotLoading			0x00000001
#define iopSND_Cte_DmaWaitingStream0				0x00000003
#define iopSND_Cte_DmaWaitingStream1				0x00000004
#define iopSND_Cte_DmaStreamLoading0				0x00000005
#define iopSND_Cte_DmaStreamLoading1				0x00000006
#define iopSND_Cte_DmaStreamLoading2				0x00000007
#define iopSND_Cte_DmaStreamLoading3				0x00000008
#define iopSND_Cte_DmaCancel						0x00000009
#define iopSND_Cte_DmaWaitingStreamLoadAndPlay		0x0000000A
#define iopSND_Cte_DmaRunningStreamLoadAndPlay		0x0000000B
#define iopSND_Cte_DmaStopStream					0x0000000C
#define iopSND_Cte_DmaPlayRq						0x0000000D
#define iopSND_Cte_DmaCLIRead0						0x0000000F
#define iopSND_Cte_DmaCLIRead1						0x00000010
#define iopSND_Cte_DmaCLIRead2						0x00000011
#define iopSND_Cte_DmaCLIRead3						0x00000012
#define iopSND_Cte_DmaCLIRead4						0x00000013
#define iopSND_Cte_DmaCLIRead5						0x00000014
#define iopSND_Cte_DmaCLIRead6						0x00000015
#define iopSND_Cte_DmaCLIRead7						0x00000016
#define iopSND_Cte_DmaCLIRead8						0x00000017
#define iopSND_Cte_DmaCLIRead9						0x00000018
#define iopSND_Cte_DmaWaitingStreamPrefetch			0x00000019
#define iopSND_Cte_DmaWaitingReload					0x0000001A
#define iopSND_Cte_DmaStreamLoading0AndPlayStereo	0x0000001B
#define iopSND_Cte_DmaStreamLoading2AndPlayStereo	0x0000001C

#define iopSND_Cte_DmaTransOneShot					1
#define iopSND_Cte_DmaTransStream0					2
#define iopSND_Cte_DmaTransStream1					3
#define iopSND_Cte_DmaTransStream2					4
#define iopSND_Cte_DmaTransStream3					5

#define iopSND_Cte_DmaRqNb							(2 * SND_Cte_MaxSoftBufferNb)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef enum	iopSND_tden_StreamBufferState_
{
	en_BuffInit		= 0,
	en_BuffWritting = 1,
	en_BuffWritten	= 2,
	en_BuffReading	= 3,
	en_Buffdummy	= 0xFFFFFFFF
} iopSND_tden_StreamBufferState;

typedef struct	iopSND_tdst_StreamBufferRefresh_
{
	int								i_BufferId;
	int								i_ChangeCount;
	iopSND_tden_StreamBufferState	e_BufferState[4];
} iopSND_tdst_StreamBufferRefresh;

extern iopSND_tdst_StreamBufferRefresh	iopSND_vai_StreamBufferCpt[];

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

extern char			*iopSND_gac_Dma0Buff;
extern char			*iopSND_gac_Dma1Buff;
extern volatile int iopSND_gi_DmaEvent[iopSND_Cte_CoreNb];
extern int			iopSND_gi_RequestLock;
extern volatile int iopSND_vb_SchedulerLocked;
extern int			iopSND_gi_DmaSignalSema;
extern int			iopSND_gi_PendingDmaRqSema;
extern volatile int iopSND_TransferingVoice[iopSND_Cte_CoreNb];

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

/*$2- modules --------------------------------------------------------------------------------------------------------*/

void	iopSND_DmaSchedulerInitModule(void);
void	iopSND_DmaSchedulerCloseModule(void);

/*$2- dma thread -----------------------------------------------------------------------------------------------------*/

void	iopSND_DmaThread(void);
int		iopSND_SchedulerResetBuffer(int iVoice);
void	iopSND_RequestDelete(int _i_voice);
void	iopSND_RequestRegister(int _i_Voice, int i_mode);
void	iopSND_RequestWrite(int _i_voice, int _i_request);
void	iopSND_RequestReplace(int _i_voice, int iOld, int iNew);

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#ifdef __cplusplus
}
#endif
#endif /* __iopSND_VOICE_H__ */
#endif /* PSX2_IOP */
