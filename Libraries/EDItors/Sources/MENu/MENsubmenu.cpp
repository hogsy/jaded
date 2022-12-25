/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    The submenu that appears when we clicked on a top menu category. This submenu draw its
            own items, by calling DrawAction of the frame menu.
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "MENin.h"
#include "MENsubmenu.h"
#include "MENlist.h"
#include "EDImainframe.h"

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_SubMenu::DrawItem(DRAWITEMSTRUCT *pdraw)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    CRect   o_Rect;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /*
     * The idem id must be decreased by one cause the first one is 1, and not 0. 0 is reserved to
     * indicate a cancel mode of the trackpopupmenu.
     */
    pdraw->itemID--;

    /* We compute the bounding rectangle of the item. We keep height, but give a constant width. */
	o_Rect = pdraw->rcItem;
	o_Rect.left += 4;
	o_Rect.right = o_Rect.left + 250;

    /*
     * Call drawaction of left list to draw the item (so the item will be displayed as in the
     * left menu
     */
	if(!mpo_MenuFrame) return;
    mpo_MenuFrame->mpo_ListDown->DrawAction(pdraw, o_Rect, mb_TextFromList, TRUE);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EMEN_cl_SubMenu::MeasureItem(MEASUREITEMSTRUCT *pmes)
{
	pmes->itemWidth = 250;
    pmes->itemHeight = M_MF()->mst_Ini.ui_HeightMenu;
	if(pmes->itemHeight < 14) pmes->itemHeight = 14;
	if(pmes->itemHeight > 24) pmes->itemHeight = 14;
}

#endif /* ACTIVE_EDITORS */
