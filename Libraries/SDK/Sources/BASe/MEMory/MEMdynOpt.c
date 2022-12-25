/*$T MEMdynOpt.c GC! 1.081 01/23/03 17:58:46 */

/*$4
 ***********************************************************************************************************************
    HEADERS
 ***********************************************************************************************************************
 */

#include "Precomp.h"

/*$2------------------------------------------------------------------------------------------------------------------*/

#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMLog.h"
#include "BASe/MEMory/MEMpro.h"
#include "BASe/BASsys.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "MATHs/MATHlong.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BASe/BENch/BENch.h"


#ifdef MEM_OPT

/*$2------------------------------------------------------------------------------------------------------------------*/

#if defined(_DEBUG) && !defined(PSX2_TARGET) && !defined(_GAMECUBE)
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "LINks/LINKmsg.h"
#endif /* debug + PC */

/*$2------------------------------------------------------------------------------------------------------------------*/

#ifdef _GAMECUBE
#include "GameCube/GC_arammng.h"
#include "Sound/Sources/gc/gcSND_ARAM.h"
#include <dolphin/vm.h>
#endif

#if defined( _XBOX ) || defined (_XENON)
#  include <XbDm.h>
#endif


/////////////////////////////////////////////////////////////////////////////////////
//                          MEMORY MANAGEMENT DESCRIPTION                          //
/////////////////////////////////////////////////////////////////////////////////////

// The memory management is as follows :
// A block is a chunk of RAM. It can be free or allocated. 
// The heap is a linear juxtaposition of free and allocated blocks. 
// A free block can only have allocated neighbours (or no neighbours for the first and last blocks).
// In the RAM space, the terms "block before" and "block after" are used for the block neighbours 
// whose address immediately precede and follow the address of the current block. 
// Free blocks are organized in double chained lists of the same size ;
// the prev and next pointers are stored in the block.
// Thus, the previous block and the block before have nothing in common 
// (idem for block after and next block).
// An element is a struct that points to a list of free blocks of the same size.
// Elements are organized in double chained lists, sorted by element size.
// The power array is an array that points to lists of elements.
// At index N of power array, we find a list of elements whose size vary from 16 * 2^N to 16 * (2^(N+1))-1.
// So, if we need to look for a free block of size s, we compute the index in the power array, 
// then we parse the element list for an element of size bigger or equal to s, 
// and then we take a free block from this element.
// If the element is empty, we remove it.
// If the block is bigger than s, we make a smaller block with the remaining memory.
// If we need to free an allocated block, we look the blocks before and after the block :
// if they are free blocks, we merge them with the freed block to make a bigger block. 

// When we need a new element, we take it from a static pool (MEM_ast_ElementPool), 
// so that we don't need to allocate memory for elements.

// Block management : 
// Each allocated block starts with a long (4 bytes) with the following data :
// 1 bit : is it a free block ?
// 1 bit : is block before free ?
// 1 bit : has block no spy key ?
// If it has a spy key (allocated blocks of size under 4 Mo) : 9 bits for the index in the spy table + 20 bits for the size (with a shift of 2 because we can only allocate multiples of 4 bytes)
// For other blocks : 29 bits for the size (with a shift of 2 because we can only allocate multiples of 4 bytes)
// These 4 bytes are the only memory used by the memory manager for each allocated block.

// Each free block starts with the same 4 bytes than the allocated block.
// Then it has a pointer to the previous and next blocks in its double chain (takes 8 bytes).
// The last 4 bytes of the free block is its (begining) address. 
// It is used when the block after is an allocated block 
// that wants to know the address of its block before (if this block is free).

// All in all a free block must be at least 16 bytes long. 
// Thus all blocs (free or allocated) must be at least 16 bytes long, 
// and their length must be multiples of 4 bytes.

// This management permits to rapidly find, remove or add a free block. 
// The defragmentation occurs naturally while data is freed (no extra defragmentation code is needed).

// Alloc from end : 
// It is possible to allocate data from the end of the heap. 
// This kind of allocation is used for fast temporary allocations, for example during the loading of a map.
// All allocations from end MUST be freed at the end of the temporary operation.
// The management is as follows : 
// We have a pointer to the end of the last free block. 
// When we allocate from end, we simply move this pointer back for the allocated size.
// When we free from end, if we free a block in the middle of other blocks from end, 
// we just mark it as freed : it cannot be reused immediatly. 
// If we free the last block that has been allocated, we move the pointer forward 
// until we find a block that has not been marked as freed.
// Thus, when all blocks that have been allocated from end have been freed, 
// the pointer is at the end of the heap.

// The header of a block allocated from end is as follows : 
// 1 bit that tells is the block has been freed.
// 1 bit with 1 (the block before is free). This bit is written by the normal allocation.
// 30 bits for the size. Must be a multiple of 4, at least 8.



/////////////////////////////////////////////////////////////////////////////////////
//                          USER INSTRUCTIONS                                      //
/////////////////////////////////////////////////////////////////////////////////////

// You can change the memory management behavior by changing the following #defines 

#if (defined _GAMECUBE) || (defined WIN32)
#define MEM_CRITICAL // allocs and frees should not be interrupted.
#endif

// These defines are used to change the memory management behavior.

// Choose the free block of the smallest size AND smallest address (slower but better for fragmentation).
//#define MEM_BEST_FIT

// Don't use virtual memory
//#define MEM_DONT_USE_VIRTUAL_MEMORY

// To check all the memory every time that a memory function is called (very slow)
//#define MEM_FULLCHECK

// To track leaks with history data (output in debugger window). Track reallocations.
#define MEM_TRACK_LEAKS_HISTORY

#ifdef MEM_SPY
// Permet de détecter les memory leaks in-game (à chaque changement de map).
#define MEM_TRACK_LEAKS
#endif // MEM_SPY

#ifdef _DEBUG

#ifdef JADEFUSION
//#define MEM_USE_HISTORY
#else
#define MEM_USE_HISTORY
#endif
// Fill the free blocks with a specific character in order to have reproductible bugs
#define MEM_FILL_FREE_BLOCKS
#define MEM_FREE_FILL_DATA 0xFAFAFAFA

// Fill the allocated blocks with a specific character in order to have reproductible bugs
#define MEM_FILL_ALLOCATED_BLOCKS
#define MEM_ALLOC_FILL_DATA 0xECECECEC

#ifdef MEM_CRITICAL
// Check that mem management is inside critical blocks (cannot be interrupted).
//#define MEM_CHECK_CRITICAL
#endif //_GAMECUBE

// Put markers at the begin and the end of allocated blocks, so that if the program writes 
// outside of its allocated blocks, it can be detected.
// Markers artificially increase measured allocated size : don't use them if you measure allocation size.

#define MEM_OVERRUN_MARKER 
#define MEM_OVERRUN_MARKER_BEGIN 0xDEADC0DE
#define MEM_OVERRUN_MARKER_END 0xC0DEDEAD

#endif // _DEBUG

#ifdef MEM_OVERRUN_MARKER
#define MEM_ASSERT_USER_ADDRESS(address) ERR_X_Assert( *((ULONG *)(address) - 1) == MEM_OVERRUN_MARKER_BEGIN )
#define MEM_ASSERT_BLOCK_ADDRESS(address) ERR_X_Assert( *((ULONG *)(address) + 1) == MEM_OVERRUN_MARKER_BEGIN )
#else // MEM_OVERRUN_MARKER
#define MEM_ASSERT_USER_ADDRESS(address) 
#define MEM_ASSERT_BLOCK_ADDRESS(address) 
#endif // MEM_OVERRUN_MARKER

#ifdef MEM_CHECK_CRITICAL
#define MEM_ASSERT_CRITICAL() ERR_X_Assert(MEM_bInCriticalSection || MEM_bInVMCriticalSection)
#else // MEM_CHECK_CRITICAL
#define MEM_ASSERT_CRITICAL() 
#endif// MEM_CHECK_CRITICAL

/*$4
 ***********************************************************************************************************************
    MACROS
 ***********************************************************************************************************************
 */
 
//#define MEM_Mb_IsAllocPointer(p) (((p) >= _pMem->pv_DynamicBloc) && ((p) < _pMem->pv_DynamicNextFree))

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Multithread access
 -----------------------------------------------------------------------------------------------------------------------
 */
#ifdef MEM_CRITICAL // Gamecube or win32
static BOOL MEM_bInCriticalSection = 0;

# ifdef _GAMECUBE
// BEGIN Gamecube
static BOOL sh_MultiThreadingLock = 1;
static BOOL MEM_bLogPageFaults = 1;
static BOOL MEM_bInVMCriticalSection = 0;
#define MEM_M_InitLock()

//OSMutex gcMEMMutex; provoque des bugs...

void MEM_M_BeginCriticalSection(MEM_tdst_MainStruct *_pMem)
{
#ifndef GC2RVL_NO_ARAM
	if ((_pMem)->bDisableInterrupts)
	{
		sh_MultiThreadingLock = OSDisableInterrupts(); 
    	ERR_X_Assert(!MEM_bInCriticalSection);
		MEM_bInCriticalSection = 1;
	}
	else
	{
    	ERR_X_Assert(!MEM_bInVMCriticalSection);
		MEM_bInVMCriticalSection = 1;
		MEM_bLogPageFaults = 0;
	}
//	OSLockMutex(&gcMEMMutex);
#endif // GC2RVL_NO_ARAM
}

void MEM_M_EndCriticalSection(MEM_tdst_MainStruct *_pMem)
{
#ifndef GC2RVL_NO_ARAM
//	OSUnlockMutex(&gcMEMMutex);
	if ((_pMem)->bDisableInterrupts) 
	{
		ERR_X_Assert(MEM_bInCriticalSection == 1);
		MEM_bInCriticalSection = 0;
		OSRestoreInterrupts(sh_MultiThreadingLock); 
	}
	else
	{
		ERR_X_Assert(MEM_bInVMCriticalSection == 1);
		MEM_bInVMCriticalSection = 0;
		MEM_bLogPageFaults = 1;
	}
#endif // GC2RVL_NO_ARAM	
} 

BOOL MEM_bIsInCriticalSection(MEM_tdst_MainStruct *_pMem)
{
    return ((_pMem)->bDisableInterrupts ? MEM_bInCriticalSection : MEM_bInVMCriticalSection);
}

#define MEM_M_DestroyLock()
// END Gamecube

# else //  _GAMECUBE

// BEGIN win32 
#define MEM_bIsInCriticalSection(_pMem) MEM_bInCriticalSection
static HANDLE	sh_MultiThreadingLock;
#define MEM_M_InitLock() \
	do \
	{ \
		sh_MultiThreadingLock = (HANDLE) 0; \
		sh_MultiThreadingLock = CreateEvent(NULL, FALSE, FALSE, NULL); \
		ERR_X_Warning(sh_MultiThreadingLock,"Cannot create event in MEM_M_InitLock()",NULL); \
		SetEvent(sh_MultiThreadingLock); \
	} while(0)
#define MEM_M_BeginCriticalSection(_pMem) \
	do \
	{ \
        ERR_X_Warning(MEM_bInCriticalSection == 0,"Entering twice in critical section",NULL);\
        MEM_bInCriticalSection = 1;\
		WaitForSingleObject(sh_MultiThreadingLock, INFINITE); \
	} while(0)
#define MEM_M_EndCriticalSection(_pMem) \
	do \
	{ \
        ERR_X_Warning(MEM_bInCriticalSection == 1, "Exiting twice critical section", NULL);\
        MEM_bInCriticalSection = 0;\
		SetEvent(sh_MultiThreadingLock); \
	} while(0)
#define MEM_M_DestroyLock() \
	do \
	{ \
		CloseHandle(sh_MultiThreadingLock); \
		sh_MultiThreadingLock = (HANDLE) 0; \
	} while(0)
#endif // 

// END win32 

#elif defined(_XENON_RENDER)

static CRITICAL_SECTION s_oMemoryAccessLock;

#define MEM_M_InitLock()                                    \
    do                                                      \
{                                                       \
    InitializeCriticalSection(&s_oMemoryAccessLock);    \
} while (0)

#define MEM_M_BeginCriticalSection(_pMem)                        \
    do                                                      \
{                                                       \
    EnterCriticalSection(&s_oMemoryAccessLock);         \
} while (0)

#define MEM_M_EndCriticalSection(_pMem)                          \
    do                                                      \
{                                                       \
    LeaveCriticalSection(&s_oMemoryAccessLock);         \
} while (0)

#define MEM_M_DestroyLock()                                 \
    do                                                      \
{                                                       \
    DeleteCriticalSection(&s_oMemoryAccessLock);        \
} while (0)

#else //MEM_CRITICAL

// BEGIN PS2 & Xbox 

#define MEM_bIsInCriticalSection(_pMem) 0
#define MEM_M_InitLock()
#define MEM_M_BeginCriticalSection(_pMem)
#define MEM_M_EndCriticalSection(_pMem)
#define MEM_M_DestroyLock()
// END PS2 & Xbox 
#endif // MEM_CRITICAL

#ifndef _FINAL_
// A measure of the peak memory usage
int					MEM_gi_MinDelta = 0x7FFFFFFF;
int					MEM_gi_Delta = 0x7FFFFFFF;
#endif // _FINAL_



/*$2
 -----------------------------------------------------------------------------------------------------------------------
    signal fatal error
 -----------------------------------------------------------------------------------------------------------------------
 */

void MEM_vBuildSummary();

#if defined(_FINAL_)

/*$1- final = no signal ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define MEM_M_SignalFatalError(a, b)

/*$1- PSX2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#elif defined(PSX2_TARGET)
extern void 						Gsp_Crash(unsigned int Address, unsigned char *);
extern void							Gsp_CrashPrintf(char *);
extern void 						Gsp_CrashPrintf2(char *str,u32 Color);
extern void							Gsp_FlipFB(void);

void MEM_M_SignalFatalError(u32 a, char *b) 
{
	do 
	{ 
		int		tmp; 
		char	az[256]; 
		asm("qmove tmp, ra"); 
		sprintf(az, "alloc size %d, from 0x%X", a, tmp); 
		printf("*** No more memory : %s ***\n", az);
		Gsp_Crash(0, b); 
		Gsp_CrashPrintf(b); 
		Gsp_CrashPrintf(az); 
#ifdef MEM_SPY
		{
			u32 CurrentNum,Summ;
			CurrentNum = MEM_ulSummaryNb;
			MEM_vBuildSummary();
			if (MEM_ulSummaryNb != -1)
			{
				char string_MO[1024];
				char string[1024];
				Summ  = 0;
				for (CurrentNum = 0 ; CurrentNum < MEM_ulSummaryNb ; CurrentNum++) Summ += MEM_SortSummaryTable[CurrentNum]->ulAllocSize;
				sprintf(string_MO,"%2d.%03dMo ", Summ >> 20 , (((Summ >> 10) & 1023) * 999) / 1024 );
				sprintf(string,"TOTAL:%s", string_MO );
				Gsp_CrashPrintf(string);

				for (CurrentNum = 0 ; CurrentNum < MEM_ulSummaryNb ; CurrentNum++)
				{
					if (MEM_SortSummaryTable[CurrentNum]->ulAllocSize > 128 * 1024)
					{
						u32 color ;
						sprintf(string_MO,"%2d.%03dMo ", MEM_SortSummaryTable[CurrentNum]->ulAllocSize >> 20 , (((MEM_SortSummaryTable[CurrentNum]->ulAllocSize >> 10) & 1023) * 999) / 1024 );
						sprintf(string,"%s:%s", MEM_SortSummaryTable[CurrentNum]->sHeader , string_MO );
						color  = (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 4)) << (24 - 2 - 4);
						color |= (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 2)) << (16 - 2 - 2);
						color |= (MEM_SortSummaryTable[CurrentNum]->ulKey & (0x3 << 0)) << (8 - 2);
						color |= 0x3f3f3f;
						
						color &= 0xfefefefe;
						color += 0xfefefefe;
						color >>= 1;
						

						
						Gsp_CrashPrintf2(string,color);
					}
				}
			}
		}
#endif
		Gsp_FlipFB(); 
		while(1) 
		{ 
		}; 
	} while(0);
}

/*$1- Gamecube ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#elif defined(_GAMECUBE)

void MEM_M_SignalFatalError(u32 a, char *b) 
{
#ifdef MEM_SPY
	MEM_vBuildSummary();
#endif // MEM_SPY
	while (1)
	{ 
		char	az[256]; 
		int i;
		
		GXI_ErrBegin(); 
		sprintf(az, "%s : %d oct\n",b , a); 
		GXI_ErrPrint(az); 
		
#ifdef MEM_SPY
		for (i=0; i<MEM_ulSummaryNb; i++)
		{
			MEM_stSummaryElement *pElement = MEM_SortSummaryTable[i];
			sprintf(az, "%s : %d oct\n", pElement->sHeader, pElement->ulAllocSize); 
			GXI_ErrPrint(az); 
		}
#endif // MEM_SPY
		GXI_ErrEnd(); 
	};
}


/*$1- others (win32+Xbox) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#elif defined(_XBOX) || defined(_XENON)
#define MEM_M_SignalFatalError(a, b)	ERR_X_Assert(0)

/*$1- others (win32+Xbox) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#else
#define MEM_M_SignalFatalError(a, b)	MessageBox(0, "No more memory", "No more memory", MB_OK)
#endif


#define PRELOADTEXGRAN	(40 * 1024)
extern void PreLoadTexReset(void);
extern char	*gp_LastPointer;		/* Pointer from end of block to free zone */
extern void PreloadCancel(void);


// Platform depend print
#ifdef _GAMECUBE
#define _print	OSReport
#elif defined(PSX2_TARGET)
#define _print	printf
#elif defined(WIN32)

void _print(const char *_sMsg, ...)
{
	char	sz_Log[1024];
	va_list args;
 
	va_start( args, _sMsg );

	vsprintf( sz_Log, _sMsg, args );
	OutputDebugString(sz_Log);
}


#else
FILE	*gfh = NULL;

void _print(const char *_sMsg, ...)
{
	if(!gfh) 
	{ 
		gfh = fopen("memdyn.log", "w+"); 
	} 
	if(gfh) 
	{ 
		fprintf(gfh, _sMsg); 		
   } 
}
#endif


#ifdef MEM_MEASURES
void MEM_UPDATE_MEASURES(MEM_tdst_MainStruct *_pMem)
{
	ULONG ulCurrentLastFree = MEM_uGetLastBlockSize(_pMem);
	_pMem->ul_CurrentFree = MEM_uGetFreeMemSize(_pMem);
	
	if (_pMem->ul_MinFreeTotal > _pMem->ul_CurrentFree) 
		_pMem->ul_MinFreeTotal = _pMem->ul_CurrentFree;
		
	if (ulCurrentLastFree < _pMem->ul_MinLastFree)
		_pMem->ul_MinLastFree = ulCurrentLastFree;
}
#else // MEM_MEASURES
#define MEM_UPDATE_MEASURES(_pMem)
#endif // MEM_MEASURES


////////////////////////////////////////
// VALUES 
////////////////////////////////////////

#define MEM_HEADER_SIZE 4
#ifdef MEM_OVERRUN_MARKER
# define MEM_OVERRUN_MARKER_SIZE 4
#else // MEM_OVERRUN_MARKER
# define MEM_OVERRUN_MARKER_SIZE 0
#endif // MEM_OVERRUN_MARKER
# define MEM_TOTAL_HEADER_SIZE (MEM_HEADER_SIZE + MEM_OVERRUN_MARKER_SIZE)
# define MEM_LOST_SIZE (MEM_HEADER_SIZE + (MEM_OVERRUN_MARKER_SIZE*2))

// Min and max size. Min size is for free blocks, which are not concerned by markers.
#define MEM_MIN_SIZE 16

#ifdef _RVL
#define MEM_MAX_SIZE 	0x04000000
#elif defined(_GAMECUBE)
#define MEM_MAX_SIZE 	0x03000000
#elif defined(_XENON)
#define MEM_MAX_SIZE 	0x08000000
#else
#define MEM_MAX_SIZE 	0x04000000
#endif

#ifndef _RVL
// Buffer in RAM used to put data when needed (page cache), for VRAM manager
#ifdef JADEFUSION
ULONG MEM_VM_RAM_Size = (3000*1024);
#else
ULONG MEM_VM_RAM_Size = (2500*1024);
#endif
// Buffer in ARAM containing all the data (size of the virtual heap). 
// Equal value in g_u32ARAM_BlocksSize.
#ifdef ANIMS_USE_ARAM
ULONG MEM_VM_ARAM_Size = ((6*1024-16)*1024); // 16ko for system use
#else // ANIMS_USE_ARAM
ULONG MEM_VM_ARAM_Size = ((11*1024-16)*1024); // 16ko for system use
#endif // ANIMS_USE_ARAM


// Virtual Heap (used to manage the virtual RAM : cached in RAM but really in ARAM
// Start of the virtual heap.
#define MEM_VM_VirtualStartAddress 0x7E000000
//ULONG MEM_VM_VirtualStartAddress = 0x7E000000;
// End of the virtual heap.
#define MEM_VM_VirtualEndAddress (MEM_VM_VirtualStartAddress + MEM_VM_RAM_Size)

// Sound buffer (used for system sound allocs which would otherwise fragment the RAM).
// Size of sound buffer.
#define MEM_Sound_RAM_Size (650*1024)

// Start and end of sound heap.
static ULONG MEM_SoundStartAddress = 0;
static ULONG MEM_SoundEndAddress = 0;

// Size of Gamecube buffer.
// Load mode : load buffers + bink + small frame buffers

// Play mode :
//  after fx  + zlist + compute buffer + big frame buffers //+ SPG2(200 lines)

// For 750ko binks, we need 4,3Mo -> too much for in-game.
// With 3330000 bytes, we have enough for in-game buffers (except SPG2)
// Load buffers must manage with that...
#define MEM_GC_RAM_Size 4000000 //3330000 

// Start and end of sound heap.
static ULONG MEM_GCStartAddress = 0;
static ULONG MEM_GCEndAddress = 0;
#endif // _RVL

#ifdef MEM_TRACK_LEAKS_HISTORY
static BOOL MEM_gb_bTrackLeaks = TRUE;
#else // MEM_TRACK_LEAKS_HISTORY
static BOOL MEM_gb_bTrackLeaks = FALSE;
#endif // MEM_TRACK_LEAKS_HISTORY

////////////////////////////////////////
// MACROS 
////////////////////////////////////////

#define GetHeapStart(p) (p)->pv_DynamicBloc
#define GetHeapEnd(p) (p)->pvMemoryHeapEnd
#define IsAddrInHeap(p,a) (((a)>=GetHeapStart((p))) && ((a)<=GetHeapEnd((p))))

// What kind of address is it ?
#ifdef _GAMECUBE

#define MEM_bIS_VMAddress(pAddress)	   IsAddrInHeap(&MEM_gst_VirtualMemoryInfo, pAddress)
#define MEM_bIS_SoundAddress(pAddress) IsAddrInHeap(&MEM_gst_SoundMemoryInfo, pAddress)
#define MEM_bIS_GCAddress(pAddress)    IsAddrInHeap(&MEM_gst_GCMemoryInfo, pAddress)

// if (MEM_bIS_SpyAddress == 0), don't spy on this address
#define MEM_bIS_SpyAddress(pAddress)  (!MEM_bIS_GCAddress(pAddress) && !MEM_bIS_SoundAddress(pAddress))

#else //_GAMECUBE

#define MEM_bIS_VMAddress(pAddress)	0
#define MEM_bIS_SpyAddress(pAddress) 1

#endif //_GAMECUBE

#ifdef JADEFUSION
extern s32 GetExpOfUpperPowerOf2(f32 Value);
#endif

// Check that the size is valid
#ifndef _RVL
#define MEM_CHECK_SIZE(size,pAddress) \
	ERR_X_Assert((size) >= MEM_MIN_SIZE && \
	(size) <= (MEM_bIS_VMAddress(pAddress) ? MEM_VM_ARAM_Size : MEM_MAX_SIZE ))
#else
#define MEM_CHECK_SIZE(size,pAddress)
#endif // _RVL

// Get the index in MEM_ap_PowerArray that corresponds to Size.
#define MEM_iGetIndexInPowerArray(Size) (GetExpOfUpperPowerOf2((f32)(((Size)>>4)+1))-1)

// Get address of block, using user data address (remove header from address)
#define MEM_pGetBlockAddress(user_address) (((unsigned char *)(user_address))-MEM_TOTAL_HEADER_SIZE)

// Get address of user data, using the block address (add header from address)
#define MEM_pGetUserAddress(block_address) (((unsigned char *)(block_address))+MEM_TOTAL_HEADER_SIZE)

// Compute block size needed to put user data of given size (header + data = multiple of 4, at least 16).
#define MEM_iComputeNeededBlockSize(user_size) lMax(MEM_MIN_SIZE,(((user_size) + MEM_LOST_SIZE + 3) >> 2) << 2)

