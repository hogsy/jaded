/*$T EVEscroll.h GC! 1.081 04/03/00 12:45:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASE/CLIbrary/CLIwin.h"
#include "EDIbaseframe.h"
#include "EVEtrack.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	EEVE_cl_Frame;
class	EEVE_cl_Inside;
class EEVE_cl_Scroll : public CScrollView
{
	DECLARE_DYNCREATE(EEVE_cl_Scroll)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EEVE_cl_Scroll(void);
	~EEVE_cl_Scroll(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MEMBERS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EEVE_cl_Frame	*mpo_Parent;
	EEVE_cl_Inside	*mpo_Inside;
	BOOL			mb_PaneMode;
	BOOL			mb_ZoomMode;
	CPoint			mo_PanePt;
	CRect			mo_ZoomRect;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	OnDraw(class CDC *);
	BOOL	OnScroll(UINT, UINT, BOOL = TRUE);
	void	PlaceTracks(BOOL = TRUE);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnMouseMove(UINT, CPoint);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
	afx_msg BOOL	OnMouseWheel(UINT, short, CPoint);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
