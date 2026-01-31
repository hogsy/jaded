//------------------------------------------------------------------------------
// Filename   :STReamBigFile.cpp
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Description: Implementation of STReamBigFile
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "STReamBigFile.h"
#include "STReamaccess.h"

#include "BIGfiles\BIGfat.h"
#include "BIGfiles\BIGread.h"
#include "BASe\CLIbrary\CLIerrid.h"
#include "BIGfiles\BIGmdfy_file.h"
#include "BIGfiles\BIGmerge.h"
#include "BIGfiles\BIGdefs.h"
#include "BIGfiles\BIGmdfy_dir.h"
#include "BASe/MEMory/MEM.h"

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------
#define FILENAMELENGHT 10	
#define KEYLENGHT 8			

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------
extern "C" BOOL BIG_gb_IgnoreRecent;

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   int STReams_CBigFile::Export(const std::string& strFileExportDir, const std::string& strBFDirToExport)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int STReams_CBigFile::Export(const std::string& strFileExportDir, const std::string& strBFDirToExport)
{
	L_FILE h_TmpFile;
	int r;

	char szFullFilePathForKey[BIG_C_MaxLenPath];
	char szBFPathForKey[BIG_C_MaxLenPath], szFilePathForKey[BIG_C_MaxLenPath];
	
	// Go through every key stored in the fat
	for(BIG_INDEX index = 0; index < BIG_gst.st_ToSave.ul_MaxFile; index++)
	{
		if(BIG_FileKey(index) != BIG_C_InvalidKey)
		{
			// Get the BF path for the key 
			GetPathFromIndex(index, szBFPathForKey);

			// Get the File path for the key
			strcpy(szFilePathForKey, strFileExportDir.c_str());
			CreateDirectoryName(szFilePathForKey, index);
			
			// Does Key Dir exists ?
			if ( L_access( szFilePathForKey, 00 ) != 0 )
				CreateDirectoryRecursive(szFilePathForKey);

			// Get the key in the BF
			sprintf(szFullFilePathForKey, "%s/0x%08X", szFilePathForKey, BIG_FileKey(index));

			// Open file
			h_TmpFile = L_fopen(szFullFilePathForKey, L_fopen_WB);
			r=CLI_FileOpen(h_TmpFile);
			ERR_X_Error(r, L_ERR_Csz_FOpen, szFullFilePathForKey);

			// Write file 
			ULONG ul_Length;
			void *p_Buffer = (void *) BIG_pc_ReadFileTmp(BIG_PosFile(index), &ul_Length);
			r=BIG_fwrite(p_Buffer, ul_Length, h_TmpFile);
			ERR_X_Error(r == 1, L_ERR_Csz_FWrite, NULL);

			// Close file
			r=L_fclose(h_TmpFile);
			ERR_X_Error(r == 0, L_ERR_Csz_FClose, szFullFilePathForKey);

			// Write stream
			SetStreamByName(szFullFilePathForKey, STREAM_FILENAME,szBFPathForKey);
		}
	}
	return 0;

}

//------------------------------------------------------------
//   int STReams_CBigFile::Import(const std::string& strFileImportDir, const std::string& strBFDirToImport)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int STReams_CBigFile::Import(const std::string& strFileImportDir, const std::string& strBFDirToImport)
{
	ImportRecurse(strFileImportDir);

	return 0;
}

//------------------------------------------------------------
//   void STReams_CBigFile::ImportRecurse(std::string& strPath)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void STReams_CBigFile::ImportRecurse(const std::string& strPath)
{
	HANDLE hFind;
	WIN32_FIND_DATA wfd;
	std::string strText;

	strText = strPath + ("*");

	// ---------------------------------
	// Iterate through dirs
	hFind = FindFirstFile(strText.c_str(), &wfd);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if ((strcmp(wfd.cFileName,_T("."))) && (strcmp(wfd.cFileName,_T(".."))) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				strText = strPath + wfd.cFileName;

				// Recurse Dirs
				ImportRecurse(strText+("/"));
			}
		} while ( FindNextFile(hFind, &wfd) );
		FindClose(hFind);
	}

	// ---------------------------------
	// Iterate through files
	strText = strPath + "*.*" ;
	hFind = FindFirstFile(strText.c_str(), &wfd);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) 
			{
				std::string strFilePath;
				
				BIG_KEY ulBFFileKey;
				char strBFFileName[BIG_C_MaxLenPath + BIG_C_MaxLenName];
				void* pBFFileBuffer;
				ULONG ulBFFileSize;
				
				// Get KeyFileName
				strFilePath = strPath + wfd.cFileName ;

				// Get Key
				ulBFFileKey = GetKeyFromString(strFilePath.c_str());
				
				// Get BFFileName
				GetStreamByName(strFilePath.c_str(),STREAM_FILENAME, strBFFileName);
	
				// Get BFFileBuffer
				GetFileBuffer(strFilePath, pBFFileBuffer, ulBFFileSize);
	
				// UPDATE BIG FILE
				UpdateFileBuffer(ulBFFileKey, pBFFileBuffer, ulBFFileSize, strBFFileName);

				// Delete TMP BUFFER
				free(pBFFileBuffer);
			}
		} while ( FindNextFile(hFind, &wfd) );
		FindClose(hFind);
	}
}

