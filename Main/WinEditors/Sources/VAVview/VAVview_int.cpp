/*$T VAVview_int.cpp GC!1.39 06/25/99 12:29:00 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Treat a char/short/int, and display it in an int (signed) format.

    Note:   User Param1 = Born min £
            User Param2 = Born max £
            User Param3 = sizeof field (1 = char, 2 = short, 4 = int)
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "VAVview/VAVview.h"
#include "EDImainframe.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_Int_DrawItem(EVAV_cl_ViewItem *_po_Data, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    asz_Temp[100];
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(M_MF()->mst_Ini.b_IntAsHex) return EVAV_Hexa_DrawItem(_po_Data, _p_Value);

    switch(_po_Data->mi_Param3)
    {
    case -1:
        sprintf(asz_Temp, "%u", *((unsigned char *) _p_Value));
        break;

    case -2:
        sprintf(asz_Temp, "%u", *((unsigned short *) _p_Value));
        break;

    case -4:
        sprintf(asz_Temp, "%u", *((unsigned int *) _p_Value));
        break;

    case 1:
        sprintf(asz_Temp, "%d", *((char *) _p_Value));
        break;

    case 2:
        sprintf(asz_Temp, "%d", *((short *) _p_Value));
        break;

    case 4:
    default:
        sprintf(asz_Temp, "%d", *((int *) _p_Value));
        break;
    }

    return asz_Temp;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Int_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CEdit   *po_Edit;
    CString o_String;
    int     i_Res;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(M_MF()->mst_Ini.b_IntAsHex)
	{
		EVAV_Hexa_FillSelect(_po_Wnd, _po_Data, _p_Value, _b_Fill);
		return;
	}

    /* Want to fill ? */
    po_Edit = (CEdit *) _po_Wnd;
    if(_b_Fill)
    {
        po_Edit->SetWindowText(EVAV_Int_DrawItem(_po_Data, _p_Value));
        return;
    }

    po_Edit->GetWindowText(o_String);
    i_Res = L_atoi((char *) (LPCSTR) o_String);

    /* Born value */
    if(_po_Data->mi_Param1 || _po_Data->mi_Param2)
    {
        if(i_Res < _po_Data->mi_Param1)
            i_Res = _po_Data->mi_Param1;
        if(i_Res > _po_Data->mi_Param2)
            i_Res = _po_Data->mi_Param2;
    }

    /* Save it */
    switch(_po_Data->mi_Param3)
    {
    case 1:
	case -1:
        *((char *) _p_Value) = i_Res;
        break;

    case 2:
	case -2:
        *((short *) _p_Value) = i_Res;
        break;

    case 4:
	case -4:
    default:
        *((int *) _p_Value) = i_Res;
        break;
    }
}

#endif /* ACTIVE_EDITORS */
