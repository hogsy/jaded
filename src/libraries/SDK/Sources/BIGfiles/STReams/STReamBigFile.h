//------------------------------------------------------------------------------
//   STReamBigFile.h
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Description: 
///            Specification of class ...
/// \see 
//------------------------------------------------------------------------------

#ifndef __STREAMBIGFILE_H__
#define __STREAMBIGFILE_H__

#ifdef ACTIVE_EDITORS

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include <string>

//------------------------------------------------------------------------------
// Preprocessor definitions
//------------------------------------------------------------------------------

#define BFPLATFORM_CURRENT		"PS2"

//------------------------------------------------------------
//  Name   :   STReams_CBigFile
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Description: 
///            No description available ...
/// \see 
//------------------------------------------------------------
class STReams_CBigFile
{
	//--------------------------------------------------------------------------
	// public definitions 
	//--------------------------------------------------------------------------

public:
	STReams_CBigFile () {} ;
	virtual ~STReams_CBigFile () {} ;

	static int Export(const std::string& strFileExportDir, const std::string& strBFDirToExport);
	static int Import(const std::string& strFileImportDir, const std::string& strBFDirToImport);

	//--------------------------------------------------------------------------
	// protected definitions
	//--------------------------------------------------------------------------

protected:

	//--------------------------------------------------------------------------
	// private definitions
	//--------------------------------------------------------------------------

private:

	static ULONG GetKeyFromString(const char* _psz_filename);
	static ULONG GetKeyFromExactString(const char* _psz_filename);
	static void GetPathFromIndex(BIG_INDEX index, char* szPath);
	static void CreateDirectoryRecursive(const char* szDirectory);
	static void CreateDirectoryName(char* szDirectory, BIG_INDEX index);
	static void	ExtractPathNameAndFiledName(std::string& strFileName, char* szPath, char* szName);

	static ULONG UpdateFileBuffer(BIG_KEY _ulKey, void* _pBuffer,ULONG _ulBufferSize, char* _psz_BFFileName);
	static void GetFileBuffer(const std::string& strFilePath, void*& pFileBuffer, ULONG& ulFileBufferSize);
	static void ImportRecurse(const std::string& strPath);

};

#endif // ACTIVE_EDITORS

//------------------------------------------------------------------------------

#endif //#ifndef __STREAMBIGFILE_H__

