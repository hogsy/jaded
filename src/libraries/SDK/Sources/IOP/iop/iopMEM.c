/*$T iopMEM.c GC 1.138 09/20/05 14:29:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifdef PSX2_IOP

/*$T iopMEM.c GC 1.138 09/20/05 14:29:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "iop/iopDebug.h"
#include "iop/iopMEM.h"
#include <stdio.h>
#include <kernel.h>
#include <string.h>
#include <libsd.h>
#include <sys/fcntl.h>
#include "iop/iopSND_libsd.h"
#include "iop/iopCLI.h"

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

#ifndef NULL
#define NULL	0
#endif

/*$4
 ***********************************************************************************************************************
    private variables
 ***********************************************************************************************************************
 */

extern int	iopMEM_Sema;

#ifndef _FINAL_
int			MEM_gi_AllocNb;
int			MEM_gi_AllocSize;
int			MEM_gi_AllocMaxSize;
int			MEM_gi_FreeSize;
int			MEM_gi_FreeSizeMin;
int			MEM_gi_RamSize;
#endif
//
//
#ifdef iopMEM_HISTORY
#define C_HistoSize		2000
#define C_FillRamSize	(5 * 1024 * 1024)
//
volatile int			iopMEM_gi_Id = 1;
volatile int			iopMEM_gi_Nb = 0;
volatile int			iopMEM_gai_Size[C_HistoSize];
volatile int			iopMEM_gai_Ptr[C_HistoSize];
volatile int			iopMEM_gai_Id[C_HistoSize];
volatile int			iopMEM_gai_Line[C_HistoSize];
volatile char		iopMEM_gai_File[C_HistoSize][512];
#else
#define C_FillRamSize	(6 * 1024 * 1024)
#endif

/*$4
 ***********************************************************************************************************************
    public functions
 ***********************************************************************************************************************
 */

void	*MEM_p_dbg_6Mo;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int iopMEM_i_InitModule(void)
{
#ifndef _FINAL_
	MEM_gi_AllocNb = 0;
	MEM_gi_AllocSize = 0;
	MEM_gi_AllocMaxSize = 0;

	MEM_gi_FreeSize = QueryTotalFreeMemSize();

	if(MEM_gi_FreeSize > iopMEM_Cte_RAMSize)
	{
		MEM_p_dbg_6Mo = (void *) (AllocSysMemory(0, C_FillRamSize, NULL));
		L_memset(MEM_p_dbg_6Mo, -1, C_FillRamSize);
		MEM_gi_FreeSize = QueryTotalFreeMemSize();
	}

	if(MEM_gi_FreeSize > iopMEM_Cte_RAMSize)
	{
		MEM_gi_RamSize = iopMEM_Cte_KitRAMSize;
		MEM_gi_FreeSize = MEM_gi_FreeSize - (iopMEM_Cte_KitRAMSize - iopMEM_Cte_RAMSize);
	}
	else
	{
		MEM_gi_RamSize = iopMEM_Cte_RAMSize;
	}

	MEM_gi_FreeSizeMin = MEM_gi_FreeSize;
#endif
	CLI_SignalSema(iopMEM_Sema);
	return 0;
}

