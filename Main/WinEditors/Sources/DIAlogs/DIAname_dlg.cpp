/*$T DIAname_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAname_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#include "edipaths.h"
#include "engine/sources/animation/aniinit.h"
#include "bigfiles/bigkey.h"
#include "bigfiles/bigfat.h"
#include "engine/sources/world/worsave.h"
#include "f3dframe/f3dview.h"
#include "engine/sources/objects/objinit.h"
#include <string>

#include "BIGfiles\LOAding\LOAdefs.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"

#ifdef JADEFUSION
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGread.h"
#endif

#include "Engine/sources/OBJects/OBJslowaccess.h"

ULONG IsSubGAODirectoryPresent(WOR_tdst_World* pWorld, char* pstrSubDirName);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_NameDialog::EDIA_cl_NameDialog(char *_psz_Title, int _mi_MaxLen, BOOL _b_Pass) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_NAME)
{
	mo_Title = _psz_Title;
	mi_MaxLen = _mi_MaxLen;
	mb_Pass = _b_Pass;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_NameDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CEdit	*po_Edit, *po_Edit1;
	int		nIDC; // Control ID to do data exchange
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	CDialog::DoDataExchange(pDX);

	po_Edit = ((CEdit *) GetDlgItem(IDC_EDIT));
	po_Edit1 = ((CEdit *) GetDlgItem(IDC_EDIT1));

	/* Password or not ? */
	if(mb_Pass)
	{
		po_Edit->ShowWindow(SW_HIDE);
		po_Edit1->ShowWindow(SW_SHOW);
		po_Edit = po_Edit1;
		nIDC = IDC_EDIT1;
	}
	else
	{
		po_Edit->ShowWindow(SW_SHOW);
		po_Edit1->ShowWindow(SW_HIDE);
		nIDC = IDC_EDIT;
	}

	SetWindowText(mo_Title);

	DDX_Text(pDX, nIDC, mo_Name);

	// -----------------------------------------
	// Strip trailing spaces in variable mo_Name
	mo_Name.TrimLeft();
	mo_Name.TrimRight();

	po_Edit->SetLimitText(mi_MaxLen);
	po_Edit->SetFocus();
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_NameDialogCombo::EDIA_cl_NameDialogCombo(char *_psz_Title, int _mi_MaxLen) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_NAMECOMBO)
{
	mo_Title = _psz_Title;
	mi_MaxLen = _mi_MaxLen;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_NameDialogCombo::AddItem(char *_psz_Name, int _i_Data)
{
	mo_List.AddTail(_psz_Name);
	mo_ListDatas.AddTail(_i_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_NameDialogCombo::SetDefault(char *_psz_Name)
{
	if(_psz_Name)
		mo_Name = _psz_Name;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_NameDialogCombo::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox	*po_Combo;
	POSITION	pos, pos1;
	char		asz_Name[BIG_C_MaxLenName];
	char		*psz_Temp;
	int			index;
    CString     o_Str;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CDialog::DoDataExchange(pDX);
	SetWindowText(mo_Title);
	po_Combo = ((CComboBox *) GetDlgItem(IDC_COMBO4));
	po_Combo->LimitText(mi_MaxLen);
	po_Combo->SetFocus();

	if(pDX->m_bSaveAndValidate == FALSE)
	{
		if(mo_Name) po_Combo->SetWindowText(mo_Name);
		pos = mo_List.GetHeadPosition();
		pos1 = mo_ListDatas.GetHeadPosition();
		while(pos)
		{
			L_strcpy(asz_Name, mo_List.GetAt(pos));
			psz_Temp = L_strrchr(asz_Name, '.');
			if(psz_Temp) *psz_Temp = 0;
			index = po_Combo->AddString(asz_Name);
			po_Combo->SetItemData(index, mo_ListDatas.GetNext(pos1));
			mo_List.GetNext(pos);
		}
	}
	else
	{
		po_Combo->GetWindowText(mo_Name);
		index = po_Combo->GetCurSel();

        if (index == -1)
        {
            index = po_Combo->GetCount() - 1;
            while (index >= 0)
            {
                po_Combo->GetLBText( index, o_Str );
                //if (L_strcmp( (char *) (LPSTR) &o_Str, _psz_Name ) == 0)
                if (mo_Name == o_Str )
                    break;
                index--;
            }
        }
        mi_CurSelData = po_Combo->GetItemData(index);
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BEGIN_MESSAGE_MAP(EDIA_cl_NameRLIDialogCombo, EDIA_cl_BaseDialog)
	ON_CBN_SELCHANGE(IDC_COMBO4, OnSelChange)
	ON_BN_CLICKED( IDC_RADIO_GAO, OnRadio_GAO )
	ON_BN_CLICKED( IDC_RADIO_GRO, OnRadio_GRO )
	ON_WM_PAINT()
END_MESSAGE_MAP()

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_NameRLIDialogCombo::EDIA_cl_NameRLIDialogCombo(char *_psz_Title,
													   OBJ_tdst_GameObject	**_mo_ListGAO,
													   int _mi_MaxLen) :
#ifdef JADEFUSION
EDIA_cl_BaseDialog(0)
#else
EDIA_cl_BaseDialog(DIALOGS_IDD_NAMERLICOMBO)
#endif
{
	mo_Title = _psz_Title;
	mi_MaxLen = _mi_MaxLen;

	WorkWithGRORLI = FALSE;
	mo_ListGAO = _mo_ListGAO;
	AlreadyPainted = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_NameRLIDialogCombo::AddItem(int _i_Data)
{
	mo_List.AddTail(mo_ListGAO[_i_Data]->sz_Name);
	mo_ListDatas.AddTail(_i_Data);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_NameRLIDialogCombo::SetDefault(char *_psz_Name)
{
	if(_psz_Name)
		mo_Name = _psz_Name;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_NameRLIDialogCombo::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox	*po_Combo;
	POSITION	pos, pos1;
	char		asz_Name[BIG_C_MaxLenName];
	char		*psz_Temp;
	int			index;
    CString     o_Str;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CDialog::DoDataExchange(pDX);
	SetWindowText(mo_Title);
	po_Combo = ((CComboBox *) GetDlgItem(IDC_COMBO4));
	po_Combo->LimitText(mi_MaxLen);
	po_Combo->SetFocus();

	if(pDX->m_bSaveAndValidate == FALSE)
	{
		if(mo_Name) po_Combo->SetWindowText(mo_Name);
		pos = mo_List.GetHeadPosition();
		pos1 = mo_ListDatas.GetHeadPosition();
		while(pos)
		{
			L_strcpy(asz_Name, mo_List.GetAt(pos));
			psz_Temp = L_strrchr(asz_Name, '.');
			if(psz_Temp) *psz_Temp = 0;
			index = po_Combo->AddString(asz_Name);
			po_Combo->SetItemData(index, mo_ListDatas.GetNext(pos1));
			mo_List.GetNext(pos);
		}
	}
	else
	{
		po_Combo->GetWindowText(mo_Name);
		index = po_Combo->GetCurSel();

        if (index == -1)
        {
            index = po_Combo->GetCount() - 1;
            while (index >= 0)
            {
                po_Combo->GetLBText( index, o_Str );
                //if (L_strcmp( (char *) (LPSTR) &o_Str, _psz_Name ) == 0)
                if (mo_Name == o_Str )
                    break;
                index--;
            }
        }
        mi_CurSelData = po_Combo->GetItemData(index);
	}
}

void EDIA_cl_NameRLIDialogCombo::OnSelChange( void ) {
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox			*po_Combo;
	int					index;
	u8					RLIloc;
	OBJ_tdst_GameObject	*CurGAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Combo = ((CComboBox *) GetDlgItem(IDC_COMBO4));
	index = po_Combo->GetCurSel();
	mi_CurSelData = po_Combo->GetItemData(index);
	CurGAO = mo_ListGAO[mi_CurSelData];

	// Where is the RLI ?
	RLIloc = OBJ_i_RLIlocation(CurGAO);
	GetDlgItem(IDC_RADIO_GAO)->EnableWindow(RLIloc & u8_RLIinGAO);
	GetDlgItem(IDC_RADIO_GRO)->EnableWindow(RLIloc >= u8_RLIinGRO);
	// Check the right radio button
	WorkWithGRORLI = !(RLIloc & u8_RLIinGAO) && (RLIloc >= u8_RLIinGRO);
	((CButton *) GetDlgItem(IDC_RADIO_GRO))->SetCheck( WorkWithGRORLI && (RLIloc >= u8_RLIinGRO));
	((CButton *) GetDlgItem(IDC_RADIO_GAO))->SetCheck(!WorkWithGRORLI && (RLIloc & u8_RLIinGAO));
}

// Activate the right radio buttons the first time the windows is paint
void EDIA_cl_NameRLIDialogCombo::OnPaint( void ) {
	EDIA_cl_BaseDialog::OnPaint();
	if (AlreadyPainted) return;
	AlreadyPainted = TRUE;
	CComboBox *po_Combo = ((CComboBox *) GetDlgItem(IDC_COMBO4));
	po_Combo->SetCurSel(0);
	OnSelChange();

}

void EDIA_cl_NameRLIDialogCombo::OnRadio_GAO( void ) {
	WorkWithGRORLI = FALSE;
}

void EDIA_cl_NameRLIDialogCombo::OnRadio_GRO( void ) {
	WorkWithGRORLI = TRUE;
}

/*$4
***********************************************************************************************************************
***********************************************************************************************************************
*/

#define OBJECTNAME_NOTSET "Unknown or not set"
#define TYPEDUPLICATION_COPY IDC_RADIO1
#define TYPEDUPLICATION_INST IDC_RADIO2

#ifdef JADEFUSION
//WOW or WOL key to fill combobox 
extern unsigned int WOR_gul_WorldKey;
#endif
/*
=======================================================================================================================
=======================================================================================================================
*/
EDIA_cl_NameDialogDuplicate::EDIA_cl_NameDialogDuplicate(F3D_cl_View *po_View, OBJ_tdst_GameObject *po_Gao) :
EDIA_cl_BaseDialog(DIALOGS_IDD_NAMEDUPLICATE)
{
	mpo_View = po_View ;
	mpo_Gao = po_Gao ;
	
	m_pTreeCtl = NULL ;
#ifdef JADEFUSION
	m_iTypeOfDuplication = -1;
#endif
	m_bDlgIsInitialized = FALSE ;
}


BEGIN_MESSAGE_MAP(EDIA_cl_NameDialogDuplicate, CDialog)
#ifdef JADEFUSION
	ON_WM_SIZE()
#endif
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio)
	ON_CBN_EDITCHANGE(IDC_COMBO, OnEnChangeCombo)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
#ifdef JADEFUSION
	ON_WM_GETMINMAXINFO()
#endif
END_MESSAGE_MAP()

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_NameDialogDuplicate::OnInitDialog(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG ul_File;
#ifdef JADEFUSION
	CHAR		*pc_Buf;
	ULONG		ul_Pos, ul_Size;
	BIG_INDEX	ul_Index;
	BIG_KEY		ul_Key;
	CString		strWorldName;
#endif
	std::string strGaoName ;
	char strSubDirName[BIG_C_MaxLenPath] = "";
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	EDIA_cl_BaseDialog::OnInitDialog();

	if( ! mpo_Gao ) 
		return TRUE ;

	// -------------------------------------------------------------------------
	// Check GAO name to duplicate
	if ( mpo_Gao->sz_Name )
	{
		strGaoName = mpo_Gao->sz_Name;
		
		// Remove file extension if present
		if (strGaoName.find(EDI_Csz_ExtGameObject) != -1)
			strGaoName.erase(strGaoName.size()-strlen(EDI_Csz_ExtGameObject));
		//initial name of source Gao
		m_strInitialGaoName = mpo_Gao->sz_Name;
	}
	else
		strGaoName = OBJECTNAME_NOTSET ;
	
	// -------------------------------------------------------------------------
	// Get GAO sub directory name from name
	int index = 0;
	index = strGaoName.find('_',index);
	index = strGaoName.find('_',index+1);
	strcpy(strSubDirName, strGaoName.substr(0,index).c_str());

#ifdef JADEFUSION
	if(WOR_gul_WorldKey != BIG_C_InvalidKey)
	 	ul_Index = BIG_ul_SearchKeyToFat(WOR_gul_WorldKey);
	else
		ul_Index = BIG_C_InvalidIndex;
#else
	ul_File = IsSubGAODirectoryPresent(mpo_Gao->pst_World, strSubDirName);
#endif	

	// -------------------------------------------------------------------------
	// Setup working directory where we are getting the GAOs
	char strName[BIG_C_MaxLenPath] = "";
	sprintf(strName,"(List of GAO coming from:\r\n %s)", strSubDirName);
	GetDlgItem(IDC_STATIC)->SetWindowText(strName);

	// -------------------------------------------------------------------------
	// Setup ComboBox with current GAO and list of GAO in WOW
	m_cbCompletion.SubclassDlgItem(IDC_COMBO, this);
#ifdef JADEFUSION
	if(ul_Index != BIG_C_InvalidIndex)
	{
		strWorldName = BIG_NameFile(ul_Index);
		if(strWorldName.Find(EDI_Csz_ExtWorldList))
		{
			//it's a WOL
			ul_Pos = BIG_ul_SearchKeyToPos(WOR_gul_WorldKey);
			pc_Buf = BIG_pc_ReadFileTmp(ul_Pos, &ul_Size);

			// wol parsing begin --
			for(int i = 0; i < ul_Size >> 3; ++i)
			{
				ul_Key = LOA_ReadULong(&pc_Buf);
				
				ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					//send index of first file for each GameObject subdirectories
					ul_Index = BIG_ParentFile(ul_Index);
					if(ul_Index != BIG_C_InvalidIndex)	//parent dir is valid
					{
						//search for GameObject dir
						ul_Index = BIG_ul_SearchDirInDir(ul_Index, EDI_Csz_Path_GameObject);
						if(ul_Index != BIG_C_InvalidIndex)
						{
							m_cbCompletion.FillCombo(BIG_FirstFile(ul_Index));
						}
					}
				}

				//read type
				LOA_ReadULong(&pc_Buf);
			}
			// wol parsing end --
		}
		else	
		{
			//it's a WOW
			ul_Index = IsSubGAODirectoryPresent(mpo_Gao->pst_World, strSubDirName);
			m_cbCompletion.FillCombo(ul_Index);
		}
	}
	else
	{
		ul_File = IsSubGAODirectoryPresent(mpo_Gao->pst_World, strSubDirName);
		m_cbCompletion.FillCombo(ul_File);
	}
#else
	m_cbCompletion.FillCombo(ul_File);
#endif
	m_cbCompletion.SetWindowText(strGaoName.c_str());

	// -------------------------------------------------------------------------
	// Setup OKButton
	m_OKButton = (CButton*) GetDlgItem(IDOK);
	// -------------------------------------------------------------------------
	// Setup RadioButton
#ifdef JADEFUSION
	if(m_iTypeOfDuplication == -1)
	CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
	else
		CheckRadioButton(IDC_RADIO1, IDC_RADIO2, m_iTypeOfDuplication ? IDC_RADIO2 : IDC_RADIO1);
#else
	CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO2);
#endif
	m_iTypeOfDuplication = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2) ;

	// -------------------------------------------------------------------------
	// Setup TreeCtrl with current GAO
	m_pTreeCtl = (CTreeCtrl *) GetDlgItem(IDC_TREE);
	m_pTreeCtl->SetImageList(&(M_MF()->mo_FileImageList), TVSIL_NORMAL);
	m_pTreeCtl->SetFont(&M_MF()->mo_Fnt);

	mi_IconGro = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicObject, 0);
	mi_IconGrm = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicMaterial, 0);
	mi_IconTex = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtTexture1, 0);
	mi_IconGao = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGameObject, 0);
	mi_IconGrl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtGraphicLight, 0);
	mi_IconMap = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLMap, 0);
	mi_IconCob = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLObject, 0);
	mi_IconGam = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLGMAT, 0);
	mi_IconCin = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLInstance, 0);
	mi_IconCmd = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtCOLSetModel, 0);
	mi_IconSkl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtSkeleton, 0);
	mi_IconShp = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtShape, 0);
	mi_IconAck = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtActionKit, 0);
	mi_IconTrl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtEventAllsTracks, 0);
	mi_IconOin = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtAIEngineInstance, 0);
	mi_IconOva = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtAIEngineVars, 0 );
	mi_IconMdl = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtAIEditorModel, 0);
	mi_IconSnk = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtSoundBank, 0);
	mi_IconMsk = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtSoundMetaBank, 0);
	mi_IconGrp = M_MF()->i_GetIconImage(NULL, EDI_Csz_ExtObjGroups, 0);

	//UpdateTree(m_iTypeOfDuplication);

	m_bDlgIsInitialized = TRUE ;

	//pour obtenir un nouveau nom au départ
	VerifyName();
	//disable OKbutton to force name change
	//m_OKButton->EnableWindow(FALSE);

	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_NameDialogDuplicate::OnBnClickedRadio()
{
	if(!mpo_Gao || !m_bDlgIsInitialized) 
		return;

	// Empty TreeCtrl
	m_pTreeCtl->DeleteAllItems();

	// Get the radio button that has been clicked
	m_iTypeOfDuplication = GetCheckedRadioButton(IDC_RADIO1, IDC_RADIO2);
	
	// Update the content of the tree
	UpdateTree(m_iTypeOfDuplication) ;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_NameDialogDuplicate::OnEnChangeCombo()
{
	if(! mpo_Gao || ! m_bDlgIsInitialized) 
		return;

	CString strGaoName;

	// Empty TreeCtrl
	m_pTreeCtl->DeleteAllItems();

	// Get the text typed in combobox
	m_cbCompletion.GetWindowText(strGaoName);
	
	// Remove file extension if present
	if (strGaoName.Find(EDI_Csz_ExtGameObject) != -1)
#ifdef JADEFUSION
	{
		strGaoName.Replace(EDI_Csz_ExtGameObject, "");
		m_cbCompletion.SetWindowText(strGaoName);
	}
#else
		strGaoName.Delete(strGaoName.GetLength()-strlen(EDI_Csz_ExtGameObject), strlen(EDI_Csz_ExtGameObject));
#endif
	// Update the content of the tree
	UpdateTree(m_iTypeOfDuplication) ;

	//Verify new typed name
	VerifyName();
}

void EDIA_cl_NameDialogDuplicate::OnBnClickedOk()
{
	if(! mpo_Gao || ! m_bDlgIsInitialized || ! mpo_View)
		return ;
	
#ifdef JADEFUSION
	m_iTypeOfDuplication = GetCheckedRadioButton(IDC_RADIO1,IDC_RADIO2) - TYPEDUPLICATION_COPY ;
	CString strPreviousGAOName;

	// Strip trailing spaces in variable mo_Name
	m_strGaoName.TrimLeft();
	m_strGaoName.TrimRight();

	strPreviousGAOName = m_strGaoName ;
#else
	CString strInitialGaoName(mpo_Gao->sz_Name);
	CString strGaoName, strPreviousGAOName;

	// Remove file extension if present
	if (strInitialGaoName.Find(EDI_Csz_ExtGameObject) != -1)
		strInitialGaoName.Delete(strInitialGaoName.GetLength()-strlen(EDI_Csz_ExtGameObject), strlen(EDI_Csz_ExtGameObject));

	// Get the text typed in combobox
	m_cbCompletion.GetWindowText(strGaoName);

	// Strip trailing spaces in variable mo_Name
	strGaoName.TrimLeft();
	strGaoName.TrimRight();

	strPreviousGAOName = strGaoName ;
#endif

#ifdef JADEFUSION
	if ( m_strInitialGaoName != m_strGaoName && m_cbCompletion.IsNamePresent(m_strGaoName) )
	{
		CString strMessage;
		strMessage.Format("'%s' is already used in this wow", strPreviousGAOName);
		GetDlgItem(IDC_STATIC)->SetWindowText(strMessage);

		m_cbCompletion.SetWindowText(m_strGaoName);
		m_cbCompletion.GetFocus();
		m_cbCompletion.SetEditSel(m_strGaoName.GetLength(),m_strGaoName.GetLength()); 
		return;

	}
#else
	if ( strInitialGaoName != strGaoName && m_cbCompletion.IsNamePresent(strGaoName) )
	{
		CString strMessage;
		strMessage.Format("'%s' is already used in this wow", strPreviousGAOName);
		GetDlgItem(IDC_STATIC)->SetWindowText(strMessage);

		m_cbCompletion.SetWindowText(strGaoName);
		m_cbCompletion.GetFocus();
		m_cbCompletion.SetEditSel(strGaoName.GetLength(),strGaoName.GetLength()); 


	}
	else
	{
		// Verify format of GAO name 
		if(BIG_b_CheckName((LPSTR)(LPCSTR) strGaoName))
		{
			char asz_NewName[BIG_C_MaxLenPath];

			if(strGaoName.Find(EDI_Csz_ExtGameObject) == -1) 
				sprintf(asz_NewName, "%s%s", (LPSTR)(LPCSTR) strGaoName, EDI_Csz_ExtGameObject);
			else
				sprintf(asz_NewName, "%s", (LPSTR)(LPCSTR) strGaoName);
				
			// Rename GAO if different
			if ( L_stricmp(mpo_Gao->sz_Name, asz_NewName) != 0 )
				OBJ_GameObject_Rename(mpo_Gao, asz_NewName);

			// If COPY was asked, copy each element of GAO & propose new name (except for materials)
			if ( m_iTypeOfDuplication == TYPEDUPLICATION_COPY )	
			{
				// COPY lights
				if ( mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
					mpo_View->Selection_DuplicateGro(F3D_Duplicate_Light | F3D_Duplicate_AllLocation);

				if ( mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu )
				{
					// COPY geometry
					if ( mpo_Gao->pst_Base->pst_Visu->pst_Object->i->ul_Type ==	GRO_Geometric )			
						mpo_View->Selection_DuplicateGro(F3D_Duplicate_Geometry | F3D_Duplicate_AllLocation);
				
					// COPY particle generator
					if ( mpo_Gao->pst_Base->pst_Visu->pst_Object->i->ul_Type ==	GRO_ParticleGenerator )			
						mpo_View->Selection_DuplicateGro(F3D_Duplicate_PAG | F3D_Duplicate_AllLocation);
				}


					//Copy Cob
					if (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap )
					{
						COL_tdst_ColMap *pst_ColMap = ((COL_tdst_Base *) mpo_Gao->pst_Extended->pst_Col)->pst_ColMap;
						if(pst_ColMap)
						{
							if(pst_ColMap->uc_NbOfCob)
							{
								for(int i = 0; i < pst_ColMap->uc_NbOfCob; i++)
								{
									if(!pst_ColMap->dpst_Cob[i]) continue;
									COL_tdst_Cob *pst_Cob = pst_ColMap->dpst_Cob[i];
									//SEL_pst_AddItem(pst_Selection, pst_Cob, SEL_C_SIF_Cob);
									// on envoi TRUE pour dire a la fct que c'est un GAO qui est Sel et non un COB
									mpo_View->Selection_DuplicateCob(i);
									//SEL_DelItem(pst_Selection, pst_Cob);
								}
							}
						}
					}


			}
#endif			
			// Exit dialog
			OnOK();
#ifndef JADEFUSION
		}
	}
#endif
}	
/*
=======================================================================================================================
=======================================================================================================================
*/

void EDIA_cl_NameDialogDuplicate::VerifyName(void)
{

	CString strPreviousGAOName;

	// Get the text typed in combobox
	m_cbCompletion.GetWindowText(m_strGaoName);
	// Strip trailing spaces in variable mo_Name
	m_strGaoName.TrimLeft();
	m_strGaoName.TrimRight();

	strPreviousGAOName = m_strGaoName ;

	if ( m_strInitialGaoName != m_strGaoName && m_cbCompletion.IsNamePresent(m_strGaoName) )
	{
		CString strMessage;
		strMessage.Format("'%s' is already used in this wow", strPreviousGAOName);
		GetDlgItem(IDC_STATIC3)->SetWindowText(strMessage);

		m_cbCompletion.SetWindowText(m_strGaoName);
		m_cbCompletion.GetFocus();
		m_cbCompletion.SetEditSel(m_strGaoName.GetLength(),m_strGaoName.GetLength()); 
		//disable OK button - mis en comment car l'auto completion propose un new name
		//m_OKButton->EnableWindow(FALSE);
	}
	else
	{
		//enable OK button
		m_OKButton->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC3)->SetWindowText("");

	}
#ifdef JADEFUSION
	//test if we exceed the MAX file name length
	if(m_strGaoName.GetLength() > BIG_C_MaxLenName)
	{
		GetDlgItem(IDC_STATIC3)->SetWindowText("The length of the object name exceed 64 characters !");
		m_OKButton->EnableWindow(FALSE);
	}
#endif
	//voir le changement de nom dû à l'auto incrémentation
	
	// Empty TreeCtrl
	m_pTreeCtl->DeleteAllItems();
	UpdateTree(m_iTypeOfDuplication) ;
}

/*
=======================================================================================================================
=======================================================================================================================
*/

void EDIA_cl_NameDialogDuplicate::UpdateTree(int iTypeOfDuplication)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString					strGaoName;
#ifdef JADEFUSION
	CString					strObjName;
	int						i_Str;
#endif
	HTREEITEM			    h_Parent, hItem, hItemSub, hItemSubSub;
	
	GRO_tdst_Struct		    *pst_Gro;
	COL_tdst_ColMap		    *pst_ColMap;
	COL_tdst_Instance	    *pst_Zdm;
	ANI_st_GameObjectAnim   *pst_Anim;
	AI_tdst_Instance        *pst_AI;

	BIG_INDEX			    ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if( ! mpo_Gao ) 
		return;

	m_cbCompletion.GetWindowText(strGaoName);

	// Insert root of the tree
	h_Parent = m_pTreeCtl->InsertItem(strGaoName, mi_IconGao, mi_IconGao);
	m_pTreeCtl->SetItemData(h_Parent, (DWORD) mpo_Gao);

	// Bring value to boolean value ( 0 or 1 )
	iTypeOfDuplication = iTypeOfDuplication - TYPEDUPLICATION_COPY ;

	// Insert leaves of the tree
	if(mpo_Gao->pst_Base && mpo_Gao->pst_Base->pst_Visu)
	{
		if (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
		{
			pst_Anim = mpo_Gao->pst_Base->pst_GameObjectAnim;
			if (pst_Anim->pst_SkeletonModel)
			{
				hItem = m_pTreeCtl->InsertItem(pst_Anim->pst_SkeletonModel->sz_Name, mi_IconSkl, mi_IconSkl, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Anim->pst_SkeletonModel);
			}
			if (pst_Anim->pst_Shape)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_Shape);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconShp, mi_IconShp, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Anim->pst_Shape);
				}
			}
			if (pst_Anim->pst_ActionKit)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Anim->pst_ActionKit );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconAck, mi_IconAck, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Anim->pst_ActionKit);
				}
			}
		}
		else
		{
			pst_Gro = mpo_Gao->pst_Base->pst_Visu->pst_Object;
			if(pst_Gro)
			{   
				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ? GRO_sz_Struct_GetName(pst_Gro) : strGaoName, mi_IconGro, mi_IconGro, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Gro);
			}

			pst_Gro = mpo_Gao->pst_Base->pst_Visu->pst_Material;
			if(pst_Gro)
			{
				hItem = m_pTreeCtl->InsertItem(GRO_sz_Struct_GetName(pst_Gro), mi_IconGrm, mi_IconGrm, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Gro);
			}
		}
	}

	// Insert leaves of the tree
	if ( mpo_Gao->pst_Extended )
	{
		if((mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Events) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Gao->pst_Extended->pst_Events->pst_ListTracks );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = m_pTreeCtl->InsertItem(strGaoName, mi_IconTrl, mi_IconTrl, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) mpo_Gao->pst_Extended->pst_Events->pst_ListTracks);
			}

			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Gao->pst_Extended->pst_Events->pst_ListParam );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconTrl, mi_IconTrl, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) mpo_Gao->pst_Extended->pst_Events->pst_ListParam);
			}
		}
		if((mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights) )
		{
			pst_Gro = mpo_Gao->pst_Extended->pst_Light;
			if(pst_Gro)
			{
				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ? GRO_sz_Struct_GetName(pst_Gro) : strGaoName, mi_IconGrl, mi_IconGrl, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Gro);
			}
		}

		if( (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_Group) )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) mpo_Gao->pst_Extended->pst_Group );
			if(ul_Index != BIG_C_InvalidIndex)
			{
				hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconGrp, mi_IconGrp, h_Parent);
				m_pTreeCtl->SetItemData(hItem, (DWORD) mpo_Gao->pst_Extended->pst_Group );
			}
		}

		if ( (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap) && (mpo_Gao->pst_Extended->pst_Col) )
		{
			pst_ColMap = ((COL_tdst_Base *) mpo_Gao->pst_Extended->pst_Col)->pst_ColMap;
			if(pst_ColMap)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(strGaoName, mi_IconMap, mi_IconMap, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_ColMap);

					if(pst_ColMap->uc_NbOfCob)
					{
						for(int i = 0; i < pst_ColMap->uc_NbOfCob; i++)
						{
							if(!pst_ColMap->dpst_Cob[i]) continue;
							ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap->dpst_Cob[i]);
							if(ul_Index == BIG_C_InvalidIndex) continue;

							hItemSub = m_pTreeCtl->InsertItem(iTypeOfDuplication ? BIG_NameFile(ul_Index) : strGaoName + EDI_Csz_ExtCOLObject, mi_IconCob, mi_IconCob, hItem);
							m_pTreeCtl->SetItemData(hItemSub, (DWORD) pst_ColMap->dpst_Cob[i]);

							if(pst_ColMap->dpst_Cob[i]->pst_GMatList)
							{
								hItemSubSub = m_pTreeCtl->InsertItem(pst_ColMap->dpst_Cob[i]->sz_GMatName, mi_IconGam, mi_IconGam, hItemSub);
								m_pTreeCtl->SetItemData(hItemSubSub, (DWORD) pst_ColMap->dpst_Cob[i]->pst_GMatList);
							}
						}
					}
				}
			}
		}

		if ( (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_ZDM) && (mpo_Gao->pst_Extended->pst_Col)	)
		{
			pst_Zdm = ((COL_tdst_Base *) mpo_Gao->pst_Extended->pst_Col)->pst_Instance;
			if(pst_Zdm)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm);
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconCin, mi_IconCin, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_Zdm);

					if(pst_Zdm->pst_ColSet)
					{
						ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Zdm->pst_ColSet);
						if(ul_Index != BIG_C_InvalidIndex)
						{
							hItemSub = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconCmd, mi_IconCmd, hItem);
							m_pTreeCtl->SetItemData(hItemSub, (DWORD) pst_Zdm->pst_ColSet);
						}
					}
				}
			}
		}

		if ( (mpo_Gao->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) && (mpo_Gao->pst_Extended->pst_Ai) )
		{
			pst_AI = (AI_tdst_Instance *) mpo_Gao->pst_Extended->pst_Ai;
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI );
			if(ul_Index != BIG_C_InvalidIndex)
			{
#ifdef JADEFUSION	
				strObjName = BIG_NameFile(ul_Index);
				i_Str = strObjName.Find('.');
				if(i_Str)
					strObjName = strObjName.Left(i_Str);
				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ?	strObjName : strGaoName, mi_IconOin, mi_IconOin, h_Parent);

