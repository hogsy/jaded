//-----------------------------------------------------------------------------
// File: XBProfiling.cpp
//
// Desc: Helper classes to support easier profiling, as describe in
//       the Holistic Guide to Xbox Profilers whitepaper.
//
// Hist: 10.22.03 - New for December 2003 XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#if !defined(_XENON)
#include <xtl.h>
#endif
#include "Precomp.h"
#include "XeProfiling.h"
#if defined(_XENON)
#include <xbdm.h>
#endif

#include "ENGine/Sources/WORld/WORaccess.h"
#include "XenonGraphics/Xeutils.h"

CXBSimpleFastCapProfile g_oXeProfileManager;

// This entire file compiles to nothing if PROFILE_ENABLE is not defined.
#ifdef PROFILE_ENABLE

extern WOR_tdst_World	*WOR_gpst_CurrentWorld;

// The command prefix is compatible with the DebugConsole sample
// so we can use DebugConsole to trigger profiles as well as using
// the TriggerProfile tool.
static const char* s_CommandPrefix = "XCMD";

// Static global flag to indicate whether profiling has been
// requested.
static BOOL s_FastCAPRequested = FALSE;


//-----------------------------------------------------------------------------
// Name: dbgtolower()
// Desc: Returns lowercase of char. Used by dbgstrnicmp
//-----------------------------------------------------------------------------
static inline CHAR dbgtolower( CHAR ch )
{
    if( ch >= 'A' && ch <= 'Z' )
        return ch - ( 'A' - 'a' );
    else
        return ch;
}




//-----------------------------------------------------------------------------
// Name: dbgstrnicmp()
// Desc: Case-insensitive string compare that avoids potential deadlocks from
// the CRT strnicmp function.
// Note that this function returns TRUE if the strings match, which is the
// reverse of the CRT strnicmp function.
//-----------------------------------------------------------------------------
static BOOL dbgstrnicmp( const CHAR* str1, const CHAR* str2, int n )
{
    while( ( dbgtolower( *str1 ) == dbgtolower( *str2 ) ) && *str1 && n > 0 )
    {
        --n;
        ++str1;
        ++str2;
    }

    return( n == 0 || dbgtolower( *str1 ) == dbgtolower( *str2 ) );
}




//-----------------------------------------------------------------------------
// Name: ProfileCommandHandler()
// Desc: Command handler to hook in to the Debug Manager command system.
// This function is called from the Debug Manager DPC (Deferred Procedure Call)
// which means there are many CRT and XTL functions that cannot be used.
// It also means you cannot set breakpoints in this function using VC++.
// This function just looks for the command StartFastCap and sets a flag when
// it sees it.
//-----------------------------------------------------------------------------
static HRESULT __stdcall ProfileCommandHandler( const CHAR* strCommand,
                                        CHAR* strResponse, DWORD dwResponseLen,
                                        PDM_CMDCONT pdmcc )
{
    // Skip over the command prefix and the exclamation mark
    strCommand += strlen( s_CommandPrefix ) + 1;

    // Check if this is the initial connect signal
    if( dbgstrnicmp ( strCommand, "__connect__", 11 ) )
    {
        // If so, respond that we're connected
        lstrcpynA( strResponse, "RemoteTrigger connected.", dwResponseLen );
        return XBDM_NOERR;
    }

    const char* command = "StartFastCap";
    const char* response = 0;
    if( dbgstrnicmp( strCommand, command, strlen( command ) ) )
    {
        if( DmIsFastCAPEnabled() == XBDM_FASTCAPENABLED )
        {
            s_FastCAPRequested = TRUE;
            response = "Begin profiling request received.";
        }
        else
        {
            // This happens when no source files were compiled with /fastcap
            response = "Begin profiling request received when fastcap profiling is not enabled.";
        }
    }

    // Copy the response to the output buffer.
    if( response )
    {
        OutputDebugStringA( response );
        OutputDebugStringA( "\n" );
        lstrcpynA( strResponse, response, dwResponseLen );
    }

    return XBDM_NOERR;
}