// Inform block after that its block before is a free block.
#define MEM_vBlockBeforeOfBlockAfterIsFree(block,size) \
    (*((ULONG *) (((unsigned char *)(block)) + (size)))) |= 0x40000000

// Inform block after that its block before is an allocated block
#define MEM_vBlockBeforeOfBlockAfterIsAllocated(block,size) \
    (*((ULONG *) (((unsigned char *)(block)) + (size)))) &= 0xBFFFFFFF

// Is the block free ?
#define MEM_bIsBlockFree(block) ((*((ULONG *)(block))) & 0x80000000)

// Set block as free.
#define MEM_bSetBlockAsFree(block) ((*((ULONG *)(block))) |= 0x80000000)

#define MEM_bHasNoSpyKey(BlockAddress) (*((ULONG *)(BlockAddress)) & 0x20000000)

// Get size from block
#define MEM_iGetNoSpyKeyBlockSize(block)  (((*((ULONG *)(block))) & 0x1FFFFFFF) << 2)
#define MEM_iGetSpyKeyBlockSize(block) (((*((ULONG *)(block))) & 0x000FFFFF) << 2)
#define MEM_iGetBlockSize(block) (MEM_bHasNoSpyKey(block) ? MEM_iGetNoSpyKeyBlockSize(block) : MEM_iGetSpyKeyBlockSize(block))

// Get the block after this block
#define MEM_pGetBlockAfter(block) ( (unsigned char *)(block) + MEM_iGetBlockSize(block))

// Get previous block in free block chained list (only for free blocks)
#define MEM_pGetPrevBlock(block) (*(((ULONG *)(block))+1))

// Get next block in free block chained list (only for free blocks)
#define MEM_pGetNextBlock(block) (*(((ULONG *)(block))+2))

// Is block before this block free ? (only for allocated blocks)
#define MEM_bIsBlockBeforeFree(block) ((*((ULONG *)(block))) & 0x40000000)

// Get block before this block (only for allocated blocks)
#define MEM_pGetBlockBefore(block) (*(((ULONG *)(block))-1))

// Get correct memory manager with address 
#ifdef _GAMECUBE
/*#define MEM_pChooseMemFromAddress(pAddress) \
	(MEM_bIS_VMAddress(pAddress) ? &MEM_gst_VirtualMemoryInfo : \
	(MEM_bIS_SoundAddress(pAddress) ? &MEM_gst_SoundMemoryInfo : \
	(MEM_bIS_GCAddress(pAddress) ? &MEM_gst_GCMemoryInfo : &MEM_gst_MemoryInfo)))*/

#ifndef _RVL	 
#define MEM_bIsVirtual(_pMem) ((_pMem) == &MEM_gst_VirtualMemoryInfo)
#else
#define MEM_bIsVirtual(_pMem) 0
#endif // _RVL

#else //_GAMECUBE
#define MEM_pChooseMemFromAddress(pAddress) (&MEM_gst_MemoryInfo)
#define MEM_bIsVirtual(_pMem) 0
#endif // _GAMECUBE

// Get all allocated mem
#ifdef _GAMECUBE
#define MEM_ulGetTotalAllocatedMem() (MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated + MEM_gst_VirtualMemoryInfo.ul_DynamicCurrentAllocated)
#else // _GAMECUBE
#define MEM_ulGetTotalAllocatedMem() (MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated)
#endif // _GAMECUBE

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    history
 -----------------------------------------------------------------------------------------------------------------------
 */


#ifdef MEM_SPY
static u32 MEM_SPY_LOCKER = 0;
BOOL MEM_bDontTrackLeaks = FALSE;

MEM_stElement MEM_SpyTable[MEM_TableSize];
ULONG MEM_ulElementNb;


void MEM_vInitSpy()
{
    MEM_ulElementNb = 0;
    L_memset(MEM_SpyTable,0,sizeof(MEM_stElement) * MEM_TableSize);
}

ULONG MEM_ulComputeSpyKey(char *_sFile, int _iLine)
{
    ULONG ulKey = 0;
    while ((_sFile[0] != '\0') && 
        (_sFile[1] != '\0') &&
        (_sFile[2] != '\0') &&
        (_sFile[3] != '\0'))
    {
        ulKey += *((ULONG *)_sFile);
        _sFile += 4;
    }
    ulKey += _iLine;

    return ulKey % MEM_TableSize;
}

BOOL MEM_bIsElement(char *_sFile, int _iLine, ULONG _ulKey )
{
    MEM_stElement *pElement = MEM_SpyTable + _ulKey;

    if (!pElement->sFile)
    {
        // Create new element
        MEM_ulElementNb++;
        pElement->iLine = _iLine;
        pElement->sFile = _sFile;
        return TRUE;
    }
    else
    {
        return ((0 == L_strcmp(_sFile,pElement->sFile)) && (_iLine == pElement->iLine));
    }
}

char *MEM_FormatString(char *Letter)
{
	char *Base;
	Base = Letter;
	while (*Letter) {Letter++; };

	while ((Letter > Base) && (*Letter != '\\')) {Letter--;};
	if (*Letter == '\\') Letter ++;
	return Letter ;
}

ULONG MEM_ulGenerateKey(char *_sFile, int _iLine)
{
	ULONG ulKey ;
	
	_sFile = MEM_FormatString(_sFile);
    ulKey = MEM_ulComputeSpyKey(_sFile, _iLine);
    ERR_X_Assert(MEM_ulElementNb < MEM_TableSize-1);


    while (!MEM_bIsElement(_sFile, _iLine, ulKey))
        ulKey = (ulKey + 1) % MEM_TableSize;

    return ulKey;
}

void MEM_vAddSpyAlloc(char *_sFile, int _iLine, void * _pBlockAddress)
{
	if (MEM_SPY_LOCKER || !MEM_bIS_SpyAddress(_pBlockAddress))	
		return;
	else
	{
		ULONG ulKey = MEM_ulGenerateKey(_sFile, _iLine);
	    MEM_stElement *pElement = MEM_SpyTable + ulKey;
	    ULONG ulHeader = *((ULONG *)_pBlockAddress); 

        if (ulHeader & 0x1FF00000 || MEM_bDontTrackLeaks)
        {
            // We don't want to track leak or size is too big : 
            // put NoSpyKey bit (0x20000000) to 1 and return.
            *((ULONG *)_pBlockAddress) = *((ULONG *)_pBlockAddress) | 0x20000000;
            return;
        }

	    ERR_X_Assert(!(ulHeader & 0x1FF00000)); // Is key == 0 ?
	    
	    pElement->iAllocSize += MEM_iGetBlockSize(_pBlockAddress);
	    if (pElement->iAllocSize > pElement->iAllocSizeMax)
			pElement->iAllocSizeMax = pElement->iAllocSize;

	    pElement->iAllocNb ++ ; 
	    if (pElement->iAllocNb > pElement->iAllocNbMax)
	        pElement->iAllocNbMax = pElement->iAllocNb;

	    // Set key in alloc header
	    *((ULONG *)_pBlockAddress) = (ulHeader & 0xE00FFFFF) | (ulKey << 20);//*/
	}
}

// Get bits from 2 to 2 + 9.
#define MEM_ulGetSpyKey(BlockAddress) (((*((ULONG *)(BlockAddress))) & 0x1FF00000) >> 20)


void MEM_vRemoveSpyAlloc(void * _pBlockAddress)
{
	if (MEM_SPY_LOCKER || MEM_bHasNoSpyKey(_pBlockAddress)  || !MEM_bIS_SpyAddress(_pBlockAddress))	
		return;
	else
	{
	    ULONG ulKey = MEM_ulGetSpyKey(_pBlockAddress);
	    ULONG ulSize = MEM_iGetBlockSize(_pBlockAddress);
	    MEM_stElement *pElement = MEM_SpyTable + ulKey;
	    ERR_X_Assert(pElement->iLine && pElement->iAllocNb);
	    pElement->iAllocSize -= ulSize;
	    pElement->iAllocNb-- ;//*/
	}
}

static MEM_stSummaryElement MEM_SummaryTable[MEM_ulSummaryTableSize];
MEM_stSummaryElement *MEM_SortSummaryTable[MEM_ulSummaryTableSize];
ULONG MEM_ulSummaryNb = 0;

#define MEM_ulComputeSummaryKey(_sFile) (((_sFile)[0] + (_sFile)[1] + (_sFile)[2]) % MEM_ulSummaryTableSize)


BOOL MEM_bIsSummaryElement(char *_sFile, ULONG _ulKey )
{
    MEM_stSummaryElement *pElement = MEM_SummaryTable + _ulKey;

    if (!pElement->bIsUsed)
    {
        // Create new element
        MEM_ulSummaryNb++;
        pElement->sHeader[0] = _sFile[0];
        pElement->sHeader[1] = _sFile[1];
	    pElement->sHeader[2] = _sFile[2];
    	pElement->sHeader[3] = '\0';
    	pElement->ulKey = _ulKey;
        return TRUE;
    }
    else
        return (pElement->sHeader[0] == _sFile[0]) && (pElement->sHeader[1] == _sFile[1]) && (pElement->sHeader[2] == _sFile[2]);
}

int MEM_iSummaryCompare( const void *arg1, const void *arg2 )
{
    const MEM_stSummaryElement *pElement1 = *((const MEM_stSummaryElement **)arg1);
    const MEM_stSummaryElement *pElement2 = *((const MEM_stSummaryElement **)arg2);
    
    if (! (pElement1 && pElement2))
    	return pElement1 - pElement2;
    else
    {
	    int iDiffSize = ((int)pElement2->ulAllocSize) - ((int)pElement1->ulAllocSize);
	    if (iDiffSize)
	        return iDiffSize;
	    else
	        return L_strcmp(pElement1->sHeader, pElement2->sHeader);
    }
}

int MEM_iTableCompare( const void *arg1, const void *arg2 )
{
    const MEM_stElement *pElement1 = *((const MEM_stElement **)arg1);
    const MEM_stElement *pElement2 = *((const MEM_stElement **)arg2);
    
    if (! (pElement1 && pElement2))
    {
	    return pElement1 - pElement2;
	}
    else
    {
	    int iDiffSize = ((int)pElement2->iAllocSize) - ((int)pElement1->iAllocSize);
	    if (iDiffSize)
	        return iDiffSize;
	    else if (pElement1->iLine != pElement2->iLine)
	    	return pElement1->iLine - pElement2->iLine;
	    else
	        return L_strcmp(pElement1->sFile, pElement2->sFile);
    }
}


// Parse spy table, add elements to summary and sort with size.
void MEM_vBuildSummary()
{
    int i,iSortSize;

    // Empty summary table
    L_memset(MEM_SummaryTable,0,MEM_ulSummaryTableSize * sizeof(MEM_stSummaryElement));
    MEM_ulSummaryNb = 0;

    // Fill summary table
    for (i=0; i<MEM_TableSize; i++)
    {
        MEM_stElement *pElement = MEM_SpyTable + i;
        if (pElement->sFile)
        {
            MEM_stSummaryElement *pSummaryElement;
            ULONG ulSummaryKey = MEM_ulComputeSummaryKey(pElement->sFile);
            while (!MEM_bIsSummaryElement(pElement->sFile, ulSummaryKey))
                ulSummaryKey = (ulSummaryKey + 1) % MEM_ulSummaryTableSize;

            pSummaryElement = MEM_SummaryTable + ulSummaryKey;
            pSummaryElement->ulAllocSize += pElement->iAllocSize;
            pSummaryElement->ulAllocNb += pElement->iAllocNb;
        }
    }

    // Sort summary table
    iSortSize = 0;
    for (i=0; i<MEM_ulSummaryTableSize; i++)
    {
        MEM_stSummaryElement *pSummaryElement = MEM_SummaryTable + i;
        if (pSummaryElement->bIsUsed)
        {
            MEM_SortSummaryTable[iSortSize] = pSummaryElement;
            iSortSize++;
        }
    }
    qsort(MEM_SortSummaryTable,iSortSize,sizeof(MEM_stSummaryElement *),&MEM_iSummaryCompare);
}

void MEM_PrintSpySummary(MEM_tdst_MainStruct *_pMem)
{
	int i;
	
	MEM_vBuildSummary();
	
	for (i=0; i<MEM_ulSummaryTableSize; i++)
	{
        MEM_stSummaryElement *pSummaryElement = MEM_SummaryTable + i;
		_print("%s\t%d\t%d\n",pSummaryElement->sHeader,pSummaryElement->ulAllocSize,pSummaryElement->ulAllocNb);
	}
}

void MEM_PrintSpyDetails(MEM_tdst_MainStruct *_pMem)
{
	void *pBlockAddress;
	
#ifdef MEM_CRITICAL
	ULONG bInCriticalSection = MEM_bIsInCriticalSection(_pMem);
#endif // MEM_CRITICAL	
	
	// Check the heap from begin to end.
	pBlockAddress = _pMem->pv_DynamicBloc;
	
	
#ifdef MEM_CRITICAL
	if (!bInCriticalSection)
		MEM_M_BeginCriticalSection(_pMem);
#endif // MEM_CRITICAL	
	
	while (pBlockAddress < _pMem->pvMemoryHeapEnd)
	{
		ULONG uSize = MEM_iGetBlockSize(pBlockAddress); 
		
		if (!MEM_bIsBlockFree(pBlockAddress))
		{
		    ULONG ulKey = MEM_ulGetSpyKey(pBlockAddress);
		    MEM_stElement *pElement = MEM_SpyTable + ulKey;
		    ERR_X_Assert(pElement->iLine && pElement->iAllocNb);
			_print("%s(%d)\tsize\t%d\t%d\tnb\t%d\t%d\n",
				pElement->sFile,pElement->iLine,
				pElement->iAllocSize,pElement->iAllocSizeMax,
				pElement->iAllocNb,pElement->iAllocNbMax);
		}
#ifdef JADEFUSION	
		pBlockAddress = (unsigned char *)pBlockAddress + uSize;
#else
		(unsigned char *)pBlockAddress += uSize;
#endif
	}
	
#ifdef MEM_CRITICAL
	if (!bInCriticalSection)
		MEM_M_EndCriticalSection(_pMem);
#endif // MEM_CRITICAL
}

void MEM_vLogHeapUsage(MEM_tdst_MainStruct *_pMem)
{
	// On parcourt le heap et on loggue qui utilise la RAM, à quel endroit.
	void *pBlockAddress;
	ULONG uFreeMem,uAllocatedMem,uFreeBlockNb1,uFreeBlockNb2,uAllocatedBlockNb;
	
#ifdef MEM_CRITICAL
	ULONG bInCriticalSection = MEM_bIsInCriticalSection(_pMem);
#endif // MEM_CRITICAL	
	
	// Check the heap from begin to end.
	pBlockAddress = _pMem->pv_DynamicBloc;
	
	uFreeMem = 0;
	uAllocatedMem = 0;
	uAllocatedBlockNb = 0;
	uFreeBlockNb1 = 0;
	uFreeBlockNb2 = 0;
	
#ifdef MEM_CRITICAL
	if (!bInCriticalSection)
		MEM_M_BeginCriticalSection(_pMem);
#endif // MEM_CRITICAL	
	
	_print("BeginAddres\tEnd Address\tSIZE\tUSER\n");
	while (pBlockAddress < _pMem->pvMemoryHeapEnd)
	{
		ULONG uSize;
		
		uSize = MEM_iGetBlockSize(pBlockAddress); 
		if (MEM_bIsBlockFree(pBlockAddress))
		{
			_print("0x%08x\t0x%08x\t0x%08x\tFREE BLOCK\n",
				(u32)pBlockAddress,
				(u32) ((char *) pBlockAddress + uSize),
				uSize);
		}
		else
		{
			// Get block in spy table.
		    ULONG ulKey = MEM_ulGetSpyKey(pBlockAddress);
		    MEM_stElement *pElement = MEM_SpyTable + ulKey;
		    
			_print("0x%08x\t0x%08x\t0x%08x\t%s(%d)\n",
				(u32)pBlockAddress,
				(u32) ((char *) pBlockAddress + uSize),
				uSize,
				pElement->sFile,
				pElement->iLine);
		}
#ifdef JADEFUSION	
        pBlockAddress = (unsigned char *)pBlockAddress + uSize;
#else
		(unsigned char *)pBlockAddress += uSize;
#endif
	}
	
#ifdef MEM_CRITICAL
	if (!bInCriticalSection)
		MEM_M_EndCriticalSection(_pMem);
#endif // MEM_CRITICAL
}


#ifdef MEM_TRACK_LEAKS
BOOL MEM_bStartLeakTracking = FALSE; // start leak tracking when a real map has been loaded
BOOL MEM_bDisplayLeaks = FALSE;
int MEM_iLoss = 0;
int MEM_iGain = 0;

MEM_stElement MEM_SaveSpyTable[MEM_TableSize];
MEM_stElement *MEM_SortedLeaks[MEM_TableSize];

// Save state of memory (snap shot) in MEM_SaveSpyTable
void MEM_vSaveSpySnapShot()
{
#ifdef MEM_CRITICAL
	ULONG bInCriticalSection = MEM_bIsInCriticalSection(&MEM_gst_MemoryInfo);
#ifdef _GAMECUBE
	ULONG bVMInCriticalSection = MEM_bIsInCriticalSection(&MEM_gst_VirtualMemoryInfo);
	if (!bVMInCriticalSection)
		MEM_M_BeginCriticalSection(&MEM_gst_VirtualMemoryInfo);
#endif //_GAMECUBE		
	if (!bInCriticalSection)
		MEM_M_BeginCriticalSection(&MEM_gst_MemoryInfo);
#endif //  MEM_CRITICAL
		
	L_memcpy(MEM_SaveSpyTable,MEM_SpyTable,sizeof(MEM_stElement)*MEM_TableSize);
	
#ifdef MEM_CRITICAL
	if (!bInCriticalSection)
		MEM_M_EndCriticalSection(&MEM_gst_MemoryInfo);
#ifdef _GAMECUBE
	if (!bVMInCriticalSection)
		MEM_M_EndCriticalSection(&MEM_gst_VirtualMemoryInfo);
#endif //_GAMECUBE		
#endif //  MEM_CRITICAL
}



// Compute difference between state of memory saved by MEM_vSaveSpySnapShot and present state of memory.
// Put result in MEM_SaveSpyTable.
void MEM_vComputeSpyDeltaSnapShot()
{
    int i;
	MEM_iLoss = 0; // Alloacted mem that was in MEM_vSaveSpySnapShot and is not in MEM_SpyTable
	MEM_iGain = 0; // Allocated mem that was not in MEM_vSaveSpySnapShot and is in MEM_SpyTable (leak)
	
#ifdef _GAMECUBE
	MEM_M_BeginCriticalSection(&MEM_gst_VirtualMemoryInfo);
#endif //_GAMECUBE	

	for (i=0; i<MEM_TableSize; i++)
	{
		MEM_stElement *pElementBefore = MEM_SaveSpyTable+i;
		MEM_stElement *pElementAfter = MEM_SpyTable+i;
		
		// Does element exist  ?
		if (pElementAfter->sFile && pElementAfter->iLine)
		{
			pElementBefore->sFile = pElementAfter->sFile;
			pElementBefore->iLine = pElementAfter->iLine;
			pElementBefore->iAllocSize = pElementAfter->iAllocSize - pElementBefore->iAllocSize;
			
			if (pElementBefore->iAllocSize > 0)
				MEM_iGain += pElementBefore->iAllocSize;
				
			if (pElementBefore->iAllocSize < 0)
				MEM_iLoss += -pElementBefore->iAllocSize;
				
			pElementBefore->iAllocSizeMax = -1;
			pElementBefore->iAllocNb = pElementAfter->iAllocNb - pElementBefore->iAllocNb;
			pElementBefore->iAllocNbMax = -1;
		}
		else
			ERR_X_Assert(!pElementBefore->sFile && !pElementBefore->iLine);
	}
	
	MEM_bDisplayLeaks = MEM_iLoss || MEM_iGain;
	
	if (MEM_bDisplayLeaks)
	{
		// Tri en fonction de la taille
		for (i=0; i<MEM_TableSize; i++)
		{
			if (MEM_SaveSpyTable[i].iAllocSize)
				MEM_SortedLeaks[i] = &(MEM_SaveSpyTable[i]);
			else
				MEM_SortedLeaks[i] = NULL;
		}	
		
	    qsort(MEM_SortedLeaks,MEM_TableSize,sizeof(MEM_stElement *),&MEM_iTableCompare);
	}

#ifdef _GAMECUBE
	MEM_M_EndCriticalSection(&MEM_gst_VirtualMemoryInfo);
	//MEM_TestVM();
#endif //_GAMECUBE		
}

void MEM_vLogLeaks()
{
	int i;
	
	_print("MEMORY LEAK : GAIN %d LOSS %d\n", MEM_iGain, MEM_iLoss);
	
	for (i=0; i<MEM_TableSize; i++)
	{
		MEM_stElement *pElement = MEM_SortedLeaks[i];
		if (pElement)
			_print("%s(%d) : %d\n",pElement->sFile, pElement->iLine, pElement->iAllocSize);
	}
}

void MEM_vDispLeaks()
{
#ifdef _GAMECUBE
	BOOL bIsTouched = gcINO_bIsJoystickTouched();
	BOOL bWasTouched = bIsTouched;
	
	while (bWasTouched || !bIsTouched)
	{ 
		int i;
    	char sMsg[512];
		
		GXI_ErrBegin(); 
		
		sprintf(sMsg,"PLEASE SEND THIS SCREEN TO CORRECTOR");
		GXI_ErrPrint(sMsg); 
		
		sprintf(sMsg,"PRESS START BUTTON TO EXIT");
		GXI_ErrPrint(sMsg); 
		
		sprintf(sMsg,"MEMORY LEAK : GAIN %d LOSS %d", MEM_iGain, MEM_iLoss);
		GXI_ErrPrint(sMsg); 
		
		for (i=0; i<16; i++)
		{
			MEM_stElement *pElement = MEM_SortedLeaks[i];
			if (pElement)
			{
				sprintf(sMsg,"%s(%d) : %d",pElement->sFile, pElement->iLine, pElement->iAllocSize);
				GXI_ErrPrint(sMsg); 
			}
			else
				break;
		}
		GXI_ErrEnd(); 
		bWasTouched = bIsTouched;
		bIsTouched = gcINO_bIsJoystickTouched();
	};
	
	MEM_bDisplayLeaks = FALSE;
#endif // _GAMECUBE
}

#endif // MEM_TRACK_LEAKS




#ifdef MEM_USE_HISTORY

#define MEM_Cul_AllocationHistorySize	200000

// Number of elements in history arrays
LONG		MEM_g_nbpointers = 0;

// Table used to store result of sort.
int 		MEM_gap_SortTable[MEM_Cul_AllocationHistorySize];

// Allocated addresses
LONG		MEM_gap_AllocationHistoryTable[MEM_Cul_AllocationHistorySize];

// End of allocated addresses
LONG		MEM_gap_AllocationHistoryTableEnd[MEM_Cul_AllocationHistorySize];

// File name and line numbers where the allocations have been made.
char		*MEM_gapsz_AllocatedInFile[MEM_Cul_AllocationHistorySize];
int			MEM_gai_AllocatedInLine[MEM_Cul_AllocationHistorySize];


// The allocation increment increases each time an allocation is made.
// It is used to know the allocation order.
int			MEM_gai_AllocationIncrement[MEM_Cul_AllocationHistorySize];
int 		MEM_giAllocationIncrement = 0;
int 		MEM_giAllocationIncrementBreakPoint = -1;

// Snapshot of memory
char		*MEM_gapsz_SnapShotAllocatedInFile[MEM_Cul_AllocationHistorySize];
int			MEM_gai_SnapShotAllocatedInLine[MEM_Cul_AllocationHistorySize];
int			MEM_gai_SnapShotSize[MEM_Cul_AllocationHistorySize];
int			MEM_gai_SnapShotAddress[MEM_Cul_AllocationHistorySize];
int			MEM_gai_SnapShotAllocationIncrement[MEM_Cul_AllocationHistorySize];

// Max mem snapshot. 
// We do a snapshot of the RAM each time the total allocated mem
// is as its highest (with a histeresis of MEM_gul_SnapShotDeltaSize).
BOOL 		MEM_gb_SnapShotMaxMemUsed = 0;
ULONG 		MEM_gul_SnapShotMaxMemSize = 0;
ULONG 		MEM_gul_SnapShotDeltaSize = 10 * 1024;

// Have the allocations been made in basic or virtual RAM ?
char		MEM_gab_VirtualAllocation[MEM_Cul_AllocationHistorySize];

// Number of page miss for each alloc
int			MEM_gai_PageMissNb[MEM_Cul_AllocationHistorySize];

// Time lost to page miss for each alloc
int		MEM_gai_PageMissTime[MEM_Cul_AllocationHistorySize];

// Number of frames that have been logged.
LONG MEM_iLogFrameNb = -1;

