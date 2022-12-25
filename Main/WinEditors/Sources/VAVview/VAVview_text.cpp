/*$T VAVview_text.cpp GC!1.52 12/07/99 16:40:13 */

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
#include "ENGine/Sources/WORld/WORstruct.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/TEXT/TEXTload.h"
#include "DIAlogs/DIAlist2_dlg.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_Text_DrawItem(EVAV_cl_ViewItem *item, void *_p_Value)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    TEXT_tdst_Eval      *pst_Text;
    TEXT_tdst_OneText   *pst_Txt;
    int id;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Text = (TEXT_tdst_Eval *) _p_Value;
	if(pst_Text->i_FileKey == BIG_C_InvalidIndex) return "None";
	if(pst_Text->i_FileKey == 0) return "None";

    
    id = TEXT_i_GetOneTextIndex(pst_Text->i_FileKey);
    if(id < 0)
        return "TEXT NOT LOADED";
    pst_Txt = TEXT_gst_Global.pst_AllTexts[id];


    id = TEXT_i_GetEntryIndex(pst_Txt, pst_Text->i_Id);
    if(id == -1)
        return "TEXT UNKNOWN";

	item->psz_Help = pst_Txt->psz_Text + pst_Txt->pst_Ids[id].i_Offset;
    return pst_Txt->pst_Ids[id].asz_Name;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_Text_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_List2Dialog o_Dlg(LIST2_ModeText);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    o_Dlg.mp_TextRef = (TEXT_tdst_Eval *) _p_Value;
	o_Dlg.DoModal();
}

#endif /* ACTIVE_EDITORS */
