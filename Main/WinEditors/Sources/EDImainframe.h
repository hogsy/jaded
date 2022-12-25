/*$T EDImainframe.h GC! 1.088 10/03/00 17:07:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BASsys.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "BIGfiles/BIGdefs.h"
#include "EDIbaseframe.h"
#include "EDIbaseview.h"
#include "EDIsplitter.h"

#if MONTREAL_SPECIFIC
#define EDI_VERSION_FILENAME_LENGTH 128
#define EDI_VERSION_LENGTH          8
#define EDI_ERR_MSG_LENGTH          256
#endif

#ifdef JADEFUSION
/*$2
-----------------------------------------------------------------------------------------------------------------------
Features Mgr
-----------------------------------------------------------------------------------------------------------------------
*/

#define EDI_MTL_FEATURE_INI_FILENAME "FeaturesMgr.ini"
#define	EDI_MTL_MAX_FEATURES	32

#define EDI_MTL_FEATURE_IS_ACTIVE(a)	M_MF()->ab_MtlFeatures[a]
// ***********************************************************************************************************************
//    Enums
// ***********************************************************************************************************************

typedef enum
{
	// --------------------------------------------------------------------------------

	EDI_MTL_CheckWorldDialog,
	EDI_MTL_SubObjShortCut,
	EDI_MTL_AdvanceRename,
	EDI_MTL_Create_WP,
	EDI_MTL_PrefabInstance,
	EDI_MTL_DuplicateExt,
	// <-- Insert New Entries Here
	// >-- Don't forget to update the text array in mainframe.cpp
	// <-- Don't go over MAX_FEATURES

	// --------------------------------------------------------------------------------

	EDI_MTL_FeaturesCount
}
EDI_MTL_Features;
#endif

/*$4
 ***********************************************************************************************************************
    To describe a drag & drop operation.
 ***********************************************************************************************************************
 */

typedef void (* VSSCB)(int);
class	EDI_cl_BaseFrame;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Constant for the i_Type of the d&d structure
 -----------------------------------------------------------------------------------------------------------------------
 */

#define EDI_DD_User 0	/* A special data, depends on user */
#define EDI_DD_File 1	/* Ul_FatDir and/or ul_FatFile must be valid in structure */
#define EDI_DD_Data 2	/* I_Param2 must be a registered pointers (in LINK module) */
#define EDI_DD_Long 3	/* D&D of a LONG value (to copy it elsewhere) */
#define EDI_DD_Zone 4	/* D&D of a zone to add it to a colset */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Main global structure
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	EDI_tdst_DragDrop_
{
	BIG_INDEX			ul_FatDir;
	BIG_INDEX			ul_FatFile;
	int					i_Param1;
	int					i_Param2;
	int					i_Param3;
	int					i_Type; /* Type of the data associated with the d&d operation */

	CWnd				*po_CaptureWnd;
	EDI_cl_BaseFrame	*po_SourceEditor;
	EDI_cl_BaseFrame	*po_DestEditor;
	EDI_cl_BaseFrame	*po_FocusEditor;
	CPoint				o_Pt;
	CRect				o_OutRect;
	BOOL				b_BeginDragDrop;
	BOOL				b_CanDragDrop;
	BOOL				b_Copy;
	BOOL				b_ForceCopy;
} EDI_tdst_DragDrop;

extern EDI_tdst_DragDrop					EDI_gst_DragDrop;
extern BOOL									EDI_gb_RecurseVss;
extern CMap<BIG_INDEX, BIG_INDEX, int, int> EDI_go_FHCDir;
extern CMap<BIG_INDEX, BIG_INDEX, int, int> EDI_go_FHCFile;
extern CMapPtrToPtr							EDI_mo_FileIcons16;
extern CMapPtrToPtr							EDI_mo_FileIcons32;
extern CMapPtrToPtr							EDI_mo_FileIconsKey16;
extern CMapPtrToPtr							EDI_mo_FileIconsKey32;
extern CMapPtrToPtr							EDI_mo_FileKeyBmp;

