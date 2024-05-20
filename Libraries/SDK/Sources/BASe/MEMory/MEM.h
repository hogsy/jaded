/*$T MEM.h GC! 1.097 02/14/01 14:15:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Memory managment */
#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __MEM_H__
#define __MEM_H__
#include "BASe/BAStypes.h"

/* For the L_malloc... */
#include "BASe/BASsys.h"
#include "BASe/ERRors/ERRasser.h"

#if defined(_XBOX) && !defined(_XENON)
#include <xtl.h>
#endif //_XBOX

/* #include "MATHs/MATH.h" */
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#if (defined MEM_OPT) && !(defined _FINAL_)
#define MEM_MEASURES
#endif // if (defined MEM_OPT) && !(defined _FINAL_)

#ifndef _FINAL_
#define MEM_SPY
#endif //_FINAL_

//#define ANIMS_USE_ARAM
//#endif // _GAMECUBE


/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

/* Number of values in the Zlist */
#define MEM_C_ZlistSize 2304

#define MEM_C_BlockHeaderSize	8

#define MEM_Cul_BlockMarkMEMpAlloc		0x110270
#define MEM_Cul_BlockMarkMEMpEcoAlloc	0x050671
#define MEM_Cul_BlockMarkMEMpAllocAlign 0xc0ffee
#define MEM_Cul_BlockAllocated			0xA0000000
#define MEM_Cul_BlockFree				0xB0000000
#define MEM_Cul_MaxSize					(LONG) 134217216

/* SP2 is for "Somme des puissances de 2" */
#define SP2_9_10									(((LONG) 1 << 9) + ((LONG) 1 << 10))
#define SP2_9_11									(SP2_9_10 + ((LONG) 1 << 11))
#define SP2_9_12									(SP2_9_11 + ((LONG) 1 << 12))
#define SP2_9_13									(SP2_9_12 + ((LONG) 1 << 13))
#define SP2_9_14									(SP2_9_13 + ((LONG) 1 << 14))
#define SP2_9_15									(SP2_9_14 + ((LONG) 1 << 15))
#define SP2_9_16									(SP2_9_15 + ((LONG) 1 << 16))
#define SP2_9_17									(SP2_9_16 + ((LONG) 1 << 17))
#define SP2_9_18									(SP2_9_17 + ((LONG) 1 << 18))
#define SP2_9_19									(SP2_9_18 + ((LONG) 1 << 19))
#define SP2_9_20									(SP2_9_19 + ((LONG) 1 << 20))
#define SP2_9_21									(SP2_9_20 + ((LONG) 1 << 21))
#define SP2_9_22									(SP2_9_21 + ((LONG) 1 << 22))
#define SP2_9_23									(SP2_9_22 + ((LONG) 1 << 23))
#define SP2_9_24									(SP2_9_23 + ((LONG) 1 << 24))
#define SP2_9_25									(SP2_9_24 + ((LONG) 1 << 25))
#define SP2_9_26									(SP2_9_25 + ((LONG) 1 << 26))
#define SP2_9_27									(SP2_9_26 + ((LONG) 1 << 27))

#define MEM_ERR_Csz_NotEnoughSystemMemory			"Not enough system memory to perform the initial allocation"
#define MEM_ERR_Csz_NoMoreMemoryToPerformAllocation "No more dynamic memory to perform this allocation"
#define MEM_ERR_Csz_BlockNotFreed					"Memory block has not been freed"
#define MEM_ERR_Csz_JadeMemoryManagementStart		"================ JADE MEMORY MANAGEMENT WARNING ================\n"
#define MEM_ERR_Csz_JadeMemoryManagementEnd			"================ JADE MEMORY MANAGEMENT END ================\n"
#define MEM_ERR_Csz_UnvalidPointer					"Unvalid pointer"

#ifdef __cplusplus

/*$4
***********************************************************************************************************************
    Macros
***********************************************************************************************************************
*/
#define M_DeclareOperatorNewAndDelete()                                     \
    _inline_ void* operator new(size_t _SizeStr)                            \
    {                                                                       \
        return MEM_p_Alloc(_SizeStr);                                       \
    }                                                                       \
    _inline_ void * operator new[](size_t _SizeStr)                         \
    {                                                                       \
        return MEM_p_Alloc(_SizeStr);                                       \
    }                                                                       \
    _inline_ void * operator new(size_t _SizeStr, void* _AllocatedAddress)  \
    {                                                                       \
        return _AllocatedAddress;                                           \
    }                                                                       \
    _inline_ void operator delete(void *_pPtr)                              \
    {                                                                       \
        MEM_Free(_pPtr);                                                    \
    }                                                                       \
    _inline_ void operator delete[](void *_pPtr)                            \
    {                                                                       \
        MEM_Free(_pPtr);                                                    \
    }                                                                       \
    _inline_ void operator delete(void*, void*)                             \
    {                                                                       \
    }

