/*$T PROframe_act.cpp GC! 1.100 03/08/01 11:16:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "PROframe.h"
#include "PROframe_act.h"
#include "Res/Res.h"
#include "Dialogs/DIAname_dlg.h"
#include "ENGine\Sources\WORld\WORaccess.h"
#include "EDImsg.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPRO_cl_Frame::OnAction(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDIA_cl_NameDialog	o_Dlg("Enter string to search");
	int					i;
	CString				o_Str;
	static CString		o_Last = "";
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Action)
	{
	case EPRO_ACTION_REFRESH:
		mpo_VarsView->mpo_ListBox->Invalidate();
		break;
	case EPRO_ACTION_FIND:
		o_Dlg.mo_Name = o_Last;
		if(o_Dlg.DoModal() == IDOK)
		{
			o_Last = o_Dlg.mo_Name;
			for(i = mpo_VarsView->mpo_ListBox->GetCurSel() + 1; i < mpo_VarsView->mpo_ListBox->GetCount(); i++)
			{
				mpo_VarsView->mpo_ListBox->GetText(i, o_Str);
				o_Dlg.mo_Name.MakeLower();
				o_Str.MakeLower();
				if(strstr((char *) (LPCSTR) o_Str, (char *) (LPCSTR) o_Dlg.mo_Name))
				{
					mpo_VarsView->mpo_ListBox->SetFocus();
					mpo_VarsView->mpo_ListBox->SetCurSel(i);
					mpo_VarsView->mpo_ListBox->SetTopIndex(i);
					mpo_VarsView->mpo_ListBox->Invalidate();
					return;
				}
			}
		};
		break;
	case EPRO_ACTION_CLEAR:
		mpo_VarsView->ResetList();
		break;
	case EPRO_ACTION_UNIVERSE:
		if(WOR_gst_Universe.pst_AI) i_OnMessage(EDI_MESSAGE_SELDATA, 0, (ULONG) WOR_gst_Universe.pst_AI);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPRO_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	switch(_ul_Action)
	{
	case EPRO_ACTION_UNIVERSE: if(!WOR_gst_Universe.pst_AI) return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EPRO_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	if(M_MF()->b_EditKey(_uw_Key)) return TRUE;

	switch(_uw_Key)
	{
	case VK_RETURN:
	case VK_DELETE:
	case VK_UP:
	case VK_DOWN:
		return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPRO_cl_Frame::OnToolBarCommand(UINT nID)
{
	switch(nID)
	{
	case EPRO_ID_FILTERINIT:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Init)
		{
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup &= ~EVAV_Filter_Init;
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup |= EVAV_Filter_Cur;
		}
		else
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup |= EVAV_Filter_Init;
		break;

	case EPRO_ID_FILTERCUR:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Cur)
		{
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup &= ~EVAV_Filter_Cur;
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup |= EVAV_Filter_Init;
		}
		else
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup |= EVAV_Filter_Cur;
		break;

	case EPRO_ID_FILTERPRIV:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Private)
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup &= ~EVAV_Filter_Private;
		else
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup |= EVAV_Filter_Private;
		break;

	case EPRO_ID_FILTERTRACK:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Track)
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup &= ~EVAV_Filter_Track;
		else
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup |= EVAV_Filter_Track;
		break;

	case EPRO_ID_FILTERSEP:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Sep)
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup &= ~EVAV_Filter_Sep;
		else
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup |= EVAV_Filter_Sep;
		break;

	case EPRO_ID_FILTERNO:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_NoFil)
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup &= ~EVAV_Filter_NoFil;
		else
			mpo_VarsView->mpo_ListBox->mi_DisplayGroup |= EVAV_Filter_NoFil;
		break;
	}

	/* Refresh */
	mpo_VarsView->SetItemList(mpo_VarsView->mpo_ListBox->mpo_ListItems);
	Invalidate();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EPRO_cl_Frame::OnToolBarCommandUI(UINT nID, CCmdUI *pUI)
{
	switch(nID)
	{
	case EPRO_ID_FILTERINIT:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Init)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EPRO_ID_FILTERCUR:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Cur)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EPRO_ID_FILTERPRIV:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Private)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EPRO_ID_FILTERTRACK:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Track)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EPRO_ID_FILTERSEP:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_Sep)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;

	case EPRO_ID_FILTERNO:
		if(mpo_VarsView->mpo_ListBox->mi_DisplayGroup & EVAV_Filter_NoFil)
			pUI->SetCheck(1);
		else
			pUI->SetCheck(0);
		break;
	}

	pUI->Enable(TRUE);
}

#endif
