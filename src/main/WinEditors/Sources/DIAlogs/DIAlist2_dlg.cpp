/*$T DIAlist2_dlg.cpp GC 1.134 05/14/04 10:41:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAlist2_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDIpaths.h"

#define A(a, b, c, d, e, f, g, h) \
	mo_ListItems.AddTail(po_NewItem = new EVAV_cl_ViewItem(a, b, c, d, e, f, g, h)); \
	
extern void EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);

/*$4-******************************************************************************************************************/

BEGIN_MESSAGE_MAP(EDIA_cl_List2Dialog, EDIA_cl_BaseDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelChange)
	ON_LBN_SELCHANGE(IDC_LIST2, OnSelChange2)
	ON_COMMAND(ID_SETNULL, OnSetNull)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_List2Dialog::EDIA_cl_List2Dialog(int mode) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_LIST2)
{
	mi_Mode = mode;

	mpo_VarsView = NULL;
	if(mi_Mode == LIST2_ModeTrigger)
	{
		mpo_VarsView = new EVAV_cl_View;
		mst_VarsViewStruct.po_ListItems = &mo_ListItems;
		mpo_VarsView->mpo_ListBox->mpo_ListItems = &mo_ListItems;
		mst_VarsViewStruct.psz_NameCol1 = "Name";
		mst_VarsViewStruct.psz_NameCol2 = "Value";
		mst_VarsViewStruct.i_WidthCol1 = 100;
		mpo_VarsView->mb_CanDragDrop = FALSE;
	}
	mi_TextDisplay = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_List2Dialog::~EDIA_cl_List2Dialog(void)
{
	if(mpo_VarsView) delete mpo_VarsView;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_List2Dialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				bRes;
	CRect				rect;
	EVAV_cl_ViewItem	*po_NewItem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	bRes = EDIA_cl_BaseDialog::OnInitDialog();
	if(mi_Mode == LIST2_ModeTrigger)
	{
		mpo_VarsView->MyCreate(this, &mst_VarsViewStruct, this);
		GetWindowRect(&rect);
		rect.right += 500;
		MoveWindow(rect);
		L_memcpy(&m_Msg, &pt_Trigger->t_Msg, sizeof(AI_tdst_Message));
		A("Param", EVAV_EVVIT_SubStruct, &m_Msg, EVAV_AutoExpand, LINK_C_MSG_Struct, 0, 0, NULL);
		if(pt_Trigger && pt_Trigger->pt_Proc) po_NewItem->psz_NameKit = pt_Trigger->pt_Proc->asz_Comment;
		mpo_VarsView->SetItemList(&mo_ListItems);
		mpo_VarsView->ShowWindow(SW_SHOW);
		mpo_VarsView->mpo_ListBox->ModifyStyle(0, WS_BORDER);
	}
	
	EnableToolTips(TRUE);
	return bRes;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_List2Dialog::PreTranslateMessage( MSG *pMsg )
{
	CListBox	*po_List2;
	int			i_TopIndex;
	
	if(mi_Mode == LIST2_ModeText)
	{
		if ( (pMsg->message == WM_KEYDOWN ) && (pMsg->wParam == 'T' ) )
		{
			mi_TextDisplay = !mi_TextDisplay;
			po_List2 = (CListBox *) GetDlgItem(IDC_LIST2);
			i_TopIndex = po_List2->GetTopIndex();
			OnSelChange();
			po_List2->SetTopIndex( i_TopIndex );
		}
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage( pMsg );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_List2Dialog::OnSize(UINT n, int cx, int cy)
{
	/*~~~~~~~~~*/
	CRect	rect;
	/*~~~~~~~~~*/

	GetClientRect(&rect);
	cx = rect.Width();
	cy = rect.Height();
	if(mpo_VarsView && IsWindow(mpo_VarsView->m_hWnd))
	{
		rect.left = cx - 495;
		rect.top = 5;
		rect.right = cx - 5;
		rect.bottom = cy - 240;
		mpo_VarsView->MoveWindow(rect);
		mpo_VarsView->ModifyStyle(0, WS_BORDER);

		GetDlgItem(IDC_STATIC)->ShowWindow(SW_SHOW);
		rect.left = cx - 495;
		rect.top = cy - 235;
		rect.bottom = cy - 5; 
		rect.right = cx - 5;
		GetDlgItem(IDC_STATIC)->MoveWindow(rect);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_List2Dialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem				*pst_CurrentElemW;
	TAB_tdst_PFelem				*pst_EndElemW;
	WOR_tdst_World				*pst_World;
	AI_tdst_Instance			*pst_Instance;
	TAB_tdst_PFtable			*pst_AIEOT;
	TAB_tdst_PFelem				*pst_CurrentElem;
	TAB_tdst_PFelem				*pst_EndElem;
	OBJ_tdst_GameObject			*pst_GAO;
	UINT						i, j;
	CListBox					*po_List1;
	BIG_INDEX					h_File;
	BIG_KEY						h_KeyCur;
	CList<BIG_INDEX, BIG_INDEX> o_LstFile;
	int							item, sel, data;
	char						az1[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CDialog::DoDataExchange(pDX);
	po_List1 = (CListBox *) GetDlgItem(IDC_LIST1);

	sel = -1;
	if(mi_Mode == LIST2_ModeTrigger)
	{
		// Current key of current trigger
		h_KeyCur = 0;
		if(pt_Trigger && pt_Trigger->h_KeyFile) 
		{
			h_KeyCur = pt_Trigger->h_KeyFile;
			h_KeyCur = BIG_ul_SearchKeyToFat(h_KeyCur);
			L_strcpy(az1, BIG_NameFile(h_KeyCur));
			*L_strrchr(az1, '.') = 0;
			L_strcat(az1, EDI_Csz_ExtAIEditorFctLib);
			h_KeyCur = BIG_ul_SearchFile(BIG_ParentFile(h_KeyCur), az1);
			if(h_KeyCur != BIG_C_InvalidIndex)
				h_KeyCur = BIG_FileKey(h_KeyCur);
			else
				h_KeyCur = BIG_C_InvalidKey;
		}

		pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
		pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
		for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
		{
			pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
			if(!TAB_b_IsAHole(pst_World))
			{
				pst_AIEOT = &(pst_World->st_AllWorldObjects);
				pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
				pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
				for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
				{
					pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
					if(TAB_b_IsAHole(pst_GAO)) continue;

					if((pst_GAO->pst_Extended) && (pst_GAO->pst_Extended->pst_Ai))
					{
						pst_Instance = (AI_tdst_Instance *) pst_GAO->pst_Extended->pst_Ai;
						if(!pst_Instance->pst_Model) continue;
						for(i = 0; i < pst_Instance->pst_Model->u32_NbProcList; i++)
						{
							h_File = BIG_ul_SearchKeyToFat(pst_Instance->pst_Model->pp_ProcList[i]->h_SourceFile);
							if(!o_LstFile.Find(h_File))
							{
								o_LstFile.AddTail(h_File);
								for(j = 0; j < pst_Instance->pst_Model->pp_ProcList[i]->u16_Num; j++)
								{
									/* Name with a @ before : trigger */
									if(pst_Instance->pst_Model->pp_ProcList[i]->pt_All[j].pz_Name[0] == '@')
									{
										item = po_List1->AddString(BIG_NameFile(h_File));
										po_List1->SetItemData(item, (int) pst_Instance->pst_Model->pp_ProcList[i]);
										if(h_KeyCur == BIG_FileKey(h_File)) sel = (int) pst_Instance->pst_Model->pp_ProcList[i];
										break;
									}
								}
							}
						}
					}
				}
			}
		}

		if(sel != -1)
		{
			for(i = 0; i < (UINT) po_List1->GetCount(); i++)
			{
				data = po_List1->GetItemData(i);
				if(data == sel)
				{
					po_List1->SetCurSel(i);
					break;
				}
			}

			OnSelChange();
//			OnSelChange2();
		}
	}
	else if(mi_Mode == LIST2_ModeText)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		TEXT_tdst_OneText	*pText;
		int					i;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		for(i = 0; i < TEXT_gst_Global.i_Num; i++)
		{
			pText = TEXT_gst_Global.pst_AllTexts[i];
            if(!pText) continue;

			h_File = BIG_ul_SearchKeyToFat(pText->ul_FatherKey);
			if(h_File == BIG_C_InvalidIndex) continue;

			item = po_List1->AddString(BIG_NameFile(h_File));
			po_List1->SetItemData(item, (int) pText);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_List2Dialog::OnSelChange2(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox				*po_List1;
	CListBox				*po_List2;
	int						sel1, sel2;
	SCR_tt_Procedure		*pt_Proc;
	SCR_tt_ProcedureList	*pt_ProcList;
	EVAV_cl_ViewItem		*po_NewItem;
	char					*p;
	BIG_KEY					pos;
	ULONG					len;
	char					*pz1;
	CString					str, str1;
	int						ipos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_Mode == LIST2_ModeTrigger)
	{
		po_List1 = (CListBox *) GetDlgItem(IDC_LIST1);
		sel1 = po_List1->GetCurSel();
		po_List2 = (CListBox *) GetDlgItem(IDC_LIST2);
		sel2 = po_List2->GetCurSel();
		if(sel2 == -1) return;
		sel2 = po_List2->GetItemData(sel2);
		pt_ProcList = (SCR_tt_ProcedureList *) po_List1->GetItemData(sel1);
		pt_Proc = &pt_ProcList->pt_All[sel2];
		mo_ListItems.RemoveAll();
		A("Param", EVAV_EVVIT_SubStruct, &m_Msg, EVAV_AutoExpand, LINK_C_MSG_Struct, 0, 0, NULL);
		po_NewItem->psz_NameKit = pt_Proc->asz_Comment;
		mpo_VarsView->SetItemList(&mo_ListItems);

		pos = BIG_PosFile(BIG_ul_SearchKeyToFat(pt_ProcList->h_SourceFile));
		p = BIG_pc_ReadFileTmp(pos, &len);
		GetDlgItem(IDC_STATIC)->SetWindowText("");

		pz1 = pt_ProcList->pt_All[sel2].pz_Name + 1;
		while(L_isdigit(*pz1)) pz1++;
		str = (char *) p;
		str1 = "///";
		str1 += pz1;
		ipos = str.Find(pz1);
		GetDlgItem(IDC_STATIC)->SetWindowText("");
		if(ipos != -1) 
		{
			pz1 = p + ipos;
			while(*pz1 && *pz1 != '\n') pz1++;
			if(*pz1) pz1++;

			str.Empty();
			while(pz1[0] == '/' && pz1[1] == '/')
			{
				while(*pz1 && *pz1 != '\n') 
				{
					if(*pz1 == '\t') 
					{
						str += "    ";
						pz1++;
					}
					else 
					{
						str += *pz1++;
					}
				}
				if(*pz1) pz1++;
				str += "\r\n";
			}

			((CEdit *) GetDlgItem(IDC_STATIC))->SetWindowText(str);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_List2Dialog::OnSelChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox				*po_List1;
	CListBox				*po_List2;
	int						sel;
	SCR_tt_ProcedureList	*pt_ProcList;
	UINT					i;
	char					*pz;
	int						mysel, data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List1 = (CListBox *) GetDlgItem(IDC_LIST1);
	po_List2 = (CListBox *) GetDlgItem(IDC_LIST2);
	po_List2->ResetContent();
	sel = po_List1->GetCurSel();

	if(mi_Mode == LIST2_ModeTrigger)
	{
		mysel = -1;
		pt_ProcList = (SCR_tt_ProcedureList *) po_List1->GetItemData(sel);
		for(i = 0; i < pt_ProcList->u16_Num; i++)
		{
			if(pt_ProcList->pt_All[i].pz_Name[0] != '@') continue;
			pz = pt_ProcList->pt_All[i].pz_Name + 1;
			while(L_isdigit(*pz)) pz++;
			sel = po_List2->AddString(pz);
			po_List2->SetItemData(sel, i);
			if(pt_Trigger && !L_strcmpi(pt_ProcList->pt_All[i].pz_Name, pt_Trigger->az_Name)) mysel = i;
		}

		if(mysel != -1)
		{
			for(sel = 0; sel < po_List2->GetCount(); sel++)
			{
				data = po_List2->GetItemData(sel);
				if(data == mysel)
				{
					po_List2->SetCurSel(sel);
					break;
				}
			}
		}
	}
	else if(mi_Mode == LIST2_ModeText)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		TEXT_tdst_OneText	*pText;
		char				*sz_Text;
		ULONG				i;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		pText = (TEXT_tdst_OneText *) po_List1->GetItemData(sel);
		for(i = 0; i < pText->ul_Num; i++)
		{
			if (mi_TextDisplay)
			{
				sz_Text = pText->psz_Text + pText->pst_Ids[i].i_Offset;
				if ( !L_strcmp( sz_Text, "<hole>" ) )	
					continue;
				if ( !L_strcmp( sz_Text, "<notext>" ) )	
					continue;
				sel = po_List2->AddString( sz_Text );
			}
			else
				sel = po_List2->AddString(pText->pst_Ids[i].asz_Name);
			po_List2->SetItemData(sel, pText->pst_Ids[i].ul_IdKey);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_List2Dialog::OnOK(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox				*po_List1;
	CListBox				*po_List2;
	SCR_tt_ProcedureList	*pt_ProcList;
	int						sel;
	BIG_INDEX				ul_File;
	char					name[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List1 = (CListBox *) GetDlgItem(IDC_LIST1);
	po_List2 = (CListBox *) GetDlgItem(IDC_LIST2);

	if(mi_Mode == LIST2_ModeTrigger)
	{
		mpo_VarsView->mpo_ListBox->OnSelect();
		L_memcpy(&pt_Trigger->t_Msg, &m_Msg, sizeof(AI_tdst_Message));

		sel = po_List1->GetCurSel();
		if(sel != -1)
		{
			pt_ProcList = (SCR_tt_ProcedureList *) po_List1->GetItemData(sel);
			sel = po_List2->GetItemData(po_List2->GetCurSel());
			if(sel != -1)
			{
				L_strcpy(pt_Trigger->az_Name, pt_ProcList->pt_All[sel].pz_Name);
				pt_Trigger->h_KeyFile = pt_ProcList->h_SourceFile;

				/* Get engine file */
				ul_File = BIG_ul_SearchKeyToFat(pt_Trigger->h_KeyFile);
				L_strcpy(name, BIG_NameFile(ul_File));
				*L_strrchr(name, '.') = 0;
				L_strcat(name, ".fce");
				ul_File = BIG_ul_SearchFile(BIG_ParentFile(ul_File), name);
				pt_Trigger->h_KeyFile = BIG_FileKey(ul_File);

				pt_Trigger->pt_Proc = pt_ProcList->pt_All + sel;
			}
		}
	}
	else if(mi_Mode == LIST2_ModeText)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		TEXT_tdst_OneText	*pText;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		sel = po_List1->GetCurSel();
		pText = (TEXT_tdst_OneText *) po_List1->GetItemData(sel);
		if(pText && ((int) pText != -1))
		{
			*(ULONG *) &mp_TextRef->i_FileKey = pText->ul_FatherKey;

			sel = po_List2->GetCurSel();
			mp_TextRef->i_Id = po_List2->GetItemData(sel);
		}
		else
		{
			mp_TextRef->i_FileKey = -1;
			mp_TextRef->i_Id = -1;
		}
	}

	EDIA_cl_BaseDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_List2Dialog::OnSetNull(void)
{
	if(mi_Mode == LIST2_ModeTrigger)
	{
		pt_Trigger->az_Name[0] = 0;
		pt_Trigger->h_KeyFile = 0;
		pt_Trigger->pt_Proc = NULL;
	}
	else if(mi_Mode == LIST2_ModeText)
	{
		mp_TextRef->i_FileKey = -1;
		mp_TextRef->i_Id = -1;
	}

	EDIA_cl_BaseDialog::OnOK();
}
#endif /* ACTIVE_EDITORS */
