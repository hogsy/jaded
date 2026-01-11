//------------------------------------------------------------------------------
// Filename   :SELectionframe_act.cpp
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: Implementation of SELectionframe_act
///            No description available ...
/// \see 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Header Files
//------------------------------------------------------------------------------
#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include <assert.h>
#include "SELectionframe.h"
#include "SELectionGridData.h"
#include "Res/Res.h"
#include "EDImainframe.h"
#include "LINKs/LINKtoed.h"
#include "EDImsg.h"
#include "PERForce\PERmsg.h"
#include "EDIeditors_infos.h"
#include "BIGfiles\BIGfat.h"

//------------------------------------------------------------------------------
// PreProcessor defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// externals and global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// statics and constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Class Implementation
//------------------------------------------------------------------------------


//------------------------------------------------------------
//   void ESELection_cl_Frame::OnAction(ULONG _ul_Action)
/// \author    Nbeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
void ESELection_cl_Frame::OnAction(ULONG _ul_Action)
{
	switch ( _ul_Action )
	{
		case ESELECTION_ACTION_INSERT:
		{
			InsertRowsGrid();
			break;
		}
		case ESELECTION_ACTION_SHOW_STANDARD:
		{
			for ( int i=eSEC_GENERAL; i < eSEC_STANDARD_PARAM ; i++ )
			{
				if ( mb_ShowStandard )
					mp_GridCtrl->SetColumnWidth(i, 0);
				else
				{
					mp_GridCtrl->SetColumnWidth(i, 10);
					mp_GridCtrl->AutoSizeColumn(i);
				}
			}
			mb_ShowStandard = ! mb_ShowStandard ;
			mp_GridCtrl->Refresh();
			break;
		}
		case ESELECTION_ACTION_SHOW_SHADING:
		{
			for ( int i=eSEC_STANDARD_PARAM; i < eSEC_SHADING_PARAM ; i++ )
			{
				if ( mb_ShowShading )
					mp_GridCtrl->SetColumnWidth(i, 0);
				else
				{
					mp_GridCtrl->SetColumnWidth(i, 10);
					mp_GridCtrl->AutoSizeColumn(i);
				}
			}
			mb_ShowShading = ! mb_ShowShading ;
			mp_GridCtrl->Refresh();
			break;
		}
		case ESELECTION_ACTION_SHOW_NORMALMAP:
		{
			for ( int i=eSEC_SHADING_PARAM; i < eSEC_NORMALMAP_PARAM ; i++ )
			{
				if ( mb_ShowNormalMap )
					mp_GridCtrl->SetColumnWidth(i, 0);
				else
				{
					mp_GridCtrl->SetColumnWidth(i, 10);
					mp_GridCtrl->AutoSizeColumn(i);
				}
			}
			mb_ShowNormalMap = ! mb_ShowNormalMap ;
			mp_GridCtrl->Refresh();
			break;
		}
		case ESELECTION_ACTION_SHOW_DETAILNORMALMAP:
		{
			for ( int i=eSEC_NORMALMAP_PARAM; i < eSEC_DETAILNORMALMAP_PARAM ; i++ )
			{
				if ( mb_ShowDetailNormalMap )
					mp_GridCtrl->SetColumnWidth(i, 0);
				else
				{
					mp_GridCtrl->SetColumnWidth(i, 10);
					mp_GridCtrl->AutoSizeColumn(i);
				}
			}
			mb_ShowDetailNormalMap = ! mb_ShowDetailNormalMap ;
			mp_GridCtrl->Refresh();
			break;
		}
		case ESELECTION_ACTION_SHOW_SPECULARMAP:
		{
			for ( int i=eSEC_DETAILNORMALMAP_PARAM; i < eSEC_SPECULARMAP_PARAM ; i++ )
			{
				if ( mb_ShowSpecular )
					mp_GridCtrl->SetColumnWidth(i, 0);
				else
				{
					mp_GridCtrl->SetColumnWidth(i, 10);
					mp_GridCtrl->AutoSizeColumn(i);
				}
			}
			mb_ShowSpecular = ! mb_ShowSpecular ;
			mp_GridCtrl->Refresh();
			break;
		}
		case ESELECTION_ACTION_SHOW_ENVIRONMENTMAP:
		{
			for ( int i=eSEC_SPECULARMAP_PARAM; i < eSEC_ENVIRONMENTMAP_PARAM ; i++ )
			{
				if ( mb_ShowEnvironmentMap )
					mp_GridCtrl->SetColumnWidth(i, 0);
				else
				{
					mp_GridCtrl->SetColumnWidth(i, 10);
					mp_GridCtrl->AutoSizeColumn(i);
				}
			}
			mb_ShowEnvironmentMap = ! mb_ShowEnvironmentMap ;
			mp_GridCtrl->Refresh();
			break;
		}
		case ESELECTION_ACTION_SHOW_MOSSMAP:
		{
			for ( int i=eSEC_ENVIRONMENTMAP_PARAM; i < eSEC_MOSS_PARAM ; i++ )
			{
				if ( mb_ShowMossMap )
					mp_GridCtrl->SetColumnWidth(i, 0);
				else
				{
					mp_GridCtrl->SetColumnWidth(i, 10);
					mp_GridCtrl->AutoSizeColumn(i);
				}
			}
			mb_ShowMossMap = ! mb_ShowMossMap ;
			mp_GridCtrl->Refresh();
			break;
		}
        case ESELECTION_ACTION_SHOW_RIMLIGHT:
        {
            for ( int i=eSEC_MOSS_PARAM; i < eSEC_RIMLIGHT_PARAM; i++ )
            {
                if ( mb_ShowRimLight )
                    mp_GridCtrl->SetColumnWidth(i, 0);
                else
				{
					mp_GridCtrl->SetColumnWidth(i, 10);
					mp_GridCtrl->AutoSizeColumn(i);
				}
            }
            mb_ShowRimLight = ! mb_ShowRimLight ;
            mp_GridCtrl->Refresh();
            break;
        }
		case ESELECTION_ACTION_SHOW_MESHPROCESSING:
		{
			for ( int i=eSEC_RIMLIGHT_PARAM; i < eSEC_MESHPROCESSING_PARAM; i++ )
			{
				if ( mb_ShowMeshProcessing )
					mp_GridCtrl->SetColumnWidth(i, 0);
				else
				{
					mp_GridCtrl->SetColumnWidth(i, 10);
					mp_GridCtrl->AutoSizeColumn(i);
				}
			}
			mb_ShowMeshProcessing = ! mb_ShowMeshProcessing ;
			mp_GridCtrl->Refresh();
			break;
		}
		case ESELECTION_ACTION_SAVE:
			SaveMaterials();
			break;

		case ESELECTION_ACTION_PERFORCE_EDIT:
			M_MF()->BeginWaitCursor();
			mp_GridCtrl->InitializeVectorSelectedItem();
			LINK_SendMessageToEditors(EPER_MESSAGE_EDIT_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(mp_GridCtrl))),0);
			M_MF()->EndWaitCursor();
			break;
		case ESELECTION_ACTION_PERFORCE_DIFF:
			mp_GridCtrl->InitializeVectorSelectedItem();
			LINK_SendMessageToEditors(EPER_MESSAGE_DIFF_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(mp_GridCtrl))),0);
			break;
		case ESELECTION_ACTION_PERFORCE_HISTORY:
			mp_GridCtrl->InitializeVectorSelectedItem();
			LINK_SendMessageToEditors(EPER_MESSAGE_SHOWHISTORY_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(mp_GridCtrl))),0);
			break;
		case ESELECTION_ACTION_PERFORCE_REVERT:
			mp_GridCtrl->InitializeVectorSelectedItem();
			LINK_SendMessageToEditors(EPER_MESSAGE_REVERT_SELECTED,((ULONG)(dynamic_cast<PER_CDataCtrl*>(mp_GridCtrl))),0);
			break;
		case ESELECTION_ACTION_SHOW_INMATERIAL:
		{
			EMAT_cl_Frame* po_MaterialEditor = (EMAT_cl_Frame*) M_MF()->po_GetEditorByType(EDI_IDEDIT_MAT, 0);
			po_MaterialEditor->mpo_MyView->IWantToBeActive(po_MaterialEditor);

			mp_GridCtrl->InitializeVectorSelectedItem();	
			HDATACTRLITEM hItem = mp_GridCtrl->GetFirstSelectedItem();
			BIG_INDEX ulIndex = mp_GridCtrl->GetItemReference(hItem);
			
			if ( ulIndex != BIG_C_InvalidKey )
				po_MaterialEditor->i_OnMessage(EDI_MESSAGE_SELFILE, BIG_ParentFile(ulIndex), ulIndex);
			break;
		}
		case ESELECTION_ACTION_SHOW_SELECTED:
		{
			CCellRange range = mp_GridCtrl->GetSelectedCellRange();
			for ( int iRow=1 ; iRow < mp_GridCtrl->GetRowCount() ; iRow++ )
			{
				if ( ! range.InRange(iRow, range.GetMinCol()) )
					mp_GridCtrl->SetRowHeight(iRow,0);
			}
			mp_GridCtrl->Refresh();
			break;
		}
		case ESELECTION_ACTION_SHOW_ALL:
		{		
			for ( int iRow=1 ; iRow < mp_GridCtrl->GetRowCount() ; iRow++ )
				mp_GridCtrl->SetRowHeight(iRow, 19);
			mp_GridCtrl->Refresh();
			break;
		}
	}
}

