/*$T DIAsecto_dlg.h GC 1.139 03/26/04 11:06:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"

class EDIA_cl_SectoDialog :
	public EDIA_cl_BaseDialog
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EDIA_cl_SectoDialog(struct WOR_tdst_World_ *);
	~EDIA_cl_SectoDialog(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	struct WOR_tdst_World_	*mpst_World;
	BOOL mb_SpecSort;
	int mi_SpecSort;
#ifdef JADEFUSION
	CImageList				mo_ImageList;
#endif
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	void	DoDataExchange(CDataExchange *);
	BOOL	PreTranslateMessage(MSG *);
	void	FillListSect(void);
	void	RefSetMenu(CListBox *, UCHAR *, UCHAR *);
	BOOL	PickPoint3D(MATH_tdst_Vector *);
	BOOL	OnInitDialog(void);
	void	ColorizeSel(void);
	void	ShowPortalsSel(void);
	void	UpdateFlags(BOOL);
	void	DeleteSharePortalTo(int, int);
	void	CopySharePortalTo(int, int);
	void	AddPickObj(int, BOOL);
	int		i_OnMessage(ULONG, ULONG, ULONG);
	void	RefreshFromExt(void);
	void	SortSecto(CList<int, int> &);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    OVERWRITE.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void	OnGetMinMaxInfo(MINMAXINFO FAR *);
	afx_msg void	OnMenuSect(void);
	afx_msg void	OnMenuSectVis(void);
	afx_msg void	OnMenuSectAct(void);
	afx_msg void	OnMenuSectObj(void);
	afx_msg void	OnMenuSectPortal(void);
	afx_msg void	OnSelSect(void);
	afx_msg void	OnSectDblClk(void);
	afx_msg void	OnBnClick(void);
#ifdef JADEFUSION
	afx_msg void	OnBnExport(void);
#endif
	afx_msg void	OnCopy(void);
	afx_msg void	OnPaste(void);
	DECLARE_MESSAGE_MAP()
};
#endif /* ACTIVE_EDITORS */
