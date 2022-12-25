/*$T AIframe_act.cpp GC! 1.100 03/08/01 14:11:21 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "AIframe.h"
#include "AIframe_act.h"
#include "EditView/AIview.h"
#include "EditView/AIleftview.h"
#include "AIstrings.h"
#include "EDIstrings.h"
#include "EDImainframe.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "MENu/MENin.h"
#include "Res/Res.h"
#include "AIinterp/Sources/AIdebug.h"
#include "EDIapp.h"
#include "AIleftframe.h"
#include "LINKs/LINKtoed.h"
#include "EDIpaths.h"
#include "DIAlogs/DIAlist_dlg.h"
#include "EDImsg.h"
#include "DIAlogs\DIAfindfile_dlg.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "DATaControl/DATCPerforce.h"
#include "EDItors\Sources\PERForce\PERmsg.h"
#include "EDItors\Sources\PERForce\PERCframe.h"

extern BOOL	EDI_gb_CheckInDel;
extern BOOL AI2C_gb_Activated;
extern BOOL AI_gb_Compile1;
extern BOOL	EDI_can_lock;
#ifdef JADEFUSION
extern void AI_DelAllWatch(void);
#else
extern "C" void AI_DelAllWatch(void);
#endif
extern CMapPtrToPtr	AI_go_CommonList;
extern BOOL EDI_gb_NoUpdateVSS;

/*
 =======================================================================================================================
 =======================================================================================================================
 */

static PER_CDataCtrlEmulator g_DataCtrlEmul;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL VssOther(char *path, BIG_INDEX index, char *name)
{
	L_strcpy(name, BIG_NameFile(index));
	*L_strrchr(name, '.') = 0;
	if(!L_strcmpi(L_strrchr(BIG_NameFile(index), '.'), ".fct"))
		L_strcat(name, ".ofc");
	else if(!L_strcmpi(L_strrchr(BIG_NameFile(index), '.'), ".var"))
		L_strcat(name, ".ova");
	else if(!L_strcmpi(L_strrchr(BIG_NameFile(index), '.'), ".fcl"))
		L_strcat(name, ".fce");
	if(BIG_ul_SearchFileExt(path, name) != BIG_C_InvalidIndex) return TRUE;
	return FALSE;
}

