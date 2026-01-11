#include "Precomp.h"
#ifdef ACTIVE_EDITORS

#include "DIAhistory_dlg.h"
#include "DATCP4View.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGexport.h"

BEGIN_MESSAGE_MAP(EDIA_cl_HistoryDialog, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(IDC_HISTORY_CLOSE, OnHistoryClose)
	ON_BN_CLICKED(IDC_HISTORY_DIFF_REVS, OnDiffRevs)
	ON_BN_CLICKED(IDC_HISTORY_DIFF_VS_CLIENT, OnDiffVsClient)
	ON_BN_CLICKED(IDC_HISTORY_VIEW, OnView)
	ON_BN_CLICKED(IDC_HISTORY_SYNC, OnSync)
	ON_BN_CLICKED(IDC_HISTORY_DESCRIBE, OnDescribe)
	ON_BN_CLICKED(IDC_HISTORY_RECOVER, OnRecover)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_HISTORY_LIST, OnLvnItemActivate)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// module data

bool g_bSynchedInHistory = false;

//////////////////////////////////////////////////////////////////////////
// module functions

bool SupportedFormat( const ULONG ulKey, const DAT_SHistoryInfo* pInfo )
{
	// all formats are now supported to some extent
	// non-native formats are simply displayed as encoded hex values
	return true;
}

void ReplaceFileNames( std::string& strText )
{
	std::string strNewText;
	size_t nCurOffset = 0;

	while( true )
	{
		size_t nEOLOffset = strText.find_first_of( '\n', nCurOffset );
		if( nEOLOffset != -1 )
			strText[ nEOLOffset ] = '\0';
		else
			break;

		static const regex::rpattern patternfiles("(.*)\\/\\/(.+)#(-?[0-9]+) (.*)");
		regex::match_results results;

		//const char* szText = strText.c_str( ) + nCurOffset;
		std::string szText = strText.c_str( ) + nCurOffset;
		if (patternfiles.match(szText, results).matched)
		{
			std::string strFilename = "//";
			strFilename += results.backref(2).str( );
			strFilename += "#";
			strFilename += results.backref(3).str( );

			std::string strRealFilename = strFilename;
			#if 0
			if( DAT_CPerforce::GetInstance( )->P4Connect( ) )
			{
				std::vector<DAT_CP4ClientInfoHeader*> vFileInfo;
				DAT_CPerforce::GetInstance( )->P4FileInfo( strFilename.c_str( ), &vFileInfo );

				if( !vFileInfo.empty( ) )
					strRealFilename = vFileInfo[0]->aszBFFilename;

				DAT_CPerforce::GetInstance( )->P4Disconnect( );

				for ( UINT ui = 0; ui < vFileInfo.size(); ui++ ) 
					delete vFileInfo[ui];
			}
			#endif

			strNewText += results.backref(1).str( );
			strNewText += strRealFilename;
			strNewText += " ";
			strNewText += results.backref(4).str( );
			strNewText += "\n";
		}
		else
		{
			strNewText += szText;
			strNewText += "\n";
		}

		if( nEOLOffset != -1 )
			strText[ nEOLOffset ] = '\n';

		nCurOffset = nEOLOffset + 1;
	}

	strText = strNewText;
}

size_t CountFileNames( const std::string& strText )
{
	size_t nCount = 0;

	const char* szFilename = strText.c_str( );
	while( szFilename )
	{
		szFilename = strstr( szFilename, "//" );

		if( szFilename )
		{
			++nCount;
			szFilename += 2;
		}
	}

	return nCount;
}

std::string MakeTempFName( const DAT_SHistoryInfo* pInfo )
{
	const ULONG ulRev = atol( pInfo->strRevision.c_str( ) );

	char szShortName[ _MAX_FNAME ];
	sprintf( szShortName, "ReadOnly-0-Rev-%d-%s", ulRev, L_strrchr( pInfo->strFilename.c_str( ), '/' ) + 1 );

	char szPath[ _MAX_PATH + 1 ];
	GetTempPath( sizeof( szPath ), szPath );

	char szFilename[ _MAX_PATH + 1 ];
	_makepath( szFilename, NULL, szPath, szShortName, "tmp" );

	return szFilename;
}

std::string MakeTempFName( const char* szBasename )
{
	char szPath[ _MAX_PATH + 1 ];
	GetTempPath( sizeof( szPath ), szPath );

	char szFilename[ _MAX_PATH + 1 ];
	_makepath( szFilename, NULL, szPath, szBasename, "tmp" );

	return szFilename;
}

