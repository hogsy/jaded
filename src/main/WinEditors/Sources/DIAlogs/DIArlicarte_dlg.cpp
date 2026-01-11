/*$T DIARLICarte_dlg.cpp GC! 1.081 06/14/00 10:03:17 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIArlicarte_dlg.h"
#include "DIAlogs/DIACOLOR_dlg.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "Engine/Sources/MODifier/MDFmodifier_GEO.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dframe.h"
#include "EDIapp.h"
#include "EDIeditors_infos.h"
#include "Editors/Sources/OUTput/OUTframe.h"
#include "GEOmetric/GEOsubobject.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "LINKs/LINKtoed.h"
#include "LINks/LINKmsg.h"
#include "DIAlogs/DIAfile_dlg.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "EDImsg.h"

BEGIN_MESSAGE_MAP(EDIA_cl_RLICarteDialog, EDIA_cl_BaseDialog)
	ON_BN_CLICKED(IDC_BUTTON_DEDUCT, OnButton_Deduct)
	ON_BN_CLICKED(IDC_BUTTON_MARK, OnButton_Mark)
	ON_BN_CLICKED(IDC_BUTTON_COLORINIT, OnButton_ColorInit)
	ON_BN_CLICKED(IDC_BUTTON_ALLCOLORINIT, OnButton_AllColorInit)
	ON_BN_CLICKED(IDC_BUTTON_COLOR, OnButton_Color)
	ON_BN_CLICKED(IDC_BUTTON_ALLCOLOR, OnButton_AllColor)
	ON_BN_CLICKED(IDC_BUTTON_SEL, OnButton_Sel)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButton_Update)
	ON_BN_CLICKED(IDC_BUTTON_REINIT, OnButton_Reinit)
	ON_BN_CLICKED(IDC_RADIO_NONE, OnButton_OpNone)
	ON_BN_CLICKED(IDC_RADIO_SET, OnButton_OpSet)
	ON_BN_CLICKED(IDC_RADIO_ADD, OnButton_OpAdd)
	ON_BN_CLICKED(IDC_RADIO_SUB, OnButton_OpSub)
	ON_BN_CLICKED(IDC_BUTTON_SAVERLIDATA, OnButton_Save)
	ON_BN_CLICKED(IDC_BUTTON_LOADRLIDATA, OnButton_Load)
    ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_RLICarteDialog::EDIA_cl_RLICarteDialog(MDF_tdst_Modifier *_pst_Modifier, OBJ_tdst_GameObject *_pst_GO, GEO_tdst_Object *_pst_Obj ) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_RLICARTE)
{
	mpst_Modifier = _pst_Modifier;
	mpst_RLICarte = (GEO_tdst_ModifierRLICarte *) mpst_Modifier->p_Data;
    mpst_Gao = _pst_GO;
    mpst_Geo = _pst_Obj;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_RLICarteDialog::~EDIA_cl_RLICarteDialog(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_RLICarteDialog::OnInitDialog(void)
{
	/*~~~~~~~~~~~*/
	CRect	o_Rect;
	/*~~~~~~~~~~~*/

	List_Fill();
    SetWindowPos( NULL, 0, 0, 560, 400, SWP_NOMOVE | SWP_NOZORDER );
    CenterWindow();

	Op_Update();
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDIA_cl_RLICarteDialog::i_OnMessage(ULONG msg, ULONG, ULONG) 
{ 
	if(msg == EDI_MESSAGE_REFRESHDLG)
	{
		RefreshFromExt();
	}
	return 1; 
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::RefreshFromExt(void)
{
	M_MF()->LockDisplay(this);
	//List_Fill();
	M_MF()->UnlockDisplay(this);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnSize(UINT n, int x, int y)
{
	/*~~~~~~~~~~~*/
    //int h, w;
	/*~~~~~~~~~~~*/
    
    EDIA_cl_BaseDialog::OnSize(n, x, y);

    if ( !GetDlgItem( IDC_LIST ) ) return;

    M_MF()->LockDisplay( this );

    //Invalidate();
    M_MF()->UnlockDisplay( this );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 500;
	lpMMI->ptMinTrackSize.y = 360;
}


/*$4
 ***********************************************************************************************************************
    Op radio button functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::Op_Update( void )
{
	int i;

	dpo_OpButton[ 0 ] = (CButton *) GetDlgItem( IDC_RADIO_NONE );
	dpo_OpButton[ 1 ] = (CButton *) GetDlgItem( IDC_RADIO_SET );
	dpo_OpButton[ 2 ] = (CButton *) GetDlgItem( IDC_RADIO_ADD );
	dpo_OpButton[ 3 ] = (CButton *) GetDlgItem( IDC_RADIO_SUB );
	for (i = 0; i < 4; i++)
		dpo_OpButton[i]->SetCheck( (mpst_RLICarte->uc_Op == i) ? 1 : 0 );
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_OpNone( void )
{
	mpst_RLICarte->uc_Op = GEO_C_RLIOp_None;
	Op_Update();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_OpSet( void )
{
	mpst_RLICarte->uc_Op = GEO_C_RLIOp_Set;
	Op_Update();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_OpAdd( void )
{
	mpst_RLICarte->uc_Op = GEO_C_RLIOp_Add;
	Op_Update();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_OpSub( void )
{
	mpst_RLICarte->uc_Op = GEO_C_RLIOp_Sub;
	Op_Update();
}

/*$4
 ***********************************************************************************************************************
    Listfunctions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::List_Fill(void)
{
	CListBox	*po_LB;
	char		sz_String[ 128 ];
	int			i, j, nb;
	int			i_Cur, i_Top;

	po_LB = (CListBox *) GetDlgItem( IDC_LIST );

	i_Cur = po_LB->GetCurSel();
	i_Top = po_LB->GetTopIndex();

	po_LB->ResetContent();

	for ( i = 0; i < 64; i++)
	{
		nb = 0;
		if ( mpst_RLICarte->duc_PtGroup )
		{
			for (j = 0; j < (int) mpst_RLICarte->ul_NbPoints; j++)
			{
				if (mpst_RLICarte->duc_PtGroup[j] == i)
					nb++;
			}
		}
		sprintf( sz_String,"%2d - %4d - %08X - %08X", i, nb, mpst_RLICarte->aul_ColorInit[ i ] , mpst_RLICarte->aul_Color[ i ] );
		j = po_LB->AddString( sz_String );
		po_LB->SetItemData( j, i );
	}

	if (i_Cur == -1)
		po_LB->SetCurSel( 0 );
	else
	{
		po_LB->SetCurSel( i_Cur );
		po_LB->SetTopIndex( i_Top );
	}
	i_CurGroup = po_LB->GetCurSel();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::List_GetCur( void )
{
	i_CurGroup = ((CListBox *) GetDlgItem( IDC_LIST ))->GetCurSel();
	if (i_CurGroup == -1)
	{
		i_CurGroup = 0;
		((CListBox *) GetDlgItem( IDC_LIST ))->SetCurSel(0);
	}
}

/*$4
 ***********************************************************************************************************************
    vertex functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_Sel( void )
{
    int                             i, nb;
    char                            *pc_Sel;

	if ( !mpst_RLICarte->duc_PtGroup )
		return;
    
	List_GetCur();

    if ( (mpst_Geo) && (mpst_Geo->pst_SubObject) )
    {
        pc_Sel = mpst_Geo->pst_SubObject->dc_VSel;
        L_memset( pc_Sel, 0, mpst_Geo->l_NbPoints );
		nb = (mpst_Geo->l_NbPoints < (int) mpst_RLICarte->ul_NbPoints) ? mpst_Geo->l_NbPoints : mpst_RLICarte->ul_NbPoints;
        for (i = 0; i < nb; i++)
        {
			if ( mpst_RLICarte->duc_PtGroup[ i ] == i_CurGroup )
				pc_Sel[ i ] = 1;
        }
        LINK_Refresh();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_Update( void )
{
	int                             i, nb;
    char                            *pc_Sel;

	if ( !mpst_RLICarte->duc_PtGroup )
		return;
    
	List_GetCur();

    if ( (mpst_Geo) && (mpst_Geo->pst_SubObject) )
    {
        pc_Sel = mpst_Geo->pst_SubObject->dc_VSel;
		nb = (mpst_Geo->l_NbPoints < (int) mpst_RLICarte->ul_NbPoints) ? mpst_Geo->l_NbPoints : mpst_RLICarte->ul_NbPoints;
        for (i = 0; i < nb; i++)
        {
			if (pc_Sel[ i ] & 1 )
				mpst_RLICarte->duc_PtGroup[ i ] = i_CurGroup;
        }
        LINK_Refresh();
		List_Fill();
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_Reinit( void )
{
	L_memcpy( mpst_RLICarte->aul_Color, mpst_RLICarte->aul_ColorInit, 64 *sizeof( ULONG ) );
	List_Fill();
	LINK_Refresh();
}

/*$4
 ***********************************************************************************************************************
    color functions
 ***********************************************************************************************************************
 */

static void HookProc(ULONG, ULONG)
{
	LINK_Refresh();
}
/**/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_ColorInit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_ColorDialog		*po_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	List_GetCur();

	po_Color = new EDIA_cl_ColorDialog("set color init", &mpst_RLICarte->aul_ColorInit[ i_CurGroup ], HookProc, (ULONG) this, 0);
	po_Color->DoModal();
	delete(po_Color);
    List_Fill();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_AllColorInit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_ColorDialog		*po_Color;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	List_GetCur();

	po_Color = new EDIA_cl_ColorDialog("set all color init", &mpst_RLICarte->aul_ColorInit[ i_CurGroup ], HookProc, (ULONG) this, 0);
	po_Color->DoModal();
	delete(po_Color);
	for (i = 0; i < 64; i++)
	{
		mpst_RLICarte->aul_ColorInit[ i ] = mpst_RLICarte->aul_ColorInit[ i_CurGroup ]; 
	}
    List_Fill();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_Color(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_ColorDialog		*po_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	List_GetCur();

	po_Color = new EDIA_cl_ColorDialog("set color", &mpst_RLICarte->aul_Color[ i_CurGroup ], HookProc, (ULONG) this, 0);
	po_Color->DoModal();
	delete(po_Color);
    List_Fill();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_AllColor(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_ColorDialog		*po_Color;
	int						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	List_GetCur();

	po_Color = new EDIA_cl_ColorDialog("set all color", &mpst_RLICarte->aul_Color[ i_CurGroup ], HookProc, (ULONG) this, 0);
	po_Color->DoModal();
	delete(po_Color);
	for (i = 0; i < 64; i++)
	{
		mpst_RLICarte->aul_Color[ i ] = mpst_RLICarte->aul_Color[ i_CurGroup ]; 
	}
    List_Fill();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_Deduct(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG						*PC, C;
	int							i; //, j, nb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ((mpst_Gao->pst_Base) && (mpst_Gao->pst_Base->pst_Visu) && (mpst_Gao->pst_Base->pst_Visu->dul_VertexColors) )
	{
		PC = mpst_Gao->pst_Base->pst_Visu->dul_VertexColors + 1;
		mpst_RLICarte->ul_NbPoints = mpst_Gao->pst_Base->pst_Visu->dul_VertexColors[ 0 ];
		if (mpst_RLICarte->duc_PtGroup)
			MEM_Free( mpst_RLICarte->duc_PtGroup );
		mpst_RLICarte->duc_PtGroup = (UCHAR *) MEM_p_Alloc( mpst_RLICarte->ul_NbPoints );
		L_memset( mpst_RLICarte->duc_PtGroup, 0, mpst_RLICarte->ul_NbPoints );

		/*
		nb = 0;
		for (i = 0; i < (int) mpst_RLICarte->ul_NbPoints; i++)
		{
			C = PC[i] & 0xFF00;
			for (j = 0; j < nb; j++)
			{
				if (C == mpst_RLICarte->aul_Color[ j ] )
				{
					nb--;
					break;
				}
				if (C < mpst_RLICarte->aul_Color[ j ] )
				{
					L_memmove( &mpst_RLICarte->aul_Color[ j + 1 ], &mpst_RLICarte->aul_Color[ j ], nb - j );
					break;
				}
			}
			mpst_RLICarte->aul_Color[ j ] = C;
			nb++;
			if (nb == 64) break;
		}

		for (i = 0; i < (int) mpst_RLICarte->ul_NbPoints; i++)
		{
			C = PC[i];
			for (j = 0; j < nb; j++)
			{
				if (C == mpst_RLICarte->aul_Color[ j ] )
				{
					mpst_RLICarte->duc_PtGroup[ i ] = j;
					break;
				}
			}
		}
		*/

		for (i = 0; i < (int) mpst_RLICarte->ul_NbPoints; i++)
		{
			C = PC[i];
			mpst_RLICarte->duc_PtGroup[ i ] = (UCHAR) ((C & 0xFF00) >> 10);
		}
	}

    List_Fill();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_Mark(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG						*PC, C;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ((mpst_Gao->pst_Base) && (mpst_Gao->pst_Base->pst_Visu) && (mpst_Gao->pst_Base->pst_Visu->dul_VertexColors) )
	{
		PC = mpst_Gao->pst_Base->pst_Visu->dul_VertexColors + 1;
		mpst_RLICarte->ul_NbPoints = mpst_Gao->pst_Base->pst_Visu->dul_VertexColors[ 0 ];

		for (i = 0; i < (int) mpst_RLICarte->ul_NbPoints; i++)
		{
			C = mpst_RLICarte->duc_PtGroup[ i ];
			PC[i] = (PC[i] & 0xFFFF00FF) | (C << 10);
		}
	}

    List_Fill();
}

/*$4
 ***********************************************************************************************************************
    file functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_Save()
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_FileDialog	o_File("Choose file", 3, TRUE, TRUE, NULL, "*.r4c" );
    CString				o_Temp;
    BIG_INDEX			ul_Index;
    char                *sz_Ext;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(o_File.DoModal() != IDOK) return;

    o_File.GetItem(o_File.mo_File, 0, o_Temp);

    sz_Ext = strrchr( (char *) (LPCSTR) o_Temp, '.' );
    if (sz_Ext == NULL)
        o_Temp += ".r4c";
    else if (L_stricmp( sz_Ext, ".r4c" ) != 0)
    {
        M_MF()->MessageBox( "Bad file extension\nUse .r4c extension", "Error", MB_OK | MB_ICONSTOP );
        return;
    }
    

	ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);

	
    if(ul_Index != BIG_C_InvalidIndex)
	{
        if (M_MF()->MessageBox( "Erase file ?", "Confirm", MB_YESNO | MB_ICONQUESTION ) == IDNO )
            return;
    }

    SAV_Begin( o_File.masz_FullPath, (char *) (LPCSTR) o_Temp );
    GEO_ModifierRLICarte_Save( mpst_Modifier );
    SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_RLICarteDialog::OnButton_Load()
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_FileDialog	        o_File("Choose file", 3, TRUE, TRUE, NULL, "*.r4c" );
    CString				        o_Temp;
    BIG_INDEX			        ul_Index;
    char                        *sz_Ext, *pc_Buffer;
    ULONG                       ul_Length;
    GEO_tdst_ModifierRLICarte	*pst_Data;
    GEO_tdst_Object             *pst_Geo;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // check data
    pst_Geo = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo( mpst_Modifier->pst_GO );
    if ( !pst_Geo )
    {
        M_MF()->MessageBox( "modifier is associated to a GameObject without visual object", "Error", MB_OK | MB_ICONSTOP );
        return;
    }

    if(o_File.DoModal() != IDOK) return;

    o_File.GetItem(o_File.mo_File, 0, o_Temp);

    sz_Ext = strrchr( (char *) (LPCSTR) o_Temp, '.' );
    if (sz_Ext == NULL)
        o_Temp += ".r4c";
    else if (L_stricmp( sz_Ext, ".r4c" ) != 0)
    {
        M_MF()->MessageBox( "Bad file extension\nUse .r4c extension", "Error", MB_OK | MB_ICONSTOP );
        return;
    }
    

	ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
    if(ul_Index == BIG_C_InvalidIndex)
	{
        M_MF()->MessageBox( "file doesn't exist !!", "Error", MB_OK | MB_ICONSTOP );
        return;
    }

    pc_Buffer = BIG_pc_ReadFileTmp( BIG_PosFile( ul_Index ), &ul_Length );

    // create new RLICarte data
    GEO_ModifierRLICarte_Create( mpst_Gao, mpst_Modifier, NULL );
    // load data
    GEO_ul_ModifierRLICarte_Load( mpst_Modifier, pc_Buffer );

    /* cheack data */
    pst_Data = (GEO_tdst_ModifierRLICarte *) mpst_Modifier->p_Data;
    if (!pst_Data)
    {
        M_MF()->MessageBox( "Loading failed", "Error", MB_OK | MB_ICONSTOP );
        mpst_Modifier->p_Data = pst_Data;
        return;
    }

    if (pst_Geo->l_NbPoints != (LONG) pst_Data->ul_NbPoints)
    {
        M_MF()->MessageBox( "RLICarte modifier and Geometry doesn't have same number of vertices", "Error", MB_OK | MB_ICONSTOP );
        GEO_ModifierRLICarte_Destroy( mpst_Modifier );
        mpst_Modifier->p_Data = mpst_RLICarte;
        return;
    }

    // free old RLICarte modifier
    mpst_Modifier->p_Data = mpst_RLICarte;
    GEO_ModifierRLICarte_Destroy( mpst_Modifier );
    mpst_Modifier->p_Data = pst_Data;

    // update display
    mpst_RLICarte = (GEO_tdst_ModifierRLICarte *) mpst_Modifier->p_Data;
    List_Fill();
}





#endif /* ACTIVE_EDITORS */
