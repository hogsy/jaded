/*$T OUTframe_msg.cpp GC! 1.086 07/06/00 17:54:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImainframe.h"
#include "OUTframe.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"
#include "EDImsg.h"
#include "EDIapp.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "OUTmsg.h"
#include "LINKs/LINKstruct.h"
#include "TEXture/TEXmemory.h"
#include "DIAlogs/DIAselection_dlg.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAshape_dlg.h"
#include "DIAlogs/DIAanim_dlg.h"
#include "ENGvars.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"

#ifdef JADEFUSION
#include "DIAlogs/DIAcheckworld_dlg.h"
#include "DIAlogs/DIALightrej_dlg.h"
#endif

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeGDInterface.h"
#endif

#ifdef JADEFUSION
extern ULONG	EDI_OUT_gl_ForceSetMode;
#else
extern "C"
{
extern ULONG	EDI_OUT_gl_ForceSetMode;
};
#endif

extern void EDI_Tooltip_DisplayMessage(char *, ULONG ulSpeed = 200);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EOUT_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_tdst_DragDrop	*pst_DragDrop;
	int					i_Msg;
	EDI_cl_BaseFrame	*po_Editor1, *po_Editor2;
	int					p;
	BIG_INDEX			ul_Index, ul_File;
	BOOL				b_Exist;
	POINT				pt;
	ULONG				ul_Res;
    BOOL                b_DragInToolBox;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Msg)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_DATAHASCHANGED:
		if(DW() == NULL) break;

#if defined(_XENON_RENDER)
        // data has changed, must flush "cached" data
        Xe_InvalidateRenderLists();
#endif

		if(LINK_gx_PointersJustDeleted.Lookup(DW(), (void * &) p))
		{
			DW() = NULL;
			OnSetMode();
			RefreshMenu();
			mul_CurrentWorld = BIG_C_InvalidIndex;
			if(DP()->mb_AnimOn) DP()->Selection_Anim();
		}

		/* Update selection dialog */
		if(mpo_EngineFrame && mpo_EngineFrame->mpo_DisplayView && mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog)
			mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->UpdateList();
		if(mpo_EngineFrame && mpo_EngineFrame->mpo_DisplayView && mpo_EngineFrame->mpo_DisplayView->mpo_ToolBoxDialog)
			mpo_EngineFrame->mpo_DisplayView->mpo_ToolBoxDialog->Reset();
		if(mpo_EngineFrame && mpo_EngineFrame->mpo_DisplayView && mpo_EngineFrame->mpo_DisplayView->mpo_AnimDialog)
			mpo_EngineFrame->mpo_DisplayView->mpo_AnimDialog->Reset();

		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_FATHASCHANGED:
		if(mul_CurrentWorld == BIG_C_InvalidIndex) return 0;

		/* Have we changed the file ? */
		if(EDI_go_FHCFile.Lookup(mul_CurrentWorld, i_Msg))
		{
			switch(i_Msg)
			{
			case EDI_FHC_Delete:
				CloseWorld();
				return TRUE;

			case EDI_FHC_Rename:
			case EDI_FHC_Move:
				return TRUE;

			case EDI_FHC_AddUpdate:
				if(!mb_LockUpdate)
				{
					ul_Index = mul_LoadedWorld;
					CloseWorld();
					ChangeWorld(ul_Index);
				}

				mb_LockUpdate = FALSE;
				return TRUE;
			}
		}

		/* Have to check object and materials changes */
		{
			/*~~~~~~~~~~~~~~~~~~~*/
			POSITION	pos;
			BIG_INDEX	ul_Ref;
			ULONG		ul_Address;
			BOOL		b_SetMode;
			/*~~~~~~~~~~~~~~~~~~~*/

			b_SetMode = FALSE;

			pos = EDI_go_FHCFile.GetStartPosition();
			while(pos)
			{
				EDI_go_FHCFile.GetNextAssoc(pos, ul_Ref, i_Msg);

				if
				(
					    BIG_b_IsFileExtension( ul_Ref, ".tga" ) ||
					    BIG_b_IsFileExtension( ul_Ref, ".bmp" ) ||
					    BIG_b_IsFileExtension( ul_Ref, ".jpg" ) ||
					    BIG_b_IsFileExtension( ul_Ref, ".png" ) ||
					    BIG_b_IsFileExtension( ul_Ref, ".psd" ) ||
					    BIG_b_IsFileExtension( ul_Ref, ".gif" )
				)
				{
					if(TEX_l_Memory_DeleteTexture(BIG_FileKey(ul_Ref))) b_SetMode = TRUE;
					continue;
				}

				ul_Address = LOA_ul_SearchAddress(BIG_PosFile(ul_Ref));
				if(ul_Address != -1)
				{
					if(BIG_b_IsFileExtension(ul_Ref, EDI_Csz_ExtGameObject))
						DP()->OneGameObjectChanged(ul_Ref, ul_Address, i_Msg);
					else if(BIG_b_IsFileExtension(ul_Ref, EDI_Csz_ExtGraphicObject))
						DP()->OneGraphicObjectChanged(ul_Ref, ul_Address, i_Msg);
					else if(BIG_b_IsFileExtension(ul_Ref, EDI_Csz_ExtGraphicLight))
						DP()->OneGraphicLightChanged(ul_Ref, ul_Address, i_Msg);
					else if(BIG_b_IsFileExtension(ul_Ref, EDI_Csz_ExtGraphicMaterial))
						DP()->OneGraphicMaterialChanged(ul_Ref, ul_Address, i_Msg);
				}
			}

			if(b_SetMode) OnSetMode();
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_REFRESHMENU:
		RefreshMenu();
		return TRUE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_REFRESH:
		if(EDI_OUT_gl_ForceSetMode) OnSetMode();
		if(!ENG_gb_EngineRunning || (GetAsyncKeyState(VK_SPACE) < 0)) mpo_EngineFrame->mpo_DisplayView->Refresh();
		if(mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog)
			mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->UpdateList(FALSE);
		if(mpo_EngineFrame->mpo_DisplayView->mpo_AnimDialog)
			mpo_EngineFrame->mpo_DisplayView->mpo_AnimDialog->Invalidate();
		if(mpo_EngineFrame->mpo_DisplayView->mpo_ToolBoxDialog)
			mpo_EngineFrame->mpo_DisplayView->mpo_ToolBoxDialog->OneTrameEnding();
		return TRUE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANDRAGDROP:
		GetCursorPos(&pt);

		/* est ce que la souris est dans la boite de sélection ? */
		if(DP()->mb_SelectOn && DP()->mpo_SelectionDialog)
		{
			ul_Res = DP()->mpo_SelectionDialog->ul_GAOFromPt(pt);
			if(ul_Res && ul_Res != 1) EDI_Tooltip_DisplayMessage(((OBJ_tdst_GameObject *) ul_Res)->sz_Name, 250);
		}

        /* est ce que la souris est dans la toolbox */
        b_DragInToolBox = FALSE;
        if (DP()->mpo_ToolBoxDialog && DP()->mb_ToolBoxOn)
        {
            CRect o_Rect;
            DP()->mpo_ToolBoxDialog->GetWindowRect( (LPRECT) &o_Rect );
            if ( o_Rect.PtInRect( pt ) )
                b_DragInToolBox = TRUE;
        }

        pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		if(pst_DragDrop->i_Type == EDI_DD_Long) return FALSE;
		if(pst_DragDrop->i_Type == EDI_DD_Zone) return TRUE;
		if(pst_DragDrop->i_Type == EDI_DD_File)
		{
			_ul_Param2 = pst_DragDrop->ul_FatFile;
			if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
		}
		else
		{
			if(pst_DragDrop->i_Type == EDI_DD_Data)
			{
				_ul_Param2 = LOA_ul_SearchKeyWithAddress((ULONG) pst_DragDrop->i_Param2);
				if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
				_ul_Param2 = BIG_ul_SearchKeyToFat(_ul_Param2);
				if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
                /* si on n'est pas dans la toolbox */
                if (!b_DragInToolBox)
                {
				    if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtAIEngineModel)) return DW() != NULL;
				    if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtAIEditorModel)) return DW() != NULL;
				    if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtAIEngineInstance)) return DW() != NULL;
				    if(GetAsyncKeyState(VK_CONTROL) >= 0) return FALSE;
                }
			}
		}

        /* si on est dans la toolbox */
        if (b_DragInToolBox)
        {
            if (BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicObject) || (BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGameObject) && (pst_DragDrop->i_Type == EDI_DD_Data)) )
                return DP()->mpo_ToolBoxDialog->b_CanDrop( pt );
            return FALSE;
        }

		/* Graphic object */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicObject)) return DW() != NULL;

		/* Graphic light */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicLight)) return DW() != NULL;

		/* Material */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicMaterial)) return DW() != NULL;

		/* Game object */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGameObject)) return DW() != NULL;

		/* AImodel (editor) */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtAIEditorModel)) return DW() != NULL;

		/* AImodel (engine) */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtAIEngineModel)) return DW() != NULL;

		/* Shape */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtShape)) return DW() != NULL;

		/* Tracklist */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtEventAllsTracks)) return DW() != NULL;

		/* OldSkin */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSkin)) return DW() != NULL;

		/* Action kit */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtActionKit)) return DW() != NULL;

		/* Object model */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtObjModels)) return DW() != NULL;

		/* ColSets */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtCOLSetModel)) return DW() != NULL;

		/* Collision Objects */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtCOLObject)) return DW() != NULL;

		/* Game Material */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtCOLGMAT)) return DW() != NULL;

		/* Groups */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtObjGroups)) return DW() != NULL;

		/* Sound files */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundMetaBank)) return DW() != NULL;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundBank)) return DW() != NULL;

        /* morph data */
        if(BIG_b_IsFileExtension(_ul_Param2, ".mor")) return DW() != NULL;
        
        if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGameObjectRLI)) return DW() != NULL;
        
    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	case EDI_MESSAGE_CANSELFILE:
		if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtWorld)) return TRUE;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtWorldList)) return TRUE;
		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANSELDATA:
