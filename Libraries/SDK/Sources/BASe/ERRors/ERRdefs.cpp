/*$T ERRdefs.cpp GC!1.41 09/14/99 12:30:22 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#include "BASe/BASlog.h"
#include "BASe/ERRors/ERRdefs.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "AIinterp/Sources/AI.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIdebug.h"

#include "BIGfiles/BIGfat.h"

#ifdef ACTIVE_EDITORS
#	include "EDImainframe.h"
#	include "LINKs/LINKmsg.h"
#endif

#include "../../Shared/MainSharedSystem.h"

/*$4
 ***************************************************************************************************
    GLOBAL VARS
 ***************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
_ERR_tdst_Exception *_ERR_gpst_LastException = NULL;
OBJ_tdst_GameObject *ERR_gpst_ContextGAO     = NULL;
char *ERR_gpsz_ContextString                 = NULL;
extern char EDI_az_LogFileName[ L_MAX_PATH ];
#endif
BOOL ERR_gb_Warning    = FALSE;
BOOL ERR_gb_WarningBox = TRUE;
BOOL ERR_gb_Log        = FALSE;

CHAR ERR_g_szFormatBuffer[ ERR_BUFSIZEMSG ];// Buffer Used by ERR_vFormatMessage

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Call when an assertion failed.

    In:     _b_Assert           TRUE if it's an assert, FALSE for an error. Can't be TRUE in FINAL
                                mode cause all assert are ignored.
            _psz_File           Source file where error occured.
            _i_Line             Source line where error occured.
            _psz_Expression     Expression to print.
            _psz_Text1          Other expression to print. Can be NULL.
            _psz_Text2          Other expression to print. Can be NULL.
            _b_Msg              TRUE to display a message box, FALSE to update log. 

    Out:    Returns TRUE to make a breakpoint in debug mode, FALSE else.
 ===================================================================================================
 */
#ifdef JADEFUSION
extern BOOL ENG_gb_SlashL;
extern BOOL LOA_gb_SpeedMode;
extern char ENG_gaz_SlashL[ 1024 ];
extern int ENG_gi_Map2;
#else
extern "C" BOOL ENG_gb_SlashL;
extern "C" BOOL LOA_gb_SpeedMode;
extern "C" char ENG_gaz_SlashL[ 1024 ];
extern "C" int ENG_gi_Map2;
#endif

/**
 * hogsy: A special version of _ERR_fnb_AssertFailed for handling script-related errors.
 */
bool ERR_ScriptAssertFailed( const char *filename, int line, const char *expression, const char *message )
{
	std::string tmp;
	AI_tdst_BreakPoint bp = {};
	AI_FillBreakPoint( &bp, -1 );

	char scriptPath[ BIG_C_MaxLenPath + 1 ] = "unknown";
	if ( bp.ul_File != BIG_C_InvalidIndex )
	{
		snprintf( scriptPath, sizeof( scriptPath ), "%s", BIG_NameFile( bp.ul_File ) );
	}

	OBJ_tdst_GameObject *object = AI_Mpst_GetCurrentObject();
	const char *objectName      = ( object != nullptr ) ? object->sz_Name : "unknown";
	tmp.append( "Object:\t" + std::string( objectName ) + "\n" );
	tmp.append( "Script:\t" + std::string( scriptPath ) + "\n" );
	tmp.append( "Line:\t" + std::to_string( bp.i_Line ) + "\n\n" );

	if ( message != nullptr )
	{
		tmp.append( std::string( message ) + "\n\n" );
	}

#if !defined( NDEBUG )
	if ( expression != nullptr )
	{
		tmp.append( std::string( filename ) + ":" + std::to_string( line ) + "\n" );
		tmp.append( std::string( expression ) + "\n\n" );
	}
#endif

	LINK_gul_ColorTxt = 0x000000FF;
	LINK_PrintStatusMsg( ( char * ) tmp.c_str() );
	LINK_gul_ColorTxt = 0;

	// Showin added this option so the user has more control over how errors appear.
	// By default we can have it be a bit less disruptive in the editor by having it input these errors to the console instead.
	// This still shows the error to the user without disrupting the game.
	if ( jaded::sys::launchOperations.editorMode && jaded::sys::launchOperations.popupError )
	{
		tmp.append( "\nClick OK to debug, Cancel to ignore." );

		if ( MessageBox( nullptr, tmp.c_str(), "Script Error", MB_OKCANCEL | MB_ICONWARNING | MB_SETFOREGROUND ) == IDOK )
		{
			L_longjmp( AI_gst_ContextCheck, 1 );
			return true;
		}
	}

	return false;
}

