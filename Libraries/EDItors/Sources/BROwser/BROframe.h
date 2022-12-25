/*$T BROframe.h GC!1.71 01/18/00 10:55:58 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once

#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "BIGfiles/BIGdefs.h"
#include "EDIbaseframe.h"
#include "BROframe_act.h"
#include "Res/Res.h"
#include "EDImainframe.h"

/*$4
 ***********************************************************************************************************************
    Global.
 ***********************************************************************************************************************
 */

#define EBRO_C_MaxFavorites             20

#define EBRO_C_ListCtrlMaxColumns       9
#define EBRO_C_ListCtrlSpecial          8
#define EBRO_C_ListCtrlLastNoEngine     5

#define EBRO_C_GrpCtrlMaxColumns        4

#define EBRO_C_ItemStateExpanded        0x01
#define EBRO_C_ItemStateExpandedOnce    0x02

extern char *EBRO_gasz_ListColNames[EBRO_C_ListCtrlMaxColumns];
extern char *EBRO_gasz_GrpColNames[EBRO_C_GrpCtrlMaxColumns];

#define EBRO_M_EngineMode() (mst_Ini.b_EngineMode)

/*
 -----------------------------------------------------------------------------------------------------------------------
    To describe a favorite.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  EBRO_tdst_Favorite_
{
    char    asz_PathName[BIG_C_MaxLenPath];
    char    asz_DisplayName[BIG_C_MaxLenPath];
} EBRO_tdst_Favorite;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Structure for linked path
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  EBRO_tdst_LinkedPath_
{
    char    asz_PathSrc[BIG_C_MaxLenPath];
    char    asz_Path[L_MAX_PATH];

#define EBRO_C_Mirror           0x00000001
#define EBRO_C_InvMirror        0x00000002
#define EBRO_C_ImpOnlyLoaded    0x00000004
#define EBRO_C_ImpOnlyTruncated 0x00000008

    UINT    ui_Flags;
    int     i_Order;
} EBRO_tdst_LinkedPath;

/*
 =======================================================================================================================
    Aim:    Determin if the current activated pane is the list control, or the tree control
 =======================================================================================================================
 */
#define LIST_ACTIVATED() \
        ( \
            ( \
                (GetFocus() == mpo_ListCtrl) || \
                (mpo_InsideLstSplitter->GetActivePane() == mpo_ListCtrl) || \
                (EDI_go_TheApp.mpo_FocusWndWhenDlg == mpo_ListCtrl) \
            ) && (mpo_ListCtrl->GetNextItem(-1, LVNI_SELECTED) != -1) \
        )

