/*$T DIAfile_dlg.cpp GC!1.71 01/25/00 13:03:14 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "EDIpaths.h"
#include "DIAstrings.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGopen.h"

#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDItors/Sources/BROwser/BROframe.h"
#include "EDIeditors_infos.h"

/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

#define MAX_FULL    20
#define SEP         '/'
static char sgasz_PreviousPath[L_MAX_PATH] = { 0 };
static char sgasz_PreviousPath1[MAX_FULL][L_MAX_PATH];
static int  sgi_PreviousPath = 0;
static BOOL sgb_SortByName = TRUE;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_FileDialog, EDIA_cl_BaseDialog)
    ON_COMMAND(IDC_RADIONAME, OnSortName)
    ON_COMMAND(IDC_RADIOTYPE, OnSortType)
    ON_COMMAND(IDC_BUTTON1, OnGoToParent)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnListCtrlSelChange)
    ON_LBN_SELCHANGE(IDC_LIST2, OnDriveSelChange)
    ON_WM_LBUTTONDBLCLK()
    ON_WM_ERASEBKGND()
    ON_CBN_EDITUPDATE(IDC_EDIT_FULLPATH, OnEditChange)
    ON_CBN_CLOSEUP(IDC_EDIT_FULLPATH, OnSelCombo)
	ON_EN_KILLFOCUS(IDC_EDIT_FILTER, OnFilter)
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_FileDialog::EDIA_cl_FileDialog
(
    char    *_psz_Title,
    int     _i_ForPath,
    BOOL    _b_EnableDir,
    BOOL    _b_ForBig,
    char    *_psz_InitPath,
    char    *_psz_Filter
) :
    EDIA_cl_BaseDialog(DIALOGS_IDD_FILE)
{
    mi_ForPath = _i_ForPath;
    mb_EnableDir = _b_EnableDir;
    mb_ForBig = _b_ForBig;
    mo_Title = _psz_Title;
    mpsz_Filter = _psz_Filter;
	*masz_Filter = 0;
    mb_UseFavorite = FALSE;

    /* For a bigfile, force first path to be null */
    if(_b_ForBig)
    {
        if(_psz_InitPath)
            L_strcpy(masz_FullPath, _psz_InitPath);
        else if(*sgasz_PreviousPath && BIG_ul_SearchDir(sgasz_PreviousPath) != BIG_C_InvalidIndex)
            L_strcpy(masz_FullPath, sgasz_PreviousPath);
        else
            L_strcpy(masz_FullPath, BIG_Csz_Root);
        return;
    }
    else
    {
        /* Save current directory */
        GetCurrentDirectory(L_MAX_PATH, masz_FirstPath);

        if ( (_psz_InitPath) && SetCurrentDirectory( _psz_InitPath ) )
        {
            L_strcpy(masz_FullPath, _psz_InitPath);
        }
        else
        {
            /* Set to previous one */
            if(*sgasz_PreviousPath)
            {
                SetCurrentDirectory(sgasz_PreviousPath);
                L_strcpy(masz_FullPath, sgasz_PreviousPath);
            }
            else
            {
                GetCurrentDirectory(L_MAX_PATH, masz_FullPath);
            }
        }
    }
}