#ifdef JADEFUSION
		if(EDI_MTL_FEATURE_IS_ACTIVE(EDI_MTL_CheckWorldDialog))
		{
			//if we have a CheckWorld Dialog already open, notify it
			if(DP()->mpo_CheckWorldDialog)
			{
				DP()->mpo_CheckWorldDialog->b_Change_Selection((BIG_INDEX) _ul_Param2);
				DP()->mpo_CheckWorldDialog->SetFocus();
			}
			break;
		}
		else
#endif
		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_CREATEWORLD:
		ChangeWorld(_ul_Param1, FALSE);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_SAVEWORLD:
		if(_ul_Param1 == (ULONG) DW()) mb_LockUpdate = TRUE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_DESTROYWORLD:
		if((mb_CanDestroy) && ((ULONG) DW() == _ul_Param1))
		{
			if(DW()) WOR_Universe_DeleteWorld(DW(), 1);
			DW() = NULL;
			OnSetMode();
			RefreshMenu();
			mul_CurrentWorld = BIG_C_InvalidIndex;
		}
		{
			/*~~~~~~~~~~~~~*/
			POSITION	pos;
			HWND		hwnd;
			EDIA_cl_BaseDialog *po_Dlg;
			/*~~~~~~~~~~~~~*/
			pos = APP_go_Modeless.GetHeadPosition();
			while(pos)
			{
				hwnd = APP_go_Modeless.GetNext(pos);
				if(APP_go_ModelessNoRef.Find(hwnd) == NULL)
				{
					po_Dlg = (EDIA_cl_BaseDialog *) CWnd::FromHandle(hwnd);
					po_Dlg->i_OnMessage(EOUT_MESSAGE_DESTROYWORLD, 0, 0);
				}
			}
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_HASBEENLINKED:
		po_Editor1 = (EDI_cl_BaseFrame *) _ul_Param1;
		po_Editor2 = (EDI_cl_BaseFrame *) _ul_Param2;
		if(po_Editor2 == this) po_Editor1->i_OnMessage(EOUT_MESSAGE_CREATEWORLD, mul_CurrentWorld, 0);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_HASBEENUNLINKED:
		po_Editor1 = (EDI_cl_BaseFrame *) _ul_Param1;
		po_Editor2 = (EDI_cl_BaseFrame *) _ul_Param2;
		if(po_Editor2 == this) po_Editor1->i_OnMessage(EOUT_MESSAGE_DESTROYWORLD, (ULONG) DW(), 0);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_ENDDRAGDROP:
		pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		_ul_Param2 = pst_DragDrop->ul_FatFile;

		if(pst_DragDrop->i_Type == EDI_DD_Data)
		{
			_ul_Param2 = LOA_ul_SearchKeyWithAddress((ULONG) pst_DragDrop->i_Param2);
			if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;

			ul_File = BIG_ul_SearchKeyToFat(_ul_Param2);
			if(ul_File != BIG_C_InvalidIndex && BIG_b_IsFileExtension(ul_File, EDI_Csz_ExtAIEngineInstance))
			{
				_ul_Param2 = LOA_ul_SearchKeyWithAddress((ULONG) (((AI_tdst_Instance *) pst_DragDrop->i_Param2)->pst_Model));
				if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
			}

			pst_DragDrop->ul_FatFile = _ul_Param2 = BIG_ul_SearchKeyToFat(_ul_Param2);
		}

        /* est ce que la souris est dans la toolbox */
        if (DP()->mpo_ToolBoxDialog && DP()->mb_ToolBoxOn)
        {
            CRect o_Rect;
            DP()->mpo_ToolBoxDialog->GetWindowRect( (LPRECT) &o_Rect );
            GetCursorPos(&pt);
            if ( o_Rect.PtInRect( pt ) )
            {
                if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicObject))
                    DP()->mpo_ToolBoxDialog->DropGro(pst_DragDrop->ul_FatFile);
                else if (BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGameObject))
                    DP()->mpo_ToolBoxDialog->DropGao(pst_DragDrop->ul_FatFile);
	            break;
            }
        }

		if(pst_DragDrop->i_Type == EDI_DD_Zone)
		{
			DP()->DropZone(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicObject))
		{
			DP()->GRO_Drop(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicLight))
		{
			DP()->LIGHT_Drop(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGraphicMaterial))
		{
			DP()->DropMaterial(pst_DragDrop);
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtGameObject))
		{
			if(LOA_ul_SearchAddress(BIG_PosFile(pst_DragDrop->ul_FatFile)) != -1)
				b_Exist = TRUE;
			else
				b_Exist = FALSE;
			DP()->DropGameObject(pst_DragDrop,TRUE);
			RefreshMenu();
			if(!b_Exist) OnSetMode();
			LINK_Refresh();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtAIEditorModel))
		{
			DP()->DropAIModel(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtAIEngineModel))
		{
			DP()->DropAIModel(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtShape))
		{
			DP()->DropShape(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtEventAllsTracks))
		{
			DP()->DropTrackList(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtActionKit))
		{
			DP()->DropActionKit(pst_DragDrop);
			RefreshMenu();
			OnSetMode();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtObjModels))
		{
			DP()->DropObjectModel(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtCOLSetModel))
		{
			DP()->DropColSet(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtCOLGMAT))
		{
			DP()->DropGameMaterial(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtCOLObject))
		{
			DP()->DropCob(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtObjGroups))
		{
			DP()->DropObjGroup(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundMetaBank))
		{
			DP()->DropSoundMetaBank(pst_DragDrop);
			RefreshMenu();
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundBank))
		{
			DP()->DropSoundBank(pst_DragDrop);
			RefreshMenu();
			break;
		}

        if (BIG_b_IsFileExtension( _ul_Param2, ".mor" ))
        {
            DP()->DropMorphData( pst_DragDrop );
            RefreshMenu();
            break;
        }
        
        if (BIG_b_IsFileExtension( _ul_Param2, EDI_Csz_ExtGameObjectRLI ))
        {
            DP()->DropRLIData( pst_DragDrop );
            RefreshMenu();
            break;
        }

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_SELFILE:
_Try_
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtWorld))
		{
			ChangeWorld(_ul_Param2);
			break;
		}

		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtWorldList))
		{
			ChangeWorld(_ul_Param2);
			break;
		}