#define GRP_ACTIVATED() \
        ( \
            ( \
                (GetFocus() == mpo_GrpCtrl) || \
                (mpo_InsideLstSplitter->GetActivePane() == mpo_GrpCtrl) || \
                (EDI_go_TheApp.mpo_FocusWndWhenDlg == mpo_GrpCtrl) \
            ) && (mpo_GrpCtrl->GetNextItem(-1, LVNI_SELECTED) != -1) \
        )

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class   EBRO_cl_TreeCtrl;
class   EBRO_cl_ListCtrl;
class   EBRO_cl_GrpCtrl;
class EBRO_cl_Frame : public EDI_cl_BaseFrame
{
    DECLARE_DYNCREATE(EBRO_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EBRO_cl_Frame (void);
    ~EBRO_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:

    /* Structure that is saved to .ini file */
    struct
    {
        int                 i_HeightFirstPane;
        int                 i_HeightSecondPane;

        int                 ai_WidthListCol[EBRO_C_ListCtrlMaxColumns];
        int                 ai_NumColListCtrl[EBRO_C_ListCtrlMaxColumns];
        int                 ai_WidthGrpCol[EBRO_C_GrpCtrlMaxColumns];
        int                 ai_NumColGrpCtrl[EBRO_C_GrpCtrlMaxColumns];

        EBRO_tdst_Favorite  ast_Favorites[EBRO_C_MaxFavorites];
        int                 i_NumFavorites;

        int                 i_SortColumnListCtrl;
        int                 i_SortColumnGrpCtrl;

        BOOL                b_DUMMY;
        BOOL                b_EngineMode;

        int                 i_Filter;

        struct
        {
            BOOL    b_RecImp;
            BOOL    b_RecExp;
            BOOL    b_RecIn;
            BOOL    b_RecOut;
            BOOL    b_RecUndoOut;
            BOOL    b_RecGet;
            BOOL    b_AutoOn;
            int     i_AutoFreq;
            BOOL    b_AutoRecImp;
            BOOL    b_AutoRecExp;
            BOOL    b_LinkRefresh;
			int		BBB;
			int		b_MirrorGetLatest;
            int     l[6];
        } mst_Options;

        struct
        {
            BOOL    b_ForceImp;
            BOOL    b_AutoImp;
            char    c_MaterialUpdate;
            char    c_MaterialMerge;
            char    c_GraphicObjectUpdate;
            char    c_GraphicObjectMerge;
            char    c_MaterialNoSingle;
            char    c_dummy[3];
            int     l2[7];
        } mst_MadOptions;

#define EBRO_C_ListModeIcon         1
#define EBRO_C_ListModeSmallIcon    2
#define EBRO_C_ListModeReport       3
        int     i_ListDispMode;
        int     i_GrpDispMode;

		ULONG	i_GroupAutoHide;
        ULONG   l1[7];
    } mst_Ini;

    EBRO_cl_TreeCtrl    *mpo_TreeCtrl;
    EBRO_cl_ListCtrl    *mpo_ListCtrl;
    EBRO_cl_GrpCtrl     *mpo_GrpCtrl;
    CSplitterWnd        *mpo_InsideSplitter;
    CSplitterWnd        *mpo_InsideLstSplitter;
    CMapStringToOb      mo_LinkedPath;
	char				maz_Filter[1024];

    BIG_INDEX           mul_TreeItemNormal;
    BIG_INDEX           mul_TreeItemEngine;
    int                 mi_ListItemNormal;
    int                 mi_ScrollPosXNormal;
    int                 mi_ScrollPosYNormal;
    int                 mi_ScrollPosXEngine;
    int                 mi_ScrollPosYEngine;

    BOOL                mb_CreateSpecialFile;
	BOOL				mb_NoUpdateVss;

	BOOL				mb_P4RefreshListCtrl;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    HTREEITEM   x_GetTreeItemByName(BIG_INDEX);

    void        SaveColSizeList(void);
    void        SaveColSizeGrp(void);
    void        ResetAllColumnsListCtrl(BOOL _b_Save = FALSE);
    void        AddDeleteColumnListCtrl(int);
    void        AddColumnToListCtrl(int);

    void        ResetAllColumnsGrpCtrl(BOOL _b_Save = FALSE);
    void        AddDeleteColumnGrpCtrl(int);
    void        AddColumnToGrpCtrl(int);

    BOOL        FillWithPointers(void *);
    void        RefreshAll
                (
                    BOOL        _b_VSS = FALSE,
                    BIG_INDEX   _ul_TreeSel = BIG_C_InvalidIndex,
                    int         _i_LstSel = -1,
                    int         _i_ScrollX = -1,
                    int         _i_ScrollY = -1
                );
    int         i_NumPaneForPoint(CPoint, void **);
    void        AppendMenu(CMenu &, CMenu &);
    void        SelectDir(BIG_INDEX);
    BOOL        b_GetOBJ(OBJ_tdst_GameObject **, char *_psz_Ext, BIG_INDEX *_pul = NULL);

    void        ResetExpanded(int, BIG_INDEX);
    void        OnCtrlPopup(CPoint);
    void        OnTreeCtrlSelChange(void);
    void        OnTreeCtrlExpanded(NM_TREEVIEW *);
    void        OnTreeCtrlBeginDrag(CPoint);
    void        OnListCtrlItemSel(int);
    void        OnListCtrlDblClk(int);
    void        OnListCtrlBeginDrag(CPoint, int);
    void        OnGrpCtrlItemSel(int);
    void        OnGrpCtrlBeginDrag(CPoint, int);
    void        OnGrpCtrlDblClk(int);

    void        CommonVss(int);
	void		OnDelDirBase(void);
    void        OnCheckIn(void);
    void        OnCheckInNoOut(void);
    void        OnCheckOut(void);
    void        OnLocalCheckOut(void);
    void        OnUndoCheckOut(void);
    void        OnGetLatestVersion(void);

	// --------------------------------------------------------------------------
	// Added methods to access Perforce server (SourceControl)
	void		OnPerforceSync(BOOL _bForceSync = FALSE);
	void		OnPerforceServerSync();
	void		OnPerforceEdit();
	void		OnPerforceAdd();
	void		OnPerforceDelete();
	void		OnPerforceDiff();
	void		OnPerforceHistory();
	void		OnPerforceSubmit();
	void		OnPerforceSubmitEdit();
	void		OnPerforceRevert();

	void		OnPerforceRefreshListCtrl(BIG_INDEX _ul_DirIndex);
	// --------------------------------------------------------------------------

    void        OnOrderPaths(void);
    void        OnSetAsMirror(void);
    void        OnSetAsInvMirror(void);
    void        OnImportOnlyTruncated(void);
    void        OnImportOnlyLoaded(void);
    void        ReadLinkedPath(void);
    void        SaveLinkedPath(void);
    void        DeleteLinkedPath(void);
    BOOL        b_GetLinkedPath(BIG_INDEX, char *, BOOL _b_Real = TRUE, EBRO_tdst_LinkedPath **p = NULL);
    void        CommonExport(char *);

    void        CopyMove(EDI_tdst_DragDrop *);
    void        MirrorImport(char *, char *);

    void        ClearUnloaded(void);
    void        UnloadedReset(BIG_INDEX);

    void        SelectGroup(int);
    void        Zoom3DView(int);
    void        ForceLoadObject(int);
	void		FindCheckWorld(void);
    void        CreateCob();
    void        GMAT_DisplayInfo();
    ULONG       ul_CreateAssociatedSMD();
    ULONG       ul_CreateAssociatedMTX();
    void        OnImportMergeEdit(void);
    void        OnExportMergeEdit(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    void    ReinitIni(void);
    void    LoadIni(void);
    void    SaveIni(void);
    void    TreatIni(void);
    void    OpenProject(void);
    void    CloseProject(void);
    void    OnActivate(void);
    char    *psz_OnActionGetBase(void)  { return EBRO_asz_ActionBase; };
    int     i_IsItVarAction(ULONG, EVAV_cl_View *);
    void    ForceRefresh(void)  { RefreshAll(); };
    void    OnAction(ULONG);
    UINT    ui_OnActionState(ULONG);
    BOOL    b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
    void    OnActionUI(ULONG, CString &, CString &);
    int     i_OnMessage(ULONG, ULONG, ULONG);
    void    OnRealIdle(void);
    void    OneTrameEnding(void);
    void    BeforeEngine(void);
    void    AfterEngine(void);
    void    OnToolBarCommand(UINT);
    void    OnToolBarCommandUI(UINT, CCmdUI *);
    BOOL    b_KnowsKey(USHORT);

    BOOL    b_CreateSpecialFile(char *_psz_Path, char *_sz_Name );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg void    OnSize(UINT, int, int);
    afx_msg void    OnLButtonUp(UINT, CPoint);
    afx_msg void    OnRButtonDown(UINT, CPoint);
    afx_msg void    OnMouseMove(UINT, CPoint);
    afx_msg void    OnEngineMode(void);
    afx_msg void    OnImport(void);
    afx_msg void    OnExport(void);
    afx_msg void    OnImportMerge(int);
    afx_msg void    OnExportMerge(int);

	afx_msg void	OnExportKeys(void);
	afx_msg void	OnImportKeys(void);

    afx_msg void    OnAddFavorites(void);
    afx_msg void    OnOrganizeFavorites(void);
    afx_msg void    OnCreateDir(void);
    afx_msg void    OnCreateFile(void);
    afx_msg void    OnRename(void);
	afx_msg void	OnRenameMulti(void);
    afx_msg void    OnChangeKey(void);
    afx_msg void    OnDelete(void);
    afx_msg void    OnLogSize(void);
    afx_msg void    OnRefresh(void);
    afx_msg void    OnGoParent(void);
    afx_msg void    OnGoRoot(void);
    afx_msg void    OnGoParentUI(CCmdUI *);
	afx_msg void	OnP4RefreshListCtrl(void);
    afx_msg void    OnFavoriteChange(void);
    afx_msg void    OnViewColumnListCtrl(UINT);
    afx_msg void    OnViewColumnGrpCtrl(UINT);
    afx_msg void    OnUnlinkPath(void);
    afx_msg void    OnLinkToExt(char*);
    afx_msg void    OnImpFromExt(void);
    afx_msg void    OnExpToExt(void);
    afx_msg void    OnRefreshLinkedPaths(void);
    afx_msg void    OnOrderGroup(void);
    afx_msg void    OnCleanGroup(void);
	afx_msg void	OnCheckInMirror(void);

    DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */

