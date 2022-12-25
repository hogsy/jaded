/*$T DIAfile_dlg.h GC!1.41 08/16/99 12:22:24 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIfile.h"
#include "DIAlogs/DIAbase.h"

/*
 ---------------------------------------------------------------------------------------------------
    Description of a file
 ---------------------------------------------------------------------------------------------------
 */
typedef struct  tdst_FileDes_
{
    char    asz_Name[L_MAX_PATH];
    int		b_Dir; /* 1 dir, 0 file, 2 shortcut dir */
} tdst_FileDes;

/*$4
 ***************************************************************************************************
 ***************************************************************************************************
 */

class EDIA_cl_FileDialog : public EDIA_cl_BaseDialog
{
/*$2
 ---------------------------------------------------------------------------------------------------
    CONSTRUCT.
 ---------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_FileDialog
    (
        char *,
        int,
        BOOL _b_EnableDir = TRUE,
        BOOL _b_ForBig = FALSE,
        char *_psz_InitPath = NULL,
		char *_psz_Filter = NULL
    );

/*$2
 ---------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 ---------------------------------------------------------------------------------------------------
 */

public:
    int             mi_ForPath;                 /* 1 file only, 2 path only, 3 both */
    BOOL            mb_EnableDir;
    BOOL            mb_ForBig;                  /* Browse current bigfile */
    char            masz_FullPath[L_MAX_PATH];  /* Initial selected directory */
    char            masz_FirstPath[L_MAX_PATH]; /* First path when open dialog */
	char			masz_Filter[1024];
    CString         mo_Path;                    /* Selected path(s) */
    int             mi_NumPaths;                /* Number of selected paths */
    CString         mo_File;                    /* Selected file(s) */
    int             mi_NumFiles;                /* Number of selected files */
    CString         mo_Filter;                  /* Filter */
    CString         mo_Title;                   /* Title of the window */
    CImageList      mo_ImageList;               /* Image list to browse */
	char			*mpsz_Filter;				/* Initial filter */
    BOOL            mb_UseFavorite;         

/*$2
 ---------------------------------------------------------------------------------------------------
    OVERWRITE.
 ---------------------------------------------------------------------------------------------------
 */

public:
    virtual BOOL    OnInitDialog(void);
    virtual BOOL    PreTranslateMessage(MSG *);
    virtual void    OnOK(void);
    virtual void    OnCancel(void);

/*$2
 ---------------------------------------------------------------------------------------------------
    FUNCTIONS.
 ---------------------------------------------------------------------------------------------------
 */

public:
    void    ClearList(void);
    void    FillList(void);
    void    FillListNormal(void);
    void    FillListBig(void);
    void    OnLButtonDblClk(UINT, CPoint);
    void    GetItem(CString &, int, CString &);
    void    SelectDir(CString &);

/*$2
 ---------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 ---------------------------------------------------------------------------------------------------
 */

protected:
    afx_msg void    OnListCtrlSelChange(NMHDR *, LRESULT *);
    afx_msg void    OnDriveSelChange(void);
    afx_msg void    OnGoToParent(void);
    afx_msg void    OnSortName(void);
    afx_msg void    OnSortType(void);
	afx_msg void	OnSelCombo(void);
	afx_msg void	OnEditChange(void);
	afx_msg void	OnFilter(void);
    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
