/*$T TEXTframe.cpp GC 1.138 08/02/05 15:00:57 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#define ACTION_GLOBAL
/**/
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
/**/
#include "TEXTframe.h"
#include "TEXTscroll.h"
/**/
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGgroup.h"
/**/
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
/**/
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "STRing/STRstruct.h"
/**/
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
/**/
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "DIAlogs/DIAlang_dlg.h"
#include "DIAlogs/DIAtextfile_dlg.h"
#include "DIAlogs/DIAlist_dlg.h"
/**/
#include "BASe/MEMory/MEM.h"
/**/
#include "INOut/INO.h"
#include "Res/res.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

void				TEXT_EntryInterestingFieldsCopy (TEXT_tdst_Id *pIdDest, TEXT_tdst_Id *pIdSrc, bool includeComments);
unsigned int		TEXT_ui_GetTextLenght(TEXT_tdst_OneText *);
void				TEXT_SetTextID(TEXT_tdst_OneText *, int ID, char *);
int					TEXT_i_GetEntryIndex(TEXT_tdst_OneText *, int key);
void				ETEXT_FilterString(char *, int iSize = -1, int iMaxSize = -1);
void				ETEXT_BuildAutoLabelString(char *, char *, ULONG ul_Key);
void				ETEXT_CompactString(TEXT_tdst_OneText *);
#ifdef JADEFUSION
extern ULONG	TEXT_ul_CallbackLoadTxlEditor(ULONG _ul_PosFile, ULONG key);
extern void		BAS_bdeletegetval(ULONG _ul_Key, BAS_tdst_barray *array, ULONG *val);
#else
extern "C" ULONG	TEXT_ul_CallbackLoadTxlEditor(ULONG _ul_PosFile, ULONG key);
extern "C" void		BAS_bdeletegetval(ULONG _ul_Key, BAS_tdst_barray *array, ULONG *val);
#endif
/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern char			*ETEXT_gasz_ColName[];