BOOL _ERR_fnb_AssertFailed(
        BOOL _b_Assert,
        const char *_psz_File,
        int _i_Line,
        const char *_psz_Expression,
        const char *_psz_Text1,
        const char *_psz_Text2,
        BOOL _b_Msg )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char *psz_Temp;
	char *psz_Title;
	BOOL b_DispMsg;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Allocate a temporary buffer and fill it with the text. */
	b_DispMsg = TRUE;
	psz_Temp  = NULL;
	psz_Temp  = ( char  *) L_malloc( 2048L );
	if ( psz_Temp == NULL )
	{
		psz_Temp = "XXX";
	}
	else
	{
		*psz_Temp = '\0';

		/* For an error, do not print expression in FINAL mode */
#ifndef NDEBUG
		if ( _b_Msg && _b_Assert && _psz_Expression != NULL )
		{
			L_strcat( psz_Temp, _psz_Expression );
		}
		else if ( !_b_Msg )
			L_strcat( psz_Temp, "[WARNING] " );

#endif /* NDEBUG */
		if ( _psz_Text1 )
		{
#ifndef NDEBUG
			if ( _b_Msg && _b_Assert && _psz_Expression != NULL )
			{
				L_strcat( psz_Temp, "\n" );
				L_strcat( psz_Temp, "\n" );
			}

#endif /* NDEBUG */
			L_strcat( psz_Temp, _psz_Text1 );
			if ( _psz_Text2 )
			{
				L_strcat( psz_Temp, " (" );
			}
		}

		if ( _psz_Text2 )
		{
			L_strcat( psz_Temp, _psz_Text2 );
			if ( _psz_Text1 )
			{
				L_strcat( psz_Temp, ")" );
			}
		}
	}

	/* Title. */
	if ( _b_Assert )
	{
		psz_Title = "Assertion failed (Press Shift + OK For A Breakpoint);";
	}
	else
	{
		psz_Title = "Error detected";
	}

	/* Log ? */
#ifdef ACTIVE_EDITORS
	if ( ERR_gb_Log )
	{
		FILE *f;
		f = fopen( EDI_az_LogFileName, "at" );
		if ( !f ) f = fopen( EDI_az_LogFileName, "wt" );
		fwrite( psz_Title, 1, strlen( psz_Title ), f );
		fwrite( "  --  ", 1, 6, f );
		fwrite( psz_Temp, 1, strlen( psz_Temp ), f );
		fwrite( "\n\r", 1, 2, f );
		fclose( f );
		b_DispMsg = FALSE;
	}
#endif

	/* Display error text. */
	if ( _b_Msg && b_DispMsg )
		jaded::sys::AlertBox( psz_Temp, psz_Title, jaded::sys::ALERT_BOX_ERROR );
#ifdef ACTIVE_EDITORS
	else if ( b_DispMsg )
	{
		LINK_gul_ColorTxt = 0x000000FF;
		LINK_PrintStatusMsg( psz_Temp );
		LINK_gul_ColorTxt = 0;
	}
#endif

#ifdef ACTIVE_EDITORS
	if ( _b_Assert )
	{
		extern void EDI_LogBug( char * );
		EDI_LogBug( psz_Temp );
	}
#endif

	/* Free temporary buffer. */
	if ( psz_Temp )
	{
		L_free( psz_Temp );
	}

	/* In engine mode, only assert in debug mode are not fatal. */
#if !defined( ACTIVE_EDITORS )
#	ifdef _DEBUG
	if ( ( _b_Assert == FALSE ) && _b_Msg )
	{
#	endif /* _DEBUG */
#	if !defined( _XBOX ) && !defined( _XENON )
		L_exit( -1 );
#	endif// _XBOX
#	ifdef _DEBUG
	}

#	endif /* _DEBUG */
#endif     /* !ACTIVE_EDITORS */

#if !defined( _XBOX ) && !defined( _XENON )
	/* If it was an assert, breakpoint is requested (only in debug mode). */
	if ( _b_Assert && ( GetAsyncKeyState( VK_SHIFT ) < 0 ) ) return TRUE;
#endif// _XBOX

	return FALSE;
}