void ConvertToXml( const char* szSource, const char* szDest )
{
	char szCmdLine[ 512 ];
	sprintf( szCmdLine, "BFObjectXmlConv_.exe %s %s", szSource, szDest );

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
		::MessageBox( NULL, "Unable to run XML conversion tool!", "Error", MB_OK | MB_ICONEXCLAMATION );
	}
	else
	{
		// Wait until child process exits.
		WaitForSingleObject( pi.hProcess, INFINITE );

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		retVal = TRUE;
	}
}

std::string FormatString( const std::string& strText, bool bSingleLine )
{
	regex::subst_results results;
	std::string strNewString = strText;

	if( bSingleLine )
	{
		const regex::rpattern revertPatternN("\n", " ");
		const regex::rpattern revertPatternR("\r", "");
		const regex::rpattern revertPatternT("\t", " ");

		while( revertPatternN.substitute(strNewString, results) );
		while( revertPatternR.substitute(strNewString, results) );
		while( revertPatternT.substitute(strNewString, results) );
	}
	else
	{
		const regex::rpattern revertPatternCRLF1("\n\t", "\r\n");
		while( revertPatternCRLF1.substitute(strNewString, results) );

		if( strNewString.length( ) > 0 )
		{
			if( strNewString[ strNewString.length( ) - 1 ] == 0x0a )
				strNewString.erase( strNewString.length( ) - 1 );
		}
	}

	return strNewString;
}

void BuildCompleteFileInfoList
	(
	const std::vector< DAT_SHistoryInfo* >&			lstEntries,
	const std::vector< DAT_CP4ClientInfoHeader* >&	vFileInfo,
	std::vector< DAT_CP4ClientInfoHeader* >&		vNewFileInfo
	)
{
	// go through entries
	std::vector< DAT_SHistoryInfo* >::const_iterator iEntries = lstEntries.begin( );
	std::vector< DAT_SHistoryInfo* >::const_iterator eEntries = lstEntries.end( );

	std::vector< DAT_CP4ClientInfoHeader* >::const_iterator iFileInfo = vFileInfo.begin( );
	std::vector< DAT_CP4ClientInfoHeader* >::const_iterator eFileInfo = vFileInfo.end( );

	for( ; iEntries != eEntries; ++iEntries )
	{
		DAT_SHistoryInfo* pEntry = *iEntries;

		DAT_CP4ClientInfoHeader* pInfoHeader = *iFileInfo;
		vNewFileInfo.push_back( pInfoHeader );

		if( pEntry->strAction != "delete" )
			++iFileInfo;
	}
}

//////////////////////////////////////////////////////////////////////////
//

EDIA_cl_HistoryDialog::EDIA_cl_HistoryDialog(ULONG ul_Key)
	: EDIA_cl_BaseDialog(IDD_FILE_HISTORY,M_MF()),
	  m_ulKey( ul_Key )
{
	g_bSynchedInHistory = false;
}

EDIA_cl_HistoryDialog::~EDIA_cl_HistoryDialog()
{
	// clean up
	while( !m_lstEntries.empty( ) )
	{
		delete m_lstEntries.back( );
		m_lstEntries.pop_back( );
	}
}

void EDIA_cl_HistoryDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HISTORY_LIST, m_ctlEntries);
	DDX_Control(pDX, IDC_HISTORY_ACTIONS_TEXT, m_ctlActionsText);
	DDX_Control(pDX, IDC_HISTORY_DESCRIPTION_TEXT, m_ctlDescriptionText);
	DDX_Control(pDX, IDC_HISTORY_FILETYPE_TEXT, m_ctlFileTypeText);
	DDX_Control(pDX, IDC_HISTORY_ACTIONS, m_ctlActionsEdit );
	DDX_Control(pDX, IDC_HISTORY_DESCRIPTION, m_ctlDescriptionEdit );

	DDX_Control(pDX, IDC_HISTORY_VIEW, m_btnView );
	DDX_Control(pDX, IDC_HISTORY_SYNC, m_btnSync );
	DDX_Control(pDX, IDC_HISTORY_DIFF_REVS, m_btnDiffRevs );
	DDX_Control(pDX, IDC_HISTORY_DIFF_VS_CLIENT, m_btnDiffVsClient );
	DDX_Control(pDX, IDC_HISTORY_DESCRIBE, m_btnDescribe );
	DDX_Control(pDX, IDC_HISTORY_RECOVER, m_btnRecover );
}