#endif

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

typedef struct	MEM_tdst_ZlistEntry_
{
	ULONG	ul_NumFreeDefrag;
	ULONG	ul_Size;
	ULONG	ul_Count;
	void	*pv_Hole;
} MEM_tdst_ZlistEntry;


#ifdef ACTIVE_EDITORS
// We can have elements from 16 * 2^0 bytes included to 16 * 2^MEM_C_PowerArraySize bytes excluded.
#define MEM_C_PowerArraySize 28 // 4 Go max
#elif _XENON
#define MEM_C_PowerArraySize 25 // 512 Mo max
#else 
#define MEM_C_PowerArraySize 22 // 64 Mo max
#endif // ACTIVE_EDITORS

typedef struct	MEM_tdst_Element_
{
	ULONG						u_Size; 			// Size of the blocks in this element's list (p_BlockList).
	void						*p_BlockList;		// First free block of size u_Size.
	struct MEM_tdst_Element_	*pst_PrevElement; 	// Previous element (of smaller u_Size). NULL when first in _pMem->ap_PowerArray.
	struct MEM_tdst_Element_	*pst_NextElement; 	// Next element (of bigger u_Size).
} MEM_tdst_Element;

#ifdef JADEFUSION
__declspec(align(32)) typedef struct MEM_tdst_MainStruct_
#else
typedef struct	MEM_tdst_MainStruct_
#endif
{
	// Main dynamic block begin address
	void				*pv_DynamicBloc;

	// First allocated address at end of heap (i.e. end of last free block)
	void *pv_LastFree;

	// Total size of dynamic block (in bytes). Is reduced if end of heap alloc are made.
	ULONG				ul_DynamicMaxSize;

	// Currently allocated size in the Dynamic block (in bytes).
	ULONG				ul_DynamicCurrentAllocated;
	
	// Currently allocated texture size (in bytes).
	ULONG				ul_TexturesCurrentAllocated;
	 		
	// Number of holes, or free blocks in dynamic block (the name is badly chosen).
	ULONG				ul_HolesStatic;
	
	// Memory heap size. Does not ever change. 
	ULONG ul_RealSize;

#ifdef MEM_SPY
	char *pcHeapName;
#endif // MEM_SPY

#ifdef MEM_OPT

	// First address after heap not in heap
	void *pvMemoryHeapEnd; 	
	
	// Disable interrupts ? True for basic management (sound should not interrupt), false for virtual management that needs to be interrupted by low level VM.
	int bDisableInterrupts;

	// Virtual allocations ?
	int bVirtual;

#ifdef _RVL
	// Try this heap if no mem.
	struct MEM_tdst_MainStruct_ *pLastResort;
#endif // _RVL

#ifdef MEM_MEASURES
	// Current free memory (last block + holes) in bytes
	ULONG 				ul_CurrentFree;

	// Minimum size of free memory ever reached, in bytes.
	ULONG 				ul_MinFreeTotal;
	
	// Minimum size of last free block ever reached, in bytes.
	ULONG 				ul_MinLastFree;
#endif // MEM_MEASURES
	
	// Array of pointers to lists of elements. 
	// The size of the elements in the list for index N vary from 16 * 2^N to 16 * (2^(N+1) - 1)
	MEM_tdst_Element *ap_PowerArray[MEM_C_PowerArraySize];
	
	void				*pv_DynamicNextFree;

#else // MEM_OPT
	void				*pv_StaticBloc;			/* Main static bloc (unused yet) */
	void				*pv_DynamicNextFree;	/* Next free adress (not holes) */

	/* Max number of bytes in the static bloc (unused) */
	ULONG				ul_StaticMaxSize;

	/* Currently allocated size in the Static bloc (unused) */
	ULONG				ul_StaticCurrentAllocated;

	/* Z list of holes */
	MEM_tdst_ZlistEntry MEM_ast_Zlist[MEM_C_ZlistSize];
#endif // MEM_OPT

} MEM_tdst_MainStruct;



/*$4
 ***********************************************************************************************************************
    Variables
 ***********************************************************************************************************************
 */
#ifdef JADEFUSION
extern __declspec(align(32)) MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#else
extern MEM_tdst_MainStruct	MEM_gst_MemoryInfo;
#endif
#ifdef _GAMECUBE
// virtual space
extern MEM_tdst_MainStruct	MEM_gst_VirtualMemoryInfo;

