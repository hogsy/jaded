//------------------------------------------------------------------------
//
// (C) Copyright 2000 Ubi Soft
//
//  Author        : Dany Joannette
//  Date          : 10 April 2001
//
// Package       BASE
// File          arammng.cpp
// Description   ARAM manager for dolphin
//
//------------------------------------------------------------------------

#include "Precomp.h"

#ifndef GC2RVL_NO_ARAM

#include "BASe/BASsys.h"
#include "GameCube/GC_aramheap.h"
#include "GameCube/GC_arammng.h"

#define ARAM_NUMBER_OF_TASKS 200
static ARQRequest  	g_stTask[ARAM_NUMBER_OF_TASKS] ATTRIBUTE_ALIGN(32);
static BOOL 		g_bTaskUsed[ARAM_NUMBER_OF_TASKS];
static u8 	  		g_u8TaskCounter;

unsigned int	ARAM_TotalAlloc_Level = 0;
unsigned int	ARAM_TotalAlloc_Fix = 0;

ARAM_DMA_FIFO	*ARAM_gpst_DMA_FIFO; 


//-------------------------------------------------------------------------------------------------------------------------
// Definitions of the number and size of ARAM blocks

#ifdef ANIMS_USE_ARAM
#define EVE_C_ARAM_NumCacheLine				2
#define EVE_C_ARAM_NumEventsPerCacheLine	2
#endif


u32 g_u32ARAM_Blocks[eARAM_NumberOfARAMBlocks]; 			// used by ARInit 


#ifdef ANIMS_USE_ARAM
u32 g_u32ARAM_BlocksSize[eARAM_NumberOfARAMBlocks] = {	// size to use for ARAlloc (creation of blocks)
	(6*1024-16)*1024, // for virtual memory management (16 ko are used by system).
 	5*1024*1024, // for audio
 	5*1024*1024  // for game. Must be equal to value in u32GameARAMSize.
 	};
#else // ANIMS_USE_ARAM
u32 g_u32ARAM_BlocksSize[eARAM_NumberOfARAMBlocks] = {	// size to use for ARAlloc (creation of blocks)
	(11*1024-16)*1024, // for virtual memory management (16 ko are used by system).
 	5*1024*1024 // for audio
 	};
#endif // ANIMS_USE_ARAM
 	
tdeARAMHeapType g_ARAMDefaultHeaps[eARAM_NumberOfARAMBlocks];

u8 				g_nbARAMHeaps[eARAM_NumberOfARAMBlocks];
tdstARAMHeap    *g_pARAMHeaps[eARAM_NumberOfARAMBlocks];	
u32				g_ARAMAddrStart;
u32				g_CurrentARAMAddr;

//-------------------------------------------------------------------------------------------------------------------------
// prototypes
void fn_vARAMManagementAfterTransfer(ARQRequest *task);

//-------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------

#define M_Lock() \
	do \
	{ \
		ARAM_gb_Lock = OSDisableInterrupts(); \
	} while(0);

#define M_Unlock() \
	do \
	{ \
		OSRestoreInterrupts(ARAM_gb_Lock); \
	} while(0);

BOOL				ARAM_gb_Lock;

#ifdef ANIMS_USE_ARAM
ARAM_tdst_Cache 	ARAM_Cache;
ARAM_tdst_Section	*ARAM_Cache_Anim;


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
void ARAM_Cache_Init()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
	ARAM_tdst_Cluster	*pst_CL, *pst_LastCL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
	
	ARAM_Cache.pst_Anim = MEM_p_Alloc(sizeof(ARAM_tdst_Section));
	
	/*$F ---------  ANIM CACHE ----------- */
	
	ARAM_Cache_Anim = ARAM_Cache.pst_Anim;
	
	ARAM_Cache_Anim->pst_Clusters = MEM_p_Alloc(ARAM_Anim_NumberOfClusters * sizeof(ARAM_tdst_Cluster));
	ARAM_Cache_Anim->ul_NumCL = ARAM_Anim_NumberOfClusters;	
	ARAM_Cache_Anim->ul_NextCL = 0;
			
	pst_CL = ARAM_Cache_Anim->pst_Clusters;
	pst_LastCL = pst_CL + ARAM_Anim_NumberOfClusters;
	for(; pst_CL < pst_LastCL; pst_CL ++)
	{
		pst_CL->pc_Cache = (char *)MEM_p_AllocAlign(32, 32);
		pst_CL->ul_Start = 0xFFFFFFFF;
		pst_CL->ul_CheckSum = 0;
		pst_CL->ul_Time = 0;
		pst_CL->ul_Valid = 1;		
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */
void ARAM_Cache_Resfresh(ARAM_tdst_Section *_pst_Section)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
	ARAM_tdst_Cluster 	*pst_CL, *pst_LastCL;	
	ULONG				ul_MaxTime;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
	ul_MaxTime = 0;
	
	pst_CL = _pst_Section->pst_Clusters;
	pst_LastCL = pst_CL + _pst_Section->ul_NumCL;	
	for(;pst_CL < pst_LastCL; pst_CL ++)
	{
		pst_CL->ul_Time ++;	
		
		if(pst_CL->ul_Time > ul_MaxTime)
		{
			ul_MaxTime = pst_CL->ul_Time;
			_pst_Section->ul_NextCL = pst_CL - _pst_Section->pst_Clusters;
		}
	}
}
#endif

