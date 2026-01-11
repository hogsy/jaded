/*
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */
#include "Precomp.h"

#if defined(_XENON)

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/MEMory/MEM.h"
#include "INOut/INOsaving.h"
#include "INOut/Xenon/XeINOsaving.h"
#include "INOut/Xenon/XeINOjoystick.h"

#include "Xenon/Interf/BuiltInInterf.h"
#include "Xenon/Interf/MessageBar.h"
#include "Xenon/Profile/Profile.h"
#include "Xenon/MenuManager/MenuManager.h"

#define ENABLE_DEBUGGING 0
extern int AI_EvalFunc_IoButtonJustPressed_C(int _i_Button);
extern char* AI_EvalFunc_TEXTToString_C( TEXT_tdst_Eval* );
extern int g_bForceProfileRefresh;
extern bool g_bFoundNewDefaultDevice;

//////////////////////////////////////////////////////////////////////////
// type definitions

const DWORD XE_RAW_PROFILE_FILE_SIZE = INO_Cte_SavFileSize / INO_Cte_SavSlotNbMax;
const DWORD CLUSTER_SIZE			= 16 * 1024;
const DWORD HEADER_SIZE				= 40 * 1024;
const DWORD HASHBLOCK_SIZE			=  4 * 1024;
const DWORD SIG_BLOCK_SIZE			= 20 * 1024;

#define KK_SAVE_ROOT		"kksave"
#define KK_SLOT_FILE_NAME	"KKProfileSlot%d.sav"

enum xeINO_tden_SavCommand
{
	SavCmd_e_None,
	SavCmd_e_ReadAllHeaders,
	SavCmd_e_Read,
	SavCmd_e_Write,
	SavCmd_e_BootupTest
};

enum xeINO_tden_SavStatus
{
	SavStatus_e_Idle,
	SavStatus_e_Active,
	SavStatus_e_Terminating
};

#if ENABLE_DEBUGGING
enum xeINO_tden_SavForceFail
{
	SavFail_None,

	SavFail_Thumbnail,
	SavFail_WriteFile,
	SavFail_ReadFile,
	SavFail_ReadHeaders,
	SavFail_DeleteFile,

	SavFail_Max
};
#endif

struct xeINO_tdst_SavManager
{
	INO_tden_SavUserMessage		en_UserMessage;
	INO_tden_SavUserMessage		en_Result;
	xeINO_tden_SavCommand		en_UserCommand;
	xeINO_tden_SavStatus		en_Status;
	int							i_UserAction;

	BOOL						bConfirmOnSuccess;

	DWORD						dw_StartTime;
	HANDLE						h_WorkerThread;

	BOOL						bNewUser;

#if ENABLE_DEBUGGING
	DWORD						dw_ForceFail;
#endif
};

xeINO_tdst_SavManager	xeINO_gst_SavManager;

enum ReadResult
{
	READ_SUCCEEDED,
	READ_FAILED,
	READ_FILE_NOT_FOUND,
	READ_FILE_CORRUPT
};

enum WriteResult
{
	WRITE_SUCCEEDED,
	WRITE_ERROR_THUMBNAIL,
	WRITE_FAILED
};

class CAutoCleanup
{
public:
	CAutoCleanup( );
	~CAutoCleanup( );

	std::string		m_strRootName;
	HANDLE			m_hFile;
	void*			m_pMemoryArray;
};

CAutoCleanup::CAutoCleanup( )
	: m_hFile( INVALID_HANDLE_VALUE ),
	  m_pMemoryArray( NULL )
{
}

CAutoCleanup::~CAutoCleanup( )
{
	// free memory
	if( m_pMemoryArray )
		delete[] m_pMemoryArray;

	// close file
	if( m_hFile != INVALID_HANDLE_VALUE )
		CloseHandle( m_hFile );

	// unmount root
	if( !m_strRootName.empty( ) )
		XContentClose( m_strRootName.c_str( ), NULL );
}

/*
=======================================================================================================================
=======================================================================================================================
*/

DWORD xeINO_GetSavegameSize( )
{
	// minimum space required on MU for save to succeed in all cases, even though savegame size is actually much smaller
	return 196608;
}

bool xeINO_SavegameExists( int nSaveNumber )
{
	if( INO_gst_SavManager.i_CurrentSlot < 0 || INO_gst_SavManager.i_CurrentSlot > 4 )
		return false;

	// quick out if not signed-in
	if( XUserGetSigninState( g_XeProfile.GetUserIndex( ) ) == eXUserSigninState_NotSignedIn )
		return false;

	char szFilenameToFind[ 64 ];
	sprintf( szFilenameToFind, KK_SLOT_FILE_NAME, nSaveNumber + 1 );

	// create enumerator
	HANDLE hEnum = INVALID_HANDLE_VALUE;
	DWORD cbBuffer;
	XCONTENT_DATA contentData;

	DWORD dwRetVal = XContentCreateEnumerator
		(
		g_XeProfile.GetUserIndex( ),
		g_XeBuiltInInterf.GetSelectedDeviceID(),
		XCONTENTTYPE_SAVEDGAME,
		XCONTENTFLAG_NONE,
		1,
		&cbBuffer,
		&hEnum
		);

	// validate enumerator creation
	ERR_X_Assert( dwRetVal == ERROR_SUCCESS );
	ERR_X_Assert( cbBuffer == sizeof( contentData ) );
	if( dwRetVal != ERROR_SUCCESS )
		return false;

	// get data from enumerator
	dwRetVal = ERROR_SUCCESS;
	while( dwRetVal == ERROR_SUCCESS )
	{
		DWORD dwReturnCount = 0;
		dwRetVal = XEnumerate( hEnum, &contentData, sizeof( contentData ), &dwReturnCount, NULL );

		if( dwRetVal == ERROR_SUCCESS )
		{
			if( stricmp( szFilenameToFind, contentData.szFileName ) == 0 )
			{
				CloseHandle( hEnum );
				return true;
			}
		}
	};

	// close enumerator
	CloseHandle( hEnum );
	return false;
}

bool FindValidDevice( int& nDeviceFound )
{
	int nDevice = -1;

	HANDLE hEnum = INVALID_HANDLE_VALUE;
	DWORD cbBuffer;
	XCONTENT_DATA contentData;
	DWORD dwRetVal = 0;

	// quick out if not signed-in
	if( XUserGetSigninState( g_XeProfile.GetUserIndex( ) ) == eXUserSigninState_NotSignedIn )
		return false;

	// create enumerator
	dwRetVal = XContentCreateEnumerator
		(
		g_XeProfile.GetUserIndex( ),
		XCONTENTDEVICE_ANY,
		XCONTENTTYPE_SAVEDGAME,
		XCONTENTFLAG_NONE,
		1,
		&cbBuffer,
		&hEnum
		);

	// validate enumerator creation
	ERR_X_Assert( dwRetVal == ERROR_SUCCESS );
	ERR_X_Assert( cbBuffer == sizeof( contentData ) );
	if( dwRetVal != ERROR_SUCCESS )
		return false;

	// get data from enumerator
	dwRetVal = ERROR_SUCCESS;
	while( dwRetVal == ERROR_SUCCESS )
	{
		DWORD dwReturnCount = 0;
		dwRetVal = XEnumerate( hEnum, &contentData, sizeof( contentData ), &dwReturnCount, NULL );

		if( dwRetVal == ERROR_SUCCESS )
		{
			if( nDevice == -1 || (int)contentData.DeviceID < nDevice )
				nDevice = contentData.DeviceID;
		}	
	};

	// close enumerator
	CloseHandle( hEnum );

	// choose device sequentially if no savegame found
	if( nDevice == -1 )
	{
		for( int i = 1; i < 50; ++i )
		{
			if( XContentGetDeviceState( i, NULL ) == ERROR_SUCCESS )
			{
				nDevice = i;
				break;
			}
		}
	}

	// save device to output variable
	nDeviceFound = nDevice;

	return true;
}