// memory allocated by sound API (because it creates frags in memory)
extern MEM_tdst_MainStruct	MEM_gst_SoundMemoryInfo;

// When loading, this memory is used by BINK, load buffers, and small frame buffers.
// When playing, this memory is used by after fx, z list, compute buffer, SPG2 cache lines and big frame buffers.
extern MEM_tdst_MainStruct	MEM_gst_GCMemoryInfo;
#define MEM_IsTmpPointer(p) 0
#else //_GAMECUBE

extern char *MEM_gp_AllocTmpFirst;
extern char *MEM_gp_AllocTmpLast;
#define MEM_IsTmpPointer(p) (((p) >= MEM_gp_AllocTmpFirst) && ((p) < MEM_gp_AllocTmpLast))
#endif //_GAMECUBE

extern void *MEM_p_AllocTmp(ULONG);
#ifdef JADEFUSION
extern void MEM_FreeTmp(void *p);
#endif
extern BOOL MEM_gb_EnableTmp;
extern void MEM_ResetTmpMemory(void);

/*$3
 =======================================================================================================================
    Static allocation (For the moment = normal allocation, but will be changed)
 =======================================================================================================================
 */
#ifdef JADEFUSION
#define MEM_p_sSize				L_mallocsize
#endif
#define MEM_p_sAlloc			L_malloc
#define MEM_sFree				L_free
#define MEM_p_sRealloc(a, b)	L_realloc(a, b)

/*$4
 ***********************************************************************************************************************
    Init/close
 ***********************************************************************************************************************
 */

extern void MEM_InitModule(void);
extern void MEM_CloseModule(void);

/*$4
 ***********************************************************************************************************************
    Allocation / Free (normal, aligned, from end)
 ***********************************************************************************************************************
 */

#	define MEM_Free( a )        free( a )
#	define MEM_FreeFromEnd( a ) free( a )
#	define MEM_FreeAlign( a )   _aligned_free( a )

#	define MEM_SafeFree( a )  \
		do {                   \
			if ( a != NULL )   \
			{                  \
				MEM_Free( a ); \
				( a ) = NULL;  \
			}                  \
		} while ( 0 )
#	define MEM_SetNextType( a )

enum MEM_Type
{
	MEM_td_Undefined,
	MEM_td_Geometry,
	MEM_td_Geometry_Skinning,
	MEM_td_Materials,
	MEM_td_GAO,
	MEM_td_GFX,
	MEM_td_SPG2,
	MEM_td_Anims,
	MEM_td_IAstructs,
	MEM_td_Collision,
	MEM_td_ODE,
	MEM_td_Others
};

