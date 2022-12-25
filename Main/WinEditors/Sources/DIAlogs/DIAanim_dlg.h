/*$T DIAanim_dlg.h GC! 1.086 07/03/00 10:52:48 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "F3Dframe/F3Dview.h"

class EDIA_cl_AnimDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_AnimDialog(F3D_cl_View *, struct OBJ_tdst_GameObject_ *);
	~EDIA_cl_AnimDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_View					*mpo_View;
	struct OBJ_tdst_GameObject_ *mpst_GAO;
	int							mi_FirstFrame;
	BOOL						mb_BegSel;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	OneTrameEnding(void);
	void	Reset(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    afx_msg void    OnGeneral(void);
    afx_msg void    OnPlay(void);
	afx_msg void	OnMouseMove(UINT, CPoint pt);
	afx_msg void	OnRButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonDown(UINT, CPoint);
	afx_msg void	OnLButtonUp(UINT, CPoint);
	afx_msg BOOL	OnEraseBkgnd(CDC *);
    DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
