/*$T DIApostit_dlg.h GC 1.134 04/22/04 10:40:58 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "F3Dframe/F3Dview_postit.h"

class EDIA_cl_PostItDialog :
	public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_PostItDialog(struct WOR_tdst_World_ *, class F3D_cl_View *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_tdst_PostIt			mst_PostIt;
	F3D_tdst_PostIt			*mpt_Org;
	struct WOR_tdst_World_	*mpst_World;
	class F3D_cl_View		*mpo_View;
	BOOL					mb_ReadOnly;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	DoDataExchange(CDataExchange *);
	BOOL	PreTranslateMessage(MSG *);
	BOOL	OnInitDialog(void);
	void	OnOK(void);
	void	OnCancel(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnKillRead(void);
	afx_msg void	OnTo(void);
	afx_msg void	OnPickPos(void);
	afx_msg void	OnZoom(void);
	afx_msg void	OnPublic(void);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