#ifdef MEM_OPT

	#define MEM_p_AllocFromEndAlign(_size, _align)	MEM_p_AllocFromEnd(_size)
	#define MEM_FreeFromEndAlign(_block)			MEM_FreeFromEnd(_block)




	#ifdef MEM_SPY
	extern void *_MEM_p_Alloc(MEM_tdst_MainStruct *,ULONG, char *, int);
	extern void *_MEM_p_Realloc(MEM_tdst_MainStruct *,void *_pv_Block, ULONG _ul_Size, char *, int);
	extern void *_MEM_p_AllocAlign(MEM_tdst_MainStruct *,ULONG, ULONG, char*, int);
	extern void *_MEM_p_ReallocAlign(MEM_tdst_MainStruct *,void * _pv, ULONG _ul_BlockSize, ULONG Alignment, char*file, int line);
	extern void *_MEM_p_AllocFromEnd(MEM_tdst_MainStruct *,ULONG _ul_size, char *_str_file, int _i_line);
	#else // MEM_SPY
	extern void *_MEM_p_Alloc(MEM_tdst_MainStruct *,ULONG);
	extern void *_MEM_p_Realloc(MEM_tdst_MainStruct *,void *_pv_Block, ULONG _ul_Size);
	extern void *_MEM_p_AllocAlign(MEM_tdst_MainStruct *,ULONG, ULONG);
	extern void *_MEM_p_ReallocAlign(MEM_tdst_MainStruct *,void * _pv, ULONG _ul_BlockSize, ULONG Alignment);
	extern void *_MEM_p_AllocFromEnd(MEM_tdst_MainStruct *,ULONG _ul_size);
	#endif /* MEM_SPY */

	#ifdef MEM_SPY
	
	// Normal alloc
	#define MEM_p_Alloc(a)							_MEM_p_Alloc(&MEM_gst_MemoryInfo,a, __FILE__, __LINE__)
	#define MEM_p_Realloc(a, b) 					_MEM_p_Realloc(&MEM_gst_MemoryInfo,a, b, __FILE__, __LINE__)
	#define MEM_p_AllocAlign(a, Align)				_MEM_p_AllocAlign(&MEM_gst_MemoryInfo,a, Align, __FILE__, __LINE__)
	#define MEM_p_ReallocAlign(a, b, Align)			_MEM_p_ReallocAlign(&MEM_gst_MemoryInfo,a, b, Align, __FILE__, __LINE__)
	#define MEM_p_AllocFromEnd(_size)				_MEM_p_AllocFromEnd(&MEM_gst_MemoryInfo,_size, __FILE__, __LINE__)

	// Alloc from specific mem heap
	#define MEM_p_AllocMem(a,pMem)					_MEM_p_Alloc(pMem,a, __FILE__, __LINE__)
	#define MEM_p_ReallocMem(a, b,pMem)		 		_MEM_p_Realloc(pMem,a, b, __FILE__, __LINE__)
	#define MEM_p_AllocAlignMem(a, Align,pMem)		_MEM_p_AllocAlign(pMem,a, Align, __FILE__, __LINE__)
	#define MEM_p_ReallocAlignMem(a,b,Align,pMem)	_MEM_p_ReallocAlign(pMem,a, b, Align, __FILE__, __LINE__)
	#define MEM_p_AllocFromEndMem(_size,pMem)		_MEM_p_AllocFromEnd(pMem,_size, __FILE__, __LINE__)

	// VM Alloc
    #ifdef _GAMECUBE
	#define MEM_p_VMAlloc(a)						_MEM_p_Alloc(&MEM_gst_VirtualMemoryInfo,a, __FILE__, __LINE__)
	#define MEM_p_VMRealloc(a, b) 					_MEM_p_Realloc(&MEM_gst_VirtualMemoryInfo,a, b, __FILE__, __LINE__)
	#define MEM_p_VMAllocAlign(a, Align)			_MEM_p_AllocAlign(&MEM_gst_VirtualMemoryInfo,a, Align, __FILE__, __LINE__)
	#define MEM_p_VMReallocAlign(a, b, Align) 		_MEM_p_ReallocAlign(&MEM_gst_VirtualMemoryInfo,a, b, Align, __FILE__, __LINE__)
	#define MEM_p_VMAllocFromEnd(_size)				_MEM_p_AllocFromEnd(&MEM_gst_VirtualMemoryInfo,_size, __FILE__, __LINE__)
    #else // _GAMECUBE
	#define MEM_p_VMAlloc(a)						_MEM_p_Alloc(&MEM_gst_MemoryInfo,a, __FILE__, __LINE__)
	#define MEM_p_VMRealloc(a, b) 					_MEM_p_Realloc(&MEM_gst_MemoryInfo,a, b, __FILE__, __LINE__)
	#define MEM_p_VMAllocAlign(a, Align)			_MEM_p_AllocAlign(&MEM_gst_MemoryInfo,a, Align, __FILE__, __LINE__)
	#define MEM_p_VMReallocAlign(a, b, Align) 		_MEM_p_ReallocAlign(&MEM_gst_MemoryInfo,a, b, Align, __FILE__, __LINE__)
	#define MEM_p_VMAllocFromEnd(_size)				_MEM_p_AllocFromEnd(&MEM_gst_MemoryInfo,_size, __FILE__, __LINE__)
    #endif // _GAMECUBE
    
	#else // MEM_SPY
	
	// Normal alloc
	#define MEM_p_Alloc(a)							_MEM_p_Alloc(&MEM_gst_MemoryInfo,a)
	#define MEM_p_Realloc(a, b) 					_MEM_p_Realloc(&MEM_gst_MemoryInfo,a, b)
	#define MEM_p_AllocAlign(a, Align)				_MEM_p_AllocAlign(&MEM_gst_MemoryInfo,a, Align)
	#define MEM_p_ReallocAlign(a, b, Align) 		_MEM_p_ReallocAlign(&MEM_gst_MemoryInfo,a, b, Align)
	#define MEM_p_AllocFromEnd(_size)				_MEM_p_AllocFromEnd(&MEM_gst_MemoryInfo,_size)

	// Alloc from specific mem heap
	#define MEM_p_AllocMem(a,pMem)					_MEM_p_Alloc(pMem,a)
	#define MEM_p_ReallocMem(a, b,pMem)		 		_MEM_p_Realloc(pMem,a, b)
	#define MEM_p_AllocAlignMem(a, Align,pMem)		_MEM_p_AllocAlign(pMem,a, Align)
	#define MEM_p_ReallocAlignMem(a,b,Align,pMem)	_MEM_p_ReallocAlign(pMem,a, b, Align)
	#define MEM_p_AllocFromEndMem(_size,pMem)		_MEM_p_AllocFromEnd(pMem,_size)
	
	// VM Alloc
    #ifdef _GAMECUBE
	#define MEM_p_VMAlloc(a)						_MEM_p_Alloc(&MEM_gst_VirtualMemoryInfo,a)
	#define MEM_p_VMRealloc(a, b) 					_MEM_p_Realloc(&MEM_gst_VirtualMemoryInfo,a, b)
	#define MEM_p_VMAllocAlign(a, Align)			_MEM_p_AllocAlign(&MEM_gst_VirtualMemoryInfo,a, Align)
	#define MEM_p_VMReallocAlign(a, b, Align) 		_MEM_p_ReallocAlign(&MEM_gst_VirtualMemoryInfo,a, b, Align)
	#define MEM_p_VMAllocFromEnd(_size)				_MEM_p_AllocFromEnd(&MEM_gst_VirtualMemoryInfo,_size)
    #else // _GAMECUBE
	#define MEM_p_VMAlloc(a)						_MEM_p_Alloc(&MEM_gst_MemoryInfo,a)
	#define MEM_p_VMRealloc(a, b) 					_MEM_p_Realloc(&MEM_gst_MemoryInfo,a, b)
	#define MEM_p_VMAllocAlign(a, Align)			_MEM_p_AllocAlign(&MEM_gst_MemoryInfo,a, Align)
	#define MEM_p_VMReallocAlign(a, b, Align) 		_MEM_p_ReallocAlign(&MEM_gst_MemoryInfo,a, b, Align)
	#define MEM_p_VMAllocFromEnd(_size)				_MEM_p_AllocFromEnd(&MEM_gst_MemoryInfo,_size)
    #endif // _GAMECUBE
    
	#endif /* MEM_SPY */


