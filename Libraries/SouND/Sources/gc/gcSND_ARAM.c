/*$T gcSND_ARAM.c GC! 1.097 04/22/02 11:10:09 */


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

#include "BASe/BASsys.h"
#include "GameCube/GC_arammng.h"
#include "GameCube/GC_aramheap.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"

#include "Sound/Sources/gc/gcSND_ARAM.h"
#include "Sound/Sources/SNDstruct.h"
#include "Sound/Sources/SNDconst.h"
#include "Sound/Sources/SNDtrack.h"
#include "Sound/Sources/SNDstream.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

static void		gcSND_ZeroBufferCleaned(u32 task);

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

unsigned int	gcSND_gaui_ARAMPartition[gcSND_e_ARAMNumberOfType];
unsigned int	gcSND_gui_ZeroBuffer;
ARQRequest		gcSND_gst_ARQRqZeroBuffer;
volatile int	gcSND_gi_ZeroBufferCleaned;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_ARAMInit(void)
{
	/*~~~~~~~~~~~~~*/
	char	*pc_Temp;
	/*~~~~~~~~~~~~~*/

	/*$2- create the partition of ARAM -------------------------------------------------------------------------------*/

	gcSND_gaui_ARAMPartition[gcSND_e_ARAMZeroBuffer] = gcSND_Cte_ZeroBufferSize;

	/* fix */
	gcSND_gaui_ARAMPartition[gcSND_e_ARAMStreaming] = ((SND_Cte_MaxSimultaneousStream+SND_Cte_StreamPrefetchMax) * SND_Cte_MaxBufferSize);

	/* wav */
	gcSND_gaui_ARAMPartition[gcSND_e_ARAMOneShot] = g_u32ARAM_BlocksSize[eARAM_Audio] -
		gcSND_gaui_ARAMPartition[gcSND_e_ARAMZeroBuffer] -
		gcSND_gaui_ARAMPartition[gcSND_e_ARAMStreaming];

	ARAM_CreateHeaps(eARAM_Audio, gcSND_e_ARAMNumberOfType, gcSND_gaui_ARAMPartition);

	/*$2- create the zero buffer -------------------------------------------------------------------------------------*/

	/* alloc */
	gcSND_gui_ZeroBuffer = ARAM_Alloc
		(
			eARAM_Audio,
			gcSND_e_ARAMZeroBuffer,
			gcSND_Cte_ZeroBufferSize,
			FirstFreeBlockAlloc
		);

	/* reset the zero buffer */
	pc_Temp = MEM_p_AllocAlign(gcSND_Cte_ZeroBufferSize, 32);
	L_memset(pc_Temp, 0, gcSND_Cte_ZeroBufferSize);
	gcSND_gi_ZeroBufferCleaned = 0;

	ARQPostRequest
	(
		&gcSND_gst_ARQRqZeroBuffer,
		0,
		ARQ_TYPE_MRAM_TO_ARAM,
		ARQ_PRIORITY_HIGH,
		(u32) pc_Temp,
		gcSND_gui_ZeroBuffer,
		gcSND_Cte_ZeroBufferSize,
		gcSND_ZeroBufferCleaned
	);

	while(!gcSND_gi_ZeroBufferCleaned)
	{
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_ARAMClose(void)
{
	ARAM_Free(eARAM_Audio, gcSND_e_ARAMZeroBuffer, gcSND_gui_ZeroBuffer);
	gcSND_gui_ZeroBuffer = 0;
	ARAM_DestroyHeaps(eARAM_Audio);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void gcSND_ZeroBufferCleaned(u32 task)
{
	/*~~~~~~~~~~~*/
	ARQRequest	*p;
	/*~~~~~~~~~~~*/

	p = (ARQRequest *) task;
	if(p->source) MEM_FreeAlign((void *) p->source);
	gcSND_gi_ZeroBufferCleaned = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *gcSND_pv_AllocARAM(unsigned int _ui_Size)
{
	return (void*)ARAM_Alloc(eARAM_Audio, gcSND_e_ARAMOneShot, _ui_Size, FirstFreeBlockAlloc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_FreeARAM(void *_pv_Addr)
{
	ARAM_Free(eARAM_Audio, gcSND_e_ARAMOneShot, (unsigned int) _pv_Addr);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *gcSND_pv_StreamAllocARAM(unsigned int _ui_Size)
{
	return (void *) ARAM_Alloc(eARAM_Audio, gcSND_e_ARAMStreaming, _ui_Size, FirstFreeBlockAlloc);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcSND_StreamFreeARAM(void *_pv_Addr)
{
	ARAM_Free(eARAM_Audio, gcSND_e_ARAMStreaming, (unsigned int) _pv_Addr);
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _GAMECUBE */
