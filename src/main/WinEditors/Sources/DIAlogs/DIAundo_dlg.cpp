/*$T DIAundo_dlg.cpp GC! 1.081 05/23/01 08:14:41 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAundo_dlg.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/ENGVars.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "LINKs/LINKtoed.h"
#include "EDItors/Sources/AIscript/AIframe.h"
#include "EDItors/Sources/AIscript/EditView/AIview.h"
#include "EDImsg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "EDIpaths.h"

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

BEGIN_MESSAGE_MAP(EDIA_cl_UndoDialog, EDIA_cl_BaseDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_UndoDialog::EDIA_cl_UndoDialog(EDI_cl_UndoManager *_po_UM) :	EDIA_cl_BaseDialog(DIALOGS_IDD_UNDO)
{
	mpo_UndoManager = _po_UM;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_UndoDialog::~EDIA_cl_UndoDialog()
{
	mpo_UndoManager->mpo_UndoDlg = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_UndoDialog::OnInitDialog(void)
{
	EDIA_cl_BaseDialog::OnInitDialog();
	GetDlgItem(IDC_LIST_UNDO)->SetFocus();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UndoDialog::OnDestroy(void)
{
	mpo_UndoManager->mpo_UndoDlg = NULL;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UndoDialog::OnSize(UINT a, int cx, int cy)
{
	EDIA_cl_BaseDialog::OnSize(a, cx, cy);
	if(GetDlgItem(IDC_LIST_UNDO)) GetDlgItem(IDC_LIST_UNDO)->MoveWindow(0, 0, cx, cy);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_UndoDialog::UpdateUndoList(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListBox			*po_LB;
	POSITION			x_Pos;
	EDI_cl_UndoModif	*po_Modif;
	int					count;
	char				sz_NoNameDesc[] = "<No descriptor>";
	char				*psz_Desc, sz_String[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (ENG_gb_ExitApplication) return;

	po_LB = (CListBox *) GetDlgItem(IDC_LIST_UNDO);
	po_LB->ResetContent();

	count = 0;
	x_Pos = mpo_UndoManager->mo_ListOfModifs.GetHeadPosition();
	while(x_Pos)
	{
		po_Modif = mpo_UndoManager->mo_ListOfModifs.GetAt(x_Pos);

		psz_Desc = (po_Modif->msz_Desc == NULL) ? sz_NoNameDesc : po_Modif->msz_Desc;
		sprintf(sz_String, "%2d %c %s", count, (count < mpo_UndoManager->mi_CurrentIndex) ? '*' : ' ', psz_Desc);
		po_LB->AddString( sz_String );

		mpo_UndoManager->mo_ListOfModifs.GetNext(x_Pos);
        count++;
	}
}

#endif /* ACTIVE_EDITORS */