#else				
				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ? BIG_NameFile(ul_Index) : strGaoName, mi_IconOin, mi_IconOin, h_Parent);
#endif
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_AI);
			}

			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_VarDes );
			if(ul_Index != BIG_C_InvalidIndex)
			{
#ifdef JADEFUSION
				strObjName = BIG_NameFile(ul_Index);
				i_Str = strObjName.Find('.');
				if(i_Str)
					strObjName = strObjName.Left(i_Str);

				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ? strObjName : strGaoName, mi_IconOva, mi_IconOva, h_Parent);
#else
				hItem = m_pTreeCtl->InsertItem(iTypeOfDuplication ? BIG_NameFile(ul_Index) : strGaoName, mi_IconOva, mi_IconOva, h_Parent);
#endif
				m_pTreeCtl->SetItemData(hItem, (DWORD) pst_AI->pst_VarDes);
			}

			if(pst_AI->pst_Model)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_Model );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconMdl, mi_IconMdl, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_AI->pst_Model);
				}
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_AI->pst_Model->pst_VarDes );
				if(ul_Index != BIG_C_InvalidIndex)
				{
					hItem = m_pTreeCtl->InsertItem(BIG_NameFile(ul_Index), mi_IconOva, mi_IconOva, h_Parent);
					m_pTreeCtl->SetItemData(hItem, (DWORD) pst_AI->pst_VarDes);
				}
			}
		}
	}

	m_pTreeCtl->Expand( h_Parent, TVE_EXPAND );
}