/*
===================================================================================================
===================================================================================================
*/

//------------------------------------------------------------
//   BOOL ESELection_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
BOOL ESELection_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	switch (_ul_Action)
	{
		case ESELECTION_ACTION_INSERT:

		case ESELECTION_ACTION_SHOW_STANDARD:
		case ESELECTION_ACTION_SHOW_SHADING:
		case ESELECTION_ACTION_SHOW_NORMALMAP:
		case ESELECTION_ACTION_SHOW_DETAILNORMALMAP:
		case ESELECTION_ACTION_SHOW_SPECULARMAP:
		case ESELECTION_ACTION_SHOW_ENVIRONMENTMAP:
		case ESELECTION_ACTION_SHOW_MOSSMAP:
        case ESELECTION_ACTION_SHOW_RIMLIGHT:
		case ESELECTION_ACTION_SHOW_MESHPROCESSING:
		
		case ESELECTION_ACTION_SAVE:

		case ESELECTION_ACTION_PERFORCE_EDIT:
		case ESELECTION_ACTION_PERFORCE_DIFF:
		case ESELECTION_ACTION_PERFORCE_HISTORY:
		case ESELECTION_ACTION_PERFORCE_REVERT:
			return TRUE;

		case ESELECTION_ACTION_SHOW_SELECTED:
		case ESELECTION_ACTION_SHOW_ALL:
		{
			CCellRange range = mp_GridCtrl->GetSelectedCellRange();
			if ( range.GetRowSpan() > 0 )
				return TRUE;

			return FALSE;
		}
		case ESELECTION_ACTION_SHOW_INMATERIAL:			
		{
			BOOL bSingleCellSelected = FALSE;
			CCellRange range = mp_GridCtrl->GetSelectedCellRange();
			if ( range.GetRowSpan() == 1 )
				return TRUE;
				
			return FALSE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------
//   BOOL ESELection_cl_Frame::b_KnowsKey(USHORT _uw_Key)
/// \author    NBeaufils
/// \date      23-Feb-05
/// \par       Behavior: 
///            No description available ...
/// \param     No description available... 
/// \see 
//------------------------------------------------------------
BOOL ESELection_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	BOOL b_Shift = _uw_Key & SHIFT;
	BOOL b_Ctrl = _uw_Key & CONTROL;
	
	switch ( _uw_Key )
	{
		case VK_INSERT:
		case VK_DELETE:
			return FALSE;
	}
	
	if(b_Ctrl || b_Shift) 
		return FALSE;

	return TRUE;
}

/*
===================================================================================================
===================================================================================================
*/

//------------------------------------------------------------
//   UINT ESELection_cl_Frame::ui_OnActionState(ULONG _ul_Action)
/// \author    NBeaufils
/// \date      2005-01-17
/// \par       Description: 
///            No description available ...
/// \param     No param description available... 
/// \see 
//------------------------------------------------------------
UINT ESELection_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	UINT ui_State = -1;

	switch(_ul_Action)
	{
		case ESELECTION_ACTION_SHOW_STANDARD:
			ui_State = DFCS_BUTTONCHECK | mb_ShowStandard ? DFCS_CHECKED : 0 ;
			break;
		case ESELECTION_ACTION_SHOW_SHADING:
			ui_State = DFCS_BUTTONCHECK | mb_ShowShading ? DFCS_CHECKED : 0 ;
			break;
		case ESELECTION_ACTION_SHOW_NORMALMAP:
			ui_State = DFCS_BUTTONCHECK | mb_ShowNormalMap ? DFCS_CHECKED : 0 ;
			break;
		case ESELECTION_ACTION_SHOW_DETAILNORMALMAP:
			ui_State = DFCS_BUTTONCHECK | mb_ShowDetailNormalMap ? DFCS_CHECKED : 0 ;
			break;
		case ESELECTION_ACTION_SHOW_SPECULARMAP:
			ui_State = DFCS_BUTTONCHECK | mb_ShowSpecular ? DFCS_CHECKED : 0 ;
			break;
		case ESELECTION_ACTION_SHOW_ENVIRONMENTMAP:
			ui_State = DFCS_BUTTONCHECK | mb_ShowEnvironmentMap ? DFCS_CHECKED : 0 ;
			break;
		case ESELECTION_ACTION_SHOW_MOSSMAP:
			ui_State = DFCS_BUTTONCHECK | mb_ShowMossMap ? DFCS_CHECKED : 0 ;
			break;
        case ESELECTION_ACTION_SHOW_RIMLIGHT:
            ui_State = DFCS_BUTTONCHECK | mb_ShowRimLight ? DFCS_CHECKED : 0 ;
            break;
		case ESELECTION_ACTION_SHOW_MESHPROCESSING:
			ui_State = DFCS_BUTTONCHECK | mb_ShowMeshProcessing ? DFCS_CHECKED : 0 ;
			break;
	}
	return ui_State;
}

#endif

