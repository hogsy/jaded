//------------------------------------------------------------------------------
// Filename   :DATCUtils.cpp
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: Implementation of DATCUtils
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCUtils.h"
//#include "DATCPerforce.h"
#include "DATCP4BFObject.h"

#include "assert.h"

// Bigfile support
#include "BIGfiles\BIGfat.h"
#include "BIGfiles\BIGmdfy_file.h"
#include "BIGfiles\BIGmerge.h"
#include "BIGfiles\BIGdefs.h"
#include "BIGfiles\BIGmdfy_dir.h"
#include "BIGfiles\BIGread.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "EDIpaths.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAmsglink_dlg.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"

#include "sdk/Sources/BIGfiles/BIGgroup.h"

#define FILENAMELENGHT 10
#define KEYLENGHT      8	

EDIA_cl_UPDATEDialog* DAT_CUtils::ms_pProgress = NULL;
//extern EDIA_cl_MsgLinkDialog    EDI_go_MsgTruncateFiles;
#ifdef JADEFUSION
extern BOOL						EDI_gb_NoVerbose;
#else
extern "C" BOOL						EDI_gb_NoVerbose;
#endif
//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------
//------------------------------------------------------------
//   void DAT_CUtils::ExtractPathAndFileName(const char* szFileName, char* szPath, char* szName)
/// \author    NBeaufils
/// \date      20-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::ExtractPathAndFileName(const char* szFileName, char* szPath, char* szName)
{
	char* psz_Tmp;
	strcpy(szPath, szFileName);

	if ( strchr(szPath, '\\') ) 
	{
		psz_Tmp = szPath;
		while ( strchr(psz_Tmp, '\\') )
			psz_Tmp = strchr(psz_Tmp, '\\')+1;

		strcpy(szName, psz_Tmp);
		szPath[strlen(szPath)-(strlen(szName)+1)] = '\0';
		return;
	}		
	if ( strchr(szPath, '/') ) 
	{
		psz_Tmp = szPath;
		while ( strchr(psz_Tmp, '/') )
			psz_Tmp = strchr(psz_Tmp, '/')+1;

		strcpy(szName, psz_Tmp);
		szPath[strlen(szPath)-(strlen(szName)+1)] = '\0';
		return;
	}		
	szPath[0] = '\0';
	szName[0] = '\0';
	
	return;
}

//------------------------------------------------------------
//   ULONG DAT_CUtils::GetKeyFromString(const char* _psz_filename)
/// \author    YCharbonneau
/// \date      2005-01-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG DAT_CUtils::GetKeyFromString(const char* _psz_filename)
{
	if ( strrchr(_psz_filename,'/') == NULL && strrchr(_psz_filename,'\\') == NULL )
		return BIG_C_InvalidKey;

	char* psz_Key = {0};
	if ( strrchr(_psz_filename,'/') == NULL )
		psz_Key = strrchr(const_cast<char*>(_psz_filename),'\\')+1;
	else
		psz_Key = strrchr(const_cast<char*>(_psz_filename),'/')+1;

	if ( psz_Key[0] != '0' || psz_Key[1] != 'x')
		return BIG_C_InvalidKey;

	// Skip 0x contained in the string
	psz_Key = psz_Key + 2;
	
	char aszKey[KEYLENGHT + 1] = {0};
	strncpy(aszKey,psz_Key,KEYLENGHT);
	return GetKeyFromExactString(aszKey);
}

//------------------------------------------------------------
//   ULONG DAT_CUtils::GetKey(const char* _psz_filename)
/// \author    YCharbonneau
/// \date      2005-01-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG DAT_CUtils::GetKeyFromExactString(const char* _psz_filename)
{
	ULONG ulKey = 0;
	
	if (strlen(_psz_filename)  != KEYLENGHT ) 
		return BIG_C_InvalidKey;

	for ( int i = 0; i < KEYLENGHT; i++) 
	{
		int iHex = (int) ( toupper(_psz_filename[i]) - 0x30 );
		if ( iHex > 10 ) iHex --;
		iHex = ((iHex / 16) * 10) + (iHex%16);
		static int it = 1;
		static int it2 = 2;
		ulKey +=  iHex << (32 + (4 * (KEYLENGHT - i - 1)));
	}
	return ulKey;
}

