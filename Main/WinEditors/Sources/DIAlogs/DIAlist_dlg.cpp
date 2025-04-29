/*$T DIAlist_dlg.cpp GC 1.139 03/23/04 17:02:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAlist_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "LINKs/LINKtoed.h"
#include "EDItors/Sources/AIscript/AIframe.h"
#include "EDItors/Sources/AIscript/EditView/AIview.h"
#include "EDImsg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "EDIpaths.h"
#include "AIinterp/Sources/AIdebug.h"
#include "INOut/INO.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_ListDialog, EDIA_cl_BaseDialog)
	ON_WM_SIZE()
	ON_LBN_DBLCLK(IDC_LIST1, OnOK)
	ON_LBN_DBLCLK(IDC_LIST22, OnOK)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_ListDialog::EDIA_cl_ListDialog(int _i_Mode) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_LIST)
{
	mi_Mode = _i_Mode;
	mul_Current = BIG_C_InvalidIndex;
	mpi_Sel = NULL;
}

EDIA_cl_ListDialog::~EDIA_cl_ListDialog()
{
	if(mi_Mode == EDIA_List_SelectMultipleTxg)
	{
		if(mpi_Sel) delete[] mpi_Sel ;
		mpi_Sel = NULL;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_ListDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~*/
	int			i;
	CListBox	*p_List;
	/*~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnInitDialog();
	switch(mi_Mode)
	{
	case EDIA_List_SpeedSelectGAO:
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_EXTENDEDSEL )->ShowWindow(SW_HIDE);
		SpeedSelectGAO();
		GetDlgItem(IDC_LIST1)->SetFocus();
		break;

	case EDIA_List_SpeedSelectMDL:
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_EXTENDEDSEL )->ShowWindow(SW_HIDE);
		SpeedSelectMDL(BIG_ul_SearchDir(EDI_Csz_Path_AIModels));
		if(mul_Current != BIG_C_InvalidIndex)
		{
			p_List = (CListBox *) GetDlgItem(IDC_LIST1);
			i = p_List->FindString(-1, BIG_NameFile(mul_Current));
			if(i != -1) p_List->SetCurSel(i);
		}

		GetDlgItem(IDC_LIST1)->SetFocus();
		break;

	case EDIA_List_SpeedSelectGlob:
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_EXTENDEDSEL )->ShowWindow(SW_HIDE);
		SpeedSelectGlob(BIG_ul_SearchDir(EDI_Csz_Path_AILib));
		p_List = (CListBox *) GetDlgItem(IDC_LIST1);
		p_List->SetCurSel(0);
		GetDlgItem(IDC_LIST1)->SetFocus();
		break;

	case EDIA_List_SpeedSelectFCT:
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_EXTENDEDSEL )->ShowWindow(SW_HIDE);
		SpeedSelectFCT();
		if(mul_Current != BIG_C_InvalidIndex)
		{
			p_List = (CListBox *) GetDlgItem(IDC_LIST1);
			i = p_List->FindString(-1, BIG_NameFile(mul_Current));
			if(i != -1) p_List->SetCurSel(i);
		}

		GetDlgItem(IDC_LIST1)->SetFocus();
		break;

	case EDIA_List_SpeedSelectHistoryFCT:
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_EXTENDEDSEL )->ShowWindow(SW_HIDE);
		SpeedSelectHisFCT();
		if(mul_Current != BIG_C_InvalidIndex)
		{
			p_List = (CListBox *) GetDlgItem(IDC_LIST1);
			i = p_List->FindString(-1, BIG_NameFile(mul_Current));
			if(i != -1) p_List->SetCurSel(i);
		}

		GetDlgItem(IDC_LIST1)->SetFocus();
		break;

	case EDIA_List_CallStack:
		GetDlgItem(IDC_LIST22)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_EXTENDEDSEL )->ShowWindow(SW_HIDE);
		SpeedSelectCallStack();
		GetDlgItem(IDC_LIST22)->SetFocus();
		break;

	case EDIA_List_SpeedSelectSector:
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_EXTENDEDSEL )->ShowWindow(SW_HIDE);
		SpeedSelectSector();
		p_List = (CListBox *) GetDlgItem(IDC_LIST1);
		p_List->SetCurSel(0);
		GetDlgItem(IDC_LIST1)->SetFocus();
		break;
		
	case EDIA_List_SelectMultipleLangage:
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_EXTENDEDSEL )->ShowWindow(SW_SHOW);
		SelectMultipleLangage();
		GetDlgItem(IDC_LIST_EXTENDEDSEL)->SetFocus();
		break;

	case EDIA_List_SelectMultipleTxg:
		GetDlgItem(IDC_LIST1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST22)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_EXTENDEDSEL )->ShowWindow(SW_SHOW);
		SelectMultipleTxg();
		GetDlgItem(IDC_LIST_EXTENDEDSEL)->SetFocus();
		break;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::OnSize(UINT a, int cx, int cy)
{
	EDIA_cl_BaseDialog::OnSize(a, cx, cy);
	if(GetDlgItem(IDC_LIST1)) GetDlgItem(IDC_LIST1)->MoveWindow(0, 0, cx, cy);
	if(GetDlgItem(IDC_LIST22)) GetDlgItem(IDC_LIST22)->MoveWindow(0, 0, cx, cy);
	if(GetDlgItem(IDC_LIST_EXTENDEDSEL)) GetDlgItem(IDC_LIST_EXTENDEDSEL)->MoveWindow(0, 0, cx, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::SpeedSelectSector(void)
{
	/*~~~~~~~~~~~~~~~~*/
	int			i, item;
	CListBox	*p_List;
	/*~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	for(i = 0; i < WOR_C_MaxSecto; i++)
	{
		if(mpst_World->ast_AllSectos[i].ul_Flags & WOR_CF_SectorValid)
		{
			item = p_List->AddString(mpst_World->ast_AllSectos[i].az_Name);
			p_List->SetItemData(item, i);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::SpeedSelectGAO(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	CListBox			*p_List;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&mpst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&mpst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone) continue;
		i = p_List->AddString(pst_GO->sz_Name);
		p_List->SetItemData(i, (DWORD) pst_GO);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::SpeedSelectMDL(BIG_INDEX _ul_Dir)
{
	/*~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_File;
	int			i;
	CListBox	*p_List;
	/*~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	if(_ul_Dir == BIG_C_InvalidIndex) return;
	ul_File = BIG_FirstFile(_ul_Dir);
	while(ul_File != BIG_C_InvalidIndex)
	{
		if(BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtAIEditorModel))
		{
			i = p_List->AddString(BIG_NameFile(ul_File));
			p_List->SetItemData(i, ul_File);
		}

		ul_File = BIG_NextFile(ul_File);
	}

	ul_File = BIG_SubDir(_ul_Dir);
	while(ul_File != BIG_C_InvalidIndex)
	{
		SpeedSelectMDL(ul_File);
		ul_File = BIG_NextDir(ul_File);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::SpeedSelectGlob(BIG_INDEX _ul_Dir)
{
	/*~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_File;
	int			i;
	CListBox	*p_List;
	/*~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	if(_ul_Dir == BIG_C_InvalidIndex) return;
	ul_File = BIG_FirstFile(_ul_Dir);
	while(ul_File != BIG_C_InvalidIndex)
	{
		if
		(
			BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtAIEditorVars)
		||	BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtAIEditorFct)
		)
		{
			i = p_List->AddString(BIG_NameFile(ul_File));
			p_List->SetItemData(i, ul_File);
		}

		ul_File = BIG_NextFile(ul_File);
	}

	ul_File = BIG_SubDir(_ul_Dir);
	while(ul_File != BIG_C_InvalidIndex)
	{
		SpeedSelectGlob(ul_File);
		ul_File = BIG_NextDir(ul_File);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::SpeedSelectFCT(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_Size;
	CListBox			*p_List;
	ULONG				i;
	int					j;
	BIG_INDEX			ul_File;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(mul_Model), &ul_Size);
	for(i = 0; i < (ul_Size / sizeof(BIG_tdst_GroupElem)); i++)
	{
		if(!pst_Buf[i].ul_Key) continue;
		ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
		j = p_List->AddString(BIG_NameFile(ul_File));
		p_List->SetItemData(j, ul_File);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::SpeedSelectHisFCT(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*p_List;
	POSITION	pos;
	int			i;
	BIG_INDEX	ul_File;
	/*~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST1);
	pos = mpo_AIFrame->mo_History.GetHeadPosition();
	while(pos)
	{
		ul_File = mpo_AIFrame->mo_History.GetNext(pos);
		i = p_List->AddString(BIG_NameFile(ul_File));
		p_List->SetItemData(i, ul_File);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::SpeedSelectCallStack(void)
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*p_List;
	int			i, res;
	/*~~~~~~~~~~~~~~~~*/

	p_List = (CListBox *) GetDlgItem(IDC_LIST22);
	for(i = 0; i < AI_gi_RunFctLevel; i++)
	{
		res = p_List->AddString(AI_gst_CallStack[i].pt_Function->az_Name);
		p_List->SetItemData(res, i);
	}

	if(AI_gpst_CurrentFunction)
	{
		res = p_List->AddString(AI_gpst_CurrentFunction->az_Name);
		p_List->SetItemData(res, -1);
		p_List->SetCurSel(res);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::SelectMultipleLangage( void )
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*p_List;
	int			i, res;
	char		sz_Text[ 256 ];
	/*~~~~~~~~~~~~~~~~*/
	
	p_List = (CListBox *) GetDlgItem(IDC_LIST_EXTENDEDSEL );
	
	for(i = 0; i < INO_e_MaxLangNb; i++)
	{
		snprintf( sz_Text, sizeof(sz_Text), "%02d - %s", i, INO_dst_LanguageName[ i ].az_LongName );
		res = p_List->AddString( sz_Text );
		p_List->SetItemData(res, i);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::SelectMultipleTxg( void )
{
	/*~~~~~~~~~~~~~~~~*/
	CListBox	*p_List;
	int			res;
	char		sz_Text[ 256 ];
	/*~~~~~~~~~~~~~~~~*/
	
	p_List = (CListBox *) GetDlgItem(IDC_LIST_EXTENDEDSEL );
	for(ULONG ul_Index = 0, i=0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;
		if(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtTextLang)) continue;
		
		snprintf( sz_Text, sizeof(sz_Text), "%02d - %s", i, BIG_NameFile( ul_Index ) );
		res = p_List->AddString( sz_Text );
		p_List->SetItemData(res, ul_Index);
		i++;
	}	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_ListDialog::OnOK(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i,j;
	OBJ_tdst_GameObject *pst_GO;
	CListBox			*p_List;
	BIG_INDEX			h;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(mi_Mode)
	{
	case EDIA_List_SpeedSelectGAO:
		p_List = (CListBox *) GetDlgItem(IDC_LIST1);
		i = p_List->GetCurSel();
		if(i != -1)
		{
			pst_GO = (OBJ_tdst_GameObject *) p_List->GetItemData(i);
			mpo_View->ForceSelectObject(pst_GO, TRUE);
			mpo_View->ZoomExtendSelected
				(
					&mpo_View->mst_WinHandles.pst_DisplayData->st_Camera,
					mpo_View->mst_WinHandles.pst_World
				);
			LINK_Refresh();
		}
		break;

	case EDIA_List_SpeedSelectGlob:
	case EDIA_List_SpeedSelectMDL:
	case EDIA_List_SpeedSelectFCT:
	case EDIA_List_SpeedSelectHistoryFCT:
		p_List = (CListBox *) GetDlgItem(IDC_LIST1);
		i = p_List->GetCurSel();
		if(i != -1) mpo_AIFrame->i_OnMessage(EDI_MESSAGE_SELFILE, 0, p_List->GetItemData(i));
		break;

	case EDIA_List_CallStack:
		p_List = (CListBox *) GetDlgItem(IDC_LIST22);
		i = p_List->GetCurSel();
		if(i != -1)
		{
			i = p_List->GetItemData(i);
			if(i == -1)
			{
				mpo_AIFrame->i_OnMessage(EDI_MESSAGE_SELFILE, 0, AI_gst_BreakInfo.ul_File);
				mpo_AIFrame->ConnectToLine(AI_gst_BreakInfo.i_Line);
			}
			else
			{
				h = BIG_ul_SearchKeyToFat(*((LONG *) &AI_gst_CallStack[i].pt_Node->w_Param));
				mpo_AIFrame->i_OnMessage(EDI_MESSAGE_SELFILE, 0, h);
				mpo_AIFrame->ConnectToLine(AI_gst_CallStack[i].pt_Node->l_Param);
			}
		}
		break;

	case EDIA_List_SpeedSelectSector:
		p_List = (CListBox *) GetDlgItem(IDC_LIST1);
		mul_Model = -1;
		i = p_List->GetCurSel();
		if(i != -1) mul_Model = p_List->GetItemData(i);
		break;
		
	case EDIA_List_SelectMultipleLangage:
		if (mpi_Sel)
		{	
			int ai_Sel[ INO_e_MaxLangNb ], nb;
			
			L_memset( mpi_Sel, 0, sizeof(int) * mi_SelNb );
				
			p_List = (CListBox *) GetDlgItem(IDC_LIST_EXTENDEDSEL);
			nb = p_List->GetSelCount();
			p_List->GetSelItems( INO_e_MaxLangNb, ai_Sel );
			for ( i = 0; i < nb; i++)
				mpi_Sel[ ai_Sel[ i ] ] = 1;
		}
		break;
		
	case EDIA_List_SelectMultipleTxg:
		p_List = (CListBox *) GetDlgItem(IDC_LIST_EXTENDEDSEL);
		
		if(mpi_Sel) delete[] mpi_Sel;
		mpi_Sel = NULL;
		mi_SelNb = p_List->GetSelCount();
		if(mi_SelNb)
		{
			mpi_Sel = new int[mi_SelNb];
			for(i=j=0; i<p_List->GetCount();i++)
			{
				if(p_List->GetSel(i) > 0)
				{
					mpi_Sel[j++] = p_List->GetItemData(i);
				}
			}
		}
		break;
	}

	EDIA_cl_BaseDialog::OnOK();
}
#endif /* ACTIVE_EDITORS */
