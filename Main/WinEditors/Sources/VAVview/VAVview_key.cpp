/*$T VAVview_key.cpp GC!1.68 01/06/00 13:32:56 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "DIAlogs/DIAvector_dlg.h"
#include "VAVview/VAVview.h"
#include "MATHs/MATH.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGfat.h"
#include "DIAlogs/DIAfile_dlg.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
CString EVAV_Key_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~*/
    BIG_KEY     ul_Key;
    BIG_INDEX   ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~*/

    ul_Key = *(BIG_KEY *) _p_Value;
    if(ul_Key == BIG_C_InvalidKey) return "INVALID KEY";
    ul_Index = BIG_ul_SearchKeyToFat(ul_Key);
    if(ul_Index == BIG_C_InvalidIndex) return "KEY NOT EXISTS";
    return BIG_NameFile(ul_Index);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVAV_Key_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    EDIA_cl_FileDialog  o_File("Choose File", 0, 0, 1, (char *) _po_Data->mi_Param1, (char *) _po_Data->mi_Param2);
    CString             o_Temp;
    BIG_INDEX           ul_Index;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(o_File.DoModal() == IDOK)
    {
        o_File.GetItem(o_File.mo_File, 0, o_Temp);
        ul_Index = BIG_ul_SearchFileExt(o_File.masz_FullPath, (char *) (LPCSTR) o_Temp);
        if(ul_Index == BIG_C_InvalidIndex)
            *(BIG_KEY *) _p_Value = BIG_C_InvalidKey;
        else
            *(BIG_KEY *) _p_Value = BIG_FileKey(ul_Index);
    }
}

#endif /* ACTIVE_EDITORS */