//------------------------------------------------------------
//   void DAT_CUtils::DAT_CUtils::DAT_CUtils::GetP4FileFromKey(ULONG ul_Key,char* _psz_filename)
/// \author    YCharbonneau
/// \date      2005-01-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::GetP4FileFromKey(ULONG ul_Key,char* _psz_filename,const char* _pszP4Root)
{
	char asz_FirstPath[MAX_PATH] = {0};
	char asz_SecondPath[MAX_PATH] = {0};
	sprintf(asz_FirstPath,"0x%08X",ul_Key);
	asz_FirstPath[4] = 0;
	sprintf(asz_SecondPath,"%08X",ul_Key);
	char * psz_SecondPath = &asz_SecondPath[0] + 2;
	psz_SecondPath[4] = 0;
	sprintf(_psz_filename,"%s%s/%s/0x%08X",_pszP4Root,asz_FirstPath,psz_SecondPath,ul_Key);
}

//------------------------------------------------------------
//   void DAT_CUtils::GetBFIndexesFromDirIndex(BIG_INDEX ul_Dir,std::vector<BIG_INDEX> _vIndex)
/// \author    YCharbonneau
/// \date      2005-01-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::GetBFIndexesFromDirIndex(BIG_INDEX ul_Dir,std::vector<BIG_INDEX>& _vIndex,BOOL _bRecursive /* = FALSE */ )
{
	if ( _bRecursive ) 
	{
		BIG_INDEX SubDirIndex = BIG_SubDir(ul_Dir);
		while(SubDirIndex != BIG_C_InvalidIndex)
		{
			GetBFIndexesFromDirIndex(SubDirIndex,_vIndex,_bRecursive);
			SubDirIndex = BIG_NextDir(SubDirIndex);
		}
	}

	BIG_INDEX FileIndex = BIG_FirstFile(ul_Dir);
	while ( FileIndex != BIG_C_InvalidIndex )
	{
		_vIndex.push_back(FileIndex);
		FileIndex = BIG_NextFile(FileIndex);
	}
}


//------------------------------------------------------------
//   void DAT_CUtils::GetP4FileFromKey(BIG_KEY _ul_Key, std::string& _strP4FilePath,std::string& _strP4Root);
/// \author    NBeaufils
/// \date      18-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::GetP4FileFromKey(BIG_KEY _ul_Key, std::string& _strP4FilePath,std::string& _strP4Root)
{
	char asz_P4FilePath[MAX_PATH] = {0};
	char asz_FirstPath[MAX_PATH] = {0};
	char asz_SecondPath[MAX_PATH] = {0};
	sprintf(asz_FirstPath,"0x%08X",_ul_Key);
	asz_FirstPath[4] = 0;
	sprintf(asz_SecondPath,"%08X",_ul_Key);
	char * psz_SecondPath = &asz_SecondPath[0] + 2;
	psz_SecondPath[4] = 0;
	sprintf(asz_P4FilePath,"%s%s/%s/0x%08X",_strP4Root.c_str(),asz_FirstPath,psz_SecondPath,_ul_Key);

	_strP4FilePath = asz_P4FilePath;
}

//------------------------------------------------------------
//   void DAT_CUtils::GetP4FileFromDirIndex(BIG_KEY _ul_Key, std::string& _strP4FilePath,std::string& _strP4Root);
/// \author    NBeaufils
/// \date      18-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::GetP4FilesFromDirIndex(const BIG_INDEX _DirIndex,
										std::vector<std::string>& _vP4Files, 
										std::vector<BIG_INDEX>* _pvP4Index, /* can be null */
										const char* _pszP4Root,
										BOOL _bRecursive /* = FALSE */ 
										)
										
{
	if ( _bRecursive ) 
	{
		BIG_INDEX SubDirIndex = BIG_SubDir(_DirIndex);
		while(SubDirIndex != BIG_C_InvalidIndex)
		{
			GetP4FilesFromDirIndex(SubDirIndex,_vP4Files,_pvP4Index,_pszP4Root,_bRecursive);
			SubDirIndex = BIG_NextDir(SubDirIndex);
		}
	}

	BIG_INDEX FileIndex = BIG_FirstFile(_DirIndex);
	while ( FileIndex != BIG_C_InvalidIndex )
	{
		if ( _pvP4Index ) _pvP4Index->push_back(FileIndex);

		// Get P4 file path
		char asz_P4FilePath[MAX_PATH];
		DAT_CUtils::GetP4FileFromKey(BIG_FileKey(FileIndex), asz_P4FilePath, _pszP4Root);
		
		_vP4Files.push_back(asz_P4FilePath);

		FileIndex = BIG_NextFile(FileIndex);
	}
}