//-------------------------------------------------------------------------------------------------------------------------
// Initialize ARAM with the requested number of blocks and allocate the necessary space for each block
//-------------------------------------------------------------------------------------------------------------------------
void ARAM_Init()
{
	u32 i;
	ARInit(g_u32ARAM_Blocks, eARAM_NumberOfARAMBlocks);
	ARQInit();
	
	g_ARAMAddrStart = ARAlloc(g_u32ARAM_BlocksSize[0]); // first block determines the start of the workable ARAM address range
	g_CurrentARAMAddr = g_ARAMAddrStart;
	
	for(i=1; i<eARAM_NumberOfARAMBlocks; i++)
	{
		ARAlloc(g_u32ARAM_BlocksSize[i]);
	}

	for(i = 0; i < ARAM_NUMBER_OF_TASKS;i++)
	{
		g_bTaskUsed[i] = FALSE;
	}

	for(i=0; i<eARAM_NumberOfARAMBlocks; i++)
	{
		g_nbARAMHeaps[i]=0; 
		g_pARAMHeaps[i]=NULL; 
	}
		
	g_u8TaskCounter = 0;
	
#ifdef ANIMS_USE_ARAM
	ARAM_Cache_Init();
#endif	
}

//-------------------------------------------------------------------------------------------------------------------------
// Create _nbHeaps of size _sizeOfHeaps using the specified block of ARAM
//-------------------------------------------------------------------------------------------------------------------------
BOOL ARAM_CreateHeaps(u8 _block, u8 _nbHeaps, u32*_sizeOfHeaps)
{
	u32 i;
	
	ARAM_DestroyHeaps(_block);
	
	g_nbARAMHeaps[_block] = _nbHeaps;
	g_pARAMHeaps[_block] = (tdstARAMHeap*)MEM_p_Alloc( sizeof(tdstARAMHeap)*g_nbARAMHeaps[_block]);
	
#ifndef _FINAL_
	OSReport("Block %d (%x) has %d heaps\n",_block,(ULONG)(g_pARAMHeaps[_block]),_nbHeaps);
#endif // _FINAL_	
	
	for(i=0; i<g_nbARAMHeaps[_block]; i++)
	{
		ARAM_Heap_Init(&g_pARAMHeaps[_block][i], g_CurrentARAMAddr, _sizeOfHeaps[i]);
		g_CurrentARAMAddr += _sizeOfHeaps[i];
#ifndef _FINAL_
		OSReport("Heap %d (%x) has %d bytes\n",i,(ULONG)(&g_pARAMHeaps[_block][i]),_sizeOfHeaps[i]);
#endif // _FINAL_
	}
	
	return TRUE;
}

void ARAM_ReserveBlock(u8 _block, u32 _sizeofBlock)
{
	g_nbARAMHeaps[_block] = 0;
	g_pARAMHeaps[_block] = NULL;
	g_CurrentARAMAddr += _sizeofBlock;
}

