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

#if defined( _DEBUG ) && !defined( PSX2_TARGET ) && !defined( _GAMECUBE )
#	include "BASe/CLIbrary/CLIstr.h"
#	include "BASe/CLIbrary/CLIwin.h"
#	include "LINks/LINKmsg.h"
#endif /* debug + PC */

/*$2------------------------------------------------------------------------------------------------------------------*/

#ifndef MEM_OPT
//#define MEM_USE_TYPES

/*
 =======================================================================================================================
    Aim:    Return the size of a block (the initially asked size, not the really allocated size)

    Note:   This function works only with blocks allocated with MEM_p_Alloc, since blocks allocated with MEM_p_EcoAlloc
            don't have the size stored.
 =======================================================================================================================
 */
_inline_ ULONG MEM_ul_GetSize( void *_pv_Block )
{
	return ( *( ( ( ULONG * ) _pv_Block ) - 1 ) );
}


/*
 =======================================================================================================================
    Aim:    Sets the size of a block
 =======================================================================================================================
 */
_inline_ void MEM_SetSize( void *_pv_Block, ULONG _ul_Size )
{
	( *( ( ( ULONG * ) _pv_Block ) - 1 ) ) = _ul_Size;
}

ULONG MEM_ul_GetRealSize( void *_pv_Block )
{
	ULONG ul_Size;
	ul_Size = MEM_ul_GetSize( _pv_Block );
	return ( MEM_ul_GetRealSizeFromSize( ul_Size ) );
}

/*$4
 ***********************************************************************************************************************
    MACROS
 ***********************************************************************************************************************
 */

#	define MEM_Mb_IsAllocPointer( p ) \
		( ( ( p ) >= MEM_gst_MemoryInfo.pv_DynamicBloc ) && ( ( p ) < MEM_gst_MemoryInfo.pv_DynamicNextFree ) )

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Multithread access
 -----------------------------------------------------------------------------------------------------------------------
 */

#	if defined( _XENON_RENDER )

static CRITICAL_SECTION s_oMemoryAccessLock;

#		define MEM_M_InitLock()                                   \
			do                                                     \
			{                                                      \
				InitializeCriticalSection( &s_oMemoryAccessLock ); \
			} while ( 0 )

#		define MEM_M_BeginCriticalSection()                  \
			do                                                \
			{                                                 \
				EnterCriticalSection( &s_oMemoryAccessLock ); \
			} while ( 0 )

#		define MEM_M_EndCriticalSection()                    \
			do                                                \
			{                                                 \
				LeaveCriticalSection( &s_oMemoryAccessLock ); \
			} while ( 0 )

#		define MEM_M_DestroyLock()                            \
			do                                                 \
			{                                                  \
				DeleteCriticalSection( &s_oMemoryAccessLock ); \
			} while ( 0 )

#	else
static HANDLE sh_MultiThreadingLock;
#		define MEM_M_InitLock()                                                 \
			do                                                                   \
			{                                                                    \
				sh_MultiThreadingLock = ( HANDLE ) 0;                            \
				sh_MultiThreadingLock = CreateEvent( NULL, FALSE, FALSE, NULL ); \
				ERR_X_Assert( sh_MultiThreadingLock );                           \
				SetEvent( sh_MultiThreadingLock );                               \
			} while ( 0 )
#		define MEM_M_BeginCriticalSection()                            \
			do                                                          \
			{                                                           \
				WaitForSingleObject( sh_MultiThreadingLock, INFINITE ); \
			} while ( 0 )
#		define MEM_M_EndCriticalSection()         \
			do                                     \
			{                                      \
				SetEvent( sh_MultiThreadingLock ); \
			} while ( 0 )
#		define MEM_M_DestroyLock()                   \
			do                                        \
			{                                         \
				CloseHandle( sh_MultiThreadingLock ); \
				sh_MultiThreadingLock = ( HANDLE ) 0; \
			} while ( 0 )
#	endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    signal fatal error
 -----------------------------------------------------------------------------------------------------------------------
 */

#	if defined( _FINAL_ )
/*$1- final = no signal ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#		define MEM_M_SignalFatalError( a, b )
#	else
#		define MEM_M_SignalFatalError( a, b ) MessageBox( 0, "No more memory", "No more memory", MB_OK )
#	endif

#	define PRELOADTEXGRAN ( 40 * 1024 )
extern void PreLoadTexReset( void );
extern char *gp_LastPointer; /* Pointer from end of block to free zone */
extern void PreloadCancel( void );
void MEM_InitDynamic( ULONG _ul_BigBlockSize );
void MEM_CloseDynamic( void );
//void MEM_FreeFromEnd(void *_pv_block);

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
#	ifdef JADEFUSION
__declspec( align( 32 ) ) MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#	else
MEM_tdst_MainStruct MEM_gst_MemoryInfo;
#	endif