#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_NameDialogDuplicate::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 290;
	lpMMI->ptMinTrackSize.y = 390;
	lpMMI->ptMaxTrackSize.y = 390;

}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_NameDialogDuplicate::OnSize(UINT a, int cx, int cy)
{
	/*~~~~~~~~~~~*/
	CRect	Size, Size2;
	int		i_Shift;
	/*~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnSize(a, cx, cy);

	if(!m_cbCompletion) return;
	if(!GetDlgItem(IDCANCEL)) return;

	//Replace CANCEL Button
	GetDlgItem(IDCANCEL)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDCANCEL)->MoveWindow((cx - Size.Width() - 36), Size.top, Size.Width(), Size.Height());


	//get shifting
	GetDlgItem(IDCANCEL)->GetWindowRect(Size2);
	ScreenToClient(Size2);
	i_Shift = Size2.left - Size.left;
	//get shifting

	//Replace Copy Checkbox
	GetDlgItem(IDC_RADIO1)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_RADIO1)->MoveWindow((cx - Size.Width() - 74), Size.top, Size.Width(), Size.Height());

	//replace ComboBox
	m_cbCompletion.GetWindowRect(Size);
	ScreenToClient(Size);
	m_cbCompletion.MoveWindow(Size.left, Size.top, Size.Width() + i_Shift, Size.Height());

	//replace TreeCtrl
	GetDlgItem(IDC_TREE)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_TREE)->MoveWindow(Size.left, Size.top, Size.Width() + i_Shift, Size.Height());

	//replace Static
	GetDlgItem(IDC_STATIC)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_STATIC)->MoveWindow(Size.left, Size.top, Size.Width() + i_Shift, Size.Height());

	//replace Static1
	GetDlgItem(IDC_STATIC1)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_STATIC1)->MoveWindow(Size.left, Size.top, Size.Width() + i_Shift, Size.Height());

	//replace Static2
	GetDlgItem(IDC_STATIC2)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_STATIC2)->MoveWindow(Size.left, Size.top, Size.Width() + i_Shift, Size.Height());

	//replace Static3
	GetDlgItem(IDC_STATIC3)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDC_STATIC3)->MoveWindow(Size.left, Size.top, Size.Width() + i_Shift, Size.Height());

	//clear static text message
	((CStatic*) GetDlgItem(IDC_STATIC3))->SetWindowText("");
}
#endif

/*$4
***********************************************************************************************************************
***********************************************************************************************************************
*/


/*
=======================================================================================================================
=======================================================================================================================
*/
EDIA_cl_NameDialogCompletion::EDIA_cl_NameDialogCompletion(WOR_tdst_World *pst_World, int iType, OBJ_tdst_GameObject *pst_GO ) :
EDIA_cl_BaseDialog(DIALOGS_IDD_NAMECOMPLETION)
{
	m_pWorld = pst_World;
	m_pst_GO = pst_GO;
	m_iType = iType ;

}

BEGIN_MESSAGE_MAP(EDIA_cl_NameDialogCompletion, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_EDITCHANGE(IDC_COMBO, OnEnChangeCombo)
#ifdef JADEFUSION
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
#endif
END_MESSAGE_MAP()

/*
=======================================================================================================================
=======================================================================================================================
*/
BOOL EDIA_cl_NameDialogCompletion::OnInitDialog(void)
{
	ULONG ul_File;
	char strNewNamePrefix[BIG_C_MaxLenName] = "";
	char strSubDirName[BIG_C_MaxLenPath] = "";
	
	EDIA_cl_BaseDialog::OnInitDialog();

	// -------------------------------------------------------------------------
	// Disable OKButton to force name completion
	m_OKButton = (CButton*) GetDlgItem(IDOK);
	m_OKButton->EnableWindow(FALSE);
	// -------------------------------------------------------------------------

	//rename GAO
	if(m_pst_GO)
	{
		strcpy(strNewNamePrefix, m_pst_GO->sz_Name);
		char *psz_Tmp = strrchr(strNewNamePrefix, '.');
		if(psz_Tmp)
			*psz_Tmp = 0;
	}
	else	//enter name for new GAO
	{
		// Setup new GAO name
		if ( strNewNamePrefix[0] != '\0' )
		{
			if ( m_iType == DLG_LIGHT )
				strcat(strNewNamePrefix,"_LUM_");
		}
		else
		{
			//strcpy(strNewNamePrefix, OBJECTNAME_NOTSET) ;

			// Get GAO world name
			//strcpy(strNewNamePrefix, m_pWorld->sz_Name);
			//char *psz_Tmp = strrchr(strNewNamePrefix, '.');
			//if(psz_Tmp)
			//	*psz_Tmp = 0;
			//strcat(strNewNamePrefix,"_");
		}
	}


	// -------------------------------------------------------------------------
	
	ul_File = IsSubGAODirectoryPresent(m_pWorld, strSubDirName);

	// -------------------------------------------------------------------------
	// Setup working directory where we are getting the GAOs
	char strName[BIG_C_MaxLenPath] = "";
	sprintf(strName,"(List of GAO coming from:\r\n %s)", strSubDirName);
	GetDlgItem(IDC_STATIC)->SetWindowText(strName);

	// -------------------------------------------------------------------------
	// Setup ComboBox with current GAO and list of GAO in WOW
	m_cbCompletion.SubclassDlgItem(IDC_COMBO, this);
	m_cbCompletion.FillCombo(ul_File);
	m_cbCompletion.SetWindowText(strNewNamePrefix);

	return TRUE;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_NameDialogCompletion::OnEnChangeCombo()
{
	m_OKButton->EnableWindow(TRUE);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_NameDialogCompletion::OnBnClickedOk()
{
	CString strPreviousName;

	// Get the text typed in combobox
	m_cbCompletion.GetWindowText(m_strName);

	// Strip trailing spaces in variable mo_Name
	m_strName.TrimLeft();
	m_strName.TrimRight();

	strPreviousName = m_strName;

	if ( m_cbCompletion.IsNamePresent(m_strName) )
	{
		CString strMessage;
		strMessage.Format("'%s' is already used in this wow", strPreviousName);
		GetDlgItem(IDC_STATIC)->SetWindowText(strMessage);
		
		m_cbCompletion.SetWindowText(m_strName);
		m_cbCompletion.GetFocus();
		m_cbCompletion.SetEditSel(m_strName.GetLength(),m_strName.GetLength()); 
	}
	else
	{
		// Verify format of GAO name & exit dialog
		if(BIG_b_CheckName((LPSTR)(LPCSTR) m_strName))
			OnOK();
	}
}
#ifdef JADEFUSION
/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_NameDialogCompletion::OnGetMinMaxInfo(MINMAXINFO FAR *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 305;
	lpMMI->ptMinTrackSize.y = 122;
	lpMMI->ptMaxTrackSize.y = 122;

}

/*
=======================================================================================================================
=======================================================================================================================
*/
void EDIA_cl_NameDialogCompletion::OnSize(UINT a, int cx, int cy)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	CRect	Size, Size2;
	int		i_Shift;
	/*~~~~~~~~~~~~~~~~~~~*/

	EDIA_cl_BaseDialog::OnSize(a, cx, cy);
	if(!m_cbCompletion) return;

	//Replace CANCEL Button
	GetDlgItem(IDCANCEL)->GetWindowRect(Size);
	ScreenToClient(Size);
	GetDlgItem(IDCANCEL)->MoveWindow((cx - Size.Width() - 43), Size.top, Size.Width(), Size.Height());

	//get shifting
	GetDlgItem(IDCANCEL)->GetWindowRect(Size2);
	ScreenToClient(Size2);
	i_Shift = Size2.left - Size.left;
	//get shifting

	//replace ComboBox
	m_cbCompletion.GetWindowRect(Size);
	ScreenToClient(Size);
	m_cbCompletion.MoveWindow(Size.left, Size.top, Size.Width() + i_Shift, Size.Height());
}
#endif
/*
=======================================================================================================================
=======================================================================================================================
*/
ULONG IsSubGAODirectoryPresent(WOR_tdst_World* pWorld, char* pstrSubDirName)
{
	BIG_INDEX index;

	char *psz_Tmp ;
	
	char asz_Name[BIG_C_MaxLenName];
	char asz_TmpName[BIG_C_MaxLenName];

	char asz_DirPath[BIG_C_MaxLenPath] = "";
	char asz_TmpDirPath[BIG_C_MaxLenPath] = "";

	// Get GAO world name
	strcpy(asz_Name, pWorld->sz_Name);
	psz_Tmp = strrchr(asz_Name, '.');
	if(psz_Tmp) *psz_Tmp = 0;
	strcpy(asz_TmpName, asz_Name);
	
	// Get GAO world directory path
	WOR_GetPath(pWorld, asz_DirPath);
	strcpy(asz_TmpDirPath, asz_DirPath);

	// Check if SubDirName is already included in GAO world directory path
	if ( strstr(asz_DirPath, pstrSubDirName) == NULL )
	{
		// Add SubDirName to path
		strcat(asz_TmpDirPath, "/");
		strcat(asz_TmpDirPath, pstrSubDirName);

		// Add SubDirName to name
		strcat(asz_TmpName, "/");
		strcat(asz_TmpName, pstrSubDirName);
	}

	// Check if TmpDirPath exists in world
	if ( BIG_ul_SearchDir(asz_TmpDirPath) != BIG_C_InvalidIndex)
	{
		// Add GameObject to path
		strcat(asz_TmpDirPath, "/");
		strcat(asz_TmpDirPath, EDI_Csz_Path_GameObject);

		// Add GameObject to name
		strcat(asz_TmpName, "/");
		strcat(asz_TmpName, EDI_Csz_Path_GameObject);
	}
	else
	{
		// Since SubDirName doesn't exist, remove from path
		strcpy(asz_TmpDirPath, asz_DirPath);

		// Since SubDirName doesn't exist, remove from Name
		strcpy(asz_TmpName, asz_Name);

		// Add GameObject to path
		strcat(asz_TmpDirPath, "/");
		strcat(asz_TmpDirPath, EDI_Csz_Path_GameObject);

		// Add GameObject to name
		strcat(asz_TmpName, "/");
		strcat(asz_TmpName, EDI_Csz_Path_GameObject);
	}
	
	if ( (index=BIG_ul_SearchDir(asz_TmpDirPath)) != BIG_C_InvalidIndex)
	{
		// Path exist, return firstfile & name 
		strcpy(pstrSubDirName,asz_TmpName);
		return BIG_FirstFile(index);
	}
	else
	{
		// Since GameObject doesn't exist, remove from path
		strcpy(asz_TmpDirPath, asz_DirPath);

		// Since SubDirName doesn't exist, remove from Name
		strcpy(asz_TmpName, asz_Name);
	}

	if ( (index=BIG_ul_SearchDir(asz_TmpDirPath)) != BIG_C_InvalidIndex)
	{
		// Path exist, return firstfile & name 
		strcpy(pstrSubDirName,asz_TmpName);
		return BIG_FirstFile(index);
	}

	// Nothing was found
	strcpy(pstrSubDirName,"");
	return BIG_C_InvalidIndex;
}

#endif /* ACTIVE_EDITORS */
