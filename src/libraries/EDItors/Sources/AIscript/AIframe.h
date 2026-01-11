/*$T AIframe.h GC!1.71 01/31/00 09:30:14 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"
#include "AIframe_act.h"
#include "BIGfiles/BIGdefs.h"
#include "Compiler/AICompile.h"
#include "DIAlogs/DIAfindai_dlg.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define EAI_C_MaxColors         20
#define EAI_C_ColorBkGnd        0
#define EAI_C_ColorDefault      1
#define EAI_C_ColorType         2
#define EAI_C_ColorKeyword      3
#define EAI_C_ColorFunction     4
#define EAI_C_ColorComment      5
#define EAI_C_ColorPP           6
#define EAI_C_ColorField        7
#define EAI_C_ColorReserved     8
#define EAI_C_ColorString       9
#define EAI_C_ColorConstant     10
#define EAI_C_ColorPPConstants  11
#define EAI_C_ColorPPMacros     12
#define EAI_C_ColorProcedures	13

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Structure to define a type
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  EIA_tdst_Type_
{
    short   w_ID;
    char    *psz_Name;
    int     i_Size;
} EIA_tdst_Type;

/*
 -----------------------------------------------------------------------------------------------------------------------
    To define a constant
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  EIA_tdst_Constant_
{
    char    *psz_Name;
    int     i_Type;
    char    *psz_Value;
} EIA_tdst_Constant;

/*
 -----------------------------------------------------------------------------------------------------------------------
    To define a keyword
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  EIA_tdst_Keyword_
{
    short   w_ID;
    char    *psz_Name;
} EIA_tdst_Keyword;

/*
 -----------------------------------------------------------------------------------------------------------------------
    To define a function
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  EIA_tdst_Function_
{
	int		i_EvtFlags;
    short   w_ID;
    char    *psz_Name;
    char    *psz_CName;
    int     i_ReturnType;
    char    *psz_Return;
    char    *psz_Types;
    int     i_NumPars;
    char    c_IsUltra;
    int     ai_TypePars[100];
    int     ai_PTypePars[100];
} EIA_tdst_Function;
extern int EIA_gi_NumFct;

/*
 -----------------------------------------------------------------------------------------------------------------------
    To define a field
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct  EIA_tdst_Field_
{
    short   w_ID;
    char    *psz_Name;
    int     i_ReturnType;
    int     i_Type;
} EIA_tdst_Field;

extern EIA_tdst_Type        EAI_gast_Types[];
extern EIA_tdst_Constant    EAI_gast_Constants[];
extern EIA_tdst_Keyword     EAI_gast_Keywords[];
extern EIA_tdst_Function    EAI_gast_Functions[];
extern EIA_tdst_Field       EAI_gast_Fields[];

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class                       EAI_cl_View;
class                       EAI_cl_LeftView;
class                       EAI_cl_LeftFrame;
class                       EAI_cl_InsideFrame;
class                       EAI_cl_Pane;

class EAI_cl_Frame : public EDI_cl_BaseFrame
{
    DECLARE_DYNCREATE(EAI_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EAI_cl_Frame (void);
    ~EAI_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    struct
    {
        COLORREF    ax_Colors[EAI_C_MaxColors];
        char        asz_FaceName[128];
        int         i_FontSize;
        int         i_TabStop;
        int         i_XCompletion;
        int         i_YCompletion;
        int         i_WidthSplitter;
        int         i_WidthCol1;
        int         i_WidthCol2;
        int         i_SortedCol;
        BOOL        b_AutoSave;
        int         i_HeightSplitter1;
        BOOL        b_BreakEnable;
		BOOL		b_WarningHigh;
		BOOL		b_LookDate;
    } mst_Ini;

	// - NOTE - (Perforce option for AI programmers)
	// Don't close world before a perforce action
	BOOL											mb_P4CloseWorld;

    EAI_cl_View                                     *mpo_Edit;
    EAI_cl_LeftView                                 *mpo_LeftView;
    CSplitterWnd                                    *mpo_Splitter;
    EAI_cl_LeftFrame                                *mpo_ListAI;
    EAI_cl_InsideFrame                              *mpo_InsideWnd;

    BIG_INDEX                                       mul_CurrentEditFile;        /* The current file */
    BIG_INDEX                                       mul_CurrentEditModel;       /* The current model */
	CMapPtrToPtr									mo_ModelDateList;
    BIG_INDEX                                       mul_CurrentEngineModel;
    BIG_INDEX                                       mul_CurrentEditInstance;    /* The current instance */
    BOOL                                            mb_CanReload;
    BOOL                                            mb_ListMode;
    BOOL                                            mb_CanAskVss;
    BOOL                                            mb_HasTouchedVars;
    BOOL                                            mb_CompileAll;
	BOOL											mb_CompileDep;
	BOOL											mb_UpdateOwner;
	BOOL											mb_RecurseError;
	BOOL											mb_RefreshLeft;

	BOOL											mb_LockHistory;
	CList<BIG_INDEX, BIG_INDEX>						mo_History;
	CList<CHARRANGE, CHARRANGE>						mo_HistoryCR;
	CList<int, int>									mo_HistoryMarks[500];

    /* Known keywords */
    CMapStringToPtr                                 mo_TypeList;
    CMapStringToPtr                                 mo_KeywordList;
    CMapStringToPtr                                 mo_FunctionList;
    CMapStringToPtr                                 mo_FieldList;
    CMapStringToPtr                                 mo_PPList;
    CMapStringToPtr                                 mo_ReservedList;
    CMapStringToPtr                                 mo_ConstantList;

    /* The compiler */
    EAI_cl_Compiler                                 mo_Compiler;

    /* Compilers for references */
    CMapPtrToPtr                                    mo_RefCompilers;

    /* Address of engine compiled instance or model */
    AI_tdst_Instance                                *mpst_Instance;
    AI_tdst_Model                                   *mpst_Model;

    /* Top pane */
    EAI_cl_Pane                                     *mpo_Pane1;
    EAI_cl_Pane                                     *mpo_Pane2;
    EAI_cl_Pane                                     *mpo_Pane3;

    /* Find dialog */
    EDIA_cl_FindAIDialog                            *mpo_FindDlg;
    CString                                         mo_SearchString;

	ULONG											mul_NumberAIModels;
	ULONG											mul_CurrentAIModel;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    int     i_ComputeParamsTypes(char *, int *, int *);
    void    DisplayPaneNames(void);
    void    Load(BIG_INDEX);

    BOOL    b_IsModelFile(char *);
    BOOL    b_IsInstanceFile(char *);
    BOOL    b_IsAIFile(BIG_INDEX);

    void    UndoCheckOut(void);
    void    CheckOut(void);
    void    CheckIn(void);
    void    CheckInOut(void);
    void    GetLatest(void);
	void	UpdateOwner(void);

	void	OnPerforceEditModel();
	void	OnPerforceCheckInModel();
	void	OnPerforceCheckInOutModel();
	void	OnPerforceRevertModel();
	void	OnPerforceSyncModel();

    BOOL    SetCurrentInstance(BIG_INDEX);
    void    InitListAI(void);
    BOOL    SetCurrentModel(BIG_INDEX);
    void    OnNewModel(void);
    void    OnNewFunction(void);
	void	OnNewProcList(void);
    void    OnNewVars(void);
    void    OnNewGlobalFunction(void);
    void    OnDeleteFile(void);
    void    OnRenameFile(void);
    void    OnAddFunction(void);

    void    OnFind(void);
    void    OnFindNext(void);
    void    OnReplace(void);
	void	OnMatch(void);

	void	SendP4Message( ULONG ulMessage, EAI_cl_LeftFrame* pLeftFrame, ULONG ulCurrentEditFile );


	void	RefreshDialogBar(void);
    void    SaveAllInstances(void);
    void    Resolve(EAI_cl_Compiler *);
    void    UpdateEngineNodes(EAI_cl_Compiler *, BIG_INDEX);
    void    UpdateEngineVars(EAI_cl_Compiler *);
    void    ResetRefCompilers(void);
    int     i_GetVarOfModel(BIG_KEY, char *, BOOL _b_DirectModel = FALSE);
    BOOL    OnCompileVarsOfModel(EAI_cl_Compiler *, BIG_INDEX, BOOL = FALSE);
    BOOL    OnCompileFile(EAI_cl_Compiler *, BIG_INDEX, BIG_INDEX, char *);
    void    OnCompileCurrentFile(void);
    BOOL    OnCompileCurrentModel(void);
    void    OnCompileCurrentModelDep(void);
    void    GenerateEngineModel(BIG_INDEX);
    BOOL    OnCompileAllModels(BIG_INDEX, BOOL, ULONG (*SeprogressPos) (float F01, char *));
    void    OnCompileAllModels(void);

    void    OnCreateInstance(void);
    void    OnSaveInstance(void);
    void    OnReinitInstance(void);

    BOOL    b_CanClose(void);
    BOOL    OnClose(BOOL _b_Forced = FALSE, BOOL _b_All = TRUE);
    void    OnSave(void);

    ULONG   ChangeLine(ULONG, BIG_INDEX, AI_tdst_Node **);
    void    OnBreakPoint(void);
    void    SetEngineBreakPointAtLine(AI_tdst_Function *, LONG, BOOL);
    void    SetEngineBreakPointAtLine(SCR_tt_ProcedureList *, LONG, BOOL);
    void    SetEngineBreakPoint(void);
    void    OnOneStep(BOOL);
    void    OnSysBreak(void);
    void    ConnectToEngine(void);
	void	OnDeleteAllBkp(void);

    BOOL    SelFile(BIG_INDEX);
    BOOL    SelData(BIG_INDEX);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    char    *psz_OnActionGetBase(void)  { return EAI_asz_ActionBase; };
    void    OnAction(ULONG);
    BOOL    b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);
    UINT    ui_OnActionState(ULONG);
    void    OnActionUI(ULONG, CString &, CString &);
    BOOL    b_KnowsKey(USHORT);
    int     i_IsItVarAction(ULONG, EVAV_cl_View *);
    BOOL    b_AcceptToCloseProject(void);
    void    CloseProject(void);
    BOOL    b_CanActivate(void);
    void    ReinitIni(void);
    void    LoadIni(void);
    void    TreatIni(void);
    void    SaveIni(void);
    void    ForceRefresh(void);
    int     i_OnMessage(ULONG, ULONG, ULONG);
    void    OnRealIdle(void);
	void	ConnectToLine(int);
    void    AfterEngine(void);
    void    OneTrameEnding(void);
	void	SetCompile1(void);
	void	SaveColors(void);
	void	LoadColors(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    afx_msg int     OnCreate(LPCREATESTRUCT);
    afx_msg void    OnSize(UINT, int, int);
    DECLARE_MESSAGE_MAP()

private:
	void	RunCommandOnFiles( int nCommand );
};
#endif /* ACTIVE_EDITORS */
