//------------------------------------------------------------------------------
// Filename   :DATCUtilsDiff.cpp
/// \author    YCharbonneau
/// \date      11/04/2005
/// \par       Description: Implementation of DATCUtilsDiff
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCUtils.h"
#include "DATCPerforce.h"
#include "DATCP4FileSysSync.h"

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------
struct ThreadData
{
	std::string		strFile1;
	std::string		strFile2;
	BIG_KEY			saveFileKey;
	HWND			hWndDlg;
};

struct ChildInfo
{
	HWND	hWnd;
	BOOL	bEnabled;
};

struct WndFreezer
{
	std::vector< ChildInfo >	lstChildren;
};

//------------------------------------------------------------------------------
// Module data
//------------------------------------------------------------------------------
static HANDLE g_hThread = NULL;
static DWORD g_dwThreadId = 0;
static bool g_bDiffInProgress = false;
extern "C" BOOL BIG_gb_IgnoreRecent;


//------------------------------------------------------------------------------
// Module functions
//------------------------------------------------------------------------------
//------------------------------------------------------------
//   BOOL CALLBACK DisableChildren( HWND hwnd, LPARAM lParam )
/// \author    FFerland
/// \date      11/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL CALLBACK DisableChildren( HWND hwnd, LPARAM lParam )
{
	WndFreezer* pWndFreezer = (WndFreezer*)lParam;

	ChildInfo info;
	info.hWnd = hwnd;
	info.bEnabled = ::IsWindowEnabled( hwnd );

	pWndFreezer->lstChildren.push_back( info );

	::EnableWindow( hwnd, FALSE );

	return TRUE;
}

//------------------------------------------------------------
//   DWORD WINAPI DiffThreadEntryFunc( LPVOID lpParameter )
/// \author    FFerland
/// \date      11/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
DWORD WINAPI DiffThreadEntryFunc( LPVOID lpParameter )
{
	g_bDiffInProgress = true;

	ThreadData data = *(ThreadData*)lpParameter;
	delete (ThreadData*)lpParameter;
	std::string saveFileName;

	WndFreezer wndFreezer;
	if( data.hWndDlg != NULL )
		EnumChildWindows( data.hWndDlg, DisableChildren, (LPARAM)&wndFreezer );

	// When we do a diff, we provide a 3rd file so that any edits will be saved in there
	// We must make sure that that file does not exist before we call BFObjectMerge
	if (data.saveFileKey != BIG_C_InvalidKey)
	{
		saveFileName = "c:\\bfMergeFile.tmp";
		if (!access(saveFileName.c_str(), 0))
			unlink(saveFileName.c_str());
	}

	char szCmdLine[ 512 ];
	sprintf( szCmdLine, "BFObjectMerge.exe %s %s %s", data.strFile2.c_str( ), data.strFile1.c_str( ), saveFileName.c_str() );

	PROCESS_INFORMATION pi;
	STARTUPINFO			si;
	BOOL				retVal = FALSE;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	if (!CreateProcess(NULL,
		szCmdLine,
		NULL,
		NULL,
		FALSE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&si,
		&pi))
	{
		::MessageBox( NULL, "Unable to run file comparison tool!", "Error", MB_OK | MB_ICONEXCLAMATION );
	}
	else
	{
		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		retVal = TRUE;
	}

	CloseHandle( g_hThread );
	g_hThread = NULL;
	g_dwThreadId = 0;

	DeleteFile( data.strFile1.c_str( ) );
	DeleteFile( data.strFile2.c_str( ) );

	if (data.saveFileKey != BIG_C_InvalidKey)
	{
		Error err;

		// TODO: ask if we want to use merged result
		CFile file;
		BOOL bOpen = file.Open( saveFileName.c_str( ), CFile::modeRead );
		if (bOpen && MessageBox(data.hWndDlg, "File has been modified. Do you want to keep your changes ?", "Please confirm", MB_YESNO| MB_DEFBUTTON2| MB_ICONQUESTION) == IDYES)
		{
			size_t len = (size_t)file.GetLength( );
			void* buff = malloc(len);

			file.Read( buff, len );
			file.Close( );

			char keyFileName[MAX_PATH];
			sprintf(keyFileName, "\\0x%08X", data.saveFileKey);
			DAT_CP4FileSysSync syncFile("");
			syncFile.Set(keyFileName);
			syncFile.Open( FOM_WRITE, &err );
			syncFile.Write((const char* )buff, len, &err);
			syncFile.Close( &err );

			free(buff);
		}
	}

	// reenable children
	std::vector< ChildInfo >::const_iterator i = wndFreezer.lstChildren.begin( );
	std::vector< ChildInfo >::const_iterator e = wndFreezer.lstChildren.end( );

	for( ; i != e; ++i )
	{
		const ChildInfo info = *i;
		::EnableWindow( info.hWnd, info.bEnabled );
	}

	g_bDiffInProgress = false;

	return retVal;
}