_Catch_
_End_
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_CURVE_CHANGEINTERPERSECONDS:
		DP()->Helper_SetInterPerSeconds(*(float *) &_ul_Param1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_SUBOBJECT_PAINTSEL:
		DP()->Selection_SubObjectTreat(NULL);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_SUBOBJECT_FLIPNORMALS:
		DP()->Selection_SubObject_FlipNormals( TRUE, FALSE );
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_SUBOBJECT_CHANGEID:
		DP()->Selection_SubObject_ChangeId(_ul_Param1);
		break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_SUBOBJECT_EUPDATEVSEL:
		DP()->Selection_SubObject_SelForEdge();
		break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_SUBOBJECT_FUPDATEVSEL:
		DP()->Selection_SubObject_SelForFace();
		break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_SUBOBJECT_FSELEXT:
		DP()->Selection_SubObject_SelFaceExt();
		break;

    /*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_SUBOBJECT_FDETACH:
		DP()->Selection_SubObject_DetachFace();
		break;
		
	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_SUBOBJECT_PIVOTCENTER:
		DP()->Selection_SubObject_PivotCenter( (int) _ul_Param1, (MATH_tdst_Vector *) _ul_Param2 );
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EOUT_MESSAGE_ANIMADDCURVE:
		mpo_EngineFrame->mpo_DisplayView->Helper_AddCurve
			(
				((EOUT_tdst_CurveParam *) _ul_Param1)->pst_GAO,
				((EOUT_tdst_CurveParam *) _ul_Param1)->pst_Data,
				((EOUT_tdst_CurveParam *) _ul_Param1)->pst_Track
			);
		break;
	case EOUT_MESSAGE_ANIMDELCURVE:
		mpo_EngineFrame->mpo_DisplayView->Helper_DelCurve
			(
				((EOUT_tdst_CurveParam *) _ul_Param1)->pst_Data,
				((EOUT_tdst_CurveParam *) _ul_Param1)->pst_Track
			);
		break;
    case EOUT_MESSAGE_ANIMREPLACECURVE:
        mpo_EngineFrame->mpo_DisplayView->Helper_ReplaceCurve
            (
                ((EOUT_tdst_CurveParam *) _ul_Param1)->pst_GAO,
				((EOUT_tdst_CurveParam *) _ul_Param1)->pst_Data,
				((EOUT_tdst_CurveParam *) _ul_Param1)->pst_NewData
            );

	}

	return TRUE;
}

#endif /* ACTIVE_EDITORS */

