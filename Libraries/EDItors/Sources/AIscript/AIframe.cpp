/*$T AIframe.cpp GC! 1.100 03/07/01 15:46:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGgroup.h"
#include "DIAlogs/DIAfindai_dlg.h"
#include "EDIapp_options.h"
#include "EDItors/Sources/OUTput/OUTframe.h"

#define ACTION_GLOBAL

#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "AIscript/AIframe_act.h"
#include "AIframe.h"
#include "AIerrid.h"
#include "EditView/AIview.h"
#include "EditView/AIleftview.h"
#include "EditView/AIpane.h"
#include "AIleftframe.h"
#include "EDImainframe.h"
#include "EDIapp.h"
#include "EDIpaths.h"
#include "AIstrings.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "LINKs/LINKmsG.h"
#include "EDIstrings.h"
#include "EDIerrid.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AImsg.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDImsg.h"
#include "EDImainframe.h"
#include "DIAlogs/DIAchecklist_dlg.h"
#include "DIAlogs/DIAcompletion_dlg.h"
#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "MATHs/MATH.h"
#include "Res/Res.h"
#include "LINKs/LINKtoed.h"

#include "PERForce/PERmsg.h"
#include "EDImainframe_act.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Types.
 =======================================================================================================================
 */
#define DEFINE_CONSTANT(a, b, c)
#define DEFINE_TYPE(a, b, c, d, e)	{ b, c, d },
EIA_tdst_Type	EAI_gast_Types[] =
{
#include "AIinterp/sources/Types/AIdeftyp.h"
};

/*
 =======================================================================================================================
    Constants.
 =======================================================================================================================
 */
#define DEFINE_TYPE(a, b, c, d, e)
#define DEFINE_CONSTANT(a, b, c)	{ a, b, c },
EIA_tdst_Constant	EAI_gast_Constants[] =
{
#include "AIinterp/sources/Types/AIdeftyp.h"
};

/*
 =======================================================================================================================
    Keywords.
 =======================================================================================================================
 */
#define DEFINE_KEYWORD(a, b, c, d)	{ b, c },
EIA_tdst_Keyword	EAI_gast_Keywords[] =
{
#include "AIinterp/sources/Keywords/AIdefkey.h"
};

/*
 =======================================================================================================================
    Functions.
 =======================================================================================================================
 */
#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j)	{ i, b, c, h, d, #d, #e, 0, g },
EIA_tdst_Function	EAI_gast_Functions[] =
{
#include "AIinterp/sources/Functions/AIdeffct.h"
};

/*
 =======================================================================================================================
    Fields.
 =======================================================================================================================
 */
#define DEFINE_FIELD(a, b, c, d, e, f)	{ b, c, d, e },
EIA_tdst_Field	EAI_gast_Fields[] =
{
#include "AIinterp/sources/Fields/AIdeffields.h"
};

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EAI_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EAI_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

