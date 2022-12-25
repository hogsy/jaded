/*$T DIAtoolbox_dlg.h GC! 1.100 06/15/01 12:26:47 */


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

#define DIA_ToolBox_NumberOfView	3

class	EOUT_cl_Frame;
class EDIA_cl_ToolBoxDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_ToolBoxDialog(F3D_cl_View *);
	~EDIA_cl_ToolBoxDialog();

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	F3D_cl_View					*mpo_View;
	struct OBJ_tdst_GameObject_ *mpst_Gao;
	struct GRO_tdst_Struct_		*mpst_Gro;
	CBitmap						mao_SelectionButtonBmp[2];

	CFormView					*mpo_ToolView[DIA_ToolBox_NumberOfView];

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Functions
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);
	void	SetGro(GRO_tdst_Struct *);
	void	SelectGaoFromPtr(struct OBJ_tdst_GameObject_ *);
	void	SelectGao(ULONG);
	void	OneTrameEnding(void);
	void	Reset(void);
	void	UpdateSelectionButton(void);
	void	UpdateFaceId(int);
	void	FirstDraw(void);
    BOOL    b_CanDrop(POINT);
    void    DropGro( BIG_INDEX );
    void    DropGao( BIG_INDEX );

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnCloseSel(void);
	afx_msg void	OnMinimize(void);
	afx_msg void	OnSelection(void);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnSelchangeTabView(NMHDR *, LRESULT *);
	afx_msg void	OnSelectionType(void);
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