//------------------------------------------------------------
//   DAT_CUtils::GetP4FilesFromVecIndex(std::vector<BIG_INDEX> vFileIndex, std::vector<std::string> vP4Files, const char* _pszP4Root)
/// \author    NBeaufils
/// \date      18-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::GetP4FilesFromVecIndex(const std::vector<BIG_INDEX>& vFileIndex, std::vector<std::string>& vP4Files, const char* _pszP4Root)
{
	std::vector<BIG_INDEX>::const_iterator Iter = vFileIndex.begin();
	std::vector<BIG_INDEX>::const_iterator IterEnd = vFileIndex.end();
	
	for ( ; Iter < IterEnd ; Iter++ )
	{
		if ( BIG_FileKey((*Iter)) == BIG_C_InvalidKey )
			continue;

		// Get P4 file path
		char asz_P4FilePath[MAX_PATH];
		DAT_CUtils::GetP4FileFromKey(BIG_FileKey((*Iter)), asz_P4FilePath, _pszP4Root);

		vP4Files.push_back(asz_P4FilePath);
	}
}

//------------------------------------------------------------
//   void DAT_CUtils::GetP4FilesFromVecKey(std::vector<BIG_KEY> vFileKey, std::vector<std::string> vP4Files, const char* _pszP4Root)
/// \author    NBeaufils
/// \date      18-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::GetP4FilesFromVecKey(const std::vector<BIG_KEY>& vFileKey, std::vector<std::string>& vP4Files, const char* _pszP4Root)
{
	std::vector<BIG_INDEX>::const_iterator Iter = vFileKey.begin();
	std::vector<BIG_INDEX>::const_iterator IterEnd = vFileKey.end();

	for ( ; Iter < IterEnd ; Iter++ )
	{
		if ( (*Iter) == BIG_C_InvalidKey )
			continue;

		// Get P4 file path
		char asz_P4FilePath[MAX_PATH];
		DAT_CUtils::GetP4FileFromKey((*Iter), asz_P4FilePath, _pszP4Root);

		vP4Files.push_back(asz_P4FilePath);
	}
}

//------------------------------------------------------------
//   void DAT_CUtils::GetBFFileFromKey(ULONG ul_Key,char* _psz_BFFilePath)
/// \author    YCharbonneau
/// \date      2005-01-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::GetBFFileFromKey(BIG_KEY _ulKey,char* _psz_BFFilePath)
{
	ULONG ulIndex = BIG_ul_SearchKeyToFat(_ulKey);
	if ( ulIndex == BIG_C_InvalidIndex ) 
		return;

	BIG_ComputeFullName(BIG_ParentFile(ulIndex), _psz_BFFilePath);
	strcat(_psz_BFFilePath,"/");
	strcat(_psz_BFFilePath,BIG_NameFile(ulIndex));
}

//------------------------------------------------------------
//   void DAT_CUtils::GetBFFileFromIndex(BIG_INDEX _ulIndex,char* _psz_BFFilePath)
/// \author    YCharbonneau
/// \date      2005-02-07
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::GetBFFileFromIndex(BIG_INDEX _ulIndex,char* _psz_BFFilePath)
{
	BIG_ComputeFullName(BIG_ParentFile(_ulIndex), _psz_BFFilePath);
	strcat(_psz_BFFilePath,"/");
	strcat(_psz_BFFilePath,BIG_NameFile(_ulIndex));
}