void EDIA_cl_HistoryDialog::OnHistoryClose(void)
{
	OnOK( );
}

void EDIA_cl_HistoryDialog::OnCancel( )
{
	if( GetDlgItem( IDC_HISTORY_CLOSE )->IsWindowEnabled( ) )
		CDialog::OnCancel( );
}

void EDIA_cl_HistoryDialog::OnView( void )
{
#if 0
	POSITION pos = m_ctlEntries.GetFirstSelectedItemPosition();
	if( pos )
	{
		const int nItem = m_ctlEntries.GetNextSelectedItem( pos );
		const DAT_SHistoryInfo* pInfo = (const DAT_SHistoryInfo*)m_ctlEntries.GetItemData( nItem );

		if( DAT_CPerforce::GetInstance( )->P4Connect( ) )
		{
			StrBuf data;
			const ULONG ulRev = atol( pInfo->strRevision.c_str( ) );

			DAT_CPerforce::GetInstance( )->P4Print( m_ulKey, ulRev, data, true );
			DAT_CPerforce::GetInstance( )->P4Disconnect( );

			CFile file;
			file.Open( "c:\\JadeTemp.buf", CFile::modeCreate | CFile::modeWrite );
			file.Write( data.Value( ), data.Length( ) );
			file.Flush( );
			file.Close( );

			data.Clear( );

			// run conv tool
			ConvertToXml( "c:\\JadeTemp.buf", "c:\\JadeTemp.xml" );
			unlink( "c:\\JadeTemp.buf" );

			// read result
			const BOOL bOpenOk = file.Open( "c:\\JadeTemp.xml", CFile::modeRead );

			if( !bOpenOk )
			{
				M_MF( )->MessageBox( "Could not convert file to XML", "Error", MB_OK | MB_ICONERROR );
				return;
			}

			ULONGLONG lSize = file.GetLength( );
			while( lSize > 0 )
			{
				char szData[ 1024 * 32 ];
				UINT nReadSize = file.Read( szData, sizeof( szData ) );
				lSize -= nReadSize;
				data.Append( szData, nReadSize );
			}

			file.Close( );
			unlink( "c:\\JadeTemp.xml" );

			EDIA_cl_P4ViewDialog o_DescribeDlg( "Viewing file", data.Value( ),FALSE );
			o_DescribeDlg.DoModal( );
		}
	}
#endif
}

void EDIA_cl_HistoryDialog::OnSync( void )
{
	g_bSynchedInHistory = true;

	POSITION pos = m_ctlEntries.GetFirstSelectedItemPosition();
	if ( pos ) 
	{
		// get file's index
		BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(m_ulKey);

		// get file info
		const int nItem = m_ctlEntries.GetNextSelectedItem( pos );
		const DAT_SHistoryInfo* pInfo = (const DAT_SHistoryInfo*)m_ctlEntries.GetItemData( nItem );

		// verify if another file already exists with that name
		std::string strDir, strFilename;
		const char* szSeparator = strrchr( pInfo->strFilename.c_str( ), '/' );
		strDir.assign( pInfo->strFilename.c_str( ), szSeparator );
		strFilename = szSeparator + 1;

		const BIG_INDEX ulFileWithSameNameIndex =
			BIG_ul_SearchFile( BIG_ul_SearchDir( strDir.c_str( ) ), strFilename.c_str( ) );

		if( ulFileWithSameNameIndex != BIG_C_InvalidIndex && ulFileWithSameNameIndex != ulIndex )
		{
			M_MF( )->MessageBox( "A file with that name already exists in the destination folder.", "Error", MB_OK | MB_ICONERROR );
			return;
		}
	}
}

void EDIA_cl_HistoryDialog::OnDiffRevs( void )
{
	StrBuf buffer[2];
	const DAT_SHistoryInfo* pInfo[2];
	int nNdx = 0;

	// read both versions
	POSITION pos = m_ctlEntries.GetFirstSelectedItemPosition( );
	while( pos && nNdx < 2 )
	{
		const int nItem = m_ctlEntries.GetNextSelectedItem( pos );
		pInfo[ nNdx ] = (const DAT_SHistoryInfo*)m_ctlEntries.GetItemData( nItem );

		ULONG ulRev = atol( pInfo[ nNdx ]->strRevision.c_str( ) );

		++nNdx;
	}

	// dump to file
	const std::string strFilename1 = MakeTempFName( pInfo[ 0 ] );
	const std::string strFilename2 = MakeTempFName( pInfo[ 1 ] );

	CFile file1( strFilename1.c_str( ), CFile::modeCreate | CFile::modeWrite );
	CFile file2( strFilename2.c_str( ), CFile::modeCreate | CFile::modeWrite );

	file1.Write( buffer[ 0 ].Value( ), buffer[ 0 ].Length( ) );
	file2.Write( buffer[ 1 ].Value( ), buffer[ 1 ].Length( ) );

	file1.Flush( ); file1.Close( );
	file2.Flush( ); file2.Close( );

	// call comparison tool
	//DAT_CUtils::RunDiff( strFilename1.c_str( ), strFilename2.c_str( ), BIG_C_InvalidKey, m_hWnd );
}