#else // MEM_OPT

#		if 0
	#ifdef PSX2_TARGET
	#ifdef _DEBUG
	extern void *_MEM_p_AllocFromEndAlign(ULONG _ul_size, ULONG _i_modulo, char *_str_file, int _i_line);
	#define MEM_p_AllocFromEndAlign(_size, _align)	_MEM_p_AllocFromEndAlign(_size, _align, __FILE__, __LINE__)
	#else
	extern void *_MEM_p_AllocFromEndAlign(ULONG _ul_size, ULONG _i_modulo);
	#define MEM_p_AllocFromEndAlign(_size, _align)	_MEM_p_AllocFromEndAlign(_size, _align)
	#endif /* _DEBUG */
	extern void MEM_FreeFromEndAlign(void *_pv_block);
	#else
	#define MEM_p_AllocFromEndAlign(_size, _align)	MEM_p_AllocFromEnd(_size)
	#define MEM_FreeFromEndAlign(_block)			MEM_FreeFromEnd(_block)
	#endif /* PSX2_TARGET */

	#ifdef _DEBUG
	extern void *_MEM_p_Alloc(ULONG, char *, int);
	extern void *_MEM_p_Realloc(void *_pv_Block, ULONG _ul_Size, char *, int);
	extern void *_MEM_p_AllocFromEnd(ULONG _ul_size, char *_str_file, int _i_line);
    extern void *_MEM_p_AllocAlign(ULONG, ULONG, char*, int);
    extern void *_MEM_p_ReallocAlign(void * _pv, ULONG _ul_BlockSize, ULONG Alignment, char*file, int line);
	#else // _DEBUG
	extern void *_MEM_p_AllocFromEnd(ULONG _ul_size);
	extern void *_MEM_p_Alloc(ULONG);
	extern void *_MEM_p_Realloc(void *_pv_Block, ULONG _ul_Size);
    extern void *_MEM_p_AllocAlign(ULONG, ULONG);
    extern void *_MEM_p_ReallocAlign(void * _pv, ULONG _ul_BlockSize, ULONG Alignment);
	#endif /* _DEBUG */
#else

#			define _MEM_p_Alloc( _SIZE, ... )           calloc( 1, ( _SIZE ) )
#			define _MEM_p_Realloc( _BLOCK, _SIZE, ... ) realloc( ( _BLOCK ), ( _SIZE ) )

#			define _MEM_p_AllocFromEnd( _SIZE, ... ) calloc( 1, ( _SIZE ) )

#			define _MEM_p_AllocAlign( _SIZE, _ALIGN, ... )           memset( _aligned_malloc( _SIZE, _ALIGN ), 0, _SIZE )
#			define _MEM_p_ReallocAlign( _BLOCK, _SIZE, _ALIGN, ... ) _aligned_realloc( _BLOCK, _SIZE, _ALIGN )

