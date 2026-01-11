
//------------------------------------------------------------------------------
//   PERCheck.h
/// \author    Nbeaufils
/// \date      2005-01-06
/// \par       Description: 
///            Specification of class EPERCheck_cl_Manager
/// \see 
//------------------------------------------------------------------------------

#ifndef __PERCHECK_H__
#define __PERCHECK_H__

//------------------------------------------------------------------------------
// Header files
//------------------------------------------------------------------------------
#include <map>
#include <vector>
#include <string>

#include "DATaControl/DATCPerforce.h"
#include "DATaControl/DATCUtils.h"

//------------------------------------------------------------------------------
// Typedef definitions 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Preprocessor definitions 
//------------------------------------------------------------------------------
#define NAME_UNAVAILABLE			"n/a"
#define SIZE_UNAVAILABLE			"n/a"
#define TIME_UNAVAILABLE			"n/a"

//------------------------------------------------------------------------------
// Class
//------------------------------------------------------------------------------

class EPERCheck_cl_Manager    
{
public:

	class EPERCheck_cl_Diff
	{
	public:
		EPERCheck_cl_Diff():m_Key(0xFFFFFFFF),m_Index(0xFFFFFFFF),m_dwFileDiff(0),m_ClientSize(-1),m_ClientTime(0xFFFFFFFF),m_ClientRevision(0xFFFFFFFF), m_ServerSize(-1),m_ServerTime(0xFFFFFFFF),m_ServerRevision(0xFFFFFFFF) {};

		BIG_INDEX m_Index;
		BIG_KEY m_Key;
		DWORD m_dwFileDiff;

		std::string m_ClientFileName;
		ULONG m_ClientSize;
		time_t m_ClientTime;
		ULONG m_ClientRevision;

		std::string m_ServerFileName;
		ULONG m_ServerSize;
		time_t m_ServerTime;
		ULONG m_ServerRevision;
	};

	// --- Singleton methods --------------------------
	static EPERCheck_cl_Manager* GetInstance();
	static void ReleaseInstance();

	// --- Diff methods -------------------------------
	void FillDiffStruct(BIG_INDEX DirIndex, BOOL bRecursive=FALSE);
	
	void RemoveDiffStruct(std::vector<BIG_INDEX> vDirIndex);
	void RemoveDiffStruct(BIG_INDEX DirIndex);

	BOOL IsDiffStructEmpty() { return m_mapFolderToFiles.empty() ; } ;
	BOOL IsDiffStructEmpty(BIG_INDEX DirIndex) { return m_mapFolderToFiles[DirIndex].empty() ; } ;

	void ClearDiffStruct();

	void SortDiffStruct();

	std::map< BIG_INDEX, std::vector<EPERCheck_cl_Diff*> >& GetDiffStruct() const { return m_mapFolderToFiles ; } ;
	std::vector<EPERCheck_cl_Diff*>& GetDiffStruct(BIG_INDEX DirIndex) const { return m_mapFolderToFiles[DirIndex] ; } ;
	
private:
	// --- Constructor/Destructor ---------------------
	EPERCheck_cl_Manager();
	~EPERCheck_cl_Manager();

	// --- Singleton instance -------------------------
	static EPERCheck_cl_Manager * m_pUniqueInstance;

	// -- NOTE --
	// First parameter represents a directory index in the BF
	// Second parameter is a vector containing all the files in the directory 
	static std::map< BIG_INDEX, std::vector<EPERCheck_cl_Diff*> > m_mapFolderToFiles;

	void InsertFolderAndFiles(std::vector<BIG_INDEX>& vFilesIndex, std::vector<DAT_CP4ClientInfo*>& vFilesDiff);
};

typedef std::map< BIG_INDEX, std::vector<EPERCheck_cl_Manager::EPERCheck_cl_Diff*> > EPERCheck_DiffMap ;

//------------------------------------------------------------------------------
#endif // __PERCHECK_H__