void EDIA_cl_HistoryDialog::OnDiffVsClient( void )
{
	const DAT_SHistoryInfo* pInfo = NULL;

	std::string strOutputFilename1;
	std::string strOutputFilename2;

	// extract requested revision from Perforce
	POSITION pos = m_ctlEntries.GetFirstSelectedItemPosition();
	if( pos )
	{
		const int nItem = m_ctlEntries.GetNextSelectedItem( pos );
		pInfo = (const DAT_SHistoryInfo*)m_ctlEntries.GetItemData( nItem );
		const ULONG ulRev = atol( pInfo->strRevision.c_str( ) );

		#if 0
		if( DAT_CPerforce::GetInstance()->P4Connect( ) )
		{
			StrBuf buffer;
			DAT_CPerforce::GetInstance()->P4Print( m_ulKey, ulRev, buffer, true );
			DAT_CPerforce::GetInstance()->P4Disconnect();

			strOutputFilename2 = MakeTempFName( pInfo );
			CFile file( strOutputFilename2.c_str( ), CFile::modeCreate | CFile::modeWrite );
			file.Write( buffer.Value( ), buffer.Length( ) );
			file.Flush( );
			file.Close( );
		}
		#endif
	}
	else
	{
		return;
	}

	// extract file currently in BF
	DWORD ulBufferSize = 0;
	BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(m_ulKey);

	char* pBfBuffer = DAT_CUtils::PrepareFileBuffer( ulIndex, ulBufferSize,TRUE );

	if( pBfBuffer )
	{
		const char* szFilenameBegin = L_strrchr( pInfo->strFilename.c_str( ), '/' );

		std::string strPath, strFilename;
		strPath.assign( pInfo->strFilename.c_str( ), szFilenameBegin );
		strFilename.assign( szFilenameBegin + 1, pInfo->strFilename.c_str( ) + pInfo->strFilename.length( ) );

		strOutputFilename1 = MakeTempFName( ( std::string( "CurBF_" ) + ( L_strrchr( pInfo->strFilename.c_str( ), '/' ) + 1 ) ).c_str( ) );

		CFile file( strOutputFilename1.c_str( ), CFile::modeCreate | CFile::modeWrite );
		file.Write( pBfBuffer, ulBufferSize );
		file.Flush( );
		file.Close( );

		delete[] pBfBuffer;
	}
	else
	{
		return;
	}

	// call comparison tool
	//DAT_CUtils::RunDiff( strOutputFilename2.c_str( ), strOutputFilename1.c_str( ), m_ulKey, m_hWnd );
}

void EDIA_cl_HistoryDialog::OnDescribe( void )
{
	// get changelist number
	POSITION pos = m_ctlEntries.GetFirstSelectedItemPosition( );
	const int nItem = m_ctlEntries.GetNextSelectedItem( pos );
	const DAT_SHistoryInfo* pInfo = (const DAT_SHistoryInfo*)m_ctlEntries.GetItemData( nItem );

	const ULONG ulChangelist = atoi( pInfo->strChangelist.c_str( ) );
}

void EDIA_cl_HistoryDialog::OnRecover( void )
{
}

BOOL EDIA_cl_HistoryDialog::OnInitDialog( )
{
	CDialog::OnInitDialog( );

	// setup list control
	m_ctlEntries.InsertColumn( 0, "Revision",    LVCFMT_LEFT,  60 );
	m_ctlEntries.InsertColumn( 1, "File Name",   LVCFMT_LEFT, 120 );
	m_ctlEntries.InsertColumn( 2, "Changelist",  LVCFMT_LEFT,  70 );
	m_ctlEntries.InsertColumn( 3, "Date",        LVCFMT_LEFT,  80 );
	m_ctlEntries.InsertColumn( 4, "User",        LVCFMT_LEFT,  80 );
	m_ctlEntries.InsertColumn( 5, "Action",      LVCFMT_LEFT,  60 );
	m_ctlEntries.InsertColumn( 6, "Description", LVCFMT_LEFT, 250 );

	// enable full row select
	DWORD dwStyle = m_ctlEntries.SendMessage( LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0 ); 
	dwStyle |= LVS_EX_FULLROWSELECT; 

	m_ctlEntries.SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle );

	// populate list control
	int nSuccess = 0;