#			define MEM_p_AllocFromEndAlign( _SIZE, _ALIGN ) _MEM_p_AllocAlign( _SIZE, _ALIGN )
#			define MEM_FreeFromEndAlign( _PTR )             _aligned_free( ( _PTR ) )

#endif

	#ifdef _DEBUG
	#define MEM_p_Alloc(a)		   			_MEM_p_Alloc(a, __FILE__, __LINE__)
	#define MEM_p_VMAlloc(a)				_MEM_p_Alloc(a, __FILE__, __LINE__)

	#define MEM_p_Realloc(a, b)         	_MEM_p_Realloc(a, b, __FILE__, __LINE__)
	#define MEM_p_VMRealloc(a, b)       	_MEM_p_Realloc(a, b, __FILE__, __LINE__)

	#define MEM_p_AllocFromEnd(_size)		_MEM_p_AllocFromEnd(_size, __FILE__, __LINE__)
	#define MEM_p_VMAllocFromEnd(_size)		_MEM_p_AllocFromEnd(_size, __FILE__, __LINE__)

    #define MEM_p_AllocAlign(a, Align)				_MEM_p_AllocAlign(a, Align, __FILE__, __LINE__)
    #define MEM_p_VMAllocAlign(a, Align)			_MEM_p_AllocAlign(a, Align, __FILE__, __LINE__)

    #define MEM_p_ReallocAlign(a, b, Align) 		_MEM_p_ReallocAlign(a, b, Align, __FILE__, __LINE__)
    #define MEM_p_VMReallocAlign(a, b, Align) 		_MEM_p_ReallocAlign(a, b, Align, __FILE__, __LINE__)
	#else // _DEBUG
	#define MEM_p_Alloc(a)					_MEM_p_Alloc(a)
	#define MEM_p_VMAlloc(a)				_MEM_p_Alloc(a)

	#define MEM_p_Realloc(a, b) 			_MEM_p_Realloc(a, b)
	#define MEM_p_VMRealloc(a, b)			_MEM_p_Realloc(a, b)

    #define MEM_p_AllocFromEnd(_size)		_MEM_p_AllocFromEnd(_size)
	#define MEM_p_VMAllocFromEnd(_size)		_MEM_p_AllocFromEnd(_size)
    #ifdef ACTIVE_EDITORS
        #define MEM_p_AllocAlign(a, Align)				_MEM_p_Alloc(a)
	    #define MEM_p_VMAllocAlign(a, Align)			_MEM_p_Alloc(a)
    	#define MEM_p_ReallocAlign(a, b, Align)	 		_MEM_p_Realloc(a, b)
    	#define MEM_p_VMReallocAlign(a, b, Align) 		_MEM_p_Realloc(a, b)
    #else // ACTIVE_EDITORS
        #define MEM_p_AllocAlign(a, Align)				_MEM_p_AllocAlign(a, Align)
	    #define MEM_p_VMAllocAlign(a, Align)			_MEM_p_AllocAlign(a, Align)
    	#define MEM_p_ReallocAlign(a, b, Align) 		_MEM_p_ReallocAlign(a, b, Align)
    	#define MEM_p_VMReallocAlign(a, b, Align) 		_MEM_p_ReallocAlign(a, b, Align)
    #endif //ACTIVE_EDITORS
	#endif /* _DEBUG */

#ifdef JADEFUSION
		extern void MEM_FreeFromEnd(void *_pv_block);
#endif

#endif // MEM_OPT


#ifdef MEM_SPY

typedef struct MEM_stElement_
{
    char *sFile;
    int iLine;
    LONG iAllocSize;
    LONG iAllocSizeMax;
    short iAllocNb;
    short iAllocNbMax;
} MEM_stElement;

#define MEM_TableSize 512

extern MEM_stElement MEM_SpyTable[MEM_TableSize];

extern BOOL MEM_bDisplayLeaks; // True if there are leaks to display 
extern int MEM_iLoss;
extern int MEM_iGain;
extern MEM_stElement *MEM_SortedLeaks[MEM_TableSize];


typedef struct MEM_stSummaryElement_
{
    union 
    {
        char sHeader[4];
        BOOL bIsUsed;
    };
    ULONG ulKey;
    ULONG ulAllocSize;
    ULONG ulAllocNb;
} MEM_stSummaryElement;

#define MEM_ulSummaryTableSize 64

extern MEM_stSummaryElement *MEM_SortSummaryTable[MEM_ulSummaryTableSize];
extern ULONG MEM_ulSummaryNb;			

extern void MEM_vBuildSummary();

extern void MEM_vSaveSpySnapShot();
extern void MEM_vComputeSpyDeltaSnapShot();