/*$off*/
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef iopMEM_HISTORY
void *_iopMEM_pv_iopAlloc(unsigned int _ui_Size, char *file, int line)
#else
void *_iopMEM_pv_iopAlloc(unsigned int _ui_Size)
#endif
/*$on*/
{
	void	*p;

	CLI_WaitSema(iopMEM_Sema);
	p = (void *) (AllocSysMemory(0, _ui_Size, NULL));

#ifdef iopMEM_HISTORY
	if(p && (iopMEM_gi_Nb < C_HistoSize))
	{
	    iopMEM_gai_Size[iopMEM_gi_Nb] = _ui_Size;
    	iopMEM_gai_Ptr[iopMEM_gi_Nb] = (int) p;
    	iopMEM_gai_Id[iopMEM_gi_Nb] = iopMEM_gi_Id++;
    	iopMEM_gai_Line[iopMEM_gi_Nb] = line;
    	sprintf((char*)iopMEM_gai_File[iopMEM_gi_Nb], "%s", file);
    	iopMEM_gi_Nb++;
	}
#endif

	CLI_SignalSema(iopMEM_Sema);


#if defined(_DEBUG) && defined(iopMEM_HISTORY)
	iopDbg_M_AssertX((p), iopDbg_Err_004F "-2 from %s(%d)", file, line);
#else
   	iopDbg_M_Assert((p), iopDbg_Err_004F "-2 from");
#endif


#ifndef _FINAL_
	if(p)
	{
		MEM_gi_AllocNb++;
		MEM_gi_AllocSize += _ui_Size;
		if(MEM_gi_AllocMaxSize < MEM_gi_AllocSize)
		{
			MEM_gi_AllocMaxSize = MEM_gi_AllocSize;
		}
		MEM_gi_FreeSize -= _ui_Size;
		if(MEM_gi_FreeSizeMin > MEM_gi_FreeSize)
		{
			MEM_gi_FreeSizeMin = MEM_gi_FreeSize;
		}
	}
#endif
	return p;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopMEM_iopFree(void *_pv)
{
#ifdef _FINAL_
	if(!_pv) return;

	CLI_WaitSema(iopMEM_Sema);
	FreeSysMemory(_pv);
	CLI_SignalSema(iopMEM_Sema);
#else
	/*~~~~~~~*/
	int before;
	/*~~~~~~~*/

	if(!_pv) return;
	CLI_WaitSema(iopMEM_Sema);

#ifdef iopMEM_HISTORY
    {
        int i;
	    for(i=0; i<iopMEM_gi_Nb; i++)
	    {
	        if(iopMEM_gai_Ptr[i] != (int)_pv) continue;
	        
	        iopMEM_gi_Nb--;
	        
	        iopMEM_gai_Size[i] = iopMEM_gai_Size[iopMEM_gi_Nb] ;
    	    iopMEM_gai_Ptr[i] = iopMEM_gai_Ptr[iopMEM_gi_Nb] ;
    	    iopMEM_gai_Id[i] = iopMEM_gai_Id[iopMEM_gi_Nb];
    	    iopMEM_gai_Line[i] = iopMEM_gai_Line[iopMEM_gi_Nb];
    	    sprintf((char*)iopMEM_gai_File[i], "%s", iopMEM_gai_File[iopMEM_gi_Nb]);


	        
	        iopMEM_gai_Size[iopMEM_gi_Nb]=0 ;
    	    iopMEM_gai_Ptr[iopMEM_gi_Nb] =0;
    	    iopMEM_gai_Id[iopMEM_gi_Nb]=0;
    	    iopMEM_gai_Line[iopMEM_gi_Nb] =0;
    	    iopMEM_gai_File[iopMEM_gi_Nb][0] = 0;
    	    break;
	    }
	}
#endif

	before = QueryTotalFreeMemSize();
	FreeSysMemory(_pv);
	before = (QueryTotalFreeMemSize() - before);
	MEM_gi_AllocSize -= before;
	MEM_gi_AllocNb--;
	MEM_gi_FreeSize += before;
	CLI_SignalSema(iopMEM_Sema);
#endif
}

#ifdef iopMEM_HISTORY
void iopMEM_Histo(void)
{
    int i;
    for(i=0; i<iopMEM_gi_Nb; i++)    
    {
        printf("[IOP][MEM][HISTO] %s(%d) : %d\n",iopMEM_gai_File[i], iopMEM_gai_Line[i],iopMEM_gai_Size[i]);
    }
    
}
#endif

/*$off*/
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef iopMEM_HISTORY
void* _iopMEM_pv_iopAllocAlign(unsigned int _ui_BlockSize, unsigned int _ui_Alignment, char *file, int line)
#else
void* _iopMEM_pv_iopAllocAlign(unsigned int _ui_BlockSize, unsigned int _ui_Alignment)
#endif
/*$on*/
{
	unsigned int	Allocated, RealPtr;

#ifdef iopMEM_HISTORY
	RealPtr = (unsigned int) _iopMEM_pv_iopAlloc(_ui_BlockSize + (2 * _ui_Alignment), file, line);
#else
	RealPtr = (unsigned int) _iopMEM_pv_iopAlloc(_ui_BlockSize + (2 * _ui_Alignment));
#endif
	if(RealPtr == 0)
	{
		return NULL;
	}
	Allocated = RealPtr &~(_ui_Alignment - 1);
	Allocated += _ui_Alignment;
	Allocated += _ui_Alignment;
	*((unsigned int *) Allocated - 1) = RealPtr;

	return (void *) Allocated;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopMEM_iopFreeAlign(void *_pv)
{
	if(!_pv) return;

	iopMEM_iopFree((void *) *((unsigned int *) _pv - 1));
}

/*
 =======================================================================================================================
    libsd must be initialized and DMA handler/callback must not be changed !!
 =======================================================================================================================
 */
int iopMEM_i_spuResetMemory(void)
{
	/*~~~~~~~*/
	char	*p;
	int		i;
	/*~~~~~~~*/

#define Cte_spuResetSize	(64 * 2 * 1024)
#define Cte_spuMemStartAddr 0x00000000
#define Cte_spuMemSize		0x00200000
	/* reset SPU Voices memory */
	p = iopMEM_pv_iopAllocAlign(Cte_spuResetSize, 64);
	if(p == NULL)
	{
		iopDbg_M_Err(iopDbg_Err_0018);
		return -1;
	}

	L_memset(p, 0, Cte_spuResetSize);

	for(i = 0; (i + Cte_spuResetSize) < Cte_spuMemSize; i += Cte_spuResetSize)
	{
		L_sceSdVoiceTrans
		(
			0,
			SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA,
			(u_char *) p,
			(u_int) (Cte_spuMemStartAddr + i),
			(u_int) Cte_spuResetSize
		);
		L_sceSdVoiceTransStatus(0, SD_TRANS_STATUS_WAIT);
	}

	L_sceSdVoiceTrans
	(
		0,
		SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA,
		(u_char *) p,
		(u_int) (Cte_spuMemStartAddr + i),
		(u_int) Cte_spuMemSize - (u_int) i
	);
	L_sceSdVoiceTransStatus(0, SD_TRANS_STATUS_WAIT);

#define _ADPCM_MARK_START	0x04
#define _ADPCM_MARK_LOOP	0x02
#define _ADPCM_MARK_END		0x01
	* (p + 1) = _ADPCM_MARK_START | _ADPCM_MARK_LOOP | _ADPCM_MARK_END;
	L_sceSdVoiceTrans(0, SD_TRANS_MODE_WRITE | SD_TRANS_BY_DMA, (u_char *) p, (u_int) 0x5000, (u_int) 16);
	L_sceSdVoiceTransStatus(0, SD_TRANS_STATUS_WAIT);

	iopMEM_iopFreeAlign(p);

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *iopMEM_pv_spuFxAlloc(int core)
{
#define Cte_spuMemFx0Addr	0x1FFFFF
#define Cte_spuMemFx1Addr	0x1DFFFF
	if(core)
		return (void *) Cte_spuMemFx1Addr;
	else
		return (void *) Cte_spuMemFx0Addr;
}

#ifndef _FINAL_

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void iopMEM_Stat(void)
{
    printf("[IOP][MEM] current state : free %d oct, max bloc %d oct\n", QueryTotalFreeMemSize(), QueryMaxFreeMemSize());
}
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
