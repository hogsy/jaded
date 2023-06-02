/*$T MEMdyn.c GC! 1.081 01/23/03 17:58:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
    HEADERS
 ***********************************************************************************************************************
 */


#include "Precomp.h"

#undef _MEMORY_MEGADEBUG

/*$2------------------------------------------------------------------------------------------------------------------*/

#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMLog.h"
#include "BASe/MEMory/MEMpro.h"
#include "BASe/BASsys.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "MATHs/MATHlong.h"
#include "BIGfiles/LOAding/LOAdefs.h"

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
#endif

#if defined( _XBOX ) || defined( _XENON )
#  include <XbDm.h>
#endif

#ifndef MEM_OPT
#define MEM_USE_TYPES

/*
 =======================================================================================================================
    Aim:    Return the size of a block (the initially asked size, not the really allocated size)

    Note:   This function works only with blocks allocated with MEM_p_Alloc, since blocks allocated with MEM_p_EcoAlloc
            don't have the size stored.
 =======================================================================================================================
 */
_inline_ ULONG MEM_ul_GetSize(void *_pv_Block)
{
#ifdef  MEM_USE_TYPES
	if ((*(((ULONG *) _pv_Block) - 1)) & 0x80000000)
	{
		return (*(((ULONG *) _pv_Block) - 1)) & ~0x80000000;
	} else
		return(*(((ULONG *) _pv_Block) - 1)) & 0xffffff;
#else
	return(*(((ULONG *) _pv_Block) - 1));
#endif
}


/*
 =======================================================================================================================
    Aim:    Sets the size of a block
 =======================================================================================================================
 */
_inline_ void MEM_SetSize(void *_pv_Block, ULONG _ul_Size)
{
#ifdef  MEM_USE_TYPES
	if (_ul_Size & 0xff000000)
	{
		(*(((ULONG *) _pv_Block) - 1)) = _ul_Size | 0x80000000;
	} else
#endif
	(*(((ULONG *) _pv_Block) - 1)) = _ul_Size;

}

#ifdef  MEM_USE_TYPES
/*
 =======================================================================================================================
    
 =======================================================================================================================
 */
_inline_ ULONG MEM_ul_GetType(void *_pv_Block)
{
	if ((*(((ULONG *) _pv_Block) - 1)) & 0x80000000)
	{
		return 0;
	} else
		return ((*(((ULONG *) _pv_Block) - 1))>>24) & 0x7f;
}

_inline_ void MEM_ul_ResetType(void *_pv_Block)
{
	if (!((*(((ULONG *) _pv_Block) - 1)) & 0x80000000))
		(*(((ULONG *) _pv_Block) - 1)) &= 0xffffff;
	else
		(*(((ULONG *) _pv_Block) - 1)) &= ~0x80000000;
}

_inline_ void MEM_SetType(void *_pv_Block, ULONG _ul_Type)
{
	if ((*(((ULONG *) _pv_Block) - 1)) & 0x80000000)
	{
		return;
	} else
	{
		(*(((ULONG *) _pv_Block) - 1)) |= (_ul_Type & 0x7f) << 24;
	}
}
#endif

ULONG MEM_ul_GetRealSize(void *_pv_Block)
{
	ULONG	ul_Size;
	ul_Size = MEM_ul_GetSize(_pv_Block);
	return(MEM_ul_GetRealSizeFromSize(ul_Size));
}

/*$4
 ***********************************************************************************************************************
    MACROS
 ***********************************************************************************************************************
 */

#define MEM_Mb_IsAllocPointer(p) \
		(((p) >= MEM_gst_MemoryInfo.pv_DynamicBloc) && ((p) < MEM_gst_MemoryInfo.pv_DynamicNextFree))

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Multithread access
 -----------------------------------------------------------------------------------------------------------------------
 */

#if defined(PSX2_TARGET) || defined(_XBOX) || defined(XML_CONV_TOOL)

/*$1- PS2 & Xbox ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define MEM_M_InitLock()
#define MEM_M_BeginCriticalSection()
#define MEM_M_EndCriticalSection()
#define MEM_M_DestroyLock()

#elif defined(_XENON_RENDER)

static CRITICAL_SECTION s_oMemoryAccessLock;

#define MEM_M_InitLock()                                    \
    do                                                      \
    {                                                       \
        InitializeCriticalSection(&s_oMemoryAccessLock);    \
    } while (0)

#define MEM_M_BeginCriticalSection()                        \
    do                                                      \
    {                                                       \
        EnterCriticalSection(&s_oMemoryAccessLock);         \
    } while (0)

#define MEM_M_EndCriticalSection()                          \
    do                                                      \
    {                                                       \
        LeaveCriticalSection(&s_oMemoryAccessLock);         \
    } while (0)

#define MEM_M_DestroyLock()                                 \
    do                                                      \
    {                                                       \
        DeleteCriticalSection(&s_oMemoryAccessLock);        \
    } while (0)

/*$1- Gamecube ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#elif defined(_GAMECUBE)
static BOOL sh_MultiThreadingLock;
#define MEM_M_InitLock()
#define MEM_M_BeginCriticalSection() \
	do \
	{ \
		sh_MultiThreadingLock = OSDisableInterrupts(); \
	} while(0)
#define MEM_M_EndCriticalSection() \
	do \
	{ \
		OSRestoreInterrupts(sh_MultiThreadingLock); \
	} while(0)
#define MEM_M_DestroyLock()

/*$1- win32 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#else
		static HANDLE	sh_MultiThreadingLock;
#define MEM_M_InitLock() \
	do \
	{ \
		sh_MultiThreadingLock = (HANDLE) 0; \
		sh_MultiThreadingLock = CreateEvent(NULL, FALSE, FALSE, NULL); \
		ERR_X_Assert(sh_MultiThreadingLock); \
		SetEvent(sh_MultiThreadingLock); \
	} while(0)
#define MEM_M_BeginCriticalSection() \
	do \
	{ \
		WaitForSingleObject(sh_MultiThreadingLock, INFINITE); \
	} while(0)
#define MEM_M_EndCriticalSection() \
	do \
	{ \
		SetEvent(sh_MultiThreadingLock); \
	} while(0)
#define MEM_M_DestroyLock() \
	do \
	{ \
		CloseHandle(sh_MultiThreadingLock); \
		sh_MultiThreadingLock = (HANDLE) 0; \
	} while(0)
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    signal fatal error
 -----------------------------------------------------------------------------------------------------------------------
 */

#if defined(_FINAL_)

/*$1- final = no signal ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#define MEM_M_SignalFatalError(a, b)

/*$1- PSX2 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#elif defined(PSX2_TARGET)
				extern unsigned int Gsp_Crash(unsigned int Address, unsigned char *);
extern void							Gsp_CrashPrintf(char *);
extern void							Gsp_FlipFB(void);

#define MEM_M_SignalFatalError(a, b) \
	do \
	{ \
		int		tmp; \
		char	az[256]; \
		asm("qmove tmp, ra"); \
		sprintf(az, "alloc size %d, from 0x%X", a, tmp); \
		printf("*** No more memory : %s ***\n", az);\
		Gsp_Crash(0, b); \
		Gsp_CrashPrintf(b); \
		Gsp_CrashPrintf(az); \
		Gsp_FlipFB(); \
		while(1) \
		{ \
		}; \
	} while(0);

/*$1- Gamecube ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#elif defined(_GAMECUBE)
#define MEM_M_SignalFatalError(a, b) \
	do \
	{ \
		char	az[256]; \
		sprintf(az, b " : %d oct\n", a); \
		GXI_ErrBegin(); \
		GXI_ErrPrint(az); \
		GXI_ErrEnd(); \
		while(1) \
		{ \
		}; \
	} while(0);

/*$1- others (win32+Xbox) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#elif defined(_XBOX) || defined(_XENON)
#define MEM_M_SignalFatalError(a, b)	ERR_X_Assert(0)

/*$1- others (win32+Xbox) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#else
#define MEM_M_SignalFatalError(a, b)	MessageBox(0, "No more memory", "No more memory", MB_OK)
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Hole Optimisation
 -----------------------------------------------------------------------------------------------------------------------
 */
#ifdef _GAMECUBE
#define USE_HOLE_OPTIM
#endif

#ifdef USE_HOLE_OPTIM

/*$1- macros ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//#define MEM_Cte_HoleChainSize	30000
#define MEM_Cte_HoleChainSize	10

/*$1- types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

typedef struct	MEM_tdst_HoleChain_
{
	ULONG						ul_Index;
	ULONG						*pul_Hole;
	struct MEM_tdst_HoleChain_	*pst_NextHole;
	struct MEM_tdst_HoleChain_	*pst_PrevHole;
} MEM_tdst_HoleChain;

/*$1- variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

MEM_tdst_HoleChain	MEM_gax_HoleChain[MEM_Cte_HoleChainSize];
ULONG				MEM_gul_HoleChainNb;
ULONG				MEM_gul_LostHoleSize;
ULONG				MEM_gul_LostHoleNb;
MEM_tdst_HoleChain	*MEM_gp_HoleChainLastElem;
#endif

#define PRELOADTEXGRAN	(40 * 1024)
extern void PreLoadTexReset(void);
extern char	*gp_LastPointer;		/* Pointer from end of block to free zone */
extern void PreloadCancel(void);
void MEM_InitDynamic(ULONG _ul_BigBlockSize);
void MEM_CloseDynamic(void);
void MEM_FreeFromEnd(void *_pv_block);

/*$4
 ***********************************************************************************************************************
    VARIABLES
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Global variable with various memory infos
 -----------------------------------------------------------------------------------------------------------------------
 */
#ifdef JADEFUSION
__declspec(align(32)) MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#else
MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#endif

char				*MEM_gp_MaxPeakMemory = 0x00000000;
void				*MEM_gp_OldHolesList = NULL;
ULONG				MEM_ul_NumOldHoles = 0;
ULONG				MEM_ul_SizeOldHoles = 0;
ULONG				MEM_ul_UpdateHoles = 0;
ULONG				MEM_ul_EmergencyConcat = 0;
void                *MEM_gp_NextFreeAfterDefrag=NULL;
int					MEM_gi_MinDelta = 0x7FFFFFFF;
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    history
 -----------------------------------------------------------------------------------------------------------------------
 */

#if defined(_DEBUG) && ( defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL) )
#define MEM_USE_HISTORY
#endif

#ifdef _DEBUG
#define MEM_USE_HISTORY
#endif

#ifdef MEM_USE_HISTORY

#ifdef PSX2_TARGET
#define MEM_Cul_AllocationHistorySize	200000
#else
#define MEM_Cul_AllocationHistorySize	1000000
#endif

LONG		MEM_gap_AllocationHistoryTable[MEM_Cul_AllocationHistorySize];
char		*MEM_gapsz_AllocatedInFile[MEM_Cul_AllocationHistorySize];
int			MEM_gai_AllocatedInLine[MEM_Cul_AllocationHistorySize];
LONG		MEM_g_nbpointers = 0;

LONG		MEM_gap_FreeHistoryTable[MEM_Cul_AllocationHistorySize];
char		*MEM_gapsz_FreeInFile[MEM_Cul_AllocationHistorySize];
int			MEM_gai_FreeInLine[MEM_Cul_AllocationHistorySize];
#endif // MEM_USE_HISTORY

/*$4
 ***********************************************************************************************************************
    PROTOTYPES
 ***********************************************************************************************************************
 */

char		MEM_b_CheckBlockAllocated(void *);
void		MEM_CheckBlockIsFree(void *);
void		MEM_CheckHolesChain(void *, ULONG);
void		MEM_Defrag(int single);
void		MEM_FreeTmp(void *);
extern void MEM_ComputeMemoryHoleInfo(ULONG *, ULONG *, BOOL);

#ifdef _GAMECUBE
void		gcMEM_PrepareMemoryPool(void);
#endif
#ifdef USE_HOLE_OPTIM
static void *MEM_p_AllocInOldHoles(ULONG);
static void MEM_DeleteHoleInChain(ULONG *);
static void *MEM_p_SplitHoleInChain(void *, ULONG _ul_BlockSize);
static void *MEM_p_GetHoleInChain(ULONG _ul_Size);
static void MEM_RegisterHoleInChain(ULONG *, ULONG index);
static BOOL MEM_b_ConcatHolesInChain(ULONG *, ULONG index);
void 		MEM_EmergencyConcat();
#else
#define MEM_DeleteHoleInChain(a)
#define MEM_p_SplitHoleInChain(a, b)	(NULL)
#define MEM_p_GetHoleInChain(a)			(NULL)
#define MEM_RegisterHoleInChain(a, b)
#define MEM_b_ConcatHolesInChain(a, b)	(FALSE)
#endif /* USE_HOLE_OPTIM */

static int	MEM_s_DynamicMemoryCheck = 1;

#if defined(_DEBUG) && !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_XENON)

#ifdef MEM_USE_TYPES
u32 HASH_TAble[32768];

static MEM_stSummaryElement MEM_SummaryTable[MEM_ulSummaryTableSize];
MEM_stSummaryElement *MEM_SortSummaryTable[MEM_ulSummaryTableSize];
ULONG MEM_ulSummaryNb = -1;

