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

#ifndef __GC_ARAMHEAP_H__
#define __GC_ARAMHEAP_H__

#define	FirstFreeBlockAlloc 		0
#define	AfterLastBlockAlloc 		1


typedef struct _tdstMemoryBlock
{
	u32 memAddress;
	u32 SizeAndType; // first bit : 0=free, 1=allocated. Other 31 bits : size
} tdstMemoryBlock;

typedef struct _tdstMemoryBlockList
{
	struct _tdstMemoryBlockList* next;
	tdstMemoryBlock block;
} tdstMemoryBlockList;

typedef struct _tdstARAMHeap
{
	u32 m_u32HeapAddr;
	u32 m_u32HeapSize;
	
	tdstMemoryBlockList m_lstMemBlocksList;	
	
	struct _tdstMemoryBlockList* last;
	
} tdstARAMHeap;

#define ARAM_uGetSize(block) (((block).SizeAndType) & 0x7FFFFFFF)
#define ARAM_uGetType(block) (((block).SizeAndType) & 0x80000000)
#define ARAM_SetSize(block,size) ((((block).SizeAndType) = ((size) & 0x7FFFFFFF) | ARAM_uGetType(block)))

void   ARAM_Heap_Init(tdstARAMHeap *_heap, u32 _memaddr, u32 _size);
void   ARAM_Heap_Free(tdstARAMHeap *_heap);
u32    ARAM_Heap_Alloc(tdstARAMHeap *_heap, u32 _size, u8 _Type);
void   ARAM_Heap_DeAlloc(tdstARAMHeap *_heap, u32 _addr);

#endif