char *MEM_gp_MaxPeakMemory       = 0x00000000;
void *MEM_gp_OldHolesList        = NULL;
ULONG MEM_ul_NumOldHoles         = 0;
ULONG MEM_ul_SizeOldHoles        = 0;
ULONG MEM_ul_UpdateHoles         = 0;
ULONG MEM_ul_EmergencyConcat     = 0;
void *MEM_gp_NextFreeAfterDefrag = NULL;
int MEM_gi_MinDelta              = 0x7FFFFFFF;
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    history
 -----------------------------------------------------------------------------------------------------------------------
 */

#	ifdef MEM_USE_HISTORY

#		ifdef PSX2_TARGET
#			define MEM_Cul_AllocationHistorySize 200000
#		else
#			define MEM_Cul_AllocationHistorySize 1000000
#		endif

LONG MEM_gap_AllocationHistoryTable[ MEM_Cul_AllocationHistorySize ];
char *MEM_gapsz_AllocatedInFile[ MEM_Cul_AllocationHistorySize ];
int MEM_gai_AllocatedInLine[ MEM_Cul_AllocationHistorySize ];
LONG MEM_g_nbpointers = 0;

LONG MEM_gap_FreeHistoryTable[ MEM_Cul_AllocationHistorySize ];
char *MEM_gapsz_FreeInFile[ MEM_Cul_AllocationHistorySize ];
int MEM_gai_FreeInLine[ MEM_Cul_AllocationHistorySize ];
#	endif// MEM_USE_HISTORY

/*$4
 ***********************************************************************************************************************
    PROTOTYPES
 ***********************************************************************************************************************
 */

char MEM_b_CheckBlockAllocated( void * );
void MEM_CheckBlockIsFree( void * );
void MEM_CheckHolesChain( void *, ULONG );
void MEM_Defrag( int single );
void MEM_FreeTmp( void * );
extern void MEM_ComputeMemoryHoleInfo( ULONG *, ULONG *, BOOL );

#	define MEM_DeleteHoleInChain( a )
#	define MEM_p_SplitHoleInChain( a, b ) ( NULL )
#	define MEM_p_GetHoleInChain( a )      ( NULL )
#	define MEM_RegisterHoleInChain( a, b )
#	define MEM_b_ConcatHolesInChain( a, b ) ( FALSE )

static int MEM_s_DynamicMemoryCheck = 1;

/*$4
 ***********************************************************************************************************************
    public functions (all targets)
 ***********************************************************************************************************************
 */

extern int GetHeapSize( void );
extern int GetStackSize( void );
extern int GetStackStart( void );

#	ifdef _XENON
PVOID g_DbgMemPool;// use debug memory until officially 512MB
// stupid dm allocator does not reinitialize on reset of console so address is hard coded
class DMMemory
{
public:
	DMMemory()
	{
		g_DbgMemPool = DmAllocatePool( DM_MEM_SIZE );
		if ( !g_DbgMemPool ) g_DbgMemPool = ( LPVOID ) 0xb01c1000;
	}
	~DMMemory()
	{
		if ( g_DbgMemPool ) DmFreePool( g_DbgMemPool );
	}
};

//DMMemory DMMemManager;

#	endif

/*
 =======================================================================================================================
    Aim:    Init the memory module
 =======================================================================================================================
 */
void MEM_InitModule( void )
{
	extern BOOL LOA_gb_SpeedMode;

	MEM_M_InitLock();

	MEM_gst_MemoryInfo.pv_LastFree = NULL;
	MEM_gst_MemoryInfo.ul_RealSize = 0;

	MEM_InitDynamic( ( LONG ) 256 * ( LONG ) 1024 * ( LONG ) 1024 );

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#	ifdef MEM_USE_HISTORY
	L_memset( MEM_gap_AllocationHistoryTable, 0, sizeof( MEM_gap_AllocationHistoryTable ) );
	L_memset( MEM_gap_FreeHistoryTable, 0, sizeof( MEM_gap_FreeHistoryTable ) );
#	endif
}

/*
 =======================================================================================================================
    Aim:    Close the memory module
 =======================================================================================================================
 */
void MEM_CloseModule( void )
{
	MEM_CloseDynamic();
	MEM_M_DestroyLock();
}

/*
 =======================================================================================================================
    Aim:    Allocates the main dynamic block and initialises the Z list.
 =======================================================================================================================
 */