extern CMapStringToString	go_PPConstants;
extern CMapStringToString	go_PPConstantsFiles;
extern CMapStringToString	go_PPConstantsLines;
extern CMapStringToPtr		go_PPMacros;
extern CMapStringToPtr		go_PPMacrosLines;
extern CMapStringToPtr		go_PPFunctions;
int							EIA_gi_NumFct = 0;
extern BOOL					AI_gb_Compile1;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Frame::i_ComputeParamsTypes(char *_psz_String, int *_pi_Res, int *_pi_PRes)
{
	/*~~~~~~~~~~~~~~~~~*/
	int		i_Num;
	int		i;
	char	*psz_Temp;
	char	asz_Dup[512];
	/*~~~~~~~~~~~~~~~~~*/

	i_Num = 0;

	L_strcpy(asz_Dup, _psz_String);
	_psz_String = asz_Dup;

	while(*_psz_String)
	{
		while(L_isspace(*_psz_String)) _psz_String++;
		if(!*_psz_String) break;

		psz_Temp = _psz_String;
		while(*_psz_String == '&' || L_isalpha(*_psz_String)) _psz_String++;
		if(*_psz_String) *_psz_String++ = 0;

		for(i = 0; i < sizeof(EAI_gast_Types) / sizeof(EAI_gast_Types[0]); i++)
		{
			if
			(
				(*psz_Temp == '&' && !L_strcmpi(EAI_gast_Types[i].psz_Name, psz_Temp + 1))
			||	(*psz_Temp != '&' && !L_strcmpi(EAI_gast_Types[i].psz_Name, psz_Temp))
			)
			{
				if(L_strcmpi(psz_Temp, "VOID"))
				{
					if(*psz_Temp == '&')
						*_pi_PRes++ = 1;
					else
						*_pi_PRes++ = 0;
					i_Num++;
					*_pi_Res++ = EAI_gast_Types[i].w_ID;
				}
				break;
			}
		}
	}

	return i_Num;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_Frame::EAI_cl_Frame(void)
{
	/*~~*/
	int i;
	/*~~*/

	mpo_Edit = new EAI_cl_View;
	mpo_LeftView = new EAI_cl_LeftView;
	mpo_Splitter = new CSplitterWnd;
	mpo_ListAI = new EAI_cl_LeftFrame;
	mpo_InsideWnd = new EAI_cl_InsideFrame;

	mpo_Pane1 = new EAI_cl_Pane(1);
	mpo_Pane1->mpo_Frame = this;
	mpo_Pane2 = new EAI_cl_Pane(2);
	mpo_Pane2->mpo_Frame = this;
	mpo_Pane3 = new EAI_cl_Pane(3);
	mpo_Pane3->mpo_Frame = this;

	mo_Compiler.mpo_Frame = this;
	mb_CanReload = TRUE;
	mb_ListMode = FALSE;
	mb_CanAskVss = TRUE;
	mb_HasTouchedVars = FALSE;
	mpo_FindDlg = NULL;
	mb_CompileAll = FALSE;
	mb_CompileDep = FALSE;
	mb_UpdateOwner = TRUE;
	mb_RecurseError = FALSE;
	mb_RefreshLeft = FALSE;

	mul_CurrentEditFile = BIG_C_InvalidIndex;
	mul_CurrentEditModel = BIG_C_InvalidIndex;
	mul_CurrentEditInstance = BIG_C_InvalidIndex;
	mul_CurrentEngineModel = BIG_C_InvalidIndex;
	mpst_Model = NULL;
	mpst_Instance = NULL;
	mb_LockHistory = FALSE;

	mb_P4CloseWorld = FALSE;

	/* Add types to list */
	for(i = 0; i < sizeof(EAI_gast_Types) / sizeof(EAI_gast_Types[0]); i++)
		mo_TypeList.SetAt(EAI_gast_Types[i].psz_Name, (void *) i);

	/* Add constants to list */
	for(i = 0; i < sizeof(EAI_gast_Constants) / sizeof(EAI_gast_Constants[0]); i++)
		mo_ConstantList.SetAt(EAI_gast_Constants[i].psz_Name, (void *) i);

	/* Add keywords */
	for(i = 0; i < sizeof(EAI_gast_Keywords) / sizeof(EAI_gast_Keywords[0]); i++)
		mo_KeywordList.SetAt(EAI_gast_Keywords[i].psz_Name, (void *) i);

	/* Add functions */
	EIA_gi_NumFct = sizeof(EAI_gast_Functions) / sizeof(EAI_gast_Functions[0]);
	for(i = 0; i < sizeof(EAI_gast_Functions) / sizeof(EAI_gast_Functions[0]); i++)
	{
		mo_FunctionList.SetAt(EAI_gast_Functions[i].psz_Name, (void *) i);
		EAI_gast_Functions[i].i_NumPars = i_ComputeParamsTypes
			(
				EAI_gast_Functions[i].psz_Types,
				EAI_gast_Functions[i].ai_TypePars,
				EAI_gast_Functions[i].ai_PTypePars
			);
	}

	/* Add fields */
	for(i = 0; i < sizeof(EAI_gast_Fields) / sizeof(EAI_gast_Fields[0]); i++)
		mo_FieldList.SetAt(EAI_gast_Fields[i].psz_Name, (void *) i);

	/* Add pp directives */
	mo_PPList.SetAt("#define", (void *) 0);
	mo_PPList.SetAt("#undef", (void *) 0);
	mo_PPList.SetAt("#include", (void *) 0);
	mo_PPList.SetAt("#ifdef", (void *) 0);
	mo_PPList.SetAt("#ifndef", (void *) 0);
	mo_PPList.SetAt("#endif", (void *) 0);

	/* Add reserved keywords */
	mo_ReservedList.SetAt("track0", (void *) 0);
	mo_ReservedList.SetAt("track1", (void *) 0);
	mo_ReservedList.SetAt("track2", (void *) 0);
	mo_ReservedList.SetAt("track3", (void *) 0);
	mo_ReservedList.SetAt("track4", (void *) 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_Frame::~EAI_cl_Frame(void)
{
	mpo_Splitter->DestroyWindow();
	delete mpo_Splitter;

	mpo_InsideWnd->DestroyWindow();
	delete mpo_InsideWnd;

	mpo_ListAI->DestroyWindow();
	delete mpo_ListAI;

	mpo_Edit->DestroyWindow();
	delete mpo_Edit;

	delete mpo_Pane1;
	delete mpo_Pane2;
	delete mpo_Pane3;

	if(mpo_FindDlg) delete mpo_FindDlg;

	mo_Compiler.Reset(TRUE);
	ResetRefCompilers();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CCreateContext	o_Context;
	CRect			o_Rect;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1) return -1;

	mpo_Splitter->CreateStatic(this, 1, 2);

	/* List of functions */
	mpo_ListAI->mpo_Parent = this;
	mpo_ListAI->Create
		(
			LVS_SINGLESEL | WS_VISIBLE | LVS_REPORT | LVS_SORTASCENDING | LVS_SHOWSELALWAYS,
			CRect(0, 0, 0, 0),
			mpo_Splitter,
			mpo_Splitter->IdFromRowCol(0, 0)
		);

	mpo_ListAI->SetImageList(&(M_MF()->mo_FileImageList), LVSIL_SMALL);
	mpo_ListAI->SetFont(&M_MF()->mo_Fnt);

	/* Frame inside the other pane */
	mpo_InsideWnd->mpo_Parent = this;
	mpo_InsideWnd->Create
		(
			NULL,
			NULL,
			WS_CHILD | WS_VISIBLE,
			CRect(0, 0, 0, 0),
			mpo_Splitter,
			mpo_Splitter->IdFromRowCol(0, 1)
		);
	mpo_InsideWnd->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);

	/* Create left frame. */
	mpo_LeftView->Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, CRect(0, 0, 0, 0), mpo_InsideWnd);
	mpo_LeftView->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	mpo_LeftView->mpo_Frame = this;

	/* Create edit view */
	mpo_Edit->Create
		(
			ES_NOHIDESEL |
				WS_BORDER |
				ES_AUTOVSCROLL |
				ES_AUTOHSCROLL |
				WS_HSCROLL |
				WS_VSCROLL |
				WS_VISIBLE |
				WS_CHILD |
				ES_MULTILINE,
			CRect(0, 0, 0, 0),
			mpo_InsideWnd,
			10
		);
	mpo_Edit->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
	mpo_Edit->mpo_Frame = this;

	/* Tooltip */
	EnableToolTips(TRUE);
	mpo_InsideWnd->EnableToolTips(TRUE);

	/* Top pane */
	mpo_Pane1->SubclassDlgItem(IDC_PATH, mpo_DialogBar);
	mpo_Pane1->SetFont(&M_MF()->mo_Fnt);
	mpo_Pane2->SubclassDlgItem(IDC_FILE, mpo_DialogBar);
	mpo_Pane2->SetFont(&M_MF()->mo_Fnt);
	mpo_Pane3->SubclassDlgItem(IDC_INFOS, mpo_DialogBar);
	mpo_Pane3->SetFont(&M_MF()->mo_Fnt);

	EDIA_cl_BaseDialog::SetTheme(this);
	mpo_ListAI->InsertColumn(0, EAI_STR_Csz_NameList);
	mpo_ListAI->SetColumnWidth(0, 100);
	mpo_ListAI->InsertColumn(1, EAI_STR_Csz_TypeList);
	mpo_ListAI->SetColumnWidth(1, 40);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnSize(UINT nType, int cx, int cy)
{
	/*~~~~~~~~~~~~*/
	CRect	o_Rect;
	int		cxx, w1, w2;
	CWnd	*po_Wnd;
	/*~~~~~~~~~~~~*/

	/* Call parent function */
	EDI_cl_BaseFrame::OnSize(nType, cx, cy);

	mpo_DialogBar->GetWindowRect(o_Rect);
	cxx = o_Rect.right - o_Rect.left;
    
    w1 = 3;
	w2 = 4;

	/* Move statics inside dialog */
	po_Wnd = mpo_DialogBar->GetDlgItem(IDC_PATH);
	po_Wnd->GetWindowRect(&o_Rect);
	mpo_DialogBar->ScreenToClient(&o_Rect);
	o_Rect.left = 2;
	o_Rect.right = o_Rect.left + cxx / w1;
	po_Wnd->MoveWindow(&o_Rect);

	po_Wnd = mpo_DialogBar->GetDlgItem(IDC_FILE);
	o_Rect.left = o_Rect.right + 2;
	o_Rect.right = o_Rect.left + cxx / w2;
	po_Wnd->MoveWindow(&o_Rect);

	po_Wnd = mpo_DialogBar->GetDlgItem(IDC_INFOS);
	o_Rect.left = o_Rect.right + 2;
	o_Rect.right = cxx;
	po_Wnd->MoveWindow(&o_Rect);
}

CList<BIG_INDEX, BIG_INDEX>	o_SkipMdl;

#ifdef JADEFUSION
extern int AI2C_i_C_CodeGenerationIsEnabled(ULONG key);
extern int AI2C_i_LoadListModel(void);
extern void AI2C_UnloadListModel(void);
extern void AI2C_LoadDisableCModelList(void);
extern void AI2C_UnloadDisableCModelList(void);
extern BOOL LOA_gb_SlashY;
#else
extern "C" int AI2C_i_C_CodeGenerationIsEnabled(ULONG key);
extern "C" int AI2C_i_LoadListModel(void);
extern "C" void AI2C_UnloadListModel(void);
extern "C" void AI2C_LoadDisableCModelList(void);
extern "C" void AI2C_UnloadDisableCModelList(void);
extern "C" BOOL LOA_gb_SlashY;
#endif

/*
 =======================================================================================================================
    Aim:    When no message, we colorize the file to the last position to a new one. This avoid to colorize the whole
            file when it's loaded in editor.
 =======================================================================================================================
 */
void EAI_cl_Frame::OnRealIdle(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			i_Beg;
	LONG		l_First, l_Last, l_First1, l_Last1;
	BIG_INDEX	ul_File;
	char		asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if 0
	if(	mpo_Edit->GetModify() && BIG_P4Action(mul_CurrentEditFile)[0] == 0 	)
	{
		BOOL isCheckout = FALSE;
		LINK_SendMessageToEditors(EPER_MESSAGE_AUTOCHECKOUT, mul_CurrentEditFile, ((ULONG)&isCheckout));
		if (!isCheckout) Load(mul_CurrentEditFile);
	}
#endif

	/* Auto compile all models */
	if(EDI_gb_SlashC)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		EOUT_cl_Frame	*po_Out;
		char			*pz_Temp;
		static int		irr = 0;
		int				bbb = 0;
		/*~~~~~~~~~~~~~~~~~~~~~*/

		if(irr) return;
		irr = 1;

		if(AI2C_i_LoadListModel())
		{
			ULONG	ii, key;
			BIG_INDEX	ul_File;
			char azpath[1024];
			char azfile[1024];

			AI2C_LoadDisableCModelList();

			for(ii = 0; ii < BIG_MaxFile(); ii++)
			{
				if(BIG_FileChanged(ii) == EDI_FHC_Deleted) continue;
				if(BIG_b_IsFileExtension(ii, ".omd"))
				{
					
					key = BIG_FileKey(ii);
					bbb = AI2C_i_C_CodeGenerationIsEnabled(key);

					if(!bbb)
					{
						L_strcpy(azfile, BIG_NameFile(ii));
						*L_strrchr(azfile, '.') = 0;
						L_strcat(azfile, ".mdl");
						BIG_ComputeFullName(BIG_ParentFile(ii), azpath);
						*L_strrchr(azpath, '/') = 0;
						ul_File = BIG_ul_SearchFileExt(azpath, azfile);
						if(ul_File != BIG_C_InvalidIndex) 
						{
							o_SkipMdl.AddTail(ul_File);
//							*L_strrchr(BIG_NameFile(ul_File), '.') = 0;
						}
					}
				}
			}
			
			AI2C_UnloadDisableCModelList();
			AI2C_UnloadListModel();
		}
        else
        {
			if(LOA_gb_SlashY)
			{
				ERR_X_Warning(0, "Compil all for IA2C", "jade.mdl can not be opened");
			}
        }

		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		if(po_Out)
		{
			strcpy(asz_Path, EDI_Csz_Path_Levels);
			strcat(asz_Path, "/");
			strcat(asz_Path, EDI_gaz_SlashC);
			pz_Temp = L_strrchr(asz_Path, '/');
			if(pz_Temp)
			{
				*pz_Temp = 0;
				ul_File = BIG_ul_SearchFileExt(asz_Path, pz_Temp + 1);
				if(ul_File != BIG_C_InvalidIndex)
				{
#ifdef JADEFUSION
					EDI_gpo_EnterWnd->DisplayMessage("Loading world...");
#else
					if (EDI_gpo_EnterWnd) EDI_gpo_EnterWnd->DisplayMessage("Loading world...");
#endif
					po_Out->i_OnMessage(EDI_MESSAGE_SELFILE, 0, ul_File);
#ifdef JADEFUSION
					EDI_gpo_EnterWnd->DisplayMessage("Compiling AI models...");
#else
					if (EDI_gpo_EnterWnd) EDI_gpo_EnterWnd->DisplayMessage("Compiling AI models...");
#endif
					mst_Ini.b_LookDate = FALSE;
					AI_gb_Compile1 = FALSE;
					OnCompileAllModels();
#ifdef JADEFUSION
					M_MF()->OnAction(EDI_ACTION_CLOSEPROJECT);
					M_MF()->OnAction(EDI_ACTION_EXIT);
#endif
					ExitProcess(0);
				}
			}
		}
	}

	if(mb_RefreshLeft)
	{
		mpo_LeftView->Invalidate();
		mpo_LeftView->RedrawWindow
			(
				NULL,
				NULL,
				RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW | RDW_FRAME | RDW_ALLCHILDREN
			);
		mb_RefreshLeft = FALSE;
	}

	if(!mpo_Edit->mb_FirstColoring) return;

	i_Beg = mpo_Edit->mi_LastColLine;

	/* Prevent OnChange and OnProtected of edit control to be call */
	mpo_Edit->SetEventMask(0);

	l_First = l_First1 = mpo_Edit->l_FirstVisibleLine();
	l_Last = l_Last1 = mpo_Edit->l_LastVisibleLine();

	if((l_First == mpo_Edit->mi_FirstColLine) && (l_Last == mpo_Edit->mi_LastColLine)) 
	{
		mpo_Edit->SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED);
		return;
	}

	if((l_First < mpo_Edit->mi_FirstColLine) && (l_Last < mpo_Edit->mi_FirstColLine))
	{
	}
	else if((l_First < mpo_Edit->mi_FirstColLine) && (l_Last > mpo_Edit->mi_FirstColLine))
	{
		l_Last = mpo_Edit->mi_FirstColLine;
	}
	else if((l_First < mpo_Edit->mi_LastColLine) && (l_Last > mpo_Edit->mi_LastColLine))
	{
		l_First = mpo_Edit->mi_LastColLine;
	}

	/* First color visible lines */
	mpo_Edit->mi_FirstColLine = l_First1;
	mpo_Edit->mi_LastColLine = l_Last1;
	mpo_Edit->ColorRange(l_First, l_Last);

	/* Restore event mask */
	mpo_Edit->SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED);

	

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OneTrameEnding(void)
{
	/*~~~~~~~~~~~*/
	CPoint	pt;
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	if
	(
		M_MF()->mpo_MaxView
	&&	M_MF()->mpo_MaxView != mpo_MyView
	&&	!mpo_MyView->mb_Floating
	&&	M_MF()->mst_Desktop.b_VeryMaximized
	&&	mb_IsActivate
	) return;
	if(LINK_gi_SpeedDraw != 0) return;

	OnRealIdle();
	if(!mpo_Edit->mo_DisplayVar.IsEmpty())
	{
		GetCursorPos(&pt);
		mpo_Edit->GetWindowRect(&o_Rect);
		if(o_Rect.PtInRect(pt)) mpo_Edit->DisplayVar(mpo_Edit->mo_DisplayVar, TRUE);
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::b_AcceptToCloseProject(void)
{
	if(mul_CurrentEditFile == BIG_C_InvalidIndex) return TRUE;
	return b_CanClose();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::CloseProject(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::b_CanActivate(void)
{
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::ForceRefresh(void)
{
	/* Set background color */
	mpo_Edit->SetBackgroundColor(FALSE, mst_Ini.ax_Colors[EAI_C_ColorBkGnd]);

	/* Color current visible part */
	mpo_Edit->ColorRange(mpo_Edit->l_FirstVisibleLine(), mpo_Edit->l_LastVisibleLine());

	/* Color top of file */
	mpo_Edit->mb_FirstColoring = TRUE;
	mpo_Edit->mi_LastColLine = -1;
//	mpo_Edit->ColorRange(0, mpo_Edit->mi_LastColLine);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::InitListAI(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_SizeModel;
	ULONG				i, i_Res;
	BIG_INDEX			ul_File;
	char				*psz_Temp;
	LV_ITEM				st_ListCtrlItem;
	char				asz_Name[BIG_C_MaxLenName];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mb_ListMode) return;
	if(mb_CompileAll) return;
	if(mb_CompileDep) return;
	if(mul_CurrentEditModel == BIG_C_InvalidIndex) return;

	/* Read model, and add each function/var in AI list */
	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(mul_CurrentEditModel), &ul_SizeModel);

	mpo_ListAI->DeleteAllItems();
	for(i = 0; i < (ul_SizeModel / sizeof(BIG_tdst_GroupElem)); i++)
	{
		if(!pst_Buf[i].ul_Key) continue;
		ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
		if(ul_File == BIG_C_InvalidIndex) continue;
		psz_Temp = L_strrchr(BIG_NameFile(ul_File), '.');
		if(!psz_Temp) continue;

		if((!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorVars)) || (!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFct) || !L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFctLib)))
		{
			st_ListCtrlItem.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
			st_ListCtrlItem.iItem = 0;
			st_ListCtrlItem.iSubItem = 0;
			st_ListCtrlItem.iImage = M_MF()->i_GetIconImage(NULL, BIG_NameFile(ul_File));
			L_strcpy(asz_Name, BIG_NameFile(ul_File));
			psz_Temp = L_strrchr(asz_Name, '.');
			*psz_Temp = 0;
			st_ListCtrlItem.pszText = asz_Name;
			st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
			st_ListCtrlItem.lParam = ul_File;
			i_Res = mpo_ListAI->InsertItem(&st_ListCtrlItem);

			st_ListCtrlItem.mask = LVIF_TEXT;
			st_ListCtrlItem.iItem = i_Res;
			st_ListCtrlItem.iSubItem = 1;
			st_ListCtrlItem.pszText = psz_Temp + 1;
			st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
			mpo_ListAI->SetItem(&st_ListCtrlItem);
		}
	}

	mpo_ListAI->SortList();
	mpo_ListAI->Invalidate();
	L_free(pst_Buf);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::RefreshDialogBar(void)
{
	mpo_DialogBar->Invalidate();
	mpo_DialogBar->GetDlgItem(IDC_PATH)->Invalidate();
	mpo_DialogBar->GetDlgItem(IDC_FILE)->Invalidate();
	mpo_DialogBar->GetDlgItem(IDC_INFOS)->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::DisplayPaneNames(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				o_Name;
	LINK_tdst_Pointer	*p2;
	char				asz_CurrentOwner[256];
	char				asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mul_CurrentEditFile != BIG_C_InvalidIndex)
	{
		/* Current owner */
		*asz_CurrentOwner = 0;
		//if(M_MF()->mst_Ini.b_LinkControlON)
		{
			L_strcpy(asz_CurrentOwner, BIG_P4OwnerName(mul_CurrentEditFile));
			if(!L_strnicmp(asz_CurrentOwner, "_nobody_", 8)) *asz_CurrentOwner = 0;
		}

		if(*asz_CurrentOwner)
		{
			sprintf(asz_Path, "%s   (%s)", BIG_NameDir(BIG_ParentFile(mul_CurrentEditFile)), asz_CurrentOwner);
			mpo_DialogBar->GetDlgItem(IDC_PATH)->SetWindowText(asz_Path);
		}
		else
		{
			mpo_DialogBar->GetDlgItem(IDC_PATH)->SetWindowText(BIG_NameDir(BIG_ParentFile(mul_CurrentEditFile)));
		}

		mpo_DialogBar->GetDlgItem(IDC_FILE)->SetWindowText(BIG_NameFile(mul_CurrentEditFile));
	}
	else
	{
		mpo_DialogBar->GetDlgItem(IDC_PATH)->SetWindowText("");
		mpo_DialogBar->GetDlgItem(IDC_FILE)->SetWindowText("");
	}

	if(mpst_Instance)
	{
		p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer(mpst_Instance);
		if(!p2) 
		{
			mpst_Instance = NULL;
		}
	}

	if(mul_CurrentEditModel != BIG_C_InvalidIndex)
	{
		/* Engine instance with file */
		if(mul_CurrentEditInstance != BIG_C_InvalidIndex)
		{
			o_Name = "";
			o_Name += BIG_NameFile(mul_CurrentEditInstance);
		}

		/* Engine instance without file (generated object) */
		else if(mpst_Instance)
		{
			o_Name = "";
			o_Name += p2->asz_Name;
		}

		/* Else model */
		else
		{
			o_Name = "";
			o_Name += BIG_NameFile(mul_CurrentEditModel);
		}

		if(mpst_Instance && mpst_Instance->pst_GameObject)
		{
			o_Name += "  [";
			o_Name += ((OBJ_tdst_GameObject *) mpst_Instance->pst_GameObject)->sz_Name;
			o_Name += "]";
		}

		mpo_DialogBar->GetDlgItem(IDC_INFOS)->SetWindowText(o_Name);
	}
	else
	{
		mpo_DialogBar->GetDlgItem(IDC_INFOS)->SetWindowText("");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::Load(BIG_INDEX _ul_Index)
{
	/*~~~~~~~~~~~~~~~~*/
	char		*pc_Buf;
	ULONG		ul_Size;
	POSITION	pos;
	CHARRANGE	cr;
	int			i;
	BOOL		bSel;
	/*~~~~~~~~~~~~~~~~*/

	/* Reset undo manager */
	mpo_Edit->mo_UndoManager.Clean();

	/* Current file */
	if(mb_LockHistory)
	{
		bSel = FALSE;
	}
	else
	{
		pos = mo_History.GetHeadPosition();
		i = 0;
		while(pos)
		{
			ul_Size = mo_History.GetNext(pos);
			if(ul_Size == mul_CurrentEditFile) break;
			i++;
		}
		if(i != mo_History.GetCount())
		{
			pos = mo_HistoryCR.FindIndex(i);
			mpo_Edit->GetSel(cr);
			mo_HistoryCR.SetAt(pos, cr);
		}

		/* Function history */
		pos = mo_History.GetHeadPosition();
		i = 0;
		while(pos)
		{
			ul_Size = mo_History.GetNext(pos);
			if(ul_Size == _ul_Index) break;
			i++;
		}

		bSel = TRUE;
		if(i == mo_History.GetCount())
		{
			mo_History.AddTail(_ul_Index);
			cr.cpMin = cr.cpMax = 0;
			mo_HistoryCR.AddTail(cr);
			bSel = FALSE;
		}
	}

	ERR_X_Error(_ul_Index != BIG_C_InvalidIndex, "Unknown AI file", NULL);

	/* Read the file */
	mul_CurrentEditFile = _ul_Index;
	mpo_ListAI->SelectFromFile();
	pc_Buf = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_ul_Index), &ul_Size);

	mpo_Edit->Init();
	mpo_Edit->LimitText(ul_Size * 2);
	mpo_LeftView->Init();

	/* Dialog bar infos */
	DisplayPaneNames();
	M_MF()->LockDisplay(mpo_Edit);
	mpo_Edit->SetEventMask(0);

	/* Set text in edit */
	if(ul_Size)
		mpo_Edit->SetWindowText(pc_Buf);
	else
		mpo_Edit->SetWindowText("");

	/* Colorize the visible part */
	mpo_Edit->mi_LastColLine = 0;
	mpo_Edit->ColorRange(0, mpo_Edit->l_LastVisibleLine());

	mpo_Edit->SetEventMask(ENM_CHANGE | ENM_SELCHANGE | ENM_PROTECTED);
	if(bSel)
	{
		cr = mo_HistoryCR.GetAt(mo_HistoryCR.FindIndex(i));
		mpo_Edit->SetSel(cr);
	}

	M_MF()->UnlockDisplay(mpo_Edit);

	mpo_Edit->SetModify(FALSE);

	/* Refresh left menu */
	RefreshMenu();

	L_free(pc_Buf);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void EAI_cl_Frame::OnSave(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString o_Text;
	char	asz_Path[BIG_C_MaxLenPath];
	char	*psz_Buf;
	ULONG	ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpo_Edit->GetModify()) return;

	BOOL isCheckout = FALSE;
#if 0
	if (	BIG_P4Action(mul_CurrentEditFile)[0] == 0  )
	{
		LINK_SendMessageToEditors(EPER_MESSAGE_AUTOCHECKOUT, mul_CurrentEditFile, ((ULONG)&isCheckout));
	}
	else 
#endif
	{
		isCheckout = TRUE;
	}

	if ( isCheckout ) 
	{
	mpo_Edit->GetWindowText(o_Text);
	BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditFile), asz_Path);
	psz_Buf = (char *) (LPCSTR) o_Text;
	ul_Size = o_Text.GetLength();
	BIG_UpdateFileFromBuffer(asz_Path, BIG_NameFile(mul_CurrentEditFile), psz_Buf, ul_Size);

	/* Message */
	L_strcpy(asz_Path, EAI_STR_Csz_Saving);
	L_strcat(asz_Path, BIG_NameDir(BIG_ParentFile(mul_CurrentEditFile)));
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, BIG_NameFile(mul_CurrentEditFile));
	L_strcat(asz_Path, EAI_STR_Csz_OK);
	LINK_PrintStatusMsg(asz_Path);

	/* The file is not modified now */
	mpo_Edit->SetModify(FALSE);
}
	else 
	{
		Load(mul_CurrentEditFile);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::b_CanClose(void)
{
	/*~~~~~~~~~~*/
	int		i_Res;
	CString o_Str;
	/*~~~~~~~~~~*/

	if(mul_CurrentEditFile == BIG_C_InvalidIndex) return TRUE;
	if(mpo_Edit->GetModify())
	{
		if(mst_Ini.b_AutoSave)
			OnSave();
		else
		{
			/* Force editor to be activate */
			mpo_MyView->IWantToBeActive(this);

			/* Ask user to close without saving */
			o_Str = "AI Script, file ";
			o_Str += BIG_NameFile(mul_CurrentEditFile);
			o_Str += "\n";
			o_Str += EAI_STR_Csz_FileModified;
			i_Res = M_MF()->MessageBox
				(
					(char *) (LPCSTR) o_Str,
					EDI_STR_Csz_TitleConfirm,
					MB_ICONQUESTION | MB_YESNOCANCEL
				);
			if(i_Res == IDCANCEL) return FALSE;
			if(i_Res == IDYES) OnSave();
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::OnClose(BOOL _b_Forced, BOOL _b_All)
{
	if(!_b_Forced)
	{
		if(!b_CanClose()) return FALSE;
	}

	/* Erase content of info panes */
	mb_HasTouchedVars = FALSE;
	mpo_DialogBar->GetDlgItem(IDC_FILE)->SetWindowText("");
	mpo_DialogBar->GetDlgItem(IDC_PATH)->SetWindowText("");
	if(_b_All) mpo_DialogBar->GetDlgItem(IDC_INFOS)->SetWindowText("");
	mo_History.RemoveAll();
	mo_HistoryCR.RemoveAll();
	for(int i = 0; i < sizeof(mo_HistoryMarks) / sizeof(CList<int, int>); i++)
		mo_HistoryMarks[i].RemoveAll();

	/* Free current file */
	if(!mb_ListMode)
	{
		mpo_Edit->SetWindowText("");
		mpo_Edit->SetModify(FALSE);
		mul_CurrentEditFile = BIG_C_InvalidIndex;
	}

	/* Free current model and instance */
	if(_b_All)
	{
		/* Reset instance and model */
		mpst_Instance = NULL;
		mpst_Model = NULL;
		mul_CurrentEditModel = BIG_C_InvalidIndex;
		mo_ModelDateList.RemoveAll();
		mul_CurrentEditInstance = BIG_C_InvalidIndex;

		/* List of all functions */
		go_PPFunctions.RemoveAll();

		/* Reset undo manager */
		mpo_Edit->mo_UndoManager.Clean();
	}

	mpo_Edit->Invalidate();
	mpo_LeftView->Invalidate();

	if(!mb_ListMode && _b_All)
	{
		mpo_ListAI->DeleteAllItems();
		RefreshMenu();
	}

	return TRUE;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnNewGlobalFunction(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EAI_STR_Csz_TitleNewGlobFct);
	char				asz_Name[BIG_C_MaxLenName];
	char				*psz_Name;
	BIG_INDEX			ul_Func;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	if(o_Dialog.DoModal() == IDOK)
	{
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(BIG_b_CheckName(psz_Name) == FALSE) return;

		L_strcpy(asz_Name, psz_Name);
		L_strcat(asz_Name, EDI_Csz_ExtAIEditorFct);
		if(BIG_ul_SearchFileExt(EDI_Csz_Path_AILib, asz_Name) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError(EAI_ERR_Csz_FunctionAlreadyExists, psz_Name);
		}
		else
		{
			BIG_ul_CreateDir(EDI_Csz_Path_AILib);
			ul_Func = BIG_ul_CreateFile(EDI_Csz_Path_AILib, asz_Name);

			/* Inform all editors */
			M_MF()->FatHasChanged();

			/* Set created model as current one */
			mul_CurrentEditModel = BIG_C_InvalidIndex;
			mo_ModelDateList.RemoveAll();
			Load(ul_Func);
		}
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnNewProcList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog("Enter name of file");
	char				asz_Temp[BIG_C_MaxLenPath];
	char				asz_Temp1[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenPath];
	char				*psz_Name, *psz_Temp;
	BIG_INDEX			ul_Group, ul_Func;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!b_CanClose()) return;
_Try_
	if(o_Dialog.DoModal() == IDOK)
	{
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(BIG_b_CheckName(psz_Name) == FALSE) return;

		/* Compute model name */
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Temp);
		L_strcat(asz_Temp, "/");
		L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
		psz_Temp = L_strrchr(asz_Name, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcat(asz_Temp, asz_Name);

		/* Test if file is already here */
		L_strcpy(asz_Name, psz_Name);
		L_strcat(asz_Name, EDI_Csz_ExtAIEditorFctLib);
		if(BIG_ul_SearchFileExt(asz_Temp, asz_Name) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("File already exists", asz_Name);
		}
		else
		{
			L_strcpy(asz_Temp1, asz_Temp);
			*L_strrchr(asz_Temp1, '/') = 0;

			/* Get model file index */
			ul_Group = BIG_ul_SearchFileExt(asz_Temp1, BIG_NameFile(mul_CurrentEditModel));
			ERR_X_Assert(ul_Group != BIG_C_InvalidIndex);

			/* Create function file */
			BIG_ul_CreateDir(asz_Temp);
			ul_Func = BIG_ul_CreateFile(asz_Temp, asz_Name);

			/* Add vars in group */
			BIG_AddRefInGroup(ul_Group, BIG_NameFile(ul_Func), BIG_FileKey(ul_Func));

			/* Inform all editors */
			M_MF()->FatHasChanged();

			/* Set created model as current one */
			Load(ul_Func);
		}
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnNewFunction(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EAI_STR_Csz_TitleNewFunction);
	char				asz_Temp[BIG_C_MaxLenPath];
	char				asz_Temp1[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenPath];
	char				*psz_Name, *psz_Temp;
	BIG_INDEX			ul_Group, ul_Func;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!b_CanClose()) return;
_Try_
	if(o_Dialog.DoModal() == IDOK)
	{
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(BIG_b_CheckName(psz_Name) == FALSE) return;

		/* Compute model name */
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Temp);
		L_strcat(asz_Temp, "/");
		L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
		psz_Temp = L_strrchr(asz_Name, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcat(asz_Temp, asz_Name);

		/* Test if file is already here */
		L_strcpy(asz_Name, psz_Name);
		L_strcat(asz_Name, EDI_Csz_ExtAIEditorFct);
		if(BIG_ul_SearchFileExt(asz_Temp, asz_Name) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError(EAI_ERR_Csz_FunctionAlreadyExists, asz_Name);
		}
		else
		{
			L_strcpy(asz_Temp1, asz_Temp);
			*L_strrchr(asz_Temp1, '/') = 0;

			/* Get model file index */
			ul_Group = BIG_ul_SearchFileExt(asz_Temp1, BIG_NameFile(mul_CurrentEditModel));
			ERR_X_Assert(ul_Group != BIG_C_InvalidIndex);

			/* Create function file */
			BIG_ul_CreateDir(asz_Temp);
			ul_Func = BIG_ul_CreateFile(asz_Temp, asz_Name);

			/* Add vars in group */
			BIG_AddRefInGroup(ul_Group, BIG_NameFile(ul_Func), BIG_FileKey(ul_Func));

			/* Inform all editors */
			M_MF()->FatHasChanged();

			/* Set created model as current one */
			Load(ul_Func);
		}
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnNewVars(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EAI_STR_Csz_TitleNewVars);
	char				asz_Temp[BIG_C_MaxLenPath];
	char				asz_Temp1[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenPath];
	char				*psz_Name, *psz_Temp;
	BIG_INDEX			ul_Group, ul_Func;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!b_CanClose()) return;
_Try_
	if(o_Dialog.DoModal() == IDOK)
	{
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(BIG_b_CheckName(psz_Name) == FALSE) return;

		/* Compute model name */
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Temp);
		L_strcat(asz_Temp, "/");
		L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
		psz_Temp = L_strrchr(asz_Name, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcat(asz_Temp, asz_Name);

		/* Compute var file name */
		L_strcpy(asz_Name, psz_Name);
		L_strcat(asz_Name, EDI_Csz_ExtAIEditorVars);

		/* Test if file is already here */
		if(BIG_ul_SearchFileExt(asz_Temp, asz_Name) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError(EDI_ERR_Csz_FileAlreadyHere, asz_Name);
		}
		else
		{
			L_strcpy(asz_Temp1, asz_Temp);
			*L_strrchr(asz_Temp1, '/') = 0;

			/* Get model file index */
			ul_Group = BIG_ul_SearchFileExt(asz_Temp1, BIG_NameFile(mul_CurrentEditModel));
			ERR_X_Assert(ul_Group != BIG_C_InvalidIndex);

			/* Create var file */
			BIG_ul_CreateDir(asz_Temp);
			ul_Func = BIG_ul_CreateFile(asz_Temp, asz_Name);

			/* Add vars in group */
			BIG_AddRefInGroup(ul_Group, BIG_NameFile(ul_Func), BIG_FileKey(ul_Func));

			/* Inform all editors */
			M_MF()->FatHasChanged();

			/* Set created model as current one */
			Load(ul_Func);
		}
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnNewModel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dialog(EAI_STR_Csz_TitleNewModel);
	char				asz_Temp[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenName];
	char				*psz_Name;
	BIG_INDEX			ul_Group, ul_Vars;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	if(o_Dialog.DoModal() == IDOK)
	{
		psz_Name = (char *) (LPCSTR) o_Dialog.mo_Name;
		if(BIG_b_CheckName(psz_Name) == FALSE) return;

		L_strcpy(asz_Temp, EDI_Csz_Path_AIModels);
		L_strcat(asz_Temp, "/");
		L_strcat(asz_Temp, psz_Name);

		if(BIG_ul_SearchDir(asz_Temp) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError(EAI_ERR_Csz_ModelAlreadyExists, psz_Name);
		}
		else
		{
			/* Close current model/instance/file */
			if(!b_CanClose()) return;
			OnClose();

			/* Create dir for model */
			BIG_ul_CreateDir(asz_Temp);

			/* Create group file for the model */
			L_strcpy(asz_Name, psz_Name);
			L_strcat(asz_Name, EDI_Csz_ExtAIEditorModel);
			ul_Group = BIG_ul_CreateFile(EDI_Csz_Path_AIModels, asz_Name);

			/* Create file for variables */
			L_strcpy(asz_Name, psz_Name);
			L_strcat(asz_Name, EDI_Csz_ExtAIEditorVars);
			SAV_Begin(asz_Temp, asz_Name);
			SAV_Buffer(EAI_STR_Csz_VarDef1, L_strlen(EAI_STR_Csz_VarDef1));
			SAV_Buffer(EAI_STR_Csz_VarDef2, L_strlen(EAI_STR_Csz_VarDef2));
			SAV_Buffer(EAI_STR_Csz_VarDef3, L_strlen(EAI_STR_Csz_VarDef3));
			SAV_Buffer(EAI_STR_Csz_VarDef4, L_strlen(EAI_STR_Csz_VarDef4));
			SAV_Buffer(EAI_STR_Csz_VarDef5, L_strlen(EAI_STR_Csz_VarDef5));
			SAV_Buffer(EAI_STR_Csz_VarDef6, L_strlen(EAI_STR_Csz_VarDef6));
			ul_Vars = SAV_ul_End();
			BIG_AddRefInGroup(ul_Group, BIG_NameFile(ul_Vars), BIG_FileKey(ul_Vars));

			/* Set created model as current one */
			SetCurrentModel(ul_Group);

			/* Update menu */
			RefreshMenu();

			/* Inform all editors */
			M_MF()->FatHasChanged();
		}
	}

_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnDeleteFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenPath];
	char		*psz_Temp;
	BIG_INDEX	ul_EngineFile;
	int			i_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Res = M_MF()->MessageBox
		(
			EDI_STR_Csz_DeleteFile,
			BIG_NameFile(mul_CurrentEditFile),
			MB_ICONQUESTION | MB_YESNOCANCEL
		);
	if(i_Res == IDCANCEL) return;

	/* Delete editor file */
	BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditFile), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditFile));
	psz_Temp = L_strrchr(asz_Name, '.');
	ERR_X_Assert(psz_Temp);

	BIG_DelRefFromGroup(mul_CurrentEditModel, BIG_FileKey(mul_CurrentEditFile));
	if(i_Res == IDYES) BIG_DelFile(asz_Path, asz_Name);

	if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFct))
	{
		*psz_Temp = 0;
		L_strcat(asz_Name, EDI_Csz_ExtAIEngineFct);
	}
	else if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFctLib))
	{
		*psz_Temp = 0;
		L_strcat(asz_Name, EDI_Csz_ExtAIEngineFctLib);
	}
	else if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorVars))
	{
		*psz_Temp = 0;
		L_strcat(asz_Name, EDI_Csz_ExtAIEngineVars);
	}
	else
		ERR_X_ForceAssert();

	/* Delete engine file */
	ul_EngineFile = BIG_ul_SearchFileExt(asz_Path, asz_Name);
	if(ul_EngineFile != BIG_C_InvalidIndex)
	{
		BIG_DelRefFromGroup(mul_CurrentEngineModel, BIG_FileKey(ul_EngineFile));
		if(i_Res == IDYES) BIG_DelFile(asz_Path, asz_Name);
	}

	OnClose(TRUE, FALSE);
	M_MF()->FatHasChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnRenameFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenPath];
	char				asz_Name1[BIG_C_MaxLenPath];
	char				*psz_Temp;
	BIG_INDEX			ul_EngineFile;
	EDIA_cl_NameDialog	o_Dialog(EAI_STR_Csz_TitleNewVars);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Current (without extension) */
	L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditFile));
	psz_Temp = L_strrchr(asz_Name, '.');
	ERR_X_Assert(psz_Temp);
	*psz_Temp = 0;
	o_Dialog.mo_Name = asz_Name;

	if(o_Dialog.DoModal() == IDOK)
	{
		if(BIG_b_CheckName((char *) (LPCSTR) o_Dialog.mo_Name) == FALSE) return;

		/* Editor file */
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditFile), asz_Path);
		L_strcpy(asz_Name, (char *) (LPCSTR) o_Dialog.mo_Name);
		L_strcpy(asz_Name1, BIG_NameFile(mul_CurrentEditFile));
		psz_Temp = L_strrchr(asz_Name1, '.');
		ERR_X_Assert(psz_Temp);
		L_strcat(asz_Name, psz_Temp);
		BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(mul_CurrentEditFile));

		/* Engine file */
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFct))
		{
			*psz_Temp = 0;
			L_strcat(asz_Name1, EDI_Csz_ExtAIEngineFct);
			psz_Temp = L_strrchr(asz_Name, '.');
			*psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEngineFct);
		}
		else if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFctLib))
		{
			*psz_Temp = 0;
			L_strcat(asz_Name1, EDI_Csz_ExtAIEngineFctLib);
			psz_Temp = L_strrchr(asz_Name, '.');
			*psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEngineFctLib);
		}
		else if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorVars))
		{
			*psz_Temp = 0;
			L_strcat(asz_Name1, EDI_Csz_ExtAIEngineVars);
			psz_Temp = L_strrchr(asz_Name, '.');
			*psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEngineVars);
		}
		else
			ERR_X_ForceAssert();

		ul_EngineFile = BIG_ul_SearchFileExt(asz_Path, asz_Name1);
		if(ul_EngineFile != BIG_C_InvalidIndex) BIG_RenFile(asz_Name, asz_Path, asz_Name1);

		/* Refresh */
		InitListAI();
		M_MF()->FatHasChanged();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnAddFunction(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File(EAI_STR_Csz_InsertFct, 3, TRUE, TRUE, EDI_Csz_Path_AILib);
	int					i_Num;
	char				*psz_Temp;
	CString				o_Temp;
	BIG_INDEX			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(o_File.DoModal() == IDOK)
	{
		for(i_Num = 1; i_Num <= o_File.mi_NumFiles; i_Num++)
		{
			/* Get dir name */
			o_File.GetItem(o_File.mo_File, i_Num, o_Temp);
			psz_Temp = (char *) (LPCSTR) o_Temp;

			/* Check validity of name */
			if(BIG_b_CheckName(psz_Temp) == FALSE) continue;

			/* Export to main bigfile (so import, that's the same) */
			ul_Index = BIG_ul_SearchDir(o_File.masz_FullPath);
			if(ul_Index == BIG_C_InvalidIndex)
			{
				ERR_X_ForceError(EDI_ERR_Csz_UnknowPath, o_File.masz_FullPath);
				break;
			}

			ul_Index = BIG_ul_SearchFile(ul_Index, psz_Temp);
			if(ul_Index == BIG_C_InvalidIndex)
			{
				ERR_X_ForceError(EDI_ERR_Csz_UnknowFile, psz_Temp);
				break;
			}

			/* Check if it's an ia file */
			if
			(
				(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtAIEditorFct))
			&&	(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtAIEditorVars))
			&&	(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtAIEditorFctLib))
			)
			{
				ERR_X_ForceError(EDI_ERR_Csz_UnknowFile, psz_Temp);
				break;
			}

			/* Add file to model */
			BIG_AddRefInGroup(mul_CurrentEditModel, BIG_NameFile(ul_Index), BIG_FileKey(ul_Index));
		}

		/* Inform all editors */
		M_MF()->FatHasChanged();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnSysBreak(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_CheckListDialog *po_Dialog;
	int						i, num;
	CStringArray			o_Arr;
	CString					str;
	char					*pz;
	char					az[128];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Dialog = new EDIA_cl_CheckListDialog(EAI_STR_Csz_SysBreakTitle, CPoint(0, 0));
	for(i = 0; i < sizeof(EAI_gast_Functions) / sizeof(EAI_gast_Functions[0]); i++)
	{
		str.Format("%s@%d", EAI_gast_Functions[i].psz_Name, i);
		o_Arr.Add(str);
	}

	SortStringArray(o_Arr, FALSE);
	for(i = 0; i < o_Arr.GetCount(); i++)
	{
		str = o_Arr[i];
		L_strcpy(az, (char *) (LPCSTR) o_Arr[i]);
		pz = L_strchr(az, '@');
		*pz = 0;
		num = L_atoi(pz + 1);
		po_Dialog->AddItem(az, EVAV_EVVIT_Bool, &AI_gast_Functions[num].b_SysBreak);
	}

	po_Dialog->DoModeless();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnFind(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString		o_Left, o_Right, o_Word;
	CHARRANGE	cp;
	BOOL		bCreate;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	bCreate = FALSE;
	if(!mpo_FindDlg)
	{
		mpo_FindDlg = new(EDIA_cl_FindAIDialog);
		bCreate = TRUE;
	}

	mpo_FindDlg->mpo_AI = this;

	mpo_Edit->GetSel(cp);
	if(cp.cpMin == cp.cpMax)
	{
		o_Left = mpo_Edit->o_GetWordLeft(cp.cpMin);
		o_Right = mpo_Edit->o_GetWordRight(cp.cpMin);
		o_Word = o_Left + o_Right;
	}
	else
	{
		o_Word = mpo_Edit->GetSelText();
	}

	mpo_FindDlg->mo_Name = o_Word;
	if(bCreate)
	{
		mpo_FindDlg->DoModeless();
		mpo_FindDlg->CenterWindow();
	}
	else
	{
		mpo_FindDlg->UpdateData(FALSE);
		mpo_FindDlg->ShowWindow(SW_SHOW);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnFindNext(void)
{
	mpo_FindDlg->Find(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnMatch(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	CHARRANGE	cp;
	CString		o_Str;
	int			i_Count;
	char		*psz_Temp;
	char		c1, c2;
	int			sens;
	char		cmode;
	/*~~~~~~~~~~~~~~~~~~*/

	mpo_Edit->GetSel(cp);
	mpo_Edit->GetWindowText(o_Str);
	psz_Temp = (char *) (LPCSTR) o_Str;
	psz_Temp += cp.cpMin;
	cmode = 0;
	sens = 2;
	while(cmode <= 1)
	{
		switch(*psz_Temp)
		{
		case '(':
		case ')':
			c1 = '(';
			c2 = ')';
			sens = *psz_Temp == '(' ? 1 : 0;
			cmode = 2;
			break;

		case '{':
		case '}':
			c1 = '{';
			c2 = '}';
			sens = 1;
			sens = *psz_Temp == '{' ? 1 : 0;
			cmode = 2;
			break;

		case '[':
		case ']':
			c1 = '[';
			c2 = ']';
			sens = 1;
			sens = *psz_Temp == '[' ? 1 : 0;
			cmode = 2;
			break;
		default:
			cmode++;
			psz_Temp--;
			break;
		}
	}

	if(sens == 1)
	{
		i_Count = 1;
		while(cp.cpMin < mpo_Edit->GetTextLength())
		{
			cp.cpMin++;
			psz_Temp++;
			if(*psz_Temp == c1) i_Count++;
			if(*psz_Temp == c2) i_Count--;
			if(!i_Count) break;
		}

		if(!i_Count)
			mpo_Edit->SetSel(cp.cpMin, cp.cpMin);
		else
			LINK_PrintStatusMsg("No Match Found");
	}
	else if(sens == 0)
	{
		i_Count = 1;
		while(cp.cpMin >= 0)
		{
			cp.cpMin--;
			psz_Temp--;
			if(*psz_Temp == c1) i_Count--;
			if(*psz_Temp == c2) i_Count++;
			if(!i_Count) break;
		}

		if(!i_Count)
			mpo_Edit->SetSel(cp.cpMin, cp.cpMin);
		else
			LINK_PrintStatusMsg("No Match Found");
	}
}

#endif /* ACTIVE_EDITORS */