//------------------------------------------------------------
//   bool AreIdenticalFiles( const char* szFile1, const char* szFile2 )
/// \author    FFerland
/// \date      11/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
bool AreIdenticalFiles( const char* szFile1, const char* szFile2 )
{
	CFile file1, file2;

	if( !file1.Open( szFile1, CFile::modeRead ) )
		return false;

	if( !file2.Open( szFile2, CFile::modeRead ) )
		return false;

	// get file sizes
	ULONG dwLength1 = (ULONG )file1.GetLength( );
	ULONG dwLength2 = (ULONG )file2.GetLength( );

	if( dwLength1 != dwLength2 )
		return false;

	// create buffers
	char* pBuffer1 = new char[ dwLength1 ];
	char* pBuffer2 = new char[ dwLength2 ];

	// read
	file1.Read( pBuffer1, dwLength1 );
	file2.Read( pBuffer2, dwLength2 );

	if( memcmp( pBuffer1, pBuffer2, dwLength1 ) != 0 )
	{
		delete[] pBuffer1;
		delete[] pBuffer2;

		return false;
	}

	// cleanup
	delete[] pBuffer1;
	delete[] pBuffer2;

	return true;
}

//------------------------------------------------------------
//   void DAT_CUtils::RunDiff( const char* szFile1, const char* szFile2, BIG_KEY _ul_Key, HWND hWndDlg )
/// \author    FFerland
/// \date      11/04/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::RunDiff( const char* szFile1, const char* szFile2, BIG_KEY _ul_Key, HWND hWndDlg )
{
	if( g_bDiffInProgress )
	{
		MessageBox( NULL, "Another file comparison is in progress. Please close it and try again.", "Can't compare...", MB_OK | MB_ICONINFORMATION );
		return;
	}

	// compare files
	if( AreIdenticalFiles( szFile1, szFile2 ) )
	{
		AfxMessageBox( "Files are identical!" );
		return;
	}

	// start diff thread
	ThreadData* pThreadData = new ThreadData;
	pThreadData->strFile1 = szFile1;
	pThreadData->strFile2 = szFile2;
	pThreadData->saveFileKey = _ul_Key;

	pThreadData->hWndDlg  = hWndDlg;

	g_hThread = CreateThread( NULL, 0, DiffThreadEntryFunc, pThreadData, 0, &g_dwThreadId );
}



//------------------------------------------------------------
//   std::string DAT_CUtils::MakeTempFName( const char* szBaseName, const DAT_CP4ClientInfoHeader& info )
/// \author    FFerland
/// \date      2005-02-16
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
std::string DAT_CUtils::MakeTempFName( const char* szBaseName, const DAT_CP4ClientInfoHeader& info )
{
	char szShortName[ _MAX_FNAME ];
	sprintf( szShortName, "%s-%s", szBaseName, L_strrchr( info.aszBFFilename, '/' ) + 1 );

	char szPath[ _MAX_PATH + 1 ];
	GetTempPath( sizeof( szPath ), szPath );

	char szFilename[ _MAX_PATH + 1 ];
	_makepath( szFilename, NULL, szPath, szShortName, "tmp" );

	return szFilename;
}
//------------------------------------------------------------------------------