//-------------------------------------------------------------------------------------------------------------------------
// Destroy all heaps from specified block of ARAM
//-------------------------------------------------------------------------------------------------------------------------
BOOL ARAM_DestroyHeaps(u8 _block)
{
	if(g_pARAMHeaps[_block])
	{
		u32 i;
		// free all heaps created for this block
		for(i=0; i<g_nbARAMHeaps[_block]; i++)
		{
			ARAM_FreeEntireHeap(_block, i);
		}
		
		MEM_Free(g_pARAMHeaps[_block]);
	}
	
	g_pARAMHeaps[_block] = NULL;
	g_nbARAMHeaps[_block] = 0;

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------
// Allocate some ARAM from specified block and heap
//-------------------------------------------------------------------------------------------------------------------------
u32	ARAM_Alloc(u8 _block, u8 _heap, u32 _size, u8 _Type)
{
    u32 uu;
    
    M_Lock();
	uu=ARAM_Heap_Alloc(&g_pARAMHeaps[_block][_heap], _size, _Type);
	M_Unlock();
    
	return uu;
}

//-------------------------------------------------------------------------------------------------------------------------
// Free an allocated block of ARAM
//-------------------------------------------------------------------------------------------------------------------------
void ARAM_Free(u8 _block, u8 _heap, u32 _addr)
{
	M_Lock();
	ARAM_Heap_DeAlloc(&g_pARAMHeaps[_block][_heap], _addr);
    M_Unlock();
}

//-------------------------------------------------------------------------------------------------------------------------
// Free the specified heap from the specified block
// Is is then as clean as when it was newly created
//-------------------------------------------------------------------------------------------------------------------------
void ARAM_FreeEntireHeap(u32 _block, u8 _heap)
{
	ARAM_Heap_Free(&g_pARAMHeaps[_block][_heap]);
}

/*---------------------------------------------------------------------------*
/  CALLBACK for DMA transfer
/*---------------------------------------------------------------------------*/
void fn_vARAMManagementAfterTransfer(ARQRequest *task)
{
    u32 u32TaskIndex = task - (ARQRequest *)&g_stTask;

	g_bTaskUsed[u32TaskIndex] = FALSE;
}

/*---------------------------------------------------------------------------*/

static void TransactionCompletedCallback(u32 task)
{
    ARQRequest *p = (ARQRequest*)task;
    BOOL *bTransferCompleted = (BOOL*)p->owner;

    if(bTransferCompleted) *bTransferCompleted = TRUE;

    fn_vARAMManagementAfterTransfer(p);        
}


/*---------------------------------------------------------------------------*/

void ARAM_DMA_RequestManagerCallback(u32 task)
{
    ARQRequest 			*p = (ARQRequest*)task;
    ARAM_DMA_Request	*Request;
    u32					c;    
	BOOL				old;
	
	old = OSDisableInterrupts();	
            
    ARAM_gpst_DMA_FIFO->NumberOfRequests --;
  
    if((int) ARAM_gpst_DMA_FIFO->NumberOfRequests < 0)
    {
	    ARAM_gpst_DMA_FIFO->NumberOfRequests = 0;    
#ifdef _DEBUG    	    
    	OSReport("DMA Request Manager\n");
#endif        	
    }        
    
    if(ARAM_gpst_DMA_FIFO->NumberOfRequests > 0)
    {        
    	c = (ARAM_gpst_DMA_FIFO->CurrentRequest + 1) % ARAM_DMA_MaxNumberOfRequests;
    	
    	ARAM_gpst_DMA_FIFO->CurrentRequest = c;
    	
    	Request = &ARAM_gpst_DMA_FIFO->apst_Request[c];    
    		
    	if(Request->Type == ARQ_TYPE_ARAM_TO_MRAM)
	    	ARAM_GetFromARAM((u32) Request->Ram, (u32) Request->Aram, (u32) Request->Size, p->owner, &ARAM_DMA_RequestManagerCallback);
    	else
	    	ARAM_SendToARAM((u32) Request->Ram, (u32) Request->Aram, (u32) Request->Size, p->owner, &ARAM_DMA_RequestManagerCallback);
    }
    else
		ARAM_gpst_DMA_FIFO->Locked = FALSE;
    
    fn_vARAMManagementAfterTransfer(p);
    
    OSRestoreInterrupts(old);        
}

/*---------------------------------------------------------------------------*/
#ifdef ANIMS_USE_ARAM

void ARAM_DMA_PreloadCallback(u32 task)
{
    ARQRequest 			*p = (ARQRequest*)task;
    ARAM_tdst_Cluster	*pst_CL;            	
        
        
    pst_CL = (ARAM_tdst_Cluster	*)p->owner;
    pst_CL->ul_Valid = 1;
    
    fn_vARAMManagementAfterTransfer(p);        
    
}
#endif



//-------------------------------------------------------------------------------------------------------------------------
// Send something to ARAM
// _pTransferCompleted may be used as a pointer to a bool value. It will be assigned true when the transaction is completed.
// _pTransferCompleted may also be a callback function if _bTransferCompletedIsACallback is set to true
// _pTransferCompleted may be NULL
//-------------------------------------------------------------------------------------------------------------------------
void ARAM_SendToARAM(u32 _ramAddr, u32 _aramAddr, u32 _length, u32 _userData, ARQCallback _Callback)
{
	BOOL		old;
	
	old = OSDisableInterrupts();	
	
	if((_ramAddr % 32) || (_length % 32))
	{
		OSHalt("All Transfer be aligned and multiple of 32 Bytes");
	}
	
	
	// dump cached data for the range before post request
	DCStoreRange((void*)_ramAddr, _length);
	
    g_u8TaskCounter = (g_u8TaskCounter + 1) % ARAM_NUMBER_OF_TASKS;
    	
	if(g_bTaskUsed[g_u8TaskCounter])
	{
		OSHalt("Too many tasks in ARAM Queue. Increase NUMBER_OF_TASKS in arammng.cpp");
	}
	else
	{
		g_bTaskUsed[g_u8TaskCounter] = TRUE;
	}

	ARQPostRequest(
        &g_stTask[g_u8TaskCounter],
        _userData,
        ARQ_TYPE_MRAM_TO_ARAM,
        ARQ_PRIORITY_HIGH,
        (u32)_ramAddr,
        (u32)_aramAddr,
        OSRoundUp32B(_length),
        (_Callback ? _Callback : &TransactionCompletedCallback)
        );
        
	OSRestoreInterrupts(old);     
	   
}

//-------------------------------------------------------------------------------------------------------------------------
// Get something from ARAM
// _pTransferCompleted may be used as a pointer to a bool value. It will be assigned true when the transaction is completed.
// _pTransferCompleted may also be a callback function if _bTransferCompletedIsACallback is set to true
// _pTransferCompleted may be NULL
//-------------------------------------------------------------------------------------------------------------------------
void ARAM_GetFromARAM(u32 _ramAddr, u32 _aramAddr, u32 _length, u32 _userData, ARQCallback _Callback)
{
	BOOL		old;
		
	old = OSDisableInterrupts();	

	if((_ramAddr % 32) || (_length % 32))
	{
		OSHalt("All Transfer be aligned and multiple of 32 Bytes");
	}
	
#ifdef _DEBUG	
	if((ULONG)_aramAddr > 0x01000000)
		OSReport("ARAM Loading Error\n");
#endif	

	DCInvalidateRange((void*)_ramAddr, _length);
	
    g_u8TaskCounter = (g_u8TaskCounter + 1) % ARAM_NUMBER_OF_TASKS;	

	if(g_bTaskUsed[g_u8TaskCounter])
	{
		OSHalt("Too many tasks in ARAM Queue. Increase NUMBER_OF_TASKS in arammng.cpp");
	}
	else
	{
		g_bTaskUsed[g_u8TaskCounter] = TRUE;
	}

	ARQPostRequest(
        &g_stTask[g_u8TaskCounter],
        _userData,
        ARQ_TYPE_ARAM_TO_MRAM,
        ARQ_PRIORITY_HIGH,
        (u32)_aramAddr,
        (u32)_ramAddr,
        _length,
        (_Callback ? _Callback : &TransactionCompletedCallback)
        );
        
	OSRestoreInterrupts(old);    
		            
}

/*$F
-------------------------------------------------------------------------------------------------------------------------
	Memmove into ARAM
-------------------------------------------------------------------------------------------------------------------------
*/
void ARAM_memmove(u32 _pc_AramDest, u32 _pc_AramSrc, u32 _ul_Length, u32 _pc_RamBuffer)
{
	char 				*pc_TempBuffer;
	BOOL	volatile	b_TransferFinised;
	
	if((_pc_AramDest % 32) || (_pc_AramSrc % 32) || (_ul_Length % 32))
		OSHalt("ARAM_memmove argument not aligned to 32 Bytes");
	
	if(!_pc_RamBuffer)
		pc_TempBuffer = MEM_p_AllocAlign(_ul_Length, 32);
	else
		pc_TempBuffer = (char *)_pc_RamBuffer;
	
	b_TransferFinised = FALSE;
	ARAM_GetFromARAM((u32) pc_TempBuffer, _pc_AramSrc, _ul_Length, (u32)&b_TransferFinised, NULL);
	while(!b_TransferFinised){};
	
	b_TransferFinised = FALSE;		
	ARAM_SendToARAM((u32) pc_TempBuffer, _pc_AramDest, _ul_Length,  (u32)&b_TransferFinised, NULL);
	while(!b_TransferFinised){};
	
	if(!_pc_RamBuffer)	MEM_FreeAlign(pc_TempBuffer);
	
}

/*---------------------------------------------------------------------------*/

void ARAM_DMA_AddRequest(u32 Ram, u32 Aram, u32 Size, u32 Type, u32 Owner)
{
	u32 c;
	
	if(ARAM_gpst_DMA_FIFO->NumberOfRequests == ARAM_DMA_MaxNumberOfRequests - 1)
		OSHalt("Soft Max Number of streaming ARAM Requests reached. Increase it to avoid this problem");
		
		
	c = (ARAM_gpst_DMA_FIFO->CurrentRequest + ARAM_gpst_DMA_FIFO->NumberOfRequests) % ARAM_DMA_MaxNumberOfRequests;
	
	ARAM_gpst_DMA_FIFO->apst_Request[c].Ram = Ram;
	ARAM_gpst_DMA_FIFO->apst_Request[c].Aram = Aram;
	ARAM_gpst_DMA_FIFO->apst_Request[c].Size = Size;
	ARAM_gpst_DMA_FIFO->apst_Request[c].Type = Type;
	ARAM_gpst_DMA_FIFO->apst_Request[c].Owner = Owner;	
	ARAM_gpst_DMA_FIFO->NumberOfRequests ++;		
	
}

/*---------------------------------------------------------------------------*/

void ARAM_DMA_RequestManager()
{
    ARAM_DMA_Request	*Request;
	BOOL				old;
	
	
	old = OSDisableInterrupts();	
                
	if((ARAM_gpst_DMA_FIFO->NumberOfRequests > 0) && !(ARAM_gpst_DMA_FIFO->Locked))
	{
    	Request = &ARAM_gpst_DMA_FIFO->apst_Request[ARAM_gpst_DMA_FIFO->CurrentRequest];
    	
    	if(Request->Type == ARQ_TYPE_ARAM_TO_MRAM)
	    	ARAM_GetFromARAM((u32) Request->Ram, (u32) Request->Aram, (u32) Request->Size, Request->Owner, &ARAM_DMA_RequestManagerCallback);
    	else
	    	ARAM_SendToARAM((u32) Request->Ram, (u32) Request->Aram, (u32) Request->Size, Request->Owner, &ARAM_DMA_RequestManagerCallback);    		
	    	
		ARAM_gpst_DMA_FIFO->Locked = TRUE;	    	
	}
	
	OSRestoreInterrupts(old);	
	
}
#else // GC2RVL_NO_ARAM
void    ARQPostRequest          (ARQRequest* task,
                                 u32         owner,
                                 u32         type,
                                 u32         priority,
                                 u32         source,
                                 u32         dest,
                                 u32         length,
                                 ARQCallback callback
                                )
{
	ARQRequest ltask;
	switch(type)
	{
	case ARQ_TYPE_MRAM_TO_ARAM:
	case ARQ_TYPE_ARAM_TO_MRAM:
		L_memcpy( (void*) source, (void*) dest, length);
		break;
	}
	
	
	if(callback)
	{
		//ltask = {NULL,owner,type,priority,source,dest,length,callback};	
		ltask.next = NULL;
		ltask.owner = owner;		
		ltask.type = type;
		ltask.priority = priority;
		ltask.source = source;
		ltask.dest = dest;
		ltask.length = length;
		ltask.callback = callback;		
		callback((u32)&ltask);
	}
}

void    ARQRemoveOwnerRequest   (u32 owner)
{
}
#endif // GC2RVL_NO_ARAM