//------------------------------------------------------------
//   void DAT_CUtils::SetHeader(BIG_INDEX _ulIndex, DAT_CP4BFObjectHeader& _ObjectHeader)
/// \author    YCharbonneau
/// \date      2005-02-07
/// \par       Description: 
///			   Retreive the requested information from the bf to build a header for 
///			   a file.	
///            We need to have every added info in the file header to be added here. 
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::SetHeader(BIG_INDEX _ulIndex, DAT_CP4BFObjectHeader& _ObjectHeader)
{
	// getting the filepath from the BF and setting it to the file
	char aszBFFilename[BIG_C_MaxLenPath + BIG_C_MaxLenName];
	DAT_CUtils::GetBFFileFromIndex(_ulIndex,aszBFFilename);

	_ObjectHeader.SetPath(std::string(aszBFFilename));
	_ObjectHeader.SetVersion(BIG_Version());
	_ObjectHeader.SetJadeKey(BIG_FileKey(_ulIndex));

	if ( BIG_FileKey(_ulIndex) == BIG_UniverseKey() ) 
	{
		_ObjectHeader.SetIsUniverseKey(true);
	}
}

//------------------------------------------------------------
//   void DAT_CUtils::PrepareFileBuffer()
/// \author    FFerland
/// \date      2005-02-10
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
char* DAT_CUtils::PrepareFileBuffer( BIG_INDEX _ulIndex, DWORD& _ulBufferSize, BOOL _bEnableTruncate  /* = FALSE */  )
{
	if ( _ulIndex == BIG_C_InvalidIndex)
		return NULL;

	void* pBfBuffer = NULL; 
	// getting buffer from BF 
	ULONG ulBfBufferSize = 0 ;

	// truncate specific sound & videos
	if ( _bEnableTruncate && DAT_CUtils::IsTruncateFile(_ulIndex) ) 
	{
		pBfBuffer = L_malloc( 8 ); 
		_ulBufferSize = 8;
	}
	else
	{
		pBfBuffer = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_ulIndex), &_ulBufferSize);
		if( pBfBuffer == NULL && ulBfBufferSize == 0 )
		{
			// Bug fix: Allocate memory anyway, otherwise some code exists early later on because 'pBfBuffer' is NULL.
			//          'ulBfBufferSize' will still be 0, so allocating memory is still ok.
			pBfBuffer = L_malloc( 0 );
		}
	}

	// creating a special BF fileobject
	DAT_CP4BFObjectHeader	ObjectHeader;
	DAT_CP4BFObjectData		ObjectData;

	// setting up the header for the file
	DAT_CUtils::SetHeader(_ulIndex,ObjectHeader);

	// Dont compress the data when exporting to keys if more than 20 000 000 bytes
	// Because if the data is binarized and is big (like texture files)
	// the compress will fail to allocate enough memory
	BOOL compressData = true;
	extern BOOL EDI_gb_ExportToKeys;
	if (EDI_gb_ExportToKeys && _ulBufferSize > 20000000)
		compressData = false;

	// if we do have a content
	if( pBfBuffer )
		ObjectData.SetData(pBfBuffer,_ulBufferSize,compressData);

	// creation the output buffer
	char* pBuffer = new char[ObjectHeader.Size() + ObjectData.CompressedSize()];

	// writing header and data info to buffer
	ObjectHeader.Write(pBuffer,ObjectHeader.Size());

	size_t nObjectSize = ObjectData.CompressedSize();
	ObjectData.Write(pBuffer + ObjectHeader.Size(), nObjectSize);

	_ulBufferSize = ObjectHeader.Size() + nObjectSize;

	// cleanup
	free(pBfBuffer);

	// return buffer
	return pBuffer;
}



