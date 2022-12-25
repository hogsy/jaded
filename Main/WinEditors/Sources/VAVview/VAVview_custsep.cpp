/*$T VAVview_aifunc.cpp GC!1.41 08/04/99 15:54:55 */

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

/*
 ===================================================================================================
 ===================================================================================================
 */
CString EVAV_CustSep_DrawItem(EVAV_cl_ViewItem *, void *_p_Value)
{
	return " ";
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void EVAV_CustSep_FillSelect(CWnd *_po_Wnd, EVAV_cl_ViewItem *_po_Data, void *_p_Value, BOOL _b_Fill)
{
}

#endif /* ACTIVE_EDITORS */