char *MEM_FormatString(char *Letter)
{
	char *Base;
	Base = Letter;
	while (*Letter) {Letter++; };

	while ((Letter > Base) && (*Letter != '\\')) {Letter--;};
	if (*Letter == '\\') Letter ++;
	return Letter ;
}
u32 MEM_FormatChar(char Letter)
{
	if (Letter > 'Z') Letter -= 'a' - 'A';
	Letter -= 'A';
	return Letter & 31;
}
u32 MEM_GetTypeFromName(char *_psz_File)
{
	u32 ThisName;
	_psz_File = MEM_FormatString(_psz_File);
	if (MEM_ulSummaryNb == -1)
	{
		L_memset(HASH_TAble,0xff,sizeof(HASH_TAble));
		L_memset(MEM_SummaryTable,0,sizeof(MEM_SummaryTable));
		MEM_ulSummaryNb = 1;
	}
	ThisName = MEM_FormatChar(*_psz_File) | (MEM_FormatChar(*(_psz_File+1))<<5) | (MEM_FormatChar(*(_psz_File+2))<<10);
	if (HASH_TAble[ThisName] == -1)
	{
		MEM_SummaryTable[MEM_ulSummaryNb].sHeader[0] = _psz_File[0];
		MEM_SummaryTable[MEM_ulSummaryNb].sHeader[1] = _psz_File[1];
		MEM_SummaryTable[MEM_ulSummaryNb].sHeader[2] = _psz_File[2];
		MEM_SummaryTable[MEM_ulSummaryNb].sHeader[3] = 0;
		HASH_TAble[ThisName] = MEM_ulSummaryNb++;
	}
	return HASH_TAble[ThisName] & 0x7f;
}
void MEM_IncrementType(u32 ulType, s32 Size)
{
	MEM_SummaryTable[ulType].ulAllocSize += Size;
}
void MEM_vBuildSummary()
{
	u32 i,j;
	i = MEM_ulSummaryNb;
	MEM_SummaryTable[0].ulAllocSize = 0;
	MEM_SummaryTable[0].sHeader[0] = 0;
	while (i--) 
	{
		MEM_SortSummaryTable[i] = &MEM_SummaryTable[i];
		MEM_SummaryTable[i].ulKey = i;
	}
	for ( i = 0 ; i < MEM_ulSummaryNb-1; i++ )
		for (j = i + 1 ;j < MEM_ulSummaryNb; j++)
			if (MEM_SortSummaryTable[i]->ulAllocSize < MEM_SortSummaryTable[j]->ulAllocSize)
			{
				MEM_stSummaryElement *SWPA;
				SWPA= MEM_SortSummaryTable[i];
				MEM_SortSummaryTable[i] = MEM_SortSummaryTable[j];
				MEM_SortSummaryTable[j] = SWPA;
			}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *L_Dbg_memcpy(void *_pv_Dest, const void *_pv_Src, int i_Size)
{
	if(MEM_s_DynamicMemoryCheck)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	*_pul_Byte, *_pul_LastByte;
		ULONG	ul_BlockLength;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(!_pv_Dest)
		{
			if(i_Size)
			{
				LINK_PrintStatusMsg("-Memcpy warning (NULL address detected)");
			}

			return(memcpy(_pv_Dest, _pv_Src, i_Size));
		}

		/*$F
		_pul_Byte = (ULONG *) _pv_Src;
		_pul_LastByte = _pul_Byte + (i_Size >> 2);
		for(; _pul_Byte < _pul_LastByte; _pul_Byte++)
		{
			if(((*_pul_Byte) & 0xFFFFFFFF) == 0xA0110270)
			{
				LINK_PrintStatusMsg("- Memcpy copy of Mark ");
				break;
			}
		}
		*/
		if(*(((ULONG *) _pv_Dest) - 2) == 0xA0110270)
		{
			ul_BlockLength = *(((ULONG *) _pv_Dest) - 1);
		}
		else
		{
			/*~~*/
			int i;
			/*~~*/

			ul_BlockLength = 0;
			for(i = 2; i < 1024; i++)
			{
				if(IsBadReadPtr((((ULONG *) _pv_Dest) - i), 4)) break;
				if((*(((ULONG *) _pv_Dest) - i)) == 0xA0110270)
				{
					ul_BlockLength = *(((ULONG *) _pv_Dest) - (i - 1));
					break;
				}
			}

			if(ul_BlockLength)
			{
				if(i_Size + ((i - 2) << 2) > (int) MEM_ul_GetRealSizeFromSize(ul_BlockLength))
				{
					LINK_PrintStatusMsg("- Memcpy warning (Length overpass block size)");
				}
				else
				{
					return(memcpy(_pv_Dest, _pv_Src, i_Size));
				}
			}
		}

		if(ul_BlockLength)
		{
			if(i_Size > (int) MEM_ul_GetRealSizeFromSize(ul_BlockLength))
			{
				LINK_PrintStatusMsg("- Memcpy warning (Length overpass block size)");
			}
			else
			{
				return(memcpy(_pv_Dest, _pv_Src, i_Size));
			}
		}

		_pul_Byte = (ULONG *) _pv_Dest;
		_pul_LastByte = _pul_Byte + (i_Size >> 2);
		for(; _pul_Byte < _pul_LastByte; _pul_Byte++)
		{
			if(((*_pul_Byte) & 0xFFFFFFFF) == 0xA0110270)
			{
				LINK_PrintStatusMsg("- Memcpy warning (Block mark found)");
			}
		}
	}

	return(memcpy(_pv_Dest, _pv_Src, i_Size));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *L_Dbg_memmove(void *_pv_Dest, const void *_pv_Src, int i_Size)
{
	if(MEM_s_DynamicMemoryCheck)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	*_pul_Byte, *_pul_LastByte;
		ULONG	ul_BlockLength;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(!_pv_Dest)
		{
			if(i_Size)
			{
				LINK_PrintStatusMsg("-Memmove warning (NULL address detected)");
			}

			return(memmove(_pv_Dest, _pv_Src, i_Size));
		}

		/*$F
		_pul_Byte = (ULONG *) _pv_Src;
		_pul_LastByte = _pul_Byte + (i_Size >> 2);
		for(; _pul_Byte < _pul_LastByte; _pul_Byte++)
		{
			if(((*_pul_Byte) & 0xFFFFFFFF) == 0xA0110270)
			{
				LINK_PrintStatusMsg("-Memcpy copy of Mark ");
				break;
			}
		}
		*/
		if(*(((ULONG *) _pv_Dest) - 2) == 0xA0110270)
		{
			ul_BlockLength = *(((ULONG *) _pv_Dest) - 1);
		}
		else
		{
			/*~~*/
			int i;
			/*~~*/

			ul_BlockLength = 0;
			for(i = 2; i < 1024; i++)
			{
				if(IsBadReadPtr((((ULONG *) _pv_Dest) - i), 4)) break;
				if((*(((ULONG *) _pv_Dest) - i)) == 0xA0110270)
				{
					ul_BlockLength = *(((ULONG *) _pv_Dest) - (i - 1));
					break;
				}
			}

			if(ul_BlockLength)
			{
				if(i_Size + ((i - 2) << 2) > (int) MEM_ul_GetRealSizeFromSize(ul_BlockLength))
				{
					LINK_PrintStatusMsg("-Memmove warning (Length overpass block size)");
				}
				else
				{
					return(memmove(_pv_Dest, _pv_Src, i_Size));
				}
			}
		}

		if(ul_BlockLength)
		{
			if(i_Size > (int) MEM_ul_GetRealSizeFromSize(ul_BlockLength))
			{
				LINK_PrintStatusMsg("-Memmove warning (Length overpass block size)");
			}
			else
			{
				return(memmove(_pv_Dest, _pv_Src, i_Size));
			}
		}

		_pul_Byte = (ULONG *) _pv_Dest;
		_pul_LastByte = _pul_Byte + (i_Size >> 2);
		for(; _pul_Byte < _pul_LastByte; _pul_Byte++)
		{
			if(((*_pul_Byte) & 0xFFFFFFFF) == 0xA0110270)
			{
				LINK_PrintStatusMsg("Memmove warning (Block mark found)");
			}
		}
	}

	return(memmove(_pv_Dest, _pv_Src, i_Size));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *L_Dbg_memset(void *_pv_Dest, int i_Value, int i_Size)
{
	if(MEM_s_DynamicMemoryCheck)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		ULONG	*_pul_Byte, *_pul_LastByte;
		ULONG	ul_BlockLength;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(!_pv_Dest)
		{
			if(i_Size)
			{
				LINK_PrintStatusMsg("-Memset warning (NULL address detected)");
			}

			return(memset(_pv_Dest, i_Value, i_Size));
		}

		if(*(((ULONG *) _pv_Dest) - 2) == 0xA0110270)
		{
			ul_BlockLength = *(((ULONG *) _pv_Dest) - 1);
		}
		else
		{
			/*~~*/
			int i;
			/*~~*/

			ul_BlockLength = 0;
			for(i = 2; i < 1024; i++)
			{
				if(IsBadReadPtr((((ULONG *) _pv_Dest) - i), 4)) break;
				if((*(((ULONG *) _pv_Dest) - i)) == 0xA0110270)
				{
					ul_BlockLength = *(((ULONG *) _pv_Dest) - (i - 1));
					break;
				}
			}

			if(ul_BlockLength)
			{
				if(i_Size + ((i - 2) << 2) > (int) MEM_ul_GetRealSizeFromSize(ul_BlockLength))
				{
					LINK_PrintStatusMsg("-Memset warning (Length overpass block size)");
				}
				else
				{
					return(memset(_pv_Dest, i_Value, i_Size));
				}
			}
		}

		if(ul_BlockLength)
		{
			if(i_Size > (int) MEM_ul_GetRealSizeFromSize(ul_BlockLength))
			{
				LINK_PrintStatusMsg("-Memset warning (Length overpass block size)");
			}
			else
			{
				return(memset(_pv_Dest, i_Value, i_Size));
			}
		}

		_pul_Byte = (ULONG *) _pv_Dest;
		_pul_LastByte = _pul_Byte + (i_Size >> 2);
		for(; _pul_Byte < _pul_LastByte; _pul_Byte++)
		{
			if(((*_pul_Byte) & 0xFFFFFFFF) == 0xA0110270)
			{
				LINK_PrintStatusMsg("Memset warning (Block mark found)");
			}
		}
	}

	return(memset(_pv_Dest, i_Value, i_Size));
}

#endif

/*$4
 ***********************************************************************************************************************
    Game Cube Functions
 ***********************************************************************************************************************
 */

#ifdef _GAMECUBE

#define MEM_GC_SYSTEM_MEMORY (250 * 1024)
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void gcMEM_PrepareMemoryPool(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	void	*arenalo, *arenahi;
	u32		u32GameARAMSize[eARAM_NumberOfARAMHeapTypes] = { 256 * 1024, (3 * 1024 + 3 * 256 ) * 1024 };
	// must be equal to value in g_u32ARAM_BlocksSize
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OSInit();									/* init the memory system */

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

	/* allouer toute la memoire disponible pour le dynamic */
	MEM_InitDynamic(OSRoundDown32B(((u32) arenahi - (u32) arenalo - MEM_GC_SYSTEM_MEMORY)));

	/* init aram */
	ARAM_Init();
	gcSND_ARAMInit();
	ARAM_CreateHeaps(eARAM_Game, eARAM_NumberOfARAMHeapTypes, u32GameARAMSize);
	ARAM_SetDefaultHeap(eARAM_Game, eARAM_Fix);
	ARAM_gpst_DMA_FIFO = (ARAM_DMA_FIFO *) MEM_p_Alloc(sizeof(ARAM_DMA_FIFO));
	L_memset(ARAM_gpst_DMA_FIFO, 0, sizeof(ARAM_DMA_FIFO));
}

#endif /* _GAMECUBE */

/*$4
 ***********************************************************************************************************************
    public functions (all targets)
 ***********************************************************************************************************************
 */

extern int	GetHeapSize(void);
extern int	GetStackSize(void);
extern int	GetStackStart(void);

#ifdef _XENON
PVOID    g_DbgMemPool; // use debug memory until officially 512MB
// stupid dm allocator does not reinitialize on reset of console so address is hard coded
class DMMemory
{
public:
	DMMemory() { g_DbgMemPool = DmAllocatePool(DM_MEM_SIZE); if(!g_DbgMemPool) g_DbgMemPool = (LPVOID)0xb01c1000; }
	~DMMemory() { if(g_DbgMemPool) DmFreePool(g_DbgMemPool); }
};

//DMMemory DMMemManager;

#endif

/*
 =======================================================================================================================
    Aim:    Init the memory module
 =======================================================================================================================
 */
void MEM_InitModule(void)
{
    extern BOOL LOA_gb_SpeedMode;

	MEM_M_InitLock();

    MEM_gst_MemoryInfo.pv_LastFree = NULL;
    MEM_gst_MemoryInfo.ul_RealSize = 0;

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
		MEM_InitDynamic(ul_alloc);
		if(!MEM_gst_MemoryInfo.pv_DynamicBloc)
		{
			MEM_M_SignalFatalError(ul_alloc, "No more memory to perform init allocation");
		}
	}

#elif defined(PCWIN_TOOL)

	/*$1- PC tool engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
	MEM_InitDynamic(512 * 1024 * 1024)//popowarning;
#else
	MEM_InitDynamic(50 * 1024 * 1024);
#endif
#elif defined(_XENON)

	/*$1- XENON engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Current maximum for dynamic memory set to 96MB

    MEM_InitDynamic(DM_MEM_SIZE);

#elif defined(_XBOX)

	/*$1- XBOX engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(LOA_gb_SpeedMode)
        MEM_InitDynamic( 25*1024*1024 );//26
    else
        MEM_InitDynamic( 30*1024*1024 ); // wave files are scanned in this mode, and needed lot of place during map loading


#if defined( _DEBUG ) || defined( XBOX_TUNING )
    //DmRegisterPerformanceCounter( "DynamicMemory", DMCOUNT_VALUE|DMCOUNT_ASYNC32, &MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated );
#endif

#elif defined(_PC_RETAIL)

	/*$1- PC engine ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_InitDynamic( (ULONG) (26.5 * 1024 * 1024) );

#elif defined(ACTIVE_EDITORS)

	/*$1- PC editor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
	MEM_InitDynamic((LONG) 256 * (LONG) 1024 * (LONG) 1024);
#else
	MEM_InitDynamic((LONG) 100 * (LONG) 1024 * (LONG) 1024);
#endif

#else
#pragma message(__FILE__ ">> error : Target is unknown <<")
#endif

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef MEM_USE_HISTORY
	L_memset(MEM_gap_AllocationHistoryTable, 0, sizeof(MEM_gap_AllocationHistoryTable));
	L_memset(MEM_gap_FreeHistoryTable, 0, sizeof(MEM_gap_FreeHistoryTable));
#endif

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef USE_HOLE_OPTIM
	L_memset(MEM_gax_HoleChain, 0, MEM_Cte_HoleChainSize * sizeof(MEM_tdst_HoleChain));
	MEM_gul_HoleChainNb = 0;
	MEM_gul_LostHoleSize = MEM_gul_LostHoleNb = 0;
	MEM_gp_HoleChainLastElem = NULL;
#endif
}

/*
 =======================================================================================================================
    Aim:    Close the memory module
 =======================================================================================================================
 */
void MEM_CloseModule(void)
{
	/* Close the dynamic memory bloc */
#if defined(_DEBUG) && ( defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL) )
	MEM_CheckDynamicBlocEmpty();
#endif
	MEM_CloseDynamic();
	MEM_M_DestroyLock();
}

/*
 =======================================================================================================================
    Aim:    Allocates the main dynamic block and initialises the Z list.
 =======================================================================================================================
 */
void MEM_InitDynamic(ULONG _ul_BigBlockSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int i, j, iSize, z, iStep;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Allocates the Big Dynamic bloc using static allocation */
#ifdef WIN32

	/* Plus rapide qu'au dessous sous windows */
	MEM_gst_MemoryInfo.pv_DynamicBloc = HeapAlloc(GetProcessHeap(), 0, _ul_BigBlockSize);
#else
	MEM_gst_MemoryInfo.pv_DynamicBloc = (void *) (MEM_p_sAlloc(_ul_BigBlockSize));
#endif
	/* Check that we had enough system memory to perform the allocation */
	ERR_X_ErrorAssert(MEM_gst_MemoryInfo.pv_DynamicBloc != 0, MEM_ERR_Csz_NotEnoughSystemMemory, NULL);

	z = 0;
	iSize = 0;

	/* Initializes the sizes of the entries in the Z list */
	for(i = 0; i < 18; i++)
	{
		/* We calculate the new step (we have 18 different steps) */
		iStep = 1 << (i + 2);

		/* We have 128 z with the same step */
		for(j = 0; j < 128; j++)
		{
			MEM_gst_MemoryInfo.MEM_ast_Zlist[z].ul_NumFreeDefrag = 0;

			iSize += iStep;
			MEM_gst_MemoryInfo.MEM_ast_Zlist[z].ul_Size = iSize;

			/* No holes */
			MEM_gst_MemoryInfo.MEM_ast_Zlist[z].pv_Hole = 0;
			z++;
		}
	}

	/* We init the Next free pointer */
	MEM_gst_MemoryInfo.pv_DynamicNextFree = MEM_gst_MemoryInfo.pv_DynamicBloc;
	MEM_gst_MemoryInfo.ul_DynamicMaxSize = _ul_BigBlockSize;

	/* At the beginning, allocated size is 0 */
	MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated = 0;
	MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated = 0;
	MEM_gst_MemoryInfo.ul_HolesStatic = 0;
}