#	if 0
	if ( DAT_CPerforce::GetInstance( )->P4Connect( ) ) 
	{
		nSuccess = DAT_CPerforce::GetInstance( )->P4History( m_ulKey, m_lstEntries );

		if( nSuccess && m_lstEntries.size() > 0)
		{
			DAT_SHistoryInfo* pInfo = m_lstEntries[ 0 ];
			char aszP4Path[MAX_PATH] = {0};
			DAT_CUtils::GetP4FileFromKey(m_ulKey,aszP4Path,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
			
			char aszP4PathWithRevisionRange[MAX_PATH] = {0};
			sprintf(aszP4PathWithRevisionRange,"%s#1,%s",aszP4Path,pInfo->strRevision.c_str());
			std::vector<DAT_CP4ClientInfoHeader*> vFileInfoList;
			nSuccess = DAT_CPerforce::GetInstance()->P4FileInfo(aszP4PathWithRevisionRange ,&vFileInfoList );

			std::vector<DAT_CP4ClientInfoHeader*> vCompleteFileInfo;
			BuildCompleteFileInfoList( m_lstEntries, vFileInfoList, vCompleteFileInfo );

			for( size_t i = 0; i < m_lstEntries.size( ); ++i )
			{
				pInfo = m_lstEntries[ i ];

				if( i < vCompleteFileInfo.size() ) 
					pInfo->strFilename = vCompleteFileInfo[i]->aszBFFilename;
			}

			// cleaning up the file stat info vector
			for ( UINT ui = 0; ui < vFileInfoList.size(); ui++ ) 
				delete vFileInfoList[ui];
		}

		DAT_CPerforce::GetInstance( )->P4Disconnect( );
	}
	#endif


	if( !nSuccess )
	{
		M_MF( )->MessageBox( "Could not get this file's revision history!", "Error", MB_OK | MB_ICONEXCLAMATION );
		OnCancel( );
		return FALSE;
	}


	for( size_t i = 0; i < m_lstEntries.size( ); ++i )
	{
		const DAT_SHistoryInfo* pInfo = m_lstEntries[ i ];

		const int nIndex = m_ctlEntries.InsertItem( i, pInfo->strRevision.c_str( ) );
		m_ctlEntries.SetItemData( nIndex, (DWORD_PTR)pInfo );
		m_ctlEntries.SetItem( i, 1, LVIF_TEXT, pInfo->strFilename.c_str(), 0, 0, 0, 0 );
		m_ctlEntries.SetItem( i, 2, LVIF_TEXT, pInfo->strChangelist.c_str( ), 0, 0, 0, 0 );
		m_ctlEntries.SetItem( i, 3, LVIF_TEXT, pInfo->strDate.c_str( ), 0, 0, 0, 0 );
		m_ctlEntries.SetItem( i, 4, LVIF_TEXT, pInfo->strUser.c_str( ), 0, 0, 0, 0 );
		m_ctlEntries.SetItem( i, 5, LVIF_TEXT, pInfo->strAction.c_str( ), 0, 0, 0, 0 );
		m_ctlEntries.SetItem( i, 6, LVIF_TEXT, FormatString( pInfo->strDescription, true ).c_str( ), 0, 0, 0, 0 );
	}

	m_ctlEntries.SetFocus( );
	m_ctlEntries.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );
	m_ctlEntries.SetItemState( 0, LVIS_FOCUSED, LVIS_FOCUSED );

	// display text
	char szNewText[ 256 ];
	CString strText;

	GetWindowText( strText );
	sprintf( szNewText, strText, m_lstEntries.empty( ) ? "" : m_lstEntries[0]->strFilename.c_str( ) );
	SetWindowText( szNewText );

	m_ctlActionsText.GetWindowText( m_strActionsText );
	m_ctlDescriptionText.GetWindowText( m_strDescriptionText );
	m_ctlFileTypeText.GetWindowText( m_strFileTypeText );

	UpdateText( m_lstEntries.empty( ) ? NULL : m_lstEntries[0] );

	return FALSE;
}