//------------------------------------------------------------
//   void STReams_CBigFile::GetFileBuffer(const std::string& strFilePath, void*& pFileBuffer, ULONG& ulFileBufferSize)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void STReams_CBigFile::GetFileBuffer(const std::string& strFilePath, void*& pFileBuffer, ULONG& ulFileBufferSize)
{
	HANDLE hFile = CreateFile(strFilePath.c_str(), FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if(hFile != INVALID_HANDLE_VALUE)
	{
		HANDLE hMap;

		if( (ulFileBufferSize = GetFileSize(hFile,0)) != INVALID_FILE_SIZE)
			hMap = CreateFileMapping(hFile,0,PAGE_READONLY,0,0,0);
		else
		{
			pFileBuffer = NULL;
			ulFileBufferSize = 0;
			CloseHandle(hFile);
			return; 
		}

		void* pGetBuffer = (void *)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

		pFileBuffer = malloc(ulFileBufferSize);
		memcpy(pFileBuffer,pGetBuffer,ulFileBufferSize);

		UnmapViewOfFile(pGetBuffer);
		CloseHandle(hMap);
		CloseHandle(hFile);
		return;
	}

	pFileBuffer = NULL;
	ulFileBufferSize = 0;
	return;
}

//------------------------------------------------------------
//   char* STReams_CBigFile::GetPathFromIndex(BIG_INDEX FileIndex)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void STReams_CBigFile::GetPathFromIndex(BIG_INDEX index, char* szPath)
{
	if(index != BIG_C_InvalidIndex) 
	{
		BIG_ComputeFullName(BIG_ParentFile(index), szPath); 
		strcat(szPath, "/");
		strcat(szPath, BIG_NameFile(index));
	}
}

//------------------------------------------------------------
//   void STReams_CBigFile::CreateDirectoryRecursive(const char* szDirectory)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void STReams_CBigFile::CreateDirectoryRecursive(const char* szDirectory)
{
	char szTempPath[1024];
	strcpy(szTempPath, szDirectory);
	char* searchTarget = szTempPath;
	char charBackup=0;

	// When doing recursive creation, we don't want to match the first slash
	if(searchTarget[0] == '/')
		++searchTarget;

	while(1)
	{ 
		// Find the next slash
		searchTarget = strchr(searchTarget, '/');

		if ( searchTarget != NULL )
		{
			// Temporary ends the string.
			charBackup = *(searchTarget + 1);
			*(searchTarget + 1) = 0;
		}

		// Does directory exists ?
		if ( L_access( szTempPath, 0 ) != 0 )
			CreateDirectory(szTempPath, 0);

		// Restore the slash and check if we need to end the recursive creation
		if ( searchTarget != NULL )
		{
			++searchTarget;
			*searchTarget = charBackup;
		}
		else
			return;
	}
}

//------------------------------------------------------------
//   void STReams_CBigFile::CreateDirectoryName(char* szDirectory)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void STReams_CBigFile::CreateDirectoryName(char* szDirectoryName, BIG_INDEX index)
{
	// Replace '\' by '/'
	while (strchr(szDirectoryName, '\\')) 
		*strchr(szDirectoryName, '\\') = '/';

	// Make sure there is a trailing '/'
	if ( szDirectoryName[strlen(szDirectoryName)-1] != '/' )
	{
		szDirectoryName[strlen(szDirectoryName)] = '/';
		szDirectoryName[strlen(szDirectoryName)+1] = '\0';
	}

	char szKeyName[15], szDirectoryNameTmp[BIG_C_MaxLenPath];

	if(BIG_FileKey(index) != BIG_C_InvalidKey)
	{
		sprintf(szKeyName, "%08X", BIG_FileKey(index));

		// Extract the first two numbers of the keys to make it a subdirectory
		sprintf(szDirectoryNameTmp, "0x%08X", BIG_FileKey(index));
		szDirectoryNameTmp[4] = '/';

		// Extract the next four numbers of the keys to make it a subdirectory 
		strcpy(&szDirectoryNameTmp[4+1], &szKeyName[2]);
		szDirectoryNameTmp[4+1+4] = '\0';

		// Concatenate root dir with the subdir
		
		strcat(szDirectoryName, szDirectoryNameTmp);
	}
}

//------------------------------------------------------------
//   ULONG STReams_CBigFile::GetKeyFromString(const char* _psz_filename)
/// \author    YCharbonneau
/// \date      2005-01-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG STReams_CBigFile::GetKeyFromString(const char* _psz_filename)
{
	char* psz_Key = {0};
	if ( strrchr(_psz_filename,'/') == NULL )
		psz_Key = strrchr(const_cast<char*>(_psz_filename),'\\') + 3;
	else
		psz_Key = strrchr(const_cast<char*>(_psz_filename),'/') + 3;

	return GetKeyFromExactString(psz_Key);
}

//------------------------------------------------------------
//   ULONG STReams_CBigFile::GetKeyFromExactString(const char* _psz_filename)
/// \author    YCharbonneau
/// \date      2005-01-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG STReams_CBigFile::GetKeyFromExactString(const char* _psz_filename)
{
	ULONG ulKey = 0;
	for ( int i = 0; i < KEYLENGHT; i++) 
	{
		int iHex = (int) ( _psz_filename[i] - 0x30 );
		if ( iHex > 10 ) iHex --;
		iHex = ((iHex / 16) * 10) + (iHex%16);
		static int it = 1;
		static int it2 = 2;
		ulKey +=  iHex << (32 + (4 * (KEYLENGHT - i - 1)));
	}
	return ulKey;
}

//------------------------------------------------------------
//   void STReams_CBigFile::ExtractPathNameAndFiledName(std::string& strFileName, char* szPath, char* szName)
/// \author    NBeaufils
/// \date      20-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void STReams_CBigFile::ExtractPathNameAndFiledName(std::string& strFileName, char* szPath, char* szName)
{
	int nIndex ;
	if ((nIndex=strFileName.rfind('/')) != -1 )
	{
		strcpy(szName, strFileName.substr(nIndex+1).c_str());
		strcpy(szPath, strFileName.substr(0, nIndex).c_str());
		return;
	}

	if ((nIndex = strFileName.rfind('\\')) != -1 )
	{
		strcpy(szName, strFileName.substr(nIndex+1).c_str());
		strcpy(szPath, strFileName.substr(0, nIndex).c_str());
		return;
	}
	szPath[0] = '\0';
	szName[0] = '\0';
}

//------------------------------------------------------------
//   ULONG STReams_CBigFile::UpdateFileBuffer()
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG STReams_CBigFile::UpdateFileBuffer(BIG_KEY _ulKey, void* _pBuffer,ULONG _ulBufferSize, char* _psz_BFFileName)
{
	char asz_PathName[MAX_PATH];
	char asz_FileName[MAX_PATH];

	std::string strTmp(_psz_BFFileName);

	ExtractPathNameAndFiledName(strTmp, asz_PathName, asz_FileName);

	// Set global variables so the update is done properly
	BIG_gpsz_RealName = NULL;
	BIG_gul_GlobalKey = _ulKey;
	BIG_gx_GlobalTime = 0;
	BIG_gx_GlobalClientRev = 0;
	BIG_gpsz_GlobalName = asz_FileName;
	BIG_gp_Buffer = _pBuffer;
	BIG_gul_Length = _ulBufferSize;
	BIG_gb_IgnoreRecent = TRUE;

	ULONG ulIndex = BIG_ul_UpdateCreateFileOptim(BIG_C_InvalidIndex, BIG_C_InvalidIndex, asz_PathName, asz_FileName, NULL, FALSE);

	// Once the update is done, reset variables to make 
	// sure they are not used with the current values
	BIG_gb_IgnoreRecent = FALSE;
	BIG_gx_GlobalTime = NULL;
	BIG_gx_GlobalClientRev = NULL;
	BIG_gp_Buffer = NULL;
	BIG_gul_Length = NULL;
	BIG_gul_GlobalKey = BIG_C_InvalidKey;

	return ulIndex;
}

#endif // ACTIVE_EDITORS

//------------------------------------------------------------------------------