bool xeINO_WriteThumbnail( const XCONTENT_DATA& contentData, CHAR* szThumbnailFile )
{
	CAutoCleanup autoCleanup;

	// Read the thumbnail image file
	HANDLE hFile = CreateFile( szThumbnailFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	autoCleanup.m_hFile = hFile;
	ERR_X_Assert( hFile != INVALID_HANDLE_VALUE );

	if( hFile == INVALID_HANDLE_VALUE )
		return false;

	DWORD dwThumbnailBytes = GetFileSize( hFile, NULL );

	BYTE* pThumbnailImage = new BYTE[ dwThumbnailBytes ];
	if( pThumbnailImage == NULL )
		return false;

	autoCleanup.m_pMemoryArray = pThumbnailImage;

	if( ReadFile( hFile, pThumbnailImage, dwThumbnailBytes, &dwThumbnailBytes, NULL) == 0 )
		return false;

#if ENABLE_DEBUGGING
	if( xeINO_gst_SavManager.dw_ForceFail == SavFail_Thumbnail )
		return false;
#endif

	if( XContentSetThumbnail( g_XeProfile.GetUserIndex(), &contentData, pThumbnailImage, dwThumbnailBytes, NULL ) != ERROR_SUCCESS )
		return false;

	return true;
}

WriteResult xeINO_WriteFile( int nSaveNumber )
{
	CAutoCleanup autoCleanup;

	// mount device
	XCONTENT_DATA contentData =
	{
		g_XeBuiltInInterf.GetSelectedDeviceID( ),
		XCONTENTTYPE_SAVEDGAME,
		L"",
		""
	};

	MultiByteToWideChar( CP_ACP, 0, INO_gst_SavManager.ast_SlotDesc[ nSaveNumber ].asz_Name, -1, contentData.szDisplayName, XCONTENT_MAX_DISPLAYNAME_LENGTH );
	sprintf( contentData.szFileName, KK_SLOT_FILE_NAME, nSaveNumber + 1 );

	DWORD dwRetVal = XContentCreate( g_XeProfile.GetUserIndex( ),
		KK_SAVE_ROOT, &contentData, XCONTENTFLAG_OPENALWAYS, NULL, NULL, NULL );

	ERR_X_Assert( dwRetVal == ERROR_SUCCESS );
	if( dwRetVal != ERROR_SUCCESS )
		return WRITE_FAILED;

	autoCleanup.m_strRootName = KK_SAVE_ROOT;

	// create savegame file
	HANDLE hFile = CreateFile( KK_SAVE_ROOT":\\KKProfile.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	autoCleanup.m_hFile = hFile;

	ERR_X_Assert( hFile != INVALID_HANDLE_VALUE );
	if( hFile == INVALID_HANDLE_VALUE )
		return WRITE_FAILED;

	// write savegame data
	const DWORD dwBytesToWrite = XE_RAW_PROFILE_FILE_SIZE;

	DWORD dwWritten;
	BOOL bWriteOk = WriteFile
		(
		hFile,
		INO_gst_SavManager.p_Temp + ( nSaveNumber * dwBytesToWrite ),
		dwBytesToWrite,
		&dwWritten,
		NULL
		);

	ERR_X_Assert( bWriteOk );
	ERR_X_Assert( dwWritten == dwBytesToWrite );

	CloseHandle( hFile );
	autoCleanup.m_hFile = INVALID_HANDLE_VALUE;

	if( !bWriteOk )
		return WRITE_FAILED;

	// write thumbnail
	bool bIconFailed = false;
	if( !xeINO_WriteThumbnail( contentData, "game:\\Images\\sg.png" ) )
		bIconFailed = true;

	// flush data
	dwRetVal = XContentFlush( KK_SAVE_ROOT, NULL );

	ERR_X_Assert( dwRetVal == ERROR_SUCCESS );
	if( dwRetVal != ERROR_SUCCESS )
		return WRITE_FAILED;

	// unmount device
	dwRetVal = XContentClose( KK_SAVE_ROOT, NULL );
	autoCleanup.m_strRootName = "";

	ERR_X_Assert( dwRetVal == ERROR_SUCCESS );
	if( dwRetVal != ERROR_SUCCESS )
		return WRITE_FAILED;

#if ENABLE_DEBUGGING
	if( xeINO_gst_SavManager.dw_ForceFail == SavFail_WriteFile )
		return WRITE_FAILED;
#endif

	// success
	if( bIconFailed )
		return WRITE_ERROR_THUMBNAIL;
	else
		return WRITE_SUCCEEDED;
}

ReadResult xeINO_ReadFile( int nSaveNumber, bool bSaveOwner )
{
#if ENABLE_DEBUGGING
	if( xeINO_gst_SavManager.dw_ForceFail == SavFail_ReadHeaders && nSaveNumber == 2 )
		return READ_FAILED;
#endif

	CAutoCleanup autoCleanup;

	// mount device
	XCONTENT_DATA contentData =
	{
		g_XeBuiltInInterf.GetSelectedDeviceID( ),
		XCONTENTTYPE_SAVEDGAME,
		L"",
		""
	};

	sprintf( contentData.szFileName, KK_SLOT_FILE_NAME, nSaveNumber + 1 );

	if( contentData.DeviceID == -1 )
		return READ_FILE_NOT_FOUND;

	DWORD dwRetVal = XContentCreate( g_XeProfile.GetUserIndex( ),
		KK_SAVE_ROOT, &contentData, XCONTENTFLAG_OPENEXISTING, NULL, NULL, NULL );

	ERR_X_Assert( dwRetVal == ERROR_SUCCESS || dwRetVal == ERROR_PATH_NOT_FOUND );

	if( dwRetVal == ERROR_PATH_NOT_FOUND )
		return READ_FILE_NOT_FOUND;
	if( dwRetVal == ERROR_FILE_CORRUPT )
		return READ_FILE_CORRUPT;

	if( dwRetVal != ERROR_SUCCESS )
		return READ_FAILED;

	autoCleanup.m_strRootName = KK_SAVE_ROOT;

	// create savegame file
	HANDLE hFile = CreateFile( KK_SAVE_ROOT":\\KKProfile.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	autoCleanup.m_hFile = hFile;

	if( hFile == INVALID_HANDLE_VALUE )
	{
		const DWORD dwLastError = GetLastError( );
		if( dwLastError == ERROR_FILE_NOT_FOUND )
		{
			return READ_FILE_NOT_FOUND;
		}
		else
		{
			ERR_X_Assert( false );
			return READ_FAILED;
		}
	}

	// read savegame data
	const DWORD dwBytesToRead = XE_RAW_PROFILE_FILE_SIZE;

	DWORD dwRead;
	BOOL bReadOk = ReadFile
		(
		hFile,
		INO_gst_SavManager.p_Temp + ( nSaveNumber * dwBytesToRead ),
		dwBytesToRead,
		&dwRead,
		NULL
		);
	ERR_X_Assert( bReadOk );
	ERR_X_Assert( dwRead == dwBytesToRead );

	CloseHandle( hFile );
	autoCleanup.m_hFile = INVALID_HANDLE_VALUE;

	if( !bReadOk )
		return READ_FAILED;

	if( dwRead != dwBytesToRead )
		return READ_FILE_CORRUPT;

	// remember content creator for validation when awarding achievements
	if( bSaveOwner )
	{
		XUID xuid;
        BOOL bUserIsCreator;
        dwRetVal = XContentGetCreator( g_XeProfile.GetUserIndex( ), &contentData, &bUserIsCreator, &xuid, NULL );

		if( dwRetVal != ERROR_SUCCESS )
			return READ_FAILED;

		g_XeProfile.SetGameOwner( bUserIsCreator != FALSE );
	}

	// unmount device
	dwRetVal = XContentClose( KK_SAVE_ROOT, NULL );
	autoCleanup.m_strRootName = "";

	ERR_X_Assert( dwRetVal == ERROR_SUCCESS );
	if( dwRetVal != ERROR_SUCCESS )
		return READ_FAILED;

#if ENABLE_DEBUGGING
	if( xeINO_gst_SavManager.dw_ForceFail == SavFail_ReadFile )
		return READ_FAILED;
#endif

	// success
	return READ_SUCCEEDED;
}

bool xeINO_DeleteFile( int nSaveNumber )
{
	XCONTENT_DATA contentData =
	{
		g_XeBuiltInInterf.GetSelectedDeviceID( ),
		XCONTENTTYPE_SAVEDGAME,
		L"",
		""
	};

	sprintf( contentData.szFileName, KK_SLOT_FILE_NAME, nSaveNumber + 1 );

	if( contentData.DeviceID == -1 )
		return false;

	// delete file
	DWORD dwRetVal = XContentDelete( g_XeProfile.GetUserIndex( ), &contentData, NULL );

	if( dwRetVal != ERROR_SUCCESS )
	{
		ERR_X_Assert( false );
		return false;
	}

#if ENABLE_DEBUGGING
	if( xeINO_gst_SavManager.dw_ForceFail == SavFail_DeleteFile )
		return false;
#endif

	return true;
}

DWORD thread_WriteFile(void* pData)
{
	// error by default
	xeINO_gst_SavManager.en_Result = INO_e_SavMsg_ErrorWhileSaving;

	// quick out if not signed-in
	if( XUserGetSigninState( g_XeProfile.GetUserIndex( ) ) == eXUserSigninState_NotSignedIn )
		return FALSE;

	// copy the header
	L_memcpy
		(
		INO_gst_SavManager.p_Temp + INO_gst_SavManager.i_CurrentSlot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
		&INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot],
		INO_Cte_SavHeaderSize
		);

	// copy the data
	L_memcpy
		(
		INO_gst_SavManager.p_Temp + INO_gst_SavManager.i_CurrentSlot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize) + INO_Cte_SavHeaderSize,
		INO_gst_SavManager.ac_CurrSlotBuffer,
		INO_Cte_SavOneSlotMaxSize
		);

	// write to disk
	WriteResult writeResult = WRITE_SUCCEEDED;

	if( INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot].i_Map == 0 )
	{
		if( !xeINO_DeleteFile( INO_gst_SavManager.i_CurrentSlot ) )
		{
			ERR_X_Assert( false );
			return FALSE;
		}
	}
	else
	{
		bool bFileExisted = xeINO_SavegameExists( INO_gst_SavManager.i_CurrentSlot );

		writeResult = xeINO_WriteFile( INO_gst_SavManager.i_CurrentSlot );

		if( !bFileExisted && ( writeResult == WRITE_SUCCEEDED || writeResult == WRITE_ERROR_THUMBNAIL ) )
		{
			// for some reason, the first time a game is saved, it is around 96KB, whereas the second time,
			// it grows to 128KB (sizes seen in Dashboard).  To prevent problems, save new games twice right away
			// so that it takes as much space as it's ever going to need.
			writeResult = xeINO_WriteFile( INO_gst_SavManager.i_CurrentSlot );
		}
	}

	switch( writeResult )
	{
		case WRITE_SUCCEEDED:
			xeINO_gst_SavManager.en_Result = INO_e_SavMsg_SuccessWhileSaving;
			return TRUE;

		case WRITE_ERROR_THUMBNAIL:
			xeINO_gst_SavManager.en_Result = INO_e_SavMsg_SuccessSavingFailIcon;
			return TRUE;

		default:
			ERR_X_Assert( false );
			return FALSE;
	}
}