void MEM_InitDynamic( ULONG _ul_BigBlockSize )
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int i, j, iSize, z, iStep;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	/* Allocates the Big Dynamic bloc using static allocation */
#	ifdef WIN32

	/* Plus rapide qu'au dessous sous windows */
	MEM_gst_MemoryInfo.pv_DynamicBloc = HeapAlloc( GetProcessHeap(), 0, _ul_BigBlockSize );
#	else
	MEM_gst_MemoryInfo.pv_DynamicBloc = ( void * ) ( MEM_p_sAlloc( _ul_BigBlockSize ) );
#	endif
	/* Check that we had enough system memory to perform the allocation */
	ERR_X_ErrorAssert( MEM_gst_MemoryInfo.pv_DynamicBloc != 0, MEM_ERR_Csz_NotEnoughSystemMemory, NULL );

	z     = 0;
	iSize = 0;

	/* Initializes the sizes of the entries in the Z list */
	for ( i = 0; i < 18; i++ )
	{
		/* We calculate the new step (we have 18 different steps) */
		iStep = 1 << ( i + 2 );

		/* We have 128 z with the same step */
		for ( j = 0; j < 128; j++ )
		{
			MEM_gst_MemoryInfo.MEM_ast_Zlist[ z ].ul_NumFreeDefrag = 0;

			iSize += iStep;
			MEM_gst_MemoryInfo.MEM_ast_Zlist[ z ].ul_Size = iSize;

			/* No holes */
			MEM_gst_MemoryInfo.MEM_ast_Zlist[ z ].pv_Hole = 0;
			z++;
		}
	}

	/* We init the Next free pointer */
	MEM_gst_MemoryInfo.pv_DynamicNextFree = MEM_gst_MemoryInfo.pv_DynamicBloc;
	MEM_gst_MemoryInfo.ul_DynamicMaxSize  = _ul_BigBlockSize;

	/* At the beginning, allocated size is 0 */
	MEM_gst_MemoryInfo.ul_DynamicCurrentAllocated  = 0;
	MEM_gst_MemoryInfo.ul_TexturesCurrentAllocated = 0;
	MEM_gst_MemoryInfo.ul_HolesStatic              = 0;
}

/*
 =======================================================================================================================
    Aim:    Frees the memory of the main dynamic module
 =======================================================================================================================
 */
void MEM_CloseDynamic( void )
{
#	ifdef WIN32
	HeapFree( GetProcessHeap(), 0, MEM_gst_MemoryInfo.pv_DynamicBloc );
#	else
	MEM_sFree( MEM_gst_MemoryInfo.pv_DynamicBloc );
#	endif
}

void MEM_PrintHistory( void )
{
}

#	ifdef _DEBUG
#		define MEM_Csz_PointerOutOfRange   "Pointer out of range detected"
#		define MEM_Csz_NonalignedPointer   "Non aligned pointer detected"
#		define MEM_Csz_NullPointerDetected "Null pointer detected"

/*
 =======================================================================================================================
    Aim:    Check the validity of a pointer
 =======================================================================================================================
 */
char MEM_b_CheckPointer( void *p )
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	LONG l_max, l_min, l_p;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	_Try_
	        l_p = ( LONG ) p;

	ERR_X_ErrorAssert( l_p, MEM_Csz_NullPointerDetected, NULL );

#		if 0
	l_min = (LONG) MEM_gst_MemoryInfo.pv_DynamicBloc;
	l_max = (LONG) MEM_gst_MemoryInfo.pv_DynamicBloc + MEM_gst_MemoryInfo.ul_DynamicMaxSize;

	ERR_X_ErrorAssert(((l_p > l_min) && (l_p < l_max)), MEM_Csz_PointerOutOfRange, "");
	ERR_X_ErrorAssert(((l_p & 3) == 0), MEM_Csz_NonalignedPointer, "");
#		endif
	_Catch_ return ( 0 );
	_End_ return ( 1 );
}

#endif /* _DEBUG */

/*
 =======================================================================================================================
    Aim:    We check that the Main dynamic bloc is not full
 =======================================================================================================================
 */
_inline_ void MEM_CheckDynamicBlocFull( void )
{
	/*~~~~~~~~~~~~~~~*/

	/* Check that we had enough memory in the dynamic bloc */
	char c_SizeTest;
	/*~~~~~~~~~~~~~~~*/

	c_SizeTest =
	        ( ( ( char * ) MEM_gst_MemoryInfo.pv_DynamicNextFree ) -
	                  ( ( char * ) MEM_gst_MemoryInfo.pv_DynamicBloc ) <
	          ( LONG ) MEM_gst_MemoryInfo.ul_DynamicMaxSize );

	ERR_X_ErrorAssert( c_SizeTest, MEM_ERR_Csz_NoMoreMemoryToPerformAllocation, NULL );
}

