//------------------------------------------------------------------------------
// Filename   :DATCPerforce.cpp
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: Implementation of DATCPerforce
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCPerforce.h"

//------------------------------------------------------------------------------
// client users
#include "DATCP4ClientUserFlush.h"

// Bigfile support
#include "BIGfiles\BIGfat.h"
#include "BIGfiles\BIGmdfy_file.h"
#include "BIGfiles\BIGmerge.h"
#include "BIGfiles\BIGopen.h"
//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   int DAT_CPerforce::P4ValidateRevision()
/// \author    NBeaufils
/// \date      19-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::P4FlushBF(const char* _psz_P4Root)
{
	// Get all files from P4
	P4FStatBF(_psz_P4Root);	

	// Set the key's client revision stored in Perforce if different from one stored in BF.
	UINT iP4MaxFile = 0;
	DAT_CP4ClientUserFlush ClientUserFlush;

	// If the BF is empty, flush using ...#0 (optimization)
	if ( BIG_MaxFile() == 0 )
	{
		// ------------------------------------------------------
		// Number of files to be flushed
		ClientUserFlush.SetNumberFilesToHandle(GetP4FilesNotInBF().size());

		// ------------------------------------------------------
		// For each P4 directories, we get the keys inside. This
		// we don't bust the limit (maxresults) of perforce
		char** ppsz_P4DirPathAndRevision = new char*[P4_DIR_TOTAL];
		std::vector<char*>& vcP4Directories = GetP4Directories(_psz_P4Root, "#0");
		UINT uiArgCount =0;

		for ( UINT iP4Directories = 0 ; iP4Directories < P4_DIR_TOTAL ; iP4Directories++ )
		{

			ppsz_P4DirPathAndRevision[uiArgCount] = vcP4Directories[iP4Directories];
			uiArgCount ++;

			if ( uiArgCount % P4_MAX_DIR_LIMIT == 0)
			{
				m_Client.SetArgv( uiArgCount, ppsz_P4DirPathAndRevision );
				m_Client.Run( P4_FLUSH, &ClientUserFlush );
				uiArgCount = 0;
			}
		}
		
		if ( uiArgCount > 0 ) 
		{
			m_Client.SetArgv( uiArgCount, ppsz_P4DirPathAndRevision);
			m_Client.Run( P4_FLUSH, &ClientUserFlush);
		}

		delete [] ppsz_P4DirPathAndRevision ;

		return 0 ;
	}

	// Go through every key stored in BF to set the client revision on Perforce
	// to the one stored in the BF
	char** ppsz_P4FilePathAndRevision = new char*[P4_MAX_FILE];
	for(BIG_INDEX index = 0; index < BIG_gst.st_ToSave.ul_MaxFile; index++)
	{
		// Once we have accumulated enough files, send data to Perforce
		if ( (iP4MaxFile != 0) && (iP4MaxFile == P4_MAX_FILE) )
		{
			m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
			m_Client.Run( P4_FLUSH, &ClientUserFlush);

			// delete allocated string
			for(ULONG index = 0 ; index < iP4MaxFile ; index++)
				delete[] ppsz_P4FilePathAndRevision[index];

			iP4MaxFile = 0;
		}

		if ( index != BIG_C_InvalidIndex)			
		{
			// Make sure the client revision on Perforce is the same as the 
			// client revision stored on the BF. If not set client revision on Perforce
			// to the client revision stored in BF
			if ( BIG_TmpRevisionClient(index) != BIG_P4RevisionClient(index) )
			{
				// Get P4 file path
				char asz_P4FilePath[MAX_PATH];
				DAT_CUtils::GetP4FileFromKey(BIG_FileKey(index), asz_P4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

				// Get P4 file path and revision
				char* psz_P4FilePathAndRevision = new char[MAX_PATH];
				sprintf(psz_P4FilePathAndRevision, "%s#%d", asz_P4FilePath, BIG_TmpRevisionClient(index));

				ppsz_P4FilePathAndRevision[iP4MaxFile] = psz_P4FilePathAndRevision;
				iP4MaxFile++;
			}		
		}
	}
	// send remaining data to Perforce
	if ( iP4MaxFile != 0 )
	{
		m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
		m_Client.Run( P4_FLUSH, &ClientUserFlush);
	}
	// delete allocated string
	for(ULONG index = 0 ; index < iP4MaxFile ; index++)
		delete[] ppsz_P4FilePathAndRevision[index];

	// Go through every key not stored in BF to set the client revision on Perforce to 0
	// since the file doesn't exist in the BF
	iP4MaxFile = 0;
	DAT_TP4FilesNotInBF::iterator Iter ;

	for ( Iter = GetP4FilesNotInBF().begin() ; Iter != GetP4FilesNotInBF().end() ; Iter++ )
	{
		// Once we have accumulated enough files, send data to Perforce
		if ( (iP4MaxFile != 0) && (iP4MaxFile == P4_MAX_FILE) )
		{
			m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
			m_Client.Run( P4_FLUSH, &ClientUserFlush);

			// delete allocated string
			for(ULONG index = 0 ; index < iP4MaxFile ; index++)
				delete[] ppsz_P4FilePathAndRevision[index];

			iP4MaxFile = 0;
		}

		// Get P4 file path
		char asz_P4FilePath[MAX_PATH];
		DAT_CUtils::GetP4FileFromKey((Iter->second)->ulKey, asz_P4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str());

		// Get P4 file path and revision
		char* psz_P4FilePathAndRevision = new char[MAX_PATH];
		sprintf(psz_P4FilePathAndRevision, "%s#0", asz_P4FilePath);

		ppsz_P4FilePathAndRevision[iP4MaxFile] = psz_P4FilePathAndRevision;
		iP4MaxFile++;
	}
	// send remaining data to Perforce
	if ( iP4MaxFile != 0 )
	{
		m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
		m_Client.Run( P4_FLUSH, &ClientUserFlush);
	}

	// delete allocated string
	for(ULONG index = 0 ; index < iP4MaxFile ; index++)
		delete[] ppsz_P4FilePathAndRevision[index];

	delete [] ppsz_P4FilePathAndRevision ;

	return 0;
}


int DAT_CPerforce::P4Flush(std::vector<ULONG >& _vFiles, ULONG rev)
{
	// Set the key's client revision stored in Perforce if different from one stored in BF.
	UINT iP4MaxFile = 0;
	DAT_CP4ClientUserFlush ClientUserFlush;
	char** ppsz_P4FilePathAndRevision = new char*[P4_MAX_FILE];

	// Calculated # of file to flush
	ClientUserFlush.SetNumberFilesToHandle(_vFiles.size());

	// Go through every file stored in vector to set the client revision on Perforce
	for(UINT ui = 0; ui < _vFiles.size(); ui ++ )
	{
		// Once we have accumulated enough files, send data to Perforce
		if ( (iP4MaxFile != 0) && (iP4MaxFile == P4_MAX_FILE) )
		{
			m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
			m_Client.Run( P4_FLUSH, &ClientUserFlush);

			// delete allocated string
			for(ULONG index = 0 ; index < iP4MaxFile ; index++)
				delete[] ppsz_P4FilePathAndRevision[index];

			iP4MaxFile = 0;
		}

		// Get P4 file path and revision
		char* psz_P4FilePathAndRevision = new char[MAX_PATH];
		char  asz_P4FilePath[MAX_PATH];
		
		DAT_CUtils::GetP4FileFromKey(_vFiles[ui], asz_P4FilePath,DAT_CPerforce::GetInstance()->GetP4Root().c_str());
		sprintf(psz_P4FilePathAndRevision, "%s#%d", asz_P4FilePath, rev);
		ppsz_P4FilePathAndRevision[iP4MaxFile] = psz_P4FilePathAndRevision;
		iP4MaxFile++;
	}
	// send remaining data to Perforce
	if ( iP4MaxFile != 0 )
	{
		m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
		m_Client.Run( P4_FLUSH, &ClientUserFlush);
	}
	// delete allocated string
	for(ULONG index = 0 ; index < iP4MaxFile ; index++)
		delete[] ppsz_P4FilePathAndRevision[index];

	delete [] ppsz_P4FilePathAndRevision ;

	return 0;
}

int DAT_CPerforce::P4Flush(std::vector<std::string>& _vFiles, ULONG rev)
{
	// Set the key's client revision stored in Perforce if different from one stored in BF.
	UINT iP4MaxFile = 0;
	DAT_CP4ClientUserFlush ClientUserFlush;
	char** ppsz_P4FilePathAndRevision = new char*[P4_MAX_FILE];

	// Calculated # of file to flush
	ClientUserFlush.SetNumberFilesToHandle(_vFiles.size());

	// Go through every file stored in vector to set the client revision on Perforce
	for(UINT ui = 0; ui < _vFiles.size(); ui ++ )
	{
		// Once we have accumulated enough files, send data to Perforce
		if ( (iP4MaxFile != 0) && (iP4MaxFile == P4_MAX_FILE) )
		{
			m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
			m_Client.Run( P4_FLUSH, &ClientUserFlush);

			// delete allocated string
			for(ULONG index = 0 ; index < iP4MaxFile ; index++)
				delete[] ppsz_P4FilePathAndRevision[index];

			iP4MaxFile = 0;
		}

		// Get P4 file path and revision
		char* psz_P4FilePathAndRevision = new char[MAX_PATH];

		// Do we want to flush to HEAD ?
		if ( rev != 0xFFFFFFFF)
			sprintf(psz_P4FilePathAndRevision, "%s#%d", _vFiles[ui].c_str(), rev);
		else
			sprintf(psz_P4FilePathAndRevision, "%s#head", _vFiles[ui].c_str());

		ppsz_P4FilePathAndRevision[iP4MaxFile] = psz_P4FilePathAndRevision;
		iP4MaxFile++;
	}
	// send remaining data to Perforce
	if ( iP4MaxFile != 0 )
	{
		m_Client.SetArgv( iP4MaxFile, ppsz_P4FilePathAndRevision );
		m_Client.Run( P4_FLUSH, &ClientUserFlush);
	}
	// delete allocated string
	for(ULONG index = 0 ; index < iP4MaxFile ; index++)
		delete[] ppsz_P4FilePathAndRevision[index];

	delete [] ppsz_P4FilePathAndRevision ;

	return 0;
}