extern BOOL MEM_bDontTrackLeaks;
#define MEM_STOP_TRACK_LEAKS() do { MEM_bDontTrackLeaks = TRUE;} while (0)
#define MEM_START_TRACK_LEAKS() do { MEM_bDontTrackLeaks = FALSE;}  while (0)

#else // MEM_SPY
#define MEM_STOP_TRACK_LEAKS() 
#define MEM_START_TRACK_LEAKS()   
#endif // MEM_SPY

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    RAM allocation infos
 -----------------------------------------------------------------------------------------------------------------------
 */


#ifdef MEM_OPT

void MEM_vFromEndFastFree(MEM_tdst_MainStruct *_pMem,void *); 

void MEM_vManageSnapShot();

// Get size of last block (free mem discarding holes)
extern ULONG MEM_uGetLastBlockSize(MEM_tdst_MainStruct *_pMem);

#define MEM_uGetAllocatedSizeVirtual() (MEM_gst_VirtualMemoryInfo.ul_DynamicCurrentAllocated)
#define MEM_uGetAllocatedSizeBasic() (MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated)

#define MEM_uGetAllocatedSize() (MEM_uGetAllocatedSizeBasic() + MEM_uGetAllocatedSizeVirtual())

// Size of allocations + holes 
#define MEM_uGetAllocatedBlockSize(_pMem) ((_pMem)->ul_DynamicMaxSize - MEM_uGetLastBlockSize(_pMem))

// Size of free memory (holes + last block) 
#define MEM_uGetFreeMemSize(_pMem) ((_pMem)->ul_DynamicMaxSize - (_pMem)->ul_DynamicCurrentAllocated)

// Size of holes (not counting free block)
#define MEM_uGetHolesSize(_pMem) ((_pMem)->ul_DynamicMaxSize - ((_pMem)->ul_DynamicCurrentAllocated + MEM_uGetLastBlockSize(_pMem)))

#else // MEM_OPT

// Size of allocations + holes 
#define MEM_uGetAllocatedBlockSize() (MEM_gst_MemoryInfo.ul_DynamicMaxSize - MEM_uGetLastBlockSize())

// Size of allocations
#define MEM_uGetAllocatedSize() (MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated)

#define MEM_vManageSnapShot()

#endif // MEM_OPT





/*$2
 -----------------------------------------------------------------------------------------------------------------------
    check mode
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef _DEBUG
//#define MEM_CHECK
#endif // _DEBUG

#ifdef MEM_CHECK
void MEM_vCheckFreeBlock(void *_pAddress);
void MEM_vCheckAllocatedBlock(void *_pAddress);
void MEM_vSlowCheckAllocatedBlock(MEM_tdst_MainStruct *_pMem,void *_pAddress);
void MEM_vCheckMem(MEM_tdst_MainStruct *_pMem);
#else // MEM_CHECK
#define MEM_vCheckFreeBlock(_pAddress)
#define MEM_vCheckAllocatedBlock(_pAddress)
#define MEM_vSlowCheckAllocatedBlock(_pMem,_pAddress)
#define MEM_vCheckMem(_pMem)
#endif// MEM_CHECK

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    opt mode
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifndef MEM_OPT


/*$4
 ***********************************************************************************************************************
    Handle the Size or Index in various ways
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Returns the index in the Z-list for a given size

    Note:   Does not test if the index is in the limits or not, so be carefull
 =======================================================================================================================
 */