void EDIA_cl_HistoryDialog::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if( m_btnView.IsWindowEnabled( ) )
		OnView( );
}

BOOL EDIA_cl_HistoryDialog::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	CDialog::OnNotify( wParam, lParam, pResult );

	LPNMLISTVIEW pLVHdr = reinterpret_cast<LPNMLISTVIEW>(lParam);

	if( pLVHdr->hdr.code == LVN_ITEMCHANGED &&
		(pLVHdr->uChanged & LVIF_STATE) &&
		pLVHdr->iItem != -1 )
	{
		if( ( pLVHdr->uNewState & ( LVIS_FOCUSED | LVIS_SELECTED ) ) == ( LVIS_FOCUSED | LVIS_SELECTED ) )
		{
			const DAT_SHistoryInfo* pInfo = (const DAT_SHistoryInfo*)m_ctlEntries.GetItemData( pLVHdr->iItem );
			UpdateText( pInfo );
		}

		UpdateUI( );
	}

	return FALSE;
}

void EDIA_cl_HistoryDialog::UpdateText( const DAT_SHistoryInfo* pInfo )
{
	char szNewText[ 512 ];

	sprintf( szNewText, m_strActionsText, pInfo ? pInfo->strChangelist.c_str( ) : "" );
	m_ctlActionsText.SetWindowText( szNewText );

	sprintf( szNewText, m_strDescriptionText, pInfo ? pInfo->strChangelist.c_str( ) : "" );
	m_ctlDescriptionText.SetWindowText( szNewText );

	sprintf( szNewText, m_strFileTypeText, pInfo ? pInfo->strFileType.c_str( ) : "" );
	m_ctlFileTypeText.SetWindowText( szNewText );

	m_ctlActionsEdit.SetWindowText( pInfo ? pInfo->strAction.c_str( ) : "" );
	m_ctlDescriptionEdit.SetWindowText( pInfo ? FormatString( pInfo->strDescription, false ).c_str( ) : "" );
}

void EDIA_cl_HistoryDialog::UpdateUI( )
{
	// get selection item count
	const int nSelected = m_ctlEntries.GetSelectedCount( );

	// check selected items for deleted files and format
	bool bDeleted = false;
	bool bSupportedFormat = false;

	POSITION pos = m_ctlEntries.GetFirstSelectedItemPosition( );
	while( pos )
	{
		const int nItem = m_ctlEntries.GetNextSelectedItem( pos );
		const DAT_SHistoryInfo* pInfo = (const DAT_SHistoryInfo*)m_ctlEntries.GetItemData( nItem );
		if( strncmp( pInfo->strAction.c_str( ), "delete", 6 ) == 0 )
			bDeleted = true;
		if( SupportedFormat( m_ulKey, pInfo ) )
			bSupportedFormat = true;
	}

	// is top file deleted
	const bool bTopDeleted = strncmp( m_lstEntries[0]->strAction.c_str( ), "delete", 6 ) == 0;

	// enable/disable buttons based on selection
	if( nSelected == 1 )
	{
		m_btnView.EnableWindow( (bSupportedFormat && !bDeleted) ? TRUE : FALSE );
		m_btnSync.EnableWindow( TRUE );
		m_btnDiffRevs.EnableWindow( FALSE );
		m_btnDiffVsClient.EnableWindow( (bSupportedFormat && !bDeleted) ? TRUE : FALSE );
		m_btnDescribe.EnableWindow( TRUE );
		m_btnRecover.EnableWindow( (bTopDeleted && !bDeleted) ? TRUE : FALSE );
	}
	else if( nSelected == 2 )
	{
		m_btnView.EnableWindow( FALSE );
		m_btnSync.EnableWindow( FALSE );
		m_btnDiffRevs.EnableWindow( bDeleted ? FALSE : TRUE );
		m_btnDiffVsClient.EnableWindow( FALSE );
		m_btnDescribe.EnableWindow( FALSE );
		m_btnRecover.EnableWindow( FALSE );
	}
	else
	{
		m_btnView.EnableWindow( FALSE );
		m_btnSync.EnableWindow( FALSE );
		m_btnDiffRevs.EnableWindow( FALSE );
		m_btnDiffVsClient.EnableWindow( FALSE );
		m_btnDescribe.EnableWindow( FALSE );
		m_btnRecover.EnableWindow( FALSE );
	}
}

#endif // ACTIVE_EDITORS
