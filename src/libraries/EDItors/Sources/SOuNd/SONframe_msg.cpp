/*$T SONframe_msg.cpp GC 1.138 04/05/04 11:47:38 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS

/*$4
 ***********************************************************************************************************************
    headers
 ***********************************************************************************************************************
 */

#include "LINKs/LINKmsg.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "EDImainframe.h"

#include "SouND/Sources/SND.h"
#include "SouND/sources/SNDwave.h"

#include "SONmsg.h"
#include "SONframe.h"
#include "SONpane.h"
#include "SONview.h"
#include "SONview_Smd.h"
#include "SONview_group.h"
#include "SONutil.h"

#include "EDImsg.h"
#include "EDIpaths.h"

#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmerge.h"
#include "EDItors/Sources/BROwser/BROstrings.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "OUTput/OUTmsg.h"

#include "Res/Res.h"

#ifdef JADEFUSION
extern BOOL ENG_gb_EngineRunning;
extern BOOL SND_gb_EdiPause;
#else
extern "C" BOOL ENG_gb_EngineRunning;
extern "C" BOOL SND_gb_EdiPause;
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int ESON_cl_Frame::i_OnMessage(ULONG _ul_Msg, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_tdst_DragDrop	*pst_DragDrop;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Msg)
	{
	case ESON_MESSAGE_REINITENGINE:
		OnReinitEngine();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_DATAHASCHANGED:
		mpo_VarsView0->mpo_ListBox->Invalidate();
		mpo_VarsView1->mpo_ListBox->Invalidate();
		mpo_ViewSmd->Invalidate();
		mpo_ViewGroup->Invalidate();
		mpo_View->UpdateControls();
		RefreshDialogBar();
        SModifier_DisplayName();
		return FALSE;

	case EDI_MESSAGE_REFRESH:
		if(ENG_gb_EngineRunning)
		{
			mpo_VarsView0->mpo_ListBox->Invalidate();
			mpo_VarsView1->mpo_ListBox->Invalidate();
			mpo_ViewSmd->Invalidate();
			mpo_ViewGroup->Invalidate();
			mpo_View->UpdateControls();
			RefreshDialogBar();
		}
        SModifier_DisplayName();
		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_REFRESHMENU:
		RefreshMenu();
        SModifier_DisplayName();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANDRAGDROP:
		pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		if((pst_DragDrop->i_Type == EDI_DD_Long) || (pst_DragDrop->i_Type == EDI_DD_User)) return FALSE;

		if(pst_DragDrop->i_Type == EDI_DD_File)
		{
			_ul_Param2 = pst_DragDrop->ul_FatFile;
		}
		else if(pst_DragDrop->i_Type == EDI_DD_Data)
		{
			if
			(
				((ULONG) pst_DragDrop->i_Param2 >= (ULONG) SND_gst_Params.dst_Sound)
			&&	((ULONG) pst_DragDrop->i_Param2 < (ULONG) (SND_gst_Params.dst_Sound + SND_gst_Params.l_SoundNumber))
			)
			{
				_ul_Param2 = BIG_ul_SearchKeyToFat(((SND_tdst_OneSound *) pst_DragDrop->i_Param2)->ul_FileKey);
			}
			else
			{
				_ul_Param2 = LOA_ul_SearchKeyWithAddress((ULONG) pst_DragDrop->i_Param2);
				if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
				_ul_Param2 = BIG_ul_SearchKeyToFat(_ul_Param2);
			}
		}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANSELFILE:
		if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;

		/* wav */
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundDialog)) break;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundMusic)) break;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundAmbience)) break;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundFile)) break;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtLoadingSound)) break;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundBank)) break;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSModifier)) break;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundInsert)) break;
		if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundFade)) break;

		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_CANSELDATA:
		return FALSE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_MESSAGE_ENDDRAGDROP:
		pst_DragDrop = (EDI_tdst_DragDrop *) _ul_Param1;
		_ul_Param2 = pst_DragDrop->ul_FatFile;

		if(pst_DragDrop->i_Type == EDI_DD_Data)
		{
			if
			(
				((ULONG) pst_DragDrop->i_Param2 >= (ULONG) SND_gst_Params.dst_Sound)
			&&	((ULONG) pst_DragDrop->i_Param2 < (ULONG) (SND_gst_Params.dst_Sound + SND_gst_Params.l_SoundNumber))
			)
			{
				_ul_Param2 = BIG_ul_SearchKeyToFat(((SND_tdst_OneSound *) pst_DragDrop->i_Param2)->ul_FileKey);
			}
			else
			{
				_ul_Param2 = LOA_ul_SearchKeyWithAddress((ULONG) pst_DragDrop->i_Param2);
				if(_ul_Param2 == BIG_C_InvalidIndex) return FALSE;
				_ul_Param2 = BIG_ul_SearchKeyToFat(_ul_Param2);
			}
		}

	case EDI_MESSAGE_SELFILE:
		if
		(
			BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundMusic)
		||	BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundAmbience)
		||	BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundDialog)
		||	BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundFile)
		||	BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtLoadingSound)
		)
		{
			Sound_Set(_ul_Param2);
			break;
		}
		else if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundBank))
		{
			SoundBank_Set(_ul_Param2);
			break;
		}
		else if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSModifier))
		{
			SModifier_Set(_ul_Param2);
			break;
		}
		else if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundInsert))
		{
			Insert_OnOpen(_ul_Param2);
			break;
		}
		else if(BIG_b_IsFileExtension(_ul_Param2, EDI_Csz_ExtSoundFade))
		{
			Insert_OnOpen(_ul_Param2);
			break;
		}
		return TRUE;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_MESSAGE_REFRESH_SMODIFIER_VIEW:
		mpo_ViewSmd->UpdateSmdView();
        SModifier_DisplayName();
		break;

	case ESON_MESSAGE_REFRESH_SMODIFIER_TITLE:
		SModifier_DisplayName();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_MESSAGE_CREATEASSSMD:
		SModifier_CreateAss(_ul_Param1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_MESSAGE_GENERATE_MTX:
		OnCreateMTX(_ul_Param1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_MESSAGE_AUTOVOLUMEOFF:
		mpo_DialogBar->GetDlgItem(IDC0)->SetWindowText("AutoVolume Off");
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_MESSAGE_REFRESH_BANK:
		if(_ul_Param2) ESON_BankUpdate(_ul_Param1);
		if(_ul_Param1 == mst_SndBankDesc.ul_Fat) SoundBank_ForceReload();
		break;

	case EOUT_MESSAGE_DESTROYWORLD:
		OnWorldDestruction();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case ESON_MESSAGE_GAO_HAS_CHANGED:
		MdF_GaoHasChanged(_ul_Param1);
		break;

    case ESON_MESSAGE_UPDATERASTER:
        OnRefreshRaster();
        break;
	}

	return TRUE;
}
#endif /* ACTIVE_EDITORS */
