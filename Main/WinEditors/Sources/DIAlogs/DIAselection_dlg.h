/*$T DIAselection_dlg.h GC! 1.081 04/27/00 14:45:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "F3Dframe/F3Dview.h"
#include "BASe/BAStypes.h"
#include "HScrollListBox.h"

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define EDIA_Selection_SortAlpha	0
#define EDIA_Selection_SortIdentity 1
#define EDIA_Selection_SortGroType	2

class EDIA_cl_SelectionDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_SelectionDialog(F3D_cl_View *);
	~EDIA_cl_SelectionDialog();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_View				*mpo_View;
	struct WOR_tdst_World_	*mpst_World;
	LONG					ml_SortType;
	ULONG					mul_IFFilter;
	ULONG					mul_GroFilter;
	ULONG					mul_InvisibleFilter;
	char					*mdc_Sel;
	BOOL					mb_ShowHidden;
	BOOL					mb_ShowNotHidden;
	BOOL					mb_ShowBones;
	BOOL					mb_ShowPrefab;
	BOOL					mb_ShowPrefabSon;
	BOOL					mb_LockUpdate;
	BOOL					mb_ShowAct;
	BOOL					mb_ShowNotAct;
	BOOL					mb_ShowSpg2GridGen;
	BOOL					mb_ShowVis;
	BOOL					mb_ShowNotVis;
	BOOL					mb_DrawMask_DrawIt;
	BOOL					mb_DrawMask_NotDrawIt;
	BOOL					mb_NotInSecto;
    CBitmap                 mao_SelectionButtonBmp[2];
	BOOL					mb_Merge;
	BOOL					mb_NotMerge;
	ULONG					mul_FilterPostIt;
	CList<CString, CString>	mo_LstPrefab_Str;
	CList<BIG_KEY, BIG_KEY> mo_LstPrefab_Key;
	CString					mo_GaoF;
	CString					mo_GaoFregexp[10];
	CHScrollListBox			mo_ListBox;


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Functions
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	UpdateIFCheck(void);
	void	UpdateGroCheck(void);
	void	UpdateInvisibleCheck(void);
	int		AddGAO(OBJ_tdst_GameObject *);
	void	UpdateGroup(void);
	void	UpdateGos(void);
	void	UpdateNetworks(void);
	void	ClearList(void);
	void	UpdateList(BOOL = TRUE);
	BOOL	GaoNameIsSkipped(CString & str);

    void    UpdateSelectionButton(void);
	void	UpdateIFFilter(void);
	void	UpdateGroFilter(void);
	void	UpdateInvisibleFilter(void);

	ULONG	ul_GAOFromPt(POINT);

	BOOL	OnInitDialog(void);
	BOOL	PreTranslateMessage(MSG *);
	void	DoDataExchange(CDataExchange *);

	void	MenuPostIt(void);
	void	FillPostIt(void);
	void	MenuNetwork(void);

	void	AddNameToList(OBJ_tdst_GameObject *pst_GO);
	void	RemoveNameFromList(OBJ_tdst_GameObject *pst_GO);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnIFCheckClick(UINT);
	afx_msg void	OnGroCheckClick(UINT);
	afx_msg void	OnInvisibleFCheckClick(UINT);
	afx_msg void	OnSelChange(void);
	afx_msg void	OnBtNoneClicked(void);
	afx_msg void	OnBtAllClicked(void);
	afx_msg void	OnBtInvertClicked(void);
	afx_msg void	OnListDblClk(void);
	afx_msg void	OnBtInvers1(void);
	afx_msg void	OnBtInvers2(void);
	afx_msg void	OnBtInvers3(void);
	afx_msg void	OnBtClear1(void);
	afx_msg void	OnBtClear2(void);
	afx_msg void	OnBtClear3(void);
	afx_msg void	OnHide(void);
	afx_msg void	OnShow(void);
	afx_msg void	OnWired(void);
	afx_msg void	OnNoWired(void);
	afx_msg void	OnHidden(void);
	afx_msg void	OnNotHidden(void);
	afx_msg void	OnBones(void);
	afx_msg void	OnShowAct(void);
	afx_msg void	OnShowNotAct(void);
	afx_msg void	OnShowVis(void);
	afx_msg void	OnShowNotVis(void);
	afx_msg void	OnUpdateGrp(void);

	afx_msg void	OnCloseSel(void);
	afx_msg void	OnMinimize(void);
    afx_msg void	OnSelectionType(void );
	afx_msg void	OnToolBox(void);
	afx_msg void	OnNewGroup(BOOL = TRUE);
	afx_msg void	OnNewGos(BOOL = TRUE);
	afx_msg void	OnGroupSel(void);
	afx_msg void	OnGosSel(void);
	afx_msg void	OnUpdateGroup(void);
	afx_msg void	OnAdd2Group(void);
	afx_msg void	OnSelChangeGroups(void);
	afx_msg void	OnClearGos(void);

	afx_msg void	OnNetSel(void);
	afx_msg void	OnNetDblClk(void);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnChangePane(NMHDR *, LRESULT *);
	afx_msg BOOL	OnEraseBkgnd(CDC *);

	afx_msg void	OnDrawIt(void);
	afx_msg void	OnNotDrawIt(void);
	afx_msg void	OnMerged(void);
	afx_msg void	OnNotMerged(void);

	afx_msg void	OnAIModels(void);
	afx_msg void	OnPostItDblClk(void);
	afx_msg void	OnPrefab(void);
	afx_msg void	OnSpg2Grid(void);
	afx_msg void	OnPrefabSon(void);

	afx_msg void	OnGaoF(void);
	afx_msg void	OnNotInSecto(void);

	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */