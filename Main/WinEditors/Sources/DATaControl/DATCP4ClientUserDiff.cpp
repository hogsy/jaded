//------------------------------------------------------------------------------
// Filename   :DATCP4ClientUserDiff.cpp
/// \author    NBeaufils
/// \date      12-Jan-05
/// \par       Description: Implementation of DATCP4ClientUserDiff
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#include "DATCP4ClientUserDiff.h"
#include "p4/clientapi.h"
//------------------------------------------------------------------------------

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

void DAT_CP4ClientUserDiff::Diff(FileSys *pServerFile, FileSys *pClientFile, int doPage, char *diffFlags, Error *e)
{
	BIG_KEY ulKeyClient = DAT_CUtils::GetKeyFromString( pClientFile->Name() );
	BIG_INDEX ulIndexClient = BIG_ul_SearchKeyToFat(ulKeyClient);

	assert(ulIndexClient != BIG_C_InvalidIndex && "DAT_CP4ClientUserDiff::Diff - File doesn't exist on client, cannot diff with server.");
	assert(m_pvFiles != NULL && "DAT_CP4ClientUserDiff::Diff - Vector is NULL, cannot push back results of diff.");
	assert((*m_pvFiles->back()).IsValid() && "DAT_CP4ClientUserDiff::Diff - Last element of vector is invalid, cannot push back results of diff.");
	assert((*m_pvFiles->back()).ulKey == ulKeyClient && "DAT_CP4ClientUserDiff::Diff - Last element of vector doesn't correspond to file diffed.");
		
	DAT_CP4ClientInfoDiff* pInfoDiff = (DAT_CP4ClientInfoDiff*)&(*m_pvFiles->back());
	
	// Initialize Diff
	pInfoDiff->dwFileDiff = 0;
	strcpy(pInfoDiff->aszFilenameServer, ((DAT_CP4FileSysDiff*)pServerFile)->GetP4Filename().c_str());
	pInfoDiff->ulFileSizeServer = ((DAT_CP4FileSysDiff*)pServerFile)->GetP4FileSize();

   	// Get client path/name 
	char aszClientPathname[BIG_C_MaxLenPath];
	char aszClientFilename[BIG_C_MaxLenName];	
	BIG_ComputeFullName(BIG_ParentFile(ulIndexClient), aszClientPathname);
	strcpy(aszClientFilename, BIG_NameFile(ulIndexClient));

	// Get server path/name
	char aszServerPathName[BIG_C_MaxLenPath];
	char aszServerFilename[BIG_C_MaxLenName];	
	DAT_CUtils::ExtractPathAndFileName(pInfoDiff->aszFilenameServer, aszServerPathName, aszServerFilename);

	// Compare client path with server path
	if ( strcmp(aszClientPathname, aszServerPathName) != 0 )
	{
		pInfoDiff->dwFileDiff |= DAT_CP4ClientInfoDiff::eFileMoved;

		char aszMessage[MAX_PATH] = {0};
		sprintf ( aszMessage, " File %s has been moved from %s to %s", aszClientFilename, aszServerPathName, aszClientPathname);
		DAT_CP4Message::OutputWarningMessage(aszMessage);
	}
	
	// Compare client name with server name
	if ( strcmp(aszClientFilename, aszServerFilename) != 0 )
	{
		pInfoDiff->dwFileDiff |= DAT_CP4ClientInfoDiff::eFileRenamed;

		char aszMessage[MAX_PATH] = {0};
		sprintf ( aszMessage, " File %s has been renamed from %s", aszClientFilename, aszServerFilename);
		DAT_CP4Message::OutputWarningMessage(aszMessage);
	}

	// Compare client data with server data
	ULONG ulFileSizeClient = 0 ;
	void* pFileDataClient = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ulIndexClient), &ulFileSizeClient);

	if ( ulFileSizeClient != pInfoDiff->ulFileSizeServer ) 
	{
		pInfoDiff->dwFileDiff |= DAT_CP4ClientInfoDiff::eFileModified;

		char aszMessage[MAX_PATH] = {0};
		sprintf ( aszMessage, " File %s has been modified in directory %s", aszClientFilename, aszClientPathname);
		DAT_CP4Message::OutputWarningMessage(aszMessage);
	}
	else
	{
		// Comparing the 2 buffers to be sure nothing has changed.
		if ( memcmp(((DAT_CP4FileSysDiff*)pServerFile)->GetP4FileData(), pFileDataClient, ulFileSizeClient) != 0 )
		{
			pInfoDiff->dwFileDiff |= DAT_CP4ClientInfoDiff::eFileModified;

			char aszMessage[MAX_PATH] = {0};
			sprintf ( aszMessage, " File %s has been modified in directory %s", aszClientFilename, aszClientPathname);
			DAT_CP4Message::OutputWarningMessage(aszMessage);
		}
	}
		
	L_free(pFileDataClient);
}

//------------------------------------------------------------------------------