void EAI_cl_Frame::SendP4Message( ULONG ulMessage, EAI_cl_LeftFrame* pLeftFrame, ULONG ulCurrentEditFile )
{
	PER_CDataCtrl* _pItemCtrl = dynamic_cast<PER_CDataCtrl*>(pLeftFrame);
	if( _pItemCtrl->GetFirstSelectedItem() != 0xFFFFFFFF )
		LINK_SendMessageToEditors( ulMessage, (ULONG)_pItemCtrl, 0);
	else
	{
		g_DataCtrlEmul.Clear( );
		g_DataCtrlEmul.AddIndex( ulCurrentEditFile );
		LINK_SendMessageToEditors( ulMessage, (ULONG)&g_DataCtrlEmul, 0);
	}
	RefreshDialogBar();
	DisplayPaneNames();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnAction(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					iLine, iSel, iNext, iCur;
	EDIA_cl_ListDialog	o_Dlg(EDIA_List_SpeedSelectMDL);
	EDIA_cl_ListDialog	o_Dlg1(EDIA_List_SpeedSelectFCT);
	EDIA_cl_ListDialog	o_Dlg2(EDIA_List_SpeedSelectHistoryFCT);
	EDIA_cl_ListDialog	o_Dlg3(EDIA_List_SpeedSelectGlob);
	EDIA_cl_ListDialog	o_Dlg4(EDIA_List_CallStack);
	int					ul_Mod, ul_Ins;
	POSITION			pos, pos1;
	CHARRANGE			cr;
	BIG_INDEX			ul_File;
	char				asz_Path[512];
	char				az_Name[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mb_P4CloseWorld = FALSE;

	switch(_ul_Action)
	{
	case EAI_ACTION_UNIVERSE:
		if(BIG_UniverseKey() != BIG_C_InvalidKey)
		{
			ul_File = BIG_ul_SearchKeyToFat(BIG_UniverseKey());
			i_OnMessage(EDI_MESSAGE_SELFILE, 0, ul_File);
		}

		break;

	case EAI_ACTION_AI2C:
		AI2C_gb_Activated = AI2C_gb_Activated ? FALSE : TRUE;
		break;

	case EAI_ACTION_SPEEDSELECTMDL:
		o_Dlg.mpo_AIFrame = this;
		o_Dlg.mul_Current = mul_CurrentEditModel;
		o_Dlg.DoModal();
		break;

	case EAI_ACTION_SPEEDSELECTGLOB:
		o_Dlg3.mpo_AIFrame = this;
		o_Dlg3.DoModal();
		break;

	case EAI_ACTION_SPEEDSELECTFCT:
		o_Dlg1.mpo_AIFrame = this;
		o_Dlg1.mul_Model = mul_CurrentEditModel;
		o_Dlg1.mul_Current = mul_CurrentEditFile;
		o_Dlg1.DoModal();
		break;

	case EAI_ACTION_SPEEDSELECTHISFCT:
		o_Dlg2.mpo_AIFrame = this;
		o_Dlg2.DoModal();
		break;

	case EAI_ACTION_SAVE:
		OnSave();
		break;

	case EAI_ACTION_WARNINGHIGH:
		mst_Ini.b_WarningHigh = mst_Ini.b_WarningHigh ? FALSE : TRUE;
		break;

	case EAI_ACTION_TOGGLEMARK:
		pos = mo_History.Find(mul_CurrentEditFile);
		if(pos)
		{
			iSel = 0;
			pos1 = mo_History.GetHeadPosition();
			while(pos != pos1)
			{
				mo_History.GetNext(pos1);
				iSel++;
			}

			mpo_Edit->GetSel(cr);
			iLine = mpo_Edit->LineFromChar(cr.cpMax);

			pos = mo_HistoryMarks[iSel].Find(iLine);
			if(pos)
				mo_HistoryMarks[iSel].RemoveAt(pos);
			else
				mo_HistoryMarks[iSel].AddTail(iLine);
			mpo_LeftView->Invalidate();
		}
		break;

	case EAI_ACTION_NEXTMARK:
		pos = mo_History.Find(mul_CurrentEditFile);
		if(pos)
		{
			iSel = 0;
			pos1 = mo_History.GetHeadPosition();
			while(pos != pos1)
			{
				mo_History.GetNext(pos1);
				iSel++;
			}

			mpo_Edit->GetSel(cr);
			iLine = mpo_Edit->LineFromChar(cr.cpMax);

			iCur = -1;
			pos = mo_HistoryMarks[iSel].GetHeadPosition();
			while(pos)
			{
				iNext = mo_HistoryMarks[iSel].GetNext(pos);
				if(iNext > iLine && (iNext < iCur || iCur == -1)) iCur = iNext;
			}

			if(iCur == -1)
			{
				pos = mo_HistoryMarks[iSel].GetHeadPosition();
				while(pos)
				{
					iNext = mo_HistoryMarks[iSel].GetNext(pos);
					if((iNext < iCur) || (iCur == -1)) iCur = iNext;
				}
			}

			iCur = mpo_Edit->LineIndex(iCur);
			cr.cpMin = cr.cpMax = iCur;
			mpo_Edit->SetSel(cr);
		}
		break;

	case EAI_ACTION_COMPILE2LOG:
		mo_Compiler.mb_CompileToLog = TRUE;
		OnSave();
		OnCompileCurrentFile();
		mo_Compiler.mb_CompileToLog = FALSE;
		break;

	case EAI_ACTION_COMPILE:
		OnSave();
		OnCompileCurrentFile();
		LINK_Refresh();
		mpo_Edit->mb_FirstColoring = TRUE;	/* Force coloration */
		mpo_Edit->mi_LastColLine = -1;
		break;

	case EAI_ACTION_MODELCOMPILE:
		AI2C_Reinit();
		SetCompile1();
		OnSave();
		OnCompileCurrentModel();
		LINK_Refresh();
		mpo_Edit->mb_FirstColoring = TRUE;	/* Force coloration */
		mpo_Edit->mi_LastColLine = -1;
		break;

	case EAI_ACTION_LOOKDATE:
		AI_go_CommonList.RemoveAll();
		mo_ModelDateList.RemoveAll();
		AI_gb_Compile1 = FALSE;
		mst_Ini.b_LookDate = mst_Ini.b_LookDate ? FALSE : TRUE;
		break;

	case EAI_ACTION_AIRASTERS:
		AI_gb_FullRasters = AI_gb_FullRasters ? FALSE : TRUE;
		break;

	case EAI_ACTION_COMPILEALL:
		AI2C_Reinit();
		AI_gb_Compile1 = mst_Ini.b_LookDate;
		mst_Ini.b_LookDate = FALSE;
		OnSave();
		OnCompileAllModels();
		LINK_Refresh();
		mpo_Edit->mb_FirstColoring = TRUE;	/* Force coloration */
		mpo_Edit->mi_LastColLine = -1;
		mpo_Edit->Invalidate();
		mst_Ini.b_LookDate = AI_gb_Compile1;
		break;

	case EAI_ACTION_MODELCOMPILEDEP:
		AI2C_Reinit();
		SetCompile1();
		OnSave();
		OnCompileCurrentModelDep();
		LINK_Refresh();
		mpo_Edit->mb_FirstColoring = TRUE;	/* Force coloration */
		mpo_Edit->mi_LastColLine = -1;
		break;

	case EAI_ACTION_CREATEINSTANCE:
		OnCreateInstance();
		break;

	case EAI_ACTION_REINITINSTANCE:
		OnReinitInstance();
		break;

	case EAI_ACTION_NEWMODEL:
		OnNewModel();
		break;

	case EAI_ACTION_NEWFUNCTION:
		OnNewFunction();
		break;

	case EAI_ACTION_NEWPROCLIST:
		OnNewProcList();
		break;

	case EAI_ACTION_NEWVARS:
		OnNewVars();
		break;

	case EAI_ACTION_ADDFUNCTION:
		OnAddFunction();
		break;

	case EAI_ACTION_DELETE:
		OnDeleteFile();
		break;

	case EAI_ACTION_RENAMEFUNC:
		OnRenameFile();
		break;
		
	case EAI_ACTION_SHOWINBROWSER:
		if  (mul_CurrentEditFile != BIG_C_InvalidIndex)
		{
			EBRO_cl_Frame   *po_Browser;
			po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
            po_Browser->i_OnMessage( EDI_MESSAGE_SELFILE, BIG_ParentFile( mul_CurrentEditFile ), mul_CurrentEditFile );
		}
		break;

	case EAI_ACTION_GLOBFCT:
		OnNewGlobalFunction();
		break;

	case EAI_ACTION_MODELCLOSE:
		if(OnClose())
		{
			mul_CurrentEditModel = BIG_C_InvalidIndex;
			mo_ModelDateList.RemoveAll();
			mpo_DialogBar->GetDlgItem(IDC_INFOS)->SetWindowText("");
		}
		break;

	case EAI_ACTION_BREAKPOINT:
		OnBreakPoint();
		break;

	case EAI_ACTION_ONESTEPOUT:
		OnOneStep(FALSE);
		break;

	case EAI_ACTION_ONESTEPIN:
		OnOneStep(TRUE);
		break;

	case EAI_ACTION_OUTPROC:
		AI_gi_OneStepMode = 3;
		EDI_go_TheApp.mb_RunEngine = !EDI_go_TheApp.mb_RunEngine;
		if(EDI_go_TheApp.mb_RunEngine == FALSE) ENG_gb_ForceEndEngine = TRUE;
		break;

	case EAI_ACTION_SYSBREAK:
		OnSysBreak();
		break;

	case EAI_ACTION_SYSBREAKON:
		AI_gb_CanBreakSystem = AI_gb_CanBreakSystem ? FALSE : TRUE;
		break;

	case EAI_ACTION_BREAKON:
		AI_gb_CanBreak = AI_gb_CanBreak ? FALSE : TRUE;
		mst_Ini.b_BreakEnable = AI_gb_CanBreak;
		mpo_LeftView->Invalidate();
		break;

	case EAI_ACTION_DELBKP:
		AfxGetApp()->DoWaitCursor(1);
		AI_InitBreakList();
		OnDeleteAllBkp();
		mpo_LeftView->Invalidate();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_CALLSTACK:
		o_Dlg4.mpo_AIFrame = this;
		o_Dlg4.DoModal();
		break;

	case EAI_ACTION_DELALLWATCH:
		AI_DelAllWatch();
		break;

	case EAI_ACTION_GOTONEXTCPT:
		pos = mo_History.Find(mul_CurrentEditFile);
		if(!pos) break;
		mo_History.GetNext(pos);
		if(!pos) pos = mo_History.GetHeadPosition();
		iSel = mo_History.GetAt(pos);
		Load(iSel);
		break;

	case EAI_ACTION_UPDATEOWNER:
		UpdateOwner();
		break;

	case EAI_ACTION_CHECKIN:
		AfxGetApp()->DoWaitCursor(1);
		OnSave();
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditFile), asz_Path);
		EDI_gb_CheckInDel = FALSE;
		if(M_MF()->b_LockBigFile())
		{
			EDI_can_lock = FALSE;
			M_MF()->CheckInFile(asz_Path, BIG_NameFile(mul_CurrentEditFile));
			if(VssOther(asz_Path, mul_CurrentEditFile, az_Name))
				M_MF()->CheckInFile(asz_Path, az_Name);
			EDI_can_lock = TRUE;
			M_MF()->UnLockBigFile();
		}

		EDI_gb_NoUpdateVSS = TRUE;
		UpdateOwner();
		LINK_gb_RefreshOnlyFiles = TRUE;
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_CHECKOUT:
		AfxGetApp()->DoWaitCursor(1);
		OnSave();
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditFile), asz_Path);
		if(M_MF()->b_LockBigFile())
		{
			EDI_can_lock = FALSE;
			M_MF()->CheckOutFile(asz_Path, BIG_NameFile(mul_CurrentEditFile));
			if(VssOther(asz_Path, mul_CurrentEditFile, az_Name))
				M_MF()->CheckOutFile(asz_Path, az_Name);
			EDI_can_lock = TRUE;
			M_MF()->UnLockBigFile();
		}
		EDI_gb_NoUpdateVSS = TRUE;
		UpdateOwner();
		LINK_gb_RefreshOnlyFiles = TRUE;
		M_MF()->FatHasChanged();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_CHECKINOUT:
		AfxGetApp()->DoWaitCursor(1);
		OnSave();
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditFile), asz_Path);
		EDI_gb_CheckInDel = FALSE;
		if(M_MF()->b_LockBigFile())
		{
			EDI_can_lock = FALSE;
			M_MF()->CheckInFileNoOut(asz_Path, BIG_NameFile(mul_CurrentEditFile));
			if(VssOther(asz_Path, mul_CurrentEditFile, az_Name))
				M_MF()->CheckInFileNoOut(asz_Path, az_Name);
			EDI_can_lock = TRUE;
			M_MF()->UnLockBigFile();
		}

		EDI_gb_NoUpdateVSS = TRUE;
		UpdateOwner();
		LINK_gb_RefreshOnlyFiles = TRUE;
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_GETLATESTFILE:
		AfxGetApp()->DoWaitCursor(1);
		OnSave();
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditFile), asz_Path);
		if(M_MF()->b_LockBigFile())
		{
			EDI_can_lock = FALSE;
			M_MF()->GetLatestVersionFile(asz_Path, BIG_NameFile(mul_CurrentEditFile));
			if(VssOther(asz_Path, mul_CurrentEditFile, az_Name))
				M_MF()->GetLatestVersionFile(asz_Path, az_Name);
			EDI_can_lock = TRUE;
			M_MF()->UnLockBigFile();
		}

		EDI_gb_NoUpdateVSS = TRUE;
		LINK_gb_RefreshOnlyFiles = TRUE;
		UpdateOwner();
		M_MF()->FatHasChanged();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_UNDOCHECKOUTFILE:
		AfxGetApp()->DoWaitCursor(1);
		OnSave();
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditFile), asz_Path);
		if(M_MF()->b_LockBigFile())
		{
			EDI_can_lock = FALSE;
			M_MF()->UndoCheckOutFile(asz_Path, BIG_NameFile(mul_CurrentEditFile));
			if(VssOther(asz_Path, mul_CurrentEditFile, az_Name))
				M_MF()->UndoCheckOutFile(asz_Path, az_Name);
			EDI_can_lock = TRUE;
			M_MF()->UnLockBigFile();
		}

		EDI_gb_NoUpdateVSS = TRUE;
		LINK_gb_RefreshOnlyFiles = TRUE;
		UpdateOwner();
		AfxGetApp()->DoWaitCursor(-1);
		break;
		
	case EAI_ACTION_UNDOCHECKOUT:
		OnSave();
		UndoCheckOut();
		break;

	case EAI_ACTION_CHECKOUTMODEL:
		OnSave();
		ul_Mod = mul_CurrentEditModel;
		ul_Ins = mul_CurrentEditInstance;
		OnClose(TRUE, TRUE);
		mul_CurrentEditModel = ul_Mod;
		mo_ModelDateList.RemoveAll();
		mul_CurrentEditInstance = ul_Ins;

		CheckOut();

		EDI_gb_NoUpdateVSS = TRUE;
		mo_ModelDateList.RemoveAll();
		mul_CurrentEditModel = BIG_C_InvalidIndex;
		mul_CurrentEditInstance = BIG_C_InvalidIndex;
		if(ul_Ins != BIG_C_InvalidIndex)
			SelFile(ul_Ins);
		else
			SelFile(ul_Mod);
		EDI_gb_NoUpdateVSS = FALSE;
		break;

	case EAI_ACTION_P4_EDITMODEL:
		OnSave();
		ul_Mod = mul_CurrentEditModel;
		ul_Ins = mul_CurrentEditInstance;
		OnClose(TRUE, TRUE);
		mul_CurrentEditModel = ul_Mod;
		mo_ModelDateList.RemoveAll();
		mul_CurrentEditInstance = ul_Ins;

		AfxGetApp()->DoWaitCursor(1);
		OnPerforceEditModel();

		mo_ModelDateList.RemoveAll();
		mul_CurrentEditModel = BIG_C_InvalidIndex;
		mul_CurrentEditInstance = BIG_C_InvalidIndex;
		if(ul_Ins != BIG_C_InvalidIndex)
			SelFile(ul_Ins);
		else
		if(ul_Mod != BIG_C_InvalidIndex)
			SelFile(ul_Mod);

		LINK_gb_RefreshOnlyFiles = TRUE;
		M_MF()->FatHasChanged();
		AfxGetApp()->DoWaitCursor(-1);
		break;
	
	case EAI_ACTION_P4_EDIT:
		AfxGetApp()->DoWaitCursor(1);
		SendP4Message(EPER_MESSAGE_EDIT_SELECTED, mpo_ListAI, mul_CurrentEditFile);
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_P4_REVERT:
		AfxGetApp()->DoWaitCursor(1);
		SendP4Message(EPER_MESSAGE_REVERT_SELECTED, mpo_ListAI, mul_CurrentEditFile);
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_P4_REVERTMODEL:
		OnSave();
		ul_Mod = mul_CurrentEditModel;
		ul_Ins = mul_CurrentEditInstance;
		OnClose(TRUE, TRUE);
		mul_CurrentEditModel = ul_Mod;
		mo_ModelDateList.RemoveAll();
		mul_CurrentEditInstance = ul_Ins;

		AfxGetApp()->DoWaitCursor(1);
		OnPerforceRevertModel();

		mo_ModelDateList.RemoveAll();
		mul_CurrentEditModel = BIG_C_InvalidIndex;
		mul_CurrentEditInstance = BIG_C_InvalidIndex;
		if(ul_Ins != BIG_C_InvalidIndex)
			SelFile(ul_Ins);
		else
		if(ul_Mod != BIG_C_InvalidIndex)
			SelFile(ul_Mod);

		LINK_gb_RefreshOnlyFiles = TRUE;
		M_MF()->FatHasChanged();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_P4_SYNC:
		AfxGetApp()->DoWaitCursor(1);
		SendP4Message(EPER_MESSAGE_SYNC_SELECTED, mpo_ListAI, mul_CurrentEditFile);
		AfxGetApp()->DoWaitCursor(-1);

		break;

	case EAI_ACTION_P4_SYNCMODEL:
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceSyncModel();
		AfxGetApp()->DoWaitCursor(-1);

		break;

	case EAI_ACTION_P4_ADD:
		AfxGetApp()->DoWaitCursor(1);
		SendP4Message(EPER_MESSAGE_ADD_SELECTED, mpo_ListAI, mul_CurrentEditFile);
		AfxGetApp()->DoWaitCursor(-1);

		break;

	case EAI_ACTION_P4_DELETE:
		AfxGetApp()->DoWaitCursor(1);
		SendP4Message(EPER_MESSAGE_DELETE_SELECTED, mpo_ListAI, mul_CurrentEditFile);
		AfxGetApp()->DoWaitCursor(-1);

		break;

	case EAI_ACTION_P4_SUBMIT:
		AfxGetApp()->DoWaitCursor(1);
		SendP4Message(EPER_MESSAGE_SUBMIT_SELECTED, mpo_ListAI, mul_CurrentEditFile);
		AfxGetApp()->DoWaitCursor(-1);

		break;

	case EAI_ACTION_P4_SUBMITEDIT:
		AfxGetApp()->DoWaitCursor(1);
		SendP4Message(EPER_MESSAGE_SUBMITEDIT_SELECTED, mpo_ListAI, mul_CurrentEditFile);
		AfxGetApp()->DoWaitCursor(-1);

		break;

	case EAI_ACTION_P4_DIFF		:
		SendP4Message(EPER_MESSAGE_DIFF_SELECTED, mpo_ListAI, mul_CurrentEditFile);
		break;
	case EAI_ACTION_P4_SHOWHISTORY:
		SendP4Message(EPER_MESSAGE_SHOWHISTORY_SELECTED, mpo_ListAI, mul_CurrentEditFile);
		break;

	case EAI_ACTION_CHECKINMODEL:
		OnSave();
		CheckIn();
		break;

	case EAI_ACTION_P4_CHECKINMODEL:
		OnSave();
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceCheckInModel();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_P4_CHECKINOUTMODEL:
		OnSave();
		AfxGetApp()->DoWaitCursor(1);
		OnPerforceCheckInOutModel();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EAI_ACTION_CHECKINOUTMODEL:
		OnSave();

		ul_Mod = mul_CurrentEditModel;
		ul_Ins = mul_CurrentEditInstance;
		OnClose(TRUE, TRUE);
		mul_CurrentEditModel = ul_Mod;
		mo_ModelDateList.RemoveAll();
		mul_CurrentEditInstance = ul_Ins;

		CheckInOut();

		EDI_gb_NoUpdateVSS = TRUE;
		mo_ModelDateList.RemoveAll();
		mul_CurrentEditModel = BIG_C_InvalidIndex;
		mul_CurrentEditInstance = BIG_C_InvalidIndex;
		if(ul_Ins != BIG_C_InvalidIndex)
			SelFile(ul_Ins);
		else
			SelFile(ul_Mod);
		EDI_gb_NoUpdateVSS = FALSE;
		break;

	case EAI_ACTION_GETLATESTMODEL:
		OnSave();

		ul_Mod = mul_CurrentEditModel;
		ul_Ins = mul_CurrentEditInstance;
		OnClose(TRUE, TRUE);
		mul_CurrentEditModel = ul_Mod;
		mo_ModelDateList.RemoveAll();
		mul_CurrentEditInstance = ul_Ins;

		GetLatest();

		EDI_gb_NoUpdateVSS = TRUE;
		mo_ModelDateList.RemoveAll();
		mul_CurrentEditModel = BIG_C_InvalidIndex;
		mul_CurrentEditInstance = BIG_C_InvalidIndex;
		if(ul_Ins != BIG_C_InvalidIndex)
			SelFile(ul_Ins);
		else
			SelFile(ul_Mod);
		EDI_gb_NoUpdateVSS = FALSE;
		break;

	case EAI_ACTION_FIND:
		OnFind();
		break;

	case EAI_ACTION_FINDFILES:
		go_FindFileDialog.mb_FindInFiles = TRUE;
		go_FindFileDialog.DoModal();
		go_FindFileDialog.mb_FindInFiles = FALSE;
		break;

	case EAI_ACTION_FINDNEXT:
		OnFindNext();
		break;

	case EAI_ACTION_MATCH:
		OnMatch();
		break;
	}

	mb_P4CloseWorld = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EAI_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~*/
	UINT	ui_State;
	/*~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;

	switch(_ul_Action)
	{
	case EAI_ACTION_SYSBREAKON:
		ui_State = DFCS_BUTTONCHECK;
		if(AI_gb_CanBreakSystem) ui_State |= DFCS_CHECKED;
		break;

	case EAI_ACTION_BREAKON:
		ui_State = DFCS_BUTTONCHECK;
		if(AI_gb_CanBreak) ui_State |= DFCS_CHECKED;
		break;

	case EAI_ACTION_AI2C:
		ui_State = DFCS_BUTTONCHECK;
		if(AI2C_gb_Activated) ui_State |= DFCS_CHECKED;
		break;

	case EAI_ACTION_WARNINGHIGH:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.b_WarningHigh) ui_State |= DFCS_CHECKED;
		break;

	case EAI_ACTION_LOOKDATE:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.b_LookDate) ui_State |= DFCS_CHECKED;
		break;

	case EAI_ACTION_AIRASTERS:
		ui_State = DFCS_BUTTONCHECK;
		if(AI_gb_FullRasters) ui_State |= DFCS_CHECKED;
		break;
	}

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern BOOL EDI_gb_BatchModeWithoutPerforce;

BOOL EAI_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	switch(_ul_Action)
	{
	case EAI_ACTION_BREAKPOINT:
		ConnectToEngine();
		if(mul_CurrentEditFile == BIG_C_InvalidIndex) return FALSE;
		if(!mpst_Model) return FALSE;
		if(BIG_b_IsFileExtension(mul_CurrentEditFile, EDI_Csz_ExtAIEditorFct))
		{
			if(mo_Compiler.ul_GetEngineFileForForFunction(BIG_FileKey(mul_CurrentEditFile)) == BIG_C_InvalidIndex)
				return FALSE;
		}
		if(BIG_b_IsFileExtension(mul_CurrentEditFile, EDI_Csz_ExtAIEditorFctLib))
		{
			if(mo_Compiler.ul_GetEngineFileForForProcList(BIG_FileKey(mul_CurrentEditFile)) == BIG_C_InvalidIndex)
				return FALSE;
		}
		break;

	case EAI_ACTION_ONESTEPOUT:
	case EAI_ACTION_ONESTEPIN:
		ConnectToEngine();
		if(!mpst_Model) return FALSE;
		if(!AI_gb_ExitByBreak) return FALSE;
		break;

	case EAI_ACTION_SAVE:
	case EAI_ACTION_COMPILE:
	case EAI_ACTION_COMPILE2LOG:
	case EAI_ACTION_TOGGLEMARK:
	case EAI_ACTION_NEXTMARK:
		if(mul_CurrentEditFile == BIG_C_InvalidIndex) return FALSE;
		break;

	case EAI_ACTION_SPEEDSELECTFCT:
		if(mul_CurrentEditModel == BIG_C_InvalidIndex) return FALSE;
		break;

	case EAI_ACTION_SPEEDSELECTHISFCT:
		if(!mo_History.GetCount()) return FALSE;
		break;

	case EAI_ACTION_NEWFUNCTION:
	case EAI_ACTION_NEWPROCLIST:
	case EAI_ACTION_NEWVARS:
	case EAI_ACTION_MODELCLOSE:
	case EAI_ACTION_MODELCOMPILE:
	case EAI_ACTION_MODELCOMPILEDEP:
	case EAI_ACTION_CREATEINSTANCE:
	case EAI_ACTION_ADDFUNCTION:
		if(mul_CurrentEditModel == BIG_C_InvalidIndex) return FALSE;
		break;

	case EAI_ACTION_GOTONEXTCPT:
		if(mo_History.GetCount() <= 1) return FALSE;
		break;

	case EAI_ACTION_DELETE:
	case EAI_ACTION_RENAMEFUNC:
	case EAI_ACTION_SHOWINBROWSER:
		if(mul_CurrentEditModel == BIG_C_InvalidIndex) return FALSE;
		if(mul_CurrentEditFile == BIG_C_InvalidIndex) return FALSE;
		break;

	case EAI_ACTION_REINITINSTANCE:
		ConnectToEngine();
		if(!mpst_Instance) return FALSE;
		if(mul_CurrentEditModel == BIG_C_InvalidIndex) return FALSE;
		if(mul_CurrentEditInstance == BIG_C_InvalidIndex) return FALSE;
		break;

	case EAI_ACTION_FINDNEXT:
		if(mo_SearchString.IsEmpty()) return FALSE;
	case EAI_ACTION_FIND:
	case EAI_ACTION_MATCH:
		if(mul_CurrentEditFile == BIG_C_InvalidIndex) return FALSE;
		return TRUE;

	case EAI_ACTION_UNDOCHECKOUT:
	case EAI_ACTION_CHECKOUTMODEL:
	case EAI_ACTION_CHECKINMODEL:
	case EAI_ACTION_CHECKINOUTMODEL:
	case EAI_ACTION_GETLATESTMODEL:
	case EAI_ACTION_UPDATEOWNER:
		if(!M_MF()->mst_Ini.b_LinkControlON) return FALSE;
		if(mul_CurrentEditModel == BIG_C_InvalidIndex) return FALSE;
		return TRUE;


    case EAI_ACTION_P4_ADD:
    case EAI_ACTION_P4_DELETE:
    case EAI_ACTION_P4_EDIT:
    case EAI_ACTION_P4_REVERT:
    case EAI_ACTION_P4_SYNC:
    case EAI_ACTION_P4_EDITMODEL:
    case EAI_ACTION_P4_DIFF:
    case EAI_ACTION_P4_SHOWHISTORY:
    case EAI_ACTION_P4_SUBMIT:
    case EAI_ACTION_P4_CHECKINMODEL:
    case EAI_ACTION_P4_REVERTMODEL:
    case EAI_ACTION_P4_SYNCMODEL:
    case EAI_ACTION_P4_SUBMITEDIT:
    case EAI_ACTION_P4_CHECKINOUTMODEL:
    case EAI_ACTION_P4_FORCESYNC:
        if (EDI_gb_BatchModeWithoutPerforce)
            return FALSE;
        return TRUE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnActionUI(ULONG _ul_Action, CString &_o_Ref, CString &o_Ret)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Name[BIG_C_MaxLenName];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	o_Ret = _o_Ref;
	switch(_ul_Action)
	{
	case EAI_ACTION_FINDNEXT:
		o_Ret += " (";
		o_Ret += mo_SearchString;
		o_Ret += ")";
		break;

	case EAI_ACTION_DELETE:
	case EAI_ACTION_RENAMEFUNC:
		L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditFile));
		o_Ret += " ";
		o_Ret += asz_Name;
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	unsigned char	c_Key;
	BOOL			b_Shift, b_Ctrl;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* If not edit file, editor knows nothing */
	if(mul_CurrentEditFile == BIG_C_InvalidIndex) return FALSE;
	if((GetFocus() != mpo_Edit) && (GetFocus() != (CWnd *) mpo_ListAI)) return FALSE;

	b_Shift = _uw_Key & SHIFT;
	b_Ctrl = _uw_Key & CONTROL;
	c_Key = (char) _uw_Key;

	switch(c_Key)
	{
	case VK_SPACE:
		if(b_Shift || b_Ctrl) return FALSE;
		break;

	case VK_RETURN:
	case '/':
	case 0xde:					/* Completion */
		if(!b_Shift && !b_Ctrl) return TRUE;
		break;

	case VK_TAB:				/* Unindent block */
		if(b_Shift) return TRUE;
		break;

	case VK_DELETE:
		if(b_Ctrl) return TRUE; /* Delete word */
		break;

	case VK_UP:
	case VK_DOWN:
		if(b_Ctrl) return TRUE;
		break;

	case 'A':					/* Select all */
	case 'C':					/* Copy */
	case 'V':					/* Paste */
	case 'X':					/* Cut */
	case 'L':					/* Cut current line */
		if(b_Ctrl && !b_Shift) return TRUE;
		break;

	case 'Z':					/* Undo */
	case 'Y':					/* Redo */
		if(b_Ctrl && !b_Shift) return TRUE;
		break;

	case 0xbe:					/* (UN) Comment block */
		if(b_Ctrl) return TRUE;
		break;
	}

	if(M_MF()->b_EditKey(_uw_Key)) return TRUE;
	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Frame::i_IsItVarAction(ULONG _ul_Action, EVAV_cl_View *_po_List)
{
	switch(_ul_Action)
	{
	case EAI_SEPACTION_OPTIONS:
		/* To avoid reset and refresh the list if not necessary */
		if(!_po_List) return 2;
		if(_po_List->mul_ID == EAI_SEPACTION_OPTIONS) return 2;
		_po_List->mul_ID = EAI_SEPACTION_OPTIONS;

		_po_List->ResetList();

		/* General */
		_po_List->AddItem(EAI_STR_Csz_General, EVAV_EVVIT_Separator, NULL);
		_po_List->AddItem(EAI_STR_Csz_FontSize, EVAV_EVVIT_Int, &mst_Ini.i_FontSize, 0, 1, 100, 0);
		_po_List->AddItem(EAI_STR_Csz_TabStop, EVAV_EVVIT_Int, &mst_Ini.i_TabStop, 0, 1, 20, 0);
		_po_List->AddItem(EAI_STR_Csz_AutoSave, EVAV_EVVIT_Bool, &mst_Ini.b_AutoSave, 0, 0, 0, 0);

		/* Colors */
		_po_List->AddItem(EAI_STR_Csz_Display, EVAV_EVVIT_Separator, NULL);
		_po_List->AddItem(EAI_STR_Csz_ColorBkGnd, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorBkGnd]);
		_po_List->AddItem(EAI_STR_Csz_ColorField, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorField]);
		_po_List->AddItem(EAI_STR_Csz_ColorPP, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorPP]);
		_po_List->AddItem(EAI_STR_Csz_ColorComment, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorComment]);
		_po_List->AddItem(EAI_STR_Csz_ColorFunction, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorFunction]);
		_po_List->AddItem(EAI_STR_Csz_ColorKeyword, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorKeyword]);
		_po_List->AddItem(EAI_STR_Csz_ColorType, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorType]);
		_po_List->AddItem(EAI_STR_Csz_ColorReserved, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorReserved]);
		_po_List->AddItem(EAI_STR_Csz_ColorString, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorString]);
		_po_List->AddItem(EAI_STR_Csz_ColorConstant, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorConstant]);
		_po_List->AddItem(EAI_STR_Csz_ColorText, EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorDefault]);
		_po_List->AddItem("PP defines", EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorPPConstants]);
		_po_List->AddItem("PP macros", EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorPPMacros]);
		_po_List->AddItem("Procedures", EVAV_EVVIT_Color, &mst_Ini.ax_Colors[EAI_C_ColorProcedures]);

		/* Perforce */
		_po_List->AddItem(EAI_STR_Csz_P4, EVAV_EVVIT_Separator, NULL);
		_po_List->AddItem(EAI_STR_Csz_P4CloseWorld, EVAV_EVVIT_Bool, &mb_P4CloseWorld);
		return 1;
	}

	return 0;
}

#endif
