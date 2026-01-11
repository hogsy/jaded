/*$T TEXTframe_act.cpp GC 1.138 03/26/04 11:53:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "TEXTframe.h"
#include "TEXTframe_act.h"
#include "TEXTscroll.h"
#include "EDImainframe.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGfat.h"
#include "EDIpaths.h"
#include "DIAlogs/DIAname_dlg.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ETEXT_cl_Frame::OnAction(ULONG _ul_Action)
{
	switch(_ul_Action)
	{
	case ETEXT_ACTION_UNICODE:					mb_Unicode = mb_Unicode ? FALSE : TRUE; break;
	case ETEXT_ACTION_UPDATECHARTABLE:			CharTable_Import(6); break;
	/**/
	case ETEXT_ACTION_BANK_OPEN:				OnBankOpen(); break;
	case ETEXT_ACTION_BANK_NEW:					OnBankNew(); break;
	case ETEXT_ACTION_BANK_CLOSE:				OnBankClose(); break;
	case ETEXT_ACTION_BANK_NEWNOLANG:			OnBankNoLangNew(); break;
	/**/
	case ETEXT_ACTION_FILE_NEW:					OnFileNew(); break;
	case ETEXT_ACTION_FILE_DEL:					OnFileDelete(); break;
	case ETEXT_ACTION_FILE_SAVE:				mb_FileIsModified = TRUE; OnFileSave(); break;
    case ETEXT_ACTION_FILE_AUTOSNDFILE:			OnFileGetSndFileAuto();break;
    case ETEXT_ACTION_FILE_AUTOPREFIX:			OnFileGetPrefixAuto(FALSE);break;
    case ETEXT_ACTION_FILE_AUTOPREFIXSEL:		OnFileGetPrefixAuto(TRUE);break;
	/**/
	case ETEXT_ACTION_CLOSEALL:					OnCloseAll(); break;
	case ETEXT_ACTION_SYNCALL:					OnSynchronizeAll(); break;
	case ETEXT_ACTION_SYNC:						OnSynchronize(); break;
	case ETEXT_ACTION_FIND:						OnFind(); break;
	case ETEXT_ACTION_FINDSALECHAR:				OnFindSaleChar(); break;
	case ETEXT_ACTION_REPLACESALECHAR:			OnReplaceSaleChar(); break;
	case ETEXT_ACTION_NEXTFIND:					OnFindNext( 1 ); break;
	case ETEXT_ACTION_FINDPREV:					OnFindNext( 0 ); break;
	/**/
	case ETEXT_ACTION_ENTRY_NEW:				OnEntryNew( FALSE ); break;
	case ETEXT_ACTION_ENTRY_NEWMULTI:			OnEntryNew( TRUE ); break;
	case ETEXT_ACTION_ENTRY_DEL:				OnEntryDelete(); break;
	case ETEXT_ACTION_ENTRY_COPY:				OnEntryCopy(); break;
	case ETEXT_ACTION_ENTRY_CUT:				OnEntryCut(); break;
	case ETEXT_ACTION_ENTRY_PASTE:				OnEntryPaste(); break;
	/**/
	case ETEXT_ACTION_OPTION_STANDARD_LANG:		mst_Ini.i_OptionFlags ^= ETEXT_Cte_UseStandardLang; break;
	case ETEXT_ACTION_OPTION_SYNC_AUTO:			mst_Ini.i_OptionFlags ^= ETEXT_Cte_SyncAuto; break;
	case ETEXT_ACTION_OPTION_KEEP_EMPTY_TXT:	mst_Ini.i_OptionFlags ^= ETEXT_Cte_KeepEmptyTxt; break;
	case ETEXT_ACTION_OPTION_KEEP_ALL_TXT:		mst_Ini.i_OptionFlags ^= ETEXT_Cte_KeepAllTxt; break;
	/**/
	case ETEXT_ACTION_EXPORT_FILE:				OnExportFile(); break;
	case ETEXT_ACTION_EXPORT_ALL:				OnExportAll(); break;
	case ETEXT_ACTION_IMPORT:					OnImport(); break;
	case ETEXT_ACTION_EXPORTNOFORMAT:			mb_ExportNoFormatCode = !mb_ExportNoFormatCode; break;
    /**/
    case ETEXT_ACTION_CHECKANDFIX:              OnCheckAndFixAll();break;
	case ETEXT_ACTION_REPORT_SOUND_FILE:		OnReportSoundFile();break;
	case ETEXT_ACTION_TOOL_DISP_LOADED:			OnDisplayAllFilesLoaded();break;
	case ETEXT_ACTION_TOOL_REPORT_NOSOUND:		OnReportNoSound();break;
	default:									break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Frame::b_OnActionValidate(ULONG _ul_Action, BOOL)
{
	switch(_ul_Action)
	{
	case ETEXT_ACTION_BANK_CLOSE:	if ( ul_GetCurBank() == BIG_C_InvalidIndex) return FALSE;
									if ( mpo_Scroll ) return mpo_Scroll->b_CanCloseWithKey();
									return TRUE;
	/**/
	case ETEXT_ACTION_FILE_NEW:			return(ul_GetCurBank() == BIG_C_InvalidIndex) ? FALSE : TRUE;
	case ETEXT_ACTION_FILE_DEL:			return(ul_GetCurFile() == BIG_C_InvalidIndex) ? FALSE : TRUE;
    case ETEXT_ACTION_FILE_AUTOSNDFILE:	return(ul_GetCurFile() == BIG_C_InvalidIndex) ? FALSE : TRUE;
    case ETEXT_ACTION_FILE_AUTOPREFIXSEL:
	case ETEXT_ACTION_FILE_AUTOPREFIX:	return(ul_GetCurFile() == BIG_C_InvalidIndex) ? FALSE : TRUE;
	case ETEXT_ACTION_FILE_SAVE:		return mb_FileIsModified;
	/**/
	case ETEXT_ACTION_CLOSEALL:			return(mpo_ListBank->GetCount()) ? TRUE : FALSE;
	case ETEXT_ACTION_SYNC:				return(ul_GetCurFile() == BIG_C_InvalidIndex) ? FALSE : TRUE;
	case ETEXT_ACTION_FIND:				return(ul_GetCurFile() == BIG_C_InvalidIndex) ? FALSE : TRUE;
	case ETEXT_ACTION_FINDSALECHAR:		return(ul_GetCurFile() == BIG_C_InvalidIndex) ? FALSE : TRUE;
	case ETEXT_ACTION_REPLACESALECHAR:	return(ul_GetCurFile() == BIG_C_InvalidIndex) ? FALSE : TRUE;
	case ETEXT_ACTION_NEXTFIND:			return mb_FindSomething;
	case ETEXT_ACTION_FINDPREV:			return mb_FindSomething;
	/**/
	case ETEXT_ACTION_ENTRY_NEW:	return(ul_GetCurFile() == BIG_C_InvalidIndex) ? FALSE : TRUE;
	case ETEXT_ACTION_ENTRY_DEL:	return (i_GetSelNb() ? TRUE : FALSE);
	/**/
	case ETEXT_ACTION_EXPORT_FILE:	return(ul_GetCurFile() == BIG_C_InvalidIndex) ? FALSE : TRUE;
	default:						return TRUE;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT ETEXT_cl_Frame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~*/
	UINT	ui_State;
	/*~~~~~~~~~~~~~*/

	switch(_ul_Action)
	{
	case ETEXT_ACTION_UNICODE:        ui_State = DFCS_BUTTONCHECK | (mb_Unicode ? DFCS_CHECKED : 0);	break;
	
	case ETEXT_ACTION_OPTION_STANDARD_LANG:
		ui_State = DFCS_BUTTONCHECK | ((mst_Ini.i_OptionFlags & ETEXT_Cte_UseStandardLang) ? DFCS_CHECKED : 0);
		break;

	case ETEXT_ACTION_OPTION_SYNC_AUTO:
		ui_State = (mst_Ini.i_OptionFlags & ETEXT_Cte_SyncAuto) ? DFCS_BUTTONCHECK | DFCS_CHECKED : DFCS_BUTTONCHECK;
		break;

	case ETEXT_ACTION_OPTION_KEEP_ALL_TXT:
		ui_State = (mst_Ini.i_OptionFlags & ETEXT_Cte_KeepAllTxt) ? DFCS_BUTTONCHECK | DFCS_CHECKED : DFCS_BUTTONCHECK;
		break;

	case ETEXT_ACTION_OPTION_KEEP_EMPTY_TXT:
		ui_State = (mst_Ini.i_OptionFlags & ETEXT_Cte_KeepEmptyTxt) ? DFCS_BUTTONCHECK | DFCS_CHECKED : DFCS_BUTTONCHECK;
		break;

	case ETEXT_ACTION_EXPORTNOFORMAT:
		ui_State = (mb_ExportNoFormatCode) ? DFCS_BUTTONCHECK | DFCS_CHECKED : DFCS_BUTTONCHECK;
		break;

	default:
		ui_State = (UINT) - 1;
		break;
	}

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL ETEXT_cl_Frame::b_KnowsKey(USHORT _uw_Key)
{
	if ( /*(GetFocus() == mpo_Edit) || */ (mpo_Scroll && mpo_Scroll->mb_EditContent) )
		return M_MF()->b_EditKey(_uw_Key);
	else
		return FALSE;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */

#endif