/* Aim: We check that a block has been correctly allocated (check the mark) */
#	ifdef _DEBUG

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char MEM_b_CheckBlockAllocated( void *_pv_Block )
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG ul_size;
	ULONG ul_BlockMark;
	/*~~~~~~~~~~~~~~~~~*/

	_Try_
	        /* Check if a pointer was correctly allocated */
	        if ( MEM_b_CheckPointer( _pv_Block ) )
	{
		ul_BlockMark = MEM_ul_GetMark( _pv_Block );

		/*$F hints to debug if asserts here:
			1: watch (ul_BlockMark&0xFFFFFF):
					if different from 0x110270, the bloc has been allocated with
					another function than MEM_p_Alloc, or memory has been overwriten.
			2: watch (ul_BlockMark&0xF0000000):  
					The normal value should be 0xA0000000
					if equals 0xB0000000, the bloc has already been freed ! 
					
		*/
		ERR_X_Assert( ( ul_BlockMark & 0xF0000000 ) == MEM_Cul_BlockAllocated );
		ERR_X_Assert(
		        (
		                ( ( ul_BlockMark & 0xFFFFFF ) == MEM_Cul_BlockMarkMEMpAlloc ) || ( ( ul_BlockMark & 0xFFFFFF ) == MEM_Cul_BlockMarkMEMpAllocAlign ) ) );

		/* Check that the size is coherent */
		ul_size = MEM_ul_GetSize( _pv_Block );
		ERR_X_Assert( ( ul_size > 0 ) && ( ul_size < MEM_Cul_MaxSize ) );
	}

	_Catch_ return 0;
	_End_ return 1;
}

/*
 =======================================================================================================================
    Aim:    We check that a block is free
 =======================================================================================================================
 */
void MEM_CheckBlockIsFree( void *_pv_Block )
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG ul_BlockMark;
	/*~~~~~~~~~~~~~~~~~*/

	if ( _pv_Block != 0 )
	{
		ul_BlockMark = MEM_ul_GetMark( _pv_Block );

		/* Check that the block is free */
		ERR_X_Assert( ( ul_BlockMark & 0xF0000000 ) == MEM_Cul_BlockFree );

		/* Check it has been freed by MEM_p_Free */
		ERR_X_Assert( ( ul_BlockMark & 0xFFFFFF ) == MEM_Cul_BlockMarkMEMpAlloc );
	}
}

/*
 =======================================================================================================================
    Aim:    We check that the holes chain is OK
 =======================================================================================================================
 */
void MEM_CheckHolesChain( void *_pv_Hole, ULONG index )
{
	/*~~~~~~~~~~~~~~~*/
	ULONG test;
	ULONG ul_EffSize;
	ULONG *p;
	/*~~~~~~~~~~~~~~~*/

	ul_EffSize = MEM_gst_MemoryInfo.MEM_ast_Zlist[ index ].ul_Size +
	             MEM_C_BlockHeaderSize;

	if ( _pv_Hole )
	{
		test = *( LONG * ) _pv_Hole;
		while ( test != 0 )
		{
			/* The start of the block is always 4 bytes after the pointer (in debug or not) */
			MEM_CheckBlockIsFree( ( void * ) ( test + 4 ) );

			p = ( ULONG * ) test;
			ERR_X_Assert( *( p + ( ul_EffSize >> 2 ) - 2 ) == ul_EffSize );

			test = *( LONG * ) test;
		}
	}
}

#	endif /* _DEBUG */

