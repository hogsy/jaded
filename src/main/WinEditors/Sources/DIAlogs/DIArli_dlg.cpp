/*$T DIArli_dlg.cpp GC 1.129 09/14/01 17:23:20 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIArli_dlg.h"
#include "DIAlogs/DIACOLOR_dlg.h"
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
#include "EDItors/Sources/MENu/MENsubmenu.h"
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

BEGIN_MESSAGE_MAP(EDIA_cl_RLIDialog, EDIA_cl_BaseDialog)
	
    ON_BN_CLICKED(IDC_BUTTON_COMPUTE, OnBt_GaoCompute)
    ON_BN_CLICKED(IDC_BUTTON_DESTROY, OnBt_GaoDestroy)
    ON_BN_CLICKED(IDC_BUTTON_LOCK, OnBt_GaoLock)
    ON_BN_CLICKED(IDC_BUTTON_UNLOCK, OnBt_GaoUnlock)
    ON_BN_CLICKED(IDC_BUTTON_ADVANCED, OnBt_GaoAdvanced )
	
	ON_BN_CLICKED(IDC_BUTTON_GROCOMPUTE, OnBt_GroCompute)
    ON_BN_CLICKED(IDC_BUTTON_GRODESTROY, OnBt_GroDestroy)
    ON_BN_CLICKED(IDC_BUTTON_GROLOCK, OnBt_GroLock)
    ON_BN_CLICKED(IDC_BUTTON_GROUNLOCK, OnBt_GroUnlock)
    ON_BN_CLICKED(IDC_BUTTON_GROADVANCED, OnBt_GroAdvanced )

    ON_BN_CLICKED(IDC_BUTTON_GAO2GRO, OnBt_CopyGao2Gro)
	ON_BN_CLICKED(IDC_BUTTON_GRO2GAO, OnBt_CopyGro2Gao)
    ON_BN_CLICKED(IDC_BUTTON_GAO2LOD, OnBt_CopyGao2Lod)
	
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_RLIDialog::EDIA_cl_RLIDialog(F3D_cl_View *_po_View ) :	EDIA_cl_BaseDialog(DIALOGS_IDD_RLI)
{
    mpo_3DView = _po_View;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_RLIDialog::OnInitDialog(void)
{
	EDIA_cl_BaseDialog::OnInitDialog();
	//((CButton *) GetDlgItem(IDC_CHECK_R))->SetCheck(1);
	//((CButton *) GetDlgItem(IDC_CHECK_G))->SetCheck(1);
	//((CButton *) GetDlgItem(IDC_CHECK_B))->SetCheck(1);
	//((CButton *) GetDlgItem(IDC_CHECK_A))->SetCheck(0);
	//((CButton *) GetDlgItem(IDC_CHECK_GLOBAL))->SetCheck(1);
	//((CButton *) GetDlgItem(IDC_CHECK_LOCAL))->SetCheck(0);
    ((CButton *) GetDlgItem( IDC_RADIO_LIGHTALL ))->SetCheck(1);
    ((CButton *) GetDlgItem( IDC_RADIO_LIGHTSELECTED ))->SetCheck(0);
    ((CButton *) GetDlgItem( IDC_RADIO_OBJECTALL ))->SetCheck(0);
    ((CButton *) GetDlgItem( IDC_RADIO_OBJECTSELECTED ))->SetCheck(1);
	mul_Mask = 0x00FFFFFF;
	mul_Target = DIARLI_tgt_GAO;
	mul_Operation = DIARLI_op_none;

	CenterWindow();

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnDestroy(void)
{
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::GetParams(void)
{
	mul_Mask = 0;
	mul_Target = 0;

	//if(((CButton *) GetDlgItem(IDC_CHECK_R))->GetCheck()) mul_Mask |= 0xFF;
	//if(((CButton *) GetDlgItem(IDC_CHECK_G))->GetCheck()) mul_Mask |= 0xFF00;
	//if(((CButton *) GetDlgItem(IDC_CHECK_B))->GetCheck()) mul_Mask |= 0xFF0000;
	//if(((CButton *) GetDlgItem(IDC_CHECK_A))->GetCheck()) mul_Mask |= 0xFF000000;
	//if(((CButton *) GetDlgItem(IDC_CHECK_GLOBAL))->GetCheck()) mul_Target |= DIARLI_tgt_global;
	//if(((CButton *) GetDlgItem(IDC_CHECK_LOCAL))->GetCheck()) mul_Target |= DIARLI_tgt_local;

    mb_LightSel = ((CButton *) GetDlgItem( IDC_RADIO_LIGHTALL ))->GetCheck() ? 0 : 1;
    mb_ObjectSel = ((CButton *) GetDlgItem( IDC_RADIO_OBJECTALL ))->GetCheck() ? 0 : 1;
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    GAO OPERATION
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GaoCompute(void)
{
    GetParams();
    mpo_3DView->ComputeRLI( 0xFFFFFFFF, TRUE, mb_ObjectSel, mb_LightSel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GaoDestroy(void)
{
    GetParams();
    mpo_3DView->Selection_DestroyRLI( DIARLI_tgt_GAO, mb_ObjectSel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GaoLock(void)
{
    GetParams();
    mpo_3DView->Selection_RLILock( mb_ObjectSel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GaoUnlock(void)
{
    GetParams();
	mpo_3DView->Selection_RLIUnlock( mb_ObjectSel );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GaoAdvanced(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EMEN_cl_SubMenu     o_Menu(FALSE);
	POINT			    pt;
    EDIA_cl_ColorDialog *po_Color;
    ULONG               ul_Color;
    int                 i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetParams();

	M_MF()->InitPopupMenuAction(NULL, &o_Menu);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 1, TRUE, "Reset Alpha", -1);
    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 2, TRUE, "Reset RGB", -1);
    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 3, TRUE, "Reset RGB-Alpha", -1);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, 4, TRUE, "Invert Alpha", -1);
    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 5, TRUE, "Deduct RGB from alpha", -1);
    M_MF()->AddPopupMenuAction(NULL, &o_Menu, 6, TRUE, "Add color", -1);

	GetCursorPos(&pt);
	i = M_MF()->TrackPopupMenuAction(NULL, pt, &o_Menu);
	switch(i)
    {
	case 1:
        mpo_3DView->Selection_ResetRLI( DIARLI_tgt_GAO, 0xFF000000, mb_ObjectSel);
		break;
	case 2:
        mpo_3DView->Selection_ResetRLI( DIARLI_tgt_GAO, 0xFFFFFF, mb_ObjectSel);
		break;
	case 3:
        mpo_3DView->Selection_ResetRLI( DIARLI_tgt_GAO, 0xFFFFFFFF, mb_ObjectSel);
		break;
    case 4:
        mpo_3DView->Selection_RLIInvertAlpha();
        break;
    case 5:
        mpo_3DView->Selection_RLIComputeAlpha2Color( mb_ObjectSel );
        break;
    case 6:
        po_Color = new EDIA_cl_ColorDialog("add color to RLI", &ul_Color, NULL, (ULONG) this, 0);
		po_Color->DoModal();
		delete(po_Color);
        mpo_3DView->Selection_RLIAddColor( DIARLI_tgt_GAO, ul_Color, mb_ObjectSel );
        break;
	}
}


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    GRO OPERATION
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GroCompute(void)
{
    GetParams();
    //mpo_3DView->ComputeRLI( 0xFFFFFFFF, TRUE, mb_ObjectSel, mb_LightSel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GroDestroy(void)
{
    GetParams();
    mpo_3DView->Selection_DestroyRLI( DIARLI_tgt_GRO, mb_ObjectSel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GroLock(void)
{
    GetParams();
    //Selection_RLILock( mb_ObjectSel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GroUnlock(void)
{
    GetParams();
	//Selection_RLIUnlock( mb_ObjectSel );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_GroAdvanced(void)
{
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    GAO <-> GRO OPERATION
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_CopyGao2Gro(void)
{
	GetParams();
    mpo_3DView->Selection_CopyLocal2Global(0xffffffff, mb_ObjectSel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_CopyGro2Gao(void)
{
	GetParams();
    mpo_3DView->Selection_CopyGlobal2Local( 0xffffffff, mb_ObjectSel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBt_CopyGao2Lod(void)
{
	GetParams();
    //mpo_3DView->Selection_CopyGlobal2Local( 0xffffffff, mb_ObjectSel);
}




#endif /* ACTIVE_EDITORS */


#ifdef oldoldold
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBtCompute2(void)
{
	GetParams();
	mul_Operation = DIARLI_op_destroycompute;
	OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBtComputeAlphaToColor( void )
{
    GetParams();
    mul_Operation = DIARLI_op_computealphatocolor;
    OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBtCopyLG(void)
{
	GetParams();
	mul_Operation = DIARLI_op_copylg;
	OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBtCopyGL(void)
{
	GetParams();
	mul_Operation = DIARLI_op_copygl;
	OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBtReset(void)
{
	GetParams();
	mul_Operation = DIARLI_op_reset;
	OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBtDestroy(void)
{
	GetParams();
	mul_Operation = DIARLI_op_destroy;
	OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBtLock(void)
{
	GetParams();
	mul_Operation = DIARLI_op_lock;
	OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBtUnlock(void)
{
	GetParams();
	mul_Operation = DIARLI_op_unlock;
	OnOK();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLIDialog::OnBtInvertAlpha(void)
{
	GetParams();
	mul_Operation = DIARLI_op_invertalpha;
	OnOK();
}
#endif