//-----------------------------------------------------------------------------
// Name: IsFastCapProfileRequested()
// Desc: Returns TRUE if a fastcap profile was requested, and resets the
// internal variable so it will return FALSE next time.
//-----------------------------------------------------------------------------
BOOL XBProfiling_IsFastCAPRequested()
{
    // Initialize the command processor if this hasn't been done yet.
    static BOOL s_Initialized = FALSE;
    if( !s_Initialized )
    {
        s_Initialized = TRUE;
        HRESULT hr = DmRegisterCommandProcessor( s_CommandPrefix, ProfileCommandHandler );
        if( FAILED(hr) )
            OutputDebugStringA( "Error registering command processor.\n" );
    }

    BOOL Result = s_FastCAPRequested;
    s_FastCAPRequested = FALSE;
    return Result;
}




//-----------------------------------------------------------------------------
// Name: GetLogFileName()
// Desc: Get the log file name and path for the CAP profiling.
//-----------------------------------------------------------------------------
void XBProfiling_GetProfilingFileName(char *_sz_ProfilingFileName)
{
	//SYSTEMTIME LocalSysTime;
	//GetLocalTime(&LocalSysTime);

	if(WOR_gpst_CurrentWorld)
	{
		char sz_ProfilinfDirName[260];

		//test for Profiling Dir
		static BOOL DirExist = FALSE;
		if(!DirExist)
		{
			GetProfilingDirName(sz_ProfilinfDirName);

			WIN32_FIND_DATA wfd;
			HANDLE hFind = FindFirstFile(sz_ProfilinfDirName, &wfd);
			if(hFind == INVALID_HANDLE_VALUE)
			{
				DirExist = XeCreateDirectory(sz_ProfilinfDirName);
			}
			else
			{
				DirExist = TRUE;

				// Close the find handle.
				FindClose( hFind );
			}
		}
		GetProfilingFileName(_sz_ProfilingFileName);
	}
	else
		strcpy(_sz_ProfilingFileName, XBPROFILING_DEFAULT_LOGFILENAME);
}




//-----------------------------------------------------------------------------
// Name: BeginProfiling()
// Desc: Start /fastcap profiling. You can specify how much memory to use, or
// you can accept the default and let the function adjust as needed if less
// memory is available. 
//     Call EndProfiling() or let the destructor call it at the end of the
// frame.
// note: PROFILE_ENABLE is defined in both XProfile and XProfile_Fast, 
// but the beginProfiling function test for the FastCap option so its do nothing
// in the XPROFILE(callcap) build configuration
//-----------------------------------------------------------------------------
void CXBSimpleFastCapProfile::BeginProfiling( DWORD amountOfMemory )
{
	//if not in continuous capture mode and don't need a dump, do nothing
	if(!mb_ContinuousCapture && !mb_DumpProfilingData)
	{
			return;
	}

    if ( DmIsFastCAPEnabled() != XBDM_FASTCAPENABLED )
    {
        OutputDebugStringA( "Fast cap not enabled on this build.\n" );
        //return;
    }

    HRESULT result = S_OK;
    for (;;)    // Loop until we successfully start profiling or completely fail
    {
        const DWORD decreaseAmount = 1024 * 1024;

		if(mb_DumpProfilingData)
		{
			XBProfiling_GetProfilingFileName(msz_FileName);
			result = DmStartProfiling( msz_FileName, amountOfMemory );
			OutputDebugStringA(msz_FileName);
			mb_DumpProfilingData = FALSE;
		}
		else
		{
			result = DmStartProfiling( XBPROFILING_DEFAULT_LOGFILENAME, amountOfMemory );
		}

		// 0x80070008 is the error return code for not enough memory - ERROR_NOT_ENOUGH_MEMORY
        if ( result == 0x80070008 && amountOfMemory > decreaseAmount )
        {
            // Try again with a smaller amount of memory.
            amountOfMemory -= decreaseAmount;
        }
        else
        {
            // Either we succeeded, failed due to something other than lack of memory,
            // or we have already tried with the minimum amount of memory.
            break;
        }
    }

    if ( result != XBDM_NOERR )
    {
        char    buffer[1000];
        sprintf( buffer, "DmStartProfiling failed with %08lx.\n", result );
        OutputDebugStringA( buffer );
    }
    else
    {
        // Record the amount of memory used so we can display it later.
        // Also, m_amountOfMemory indicates that profiling was successfully started
        m_amountOfMemory = amountOfMemory;
    }
}