/*
 -----------------------------------------------------------------------------------------------------------------------
    To define a present editor
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct
{
	int					i_Type;
	int					i_Cpt;
	EDI_cl_BaseFrame	*po_Editor;
} tdst_PresentEditor;

/* To describe a desktop */
#define EDI_C_MaxViews	5

typedef struct
{
	int		i_SplitterCx;
	int		i_SplitterCx1;
	int		i_SplitterCy1;
	int		i_SplitterCy12;
	int		i_SplitterCy2;
	int		i_SplitterCy22;

	int		i_FSSplitterCx;
	int		i_FSSplitterCx1;
	int		i_FSSplitterCy1;
	int		i_FSSplitterCy12;
	int		i_FSSplitterCy2;
	int		i_FSSplitterCy22;

	BOOL	b_FSLeftFrameOn;
	BOOL	b_FSMainBarOn;
	CRect	o_FSPos;
	BOOL	b_FSIsZoomed;
	BOOL	b_VeryMaximized;

	BOOL	b_HrzOrient;
	BOOL	b_ShortMode;

	struct
	{
		BOOL				b_TopFill;
		int					i_CurrentSelScreen;
		int					i_NumEditors;
		tdst_PresentEditor	ast_Editors[EDI_C_MaxEditors];
	} ast_View[EDI_C_MaxViews];
}
EDI_tdst_Desktop;

/*
 =======================================================================================================================
    To get main frame
 =======================================================================================================================
 */
#define M_MF()	((EDI_cl_MainFrame *) AfxGetMainWnd())

/*
 =======================================================================================================================
    Macro to register an editor
 =======================================================================================================================
 */
#define EDI_M_RegisterEditor(Type, __Class, Name, Dial, TB) \
	{ \
		mast_ListOfEditors[mi_TotalEditors].i_Type = Type; \
		mast_ListOfEditors[mi_TotalEditors].po_Class = RUNTIME_CLASS(__Class); \
		L_strcpy(mast_ListOfEditors[mi_TotalEditors].asz_Name, Name); \
		mast_ListOfEditors[mi_TotalEditors].ui_ResDialogBar = Dial; \
		mast_ListOfEditors[mi_TotalEditors].ui_ResToolBar = TB; \
		mast_ListOfEditors[mi_TotalEditors].po_Instance = NULL; \
		mi_TotalEditors++; \
	}

/*
 =======================================================================================================================
    Aim:    Call that function when an atomic LONG operation is performed. This this dispatch some messages to avoid
            windows to be out...
 =======================================================================================================================
 */
