/*$T DIAcopymatrixfrom_dlg.cpp GC! 1.081 08/23/01 15:35:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAcopymatrixfrom_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDIPaths.h"
#include "LINks/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJorient.h"

/* Message map */
BEGIN_MESSAGE_MAP(EDIA_cl_CopyMatrixFrom, EDIA_cl_BaseDialog)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_CopyMatrixFrom::EDIA_cl_CopyMatrixFrom( OBJ_tdst_GameObject *_pst_GO, ULONG ul_Flags ) : EDIA_cl_BaseDialog(DIALOGS_IDD_COPYMATRIXFROM)
{
    mpst_GO = _pst_GO;
    mul_Flags = ul_Flags;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_CopyMatrixFrom::~EDIA_cl_CopyMatrixFrom()
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_CopyMatrixFrom::OnInitDialog(void)
{
    char sz_Text[ 256 ];

    sprintf( sz_Text, "from %s", mpst_GO ? mpst_GO->sz_Name : "Unknow" );
    GetDlgItem( IDC_STATIC_GAONAME)->SetWindowText( sz_Text );
    ((CButton *) GetDlgItem( IDC_CHECK_TRANS ))->SetCheck( (mul_Flags & EDIA_CopyMatrixFlag_Trans) ? 1 : 0 );
    ((CButton *) GetDlgItem( IDC_CHECK_ROTATION ))->SetCheck( (mul_Flags & EDIA_CopyMatrixFlag_Rotation) ? 1 : 0 );
    ((CButton *) GetDlgItem( IDC_CHECK_SCALE ))->SetCheck( (mul_Flags & EDIA_CopyMatrixFlag_Scale) ? 1 : 0 );
    ((CButton *) GetDlgItem( IDC_CHECK_TRANSX ))->SetCheck( (mul_Flags & EDIA_CopyMatrixFlag_TransX) ? 1 : 0 );
    ((CButton *) GetDlgItem( IDC_CHECK_TRANSY ))->SetCheck( (mul_Flags & EDIA_CopyMatrixFlag_TransY) ? 1 : 0 );
    ((CButton *) GetDlgItem( IDC_CHECK_TRANSZ ))->SetCheck( (mul_Flags & EDIA_CopyMatrixFlag_TransZ) ? 1 : 0 );
    return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_CopyMatrixFrom::OnOK(void)
{
    mul_Flags = ((CButton *) GetDlgItem( IDC_CHECK_TRANS ))->GetCheck() ? EDIA_CopyMatrixFlag_Trans : 0;
    mul_Flags |= ((CButton *) GetDlgItem( IDC_CHECK_ROTATION ))->GetCheck() ? EDIA_CopyMatrixFlag_Rotation : 0;
    mul_Flags |= ((CButton *) GetDlgItem( IDC_CHECK_SCALE ))->GetCheck() ? EDIA_CopyMatrixFlag_Scale: 0;
    mul_Flags |= ((CButton *) GetDlgItem( IDC_CHECK_TRANSX ))->GetCheck() ? EDIA_CopyMatrixFlag_TransX : 0;
    mul_Flags |= ((CButton *) GetDlgItem( IDC_CHECK_TRANSY ))->GetCheck() ? EDIA_CopyMatrixFlag_TransY : 0;
    mul_Flags |= ((CButton *) GetDlgItem( IDC_CHECK_TRANSZ ))->GetCheck() ? EDIA_CopyMatrixFlag_TransZ : 0;
    CDialog::OnOK();
}


#endif /* ACTIVE_EDITORS */
