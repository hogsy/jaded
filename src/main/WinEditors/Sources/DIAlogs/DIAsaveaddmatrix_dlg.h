/*$T DIAmorphing_dlg.h GC! 1.081 05/31/00 10:59:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

#include "BASe/BAStypes.h"

class EDIA_cl_SaveAddMatrixDialog : public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_SaveAddMatrixDialog(struct MDF_tdst_Modifier_ *);
	~EDIA_cl_SaveAddMatrixDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	struct MDF_tdst_Modifier_					*mpst_Modifier;
	struct GAO_tdst_ModifierSaveAddMatrix_		*mpst_Data;
	struct GAO_tdst_ModifierSaveAddMatrix_Slot_	*mpst_CurSlot;
    int                                         mi_CurSlot;


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL	OnInitDialog(void);

	void	Slot_FillList(void);
	void	Slot_Display(void);
    void    Slot_Delete(void);
    void    Slot_GetCur(void);
    void    Slot_Select(int);

	void	Gizmo_FillList(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage(MSG *);

	afx_msg void	OnSelChange_SlotList(void);
	afx_msg void	OnButton_SlotRename(void);
	afx_msg void	OnButton_SlotDelete(void);
	afx_msg void	OnButton_SlotNew(void);
    afx_msg void	OnButton_SlotUpdate(void);
    afx_msg void	OnButton_SlotApply(void);

	
	DECLARE_MESSAGE_MAP()
};

#endif /* ACTIVE_EDITORS */