DWORD thread_ReadFile(void* pData)
{
	// error by default
	xeINO_gst_SavManager.en_Result = INO_e_SavMsg_ErrorWhileLoading;

	// quick out if not signed-in
	if( XUserGetSigninState( g_XeProfile.GetUserIndex( ) ) == eXUserSigninState_NotSignedIn )
		return FALSE;

	// read file content
	const DWORD dwResult = xeINO_ReadFile( INO_gst_SavManager.i_CurrentSlot, true );

	if( dwResult != READ_SUCCEEDED )
	{
		ERR_X_Assert( false );
		return FALSE;
	}

	// copy the header
	L_memcpy
		(
		&INO_gst_SavManager.ast_SlotDesc[INO_gst_SavManager.i_CurrentSlot],
		INO_gst_SavManager.p_Temp + INO_gst_SavManager.i_CurrentSlot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
		INO_Cte_SavHeaderSize
		);

	// the data
	L_memcpy
		(
		INO_gst_SavManager.ac_CurrSlotBuffer,
		INO_gst_SavManager.p_Temp + INO_gst_SavManager.i_CurrentSlot * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize) + INO_Cte_SavHeaderSize,
		INO_Cte_SavOneSlotMaxSize
		);

	xeINO_gst_SavManager.en_Result = INO_e_SavMsg_SuccessWhileLoading;

	return TRUE;
}

