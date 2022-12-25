/*$T PENCframe.h GC 1.138 10/14/04 14:40:27 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "EDIbaseframe.h"
#include "PENcframe.h"
#include "PENcframe_act.h"

/*$4
 ***********************************************************************************************************************
    CONSTANTS FOR STRING
 ***********************************************************************************************************************
 */

class EPEN_cl_Frame :
	public EDI_cl_BaseFrame
{
	DECLARE_DYNCREATE(EPEN_cl_Frame)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EPEN_cl_Frame(void);
	~EPEN_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	CTreeCtrl		*mpo_TreeView;
	CMapPtrToPtr	IndexDirToTree;
	BOOL			mb_Locked;

	struct
	{
		int i_Version;
	} mst_Ini;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	HTREEITEM	GetTreeItemFromlParam(LONG);
	void		OnCtrlPopup(CPoint);
    //
    void        UndoCheckOutAll(void);
    void        CheckInAll(void);
    void        CheckInOutAll(void);
    //
    void        ShowInBrowerSel(void);
    void        UndoCheckOutSel(void);
    void        CheckInSel(void);
    void        CheckInOutSel(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    INTERFACE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	b_CanBeLinkedToEngine(void)		{ return FALSE; };
	void	CloseProject(void);
	BOOL	b_CanActivate(void);
	char	*psz_OnActionGetBase(void)	{ return EPENC_asz_ActionBase; };
	void	Refresh(void);
	void	OnAction(ULONG);
	BOOL	b_OnActionValidate(ULONG, BOOL _b_Disp = TRUE);

	int		i_OnMessage(ULONG, ULONG, ULONG);
	void	OnToolBarCommand(UINT);
    void	OnToolBarCommandUI(UINT, CCmdUI *);

	UINT	ui_OnActionState(ULONG);
	void	ReinitIni(void);
	void	LoadIni(void);
	void	TreatIni(void);
	void	SaveIni(void);

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	void	AddFile(BIG_INDEX);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage(MSG *);
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnClose(void);
	afx_msg void	OnParentNotify(UINT, LONG);
	afx_msg void	On_TVN_Notify(NMHDR *, LRESULT *);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