// Infos to log page miss that produces worst peak time during one frame.
#define MEM_cul_PageMissSize	50000

static int 		MEM_gi_WorstPageMissSize;
static int 		MEM_gi_WorstPageMissTime;
static char		*MEM_gas_WorstPageMissFile[MEM_cul_PageMissSize];
static int		MEM_gai_WorstPageMissLine[MEM_cul_PageMissSize];
static int		MEM_gai_WorstPageMissTime[MEM_cul_PageMissSize];

static int 		MEM_gi_CurrentPageMissSize;
static int	 	MEM_gi_CurrentPageMissTime;
static char		*MEM_gas_CurrentPageMissFile[MEM_cul_PageMissSize];
static int		MEM_gai_CurrentPageMissLine[MEM_cul_PageMissSize];
static int		MEM_gai_CurrentPageMissTime[MEM_cul_PageMissSize];

static BOOL MEM_bPrintSnapShotAtExit = FALSE;

//LONG		MEM_gap_FreeHistoryTable[MEM_Cul_AllocationHistorySize];
//char		*MEM_gapsz_FreeInFile[MEM_Cul_AllocationHistorySize];
//int			MEM_gai_FreeInLine[MEM_Cul_AllocationHistorySize];
#endif // MEM_USE_HISTORY
#endif // MEM_SPY

/*$4
 ***********************************************************************************************************************
    VARIABLES
 ***********************************************************************************************************************
 */

// Basic memory heap  
#ifdef JADEFUSION
__declspec(align(32)) MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#else
MEM_tdst_MainStruct	MEM_gst_MemoryInfo;
#endif

#ifdef _GAMECUBE
// Virtual memory heap  
MEM_tdst_MainStruct	MEM_gst_VirtualMemoryInfo;

// Sound heap (because it creates frags in memory).
MEM_tdst_MainStruct MEM_gst_SoundMemoryInfo;

// When loading, this memory is used by BINK, load buffers, and small frame buffers.
// When playing, this memory is used by after fx, z list, compute buffer, SPG2 cache lines and big frame buffers.
MEM_tdst_MainStruct	MEM_gst_GCMemoryInfo;

#ifdef _RVL
MEM_tdst_MainStruct	MEM_gst_RVLMEM1;
MEM_tdst_MainStruct	MEM_gst_RVLMEM2;
MEM_tdst_MainStruct	MEM_gst_RVLMEM3;
MEM_tdst_MainStruct	MEM_gst_RVLMEMTMP;
#endif // _RVL

MEM_tdst_MainStruct* MEM_tdst_MainStructArray[] = {
			&MEM_gst_VirtualMemoryInfo,&MEM_gst_SoundMemoryInfo,&MEM_gst_GCMemoryInfo,&MEM_gst_MemoryInfo
#ifdef _RVL
			,&MEM_gst_RVLMEM1, &MEM_gst_RVLMEM2, &MEM_gst_RVLMEM3, &MEM_gst_RVLMEMTMP
#endif // _RVL
			, 0
			};
#endif //_GAMECUBE

#ifdef _GAMECUBE
MEM_tdst_MainStruct* MEM_pChooseMemFromAddress(void *pAddress)
{
	MEM_tdst_MainStruct** p = MEM_tdst_MainStructArray;
	while(*p)
	{
		if(IsAddrInHeap(*p,pAddress))
			return *p;
		p++;
	}
	return 0;
}
#endif // _GAMECUBE
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Elements
 -----------------------------------------------------------------------------------------------------------------------
 */

#define MEM_C_ElementPoolSize 10000

// Number of free elements left (current and min)
int MEM_iCurrentFreeElementLeft = MEM_C_ElementPoolSize;
int MEM_iMinFreeElementLeft = MEM_C_ElementPoolSize;

// List of free elements.
MEM_tdst_Element *MEM_pFreeElements = NULL;
	
// Pool from which we take elements
MEM_tdst_Element MEM_ast_ElementPool[MEM_C_ElementPoolSize];

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */
#ifdef MEM_USE_HISTORY
void MEM_vSaveSnapShot();
#endif // MEM_USE_HISTORY

#ifdef MEM_CHECK
// Check the validity of a free block
void MEM_vCheckFreeBlock(void *_pAddress)
{
	void *pPrevBlock,*pNextBlock,*pBlockAfter;
	void *pUserAddress;
	
	ULONG uSize;
	
	MEM_ASSERT_CRITICAL();
	
	uSize = MEM_iGetBlockSize(_pAddress); 

	ERR_X_Assert(!(uSize & 0x00000003));
	MEM_CHECK_SIZE(uSize,_pAddress);
	
	pUserAddress = MEM_pGetUserAddress(_pAddress);
	ERR_X_Assert(!((ULONG)pUserAddress & 0x00000003));
			
	pPrevBlock = (void *)MEM_pGetPrevBlock(_pAddress);
	if (pPrevBlock)
	{
		ERR_X_Assert(uSize == MEM_iGetBlockSize(pPrevBlock));
		ERR_X_Assert(_pAddress == (void *)MEM_pGetNextBlock(pPrevBlock));
	}
				
	pNextBlock = (void *)MEM_pGetNextBlock(_pAddress);
	if (pNextBlock)
	{
		ERR_X_Assert(uSize == MEM_iGetBlockSize(pNextBlock));
		ERR_X_Assert(_pAddress == (void *)MEM_pGetPrevBlock(pNextBlock));
	}
				
	pBlockAfter = MEM_pGetBlockAfter(_pAddress);
	ERR_X_Assert(!MEM_bIsBlockFree(pBlockAfter));	
	if (MEM_iGetBlockSize(pBlockAfter))
		MEM_vCheckAllocatedBlock(pBlockAfter);
	
	ERR_X_Assert(MEM_bIsBlockBeforeFree(pBlockAfter));
		
	ERR_X_Assert((void *)MEM_pGetBlockBefore(pBlockAfter) == _pAddress);
}

#ifdef MEM_USE_HISTORY
int MEM_iGetKeyFromAddress(void *_pBlockAddress)
{
	ULONG i_Key,i_EndKey;
	MEM_tdst_MainStruct *pMem = MEM_pChooseMemFromAddress(_pBlockAddress);
	ULONG uAddress = (ULONG)MEM_pGetUserAddress(_pBlockAddress);
	
	i_Key = uAddress;
	i_Key = i_Key % MEM_Cul_AllocationHistorySize;
	i_EndKey = (i_Key+1) % MEM_Cul_AllocationHistorySize;

	while ((i_Key != i_EndKey) && (MEM_gap_AllocationHistoryTable[i_Key] != uAddress))
	{
		i_Key = (i_Key ? i_Key - 1 : MEM_Cul_AllocationHistorySize - 1);
	}
	
	ERR_X_Assert(MEM_gap_AllocationHistoryTable[i_Key] == uAddress);
	
	return i_Key;
}
#endif // MEM_USE_HISTORY

void MEM_AssertAddress(void *_pAddress, int _bCondition)
{
	if (!_bCondition)
	{
		ULONG uAddress = (ULONG)MEM_pGetUserAddress(_pAddress);
#ifdef MEM_USE_HISTORY
		ULONG i_Key = MEM_iGetKeyFromAddress(_pAddress);
		_print ("Address problem at %s(%d) : %x to %x\n",
			MEM_gapsz_AllocatedInFile[i_Key],
			MEM_gai_AllocatedInLine[i_Key],
			MEM_gap_AllocationHistoryTable[i_Key],
			MEM_gap_AllocationHistoryTableEnd[i_Key]);
#else // MEM_USE_HISTORY
		_print ("Address problem at %x\n",uAddress);
#endif // MEM_USE_HISTORY
		
		
#if (defined MEM_OVERRUN_MARKER) && (defined MEM_USE_HISTORY)
		// Search next alloc using marker (to know who wrote on the marker)
		{
			void *pCurrentAddress = (void *)uAddress;
			void *pEndAddress = (void *)MEM_gap_AllocationHistoryTableEnd[i_Key];
			while ((pCurrentAddress <= pEndAddress) && (MEM_OVERRUN_MARKER_BEGIN != *((ULONG *)pCurrentAddress)))
			{
#ifdef JADEFUSION
				pCurrentAddress = ((ULONG *)pCurrentAddress) + 1;
#else
				((ULONG *)pCurrentAddress) += 1;
#endif
			}
			
			if (pCurrentAddress <= pEndAddress)
			{
				i_Key = MEM_iGetKeyFromAddress((void *) (((ULONG)pCurrentAddress)-4));
				_print("The following alloc probably wrote over your data %s(%d) : %x to %x\n",
					MEM_gapsz_AllocatedInFile[i_Key],
					MEM_gai_AllocatedInLine[i_Key],
					MEM_gap_AllocationHistoryTable[i_Key],
					MEM_gap_AllocationHistoryTableEnd[i_Key]);
			}
			else 
			{
				_print("Impossible to know who wrote over the data\n");
			}
			
		}
#endif // MEM_OVERRUN_MARKER
		
		ERR_X_Assert(0);
	}		
}

// Check the validity of an allocated block
void MEM_vCheckAllocatedBlock(void *_pAddress)
{
	void *pBlockAfter;
	ULONG uSize;
	MEM_tdst_MainStruct *pMem = MEM_pChooseMemFromAddress(_pAddress);

#ifdef MEM_CRITICAL
	ULONG bInCriticalSection = MEM_bIsInCriticalSection(pMem);	
	if (!bInCriticalSection)
		MEM_M_BeginCriticalSection(pMem);
#endif //MEM_CRITICAL
		
	uSize = MEM_iGetBlockSize(_pAddress); 
	ERR_X_Assert(!((ULONG)_pAddress & 0x00000003));
	MEM_AssertAddress(_pAddress,
		(!(uSize & 0x00000003) &&
		(uSize >= MEM_MIN_SIZE) 
#ifdef _GAMECUBE
	  &&(uSize <= (MEM_bIS_VMAddress(_pAddress) ? MEM_gst_VirtualMemoryInfo.ul_RealSize : MEM_MAX_SIZE ))
#endif // _GAMECUBE
		));
		
	pBlockAfter = MEM_pGetBlockAfter(_pAddress);
	
#ifdef MEM_OVERRUN_MARKER
	MEM_AssertAddress(_pAddress,MEM_OVERRUN_MARKER_BEGIN == (*(((ULONG *)_pAddress) + 1)));
	MEM_AssertAddress(_pAddress,MEM_OVERRUN_MARKER_END == (*(((ULONG *)pBlockAfter) - 1)));
#endif // MEM_OVERRUN_MARKER
	
	MEM_AssertAddress(_pAddress,MEM_bIsBlockFree(pBlockAfter) || !MEM_bIsBlockBeforeFree(pBlockAfter));
	MEM_AssertAddress(_pAddress,!MEM_bIsBlockBeforeFree(_pAddress) || MEM_bIsBlockFree((void *)MEM_pGetBlockBefore(_pAddress)));
		
#ifdef MEM_CRITICAL
	if (!bInCriticalSection)
		MEM_M_EndCriticalSection(pMem);
#endif // MEM_CRITICAL
}


// Check the validity of an allocated block by scanning of all the memory.
void MEM_vSlowCheckAllocatedBlock(MEM_tdst_MainStruct *_pMem,void *_pAddress)
{
	static int MEM_bSlowCheckAlloc = 0;
	void *pBlockAddress;
#ifdef MEM_CRITICAL
	ULONG bInCriticalSection = MEM_bIsInCriticalSection(_pMem);
	
	if (!bInCriticalSection)
		MEM_M_BeginCriticalSection(_pMem);
#endif //MEM_CRITICAL
	
	
	if (MEM_bSlowCheckAlloc)
	{
		MEM_vCheckAllocatedBlock(_pAddress);
		
		// Check that the address can be found by scanning all the heap from begin to end.
		pBlockAddress = _pMem->pv_DynamicBloc;
				
		while (pBlockAddress < _pMem->pvMemoryHeapEnd)
		{
			if (_pAddress == pBlockAddress)
			{
				MEM_vCheckAllocatedBlock(pBlockAddress);
				return; // Address found;
			}
			
			ERR_X_Assert(pBlockAddress < _pAddress); // Error
#ifdef JADEFUSION		
			pBlockAddress = MEM_pGetBlockAfter(pBlockAddress);
#else
			(unsigned char *)pBlockAddress = MEM_pGetBlockAfter(pBlockAddress);
#endif
		}
	}

#ifdef MEM_CRITICAL
	if (!bInCriticalSection)
		MEM_M_EndCriticalSection(_pMem);
#endif //MEM_CRITICAL
}

#ifdef MEM_FULLCHECK
int MEM_bCheckMem = 1;
#else // MEM_FULLCHECK
int MEM_bCheckMem = 0;
#endif // MEM_FULLCHECK
int MEM_iCheckMem = 0;
int MEM_iCheckMemSTART = 0;

#define MEM_vConditionnalCheckMem(_pMem) {MEM_iCheckMem ++ ; if (MEM_bCheckMem && (MEM_iCheckMem >= MEM_iCheckMemSTART)) MEM_vCheckMem(_pMem);}

// Check that the memory is coherent.
void MEM_vCheckMem(MEM_tdst_MainStruct *_pMem)
{
	static int MEM_bCheckHeap = 1;
	static int MEM_bCheckFreeMem = 1;
	static int MEM_bCheckHistory = 1;
	static int MEM_bFullCheckHistory = 1;
	
	void *pBlockAddress;
	ULONG i,uFreeMem,uAllocatedMem,uFreeBlockNb1,uFreeBlockNb2,uAllocatedBlockNb;
	
#ifdef MEM_CRITICAL
	ULONG bInCriticalSection = MEM_bIsInCriticalSection(_pMem);
#endif // MEM_CRITICAL	
	
	
	if (MEM_bCheckHeap)
	{		
			
		// Check the heap from begin to end.
		pBlockAddress = _pMem->pv_DynamicBloc;
		
		uFreeMem = 0;
		uAllocatedMem = 0;
		uAllocatedBlockNb = 0;
		uFreeBlockNb1 = 0;
		uFreeBlockNb2 = 0;
		
#ifdef MEM_CRITICAL
		if (!bInCriticalSection)
			MEM_M_BeginCriticalSection(_pMem);
#endif // MEM_CRITICAL	
		
		while (pBlockAddress < _pMem->pvMemoryHeapEnd)
		{
			ULONG uSize;
			
			uSize = MEM_iGetBlockSize(pBlockAddress); 
			MEM_CHECK_SIZE(uSize,pBlockAddress);
			ERR_X_Assert(!(uSize & 0x00000003));
			if (MEM_bIsBlockFree(pBlockAddress))
			{
				MEM_vCheckFreeBlock(pBlockAddress);
				uFreeMem += uSize;
				uFreeBlockNb1++;
			}
			else
			{
				MEM_vCheckAllocatedBlock(pBlockAddress);
				uAllocatedMem += uSize;				
				uAllocatedBlockNb++;
				
#ifdef MEM_USE_HISTORY
				if (MEM_bFullCheckHistory)
				{
					MEM_iGetKeyFromAddress(pBlockAddress);
				}
#endif // MEM_USE_HISTORY
				
			}
#ifdef JADEFUSION		
			pBlockAddress = (unsigned char *)pBlockAddress + uSize;
#else
			(unsigned char *)pBlockAddress += uSize;
#endif
		}
		
		ERR_X_Assert(uFreeMem + uAllocatedMem == _pMem->ul_RealSize);
		ERR_X_Assert(uAllocatedMem == _pMem->ul_DynamicCurrentAllocated);
		ERR_X_Assert(_pMem->ul_HolesStatic == uFreeBlockNb1);
		
#ifdef MEM_CRITICAL
		if (!bInCriticalSection)
			MEM_M_EndCriticalSection(_pMem);
#endif // MEM_CRITICAL
	}
	
	if (MEM_bCheckFreeMem)
	{
#ifdef MEM_CRITICAL
		if (!bInCriticalSection)
			MEM_M_BeginCriticalSection(_pMem);
#endif // MEM_CRITICAL

		// Check the element and free block lists.
		for (i=0; i<MEM_C_PowerArraySize; i++)
		{
			MEM_tdst_Element *pElement;
			pElement = _pMem->ap_PowerArray[i];
			if (pElement)
				ERR_X_Assert(!pElement->pst_PrevElement);
			
			while (pElement)
			{
				ULONG uSize;
				void *pFreeBlock;
				MEM_tdst_Element *pNextElement;
				
				uSize = pElement->u_Size;
				
				ERR_X_Assert(MEM_iGetIndexInPowerArray(uSize) == i);
				
				pFreeBlock = pElement->p_BlockList;
				ERR_X_Assert(pFreeBlock);
					
				while (pFreeBlock)
				{
					ERR_X_Assert(MEM_iGetBlockSize(pFreeBlock) == uSize);
					MEM_vCheckFreeBlock(pFreeBlock);
					uFreeBlockNb2++;
					pFreeBlock = (void *)MEM_pGetNextBlock(pFreeBlock);
				}
						
				pNextElement = pElement->pst_NextElement;
				if (pNextElement)
					ERR_X_Assert(pNextElement->pst_PrevElement == pElement);
					
				pElement = pNextElement;
			}
		}
		
		ERR_X_Assert(_pMem->ul_HolesStatic == uFreeBlockNb2);
		//ERR_X_Assert(MEM_bIsBlockBeforeFree(_pMem->pv_LastFree));
#ifdef MEM_CRITICAL
		if (!bInCriticalSection)
			MEM_M_EndCriticalSection(_pMem);
#endif // MEM_CRITICAL
		
		//ERR_X_Assert(MEM_uGetLastBlockSize(_pMem)) ;
	}
	
#ifdef MEM_USE_HISTORY
	if (MEM_bCheckHistory)
	{
		int iTableElementNb = 0;
		
	#ifdef MEM_CRITICAL
		ULONG bInCriticalSection = MEM_bIsInCriticalSection(&MEM_gst_MemoryInfo);
	#ifdef _GAMECUBE
		ULONG bVMInCriticalSection = MEM_bIsInCriticalSection(&MEM_gst_VirtualMemoryInfo);
		if (!bVMInCriticalSection)
			MEM_M_BeginCriticalSection(&MEM_gst_VirtualMemoryInfo);
	#endif //_GAMECUBE		
		if (!bInCriticalSection)
			MEM_M_BeginCriticalSection(&MEM_gst_MemoryInfo);
	#endif //  MEM_CRITICAL
	
		
		for (i=0; i<MEM_Cul_AllocationHistorySize; i++)
		{
			if (MEM_gap_AllocationHistoryTable[i])
			{
				ULONG uBlockAddress = (ULONG)MEM_pGetBlockAddress(MEM_gap_AllocationHistoryTable[i]);
				ULONG uSize = MEM_iGetBlockSize(uBlockAddress);
				MEM_vCheckAllocatedBlock((void *)uBlockAddress);
				ERR_X_Assert(MEM_gap_AllocationHistoryTableEnd[i] = uSize + uBlockAddress);
				iTableElementNb++;
			}
		}
		ERR_X_Assert(MEM_g_nbpointers == iTableElementNb);
		
	#ifdef MEM_CRITICAL
		if (!bInCriticalSection)
			MEM_M_EndCriticalSection(&MEM_gst_MemoryInfo);
	#ifdef _GAMECUBE
		if (!bVMInCriticalSection)
			MEM_M_EndCriticalSection(&MEM_gst_VirtualMemoryInfo);
	#endif //_GAMECUBE		
	#endif //  MEM_CRITICAL		
	}
#endif // MEM_USE_HISTORY
}

#else// MEM_CHECK

#define MEM_vConditionnalCheckMem(_pMem)

#endif// MEM_CHECK






/*$4
 ***********************************************************************************************************************
    public functions (all targets)
 ***********************************************************************************************************************
 */

extern int	GetHeapSize(void);
extern int	GetStackSize(void);
extern int	GetStackStart(void);



/*
 =======================================================================================================================
    Aim:    Get element from pool.
 =======================================================================================================================
 */
MEM_tdst_Element *MEM_pGetFreeElement()
{
	MEM_tdst_Element *pElement;
	
	MEM_ASSERT_CRITICAL();
	
	ERR_X_Assert(MEM_pFreeElements); // Make MEM_C_ElementPoolSize bigger if assert is false.
		
	pElement = MEM_pFreeElements;
	MEM_pFreeElements = pElement->pst_NextElement;
	pElement->pst_NextElement = NULL;

	MEM_iCurrentFreeElementLeft--;
	if (MEM_iMinFreeElementLeft > MEM_iCurrentFreeElementLeft)
		MEM_iMinFreeElementLeft = MEM_iCurrentFreeElementLeft;
		
	return pElement;
}

u32 MEM_iGetBiggestFreeSize(MEM_tdst_MainStruct *_pMem)
{
	int i;
	for (i = MEM_C_PowerArraySize-1; i>=0; i--) 
	{
		MEM_tdst_Element *pElement = _pMem->ap_PowerArray[i];
		if (pElement != NULL)
		{
			while (pElement->pst_NextElement != NULL)
			{
				pElement = pElement->pst_NextElement;
			}
			return pElement->u_Size;
		}
	}
	return 0;
}

/*
 =======================================================================================================================
    Aim:    Put element in pool.
 =======================================================================================================================
 */
void MEM_vPutBackElement(MEM_tdst_Element *_pElement)
{
	MEM_ASSERT_CRITICAL();
	
	_pElement->u_Size = 0;
	_pElement->p_BlockList = NULL;
	_pElement->pst_PrevElement = NULL;
	_pElement->pst_NextElement = MEM_pFreeElements;
	MEM_pFreeElements = _pElement;

	MEM_iCurrentFreeElementLeft++;
}


MEM_tdst_Element *MEM_pSearchOrCreateElementWithSize(MEM_tdst_MainStruct *_pMem,ULONG _uSize)
{
	MEM_tdst_Element *pElement,*pPrevElement;
	
	MEM_ASSERT_CRITICAL();
	
	// Search for element that corresponds to this size.
	pElement = _pMem->ap_PowerArray[MEM_iGetIndexInPowerArray(_uSize)];
	pPrevElement = NULL;
	
	while (pElement && (pElement->u_Size < _uSize))
	{
		pPrevElement = pElement;
		pElement = pElement->pst_NextElement;
	}
		
	if (pElement && (pElement->u_Size == _uSize))
		return pElement;
	else
	{
		MEM_tdst_Element *pNewElement;
		
		// We did not find the element, we have to create it
		pNewElement = MEM_pGetFreeElement();
		pNewElement->u_Size = _uSize;
		pNewElement->p_BlockList = NULL;
		
		pNewElement->pst_NextElement = pElement;
		pNewElement->pst_PrevElement = pPrevElement;

		if (pPrevElement)
			pPrevElement->pst_NextElement = pNewElement;
		else
			_pMem->ap_PowerArray[MEM_iGetIndexInPowerArray(_uSize)] = pNewElement;
		
		if (pElement)
			pElement->pst_PrevElement = pNewElement;
		
		return pNewElement;
	}
}


/*
 =======================================================================================================================
    Aim:    Create free block from RAM chunk, and insert it in lists.
 =======================================================================================================================
 */
void MEM_vCreateFreeBlock(MEM_tdst_MainStruct *_pMem,void *_pAddress, ULONG _uSize)
{
	MEM_tdst_Element *pElement;
	
	MEM_ASSERT_CRITICAL();
	
	MEM_CHECK_SIZE(_uSize,_pAddress);
	
	// First long (except first bit) is the size of the block, with a shift of 2. 
	*((ULONG *)_pAddress) = _uSize >> 2;

	// The first bit says that it is a free block, and the third that it does not have a spy key.
	*((ULONG *)_pAddress) |= 0xA0000000;

	// The last long is a pointer to the block address.
	((ULONG *)_pAddress)[(_uSize>>2) - 1] = (ULONG)_pAddress;
	
	// Search for element that corresponds to this size.
	pElement = MEM_pSearchOrCreateElementWithSize(_pMem,_uSize);
	
	// Set prev and next pointers and insert the block in the element.
	MEM_pGetPrevBlock(_pAddress) = (unsigned int) NULL;
	MEM_pGetNextBlock(_pAddress) = (ULONG)pElement->p_BlockList;
	if (pElement->p_BlockList)
		MEM_pGetPrevBlock(pElement->p_BlockList) = (ULONG)_pAddress;
	pElement->p_BlockList = _pAddress;
		
	MEM_vBlockBeforeOfBlockAfterIsFree(_pAddress,_uSize);
	
	_pMem->ul_HolesStatic++;
}


/*
 =======================================================================================================================
    Aim:    Create allocated block from RAM chunk
 =======================================================================================================================
 */