//------------------------------------------------------------------------
//
// Author           Jean-Sebastien Pelletier (Stephane Girard)
// Date             30 Apr 1999
//
// Prototype        ERR_vFormatMessage
// Parameters       _szFormat : Printf format...
// Return Type      CHAR *
//
// Description      Format a string using the printf syntax, then put
//                  it in ERROR_g_szMessageBuffer and finally
//                  return it.
//
//------------------------------------------------------------------------
CHAR *ERR_szFormatMessage( CHAR *_szFormat, ... )
{
	va_list Marker;

	// Preconditions
	ERR_X_Assert( _szFormat != NULL );

	// Processing the Variable Arguments
	va_start( Marker, _szFormat );
	_vsnprintf( ERR_g_szFormatBuffer, ERR_BUFSIZEMSG - 1, _szFormat, Marker );
	va_end( Marker );
	ERR_g_szFormatBuffer[ ERR_BUFSIZEMSG - 1 ] = '\0';

	return ERR_g_szFormatBuffer;
}

#if defined( ACTIVE_EDITORS )

/*
 ===================================================================================================
 ===================================================================================================
 */
void ERR_LogPrint( const char *_s_Msg )
{
	if ( ERR_gb_Log )
	{
		FILE *f;
		f = fopen( EDI_az_LogFileName, "at" );
		if ( !f ) f = fopen( EDI_az_LogFileName, "wt" );
		fprintf( f, _s_Msg );
		fclose( f );
	}
}


/*
 ===================================================================================================
 ===================================================================================================
 */
void ERR_TestWarning( void )
{
#	if 0// unnecessary imo - let user check manually ~hogsy
    if(ERR_gb_Warning && ERR_gb_WarningBox)
    {
        if(M_MF()->MessageBox
            (
				"There's warning(s). Watch LOG window",
                "Warning(s) detected",
                MB_OKCANCEL | MB_ICONERROR
            ) != IDOK)
			ERR_gb_WarningBox = FALSE;
    }
#	endif

	ERR_gb_Warning = FALSE;
}

/*
 ===================================================================================================
    Aim:    Call for the last exception. Will reset some things in editor mode to restore as
            possible the application...
 ===================================================================================================
 */
void ERR_LastException( void )
{
	/* Just in case a mousecapture is currently running... */
	ReleaseCapture();

	/* To just in case a window is currently locked */
	if ( M_MF() )
	{
		for ( int i = 0; i < M_MF()->mi_NumLockDisplay; i++ )
		{
			if ( M_MF()->mapo_LockWnd[ i ] )
			{
				M_MF()->mapo_LockWnd[ i ]->SetRedraw( TRUE );
			}

			M_MF()->mai_LockCount[ i ] = 0;
			M_MF()->mapo_LockWnd[ i ]  = NULL;
		}

		M_MF()->mi_NumLockDisplay = 0;
	}

	EDI_gst_DragDrop.b_BeginDragDrop = FALSE;

	/* Reset internal error handling */
	_ERR_gpst_LastException = NULL;
}

#endif /* ACTIVE_EDITORS */

#if defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL ) || ( defined( _XENON ) && defined( _DEBUG ) )

// JFP: We got a shader compilation error that was much larger than 16384. Using 64k for now.
#	define XEERR_BUFSIZEMSG ( 65536 )

// ------------------------------------------------------------------------------------------------
// Name   : ERR_OutputDebugString
// Params : _sz_Format : Message format
// RetVal : None
// Descr. : Debug output
// ------------------------------------------------------------------------------------------------
void __stdcall ERR_OutputDebugString( CHAR *_sz_Format, ... )

{
	va_list Marker;
	CHAR Buffer[ XEERR_BUFSIZEMSG ];

	// Processing the Variable Arguments
	va_start( Marker, _sz_Format );
#	if defined( _XENON )
	vsprintf( Buffer, _sz_Format, Marker );
#	else
	_vsnprintf( Buffer, XEERR_BUFSIZEMSG, _sz_Format, Marker );
#	endif
	va_end( Marker );

	OutputDebugString( Buffer );
}

#endif// defined(ACTIVE_EDITORS) || (defined(_XENON) && defined(_DEBUG))

/*$4
 ***************************************************************************************************
    XBOX DEBUG
 ***************************************************************************************************
 */


#if defined( _XBOX ) || defined( _XENON )
#	if defined( __cplusplus ) && !defined( JADEFUSION )
extern "C"
{
#	endif

#	ifdef _DEBUG
	void xbERR_Print( char *sz_Log )
	{
		OutputDebugStringA( sz_Log );
	}
#	endif

#	if defined( __cplusplus ) && !defined( JADEFUSION )
}
#	endif
#endif
