/*$T DIAname_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DATCP4View.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles\LOAding\LOAdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "F3Dframe/F3Dview.h"
#include "EDIpaths.h"
#include "EDImainframe.h"
#include "Res/Res.h"

BEGIN_MESSAGE_MAP(EDIA_cl_P4ViewDialog,EDIA_cl_BaseDialog)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()
/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_P4ViewDialog::EDIA_cl_P4ViewDialog(	const char *_psz_Title,
											const char* _psz_comment,
											BOOL _bSelectAllText /* = TRUE */) :
EDIA_cl_BaseDialog(DIALOGS_IDD_P4VIEW),
mo_Title(_psz_Title),
mo_Comment(_psz_comment),
m_bSelectAllText(_bSelectAllText)
{
	//mo_Title = _psz_Title;
	//mo_Comment = _psz_comment;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
int EDIA_cl_P4ViewDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int iReturn ;
	if(  (iReturn = EDIA_cl_BaseDialog::OnCreate(lpCreateStruct)) == -1)
	{
		return -1;
	}

	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRichEditCtrl	*po_Edit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	po_Edit = ((CRichEditCtrl *) GetDlgItem(IDC_P4TEXT));
	po_Edit->SetFocus();

	if ( m_bSelectAllText ) 
	{
		po_Edit->SetSel(0,mo_Comment.GetLength());
	}
	else
	{
		po_Edit->SetSel(0,0);
	}
	return iReturn;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_P4ViewDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRichEditCtrl	*po_Edit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	CDialog::DoDataExchange(pDX);

	po_Edit = ((CRichEditCtrl *) GetDlgItem(IDC_P4TEXT));


	SetWindowText(mo_Title);
	DDX_Text(pDX, IDC_P4TEXT, mo_Comment);
	po_Edit->SetFocus();
}

#endif /* ACTIVE_EDITORS */