void MEM_vCreateAllocatedBlock(void *_pAddress, ULONG _uSize, int _bIsBeforeBlockFree)
{
	MEM_CHECK_SIZE(_uSize,_pAddress);
	ERR_X_Assert(!(_uSize & 3));
	
	MEM_ASSERT_CRITICAL();
	
	// First long (except first bits) is the size of the block, with a shift of 2. 
	*((ULONG *)_pAddress) = _uSize >> 2;

	// The first bit says that it is not a free block.
	*((ULONG *)_pAddress) &= 0x7FFFFFFF;
	
	// The second bit tells if the before block is free.
	if (_bIsBeforeBlockFree)
		*((ULONG *)_pAddress) |= 0x40000000;
	else
		*((ULONG *)_pAddress) &= 0xBFFFFFFF;
		
#ifdef MEM_OVERRUN_MARKER
	*(((ULONG *)_pAddress)+1) = MEM_OVERRUN_MARKER_BEGIN;
	*(((ULONG *)_pAddress)+(_uSize>>2)-1) = MEM_OVERRUN_MARKER_END;
#endif // MEM_OVERRUN_MARKER		

	MEM_vBlockBeforeOfBlockAfterIsAllocated(_pAddress,_uSize);	
}

 
/*
 =======================================================================================================================
    Aim:    Init the elements.
 =======================================================================================================================
 */
void MEM_InitElements()
{
	int i;
	MEM_tdst_Element *pCurrentElement;
		
	// Init the elements, and build the free element list.
	MEM_ast_ElementPool->u_Size = 0;
	MEM_ast_ElementPool->p_BlockList = NULL;
	MEM_ast_ElementPool->pst_PrevElement = NULL;
	MEM_ast_ElementPool->pst_NextElement = MEM_ast_ElementPool+1;
	for (i=1; i<MEM_C_ElementPoolSize-1; i++)
	{
		pCurrentElement = MEM_ast_ElementPool + i;
		
		pCurrentElement->u_Size = 0;
		pCurrentElement->p_BlockList = NULL;
		pCurrentElement->pst_PrevElement = NULL;
		pCurrentElement->pst_NextElement = MEM_ast_ElementPool + (i+1);
	}
	MEM_ast_ElementPool[MEM_C_ElementPoolSize-1].u_Size = 0;
	MEM_ast_ElementPool[MEM_C_ElementPoolSize-1].p_BlockList = NULL;
	MEM_ast_ElementPool[MEM_C_ElementPoolSize-1].pst_PrevElement = MEM_ast_ElementPool + MEM_C_ElementPoolSize-2;
	MEM_ast_ElementPool[MEM_C_ElementPoolSize-1].pst_NextElement = NULL;
	
	MEM_pFreeElements = MEM_ast_ElementPool;

}

// Alloc heap
void *MEM_pAllocHeap(ULONG _ul_BigBlockSize)
{
	void *pHeapAddress;
	
	// Check that the size is a multiple of 16 bytes.	
	ERR_X_Assert(!(_ul_BigBlockSize & 0x00000003));
	
	
	/* Allocates the Big Dynamic bloc using static allocation */
#ifdef WIN32

	/* Plus rapide qu'au dessous sous windows */
	pHeapAddress = HeapAlloc(GetProcessHeap(), 0, _ul_BigBlockSize);
#else
	pHeapAddress = (void *) (MEM_p_sAlloc(_ul_BigBlockSize));
#endif

	/* Check that we had enough system memory to perform the allocation */
	ERR_X_ErrorAssert(pHeapAddress != 0, MEM_ERR_Csz_NotEnoughSystemMemory, NULL);	
	
#ifdef MEM_FILL_FREE_BLOCKS
	L_memset(pHeapAddress ,MEM_FREE_FILL_DATA ,_ul_BigBlockSize);
#endif // MEM_FILL_FREE_BLOCKS	

	return pHeapAddress;
}

void MEM_vInitHeap( MEM_tdst_MainStruct *_pMem,void *_pHeapAddress,ULONG _ul_BigBlockSize)
{
	int i;
	_pMem->bVirtual = MEM_bIsVirtual(_pMem);
	_pMem->bDisableInterrupts = !_pMem->bVirtual;

	_pMem->pv_DynamicBloc = _pHeapAddress;
	
	// Init _pMem->ap_PowerArray	
	for (i=0; i<MEM_C_PowerArraySize; i++)
	{
		_pMem->ap_PowerArray[i] = NULL;
	}
		
	MEM_M_BeginCriticalSection(_pMem);
	
	// Create a free block with the heap
	// The last bytes (at least 4) of the heap are used as a fake allocated block (of size 0),
	// because when we create a free (resp. allocated) block, 
	// we write in the block after that its block before is free (resp. allocated).
	_pMem->pvMemoryHeapEnd = (void *)((ULONG)(_pMem->pv_DynamicBloc) + _ul_BigBlockSize - 4);
	
	*((ULONG *)_pMem->pvMemoryHeapEnd) = 0x40000000; // Fake allocated block of size 0 with a free block before.
			
	_pMem->ul_RealSize = (ULONG) _pMem->pvMemoryHeapEnd - (ULONG)_pMem->pv_DynamicBloc;
	
	MEM_vCreateFreeBlock(_pMem,_pMem->pv_DynamicBloc,_pMem->ul_RealSize);
	
	// End alloc management
	_pMem->pv_LastFree = _pMem->pvMemoryHeapEnd;
	
	// Init MEM_gst_MemoryInfo
	_pMem->ul_DynamicMaxSize = _pMem->ul_RealSize;
	
	_pMem->ul_HolesStatic = 1;	// Number of free blocks (counting initial heap)
	_pMem->ul_DynamicCurrentAllocated = 0;
	_pMem->ul_TexturesCurrentAllocated = 0;
	
	_pMem->pv_DynamicNextFree = _pMem->pv_DynamicBloc;
	
#ifdef MEM_MEASURES
	_pMem->ul_CurrentFree = MEM_uGetFreeMemSize(_pMem);
	_pMem->ul_MinFreeTotal = _pMem->ul_CurrentFree;
	_pMem->ul_MinLastFree = MEM_uGetLastBlockSize(_pMem);
#endif // MEM_MEASURES

	MEM_M_EndCriticalSection(_pMem);
}

void MEM_vReinitHeap(MEM_tdst_MainStruct *_pMem)
{
	MEM_vInitHeap(_pMem, _pMem->pv_DynamicBloc, _pMem->ul_RealSize + 4);
}

/*
 =======================================================================================================================
    Aim:    Allocates the main dynamic block and initialises the lists.
 =======================================================================================================================
 */
void MEM_InitDynamic(MEM_tdst_MainStruct *_pMem,ULONG _ul_BigBlockSize)
{	
	MEM_vInitHeap(_pMem,MEM_pAllocHeap(_ul_BigBlockSize),_ul_BigBlockSize);
}

/*
 =======================================================================================================================
    Aim:    Frees the memory of the main dynamic module
 =======================================================================================================================
 */
void MEM_CloseDynamic(MEM_tdst_MainStruct *_pMem)
{
	
#ifdef WIN32
	HeapFree(GetProcessHeap(), 0, _pMem->pv_DynamicBloc);
#else
	MEM_sFree(_pMem->pv_DynamicBloc);
	_pMem->pv_DynamicBloc = NULL;
	_pMem->ul_RealSize = 0;
#endif
}

#if !((defined _GAMECUBE) && (defined MEM_USE_HISTORY))
#define MEM_vInitLogHeapState()
#endif //if !((defined _GAMECUBE) && (defined MEM_USE_HISTORY))

#ifdef _GAMECUBE

/*
 =======================================================================================================================
 =======================================================================================================================
 */


#ifdef MEM_USE_HISTORY
// Callback function used to gather data about who does page faults.
void gcMEMPageMissCallback( u32 realVirtualAddress, // The virtual address that caused the page fault.
                              u32,// physicalAddress 		// The main memory physical address where the page is being stored.
                              u32 ,//pageNumber,			// The page number where the page is being stored.
                              u32 pageMissLatency,		// The time it took to replace the page (in microseconds).
                              BOOL  //pageSwappedOut 		// Whether the old page was written back to ARAM.
                               )		
{
	// Parse the virtual addresses to find the alloc file name and line number of the alloc.
	ULONG i_Key,i_EndKey;
	
	if (!MEM_bLogPageFaults)
		return;
		
	ERR_X_Assert(MEM_bIS_VMAddress((void*)realVirtualAddress));
	
	/* Fills the history table with the new pointer */
	i_Key = realVirtualAddress;
	i_Key = i_Key % MEM_Cul_AllocationHistorySize;
	i_EndKey = (i_Key+1) % MEM_Cul_AllocationHistorySize;

	while ((i_Key != i_EndKey) && 
		!(MEM_gab_VirtualAllocation[i_Key] && 
		(MEM_gap_AllocationHistoryTable[i_Key] <= realVirtualAddress) &&
		(MEM_gap_AllocationHistoryTableEnd[i_Key] >= realVirtualAddress)))
	{
		i_Key = (i_Key ? i_Key - 1 : MEM_Cul_AllocationHistorySize - 1);
	}
	
	ERR_X_Assert(MEM_gab_VirtualAllocation[i_Key] && 
		(MEM_gap_AllocationHistoryTable[i_Key] <= realVirtualAddress) &&
		(MEM_gap_AllocationHistoryTableEnd[i_Key] >= realVirtualAddress));
	
	// Found the alloc : update stats.
	MEM_gai_PageMissNb[i_Key]++;
	MEM_gai_PageMissTime[i_Key] += pageMissLatency;
	
	MEM_gi_CurrentPageMissTime += pageMissLatency;
	
	MEM_gas_CurrentPageMissFile[MEM_gi_CurrentPageMissSize] = MEM_gapsz_AllocatedInFile[i_Key];
	MEM_gai_CurrentPageMissLine[MEM_gi_CurrentPageMissSize] = MEM_gai_AllocatedInLine[i_Key];
	MEM_gai_CurrentPageMissTime[MEM_gi_CurrentPageMissSize] = pageMissLatency;
	MEM_gi_CurrentPageMissSize++;
	ERR_X_Assert(MEM_gi_CurrentPageMissSize < MEM_cul_PageMissSize);
	if (MEM_gi_CurrentPageMissSize >=  MEM_cul_PageMissSize)
		MEM_gi_CurrentPageMissSize = MEM_cul_PageMissSize-1;
}      

void MEM_vInitLogHeapState()
{
	int i;
	
	for (i=0; i<MEM_C_PowerArraySize; i++)
	{
		MEM_gai_PageMissNb[i] = 0;
		MEM_gai_PageMissTime[i] = 0;
	}
	MEM_iLogFrameNb = 0;
	
	MEM_gi_WorstPageMissSize = 0;
	MEM_gi_CurrentPageMissSize = 0;
	MEM_gi_WorstPageMissTime = 0;
	MEM_gi_CurrentPageMissTime = 0;
}
#endif // MEM_USE_HISTORY
               

/*void *MEM_pTestVMAlloc = NULL;
ULONG MEM_uTestVMSize = 0;
void MEM_InitTestVM()
{
	void *pAddress,*pEndAddress;
	
	MEM_uTestVMSize = MEM_VM_ARAM_Size-32;
	MEM_pTestVMAlloc = MEM_p_VMAlloc(MEM_uTestVMSize);
	
	pAddress = MEM_pTestVMAlloc;
	pEndAddress = (void *)((ULONG)MEM_pTestVMAlloc + MEM_uTestVMSize);
	while (pAddress<pEndAddress)
	{
		*((ULONG *)pAddress) = ((ULONG)pAddress);
		((ULONG *)pAddress) += 1;
	}
}

void MEM_TestVM()
{
	void *pAddress,*pEndAddress;
	
	pAddress = MEM_pTestVMAlloc;
	pEndAddress = (void *)((ULONG)MEM_pTestVMAlloc + MEM_uTestVMSize);
	while (pAddress<pEndAddress)
	{
		ERR_X_Assert(*((ULONG *)pAddress) == ((ULONG)pAddress));
		((ULONG *)pAddress) += 1;
	}
}*/
extern u32	g_CurrentARAMAddr;

//extern OSMutex gcINO_ResetMutex;
//extern OSMutex GC_DVDGetDriveStatusMutex;
//extern OSMutex gcINO_JoystickMutex;

#ifdef GC2RVL_NO_ARAM
void* L_malloc(ULONG _ul_size)
{
	return MEM_p_AllocMem(_ul_size, &MEM_gst_VirtualMemoryInfo);
}
void L_free(void *p)
{
	MEM_Free(p);
}
#endif // GC2RVL_NO_ARAM

void gcMEM_PrepareMemoryPool(void)
{

#ifdef GC2RVL_NO_ARAM
	u8	*arenalo, *arenahi;
	MEM_tdst_MainStruct** pMainStruct;
	u32 s;
	
	GC_OutputDebugString("Mem1 size = %dKB, free = %dKB\n",OSGetPhysicalMem1Size()/1024,((UINT)OSGetMEM1ArenaHi()-(UINT)OSGetMEM1ArenaLo())/1024);
	//GC_OutputDebugString("Mem1 arena lo 0x%x hi 0x%x\n",OSGetMEM1ArenaLo(), OSGetMEM1ArenaHi());
	GC_OutputDebugString("Mem2 size = %dKB, free = %dKB\n",OSGetPhysicalMem2Size()/1024,((UINT)OSGetMEM2ArenaHi()-(UINT)OSGetMEM2ArenaLo())/1024);	
	//GC_OutputDebugString("Mem2 arena lo 0x%x hi 0x%x\n",OSGetMEM2ArenaLo(), OSGetMEM2ArenaHi());

	/* Zero heap structs */
	pMainStruct = MEM_tdst_MainStructArray;
	while(*pMainStruct)
	{
		memset(*pMainStruct, 0,sizeof(MEM_tdst_MainStruct));
		pMainStruct++;
	}

	MEM_gst_RVLMEM1.pLastResort = &MEM_gst_RVLMEM2;
	MEM_gst_RVLMEM2.pLastResort = &MEM_gst_RVLMEM3;
	//MEM_gst_RVLMEM3.pLastResort = 0;
	MEM_gst_RVLMEMTMP.pLastResort = &MEM_gst_RVLMEM3;
	MEM_gst_MemoryInfo.pLastResort = &MEM_gst_VirtualMemoryInfo;
	MEM_gst_VirtualMemoryInfo.pLastResort = &MEM_gst_RVLMEM2;
	MEM_gst_SoundMemoryInfo.pLastResort = &MEM_gst_VirtualMemoryInfo;
	MEM_gst_GCMemoryInfo.pLastResort = &MEM_gst_MemoryInfo;

#ifdef MEM_SPY
	MEM_gst_RVLMEM1.pcHeapName = "RVLMEM1";
	MEM_gst_RVLMEM2.pcHeapName = "RVLMEM2";
	MEM_gst_RVLMEM3.pcHeapName = "RVLMEM3";
	MEM_gst_MemoryInfo.pcHeapName = "MainMem";
	MEM_gst_VirtualMemoryInfo.pcHeapName = "Virtual Mem";
	MEM_gst_SoundMemoryInfo.pcHeapName = "Sound Mem";
	MEM_gst_GCMemoryInfo.pcHeapName = "GC Mem";
#endif // MEM_SPY

	/* Create allocator on MEM1 */
	s = (UINT)OSGetMEM1ArenaHi()-(UINT)OSGetMEM1ArenaLo();
	arenalo = OSAllocFromMEM1ArenaLo(s,32);
	MEM_vInitHeap(&MEM_gst_RVLMEM1,arenalo, s);

	/* Create allocator on MEM2 */
	s = (UINT)OSGetMEM2ArenaHi()-(UINT)OSGetMEM2ArenaLo();
	if( s > 32*1024*1024 )
		s = 32*1024*1024;
	arenalo = OSAllocFromMEM2ArenaLo(s,32);
	MEM_vInitHeap(&MEM_gst_RVLMEM2, arenalo, s);
	OSSetMEM2ArenaLo(arenalo+s);

	/* Create allocator on MEM3 */
	s = 10*1024*1024;
	arenalo = OSAllocFromMEM2ArenaLo(s,32);
	MEM_vInitHeap(&MEM_gst_RVLMEM3, arenalo, s);
	OSSetMEM2ArenaLo(arenalo+s);

	/* Create allocator on MEMTMP */
	s = 10*1024*1024;
	arenalo = OSAllocFromMEM2ArenaLo(s,32);
	MEM_vInitHeap(&MEM_gst_RVLMEMTMP, arenalo, s);
	OSSetMEM2ArenaLo(arenalo+s);

	/* Default heaps */	

	/* MEM_gst_GCMemoryInfo */
	s = 8*1024*1024;
	MEM_vInitHeap(&MEM_gst_GCMemoryInfo,
				  MEM_p_AllocMem(s, &MEM_gst_RVLMEM1),s);

	/* MEM_gst_MemoryInfo */
	//s = MEM_gst_RVLMEM1.ul_DynamicMaxSize - MEM_gst_RVLMEM1.ul_DynamicCurrentAllocated - 1024;
	s = MEM_gst_RVLMEM1.ul_CurrentFree - 512;
	MEM_vInitHeap(&MEM_gst_MemoryInfo,
				  MEM_p_AllocMem(s, &MEM_gst_RVLMEM1), s);	
	
	/* MEM_gst_VirtualMemoryInfo */
	s = 30*1024*1024;
	MEM_vInitHeap(&MEM_gst_VirtualMemoryInfo,
				  MEM_p_AllocMem(s, &MEM_gst_RVLMEM2), s);

	/* MEM_gst_SoundMemoryInfo */
	s = 5*1024*1024;
	arenalo = OSAllocFromMEM2ArenaLo(s,32);
	MEM_vInitHeap(&MEM_gst_SoundMemoryInfo, arenalo, s);
	OSSetMEM2ArenaLo(arenalo+s);
#else /* GC2RVL_NO_ARAM */

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	void	*arenalo, *arenahi;
	ULONG 	VirtualRAMBaseAddress;
#ifdef ANIMS_USE_ARAM
	u32		u32GameARAMSize[eARAM_NumberOfARAMHeapTypes] = { 256 * 1024, (5 * 1024 - 256 ) * 1024 };
	// must be equal to value in g_u32ARAM_BlocksSize
#endif // ANIMS_USE_ARAM
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Init the memory system
	//OSInitMutex(&gcMEMMutex);
	//OSInitMutex(&gcINO_ResetMutex);
	//OSInitMutex(&GC_DVDGetDriveStatusMutex);
	//OSInitMutex(&gcINO_JoystickMutex);
	
#ifdef MEM_SPY
	MEM_gst_MemoryInfo.pcHeapName = "MainMem";
	MEM_gst_VirtualMemoryInfo.pcHeapName = "Virtual Mem";
	MEM_gst_SoundMemoryInfo.pcHeapName = "Sound Mem";
	MEM_gst_GCMemoryInfo.pcHeapName = "GC Mem";
#endif // MEM_SPY	
	
	// Init the virtual memory system
	VirtualRAMBaseAddress = ARGetBaseAddress();
	VMInit( MEM_VM_RAM_Size, VirtualRAMBaseAddress, MEM_VM_ARAM_Size );
	VMSetPageReplacementPolicy(VM_PRP_RANDOM); // VM_PRP_LRU
#ifdef MEM_USE_HISTORY
	VMSetLogStatsCallback(&gcMEMPageMissCallback);
#endif // MEM_USE_HISTORY

	// Get arena after that the memory manager has taken its memory.
	arenalo = OSGetArenaLo();
	arenahi = OSGetArenaHi();

	/* create the heap */
	arenalo = OSInitAlloc(arenalo, arenahi, 1); /* only one heap */
	OSSetArenaLo(arenalo);
	arenalo = (void *) OSRoundUp32B(arenalo);	/* 32 bytes align */
	arenahi = (void *) OSRoundDown32B(arenahi); /* 32 bytes align */

	GC_OutputDebugString
	(
		"\nArena Lo : %x, Arena Hi : %x Total Free mem : %dKb\n",
		arenalo,
		arenahi,
		((u32) arenahi - (u32) arenalo) / 1024
	);

	OSSetCurrentHeap(OSCreateHeap(arenalo, arenahi));

	// Create the virtual memory manager. 
	VMAlloc( MEM_VM_VirtualStartAddress, MEM_VM_ARAM_Size );
	MEM_vInitHeap(&MEM_gst_VirtualMemoryInfo,(void *)MEM_VM_VirtualStartAddress,MEM_VM_ARAM_Size);

	// Allocate all available memory (except a bit for system calls),
	// and create basic memory manager. 
	MEM_InitDynamic(&MEM_gst_MemoryInfo,OSRoundDown32B(((u32) arenahi - (u32) arenalo - (250 * 1024))));

	// Init aram (independant from virtual memory system).
	ARAM_Init();
	// DO NOT CHANGE THE ORDER OF ARAM HEAP INITS : eARAM_VirtualRAM must be at pVirtualRAMBaseAddress.
	ERR_X_Assert(VirtualRAMBaseAddress == g_CurrentARAMAddr); 
	ARAM_ReserveBlock(eARAM_VirtualRAM,MEM_VM_ARAM_Size);
	gcSND_ARAMInit();
#ifdef ANIMS_USE_ARAM
	ARAM_CreateHeaps(eARAM_Game, eARAM_NumberOfARAMHeapTypes, u32GameARAMSize);
	ARAM_SetDefaultHeap(eARAM_Game, eARAM_Fix);
#endif // ANIMS_USE_ARAM
	
	ARAM_gpst_DMA_FIFO = (ARAM_DMA_FIFO *) MEM_p_Alloc(sizeof(ARAM_DMA_FIFO));
	L_memset(ARAM_gpst_DMA_FIFO, 0, sizeof(ARAM_DMA_FIFO));
	
#ifdef MEM_USE_HISTORY
	MEM_vInitLogHeapState();
#endif // MEM_USE_HISTORY
	
	// Init memory for sound
	MEM_vInitHeap(&MEM_gst_SoundMemoryInfo,MEM_p_Alloc(MEM_Sound_RAM_Size),MEM_Sound_RAM_Size);
	MEM_SoundStartAddress = (ULONG)MEM_gst_SoundMemoryInfo.pv_DynamicBloc;
	MEM_SoundEndAddress = (ULONG)MEM_gst_SoundMemoryInfo.pv_LastFree;
	
	// Init memory for GC
	MEM_vInitHeap(&MEM_gst_GCMemoryInfo,MEM_p_Alloc(MEM_GC_RAM_Size),MEM_GC_RAM_Size);
	MEM_GCStartAddress = (ULONG)MEM_gst_GCMemoryInfo.pv_DynamicBloc;
	MEM_GCEndAddress = (ULONG)MEM_gst_GCMemoryInfo.pv_LastFree;
#endif /* GC2RVL_NO_ARAM */	
}

#endif /* _GAMECUBE */



/*
 =======================================================================================================================
    Aim:    Init the memory module
 =======================================================================================================================
 */
void MEM_InitModule(void)
{
    extern BOOL LOA_gb_SpeedMode;

#ifdef MEM_USE_HISTORY
	L_memset(MEM_gap_AllocationHistoryTable, 0, sizeof(MEM_gap_AllocationHistoryTable));
//	L_memset(MEM_gap_FreeHistoryTable, 0, sizeof(MEM_gap_FreeHistoryTable));
#endif

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_M_InitLock();
	
	MEM_InitElements();

#ifdef MEM_SPY
    MEM_vInitSpy();
#endif //MEM_SPY

#if defined(_GAMECUBE)

	/*$1- GC ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	gcMEM_PrepareMemoryPool();

#elif defined(PSX2_TARGET)

	/*$1- PS2 engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	{
		/*~~~~~~~~~~~~~*/
		ULONG	ul_alloc;
		/*~~~~~~~~~~~~~*/

		ul_alloc = GetStackStart();
		ul_alloc = ul_alloc - GetHeapSize();	/* exe size */
		ul_alloc = ul_alloc - (150 * 1024);		/* secure size */
		
#ifndef _FINAL_		
		// KER
//		ul_alloc = ul_alloc - (1024*1024);		/* secure size */
//		ul_alloc = ul_alloc - 21 * (1024*1024);		/* secure size */		
		// KER
#endif		
		
		MEM_InitDynamic(&MEM_gst_MemoryInfo,ul_alloc);
		/*
		if(!_pMem->pv_DynamicBloc)
		{
			MEM_M_SignalFatalError(ul_alloc, "No more memory to perform init allocation");
		}
		*/
	}