DWORD thread_ReadHeaders(void* pData)
{
	// error by default
	xeINO_gst_SavManager.en_Result = INO_e_SavMsg_NoSaving;

	// clear data
	for(int i = 0; i < INO_Cte_SavSlotNbMax; ++i)
		L_memset( &INO_gst_SavManager.ast_SlotDesc[i], 0, INO_Cte_SavHeaderSize );

	g_XeProfile.SetGameOwner( false );

	// quick out if not signed-in
	if( XUserGetSigninState( g_XeProfile.GetUserIndex( ) ) == eXUserSigninState_NotSignedIn )
	{
		xeINO_gst_SavManager.en_Result = INO_e_SavMsg_SuccessWhileLoading;
		return TRUE;
	}

	// look for default device if required
	if( xeINO_gst_SavManager.bNewUser )
	{
		int nDevice = -1;
		if( FindValidDevice( nDevice ) )
		{
			// assign default device
			g_XeBuiltInInterf.SetSelectedDeviceID( nDevice );
			g_bFoundNewDefaultDevice = true;
		}
	}

	xeINO_gst_SavManager.bNewUser = FALSE;

	// read file
	bool lstCorrupt[ INO_Cte_SavSlotNbMax ];

	bool bAtLeastOneFailed = false;
	for( int nSavegameNb = 0; nSavegameNb < INO_Cte_SavSlotNbMax; ++nSavegameNb )
	{
		lstCorrupt[ nSavegameNb ] = false;

		const DWORD dwResult = xeINO_ReadFile( nSavegameNb, false );

		if( dwResult == READ_FAILED )
		{
			// error reading file, flag but continue reading other files
			bAtLeastOneFailed = true;
		}
		else if( dwResult == READ_FILE_CORRUPT )
		{
			// error reading file, flag but continue reading other files
			bAtLeastOneFailed = true;
			lstCorrupt[ nSavegameNb ] = true;
		}
	}

	// extract headers
	for(i = 0; i < INO_Cte_SavSlotNbMax; ++i)
	{
		L_memcpy
			(
			&INO_gst_SavManager.ast_SlotDesc[i],
			INO_gst_SavManager.p_Temp + i * (INO_Cte_SavHeaderSize + INO_Cte_SavOneSlotMaxSize),
			INO_Cte_SavHeaderSize
			);

		if( lstCorrupt[i] == true )
		{
			TEXT_tdst_Eval text;
				text.i_FileKey = 0x35001D04;
				text.i_Id = 0x350030D2;

			const char* szCorrupted = AI_EvalFunc_TEXTToString_C( &text );

			L_strcpy( INO_gst_SavManager.ast_SlotDesc[i].asz_Name, szCorrupted );
			INO_gst_SavManager.ast_SlotDesc[i].i_Slot = i;
			INO_gst_SavManager.ast_SlotDesc[i].i_Map = 101;
			INO_gst_SavManager.ast_SlotDesc[i].i_WP = 1;
			INO_gst_SavManager.ast_SlotDesc[i].i_Time = 1;
		}
	}

	if( bAtLeastOneFailed )
		return FALSE;
	else
		xeINO_gst_SavManager.en_Result = INO_e_SavMsg_SuccessWhileLoading;

	return TRUE;
}

