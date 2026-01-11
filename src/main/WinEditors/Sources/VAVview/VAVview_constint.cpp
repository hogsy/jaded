/*$T VAVview_constint.cpp GC!1.53 11/19/99 17:53:36 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "VAVview/VAVview.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"
#include "BAse/CLIbrary/CLImem.h"
#include "BAse/CLIbrary/CLIstr.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
static void GetNameWithId(char *_psz_Buf, int id, char *_psz_Name)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *psz_CpBuf, *psz_Beg;
    char    *psz_Temp, *psz_Temp1;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    psz_CpBuf = psz_Beg = L_strdup(_psz_Buf);
	L_strcpy(_psz_Name, "ERROR");

    while(*psz_Beg)
    {
        psz_Temp = L_strchr(psz_Beg, '\n');
        if(!psz_Temp) 
		{
			L_free(psz_CpBuf);
			return;
		}

        *psz_Temp = 0;
        psz_Temp1 = L_strchr(psz_Temp + 1, '\n');
        if(!psz_Temp1) 
		{
			L_free(psz_CpBuf);
			return;
		}

        *psz_Temp1 = 0;
        if(L_atol(psz_Temp + 1) == id) break;
        psz_Beg = psz_Temp1 + 1;
    }

    if(!*psz_Beg)
        L_strcpy(_psz_Name, "UNKNOWN CONSTANT");
    else
        L_strcpy(_psz_Name, psz_Beg);
    L_free(psz_CpBuf);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_ConstInt_DrawItem(EVAV_cl_ViewItem *_po_Item, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG   ul;
    char    asz_Name[200];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ul = *(ULONG *) _p_Value;
    if(_po_Item->mi_Param3 == 1)
        ul &= 0xFF;
    else if (_po_Item->mi_Param3 == 2)
        ul &= 0xFFFF;

	if(L_strchr((char *) _po_Item->mi_Param4, '¤'))
		GetNameWithId(L_strchr((char *) _po_Item->mi_Param4, '¤') + 1, ul, asz_Name);
	else
		GetNameWithId((char *) _po_Item->mi_Param4, ul, asz_Name);
    return asz_Name;
}


/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_ConstInt_FillSelect
(
    CWnd                *_po_Wnd,
    EVAV_cl_ViewItem    *_po_Data,
    void                *_p_Value,
    BOOL                _b_Fill
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CComboBox           *po_Combo;
    int                 iSel, iIndex;
	char				*psz_Beg, *psz_Buf, *psz_Buf1;
	char				*psz_Temp;
	char				asz_Sel[200];
	ULONG				ul_Sel, ul_Val;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    po_Combo = (CComboBox *) _po_Wnd;

    /* Fill the combo box with the list of all constants */
    if(_b_Fill)
    {
		psz_Buf = psz_Beg = L_strdup((char *) _po_Data->mi_Param4);
        ul_Sel = *(ULONG *) _p_Value;
		psz_Buf1 = L_strchr(psz_Buf, '¤');
		if(!psz_Buf1)
			psz_Buf1 = psz_Buf;
		else
			psz_Buf1++;
		GetNameWithId(psz_Buf1, ul_Sel, asz_Sel);
        po_Combo->ResetContent();

        /* Scan all constants */
		psz_Beg = psz_Buf1;
        while(*psz_Beg)
        {
			psz_Temp = L_strchr(psz_Beg, '\n');
			*psz_Temp = 0;
            iIndex = po_Combo->AddString(psz_Beg);
			psz_Beg = psz_Temp + 1;
			psz_Temp = L_strchr(psz_Beg, '\n');
			*psz_Temp = 0;
			ul_Val = L_atol(psz_Beg);
            po_Combo->SetItemData(iIndex, ul_Val);
			psz_Beg = psz_Temp + 1;
        }

        /* Select current function */
        po_Combo->SetTopIndex(0);
        po_Combo->SetCurSel(po_Combo->FindStringExact(-1, asz_Sel));
		L_free(psz_Buf);
        return;
    }

    /* Search the selected network, and change it */
    iSel = po_Combo->GetCurSel();
    if(iSel == -1) return;
    ul_Sel = po_Combo->GetItemData(iSel);

    switch( _po_Data->mi_Param3 )
    {
    case 1:
        *(UCHAR *) _p_Value = (UCHAR) ul_Sel;
        break;
    case 2:
        *(USHORT *) _p_Value = (USHORT) ul_Sel;
        break;
    default:
        *(ULONG *) _p_Value = ul_Sel;
        break;
    }

}

#endif /* ACTIVE_EDITORS */