INO_tden_LanguageId ETEXT_gaen_StandardLanguage[] =
{
	INO_e_French,
	INO_e_English,
	INO_e_Danish,
	INO_e_Dutch,
	INO_e_Finnish,
	INO_e_German,
	INO_e_Italian,
	INO_e_Spanish,
	INO_e_Swedish,
	INO_e_Polish,
	INO_e_Russian,
	INO_e_Japanese,
	INO_e_Chinese,
	INO_e_Norwegian,
	INO_e_Slovak,
	INO_e_Slovenian,
	INO_e_Czech,
	INO_e_Hungarian,
	INO_e_Taiwanese,
	INO_e_Dummy
};

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(ETEXT_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(ETEXT_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(AFX_IDW_PANE_FIRST, OnBankSelChange)
	ON_LBN_SELCHANGE(AFX_IDW_PANE_FIRST + 16, OnFileChange)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ETEXT_cl_Frame::ETEXT_cl_Frame(void)
{
	mpo_ListBank = NULL;
	mpo_ListFile = NULL;
	mpo_Scroll = NULL;
	mpt_EditText = NULL;
	mb_NeedSync = mb_FileIsModified = FALSE;
	mi_SortCol = COLMAX + 1;
	mb_FindSomething = FALSE;
	mb_ExportNoFormatCode = FALSE;
	mi_SynchroLang = INO_e_Dummy;
	*msz_NewEntry_Name = 0;
	mi_NewEntry_Number = 10;
	L_memset(mapuw_CharTable, 0, sizeof(mapuw_CharTable));
	mb_Unicode = 0;

	L_memset(max_EnumUserDefinition, 0, sizeof(ETEXT_tdst_EnumUserDefinition) * en_UserData);

	mi_ForceSynchroEntry = -1;
	L_memset(&mst_ForceSynchroEntry, 0, sizeof(TEXT_tdst_Id));

	ul_BufferBankIdx = BIG_C_InvalidKey;
	BufferBank.i_Num = 0;
	BufferBank.pst_AllTexts = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ETEXT_cl_Frame::~ETEXT_cl_Frame(void)
{
	if(mpt_EditText)
	{
		mpt_EditText->ul_Inst = 1;
		TEXT_Free(mpt_EditText);
		mpt_EditText = NULL;
	}

	if(mpo_ListBank) delete mpo_ListBank;
	if(mpo_ListFile) delete mpo_ListFile;

	CharTable_FreeAll();

	for(int i = 0; i < en_UserData; i++)
	{
		/*~~~~~~~~~*/
		char	*ptr;
		/*~~~~~~~~~*/

		if(max_EnumUserDefinition[i].st_NameToValue.gran == 0) continue;

		for(int j = 0; j < max_EnumUserDefinition[j].st_NameToValue.num; j++)
		{
			ptr = (char *) max_EnumUserDefinition[i].st_ValueToName.base[j].ul_Val;
			delete[] ptr;
		}

		BAS_bfree(&max_EnumUserDefinition[i].st_NameToValue);
		BAS_bfree(&max_EnumUserDefinition[i].st_ValueToName);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Frame::b_HasUserValue(en_ColContent eType)
{
	if(max_EnumUserDefinition[eType].st_NameToValue.gran == 0) return FALSE;
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Frame::i_GetUserValueNb(en_ColContent eType)
{
	return max_EnumUserDefinition[eType].st_ValueToName.num;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ETEXT_cl_Frame::ul_GetUserValue(en_ColContent eType, int i)
{
	return max_EnumUserDefinition[eType].st_ValueToName.base[i].ul_Key;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *ETEXT_cl_Frame::p_GetUserName(en_ColContent eType, int i)
{
	return (char *) max_EnumUserDefinition[eType].st_ValueToName.base[i].ul_Val;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *ETEXT_cl_Frame::p_GetUserValueToName(en_ColContent eType, ULONG ulVal)
{
	/*~~~~~~~~*/
	ULONG	ptr;
	/*~~~~~~~~*/

	ptr = BAS_bsearch(ulVal, &max_EnumUserDefinition[eType].st_ValueToName);
	if(ptr == -1) return "undefined";
	return (char *) ptr;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ETEXT_cl_Frame::ul_GetUserNameToValue(en_ColContent eType, char *ptr)
{
	/*~~~~~~~~*/
	ULONG	val;
	/*~~~~~~~~*/

	val = BAS_bsearch((ULONG) ptr, &max_EnumUserDefinition[eType].st_NameToValue);
	return val;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::SetFileModified(BOOL b_Modif, ULONG ulFat)
{
	mb_FileIsModified = b_Modif;
	if(!mb_FileIsModified) mb_NeedSync = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	CCreateContext	o_Context;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1) return -1;

	mo_Splitter0.CreateStatic(this, 1, 2);
	mo_Splitter2.CreateStatic(&mo_Splitter0, 2, 1, WS_CHILD | WS_VISIBLE, mo_Splitter0.IdFromRowCol(0, 0));

	/* bank list */
	mpo_ListBank = new CListBox;
	mpo_ListBank->Create
		(
			WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
			CRect(0, 0, 0, 0),
			&mo_Splitter2,
			mo_Splitter2.IdFromRowCol(0, 0)
		);
	mpo_ListBank->SetFont(&M_MF()->mo_Fnt);
	mpo_ListBank->EnableWindow(FALSE);

	/* bank contains : file list */
	mpo_ListFile = new CListBox;
	mpo_ListFile->Create
		(
			WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
			CRect(0, 0, 0, 0),
			&mo_Splitter2,
			mo_Splitter2.IdFromRowCol(1, 0)
		);
	mpo_ListFile->SetFont(&M_MF()->mo_Fnt);
	mpo_ListFile->EnableWindow(FALSE);

	/* scroll view */
	mo_Splitter0.CreateView(0, 1, RUNTIME_CLASS(ETEXT_cl_Scroll), CSize(0, 0), NULL);
	mpo_Scroll = (ETEXT_cl_Scroll *) mo_Splitter0.GetPane(0, 1);
	mpo_Scroll->SetScrollSizes(MM_TEXT, CSize(1, 1));
	mpo_Scroll->mpo_Parent = this;

	/* force recalc */
	RecalcLayout();

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnBankNew(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Name("Enter the text bank name");
	char				az_Path[BIG_C_MaxLenPath];
	char				az_Name[BIG_C_MaxLenName];
	BIG_INDEX			h_Dir;
	char				*pz_Name;
	int					res, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((o_Name.DoModal() == IDOK) && o_Name.mo_Name.GetLength())
	{
		pz_Name = (char *) (LPCSTR) o_Name.mo_Name;

		/* check */
		L_strcpy(az_Path, EDI_Csz_Path_Texts);
		L_strcat(az_Path, "/");
		L_strcat(az_Path, pz_Name);
		if(BIG_ul_SearchDir(az_Path) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("This text bank already exists !", NULL);
			return;
		}

		/* makedir */
		h_Dir = BIG_ul_CreateDir(az_Path);
		if(h_Dir == BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("Can't create text bank directory", NULL);
			return;
		}

		/* makefile txg */
		L_strcpy(az_Name, pz_Name);
		L_strcat(az_Name, EDI_Csz_ExtTextLang);

		pz_Name = L_strrchr(az_Path, '/');
		if(pz_Name) *pz_Name = 0;

		h_Dir = BIG_ul_CreateFile(az_Path, az_Name);

		/* add txg file in the bank list */
		res = mpo_ListBank->FindString(-1, az_Name);
		if(res == LB_ERR)
		{
			res = mpo_ListBank->AddString(az_Name);
			if(res != LB_ERR)
			{
				mpo_ListBank->SetItemData(res, h_Dir);
				mpo_ListBank->SetCurSel(res);
			}
		}

		/* create standard language contents */
		if(mst_Ini.i_OptionFlags & ETEXT_Cte_UseStandardLang)
		{
			/*~~~~~~~~~~~~~~*/
			ULONG	ulBankFat;
			/*~~~~~~~~~~~~~~*/

			ulBankFat = h_Dir;

			L_strcat(az_Path, "/");
			L_strcat(az_Path, o_Name.mo_Name);
			for(i = 0; ETEXT_gaen_StandardLanguage[i] != INO_e_Dummy; i++)
			{
				/*~~~~~~*/
				int iLang;
				/*~~~~~~*/

				iLang = ETEXT_gaen_StandardLanguage[i];

				/* filename */
				sprintf
				(
					az_Name,
					"%s_%s%s",
					INO_dst_LanguageName[iLang].az_ShortName,
					o_Name.mo_Name,
					EDI_Csz_ExtTextFile
				);
				if(BIG_ul_SearchFileExt(az_Path, az_Name) != BIG_C_InvalidIndex)
				{
					ERR_X_Warning(0, "This text file already exists !", NULL);
					continue;
				}

				/* create txl file */
				h_Dir = BIG_ul_CreateFile(az_Path, az_Name);

				/* add file in the bank at the right rank */
				BIG_AddRefInGroupAtRank(ulBankFat, BIG_NameFile(h_Dir), BIG_FileKey(h_Dir), iLang);
			}

			h_Dir = ulBankFat;
		}

		/* force open */
		OnBankOpen(h_Dir);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnBankNoLangNew(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Name("Enter the text bank name");
	char				az_Path[BIG_C_MaxLenPath];
	char				az_Name[BIG_C_MaxLenName];
	BIG_INDEX			h_Dir;
	char				*pz_Name;
	int					res, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((o_Name.DoModal() == IDOK) && o_Name.mo_Name.GetLength())
	{
		/*~~~~~~~~~~~~~~*/
		ULONG	ulBankFat;
		/*~~~~~~~~~~~~~~*/

		pz_Name = (char *) (LPCSTR) o_Name.mo_Name;

		/* check */
		L_strcpy(az_Path, EDI_Csz_Path_Texts);
		L_strcat(az_Path, "/");
		L_strcat(az_Path, pz_Name);
		if(BIG_ul_SearchDir(az_Path) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("This text bank already exists !", NULL);
			return;
		}

		/* makedir */
		h_Dir = BIG_ul_CreateDir(az_Path);
		if(h_Dir == BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("Can't create text bank directory", NULL);
			return;
		}

		/* makefile txg */
		L_strcpy(az_Name, pz_Name);
		L_strcat(az_Name, EDI_Csz_ExtTextLang);

		pz_Name = L_strrchr(az_Path, '/');
		if(pz_Name) *pz_Name = 0;

		h_Dir = BIG_ul_CreateFile(az_Path, az_Name);

		/* add txg file in the bank list */
		res = mpo_ListBank->FindString(-1, az_Name);
		if(res == LB_ERR)
		{
			res = mpo_ListBank->AddString(az_Name);
			if(res != LB_ERR)
			{
				mpo_ListBank->SetItemData(res, h_Dir);
				mpo_ListBank->SetCurSel(res);
			}
		}

		/* create standard language contents */
		ulBankFat = h_Dir;

		L_strcat(az_Path, "/");
		L_strcat(az_Path, o_Name.mo_Name);

		/* create the independant lang file (xx_....txl) */
		sprintf(az_Name, "xx_%s%s", o_Name.mo_Name, EDI_Csz_ExtTextFile);
		if(BIG_ul_SearchFileExt(az_Path, az_Name) != BIG_C_InvalidIndex)
		{
			ERR_X_Warning(0, "This text file already exists !", NULL);
		}

		/* create txl file */
		h_Dir = BIG_ul_CreateFile(az_Path, az_Name);
		for(i = 0; i < INO_e_MaxLangNb; i++)
		{
			/* add file in the bank at the right rank */
			BIG_AddRefInGroupAtRank(ulBankFat, BIG_NameFile(h_Dir), BIG_FileKey(h_Dir), i);
		}

		h_Dir = ulBankFat;

		/* force open */
		OnBankOpen(h_Dir);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ETEXT_cl_Frame::ul_GetCurBank(int *piIdx)
{
	/*~~~~~*/
	int item;
	/*~~~~~*/

	item = mpo_ListBank->GetCurSel();
	if(piIdx) *piIdx = item;
	if(item == LB_ERR) return BIG_C_InvalidIndex;
	return(ULONG) mpo_ListBank->GetItemData(item);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ETEXT_cl_Frame::ul_GetCurFile(int *piIdx)
{
	/*~~~~~*/
	int item;
	/*~~~~~*/

	item = mpo_ListFile->GetCurSel();
	if(piIdx) *piIdx = item;
	if(item == LB_ERR) return BIG_C_InvalidIndex;
	return(ULONG) mpo_ListFile->GetItemData(item);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnBankSelChange(void)
{
	OnBankChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnBankChange(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ulFat;
	ULONG				ulSize, i;
	BIG_tdst_GroupElem	*pGroup;
	int					item;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OnFileClose();

	mpo_ListFile->SetRedraw(FALSE);
	mpo_ListFile->ResetContent();

	ulFat = ul_GetCurBank();
	if(ulFat != BIG_C_InvalidIndex)
	{
		mpo_ListFile->EnableWindow(TRUE);
		mpo_ListBank->EnableWindow(TRUE);

		pGroup = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(ulFat), (ULONG *) &ulSize);
		for(i = 0; i < ulSize / sizeof(BIG_tdst_GroupElem); i++, pGroup++)
		{
			if((pGroup->ul_Key == BIG_C_InvalidKey) || (pGroup->ul_Key == 0))
			{
				item = mpo_ListFile->AddString("<hole>");
				mpo_ListFile->SetItemData(item, BIG_C_InvalidIndex);
				continue;
			}

			ulFat = BIG_ul_SearchKeyToFat(pGroup->ul_Key);

			if(ulFat == BIG_C_InvalidIndex)
			{
				item = mpo_ListFile->AddString("<unknown key>");
				mpo_ListFile->SetItemData(item, BIG_C_InvalidIndex);
			}
			else
			{
				item = mpo_ListFile->AddString(BIG_NameFile(ulFat));
				mpo_ListFile->SetItemData(item, ulFat);
			}
		}

		mpo_ListFile->SetCurSel(TEXT_gst_Global.i_CurrentLanguage);
	}
	else
	{
		mpo_ListFile->EnableWindow(FALSE);
		mpo_ListBank->EnableWindow(FALSE);
	}

	mpo_ListFile->SetRedraw(TRUE);
	OnFileChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFileClose(void)
{
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	mb_FindSomething = FALSE;
	if(mpt_EditText)
	{
		if(mb_FileIsModified) OnFileSave();
		mpt_EditText->ul_Inst = 1;
		TEXT_Free(mpt_EditText);
		mpt_EditText = NULL;
	}

	if(mpo_Scroll) mpo_Scroll->TextChange();
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFileChange(void)
{
	/*~~~~~~~~~~~~~~*/
	BIG_INDEX	ulFat;
	/*~~~~~~~~~~~~~~*/

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	mb_FindSomething = FALSE;
	ulFat = ul_GetCurFile();
	
	FileChange(ulFat);

	if(mpo_Scroll) mpo_Scroll->TextChange();
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void ETEXT_cl_Frame::FileChange(BIG_INDEX ulFat) {
	if(ulFat == BIG_C_InvalidIndex)
	{
		if(mpt_EditText)
		{
			if(mb_FileIsModified) OnFileSave();
			mpt_EditText->ul_Inst = 1;
			TEXT_Free(mpt_EditText);
		}

		mpt_EditText = NULL;
	}
	else
	{
		if(mpt_EditText)
		{
			if(mb_FileIsModified) OnFileSave();
			mpt_EditText->ul_Inst = 1;
			TEXT_Free(mpt_EditText);
			mpt_EditText = NULL;
		}

		if(!mpt_EditText)
		{
			*(ULONG *) &mpt_EditText = TEXT_ul_CallbackLoadTxlEditor(BIG_PosFile(ulFat), BIG_FileKey(ulFat));
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFileSave(void)
{
	/* if(!mb_FileIsModified) return; */
	if(!mpt_EditText) return;

	SaveFile(mpt_EditText);
	SetFileModified(FALSE);

	if(mst_Ini.i_OptionFlags & ETEXT_Cte_SyncAuto)
	{
		mb_NeedSync = FALSE;
		OnSynchronize();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::SaveFile(TEXT_tdst_OneText *pText)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			asz_Path[BIG_C_MaxLenPath];
	ULONG			ulFat, ulSize, i, ulComment;
	TEXT_tdst_Id	*p;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pText) return;

	CompressComment(pText);

	ulFat = BIG_ul_SearchKeyToFat(pText->ul_Key);
	if(ulFat == BIG_C_InvalidIndex) return;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	BIG_ComputeFullName(BIG_ParentFile(ulFat), asz_Path);
	SAV_Begin(asz_Path, BIG_NameFile(ulFat));
	if(pText->ul_Num) SAV_Buffer(&pText->ul_Num, sizeof(ULONG));

	p = pText->pst_Ids;
	ulSize = 0;

	for(i = 0; i < pText->ul_Num; i++, p++)
	{
		/*~~~~~~~~~~~*/
		int i_Priority;
		/*~~~~~~~~~~~*/

		SAV_Buffer(&p->ul_IdKey, sizeof(ULONG));
		SAV_Buffer(&p->ul_SoundKey, sizeof(ULONG));
		SAV_Buffer(&p->ul_ObjKey, sizeof(ULONG));
		SAV_Buffer(&p->i_Offset, sizeof(int));

		p->us_Version = TEXT_Cte_CurrVersion;
		i_Priority = (p->us_Priority & 0x0000FFFF) | ((p->us_Version << 16) & 0xFFFF0000);
		SAV_Buffer(&i_Priority, sizeof(int));

		SAV_Buffer(&p->c_FacialIdx, sizeof(char));
		SAV_Buffer(&p->c_LipsIdx, sizeof(char));
		SAV_Buffer(&p->c_AnimIdx, sizeof(char));
		SAV_Buffer(&p->c_DumIdx, sizeof(char));

		/* ulComment = L_strlen(p->asz_Comment) + 1; */
		ulComment = p->i_CommentOffset[31] + L_strlen(p->asz_Comment + p->i_CommentOffset[31]) + 1;
		SAV_Buffer(&ulComment, sizeof(ULONG));
		SAV_Buffer(p->asz_Name, TEXT_MaxLenId);

		SAV_Buffer(p->asz_Comment, ulComment);

		if(p->i_Offset != -1) ulSize += L_strlen(pText->psz_Text + p->i_Offset) + 1;
	}

	if(pText->ul_Num) SAV_Buffer(pText->psz_Text, ulSize);
	SAV_ul_End();
	LINK_FatHasChanged();

	sprintf(asz_Path, "File %s saved.", BIG_NameFile(ulFat));
	LINK_PrintStatusMsg(asz_Path);
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFileNew(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_TextFile	o_Name;
	char				az_Path[BIG_C_MaxLenPath];
	char				az_Name[BIG_C_MaxLenName];
	BIG_INDEX			h_Dir;
	char				*pz_Name;
	int					res;
	BIG_INDEX			ulBankFat;
	int					iLang;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ulBankFat = ul_GetCurBank();
	if(ulBankFat == BIG_C_InvalidIndex)
	{
		ERR_X_ForceError("No text bank is selected !", NULL);
		return;
	}

	if(o_Name.DoModal() == IDOK)
	{
		iLang = o_Name.mi_Country;

		/* dir path */
		L_strcpy(az_Path, EDI_Csz_Path_Texts);
		L_strcat(az_Path, "/");
		L_strcat(az_Path, BIG_NameFile(ulBankFat));
		pz_Name = L_strrchr(az_Path, '.');
		if(pz_Name) *pz_Name = 0;

		/* filename */
		sprintf(az_Name, "%s_%s%s", INO_dst_LanguageName[iLang].az_ShortName, o_Name.mo_FileName, EDI_Csz_ExtTextFile);
		if(BIG_ul_SearchFileExt(az_Path, az_Name) != BIG_C_InvalidIndex)
		{
			ERR_X_ForceError("This text file already exists !", NULL);
			return;
		}

		/* create file */
		h_Dir = BIG_ul_CreateFile(az_Path, az_Name);

		/* add file in the bank at the right rank */
		BIG_AddRefInGroupAtRank(ulBankFat, BIG_NameFile(h_Dir), BIG_FileKey(h_Dir), iLang);

		/* force reloding the bank */
		res = mpo_ListBank->FindString(-1, BIG_NameFile(ulBankFat));
		if(res != LB_ERR)
		{
			mpo_ListBank->SetCurSel(res);
			OnBankChange();
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFileDelete(void)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	ulBankFat;
	ULONG	ulFat;
	int		iLang;
	/*~~~~~~~~~~~~~~*/

	ulBankFat = ul_GetCurBank();
	ulFat = ul_GetCurFile();

	if(ulBankFat == BIG_C_InvalidIndex) return;
	if(ulFat == BIG_C_InvalidIndex) return;

	SetFileModified(FALSE);
	iLang = mpo_ListFile->FindString(-1, BIG_NameFile(ulFat));
	if(iLang == LB_ERR) return;
	BIG_AddRefInGroupAtRank(ulBankFat, BIG_NameFile(ulFat), 0, iLang);
	OnBankChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnEntryNew(BOOL _b_Multi, BOOL b_hole)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Name("Enter the entry name");
	CString				o(" ");
	TEXT_tdst_Id		*pId;
	char				c_Save, *sz_Name;
	unsigned char		*sz_Cur;
	char				sz_EntryName[TEXT_MaxLenId], sz_RootName[TEXT_MaxLenId], sz_Format[16];
	int					i, i_Digit, i_Num, i_Nb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpt_EditText) return;

	if(b_hole)
	{
		o_Name.mo_Title = CString("Enter number of entry you want");
		o_Name.mo_Name.Format("%d", mi_NewEntry_Number);
		while(1)
		{
			if(o_Name.DoModal() != IDOK) return;
			i_Nb = atoi((LPCTSTR) o_Name.mo_Name);
			if((i_Nb <= 0) || (i_Nb > 1000))
			{
				if
				(
					MessageBox
						(
							"Enter a number between 1 and 1000\nTry again ?",
							"Error",
							MB_YESNO | MB_ICONEXCLAMATION
						) != IDYES
				) return;
			}
			else
				break;
		}

		mi_NewEntry_Number = i_Nb;
	}
	else
	{
		o_Name.mo_Name = CString(msz_NewEntry_Name);
		while(1)
		{
			if(o_Name.DoModal() != IDOK) return;
			sz_Name = (char *) (LPCTSTR) o_Name.mo_Name;
			if(L_strlen(sz_Name) > TEXT_MaxLenId - 1)
			{
				sprintf(sz_RootName, "Name is too long (max %d character)\nTry another name ?", TEXT_MaxLenId - 1);
				if(MessageBox(sz_RootName, "Error", MB_YESNO | MB_ICONEXCLAMATION) != IDYES) return;
			}
			else
				break;
		}

		L_strcpy(sz_RootName, sz_Name);
		sz_Cur = (unsigned char *) (sz_RootName + strlen(sz_RootName) - 1);
		while(isdigit(*sz_Cur) && (sz_Cur > (unsigned char *) sz_RootName)) sz_Cur--;
		sz_Name = (char *)++sz_Cur;
		i_Digit = i_Num = 0;
		while(*sz_Cur)
		{
			i_Digit++;
			i_Num = (i_Num * 10) + (*sz_Cur++ -'0');
		}

		sprintf(sz_Format, "%%s%%0%dd", i_Digit);
		c_Save = *sz_Name;
		*sz_Name = 0;

		if(_b_Multi)
		{
			o_Name.mo_Title = CString("Enter number of entry you want");
			o_Name.mo_Name.Format("%d", mi_NewEntry_Number);
			while(1)
			{
				if(o_Name.DoModal() != IDOK) return;
				i_Nb = atoi((LPCTSTR) o_Name.mo_Name);
				if((i_Nb <= 0) || (i_Nb > 1000))
				{
					if
					(
						MessageBox
							(
								"Enter a number between 1 and 1000\nTry again ?",
								"Error",
								MB_YESNO | MB_ICONEXCLAMATION
							) != IDYES
					) return;
				}
				else
					break;
			}

			mi_NewEntry_Number = i_Nb;

			sprintf(msz_NewEntry_Name, sz_Format, sz_RootName, i_Num + i_Nb);
		}
		else
		{
			sprintf(msz_NewEntry_Name, sz_Format, sz_RootName, i_Num + 1);
			*sz_Name = c_Save;
			sprintf(sz_Format, "%%s");
			i_Nb = 1;
		}
	}

	for(i = 0; i < i_Nb; i++)
	{
		if(b_hole)
			strcpy(sz_EntryName, "<hole>");
		else
			sprintf(sz_EntryName, sz_Format, sz_RootName, (i_Num + i));

		mpt_EditText->ul_Num++;
		if(mpt_EditText->pst_Ids)
		{
			mpt_EditText->pst_Ids = (TEXT_tdst_Id *) MEM_p_Realloc
				(
					mpt_EditText->pst_Ids,
					mpt_EditText->ul_Num * sizeof(TEXT_tdst_Id)
				);
			L_memset(&mpt_EditText->pst_Ids[mpt_EditText->ul_Num - 1], 0, sizeof(TEXT_tdst_Id));
		}
		else
		{
			mpt_EditText->pst_Ids = (TEXT_tdst_Id *) MEM_p_Alloc(sizeof(TEXT_tdst_Id));
			L_memset(mpt_EditText->pst_Ids, 0, sizeof(TEXT_tdst_Id));
		}

		pId = &mpt_EditText->pst_Ids[mpt_EditText->ul_Num - 1];
		pId->i_Offset = -1;
		pId->us_Priority = TEXT_e_PrioNormal;
		pId->us_Version = TEXT_Cte_CurrVersion;
		pId->c_AnimIdx = 0;
		pId->c_FacialIdx = TEXT_e_ExpNormal;
		pId->c_DumIdx = 0;
		pId->c_LipsIdx = TEXT_e_LipsNormal;
		pId->ul_ObjKey = BIG_C_InvalidKey;
		pId->pv_Obj = NULL;
		pId->ul_SoundKey = BIG_C_InvalidKey;
		pId->ul_IdKey = BIG_ul_GetNewKey(BIG_C_InvalidKey);
		pId->asz_Comment = (char *) MEM_p_Alloc(32);
		L_memset(pId->asz_Comment, 0, 32);
		for(int jj = 0; jj < 32; jj++) pId->i_CommentOffset[jj] = jj;

		L_strcpy(pId->asz_Name, sz_EntryName);
		pId->asz_Name[TEXT_MaxLenId - 1] = 0;

		/* filter the entry name */
		ETEXT_FilterString(pId->asz_Name, TEXT_MaxLenId, 23);
		ETEXT_BuildAutoLabelString(pId->asz_LabelAuto, pId->asz_Name, pId->ul_IdKey);

		if(mpt_EditText->psz_Text == NULL)
		{
			mpt_EditText->psz_Text = (char *) MEM_p_Alloc(1);
			*mpt_EditText->psz_Text = 0;
		}

		SetFileModified(TRUE);
		mb_NeedSync = TRUE;
	}

	if(mpo_Scroll) mpo_Scroll->TextChange();

	/* OnFileChange(); */
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_BuildAutoLabelString(char *asz_Label, char *asz_Name, ULONG ul_Key)
{
	/*~~~~~~~~*/
	char	*pz;
	/*~~~~~~~~*/

	L_memset(asz_Label, 0, TEXT_MaxLenId);
	L_memcpy(asz_Label, asz_Name, 10);
	L_strcat(asz_Label, "__________");
	pz = asz_Label + 10;
	sprintf(pz, "_%08X", ul_Key);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_FilterString(char *pz, int iSize, int iMaxSize)
{
	/*~~*/
	int i;
	/*~~*/

	if((iMaxSize != -1) && ((int) L_strlen(pz) > iMaxSize))
	{
		M_MF()->MessageBox("Take care off the text lenght !", "JACQUOT WARNING", MB_OK | MB_ICONSTOP);
	}

	if(iSize == -1) iSize = L_strlen(pz);

	for(i = 0; i < iSize; i++)
	{
		switch(*pz)
		{
		case '<':
		case '>':
			break;

		case 0:
			i = iSize;
			break;

		case 'à':
		case 'â':
		case 'ä':
			*pz = 'a';
			break;

		case 'é':
		case 'è':
		case 'ê':
		case 'ë':
			*pz = 'e';
			break;

		case 'ì':
		case 'î':
		case 'ï':
			*pz = 'i';
			break;

		case 'ô':
		case 'ö':
		case 'ò':
			*pz = 'o';
			break;

		case 'ù':
		case 'û':
		case 'ü':
			*pz = 'u';
			break;

		case ' ':
		case '-':
		case '_':
			*pz = '_';
			break;

		default:
			if(!isalnum(*(unsigned char *) pz)) *pz = '_';
			break;
		}

		pz++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_CompactString(TEXT_tdst_OneText *_pt_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int i, i_CurOffset, i_Len;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	i_CurOffset = 0;
	for(i = 0; i < (int) _pt_Text->ul_Num; i++)
	{
		if(_pt_Text->pst_Ids[i].i_Offset == -1) continue;

		i_Len = strlen(_pt_Text->psz_Text + _pt_Text->pst_Ids[i].i_Offset) + 1;
		if(i_CurOffset < _pt_Text->pst_Ids[i].i_Offset)
		{
			L_memmove(_pt_Text->psz_Text + i_CurOffset, _pt_Text->psz_Text + _pt_Text->pst_Ids[i].i_Offset, i_Len);
			_pt_Text->pst_Ids[i].i_Offset = i_CurOffset;
		}

		i_CurOffset += i_Len;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnEntryDelete(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	int				idx, i;
	TEXT_tdst_Id	*pId;
	/*~~~~~~~~~~~~~~~~~~~*/

	if(!mpt_EditText) return;

	for(idx = 0; idx < (int) mpt_EditText->ul_Num;)
	{
		/* sel = delete */
		pId = &mpt_EditText->pst_Ids[idx];
		if(!pId->sel)
		{
			idx++;
			continue;
		}

		/* delete comment */
		if(pId->asz_Comment) MEM_Free(pId->asz_Comment);

		/* decal allother entries */
		mpt_EditText->ul_Num--;
		for(i = idx; i < (int) mpt_EditText->ul_Num; i++, pId++) L_memcpy(pId, pId + 1, sizeof(TEXT_tdst_Id));
	}

	if(!mpt_EditText->ul_Num)
	{
		MEM_Free(mpt_EditText->pst_Ids);
		mpt_EditText->pst_Ids = NULL;
	}

	ETEXT_CompactString(mpt_EditText);
	SetFileModified(TRUE);
	mb_NeedSync = TRUE;

	if(mpo_Scroll) mpo_Scroll->TextChange();

	/* OnFileChange(); */
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
// Copy "interesting fields" (for copy and paste) from one TEXT_tdst_Id to the other.
// pIdDest->asz_Comment will be freed if necessary
void TEXT_EntryInterestingFieldsCopy (TEXT_tdst_Id *pIdDest, TEXT_tdst_Id *pIdSrc, bool includeComments) {
	pIdDest->us_Priority = pIdSrc->us_Priority;
	pIdDest->us_Version = pIdSrc->us_Version;
	pIdDest->c_AnimIdx = pIdSrc->c_AnimIdx;
	pIdDest->c_FacialIdx = pIdSrc->c_FacialIdx;
	pIdDest->c_DumIdx = pIdSrc->c_DumIdx;
	pIdDest->c_LipsIdx = pIdSrc->c_LipsIdx;
	pIdDest->ul_ObjKey = pIdSrc->ul_ObjKey;
	pIdDest->pv_Obj = pIdSrc->pv_Obj;
	pIdDest->ul_SoundKey = pIdSrc->ul_SoundKey;
	//if (pIdDest->asz_Comment) {
	//	MEM_Free(pIdDest->asz_Comment);
	//	pIdDest->asz_Comment = NULL;
	//}

	if (includeComments && pIdSrc->asz_Comment) {
		if (pIdDest->asz_Comment) MEM_Free(pIdDest->asz_Comment);
		pIdDest->asz_Comment = (char *) MEM_p_Alloc(L_strlen(pIdSrc->asz_Comment)+1);
		L_memcpy(pIdDest->asz_Comment, pIdSrc->asz_Comment, L_strlen(pIdSrc->asz_Comment)+1);
		L_memcpy(pIdDest->i_CommentOffset, pIdSrc->i_CommentOffset, 100*sizeof(int));
	}

	/* else {
		pIdDest->asz_Comment = (char *) MEM_p_Alloc(32);
		L_memset(pIdDest->asz_Comment, 0, 32);
		for(int jj = 0; jj < 32; jj++) pIdDest->i_CommentOffset[jj] = jj;
		L_memset(pIdDest->i_CommentOffset + 32, 0, (100-32)*sizeof(int));
	}*/
	L_strcpy(pIdDest->asz_Name, pIdSrc->asz_Name);
	pIdDest->asz_Name[L_strlen(pIdSrc->asz_Name)] = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
// Copy the comments
// Don't change anything if pIdSrc doesn't have comments.
// If it has, pIdSrc's comments are freed.
void TEXT_CopyComment (TEXT_tdst_Id *pIdDest, TEXT_tdst_Id *pIdSrc) {
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int	commentSize, maxOffset, maxOffsetIdx;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!pIdSrc->asz_Comment) return;
	if (pIdDest->asz_Comment) MEM_Free(pIdDest->asz_Comment);
	// Size of the comment that will be copied
	maxOffset = pIdSrc->i_CommentOffset[0];
	maxOffsetIdx = 0;
	for (int i=0; i<100; i++) {
		if (pIdSrc->i_CommentOffset[i] > maxOffset) {
			maxOffset = pIdSrc->i_CommentOffset[i];
			maxOffsetIdx = i;
		}
	}
	commentSize = maxOffset + L_strlen(pIdSrc->asz_Comment + maxOffset) + 1;
	pIdDest->asz_Comment = (char *) MEM_p_Alloc(commentSize);
	L_memcpy(pIdDest->asz_Comment, pIdSrc->asz_Comment, commentSize);
	L_memcpy(pIdDest->i_CommentOffset, pIdSrc->i_CommentOffset, 100*sizeof(int));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnEntryCopy(void) {
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				i, ulSize;
	BIG_tdst_GroupElem	*pGroup, *pGroupTemp;
	BIG_INDEX			InitFileIdx, CurFileIdx;
	TEXT_tdst_OneText	*CurText;
	TEXT_tdst_Id		*pIdSrc, *pIdDest;
	int					idx, NbSel, offset, TextSize;
	BOOL				*SelTab; // SelTab[i] <=> ith entry of the mpt_EditText is selected
	char				*CurStr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (!mpt_EditText) return;
	// Find number of entries to copy
	NbSel = 0;
	SelTab = (BOOL *) MEM_p_Alloc (mpt_EditText->ul_Num * sizeof(BOOL));
	for (int j=0; j<mpt_EditText->ul_Num; j++) {
		if (mpt_EditText->pst_Ids[j].sel) {
			NbSel++;
			SelTab[j] = TRUE;
		} else {
			SelTab[j] = FALSE;
		}
	}
	if (!NbSel) return;
	// Free old buffer
	if (BufferBank.pst_AllTexts) {
		for (int k=0; k<BufferBank.i_Num; k++) {
			if (!BufferBank.pst_AllTexts[k]) continue;
			CurText = BufferBank.pst_AllTexts[k];
			for (int j=0; j<CurText->ul_Num; j++) {
				if (CurText->pst_Ids[j].asz_Comment)
					MEM_Free (CurText->pst_Ids[j].asz_Comment);
			}
			MEM_Free (CurText->pst_Ids);
			MEM_Free (CurText->psz_Text);
		}
		MEM_Free (BufferBank.pst_AllTexts);
		// BufferBank.i_Num = 0;
	}
	// Allocate new one
	ul_BufferBankIdx = ul_GetCurBank();
	pGroupTemp = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(ul_GetCurBank()), (ULONG *) &ulSize);
	// We must copy the content of pGroupTemp or it will be erased by FileChange(...) later
	pGroup = (BIG_tdst_GroupElem *) MEM_p_Alloc(ulSize);
	L_memcpy(pGroup, pGroupTemp, ulSize);
	// Now we can free pGroupTemp
	BIG_FreeGlobalBuffer();
	BufferBank.i_Num = ulSize / sizeof(BIG_tdst_GroupElem);
	BufferBank.pst_AllTexts = (TEXT_tdst_OneText **) MEM_p_Alloc(
	                                             BufferBank.i_Num * sizeof(TEXT_tdst_OneText *));
	L_memset(BufferBank.pst_AllTexts, 0, BufferBank.i_Num * sizeof(TEXT_tdst_OneText *));
	
	// Sauvegarde
	InitFileIdx = ul_GetCurFile();
	// For each TEXT_tdst_OneText...
	for(i = 0; i < BufferBank.i_Num; i++, pGroup++) {
		if((pGroup->ul_Key == BIG_C_InvalidKey) || (pGroup->ul_Key == 0)) {
			// Fichier vide
			continue;
		}
		CurFileIdx = BIG_ul_SearchKeyToFat(pGroup->ul_Key);
		if(CurFileIdx == BIG_C_InvalidIndex) {
			// Unknown key
			continue;
		}
		FileChange(CurFileIdx);

		// Copy the entries
		BufferBank.pst_AllTexts[i] =
			(TEXT_tdst_OneText *) MEM_p_Alloc(sizeof(TEXT_tdst_OneText));
		CurText = BufferBank.pst_AllTexts[i];
		CurText->ul_Num = NbSel;
		CurText->pst_Ids = (TEXT_tdst_Id *) MEM_p_Alloc(NbSel * sizeof(TEXT_tdst_Id));
		L_memset(CurText->pst_Ids, 0, NbSel * sizeof(TEXT_tdst_Id));
		idx = -1;
		offset = 0;
		TextSize = 0;
		for (int k=0; k<mpt_EditText->ul_Num; k++) {
			if (!SelTab[k]) continue;
			idx++;
			pIdDest = &CurText->pst_Ids[idx];
			pIdSrc  = &mpt_EditText->pst_Ids[k];
			TEXT_EntryInterestingFieldsCopy (pIdDest, pIdSrc, false);
			TEXT_CopyComment(pIdDest, pIdSrc);
			// Copy the text
			if (mpt_EditText->pst_Ids[k].i_Offset == -1) {
				CurText->pst_Ids[idx].i_Offset = -1;
				continue;
			}
			CurStr = mpt_EditText->psz_Text + mpt_EditText->pst_Ids[k].i_Offset;
			CurText->pst_Ids[idx].i_Offset = TextSize;
			TextSize += L_strlen(CurStr) + 1;
			if (CurText->psz_Text)
				CurText->psz_Text = (char *) MEM_p_Realloc(CurText->psz_Text, TextSize);
			else
				CurText->psz_Text = (char *) MEM_p_Alloc(TextSize);
			L_memcpy(CurText->psz_Text + CurText->pst_Ids[idx].i_Offset, CurStr, L_strlen(CurStr) + 1);
			offset += L_strlen(mpt_EditText->psz_Text + offset) + 1;
		}
	}
	// Restauration
	if (i) FileChange(InitFileIdx);
	for (i=0; i<mpt_EditText->ul_Num; i++) {
		mpt_EditText->pst_Ids[i].sel = SelTab[i];
	}
	if(mpo_Scroll) mpo_Scroll->TextChange();
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnEntryCut(void) {
	OnEntryCopy();
	OnEntryDelete();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnEntryPaste(void) {
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				i, ulSize, ul_NbNewEntries;
	BIG_tdst_GroupElem	*pGroup, *pGroupTemp;
	int					j, idx, curOffset;
	BIG_INDEX			InitFileIdx, CurFileIdx;
	TEXT_tdst_OneText	*BufferText;
	TEXT_tdst_Id		*pIdDest, *pIdSrc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(ul_BufferBankIdx == BIG_C_InvalidIndex) return;
	if(!mpt_EditText) return; // <hole>

	// Compute the index of the first pasted entry
	for (idx=0; idx<mpt_EditText->ul_Num && !mpt_EditText->pst_Ids[idx].sel; idx++) {}
	if (idx != mpt_EditText->ul_Num) idx++;
	ul_NbNewEntries = BufferBank.pst_AllTexts[0]->ul_Num;

	j = mpt_EditText->ul_Num - 1;
	// Create the entries in the current ".txl" and save (adaptation of the code of OnEntryNew)
	mpt_EditText->ul_Num += ul_NbNewEntries;
	if(mpt_EditText->pst_Ids)
	{
		mpt_EditText->pst_Ids = (TEXT_tdst_Id *) MEM_p_Realloc
			(
				mpt_EditText->pst_Ids,
				mpt_EditText->ul_Num * sizeof(TEXT_tdst_Id)
			);
		//L_memset(&mpt_EditText->pst_Ids[mpt_EditText->ul_Num - 1], 0, sizeof(TEXT_tdst_Id));
	}
	else
	{
		mpt_EditText->pst_Ids = (TEXT_tdst_Id *) MEM_p_Alloc(ul_NbNewEntries * sizeof(TEXT_tdst_Id));
		//L_memset(mpt_EditText->pst_Ids, 0, sizeof(TEXT_tdst_Id));
	}
	// Move the entries so that the new hole are at the right place
	while (j>=idx) {
		L_memcpy(mpt_EditText->pst_Ids + j + ul_NbNewEntries,
		         mpt_EditText->pst_Ids + j,
		         sizeof(TEXT_tdst_Id));
		j--;
	}
	// Init the new entries
	L_memset(mpt_EditText->pst_Ids + idx, 0, ul_NbNewEntries * sizeof(TEXT_tdst_Id));
	for(i = 0; i < ul_NbNewEntries; i++)
	{
		pIdDest = &mpt_EditText->pst_Ids[idx + i];
		pIdDest->i_Offset = -1;
		pIdDest->us_Priority = TEXT_e_PrioNormal;
		pIdDest->us_Version = TEXT_Cte_CurrVersion;
		pIdDest->c_AnimIdx = 0;
		pIdDest->c_FacialIdx = TEXT_e_ExpNormal;
		pIdDest->c_DumIdx = 0;
		pIdDest->c_LipsIdx = TEXT_e_LipsNormal;
		pIdDest->ul_ObjKey = BIG_C_InvalidKey;
		pIdDest->pv_Obj = NULL;
		pIdDest->ul_SoundKey = BIG_C_InvalidKey;
		pIdDest->ul_IdKey = BIG_ul_GetNewKey(BIG_C_InvalidKey);
		pIdDest->asz_Comment = (char *) MEM_p_Alloc(32);
		L_memset(pIdDest->asz_Comment, 0, 32);
		for(int jj = 0; jj < 32; jj++) pIdDest->i_CommentOffset[jj] = jj;

		L_strcpy(pIdDest->asz_Name, "<hole>");
		pIdDest->asz_Name[TEXT_MaxLenId - 1] = 0;

		/* filter the entry name */
		ETEXT_FilterString(pIdDest->asz_Name, TEXT_MaxLenId, 23);
		ETEXT_BuildAutoLabelString(pIdDest->asz_LabelAuto, pIdDest->asz_Name, pIdDest->ul_IdKey);

		if(mpt_EditText->psz_Text == NULL)
		{
			mpt_EditText->psz_Text = (char *) MEM_p_Alloc(1);
			*mpt_EditText->psz_Text = 0;
		}

		SetFileModified(TRUE);
		mb_NeedSync = TRUE;
	}

	// Sauvegarde
	InitFileIdx = ul_GetCurFile();

	// Iteration sur tous les ".txl" du ".txg" courant
	pGroupTemp = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmp(BIG_PosFile(ul_GetCurBank()), (ULONG *) &ulSize);
	// We must copy the content of pGroupTemp or it will be erased by FileChange(...) later
	pGroup = (BIG_tdst_GroupElem *) MEM_p_Alloc(ulSize);
	L_memcpy(pGroup, pGroupTemp, ulSize);
	// Now we can free pGroupTemp
	BIG_FreeGlobalBuffer();
	BOOL firstValidTxl = TRUE;
	for(i = 0; i < ulSize / sizeof(BIG_tdst_GroupElem); i++, pGroup++) {
		if((pGroup->ul_Key == BIG_C_InvalidKey) || (pGroup->ul_Key == 0)) {
			// Fichier vide
			continue;
		}
		CurFileIdx = BIG_ul_SearchKeyToFat(pGroup->ul_Key);
		if(CurFileIdx == BIG_C_InvalidIndex) {
			// Unknown key
			continue;
		}
		FileChange(CurFileIdx);

		// Find the right TEXT_tdst_OneText in the buffer
		//BufferText = BufferBank.pst_AllTexts[TEXT_i_GetOneTextIndex(pGroup->ul_Key)];
		BufferText = BufferBank.pst_AllTexts[i];
		if (!BufferText) continue;

		curOffset = (int)TEXT_ui_GetTextLenght(mpt_EditText);
		for (j=0; j<BufferText->ul_Num; j++) {
			pIdDest = mpt_EditText->pst_Ids + idx + j;
			pIdSrc  = BufferText->pst_Ids + j;
			TEXT_EntryInterestingFieldsCopy(pIdDest, pIdSrc, false);

// TODO : comments
		// Done in TEXT_EntryInterestingFieldsCopy
			//if (firstValidTxl && pIdSrc->asz_Comment) {
			//	MEM_Free(pIdDest->asz_Comment);
			//	pIdDest->asz_Comment = (char *) MEM_p_Alloc(L_strlen(pIdSrc->asz_Comment)+1);
			//	L_memcpy(pIdDest->asz_Comment, pIdSrc->asz_Comment, L_strlen(pIdSrc->asz_Comment)+1);
			//	L_memcpy(pIdDest->i_CommentOffset, pIdSrc->i_CommentOffset, 100*sizeof(int));
			//}
			if (firstValidTxl) TEXT_CopyComment (pIdDest, pIdSrc);

			// Text
			if (pIdDest->i_Offset != -1) {
				// Clear the text
				char *strToClear = mpt_EditText->psz_Text + pIdDest->i_Offset;
				int len = L_strlen(strToClear) + 1;
				L_memmove(
					strToClear,
					strToClear + L_strlen(strToClear) + 1,
					curOffset - pIdDest->i_Offset);
				// Decaler les offset des entrées suivantes
				for (int i=0; i<mpt_EditText->ul_Num; i++) {
					if (mpt_EditText->pst_Ids[i].i_Offset > pIdDest->i_Offset) {
						mpt_EditText->pst_Ids[i].i_Offset -= len;
					}
				}
				// Update size of the text
				curOffset -= len;
			}
			// Copy the text
			if (pIdSrc->i_Offset != -1) {
				pIdDest->i_Offset = curOffset;
				curOffset += L_strlen(BufferText->psz_Text + pIdSrc->i_Offset) + 1;
				mpt_EditText->psz_Text = (char*)MEM_p_Realloc(mpt_EditText->psz_Text, curOffset);
				L_memcpy(
					mpt_EditText->psz_Text + pIdDest->i_Offset,
					BufferText->psz_Text + pIdSrc->i_Offset,
					L_strlen(BufferText->psz_Text + pIdSrc->i_Offset) + 1);
			}
			SetFileModified(TRUE);
			mb_NeedSync = TRUE;
		}
		if (firstValidTxl) {
			firstValidTxl = FALSE;
		}
	}

	// Restauration
	FileChange(InitFileIdx);
	if (idx) mpt_EditText->pst_Ids[idx-1].sel = 1;
	else     mpt_EditText->pst_Ids[idx].sel   = 1;
	if(mpo_Scroll) mpo_Scroll->TextChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnBankOpen(ULONG ul_Index)
{
	/*~~~~*/
	int res;
	/*~~~~*/

	if(ul_Index == BIG_C_InvalidIndex)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		EDIA_cl_FileDialog	o_File("Choose File", 0, 0, 1, NULL, "*"EDI_Csz_ExtTextLang);
		CString				o_Temp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(o_File.DoModal() == IDOK)
		{
			o_File.GetItem(o_File.mo_File, 0, o_Temp);
			ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
		}
	}

	if(ul_Index == BIG_C_InvalidIndex) return;

	res = mpo_ListBank->FindString(-1, BIG_NameFile(ul_Index));
	if(res == LB_ERR)
	{
		res = mpo_ListBank->AddString(BIG_NameFile(ul_Index));
		if(res != LB_ERR)
		{
			mpo_ListBank->SetItemData(res, ul_Index);
		}
	}

	mpo_ListBank->SetCurSel(res);

	OnBankChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnBankClose(void)
{
	/*~~~~~~~~~~*/
	ULONG	ulFat;
	int		item;
	/*~~~~~~~~~~*/

	ulFat = ul_GetCurBank();
	if(ulFat == BIG_C_InvalidIndex) return;
	OnFileClose();

	item = mpo_ListBank->FindString(-1, BIG_NameFile(ulFat));
	if(item == LB_ERR) return;

	mpo_ListBank->DeleteString(item);
	mpo_ListBank->SetCurSel(0);

	OnBankChange();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::AfterEngine(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Frame::i_GetSelNb(void)
{
	/*~~~~~~~*/
	int i, ret;
	/*~~~~~~~*/

	ret = 0;

	if(!mpt_EditText) return 0;
	for(i = 0; i < (int) mpt_EditText->ul_Num; i++)
		if(mpt_EditText->pst_Ids[i].sel) ret++;
	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Frame::PreTranslateMessage(MSG *pMsg)
{
	/*~~~~~~~~~~~~*/
	CPoint	o_Point;
	CMenu	o_Menu;
	CPoint	pt;
	/*~~~~~~~~~~~~*/

	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN:
		break;

	case WM_RBUTTONDOWN:
		if(pMsg->hwnd == mpo_ListBank->GetSafeHwnd())
		{
			BankPopup();
			return 1;
		}
		else if(pMsg->hwnd == mpo_ListFile->GetSafeHwnd())
		{
			FilePopup();
			return 1;
		}
		break;

	case WM_KEYDOWN:
		if((pMsg->wParam == VK_ESCAPE) || ((pMsg->wParam == VK_RETURN) && GetAsyncKeyState(VK_SHIFT) < 0))
		{
			mpo_Scroll->UpdateSel(-1, 0, 0, -1);
			if(mpo_Scroll->mb_EditContent) mpo_Scroll->OnLooseFocus();
			return 1;
		}
		break;

	default:
		break;
	}

	return EDI_cl_BaseFrame::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::BankPopup(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint			o_Point;
	EMEN_cl_SubMenu o_SubMenu(FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&o_Point);
	this->InitPopupMenuAction(&o_SubMenu);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_BANK_NEW);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_BANK_CLOSE);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_BANK_OPEN);
	this->TrackPopupMenuAction(o_Point, &o_SubMenu);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::FilePopup(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint			o_Point;
	EMEN_cl_SubMenu o_SubMenu(FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&o_Point);
	this->InitPopupMenuAction(&o_SubMenu);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_FILE_NEW);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_SYNC);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_FILE_SAVE);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_FILE_DEL);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_FILE_AUTOSNDFILE);
	this->TrackPopupMenuAction(o_Point, &o_SubMenu);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::EntryPopup(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CPoint			o_Point;
	EMEN_cl_SubMenu o_SubMenu(FALSE);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetCursorPos(&o_Point);
	this->InitPopupMenuAction(&o_SubMenu);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_ENTRY_NEW);
	this->AddPopupMenuAction(&o_SubMenu, ETEXT_ACTION_ENTRY_DEL);
	this->TrackPopupMenuAction(o_Point, &o_SubMenu);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::CompressComment(TEXT_tdst_OneText *pText)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			*sz_NewComment, *sz_Cur;
	ULONG			i, j, ulComment;
	TEXT_tdst_Id	*p;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pText) return;
	p = pText->pst_Ids;
	for(i = 0; i < pText->ul_Num; i++, p++)
	{
		ulComment = p->i_CommentOffset[31] + L_strlen(p->asz_Comment + p->i_CommentOffset[31]) + 1;
		sz_NewComment = (char *) MEM_p_Alloc(ulComment);
		sz_Cur = sz_NewComment;
		for(j = 0; j < 32; j++)
		{
			L_strcpy(sz_Cur, p->asz_Comment + p->i_CommentOffset[j]);
			p->i_CommentOffset[j] = sz_Cur - sz_NewComment;
			sz_Cur += L_strlen(sz_Cur) + 1;
		}

		MEM_Free(p->asz_Comment);
		p->asz_Comment = sz_NewComment;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnCloseAll(void)
{
	while(mpo_ListBank->GetCount())
	{
		mpo_ListBank->SetCurSel(0);
		OnBankClose();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnSynchronize(void)
{
	/*~~~~~~~~~~~~~*/
	ULONG	ulRefFat;
	ULONG	ulFat;
	int		i;
	/*~~~~~~~~~~~~~*/

	ulRefFat = BIG_ul_SearchKeyToFat(mpt_EditText->ul_Key);
	for(i = 0; i < mpo_ListFile->GetCount(); i++)
	{
		ulFat = mpo_ListFile->GetItemData(i);
		if(ulFat == 0) continue;
		if(ulFat == BIG_C_InvalidIndex) continue;
		if(ulFat == ulRefFat) continue;
		Synchronize(ulFat, ulRefFat);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::Synchronize(ULONG ulFatDest, ULONG ulFatRef)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_OneText	*pRef, *pTextOld, *pTextNew, stText;
	TEXT_tdst_Id		*pDestId, *pSrcId;
	ULONG				i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mpt_EditText && (mpt_EditText->ul_Key == BIG_FileKey(ulFatRef)))
		pRef = mpt_EditText;
	else
		* (ULONG *) &pRef = TEXT_ul_CallbackLoadTxlEditor(BIG_PosFile(ulFatRef), BIG_FileKey(ulFatRef));

	*(ULONG *) &pTextOld = TEXT_ul_CallbackLoadTxlEditor(BIG_PosFile(ulFatDest), BIG_FileKey(ulFatDest));

	/**/
	pTextNew = (TEXT_tdst_OneText *) MEM_p_Alloc(sizeof(TEXT_tdst_OneText));
	L_memset(pTextNew, 0, sizeof(TEXT_tdst_OneText));

	pTextNew->ul_Inst = 1;
	pTextNew->ul_Key = pTextOld->ul_Key;
	pTextNew->ul_Num = pRef->ul_Num;

	pTextNew->psz_Text = (char *) MEM_p_Alloc(1);
	*pTextNew->psz_Text = 0;

	if(pRef->ul_Num)
	{
		pTextNew->pst_Ids = (TEXT_tdst_Id *) MEM_p_Alloc(pRef->ul_Num * sizeof(TEXT_tdst_Id));
		L_memset(pTextNew->pst_Ids, 0, pRef->ul_Num * sizeof(TEXT_tdst_Id));
	}
	/**/
	pSrcId = pRef->pst_Ids;
	pDestId = pTextNew->pst_Ids;
	for(i = 0; i < pRef->ul_Num; i++, pDestId++, pSrcId++)
	{
		L_strcpy(pDestId->asz_Name, pSrcId->asz_Name);
		ETEXT_BuildAutoLabelString(pDestId->asz_LabelAuto, pSrcId->asz_Name, pSrcId->ul_IdKey);
		pDestId->ul_IdKey = pSrcId->ul_IdKey;
		pDestId->ul_ObjKey = pSrcId->ul_ObjKey;
		pDestId->pv_Obj = NULL;
		pDestId->ul_SoundKey = BIG_C_InvalidKey;
		pDestId->us_Priority = pSrcId->us_Priority;
		pDestId->us_Version = pSrcId->us_Version;

		pDestId->c_AnimIdx = pSrcId->c_AnimIdx;
		pDestId->c_DumIdx = pSrcId->c_DumIdx;
		pDestId->c_FacialIdx = pSrcId->c_FacialIdx;
		pDestId->c_LipsIdx = pSrcId->c_LipsIdx;

		pDestId->i_Offset = -1;

		/* Comment */
		j = pRef->pst_Ids[i].i_CommentOffset[31];
		j += L_strlen(pRef->pst_Ids[i].asz_Comment + j) + 1;
		pDestId->asz_Comment = (char *) MEM_p_Alloc(j);
		L_memcpy(pDestId->asz_Comment, pRef->pst_Ids[i].asz_Comment, j);
		L_memcpy(pDestId->i_CommentOffset, pRef->pst_Ids[i].i_CommentOffset, sizeof(pDestId->i_CommentOffset));

		/*
		 * pDestId->asz_Comment = (char *)
		 * MEM_p_Alloc(L_strlen(pRef->pst_Ids[i].asz_Comment) + 1);
		 * L_strcpy(pDestId->asz_Comment, pRef->pst_Ids[i].asz_Comment);
		 */

		j = TEXT_i_GetEntryIndex(pTextOld, pSrcId->ul_IdKey);

		if(mst_Ini.i_OptionFlags & ETEXT_Cte_KeepAllTxt)
		{
			/* we don't want to synchronize text data */
			if(j==-1)
			{
#ifdef JADEFUSION
			/*popowarning popoverif popoajour ATTENTION code avant ?? ??
				if(mst_Ini.i_OptionFlags & ETEXT_Cte_KeepEmptyTxt)
					pTextNew->pst_Ids[i].i_Offset = -1;
				else
					TEXT_SetTextID(pTextNew, i, "[need translation]");
			}
			else if
				(
					(mst_Ini.i_OptionFlags & ETEXT_Cte_KeepEmptyTxt)
				&&	(L_strlen(pRef->psz_Text + pRef->pst_Ids[i].i_Offset) < 2)
				)	// just one space
					TEXT_SetTextID(pTextNew, i, pRef->psz_Text + pRef->pst_Ids[i].i_Offset);
				else
			{		// else
					if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<hole>") == 0)
						TEXT_SetTextID(pTextNew, i, "<hole>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<notext>") == 0)
						TEXT_SetTextID(pTextNew, i, "<notext>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<todo>") == 0)
						TEXT_SetTextID(pTextNew, i, "<todo>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<sfx>") == 0)
						TEXT_SetTextID(pTextNew, i, "<sfx>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<deleted>") == 0)
						TEXT_SetTextID(pTextNew, i, "<deleted>");
					else
						TEXT_SetTextID(pTextNew, i, "[need translation]");
				}
			}*/
			//**** NEW ?? ?? *****
#endif
				// new entry
				TEXT_SetTextID(pTextNew, i, "[need translation]");
			}
			else
			{
				// entry exist yet
				pDestId->ul_SoundKey = pTextOld->pst_Ids[j].ul_SoundKey;
				
				if(pTextOld->pst_Ids[j].i_Offset == -1)
				{
					// old text was empty => lets it
					pTextNew->pst_Ids[i].i_Offset = -1;
				}
				else
				{
					// copy old text
					TEXT_SetTextID(pTextNew, i, pTextOld->psz_Text + pTextOld->pst_Ids[j].i_Offset);
				}
			}
		}
		else
		{
			/* we want to synchronize text data */
			if(j == -1)
			{
				/* => new entry */
				if(pRef->pst_Ids[i].i_Offset == -1) /* no text */
				{
					if(mst_Ini.i_OptionFlags & ETEXT_Cte_KeepEmptyTxt)
						pTextNew->pst_Ids[i].i_Offset = -1;
					else
						TEXT_SetTextID(pTextNew, i, "[need translation]");
				}
				else if
					(
						(mst_Ini.i_OptionFlags & ETEXT_Cte_KeepEmptyTxt)
					&&	(L_strlen(pRef->psz_Text + pRef->pst_Ids[i].i_Offset) < 2)
					)	/* just one space */
					TEXT_SetTextID(pTextNew, i, pRef->psz_Text + pRef->pst_Ids[i].i_Offset);
				else
				{		/* else */
					if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<hole>") == 0)
						TEXT_SetTextID(pTextNew, i, "<hole>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<notext>") == 0)
						TEXT_SetTextID(pTextNew, i, "<notext>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<todo>") == 0)
						TEXT_SetTextID(pTextNew, i, "<todo>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<sfx>") == 0)
						TEXT_SetTextID(pTextNew, i, "<sfx>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<deleted>") == 0)
						TEXT_SetTextID(pTextNew, i, "<deleted>");
					else
						TEXT_SetTextID(pTextNew, i, "[need translation]");
				}
			}
			else
			{
				/* existing entry */
				pDestId->ul_SoundKey = pTextOld->pst_Ids[j].ul_SoundKey;

				if
				(
					((pRef->pst_Ids[i].i_Offset != -1) && L_strlen(pRef->psz_Text + pRef->pst_Ids[i].i_Offset))
				&&	!
						(
							(pTextOld->pst_Ids[j].i_Offset != -1)
						&&	L_strlen(pTextOld->psz_Text + pTextOld->pst_Ids[j].i_Offset)
						)
				)
				{
					/* ref has a text + old has no text => we just have fill in a new text */
					if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<hole>") == 0)
						TEXT_SetTextID(pTextNew, i, "<hole>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<notext>") == 0)
						TEXT_SetTextID(pTextNew, i, "<notext>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<todo>") == 0)
						TEXT_SetTextID(pTextNew, i, "<todo>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<sfx>") == 0)
						TEXT_SetTextID(pTextNew, i, "<sfx>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<deleted>") == 0)
						TEXT_SetTextID(pTextNew, i, "<deleted>");
					else
						TEXT_SetTextID(pTextNew, i, "[need translation]");
				}
				else if(!((pRef->pst_Ids[i].i_Offset != -1) && L_strlen(pRef->psz_Text + pRef->pst_Ids[i].i_Offset)))
				{
					/* ref has no text => we have deleted the ref text */
					TEXT_SetTextID(pTextNew, i, "");
				}
				else if
					(
						(pTextOld->pst_Ids[j].i_Offset != -1)
					&&	!L_strcmp(pTextOld->psz_Text + pTextOld->pst_Ids[j].i_Offset, "[need translation]")
					)
				{
					/* the prev text was need translation => check if the content meaning has changed */
					if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<hole>") == 0)
						TEXT_SetTextID(pTextNew, i, "<hole>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<notext>") == 0)
						TEXT_SetTextID(pTextNew, i, "<notext>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<todo>") == 0)
						TEXT_SetTextID(pTextNew, i, "<todo>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<sfx>") == 0)
						TEXT_SetTextID(pTextNew, i, "<sfx>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<deleted>") == 0)
						TEXT_SetTextID(pTextNew, i, "<deleted>");
					else
						TEXT_SetTextID(pTextNew, i, "[need translation]");
				}
				else
				{
					if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<hole>") == 0)
						TEXT_SetTextID(pTextNew, i, "<hole>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<notext>") == 0)
						TEXT_SetTextID(pTextNew, i, "<notext>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<todo>") == 0)
						TEXT_SetTextID(pTextNew, i, "<todo>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<sfx>") == 0)
						TEXT_SetTextID(pTextNew, i, "<sfx>");
					else if(L_strcmp(pRef->psz_Text + pRef->pst_Ids[i].i_Offset, "<deleted>") == 0)
						TEXT_SetTextID(pTextNew, i, "<deleted>");
					else
						TEXT_SetTextID(pTextNew, i, pTextOld->psz_Text + pTextOld->pst_Ids[j].i_Offset);
				}
			}
		}				
		
		if((mi_ForceSynchroEntry != -1) && (mi_ForceSynchroEntry == i))
		{
			ERR_X_Warning(mst_ForceSynchroEntry.ul_IdKey == pDestId->ul_IdKey, "entry key are different", NULL);
			pDestId->ul_SoundKey = mst_ForceSynchroEntry.ul_SoundKey;
		}
	}

	SaveFile(pTextNew);

	L_memcpy(&stText, pTextOld, sizeof(TEXT_tdst_OneText));
	L_memcpy(pTextOld, pTextNew, sizeof(TEXT_tdst_OneText));
	L_memcpy(pTextNew, &stText, sizeof(TEXT_tdst_OneText));

	pTextNew->ul_Inst = 1;
	TEXT_Free(pTextNew);

	pTextOld->ul_Inst = 1;
	TEXT_Free(pTextOld);

	if(mpt_EditText != pRef)
	{
		pRef->ul_Inst = 1;
		TEXT_Free(pRef);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnSynchronizeAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Index;
	char			log[256];
	EDIA_cl_Lang	o_Lang;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(mi_SynchroLang == INO_e_Dummy)
	{
		if(o_Lang.DoModal() != IDOK) return;
	}
	else
	{
		o_Lang.mi_Lang = mi_SynchroLang;
	}

	OnCloseAll();

	sprintf(log, "Synchronizing all files from %s language...", INO_dst_LanguageName[o_Lang.mi_Lang].az_LongName);
	LINK_PrintStatusMsg(log);

	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;
		if(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtTextLang)) continue;

		sprintf(log, "- %s [%x]", BIG_NameFile(ul_Index), BIG_FileKey(ul_Index));
		LINK_PrintStatusMsg(log);

		/* load the bank */
		OnBankOpen(ul_Index);

		/* select ref language file */
		i = mpo_ListFile->GetItemData(o_Lang.mi_Lang);
		if(!i || (i == -1))
		{
			ERR_X_Warning(0, "  The reference language file doesn't exist !", NULL);
			continue;
		}

		mpo_ListFile->SetCurSel(o_Lang.mi_Lang);

		/* mpo_ListEntry->mi_CurrSelCol = mpo_ListEntry->mi_CurrSelRow = -1; */
		OnFileChange();
		OnSynchronize();

		/* close bank */
		OnBankClose();
	}

	OnCloseAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFind(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog			o_Name("Enter substring");
	static EDIA_cl_NameDialog	o_FindEntry("Enter key to search");
	int							i;
	CString						str;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetEntryColumnInfo();
	SetEntryColumnInfo();
	OnFileChange();
	mb_FindSomething = FALSE;

	if(!mpt_EditText) return;

	if(GetAsyncKeyState(VK_SHIFT) < 0)
	{
		/*~~~~~~~~~~~~~~~~~~*/
		int iEntry, iEntryKey;
		/*~~~~~~~~~~~~~~~~~~*/

		if(o_FindEntry.DoModal() != IDOK) return;
		sscanf((char *) (LPCSTR) o_FindEntry.mo_Name, "%x", &iEntryKey);
		iEntry = TEXT_i_GetEntryIndex(mpt_EditText, iEntryKey);
		if(iEntry < 0)
		{
			MessageBox("Entry Key doesn't exist", "Find result");
		}
		else
		{
			mpt_EditText->pst_Ids[iEntry].searchres |= TEXT_SearchResult_EntryKey;
			mb_FindSomething = TRUE;
		}
	}
	else
	{
		if(o_Name.DoModal() != IDOK) return;

		if(!o_Name.mo_Name.GetLength())
		{
			for(i = 0; i < (int) mpt_EditText->ul_Num; i++) mpt_EditText->pst_Ids[i].searchres = 0;
			mb_FindSomething = FALSE;
			mpo_Scroll->Invalidate();
			return;
		}

		o_Name.mo_Name.MakeLower();
		for(i = 0; i < (int) mpt_EditText->ul_Num; i++)
		{
			mpt_EditText->pst_Ids[i].searchres = 0;

			str = CString(mpt_EditText->pst_Ids[i].asz_Name);
			str.MakeLower();
			if(str.Find(o_Name.mo_Name) != -1)
			{
				mpt_EditText->pst_Ids[i].searchres |= TEXT_SearchResult_Name;
				mb_FindSomething = TRUE;
			}

			if(mpt_EditText->pst_Ids[i].i_Offset == -1) continue;

			str = CString(mpt_EditText->psz_Text + mpt_EditText->pst_Ids[i].i_Offset);
			str.MakeLower();
			if(str.Find(o_Name.mo_Name) != -1)
			{
				mpt_EditText->pst_Ids[i].searchres |= TEXT_SearchResult_Content;
				mb_FindSomething = TRUE;
			}
		}
	}

	if(mb_FindSomething)
	{
		mi_FindLine = -1;
		OnFindNext(1);
		mpo_Scroll->Invalidate();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFindSaleChar(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	CString						str;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GetEntryColumnInfo();
	SetEntryColumnInfo();
	OnFileChange();
	mb_FindSomething = FALSE;

	if(!mpt_EditText) return;

	for(i = 0; i < (int) mpt_EditText->ul_Num; i++)
	{
		mpt_EditText->pst_Ids[i].searchres = 0;
		if(mpt_EditText->pst_Ids[i].i_Offset == -1) continue;
		if ( strchr( mpt_EditText->psz_Text + mpt_EditText->pst_Ids[i].i_Offset, 0xA0 ) )
		{
			mpt_EditText->pst_Ids[i].searchres |= TEXT_SearchResult_Content;
			mb_FindSomething = TRUE;
		}
	}

	if(mb_FindSomething)
	{
		mi_FindLine = -1;
		OnFindNext(1);
		mpo_Scroll->Invalidate();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnReplaceSaleChar(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	int		i, i_Save;
	char	*sz_SaleChar;			
	/*~~~~~~~~~~~~~~~~~~~*/

	GetEntryColumnInfo();
	SetEntryColumnInfo();
	OnFileChange();
	i_Save = 0;

	if(!mpt_EditText) return;
	i_Save = 0;
	for(i = 0; i < (int) mpt_EditText->ul_Num; i++)
	{
		if(mpt_EditText->pst_Ids[i].i_Offset == -1) continue;
		while ( sz_SaleChar = strchr( mpt_EditText->psz_Text + mpt_EditText->pst_Ids[i].i_Offset, 0xA0 ) )
		{
			*sz_SaleChar = ' ';
			i_Save = 1;
		}
	}

	if(i_Save)
		OnFileSave();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFindNext(int _i_Next)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int c, idx, col, line, search, prev;
	int First_line, First_col;
	int Prev_line, Prev_col;
	int Next_line, Next_col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpt_EditText || !mb_FindSomething) return;

	First_line = -1;
	Next_line = -1;
	Prev_line = -1;
	prev = 1;

	for(line = 0; line < (int) mpt_EditText->ul_Num; line++)
	{
		idx = mpt_EditText->pst_Ids[line].order;
		if((search = mpt_EditText->pst_Ids[idx].searchres))
		{
			for(col = 0; col < COLMAX; col++)
			{
				c = mst_Ini.ai_ColOrder[col];
				if(mst_Ini.ai_ColSize[c] <= 0) continue;
				c++;

				if
				(
					((c == en_Preview) && (search & TEXT_SearchResult_Content))
				||	((c == en_ID) && (search & TEXT_SearchResult_Name))
				||	((c == en_IDKey) && (search & TEXT_SearchResult_EntryKey))
				)
				{
					/* first */
					if(First_line == -1)
					{
						First_line = idx;
						First_col = c;
					}

					/* next */
					if(mi_FindLine == -1)
					{
						Next_line = idx;
						Next_col = c;
						goto OnFindNext_ThisIsTheEnd;
					}
					else if(mi_FindLine == idx)
					{
						if(mi_FindCol == c)
						{
							mi_FindLine = -1;
							prev = 0;
						}
						else if(mi_FindCol == -1)
						{
							Next_line = idx;
							Next_col = mi_FindCol = c;
							goto OnFindNext_ThisIsTheEnd;
						}
					}

					/* previous */
					if(prev)
					{
						Prev_line = idx;
						Prev_col = c;
					}
				}
			}
		}

		if((mi_FindLine > 0) && (mi_FindLine == idx)) mi_FindLine = -1;
	}

OnFindNext_ThisIsTheEnd:
	if(_i_Next)
	{
		if(Next_line == -1)
		{
			mi_FindLine = First_line;
			mi_FindCol = First_col;
		}
		else
		{
			mi_FindLine = Next_line;
			mi_FindCol = Next_col;
		}
	}
	else if(Prev_line != -1)
	{
		mi_FindLine = Prev_line;
		mi_FindCol = Prev_col;
	}

	if(mi_FindLine != -1) mpo_Scroll->EnsureVisible(mi_FindLine, mi_FindCol);
	mpo_Scroll->Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFileOpen(ULONG ulFat)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < mpo_ListFile->GetCount(); i++)
	{
		if(mpo_ListFile->GetItemData(i) == ulFat)
		{
			mpo_ListFile->SetCurSel(i);
			OnFileChange();
		}
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
char *ETEXT_cl_Frame::CEL_GetText(int _i_Line, int _i_ColId)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static char		sz_Text[256];
	TEXT_tdst_Id	*pst_TextId;
	ULONG			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_TextId = mpt_EditText->pst_Ids + _i_Line;
	*sz_Text = 0;

	if(!mpt_EditText->pst_Ids) return sz_Text;

	switch(_i_ColId &~en_Masked)
	{
	/* Empty */
	case en_Empty:
		return sz_Text;

	/* Rank */
	case en_Rank:
		return(itoa(_i_Line, sz_Text, 10));

	/* ID */
	case en_ID:
		return pst_TextId->asz_Name;

	/* Preview */
	case en_Preview:
		if(!pst_TextId) return sz_Text;
		return(pst_TextId->i_Offset == -1) ? sz_Text : mpt_EditText->psz_Text + pst_TextId->i_Offset;

	/* Key */
	case en_IDKey:
		sprintf(sz_Text, "0x%08X", pst_TextId->ul_IdKey);
		return sz_Text;

	/* Snd Key */
	case en_Priority:
		if(b_HasUserValue(en_Priority)) return p_GetUserValueToName(en_Priority, (ULONG) pst_TextId->us_Priority);

		switch(pst_TextId->us_Priority)
		{
		case TEXT_e_PrioUltraHigh:	return "ultra high";
		case TEXT_e_PrioUltraLow:	return "ultra low";
		case TEXT_e_PrioVeryHigh:	return "very high";
		case TEXT_e_PrioNormal:		return "normal";
		case TEXT_e_PrioVeryLow:	return "very low";
		case TEXT_e_PrioHigh:		return "high";
		case TEXT_e_PrioLow:		return "low";
		}

		return "undefined";

	case en_FacialExp:
		if(b_HasUserValue(en_FacialExp)) return p_GetUserValueToName(en_FacialExp, (ULONG) pst_TextId->c_FacialIdx);

		switch(pst_TextId->c_FacialIdx)
		{
		case TEXT_e_ExpAngry:		return "angry";
		case TEXT_e_ExpFighting:	return "fighting";
		case TEXT_e_ExpHappy:		return "happy";
		case TEXT_e_ExpNormal:		return "normal";
		case TEXT_e_ExpRelieved:	return "relieved";
		case TEXT_e_ExpSurprised:	return "surprise";
		case TEXT_e_ExpWorried:		return "worried";
		}

		return "undefined";

	case en_Lips:
		if(b_HasUserValue(en_Lips)) return p_GetUserValueToName(en_Lips, (ULONG) pst_TextId->c_LipsIdx);

		switch(pst_TextId->c_LipsIdx)
		{
		case TEXT_e_LipsNormal:		return "normal";
		case TEXT_e_LipsShouted:	return "shouted";
		case TEXT_e_LipsWhispered:	return "whispered";
		case TEXT_e_LipsScreamed:	return "screamed";
		}

		return "undefined";

	case en_Anims:
		if(b_HasUserValue(en_Anims))
			return p_GetUserValueToName(en_Anims, (ULONG) pst_TextId->c_AnimIdx);
		else
			return(itoa(pst_TextId->c_AnimIdx, sz_Text, 10));

	/* Snd Key */
	case en_SndFileKey:
		if(pst_TextId->ul_SoundKey == 0) return "muted";
		if(pst_TextId->ul_SoundKey == BIG_C_InvalidKey) return "none";
		ul_Index = BIG_ul_SearchKeyToFat(pst_TextId->ul_SoundKey);
		if(ul_Index == BIG_C_InvalidIndex) return "invalid";
		return BIG_NameFile(ul_Index);

	/* character */
	case en_Character:
		if(pst_TextId->ul_ObjKey == 0) return "nobody";
		if(pst_TextId->ul_ObjKey == BIG_C_InvalidKey) return "none";
		ul_Index = BIG_ul_SearchKeyToFat(pst_TextId->ul_ObjKey);
		if(ul_Index == BIG_C_InvalidIndex) return "invalid";
		return BIG_NameFile(ul_Index);

	/* txg key */
	case en_TxgFileKey:
		sprintf(sz_Text, "0x%08X", BIG_FileKey(ul_GetCurBank()));
		return sz_Text;

	/* txl key */
	case en_TxlFileKey:
		sprintf(sz_Text, "0x%08X", mpt_EditText->ul_Key);
		return sz_Text;

	/* label auto */
	case en_LabelAuto:
		return pst_TextId->asz_LabelAuto;

	/* other = user */
	default:
		if((en_UserData <= _i_ColId) && (_i_ColId < en_UserData + 32))
			return pst_TextId->asz_Comment + pst_TextId->i_CommentOffset[_i_ColId - en_UserData];
		return sz_Text;
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
int ETEXT_cl_Frame::CEL_GetContent(int _i_Line, int _i_ColId, ULONG *_pul_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static char		sz_Text[256];
	TEXT_tdst_Id	*pst_TextId;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pul_Value) return ETEXT_Content_None;

	pst_TextId = mpt_EditText->pst_Ids + _i_Line;
	*sz_Text = 0;
	switch(_i_ColId &~en_Masked)
	{
	/* Empty */
	case en_Empty:
		return ETEXT_Content_None;

	/* Rank */
	case en_Rank:
		*_pul_Value = _i_Line;
		return ETEXT_Content_Int;

	/* ID */
	case en_ID:
		*_pul_Value = (ULONG) pst_TextId->asz_Name;
		return ETEXT_Content_String;

	case en_Priority:
		*_pul_Value = (ULONG) pst_TextId->us_Priority;
		return ETEXT_Content_Int;

	case en_Anims:
		*_pul_Value = (ULONG) pst_TextId->c_AnimIdx;
		return ETEXT_Content_Int;

	case en_Lips:
		*_pul_Value = (ULONG) pst_TextId->c_LipsIdx;
		return ETEXT_Content_Int;

	case en_FacialExp:
		*_pul_Value = (ULONG) pst_TextId->c_FacialIdx;
		return ETEXT_Content_Int;

	/* Preview */
	case en_Preview:
		*_pul_Value = (ULONG) ((pst_TextId->i_Offset == -1) ? sz_Text : mpt_EditText->psz_Text + pst_TextId->i_Offset);
		return ETEXT_Content_String;

	/* Key */
	case en_IDKey:
		*_pul_Value = pst_TextId->ul_IdKey;
		return ETEXT_Content_Int;

	/* Snd Key */
	case en_SndFileKey:
		*_pul_Value = pst_TextId->ul_SoundKey;
		return ETEXT_Content_Int;

	/* character */
	case en_Character:
		*_pul_Value = pst_TextId->ul_ObjKey;
		return ETEXT_Content_Int;

	/* txg key */
	case en_TxgFileKey:
		*_pul_Value = BIG_FileKey(ul_GetCurBank());
		return ETEXT_Content_Int;

	/* txl key */
	case en_TxlFileKey:
		*_pul_Value = mpt_EditText->ul_Key;
		return ETEXT_Content_Int;

	/* label auto */
	case en_LabelAuto:
		*_pul_Value = (ULONG) pst_TextId->asz_LabelAuto;
		return ETEXT_Content_String;

	/* other = user */
	default:
		if((en_UserData <= _i_ColId) && (_i_ColId < en_UserData + 32))
		{
			*_pul_Value = (ULONG) (pst_TextId->asz_Comment + pst_TextId->i_CommentOffset[_i_ColId - en_UserData]);
			return ETEXT_Content_String;
		}
	}

	return ETEXT_Content_None;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Frame::CEL_SetString(int _i_Row, int _i_Col, char *_sz_String)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*pId;
	int				i, length, userdata;
	char			*sz_NewComment, *sz_Cur;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_i_Row < 0) || (_i_Row >= (int) mpt_EditText->ul_Num)) return -1;
	pId = &mpt_EditText->pst_Ids[_i_Row];

	switch(_i_Col)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case en_ID:
		mb_NeedSync = TRUE;
		L_memcpy(pId->asz_Name, _sz_String, TEXT_MaxLenId - 1);
		pId->asz_Name[TEXT_MaxLenId - 1] = 0;
		ETEXT_FilterString(pId->asz_Name, TEXT_MaxLenId);
		ETEXT_BuildAutoLabelString(pId->asz_LabelAuto, pId->asz_Name, pId->ul_IdKey);
		return 0;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case en_Preview:
		TEXT_SetTextID(mpt_EditText, _i_Row, _sz_String);
		return 0;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	default:
		if(((int) en_UserData <= _i_Col) && (_i_Col < (int) en_UserData + 32))
		{
			mb_NeedSync = TRUE;
			userdata = _i_Col - en_UserData;

			if(L_strlen(pId->asz_Comment + pId->i_CommentOffset[userdata]) < L_strlen(_sz_String))
			{
				length = pId->i_CommentOffset[31];
				length += L_strlen(pId->asz_Comment + pId->i_CommentOffset[31]) + 1;
				length += L_strlen(_sz_String) + 1;

				sz_NewComment = (char *) MEM_p_Alloc(length + 1);
				sz_Cur = sz_NewComment;
				for(i = 0; i < 32; i++)
				{
					if(i == userdata)
						L_strcpy(sz_Cur, _sz_String);
					else
						L_strcpy(sz_Cur, pId->asz_Comment + pId->i_CommentOffset[i]);
					pId->i_CommentOffset[i] = sz_Cur - sz_NewComment;
					sz_Cur += L_strlen(sz_Cur) + 1;
				}

				MEM_Free(pId->asz_Comment);
				pId->asz_Comment = sz_NewComment;
			}
			else
				L_strcpy(pId->asz_Comment + pId->i_CommentOffset[userdata], _sz_String);
			return 0;
		}

		return -1;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ETEXT_cl_Frame::CEL_SetInt(int _i_Row, int _i_Col, ULONG _ul_Value)
{
	/*~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*pId;
	/*~~~~~~~~~~~~~~~~~*/

	if((_i_Row < 0) || (_i_Row >= (int) mpt_EditText->ul_Num)) return -1;
	pId = &mpt_EditText->pst_Ids[_i_Row];

	switch(_i_Col)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case en_Rank:		return -1;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case en_SndFileKey: pId->ul_SoundKey = _ul_Value; return 0;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case en_Priority:	pId->us_Priority = (unsigned short) (0xFFFF & _ul_Value); return 0;
	case en_Anims:		pId->c_AnimIdx = (char) (0xff & _ul_Value); return 0;
	case en_Lips:		pId->c_LipsIdx = (char) (0xff & _ul_Value); return 0;
	case en_FacialExp:	pId->c_FacialIdx = (char) (0xff & _ul_Value); return 0;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case en_Character:	pId->ul_ObjKey = _ul_Value; pId->pv_Obj = NULL; return 0;
	}

	return -1;
}

/*$4
 ***********************************************************************************************************************
    History functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::CheckKeyHistory(int *_pi_Nb, ULONG *_pul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_Key[10], i_Nb, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* save */
	i_Nb = *_pi_Nb;
	L_memcpy(ul_Key, _pul_Key, 10 * sizeof(ULONG));

	/* rebuild with check */
	*_pi_Nb = 0;
	for(i = 0; i < i_Nb; i++)
	{
		if(ul_Key[i] == 0) continue;
		if(ul_Key[i] == BIG_C_InvalidIndex) continue;
		if(BIG_ul_SearchKeyToFat(ul_Key[i]) == BIG_C_InvalidIndex) continue;
		_pul_Key[*_pi_Nb] = ul_Key[i];
		(*_pi_Nb)++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::AddKeyInHistory(int *_pi_Nb, ULONG *_pul_Key, ULONG _ul_Key)
{
	/*~~~~~~~~~~~~~~~*/
	int		i, i_Nb;
	ULONG	ul_Key[10];
	/*~~~~~~~~~~~~~~~*/

	/* save */
	i_Nb = *_pi_Nb;
	L_memcpy(ul_Key, _pul_Key, 10 * sizeof(ULONG));

	/* rebuild with new key */
	_pul_Key[0] = _ul_Key;
	*_pi_Nb = 1;

	for(i = 0; i < i_Nb; i++)
	{
		if(ul_Key[i] == _ul_Key) continue;
		_pul_Key[(*_pi_Nb)++] = ul_Key[i];
		if(*_pi_Nb == 10) break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::CheckSndFileHistory(void)
{
	CheckKeyHistory(&mst_Ini.i_NbSndFileKey, mst_Ini.ai_SndFileKey);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::AddSndFileInHistory(ULONG _ul_Key)
{
	AddKeyInHistory(&mst_Ini.i_NbSndFileKey, mst_Ini.ai_SndFileKey, _ul_Key);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::CheckCharacterHistory(void)
{
	CheckKeyHistory(&mst_Ini.i_NbCharacterKey, mst_Ini.ai_CharacterKey);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::AddCharacterInHistory(ULONG _ul_Key)
{
	AddKeyInHistory(&mst_Ini.i_NbCharacterKey, mst_Ini.ai_CharacterKey, _ul_Key);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
unsigned int TEXT_ui_GetTextLenght(TEXT_tdst_OneText *_pt_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	unsigned int	i, size;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(_pt_Text->ul_Num == 0) return 0;
	size = 0;

	for(i = 0; i < _pt_Text->ul_Num; i++)
	{
		if(_pt_Text->pst_Ids[i].i_Offset != -1)
			size += L_strlen(_pt_Text->psz_Text + _pt_Text->pst_Ids[i].i_Offset) + 1;
	}

	return size;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TEXT_SetTextID(TEXT_tdst_OneText *_pt_Text, int ID, char *pz)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEXT_tdst_Id	*pt_ID;
	TEXT_tdst_Id	*pt_ID1;
	CString			o;
	int				off, len, len1, len2;
	char			*pz_New;
	unsigned int	i;
	unsigned int	u32_SizeBuf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pt_Text->pst_Ids) return;

	pt_ID = _pt_Text->pst_Ids + ID;
	len = L_strlen(pz) + 1;

	if(!len)
	{
		pt_ID->i_Offset = -1;
		return;
	}

	u32_SizeBuf = TEXT_ui_GetTextLenght(_pt_Text);

	/* Pas encore de texte attribué ? */
	if(pt_ID->i_Offset == -1)
		len1 = 0;
	else
		len1 = L_strlen(_pt_Text->psz_Text + pt_ID->i_Offset) + 1;	/* Ancienne taille */

	/* Reconstruction */
	off = 0;
	pz_New = (char *) MEM_p_Alloc(u32_SizeBuf + len - len1);
	for(i = 0; i < _pt_Text->ul_Num; i++)
	{
		pt_ID1 = _pt_Text->pst_Ids + i;
		if(pt_ID1 == pt_ID)
		{
			L_memcpy(pz_New + off, pz, len);
			pt_ID1->i_Offset = off;
			off += len;
		}
		else if(pt_ID1->i_Offset != -1)
		{
			len2 = L_strlen(_pt_Text->psz_Text + pt_ID1->i_Offset) + 1;
			L_memcpy(pz_New + off, _pt_Text->psz_Text + pt_ID1->i_Offset, len2);
			pt_ID1->i_Offset = off;
			off += len2;
		}
	}

	/* Remplacement buffer */
	MEM_Free(_pt_Text->psz_Text);
	_pt_Text->psz_Text = pz_New;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG ETEXT_cl_Frame::GetSndFileAuto(char *pz)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		language;
	char	asz_DirPath[BIG_C_MaxLenPath];
	char	asz_FilePath[BIG_C_MaxLenPath];
	ULONG	ul;
	CString o;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pz) return BIG_C_InvalidKey;
	o = CString(pz);
	o.MakeLower();

	ul_GetCurFile(&language);
	if(language < 0) return BIG_C_InvalidKey;
	if(language >= INO_e_MaxLangNb) return BIG_C_InvalidKey;

	L_strcpy(asz_DirPath, EDI_Csz_Path_Audio);
	L_strcat(asz_DirPath, "/Dialog/");
	L_strcat(asz_DirPath, INO_dst_LanguageName[language].az_LongName);

	L_strcpy(asz_FilePath, INO_dst_LanguageName[language].az_ShortName);
	L_strcat(asz_FilePath, "_");
	L_strcat(asz_FilePath, LPCTSTR(o));
	ETEXT_FilterString(asz_FilePath);
	L_strcat(asz_FilePath, EDI_Csz_ExtSoundDialog);

	ul = BIG_ul_SearchFileExt(asz_DirPath, asz_FilePath);
	if(ul == BIG_C_InvalidIndex)
		ul = BIG_C_InvalidKey;
	else
		ul = BIG_FileKey(ul);

	return ul;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFileGetSndFileAuto(void)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	if(!mpt_EditText) return;

	for(i = 0; i < mpt_EditText->ul_Num; i++)
	{
		/* skip if there is yet one file */
		if(mpt_EditText->pst_Ids[i].ul_SoundKey != BIG_C_InvalidKey) continue;

		/* skip if any "<...>" */
		if
		(
			(mpt_EditText->pst_Ids[i].i_Offset != -1)
		&&	(mpt_EditText->psz_Text[mpt_EditText->pst_Ids[i].i_Offset] == '<')
		)
		{
			if(L_strcmp(mpt_EditText->psz_Text + mpt_EditText->pst_Ids[i].i_Offset, "<hole>") == 0) continue;
			if(L_strcmp(mpt_EditText->psz_Text + mpt_EditText->pst_Ids[i].i_Offset, "<deleted>") == 0) continue;
		}

		mpt_EditText->pst_Ids[i].ul_SoundKey = GetSndFileAuto(mpt_EditText->pst_Ids[i].asz_Name);
	}

	SetFileModified(TRUE);
	mpo_Scroll->Invalidate();
}

char	gazs_NameFilter[] = "HaAnJaDeJiNaEnPrLu";

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnFileGetPrefixAuto(BOOL b_OnlySelected)
{
	/*~~~~~~~~~~~~~~*/
	ULONG	i;
	char	*pz, *pz2;
	/*~~~~~~~~~~~~~~*/

	if(!mpt_EditText) return;

	for(i = 0; i < mpt_EditText->ul_Num; i++)
	{
		if(b_OnlySelected && !mpt_EditText->pst_Ids[i].sel) continue;
		if(mpt_EditText->pst_Ids[i].ul_ObjKey == BIG_C_InvalidKey) continue;
		if(mpt_EditText->pst_Ids[i].ul_ObjKey == 0) continue;

		if(mpt_EditText->pst_Ids[i].i_Offset != -1)
		{
			/* skip "<...>" entries */
			if(mpt_EditText->psz_Text[mpt_EditText->pst_Ids[i].i_Offset] == '<') continue;
		}

		/* skip bad named gao */
		pz = BIG_NameFile(BIG_ul_SearchKeyToFat(mpt_EditText->pst_Ids[i].ul_ObjKey));
		pz += 2;
		pz += 2;
		pz2 = gazs_NameFilter;
		while(*pz && *pz2)
		{
			if((pz[0] == pz2[0]) && (pz[1] == pz2[1])) break;

			/* pz += 2; */
			pz2 += 2;
		}

		if((!*pz) || !*pz2) continue;

		/* get prefix */
		pz = BIG_NameFile(BIG_ul_SearchKeyToFat(mpt_EditText->pst_Ids[i].ul_ObjKey));
		mpt_EditText->pst_Ids[i].asz_Name[0] = pz[4];
		mpt_EditText->pst_Ids[i].asz_Name[1] = pz[5];
		mpt_EditText->pst_Ids[i].asz_Name[2] = '_';
	}

	SetFileModified(TRUE);
	mpo_Scroll->Invalidate();
}

#ifdef JADEFUSION
extern ULONG	BIG_ul_GetRefInGroupAtRank(BIG_INDEX _ul_Group, int _i_Rank);
#else
extern "C" ULONG	BIG_ul_GetRefInGroupAtRank(BIG_INDEX _ul_Group, int _i_Rank);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */

void ETEXT_cl_Frame::OnCheckAndFixAll(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Index, ul, ul_From, ul_New, h_Dir;
	BAS_tdst_barray stArray;
	char			log[512], sz_Extrait[61];
	int				i;
	char			az_Path[BIG_C_MaxLenPath];
	char			az_Name[BIG_C_MaxLenName];
	char			az_BankName[BIG_C_MaxLenName];
	char			*sz_SaleChar;
	TEXT_tdst_OneText	*mpt_Text;
	int				b_PrintName, b_PrintEntry;
	char			sz_LetterAfterP16[] = { "0123456789:;<=1ABCDEFGHIJKLMNOPQRSTUabcdmnop" };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LINK_PrintStatusMsg("Check and fix all files...");
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    close all
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	OnCloseAll();

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    check language
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	if(mst_Ini.i_OptionFlags & ETEXT_Cte_UseStandardLang)
	{
		for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
		{
			if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
			if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;
			if(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtTextLang)) continue;

			/* get the file name + folder */
			ul = BIG_ul_GetRefInGroupAtRank(ul_Index, INO_e_French);
			ul = BIG_ul_SearchKeyToFat(ul);
			BIG_ComputeFullName(BIG_ParentFile(ul), az_Path);

			L_strcpy(az_BankName, BIG_NameFile(ul) + 3);
			az_BankName[L_strlen(az_BankName) - 4] = 0;

			for(i = 0; ETEXT_gaen_StandardLanguage[i] != INO_e_Dummy; i++)
			{
				/*~~~~~~*/
				int iLang;
				/*~~~~~~*/

				iLang = ETEXT_gaen_StandardLanguage[i];
				ul = BIG_ul_GetRefInGroupAtRank(ul_Index, iLang);
				if(BIG_ul_SearchKeyToPos(ul) != -1) continue;	/* language exist */

				/* filename */
				sprintf(az_Name, "%s_%s%s", INO_dst_LanguageName[iLang].az_ShortName, az_BankName, EDI_Csz_ExtTextFile);
				if(BIG_ul_SearchFileExt(az_Path, az_Name) != BIG_C_InvalidIndex)
				{
					ERR_X_Warning(0, "This text file already exists !", NULL);
					continue;
				}

				/* create txl file */
				h_Dir = BIG_ul_CreateFile(az_Path, az_Name);

				/* add file in the bank at the right rank */
				BIG_AddRefInGroupAtRank(ul_Index, BIG_NameFile(h_Dir), BIG_FileKey(h_Dir), iLang);
			}
		}
	}

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    get a safe config
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	mi_SynchroLang = INO_e_French;
	OnSynchronizeAll();
	mi_SynchroLang = INO_e_Dummy;
	GetEntryColumnInfo();
	SetEntryColumnInfo();

	/*$1
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	    open file
	 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 */

	BAS_binit(&stArray, 100);
	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;
		if(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtTextLang)) continue;

		OnBankOpen(ul_Index);

		for(ul = 0; ul < mpt_EditText->ul_Num; ul++)
		{
			if((mpt_EditText->pst_Ids[ul].ul_IdKey == 0) || (mpt_EditText->pst_Ids[ul].ul_IdKey == -1))
			{
				ul_New = BIG_ul_GetNewKey(BIG_C_InvalidKey);
				sprintf
				(
					log,
					"bad key : file %s, entry(%d) %s, key %x -> %x",
					BIG_NameFile(ul_Index),
					ul,
					mpt_EditText->pst_Ids[ul].asz_Name,
					mpt_EditText->pst_Ids[ul].ul_IdKey,
					ul_New
				);
				ERR_X_Warning(0, log, NULL);

				mpt_EditText->pst_Ids[ul].ul_IdKey = ul_New;
				mpo_Scroll->Invalidate();
				mb_FileIsModified = TRUE;
			}
			else
			{
				ul_From = BAS_bsearch(mpt_EditText->pst_Ids[ul].ul_IdKey, &stArray);

				if(ul_From != BIG_C_InvalidIndex)
				{
					ul_New = BIG_ul_GetNewKey(BIG_C_InvalidKey);
					sprintf
					(
						log,
						"key duplicated : file %s, entry(%d) %s, key %x -> %x",
						BIG_NameFile(ul_Index),
						ul,
						mpt_EditText->pst_Ids[ul].asz_Name,
						mpt_EditText->pst_Ids[ul].ul_IdKey,
						ul_New
					);
					ERR_X_Warning(0, log, NULL);

					mpt_EditText->pst_Ids[ul].ul_IdKey = ul_New;
					mpo_Scroll->Invalidate();
					mb_FileIsModified = TRUE;
				}
			}

			BAS_binsert(mpt_EditText->pst_Ids[ul].ul_IdKey, BIG_FileKey(ul_Index), &stArray);
		}

		OnBankClose();
	}

	BAS_bfree(&stArray);

	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		int i_Error;

		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;
		if(!BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtTextFile)) continue;

		*(ULONG *) &mpt_Text = TEXT_ul_CallbackLoadTxlEditor(BIG_PosFile(ul_Index), BIG_FileKey(ul_Index));
		b_PrintName = 0;

		for(ul = 0; ul < mpt_Text->ul_Num; ul++)
		{
			if( mpt_Text->pst_Ids[ul].i_Offset != -1 ) 
			{
				b_PrintEntry = 0;
				/*i_Error = 0;
				sz_SaleChar = mpt_Text->psz_Text + mpt_Text->pst_Ids[ul].i_Offset;
				while ( sz_SaleChar = strchr( sz_SaleChar, 'p' ) )
				{
					if ( (*(sz_SaleChar + 1) == '1') && (*(sz_SaleChar + 2) == '6') )
					{
						if ( sz_SaleChar == mpt_Text->psz_Text + mpt_Text->pst_Ids[ul].i_Offset )
						{
							i_Error = 1;
							break;
						}
						if ( *(sz_SaleChar - 1) != '\\' )
						{
							i_Error = 1;
							break;
						}
						if ( *(sz_SaleChar + 3) != '\\' )
						{
							i_Error = 1;
							break;
						}
						if ( !strchr( sz_LetterAfterP16, *(sz_SaleChar + 4)))
						{
							i_Error = 1;
							break;
						}
					}
					sz_SaleChar++;
				}
				if ( i_Error )
				{
					if ( !b_PrintName )
					{
						b_PrintName = 1;
						sprintf( log, "sale p16 found in %s", BIG_NameFile( ul_Index ) );
						LINK_PrintStatusMsg( log );
					}
					if ( !b_PrintEntry )
					{
						b_PrintEntry = 1;
						L_memcpy( sz_Extrait, mpt_Text->psz_Text + mpt_Text->pst_Ids[ul].i_Offset, 60 );
						sz_Extrait[ 60 ] = 0;
						sprintf( log, "....%08X : %s", mpt_Text->pst_Ids[ul].ul_IdKey, sz_Extrait );
						LINK_PrintStatusMsg( log );
					}
				}
				*/

				while ( sz_SaleChar = strchr( mpt_Text->psz_Text + mpt_Text->pst_Ids[ul].i_Offset, 0xA0 ) )
				{
					if ( !b_PrintName )
					{
						b_PrintName = 1;
						sprintf( log, "0x0A found in %s", BIG_NameFile( ul_Index ) );
						LINK_PrintStatusMsg( log );
					}
					if ( !b_PrintEntry )
					{
						b_PrintEntry = 1;
						L_memcpy( sz_Extrait, mpt_Text->psz_Text + mpt_Text->pst_Ids[ul].i_Offset, 60 );
						sz_Extrait[ 60 ] = 0;
						sprintf( log, "....%08X : %s", mpt_Text->pst_Ids[ul].ul_IdKey, sz_Extrait );
						LINK_PrintStatusMsg( log );
					}
					
					*sz_SaleChar = ' ';
					mb_FileIsModified = TRUE;
				}
			}
		}
		
		if (mb_FileIsModified)
		{
			SaveFile(mpt_Text);
			mb_FileIsModified = FALSE;
		}
		mpt_Text->ul_Inst = 1;
		TEXT_Free(mpt_Text);
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnReportSoundFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BAS_tdst_barray		st_WadFilesList;
	BAS_tdst_barray		st_TxgFilesList;
	ULONG				ul_Index, ul, ulFat;
	int					i, ii;
	ULONG				ul_val;
	char				log[512];
	TEXT_tdst_OneText	*pOneText;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	OnCloseAll();

	LINK_PrintStatusMsg("Searching all dialog files...");
	BAS_binit(&st_TxgFilesList, 100);
	BAS_binit(&st_WadFilesList, 100);

	for(ul_Index = 0; ul_Index < BIG_gst.st_ToSave.ul_MaxFile; ul_Index++)
	{
		if(BIG_FileChanged(ul_Index) == EDI_FHC_Deleted) continue;
		if(BIG_FileKey(ul_Index) == BIG_C_InvalidKey) continue;

		if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtTextLang))
			BAS_binsert(ul_Index, ul_Index, &st_TxgFilesList);
		else if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtSoundDialog))
			BAS_binsert(BIG_FileKey(ul_Index), ul_Index, &st_WadFilesList);
	}

	sprintf(log, "Parsing all %d txg files...", st_TxgFilesList.num);
	LINK_PrintStatusMsg(log);

	for(i = 0; i < st_TxgFilesList.num; i++)
	{
		ul_Index = st_TxgFilesList.base[i].ul_Key;
		OnBankOpen(ul_Index);

		for(ii = 0; ii < mpo_ListFile->GetCount(); ii++)
		{
			ulFat = mpo_ListFile->GetItemData(ii);
			if(ulFat == 0) continue;
			if(ulFat == BIG_C_InvalidIndex) continue;

			*(ULONG *) &pOneText = TEXT_ul_CallbackLoadTxlEditor(BIG_PosFile(ulFat), BIG_FileKey(ulFat));
			if(!pOneText) continue;

			for(ul = 0; ul < pOneText->ul_Num; ul++)
			{
				/* skip muted dialogs */
				if(pOneText->pst_Ids[ul].ul_SoundKey == 0) continue;

				/* skip no sounded dialogs */
				if(pOneText->pst_Ids[ul].ul_SoundKey == BIG_C_InvalidKey) continue;

				/* check wad is known */
				BAS_bdeletegetval(pOneText->pst_Ids[ul].ul_SoundKey, &st_WadFilesList, &ul_val);
				if(ul_val == -1)
				{
					ul_val = BIG_ul_SearchKeyToFat(pOneText->pst_Ids[ul].ul_SoundKey);
					sprintf
					(
						log,
						"text file [%x] is referencing a bad sound file [%x]",
						pOneText->ul_Key,
						pOneText->pst_Ids[ul].ul_SoundKey
					);
					ERR_X_Warning(BIG_b_IsFileExtension(ul_val, EDI_Csz_ExtSoundDialog), log, NULL);
				}
			}

			TEXT_Free(pOneText);
			pOneText = NULL;
		}

		OnBankClose();
	}

	for(i = 0; i < st_WadFilesList.num; i++)
	{
		sprintf
		(
			log,
			"unreferenced sound file [%x] %s",
			st_WadFilesList.base[i].ul_Key,
			BIG_NameFile(st_WadFilesList.base[i].ul_Val)
		);
		ERR_X_Warning(0, log, NULL);
	}

	BAS_bfree(&st_WadFilesList);
	BAS_bfree(&st_TxgFilesList);

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void ETEXT_cl_Frame::OnDisplayAllFilesLoaded(void)
{
	TEXT_tdst_OneText *pOneTxt;
	char log[256];
	ULONG ulFat;

	
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	for(int i=0; i<	TEXT_gst_Global.i_Num; i++)
	{
		pOneTxt = TEXT_gst_Global.pst_AllTexts[i];
		if(!pOneTxt) continue;

		ulFat = BIG_ul_SearchKeyToFat(pOneTxt->ul_Key);
		if(ulFat != BIG_C_InvalidIndex)
		{
			sprintf(log, "[TXT] [%08x] %s", pOneTxt->ul_Key, BIG_NameFile(ulFat));
			LINK_PrintStatusMsg(log);
		}
		else
		{
			sprintf(log, "[TXT] bad key [%08x]", pOneTxt->ul_Key);
			ERR_X_Warning(0, log, NULL);
		}
		
	}
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void ETEXT_cl_Frame::OnReportNoSound(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Index, ul, ulFat;
	int					i, ii;
	char				log[512];
	TEXT_tdst_OneText	*pOneText;
	BOOL				ab_Lang[INO_e_MaxLangNb];
	EDIA_cl_ListDialog	o_ListDlg( EDIA_List_SelectMultipleTxg);


	if(i_UserChooseLanguageList(ab_Lang) != IDOK) return;

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	LINK_PrintStatusMsg("Searching all text files...");
	if(o_ListDlg.DoModal() != IDOK) return;

	OnCloseAll();
	for(i = 0; i < o_ListDlg.mi_SelNb; i++)
	{
		ul_Index = *(ULONG*)&o_ListDlg.mpi_Sel[i];
		OnBankOpen(ul_Index);

		for(ii = 0; ii < mpo_ListFile->GetCount(); ii++)
		{
			// skip language
			if(ab_Lang[ii] == FALSE) continue;

			ulFat = mpo_ListFile->GetItemData(ii);
			if(ulFat == 0) continue;
			if(ulFat == BIG_C_InvalidIndex) continue;

			*(ULONG *) &pOneText = TEXT_ul_CallbackLoadTxlEditor(BIG_PosFile(ulFat), BIG_FileKey(ulFat));
			if(!pOneText) continue;

			for(ul = 0; ul < pOneText->ul_Num; ul++)
			{
				/* skip sounded dialogs */
				if(pOneText->pst_Ids[ul].ul_SoundKey != BIG_C_InvalidKey) continue;

				// skip empty text
				if(L_strlen(pOneText->pst_Ids[ul].i_Offset + pOneText->psz_Text) < 3) continue; 

				/* check if there is any text */
				if(L_strcmp(pOneText->pst_Ids[ul].i_Offset + pOneText->psz_Text, "<todo>") == 0) continue;
				if(L_strcmp(pOneText->pst_Ids[ul].i_Offset + pOneText->psz_Text, "<hole>") == 0) continue;
				if(L_strcmp(pOneText->pst_Ids[ul].i_Offset + pOneText->psz_Text, "<deleted>") == 0) continue;

				sprintf
				(
					log,
					"text [%08x] %s, entry(%d) %08x %s has no sound",
					BIG_FileKey(ulFat),
					BIG_NameFile(ulFat),
					ul,
					pOneText->pst_Ids[ul].ul_IdKey,
					pOneText->pst_Ids[ul].asz_Name
				);
				LINK_PrintStatusMsg(log);
			}

			TEXT_Free(pOneText);
			pOneText = NULL;
		}

		OnBankClose();
	}


	LINK_PrintStatusMsg("done.");
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif /* ACTIVE_EDITORS */