DWORD thread_BootupTest(void* pData)
{
	xeINO_gst_SavManager.en_Result = INO_e_SavMsg_SuccessWhileLoading;

	int nDevice = -1;
	if( !FindValidDevice( nDevice ) )
		return FALSE;

	// assign default device
	g_XeBuiltInInterf.SetSelectedDeviceID( nDevice );

	return TRUE;
}

void ProcessCommand( LPTHREAD_START_ROUTINE threadFunction, int nMinDisplayTime )
{
	if( xeINO_gst_SavManager.h_WorkerThread == NULL )
	{
		// start operation
		xeINO_gst_SavManager.dw_StartTime = GetTickCount( );
		xeINO_gst_SavManager.en_Status = SavStatus_e_Active;
		xeINO_gst_SavManager.h_WorkerThread = CreateThread
			(
			NULL,
			0,
			threadFunction,
			NULL,
			0,
			NULL
			);
	}
	else
	{
		// check for completion
		const bool bThreadCompleted =
			WaitForSingleObject( xeINO_gst_SavManager.h_WorkerThread, 0 ) == WAIT_OBJECT_0;

		const bool bMinTimeElapsed =
			int( GetTickCount( ) - xeINO_gst_SavManager.dw_StartTime ) >= nMinDisplayTime;

		if( bThreadCompleted && bMinTimeElapsed )
		{
			// reset system and clean up
			xeINO_gst_SavManager.dw_StartTime = 0;

			CloseHandle( xeINO_gst_SavManager.h_WorkerThread );
			xeINO_gst_SavManager.h_WorkerThread = NULL;

			// communicate result to AI
			xeINO_gst_SavManager.en_Status = SavStatus_e_Terminating;
			xeINO_gst_SavManager.en_UserMessage = xeINO_gst_SavManager.en_Result;
			xeINO_gst_SavManager.en_Result = INO_e_SavMsg_None;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xeINO_SavModuleClose(void)
{
	if( g_MenuManager.IsInDemoMode_NoStorageAccess() )
		return;

	// do close here
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xeINO_SavModuleInit(void)
{
	// initialize save manager
	L_memset( &xeINO_gst_SavManager, 0, sizeof( xeINO_tdst_SavManager ) );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void xeINO_SavUpdate(void)
{
#if ENABLE_DEBUGGING
	if( AI_EvalFunc_IoButtonJustPressed_C( eXeButton_Trigger_Left ) )
	{
		xeINO_gst_SavManager.dw_ForceFail++;

		if( xeINO_gst_SavManager.dw_ForceFail == SavFail_Max )
			xeINO_gst_SavManager.dw_ForceFail = SavFail_None;

		const char* szText = NULL;
		switch( xeINO_gst_SavManager.dw_ForceFail )
		{
			case SavFail_None:
				szText = "Do not fail";
				break;

			case SavFail_Thumbnail:
				szText = "Fail on thumbnail";
				break;

			case SavFail_WriteFile:
				szText = "Fail on write file";
				break;

			case SavFail_ReadFile:
				szText = "Fail on read file";
				break;

			case SavFail_ReadHeaders:
				szText = "Fail on read headers";
				break;

			case SavFail_DeleteFile:
				szText = "Fail on delete file";
				break;

			default:
				szText = "Fail on unhandled condition";
				break;
		}

		CXeMessageBarManager::Get( ).AddMessage( 500, szText );
	}
#endif

	// quick exit if nothing to do
	if( xeINO_gst_SavManager.en_UserCommand == SavCmd_e_None )
		return;

	// no storage access for demo
	if( g_MenuManager.IsInDemoMode_NoStorageAccess( ) )
		return;

	// branch according to current state
	if( xeINO_gst_SavManager.en_Status != SavStatus_e_Terminating )
	{
		switch( xeINO_gst_SavManager.en_UserCommand )
		{
			case SavCmd_e_ReadAllHeaders:
			{
				ProcessCommand( (LPTHREAD_START_ROUTINE)thread_ReadHeaders, 3000 );
				break;
			}

			case SavCmd_e_Write:
			{
				ProcessCommand( (LPTHREAD_START_ROUTINE)thread_WriteFile, 3000 );
				break;
			}

			case SavCmd_e_Read:
			{
				ProcessCommand( (LPTHREAD_START_ROUTINE)thread_ReadFile, 3000 );
				break;
			}

			case SavCmd_e_BootupTest:
			{
				ProcessCommand( (LPTHREAD_START_ROUTINE)thread_BootupTest, 2000 );
				break;
			}

			default:
			{
				ERR_X_Assert( false );
				break;
			}
		}
	}

	// process result
	switch( xeINO_gst_SavManager.en_UserMessage )
	{
		case INO_e_SavMsg_Accessing:
		{
			xeINO_gst_SavManager.en_Status = SavStatus_e_Active;
			break;
		}

		case INO_e_SavMsg_ErrorWhileSaving:
		{
			// user action is inverted on purpose (Y is A and A is Y)!
			if( AI_EvalFunc_IoButtonJustPressed_C( eXeButton_Y ) )
			{
				xeINO_gst_SavManager.i_UserAction = eXeButton_A;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
				g_bForceProfileRefresh = 1;
			}
			else if( AI_EvalFunc_IoButtonJustPressed_C( eXeButton_A ) )
			{
				xeINO_gst_SavManager.i_UserAction = eXeButton_Y;
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Retrying;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
			}
			break;
		}

		case INO_e_SavMsg_ErrorWhileLoading:
		{
			// user action is inverted on purpose (Y is A and A is Y)!
			if( AI_EvalFunc_IoButtonJustPressed_C( eXeButton_Y ) )
			{
				xeINO_gst_SavManager.i_UserAction = eXeButton_A;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
			}
			else if( AI_EvalFunc_IoButtonJustPressed_C( eXeButton_A ) )
			{
				xeINO_gst_SavManager.i_UserAction = eXeButton_Y;
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Retrying;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
			}
			break;
		}

		case INO_e_SavMsg_SuccessWhileSaving:
		case INO_e_SavMsg_SuccessSavingFailIcon:
		{
			if( AI_EvalFunc_IoButtonJustPressed_C( eXeButton_A ) )
			{
				xeINO_gst_SavManager.i_UserAction = eXeButton_A;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
			}
			break;
		}

		case INO_e_SavMsg_SuccessWhileLoading:
		{
			if( xeINO_gst_SavManager.bConfirmOnSuccess )
			{
				if( AI_EvalFunc_IoButtonJustPressed_C( eXeButton_A ) )
				{
					xeINO_gst_SavManager.i_UserAction = eXeButton_A;
					xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
				}
			}
			else
			{
				xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
			}
			break;
		}

		case INO_e_SavMsg_NoSaving:
		{
			// user action is inverted on purpose (Y is A and A is Y)!
			if( AI_EvalFunc_IoButtonJustPressed_C( eXeButton_Y ) )
			{
				xeINO_gst_SavManager.i_UserAction = eXeButton_A;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
			}
			else if( AI_EvalFunc_IoButtonJustPressed_C( eXeButton_A ) )
			{
				xeINO_gst_SavManager.i_UserAction = eXeButton_Y;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Retrying;
			}
			break;
		}

		default:
			ERR_X_Assert( false );
	}

	// terminate command when work is all done
	if( xeINO_gst_SavManager.en_Status == SavStatus_e_Idle )
		xeINO_gst_SavManager.en_UserCommand = SavCmd_e_None;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int xeINO_SavSeti(int rq, int val)
{
	int i;
	INO_tden_SavRequestId request = (INO_tden_SavRequestId)rq;

	switch( request )
	{
		case INO_e_SavRq_ClearMessageId:
			xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_None;
			xeINO_gst_SavManager.i_UserAction = -1;
			break;

		case INO_e_SavRq_FreeRessource:
			xeINO_gst_SavManager.i_UserAction = -1;
			break;

		case INO_e_SavRq_ReadAllHeaders:
			xeINO_gst_SavManager.i_UserAction = -1;
			if( g_MenuManager.IsInDemoMode_NoStorageAccess() )
			{
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileLoading;
				xeINO_gst_SavManager.en_UserCommand = SavCmd_e_None;
			}
			else
			{
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
				xeINO_gst_SavManager.en_UserCommand = SavCmd_e_ReadAllHeaders;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Active;
				xeINO_gst_SavManager.bConfirmOnSuccess = FALSE;
			}
			break;

		case INO_e_SavRq_WriteOneProfile:
			xeINO_gst_SavManager.i_UserAction = -1;
			if( g_MenuManager.IsInDemoMode_NoStorageAccess() )
			{
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileSaving;
				xeINO_gst_SavManager.en_UserCommand = SavCmd_e_None;
			}
			else
			{
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
				xeINO_gst_SavManager.en_UserCommand = SavCmd_e_Write;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Active;
				xeINO_gst_SavManager.bConfirmOnSuccess = TRUE;
			}
			break;

		case INO_e_SavRq_ReadOneProfile:
			xeINO_gst_SavManager.i_UserAction = -1;
			if( g_MenuManager.IsInDemoMode_NoStorageAccess() )
			{
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileLoading;
				xeINO_gst_SavManager.en_UserCommand = SavCmd_e_None;
			}
			else
			{
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
				xeINO_gst_SavManager.en_UserCommand = SavCmd_e_Read;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Active;
				xeINO_gst_SavManager.bConfirmOnSuccess = TRUE;
			}
			break;

		case INO_e_SavRq_BootupTest:
			xeINO_gst_SavManager.i_UserAction = -1;
			if( g_MenuManager.IsInDemoMode_NoStorageAccess() )
			{
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_SuccessWhileLoading;
				xeINO_gst_SavManager.en_UserCommand = SavCmd_e_None;
			}
			else
			{
				xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_Accessing;
				xeINO_gst_SavManager.en_UserCommand = SavCmd_e_BootupTest;
				xeINO_gst_SavManager.en_Status = SavStatus_e_Active;
				xeINO_gst_SavManager.bConfirmOnSuccess = FALSE;
			}
			break;

		case INO_e_SavRq_TestMemCard:
			xeINO_gst_SavManager.en_UserMessage = INO_e_SavMsg_None;
			xeINO_gst_SavManager.en_Status = SavStatus_e_Idle;
			break;

		case INO_e_SavRq_UserMessageId:
			return xeINO_gst_SavManager.en_UserMessage;

		case INO_e_SavRq_Status:
			return ( xeINO_gst_SavManager.en_Status == SavStatus_e_Idle ) ? 0 : 1;

		case INO_e_SavRq_UserAction:
			i = xeINO_gst_SavManager.i_UserAction;
			xeINO_gst_SavManager.i_UserAction = -1;
			return i;

		case INO_e_SavRq_SetRetryFlag:
			return 0;

		case INO_e_SavRq_SetAsProfileOwner:
			g_XeProfile.SetGameOwner( true );
			return 0;

		default:
			ERR_X_Assert( false );
			return -1;
	}

    return 0;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void xeINO_FlagNewUser( )
{
	xeINO_gst_SavManager.bNewUser = TRUE;
}

#endif // defined(_XENON)
