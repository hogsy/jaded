/*$T DIAname_dlg.cpp GC! 1.078 03/16/00 10:33:16 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "DIAlogs/DIAgrid_dlg.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "Res/Res.h"
#include "EDItors/Sources/OUTput/OUTframe.h"
#include <windowsx.h>
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "EDIeditors_infos.h"

BEGIN_MESSAGE_MAP(EDIA_cl_GridDialog, EDIA_cl_BaseDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, On_LVN_Notify)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemChanged)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnPaintMode)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

#ifdef JADEFUSION
extern ULONG	GRI_ul_CapaColor[128];
extern ULONG	GRI_CapaColor(char);
#else
extern "C" ULONG	GRI_ul_CapaColor[128];
extern "C" ULONG	GRI_CapaColor(char);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDIA_cl_GridDialog::EDIA_cl_GridDialog(void) :
	EDIA_cl_BaseDialog(DIALOGS_IDD_GRID)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GridDialog::OnInitDialog(void)
{
	CenterWindow();
	return EDIA_cl_BaseDialog::OnInitDialog();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GridDialog::OnClose()
{
	ShowWindow( SW_HIDE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GridDialog::DoDataExchange(CDataExchange *pDX)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl		*po_List;
	CComboBox		*po_Combo;
	int				item;
	EOUT_cl_Frame	*po_Out;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	CDialog::DoDataExchange(pDX);

	GRI_CapaColor(0);	// Force init of grid colors
	po_List = (CListCtrl *) GetDlgItem(IDC_LIST1);
	item = 0;

#define ADDC(agb) \
	{\
		char az[1024];\
		L_memset(az, ' ', 1024);\
		L_memcpy(az, agb, strlen(agb));\
		az[1023] = 0;\
		po_List->SetItemData(po_List->InsertItem(item, az), item++);\
	}

	ADDC("Vide");
	ADDC("Mur         | Zone Dangereuse");
	ADDC("Herbe Basse | Zone Safe");
	ADDC("Eau");
	ADDC("Herbe Haute | Muret Jack");
	ADDC("Ronces");
	ADDC("---         | Cache Raptor");
	ADDC("---         | Cache Bats");
	ADDC("---         | Inflammable");
	ADDC("Eau Basse");
	ADDC("Eau Herbe Haute");
	ADDC("Eau Herbe Basse");
	ADDC("Eau Mouche A Feu");
	ADDC("--");
	ADDC("--");
	ADDC("Pastis 51");

	po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	po_Combo = (CComboBox *) GetDlgItem(IDC_COMBO1);
	po_Combo->SetItemData(po_Combo->AddString("Normal (1)"), 0);
	po_Combo->SetItemData(po_Combo->AddString("Box (3*3)"), 1);
	po_Combo->SetItemData(po_Combo->AddString("Fill"), 2);
	po_Combo->SetCurSel(po_Out->mst_Ini.ul_GridPaintMode);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GridDialog::OnPaintMode(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CComboBox		*po_Combo;
	EOUT_cl_Frame	*po_Out;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
	po_Combo = (CComboBox *) GetDlgItem(IDC_COMBO1);
	po_Out->mst_Ini.ul_GridPaintMode = po_Combo->GetCurSel();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GridDialog::OnItemChanged(NMHDR *pNotifyStruct2, LRESULT *result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	NM_LISTVIEW		*pNotifyStruct;
	EOUT_cl_Frame	*po_Out;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pNotifyStruct = (NM_LISTVIEW *)pNotifyStruct2;
	if(pNotifyStruct->uNewState & LVIS_FOCUSED)
	{
		po_Out = (EOUT_cl_Frame *) M_MF()->po_GetEditorByType(EDI_IDEDIT_OUTPUT, 0);
		po_Out->mst_Ini.uc_GridPaintValue = pNotifyStruct->iItem;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDIA_cl_GridDialog::PreTranslateMessage(MSG *pMsg)
{
	return EDIA_cl_BaseDialog::PreTranslateMessage(pMsg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDIA_cl_GridDialog::On_LVN_Notify(NMHDR * pNotifyStruct, LRESULT * result)
{ 
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CListCtrl		*po_List;
	int				num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_List = (CListCtrl *) GetDlgItem(IDC_LIST1);
    switch(pNotifyStruct->code)
    {
	case NM_CUSTOMDRAW:
		{
			LPNMLVCUSTOMDRAW pCustomDraw = (LPNMLVCUSTOMDRAW) pNotifyStruct;
			num = pCustomDraw->nmcd.lItemlParam;
			*result = 0;
			switch(pCustomDraw->nmcd.dwDrawStage)
			{
			case CDDS_PREPAINT:
				*result = CDRF_NOTIFYITEMDRAW;
				break;
			case CDDS_ITEMPREPAINT:
				pCustomDraw->clrTextBk = GRI_ul_CapaColor[num] & 0x00FFFFFF;
				pCustomDraw->clrText = ((pCustomDraw->clrTextBk & 0xfefefe) + 0x888888) & 0xfefefe;
				break;
			}
		}
		break;

	default:
		*result = 0;
    }
}


#endif /* ACTIVE_EDITORS */
