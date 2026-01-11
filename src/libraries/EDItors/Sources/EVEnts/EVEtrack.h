/*$T EVEtrack.h GC! 1.080 03/29/00 11:20:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EVEevent.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define EEVE_C_CXEvent		0	/* Min width of event */
#define EEVE_C_CXBorder		5
#define EEVE_C_CXBorder2	5
class	EEVE_cl_Frame;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class EEVE_cl_Track : public CWnd
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EEVE_cl_Track(void);
	~EEVE_cl_Track(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MEMBERS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EEVE_cl_Frame								*mpo_Frame;
	CArray<EEVE_cl_Event *, EEVE_cl_Event *>	mo_ListEvents;
	BOOL										mb_Visible;
	EEVE_cl_Event								*mpo_HitTest;
	int											mi_HitTest;
	CPoint										mo_HitPt;
	CPoint										mo_PanePt;
	BOOL										mb_MoveSize;
	BOOL										mb_SelMode;
	CRect										mo_TrackTime;
	CRect										mo_ZoomRect;
	int											mi_DecSize;
	int											mi_NumSelected;

	struct EVE_tdst_Track_						*mpst_Track;
	struct EVE_tdst_Params_						*mpst_Param;


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	Create(CWnd *);
	int		HitTest(CPoint, EEVE_cl_Event **, BOOL = FALSE);
	void	CloseTrack(BOOL _b_Refresh = TRUE );
	void	SetTrack(struct EVE_tdst_Track_ *);
	void	FillJustBar(void);
	void	FillBar(void);
	void	DrawBars(void);
	BOOL	SnapOtherTracks(CPoint *);
	BOOL	UnselectAll(void);
	void	SelectCurrentEvent(BOOL = FALSE);
	void	MoveRunningBox(BOOL, float);
	void	Select(void);
	void	Unselect(void);
	void	UnselectTracks(void);
	void	RoundTo(float);
	BOOL	ToggleExpand(CPoint &);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnPaint(void);
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnLButtonDblClk(UINT, CPoint);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnMButtonDown(UINT, CPoint);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
