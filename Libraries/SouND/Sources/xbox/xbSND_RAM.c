/*$T xbSND_RAM.c GC! 1.081 01/09/03 11:17:48 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#ifdef _XBOX

/*$4
 ***********************************************************************************************************************
    Headers
 ***********************************************************************************************************************
 */

#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/xbox/xbSND.h"
#include "SouND/Sources/xbox/xbSND_RAM.h"
#include "SouND/Sources/xbox/xbSND_Stream.h"
#include "SouND/Sources/xbox/xbSND_debug.h"

/*$4
 ***********************************************************************************************************************
    Types
 ***********************************************************************************************************************
 */

typedef struct	xbSND_tdst_RamPartition_
{
	unsigned int	ui_FirstAddress;
	unsigned int	ui_BlockSize;
	unsigned int	ui_CurrentAllocatedSize;
	unsigned int	ui_CurrentBlockNumber;
} xbSND_tdst_RamPartition;

/*$4
 ***********************************************************************************************************************
    variables
 ***********************************************************************************************************************
 */

xbSND_tdst_RamPartition xbSND_gst_FixRam;
xbSND_tdst_RamPartition xbSND_gst_LevelRam;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xbSND_i_InitRAMModule(void)
{
	L_memset(&xbSND_gst_FixRam, 0, sizeof(xbSND_tdst_RamPartition));
	L_memset(&xbSND_gst_LevelRam, 0, sizeof(xbSND_tdst_RamPartition));

	/*$1- Fix size ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* streamed buffers */
	xbSND_gst_FixRam.ui_BlockSize = 0;	/* stream are uncached -> XPhysicalAlloc */

	/* space for prefetching */
	xbSND_gst_FixRam.ui_BlockSize += (SND_Cte_StreamPrefetchMax * (SND_Cte_MaxBufferSize / 2));

	/* space for WAC loading */
	xbSND_gst_FixRam.ui_BlockSize += 0; /* uncached -> XPhysicalAlloc */

	/*$1- level size ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* 2Mo size will be the upper limit = the PS2 sound RAM size */
	xbSND_gst_LevelRam.ui_BlockSize = (2 * 1024 * 1024);

	/*$1- alloc the big block ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	xbSND_gst_FixRam.ui_FirstAddress = (unsigned int) MEM_p_AllocAlign
		(
			xbSND_gst_FixRam.ui_BlockSize + xbSND_gst_LevelRam.ui_BlockSize,
			xbSND_Cte_AlignSize
		);
	if(!xbSND_gst_FixRam.ui_FirstAddress)
	{
		ERR_X_Warning(0, "[SND] no more memory for sound", NULL);
		return -1;
	}

	xbSND_gst_LevelRam.ui_FirstAddress = xbSND_gst_FixRam.ui_FirstAddress + xbSND_gst_FixRam.ui_BlockSize;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xbSND_CloseRAMModule(void)
{
	if(xbSND_gst_FixRam.ui_FirstAddress) MEM_Free((void *) xbSND_gst_FixRam.ui_FirstAddress);
	L_memset(&xbSND_gst_FixRam, 0, sizeof(xbSND_tdst_RamPartition));
	L_memset(&xbSND_gst_LevelRam, 0, sizeof(xbSND_tdst_RamPartition));
}

/*
 =======================================================================================================================
    allocate space for one stream (WAD, WAA, WAM) or the WAC
 =======================================================================================================================
 */
void *xbSND_AllocFix(unsigned int _ui_size)
{
	/*~~~~~~~~*/
	void	*pv;
	/*~~~~~~~~*/

	if(_ui_size & (xbSND_Cte_AlignSize - 1))
	{
		/* rounds up the size for aligning */
		_ui_size = (_ui_size + xbSND_Cte_AlignSize) &~(xbSND_Cte_AlignSize - 1);
	}

	if((_ui_size + xbSND_gst_FixRam.ui_CurrentAllocatedSize) > xbSND_gst_FixRam.ui_BlockSize)
	{
		ERR_X_Warning(0, "[SND] No more memory", NULL);
		return NULL;
	}

	pv = (void *) (xbSND_gst_FixRam.ui_FirstAddress + xbSND_gst_FixRam.ui_CurrentAllocatedSize);
	xbSND_gst_FixRam.ui_CurrentAllocatedSize += _ui_size;
	xbSND_gst_FixRam.ui_CurrentBlockNumber++;
	return pv;
}

/*
 =======================================================================================================================
    fix is never freed
 =======================================================================================================================
 */
void xbSND_FreeFix(void *_pv_Bloc)
{
	if(!xbSND_gst_FixRam.ui_CurrentBlockNumber) return;
	xbSND_gst_FixRam.ui_CurrentBlockNumber--;
}

/*
 =======================================================================================================================
    allocate space for one sound (WAV)
 =======================================================================================================================
 */
void *xbSND_Alloc(unsigned int _ui_size)
{
	/*~~~~~~~~*/
	void	*pv;
	/*~~~~~~~~*/

	if(_ui_size & (xbSND_Cte_AlignSize - 1))
	{
		/* rounds up the size for aligning */
		_ui_size = (_ui_size + xbSND_Cte_AlignSize) &~(xbSND_Cte_AlignSize - 1);
	}

	if((_ui_size + xbSND_gst_LevelRam.ui_CurrentAllocatedSize) > xbSND_gst_LevelRam.ui_BlockSize)
	{
		xbSND_M_Assert(0);	/* No more memory */
		return NULL;
	}

	pv = (void *) (xbSND_gst_LevelRam.ui_FirstAddress + xbSND_gst_LevelRam.ui_CurrentAllocatedSize);
	xbSND_gst_LevelRam.ui_CurrentAllocatedSize += _ui_size;
	xbSND_gst_LevelRam.ui_CurrentBlockNumber++;

	return pv;
}

/*
 =======================================================================================================================
    sound are never freed except when existing from the map -> all sounds are destroyed
 =======================================================================================================================
 */
void xbSND_Free(void *_pv_Bloc)
{
	if(!xbSND_gst_LevelRam.ui_CurrentBlockNumber) return;
	xbSND_gst_LevelRam.ui_CurrentBlockNumber--;
	if(!xbSND_gst_LevelRam.ui_CurrentBlockNumber) xbSND_Defrag();
}

/*
 =======================================================================================================================
    reset the allocated size
 =======================================================================================================================
 */
void xbSND_Defrag(void)
{
	xbSND_gst_LevelRam.ui_CurrentAllocatedSize = 0;
	xbSND_gst_LevelRam.ui_CurrentBlockNumber = 0;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* _XBOX */ 
 