#elif defined(PCWIN_TOOL)

	/*$1- PC tool engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_InitDynamic(&MEM_gst_MemoryInfo,50 * 1024 * 1024);

#elif defined(_XBOX)

	/*$1- XBOX engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(LOA_gb_SpeedMode)
        MEM_InitDynamic(&MEM_gst_MemoryInfo, 25*1024*1024 );//26
    else
        MEM_InitDynamic(&MEM_gst_MemoryInfo, 30*1024*1024 ); // wave files are scanned in this mode, and needed lot of place during map loading

#elif defined(_XENON) 

    MEM_InitDynamic(&MEM_gst_MemoryInfo, DM_MEM_SIZE); 


#elif defined(WIN32)

	/*$1- PC engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_InitDynamic(&MEM_gst_MemoryInfo, (ULONG) (26.5 * 1024 * 1024) );

#elif defined(ACTIVE_EDITORS)

	/*$1- PC editor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_InitDynamic(&MEM_gst_MemoryInfo,(LONG) 100 * (LONG) 1024 * (LONG) 1024);


#else
#pragma message(__FILE__ ">> error : Target is unknown <<")
#endif

#ifdef MEM_USE_HISTORY
    MEM_vSaveSnapShot();
    MEM_bPrintSnapShotAtExit = TRUE;
#endif //MEM_USE_HISTORY

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
}

void MEM_vLogHeapState(void);
void MEM_vPrintDeltaSnapShot();
/*
 =======================================================================================================================
    Aim:    Close the memory module
 =======================================================================================================================
 */
void MEM_CloseModule()
{
	MEM_vLogHeapState();
#ifdef MEM_USE_HISTORY
    // En mode editeur il y a plein de fuites mémoire que je n'ai pas envie de chercher, 
    // et comme ça fait ramer de toutes les afficher, on ne les affiche pas en détail
#ifndef ACTIVE_EDITORS
    if (MEM_bPrintSnapShotAtExit)
        MEM_vPrintDeltaSnapShot();
#endif //ACTIVE_EDITORS
#endif //MEM_USE_HISTORY

	/* Close the dynamic memory bloc */
	MEM_CloseDynamic(&MEM_gst_MemoryInfo);
#if defined( _GAMECUBE) && !defined(GC2RVL_NO_ARAM)
	VMQuit();
#endif //_GAMECUBE

	MEM_M_DestroyLock();
}


ULONG MEM_uGetLastBlockSize(MEM_tdst_MainStruct *_pMem)
{
	void *pLastBlock;
	ULONG uSize;
#ifdef MEM_CRITICAL	
	ULONG bInCriticalSection = MEM_bIsInCriticalSection(_pMem);
	
	if (!bInCriticalSection)
		MEM_M_BeginCriticalSection(_pMem);
#endif //MEM_CRITICAL
	pLastBlock = (void *)_pMem->pv_LastFree;
	
	if (MEM_bIsBlockBeforeFree(pLastBlock))
	{
		pLastBlock = (void *)MEM_pGetBlockBefore(pLastBlock);
		uSize = MEM_iGetBlockSize(pLastBlock);
	}
	else
		uSize= 0;
		
#ifdef MEM_CRITICAL		
	if (!bInCriticalSection)
		MEM_M_EndCriticalSection(_pMem);
#endif // MEM_CRITICAL		
	return uSize;
}



#ifdef MEM_SPY
#ifdef MEM_USE_HISTORY

/*
 =======================================================================================================================
    Aim:    Check that current allocated pointer have good block mark _c_Dump:: 1:: show number of pointer and total
            size of allocated blocks |2:: dump all pointers
 =======================================================================================================================
 */
void MEM_CheckAllocatedBlocks(char _c_Dump)
{
	/*~~~~~~~~~~~~~~~~~*/
	LONG	i;
	ULONG	*pul_Address;
	ULONG	ul_Size,ul_TotalSize;
	/*~~~~~~~~~~~~~~~~~*/

	MEM_ASSERT_CRITICAL();
	
	ul_TotalSize = 0;
	for(i = 0; i < MEM_Cul_AllocationHistorySize; i++)
	{
		pul_Address = (ULONG *) MEM_gap_AllocationHistoryTable[i];
		if(!pul_Address)
			continue;
			
		ul_Size = MEM_iGetBlockSize(MEM_pGetBlockAddress(pul_Address));
		ul_TotalSize += ul_TotalSize;

		if(_c_Dump & 2)
		{
			_print("0x%08X, %8d", (LONG) pul_Address, ul_Size);
		}
		
		MEM_vCheckAllocatedBlock(MEM_pGetBlockAddress(pul_Address));
	}

	if(_c_Dump & 1)
	{
		_print("Mem : %d pointers, total size : %d", MEM_g_nbpointers, ul_TotalSize);
	}
}




int MEM_iCompareSnapShotData(const void *_pIndex1, const void *_pIndex2)
{
	int iIndex1,iIndex2,iCompare,iAddress1,iAddress2;
	
	MEM_ASSERT_CRITICAL();
	
	iIndex1 = *((int *)_pIndex1);
	iIndex2 = *((int *)_pIndex2);
	iAddress1 = MEM_gai_SnapShotAddress[iIndex1];
	iAddress2 = MEM_gai_SnapShotAddress[iIndex2];
	
	// NULL pointers at end of table
	if (iAddress1 == 0)
	{
		if (iAddress2 == 0)
			return 0;
		else
			return 1;
	}
	else
	{
		if (iAddress2 == 0)
			return -1;
	}
	
	iCompare = L_strcmp(MEM_gapsz_SnapShotAllocatedInFile[iIndex1],MEM_gapsz_SnapShotAllocatedInFile[iIndex2]);
	
	// Compare with file name	
	if (iCompare > 0)
		return 1;
	else if (iCompare < 0)
		return -1;
	else 
	{
		// Compare with line nb	
		iCompare = MEM_gai_SnapShotAllocatedInLine[iIndex2] - MEM_gai_SnapShotAllocatedInLine[iIndex1];
		if (iCompare > 0)
			return -1;
		else if (iCompare < 0)
			return 1;
		else
			return 0;
	}
}


int MEM_iCompareHistoryData(const void *_pIndex1, const void *_pIndex2)
{
	int iIndex1,iIndex2,iCompare,iAddress1,iAddress2;
	
	MEM_ASSERT_CRITICAL();
	
	iIndex1 = *((int *)_pIndex1);
	iIndex2 = *((int *)_pIndex2);
	iAddress1 = MEM_gap_AllocationHistoryTable[iIndex1];
	iAddress2 = MEM_gap_AllocationHistoryTable[iIndex2];
	
	// NULL pointers at end of table
	if (iAddress1 == 0)
	{
		if (iAddress2 == 0)
			return 0;
		else
			return 1;
	}
	else
	{
		if (iAddress2 == 0)
			return -1;
	}
	
	// Separate basic and virtual allocs
	if (MEM_gab_VirtualAllocation[iIndex1] != MEM_gab_VirtualAllocation[iIndex2])
		return MEM_gab_VirtualAllocation[iIndex1] - MEM_gab_VirtualAllocation[iIndex2];

	iCompare = L_strcmp(MEM_gapsz_AllocatedInFile[iIndex1],MEM_gapsz_AllocatedInFile[iIndex2]);
	
	// Compare with file name	
	if (iCompare > 0)
		return 1;
	else if (iCompare < 0)
		return -1;
	else 
	{
		// Compare with line nb	
		iCompare = MEM_gai_AllocatedInLine[iIndex2] - MEM_gai_AllocatedInLine[iIndex1];
		if (iCompare > 0)
			return -1;
		else if (iCompare < 0)
			return 1;
		else
			return 0;
	}
}

int MEM_iComparePageMissData(const void *_pIndex1, const void *_pIndex2)
{
	int iIndex1,iIndex2,iCompare;
	
	MEM_ASSERT_CRITICAL();
	
	iIndex1 = *((int *)_pIndex1);
	iIndex2 = *((int *)_pIndex2);
		
	iCompare = L_strcmp(MEM_gas_WorstPageMissFile[iIndex1],MEM_gas_WorstPageMissFile[iIndex2]);
	
	// Compare with file name	
	if (iCompare > 0)
		return 1;
	else if (iCompare < 0)
		return -1;
	else 
	{
		// Compare with line nb	
		iCompare = MEM_gai_WorstPageMissLine[iIndex2] - MEM_gai_WorstPageMissLine[iIndex1];
		if (iCompare > 0)
			return -1;
		else if (iCompare < 0)
			return 1;
		else
			return 0;
	}
}

int MEM_iCompareWithAllocationInc(const void *_pIndex1, const void *_pIndex2)
{
	int iIndex1,iIndex2;
	
	MEM_ASSERT_CRITICAL();
	
	iIndex1 = *((int *)_pIndex1);
	iIndex2 = *((int *)_pIndex2);
		
	// Compare with allocation increment
    return ((iIndex1 == -1) ? 
                (iIndex2 == -1 ? 0 : 1) : 
                ((iIndex2 == -1) ? -1 : MEM_gai_AllocationIncrement[iIndex1] - MEM_gai_AllocationIncrement[iIndex2]));
}

int MEM_iCompareSnapShotWithAllocationInc(const void *_pIndex1, const void *_pIndex2)
{
	int iIndex1,iIndex2;
	
	MEM_ASSERT_CRITICAL();
	
	iIndex1 = *((int *)_pIndex1);
	iIndex2 = *((int *)_pIndex2);
		
	// Compare with allocation increment
    return ((iIndex1 == -1) ? 
                (iIndex2 == -1 ? 0 : 1) : 
                ((iIndex2 == -1) ? -1 : MEM_gai_SnapShotAllocationIncrement[iIndex1] - MEM_gai_SnapShotAllocationIncrement[iIndex2]));
}

#define MEM_Cul_PrintHistoryModeDetail      0x1    
#define MEM_Cul_PrintHistoryModeSummary     0x2    
#define MEM_Cul_PrintHistoryModeWorstPage   0x4    

ULONG MEM_gul_PrintHistoryMode=-1;

#else // MEM_USE_HISTORY
void MEM_CheckAllocatedBlocks(char _c_Dump) {}
#endif // MEM_USE_HISTORY


void MEM_vRemoveAllocFromHistory(void * _pBlockAddress)
{
	MEM_ASSERT_BLOCK_ADDRESS(_pBlockAddress);
	
#ifdef MEM_USE_HISTORY
	{
	ULONG i_Key = MEM_iGetKeyFromAddress(_pBlockAddress);

	MEM_gap_AllocationHistoryTable[i_Key] = 0;
	MEM_g_nbpointers--;
	}
#endif //MEM_USE_HISTORY

    MEM_vRemoveSpyAlloc(_pBlockAddress);
}


void MEM_vAddAllocToHistory(MEM_tdst_MainStruct *_pMem,ULONG _uUserAddress,ULONG _uEndAddress,char *_psz_File, int _i_Line)
{	
	MEM_ASSERT_USER_ADDRESS(_uUserAddress);
#ifdef MEM_USE_HISTORY

	/*
	 * If asserts here, the allocation history buffer is full, if you continue, it
	 * will be cleared, but invalid holes may be detected when exiting
	 */

	ERR_X_Assert(MEM_g_nbpointers < MEM_Cul_AllocationHistorySize);
	
	if(MEM_g_nbpointers >= MEM_Cul_AllocationHistorySize)
	{
		//ERR_X_ForceError("Memory allocation history buffer is full and will be cleared", NULL);
		_print("Simple Info: Memory allocation history buffer is full and will be cleared\n");
		MEM_g_nbpointers = 0;
	}
	else
	{
		ULONG i_Key;
		ULONG i_FirstKey;

		/* Fills the history table with the new pointer */
		i_Key = _uUserAddress;
		i_Key = i_Key % MEM_Cul_AllocationHistorySize;

		i_FirstKey = i_Key;
		// Find first free slot before the default slot. 
		// Go backward in order to accelerate search 
		// where the address is somewhere in the block (page miss search).
		while(MEM_gap_AllocationHistoryTable[i_Key])
		{
			i_Key = (i_Key - 1) % MEM_Cul_AllocationHistorySize;
			ERR_X_ErrorAssert(i_FirstKey!=i_Key,"MEM_Cul_AllocationHistorySize too short",0);
		}
		
		MEM_gapsz_AllocatedInFile[i_Key] = _psz_File;
		MEM_gai_AllocatedInLine[i_Key] = _i_Line;
		MEM_gap_AllocationHistoryTable[i_Key] = _uUserAddress;
		MEM_gap_AllocationHistoryTableEnd[i_Key] = _uEndAddress;
		
		MEM_gab_VirtualAllocation[i_Key] = _pMem->bVirtual;
		
		MEM_gai_AllocationIncrement[i_Key] = MEM_giAllocationIncrement++;
        if (MEM_giAllocationIncrementBreakPoint == MEM_gai_AllocationIncrement[i_Key])
        {
            _print("breakpoint allocation increment");
        }
		//MEM_gap_FreeHistoryTable[i_Key] = 0;
		MEM_g_nbpointers++;
	}
#endif // MEM_USE_HISTORY

    MEM_vAddSpyAlloc(_psz_File, _i_Line,(void *)MEM_pGetBlockAddress(_uUserAddress));

#ifdef MEM_USE_HISTORY
    if (MEM_gb_SnapShotMaxMemUsed && (MEM_ulGetTotalAllocatedMem() > MEM_gul_SnapShotMaxMemSize + MEM_gul_SnapShotDeltaSize))
	{
        MEM_gul_SnapShotMaxMemSize = MEM_ulGetTotalAllocatedMem();
        MEM_vSaveSnapShot();
    }
#endif // MEM_USE_HISTORY
}
#else /* MEM_SPY */

#define MEM_vAddAllocToHistory(_pMem,_uUserAddress,_uEndAddress,_psz_File, _i_Line)
#define MEM_vRemoveAllocFromHistory(_pBlockAddress)


#endif /* MEM_SPY */


void MEM_PrintHistory(void)
{
#ifdef MEM_USE_HISTORY
	int i, iIndex;
	ULONG *pAddress;
	
	char *sCurrentFile;
	int iCurrentLine;
	int iCurrentSize;
	int iSizeTotal;
	int iNbAlloc;
	int iPageMissNb;
	int iPageMissTime;
	int iPageMissNbTotal;
	int iPageMissTimeTotal;
	int bVirtual;
	
	MEM_ASSERT_CRITICAL();
	
	for(i=0; i<MEM_Cul_AllocationHistorySize; i++)
	{
		MEM_gap_SortTable[i] = i;
	}
	
	qsort(MEM_gap_SortTable,MEM_Cul_AllocationHistorySize,sizeof(ULONG),&MEM_iCompareHistoryData);
	
	if(MEM_gul_PrintHistoryMode & MEM_Cul_PrintHistoryModeSummary)
    {
    	// Gather allocs by 3 first letters of file name
    	_print("\n");	
    	_print("ALLOCATION SUMMARY :\n");
    	sCurrentFile = NULL;
    	iCurrentLine = 0;
    	bVirtual = 0;
    	
    	for(i=0; i<MEM_Cul_AllocationHistorySize; i++)
    	{
    		iIndex = MEM_gap_SortTable[i];
    		
    		pAddress = (ULONG*)MEM_gap_AllocationHistoryTable[iIndex];
    			
    		if(pAddress) 
    			iCurrentSize = MEM_iGetBlockSize(MEM_pGetBlockAddress(pAddress));
    		
    		if (pAddress && sCurrentFile && 0 == strncmp(MEM_gapsz_AllocatedInFile[iIndex],sCurrentFile,3))
    		{
    			iSizeTotal += iCurrentSize;		
    			iNbAlloc++;
    		}
    		else
    		{
    			char sName[10];
    			if (sCurrentFile)
    			{
    				sName[0] = sCurrentFile[0];
    				sName[1] = sCurrentFile[1];
    				sName[2] = sCurrentFile[2];
    				sName[3] = '\0';
    				if (bVirtual)
    					L_strcat(sName,"(ARAM)");
    				else
    					L_strcat(sName,"(RAM)");
    					
    				ERR_X_Assert(iSizeTotal <MEM_MAX_SIZE);
    					
    				_print("%s\t%d\t%d\n",sName,iSizeTotal,iNbAlloc);
    			}
    				
    			if(!pAddress) 
    				break;
    				
    			// New data
    			sCurrentFile = MEM_gapsz_AllocatedInFile[iIndex];
    			iSizeTotal = iCurrentSize;
    			bVirtual = MEM_gab_VirtualAllocation[iIndex];
    			iNbAlloc = 1;
    		}
    		
    	}
   	}
	
	if(MEM_gul_PrintHistoryMode & MEM_Cul_PrintHistoryModeDetail)
    {
    	// Gather allocs by file name and line number
    	_print("\n");	
    	_print("ALLOCATION DETAILS :\n");
    	_print("Page Miss Log Frame Nb : %d\n",MEM_iLogFrameNb);
    	_print("file(line) size alloc_nb page_miss_time page_miss_nb page_miss_per_size\n");
    	
    	sCurrentFile = NULL;
    	iCurrentLine = 0;
    	for(i=0; i<MEM_Cul_AllocationHistorySize; i++)
    	{
    		iIndex = MEM_gap_SortTable[i];
    		
    		pAddress = (ULONG*)MEM_gap_AllocationHistoryTable[iIndex];
    		if(pAddress) 
    		{
    			iCurrentSize = MEM_iGetBlockSize(MEM_pGetBlockAddress(pAddress));
    			iPageMissNb = MEM_gai_PageMissNb[iIndex];
    			iPageMissTime = MEM_gai_PageMissTime[iIndex];
    		}
    		
    		if (pAddress && (MEM_gai_AllocatedInLine[iIndex] == iCurrentLine) && (0 == L_strcmp(MEM_gapsz_AllocatedInFile[iIndex],sCurrentFile)))
    		{
    			iSizeTotal += iCurrentSize;
    			iPageMissNbTotal += iPageMissNb;
    			iPageMissTimeTotal += iPageMissTime;
    			
    			iNbAlloc++;
    		}
    		else
    		{
    			if (sCurrentFile)
    			{
    				ERR_X_Assert(iSizeTotal <MEM_MAX_SIZE);
    				if (bVirtual)
    					_print("%s(%d)\t%d\t%d\t%d\t%d\t%f\n",sCurrentFile,iCurrentLine,iSizeTotal,iNbAlloc,iPageMissTimeTotal,iPageMissNbTotal,1000.f*(float)iPageMissTimeTotal/(((float)(iSizeTotal) * (float)(MEM_iLogFrameNb))));
    				else
    					_print("%s(%d)\t%d\t%d\n",sCurrentFile,iCurrentLine,iSizeTotal,iNbAlloc);
    			}
    				
    			if(!pAddress) 
    				break;

    			sCurrentFile = MEM_gapsz_AllocatedInFile[iIndex];
    			iCurrentLine = MEM_gai_AllocatedInLine[iIndex];
    			bVirtual = MEM_gab_VirtualAllocation[iIndex];

    			iSizeTotal = iCurrentSize;
    			iNbAlloc = 1;
    			iPageMissNbTotal = iPageMissNb;
    			iPageMissTimeTotal = iPageMissTime;
    		}
    	}
    }	
	
	// Worst CPU peak during one frame because of ARAM page miss.
	
	for(i=0; i<MEM_gi_WorstPageMissSize; i++)
	{
		MEM_gap_SortTable[i] = i;
	}
		
	qsort(MEM_gap_SortTable,MEM_gi_WorstPageMissSize,sizeof(ULONG),&MEM_iComparePageMissData);
	
	
	if(MEM_gul_PrintHistoryMode & MEM_Cul_PrintHistoryModeWorstPage)
    {
    	// Gather page miss by file name and line number
    	_print("\n");	
    	_print("WORST PAGE MISS :\n");
    	_print("file(line) page_miss_time page_miss_nb\n");
    	
    	sCurrentFile = NULL;
    	iCurrentLine = 0;
    	for(i=0; i<MEM_gi_WorstPageMissSize; i++)
    	{
    		iIndex = MEM_gap_SortTable[i];
    		
    		iPageMissTime = MEM_gai_WorstPageMissTime[iIndex];
    		
    		if ((MEM_gai_WorstPageMissLine[iIndex] == iCurrentLine) && (0 == L_strcmp(MEM_gas_WorstPageMissFile[iIndex],sCurrentFile)))
    		{
    			iPageMissTimeTotal += iPageMissTime;
    			iPageMissNb++;
    		}
    		else
    		{
    			if (sCurrentFile)
    			{
    				_print("%s(%d)\t%d\t%d\n",sCurrentFile,iCurrentLine,iPageMissTimeTotal,iPageMissNb);
    			}
    				
    			sCurrentFile = MEM_gas_WorstPageMissFile[iIndex];
    			iCurrentLine = MEM_gai_WorstPageMissLine[iIndex];

    			iPageMissNb = 1;
    			iPageMissTimeTotal = iPageMissTime;
    		}
    	}
    	
    		
    	if (sCurrentFile)
    	{
    		_print("%s(%d)\t%d\t%d\n",sCurrentFile,iCurrentLine,iPageMissTimeTotal,iPageMissNb);
    	}
    	MEM_vInitLogHeapState();
  	}
#endif // MEM_USE_HISTORY
}

/*
 =======================================================================================================================
    Aim:    Remove element from _pMem->ap_PowerArray and element chain .
 =======================================================================================================================
 */
void MEM_vRemoveElementFromPowerArray(MEM_tdst_MainStruct *_pMem,MEM_tdst_Element *_pElement)
{
	int iIndexInPowerArray;
	MEM_tdst_Element *pElement;
	
	MEM_ASSERT_CRITICAL();
	
	iIndexInPowerArray = MEM_iGetIndexInPowerArray(_pElement->u_Size);
	pElement = _pMem->ap_PowerArray[iIndexInPowerArray];
	
	if (pElement == _pElement)
	{
		// Element is the first in power array -> remove it.
		_pMem->ap_PowerArray[iIndexInPowerArray] = _pElement->pst_NextElement;
		if (_pMem->ap_PowerArray[iIndexInPowerArray])
			_pMem->ap_PowerArray[iIndexInPowerArray]->pst_PrevElement = NULL;
			
		_pElement->pst_PrevElement = NULL;
		_pElement->pst_NextElement = NULL;
	}
	else
	{
		// Restore links of the neighbours.
		if (_pElement->pst_NextElement)
			_pElement->pst_NextElement->pst_PrevElement = _pElement->pst_PrevElement;
		_pElement->pst_PrevElement->pst_NextElement = _pElement->pst_NextElement;
	}	
	
	// Put back element in memory pool.
	MEM_vPutBackElement(_pElement);
}

void *MEM_pRemoveFirstBlockFromElement(MEM_tdst_MainStruct *_pMem,MEM_tdst_Element *_pElement)
{
	void *pBlock,*pNextBlock;
		
	MEM_ASSERT_CRITICAL();
	
	pBlock = _pElement->p_BlockList;
	
	 // Size from element and block must be equal.
	ERR_X_Assert(_pElement->u_Size == MEM_iGetBlockSize(pBlock));
	
	pNextBlock = (void *)MEM_pGetNextBlock(pBlock);
	
	if (pNextBlock == NULL)
	{
		// Element is empty, remove it.
		MEM_vRemoveElementFromPowerArray(_pMem,_pElement);
	}
	else
	{
		// Restore links in chained list.
		_pElement->p_BlockList = pNextBlock;
		MEM_pGetPrevBlock(pNextBlock) = (unsigned int)NULL;
	}		
		
	_pMem->ul_HolesStatic--;
	
	return pBlock;
}

void *MEM_pRemoveBestFitBlockFromElement(MEM_tdst_MainStruct *_pMem,MEM_tdst_Element *_pElement)
{
	void *pBlock,*pNextBlock,*pBestBlock;
		
	MEM_ASSERT_CRITICAL();
	
	pBlock = _pElement->p_BlockList;
	pBestBlock = pBlock;
	
	while (pBlock)
	{
		if (pBlock < pBestBlock)
			pBestBlock = pBlock;
		
		pBlock = (void *)MEM_pGetNextBlock(pBlock);
	}
	
	if (pBestBlock == _pElement->p_BlockList)
	{
		// Best element is first
		pNextBlock = (void *)MEM_pGetNextBlock(pBestBlock); 
		if (NULL == pNextBlock)
		{
			// Element is empty, remove it.
			MEM_vRemoveElementFromPowerArray(_pMem,_pElement);
		}
		else
		{
			// Restore links in chained list.
			_pElement->p_BlockList = pNextBlock;
			MEM_pGetPrevBlock(pNextBlock) = (unsigned int)NULL;
		}
	}
	else
	{
		void *pPrevBlock,*pNextBlock;
		
		// Remove block from chained list.
		pPrevBlock = (void *)MEM_pGetPrevBlock(pBestBlock);
		pNextBlock = (void *)MEM_pGetNextBlock(pBestBlock);
		
		ERR_X_Assert(pPrevBlock);
		MEM_pGetNextBlock(pPrevBlock) = (ULONG)pNextBlock;
		if (pNextBlock)
			MEM_pGetPrevBlock(pNextBlock) = (ULONG)pPrevBlock;
	}
		
	_pMem->ul_HolesStatic--;
	
	return pBestBlock;
}


void MEM_vRemoveFreeBlock(MEM_tdst_MainStruct *_pMem,void *_pFreeBlock)
{
	MEM_tdst_Element *pElement;
	ULONG uSize;
	
	MEM_ASSERT_CRITICAL();
	
	ERR_X_Assert(MEM_bIsBlockFree(_pFreeBlock));
	
	uSize = MEM_iGetBlockSize(_pFreeBlock);
	
	// Search for element that corresponds to this size.
	pElement = _pMem->ap_PowerArray[MEM_iGetIndexInPowerArray(uSize)];
	while (pElement && (pElement->u_Size < uSize))
		pElement = pElement->pst_NextElement;
	
	// The free block should belong to an element. 
	ERR_X_Assert(pElement && (pElement->u_Size == uSize));
			
	if (pElement->p_BlockList == _pFreeBlock)
	{
		MEM_pRemoveFirstBlockFromElement(_pMem,pElement);
	}
	else
	{
		void *pPrevBlock,*pNextBlock;
		
		// Remove block from chained list.
		pPrevBlock = (void *)MEM_pGetPrevBlock(_pFreeBlock);
		pNextBlock = (void *)MEM_pGetNextBlock(_pFreeBlock);
		
		ERR_X_Assert(pPrevBlock);
		MEM_pGetNextBlock(pPrevBlock) = (ULONG)pNextBlock;
		if (pNextBlock)
			MEM_pGetPrevBlock(pNextBlock) = (ULONG)pPrevBlock;
			
		_pMem->ul_HolesStatic--;
	}
}


/*$4
 ***********************************************************************************************************************
    ALLOC TMP
 ***********************************************************************************************************************
 */

#ifndef _GAMECUBE
char	*MEM_gp_AllocTmpFirst = NULL;
char	*MEM_gp_AllocTmpLast = NULL;
char	*MEM_gp_AllocTmpNext = NULL;
int		MEM_gi_AllocTmpNb = 0;
#endif // _GAMECUBE

BOOL	MEM_gb_EnableTmp = FALSE;

/*void *MEM_p_AllocTmp(ULONG ulSize)
{
	return MEM_p_Alloc(ulSize);
}

void MEM_FreeTmp(void *p)
{
	MEM_Free(p);
}

void *MEM_p_ReallocTmp(void *_pv_Block, ULONG _ul_BlockSize)
{
	return MEM_p_Realloc(_pv_Block, _ul_BlockSize);
}

void MEM_ResetTmpMemory() 
{
}*/

#ifdef _GAMECUBE
#ifndef _RVL
MEM_tdst_MainStruct *TmpHeap = &MEM_gst_GCMemoryInfo;
#else
MEM_tdst_MainStruct *TmpHeap = &MEM_gst_RVLMEMTMP;
#endif

#endif

void *MEM_p_AllocTmp(ULONG ulSize)
{
#ifdef ACTIVE_EDITORS
	ulSize += 128;
	ulSize &= ~63;
	return MEM_p_Alloc(ulSize);

#else // ACTIVE_EDITORS
#ifdef _GAMECUBE
	return MEM_p_AllocAlignMem(ulSize,64,TmpHeap);
#else // _GAMECUBE
	char	*p;
	ULONG	*p1;
	
	ulSize += 128;
	ulSize &= ~63;
	if(!MEM_gp_AllocTmpFirst || !MEM_gb_EnableTmp) return MEM_p_AllocAlign(ulSize, 64);
	if(MEM_gp_AllocTmpNext + ulSize >= MEM_gp_AllocTmpLast) return MEM_p_AllocAlign(ulSize, 64);

	MEM_M_BeginCriticalSection(&MEM_gst_MemoryInfo);
	p = MEM_gp_AllocTmpNext +
	4;
	p1 = (ULONG *) MEM_gp_AllocTmpNext;
	*p1 = ulSize;
	MEM_gp_AllocTmpNext += ulSize;
	MEM_gi_AllocTmpNb++;
	MEM_M_EndCriticalSection(&MEM_gst_MemoryInfo);

	return p;
#endif // _GAMECUBE
#endif // ACTIVE_EDITORS
}

void MEM_FreeTmp(void *p)
{
#ifdef _GAMECUBE
	MEM_Free(p);
#else  // _GAMECUBE
	ULONG	*p1, size;
#if defined(PSX2_TARGET)
	* (ULONG *) &p &= 0x0fffffff;
#endif

	p1 = (ULONG *) p;
	p1--;
	size = *p1;

	MEM_M_BeginCriticalSection(&MEM_gst_MemoryInfo);
	if((char *) p + size - 4 == MEM_gp_AllocTmpNext) MEM_gp_AllocTmpNext = (char *) p1;

	MEM_gi_AllocTmpNb--;
	MEM_M_EndCriticalSection(&MEM_gst_MemoryInfo);

	if(!MEM_gi_AllocTmpNb)
	{
		MEM_ResetTmpMemory();
	}
#endif // _GAMECUBE
}

void *MEM_p_ReallocTmp(void *_pv_Block, ULONG _ul_BlockSize)
{
#ifdef _GAMECUBE
	return MEM_p_ReallocAlignMem(_pv_Block,_ul_BlockSize,64,TmpHeap);
#else // _GAMECUBE
	char	*p;
    ULONG size;

    size = *(((ULONG*)_pv_Block)-1);
	
    p = (char *)MEM_p_AllocTmp(_ul_BlockSize);
	L_memmove(p, _pv_Block, size);
	MEM_Free(_pv_Block);

	return p;
#endif // _GAMECUBE
}

void MEM_ResetTmpMemory(void)
{
	extern void SOFT_ZList_Init(void);
	extern void SOFT_ZList_Clear(void);

#ifndef _GAMECUBE
	MEM_M_BeginCriticalSection(TmpHeap);
#ifdef _RVL
	MEM_vReinitHeap(TmpHeap);
#endif // _RVL
	MEM_gp_AllocTmpNext = MEM_gp_AllocTmpFirst;
	MEM_gi_AllocTmpNb = 0;
	MEM_M_EndCriticalSection(TmpHeap);
	
	SOFT_ZList_Init();
	SOFT_ZList_Clear();
#endif // _GAMECUBE

}

#define PRELOADTEXGRAN (40 * 1024)

/*
 =======================================================================================================================
  Aim:  Allocates a block of memory 
 =======================================================================================================================
 */
	extern int		gi_CurBin;
	extern int		gai_CurSize[];
	extern int 		gi_PreloadReadSize;
	extern char		*gap_FirstPointer[];	
 
