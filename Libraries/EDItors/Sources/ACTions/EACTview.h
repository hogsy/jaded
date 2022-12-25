/*$T EACTview.h GC! 1.098 10/11/00 16:27:08 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __EACTVIEW_H__
#define __EACTVIEW_H__
#include "BASe/BAStypes.h"

#pragma once
#ifdef ACTIVE_EDITORS
class	EGRO_cl_Frame;
class	EOUT_cl_Frame;
class	F3D_cl_View;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

class EACT_cl_View : public CFormView
{
	DECLARE_DYNCREATE(EACT_cl_View)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EACT_cl_View(void);
	~EACT_cl_View();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EACT_cl_Frame	*mpo_Editor;
	BOOL			mb_LockTrans;
	int				mi_SelWhenTrans;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Functions
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void				CloseAll(void);
	void				CloseAction(void);
	void				CloseActionItem(void);

	BOOL				PreTranslateMessage(MSG *);

	void				SetActionKit(void);
	void				SetActionByIndex(BIG_INDEX, BOOL = FALSE);
	void				SetAction(void);
	void				SetActionItemByIndex(int);
	void				SetActionItem(void);
	void				SetAnim(BIG_INDEX);
	void				SetShape(BIG_INDEX);
	ACT_tdst_Transition *GetTrans(int);
	void				SetTrans(void);
	void				EnableTrans(void);

	void				OnNewAction(void);
	void				OnNewActionItem(void);

	void				HasBeenMove(int, int);
	void				RenameAction(void);

	BOOL				Create
						(
							LPCTSTR lpszClassName,
							LPCTSTR lpszWindowName,
							DWORD	dwStyle,
							const RECT &,
							CWnd *,
							UINT nID
						);
	void				MoveActionList(void);
	BOOL				OnScroll(UINT, UINT, BOOL);
	void				CommonCopyFrom(BOOL);
	void				DeleteAllTrans(void);
	void				OnTransActNone(void);
	void				OnCopySameAs(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Message headers
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnActionKitSelect(NMHDR *, LRESULT *);
	afx_msg void	OnActionItemSelect(void);
	afx_msg void	OnModeChange(void);
	afx_msg void	OnFreqChange(void);
	afx_msg void	OnLoopNumChange(void);
	afx_msg void	OnZoneCheckClick(UINT);
	afx_msg void	OnARAMClick();
	afx_msg void	OnCBitCheckClick(UINT);
	afx_msg void	OnIgnoreGravity(void);
	afx_msg void	OnDevelopped(void);
	afx_msg void	OnIgnoreTraction(void);
	afx_msg void	OnIgnoreStream(void);
	afx_msg void	OnIgnoreVTrans(void);
	afx_msg void	OnUpActionItem(void);
	afx_msg void	OnDownActionItem(void);
	afx_msg void	OnLoopChange(void);
	afx_msg void	OnUpAction(void);
	afx_msg void	OnDownAction(void);
	afx_msg void	OnHoleAction(void);
	afx_msg void	OnBrowseAnim(void);
	afx_msg void	OnBrowseShape(void);
	afx_msg void	OnLockTrans(void);
	afx_msg void	OnTransActChange(void);
	afx_msg void	OnTransBlendProgressive(void);
	afx_msg void	OnTransBlendProgressiveInv(void);
	afx_msg void	OnBlendTimeChange(void);
	afx_msg void	OnTransMe(void);
	afx_msg void	OnCopyFromActChange(void);
	afx_msg void	OnCopyFrom(void);
	afx_msg void	OnCopyFromOver(void);
	afx_msg void	OnKillTo(void);
	afx_msg void	OnViewTo(void);
	afx_msg void	OnUseTrans(void);
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */

#endif /* __EACTVIEW_H__ */
