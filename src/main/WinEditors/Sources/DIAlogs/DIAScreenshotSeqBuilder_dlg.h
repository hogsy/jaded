/*$T DIAreplace_dlg.h GC! 1.078 03/16/00 10:29:29 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIASCREENSHOTSEQUENCEBUILDER_DLG_H__INCLUDED
#define UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIASCREENSHOTSEQUENCEBUILDER_DLG_H__INCLUDED

#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAbase.h"
#include <vector>
#include <algorithm>

// -- Forward declaration --
class F3D_cl_View;
typedef struct	WOR_tdst_World_ WOR_tdst_World;
typedef struct	CAM_tdst_Camera_ CAM_tdst_Camera;
//// -- Forward declaration --

class EDIA_cl_ScreenshotSeqBuilderDlg : public EDIA_cl_BaseDialog
{

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	CONSTRUCT.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	EDIA_cl_ScreenshotSeqBuilderDlg ();
	~EDIA_cl_ScreenshotSeqBuilderDlg();

	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	ATTRIBUTES.
	-----------------------------------------------------------------------------------------------------------------------
	*/
public:
	struct WOR_tdst_World_		*mpst_World;
	F3D_cl_View					*mpo_View;

private:
	int			m_iOldWidth;
	int			m_iOldHeight;
	BOOL		m_bSequenceStarted;
	
	/*$2
	-----------------------------------------------------------------------------------------------------------------------
	OVERWRITE.
	-----------------------------------------------------------------------------------------------------------------------
	*/

public:
	BOOL		OnInitDialog(void);
	BOOL		PreTranslateMessage( MSG * );

	void		UpdateList(void);
	void		RefreshControls();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	void		OnMenuListCtrl(void);

/*$2
-----------------------------------------------------------------------------------------------------------------------
MESSAGE MAP.
-----------------------------------------------------------------------------------------------------------------------
*/

protected:
	afx_msg void	OnBtLoad(void);
	afx_msg void	OnBtSave(void);
	afx_msg void	OnBtStart(void);
	afx_msg void	OnBtUp(void);
	afx_msg void	OnBtDown(void);
	afx_msg void	OnBtDelete(void);
	afx_msg void	OnBtClear(void);
	afx_msg void	OnListLButtonDown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnListDblClk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnSize(UINT, int, int);
	afx_msg void    OnGetMinMaxInfo(MINMAXINFO FAR *);
	DECLARE_MESSAGE_MAP()
};


#endif /* ACTIVE_EDITORS */

#endif //UBI_X__JADE_MAIN_WINEDITORS_SOURCES_DIALOGS_DIASCREENSHOTSEQUENCEBUILDER_DLG_H__INCLUDED