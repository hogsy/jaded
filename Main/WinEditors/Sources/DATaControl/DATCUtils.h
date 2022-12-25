
//------------------------------------------------------------------------------
//   DAT_CUtils.h
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class DAT_CUtils
/// \see 
//------------------------------------------------------------------------------

#ifndef __DATCUTILS_H__
#define __DATCUTILS_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------

#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Preprocessor definitions 
//------------------------------------------------------------------------------
class DAT_CP4BFObjectHeader;
class DAT_CP4ClientInfoHeader;
class EDIA_cl_UPDATEDialog;
//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------

class DAT_CUtils    
{
	   //--------------------------------------------------------------------------
	   // public definitions 
	   //--------------------------------------------------------------------------

public:
 
		static void		GetP4FileFromKey(BIG_KEY _ul_Key, char* _psz_P4FilePath,const char* _pszP4Root);
		static void		GetP4FileFromKey(BIG_KEY _ul_Key, std::string& _strP4FilePath,std::string& _strP4Root);

		static void		GetP4FilesFromDirIndex(const BIG_INDEX _DirIndex,std::vector<std::string>& _vP4Files, std::vector<BIG_INDEX>* _pvP4Index,const char* _pszP4Root,BOOL _bRecursive = FALSE ); 

		static void		GetP4FilesFromVecIndex(const std::vector<BIG_INDEX>& vFileIndex, std::vector<std::string>& vP4Files, const char* _pszP4Root);
		static void		GetP4FilesFromVecKey(const std::vector<BIG_KEY>& vFileKey, std::vector<std::string>& vP4Files, const char* _pszP4Root);

		static void		GetBFFileFromKey(BIG_KEY _ul_Key, char* _psz_BFFilePath);		
		static void		GetBFFileFromIndex(BIG_INDEX _ulIndex,char* _psz_BFFilePath);
		static void		GetBFIndexesFromDirIndex(BIG_INDEX ul_Dir,std::vector<BIG_INDEX>& _vIndex,BOOL _bRecursive = FALSE );

		static ULONG	GetKeyFromString(const char* _psz_filename);
		static ULONG	GetKeyFromExactString(const char* _psz_filename);

		static void		ExtractPathAndFileName(const char*, char* szPath, char* szName);
		static void		SetHeader(BIG_INDEX _ulIndex, DAT_CP4BFObjectHeader& _ObjectHeader);
		static void		GetHeader(DAT_CP4BFObjectHeader& _ObjectHeader, DAT_CP4ClientInfoHeader& _InfoHeader);

		static char*	PrepareFileBuffer( BIG_INDEX _ulIndex, DWORD& _ulBufferSize,BOOL _bEnableTrucate  = FALSE );

		static void		RunDiff( const char* szFile1, const char* szFile2, BIG_KEY _ul_Key, HWND hWndDlg = NULL );
		static std::string MakeTempFName( const char* szBaseName, const DAT_CP4ClientInfoHeader& info );

		static int Export(std::string& strFileExportDir, std::string& strBFDirToExport);
		static int Import(std::string& strFileImportDir, std::string& strBFDirToImport);
		static void CountDirs(const std::string& strPath);
		static void ImportRecurse(const std::string& strPath);
		static void GetFileBuffer(const std::string& strFilePath, void*& pFileBuffer, ULONG& ulFileBufferSize);
		static ULONG UpdateFileBuffer(BIG_KEY _ulKey, void* _pBuffer,ULONG _ulBufferSize);
		static void CreateDirectoryRecursive(const char* szDirectory);
		static BOOL GetTruncateFile(BIG_INDEX _ul_File);
		static BOOL IsTruncateFile(BIG_INDEX _ul_File);
		static BOOL GetGroupReferences(BIG_INDEX _ul_File,std::vector<BIG_KEY>& _vGroup );
		static BOOL GetGroupReferences(char* _pBuffer,ULONG _ulSize,std::vector<BIG_KEY>& _vGroup );

protected:
		static EDIA_cl_UPDATEDialog* ms_pProgress;
};
//------------------------------------------------------------------------------
#endif // __DATCUTILS_H__
