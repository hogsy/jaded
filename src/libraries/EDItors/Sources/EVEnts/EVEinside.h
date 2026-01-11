/*$T EVEinside.h GC! 1.081 04/05/00 12:19:21 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define EEVE_C_YBarInfo 20
#define EVE_C_YRegle	35
#define EVE_C_YRegleSel 25

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	EEVE_cl_Scroll;
class EEVE_cl_Inside : public CScrollView
{
	DECLARE_DYNCREATE(EEVE_cl_Inside)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EEVE_cl_Inside(void);
	~EEVE_cl_Inside(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MEMBERS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EEVE_cl_Frame	*mpo_Parent;
	EEVE_cl_Scroll	*mpo_Scroll;
	BOOL			mb_SelectMode;
	BOOL			mb_SelTrackMode;
	BOOL			mb_MoveLeft;
	CRect			mo_ZoomRect;
	CPoint			mo_PanePt;
	EEVE_cl_Track	*mpo_LastSelTrack;
	CRect			mo_Marker;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void			OnDraw(class CDC *);
	void			OnDrawRegle(CDC *);
	void			OnDrawMarks(void);
	void			EndZoom(void);
	EEVE_cl_Track	*po_GetTrackWithY(int);
	void			DisplayHideTracks(EEVE_cl_Track *);
	void			DisplayHideCurves(EEVE_cl_Track *);
	void			SelectTrackMode(CPoint);
	void			WhenScroll(int, int);
	void			DrawRealTime(void);
	OBJ_tdst_GameObject *pt_GetGaoFromTrack(EVE_tdst_Track *, CString &);
	int				PtToMark(CPoint);
	UINT			PtToFrame(CPoint);
	void			FrameToPt(UINT, CPoint &);
	BOOL			PreTranslateMessage(MSG *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnSize(UINT, int, int);
	afx_msg int		OnCreate(LPCREATESTRUCT);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonDblClk(UINT, CPoint);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
