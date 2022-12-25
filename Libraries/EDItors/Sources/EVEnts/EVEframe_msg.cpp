/*$T EVEframe_msg.cpp GC! 1.081 01/25/02 11:19:45 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "EDImainframe.h"
#include "EVEframe.h"
#include "EVEinside.h"
#include "EDImsg.h"
#include "BIGfiles/BIGread.h"
#include "BASe/CLIbrary/CLImem.h"
#include "LINKs/LINKmsg.h"
#include "EDIapp.h"
#include "EDIpaths.h"
#include "EVEmsg.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "AIinterp/Sources/EVEnts/EVEstruct.h"
#include "AIinterp/Sources/EVEnts/EVEload.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKtoed.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/ANImation/NIMtoTracks.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "AIinterp\Sources\AIdebug.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern int			EEVE_gi_CopyMode;
#ifdef JADEFUSION
extern BOOL		EVE_gb_ForceNotDone;
#else
extern "C" BOOL		EVE_gb_ForceNotDone;
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EEVE_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_tdst_DragDrop	*pst_DragDrop;
	OBJ_tdst_GameObject *pst_GAO;
	OBJ_tdst_GameObject *pst_Father;
	LINK_tdst_Pointer	*p2;
	int					i_Msg;
	int					p;
	BIG_INDEX			ul_File;
	ULONG				ul_Adr;
	EVE_tdst_Track		*pst_Track;
	EEVE_cl_Track		*po_Track;
	EMEN_cl_SubMenu		o_Menu(FALSE);
	CPoint				point;
	int					count, max;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Msg)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_FATHASCHANGED:
		if(!mpst_ListTracks) return 0;

		/* Have we changed the file ? */
		if((EDI_go_FHCFile.Lookup(mul_ListTracks, i_Msg)) || (EDI_go_FHCFile.Lookup(mul_GAO, i_Msg)))
		{
			switch(i_Msg)
			{
			case EDI_FHC_Delete:
				CloseAll();
				return TRUE;

			case EDI_FHC_AddUpdate:
			case EDI_FHC_Loaded:
				if(mul_GAO != BIG_C_InvalidIndex)
				{
					ul_File = mul_GAO;
					CloseAll();
					SetGAO((OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(ul_File)));
				}

				return TRUE;
			}
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_REFRESH:
		if(!mst_Ini.b_AnimMode)
		{
			ForceRefreshReal();
			mpo_VarsView->mpo_ListBox->RedrawWindow(NULL, NULL, RDW_UPDATENOW);
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_DATAHASCHANGED:
		if
		(
			(LINK_gx_PointersJustDeleted.Lookup(mpst_ListTracks, (void * &) p))
		||	(LINK_gx_PointersJustDeleted.Lookup(mpst_GAO, (void * &) p))
		)
		{
			CloseAll(FALSE);
			return TRUE;
		}

		if(!mb_LockUpdate && (LINK_gx_PointersJustUpdated.Lookup(mpst_GAO, (void * &) p)))
		{
			SetGAO(mpst_GAO, TRUE, TRUE);
			return TRUE;
		}

		return TRUE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANDRAGDROP:
		pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		if((pst_DragDrop->i_Type == EDI_DD_Long) || (pst_DragDrop->i_Type == EDI_DD_User)) return FALSE;

		/* A data */
		if(pst_DragDrop->i_Type == EDI_DD_Data)
		{
			_ul_Param2 = (ULONG) pst_DragDrop->i_Param2;

		case EDI_MESSAGE_CANSELDATA:
			if(mst_Ini.b_Lock) return FALSE;
			p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
			if(!p2) return FALSE;
			if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
			{
				pst_GAO = (OBJ_tdst_GameObject *) p2->pv_Data;
				if(ANI_b_IsGizmoAnim(pst_GAO, &pst_Father) && mpst_GAO == pst_Father) return TRUE;
				if
				(
					(OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Events))
				||	(OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Anims))
				) return TRUE;
			}

			return FALSE;
		}

		/* A game object */
		if(BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtGameObject))
		{
			pst_GAO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(pst_DragDrop->ul_FatFile));
			if(pst_GAO && ((int) pst_GAO != -1))
			{
				if
				(
					(OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Events))
				||	(OBJ_b_TestIdentityFlag(pst_GAO, OBJ_C_IdentityFlag_Anims))
				) return TRUE;
			}
		}

		/* A .nim file */
		else if(
                BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtAnimation) 
                //|| BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtEventAllsTracks)
            )
		{
			ul_Adr = LOA_ul_SearchAddress(BIG_PosFile(pst_DragDrop->ul_FatFile));
			if(ul_Adr && ((int) ul_Adr != -1))
			{
				return TRUE;
			}
		}

		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANSELFILE:
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGameObject))
		{
			if(LOA_ul_SearchAddress(BIG_PosFile(_ul_Param2)) != BIG_C_InvalidIndex) return TRUE;
		}

		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_ENDDRAGDROP:
		pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		_ul_Param2 = pst_DragDrop->ul_FatFile;

		if(pst_DragDrop->i_Type == EDI_DD_Data)
		{
			_ul_Param2 = (ULONG) pst_DragDrop->i_Param2;
	case EDI_MESSAGE_SELDATA:
			p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
			if(!p2) return FALSE;
			if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
			{
				/* Gizmo in anim edition */
				pst_GAO = (OBJ_tdst_GameObject *) _ul_Param2;
				if(ANI_b_IsGizmoAnim(pst_GAO, &pst_Father) && mpst_GAO == pst_Father) goto SelGizmo;

				p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
				ERR_X_Assert(p2);
				_ul_Param2 = LOA_ul_SearchIndexWithAddress((ULONG) p2->pv_Data);
				if(_ul_Param2 == BIG_C_InvalidIndex)	
				{
					if(((OBJ_tdst_GameObject *) pst_DragDrop->i_Param2)->ul_PrefabKey && (((OBJ_tdst_GameObject *) pst_DragDrop->i_Param2)->ul_PrefabKey != 0xFFFFFFFF))
					{
						pst_GAO = (OBJ_tdst_GameObject *) pst_DragDrop->i_Param2;
						if(pst_GAO && ((int) pst_GAO != -1))
						{
							if
							(
								(pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Events)
							||	(pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
							)
							{
								SetGAO(pst_GAO);
								break;
							}
						}
					}
					else
						break;
				}
			}
			else
				return FALSE;
		}

	case EDI_MESSAGE_SELFILE:

		/* GAO */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGameObject))
		{
			pst_GAO = (OBJ_tdst_GameObject *) LOA_ul_SearchAddress(BIG_PosFile(_ul_Param2));
			if(pst_GAO && ((int) pst_GAO != -1))
			{
				if
				(
					(pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Events)
				||	(pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
				)
				{
					SetGAO(pst_GAO);
					break;
				}
			}
		}
        /*$F
        else if
            (
                BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtAnimation) ||
                BIG_b_IsFileExtension(pst_DragDrop->ul_FatFile, EDI_Csz_ExtEventAllsTracks)
            )
		{
			ul_Adr = LOA_ul_SearchAddress(BIG_PosFile(pst_DragDrop->ul_FatFile));
			if(ul_Adr && ((int) ul_Adr != -1))
			{
				SetListTracks( (EVE_tdst_ListTracks *) ul_Adr );
			}
		}
        */
		break;

SelGizmo:
	case EDI_MESSAGE_ADDSELDATA:
		if(!mpst_ListTracks) return FALSE;
		p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
		if(!p2) return FALSE;
		if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
		{
			pst_GAO = (OBJ_tdst_GameObject *) _ul_Param2;
			if(ANI_b_IsGizmoAnim(pst_GAO, &pst_Father) && mpst_GAO == pst_Father)
			{
				if(_ul_Msg == EDI_MESSAGE_SELDATA) UnselectAll();
				if(!ANI_b_GizmoHasTrack(pst_GAO)) break;
				pst_Track = EVE_pst_GizmoToTrack(mpst_ListTracks, pst_GAO);
				if(pst_Track)
				{
					/*~~~~~~~~~~~~~*/
					USHORT	uw_Gizmo;
					/*~~~~~~~~~~~~~*/

					uw_Gizmo = pst_Track->uw_Gizmo;

					po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
					po_Track->Select();

					/*
					 * In classical Animation cases, the rotation track follows the Translation one.
					 * If we select one bone, both tracks must be selected. We check that the
					 * following track deals with the same gizmo.
					 */
					pst_Track++;

					if
					(
						(pst_Track - mpst_ListTracks->pst_AllTracks <= mpst_ListTracks->uw_NumTracks)
					&&	(pst_Track->uw_Gizmo == uw_Gizmo)
					)
					{
						po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
						po_Track->Select();
					}
				}

				SelectCurrentEvents();
			}
		}
		break;

	case EDI_MESSAGE_DELSELDATA:
		if(!mpst_ListTracks) break;
		p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Param2);
		if(!p2) return FALSE;
		if(p2->i_Type == LINK_C_ENG_GameObjectOriented)
		{
			pst_GAO = (OBJ_tdst_GameObject *) _ul_Param2;
			if(ANI_b_IsGizmoAnim(pst_GAO, &pst_Father))
			{
				pst_Track = EVE_pst_GizmoToTrack(mpst_ListTracks, pst_GAO);
				if(pst_Track)
				{
					/*~~~~~~~~~~~~~*/
					USHORT	uw_Gizmo;
					/*~~~~~~~~~~~~~*/

					uw_Gizmo = pst_Track->uw_Gizmo;

					po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
					po_Track->Unselect();
					po_Track->UnselectAll();
					/*
					 * In classical Animation cases, the rotation track follows the Translation one.
					 * If we select one bone, both tracks must be selected. We check that the
					 * following track deals with the same gizmo.
					 */
					pst_Track++;

					if
					(
						(pst_Track - mpst_ListTracks->pst_AllTracks <= mpst_ListTracks->uw_NumTracks)
					&&	(pst_Track->uw_Gizmo == uw_Gizmo)
					)
					{
						po_Track = mo_ListTracks.GetAt(mo_ListTracks.FindIndex(pst_Track - mpst_ListTracks->pst_AllTracks));
						po_Track->Unselect();
						po_Track->UnselectAll();
					}
				}
			}
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_HASBEENLINKED:
	case EDI_MESSAGE_HASBEENUNLINKED:
		SetLinkedOutputEditors();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EEVE_MESSAGE_RECOMPUTE:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		Recompute((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE);
		break;
	case EEVE_MESSAGE_ADDDELCURVEROT:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		DisplayHideCurveForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		break;
	case EEVE_MESSAGE_ADDDELCURVETRA:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		DisplayHideCurveForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE);
		break;
	case EEVE_MESSAGE_SPLITKEY:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		LINK_gb_AllRefreshEnable = FALSE;
		SplitEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE, FALSE);
		LINK_gb_AllRefreshEnable = TRUE;
		LINK_Refresh();
		LINK_gb_AllRefreshEnable = FALSE;
		SplitEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE, FALSE);
		SelectCurrentEvents(TRUE);
		LINK_gb_AllRefreshEnable = TRUE;
		LINK_Refresh();
		break;
	case EEVE_MESSAGE_SPLITKEYROT:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		SplitEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		SelectCurrentEvents();
		break;
	case EEVE_MESSAGE_SPLITKEYTRA:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		SplitEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE);
		SelectCurrentEvents();
		break;
	case EEVE_MESSAGE_DELKEY:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		DelEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		DelEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE);
		break;
	case EEVE_MESSAGE_DELKEYROT:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		DelEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		break;
	case EEVE_MESSAGE_DELKEYTRA:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		DelEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE);
		break;
	case EEVE_MESSAGE_SHOWTHISCURVEROT:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		DelAllCurves();
		DisplayHideCurveForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		break;
	case EEVE_MESSAGE_MOVEKEYLEFTROT:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		MoveKeyLeftForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		MoveReal(-1);
		break;
	case EEVE_MESSAGE_MOVEKEYRIGHTROT:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		MoveKeyRightForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		MoveReal(1);
		break;
	case EEVE_MESSAGE_MOVEKEYLEFTTRA:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		MoveKeyLeftForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE);
		MoveReal(-1);
		break;
	case EEVE_MESSAGE_MOVEKEYRIGHTTRA:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		MoveKeyRightForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE);
		MoveReal(1);
		break;
	case EEVE_MESSAGE_MOVEKEYLEFT:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		MoveKeyLeftForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		MoveKeyLeftForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE);
		MoveReal(-1);
		break;
	case EEVE_MESSAGE_MOVEKEYRIGHT:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		MoveKeyRightForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		MoveKeyRightForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE);
		MoveReal(1);
		break;
	case EEVE_MESSAGE_RECOMPUTEHIE:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		Recompute((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE);
		break;
	case EEVE_MESSAGE_SELECTALLBONES:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SelectAllBones((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2);
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_MESSAGE_TBGENERAL:
		mpo_MyView->IWantToBeActive(this);
		GetCursorPos(&point);
		M_MF()->InitPopupMenuAction(this, &o_Menu);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_ADDFRAMEALL);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_ADDXFRAMEALL);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_DUPLICATEFRAMEALL);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_DELKEYFRAME);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_ADDKEY);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_DELKEY);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "");
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_SHOWALLROTCURVE);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_DELALLCURVE);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "");
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_EXPAND2);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_EXPAND3);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "");
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_UNDO);
		M_MF()->TrackPopupMenuAction(this, point, &o_Menu);
		break;
	case EEVE_MESSAGE_TBPLAY:
		mpo_MyView->IWantToBeActive(this);
		GetCursorPos(&point);
		M_MF()->InitPopupMenuAction(this, &o_Menu);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_PLAY);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_PLAY2CYCLES);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_PLAYCONTRACT);
		M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "");
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_PLAY60);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_PLAY30);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_PLAY25);
		M_MF()->AddPopupMenuAction(this, &o_Menu, EEVE_ACTION_PLAY15);
		M_MF()->TrackPopupMenuAction(this, point, &o_Menu);
		break;
	case EEVE_MESSAGE_SETFRAME:
		UnselectAll(FALSE);
	case EEVE_MESSAGE_ADDSETFRAME:
		mpo_MyView->IWantToBeActive(this);
		AfterEngine();
		if((OBJ_tdst_GameObject *) _ul_Param1 != mpst_GAO) break;
		MoveRunningBox(NULL, *(float *) &_ul_Param2, TRUE);
		SelectCurrentEvents();
		break;
	case EEVE_MESSAGE_ADDSETFRAME2:
		mpo_MyView->IWantToBeActive(this);
		AfterEngine();
		if((OBJ_tdst_GameObject *) _ul_Param1 != mpst_GAO) break;
		MoveRunningBox(NULL, *(float *) &_ul_Param2, TRUE);
		SelectCurrentEvents(TRUE);
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_MESSAGE_IK:
		mpo_MyView->IWantToBeActive(this);
		AfterEngine();
		if((OBJ_tdst_GameObject *) _ul_Param1 != mpst_GAO) break;
		SaveForUndo();
		AddRemoveIK((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2);
		break;

	case EEVE_MESSAGE_SWITCHTRANS:
		AfterEngine();
		if((OBJ_tdst_GameObject *) _ul_Param1 != mpst_GAO) break;
		SaveForUndo();
		SwitchTrans((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2);
		break;

	/*$2--------------------------------------------------------------------------------------------------------------*/

	case EEVE_MESSAGE_COPY:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		Copy();
		break;
	case EEVE_MESSAGE_PASTEABSOLUTE:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		PasteFirstSelAbsolute();
		break;

	case EEVE_MESSAGE_PASTE:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();

		LINK_gb_AllRefreshEnable = FALSE;
		SplitEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, TRUE, FALSE);
		SplitEventForGAO((OBJ_tdst_GameObject *) _ul_Param1, (OBJ_tdst_GameObject *) _ul_Param2, FALSE, FALSE);
		SelectCurrentEvents(TRUE);
		LINK_gb_AllRefreshEnable = TRUE;

		PasteFirstSel(TRUE, FALSE);

		/* Count max track length */
		max = 0;
		for(i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
		{
			count = 0;
			for(int j = 0; j < mpst_ListTracks->pst_AllTracks[i].uw_NumEvents; j++)
			{
				count += mpst_ListTracks->pst_AllTracks[i].pst_AllEvents[j].uw_NumFrames;
			}

			if(count > max) max = count;
		}

		/* Update all tracks to be align */
		for(i = 0; i < mpst_ListTracks->uw_NumTracks; i++)
		{
			count = 0;
			for(j = 0; j < mpst_ListTracks->pst_AllTracks[i].uw_NumEvents; j++)
			{
				count += mpst_ListTracks->pst_AllTracks[i].pst_AllEvents[j].uw_NumFrames;
			}

			mpst_ListTracks->pst_AllTracks[i].pst_AllEvents[mpst_ListTracks->pst_AllTracks[i].uw_NumEvents - 2].
					uw_NumFrames += max -
				count;
		}

		SelectCurrentEvents(TRUE);
		if(!mst_Ini.b_AnimMode) ForceRefresh();
		break;

	case EEVE_MESSAGE_PASTESYMETRYREF:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		PasteFirstSel(FALSE, TRUE);
		break;
	case EEVE_MESSAGE_PASTESYMETRYINSERT:
		if(!mpst_GAO) return TRUE;
		mpo_MyView->IWantToBeActive(this);
		SaveForUndo();
		PasteFirstSel(TRUE, TRUE);
		break;

	case EEVE_MESSAGE_FORCEPLAY0:
		if(!mpst_GAO) return TRUE;

#ifdef ACTIVE_EDITORS
		if(L_setjmp(AI_gst_ContextCheck)) 
		{
			LINK_gb_AllRefreshEnable = TRUE;
			EVE_gb_ForceNotDone = FALSE;
			M_MF()->UnlockDisplay(this);
			break;
		}
#endif

		M_MF()->LockDisplay(this);
		LINK_gb_AllRefreshEnable = FALSE;
		EVE_gb_ForceNotDone = TRUE;
		MoveRunningBox(NULL, -0.01f);
		LINK_gb_AllRefreshEnable = TRUE;
		MoveRunningBox(NULL, 0.01f);
		EVE_gb_ForceNotDone = FALSE;
		M_MF()->UnlockDisplay(this);
		break;
	}

	return TRUE;
}

#endif /* ACTIVE_EDITORS */
