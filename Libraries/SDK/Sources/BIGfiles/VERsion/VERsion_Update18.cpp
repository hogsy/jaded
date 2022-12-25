/*$T VERsion_Update18.cpp GC! 1.081 04/21/00 17:16:38 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_file.h"

typedef struct	ACT_st_ActionItem_Old_
{
	struct EVE_tdst_ListTracks_	*pst_TrackList;
	UCHAR						uc_Repetition;
	UCHAR						uc_NumberOfFrameForBlend;
	UCHAR						uc_Frequency;
	UCHAR						uc_CustomBit;
	USHORT						uw_DesignFlags;
	UCHAR						uc_Flag;
	UCHAR						uc_Dummy;
} ACT_st_ActionItem_Old;

typedef struct	ACT_st_Action_Old_
{
	UCHAR					uc_NumberOfActionItem;
	UCHAR					uc_ActionItemNumberForLoop;
	USHORT					uw_Counter;
	ACT_st_ActionItem_Old	ast_ActionItem[ACT_C_SizeOfAction];
} ACT_st_Action_Old;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void VERsion_UpdateOneFileVersion18(char *_pc_Buf, ULONG _ul_Len, char *_pz_Path, char *_pz_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*pz_Ext;
	ACT_st_Action_Old	*pst_ActionOld;
	ACT_st_Action		*pst_Action;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pz_Ext = L_strrchr(_pz_File, '.');
	if(!pz_Ext || L_stricmp(pz_Ext, EDI_Csz_ExtAction) != 0) return;
	if(!_ul_Len) return;
	pst_ActionOld = (ACT_st_Action_Old *) _pc_Buf;

	pst_Action = (ACT_st_Action *) L_malloc(sizeof(ACT_st_Action));
	pst_Action->uc_NumberOfActionItem = pst_ActionOld->uc_NumberOfActionItem;
	pst_Action->uc_ActionItemNumberForLoop = pst_ActionOld->uc_ActionItemNumberForLoop;
	pst_Action->uw_Counter = pst_ActionOld->uw_Counter;
	for(i = 0; i < pst_ActionOld->uc_NumberOfActionItem; i++)
	{
		pst_Action->ast_ActionItem[i].pst_TrackList = pst_ActionOld->ast_ActionItem[i].pst_TrackList;
		pst_Action->ast_ActionItem[i].pst_Transitions = NULL;
		pst_Action->ast_ActionItem[i].uc_Repetition = pst_ActionOld->ast_ActionItem[i].uc_Repetition;
		pst_Action->ast_ActionItem[i].uc_NumberOfFrameForBlend = pst_ActionOld->ast_ActionItem[i].uc_NumberOfFrameForBlend;
		pst_Action->ast_ActionItem[i].uc_Frequency = pst_ActionOld->ast_ActionItem[i].uc_Frequency;
		pst_Action->ast_ActionItem[i].uc_CustomBit = pst_ActionOld->ast_ActionItem[i].uc_CustomBit;
		pst_Action->ast_ActionItem[i].uw_DesignFlags = pst_ActionOld->ast_ActionItem[i].uw_DesignFlags;
		pst_Action->ast_ActionItem[i].uc_Flag = pst_ActionOld->ast_ActionItem[i].uc_Flag;
		pst_Action->ast_ActionItem[i].uc_Dummy = pst_ActionOld->ast_ActionItem[i].uc_Dummy;
	}

	BIG_UpdateFileFromBuffer(_pz_Path, _pz_File, (UCHAR *) pst_Action, sizeof(ACT_st_Action));
}

#endif /* ACTIVE_EDITORS */
