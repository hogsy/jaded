/*$T DIAlist2_dlg.h GC 1.134 05/14/04 10:32:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "AIinterp/Sources/AIstruct.h"
#include "VAVview/VAVview.h"

#define LIST2_ModeTrigger	1
#define LIST2_ModeText		2

class EDIA_cl_List2Dialog :
	public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_List2Dialog(int);
	~EDIA_cl_List2Dialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	SCR_tt_Trigger				*pt_Trigger;
	TEXT_tdst_Eval				*mp_TextRef;
	int							mi_Mode;
	EVAV_cl_View				*mpo_VarsView;
	EVAV_tdst_VarsViewStruct	mst_VarsViewStruct;
	CList<EVAV_cl_ViewItem *, EVAV_cl_ViewItem *>	mo_ListItems;
	AI_tdst_Message				m_Msg;
	int							mi_TextDisplay;
	

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	
	void	DoDataExchange(CDataExchange *);
	void	OnOK(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	BOOL			PreTranslateMessage( MSG * );
	afx_msg void	OnSelChange(void);
	afx_msg void	OnSelChange2(void);
	afx_msg void	OnSetNull(void);
	afx_msg BOOL	OnInitDialog(void);
	afx_msg void	OnSize(UINT, int, int);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
