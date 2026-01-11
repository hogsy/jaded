/*$T SPU.c GC! 1.097 01/18/02 15:30:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_TARGET

/*$T SPU.c GC! 1.097 01/18/02 15:30:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#include "BASe/CLIbrary/CLIstr.h"
#include <eekernel.h>

#include "BIGfiles/BIGdefs.h"

#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/ps2/SPU.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "IOP/ee/eeDebug.h"

/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    address and size
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 * 0x00000000 -> 0x000003FF Reserved £
 * 0x00000400 -> 0x00001FFF Out
 */
#define SPU_Cte_0OutVoice1Addr		(0x00000400 * 2)
#define SPU_Cte_0OutVoice1Size		(0x200 * 2)
#define SPU_Cte_0OutVoice3Addr		(0x00000600 * 2)
#define SPU_Cte_0OutVoice3Size		(0x200 * 2)
#define SPU_Cte_0Outspu1InLeftAddr	(0x00000800 * 2)
#define SPU_Cte_0Outspu1InLeftSize	(0x200 * 2)
#define SPU_Cte_0Outspu1InRightAddr (0x00000A00 * 2)
#define SPU_Cte_0Outspu1InRightSize (0x200 * 2)
#define SPU_Cte_1OutVoice1Addr		(0x00000C00 * 2)
#define SPU_Cte_1OutVoice1Size		(0x200 * 2)
#define SPU_Cte_1OutVoice3Addr		(0x00000E00 * 2)
#define SPU_Cte_1OutVoice3Size		(0x200 * 2)
#define SPU_Cte_0OutDryLeftAddr		(0x00001000 * 2)
#define SPU_Cte_0OutDryLeftSize		(0x200 * 2)
#define SPU_Cte_0OutDryRightAddr	(0x00001200 * 2)
#define SPU_Cte_0OutDryRightSize	(0x200 * 2)
#define SPU_Cte_0OutWetLeftAddr		(0x00001400 * 2)
#define SPU_Cte_0OutWetLeftSize		(0x200 * 2)
#define SPU_Cte_0OutWetRightAddr	(0x00001600 * 2)
#define SPU_Cte_0OutWetRightSize	(0x200 * 2)
#define SPU_Cte_1OutDryLeftAddr		(0x00001800 * 2)
#define SPU_Cte_1OutDryLeftSize		(0x200 * 2)
#define SPU_Cte_1OutDryRightAddr	(0x00001A00 * 2)
#define SPU_Cte_1OutDryRightSize	(0x200 * 2)
#define SPU_Cte_1OutWetLeftAddr		(0x00001C00 * 2)
#define SPU_Cte_1OutWetLeftSize		(0x200 * 2)
#define SPU_Cte_1OutWetRightAddr	(0x00001E00 * 2)
#define SPU_Cte_1OutWetRightSize	(0x200 * 2)

/* 0x00002000 -> 0x000027FF In For PCM stream */
#define SPU_Cte_0InLeftAddr		(0x00002000 * 2)
#define SPU_Cte_0InLeftSize		(0x0200 * 2)
#define SPU_Cte_0InRightAddr	(0x00002200 * 2)
#define SPU_Cte_0InRightSize	(0x0200 * 2)
#define SPU_Cte_1InLeftAddr		(0x00002400 * 2)
#define SPU_Cte_1InLeftSize		(0x0200 * 2)
#define SPU_Cte_1InRightAddr	(0x00002600 * 2)
#define SPU_Cte_1InRightSize	(0x0200 * 2)

/*
 * 0x00002800 -> 0x00002807 Reserved £
 * 0x00002808 -> 0x000FFFFF In for ADPCM Voices and FX
 */
#define SPU_Cte_VoicesMemAddr	(0x00002808 * 2)

/* #define SPU_Cte_VoicesMemSize ((0x100000 - 0x2808) * 2) */
#define SPU_Cte_VoicesMemSize	(0x001C7FBF - 0x00005010)

/*
 * fx area £
 * 0x001C7FBE -> 0x001DFFFF core 0 £
 * 0x001DFFFF -> 0x001FFFFF core 1
 */
