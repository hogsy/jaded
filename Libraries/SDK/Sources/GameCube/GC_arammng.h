//------------------------------------------------------------------------
//
// (C) Copyright 2000 Ubi Soft
//
//  Author        : Dany Joannette
//  Date          : 10 April 2001
//
// Package       BASE
// File          aramheap.cpp
// Description   ARAM heap dolphin
//
//------------------------------------------------------------------------

#ifndef __GC_ARAMMNG_H__
#define __GC_ARAMMNG_H__


#include "BASe/MEMory/MEM.h"

#ifdef ANIMS_USE_ARAM

#define ARAM_Anim_NumberOfClusters 		10
#define ARAM_Photo_NumberOfClusters 	1



typedef struct ARAM_tdst_Cluster_
{
	char 						*pc_Cache;	
	u32							ul_Start;
	u32							ul_Time;
	u32							ul_CheckSum;
	volatile u32				ul_Valid;
} ARAM_tdst_Cluster;

typedef struct ARAM_tdst_Section_
{
	ARAM_tdst_Cluster 	*pst_Clusters;
	u32					ul_NumCL;
	u32					ul_NextCL;	/* Next Cluster to load in */
	
} ARAM_tdst_Section;

typedef struct ARAM_tdst_Cache_
{
	ARAM_tdst_Section 	*pst_Anim;
	ARAM_tdst_Section 	*pst_Photo;		
	
} ARAM_tdst_Cache;

#endif


#define ARAM_DMA_MaxNumberOfRequests		100

typedef struct ARAM_DMA_Request_
{
	u32 	Ram;
	u32 	Aram;
	u32		Size;
	u32		Type;
	u32		Owner;
} ARAM_DMA_Request;


typedef struct ARAM_DMA_FIFO_
{
	ARAM_DMA_Request 	apst_Request[ARAM_DMA_MaxNumberOfRequests];
	u32					CurrentRequest;
	u32					NumberOfRequests;
	BOOL	volatile	Locked;	
} ARAM_DMA_FIFO;


ARAM_DMA_FIFO	*ARAM_gpst_DMA_FIFO;


//-------------------------------------------------------------------------------------------------------------------------
// Definitions of the number and size of ARAM blocks

typedef enum
{
    eARAM_VirtualRAM        =   0,
    eARAM_Audio             =   1,
#ifdef ANIMS_USE_ARAM
    eARAM_Game              =   2,
#endif // ANIMS_USE_ARAM
    eARAM_NumberOfARAMBlocks,    
    eARAM_Dummy             =   0xFFFFFFFF // force the 32 bit alignment
} tdeARAMBlockID;

typedef enum
{
    eARAM_Fix                   =   0,
    eARAM_Level                 =   1,
    eARAM_NumberOfARAMHeapTypes =   2,    
    eARAM_HeapTypeDummy         =   0xFFFFFFFF // force the 32 bit alignment
} tdeARAMHeapType;

extern u32 g_u32ARAM_Blocks[eARAM_NumberOfARAMBlocks];
extern u32 g_u32ARAM_BlocksSize[eARAM_NumberOfARAMBlocks];
extern unsigned int	ARAM_TotalAlloc_Level ;
extern unsigned int	ARAM_TotalAlloc_Fix ;

extern tdeARAMHeapType g_ARAMDefaultHeaps[eARAM_NumberOfARAMBlocks];
#ifdef ANIMS_USE_ARAM
extern ARAM_tdst_Cache 	ARAM_Cache;
extern ARAM_tdst_Section	*ARAM_Cache_Anim;
#endif

//-------------------------------------------------------------------------------------------------------------------------
#ifndef GC2RVL_NO_ARAM
void			ARAM_Cache_Init();
void 			ARAM_Init();
BOOL 			ARAM_CreateHeaps(u8 _block, u8 _nbHeaps, u32 *_sizeOfHeaps);
BOOL 			ARAM_DestroyHeaps(u8 _block);
u32 			ARAM_Alloc(u8 _block, u8 _heap, u32 _size, u8 _Type);
void 			ARAM_Free(u8 _block, u8 _heap, u32 _addr);
void 			ARAM_FreeEntireHeap(u32 _block, u8 _heap);
void 			ARAM_SendToARAM(u32 _ramAddr, u32 _aramAddr, u32 _length, u32 _userData, ARQCallback _Callback);
void 			ARAM_GetFromARAM(u32 _ramAddr, u32 _aramAddr, u32 _length, u32 _userData, ARQCallback _Callback);
#ifdef ANIMS_USE_ARAM
void			ARAM_Cache_Resfresh(ARAM_tdst_Section * _section);
#endif

void 			ARAM_memmove(u32 _pc_AramDest, u32  _pc_AramSrc, u32 _ul_Length, u32 _pc_RamBuffer);
void			ARAM_DMA_RequestManagerCallback(u32 task);
void 			ARAM_DMA_PreloadCallback(u32 task);
void			ARAM_DMA_AddRequest(u32 Ram, u32 Aram, u32 Size, u32 Type, u32 Owner);
void			ARAM_DMA_RequestManager();

_inline_ tdeARAMHeapType ARAM_GetDefaultHeap(u8 _block) 
{ 
    return g_ARAMDefaultHeaps[_block]; 
}

_inline_ void ARAM_SetDefaultHeap(u8 _block, tdeARAMHeapType _heap)
{ 
    g_ARAMDefaultHeaps[_block] = _heap; 
}
#else // GC2RVL_NO_ARAM
#include "BASe/CLibrary/CLIstr.h"
inline
BOOL ARAM_CreateHeaps(u8 _block, u8 _nbHeaps, u32 *_sizeOfHeaps) {return TRUE;}

inline
BOOL ARAM_DestroyHeaps(u8 _block) {return TRUE;}

inline
u32 ARAM_Alloc(u8 _block, u8 _heap, u32 _size, u8 _Type)
{
	return (u32) MEM_p_Alloc(_size);
}

inline
void ARAM_Free(u8 _block, u8 _heap, u32 _addr)
{
	MEM_Free((void*)_addr);
}

inline
void ARAM_SendToARAM(u32 _ramAddr, u32 _aramAddr, u32 _length, u32 _userData, ARQCallback _Callback)
{
	L_memcpy((void*)_aramAddr,(void*)_ramAddr, _length);
	if(_Callback)
		_Callback(_userData);
}
inline
void ARAM_GetFromARAM(u32 _ramAddr, u32 _aramAddr, u32 _length, u32 _userData, ARQCallback _Callback)
{
	L_memcpy((void*)_ramAddr, (void*)_aramAddr, _length);
	if(_Callback)
		_Callback(_userData);
}
#endif // GC2RVL_NO_ARAM
#endif