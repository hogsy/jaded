//------------------------------------------------------------------------------
// Filename   :PERCheck.cpp
/// \author    Nbeaufils
/// \date      2005-01-06
/// \par       Description: Implementation of EPERCheck_cl_Manager
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "PERCheck.h"

#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"


//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------
EPERCheck_cl_Manager* EPERCheck_cl_Manager::m_pUniqueInstance = NULL;
EPERCheck_DiffMap EPERCheck_cl_Manager::m_mapFolderToFiles;

//------------------------------------------------------------
//   EPERCheck_cl_Manager::EPERCheck_cl_Manager()
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
EPERCheck_cl_Manager::EPERCheck_cl_Manager()
{
}

//------------------------------------------------------------
//   EPERCheck_cl_Manager::~EPERCheck_cl_Manager()
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
EPERCheck_cl_Manager::~EPERCheck_cl_Manager()
{
	EPERCheck_cl_Manager::GetInstance()->ClearDiffStruct();	
}

//------------------------------------------------------------
//   void EPERCheck_cl_Manager::ReleaseInstance()
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Manager::ReleaseInstance()
{
	delete m_pUniqueInstance;
	m_pUniqueInstance = NULL;
}

//------------------------------------------------------------
//   EPERCheck_cl_Manager* EPERCheck_cl_Manager::GetInstance()
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
EPERCheck_cl_Manager* EPERCheck_cl_Manager::GetInstance()
{
	if ( m_pUniqueInstance == NULL )
		m_pUniqueInstance = new EPERCheck_cl_Manager();
	return m_pUniqueInstance;
}

//------------------------------------------------------------
//   EPERCheck_cl_Manager::ClearDiffStruct()
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Manager::ClearDiffStruct()
{
	EPERCheck_DiffMap::iterator IterFolder;
	std::vector<EPERCheck_cl_Diff*>::iterator IterFile;
	
	for ( IterFolder = m_mapFolderToFiles.begin() ; IterFolder != m_mapFolderToFiles.end() ; IterFolder++ )
	{
		for ( IterFile = (IterFolder->second).begin() ; IterFile != (IterFolder->second).end() ; IterFile++ )
		{
			if ( (*IterFile) != NULL )
				delete (*IterFile);
		}
	}
	m_mapFolderToFiles.clear();
}

//------------------------------------------------------------
//   EPERCheck_cl_Manager::FillDiffStruct()
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Manager::FillDiffStruct(BIG_INDEX DirIndex, BOOL bRecursive/*=FALSE*/)
{
	return;
#if 0
	UINT uiIndexDiff = 0;

	std::vector<std::string> vP4FileToDiff;
	std::vector<DAT_CP4ClientInfo*> vP4FileDiff;

	DAT_CUtils::GetP4FilesFromDirIndex(DirIndex, vP4FileToDiff, NULL, DAT_CPerforce::GetInstance()->GetP4Root().c_str(), bRecursive);

	// Query server for all files to diff
	if ( vP4FileToDiff.size() && DAT_CPerforce::GetInstance()->P4Connect() ) 
	{
		DAT_CPerforce::GetInstance()->P4Fstat(vP4FileToDiff);
		DAT_CPerforce::GetInstance()->P4Diff(vP4FileToDiff, &vP4FileDiff);
		DAT_CPerforce::GetInstance()->P4Disconnect();
	}

	// Did the user press escape ?
	if ( LINK_gb_EscapeDetected )
	{
		for ( uiIndexDiff = 0 ; uiIndexDiff < vP4FileDiff.size() ; uiIndexDiff++ )
			delete vP4FileDiff[uiIndexDiff];
		return;
	}

	assert(vP4FileToDiff.size()==vP4FileDiff.size() && "EPERCheck_cl_Manager::FillDiffStruct() - Number of files in dir is different from number of files diffed.");

	for ( uiIndexDiff=0 ; uiIndexDiff < vP4FileDiff.size() ; uiIndexDiff++ )
	{
		DAT_CP4ClientInfoDiff* pP4FileDiff = (DAT_CP4ClientInfoDiff*)vP4FileDiff[uiIndexDiff] ;
		BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(pP4FileDiff->ulKey);
		
		// ------------------------------------------------------------------
		// Insert FOLDER
		std::vector<EPERCheck_cl_Diff*> vFileDiff; 
		std::pair<EPERCheck_DiffMap::iterator, bool> pairInsert = m_mapFolderToFiles.insert(std::make_pair(BIG_ParentFile(ulIndex), vFileDiff));

		EPERCheck_DiffMap::iterator IterFolder = (pairInsert.first) ;

		// ------------------------------------------------------------------
		// Insert FILES
		EPERCheck_cl_Diff* pFileDiff = new EPERCheck_cl_Diff();

		pFileDiff->m_Key = pP4FileDiff->ulKey;
		pFileDiff->m_Index = ulIndex;
		pFileDiff->m_dwFileDiff = pP4FileDiff->dwFileDiff;		

		// On Client
		pFileDiff->m_ClientFileName = BIG_NameFile(ulIndex);
		pFileDiff->m_ClientTime = BIG_TimeFile(ulIndex);
		pFileDiff->m_ClientRevision = BIG_P4RevisionClient(ulIndex);
		pFileDiff->m_ClientSize = BIG_ul_GetLengthFile(BIG_PosFile(ulIndex));

		// On Perforce (if file not new)
		if ( pP4FileDiff->dwFileDiff & (~DAT_CP4ClientInfoDiff::eFileNew) )
		{
			pFileDiff->m_ServerTime = BIG_P4Time(ulIndex);
			pFileDiff->m_ServerRevision = BIG_P4RevisionServer(ulIndex);

			if ( pFileDiff->m_dwFileDiff & DAT_CP4ClientInfoDiff::eFileModified)
				pFileDiff->m_ServerSize = pP4FileDiff->ulFileSizeServer;
			else
				pFileDiff->m_ServerSize = pFileDiff->m_ClientSize;

			if ((pFileDiff->m_dwFileDiff & DAT_CP4ClientInfoDiff::eFileRenamed) || ( pFileDiff->m_dwFileDiff & DAT_CP4ClientInfoDiff::eFileMoved))
				pFileDiff->m_ServerFileName = pP4FileDiff->aszFilenameServer;
			else
				pFileDiff->m_ServerFileName = BIG_NameFile(ulIndex);
		}
		
		(IterFolder->second).push_back(pFileDiff);

		// Did the user press escape ?
		if ( LINK_gb_EscapeDetected )
			break;
	}

	// Clean up diff
	for ( uiIndexDiff = 0 ; uiIndexDiff < vP4FileDiff.size() ; uiIndexDiff++ )
		delete vP4FileDiff[uiIndexDiff];

	// Did the user press escape ?
	if ( LINK_gb_EscapeDetected )
		ClearDiffStruct();
#endif
}