//------------------------------------------------------------
//   int DAT_CUtils::Export(const std::string& strFileExportDir, const std::string& strBFDirToExport)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CUtils::Export(std::string& strFileExportDir, const std::string &strBFDirToExport)
{
	L_FILE h_TmpFile;
	
	char szFullFilePathForKey[BIG_C_MaxLenPath];
	char szBFPathForKey[BIG_C_MaxLenPath];
	std::string strFilePathForKey;

	ms_pProgress = new EDIA_cl_UPDATEDialog("");
	ms_pProgress->DoModeless();

	// Go through every key stored in the fat
	for(BIG_INDEX index = 0; index < BIG_gst.st_ToSave.ul_MaxFile; index++)
	{
		if(BIG_FileKey(index) != BIG_C_InvalidKey)
		{
			// Get the BF path for the key 
			GetBFFileFromIndex(index, szBFPathForKey);

			// Get the File path for the key
			GetP4FileFromKey(BIG_FileKey(index), strFilePathForKey, strFileExportDir);
			sprintf(szFullFilePathForKey, "%s", strFilePathForKey.c_str());
			strFilePathForKey.erase(strFilePathForKey.size() - FILENAMELENGHT -1);	// remote filename from path

			// Does Key Dir exists ?
			if ( L_access(strFilePathForKey.c_str(), 00) != 0 )
				CreateDirectoryRecursive(strFilePathForKey.c_str());

			// Open file
			h_TmpFile = L_fopen(szFullFilePathForKey, L_fopen_WB);
			ERR_X_Error(CLI_FileOpen(h_TmpFile), L_ERR_Csz_FOpen, szFullFilePathForKey);

			// Create the BF Object with the header and all
			DWORD buffSize;
			char* pBuffer;
			pBuffer = PrepareFileBuffer( index, buffSize );

			ERR_X_Error(BIG_fwrite(pBuffer, buffSize, h_TmpFile) == 1, L_ERR_Csz_FWrite, NULL);

			delete[] pBuffer;

			// Close file
			ERR_X_Error(L_fclose(h_TmpFile) == 0, L_ERR_Csz_FClose, szFullFilePathForKey);

			// Show progress
			if (index % (BIG_gst.st_ToSave.ul_MaxFile/100) == 0)
				ms_pProgress->OnRefreshBarText((float)index/(float)BIG_gst.st_ToSave.ul_MaxFile, "Exporting... Please wait");
		}
	}

	delete ms_pProgress;
	ms_pProgress = NULL;
	return 0;

}

//------------------------------------------------------------
//   void DAT_CUtils::CreateDirectoryRecursive(const char* szDirectory)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::CreateDirectoryRecursive(const char* szDirectory)
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
		if ( L_access(szTempPath, 0) != 0 )
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

static ULONG ul_DirsImported = 0;
static ULONG ul_DirsToImport = 0;

//------------------------------------------------------------
//   void DAT_CUtils::CountDirs(std::string& strPath)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::CountDirs(const std::string& strPath)
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
				CountDirs(strText + ("/"));
				ul_DirsToImport++;
			}
		} while ( FindNextFile(hFind, &wfd) );
		FindClose(hFind);
	}
}

//------------------------------------------------------------
//   int DAT_CUtils::Import(const std::string& strFileExportDir, const std::string& strBFDirToExport)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
int DAT_CUtils::Import(std::string& strFileImportDir, const std::string &strBFDirToImport)
{
	ul_DirsImported = 0;
	ul_DirsToImport = 0;
	CountDirs(strFileImportDir);

	ms_pProgress = new EDIA_cl_UPDATEDialog("");
	ms_pProgress->DoModeless();

	ImportRecurse(strFileImportDir);

	M_MF()->FatHasChanged();
	delete ms_pProgress;
	ms_pProgress = NULL;
	return 0;
}

//------------------------------------------------------------
//   void DAT_CUtils::ImportRecurse(std::string& strPath)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::ImportRecurse(const std::string& strPath)
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

				// Show progress
				ul_DirsImported++;
				ms_pProgress->OnRefreshBarText((float)ul_DirsImported/(float)ul_DirsToImport, "Importing... Please wait");
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
				void* pBFFileBuffer;
				ULONG ulBFFileSize;

				// Get KeyFileName
				strFilePath = strPath + wfd.cFileName ;

				// Get Key
				ulBFFileKey = GetKeyFromString(strFilePath.c_str());

#ifdef JADEFUSION
				if (ulBFFileKey != BIG_C_InvalidKey)	// skip invalid files (those whose name is not a key)
#endif
				{
				// Get BFFileBuffer
				GetFileBuffer(strFilePath, pBFFileBuffer, ulBFFileSize);

				// UPDATE BIG FILE
				UpdateFileBuffer(ulBFFileKey, pBFFileBuffer, ulBFFileSize);

				// Delete TMP BUFFER
				free(pBFFileBuffer);
				}
				}
		} while ( FindNextFile(hFind, &wfd) );
		FindClose(hFind);
	}
}

//------------------------------------------------------------
//   void DAT_CUtils::GetFileBuffer(const std::string& strFilePath, void*& pFileBuffer, ULONG& ulFileBufferSize)
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void DAT_CUtils::GetFileBuffer(const std::string& strFilePath, void*& pFileBuffer, ULONG& ulFileBufferSize)
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
			assert(0);
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

	assert(0);
	pFileBuffer = NULL;
	ulFileBufferSize = 0;
	return;
}