#define SPU_Cte_MemFx0Addr		0x1FFFFF
#define SPU_Cte_MemFx1Addr		0x1DFFFF

#define SPU_Cte_MemStartAddr	0x00000000
#define SPU_Cte_MemSize			0x00200000

/* FX memory sizes */
#define iopSND_Cte_FXSize_RevOff	0x80
#define iopSND_Cte_FXSize_Room		0x26c0
#define iopSND_Cte_FXSize_StdSmall	0x1f40
#define iopSND_Cte_FXSize_StdMedium 0x4840
#define iopSND_Cte_FXSize_StdLarge	0x6fe0
#define iopSND_Cte_FXSize_Hall		0xade0
#define iopSND_Cte_FXSize_SpaceEcho 0xf6c0
#define iopSND_Cte_FXSize_Echo		0x18040
#define iopSND_Cte_FXSize_Delay		0x18040
#define iopSND_Cte_FXSize_PipeEcho	0x3c00

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    memory manager configuration
 -----------------------------------------------------------------------------------------------------------------------
 */

#define SPU_Cte_MemHoleNb		200
#define SPU_Cte_MemInitHoleRank 1
#define SPU_Cte_MemReservedHole (void *) 0x00FFFFFF
#define SPU_Cte_MemNoPrevHole	(tdst_HoleDescriptor *) - 1

/*
 =======================================================================================================================
    memory mamnager macros
 =======================================================================================================================
 */
#define M_MakeMark(__addr, __id)	(void *) ((unsigned int) __addr | (__id & 0x000000FF) << 24)
#define M_GetId(__a)				(unsigned int) (((unsigned int) __a & 0xFF000000) >> 24)

/*$4
 ***********************************************************************************************************************
    types
 ***********************************************************************************************************************
 */

typedef struct	tdst_HoleDescriptor_
{
	void						*pv_Addr;
	unsigned int				ui_Size;
	struct tdst_HoleDescriptor_ *p_NextHole;
	unsigned int				ui_AllocSize;
} tdst_HoleDescriptor;

/*$4
 ***********************************************************************************************************************
    private prototypes
 ***********************************************************************************************************************
 */

static void					SPU_s_InsertHole(tdst_HoleDescriptor *_p_Hole);
static tdst_HoleDescriptor	*SPU_spx_FindPrevHole(unsigned int _ui_Size);
static tdst_HoleDescriptor	*SPU_spx_CreateHole(void);
tdst_HoleDescriptor			*SPU_spx_NewHole(unsigned int _ui_Size);
tdst_HoleDescriptor			*SPU_spx_FindHole(unsigned int _ui_Size, tdst_HoleDescriptor **_pp_Prev);

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

static tdst_HoleDescriptor	SPU_sa_HoleArray[SPU_Cte_MemHoleNb];
static tdst_HoleDescriptor	*SPU_spx_FirstHole;
static char					*SPU_pc_EndOfMemory;
static char					*SPU_pc_EndOfAllocated;
static char					*SPU_pc_StreamingMemory;
static char					*SPU_pc_LoadingSoundMemory;