_inline_ void EDI_DispatchMSG(void)
{
	/*~~~~*/
	MSG msg;
	/*~~~~*/

	if(::PeekMessage(&msg, 0, 0, 0xFFFFFFFF, PM_REMOVE))
	{
		if((msg.message == WM_PAINT) || (msg.message == WM_NCPAINT))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	/* To force wait cursor if it is currently set (cause peekmessage remove it) */
	AfxGetApp()->DoWaitCursor(1);
	AfxGetApp()->DoWaitCursor(-1);
}

/*$4
 ***********************************************************************************************************************
    For link control.
 ***********************************************************************************************************************
 */

#define EDI_C_MaxLenPassWord	16

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	EDI_cl_BaseView;
class	EDI_cl_LeftView;
class	EDI_cl_RightView;
class	EDI_cl_BottomView;
class	EDI_cl_BaseFrame;
class	EDI_cl_LinkControlDialog;
class	EDI_cl_ActionList;
class	EDI_cl_Action;
class	EMEN_cl_FrameList;
class	EMEN_cl_FrameIn;
class	EMEN_cl_Menu;

class EDI_cl_MainFrame : public CFrameWnd
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDI_cl_MainFrame(void);
	~EDI_cl_MainFrame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:

	/* Font for dynamic controls */
	CFont				mo_Fnt;
	CFont				mo_Fnt1;
	CFont				mo_Fnt2;
	HICON				mh_MainIcon;

	/* Inside main frame */
	EDI_cl_Splitter		mo_BigSplitter;

	/* Splitters */
	EDI_cl_Splitter		mo_Splitter;
	EDI_cl_Splitter		mo_Splitter1;
	EDI_cl_Splitter		mo_Splitter2;

	/* The views */
	EDI_cl_BaseView		*mpo_MenuView;
	EDI_cl_BaseView		*mpo_LeftView;
	EDI_cl_BaseView		*mpo_RightView;
	EDI_cl_BaseView		*mpo_CornerView;
	EDI_cl_BaseView		*mpo_BottomView;
	EDI_cl_BaseView		*mpo_MaxView;
	EDI_cl_BaseFrame	*mpo_PipeFrame;

	/* The eventual top menu */
	CToolBar			*mpo_MenuBar;
	EMEN_cl_Menu		*mpo_Menu;

	/* Status bar */
	CStatusBar			mo_Status;

	/* List of actions */
	EDI_cl_ActionList	*mpo_Actions;
	EMEN_cl_FrameIn		*mpo_MenuFrame;

	/* For editors */
	int					mi_TotalEditors;
	EDI_tdst_DefEdit	mast_ListOfEditors[EDI_C_MaxEditors];

	/* Image list for files */
	CImageList			mo_FileImageList;
	CImageList			mo_FileImageList1;

	/* To lock window update */
	int					mi_NumLockDisplay;
	int					mai_LockCount[50];
	CWnd				*mapo_LockWnd[50];

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    All action lists
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	CList<EDI_cl_ActionList *, EDI_cl_ActionList *> mo_ActionList;
	CList<ULONG, ULONG>								mo_TrackPopupMenuList;	/* For popup menu */
	CList<EDI_cl_Action *, EDI_cl_Action *>			mo_TrackPopupMenuActionList;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Structure that is saved to .ini file
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	struct
	{
		char	asz_CurrentDeskName[BIG_C_MaxLenName];

		BOOL	b_LeftFrameVisible;
		int		i_LeftFrameSize;
		int		i_ActionSet;
		int		i_CountMenu;
		BOOL	b_TopMenu;

		BOOL	b_ActivateUniverse;
		UINT	ui_HeightMenu;
		UINT	ui_NumOpen;
		RECT	st1;
		BOOL	b_IntAsHex;
		POINT	st2;
		int		i_TopInMenu;							/* Position of toolbar in topmenu */

		BOOL	b_LinkControlON;						/* Is link control ON or OFF */
		char	asz_CurrentRefFile[L_MAX_PATH];
		char	asz_CurrentVSSFile[L_MAX_PATH];
		char	asz_CurrentUserName[BIG_C_MaxLenUserName];
		char	asz_UserPassWord[EDI_C_MaxLenPassWord]; /* Current user password */

		struct
		{
			BOOL		b_SaveWhenExit; /* Save workspace when exit app */
			BOOL		b_AskExit;		/* Request when exit */
			COLORREF	x_ColRunEngine; /* Color of bar when engine is running */
		} mst_Options;

		BOOL	b_Synchro;

		LONG	al_JoyCalibration[8];
		int		mi_JoyType;
		LONG	al_JoyCalibration1[8];
	} mst_Ini;

	BOOL				mi_VssCanWrite;
	EDI_tdst_Desktop	mst_Desktop;
	L_time_t			x_TimeVSS;
	L_time_t			x_TimeREF;

	DWORD				mdw_VssTaskID;

#ifdef JADEFUSION
	BOOL	ab_MtlFeatures[EDI_MTL_MAX_FEATURES];
#endif

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	void				CheckInTask(char *, char *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Structure that is saved in external ini file
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	struct
	{
		char			asz_DataBigFileName[L_MAX_PATH];
		WINDOWPLACEMENT st_Placement;
		BOOL			b_SynchroEditorsData;
		char			asz_PathHelp[L_MAX_PATH];
	} mst_ExternIni;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	virtual BOOL	OnCreateClient(LPCREATESTRUCT, CCreateContext *);
	virtual LRESULT WindowProc(UINT, WPARAM, LPARAM);
	virtual LRESULT DefWindowProc(UINT, WPARAM, LPARAM);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void				SynchronizeEditorsDataImport(char *);
	void				SynchronizeEditorsData(BOOL);

	void				OneActionKeyHasChanged(void)	{ };
	int					MessageBox(char *, char *, UINT);

	BOOL				b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
	void				OnActionUI(ULONG, CString &, CString &);
	void				OnAction(ULONG);
	UINT				ui_OnActionState(ULONG);
	UINT				ui_ActionFillDynamic(EDI_cl_ConfigList *, POSITION);
	int					i_IsItVarAction(ULONG, EVAV_cl_View *);
	EDI_cl_Action		*po_ActionGetSetDynKey(EDI_cl_ActionList *, EDI_cl_Action *);
	void				OnToolBarCommand(UINT);
	void				OnToolBarCommandUI(UINT, CCmdUI *);

	void				InitPopupMenuAction(EDI_cl_BaseFrame *, EMEN_cl_SubMenu *);
	BOOL				AddPopupMenuAction
						(
							EDI_cl_BaseFrame *,
							EMEN_cl_SubMenu *,
							ULONG,
							BOOL _b_Custom = FALSE,
							char * = NULL,
							UINT _ui_State = -1
						);
	int					TrackPopupMenuAction(EDI_cl_BaseFrame *, CPoint, EMEN_cl_SubMenu *);

	EDI_cl_BaseFrame	*po_EditorFromUserID(UINT);

	void				RegisterEditors(void);

	void				LockDisplay(CWnd *);
	void				UnlockDisplay(CWnd *, CRect * = NULL, BOOL _b_Update = TRUE);
	void				LockWindowUpdate(CWnd *);

	void				BaseLoadIni(char *, UCHAR *, int);
	void				BaseSaveIni(char *, UCHAR *, int);
	void				ReinitIni(void);
	void				LoadIni(void);
	void				SaveIni(void);

	void				UpdateMainFrameTitle(void);
	void				OpenProject(char *);
	BOOL				b_CloseProject(BOOL);

	void				BeforeEngine(void);
	void				AfterEngine(void);
	void				OneTrameEnding(void);
	void				RefreshCheckDlg(void);

	EDI_cl_BaseView		*po_GetViewWithNum(int);
	int					i_GetNumWithView(EDI_cl_BaseView *);
	void				FatHasChanged(void);
	void				DataHasChanged(void);

	BOOL				b_EditKey(USHORT);

	EDI_cl_BaseFrame	*po_GetEditorByType(int, int);

	void				SendMessageToEditors(ULONG, ULONG, ULONG);
	void				SendActionToEditors(ULONG);
	void				SendMessageToLinks(EDI_cl_BaseFrame *, ULONG, ULONG, ULONG);
	EDI_cl_BaseFrame	*po_EditorUnderPoint(CPoint &, EDI_cl_BaseView **);
	EDI_cl_BaseView		*po_ViewUnderPoint(CPoint &);

	void				BeginDragDrop(CPoint, CWnd *, EDI_cl_BaseFrame *, int _i_Type = EDI_DD_File);
	BOOL				b_MoveDragDrop(CPoint);
	void				EndDragDrop(CPoint);
	void				CancelDragDrop(void);

	void				EditorMenu(CMenu *, EDI_cl_BaseFrame *, int, int _i_Of = 101, int _i_Param = 0);

	EDI_cl_BaseFrame	*po_GetEditorByMenu(int, int _i_Of = 101);

	void				SizePanesDialogBar(EDI_cl_BaseFrame *);
	void				OnHistory(void);
	void				AddHistoryFile(EDI_cl_BaseFrame *, BIG_KEY);

	void				OnProjectClose(void);

	void				Universe_SetAIKey(void);
	void				Universe_ReinitEngine(void);
	void				Universe_Close(void);

	void				InitDesktop(EDI_tdst_Desktop *);
	void				LoadDesktop(char *, EDI_tdst_Desktop *);
	void				SaveCurrentDesktop(void);
	void				DesktopTreat(void);
	void				ChangeDesktop(char *);
	void				NewDesktop(void);

	void				OpenWorkspace(void);
	BOOL				b_CloseWorkspace(BOOL);
	void				SaveWorkspace(void);

	void				RestoreMenuFrames(void);
	void				ForceActionConfig(int);
	void				ForceMaximised(EDI_cl_BaseView *);
	void				SwapMaximised(EDI_cl_BaseView *);
	void				OnDesktopEqualize(void);
	void				OnDesktopFill(void);

	BOOL				b_UndoCheckOut(BIG_KEY, char *, BOOL, BOOL _b_Force = TRUE);
	void				CheckOutFile(char *, char *);
	int					GetLatestVersionFile(char *, char *);
	void				UndoCheckOutFile(char *, char *);
	void				CheckInFile(char *, char *);
	void				CheckInFileNoOut(char *, char *);
	void				CheckOutDir(char *);
	void				LocalCheckOutDir(char *, BOOL = TRUE);
	void				GetLatestVersionDir(char *);
	void				UndoCheckOutDir(char *);
	void				CheckInDir(char *);
	void				CheckInDirNoOut(char *);
	void				VssFileChanged(BOOL = FALSE, BOOL = FALSE);
	void				REFFileChanged(BOOL = FALSE, BOOL = FALSE);
	void				CloseVssFile(void);
	void				DestroyVssFile(void);
	void				CloseRefFile(void);
	void				DestroyRefFile(void);
	void				ShowCheckOut(void);
	void				VssAddedFiles(void);
	void				UpdateOwnerPath(char *, char *, char *);
	BOOL				b_CheckOwnerDir(BIG_INDEX);
	void				VssUpdateStatus(void);

	void				ComputeLockedFileName(char *, char *);
	void				ComputeLockedFileOpen(char *, char *);
	BOOL				b_LockBigFile();
	void				UnLockBigFile();
	void				OnLinkControl(void);
	void				OnLinkControlBeg(void);

	void				ForceClose(void);

	void				FlashJade(void);
	void				FlashJade1(void);
	void				DrawTransparentBmp(UINT, CDC *, int, int, int, int, int, int, COLORREF);
	ULONG				u4_Interpol2PackedColor(ULONG, ULONG, float);
	int					i_GetIconImage(char *, char *, int _i_Size = 16);

	void				SaveExternIni(void);

	void				Joystick_Calibrate(void);
	void				MemoryStats(void);
	void				OnSetLanguage(void);
	void				TextureStats(void);
	void				RefStats(void);
	void				UnrefStats(void);

	void				SendMsg(void);
	void				DisplayMsg(void);
#if MONTREAL_SPECIFIC
    BOOL                editorVersionCheck(void);
	void				editorKeyCheck(void);
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */
#if _MSC_VER == 1200 
#define LOCAL_HTASK HTASK
#else
#define LOCAL_HTASK DWORD
#endif


public:
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnGetMinMaxInfo(MINMAXINFO FAR *);
	afx_msg void	OnClose(void);
	afx_msg void	OnExit(void);
	afx_msg void	OnProjectNew(void);
	afx_msg void	OnProjectOpen(void);
	afx_msg void	OnActivateApp(BOOL, LOCAL_HTASK);
	DECLARE_MESSAGE_MAP()
};
#endif	/* ACTIVE_EDITORS */