/*
 =======================================================================================================================
    Aim:    Frees the memory of the main dynamic module
 =======================================================================================================================
 */
void MEM_CloseDynamic(void)
{
#ifdef WIN32
	HeapFree(GetProcessHeap(), 0, MEM_gst_MemoryInfo.pv_DynamicBloc);
#else
	MEM_sFree(MEM_gst_MemoryInfo.pv_DynamicBloc);
#endif
}

#ifdef _GAMECUBE
ULONG MEM_uGetLastBlockSize(MEM_tdst_MainStruct *)
{
	u32		Arena;
	u32		Memory;
	
	Arena = (u32) OSGetArenaHi() - (u32) OSGetArenaLo() - MEM_GC_SYSTEM_MEMORY;
	Memory = (int) MEM_gst_MemoryInfo.pv_DynamicNextFree - (int) MEM_gst_MemoryInfo.pv_DynamicBloc;
	
	return (Arena - Memory);	
}
#endif //_GAMECUBE

#ifdef MEM_USE_HISTORY

/*
 =======================================================================================================================
    Aim:    Check that the dynamic bloc is completely empty
 =======================================================================================================================
 */
void MEM_CheckDynamicBlocEmpty(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	*p, *pfinal;
	LONG	ul_Size, ul_first4bytes;
	void	*pv_blockAddress;
	LONG	nb_blocs, i;
	extern BOOL ENG_gb_SlashL;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#if defined(_DEBUG) && defined(PCWIN_TOOL) && !defined(ACTIVE_EDITORS)
	// no check when loading with "pc-engine" + "batch mode"
	if(ENG_gb_SlashL) return;
#endif



#ifdef ACTIVE_EDITORS
    {
    	// no check when binarizing
        extern BOOL LOA_gb_SpeedMode ;
        extern BOOL EDI_gb_ComputeMap;
        extern BOOL	 EDI_gb_SlashC ;
        extern BOOL EDI_gb_NoVerbose;
        if(LOA_gb_SpeedMode || EDI_gb_ComputeMap || EDI_gb_SlashC || EDI_gb_NoVerbose) return;
    }
#endif


	/* We loop thru the whole memory to find the mark of an allocated block */
	p = (LONG *) MEM_gst_MemoryInfo.pv_DynamicBloc;
	pfinal = (p + (MEM_gst_MemoryInfo.ul_DynamicMaxSize >> 2));
	nb_blocs = 0;
	for(; p < pfinal; p++)
	{
		if
		(
			(*p == (LONG) (MEM_Cul_BlockMarkMEMpAlloc + MEM_Cul_BlockAllocated))
		||	(*p == (LONG) (MEM_Cul_BlockMarkMEMpAllocAlign + MEM_Cul_BlockAllocated))
		)
		{
			/*
			 * We found the mark of an allocated block, but is is possible that it is not a
			 * real allocated adress so we check in the table of all allocated pointers
			 */
			for(i = 0; i < MEM_Cul_AllocationHistorySize; i++)
			{
				if(MEM_gap_AllocationHistoryTable[i] == (LONG) (p + 2))
				{
					nb_blocs++;
					if(nb_blocs == 1) MEM_LogString(MEM_ERR_Csz_JadeMemoryManagementStart);

					/* Found the mark of a non empty block */
					pv_blockAddress = p + 2;
					ul_first4bytes = *(ULONG *) pv_blockAddress;
					ul_Size = *(p + 1);
#if defined ( MEM_USE_TYPES ) && defined ( JADEFUSION )
                    if (ul_Size & 0x80000000)
                    {
                        ul_Size &= ~0x80000000;
                    }
                    else
                    {
                        ul_Size &= 0xffffff;
                    }
#endif
					MEM_Log
					(
						MEM_ERR_Csz_BlockNotFreed,
						pv_blockAddress,
						ul_Size,
						ul_first4bytes,
						MEM_gapsz_AllocatedInFile[i],
						MEM_gai_AllocatedInLine[i]
					);
					continue;
				}
			}
		}
	}

	if(nb_blocs > 0) MEM_LogString(MEM_ERR_Csz_JadeMemoryManagementEnd);
}

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
	char	sz_Text[256];
	ULONG	*pul_Address;
	ULONG	ul_Size;
	/*~~~~~~~~~~~~~~~~~*/

	ul_Size = 0;
	for(i = 0; i < MEM_Cul_AllocationHistorySize; i++)
	{
		pul_Address = (ULONG *) MEM_gap_AllocationHistoryTable[i];
		if(!pul_Address) continue;

		ul_Size += pul_Address[-1];

		if(_c_Dump & 2)
		{
			sprintf(sz_Text, "0x%08X, %8d", (LONG) pul_Address, pul_Address[-1]);
			LINK_PrintStatusMsg(sz_Text);
		}

		if
		(
			(pul_Address[-2] != (ULONG) (MEM_Cul_BlockMarkMEMpAlloc + MEM_Cul_BlockAllocated))
		&&	(pul_Address[-2] != (ULONG) (MEM_Cul_BlockMarkMEMpAllocAlign + MEM_Cul_BlockAllocated))
		)
		{
			sprintf(sz_Text, "Bad mark on allocated block : 0x%08X, %8d", (LONG) pul_Address, pul_Address[-1]);
			ERR_X_ErrorAssert(0, sz_Text, NULL);
		}
	}

	if(_c_Dump & 1)
	{
		sprintf(sz_Text, "Mem : %d pointers, total size : %d", MEM_g_nbpointers, ul_Size);
		LINK_PrintStatusMsg(sz_Text);
	}
}

void MEM_PrintHistory(void)
{
	int i, j;
	ULONG*pSize;
	
	for(i=j=0; i<MEM_Cul_AllocationHistorySize; i++)
	{
		if(!MEM_gap_AllocationHistoryTable[i]) continue;
		j++;
		
		pSize = (ULONG*)MEM_gap_AllocationHistoryTable[i];
		
		printf("%s(%d) %d\n",
		MEM_gapsz_AllocatedInFile[i],
		MEM_gai_AllocatedInLine[i],
		*(pSize-1)	
		);
		
		if(j >= MEM_g_nbpointers) break;
	}
	
	printf("-----------\n");
}
#else /* MEM_USE_HISTORY */
void MEM_PrintHistory(void)
{
}
#endif /* MEM_USE_HISTORY */

#ifdef _DEBUG
#define MEM_Csz_PointerOutOfRange	"Pointer out of range detected"
#define MEM_Csz_NonalignedPointer	"Non aligned pointer detected"
#define MEM_Csz_NullPointerDetected "Null pointer detected"

/*
 =======================================================================================================================
    Aim:    Check the validity of a pointer
 =======================================================================================================================
 */
char MEM_b_CheckPointer(void *p)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_max, l_min, l_p;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	l_p = (LONG) p;

	ERR_X_ErrorAssert(l_p, MEM_Csz_NullPointerDetected, NULL);

	l_min = (LONG) MEM_gst_MemoryInfo.pv_DynamicBloc;
	l_max = (LONG) MEM_gst_MemoryInfo.pv_DynamicBloc + MEM_gst_MemoryInfo.ul_DynamicMaxSize;

	ERR_X_ErrorAssert(((l_p > l_min) && (l_p < l_max)), MEM_Csz_PointerOutOfRange, "");
	ERR_X_ErrorAssert(((l_p & 3) == 0), MEM_Csz_NonalignedPointer, "");
_Catch_
	return(0);
_End_
	return(1);
}

#endif /* _DEBUG */

/*
 =======================================================================================================================
    Aim:    We check that the Main dynamic bloc is not full
 =======================================================================================================================
 */
_inline_ void MEM_CheckDynamicBlocFull(void)
{
	/*~~~~~~~~~~~~~~~*/

	/* Check that we had enough memory in the dynamic bloc */
	char	c_SizeTest;
	/*~~~~~~~~~~~~~~~*/

	c_SizeTest =
		(
			((char *) MEM_gst_MemoryInfo.pv_DynamicNextFree) -
			((char *) MEM_gst_MemoryInfo.pv_DynamicBloc) < (LONG) MEM_gst_MemoryInfo.ul_DynamicMaxSize
		);

	ERR_X_ErrorAssert(c_SizeTest, MEM_ERR_Csz_NoMoreMemoryToPerformAllocation, NULL);
}

/* Aim: We check that a block has been correctly allocated (check the mark) */
#ifdef _DEBUG

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char MEM_b_CheckBlockAllocated(void *_pv_Block)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG	ul_size;
	ULONG	ul_BlockMark;
	/*~~~~~~~~~~~~~~~~~*/

_Try_
	/* Check if a pointer was correctly allocated */
	if(MEM_b_CheckPointer(_pv_Block))
	{
		ul_BlockMark = MEM_ul_GetMark(_pv_Block);

		/*$F hints to debug if asserts here:
			1: watch (ul_BlockMark&0xFFFFFF):
					if different from 0x110270, the bloc has been allocated with
					another function than MEM_p_Alloc, or memory has been overwriten.
			2: watch (ul_BlockMark&0xF0000000):  
					The normal value should be 0xA0000000
					if equals 0xB0000000, the bloc has already been freed ! 
					
		*/
		ERR_X_Assert((ul_BlockMark & 0xF0000000) == MEM_Cul_BlockAllocated);
		ERR_X_Assert
		(
			(
				((ul_BlockMark & 0xFFFFFF) == MEM_Cul_BlockMarkMEMpAlloc)
			||	((ul_BlockMark & 0xFFFFFF) == MEM_Cul_BlockMarkMEMpAllocAlign)
			)
		);

		/* Check that the size is coherent */
		ul_size = MEM_ul_GetSize(_pv_Block);
		ERR_X_Assert((ul_size > 0) && (ul_size < MEM_Cul_MaxSize));
	}

_Catch_
	return 0;
_End_
	return 1;
}

/*
 =======================================================================================================================
    Aim:    We check that a block is free
 =======================================================================================================================
 */
void MEM_CheckBlockIsFree(void *_pv_Block)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG	ul_BlockMark;
	/*~~~~~~~~~~~~~~~~~*/

	if(_pv_Block != 0)
	{
		ul_BlockMark = MEM_ul_GetMark(_pv_Block);

		/* Check that the block is free */
		ERR_X_Assert((ul_BlockMark & 0xF0000000) == MEM_Cul_BlockFree);

		/* Check it has been freed by MEM_p_Free */
		ERR_X_Assert((ul_BlockMark & 0xFFFFFF) == MEM_Cul_BlockMarkMEMpAlloc);
	}
}

/*
 =======================================================================================================================
    Aim:    We check that the holes chain is OK
 =======================================================================================================================
 */
void MEM_CheckHolesChain(void *_pv_Hole, ULONG index)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	test;
	ULONG	ul_EffSize;
	ULONG	*p;
	/*~~~~~~~~~~~~~~~*/

	ul_EffSize = MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size +
	MEM_C_BlockHeaderSize;

	if(_pv_Hole)
	{
		test = *(LONG *) _pv_Hole;
		while(test != 0)
		{
			/* The start of the block is always 4 bytes after the pointer (in debug or not) */
			MEM_CheckBlockIsFree((void *) (test + 4));

			p = (ULONG *) test;
			ERR_X_Assert(*(p + (ul_EffSize >> 2) - 2) == ul_EffSize);

			test = *(LONG *) test;
		}
	}
}

#endif /* _DEBUG */

#ifdef USE_HOLE_OPTIM
extern void MEM_UpdateHoles();
extern BOOL MEM_b_FindOldHole(ULONG);
#endif
/*$off*/
/*
 =======================================================================================================================
  Aim:  Allocates a block of memory Note: (Must be less than 1 Mb) £
        The size of the block is stored. £
 =======================================================================================================================
 */
#ifdef _DEBUG
	void *_MEM_p_Alloc(ULONG _ul_BlockSize, char *_psz_File, int _i_Line)
#else
	void *_MEM_p_Alloc(ULONG _ul_BlockSize)
#endif
/*$on*/
{
	ULONG				ul_size, index;
	void				**ppv_Hole;
	void				*pv_Return;
	ULONG				ul_EffSize;
	char				c_SizeTest;
	MEM_tdst_ZlistEntry *pst_Zelem;
	extern char			*MEM_gp_MaxBlock;
#ifdef USE_HOLE_OPTIM
	BOOL				b_FoundInOldList = FALSE;
	BOOL				b_Emergency;
#endif	

_Try_
#if defined(PSX2_TARGET) || defined(_GAMECUBE) || defined(_XBOX) || defined(_XENON)
	if(_ul_BlockSize == 0) return NULL;
#endif
 	 		
	ERR_X_Assert(_ul_BlockSize > 0);
	if(_ul_BlockSize < 8) _ul_BlockSize = 8;

	MEM_M_BeginCriticalSection();
		
#ifdef USE_HOLE_OPTIM
	MEM_ul_UpdateHoles ++;
	if(MEM_ul_UpdateHoles == 100)
	{
	 	MEM_UpdateHoles();
	 	MEM_ul_UpdateHoles = 0;
	 }	 
#endif
	
#ifdef USE_HOLE_OPTIM
	b_Emergency = FALSE;
start:
#endif	
	/* We calculate the index in the Z list and get the real size to allocate */
	MEM_GetIndexAndRealSize(&index, &ul_size, _ul_BlockSize);
	pst_Zelem = &MEM_gst_MemoryInfo.MEM_ast_Zlist[index];

	MEM_gst_MemoryInfo.ul_HolesStatic += (pst_Zelem->ul_Size - _ul_BlockSize);

	/* Get a pointer to existing hole */
	pst_Zelem->ul_Count++;
	ppv_Hole = &(pst_Zelem->pv_Hole);
	if(*ppv_Hole)
	{
hole:
		MEM_DeleteHoleInChain(*ppv_Hole);

#ifdef _DEBUG
#ifdef _MEMORY_MEGADEBUG
		/* Check the whole chain of holes at the current index */
		MEM_CheckHolesChain(pst_Zelem->pv_Hole, index);
#endif
		/* Check the hole is a correct free block */
		MEM_CheckBlockIsFree(((LONG *) (*ppv_Hole)) + 1);
#endif
		/* Pointer to return adress */
		pv_Return = (void *) (((LONG *) (*ppv_Hole)) + 1);

		/*
		 * We set the pv_Hole entry of the zlist with the next hole £
		 * Note that When a block is free, MEM_ul_GetSize returns the pointer to next hole
		 */
		*ppv_Hole = (void *) *(((ULONG *) pv_Return) - 1);
	}	
/*	
	else if((pv_Return = MEM_p_GetHoleInChain(_ul_BlockSize)))
	{
		ppv_Hole = MEM_p_SplitHoleInChain(pv_Return, _ul_BlockSize);
		if(!ppv_Hole) goto nohole;
		goto hole;
	}
*/	
#ifdef USE_HOLE_OPTIM
	else if(1)
	{	
			
		ppv_Hole = NULL;
		if(MEM_gp_OldHolesList)
			ppv_Hole = MEM_p_AllocInOldHoles(_ul_BlockSize);
			
		if(!ppv_Hole && MEM_b_FindOldHole(_ul_BlockSize))
			ppv_Hole = MEM_p_AllocInOldHoles(_ul_BlockSize);				
				
		if(!ppv_Hole) goto nohole;
		
		b_FoundInOldList = TRUE;
		MEM_DeleteHoleInChain(*ppv_Hole);

#ifdef _DEBUG
		/* Check the hole is a correct free block */
		MEM_CheckBlockIsFree(((LONG *) (*ppv_Hole)) + 1);
#endif
		/* Pointer to return adress */
		pv_Return = (void *) (((LONG *) (*ppv_Hole)) + 1);
	}
#endif	
	else
	{
#ifndef PSX2_TARGET
		ULONG		ul_NumHoles;
		ULONG		ul_SizeOfHoles;
		char		asz_Buf[1000];
#endif		
		
#ifdef USE_HOLE_OPTIM
nohole:
#endif
		c_SizeTest =
			(
				((char *) MEM_gst_MemoryInfo.pv_DynamicNextFree + ul_size + MEM_C_BlockHeaderSize) -
					((char *) MEM_gst_MemoryInfo.pv_DynamicBloc) < (LONG) MEM_gst_MemoryInfo.ul_DynamicMaxSize
			);
		if(!c_SizeTest)
		{
#ifdef USE_HOLE_OPTIM		
			if(!b_Emergency)
			{
				MEM_EmergencyConcat();
				b_Emergency = TRUE;
				goto start;
			}
#endif			
			for(pst_Zelem++; pst_Zelem < &MEM_gst_MemoryInfo.MEM_ast_Zlist[MEM_C_ZlistSize]; pst_Zelem++)
			{
				ppv_Hole = &pst_Zelem->pv_Hole;
				if(*ppv_Hole)
				{
					_ul_BlockSize += pst_Zelem->ul_Size -
					ul_size;
					ul_size = pst_Zelem->ul_Size;
					goto hole;
				}
			}
			MEM_M_EndCriticalSection();


#ifndef PSX2_TARGET
			MEM_ComputeMemoryHoleInfo(&ul_NumHoles, &ul_SizeOfHoles, FALSE);
			sprintf
				(
					asz_Buf, "[Engine %u ko -- Textures %u ko -- WH %u ko -- Gran %u ko -- Holes %u -- Size Holes %u ko]", MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated /
					1024, MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated /
					1024,
							((char *) MEM_gst_MemoryInfo.pv_DynamicNextFree - (char *) MEM_gst_MemoryInfo.pv_DynamicBloc) /
						1024, MEM_gst_MemoryInfo.ul_HolesStatic /
						1024, ul_NumHoles, ul_SizeOfHoles /
						1024
				);
#endif

			MEM_M_SignalFatalError(_ul_BlockSize, MEM_ERR_Csz_NoMoreMemoryToPerformAllocation);
			return NULL;
		}

		/*
		 * We don't have a hole, so we return the next free adress (+MEM_C_BlockHeaderSize
		 * bytes, because we store the size in the MEM_C_BlockHeaderSize first bytes)
		 */
		else
		{
			/* Pointer to return */
			pv_Return = (void *) (((char *) (MEM_gst_MemoryInfo.pv_DynamicNextFree)) + MEM_C_BlockHeaderSize);
			MEM_gst_MemoryInfo.pv_DynamicNextFree = (void *) ((char *) pv_Return + ul_size);
#if !defined(MAX_PLUGIN)
			MEM_gp_MaxBlock = (char*)MEM_gst_MemoryInfo.pv_DynamicNextFree;
#endif
#ifdef _DEBUG
			if(MEM_gp_MaxBlock > MEM_gp_MaxPeakMemory) 
			{
				MEM_gp_MaxPeakMemory = MEM_gp_MaxBlock;
			}
#endif
#if !defined(MAX_PLUGIN)
			if((char *) MEM_gp_MaxBlock >= gp_LastPointer - PRELOADTEXGRAN)
			{
				PreloadCancel();
				PreLoadTexReset();
			}
#endif // !defined(MAX_PLUGIN)
		}
	}

	if(1)
	{
		char 	*pLast;
		ULONG	ulTest;

		ulTest = (MEM_gst_MemoryInfo.ul_DynamicMaxSize > 32 * 1024 * 1024) ? MEM_gst_MemoryInfo.ul_DynamicMaxSize - (32 * 1024 * 1024) : MEM_gst_MemoryInfo.ul_DynamicMaxSize;
		pLast = (char *)MEM_gst_MemoryInfo.pv_DynamicBloc + ulTest;
		if(((int) pLast - (int) MEM_gst_MemoryInfo.pv_DynamicNextFree) < MEM_gi_MinDelta)
		{
			MEM_gi_MinDelta = (int) pLast - (int) MEM_gp_MaxBlock;
		}								
	}
	

	/* Set the size of the block */
	MEM_SetSize(pv_Return, _ul_BlockSize);

	/* We check that the bloc is not full */
	MEM_CheckDynamicBlocFull();

	/* We update the total allocated size */
	ul_EffSize = (pst_Zelem->ul_Size + MEM_C_BlockHeaderSize);
	MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated += ul_EffSize;

#ifdef MEMpro_M_ActiveRasters
	if(!MEMpro_gb_StackOverflow) MEMpro_gp_StackPointer->ui_Alloc += (ul_EffSize - 4);
#endif
	/* Mark the block as allocated with MEM_p_Alloc */
	MEM_SetMark(pv_Return, MEM_Cul_BlockMarkMEMpAlloc + MEM_Cul_BlockAllocated);

/*#ifdef JADEFUSION
#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) && !defined(_XBOX) && !defined(_PC_RETAIL)//popowarning
#endif*/

#ifdef MEM_USE_HISTORY
	/*
	 * If asserts here, the allocation history buffer is full, if you continue, it
	 * will be cleared, but invalid holes may be detected when exiting
	 */
	if(MEM_g_nbpointers >= MEM_Cul_AllocationHistorySize)
	{
		//ERR_X_ForceError("Simple Info: Memory allocation history buffer is full and will be cleared", NULL);
		printf("Simple Info: Memory allocation history buffer is full and will be cleared\n");
		MEM_g_nbpointers = 0;
	}
	else
	{
		int i_Key;

		/* Fills the history table with the new pointer */
		i_Key = ((int) pv_Return) ;
		i_Key = i_Key % MEM_Cul_AllocationHistorySize;

		while(MEM_gap_AllocationHistoryTable[i_Key])
		{
			i_Key = (i_Key + 1) % MEM_Cul_AllocationHistorySize;
		}
		
		MEM_gapsz_AllocatedInFile[i_Key] = _psz_File;
		MEM_gai_AllocatedInLine[i_Key] = _i_Line;
		MEM_gap_AllocationHistoryTable[i_Key] = (LONG) pv_Return;
		MEM_gap_FreeHistoryTable[i_Key] = 0;
		MEM_g_nbpointers++;
	}
#endif /* MEM_USE_HISTORY */


#ifdef _DEBUG
#ifdef _MEMORY_MEGADEBUG

	/* Check the whole chain of holes at the current index */
	MEM_CheckHolesChain(pst_Zelem->pv_Hole, index);

	/* Check The block has been correctly allocated */
	MEM_b_CheckBlockAllocated(pv_Return);
#endif /* _MEMORY_MEGADEBUG */
#endif /* _DEBUG */
_Catch_
	pv_Return = 0;
_End_
#ifdef _DEBUG
	{
		static void *pv_BreakAdr = NULL;
		static char *pv_TestAdr = NULL;

		if(pv_Return == pv_BreakAdr)
			pv_Return = pv_BreakAdr;
		if((char *) pv_Return < pv_TestAdr && (char *) pv_Return + _ul_BlockSize > pv_TestAdr)
		{
			int i;

			i = 1;

/*#ifdef _GAMECUBE			
			OSReport("Temp\n");
#endif			*/
			pv_Return = pv_Return;
		}
	}
#endif
	MEM_M_EndCriticalSection();

#ifdef ACTIVE_EDITORS
    L_memset(pv_Return, 0, _ul_BlockSize);
#endif
#if defined(MEM_USE_TYPES) && defined(_DEBUG) && !defined(_XENON)
	MEM_SetType(pv_Return, MEM_GetTypeFromName(_psz_File));
	MEM_IncrementType(MEM_GetTypeFromName(_psz_File), _ul_BlockSize);
#endif

	return(pv_Return);
}

/*
 =======================================================================================================================
    Aim:    Free a block allocated with MEM_p_Alloc()

    Note:   Syntax Works just like the standard malloc

    Ex:     P=MEM_p_Alloc(sizeof(struct X)) £
            MEM_Free(P);
 =======================================================================================================================
 */
void MEM_Free(void *_pv_Block)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	index;
	ULONG	*pul_Size;
	void	**ppv_Hole;
	ULONG	ul_EffSize;
	ULONG	ulsize;
	/*~~~~~~~~~~~~~~~*/

	if(!_pv_Block) return;



	/* Pointeur temporaire ? */
	if(MEM_IsTmpPointer((char *) _pv_Block))
	{
		MEM_FreeTmp(_pv_Block);
		return;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~*/
		void	*p;
		ULONG	ul_BlockMark;
		/*~~~~~~~~~~~~~~~~~*/

		p = (void *) *
		((ULONG *) _pv_Block - 1);
		if(MEM_Mb_IsAllocPointer(p) && (((unsigned int) p % 4) == 0))
		{
			ul_BlockMark = MEM_ul_GetMark(p);
			if(ul_BlockMark == (MEM_Cul_BlockMarkMEMpAllocAlign + MEM_Cul_BlockAllocated))
			{
				MEM_SetMark(p, MEM_Cul_BlockMarkMEMpAlloc + MEM_Cul_BlockAllocated);
				_pv_Block = p;
			}
		}
#if defined(MEM_USE_TYPES) && defined(_DEBUG) && !defined(_XENON)
		MEM_IncrementType(MEM_ul_GetType(_pv_Block), -(s32)MEM_ul_GetSize(_pv_Block));
		MEM_ul_ResetType(_pv_Block);
#endif

	}

#if defined(PSX2_TARGET)
	* (ULONG *) &_pv_Block &= 0x0fffffff;
#endif
	MEM_M_BeginCriticalSection();

#ifdef _DEBUG
	MEM_b_CheckBlockAllocated(_pv_Block);
#endif
	/* Read the stored block size */
	pul_Size = (((ULONG *) _pv_Block) - 1);
	ulsize = *pul_Size;


	/* We calculate the index in the Z list */
	index = MEM_ul_GetIndex(*pul_Size);
	ul_EffSize = (MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size + MEM_C_BlockHeaderSize);

	MEM_gst_MemoryInfo.ul_HolesStatic -= (MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size - *pul_Size);

	/* The last one ? */
	if(MEM_gst_MemoryInfo.pv_DynamicNextFree == (char *) _pv_Block + ul_EffSize - MEM_C_BlockHeaderSize)
	{
		MEM_gst_MemoryInfo.pv_DynamicNextFree = (void *) ((ULONG) MEM_gst_MemoryInfo.pv_DynamicNextFree - ul_EffSize);

		/* no Mark the block is not a cluster */
		MEM_SetMark(_pv_Block, 0);
	}
	else
	{
		/* Get a pointer to existing hole */
		ppv_Hole = &(MEM_gst_MemoryInfo.MEM_ast_Zlist[index].pv_Hole);

		/* We replace the block size by the next hole... */
		*pul_Size = (ULONG) (*ppv_Hole);
		
#ifdef USE_HOLE_OPTIM
		if(ul_EffSize >= 20)
			*(pul_Size + 2) = 0;
#endif		
		
		/* Taille en debut de bloc */
		*(pul_Size + 1) = ul_EffSize;

		/* Taille en fin de bloc */
		*(pul_Size + (ul_EffSize >> 2) - 2) = ul_EffSize;

		/* The just freed block is the next hole to take */
		(*ppv_Hole) = (void *) pul_Size;

		/* record into HoleChain */
		MEM_RegisterHoleInChain(pul_Size, index);

		/* Mark the block as free */
		MEM_SetMark(_pv_Block, MEM_Cul_BlockMarkMEMpAlloc + MEM_Cul_BlockFree);

#ifdef USE_HOLE_OPTIM
//	((ULONG) MEM_gst_MemoryInfo.pv_DynamicNextFree ) - ((ULONG) MEM_gst_MemoryInfo.pv_DynamicBloc)
/*
		if(1)
		{
			MEM_b_ConcatHole((void *)pul_Size, 0);
			MEM_b_ConcatHole((void *)pul_Size, 1);				
		}
*/		
#endif	
		
		
	}
	

	/* In debug and editors, we update the total allocated size */
	MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated -= ul_EffSize;

#ifdef MEMpro_M_ActiveRasters
	if(!MEMpro_gb_StackOverflow) MEMpro_gp_StackPointer->ui_Free += (ul_EffSize - 4);
#endif

#ifdef MEM_USE_HISTORY
	{
		/*~~~~~~*/
		int i_Key;
		int i_cmpt= 0 ; 
		/*~~~~~~*/
		

		i_Key = ((int) _pv_Block) ;
		i_Key = i_Key % MEM_Cul_AllocationHistorySize;
		
		while((ULONG) MEM_gap_AllocationHistoryTable[i_Key] != (ULONG) _pv_Block)
		{
			i_Key = (i_Key + 1) % MEM_Cul_AllocationHistorySize;
			i_cmpt ++ ;
			
			if ( i_cmpt > MEM_Cul_AllocationHistorySize ) 
			{
				LINK_PrintStatusMsg("ERROR - MEM_Free() - 2 MEM_Free has been called for the same address");
				MEM_M_EndCriticalSection();
				return;
			}
		}

		MEM_gap_FreeHistoryTable[i_Key] = MEM_gap_AllocationHistoryTable[i_Key];
		MEM_gapsz_FreeInFile[i_Key] = MEM_gapsz_AllocatedInFile[i_Key];
		MEM_gai_FreeInLine[i_Key] = MEM_gai_AllocatedInLine[i_Key];

		MEM_gap_AllocationHistoryTable[i_Key] = 0;
		MEM_g_nbpointers--;
	}
#endif


#ifdef _DEBUG
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		static void *pv_BreakAdr = NULL;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(_pv_Block == pv_BreakAdr)
		{
			_pv_Block = pv_BreakAdr;
		}
	}
#endif	


#if defined(WIN32) && !defined(ACTIVE_EDITORS) && defined(_DEBUG)
	if(ul_EffSize > 16) 
	{
/*		ULONG iii;
		for(iii = 0; iii < (ul_EffSize - 16) / 4; iii++)
		{
			((int *) _pv_Block)[iii] = MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc;
		}*/

		L_memset(((char *) _pv_Block) + 4, 0xFF, ul_EffSize - 16);
	}