_inline_ ULONG MEM_ul_GetIndex(ULONG _ul_BlockSize)
{
	/* Find the index in Z list */
	if(_ul_BlockSize <= 512)
		return((_ul_BlockSize - 1) >> 2);
	else if(_ul_BlockSize <= SP2_9_10)
		return(((_ul_BlockSize - 512 - 1) >> 3) + 128);
	else if(_ul_BlockSize <= SP2_9_11)
		return(((_ul_BlockSize - SP2_9_10 - 1) >> 4) + 128 * 2);
	else if(_ul_BlockSize <= SP2_9_12)
		return(((_ul_BlockSize - SP2_9_11 - 1) >> 5) + 128 * 3);
	else if(_ul_BlockSize <= SP2_9_13)
		return(((_ul_BlockSize - SP2_9_12 - 1) >> 6) + 128 * 4);
	else if(_ul_BlockSize <= SP2_9_14)
		return(((_ul_BlockSize - SP2_9_13 - 1) >> 7) + 128 * 5);
	else if(_ul_BlockSize <= SP2_9_15)
		return(((_ul_BlockSize - SP2_9_14 - 1) >> 8) + 128 * 6);
	else if(_ul_BlockSize <= SP2_9_16)
		return(((_ul_BlockSize - SP2_9_15 - 1) >> 9) + 128 * 7);
	else if(_ul_BlockSize <= SP2_9_17)
		return(((_ul_BlockSize - SP2_9_16 - 1) >> 10) + 128 * 8);
	else if(_ul_BlockSize <= SP2_9_18)
		return(((_ul_BlockSize - SP2_9_17 - 1) >> 11) + 128 * 9);
	else if(_ul_BlockSize <= SP2_9_19)
		return(((_ul_BlockSize - SP2_9_18 - 1) >> 12) + 128 * 10);
	else if(_ul_BlockSize <= SP2_9_20)
		return(((_ul_BlockSize - SP2_9_19 - 1) >> 13) + 128 * 11);
	else if(_ul_BlockSize <= SP2_9_21)
		return(((_ul_BlockSize - SP2_9_20 - 1) >> 14) + 128 * 12);
	else if(_ul_BlockSize <= SP2_9_22)
		return(((_ul_BlockSize - SP2_9_21 - 1) >> 15) + 128 * 13);
	else if(_ul_BlockSize <= SP2_9_23)
		return(((_ul_BlockSize - SP2_9_22 - 1) >> 16) + 128 * 14);
	else if(_ul_BlockSize <= SP2_9_24)
		return(((_ul_BlockSize - SP2_9_23 - 1) >> 17) + 128 * 15);
	else if(_ul_BlockSize <= SP2_9_25)
		return(((_ul_BlockSize - SP2_9_24 - 1) >> 18) + 128 * 16);
	else if(_ul_BlockSize <= SP2_9_26)
		return(((_ul_BlockSize - SP2_9_25 - 1) >> 19) + 128 * 17);
	else
	{
		/* If we break here, we have a block size over 131 Mb which is not allowed */
		ERR_X_ForceAssert();
		return(0);
	}
}


/*
 =======================================================================================================================
    Aim:    Return the real allocated size, given a wanted size
 =======================================================================================================================
 */
_inline_ ULONG MEM_ul_GetRealSizeFromSize(ULONG _ul_Size)
{
	return(MEM_gst_MemoryInfo.MEM_ast_Zlist[MEM_ul_GetIndex(_ul_Size)].ul_Size);
}

/*
 =======================================================================================================================
    Aim:    Return the real allocated size, given an index in the Z table
 =======================================================================================================================
 */
_inline_ ULONG MEM_ul_GetRealSizeFromIndex(ULONG _ul_Index)
{
	return(MEM_gst_MemoryInfo.MEM_ast_Zlist[_ul_Index].ul_Size);
}

/*
 =======================================================================================================================
    Aim:    Given a wished Block size to allocate, we return the index in the z list and the real size to allocate

    In:     _pul_index      : Pointer to be filled with the index in the z list
            _pul_size       : Pointer to be filled with the real size to allocate (always greater or equal than the
                            asked size)
            _ul_BlockSize   : Asked size to allocate
 =======================================================================================================================
 */
_inline_ void MEM_GetIndexAndRealSize(ULONG *_pul_index, ULONG *_pul_size, ULONG _ul_BlockSize)
{
	/* If asserts here we have a block with a bad size */
	ERR_X_Assert(_ul_BlockSize > 0);

	/* Find the index in Z list */
	*_pul_index = MEM_ul_GetIndex(_ul_BlockSize);

	/* Check the index found is within the range of the Z list */
	ERR_X_Assert(*_pul_index < MEM_C_ZlistSize);

	/* Get the real Size to allocate */
	*_pul_size = MEM_ul_GetRealSizeFromIndex(*_pul_index);

	/*
	 * We chech there is no bug in the code... ul_Size must never be lower than the
	 * requested size
	 */
	ERR_X_Assert(*_pul_size >= _ul_BlockSize);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void MEM_SetMark(void *_pv_Block, ULONG _ul_Mark)
{
	(*(((ULONG *) _pv_Block) - 2)) = _ul_Mark;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ ULONG MEM_ul_GetMark(void *_pv_Block)
{
	return(*(((ULONG *) _pv_Block) - 2));
}


#endif // MEM_OPT

/*
 =======================================================================================================================
    Aim:    Return the really allocated size of a block
 =======================================================================================================================
 */
ULONG MEM_ul_GetRealSize(void *_pv_Block);


/*$4
 ***********************************************************************************************************************
    For debugging:
 ***********************************************************************************************************************
 */

#if defined(_DEBUG)

void	MEM_CheckDynamicBlocEmpty(void);
void	MEM_CheckAllocatedBlocks(char);
char	MEM_b_CheckPointer(void *p);

#else

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ char MEM_b_CheckPointer(void *p)
{
	if(p)
		return((char) 1);
	else
		return((char) 0);
}

#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __MEM_H__ */
