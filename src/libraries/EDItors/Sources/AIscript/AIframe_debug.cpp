/*$T AIframe_debug.cpp GC 1.138 12/10/03 16:17:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "AIframe.h"
#include "EditView/AIleftview.h"
#include "EditView/AIview.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "EDIpaths.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLibrary/CLIMEM.h"
#include "AIinterp/Sources/AIdebug.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDImsg.h"
#include "EDIbaseview.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/ENGinit.h"
#include "EDImainframe.h"
#include "BIGfiles/BIGkey.h"
#include "EDIapp.h"
#include "LINKs/LINKmsg.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EAI_cl_Frame::ChangeLine(ULONG _ul_Line, BIG_INDEX _ul_File, AI_tdst_Node **_ppst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX				ul_Engine;
	AI_tdst_Function		*pst_Function;
	SCR_tt_ProcedureList	*pst_ProcList;
	SCR_tt_Procedure		*pst_Proc;
	AI_tdst_Node			*pst_Node1, *pst_Node2, *pst_MemoNode1, *pst_MemoNode2;
	int						i;
	BOOL					first;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtAIEditorFct))
	{
		ul_Engine = mo_Compiler.ul_GetEngineFileForForFunction(BIG_FileKey(_ul_File));
		ul_Engine = BIG_ul_SearchKeyToFat(ul_Engine);
		pst_Function = (AI_tdst_Function *) LOA_ul_SearchAddress(BIG_PosFile(ul_Engine));
		if((ULONG) pst_Function == BIG_C_InvalidIndex) pst_Function = NULL;
		pst_Node1 = pst_Node2 = NULL;
		if(pst_Function && pst_Function->pst_RootNode && pst_Function->pst_OtherRootNode)
		{
			pst_MemoNode1 = pst_Node1 = pst_Function->pst_RootNode;
			pst_MemoNode2 = pst_Node2 = pst_Function->pst_OtherRootNode;
			while(pst_Node1->c_Type != CATEG_ENDTREE)
			{
				if((pst_Node2->l_Param >= (LONG) _ul_Line) && (pst_Node2->l_Param != 0xFFFFFFFF))
				{
					pst_MemoNode1 = pst_Node1;
					pst_MemoNode2 = pst_Node2;
					_ul_Line = pst_MemoNode2->l_Param;
					break;
				}

				if(pst_Node2->l_Param != 0xFFFFFFFF)
				{
					pst_MemoNode1 = pst_Node1;
					pst_MemoNode2 = pst_Node2;
				}

				pst_Node1++;
				pst_Node2++;
			}

			if(pst_Node1->c_Type == CATEG_ENDTREE) _ul_Line = pst_MemoNode2->l_Param;
		}

		if(_ppst_Node) *_ppst_Node = pst_MemoNode1;
	}
	else if(!BIG_b_IsFileExtension(_ul_File, EDI_Csz_ExtAIEditorVars))
	{
		ul_Engine = mo_Compiler.ul_GetEngineFileForForProcList(BIG_FileKey(_ul_File));
		ul_Engine = BIG_ul_SearchKeyToFat(ul_Engine);
		pst_ProcList = (SCR_tt_ProcedureList *) LOA_ul_SearchAddress(BIG_PosFile(ul_Engine));
		if((ULONG) pst_ProcList == BIG_C_InvalidIndex) pst_Proc = NULL;
		pst_Node1 = pst_Node2 = NULL;
		if(pst_ProcList && pst_ProcList->pt_All)
		{
			for(i = 0; i < pst_ProcList->u16_Num; i++)
			{
				pst_Proc = &pst_ProcList->pt_All[i];
				if(pst_Proc->pu32_Nodes && pst_Proc->pu32_DbgNodes)
				{
					pst_MemoNode1 = pst_Node1 = pst_Proc->pu32_Nodes;
					pst_MemoNode2 = pst_Node2 = pst_Proc->pu32_DbgNodes;
					first = TRUE;
					while(pst_Node1->c_Type != CATEG_ENDTREE)
					{
						if((pst_Node2->l_Param >= (LONG) _ul_Line) && (pst_Node2->l_Param != 0xFFFFFFFF))
						{
							if(first) goto l_Next;
							first = FALSE;
							if(pst_Node2 != pst_Proc->pu32_DbgNodes || pst_Node2->l_Param == (LONG) _ul_Line)
							{
								pst_MemoNode1 = pst_Node1;
								pst_MemoNode2 = pst_Node2;
								_ul_Line = pst_MemoNode2->l_Param;
								goto l_end;
							}
							else
							{
								goto l_Next;
							}
						}

						if(pst_Node2->l_Param != 0xFFFFFFFF)
						{
							first = FALSE;
							pst_MemoNode1 = pst_Node1;
							pst_MemoNode2 = pst_Node2;
						}

						pst_Node1++;
						pst_Node2++;
					}
l_Next:;
				}
			}

l_end:
			if(_ppst_Node) *_ppst_Node = pst_MemoNode1;
		}
	}

	return _ul_Line;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnBreakPoint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CHARRANGE		cr;
	LONG			l_Line, l_Line1;
	POSITION		pos, ppos, pos1, pos2, pos3, pos4;
	BIG_INDEX		ul_File, ul_Model, ul_Instance;
	BOOL			b_Ok;
	AI_tdst_Node	*pst_MemoNode1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mpo_Edit->GetSel(cr);
	l_Line = mpo_Edit->LineFromChar(cr.cpMin);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Search the node that best match the line
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_MemoNode1 = NULL;
	l_Line = ChangeLine(l_Line, mul_CurrentEditFile, &pst_MemoNode1);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Search the same line in the same file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pos = mpo_LeftView->mo_ListBreak.GetHeadPosition();
	pos1 = mpo_LeftView->mo_ListBreakFile.GetHeadPosition();
	pos2 = mpo_LeftView->mo_ListBreakModel.GetHeadPosition();
	pos3 = mpo_LeftView->mo_ListBreakInstance.GetHeadPosition();
	b_Ok = FALSE;
	while(pos)
	{
		ppos = pos;
		l_Line1 = mpo_LeftView->mo_ListBreak.GetNext(pos);
		ul_File = BIG_ul_SearchKeyToFat(mpo_LeftView->mo_ListBreakFile.GetNext(pos1));
		ul_Model = BIG_ul_SearchKeyToFat(mpo_LeftView->mo_ListBreakModel.GetNext(pos2));
		ul_Instance = BIG_ul_SearchKeyToFat(mpo_LeftView->mo_ListBreakInstance.GetNext(pos3));
		if
		(
			(l_Line1 == l_Line)
		&&	(ul_File == mul_CurrentEditFile)
		&&	(ul_Model == mul_CurrentEditModel)
		&&	((ul_Instance == mul_CurrentEditInstance) || (ul_Instance == -1))
		)
		{
			b_Ok = TRUE;
			break;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Add breakpoint
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!b_Ok)
	{
		mpo_LeftView->mo_ListBreak.AddTail(l_Line);

		if(mul_CurrentEditFile != BIG_C_InvalidIndex)
			mpo_LeftView->mo_ListBreakFile.AddTail(BIG_FileKey(mul_CurrentEditFile));
		else
			mpo_LeftView->mo_ListBreakFile.AddTail(BIG_C_InvalidIndex);

		if(mul_CurrentEditModel != BIG_C_InvalidIndex)
			mpo_LeftView->mo_ListBreakModel.AddTail(BIG_FileKey(mul_CurrentEditModel));
		else
			mpo_LeftView->mo_ListBreakModel.AddTail(BIG_C_InvalidIndex);

		if(mul_CurrentEditInstance != BIG_C_InvalidIndex)
			mpo_LeftView->mo_ListBreakInstance.AddTail(BIG_FileKey(mul_CurrentEditInstance));
		else
			mpo_LeftView->mo_ListBreakInstance.AddTail(BIG_C_InvalidIndex);

		mpo_LeftView->mo_ListBreakPtInst.AddTail(mpst_Instance);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Delete the corresponding file index
	 -------------------------------------------------------------------------------------------------------------------
	 */

	else
	{
		pos = ppos;
		ppos = mpo_LeftView->mo_ListBreak.GetHeadPosition();
		pos1 = mpo_LeftView->mo_ListBreakFile.GetHeadPosition();
		pos2 = mpo_LeftView->mo_ListBreakModel.GetHeadPosition();
		pos3 = mpo_LeftView->mo_ListBreakInstance.GetHeadPosition();
		pos4 = mpo_LeftView->mo_ListBreakPtInst.GetHeadPosition();
		while(ppos != pos)
		{
			mpo_LeftView->mo_ListBreak.GetNext(ppos);
			mpo_LeftView->mo_ListBreakFile.GetNext(pos1);
			mpo_LeftView->mo_ListBreakModel.GetNext(pos2);
			mpo_LeftView->mo_ListBreakInstance.GetNext(pos3);
			mpo_LeftView->mo_ListBreakPtInst.GetNext(pos4);
		}

		mpo_LeftView->mo_ListBreak.RemoveAt(pos);
		mpo_LeftView->mo_ListBreakFile.RemoveAt(pos1);
		mpo_LeftView->mo_ListBreakModel.RemoveAt(pos2);
		mpo_LeftView->mo_ListBreakInstance.RemoveAt(pos3);
		mpo_LeftView->mo_ListBreakPtInst.RemoveAt(pos4);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Delete breakpoint in node if necessary
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(b_Ok && pst_MemoNode1) pst_MemoNode1->c_Flags &= ~AI_Cflag_BreakPoint;

	SetEngineBreakPoint();
	mpo_LeftView->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::SetEngineBreakPointAtLine(AI_tdst_Function *_pst_Func, LONG _l_Line, BOOL _b_Set)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Node	*pst_Node1, *pst_Node2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Node1 = _pst_Func->pst_RootNode;
	pst_Node2 = _pst_Func->pst_OtherRootNode;
	if(!pst_Node1) return;
	if(!pst_Node2) return;
	while(pst_Node1->c_Type != CATEG_ENDTREE)
	{
		if(pst_Node2->l_Param == _l_Line)
		{
			if(_b_Set)
				pst_Node1->c_Flags |= AI_Cflag_BreakPoint;
			else
				pst_Node1->c_Flags &= ~AI_Cflag_BreakPoint;
			break;
		}

		pst_Node1++;
		pst_Node2++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::SetEngineBreakPointAtLine(SCR_tt_ProcedureList *_pst_ProcList, LONG _l_Line, BOOL _b_Set)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Node		*pst_Node1, *pst_Node2;
	int					i;
	SCR_tt_Procedure	*pt_Proc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < _pst_ProcList->u16_Num; i++)
	{
		pt_Proc = &_pst_ProcList->pt_All[i];
		pst_Node1 = pt_Proc->pu32_Nodes;
		pst_Node2 = pt_Proc->pu32_DbgNodes;
		if(!pst_Node1) return;
		if(!pst_Node2) return;
		while(pst_Node1->c_Type != CATEG_ENDTREE)
		{
			if(pst_Node2->l_Param == _l_Line)
			{
				if(_b_Set)
					pst_Node1->c_Flags |= AI_Cflag_BreakPoint;
				else
					pst_Node1->c_Flags &= ~AI_Cflag_BreakPoint;
				return;
			}

			pst_Node1++;
			pst_Node2++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::SetEngineBreakPoint(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION				ppos, pos, pos1, pos2, pos3, pos4;
	int						i_Line;
	BIG_INDEX				ul_File, ul_Model, ul_Instance, ul_Engine;
	AI_tdst_Function		*pst_Function;
	AI_tdst_Instance		*pst_Instance;
	SCR_tt_ProcedureList	*pst_ProcList;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Model) return;

	AI_DelBreakListForModel(mpst_Model);
	pos = mpo_LeftView->mo_ListBreak.GetHeadPosition();
	pos1 = mpo_LeftView->mo_ListBreakFile.GetHeadPosition();
	pos2 = mpo_LeftView->mo_ListBreakModel.GetHeadPosition();
	pos3 = mpo_LeftView->mo_ListBreakInstance.GetHeadPosition();
	pos4 = mpo_LeftView->mo_ListBreakPtInst.GetHeadPosition();
	while(pos)
	{
		ppos = pos;
		pst_Instance = NULL;
		i_Line = mpo_LeftView->mo_ListBreak.GetNext(pos);
		ul_File = BIG_ul_SearchKeyToFat(mpo_LeftView->mo_ListBreakFile.GetNext(pos1));
		ul_Model = BIG_ul_SearchKeyToFat(mpo_LeftView->mo_ListBreakModel.GetNext(pos2));
		ul_Instance = BIG_ul_SearchKeyToFat(mpo_LeftView->mo_ListBreakInstance.GetNext(pos3));
		pst_Instance = (AI_tdst_Instance *) mpo_LeftView->mo_ListBreakPtInst.GetNext(pos4);
		i_Line = ChangeLine(i_Line, ul_File, NULL);
		mpo_LeftView->mo_ListBreak.SetAt(ppos, i_Line);

		/* Instance */
		if(ul_Instance != BIG_C_InvalidIndex)
		{
			if(!pst_Instance) pst_Instance = (AI_tdst_Instance *) LOA_ul_SearchAddress(BIG_PosFile(ul_Instance));
			if((ULONG) pst_Instance == BIG_C_InvalidIndex) continue;
		}

		/* Function */
		if(BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtAIEditorFct))
		{
			ul_Engine = mo_Compiler.ul_GetEngineFileForForFunction(BIG_FileKey(ul_File));
			ul_Engine = BIG_ul_SearchKeyToFat(ul_Engine);
			pst_Function = (AI_tdst_Function *) LOA_ul_SearchAddress(BIG_PosFile(ul_Engine));
			if((ULONG) pst_Function == BIG_C_InvalidIndex) continue;

			/* Add engine breakpoint */
			if(pst_Function)
			{
				AI_AddBreakPoint(ul_File, ul_Engine, mpst_Model, pst_Instance, i_Line);
				SetEngineBreakPointAtLine(pst_Function, i_Line, TRUE);
			}
		}
		else
		{
			ul_Engine = mo_Compiler.ul_GetEngineFileForForProcList(BIG_FileKey(ul_File));
			ul_Engine = BIG_ul_SearchKeyToFat(ul_Engine);
			pst_ProcList = (SCR_tt_ProcedureList *) LOA_ul_SearchAddress(BIG_PosFile(ul_Engine));
			if((ULONG) pst_ProcList == BIG_C_InvalidIndex) continue;

			/* Add engine breakpoint */
			if(pst_ProcList)
			{
				AI_AddBreakPoint(ul_File, ul_Engine, mpst_Model, pst_Instance, i_Line);
				SetEngineBreakPointAtLine(pst_ProcList, i_Line, TRUE);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::ConnectToEngine(void)
{
	/*~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_File;
	/*~~~~~~~~~~~~~~~~*/

	/* The if the current loaded AI has been loaded by engine */
	if(!mpst_Model && mul_CurrentEditModel != BIG_C_InvalidIndex)
	{
		ul_File = mo_Compiler.ul_GetEngineModelIndex(mul_CurrentEditModel, NULL, NULL);
		if(ul_File != BIG_C_InvalidIndex)
		{
			ul_File = LOA_ul_SearchAddress(BIG_PosFile(ul_File));
			if(ul_File != BIG_C_InvalidIndex)
			{
				mpst_Model = (AI_tdst_Model *) ul_File;
				if(mul_CurrentEditInstance != BIG_C_InvalidIndex)
				{
					ul_File = LOA_ul_SearchAddress(BIG_PosFile(mul_CurrentEditInstance));
					if(ul_File != BIG_C_InvalidIndex) mpst_Instance = (AI_tdst_Instance *) ul_File;
				}

				DisplayPaneNames();
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::ConnectToLine(int line)
{
	/*~~~~~~~*/
	int i_Line;
	/*~~~~~~~*/

	/* Place on good line */
	mpo_LeftView->mi_LineBreak = line;
	i_Line = mpo_Edit->LineIndex(mpo_LeftView->mi_LineBreak);
	if
	(
		!AI_gi_OneStepMode
	||	(mpo_LeftView->mi_LineBreak <= mpo_Edit->l_FirstVisibleLine())
	||	(mpo_LeftView->mi_LineBreak >= mpo_Edit->l_LastVisibleLine())
	)
	{
		if(mpo_LeftView->mi_LineBreak < mpo_Edit->l_FirstVisibleLine())
		{
			mpo_Edit->LineScroll(mpo_LeftView->mi_LineBreak - mpo_Edit->l_FirstVisibleLine() - 2);
		}
		else if(mpo_LeftView->mi_LineBreak >= mpo_Edit->l_LastVisibleLine())
		{
			mpo_Edit->LineScroll
				(
					mpo_LeftView->mi_LineBreak - mpo_Edit->l_LastVisibleLine() +
						((mpo_Edit->l_LastVisibleLine() - mpo_Edit->l_FirstVisibleLine()) / 2)
				);
		}
	}

	mpo_Edit->SetSel(i_Line, i_Line);
	if(AI_gi_OneStepMode) mpo_Edit->SetFocus();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::AfterEngine(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	BOOL		b;
	CPoint		pt;
	extern int	NNN_ForceNow;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&pt);
	mpo_Edit->ScreenToClient(&pt);
	mpo_Edit->SetDisplayVar(pt);
	b = NNN_ForceNow;
	NNN_ForceNow = TRUE;
	mpo_Edit->DisplayVar(mpo_Edit->mo_DisplayVar, TRUE);
	NNN_ForceNow = b;

	ConnectToEngine();
	if(AI_gb_ExitByBreak && mst_BaseIni.b_EngineRefresh)
	{
		M_MF()->LockDisplay(mpo_Edit);

		mb_UpdateOwner = FALSE;
		if(AI_gst_BreakInfo.pst_Instance)
			i_OnMessage(EDI_MESSAGE_SELDATA, 0, (ULONG) AI_gst_BreakInfo.pst_Instance);
		else
			i_OnMessage(EDI_MESSAGE_SELDATA, 0, (ULONG) AI_gst_BreakInfo.pst_Model);
		mb_UpdateOwner = TRUE;

		/* Load the good file */
		if(AI_gst_BreakInfo.ul_File == BIG_C_InvalidIndex)
		{
			M_MF()->UnlockDisplay(mpo_Edit);
			return;
		}

		if(mul_CurrentEditFile != AI_gst_BreakInfo.ul_File) Load(AI_gst_BreakInfo.ul_File);

		/* Place on good line */
		ConnectToLine(AI_gst_BreakInfo.i_Line);

		M_MF()->UnlockDisplay(mpo_Edit);

		/* Force show editor */
		mpo_MyView->IWantToBeActive(this);
	}
	else
		mpo_LeftView->mi_LineBreak = -1;

	mpo_LeftView->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnOneStep(BOOL _b_In)
{
	if(_b_In)
		AI_gi_OneStepMode = 2;
	else
		AI_gi_OneStepMode = 1;
	EDI_go_TheApp.mb_RunEngine = !EDI_go_TheApp.mb_RunEngine;
	if(EDI_go_TheApp.mb_RunEngine == FALSE) ENG_gb_ForceEndEngine = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnDeleteAllBkp(void)
{
	mpo_LeftView->mo_ListBreak.RemoveAll();
	mpo_LeftView->mo_ListBreakFile.RemoveAll();
	mpo_LeftView->mo_ListBreakModel.RemoveAll();
	mpo_LeftView->mo_ListBreakInstance.RemoveAll();
	mpo_LeftView->mo_ListBreakPtInst.RemoveAll();
	AI_DelAllBreakPoints();
}
