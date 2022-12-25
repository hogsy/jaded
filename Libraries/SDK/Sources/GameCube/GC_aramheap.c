/*$T GC_aramheap.c GC! 1.081 06/10/02 17:04:56 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * (C) Copyright 2000 Ubi Soft Author : Dany Joannette Date : 10 April 2001
 * Package BASE File aramheap.cpp Description ARAM heap dolphin
 */
#include "GameCube/GC_aramheap.h"
#include "BASe/MEMory/MEM.h"

/*
 =======================================================================================================================
    Initialization of the heap (starting ARAM address and size)
 =======================================================================================================================
 */
void ARAM_Heap_Init(tdstARAMHeap *_heap, u32 _memaddr, u32 _size)
{
	_heap->m_u32HeapAddr = _memaddr;
	_heap->m_u32HeapSize = _size;

	_heap->last = &_heap->m_lstMemBlocksList;	
	_heap->m_lstMemBlocksList.next = NULL;
	_heap->m_lstMemBlocksList.block.memAddress = _heap->m_u32HeapAddr;
	_heap->m_lstMemBlocksList.block.SizeAndType = _heap->m_u32HeapSize;
}

/*
 =======================================================================================================================
    Free the heap of all allocated chunks
 =======================================================================================================================
 */
void ARAM_Heap_Free(tdstARAMHeap *_heap)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	tdstMemoryBlockList *next;
	tdstMemoryBlockList *current;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	current = &_heap->m_lstMemBlocksList;

	while(current)
	{
		next = current->next;
		if(current != &_heap->m_lstMemBlocksList) MEM_Free(current);
		current = next;
	}

	_heap->last = &_heap->m_lstMemBlocksList;	
	_heap->m_lstMemBlocksList.next = NULL;
	_heap->m_lstMemBlocksList.block.memAddress = _heap->m_u32HeapAddr;
	_heap->m_lstMemBlocksList.block.SizeAndType = _heap->m_u32HeapSize;
}

/*
 =======================================================================================================================
    Allocate a new chunk of ARAM to the heap
 =======================================================================================================================
 */
u32 ARAM_Heap_Alloc(tdstARAMHeap *_heap, u32 _size, u8 _Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	u32					l_size;

	tdstMemoryBlock		*pNewBlock;
	tdstMemoryBlockList *pNewBlockList;
	tdstMemoryBlockList *pCurrentBlock;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_size = OSRoundUp32B(_size);
	pNewBlock = NULL;
	pNewBlockList = NULL;
	pCurrentBlock = NULL;

	/* find a free chunk of ARAM suitable for the allocation */
	if(_Type == FirstFreeBlockAlloc)
		pCurrentBlock = &_heap->m_lstMemBlocksList;
	else
	{
		pCurrentBlock = _heap->last;
		if(ARAM_uGetSize(pCurrentBlock->block) < l_size)
		{
    		pCurrentBlock = &_heap->m_lstMemBlocksList;		
    		_Type = FirstFreeBlockAlloc;
    	}
	}

	while
	(
		pCurrentBlock
	&&	(ARAM_uGetType(pCurrentBlock->block) != 0 || ARAM_uGetSize(pCurrentBlock->block) < l_size)
	)
	{
		/* not a free block or not large enough */
		pCurrentBlock = pCurrentBlock->next;
	}

	if(!pCurrentBlock)
	{
		OSReport("Could not allocate ARAM at %x \n",(ULONG)_heap);
		return 0xEFFFFFFE;
	}

	/*
	 * no need to add a new entry to the heap if all the chunk is to be used (simple
	 * change its flag)
	 */
	if(ARAM_uGetSize(pCurrentBlock->block) > l_size)
	{		
		MEM_STOP_TRACK_LEAKS();
		pNewBlockList = (tdstMemoryBlockList *) MEM_p_Alloc(sizeof(tdstMemoryBlockList));
		MEM_START_TRACK_LEAKS();
		pNewBlockList->next = pCurrentBlock->next;
		_heap->last = pNewBlockList;

		pCurrentBlock->next = pNewBlockList;
		
		pNewBlockList->block.memAddress = pCurrentBlock->block.memAddress + l_size;
		pNewBlockList->block.SizeAndType = 0x7FFFFFFF & (ARAM_uGetSize(pCurrentBlock->block) - l_size);
	}

	pCurrentBlock->block.SizeAndType = l_size | 0x80000000;

	return(pCurrentBlock->block.memAddress);
}

/*
 =======================================================================================================================
    Remove an allocated chunk of ARAM for the heap
 =======================================================================================================================
 */
void ARAM_Heap_DeAlloc(tdstARAMHeap *_heap, u32 _addr)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	tdstMemoryBlockList *pCurrentBlock;
	tdstMemoryBlockList *pPreviousBlock;
	tdstMemoryBlockList *pBlockList;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pCurrentBlock = NULL;
	pPreviousBlock = NULL;
	pBlockList = NULL;

	/* find the ARAM chunk by its address */
	pCurrentBlock = &_heap->m_lstMemBlocksList;
	pPreviousBlock = pCurrentBlock;
	while(pCurrentBlock && pCurrentBlock->block.memAddress != _addr)
	{
		if(pPreviousBlock != pCurrentBlock) 
			pPreviousBlock = pCurrentBlock;

		pCurrentBlock = pCurrentBlock->next;
	}

	if(!pCurrentBlock)
	{
		OSReport("Free of an ARAM block but block not found\n");
		return;
	}

	if(ARAM_uGetType(pCurrentBlock->block) == 0)	/* not found in list or already freed */
	{
		OSReport("Free of an ARAM block already freed!?\n");
		return;
	}

	pCurrentBlock->block.SizeAndType &= 0x7FFFFFFF;

	/*
	 * if not surrounded by allocated chunks, try to concatenate and then eliminate
	 * unused chunks
	 */
	if
	(
	    pCurrentBlock->next &&
		!(
		    (ARAM_uGetType(pCurrentBlock->block) != 0) && (ARAM_uGetType(pPreviousBlock->block) != 0)
		)
	)
	{
		/* merge the empty blocks together */
		if(pCurrentBlock != pPreviousBlock && ARAM_uGetType(pPreviousBlock->block) == 0)
		{
			pBlockList = pPreviousBlock;
			ARAM_SetSize(pBlockList->block,ARAM_uGetSize(pBlockList->block) + ARAM_uGetSize(pCurrentBlock->block));

			pBlockList->next = pCurrentBlock->next;

			MEM_Free(pCurrentBlock);

			if(pBlockList->next && ARAM_uGetType(pBlockList->next->block) == 0)
			{
				ARAM_SetSize(pBlockList->block,ARAM_uGetSize(pBlockList->block) + ARAM_uGetSize(pBlockList->next->block));
				pCurrentBlock = pBlockList->next;
				pBlockList->next = pBlockList->next->next;

				MEM_Free(pCurrentBlock);
			}
		}
		else if(pCurrentBlock->next && ARAM_uGetType(pCurrentBlock->next->block) == 0)
		{
			pBlockList = pCurrentBlock;
			ARAM_SetSize(pBlockList->block,ARAM_uGetSize(pBlockList->block) + ARAM_uGetSize(pBlockList->next->block));
			pCurrentBlock = pBlockList->next;
			pBlockList->next = pBlockList->next->next;

			MEM_Free(pCurrentBlock);
		}
	}
}