#ifdef MEM_SPY
void *_MEM_p_Alloc(MEM_tdst_MainStruct *_pMem,ULONG _ul_BlockSize, char *_psz_File, int _i_Line)
#else
void *_MEM_p_Alloc(MEM_tdst_MainStruct *_pMem,ULONG _ul_BlockSize)
#endif
{
	ULONG uSize,i,uBlockSize;
	MEM_tdst_Element *pElement;
	void *pCurrentBlock;
	void *pUserAddress;
	u32 uFreeSize;
#ifndef _RVL
	MEM_tdst_MainStruct *lastChanceHeap = &MEM_gst_MemoryInfo;
#else
	MEM_tdst_MainStruct *lastChanceHeap = _pMem->pLastResort;
#endif // _RVL

#ifdef MEM_DONT_USE_VIRTUAL_MEMORY
	_pMem = &MEM_gst_MemoryInfo;
#endif // MEM_DONT_USE_VIRTUAL_MEMORY

	MEM_vConditionnalCheckMem(_pMem);
	
#if defined(PSX2_TARGET) || defined(_GAMECUBE) || defined(_XBOX) || defined(_XENON)
	if(_ul_BlockSize == 0) 
		return NULL;
#else
	ERR_X_Assert(_ul_BlockSize > 0);
#endif

	_GSP_BeginRaster(56);
	
	// Real begin of alloc
	
	MEM_M_BeginCriticalSection(_pMem);
		
	// Compute size that we really want to alloc.
	uSize = MEM_iComputeNeededBlockSize(_ul_BlockSize);

	// Find smallest element bigger or equal to this size.
	i = MEM_iGetIndexInPowerArray(uSize);
	pElement = _pMem->ap_PowerArray[i];
	
	// Search in _pMem->ap_PowerArray[i] element list (size between 2^i and 2^(i+1)-1).
	while (pElement && (pElement->u_Size < uSize) )
		pElement = pElement->pst_NextElement;
		
	if (!pElement)
	{
		// Search in other _pMem->ap_PowerArray[].
		i++;
		while ((i < MEM_C_PowerArraySize) && ((pElement = _pMem->ap_PowerArray[i]) == NULL))
			i++;
	}
	
	if (pElement == NULL)
	{
#ifdef _GAMECUBE
		// If there is no more space in special RAM, we can allocate in basic RAM.
		if (lastChanceHeap && (_pMem != lastChanceHeap))
		{
			MEM_M_EndCriticalSection(_pMem);
			
#ifndef _FINAL_
			{
				/*if ((_pMem == &MEM_gst_SoundMemoryInfo))
				{
					OSReport("No more sound RAM for alloc of %d bytes, we have %d\n",_ul_BlockSize,MEM_iGetBiggestFreeSize(&MEM_gst_SoundMemoryInfo));
				}*/
				/*else if ((_pMem == &MEM_gst_GCMemoryInfo))
				{
					OSReport("No more GC RAM for alloc of %d bytes, we have %d\n",_ul_BlockSize,MEM_iGetBiggestFreeSize(&MEM_gst_GCMemoryInfo));
				}
				else if ((_pMem == &MEM_gst_VirtualMemoryInfo))
				{
					OSReport("No more VM RAM for alloc of %d bytes, we have %d\n",_ul_BlockSize,MEM_iGetBiggestFreeSize(&MEM_gst_VirtualMemoryInfo));
				}*/
#ifdef _RVL				
#ifdef MEM_SPY
				OSReport("No more RAM on '%s' for alloc of %d bytes, we have %d (%s l.%u)\n", _pMem->pcHeapName?_pMem->pcHeapName:"",
						_ul_BlockSize,MEM_iGetBiggestFreeSize(_pMem), _psz_File, _i_Line);
#else
				OSReport("No more RAM on '%s' for alloc of %d bytes, we have %d\n", _pMem->pcHeapName?_pMem->pcHeapName:"",
						_ul_BlockSize,MEM_iGetBiggestFreeSize(_pMem));
#endif // MEM_SPY
#endif // _RVL
			}
#endif // _FINAL_

#ifdef MEM_SPY
			return _MEM_p_Alloc(lastChanceHeap, _ul_BlockSize, _psz_File,  _i_Line);
#else
			return _MEM_p_Alloc(lastChanceHeap, _ul_BlockSize);
#endif
		}
		else
#endif //_GAMECUBE
		{
			MEM_M_EndCriticalSection(_pMem);
			MEM_M_SignalFatalError(_ul_BlockSize, MEM_ERR_Csz_NoMoreMemoryToPerformAllocation);
		}
	}
	
	// Get free block from pElement.
	uBlockSize = pElement->u_Size;
#ifdef MEM_BEST_FIT	
	pCurrentBlock = MEM_pRemoveBestFitBlockFromElement(_pMem,pElement);	
#else // MEM_BEST_FIT	
	pCurrentBlock = MEM_pRemoveFirstBlockFromElement(_pMem,pElement);
#endif // MEM_BEST_FIT	
	
	uFreeSize = uBlockSize - uSize;
	
	// If block is bigger than what we need, 
	// create smaller free block with remaining memory, 
	// if it is at least 16 bytes.
	// Else, make a bigger alloc in order not leave memory out of blocks.
	if (uFreeSize >= MEM_MIN_SIZE)
		MEM_vCreateFreeBlock(_pMem,((unsigned char *)pCurrentBlock)+uSize,uFreeSize);
	else
		uSize += uFreeSize;
	_pMem->ul_DynamicCurrentAllocated += uSize;	
	
		
	// Create allocated block.
	MEM_vCreateAllocatedBlock(pCurrentBlock, uSize, 0);
	
	pUserAddress = MEM_pGetUserAddress(pCurrentBlock);
	

#ifdef MEM_FILL_ALLOCATED_BLOCKS
	L_memset(pUserAddress,MEM_ALLOC_FILL_DATA,uSize-MEM_LOST_SIZE);
#endif 	
	
	// End of alloc. The following code is for verification and debug

	_GSP_EndRaster(56);

	MEM_vAddAllocToHistory(_pMem,
		(ULONG)MEM_pGetUserAddress(pCurrentBlock),
		(ULONG)pCurrentBlock + uSize,
		_psz_File,
		 _i_Line);
		 

	MEM_UPDATE_MEASURES(_pMem);

	/* Update du pointeur "haut" dynamique de la mémoire */
	_pMem->pv_DynamicNextFree = (void *)((unsigned int)_pMem->pv_DynamicBloc + ((_pMem)->ul_DynamicMaxSize - MEM_uGetLastBlockSize(_pMem)));

	MEM_vConditionnalCheckMem(_pMem);
	MEM_vCheckAllocatedBlock(pCurrentBlock);
	MEM_M_EndCriticalSection(_pMem);
		
#ifndef _FINAL_
	{
		char 	*pLast;
		ULONG	ulTest;

		ulTest = (_pMem->ul_DynamicMaxSize > 32 * 1024 * 1024) ? _pMem->ul_DynamicMaxSize - (32 * 1024 * 1024) : _pMem->ul_DynamicMaxSize;
		pLast = (char *)_pMem->pv_DynamicBloc + ulTest;
		MEM_gi_Delta = (int) pLast - (int) _pMem->pv_DynamicNextFree;		
		if(MEM_gi_Delta < MEM_gi_MinDelta)
		{
			MEM_gi_MinDelta = MEM_gi_Delta;
		}								
	}
	
	// Pour avoir le max de consommation dans la map.
	if (_pMem == &MEM_gst_MemoryInfo)
	{
		static s32 uiMaxAlloc = 0;
		static s32 uiMinBiggestFreeBlock = 0x7FFFFFFF;
		s32 uiBiggestFreeBlock = MEM_iGetBiggestFreeSize(&MEM_gst_MemoryInfo);
		
		if (uiBiggestFreeBlock < uiMinBiggestFreeBlock)
			uiMinBiggestFreeBlock = uiBiggestFreeBlock;
		if ((int)MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated > uiMaxAlloc)
			uiMaxAlloc = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
#ifdef JADEFUSION			
		if (((ULONG)uiMaxAlloc > MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated + 5000000)
#else
			if ((uiMaxAlloc > (int)(MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated + 5000000))
#endif
			|| (uiMinBiggestFreeBlock < uiBiggestFreeBlock - 3000000))
		{
			_print("Max RAM allocated : %d/%d\n",uiMaxAlloc,MEM_gst_MemoryInfo.ul_DynamicMaxSize);
			_print("Min biggest free block : %d\n",uiMinBiggestFreeBlock);
			uiMaxAlloc = MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated;
			uiMinBiggestFreeBlock = uiBiggestFreeBlock;
		}		
	}
	
#endif // _FINAL_
	

	/* Ai-je du preload ? si oui, je dois check que j'écrase pas tout comme un gros porc */
	if(gi_CurBin || gai_CurSize[0] )
	{
		int i;
		
		for(i = 0; i <= 4; i++)
		{		
			if(!gai_CurSize[i]) continue;
			
			if(gap_FirstPointer[i] - gai_CurSize[i] < (char *)pCurrentBlock + uSize + 12 /*12 = Next Free Bloc Header */)
			{
				/* Ai-je besoin de virer l'intégralité du bin preload */
				if(gap_FirstPointer[i] < (char *)pCurrentBlock + uSize + 12)
				{
					gap_FirstPointer[i] = 0;
					gai_CurSize[i] = 0;
				
					gi_CurBin --;
#ifndef _FINAL_
					printf("Preload (%i) totally canceled ... Data overwriten by classical Alloc\n", i);
#endif				
				}
				else
				{
					int		MaxSizeForPreload, BlocksToKeep;

					MaxSizeForPreload = gap_FirstPointer[i] - ((char *)pCurrentBlock + uSize + 12);
					
					BlocksToKeep = MaxSizeForPreload / PRELOADTEXGRAN;

					gai_CurSize[i] = BlocksToKeep * PRELOADTEXGRAN;

					if(gai_CurSize[i] <= 0)
					{
						gap_FirstPointer[i] = 0;
						gai_CurSize[i] = 0;				
						gi_CurBin --;
						
#ifndef _FINAL_
						printf("Preload (%i) totally canceled ... Data overwriten by classical Alloc\n", i);
#endif				
						
					}
#ifndef _FINAL_
					
					else
					{
						printf("Preload (%i): Some 40k Blocks were removed (Alloc overwrite). Still Preloaded = %iKo\n", i, gai_CurSize[i] / 1024);
					}
#endif									
				}
			}			
		}		
	}
	
	// We return the address for the user.
	return pUserAddress;
}



/*
 =======================================================================================================================
    Aim:    Free a block allocated with MEM_p_Alloc() or MEM_p_Realloc()
 =======================================================================================================================
 */
 
void MEM_Free(void *_pAddress)
{
	ULONG uSize;
	void *pBlockBefore, *pBlockAfter,*pBlockAddress;
	MEM_tdst_MainStruct *pMem ;
#ifdef MEM_FILL_FREE_BLOCKS
	void *pFillAddress;
	ULONG uFillSize;
#endif // MEM_FILL_FREE_BLOCKS

#ifdef _DEBUG
    {
        static ULONG ulDebugAddress = (ULONG)-1;
        if (ulDebugAddress == (ULONG)_pAddress)
            ulDebugAddress = 0;
    }
#endif // _DEBUG

#if defined(PSX2_TARGET)
	* (ULONG *) &_pAddress &= 0x0fffffff;
#endif


	pMem = MEM_pChooseMemFromAddress(_pAddress);

	if(!_pAddress)
		return;
		
	MEM_vConditionnalCheckMem(pMem);
		
	// Pointeur temporaire ?
	if(MEM_IsTmpPointer((char *) _pAddress))
	{
		MEM_FreeTmp(_pAddress);
		return;
	}
	
	
	_GSP_BeginRaster(57);		
	
	// Real begin of free.
	
	MEM_M_BeginCriticalSection(pMem);
	MEM_vCheckAllocatedBlock(MEM_pGetBlockAddress(_pAddress));
	
	// Get block address.
	pBlockAddress = (void *)MEM_pGetBlockAddress(_pAddress);
		
	// Are we really freeing an allocated block ?
	ERR_X_Assert(!MEM_bIsBlockFree(pBlockAddress));
	
	MEM_vRemoveAllocFromHistory(pBlockAddress);
	
	// Get block size
	uSize = MEM_iGetBlockSize(pBlockAddress);
	pMem->ul_DynamicCurrentAllocated -= uSize ;	
	
#ifdef MEM_FILL_FREE_BLOCKS
	pFillAddress = ((ULONG *)pBlockAddress)+3;
	uFillSize = uSize-MEM_MIN_SIZE;
#endif // MEM_FILL_FREE_BLOCKS		
		
	// Get previous and next blocks, and remove them from the lists if they are free.
	// Change size and address of new free block accordingly.
	pBlockAfter = MEM_pGetBlockAfter(pBlockAddress);
	if (MEM_bIsBlockFree(pBlockAfter))
	{
		uSize += MEM_iGetBlockSize(pBlockAfter);
		MEM_vRemoveFreeBlock(pMem,pBlockAfter);
#ifdef MEM_FILL_FREE_BLOCKS
		uFillSize += MEM_MIN_SIZE;
#endif // MEM_FILL_FREE_BLOCKS		
	}
	
	if (MEM_bIsBlockBeforeFree(pBlockAddress))
	{
#ifdef MEM_FILL_FREE_BLOCKS
		pFillAddress = ((ULONG *)pBlockAddress)-1;
		uFillSize += MEM_MIN_SIZE;
#endif // MEM_FILL_FREE_BLOCKS		

		pBlockBefore = (void *)MEM_pGetBlockBefore(pBlockAddress);
		ERR_X_Assert(MEM_bIsBlockFree(pBlockBefore));
		
		uSize += MEM_iGetBlockSize(pBlockBefore);
		pBlockAddress = pBlockBefore;

		MEM_vRemoveFreeBlock(pMem,pBlockBefore);
	}
	
#ifdef MEM_FILL_FREE_BLOCKS
	L_memset(pFillAddress ,MEM_FREE_FILL_DATA ,uFillSize);
#endif // MEM_FILL_FREE_BLOCKS		
		
	// Put the new merged block in the lists.
	MEM_vCreateFreeBlock(pMem,pBlockAddress,uSize);

	MEM_vConditionnalCheckMem(pMem);
	MEM_vCheckFreeBlock(pBlockAddress);
	MEM_M_EndCriticalSection(pMem);
	
	// End of free. The following code is for verification and debug
	MEM_UPDATE_MEASURES(pMem);
	
	_GSP_EndRaster(57);	
		
}

void MEM_CheckDynamicBlocFull(void)
{
}

void MEM_Defrag(int _value)
{
}


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Reallocates a bloc of memory (allocated with MEM_p_Alloc)
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef MEM_SPY
void *_MEM_p_Realloc(MEM_tdst_MainStruct *_pMem,void *_pAddress, ULONG _ul_BlockSize, char *_psz_File, int _i_Line)
#else
void *_MEM_p_Realloc(MEM_tdst_MainStruct *_pMem,void *_pAddress, ULONG _ul_BlockSize)
#endif
{
	ULONG uSize;
	void *pBlockAddress,*pReturnAddress;

#ifdef MEM_DONT_USE_VIRTUAL_MEMORY
	_pMem = &MEM_gst_MemoryInfo;
#endif // MEM_DONT_USE_VIRTUAL_MEMORY
	
	MEM_vConditionnalCheckMem(_pMem);
	
	// Pointeur temporaire ?
	if(MEM_IsTmpPointer((char *) _pAddress))
	{
		return MEM_p_ReallocTmp(_pAddress, _ul_BlockSize);
	}
		
	if (_pAddress == NULL)
#ifdef MEM_SPY
		pReturnAddress = _MEM_p_Alloc(_pMem,_ul_BlockSize, _psz_File,  _i_Line);
#else
		pReturnAddress =  _MEM_p_Alloc(_pMem,_ul_BlockSize);
#endif
	else
	{
		MEM_tdst_MainStruct *pMem;
		
		_GSP_BeginRaster(58);	
			
		pBlockAddress = (void *)MEM_pGetBlockAddress(_pAddress);

#ifdef _GAMECUBE
        {
    		int bVMRamThenRam;
	    	pMem =  MEM_pChooseMemFromAddress(pBlockAddress);
		    bVMRamThenRam = ((_pMem == &MEM_gst_MemoryInfo) && (&MEM_gst_VirtualMemoryInfo == pMem));
#ifdef MEM_USE_HISTORY
    		if (bVMRamThenRam)
	    	{
		    	ULONG i_Key = MEM_iGetKeyFromAddress(pBlockAddress);
			    _print("Previous alloc was here : %s(%d)\n",
				    MEM_gapsz_AllocatedInFile[i_Key],
				    MEM_gai_AllocatedInLine[i_Key]);
    		}
#endif // MEM_USE_HISTORY
	    	ERR_X_Assert(!bVMRamThenRam);
        }
#else //_GAMECUBE
        pMem = _pMem;
#endif //_GAMECUBE
		
		MEM_vSlowCheckAllocatedBlock(pMem,pBlockAddress);
		if (_ul_BlockSize == 0)
		{
			MEM_Free(_pAddress);
			pReturnAddress = NULL;
		}
		else
		{	
			ULONG uAllocSize;
			s32 FreeSize; // Size of the free block after the allocated block.
		
			uAllocSize = MEM_iComputeNeededBlockSize(_ul_BlockSize);
			uSize = MEM_iGetBlockSize(pBlockAddress);
			FreeSize = uSize - uAllocSize;
	
			if (FreeSize == 0)
				pReturnAddress =_pAddress; // Same size : do nothing.
			else 
			{
				int bIsBlockAfterFree;
				void *pBlockAfter;
				u32 uFreeBlockAfterSize;
				
				MEM_M_BeginCriticalSection(pMem);	
				pBlockAfter = MEM_pGetBlockAfter(pBlockAddress);
				
				bIsBlockAfterFree = MEM_bIsBlockFree(pBlockAfter);
				if (bIsBlockAfterFree)
				{
					uFreeBlockAfterSize = MEM_iGetBlockSize(pBlockAfter);
					FreeSize += uFreeBlockAfterSize;
				}
				else
					uFreeBlockAfterSize = 0;
				
				if (FreeSize >= 0)
				{
					// Block after is a free block big enough for new size : 
					// we stay in same area.
                    MEM_vRemoveAllocFromHistory(pBlockAddress);
					
					// Remove free block after from lists.
					if (bIsBlockAfterFree)
						MEM_vRemoveFreeBlock(pMem,pBlockAfter);
										
					// Create free block with remaining free memory (if possible)
					if (FreeSize>=MEM_MIN_SIZE)
					{
						MEM_vCreateFreeBlock(pMem,((unsigned char *)pBlockAddress) + uAllocSize,FreeSize);
						pMem->ul_DynamicCurrentAllocated -= (FreeSize - uFreeBlockAfterSize);	
					}
					else
					{
						uAllocSize += FreeSize;
						pMem->ul_DynamicCurrentAllocated += uAllocSize - uSize;
					}

					// (re)Create allocated bloc.
					MEM_vCreateAllocatedBlock(pBlockAddress,uAllocSize,MEM_bIsBlockBeforeFree(pBlockAddress));
					
					MEM_vAddAllocToHistory(pMem,(ULONG)_pAddress,
							(ULONG)pBlockAddress + uAllocSize,
							_psz_File,
		 					_i_Line);
		 					
					pReturnAddress = _pAddress;
					MEM_M_EndCriticalSection(pMem);
				}
				else
				{
					// We don't have enough memory after current block, we must allocate a new block, 
					// copy the data from the current block and free it. 
					void *pNewBlock;
					MEM_M_EndCriticalSection(pMem);
					
			#ifdef MEM_SPY
					pNewBlock = _MEM_p_Alloc(pMem,uAllocSize, _psz_File,  _i_Line);
			#else
					pNewBlock = _MEM_p_Alloc(pMem,uAllocSize);
			#endif
					L_memcpy(pNewBlock, _pAddress, uSize);
					MEM_Free(_pAddress);
					
					pReturnAddress = pNewBlock;
				}
			}
		}
		MEM_vConditionnalCheckMem(pMem);
		_GSP_EndRaster(58);				
	}
	if (pReturnAddress)
		MEM_vCheckAllocatedBlock(MEM_pGetBlockAddress(pReturnAddress));
	MEM_vConditionnalCheckMem(_pMem);
	
	MEM_UPDATE_MEASURES(_pMem);
	
	return pReturnAddress;
}

/*$4
 ***********************************************************************************************************************
    ALLOC ALIGN
 ***********************************************************************************************************************
 */


#ifdef MEM_SPY
void *_MEM_p_AllocAlign(MEM_tdst_MainStruct *_pMem,ULONG _ul_BlockSize, ULONG Alignment, char *_pz_File, int _i_Line)
#else
void *_MEM_p_AllocAlign(MEM_tdst_MainStruct *_pMem,ULONG _ul_BlockSize, ULONG Alignment)
#endif
{	
	void *pAddress,*pBlockAddress,*pAlignedAddress,*pAlignedBlockAddress,*pBlockAfter;
	ULONG uSize,uFreeSizeAfter,uAllocatedSize,uNeededSize,uSizeBlockAfter,ulBeforeAlignSize;
	int bIsBlockBeforeFree,bChangeAllocStart;
	
#ifdef MEM_DONT_USE_VIRTUAL_MEMORY
	_pMem = &MEM_gst_MemoryInfo;
#endif // MEM_DONT_USE_VIRTUAL_MEMORY


	MEM_vConditionnalCheckMem(_pMem);
	
	ERR_X_Assert(Alignment >= 4);
	
	uNeededSize = MEM_iComputeNeededBlockSize(_ul_BlockSize);
	
#ifdef MEM_SPY
	pAddress = _MEM_p_Alloc(_pMem,uNeededSize + (Alignment-1) + MEM_MIN_SIZE, _pz_File, _i_Line);
#else	
	pAddress = _MEM_p_Alloc(_pMem,uNeededSize + (Alignment-1) + MEM_MIN_SIZE);
#endif	
	
	_GSP_BeginRaster(56);	
	
	_pMem = MEM_pChooseMemFromAddress(pAddress);
	 
	MEM_M_BeginCriticalSection(_pMem);
	pBlockAddress = (void *)MEM_pGetBlockAddress(pAddress);
	pBlockAfter = MEM_pGetBlockAfter(pBlockAddress);
	
	MEM_vRemoveAllocFromHistory(pBlockAddress);
	
	uSize = MEM_iGetBlockSize(pBlockAddress);
	
	bChangeAllocStart = ((ULONG)pAddress) % Alignment;
	
	if (bChangeAllocStart)
	{
		ULONG uFreeSizeBefore;
		void *pFreeBlockBefore;
		
		// We have memory before the aligned address 
		// -> make sure it is at least MEM_MIN_SIZE long, so we can create a free block, 
		// merged with the block before if it is free.
		pAlignedAddress = (void *)(Alignment * ((Alignment-1 + MEM_MIN_SIZE + (ULONG)pAddress)/Alignment));
		
		pAlignedBlockAddress = (void *)MEM_pGetBlockAddress(pAlignedAddress);
		
		ulBeforeAlignSize = ((unsigned char *)pAlignedBlockAddress) - ((unsigned char *)pBlockAddress);
				
		ERR_X_Assert(ulBeforeAlignSize >= MEM_MIN_SIZE);
		ERR_X_Assert(ulBeforeAlignSize <= Alignment*2+MEM_MIN_SIZE);
				
		uFreeSizeBefore = ulBeforeAlignSize;
		if (MEM_bIsBlockBeforeFree(pBlockAddress))
		{
			pFreeBlockBefore = (void *)MEM_pGetBlockBefore(pBlockAddress);
			
			uFreeSizeBefore += MEM_iGetBlockSize(pFreeBlockBefore);
			MEM_vRemoveFreeBlock(_pMem,pFreeBlockBefore);
		}
		else
			pFreeBlockBefore = pBlockAddress;
							
		MEM_vCreateFreeBlock(_pMem,pFreeBlockBefore,uFreeSizeBefore);
		bIsBlockBeforeFree = 1;	
	}
	else
	{
		// Memory is already aligned
		pAlignedAddress = pAddress;
		pAlignedBlockAddress = pBlockAddress;
		
		bIsBlockBeforeFree = MEM_bIsBlockBeforeFree(pBlockAddress);
		ulBeforeAlignSize = 0;
	}
	
	// If possible create a free block after with the rest of the memory, 
	// merged with the block after if it is free.
	ERR_X_Assert(uSize - ulBeforeAlignSize >= uNeededSize);
	uFreeSizeAfter = uSize - uNeededSize - ulBeforeAlignSize;
	
	// If block after is free, merge it.
	if ((uFreeSizeAfter > 0) && (MEM_bIsBlockFree(pBlockAfter)))
	{
		uSizeBlockAfter = MEM_iGetBlockSize(pBlockAfter);
		uFreeSizeAfter += uSizeBlockAfter;
		MEM_vRemoveFreeBlock(_pMem,pBlockAfter);
	}
	else
		uSizeBlockAfter = 0;
	
	if (uFreeSizeAfter >= MEM_MIN_SIZE)
	{
		MEM_vCreateFreeBlock(_pMem,((unsigned char *)pAlignedBlockAddress) + uNeededSize,uFreeSizeAfter);
		uAllocatedSize = uNeededSize;
	}
	else
	{
		uFreeSizeAfter = 0;
		uAllocatedSize = uSize - ulBeforeAlignSize;
	}
		
	ERR_X_Assert((uSize - ulBeforeAlignSize + uSizeBlockAfter) == (uAllocatedSize + uFreeSizeAfter));
	ERR_X_Assert((unsigned char *)pBlockAddress + ulBeforeAlignSize == ((unsigned char *)pAlignedBlockAddress));
	
	_pMem->ul_DynamicCurrentAllocated -= (ulBeforeAlignSize + uFreeSizeAfter - uSizeBlockAfter) ;
	
	// Create allocated block
	MEM_vCreateAllocatedBlock(pAlignedBlockAddress,uAllocatedSize,bIsBlockBeforeFree);
	
	MEM_vAddAllocToHistory(_pMem,(ULONG)pAlignedAddress,(ULONG)pAlignedBlockAddress + uAllocatedSize ,_pz_File, _i_Line);
	
	MEM_vCheckAllocatedBlock(pAlignedBlockAddress);
	MEM_vConditionnalCheckMem(_pMem);
	MEM_M_EndCriticalSection(_pMem);
	
	MEM_UPDATE_MEASURES(_pMem);
	
	_GSP_EndRaster(56);			
	
	return pAlignedAddress;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef MEM_SPY
void *_MEM_p_ReallocAlign(MEM_tdst_MainStruct *_pMem,void *_pv, ULONG _ul_BlockSize, ULONG Alignment, char*_pz_File,int _i_Line)
#else
void *_MEM_p_ReallocAlign(MEM_tdst_MainStruct *_pMem,void *_pv, ULONG _ul_BlockSize, ULONG Alignment)
#endif
{
	/*~~~~~~~~~~~~~*/
	void	*pv_temp;
	/*~~~~~~~~~~~~~*/

#ifdef MEM_DONT_USE_VIRTUAL_MEMORY
	_pMem = &MEM_gst_MemoryInfo;
#endif // MEM_DONT_USE_VIRTUAL_MEMORY
	
	MEM_vConditionnalCheckMem(_pMem);
	
#ifdef MEM_SPY
	pv_temp = _MEM_p_AllocAlign(_pMem,_ul_BlockSize, Alignment,_pz_File, _i_Line );
#else
	pv_temp = _MEM_p_AllocAlign(_pMem,_ul_BlockSize, Alignment);
#endif

	if (pv_temp)
	{		
		MEM_vCheckAllocatedBlock(MEM_pGetBlockAddress(pv_temp));
		L_memcpy(pv_temp, _pv, _ul_BlockSize);
	}
	MEM_Free(_pv);
	
	MEM_vConditionnalCheckMem(_pMem);
	
	return pv_temp;
}



/*$4
 ***********************************************************************************************************************
    special functions for allocating blocks from the end of memory area
 ***********************************************************************************************************************
 */
#ifdef PSX2_TARGET
/*$off*/
#ifdef MEM_SPY
void *_MEM_p_AllocFromEndAlign(MEM_tdst_MainStruct *_pMem,ULONG _ul_size, ULONG _i_modulo, char *_str_file, int _i_line)
#else
void *_MEM_p_AllocFromEndAlign(MEM_tdst_MainStruct *_pMem,ULONG _ul_size, ULONG _i_modulo)
#endif
/*$on*/
{
	void *pLastFreeBlock;
	void *pReturnAddress;
	ULONG uNeededSize;
	
#ifdef MEM_DONT_USE_VIRTUAL_MEMORY
	_pMem = &MEM_gst_MemoryInfo;
#endif // MEM_DONT_USE_VIRTUAL_MEMORY
	MEM_vConditionnalCheckMem(_pMem);
	if (_ul_size == 0)
		return NULL;
	

	_GSP_BeginRaster(56);		
	MEM_M_BeginCriticalSection(_pMem);
		
	ERR_X_ErrorAssert(MEM_bIsBlockBeforeFree(_pMem->pv_LastFree), MEM_ERR_Csz_NotEnoughSystemMemory, NULL);	
	
	// Get last free block
	pLastFreeBlock = (void *)MEM_pGetBlockBefore(_pMem->pv_LastFree);
	
	// Do we have enough memory in last block to make the allocation ?
	
	// Memory needed for allocation (without header)
	uNeededSize = MEM_iComputeNeededBlockSize(_ul_size) - (MEM_TOTAL_HEADER_SIZE);
	
	// Compute return address without alignement (without header)
	pReturnAddress = (void *) ((unsigned int)_pMem->pv_LastFree - (unsigned int)uNeededSize);
	
	// Align return address (without header)
	pReturnAddress = (void *)(((ULONG)(pReturnAddress)/_i_modulo) * _i_modulo);
	
	// Add header
	pReturnAddress = MEM_pGetBlockAddress(pReturnAddress);
	
	if ((unsigned int) pReturnAddress > (unsigned int)pLastFreeBlock + MEM_MIN_SIZE)
	{
		// We have enough memory for the allocation
		ULONG uFreeSize,uAllocatedSize;
		
		// Resize free block.
		MEM_vRemoveFreeBlock(_pMem,pLastFreeBlock);
		uFreeSize = (unsigned int) pReturnAddress - (unsigned int)pLastFreeBlock;
		MEM_vCreateFreeBlock(_pMem,pLastFreeBlock,uFreeSize);
		
		// Create allocated block
		uAllocatedSize = (unsigned int)_pMem->pv_LastFree - (unsigned int)pReturnAddress;
		MEM_vCreateAllocatedBlock(pReturnAddress,uAllocatedSize,1);
		
		// Update counters and pointers
		_pMem->pv_LastFree = pReturnAddress;
		_pMem->ul_DynamicMaxSize = (ULONG) _pMem->pv_LastFree - (ULONG) _pMem->pv_DynamicBloc;
		pReturnAddress = (void *)MEM_pGetUserAddress(pReturnAddress);
		_pMem->ul_DynamicCurrentAllocated += uAllocatedSize;			
		
#ifdef MEM_SPY
		MEM_SPY_LOCKER = 1;
#endif
		MEM_vAddAllocToHistory(_pMem,
			(ULONG)pReturnAddress,
			(ULONG)pReturnAddress + uAllocatedSize - MEM_TOTAL_HEADER_SIZE,
			_str_file,
			 _i_line);
#ifdef MEM_SPY
		MEM_SPY_LOCKER = 0;
#endif
		
		MEM_M_EndCriticalSection(_pMem);
	}
	else
	{
		MEM_M_EndCriticalSection(_pMem);
		// Not enough memory in last free block. Try to allocate in free blocks.
#ifdef MEM_SPY
		_print("can't alloc from end %d for %s(%d)\n", _ul_size, _str_file, _i_line);
		pReturnAddress = _MEM_p_Alloc(_pMem,_ul_size, _str_file, _i_line);
#else // _DEBUG
		pReturnAddress = _MEM_p_Alloc(_pMem,_ul_size);
#endif // _DEBUG	
	}
	
	MEM_vConditionnalCheckMem(_pMem);
	if (pReturnAddress)
		MEM_vCheckAllocatedBlock(MEM_pGetBlockAddress(pReturnAddress));

	MEM_UPDATE_MEASURES(_pMem);
	_GSP_EndRaster(56);		
		
	return pReturnAddress;
}



#endif


#ifdef MEM_SPY
void *_MEM_p_AllocFromEnd(MEM_tdst_MainStruct *_pMem,ULONG _ul_size, char *_str_file, int _i_line)
#else // _DEBUG
void *_MEM_p_AllocFromEnd(MEM_tdst_MainStruct *_pMem,ULONG _ul_size)
#endif // _DEBUG
{
	void *pLastFreeBlock;
	void *pReturnAddress;
	ULONG uNeededSize,uFreeSize;
	
#ifdef MEM_DONT_USE_VIRTUAL_MEMORY
	_pMem = &MEM_gst_MemoryInfo;
#endif // MEM_DONT_USE_VIRTUAL_MEMORY
	
	
	MEM_vConditionnalCheckMem(_pMem);
	if (_ul_size == 0)
		return NULL;
	
	_GSP_BeginRaster(56);		
	MEM_M_BeginCriticalSection(_pMem);
		
		
	if (!MEM_bIsBlockBeforeFree(_pMem->pv_LastFree))
	{
	#ifdef _GAMECUBE
		// If there is no more space in Virtual RAM, we can allocate in basic RAM.
		if (_pMem == &MEM_gst_VirtualMemoryInfo)
		{
			MEM_M_EndCriticalSection(_pMem);
		
		#ifdef MEM_SPY
			return _MEM_p_AllocFromEnd(&MEM_gst_MemoryInfo, _ul_size, _str_file,  _i_line);
		#else
			return _MEM_p_AllocFromEnd(&MEM_gst_MemoryInfo, _ul_size);
		#endif
		}
		else
	#endif //_GAMECUBE
		{
			// No more space at end : we allocate in the holes.
			MEM_M_EndCriticalSection(_pMem);
			
		#ifdef MEM_SPY
			return _MEM_p_Alloc(&MEM_gst_MemoryInfo, _ul_size, _str_file,  _i_line);
		#else
			return _MEM_p_Alloc(&MEM_gst_MemoryInfo, _ul_size);
		#endif
		}
	}
	
	// Get last free block
	pLastFreeBlock = (void *)MEM_pGetBlockBefore(_pMem->pv_LastFree);
	
	// Do we have enough memory in last block to make the allocation ?
	
	// Memory needed for allocation
	uNeededSize = MEM_iComputeNeededBlockSize(_ul_size);

	// Free memory
	uFreeSize = MEM_iGetBlockSize(pLastFreeBlock);
	
	if (uNeededSize + MEM_MIN_SIZE <= uFreeSize)
	{
		// We have enough memory for the allocation : do it, and resize free block.
		void *pAddress;
		
		MEM_vRemoveFreeBlock(_pMem,pLastFreeBlock);
		
		uFreeSize -= uNeededSize;
		
		MEM_vCreateFreeBlock(_pMem,pLastFreeBlock,uFreeSize);
		
		
		pAddress = ((unsigned char *)pLastFreeBlock)+uFreeSize;
		MEM_vCreateAllocatedBlock(pAddress,uNeededSize,1);
		_pMem->pv_LastFree = pAddress;
		_pMem->ul_DynamicMaxSize = (ULONG) _pMem->pv_LastFree - (ULONG) _pMem->pv_DynamicBloc;
		pReturnAddress = (void *)MEM_pGetUserAddress(pAddress);
		
		_pMem->ul_DynamicCurrentAllocated += uNeededSize;	
		
#ifdef MEM_SPY
		MEM_SPY_LOCKER = 1;
#endif
		MEM_vAddAllocToHistory(_pMem,
			(ULONG)pReturnAddress,
			(ULONG)pReturnAddress + uNeededSize - MEM_TOTAL_HEADER_SIZE,
			_str_file,
			 _i_line);
		 
#ifdef MEM_SPY
		MEM_SPY_LOCKER = 0;
#endif
		
		MEM_M_EndCriticalSection(_pMem);
	}
	else
	{
		MEM_M_EndCriticalSection(_pMem);
		// Not enough memory in last free block. Try to allocate in free blocks.
#ifdef MEM_SPY
		_print("can't alloc from end %d for %s(%d)\n", _ul_size, _str_file, _i_line);
		pReturnAddress = _MEM_p_Alloc(_pMem,_ul_size, _str_file, _i_line);
#else // _DEBUG
		pReturnAddress = _MEM_p_Alloc(_pMem,_ul_size);
#endif // _DEBUG	
	}
	
	MEM_vConditionnalCheckMem(_pMem);
	if (pReturnAddress)
		MEM_vCheckAllocatedBlock(MEM_pGetBlockAddress(pReturnAddress));

	MEM_UPDATE_MEASURES(_pMem);
	_GSP_EndRaster(56);		
		
	return pReturnAddress;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
 

void MEM_FreeFromEnd(void *_pAddress)
{
	void *pBlockAddress;
	MEM_tdst_MainStruct *pMem; 
	if (!_pAddress)
		return;

	pMem = MEM_pChooseMemFromAddress(_pAddress);

#if defined(PSX2_TARGET)
	* (ULONG *) &_pAddress &= 0x0fffffff;
#endif
	
	_GSP_BeginRaster(57);	
				
	MEM_vConditionnalCheckMem(pMem);
	MEM_M_BeginCriticalSection(pMem);
	
	// Is the freed block really from end ?
	pBlockAddress = (void *)MEM_pGetBlockAddress(_pAddress);
	
	if (pBlockAddress >= pMem->pv_LastFree)
	{
#ifdef MEM_SPY
		MEM_SPY_LOCKER = 1;
#endif
		MEM_vRemoveAllocFromHistory(pBlockAddress);
#ifdef MEM_SPY
		MEM_SPY_LOCKER = 0;
#endif

		// If block is first, free all blocks that have been marked as free.
		if (pBlockAddress == pMem->pv_LastFree)
		{
			ULONG uFreeSize;
			void *pFreeBlock;
			
			if (MEM_bIsBlockBeforeFree(pBlockAddress))
			{
				pFreeBlock = (void *)MEM_pGetBlockBefore(pBlockAddress);
				uFreeSize = MEM_iGetBlockSize(pFreeBlock);
				MEM_vRemoveFreeBlock(pMem,pFreeBlock);
			}
			else
			{
				pFreeBlock = pBlockAddress;
				uFreeSize = 0;
			}
		
			// Move pMem->pv_LastFree forward until we find a block that is not freed
			do 
			{
				ULONG uSize;
				
				uSize = MEM_iGetBlockSize(pMem->pv_LastFree);
				uFreeSize += uSize;
				pMem->ul_DynamicCurrentAllocated -= uSize;			
				
#ifdef JADEFUSION
				pMem->pv_LastFree = ((unsigned char *)pMem->pv_LastFree) + uSize;
#else
				((unsigned char *)pMem->pv_LastFree) += uSize;
#endif
			}
			while (MEM_bIsBlockFree(pMem->pv_LastFree));
			
			// Create free block with freed memory.
			MEM_vCreateFreeBlock(pMem,pFreeBlock,uFreeSize);
						
			pMem->ul_DynamicMaxSize = (ULONG) pMem->pv_LastFree - (ULONG) pMem->pv_DynamicBloc;
		}
		else
		{
			// Just mark block as free (but delay real free until all block before it are freed).
			MEM_bSetBlockAsFree(pBlockAddress);
		}
		
		MEM_M_EndCriticalSection(pMem);
	}
	else
	{
		// The address is in the normal stack (not from end). Free it there.
		MEM_M_EndCriticalSection(pMem);
		MEM_Free(_pAddress);
	}
	MEM_UPDATE_MEASURES(pMem);
	MEM_vConditionnalCheckMem(pMem);
	_GSP_EndRaster(57);			
	
}

void MEM_vFromEndFastFree(MEM_tdst_MainStruct *_pMem,void *_pNewLastFree)
{
#ifdef MEM_DONT_USE_VIRTUAL_MEMORY
	_pMem = &MEM_gst_MemoryInfo;
#endif // MEM_DONT_USE_VIRTUAL_MEMORY

	if (_pNewLastFree != _pMem->pv_LastFree)
	{
		void *pBeforeBlock;
		u32 uSize,uPrevSize;
		
		MEM_vConditionnalCheckMem(_pMem);
		MEM_M_BeginCriticalSection(_pMem);

#ifdef MEM_SPY
		{
			// We must parse all allocated blocks that will be freed in order to remove them from history
			void *pCurrentBlock = _pMem->pv_LastFree;
			
			while (pCurrentBlock < _pNewLastFree)
			{
				if (!MEM_bIsBlockFree(pCurrentBlock))
				{
#ifdef MEM_SPY
					MEM_SPY_LOCKER = 1;
#endif
					MEM_vRemoveAllocFromHistory(pCurrentBlock);
#ifdef MEM_SPY
					MEM_SPY_LOCKER = 0;
#endif
				}
				pCurrentBlock = (void *)((ULONG)pCurrentBlock + MEM_iGetBlockSize(pCurrentBlock));
			}
		}
#endif					
		
		
		pBeforeBlock = (void *)MEM_pGetBlockBefore(_pMem->pv_LastFree);
		_pMem->pv_LastFree = _pNewLastFree;
	
		uPrevSize = MEM_iGetBlockSize(pBeforeBlock);
		MEM_vRemoveFreeBlock(_pMem,pBeforeBlock);
		
		uSize = (ULONG)_pNewLastFree - (ULONG)pBeforeBlock;
		MEM_vCreateFreeBlock(_pMem,pBeforeBlock,uSize);
	
		_pMem->ul_DynamicCurrentAllocated -= uSize - uPrevSize;
		_pMem->ul_DynamicMaxSize = (ULONG) _pMem->pv_LastFree - (ULONG) _pMem->pv_DynamicBloc;
		
		MEM_UPDATE_MEASURES(_pMem);
		MEM_M_EndCriticalSection(_pMem);
		MEM_vConditionnalCheckMem(_pMem);
	}
}
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    debugging functions
 -----------------------------------------------------------------------------------------------------------------------
 */


/*
 =======================================================================================================================
 Print state of RAM.
 =======================================================================================================================
 */
LONG	MEM_bLogHeap = 0;

// Get size of allocated mem at end of heap.
#define MEM_uGetFromEndAllocatedSize(Mem) ((ULONG)(Mem).pvMemoryHeapEnd - (ULONG)(Mem).pv_LastFree)

void MEM_vLogHeapStateSummary()
{
    ULONG uSize;
	
	MEM_vCheckMem(&MEM_gst_MemoryInfo);

#ifndef _FINAL_

#ifdef _GAMECUBE
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
	//MEM_TestVM();
#endif //_GAMECUBE

	// Summary of RAM state :
	_print("BASIC RAM SUMMARY :\n");
#ifdef MEM_OVERRUN_MARKER
	_print( "WARNING : Allocated size includes markers (8 bytes per alloc) !!!!!!\n");
#endif // MEM_OVERRUN_MARKER
	_print( "Heap size (in bytes) : %d\n", MEM_gst_MemoryInfo.ul_RealSize);
	_print( "Allocated size (in bytes) : %d\n", MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated);
	uSize = MEM_uGetFromEndAllocatedSize(MEM_gst_MemoryInfo);
	_print( "From end allocated size (in bytes) : %d\n", uSize);
	uSize = MEM_uGetLastBlockSize(&MEM_gst_MemoryInfo);
	_print( "Last block size (in bytes) : %d\n",uSize);
	uSize = MEM_uGetHolesSize(&MEM_gst_MemoryInfo);
	_print( "Holes size (in bytes) : %d\n", uSize);
	_print( "Holes nb : %d\n", MEM_gst_MemoryInfo.ul_HolesStatic-1);
#ifdef MEM_MEASURES
	uSize = MEM_gst_MemoryInfo.ul_CurrentFree;
	_print( "Total free size (in bytes) : %d\n",uSize);
	uSize = MEM_gst_MemoryInfo.ul_MinFreeTotal;
	_print( "Min reached total free size (in bytes) : %d\n",uSize);
	uSize = MEM_gst_MemoryInfo.ul_MinLastFree;
	_print( "Min reached last block size (in bytes) : %d\n",uSize);
#endif // MEM_MEASURES	
	_print("\n");		
	
	
#ifdef _GAMECUBE
	_print("VRAM SUMMARY :\n");
	_print( "Virtual Heap size (in bytes) : %d\n", MEM_gst_VirtualMemoryInfo.ul_RealSize);
	_print( "RAM Heap size (in bytes) : %d\n", MEM_gst_VirtualMemoryInfo.ul_RealSize);
	_print( "Allocated size (in bytes) : %d\n", MEM_gst_VirtualMemoryInfo.ul_DynamicCurrentAllocated);
	uSize = MEM_uGetFromEndAllocatedSize(MEM_gst_VirtualMemoryInfo);
	_print( "From end allocated size (in bytes) : %d\n", uSize);
	uSize = MEM_uGetLastBlockSize(&MEM_gst_VirtualMemoryInfo);
	_print( "Last block size (in bytes) : %d\n",uSize);
	uSize = MEM_uGetHolesSize(&MEM_gst_VirtualMemoryInfo);
	_print( "Holes size (in bytes) : %d\n", uSize);
	_print( "Holes nb : %d\n", MEM_gst_VirtualMemoryInfo.ul_HolesStatic-1);
#ifdef MEM_MEASURES
	uSize = MEM_gst_VirtualMemoryInfo.ul_CurrentFree;
	_print( "Total free size (in bytes) : %d\n",uSize);
	uSize = MEM_gst_VirtualMemoryInfo.ul_MinFreeTotal;
	_print( "Min reached total free size (in bytes) : %d\n",uSize);
	uSize = MEM_gst_VirtualMemoryInfo.ul_MinLastFree;
	_print( "Min reached last block size (in bytes) : %d\n",uSize);
#endif // MEM_MEASURES	
	
	_print("\n");	
#endif //_GAMECUBE
#endif
}

void MEM_vLogHeapStateComplement()
{
	/*~~~~~~~~~~~~~~~~*/
	ULONG uSize,i,uBlockNb;
	/*~~~~~~~~~~~~~~~~*/
	_print("ELEMENTS :\n");
	_print("Element nb/max nb : %d/%d\n",MEM_C_ElementPoolSize - MEM_iCurrentFreeElementLeft,MEM_C_ElementPoolSize - MEM_iMinFreeElementLeft);	
	_print("\n");	

#ifdef _GAMECUBE
	MEM_M_BeginCriticalSection(&MEM_gst_VirtualMemoryInfo);
#endif //_GAMECUBE
	
	_print("BASIC HOLES DETAILS :\n");
	for (i=0; i<MEM_C_PowerArraySize; i++)
	{
		MEM_tdst_Element *pElement;
		pElement = MEM_gst_MemoryInfo.ap_PowerArray[i];
			
		while (pElement)
		{
			void *pFreeBlock;
			
			uBlockNb = 0;
				
			uSize = pElement->u_Size;
			pFreeBlock = pElement->p_BlockList;
					
			while (pFreeBlock)
			{
				uBlockNb++;
				pFreeBlock = (void *)MEM_pGetNextBlock(pFreeBlock);
			}
			
			_print("Hole size/nb : %d/%d\n",uSize,uBlockNb);
			
			pElement = pElement->pst_NextElement;
		}
	}

	_print("\n");	
#ifdef _GAMECUBE
	_print("VRAM HOLES DETAILS :\n");
	for (i=0; i<MEM_C_PowerArraySize; i++)
	{
		MEM_tdst_Element *pElement;
		pElement = MEM_gst_VirtualMemoryInfo.ap_PowerArray[i];
			
		while (pElement)
		{
			void *pFreeBlock;
			
			uBlockNb = 0;
				
			uSize = pElement->u_Size;
			pFreeBlock = pElement->p_BlockList;
					
			while (pFreeBlock)
			{
				uBlockNb++;
				pFreeBlock = (void *)MEM_pGetNextBlock(pFreeBlock);
			}
			
			_print("Hole size/nb : %d/%d\n",uSize,uBlockNb);
			
			pElement = pElement->pst_NextElement;
		}
	}
#endif //_GAMECUBE
	
#ifdef MEM_USE_HISTORY
	MEM_PrintHistory();
	MEM_iLogFrameNb = -1;
#endif // MEM_USE_HISTORY	
#ifdef _GAMECUBE
	MEM_M_EndCriticalSection(&MEM_gst_VirtualMemoryInfo);
#endif //_GAMECUBE
}

void MEM_vLogHeapStateSummaryMem(MEM_tdst_MainStruct *_pMem)
{
    ULONG uSize;
	

#ifndef _FINAL_

	// Summary of RAM state :
	_print("BASIC RAM SUMMARY :\n");
#ifdef MEM_OVERRUN_MARKER
	_print( "WARNING : Allocated size includes markers (8 bytes per alloc) !!!!!!\n");
#endif // MEM_OVERRUN_MARKER
	_print( "Heap size (in bytes) : %d\n", _pMem->ul_RealSize);
	_print( "Allocated size (in bytes) : %d\n", _pMem->ul_DynamicCurrentAllocated);
	uSize = MEM_uGetFromEndAllocatedSize(*_pMem);
	_print( "From end allocated size (in bytes) : %d\n", uSize);
	uSize = MEM_uGetLastBlockSize(_pMem);
	_print( "Last block size (in bytes) : %d\n",uSize);
	uSize = MEM_uGetHolesSize(_pMem);
	_print( "Holes size (in bytes) : %d\n", uSize);
	_print( "Holes nb : %d\n", _pMem->ul_HolesStatic-1);
#ifdef MEM_MEASURES
	uSize = _pMem->ul_CurrentFree;
	_print( "Total free size (in bytes) : %d\n",uSize);
	uSize = _pMem->ul_MinFreeTotal;
	_print( "Min reached total free size (in bytes) : %d\n",uSize);
	uSize = _pMem->ul_MinLastFree;
	_print( "Min reached last block size (in bytes) : %d\n",uSize);
#endif // MEM_MEASURES	
	_print("\n");		
	
#endif
}

void MEM_vLogHeapStateComplementMem(MEM_tdst_MainStruct *_pMem)
{
	/*~~~~~~~~~~~~~~~~*/
	ULONG uSize,i,uBlockNb;
	/*~~~~~~~~~~~~~~~~*/

#ifdef _GAMECUBE
	MEM_M_BeginCriticalSection(&MEM_gst_VirtualMemoryInfo);
#endif //_GAMECUBE
	
	_print("BASIC HOLES DETAILS :\n");
	for (i=0; i<MEM_C_PowerArraySize; i++)
	{
		MEM_tdst_Element *pElement;
		pElement = _pMem->ap_PowerArray[i];
			
		while (pElement)
		{
			void *pFreeBlock;
			
			uBlockNb = 0;
				
			uSize = pElement->u_Size;
			pFreeBlock = pElement->p_BlockList;
					
			while (pFreeBlock)
			{
				uBlockNb++;
				pFreeBlock = (void *)MEM_pGetNextBlock(pFreeBlock);
			}
			
			_print("Hole size/nb : %d/%d\n",uSize,uBlockNb);
			
			pElement = pElement->pst_NextElement;
		}
	}

	_print("\n");	
	
#ifdef MEM_SPY
	MEM_PrintSpySummary(_pMem);
#endif // MEM_SPY
	
#ifdef _GAMECUBE
	MEM_M_EndCriticalSection(&MEM_gst_VirtualMemoryInfo);
#endif //_GAMECUBE
}





void MEM_vLogHeapState()
{
	MEM_vLogHeapStateSummary();
	MEM_vLogHeapStateComplement();
}

void MEM_vLogHeapStateMem(MEM_tdst_MainStruct *_pMem)
{
	MEM_vLogHeapStateSummaryMem(_pMem);
	MEM_vLogHeapStateComplementMem(_pMem);
}

#ifdef MEM_USE_HISTORY

// Print state of memory (all allocated blocks)
void MEM_vPrintSnapShot()
{
	int i, iIndex;
	ULONG *pAddress;
	
	char *sCurrentFile;
	int iCurrentLine;
	int iCurrentSize;
	int iSizeTotal;
	int iNbAlloc;
	
	MEM_ASSERT_CRITICAL();
	
	for(i=0; i<MEM_Cul_AllocationHistorySize; i++)
	{
		MEM_gap_SortTable[i] = i;
	}
	
	qsort(MEM_gap_SortTable,MEM_Cul_AllocationHistorySize,sizeof(ULONG),&MEM_iCompareSnapShotData);

    {
    	// Gather allocs by 3 first letters of file name
    	_print("\n");	
    	_print("ALLOCATION SUMMARY :\n");
    	sCurrentFile = NULL;
    	iCurrentLine = 0;
    	
    	for(i=0; i<MEM_Cul_AllocationHistorySize; i++)
    	{
    		iIndex = MEM_gap_SortTable[i];
    		
    		pAddress = (ULONG*)MEM_gai_SnapShotAddress[iIndex];
    		iCurrentSize = MEM_gai_SnapShotSize[iIndex];
    		
    		if (pAddress && sCurrentFile && 0 == strncmp(MEM_gapsz_SnapShotAllocatedInFile[iIndex],sCurrentFile,3))
    		{
    			iSizeTotal += iCurrentSize;		
    			iNbAlloc++;
    		}
    		else
    		{
    			char sName[10];
    			if (sCurrentFile)
    			{
    				sName[0] = sCurrentFile[0];
    				sName[1] = sCurrentFile[1];
    				sName[2] = sCurrentFile[2];
    				sName[3] = '\0';
    					
    				ERR_X_Assert(iSizeTotal <MEM_MAX_SIZE);
    					
    				_print("%s\t%d\t%d\n",sName,iSizeTotal,iNbAlloc);
    			}
    				
    			if(!pAddress) 
    				break;
    				
    			// New data
    			sCurrentFile = MEM_gapsz_SnapShotAllocatedInFile[iIndex];
    			iSizeTotal = iCurrentSize;
    			iNbAlloc = 1;
    		}
    		
    	}
   	}
	
    {
    	// Gather allocs by file name and line number
    	_print("\n");	
    	_print("ALLOCATION DETAILS :\n");
    	_print("file(line) size alloc_nb \n");
    	
    	sCurrentFile = NULL;
    	iCurrentLine = 0;
    	for(i=0; i<MEM_Cul_AllocationHistorySize; i++)
    	{
    		iIndex = MEM_gap_SortTable[i];
    		
    		pAddress = (ULONG*)MEM_gai_SnapShotAddress[iIndex];
    		iCurrentSize = MEM_gai_SnapShotSize[iIndex];
    		
    		if (pAddress && (MEM_gai_SnapShotAllocatedInLine[iIndex] == iCurrentLine) && (0 == L_strcmp(MEM_gapsz_SnapShotAllocatedInFile[iIndex],sCurrentFile)))
    		{
    			iSizeTotal += iCurrentSize;
    			
    			iNbAlloc++;
    		}
    		else
    		{
    			if (sCurrentFile)
    			{
    				ERR_X_Assert(iSizeTotal <MEM_MAX_SIZE);
    				_print("%s(%d)\t%d\t%d\n",sCurrentFile,iCurrentLine,iSizeTotal,iNbAlloc);
    			}
    				
    			if(!pAddress) 
    				break;

    			sCurrentFile = MEM_gapsz_SnapShotAllocatedInFile[iIndex];
    			iCurrentLine = MEM_gai_SnapShotAllocatedInLine[iIndex];

    			iSizeTotal = iCurrentSize;
    			iNbAlloc = 1;
    		}
    	}
    }	
}

// Save state of memory
void MEM_vSaveSnapShot()
{
	int i;
	LONG uAddress;
		
#ifdef MEM_CRITICAL
	ULONG bInCriticalSection = MEM_bIsInCriticalSection(&MEM_gst_MemoryInfo);
#ifdef _GAMECUBE
	ULONG bVMInCriticalSection = MEM_bIsInCriticalSection(&MEM_gst_VirtualMemoryInfo);
	if (!bVMInCriticalSection)
		MEM_M_BeginCriticalSection(&MEM_gst_VirtualMemoryInfo);
#endif //_GAMECUBE		
	if (!bInCriticalSection)
		MEM_M_BeginCriticalSection(&MEM_gst_MemoryInfo);
#endif //  MEM_CRITICAL

    MEM_bPrintSnapShotAtExit = FALSE;
		
	for (i=0; i<MEM_Cul_AllocationHistorySize; i++)
	{
		uAddress = MEM_gap_AllocationHistoryTable[i];
		MEM_gai_SnapShotAddress[i] = uAddress;
		if (uAddress)
		{
			ULONG uSize = MEM_iGetBlockSize((void *)MEM_pGetBlockAddress(uAddress)); 
			MEM_gapsz_SnapShotAllocatedInFile[i] = MEM_gapsz_AllocatedInFile[i];
			MEM_gai_SnapShotAllocatedInLine[i] = MEM_gai_AllocatedInLine[i];
			MEM_gai_SnapShotAllocationIncrement[i] = MEM_gai_AllocationIncrement[i];
			MEM_gai_SnapShotSize[i] = uSize;
		}
	}	
	
#ifdef MEM_CRITICAL
	if (!bInCriticalSection)
		MEM_M_EndCriticalSection(&MEM_gst_MemoryInfo);
#ifdef _GAMECUBE
	if (!bVMInCriticalSection)
		MEM_M_EndCriticalSection(&MEM_gst_VirtualMemoryInfo);
#endif //_GAMECUBE		
#endif //  MEM_CRITICAL
	
}

// Print difference between state of memory saved by MEM_vSaveSnapShot and present state of memory.
void MEM_vPrintDeltaSnapShot()
{
	int i,iIndex;
	LONG uAddressBefore,uAddressAfter,uAddress;
	ULONG uSize;
	
	MEM_vCheckMem(&MEM_gst_MemoryInfo);
#ifdef _GAMECUBE
	MEM_vCheckMem(&MEM_gst_VirtualMemoryInfo);
	//MEM_TestVM();
#endif //_GAMECUBE


#ifdef _GAMECUBE
	MEM_M_BeginCriticalSection(&MEM_gst_VirtualMemoryInfo);
	//MEM_TestVM();
#endif //_GAMECUBE	
	
	_print("Memory that should have been freed : \n");
	for (i=0; i<MEM_Cul_AllocationHistorySize; i++)
	{
		uAddressBefore = MEM_gai_SnapShotAddress[i];
		uAddressAfter = MEM_gap_AllocationHistoryTable[i];
		if (uAddressAfter && (uAddressAfter != uAddressBefore)) 
			MEM_gap_SortTable[i] = i;
		else
			MEM_gap_SortTable[i] = -1;
	}

	qsort(MEM_gap_SortTable,MEM_Cul_AllocationHistorySize,sizeof(ULONG),&MEM_iCompareWithAllocationInc);
	
	for (i=0; i<MEM_Cul_AllocationHistorySize; i++)
	{
		iIndex = MEM_gap_SortTable[i];
		if (-1 == iIndex)
			break;
		uAddress = MEM_gap_AllocationHistoryTable[iIndex];
		uSize = MEM_iGetBlockSize((void *)MEM_pGetBlockAddress(uAddress)); 
		_print("0x%x %s(%d) %d %d\n",
			uAddress,
			MEM_gapsz_AllocatedInFile[iIndex],
			MEM_gai_AllocatedInLine[iIndex],
			uSize,
			MEM_gai_AllocationIncrement[iIndex]
			);
	}	
	
	_print("Memory that should not have been freed :\n");
	for (i=0; i<MEM_Cul_AllocationHistorySize; i++)
	{
		uAddressBefore = MEM_gai_SnapShotAddress[i];
		uAddressAfter = MEM_gap_AllocationHistoryTable[i];
		if (uAddressBefore && (uAddressBefore != uAddressAfter)) 
			MEM_gap_SortTable[i] = i;
		else
			MEM_gap_SortTable[i] = -1;
	}
	
	qsort(MEM_gap_SortTable,MEM_Cul_AllocationHistorySize,sizeof(ULONG),&MEM_iCompareSnapShotWithAllocationInc);
	
	for (i=0; i<MEM_Cul_AllocationHistorySize; i++)
	{
		iIndex = MEM_gap_SortTable[i];
		if (-1 == iIndex)
			break;
		uAddress = MEM_gai_SnapShotAddress[iIndex];
		_print("0x%x %s(%d) %d %d \n",
			uAddress,
			MEM_gapsz_SnapShotAllocatedInFile[iIndex],
			MEM_gai_SnapShotAllocatedInLine[iIndex],
			MEM_gai_SnapShotSize[iIndex],
			MEM_gai_SnapShotAllocationIncrement[iIndex]
			);
	}		
	
#ifdef _GAMECUBE
	MEM_M_EndCriticalSection(&MEM_gst_VirtualMemoryInfo);
	//MEM_TestVM();
#endif //_GAMECUBE		
}
#endif // MEM_USE_HISTORY


#ifdef MEM_MEASURES
void MEM_vResetMeasureMins(MEM_tdst_MainStruct *_pMem)
{
	_pMem->ul_MinFreeTotal = _pMem->ul_CurrentFree;
	_pMem->ul_MinLastFree = MEM_uGetLastBlockSize(_pMem);
}
#endif // MEM_MEASURES

// Mettre ce booléen à TRUE pour logguer l'utilisation du heap.
static BOOL MEM_bLogHeapUsage = FALSE;

void MEM_vManageSnapShot()
{
#ifdef MEM_SPY
	if (!MEM_bDisplayLeaks)
	{
        if (MEM_bStartLeakTracking)
        {
            MEM_vComputeSpyDeltaSnapShot();
            if (MEM_bDisplayLeaks)
            {
            	MEM_vLogLeaks();
            	MEM_vDispLeaks();
            }
        }

        if (!MEM_bDisplayLeaks)
            MEM_vSaveSpySnapShot();
	}
	
	if (MEM_bLogHeapUsage)
		MEM_vLogHeapUsage(&MEM_gst_MemoryInfo);
		
#ifdef _GAMECUBE
	if (MEM_bLogHeapUsage)
		MEM_vLogHeapUsage(&MEM_gst_VirtualMemoryInfo);
#endif // _GAMECUBE
		
#endif // MEM_SPY

#ifdef MEM_USE_HISTORY
#ifdef _GAMECUBE
    // Number of times that we go here before doing snapshots.
    if (MEM_gb_bTrackLeaks)
    {
        if (MEM_bStartLeakTracking)
            MEM_vPrintDeltaSnapShot();

        MEM_vSaveSnapShot();
    }
    else if (MEM_gb_SnapShotMaxMemUsed)
    {
    	MEM_vPrintSnapShot();
    	MEM_gul_SnapShotMaxMemSize = 0;
    }
#endif // _GAMECUBE
#endif //MEM_USE_HISTORY


// Here is where we begin the loading of a map : reset the mins...
#ifdef MEM_MEASURES
#ifdef _GAMECUBE
	MEM_vResetMeasureMins(&MEM_gst_VirtualMemoryInfo);
#endif // _GAMECUBE
	MEM_vResetMeasureMins(&MEM_gst_MemoryInfo);
#endif // MEM_MEASURES
}

ULONG MEM_ul_GetRealSize(void *_pv_Block)
{
	ULONG uSize;
	MEM_tdst_MainStruct *pMem = MEM_pChooseMemFromAddress(_pv_Block);
	
	MEM_M_BeginCriticalSection(pMem);
	uSize = MEM_iGetBlockSize((void *)MEM_pGetBlockAddress(_pv_Block)); 
	MEM_M_EndCriticalSection(pMem);
	return uSize;
}

void MEM_dbg_FindLastAllocatedCluster(void)
{
}


void MEM_vConditionnallyLogHeap()
{
	static int MEM_bLogHeapBefore = 0;
	
#ifdef MEM_USE_HISTORY
	MEM_iLogFrameNb++;
	if (MEM_gi_CurrentPageMissTime > MEM_gi_WorstPageMissTime)
	{
		int i;	
		MEM_gi_WorstPageMissTime = MEM_gi_CurrentPageMissTime;
		ERR_X_Assert( MEM_gi_CurrentPageMissSize > 0);
		MEM_gi_WorstPageMissSize = MEM_gi_CurrentPageMissSize;
		for (i=0; i<MEM_gi_CurrentPageMissSize; i++)
		{
			MEM_gas_WorstPageMissFile[i] = MEM_gas_CurrentPageMissFile[i];
			MEM_gai_WorstPageMissLine[i] = MEM_gai_CurrentPageMissLine[i];
			MEM_gai_WorstPageMissTime[i] = MEM_gai_CurrentPageMissTime[i];
		}		
	}	
	MEM_gi_CurrentPageMissTime = 0; 
	MEM_gi_CurrentPageMissSize = 0;
#endif // MEM_USE_HISTORY
	
	// We do the log only when going from 1 to 0
	if (MEM_bLogHeapBefore && !MEM_bLogHeap) 
	{
		static BOOL bLogSnapShotMode = 0;
		MEM_vLogHeapState();
#ifdef MEM_USE_HISTORY
		if (bLogSnapShotMode)
		{
			MEM_vSaveSnapShot();
			MEM_vPrintSnapShot();
		}
#endif // MEM_USE_HISTORY
	}

	if (!MEM_bLogHeapBefore && MEM_bLogHeap) 
		MEM_vInitLogHeapState();
	
	MEM_bLogHeapBefore = MEM_bLogHeap;
}

// Check if block is marked as free 
// (knowing that the address may have been allocated by another part of the code)
char MEM_b_CheckBlockHasBeenFreed(void *_pv_Block)
{
	int iResult;
	void *pAddress = (void *)MEM_pGetBlockAddress(_pv_Block);
	MEM_tdst_MainStruct *pMem = MEM_pChooseMemFromAddress(_pv_Block);
	
	MEM_M_BeginCriticalSection(pMem);
	iResult = (MEM_bIsBlockFree(pAddress) != (unsigned int) NULL);
	MEM_M_EndCriticalSection(pMem);
	return iResult;
}


#ifdef _DEBUG
/*
 =======================================================================================================================
    Aim:    Check the validity of a pointer
 =======================================================================================================================
 */
#define MEM_Csz_PointerOutOfRange	"Pointer out of range detected"
#define MEM_Csz_NonalignedPointer	"Non aligned pointer detected"
#define MEM_Csz_NullPointerDetected "Null pointer detected"
#define MEM_Csz_FreePointerDetected "Free pointer detected"
 
 
char MEM_b_CheckPointer(void *p)
{
	LONG	l_max, l_min, l_p, l_ARAMmin, l_ARAMmax;

_Try_
	l_p = (LONG) p;

	ERR_X_ErrorAssert(l_p, MEM_Csz_NullPointerDetected, NULL);
	
	l_min = (LONG) MEM_gst_MemoryInfo.pv_DynamicBloc;
	l_max = (LONG) MEM_gst_MemoryInfo.pvMemoryHeapEnd;
#ifdef _GAMECUBE
	l_ARAMmin = (LONG) MEM_gst_VirtualMemoryInfo.pv_DynamicBloc;
	l_ARAMmax = (LONG) MEM_gst_VirtualMemoryInfo.pvMemoryHeapEnd;
#endif // _GAMECUBE
	l_p = (LONG) MEM_pGetBlockAddress(p);
	
	ERR_X_ErrorAssert(
		(((l_p >= l_min) && (l_p < l_max)) || ((l_p >= l_ARAMmin) && (l_p < l_ARAMmax))), 
		MEM_Csz_PointerOutOfRange, "");

#ifdef MEM_FREE_FILL_DATA
	ERR_X_ErrorAssert((*(ULONG *)p!=MEM_FREE_FILL_DATA),"Pointer probably points to freed data", "");
#endif //MEM_FREE_FILL_DATA

	
	// TODO : Add code with safe markers
	
_Catch_
	return(0);
_End_
	return(1);
}


#endif // _DEBUG
#ifdef JADEFUSION
int MEM_i_GetInternalSize(void *_pAddress)
{
	void* pBlockAddress = (void *)MEM_pGetBlockAddress(_pAddress);
	return MEM_iGetBlockSize(pBlockAddress);
}
#endif

#endif // MEM_OPT