#endif


	/* Check the whole chain of holes */
#ifdef _MEMORY_MEGADEBUG
	MEM_CheckHolesChain(MEM_gst_MemoryInfo.MEM_ast_Zlist[index].pv_Hole, index);
#endif

	MEM_M_EndCriticalSection();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int s_Bichon = 0;
void MEM_Defrag(int single)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					index, size;
	ULONG				*p, *p1, *p2;
	MEM_tdst_ZlistEntry *pst_Zelem;
	void				*next;
	void				*next_prev;
	int					i;
	int					fuckingE3;
	static int			i_Last = 0;
#ifdef USE_HOLE_OPTIM
	char				asz_Log[512];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef MEM_USE_TYPES
	return;
#endif

	fuckingE3 = 0;
	MEM_M_BeginCriticalSection();

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    try to concat some free clusters
	 -------------------------------------------------------------------------------------------------------------------
	 */
	 
	for(i = i_Last; i < MEM_C_ZlistSize; i++)
	{
		pst_Zelem = MEM_gst_MemoryInfo.MEM_ast_Zlist +
		i;
		if(pst_Zelem->pv_Hole)
		{
			MEM_b_ConcatHolesInChain((ULONG *) pst_Zelem->pv_Hole, i);
			if(single) break;
		}
	}

	i_Last = (i < MEM_C_ZlistSize) ? i : 0;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    looking for free mark from end of memory
	 -------------------------------------------------------------------------------------------------------------------
	 */

	while(1)
	{
		p = (ULONG *) MEM_gst_MemoryInfo.pv_DynamicNextFree;
		if(p == MEM_gst_MemoryInfo.pv_DynamicBloc) break;

		size = *(p - 1);
		if(size <= 0) break;
		if(size & 3) break;
		if(size < MEM_C_BlockHeaderSize) break;

		p1 = p - (size >> 2);
		if(p1 <= (ULONG *) MEM_gst_MemoryInfo.pv_DynamicBloc) break;
		if(p1 > (ULONG *) MEM_gst_MemoryInfo.pv_DynamicNextFree) break;

		if(*p1 == (MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc))
		{
			if(*(p1 + 1) && (*(p1 + 1) < (ULONG) MEM_gst_MemoryInfo.pv_DynamicBloc)) break;
			if(*(p1 + 1) > (ULONG)((char *) MEM_gst_MemoryInfo.pv_DynamicNextFree + MEM_gst_MemoryInfo.ul_DynamicMaxSize)) break;
			if(*(p1 + 2) != (ULONG)size) break;
			p2 = (ULONG *) *(p1 + 1);

			if(p2)
			{
			
				if(*(p2 - 1) != (MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc)) 
				{
					ERR_X_Assert(0);
					break;
				}
				if(*p2 && *p2 < (ULONG) MEM_gst_MemoryInfo.pv_DynamicBloc) 
				{
					ERR_X_Assert(0);
					break;
				}

				if(*p2 > (ULONG)((char *) MEM_gst_MemoryInfo.pv_DynamicNextFree + MEM_gst_MemoryInfo.ul_DynamicMaxSize)) 
				{
					ERR_X_Assert(0);
					break;
				}

#ifndef _GAMECUBE
				if(*(p2 + 1) != (ULONG)size)
				{
					ERR_X_Assert(0);
					break;
				}
#endif				
			}

			index = MEM_ul_GetIndex(size - MEM_C_BlockHeaderSize);
			MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_NumFreeDefrag++;
#ifdef JADEFUSION
			//POPOWARNING (char *) MEM_gst_MemoryInfo.pv_DynamicNextFree -= size;
#else
			(char *) MEM_gst_MemoryInfo.pv_DynamicNextFree -= size;
#endif
			fuckingE3 = size;
			if(single) break;
		}
		else
			break;
			
	}
	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    unlink all free buffers
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i = 0; i < MEM_C_ZlistSize; i++)
	{
		pst_Zelem = &MEM_gst_MemoryInfo.MEM_ast_Zlist[i];
		if(!pst_Zelem->ul_NumFreeDefrag) continue;

		next = pst_Zelem->pv_Hole;
		if(!next)
		{
#ifdef JADEFUSION
			if(single) MEM_gst_MemoryInfo.pv_DynamicNextFree = (void*)((int)MEM_gst_MemoryInfo.pv_DynamicNextFree + fuckingE3);//(char *) MEM_gst_MemoryInfo.pv_DynamicNextFree += fuckingE3;
#else
			if(single) (char *) MEM_gst_MemoryInfo.pv_DynamicNextFree += fuckingE3;
#endif
			pst_Zelem->ul_NumFreeDefrag = 0;
			continue;
		}

		next_prev = NULL;
		while(pst_Zelem->ul_NumFreeDefrag)
		{
			while((char *) next - 4 < (char *) MEM_gst_MemoryInfo.pv_DynamicNextFree)
			{
				next_prev = next;
				next = *(void **) next;
#ifndef USE_HOLE_OPTIM				
				ERR_X_Assert(next);
#endif
			}

#ifdef USE_HOLE_OPTIM
			if(!next)
			{
				pst_Zelem->ul_NumFreeDefrag = 0;
				continue;
			}
#endif			
			
			*((ULONG **) next - 1) = 0; /* Clean free mark */
			MEM_DeleteHoleInChain((ULONG *) next);

			pst_Zelem->ul_NumFreeDefrag--;
			next = *(void **) next;
			if(!next_prev)
			{
				pst_Zelem->pv_Hole = next;
			}
			else
			{
				*(void **) next_prev = next;
			}
		}
	}

#ifdef USE_HOLE_OPTIM
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    unlink all free buffers from OldHolesList
	 -------------------------------------------------------------------------------------------------------------------
	 */
	 {
	 	int test;
	 	
		 if(MEM_gp_OldHolesList)
		 {
		 	ULONG	*pul_Hole;
		 	ULONG	*pul_PreviousHole;
		 	
		 	
		 	test = 0;
		 	pul_PreviousHole = NULL;
		 	pul_Hole = MEM_gp_OldHolesList;
pipik:		 			 	
		 	while(pul_Hole)
		 	{
				/*$F
				sprintf(asz_Log, "Hole = %x / Next Free = %x\n", (ULONG) pul_Hole, (ULONG) MEM_gst_MemoryInfo.pv_DynamicNextFree);
				if(s_Bichon)
					OSReport(asz_Log);
				 */
					
			 	test ++;
			 	/*$F
		 		if((ULONG) pul_Hole == (ULONG) MEM_gst_MemoryInfo.pv_DynamicNextFree)
		 			OSReport("Cas batard à traiter\n");
		 		 */
		 					 			
		 		if((ULONG) pul_Hole > (ULONG) MEM_gst_MemoryInfo.pv_DynamicNextFree)
		 		{
		 			if(pul_PreviousHole)
		 			{
						MEM_DeleteHoleInChain((ULONG *) pul_Hole);		 			
		 				*pul_PreviousHole = *pul_Hole;
		 				MEM_ul_NumOldHoles --;
						test --;		 				
						MEM_ul_SizeOldHoles -= *(pul_Hole + 1);
		 				*(pul_Hole - 1) = 0; /* Clean Free Mark */
						
						/* To loop again on the previous */
						pul_Hole = pul_PreviousHole;						
		 			}
		 			else
		 			{
						MEM_DeleteHoleInChain((ULONG *) pul_Hole);		 					 			
		 				MEM_gp_OldHolesList = (ULONG *) *pul_Hole;
						MEM_ul_NumOldHoles --;
						test --;
						MEM_ul_SizeOldHoles -= *(pul_Hole + 1);
		 				*(pul_Hole - 1) = 0; /* Clean Free Mark */
		 				pul_Hole = (ULONG *) *pul_Hole;
				 		pul_PreviousHole = NULL;				 		
		 				goto pipik;

		 			}		 			
		 		}
		 		pul_PreviousHole = pul_Hole;
		 		pul_Hole = (ULONG *) *pul_Hole;
		 	}
		 	
		 	/*$F
		 	if((ULONG)test != MEM_ul_NumOldHoles)
		 	{
		 		OSReport("Defrag PB\n");
		 	}
		 	*/
		 }	 	
	 }
#endif	
	
	

	MEM_M_EndCriticalSection();
	
	if(!single) MEM_gp_NextFreeAfterDefrag = MEM_gst_MemoryInfo.pv_DynamicNextFree;
}

/*$4
 ***********************************************************************************************************************
    ALLOC TMP
 ***********************************************************************************************************************
 */

