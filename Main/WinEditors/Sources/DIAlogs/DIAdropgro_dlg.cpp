/*$T DIAdropgro_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAdropgro_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDIPaths.h"

/*
 =======================================================================================================================
 Message map
 =======================================================================================================================
 */
BEGIN_MESSAGE_MAP(EDIA_cl_DropGro, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBt_Create)
	ON_BN_CLICKED(IDC_BUTTON_ATTACH, OnBt_Attach)
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_DropGro::EDIA_cl_DropGro( char *_sz_Path, char *_sz_Name, int _i_Number, void **_pp_GO ) : EDIA_cl_BaseDialog(DIALOGS_IDD_DROPGRO)
{
    msz_Path = _sz_Path;

    if (_sz_Name)
        L_strcpy( msz_Name, _sz_Name);
    else
        *msz_Name = 0;

    mi_Number = _i_Number;
    mpp_GO = _pp_GO;

    mi_Choice = EDIA_DropGro_Cancel;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_DropGro::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject     *pst_GO;
    CComboBox               *po_CB;
    int                     i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_CB = (CComboBox *) GetDlgItem( IDC_COMBO_GO );
    po_CB->ResetContent();

    for ( i = 0; i < mi_Number; i++)
    {
        pst_GO = (OBJ_tdst_GameObject *) mpp_GO[ i ];
        j = po_CB->AddString( pst_GO->sz_Name );
        po_CB->SetItemData( j, i );
    }
    po_CB->SetCurSel( 0 );

    GetDlgItem( IDC_EDIT_NAME )->SetWindowText( msz_Name );
    return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_DropGro::OnBt_Create()
{
    char    *sz_Ext;
    ULONG   ul_File;

    GetDlgItem( IDC_EDIT_NAME )->GetWindowText( msz_Name, 255 );
    if ( !BIG_b_CheckName( msz_Name ) )
        return;

    sz_Ext = L_strrchr( msz_Name, '.' );
    if (!sz_Ext)
        L_strcat( msz_Name, EDI_Csz_ExtGameObject );
    else
        L_strcpy( sz_Ext, EDI_Csz_ExtGameObject );

    ul_File = BIG_ul_SearchFileExt( msz_Path, msz_Name);
    if (ul_File != BIG_C_InvalidIndex) 
    {
        ERR_X_ForceError( "File already exist", msz_Name);
        return;
    }

    mi_Choice = EDIA_DropGro_Create;
    OnCancel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_DropGro::OnBt_Attach()
{
    CComboBox               *po_CB;

    po_CB = (CComboBox *) GetDlgItem( IDC_COMBO_GO );
    mi_Index = po_CB->GetCurSel();
    if (mi_Index == CB_ERR) mi_Index = 0;
    mi_Index = po_CB->GetItemData( mi_Index );

    mi_Choice = EDIA_DropGro_Attach;
    OnCancel();
}


#endif /* ACTIVE_EDITORS */