#	ifdef USE_HOLE_OPTIM
extern void MEM_UpdateHoles();
extern BOOL MEM_b_FindOldHole( ULONG );
#	endif
/*$off*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int s_Bichon = 0;
void MEM_Defrag( int single )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int index, size;
	ULONG *p, *p1, *p2;
	MEM_tdst_ZlistEntry *pst_Zelem;
	void *next;
	void *next_prev;
	int i;
	int fuckingE3;
	static int i_Last = 0;
#	ifdef USE_HOLE_OPTIM
	char asz_Log[ 512 ];
#	endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#	ifdef MEM_USE_TYPES
	return;
#	endif

	fuckingE3 = 0;
	MEM_M_BeginCriticalSection();

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    try to concat some free clusters
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for ( i = i_Last; i < MEM_C_ZlistSize; i++ )
	{
		pst_Zelem = MEM_gst_MemoryInfo.MEM_ast_Zlist +
		            i;
		if ( pst_Zelem->pv_Hole )
		{
			MEM_b_ConcatHolesInChain( ( ULONG * ) pst_Zelem->pv_Hole, i );
			if ( single ) break;
		}
	}

	i_Last = ( i < MEM_C_ZlistSize ) ? i : 0;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    looking for free mark from end of memory
	 -------------------------------------------------------------------------------------------------------------------
	 */

	while ( 1 )
	{
		p = ( ULONG * ) MEM_gst_MemoryInfo.pv_DynamicNextFree;
		if ( p == MEM_gst_MemoryInfo.pv_DynamicBloc ) break;

		size = *( p - 1 );
		if ( size <= 0 ) break;
		if ( size & 3 ) break;
		if ( size < MEM_C_BlockHeaderSize ) break;

		p1 = p - ( size >> 2 );
		if ( p1 <= ( ULONG * ) MEM_gst_MemoryInfo.pv_DynamicBloc ) break;
		if ( p1 > ( ULONG * ) MEM_gst_MemoryInfo.pv_DynamicNextFree ) break;

		if ( *p1 == ( MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc ) )
		{
			if ( *( p1 + 1 ) && ( *( p1 + 1 ) < ( ULONG ) MEM_gst_MemoryInfo.pv_DynamicBloc ) ) break;
			if ( *( p1 + 1 ) > ( ULONG ) ( ( char * ) MEM_gst_MemoryInfo.pv_DynamicNextFree + MEM_gst_MemoryInfo.ul_DynamicMaxSize ) ) break;
			if ( *( p1 + 2 ) != ( ULONG ) size ) break;
			p2 = ( ULONG * ) *( p1 + 1 );

			if ( p2 )
			{

				if ( *( p2 - 1 ) != ( MEM_Cul_BlockFree | MEM_Cul_BlockMarkMEMpAlloc ) )
				{
					ERR_X_Assert( 0 );
					break;
				}
				if ( *p2 && *p2 < ( ULONG ) MEM_gst_MemoryInfo.pv_DynamicBloc )
				{
					ERR_X_Assert( 0 );
					break;
				}

				if ( *p2 > ( ULONG ) ( ( char * ) MEM_gst_MemoryInfo.pv_DynamicNextFree + MEM_gst_MemoryInfo.ul_DynamicMaxSize ) )
				{
					ERR_X_Assert( 0 );
					break;
				}

#	ifndef _GAMECUBE
				if ( *( p2 + 1 ) != ( ULONG ) size )
				{
					ERR_X_Assert( 0 );
					break;
				}
#	endif
			}

			index = MEM_ul_GetIndex( size - MEM_C_BlockHeaderSize );
			MEM_gst_MemoryInfo.MEM_ast_Zlist[ index ].ul_NumFreeDefrag++;
#	ifdef JADEFUSION
			//POPOWARNING (char *) MEM_gst_MemoryInfo.pv_DynamicNextFree -= size;
#	else
			( char * ) MEM_gst_MemoryInfo.pv_DynamicNextFree -= size;
#	endif
			fuckingE3 = size;
			if ( single ) break;
		}
		else
			break;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    unlink all free buffers
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for ( i = 0; i < MEM_C_ZlistSize; i++ )
	{
		pst_Zelem = &MEM_gst_MemoryInfo.MEM_ast_Zlist[ i ];
		if ( !pst_Zelem->ul_NumFreeDefrag ) continue;

		next = pst_Zelem->pv_Hole;
		if ( !next )
		{
#	ifdef JADEFUSION
			if ( single ) MEM_gst_MemoryInfo.pv_DynamicNextFree = ( void * ) ( ( int ) MEM_gst_MemoryInfo.pv_DynamicNextFree + fuckingE3 );//(char *) MEM_gst_MemoryInfo.pv_DynamicNextFree += fuckingE3;
#	else
			if ( single ) ( char * ) MEM_gst_MemoryInfo.pv_DynamicNextFree += fuckingE3;
#	endif
			pst_Zelem->ul_NumFreeDefrag = 0;
			continue;
		}

		next_prev = NULL;
		while ( pst_Zelem->ul_NumFreeDefrag )
		{
			while ( ( char * ) next - 4 < ( char * ) MEM_gst_MemoryInfo.pv_DynamicNextFree )
			{
				next_prev = next;
				next      = *( void      **) next;
#	ifndef USE_HOLE_OPTIM
				ERR_X_Assert( next );
#	endif
			}

#	ifdef USE_HOLE_OPTIM
			if ( !next )
			{
				pst_Zelem->ul_NumFreeDefrag = 0;
				continue;
			}
#	endif

			*( ( ULONG ** ) next - 1 ) = 0; /* Clean free mark */
			MEM_DeleteHoleInChain( ( ULONG * ) next );

			pst_Zelem->ul_NumFreeDefrag--;
			next = *( void ** ) next;
			if ( !next_prev )
			{
				pst_Zelem->pv_Hole = next;
			}
			else
			{
				*( void ** ) next_prev = next;
			}
		}
	}

#	ifdef USE_HOLE_OPTIM
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    unlink all free buffers from OldHolesList
	 -------------------------------------------------------------------------------------------------------------------
	 */
	{
		int test;

		if ( MEM_gp_OldHolesList )
		{
			ULONG *pul_Hole;
			ULONG *pul_PreviousHole;


			test             = 0;
			pul_PreviousHole = NULL;
			pul_Hole         = MEM_gp_OldHolesList;
		pipik:
			while ( pul_Hole )
			{
				/*$F
				sprintf(asz_Log, "Hole = %x / Next Free = %x\n", (ULONG) pul_Hole, (ULONG) MEM_gst_MemoryInfo.pv_DynamicNextFree);
				if(s_Bichon)
					OSReport(asz_Log);
				 */

				test++;
				/*$F
		 		if((ULONG) pul_Hole == (ULONG) MEM_gst_MemoryInfo.pv_DynamicNextFree)
		 			OSReport("Cas batard à traiter\n");
		 		 */

				if ( ( ULONG ) pul_Hole > ( ULONG ) MEM_gst_MemoryInfo.pv_DynamicNextFree )
				{
					if ( pul_PreviousHole )
					{
						MEM_DeleteHoleInChain( ( ULONG * ) pul_Hole );
						*pul_PreviousHole = *pul_Hole;
						MEM_ul_NumOldHoles--;
						test--;
						MEM_ul_SizeOldHoles -= *( pul_Hole + 1 );
						*( pul_Hole - 1 ) = 0; /* Clean Free Mark */

						/* To loop again on the previous */
						pul_Hole = pul_PreviousHole;
					}
					else
					{
						MEM_DeleteHoleInChain( ( ULONG * ) pul_Hole );
						MEM_gp_OldHolesList = ( ULONG * ) *pul_Hole;
						MEM_ul_NumOldHoles--;
						test--;
						MEM_ul_SizeOldHoles -= *( pul_Hole + 1 );
						*( pul_Hole - 1 ) = 0; /* Clean Free Mark */
						pul_Hole          = ( ULONG          *) *pul_Hole;
						pul_PreviousHole  = NULL;
						goto pipik;
					}
				}
				pul_PreviousHole = pul_Hole;
				pul_Hole         = ( ULONG         *) *pul_Hole;
			}

			/*$F
		 	if((ULONG)test != MEM_ul_NumOldHoles)
		 	{
		 		OSReport("Defrag PB\n");
		 	}
		 	*/
		}
	}
#	endif

	MEM_M_EndCriticalSection();

	if ( !single ) MEM_gp_NextFreeAfterDefrag = MEM_gst_MemoryInfo.pv_DynamicNextFree;
}