//------------------------------------------------------------
//   ULONG DAT_CUtils::UpdateFileBuffer()
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
ULONG DAT_CUtils::UpdateFileBuffer(BIG_KEY _ulKey, void* _pBuffer,ULONG _ulBufferSize)
{
	DAT_CP4BFObject BfObject(_pBuffer, _ulBufferSize);

	std::string str_FileName;
	BfObject.Header().Path(str_FileName);

	BIG_KEY ulHeaderKey;
	bool bJadeKeyIsSet;
	bJadeKeyIsSet = BfObject.Header().JadeKey(ulHeaderKey);
	if (!bJadeKeyIsSet)		// skip files that dont have a key in the header
		return BIG_C_InvalidIndex;

	assert( ulHeaderKey == _ulKey ) ;

	// if its the univers key we set it 
	UINT IsUniverse = FALSE;
	BfObject.Header().IsUniverseKey(IsUniverse);
	if(IsUniverse) BIG_UniverseKey() = _ulKey;

	void* pData = NULL;
	size_t dataSize = 0;
	BfObject.Data().Data(pData, dataSize);

	// -- IMPORTANT NOTE --
	// If pData is NULL and uiDataSize is 0, we still want the file
	// to be updated in the BF. So we set a value for the pointer
	// pData (see BIG_ul_UpdateCreateFileOptim function for details)
	if ( pData == NULL && dataSize == 0)
		pData = _pBuffer; 

	ULONG ulIndex = BIG_C_InvalidIndex; //DAT_CPerforce::UpdateBFFileBuffer(ulHeaderKey,pData,dataSize,str_FileName.c_str());
	BfObject.Data().Free();

	return ulIndex;
}

//------------------------------------------------------------
//   BOOL DAT_CUtils::IsTruncateFile(BIG_INDEX _ul_File )
/// \author    YCharbonneau
/// \date      08/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL DAT_CUtils::IsTruncateFile(BIG_INDEX _ul_File )
{
	if
		(
		(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtSoundMusic))
		||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtSoundAmbience))
		||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtSoundDialog))
		||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtSoundFile))
		||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtLoadingSound))
		||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo1))
		||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo2))
		||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo4))
		||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo5))
		||	(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtVideo3))
		)
	{
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------
//   BOOL DAT_CUtils::GetTruncateFile(BIG_INDEX _ul_File)
/// \author    YCharbonneau
/// \date      08/07/2005
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL DAT_CUtils::GetTruncateFile(BIG_INDEX _ul_File)
{
    // Always answer YES.
    return TRUE;
    /*
	if ( !IsTruncateFile(_ul_File ) ) return TRUE; 

	EDI_go_MsgTruncateFiles.mo_Name = CString(BIG_NameFile(_ul_File));
	::SetFocus(0);

	if( EDI_gb_NoVerbose ) return TRUE;

	int res;
	if(((res = EDI_go_MsgTruncateFiles.DoModal()) == IDYES)) 
			return TRUE;
	return FALSE;*/
}



BOOL DAT_CUtils::GetGroupReferences(BIG_INDEX _ul_File,std::vector<BIG_KEY>& _vGroup )
{
	if ( !BIG_b_IsGrpFile(_ul_File ) ) 
		return FALSE;	

	char	*pst_Buf;
	ULONG ulSize;
	

	pst_Buf = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_ul_File), &ulSize);
	DAT_CUtils::GetGroupReferences(pst_Buf,ulSize,_vGroup );
	L_free(pst_Buf);
	return TRUE;

}

BOOL DAT_CUtils::GetGroupReferences(char* _pBuffer,ULONG _ulSize,std::vector<BIG_KEY>& _vGroup )
{
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG ulIndex;

	pst_Buf = (BIG_tdst_GroupElem *) _pBuffer;

	/* Test if key is already here */
	for(ulIndex = 0; ulIndex < _ulSize / sizeof(BIG_tdst_GroupElem); ulIndex++)
	{
		if(pst_Buf[ulIndex].ul_Key == 0) continue;
		if(pst_Buf[ulIndex].ul_Key == BIG_C_InvalidKey) continue;
		_vGroup.push_back(pst_Buf[ulIndex].ul_Key);
	}
	return TRUE;
}