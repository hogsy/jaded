/*$T F3Dview_ai.cpp GC! 1.081 06/19/00 14:26:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/EOT/EOTmain.h"
#include "F3Dview.h"
#include "SDK/Sources/TABles/TABles.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "EDIstrings.h"
#include "F3Dstrings.h"
#include "AIinterp/Sources/AIsave.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "EDIpaths.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void F3D_cl_View::DropAIModel(EDI_tdst_DragDrop *_pst_DragDrop)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject				*pst_GO;
	WOR_tdst_World					*pst_World;
	GDI_tdst_Device					*pst_Dev;
	BOOL							b_HasAI;
	AI_tdst_Instance				*pst_Instance;
	BIG_KEY							ul_Key;
	BIG_INDEX						ul_Fat;
	CString							oMsg;
	SOFT_tdst_PickingBuffer_Query	*pst_Query;
	char							asz_Name[BIG_C_MaxLenPath];
	char							asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* For an editor model, check if engine model exists */
	if(BIG_b_IsFileExtension(_pst_DragDrop->ul_FatFile, EDI_Csz_ExtAIEditorModel))
	{
		L_strcpy(asz_Name, BIG_NameFile(_pst_DragDrop->ul_FatFile));
		*L_strrchr(asz_Name, '.') = 0;
		BIG_ComputeFullName(BIG_ParentFile(_pst_DragDrop->ul_FatFile), asz_Path);
		L_strcat(asz_Path, "/");
		L_strcat(asz_Path, asz_Name);
		L_strcat(asz_Name, EDI_Csz_ExtAIEngineModel);
		if(BIG_ul_SearchFileExt(asz_Path, asz_Name) == BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("Engine model does not exists. You must compile model first !", NULL);
			return;
		}
	}

	M_MF()->FatHasChanged();

	/* Find object where to drop ai model */
	pst_Dev = &mst_WinHandles.pst_DisplayData->st_Device;
	ScreenToClient(&_pst_DragDrop->o_Pt);
	pst_Query = &mst_WinHandles.pst_DisplayData->pst_PickingBuffer->st_Query;
	pst_Query->st_Point1.x = (float) _pst_DragDrop->o_Pt.x / (float) pst_Dev->l_Width;
	pst_Query->st_Point1.y = 1.0f - ((float) _pst_DragDrop->o_Pt.y / (float) pst_Dev->l_Height);
	pst_Query->l_Filter = SOFT_Cuc_PBQF_GameObject;
	pst_Query->l_Tolerance = 0;
	pst_GO = (OBJ_tdst_GameObject *) ul_PickExt();

	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	if(ul_Key == BIG_C_InvalidIndex) return;
	pst_World = mst_WinHandles.pst_World;

	/* Object already has an AI ? */
	b_HasAI = TRUE;
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_AI) || (!pst_GO->pst_Extended->pst_Ai))
	{
		b_HasAI = FALSE;

		/* No extended struct, allocate */
		OBJ_GameObject_CreateExtendedIfNot(pst_GO);

		/* Object now has an AI */
		OBJ_SetIdentityFlag(pst_GO, OBJ_C_IdentityFlag_AI);

		/* Sets the Detection List Flag. */
		pst_GO->ul_StatusAndControlFlags |= OBJ_C_StatusFlag_Detection;

		/*
		 * Alloc Detection List and Set Flag. If u remove the following lines, remove also
		 * the COL header.
		 */
		COL_AllocDetectionList(pst_GO);

		/*
		 * We have just allocated a Detection List for that GO. We have to fill it with
		 * the current overlaps of this Object.
		 */
		INT_SnP_UpdateDetectionList(pst_GO, pst_World);
	}
	else
	{
		if(M_MF()->MessageBox(F3D_STR_Csz_ConfirmDelAI, EDI_STR_Csz_TitleConfirm, MB_ICONQUESTION | MB_YESNO) == IDNO)
			return;
		AI_FreeInstance((AI_tdst_Instance *) pst_GO->pst_Extended->pst_Ai);
	}

	/* Create a dummy instance */
	ul_Key = ul_Fat = BIG_ul_SearchKeyToFat(ul_Key);
	ERR_X_Assert(ul_Key != BIG_C_InvalidIndex);
	ul_Key = AI_ul_CreateInstance(pst_World, ul_Key, _pst_DragDrop->ul_FatFile, BIG_NameFile(ul_Key));

	/* Load the instance */
	OBJ_gpst_ObjectToLoadIn = pst_GO;
	pst_Instance = AI_pst_LoadInstance(ul_Key);
	pst_GO->pst_Extended->pst_Ai = pst_Instance;
	pst_Instance->pst_GameObject = pst_GO;

	/* Add a message */
	oMsg = F3D_STR_Csz_DropAIModel;
	oMsg += BIG_NameFile(ul_Fat);
	LINK_PrintStatusMsg((char *) (LPCSTR) oMsg);

	/* Update GO */
	LINK_UpdatePointer(pst_GO);
	LINK_UpdatePointers();
}

#endif