//-----------------------------------------------------------------------------
// Name: EndProfiling()
// Desc: End /fastcap profiling. Then you can manually copy the resulting
// .cap file to your PC and use xbperfview, together with the .exe and .pdb
// files, to analyze your performance.
//-----------------------------------------------------------------------------
void CXBSimpleFastCapProfile::EndProfiling()
{
    // If we weren't doing profiling, just exit.
    if ( !m_amountOfMemory )
        return;

    DmStopProfiling();
    // You should check to see whether the file size is smaller than the
    // buffer size used. If it is the same size then that means that the
    // capture ran out of memory, and you should try to free more memory
    // for a more complete capture.
    OutputDebugStringA( "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
    char    buffer[1000];
    sprintf( buffer, "\t\tFinished profiling with a %d Mbyte buffer.\n", m_amountOfMemory / ( 1024 * 1024 ) );
    OutputDebugStringA( buffer );
    OutputDebugStringA( "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n" );

    // Make sure we don't stop profiling twice.
    m_amountOfMemory = 0;
}



//-----------------------------------------------------------------------------
// Name: SetProfilingOptions()
// Desc: Establish the operating options for collection of profiling data.

//there is the missing flagson the SDK Doc.
	/*
	E.g. to measure 
	instruction L1 misses on threads 0, 2, 4:

	flags =  DM_PROFILE_FLAG_CAPTURE_PERFCOUNTER  | DM_PROFILE_FLAG_HW_THREAD2 | 
	DM_PROFILE_FLAG_HW_THREAD4 | DM_PROFILE_FLAG_PMC(DM_PROFILE_PMC_IL1);

	(thread 0 is always measured)

	Let me know if you have problems getting this to work.


	// Flag for DmSetProfilingOptions
	#define DM_PROFILE_FLAG_CAPTURE_PERFCOUNTER  0x00000001L
	#define DM_PROFILE_FLAG_HW_THREAD1 (1<<1)
	#define DM_PROFILE_FLAG_HW_THREAD2 (1<<2)
	#define DM_PROFILE_FLAG_HW_THREAD3 (1<<3)
	#define DM_PROFILE_FLAG_HW_THREAD4 (1<<4)
	#define DM_PROFILE_FLAG_HW_THREAD5 (1<<5)

	typedef enum
	{
	DM_PROFILE_PMC_L2 = 0,

	DM_PROFILE_PMC_DL1,
	DM_PROFILE_PMC_IL1 ,
	DM_PROFILE_PMC_LHS,
	DM_PROFILE_PMC_DERAT,
	DM_PROFILE_PMC_IERAT,
	DM_PROFILE_PMC_BRANCH,

	DM_PROFILE_PMC_UCODE,
	DM_PROFILE_PMC_INSTR,

	DM_PROFILE_PMC_MISALIGNED,
	DM_PROFILE_PMC_STQ_FULL,

	DM_PROFILE_PMC_DIVSQRT,
	DM_PROFILE_PMC_DENORM,
	DM_PROFILE_PMC_FPU_NAN,
	DM_PROFILE_PMC_VMX_ABNORMAL,


	DM_PROFILE_PMC_LAST
	} eDM_PROFILE_PMC_SETUP;

	#define DM_PROFILE_PMC_SHIFT     6

	#define DM_PROFILE_PMC_MASK      ((0xF) << DM_PROFILE_PMC_SHIFT)
	#define DM_PROFILE_FLAG_PMC(x)    (((x) << DM_PROFILE_PMC_SHIFT) & 
	DM_PROFILE_PMC_MASK)
	*/
//-----------------------------------------------------------------------------
void CXBSimpleFastCapProfile::SetProfilingOptions( DWORD flags )
{
	HRESULT result = DmSetProfilingOptions(flags);

	if( result != XBDM_NOERR )
	{
		OutputDebugStringA( "Error while setting profiling options!" );
		return;
	}
}

#endif
