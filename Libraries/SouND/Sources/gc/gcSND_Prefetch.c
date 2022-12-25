/*$T gcSND_Prefetch.c GC 1.138 12/08/04 09:27:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef _GAMECUBE

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include <dolphin/mix.h>

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGopen.h"

#include "GameCube/GC_File.h"
#include "GameCube/GC_Stream.h"

#include "SouND/Sources/SNDspecific.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/gc/gcSND.h"
#include "SouND/Sources/gc/gcSND_Stream.h"
#include "SouND/Sources/gc/gcSND_Debug.h"
#include "SouND/Sources/gc/gcSND_Soft.h"
#include "SouND/Sources/gc/gcSND_ARAM.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/gc/gcSND_Prefetch.h"

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static void gcSND_PrefetchDvdCallback(s32 result, DVDFileInfo *, unsigned int context);
static void gcSND_PrefetchAramCallback(u32 task);
static void gcSND_PrefetchRamCallback(u32 task);

/**/
extern void CheckMediaError(void);
/**/
extern void gcSND_GetNewParam(volatile gcSND_tdst_Stream *pst_Stream);
extern int	gcSND_i_StreamPlay(volatile gcSND_tdst_Stream *pst_Stream);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

gcSND_tdst_Prefetch gcSND_gst_PrefetchList[SND_Cte_StreamPrefetchMax];
volatile int		gcSND_gi_PendingPrefetch;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamPrefetch(unsigned int _ui_Position, unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	gcSND_tdst_Prefetch *pst_Prefetch;
	BOOL				old;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_ui_Position) return;
	if(!_ui_Size) return;

	old = OSDisableInterrupts();
	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = gcSND_gst_PrefetchList + i;
		if(pst_Prefetch->en_State != e_PrefetchFree) continue;

		if(!pst_Prefetch->pst_SoundFileHandler)
			pst_Prefetch->pst_SoundFileHandler = GC_pst_StreamOpen(BIG_gst.h_CLibFileHandle, 2);
		gcSND_M_Assert(pst_Prefetch->pst_SoundFileHandler);

		pst_Prefetch->ui_PrefetchPosition = _ui_Position;
		pst_Prefetch->ui_PrefetchSize = min(_ui_Size, SND_Cte_MaxBufferSize / 2);

		if(!pst_Prefetch->pc_RamBuffer) pst_Prefetch->pc_RamBuffer = MEM_p_AllocAlign(SND_Cte_MaxBufferSize / 2, 64);
		gcSND_M_Assert(pst_Prefetch->pc_RamBuffer);

		if(!pst_Prefetch->pst_SoundFileHandler || !pst_Prefetch->pc_RamBuffer)
		{
			OSRestoreInterrupts(old);
			return;
		}

		L_memset(pst_Prefetch->pc_RamBuffer, 0, SND_Cte_MaxBufferSize / 2);

		gcSND_gi_PendingPrefetch++;
		if(gcSND_gi_PendingPrefetch > SND_Cte_StreamPrefetchMax) gcSND_gi_PendingPrefetch = SND_Cte_StreamPrefetchMax;

		pst_Prefetch->en_State = e_PrefetchLoadingRam;
		GC_i_StreamRead
		(
			pst_Prefetch->pst_SoundFileHandler,
			pst_Prefetch->pc_RamBuffer,
			pst_Prefetch->ui_PrefetchSize,
			pst_Prefetch->ui_PrefetchPosition,
			gcSND_PrefetchDvdCallback,
			(unsigned int) pst_Prefetch
		);
		break;
	}

	OSRestoreInterrupts(old);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_PrefetchDvdCallback(s32 result, DVDFileInfo *fileInfo, unsigned int context)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	gcSND_tdst_Prefetch *pst_Prefetch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Prefetch = (gcSND_tdst_Prefetch *) context;

	/* check current state */
	if(pst_Prefetch->en_State != e_PrefetchLoadingRam) return;

	if(result == DVD_RESULT_GOOD)
	{
		pst_Prefetch->en_State = e_PrefetchLoadingAram;
		DCFlushRange(pst_Prefetch->pc_RamBuffer, SND_Cte_MaxBufferSize / 2);

		ARQPostRequest
		(
			&pst_Prefetch->st_ARQRequest,
			(u32) pst_Prefetch,
			ARQ_TYPE_MRAM_TO_ARAM,
			ARQ_PRIORITY_HIGH,
			(u32) pst_Prefetch->pc_RamBuffer,
			(u32) pst_Prefetch->pc_AramBuffer,
			SND_Cte_MaxBufferSize / 2,
			gcSND_PrefetchAramCallback
		);
	}
	else
	{
		gcSND_gi_PendingPrefetch--;
		if(gcSND_gi_PendingPrefetch < 0) gcSND_gi_PendingPrefetch = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_PrefetchAramCallback(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest			*p;
	gcSND_tdst_Prefetch *pst_Prefetch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	pst_Prefetch = (gcSND_tdst_Prefetch *) p->owner;

	if(pst_Prefetch->en_State != e_PrefetchLoadingAram) return;
	pst_Prefetch->en_State = e_PrefetchWaitingForUse;

	GC_StreamClose(pst_Prefetch->pst_SoundFileHandler);
	pst_Prefetch->pst_SoundFileHandler = NULL;

	MEM_FreeAlign(pst_Prefetch->pc_RamBuffer);
	pst_Prefetch->pc_RamBuffer = NULL;

	gcSND_gi_PendingPrefetch--;
	if(gcSND_gi_PendingPrefetch < 0) gcSND_gi_PendingPrefetch = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamFlush(unsigned int _ui_Position)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	gcSND_tdst_Prefetch *pst_Prefetch;
	BOOL				old;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_ui_Position) return;

	old = OSDisableInterrupts();
	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = gcSND_gst_PrefetchList + i;
		if(pst_Prefetch->en_State == e_PrefetchFree) continue;
		if(pst_Prefetch->ui_PrefetchPosition != _ui_Position) continue;

		switch(pst_Prefetch->en_State)
		{
		case e_PrefetchFree:
			break;

		case e_PrefetchLoadingRam:
			GC_StreamCancel(pst_Prefetch->pst_SoundFileHandler);
			break;

		case e_PrefetchLoadingAram:
			ARQRemoveOwnerRequest((u32) pst_Prefetch);
			break;

		case e_PrefetchRestoringRam:
			if(pst_Prefetch->pfv_UserCallBack)
			{
				((GC_tdpfv_StreamCallBack) pst_Prefetch->pfv_UserCallBack)
					(
						DVD_RESULT_CANCELED, &pst_Prefetch->pst_UserStreamHandler->st_FileInfo, pst_Prefetch->
							ui_UserContext
					);
			}

			ARQRemoveOwnerRequest((u32) pst_Prefetch);
			break;

		case e_PrefetchWaitingForUse:
			break;

		default:
			break;
		}

		pst_Prefetch->en_State = e_PrefetchFree;

		gcSND_gi_PendingPrefetch--;
		if(gcSND_gi_PendingPrefetch < 0) gcSND_gi_PendingPrefetch = 0;
		if(pst_Prefetch->pst_SoundFileHandler) GC_StreamClose(pst_Prefetch->pst_SoundFileHandler);
		pst_Prefetch->pst_SoundFileHandler = NULL;

		if(pst_Prefetch->pc_RamBuffer) MEM_FreeAlign(pst_Prefetch->pc_RamBuffer);
		pst_Prefetch->pc_RamBuffer = NULL;
	}

	OSRestoreInterrupts(old);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamPrefetchArray(unsigned int *_pui_Position, unsigned int *_pui_Size)
{
	/*~~*/
	int i;
	/*~~*/

	gcSND_gi_PendingPrefetch = 0;

	for(i = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		if(_pui_Position[i]) gcSND_StreamPrefetch(_pui_Position[i], _pui_Size[i]);
		_pui_Position[i] = 0;
	}

	while(gcSND_gi_PendingPrefetch)
	{
		CheckMediaError();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamGetPrefetchStatus(unsigned int *auiPos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j;
	gcSND_tdst_Prefetch *pst_Prefetch;
	BOOL				old;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	old = OSDisableInterrupts();
	for(i = j = 0; i < SND_Cte_StreamPrefetchMax; i++)
	{
		pst_Prefetch = gcSND_gst_PrefetchList + i;
		if(pst_Prefetch->en_State == e_PrefetchWaitingForUse) continue;
		auiPos[j++] = pst_Prefetch->ui_PrefetchPosition;
	}

	OSRestoreInterrupts(old);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int Overloaded_GC_i_StreamRead
(
	GC_tdst_StreamHandler	*_pst_Stream,
	char					*_pc_Buff,
	unsigned int			_ui_Size,
	unsigned int			_ui_Position,
	void					*_pfv_CallBack,
	unsigned int			_ui_Context
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	gcSND_tdst_Prefetch *pst_Prefetch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0, pst_Prefetch = gcSND_gst_PrefetchList; i < SND_Cte_StreamPrefetchMax; i++, pst_Prefetch++)
	{
		if(pst_Prefetch->en_State == e_PrefetchFree) continue;
		if(pst_Prefetch->ui_PrefetchPosition != _ui_Position) continue;

		if(pst_Prefetch->en_State == e_PrefetchWaitingForUse)
		{
			/* register all user settings */
			pst_Prefetch->pst_UserStreamHandler = _pst_Stream;
			pst_Prefetch->ui_UserBufferSize = _ui_Size;
			pst_Prefetch->pc_UserBufferPointer = _pc_Buff;
			pst_Prefetch->ui_UserFilePointer = _ui_Position;
			pst_Prefetch->pfv_UserCallBack = _pfv_CallBack;
			pst_Prefetch->ui_UserContext = _ui_Context;

			/* restore data to Main RAM */
			pst_Prefetch->pc_RamBuffer = MEM_p_AllocAlign(SND_Cte_MaxBufferSize / 2, 64);
			pst_Prefetch->en_State = e_PrefetchRestoringRam;

			ARQPostRequest
			(
				&pst_Prefetch->st_ARQRequest,
				(u32) pst_Prefetch,
				ARQ_TYPE_ARAM_TO_MRAM,
				ARQ_PRIORITY_HIGH,
				(u32) pst_Prefetch->pc_AramBuffer,
				(u32) pst_Prefetch->pc_RamBuffer,
				SND_Cte_MaxBufferSize / 2,
				gcSND_PrefetchRamCallback
			);
    		return 0;
		}
	}

	return GC_i_StreamRead(_pst_Stream, _pc_Buff, _ui_Size, _ui_Position, _pfv_CallBack, _ui_Context);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_PrefetchRamCallback(u32 task)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ARQRequest			*p;
	gcSND_tdst_Prefetch *pst_Prefetch;
	unsigned int		uiSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	pst_Prefetch = (gcSND_tdst_Prefetch *) p->owner;
	if(pst_Prefetch->en_State != e_PrefetchRestoringRam) return;

	/* flush RAM */
	DCFlushRange(pst_Prefetch->pc_RamBuffer, SND_Cte_MaxBufferSize / 2);

	/* copy data to user buffer */
	uiSize = min(pst_Prefetch->ui_PrefetchSize, pst_Prefetch->ui_UserBufferSize);
	L_memcpy(pst_Prefetch->pc_UserBufferPointer, pst_Prefetch->pc_RamBuffer, uiSize);

	/* free prefetch buffer */
	MEM_FreeAlign(pst_Prefetch->pc_RamBuffer);
	pst_Prefetch->pc_RamBuffer = NULL;

	/* update user data */
	pst_Prefetch->ui_UserBufferSize -= uiSize;
	pst_Prefetch->pc_UserBufferPointer += uiSize;
	pst_Prefetch->ui_UserFilePointer += uiSize;

	/* any more data ? */
	if(pst_Prefetch->ui_UserBufferSize)
	{
		GC_i_StreamRead
		(
			pst_Prefetch->pst_UserStreamHandler,
			pst_Prefetch->pc_UserBufferPointer,
			pst_Prefetch->ui_UserBufferSize,
			pst_Prefetch->ui_UserFilePointer,
			pst_Prefetch->pfv_UserCallBack,
			pst_Prefetch->ui_UserContext
		);
	}
	else if(pst_Prefetch->pfv_UserCallBack)
	{
		((GC_tdpfv_StreamCallBack) pst_Prefetch->pfv_UserCallBack) (DVD_RESULT_GOOD, &pst_Prefetch->pst_UserStreamHandler->st_FileInfo, pst_Prefetch->ui_UserContext);
	}

	pst_Prefetch->en_State = e_PrefetchFree;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _GAMECUBE */
