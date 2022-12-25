//------------------------------------------------------------------------------
// Filename   :DATCPerforce.cpp
/// \author    stourangeau
/// \date      2005-01-22
/// \par       Description: Implementation of DATCPerforce
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"
#include "DATCPerforce.h"
#include "BIGfiles\BIGfat.h"
// client users
#include "DATCUtils.h"

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
typedef std::vector<DAT_CP4ClientInfo>	tP4ClientInfo;
typedef std::pair<BIG_KEY, BIG_KEY>		tBigKeyPair;
typedef std::vector< tBigKeyPair >		tBigKeyPairVector;
typedef std::vector< BIG_KEY >			tBigKeyVector;

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------
//   int DAT_CPerforce::P4Submit(ULONG _ul_Changelist)
/// \author    YCharbonneau
/// \date      2005-01-06
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
int DAT_CPerforce::VerifyFileLinks(std::vector<DAT_CP4ClientInfo>& vFileList)
{

	// disabling this feature because it seems to cause a great deal of problems
	// 1. when you submit and your map is opened, and try to save, you get a FFFFFFF key message
	// 2. I suspect that it create much problems than it really prevents, memory unload then map saves
	//    will destroy gao references( this has not been reproduced, i think this could be a reason for it) , etc. 
	// 3. To re-enable this feature, we should try to temporary load the files instead of loading 
	//	  and unloading them.

	tP4ClientInfo::iterator		fileIt;
	tBigKeyVector				vLinkedFiles;
	tBigKeyVector::iterator		linkedIt;
	tBigKeyPairVector			vBrokenLinks;
	tBigKeyPairVector::iterator brokenIt;
	bool						incremented;

	// list all linked files
	std::vector< ULONG > lstLinkedKeys;
	for (fileIt = vFileList.begin(); fileIt != vFileList.end(); ++fileIt)
	{
		DAT_CP4ClientInfo& fileInfo = *fileIt;

		vLinkedFiles.clear();	// flush the linked files list
		
		BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(fileInfo.ulKey);

		if ( ulIndex == BIG_C_InvalidIndex ) continue;
		
		DAT_CUtils::GetGroupReferences(ulIndex,vLinkedFiles);

		for (linkedIt = vLinkedFiles.begin(); linkedIt != vLinkedFiles.end(); ++linkedIt)
		{
			BIG_KEY linkedKey = *linkedIt;	
			lstLinkedKeys.push_back( linkedKey );
		}
	}

	// list P4 file names for linked files
	std::vector<std::string> vecStrLinkedFiles;
	for( size_t nFile = 0; nFile < lstLinkedKeys.size(); ++nFile )
	{
		BIG_KEY linkedFileKey = lstLinkedKeys[ nFile ];

		std::string strP4File;
		DAT_CUtils::GetP4FileFromKey( linkedFileKey, strP4File, DAT_CPerforce::GetInstance()->GetP4Root() );
		vecStrLinkedFiles.push_back( strP4File );
	}

	// update stats for all linked files
	if( !vecStrLinkedFiles.empty( ) )
		DAT_CPerforce::GetInstance()->P4Fstat( vecStrLinkedFiles );

	// loop thru all files in our changelist
	for (fileIt = vFileList.begin();
		 fileIt != vFileList.end();
		 fileIt++)
	{
		DAT_CP4ClientInfo& fileInfo = *fileIt;

		vLinkedFiles.clear();	// flush the linked files list

		BIG_INDEX ulIndex = BIG_ul_SearchKeyToFat(fileInfo.ulKey);
		if ( ulIndex == BIG_C_InvalidIndex ) continue;

		DAT_CUtils::GetGroupReferences(ulIndex,vLinkedFiles);

		// Make sure all files referenced exist in p4 or in changelist
		for (linkedIt = vLinkedFiles.begin();
				linkedIt != vLinkedFiles.end();
				linkedIt++)
		{
			BIG_KEY linkedKey= *linkedIt;	
			ULONG	ul_Index;

			ul_Index = BIG_ul_SearchKeyToFat(linkedKey);				
			if (ul_Index == BIG_C_InvalidIndex || BIG_P4RevisionServer(ul_Index) == 0)
			{
				// the file is not already in p4.. 
				vBrokenLinks.push_back(tBigKeyPair(fileInfo.ulKey, linkedKey));
			}
		}
	}

	// We now a vector full of missing links in the p4 server

	// look thru our list of broken links
	for (brokenIt  = vBrokenLinks.begin();
		 brokenIt != vBrokenLinks.end();
		 // do not incr iterator here.. we may delete entries below
		)
	{
		BIG_KEY linkedFileKey = (*brokenIt).second;	// key that is missing

		incremented = false;

		// Check if the file exists but we are not synced
		if (m_mapP4FilesNotInBF.find(linkedFileKey) != m_mapP4FilesNotInBF.end())
		{
			// the linked file was found in the list of unsynched files
			// remove it from out list of missing links
			brokenIt = vBrokenLinks.erase(brokenIt);
			incremented = true;
		}
		else
		{
			// look if these links point to files in this changelist
			// loop thru the files in our changelist
			for (fileIt  = vFileList.begin();
				 fileIt != vFileList.end();
				 fileIt++)
			{
				DAT_CP4ClientInfo& fileInfo = *fileIt;

				if (fileInfo.ulKey == linkedFileKey)
				{
					// the linked file was found inside the changelist
					// remove it from our list of missing links
					brokenIt = vBrokenLinks.erase(brokenIt);
					incremented = true;
				}
			}
		}

		if (!incremented)
			brokenIt++;
	}

	// if we have broken links left in our list, display them to the user
	if (vBrokenLinks.size() > 0)
	{
		char tmpStr[MAX_PATH];

		std::vector<std::string> vstrAdd;
		for (brokenIt  = vBrokenLinks.begin();
			brokenIt != vBrokenLinks.end();
			brokenIt++)
		{
			sprintf(tmpStr, "File with key %08x references the key %08x which is not in your changelist or on perforce.", (*brokenIt).first, (*brokenIt).second);
			DAT_CP4Message::OutputWarningMessage(tmpStr);
			std::string strFilePath;
			DAT_CUtils::GetP4FileFromKey((*brokenIt).second,strFilePath,DAT_CPerforce::GetInstance()->GetP4Root());
			vstrAdd.push_back(strFilePath);
		}

		if ( DAT_CPerforce::GetInstance()->P4Add(vstrAdd) ) 
		{
			DAT_CP4Message::OutputErrorMessage("INFO - Some files where missing and where successfully added to your changelist\n Try to submit again.");
		}
		else if ( DAT_CPerforce::GetInstance()->P4Reopen(vstrAdd,GetWorkingChangelist() ) ) 
		{
			DAT_CP4Message::OutputErrorMessage("INFO - Some files where missing and where successfully reopened to your changelist\n Try to submit again.");
		}

		return 0;
	}

	return 1;
}