/*$4
 ***********************************************************************************************************************
    OVERWRITE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_FileDialog::OnInitDialog(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl   *po_List;
    CListBox    *po_ListBox;
    char        asz_Path[L_MAX_PATH];
    ITEMIDLIST  *idlist;
    int         i;
    long        l_Return;
    SHFILEINFO  st_FileInfo;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    EDIA_cl_BaseDialog::OnInitDialog();

    /* Set title of window */
    SetWindowText(mo_Title);

    /* Set buttons bitmaps */
    ((CButton *) GetDlgItem(IDC_BUTTON1))->SetIcon(AfxGetApp()->LoadIcon(EDIA_IDI_FILE_GOPARENT));

    /* Attach the system image list to list ctrl */
    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));
    l_Return = SHGetFileInfo("*.*", FILE_ATTRIBUTE_NORMAL, &st_FileInfo, sizeof(SHFILEINFO), SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
    mo_ImageList.Attach((HIMAGELIST) l_Return);
    po_List->SetImageList(&mo_ImageList, LVSIL_SMALL);

    /* File list with drives */
    if(mb_ForBig == FALSE)
    {
        L_strcpy(asz_Path, "*.*");
        DlgDirList(asz_Path, IDC_LIST2, 0, DDL_DRIVES);

        /* Mark the normal folders with data to 0 */
        po_ListBox = (CListBox *) GetDlgItem(IDC_LIST2);
        for(i = 0; i < po_ListBox->GetCount(); i++)
            po_ListBox->SetItemData(i, 0);

        /* Desktop folder */
        if(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &idlist) == NOERROR)
        {
            if(SHGetPathFromIDList(idlist, asz_Path))
            {
                i = po_ListBox->AddString("Desktop");
                po_ListBox->SetItemData(i, 1);
            }
        }

        /* Network */
        if(SHGetSpecialFolderLocation(NULL, CSIDL_NETWORK, &idlist) == NOERROR)
        {
            i = po_ListBox->AddString("Network");
            po_ListBox->SetItemData(i, 2);
        }
    }

    /* Invalide depending on browse for file or dir */
    if(mi_ForPath >= 2)
    {
        if(mi_ForPath == 2) 
		{
			((CEdit *) GetDlgItem(IDC_EDIT_FILE))->EnableWindow(FALSE);
			((CEdit *) GetDlgItem(IDC_EDIT_PATH))->SetFocus();
		}
        if(mb_EnableDir == FALSE) 
		{
			((CEdit *) GetDlgItem(IDC_EDIT_PATH))->EnableWindow(FALSE);
			((CEdit *) GetDlgItem(IDC_EDIT_FILE))->SetFocus();
		}
    }
    else
    {
        ((CEdit *) GetDlgItem(IDC_EDIT_PATH))->EnableWindow(FALSE);
        ((CEdit *) GetDlgItem(IDC_EDIT_FILE))->SetFocus();
    }

    /* Init path editbox with actual path */
    ((CEdit *) GetDlgItem(IDC_EDIT_FULLPATH))->SetWindowText(masz_FullPath);

    /* All combo values */
    for(i = 0; i < sgi_PreviousPath; i++) ((CComboBox *) GetDlgItem(IDC_EDIT_FULLPATH))->AddString(sgasz_PreviousPath1[i]);
    
    if(mb_UseFavorite)
    {
        EBRO_cl_Frame	*po_Browser;
        po_Browser = (EBRO_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_BROWSER, 0);
        for(i = 0; i < po_Browser->mst_Ini.i_NumFavorites; i++)
        {
	        if(po_Browser->mst_Ini.ast_Favorites[i].asz_DisplayName[0] != '\0')
		    {
                ((CComboBox *) GetDlgItem(IDC_EDIT_FULLPATH))->AddString(po_Browser->mst_Ini.ast_Favorites[i].asz_PathName);
    			
		    }
	    }
    }


    /* No file name */
    mo_File.Empty();
    if(!mpsz_Filter)
        mo_Filter = "*.*";
    else
        mo_Filter = mpsz_Filter;

    /* Fill list ctrl with actual path */
    FillList();

    /* No path */
    ((CEdit *) GetDlgItem(IDC_EDIT_PATH))->SetWindowText("");
    mi_NumPaths = 0;

    /* No file */
    ((CEdit *) GetDlgItem(IDC_EDIT_FILE))->SetWindowText("");
    mi_NumFiles = 0;

    /* Sort type */
    if(sgb_SortByName)
    {
        ((CButton *) GetDlgItem(IDC_RADIONAME))->SetCheck(1);
        ((CButton *) GetDlgItem(IDC_RADIOTYPE))->SetCheck(0);
    }
    else
    {
        ((CButton *) GetDlgItem(IDC_RADIONAME))->SetCheck(0);
        ((CButton *) GetDlgItem(IDC_RADIOTYPE))->SetCheck(1);
    }

    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnSelCombo(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    CComboBox   *po_Combo;
    /*~~~~~~~~~~~~~~~~~~~~~~*/

    po_Combo = (CComboBox *) GetDlgItem(IDC_EDIT_FULLPATH);
    if(po_Combo->GetCurSel() == -1) return;
    po_Combo->GetLBText(po_Combo->GetCurSel(), masz_FullPath);
    FillList();
    ((CListBox *) GetDlgItem(IDC_LIST2))->SetCurSel(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnEditChange(void)
{
    GetDlgItem(IDC_EDIT_FULLPATH)->GetWindowText(masz_FullPath, L_MAX_PATH);
    FillList();
    ((CListBox *) GetDlgItem(IDC_LIST2))->SetCurSel(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_FileDialog::PreTranslateMessage(MSG *pMsg)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl   *po_List;
    char        *psz_Temp;
    int         i_Res;
    CString     o_String;
    /*~~~~~~~~~~~~~~~~~~~~~~*/

    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));
    if((po_List) && (pMsg->hwnd == po_List->m_hWnd))
    {
        if(pMsg->message == WM_LBUTTONDBLCLK)
        {
            OnLButtonDblClk(pMsg->wParam, CPoint(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)));
            return TRUE;
        }
    }

    /*
     * If return is pressed in full path edit box, change the current path to that.
     */
    if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
    {
        if(pMsg->hwnd == GetDlgItem(IDC_EDIT_FULLPATH)->m_hWnd)
        {
            GetDlgItem(IDC_EDIT_FULLPATH)->GetWindowText(masz_FullPath, L_MAX_PATH);
            FillList();
            ((CListBox *) GetDlgItem(IDC_LIST2))->SetCurSel(-1);
            return TRUE;
        }

        if(pMsg->hwnd == GetDlgItem(IDC_LIST1)->m_hWnd)
        {
            po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));
            i_Res = po_List->GetNextItem(-1, LVNI_SELECTED);
            if(i_Res != -1)
            {
                o_String = po_List->GetItemText(i_Res, 0);
                if(((tdst_FileDes *) po_List->GetItemData(i_Res))->b_Dir)
                {
                    SelectDir(o_String);
                    return TRUE;
                }
            }
        }

        if(pMsg->hwnd == GetDlgItem(IDC_EDIT_FILTER)->m_hWnd)
        {
			GetDlgItem(IDC_LIST1)->SetFocus();
			return TRUE;
        }
    }

    /*
     * If return is pressed in full path edit box, change the current path to that.
     */
    if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_BACK))
    {
        if(pMsg->hwnd == GetDlgItem(IDC_LIST1)->m_hWnd)
        {
            GetDlgItem(IDC_EDIT_FULLPATH)->GetWindowText(masz_FullPath, L_MAX_PATH);
            psz_Temp = L_strrchr(masz_FullPath, '/');
            if(!psz_Temp) psz_Temp = L_strrchr(masz_FullPath, '\\');
            if(psz_Temp)
            {
                if(psz_Temp[-1] == ':')
                {
                    *psz_Temp = '/';
                    psz_Temp[1] = 0;
                }
                else
                    *psz_Temp = 0;
                GetDlgItem(IDC_EDIT_FULLPATH)->SetWindowText(masz_FullPath);
                FillList();
                ((CListBox *) GetDlgItem(IDC_LIST2))->SetCurSel(-1);
                return TRUE;
            }
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnOK(void)
{
    /*~~~~~~~~~~~~~~~~~~*/
    CString o_Temp;
    char    *psz_Temp;
    CString o_Path;
    int     i;
    /*~~~~~~~~~~~~~~~~~~*/

    /* Retreive current file */
    ((CEdit *) GetDlgItem(IDC_EDIT_FILE))->GetWindowText(mo_File);

    /* Is there a filter ? */
    if((mo_File.Find('*') != -1) || (mo_File.Find('?') != -1))
    {
        mo_Filter = mo_File;
        mo_File.Empty();
        FillList();
        return;
    }

    /* Retreive current path */
    ((CEdit *) GetDlgItem(IDC_EDIT_PATH))->GetWindowText(mo_Path);

    /* Is there a filter ? */
    if((mo_Path.Find('*') != -1) || (mo_Path.Find('?') != -1))
    {
        mo_Filter = mo_Path;
        mo_Path.Empty();
        FillList();
        return;
    }

    /* We need a file, and no one is selected */
    if((mi_ForPath == 1) && (mo_File.IsEmpty()))
    {
        if(mo_Path.IsEmpty()) return; /* No selected path, do nothing */
        GetItem(mo_Path, 0, o_Path);
        SelectDir(o_Path);
        return;
    }

    /* Retreive full path */
    ((CEdit *) GetDlgItem(IDC_EDIT_FULLPATH))->GetWindowText(o_Temp);
    L_strcpy(masz_FullPath, (char *) (LPCSTR) o_Temp);

    /* No path, but a full path */
    if(mo_Path.IsEmpty() && (mi_ForPath == 2) && mb_EnableDir)
    {
        psz_Temp = strrchr(masz_FullPath, '/');
        if(!psz_Temp) psz_Temp = strrchr(masz_FullPath, '\\');
        if(psz_Temp)
        {
            mo_Path = psz_Temp + 1;
            *psz_Temp = 0;
        }
        else
            mo_Path = masz_FullPath;
        mo_Path += "/";
        mi_NumPaths = 1;
    }

    /* We only need a full path */
    if(!mo_Path.IsEmpty() && (mi_ForPath == 2) && !mb_EnableDir)
    {
        GetItem(mo_Path, 1, o_Temp);
        if((masz_FullPath[L_strlen(masz_FullPath) - 1] != '/') && (masz_FullPath[L_strlen(masz_FullPath) - 1] != '\\'))
            L_strcat(masz_FullPath, "/");
        L_strcat(masz_FullPath, (char *) (LPCSTR) mo_Path);
    }

    /* Clear the current list */
    ClearList();

    /* Restore initial directory */
    if(mb_ForBig == FALSE)
    {
        GetCurrentDirectory(L_MAX_PATH, sgasz_PreviousPath);
        SetCurrentDirectory(masz_FirstPath);
    }
    else
    {
        L_strcpy(sgasz_PreviousPath, masz_FullPath);
    }

    /* Convert full path */
    psz_Temp = masz_FullPath;
    while(*psz_Temp)
    {
        if(*psz_Temp == '\\') *psz_Temp = '/';
        psz_Temp++;
    }

    /* Detach system image list */
    mo_ImageList.Detach();

    /* Save fullpath */
    ((CEdit *) GetDlgItem(IDC_EDIT_FULLPATH))->GetWindowText(o_Temp);
    for(i = 0; i < sgi_PreviousPath; i++)
    {
        if(!L_strcmpi(sgasz_PreviousPath1[i], (char *) (LPCSTR) o_Temp))
            break;
    }

    if(i == sgi_PreviousPath)
    {
        L_strcpy(sgasz_PreviousPath1[i], (char *) (LPCSTR) o_Temp);
        sgi_PreviousPath++;
    }

    CDialog::OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnCancel(void)
{
    /*~~~~~~~~~~~~~~~*/
    CString o_Temp;
    int     i;
    /*~~~~~~~~~~~~~~~*/

    /* Restore initial directory */
    if(mb_ForBig == FALSE)
    {
        GetCurrentDirectory(L_MAX_PATH, sgasz_PreviousPath);
        SetCurrentDirectory(masz_FirstPath);
    }
    else
    {
        L_strcpy(sgasz_PreviousPath, masz_FullPath);
    }

    /* Save fullpath */
    ((CEdit *) GetDlgItem(IDC_EDIT_FULLPATH))->GetWindowText(o_Temp);
    for(i = 0; i < sgi_PreviousPath; i++)
    {
        if(!L_strcmpi(sgasz_PreviousPath1[i], (char *) (LPCSTR) o_Temp))
            break;
    }

    if(i == sgi_PreviousPath)
    {
        L_strcpy(sgasz_PreviousPath1[i], (char *) (LPCSTR) o_Temp);
        sgi_PreviousPath++;
    }

    /* Clear the current list */
    ClearList();

    /* Detach system image list */
    mo_ImageList.Detach();

    CDialog::OnCancel();
}

/*$4
 ***********************************************************************************************************************
    MESSAGES
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnListCtrlSelChange(NMHDR *plv2, LRESULT *pResult)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl   *po_List;
    CString     o_String;
    UINT        i;
    int         i_Item;
    /*~~~~~~~~~~~~~~~~~~~~~*/
	NM_LISTVIEW *plv ;
	plv = (NM_LISTVIEW *)plv2;

    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));
    o_String = po_List->GetItemText(plv->iItem, 0);

    /* Dir */
    if(((tdst_FileDes *) po_List->GetItemData(plv->iItem))->b_Dir)
    {
        /* Search all selected paths */
        o_String.Empty();
        i_Item = po_List->GetNextItem(-1, LVNI_SELECTED);
        mi_NumPaths = 0;
        for(i = 0; i < po_List->GetSelectedCount(); i++)
        {
            if(((tdst_FileDes *) po_List->GetItemData(i_Item))->b_Dir)
            {
                mi_NumPaths++;
                o_String += po_List->GetItemText(i_Item, 0);
                if(i != (po_List->GetSelectedCount() - 1))
                    o_String += CString(SEP);
            }

            i_Item = po_List->GetNextItem(i_Item, LVNI_SELECTED);
        }

        /* Current path */
        ((CEdit *) GetDlgItem(IDC_EDIT_PATH))->SetWindowText((char *) (LPCSTR) o_String);
    }

    /* File */
    else
    {
        /* Search all selected paths */
        o_String.Empty();
        i_Item = po_List->GetNextItem(-1, LVNI_SELECTED);
        mi_NumFiles = 0;
        for(i = 0; i < po_List->GetSelectedCount(); i++)
        {
            if(((tdst_FileDes *) po_List->GetItemData(i_Item))->b_Dir == FALSE)
            {
                mi_NumFiles++;
                o_String += po_List->GetItemText(i_Item, 0);
                o_String += CString(SEP);
            }

            i_Item = po_List->GetNextItem(i_Item, LVNI_SELECTED);
        }

        /* Current file name */
        ((CEdit *) GetDlgItem(IDC_EDIT_FILE))->SetWindowText(o_String);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int __stdcall BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    /*~~~~~~~~~~~~~~~~*/
    DWORD   dwStyle;
    /*~~~~~~~~~~~~~~~~*/

    SetWindowText(hwnd, EDIA_STR_Csz_TitleComputers);
    dwStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW;
    ::SetWindowLong(hwnd, GWL_EXSTYLE, dwStyle);
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnDriveSelChange(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CListBox        *po_ListBox;
    DWORD           uw_Data;
    ITEMIDLIST      *idlist;
    LPITEMIDLIST    pidlRoot;
    CString         strDisplayName;
    BROWSEINFO      bi;
    LPITEMIDLIST    pidl;
    IMalloc         *pMalloc;
    char            asz_Temp[L_MAX_PATH];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* Get drive */
    po_ListBox = (CListBox *) GetDlgItem(IDC_LIST2);
    uw_Data = po_ListBox->GetItemData(po_ListBox->GetCurSel());
    switch(uw_Data)
    {
    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Normal drive
     -------------------------------------------------------------------------------------------------------------------
     */

    case 0:
        DlgDirSelect(masz_FullPath, IDC_LIST2);
        break;

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Desktop
     -------------------------------------------------------------------------------------------------------------------
     */

    case 1:
        if(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &idlist) == NOERROR)
        {
            if(!SHGetPathFromIDList(idlist, masz_FullPath))
                return;
        }
        break;

    /*$2
     -------------------------------------------------------------------------------------------------------------------
        Network
     -------------------------------------------------------------------------------------------------------------------
     */

    case 2:
        *asz_Temp = 0;

        pidlRoot = NULL;
        pMalloc = NULL;

        SHGetSpecialFolderLocation(GetSafeHwnd(), CSIDL_NETWORK, &pidlRoot);

        /* Browser for a computer */
        L_memset(&bi, 0, sizeof(BROWSEINFO));
        bi.hwndOwner = GetSafeHwnd();
        bi.pszDisplayName = strDisplayName.GetBuffer(L_MAX_PATH + 1);
        bi.lpfn = BrowseCallbackProc;

        bi.pidlRoot = pidlRoot;
        bi.lpszTitle = EDIA_STR_Csz_BrowseComputer;
        bi.ulFlags = BIF_BROWSEFORCOMPUTER;
        pidl = SHBrowseForFolder(&bi);

        /* Browse for content of the computer */
        if(pidl)
        {
            L_strcpy(asz_Temp, "\\\\");
            L_strcat(asz_Temp, (char *) (LPCSTR) strDisplayName);
            bi.pidlRoot = pidl;
            bi.lpszTitle = EDIA_STR_Csz_BrowseContent;
            bi.ulFlags = 0;
            pidl = SHBrowseForFolder(&bi);
        }

        strDisplayName.ReleaseBuffer();
        if(pidl) SHGetPathFromIDList(pidl, masz_FullPath);

        SHGetMalloc(&pMalloc);
        pMalloc->Free(pidlRoot);

        if(!pidl) return;
        break;
    }

    /* Fill list with new path */
    L_strcat(masz_FullPath, "\\");
    FillList();

    /* Update full path */
    ((CEdit *) GetDlgItem(IDC_EDIT_FULLPATH))->SetWindowText(masz_FullPath);

    /* Current path */
    ((CEdit *) GetDlgItem(IDC_EDIT_PATH))->SetWindowText("");

    /* Disable parent button */
    ((CButton *) GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnGoToParent(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl   *po_List;
    CString     o_String;
    char        *psz_Temp;
    /*~~~~~~~~~~~~~~~~~~~~~~*/

    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));
    if(((psz_Temp = L_strrchr(masz_FullPath, '\\')) != NULL) || ((psz_Temp = L_strrchr(masz_FullPath, '/')) != NULL))
    {
        /* Go to parent */
        *psz_Temp = '\0';

        /* Cause of drive */
        if(psz_Temp[-1] == ':')
        {
            L_strcat(masz_FullPath, "\\");
            ((CButton *) GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE);
        }

        FillList();

        /* Full path */
        ((CEdit *) GetDlgItem(IDC_EDIT_FULLPATH))->SetWindowText(masz_FullPath);

        /* Current path */
        ((CEdit *) GetDlgItem(IDC_EDIT_PATH))->SetWindowText("");
    }

    /* Force focus and highlight on ok button */
    ((CButton *) GetDlgItem(IDC_BUTTON1))->SetButtonStyle(BS_PUSHBUTTON);
    ((CButton *) GetDlgItem(IDOK))->SetButtonStyle(BS_DEFPUSHBUTTON);
    GetDlgItem(IDOK)->SetFocus();
}

/*$4
 ***********************************************************************************************************************
    FUNCTIONS
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    tdst_FileDes    *p1, *p2;
    char            *psz_Ext1, *psz_Ext2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    p1 = (tdst_FileDes *) lParam1;
    p2 = (tdst_FileDes *) lParam2;

    if(p1->b_Dir > p2->b_Dir)
    {
        return -1;
    }

    if(p1->b_Dir < p2->b_Dir)
    {
        return 1;
    }

    psz_Ext1 = L_strrchr(p1->asz_Name, '.');
    psz_Ext2 = L_strrchr(p2->asz_Name, '.');

    if((sgb_SortByName) || (!psz_Ext1 && !psz_Ext2))
    {
		if(p1->asz_Name[0] == '_' && L_isdigit(p2->asz_Name[0])) return -1;
		if(p2->asz_Name[0] == '_' && L_isdigit(p1->asz_Name[0])) return 1;
        return L_strcmpi(p1->asz_Name, p2->asz_Name);
    }

    if(!psz_Ext1 && psz_Ext2)
    {
        return 1;
    }

    if(psz_Ext1 && !psz_Ext2)
    {
        return -1;
    }

    return L_strcmpi(psz_Ext1, psz_Ext2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::ClearList(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl   *po_List;
    int         i;
    LV_ITEM     st_ListCtrlItem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));
    for(i = 0; i < po_List->GetItemCount(); i++)
    {
        st_ListCtrlItem.mask = LVIF_PARAM;
        st_ListCtrlItem.iItem = i;
        st_ListCtrlItem.iSubItem = 0;
        po_List->GetItem(&st_ListCtrlItem);
        delete(tdst_FileDes *) st_ListCtrlItem.lParam;
    }

    po_List->DeleteAllItems();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnSortName(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl   *po_List;
    /*~~~~~~~~~~~~~~~~~~~~~*/

    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));
    sgb_SortByName = TRUE;
    po_List->SortItems(CompareFunc, 0);
    ((CButton *) GetDlgItem(IDC_RADIONAME))->SetCheck(1);
	((CButton *) GetDlgItem(IDC_RADIOTYPE))->SetCheck(0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnSortType(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl   *po_List;
    /*~~~~~~~~~~~~~~~~~~~~~*/

    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));
    sgb_SortByName = FALSE;
    po_List->SortItems(CompareFunc, 0);
    ((CButton *) GetDlgItem(IDC_RADIONAME))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIOTYPE))->SetCheck(1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::SelectDir(CString &o_Path)
{
    /* Compute new path and erase current file */
    if(mb_ForBig == FALSE)
    {
        L_fullpath(masz_FullPath, (char *) (LPCSTR) o_Path, L_MAX_PATH);
    }
    else
    {
        L_strcat(masz_FullPath, "/");
        L_strcat(masz_FullPath, (char *) (LPCSTR) o_Path);
    }

    /* Fill list with new path */
    mi_NumPaths = 0;
    FillList();

    /* Full path */
    ((CEdit *) GetDlgItem(IDC_EDIT_FULLPATH))->SetWindowText(masz_FullPath);

    /* Current path */
    ((CEdit *) GetDlgItem(IDC_EDIT_PATH))->SetWindowText("");

    /* Enable parent button */
    ((CButton *) GetDlgItem(IDC_BUTTON1))->EnableWindow(TRUE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnLButtonDblClk(UINT, CPoint pt)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl   *po_List;
    int         i_Res;
    CString     o_String;
    /*~~~~~~~~~~~~~~~~~~~~~*/

    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));

    if((i_Res = po_List->HitTest(pt)) != -1)
    {
        o_String = po_List->GetItemText(i_Res, 0);

        /* DblClk on a dir */
        if(((tdst_FileDes *) po_List->GetItemData(i_Res))->b_Dir == 1)
		{
            SelectDir(o_String);
		}

        /* DblClk on a file */
        else
		{
            OnOK();
		}
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::GetItem(CString &_o_String, int _i_Num, CString &_o_Ret)
{
    /*~~~~~~~~~~~~~~~~~*/
    int     i, i_Beg;
    CString o_Temp;
    /*~~~~~~~~~~~~~~~~~*/

    o_Temp = _o_String;

    for(i = 1; i < _i_Num; i++)
    {
        i_Beg = o_Temp.Find(SEP);
        if(i_Beg == -1)
        {
            _o_Ret = o_Temp;
            return;
        }

        o_Temp = o_Temp.Right(o_Temp.GetLength() - i_Beg - 1);
    }

    i_Beg = o_Temp.Find(SEP);
    if(i_Beg == -1)
    {
        _o_Ret = o_Temp;
        return;
    }

    _o_Ret = o_Temp.Left(i_Beg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::FillList(void)
{
    if(mb_ForBig)
        FillListBig();
    else
        FillListNormal();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::FillListNormal(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl           *po_List;
    struct L_finddata_t st_FileInfos;
    long                l_Handle;
    char                asz_Name[L_MAX_PATH];
    char                asz_Path[L_MAX_PATH];
    SHFILEINFO          st_FileInfo;
    LV_ITEM             st_ListCtrlItem;
    int                 i_FoundDir;
    tdst_FileDes        *pst_Des;
	CString				o_Tgt;
	CString				o_Word;
	static int			icondir = -1;
	static CMapStringToPtr	iconfiles;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    i_FoundDir = 0;

    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));

    AfxGetApp()->DoWaitCursor(1);
    M_MF()->LockDisplay(po_List);

    /* Change current directory */
    SetCurrentDirectory(masz_FullPath);

    /* First fill list with in path */
    L_strcpy(asz_Name, masz_FullPath);
    if((asz_Name[L_strlen(asz_Name) - 1] != '\\') && (asz_Name[L_strlen(asz_Name) - 1] != '/'))
        L_strcat(asz_Name, "/");
    L_strcat(asz_Name, (char *) (LPCSTR) mo_Filter);

    /* Delete all list */
    ClearList();

    /* Search all files. */
    do
    {
        /* Get first file */
        if((l_Handle = L_findfirst(asz_Name, &st_FileInfos)) != -1)
        {
            do
            {
                L_strcpy(asz_Path, masz_FullPath);
                if
                (
                    (asz_Path[L_strlen(asz_Path) - 1] != '\\') &&
                    (asz_Path[L_strlen(asz_Path) - 1] != '/')
                ) L_strcat(asz_Path, "\\");
				if(asz_Path[L_strlen(asz_Path) - 1] == '/')
					asz_Path[L_strlen(asz_Path) - 1] = '\\';
                L_strcat(asz_Path, st_FileInfos.name);

                /* Detect dir. Zap . and .. */
                if(st_FileInfos.attrib & _A_SUBDIR)
                {
					st_ListCtrlItem.lParam = 1;

                    /* Get icon in system image list */
					if(icondir == -1)
						SHGetFileInfo(asz_Path, FILE_ATTRIBUTE_DIRECTORY, &st_FileInfo, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
					else
						st_FileInfo.iIcon = icondir;
					icondir = st_FileInfo.iIcon;

                    i_FoundDir = 1;
                    if(!L_strcmpi(st_FileInfos.name, ".")) continue;
                    if(!L_strcmpi(st_FileInfos.name, "..")) continue;
                }

                /* Detect a file */
                else
                {
					if(*masz_Filter)
					{
						o_Word = st_FileInfos.name;
						o_Word.MakeLower();
						if(o_Word.Find(masz_Filter) < 0)
							goto zap;
					}

                    /* Get icon in system image list */
					{
						char *pz;
						int icon;
						pz = L_strrchr(st_FileInfos.name, '.');
						if(pz)
						{
							if(iconfiles.Lookup(pz, (void * &) icon))
								st_FileInfo.iIcon = icon;
							else
							{
								SHGetFileInfo(asz_Path, FILE_ATTRIBUTE_NORMAL, &st_FileInfo, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
								iconfiles.SetAt(pz, (void *) st_FileInfo.iIcon);
							}
						}
						else
							SHGetFileInfo(asz_Path, FILE_ATTRIBUTE_NORMAL, &st_FileInfo, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON | SHGFI_SYSICONINDEX);
					}

                    /* If we browse for paths, dont display files */
                    if(mi_ForPath == 2) continue;
                    st_ListCtrlItem.lParam = 0;
                }

                /* Init user structure with name and dir info (is it a dir guy ?) */
                pst_Des = new tdst_FileDes;
                L_strcpy(pst_Des->asz_Name, st_FileInfos.name);
                pst_Des->b_Dir = (BOOL) st_ListCtrlItem.lParam;
                st_ListCtrlItem.lParam = (long) pst_Des;

                /* Init the rest of the structure */
                st_ListCtrlItem.mask = LVIF_TEXT | TVIF_PARAM | LVIF_IMAGE;
                st_ListCtrlItem.iItem = 0;
                st_ListCtrlItem.iSubItem = 0;
                st_ListCtrlItem.pszText = st_FileInfos.name;
                st_ListCtrlItem.iImage = st_FileInfo.iIcon;
                st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
                po_List->InsertItem(&st_ListCtrlItem);
zap:;
            } while(L_findnext(l_Handle, &st_FileInfos) != -1);

            L_findclose(l_Handle);
        }

        i_FoundDir++;
        if(i_FoundDir == 1)
        {
            L_strcpy(asz_Name, masz_FullPath);
            if((asz_Name[L_strlen(asz_Name) - 1] != '\\') && (asz_Name[L_strlen(asz_Name) - 1] != '/'))
            {
                L_strcat(asz_Name, "/");
            }

            L_strcat(asz_Name, "*.");
        }
    } while(i_FoundDir <= 1);

    /* Sort item (fir subdir and then files) */
    po_List->SortItems(CompareFunc, 0);

    M_MF()->UnlockDisplay(po_List);

    AfxGetApp()->DoWaitCursor(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::FillListBig(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CListCtrl       *po_List;
    LV_ITEM         st_ListCtrlItem;
    tdst_FileDes    *pst_Des;
    BIG_INDEX       ul_Index, ul_Dir;
    SHFILEINFO      st_FileInfo;
    char            asz_Tmp[128];
    char            *psz_Tmp, *psz_Tmp1;
    CString         o1, o2;
	CString			o_Word;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_List = ((CListCtrl *) GetDlgItem(IDC_LIST1));

    AfxGetApp()->DoWaitCursor(1);
    M_MF()->LockDisplay(po_List);

    /* Change current directory */
    SetCurrentDirectory(masz_FullPath);

    /* First fill list with in path */
    ul_Dir = BIG_ul_SearchDir(masz_FullPath);
    if(ul_Dir == BIG_C_InvalidIndex)
    {
        ul_Dir = 0;
        L_strcpy(masz_FullPath, BIG_NameDir(0));
    }

    /* Delete all list */
    ClearList();

    /* Search all files. */
    ul_Index = BIG_FirstFile(ul_Dir);
    while(ul_Index != BIG_C_InvalidIndex)
    {
		if(*masz_Filter)
		{
			o_Word = BIG_NameFile(ul_Index);
			o_Word.MakeLower();
			if(o_Word.Find(masz_Filter) < 0)
				goto zap;
		}

        /* Get icon in system image list */
        SHGetFileInfo
        (
            BIG_NameFile(ul_Index),
            FILE_ATTRIBUTE_NORMAL,
            &st_FileInfo,
            sizeof(SHFILEINFO),
            SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES
        );

        /* A filter */
        if(!mo_Filter.IsEmpty() && (mo_Filter != "*.*"))
        {
            L_strcpy(asz_Tmp, (char *) (LPCSTR) mo_Filter);
            psz_Tmp = asz_Tmp;

            while(psz_Tmp)
            {
                psz_Tmp1 = L_strchr(psz_Tmp, ',');
                if(psz_Tmp1) *psz_Tmp1 = 0;
                o1 = BIG_NameFile(ul_Index);
                o1.MakeLower();
                o2 = psz_Tmp + 1;
                o2.MakeLower();
                if(L_strstr((char *) (LPCSTR) o1, (char *) (LPCSTR) o2))
                    goto l_Ok;
                psz_Tmp = psz_Tmp1;
                if(psz_Tmp) psz_Tmp++;
            }

            ul_Index = BIG_NextFile(ul_Index);
            continue;
        }

        /* Init user structure with name and dir info (is it a dir guy ?) */
l_Ok:
        pst_Des = new tdst_FileDes;
        L_strcpy(pst_Des->asz_Name, BIG_NameFile(ul_Index));
        pst_Des->b_Dir = 0;
        st_ListCtrlItem.lParam = (long) pst_Des;

        /* Init the rest of the structure */
        st_ListCtrlItem.mask = LVIF_TEXT | TVIF_PARAM | LVIF_IMAGE;
        st_ListCtrlItem.iItem = 0;
        st_ListCtrlItem.iSubItem = 0;
        st_ListCtrlItem.pszText = BIG_NameFile(ul_Index);
        st_ListCtrlItem.iImage = st_FileInfo.iIcon;
        st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
        po_List->InsertItem(&st_ListCtrlItem);
zap:
        ul_Index = BIG_NextFile(ul_Index);
    }

    /* Search subdirs. */
    ul_Index = BIG_SubDir(ul_Dir);
    while(ul_Index != BIG_C_InvalidIndex)
    {
        /* Get icon in system image list */
        SHGetFileInfo
        (
            BIG_NameDir(ul_Index),
            FILE_ATTRIBUTE_DIRECTORY,
            &st_FileInfo,
            sizeof(SHFILEINFO),
            SHGFI_SMALLICON | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES
        );

        /* Init user structure with name and dir info (is it a dir guy ?) */
        pst_Des = new tdst_FileDes;
        L_strcpy(pst_Des->asz_Name, BIG_NameDir(ul_Index));
        pst_Des->b_Dir = 1;
        st_ListCtrlItem.lParam = (long) pst_Des;

        /* Init the rest of the structure */
        st_ListCtrlItem.mask = LVIF_TEXT | TVIF_PARAM | LVIF_IMAGE;
        st_ListCtrlItem.iItem = 0;
        st_ListCtrlItem.iSubItem = 0;
        st_ListCtrlItem.pszText = BIG_NameDir(ul_Index);
        st_ListCtrlItem.iImage = st_FileInfo.iIcon;
        st_ListCtrlItem.cchTextMax = L_strlen(st_ListCtrlItem.pszText);
        po_List->InsertItem(&st_ListCtrlItem);

        ul_Index = BIG_NextDir(ul_Index);
    }

    /* Sort item (fir subdir and then files) */
    po_List->SortItems(CompareFunc, 0);

    M_MF()->UnlockDisplay(po_List);

    AfxGetApp()->DoWaitCursor(-1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_FileDialog::OnFilter(void)
{
	CEdit	*po;
	CString o;
	po = (CEdit *) GetDlgItem(IDC_EDIT_FILTER);
	po->GetWindowText(o);
	o.MakeLower();
	L_strcpy(masz_Filter, (char *) (LPCSTR) o);
    FillList();
}

#endif /* ACTIVE_EDITORS */
