/*$T BROframe_vss.cpp GC!1.71 02/08/00 13:59:29 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"
#include "AIFrame.h"

#include "EDItors/Sources/PERForce/PERmsg.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/Perforce/PERCframe.h"

#include "DATaControl/DATCPerforce.h"

#include <assert.h>

//////////////////////////////////////////////////////////////////////////
//

static PER_CDataCtrlEmulator g_DataCtrlEmulator;

//////////////////////////////////////////////////////////////////////////
//
//------------------------------------------------------------
//   void EAI_cl_Frame::OnPerforceEditModel()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EAI_cl_Frame::RunCommandOnFiles( int nCommand )
{
	if ( mul_CurrentEditModel == BIG_C_InvalidIndex ) 
		return;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[BIG_C_MaxLenPath];
	char	asz_Path1[BIG_C_MaxLenPath];
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Name1[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AfxGetApp()->DoWaitCursor(1);
	BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Path);
	L_strcpy(asz_Path1, asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
	L_strcpy(asz_Name1, BIG_NameFile(mul_CurrentEditModel));
	*L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, asz_Name);
	BIG_INDEX ulDir = BIG_ul_SearchDir(asz_Path);
	
	if ( ulDir == BIG_C_InvalidIndex ) 
	{	
		AfxGetApp()->DoWaitCursor(-1);
		return;
	}

	if ( DAT_CPerforce::GetInstance()->P4Connect()  )
	{
		std::vector<BIG_INDEX> vFileIndex;
		std::vector<std::string> vFiles;
		DAT_CUtils::GetP4FilesFromDirIndex(ulDir,vFiles,&vFileIndex,DAT_CPerforce::GetInstance()->GetP4Root().c_str(),TRUE);

		std::string strP4ModelFile;
		DAT_CUtils::GetP4FileFromKey(BIG_FileKey(mul_CurrentEditModel),strP4ModelFile,DAT_CPerforce::GetInstance()->GetP4Root());
		vFiles.push_back(strP4ModelFile);
		vFileIndex.push_back(mul_CurrentEditModel);

		DAT_CPerforce::GetInstance()->P4Fstat(vFiles);
		if ( EPER_cl_Frame::PerforceCheckFileUpToDate(vFileIndex) ) 
		{
			g_DataCtrlEmulator.Clear( );
			for( size_t n = 0; n < vFileIndex.size( ); ++n )
				g_DataCtrlEmulator.AddIndex( vFileIndex[ n ] );
		}


		DAT_CPerforce::GetInstance()->P4Disconnect();

		// as soon as we do a checkout in the ai frame, this option needs to be on 
		// so we don't close the map when we sync/checkout ai file.
		mb_P4CloseWorld = FALSE;
		if( !vFileIndex.empty( ) )
			LINK_SendMessageToEditors(nCommand, ((ULONG)(dynamic_cast<PER_CDataCtrl*>(&g_DataCtrlEmulator))), 0);
		mb_P4CloseWorld = TRUE;

		LINK_SendMessageToEditors(EPER_MESSAGE_REFRESH, -1, 0);
	}

	RefreshDialogBar();
	DisplayPaneNames();
	AfxGetApp()->DoWaitCursor(-1);
}

//------------------------------------------------------------
//   void EAI_cl_Frame::OnPerforceEditModel()
/// \author    YCharbonneau
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EAI_cl_Frame::OnPerforceEditModel()
{
	RunCommandOnFiles(EPER_MESSAGE_EDIT_SELECTED);
}

//------------------------------------------------------------
//   void EAI_cl_Frame::OnPerforceCheckInModel()
/// \author    FFerland
/// \date      2005-03-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EAI_cl_Frame::OnPerforceCheckInModel()
{
	RunCommandOnFiles(EPER_MESSAGE_SUBMIT_SELECTED);
}

//------------------------------------------------------------
//   void EAI_cl_Frame::OnPerforceCheckInOutModel()
/// \author    FFerland
/// \date      2005-03-23
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EAI_cl_Frame::OnPerforceCheckInOutModel()
{
	RunCommandOnFiles(EPER_MESSAGE_SUBMITEDIT_SELECTED);
}

//------------------------------------------------------------
//   void EAI_cl_Frame::OnPerforceRevertModel()
/// \author    FFerland
/// \date      2005-03-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EAI_cl_Frame::OnPerforceRevertModel()
{
	RunCommandOnFiles(EPER_MESSAGE_REVERT_SELECTED);
}

//------------------------------------------------------------
//   void EAI_cl_Frame::OnPerforceSyncModel()
/// \author    FFerland
/// \date      2005-03-21
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void EAI_cl_Frame::OnPerforceSyncModel()
{
	RunCommandOnFiles(EPER_MESSAGE_SYNC_SELECTED);
}

#endif /* ACTIVE_EDITORS */
