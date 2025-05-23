/*$T F3Dframe.h GC! 1.081 04/27/00 15:40:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define WIDTH_SEL		380
#define WIDTH_MINSEL	140
#define WIDTH_MINSCRSEL	140//173

#ifdef JADEFUSION
// NB: Added Light Rejection dialog - from POP5 Jade Editor
#define WIDTH_LIGHTREJECT	240
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	F3D_cl_View;
class	EDI_cl_BaseFrame;
class F3D_cl_Frame : public CFrameWnd
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_Frame(void);
	~F3D_cl_Frame(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_View			*mpo_DisplayView;
	EDI_cl_BaseFrame	*mpo_AssociatedEditor;
	CWnd				*mpo_Parent;
	BOOL				mb_LeftSel;
	BOOL				mb_SelMinimize;
	CRect				mo_PrevRect;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	MyCreate(EDI_cl_BaseFrame *, CWnd *);
	LRESULT DefWindowProc(UINT, WPARAM, LPARAM);
	void	MinMaxSel(void);
	void	UpdatePosSize(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg BOOL	OnEraseBkgnd(CDC*);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