/*$4
 ***********************************************************************************************************************
    ALLOC TMP
 ***********************************************************************************************************************
 */

char *MEM_gp_AllocTmpFirst = NULL;
char *MEM_gp_AllocTmpLast  = NULL;
char *MEM_gp_AllocTmpNext  = NULL;
BOOL MEM_gb_EnableTmp      = FALSE;
int MEM_gi_AllocTmpNb      = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_p_AllocTmp( ULONG ulSize )
{
#	ifdef ACTIVE_EDITORS
	ulSize += 128;
	ulSize &= ~63;
	return MEM_p_Alloc( ulSize );

	/*~~~~~~~~*/
#	else
	char *p;
	ULONG *p1;
	/*~~~~~~~~*/

	ulSize += 128;
	ulSize &= ~63;
	if ( !MEM_gp_AllocTmpFirst || !MEM_gb_EnableTmp ) return MEM_p_AllocAlign( ulSize, 64 );
	if ( MEM_gp_AllocTmpNext + ulSize >= MEM_gp_AllocTmpLast ) return MEM_p_AllocAlign( ulSize, 64 );

	MEM_M_BeginCriticalSection();
	p = MEM_gp_AllocTmpNext +
	    4;
	p1  = ( ULONG  *) MEM_gp_AllocTmpNext;
	*p1 = ulSize;
	MEM_gp_AllocTmpNext += ulSize;
	MEM_gi_AllocTmpNb++;
	MEM_M_EndCriticalSection();

	return p;
#	endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_FreeTmp( void *p )
{
	/*~~~~~~~~~~~~~~*/
	ULONG *p1, size;
	/*~~~~~~~~~~~~~~*/

	p1 = ( ULONG * ) p;
	p1--;
	size = *p1;

	MEM_M_BeginCriticalSection();
	if ( ( char * ) p + size - 4 == MEM_gp_AllocTmpNext ) MEM_gp_AllocTmpNext = ( char * ) p1;

	MEM_gi_AllocTmpNb--;
	MEM_M_EndCriticalSection();

	if ( !MEM_gi_AllocTmpNb )
	{
		MEM_ResetTmpMemory();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *MEM_p_ReallocTmp( void *_pv_Block, ULONG _ul_BlockSize )
{
	/*~~~~~~~*/
	char *p;
	ULONG size;
	/*~~~~~~~*/

	size = *( ( ( ULONG * ) _pv_Block ) - 1 );

	p = ( char * ) MEM_p_AllocTmp( _ul_BlockSize );
	L_memmove( p, _pv_Block, size );
	MEM_Free( _pv_Block );

	return p;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_ResetTmpMemory( void )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern void SOFT_ZList_Init( void );
	extern void SOFT_ZList_Clear( void );
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MEM_M_BeginCriticalSection();
	MEM_gp_AllocTmpNext = MEM_gp_AllocTmpFirst;
	MEM_gi_AllocTmpNb   = 0;
	MEM_M_EndCriticalSection();

#	if !defined( MAX_PLUGIN )
	SOFT_ZList_Init();
	SOFT_ZList_Clear();
#	endif// !defined(MAX_PLUGIN)
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    debugging functions
 -----------------------------------------------------------------------------------------------------------------------
 */

LONG *MEM_gp_LastAllocated = ( ( LONG * ) -1 );
LONG MEM_gp_PrintCluster   = 0;
void MEM_dbg_PrintClustersChain( void );
void MEM_dbg_PrintMemBlock( void );
void MEM_dbg_FindLastAllocated( void );

#	ifdef _GAMECUBE
#		define _print OSReport
#	elif defined( PSX2_TARGET )
#		define _print printf
#	else
FILE *gfh = NULL;

#		define _print( a )                            \
			do                                         \
			{                                          \
				if ( !gfh )                            \
				{                                      \
					gfh = fopen( "memdyn.log", "w+" ); \
				}                                      \
				if ( gfh )                             \
				{                                      \
					fprintf( gfh, a );                 \
				}                                      \
			} while ( 0 );
#	endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_dbg_FindLastAllocatedCluster( void )
{
	MEM_dbg_FindLastAllocated();
	MEM_dbg_PrintClustersChain();
	MEM_dbg_PrintMemBlock();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_dbg_FindLastAllocated( void )
{
	/*~~~~~~~~~~~~~~~~*/
	LONG *p;
	char memlog[ 128 ];
	/*~~~~~~~~~~~~~~~~*/

	if ( MEM_gp_LastAllocated ) return;

	MEM_gp_LastAllocated = ( LONG * ) MEM_gst_MemoryInfo.pv_DynamicNextFree;

	while ( ( ULONG ) MEM_gp_LastAllocated > ( ULONG ) MEM_gst_MemoryInfo.pv_DynamicBloc )
	{
		p = MEM_gp_LastAllocated -
		    1;
		while (
		        ( *p != ( LONG ) ( MEM_Cul_BlockMarkMEMpAlloc + MEM_Cul_BlockAllocated ) ) && ( *p != ( LONG ) ( MEM_Cul_BlockMarkMEMpAllocAlign + MEM_Cul_BlockAllocated ) ) )
		{
			p = p - 1;
		}

		MEM_gp_LastAllocated = p;
		snprintf( memlog, sizeof( memlog ), "%08x - %d\n", ( uintptr_t ) p, *( p + 1 ) );
		_print( memlog );
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_dbg_PrintClustersChain( void )
{
	/*~~~~~~~~~~~~~~~~*/
	ULONG *p;
	ULONG *pfree;
	ULONG *p1stfree;
	char memlog[ 128 ];
	/*~~~~~~~~~~~~~~~~*/

	pfree    = NULL;
	p1stfree = NULL;

	if ( !MEM_gp_PrintCluster ) return;
#	ifdef JADEFUSION
	p = ( ULONG * ) MEM_gst_MemoryInfo.pv_DynamicBloc;
#	else
	p = MEM_gst_MemoryInfo.pv_DynamicBloc;
#	endif
	while ( 1 )
	{
		switch ( *p )
		{
			case ( ( ULONG ) ( MEM_Cul_BlockMarkMEMpAlloc + MEM_Cul_BlockAllocated ) ):
			case ( ( ULONG ) ( MEM_Cul_BlockMarkMEMpAllocAlign + MEM_Cul_BlockAllocated ) ):
				if ( pfree )
				{
					sprintf( memlog, "Free, %x, %d\n", ( ULONG ) pfree, ( ULONG ) p - ( ULONG ) pfree );
					_print( memlog );
					pfree = NULL;
				}

				if ( p1stfree )
				{
					sprintf( memlog, "Free block, %x, %d\n", ( ULONG ) p1stfree, ( ULONG ) p - ( ULONG ) p1stfree );
					_print( memlog );
					p1stfree = NULL;
				}

				sprintf( memlog, "Allo, %x, %d\n", ( ULONG ) p, *( p + 1 ) + 8 );
				_print( memlog );
				p++;
				break;

			case ( ( ULONG ) ( MEM_Cul_BlockFree + MEM_Cul_BlockMarkMEMpAlloc ) ):
				if ( pfree )
				{
					sprintf( memlog, "Free, %x, %d\n", ( ULONG ) pfree, ( ULONG ) p - ( ULONG ) pfree );
					_print( memlog );
					pfree = p;
				}
				else
				{
					pfree = p;
				}

				if ( !p1stfree ) p1stfree = p;
				p++;
				break;

			default:
				p++;
				break;
		}

		if ( ( ULONG ) p >= ( ULONG ) MEM_gst_MemoryInfo.pv_DynamicNextFree ) break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void MEM_dbg_PrintMemBlock( void )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int index;
	ULONG size;
	ULONG **ppc_hole;
	ULONG *pfree;
#	ifdef MEM_USE_HISTORY
	int i, j;
#	endif
	char memlog[ 256 ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !MEM_gp_PrintCluster ) return;

	for ( index = 0; index < MEM_C_ZlistSize; index++ )
	{
		ppc_hole = ( ULONG ** ) &MEM_gst_MemoryInfo.MEM_ast_Zlist[ index ].pv_Hole;
		size     = MEM_gst_MemoryInfo.MEM_ast_Zlist[ index ].ul_Size +
		       MEM_C_BlockHeaderSize;

		while ( *ppc_hole )
		{
			pfree = *ppc_hole + 1;

#	ifdef MEM_USE_HISTORY
			if ( ( int ) ( pfree - 2 ) < ( int ) MEM_gst_MemoryInfo.pv_DynamicNextFree )
			{
				i = ( int ) ( pfree ) % MEM_Cul_AllocationHistorySize;
				j = -2;
				while ( ( ULONG ) MEM_gap_FreeHistoryTable[ i ] != ( ULONG ) pfree )
				{
					i = ( i + 1 ) % MEM_Cul_AllocationHistorySize;
					j++;
					if ( j > MEM_Cul_AllocationHistorySize ) break;
					ERR_X_Assert( i < MEM_Cul_AllocationHistorySize );
				}

				if ( j > MEM_Cul_AllocationHistorySize )
				{
					sprintf( memlog, "Free, %08d, %08x ?(?)\n", size, ( ULONG ) pfree );
				}
				else
				{
					sprintf(
					        memlog,
					        "Free, %08d, %08x %s(%d)\n",
					        size,
					        ( ULONG ) pfree,
					        MEM_gapsz_FreeInFile[ i ],
					        MEM_gai_FreeInLine[ i ] );
				}

				_print( memlog );
			}

#	else
			sprintf( memlog, "Free, %08d, %08x\n", size, ( ULONG ) pfree );
			_print( memlog );
#	endif
			ppc_hole = ( ULONG ** ) *ppc_hole;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char MEM_b_CheckBlockHasBeenFreed( void *_pv_Block )
{
	/*~~~~~~~~~~~~~~~~~*/
	ULONG ul_size;
	ULONG ul_BlockMark;
	/*~~~~~~~~~~~~~~~~~*/

	ul_BlockMark = MEM_ul_GetMark( _pv_Block );

	if ( ( ul_BlockMark & 0xF0000000 ) != MEM_Cul_BlockAllocated ) return 1;
	if ( ( ( ul_BlockMark & 0xFFFFFF ) != MEM_Cul_BlockMarkMEMpAlloc ) && ( ( ul_BlockMark & 0xFFFFFF ) != MEM_Cul_BlockMarkMEMpAllocAlign ) ) return 1;

	/* Check that the size is coherent */
	ul_size = MEM_ul_GetSize( _pv_Block );
	if ( !( ( ul_size > 0 ) && ( ul_size < MEM_Cul_MaxSize ) ) ) return 1;

	return 0;
}

#endif// MEM_OPT
