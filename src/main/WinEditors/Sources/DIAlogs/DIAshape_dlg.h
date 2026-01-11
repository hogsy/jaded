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

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

class EDIA_cl_ShapeDialog : public CFormView
{
    DECLARE_DYNCREATE(EDIA_cl_ShapeDialog)

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_ShapeDialog(EDIA_cl_ToolBoxDialog *);
    EDIA_cl_ShapeDialog(void);
	~EDIA_cl_ShapeDialog();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    EDIA_cl_ToolBoxDialog	*mpo_ToolBox;
	F3D_cl_View				*mpo_View;
	OBJ_tdst_GameObject		*mpst_Object;
	UCHAR					*mdc_Visual;
	UCHAR					*mdc_Canal;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Functions
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
    BOOL	        Create(LPCTSTR, LPCTSTR, DWORD, const RECT &, CWnd *);
	void			UpdateList(BOOL);
	BOOL			CheckShape();
	void			Initialize();
	BOOL			OnInitDialog(void);


public:
	afx_msg void	OnSelChange(void);
	afx_msg void	OnUpdateVisu(void);
	afx_msg void	OnUpdateCanal(void);
	afx_msg void	OnSave(void);
	afx_msg void	OnReset(void);
	afx_msg void	OnClear(void);
	afx_msg void	OnInvisible(void);
    afx_msg void	OnSize(UINT, int, int);

	DECLARE_MESSAGE_MAP()

public:
};

#endif /* ACTIVE_EDITORS */