#ifndef _FINAL_
int							SPU_gi_AllocMaxSize = 0;
int							SPU_gi_AllocMaxClusterSize = 0;
int							SPU_gi_AllocSize = 0;
int							SPU_gi_AllocClusterSize = 0;
int							SPU_gi_StreamAllocSize = 0;
int							SPU_gi_LoadingSoundAllocSize = 0;
#endif

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SPU_i_InitModule(void)
{
	L_memset(SPU_sa_HoleArray, 0, SPU_Cte_MemHoleNb * sizeof(tdst_HoleDescriptor));
	SPU_spx_FirstHole = NULL;

	SPU_pc_StreamingMemory = (char *) SPU_Cte_VoicesMemAddr;
	SPU_pc_LoadingSoundMemory = SPU_pc_StreamingMemory + (SND_Cte_MaxBufferSize * SND_Cte_MaxSimultaneousStream);
	SPU_pc_EndOfAllocated = SPU_pc_LoadingSoundMemory + (SND_Cte_LoadingSoundMaxSize);
	SPU_pc_EndOfMemory = (char *) SPU_Cte_VoicesMemAddr + SPU_Cte_VoicesMemSize ;

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *SPU_pv_StreamAlloc(unsigned int _ui_Size)
{
	/*~~~~~~~~*/
	void	*pv;
	/*~~~~~~~~*/

	pv = SPU_pc_StreamingMemory;
	SPU_pc_StreamingMemory += _ui_Size;

#ifndef _FINAL_
	SPU_gi_StreamAllocSize += _ui_Size;
#endif
	return pv;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SPU_StreamFree(void *_pv)
{
#ifndef _FINAL_
	SPU_gi_StreamAllocSize=0;
#endif
}

void *SPU_pv_LoadingSoundAlloc(unsigned int _ui_Size)
{
	/*~~~~~~~~*/
	void	*pv;
	/*~~~~~~~~*/

	pv = SPU_pc_LoadingSoundMemory;
	SPU_pc_LoadingSoundMemory += _ui_Size;

#ifndef _FINAL_
	SPU_gi_LoadingSoundAllocSize += _ui_Size;
#endif
	return pv;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SPU_LoadingSoundFree(void *_pv)
{
#ifndef _FINAL_
	SPU_gi_LoadingSoundAllocSize=0;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *SPU_pv_Alloc(unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	tdst_HoleDescriptor *p_Hole, *p_Prev;
	unsigned int		ui_RSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ui_Size == 0)
	{
		eeDbg_M_Err(eeDbg_Err_002F);
		return NULL;
	}

	/* roud up to 64 oct, because of transfert SPU<->IOP is done in 64 oct unit */
	ui_RSize = (_ui_Size + 64) &~63;

	/* find one freed hole */
	p_Hole = SPU_spx_FindHole(ui_RSize, &p_Prev);

	/* no freed hole */
	if(p_Hole == NULL)
	{
		p_Hole = SPU_spx_NewHole(ui_RSize);
		if(p_Hole == NULL)
		{
			eeDbg_M_Err(eeDbg_Err_002E);
			return NULL;
		}
	}
	else if(p_Prev)
	{
		p_Prev->p_NextHole = p_Hole->p_NextHole;
	}
	else
	{
		SPU_spx_FirstHole = p_Hole->p_NextHole;
	}

#ifndef _FINAL_
	p_Hole->ui_AllocSize = _ui_Size;
	SPU_gi_AllocSize += _ui_Size;
	SPU_gi_AllocClusterSize += p_Hole->ui_Size;
	if(SPU_gi_AllocMaxClusterSize < SPU_gi_AllocClusterSize)
	{
	    SPU_gi_AllocMaxSize = SPU_gi_AllocSize;
	    SPU_gi_AllocMaxClusterSize = SPU_gi_AllocClusterSize;
	}
#endif

	return p_Hole->pv_Addr;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SPU_Free(void *_pv)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	tdst_HoleDescriptor *p_Next, *p_Freed, *p_Prev;
	unsigned int		ui_Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pv) return;

	/* get the corresponding deleted hole */
	ui_Id = M_GetId(_pv);
	p_Freed = &SPU_sa_HoleArray[ui_Id];

	/* check the hole */
	if(M_GetId(p_Freed->pv_Addr) == ui_Id)
	{
#ifndef _FINAL_
		SPU_gi_AllocSize -= p_Freed->ui_AllocSize;
		SPU_gi_AllocClusterSize -= p_Freed->ui_Size;
#endif
		p_Next = SPU_spx_FindHole(p_Freed->ui_Size, &p_Prev);

		if(p_Next && p_Prev)
		{
			p_Freed->p_NextHole = p_Next;
			p_Prev->p_NextHole = p_Freed;
		}
		else if(p_Next)
		{
			p_Freed->p_NextHole = SPU_spx_FirstHole;
			SPU_spx_FirstHole = p_Freed;
		}
		else if(p_Prev)
		{
			p_Freed->p_NextHole = NULL;
			p_Prev->p_NextHole = p_Freed;
		}
		else
		{
			p_Freed->p_NextHole = NULL;
			SPU_spx_FirstHole = p_Freed;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SPU_FreeAll(void)
{
	L_memset(SPU_sa_HoleArray, 0, SPU_Cte_MemHoleNb * sizeof(tdst_HoleDescriptor));
	SPU_spx_FirstHole = NULL;

	SPU_pc_EndOfAllocated = (char *) SPU_Cte_VoicesMemAddr +(SND_Cte_LoadingSoundMaxSize)+ (SND_Cte_MaxBufferSize * SND_Cte_MaxSimultaneousStream);
	SPU_pc_EndOfMemory = (char *) SPU_Cte_VoicesMemAddr + SPU_Cte_VoicesMemSize ;

#ifndef _FINAL_
    SPU_gi_AllocSize = 0;
	SPU_gi_AllocClusterSize = 0;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
tdst_HoleDescriptor *SPU_spx_NewHole(unsigned int _ui_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	tdst_HoleDescriptor *pst_Hole;
	unsigned int		ui_Id;
	unsigned int		ui_Addr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_ui_Size + 128) > (unsigned int) (SPU_pc_EndOfMemory - SPU_pc_EndOfAllocated)) return NULL;

	for(pst_Hole = &SPU_sa_HoleArray[0]; pst_Hole != &SPU_sa_HoleArray[SPU_Cte_MemHoleNb]; pst_Hole++)
	{
		if(pst_Hole->pv_Addr == NULL) break;
	}

	if(pst_Hole == &SPU_sa_HoleArray[SPU_Cte_MemHoleNb])
	{
		return NULL;
	}

	ui_Id = ((unsigned int) pst_Hole - (unsigned int) SPU_sa_HoleArray) / sizeof(tdst_HoleDescriptor);

	ui_Addr = (unsigned int) SPU_pc_EndOfAllocated &~127;
	ui_Addr += 128;

	pst_Hole->pv_Addr = M_MakeMark(ui_Addr, ui_Id);
	pst_Hole->ui_Size = _ui_Size;
	pst_Hole->p_NextHole = NULL;

	SPU_pc_EndOfAllocated = (char *) (ui_Addr + _ui_Size);

	return pst_Hole;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
tdst_HoleDescriptor *SPU_spx_FindHole(unsigned int _ui_Size, tdst_HoleDescriptor **_pp_Prev)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	tdst_HoleDescriptor *p_Hole;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	*_pp_Prev = NULL;
	p_Hole = SPU_spx_FirstHole;
	while(p_Hole)
	{
		if(p_Hole->ui_Size >= _ui_Size) break;
		*_pp_Prev = p_Hole;
		p_Hole = p_Hole->p_NextHole;
	}

	return p_Hole;
}

/*
 =======================================================================================================================
    alloc 64 aligned blocks
 =======================================================================================================================
 */
void *SPU_pv_FxAlloc(int core)
{
	if(core)
		return (void *) SPU_Cte_MemFx1Addr;
	else
		return (void *) SPU_Cte_MemFx0Addr;
}

#ifndef _FINAL_
int SPU_i_GetVoiceSize(void);
int SPU_i_GetFxSize(void);
int SPU_i_GetSysSize(void);
int SPU_i_GetFragmentSize(void);
int SPU_i_GetFreeSize(void);
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SPU_i_GetVoiceSize(void)
{
	return SPU_Cte_VoicesMemSize;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SPU_i_GetFxSize(void)
{
	return 2 * 0x18040;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SPU_i_GetSysSize(void)
{
	return SPU_Cte_VoicesMemAddr;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SPU_i_GetFragmentSize(void)
{
	return SPU_Cte_VoicesMemSize - ((int) SPU_pc_EndOfMemory - (int) SPU_pc_EndOfAllocated);
}

int SPU_i_GetFreeSize(void)
{
	return ((int) SPU_pc_EndOfMemory - (int) SPU_pc_EndOfAllocated) - SPU_gi_AllocMaxClusterSize;
}

#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