//------------------------------------------------------------
//   EPERCheck_cl_Manager::RemoveDiffStruct()
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Manager::RemoveDiffStruct(std::vector<BIG_INDEX> vDirIndex)
{
	UINT index = 0;
	std::vector<EPERCheck_cl_Diff*>::iterator IterDiff;
	for ( index = 0 ; index < vDirIndex.size() ; index++ )
	{
		BIG_INDEX DirIndex = vDirIndex[index];
		for ( IterDiff = m_mapFolderToFiles[DirIndex].begin() ; IterDiff != m_mapFolderToFiles[DirIndex].end() ; IterDiff++ )
			delete (*IterDiff) ;

		m_mapFolderToFiles.erase(DirIndex);
	}
}

//------------------------------------------------------------
//   EPERCheck_cl_Manager::RemoveDiffStruct()
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
void EPERCheck_cl_Manager::RemoveDiffStruct(BIG_INDEX DirIndex)
{
	BIG_INDEX SubDirIndex = BIG_SubDir(DirIndex);
	while(SubDirIndex != BIG_C_InvalidIndex)
	{
		RemoveDiffStruct(SubDirIndex);
		SubDirIndex = BIG_NextDir(SubDirIndex);
	}

	std::vector<EPERCheck_cl_Diff*>::iterator IterDiff;
	for ( IterDiff = m_mapFolderToFiles[DirIndex].begin() ; IterDiff != m_mapFolderToFiles[DirIndex].end() ; IterDiff++ )
		delete (*IterDiff) ;

	m_mapFolderToFiles.erase(DirIndex);
}

//------------------------------------------------------------
//   EPERCheck_cl_Manager::SortDiffStruct()
/// \author    NBeaufils
/// \date      07-Jan-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
bool ComparePERCheckDiff(EPERCheck_cl_Manager::EPERCheck_cl_Diff* elem1, EPERCheck_cl_Manager::EPERCheck_cl_Diff* elem2)
{
	CString strTmp1(elem1->m_ClientFileName.c_str());
	CString strTmp2(elem2->m_ClientFileName.c_str());
	return strTmp1.MakeUpper() < strTmp2.MakeUpper() ;
}

void EPERCheck_cl_Manager::SortDiffStruct()
{
	EPERCheck_DiffMap::iterator IterFolder;
	for ( IterFolder = m_mapFolderToFiles.begin() ; IterFolder != m_mapFolderToFiles.end() ; IterFolder++ )
		std::sort((IterFolder->second).begin(), (IterFolder->second).end(), ComparePERCheckDiff);
}