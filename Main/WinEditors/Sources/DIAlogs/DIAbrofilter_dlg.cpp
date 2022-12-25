/*$T DIAbrofilter_dlg.cpp GC 1.138 12/20/04 15:30:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbrofilter_dlg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "Res/Res.h"
#include "VAVview/VAVlist.h"
#include "LINks/LINKtoed.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "EDItors/Sources/BROwser/BROmsg.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern ULONG					BRO_ul_LoadFilter(char *_asz_FilterFileName);
extern void						BRO_UnloadFilter(void);

extern CList<CString, CString>	EDI_go_ForceDirList;
extern BAS_tdst_barray			EDI_gst_ForceIdKeyList;
extern BAS_tdst_barray			EDI_gst_ForceFileKeyList;

extern CList<CString, CString>	EDI_go_ExcludeDirList;
extern BAS_tdst_barray			EDI_gst_ExcludeIdKeyList;
extern BAS_tdst_barray			EDI_gst_ExcludeFileKeyList;

extern BOOL						EDI_gb_FIM_Mirror;
extern BOOL						EDI_gb_FIM_TestDate;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_BroFilterDialog, EDIA_cl_BaseDialog)
	ON_COMMAND(IDOK, OnOk)
	ON_COMMAND(IDC_BTN_ADDEXID, OnAddExId)
	ON_COMMAND(IDC_BTN_ADDEXFILE, OnAddExFile)
	ON_COMMAND(IDC_BTN_ADDEXDIR, OnAddExDir)
	ON_COMMAND(IDC_BTN_ADDFOID, OnAddFoId)
	ON_COMMAND(IDC_BTN_ADDFOFILE, OnAddFoFile)
	ON_COMMAND(IDC_BTN_ADDFODIR, OnAddFoDir)
	ON_COMMAND(IDC_CK_MIRROR, OnMirror)
	ON_COMMAND(IDC_CK_TESTDATE, OnTestDate)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_BroFilterDialog::EDIA_cl_BroFilterDialog(char *_asz_FilterName) :
	EDIA_cl_BaseDialog(EBRO_IDD_FILTER)
{
	mul_Index = BRO_ul_LoadFilter(_asz_FilterName);
	mo_FileName = CString(_asz_FilterName);
	mb_Modified = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_BroFilterDialog::~EDIA_cl_BroFilterDialog(void)
{
	BRO_UnloadFilter();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_BroFilterDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~~*/
	CString o_Title;
	/*~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnInitDialog();
	o_Title = CString("Merge filter - ") + mo_FileName;
	SetWindowText(o_Title);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::DoDataExchange(CDataExchange *pDX)
{
	LINK_gb_CanRefresh = FALSE;

	/*~~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_List;
	/*~~~~~~~~~~~~~~~~~*/

	CDialog::DoDataExchange(pDX);

	if(pDX->m_bSaveAndValidate)
	{
		EDI_gb_FIM_TestDate = ((CButton *) GetDlgItem(IDC_CK_TESTDATE))->GetCheck() ? TRUE : FALSE;
		EDI_gb_FIM_Mirror = ((CButton *) GetDlgItem(IDC_CK_MIRROR))->GetCheck() ? TRUE : FALSE;
	}
	else
	{
		po_List = (CListCtrl *) GetDlgItem(IDC_LST_EXFILE);
		po_List->SetExtendedStyle(LVS_EX_FULLROWSELECT);
		po_List->InsertColumn(0, "Name", LVCFMT_LEFT, 150);
		po_List->InsertColumn(1, "Path", LVCFMT_LEFT, 300);
		po_List->InsertColumn(2, "Key", LVCFMT_LEFT, 80);
		po_List = (CListCtrl *) GetDlgItem(IDC_LST_FOFILE);
		po_List->SetExtendedStyle(LVS_EX_FULLROWSELECT);
		po_List->InsertColumn(0, "Name", LVCFMT_LEFT, 150);
		po_List->InsertColumn(1, "Path", LVCFMT_LEFT, 300);
		po_List->InsertColumn(2, "Key", LVCFMT_LEFT, 80);

		po_List = (CListCtrl *) GetDlgItem(IDC_LST_EXDIR);
		po_List->SetExtendedStyle(LVS_EX_FULLROWSELECT);
		po_List->InsertColumn(0, "Name", LVCFMT_LEFT, 500);
		po_List = (CListCtrl *) GetDlgItem(IDC_LST_FODIR);
		po_List->SetExtendedStyle(LVS_EX_FULLROWSELECT);
		po_List->InsertColumn(0, "Name", LVCFMT_LEFT, 500);

        /* var -> dialog */
		RedrawIdList(IDC_LST_EXID, &EDI_gst_ExcludeIdKeyList);
		RedrawFileList(IDC_LST_EXFILE, &EDI_gst_ExcludeFileKeyList);
		RedrawDirList(IDC_LST_EXDIR, &EDI_go_ExcludeDirList);
/**/
		RedrawIdList(IDC_LST_FOID, &EDI_gst_ForceIdKeyList);
		RedrawFileList(IDC_LST_FOFILE, &EDI_gst_ForceFileKeyList);
		RedrawDirList(IDC_LST_FODIR, &EDI_go_ForceDirList);
		((CButton *) GetDlgItem(IDC_CK_TESTDATE))->SetCheck(EDI_gb_FIM_TestDate ? 1 : 0);
		((CButton *) GetDlgItem(IDC_CK_MIRROR))->SetCheck(EDI_gb_FIM_Mirror ? 1 : 0);
	}

	LINK_gb_CanRefresh = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::SaveFilter(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_File[BIG_C_MaxLenPath];
	char		*pz;
	int			i;
	POSITION	pos;
	CString		o_Dir;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(mul_Index == BIG_C_InvalidIndex)
	{
		L_strcpy(asz_Path, LPCTSTR(mo_FileName));
		pz = L_strrchr(asz_Path, '/');

		L_strcpy(asz_File, pz + 1);
		*pz = 0;

		mul_Index = BIG_ul_CreateFile(asz_Path, asz_File);
	}

	BIG_ComputeFullName(BIG_ParentFile(mul_Index), asz_Path);
	SAV_Begin(asz_Path, BIG_NameFile(mul_Index));

	if(EDI_gb_FIM_TestDate)
		sprintf(asz_Path, "o:testdate-on\n");
	else
		sprintf(asz_Path, "o:testdate-off\n");
	SAV_Buffer(asz_Path, L_strlen(asz_Path));

	if(EDI_gb_FIM_Mirror)
		sprintf(asz_Path, "o:mirror-on\n");
	else
		sprintf(asz_Path, "o:mirror-off\n");
	SAV_Buffer(asz_Path, L_strlen(asz_Path));

	for(i = 0; i < EDI_gst_ForceIdKeyList.num; i++)
	{
		if(EDI_gst_ForceIdKeyList.base[i].ul_Key == 0) continue;
		if(EDI_gst_ForceIdKeyList.base[i].ul_Key == -1) continue;

		sprintf(asz_Path, "fn:%x\n", EDI_gst_ForceIdKeyList.base[i].ul_Key);
		SAV_Buffer(asz_Path, L_strlen(asz_Path));
	}

	for(i = 0; i < EDI_gst_ForceFileKeyList.num; i++)
	{
		if(EDI_gst_ForceFileKeyList.base[i].ul_Key == 0) continue;
		if(EDI_gst_ForceFileKeyList.base[i].ul_Key == -1) continue;

		sprintf(asz_Path, "ff:%x\n", EDI_gst_ForceFileKeyList.base[i].ul_Key);
		SAV_Buffer(asz_Path, L_strlen(asz_Path));
	}

	for(pos = EDI_go_ForceDirList.GetHeadPosition(); pos;)
	{
		o_Dir = EDI_go_ForceDirList.GetNext(pos);

		sprintf(asz_Path, "fd:%s\n", LPCTSTR(o_Dir));
		SAV_Buffer(asz_Path, L_strlen(asz_Path));
	}

	for(i = 0; i < EDI_gst_ExcludeIdKeyList.num; i++)
	{
		if(EDI_gst_ExcludeIdKeyList.base[i].ul_Key == 0) continue;
		if(EDI_gst_ExcludeIdKeyList.base[i].ul_Key == -1) continue;

		sprintf(asz_Path, "en:%x\n", EDI_gst_ExcludeIdKeyList.base[i].ul_Key);
		SAV_Buffer(asz_Path, L_strlen(asz_Path));
	}

	for(i = 0; i < EDI_gst_ExcludeFileKeyList.num; i++)
	{
		if(EDI_gst_ExcludeFileKeyList.base[i].ul_Key == 0) continue;
		if(EDI_gst_ExcludeFileKeyList.base[i].ul_Key == -1) continue;

		sprintf(asz_Path, "ef:%x\n", EDI_gst_ExcludeFileKeyList.base[i].ul_Key);
		SAV_Buffer(asz_Path, L_strlen(asz_Path));
	}

	for(pos = EDI_go_ExcludeDirList.GetHeadPosition(); pos;)
	{
		o_Dir = EDI_go_ExcludeDirList.GetNext(pos);

		sprintf(asz_Path, "ed:%s\n", LPCTSTR(o_Dir));
		SAV_Buffer(asz_Path, L_strlen(asz_Path));
	}

	SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnOk(void)
{
	if(mb_Modified) SaveFilter();
	EDIA_cl_BaseDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnAddExId(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Num[3];
	ULONG	ul_Id;
	CEdit	*po_Edit = (CEdit *) GetDlgItem(IDC_ED_EXID);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Edit->GetWindowText(asz_Num, 3);
	sscanf(asz_Num, "%x", &ul_Id);
	ul_Id <<= 24;

	mb_Modified = TRUE;
	BAS_binsert(ul_Id, ul_Id, &EDI_gst_ExcludeIdKeyList);
	RedrawIdList(IDC_LST_EXID, &EDI_gst_ExcludeIdKeyList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnAddFoId(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Num[3];
	ULONG	ul_Id;
	CEdit	*po_Edit = (CEdit *) GetDlgItem(IDC_ED_FOID);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Edit->GetWindowText(asz_Num, 3);
	sscanf(asz_Num, "%x", &ul_Id);
	ul_Id <<= 24;

	mb_Modified = TRUE;
	BAS_binsert(ul_Id, ul_Id, &EDI_gst_ForceIdKeyList);
	RedrawIdList(IDC_LST_FOID, &EDI_gst_ForceIdKeyList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnDelExId(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_List = (CListCtrl *) GetDlgItem(IDC_LST_EXID);
	int			i;
	int			nItem = -1;
	ULONG		ul_Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(po_List->GetSelectedCount() <= 0) return;

	for(i = 0; i < (int) po_List->GetSelectedCount(); i++)
	{
		nItem = po_List->GetNextItem(nItem, LVNI_SELECTED);
		ul_Id = po_List->GetItemData(nItem);
		BAS_bdelete(ul_Id, &EDI_gst_ExcludeIdKeyList);
	}

	mb_Modified = TRUE;
	RedrawIdList(IDC_LST_EXID, &EDI_gst_ExcludeIdKeyList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnDelFoId(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_List = (CListCtrl *) GetDlgItem(IDC_LST_FOID);
	int			i;
	int			nItem = -1;
	ULONG		ul_Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(po_List->GetSelectedCount() <= 0) return;

	for(i = 0; i < (int) po_List->GetSelectedCount(); i++)
	{
		nItem = po_List->GetNextItem(nItem, LVNI_SELECTED);
		ul_Id = po_List->GetItemData(nItem);
		BAS_bdelete(ul_Id, &EDI_gst_ForceIdKeyList);
	}

	mb_Modified = TRUE;
	RedrawIdList(IDC_LST_FOID, &EDI_gst_ForceIdKeyList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnAddExFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File1("Choose files", 3, FALSE, TRUE);
	CString				o_Temp;
	ULONG				ul_Index, ul_DirIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File1.mb_UseFavorite = TRUE;

	if(o_File1.DoModal() == IDOK)
	{
		if(o_File1.mi_NumFiles == 0) return;

		ul_DirIndex = BIG_ul_SearchDir(o_File1.masz_FullPath);
		if(ul_DirIndex == BIG_C_InvalidIndex) return;

		for(int i_Num = 0; i_Num < o_File1.mi_NumFiles; i_Num++)
		{
			o_File1.GetItem(o_File1.mo_File, i_Num + 1, o_Temp);
			ul_Index = BIG_ul_SearchFile(ul_DirIndex, LPCTSTR(o_Temp));
			if(ul_Index == BIG_C_InvalidIndex) continue;

			BAS_binsert(BIG_FileKey(ul_Index), BIG_FileKey(ul_Index), &EDI_gst_ExcludeFileKeyList);
		}
	}

	mb_Modified = TRUE;
	RedrawFileList(IDC_LST_EXFILE, &EDI_gst_ExcludeFileKeyList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnAddFoFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File1("Choose files", 3, FALSE, TRUE);
	CString				o_Temp;
	ULONG				ul_Index, ul_DirIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File1.mb_UseFavorite = TRUE;

	if(o_File1.DoModal() == IDOK)
	{
		if(o_File1.mi_NumFiles == 0) return;

		ul_DirIndex = BIG_ul_SearchDir(o_File1.masz_FullPath);
		if(ul_DirIndex == BIG_C_InvalidIndex) return;

		for(int i_Num = 0; i_Num < o_File1.mi_NumFiles; i_Num++)
		{
			o_File1.GetItem(o_File1.mo_File, i_Num + 1, o_Temp);
			ul_Index = BIG_ul_SearchFile(ul_DirIndex, LPCTSTR(o_Temp));
			if(ul_Index == BIG_C_InvalidIndex) continue;

			BAS_binsert(BIG_FileKey(ul_Index), BIG_FileKey(ul_Index), &EDI_gst_ForceFileKeyList);
		}
	}

	mb_Modified = TRUE;
	RedrawFileList(IDC_LST_FOFILE, &EDI_gst_ForceFileKeyList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnDelExFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_List = (CListCtrl *) GetDlgItem(IDC_LST_EXFILE);
	int			i;
	int			nItem = -1;
	ULONG		ul_Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(po_List->GetSelectedCount() <= 0) return;

	for(i = 0; i < (int) po_List->GetSelectedCount(); i++)
	{
		nItem = po_List->GetNextItem(nItem, LVNI_SELECTED);
		ul_Id = po_List->GetItemData(nItem);
		BAS_bdelete(ul_Id, &EDI_gst_ExcludeFileKeyList);
	}

	mb_Modified = TRUE;
	RedrawFileList(IDC_LST_EXFILE, &EDI_gst_ExcludeFileKeyList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnDelFoFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_List = (CListCtrl *) GetDlgItem(IDC_LST_FOFILE);
	int			i;
	int			nItem = -1;
	ULONG		ul_Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(po_List->GetSelectedCount() <= 0) return;

	for(i = 0; i < (int) po_List->GetSelectedCount(); i++)
	{
		nItem = po_List->GetNextItem(nItem, LVNI_SELECTED);
		ul_Id = po_List->GetItemData(nItem);
		BAS_bdelete(ul_Id, &EDI_gst_ForceFileKeyList);
	}

	mb_Modified = TRUE;
	RedrawFileList(IDC_LST_FOFILE, &EDI_gst_ForceFileKeyList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnAddExDir(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File1("Choose directories", 3, TRUE, TRUE);
	CString				o_Temp;
	ULONG				ul_Index, ul_DirIndex;
	POSITION			pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File1.mb_UseFavorite = TRUE;

	if(o_File1.DoModal() == IDOK)
	{
		if(o_File1.mi_NumPaths == 0) return;

		ul_DirIndex = BIG_ul_SearchDir(o_File1.masz_FullPath);
		if(ul_DirIndex == BIG_C_InvalidIndex) return;

		for(int i_Num = 0; i_Num < o_File1.mi_NumPaths; i_Num++)
		{
			o_File1.GetItem(o_File1.mo_Path, i_Num + 1, o_Temp);
			o_Temp = CString(o_File1.masz_FullPath) + CString("/") + o_Temp;

			ul_Index = BIG_ul_SearchDir(LPCTSTR(o_Temp));
			if(ul_Index == BIG_C_InvalidIndex) continue;

			pos = EDI_go_ExcludeDirList.Find(o_Temp);
			if(!pos)
			{
				EDI_go_ExcludeDirList.AddHead(o_Temp);
			}
		}
	}

	mb_Modified = TRUE;
	RedrawDirList(IDC_LST_EXDIR, &EDI_go_ExcludeDirList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnMirror(void)
{
	EDI_gb_FIM_Mirror = !EDI_gb_FIM_Mirror;
	((CButton *) GetDlgItem(IDC_CK_MIRROR))->SetCheck(EDI_gb_FIM_Mirror ? 1 : 0);
	mb_Modified = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnTestDate(void)
{
	EDI_gb_FIM_TestDate = !EDI_gb_FIM_TestDate;
	((CButton *) GetDlgItem(IDC_CK_TESTDATE))->SetCheck(EDI_gb_FIM_TestDate ? 1 : 0);
	mb_Modified = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnAddFoDir(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_FileDialog	o_File1("Choose directories", 3, TRUE, TRUE);
	CString				o_Temp;
	ULONG				ul_Index, ul_DirIndex;
	POSITION			pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_File1.mb_UseFavorite = TRUE;
	if(o_File1.DoModal() == IDOK)
	{
		if(o_File1.mi_NumPaths == 0) return;

		ul_DirIndex = BIG_ul_SearchDir(o_File1.masz_FullPath);
		if(ul_DirIndex == BIG_C_InvalidIndex) return;

		for(int i_Num = 0; i_Num < o_File1.mi_NumPaths; i_Num++)
		{
			o_File1.GetItem(o_File1.mo_Path, i_Num + 1, o_Temp);
			o_Temp = CString(o_File1.masz_FullPath) + CString("/") + o_Temp;

			ul_Index = BIG_ul_SearchDir(LPCTSTR(o_Temp));
			if(ul_Index == BIG_C_InvalidIndex) continue;

			pos = EDI_go_ForceDirList.Find(o_Temp);
			if(!pos)
			{
				EDI_go_ForceDirList.AddHead(o_Temp);
			}
		}
	}

	mb_Modified = TRUE;
	RedrawDirList(IDC_LST_FODIR, &EDI_go_ForceDirList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnDelExDir(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_List = (CListCtrl *) GetDlgItem(IDC_LST_EXDIR);
	int			i;
	int			nItem = -1;
	POSITION	pos;
    CString     o_path;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(po_List->GetSelectedCount() <= 0) return;

	for(i = 0; i < (int) po_List->GetSelectedCount(); i++)
	{
		nItem = po_List->GetNextItem(nItem, LVNI_SELECTED);

        o_path = po_List->GetItemText(nItem, 0);
        pos = EDI_go_ForceDirList.Find(o_path);
		if(pos)
			EDI_go_ExcludeDirList.RemoveAt(pos);
        
        po_List->DeleteItem(nItem);
	}
    
	mb_Modified = TRUE;
	RedrawDirList(IDC_LST_EXDIR, &EDI_go_ExcludeDirList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::OnDelFoDir(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl	*po_List = (CListCtrl *) GetDlgItem(IDC_LST_FODIR);
	int			i;
	int			nItem = -1;
	POSITION	pos;
    CString     o_path;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(po_List->GetSelectedCount() <= 0) return;

	for(i = 0; i < (int) po_List->GetSelectedCount(); i++)
	{
		nItem = po_List->GetNextItem(nItem, LVNI_SELECTED);

        o_path = po_List->GetItemText(nItem, 0);
        pos = EDI_go_ForceDirList.Find(o_path);
		if(pos)
			EDI_go_ForceDirList.RemoveAt(pos);
        
        po_List->DeleteItem(nItem);
	}

	mb_Modified = TRUE;
	RedrawDirList(IDC_LST_FODIR, &EDI_go_ForceDirList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::RedrawIdList(unsigned int uiID, BAS_tdst_barray *pst_Array)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			line, j, i;
	char		asz_Line[512];
	CListCtrl	*po_List = (CListCtrl *) GetDlgItem(uiID);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	line = j = 0;

	po_List->SetRedraw(FALSE);
	po_List->DeleteAllItems();
	for(i = 0; i < pst_Array->num; i++)
	{
		sprintf(asz_Line, "%x", pst_Array->base[i].ul_Key);
		line = po_List->InsertItem(j++, asz_Line, 0);
		po_List->SetItemData(line, (DWORD_PTR) pst_Array->base[i].ul_Key);
	}

	po_List->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::RedrawFileList(unsigned int uiID, BAS_tdst_barray *pst_Array)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			line, j, i;
	char		asz_Line[512];
	ULONG		ul_Index;
	CListCtrl	*po_List = (CListCtrl *) GetDlgItem(uiID);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	line = j = 0;
	po_List->SetRedraw(FALSE);
	po_List->DeleteAllItems();
	for(i = 0; i < pst_Array->num; i++)
	{
		ul_Index = BIG_ul_SearchKeyToFat(pst_Array->base[i].ul_Key);
		if(ul_Index == BIG_C_InvalidIndex) continue;

		line = po_List->InsertItem(j++, BIG_NameFile(ul_Index), 0);
		po_List->SetItemData(line, (DWORD_PTR) pst_Array->base[i].ul_Key);

		BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Line);
		po_List->SetItem(line, 1, LVIF_TEXT, asz_Line, 0, 0, 0, 0);

		sprintf(asz_Line, "%x", pst_Array->base[i].ul_Key);
		po_List->SetItem(line, 2, LVIF_TEXT, asz_Line, 0, 0, 0, 0);
	}

	po_List->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_BroFilterDialog::RedrawDirList(unsigned int uiID, CList<CString, CString> *po_CList)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int			line, j;
	ULONG		ul_Index;
	POSITION	pos;
	CListCtrl	*po_List = (CListCtrl *) GetDlgItem(uiID);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	line = j = 0;

	po_List->SetRedraw(FALSE);
	po_List->DeleteAllItems();
	for(pos = po_CList->GetHeadPosition(); pos;)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		CString o_Dir = po_CList->GetNext(pos);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_Index = BIG_ul_SearchDir(LPCTSTR(o_Dir));

		line = po_List->InsertItem(j++, LPCTSTR(o_Dir), 0);
		po_List->SetItemData(line, (DWORD_PTR) ul_Index);
	}

	po_List->SetRedraw(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_BroFilterDialog::PreTranslateMessage(MSG *pMsg)
{
	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_DELETE))
	{
		if(pMsg->hwnd == GetDlgItem(IDC_LST_EXID)->GetSafeHwnd())
		{
			OnDelExId();
			return 1;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_LST_EXFILE)->GetSafeHwnd())
		{
			OnDelExFile();
			return 1;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_LST_EXDIR)->GetSafeHwnd())
		{
			OnDelExDir();
			return 1;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_LST_FOID)->GetSafeHwnd())
		{
			OnDelFoId();
			return 1;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_LST_FOFILE)->GetSafeHwnd())
		{
			OnDelFoFile();
			return 1;
		}
		else if(pMsg->hwnd == GetDlgItem(IDC_LST_FODIR)->GetSafeHwnd())
		{
			OnDelFoDir();
			return 1;
		}
	}

	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}
#endif /* ACTIVE_EDITORS */