char	*MEM_gp_AllocTmpFirst = NULL;
char	*MEM_gp_AllocTmpLast = NULL;
char	*MEM_gp_AllocTmpNext = NULL;
BOOL	MEM_gb_EnableTmp = FALSE;
int		MEM_gi_AllocTmpNb = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_p_AllocTmp(ULONG ulSize)
{
#ifdef ACTIVE_EDITORS
	ulSize += 128;
	ulSize &= ~63;
	return MEM_p_Alloc(ulSize);

	/*~~~~~~~~*/
#else
	char	*p;
	ULONG	*p1;
	/*~~~~~~~~*/

	ulSize += 128;
	ulSize &= ~63;
	if(!MEM_gp_AllocTmpFirst || !MEM_gb_EnableTmp) return MEM_p_AllocAlign(ulSize, 64);
	if(MEM_gp_AllocTmpNext + ulSize >= MEM_gp_AllocTmpLast) return MEM_p_AllocAlign(ulSize, 64);

	MEM_M_BeginCriticalSection();
	p = MEM_gp_AllocTmpNext +
	4;
	p1 = (ULONG *) MEM_gp_AllocTmpNext;
	*p1 = ulSize;
	MEM_gp_AllocTmpNext += ulSize;
	MEM_gi_AllocTmpNb++;
	MEM_M_EndCriticalSection();

	return p;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_FreeTmp(void *p)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	*p1, size;
	/*~~~~~~~~~~~~~~*/

	p1 = (ULONG *) p;
	p1--;
	size = *p1;

	MEM_M_BeginCriticalSection();
	if((char *) p + size - 4 == MEM_gp_AllocTmpNext) MEM_gp_AllocTmpNext = (char *) p1;

	MEM_gi_AllocTmpNb--;
	MEM_M_EndCriticalSection();

	if(!MEM_gi_AllocTmpNb)
	{
		MEM_ResetTmpMemory();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_p_ReallocTmp(void *_pv_Block, ULONG _ul_BlockSize)
{
	/*~~~~~~~*/
	char	*p;
    ULONG size;
	/*~~~~~~~*/

    size = *(((ULONG*)_pv_Block)-1);
	
    p = (char*)MEM_p_AllocTmp(_ul_BlockSize);
	L_memmove(p, _pv_Block, size);
	MEM_Free(_pv_Block);

	return p;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_ResetTmpMemory(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern void SOFT_ZList_Init(void);
	extern void SOFT_ZList_Clear(void);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_M_BeginCriticalSection();
	MEM_gp_AllocTmpNext = MEM_gp_AllocTmpFirst;
	MEM_gi_AllocTmpNb = 0;
	MEM_M_EndCriticalSection();

#if !defined(MAX_PLUGIN)
	SOFT_ZList_Init();
	SOFT_ZList_Clear();
#endif // !defined(MAX_PLUGIN)
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Reallocates a bloc of memory (allocated with MEM_p_Alloc)
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef _DEBUG
void	*
_MEM_p_Realloc(void *_pv_Block, ULONG _ul_BlockSize, char *_psz_File, int _i_Line)
#else
void *
_MEM_p_Realloc(void *_pv_Block, ULONG _ul_BlockSize)
#endif
{
	ULONG	*pv_NewBlock;
	ULONG	*pv_OldBlock;
	ULONG	ul_CopySize;
	ULONG	ul_OldSize;
	ULONG	ul_NewSize;
	int		index;


	/* Pointeur temporaire ? */
	if(MEM_IsTmpPointer((char *) _pv_Block))
	{
		return MEM_p_ReallocTmp(_pv_Block, _ul_BlockSize);
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~*/
		void	*p;
		ULONG	ul_BlockMark;
		/*~~~~~~~~~~~~~~~~~*/

		p = (void *) *((ULONG *) _pv_Block - 1);
		if(MEM_Mb_IsAllocPointer(p) && (((unsigned int) p % 4) == 0))
		{
			ul_BlockMark = MEM_ul_GetMark(p);
			if(ul_BlockMark == (MEM_Cul_BlockMarkMEMpAllocAlign + MEM_Cul_BlockAllocated))
			{
				return MEM_p_ReallocAlign(_pv_Block, _ul_BlockSize, 64);
			}
		}
#if defined(MEM_USE_TYPES) && defined(_DEBUG) && !defined(_XENON)
		MEM_IncrementType(MEM_ul_GetType(_pv_Block), -(s32)MEM_ul_GetSize(_pv_Block));
		MEM_ul_ResetType(_pv_Block);
#endif
	}
	pv_OldBlock = (ULONG *) _pv_Block;

	/* If real size does change, no need to realloc */
	ul_OldSize = MEM_ul_GetRealSize(pv_OldBlock);
	MEM_GetIndexAndRealSize((ULONG*)&index, &ul_NewSize, _ul_BlockSize);
	if(ul_OldSize == ul_NewSize)
	{
#if defined(MEM_USE_TYPES) && defined(_DEBUG) && !defined(_XENON)
		MEM_SetType(_pv_Block, MEM_GetTypeFromName(_psz_File));
		MEM_IncrementType(MEM_GetTypeFromName(_psz_File), _ul_BlockSize);
#endif
		return _pv_Block;
	}

	/* Allocates new bloc */
#ifdef _DEBUG
	pv_NewBlock = (ULONG *) _MEM_p_Alloc(_ul_BlockSize, _psz_File, _i_Line);
#else
	pv_NewBlock = (ULONG *) _MEM_p_Alloc(_ul_BlockSize);
#endif
	ul_OldSize = MEM_ul_GetRealSize(_pv_Block);
	ul_NewSize = MEM_ul_GetRealSize(pv_NewBlock);
	ul_CopySize = (ULONG) lMin(ul_NewSize, ul_OldSize);

	/* Copies old data to new bloc */
	L_memcpy(pv_NewBlock, pv_OldBlock, ul_CopySize);

	/* Free old block */
	MEM_Free(_pv_Block);

	/* Return new block */
	return(pv_NewBlock);
}

/*$4
 ***********************************************************************************************************************
    ALLOC ALIGN
 ***********************************************************************************************************************
 */

/*$off*/
#ifdef _DEBUG
void *_MEM_p_AllocAlign(ULONG _ul_BlockSize, ULONG Alignment, char*_pz_File, int _i_Line)
#else
void *_MEM_p_AllocAlign(ULONG _ul_BlockSize, ULONG Alignment)
#endif
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	Allocated, RealPtr;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef _DEBUG
	RealPtr = (ULONG) _MEM_p_Alloc(_ul_BlockSize + 2 * Alignment, _pz_File, _i_Line);
#else
	RealPtr = (ULONG) _MEM_p_Alloc(_ul_BlockSize + 2 * Alignment);
#endif

	MEM_SetMark((void *) RealPtr, MEM_Cul_BlockMarkMEMpAllocAlign + MEM_Cul_BlockAllocated);

	Allocated = RealPtr &~(Alignment - 1);
	Allocated += Alignment;
	Allocated += Alignment;
	*((ULONG *) Allocated - 1) = RealPtr;
	return (void *) Allocated;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef _DEBUG
void *_MEM_p_ReallocAlign(void *_pv, ULONG _ul_BlockSize, ULONG Alignment, char*_pz_File,int _i_Line)
#else
void *_MEM_p_ReallocAlign(void *_pv, ULONG _ul_BlockSize, ULONG Alignment)
#endif
{
	/*~~~~~~~~~~~~~*/
	void	*pv_temp;
	/*~~~~~~~~~~~~~*/

#ifdef _DEBUG
	pv_temp = _MEM_p_AllocAlign(_ul_BlockSize, Alignment,_pz_File, _i_Line );
#else
	pv_temp = _MEM_p_AllocAlign(_ul_BlockSize, Alignment);
#endif

	ERR_X_Assert(pv_temp);
	L_memcpy(pv_temp, _pv, _ul_BlockSize);
	MEM_FreeAlign(_pv);
	return pv_temp;
}
/*$on*/



/*$4
 ***********************************************************************************************************************
    special functions for allocating blocks from the end of memory area
 ***********************************************************************************************************************
 */

#define MEM_Cte_FromEndHeaderSize	8
#define MEM_Cte_FromEndMarkAlloc	0xcafedeca
#define MEM_Cte_FromEndMarkFree		0x5A5A5AA5
ULONG	MEM_gul_FromEndMaxAllocSize = 0;

#ifdef PSX2_TARGET
/*$off*/
#ifdef _DEBUG
void *_MEM_p_AllocFromEndAlign(ULONG _ul_size, ULONG _i_modulo, char *_str_file, int _i_line)
#else
void *_MEM_p_AllocFromEndAlign(ULONG _ul_size, ULONG _i_modulo)
#endif
/*$on*/
{
	ULONG	Allocated, RealPtr;

#ifdef _DEBUG
	RealPtr = (ULONG) _MEM_p_AllocFromEnd(_ul_size + 2 * _i_modulo, _str_file, _i_line);
#else
	RealPtr = (ULONG) _MEM_p_AllocFromEnd(_ul_size + 2 * _i_modulo);
#endif
	Allocated = RealPtr &~(_i_modulo - 1);
	Allocated += _i_modulo;
	Allocated += _i_modulo;
	*((ULONG *) Allocated - 1) = RealPtr;
	return (void *) Allocated;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_FreeFromEndAlign(void *_pv_block)
{
	MEM_FreeFromEnd((void *) * ((ULONG *) _pv_block - 1));
}

#endif
#ifdef _DEBUG
/*$off*/
void *_MEM_p_AllocFromEnd(ULONG _ul_size, char *_str_file, int _i_line)
#else
void *_MEM_p_AllocFromEnd(ULONG _ul_size)
#endif
/*$on*/
{
	ULONG	free_size;
	void	*pv_return;

	MEM_M_BeginCriticalSection();

	if(MEM_gst_MemoryInfo.pv_LastFree == NULL)
	{
		MEM_gst_MemoryInfo.pv_LastFree = (void *) ((char *) MEM_gst_MemoryInfo.pv_DynamicBloc + MEM_gst_MemoryInfo.ul_DynamicMaxSize);
		MEM_gst_MemoryInfo.ul_RealSize = MEM_gst_MemoryInfo.ul_DynamicMaxSize;
		MEM_gul_FromEndMaxAllocSize = 0;
	}
	_ul_size = (_ul_size & 0xFFFFFFFC) + 4;
	free_size = (ULONG) MEM_gst_MemoryInfo.pv_LastFree - (ULONG) MEM_gst_MemoryInfo.pv_DynamicNextFree -	MEM_Cte_FromEndHeaderSize;


	if(free_size > _ul_size)
	{

		MEM_gst_MemoryInfo.pv_LastFree = (void *) ((char *) MEM_gst_MemoryInfo.pv_LastFree - _ul_size);
		pv_return = MEM_gst_MemoryInfo.pv_LastFree;
		MEM_gst_MemoryInfo.pv_LastFree = (void *) ((char *) MEM_gst_MemoryInfo.pv_LastFree - MEM_Cte_FromEndHeaderSize);
		MEM_SetMark(pv_return, MEM_Cte_FromEndMarkAlloc);
		MEM_SetSize(pv_return, _ul_size);
		MEM_gst_MemoryInfo.ul_DynamicMaxSize = (ULONG) MEM_gst_MemoryInfo.pv_LastFree -
		(ULONG) MEM_gst_MemoryInfo.pv_DynamicBloc;
		MEM_gul_FromEndMaxAllocSize = MEM_gul_FromEndMaxAllocSize >
			(MEM_gst_MemoryInfo.ul_RealSize - MEM_gst_MemoryInfo.ul_DynamicMaxSize) ? MEM_gul_FromEndMaxAllocSize :
				(MEM_gst_MemoryInfo.ul_RealSize - MEM_gst_MemoryInfo.ul_DynamicMaxSize);

#ifdef ACTIVE_EDITORS
        L_memset(pv_return, 0, _ul_size);
#endif
    }
	else
	{
#ifdef _DEBUG
		printf("can't alloc from end %d for %s(%d)\n", _ul_size, _str_file, _i_line);
		pv_return = _MEM_p_Alloc(_ul_size, _str_file, _i_line);
#else
		pv_return = MEM_p_Alloc(_ul_size);
#endif
	}
	
	
	MEM_M_EndCriticalSection();
	return pv_return;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_FreeFromEnd(void *_pv_block)
{
	/*~~~~~~~~~~*/
	ULONG	size;
	char	**ppv;
	/*~~~~~~~~~~*/

	if(*((ULONG *) _pv_block - 2) == MEM_Cte_FromEndMarkAlloc)
	{
		MEM_M_BeginCriticalSection();
		ppv = (char **) &MEM_gst_MemoryInfo.pv_LastFree;

		if(((ULONG) _pv_block - MEM_Cte_FromEndHeaderSize) == (ULONG) MEM_gst_MemoryInfo.pv_LastFree)
		{
			*((ULONG *) _pv_block - 2) = 0;
			size = *((ULONG *) _pv_block - 1);
			MEM_gst_MemoryInfo.pv_LastFree = (void *) ((char *) MEM_gst_MemoryInfo.pv_LastFree + size + MEM_Cte_FromEndHeaderSize);
			_pv_block = (void *) ((char *) _pv_block + size + MEM_Cte_FromEndHeaderSize);
			MEM_gst_MemoryInfo.ul_DynamicMaxSize = (ULONG) MEM_gst_MemoryInfo.pv_LastFree -
			(ULONG) MEM_gst_MemoryInfo.pv_DynamicBloc;

			while(MEM_gst_MemoryInfo.ul_DynamicMaxSize < MEM_gst_MemoryInfo.ul_RealSize)
			{
				if
				(
					((ULONG) _pv_block - MEM_Cte_FromEndHeaderSize == (ULONG) MEM_gst_MemoryInfo.pv_LastFree)
				&&	(*((ULONG *) _pv_block - 2) == MEM_Cte_FromEndMarkFree)
				)
				{
					*((ULONG *) _pv_block - 2) = 0;
					size = *((ULONG *) _pv_block - 1);
					MEM_gst_MemoryInfo.pv_LastFree = (void *) ((char *) MEM_gst_MemoryInfo.pv_LastFree + size + MEM_Cte_FromEndHeaderSize);
					_pv_block = (void *) ((char *) _pv_block + size + MEM_Cte_FromEndHeaderSize);
					MEM_gst_MemoryInfo.ul_DynamicMaxSize = (ULONG) MEM_gst_MemoryInfo.pv_LastFree -
					(ULONG) MEM_gst_MemoryInfo.pv_DynamicBloc;
				}
				else
					break;
			}

			ERR_X_Assert(MEM_gst_MemoryInfo.ul_DynamicMaxSize <= MEM_gst_MemoryInfo.ul_RealSize);

			if(MEM_gst_MemoryInfo.ul_DynamicMaxSize == MEM_gst_MemoryInfo.ul_RealSize) MEM_gst_MemoryInfo.pv_LastFree = NULL;
		}
		else
		{
			*((ULONG *) _pv_block - 2) = MEM_Cte_FromEndMarkFree;
		}

		MEM_M_EndCriticalSection();
	}
	else
	{
		MEM_Free(_pv_block);
	}
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    debugging functions
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef JADEFUSION
LONG	*MEM_gp_LastAllocated = ((LONG *) - 1);
#else
LONG	*MEM_gp_LastAllocated = ((void *) - 1);
#endif
LONG	MEM_gp_PrintCluster = 0;
void	MEM_dbg_PrintClustersChain(void);
void	MEM_dbg_PrintMemBlock(void);
void	MEM_dbg_FindLastAllocated(void);

#ifdef _GAMECUBE
#define _print	OSReport
#elif defined(PSX2_TARGET)
#define _print	printf
#else
FILE	*gfh = NULL;

#define _print(a) \
	do \
	{ \
		if(!gfh) \
		{ \
			gfh = fopen("memdyn.log", "w+"); \
		} \
		if(gfh) \
		{ \
			fprintf(gfh, a); \
		} \
	} while(0);
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_dbg_FindLastAllocatedCluster(void)
{
	MEM_dbg_FindLastAllocated();
	MEM_dbg_PrintClustersChain();
	MEM_dbg_PrintMemBlock();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_dbg_FindLastAllocated(void)
{
	/*~~~~~~~~~~~~~~~~*/
	LONG	*p;
	char	memlog[128];
	/*~~~~~~~~~~~~~~~~*/

	if(MEM_gp_LastAllocated) return;

	MEM_gp_LastAllocated = (LONG *) MEM_gst_MemoryInfo.pv_DynamicNextFree;

	while((ULONG) MEM_gp_LastAllocated > (ULONG) MEM_gst_MemoryInfo.pv_DynamicBloc)
	{
		p = MEM_gp_LastAllocated -
		1;
		while
		(
			(*p != (LONG) (MEM_Cul_BlockMarkMEMpAlloc + MEM_Cul_BlockAllocated))
		&&	(*p != (LONG) (MEM_Cul_BlockMarkMEMpAllocAlign + MEM_Cul_BlockAllocated))
		)
		{
			p = p - 1;
		}

		MEM_gp_LastAllocated = p;
		snprintf( memlog, sizeof(memlog), "%08x - %d\n", ( uintptr_t ) p, *( p + 1 ) );
		_print(memlog);
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_dbg_PrintClustersChain(void)
{
	/*~~~~~~~~~~~~~~~~*/
	ULONG	*p;
	ULONG	*pfree;
	ULONG	*p1stfree;
	char	memlog[128];
	/*~~~~~~~~~~~~~~~~*/

	pfree = NULL;
	p1stfree = NULL;

	if(!MEM_gp_PrintCluster) return;
#ifdef JADEFUSION
	p = (ULONG*)MEM_gst_MemoryInfo.pv_DynamicBloc;
#else
	p = MEM_gst_MemoryInfo.pv_DynamicBloc;
#endif
	while(1)
	{
		switch(*p)
		{
		case ((ULONG) (MEM_Cul_BlockMarkMEMpAlloc + MEM_Cul_BlockAllocated)):
		case ((ULONG) (MEM_Cul_BlockMarkMEMpAllocAlign + MEM_Cul_BlockAllocated)):
			if(pfree)
			{
				sprintf(memlog, "Free, %x, %d\n", (ULONG) pfree, (ULONG) p - (ULONG) pfree);
				_print(memlog);
				pfree = NULL;
			}

			if(p1stfree)
			{
				sprintf(memlog, "Free block, %x, %d\n", (ULONG) p1stfree, (ULONG) p - (ULONG) p1stfree);
				_print(memlog);
				p1stfree = NULL;
			}

			sprintf(memlog, "Allo, %x, %d\n", (ULONG) p, *(p + 1) + 8);
			_print(memlog);
			p++;
			break;

		case ((ULONG) (MEM_Cul_BlockFree + MEM_Cul_BlockMarkMEMpAlloc)):
			if(pfree)
			{
				sprintf(memlog, "Free, %x, %d\n", (ULONG) pfree, (ULONG) p - (ULONG) pfree);
				_print(memlog);
				pfree = p;
			}
			else
			{
				pfree = p;
			}

			if(!p1stfree) p1stfree = p;
			p++;
			break;

		default:
			p++;
			break;
		}

		if((ULONG) p >= (ULONG) MEM_gst_MemoryInfo.pv_DynamicNextFree) break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_dbg_PrintMemBlock(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		index;
	ULONG	size;
	ULONG	**ppc_hole;
	ULONG	*pfree;
#ifdef MEM_USE_HISTORY
	int		i, j;
#endif
	char	memlog[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!MEM_gp_PrintCluster) return;

	for(index = 0; index < MEM_C_ZlistSize; index++)
	{
		ppc_hole = (ULONG **) &MEM_gst_MemoryInfo.MEM_ast_Zlist[index].pv_Hole;
		size = MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size +
		MEM_C_BlockHeaderSize;

		while(*ppc_hole)
		{
			pfree = *ppc_hole + 1;

#ifdef MEM_USE_HISTORY
			if((int) (pfree - 2) < (int) MEM_gst_MemoryInfo.pv_DynamicNextFree)
			{
				i = (int) (pfree) % MEM_Cul_AllocationHistorySize;
				j = -2;
				while((ULONG) MEM_gap_FreeHistoryTable[i] != (ULONG) pfree)
				{
					i = (i + 1) % MEM_Cul_AllocationHistorySize;
					j++;
					if(j > MEM_Cul_AllocationHistorySize) break;
					ERR_X_Assert(i < MEM_Cul_AllocationHistorySize);
				}

				if(j > MEM_Cul_AllocationHistorySize)
				{
					sprintf(memlog, "Free, %08d, %08x ?(?)\n", size, (ULONG) pfree);
				}
				else
				{
					sprintf
					(
						memlog,
						"Free, %08d, %08x %s(%d)\n",
						size,
						(ULONG) pfree,
						MEM_gapsz_FreeInFile[i],
						MEM_gai_FreeInLine[i]
					);
				}

				_print(memlog);
			}

#else
			sprintf(memlog, "Free, %08d, %08x\n", size, (ULONG) pfree);
			_print(memlog);
#endif
			ppc_hole = (ULONG **) *
			ppc_hole;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char MEM_b_CheckBlockHasBeenFreed(void *_pv_Block)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG	ul_size;
	ULONG	ul_BlockMark;
	/*~~~~~~~~~~~~~~~~~*/

	ul_BlockMark = MEM_ul_GetMark(_pv_Block);

	if((ul_BlockMark & 0xF0000000) != MEM_Cul_BlockAllocated) return 1;
	if(((ul_BlockMark & 0xFFFFFF) != MEM_Cul_BlockMarkMEMpAlloc) && ((ul_BlockMark & 0xFFFFFF) != MEM_Cul_BlockMarkMEMpAllocAlign)) return 1;

	/* Check that the size is coherent */
	ul_size = MEM_ul_GetSize(_pv_Block);
	if(!((ul_size > 0) && (ul_size < MEM_Cul_MaxSize))) return 1;

	return 0;
}


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    HOLE OPTIMIZE Optim = split big holes into severals smaller, and on the other and concat consecutive free clusters
    into one bigger
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef USE_HOLE_OPTIM

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void MEM_RegisterHoleInChain(ULONG *pul_Size, ULONG index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Key;
	MEM_tdst_HoleChain	*pHole;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size < 16) return;

	if(MEM_gul_HoleChainNb < MEM_Cte_HoleChainSize)
	{
		i_Key = (ULONG) pul_Size %
		MEM_Cte_HoleChainSize;
		pHole = MEM_gax_HoleChain + i_Key;
		while(pHole->pul_Hole)
		{
			i_Key = (i_Key + 1) %
			MEM_Cte_HoleChainSize;
			pHole = MEM_gax_HoleChain +
			i_Key;
		}

		MEM_gul_HoleChainNb++;

		pHole->pst_NextHole = MEM_gp_HoleChainLastElem;
		pHole->pul_Hole = pul_Size;
		pHole->ul_Index = index;
		pHole->pst_PrevHole = NULL;

		if(MEM_gp_HoleChainLastElem) MEM_gp_HoleChainLastElem->pst_PrevHole = pHole;
		MEM_gp_HoleChainLastElem = pHole;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void *MEM_p_GetHoleInChain(ULONG _ul_Size)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MEM_tdst_HoleChain	*pHole;
	MEM_tdst_HoleChain	*pHoleBetter;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!MEM_gp_HoleChainLastElem) return NULL;

	pHoleBetter = NULL;
	pHole = MEM_gp_HoleChainLastElem;

	_ul_Size = MEM_ul_GetIndex(_ul_Size);

	while(pHole)
	{
		if(pHole->ul_Index >= _ul_Size)
		{
			if(pHoleBetter)
			{
				if((ULONG) pHole->pul_Hole < (ULONG) pHoleBetter->pul_Hole) pHoleBetter = pHole;
			}
			else
				pHoleBetter = pHole;
		}

		pHole = pHole->pst_NextHole;
	}

	return pHoleBetter;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void *MEM_p_SplitHoleInChain(void *pv_Return, ULONG _ul_BlockSize)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MEM_tdst_HoleChain	*pHole;
	MEM_tdst_ZlistEntry *pst_ZelemOld, *pst_ZelemNew;
	ULONG				**ppul_Hole;
	ULONG				*pul_Size;
	ULONG				ul_EffSize;
	ULONG				ulNewSize, ulNewIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pHole = (MEM_tdst_HoleChain *) pv_Return;
	MEM_CheckBlockIsFree((void *) (pHole->pul_Hole + 1));

	pst_ZelemOld = &MEM_gst_MemoryInfo.MEM_ast_Zlist[pHole->ul_Index];

	/*$1- check we have to split the cluster else there is pb... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_GetIndexAndRealSize(&ulNewIndex, &ulNewSize, _ul_BlockSize);
	ERR_X_Assert(pHole->ul_Index != ulNewIndex);

	/*$1- extract ptr from Zlist ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ppul_Hole = (ULONG **) &pst_ZelemOld->pv_Hole;
	while(*ppul_Hole && (*ppul_Hole != pHole->pul_Hole))
	{
		ppul_Hole = (ULONG **) *
		ppul_Hole;
	}

	ERR_X_Assert(*ppul_Hole == pHole->pul_Hole);

	/*$1- now try to resize the cluster ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ZelemNew = &MEM_gst_MemoryInfo.MEM_ast_Zlist[ulNewIndex];
	if((pst_ZelemOld->ul_Size - pst_ZelemNew->ul_Size) > (MEM_C_BlockHeaderSize + 16))
	{
		/*~~~~~~~~~~*/
		int recursive;
		/*~~~~~~~~~~*/

		recursive = 20;

		ulNewSize = pst_ZelemOld->ul_Size -
		pst_ZelemNew->ul_Size;
		ulNewIndex = MEM_ul_GetIndex(ulNewSize - MEM_C_BlockHeaderSize);

		while
		(
			ulNewIndex
		&&	recursive
		&&	((MEM_gst_MemoryInfo.MEM_ast_Zlist[ulNewIndex].ul_Size + MEM_C_BlockHeaderSize) > ulNewSize)
		)
		{
			ulNewIndex--;
			recursive--;
		}

		if(!ulNewIndex || !recursive || (MEM_gst_MemoryInfo.MEM_ast_Zlist[ulNewIndex].ul_Size < 16))
		{
			goto lb_cannotsplit;
		}

		/*$1- destroy link in old Zchain ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pul_Size = pHole->pul_Hole;
		*ppul_Hole = (ULONG *) (*pul_Size);
		*(pul_Size + ((pst_ZelemOld->ul_Size + MEM_C_BlockHeaderSize) >> 2) - 2) = 0;

		/*$1- put the resized cluster in the new Zchain ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ppul_Hole = (ULONG **) &(pst_ZelemNew->pv_Hole);
		*pul_Size = (ULONG) (*ppul_Hole);
		*ppul_Hole = pul_Size;
		ul_EffSize = (pst_ZelemNew->ul_Size + MEM_C_BlockHeaderSize);
		*(pul_Size + (ul_EffSize >> 2) - 2) = ul_EffSize;

		/*$1- create the 2nd new cluster ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ppul_Hole = (ULONG **) &(MEM_gst_MemoryInfo.MEM_ast_Zlist[ulNewIndex].pv_Hole);
		pul_Size = (ULONG *) ((char *) pul_Size + pst_ZelemNew->ul_Size + MEM_C_BlockHeaderSize);
		*pul_Size = (ULONG) (*ppul_Hole);
		*ppul_Hole = pul_Size;
		ul_EffSize = (MEM_gst_MemoryInfo.MEM_ast_Zlist[ulNewIndex].ul_Size + MEM_C_BlockHeaderSize);
		*(pul_Size + (ul_EffSize >> 2) - 2) = ul_EffSize;
		pul_Size[-1] = MEM_Cul_BlockMarkMEMpAlloc | MEM_Cul_BlockFree;

		MEM_RegisterHoleInChain(pul_Size, ulNewIndex);
		MEM_DeleteHoleInChain(pHole->pul_Hole);

		return (void *) &pst_ZelemNew->pv_Hole;
	}
	else
	{
lb_cannotsplit:
		MEM_gul_LostHoleNb++;
		MEM_gul_LostHoleSize += pst_ZelemOld->ul_Size -
		pst_ZelemNew->ul_Size;

		return NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void MEM_DeleteHoleInChain(ULONG *pul_Hole)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MEM_tdst_HoleChain	*pHole;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!MEM_gp_HoleChainLastElem) return;
	pHole = MEM_gp_HoleChainLastElem;

	while(pHole)
	{
		if(pHole->pul_Hole == pul_Hole)
		{
			if(pHole->pst_PrevHole) pHole->pst_PrevHole->pst_NextHole = pHole->pst_NextHole;
			if(pHole->pst_NextHole) pHole->pst_NextHole->pst_PrevHole = pHole->pst_PrevHole;

			if(MEM_gp_HoleChainLastElem == pHole)
			{
				if(pHole->pst_PrevHole)
					MEM_gp_HoleChainLastElem = pHole->pst_PrevHole;
				else
					MEM_gp_HoleChainLastElem = pHole->pst_NextHole;
			}

			pHole->pst_NextHole = NULL;
			pHole->pst_PrevHole = NULL;
			pHole->pul_Hole = NULL;
			pHole->ul_Index = 0;

			MEM_gul_HoleChainNb--;
			return;
		}

		pHole = pHole->pst_NextHole;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL MEM_b_ConcatHolesInChain(ULONG *pul_Hole, ULONG index)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG	size;
	ULONG	*pul_Mark;
	ULONG	**ppv;
	ULONG	OldSize;
	BOOL	b_ConcatDone;
	ULONG	index2;
	/*~~~~~~~~~~~~~~~~~*/

	return FALSE;
		
	b_ConcatDone = FALSE;
	while(1)
	{
		OldSize = MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size;

		size = pul_Hole[-2];
		if(size & 0x80000000) break;
		if(size & 3) break;
		if(size < MEM_C_BlockHeaderSize) break;

		pul_Mark = &pul_Hole[-1] - (size >> 2);
		
		if(pul_Mark <= (ULONG *) MEM_gst_MemoryInfo.pv_DynamicBloc) break;
		if(pul_Mark > (ULONG *) MEM_gst_MemoryInfo.pv_DynamicNextFree) break;

		if(*pul_Mark == (MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc))
		{
			index2 = MEM_ul_GetIndex(size - MEM_C_BlockHeaderSize);
			ppv = (ULONG **) &MEM_gst_MemoryInfo.MEM_ast_Zlist[index2].pv_Hole;
			while(*ppv && (*ppv != (pul_Mark + 1)))
			{
				ppv = (ULONG **) (*ppv);
			}

			if(*ppv != (pul_Mark + 1)) break;

			b_ConcatDone = TRUE;

			/* delete 1st cluster from Zchain */
			ppv = (ULONG **) &MEM_gst_MemoryInfo.MEM_ast_Zlist[index].pv_Hole;
			while(*ppv && (*ppv != pul_Hole))
			{
				ppv = (ULONG **) (*ppv);
			}

			if(*ppv == pul_Hole)
			{
				*ppv = (ULONG *) *pul_Hole;														/* extract from chain */
			}

			MEM_DeleteHoleInChain(pul_Hole);
			pul_Hole[-1] = 0;																	/* delete free mark */
			*(pul_Hole + ((MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size + MEM_C_BlockHeaderSize) >> 2) - 2) = 0; /* delete size at the end of block */

			/* delete 2nd cluster from Zchain */
			pul_Hole[-2] = 0;																	/* del size at the end */
			pul_Hole = pul_Mark +
			1;
			index = MEM_ul_GetIndex(size - 8);

			ppv = (ULONG **) &MEM_gst_MemoryInfo.MEM_ast_Zlist[index].pv_Hole;
			while(*ppv && (*ppv != pul_Hole))
			{
				ppv = (ULONG **) (*ppv);
			}

			ERR_X_Assert(*ppv == pul_Hole);

			*ppv = (ULONG *) * pul_Hole;														/* extract from chain */
			MEM_DeleteHoleInChain(pul_Hole);

			/* add new cluster */
			index = MEM_ul_GetIndex((OldSize + MEM_C_BlockHeaderSize) + (size - MEM_C_BlockHeaderSize));
			while(MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size > ((OldSize + 8) + (size - 8)))
			{
				index--;
			}

			ERR_X_Assert(index);

			ppv = (ULONG **) &MEM_gst_MemoryInfo.MEM_ast_Zlist[index].pv_Hole;
			*pul_Hole = (ULONG) * ppv;
			*ppv = (ULONG *) pul_Hole;

			*(pul_Hole + ((MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size + 8) >> 2) - 2) = MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size + 8;
			MEM_RegisterHoleInChain(pul_Hole, index);
		}
		else
			break;
	}

	return b_ConcatDone;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_UpdateHoles()
{
	MEM_tdst_ZlistEntry *pst_Zelem;
	ULONG				*pul_Hole;
	ULONG				*pul_Cpt;
	int					z;
		
	for(z = 0; z < MEM_C_ZlistSize; z++)
	{		
		pst_Zelem = &MEM_gst_MemoryInfo.MEM_ast_Zlist[z];			
		pul_Hole = (ULONG *) (pst_Zelem->pv_Hole);
		
		if(pst_Zelem->ul_Size < 12)
			continue;
											
		while(pul_Hole)
		{		
			pul_Cpt = pul_Hole + 2;
			(*pul_Cpt) ++;
			
			pul_Hole = (ULONG *) *(pul_Hole);				
		}
	}		
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL MEM_b_FindOldHole(ULONG _ul_BlockSize)
{
	MEM_tdst_ZlistEntry *pst_Zelem;
	ULONG				*pul_PreviousHole;
	ULONG				*pul_Hole;
	ULONG				*pul_Cpt;
	int					z, test;
	ULONG				index;			
		
	index = MEM_ul_GetIndex(_ul_BlockSize);					
	for(z = index; z < MEM_C_ZlistSize; z++)
	{		
		pul_PreviousHole = NULL;		
		pst_Zelem = &MEM_gst_MemoryInfo.MEM_ast_Zlist[z];			
		pul_Hole = (ULONG *) (pst_Zelem->pv_Hole);
		if(pst_Zelem->ul_Size < 12)
			continue;
											
		while(pul_Hole)
		{		
			pul_Cpt = pul_Hole + 2;
			test = (*pul_Cpt);
			if(test >= 5)
			{
				MEM_ul_NumOldHoles ++;
				MEM_ul_SizeOldHoles += *(pul_Hole + 1);
			
				if(pul_PreviousHole)
					*pul_PreviousHole = *pul_Hole;
				else
					pst_Zelem->pv_Hole = (ULONG*) *pul_Hole;
									
				if(!MEM_gp_OldHolesList)
				{					
					MEM_gp_OldHolesList = pul_Hole;
					*(ULONG *)MEM_gp_OldHolesList = 0;
					return TRUE;
				}
				else
				{
					*pul_Hole = (ULONG) MEM_gp_OldHolesList;
					MEM_gp_OldHolesList = pul_Hole;
					return TRUE;
				}
			}
			
			pul_PreviousHole = pul_Hole;
			pul_Hole = (ULONG *) *(pul_Hole);				
		}
	}		

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG * MEM_pv_Hole=NULL;
static void *MEM_p_AllocInOldHoles(ULONG _ul_Size)
 {
	ULONG				*pul_PreviousHole; 
 	ULONG				**ppul_Hole;
 	ULONG				**ppul_BestHole;
 	ULONG				*pul_PreviousBestHole; 	 	
 	ULONG				*pul_CHole;
 	ULONG				*pul_NewHole;
 	ULONG				*pul_SizeHole;
 	ULONG				index, ul_EffSize;
 	
 	
 	
	pul_PreviousHole = NULL;		
	pul_PreviousBestHole = NULL;
	ppul_BestHole = NULL;
			
	
	index = MEM_ul_GetIndex(_ul_Size);			
	ul_EffSize = (MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size + MEM_C_BlockHeaderSize);	


	ppul_Hole = (ULONG**)&MEM_gp_OldHolesList;
	while(*ppul_Hole)
	{	
		pul_SizeHole = *ppul_Hole + 1;
					
		if(*pul_SizeHole == ul_EffSize)
		{
			MEM_pv_Hole = *ppul_Hole;
			
			if(pul_PreviousHole)
				*pul_PreviousHole = (ULONG) *(*ppul_Hole);
			else
				MEM_gp_OldHolesList = (ULONG *) *(*ppul_Hole);

			MEM_ul_SizeOldHoles -= ul_EffSize;
			MEM_ul_NumOldHoles --;			
			
			return (void*)&MEM_pv_Hole;
		}
		
		if(*pul_SizeHole >= ul_EffSize + 16)	
		{
			pul_PreviousBestHole = pul_PreviousHole;		
			ppul_BestHole = ppul_Hole;		
		}
		
		pul_PreviousHole = *ppul_Hole;
		ppul_Hole = (ULONG **) (*ppul_Hole);		
	}

	
	if(ppul_BestHole) // && *ppul_BestHole)
	{
		ppul_Hole = ppul_BestHole;
		pul_PreviousHole = pul_PreviousBestHole;	
		MEM_pv_Hole = *ppul_Hole;	
		
		pul_SizeHole = (*ppul_Hole) + 1;
		
		pul_NewHole = (*ppul_Hole) + (ul_EffSize >> 2) ;
		*(pul_NewHole - 1) = MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc;
		*(pul_NewHole) = *((*ppul_Hole));
		*(pul_NewHole + 1) = (*pul_SizeHole) - ul_EffSize;
		*(pul_NewHole + 2) = 0;
		*(pul_NewHole + ((*pul_SizeHole - ul_EffSize) >> 2) - 2) = *pul_SizeHole - ul_EffSize;				
		
		/* If the new resized hole is exactly a ZList size Value, we reinsert it in the "classical" hole management */
		index = MEM_ul_GetIndex(*pul_SizeHole - ul_EffSize - MEM_C_BlockHeaderSize);				
		if(MEM_gst_MemoryInfo.MEM_ast_Zlist[index].ul_Size == (*pul_SizeHole - ul_EffSize - MEM_C_BlockHeaderSize))
		{
			pul_CHole = (ULONG *) MEM_gst_MemoryInfo.MEM_ast_Zlist[index].pv_Hole;
			
			*((ULONG *) &MEM_gst_MemoryInfo.MEM_ast_Zlist[index].pv_Hole) = (ULONG) pul_NewHole;
			*(pul_NewHole) = (ULONG) pul_CHole;
							
			/* Remove this Hole from the OldList */
			if(pul_PreviousHole)
				*pul_PreviousHole = (ULONG) *((*ppul_Hole));
			else
				MEM_gp_OldHolesList = (ULONG *) *((*ppul_Hole));
				
			MEM_ul_SizeOldHoles -= *pul_SizeHole;
			MEM_ul_NumOldHoles --;
		}				
		else
		{					
			/* Remove this Hole from the OldList */
			if(pul_PreviousHole)
				*pul_PreviousHole = (ULONG) pul_NewHole;
			else
				MEM_gp_OldHolesList = pul_NewHole;

			MEM_ul_SizeOldHoles -= ul_EffSize;
		}
													
		return (void *)&MEM_pv_Hole;
		
	}
	
	return NULL;
 }

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL MEM_b_ConcatHole(ULONG *pul_Hole, BOOL _before)
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG	**ppv1, **ppv2;
	ULONG	*pul_Mark;	
	ULONG	*pul_Hole1, *pul_Hole2;
	ULONG	size1, size2;	
	ULONG	index1;	
	ULONG	index2;
	BOOL	res;
	BOOL	b_OldHole1;
	BOOL	b_OldHole2;	
	/*~~~~~~~~~~~~~~~~~*/
	
	res = FALSE;
	while(1)
	{
		/* Get Size of this Hole */
		size1 = *(pul_Hole + 1);	
		if(size1 & 0x80000000) return res;
		if(size1 & 3) return res;
		if(size1 < MEM_C_BlockHeaderSize) return res;
		
		
		if(_before)
		{
			size2 = *(pul_Hole - 2);
			pul_Mark = (pul_Hole -1 - (size2 >> 2));			
		}
		else
		{
			size2 = *(pul_Hole + (size1 >> 2) + 1);
			pul_Mark = (pul_Hole + (size1 >> 2) - 1);
		}			
						
		if(size2 & 0x80000000) return res;
		if(size2 & 3) return res;
		if(size2 < MEM_C_BlockHeaderSize) return res;

		
		pul_Hole1 = pul_Hole;
		pul_Hole2 = pul_Mark + 1;
					
		
		if(pul_Mark <= (ULONG *) MEM_gst_MemoryInfo.pv_DynamicBloc) return res;
		if(pul_Mark > (ULONG *) MEM_gst_MemoryInfo.pv_DynamicNextFree) return res;
		
		if(*(pul_Hole2 + 1) != size2) return res;
				
		if(*pul_Mark == (MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc))
		{		
			index1 = MEM_ul_GetIndex(size1 - MEM_C_BlockHeaderSize);
			ppv1 = (ULONG **) &MEM_gst_MemoryInfo.MEM_ast_Zlist[index1].pv_Hole;
			while(*ppv1 && (*ppv1 != pul_Hole1))
				ppv1 = (ULONG **) (*ppv1);

			/* The hole is not in the ZChain of its size. It maybe in the OldHoleList */
			if(*ppv1 != pul_Hole1)
			{
				if(!MEM_gp_OldHolesList)
					return res;
					
				ppv1 = (ULONG **) &MEM_gp_OldHolesList;					
				while(*ppv1 && (*ppv1 != pul_Hole1))
					ppv1 = (ULONG **) (*ppv1);
					
				if(*ppv1 != pul_Hole1)
					return res;
					
				b_OldHole1 = TRUE;											
			}
			else
				b_OldHole1 = FALSE;			
			
			
			index2 = MEM_ul_GetIndex(size2 - MEM_C_BlockHeaderSize);
			ppv2 = (ULONG **) &MEM_gst_MemoryInfo.MEM_ast_Zlist[index2].pv_Hole;
			while(*ppv2 && (*ppv2 != pul_Hole2))
				ppv2 = (ULONG **) (*ppv2);

			/* The hole b4 is not in the ZChain of its size. It maybe in the OldHoleList */
			if(*ppv2 != pul_Hole2)
			{
				if(!MEM_gp_OldHolesList)
					return res;
					
				ppv2 = (ULONG **) &MEM_gp_OldHolesList;					
				while(*ppv2 && (*ppv2 != pul_Hole2))
					ppv2 = (ULONG **) (*ppv2);
					
				if(*ppv2 != pul_Hole2)
					return res;
					
				b_OldHole2 = TRUE;					
				
			}
			else
				b_OldHole2 = FALSE;			
						
			if(b_OldHole1)
			{
				MEM_ul_SizeOldHoles -= size1; 
				MEM_ul_NumOldHoles --;							
			}

			if(b_OldHole2)
			{
				MEM_ul_SizeOldHoles -= size2; 
				MEM_ul_NumOldHoles --;							
			}
			
			
			/* Delete clusters from Zchain or OldList*/			
			if(_before)
			{
				if((*pul_Hole2) == (ULONG) pul_Hole1)
				{
					*ppv2 = (ULONG *) *pul_Hole1;
				}
				else
				{
					if((*pul_Hole1) == (ULONG) pul_Hole2)
					{
						*ppv1 = (ULONG *) *pul_Hole2;
					}
					else
					{
						*ppv1 = (ULONG *) *pul_Hole1;
						*ppv2 = (ULONG *) *pul_Hole2;									
					}
				}

				*(pul_Hole1 - 1) = 0;
				*(pul_Hole1 + (size1 >> 2) - 2) = 0;
				*(pul_Hole2 + (size2 >> 2) - 2) = 0;						
				
				pul_Hole = pul_Hole2;
			}
			else
			{
				if((*pul_Hole1) == (ULONG) pul_Hole2)
				{
					*ppv1 = (ULONG *) *pul_Hole2;
				}
				else
				{
					if((*pul_Hole2) == (ULONG) pul_Hole1)
					{
						*ppv2 = (ULONG *) *pul_Hole1;
					}
					else
					{
						*ppv1 = (ULONG *) *pul_Hole1;
						*ppv2 = (ULONG *) *pul_Hole2;								
					}				
				}				

				*(pul_Hole2 - 1) = 0;					
				*(pul_Hole1 + (size1 >> 2) - 2) = 0;
				*(pul_Hole2 + (size2 >> 2) - 2) = 0;						
				
				pul_Hole = pul_Hole1;				
			}

			/* Add new cluster */
			index1 = MEM_ul_GetIndex(size1 + size2 - MEM_C_BlockHeaderSize);
			while(MEM_gst_MemoryInfo.MEM_ast_Zlist[index1].ul_Size > (size1 + size2 - 8))
			{
				index1--;
			}
			
			/* If the concatened block has exactly the size of an existing ZChain index, insert it into this ZChain Hole List*/
			/* If it is not the case, insert it into the OldHoleList Chain */
			if(MEM_gst_MemoryInfo.MEM_ast_Zlist[index1].ul_Size == (size1 + size2 - 8))
			{
				ppv1 = (ULONG **) &MEM_gst_MemoryInfo.MEM_ast_Zlist[index1].pv_Hole;			
				*pul_Hole = (ULONG) * ppv1;
				*ppv1 = (ULONG *) pul_Hole;				
				*(pul_Hole + ((MEM_gst_MemoryInfo.MEM_ast_Zlist[index1].ul_Size + 8) >> 2) - 2) = MEM_gst_MemoryInfo.MEM_ast_Zlist[index1].ul_Size + 8;
				*(pul_Hole + 1) = MEM_gst_MemoryInfo.MEM_ast_Zlist[index1].ul_Size + 8;
				if(size1 + size2 >= 20)
					*(pul_Hole + 2) = 0;				
			}
			else
			{
				ppv1 = (ULONG **) &MEM_gp_OldHolesList;			
				
				*pul_Hole = (ULONG) * ppv1;
				*ppv1 = (ULONG *) pul_Hole;				
				*(pul_Hole + ((size1 + size2) >> 2) - 2) = size1 + size2;	
				*(pul_Hole + 1) = size1 + size2;											
				
				MEM_ul_SizeOldHoles += size1 + size2; 
				MEM_ul_NumOldHoles ++;
				
			}

/*			
#ifdef _DEBUG
				if(*(pul_Hole - 1) != (MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc))
					OSReport("Concat Bug\n");										
#endif						
*/
			
			
			
						
						
			res = TRUE;
		}
		else
			return res;
	}
		
	return res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_ConcatHolesAfterDefrag()
{
	ULONG	**ppv;
#ifdef _DEBUG
	char asz_Log[512];
	ULONG	ul_NumHoles, ul_SizeOfHoles;
#endif	
	
/*			
#ifdef _DEBUG
{
	int	 Mem;
		
	MEM_ComputeMemoryHoleInfo(&ul_NumHoles, &ul_SizeOfHoles, FALSE);	
		
	Mem = (ULONG) ((char *) MEM_gst_MemoryInfo.pv_DynamicNextFree  - ((char *) MEM_gst_MemoryInfo.pv_DynamicBloc));

	sprintf(asz_Log, "-- Defrag=%0.3fMo\n", (((float)Mem / 1024.f) / 1024.f));
	OSReport(asz_Log);
	
	sprintf(asz_Log, "%u Old Holes Before Concat (Size=%d) (Num Holes=%u, Size Holes=%d B)\n", MEM_ul_NumOldHoles, MEM_ul_SizeOldHoles, ul_NumHoles, ul_SizeOfHoles);
	OSReport(asz_Log);
}
#endif
*/

	if(!MEM_gp_OldHolesList)
		return;
		
				
	//MEM_M_BeginCriticalSection();					
	ppv = (ULONG **) &MEM_gp_OldHolesList;					
	while(*ppv)
	{
/*	
#ifdef _DEBUG	
		if(*(*ppv - 1) != (MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc))
				OSReport("Concat Bug\n");			
#endif		
*/
		if(MEM_b_ConcatHole((ULONG *) (*ppv), 1))
			ppv = (ULONG **) &MEM_gp_OldHolesList;					
		else
		{
			if(MEM_b_ConcatHole((ULONG *) (*ppv), 0))
				ppv = (ULONG **) &MEM_gp_OldHolesList;					
			else
				ppv = (ULONG **) *ppv;
		}
	}
	//MEM_M_EndCriticalSection();	
/*
#ifdef _DEBUG
{	
	MEM_ComputeMemoryHoleInfo(&ul_NumHoles, &ul_SizeOfHoles, FALSE);		
	
	sprintf(asz_Log, "%u Old Holes After  Concat (Size=%d) (Num Holes=%u, Size Holes=%d B)\n", MEM_ul_NumOldHoles, MEM_ul_SizeOldHoles, ul_NumHoles, ul_SizeOfHoles);
	OSReport(asz_Log);
}
#endif
*/	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_EmergencyConcat()
{
	int 	i;
	ULONG	**ppv;
	MEM_tdst_ZlistEntry *pst_Zelem;	
#ifdef _DEBUG
//	char asz_Log[512];
	ULONG	ul_NumHoles, ul_SizeOfHoles;
#endif	
	
	if(!MEM_gp_OldHolesList)
		return;
	
#ifdef _DEBUG
{		
	MEM_ComputeMemoryHoleInfo(&ul_NumHoles, &ul_SizeOfHoles, FALSE);	
			
//	sprintf(asz_Log, "%u Old Holes Before Emergency Concat (Size=%u) (Num Holes=%u, Size Holes=%d B)\n", MEM_ul_NumOldHoles, MEM_ul_SizeOldHoles, ul_NumHoles, ul_SizeOfHoles);
//	OSReport(asz_Log);
}
#endif		
				
	/* Old Holes List */				
	ppv = (ULONG **) &MEM_gp_OldHolesList;					
	while(*ppv)
	{
		if(MEM_b_ConcatHole((ULONG *) (*ppv), 1))
			ppv = (ULONG **) &MEM_gp_OldHolesList;					
		else
		{
			if(MEM_b_ConcatHole((ULONG *) (*ppv), 0))
				ppv = (ULONG **) &MEM_gp_OldHolesList;					
			else

				ppv = (ULONG **) *ppv;
		}
	}

	/* Classical Holes */
	for(i = 0; i < MEM_C_ZlistSize; i++)
	{
		pst_Zelem = MEM_gst_MemoryInfo.MEM_ast_Zlist + i;	
		ppv = (ULONG**)&pst_Zelem->pv_Hole;
		while(*ppv)
		{
			if(MEM_b_ConcatHole((ULONG *) (*ppv), 1))
				ppv = (ULONG **) &pst_Zelem->pv_Hole;					
			else
			{
				if(MEM_b_ConcatHole((ULONG *) (*ppv), 0))
					ppv = (ULONG **) &pst_Zelem->pv_Hole;					
				else

					ppv = (ULONG **) *ppv;
			}
		}
	}	
	
#ifdef _DEBUG
{	
	MEM_ComputeMemoryHoleInfo(&ul_NumHoles, &ul_SizeOfHoles, FALSE);		
	
//	sprintf(asz_Log, "%u Old Holes After Emergency Concat (Size=%u) (Num Holes=%u, Size Holes=%d B)\n", MEM_ul_NumOldHoles, MEM_ul_SizeOldHoles, ul_NumHoles, ul_SizeOfHoles);
//	OSReport(asz_Log);
}
#endif
}
	


#endif /* USE_HOLE_OPTIM */

#endif // MEM_OPT