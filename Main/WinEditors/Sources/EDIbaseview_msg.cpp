/*$T EDIbaseview_msg.cpp GC! 1.081 06/20/00 11:07:15 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIbaseview.h"
#include "EDIbaseframe.h"
#include "EDImainframe.h"
#include "EDImsg.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::SendMessageToEditors(ULONG ul_Message, ULONG ul_Param1, ULONG ul_Param2, int _i_Menu)
{
	/*~~*/
	int i;
	/*~~*/

	/* Without menus first */
	if(_i_Menu & 1)
	{
		for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
		{
			if((M_CurEd(mi_NumPane, i).po_Editor) && (M_CurEd(mi_NumPane, i).po_Editor->mst_Def.i_Type != EDI_IDEDIT_MENU))
				M_CurEd(mi_NumPane, i).po_Editor->i_OnMessage(ul_Message, ul_Param1, ul_Param2);
		}
	}

	/* And menus */
	if(_i_Menu & 2)
	{
		for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
		{
			if((M_CurEd(mi_NumPane, i).po_Editor) && (M_CurEd(mi_NumPane, i).po_Editor->mst_Def.i_Type == EDI_IDEDIT_MENU))
				M_CurEd(mi_NumPane, i).po_Editor->i_OnMessage(ul_Message, ul_Param1, ul_Param2);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_BaseView::OnRealIdle(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < M_CurNumEd(mi_NumPane); i++)
	{
		if(M_CurEd(mi_NumPane, i).po_Editor) M_CurEd(mi_NumPane, i).po_Editor->OnRealIdle();
	}

	/* Simulate one trame ending (to update some engine values) */
	OneTrameEnding();

	/* Just for fun */
	GameTest();
}

#endif /* ACTIVE_EDITORS */
