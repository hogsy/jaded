/*$T DIAreplace_dlg.h GC! 1.078 03/16/00 10:29:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIAREPLACE_DLG_H__INCLUDED
#define UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIAREPLACE_DLG_H__INCLUDED

#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include "DIAlogs/DIAComboBoxCompletion.h"


#define ACTUAL	0
#define PREVIEW	1
// -- Forward declaration --
class F3D_cl_View;
typedef struct OBJ_tdst_GameObject_ OBJ_tdst_GameObject;
typedef struct	WOR_tdst_World_ WOR_tdst_World;
//// -- Forward declaration --


class EDIA_cl_ReplaceDialog : public EDIA_cl_BaseDialog
{

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	CONSTRUCT.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	EDIA_cl_ReplaceDialog (WOR_tdst_World *pst_World);
	~EDIA_cl_ReplaceDialog();

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	ATTRIBUTES.
	-----------------------------------------------------------------------------------------------------------------------
	*/
public:
	struct WOR_tdst_World_	*mpst_World;
	F3D_cl_View				*mpo_View;

private:
	CString		m_StrFind;
	CString		m_StrReplace;

	BOOL		mb_AutoSelect;
	BOOL		mb_Loop;
	BOOL		mb_ItemReplaced;

	DWORD		m_SelectedItemData;	//alway from Actual_Tree
	int         mi_TreeContent;
	int         mi_IconGro;
	int         mi_IconGrm;
	int         mi_IconTex;
	int         mi_IconGao;
	int         mi_IconGrl;
	int         mi_IconMap;
	int         mi_IconCob;
	int         mi_IconGam;
	int         mi_IconCin;
	int         mi_IconCmd;
	int         mi_IconSkl;
	int         mi_IconShp;
	int         mi_IconAck;
	int         mi_IconTrl;
	int         mi_IconOin;
	int         mi_IconOva;
	int         mi_IconMdl;
	int         mi_IconSnk;
	int         mi_IconMsk;
	int			mi_IconGrp;

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	OVERWRITE.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	BOOL		OnInitDialog(void);
	void		OnDestroy(void);
	BOOL		PreTranslateMessage( MSG * );

	void		UpdateTrees(void);

private:
	void		AddGaoToTree(OBJ_tdst_GameObject*, BOOL);

	BOOL		b_SelectNextItem(HTREEITEM);
	BOOL		b_ReplaceSelectedItem(void);
	BOOL		b_SelectItemInTree(DWORD, BOOL);
	HTREEITEM	h_GetTopParentItem(HTREEITEM, BOOL);
	BOOL		b_FindItem(DWORD , HTREEITEM &, BOOL, HTREEITEM = NULL);

	BOOL		b_SetSelectedItemData(HTREEITEM);

	void		EnableButtons(void);
	void		DisableButtons(void);

/*$2
-----------------------------------------------------------------------------------------------------------------------
MESSAGE MAP.
-----------------------------------------------------------------------------------------------------------------------
*/

protected:
	afx_msg void OnBtReplace(void);
	afx_msg void OnBtReplaceAll(void);
	afx_msg void OnBtFindNext(void);
	afx_msg void OnBtCancel(void);
	afx_msg void OnChangeComboFind(void);
	afx_msg void OnChangeComboReplace(void);
	afx_msg void OnSelectActual(NMHDR *, LRESULT *);
	afx_msg void OnSelectPreview(NMHDR *, LRESULT *);
	afx_msg void OnSize(UINT, int, int);
	afx_msg void    OnGetMinMaxInfo(MINMAXINFO FAR *);
	DECLARE_MESSAGE_MAP()
};


#endif /* ACTIVE_EDITORS */

#endif //UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIAREPLACE_DLG_H__INCLUDED